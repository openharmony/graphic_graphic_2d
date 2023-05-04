/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "common/rs_vector4.h"
#include "pipeline/rs_surface_render_node.h"

namespace OHOS {
namespace Rosen {

void SurfaceNodeCommandHelper::Create(RSContext& context, NodeId id)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context.weak_from_this());
    auto& nodeMap = context.GetMutableNodeMap();
    nodeMap.RegisterRenderNode(node);
}

void SurfaceNodeCommandHelper::SetContextMatrix(RSContext& context, NodeId id, SkMatrix matrix)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(id)) {
        node->SetContextMatrix(matrix, false);
    }
}

void SurfaceNodeCommandHelper::SetContextAlpha(RSContext& context, NodeId id, float alpha)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(id)) {
        node->SetContextAlpha(alpha, false);
    }
}

void SurfaceNodeCommandHelper::SetContextClipRegion(RSContext& context, NodeId id, SkRect clipRect)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(id)) {
        node->SetContextClipRegion(clipRect, false);
    }
}

void SurfaceNodeCommandHelper::SetSecurityLayer(RSContext& context, NodeId id, bool isSecurityLayer)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(id)) {
        node->SetSecurityLayer(isSecurityLayer);
    }
}

#if !defined(_WIN32) && !defined(__APPLE__) && !defined(__gnu_linux__)
void SurfaceNodeCommandHelper::SetColorSpace(RSContext& context, NodeId id, ColorGamut colorSpace)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(id)) {
        node->SetColorSpace(colorSpace);
    }
}
#endif

void SurfaceNodeCommandHelper::UpdateSurfaceDefaultSize(RSContext& context, NodeId id, float width, float height)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(id)) {
        node->UpdateSurfaceDefaultSize(width, height);
    }
}

void SurfaceNodeCommandHelper::ConnectToNodeInRenderService(RSContext& context, NodeId id)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(id)) {
        node->ConnectToNodeInRenderService();
    }
}

void SurfaceNodeCommandHelper::SetCallbackForRenderThreadRefresh(RSContext& context, NodeId id, std::function<void(void)> callback)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(id)) {
        if (node->NeedSetCallbackForRenderThreadRefresh()) {
            node->SetCallbackForRenderThreadRefresh(callback);
        }
    }
}

void SurfaceNodeCommandHelper::SetContextBounds(RSContext& context, NodeId id, Vector4f bounds)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(id)) {
        node->GetMutableRenderProperties().SetBounds(bounds);
    }
}

void SurfaceNodeCommandHelper::SetAbilityBGAlpha(RSContext& context, NodeId id, uint8_t alpha)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(id)) {
        node->SetAbilityBGAlpha(alpha);
    }
}

void SurfaceNodeCommandHelper::UpdateParentWithoutTransition(RSContext& context, NodeId nodeId, NodeId parentId)
{
    auto& nodeMap = context.GetNodeMap();
    auto node = nodeMap.GetRenderNode(nodeId);
    auto parent = nodeMap.GetRenderNode(parentId);
    if (node && parent) {
        node->RemoveFromTree(true);
        parent->AddChild(node);
    }
}

void SurfaceNodeCommandHelper::SetIsNotifyUIBufferAvailable(RSContext& context, NodeId id, bool available)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(id)) {
        node->SetIsNotifyUIBufferAvailable(available);
    }
}

void SurfaceNodeCommandHelper::SetAppFreeze(RSContext& context, NodeId nodeId, bool isAppFreeze)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId)) {
        node->SetAppFreeze(isAppFreeze);
    }
}

void SurfaceNodeCommandHelper::SetSurfaceNodeType(RSContext& context, NodeId nodeId, RSSurfaceNodeType type)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId)) {
        node->SetSurfaceNodeType(type);
    }
}

void SurfaceNodeCommandHelper::SetContainerWindow(RSContext& context, NodeId nodeId,
    bool hasContainerWindow, float density)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId)) {
        node->SetContainerWindow(hasContainerWindow, density);
    }
}

void SurfaceNodeCommandHelper::SetAnimationFinished(RSContext& context, NodeId nodeId)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId)) {
        node->SetStartAnimationFinished();
    }
}
} // namespace Rosen
} // namespace OHOS
