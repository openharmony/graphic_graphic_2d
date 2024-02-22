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
#ifndef USE_ROSEN_DRAWING
#include "include/core/SkCanvas.h"
#else
#include "recording/recording_canvas.h"
#endif
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

#ifndef USE_ROSEN_DRAWING
void RSCanvasRenderNode::UpdateRecording(std::shared_ptr<DrawCmdList> drawCmds,
    RSModifierType type, bool isSingleFrameComposer)
{
    if (!drawCmds || drawCmds->GetSize() == 0) {
        return;
    }
    auto renderProperty = std::make_shared<RSRenderProperty<DrawCmdListPtr>>(drawCmds, ANONYMOUS_MODIFIER_ID);
    auto renderModifier = std::make_shared<RSDrawCmdListRenderModifier>(renderProperty);
    renderModifier->SetType(type);
    AddModifier(renderModifier, isSingleFrameComposer);
}
#else
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
#endif

void RSCanvasRenderNode::ClearRecording()
{
    RemoveModifier(ANONYMOUS_MODIFIER_ID);
}

void RSCanvasRenderNode::Prepare(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    if (!visitor) {
        return;
    }
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
    std::unique_lock<std::mutex> lock(canvasNodeProcessMutex_);
    if (!visitor) {
        return;
    }
    RSRenderNode::RenderTraceDebug();
    visitor->ProcessCanvasRenderNode(*this);
}

void RSCanvasRenderNode::ProcessTransitionBeforeChildren(RSPaintFilterCanvas& canvas)
{
    if (RSSystemProperties::GetPropertyDrawableEnable()) {
        DrawPropertyDrawableRange(RSPropertyDrawableSlot::SAVE_ALL, RSPropertyDrawableSlot::MASK, canvas);
        return;
    }
    RSRenderNode::ProcessTransitionBeforeChildren(canvas);
}

void RSCanvasRenderNode::ProcessShadowBatching(RSPaintFilterCanvas& canvas)
{
    RSAutoCanvasRestore acr(&canvas);
    if (RSSystemProperties::GetPropertyDrawableEnable()) {
        DrawPropertyDrawableRange(
            RSPropertyDrawableSlot::BOUNDS_MATRIX, RSPropertyDrawableSlot::TRANSITION, canvas);
        DrawPropertyDrawable(
            RSPropertyDrawableSlot::SHADOW, canvas);
        return;
    }
    RSModifierContext context = { GetMutableRenderProperties(), &canvas };
    ApplyBoundsGeometry(canvas);
    ApplyAlpha(canvas);
    RSPropertiesPainter::DrawMask(GetRenderProperties(), canvas);
    RSPropertiesPainter::DrawShadow(GetRenderProperties(), canvas);
    RSPropertiesPainter::DrawOutline(GetRenderProperties(), canvas);
}

void RSCanvasRenderNode::DrawShadow(RSModifierContext& context, RSPaintFilterCanvas& canvas)
{
    ApplyDrawCmdModifier(context, RSModifierType::TRANSITION);
    ApplyDrawCmdModifier(context, RSModifierType::ENV_FOREGROUND_COLOR);
    
    if (RSSystemProperties::GetUseShadowBatchingEnabled()) {
        auto parent = GetParent().lock();
        if (!(parent && parent->GetRenderProperties().GetUseShadowBatching())) {
            RSPropertiesPainter::DrawShadow(GetRenderProperties(), canvas);
            RSPropertiesPainter::DrawOutline(GetRenderProperties(), canvas);
        }
    } else {
        RSPropertiesPainter::DrawShadow(GetRenderProperties(), canvas);
        RSPropertiesPainter::DrawOutline(GetRenderProperties(), canvas);
    }
}

