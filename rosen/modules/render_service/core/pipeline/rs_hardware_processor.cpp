/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "pipeline/rs_hardware_processor.h"
#include "display_type.h"
#include "pipeline/rs_main_thread.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSHardwareProcessor::RSHardwareProcessor() {}

RSHardwareProcessor::~RSHardwareProcessor() {}

void RSHardwareProcessor::Init(ScreenId id)
{
    backend_ = HdiBackend::GetInstance();
    backend_->RegPrepareComplete(std::bind(&RSHardwareProcessor::Redraw, this, std::placeholders::_1,
        std::placeholders::_2, std::placeholders::_3), nullptr);
    screenManager_ = CreateOrGetScreenManager();
    if (!screenManager_) {
        ROSEN_LOGE("RSHardwareProcessor::Init ScreenManager is nullptr");
        return;
    }
    rotation_ = screenManager_->GetRotation(id);
    output_ = screenManager_->GetOutput(id);
    if (!output_) {
        ROSEN_LOGE("RSHardwareProcessor::Init output_ is nullptr");
        return;
    }
    currScreenInfo_ = screenManager_->QueryScreenInfo(id);
    ROSEN_LOGI("RSHardwareProcessor::Init screen w:%d, w:%d", currScreenInfo_.width, currScreenInfo_.height);
    IRect damageRect;
    damageRect.x = 0;
    damageRect.y = 0;
    damageRect.w = currScreenInfo_.width;
    damageRect.h = currScreenInfo_.height;
    output_->SetOutputDamage(1, damageRect);
}

void RSHardwareProcessor::PostProcess()
{
    if (output_ == nullptr) {
        ROSEN_LOGE("RSHardwareProcessor::PostProcess output is nullptr");
        return;
    }
    // Rotaion must be executed before CropLayers.
    if (rotation_ != ScreenRotation::ROTATION_0) {
        OnRotate();
    }
    CropLayers();
    output_->SetLayerInfo(layers_);
    std::vector<std::shared_ptr<HdiOutput>> outputs{output_};
    if (backend_) {
        backend_->Repaint(outputs);
    }
}

void RSHardwareProcessor::CropLayers()
{
    for (auto layer : layers_) {
        bool cut = false;
        IRect dstRect = layer->GetLayerSize();
        IRect srcRect = layer->GetCropRect();
        IRect orgSrcRect = srcRect;
        int32_t screenWidth = currScreenInfo_.width;
        int32_t screenHeight = currScreenInfo_.height;
        if (dstRect.x < 0 && dstRect.x + dstRect.w > 0) {
            srcRect.w = srcRect.w * (dstRect.w + dstRect.x) / dstRect.w;
            srcRect.x = orgSrcRect.w - srcRect.w;
            dstRect.w = dstRect.w + dstRect.x;
            dstRect.x = 0;
            cut = true;
        }
        if (dstRect.x + dstRect.w > screenWidth) {
            srcRect.w = srcRect.w * (screenWidth - dstRect.x) / dstRect.w;
            dstRect.w = screenWidth - dstRect.x;
            cut = true;
        }
        if (dstRect.y < 0 && dstRect.y + dstRect.h > 0) {
            srcRect.h = srcRect.h * (dstRect.h + dstRect.y) / dstRect.h;
            srcRect.y = orgSrcRect.h - srcRect.h;
            dstRect.h = dstRect.h + dstRect.y;
            dstRect.y = 0;
            cut = true;
        }
        if (dstRect.y + dstRect.h > screenHeight) {
            srcRect.h = srcRect.h * (screenHeight - dstRect.y) / dstRect.h;
            dstRect.h = screenHeight - dstRect.y;
            cut = true;
        }
        if (cut) {
            layer->SetLayerSize(dstRect);
            layer->SetDirtyRegion(srcRect);
            layer->SetCropRect(srcRect);
            ROSEN_LOGD("RsDebug RSHardwareProcessor::PostProcess: layer has been cropped to fit the Screen");
        }
    }
}

