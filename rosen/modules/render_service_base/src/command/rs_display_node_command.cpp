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

#include "command/rs_display_node_command.h"

#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_render_node_gc.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

void DisplayNodeCommandHelper::Create(RSContext& context, NodeId id, const RSDisplayNodeConfig& config)
{
    auto node = std::shared_ptr<RSDisplayRenderNode>(new RSDisplayRenderNode(id,
        config, context.weak_from_this()), RSRenderNodeGC::NodeDestructor);
    auto& nodeMap = context.GetMutableNodeMap();
    nodeMap.RegisterDisplayRenderNode(node);
    context.GetGlobalRootRenderNode()->AddChild(node);
    if (config.isMirrored) {
        auto mirrorSourceNode = nodeMap.GetRenderNode<RSDisplayRenderNode>(config.mirrorNodeId);
        if (mirrorSourceNode == nullptr) {
            return;
        }
        auto displayNode = RSBaseRenderNode::ReinterpretCast<RSDisplayRenderNode>(node);
        if (displayNode == nullptr) {
            RS_LOGE("DisplayNodeCommandHelper::Create displayNode is nullptr");
            return;
        }
        displayNode->SetMirrorSource(mirrorSourceNode);
    }
}

std::shared_ptr<RSDisplayRenderNode> DisplayNodeCommandHelper::CreateWithConfigInRS(
    RSContext& context, NodeId id, const RSDisplayNodeConfig& config)
{
    auto node = std::shared_ptr<RSDisplayRenderNode>(new RSDisplayRenderNode(id, config,
        context.weak_from_this()), RSRenderNodeGC::NodeDestructor);
    return node;
}

void DisplayNodeCommandHelper::AddDisplayNodeToTree(RSContext& context, NodeId id)
{
    auto& nodeMap = context.GetMutableNodeMap();
    auto node = nodeMap.GetRenderNode<RSDisplayRenderNode>(id);
    context.GetGlobalRootRenderNode()->AddChild(node);

    ROSEN_LOGD("DisplayNodeCommandHelper::AddDisplayNodeToTree, id:[%{public}" PRIu64 "]", id);
}

void DisplayNodeCommandHelper::RemoveDisplayNodeFromTree(RSContext& context, NodeId id)
{
    auto& nodeMap = context.GetMutableNodeMap();
    auto node = nodeMap.GetRenderNode<RSDisplayRenderNode>(id);
    context.GetGlobalRootRenderNode()->RemoveChild(node);

    ROSEN_LOGD("DisplayNodeCommandHelper::RemoveDisplayNodeFromTree, id:[%{public}" PRIu64 "]", id);
}

void DisplayNodeCommandHelper::SetScreenId(RSContext& context, NodeId id, uint64_t screenId)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSDisplayRenderNode>(id)) {
        node->SetScreenId(screenId);
        node->NotifyScreenNotSwitching();
        return;
    }
    RS_LOGE("DisplayNodeCommandHelper::%{public}s, displayNode not found, id:[%{public}" PRIu64
            "], screenId:[%{public}" PRIu64 "]", __func__, id, screenId);
}

void DisplayNodeCommandHelper::SetRogSize(RSContext& context, NodeId id, uint32_t rogWidth, uint32_t rogHeight)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSDisplayRenderNode>(id)) {
        node->SetRogSize(rogWidth, rogHeight);
    }
}

void DisplayNodeCommandHelper::SetForceCloseHdr(RSContext& context, NodeId id, bool isForceCloseHdr)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSDisplayRenderNode>(id)) {
        node->SetForceCloseHdr(isForceCloseHdr);
    }
}

void DisplayNodeCommandHelper::SetDisplayOffset(RSContext& context, NodeId id, int32_t offsetX, int32_t offsetY)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSDisplayRenderNode>(id)) {
        node->SetDisplayOffset(offsetX, offsetY);
    }
}

void DisplayNodeCommandHelper::SetScreenRotation(RSContext& context, NodeId id, const ScreenRotation& screenRotation)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSDisplayRenderNode>(id)) {
        node->SetScreenRotation(screenRotation);
    }
}

void DisplayNodeCommandHelper::SetSecurityDisplay(RSContext& context, NodeId id, bool isSecurityDisplay)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSDisplayRenderNode>(id)) {
        node->SetSecurityDisplay(isSecurityDisplay);
    }
}

void DisplayNodeCommandHelper::SetDisplayMode(RSContext& context, NodeId id, const RSDisplayNodeConfig& config)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSDisplayRenderNode>(id)) {
        bool isMirror = config.isMirrored;
        node->SetIsMirrorDisplay(isMirror);
        if (isMirror) {
            NodeId mirrorNodeId = config.mirrorNodeId;
            auto& nodeMap = context.GetNodeMap();
            auto mirrorSourceNode = nodeMap.GetRenderNode<RSDisplayRenderNode>(mirrorNodeId);
            if (mirrorSourceNode == nullptr) {
                ROSEN_LOGW("DisplayNodeCommandHelper::SetDisplayMode fail, displayNodeId:[%{public}" PRIu64 "]"
                    "mirrorNodeId:[%{public}" PRIu64 "]", id, mirrorNodeId);
                return;
            }
            node->SetMirrorSource(mirrorSourceNode);
        } else {
            node->ResetMirrorSource();
        }
    }
}

void DisplayNodeCommandHelper::SetBootAnimation(RSContext& context, NodeId nodeId, bool isBootAnimation)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSDisplayRenderNode>(nodeId)) {
        node->SetBootAnimation(isBootAnimation);
    }
}

void DisplayNodeCommandHelper::SetScbNodePid(RSContext& context, NodeId nodeId,
    const std::vector<int32_t>& oldScbPids, int32_t currentScbPid)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSDisplayRenderNode>(nodeId)) {
        ROSEN_LOGI("SetScbNodePid NodeId:[%{public}" PRIu64 "] currentPid:[%{public}d]", nodeId, currentScbPid);
        node->SetScbNodePid(oldScbPids, currentScbPid);
    }
}

void DisplayNodeCommandHelper::SetVirtualScreenMuteStatus(RSContext& context, NodeId nodeId,
    bool virtualScreenMuteStatus)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSDisplayRenderNode>(nodeId)) {
        ROSEN_LOGI("SetVirtualScreenMuteStatus NodeId:[%{public}" PRIu64 "]"
            " screenId: %{public}" PRIu64 " virtualScreenMuteStatus: %{public}d",
            nodeId, node->GetScreenId(), virtualScreenMuteStatus);
        node->SetVirtualScreenMuteStatus(virtualScreenMuteStatus);
    }
}
} // namespace Rosen
} // namespace OHOS
