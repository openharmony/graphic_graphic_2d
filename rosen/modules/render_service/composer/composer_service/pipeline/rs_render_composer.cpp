/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "rs_render_composer.h"

#include <memory>
#include <unistd.h>

#include "common/rs_exception_check.h"
#include "common/rs_optional_trace.h"
#include "common/rs_singleton.h"
#ifdef RS_ENABLE_EGLIMAGE
#include "feature/gpuComposition/rs_egl_image_manager.h"
#endif // RS_ENABLE_EGLIMAGE
#include "feature/hdr/rs_hdr_util.h"
#include "feature/lpp/render_process/lpp_video_handler.h"
#include "feature/round_corner_display/rs_rcd_render_manager.h"
#include "feature/round_corner_display/rs_round_corner_display_manager.h"
#ifdef RS_ENABLE_TV_PQ_METADATA
#include "feature/tv_metadata/rs_tv_metadata_manager.h"
#endif
#include "frame_report.h"
#include "gfx/fps_info/rs_surface_fps_manager.h"
#include "gfx/first_frame_notifier/rs_first_frame_notifier.h"
#include "graphic_feature_param_manager.h"
#include "hgm_frame_rate_manager.h"
#include "hisysevent.h"

#ifdef USE_VIDEO_PROCESSING_ENGINE
#include "metadata_helper.h"
#endif
#include "parameters.h"
#include "platform/common/rs_hisysevent.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "platform/ohos/backend/rs_surface_ohos_gl.h"
#include "platform/ohos/backend/rs_surface_ohos_raster.h"
#include "pipeline/hardware_thread/rs_realtime_refresh_rate_manager.h"
#include "pipeline/render_thread/rs_base_render_util.h"
#include "pipeline/render_thread/rs_uni_render_engine.h"
#include "pipeline/render_thread/rs_uni_render_util.h"

#include "rs_frame_report.h"
#include "rs_trace.h"
#include "rs_layer_cmd_type.h"
#include "rs_render_surface_layer.h"
#include "rs_surface_layer.h"
#include "screen_manager/rs_screen_manager.h"
#ifdef RS_ENABLE_EGLIMAGE
#ifdef USE_M133_SKIA
#include "src/gpu/ganesh/gl/GrGLDefines.h"
#else
#include "src/gpu/gl/GrGLDefines.h"
#endif
#endif
#include "vsync_sampler.h"

#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_surface_ohos_vulkan.h"
#include "feature/gpuComposition/rs_vk_image_manager.h"
#endif

#ifdef RES_SCHED_ENABLE
#include "concurrent_task_client.h"
#include "system_ability_definition.h"
#include "if_system_ability_manager.h"
#include <iservice_registry.h>
#include "ressched_event_listener.h"
#endif

#undef LOG_TAG
#define LOG_TAG "RSRenderComposer"

namespace OHOS::Rosen {
namespace {
constexpr uint32_t HARD_JANK_TWO_TIME = 2;
constexpr int64_t REFRESH_PERIOD = 16667; // 16667us == 16.667ms
constexpr int64_t REPORT_LOAD_WARNING_INTERVAL_TIME = 5000000; // 5s == 5000000us
constexpr int64_t RESERVE_TIME = 1000000; // we reserve 1ms more for the composition
constexpr int64_t COMMIT_DELTA_TIME = 2; // 2ms
constexpr int64_t MAX_DELAY_TIME = 100; // 100ms
constexpr int64_t NS_MS_UNIT_CONVERSION = 1000000;
constexpr int64_t UNI_RENDER_VSYNC_OFFSET_DELAY_MODE = 3300000; // 3.3ms
constexpr uint32_t MAX_TOTAL_SURFACE_NAME_LENGTH = 320;
constexpr uint32_t MAX_SINGLE_SURFACE_NAME_LENGTH = 20;
// Threshold for abnormal time in the composer pipeline
constexpr int COMPOSER_TIMEOUT = 800;
// The number of exceptions in the composer pipeline required to achieve render_service reset
constexpr int COMPOSER_TIMEOUT_ABORT_CNT = 30;
// The number of exceptions in the composer pipeline required to report hisysevent
constexpr int COMPOSER_TIMEOUT_CNT = 15;
const std::string PROCESS_NAME_FOR_HISYSEVENT = "/system/bin/render_service";
const std::string COMPOSER_PIPELINE_TIMEOUT = "composer_pipeline_timeout";
}

static int64_t SystemTime()
{
    timespec t = {};
    clock_gettime(CLOCK_MONOTONIC, &t);
    return int64_t(t.tv_sec) * 1000000000LL + t.tv_nsec; // 1000000000ns == 1s
}

RSRenderComposer::RSRenderComposer(const std::shared_ptr<HdiOutput>& output, const sptr<RSScreenProperty>& property)
{
    CreateAndInitComposer(output, property);
}

void RSRenderComposer::CreateAndInitComposer(const std::shared_ptr<HdiOutput>& output,
    const sptr<RSScreenProperty>& property)
{
    hdiOutput_ = output;
    screenId_ = hdiOutput_->GetScreenId();
    RS_LOGI("%{public}s screenId:%{public}" PRIu64, __func__, screenId_);
    GraphicIRect damageRect {
        0, 0, static_cast<int32_t>(property->GetWidth()), static_cast<int32_t>(property->GetHeight())
    };
    std::vector<GraphicIRect> damageRects;
    damageRects.emplace_back(damageRect);
    hdiOutput_->SetOutputDamages(damageRects);

    std::string threadName = "CompThread_" + std::to_string(screenId_);
    runner_ = AppExecFwk::EventRunner::Create(threadName);
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    rsRenderComposerContext_ = std::make_shared<RSRenderComposerContext>();
    redrawCb_ = [this](const sptr<Surface>& surface, const std::vector<std::shared_ptr<RSLayer>>& layers) {
        return this->Redraw(surface, layers);
    };
    hgmHardwareUtils_ = std::make_shared<HgmHardwareUtils>();

    if (handler_) {
        ScheduleTask([this]() {
#if defined (RS_ENABLE_VK)
            // Change vk interface type from UNIRENDER into UNPROTECTED_REDRAW, this is necessary for hardware init.
            if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
                RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
                RsVulkanContext::GetSingleton().SetIsProtected(false);
            }
#endif
#ifdef RES_SCHED_ENABLE
            SubScribeSystemAbility();
#endif
            uniRenderEngine_ = std::make_shared<RSUniRenderEngine>();
            uniRenderEngine_->Init();
            // posttask for multithread safely release surface and image
            ContextRegisterPostTask();
            threadTid_ = gettid();
        }).wait();
    }
#ifdef RES_SCHED_ENABLE
    int qosRes = OHOS::ConcurrentTask::ConcurrentTaskClient::GetInstance().SetSystemQoS(threadTid_,
        OHOS::ConcurrentTask::SystemQoSLevel::SYSTEM_QOS_EIGHT);
    RS_LOGI("%{public}s SetSystemQoS qosRes = %{public}d", __func__, qosRes);
#endif
#ifdef RS_ENABLE_GPU
    // report composer tid to sched deadline
    RsFrameReport::ReportComposerInfo(screenId_, threadTid_);
#endif
    auto onPrepareCompleteFunc = [this](auto& surface, const auto& param, void* data) {
        OnPrepareComplete(surface, param, data);
    };
    hdiOutput_->RegPrepareComplete(onPrepareCompleteFunc, this);
}

