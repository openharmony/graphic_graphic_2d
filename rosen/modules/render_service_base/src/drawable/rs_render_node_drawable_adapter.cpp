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
#include "pipeline/rs_render_node.h"
#include "platform/common/rs_log.h"

namespace OHOS::Rosen::DrawableV2 {
std::unordered_map<RSRenderNodeType, RSRenderNodeDrawableAdapter::Generator> RSRenderNodeDrawableAdapter::GeneratorMap;
std::unordered_map<NodeId, RSRenderNodeDrawableAdapter::WeakPtr> RSRenderNodeDrawableAdapter::RenderNodeDrawableCache;

RSRenderNodeDrawableAdapter::RSRenderNodeDrawableAdapter(std::shared_ptr<const RSRenderNode>&& node)
    : renderNode_(std::move(node)) {};

RSRenderNodeDrawableAdapter::SharedPtr RSRenderNodeDrawableAdapter::GetDrawableById(NodeId id)
{
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
    static const auto Destructor = [](RSRenderNodeDrawableAdapter* ptr) {
        RenderNodeDrawableCache.erase(ptr->renderNode_->GetId()); // Remove from cache before deleting
        delete ptr;
    };

    if (node == nullptr) {
        return nullptr;
    }
    auto id = node->GetId();

    // Try to get a cached drawable if it exists.
    if (const auto cacheIt = RenderNodeDrawableCache.find(id); cacheIt != RenderNodeDrawableCache.end()) {
        if (const auto ptr = cacheIt->second.lock()) {
            return ptr;
        } else {
            RenderNodeDrawableCache.erase(cacheIt);
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
    RenderNodeDrawableCache.emplace(id, sharedPtr);
    return sharedPtr;
}

RSRenderNodeDrawableAdapter::SharedPtr RSRenderNodeDrawableAdapter::OnGenerateShadowDrawable(
    const std::shared_ptr<const RSRenderNode>& node)
{
    static std::unordered_map<NodeId, RSRenderNodeDrawableAdapter::WeakPtr> shadowDrawableCache;
    static const auto Destructor = [](RSRenderNodeDrawableAdapter* ptr) {
        shadowDrawableCache.erase(ptr->renderNode_->GetId()); // Remove from cache before deleting
        delete ptr;
    };

    if (node == nullptr) {
        return nullptr;
    }
    auto id = node->GetId();

    // Try to get a cached drawable if it exists.
    if (const auto cacheIt = shadowDrawableCache.find(id); cacheIt != shadowDrawableCache.end()) {
        if (const auto ptr = cacheIt->second.lock()) {
            return ptr;
        } else {
            shadowDrawableCache.erase(cacheIt);
        }
    }

    auto ptr = new RSRenderNodeShadowDrawable(node);
    auto sharedPtr = std::shared_ptr<RSRenderNodeDrawableAdapter>(ptr, Destructor);
    shadowDrawableCache.emplace(id, sharedPtr);
    return sharedPtr;
}

void RSRenderNodeDrawableAdapter::DrawRangeImpl(
    Drawing::Canvas& canvas, const Drawing::Rect& rect, int8_t start, int8_t end) const
{
    const auto& drawCmdList_ = renderNode_->drawCmdList_;

    if (UNLIKELY(skipShadow_)) {
        auto shadowIndex = renderNode_->drawCmdIndex_.shadowIndex_;
        if (shadowIndex != -1 || start <= shadowIndex || end > shadowIndex) {
            for (auto i = start; i < shadowIndex; i++) {
                drawCmdList_[i](&canvas, &rect);
            }
            for (auto i = shadowIndex + 1; i < end; i++) {
                drawCmdList_[i](&canvas, &rect);
            }
        }
    }

    for (auto i = start; i < end; i++) {
        drawCmdList_[i](&canvas, &rect);
    }
}

void RSRenderNodeDrawableAdapter::DrawBackground(Drawing::Canvas& canvas, const Drawing::Rect& rect) const
{
    DrawRangeImpl(canvas, rect, 0, renderNode_->drawCmdIndex_.backgroundEndIndex_);
}

void RSRenderNodeDrawableAdapter::DrawContent(Drawing::Canvas& canvas, const Drawing::Rect& rect) const
{
    auto index = renderNode_->drawCmdIndex_.contentIndex_;
    if (index == -1) {
        return;
    }
    renderNode_->drawCmdList_[index](&canvas, &rect);
}

void RSRenderNodeDrawableAdapter::DrawChildren(Drawing::Canvas& canvas, const Drawing::Rect& rect) const
{
    auto index = renderNode_->drawCmdIndex_.childrenIndex_;
    if (index == -1) {
        return;
    }
    renderNode_->drawCmdList_[index](&canvas, &rect);
}

void RSRenderNodeDrawableAdapter::DrawUifirstContentChildren(Drawing::Canvas& canvas, const Drawing::Rect& rect) const
{
    const auto& drawCmdList_ = renderNode_->uifirstDrawCmdList_;
    auto contentIdx = renderNode_->uifirstDrawCmdIndex_.contentIndex_;
    auto childrenIdx = renderNode_->uifirstDrawCmdIndex_.childrenIndex_;
    if (contentIdx != -1) {
        drawCmdList_[contentIdx](&canvas, &rect);
    }
    if (childrenIdx != -1) {
        drawCmdList_[childrenIdx](&canvas, &rect);
    }
}

void RSRenderNodeDrawableAdapter::DrawForeground(Drawing::Canvas& canvas, const Drawing::Rect& rect) const
{
    DrawRangeImpl(canvas, rect, renderNode_->drawCmdIndex_.foregroundBeginIndex_, renderNode_->drawCmdIndex_.endIndex_);
}

void RSRenderNodeDrawableAdapter::DrawAll(Drawing::Canvas& canvas, const Drawing::Rect& rect) const
{
    const auto& drawCmdList_ = renderNode_->drawCmdList_;
    if (drawCmdList_.empty()) {
        return;
    }
    DrawRangeImpl(canvas, rect, 0, renderNode_->drawCmdIndex_.endIndex_);
}

void RSRenderNodeDrawableAdapter::DumpDrawableTree(int32_t depth, std::string& out) const
{
    for (int32_t i = 0; i < depth; ++i) {
        out += "  ";
    }
    renderNode_->DumpNodeType(out);
    out += "[" + std::to_string(renderNode_->GetId()) + "]";
    renderNode_->DumpSubClassNode(out);
    out += ", DrawableVec:[" + DumpDrawableVec() + "]";
    out += ", " + renderNode_->GetRenderParams()->ToString();
    out += "\n";

    auto childrenDrawable = std::static_pointer_cast<RSChildrenDrawable>(
        renderNode_->drawableVec_[static_cast<int32_t>(RSDrawableSlot::CHILDREN)]);
    if (childrenDrawable) {
        for (const auto& renderNodeDrawable : childrenDrawable->childrenDrawableVec_) {
            renderNodeDrawable->DumpDrawableTree(depth + 1, out);
        }
    }
}

std::string RSRenderNodeDrawableAdapter::DumpDrawableVec() const
{
    const auto& drawableVec = renderNode_->drawableVec_;
    std::string str;
    for (uint i = 0; i < drawableVec.size(); ++i) {
        if (drawableVec[i]) {
            str += std::to_string(i) + ", ";
        }
    }
    if (str.length() > 2) {
        str.pop_back();
        str.pop_back();
    }

    return str;
}

bool RSRenderNodeDrawableAdapter::QuickReject(Drawing::Canvas& canvas, RectI localDrawRect)
{
    Drawing::Rect dst;
    canvas.GetTotalMatrix().MapRect(
        dst, { localDrawRect.GetLeft(), localDrawRect.GetTop(), localDrawRect.GetRight(), localDrawRect.GetBottom() });
    auto deviceClipRegion = static_cast<RSPaintFilterCanvas*>(&canvas)->GetDirtyRegion();
    Drawing::Region dstRegion;
    dstRegion.SetRect(dst.RoundOut());
    return !(deviceClipRegion.IsIntersects(dstRegion));
}

void RSRenderNodeDrawableAdapter::DrawBackgroundWithoutFilterAndEffect(
    Drawing::Canvas& canvas, const RSRenderParams& params) const
{
    const auto& drawCmdList_ = renderNode_->drawCmdList_;
    auto backgroundIndex = renderNode_->drawCmdIndex_.backgroundEndIndex_;
    auto bounds = params.GetBounds();
    auto curCanvas = static_cast<RSPaintFilterCanvas*>(&canvas);
    for (auto index = 0; index < backgroundIndex; ++index) {
        if (index == renderNode_->drawCmdIndex_.shadowIndex_) {
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
        if (index != renderNode_->drawCmdIndex_.backgroundFilterIndex_ &&
            index != renderNode_->drawCmdIndex_.useEffectIndex_) {
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

bool RSRenderNodeDrawableAdapter::HasFilterOrEffect() const
{
    return renderNode_->drawCmdIndex_.shadowIndex_ != -1 || renderNode_->drawCmdIndex_.backgroundFilterIndex_ != -1 ||
           renderNode_->drawCmdIndex_.useEffectIndex_ != -1;
}

void RSRenderNodeShadowDrawable::Draw(Drawing::Canvas& canvas)
{
    // rect is not directly used, make a dummy rect
    static Drawing::Rect rect;

    auto shadowIndex = renderNode_->drawCmdIndex_.shadowIndex_;
    if (shadowIndex == -1) {
        return;
    }

    // TODO: dirty region reject
    auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(&canvas);
    RSAutoCanvasRestore acr(paintFilterCanvas, RSPaintFilterCanvas::SaveType::kCanvasAndAlpha);
    SetSkipShadow(false);
    DrawRangeImpl(canvas, rect, 0, shadowIndex);
}

void RSRenderNodeShadowDrawable::DumpDrawableTree(int32_t depth, std::string& out) const
{
    for (int32_t i = 0; i < depth; ++i) {
        out += "  ";
    }
    renderNode_->DumpNodeType(out);
    out += "[" + std::to_string(renderNode_->GetId()) + "] Draw Shadow Only\n";
}
} // namespace OHOS::Rosen::DrawableV2
