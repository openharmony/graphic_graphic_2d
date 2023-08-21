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

#include "common/rs_obj_abs_geometry.h"
#include "memory/rs_memory_track.h"
#include "platform/common/rs_log.h"
#include "property/rs_properties_painter.h"
#include "render/rs_skia_filter.h"
#include "visitor/rs_node_visitor.h"

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
    if (effectRegion_.has_value() && !effectRegion_->isEmpty() && properties.GetBackgroundFilter() != nullptr &&
        canvas.GetCacheType() != RSPaintFilterCanvas::CacheType::OFFSCREEN) {
        RSPropertiesPainter::DrawBackgroundEffect(properties, canvas, effectRegion_->getBounds());
    }
#endif
}

RectI RSEffectRenderNode::GetFilterRect() const
{
#ifndef USE_ROSEN_DRAWING
    if (effectRegion_.has_value()) {
        auto& matrix = GetRenderProperties().GetBoundsGeometry()->GetAbsMatrix();
        auto bounds = effectRegion_->makeTransform(matrix).getBounds();
        return {bounds.x(), bounds.y(), bounds.width(), bounds.height()};
    }
#endif
    return {};
}

void RSEffectRenderNode::SetEffectRegion(const std::optional<SkPath>& region)
{
    if (!region.has_value()) {
        effectRegion_.reset();
        return;
    }
    auto matrix = GetRenderProperties().GetBoundsGeometry()->GetAbsMatrix();
    SkMatrix revertMatrix;
    // Map absolute matrix to local matrix
    if (matrix.invert(&revertMatrix)) {
        effectRegion_ = region.value().makeTransform(revertMatrix);
    }
}
} // namespace Rosen
} // namespace OHOS
