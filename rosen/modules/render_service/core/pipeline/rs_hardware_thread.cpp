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

#ifdef RS_ENABLE_EGLIMAGE
#include "src/gpu/gl/GrGLDefines.h"
#endif

#include "hgm_core.h"
#include "pipeline/rs_base_render_util.h"
#include "pipeline/rs_uni_render_util.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_uni_render_engine.h"
#include "pipeline/round_corner_display/rs_round_corner_display.h"
#include "hgm_frame_rate_manager.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "screen_manager/rs_screen_manager.h"
#include "common/rs_singleton.h"
#include "rs_realtime_refresh_rate_manager.h"
#include "rs_trace.h"
#include "common/rs_optional_trace.h"
#include "frame_report.h"
#include "hdi_backend.h"
#include "vsync_sampler.h"
#include "parameters.h"
#ifdef RS_ENABLE_VK
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
#endif

namespace OHOS::Rosen {
namespace {
constexpr uint32_t HARDWARE_THREAD_TASK_NUM = 2;

#if defined(RS_ENABLE_VK)
Drawing::ColorType GetColorTypeFromBufferFormat(int32_t pixelFmt)
{
    switch (pixelFmt) {
        case GRAPHIC_PIXEL_FMT_RGBA_8888:
            return Drawing::ColorType::COLORTYPE_RGBA_8888;
        case GRAPHIC_PIXEL_FMT_BGRA_8888 :
            return Drawing::ColorType::COLORTYPE_BGRA_8888;
        case GRAPHIC_PIXEL_FMT_RGB_565:
            return Drawing::ColorType::COLORTYPE_RGB_565;
        default:
            return Drawing::ColorType::COLORTYPE_RGBA_8888;
    }
}
#endif
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
    redrawCb_ = std::bind(&RSHardwareThread::Redraw, this, std::placeholders::_1, std::placeholders::_2,
        std::placeholders::_3);
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

uint32_t RSHardwareThread::GetunExcuteTaskNum()
{
    return unExcuteTaskNum_;
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
    auto& hgmCore = OHOS::Rosen::HgmCore::Instance();
    uint32_t rate = hgmCore.GetPendingScreenRefreshRate();
    uint32_t currentRate = hgmCore.GetScreenCurrentRefreshRate(hgmCore.GetActiveScreenId());
    uint64_t currTimestamp = hgmCore.GetCurrentTimestamp();
    RSTaskMessage::RSTask task = [this, output = output, layers = layers, rate = rate,
        currentRate = currentRate, timestamp = currTimestamp]() {
        int64_t startTimeNs = 0;
        int64_t endTimeNs = 0;
        bool hasGameScene = FrameReport::GetInstance().HasGameScene();
        if (hasGameScene) {
            startTimeNs = std::chrono::duration_cast<std::chrono::nanoseconds>(
                std::chrono::steady_clock::now().time_since_epoch()).count();
        }

        RS_TRACE_NAME_FMT("RSHardwareThread::CommitAndReleaseLayers rate: %d, now: %lu", currentRate, timestamp);
        ExecuteSwitchRefreshRate(rate);
        PerformSetActiveMode(output, timestamp);
        AddRefreshRateCount();
        output->SetLayerInfo(layers);
        if (output->IsDeviceValid()) {
            hdiBackend_->Repaint(output);
        }
        output->ReleaseLayers();
        RSMainThread::Instance()->NotifyDisplayNodeBufferReleased();

        if (hasGameScene) {
            endTimeNs = std::chrono::duration_cast<std::chrono::nanoseconds>(
                std::chrono::steady_clock::now().time_since_epoch()).count();
            FrameReport::GetInstance().SetLastSwapBufferTime(endTimeNs - startTimeNs);
        }

        unExcuteTaskNum_--;
        if (unExcuteTaskNum_ <= HARDWARE_THREAD_TASK_NUM) {
            RSMainThread::Instance()->NotifyHardwareThreadCanExcuteTask();
        }
    };
    unExcuteTaskNum_++;

    if (!hgmCore.GetLtpoEnabled()) {
        PostTask(task);
    } else {
        auto period  = CreateVSyncSampler()->GetHardwarePeriod();
        int64_t pipelineOffset = hgmCore.GetPipelineOffset();
        uint64_t expectCommitTime = static_cast<uint64_t>(currTimestamp + static_cast<uint64_t>(pipelineOffset) -
            static_cast<uint64_t>(period));
        uint64_t currTime = static_cast<uint64_t>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(
                std::chrono::steady_clock::now().time_since_epoch()).count());
        int64_t delayTime = std::round((static_cast<int64_t>(expectCommitTime - currTime)) / 1000000);
        RS_TRACE_NAME_FMT("RSHardwareThread::CommitAndReleaseLayers " \
            "expectCommitTime: %lu, currTime: %lu, delayTime: %ld, pipelineOffset: %ld, period: %ld",
            expectCommitTime, currTime, delayTime, pipelineOffset, period);
        if (period == 0 || delayTime <= 0) {
            PostTask(task);
        } else {
            PostDelayTask(task, delayTime);
        }
    }
}

void RSHardwareThread::ExecuteSwitchRefreshRate(uint32_t refreshRate)
{
    static bool refreshRateSwitch = system::GetBoolParameter("persist.hgm.refreshrate.enabled", true);
    if (!refreshRateSwitch) {
        RS_LOGD("RSHardwareThread: refreshRateSwitch is off, currRefreshRate is %{public}d", refreshRate);
        return;
    }

    auto screenManager = CreateOrGetScreenManager();
    auto& hgmCore = OHOS::Rosen::HgmCore::Instance();
    ScreenId id = hgmCore.GetFrameRateMgr()->GetCurScreenId();
    if (refreshRate != hgmCore.GetScreenCurrentRefreshRate(id)) {
        RS_LOGI("RSHardwareThread::CommitAndReleaseLayers screenId %{public}d refreshRate %{public}d",
            static_cast<int>(id), refreshRate);
        int32_t status = hgmCore.SetScreenRefreshRate(id, 0, refreshRate);
        if (status < EXEC_SUCCESS) {
            RS_LOGD("RSHardwareThread: failed to set refreshRate %{public}d, screenId %{public}" PRIu64 "", refreshRate,
                id);
        }
    }
}

void RSHardwareThread::PerformSetActiveMode(OutputPtr output, uint64_t timestamp)
{
    auto &hgmCore = OHOS::Rosen::HgmCore::Instance();
    auto screenManager = CreateOrGetScreenManager();
    if (screenManager == nullptr) {
        RS_LOGE("RSHardwareThread CreateOrGetScreenManager fail.");
        return;
    }

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
        hgmCore.GetScreenCurrentRefreshRate(screenManager->GetDefaultScreenId()));
    for (auto mapIter = modeMap->begin(); mapIter != modeMap->end(); ++mapIter) {
        ScreenId id = mapIter->first;
        int32_t modeId = mapIter->second;

        auto supportedModes = screenManager->GetScreenSupportedModes(id);
        for (auto mode : supportedModes) {
            RS_OPTIONAL_TRACE_NAME_FMT("RSHardwareThread check modes w: %d, h: %d, rate: %d, id: %d",
                mode.GetScreenWidth(), mode.GetScreenHeight(), mode.GetScreenRefreshRate(), mode.GetScreenModeId());
        }

        screenManager->SetScreenActiveMode(id, modeId);
        if (!hgmCore.GetLtpoEnabled()) {
            hdiBackend_->StartSample(output);
        } else {
            auto pendingPeriod = hgmCore.GetIdealPeriod(hgmCore.GetScreenCurrentRefreshRate(id));
            int64_t pendingTimestamp = static_cast<int64_t>(timestamp);
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
    if (uniRenderEngine_ != nullptr) {
        uniRenderEngine_->ResetCurrentContext();
    }
    return output->ClearFrameBuffer();
}

void RSHardwareThread::Redraw(const sptr<Surface>& surface, const std::vector<LayerInfoPtr>& layers, uint32_t screenId)
{
    RS_TRACE_NAME("RSHardwareThread::Redraw");
    if (surface == nullptr) {
        RS_LOGE("RSHardwareThread::Redraw: surface is null.");
        return;
    }

    RS_LOGD("RsDebug RSHardwareThread::Redraw flush frame buffer start");
    bool forceCPU = RSBaseRenderEngine::NeedForceCPU(layers);
    auto screenManager = CreateOrGetScreenManager();
    auto screenInfo = screenManager->QueryScreenInfo(screenId);
    std::shared_ptr<Drawing::ColorSpace> drawingColorSpace = nullptr;
#ifdef USE_VIDEO_PROCESSING_ENGINE
    GraphicColorGamut colorGamut = ComputeTargetColorGamut(layers);
    GraphicPixelFormat pixelFormat = ComputeTargetPixelFormat(layers);
    auto renderFrameConfig = RSBaseRenderUtil::GetFrameBufferRequestConfig(screenInfo, true, colorGamut, pixelFormat);
    drawingColorSpace = RSBaseRenderEngine::ConvertColorGamutToDrawingColorSpace(colorGamut);
#else
    auto renderFrameConfig = RSBaseRenderUtil::GetFrameBufferRequestConfig(screenInfo, true);
#endif
    // override redraw frame buffer with physical screen resolution.
    renderFrameConfig.width = screenInfo.phyWidth;
    renderFrameConfig.height = screenInfo.phyHeight;
    auto renderFrame = uniRenderEngine_->RequestFrame(surface, renderFrameConfig, forceCPU);
    if (renderFrame == nullptr) {
        RS_LOGE("RsDebug RSHardwareThread::Redraw failed to request frame.");
        return;
    }
    auto canvas = renderFrame->GetCanvas();
    if (canvas == nullptr) {
        RS_LOGE("RsDebug RSHardwareThread::Redraw canvas is nullptr.");
        return;
    }
#ifdef RS_ENABLE_EGLIMAGE
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::IsUseVulkan()) {
        canvas->Clear(Drawing::Color::COLOR_TRANSPARENT);
    }
    std::unordered_map<int32_t, std::shared_ptr<NativeVkImageRes>> imageCacheSeqsVK;
#endif
#ifdef RS_ENABLE_GL
    std::unordered_map<int32_t, std::unique_ptr<ImageCacheSeq>> imageCacheSeqs;
#endif
#endif // RS_ENABLE_EGLIMAGE
    bool isTopGpuDraw = false;
    bool isBottomGpuDraw = false;
    for (const auto& layer : layers) {
        if (layer == nullptr) {
            continue;
        }

        if (layer->GetCompositionType() == GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE ||
            layer->GetCompositionType() == GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE_CLEAR) {
            continue;
        }

        if (layer->GetSurface()->GetName() == "RCDTopSurfaceNode") {
            isTopGpuDraw = true;
            continue;
        }
        if (layer->GetSurface()->GetName() == "RCDBottomSurfaceNode") {
            isBottomGpuDraw = true;
            continue;
        }

        Drawing::AutoCanvasRestore acr(*canvas, true);
        auto dstRect = layer->GetLayerSize();
        Drawing::Rect clipRect = Drawing::Rect(static_cast<float>(dstRect.x), static_cast<float>(dstRect.y),
            static_cast<float>(dstRect.w) + static_cast<float>(dstRect.x),
            static_cast<float>(dstRect.h) + static_cast<float>(dstRect.y));
        canvas->ClipRect(clipRect, Drawing::ClipOp::INTERSECT, false);

        // prepare BufferDrawParam
        auto params = RSUniRenderUtil::CreateLayerBufferDrawParam(layer, forceCPU);
        params.matrix.PostScale(screenInfo.GetRogWidthRatio(), screenInfo.GetRogHeightRatio());
        canvas->ConcatMatrix(params.matrix);
#ifndef RS_ENABLE_EGLIMAGE
        RSBaseRenderEngine::DrawBuffer(*canvas, params);
#else
        if (!params.useCPU) {
            if (!RSBaseRenderUtil::IsBufferValid(params.buffer)) {
                RS_LOGE("RSHardwareThread::Redraw CreateEglImageFromBuffer invalid param!");
                continue;
            }
            if (canvas->GetGPUContext() == nullptr) {
                RS_LOGE("RSHardwareThread::Redraw CreateEglImageFromBuffer GrContext is null!");
                continue;
            }
            uint32_t eglTextureId = 0;
            uint32_t bufferId = 0;
            (void)eglTextureId;
            (void)bufferId;
#if defined(RS_ENABLE_GL) && defined(RS_ENABLE_EGLIMAGE)
            if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
                auto eglImageCache = uniRenderEngine_->GetEglImageManager()->CreateImageCacheFromBuffer(params.buffer,
                    params.acquireFence);
                if (eglImageCache == nullptr) {
                    continue;
                }
                eglTextureId = eglImageCache->TextureId();
                if (eglTextureId == 0) {
                    RS_LOGE("RSHardwareThread::Redraw CreateImageCacheFromBuffer return invalid texture ID");
                    continue;
                }
                bufferId = params.buffer->GetSeqNum();
                imageCacheSeqs[bufferId] = std::move(eglImageCache);
            }
#endif
            std::shared_ptr<Drawing::Image> image = nullptr;
#if defined(RS_ENABLE_GL) && defined(RS_ENABLE_EGLIMAGE)
            if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
                Drawing::ColorType colorType = Drawing::ColorType::COLORTYPE_RGBA_8888;
                auto pixelFmt = params.buffer->GetFormat();
                if (pixelFmt == GRAPHIC_PIXEL_FMT_BGRA_8888) {
                    colorType = Drawing::ColorType::COLORTYPE_BGRA_8888;
                } else if (pixelFmt == GRAPHIC_PIXEL_FMT_YCBCR_P010 || pixelFmt == GRAPHIC_PIXEL_FMT_YCRCB_P010) {
                    colorType = Drawing::ColorType::COLORTYPE_RGBA_1010102;
                }

                Drawing::BitmapFormat bitmapFormat = { colorType, Drawing::AlphaType::ALPHATYPE_PREMUL };

                Drawing::TextureInfo externalTextureInfo;
                externalTextureInfo.SetWidth(params.buffer->GetSurfaceBufferWidth());
                externalTextureInfo.SetHeight(params.buffer->GetSurfaceBufferHeight());
                externalTextureInfo.SetIsMipMapped(false);
                externalTextureInfo.SetTarget(GL_TEXTURE_EXTERNAL_OES);
                externalTextureInfo.SetID(eglTextureId);
                auto glType = GR_GL_RGBA8;
                if (pixelFmt == GRAPHIC_PIXEL_FMT_BGRA_8888) {
                    glType = GR_GL_BGRA8;
                } else if (pixelFmt == GRAPHIC_PIXEL_FMT_YCBCR_P010 || pixelFmt == GRAPHIC_PIXEL_FMT_YCRCB_P010) {
                    glType = GL_RGB10_A2;
                }
                externalTextureInfo.SetFormat(glType);

                image = std::make_shared<Drawing::Image>();
                if (!image->BuildFromTexture(*canvas->GetGPUContext(), externalTextureInfo,
                    Drawing::TextureOrigin::TOP_LEFT, bitmapFormat, drawingColorSpace)) {
                    RS_LOGE("RSHardwareThread::Redraw: image BuildFromTexture failed");
                    return;
                }
            }
#endif
#ifdef RS_ENABLE_VK
            if (RSSystemProperties::IsUseVulkan()) {
                Drawing::ColorType colorType = GetColorTypeFromBufferFormat(params.buffer->GetFormat());
                auto imageCache = uniRenderEngine_->GetVkImageManager()->CreateImageCacheFromBuffer(
                    params.buffer, params.acquireFence);
                if (!imageCache) {
                    continue;
                }
                auto bufferId = params.buffer->GetSeqNum();
                imageCacheSeqsVK[bufferId] = imageCache;
                auto& backendTexture = imageCache->GetBackendTexture();

                Drawing::BitmapFormat bitmapFormat = { colorType, Drawing::AlphaType::ALPHATYPE_PREMUL };

                image = std::make_shared<Drawing::Image>();
                if (!image->BuildFromTexture(*canvas->GetGPUContext(), backendTexture.GetTextureInfo(),
                    Drawing::TextureOrigin::TOP_LEFT, bitmapFormat, drawingColorSpace,
                    NativeBufferUtils::DeleteVkImage,
                    imageCache->RefCleanupHelper())) {
                    RS_LOGE("RSHardwareThread::Redraw: image BuildFromTexture failed");
                    return;
                }
            }
#endif
            if (image == nullptr) {
                RS_LOGE("RSHardwareThread::DrawImage: image is nullptr!");
                return;
            }
#ifdef USE_VIDEO_PROCESSING_ENGINE
            Drawing::Matrix matrix;
            auto sx = params.dstRect.GetWidth() / params.srcRect.GetWidth();
            auto sy = params.dstRect.GetHeight() / params.srcRect.GetHeight();
            auto tx = params.dstRect.GetLeft() - params.srcRect.GetLeft() * sx;
            auto ty = params.dstRect.GetTop() - params.srcRect.GetTop() * sy;
            matrix.SetScaleTranslate(sx, sy, tx, ty);
            auto imageShader = Drawing::ShaderEffect::CreateImageShader(
                *image, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, Drawing::SamplingOptions(), matrix);
            if (imageShader == nullptr) {
                RS_LOGE("RSHardwareThread::DrawImage imageShader is nullptr.");
            } else {
                params.paint.SetShaderEffect(imageShader);
                params.targetColorGamut = colorGamut;
                params.screenBrightnessNits = screenManager->GetScreenBrightnessNits(screenId);

                uniRenderEngine_->ColorSpaceConvertor(imageShader, params);
            }
#endif

            canvas->AttachBrush(params.paint);
            RS_TRACE_NAME_FMT("DrawImage(GPU) seqNum: %d", bufferId);
#ifndef USE_VIDEO_PROCESSING_ENGINE
            canvas->DrawImageRect(*image, params.srcRect, params.dstRect,
                Drawing::SamplingOptions(), Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT);
#else
            canvas->DrawRect(params.dstRect);
#endif
            canvas->DetachBrush();
        } else {
            RSBaseRenderEngine::DrawBuffer(*canvas, params);
        }
#endif
        // Dfx for redraw region
        if (RSSystemProperties::GetHwcRegionDfxEnabled()) {
            RectI dst(dstRect.x, dstRect.y, dstRect.w, dstRect.h);
            RSUniRenderUtil::DrawRectForDfx(*canvas, dst, Drawing::Color::COLOR_YELLOW, 0.4f,
                layer->GetSurface()->GetName());
        }
    }

    if (isTopGpuDraw && RSSingleton<RoundCornerDisplay>::GetInstance().GetRcdEnable()) {
        RSSingleton<RoundCornerDisplay>::GetInstance().DrawTopRoundCorner(canvas.get());
    }

    if (isBottomGpuDraw && RSSingleton<RoundCornerDisplay>::GetInstance().GetRcdEnable()) {
        RSSingleton<RoundCornerDisplay>::GetInstance().DrawBottomRoundCorner(canvas.get());
    }
    renderFrame->Flush();
#ifdef RS_ENABLE_EGLIMAGE
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::IsUseVulkan()) {
        imageCacheSeqsVK.clear();
    }
#endif
#ifdef RS_ENABLE_GL
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
        imageCacheSeqs.clear();
    }
#endif
#endif
    RS_LOGD("RsDebug RSHardwareThread::Redraw flush frame buffer end");
}

void RSHardwareThread::AddRefreshRateCount()
{
    auto screenManager = CreateOrGetScreenManager();
    ScreenId id = screenManager->GetDefaultScreenId();
    auto& hgmCore = OHOS::Rosen::HgmCore::Instance();
    uint32_t currentRefreshRate = hgmCore.GetScreenCurrentRefreshRate(id);
    auto [iter, success] = refreshRateCounts_.try_emplace(currentRefreshRate, 1);
    if (!success) {
        iter->second++;
    }
    RSRealtimeRefreshRateManager::Instance().CountRealtimeFrame();
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
