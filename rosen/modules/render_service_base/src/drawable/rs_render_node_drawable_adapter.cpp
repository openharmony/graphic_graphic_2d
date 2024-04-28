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

#include "drawable/rs_render_node_drawable_adapter.h"

#include "skia_adapter/skia_canvas.h"
#include "src/core/SkCanvasPriv.h"

#include "common/rs_optional_trace.h"
#include "drawable/rs_misc_drawable.h"
#include "params/rs_canvas_drawing_render_params.h"
#include "params/rs_display_render_params.h"
#include "params/rs_surface_render_params.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_render_node_gc.h"
#include "platform/common/rs_log.h"

namespace OHOS::Rosen::DrawableV2 {
RSRenderNodeDrawableAdapter::Generator RSRenderNodeDrawableAdapter::shadowGenerator_ = nullptr;
std::map<RSRenderNodeType, RSRenderNodeDrawableAdapter::Generator> RSRenderNodeDrawableAdapter::GeneratorMap;
std::map<NodeId, RSRenderNodeDrawableAdapter::WeakPtr> RSRenderNodeDrawableAdapter::RenderNodeDrawableCache;

RSRenderNodeDrawableAdapter::RSRenderNodeDrawableAdapter(std::shared_ptr<const RSRenderNode>&& node)
    : nodeType_(node->GetType()), renderNode_(std::move(node)) {}

RSRenderNodeDrawableAdapter::SharedPtr RSRenderNodeDrawableAdapter::GetDrawableById(NodeId id)
{
    std::lock_guard<std::mutex> lock(cacheMapMutex_);
    if (const auto cacheIt = RenderNodeDrawableCache.find(id); cacheIt != RenderNodeDrawableCache.end()) {
        if (const auto ptr = cacheIt->second.lock()) {
            return ptr;
        }
    }
    return nullptr;
}

RSRenderNodeDrawableAdapter::SharedPtr RSRenderNodeDrawableAdapter::OnGenerate(
    const std::shared_ptr<const RSRenderNode>& node)
{
    if (node == nullptr) {
        return nullptr;
    }
    if (node->renderDrawable_ != nullptr) {
        return node->renderDrawable_;
    }
    static const auto Destructor = [](RSRenderNodeDrawableAdapter* ptr) {
        {
            std::lock_guard<std::mutex> lock(cacheMapMutex_);
            RenderNodeDrawableCache.erase(ptr->nodeId_); // Remove from cache before deleting
        }
        RSRenderNodeGC::DrawableDestructor(ptr);
    };
    auto id = node->GetId();
    // Try to get a cached drawable if it exists.
    {
        std::lock_guard<std::mutex> lock(cacheMapMutex_);
        if (const auto cacheIt = RenderNodeDrawableCache.find(id); cacheIt != RenderNodeDrawableCache.end()) {
            if (const auto ptr = cacheIt->second.lock()) {
                return ptr;
            } else {
                RenderNodeDrawableCache.erase(cacheIt);
            }
        }
    }
    // If we don't have a cached drawable, try to generate a new one and cache it.
    const auto it = GeneratorMap.find(node->GetType());
    if (it == GeneratorMap.end()) {
        ROSEN_LOGE("RSRenderNodeDrawableAdapter::OnGenerate, node type %d is not supported", node->GetType());
        return nullptr;
    }
    auto ptr = it->second(node);
    auto sharedPtr = std::shared_ptr<RSRenderNodeDrawableAdapter>(ptr, Destructor);
    node->renderDrawable_ = sharedPtr;
    sharedPtr->nodeId_ = id;
    InitRenderParams(node, sharedPtr);

    {
        std::lock_guard<std::mutex> lock(cacheMapMutex_);
        RenderNodeDrawableCache.emplace(id, sharedPtr);
    }
    return sharedPtr;
}

void RSRenderNodeDrawableAdapter::InitRenderParams(const std::shared_ptr<const RSRenderNode>& node,
                                            std::shared_ptr<RSRenderNodeDrawableAdapter>& sharedPtr)
{
    switch (node->GetType()) {
        case RSRenderNodeType::SURFACE_NODE:
            sharedPtr->renderParams_ = std::make_unique<RSSurfaceRenderParams>(sharedPtr->nodeId_);
            sharedPtr->uifirstRenderParams_ = std::make_unique<RSSurfaceRenderParams>(sharedPtr->nodeId_);
            break;
        case RSRenderNodeType::DISPLAY_NODE:
            sharedPtr->renderParams_ = std::make_unique<RSDisplayRenderParams>(sharedPtr->nodeId_);
            sharedPtr->uifirstRenderParams_ = std::make_unique<RSDisplayRenderParams>(sharedPtr->nodeId_);
            break;
        case RSRenderNodeType::CANVAS_DRAWING_NODE:
            sharedPtr->renderParams_ = std::make_unique<RSCanvasDrawingRenderParams>(sharedPtr->nodeId_);
            sharedPtr->uifirstRenderParams_ = std::make_unique<RSCanvasDrawingRenderParams>(sharedPtr->nodeId_);
            break;
        default:
            sharedPtr->renderParams_ = std::make_unique<RSRenderParams>(sharedPtr->nodeId_);
            sharedPtr->uifirstRenderParams_ = std::make_unique<RSRenderParams>(sharedPtr->nodeId_);
            break;
    }
}

RSRenderNodeDrawableAdapter::SharedPtr RSRenderNodeDrawableAdapter::OnGenerateShadowDrawable(
    const std::shared_ptr<const RSRenderNode>& node)
{
    static std::map<NodeId, RSRenderNodeDrawableAdapter::WeakPtr> shadowDrawableCache;
    static std::mutex cacheMapMutex;
    static const auto Destructor = [](RSRenderNodeDrawableAdapter* ptr) {
        {
            std::lock_guard<std::mutex> lock(cacheMapMutex);
            shadowDrawableCache.erase(ptr->nodeId_); // Remove from cache before deleting
        }
        RSRenderNodeGC::DrawableDestructor(ptr);
    };

    if (node == nullptr || shadowGenerator_ == nullptr) {
        return nullptr;
    }
    auto id = node->GetId();
    // Try to get a cached drawable if it exists.
    {
        std::lock_guard<std::mutex> lock(cacheMapMutex);
        if (const auto cacheIt = shadowDrawableCache.find(id); cacheIt != shadowDrawableCache.end()) {
            if (const auto ptr = cacheIt->second.lock()) {
                return ptr;
            } else {
                shadowDrawableCache.erase(cacheIt);
            }
        }
    }

    auto ptr = shadowGenerator_(node);
    auto sharedPtr = std::shared_ptr<RSRenderNodeDrawableAdapter>(ptr, Destructor);
    {
        std::lock_guard<std::mutex> lock(cacheMapMutex);
        shadowDrawableCache.emplace(id, sharedPtr);
    }
    return sharedPtr;
}

void RSRenderNodeDrawableAdapter::DrawRangeImpl(
    Drawing::Canvas& canvas, const Drawing::Rect& rect, int8_t start, int8_t end) const
{
    if (drawCmdList_.empty() || start < 0 || end < 0 || start > end) {
        return;
    }

    if (UNLIKELY(skipIndex_ != -1)) {
        if (start <= skipIndex_ || end > skipIndex_) {
            for (auto i = start; i < skipIndex_; i++) {
                drawCmdList_[i](&canvas, &rect);
            }
            for (auto i = skipIndex_ + 1; i < end; i++) {
                drawCmdList_[i](&canvas, &rect);
            }
        }
        return;
    }

    for (auto i = start; i < end; i++) {
        drawCmdList_[i](&canvas, &rect);
    }
}

void RSRenderNodeDrawableAdapter::DrawBackground(Drawing::Canvas& canvas, const Drawing::Rect& rect) const
{
    if (drawCmdList_.empty()) {
        return;
    }
    DrawRangeImpl(canvas, rect, 0, drawCmdIndex_.backgroundEndIndex_);
}

void RSRenderNodeDrawableAdapter::DrawContent(Drawing::Canvas& canvas, const Drawing::Rect& rect) const
{
    if (drawCmdList_.empty()) {
        return;
    }

    auto index = drawCmdIndex_.contentIndex_;
    if (index == -1) {
        return;
    }
    drawCmdList_[index](&canvas, &rect);
}

void RSRenderNodeDrawableAdapter::DrawChildren(Drawing::Canvas& canvas, const Drawing::Rect& rect) const
{
    if (drawCmdList_.empty()) {
        return;
    }

    auto index = drawCmdIndex_.childrenIndex_;
    if (index == -1) {
        return;
    }
    drawCmdList_[index](&canvas, &rect);
}

void RSRenderNodeDrawableAdapter::DrawUifirstContentChildren(Drawing::Canvas& canvas, const Drawing::Rect& rect) const
{
    if (uifirstDrawCmdList_.empty()) {
        return;
    }

    const auto& drawCmdList = uifirstDrawCmdList_;
    auto contentIdx = uifirstDrawCmdIndex_.contentIndex_;
    auto childrenIdx = uifirstDrawCmdIndex_.childrenIndex_;
    if (contentIdx != -1) {
        drawCmdList[contentIdx](&canvas, &rect);
    }
    if (childrenIdx != -1) {
        drawCmdList[childrenIdx](&canvas, &rect);
    }
}

void RSRenderNodeDrawableAdapter::DrawForeground(Drawing::Canvas& canvas, const Drawing::Rect& rect) const
{
    DrawRangeImpl(canvas, rect, drawCmdIndex_.foregroundBeginIndex_, drawCmdIndex_.endIndex_);
}

void RSRenderNodeDrawableAdapter::DrawAll(Drawing::Canvas& canvas, const Drawing::Rect& rect) const
{
    const auto& drawCmdList = drawCmdList_;
    if (drawCmdList.empty()) {
        return;
    }
    DrawRangeImpl(canvas, rect, 0, drawCmdIndex_.endIndex_);
}

void RSRenderNodeDrawableAdapter::DumpDrawableTree(int32_t depth, std::string& out) const
{
    for (int32_t i = 0; i < depth; ++i) {
        out += "  ";
    }
    auto renderNode = renderNode_.lock();
    if (renderNode == nullptr) {
        return;
    }
    RSRenderNode::DumpNodeType(nodeType_, out);
    out += "[" + std::to_string(nodeId_) + "]";
    renderNode->DumpSubClassNode(out);
    out += ", DrawableVec:[" + DumpDrawableVec() + "]";
    out += ", " + renderNode->GetRenderParams()->ToString();
    out += "\n";

    auto childrenDrawable = std::static_pointer_cast<RSChildrenDrawable>(
        renderNode->drawableVec_[static_cast<int32_t>(RSDrawableSlot::CHILDREN)]);
    if (childrenDrawable) {
        for (const auto& renderNodeDrawable : childrenDrawable->childrenDrawableVec_) {
            renderNodeDrawable->DumpDrawableTree(depth + 1, out);
        }
    }
}

std::string RSRenderNodeDrawableAdapter::DumpDrawableVec() const
{
    auto renderNode = renderNode_.lock();
    if (renderNode == nullptr) {
        return "";
    }
    const auto& drawableVec = renderNode->drawableVec_;
    std::string str;
    for (uint8_t i = 0; i < drawableVec.size(); ++i) {
        if (drawableVec[i]) {
            str += std::to_string(i) + ", ";
        }
    }
    // str has more than 2 chars
    if (str.length() > 2) {
        str.pop_back();
        str.pop_back();
    }

    return str;
}

bool RSRenderNodeDrawableAdapter::QuickReject(Drawing::Canvas& canvas, RectF localDrawRect)
{
    auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(&canvas);
    if (paintFilterCanvas->IsDirtyRegionStackEmpty() || paintFilterCanvas->GetIsParallelCanvas()) {
        return false;
    }

    Drawing::Rect dst;
    canvas.GetTotalMatrix().MapRect(
        dst, { localDrawRect.GetLeft(), localDrawRect.GetTop(), localDrawRect.GetRight(), localDrawRect.GetBottom() });
    auto deviceClipRegion = paintFilterCanvas->GetCurDirtyRegion();
    Drawing::Region dstRegion;
    dstRegion.SetRect(dst.RoundOut());
    return !(deviceClipRegion.IsIntersects(dstRegion));
}

void RSRenderNodeDrawableAdapter::DrawBackgroundWithoutFilterAndEffect(
    Drawing::Canvas& canvas, const RSRenderParams& params) const
{
    if (uifirstDrawCmdList_.empty()) {
        return;
    }

    auto backgroundIndex = drawCmdIndex_.backgroundEndIndex_;
    auto bounds = params.GetBounds();
    auto curCanvas = static_cast<RSPaintFilterCanvas*>(&canvas);
    for (auto index = 0; index < backgroundIndex; ++index) {
        if (index == drawCmdIndex_.shadowIndex_) {
            if (!params.GetShadowRect().IsEmpty()) {
                auto shadowRect = params.GetShadowRect();
                RS_OPTIONAL_TRACE_NAME_FMT("ClipHoleForBlur shadowRect:[%.2f, %.2f, %.2f, %.2f]", shadowRect.GetLeft(),
                    shadowRect.GetTop(), shadowRect.GetWidth(), shadowRect.GetHeight());
                Drawing::AutoCanvasRestore arc(*curCanvas, true);
                auto coreCanvas = curCanvas->GetCanvasData();
                auto skiaCanvas = static_cast<Drawing::SkiaCanvas*>(coreCanvas.get());
                SkCanvasPriv::ResetClip(skiaCanvas->ExportSkCanvas());
                curCanvas->ClipRect(shadowRect);
                curCanvas->Clear(Drawing::Color::COLOR_TRANSPARENT);
            } else {
                drawCmdList_[index](&canvas, &bounds);
            }
            continue;
        }
        if (index != drawCmdIndex_.backgroundFilterIndex_ &&
            index != drawCmdIndex_.useEffectIndex_) {
            drawCmdList_[index](&canvas, &bounds);
        } else {
            RS_OPTIONAL_TRACE_NAME_FMT(
                "ClipHoleForBlur filterRect:[%.2f, %.2f]", bounds.GetWidth(), bounds.GetHeight());
            Drawing::AutoCanvasRestore arc(*curCanvas, true);
            curCanvas->ClipRect(bounds, Drawing::ClipOp::INTERSECT, false);
            curCanvas->Clear(Drawing::Color::COLOR_TRANSPARENT);
        }
    }
}

void RSRenderNodeDrawableAdapter::DrawCacheWithProperty(Drawing::Canvas& canvas, const Drawing::Rect& rect) const
{
    DrawRangeImpl(canvas, rect, drawCmdIndex_.renderGroupBeginIndex_,
        drawCmdIndex_.renderGroupEndIndex_);
}

void RSRenderNodeDrawableAdapter::DrawBeforeCacheWithProperty(Drawing::Canvas& canvas, const Drawing::Rect& rect) const
{
    DrawRangeImpl(canvas, rect, 0, static_cast<int8_t>(drawCmdIndex_.renderGroupBeginIndex_ - 1));
}

void RSRenderNodeDrawableAdapter::DrawAfterCacheWithProperty(Drawing::Canvas& canvas, const Drawing::Rect& rect) const
{
    DrawRangeImpl(canvas, rect, drawCmdIndex_.renderGroupEndIndex_,
        drawCmdIndex_.endIndex_);
}

bool RSRenderNodeDrawableAdapter::HasFilterOrEffect() const
{
    return drawCmdIndex_.shadowIndex_ != -1 || drawCmdIndex_.backgroundFilterIndex_ != -1 ||
           drawCmdIndex_.useEffectIndex_ != -1;
}
void RSRenderNodeDrawableAdapter::SetSkip(SkipType type)
{
    switch (type) {
        case SkipType::SKIP_BACKGROUND_COLOR:
            skipIndex_ = drawCmdIndex_.backgroundColorIndex_;
            break;
        case SkipType::SKIP_SHADOW:
            skipIndex_ = drawCmdIndex_.shadowIndex_;
            break;
        case SkipType::NONE:
        default:
            skipIndex_ = -1;
            break;
    }
}
} // namespace OHOS::Rosen::DrawableV2
