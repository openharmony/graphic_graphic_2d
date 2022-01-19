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

#include "pipeline/rs_render_node.h"

#include <algorithm>

#include "animation/rs_render_animation.h"
#include "common/rs_obj_abs_geometry.h"
#include "platform/common/rs_log.h"
#ifdef ROSEN_OHOS
#include "pipeline/rs_paint_filter_canvas.h"
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

    for (const auto& [animationId, animation] : animationManager_.animations_) {
        animation->Detach();
        target->animationManager_.AddAnimation(animation);
    }
}

bool RSRenderNode::Animate(int64_t timestamp)
{
    return animationManager_.Animate(timestamp) || RSBaseRenderNode::Animate(timestamp);
}

bool RSRenderNode::Update(RSDirtyRegionManager& dirtyManager, const RSProperties* parent, bool parentDirty)
{
    GetDisappearingChildren().remove_if([this](std::shared_ptr<RSBaseRenderNode>& child) {
        bool needToDelete = !child->HasTransition();
        if (needToDelete && ROSEN_EQ<RSBaseRenderNode>(child->GetParent(), weak_from_this())) {
            child->ResetParent();
        }
        return needToDelete;
    });

    if (!renderProperties_.GetVisible()) {
        return false;
    }
    bool dirty = renderProperties_.UpdateGeometry(parent, parentDirty);
    UpdateDirtyRegion(dirtyManager);
    renderProperties_.ResetDirty();
    return dirty;
}

RSProperties& RSRenderNode::GetRenderProperties()
{
    return renderProperties_;
}

const RSProperties& RSRenderNode::GetRenderProperties() const
{
    return renderProperties_;
}

void RSRenderNode::UpdateDirtyRegion(RSDirtyRegionManager& dirtyManager)
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

bool RSRenderNode::IsDirty() const
{
    return RSBaseRenderNode::IsDirty() || renderProperties_.IsDirty();
}

void RSRenderNode::ProcessRenderBeforeChildren(RSPaintFilterCanvas& canvas)
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

void RSRenderNode::ProcessRenderAfterChildren(RSPaintFilterCanvas& canvas)
{
#ifdef ROSEN_OHOS
    GetRenderProperties().ResetBounds();
    canvas.RestoreAlpha();
    canvas.restore();
#endif
}

} // namespace Rosen
} // namespace OHOS
