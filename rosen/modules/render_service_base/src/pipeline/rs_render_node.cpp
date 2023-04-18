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

#include "pipeline/rs_render_node.h"

#include <algorithm>

#include "animation/rs_render_animation.h"
#include "common/rs_obj_abs_geometry.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"
#include "property/rs_property_trace.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "property/rs_properties_painter.h"

namespace OHOS {
namespace Rosen {
RSRenderNode::RSRenderNode(NodeId id, std::weak_ptr<RSContext> context) : RSBaseRenderNode(id, context) {}

RSRenderNode::~RSRenderNode()
{
    if (fallbackAnimationOnDestroy_) {
        FallbackAnimationsToRoot();
    }
}

void RSRenderNode::FallbackAnimationsToRoot()
{
    if (animationManager_.animations_.empty()) {
        return;
    }

    auto context = GetContext().lock();
    if (!context) {
        ROSEN_LOGE("Invalid context");
        return;
    }
    auto target = context->GetNodeMap().GetAnimationFallbackNode();
    if (!target) {
        ROSEN_LOGE("Failed to move animation to root, root render node is null!");
        return;
    }
    context->RegisterAnimatingRenderNode(target);

    for (const auto& [animationId, animation] : animationManager_.animations_) {
        animation->Detach();
        // avoid infinite loop for fallback animation
        animation->SetRepeatCount(1);
        target->animationManager_.AddAnimation(animation);
    }
}

std::pair<bool, bool> RSRenderNode::Animate(int64_t timestamp)
{
    return animationManager_.Animate(timestamp, IsOnTheTree());
}

bool RSRenderNode::Update(
    RSDirtyRegionManager& dirtyManager, const RSProperties* parent, bool parentDirty, RectI clipRect)
{
    return Update(dirtyManager, parent, parentDirty, true, clipRect);
}

bool RSRenderNode::Update(RSDirtyRegionManager& dirtyManager, const RSProperties* parent, bool parentDirty)
{
    RectI clipRect{0, 0, 0, 0};
    return Update(dirtyManager, parent, parentDirty, false, clipRect);
}

bool RSRenderNode::Update(
    RSDirtyRegionManager& dirtyManager, const RSProperties* parent, bool parentDirty, bool needClip, RectI clipRect)
{
    // no need to update invisible nodes
    if (!ShouldPaint() && !isLastVisible_) {
        return false;
    }
    // [planning] surfaceNode use frame instead
    std::optional<SkPoint> offset;
    if (parent != nullptr && !IsInstanceOf<RSSurfaceRenderNode>()) {
        offset = SkPoint { parent->GetFrameOffsetX(), parent->GetFrameOffsetY() };
    }
    bool dirty = renderProperties_.UpdateGeometry(parent, parentDirty, offset, GetContextClipRegion());
    if ((IsDirty() || dirty) && drawCmdModifiers_.count(RSModifierType::GEOMETRYTRANS)) {
        RSModifierContext context = { GetMutableRenderProperties() };
        for (auto& modifier : drawCmdModifiers_[RSModifierType::GEOMETRYTRANS]) {
            modifier->Apply(context);
        }
    }
    isDirtyRegionUpdated_ = false;
    UpdateDirtyRegion(dirtyManager, dirty, needClip, clipRect);
    isLastVisible_ = ShouldPaint();
    renderProperties_.ResetDirty();
    return dirty;
}

RSProperties& RSRenderNode::GetMutableRenderProperties()
{
    return renderProperties_;
}

const RSProperties& RSRenderNode::GetRenderProperties() const
{
    return renderProperties_;
}

void RSRenderNode::UpdateDirtyRegion(
    RSDirtyRegionManager& dirtyManager, bool geoDirty, bool needClip, RectI clipRect)
{
    if (!IsDirty() && !geoDirty) {
        return;
    }
    if (!oldDirty_.IsEmpty()) {
        dirtyManager.MergeDirtyRect(oldDirty_);
    }
    // merge old dirty if switch to invisible
    if (!ShouldPaint() && isLastVisible_) {
        ROSEN_LOGD("RSRenderNode:: id %" PRIu64 " UpdateDirtyRegion visible->invisible", GetId());
    } else {
        RectI overlayRect;
        RectI shadowDirty;
        auto dirtyRect = renderProperties_.GetDirtyRect(overlayRect);
        if (renderProperties_.IsShadowValid()) {
            SetShadowValidLastFrame(true);
            if (IsInstanceOf<RSSurfaceRenderNode>()) {
                const RectF absBounds = {0, 0, renderProperties_.GetBoundsWidth(), renderProperties_.GetBoundsHeight()};
                RRect absClipRRect = RRect(absBounds, renderProperties_.GetCornerRadius());
                RSPropertiesPainter::GetShadowDirtyRect(shadowDirty, renderProperties_, &absClipRRect);
            } else {
                RSPropertiesPainter::GetShadowDirtyRect(shadowDirty, renderProperties_);
            }
            if (!shadowDirty.IsEmpty()) {
                dirtyRect = dirtyRect.JoinRect(shadowDirty);
            }
        }

        if (renderProperties_.IsPixelStretchValid() || renderProperties_.IsPixelStretchPercentValid()) {
            auto stretchDirtyRect = renderProperties_.GetPixelStretchDirtyRect();
            dirtyRect = dirtyRect.JoinRect(stretchDirtyRect);
        }

        if (needClip) {
            dirtyRect = dirtyRect.IntersectRect(clipRect);
        }
        // filter invalid dirtyrect
        if (!dirtyRect.IsEmpty()) {
            dirtyManager.MergeDirtyRect(dirtyRect);
            isDirtyRegionUpdated_ = true;
            oldDirty_ = dirtyRect;
            oldDirtyInSurface_ = oldDirty_.IntersectRect(dirtyManager.GetSurfaceRect());
            // save types of dirty region of target dirty manager for dfx
            if (dirtyManager.IsTargetForDfx() &&
                (GetType() == RSRenderNodeType::CANVAS_NODE || GetType() == RSRenderNodeType::SURFACE_NODE)) {
                dirtyManager.UpdateDirtyRegionInfoForDfx(
                    GetId(), GetType(), DirtyRegionType::UPDATE_DIRTY_REGION, oldDirtyInSurface_);
                dirtyManager.UpdateDirtyRegionInfoForDfx(
                    GetId(), GetType(), DirtyRegionType::OVERLAY_RECT, overlayRect);
                dirtyManager.UpdateDirtyRegionInfoForDfx(
                    GetId(), GetType(), DirtyRegionType::SHADOW_RECT, shadowDirty);
                dirtyManager.UpdateDirtyRegionInfoForDfx(
                    GetId(), GetType(), DirtyRegionType::PREPARE_CLIP_RECT, clipRect);
            }
        }
    }
    SetClean();
}

bool RSRenderNode::IsDirty() const
{
    return RSBaseRenderNode::IsDirty() || renderProperties_.IsDirty();
}

void RSRenderNode::UpdateRenderStatus(RectI& dirtyRegion, bool isPartialRenderEnabled)
{
    auto dirtyRect = renderProperties_.GetDirtyRect();
    // should judge if there's any child out of parent
    if (!isPartialRenderEnabled || HasChildrenOutOfRect()) {
        isRenderUpdateIgnored_ = false;
    } else if (dirtyRegion.IsEmpty() || dirtyRect.IsEmpty()) {
        isRenderUpdateIgnored_ = true;
    } else {
        RectI intersectRect = dirtyRegion.IntersectRect(dirtyRect);
        isRenderUpdateIgnored_ = intersectRect.IsEmpty();
    }
}

void RSRenderNode::UpdateParentChildrenRect(std::shared_ptr<RSBaseRenderNode> parentNode) const
{
    auto renderParent = RSBaseRenderNode::ReinterpretCast<RSRenderNode>(parentNode);
    if (renderParent) {
        // accumulate current node's all children region(including itself)
        // apply oldDirty_ as node's real region(including overlay and shadow)
        RectI accumulatedRect = GetChildrenRect().JoinRect(oldDirty_);
        renderParent->UpdateChildrenRect(accumulatedRect);
        // check each child is inside of parent
        if (!accumulatedRect.IsInsideOf(renderParent->GetOldDirty())) {
            renderParent->UpdateChildrenOutOfRectFlag(true);
        }
    }
}

void RSRenderNode::RenderTraceDebug() const
{
    if (RSSystemProperties::GetRenderNodeTraceEnabled()) {
        RSPropertyTrace::GetInstance().PropertiesDisplayByTrace(GetId(), GetRenderProperties());
    }
}

void RSRenderNode::ProcessRenderBeforeChildren(RSPaintFilterCanvas& canvas)
{
    renderNodeSaveCount_ = canvas.Save();
    auto boundsGeo = std::static_pointer_cast<RSObjAbsGeometry>(GetRenderProperties().GetBoundsGeometry());
    if (boundsGeo && !boundsGeo->IsEmpty()) {
        canvas.concat(boundsGeo->GetMatrix());
    }
    auto alpha = renderProperties_.GetAlpha();
    if (alpha < 1.f) {
        if ((GetChildrenCount() == 0) || !GetRenderProperties().GetAlphaOffscreen()) {
            canvas.MultiplyAlpha(alpha);
        } else {
            auto rect = RSPropertiesPainter::Rect2SkRect(GetRenderProperties().GetBoundsRect());
            canvas.saveLayerAlpha(&rect, std::clamp(alpha, 0.f, 1.f) * UINT8_MAX);
        }
    }
    RSPropertiesPainter::DrawMask(GetRenderProperties(), canvas);
}

void RSRenderNode::ProcessRenderAfterChildren(RSPaintFilterCanvas& canvas)
{
    GetMutableRenderProperties().ResetBounds();
    canvas.RestoreStatus(renderNodeSaveCount_);
}

void RSRenderNode::CheckCacheType()
{
    auto newCacheType = CacheType::NONE;
    if (GetRenderProperties().IsSpherizeValid()) {
        newCacheType = CacheType::SPHERIZE;
    } else if (isFreeze_) {
        newCacheType = CacheType::FREEZE;
    }
    if (cacheType_ != newCacheType) {
        cacheType_ = newCacheType;
        cacheTypeChanged_ = true;
    }
}

void RSRenderNode::AddModifier(const std::shared_ptr<RSRenderModifier> modifier)
{
    if (!modifier) {
        return;
    }
    if (modifier->GetType() == RSModifierType::BOUNDS || modifier->GetType() == RSModifierType::FRAME) {
        AddGeometryModifier(modifier);
    } else if (modifier->GetType() < RSModifierType::CUSTOM) {
        modifiers_.emplace(modifier->GetPropertyId(), modifier);
    } else {
        drawCmdModifiers_[modifier->GetType()].emplace_back(modifier);
    }
    modifier->GetProperty()->Attach(shared_from_this());
    SetDirty();
}

void RSRenderNode::AddGeometryModifier(const std::shared_ptr<RSRenderModifier> modifier)
{
    // bounds and frame modifiers must be unique
    if (modifier->GetType() == RSModifierType::BOUNDS) {
        if (boundsModifier_ == nullptr) {
            boundsModifier_ = modifier;
        }
        modifiers_.emplace(modifier->GetPropertyId(), boundsModifier_);
    }

    if (modifier->GetType() == RSModifierType::FRAME) {
        if (frameModifier_ == nullptr) {
            frameModifier_ = modifier;
        }
        modifiers_.emplace(modifier->GetPropertyId(), frameModifier_);
    }
}

void RSRenderNode::RemoveModifier(const PropertyId& id)
{
    bool success = modifiers_.erase(id);
    SetDirty();
    if (success) {
        return;
    }
    for (auto& [type, modifiers] : drawCmdModifiers_) {
        modifiers.remove_if([id](const auto& modifier) -> bool {
            return modifier ? modifier->GetPropertyId() == id : true;
        });
        if (type == RSModifierType::OVERLAY_STYLE) {
            UpdateOverlayBounds();
        }
    }
}

void RSRenderNode::ApplyModifiers()
{
    if (!RSBaseRenderNode::IsDirty()) {
        return;
    }
    RSModifierContext context = { GetMutableRenderProperties() };
    context.property_.Reset();
    for (auto& [id, modifier] : modifiers_) {
        if (modifier) {
            modifier->Apply(context);
        }
    }
    OnApplyModifiers();
    UpdateOverlayBounds();
}

void RSRenderNode::UpdateOverlayBounds()
{
    RSModifierContext context = { GetMutableRenderProperties() };
    RectF joinRect = RectF();
    if (GetDrawRegion() != nullptr) {
        joinRect = joinRect.JoinRect(*(GetDrawRegion()));
        context.property_.SetOverlayBounds(std::make_shared<RectF>(joinRect));
        return;
    }
    for (auto& iterator : drawCmdModifiers_) {
        if (iterator.first == RSModifierType::GEOMETRYTRANS) {
            continue;
        }
        for (auto& overlayModifier : iterator.second) {
            auto drawCmdModifier = std::static_pointer_cast<RSDrawCmdListRenderModifier>(overlayModifier);
            if (!drawCmdModifier) {
                continue;
            }
            if (drawCmdModifier->GetOverlayBounds() != nullptr &&
                !drawCmdModifier->GetOverlayBounds()->IsEmpty()) {
                joinRect = joinRect.JoinRect(*(drawCmdModifier->GetOverlayBounds()));
            } else if (drawCmdModifier->GetOverlayBounds() == nullptr) {
                auto recording = std::static_pointer_cast<RSRenderProperty<DrawCmdListPtr>>(
                    drawCmdModifier->GetProperty())->Get();
                auto recordingRect = RectF(0, 0, recording->GetWidth(), recording->GetHeight());
                joinRect = recordingRect.IsEmpty() ? joinRect : joinRect.JoinRect(recordingRect);
            }
        }
    }
    context.property_.SetOverlayBounds(std::make_shared<RectF>(joinRect));
}

std::shared_ptr<RSRenderModifier> RSRenderNode::GetModifier(const PropertyId& id)
{
    if (modifiers_.count(id)) {
        return modifiers_[id];
    }
    for (auto& [type, modifiers] : drawCmdModifiers_) {
        auto it = std::find_if(modifiers.begin(), modifiers.end(),
            [id](const auto& modifier) -> bool { return modifier->GetPropertyId() == id; });
        if (it != modifiers.end()) {
            return *it;
        }
    }
    return nullptr;
}

void RSRenderNode::FilterModifiersByPid(pid_t pid)
{
    // remove all modifiers added by given pid (by matching higher 32 bits of node id)
    EraseIf(modifiers_, [pid](const auto& pair) -> bool { return ExtractPid(pair.first) == pid; });

    // remove all modifiers added by given pid (by matching higher 32 bits of node id)
    for (auto& [type, modifiers] : drawCmdModifiers_) {
        modifiers.remove_if(
            [pid](const auto& it) -> bool { return ExtractPid(it->GetPropertyId()) == pid; });
    }
}

bool RSRenderNode::ShouldPaint() const
{
    // node should be painted if either it is visible or it has disappearing transition animation, but only when its
    // alpha is not zero
    return (renderProperties_.GetVisible() || HasDisappearingTransition(false)) &&
           (renderProperties_.GetAlpha() > 0.0f);
}
} // namespace Rosen
} // namespace OHOS
