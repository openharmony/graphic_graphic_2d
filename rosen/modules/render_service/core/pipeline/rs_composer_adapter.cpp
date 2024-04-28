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

#include "rs_composer_adapter.h"

#include <fstream>
#include <sstream>
#include <sys/time.h>

#include "common/rs_common_def.h"
#include "common/rs_obj_abs_geometry.h"
#include "platform/common/rs_log.h"
#include "rs_base_render_util.h"
#include "rs_divided_render_util.h"
#include "rs_trace.h"
#include "string_utils.h"

#include "draw/canvas.h"

namespace OHOS {
namespace Rosen {
bool RSComposerAdapter::Init(const ScreenInfo& screenInfo, int32_t offsetX, int32_t offsetY,
    float mirrorAdaptiveCoefficient, const FallbackCallback& cb)
{
    hdiBackend_ = HdiBackend::GetInstance();
    if (hdiBackend_ == nullptr) {
        RS_LOGE("RSComposerAdapter::Init: hdiBackend is nullptr");
        return false;
    }
    auto screenManager = CreateOrGetScreenManager();
    if (screenManager == nullptr) {
        RS_LOGE("RSComposerAdapter::Init: ScreenManager is nullptr");
        return false;
    }
    output_ = screenManager->GetOutput(ToScreenPhysicalId(screenInfo.id));
    if (output_ == nullptr) {
        RS_LOGE("RSComposerAdapter::Init: output_ is nullptr");
        return false;
    }

    fallbackCb_ = cb;
    auto onPrepareCompleteFunc = [this](auto& surface, const auto& param, void* data) {
        OnPrepareComplete(surface, param, data);
    };
    hdiBackend_->RegPrepareComplete(onPrepareCompleteFunc, this);

    offsetX_ = offsetX;
    offsetY_ = offsetY;
    mirrorAdaptiveCoefficient_ = mirrorAdaptiveCoefficient;
    screenInfo_ = screenInfo;

    GraphicIRect damageRect {0, 0, static_cast<int32_t>(screenInfo_.width), static_cast<int32_t>(screenInfo_.height)};
    std::vector<GraphicIRect> damageRects;
    damageRects.emplace_back(damageRect);
    output_->SetOutputDamages(damageRects);
    bool directClientCompEnableStatus = RSSystemProperties::GetDirectClientCompEnableStatus();
    output_->SetDirectClientCompEnableStatus(directClientCompEnableStatus);

#if (defined (RS_ENABLE_GL) || defined (RS_ENABLE_VK)) && (defined RS_ENABLE_EGLIMAGE)
    // enable direct GPU composition.
    output_->SetLayerCompCapacity(LAYER_COMPOSITION_CAPACITY);
#else
    output_->SetLayerCompCapacity(LAYER_COMPOSITION_CAPACITY_INVALID);
#endif

    return true;
}

void RSComposerAdapter::CommitLayers(const std::vector<LayerInfoPtr>& layers)
{
    if (hdiBackend_ == nullptr) {
        RS_LOGE("RSComposerAdapter::CommitLayers: backend is nullptr");
        return;
    }

    if (output_ == nullptr) {
        RS_LOGE("RSComposerAdapter::CommitLayers: output is nullptr");
        return;
    }

    DumpLayersToFile(layers);

    // do composition.
    output_->SetLayerInfo(layers);
    hdiBackend_->Repaint(output_);

    // get present timestamp from and set present timestamp to surface
    for (const auto& layer : layers) {
        if (layer == nullptr || layer->GetSurface() == nullptr) {
            RS_LOGW("RSComposerAdapter::CommitLayers: layer or layer's cSurface is nullptr");
            continue;
        }
        LayerPresentTimestamp(layer, layer->GetSurface());
    }

    // set all layers' releaseFence.
    const auto layersReleaseFence = output_->GetLayersReleaseFence();
    for (const auto& [layer, fence] : layersReleaseFence) {
        if (layer == nullptr) {
            continue;
        }

        auto nodePtr = static_cast<RSBaseRenderNode*>(layer->GetLayerAdditionalInfo());
        if (nodePtr == nullptr) {
            RS_LOGW("RSComposerAdapter::PostProcess: layer's node is nullptr.");
            continue;
        }

        RSSurfaceHandler* surfaceHandler = nullptr;
        if (nodePtr->IsInstanceOf<RSSurfaceRenderNode>()) {
            auto surfaceNode = static_cast<RSSurfaceRenderNode*>(nodePtr);
            surfaceHandler = static_cast<RSSurfaceHandler*>(surfaceNode);
        } else if (nodePtr->IsInstanceOf<RSDisplayRenderNode>()) {
            auto displayNode = static_cast<RSDisplayRenderNode*>(nodePtr);
            surfaceHandler = static_cast<RSSurfaceHandler*>(displayNode);
        }
        if (surfaceHandler == nullptr) {
            continue;
        }
        surfaceHandler->SetReleaseFence(fence);
    }
}

void RSComposerAdapter::DumpLayersToFile(const std::vector<LayerInfoPtr>& layers)
{
    if (!RSSystemProperties::GetDumpLayersEnabled()) {
        return;
    }

    for (auto &layerInfo : layers) {
        if (layerInfo == nullptr || layerInfo->GetSurface() == nullptr) {
            continue;
        }
        auto buffer = layerInfo->GetBuffer();
        if (buffer == nullptr) {
            RS_LOGW("RSComposerAdapter::DumpLayersToFile: Buffer is null");
            continue;
        }
        struct timeval now;
        gettimeofday(&now, nullptr);
        constexpr int secToUsec = 1000 * 1000;
        int64_t nowVal = static_cast<int64_t>(now.tv_sec) * secToUsec + static_cast<int64_t>(now.tv_usec);

        std::stringstream ss;
        ss << "/data/layer_" << layerInfo->GetSurface()->GetName()  << "_" << nowVal << "_" << buffer->GetWidth()
            << "x" << buffer->GetHeight() << ".raw";

        std::ofstream rawDataFile(ss.str(), std::ofstream::binary);
        if (!rawDataFile.good()) {
            RS_LOGW("RSComposerAdapter::DumpLayersToFile: Open failed!");
            continue;
        }
        rawDataFile.write(static_cast<const char *>(buffer->GetVirAddr()), buffer->GetSize());
        rawDataFile.close();
    }
}

// private func
bool RSComposerAdapter::IsOutOfScreenRegion(const ComposeInfo& info) const
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

void RSComposerAdapter::DealWithNodeGravity(const RSSurfaceRenderNode& node, ComposeInfo& info) const
{
    const auto& property = node.GetRenderProperties();
    const auto frameWidth = info.buffer->GetSurfaceBufferWidth();
    const auto frameHeight = info.buffer->GetSurfaceBufferHeight();
    const int boundsWidth = static_cast<int>(property.GetBoundsWidth());
    const int boundsHeight = static_cast<int>(property.GetBoundsHeight());
    const Gravity frameGravity = property.GetFrameGravity();
    // we do not need to do additional works for Gravity::RESIZE and if frameSize == boundsSize.
    if (frameGravity == Gravity::RESIZE || (frameWidth == boundsWidth && frameHeight == boundsHeight)) {
        return;
    }
    RS_TRACE_NAME_FMT("%s DealWithNodeGravity %d", node.GetName().c_str(), static_cast<int>(frameGravity));
    // get current node's translate matrix and calculate gravity matrix.
    Drawing::Matrix translateMatrix;
    translateMatrix.Translate(
        std::ceil(node.GetTotalMatrix().Get(Drawing::Matrix::Index::TRANS_X)),
        std::ceil(node.GetTotalMatrix().Get(Drawing::Matrix::Index::TRANS_Y)));
    Drawing::Matrix gravityMatrix;
    (void)RSPropertiesPainter::GetGravityMatrix(frameGravity,
        RectF {0.0f, 0.0f, boundsWidth, boundsHeight}, frameWidth, frameHeight, gravityMatrix);

    // create a canvas to calculate new dstRect and new srcRect
    int32_t screenWidth = screenInfo_.width;
    int32_t screenHeight = screenInfo_.height;
    const auto screenRotation = screenInfo_.rotation;
    if (screenRotation == ScreenRotation::ROTATION_90 || screenRotation == ScreenRotation::ROTATION_270) {
        std::swap(screenWidth, screenHeight);
    }
    auto canvas = std::make_unique<Rosen::Drawing::Canvas>(screenWidth, screenHeight);
    canvas->ConcatMatrix(translateMatrix);
    Drawing::Rect clipRect;
    Drawing::Rect srcRect(0, 0, frameWidth, frameHeight);
    gravityMatrix.MapRect(clipRect, srcRect);
    canvas->ClipRect(clipRect, Drawing::ClipOp::INTERSECT, false);
    canvas->ConcatMatrix(gravityMatrix);
    Drawing::RectI newDstRect = canvas->GetDeviceClipBounds();
    // we make the newDstRect as the intersection of new and old dstRect when frameSize > boundsSize.
    newDstRect.Intersect(Drawing::RectI(info.dstRect.x, info.dstRect.y,
        info.dstRect.w + info.dstRect.x, info.dstRect.h + info.dstRect.y));
    auto localRect = canvas->GetLocalClipBounds();
    int left = std::clamp<int>(localRect.GetLeft(), 0, frameWidth);
    int top = std::clamp<int>(localRect.GetTop(), 0, frameHeight);
    int width = std::clamp<int>(localRect.GetWidth(), 0, frameWidth - left);
    int height = std::clamp<int>(localRect.GetHeight(), 0, frameHeight - top);
    GraphicIRect newSrcRect = {left, top, width, height};

    RS_LOGD("RsDebug DealWithNodeGravity: name[%{public}s], gravity[%{public}d], oldDstRect[%{public}d"
        " %{public}d %{public}d %{public}d], newDstRect[%{public}d %{public}d %{public}d %{public}d],"\
        " oldSrcRect[%{public}d %{public}d %{public}d %{public}d], newSrcRect[%{public}d %{public}d"
        " %{public}d %{public}d].", node.GetName().c_str(), static_cast<int>(frameGravity),
        info.dstRect.x, info.dstRect.y, info.dstRect.w, info.dstRect.h,
        newDstRect.GetLeft(), newDstRect.GetTop(), newDstRect.GetWidth(), newDstRect.GetHeight(),
        info.srcRect.x, info.srcRect.y, info.srcRect.w, info.srcRect.h,
        newSrcRect.x, newSrcRect.y, newSrcRect.w, newSrcRect.h);
    info.dstRect = {newDstRect.GetLeft(), newDstRect.GetTop(), newDstRect.GetWidth(), newDstRect.GetHeight()};
    info.srcRect = newSrcRect;
}

void RSComposerAdapter::GetComposerInfoSrcRect(ComposeInfo &info, const RSSurfaceRenderNode& node)
{
    const auto& property = node.GetRenderProperties();
    const auto bufferWidth = info.buffer->GetSurfaceBufferWidth();
    const auto bufferHeight = info.buffer->GetSurfaceBufferHeight();
    const int boundsWidth = static_cast<int>(property.GetBoundsWidth());
    const int boundsHeight = static_cast<int>(property.GetBoundsHeight());
    if (bufferWidth != boundsWidth || bufferHeight != boundsHeight) {
        // float will cause loss of accuracy, and the result will 1 pixel less
        double xScale = (ROSEN_EQ(boundsWidth, 0) ? 1.0 : 1.0 * bufferWidth / boundsWidth);
        double yScale = (ROSEN_EQ(boundsHeight, 0) ? 1.0 : 1.0 * bufferHeight / boundsHeight);
        info.srcRect.x = info.srcRect.x * xScale;
        info.srcRect.y = info.srcRect.y * yScale;
        info.srcRect.w = std::min(static_cast<int32_t>(info.srcRect.w * xScale), bufferWidth);
        info.srcRect.h = std::min(static_cast<int32_t>(info.srcRect.h * yScale), bufferHeight);
    }
}

bool RSComposerAdapter::GetComposerInfoNeedClient(const ComposeInfo &info, RSSurfaceRenderNode& node) const
{
    bool needClient = RSBaseRenderUtil::IsNeedClient(node, info);
    if (info.buffer->GetSurfaceBufferColorGamut() != static_cast<GraphicColorGamut>(screenInfo_.colorGamut)) {
        needClient = true;
    }
    return needClient;
}

// private func, for RSSurfaceRenderNode.
ComposeInfo RSComposerAdapter::BuildComposeInfo(RSSurfaceRenderNode& node, bool isTunnelCheck) const
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

