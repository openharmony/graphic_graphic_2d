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
#include "pipeline/rs_display_render_node.h"
#ifndef ROSEN_CROSS_PLATFORM
#include "surface_type.h"
#endif

namespace OHOS {
namespace Rosen {

void SurfaceNodeCommandHelper::Create(RSContext& context, NodeId id)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context.weak_from_this());
    auto& nodeMap = context.GetMutableNodeMap();
    nodeMap.RegisterRenderNode(node);
}

#ifndef USE_ROSEN_DRAWING
void SurfaceNodeCommandHelper::SetContextMatrix(RSContext& context, NodeId id, const std::optional<SkMatrix>& matrix)
#else
void SurfaceNodeCommandHelper::SetContextMatrix(
    RSContext& context, NodeId id, const std::optional<Drawing::Matrix>& matrix)
#endif
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

#ifndef USE_ROSEN_DRAWING
void SurfaceNodeCommandHelper::SetContextClipRegion(
    RSContext& context, NodeId id, const std::optional<SkRect>& clipRect)
#else
void SurfaceNodeCommandHelper::SetContextClipRegion(
    RSContext& context, NodeId id, const std::optional<Drawing::Rect>& clipRect)
#endif
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

void SurfaceNodeCommandHelper::SetFingerprint(RSContext& context, NodeId id, bool hasFingerprint)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(id)) {
        node->SetFingerprint(hasFingerprint);
    }
}

#ifndef ROSEN_CROSS_PLATFORM
void SurfaceNodeCommandHelper::SetColorSpace(RSContext& context, NodeId id, GraphicColorGamut colorSpace)
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

void SurfaceNodeCommandHelper::SetCallbackForRenderThreadRefresh(
    RSContext& context, NodeId id, bool isRefresh)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(id)) {
        if (node->NeedSetCallbackForRenderThreadRefresh()) {
            node->SetCallbackForRenderThreadRefresh(isRefresh);
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

void SurfaceNodeCommandHelper::SetIsNotifyUIBufferAvailable(RSContext& context, NodeId id, bool available)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(id)) {
        node->SetIsNotifyUIBufferAvailable(available);
    }
}

void SurfaceNodeCommandHelper::MarkUIHidden(RSContext& context, NodeId id, bool isHidden)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(id)) {
        node->MarkUIHidden(isHidden);
    }
}

void SurfaceNodeCommandHelper::SetSurfaceNodeType(RSContext& context, NodeId nodeId, uint8_t surfaceNodeType)
{
    auto type = static_cast<RSSurfaceNodeType>(surfaceNodeType);
    if (auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId)) {
        node->SetSurfaceNodeType(type);
    }
}

void SurfaceNodeCommandHelper::SetContainerWindow(
    RSContext& context, NodeId nodeId, bool hasContainerWindow, float density)
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

void SurfaceNodeCommandHelper::AttachToDisplay(RSContext& context, NodeId nodeId, uint64_t screenId)
{
    const auto& nodeMap = context.GetNodeMap();
    auto surfaceRenderNode = nodeMap.GetRenderNode<RSSurfaceRenderNode>(nodeId);
    nodeMap.TraverseDisplayNodes(
        [&surfaceRenderNode, &screenId](const std::shared_ptr<RSDisplayRenderNode>& displayRenderNode) {
            if (displayRenderNode == nullptr || displayRenderNode->GetScreenId() != screenId) {
                return;
            }
            displayRenderNode->AddChild(surfaceRenderNode);
        });
}

void SurfaceNodeCommandHelper::DetachToDisplay(RSContext& context, NodeId nodeId, uint64_t screenId)
{
    const auto& nodeMap = context.GetNodeMap();
    auto surfaceRenderNode = nodeMap.GetRenderNode<RSSurfaceRenderNode>(nodeId);
    nodeMap.TraverseDisplayNodes(
        [&surfaceRenderNode, &screenId](const std::shared_ptr<RSDisplayRenderNode>& displayRenderNode) {
            if (displayRenderNode == nullptr || displayRenderNode->GetScreenId() != screenId) {
                return;
            }
            displayRenderNode->RemoveChild(surfaceRenderNode);
        });
}
} // namespace Rosen
} // namespace OHOS