void RSRenderComposer::PostTask(const std::function<void()>& task)
{
    if (handler_) {
        handler_->PostTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

void RSRenderComposer::PostSyncTask(const std::function<void()>& task)
{
    if (handler_) {
        handler_->PostSyncTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

void RSRenderComposer::PostDelayTask(const std::function<void()>& task, int64_t delayTime)
{
    if (handler_) {
        handler_->PostTask(task, delayTime, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

void RSRenderComposer::ClearRedrawGPUCompositionCache(const std::unordered_set<uint64_t>& bufferIds)
{
    std::weak_ptr<RSBaseRenderEngine> uniRenderEngine = uniRenderEngine_;
    if (auto engine = uniRenderEngine.lock()) {
        RS_LOGD("%{public}s, bufferId size %{public}d", __func__, static_cast<int32_t>(bufferIds.size()));
        engine->ClearCacheSet(bufferIds);
    }
}

void RSRenderComposer::RefreshRateCounts(std::string& dumpString)
{
    if (hgmHardwareUtils_ == nullptr) {
        RS_LOGW("%{public}s: hgmHardwareUtils_ is nullptr.", __func__);
        return;
    }
    hgmHardwareUtils_->RefreshRateCounts(dumpString);
}

void RSRenderComposer::ClearRefreshRateCounts(std::string& dumpString)
{
    if (hgmHardwareUtils_ == nullptr) {
        RS_LOGW("%{public}s: hgmHardwareUtils_ is nullptr.", __func__);
        return;
    }
    hgmHardwareUtils_->ClearRefreshRateCounts(dumpString);
}

void PrintHiperfSurfaceLog(const std::string& counterContext, uint64_t counter)
{
#ifdef HIPERF_TRACE_ENABLE
    RS_LOGW("hiperf_surface_%{public}s %{public}" PRIu64, counterContext.c_str(), counter);
#endif
}

void RSRenderComposer::ComposerPrepare(uint32_t& currentRate, int64_t& delayTime, const PipelineParam& pipelineParam)
{
    hgmHardwareUtils_->TransactRefreshRateParam(currentRate, pipelineParam.pendingScreenRefreshRate,
        pipelineParam.frameTimestamp, pipelineParam.pendingConstraintRelativeTime);
#ifdef RES_SCHED_ENABLE
    ResschedEventListener::GetInstance()->ReportFrameToRSS();
#endif
    unExecuteTaskNum_++;
    auto& hgmCore = OHOS::Rosen::HgmCore::Instance();
    delayTime = UpdateDelayTime(hgmCore, currentRate, pipelineParam);
    delayTime_ = delayTime;
}

void RSRenderComposer::ProcessComposerFrame(uint32_t currentRate, const PipelineParam& pipelineParam)
{
    RS_TRACE_NAME_FMT("ProcessComposerFrame screenId:%{public}" PRIu64, screenId_);
    std::string threadName = "CompThread_" + std::to_string(screenId_);
    RSTimer timer(threadName.c_str(), COMPOSER_TIMEOUT);
    auto layers = rsRenderComposerContext_->GetRSLayersVec();
    AddSolidColorLayer(layers);
    PrintHiperfSurfaceLog("counter3", static_cast<uint64_t>(layers.size()));
    int64_t startTime = GetCurTimeCount();
    std::string surfaceName = GetSurfaceNameInLayers(layers);
    RS_LOGD("CommitAndReleaseLayers task execute, %{public}s", surfaceName.c_str());
    RSFirstFrameNotifier::GetInstance().ExecIfFirstFrameCommit(screenId_);

    RS_LOGI_IF(DEBUG_COMPOSER, "CommitAndReleaseData hasGameScene is %{public}d %{public}s",
        pipelineParam.hasGameScene, surfaceName.c_str());

    int64_t startTimeNs = std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
    RS_TRACE_NAME_FMT("CommitLayers rate:%u,now:%" PRIu64 ",vsyncId:%" PRIu64 ",size:%zu,%s",
        currentRate, pipelineParam.frameTimestamp, pipelineParam.vsyncId, layers.size(),
        GetSurfaceNameInLayersForTrace(layers).c_str());
    RS_LOGD("CommitLayers rate:%{public}u, now:%{public}" PRIu64 ",vsyncId:%{public}" PRIu64 ", \
        size:%{public}zu, %{public}s", currentRate, pipelineParam.frameTimestamp, pipelineParam.vsyncId, layers.size(),
        GetSurfaceNameInLayersForTrace(layers).c_str());

    bool shouldDropFrame = IsDropDirtyFrame(layers);
    if (!shouldDropFrame) {
        hgmHardwareUtils_->SwitchRefreshRate(hdiOutput_);
    }

    if (RSSystemProperties::IsSuperFoldDisplay() && screenId_ == 0) {
        std::vector<std::shared_ptr<RSLayer>> reviseLayers = layers;
        ChangeLayersForActiveRectOutside(reviseLayers);
        if (!isHwcDead_) {
            hdiOutput_->SetRSLayers(reviseLayers);
        }
        lppLayerCollector_.AddLppLayerId(reviseLayers);
    } else {
        if (!isHwcDead_) {
            hdiOutput_->SetRSLayers(layers);
        }
        lppLayerCollector_.AddLppLayerId(layers);
    }
    bool doRepaint = hdiOutput_->IsDeviceValid() && !shouldDropFrame && !isHwcDead_;
    if (doRepaint) {
#ifdef RS_ENABLE_TV_PQ_METADATA
        RSTvMetadataManager::CombineMetadataForAllLayers(layers);
#endif
        hdiOutput_->Repaint();
        RecordTimestamp(pipelineParam.vsyncId, layers);
    }
    ReleaseLayerBuffersInfo releaseLayerInfo;
    releaseLayerInfo.screenId = screenId_;
    hdiOutput_->ReleaseLayers(releaseLayerInfo);
    int64_t endTimeNs = std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
    releaseLayerInfo.lastSwapBufferTime = endTimeNs - startTimeNs;
    if (composerToRenderConnection_ != nullptr) {
        composerToRenderConnection_->ReleaseLayerBuffers(releaseLayerInfo);
    }
    unExecuteTaskNum_--;

    if (rsVsyncManagerAgent_ != nullptr) {
        rsVsyncManagerAgent_->SetTaskEndWithTime(SystemTime() - lastActualTime_);
    }
    lastActualTime_ = pipelineParam.actualTimestamp;
    int64_t endTime = GetCurTimeCount();
    uint64_t frameTime = endTime - startTime;
    uint32_t missedFrames = frameTime / REFRESH_PERIOD;
    uint16_t frameRate = currentRate;
    if (missedFrames >= HARD_JANK_TWO_TIME &&
        endTime - intervalTimePoints_ > REPORT_LOAD_WARNING_INTERVAL_TIME) {
        RS_LOGI("CommitAndReleaseLayers report load event frameTime: %{public}" PRIu64
            " missedFrame: %{public}" PRIu32 " frameRate:%{public}" PRIu16 " %{public}s",
            frameTime, missedFrames, frameRate, surfaceName.c_str());
        intervalTimePoints_ = endTime;
        RS_TRACE_NAME("RSRenderComposer::CommitAndReleaseLayers HiSysEventWrite in RSRenderComposer");
        RSHiSysEvent::EventWrite(RSEventName::RS_HARDWARE_THREAD_LOAD_WARNING, RSEventType::RS_STATISTIC,
            "FRAME_RATE", frameRate, "MISSED_FRAMES", missedFrames, "FRAME_TIME", frameTime);
    }
    if (composerToRenderConnection_ != nullptr && pipelineParam.hasLppVideo) {
        composerToRenderConnection_->NotifyLppLayerToRender(pipelineParam.vsyncId, lppLayerCollector_.GetLppLayerId());
    }
    EndCheck(timer);
    if (isDisconnected_ && unExecuteTaskNum_ == 0) {
        RS_TRACE_NAME_FMT("Clear output, screenId : %" PRIu64, screenId_);
        ClearFrameBuffersInner();
        rsRenderComposerContext_ = nullptr;
        hdiOutput_ = nullptr;
    }
}

int64_t RSRenderComposer::UpdateDelayTime(HgmCore& hgmCore,
    uint32_t currentRate, const PipelineParam& pipelineParam)
{
    int64_t delayTime = 0;
    RS_LOGI_IF(DEBUG_COMPOSER, "CommitAndReleaseData hgmCore's LtpoEnabled is %{public}d", hgmCore.GetLtpoEnabled());
    int64_t currTime = SystemTime();
    if (IsDelayRequired(hgmCore, pipelineParam)) {
        delayTime = CalculateDelayTime(hgmCore, currentRate, currTime, pipelineParam);
    }

    // We need to ensure the order of composition frames, postTaskTime(n + 1) must > postTaskTime(n),
    // and we give a delta time more between two composition tasks.
    int64_t currCommitTime = currTime + delayTime * NS_MS_UNIT_CONVERSION;
    if (currCommitTime <= lastCommitTime_) {
        delayTime = delayTime +
            std::round((lastCommitTime_ - currCommitTime) * 1.0f / NS_MS_UNIT_CONVERSION) +
            COMMIT_DELTA_TIME;
        RS_LOGD("CommitAndReleaseLayers vsyncId: %{public}" PRIu64 ", " \
            "update delayTime: %{public}" PRId64 ", currCommitTime: %{public}" PRId64 ", " \
            "lastCommitTime: %{public}" PRId64, pipelineParam.vsyncId, delayTime, currCommitTime, lastCommitTime_);
        RS_TRACE_NAME_FMT("update delayTime: %" PRId64 ", currCommitTime: %" PRId64 ", lastCommitTime: %" PRId64 "",
            delayTime, currCommitTime, lastCommitTime_);
    }
    if (delayTime < 0 || delayTime >= MAX_DELAY_TIME) {
        delayTime = 0;
    }
    lastCommitTime_ = currTime + delayTime * NS_MS_UNIT_CONVERSION;
    return delayTime;
}

void RSRenderComposer::ComposerProcess(uint32_t currentRate, std::shared_ptr<RSLayerTransactionData> transactionData)
{
    if (hdiOutput_ == nullptr || rsRenderComposerContext_ == nullptr) {
        unExecuteTaskNum_--;
        RS_LOGE("%{public}s output or context is nullptr, screenId:%{public}" PRIu64, __func__, screenId_);
        return;
    }
    UpdateTransactionData(transactionData);
    ProcessComposerFrame(currentRate, transactionData->GetPipelineParam());
}

void RSRenderComposer::EndCheck(RSTimer timer)
{
    exceptionCheck_.pid_ = getpid();
    exceptionCheck_.uid_ = getuid();
    exceptionCheck_.processName_ = PROCESS_NAME_FOR_HISYSEVENT;
    exceptionCheck_.exceptionPoint_ = COMPOSER_PIPELINE_TIMEOUT;

    if (timer.GetDuration() >= COMPOSER_TIMEOUT) {
        if (++exceptionCnt_ == COMPOSER_TIMEOUT_CNT) {
            RS_LOGE("Composer Thread Exception Count[%{public}d]", exceptionCnt_);
            exceptionCheck_.exceptionCnt_ = exceptionCnt_;
            exceptionCheck_.exceptionMoment_ = timer.GetSeconds();
            exceptionCheck_.UploadRenderExceptionData();
        }
    } else {
        exceptionCnt_ = 0;
    }
    if (exceptionCnt_ == COMPOSER_TIMEOUT_ABORT_CNT) {
        exceptionCheck_.exceptionCnt_ = exceptionCnt_;
        exceptionCheck_.exceptionMoment_ = timer.GetSeconds();
        exceptionCheck_.UploadRenderExceptionData();
        RS_LOGE("%{public}s PID:%{public}d, UID:%{public}u, PROCESS_NAME:%{public}s, \
            EXCEPTION_CNT:%{public}d, EXCEPTION_TIME:%{public}" PRId64", EXCEPTION_POINT:%{public}s",
            __func__, getpid(), getuid(), PROCESS_NAME_FOR_HISYSEVENT.c_str(), exceptionCnt_,
            exceptionCheck_.exceptionMoment_, COMPOSER_PIPELINE_TIMEOUT.c_str());
    }
}

void RSRenderComposer::RecordTimestamp(uint64_t vsyncId, const std::vector<std::shared_ptr<RSLayer>>& layers)
{
    for (auto& layer : layers) {
        if (layer == nullptr) {
            continue;
        }
        uint64_t id = layer->GetNodeId();
        auto& surfaceFpsManager = RSSurfaceFpsManager::GetInstance();
        if (layer->GetBuffer() == nullptr) {
            continue;
        }
        if (layer->GetUniRenderFlag()) {
            surfaceFpsManager.RecordPresentFdForUniRender(vsyncId, hdiOutput_->GetCurrentFramePresentFd());
            surfaceFpsManager.RecordPresentTimeForUniRender(hdiOutput_->GetThirdFrameAheadPresentFd(),
                hdiOutput_->GetThirdFrameAheadPresentTime());
        } else {
            surfaceFpsManager.RecordPresentFd(id, vsyncId, hdiOutput_->GetCurrentFramePresentFd());
            surfaceFpsManager.RecordPresentTime(id, hdiOutput_->GetThirdFrameAheadPresentFd(),
                hdiOutput_->GetThirdFrameAheadPresentTime());
        }
    }
}

void RSRenderComposer::ChangeLayersForActiveRectOutside(std::vector<std::shared_ptr<RSLayer>>& layers)
{
#ifdef ROSEN_EMULATOR
    RS_LOGD("emulator device do not need add layer");
    return;
#endif
    if (!RSSystemProperties::IsSuperFoldDisplay() || layers.size() == 0) {
        return;
    }

    const RectI& reviseRect = composerScreenInfo_.reviseRect;
    if (reviseRect.width_ <= 0 || reviseRect.height_ <= 0) {
        return;
    }
    const RectI& maskRect = composerScreenInfo_.maskRect;
    if (maskRect.width_ > 0 && maskRect.height_ > 0) {
        auto solidColorLayer = std::make_shared<RSRenderSurfaceLayer>();
        solidColorLayer->SetZorder(INT_MAX - 1);
        solidColorLayer->SetTransform(GraphicTransformType::GRAPHIC_ROTATE_NONE);
        GraphicIRect dstRect = {maskRect.left_, maskRect.top_, maskRect.width_, maskRect.height_};
        solidColorLayer->SetLayerSize(dstRect);
        solidColorLayer->SetIsMaskLayer(true);
        solidColorLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_SOLID_COLOR);
        bool debugFlag = (system::GetParameter("debug.foldscreen.shaft.color", "0") == "1");
        if (debugFlag) {
            solidColorLayer->SetLayerColor({0, 255, 0, 255});
        } else {
            solidColorLayer->SetLayerColor({0, 0, 0, 255});
        }
        layers.emplace_back(solidColorLayer);
    }
    for (auto& rsLayer : layers) {
        // skip RCD layer
        if (rsLayer->IsScreenRCDLayer()) {
            continue;
        }
        GraphicIRect dstRect = rsLayer->GetLayerSize();
        GraphicIRect tmpRect = dstRect;
        int reviseRight = reviseRect.left_ + reviseRect.width_;
        int reviseBottom = reviseRect.top_ + reviseRect.height_;
        tmpRect.x = std::clamp(dstRect.x, reviseRect.left_, reviseRight);
        tmpRect.y = std::clamp(dstRect.y, reviseRect.top_, reviseBottom);
        tmpRect.w = std::min(tmpRect.x + dstRect.w, reviseRight) - tmpRect.x;
        tmpRect.h = std::min(tmpRect.y + dstRect.h, reviseBottom) - tmpRect.y;
        rsLayer->SetLayerSize(tmpRect);
    }
}

std::string RSRenderComposer::GetSurfaceNameInLayers(const std::vector<std::shared_ptr<RSLayer>>& layers)
{
    std::string surfaceName = "SurfaceName: [";
    bool isFirst = true;
    for (const auto& layer : layers) {
        if (layer == nullptr) {
            continue;
        }
        if (isFirst) {
            surfaceName += layer->GetSurfaceName();
            isFirst = false;
            continue;
        }
        surfaceName += ", " + layer->GetSurfaceName();
    }
    surfaceName += "]";
    return surfaceName;
}

std::string RSRenderComposer::GetSurfaceNameInLayersForTrace(const std::vector<std::shared_ptr<RSLayer>>& layers)
{
    uint32_t count = layers.size();
    uint32_t max = 0;
    for (const auto& layer : layers) {
        if (layer == nullptr) {
            continue;
        }
        count += layer->GetSurfaceName().length();
        if (max < layer->GetZorder()) {
            max = layer->GetZorder();
        }
    }
    bool exceedLimit = count > MAX_TOTAL_SURFACE_NAME_LENGTH;
    std::string surfaceName = "Names:";
    for (const auto& layer : layers) {
        if (layer == nullptr) {
            continue;
        }
        surfaceName += (exceedLimit ? layer->GetSurfaceName().substr(0, MAX_SINGLE_SURFACE_NAME_LENGTH) :
                                      layer->GetSurfaceName()) + ",";
        surfaceName.append("zorder: ");
        surfaceName.append(std::to_string(layer->GetZorder()));
        surfaceName.append(",");
        if (layer->GetType() == GraphicLayerType::GRAPHIC_LAYER_TYPE_CURSOR && layer->GetZorder() < max) {
            RS_LOGE("RSHardcursor is not on the top, hardcursor zorder:%{public}u", layer->GetZorder());
        }
    }

    return surfaceName;
}

bool RSRenderComposer::IsDelayRequired(HgmCore& hgmCore, const PipelineParam& pipelineParam)
{
    if (pipelineParam.isForceRefresh) {
        RS_LOGD("CommitAndReleaseLayers in Force Refresh");
        RS_TRACE_NAME("CommitAndReleaseLayers in Force Refresh");
        return false;
    }

    if (hgmCore.GetLtpoEnabled()) {
        if (AdaptiveModeStatus() == SupportASStatus::SUPPORT_AS) {
            RS_LOGD("CommitAndReleaseLayers in Adaptive Mode");
            RS_TRACE_NAME("CommitAndReleaseLayers in Adaptive Mode");
            return false;
        }
        if (pipelineParam.hasGameScene && AdaptiveModeStatus() == SupportASStatus::GAME_SCENE_SKIP) {
            RS_LOGD("CommitAndReleaseLayers skip delayTime Calculation");
            RS_TRACE_NAME("CommitAndReleaseLayers in Game Scene and skiped delayTime Calculation");
            return false;
        }
    } else {
        if (!hgmCore.IsDelayMode()) {
            return false;
        }
        if (pipelineParam.hasGameScene) {
            RS_LOGD("CommitAndReleaseLayers in Game Scene");
            RS_TRACE_NAME("CommitAndReleaseLayers in Game Scene");
            return false;
        }
    }
    return true;
}

int64_t RSRenderComposer::CalculateDelayTime(HgmCore& hgmCore, uint32_t currentRate,
    int64_t currTime, const PipelineParam& pipelineParam)
{
    int64_t frameOffset = 0;
    int64_t vsyncOffset = 0;
    int64_t idealPipelineOffset = 0;
    int64_t pipelineOffset = 0;
    int64_t expectCommitTime = 0;
    int64_t periodNum = 0;
    int64_t idealPeriod = hgmCore.GetIdealPeriod(currentRate);
    int64_t period  = CreateVSyncSampler()->GetHardwarePeriod();
    uint64_t dvsyncOffset = 0;
    int64_t compositionTime = period;
    int64_t delayTime = 0;

    if (rsVsyncManagerAgent_ != nullptr) {
        dvsyncOffset = rsVsyncManagerAgent_->GetRealTimeOffsetOfDvsync(pipelineParam.frameTimestamp);
    }
    if (!hgmCore.GetLtpoEnabled()) {
        vsyncOffset = UNI_RENDER_VSYNC_OFFSET_DELAY_MODE;
        // 2 period for draw and composition, pipelineOffset = 2 * period
        frameOffset = 2 * period + vsyncOffset - static_cast<int64_t>(pipelineParam.fastComposeTimeStampDiff);
    } else {
        idealPipelineOffset = hgmCore.GetIdealPipelineOffset();
        pipelineOffset = hgmCore.GetPipelineOffset();
        vsyncOffset = CreateVSyncGenerator()->GetVSyncOffset();
        periodNum = idealPeriod == 0 ? 0 : idealPipelineOffset / idealPeriod;

        if (vsyncOffset >= period) {
            vsyncOffset = 0;
        }
        if (periodNum * idealPeriod + vsyncOffset + IDEAL_PULSE < idealPipelineOffset) {
            periodNum = periodNum + 1;
        }
        frameOffset = periodNum * idealPeriod + vsyncOffset +
            static_cast<int64_t>(dvsyncOffset) - static_cast<int64_t>(pipelineParam.fastComposeTimeStampDiff);
    }
    expectCommitTime = pipelineParam.actualTimestamp + frameOffset - compositionTime - RESERVE_TIME;
    int64_t diffTime = expectCommitTime - currTime;
    if (diffTime > 0 && period > 0) {
        delayTime = std::round(diffTime * 1.0f / NS_MS_UNIT_CONVERSION);
    }
    RS_TRACE_NAME_FMT("CalculateDelayTime pipelineOffset: %" PRId64 ", actualTimestamp: %" PRId64 ", " \
        "expectCommitTime: %" PRId64 ", currTime: %" PRId64 ", diffTime: %" PRId64 ", delayTime: %" PRId64 ", " \
        "frameOffset: %" PRId64 ", dvsyncOffset: %" PRIu64 ", vsyncOffset: %" PRId64 ", idealPeriod: %" PRId64 ", " \
        "period: %" PRId64 ", idealPipelineOffset: %" PRId64 ", fastComposeTimeStampDiff: %" PRIu64 "",
        pipelineOffset, pipelineParam.actualTimestamp, expectCommitTime, currTime, diffTime, delayTime,
        frameOffset, dvsyncOffset, vsyncOffset, idealPeriod, period,
        idealPipelineOffset, pipelineParam.fastComposeTimeStampDiff);
    RS_LOGD_IF(DEBUG_PIPELINE, "CalculateDelayTime period:%{public}" PRId64 " delayTime:%{public}" PRId64 "", period,
        delayTime);
    return delayTime;
}

int64_t RSRenderComposer::GetCurTimeCount()
{
    auto curTime = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::microseconds>(curTime).count();
}

int32_t RSRenderComposer::AdaptiveModeStatus()
{
    auto& hgmCore = OHOS::Rosen::HgmCore::Instance();

    // if in game adaptive vsync mode and do direct composition, send layer immediately
    if (auto frameRateMgr = hgmCore.GetFrameRateMgr()) {
        int32_t adaptiveStatus = frameRateMgr->AdaptiveStatus();
        RS_LOGD("CommitAndReleaseLayers send layer adaptiveStatus: %{public}" PRId32, adaptiveStatus);
        if (adaptiveStatus == SupportASStatus::SUPPORT_AS) {
            return SupportASStatus::SUPPORT_AS;
        }
        if (adaptiveStatus == SupportASStatus::GAME_SCENE_SKIP) {
            return SupportASStatus::GAME_SCENE_SKIP;
        }
    }
    return SupportASStatus::NOT_SUPPORT;
}

void RSRenderComposer::PreAllocateProtectedBuffer(sptr<SurfaceBuffer> buffer)
{
    RS_TRACE_NAME_FMT("RSRenderComposer::PreAllocateProtectedBuffer enter screenId : %" PRIu64, screenId_);
    {
        std::unique_lock<std::mutex> lock(preAllocMutex_, std::try_to_lock);
        if (!lock.owns_lock()) {
            RS_TRACE_NAME("RSRenderComposer::PreAllocateProtectedBuffer failed, HardwareThread is redraw.");
            return;
        }
    }
    if (hdiOutput_ == nullptr) {
        RS_LOGE("hdiOutput_ is NULL");
        return;
    }
    if (hdiOutput_->GetProtectedFrameBufferState()) {
        return;
    }
    auto fbSurface = hdiOutput_->GetFrameBufferSurface();
    if (fbSurface == nullptr) {
        RS_LOGE("fbSurface is NULL");
        return;
    }
#ifdef RS_ENABLE_VK
    std::lock_guard<std::mutex> ohosSurfaceLock(surfaceMutex_);
    std::shared_ptr<RSSurfaceOhosVulkan> rsSurface;
    auto surfaceId = fbSurface->GetUniqueId();
    {
        std::lock_guard<std::mutex> lock(frameBufferSurfaceOhosMapMutex_);
        if (frameBufferSurfaceOhosMap_.count(surfaceId)) {
            rsSurface = std::static_pointer_cast<RSSurfaceOhosVulkan>(frameBufferSurfaceOhosMap_[surfaceId]);
        } else {
            rsSurface = std::make_shared<RSSurfaceOhosVulkan>(fbSurface);
            frameBufferSurfaceOhosMap_[surfaceId] = rsSurface;
        }
    }
    // SetColorSpace
    rsSurface->SetColorSpace(ComputeTargetColorGamut(buffer));

    // SetSurfacePixelFormat
    rsSurface->SetSurfacePixelFormat(ComputeTargetPixelFormat(buffer));

    // SetSurfaceBufferUsage
    auto usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_MEM_DMA | BUFFER_USAGE_MEM_FB | BUFFER_USAGE_PROTECTED |
        BUFFER_USAGE_DRM_REDRAW;
    rsSurface->SetSurfaceBufferUsage(usage);
    auto ret = rsSurface->PreAllocateProtectedBuffer(composerScreenInfo_.phyWidth, composerScreenInfo_.phyHeight);
    hdiOutput_->SetProtectedFrameBufferState(ret);
#endif
}

GraphicColorGamut RSRenderComposer::ComputeTargetColorGamut(const sptr<SurfaceBuffer>& buffer)
{
    GraphicColorGamut colorGamut = GRAPHIC_COLOR_GAMUT_SRGB;
#ifdef USE_VIDEO_PROCESSING_ENGINE
    using namespace HDI::Display::Graphic::Common::V1_0;
    CM_ColorSpaceInfo colorSpaceInfo;
    if (MetadataHelper::GetColorSpaceInfo(buffer, colorSpaceInfo) != GSERROR_OK) {
        RS_LOGD("PreAllocateProtectedBuffer Get color space failed");
    }
    if (colorSpaceInfo.primaries != COLORPRIMARIES_SRGB) {
        RS_LOGD("PreAllocateProtectedBuffer fail, primaries is %{public}d", colorSpaceInfo.primaries);
        colorGamut = GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
    }
#endif
    return colorGamut;
}

GraphicPixelFormat RSRenderComposer::ComputeTargetPixelFormat(const sptr<SurfaceBuffer>& buffer)
{
    GraphicPixelFormat pixelFormat = GRAPHIC_PIXEL_FMT_RGBA_8888;
#ifdef USE_VIDEO_PROCESSING_ENGINE
    using namespace HDI::Display::Graphic::Common::V1_0;
    auto bufferPixelFormat = buffer->GetFormat();
    if (bufferPixelFormat == GRAPHIC_PIXEL_FMT_RGBA_1010102 || bufferPixelFormat == GRAPHIC_PIXEL_FMT_YCBCR_P010 ||
        bufferPixelFormat == GRAPHIC_PIXEL_FMT_YCRCB_P010) {
        pixelFormat = GRAPHIC_PIXEL_FMT_RGBA_1010102;
        RS_LOGD("PreAllocateProtectedBuffer pixelformat is set to 1010102 for 10bit buffer");
    }
#endif
    return pixelFormat;
}

void RSRenderComposer::OnScreenVBlankIdleCallback(ScreenId screenId, uint64_t timestamp)
{
    if (hgmHardwareUtils_ == nullptr) {
        RS_LOGE("ComposerProcess hgmHardwareUtils is nullptr, screenId:%{public}" PRIu64, screenId_);
        return;
    }
    RS_TRACE_NAME_FMT("RSRenderComposer::OnScreenVBlankIdleCallback screenId: %" PRIu64" now: %" PRIu64"",
        screenId, timestamp);
    hgmHardwareUtils_->SetScreenVBlankIdle();
}

bool RSRenderComposer::IsDropDirtyFrame(const std::vector<std::shared_ptr<RSLayer>>& layers)
{
#ifdef ROSEN_EMULATOR
    RS_LOGD("emulator device do not need drop dirty frame");
    return false;
#endif
    if (!RSSystemProperties::IsSuperFoldDisplay()) {
        return false;
    }

    auto rect = composerScreenInfo_.activeRect;
    if (rect.IsEmpty()) {
        RS_LOGW("%{public}s: activeRect is empty", __func__);
        return false;
    }
    GraphicIRect activeRect = {rect.left_, rect.top_, rect.width_, rect.height_};
    if (layers.empty()) {
        RS_LOGI("%{public}s: layers is empty", __func__);
        return false;
    }
    for (const auto& rsLayer : layers) {
        if (rsLayer == nullptr) {
            continue;
        }
        auto layerSize = rsLayer->GetLayerSize();
        if (rsLayer->GetUniRenderFlag() && !(activeRect == layerSize)) {
            RS_LOGI("%{public}s: Drop dirty frame cause activeRect:[%{public}d, %{public}d, %{public}d, %{public}d]" \
                "layerSize:[%{public}d, %{public}d, %{public}d, %{public}d]", __func__, activeRect.x, activeRect.y,
                activeRect.w, activeRect.h, layerSize.x, layerSize.y, layerSize.w, layerSize.h);
            return true;
        }
    }
    return false;
}

void RSRenderComposer::OnPrepareComplete(sptr<Surface>& surface,
    const PrepareCompleteParam& param, void* data)
{
    // unused data.
    (void)(data);

    if (!param.needFlushFramebuffer) {
        return;
    }

    if (redrawCb_ != nullptr) {
        redrawCb_(surface, param.layers);
    }
}

GSError RSRenderComposer::ClearFrameBuffersInner(bool isNeedResetContext)
{
    RS_TRACE_NAME_FMT("RSRenderComposer::ClearFrameBuffers screenId : %" PRIu64, screenId_);
    if (isNeedResetContext && uniRenderEngine_ != nullptr) {
        uniRenderEngine_->ResetCurrentContext();
    }
    if (hdiOutput_ == nullptr) {
        RS_LOGE("Clear frame buffers failed for the hdiOutput_ is nullptr");
        return COMPOSITOR_ERROR_NULLPTR;
    }
    auto surface = hdiOutput_->GetFrameBufferSurface();
    if (surface == nullptr) {
        RS_LOGE("%{public}s surface is nullptr", __func__);
        return GSERROR_INVALID_ARGUMENTS;
    }
    auto surfaceId = surface->GetUniqueId();
    std::lock_guard<std::mutex> ohosSurfaceLock(surfaceMutex_);
    std::shared_ptr<RSSurfaceOhos> frameBufferSurfaceOhos;
    {
        std::lock_guard<std::mutex> lock(frameBufferSurfaceOhosMapMutex_);
        if (frameBufferSurfaceOhosMap_.count(surfaceId)) {
            frameBufferSurfaceOhos = frameBufferSurfaceOhosMap_[surfaceId];
        }
        if (frameBufferSurfaceOhos != nullptr) {
#ifdef RS_ENABLE_VK
            if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
                RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
                auto frameBufferSurface = std::static_pointer_cast<RSSurfaceOhosVulkan>(frameBufferSurfaceOhos);
                if (frameBufferSurface) {
                    frameBufferSurface->WaitSurfaceClear();
                }
            }
#endif
            frameBufferSurfaceOhosMap_.erase(surfaceId);
        }
    }
    return hdiOutput_->ClearFrameBuffer();
}

GSError RSRenderComposer::ClearFrameBuffers(bool isNeedResetContext)
{
    if (hdiOutput_ == nullptr || hdiOutput_->GetBufferCacheSize() <= 0) {
        RS_LOGE("%{public}s buffer cache size less 0");
        return COMPOSITOR_ERROR_NULLPTR;
    }
    return ClearFrameBuffersInner(isNeedResetContext);
}

std::shared_ptr<RSSurfaceOhos> RSRenderComposer::CreateFrameBufferSurfaceOhos(const sptr<Surface>& surface)
{
    std::shared_ptr<RSSurfaceOhos> rsSurface = nullptr;
#if (defined RS_ENABLE_GL) && (defined RS_ENABLE_EGLIMAGE)
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
        rsSurface = std::make_shared<RSSurfaceOhosGl>(surface);
    }
#endif
#if (defined RS_ENABLE_VK)
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        rsSurface = std::make_shared<RSSurfaceOhosVulkan>(surface);
    }
#endif
    RS_LOGD("Redraw: CreateFrameBufferSurfaceOhos.");
    return rsSurface;
}

void RSRenderComposer::RedrawScreenRCD(RSPaintFilterCanvas& canvas, const std::vector<std::shared_ptr<RSLayer>>& layers)
{
    RS_TRACE_NAME_FMT("RSRenderComposer::RedrawScreenRCD screenId : %" PRIu64, screenId_);
    std::vector<std::shared_ptr<RSLayer>> rcdLayerInfoList;
    for (const auto& layer : layers) {
        if (layer == nullptr) {
            continue;
        }
        if (layer->GetCompositionType() == GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE ||
            layer->GetCompositionType() == GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE_CLEAR ||
            layer->GetCompositionType() == GraphicCompositionType::GRAPHIC_COMPOSITION_SOLID_COLOR) {
            continue;
        }
        if (layer->IsScreenRCDLayer()) {
            rcdLayerInfoList.push_back(layer);
            continue;
        }
    }
    RSRcdRenderManager::DrawRoundCorner(canvas, rcdLayerInfoList);
}

void RSRenderComposer::Redraw(const sptr<Surface>& surface, const std::vector<std::shared_ptr<RSLayer>>& layers)
{
    RS_TRACE_NAME_FMT("RSRenderComposer::Redraw screenId : %" PRIu64, screenId_);
    std::unique_lock<std::mutex> lock(preAllocMutex_, std::try_to_lock);
    if (surface == nullptr || uniRenderEngine_ == nullptr) {
        RS_LOGE("Redraw: surface or uniRenderEngine is null.");
        return;
    }
    lppLayerCollector_.RemoveLayerId(layers);
    bool isProtected = false;

#ifdef RS_ENABLE_VK
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        for (const auto& layer : layers) {
            if (layer && layer->GetBuffer() && (layer->GetBuffer()->GetUsage() & BUFFER_USAGE_PROTECTED)) {
                isProtected = true;
                break;
            }
        }
        RsVulkanContext::GetSingleton().SetIsProtected(isProtected);
    }
#endif

    RS_LOGD("RsDebug Redraw flush frame buffer start");
    bool forceCPU = RSBaseRenderEngine::NeedForceCPU(layers);
    std::shared_ptr<Drawing::ColorSpace> drawingColorSpace = nullptr;
#ifdef USE_VIDEO_PROCESSING_ENGINE
    GraphicColorGamut colorGamut = ComputeTargetColorGamut(layers);
    GraphicPixelFormat pixelFormat = ComputeTargetPixelFormat(layers);
    RS_LOGD("Redraw computed target color gamut: %{public}d,"
        "pixel format: %{public}d, frame width: %{public}u, frame height: %{public}u",
        colorGamut, pixelFormat, composerScreenInfo_.phyWidth, composerScreenInfo_.phyHeight);
    auto renderFrameConfig = RSBaseRenderUtil::GetFrameBufferRequestConfig(composerScreenInfo_,
        isProtected, colorGamut, pixelFormat);
    drawingColorSpace = RSBaseRenderEngine::ConvertColorGamutToDrawingColorSpace(colorGamut);
    // set color space to surface buffer metadata
    using namespace HDI::Display::Graphic::Common::V1_0;
    CM_ColorSpaceType colorSpace = CM_SRGB_FULL;
    if (ConvertColorGamutToSpaceType(colorGamut, colorSpace)) {
        if (surface->SetUserData("ATTRKEY_COLORSPACE_INFO", std::to_string(colorSpace)) != GSERROR_OK) {
            RS_LOGD("Redraw set user data failed");
        }
    }
#else
    auto renderFrameConfig = RSBaseRenderUtil::GetFrameBufferRequestConfig(composerScreenInfo_, isProtected);
#endif
    // override redraw frame buffer with physical screen resolution.
    renderFrameConfig.width = static_cast<int32_t>(composerScreenInfo_.phyWidth);
    renderFrameConfig.height = static_cast<int32_t>(composerScreenInfo_.phyHeight);

    std::shared_ptr<RSSurfaceOhos> frameBufferSurfaceOhos;
    auto surfaceId = surface->GetUniqueId();
    {
        std::lock_guard<std::mutex> lock(frameBufferSurfaceOhosMapMutex_);
        if (frameBufferSurfaceOhosMap_.count(surfaceId)) {
            frameBufferSurfaceOhos = frameBufferSurfaceOhosMap_[surfaceId];
        } else {
            frameBufferSurfaceOhos = CreateFrameBufferSurfaceOhos(surface);
            frameBufferSurfaceOhosMap_[surfaceId] = frameBufferSurfaceOhos;
        }
    }
    FrameContextConfig frameContextConfig = FrameContextConfig(isProtected);
    std::lock_guard<std::mutex> ohosSurfaceLock(surfaceMutex_);
    auto renderFrame = uniRenderEngine_->RequestFrame(frameBufferSurfaceOhos, renderFrameConfig,
        forceCPU, true, frameContextConfig);
    if (renderFrame == nullptr) {
        RS_LOGE("RsDebug Redraw failed to request frame.");
        return;
    }
    auto canvas = renderFrame->GetCanvas();
    if (canvas == nullptr) {
        RS_LOGE("RsDebug Redraw canvas is nullptr.");
        return;
    }
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::IsUseVulkan()) {
        canvas->Clear(Drawing::Color::COLOR_TRANSPARENT);
    }
#endif

#ifdef USE_VIDEO_PROCESSING_ENGINE
    uniRenderEngine_->DrawLayers(*canvas, layers, false, composerScreenInfo_, colorGamut);
#else
    uniRenderEngine_->DrawLayers(*canvas, layers, false, composerScreenInfo_);
#endif
    RedrawScreenRCD(*canvas, layers);
#ifdef RS_ENABLE_TV_PQ_METADATA
    auto rsSurface = renderFrame->GetSurface();
    RSTvMetadataManager::CopyFromLayersToSurface(layers, rsSurface);
#endif
    renderFrame->Flush();
    RS_LOGD("RsDebug Redraw flush frame buffer end");
}

#ifdef RES_SCHED_ENABLE
void RSRenderComposer::SubScribeSystemAbility()
{
    RS_LOGI("%{public}s", __func__);
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityManager) {
        RS_LOGE("%{public}s failed to get system ability manager client", __func__);
        return;
    }
    std::string threadName = "CompThread_" + std::to_string(screenId_);
    std::string strUid = std::to_string(getuid());
    std::string strPid = std::to_string(getpid());
    std::string strTid = std::to_string(gettid());

    saStatusChangeListener_ = new (std::nothrow)VSyncSystemAbilityListener(threadName, strUid, strPid, strTid);
    int32_t ret = systemAbilityManager->SubscribeSystemAbility(RES_SCHED_SYS_ABILITY_ID, saStatusChangeListener_);
    if (ret != ERR_OK) {
        RS_LOGE("%{public}s subscribe system ability %{public}d failed.", __func__, RES_SCHED_SYS_ABILITY_ID);
        saStatusChangeListener_ = nullptr;
    }
}
#endif

