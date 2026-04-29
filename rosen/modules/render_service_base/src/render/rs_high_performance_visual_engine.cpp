/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "render/rs_high_performance_visual_engine.h"

#include "common/rs_color_palette.h"
#include "common/rs_optional_trace.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_effect_render_node.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
constexpr int MAX_FILTER_SIZE = 500;

HveFilter& HveFilter::GetHveFilter()
{
    static HveFilter filter;
    return filter;
}

void HveFilter::Sync()
{
    ClearSurfaceNodeInfo();
    hveFilterToSurfaceNodeMap_ = hveFilterToSurfaceNodeStagingMap_;
    hveFilterToSurfaceNodeStagingMap_.clear();
}

void HveFilter::ClearSurfaceNodeInfo()
{
    surfaceNodeInfo_.clear();
}

void HveFilter::PushSurfaceNodeInfo(SurfaceNodeInfo& surfaceNodeInfo)
{
    std::lock_guard<std::mutex> lock(hveFilterMtx_);
    surfaceNodeInfo_.push_back(surfaceNodeInfo);
}

std::vector<SurfaceNodeInfo> HveFilter::GetSurfaceNodeInfo() const
{
    return surfaceNodeInfo_;
}

int HveFilter::GetSurfaceNodeSize() const
{
    std::lock_guard<std::mutex> lock(hveFilterMtx_);
    return surfaceNodeInfo_.size();
}

bool HveFilter::HasFilterNode(NodeId filterId)
{
    return hveFilterToSurfaceNodeMap_.find(filterId) != hveFilterToSurfaceNodeMap_.end();
}

bool HveFilter::HasValidEffectNode(const std::shared_ptr<RSRenderNode>& node)
{
    auto current = node;
    while (current) {
        if (current->GetType() == RSRenderNodeType::SURFACE_NODE) {
            return false;
        }
        if (current->GetType() == RSRenderNodeType::EFFECT_NODE) {
            auto effectNode = RSRenderNode::ReinterpretCast<RSEffectRenderNode>(current);
            return effectNode->ChildHasVisibleEffectWithoutEmptyRect() &&
                   current->GetRenderProperties().GetBackgroundFilter() != nullptr &&
                   current->ShouldPaint() && current->GetGlobalAlpha() == 1;
        }
        current = current->GetParent().lock();
    }
    return false;
}

bool HveFilter::HasValidEffect(const RSRenderNode* node)
{
    // Basic termination condition
    if (!node || node->GetType() == RSRenderNodeType::SURFACE_NODE) {
        return false;
    }
    const RSProperties& properties = node->GetRenderProperties();
    if (properties.GetUseEffect() &&
        node->ShouldPaint() && node->GetGlobalAlpha() == 1 && !node->HasChildrenOutOfRect()) {
        // After finding target node, start searching upwards for EFFECT_NODE
        RS_LOGD("%{public}s UseEffect is valid", __func__);
        return HasValidEffectNode(node->GetParent().lock());
    }
    return HasValidEffect(node->GetParent().lock().get());
}

bool HveFilter::CheckPrecondition(const RSRenderNode& renderNode,
    const RectI& filterRect, RSSurfaceRenderNode& hwcNode)
{
    // Check basic conditions for hwcNode and filter size
    if (!hwcNode.GetArsrTag() ||
        (filterRect.GetWidth() > MAX_FILTER_SIZE && filterRect.GetHeight() > MAX_FILTER_SIZE)) {
        return false;
    }
    const RSProperties& properties = renderNode.GetRenderProperties();
    if (properties.GetBackgroundFilter() != nullptr ||
        properties.GetMaterialFilter() != nullptr || properties.GetUseEffect()) {
        RS_LOGD("%{public}s within filter range", __func__);
        return true;
    }
    // If there is BgBrightness, ensure the Effect is valid.
    return properties.IsBgBrightnessValid() && HasValidEffect(renderNode.GetParent().lock().get());
}

void HveFilter::ClearHveFilterSurfaceNodeMapping()
{
    hveFilterToSurfaceNodeMap_.clear();
}

