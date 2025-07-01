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

#include "command/rs_command_verify_helper.h"
#include "common/rs_vector4.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_logical_display_render_node.h"
#include "pipeline/rs_render_node_gc.h"
#include "platform/common/rs_log.h"
#ifndef ROSEN_CROSS_PLATFORM
#include "surface_type.h"
#endif

namespace OHOS {
namespace Rosen {

void SurfaceNodeCommandHelper::Create(RSContext& context, NodeId id, RSSurfaceNodeType type, bool isTextureExportNode)
{
    if (!context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(id)) {
        if (!RsCommandVerifyHelper::GetInstance().IsSurfaceNodeCreateCommandVaild(ExtractPid(id))) {
            ROSEN_LOGI("SurfaceNodeCommandHelper::Create command is not vaild because there "
            "have been too many surfaceNodes, nodeId:%{public}" PRIu64 "", id);
            return;
        }
        auto node = std::shared_ptr<RSSurfaceRenderNode>(new RSSurfaceRenderNode(id,
            context.weak_from_this(), isTextureExportNode), RSRenderNodeGC::NodeDestructor);
        node->SetSurfaceNodeType(type);
        auto& nodeMap = context.GetMutableNodeMap();
        nodeMap.RegisterRenderNode(node);
    }
}

void SurfaceNodeCommandHelper::CreateWithConfig(
    RSContext& context, NodeId nodeId, std::string name, uint8_t type, enum SurfaceWindowType windowType)
{
    RSSurfaceRenderNodeConfig config = {
        .id = nodeId, .name = name,
        .nodeType = static_cast<RSSurfaceNodeType>(type), .surfaceWindowType = windowType
    };
    if (!RsCommandVerifyHelper::GetInstance().IsSurfaceNodeCreateCommandVaild(ExtractPid(nodeId))) {
        ROSEN_LOGI("SurfaceNodeCommandHelper::CreateWithConfig command is not vaild because there "
            "have been too many surfaceNodes, nodeId:%{public}" PRIu64 "", nodeId);
        return;
    }
    auto node = std::shared_ptr<RSSurfaceRenderNode>(new RSSurfaceRenderNode(config,
        context.weak_from_this()), RSRenderNodeGC::NodeDestructor);
    context.GetMutableNodeMap().RegisterRenderNode(node);
}

std::shared_ptr<RSSurfaceRenderNode> SurfaceNodeCommandHelper::CreateWithConfigInRS(
    const RSSurfaceRenderNodeConfig& config, RSContext& context, bool unobscured)
{
    if (!RsCommandVerifyHelper::GetInstance().IsSurfaceNodeCreateCommandVaild(ExtractPid(config.id))) {
        ROSEN_LOGI("SurfaceNodeCommandHelper::CreateWithConfigInRS command is not vaild because there have "
            "been too many surfaceNodes, nodeId:%{public}" PRIu64 "", config.id);
        return nullptr;
    }
    auto node = std::shared_ptr<RSSurfaceRenderNode>(new RSSurfaceRenderNode(config,
        context.weak_from_this()), RSRenderNodeGC::NodeDestructor);
    node->SetUIExtensionUnobscured(unobscured);
    return node;
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

void SurfaceNodeCommandHelper::SetLeashPersistentId(RSContext& context, NodeId id, LeashPersistentId leashPersistentId)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(id)) {
        node->SetLeashPersistentId(leashPersistentId);
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

void SurfaceNodeCommandHelper::SetSnapshotSkipLayer(RSContext& context, NodeId id, bool isSnapshotSkipLayer)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(id)) {
        node->SetSnapshotSkipLayer(isSnapshotSkipLayer);
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
        node->SetSurfaceNodeType(type);
    }
}

void SurfaceNodeCommandHelper::SetContainerWindow(
    RSContext& context, NodeId nodeId, bool hasContainerWindow, RRect rrect)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId)) {
        node->SetContainerWindow(hasContainerWindow, rrect);
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
    nodeMap.TraverseLogicalDisplayNodes(
        [&surfaceRenderNode, &screenId](const std::shared_ptr<RSLogicalDisplayRenderNode>& logicalDisplayRenderNode) {
            if (logicalDisplayRenderNode == nullptr || logicalDisplayRenderNode->GetScreenId() != screenId ||
                logicalDisplayRenderNode->GetBootAnimation() != surfaceRenderNode->GetBootAnimation() ||
                !logicalDisplayRenderNode->IsOnTheTree()) {
                return;
            }
            logicalDisplayRenderNode->AddChild(surfaceRenderNode);
        });
}

