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
RSEffectRenderNode::RSEffectRenderNode(NodeId id, std::weak_ptr<RSContext> context)
    : RSRenderNode(id, context)
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
#ifndef USE_ROSEN_DRAWING
    canvas.SaveEffectData();
    auto& properties = GetRenderProperties();
    auto boundsGeo = (properties.GetBoundsGeometry());
    if (boundsGeo && !boundsGeo->IsEmpty()) {
        canvas.concat(boundsGeo->GetMatrix());
    }
    auto alpha = properties.GetAlpha();
    if (alpha < 1.f) {
        if ((GetChildrenCount() == 0) || !(properties.GetAlphaOffscreen() || IsForcedDrawInGroup())) {
            canvas.MultiplyAlpha(alpha);
        } else {
            auto rect = RSPropertiesPainter::Rect2SkRect(properties.GetBoundsRect());
            canvas.saveLayerAlpha(&rect, std::clamp(alpha, 0.f, 1.f) * UINT8_MAX);
        }
    }

    if (effectRegion_.has_value() && !(effectRegion_.value().isEmpty())) {
        if (properties.GetBackgroundFilter() != nullptr) {
            SkPath& path = effectRegion_.value();
            auto effectIRect = path.getBounds().roundOut();
            RSPropertiesPainter::DrawBackgroundEffect(properties, canvas, effectIRect);
        }
        if (properties.GetColorFilter() != nullptr) {
            canvas.SetChildrenPath(effectRegion_.value());
        }
    }
#endif
}

void RSEffectRenderNode::ProcessRenderAfterChildren(RSPaintFilterCanvas& canvas)
{
#ifndef USE_ROSEN_DRAWING
    RSPropertiesPainter::DrawForegroundEffect(GetRenderProperties(), canvas);
    canvas.RestoreEffectData();
#endif
}

RectI RSEffectRenderNode::GetFilterRect() const
{
    if (effectRegion_.has_value()) {
#ifndef USE_ROSEN_DRAWING
        auto bounds = effectRegion_->getBounds();
        return {bounds.x(), bounds.y(), bounds.width(), bounds.height()};
#else
        auto bounds = effectRegion_->GetBounds();
        return {bounds.GetLeft(), bounds.GetTop(), bounds.GetWidth(), bounds.GetHeight()};
#endif
    } else {
        return {};
    }
}

#ifndef USE_ROSEN_DRAWING
void RSEffectRenderNode::SetEffectRegion(const std::optional<SkPath>& region)
#else
void RSEffectRenderNode::SetEffectRegion(const std::optional<Drawing::Path>& region)
#endif
{
    effectRegion_ = region;
}
} // namespace Rosen
} // namespace OHOS