#ifdef USE_VIDEO_PROCESSING_ENGINE
GraphicColorGamut RSRenderComposer::ComputeTargetColorGamut(const std::vector<std::shared_ptr<RSLayer>>& layers)
{
    using namespace HDI::Display::Graphic::Common::V1_0;
    GraphicColorGamut colorGamut = GRAPHIC_COLOR_GAMUT_SRGB;
    for (auto& layer : layers) {
        if (layer == nullptr) {
            RS_LOGE("%{public}s layer is nullptr", __func__);
            continue;
        }
        auto buffer = layer->GetBuffer();
        if (buffer == nullptr) {
            RS_LOGW("%{public}s The buffer of layer is nullptr", __func__);
            continue;
        }

        CM_ColorSpaceInfo colorSpaceInfo;
        if (MetadataHelper::GetColorSpaceInfo(buffer, colorSpaceInfo) != GSERROR_OK) {
            RS_LOGD("%{public}s Get color space failed", __func__);
            continue;
        }

        if (colorSpaceInfo.primaries != COLORPRIMARIES_SRGB) {
            RS_LOGD("%{public}s fail, primaries is %{public}d", __func__, colorSpaceInfo.primaries);
            colorGamut = GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
            break;
        }
    }

    return colorGamut;
}

bool RSRenderComposer::IsAllRedraw(const std::vector<std::shared_ptr<RSLayer>>& layers)
{
    for (auto& layer : layers) {
        if (layer == nullptr) {
            continue;
        }
        if (layer->IsScreenRCDLayer()) {
            RS_LOGD("%{public}s skip RCD layer", __func__);
            continue;
        }
        // skip hard cursor
        if (layer->GetType() == GraphicLayerType::GRAPHIC_LAYER_TYPE_CURSOR) {
            RS_LOGD("%{public}s skip cursor", __func__);
            continue;
        }
        if (layer->GetCompositionType() == GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE ||
            layer->GetCompositionType() == GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE_CLEAR ||
            layer->GetCompositionType() == GraphicCompositionType::GRAPHIC_COMPOSITION_SOLID_COLOR) {
            RS_LOGD("%{public}s not all layers are redraw", __func__);
            return false;
        }
    }
    return true;
}