void SurfaceNodeCommandHelper::DetachToDisplay(RSContext& context, NodeId nodeId, uint64_t screenId)
{
    const auto& nodeMap = context.GetNodeMap();
    auto surfaceRenderNode = nodeMap.GetRenderNode<RSSurfaceRenderNode>(nodeId);
    if (surfaceRenderNode == nullptr) {
        return;
    }
    nodeMap.TraverseLogicalDisplayNodes(
        [&surfaceRenderNode, &screenId](const std::shared_ptr<RSLogicalDisplayRenderNode>& logicalDisplayRenderNode) {
            if (logicalDisplayRenderNode == nullptr || logicalDisplayRenderNode->GetScreenId() != screenId ||
                logicalDisplayRenderNode->GetBootAnimation() != surfaceRenderNode->GetBootAnimation()) {
                return;
            }
            logicalDisplayRenderNode->RemoveChild(surfaceRenderNode);
        });
}

void SurfaceNodeCommandHelper::SetBootAnimation(RSContext& context, NodeId nodeId, bool isBootAnimation)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId)) {
        node->SetBootAnimation(isBootAnimation);
    }
}

void SurfaceNodeCommandHelper::SetGlobalPositionEnabled(RSContext& context, NodeId nodeId, bool isEnabled)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId)) {
        node->SetGlobalPositionEnabled(isEnabled);
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

void SurfaceNodeCommandHelper::SetClonedNodeInfo(
    RSContext& context, NodeId nodeId, NodeId cloneNodeId, bool needOffscreen)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId)) {
        node->SetClonedNodeInfo(cloneNodeId, needOffscreen);
    }
}

void SurfaceNodeCommandHelper::SetForceUIFirst(RSContext& context, NodeId nodeId, bool forceUIFirst)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId)) {
        node->SetForceUIFirst(forceUIFirst);
    }
}

void SurfaceNodeCommandHelper::SetAncoFlags(RSContext& context, NodeId nodeId, uint32_t flags)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId)) {
        node->SetAncoFlags(flags);
    }
}

void SurfaceNodeCommandHelper::SetHDRPresent(RSContext& context, NodeId nodeId, bool hdrPresent)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId)) {
        node->SetHDRPresent(hdrPresent);
    }
}

void SurfaceNodeCommandHelper::SetSkipDraw(RSContext& context, NodeId nodeId, bool skip)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId)) {
        node->SetSkipDraw(skip);
    }
}

void SurfaceNodeCommandHelper::SetWatermarkEnabled(RSContext& context, NodeId nodeId,
    const std::string& name, bool isEnabled)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId)) {
        node->SetWatermarkEnabled(name, isEnabled);
    }
}

void SurfaceNodeCommandHelper::SetAbilityState(RSContext& context, NodeId nodeId,
    RSSurfaceNodeAbilityState abilityState)
{
    auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId);
    if (!node) {
        ROSEN_LOGE("SurfaceNodeCommandHelper::SetAbilityState node is null!");
        return;
    }
    node->SetAbilityState(abilityState);
}

void SurfaceNodeCommandHelper::SetApiCompatibleVersion(RSContext& context, NodeId nodeId, uint32_t apiCompatibleVersion)
{
    auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId);
    if (!node) {
        RS_LOGE("SurfaceNodeCommandHelper::SetApiCompatibleVersion node is null!");
        return;
    }
    node->SetApiCompatibleVersion(apiCompatibleVersion);
}

void SurfaceNodeCommandHelper::SetHardwareEnableHint(RSContext& context, NodeId nodeId, bool enable)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId)) {
        node->SetHardwareEnableHint(enable);
    }
}

void SurfaceNodeCommandHelper::SetSourceVirtualDisplayId(RSContext& context, NodeId nodeId, ScreenId screenId)
{
    if (auto surfaceRenderNode = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId)) {
        const auto& nodeMap = context.GetNodeMap();
        nodeMap.TraverseScreenNodes(
            [surfaceRenderNode, screenId](const std::shared_ptr<RSScreenRenderNode>& screenRenderNode) {
                if (screenRenderNode != nullptr && screenRenderNode->GetScreenId() == screenId) {
                    surfaceRenderNode->SetSourceDisplayRenderNodeId(screenRenderNode->GetId());
                }
            }
        );
    }
}

