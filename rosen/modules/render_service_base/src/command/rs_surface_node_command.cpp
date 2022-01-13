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

#include "command/rs_surface_node_command.h"

#include "pipeline/rs_surface_render_node.h"

namespace OHOS {
namespace Rosen {

void SurfaceNodeCommandHelper::Create(RSContext& context, NodeId id)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id);
    auto& nodeMap = context.GetNodeMap();
    nodeMap.RegisterRenderNode(node);
}

void SurfaceNodeCommandHelper::SetMatrix(RSContext& context, NodeId id, SkMatrix matrix)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(id)) {
        node->SetMatrix(matrix, false);
    }
}

void SurfaceNodeCommandHelper::SetAlpha(RSContext& context, NodeId id, float alpha)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(id)) {
        node->SetAlpha(alpha, false);
    }
}

void SurfaceNodeCommandHelper::SetParentSurface(RSContext& context, NodeId id, NodeId parentId)
{
    auto& nodeMap = context.GetNodeMap();
    auto node = nodeMap.GetRenderNode<RSSurfaceRenderNode>(id);
    auto parent = nodeMap.GetRenderNode<RSBaseRenderNode>(parentId);
    if (node && parent) {
        node->SetParentId(parentId, false);
        parent->AddChild(node);
    }
}

void SurfaceNodeCommandHelper::RemoveSelf(RSContext& context, NodeId id)
{
    auto& nodeMap = context.GetNodeMap();
    auto node = nodeMap.GetRenderNode<RSBaseRenderNode>(id);
    if (node) {
        node->RemoveFromTree();
    }
}

void SurfaceNodeCommandHelper::UpdateSurfaceDefaultSize(RSContext& context, NodeId id, float width, float height)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(id)) {
        node->UpdateSurfaceDefaultSize(width, height);
    }
}

} // namespace Rosen
} // namespace OHOS