void RSHardwareProcessor::ProcessSurface(RSSurfaceRenderNode &node)
{
    ROSEN_LOGI("RsDebug RSHardwareProcessor::ProcessSurface start node id:%llu available buffer:%d name:[%s]",
        node.GetId(), node.GetAvailableBufferCount(), node.GetName().c_str());
    if (!output_) {
        ROSEN_LOGE("RSHardwareProcessor::ProcessSurface output is nullptr");
        return;
    }
    if (node.GetRenderProperties().GetBoundsPositionX() >= currScreenInfo_.width ||
        node.GetRenderProperties().GetBoundsPositionY() >= currScreenInfo_.height) {
        ROSEN_LOGE("RsDebug RSHardwareProcessor::ProcessSurface this node:%llu no need to composite", node.GetId());
        return;
    }
    OHOS::sptr<SurfaceBuffer> cbuffer;
    RSProcessor::SpecialTask task = [] () -> void{};
    bool ret = ConsumeAndUpdateBuffer(node, task, cbuffer);
    if (!ret) {
        ROSEN_LOGI("RsDebug RSHardwareProcessor::ProcessSurface consume buffer fail");
        return;
    }
    if (!node.IsBufferAvailable()) {
        // Only ipc for one time.
        ROSEN_LOGI("RsDebug RSHardwareProcessor::ProcessSurface id = %llu Notify RT buffer available", node.GetId());
        node.NotifyBufferAvailable(true);
    }
    auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(node.GetRenderProperties().GetBoundsGeometry());
    if (geoPtr == nullptr) {
        ROSEN_LOGE("RsDebug RSHardwareProcessor::ProcessSurface geoPtr == nullptr");
        return;
    }
    ComposeInfo info = {
        .srcRect = {
            .x = 0,
            .y = 0,
            .w = node.GetBuffer()->GetSurfaceBufferWidth(),
            .h = node.GetBuffer()->GetSurfaceBufferHeight(),
        },
        .dstRect = {
            .x = geoPtr->GetAbsRect().left_,
            .y = geoPtr->GetAbsRect().top_,
            .w = geoPtr->GetAbsRect().width_,
            .h = geoPtr->GetAbsRect().height_,
        },
        .visibleRect = {
            .x = 0,
            .y = 0,
            .w = currScreenInfo_.width,
            .h = currScreenInfo_.height,
        },
        .zOrder = node.GetGlobalZOrder(),
        .alpha = {
            .enGlobalAlpha = true,
            .gAlpha = node.GetAlpha() * node.GetRenderProperties().GetAlpha() * 255,
        },
        .buffer = node.GetBuffer(),
        .fence = node.GetFence(),
        .preBuffer = node.GetPreBuffer(),
        .preFence = node.GetPreFence(),
        .blendType = node.GetBlendType(),
    };
    auto transitionProperties = node.GetAnimationManager().GetTransitionProperties();
    CalculateInfo(transitionProperties, info, node);
    std::shared_ptr<HdiLayerInfo> layer = HdiLayerInfo::CreateHdiLayerInfo();
    ROSEN_LOGE("RsDebug RSHardwareProcessor::ProcessSurface surfaceNode id:%llu name:[%s] dst [%d %d %d %d]"\
        "SrcRect [%d %d] rawbuffer [%d %d] surfaceBuffer [%d %d] buffaddr:%p, z:%f, globalZOrder:%d, blendType = %d",
        node.GetId(), node.GetName().c_str(),
        info.dstRect.x, info.dstRect.y, info.dstRect.w, info.dstRect.h, info.srcRect.w, info.srcRect.h,
        node.GetBuffer()->GetWidth(), node.GetBuffer()->GetHeight(), node.GetBuffer()->GetSurfaceBufferWidth(),
        node.GetBuffer()->GetSurfaceBufferHeight(), node.GetBuffer().GetRefPtr(),
        node.GetRenderProperties().GetPositionZ(), info.zOrder, info.blendType);
    RsRenderServiceUtil::ComposeSurface(layer, node.GetConsumer(), layers_, info, &node);
    if (info.buffer->GetSurfaceBufferColorGamut() != static_cast<SurfaceColorGamut>(currScreenInfo_.colorGamut)) {
        layer->SetCompositionType(CompositionType::COMPOSITION_CLIENT);
    }
}

void RSHardwareProcessor::CalculateInfo(const std::unique_ptr<RSTransitionProperties>& transitionProperties,
    ComposeInfo& info, RSSurfaceRenderNode& node)
{
    if (!transitionProperties) {
        return;
    }
    auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(node.GetRenderProperties().GetBoundsGeometry());
    if (geoPtr == nullptr) {
        ROSEN_LOGE("RsDebug RSHardwareProcessor::ProcessSurface geoPtr == nullptr");
        return;
    }
    float paddingX = (1 - transitionProperties->GetScale().x_) * geoPtr->GetAbsRect().width_ / 2;
    float paddingY = (1 - transitionProperties->GetScale().y_) * geoPtr->GetAbsRect().height_ / 2;
    info.dstRect = {
        .x = geoPtr->GetAbsRect().left_ + transitionProperties->GetTranslate().x_ + paddingX,
        .y = geoPtr->GetAbsRect().top_ + transitionProperties->GetTranslate().y_ + paddingY,
        .w = geoPtr->GetAbsRect().width_ * transitionProperties->GetScale().x_,
        .h = geoPtr->GetAbsRect().height_ * transitionProperties->GetScale().y_,
    };
    info.alpha = {
        .enGlobalAlpha = true,
        .gAlpha = node.GetAlpha() * node.GetRenderProperties().GetAlpha() * transitionProperties->GetAlpha() * 255,
    };
}