GraphicPixelFormat RSRenderComposer::ComputeTargetPixelFormat(const std::vector<std::shared_ptr<RSLayer>>& layers)
{
    using namespace HDI::Display::Graphic::Common::V1_0;
    GraphicPixelFormat pixelFormat = GRAPHIC_PIXEL_FMT_RGBA_8888;
    bool allRedraw = IsAllRedraw(layers);
    for (auto& layer : layers) {
        if (layer == nullptr) {
            continue;
        }
        if (layer->GetCompositionType() == GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE ||
            layer->GetCompositionType() == GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE_CLEAR ||
            layer->GetCompositionType() == GraphicCompositionType::GRAPHIC_COMPOSITION_SOLID_COLOR) {
            continue;
        }
        auto buffer = layer->GetBuffer();
        if (buffer == nullptr) {
            RS_LOGW("%{public}s The buffer of layer is nullptr", __func__);
            continue;
        }

        auto bufferPixelFormat = buffer->GetFormat();
        if (bufferPixelFormat == GRAPHIC_PIXEL_FMT_RGBA_1010108) {
            pixelFormat = GRAPHIC_PIXEL_FMT_RGBA_1010102;
            if (!allRedraw && RSHdrUtil::GetRGBA1010108Enabled()) {
                pixelFormat = GRAPHIC_PIXEL_FMT_RGBA_1010108;
                RS_LOGD("%{public}s pixelformat is set to GRAPHIC_PIXEL_FMT_RGBA_1010108", __func__);
            }
            break;
        }
        if (bufferPixelFormat == GRAPHIC_PIXEL_FMT_RGBA_1010102 ||
            bufferPixelFormat == GRAPHIC_PIXEL_FMT_YCBCR_P010 ||
            bufferPixelFormat == GRAPHIC_PIXEL_FMT_YCRCB_P010) {
            pixelFormat = GRAPHIC_PIXEL_FMT_RGBA_1010102;
            RS_LOGD("%{public}s pixelformat is set to 1010102 for 10bit buffer", __func__);
        }
    }

    return pixelFormat;
}

