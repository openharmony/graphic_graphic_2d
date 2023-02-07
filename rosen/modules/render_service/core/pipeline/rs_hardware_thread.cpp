/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "pipeline/rs_base_render_util.h"
#include "pipeline/rs_uni_render_util.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_uni_render_engine.h"
#include "platform/common/rs_log.h"
#include "screen_manager/rs_screen_manager.h"
#include "rs_trace.h"
#include "hdi_backend.h"

#ifdef RS_ENABLE_EGLIMAGE
#include "rs_egl_image_manager.h"
#endif // RS_ENABLE_EGLIMAGE

namespace OHOS::Rosen {
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
    fallbackCb_ = std::bind(&RSHardwareThread::Redraw, this,std::placeholders::_1, std::placeholders::_2,
        std::placeholders::_3);
    if (handler_) {
        ScheduleTask([=]() {
            auto screenManager = CreateOrGetScreenManager();
            if (screenManager == nullptr || !screenManager->Init()) {
                RS_LOGE("RSHardwareThread CreateOrGetScreenManager or init fail.");
                return;
            }
        }).wait();
    }
    uniRenderEngine_ = std::make_shared<RSUniRenderEngine>();
    uniRenderEngine_->Init();
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

void RSHardwareThread::ReleaseBuffer(sptr<SurfaceBuffer> buffer, sptr<SyncFence> releaseFence,
    sptr<Surface> cSurface)
{
    if (cSurface == nullptr) {
        RS_LOGE("RsDebug RSHardwareThread:: ReleaseBuffer failed, no consumer!");
        return;
    }

    if (buffer != nullptr) {
        RS_TRACE_NAME("RSHardwareThread::ReleaseBuffer");
        auto ret = cSurface->ReleaseBuffer(buffer, releaseFence);
        if (ret != OHOS::SURFACE_ERROR_OK) {
            RS_LOGE("RsDebug RSHardwareThread::ReleaseBuffer failed(ret: %d)!",
                buffer->GetSeqNum(), ret);
            return;
        }
        // reset prevBuffer if we release it successfully,
        // to avoid releasing the same buffer next frame in some situations.
        buffer = nullptr;
        releaseFence = SyncFence::INVALID_FENCE;
    }
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
    const auto layersReleaseFence = hdiBackend_->GetLayersReleaseFence(output);
    if (layersReleaseFence.size() == 0) {
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
    RSTaskMessage::RSTask task = [this, output = output, layers = layers]() {
        RS_TRACE_NAME("RSHardwareThread::CommitAndReleaseLayers");
        RS_LOGI("RSHardwareThread::CommitAndReleaseLayers start");
        output->SetLayerInfo(layers);
        hdiBackend_->Repaint(output);
        auto layerMap = output->GetLayers();
        ReleaseLayers(output, layerMap);
        RS_LOGI("RSHardwareThread::CommitAndReleaseLayers end");
    };
    PostTask(task);
}

void RSHardwareThread::OnPrepareComplete(sptr<Surface>& surface,
    const PrepareCompleteParam& param, void* data)
{
    // unused data.
    (void)(data);

    if (!param.needFlushFramebuffer) {
        RS_LOGD("RsDebug RSHardwareThread::OnPrepareComplete: no need to flush frame buffer");
        return;
    }

    if (fallbackCb_ != nullptr) {
        fallbackCb_(surface, param.layers, param.screenId);
    }
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
    auto renderFrameConfig = RSBaseRenderUtil::GetFrameBufferRequestConfig(screenInfo, true);
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
    std::unordered_map<int32_t, std::unique_ptr<ImageCacheSeq>> imageCacheSeqs;
#endif
    for (const auto& layer : layers) {
        if (layer == nullptr) {
            continue;
        }
        if (layer->GetCompositionType() == GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE ||
            layer->GetCompositionType() == GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE_CLEAR) {
            continue;
        }
        auto saveCount = canvas->getSaveCount();
        // prepare BufferDrawParam
        auto params = RSUniRenderUtil::CreateLayerBufferDrawParam(layer, forceCPU);
#ifndef RS_ENABLE_EGLIMAGE
        uniRenderEngine_->DrawBuffer(*canvas, params);
#else
        if (!params.useCPU) {
            RS_TRACE_NAME("RSHardwareThread::Redraw DrawImage(GPU)");
            if (!RSBaseRenderUtil::IsBufferValid(params.buffer)) {
                RS_LOGE("RSHardwareThread::Redraw CreateEglImageFromBuffer invalid param!");
                continue;
            }
            if (canvas->getGrContext() == nullptr) {
                RS_LOGE("RSBaseRenderEngine::CreateEglImageFromBuffer GrContext is null!");
                continue;
            }
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
            SkColorType colorType = (params.buffer->GetFormat() == PIXEL_FMT_BGRA_8888) ?
                kBGRA_8888_SkColorType : kRGBA_8888_SkColorType;
            GrGLTextureInfo grExternalTextureInfo = { GL_TEXTURE_EXTERNAL_OES, eglTextureId, GL_RGBA8 };
            GrBackendTexture backendTexture(params.buffer->GetSurfaceBufferWidth(),
                params.buffer->GetSurfaceBufferHeight(), GrMipMapped::kNo, grExternalTextureInfo);
            auto image = SkImage::MakeFromTexture(canvas->getGrContext(), backendTexture,
                kTopLeft_GrSurfaceOrigin, colorType, kPremul_SkAlphaType, nullptr);
            if (image == nullptr) {
                RS_LOGE("RSDividedRenderUtil::DrawImage: image is nullptr!");
                return;
            }
            canvas->drawImageRect(image, params.srcRect, params.dstRect, &(params.paint));
        } else {
            uniRenderEngine_->DrawBuffer(*canvas, params);
        }
#endif
        canvas->restoreToCount(saveCount);
    }
    renderFrame->Flush();
#ifdef RS_ENABLE_EGLIMAGE
    imageCacheSeqs.clear();
#endif
    RS_LOGD("RsDebug RSHardwareThread::Redraw flush frame buffer end");
}

// private func, guarantee the layer and surface are valid
void RSHardwareThread::LayerPresentTimestamp(const LayerInfoPtr& layer, const sptr<Surface>& surface) const
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
}
