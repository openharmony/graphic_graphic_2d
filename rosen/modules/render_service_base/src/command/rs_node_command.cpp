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

#include "command/rs_node_command.h"

namespace OHOS {
namespace Rosen {
void RSNodeCommandHelper::AddModifier(RSContext& context, NodeId nodeId,
    const std::shared_ptr<RSRenderModifier>& modifier)
{
    auto& nodeMap = context.GetNodeMap();
    auto node = nodeMap.GetRenderNode<RSRenderNode>(nodeId);
    if (node) {
        node->AddModifier(modifier);
    }
}

void RSNodeCommandHelper::RemoveModifier(RSContext& context, NodeId nodeId, PropertyId propertyId)
{
    auto& nodeMap = context.GetNodeMap();
    auto node = nodeMap.GetRenderNode<RSRenderNode>(nodeId);
    if (node) {
        node->RemoveModifier(propertyId);
    }
}

void RSNodeCommandHelper::SetFreeze(RSContext& context, NodeId nodeId, bool isFreeze)
{
    auto& nodeMap = context.GetNodeMap();
    auto node = nodeMap.GetRenderNode<RSRenderNode>(nodeId);
    if (node) {
        node->SetStaticCached(isFreeze);
    }
}

void RSNodeCommandHelper::SetNodeName(RSContext& context, NodeId nodeId, std::string& nodeName)
{
    auto& nodeMap = context.GetNodeMap();
    auto node = nodeMap.GetRenderNode<RSRenderNode>(nodeId);
    if (node) {
        node->SetNodeName(nodeName);
    }
}

void RSNodeCommandHelper::MarkNodeGroup(RSContext& context, NodeId nodeId, bool isNodeGroup, bool isForced,
    bool includeProperty)
{
    auto& nodeMap = context.GetNodeMap();
    if (auto node = nodeMap.GetRenderNode<RSRenderNode>(nodeId)) {
        node->MarkNodeGroup(isForced ? RSRenderNode::GROUPED_BY_USER : RSRenderNode::GROUPED_BY_UI, isNodeGroup,
            includeProperty);
    }
}

void RSNodeCommandHelper::MarkNodeSingleFrameComposer(RSContext& context,
    NodeId nodeId, bool isNodeSingleFrameComposer, pid_t pid)
{
    auto& nodeMap = context.GetNodeMap();
    if (auto node = nodeMap.GetRenderNode<RSRenderNode>(nodeId)) {
        RSSingleFrameComposer::AddOrRemoveAppPidToMap(isNodeSingleFrameComposer, pid);
        node->MarkNodeSingleFrameComposer(isNodeSingleFrameComposer, pid);
    }
}

void RSNodeCommandHelper::SetDrawRegion(RSContext& context, NodeId nodeId, std::shared_ptr<RectF> rect)
{
    auto& nodeMap = context.GetNodeMap();
    auto node = nodeMap.GetRenderNode<RSRenderNode>(nodeId);
    if (node) {
        node->SetDrawRegion(rect);
    }
}

void RSNodeCommandHelper::SetOutOfParent(RSContext& context, NodeId nodeId, OutOfParentType outOfParent)
{
    auto& nodeMap = context.GetNodeMap();
    auto node = nodeMap.GetRenderNode<RSRenderNode>(nodeId);
    if (node) {
        node->SetOutOfParent(outOfParent);
    }
}

void RSNodeCommandHelper::SetTakeSurfaceForUIFlag(RSContext& context, NodeId nodeId)
{
    auto& nodeMap = context.GetNodeMap();
    auto node = nodeMap.GetRenderNode<RSRenderNode>(nodeId);
    if (node) {
        node->ExcuteSurfaceCaptureCommand();
    }
}

void RSNodeCommandHelper::RegisterGeometryTransitionPair(RSContext& context, NodeId inNodeId, NodeId outNodeId)
{
    auto& nodeMap = context.GetNodeMap();
    auto inNode = nodeMap.GetRenderNode<RSRenderNode>(inNodeId);
    auto outNode = nodeMap.GetRenderNode<RSRenderNode>(outNodeId);
    if (inNode == nullptr || outNode == nullptr) {
        return;
    }
    auto sharedTransitionParam = std::make_shared<SharedTransitionParam>(inNode, outNode);
    inNode->SetSharedTransitionParam(sharedTransitionParam);
    outNode->SetSharedTransitionParam(sharedTransitionParam);
}

void RSNodeCommandHelper::UnregisterGeometryTransitionPair(RSContext& context, NodeId inNodeId, NodeId outNodeId)
{
    auto& nodeMap = context.GetNodeMap();
    auto inNode = nodeMap.GetRenderNode<RSRenderNode>(inNodeId);
    auto outNode = nodeMap.GetRenderNode<RSRenderNode>(outNodeId);
    // Sanity check, if any check failed, RSUniRenderVisitor will auto unregister the pair, we do nothing here.
    if (inNode && outNode && inNode->GetSharedTransitionParam() == outNode->GetSharedTransitionParam()) {
        inNode->SetSharedTransitionParam(nullptr);
        outNode->SetSharedTransitionParam(nullptr);
    }
}
} // namespace Rosen
} // namespace OHOS
