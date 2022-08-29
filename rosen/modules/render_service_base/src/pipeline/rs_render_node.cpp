/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include "platform/common/rs_log.h"
#ifdef ROSEN_OHOS
#include "pipeline/rs_paint_filter_canvas.h"
#include "property/rs_properties_painter.h"
#include "property/rs_transition_properties.h"
#endif

namespace OHOS {
namespace Rosen {
RSRenderNode::RSRenderNode(NodeId id, std::weak_ptr<RSContext> context)
    : RSBaseRenderNode(id, context), renderProperties_(true)
{}

RSRenderNode::~RSRenderNode()
{
    FallbackAnimationsToRoot();
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

bool RSRenderNode::Animate(int64_t timestamp)
{
    return animationManager_.Animate(timestamp);
}

bool RSRenderNode::Update(RSDirtyRegionManager& dirtyManager, const RSProperties* parent, bool parentDirty,
    const std::unique_ptr<RSTransitionProperties>& transition)
{
    // no need to update invisible nodes
    if (!renderProperties_.GetVisible() && !isLastVisible_) {
        return false;
    }
    bool dirty = renderProperties_.UpdateGeometry(parent, parentDirty, transition);
    isDirtyRegionUpdated_ = false;
    UpdateDirtyRegion(dirtyManager, dirty);
    isLastVisible_ = renderProperties_.GetVisible();
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

void RSRenderNode::UpdateDirtyRegion(RSDirtyRegionManager& dirtyManager, bool geoDirty)
{
    if (!IsDirty() && !geoDirty) {
        return;
    }
    if (!oldDirty_.IsEmpty()) {
        dirtyManager.MergeDirtyRect(oldDirty_);
    }
    // merge old dirty if switch to invisible
    if (!renderProperties_.GetVisible() && isLastVisible_) {
        ROSEN_LOGD("RSRenderNode:: id %" PRIu64 " UpdateDirtyRegion visible->invisible", GetId());
    } else {
        auto dirtyRect = renderProperties_.GetDirtyRect();
        // filter invalid dirtyrect
        if (!dirtyRect.IsEmpty()) {
            dirtyManager.MergeDirtyRect(dirtyRect);
            isDirtyRegionUpdated_ = true;
            oldDirty_ = dirtyRect;
        } else {
            // keep old dirty and print debug log in case
            ROSEN_LOGD("RSRenderNode:: id %" PRIu64 " UpdateDirtyRegion invalid dirtyRect = [%d, %d, %d, %d]",
                GetId(), dirtyRect.left_, dirtyRect.top_, dirtyRect.width_, dirtyRect.height_);
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
    // should judge if there's any child out of parent
    if (!isPartialRenderEnabled) {
        isRenderUpdateIgnored_ = false;
    } else if (dirtyRegion.IsEmpty() || oldDirty_.IsEmpty()) {
        isRenderUpdateIgnored_ = true;
    } else {
        RectI intersectRect = dirtyRegion.IntersectRect(oldDirty_);
        isRenderUpdateIgnored_ = intersectRect.IsEmpty();
    }
}

void RSRenderNode::ProcessRenderBeforeChildren(RSPaintFilterCanvas& canvas)
{
#ifdef ROSEN_OHOS
    renderNodeSaveCount_ = canvas.SaveCanvasAndAlpha();
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
    auto transitionProperties = GetAnimationManager().GetTransitionProperties();
    RSPropertiesPainter::DrawTransitionProperties(transitionProperties, GetRenderProperties(), canvas);
    RSPropertiesPainter::DrawMask(GetRenderProperties(), canvas);
#endif
}

void RSRenderNode::ProcessRenderAfterChildren(RSPaintFilterCanvas& canvas)
{
#ifdef ROSEN_OHOS
    GetMutableRenderProperties().ResetBounds();
    canvas.RestoreCanvasAndAlpha(renderNodeSaveCount_);
#endif
}

void RSRenderNode::ClearModifiers()
{
    modifiers_.clear();
    drawCmdModifiers_.clear();
    SetDirty();
}

void RSRenderNode::AddModifier(const std::shared_ptr<RSRenderModifier> modifier)
{
    if (!modifier) {
        return;
    }
    if (modifier->GetType() == RSModifierType::BOUNDS || modifier->GetType() == RSModifierType::FRAME) {
        AddGeometryModifier(modifier);
    } else if (modifier->GetType() < RSModifierType::CUSTOM) {
        modifiers_.insert({ modifier->GetPropertyId(), modifier });
    } else {
        drawCmdModifiers_[modifier->GetType()].emplace_back(modifier);
    }
    modifier->GetProperty()->Attach(shared_from_this());
    SetDirty();
}

void RSRenderNode::AddGeometryModifier(const std::shared_ptr<RSRenderModifier>& modifier)
{
    // bounds and frame modifiers must be unique
    if (modifier->GetType() == RSModifierType::BOUNDS) {
        if (boundsModifier_ == nullptr) {
            boundsModifier_ = modifier;
        }
        modifiers_.insert({ modifier->GetPropertyId(), boundsModifier_ });
    }

    if (modifier->GetType() == RSModifierType::FRAME) {
        if (frameModifier_ == nullptr) {
            frameModifier_ = modifier;
        }
        modifiers_.insert({ modifier->GetPropertyId(), frameModifier_ });
    }
}

void RSRenderNode::RemoveModifier(const PropertyId& id)
{
    auto iter = modifiers_.find(id);
    if (iter != modifiers_.end()) {
        modifiers_.erase(iter);
        SetDirty();
        return;
    }
    for (auto& [type, modifiers] : drawCmdModifiers_) {
        modifiers.remove_if([id](const auto& modifier) -> bool {
            return modifier ? modifier->GetPropertyId() == id : true;
        });
    }
}

void RSRenderNode::ApplyModifiers()
{
    if (!RSBaseRenderNode::IsDirty()) {
        return;
    }
    RSModifyContext context = { GetMutableRenderProperties() };
    for (auto& [id, modify] : modifiers_) {
        if (modify) {
            modify->Apply(context);
        }
    }
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
} // namespace Rosen
} // namespace OHOS
