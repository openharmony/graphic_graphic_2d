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
#include "platform/common/rs_system_properties.h"

namespace OHOS {
namespace Rosen {
RSEffectRenderNode::RSEffectRenderNode(NodeId id, const std::weak_ptr<RSContext>& context, bool isTextureExportNode)
    : RSRenderNode(id, context, isTextureExportNode)
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
    // 1. No child with useEffect(true) (needFilter_ == false)
    // 2. Background filter is null
    // 3. Canvas is offscreen
    if (!needFilter_ || properties.GetBackgroundFilter() == nullptr ||
        !RSSystemProperties::GetEffectMergeEnabled() ||
        canvas.GetCacheType() == RSPaintFilterCanvas::CacheType::OFFSCREEN) {
        canvas.SetEffectData(nullptr);
        return;
    }

    if (properties.GetBgImage() == nullptr) {
        // EffectRenderNode w/o background image, use snapshot as underlay image
        RSPropertiesPainter::DrawBackgroundEffect(properties, canvas);
    } else {
        // EffectRenderNode w/ background image, use bg image as underlay image
        RSPropertiesPainter::DrawBackgroundImageAsEffect(properties, canvas);
    }
}

RectI RSEffectRenderNode::GetFilterRect() const
{
    if (!needFilter_) {
        return {};
    }
    return RSRenderNode::GetFilterRect();
}

#ifndef USE_ROSEN_DRAWING
void RSEffectRenderNode::SetEffectRegion(const std::optional<SkIRect>& effectRegion)
{
    if (!effectRegion.has_value() || effectRegion->isEmpty()) {
        ROSEN_LOGD("RSEffectRenderNode::SetEffectRegion: no effect region.");
        UpdateNeedFilter(false);
        return;
    }

    const auto& properties = GetRenderProperties();
    const auto& absRect = properties.GetBoundsGeometry()->GetAbsRect();
    if (!SkIRect::Intersects(*effectRegion,
        SkIRect::MakeLTRB(absRect.GetLeft(), absRect.GetTop(), absRect.GetRight(), absRect.GetBottom()))) {
        ROSEN_LOGD("RSEffectRenderNode::SetEffectRegion: effect region is not in node.");
        UpdateNeedFilter(false);
        return;
    }
    UpdateNeedFilter(true);
}
#else
void RSEffectRenderNode::SetEffectRegion(const std::optional<Drawing::RectI>& effectRegion)
{
    if (!effectRegion.has_value() || !effectRegion->IsValid()) {
        ROSEN_LOGD("RSEffectRenderNode::SetEffectRegion: no effect region.");
        UpdateNeedFilter(false);
        return;
    }

    const auto& properties = GetRenderProperties();
    const auto& absRect = properties.GetBoundsGeometry()->GetAbsRect();
    Drawing::RectI effectRect = effectRegion.value();
    if (!effectRect.Intersect(
        Drawing::RectI(absRect.GetLeft(), absRect.GetTop(), absRect.GetRight(), absRect.GetBottom()))) {
        ROSEN_LOGD("RSEffectRenderNode::SetEffectRegion: no valid effect region.");
        UpdateNeedFilter(false);
        return;
    }
    UpdateNeedFilter(true);
}
#endif

void RSEffectRenderNode::UpdateNeedFilter(bool needFilter)
{
    // clear cache if new region is null or outside current region
    if (auto& manager = GetRenderProperties().GetFilterCacheManager(false);
        manager && manager->IsCacheValid() && needFilter == false) {
        manager->UpdateCacheStateWithFilterRegion();
    }
    needFilter_ = needFilter;
}

void RSEffectRenderNode::UpdateFilterCacheManagerWithCacheRegion(
    RSDirtyRegionManager& dirtyManager, const std::optional<RectI>& clipRect) const
{
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
