/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "pipeline/rs_hardware_thread.h"

#include <memory>
#include <unistd.h>

#include "frame_report.h"
#include "hdi_backend.h"
#include "hgm_frame_rate_manager.h"
#include "hisysevent.h"
#include "parameters.h"
#include "rs_realtime_refresh_rate_manager.h"
#include "rs_trace.h"
#include "vsync_sampler.h"

#include "common/rs_optional_trace.h"
#include "common/rs_singleton.h"
#include "pipeline/round_corner_display/rs_round_corner_display_manager.h"
#include "pipeline/rs_base_render_util.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_uni_render_engine.h"
#include "pipeline/rs_uni_render_thread.h"
#include "pipeline/rs_uni_render_util.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "platform/ohos/backend/rs_surface_ohos_gl.h"
#include "platform/ohos/backend/rs_surface_ohos_raster.h"
#include "screen_manager/rs_screen_manager.h"

#ifdef RS_ENABLE_EGLIMAGE
#include "src/gpu/gl/GrGLDefines.h"
#endif

#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_surface_ohos_vulkan.h"
#include "rs_vk_image_manager.h"
#endif

#ifdef RS_ENABLE_EGLIMAGE
#include "rs_egl_image_manager.h"
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
    RS_LOGI("RSHardwareThread::Start()!");
    hdiBackend_ = HdiBackend::GetInstance();
    runner_ = AppExecFwk::EventRunner::Create("RSHardwareThread");
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    redrawCb_ = [this](const sptr<Surface>& surface, const std::vector<LayerInfoPtr>& layers, uint32_t screenId) {
        return this->Redraw(surface, layers, screenId);
    };
    if (handler_) {
        ScheduleTask(
            [this]() {
                auto screenManager = CreateOrGetScreenManager();
                if (screenManager == nullptr || !screenManager->Init()) {
                    RS_LOGE("RSHardwareThread CreateOrGetScreenManager or init fail.");
                    return;
                }
#ifdef RES_SCHED_ENABLE
                SubScribeSystemAbility();
#endif
                uniRenderEngine_ = std::make_shared<RSUniRenderEngine>();
#ifdef RS_ENABLE_VK
                if (RSSystemProperties::IsUseVulkan()) {
                    RsVulkanContext::GetSingleton().SetIsProtected(true);
                }
#endif
                uniRenderEngine_->Init(true);
                hardwareTid_ = gettid();
            }).wait();
    }
    auto onPrepareCompleteFunc = [this](auto& surface, const auto& param, void* data) {
        OnPrepareComplete(surface, param, data);
    };
    if (hdiBackend_ != nullptr) {
        hdiBackend_->RegPrepareComplete(onPrepareCompleteFunc, this);
    }
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

