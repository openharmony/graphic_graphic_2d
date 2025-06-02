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
#include "params/rs_render_params.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_surface_render_node.h"
#include "property/rs_properties_painter.h"
#include "render/rs_blur_filter.h"
#include "render/rs_light_up_effect_filter.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
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
    MemoryInfo info = {sizeof(*this), ExtractPid(id), id, 0, MEMORY_TYPE::MEM_RENDER_NODE, ExtractPid(id)};
    MemoryTrack::Instance().AddNodeRecord(id, info);
#endif
    MemorySnapshot::Instance().AddCpuMemory(ExtractPid(id), sizeof(*this));
}

RSCanvasRenderNode::~RSCanvasRenderNode()
{
#ifndef ROSEN_ARKUI_X
    MemoryTrack::Instance().RemoveNodeRecord(GetId());
#endif
    MemorySnapshot::Instance().RemoveCpuMemory(ExtractPid(GetId()), sizeof(*this));
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
    ApplyModifiers();
    visitor->QuickPrepareCanvasRenderNode(*this);
}

void RSCanvasRenderNode::Prepare(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    if (!visitor) {
        return;
    }
    ApplyModifiers();
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
        needClearSurface_ = true;
        AddToPendingSyncList();
    }
    RSRenderNode::OnTreeStateChanged();

    // When the P3 canvasNode is up or down the tree, it transmits color gamut information to appWindow node.
    if (GetIsWideColorGamut()) {
        ModifyWideWindowColorGamutNum(IsOnTheTree());
    }
}

bool RSCanvasRenderNode::OpincGetNodeSupportFlag()
{
    const auto& property = GetRenderProperties();
    if (GetSharedTransitionParam() ||
        property.IsSpherizeValid() ||
        property.IsAttractionValid() ||
        property.NeedFilter() ||
        property.GetUseEffect() ||
        property.GetColorBlend().has_value() ||
        (IsSelfDrawingNode() && GetOpincCache().OpincGetRootFlag())) {
        return false;
    }
    return true && RSRenderNode::OpincGetNodeSupportFlag();
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
            // Just need to skip RSPropertyDrawableSlot::SHADOW
            DrawPropertyDrawableRange(
                RSPropertyDrawableSlot::FOREGROUND_FILTER, RSPropertyDrawableSlot::CLIP_TO_FRAME, canvas);
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
    auto parent = GetParent().lock();
    if (parent &&
        parent->GetRenderProperties().GetUseShadowBatching()) {
        DrawPropertyDrawableRange(
            RSPropertyDrawableSlot::SAVE_ALL, RSPropertyDrawableSlot::ENV_FOREGROUND_COLOR, canvas);
        // Just need to skip RSPropertyDrawableSlot::SHADOW
        DrawPropertyDrawableRange(
            RSPropertyDrawableSlot::FOREGROUND_FILTER, RSPropertyDrawableSlot::CUSTOM_CLIP_TO_FRAME, canvas);
    } else {
        DrawPropertyDrawableRange(
            RSPropertyDrawableSlot::SAVE_ALL, RSPropertyDrawableSlot::CUSTOM_CLIP_TO_FRAME, canvas);
    }
}

void RSCanvasRenderNode::ProcessAnimatePropertyAfterChildren(RSPaintFilterCanvas& canvas)
{
    DrawPropertyDrawableRange(
        RSPropertyDrawableSlot::FOREGROUND_STYLE, RSPropertyDrawableSlot::PARTICLE_EFFECT, canvas);
}

void RSCanvasRenderNode::ProcessTransitionAfterChildren(RSPaintFilterCanvas& canvas)
{
    DrawPropertyDrawableRange(RSPropertyDrawableSlot::PIXEL_STRETCH, RSPropertyDrawableSlot::RESTORE_ALL, canvas);
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
            modifier->Apply(context);
        }
    }
}

