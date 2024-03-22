/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "pipeline/rs_proxy_render_node.h"

#include "command/rs_surface_node_command.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"
#include "visitor/rs_node_visitor.h"

namespace OHOS {
namespace Rosen {
RSProxyRenderNode::RSProxyRenderNode(
    NodeId id, std::weak_ptr<RSSurfaceRenderNode> target, NodeId targetId, const std::weak_ptr<RSContext>& context)
    : RSRenderNode(id, context), target_(target), targetId_(targetId)
{
#ifndef ROSEN_ARKUI_X
    MemoryInfo info = {sizeof(*this), ExtractPid(id), id, MEMORY_TYPE::MEM_RENDER_NODE};
    MemoryTrack::Instance().AddNodeRecord(id, info);
#endif
}

RSProxyRenderNode::~RSProxyRenderNode()
{
    ROSEN_LOGD("RSProxyRenderNode::~RSProxyRenderNode, proxy id:%{public}" PRIu64 " target:%{public}" PRIu64,
        GetId(), targetId_);
#ifndef ROSEN_ARKUI_X
    MemoryTrack::Instance().RemoveNodeRecord(GetId());
#endif
    CleanUp(true);
}

void RSProxyRenderNode::Prepare(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    if (!visitor) {
        return;
    }
    RSRenderNode::ApplyModifiers();
    visitor->PrepareProxyRenderNode(*this);
}

void RSProxyRenderNode::Process(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    if (!visitor) {
        return;
    }
    RSRenderNode::RenderTraceDebug();
    visitor->ProcessProxyRenderNode(*this);
}

void RSProxyRenderNode::SetContextMatrix(const std::optional<Drawing::Matrix>& matrix)
{
    if (contextMatrix_ == matrix) {
        return;
    }
    contextMatrix_ = matrix;
    if (auto target = target_.lock()) {
        target->SetContextMatrix(matrix, false);
        return;
    }
    // send a Command
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetContextMatrix>(targetId_, matrix);
    SendCommandFromRT(command, GetId());
}

void RSProxyRenderNode::SetContextAlpha(float alpha)
{
    if (contextAlpha_ == alpha) {
        return;
    }
    contextAlpha_ = alpha;
    if (auto target = target_.lock()) {
        target->SetContextAlpha(alpha, false);
        return;
    }
    // send a Command
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetContextAlpha>(targetId_, alpha);
    SendCommandFromRT(command, GetId());
}

void RSProxyRenderNode::SetContextClipRegion(const std::optional<Drawing::Rect>& clipRegion)
{
    if (contextClipRect_ == clipRegion) {
        return;
    }
    contextClipRect_ = clipRegion;
    if (auto target = target_.lock()) {
        target->SetContextClipRegion(clipRegion, false);
        return;
    }
    // send a Command
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetContextClipRegion>(targetId_, clipRegion);
    SendCommandFromRT(command, GetId());
}

void RSProxyRenderNode::ResetContextVariableCache()
{
    // reset context variable cache, make sure next visit will flush correct context variables.
    contextAlpha_ = -1.0f;
    contextMatrix_.reset();
    contextClipRect_.reset();
}

void RSProxyRenderNode::OnTreeStateChanged()
{
    RSRenderNode::OnTreeStateChanged();
    if (!IsOnTheTree()) {
        // removed from tree, clean up context variables
        CleanUp(false);
    }
    ResetContextVariableCache();
}

void RSProxyRenderNode::CleanUp(bool removeModifiers)
{
    // if target do not exist (in RT) or already destroyed (in RS), skip reset
    auto target = target_.lock();
    if (target == nullptr) {
        return;
    }

    // reset target node context properties
    target->SetContextAlpha(1.0f, false);
    target->SetContextMatrix(std::nullopt, false);
    target->SetContextClipRegion(std::nullopt, false);

    if (!removeModifiers) {
        return;
    }

    // remove all modifiers and animations added via proxy node
    const auto pid_of_this_node = ExtractPid(GetId());
    target->FilterModifiersByPid(pid_of_this_node);
    target->GetAnimationManager().FilterAnimationByPid(pid_of_this_node);
}
} // namespace Rosen
} // namespace OHOS
