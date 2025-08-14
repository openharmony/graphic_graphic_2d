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

#include "modifier_ng/rs_render_modifier_ng.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
namespace {
RSNodeCommandHelper::DumpNodeTreeProcessor gDumpNodeTreeProcessor = nullptr;
RSNodeCommandHelper::CommitDumpNodeTreeProcessor gCommitDumpNodeTreeProcessor = nullptr;
}

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

void RSNodeCommandHelper::RemoveAllModifiers(RSContext& context, NodeId nodeId)
{
    auto& nodeMap = context.GetNodeMap();
    auto node = nodeMap.GetRenderNode<RSRenderNode>(nodeId);
    if (node) {
        node->RemoveAllModifiers();
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

void RSNodeCommandHelper::MarkRepaintBoundary(RSContext& context, NodeId nodeId, bool isRepaintBoundary)
{
    auto& nodeMap = context.GetNodeMap();
    if (auto node = nodeMap.GetRenderNode<RSRenderNode>(nodeId)) {
        node->MarkRepaintBoundary(isRepaintBoundary);
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

void RSNodeCommandHelper::MarkSuggestOpincNode(RSContext& context, NodeId nodeId,
    bool isOpincNode, bool isNeedCalculate)
{
    auto& nodeMap = context.GetNodeMap();
    if (auto node = nodeMap.GetRenderNode<RSRenderNode>(nodeId)) {
        node->MarkSuggestOpincNode(isOpincNode, isNeedCalculate);
    }
}

void RSNodeCommandHelper::MarkUifirstNode(RSContext& context, NodeId nodeId, bool isUifirstNode)
{
    auto& nodeMap = context.GetNodeMap();
    if (auto node = nodeMap.GetRenderNode<RSRenderNode>(nodeId)) {
        node->MarkUifirstNode(isUifirstNode);
    }
}

void RSNodeCommandHelper::ForceUifirstNode(RSContext& context, NodeId nodeId, bool isForceFlag,
    bool isUifirstEnable)
{
    auto& nodeMap = context.GetNodeMap();
    if (auto node = nodeMap.GetRenderNode<RSRenderNode>(nodeId)) {
        node->MarkUifirstNode(isForceFlag, isUifirstEnable);
    }
}

void RSNodeCommandHelper::SetUIFirstSwitch(RSContext& context, NodeId nodeId, RSUIFirstSwitch uiFirstSwitch)
{
    auto& nodeMap = context.GetNodeMap();
    if (auto node = nodeMap.GetRenderNode<RSRenderNode>(nodeId)) {
        node->SetUIFirstSwitch(uiFirstSwitch);
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
    context.GetUiCaptureHelper().InsertUiCaptureCmdsExecutedFlag(nodeId, true);
}

void RSNodeCommandHelper::SetNeedUseCmdlistDrawRegion(RSContext& context, NodeId nodeId, bool needUseCmdlistDrawRegion)
{
    auto& nodeMap = context.GetNodeMap();
    auto node = nodeMap.GetRenderNode<RSRenderNode>(nodeId);
    if (node != nullptr) {
        node->SetNeedUseCmdlistDrawRegion(needUseCmdlistDrawRegion);
    }
}

void RSNodeCommandHelper::RegisterGeometryTransitionPair(RSContext& context, NodeId inNodeId, NodeId outNodeId,
    const bool isInSameWindow)
{
    auto& nodeMap = context.GetNodeMap();
    auto inNode = nodeMap.GetRenderNode<RSRenderNode>(inNodeId);
    auto outNode = nodeMap.GetRenderNode<RSRenderNode>(outNodeId);
    if (inNode == nullptr || outNode == nullptr) {
        return;
    }
    auto sharedTransitionParam = std::make_shared<SharedTransitionParam>(inNode, outNode, isInSameWindow);
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

void RSNodeCommandHelper::DumpClientNodeTree(
    RSContext& context, NodeId nodeId, pid_t pid, uint64_t token, uint32_t taskId)
{
    if (gDumpNodeTreeProcessor) {
        gDumpNodeTreeProcessor(nodeId, pid, token, taskId);
    }
}

void RSNodeCommandHelper::SetDumpNodeTreeProcessor(DumpNodeTreeProcessor processor)
{
    gDumpNodeTreeProcessor = processor;
}

void RSNodeCommandHelper::CommitDumpClientNodeTree(RSContext& context, NodeId nodeId, pid_t pid, uint32_t taskId,
    const std::string& result)
{
    if (gCommitDumpNodeTreeProcessor) {
        gCommitDumpNodeTreeProcessor(nodeId, pid, taskId, result);
    }
}

void RSNodeCommandHelper::SetCommitDumpNodeTreeProcessor(CommitDumpNodeTreeProcessor processor)
{
    gCommitDumpNodeTreeProcessor = processor;
}

void RSNodeCommandHelper::SetUIToken(RSContext& context, NodeId nodeId, uint64_t token)
{
    auto& nodeMap = context.GetNodeMap();
    auto node = nodeMap.GetRenderNode(nodeId);
    if (node) {
        node->SetUIContextToken(token);
    }
}

void RSNodeCommandHelper::SetDrawNodeType(RSContext& context, NodeId nodeId, DrawNodeType nodeType)
{
    auto& nodeMap = context.GetNodeMap();
    auto node = nodeMap.GetRenderNode<RSRenderNode>(nodeId);
    if (node) {
        node->SetDrawNodeType(nodeType);
    }
}

void RSNodeCommandHelper::UpdateOcclusionCullingStatus(RSContext& context, NodeId nodeId,
    bool enable, NodeId keyOcclusionNodeId)
{
    auto& nodeMap = context.GetNodeMap();
    if (auto node = nodeMap.GetRenderNode<RSRenderNode>(nodeId)) {
        auto instanceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(node->GetInstanceRootNode());
        if (instanceNode == nullptr) {
            return;
        }
        if (auto occlusionParams = instanceNode->GetOcclusionParams()) {
            occlusionParams->UpdateOcclusionCullingStatus(enable, keyOcclusionNodeId);
        }
    }
}

void RSNodeCommandHelper::AddModifierNG(RSContext& context, NodeId nodeId,
    const std::shared_ptr<ModifierNG::RSRenderModifier>& modifier)
{
    auto& nodeMap = context.GetNodeMap();
    auto node = nodeMap.GetRenderNode<RSRenderNode>(nodeId);
    if (node) {
        node->AddModifier(modifier);
    }
}

void RSNodeCommandHelper::RemoveModifierNG(
    RSContext& context, NodeId nodeId, ModifierNG::RSModifierType type, ModifierId id)
{
    auto& nodeMap = context.GetNodeMap();
    auto node = nodeMap.GetRenderNode<RSRenderNode>(nodeId);
    if (node) {
        node->RemoveModifier(type, id);
    }
}

void RSNodeCommandHelper::ModifierNGAttachProperty(RSContext& context, NodeId nodeId, ModifierId modifierId,
    ModifierNG::RSModifierType modifierType, ModifierNG::RSPropertyType propertyType,
    std::shared_ptr<RSRenderPropertyBase> prop)
{
    auto& nodeMap = context.GetNodeMap();
    auto node = nodeMap.GetRenderNode<RSRenderNode>(nodeId);
    if (!node) {
        return;
    }
    auto modifier = node->GetModifierNG(modifierType, modifierId);
    if (!modifier) {
        return;
    }
    modifier->AttachProperty(propertyType, prop);
}

void RSNodeCommandHelper::UpdateModifierNGDrawCmdList(
    RSContext& context, NodeId nodeId, Drawing::DrawCmdListPtr value, PropertyId propertyId)
{
    auto& nodeMap = context.GetNodeMap();
    auto node = nodeMap.GetRenderNode<RSRenderNode>(nodeId);
    if (!node) {
        return;
    }
    auto baseProperty = node->GetProperty(propertyId);
    if (!baseProperty) {
        return;
    }
    auto property = std::static_pointer_cast<RSRenderProperty<Drawing::DrawCmdListPtr>>(baseProperty);
    property->Set(value);
    if (value) {
        value->UpdateNodeIdToPicture(nodeId);
    }
}

void RSNodeCommandHelper::ModifierNGDetachProperty(RSContext& context, NodeId nodeId, ModifierId modifierId,
    ModifierNG::RSModifierType modifierType, ModifierNG::RSPropertyType propertyType)
{
    auto& nodeMap = context.GetNodeMap();
    auto node = nodeMap.GetRenderNode<RSRenderNode>(nodeId);
    if (!node) {
        return;
    }
    auto modifier = node->GetModifierNG(modifierType, modifierId);
    if (!modifier) {
        return;
    }
    modifier->DetachProperty(propertyType);
}

void RSNodeCommandHelper::RemoveAllModifiersNG(RSContext& context, NodeId nodeId)
{
    auto& nodeMap = context.GetNodeMap();
    auto node = nodeMap.GetRenderNode<RSRenderNode>(nodeId);
    if (node) {
        node->RemoveAllModifiersNG();
    }
}
} // namespace Rosen
} // namespace OHOS
