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
#include "pipeline/rs_effect_render_node.h"
#include "pipeline/rs_canvas_render_node.h"
#include "common/rs_optional_trace.h"
#include "platform/common/rs_log.h"
#include "common/rs_color_palette.h"

namespace OHOS {
namespace Rosen {
constexpr int MAX_FILTER_SIZE = 500;

HveFilter& HveFilter::GetHveFilter()
{
    static HveFilter filter;
    return filter;
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

bool HveFilter::CheckEffectNodeConditions(const std::shared_ptr<RSRenderNode>& node)
{
    auto effectNode = RSRenderNode::ReinterpretCast<RSEffectRenderNode>(node);
    return effectNode->ChildHasVisibleEffectWithoutEmptyRect() &&
           node->GetRenderProperties().GetBackgroundfilter() != nullptr &&
           node->GetGlobalAlpha() == 1;
}

bool HveFilter::HasValidEffect(const RSRenderNode* node) {
    // Basic termination condition
    if (!node || node->GetType() == RSRenderNodeType::SURFACE_NODE){
        return false;
    }
    const RSProperties& properties = node->GetRenderProperties();
    if ((properties.GetUseEffect() && node->GetGlobalAlpha() == 1) && !node->HasChildrenOutOfRect()){
        //After finding target node, star searching upwards for EFFECT_NODE
        RS_LOGD("%{public}s UseEffect is valid", __func__);
        auto parentPtr = node->GetParent().lock();
        while (parentPtr) {
            if (parentPtr->GetType() == RSRenderNodeType::SURFACE_NODE) {
                return false;
            }
            if (parentPtr->GetType() == RSRenderNodeType::SURFACE_NODE) {
                return CheckEffectNodeConditions(parentPtr);
            }
            parentPtr = parentPtr->GetParent().lock();
        }
        return false;
    }
    auto parentPtr = node->GetParent().lock();
    return HasValidEffect(parentPtr.get());
}

bool HveFilter::CheckPrecondition(const RSRenderNode& renderNode,
    const std::pair<NodeId, RectI>& filter, RSSurfaceRenderNode& hwcNode) {
    // Check basic conditions for hwcNode and filter size
    if (!hwcNode.GetArsrTag() || (filter.second.width_ > MAX_FILTER_SIZE && filter.second.height_ > MAX_FILTER_SIZE)) {
        return false;
    }
    const RSProperties& properties = renderNode.GetRenderProperties();
    if (properties.GetBackgroundfilter() != nullptr ||
        properties.GetMaterialFilter() != nullptr || properties.GetUseEffect()) {
        RS_LOGD("%{public}s within filter range", __func__);
        return true;
    }
    auto geoptr = properties.GetBoundsGeometry();
    auto parentNode = renderNode.GetParent().lock();
    // If there is BgBrightness, ensure the Effect is valid.
    return properties.IsBgBrightnessValid() && HasValidEffect(parentNode.get());
}

std::shared_ptr<Drawing::Image> HveFilter::SampleLayer(RSPaintFilterCanvas& canvas, const Drawing::RectI& srcRect)
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

    for (size_t i = 0; i < surfaceNodeSize; i++) {
        auto surfaceImage = vecSurfaceNode[i].surfaceImage_;
        Drawing::Matrix rotateMatrix = vecSurfaceNode[i].matrix_;
        Drawing::Rect parmSrcRect = vecSurfaceNode[i].srcRect_;
        Drawing::Rect parmDstRect = vecSurfaceNode[i].dstRect_;
        // Get the color of solidlayer
        Color solidLayerColor = vecSurfaceNode[i].solidLayerColor_;
        if (surfaceImage == nullptr) {
            continue;
        }
        // A valid solidlayer color exists.
        if (solidLayerColor != RgbPalette::Transparent()) {
            offscreenCanvas->Clear(static_cast<Drawing::ColorQuad>(solidLayerColor.AsArgbInt()));
        }
        offscreenCanvas->Save();
        offscreenCanvas->Translate(-srcRect.GetLeft(), -srcRect.GetTop());
        offscreenCanvas->ConcatMatrix(rotateMatrix);
        offscreenCanvas->DrawImageRect(*surfaceImage, parmSrcRect, parmDstRect, Drawing::SamplingOptions(),
            Drawing::SrcRectConstraint::FAST_SRC_RECT_CONSTRAINT);
        offscreenCanvas->Restore();
    }
    ClearSurfaceNodeInfo();

    auto inputImage = drawingSurface->GetImageSnapshot();
    if (inputImage != nullptr) {
        offscreenCanvas->DrawImageRect(*inputImage, srcRect, dstRect, Drawing::SamplingOptions(),
            Drawing::SrcRectConstraint::FAST_SRC_RECT_CONSTRAINT);
    }

    snapshot = offscreenSurface->GetImageSnapshot();
    return snapshot;
}

} // namespace Rosen
} // namespace OHOS