void RSCanvasRenderNode::InternalDrawContent(RSPaintFilterCanvas& canvas, bool needApplyMatrix)
{
    RSModifierContext context = { GetMutableRenderProperties(), &canvas };

    if (needApplyMatrix) {
        DrawPropertyDrawableRange(RSPropertyDrawableSlot::SAVE_ALL, RSPropertyDrawableSlot::CONTENT_STYLE, canvas);
    } else {
        DrawPropertyDrawableRange(RSPropertyDrawableSlot::OUTLINE, RSPropertyDrawableSlot::CONTENT_STYLE, canvas);
    }

    for (auto& child : *GetSortedChildren()) {
        if (auto canvasChild = ReinterpretCast<RSCanvasRenderNode>(child)) {
            canvasChild->InternalDrawContent(canvas, true);
        }
    }

    if (needApplyMatrix) {
        DrawPropertyDrawableRange(RSPropertyDrawableSlot::FOREGROUND_STYLE, RSPropertyDrawableSlot::RESTORE_ALL,
            canvas);
    } else {
        DrawPropertyDrawableRange(RSPropertyDrawableSlot::FOREGROUND_STYLE, RSPropertyDrawableSlot::PIXEL_STRETCH,
            canvas);
    }
}

// When the HDR node status changed, update the node list in the ancestor display node.
// Support to add animation on canvas nodes when display node is forced to close HDR.
void RSCanvasRenderNode::UpdateDisplayHDRNodeList(bool flag, NodeId displayNodeId) const
{
    auto context = GetContext().lock();
    if (!context) {
        ROSEN_LOGE("RSCanvasRenderNode::UpdateDisplayHDRNodeList Invalid context");
        return;
    }
    auto displayNode = context->GetNodeMap().GetRenderNode<RSDisplayRenderNode>(displayNodeId);
    if (!displayNode) {
        ROSEN_LOGE("RSCanvasRenderNode::UpdateDisplayHDRNodeList Invalid displayNode");
        return;
    }
    if (flag) {
        displayNode->InsertHDRNode(GetId());
    } else {
        displayNode->RemoveHDRNode(GetId());
    }
}

void RSCanvasRenderNode::SetHDRPresent(bool hasHdrPresent)
{
    if (hasHdrPresent_ == hasHdrPresent) {
        return;
    }
    if (IsOnTheTree()) {
        SetHdrNum(hasHdrPresent, GetInstanceRootNodeId(), HDRComponentType::IMAGE);
        UpdateDisplayHDRNodeList(hasHdrPresent, GetDisplayNodeId());
    }
    hasHdrPresent_ = hasHdrPresent;
}

bool RSCanvasRenderNode::GetHDRPresent() const
{
    return hasHdrPresent_;
}

void RSCanvasRenderNode::SetIsWideColorGamut(bool isWideColorGamut)
{
    if (isWideColorGamut_ == isWideColorGamut) {
        return;
    }
    if (IsOnTheTree()) {
        ModifyWideWindowColorGamutNum(isWideColorGamut_);
    }
    isWideColorGamut_ = isWideColorGamut;
}

bool RSCanvasRenderNode::GetIsWideColorGamut() const
{
    return isWideColorGamut_;
}

void RSCanvasRenderNode::ModifyWideWindowColorGamutNum(bool flag)
{
    auto parentInstance = GetInstanceRootNode();
    if (!parentInstance) {
        RS_LOGE("RSCanvasRenderNode::ModifyWideWindowColorGamutNum get instanceRootNode failed.");
        return;
    }
    if (auto parentSurface = parentInstance->ReinterpretCastTo<RSSurfaceRenderNode>()) {
        if (flag) {
            parentSurface->IncreaseWideColorGamutNum();
        } else {
            parentSurface->ReduceWideColorGamutNum();
        }
    }
}

// [Attention] Only used in PC window resize scene now
void RSCanvasRenderNode::SetLinkedRootNodeId(NodeId rootNodeId)
{
    if (!RSSystemProperties::GetWindowKeyFrameEnabled()) {
        RS_LOGW("RSCanvasRenderNode::SetLinkedRootNodeId WindowKeyFrame feature disabled");
        return;
    }

    linkedRootNodeId_ = rootNodeId;
}

// [Attention] Only used in PC window resize scene now
NodeId RSCanvasRenderNode::GetLinkedRootNodeId() const
{
    return linkedRootNodeId_;
}

} // namespace Rosen
} // namespace OHOS