void RSHardwareProcessor::Redraw(sptr<Surface>& surface, const struct PrepareCompleteParam& param, void* data)
{
    if (!param.needFlushFramebuffer) {
        ROSEN_LOGI("RsDebug RSHardwareProcessor::Redraw no need to flush frame buffer");
        return;
    }

    if (surface == nullptr) {
        ROSEN_LOGE("RSHardwareProcessor::Redraw: surface is null.");
        return;
    }
    ROSEN_LOGI("RsDebug RSHardwareProcessor::Redraw flush frame buffer start");
    BufferRequestConfig requestConfig = {
        .width = currScreenInfo_.width,
        .height = currScreenInfo_.height,
        .strideAlignment = 0x8,
        .format = PIXEL_FMT_RGBA_8888,      // [PLANNING] different soc need different format
        .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA | HBM_USE_MEM_FB,
        .timeout = 0,
    };
    auto canvas = CreateCanvas(surface, requestConfig);
    if (canvas == nullptr) {
        ROSEN_LOGE("RSHardwareProcessor::Redraw: canvas is null.");
        return;
    }

    for (auto it = param.layers.rbegin(); it != param.layers.rend(); ++it) {
        LayerInfoPtr layerInfo = *it;
        if (layerInfo == nullptr || layerInfo->GetCompositionType() == CompositionType::COMPOSITION_DEVICE) {
            continue;
        }

        ROSEN_LOGD("RsDebug RSHardwareProcessor::Redraw layer composition Type:%d, [%d %d %d %d]",
            layerInfo->GetCompositionType(), layerInfo->GetLayerSize().x, layerInfo->GetLayerSize().y,
            layerInfo->GetLayerSize().w, layerInfo->GetLayerSize().h);

        sptr<SurfaceBuffer> buffer = layerInfo->GetBuffer();
        SurfaceColorGamut bufferColorGamut = buffer->GetSurfaceBufferColorGamut();
        if (bufferColorGamut == static_cast<SurfaceColorGamut>(currScreenInfo_.colorGamut)) {
            RsRenderServiceUtil::DrawBuffer(canvas.get(), buffer,
                *(static_cast<RSSurfaceRenderNode *>(layerInfo->GetLayerAdditionalInfo())));
        } else {
            ROSEN_LOGW("RSHardwareProcessor::Redraw: need to convert color gamut.");
            RsRenderServiceUtil::DrawBuffer(canvas.get(), buffer,
                *(static_cast<RSSurfaceRenderNode *>(layerInfo->GetLayerAdditionalInfo())),
                static_cast<ColorGamut>(currScreenInfo_.colorGamut));
        }
    }
    BufferFlushConfig flushConfig = {
        .damage = {
            .x = 0,
            .y = 0,
            .w = currScreenInfo_.width,
            .h = currScreenInfo_.height,
        },
    };
    FlushBuffer(surface, flushConfig);
}

void RSHardwareProcessor::OnRotate()
{
    int32_t width = currScreenInfo_.width;
    int32_t height = currScreenInfo_.height;
    for (auto& layer: layers_) {
        IRect rect = layer->GetLayerSize();
        ROSEN_LOGI("RsDebug RSHardwareProcessor::OnRotate Before Rotate layer size [%d %d %d %d]",
            rect.x, rect.y, rect.w, rect.h);
        switch (rotation_) {
            case ScreenRotation::ROTATION_90: {
                ROSEN_LOGI("RsDebug RSHardwareProcessor::OnRotate 90.");
                layer->SetLayerSize({rect.y, height - rect.x - rect.w, rect.h, rect.w});
                layer->SetTransform(TransformType::ROTATE_270);
                break;
            }
            case ScreenRotation::ROTATION_180: {
                ROSEN_LOGI("RsDebug RSHardwareProcessor::OnRotate 180.");
                layer->SetLayerSize({width - rect.x - rect.w, height - rect.y - rect.h, rect.w, rect.h});
                layer->SetTransform(TransformType::ROTATE_180);
                break;
            }
            case ScreenRotation::ROTATION_270: {
                ROSEN_LOGI("RsDebug RSHardwareProcessor::OnRotate 270.");
                layer->SetLayerSize({width - rect.y - rect.h, rect.x, rect.h, rect.w});
                layer->SetTransform(TransformType::ROTATE_90);
                break;
            }
            case ScreenRotation::INVALID_SCREEN_ROTATION: {
                ROSEN_LOGE("RsDebug RSHardwareProcessor::OnRotate Failed.");
                layer->SetTransform(TransformType::ROTATE_BUTT);
                break;
            }
            default:
                break;
        }
        ROSEN_LOGI("RsDebug RSHardwareProcessor::OnRotate After Rotate layer size [%d %d %d %d]",
            layer->GetLayerSize().x, layer->GetLayerSize().y, layer->GetLayerSize().w, layer->GetLayerSize().h);
    }
}
} // namespace Rosen
} // namespace OHOS