    info.dstRect.x -= static_cast<int32_t>(static_cast<float>(offsetX_) * mirrorAdaptiveCoefficient_);
    info.dstRect.y -= static_cast<int32_t>(static_cast<float>(offsetY_) * mirrorAdaptiveCoefficient_);
    info.visibleRect = info.dstRect;
    if (!isTunnelCheck) {
        const auto& buffer = node.GetBuffer();
        info.buffer = buffer;
        GetComposerInfoSrcRect(info, node);
        info.needClient = GetComposerInfoNeedClient(info, node);
        DealWithNodeGravity(node, info);
    } else {
        info.needClient = false;
    }
    return info;
}

// private func, for RSDisplayRenderNode
ComposeInfo RSComposerAdapter::BuildComposeInfo(RSDisplayRenderNode& node) const
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
    info.visibleRect = GraphicIRect {info.dstRect.x, info.dstRect.y, info.dstRect.w, info.dstRect.h};
    info.zOrder = static_cast<int32_t>(node.GetGlobalZOrder());
    info.alpha.enGlobalAlpha = false;
    info.buffer = buffer;
    info.fence = node.GetAcquireFence();
    info.blendType = GRAPHIC_BLEND_NONE;
    info.needClient = false;
    return info;
}

void RSComposerAdapter::SetComposeInfoToLayer(
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
    layer->SetZorder(info.zOrder);
    layer->SetAlpha(info.alpha);
    layer->SetLayerSize(info.dstRect);
    layer->SetLayerAdditionalInfo(node);
    layer->SetCompositionType(info.needClient ?
        GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT : GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    std::vector<GraphicIRect> visibleRegions;
    visibleRegions.emplace_back(info.visibleRect);
    layer->SetVisibleRegions(visibleRegions);
    std::vector<GraphicIRect> dirtyRegions;
    dirtyRegions.emplace_back(info.srcRect);
    layer->SetDirtyRegions(dirtyRegions);
    layer->SetBlendType(info.blendType);
    layer->SetCropRect(info.srcRect);
    if (node -> GetTunnelHandleChange()) {
        layer->SetTunnelHandleChange(true);
        layer->SetTunnelHandle(surface->GetTunnelHandle());
        node ->SetTunnelHandleChange(false);
    }
    if (surface->GetTunnelHandle() != nullptr) {
        return;
    }
    SetMetaDataInfoToLayer(layer, info, surface);
}

void RSComposerAdapter::SetMetaDataInfoToLayer(const LayerInfoPtr& layer, const ComposeInfo& info,
                                               const sptr<IConsumerSurface>& surface) const
{
    HDRMetaDataType type;
    if (surface->QueryMetaDataType(info.buffer->GetSeqNum(), type) != GSERROR_OK) {
        RS_LOGE("RSComposerAdapter::SetComposeInfoToLayer: QueryMetaDataType failed");
        return;
    }
    switch (type) {
        case HDRMetaDataType::HDR_META_DATA: {
            std::vector<GraphicHDRMetaData> metaData;
            if (surface->GetMetaData(info.buffer->GetSeqNum(), metaData) != GSERROR_OK) {
                RS_LOGE("RSComposerAdapter::SetComposeInfoToLayer: GetMetaData failed");
                return;
            }
            layer->SetMetaData(metaData);
            break;
        }
        case HDRMetaDataType::HDR_META_DATA_SET: {
            GraphicHDRMetadataKey key;
            std::vector<uint8_t> metaData;
            if (surface->GetMetaDataSet(info.buffer->GetSeqNum(), key, metaData) != GSERROR_OK) {
                RS_LOGE("RSComposerAdapter::SetComposeInfoToLayer: GetMetaDataSet failed");
                return;
            }
            GraphicHDRMetaDataSet metaDataSet;
            metaDataSet.key = key;
            metaDataSet.metaData = metaData;
            layer->SetMetaDataSet(metaDataSet);
            break;
        }
        case HDRMetaDataType::HDR_NOT_USED: {
            break;
        }
        default:  {
            break;
        }
    }
}

bool RSComposerAdapter::CheckStatusBeforeCreateLayer(RSSurfaceRenderNode& node, bool isTunnelCheck) const
{
    if (output_ == nullptr) {
        RS_LOGE("RSComposerAdapter::CheckStatusBeforeCreateLayer: output is nullptr");
        return false;
    }

    const auto& buffer = node.GetBuffer();
    if (isTunnelCheck == false && buffer == nullptr) {
        RS_LOGD("RsDebug RSComposerAdapter::CheckStatusBeforeCreateLayer:node(%{public}" PRIu64 ") has"
            " no available buffer.", node.GetId());
        return false;
    }

    const auto& dstRect = node.GetDstRect();
    const auto& srcRect = node.GetSrcRect();
    // check if the node's srcRect and dstRect are valid.
    if (srcRect.width_ <= 0 || srcRect.height_ <= 0 || dstRect.width_ <= 0 || dstRect.height_ <= 0) {
        return false;
    }

    auto& geoPtr = (node.GetRenderProperties().GetBoundsGeometry());
    if (geoPtr == nullptr) {
        RS_LOGW("RsDebug RSComposerAdapter::CheckStatusBeforeCreateLayer: node(%{public}" PRIu64 ")'s"
            " geoPtr is nullptr!", node.GetId());
        return false;
    }

    if (!node.IsNotifyRTBufferAvailable()) {
        // Only ipc for one time.
        RS_LOGD("RsDebug RSPhysicalScreenProcessor::ProcessSurface id = %{public}" PRIu64 " Notify RT buffer available",
            node.GetId());
        node.NotifyRTBufferAvailable();
    }
    return true;
}

LayerInfoPtr RSComposerAdapter::CreateBufferLayer(RSSurfaceRenderNode& node) const
{
    if (!CheckStatusBeforeCreateLayer(node)) {
        return nullptr;
    }
    ComposeInfo info = BuildComposeInfo(node);
    if (IsOutOfScreenRegion(info)) {
        RS_LOGD("RsDebug RSComposerAdapter::CreateBufferLayer: node(%{public}" PRIu64
                ") out of screen region, no need to composite.",
            node.GetId());
        return nullptr;
    }
    std::string traceInfo;
    AppendFormat(traceInfo, "ProcessSurfaceNode:%s XYWH[%d %d %d %d]", node.GetName().c_str(),
        info.dstRect.x, info.dstRect.y, info.dstRect.w, info.dstRect.h);
    RS_TRACE_NAME(traceInfo.c_str());
    RS_LOGD(
        "RsDebug RSComposerAdapter::CreateBufferLayer surfaceNode id:%{public}" PRIu64 " name:[%{public}s]"
        " dst [%{public}d %{public}d %{public}d %{public}d] SrcRect [%{public}d %{public}d] rawbuffer"
        " [%{public}d %{public}d] surfaceBuffer [%{public}d %{public}d], z:%{public}f, globalZOrder:%{public}d,"
        " blendType = %{public}d",
        node.GetId(), node.GetName().c_str(), info.dstRect.x, info.dstRect.y, info.dstRect.w, info.dstRect.h,
        info.srcRect.w, info.srcRect.h, info.buffer->GetWidth(), info.buffer->GetHeight(),
        info.buffer->GetSurfaceBufferWidth(), info.buffer->GetSurfaceBufferHeight(),
        node.GetGlobalZOrder(), info.zOrder, info.blendType);
    LayerInfoPtr layer = HdiLayerInfo::CreateHdiLayerInfo();
    SetComposeInfoToLayer(layer, info, node.GetConsumer(), &node);
    LayerRotate(layer, node);
    LayerCrop(layer);
    LayerScaleDown(layer);
    return layer;
}

LayerInfoPtr RSComposerAdapter::CreateTunnelLayer(RSSurfaceRenderNode& node) const
{
    if (!CheckStatusBeforeCreateLayer(node, true)) {
        return nullptr;
    }
    ComposeInfo info = BuildComposeInfo(node, true);
    if (IsOutOfScreenRegion(info)) {
        RS_LOGD("RsDebug RSComposerAdapter::CreateTunnelLayer: node(%{public}" PRIu64
                ") out of screen region, no need to composite.",
            node.GetId());
        return nullptr;
    }
    std::string traceInfo;
    AppendFormat(traceInfo, "ProcessSurfaceNode:%s XYWH[%d %d %d %d]", node.GetName().c_str(),
        info.dstRect.x, info.dstRect.y, info.dstRect.w, info.dstRect.h);
    RS_TRACE_NAME(traceInfo.c_str());
    LayerInfoPtr layer = HdiLayerInfo::CreateHdiLayerInfo();
    SetComposeInfoToLayer(layer, info, node.GetConsumer(), &node);
    LayerRotate(layer, node);
    RS_LOGD("RsDebug RSComposerAdapter::CreateTunnelLayer surfaceNode id:%{public}" PRIu64 " name:[%{public}s] dst"
        " [%{public}d %{public}d %{public}d %{public}d]"
        "SrcRect [%{public}d %{public}d], z:%{public}f, globalZOrder:%{public}d, blendType = %{public}d",
        node.GetId(), node.GetName().c_str(), info.dstRect.x, info.dstRect.y, info.dstRect.w, info.dstRect.h,
        info.srcRect.w, info.srcRect.h, node.GetGlobalZOrder(), info.zOrder, info.blendType);
    return layer;
}

LayerInfoPtr RSComposerAdapter::CreateLayer(RSSurfaceRenderNode& node) const
{
    auto& consumer = node.GetConsumer();
    if (consumer == nullptr) {
        RS_LOGE("RSComposerAdapter::CreateLayer get consumer fail");
        return nullptr;
    }
    sptr<SurfaceTunnelHandle> handle = consumer->GetTunnelHandle();
    if (handle != nullptr) {
        return CreateTunnelLayer(node);
    } else {
        return CreateBufferLayer(node);
    }
}

LayerInfoPtr RSComposerAdapter::CreateLayer(RSDisplayRenderNode& node) const
{
    if (output_ == nullptr) {
        RS_LOGE("RSComposerAdapter::CreateLayer: output is nullptr");
        return nullptr;
    }
    if (!RSBaseRenderUtil::ConsumeAndUpdateBuffer(node, true)) {
        RS_LOGE("RSComposerAdapter::CreateLayer consume buffer failed.");
        return nullptr;
    }

    if (node.GetBuffer() == nullptr) {
        RS_LOGE("RSComposerAdapter::CreateLayer buffer is nullptr.");
        return nullptr;
    }

    ComposeInfo info = BuildComposeInfo(node);
    RS_LOGD("RSComposerAdapter::ProcessSurface displayNode id:%{public}" PRIu64 " dst [%{public}d"
        " %{public}d %{public}d %{public}d] SrcRect [%{public}d %{public}d] rawbuffer [%{public}d %{public}d]"
        " surfaceBuffer [%{public}d %{public}d], globalZOrder:%{public}d, blendType = %{public}d",
        node.GetId(), info.dstRect.x, info.dstRect.y, info.dstRect.w, info.dstRect.h, info.srcRect.w, info.srcRect.h,
        info.buffer->GetWidth(), info.buffer->GetHeight(), info.buffer->GetSurfaceBufferWidth(),
        info.buffer->GetSurfaceBufferHeight(), info.zOrder, info.blendType);
    LayerInfoPtr layer = HdiLayerInfo::CreateHdiLayerInfo();
    SetComposeInfoToLayer(layer, info, node.GetConsumer(), &node);
    LayerRotate(layer, node);
    // do not crop or scale down for displayNode's layer.
    return layer;
}

static int GetSurfaceNodeRotation(RSBaseRenderNode& node)
{
    // only surface render node has the ability to rotate
    // the rotation of display render node is calculated as screen rotation
    if (node.GetType() != RSRenderNodeType::SURFACE_NODE) {
        return 0;
    }

    auto& surfaceNode = static_cast<RSSurfaceRenderNode&>(node);
    auto matrix = surfaceNode.GetTotalMatrix();
    Drawing::Matrix::Buffer value;
    matrix.GetAll(value);

    int rAngle = static_cast<int>(
        -round(atan2(value[Drawing::Matrix::SKEW_X], value[Drawing::Matrix::SCALE_X]) * (180 / PI)));
    // transfer the result to anti-clockwise degrees
    // only rotation with 90°, 180°, 270° are composed through hardware,
    // in which situation the transformation of the layer needs to be set.
    static const std::map<int, int> supportedDegrees = {{90, 270}, {180, 180}, {-90, 90}};
    auto iter = supportedDegrees.find(rAngle);
    return iter != supportedDegrees.end() ? iter->second : 0;
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
            RS_LOGD("RsDebug ScreenRotation 90, Before Rotate layer size [%{public}d %{public}d"
                " %{public}d %{public}d]", rect.x, rect.y, rect.w, rect.h);
            layer->SetLayerSize(GraphicIRect {rect.y, screenHeight - rect.x - rect.w, rect.h, rect.w});
            RS_LOGD("RsDebug ScreenRotation 90, After Rotate layer size [%{public}d %{public}d %{public}d %{public}d]",
                layer->GetLayerSize().x, layer->GetLayerSize().y, layer->GetLayerSize().w, layer->GetLayerSize().h);
            break;
        }
        case ScreenRotation::ROTATION_180: {
            RS_LOGD("RsDebug ScreenRotation 180, Before Rotate layer size [%{public}d %{public}d %{public}d"
                " %{public}d]", rect.x, rect.y, rect.w, rect.h);
            layer->SetLayerSize(
                GraphicIRect {screenWidth - rect.x - rect.w, screenHeight - rect.y - rect.h, rect.w, rect.h});
            RS_LOGD("RsDebug ScreenRotation 180,After Rotate layer size [%{public}d %{public}d %{public}d %{public}d]",
                layer->GetLayerSize().x, layer->GetLayerSize().y, layer->GetLayerSize().w, layer->GetLayerSize().h);
            break;
        }
        case ScreenRotation::ROTATION_270: {
            RS_LOGD("RsDebug ScreenRotation 270, Before Rotate layer size [%{public}d %{public}d %{public}d"
                " %{public}d]", rect.x, rect.y, rect.w, rect.h);
            layer->SetLayerSize(GraphicIRect {screenWidth - rect.y - rect.h, rect.x, rect.h, rect.w});
            RS_LOGD("RsDebug ScreenRotation 270,After Rotate layer size [%{public}d %{public}d %{public}d %{public}d]",
                layer->GetLayerSize().x, layer->GetLayerSize().y, layer->GetLayerSize().w, layer->GetLayerSize().h);
            break;
        }
        default:  {
            break;
        }
    }
}

