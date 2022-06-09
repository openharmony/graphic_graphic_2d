/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include <algorithm>
#include <securec.h>
#include <string>

#include "common/rs_rect.h"
#include "common/rs_vector2.h"
#include "common/rs_vector3.h"
#include "common/rs_vector4.h"
#include "display_type.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkRect.h"
#include "rs_render_service.h"
#include "rs_trace.h"
#include "sync_fence.h"
#include "common/rs_vector4.h"
#include "pipeline/rs_draw_cmd_list.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_render_service_util.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include <platform/ohos/backend/rs_surface_ohos_gl.h>
#include <platform/ohos/backend/rs_surface_ohos_raster.h>
#include "property/rs_properties_painter.h"

namespace OHOS {
namespace Rosen {
RSHardwareProcessor::RSHardwareProcessor() {}

RSHardwareProcessor::~RSHardwareProcessor() {}

void RSHardwareProcessor::Init(ScreenId id, int32_t offsetX, int32_t offsetY)
{
    offsetX_ = offsetX;
    offsetY_ = offsetY;
    backend_ = HdiBackend::GetInstance();
    backend_->RegPrepareComplete(std::bind(&RSHardwareProcessor::Redraw, this, std::placeholders::_1,
        std::placeholders::_2, std::placeholders::_3), nullptr);
    screenManager_ = CreateOrGetScreenManager();
    if (!screenManager_) {
        RS_LOGE("RSHardwareProcessor::Init ScreenManager is nullptr");
        return;
    }
    rotation_ = screenManager_->GetRotation(id);
    output_ = screenManager_->GetOutput(id);
    if (!output_) {
        RS_LOGE("RSHardwareProcessor::Init output_ is nullptr");
        return;
    }
    currScreenInfo_ = screenManager_->QueryScreenInfo(id);
    RS_LOGI("RSHardwareProcessor::Init screen w:%d, w:%d", currScreenInfo_.width, currScreenInfo_.height);
    IRect damageRect;
    damageRect.x = 0;
    damageRect.y = 0;
    damageRect.w = static_cast<int32_t>(currScreenInfo_.width);
    damageRect.h = static_cast<int32_t>(currScreenInfo_.height);
    output_->SetOutputDamage(1, damageRect);

#ifdef RS_ENABLE_GL
    auto mainThread = RSMainThread::Instance();
    if (mainThread != nullptr) {
        renderContext_ = mainThread->GetRenderContext();
        eglImageManager_ =  mainThread->GetRSEglImageManager();
    }
#endif // RS_ENABLE_GL
}

void RSHardwareProcessor::PostProcess()
{
    if (output_ == nullptr) {
        RS_LOGE("RSHardwareProcessor::PostProcess output is nullptr");
        return;
    }
    // Rotaion must be executed before CropLayers.
    OnRotate();
    CropLayers();
    ScaleDownLayers();
    output_->SetLayerInfo(layers_);
    std::vector<std::shared_ptr<HdiOutput>> outputs{output_};
    if (backend_) {
        backend_->Repaint(outputs);
        const auto layersReleaseFence = backend_->GetLayersReleaseFence(output_);
        for (const auto& [layer, fence] : layersReleaseFence) {
            if (layerToNodeMap_.count(layer) == 0) {
                continue;
            }
            auto *node = layerToNodeMap_.at(layer);
            if (node == nullptr) {
                continue;
            }
            node->SetReleaseFence(fence);
        }
    }
}

void RSHardwareProcessor::CropLayers()
{
    for (auto layer : layers_) {
        IRect dstRect = layer->GetLayerSize();
        IRect srcRect = layer->GetCropRect();
        IRect originSrcRect = srcRect;

        RectI dstRectI(dstRect.x, dstRect.y, dstRect.w, dstRect.h);
        int32_t screenWidth = static_cast<int32_t>(currScreenInfo_.width);
        int32_t screenHeight = static_cast<int32_t>(currScreenInfo_.height);
        RectI screenRectI(0, 0, screenWidth, screenHeight);
        RectI resDstRect = dstRectI.IntersectRect(screenRectI);
        if (resDstRect == dstRectI) {
            continue;
        }
        dstRect = {
            .x = resDstRect.left_,
            .y = resDstRect.top_,
            .w = resDstRect.width_,
            .h = resDstRect.height_,
        };
        srcRect.x = resDstRect.IsEmpty() ? 0 : std::ceil((resDstRect.left_ - dstRectI.left_) *
            originSrcRect.w / dstRectI.width_);
        srcRect.y = resDstRect.IsEmpty() ? 0 : std::ceil((resDstRect.top_ - dstRectI.top_) *
            originSrcRect.h / dstRectI.height_);
        srcRect.w = dstRectI.IsEmpty() ? 0 : originSrcRect.w * resDstRect.width_ / dstRectI.width_;
        srcRect.h = dstRectI.IsEmpty() ? 0 : originSrcRect.h * resDstRect.height_ / dstRectI.height_;
        layer->SetLayerSize(dstRect);
        layer->SetDirtyRegion(srcRect);
        layer->SetCropRect(srcRect);
        RS_LOGD("RsDebug RSHardwareProcessor::CropLayers layer has been cropped dst[%d %d %d %d] src[%d %d %d %d]",
            dstRect.x, dstRect.y, dstRect.w, dstRect.h, srcRect.x, srcRect.y, srcRect.w, srcRect.h);
    }
}

void RSHardwareProcessor::ScaleDownLayers()
{
    for (auto layer : layers_) {
        if (layer->GetBuffer()->GetSurfaceBufferScalingMode() == ScalingMode::SCALING_MODE_SCALE_CROP) {
            IRect dstRect = layer->GetLayerSize();
            IRect srcRect = layer->GetCropRect();

            uint32_t newWidth = static_cast<uint32_t>(srcRect.w);
            uint32_t newHeight = static_cast<uint32_t>(srcRect.h);

            if (newWidth * dstRect.h > newHeight * dstRect.w) {
                // too wide
                newWidth = dstRect.w * newHeight / dstRect.h;
            } else if (newWidth * dstRect.h < newHeight * dstRect.w) {
                // too tall
                newHeight = dstRect.h * newWidth / dstRect.w;
            } else {
                continue;
            }

            uint32_t currentWidth = static_cast<uint32_t>(srcRect.w);
            uint32_t currentHeight = static_cast<uint32_t>(srcRect.h);

            if (newWidth < currentWidth) {
                // the crop is too wide
                uint32_t dw = currentWidth - newWidth;
                auto halfdw = dw / 2;
                srcRect.x += halfdw;
                srcRect.w = newWidth;
            } else {
                // thr crop is too tall
                uint32_t dh = currentHeight - newHeight;
                auto halfdh = dh / 2;
                srcRect.y += halfdh;
                srcRect.h = newHeight;
            }
            layer->SetDirtyRegion(srcRect);
            layer->SetCropRect(srcRect);
            RS_LOGD("RsDebug RSHardwareProcessor::ScaleDownLayers layer has been scaledown dst[%d %d %d %d]"\
                "src[%d %d %d %d]", dstRect.x, dstRect.y, dstRect.w, dstRect.h,
                srcRect.x, srcRect.y, srcRect.w, srcRect.h);
        }
    }
}

void RSHardwareProcessor::ProcessSurface(RSSurfaceRenderNode &node)
{
    RS_LOGD("RsDebug RSHardwareProcessor::ProcessSurface start node id:%llu available buffer:%d name:[%s]"\
        "[%d %d %d %d]", node.GetId(), node.GetAvailableBufferCount(), node.GetName().c_str(),
        node.GetDstRect().left_, node.GetDstRect().top_, node.GetDstRect().width_, node.GetDstRect().height_);
    if (!output_) {
        RS_LOGE("RSHardwareProcessor::ProcessSurface output is nullptr");
        return;
    }
    if (node.GetBuffer() == nullptr) {
        return;
    }

    uint32_t boundWidth = currScreenInfo_.width;
    uint32_t boundHeight = currScreenInfo_.height;
    if (rotation_ == ScreenRotation::ROTATION_90 || rotation_ == ScreenRotation::ROTATION_270) {
        std::swap(boundWidth, boundHeight);
    }
    if (node.GetRenderProperties().GetBoundsPositionX() >= boundWidth ||
        node.GetRenderProperties().GetBoundsPositionY() >= boundHeight) {
        RS_LOGE("RsDebug RSHardwareProcessor::ProcessSurface this node:%llu no need to composite", node.GetId());
        return;
    }

    auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(node.GetRenderProperties().GetBoundsGeometry());
    if (geoPtr == nullptr) {
        RS_LOGE("RsDebug RSHardwareProcessor::ProcessSurface geoPtr == nullptr");
        return;
    }

    if (!node.IsNotifyRTBufferAvailable()) {
        // Only ipc for one time.
        RS_LOGD("RsDebug RSHardwareProcessor::ProcessSurface id = %llu "\
                "Notify RT buffer available", node.GetId());
        node.NotifyRTBufferAvailable();
    }
    ComposeInfo info = {
        .srcRect = {
            .x = 0,
            .y = 0,
            .w = node.GetBuffer()->GetSurfaceBufferWidth(),
            .h = node.GetBuffer()->GetSurfaceBufferHeight(),
        },
        .dstRect = {
            .x = node.GetDstRect().left_,
            .y = node.GetDstRect().top_,
            .w = node.GetDstRect().width_,
            .h = node.GetDstRect().height_,
        },
        .visibleRect = {
            .x = 0,
            .y = 0,
            .w = static_cast<int32_t>(currScreenInfo_.width),
            .h = static_cast<int32_t>(currScreenInfo_.height),
        },
        .zOrder = node.GetGlobalZOrder(),
        .alpha = {
            .enGlobalAlpha = true,
            .gAlpha = node.GetGlobalAlpha() * 255,
        },
        .buffer = node.GetBuffer(),
        .fence = node.GetAcquireFence(),
        .blendType = node.GetBlendType(),
    };
    if (info.dstRect.w <= 0 || info.dstRect.h <= 0) {
        return;
    }
    CalculateSrcRect(info, node.GetSrcRatio());
    std::string inf;
    char strBuffer[UINT8_MAX] = { 0 };
    if (sprintf_s(strBuffer, UINT8_MAX, "ProcessSurfaceNode:%s XYWH[%d %d %d %d]", node.GetName().c_str(),
        info.dstRect.x, info.dstRect.y, info.dstRect.w, info.dstRect.h) != -1) {
        inf.append(strBuffer);
    }
    RS_TRACE_NAME(inf.c_str());
    std::shared_ptr<HdiLayerInfo> layer = HdiLayerInfo::CreateHdiLayerInfo();
    RS_LOGD("RsDebug RSHardwareProcessor::ProcessSurface surfaceNode id:%llu name:[%s] dst [%d %d %d %d]"\
        "SrcRect [%d %d] rawbuffer [%d %d] surfaceBuffer [%d %d] buffaddr:%p, z:%f, globalZOrder:%d, blendType = %d",
        node.GetId(), node.GetName().c_str(),
        info.dstRect.x, info.dstRect.y, info.dstRect.w, info.dstRect.h, info.srcRect.w, info.srcRect.h,
        node.GetBuffer()->GetWidth(), node.GetBuffer()->GetHeight(), node.GetBuffer()->GetSurfaceBufferWidth(),
        node.GetBuffer()->GetSurfaceBufferHeight(), node.GetBuffer().GetRefPtr(),
        node.GetRenderProperties().GetPositionZ(), info.zOrder, info.blendType);
    RsRenderServiceUtil::ComposeSurface(layer, node.GetConsumer(), layers_, info, &node);
    layerToNodeMap_[layer] = &(static_cast<RSSurfaceHandler&>(node));
    if (info.buffer->GetSurfaceBufferColorGamut() != static_cast<ColorGamut>(currScreenInfo_.colorGamut)) {
        layer->SetCompositionType(CompositionType::COMPOSITION_CLIENT);
    }
}

void RSHardwareProcessor::ProcessSurface(RSDisplayRenderNode& node)
{
    RS_LOGI("RSHardwareProcessor::ProcessSurface displayNode id:%llu available buffer:%d", node.GetId(),
        node.GetAvailableBufferCount());
    if (!output_) {
        RS_LOGE("RSHardwareProcessor::ProcessSurface output is nullptr");
        return;
    }
    if (!RsRenderServiceUtil::ConsumeAndUpdateBuffer(node)) {
        RS_LOGE("RSHardwareProcessor::ProcessSurface consume buffer fail");
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
            .x = 0,
            .y = 0,
            .w = static_cast<int32_t>(currScreenInfo_.width),
            .h = static_cast<int32_t>(currScreenInfo_.height),
        },
        .visibleRect = {
            .x = 0,
            .y = 0,
            .w = static_cast<int32_t>(currScreenInfo_.width),
            .h = static_cast<int32_t>(currScreenInfo_.height),
        },
        .zOrder = static_cast<int32_t>(node.GetGlobalZOrder()),
        .alpha = {
            .enGlobalAlpha = false,
        },
        .buffer = node.GetBuffer(),
        .fence = node.GetAcquireFence(),
        .blendType = BLEND_NONE,
    };
    std::shared_ptr<HdiLayerInfo> layer = HdiLayerInfo::CreateHdiLayerInfo();
    RS_LOGI("RSHardwareProcessor::ProcessSurface displayNode id:%llu dst [%d %d %d %d]"\
        "SrcRect [%d %d] rawbuffer [%d %d] surfaceBuffer [%d %d] buffaddr:%p, globalZOrder:%d, blendType = %d",
        node.GetId(),
        info.dstRect.x, info.dstRect.y, info.dstRect.w, info.dstRect.h, info.srcRect.w, info.srcRect.h,
        node.GetBuffer()->GetWidth(), node.GetBuffer()->GetHeight(), node.GetBuffer()->GetSurfaceBufferWidth(),
        node.GetBuffer()->GetSurfaceBufferHeight(), node.GetBuffer().GetRefPtr(),
        info.zOrder, info.blendType);
    RsRenderServiceUtil::ComposeSurface(layer, node.GetConsumer(), layers_, info, &node);
    layerToNodeMap_[layer] = &(static_cast<RSSurfaceHandler&>(node));
}

void RSHardwareProcessor::CalculateSrcRect(ComposeInfo& info, const Vector4f& ratio)
{
    info.srcRect.x = info.srcRect.w * ratio.x_;
    info.srcRect.y = info.srcRect.h * ratio.y_;
    info.srcRect.w = info.srcRect.w * ratio.z_;
    info.srcRect.h = info.srcRect.h * ratio.w_;
}

bool IfUseGPUClient(const struct PrepareCompleteParam& param)
{
    bool ifDeviceComp = true;
    for (auto it = param.layers.begin(); it != param.layers.end(); ++it) {
        LayerInfoPtr layerInfo = *it;
        if (layerInfo == nullptr) {
            continue;
        }
        RSSurfaceRenderNode* nodePtr = static_cast<RSSurfaceRenderNode *>(layerInfo->GetLayerAdditionalInfo());
        if (nodePtr == nullptr) {
            RS_LOGE("RSHardwareProcessor::DrawBuffer surfaceNode is nullptr!");
            continue;
        }
        auto buffer = nodePtr->GetBuffer();
        ColorGamut srcGamut = static_cast<ColorGamut>(buffer->GetSurfaceBufferColorGamut());
        ColorGamut dstGamut = ColorGamut::COLOR_GAMUT_SRGB;
        if (buffer->GetFormat() == PIXEL_FMT_YCRCB_420_SP || buffer->GetFormat() == PIXEL_FMT_YCBCR_420_SP) {
            return false;
        } else if (srcGamut != dstGamut) {
            return false;
        }
        if (layerInfo->GetCompositionType() == CompositionType::COMPOSITION_CLIENT) {
            ifDeviceComp = false;
        }
    }
    if (ifDeviceComp == true) {
        return false;
    }
    return true;
}

void DrawBufferPostProcess(RSPaintFilterCanvas& canvas, RSSurfaceRenderNode& node, BufferDrawParam& params,
    Vector2f& center)
{
    RsRenderServiceUtil::DealAnimation(canvas, node, params, center);
    RectF maskBounds(0, 0, params.dstRect.width(), params.dstRect.height());
    RSPropertiesPainter::DrawMask(node.GetRenderProperties(), canvas, RSPropertiesPainter::Rect2SkRect(maskBounds));
}

void RSHardwareProcessor::Redraw(
    sptr<Surface>& surface, const struct PrepareCompleteParam& param, void* data)
{
    if (!param.needFlushFramebuffer) {
        RS_LOGD("RsDebug RSHardwareProcessor::Redraw no need to flush frame buffer");
        return;
    }
    if (surface == nullptr) {
        RS_LOGE("RSHardwareProcessor::Redraw: surface is null.");
        return;
    }
    RS_LOGD("RsDebug RSHardwareProcessor::Redraw flush frame buffer start");
    BufferRequestConfig requestConfig = {
        .width = static_cast<int32_t>(currScreenInfo_.width),
        .height = static_cast<int32_t>(currScreenInfo_.height),
        .strideAlignment = 0x8,
        .format = PIXEL_FMT_RGBA_8888,      // [PLANNING] different soc need different format
        .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA | HBM_USE_MEM_FB,
        .timeout = 0,
    };
    bool isUni = RSSystemProperties::GetUniRenderEnabledType() != UniRenderEnabledType::UNI_RENDER_DISABLED;
    RS_TRACE_NAME("Redraw");
    bool ifUseGPU = !isUni && IfUseGPUClient(param);
    RS_LOGE("RSHardwareProcessor::Redraw if use GPU client: %d!", ifUseGPU);
#ifdef RS_ENABLE_GL
    if (ifUseGPU) {
        rsSurface_ = std::make_shared<RSSurfaceOhosGl>(surface);
        rsSurface_->SetSurfaceBufferUsage(HBM_USE_CPU_READ | HBM_USE_MEM_DMA | HBM_USE_MEM_FB);
    } else {
        rsSurface_ = std::make_shared<RSSurfaceOhosRaster>(surface);
        rsSurface_->SetSurfaceBufferUsage(HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA | HBM_USE_MEM_FB);
    }
#else
    rsSurface_ = std::make_shared<RSSurfaceOhosRaster>(surface);
    rsSurface_->SetSurfaceBufferUsage(HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA | HBM_USE_MEM_FB);
#endif
    auto skCanvas = CreateCanvas(rsSurface_, requestConfig);
    if (skCanvas == nullptr) {
        RS_LOGE("RSHardwareProcessor::Redraw: canvas is null.");
        return;
    }
    std::unique_ptr<RSPaintFilterCanvas> canvas = std::make_unique<RSPaintFilterCanvas>(skCanvas);
    for (auto it = param.layers.begin(); it != param.layers.end(); ++it) {
        if (isUni) {
            break;
        }
        LayerInfoPtr layerInfo = *it;
        if (layerInfo == nullptr) {
            continue;
        }
        RSSurfaceRenderNode* nodePtr = static_cast<RSSurfaceRenderNode *>(layerInfo->GetLayerAdditionalInfo());
        if (nodePtr == nullptr) {
            RS_LOGE("RSHardwareProcessor::DrawBuffer surfaceNode is nullptr!");
            continue;
        }
        RSSurfaceRenderNode& node = *nodePtr;
        std::string info;
        char strBuffer[UINT8_MAX] = { 0 };
        if (sprintf_s(strBuffer, UINT8_MAX, "Node name:%s DstRect[%d %d %d %d]", node.GetName().c_str(),
            layerInfo->GetLayerSize().x, layerInfo->GetLayerSize().y, layerInfo->GetLayerSize().w,
            layerInfo->GetLayerSize().h) != -1) {
            info.append(strBuffer);
        }
        RS_TRACE_NAME(info.c_str());
        if (layerInfo->GetCompositionType() == CompositionType::COMPOSITION_DEVICE) {
            continue;
        }
        RS_LOGD("RsDebug RSHardwareProcessor::Redraw layer composition Type:%d, [%d %d %d %d]",
            layerInfo->GetCompositionType(), layerInfo->GetLayerSize().x, layerInfo->GetLayerSize().y,
            layerInfo->GetLayerSize().w, layerInfo->GetLayerSize().h);
        int saveCount = canvas->getSaveCount();
        auto params = RsRenderServiceUtil::CreateBufferDrawParam(node, currScreenInfo_.rotationMatrix, rotation_);
        params.targetColorGamut = static_cast<ColorGamut>(currScreenInfo_.colorGamut);
        const auto& clipRect = layerInfo->GetLayerSize();
        params.clipRect = SkRect::MakeXYWH(clipRect.x, clipRect.y, clipRect.w, clipRect.h);
        Vector2f center(node.GetDstRect().left_ + node.GetDstRect().width_ * 0.5f,
            node.GetDstRect().top_ + node.GetDstRect().height_ * 0.5f);
#ifdef RS_ENABLE_GL
        if (ifUseGPU) {
            RsRenderServiceUtil::DrawImage(eglImageManager_, renderContext_->GetGrContext(), *canvas, params,
                [this, &node, &center](RSPaintFilterCanvas& canvas, BufferDrawParam& params) -> void {
                    DrawBufferPostProcess(canvas, node, params, center);
            });
            auto consumerSurface = node.GetConsumer();
            GSError error = consumerSurface->RegisterDeleteBufferListener([eglImageManager = eglImageManager_]
                (int32_t bufferId) {eglImageManager->UnMapEglImageFromSurfaceBuffer(bufferId);
            });
            if (error != GSERROR_OK) {
                RS_LOGE("RSHardwareProcessor::Redraw: fail to register UnMapEglImage callback.");
            }
        } else {
            RsRenderServiceUtil::DrawBuffer(*canvas, params, [this, &node, &center](RSPaintFilterCanvas& canvas,
                BufferDrawParam& params) -> void {
                    DrawBufferPostProcess(canvas, node, params, center);
            });
        }
        canvas->restoreToCount(saveCount);
#else
        RsRenderServiceUtil::DrawBuffer(*canvas, params, [this, &node, &center](RSPaintFilterCanvas& canvas,
            BufferDrawParam& params) -> void {
            DrawBufferPostProcess(canvas, node, params, center);
        });
        canvas->restoreToCount(saveCount);
#endif // RS_ENABLE_GL
    }
    rsSurface_->FlushFrame(currFrame_);
#ifdef RS_ENABLE_GL
    eglImageManager_->ShrinkCachesIfNeeded();
#endif // RS_ENABLE_GL
}

void RSHardwareProcessor::OnRotate()
{
    if (RSSystemProperties::GetUniRenderEnabledType() != UniRenderEnabledType::UNI_RENDER_DISABLED) {
        return;
    }
    int32_t width = static_cast<int32_t>(currScreenInfo_.width);
    int32_t height = static_cast<int32_t>(currScreenInfo_.height);
    for (auto& layer: layers_) {
        IRect rect = layer->GetLayerSize();
        RSSurfaceRenderNode* nodePtr = static_cast<RSSurfaceRenderNode *>(layer->GetLayerAdditionalInfo());
        if (nodePtr == nullptr) {
            RS_LOGE("RSHardwareProcessor::DrawBuffer surfaceNode is nullptr!");
            continue;
        }
        sptr<Surface> surface = nodePtr->GetConsumer();
        if (surface == nullptr) {
            continue;
        }
        switch (rotation_) {
            case ScreenRotation::ROTATION_90: {
                RS_LOGD("RsDebug RSHardwareProcessor::OnRotate 90 Before Rotate layer size [%d %d %d %d]",
                    rect.x, rect.y, rect.w, rect.h);
                layer->SetLayerSize({rect.y, height - rect.x - rect.w, rect.h, rect.w});
                switch (surface->GetTransform()) {
                    case TransformType::ROTATE_90: {
                        layer->SetTransform(TransformType::ROTATE_180);
                        break;
                    }
                    case TransformType::ROTATE_180: {
                        layer->SetTransform(TransformType::ROTATE_90);
                        break;
                    }
                    case TransformType::ROTATE_270: {
                        layer->SetTransform(TransformType::ROTATE_NONE);
                        break;
                    }
                    default: {
                        layer->SetTransform(TransformType::ROTATE_270);
                        break;
                    }
                }
                RS_LOGD("RsDebug RSHardwareProcessor::OnRotate After Rotate layer size [%d %d %d %d]",
                    layer->GetLayerSize().x, layer->GetLayerSize().y, layer->GetLayerSize().w, layer->GetLayerSize().h);
                break;
            }
            case ScreenRotation::ROTATION_180: {
                RS_LOGD("RsDebug RSHardwareProcessor::OnRotate 180 Before Rotate layer size [%d %d %d %d]",
                    rect.x, rect.y, rect.w, rect.h);
                layer->SetLayerSize({width - rect.x - rect.w, height - rect.y - rect.h, rect.w, rect.h});
                switch (surface->GetTransform()) {
                    case TransformType::ROTATE_90: {
                        layer->SetTransform(TransformType::ROTATE_90);
                        break;
                    }
                    case TransformType::ROTATE_180: {
                        layer->SetTransform(TransformType::ROTATE_NONE);
                        break;
                    }
                    case TransformType::ROTATE_270: {
                        layer->SetTransform(TransformType::ROTATE_270);
                        break;
                    }
                    default: {
                        layer->SetTransform(TransformType::ROTATE_180);
                        break;
                    }
                }
                RS_LOGD("RsDebug RSHardwareProcessor::OnRotate After Rotate layer size [%d %d %d %d]",
                    layer->GetLayerSize().x, layer->GetLayerSize().y, layer->GetLayerSize().w, layer->GetLayerSize().h);
                break;
            }
            case ScreenRotation::ROTATION_270: {
                RS_LOGD("RsDebug RSHardwareProcessor::OnRotate 270 Before Rotate layer size [%d %d %d %d]",
                    rect.x, rect.y, rect.w, rect.h);
                layer->SetLayerSize({width - rect.y - rect.h, rect.x, rect.h, rect.w});
                switch (surface->GetTransform()) {
                    case TransformType::ROTATE_90: {
                        layer->SetTransform(TransformType::ROTATE_NONE);
                        break;
                    }
                    case TransformType::ROTATE_180: {
                        layer->SetTransform(TransformType::ROTATE_270);
                        break;
                    }
                    case TransformType::ROTATE_270: {
                        layer->SetTransform(TransformType::ROTATE_180);
                        break;
                    }
                    default: {
                        layer->SetTransform(TransformType::ROTATE_90);
                        break;
                    }
                }
                RS_LOGD("RsDebug RSHardwareProcessor::OnRotate After Rotate layer size [%d %d %d %d]",
                    layer->GetLayerSize().x, layer->GetLayerSize().y, layer->GetLayerSize().w, layer->GetLayerSize().h);
                break;
            }
            default:  {
                switch (surface->GetTransform()) {
                    case TransformType::ROTATE_90: {
                        layer->SetTransform(TransformType::ROTATE_270);
                        break;
                    }
                    case TransformType::ROTATE_180: {
                        layer->SetTransform(TransformType::ROTATE_180);
                        break;
                    }
                    case TransformType::ROTATE_270: {
                        layer->SetTransform(TransformType::ROTATE_90);
                        break;
                    }
                    default: {
                        layer->SetTransform(TransformType::ROTATE_NONE);
                        break;
                    }
                }
                break;
            }
        }
    }
}
} // namespace Rosen
} // namespace OHOS
