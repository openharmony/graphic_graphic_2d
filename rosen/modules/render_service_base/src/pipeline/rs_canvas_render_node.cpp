/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "pipeline/rs_canvas_render_node.h"

#include <algorithm>
#include "modifier/rs_modifier_type.h"

#include "common/rs_obj_abs_geometry.h"
#include "common/rs_common_def.h"
#include "recording/recording_canvas.h"
#include "memory/rs_memory_track.h"
#include "memory/rs_tag_tracker.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "property/rs_properties_painter.h"
#include "render/rs_blur_filter.h"
#include "render/rs_light_up_effect_filter.h"
#include "platform/common/rs_log.h"
#include "visitor/rs_node_visitor.h"
#include "property/rs_property_drawable.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr PropertyId ANONYMOUS_MODIFIER_ID = 0;
}

RSCanvasRenderNode::RSCanvasRenderNode(NodeId id, const std::weak_ptr<RSContext>& context, bool isTextureExportNode)
    : RSRenderNode(id, context, isTextureExportNode)
{
#ifndef ROSEN_ARKUI_X
    MemoryInfo info = {sizeof(*this), ExtractPid(id), id, MEMORY_TYPE::MEM_RENDER_NODE};
    MemoryTrack::Instance().AddNodeRecord(id, info);
#endif
}

RSCanvasRenderNode::~RSCanvasRenderNode()
{
#ifndef ROSEN_ARKUI_X
    MemoryTrack::Instance().RemoveNodeRecord(GetId());
#endif
}

void RSCanvasRenderNode::UpdateRecording(std::shared_ptr<Drawing::DrawCmdList> drawCmds,
    RSModifierType type, bool isSingleFrameComposer)
{
    if (!drawCmds || drawCmds->IsEmpty()) {
        return;
    }
    auto renderProperty = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>(drawCmds, ANONYMOUS_MODIFIER_ID);
    auto renderModifier = std::make_shared<RSDrawCmdListRenderModifier>(renderProperty);
    renderModifier->SetType(type);
    AddModifier(renderModifier, isSingleFrameComposer);
}

void RSCanvasRenderNode::ClearRecording()
{
    RemoveModifier(ANONYMOUS_MODIFIER_ID);
}

void RSCanvasRenderNode::QuickPrepare(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    if (!visitor) {
        return;
    }
    RSRenderNode::ApplyModifiers();
    visitor->QuickPrepareCanvasRenderNode(*this);  
}

void RSCanvasRenderNode::Prepare(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    if (!visitor) {
        return;
    }
    RSRenderNode::ApplyModifiers();
    visitor->PrepareCanvasRenderNode(*this);
}

void RSCanvasRenderNode::OnTreeStateChanged()
{
    if (!IsOnTheTree()) {
        // clear node groups cache when node is removed from tree
        if (GetCacheType() == CacheType::CONTENT) {
            SetCacheType(CacheType::NONE);
            ClearCacheSurfaceInThread();
            SetDrawingCacheType(RSDrawingCacheType::DISABLED_CACHE);
        }
    }
    RSRenderNode::OnTreeStateChanged();
}

void RSCanvasRenderNode::Process(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    if (!visitor) {
        return;
    }
    RSRenderNode::RenderTraceDebug();
    visitor->ProcessCanvasRenderNode(*this);
}

void RSCanvasRenderNode::ProcessTransitionBeforeChildren(RSPaintFilterCanvas& canvas)
{
    DrawPropertyDrawableRange(RSPropertyDrawableSlot::SAVE_ALL, RSPropertyDrawableSlot::MASK, canvas);
}

void RSCanvasRenderNode::ProcessShadowBatching(RSPaintFilterCanvas& canvas)
{
    RSAutoCanvasRestore acr(&canvas);
    DrawPropertyDrawableRange(RSPropertyDrawableSlot::BOUNDS_MATRIX, RSPropertyDrawableSlot::TRANSITION, canvas);
    DrawPropertyDrawable(RSPropertyDrawableSlot::SHADOW, canvas);
}

void RSCanvasRenderNode::DrawShadow(RSModifierContext& context, RSPaintFilterCanvas& canvas)
{
    ApplyDrawCmdModifier(context, RSModifierType::TRANSITION);
    ApplyDrawCmdModifier(context, RSModifierType::ENV_FOREGROUND_COLOR);

    auto parent = GetParent().lock();
    if (!(parent && parent->GetRenderProperties().GetUseShadowBatching())) {
        RSPropertiesPainter::DrawShadow(GetRenderProperties(), canvas);
        RSPropertiesPainter::DrawOutline(GetRenderProperties(), canvas);
    }
}