void RSCanvasRenderNode::PropertyDrawableRender(RSPaintFilterCanvas& canvas, bool includeProperty)
{
    auto parent = GetParent().lock();
    if (RSSystemProperties::GetUseShadowBatchingEnabled() && parent &&
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
    if (RSSystemProperties::GetPropertyDrawableEnable()) {
        PropertyDrawableRender(canvas, includeProperty);
        return;
    }
    RSModifierContext context = { GetMutableRenderProperties(), &canvas };
    DrawShadow(context, canvas);

    // BlendMode-Begin
    RSPropertiesPainter::BeginBlendMode(canvas, context.properties_);

    // In NEW_SKIA version, L96 code will cause dump if the 3rd parameter is true.
#ifdef NEW_SKIA
    RSPropertiesPainter::DrawBackground(GetRenderProperties(), canvas, false);
#else
    RSPropertiesPainter::DrawBackground(GetRenderProperties(), canvas);
#endif

    if (canvas.GetCacheType() != RSPaintFilterCanvas::CacheType::OFFSCREEN) {
        if (GetRenderProperties().GetUseEffect()) {
            RSPropertiesPainter::ApplyBackgroundEffect(GetRenderProperties(), canvas);
        }
        RSPropertiesPainter::DrawFilter(GetRenderProperties(), canvas, FilterType::BACKGROUND_FILTER);
    }

    ApplyDrawCmdModifier(context, RSModifierType::BACKGROUND_STYLE);

    if (GetRenderProperties().IsDynamicLightUpValid()) {
        RSPropertiesPainter::DrawDynamicLightUp(GetRenderProperties(), canvas);
    }

#ifndef USE_ROSEN_DRAWING
    canvasNodeSaveCount_ = canvas.Save();
#else
    canvasNodeSaveCount_ = canvas.SaveAllStatus();
#endif
    ApplyDrawCmdModifier(context, RSModifierType::ENV_FOREGROUND_COLOR_STRATEGY);
#ifndef USE_ROSEN_DRAWING
    canvas.translate(GetRenderProperties().GetFrameOffsetX(), GetRenderProperties().GetFrameOffsetY());
#else
    canvas.Translate(GetRenderProperties().GetFrameOffsetX(), GetRenderProperties().GetFrameOffsetY());
#endif

    if (GetRenderProperties().GetClipToFrame()) {
    // In NEW_SKIA version, L116 code will cause dump if the 3rd parameter is true.
#ifdef NEW_SKIA
        RSPropertiesPainter::Clip(canvas, GetRenderProperties().GetFrameRect(), false);
#else
        RSPropertiesPainter::Clip(canvas, GetRenderProperties().GetFrameRect());
#endif
    }
}

void RSCanvasRenderNode::ProcessRenderContents(RSPaintFilterCanvas& canvas)
{
    if (RSSystemProperties::GetPropertyDrawableEnable()) {
        DrawPropertyDrawable(RSPropertyDrawableSlot::CONTENT_STYLE, canvas);
        return;
    }
    RSModifierContext context = { GetMutableRenderProperties(), &canvas };
    ApplyDrawCmdModifier(context, RSModifierType::CONTENT_STYLE);
}

void RSCanvasRenderNode::ProcessRenderBeforeChildren(RSPaintFilterCanvas& canvas)
{
    if (RSSystemProperties::GetPropertyDrawableEnable()) {
        DrawPropertyDrawableRange(RSPropertyDrawableSlot::SAVE_ALL, RSPropertyDrawableSlot::CLIP_TO_FRAME, canvas);
        return;
    }
    ProcessTransitionBeforeChildren(canvas);
    ProcessAnimatePropertyBeforeChildren(canvas, true);
}

void RSCanvasRenderNode::ProcessAnimatePropertyAfterChildren(RSPaintFilterCanvas& canvas)
{
    if (RSSystemProperties::GetPropertyDrawableEnable()) {
        DrawPropertyDrawableRange(
            RSPropertyDrawableSlot::FOREGROUND_STYLE, RSPropertyDrawableSlot::PARTICLE_EFFECT, canvas);
        return;
    }
    RSModifierContext context = { GetMutableRenderProperties(), &canvas };
    ApplyDrawCmdModifier(context, RSModifierType::FOREGROUND_STYLE);

    auto& aiInvert = GetRenderProperties().GetAiInvert();
    if (aiInvert.has_value()) {
        RSPropertiesPainter::DrawBinarizationShader(GetRenderProperties(), canvas);
    }
    RSPropertiesPainter::DrawColorFilter(GetRenderProperties(), canvas);

    canvas.RestoreStatus(canvasNodeSaveCount_);

    if (GetRenderProperties().IsLightUpEffectValid()) {
        RSPropertiesPainter::DrawLightUpEffect(GetRenderProperties(), canvas);
    }
    RSPropertiesPainter::DrawFilter(GetRenderProperties(), canvas, FilterType::FOREGROUND_FILTER);

    auto illuminatedPtr_ = GetRenderProperties().GetIlluminated();
    if (illuminatedPtr_ && illuminatedPtr_->IsIlluminated()) {
        RSPropertiesPainter::DrawLight(GetRenderProperties(), canvas);
    }
    RSPropertiesPainter::DrawOutline(GetRenderProperties(), canvas);
    RSPropertiesPainter::DrawBorder(GetRenderProperties(), canvas);
    ApplyDrawCmdModifier(context, RSModifierType::OVERLAY_STYLE);
    RSPropertiesPainter::DrawForegroundColor(GetRenderProperties(), canvas);
    RSPropertiesPainter::DrawParticle(GetRenderProperties(), canvas);

    // BlendMode-End
    RSPropertiesPainter::EndBlendMode(canvas, context.properties_);
}

void RSCanvasRenderNode::ProcessTransitionAfterChildren(RSPaintFilterCanvas& canvas)
{
    if (RSSystemProperties::GetPropertyDrawableEnable()) {
        DrawPropertyDrawableRange(RSPropertyDrawableSlot::PIXEL_STRETCH, RSPropertyDrawableSlot::RESTORE_ALL, canvas);
        return;
    }
    RSPropertiesPainter::DrawPixelStretch(GetRenderProperties(), canvas);
    RSRenderNode::ProcessRenderAfterChildren(canvas);
}

void RSCanvasRenderNode::ProcessRenderAfterChildren(RSPaintFilterCanvas& canvas)
{
    if (RSSystemProperties::GetPropertyDrawableEnable()) {
        DrawPropertyDrawableRange(
            RSPropertyDrawableSlot::FOREGROUND_STYLE, RSPropertyDrawableSlot::RESTORE_ALL, canvas);
        return;
    }
    ProcessAnimatePropertyAfterChildren(canvas);
    ProcessTransitionAfterChildren(canvas);
    canvas.RestoreEnv();
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
            modifier->Apply(context);
        }
    }
}