bool RSRenderComposer::ConvertColorGamutToSpaceType(const GraphicColorGamut& colorGamut,
    HDI::Display::Graphic::Common::V1_0::CM_ColorSpaceType& colorSpaceType)
{
    using namespace HDI::Display::Graphic::Common::V1_0;
    static const std::map<GraphicColorGamut, CM_ColorSpaceType> RS_TO_COMMON_COLOR_SPACE_TYPE_MAP {
        {GRAPHIC_COLOR_GAMUT_STANDARD_BT601, CM_BT601_EBU_FULL},
        {GRAPHIC_COLOR_GAMUT_STANDARD_BT709, CM_BT709_FULL},
        {GRAPHIC_COLOR_GAMUT_SRGB, CM_SRGB_FULL},
        {GRAPHIC_COLOR_GAMUT_ADOBE_RGB, CM_ADOBERGB_FULL},
        {GRAPHIC_COLOR_GAMUT_DISPLAY_P3, CM_P3_FULL},
        {GRAPHIC_COLOR_GAMUT_BT2020, CM_DISPLAY_BT2020_SRGB},
        {GRAPHIC_COLOR_GAMUT_BT2100_PQ, CM_BT2020_PQ_FULL},
        {GRAPHIC_COLOR_GAMUT_BT2100_HLG, CM_BT2020_HLG_FULL},
        {GRAPHIC_COLOR_GAMUT_DISPLAY_BT2020, CM_DISPLAY_BT2020_SRGB},
    };

    if (RS_TO_COMMON_COLOR_SPACE_TYPE_MAP.find(colorGamut) == RS_TO_COMMON_COLOR_SPACE_TYPE_MAP.end()) {
        return false;
    }

    colorSpaceType = RS_TO_COMMON_COLOR_SPACE_TYPE_MAP.at(colorGamut);
    return true;
}
#endif

