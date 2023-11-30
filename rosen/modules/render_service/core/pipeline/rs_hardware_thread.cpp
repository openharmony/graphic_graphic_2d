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

#ifdef RS_ENABLE_EGLIMAGE
#include "src/gpu/gl/GrGLDefines.h"
#endif

#include "hgm_core.h"
#include "pipeline/rs_base_render_util.h"
#include "pipeline/rs_uni_render_util.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_uni_render_engine.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "screen_manager/rs_screen_manager.h"
#include "rs_trace.h"
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
    redrawCb_ = std::bind(&RSHardwareThread::Redraw, this,std::placeholders::_1, std::placeholders::_2,
        std::placeholders::_3);
    if (handler_) {
        ScheduleTask(
            [this]() {
                auto screenManager = CreateOrGetScreenManager();
                if (screenManager == nullptr || !screenManager->Init()) {
                    RS_LOGE("RSHardwareThread CreateOrGetScreenManager or init fail.");
                    return;
                }
                uniRenderEngine_ = std::make_shared<RSUniRenderEngine>();
#ifdef RS_ENABLE_VK
                uniRenderEngine_->Init(true);
#else
                uniRenderEngine_->Init();
#endif
            }).wait();
    }
    auto onPrepareCompleteFunc = [this](auto& surface, const auto& param, void* data) {
        OnPrepareComplete(surface, param, data);
    };
    if (hdiBackend_ != nullptr) {
        hdiBackend_->RegPrepareComplete(onPrepareCompleteFunc, this);
    }
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

void RSHardwareThread::ReleaseBuffer(sptr<SurfaceBuffer> buffer, sptr<SyncFence> releaseFence,
    sptr<IConsumerSurface> cSurface)
{
    if (cSurface == nullptr) {
        RS_LOGE("RsDebug RSHardwareThread:: ReleaseBuffer failed, no consumer!");
        return;
    }

    if (buffer != nullptr) {
        RS_TRACE_NAME("RSHardwareThread::ReleaseBuffer");
        auto ret = cSurface->ReleaseBuffer(buffer, releaseFence);
        if (ret != OHOS::SURFACE_ERROR_OK) {
            return;
        }
        // reset prevBuffer if we release it successfully,
        // to avoid releasing the same buffer next frame in some situations.
        buffer = nullptr;
        releaseFence = SyncFence::INVALID_FENCE;
    }
}

