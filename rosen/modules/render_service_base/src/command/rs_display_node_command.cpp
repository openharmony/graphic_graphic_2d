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

#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_logical_display_render_node.h"
#include "pipeline/rs_render_node_gc.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

/**
 * @brief This function is used to set the display node by screen id.
 * @param context The context of the render service
 * @param id The screen id of the display node
 * @param lambda The lambda function to set the display node.
 * @return Returns true if the display node is set successfully, otherwise returns false;
 */
template <class Lambda>
bool TrySetScreenNodeByScreenId(RSContext& context, ScreenId id, Lambda&& lambda)
{
    auto& nodeMap = context.GetMutableNodeMap();
    bool nodeExists = false;
    nodeMap.TraverseScreenNodes([&lambda, &nodeExists, id](auto& node) {
        if (!node || node->GetScreenId() != id) {
            return;
        }
        nodeExists = true;
        lambda(node);
    });
    return nodeExists;
}

void DisplayNodeCommandHelper::Create(RSContext& context, NodeId id, const RSDisplayNodeConfig& config)
{
    auto node = std::shared_ptr<RSLogicalDisplayRenderNode>(new RSLogicalDisplayRenderNode(id,
        config, context.weak_from_this()), RSRenderNodeGC::NodeDestructor);
    auto& nodeMap = context.GetMutableNodeMap();
    nodeMap.RegisterRenderNode(node);
    if (config.screenId == INVALID_SCREEN_ID) {
        return;
    }
    auto lambda = [&node](const std::shared_ptr<RSScreenRenderNode>& screenRenderNode) {
        screenRenderNode->AddChild(node);
    };
    if (!TrySetScreenNodeByScreenId(context, config.screenId, lambda)) {
        RS_LOGE("%{public}s Invalid ScreenId NodeId: %{public}" PRIu64
            ", curNodeId: %{public}" PRIu64, __func__, config.screenId, id);
    }

    SetDisplayMode(context, id, config);
}

std::shared_ptr<RSLogicalDisplayRenderNode> DisplayNodeCommandHelper::CreateWithConfigInRS(
    RSContext& context, NodeId id, const RSDisplayNodeConfig& config)
{
    auto node = std::shared_ptr<RSLogicalDisplayRenderNode>(new RSLogicalDisplayRenderNode(id, config,
        context.weak_from_this()), RSRenderNodeGC::NodeDestructor);
    return node;
}

void DisplayNodeCommandHelper::AddDisplayNodeToTree(RSContext& context, NodeId id)
{
    auto& nodeMap = context.GetMutableNodeMap();
    auto logicalDisplayNode = nodeMap.GetRenderNode<RSLogicalDisplayRenderNode>(id);
    if (logicalDisplayNode == nullptr) {
        return;
    }
    
    auto screenId = logicalDisplayNode->GetScreenId();
    if (screenId == INVALID_SCREEN_ID) {
        RS_LOGE("%{public}s failed, screenId must be set before display node add to true", __func__);
        return;
    }
    auto lambda = [&logicalDisplayNode](const std::shared_ptr<RSScreenRenderNode>& screenRenderNode) {
        screenRenderNode->AddChild(logicalDisplayNode);
    };
    if (!TrySetScreenNodeByScreenId(context, screenId, lambda)) {
        RS_LOGE("%{public}s Invalid ScreenId NodeId: %{public}" PRIu64
            ", curNodeId: %{public}" PRIu64, __func__, screenId, id);
        logicalDisplayNode->NotifySetOnTreeFlag();
    } else {
        logicalDisplayNode->ResetSetOnTreeFlag();
    }
}

void DisplayNodeCommandHelper::RemoveDisplayNodeFromTree(RSContext& context, NodeId id)
{
    auto& nodeMap = context.GetMutableNodeMap();
    auto logicalDisplayNode = nodeMap.GetRenderNode<RSLogicalDisplayRenderNode>(id);
    if (logicalDisplayNode == nullptr) {
        return;
    }

    auto lambda = [&logicalDisplayNode](const std::shared_ptr<RSScreenRenderNode>& screenRenderNode) {
        screenRenderNode->RemoveChild(logicalDisplayNode);
    };
    auto screenId = logicalDisplayNode->GetScreenId();
    if (!TrySetScreenNodeByScreenId(context, screenId, lambda)) {
        RS_LOGE("%{public}s Invalid ScreenId NodeId: %{public}" PRIu64
            ", curNodeId: %{public}" PRIu64, __func__, screenId, id);
    }
    logicalDisplayNode->ResetSetOnTreeFlag();
}