void RSRenderComposer::ContextRegisterPostTask()
{
#if defined(RS_ENABLE_VK) && defined(IS_ENABLE_DRM)
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        RsVulkanContext::GetSingleton().SetIsProtected(true);
        auto context = RsVulkanContext::GetSingleton().GetDrawingContext();
        if (context) {
            context->RegisterPostFunc([this](const std::function<void()>& task) { PostTask(task); });
        }
        RsVulkanContext::GetSingleton().SetIsProtected(false);
        context = RsVulkanContext::GetSingleton().GetDrawingContext();
        if (context) {
            context->RegisterPostFunc([this](const std::function<void()>& task) { PostTask(task); });
        }
    }
#endif
}

void RSRenderComposer::SetComposerToRenderConnection(const sptr<IRSComposerToRenderConnection>& composerToRenderConn)
{
    composerToRenderConnection_ = composerToRenderConn;
}

void RSRenderComposer::ReInit(const std::shared_ptr<HdiOutput>& output,
    const sptr<RSScreenProperty>& property)
{
    hdiOutput_ = output;
    GraphicIRect damageRect {
        0, 0, static_cast<int32_t>(property->GetWidth()), static_cast<int32_t>(property->GetHeight())
    };
    std::vector<GraphicIRect> damageRects;
    damageRects.emplace_back(damageRect);
    hdiOutput_->SetOutputDamages(damageRects);
    auto onPrepareCompleteFunc = [this](auto& surface, const auto& param, void* data) {
        OnPrepareComplete(surface, param, data);
    };
    hdiOutput_->RegPrepareComplete(onPrepareCompleteFunc, this);
}