// private func, guarantee the layer is valid
void RSComposerAdapter::LayerRotate(const LayerInfoPtr& layer, RSBaseRenderNode& node) const
{
    auto surface = layer->GetSurface();
    if (surface == nullptr) {
        return;
    }
    SetLayerSize(layer, screenInfo_);
    SetLayerTransform(layer, node, surface, screenInfo_.rotation);
}

// private func, guarantee the layer is valid
void RSComposerAdapter::LayerCrop(const LayerInfoPtr& layer) const
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
    std::vector<GraphicIRect> dirtyRegions;
    dirtyRegions.emplace_back(srcRect);
    layer->SetDirtyRegions(dirtyRegions);
    layer->SetCropRect(srcRect);
    RS_LOGD("RsDebug RSComposerAdapter::LayerCrop layer has been cropped dst[%{public}d %{public}d %{public}d"
        " %{public}d] src[%{public}d %{public}d %{public}d %{public}d]",
        dstRect.x, dstRect.y, dstRect.w, dstRect.h, srcRect.x, srcRect.y, srcRect.w, srcRect.h);
}

// private func, guarantee the layer is valid
void RSComposerAdapter::LayerScaleDown(const LayerInfoPtr& layer)
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

        if (newWidth * dstRect.h > newHeight * dstRect.w) {
            // too wide
            if (dstRect.h == 0) {
                return;
            }
            newWidth = dstRect.w * newHeight / dstRect.h;
        } else if (newWidth * dstRect.h < newHeight * dstRect.w) {
            // too tall
            if (dstRect.w == 0) {
                return;
            }
            newHeight = dstRect.h * newWidth / dstRect.w;
        } else {
            return;
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
        std::vector<GraphicIRect> dirtyRegions;
        dirtyRegions.emplace_back(srcRect);
        layer->SetDirtyRegions(dirtyRegions);
        layer->SetCropRect(srcRect);
        RS_LOGD("RsDebug RSComposerAdapter::LayerScaleDown layer has been scaledown dst[%{public}d %{public}d"
            " %{public}d %{public}d] src[%{public}d %{public}d %{public}d %{public}d]",
            dstRect.x, dstRect.y, dstRect.w, dstRect.h, srcRect.x, srcRect.y, srcRect.w, srcRect.h);
    }
}

// private func, guarantee the layer and surface are valid
void RSComposerAdapter::LayerPresentTimestamp(const LayerInfoPtr& layer, const sptr<IConsumerSurface>& surface)
{
    if (!layer->IsSupportedPresentTimestamp()) {
        return;
    }
    const auto& buffer = layer->GetBuffer();
    if (buffer == nullptr) {
        return;
    }
    if (surface->SetPresentTimestamp(buffer->GetSeqNum(), layer->GetPresentTimestamp()) != GSERROR_OK) {
        RS_LOGD("RsDebug RSComposerAdapter::LayerPresentTimestamp: SetPresentTimestamp failed");
    }
}

void RSComposerAdapter::OnPrepareComplete(sptr<Surface>& surface, const PrepareCompleteParam& param, void* data)
{
    // unused data.
    (void)(data);

    if (!param.needFlushFramebuffer) {
        return;
    }

    if (fallbackCb_ != nullptr) {
        fallbackCb_(surface, param.layers);
    }
}

void RSComposerAdapter::SetHdiBackendDevice(HdiDevice* device)
{
    hdiBackend_->SetHdiBackendDevice(device);
}
} // namespace Rosen
} // namespace OHOS
