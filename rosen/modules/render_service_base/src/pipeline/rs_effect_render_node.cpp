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
#include "property/rs_properties_painter.h"
#include "visitor/rs_node_visitor.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSEffectRenderNode::RSEffectRenderNode(NodeId id, const std::weak_ptr<RSContext>& context) : RSRenderNode(id, context)
{
    MemoryInfo info = {sizeof(*this), ExtractPid(id), id, MEMORY_TYPE::MEM_RENDER_NODE};
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
#ifndef USE_ROSEN_DRAWING
    auto& properties = GetRenderProperties();
    // Disable effect region if either of the following conditions is met:
    // 1. Effect region is null or empty
    // 2. Background filter is null
    // 3. Canvas is offscreen
    if (effectRegion_.has_value() && properties.GetBackgroundFilter() != nullptr &&
        canvas.GetCacheType() != RSPaintFilterCanvas::CacheType::OFFSCREEN) {
        RSPropertiesPainter::DrawBackgroundEffect(properties, canvas, *effectRegion_);
    } else {
        canvas.SetEffectData(nullptr);
    }
#else
    // PLANNING: add drawing implementation
#endif
}

RectI RSEffectRenderNode::GetFilterRect() const
{
    if (!effectRegion_.has_value()) {
        ROSEN_LOGE("RSEffectRenderNode::GetFilterRect: effectRegion has no value");
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
void RSEffectRenderNode::SetEffectRegion(const std::optional<SkPath>& region)
#else
void RSEffectRenderNode::SetEffectRegion(const std::optional<Drawing::Path>& region)
#endif
{
    if (!region.has_value()) {
        effectRegion_.reset();
        ROSEN_LOGE("RSEffectRenderNode::SetEffectRegion: region has no value");
        return;
    }
#ifndef USE_ROSEN_DRAWING
    const auto& geoPtr = GetRenderProperties().GetBoundsGeometry();
    const auto& matrix = geoPtr->GetAbsMatrix();
    const auto& absRect = geoPtr->GetAbsRect();
    // intersect effect region with node bounds
    auto rect = region->getBounds();
    if (!rect.intersect(
        SkRect::MakeLTRB(absRect.GetLeft(), absRect.GetTop(), absRect.GetRight(), absRect.GetBottom()))) {
        effectRegion_.reset();
        ROSEN_LOGE("RSEffectRenderNode::SetEffectRegion: intersect rect failed.");
        return;
    }

    // Map absolute rect to local matrix
    SkMatrix revertMatrix;
    if (!matrix.invert(&revertMatrix)) {
        effectRegion_.reset();
        ROSEN_LOGE("RSEffectRenderNode::SetEffectRegion: get invert matrix failed.");
        return;
    }
    auto prevEffectRegion = std::move(effectRegion_);
    effectRegion_ = revertMatrix.mapRect(rect).roundOut();

    // Update cache state if filter region has changed
    auto& manager = GetRenderProperties().GetFilterCacheManager(false);
    if (!manager || !manager->IsCacheValid()) {
        ROSEN_LOGE("RSEffectRenderNode::SetEffectRegion: CacheManager is null or invalid");
        return;
    }
    if (prevEffectRegion.has_value() && !prevEffectRegion->contains(*effectRegion_)) {
        manager->UpdateCacheStateWithFilterRegion();
    }
#else
    // PLANNING: add drawing implementation
#endif
}
} // namespace Rosen
} // namespace OHOS