void RSRenderComposer::OnScreenConnected(const std::shared_ptr<HdiOutput>& output,
    const sptr<RSScreenProperty>& property)
{
    RS_TRACE_NAME("RSRenderComposer::OnScreenConnected");
    RS_LOGI("%{public}s screenId: %{public}" PRIu64, __func__, screenId_);
    isDisconnected_ = false;
    ReInit(output, property);
    rsRenderComposerContext_ = std::make_shared<RSRenderComposerContext>();
}

void RSRenderComposer::OnScreenDisconnected()
{
    RS_TRACE_NAME("RSRenderComposer::OnScreenDisconnected");
    RS_LOGI("%{public}s screenId: %{public}" PRIu64, __func__, screenId_);
    if (unExecuteTaskNum_ == 0) {
        RS_TRACE_NAME_FMT("OnScreenDisconnected Clear output, screenId : %" PRIu64, screenId_);
        ClearFrameBuffersInner();
        rsRenderComposerContext_ = nullptr;
        hdiOutput_ = nullptr;
    }
    isDisconnected_ = true;
}

void RSRenderComposer::OnHwcRestored(const std::shared_ptr<HdiOutput>& output,
    const sptr<RSScreenProperty>& property)
{
    RS_TRACE_NAME("RSRenderComposer::OnHwcRestored");
    RS_LOGI("%{public}s screenId: %{public}" PRIu64, __func__, screenId_);
    isHwcDead_ = false;
    ReInit(output, property);
}

void RSRenderComposer::OnHwcDead()
{
    RS_TRACE_NAME("RSRenderComposer::OnHwcDead");
    RS_LOGI("%{public}s screenId: %{public}" PRIu64, __func__, screenId_);
    ClearFrameBuffersInner();
    isHwcDead_ = true;
    if (hdiOutput_) {
        hdiOutput_->ResetDevice();
    }
}

void RSRenderComposer::DestroyComposerLayer(std::shared_ptr<RSLayerParcel> rsLayerParcel)
{
    RS_TRACE_NAME_FMT("DestroyComposerLayer screenId: %" PRIu64, screenId_);
    RS_LOGI("%{public}s screenId: %{public}" PRIu64, __func__, screenId_);
    rsLayerParcel->ApplyRSLayerCmd(rsRenderComposerContext_);
}