void DisplayNodeCommandHelper::SetScreenId(RSContext& context, NodeId id, uint64_t screenId)
{
    auto& nodeMap = context.GetMutableNodeMap();
    auto logicalDisplayNode = nodeMap.GetRenderNode<RSLogicalDisplayRenderNode>(id);
    if (logicalDisplayNode == nullptr) {
        RS_LOGE("DisplayNodeCommandHelper::%{public}s, displayNode node found, id:[%{public}" PRIu64
            "], screenId:[%{public}" PRIu64 "]", __func__, id, screenId);
        return;
    }
    logicalDisplayNode->SetScreenId(screenId);
    logicalDisplayNode->NotifyScreenNotSwitching();
    AddDisplayNodeToTree(context, id);
}

void DisplayNodeCommandHelper::SetForceCloseHdr(RSContext& context, NodeId id, bool isForceCloseHdr)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSLogicalDisplayRenderNode>(id)) {
        auto screenId = node->GetScreenId();
        auto lambda = [isForceCloseHdr] (auto& node) {
            node->SetForceCloseHdr(isForceCloseHdr);
        };
        if (!TrySetScreenNodeByScreenId(context, screenId, lambda)) {
            RS_LOGE("%{public}s Invalid ScreenId NodeId: %{public}" PRIu64
                ", curNodeId: %{public}" PRIu64, __func__, screenId, id);
        }
    } else {
        RS_LOGE("%{public}s Invalid NodeId curNodeId: %{public}" PRIu64, __func__, id);
    }
}

void DisplayNodeCommandHelper::SetScreenRotation(RSContext& context, NodeId id, const ScreenRotation& screenRotation)
{
    RS_LOGE("SetScreenRotation %{public}d", (int)screenRotation);
    if (auto node = context.GetNodeMap().GetRenderNode<RSLogicalDisplayRenderNode>(id)) {
        node->SetScreenRotation(screenRotation);
    }
}

void DisplayNodeCommandHelper::SetSecurityDisplay(RSContext& context, NodeId id, bool isSecurityDisplay)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSLogicalDisplayRenderNode>(id)) {
        node->SetSecurityDisplay(isSecurityDisplay);
    }
}

void DisplayNodeCommandHelper::SetDisplayMode(RSContext& context, NodeId id, const RSDisplayNodeConfig& config)
{
    auto node = context.GetNodeMap().GetRenderNode<RSLogicalDisplayRenderNode>(id);
    if (node == nullptr) {
        return;
    }

    bool isMirror = config.isMirrored;
    node->SetIsMirrorDisplay(isMirror);
    if (isMirror) {
        NodeId mirroredNodeId = config.mirrorNodeId;
        auto& nodeMap = context.GetNodeMap();
        auto mirrorSourceNode = nodeMap.GetRenderNode<RSLogicalDisplayRenderNode>(mirroredNodeId);
        if (mirrorSourceNode == nullptr) {
            ROSEN_LOGW("DisplayNodeCommandHelper::SetDisplayMode fail, displayNodeId:[%{public}" PRIu64 "]"
                "mirroredNodeId:[%{public}" PRIu64 "]", id, mirroredNodeId);
            return;
        }
        node->SetMirrorSource(mirrorSourceNode);
    } else {
        node->ResetMirrorSource();
    }
}

void DisplayNodeCommandHelper::SetBootAnimation(RSContext& context, NodeId nodeId, bool isBootAnimation)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSLogicalDisplayRenderNode>(nodeId)) {
        node->SetBootAnimation(isBootAnimation);
    }
}

void DisplayNodeCommandHelper::SetScbNodePid(RSContext& context, NodeId nodeId,
    const std::vector<int32_t>& oldScbPids, int32_t currentScbPid)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSLogicalDisplayRenderNode>(nodeId)) {
        ROSEN_LOGI("SetScbNodePid NodeId:[%{public}" PRIu64 "] currentPid:[%{public}d]", nodeId, currentScbPid);
        node->SetScbNodePid(oldScbPids, currentScbPid);
    }
}

void DisplayNodeCommandHelper::SetVirtualScreenMuteStatus(RSContext& context, NodeId nodeId,
    bool virtualScreenMuteStatus)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSLogicalDisplayRenderNode>(nodeId)) {
        ROSEN_LOGI("SetVirtualScreenMuteStatus NodeId:[%{public}" PRIu64 "]"
            " screenId: %{public}" PRIu64 " virtualScreenMuteStatus: %{public}d",
            nodeId, node->GetScreenId(), virtualScreenMuteStatus);
        node->SetVirtualScreenMuteStatus(virtualScreenMuteStatus);
    }
}

} // namespace Rosen
} // namespace OHOS
