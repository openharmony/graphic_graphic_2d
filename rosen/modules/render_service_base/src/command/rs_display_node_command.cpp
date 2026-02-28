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
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {

/**
 * @brief This function is used to set the display node by screen id.
 * @param context The context of the render service.
 * @param id The screen id of the display node.
 * @param lambda The lambda function to set the display node.
 * @return Returns true if the display node is set successfully, otherwise returns false.
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
    RS_TRACE_NAME_FMT("DisplayNodeCommandHelper::Create displayNodeId[%" PRIu64 "], screenId[%" PRIu64 "]",
        id, config.screenId);
    
    auto node = std::shared_ptr<RSLogicalDisplayRenderNode>(new RSLogicalDisplayRenderNode(id,
        config, context.weak_from_this()), RSRenderNodeGC::NodeDestructor);
    auto& nodeMap = context.GetMutableNodeMap();
    nodeMap.RegisterRenderNode(node);

    AddDisplayNodeToTree(context, id);
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
        RS_LOGE("%{public}s Invalid NodeId curNodeId: %{public}" PRIu64, __func__, id);
        return;
    }
    auto screenId = logicalDisplayNode->GetScreenId();
    if (screenId == INVALID_SCREEN_ID) {
        RS_LOGE("%{public}s failed, screenId must be set before display node add to tree", __func__);
        return;
    }
    auto lambda = [&logicalDisplayNode](const std::shared_ptr<RSScreenRenderNode>& screenRenderNode) {
        screenRenderNode->AddChild(logicalDisplayNode);
    };
    if (!TrySetScreenNodeByScreenId(context, screenId, lambda)) {
        RS_LOGE("%{public}s: displayNode[%{public}" PRIu64 "] failed to AddToTree, can't find ScreenId[%{public}" PRIu64
            "]", __func__, id, screenId);
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
        RS_LOGE("%{public}s Invalid NodeId curNodeId: %{public}" PRIu64, __func__, id);
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
    const auto& nodeMap = context.GetNodeMap();
    auto logicalDisplayNode = nodeMap.GetRenderNode<RSLogicalDisplayRenderNode>(id);
    if (logicalDisplayNode == nullptr) {
        RS_LOGE("DisplayNodeCommandHelper::%{public}s, displayNode not found, id:[%{public}" PRIu64
            "], screenId:[%{public}" PRIu64 "]", __func__, id, screenId);
        return;
    }

    logicalDisplayNode->SetScreenId(screenId);
    logicalDisplayNode->NotifyScreenNotSwitching();
    for (auto& child : *(logicalDisplayNode->GetChildren())) {
        if (!child) {
            continue;
        }
        auto surfaceChild = child->ReinterpretCastTo<RSSurfaceRenderNode>();
        if (!surfaceChild) {
            continue;
        }
        auto& info = surfaceChild->GetAttachedInfo();
        if (info.has_value()) {
            info->first = screenId;
        }
    }
    logicalDisplayNode->RemoveFromTree();
    auto lambda = [&logicalDisplayNode](auto& screenRenderNode) {
        screenRenderNode->AddChild(logicalDisplayNode);
    };
    if (!TrySetScreenNodeByScreenId(context, screenId, lambda)) {
        RS_LOGE("%{public}s Invalid ScreenId NodeId: %{public}" PRIu64
            ", curNodeId: %{public}" PRIu64, __func__, screenId, id);
    }
}

void DisplayNodeCommandHelper::SetDisplayContentRect(RSContext& context, NodeId id,
    const Rect& contentRect)
{
    RS_LOGI("DisplayNodeCommandHelper::SetDisplayContentRect node:[%{public}" PRIu64 "], "
             "rect:[%{public}d,%{public}d,%{public}d,%{public}d]",
             id, contentRect.x, contentRect.y, contentRect.w, contentRect.h);
    if (auto node = context.GetNodeMap().GetRenderNode<RSLogicalDisplayRenderNode>(id)) {
        node->SetDisplayContentRect(contentRect);
    } else {
        RS_LOGE("%{public}s Invalid NodeId curNodeId: %{public}" PRIu64, __func__, id);
    }
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
    if (auto node = context.GetNodeMap().GetRenderNode<RSLogicalDisplayRenderNode>(id)) {
        node->SetScreenRotation(screenRotation);
    } else {
        RS_LOGE("%{public}s Invalid NodeId curNodeId: %{public}" PRIu64, __func__, id);
    }
}

void DisplayNodeCommandHelper::SetSecurityDisplay(RSContext& context, NodeId id, bool isSecurityDisplay)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSLogicalDisplayRenderNode>(id)) {
        node->SetSecurityDisplay(isSecurityDisplay);
    } else {
        RS_LOGE("%{public}s Invalid NodeId curNodeId: %{public}" PRIu64, __func__, id);
    }
}

void DisplayNodeCommandHelper::SetDisplayMode(RSContext& context, NodeId id, const RSDisplayNodeConfig& config)
{
    auto node = context.GetNodeMap().GetRenderNode<RSLogicalDisplayRenderNode>(id);
    if (node == nullptr) {
        RS_LOGE("%{public}s Invalid NodeId curNodeId: %{public}" PRIu64, __func__, id);
        return;
    }

    bool isMirror = config.isMirrored;
    node->SetIsMirrorDisplay(isMirror);
    if (isMirror) {
        NodeId mirroredNodeId = config.mirrorNodeId;
        auto& nodeMap = context.GetNodeMap();
        auto mirrorSourceNode = nodeMap.GetRenderNode<RSLogicalDisplayRenderNode>(mirroredNodeId);
        if (mirrorSourceNode == nullptr) {
            RS_LOGW("DisplayNodeCommandHelper::SetDisplayMode fail, displayNodeId:[%{public}" PRIu64 "]"
                "mirroredNodeId:[%{public}" PRIu64 "]", id, mirroredNodeId);
            return;
        }
        node->SetMirrorSource(mirrorSourceNode);
        node->SetMirrorSourceRotation(static_cast<ScreenRotation>(config.mirrorSourceRotation));
        RS_LOGI("DisplayNodeCommandHelper::%{public}s displayNodeId: %{public}" PRIu64 " mirrorSource: %{public}" PRIu64
            " mirrorSourceRotation: %{public}" PRIu32, __func__, id, mirroredNodeId, config.mirrorSourceRotation);
    } else {
        node->ResetMirrorSource();
    }
}

void DisplayNodeCommandHelper::SetBootAnimation(RSContext& context, NodeId nodeId, bool isBootAnimation)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSLogicalDisplayRenderNode>(nodeId)) {
        node->SetBootAnimation(isBootAnimation);
    } else {
        RS_LOGE("%{public}s Invalid NodeId curNodeId: %{public}" PRIu64, __func__, nodeId);
    }
}

void DisplayNodeCommandHelper::ClearModifiersByPid(RSContext& context, NodeId nodeId, int32_t pid)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSLogicalDisplayRenderNode>(nodeId)) {
        RS_LOGI(
            "ClearModifiersByPid NodeId:[%{public}" PRIu64 "] pid:[%{public}u]", nodeId, static_cast<uint32_t>(pid));
        node->ClearModifiersByPid(pid);
    } else {
        RS_LOGE("%{public}s Invalid NodeId curNodeId: %{public}" PRIu64, __func__, nodeId);
    }
}

void DisplayNodeCommandHelper::SetVirtualScreenMuteStatus(RSContext& context, NodeId nodeId,
    bool virtualScreenMuteStatus)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSLogicalDisplayRenderNode>(nodeId)) {
        RS_LOGI("SetVirtualScreenMuteStatus NodeId:[%{public}" PRIu64 "]"
            " screenId: %{public}" PRIu64 " virtualScreenMuteStatus: %{public}d",
            nodeId, node->GetScreenId(), virtualScreenMuteStatus);
        node->SetVirtualScreenMuteStatus(virtualScreenMuteStatus);
    } else {
        RS_LOGE("%{public}s Invalid NodeId curNodeId: %{public}" PRIu64, __func__, nodeId);
    }
}

} // namespace Rosen
} // namespace OHOS
