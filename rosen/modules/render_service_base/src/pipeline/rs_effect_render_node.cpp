/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "pipeline/rs_effect_render_node.h"

#include "memory/rs_memory_track.h"

#include "common/rs_obj_abs_geometry.h"
#include "platform/common/rs_log.h"
#include "property/rs_properties_painter.h"
#include "visitor/rs_node_visitor.h"

namespace OHOS {
namespace Rosen {
RSEffectRenderNode::RSEffectRenderNode(NodeId id, const std::weak_ptr<RSContext>& context) : RSRenderNode(id, context)
{
    MemoryInfo info = { sizeof(*this), ExtractPid(id), id, MEMORY_TYPE::MEM_RENDER_NODE };
    MemoryTrack::Instance().AddNodeRecord(id, info);
}

RSEffectRenderNode::~RSEffectRenderNode()
{
    MemoryTrack::Instance().RemoveNodeRecord(GetId());
}

void RSEffectRenderNode::Prepare(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    if (!visitor) {
        return;
    }
    visitor->PrepareEffectRenderNode(*this);
}

void RSEffectRenderNode::Process(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    if (!visitor) {
        return;
    }
    RSRenderNode::RenderTraceDebug();
    visitor->ProcessEffectRenderNode(*this);
}

void RSEffectRenderNode::ProcessRenderBeforeChildren(RSPaintFilterCanvas& canvas)
{
    RSRenderNode::ProcessTransitionBeforeChildren(canvas);
    auto& properties = GetRenderProperties();
    // Disable effect region if either of the following conditions is met:
    // 1. Effect region is null or empty (except when bg Image is set)
    // 2. Background filter is null
    // 3. Canvas is offscreen
    if ((!effectRegion_.has_value() && properties.GetBgImage() == nullptr) ||
        properties.GetBackgroundFilter() == nullptr ||
        canvas.GetCacheType() == RSPaintFilterCanvas::CacheType::OFFSCREEN) {
        canvas.SetEffectData(nullptr);
        return;
    }

#ifndef USE_ROSEN_DRAWING
    if (properties.GetBgImage() == nullptr) {
        // EffectRenderNode w/o background image, use snapshot as underlay image
        RSPropertiesPainter::DrawBackgroundEffect(properties, canvas, effectRegion_.value());
    } else {
        // EffectRenderNode w/ background image, use bg image as underlay image
        RSPropertiesPainter::DrawBackgroundImageAsEffect(properties, canvas);
    }
#else
    // PLANNING: add drawing implementation
#endif
}

RectI RSEffectRenderNode::GetFilterRect() const
{
    // w/ background image, simply return the bounds
    if (GetRenderProperties().GetBgImage()) {
        return RSRenderNode::GetFilterRect();
    }
    if (!effectRegion_.has_value()) {
        ROSEN_LOGD("RSEffectRenderNode::GetFilterRect: effectRegion has no value");
        return {};
    }
#ifndef USE_ROSEN_DRAWING
    auto& matrix = GetRenderProperties().GetBoundsGeometry()->GetAbsMatrix();
    // re-map local rect to absolute rect
    auto bounds = matrix.mapRect(SkRect::Make(*effectRegion_)).roundOut();
    return { bounds.x(), bounds.y(), bounds.width(), bounds.height() };
#else
    // PLANNING: add drawing implementation
    return {};
#endif
}

#ifndef USE_ROSEN_DRAWING
std::optional<SkPath> RSEffectRenderNode::InitializeEffectRegion() const
{
    // Only need to collect effect region if the background image is null
    return (GetRenderProperties().GetBgImage()) ? std::nullopt : std::make_optional<SkPath>();
}
#else
std::optional<Drawing::Path> RSEffectRenderNode::InitializeEffectRegion() const
{
    return (GetRenderProperties().GetBgImage()) ? std::nullopt : std::make_optional<Drawing::Path>();
}
#endif

#ifndef USE_ROSEN_DRAWING
void RSEffectRenderNode::SetEffectRegion(const std::optional<SkPath>& region)
{
    const auto& properties = GetRenderProperties();
    // if background image is set, use bounds as effect region
    if (properties.GetBgImage()) {
        UpdateEffectRegion(RSPropertiesPainter::Rect2SkRect(properties.GetBoundsRect()).roundOut());
        return;
    }

    if (!region.has_value() || region->isEmpty()) {
        ROSEN_LOGD("RSEffectRenderNode::SetEffectRegion: no effect region.");
        UpdateEffectRegion(std::nullopt);
        return;
    }

    const auto& geoPtr = properties.GetBoundsGeometry();
    const auto& absRect = geoPtr->GetAbsRect();
    // intersect effect region with node bounds
    auto rect = region->getBounds();
    if (!rect.intersect(
        SkRect::MakeLTRB(absRect.GetLeft(), absRect.GetTop(), absRect.GetRight(), absRect.GetBottom()))) {
        ROSEN_LOGD("RSEffectRenderNode::SetEffectRegion: no valid effect region.");
        UpdateEffectRegion(std::nullopt);
        return;
    }

    // Map absolute rect to local matrix
    const auto& matrix = geoPtr->GetAbsMatrix();
    SkMatrix revertMatrix;
    if (!matrix.invert(&revertMatrix)) {
        ROSEN_LOGD("RSEffectRenderNode::SetEffectRegion: cannot map effect region to local coordinates.");
        UpdateEffectRegion(std::nullopt);
        return;
    }
    UpdateEffectRegion(revertMatrix.mapRect(rect).roundOut());
}
#else
void RSEffectRenderNode::SetEffectRegion(const std::optional<Drawing::Path>& region)
{
    // PLANNING: add drawing implementation
}
#endif

#ifndef USE_ROSEN_DRAWING
void RSEffectRenderNode::UpdateEffectRegion(const std::optional<SkIRect>& region)
{
    // clear cache if new region is null or outside current region
    if (auto& manager = GetRenderProperties().GetFilterCacheManager(false);
        manager && manager->IsCacheValid() &&
        (!region.has_value() || (effectRegion_.has_value() && !effectRegion_->contains(*region)))) {
        manager->UpdateCacheStateWithFilterRegion();
    }
    effectRegion_ = region;
}
#endif

void RSEffectRenderNode::UpdateFilterCacheManagerWithCacheRegion(
    RSDirtyRegionManager& dirtyManager, const std::optional<RectI>& clipRect) const
{
    if (!GetRenderProperties().GetBgImage()) {
        RSRenderNode::UpdateFilterCacheManagerWithCacheRegion(dirtyManager, clipRect);
        return;
    }
    // We need to check cache validity by comparing the cached image region with the filter rect
    // PLANNING: the last != condition should be reconsidered
    if (auto& manager = GetRenderProperties().GetFilterCacheManager(false);
        manager != nullptr && manager->IsCacheValid() && manager->GetCachedImageRegion() != GetFilterRect()) {
        // If the cached image region is different from the filter rect, invalidate the cache
        manager->InvalidateCache();
    }
}

void RSEffectRenderNode::UpdateFilterCacheWithDirty(RSDirtyRegionManager& dirtyManager, bool isForeground) const
{
    // No need to invalidate cache if background image is not null
    if (GetRenderProperties().GetBgImage()) {
        return;
    }
    RSRenderNode::UpdateFilterCacheWithDirty(dirtyManager, isForeground);
}
} // namespace Rosen
} // namespace OHOS
