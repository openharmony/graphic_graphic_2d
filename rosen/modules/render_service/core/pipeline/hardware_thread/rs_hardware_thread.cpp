/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

#include "rs_hardware_thread.h"

#include <memory>
#include <unistd.h>

#include "frame_report.h"
#include "hdi_backend.h"
#include "hgm_frame_rate_manager.h"
#include "hisysevent.h"
#include "parameters.h"
#include "pipeline/hardware_thread/rs_realtime_refresh_rate_manager.h"
#include "rs_trace.h"
#include "vsync_sampler.h"

#include "common/rs_exception_check.h"
#include "common/rs_optional_trace.h"
#include "common/rs_singleton.h"
#include "feature/round_corner_display/rs_round_corner_display_manager.h"
#include "pipeline/render_thread/rs_base_render_util.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/render_thread/rs_uni_render_engine.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "platform/ohos/backend/rs_surface_ohos_gl.h"
#include "platform/ohos/backend/rs_surface_ohos_raster.h"
#include "screen_manager/rs_screen_manager.h"
#include "gfx/fps_info/rs_surface_fps_manager.h"
#include "gfx/first_frame_notifier/rs_first_frame_notifier.h"
#include "platform/common/rs_hisysevent.h"
#include "graphic_feature_param_manager.h"

#ifdef RS_ENABLE_EGLIMAGE
#include "src/gpu/gl/GrGLDefines.h"
#endif

#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_surface_ohos_vulkan.h"
#include "feature/gpuComposition/rs_vk_image_manager.h"
#endif

#ifdef RS_ENABLE_EGLIMAGE
#include "feature/gpuComposition/rs_egl_image_manager.h"
#endif // RS_ENABLE_EGLIMAGE

#ifdef USE_VIDEO_PROCESSING_ENGINE
#include "metadata_helper.h"
#endif

#ifdef RES_SCHED_ENABLE
#include "system_ability_definition.h"
#include "if_system_ability_manager.h"
#include <iservice_registry.h>
#include "ressched_event_listener.h"
#endif

#undef LOG_TAG
#define LOG_TAG "RSHardwareThread"

