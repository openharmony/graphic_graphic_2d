/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "pipeline/rs_effect_utils.h"

#include "common/rs_optional_trace.h"
#include "feature/filter_cache_manager/rs_filter_cache_manager.h"
#include "pipeline/rs_effect_render_node.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
bool RSEffectUtils::AccumulateFilterRenderContext(RSRenderNode& node, const RSRenderNode& rootNode,
    FilterRenderContext& context)
{
    auto selfGeo = node.GetRenderProperties().GetBoundsGeometry();
    if (!selfGeo) {
        return false;
    }

    Drawing::Matrix selfMatrix = selfGeo->GetMatrix();
    auto directParent = node.GetParent().lock();
    NodeId offscreenNodeId = RSEffectUtils::IsOffscreenForFilterCache(node) ? node.GetId() : INVALID_NODEID;

    while (directParent && directParent->GetId() != rootNode.GetId()) {
        if (auto parentGeo = directParent->GetRenderProperties().GetBoundsGeometry()) {
            selfMatrix.PostConcat(parentGeo->GetMatrix());
        }

        if (RSEffectUtils::IsOffscreenForFilterCache(*directParent) && offscreenNodeId == INVALID_NODEID) {
            offscreenNodeId = directParent->GetId();
        }
        directParent = directParent->GetParent().lock();
    }

    if (!directParent) {
        return false;
    }

    if (offscreenNodeId != INVALID_NODEID) {
        context.offscreenNodeId = offscreenNodeId;
    }
    selfMatrix.PostConcat(context.absMatrix);
    context.absMatrix = selfMatrix;
    return true;
}

bool RSEffectUtils::IsOffscreenForFilterCache(RSRenderNode& node)
{
    // align with quickprepare offscreenCanvasNodeId_ logic, only check canvasRenderNode and unionRenderNode.
    return (node.GetType() == RSRenderNodeType::CANVAS_NODE || node.GetType() == RSRenderNodeType::UNION_NODE) && (
        node.GetDrawingCacheType() == RSDrawingCacheType::FOREGROUND_FILTER_CACHE || node.IsForegroundFilterEnable());
}

bool RSEffectUtils::ShouldSkipFilterNodeCheckInOccludedSubTree(
    const std::shared_ptr<RSSurfaceRenderNode>& curSurfaceNode, const RSRenderNode& rootNode,
    RSDirtyRegionManager& dirtyManager)
{
    return curSurfaceNode &&
           !curSurfaceNode->IsTransparent() &&
           dirtyManager.GetCurrentFrameDirtyRegion().IsEmpty();
}

void RSEffectUtils::UpdateFilterCacheWithBelowDirtyAndPendingPurge(RSRenderNode& node,
    RSDirtyRegionManager& dirtyManager)
{
    if (node.GetRenderProperties().GetMaterialFilter()) {
        node.UpdateFilterCacheWithBelowDirty(
            Occlusion::Rect(dirtyManager.GetCurrentFrameDirtyRegion()), RSDrawableSlot::MATERIAL_FILTER);
        node.UpdatePendingPurgeFilterDirtyRect(dirtyManager, RSDrawableSlot::MATERIAL_FILTER);
    }
    if (node.GetRenderProperties().GetBackgroundFilter()) {
        node.UpdateFilterCacheWithBelowDirty(
            Occlusion::Rect(dirtyManager.GetCurrentFrameDirtyRegion()));
        node.UpdatePendingPurgeFilterDirtyRect(dirtyManager, RSDrawableSlot::BACKGROUND_FILTER);
    }
}

bool RSEffectUtils::HasBackgroundDependentFilter(const RSProperties& properties)
{
    return properties.GetBackgroundFilter() || properties.GetNeedDrawBehindWindow() || properties.GetMaterialFilter();
}

void RSEffectUtils::Sync(int64_t vsyncStartTime)
{
#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    RSFilterCacheManager::SetFrameTimestamp(vsyncStartTime);
#endif
}

void RSEffectUtils::CheckRegionInsideForFilterCache(RSRenderNode& node,
    std::shared_ptr<DrawableV2::RSFilterDrawable> filterDrawable)
{
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    if (filterDrawable == nullptr) {
        return;
    }
    const auto& properties = node.GetRenderProperties();
    auto snapshotRegion = filterDrawable->GetVisibleSnapshotRegion(node.GetFilterRegionInfo().defaultFilterRegion_);
    auto lastSnapshotRegion = filterDrawable->GetLastVisibleSnapshotRegion(node.lastFilterRegion_);
    bool regionChanged = ((!snapshotRegion.IsInsideOf(lastSnapshotRegion)) ||
        (properties.GetNeedDrawBehindWindow() && snapshotRegion != lastSnapshotRegion)) &&
        !node.IsForceClearOrUseFilterCache(filterDrawable);
    RS_OPTIONAL_TRACE_NAME_FMT("node[%llu] background UpdateFilterCacheWithSelfDirty"
        " lastRect:%s, curRegion:%s", node.GetId(), lastSnapshotRegion.ToString().c_str(),
        snapshotRegion.ToString().c_str());
    if (regionChanged) {
        node.MarkFilterStatusChanged(filterDrawable, false, true);
    }
#endif
}

void RSEffectUtils::CheckRegionChangeForFilterCache(RSRenderNode& node,
    std::shared_ptr<DrawableV2::RSFilterDrawable> filterDrawable, bool isForeground)
{
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    if (filterDrawable == nullptr) {
        return;
    }
    auto curDrawRegion = filterDrawable->GetVisibleDrawRegion(node.GetFilterRegionInfo().defaultFilterRegion_);
    auto curSnapshotRegion = filterDrawable->GetVisibleSnapshotRegion(node.GetFilterRegionInfo().defaultFilterRegion_);
    auto lastDrawRegion = filterDrawable->GetLastVisibleDrawRegion(node.lastFilterRegion_);
    auto lastSnapshotRegion = filterDrawable->GetLastVisibleSnapshotRegion(node.lastFilterRegion_);

    bool regionChanged = (curDrawRegion != lastDrawRegion || curSnapshotRegion != lastSnapshotRegion) &&
        !node.IsForceClearOrUseFilterCache(filterDrawable);
    RS_OPTIONAL_TRACE_NAME_FMT("node[%llu] UpdateFilterCacheWithSelfDirty"
        " lastDrawRegion:%s, curDrawRegion:%s, lastSnapshotRegion:%s, curSnapshotRegion:%s, regionChanged:%d",
        node.GetId(),
        lastDrawRegion.ToString().c_str(), curDrawRegion.ToString().c_str(),
        lastSnapshotRegion.ToString().c_str(), curSnapshotRegion.ToString().c_str(),
        static_cast<int>(regionChanged));
    if (regionChanged) {
        node.MarkFilterStatusChanged(filterDrawable, isForeground, true);
    }
#endif
}

bool RSEffectUtils::HasValidEffectNode(const std::shared_ptr<RSRenderNode>& node)
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

bool RSEffectUtils::HasValidEffect(const RSRenderNode* node)
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

bool RSEffectUtils::HasValidFilterOrEffect(const RSRenderNode& renderNode)
{
    const RSProperties& properties = renderNode.GetRenderProperties();
    if (properties.GetBackgroundFilter() != nullptr ||
        properties.GetMaterialFilter() != nullptr || properties.GetUseEffect()) {
        RS_LOGD("%{public}s within filter range", __func__);
        return true;
    }
    // If there is BgBrightness, ensure the Effect is valid.
    return properties.IsBgBrightnessValid() && HasValidEffect(renderNode.GetParent().lock().get());
}
} // namespace Rosen
} // namespace OHOS
