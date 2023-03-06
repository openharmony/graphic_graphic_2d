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

#include "rs_uni_render_composer_adapter.h"

#include "common/rs_common_def.h"
#include "common/rs_obj_abs_geometry.h"
#include "pipeline/rs_uni_render_util.h"
#include "platform/common/rs_log.h"
#include "rs_divided_render_util.h"
#include "rs_trace.h"
#include "string_utils.h"

#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
#include "pipeline/driven_render/rs_driven_surface_render_node.h"
#endif

namespace OHOS {
namespace Rosen {
using namespace std;
bool RSUniRenderComposerAdapter::Init(const ScreenInfo& screenInfo, int32_t offsetX, int32_t offsetY,
    float mirrorAdaptiveCoefficient)
{
    hdiBackend_ = HdiBackend::GetInstance();
    if (hdiBackend_ == nullptr) {
        RS_LOGE("RSUniRenderComposerAdapter::Init: hdiBackend is nullptr");
        return false;
    }
    auto screenManager = CreateOrGetScreenManager();
    if (screenManager == nullptr) {
        RS_LOGE("RSUniRenderComposerAdapter::Init: ScreenManager is nullptr");
        return false;
    }
    output_ = screenManager->GetOutput(ToScreenPhysicalId(screenInfo.id));
    if (output_ == nullptr) {
        RS_LOGE("RSUniRenderComposerAdapter::Init: output_ is nullptr");
        return false;
    }

    offsetX_ = offsetX;
    offsetY_ = offsetY;
    mirrorAdaptiveCoefficient_ = mirrorAdaptiveCoefficient;
    screenInfo_ = screenInfo;

    GraphicIRect damageRect {0, 0, static_cast<int32_t>(screenInfo_.width), static_cast<int32_t>(screenInfo_.height)};
    output_->SetOutputDamage(1, damageRect);
    bool directClientCompEnableStatus = RSSystemProperties::GetDirectClientCompEnableStatus();
    output_->SetDirectClientCompEnableStatus(directClientCompEnableStatus);

#if (defined RS_ENABLE_GL) && (defined RS_ENABLE_EGLIMAGE)
    // enable direct GPU composition.
    output_->SetLayerCompCapacity(LAYER_COMPOSITION_CAPACITY);
#else // (defined RS_ENABLE_GL) && (defined RS_ENABLE_EGLIMAGE)
    output_->SetLayerCompCapacity(LAYER_COMPOSITION_CAPACITY_INVALID);
#endif // (defined RS_ENABLE_GL) && (defined RS_ENABLE_EGLIMAGE)
    return true;
}

void RSUniRenderComposerAdapter::CommitLayers(const std::vector<LayerInfoPtr>& layers)
{
    if (hdiBackend_ == nullptr) {
        RS_LOGE("RSUniRenderComposerAdapter::CommitLayers: backend is nullptr");
        return;
    }

    if (output_ == nullptr) {
        RS_LOGE("RSUniRenderComposerAdapter::CommitLayers: output is nullptr");
        return;
    }
    RSHardwareThread::Instance().CommitAndReleaseLayers(output_, layers);
}

void RSUniRenderComposerAdapter::SetPreBufferInfo(RSSurfaceHandler& surfaceHandler, ComposeInfo& info) const
{
    if (surfaceHandler.IsPreBufferReleased()) {
        // reset prevBuffer if same layer has been committed successfully,
        // to avoid releasing the same buffer next frame in some situations.
        info.preBuffer = nullptr;
    } else {
        info.preBuffer = surfaceHandler.GetPreBuffer().buffer;
        surfaceHandler.SetPreBufferReleased(true);
    }
}

// private func, for RSDisplayRenderNode
ComposeInfo RSUniRenderComposerAdapter::BuildComposeInfo(RSDisplayRenderNode& node) const
{
    const auto& buffer = node.GetBuffer(); // we guarantee the buffer is valid.
    ComposeInfo info {};
    info.srcRect = GraphicIRect {0, 0, buffer->GetSurfaceBufferWidth(), buffer->GetSurfaceBufferHeight()};
    info.dstRect = GraphicIRect {
        0,
        0,
        static_cast<int32_t>(static_cast<float>(screenInfo_.GetRotatedWidth()) * mirrorAdaptiveCoefficient_),
        static_cast<int32_t>(static_cast<float>(screenInfo_.GetRotatedHeight()) * mirrorAdaptiveCoefficient_)
    };
    info.boundRect = info.dstRect;
    info.visibleRect = GraphicIRect {info.dstRect.x, info.dstRect.y, info.dstRect.w, info.dstRect.h};
    info.zOrder = static_cast<int32_t>(node.GetGlobalZOrder());
    info.alpha.enGlobalAlpha = true;
    info.alpha.gAlpha = 255;
    SetPreBufferInfo(node, info);
    info.buffer = buffer;
    info.fence = node.GetAcquireFence();
    info.blendType = GRAPHIC_BLEND_SRCOVER;
    info.needClient = GetComposerInfoNeedClient(info, node);
    info.matrix = GraphicMatrix {1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f};
    return info;
}

ComposeInfo RSUniRenderComposerAdapter::BuildComposeInfo(RSDrivenSurfaceRenderNode& node) const
{
#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
    const auto& buffer = node.GetBuffer(); // we guarantee the buffer is valid.
    const RectI dstRect = node.GetDstRect();
    const auto& srcRect = node.GetSrcRect();
    ComposeInfo info {};
    info.srcRect = GraphicIRect {srcRect.left_, srcRect.top_, srcRect.width_, srcRect.height_};
    info.dstRect = GraphicIRect {dstRect.left_, dstRect.top_, dstRect.width_, dstRect.height_};
    info.boundRect = info.dstRect;
    info.visibleRect = info.dstRect;
    info.zOrder = static_cast<int32_t>(node.GetGlobalZOrder());
    info.alpha.enGlobalAlpha = true;
    info.alpha.gAlpha = 255;
    SetPreBufferInfo(node, info);
    info.buffer = buffer;
    info.fence = node.GetAcquireFence();
    info.blendType = GRAPHIC_BLEND_SRCOVER;
    info.needClient = false;
    info.matrix = GraphicMatrix {1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f};
    return info;
#else
    return {};
#endif
}

void RSUniRenderComposerAdapter::SetComposeInfoToLayer(
    const LayerInfoPtr& layer,
    const ComposeInfo& info,
    const sptr<IConsumerSurface>& surface,
    RSBaseRenderNode* node) const
{
    if (layer == nullptr) {
        return;
    }
    layer->SetSurface(surface);
    layer->SetBuffer(info.buffer, info.fence);
    layer->SetPreBuffer(info.preBuffer);
    layer->SetZorder(info.zOrder);
    layer->SetAlpha(info.alpha);
    layer->SetLayerSize(info.dstRect);
    layer->SetBoundSize(info.boundRect);
    layer->SetCompositionType(info.needClient ?
        GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT : GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    layer->SetVisibleRegion(1, info.visibleRect);
    layer->SetDirtyRegion(info.srcRect);
    layer->SetBlendType(info.blendType);
    layer->SetCropRect(info.srcRect);
    layer->SetMatrix(info.matrix);
    HDRMetaDataType type;
    if (surface->QueryMetaDataType(info.buffer->GetSeqNum(), type) != GSERROR_OK) {
        RS_LOGE("RSUniRenderComposerAdapter::SetComposeInfoToLayer: QueryMetaDataType failed");
        return;
    }
    switch (type) {
        case HDRMetaDataType::HDR_META_DATA: {
            std::vector<GraphicHDRMetaData> metaData;
            if (surface->GetMetaData(info.buffer->GetSeqNum(), metaData) != GSERROR_OK) {
                RS_LOGE("RSUniRenderComposerAdapter::SetComposeInfoToLayer: GetMetaData failed");
                return;
            }
            layer->SetMetaData(metaData);
            break;
        }
        case HDRMetaDataType::HDR_META_DATA_SET: {
            GraphicHDRMetadataKey key;
            std::vector<uint8_t> metaData;
            if (surface->GetMetaDataSet(info.buffer->GetSeqNum(), key, metaData) != GSERROR_OK) {
                RS_LOGE("RSUniRenderComposerAdapter::SetComposeInfoToLayer: GetMetaDataSet failed");
                return;
            }
            GraphicHDRMetaDataSet metaDataSet;
            metaDataSet.key = key;
            metaDataSet.metaData = metaData;
            layer->SetMetaDataSet(metaDataSet);
            break;
        }
        case HDRMetaDataType::HDR_NOT_USED: {
            RS_LOGD("RSUniRenderComposerAdapter::SetComposeInfoToLayer: HDR is not used");
            break;
        }
        default:  {
            break;
        }
    }
}

void RSUniRenderComposerAdapter::GetComposerInfoSrcRect(ComposeInfo &info, const RSSurfaceRenderNode& node)
{
    const auto& property = node.GetRenderProperties();
    const auto bufferWidth = info.buffer->GetSurfaceBufferWidth();
    const auto bufferHeight = info.buffer->GetSurfaceBufferHeight();
    const auto boundsWidth = property.GetBoundsWidth();
    const auto boundsHeight = property.GetBoundsHeight();
    if (bufferWidth != boundsWidth || bufferHeight != boundsHeight) {
        float xScale = (ROSEN_EQ(boundsWidth, 0.0f) ? 1.0f : bufferWidth / boundsWidth);
        float yScale = (ROSEN_EQ(boundsHeight, 0.0f) ? 1.0f : bufferHeight / boundsHeight);
        info.srcRect.x = info.srcRect.x * xScale;
        info.srcRect.y = info.srcRect.y * yScale;
        info.srcRect.w = info.srcRect.w * xScale;
        info.srcRect.h = info.srcRect.h * yScale;
    }
}

bool RSUniRenderComposerAdapter::GetComposerInfoNeedClient(const ComposeInfo &info, RSRenderNode& node) const
{
    bool needClient = RSBaseRenderUtil::IsNeedClient(node, info);
    if (info.buffer->GetSurfaceBufferColorGamut() != static_cast<GraphicColorGamut>(screenInfo_.colorGamut)) {
        needClient = true;
    }
    return needClient;
}

void RSUniRenderComposerAdapter::DealWithNodeGravity(const RSSurfaceRenderNode& node, ComposeInfo& info) const
{
    const auto& property = node.GetRenderProperties();
    const float frameWidth = info.buffer->GetSurfaceBufferWidth();
    const float frameHeight = info.buffer->GetSurfaceBufferHeight();
    const float boundsWidth = property.GetBoundsWidth();
    const float boundsHeight = property.GetBoundsHeight();
    const Gravity frameGravity = property.GetFrameGravity();
    // we do not need to do additional works for Gravity::RESIZE and if frameSize == boundsSize.
    if (frameGravity == Gravity::RESIZE || (frameWidth == boundsWidth && frameHeight == boundsHeight)) {
        return;
    }

    auto traceInfo = node.GetName() + " DealWithNodeGravity " + std::to_string(static_cast<int>(frameGravity));
    RS_TRACE_NAME(traceInfo.c_str());

    // get current node's translate matrix and calculate gravity matrix.
    auto translateMatrix = SkMatrix::MakeTrans(
        std::ceil(node.GetTotalMatrix().getTranslateX()), std::ceil(node.GetTotalMatrix().getTranslateY()));
    SkMatrix gravityMatrix;
    (void)RSPropertiesPainter::GetGravityMatrix(frameGravity,
        RectF {0.0f, 0.0f, boundsWidth, boundsHeight}, frameWidth, frameHeight, gravityMatrix);

    // create a canvas to calculate new dstRect and new srcRect
    int32_t screenWidth = screenInfo_.width;
    int32_t screenHeight = screenInfo_.height;
    const auto screenRotation = screenInfo_.rotation;
    if (screenRotation == ScreenRotation::ROTATION_90 || screenRotation == ScreenRotation::ROTATION_270) {
        std::swap(screenWidth, screenHeight);
    }
    auto canvas = std::make_unique<SkCanvas>(screenWidth, screenHeight);
    canvas->concat(translateMatrix);
    canvas->concat(gravityMatrix);
    SkRect clipRect;
    gravityMatrix.mapRect(&clipRect, SkRect::MakeWH(frameWidth, frameHeight));
    canvas->clipRect(SkRect::MakeWH(clipRect.width(), clipRect.height()));
    SkIRect newDstRect = canvas->getDeviceClipBounds();
    // we make the newDstRect as the intersection of new and old dstRect,
    // to deal with the situation that frameSize > boundsSize.
    newDstRect.intersect(SkIRect::MakeXYWH(info.dstRect.x, info.dstRect.y, info.dstRect.w, info.dstRect.h));
    auto localRect = canvas->getLocalClipBounds();
    int left = std::clamp<int>(localRect.left(), 0, frameWidth);
    int top = std::clamp<int>(localRect.top(), 0, frameHeight);
    int width = std::clamp<int>(localRect.width(), 0, frameWidth - left);
    int height = std::clamp<int>(localRect.height(), 0, frameHeight - top);
    GraphicIRect newSrcRect = {left, top, width, height};

    // log and apply new dstRect and srcRect
    RS_LOGD("RsDebug DealWithNodeGravity: name[%s], gravity[%d], oldDstRect[%d %d %d %d], newDstRect[%d %d %d %d],"\
        " oldSrcRect[%d %d %d %d], newSrcRect[%d %d %d %d].",
        node.GetName().c_str(), static_cast<int>(frameGravity),
        info.dstRect.x, info.dstRect.y, info.dstRect.w, info.dstRect.h,
        newDstRect.left(), newDstRect.top(), newDstRect.width(), newDstRect.height(),
        info.srcRect.x, info.srcRect.y, info.srcRect.w, info.srcRect.h,
        newSrcRect.x, newSrcRect.y, newSrcRect.w, newSrcRect.h);
    info.dstRect = {newDstRect.left(), newDstRect.top(), newDstRect.width(), newDstRect.height()};
    info.srcRect = newSrcRect;
}

// private func, for RSSurfaceRenderNode.
ComposeInfo RSUniRenderComposerAdapter::BuildComposeInfo(RSSurfaceRenderNode& node) const
{
    const auto& dstRect = node.GetDstRect();
    const auto& srcRect = node.GetSrcRect();
    ComposeInfo info {};
    info.srcRect = GraphicIRect {srcRect.left_, srcRect.top_, srcRect.width_, srcRect.height_};
    info.dstRect = GraphicIRect {
        static_cast<int32_t>(static_cast<float>(dstRect.left_) * mirrorAdaptiveCoefficient_),
        static_cast<int32_t>(static_cast<float>(dstRect.top_) * mirrorAdaptiveCoefficient_),
        static_cast<int32_t>(static_cast<float>(dstRect.width_) * mirrorAdaptiveCoefficient_),
        static_cast<int32_t>(static_cast<float>(dstRect.height_) * mirrorAdaptiveCoefficient_)
    };
    info.zOrder = static_cast<int32_t>(node.GetGlobalZOrder());
    info.alpha.enGlobalAlpha = true;
    info.alpha.gAlpha = node.GetGlobalAlpha() * 255; // map gAlpha from float(0, 1) to uint8_t(0, 255).
    info.fence = node.GetAcquireFence();
    info.blendType = node.GetBlendType();
    const auto& buffer = node.GetBuffer();
    info.buffer = buffer;
    SetPreBufferInfo(node, info);
    GetComposerInfoSrcRect(info, node);
    info.needClient = GetComposerInfoNeedClient(info, node);
    DealWithNodeGravity(node, info);

    info.dstRect.x -= static_cast<int32_t>(static_cast<float>(offsetX_) * mirrorAdaptiveCoefficient_);
    info.dstRect.y -= static_cast<int32_t>(static_cast<float>(offsetY_) * mirrorAdaptiveCoefficient_);
    info.visibleRect = info.dstRect;
    auto totalMatrix = node.GetTotalMatrix();
    info.matrix = GraphicMatrix {totalMatrix[0], totalMatrix[1], totalMatrix[2],
                                 totalMatrix[3], totalMatrix[4], totalMatrix[5],
                                 totalMatrix[6], totalMatrix[7], totalMatrix[8]};

    const auto& property = node.GetRenderProperties();
    info.boundRect = { 0, 0,
        static_cast<int32_t>(property.GetBoundsWidth()), static_cast<int32_t>(property.GetBoundsHeight())};
    return info;
}

bool RSUniRenderComposerAdapter::CheckStatusBeforeCreateLayer(RSSurfaceRenderNode& node) const
{
    if (output_ == nullptr) {
        RS_LOGE("RSUniRenderComposerAdapter::CheckStatusBeforeCreateLayer: output is nullptr");
        return false;
    }

    const auto& buffer = node.GetBuffer();
    if (buffer == nullptr) {
        RS_LOGD("RsDebug RSUniRenderComposerAdapter::CheckStatusBeforeCreateLayer:node(%" PRIu64 ") has no available buffer.",
            node.GetId());
        return false;
    }
    const auto& dstRect = node.GetDstRect();
    const auto& srcRect = node.GetSrcRect();
    // check if the node's srcRect and dstRect are valid.
    if (srcRect.width_ <= 0 || srcRect.height_ <= 0 || dstRect.width_ <= 0 || dstRect.height_ <= 0) {
        return false;
    }

    auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(node.GetRenderProperties().GetBoundsGeometry());
    if (geoPtr == nullptr) {
        RS_LOGW("RsDebug RSUniRenderComposerAdapter::CheckStatusBeforeCreateLayer: node(%" PRIu64 ")'s geoPtr is nullptr!",
            node.GetId());
        return false;
    }
    return true;
}

// private func, guarantee the layer is valid
void RSUniRenderComposerAdapter::LayerCrop(const LayerInfoPtr& layer) const
{
    GraphicIRect dstRect = layer->GetLayerSize();
    GraphicIRect srcRect = layer->GetCropRect();
    GraphicIRect originSrcRect = srcRect;

    RectI dstRectI(dstRect.x, dstRect.y, dstRect.w, dstRect.h);
    int32_t screenWidth = static_cast<int32_t>(screenInfo_.width);
    int32_t screenHeight = static_cast<int32_t>(screenInfo_.height);
    RectI screenRectI(0, 0, screenWidth, screenHeight);
    RectI resDstRect = dstRectI.IntersectRect(screenRectI);
    if (resDstRect == dstRectI) {
        return;
    }
    dstRect = {resDstRect.left_, resDstRect.top_, resDstRect.width_, resDstRect.height_};
    srcRect.x = resDstRect.IsEmpty() ? 0 : std::ceil((resDstRect.left_ - dstRectI.left_) *
        originSrcRect.w / dstRectI.width_);
    srcRect.y = resDstRect.IsEmpty() ? 0 : std::ceil((resDstRect.top_ - dstRectI.top_) *
        originSrcRect.h / dstRectI.height_);
    srcRect.w = dstRectI.IsEmpty() ? 0 : originSrcRect.w * resDstRect.width_ / dstRectI.width_;
    srcRect.h = dstRectI.IsEmpty() ? 0 : originSrcRect.h * resDstRect.height_ / dstRectI.height_;
    layer->SetLayerSize(dstRect);
    layer->SetDirtyRegion(srcRect);
    layer->SetCropRect(srcRect);
    RS_LOGD("RsDebug RSUniRenderComposerAdapter::LayerCrop layer has been cropped dst[%d %d %d %d] src[%d %d %d %d]",
        dstRect.x, dstRect.y, dstRect.w, dstRect.h, srcRect.x, srcRect.y, srcRect.w, srcRect.h);
}

// private func, guarantee the layer is valid
void RSUniRenderComposerAdapter::LayerScaleDown(const LayerInfoPtr& layer)
{
    ScalingMode scalingMode = ScalingMode::SCALING_MODE_SCALE_TO_WINDOW;
    const auto& buffer = layer->GetBuffer();
    const auto& surface = layer->GetSurface();
    if (buffer == nullptr || surface == nullptr) {
        return;
    }

    if (surface->GetScalingMode(buffer->GetSeqNum(), scalingMode) == GSERROR_OK &&
        scalingMode == ScalingMode::SCALING_MODE_SCALE_CROP) {
        GraphicIRect dstRect = layer->GetLayerSize();
        GraphicIRect srcRect = layer->GetCropRect();

        uint32_t newWidth = static_cast<uint32_t>(srcRect.w);
        uint32_t newHeight = static_cast<uint32_t>(srcRect.h);
        uint32_t dstWidth = static_cast<uint32_t>(dstRect.w);
        uint32_t dstHeight = static_cast<uint32_t>(dstRect.h);

        if (newWidth * dstHeight > newHeight * dstWidth) {
            // too wide
            newWidth = dstWidth * newHeight / dstHeight;
        } else if (newWidth * dstHeight < newHeight * dstWidth) {
            // too tall
            newHeight = dstHeight * newWidth / dstWidth;
        } else {
            return;
        }

        uint32_t currentWidth = static_cast<uint32_t>(srcRect.w);
        uint32_t currentHeight = static_cast<uint32_t>(srcRect.h);

        if (newWidth < currentWidth) {
            // the crop is too wide
            uint32_t dw = currentWidth - newWidth;
            auto halfdw = dw / 2;
            srcRect.x += static_cast<int32_t>(halfdw);
            srcRect.w = static_cast<int32_t>(newWidth);
        } else {
            // thr crop is too tall
            uint32_t dh = currentHeight - newHeight;
            auto halfdh = dh / 2;
            srcRect.y += static_cast<int32_t>(halfdh);
            srcRect.h = static_cast<int32_t>(newHeight);
        }
        layer->SetDirtyRegion(srcRect);
        layer->SetCropRect(srcRect);
        RS_LOGD("RsDebug RSUniRenderComposerAdapter::LayerScaleDown layer has been scaledown dst[%d %d %d %d]"\
            "src[%d %d %d %d]", dstRect.x, dstRect.y, dstRect.w, dstRect.h,
            srcRect.x, srcRect.y, srcRect.w, srcRect.h);
    }
}

// private func
bool RSUniRenderComposerAdapter::IsOutOfScreenRegion(const ComposeInfo& info) const
{
    int32_t boundWidth = static_cast<int32_t>(screenInfo_.width);
    int32_t boundHeight = static_cast<int32_t>(screenInfo_.height);
    ScreenRotation rotation = screenInfo_.rotation;
    if (rotation == ScreenRotation::ROTATION_90 || rotation == ScreenRotation::ROTATION_270) {
        std::swap(boundWidth, boundHeight);
    }

    const auto& dstRect = info.dstRect;
    if (dstRect.x + dstRect.w <= 0 ||
        dstRect.x >= boundWidth ||
        dstRect.y + dstRect.h <= 0 ||
        dstRect.y >= boundHeight) {
        return true;
    }

    return false;
}

LayerInfoPtr RSUniRenderComposerAdapter::CreateBufferLayer(RSSurfaceRenderNode& node) const
{
    if (!CheckStatusBeforeCreateLayer(node)) {
        return nullptr;
    }
    ComposeInfo info = BuildComposeInfo(node);
    if (IsOutOfScreenRegion(info)) {
        RS_LOGD("RsDebug RSUniRenderComposerAdapter::CreateBufferLayer: node(%" PRIu64
                ") out of screen region, no need to composite.",
            node.GetId());
        return nullptr;
    }
    std::string traceInfo;
    AppendFormat(traceInfo, "CreateLayer:%s XYWH[%d %d %d %d]", node.GetName().c_str(),
        info.dstRect.x, info.dstRect.y, info.dstRect.w, info.dstRect.h);
    RS_TRACE_NAME(traceInfo.c_str());
    RS_LOGD(
        "RsDebug RSUniRenderComposerAdapter::CreateBufferLayer surfaceNode id:%" PRIu64 " name:[%s] dst [%d %d %d %d]"
        "SrcRect [%d %d] rawbuffer [%d %d] surfaceBuffer [%d %d], z:%f, globalZOrder:%d, blendType = %d",
        node.GetId(), node.GetName().c_str(), info.dstRect.x, info.dstRect.y, info.dstRect.w, info.dstRect.h,
        info.srcRect.w, info.srcRect.h, info.buffer->GetWidth(), info.buffer->GetHeight(),
        info.buffer->GetSurfaceBufferWidth(), info.buffer->GetSurfaceBufferHeight(),
        node.GetGlobalZOrder(), info.zOrder, info.blendType);
    LayerInfoPtr layer = HdiLayerInfo::CreateHdiLayerInfo();
    // planning surfaceNode prebuffer is set to hdilayerInfo, enable release prebuffer when HWC composition is ready
    SetComposeInfoToLayer(layer, info, node.GetConsumer(), &node);
    LayerRotate(layer, node);
    LayerCrop(layer);
    LayerScaleDown(layer);
    return layer;
}

LayerInfoPtr RSUniRenderComposerAdapter::CreateLayer(RSSurfaceRenderNode& node)
{
    auto& consumer = node.GetConsumer();
    if (consumer == nullptr) {
        RS_LOGE("RSUniRenderComposerAdapter::CreateLayer get consumer fail");
        return nullptr;
    }
    return CreateBufferLayer(node);
}

LayerInfoPtr RSUniRenderComposerAdapter::CreateLayer(RSDisplayRenderNode& node)
{
    RS_TRACE_NAME("RSUniRenderComposerAdapter::CreateLayer");
    if (output_ == nullptr) {
        RS_LOGE("RSUniRenderComposerAdapter::CreateLayer: output is nullptr");
        return nullptr;
    }

    RS_LOGD("RSUniRenderComposerAdapter::CreateLayer displayNode id:%" PRIu64 " available buffer:%d", node.GetId(),
        node.GetAvailableBufferCount());
    if (!RSBaseRenderUtil::ConsumeAndUpdateBuffer(node)) {
        RS_LOGE("RSUniRenderComposerAdapter::CreateLayer consume buffer failed.");
        return nullptr;
    }

    ComposeInfo info = BuildComposeInfo(node);
    RS_LOGD("RSUniRenderComposerAdapter::ProcessSurface displayNode id:%" PRIu64 " dst [%d %d %d %d]"
            "SrcRect [%d %d] rawbuffer [%d %d] surfaceBuffer [%d %d] buffaddr:%p, globalZOrder:%d, blendType = %d",
        node.GetId(), info.dstRect.x, info.dstRect.y, info.dstRect.w, info.dstRect.h, info.srcRect.w, info.srcRect.h,
        info.buffer->GetWidth(), info.buffer->GetHeight(), info.buffer->GetSurfaceBufferWidth(),
        info.buffer->GetSurfaceBufferHeight(), info.buffer.GetRefPtr(), info.zOrder, info.blendType);
    LayerInfoPtr layer = HdiLayerInfo::CreateHdiLayerInfo();
    SetComposeInfoToLayer(layer, info, node.GetConsumer(), &node);
    LayerRotate(layer, node);
    // do not crop or scale down for displayNode's layer.
    return layer;
}

LayerInfoPtr RSUniRenderComposerAdapter::CreateLayer(RSDrivenSurfaceRenderNode& node)
{
#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
    if (output_ == nullptr) {
        RS_LOGE("RSUniRenderComposerAdapter::CreateLayer: output is nullptr");
        return nullptr;
    }

    RS_LOGD("RSUniRenderComposerAdapter::CreateLayer RSDrivenSurfaceRenderNode id:%" PRIu64 " available buffer:%d",
        node.GetId(), node.GetAvailableBufferCount());
    if (!RSBaseRenderUtil::ConsumeAndUpdateBuffer(node)) {
        RS_LOGE("RSUniRenderComposerAdapter::CreateLayer consume buffer failed.");
        return nullptr;
    }

    if (node.GetBuffer() == nullptr) {
        RS_LOGE("RSUniRenderComposerAdapter::CreateLayer buffer is nullptr!");
        return nullptr;
    }

    ComposeInfo info = BuildComposeInfo(node);
    RS_LOGD("RSUniRenderComposerAdapter::ProcessDrivenSurface drivenSurfaceNode id:%" PRIu64 " DstRect [%d %d %d %d]"
            "SrcRect [%d %d %d %d] rawbuffer [%d %d] surfaceBuffer [%d %d], z-Order:%d, blendType = %d",
        node.GetId(), info.dstRect.x, info.dstRect.y, info.dstRect.w, info.dstRect.h,
        info.srcRect.x, info.srcRect.y, info.srcRect.w, info.srcRect.h,
        info.buffer->GetWidth(), info.buffer->GetHeight(), info.buffer->GetSurfaceBufferWidth(),
        info.buffer->GetSurfaceBufferHeight(), info.zOrder, info.blendType);
    LayerInfoPtr layer = HdiLayerInfo::CreateHdiLayerInfo();
    SetComposeInfoToLayer(layer, info, node.GetConsumer(), &node);
    LayerRotate(layer, node);
    LayerCrop(layer);
    LayerScaleDown(layer);
    return layer;
#else
    return nullptr;
#endif
}

static int GetSurfaceNodeRotation(RSBaseRenderNode& node)
{
    // only surface render node has the ability to rotate
    // the rotation of display render node is calculated as screen rotation
    if (node.GetType() != RSRenderNodeType::SURFACE_NODE) {
        return 0;
    }

    auto& surfaceNode = static_cast<RSSurfaceRenderNode&>(node);
    return RSUniRenderUtil::GetRotationFromMatrix(surfaceNode.GetTotalMatrix());
}

static void SetLayerTransform(const LayerInfoPtr& layer, RSBaseRenderNode& node,
    const sptr<IConsumerSurface>& surface, ScreenRotation screenRotation)
{
    // screenRotation: anti-clockwise, surfaceNodeRotation: anti-clockwise, surfaceTransform: anti-clockwise
    // layerTransform: clockwise
    int surfaceNodeRotation = GetSurfaceNodeRotation(node);
    int totalRotation = (RotateEnumToInt(screenRotation) + surfaceNodeRotation +
        RSBaseRenderUtil::RotateEnumToInt(RSBaseRenderUtil::GetRotateTransform(surface->GetTransform()))) % 360;
    GraphicTransformType rotateEnum = RSBaseRenderUtil::RotateEnumToInt(totalRotation,
        RSBaseRenderUtil::GetFlipTransform(surface->GetTransform()));
    layer->SetTransform(rotateEnum);
}

static void SetLayerSize(const LayerInfoPtr& layer, const ScreenInfo& screenInfo)
{
    const auto screenWidth = static_cast<int32_t>(screenInfo.width);
    const auto screenHeight = static_cast<int32_t>(screenInfo.height);
    const auto screenRotation = screenInfo.rotation;
    const auto rect = layer->GetLayerSize();
    // screenRotation: anti-clockwise, surfaceTransform: anti-clockwise, layerTransform: clockwise
    switch (screenRotation) {
        case ScreenRotation::ROTATION_90: {
            RS_LOGD("RsDebug ScreenRotation 90, Before Rotate layer size [%d %d %d %d]",
                rect.x, rect.y, rect.w, rect.h);
            layer->SetLayerSize(GraphicIRect {rect.y, screenHeight - rect.x - rect.w, rect.h, rect.w});
            RS_LOGD("RsDebug ScreenRotation 90, After Rotate layer size [%d %d %d %d]",
                layer->GetLayerSize().x, layer->GetLayerSize().y, layer->GetLayerSize().w, layer->GetLayerSize().h);
            break;
        }
        case ScreenRotation::ROTATION_180: {
            RS_LOGD("RsDebug ScreenRotation 180, Before Rotate layer size [%d %d %d %d]",
                rect.x, rect.y, rect.w, rect.h);
            layer->SetLayerSize(
                GraphicIRect {screenWidth - rect.x - rect.w, screenHeight - rect.y - rect.h, rect.w, rect.h});
            RS_LOGD("RsDebug ScreenRotation 180, After Rotate layer size [%d %d %d %d]",
                layer->GetLayerSize().x, layer->GetLayerSize().y, layer->GetLayerSize().w, layer->GetLayerSize().h);
            break;
        }
        case ScreenRotation::ROTATION_270: {
            RS_LOGD("RsDebug ScreenRotation 270, Before Rotate layer size [%d %d %d %d]",
                rect.x, rect.y, rect.w, rect.h);
            layer->SetLayerSize(GraphicIRect {screenWidth - rect.y - rect.h, rect.x, rect.h, rect.w});
            RS_LOGD("RsDebug ScreenRotation 270, After Rotate layer size [%d %d %d %d]",
                layer->GetLayerSize().x, layer->GetLayerSize().y, layer->GetLayerSize().w, layer->GetLayerSize().h);
            break;
        }
        default:  {
            break;
        }
    }
}

// private func, guarantee the layer is valid
void RSUniRenderComposerAdapter::LayerRotate(const LayerInfoPtr& layer, RSBaseRenderNode& node) const
{
    auto surface = layer->GetSurface();
    if (surface == nullptr) {
        return;
    }
    SetLayerSize(layer, screenInfo_);
    SetLayerTransform(layer, node, surface, screenInfo_.rotation);
}

} // namespace Rosen
} // namespace OHOS