void RSHardwareThread::PostDelayTask(const std::function<void()>& task, int64_t delayTime)
{
    if (handler_) {
        handler_->PostTask(task, delayTime, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

uint32_t RSHardwareThread::GetunExecuteTaskNum()
{
    return unExecuteTaskNum_.load();
}

void RSHardwareThread::RefreshRateCounts(std::string& dumpString)
{
    if (refreshRateCounts_.empty()) {
        RS_LOGE("RSHardwareThread::RefreshData fail, refreshRateCounts_ is empty");
        return;
    }
    std::map<uint32_t, uint64_t>::iterator iter;
    for (iter = refreshRateCounts_.begin(); iter != refreshRateCounts_.end(); ++iter) {
        dumpString.append(
            "Refresh Rate:" + std::to_string(iter->first) + ", Count:" + std::to_string(iter->second) + ";\n");
    }
    RS_LOGD("RSHardwareThread::RefreshRateCounts refresh rate counts info is displayed");
}

void RSHardwareThread::ClearRefreshRateCounts(std::string& dumpString)
{
    if (refreshRateCounts_.empty()) {
        RS_LOGE("RSHardwareThread::ClearRefreshData fail, refreshRateCounts_ is empty");
        return;
    }
    refreshRateCounts_.clear();
    dumpString.append("The refresh rate counts info is cleared successfully!\n");
    RS_LOGD("RSHardwareThread::RefreshRateCounts refresh rate counts info is cleared");
}

void RSHardwareThread::CommitAndReleaseLayers(OutputPtr output, const std::vector<LayerInfoPtr>& layers)
{
    if (!handler_) {
        RS_LOGE("RSHardwareThread::CommitAndReleaseLayers handler is nullptr");
        return;
    }
    delayTime_ = 0;
    LayerComposeCollection::GetInstance().UpdateUniformOrOfflineComposeFrameNumberForDFX(layers.size());
    RefreshRateParam param = GetRefreshRateParam();
    auto& hgmCore = OHOS::Rosen::HgmCore::Instance();
    uint32_t currentRate = hgmCore.GetScreenCurrentRefreshRate(hgmCore.GetActiveScreenId());
    bool hasGameScene = FrameReport::GetInstance().HasGameScene();
#ifdef RES_SCHED_ENABLE
    ResschedEventListener::GetInstance()->ReportFrameToRSS();
#endif
    RSTaskMessage::RSTask task = [this, output = output, layers = layers, param = param,
        currentRate = currentRate, hasGameScene = hasGameScene]() {
        int64_t startTime = GetCurTimeCount();
        if (output == nullptr || hdiBackend_ == nullptr) {
            return;
        }
        int64_t startTimeNs = 0;
        int64_t endTimeNs = 0;
        RS_LOGI_IF(DEBUG_COMPOSER, "RSHardwareThread::CommitAndReleaseData hasGameScene is %{public}d", hasGameScene);
        if (hasGameScene) {
            startTimeNs = std::chrono::duration_cast<std::chrono::nanoseconds>(
                std::chrono::steady_clock::now().time_since_epoch()).count();
        }
        RS_TRACE_NAME_FMT("RSHardwareThread::CommitAndReleaseLayers rate: %u, now: %" PRIu64 ", " \
            "vsyncId: %" PRIu64 ", size: %zu", currentRate, param.frameTimestamp, param.vsyncId, layers.size());
        RS_LOGD("RSHardwareThread::CommitAndReleaseLayers rate:%{public}u, " \
            "now:%{public}" PRIu64 ", vsyncId:%{public}" PRIu64 ", size:%{public}zu ",
            currentRate, param.frameTimestamp, param.vsyncId, layers.size());
        ExecuteSwitchRefreshRate(output, param.rate);
        PerformSetActiveMode(output, param.frameTimestamp, param.constraintRelativeTime);
        AddRefreshRateCount(output);
        output->SetLayerInfo(layers);
        if (output->IsDeviceValid()) {
            hdiBackend_->Repaint(output);
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
        RS_LOGD("RSHardwareThread::CommitAndReleaseData unExecuteTaskNum_:%{public}d,"
            " HARDWARE_THREAD_TASK_NUM:%{public}d", unExecuteTaskNum_.load(), HARDWARE_THREAD_TASK_NUM);
        if (unExecuteTaskNum_ <= HARDWARE_THREAD_TASK_NUM) {
            RSMainThread::Instance()->NotifyHardwareThreadCanExecuteTask();
        }
        int64_t endTime = GetCurTimeCount();
        uint64_t frameTime = endTime - startTime;
        uint32_t missedFrames = frameTime / REFRESH_PERIOD;
        uint16_t frameRate = currentRate;
        if (missedFrames >= HARD_JANK_TWO_TIME &&
            endTime - intervalTimePoints_ > REPORT_LOAD_WARNING_INTERVAL_TIME) {
            RS_LOGI("RSHardwareThread::CommitAndReleaseLayers report load event frameTime: %{public}" PRIu64
                " missedFrame: %{public}" PRIu32 " frameRate:%{public}" PRIu16 "",
                frameTime, missedFrames, frameRate);
            intervalTimePoints_ = endTime;
            HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::GRAPHIC, "RS_HARDWARE_THREAD_LOAD_WARNING",
                OHOS::HiviewDFX::HiSysEvent::EventType::STATISTIC, "FRAME_RATE", frameRate, "MISSED_FRAMES",
                missedFrames, "FRAME_TIME", frameTime);
        }
    };
    RSBaseRenderUtil::IncAcquiredBufferCount();
    unExecuteTaskNum_++;
    RSMainThread::Instance()->SetHardwareTaskNum(unExecuteTaskNum_.load());
    RS_LOGI_IF(DEBUG_COMPOSER,
        "RSHardwareThread::CommitAndReleaseData hgmCore's LtpoEnabled is %{public}d", hgmCore.GetLtpoEnabled());
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
        RS_LOGD("RSHardwareThread::CommitAndReleaseLayers vsyncId: %{public}" PRIu64 ", " \
            "update delayTime: %{public}" PRId64 ", currCommitTime: %{public}" PRId64 ", " \
            "lastCommitTime: %{public}" PRId64, param.vsyncId, delayTime_, currCommitTime, lastCommitTime_);
        RS_TRACE_NAME_FMT("update delayTime: %" PRId64 ", currCommitTime: %" PRId64 ", lastCommitTime: %" PRId64 "",
            delayTime_, currCommitTime, lastCommitTime_);
    }
    if (delayTime_ < 0 || delayTime_ >= MAX_DELAY_TIME) {
        delayTime_ = 0;
    }
    lastCommitTime_ = currTime + delayTime_ * NS_MS_UNIT_CONVERSION;
    PostDelayTask(task, delayTime_);
}

bool RSHardwareThread::IsDelayRequired(OHOS::Rosen::HgmCore& hgmCore, RefreshRateParam param,
    const OutputPtr& output, bool hasGameScene)
{
    if (param.isForceRefresh) {
        RS_LOGD("RSHardwareThread::CommitAndReleaseLayers in Force Refresh");
        RS_TRACE_NAME("CommitAndReleaseLayers in Force Refresh");
        return false;
    }

    if (hgmCore.GetLtpoEnabled()) {
        if (IsInAdaptiveMode(output)) {
            RS_LOGD("RSHardwareThread::CommitAndReleaseLayers in Adaptive Mode");
            RS_TRACE_NAME("CommitAndReleaseLayers in Adaptive Mode");
            isLastAdaptive_ = true;
            return false;
        }
        isLastAdaptive_ = false;
    } else {
        if (!hgmCore.IsDelayMode()) {
            return false;
        }
        if (hasGameScene) {
            RS_LOGD("RSHardwareThread::CommitAndReleaseLayers in Game Scene");
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
    int64_t pipelineOffset = 0;
    int64_t expectCommitTime = 0;
    int64_t idealPeriod = hgmCore.GetIdealPeriod(currentRate);
    int64_t period  = CreateVSyncSampler()->GetHardwarePeriod();
    uint64_t dvsyncOffset = RSMainThread::Instance()->GetRealTimeOffsetOfDvsync(param.frameTimestamp);
    int64_t compositionTime = period;

    if (!hgmCore.GetLtpoEnabled()) {
        vsyncOffset = UNI_RENDER_VSYNC_OFFSET_DELAY_MODE;
        // 2 period for draw and composition, pipelineOffset = 2 * period
        frameOffset = 2 * period + vsyncOffset;
    } else {
        vsyncOffset = CreateVSyncGenerator()->GetVSyncOffset();
        pipelineOffset = hgmCore.GetPipelineOffset();
        frameOffset = pipelineOffset + vsyncOffset + static_cast<int64_t>(dvsyncOffset);
    }
    expectCommitTime = param.actualTimestamp + frameOffset - compositionTime - RESERVE_TIME;
    int64_t diffTime = expectCommitTime - currTime;
    if (diffTime > 0 && period > 0) {
        delayTime_ = std::round(diffTime * 1.0f / NS_MS_UNIT_CONVERSION);
    }
    RS_TRACE_NAME_FMT("CalculateDelayTime pipelineOffset: %" PRId64 ", actualTimestamp: %" PRId64 ", " \
        "expectCommitTime: %" PRId64 ", currTime: %" PRId64 ", diffTime: %" PRId64 ", delayTime: %" PRId64 ", " \
        "frameOffset: %" PRId64 ", dvsyncOffset: %" PRIu64 ", vsyncOffset: %" PRId64 ", idealPeriod: %" PRId64 ", " \
        "period: %" PRId64 "",
        pipelineOffset, param.actualTimestamp, expectCommitTime, currTime, diffTime, delayTime_,
        frameOffset, dvsyncOffset, vsyncOffset, idealPeriod, period);
    RS_LOGD("RSHardwareThread::CalculateDelayTime period:%{public}" PRId64 " delayTime:%{public}" PRId64 "",
        period, delayTime_);
}

int64_t RSHardwareThread::GetCurTimeCount()
{
    auto curTime = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::microseconds>(curTime).count();
}

bool RSHardwareThread::IsInAdaptiveMode(const OutputPtr &output)
{
    if (hdiBackend_ == nullptr) {
        RS_LOGE("RSHardwareThread::IsInAdaptiveMode hdiBackend_ is nullptr");
        return false;
    }

    bool isSamplerEnabled = hdiBackend_->GetVsyncSamplerEnabled(output);
    auto& hgmCore = OHOS::Rosen::HgmCore::Instance();

    // if in game adaptive vsync mode and do direct composition,send layer immediately
    auto frameRateMgr = hgmCore.GetFrameRateMgr();
    if (frameRateMgr != nullptr) {
        bool isAdaptive = frameRateMgr->IsAdaptive();
        RS_LOGD("RSHardwareThread::CommitAndReleaseLayers send layer isAdaptive: %{public}u", isAdaptive);
        if (isAdaptive) {
            if (isSamplerEnabled) {
                // when phone enter game adaptive sync mode must disable vsync sampler
                hdiBackend_->SetVsyncSamplerEnabled(output, false);
            }
            return true;
        }
    }
    if (isLastAdaptive_ && !isSamplerEnabled) {
        // exit adaptive sync mode must restore vsync sampler, and startSample immediately
        hdiBackend_->SetVsyncSamplerEnabled(output, true);
        hdiBackend_->StartSample(output);
    }

    return false;
}

RefreshRateParam RSHardwareThread::GetRefreshRateParam()
{
    // need to sync the hgm data from main thread.
    // Temporary sync the timestamp to fix the duplicate time stamp issue.
    auto& hgmCore = OHOS::Rosen::HgmCore::Instance();
    bool directComposition = hgmCore.GetDirectCompositionFlag();
    RS_LOGI_IF(DEBUG_COMPOSER, "RSHardwareThread::GetRefreshRateData period is %{public}d", directComposition);
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
        };
    } else {
        param = {
            .rate = RSUniRenderThread::Instance().GetPendingScreenRefreshRate(),
            .frameTimestamp = RSUniRenderThread::Instance().GetCurrentTimestamp(),
            .actualTimestamp = RSUniRenderThread::Instance().GetActualTimestamp(),
            .vsyncId = RSUniRenderThread::Instance().GetVsyncId(),
            .constraintRelativeTime = RSUniRenderThread::Instance().GetPendingConstraintRelativeTime(),
            .isForceRefresh = RSUniRenderThread::Instance().GetForceRefreshFlag(),
        };
    }
    return param;
}

void RSHardwareThread::OnScreenVBlankIdleCallback(ScreenId screenId, uint64_t timestamp)
{
    RS_TRACE_NAME_FMT("RSHardwareThread::OnScreenVBlankIdleCallback screenId: %" PRIu64" now: %" PRIu64"", screenId, timestamp);
    vblankIdleCorrector_.SetScreenVBlankIdle(screenId);
}

void RSHardwareThread::ExecuteSwitchRefreshRate(const OutputPtr& output, uint32_t refreshRate)
{
    static bool refreshRateSwitch = system::GetBoolParameter("persist.hgm.refreshrate.enabled", true);
    if (!refreshRateSwitch) {
        RS_LOGD("RSHardwareThread: refreshRateSwitch is off, currRefreshRate is %{public}d", refreshRate);
        return;
    }

    auto& hgmCore = OHOS::Rosen::HgmCore::Instance();
    if (hgmCore.GetFrameRateMgr() == nullptr) {
        RS_LOGD("FrameRateMgr is null");
        return;
    }
    ScreenId id = output->GetScreenId();
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
    if (refreshRate != hgmCore.GetScreenCurrentRefreshRate(id) || lastCurScreenId != curScreenId) {
        RS_LOGD("RSHardwareThread::CommitAndReleaseLayers screenId %{public}d refreshRate %{public}d",
            static_cast<int>(id), refreshRate);
        int32_t sceneId = (lastCurScreenId != curScreenId) ? SWITCH_SCREEN_SCENE : 0;
        hgmCore.GetFrameRateMgr()->SetLastCurScreenId(curScreenId);
        int32_t status = hgmCore.SetScreenRefreshRate(id, sceneId, refreshRate);
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

        screenManager->SetScreenActiveMode(id, modeId);
        auto pendingPeriod = hgmCore.GetIdealPeriod(hgmCore.GetScreenCurrentRefreshRate(id));
        int64_t pendingTimestamp = static_cast<int64_t>(timestamp);
        if (hdiBackend_) {
            hdiBackend_->SetPendingMode(output, pendingPeriod, pendingTimestamp);
            hdiBackend_->StartSample(output);
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
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN) {
        auto frameBufferSurface = std::static_pointer_cast<RSSurfaceOhosVulkan>(frameBufferSurfaceOhos_);
        if (frameBufferSurface) {
            frameBufferSurface->WaitSurfaceClear();
        }
    }
#endif
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
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN) {
        rsSurface = std::make_shared<RSSurfaceOhosVulkan>(surface);
    }
#endif
    RS_LOGD("RSHardwareThread::Redraw: CreateFrameBufferSurfaceOhos.");
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
        auto layerSurface = layer->GetSurface();
        if (layerSurface != nullptr) {
            auto rcdlayerInfo = RSRcdManager::GetInstance().GetLayerPair(layerSurface->GetName());
            if (rcdlayerInfo.second != RoundCornerDisplayManager::RCDLayerType::INVALID) {
                rcdLayerInfoList.push_back(rcdlayerInfo);
                continue;
            }
        } else {
            RS_LOGE("RSHardwareThread::RedrawScreenRCD layerSurface is nullptr");
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
    if (surface == nullptr || uniRenderEngine_ == nullptr) {
        RS_LOGE("RSHardwareThread::Redraw: surface or uniRenderEngine is null.");
        return;
    }
    bool isProtected = false;
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        if (RSSystemProperties::GetDrmEnabled()) {
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

    RS_LOGD("RsDebug RSHardwareThread::Redraw flush frame buffer start");
    bool forceCPU = RSBaseRenderEngine::NeedForceCPU(layers);
    auto screenManager = CreateOrGetScreenManager();
    if (screenManager == nullptr) {
        RS_LOGE("RSHardwareThread::Redraw: screenManager is null.");
        return;
    }
    auto screenInfo = screenManager->QueryScreenInfo(screenId);
    std::shared_ptr<Drawing::ColorSpace> drawingColorSpace = nullptr;
#ifdef USE_VIDEO_PROCESSING_ENGINE
    GraphicColorGamut colorGamut = ComputeTargetColorGamut(layers);
    GraphicPixelFormat pixelFormat = ComputeTargetPixelFormat(layers);
    auto renderFrameConfig = RSBaseRenderUtil::GetFrameBufferRequestConfig(screenInfo,
        true, isProtected, colorGamut, pixelFormat);
    drawingColorSpace = RSBaseRenderEngine::ConvertColorGamutToDrawingColorSpace(colorGamut);
    // set color space to surface buffer metadata
    using namespace HDI::Display::Graphic::Common::V1_0;
    CM_ColorSpaceType colorSpace = CM_SRGB_FULL;
    if (ConvertColorGamutToSpaceType(colorGamut, colorSpace)) {
        if (surface->SetUserData("ATTRKEY_COLORSPACE_INFO", std::to_string(colorSpace)) != GSERROR_OK) {
            RS_LOGD("RSHardwareThread::Redraw set user data failed");
        }
    }
#else
    auto renderFrameConfig = RSBaseRenderUtil::GetFrameBufferRequestConfig(screenInfo, true, isProtected);
#endif
    // override redraw frame buffer with physical screen resolution.
    renderFrameConfig.width = static_cast<int32_t>(screenInfo.phyWidth);
    renderFrameConfig.height = static_cast<int32_t>(screenInfo.phyHeight);
    if (frameBufferSurfaceOhos_ == nullptr) {
        RS_LOGE("RSHardwareThread::Redraw fail, frameBufferSurfaceOhos_ is nullptr");
        frameBufferSurfaceOhos_ = CreateFrameBufferSurfaceOhos(surface);
    }
    FrameContextConfig frameContextConfig = {isProtected, false};
#ifdef RS_ENABLE_VKQUEUE_PRIORITY
    frameContextConfig.independentContext = RSSystemProperties::GetVkQueuePriorityEnable();
#endif
    auto renderFrame = uniRenderEngine_->RequestFrame(frameBufferSurfaceOhos_, renderFrameConfig,
        forceCPU, true, frameContextConfig);
    if (renderFrame == nullptr) {
        RS_LOGE("RsDebug RSHardwareThread::Redraw failed to request frame.");
        return;
    }
    auto canvas = renderFrame->GetCanvas();
    if (canvas == nullptr) {
        RS_LOGE("RsDebug RSHardwareThread::Redraw canvas is nullptr.");
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
    RS_LOGD("RsDebug RSHardwareThread::Redraw flush frame buffer end");
}

void RSHardwareThread::AddRefreshRateCount(const OutputPtr& output)
{
    auto screenManager = CreateOrGetScreenManager();
    if (screenManager == nullptr) {
        RS_LOGE("RSHardwareThread::AddRefreshRateCount screenManager is nullptr");
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
        RS_LOGE("RSHardwareThread::AddRefreshData fail, frameBufferSurfaceOhos_ is nullptr");
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

#ifdef USE_VIDEO_PROCESSING_ENGINE
GraphicColorGamut RSHardwareThread::ComputeTargetColorGamut(const std::vector<LayerInfoPtr>& layers)
{
    using namespace HDI::Display::Graphic::Common::V1_0;
    GraphicColorGamut colorGamut = GRAPHIC_COLOR_GAMUT_SRGB;
    for (auto& layer : layers) {
        if (layer == nullptr) {
            RS_LOGE("RSHardwareThread::ComputeTargetColorGamut layer is nullptr");
            continue;
        }
        auto buffer = layer->GetBuffer();
        if (buffer == nullptr) {
            RS_LOGW("RSHardwareThread::ComputeTargetColorGamut The buffer of layer is nullptr");
            continue;
        }

        CM_ColorSpaceInfo colorSpaceInfo;
        if (MetadataHelper::GetColorSpaceInfo(buffer, colorSpaceInfo) != GSERROR_OK) {
            RS_LOGD("RSHardwareThread::ComputeTargetColorGamut Get color space failed");
            continue;
        }

        if (colorSpaceInfo.primaries != COLORPRIMARIES_SRGB) {
            RS_LOGD("RSHardwareThread::ComputeTargetColorData fail, primaries is %{public}d",
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
            RS_LOGW("RSHardwareThread::ComputeTargetPixelFormat The buffer of layer is nullptr");
            continue;
        }

        auto bufferPixelFormat = buffer->GetFormat();
        if (bufferPixelFormat == GRAPHIC_PIXEL_FMT_RGBA_1010102 ||
            bufferPixelFormat == GRAPHIC_PIXEL_FMT_YCBCR_P010 ||
            bufferPixelFormat == GRAPHIC_PIXEL_FMT_YCRCB_P010) {
            pixelFormat = GRAPHIC_PIXEL_FMT_RGBA_1010102;
            RS_LOGD("RSHardwareThread::ComputeTargetPixelFormat pixelformat is set to 1010102 for 10bit buffer");
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
}