void HveFilter::PushHveFilterSurfaceNodeMapping(NodeId filterId, NodeId surfaceId)
{
    if (hveFilterToSurfaceNodeStagingMap_.find(filterId) == hveFilterToSurfaceNodeStagingMap_.end()) {
        hveFilterToSurfaceNodeStagingMap_[filterId] = std::vector<NodeId>();
    }
    hveFilterToSurfaceNodeStagingMap_[filterId].push_back(surfaceId);
}

void HveFilter::DrawSurfaceImage(std::shared_ptr<RSPaintFilterCanvas>& canvas,
    SurfaceNodeInfo& surfaceNodeInfo, const Drawing::RectI& srcRect)
{
    auto surfaceImage = surfaceNodeInfo.surfaceImage_;
    Drawing::Matrix rotateMatrix = surfaceNodeInfo.matrix_;
    Drawing::Rect parmSrcRect = surfaceNodeInfo.srcRect_;
    Drawing::Rect parmDstRect = surfaceNodeInfo.dstRect_;
    // Get the color of solidlayer
    Color solidLayerColor = surfaceNodeInfo.solidLayerColor_;
    // A valid solidlayer color exists.
    if (solidLayerColor != RgbPalette::Transparent()) {
        canvas->Clear(static_cast<Drawing::ColorQuad>(solidLayerColor.AsArgbInt()));
    }
    canvas->Save();
    canvas->Translate(-srcRect.GetLeft(), -srcRect.GetTop());
    canvas->ConcatMatrix(rotateMatrix);
    canvas->DrawImageRect(*surfaceImage, parmSrcRect, parmDstRect, Drawing::SamplingOptions(),
        Drawing::SrcRectConstraint::FAST_SRC_RECT_CONSTRAINT);
    canvas->Restore();
}
    
std::shared_ptr<Drawing::Image> HveFilter::SampleLayer(
    RSPaintFilterCanvas& canvas, const Drawing::RectI& srcRect, NodeId filterId)
{
    std::lock_guard<std::mutex> lock(hveFilterMtx_);
    auto drawingSurface = canvas.GetSurface();
    if (drawingSurface == nullptr) {
        ClearSurfaceNodeInfo();
        return nullptr;
    }
    int widthUI = srcRect.GetWidth();
    int heightUI = srcRect.GetHeight();
    auto offscreenSurface = drawingSurface->MakeSurface(widthUI, heightUI);
    if (offscreenSurface == nullptr) {
        ClearSurfaceNodeInfo();
        return nullptr;
    }
    auto offscreenCanvas = std::make_shared<RSPaintFilterCanvas>(offscreenSurface.get());
    if (offscreenCanvas == nullptr) {
        ClearSurfaceNodeInfo();
        return nullptr;
    }

    std::shared_ptr<Drawing::Image> snapshot;
    std::vector<SurfaceNodeInfo> vecSurfaceNode = GetSurfaceNodeInfo();
    size_t surfaceNodeSize = vecSurfaceNode.size();
    RS_TRACE_NAME_FMT("surfaceNodeSize:%d", surfaceNodeSize);
    Drawing::RectI dstRect = Drawing::RectI(0, 0, widthUI, heightUI);
    auto it = hveFilterToSurfaceNodeMap_.find(filterId);
    std::vector<NodeId> surfaceNodeIds = (it != hveFilterToSurfaceNodeMap_.end()) ? it->second : std::vector<NodeId>{};

    for (size_t i = 0; i < surfaceNodeSize; i++) {
        auto surfaceImage = vecSurfaceNode[i].surfaceImage_;
        if (surfaceImage == nullptr) {
            continue;
        }
        NodeId surfaceId = vecSurfaceNode[i].surfaceNodeId_;
        if (std::find(surfaceNodeIds.begin(), surfaceNodeIds.end(), surfaceId) == surfaceNodeIds.end()) {
            continue;
        }
        DrawSurfaceImage(offscreenCanvas, vecSurfaceNode[i], srcRect);
    }

    auto inputImage = drawingSurface->GetImageSnapshot(srcRect);
    if (inputImage != nullptr) {
        offscreenCanvas->DrawImageRect(*inputImage, dstRect, dstRect, Drawing::SamplingOptions(),
            Drawing::SrcRectConstraint::FAST_SRC_RECT_CONSTRAINT);
    }

    snapshot = offscreenSurface->GetImageSnapshot();
    return snapshot;
}

} // namespace Rosen
} // namespace OHOS