namespace OHOS::Rosen {
namespace {
constexpr uint32_t HARDWARE_THREAD_TASK_NUM = 2;
constexpr uint32_t HARD_JANK_TWO_TIME = 2;
constexpr int64_t REFRESH_PERIOD = 16667; // 16667us == 16.667ms
constexpr int64_t REPORT_LOAD_WARNING_INTERVAL_TIME = 5000000; // 5s == 5000000us
constexpr int64_t RESERVE_TIME = 1000000; // we reserve 1ms more for the composition
constexpr int64_t COMMIT_DELTA_TIME = 2; // 2ms
constexpr int64_t MAX_DELAY_TIME = 100; // 100ms
constexpr int64_t NS_MS_UNIT_CONVERSION = 1000000;
constexpr int64_t UNI_RENDER_VSYNC_OFFSET_DELAY_MODE = 3300000; // 3.3ms
constexpr uint32_t DELAY_TIME_OFFSET = 100; // 5ms
constexpr uint32_t MAX_TOTAL_SURFACE_NAME_LENGTH = 320;
constexpr uint32_t MAX_SINGLE_SURFACE_NAME_LENGTH = 20;
// Threshold for abnormal time in the hardware pipeline
constexpr int HARDWARE_TIMEOUT = 800;
// The number of exceptions in the hardware pipeline required to achieve render_service reset
constexpr int HARDWARE_TIMEOUT_ABORT_CNT = 30;
// The number of exceptions in the hardware pipeline required to report hisysevent
constexpr int HARDWARE_TIMEOUT_CNT = 15;
const std::string PROCESS_NAME_FOR_HISYSEVENT = "/system/bin/render_service";
const std::string HARDWARE_PIPELINE_TIMEOUT = "hardware_pipeline_timeout";
}

static int64_t SystemTime()
{
    timespec t = {};
    clock_gettime(CLOCK_MONOTONIC, &t);
    return int64_t(t.tv_sec) * 1000000000LL + t.tv_nsec; // 1000000000ns == 1s
}

RSHardwareThread& RSHardwareThread::Instance()
{
    static RSHardwareThread instance;
    return instance;
}

void RSHardwareThread::Start()
{
    RS_LOGI("Start()!");
    hdiBackend_ = HdiBackend::GetInstance();
    runner_ = AppExecFwk::EventRunner::Create("RSHardwareThread");
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    redrawCb_ = [this](const sptr<Surface>& surface, const std::vector<LayerInfoPtr>& layers, uint32_t screenId) {
        return this->Redraw(surface, layers, screenId);
    };
    if (handler_) {
        ScheduleTask(
            [this]() {
#if defined (RS_ENABLE_VK)
                // Change vk interface type from UNIRENDER into UNPROTECTED_REDRAW, this is necessary for hardware init.
                if (RSSystemProperties::IsUseVulkan()) {
                    RsVulkanContext::GetSingleton().SetIsProtected(false);
                }
#endif
                auto screenManager = CreateOrGetScreenManager();
                if (screenManager == nullptr || !screenManager->Init()) {
                    RS_LOGE("RSHardwareThread CreateOrGetScreenManager or init fail.");
                    return;
                }
#ifdef RES_SCHED_ENABLE
                SubScribeSystemAbility();
#endif
                uniRenderEngine_ = std::make_shared<RSUniRenderEngine>();
                uniRenderEngine_->Init(true);
#ifdef RS_ENABLE_VK
                // posttask for multithread safely release surface and image
                if (RSSystemProperties::IsUseVulkan()) {
                    ContextRegisterPostTask();
                }
#endif
                hardwareTid_ = gettid();
            }).wait();
    }
    auto onPrepareCompleteFunc = [this](auto& surface, const auto& param, void* data) {
        OnPrepareComplete(surface, param, data);
    };
    if (hdiBackend_ != nullptr) {
        hdiBackend_->RegPrepareComplete(onPrepareCompleteFunc, this);
    }
    auto changeDssRefreshRateCb = [this] (ScreenId screenId, uint32_t refreshRate, bool followPipline) {
        PostTask([this, screenId, refreshRate, followPipline] () {
            ChangeDssRefreshRate(screenId, refreshRate, followPipline);
        });
    };
    HgmTaskHandleThread::Instance().PostTask([changeDssRefreshRateCb] () {
        if (auto frameRateMgr = HgmCore::Instance().GetFrameRateMgr(); frameRateMgr != nullptr) {
            frameRateMgr->SetChangeDssRefreshRateCb(changeDssRefreshRateCb);
        }
    });
}

int RSHardwareThread::GetHardwareTid() const
{
    return hardwareTid_;
}

void RSHardwareThread::PostTask(const std::function<void()>& task)
{
    if (handler_) {
        handler_->PostTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

void RSHardwareThread::PostSyncTask(const std::function<void()>& task)
{
    if (handler_) {
        handler_->PostSyncTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

void RSHardwareThread::PostDelayTask(const std::function<void()>& task, int64_t delayTime)
{
    if (handler_) {
        handler_->PostTask(task, delayTime, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

void RSHardwareThread::DumpVkImageInfo(std::string &dumpString)
{
    std::weak_ptr<RSBaseRenderEngine> uniRenderEngine = uniRenderEngine_;
    PostSyncTask([&dumpString, uniRenderEngine]() {
        if (auto engine = uniRenderEngine.lock()) {
            engine->DumpVkImageInfo(dumpString);
        }
    });
}

uint32_t RSHardwareThread::GetunExecuteTaskNum()
{
    return unExecuteTaskNum_.load();
}

void RSHardwareThread::ClearRedrawGPUCompositionCache(const std::set<uint32_t>& bufferIds)
{
    std::weak_ptr<RSBaseRenderEngine> uniRenderEngine = uniRenderEngine_;
    PostDelayTask(
        [uniRenderEngine, bufferIds]() {
            if (auto engine = uniRenderEngine.lock()) {
                engine->ClearCacheSet(bufferIds);
            }
        },
        delayTime_);
}

void RSHardwareThread::RefreshRateCounts(std::string& dumpString)
{
    if (refreshRateCounts_.empty()) {
        RS_LOGE("RefreshData fail, refreshRateCounts_ is empty");
        return;
    }
    std::map<uint32_t, uint64_t>::iterator iter;
    for (iter = refreshRateCounts_.begin(); iter != refreshRateCounts_.end(); ++iter) {
        dumpString.append(
            "Refresh Rate:" + std::to_string(iter->first) + ", Count:" + std::to_string(iter->second) + ";\n");
    }
    RS_LOGD("RefreshRateCounts refresh rate counts info is displayed");
}

void RSHardwareThread::ClearRefreshRateCounts(std::string& dumpString)
{
    if (refreshRateCounts_.empty()) {
        RS_LOGE("ClearRefreshData fail, refreshRateCounts_ is empty");
        return;
    }
    refreshRateCounts_.clear();
    dumpString.append("The refresh rate counts info is cleared successfully!\n");
    RS_LOGD("RefreshRateCounts refresh rate counts info is cleared");
}

void RSHardwareThread::CommitAndReleaseLayers(OutputPtr output, const std::vector<LayerInfoPtr>& layers)
{
    if (!handler_) {
        RS_LOGE("CommitAndReleaseLayers handler is nullptr");
        return;
    }
    delayTime_ = 0;
    RSTimer timer("Hardware", HARDWARE_TIMEOUT);
    LayerComposeCollection::GetInstance().UpdateUniformOrOfflineComposeFrameNumberForDFX(layers.size());
    RefreshRateParam param = GetRefreshRateParam();
    refreshRateParam_ = param;
    auto& hgmCore = OHOS::Rosen::HgmCore::Instance();
    ScreenId curScreenId = hgmCore.GetActiveScreenId();
    uint32_t currentRate = hgmCore.GetScreenCurrentRefreshRate(curScreenId);
    bool hasGameScene = FrameReport::GetInstance().HasGameScene();
#ifdef RES_SCHED_ENABLE
    ResschedEventListener::GetInstance()->ReportFrameToRSS();
#endif
    RSTaskMessage::RSTask task = [this, output = output, layers = layers, param = param,
        currentRate = currentRate, hasGameScene = hasGameScene, curScreenId = curScreenId]() {
#ifdef HIPERF_TRACE_ENABLE
        RS_LOGW("hiperf_surface_counter3 %{public}" PRIu64 " ", static_cast<uint64_t>(layers.size()));
#endif
        int64_t startTime = GetCurTimeCount();
        std::string surfaceName = GetSurfaceNameInLayers(layers);
        RS_LOGI_LIMIT("CommitAndReleaseLayers task execute, %{public}s", surfaceName.c_str());
        if (output == nullptr || hdiBackend_ == nullptr) {
            RS_LOGI("CommitAndReleaseLayers task return, %{public}s", surfaceName.c_str());
            return;
        }
        int64_t startTimeNs = 0;
        int64_t endTimeNs = 0;

        RSFirstFrameNotifier::GetInstance().ExecIfFirstFrameCommit(output->GetScreenId());

        RS_LOGI_IF(DEBUG_COMPOSER, "CommitAndReleaseData hasGameScene is %{public}d %{public}s",
            hasGameScene, surfaceName.c_str());
        if (hasGameScene) {
            startTimeNs = std::chrono::duration_cast<std::chrono::nanoseconds>(
                std::chrono::steady_clock::now().time_since_epoch()).count();
        }
        RS_TRACE_NAME_FMT("CommitLayers rate:%u,now:%" PRIu64 ",vsyncId:%" PRIu64 ",size:%zu,%s",
            currentRate, param.frameTimestamp, param.vsyncId, layers.size(),
            GetSurfaceNameInLayersForTrace(layers).c_str());
        RS_LOGD_IF(DEBUG_COMPOSER, "CommitAndReleaseLayers rate:%{public}u, " \
            "now:%{public}" PRIu64 ", vsyncId:%{public}" PRIu64 ", size:%{public}zu, %{public}s",
            currentRate, param.frameTimestamp, param.vsyncId, layers.size(), surfaceName.c_str());

        bool isScreenPoweringOff = false;
        auto screenManager = CreateOrGetScreenManager();
        if (screenManager) {
            isScreenPoweringOff = RSSystemProperties::IsSmallFoldDevice() &&
                screenManager->IsScreenPoweringOff(output->GetScreenId());
        }

        if (!isScreenPoweringOff) {
            ExecuteSwitchRefreshRate(output, param.rate);
            PerformSetActiveMode(output, param.frameTimestamp, param.constraintRelativeTime);
            AddRefreshRateCount(output);
        }

        if (RSSystemProperties::IsSuperFoldDisplay() && output->GetScreenId() == 0) {
            std::vector<LayerInfoPtr> reviseLayers = layers;
            ChangeLayersForActiveRectOutside(reviseLayers, curScreenId);
            output->SetLayerInfo(reviseLayers);
        } else {
            output->SetLayerInfo(layers);
        }
        bool doRepaint = output->IsDeviceValid() && !isScreenPoweringOff && !IsDropDirtyFrame(output);
        if (doRepaint) {
            hdiBackend_->Repaint(output);
            RecordTimestamp(layers);
        }
        output->ReleaseLayers(releaseFence_);
        RSBaseRenderUtil::DecAcquiredBufferCount();
        RSUniRenderThread::Instance().NotifyDisplayNodeBufferReleased();
        if (hasGameScene) {
            endTimeNs = std::chrono::duration_cast<std::chrono::nanoseconds>(
                std::chrono::steady_clock::now().time_since_epoch()).count();
            FrameReport::GetInstance().SetLastSwapBufferTime(endTimeNs - startTimeNs);
        }

        unExecuteTaskNum_--;
        RS_LOGD_IF(DEBUG_COMPOSER, "CommitAndReleaseData unExecuteTaskNum_:%{public}d,"
            " HARDWARE_THREAD_TASK_NUM:%{public}d, %{public}s",
            unExecuteTaskNum_.load(), HARDWARE_THREAD_TASK_NUM, surfaceName.c_str());
        if (unExecuteTaskNum_ <= HARDWARE_THREAD_TASK_NUM) {
            RSMainThread::Instance()->NotifyHardwareThreadCanExecuteTask();
        }
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
            RS_TRACE_NAME("RSHardwareThread::CommitAndReleaseLayers HiSysEventWrite in RSHardwareThread");
            RSHiSysEvent::EventWrite(RSEventName::RS_HARDWARE_THREAD_LOAD_WARNING, RSEventType::RS_STATISTIC,
                "FRAME_RATE", frameRate, "MISSED_FRAMES", missedFrames, "FRAME_TIME", frameTime);
        }
    };
    RSBaseRenderUtil::IncAcquiredBufferCount();
    unExecuteTaskNum_++;
    RSMainThread::Instance()->SetHardwareTaskNum(unExecuteTaskNum_.load());
    RS_LOGI_IF(DEBUG_COMPOSER, "CommitAndReleaseData hgmCore's LtpoEnabled is %{public}d", hgmCore.GetLtpoEnabled());
    int64_t currTime = SystemTime();
    if (IsDelayRequired(hgmCore, param, output, hasGameScene)) {
        CalculateDelayTime(hgmCore, param, currentRate, currTime);
    }

    // We need to ensure the order of composition frames, postTaskTime(n + 1) must > postTaskTime(n),
    // and we give a delta time more between two composition tasks.
    int64_t currCommitTime = currTime + delayTime_ * NS_MS_UNIT_CONVERSION;
    if (currCommitTime <= lastCommitTime_) {
        delayTime_ = delayTime_ +
            std::round((lastCommitTime_ - currCommitTime) * 1.0f / NS_MS_UNIT_CONVERSION) +
            COMMIT_DELTA_TIME;
        RS_LOGD("CommitAndReleaseLayers vsyncId: %{public}" PRIu64 ", " \
            "update delayTime: %{public}" PRId64 ", currCommitTime: %{public}" PRId64 ", " \
            "lastCommitTime: %{public}" PRId64, param.vsyncId, delayTime_, currCommitTime, lastCommitTime_);
        RS_TRACE_NAME_FMT("update delayTime: %" PRId64 ", currCommitTime: %" PRId64 ", lastCommitTime: %" PRId64 "",
            delayTime_, currCommitTime, lastCommitTime_);
    }
    if (delayTime_ < 0 || delayTime_ >= MAX_DELAY_TIME) {
        delayTime_ = 0;
    }
    lastCommitTime_ = currTime + delayTime_ * NS_MS_UNIT_CONVERSION;
    EndCheck(timer);
    PostDelayTask(task, delayTime_);
}

void RSHardwareThread::EndCheck(RSTimer timer)
{
    exceptionCheck_.pid_ = getpid();
    exceptionCheck_.uid_ = getuid();
    exceptionCheck_.processName_ = PROCESS_NAME_FOR_HISYSEVENT;
    exceptionCheck_.exceptionPoint_ = HARDWARE_PIPELINE_TIMEOUT;

    if (timer.GetDuration() >= HARDWARE_TIMEOUT) {
        if (++hardwareCount_ == HARDWARE_TIMEOUT_CNT) {
            RS_LOGE("Hardware Thread Exception Count[%{public}d]", hardwareCount_);
            exceptionCheck_.exceptionCnt_ = hardwareCount_;
            exceptionCheck_.exceptionMoment_ = timer.GetSeconds();
            exceptionCheck_.UploadRenderExceptionData();
        }
    } else {
        hardwareCount_ = 0;
    }
    if (hardwareCount_ == HARDWARE_TIMEOUT_ABORT_CNT) {
        exceptionCheck_.exceptionCnt_ = hardwareCount_;
        exceptionCheck_.exceptionMoment_ = timer.GetSeconds();
        exceptionCheck_.UploadRenderExceptionData();
        sleep(1); // sleep 1s : abort will kill RS, sleep 1s for hisysevent report.
        abort(); // The RS process needs to be restarted because 30 consecutive frames in hardware times out.
    }
}

void RSHardwareThread::ChangeLayersForActiveRectOutside(std::vector<LayerInfoPtr>& layers, ScreenId screenId)
{
#ifdef ROSEN_EMULATOR
    RS_LOGD("emulator device do not need add layer");
    return;
#endif
    if (!RSSystemProperties::IsSuperFoldDisplay() || layers.size() == 0) {
        return;
    }
    auto screenManager = CreateOrGetScreenManager();
    if (!screenManager) {
        return;
    }
    auto screenInfo = screenManager->QueryScreenInfo(screenId);
    const RectI& reviseRect = screenInfo.reviseRect;
    if (reviseRect.width_ <= 0 || reviseRect.height_ <= 0) {
        return;
    }
    const RectI& maskRect = screenInfo.maskRect;
    if (maskRect.width_ > 0 && maskRect.height_ > 0) {
        auto solidColorLayer = HdiLayerInfo::CreateHdiLayerInfo();
        solidColorLayer->SetZorder(INT_MAX - 1);
        solidColorLayer->SetTransform(GraphicTransformType::GRAPHIC_ROTATE_NONE);
        GraphicIRect dstRect = {maskRect.left_, maskRect.top_, maskRect.width_, maskRect.height_};
        solidColorLayer->SetLayerSize(dstRect);
        solidColorLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_SOLID_COLOR);
        bool debugFlag = (system::GetParameter("debug.foldscreen.shaft.color", "0") == "1");
        if (debugFlag) {
            solidColorLayer->SetLayerColor({0, 255, 0, 255});
        } else {
            solidColorLayer->SetLayerColor({0, 0, 0, 255});
        }
        layers.emplace_back(solidColorLayer);
    }
    using RSRcdManager = RSSingleton<RoundCornerDisplayManager>;
    for (auto& layerInfo : layers) {
        auto layerSurface = layerInfo->GetSurface();
        if (layerSurface != nullptr) {
            auto rcdlayerInfo = RSRcdManager::GetInstance().GetLayerPair(layerSurface->GetName());
            if (rcdlayerInfo.second != RoundCornerDisplayManager::RCDLayerType::INVALID) {
                continue;
            }
        }
        GraphicIRect dstRect = layerInfo->GetLayerSize();
        GraphicIRect tmpRect = dstRect;
        int reviseRight = reviseRect.left_ + reviseRect.width_;
        int reviseBottom = reviseRect.top_ + reviseRect.height_;
        tmpRect.x = std::clamp(dstRect.x, reviseRect.left_, reviseRight);
        tmpRect.y = std::clamp(dstRect.y, reviseRect.top_, reviseBottom);
        tmpRect.w = std::min(tmpRect.x + dstRect.w, reviseRight) - tmpRect.x;
        tmpRect.h = std::min(tmpRect.y + dstRect.h, reviseBottom) - tmpRect.y;
        layerInfo->SetLayerSize(tmpRect);
    }
}

std::string RSHardwareThread::GetSurfaceNameInLayers(const std::vector<LayerInfoPtr>& layers)
{
    std::string surfaceName = "SurfaceName: [";
    bool isFirst = true;
    for (const auto& layer : layers) {
        if (layer == nullptr || layer->GetSurface() == nullptr) {
            continue;
        }
        if (isFirst) {
            surfaceName += layer->GetSurface()->GetName();
            isFirst = false;
            continue;
        }
        surfaceName += ", " + layer->GetSurface()->GetName();
    }
    surfaceName += "]";
    return surfaceName;
}

std::string RSHardwareThread::GetSurfaceNameInLayersForTrace(const std::vector<LayerInfoPtr>& layers)
{
    uint32_t count = layers.size();
    for (const auto& layer : layers) {
        if (layer == nullptr || layer->GetSurface() == nullptr) {
            continue;
        }
        count += layer->GetSurface()->GetName().length();
    }
    bool exceedLimit = count > MAX_TOTAL_SURFACE_NAME_LENGTH;
    std::string surfaceName = "Names:";
    for (const auto& layer : layers) {
        if (layer == nullptr || layer->GetSurface() == nullptr) {
            continue;
        }
        surfaceName += (exceedLimit ? layer->GetSurface()->GetName().substr(0, MAX_SINGLE_SURFACE_NAME_LENGTH) :
                                      layer->GetSurface()->GetName()) + ",";
    }
    return surfaceName;
}

void RSHardwareThread::RecordTimestamp(const std::vector<LayerInfoPtr>& layers)
{
    uint64_t currentTime = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count());
    for (auto& layer : layers) {
        if (layer == nullptr) {
            continue;
        }
        uint64_t id = layer->GetNodeId();
        auto& surfaceFpsManager = RSSurfaceFpsManager::GetInstance();
        if (layer->GetBuffer() == nullptr) {
            continue;
        }
        surfaceFpsManager.RecordPresentTime(id, currentTime, layer->GetBuffer()->GetSeqNum());
    }
}

bool RSHardwareThread::IsDelayRequired(OHOS::Rosen::HgmCore& hgmCore, RefreshRateParam param,
    const OutputPtr& output, bool hasGameScene)
{
    if (param.isForceRefresh) {
        RS_LOGD("CommitAndReleaseLayers in Force Refresh");
        RS_TRACE_NAME("CommitAndReleaseLayers in Force Refresh");
        return false;
    }

    if (hgmCore.GetLtpoEnabled()) {
        if (AdaptiveModeStatus(output) == SupportASStatus::SUPPORT_AS) {
            RS_LOGD("CommitAndReleaseLayers in Adaptive Mode");
            RS_TRACE_NAME("CommitAndReleaseLayers in Adaptive Mode");
            return false;
        }
        if (hasGameScene && AdaptiveModeStatus(output) == SupportASStatus::GAME_SCENE_SKIP) {
            RS_LOGD("CommitAndReleaseLayers skip delayTime Calculation");
            RS_TRACE_NAME("CommitAndReleaseLayers in Game Scene and skiped delayTime Calculation");
            return false;
        }
    } else {
        if (!hgmCore.IsDelayMode()) {
            return false;
        }
        if (hasGameScene) {
            RS_LOGD("CommitAndReleaseLayers in Game Scene");
            RS_TRACE_NAME("CommitAndReleaseLayers in Game Scene");
            return false;
        }
    }
    return true;
}

void RSHardwareThread::CalculateDelayTime(OHOS::Rosen::HgmCore& hgmCore, RefreshRateParam param, uint32_t currentRate,
    int64_t currTime)
{
    int64_t frameOffset = 0;
    int64_t vsyncOffset = 0;
    int64_t idealPipelineOffset = 0;
    int64_t pipelineOffset = 0;
    int64_t expectCommitTime = 0;
    int64_t periodNum = 0;
    int64_t idealPeriod = hgmCore.GetIdealPeriod(currentRate);
    int64_t period  = CreateVSyncSampler()->GetHardwarePeriod();
    uint64_t dvsyncOffset = RSMainThread::Instance()->GetRealTimeOffsetOfDvsync(param.frameTimestamp);
    int64_t compositionTime = period;

    if (!hgmCore.GetLtpoEnabled()) {
        vsyncOffset = UNI_RENDER_VSYNC_OFFSET_DELAY_MODE;
        // 2 period for draw and composition, pipelineOffset = 2 * period
        frameOffset = 2 * period + vsyncOffset - static_cast<int64_t>(param.fastComposeTimeStampDiff);
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
            static_cast<int64_t>(dvsyncOffset) - static_cast<int64_t>(param.fastComposeTimeStampDiff);
    }
    expectCommitTime = param.actualTimestamp + frameOffset - compositionTime - RESERVE_TIME;
    int64_t diffTime = expectCommitTime - currTime;
    if (diffTime > 0 && period > 0) {
        delayTime_ = std::round(diffTime * 1.0f / NS_MS_UNIT_CONVERSION);
    }
    RS_TRACE_NAME_FMT("CalculateDelayTime pipelineOffset: %" PRId64 ", actualTimestamp: %" PRId64 ", " \
        "expectCommitTime: %" PRId64 ", currTime: %" PRId64 ", diffTime: %" PRId64 ", delayTime: %" PRId64 ", " \
        "frameOffset: %" PRId64 ", dvsyncOffset: %" PRIu64 ", vsyncOffset: %" PRId64 ", idealPeriod: %" PRId64 ", " \
        "period: %" PRId64 ", idealPipelineOffset: %" PRId64 ", fastComposeTimeStampDiff: %" PRIu64 "",
        pipelineOffset, param.actualTimestamp, expectCommitTime, currTime, diffTime, delayTime_,
        frameOffset, dvsyncOffset, vsyncOffset, idealPeriod, period,
        idealPipelineOffset, param.fastComposeTimeStampDiff);
    RS_LOGD_IF(DEBUG_PIPELINE, "CalculateDelayTime period:%{public}" PRId64 " delayTime:%{public}" PRId64 "", period,
        delayTime_);
}

int64_t RSHardwareThread::GetCurTimeCount()
{
    auto curTime = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::microseconds>(curTime).count();
}

void RSHardwareThread::PreAllocateProtectedBuffer(sptr<SurfaceBuffer> buffer, uint64_t screenId)
{
    RS_TRACE_NAME("RSHardwareThread::PreAllocateProtectedBuffer enter.");
    {
        std::unique_lock<std::mutex> lock(preAllocMutex_, std::try_to_lock);
        if (!lock.owns_lock()) {
            RS_TRACE_NAME("RSHardwareThread::PreAllocateProtectedBuffer failed, HardwareThread is redraw.");
            return;
        }
    }
    auto screenManager = CreateOrGetScreenManager();
    if (screenManager == nullptr) {
        RS_LOGE("screenManager is NULL");
        return;
    }
    auto output = screenManager->GetOutput(ToScreenPhysicalId(screenId));
    if (output == nullptr) {
        RS_LOGE("output is NULL");
        return;
    }
    if (output->GetProtectedFrameBufferState()) {
        return;
    }
    auto fbSurface = output->GetFrameBufferSurface();
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
    auto screenInfo = screenManager->QueryScreenInfo(screenId);
    auto ret = rsSurface->PreAllocateProtectedBuffer(screenInfo.phyWidth, screenInfo.phyHeight);
    output->SetProtectedFrameBufferState(ret);
#endif
}

GraphicColorGamut RSHardwareThread::ComputeTargetColorGamut(const sptr<SurfaceBuffer> &buffer)
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

GraphicPixelFormat RSHardwareThread::ComputeTargetPixelFormat(const sptr<SurfaceBuffer> &buffer)
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

int32_t RSHardwareThread::AdaptiveModeStatus(const OutputPtr &output)
{
    if (hdiBackend_ == nullptr) {
        RS_LOGE("AdaptiveModeStatus hdiBackend_ is nullptr");
        return SupportASStatus::NOT_SUPPORT;
    }

    auto& hgmCore = OHOS::Rosen::HgmCore::Instance();

    // if in game adaptive vsync mode and do direct composition,send layer immediately
    auto frameRateMgr = hgmCore.GetFrameRateMgr();
    if (frameRateMgr != nullptr) {
        int32_t adaptiveStatus = frameRateMgr->AdaptiveStatus();
        RS_LOGD("CommitAndReleaseLayers send layer adaptiveStatus: %{public}u", adaptiveStatus);
        if (adaptiveStatus == SupportASStatus::SUPPORT_AS) {
            return SupportASStatus::SUPPORT_AS;
        }
        if (adaptiveStatus == SupportASStatus::GAME_SCENE_SKIP) {
            return SupportASStatus::GAME_SCENE_SKIP;
        }
    }
    return SupportASStatus::NOT_SUPPORT;
}

RefreshRateParam RSHardwareThread::GetRefreshRateParam()
{
    // need to sync the hgm data from main thread.
    // Temporary sync the timestamp to fix the duplicate time stamp issue.
    auto& hgmCore = OHOS::Rosen::HgmCore::Instance();
    bool directComposition = hgmCore.GetDirectCompositionFlag();
    RS_LOGI_IF(DEBUG_COMPOSER, "GetRefreshRateData period is %{public}d", directComposition);
    if (directComposition) {
        hgmCore.SetDirectCompositionFlag(false);
    }
    RefreshRateParam param;
    if (directComposition) {
        param = {
            .rate = hgmCore.GetPendingScreenRefreshRate(),
            .frameTimestamp = hgmCore.GetCurrentTimestamp(),
            .actualTimestamp = hgmCore.GetActualTimestamp(),
            .vsyncId = hgmCore.GetVsyncId(),
            .constraintRelativeTime = hgmCore.GetPendingConstraintRelativeTime(),
            .isForceRefresh = hgmCore.GetForceRefreshFlag(),
            .fastComposeTimeStampDiff = hgmCore.GetFastComposeTimeStampDiff()
        };
    } else {
        param = {
            .rate = RSUniRenderThread::Instance().GetPendingScreenRefreshRate(),
            .frameTimestamp = RSUniRenderThread::Instance().GetCurrentTimestamp(),
            .actualTimestamp = RSUniRenderThread::Instance().GetActualTimestamp(),
            .vsyncId = RSUniRenderThread::Instance().GetVsyncId(),
            .constraintRelativeTime = RSUniRenderThread::Instance().GetPendingConstraintRelativeTime(),
            .isForceRefresh = RSUniRenderThread::Instance().GetForceRefreshFlag(),
            .fastComposeTimeStampDiff = RSUniRenderThread::Instance().GetFastComposeTimeStampDiff()
        };
    }
    return param;
}

void RSHardwareThread::OnScreenVBlankIdleCallback(ScreenId screenId, uint64_t timestamp)
{
    RS_TRACE_NAME_FMT("RSHardwareThread::OnScreenVBlankIdleCallback screenId: %" PRIu64" now: %" PRIu64"",
        screenId, timestamp);
    vblankIdleCorrector_.SetScreenVBlankIdle(screenId);
}

bool RSHardwareThread::IsDropDirtyFrame(OutputPtr output)
{
    if (!RSSystemProperties::IsSuperFoldDisplay()) {
        return false;
    }
    if (output == nullptr) {
        RS_LOGW("%{public}s: output is null", __func__);
        return false;
    }
    auto screenManager = CreateOrGetScreenManager();
    if (screenManager == nullptr) {
        RS_LOGW("%{public}s: screenManager is null", __func__);
        return false;
    }

    auto screenId = output->GetScreenId();
    auto rect = screenManager->QueryScreenInfo(screenId).activeRect;
    if (rect.IsEmpty()) {
        RS_LOGW("%{public}s: activeRect is empty", __func__);
        return false;
    }
    GraphicIRect activeRect = {rect.left_, rect.top_, rect.width_, rect.height_};
    std::vector<LayerInfoPtr> layerInfos;
    output->GetLayerInfos(layerInfos);
    if (layerInfos.empty()) {
        RS_LOGI("%{public}s: layerInfos is empty", __func__);
        return false;
    }
    for (const auto& info : layerInfos) {
        auto layerSize = info->GetLayerSize();
        if (info->GetDisplayNodeFlag() && !(activeRect == layerSize)) {
            RS_LOGI("%{publkic}s: Drop dirty frame cause activeRect:[%{public}d, %{public}d, %{public}d, %{public}d]" \
                "layerSize:[%{public}d, %{public}d, %{public}d, %{public}d]", __func__, activeRect.x, activeRect.y,
                activeRect.w, activeRect.h, layerSize.x, layerSize.y, layerSize.w, layerSize.h);
            return true;
        }
    }
    return false;
}

void RSHardwareThread::ExecuteSwitchRefreshRate(const OutputPtr& output, uint32_t refreshRate)
{
    static bool refreshRateSwitch = system::GetBoolParameter("persist.hgm.refreshrate.enabled", true);
    if (!refreshRateSwitch) {
        RS_LOGD("refreshRateSwitch is off, currRefreshRate is %{public}d", refreshRate);
        return;
    }

    auto& hgmCore = OHOS::Rosen::HgmCore::Instance();
    if (hgmCore.GetFrameRateMgr() == nullptr) {
        RS_LOGD("FrameRateMgr is null");
        return;
    }
    ScreenId id = output->GetScreenId();
    outputMap_[id] = output;
    auto screen = hgmCore.GetScreen(id);
    if (!screen || !screen->GetSelfOwnedScreenFlag()) {
        return;
    }
    auto screenRefreshRateImme = hgmCore.GetScreenRefreshRateImme();
    if (screenRefreshRateImme > 0) {
        RS_LOGD("ExecuteSwitchRefreshRate:rate change: %{public}u -> %{public}u", refreshRate, screenRefreshRateImme);
        refreshRate = screenRefreshRateImme;
    }
    ScreenId curScreenId = hgmCore.GetFrameRateMgr()->GetCurScreenId();
    ScreenId lastCurScreenId = hgmCore.GetFrameRateMgr()->GetLastCurScreenId();
    hgmCore.SetScreenSwitchDssEnable(id, true);
    if (refreshRate != hgmCore.GetScreenCurrentRefreshRate(id) || lastCurScreenId != curScreenId ||
        needRetrySetRate_) {
        RS_LOGD("CommitAndReleaseLayers screenId %{public}d refreshRate %{public}d \
            needRetrySetRate %{public}d", static_cast<int>(id), refreshRate, needRetrySetRate_);
        int32_t sceneId = (lastCurScreenId != curScreenId || needRetrySetRate_) ? SWITCH_SCREEN_SCENE : 0;
        hgmCore.GetFrameRateMgr()->SetLastCurScreenId(curScreenId);
        int32_t status = hgmCore.SetScreenRefreshRate(id, sceneId, refreshRate);
        needRetrySetRate_ = false;
        if (status < EXEC_SUCCESS) {
            RS_LOGD("RSHardwareThread: failed to set refreshRate %{public}d, screenId %{public}" PRIu64 "", refreshRate,
                id);
        }
    }
}

void RSHardwareThread::PerformSetActiveMode(OutputPtr output, uint64_t timestamp, uint64_t constraintRelativeTime)
{
    auto &hgmCore = OHOS::Rosen::HgmCore::Instance();
    auto screenManager = CreateOrGetScreenManager();
    if (screenManager == nullptr) {
        return;
    }

    vblankIdleCorrector_.ProcessScreenConstraint(timestamp, constraintRelativeTime);
    HgmRefreshRates newRate = RSSystemProperties::GetHgmRefreshRatesEnabled();
    if (hgmRefreshRates_ != newRate) {
        hgmRefreshRates_ = newRate;
        hgmCore.SetScreenRefreshRate(screenManager->GetDefaultScreenId(), 0, static_cast<int32_t>(hgmRefreshRates_));
    }

    std::unique_ptr<std::unordered_map<ScreenId, int32_t>> modeMap(hgmCore.GetModesToApply());
    if (modeMap == nullptr) {
        return;
    }

    RS_TRACE_NAME_FMT("RSHardwareThread::PerformSetActiveMode setting active mode. rate: %d",
        HgmCore::Instance().GetScreenCurrentRefreshRate(HgmCore::Instance().GetActiveScreenId()));
    for (auto mapIter = modeMap->begin(); mapIter != modeMap->end(); ++mapIter) {
        ScreenId id = mapIter->first;
        int32_t modeId = mapIter->second;

        auto supportedModes = screenManager->GetScreenSupportedModes(id);
        for (auto mode : supportedModes) {
            RS_OPTIONAL_TRACE_NAME_FMT("RSHardwareThread check modes w: %" PRId32", h: %" PRId32", rate: %" PRId32", id: %" PRId32"",
                mode.GetScreenWidth(), mode.GetScreenHeight(), mode.GetScreenRefreshRate(), mode.GetScreenModeId());
        }

        uint32_t ret = screenManager->SetScreenActiveMode(id, modeId);
        needRetrySetRate_ = (ret == StatusCode::SET_RATE_ERROR);
        RS_LOGD_IF(needRetrySetRate_, "RSHardwareThread: need retry set modeId %{public}d", modeId);

        auto pendingPeriod = hgmCore.GetIdealPeriod(hgmCore.GetScreenCurrentRefreshRate(id));
        int64_t pendingTimestamp = static_cast<int64_t>(timestamp);
        if (hdiBackend_) {
            hdiBackend_->SetPendingMode(output, pendingPeriod, pendingTimestamp);
            hdiBackend_->StartSample(output);
        }
    }
}

void RSHardwareThread::ChangeDssRefreshRate(ScreenId screenId, uint32_t refreshRate, bool followPipline)
{
    if (followPipline) {
        auto& hgmCore = OHOS::Rosen::HgmCore::Instance();
        auto task = [this, screenId, refreshRate, vsyncId = refreshRateParam_.vsyncId] () {
            if (vsyncId != refreshRateParam_.vsyncId || !HgmCore::Instance().IsSwitchDssEnable(screenId)) {
                return;
            }
            // switch hardware vsync
            ChangeDssRefreshRate(screenId, refreshRate, false);
        };
        int64_t period = hgmCore.GetIdealPeriod(hgmCore.GetScreenCurrentRefreshRate(screenId));
        PostDelayTask(task, period / NS_MS_UNIT_CONVERSION + delayTime_ + DELAY_TIME_OFFSET);
    } else {
        auto outputIter = outputMap_.find(screenId);
        if (outputIter == outputMap_.end()) {
            return;
        }
        auto output = outputIter->second.lock();
        if (output == nullptr) {
            outputMap_.erase(screenId);
            return;
        }
        if (HgmCore::Instance().GetActiveScreenId() != screenId) {
            return;
        }
        ExecuteSwitchRefreshRate(output, refreshRate);
        PerformSetActiveMode(
            output, refreshRateParam_.frameTimestamp, refreshRateParam_.constraintRelativeTime);
        if (output->IsDeviceValid()) {
            hdiBackend_->Repaint(output);
        }
    }
}

void RSHardwareThread::OnPrepareComplete(sptr<Surface>& surface,
    const PrepareCompleteParam& param, void* data)
{
    // unused data.
    (void)(data);

    if (!param.needFlushFramebuffer) {
        return;
    }

    if (redrawCb_ != nullptr) {
        redrawCb_(surface, param.layers, param.screenId);
    }
}

GSError RSHardwareThread::ClearFrameBuffers(OutputPtr output)
{
    if (output == nullptr) {
        RS_LOGE("Clear frame buffers failed for the output is nullptr");
        return GSERROR_INVALID_ARGUMENTS;
    }
    RS_TRACE_NAME("RSHardwareThread::ClearFrameBuffers");
    if (uniRenderEngine_ != nullptr) {
        uniRenderEngine_->ResetCurrentContext();
    }
    auto surface = output->GetFrameBufferSurface();
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
    return output->ClearFrameBuffer();
}

std::shared_ptr<RSSurfaceOhos> RSHardwareThread::CreateFrameBufferSurfaceOhos(const sptr<Surface>& surface)
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

void RSHardwareThread::RedrawScreenRCD(RSPaintFilterCanvas& canvas, const std::vector<LayerInfoPtr>& layers)
{
    RS_TRACE_NAME("RSHardwareThread::RedrawScreenRCD");
    using RSRcdManager = RSSingleton<RoundCornerDisplayManager>;
    std::vector<std::pair<NodeId, RoundCornerDisplayManager::RCDLayerType>> rcdLayerInfoList;
    for (const auto& layer : layers) {
        if (layer == nullptr) {
            continue;
        }
        if (layer->GetCompositionType() == GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE ||
            layer->GetCompositionType() == GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE_CLEAR ||
            layer->GetCompositionType() == GraphicCompositionType::GRAPHIC_COMPOSITION_SOLID_COLOR) {
            continue;
        }
        auto layerSurface = layer->GetSurface();
        if (layerSurface != nullptr) {
            auto rcdlayerInfo = RSRcdManager::GetInstance().GetLayerPair(layerSurface->GetName());
            if (rcdlayerInfo.second != RoundCornerDisplayManager::RCDLayerType::INVALID) {
                rcdLayerInfoList.push_back(rcdlayerInfo);
                continue;
            }
        } else {
            RS_LOGE("RedrawScreenRCD layerSurface is nullptr");
            continue;
        }
    }

    if (RSRcdManager::GetInstance().GetRcdEnable()) {
        RSRcdManager::GetInstance().DrawRoundCorner(rcdLayerInfoList, &canvas);
    }
}

void RSHardwareThread::Redraw(const sptr<Surface>& surface, const std::vector<LayerInfoPtr>& layers, uint32_t screenId)
{
    RS_TRACE_NAME("RSHardwareThread::Redraw");
    std::unique_lock<std::mutex> lock(preAllocMutex_, std::try_to_lock);
    auto screenManager = CreateOrGetScreenManager();
    if (screenManager == nullptr) {
        RS_LOGE("Redraw: screenManager is null.");
        return;
    }
    if (surface == nullptr || uniRenderEngine_ == nullptr) {
        RS_LOGE("Redraw: surface or uniRenderEngine is null.");
        return;
    }
    bool isProtected = false;

    static bool isCCMDrmEnabled = DRMParam::IsDrmEnable();
    bool isDrmEnabled = RSSystemProperties::GetDrmEnabled() && isCCMDrmEnabled;

#ifdef RS_ENABLE_VK
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        if (isDrmEnabled) {
            for (const auto& layer : layers) {
                if (layer && layer->GetBuffer() && (layer->GetBuffer()->GetUsage() & BUFFER_USAGE_PROTECTED)) {
                    isProtected = true;
                    break;
                }
            }
            if (RSSystemProperties::IsUseVulkan()) {
                RsVulkanContext::GetSingleton().SetIsProtected(isProtected);
            }
        } else {
            RsVulkanContext::GetSingleton().SetIsProtected(false);
        }
    }
#endif

    RS_LOGD("RsDebug Redraw flush frame buffer start");
    bool forceCPU = RSBaseRenderEngine::NeedForceCPU(layers);
    auto screenInfo = screenManager->QueryScreenInfo(screenId);
    std::shared_ptr<Drawing::ColorSpace> drawingColorSpace = nullptr;
#ifdef USE_VIDEO_PROCESSING_ENGINE
    GraphicColorGamut colorGamut = ComputeTargetColorGamut(layers);
    GraphicPixelFormat pixelFormat = ComputeTargetPixelFormat(layers);
    RS_LOGD("Redraw computed target color gamut: %{public}d,"
        "pixel format: %{public}d, frame width: %{public}d, frame height: %{public}d",
        colorGamut, pixelFormat, screenInfo.phyWidth, screenInfo.phyHeight);
    auto renderFrameConfig = RSBaseRenderUtil::GetFrameBufferRequestConfig(screenInfo,
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
    auto renderFrameConfig = RSBaseRenderUtil::GetFrameBufferRequestConfig(screenInfo, isProtected);
#endif
    // override redraw frame buffer with physical screen resolution.
    renderFrameConfig.width = static_cast<int32_t>(screenInfo.phyWidth);
    renderFrameConfig.height = static_cast<int32_t>(screenInfo.phyHeight);

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
    FrameContextConfig frameContextConfig = {isProtected, false};
#ifdef RS_ENABLE_VKQUEUE_PRIORITY
    frameContextConfig.independentContext = RSSystemProperties::GetVkQueuePriorityEnable();
#endif
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
    uniRenderEngine_->DrawLayers(*canvas, layers, false, screenInfo, colorGamut);
#else
    uniRenderEngine_->DrawLayers(*canvas, layers, false, screenInfo);
#endif
    RedrawScreenRCD(*canvas, layers);
    renderFrame->Flush();
    RS_LOGD("RsDebug Redraw flush frame buffer end");
}

void RSHardwareThread::AddRefreshRateCount(const OutputPtr& output)
{
    auto screenManager = CreateOrGetScreenManager();
    if (screenManager == nullptr) {
        RS_LOGE("AddRefreshRateCount screenManager is nullptr");
        return;
    }
    ScreenId id = screenManager->GetDefaultScreenId();
    auto& hgmCore = OHOS::Rosen::HgmCore::Instance();
    uint32_t currentRefreshRate = hgmCore.GetScreenCurrentRefreshRate(id);
    auto [iter, success] = refreshRateCounts_.try_emplace(currentRefreshRate, 1);
    if (!success) {
        iter->second++;
    }
    if (output == nullptr) {
        return;
    }
    RSRealtimeRefreshRateManager::Instance().CountRealtimeFrame(output->GetScreenId());
    auto frameRateMgr = HgmCore::Instance().GetFrameRateMgr();
    if (frameRateMgr == nullptr) {
        RS_LOGE("AddRefreshData fail, frameBufferSurfaceOhos_ is nullptr");
        return;
    }
    frameRateMgr->HandleRsFrame();
}

void RSHardwareThread::SubScribeSystemAbility()
{
    RS_LOGI("%{public}s", __func__);
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityManager) {
        RS_LOGE("%{public}s failed to get system ability manager client", __func__);
        return;
    }
    std::string threadName = "RSHardwareThread";
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

void RSHardwareThread::DumpEventQueue()
{
    if (handler_ != nullptr) {
        AppExecFwk::RSHardwareDumper dumper;
        handler_->Dump(dumper);
        dumper.PrintDumpInfo();
    }
}

#ifdef USE_VIDEO_PROCESSING_ENGINE
GraphicColorGamut RSHardwareThread::ComputeTargetColorGamut(const std::vector<LayerInfoPtr>& layers)
{
    using namespace HDI::Display::Graphic::Common::V1_0;
    GraphicColorGamut colorGamut = GRAPHIC_COLOR_GAMUT_SRGB;
    for (auto& layer : layers) {
        if (layer == nullptr) {
            RS_LOGE("ComputeTargetColorGamut layer is nullptr");
            continue;
        }
        auto buffer = layer->GetBuffer();
        if (buffer == nullptr) {
            RS_LOGW("ComputeTargetColorGamut The buffer of layer is nullptr");
            continue;
        }

        CM_ColorSpaceInfo colorSpaceInfo;
        if (MetadataHelper::GetColorSpaceInfo(buffer, colorSpaceInfo) != GSERROR_OK) {
            RS_LOGD("ComputeTargetColorGamut Get color space failed");
            continue;
        }

        if (colorSpaceInfo.primaries != COLORPRIMARIES_SRGB) {
            RS_LOGD("ComputeTargetColorData fail, primaries is %{public}d",
                colorSpaceInfo.primaries);
            colorGamut = GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
            break;
        }
    }

    return colorGamut;
}

GraphicPixelFormat RSHardwareThread::ComputeTargetPixelFormat(const std::vector<LayerInfoPtr>& layers)
{
    using namespace HDI::Display::Graphic::Common::V1_0;
    GraphicPixelFormat pixelFormat = GRAPHIC_PIXEL_FMT_RGBA_8888;
    for (auto& layer : layers) {
        if (layer == nullptr) {
            continue;
        }
        auto buffer = layer->GetBuffer();
        if (buffer == nullptr) {
            RS_LOGW("ComputeTargetPixelFormat The buffer of layer is nullptr");
            continue;
        }

        auto bufferPixelFormat = buffer->GetFormat();
        if (bufferPixelFormat == GRAPHIC_PIXEL_FMT_RGBA_1010102 ||
            bufferPixelFormat == GRAPHIC_PIXEL_FMT_YCBCR_P010 ||
            bufferPixelFormat == GRAPHIC_PIXEL_FMT_YCRCB_P010) {
            pixelFormat = GRAPHIC_PIXEL_FMT_RGBA_1010102;
            RS_LOGD("ComputeTargetPixelFormat pixelformat is set to 1010102 for 10bit buffer");
            break;
        }
    }

    return pixelFormat;
}

bool RSHardwareThread::ConvertColorGamutToSpaceType(const GraphicColorGamut& colorGamut,
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

#ifdef RS_ENABLE_VK
void RSHardwareThread::ContextRegisterPostTask()
{
    RsVulkanContext::GetSingleton().SetIsProtected(true);
    auto context = RsVulkanContext::GetSingleton().GetDrawingContext();
    if (context) {
        context->RegisterPostFunc([this](const std::function<void()>& task) { PostTask(task); });
    }
    RsVulkanContext::GetSingleton().SetIsProtected(false);
    if (context) {
        context->RegisterPostFunc([this](const std::function<void()>& task) { PostTask(task); });
    }
}
#endif
}

namespace OHOS {
namespace AppExecFwk {
void RSHardwareDumper::Dump(const std::string& message)
{
    dumpInfo_ += message;
}

std::string RSHardwareDumper::GetTag()
{
    return "RSHardwareDumper";
}

void RSHardwareDumper::PrintDumpInfo()
{
    if (dumpInfo_.empty()) {
        return;
    }
    size_t compareStrSize = sizeof("}\n");

    size_t curRunningStart = dumpInfo_.find("Current Running: start");
    if (curRunningStart != std::string::npos) {
        size_t curRunningEnd = dumpInfo_.find("}\n", curRunningStart);
        if (curRunningEnd != std::string::npos) {
            RS_LOGE("%{public}s",
                dumpInfo_.substr(curRunningStart, curRunningEnd - curRunningStart + compareStrSize).c_str());
        }
    }

    size_t immediateStart = dumpInfo_.find("Immediate priority event queue information:");
    if (immediateStart != std::string::npos) {
        size_t immediateEnd = dumpInfo_.find("}\n", immediateStart);
        if (immediateEnd != std::string::npos) {
            RS_LOGE("%{public}s",
                dumpInfo_.substr(immediateStart, immediateEnd - immediateStart + compareStrSize).c_str());
        }
    }
}

} // namespace AppExecFwk
} // namespace OHOS