void RSCanvasRenderNode::InternalDrawContent(RSPaintFilterCanvas& canvas)
{
    RSModifierContext context = { GetMutableRenderProperties(), &canvas };
#ifndef USE_ROSEN_DRAWING
    canvas.translate(GetRenderProperties().GetFrameOffsetX(), GetRenderProperties().GetFrameOffsetY());
#else
    canvas.Translate(GetRenderProperties().GetFrameOffsetX(), GetRenderProperties().GetFrameOffsetY());
#endif

    if (GetRenderProperties().GetClipToFrame()) {
        RSPropertiesPainter::Clip(canvas, GetRenderProperties().GetFrameRect());
    }
    ApplyDrawCmdModifier(context, RSModifierType::CONTENT_STYLE);

    // temporary solution for drawing children
    for (auto& child : *GetSortedChildren()) {
        if (auto canvasChild = ReinterpretCast<RSCanvasRenderNode>(child)) {
            canvasChild->InternalDrawContent(canvas);
        }
    }
}

void RSCanvasRenderNode::ProcessDrivenBackgroundRender(RSPaintFilterCanvas& canvas)
{
#if defined(RS_ENABLE_DRIVEN_RENDER)
    RSRenderNode::ProcessRenderBeforeChildren(canvas);
    RSModifierContext context = { GetMutableRenderProperties(), &canvas };
    ApplyDrawCmdModifier(context, RSModifierType::TRANSITION);
    ApplyDrawCmdModifier(context, RSModifierType::ENV_FOREGROUND_COLOR);

    RSPropertiesPainter::DrawShadow(GetRenderProperties(), canvas);
    RSPropertiesPainter::DrawOutline(GetRenderProperties(), canvas);
    RSPropertiesPainter::DrawBackground(GetRenderProperties(), canvas);
    RSPropertiesPainter::DrawFilter(GetRenderProperties(), canvas, FilterType::BACKGROUND_FILTER);
    ApplyDrawCmdModifier(context, RSModifierType::BACKGROUND_STYLE);
    RSRenderNode::ProcessRenderAfterChildren(canvas);
#endif
}

