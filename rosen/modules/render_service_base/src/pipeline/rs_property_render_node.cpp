/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "pipeline/rs_property_render_node.h"

#include <algorithm>

#include "animation/rs_render_animation.h"
#include "common/rs_obj_abs_geometry.h"
#include "pipeline/rs_render_node_map.h"
#include "platform/common/rs_log.h"
#ifdef ROSEN_OHOS
#include "pipeline/rs_paint_filter_canvas.h"
#endif

namespace OHOS {
namespace Rosen {
RSPropertyRenderNode::RSPropertyRenderNode(NodeId id)
    : RSBaseRenderNode(id), renderProperties_(true, id), animationManager_(this)
{}

RSPropertyRenderNode::~RSPropertyRenderNode()
{
    FallbackAnimationsToRoot();
}

void RSPropertyRenderNode::FallbackAnimationsToRoot()
{
    auto target = RSRenderNodeMap::Instance().GetAnimationFallbackNode();
    if (!target) {
        ROSEN_LOGE("Failed to move animation to root, root render node is null!");
        return;
    }

    for (const auto& [animationId, animation] : animationManager_.animations_) {
        animation->Detach();
        target->animationManager_.OnAnimationAdd(animation->GetProperty());
        target->animationManager_.animations_[animationId] = animation;
    }
}

bool RSPropertyRenderNode::Animate(int64_t timestamp)
{
    return animationManager_.Animate(timestamp);
}

bool RSPropertyRenderNode::Update(RSDirtyRegionManager& dirtyManager, const RSProperties* parent, bool parentDirty)
{
    if (!renderProperties_.GetVisible()) {
        return false;
    }
    bool dirty = renderProperties_.UpdateGeometry(parent, parentDirty);
    UpdateDirtyRegion(dirtyManager);
    renderProperties_.ResetDirty();
    animationManager_.UpdateDisappearingChildren(dirtyManager, &renderProperties_, dirty);
    return dirty;
}

RSProperties& RSPropertyRenderNode::GetRenderProperties()
{
    return renderProperties_;
}

const RSProperties& RSPropertyRenderNode::GetRenderProperties() const
{
    return renderProperties_;
}

void RSPropertyRenderNode::UpdateDirtyRegion(RSDirtyRegionManager& dirtyManager)
{
    if (!IsDirty()) {
        return;
    }
    dirtyManager.MergeDirtyRect(renderProperties_.GetDirtyRect());
    if (!oldDirty_.IsEmpty()) {
        dirtyManager.MergeDirtyRect(oldDirty_);
    }
    oldDirty_ = renderProperties_.GetDirtyRect();
    SetClean();
}

bool RSPropertyRenderNode::IsDirty() const
{
    return RSBaseRenderNode::IsDirty() || renderProperties_.IsDirty();
}

void RSPropertyRenderNode::OnAddChild(RSBaseRenderNode::SharedPtr& child)
{
    auto childPtr = RSBaseRenderNode::ReinterpretCast<RSPropertyRenderNode>(child);
    animationManager_.RemoveDisappearingChild(childPtr);
}

void RSPropertyRenderNode::OnRemoveChild(RSBaseRenderNode::SharedPtr& child)
{
    auto childPtr = RSBaseRenderNode::ReinterpretCast<RSPropertyRenderNode>(child);

    if (childPtr && childPtr->GetAnimationManager().HasTransition()) {
        animationManager_.AddDisappearingChild(childPtr);
    } else {
        child->ResetParent();
    }
}

bool RSPropertyRenderNode::OnUnregister()
{
    if (!GetAnimationManager().HasTransition()) {
        return true;
    }
    SetPendingRemoval(true);
    return false;
}

void RSPropertyRenderNode::ProcessRenderBeforeChildren(RSPaintFilterCanvas& canvas)
{
#ifdef ROSEN_OHOS
    canvas.save();
    canvas.SaveAlpha();
    canvas.MultiplyAlpha(GetRenderProperties().GetAlpha());
    auto boundsGeo = std::static_pointer_cast<RSObjAbsGeometry>(GetRenderProperties().GetBoundsGeometry());
    canvas.concat(boundsGeo->GetMatrix());
    GetAnimationManager().DoTransition(canvas, GetRenderProperties());
#endif
}

void RSPropertyRenderNode::ProcessRenderAfterChildren(RSPaintFilterCanvas& canvas)
{
#ifdef ROSEN_OHOS
    GetAnimationManager().RenderDisappearingChildren(canvas);
    GetRenderProperties().ResetBounds();
    canvas.RestoreAlpha();
    canvas.restore();
#endif
}

} // namespace Rosen
} // namespace OHOS
