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
#include "common/rs_optional_trace.h"

namespace OHOS {
namespace Rosen {

RSEffectRenderNode::RSEffectRenderNode(NodeId id, const std::weak_ptr<RSContext>& context, bool isTextureExportNode)
    : RSRenderNode(id, context, isTextureExportNode)
{
#ifndef ROSEN_ARKUI_X
    MemoryInfo info = { sizeof(*this), ExtractPid(id), id, MEMORY_TYPE::MEM_RENDER_NODE };
    MemoryTrack::Instance().AddNodeRecord(id, info);
#endif
}

RSEffectRenderNode::~RSEffectRenderNode()
{
#ifndef ROSEN_ARKUI_X
    MemoryTrack::Instance().RemoveNodeRecord(GetId());
#endif
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
    if (!properties.GetHaveEffectRegion() || properties.GetBackgroundFilter() == nullptr ||
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
    if (!GetRenderProperties().GetHaveEffectRegion()) {
        return {};
    }
    return RSRenderNode::GetFilterRect();
}

#ifndef USE_ROSEN_DRAWING
void RSEffectRenderNode::SetEffectRegion(const std::optional<SkIRect>& effectRegion)
{
    if (!effectRegion.has_value() || effectRegion->isEmpty()) {
        ROSEN_LOGD("RSEffectRenderNode::SetEffectRegion: no effect region.");
        GetMutableRenderProperties().SetHaveEffectRegion(false);
        return;
    }

    const auto& properties = GetRenderProperties();
    const auto& absRect = properties.GetBoundsGeometry()->GetAbsRect();
    if (!SkIRect::Intersects(*effectRegion,
        SkIRect::MakeLTRB(absRect.GetLeft(), absRect.GetTop(), absRect.GetRight(), absRect.GetBottom()))) {
        ROSEN_LOGD("RSEffectRenderNode::SetEffectRegion: effect region is not in node.");
        GetMutableRenderProperties().SetHaveEffectRegion(false);
        return;
    }
    GetMutableRenderProperties().SetHaveEffectRegion(true);
}
#else
void RSEffectRenderNode::SetEffectRegion(const std::optional<Drawing::RectI>& effectRegion)
{
    if (!effectRegion.has_value() || !effectRegion->IsValid()) {
        ROSEN_LOGD("RSEffectRenderNode::SetEffectRegion: no effect region.");
        GetMutableRenderProperties().SetHaveEffectRegion(false);
        return;
    }

    const auto& properties = GetRenderProperties();
    const auto& absRect = properties.GetBoundsGeometry()->GetAbsRect();
    Drawing::RectI effectRect = effectRegion.value();
    if (!effectRect.Intersect(
        Drawing::RectI(absRect.GetLeft(), absRect.GetTop(), absRect.GetRight(), absRect.GetBottom()))) {
        ROSEN_LOGD("RSEffectRenderNode::SetEffectRegion: no valid effect region.");
        GetMutableRenderProperties().SetHaveEffectRegion(false);
        return;
    }
    GetMutableRenderProperties().SetHaveEffectRegion(true);
}
#endif

void RSEffectRenderNode::UpdateFilterCacheManagerWithCacheRegion(
    RSDirtyRegionManager& dirtyManager, const std::optional<RectI>& clipRect)
{
    if (NeedForceCache()) {
        return;
    }

    // We need to check cache validity by comparing the cached image region with the filter rect
    // PLANNING: the last != condition should be reconsidered
    auto& manager = GetRenderProperties().GetFilterCacheManager(false);
    if (!manager) {
        return;
    }
    if (manager->IsCacheValid() &&
        (manager->GetCachedImageRegion() != GetFilterRect() || preRotationStatus_ != isRotationChanged_)) {
        // If the cached image region is different from the filter rect, invalidate the cache
        manager->InvalidateCache();
    }
    // If the effectnode filter cache is invalid and there is no visited filter cache for occlusion
    // Invalid this surface's filter cache occlusion
    if (!manager->IsCacheValid() && IsVisitedFilterCacheEmpty()) {
        ROSEN_LOGD("RSEffectRenderNode::UpdateFilterCacheManagerWithCacheRegion: InvalidateFilterCacheRect.");
        dirtyManager.InvalidateFilterCacheRect();
    }
}

void RSEffectRenderNode::UpdateFilterCacheWithDirty(RSDirtyRegionManager& dirtyManager, bool isForeground)
{
    // No need to invalidate cache if background image is not null
    if (NeedForceCache()) {
        return;
    }
    RSRenderNode::UpdateFilterCacheWithDirty(dirtyManager, isForeground);
}

bool RSEffectRenderNode::NeedForceCache()
{
    // force update the first frame and last frame when rotating.
    if (preRotationStatus_ != isRotationChanged_) {
        return false;
    }
    // No need to invalidate cache if background image is not null or freezed
    if (GetRenderProperties().GetBgImage() || (IsStaticCached() && !isRotationChanged_)) {
        RS_OPTIONAL_TRACE_NAME("RSEffectRenderNode: background is not null or freezed");
        return true;
    }
    if (isRotationChanged_ && invalidateTimes_ > 0) {
        --invalidateTimes_;
        return true;
    }
    if (isRotationChanged_ && invalidateTimes_ == 0 && cacheUpdateInterval_ > 0) {
        --cacheUpdateInterval_;
        return true;
    }
    cacheUpdateInterval_ = 1; // cache one frame
    return false;
}

void RSEffectRenderNode::SetInvalidateTimesForRotation(int times)
{
    invalidateTimes_ = times;
}

void RSEffectRenderNode::SetRotationChanged(bool isRotationChanged)
{
    preRotationStatus_ = isRotationChanged_;
    isRotationChanged_ = isRotationChanged;
}

bool RSEffectRenderNode::GetRotationChanged() const
{
    return isRotationChanged_;
}
} // namespace Rosen
} // namespace OHOS