void RSRenderComposer::UpdateComposerLayer(std::shared_ptr<RSLayerParcel> rsLayerParcel)
{
    rsLayerParcel->ApplyRSLayerCmd(rsRenderComposerContext_);
}

void RSRenderComposer::UpdateTransactionData(std::shared_ptr<RSLayerTransactionData> transactionData)
{
    RS_TRACE_NAME_FMT("UpdateTransactionData screenId:%" PRIu64, screenId_);
    for (auto& [layerId, rsLayerParcel] : transactionData->GetPayload()) {
        if (rsLayerParcel == nullptr) {
            continue;
        }
        auto rsLayerParcelType = rsLayerParcel->GetRSLayerParcelType();
        switch (rsLayerParcelType) {
            case RSLayerParcelType::RS_LAYER_DESTROY: {
                DestroyComposerLayer(rsLayerParcel);
                break;
            }
            case RSLayerParcelType::RS_RCD_LAYER_UPDATE: {
                UpdateComposerLayer(rsLayerParcel);
                break;
            }
            case RSLayerParcelType::RS_LAYER_UPDATE: {
                UpdateComposerLayer(rsLayerParcel);
                break;
            }
            default: {
                RS_LOGE("%{public}s pacel type:%{public}d", __func__, rsLayerParcelType);
                break;
            }
        }
    }

    if (composerScreenInfo_.activeRect != transactionData->GetComposerScreenInfo().activeRect) {
        auto reviseRect = transactionData->GetComposerScreenInfo().reviseRect;
        const GraphicIRect graphicIRect = {
            reviseRect.left_, reviseRect.top_, reviseRect.width_, reviseRect.height_
        };
        hdiOutput_->SetActiveRectSwitchStatus(true, graphicIRect);
    }
    composerScreenInfo_ = transactionData->GetComposerScreenInfo();
    UpdateForSurfaceFps(transactionData->GetPipelineParam());
}

void RSRenderComposer::UpdateForSurfaceFps(const PipelineParam& pipelineParam) {
    for (size_t i = 0; i < pipelineParam.GetSurfaceFpsOpNum(); i++) {
        if (pipelineParam.SurfaceFpsOpList[i].surfaceFpsOpType ==
            static_cast<uint32_t>(SurfaceFpsOpType::SURFACE_FPS_ADD)) {
            RS_LOGD("update for surfaceFps add op id: %{public}" PRIu64 ", name: %{public}s",
                pipelineParam.SurfaceFpsOpList[i].surfaceNodeId, pipelineParam.SurfaceFpsOpList[i].surfaceName.c_str());
            RSSurfaceFpsManager::GetInstance().RegisterSurfaceFps(pipelineParam.SurfaceFpsOpList[i].surfaceNodeId,
                pipelineParam.SurfaceFpsOpList[i].surfaceName.c_str());
        } else if (pipelineParam.SurfaceFpsOpList[i].surfaceFpsOpType ==
            static_cast<uint32_t>(SurfaceFpsOpType::SURFACE_FPS_REMOVE)) {
            RS_LOGD("update for surfaceFps remove op id: %{public}" PRIu64 ", name: %{public}s",
                pipelineParam.SurfaceFpsOpList[i].surfaceNodeId, pipelineParam.SurfaceFpsOpList[i].surfaceName.c_str());
            RSSurfaceFpsManager::GetInstance().UnregisterSurfaceFps(pipelineParam.SurfaceFpsOpList[i].surfaceNodeId);
        }
    }
}

void RSRenderComposer::AddSolidColorLayer(std::vector<std::shared_ptr<RSLayer>>& layers)
{
    for (uint32_t i = 0; i < layers.size(); i++) {
        if (layers[i] != nullptr && layers[i]->GetSolidColorLayerProperty().compositionType ==
            GraphicCompositionType::GRAPHIC_COMPOSITION_SOLID_COLOR) {
            auto solidColorLayer = std::make_shared<RSRenderSurfaceLayer>();
            solidColorLayer->CopyLayerInfo(layers[i]);
            solidColorLayer->SetZorder(layers[i]->GetSolidColorLayerProperty().zOrder);
            solidColorLayer->SetTransform(layers[i]->GetSolidColorLayerProperty().transformType);
            solidColorLayer->SetLayerSize(layers[i]->GetSolidColorLayerProperty().layerRect);
            solidColorLayer->SetCompositionType(layers[i]->GetSolidColorLayerProperty().compositionType);
            solidColorLayer->SetLayerColor(layers[i]->GetSolidColorLayerProperty().layerColor);
            solidColorLayer->SetSurfaceUniqueId(0);
            solidColorLayer->SetSurfaceName("");
            solidColorLayer->SetSurface({});
            solidColorLayer->SetBuffer({}, {});
            solidColorLayer->SetPreBuffer({});
            solidColorLayer->SetMetaData({});
            solidColorLayer->SetSolidColorLayerProperty({0, GraphicTransformType::GRAPHIC_ROTATE_NONE, { 0, 0, 0, 0 },
                GraphicCompositionType::GRAPHIC_COMPOSITION_BUTT, { 0, 0, 0, 0 }});
            layers[i]->SetSolidColorLayerProperty({0, GraphicTransformType::GRAPHIC_ROTATE_NONE, { 0, 0, 0, 0 },
                GraphicCompositionType::GRAPHIC_COMPOSITION_BUTT, { 0, 0, 0, 0 }});
            layers.push_back(solidColorLayer);
        }
    }
}

void RSRenderComposer::CleanLayerBufferBySurfaceId(uint64_t surfaceId)
{
    if (hdiOutput_ == nullptr) {
        RS_LOGE("%{public}s output is nullptr, screenId:%{public}" PRIu64, __func__, screenId_);
        return;
    }
    hdiOutput_->CleanLayerBufferBySurfaceId(surfaceId);
}

void RSRenderComposer::InitRsVsyncManagerAgent(const sptr<RSVsyncManagerAgent>& rsVsyncManagerAgent)
{
    rsVsyncManagerAgent_ = rsVsyncManagerAgent;
}

void RSRenderComposer::SurfaceDump(std::string& dumpString)
{
    if (hdiOutput_ == nullptr) {
        RS_LOGW("%{public}s: hdiOutput_ is nullptr.", __func__);
        return;
    }
    hdiOutput_->Dump(dumpString);
}

void RSRenderComposer::GetRefreshInfoToSP(std::string& dumpString, NodeId nodeId)
{
    RSSurfaceFpsManager::GetInstance().Dump(dumpString, nodeId);
}

void RSRenderComposer::FpsDump(std::string& dumpString, const std::string& layerName)
{
    if (hdiOutput_ == nullptr) {
        RS_LOGW("%{public}s: hdiOutput_ is nullptr.", __func__);
        return;
    }
    hdiOutput_->DumpFps(dumpString, layerName);
}

void RSRenderComposer::ClearFpsDump(std::string& dumpString, std::string& layerName)
{
    if (hdiOutput_ == nullptr) {
        RS_LOGW("%{public}s: hdiOutput_ is nullptr.", __func__);
        return;
    }
    hdiOutput_->ClearFpsDump(dumpString, layerName);
}

void RSRenderComposer::HitchsDump(std::string& dumpString, std::string& layerArg)
{
    if (hdiOutput_ == nullptr) {
        RS_LOGW("%{public}s: hdiOutput_ is nullptr.", __func__);
        return;
    }
    hdiOutput_->DumpHitchs(dumpString, layerArg);
}

void RSRenderComposer::SetScreenBacklight(uint32_t level)
{
    if (hdiOutput_ == nullptr) {
        RS_LOGW("%{public}s: hdiOutput_ is nullptr.", __func__);
        return;
    }
    hdiOutput_->SetScreenBacklight(level);
}

// todo
void RSRenderComposer::SetScreenPowerOnChanged(bool flag)
{
    if (hdiOutput_ == nullptr) {
        return;
    }
    hdiOutput_->SetScreenPowerOnChanged(flag);
}

int64_t RSRenderComposer::GetDelayTime()
{
    return delayTime_.load();
}
}