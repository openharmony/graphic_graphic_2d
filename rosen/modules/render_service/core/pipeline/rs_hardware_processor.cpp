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

#include <platform/ohos/backend/rs_surface_ohos_gl.h>
#include <platform/ohos/backend/rs_surface_ohos_raster.h>

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
    output_->SetLayerInfo(layers_);
    std::vector<std::shared_ptr<HdiOutput>> outputs{output_};
    if (backend_) {
        backend_->Repaint(outputs);
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

void RSHardwareProcessor::ReleaseNodePrevBuffer(RSSurfaceRenderNode& node)
{
    const auto& consumer = node.GetConsumer();
    if (consumer == nullptr) {
        RS_LOGE("RSHardwareProcessor::ReleaseNodePrevBuffer: node's consumer is null.");
        return;
    }
    if (node.GetPreBuffer() == nullptr) {
        return;
    }
    (void)consumer->ReleaseBuffer(node.GetPreBuffer(), SyncFence::INVALID_FENCE);
}

void RSHardwareProcessor::ProcessSurface(RSSurfaceRenderNode &node)
{
    RS_LOGI("RsDebug RSHardwareProcessor::ProcessSurface start node id:%llu available buffer:%d name:[%s]"\
        "[%d %d %d %d]", node.GetId(), node.GetAvailableBufferCount(), node.GetName().c_str(),
        node.GetDstRect().left_, node.GetDstRect().top_, node.GetDstRect().width_, node.GetDstRect().height_);
    OHOS::sptr<SurfaceBuffer> cbuffer;
    RSProcessor::SpecialTask task = [] () -> void {};
    bool ret = ConsumeAndUpdateBuffer(node, task, cbuffer);
    if (!ret) {
        RS_LOGI("RsDebug RSHardwareProcessor::ProcessSurface consume buffer fail");
        return;
    }
    if (!output_) {
        RS_LOGE("RSHardwareProcessor::ProcessSurface output is nullptr");
        ReleaseNodePrevBuffer(node);
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
        ReleaseNodePrevBuffer(node);
        return;
    }

    auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(node.GetRenderProperties().GetBoundsGeometry());
    if (geoPtr == nullptr) {
        RS_LOGE("RsDebug RSHardwareProcessor::ProcessSurface geoPtr == nullptr");
        ReleaseNodePrevBuffer(node);
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
            .gAlpha = node.GetGlobalAlhpa() * 255,
        },
        .buffer = node.GetBuffer(),
        .fence = node.GetFence(),
        .preBuffer = node.GetPreBuffer(),
        .preFence = node.GetPreFence(),
        .blendType = node.GetBlendType(),
    };
    if (info.dstRect.w <= 0 || info.dstRect.h <= 0) {
        ReleaseNodePrevBuffer(node);
        return;
    }
    RectI originDstRect(geoPtr->GetAbsRect().left_ - offsetX_, geoPtr->GetAbsRect().top_ - offsetY_,
            geoPtr->GetAbsRect().width_, geoPtr->GetAbsRect().height_);
    RectI clipRegion(info.dstRect.x, info.dstRect.y, info.dstRect.w, info.dstRect.h);
    CalculateSrcRect(info, clipRegion, originDstRect);
    std::string inf;
    char strBuffer[UINT8_MAX] = { 0 };
    if (sprintf_s(strBuffer, UINT8_MAX, "ProcessSurfaceNode:%s XYWH[%d %d %d %d]", node.GetName().c_str(),
        info.dstRect.x, info.dstRect.y, info.dstRect.w, info.dstRect.h) != -1) {
        inf.append(strBuffer);
    }
    RS_TRACE_NAME(inf.c_str());
    std::shared_ptr<HdiLayerInfo> layer = HdiLayerInfo::CreateHdiLayerInfo();
    RS_LOGI("RsDebug RSHardwareProcessor::ProcessSurface surfaceNode id:%llu name:[%s] dst [%d %d %d %d]"\
        "SrcRect [%d %d] rawbuffer [%d %d] surfaceBuffer [%d %d] buffaddr:%p, z:%f, globalZOrder:%d, blendType = %d",
        node.GetId(), node.GetName().c_str(),
        info.dstRect.x, info.dstRect.y, info.dstRect.w, info.dstRect.h, info.srcRect.w, info.srcRect.h,
        node.GetBuffer()->GetWidth(), node.GetBuffer()->GetHeight(), node.GetBuffer()->GetSurfaceBufferWidth(),
        node.GetBuffer()->GetSurfaceBufferHeight(), node.GetBuffer().GetRefPtr(),
        node.GetRenderProperties().GetPositionZ(), info.zOrder, info.blendType);
    RsRenderServiceUtil::ComposeSurface(layer, node.GetConsumer(), layers_, info, &node);
    if (info.buffer->GetSurfaceBufferColorGamut() != static_cast<ColorGamut>(currScreenInfo_.colorGamut)) {
        layer->SetCompositionType(CompositionType::COMPOSITION_CLIENT);
    }
}