void RSHardwareThread::RefreshRateCounts(std::string& dumpString)
{
    if (refreshRateCounts_.empty()) {
        return;
    }
    std::map<uint32_t, int>::iterator iter;
    for (iter = refreshRateCounts_.begin(); iter != refreshRateCounts_.end(); iter++) {
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

void RSHardwareThread::ReleaseLayers(OutputPtr output, const std::unordered_map<uint32_t, LayerPtr>& layerMap)
{
    // get present timestamp from and set present timestamp to surface
    for (const auto& [id, layer] : layerMap) {
        if (layer == nullptr || layer->GetLayerInfo()->GetSurface() == nullptr) {
            RS_LOGW("RSHardwareThread::ReleaseLayers: layer or layer's cSurface is nullptr");
            continue;
        }
        LayerPresentTimestamp(layer->GetLayerInfo(), layer->GetLayerInfo()->GetSurface());
    }

    // set all layers' releaseFence.
    if (output == nullptr) {
        RS_LOGE("RSHardwareThread::ReleaseLayers: output is nullptr");
        return;
    }
    const auto layersReleaseFence = output->GetLayersReleaseFence();
    if (layersReleaseFence.size() == 0) {
        // When release fence's size is 0, the output may invalid, release all buffer
        // This situation may happen when killing composer_host
        for (const auto& [id, layer] : layerMap) {
            if (layer == nullptr || layer->GetLayerInfo()->GetSurface() == nullptr) {
                RS_LOGW("RSHardwareThread::ReleaseLayers: layer or layer's cSurface is nullptr");
                continue;
            }
            auto preBuffer = layer->GetLayerInfo()->GetPreBuffer();
            auto consumer = layer->GetLayerInfo()->GetSurface();
            ReleaseBuffer(preBuffer, SyncFence::INVALID_FENCE, consumer);
        }
        RS_LOGE("RSHardwareThread::ReleaseLayers: no layer needs to release");
    }
    for (const auto& [layer, fence] : layersReleaseFence) {
        if (layer == nullptr) {
            continue;
        }
        auto preBuffer = layer->GetPreBuffer();
        auto consumer = layer->GetSurface();
        ReleaseBuffer(preBuffer, fence, consumer);
    }
    RSMainThread::Instance()->NotifyDisplayNodeBufferReleased();
}

void RSHardwareThread::CommitAndReleaseLayers(OutputPtr output, const std::vector<LayerInfoPtr>& layers)
{
    if (!handler_) {
        RS_LOGE("RSHardwareThread::CommitAndReleaseLayers handler is nullptr");
        return;
    }
    auto& hgmCore = OHOS::Rosen::HgmCore::Instance();
    uint32_t rate = hgmCore.GetPendingScreenRefreshRate();
    uint64_t currTimestamp = hgmCore.GetCurrentTimestamp();
    RSTaskMessage::RSTask task = [this, output = output, layers = layers, rate = rate, timestamp = currTimestamp]() {
        RS_TRACE_NAME_FMT("RSHardwareThread::CommitAndReleaseLayers rate: %d, now: %lu", rate, timestamp);
        ExecuteSwitchRefreshRate(rate);
        AddRefreshRateCount(rate);
        PerformSetActiveMode(output);
        output->SetLayerInfo(layers);
        if (output->IsDeviceValid()) {
            hdiBackend_->Repaint(output);
        }
        auto layerMap = output->GetLayers();
        ReleaseLayers(output, layerMap);
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
        RS_TRACE_NAME_FMT("RSHardwareThread::CommitAndReleaseLayers expectCommitTime: %lu, period: %ld, currTime: %lu" \
            ", delayTime: %ld", expectCommitTime, period, currTime, delayTime);
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
    ScreenId id = screenManager->GetDefaultScreenId();
    auto& hgmCore = OHOS::Rosen::HgmCore::Instance();
    if (refreshRate != hgmCore.GetScreenCurrentRefreshRate(id)) {
        RS_TRACE_NAME_FMT("RSHardwareThread::CommitAndReleaseLayers SetScreenRefreshRate: %d", refreshRate);
        RS_LOGD("RSHardwareThread::CommitAndReleaseLayers SetScreenRefreshRate = %{public}d", refreshRate);
        int32_t status = hgmCore.SetScreenRefreshRate(id, 0, refreshRate);
        if (status < EXEC_SUCCESS) {
            RS_LOGE("RSHardwareThread: failed to set refreshRate %{public}d, screenId %{public}llu", refreshRate, id);
        }
    }
}

void RSHardwareThread::PerformSetActiveMode(OutputPtr output)
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

    RS_TRACE_NAME("RSHardwareThread::PerformSetActiveMode setting active mode");
    for (auto mapIter = modeMap->begin(); mapIter != modeMap->end(); ++mapIter) {
        ScreenId id = mapIter->first;
        int32_t modeId = mapIter->second;

        auto supportedModes = screenManager->GetScreenSupportedModes(id);
        for (auto mode : supportedModes) {
            std::string temp = "RSHardwareThread check modes w: " + std::to_string(mode.GetScreenWidth()) +
                ", h: " + std::to_string(mode.GetScreenHeight()) +
                ", rate: " + std::to_string(mode.GetScreenRefreshRate()) +
                ", id: " + std::to_string(mode.GetScreenModeId());
            RS_LOGD("%{public}s", temp.c_str());
        }

        screenManager->SetScreenActiveMode(id, modeId);
        if (!hgmCore.GetLtpoEnabled()) {
            hdiBackend_->StartSample(output);
        } else {
            auto pendingPeriod = hgmCore.GetIdealPeriod(hgmCore.GetScreenCurrentRefreshRate(id));
            hdiBackend_->SetPendingPeriod(output, pendingPeriod);
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
#ifdef USE_VIDEO_PROCESSING_ENGINE
    GraphicColorGamut colorGamut = ComputeTargetColorGamut(layers);
    GraphicPixelFormat pixelFormat = ComputeTargetPixelFormat(layers);
    auto renderFrameConfig = RSBaseRenderUtil::GetFrameBufferRequestConfig(screenInfo, true, colorGamut, pixelFormat);
    auto skColorSpace = RSBaseRenderEngine::ConvertColorGamutToSkColorSpace(colorGamut);
#else
    auto renderFrameConfig = RSBaseRenderUtil::GetFrameBufferRequestConfig(screenInfo, true);
#endif
    auto renderFrame = uniRenderEngine_->RequestFrame(surface, renderFrameConfig, forceCPU);
    if (renderFrame == nullptr) {
        RS_LOGE("RsDebug RSHardwareThread::Redraw：failed to request frame.");
        return;
    }
    auto canvas = renderFrame->GetCanvas();
    if (canvas == nullptr) {
        RS_LOGE("RsDebug RSHardwareThread::Redraw：canvas is nullptr.");
        return;
    }
#ifdef RS_ENABLE_EGLIMAGE
#ifdef RS_ENABLE_VK
    std::unordered_map<int32_t, std::shared_ptr<NativeVkImageRes>> imageCacheSeqs;
#else // RS_ENABLE_VK
    std::unordered_map<int32_t, std::unique_ptr<ImageCacheSeq>> imageCacheSeqs;
#endif // RS_ENABLE_VK
#endif // RS_ENABLE_EGLIMAGE

    for (const auto& layer : layers) {
        if (layer == nullptr) {
            continue;
        }
        if (layer->GetCompositionType() == GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE ||
            layer->GetCompositionType() == GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE_CLEAR) {
            continue;
        }
#ifndef USE_ROSEN_DRAWING
        auto saveCount = canvas->getSaveCount();

        canvas->save();
        auto dstRect = layer->GetLayerSize();
        SkRect clipRect = SkRect::MakeXYWH(static_cast<float>(dstRect.x), static_cast<float>(dstRect.y),
            static_cast<float>(dstRect.w), static_cast<float>(dstRect.h));
        canvas->clipRect(clipRect);

        // prepare BufferDrawParam
        auto params = RSUniRenderUtil::CreateLayerBufferDrawParam(layer, forceCPU);
        canvas->concat(params.matrix);
#else
        auto saveCount = canvas->GetSaveCount();

        canvas->Save();
        auto dstRect = layer->GetLayerSize();
        Drawing::Rect clipRect = Drawing::Rect(static_cast<float>(dstRect.x), static_cast<float>(dstRect.y),
            static_cast<float>(dstRect.w) + static_cast<float>(dstRect.x),
            static_cast<float>(dstRect.h) + static_cast<float>(dstRect.y));
        canvas->ClipRect(clipRect, Drawing::ClipOp::INTERSECT, false);

        // prepare BufferDrawParam
        auto params = RSUniRenderUtil::CreateLayerBufferDrawParam(layer, forceCPU);
        canvas->ConcatMatrix(params.matrix);
#endif
#ifndef RS_ENABLE_EGLIMAGE
        uniRenderEngine_->DrawBuffer(*canvas, params);
#else
        if (!params.useCPU) {
            if (!RSBaseRenderUtil::IsBufferValid(params.buffer)) {
                RS_LOGE("RSHardwareThread::Redraw CreateEglImageFromBuffer invalid param!");
                continue;
            }
#ifndef USE_ROSEN_DRAWING
#ifdef NEW_SKIA
            if (canvas->recordingContext() == nullptr) {
#else
            if (canvas->getGrContext() == nullptr) {
#endif
#else
            if (canvas->GetGPUContext() == nullptr) {
#endif
                RS_LOGE("RSHardwareThread::Redraw CreateEglImageFromBuffer GrContext is null!");
                continue;
            }
#if defined(RS_ENABLE_GL) && defined(RS_ENABLE_EGLIMAGE)
            auto eglImageCache = uniRenderEngine_->GetEglImageManager()->CreateImageCacheFromBuffer(params.buffer,
                params.acquireFence);
            if (eglImageCache == nullptr) {
                continue;
            }
            auto eglTextureId = eglImageCache->TextureId();
            if (eglTextureId == 0) {
                RS_LOGE("RSHardwareThread::Redraw CreateImageCacheFromBuffer return invalid texture ID");
                continue;
            }
            auto bufferId = params.buffer->GetSeqNum();
            imageCacheSeqs[bufferId] = std::move(eglImageCache);
#endif
#ifndef USE_ROSEN_DRAWING
#if defined(RS_ENABLE_GL) && defined(RS_ENABLE_EGLIMAGE)
            SkColorType colorType = kRGBA_8888_SkColorType;
            auto pixelFmt = params.buffer->GetFormat();
            if (pixelFmt == GRAPHIC_PIXEL_FMT_BGRA_8888) {
                colorType = kBGRA_8888_SkColorType;
            } else if (pixelFmt == GRAPHIC_PIXEL_FMT_YCBCR_P010 || pixelFmt == GRAPHIC_PIXEL_FMT_YCRCB_P010) {
                colorType = kRGBA_1010102_SkColorType;
            }
            auto glType = GL_RGBA8;
            if (pixelFmt == GRAPHIC_PIXEL_FMT_YCBCR_P010 || pixelFmt == GRAPHIC_PIXEL_FMT_YCRCB_P010) {
                glType = GL_RGB10_A2;
            }

            GrGLTextureInfo grExternalTextureInfo = { GL_TEXTURE_EXTERNAL_OES, eglTextureId,
                static_cast<GrGLenum>(glType) };
            GrBackendTexture backendTexture(params.buffer->GetSurfaceBufferWidth(),
                params.buffer->GetSurfaceBufferHeight(), GrMipMapped::kNo, grExternalTextureInfo);
#endif
#ifdef NEW_SKIA
#if defined(RS_ENABLE_GL) && defined(RS_ENABLE_EGLIMAGE)
#ifdef USE_VIDEO_PROCESSING_ENGINE
            auto image = SkImage::MakeFromTexture(canvas->recordingContext(), backendTexture,
                kTopLeft_GrSurfaceOrigin, colorType, kPremul_SkAlphaType, skColorSpace);
#endif
            auto image = SkImage::MakeFromTexture(canvas->recordingContext(), backendTexture,
                kTopLeft_GrSurfaceOrigin, colorType, kPremul_SkAlphaType, nullptr);
#elif defined(RS_ENABLE_VK)
            auto imageCache = uniRenderEngine_->GetVkImageManager()->CreateImageCacheFromBuffer(
                params.buffer, params.acquireFence);
            if (!imageCache) {
                continue;
            }
            auto bufferId = params.buffer->GetSeqNum();
            imageCacheSeqs[bufferId] = imageCache;
            auto& backendTexture = imageCache->GetBackendTexture();
            if (!backendTexture.isValid()) {
                ROSEN_LOGE("RSHardwareThread: backendTexture is not valid!!!");
                return;
            }

            SkColorType colorType = (params.buffer->GetFormat() == GRAPHIC_PIXEL_FMT_BGRA_8888) ?
                kBGRA_8888_SkColorType : kRGBA_8888_SkColorType;
            auto image = SkImage::MakeFromTexture(
                canvas->recordingContext(),
                backendTexture,
                kTopLeft_GrSurfaceOrigin,
                colorType,
                kPremul_SkAlphaType,
                SkColorSpace::MakeSRGB(),
                NativeBufferUtils::DeleteVkImage,
                imageCache->RefCleanupHelper());
#endif
#else
            auto image = SkImage::MakeFromTexture(canvas->getGrContext(), backendTexture,
                kTopLeft_GrSurfaceOrigin, colorType, kPremul_SkAlphaType, nullptr);
#endif
            if (image == nullptr) {
                RS_LOGE("RSHardwareThread::DrawImage: image is nullptr!");
                return;
            }

#ifdef USE_VIDEO_PROCESSING_ENGINE
            sk_sp<SkShader> imageShader = image->makeShader(SkSamplingOptions(SkFilterMode::kLinear));
            if (imageShader == nullptr) {
                RS_LOGE("RSHardwareThread::DrawImage imageShader is nullptr.");
            } else {
                params.paint.setShader(imageShader);
                params.targetColorGamut = colorGamut;

                auto screenManger = CreateOrGetScreenManager();
                params.screenBrightnessNits = screenManager->GetScreenBrightnessNits(screenId);

                uniRenderEngine_->ColorSpaceConvertor(imageShader, params);
            }
#endif

#ifdef NEW_SKIA
            RS_TRACE_NAME_FMT("DrawImage(GPU) seqNum: %d", bufferId);
#ifndef USE_VIDEO_PROCESSING_ENGINE
            canvas->drawImageRect(image, params.srcRect, params.dstRect, SkSamplingOptions(),
                &(params.paint), SkCanvas::kStrict_SrcRectConstraint);
#else
            canvas->drawRect(params.dstRect, (params.paint));
#endif // USE_VIDEO_PROCESSING_ENGINE
#else
            RS_TRACE_NAME_FMT("DrawImage(GPU) seqNum: %d", bufferId);
#ifndef USE_VIDEO_PROCESSING_ENGINE
            canvas->drawImageRect(image, params.srcRect, params.dstRect, &(params.paint));
#else
            canvas->drawRect(params.dstRect, &(params.paint));
#endif // USE_VIDEO_PROCESSING_ENGINE
#endif
#else // USE_ROSEN_DRAWING
#if defined(RS_ENABLE_GL) && defined(RS_ENABLE_EGLIMAGE)
            Drawing::ColorType colorType = (params.buffer->GetFormat() == GRAPHIC_PIXEL_FMT_BGRA_8888) ?
                Drawing::ColorType::COLORTYPE_BGRA_8888 : Drawing::ColorType::COLORTYPE_RGBA_8888;
            Drawing::BitmapFormat bitmapFormat = { colorType, Drawing::AlphaType::ALPHATYPE_PREMUL };

            Drawing::TextureInfo externalTextureInfo;
            externalTextureInfo.SetWidth(params.buffer->GetSurfaceBufferWidth());
            externalTextureInfo.SetHeight(params.buffer->GetSurfaceBufferHeight());
            externalTextureInfo.SetIsMipMapped(false);
            externalTextureInfo.SetTarget(GL_TEXTURE_EXTERNAL_OES);
            externalTextureInfo.SetID(eglTextureId);
            externalTextureInfo.SetFormat(GL_RGBA8);

            auto image = std::make_shared<Drawing::Image>();
            if (!image->BuildFromTexture(*canvas->GetGPUContext(), externalTextureInfo,
                Drawing::TextureOrigin::TOP_LEFT, bitmapFormat, nullptr)) {
                RS_LOGE("RSHardwareThread::Redraw: image BuildFromTexture failed");
                return;
            }
#elif defined RS_ENABLE_VK
            auto imageCache = uniRenderEngine_->GetVkImageManager()->CreateImageCacheFromBuffer(
                params.buffer, params.acquireFence);
            if (!imageCache) {
                continue;
            }
            auto bufferId = params.buffer->GetSeqNum();
            imageCacheSeqs[bufferId] = imageCache;
            auto& backendTexture = imageCache->GetBackendTexture();

            Drawing::ColorType colorType = (params.buffer->GetFormat() == GRAPHIC_PIXEL_FMT_BGRA_8888) ?
                Drawing::ColorType::COLORTYPE_BGRA_8888 : Drawing::ColorType::COLORTYPE_RGBA_8888;
            Drawing::BitmapFormat bitmapFormat = { colorType, Drawing::AlphaType::ALPHATYPE_PREMUL };

            auto image = std::make_shared<Drawing::Image>();
            if (!image->BuildFromTexture(*canvas->GetGPUContext(), backendTexture,
                Drawing::TextureOrigin::TOP_LEFT, bitmapFormat, nullptr,
                NativeBufferUtils::DeleteVkImage,
                imageCache->RefCleanupHelper())) {
                RS_LOGE("RSHardwareThread::Redraw: image BuildFromTexture failed");
                return;
            }
#endif
            canvas->AttachBrush(params.paint);
            RS_TRACE_NAME_FMT("DrawImage(GPU) seqNum: %d", bufferId);
            canvas->DrawImageRect(*image, params.srcRect, params.dstRect,
                Drawing::SamplingOptions(), Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT);
            canvas->DetachBrush();
#endif // USE_ROSEN_DRAWING
        } else {
            uniRenderEngine_->DrawBuffer(*canvas, params);
        }
#endif
#ifndef USE_ROSEN_DRAWING
        canvas->restore();
        canvas->restoreToCount(saveCount);
#else
        canvas->Restore();
        canvas->RestoreToCount(saveCount);
#endif
    }
    renderFrame->Flush();
#ifdef RS_ENABLE_EGLIMAGE
    imageCacheSeqs.clear();
#endif
    RS_LOGD("RsDebug RSHardwareThread::Redraw flush frame buffer end");
}

// private func, guarantee the layer and surface are valid
void RSHardwareThread::LayerPresentTimestamp(const LayerInfoPtr& layer, const sptr<IConsumerSurface>& surface) const
{
    if (!layer->IsSupportedPresentTimestamp()) {
        return;
    }
    const auto& buffer = layer->GetBuffer();
    if (buffer == nullptr) {
        return;
    }
    if (surface->SetPresentTimestamp(buffer->GetSeqNum(), layer->GetPresentTimestamp()) != GSERROR_OK) {
        RS_LOGD("RsDebug RSUniRenderComposerAdapter::LayerPresentTimestamp: SetPresentTimestamp failed");
    }
}

void RSHardwareThread::AddRefreshRateCount(uint32_t rate)
{
    auto [iter, success] = refreshRateCounts_.try_emplace(rate, 1);
    if (!success) {
        iter->second++;
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
            RS_LOGW("RSHardwareThread::ComputeTargetColorGamut Get color space from surface buffer failed");
            continue;
        }

        if (colorSpaceInfo.primaries != COLORPRIMARIES_SRGB) {
            colorGamut = GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
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

        CM_HDR_Metadata_Type metadataType;
        if (MetadataHelper::GetHDRMetadataType(buffer, metadataType) != GSERROR_OK) {
            RS_LOGW("RSHardwareThread::ComputeTargetPixelFormat Get HDR metadata type from surface buffer failed");
            continue;
        }

        if (metadataType != CM_METADATA_NONE) {
            pixelFormat = GRAPHIC_PIXEL_FMT_RGBA_1010102;
        }
    }

    return pixelFormat;
}
#endif
}
