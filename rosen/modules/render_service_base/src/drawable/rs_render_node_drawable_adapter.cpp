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
#include <mutex>
#include <sstream>

#include "skia_adapter/skia_canvas.h"

#include "common/rs_background_thread.h"
#include "common/rs_optional_trace.h"
#include "drawable/rs_misc_drawable.h"
#include "drawable/rs_render_node_shadow_drawable.h"
#include "params/rs_canvas_drawing_render_params.h"
#include "params/rs_effect_render_params.h"
#include "params/rs_logical_display_render_params.h"
#include "params/rs_screen_render_params.h"
#include "params/rs_surface_render_params.h"
#include "params/rs_rcd_render_params.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_render_node_gc.h"
#include "platform/common/rs_log.h"

#ifdef ROSEN_OHOS
#include "hisysevent.h"
#endif

namespace OHOS::Rosen::DrawableV2 {
std::map<RSRenderNodeType, RSRenderNodeDrawableAdapter::Generator> RSRenderNodeDrawableAdapter::GeneratorMap;
std::map<NodeId, RSRenderNodeDrawableAdapter::WeakPtr> RSRenderNodeDrawableAdapter::RenderNodeDrawableCache_;
std::unordered_map<NodeId, Drawing::Matrix> RSRenderNodeDrawableAdapter::unobscuredUECMatrixMap_;
#ifdef ROSEN_OHOS
thread_local RSRenderNodeDrawableAdapter* RSRenderNodeDrawableAdapter::curDrawingCacheRoot_ = nullptr;
#else
RSRenderNodeDrawableAdapter* RSRenderNodeDrawableAdapter::curDrawingCacheRoot_ = nullptr;
#endif

RSRenderNodeDrawableAdapter::RSRenderNodeDrawableAdapter(std::shared_ptr<const RSRenderNode>&& node)
    : nodeType_(node ? node->GetType() : RSRenderNodeType::UNKNOW), renderNode_(std::move(node)) {}

RSRenderNodeDrawableAdapter::~RSRenderNodeDrawableAdapter() = default;

RSRenderNodeDrawableAdapter::SharedPtr RSRenderNodeDrawableAdapter::GetDrawableById(NodeId id)
{
    std::lock_guard<std::mutex> lock(cacheMapMutex_);
    if (const auto cacheIt = RenderNodeDrawableCache_.find(id); cacheIt != RenderNodeDrawableCache_.end()) {
        if (const auto ptr = cacheIt->second.lock()) {
            return ptr;
        }
    }
    return nullptr;
}

std::vector<RSRenderNodeDrawableAdapter::SharedPtr> RSRenderNodeDrawableAdapter::GetDrawableVectorById(
    const std::unordered_set<NodeId>& ids)
{
    std::vector<RSRenderNodeDrawableAdapter::SharedPtr> vec;
    std::lock_guard<std::mutex> lock(cacheMapMutex_);
    for (const auto& id : ids) {
        if (const auto cacheIt = RenderNodeDrawableCache_.find(id); cacheIt != RenderNodeDrawableCache_.end()) {
            if (const auto ptr = cacheIt->second.lock()) {
                vec.push_back(ptr);
            }
        }
    }
    return vec;
}

RSRenderNodeDrawableAdapter::SharedPtr RSRenderNodeDrawableAdapter::OnGenerate(
    const std::shared_ptr<const RSRenderNode>& node)
{
    if (node == nullptr) {
        ROSEN_LOGE("RSRenderNodeDrawableAdapter::OnGenerate, node null");
        return nullptr;
    }
    if (node->renderDrawable_ != nullptr) {
        return node->renderDrawable_;
    }
    static const auto Destructor = [](RSRenderNodeDrawableAdapter* ptr) {
        RemoveDrawableFromCache(ptr->nodeId_); // Remove from cache before deleting
        RSRenderNodeGC::DrawableDestructor(ptr);
    };
    auto id = node->GetId();
    // Try to get a cached drawable if it exists.
    {
        std::lock_guard<std::mutex> lock(cacheMapMutex_);
        if (const auto cacheIt = RenderNodeDrawableCache_.find(id); cacheIt != RenderNodeDrawableCache_.end()) {
            if (const auto ptr = cacheIt->second.lock()) {
                ROSEN_LOGE("%{public}s, node id in Cache is %{public}" PRIu64
                    ", nodeType: %{public}u, drawableType: %{public}u", __func__, id, node->GetType(),
                    ptr->GetNodeType());
                if (node->GetType() == ptr->GetNodeType()) {
                    return ptr;
                }
            }
            RenderNodeDrawableCache_.erase(cacheIt);
        }
    }
    // If we don't have a cached drawable, try to generate a new one and cache it.
    const auto it = GeneratorMap.find(node->GetType());
    if (it == GeneratorMap.end()) {
#ifndef ROSEN_ARKUI_X
        ROSEN_LOGE("RSRenderNodeDrawableAdapter::OnGenerate, node type %{public}d is not supported", node->GetType());
#endif
        return nullptr;
    }
    auto ptr = it->second(node);
    auto sharedPtr = std::shared_ptr<RSRenderNodeDrawableAdapter>(ptr, Destructor);
    node->renderDrawable_ = sharedPtr;
    sharedPtr->nodeId_ = id;
    InitRenderParams(node, sharedPtr);

    {
        std::lock_guard<std::mutex> lock(cacheMapMutex_);
        RenderNodeDrawableCache_.emplace(id, sharedPtr);
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
        case RSRenderNodeType::SCREEN_NODE:
            sharedPtr->renderParams_ = std::make_unique<RSScreenRenderParams>(sharedPtr->nodeId_);
            sharedPtr->uifirstRenderParams_ = std::make_unique<RSScreenRenderParams>(sharedPtr->nodeId_);
            break;
        case RSRenderNodeType::EFFECT_NODE:
            sharedPtr->renderParams_ = std::make_unique<RSEffectRenderParams>(sharedPtr->nodeId_);
            sharedPtr->uifirstRenderParams_ = std::make_unique<RSEffectRenderParams>(sharedPtr->nodeId_);
            break;
        case RSRenderNodeType::ROUND_CORNER_NODE:
            sharedPtr->renderParams_ = std::make_unique<RSRcdRenderParams>(sharedPtr->nodeId_);
            sharedPtr->uifirstRenderParams_ = nullptr; // rcd node does not need uifirst params
            break;
        case RSRenderNodeType::CANVAS_DRAWING_NODE:
            sharedPtr->renderParams_ = std::make_unique<RSCanvasDrawingRenderParams>(sharedPtr->nodeId_);
            sharedPtr->uifirstRenderParams_ = std::make_unique<RSCanvasDrawingRenderParams>(sharedPtr->nodeId_);
            break;
        case RSRenderNodeType::LOGICAL_DISPLAY_NODE:
            sharedPtr->renderParams_ = std::make_unique<RSLogicalDisplayRenderParams>(sharedPtr->nodeId_);
            sharedPtr->uifirstRenderParams_ = std::make_unique<RSLogicalDisplayRenderParams>(sharedPtr->nodeId_);
            break;
        default:
            sharedPtr->renderParams_ = std::make_unique<RSRenderParams>(sharedPtr->nodeId_);
            sharedPtr->uifirstRenderParams_ = std::make_unique<RSRenderParams>(sharedPtr->nodeId_);
            break;
    }
    sharedPtr->renderParams_->SetParamsType(RSRenderParamsType::RS_PARAM_OWNED_BY_DRAWABLE);
    if (sharedPtr->uifirstRenderParams_) {
        sharedPtr->uifirstRenderParams_->SetParamsType(RSRenderParamsType::RS_PARAM_OWNED_BY_DRAWABLE_UIFIRST);
    }
}

RSRenderNodeDrawableAdapter::SharedPtr RSRenderNodeDrawableAdapter::OnGenerateShadowDrawable(
    const std::shared_ptr<const RSRenderNode>& node, const std::shared_ptr<RSRenderNodeDrawableAdapter>& drawable)
{
    static std::map<NodeId, RSRenderNodeDrawableAdapter::WeakPtr> shadowDrawableCache;
    static std::mutex shadowCacheMapMutex;
    static const auto Destructor = [](RSRenderNodeDrawableAdapter* ptr) {
        {
            std::lock_guard<std::mutex> lock(shadowCacheMapMutex);
            shadowDrawableCache.erase(ptr->nodeId_); // Remove from cache before deleting
        }
        RSRenderNodeGC::DrawableDestructor(ptr);
    };

    if (node == nullptr) {
        ROSEN_LOGE("RSRenderNodeDrawableAdapter::OnGenerateShadowDrawable, node null");
        return nullptr;
    }
    auto id = node->GetId();
    // Try to get a cached drawable if it exists.
    {
        std::lock_guard<std::mutex> lock(shadowCacheMapMutex);
        if (const auto cacheIt = shadowDrawableCache.find(id); cacheIt != shadowDrawableCache.end()) {
            if (const auto ptr = cacheIt->second.lock()) {
                return ptr;
            } else {
                shadowDrawableCache.erase(cacheIt);
            }
        }
    }

    auto ptr = new RSRenderNodeShadowDrawable(node, drawable);
    auto sharedPtr = std::shared_ptr<RSRenderNodeDrawableAdapter>(ptr, Destructor);
    {
        std::lock_guard<std::mutex> lock(shadowCacheMapMutex);
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

    if (static_cast<uint32_t>(end) > drawCmdList_.size()) {
        ROSEN_LOGE("RSRenderNodeDrawableAdapter::DrawRangeImpl, end is invalid");
        return;
    }

    if (UNLIKELY(skipType_ != SkipType::NONE)) {
        auto skipIndex_ = GetSkipIndex();
        if (start <= skipIndex_ && end > skipIndex_) {
            // skip index is in the range
            for (auto i = start; i < skipIndex_; i++) {
                drawCmdList_[i](&canvas, &rect);
            }
            for (auto i = skipIndex_ + 1; i < end; i++) {
                drawCmdList_[i](&canvas, &rect);
            }
            return;
        }
        // skip index is not in the range, fall back to normal drawing
    }

    for (auto i = start; i < end; i++) {
#ifdef RS_ENABLE_PREFETCH
            int prefetchIndex = i + 2;
            if (prefetchIndex < end) {
                __builtin_prefetch(&drawCmdList_[prefetchIndex], 0, 1);
            }
#endif
        drawCmdList_[i](&canvas, &rect);
    }
}

void RSRenderNodeDrawableAdapter::DrawImpl(Drawing::Canvas& canvas, const Drawing::Rect& rect, int8_t index) const
{
    if (drawCmdList_.empty() || index < 0 || static_cast<uint32_t>(index) >= drawCmdList_.size()) {
        return;
    }

    if (UNLIKELY(skipType_ != SkipType::NONE)) {
        auto skipIndex_ = GetSkipIndex();
        if (index == skipIndex_) {
            return;
        }
    }

    drawCmdList_[index](&canvas, &rect);
}

#ifdef SUBTREE_PARALLEL_ENABLE
void RSRenderNodeDrawableAdapter::DrawQuickImpl(
    Drawing::Canvas& canvas, const Drawing::Rect& rect) const
{
    if (drawCmdList_.empty()) {
        return;
    }

    if (drawCmdIndex_.transitionIndex_ != -1) {
        drawCmdList_[drawCmdIndex_.transitionIndex_](&canvas, &rect);
    }
    if (drawCmdIndex_.envForeGroundColorIndex_ != -1) {
        drawCmdList_[drawCmdIndex_.envForeGroundColorIndex_](&canvas, &rect);
    }

    // BG_START
    if (drawCmdIndex_.bgSaveBoundsIndex_ == - 1 ||
        (drawCmdIndex_.bgSaveBoundsIndex_ != -1 && drawCmdIndex_.bgRestoreBoundsIndex_ == -1)) {
        if (drawCmdIndex_.clipToBoundsIndex_ != -1) {
            drawCmdList_[drawCmdIndex_.clipToBoundsIndex_](&canvas, &rect);
        }
        if (drawCmdIndex_.backgroudStyleIndex_ != -1) {
            drawCmdList_[drawCmdIndex_.backgroudStyleIndex_](&canvas, &rect);
        }
    }

    if (drawCmdIndex_.envForegroundColorStrategyIndex_ != -1) {
        drawCmdList_[drawCmdIndex_.envForegroundColorStrategyIndex_](&canvas, &rect);
    }
    if (drawCmdIndex_.frameOffsetIndex_ != -1) {
        drawCmdList_[drawCmdIndex_.frameOffsetIndex_](&canvas, &rect);
    }
    if (drawCmdIndex_.clipToFrameIndex_ != -1) {
        drawCmdList_[drawCmdIndex_.clipToFrameIndex_](&canvas, &rect);
    }
    if (drawCmdIndex_.customClipToFrameIndex_ != -1) {
        drawCmdList_[drawCmdIndex_.customClipToFrameIndex_](&canvas, &rect);
    }

    if (drawCmdIndex_.contentIndex_ != -1) {
        drawCmdList_[drawCmdIndex_.contentIndex_](&canvas, &rect);
    }
}
#endif

void RSRenderNodeDrawableAdapter::DrawBackground(Drawing::Canvas& canvas, const Drawing::Rect& rect) const
{
    DrawRangeImpl(canvas, rect, 0, drawCmdIndex_.backgroundEndIndex_);
}

void RSRenderNodeDrawableAdapter::DrawLeashWindowBackground(Drawing::Canvas& canvas, const Drawing::Rect& rect,
    bool isStencilPixelOcclusionCullingEnabled, int64_t stencilVal) const
{
    if (!isStencilPixelOcclusionCullingEnabled) {
        DrawRangeImpl(canvas, rect, 0, drawCmdIndex_.backgroundEndIndex_);
        return;
    }
    DrawRangeImpl(canvas, rect, 0, drawCmdIndex_.shadowIndex_);
    canvas.SetStencilVal(stencilVal);
    DrawRangeImpl(canvas, rect, drawCmdIndex_.shadowIndex_, drawCmdIndex_.shadowIndex_ + 1);
    canvas.SetStencilVal(Drawing::Canvas::INVALID_STENCIL_VAL);
    DrawRangeImpl(canvas, rect, drawCmdIndex_.shadowIndex_ + 1, drawCmdIndex_.backgroundEndIndex_);
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

void RSRenderNodeDrawableAdapter::DrawUifirstContentChildren(Drawing::Canvas& canvas, const Drawing::Rect& rect)
{
    RSRenderNodeSingleDrawableLocker singleLocker(this);
    if (UNLIKELY(!singleLocker.IsLocked())) {
        singleLocker.DrawableOnDrawMultiAccessEventReport(__func__);
        RS_LOGE("RSRenderNodeDrawableAdapter::DrawUifirstContentChildren node %{public}" PRIu64 " onDraw!!!", GetId());
        if (RSSystemProperties::GetSingleDrawableLockerEnabled()) {
            return;
        }
    }

    if (uifirstDrawCmdList_.empty()) {
        return;
    }

    const auto& drawCmdList = uifirstDrawCmdList_;
    auto contentIdx = uifirstDrawCmdIndex_.contentIndex_;
    auto childrenIdx = uifirstDrawCmdIndex_.childrenIndex_;
    if (0 <= contentIdx && contentIdx < drawCmdList.size()) {
        drawCmdList[contentIdx](&canvas, &rect);
    }
    if (0 <= childrenIdx && childrenIdx < drawCmdList.size()) {
        drawCmdList[childrenIdx](&canvas, &rect);
    }
}

void RSRenderNodeDrawableAdapter::DrawForeground(Drawing::Canvas& canvas, const Drawing::Rect& rect) const
{
    DrawRangeImpl(canvas, rect, drawCmdIndex_.foregroundBeginIndex_, drawCmdIndex_.endIndex_);
}

void RSRenderNodeDrawableAdapter::DrawAll(Drawing::Canvas& canvas, const Drawing::Rect& rect) const
{
    DrawRangeImpl(canvas, rect, 0, drawCmdIndex_.endIndex_);
}

// can only run in sync mode
void RSRenderNodeDrawableAdapter::DumpDrawableTree(int32_t depth, std::string& out, const RSContext& context) const
{
    // Exceed max depth for dumping drawable tree, refuse to dump and add a warning.
    // Possible reason: loop in the drawable tree
    constexpr int32_t MAX_DUMP_DEPTH = 256;
    if (depth >= MAX_DUMP_DEPTH) {
        ROSEN_LOGW("RSRenderNodeDrawableAdapter::DumpDrawableTree depth too large, stop dumping. current depth = %d, "
            "nodeId = %" PRIu64, depth, nodeId_);
        out += "===== WARNING: exceed max depth for dumping drawable tree =====\n";
        return;
    }

    for (int32_t i = 0; i < depth; ++i) {
        out += "  ";
    }
    // dump node info/DrawableVec/renderParams etc.
    auto renderNode = (depth == 0 && nodeId_ == INVALID_NODEID) ? context.GetGlobalRootRenderNode()
        : context.GetNodeMap().GetRenderNode<RSRenderNode>(nodeId_);
    if (renderNode == nullptr) {
        out += "[" + std::to_string(nodeId_) + ": nullptr]\n";
        return;
    }
    RSRenderNode::DumpNodeType(nodeType_, out);
    out += "[" + std::to_string(nodeId_) + "]";
    renderNode->DumpSubClassNode(out);
    out += ", DrawableVec:[" + DumpDrawableVec(renderNode) + "]";
    if (renderParams_ == nullptr) {
        out += ", StagingParams null";
    } else {
        out += ", " + renderParams_->ToString();
    }

    if (skipType_ != SkipType::NONE) {
        out += ", SkipType:" + std::to_string(static_cast<int>(skipType_));
        out += ", SkipIndex:" + std::to_string(GetSkipIndex());
    }
    if (drawSkipType_ != DrawSkipType::NONE) {
        out += ", DrawSkipType:" + std::to_string(static_cast<int>(drawSkipType_));
    }
    out += ", ChildrenIndex:" + std::to_string(drawCmdIndex_.childrenIndex_);
    out += "\n";

    // Dump children drawable(s)
    auto childrenDrawable = std::static_pointer_cast<RSChildrenDrawable>(
        renderNode->drawableVec_[static_cast<int32_t>(RSDrawableSlot::CHILDREN)]);
    if (childrenDrawable) {
        const auto& childrenVec = childrenDrawable->needSync_ ? childrenDrawable->stagingChildrenDrawableVec_
            : childrenDrawable->childrenDrawableVec_;
        for (const auto& renderNodeDrawable : childrenVec) {
            renderNodeDrawable->DumpDrawableTree(depth + 1, out, context);
        }
    }
}

// can only run in sync mode
std::string RSRenderNodeDrawableAdapter::DumpDrawableVec(const std::shared_ptr<RSRenderNode>& renderNode) const
{
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

bool RSRenderNodeDrawableAdapter::QuickReject(Drawing::Canvas& canvas, const RectF& localDrawRect)
{
    auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(&canvas);
    if (paintFilterCanvas->IsDirtyRegionStackEmpty()) {
        return false;
    }

    Drawing::Rect dst;
    canvas.GetTotalMatrix().MapRect(
        dst, { localDrawRect.GetLeft(), localDrawRect.GetTop(), localDrawRect.GetRight(), localDrawRect.GetBottom() });
    auto originalCanvas = paintFilterCanvas->GetOriginalCanvas();
    if (originalCanvas && !paintFilterCanvas->GetOffscreenDataList().empty()) {
        originalCanvas->GetTotalMatrix().MapRect(dst, dst);
    }
    Drawing::Region dstRegion;
    if (!dstRegion.SetRect(dst.RoundOut()) && !dst.IsEmpty()) {
        RS_LOGW("invalid dstDrawRect: %{public}s, RoundOut: %{public}s",
            dst.ToString().c_str(), dst.RoundOut().ToString().c_str());
        RS_OPTIONAL_TRACE_NAME_FMT("invalid dstDrawRect: %s, RoundOut: %s",
            dst.ToString().c_str(), dst.RoundOut().ToString().c_str());
        return false;
    }
    return !(paintFilterCanvas->GetCurDirtyRegion().IsIntersects(dstRegion));
}

void RSRenderNodeDrawableAdapter::CollectInfoForNodeWithoutFilter(Drawing::Canvas& canvas)
{
    if (drawCmdList_.empty() || curDrawingCacheRoot_ == nullptr) {
        return;
    }
    curDrawingCacheRoot_->withoutFilterMatrixMap_[GetId()] = canvas.GetTotalMatrix();
}

void RSRenderNodeDrawableAdapter::CollectInfoForUnobscuredUEC(Drawing::Canvas& canvas)
{
    if (!UECChildrenIds_ || UECChildrenIds_->empty()) {
        return;
    }
    for (auto childId : *UECChildrenIds_) {
        unobscuredUECMatrixMap_[childId] = canvas.GetTotalMatrix();
        auto drawable = GetDrawableById(childId);
        if (drawable) {
            drawable->SetUIExtensionNeedToDraw(true);
        }
    }
}

void RSRenderNodeDrawableAdapter::DrawBackgroundWithoutFilterAndEffect(
    Drawing::Canvas& canvas, const RSRenderParams& params)
{
    if (uifirstDrawCmdList_.empty()) {
        return;
    }

    auto backgroundIndex = drawCmdIndex_.backgroundEndIndex_;
    auto bounds = params.GetBounds();
    auto curCanvas = static_cast<RSPaintFilterCanvas*>(&canvas);
    if (!curCanvas) {
        RS_LOGD("RSRenderNodeDrawableAdapter::DrawBackgroundWithoutFilterAndEffect curCanvas is null");
        return;
    }
    for (auto index = 0; index < backgroundIndex; ++index) {
        if (index == drawCmdIndex_.shadowIndex_) {
            if (!params.GetShadowRect().IsEmpty()) {
                auto shadowRect = params.GetShadowRect();
                RS_OPTIONAL_TRACE_NAME_FMT("ClipHoleForBlur shadowRect:[%.2f, %.2f, %.2f, %.2f]", shadowRect.GetLeft(),
                    shadowRect.GetTop(), shadowRect.GetWidth(), shadowRect.GetHeight());
                Drawing::AutoCanvasRestore arc(*curCanvas, true);
                curCanvas->ResetClip();
                curCanvas->ClipRect(shadowRect);
                curCanvas->Clear(Drawing::Color::COLOR_TRANSPARENT);
                UpdateFilterInfoForNodeGroup(curCanvas);
            } else {
                CollectInfoForNodeWithoutFilter(canvas);
                drawCmdList_[index](&canvas, &bounds);
            }
            continue;
        }
        if (index != drawCmdIndex_.useEffectIndex_ || index != drawCmdIndex_.backgroundFilterIndex_) {
            RS_OPTIONAL_TRACE_NAME_FMT(
                "ClipHoleForBlur filterRect:[%.2f, %.2f]", bounds.GetWidth(), bounds.GetHeight());
            Drawing::AutoCanvasRestore arc(*curCanvas, true);
            curCanvas->ClipRect(bounds, Drawing::ClipOp::INTERSECT, false);
            curCanvas->Clear(Drawing::Color::COLOR_TRANSPARENT);
            UpdateFilterInfoForNodeGroup(curCanvas);
        } else {
            drawCmdList_[index](&canvas, &bounds);
        }
    }
}

void RSRenderNodeDrawableAdapter::UpdateFilterInfoForNodeGroup(RSPaintFilterCanvas* curCanvas)
{
    if (curDrawingCacheRoot_ != nullptr) {
        auto iter = std::find_if(curDrawingCacheRoot_->filterInfoVec_.begin(),
            curDrawingCacheRoot_->filterInfoVec_.end(),
            [nodeId = GetId()](const auto& item) -> bool { return item.nodeId_ == nodeId; });
        if (iter != curDrawingCacheRoot_->filterInfoVec_.end()) {
            iter->rectVec_.emplace_back(curCanvas->GetDeviceClipBounds());
        } else {
            curDrawingCacheRoot_->filterInfoVec_.emplace_back(
                FilterNodeInfo(GetId(), curCanvas->GetTotalMatrix(), { curCanvas->GetDeviceClipBounds() }));
        }
    }
}

void RSRenderNodeDrawableAdapter::CheckShadowRectAndDrawBackground(
    Drawing::Canvas& canvas, const RSRenderParams& params)
{
    // The shadow without shadowRect has drawn in Nodegroup's cache, so we can't draw it again
    if (!params.GetShadowRect().IsEmpty()) {
        DrawBackground(canvas, params.GetBounds());
    } else {
        DrawRangeImpl(
            canvas, params.GetBounds(), drawCmdIndex_.foregroundFilterBeginIndex_, drawCmdIndex_.backgroundEndIndex_);
    }
}

void RSRenderNodeDrawableAdapter::DrawBeforeCacheWithForegroundFilter(Drawing::Canvas& canvas,
    const Drawing::Rect& rect) const
{
    DrawRangeImpl(canvas, rect, 0, static_cast<int8_t>(drawCmdIndex_.foregroundFilterBeginIndex_));
}

void RSRenderNodeDrawableAdapter::DrawCacheWithForegroundFilter(Drawing::Canvas& canvas,
    const Drawing::Rect& rect) const
{
    DrawRangeImpl(canvas, rect, drawCmdIndex_.foregroundFilterBeginIndex_,
        drawCmdIndex_.foregroundFilterEndIndex_);
}

void RSRenderNodeDrawableAdapter::DrawAfterCacheWithForegroundFilter(Drawing::Canvas& canvas,
    const Drawing::Rect& rect) const
{
    DrawRangeImpl(canvas, rect, drawCmdIndex_.foregroundFilterEndIndex_,
        drawCmdIndex_.endIndex_);
}

void RSRenderNodeDrawableAdapter::DrawCacheWithProperty(Drawing::Canvas& canvas, const Drawing::Rect& rect) const
{
    DrawRangeImpl(canvas, rect, drawCmdIndex_.renderGroupBeginIndex_,
        drawCmdIndex_.renderGroupEndIndex_);
}

void RSRenderNodeDrawableAdapter::DrawBeforeCacheWithProperty(Drawing::Canvas& canvas, const Drawing::Rect& rect) const
{
    DrawRangeImpl(canvas, rect, 0, static_cast<int8_t>(drawCmdIndex_.renderGroupBeginIndex_));
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

int8_t RSRenderNodeDrawableAdapter::GetSkipIndex() const
{
    switch (skipType_) {
        case SkipType::SKIP_BACKGROUND_COLOR:
            return drawCmdIndex_.backgroundColorIndex_;
        case SkipType::SKIP_SHADOW:
            return drawCmdIndex_.shadowIndex_;
        case SkipType::NONE:
        default:
            return -1;
    }
}

void RSRenderNodeDrawableAdapter::RemoveDrawableFromCache(const NodeId nodeId)
{
    std::lock_guard<std::mutex> lock(cacheMapMutex_);
    RenderNodeDrawableCache_.erase(nodeId);
}

void RSRenderNodeDrawableAdapter::RegisterClearSurfaceFunc(ClearSurfaceTask task)
{
    clearSurfaceTask_ = task;
}

void RSRenderNodeDrawableAdapter::ResetClearSurfaceFunc()
{
    clearSurfaceTask_ = nullptr;
}

void RSRenderNodeDrawableAdapter::TryClearSurfaceOnSync()
{
    if (!clearSurfaceTask_) {
        return;
    }
    clearSurfaceTask_();
}

bool RSRenderNodeDrawableAdapter::IsFilterCacheValidForOcclusion() const
{
    bool val = false;
    if (backgroundFilterDrawable_) {
        val = val || backgroundFilterDrawable_->IsFilterCacheValidForOcclusion();
    }
    if (compositingFilterDrawable_) {
        val = val || compositingFilterDrawable_->IsFilterCacheValidForOcclusion();
    }
    return val;
}

const RectI RSRenderNodeDrawableAdapter::GetFilterCachedRegion() const
{
    RectI rect{0, 0, 0, 0};
    if (compositingFilterDrawable_) {
        return compositingFilterDrawable_->GetFilterCachedRegion();
    } else if (backgroundFilterDrawable_) {
        return backgroundFilterDrawable_->GetFilterCachedRegion();
    } else {
        return rect;
    }
}
void RSRenderNodeDrawableAdapter::SetSkipCacheLayer(bool hasSkipCacheLayer)
{
    hasSkipCacheLayer_ = hasSkipCacheLayer;
}

void RSRenderNodeDrawableAdapter::SetChildInBlackList(bool hasChildInBlackList)
{
    hasChildInBlackList_ = hasChildInBlackList;
}

void RSRenderNodeDrawableAdapter::ApplyForegroundColorIfNeed(Drawing::Canvas& canvas, const Drawing::Rect& rect) const
{
    if (drawCmdList_.empty()) {
        return;
    }
    if (drawCmdIndex_.envForeGroundColorIndex_ != -1) {
        drawCmdList_[drawCmdIndex_.envForeGroundColorIndex_](&canvas, &rect);
    }
}

// will only called by OnDraw or OnSync
void RSRenderNodeSingleDrawableLocker::DrawableOnDrawMultiAccessEventReport(const std::string& func) const
{
#ifdef ROSEN_OHOS
    auto tid = gettid();
    MultiAccessReportInfo reportInfo;
    if (drawable_) {
        reportInfo.drawableNotNull = true;
        reportInfo.nodeId = drawable_->GetId();
        reportInfo.nodeType = drawable_->GetNodeType();
        const auto& params = drawable_->GetRenderParams();
        if (params) {
            reportInfo.paramNotNull = true;
            reportInfo.uifirstRootNodeId = params->GetUifirstRootNodeId();
            reportInfo.firstLevelNodeId = params->GetFirstLevelNodeId();
        }
    }

    auto task = [tid, func, reportInfo]() {
        RS_TRACE_NAME_FMT("DrawableOnDrawMultiAccessEventReport HiSysEventWrite nodeId:%" PRIu64, reportInfo.nodeId);
        std::ostringstream oss;
        oss << "func:" << func << ",";
        oss << "drawable:" << reportInfo.drawableNotNull << ",";
        oss << "param:" << reportInfo.paramNotNull << ",";
        if (reportInfo.drawableNotNull) {
            oss << "id:" << reportInfo.nodeId << ",";
            oss << "type:" << static_cast<int>(reportInfo.nodeType) << ",";
        }
        if (reportInfo.paramNotNull) {
            oss << "URN:" << reportInfo.uifirstRootNodeId << ",";
            oss << "FLN:" << reportInfo.firstLevelNodeId << ",";
        }
        oss << "tid:" << tid;
        HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::GRAPHIC, "RENDER_DRAWABLE_MULTI_ACCESS",
            OHOS::HiviewDFX::HiSysEvent::EventType::STATISTIC, "MULTI_ACCESS_MSG", oss.str());
    };
    RSBackgroundThread::Instance().PostTask(task);
#endif
}
} // namespace OHOS::Rosen::DrawableV2