void SurfaceNodeCommandHelper::AttachToWindowContainer(RSContext& context, NodeId nodeId, ScreenId screenId)
{
    const auto& nodeMap = context.GetNodeMap();
    auto surfaceRenderNode = nodeMap.GetRenderNode<RSSurfaceRenderNode>(nodeId);
    if (surfaceRenderNode == nullptr) {
        RS_LOGE("SurfaceNodeCommandHelper::AttachToWindowContainer Invalid surfaceRenderNode");
        return;
    }
    nodeMap.TraverseLogicalDisplayNodes(
        [surfaceRenderNode, screenId](const std::shared_ptr<RSLogicalDisplayRenderNode>& displayRenderNode) {
            if (displayRenderNode == nullptr || displayRenderNode->GetScreenId() != screenId ||
                displayRenderNode->GetBootAnimation() != surfaceRenderNode->GetBootAnimation()) {
                return;
            }
            auto windowContainer = displayRenderNode->GetWindowContainer();
            if (windowContainer == nullptr || !windowContainer->IsOnTheTree()) {
                displayRenderNode->AddChild(surfaceRenderNode);
                RS_LOGD("SurfaceNodeCommandHelper::AttachToWindowContainer %{public}" PRIu64 " attach to %{public}"
                    PRIu64, surfaceRenderNode->GetId(), displayRenderNode->GetId());
            } else {
                windowContainer->AddChild(surfaceRenderNode);
                RS_LOGD("SurfaceNodeCommandHelper::AttachToWindowContainer %{public}" PRIu64 " attach to %{public}"
                    PRIu64, surfaceRenderNode->GetId(), windowContainer->GetId());
            }
        }
    );
}

void SurfaceNodeCommandHelper::DetachFromWindowContainer(RSContext& context, NodeId nodeId, ScreenId screenId)
{
    const auto& nodeMap = context.GetNodeMap();
    auto surfaceRenderNode = nodeMap.GetRenderNode<RSSurfaceRenderNode>(nodeId);
    if (surfaceRenderNode == nullptr) {
        RS_LOGE("SurfaceNodeCommandHelper::DetachFromWindowContainer Invalid surfaceRenderNode");
        return;
    }
    nodeMap.TraverseLogicalDisplayNodes(
        [surfaceRenderNode, screenId](const std::shared_ptr<RSLogicalDisplayRenderNode>& displayRenderNode) {
            if (displayRenderNode == nullptr || displayRenderNode->GetScreenId() != screenId ||
                displayRenderNode->GetBootAnimation() != surfaceRenderNode->GetBootAnimation()) {
                return;
            }
            auto windowContainer = displayRenderNode->GetWindowContainer();
            if (windowContainer == nullptr || !windowContainer->IsOnTheTree()) {
                displayRenderNode->RemoveChild(surfaceRenderNode);
                RS_LOGD("SurfaceNodeCommandHelper::DetachFromWindowContainer %{public}" PRIu64 " detach from %{public}"
                    PRIu64, surfaceRenderNode->GetId(), displayRenderNode->GetId());
            } else {
                windowContainer->RemoveChild(surfaceRenderNode);
                RS_LOGD("SurfaceNodeCommandHelper::DetachFromWindowContainer %{public}" PRIu64 " detach from %{public}"
                    PRIu64, surfaceRenderNode->GetId(), windowContainer->GetId());
            }
        }
    );
}

void SurfaceNodeCommandHelper::SetRegionToBeMagnified(RSContext& context, NodeId id, Vector4f regionToBeMagnified)
{
    if (auto surfaceRenderNode = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(id)) {
        surfaceRenderNode->SetRegionToBeMagnified(regionToBeMagnified);
        RS_LOGI_LIMIT("SurfaceNodeCommandHelper::SetRegionToBeMagnified, regionToBeMagnified left=%f, top=%f, width=%f, hight=%f",
            regionToBeMagnified.x_, regionToBeMagnified.y_, regionToBeMagnified.z_, regionToBeMagnified.w_);
    }
}

void SurfaceNodeCommandHelper::SetFrameGravityNewVersionEnabled(RSContext& context, NodeId nodeId, bool isEnabled)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId)) {
        node->SetFrameGravityNewVersionEnabled(isEnabled);
    }
}

void SurfaceNodeCommandHelper::SetAncoSrcCrop(RSContext& context, NodeId nodeId, const Rect& srcCrop)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId)) {
        node->SetAncoSrcCrop(srcCrop);
    }
}
} // namespace Rosen
} // namespace OHOS
