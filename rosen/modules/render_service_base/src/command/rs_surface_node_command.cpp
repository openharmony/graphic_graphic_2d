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
#include "pipeline/rs_render_node_gc.h"
#include "platform/common/rs_log.h"
#ifndef ROSEN_CROSS_PLATFORM
#include "surface_type.h"
#endif

namespace OHOS {
namespace Rosen {
namespace {
constexpr const char* ARKTS_CARD_NODE = "ArkTSCardNode";
constexpr const char* SYSTEM_APP = "";
};

void SurfaceNodeCommandHelper::Create(RSContext& context, NodeId id, RSSurfaceNodeType type, bool isTextureExportNode)
{
    if (!context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(id)) {
        auto node = std::shared_ptr<RSSurfaceRenderNode>(new RSSurfaceRenderNode(id,
            context.weak_from_this(), isTextureExportNode), RSRenderNodeGC::NodeDestructor);
        node->SetSurfaceNodeType(type);
        auto& nodeMap = context.GetMutableNodeMap();
        nodeMap.RegisterRenderNode(node);
    }
}

void SurfaceNodeCommandHelper::CreateWithConfig(
    RSContext& context, NodeId nodeId, std::string name, uint8_t type, std::string bundleName)
{
    RSSurfaceRenderNodeConfig config = {
        .id = nodeId, .name = name, .bundleName = bundleName, .nodeType = static_cast<RSSurfaceNodeType>(type)
    };
    auto node = std::shared_ptr<RSSurfaceRenderNode>(new RSSurfaceRenderNode(config,
        context.weak_from_this()), RSRenderNodeGC::NodeDestructor);
    context.GetMutableNodeMap().RegisterRenderNode(node);
}

void SurfaceNodeCommandHelper::SetContextMatrix(
    RSContext& context, NodeId id, const std::optional<Drawing::Matrix>& matrix)
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

void SurfaceNodeCommandHelper::SetContextClipRegion(
    RSContext& context, NodeId id, const std::optional<Drawing::Rect>& clipRect)
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

void SurfaceNodeCommandHelper::SetIsTextureExportNode(RSContext& context, NodeId id, bool isTextureExportNode)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(id)) {
        node->SetIsTextureExportNode(isTextureExportNode);
    }
}

void SurfaceNodeCommandHelper::SetSkipLayer(RSContext& context, NodeId id, bool isSkipLayer)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(id)) {
        node->SetSkipLayer(isSkipLayer);
    }
}

void SurfaceNodeCommandHelper::SetFingerprint(RSContext& context, NodeId id, bool hasFingerprint)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(id)) {
        node->SetFingerprint(hasFingerprint);
    }
}

void SurfaceNodeCommandHelper::SetColorSpace(RSContext& context, NodeId id, GraphicColorGamut colorSpace)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(id)) {
        node->SetColorSpace(colorSpace);
    }
}

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
        if ((type == RSSurfaceNodeType::ABILITY_COMPONENT_NODE) && (node->GetName() != ARKTS_CARD_NODE) &&
            (node->GetName().find(SYSTEM_APP) == std::string::npos)) {
            auto& nodeMap = context.GetMutableNodeMap();
            nodeMap.CalCulateAbilityComponentNumsInProcess(nodeId);
        }
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
    if (surfaceRenderNode == nullptr) {
        return;
    }
    nodeMap.TraverseDisplayNodes(
        [&surfaceRenderNode, &screenId](const std::shared_ptr<RSDisplayRenderNode>& displayRenderNode) {
            if (displayRenderNode == nullptr || displayRenderNode->GetScreenId() != screenId ||
                displayRenderNode->GetBootAnimation() != surfaceRenderNode->GetBootAnimation()) {
                return;
            }
            displayRenderNode->AddChild(surfaceRenderNode);
        });
}

void SurfaceNodeCommandHelper::DetachToDisplay(RSContext& context, NodeId nodeId, uint64_t screenId)
{
    const auto& nodeMap = context.GetNodeMap();
    auto surfaceRenderNode = nodeMap.GetRenderNode<RSSurfaceRenderNode>(nodeId);
    if (surfaceRenderNode == nullptr) {
        return;
    }
    nodeMap.TraverseDisplayNodes(
        [&surfaceRenderNode, &screenId](const std::shared_ptr<RSDisplayRenderNode>& displayRenderNode) {
            if (displayRenderNode == nullptr || displayRenderNode->GetScreenId() != screenId ||
                displayRenderNode->GetBootAnimation() != surfaceRenderNode->GetBootAnimation()) {
                return;
            }
            displayRenderNode->RemoveChild(surfaceRenderNode);
        });
}

void SurfaceNodeCommandHelper::SetBootAnimation(RSContext& context, NodeId nodeId, bool isBootAnimation)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId)) {
        node->SetBootAnimation(isBootAnimation);
    }
}

#ifdef USE_SURFACE_TEXTURE
void SurfaceNodeCommandHelper::CreateSurfaceExt(RSContext& context, NodeId id,
    const std::shared_ptr<RSSurfaceTexture>& surfaceExt)
{
    auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(id);
    if (node != nullptr) {
        node->SetSurfaceTexture(surfaceExt);
    }
}
#endif

void SurfaceNodeCommandHelper::SetForceHardwareAndFixRotation(RSContext& context, NodeId nodeId, bool flag)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId)) {
        node->SetForceHardwareAndFixRotation(flag);
    }
}

void SurfaceNodeCommandHelper::SetForeground(RSContext& context, NodeId nodeId, bool isForeground)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId)) {
        node->SetForeground(isForeground);
    }
}

void SurfaceNodeCommandHelper::SetSurfaceId(RSContext& context, NodeId nodeId, SurfaceId surfaceId)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId)) {
        node->SetSurfaceId(surfaceId);
    }
}

void SurfaceNodeCommandHelper::SetForceUIFirst(RSContext& context, NodeId nodeId, bool forceUIFirst)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId)) {
        node->SetForceUIFirst(forceUIFirst);
    }
}

void SurfaceNodeCommandHelper::SetAncoForceDoDirect(RSContext& context, NodeId nodeId, bool ancoForceDoDirect)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId)) {
        node->SetAncoForceDoDirect(ancoForceDoDirect);
    }
}

void SurfaceNodeCommandHelper::SetHDRPresent(RSContext& context, NodeId nodeId, bool ancoForceDoDirect)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId)) {
        node->SetHDRPresent(ancoForceDoDirect);
    }
}
} // namespace Rosen
} // namespace OHOS
