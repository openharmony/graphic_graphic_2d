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
#include "hgm_core.h"
#include "hgm_frame_rate_manager.h"
#include "parameters.h"
#include "rs_realtime_refresh_rate_manager.h"
#include "rs_trace.h"
#include "vsync_sampler.h"

#include "common/rs_optional_trace.h"
#include "common/rs_singleton.h"
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
    return unExecuteTaskNum_;
}

void RSHardwareThread::RefreshRateCounts(std::string& dumpString)
{
    if (refreshRateCounts_.empty()) {
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
#ifdef RES_SCHED_ENABLE
    ResschedEventListener::GetInstance()->ReportFrameToRSS();
#endif
    RSTaskMessage::RSTask task = [this, output = output, layers = layers, param = param]() {
        if (output == nullptr || hdiBackend_ == nullptr) {
            return;
        }
        int64_t startTimeNs = 0;
        int64_t endTimeNs = 0;
        bool hasGameScene = FrameReport::GetInstance().HasGameScene();
        if (hasGameScene) {
            startTimeNs = std::chrono::duration_cast<std::chrono::nanoseconds>(
                std::chrono::steady_clock::now().time_since_epoch()).count();
        }
        uint32_t currentRate = HgmCore::Instance().GetScreenCurrentRefreshRate(HgmCore::Instance().GetActiveScreenId());
        RS_TRACE_NAME_FMT("RSHardwareThread::CommitAndReleaseLayers rate: %d, now: %lu, size: %lu",
            currentRate, param.frameTimestamp, layers.size());
        RS_LOGI("RSHardwareThread::CommitAndReleaseLayers rate:%{public}d, now:%{public}" PRIu64 ", size:%{public}zu",
            currentRate, param.frameTimestamp, layers.size());
        ExecuteSwitchRefreshRate(param.rate);
        PerformSetActiveMode(output, param.frameTimestamp, param.constraintRelativeTime);
        AddRefreshRateCount();
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
            FrameReport::GetInstance().ReportCommitTime(endTimeNs);
        }

        unExecuteTaskNum_--;
        if (unExecuteTaskNum_ <= HARDWARE_THREAD_TASK_NUM) {
            RSMainThread::Instance()->NotifyHardwareThreadCanExecuteTask();
        }
    };
    RSBaseRenderUtil::IncAcquiredBufferCount();
    unExecuteTaskNum_++;
    RSMainThread::Instance()->SetHardwareTaskNum(unExecuteTaskNum_.load());
    auto& hgmCore = OHOS::Rosen::HgmCore::Instance();
    if (!hgmCore.GetLtpoEnabled()) {
        PostTask(task);
    } else {
        auto period  = CreateVSyncSampler()->GetHardwarePeriod();
        int64_t pipelineOffset = hgmCore.GetPipelineOffset();
        uint64_t expectCommitTime = static_cast<uint64_t>(param.frameTimestamp +
            static_cast<uint64_t>(pipelineOffset) - static_cast<uint64_t>(period));
        uint64_t currTime = static_cast<uint64_t>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(
                std::chrono::steady_clock::now().time_since_epoch()).count());
        delayTime_ = std::round((static_cast<int64_t>(expectCommitTime - currTime)) / 1000000);
        RS_TRACE_NAME_FMT("RSHardwareThread::CommitAndReleaseLayers " \
            "expectCommitTime: %lu, currTime: %lu, delayTime: %ld, pipelineOffset: %ld, period: %ld",
            expectCommitTime, currTime, delayTime_, pipelineOffset, period);
        if (period == 0 || delayTime_ <= 0) {
            PostTask(task);
        } else {
            PostDelayTask(task, delayTime_);
        }
    }

    for (const auto& layer : layers) {
        if (layer == nullptr || layer->GetClearCacheSet().empty()) {
            continue;
        }

        // Remove image caches when their SurfaceNode has gobackground/cleancache.
        RSTaskMessage::RSTask clearTask = [this, cacheset = layer->GetClearCacheSet()]() {
            if (uniRenderEngine_ != nullptr) {
                uniRenderEngine_->ClearCacheSet(cacheset);
            }
        };
        PostTask(clearTask);
    }
}