void RSCanvasRenderNode::PropertyDrawableRender(RSPaintFilterCanvas& canvas, bool includeProperty)
{
    auto parent = GetParent().lock();
    if (parent &&
        parent->GetRenderProperties().GetUseShadowBatching()) {
        DrawPropertyDrawableRange(
            RSPropertyDrawableSlot::TRANSITION, RSPropertyDrawableSlot::ENV_FOREGROUND_COLOR, canvas);
        if (includeProperty) {
            DrawPropertyDrawableRange(
                RSPropertyDrawableSlot::BG_SAVE_BOUNDS, RSPropertyDrawableSlot::CLIP_TO_FRAME, canvas);
        } else {
            DrawPropertyDrawableRange(
                RSPropertyDrawableSlot::SAVE_FRAME, RSPropertyDrawableSlot::CLIP_TO_FRAME, canvas);
        }
    } else {
        if (includeProperty) {
            DrawPropertyDrawableRange(RSPropertyDrawableSlot::TRANSITION, RSPropertyDrawableSlot::CLIP_TO_FRAME,
                canvas);
        } else {
            DrawPropertyDrawableRange(RSPropertyDrawableSlot::TRANSITION, RSPropertyDrawableSlot::OUTLINE, canvas);
            DrawPropertyDrawableRange(
                RSPropertyDrawableSlot::SAVE_FRAME, RSPropertyDrawableSlot::CLIP_TO_FRAME, canvas);
        }
    }
}

void RSCanvasRenderNode::ProcessAnimatePropertyBeforeChildren(RSPaintFilterCanvas& canvas, bool includeProperty)
{
    PropertyDrawableRender(canvas, includeProperty);
}

void RSCanvasRenderNode::ProcessRenderContents(RSPaintFilterCanvas& canvas)
{
    DrawPropertyDrawable(RSPropertyDrawableSlot::CONTENT_STYLE, canvas);
}

void RSCanvasRenderNode::ProcessRenderBeforeChildren(RSPaintFilterCanvas& canvas)
{
    DrawPropertyDrawableRange(RSPropertyDrawableSlot::SAVE_ALL, RSPropertyDrawableSlot::CLIP_TO_FRAME, canvas);
}

void RSCanvasRenderNode::ProcessAnimatePropertyAfterChildren(RSPaintFilterCanvas& canvas)
{
    DrawPropertyDrawableRange(
        RSPropertyDrawableSlot::FOREGROUND_STYLE, RSPropertyDrawableSlot::PARTICLE_EFFECT, canvas);
}

void RSCanvasRenderNode::ProcessRenderAfterChildren(RSPaintFilterCanvas& canvas)
{
    DrawPropertyDrawableRange(RSPropertyDrawableSlot::FOREGROUND_STYLE, RSPropertyDrawableSlot::RESTORE_ALL, canvas);
}

void RSCanvasRenderNode::ApplyDrawCmdModifier(RSModifierContext& context, RSModifierType type)
{
    // temporary workaround, PLANNING: refactor single frame compose without modifing draw cmd list
    auto& drawCmdModifiers = const_cast<RSRenderContent::DrawCmdContainer&>(GetDrawCmdModifiers());
    auto itr = drawCmdModifiers.find(type);
    if (itr == drawCmdModifiers.end() || itr->second.empty()) {
        return;
    }

    if (RSSystemProperties::GetSingleFrameComposerEnabled()) {
        bool needSkip = false;
        if (GetNodeIsSingleFrameComposer() && singleFrameComposer_ != nullptr) {
            needSkip = singleFrameComposer_->SingleFrameModifierAddToList(type, itr->second);
        }
        for (const auto& modifier : itr->second) {
            if (singleFrameComposer_ != nullptr && singleFrameComposer_->SingleFrameIsNeedSkip(needSkip, modifier)) {
                continue;
            }
            modifier->Apply(context);
        }
    } else {
        for (const auto& modifier : itr->second) {
        }
    }
    ApplyDrawCmdModifier(context, RSModifierType::CONTENT_STYLE);

    // temporary solution for drawing children
    for (auto& child : *GetSortedChildren()) {
        if (auto canvasChild = ReinterpretCast<RSCanvasRenderNode>(child)) {
            canvasChild->InternalDrawContent(canvas);
        }
    }
}
} // namespace Rosen
} // namespace OHOS