void RSHardwareProcessor::CalculateSrcRect(ComposeInfo& info, RectI clipRegion, RectI originDstRect)
{
    info.srcRect.x = clipRegion.IsEmpty() ? 0 : std::ceil((clipRegion.left_ - originDstRect.left_) *
        info.srcRect.w / originDstRect.width_);
    info.srcRect.y = clipRegion.IsEmpty() ? 0 : std::ceil((clipRegion.top_ - originDstRect.top_) *
        info.srcRect.h / originDstRect.height_);
    info.srcRect.w = originDstRect.IsEmpty() ? 0 : info.srcRect.w * clipRegion.width_ / originDstRect.width_;
    info.srcRect.h = originDstRect.IsEmpty() ? 0 : info.srcRect.h * clipRegion.height_ / originDstRect.height_;
}

bool IfUseGPUClient(const struct PrepareCompleteParam& param)
{
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
    }
    return true;
}

void RSHardwareProcessor::Redraw(
    sptr<Surface>& surface, const struct PrepareCompleteParam& param, void* data)
{
    if (!param.needFlushFramebuffer) {
        RS_LOGI("RsDebug RSHardwareProcessor::Redraw no need to flush frame buffer");
        return;
    }
    if (surface == nullptr) {
        RS_LOGE("RSHardwareProcessor::Redraw: surface is null.");
        return;
    }
    RS_LOGI("RsDebug RSHardwareProcessor::Redraw flush frame buffer start");
    BufferRequestConfig requestConfig = {
        .width = static_cast<int32_t>(currScreenInfo_.width),
        .height = static_cast<int32_t>(currScreenInfo_.height),
        .strideAlignment = 0x8,
        .format = PIXEL_FMT_RGBA_8888,      // [PLANNING] different soc need different format
        .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA | HBM_USE_MEM_FB,
        .timeout = 0,
    };
    RS_TRACE_NAME("Redraw");
    bool ifUseGPU = IfUseGPUClient(param);
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
                    RsRenderServiceUtil::DealAnimation(canvas, node, params, center);
            });
        } else {
            RsRenderServiceUtil::DrawBuffer(*canvas, params, [this, &node, &center](RSPaintFilterCanvas& canvas,
                BufferDrawParam& params) -> void {
                    RsRenderServiceUtil::DealAnimation(canvas, node, params, center);
            });
        }
#else
        RsRenderServiceUtil::DrawBuffer(*canvas, params, [this, &node, &center](RSPaintFilterCanvas& canvas,
            BufferDrawParam& params) -> void {
            RsRenderServiceUtil::DealAnimation(canvas, node, params, center);
        });
#endif // RS_ENABLE_GL
    }
    rsSurface_->FlushFrame(currFrame_);
}

void RSHardwareProcessor::OnRotate()
{
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
                RS_LOGI("RsDebug RSHardwareProcessor::OnRotate 90 Before Rotate layer size [%d %d %d %d]",
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
                RS_LOGI("RsDebug RSHardwareProcessor::OnRotate After Rotate layer size [%d %d %d %d]",
                    layer->GetLayerSize().x, layer->GetLayerSize().y, layer->GetLayerSize().w, layer->GetLayerSize().h);
                break;
            }
            case ScreenRotation::ROTATION_180: {
                RS_LOGI("RsDebug RSHardwareProcessor::OnRotate 180 Before Rotate layer size [%d %d %d %d]",
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
                RS_LOGI("RsDebug RSHardwareProcessor::OnRotate After Rotate layer size [%d %d %d %d]",
                    layer->GetLayerSize().x, layer->GetLayerSize().y, layer->GetLayerSize().w, layer->GetLayerSize().h);
                break;
            }
            case ScreenRotation::ROTATION_270: {
                RS_LOGI("RsDebug RSHardwareProcessor::OnRotate 270 Before Rotate layer size [%d %d %d %d]",
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
                RS_LOGI("RsDebug RSHardwareProcessor::OnRotate After Rotate layer size [%d %d %d %d]",
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
