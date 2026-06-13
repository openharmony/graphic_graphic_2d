/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "command_modifier/rs_node_command_modifier.h"

#include "ui/rs_node.h"

namespace OHOS {
namespace Rosen {

void OcclusionCullingStatusCmdModifier::UpdateToRender()
{
    auto node = GetNode();
    if (!node) return;
    std::unique_ptr<RSCommand> command = std::make_unique<RSUpdateOcclusionCullingStatus>(
        node->GetId(), param_.enablekeyOcclusion_, param_.keyOcclusionNodeId_);
    AddCommand(command, node->IsRenderServiceNode());
}

void NodeNameCmdModifier::UpdateToRender()
{
    auto node = GetNode();
    if (!node) return;
    std::unique_ptr<RSCommand> command = std::make_unique<RSSetNodeName>(
        node->GetId(), param_.nodeName_);
    AddCommand(command, node->IsRenderServiceNode());
}

void IsP3ColorCmdModifier::UpdateToRender()
{
    auto node = GetNode();
    if (!node) return;
    std::unique_ptr<RSCommand> command = std::make_unique<RSMarkNodeColorSpace>(
        node->GetId(), param_.collectColorSpace_);
    AddCommand(command, node->IsRenderServiceNode());
}

void DrawRegionCmdModifier::UpdateToRender()
{
    auto node = GetNode();
    if (!node) return;
    std::unique_ptr<RSCommand> command = std::make_unique<RSSetDrawRegion>(
        node->GetId(), param_.drawRegion_);
    AddCommand(command, node->IsRenderServiceNode(), node->GetFollowType(), node->GetId());
}

void UseCmdlistDrawRegionCmdModifier::UpdateToRender()
{
    auto node = GetNode();
    if (!node) return;
    std::unique_ptr<RSCommand> command = std::make_unique<RSSetNeedUseCmdlistDrawRegion>(
        node->GetId(), param_.needUseCmdlistDrawRegion_);
    AddCommand(command, node->IsRenderServiceNode(), node->GetFollowType(), node->GetId());
}

void ExcludeNodeGroupCmdModifier::UpdateToRender()
{
    auto node = GetNode();
    if (!node) return;
    std::unique_ptr<RSCommand> command = std::make_unique<RSExcludedFromNodeGroup>(
        node->GetId(), param_.isExcludedFromNodeGroup_);
    AddCommand(command, node->IsRenderServiceNode());
}

void MarkNodeSingleFrameComposerCmdModifier::UpdateToRender()
{
    auto node = GetNode();
    if (!node) return;
    std::unique_ptr<RSCommand> command = std::make_unique<RSMarkNodeSingleFrameComposer>(
        node->GetId(), param_.isNodeSingleFrameComposer_, param_.realPid_);
    AddCommand(command, node->IsRenderServiceNode());
}

void IsRepaintBoundaryCmdModifier::UpdateToRender()
{
    auto node = GetNode();
    if (!node) return;
    std::unique_ptr<RSCommand> command = std::make_unique<RSMarkRepaintBoundary>(
        node->GetId(), param_.isRepaintBoundary_);
    AddCommand(command, node->IsRenderServiceNode());
}

void MarkOpincNodeCmdModifier::UpdateToRender()
{
    auto node = GetNode();
    if (!node) return;
    std::unique_ptr<RSCommand> command = std::make_unique<RSMarkSuggestOpincNode>(
        node->GetId(), param_.isSuggestOpincNode_, param_.isOpincNeedCalculate_);
    AddCommand(command, node->IsRenderServiceNode());
}

void IsUifirstNodeCmdModifier::UpdateToRender()
{
    auto node = GetNode();
    if (!node) return;
    std::unique_ptr<RSCommand> command = std::make_unique<RSMarkUifirstNode>(
        node->GetId(), param_.isUifirstNode_);
    AddCommand(command, node->IsRenderServiceNode());
}

void IsForceUifirstNodeCmdModifier::UpdateToRender()
{
    auto node = GetNode();
    if (!node) return;
    std::unique_ptr<RSCommand> command = std::make_unique<RSForceUifirstNode>(
        node->GetId(), param_.isForceFlag_, param_.isUifirstEnable_);
    AddCommand(command, node->IsRenderServiceNode());
}

void SyncDrawNodeTypeCmdModifier::UpdateToRender()
{
    auto node = GetNode();
    if (!node) return;
    std::unique_ptr<RSCommand> command = std::make_unique<RSSetDrawNodeType>(
        node->GetId(), param_.nodeType_);
    AddCommand(command, node->IsRenderServiceNode());
}

void UIFirstSwitchCmdModifier::UpdateToRender()
{
    auto node = GetNode();
    if (!node) return;
    std::unique_ptr<RSCommand> command = std::make_unique<RSSetUIFirstSwitch>(
        node->GetId(), param_.uifirstSwitch_);
    AddCommand(command, node->IsRenderServiceNode());
}

void OutOfParentCmdModifier::UpdateToRender()
{
    auto node = GetNode();
    if (!node) return;
    std::unique_ptr<RSCommand> command = std::make_unique<RSSetOutOfParent>(
        node->GetId(), param_.outOfParent_);
    AddCommand(command, node->IsRenderServiceNode());
}

void IsCrossNodeCmdModifier::UpdateToRender()
{
    auto node = GetNode();
    if (!node) return;
    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeSetIsCrossNode>(
        node->GetId(), param_.isCrossNode_);
    AddCommand(command, node->IsRenderServiceNode());
}

void NodeGroupCmdModifier::UpdateToRender()
{
    auto node = GetNode();
    if (!node) return;
    std::unique_ptr<RSCommand> command = std::make_unique<RSMarkNodeGroup>(
        node->GetId(), param_.isNodeGroup_, param_.nodeGroupIsForced_, param_.nodeGroupIncludeProperty_);
    AddCommand(command, node->IsRenderServiceNode());
}

void OcclusionCullingStatusCmdModifier::DumpParam(std::string& out) const
{
    out += "{enablekeyOcclusion:" + std::string(param_.enablekeyOcclusion_ ? "true" : "false") +
           ", keyOcclusionNodeId:" + std::to_string(param_.keyOcclusionNodeId_) + "}";
}

void NodeNameCmdModifier::DumpParam(std::string& out) const
{
    out += "{nodeName:" + param_.nodeName_ + "}";
}

void IsP3ColorCmdModifier::DumpParam(std::string& out) const
{
    out += "{collectColorSpace:" + std::to_string(param_.collectColorSpace_) + "}";
}

void DrawRegionCmdModifier::DumpParam(std::string& out) const
{
    out += "{drawRegion:";
    if (param_.drawRegion_) {
        out += param_.drawRegion_->ToString();
    } else {
        out += "null";
    }
    out += "}";
}

void UseCmdlistDrawRegionCmdModifier::DumpParam(std::string& out) const
{
    out += "{needUseCmdlistDrawRegion:" + std::string(param_.needUseCmdlistDrawRegion_ ? "true" : "false") + "}";
}

void ExcludeNodeGroupCmdModifier::DumpParam(std::string& out) const
{
    out += "{isExcludedFromNodeGroup:" + std::string(param_.isExcludedFromNodeGroup_ ? "true" : "false") + "}";
}

void MarkNodeSingleFrameComposerCmdModifier::DumpParam(std::string& out) const
{
    out += "{isNodeSingleFrameComposer:" + std::string(param_.isNodeSingleFrameComposer_ ? "true" : "false") +
           ", realPid:" + std::to_string(param_.realPid_) + "}";
}

void IsRepaintBoundaryCmdModifier::DumpParam(std::string& out) const
{
    out += "{isRepaintBoundary:" + std::string(param_.isRepaintBoundary_ ? "true" : "false") + "}";
}

void MarkOpincNodeCmdModifier::DumpParam(std::string& out) const
{
    out += "{isSuggestOpincNode:" + std::string(param_.isSuggestOpincNode_ ? "true" : "false") +
           ", isOpincNeedCalculate:" + std::string(param_.isOpincNeedCalculate_ ? "true" : "false") + "}";
}

void IsUifirstNodeCmdModifier::DumpParam(std::string& out) const
{
    out += "{isUifirstNode:" + std::string(param_.isUifirstNode_ ? "true" : "false") + "}";
}

void IsForceUifirstNodeCmdModifier::DumpParam(std::string& out) const
{
    out += "{isForceFlag:" + std::string(param_.isForceFlag_ ? "true" : "false") +
           ", isUifirstEnable:" + std::string(param_.isUifirstEnable_ ? "true" : "false") + "}";
}

void SyncDrawNodeTypeCmdModifier::DumpParam(std::string& out) const
{
    out += "{nodeType:" + std::to_string(static_cast<int>(param_.nodeType_)) + "}";
}

void UIFirstSwitchCmdModifier::DumpParam(std::string& out) const
{
    out += "{uifirstSwitch:" + std::to_string(static_cast<int>(param_.uifirstSwitch_)) + "}";
}

void OutOfParentCmdModifier::DumpParam(std::string& out) const
{
    out += "{outOfParent:" + std::to_string(static_cast<int>(param_.outOfParent_)) + "}";
}

void IsCrossNodeCmdModifier::DumpParam(std::string& out) const
{
    out += "{isCrossNode:" + std::string(param_.isCrossNode_ ? "true" : "false") + "}";
}

void NodeGroupCmdModifier::DumpParam(std::string& out) const
{
    out += "{isNodeGroup:" + std::string(param_.isNodeGroup_ ? "true" : "false") +
           ", nodeGroupIsForced:" + std::string(param_.nodeGroupIsForced_ ? "true" : "false") +
           ", nodeGroupIncludeProperty:" + std::string(param_.nodeGroupIncludeProperty_ ? "true" : "false") + "}";
}

} // namespace Rosen
} // namespace OHOS