void RSCanvasRenderNode::ProcessDrivenContentRender(RSPaintFilterCanvas& canvas)
{
#if defined(RS_ENABLE_DRIVEN_RENDER)
#ifndef USE_ROSEN_DRAWING
    canvasNodeSaveCount_ = canvas.Save();
    canvas.translate(GetRenderProperties().GetFrameOffsetX(), GetRenderProperties().GetFrameOffsetY());
#else
    canvasNodeSaveCount_ = canvas.SaveAllStatus();
    canvas.Translate(GetRenderProperties().GetFrameOffsetX(), GetRenderProperties().GetFrameOffsetY());
#endif
    DrawDrivenContent(canvas);
#endif
}

void RSCanvasRenderNode::ProcessDrivenContentRenderAfterChildren(RSPaintFilterCanvas& canvas)
{
#if defined(RS_ENABLE_DRIVEN_RENDER)
    // Unresolvable bug: Driven render do not support DrawFilter/DrawBorder/FOREGROUND_STYLE/OVERLAY_STYLE
    RSModifierContext context = { GetMutableRenderProperties(), &canvas };
    ApplyDrawCmdModifier(context, RSModifierType::FOREGROUND_STYLE);

    canvas.RestoreStatus(canvasNodeSaveCount_);
#endif
}

RectF RSCanvasRenderNode::GetDrivenContentClipFrameRect() const
{
#if defined(RS_ENABLE_DRIVEN_RENDER)
    // temporary solution for driven content clip
    RectF rect;
    auto itr = GetDrawCmdModifiers().find(RSModifierType::CONTENT_STYLE);
    if (itr == GetDrawCmdModifiers().end() || itr->second.empty()) {
        return rect;
    }
    if (!itr->second.empty()) {
        auto drawCmdModifier = std::static_pointer_cast<RSDrawCmdListRenderModifier>(itr->second.front());
        if (drawCmdModifier != nullptr) {
            rect = drawCmdModifier->GetCmdsClipRect();
        }
    }
    return rect;
#else
    return RectF { 0.0f, 0.0f, 0.0f, 0.0f };
#endif
}

void RSCanvasRenderNode::DrawDrivenContent(RSPaintFilterCanvas& canvas)
{
#if defined(RS_ENABLE_DRIVEN_RENDER)
    RSModifierContext context = { GetMutableRenderProperties(), &canvas };
    auto itr = GetDrawCmdModifiers().find(RSModifierType::CONTENT_STYLE);
    if (itr == GetDrawCmdModifiers().end() || itr->second.empty()) {
        return;
    }
    int32_t index = 0;
    for (const auto& modifier : itr->second) {
        if (index == 0) {
            // temporary solution for driven content clip
            auto drawCmdModifier = std::static_pointer_cast<RSDrawCmdListRenderModifier>(modifier);
            if (drawCmdModifier != nullptr) {
                drawCmdModifier->ApplyForDrivenContent(context);
                index++;
                continue;
            }
        }
        if (modifier != nullptr) {
            modifier->Apply(context);
        }
        index++;
    }
#endif
}
} // namespace Rosen
} // namespace OHOS