RefreshRateParam RSHardwareThread::GetRefreshRateParam()
{
    // need to sync the hgm data from main thread.
    // Temporary sync the timestamp to fix the duplicate time stamp issue.
    auto& hgmCore = OHOS::Rosen::HgmCore::Instance();
    bool directComposition = hgmCore.GetDirectCompositionFlag();
    if (directComposition) {
        hgmCore.SetDirectCompositionFlag(false);
    }
    RefreshRateParam param;
    if (directComposition) {
        param = {
            .rate = hgmCore.GetPendingScreenRefreshRate(),
            .frameTimestamp = hgmCore.GetCurrentTimestamp(),
            .constraintRelativeTime = hgmCore.GetPendingConstraintRelativeTime(),
        };
    } else {
        param = {
            .rate = RSUniRenderThread::Instance().GetPendingScreenRefreshRate(),
            .frameTimestamp = RSUniRenderThread::Instance().GetCurrentTimestamp(),
            .constraintRelativeTime = RSUniRenderThread::Instance().GetPendingConstraintRelativeTime(),
        };
    }
    return param;
}

void RSHardwareThread::OnScreenVBlankIdleCallback(ScreenId screenId, uint64_t timestamp)
{
    RS_TRACE_NAME_FMT("RSHardwareThread::OnScreenVBlankIdleCallback screenId: %d now: %lu", screenId, timestamp);
    vblankIdleCorrector_.SetScreenVBlankIdle(screenId);
}

void RSHardwareThread::ExecuteSwitchRefreshRate(uint32_t refreshRate)
{
    static bool refreshRateSwitch = system::GetBoolParameter("persist.hgm.refreshrate.enabled", true);
    if (!refreshRateSwitch) {
        RS_LOGD("RSHardwareThread: refreshRateSwitch is off, currRefreshRate is %{public}d", refreshRate);
        return;
    }

    static ScreenId lastScreenId = 12345; // init value diff with any real screen id
    auto screenManager = CreateOrGetScreenManager();
    auto& hgmCore = OHOS::Rosen::HgmCore::Instance();
    if (hgmCore.GetFrameRateMgr() == nullptr) {
        RS_LOGD("FrameRateMgr is null");
        return;
    }
    auto screenRefreshRateImme = hgmCore.GetScreenRefreshRateImme();
    if (screenRefreshRateImme > 0) {
        RS_LOGD("ExecuteSwitchRefreshRate:rate change: %{public}u -> %{public}u", refreshRate, screenRefreshRateImme);
        refreshRate = screenRefreshRateImme;
    }
    ScreenId id = hgmCore.GetFrameRateMgr()->GetCurScreenId();
    if (refreshRate != hgmCore.GetScreenCurrentRefreshRate(id) || lastScreenId != id) {
        RS_LOGI("RSHardwareThread::CommitAndReleaseLayers screenId %{public}d refreshRate %{public}d",
            static_cast<int>(id), refreshRate);
        lastScreenId = id;
        int32_t status = hgmCore.SetScreenRefreshRate(id, 0, refreshRate);
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
        RS_LOGE("RSHardwareThread CreateOrGetScreenManager fail.");
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
            RS_OPTIONAL_TRACE_NAME_FMT("RSHardwareThread check modes w: %d, h: %d, rate: %d, id: %d",
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
        if (RSSystemProperties::GetDrmEnabled() && RSMainThread::Instance()->GetDeviceType() != DeviceType::PC) {
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
#else
    auto renderFrameConfig = RSBaseRenderUtil::GetFrameBufferRequestConfig(screenInfo, true, isProtected);
#endif
    // override redraw frame buffer with physical screen resolution.
    renderFrameConfig.width = static_cast<int32_t>(screenInfo.phyWidth);
    renderFrameConfig.height = static_cast<int32_t>(screenInfo.phyHeight);
    if (frameBufferSurfaceOhos_ == nullptr) {
        frameBufferSurfaceOhos_ = CreateFrameBufferSurfaceOhos(surface);
    }
    FrameContextConfig frameContextConfig = {isProtected, false};
    if (RSSystemProperties::GetVkQueueDividedEnable()) {
        frameContextConfig.independentContext = true;
    }
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

    renderFrame->Flush();
    RS_LOGD("RsDebug RSHardwareThread::Redraw flush frame buffer end");
}

void RSHardwareThread::AddRefreshRateCount()
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
    RSRealtimeRefreshRateManager::Instance().CountRealtimeFrame();

    auto frameRateMgr = hgmCore.GetFrameRateMgr();
    if (frameRateMgr == nullptr) {
        return;
    }
    frameRateMgr->GetTouchManager().HandleRsFrame();
}

void RSHardwareThread::SubScribeSystemAbility()
{
    RS_LOGD("%{public}s", __func__);
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
#endif
}
