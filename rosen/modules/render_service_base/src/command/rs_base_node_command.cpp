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

#include "command/rs_base_node_command.h"

#include "pipeline/rs_base_render_node.h"

namespace OHOS {
namespace Rosen {

void BaseNodeCommandHelper::Destroy(RSContext& context, NodeId nodeId)
{
    auto& nodeMap = context.GetMutableNodeMap();
    auto node = nodeMap.GetRenderNode<RSBaseRenderNode>(nodeId);
    if (node == nullptr) {
        return;
    }
    node->ClearChildren();
    node->RemoveFromTree();
    nodeMap.UnregisterRenderNode(node->GetId());
}

void BaseNodeCommandHelper::AddChild(RSContext& context, NodeId nodeId, NodeId childNodeId, int32_t index)
{
    auto& nodeMap = context.GetNodeMap();
    auto node = nodeMap.GetRenderNode<RSBaseRenderNode>(nodeId);
    auto child = nodeMap.GetRenderNode<RSBaseRenderNode>(childNodeId);
    if (node && child) {
        node->AddChild(child, index);
    }
}

void BaseNodeCommandHelper::RemoveChild(RSContext& context, NodeId nodeId, NodeId childNodeId)
{
    auto& nodeMap = context.GetNodeMap();
    auto node = nodeMap.GetRenderNode<RSBaseRenderNode>(nodeId);
    auto child = nodeMap.GetRenderNode<RSBaseRenderNode>(childNodeId);
    if (node && child) {
        node->RemoveChild(child);
    }
}

void BaseNodeCommandHelper::RemoveFromTree(RSContext& context, NodeId nodeId)
{
    auto& nodeMap = context.GetNodeMap();
    auto node = nodeMap.GetRenderNode<RSBaseRenderNode>(nodeId);
    if (node == nullptr) {
        return;
    }
    node->RemoveFromTree();
}

void BaseNodeCommandHelper::ClearChildren(RSContext& context, NodeId nodeId)
{
    auto& nodeMap = context.GetNodeMap();
    auto node = nodeMap.GetRenderNode<RSBaseRenderNode>(nodeId);
    if (node) {
        node->ClearChildren();
    }
}

} // namespace Rosen
} // namespace OHOS
