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

#include "command_modifier/rs_surface_node_command_modifier.h"

#include "command/rs_spatial_effect_command.h"
#include "command/rs_surface_node_command.h"
#include "common/rs_optional_trace.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_render_interface.h"
#include "transaction/rs_render_service_client.h"
#include "ui/rs_node.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_ui_context.h"

namespace OHOS {
namespace Rosen {

void LeashPersistentIdCmdModifier::UpdateToRender()
{
    auto node = std::static_pointer_cast<RSSurfaceNode>(GetNode());
    if (!node) return;
    std::unique_ptr<RSCommand> command = std::make_unique<RSurfaceNodeSetLeashPersistentId>(
        node->GetId(), param_.leashPersistentId_);
    AddCommand(command, true);
}

void SecurityLayerCmdModifier::UpdateToRender()
{
    auto node = std::static_pointer_cast<RSSurfaceNode>(GetNode());
    if (!node) return;
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetSecurityLayer>(
        node->GetId(), param_.isSecurityLayer_);
    AddCommand(command, true);
}

void SkipLayerCmdModifier::UpdateToRender()
{
    auto node = std::static_pointer_cast<RSSurfaceNode>(GetNode());
    if (!node) return;
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetSkipLayer>(
        node->GetId(), param_.isSkipLayer_);
    AddCommand(command, true);
}

void SnapshotSkipLayerCmdModifier::UpdateToRender()
{
    auto node = std::static_pointer_cast<RSSurfaceNode>(GetNode());
    if (!node) return;
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetSnapshotSkipLayer>(
        node->GetId(), param_.isSnapshotSkipLayer_);
    AddCommand(command, true);
}

void HasFingerprintCmdModifier::UpdateToRender()
{
    auto node = std::static_pointer_cast<RSSurfaceNode>(GetNode());
    if (!node) return;
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetFingerprint>(
        node->GetId(), param_.hasFingerprint_);
    AddCommand(command, true);
}

void ColorSpaceCmdModifier::UpdateToRender()
{
    auto node = std::static_pointer_cast<RSSurfaceNode>(GetNode());
    if (!node) return;
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetColorSpace>(
        node->GetId(), param_.colorSpace_);
    AddCommand(command, true);
}

void AbilityBGAlphaCmdModifier::UpdateToRender()
{
    auto node = std::static_pointer_cast<RSSurfaceNode>(GetNode());
    if (!node) return;
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetAbilityBGAlpha>(
        node->GetId(), param_.abilityBGAlpha_);
    AddCommand(command, true);
}

void NotifyUIBufferAvailableCmdModifier::UpdateToRender()
{
    auto node = std::static_pointer_cast<RSSurfaceNode>(GetNode());
    if (!node) return;
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetIsNotifyUIBufferAvailable>(
        node->GetId(), param_.notifyUIBufferAvailable_);
    AddCommand(command, true);
}

void ContainerWindowCmdModifier::UpdateToRender()
{
    auto node = std::static_pointer_cast<RSSurfaceNode>(GetNode());
    if (!node) return;
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetContainerWindow>(
        node->GetId(), param_.hasContainerWindow_, param_.containerWindowRrect_);
    AddCommand(command, true);
}

void FreezeCmdModifier::UpdateToRender()
{
    auto node = std::static_pointer_cast<RSSurfaceNode>(GetNode());
    if (!node) return;
    RS_OPTIONAL_TRACE_NAME_FMT("RSSurfaceNode::SetFreeze id:%llu", node->GetId());
    std::unique_ptr<RSCommand> command = std::make_unique<RSSetFreeze>(
        node->GetId(), param_.isFreeze_, param_.isMarkedByUI_);
    AddCommand(command, true);
}

void BootAnimationCmdModifier::UpdateToRender()
{
    auto node = std::static_pointer_cast<RSSurfaceNode>(GetNode());
    if (!node) return;
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetBootAnimation>(
        node->GetId(), param_.isBootAnimation_);
    AddCommand(command, true);
}

void GlobalPositionEnabledCmdModifier::UpdateToRender()
{
    auto node = std::static_pointer_cast<RSSurfaceNode>(GetNode());
    if (!node) return;
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetGlobalPositionEnabled>(
        node->GetId(), param_.isGlobalPositionEnabled_);
    AddCommand(command, true);
}

void ClonedNodeInfoCmdModifier::UpdateToRender()
{
    auto node = std::static_pointer_cast<RSSurfaceNode>(GetNode());
    if (!node) return;
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetClonedNodeId>(
        node->GetId(), param_.clonedNodeId_, param_.clonedNeedOffscreen_, param_.clonedIsRelated_);
    AddCommand(command, true);
}

void ForceUIFirstCmdModifier::UpdateToRender()
{
    auto node = std::static_pointer_cast<RSSurfaceNode>(GetNode());
    if (!node) return;
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetForceUIFirst>(
        node->GetId(), param_.forceUIFirst_);
    AddCommand(command, true);
}

void AncoFlagsCmdModifier::UpdateToRender()
{
    auto node = std::static_pointer_cast<RSSurfaceNode>(GetNode());
    if (!node) return;
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetAncoFlags>(
        node->GetId(), param_.ancoFlags_);
    AddCommand(command, true);
}

void SkipDrawCmdModifier::UpdateToRender()
{
    auto node = std::static_pointer_cast<RSSurfaceNode>(GetNode());
    if (!node) return;
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetSkipDraw>(
        node->GetId(), param_.skipDraw_);
    AddCommand(command, true);
}

void WatermarkEnabledCmdModifier::UpdateToRender()
{
    auto node = std::static_pointer_cast<RSSurfaceNode>(GetNode());
    if (!node) return;
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetWatermarkEnabled>(
        node->GetId(), param_.watermarkName_, param_.watermarkEnabled_);
    AddCommand(command, true);
}

void AbilityStateCmdModifier::UpdateToRender()
{
    auto node = std::static_pointer_cast<RSSurfaceNode>(GetNode());
    if (!node) return;
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetAbilityState>(
        node->GetId(), param_.abilityState_);
    AddCommand(command, true);
}

void HidePrivacyContentCmdModifier::UpdateToRender()
{
    auto node = std::static_pointer_cast<RSSurfaceNode>(GetNode());
    if (!node) return;
    // To be implemented
}

RSCmdModifier::UpdateResult HidePrivacyContentCmdModifier::UpdateToRenderWithResult()
{
    auto node = std::static_pointer_cast<RSSurfaceNode>(GetNode());
    if (!node) {
        return RSInterfaceErrorCode::UNKNOWN_ERROR;
    }
    auto rsUIContext = node->GetRSUIContext();
    if (rsUIContext == nullptr || rsUIContext->GetRSRenderInterface() == nullptr) {
        RS_LOGE("HidePrivacyContentCmdModifier::UpdateToRenderWithResult: uiContext is nullptr");
        return RSInterfaceErrorCode::UNKNOWN_ERROR;
    }
    return rsUIContext->GetRSRenderInterface()->SetHidePrivacyContent(node->GetId(), param_.hidePrivacyContent_);
}

void ApiCompatibleVersionCmdModifier::UpdateToRender()
{
    auto node = std::static_pointer_cast<RSSurfaceNode>(GetNode());
    if (!node) return;
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetApiCompatibleVersion>(
        node->GetId(), param_.apiCompatibleVersion_);
    AddCommand(command, true);
}

void HardwareEnabledCmdModifier::UpdateToRender()
{
    auto node = std::static_pointer_cast<RSSurfaceNode>(GetNode());
    if (!node) return;
    // To be implemented
}

RSCmdModifier::UpdateResult HardwareEnabledCmdModifier::UpdateToRenderWithResult()
{
    auto node = std::static_pointer_cast<RSSurfaceNode>(GetNode());
    if (!node) {
        return false;
    }
    auto rsUIContext = node->GetRSUIContext();
    if (rsUIContext == nullptr || rsUIContext->GetRSRenderInterface() == nullptr) {
        ROSEN_LOGE("RSSurfaceNode::SetHardwareEnabled uiContext is nullptr");
        return false;
    }
    rsUIContext->GetRSRenderInterface()->SetHardwareEnabled(
        node->GetId(), param_.isHardwareEnabled_, param_.isSelfDrawingNodeType_, param_.isDynamicHardwareEnable_);
    return true;
}

void HardwareEnableHintCmdModifier::UpdateToRender()
{
    auto node = std::static_pointer_cast<RSSurfaceNode>(GetNode());
    if (!node) return;
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetHardwareEnableHint>(
        node->GetId(), param_.isHardwareEnabled_);
    AddCommand(command, true);
}

void VirtualDisplayIdCmdModifier::UpdateToRender()
{
    auto node = std::static_pointer_cast<RSSurfaceNode>(GetNode());
    if (!node) return;
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetSourceVirtualScreenId>(
        node->GetId(), param_.virtualDisplayId_);
    AddCommand(command, true);
}

void AttachToWindowContainerCmdModifier::UpdateToRender()
{
    auto node = std::static_pointer_cast<RSSurfaceNode>(GetNode());
    if (!node) return;
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeAttachToWindowContainer>(
        node->GetId(), param_.attachToWindowContainer_);
    AddCommand(command, true);
}

void RegionToBeMagnifiedCmdModifier::UpdateToRender()
{
    auto node = std::static_pointer_cast<RSSurfaceNode>(GetNode());
    if (!node) return;
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetRegionToBeMagnified>(
        node->GetId(), param_.regionToBeMagnified_);
    AddCommand(command, true);
}

void DetachFromWindowContainerCmdModifier::UpdateToRender()
{
    auto node = std::static_pointer_cast<RSSurfaceNode>(GetNode());
    if (!node) return;
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeDetachFromWindowContainer>(
        node->GetId(), param_.detachFromWindowContainer_);
    AddCommand(command, true);
}

void CompositeLayerCmdModifier::UpdateToRender()
{
    auto node = std::static_pointer_cast<RSSurfaceNode>(GetNode());
    if (!node) {
        return;
    }
    // To be implemented
}

RSCmdModifier::UpdateResult CompositeLayerCmdModifier::UpdateToRenderWithResult()
{
    auto node = std::static_pointer_cast<RSSurfaceNode>(GetNode());
    if (!node) {
        return false;
    }
    if (auto rsUIContext = node->GetRSUIContext()) {
        if (auto renderInterface = rsUIContext->GetRSRenderInterface()) {
            renderInterface->SetLayerTopForHWC(node->GetId(), param_.isCompositeLayer_, param_.isMarkedByUI_);
        }
    }
    return true;
}

void StaticCachedCmdModifier::UpdateToRender()
{
    auto node = std::static_pointer_cast<RSSurfaceNode>(GetNode());
    if (!node) return;
    // To be implemented
}

void FrameGravityNewVersionEnabledCmdModifier::UpdateToRender()
{
    auto node = std::static_pointer_cast<RSSurfaceNode>(GetNode());
    if (!node) return;
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetFrameGravityNewVersionEnabled>(
        node->GetId(), param_.isFrameGravityNewVersionEnabled_);
    AddCommand(command, true);
}

void SurfaceBufferOpaqueCmdModifier::UpdateToRender()
{
    auto node = std::static_pointer_cast<RSSurfaceNode>(GetNode());
    if (!node) return;
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetSurfaceBufferOpaque>(
        node->GetId(), param_.isSurfaceBufferOpaque_);
    AddCommand(command, true);
}

void ContainerWindowTransparentCmdModifier::UpdateToRender()
{
    auto node = std::static_pointer_cast<RSSurfaceNode>(GetNode());
    if (!node) return;
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetContainerWindowTransparent>(
        node->GetId(), param_.isContainerWindowTransparent_);
    AddCommand(command, true);
}

void BufferAvailableCallbackCmdModifier::UpdateToRender()
{
    auto node = std::static_pointer_cast<RSSurfaceNode>(GetNode());
    if (!node) return;
    // To be implemented
}

RSCmdModifier::UpdateResult BufferAvailableCallbackCmdModifier::UpdateToRenderWithResult()
{
    auto node = std::static_pointer_cast<RSSurfaceNode>(GetNode());
    if (!node) {
        return false;
    }
    auto rsUIContext = node->GetRSUIContext();
    if (rsUIContext == nullptr || rsUIContext->GetRSRenderInterface() == nullptr) {
        RS_LOGE("BufferAvailableCallbackCmdModifier::UpdateToRenderWithResult: uiContext is nullptr");
        return false;
    }
    return rsUIContext->GetRSRenderInterface()->RegisterBufferAvailableListener(node->GetId(), param_.callback, false);
}

void SurfaceDefaultSizeCmdModifier::UpdateToRender()
{
    auto node = std::static_pointer_cast<RSSurfaceNode>(GetNode());
    if (!node) return;
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeUpdateSurfaceDefaultSize>(
        node->GetId(), param_.width_, param_.height_);
    AddCommand(command, true);
}

void LeashPersistentIdCmdModifier::DumpParam(std::string& out) const
{
    out += "{leashPersistentId:" + std::to_string(param_.leashPersistentId_) + "}";
}

void SecurityLayerCmdModifier::DumpParam(std::string& out) const
{
    out += "{isSecurityLayer:" + std::string(param_.isSecurityLayer_ ? "true" : "false") + "}";
}

void SkipLayerCmdModifier::DumpParam(std::string& out) const
{
    out += "{isSkipLayer:" + std::string(param_.isSkipLayer_ ? "true" : "false") + "}";
}

void SnapshotSkipLayerCmdModifier::DumpParam(std::string& out) const
{
    out += "{isSnapshotSkipLayer:" + std::string(param_.isSnapshotSkipLayer_ ? "true" : "false") + "}";
}

void HasFingerprintCmdModifier::DumpParam(std::string& out) const
{
    out += "{hasFingerprint:" + std::string(param_.hasFingerprint_ ? "true" : "false") + "}";
}

void ColorSpaceCmdModifier::DumpParam(std::string& out) const
{
    out += "{colorSpace:" + std::to_string(static_cast<int>(param_.colorSpace_)) + "}";
}

void AbilityBGAlphaCmdModifier::DumpParam(std::string& out) const
{
    out += "{abilityBGAlpha:" + std::to_string(param_.abilityBGAlpha_) + "}";
}

void NotifyUIBufferAvailableCmdModifier::DumpParam(std::string& out) const
{
    out += "{notifyUIBufferAvailable:" + std::string(param_.notifyUIBufferAvailable_ ? "true" : "false") + "}";
}

void ContainerWindowCmdModifier::DumpParam(std::string& out) const
{
    out += "{hasContainerWindow:" + std::string(param_.hasContainerWindow_ ? "true" : "false") +
        ", containerWindowRrect:" + param_.containerWindowRrect_.ToString() + "}";
}

void FreezeCmdModifier::DumpParam(std::string& out) const
{
    out += "{isFreeze:" + std::string(param_.isFreeze_ ? "true" : "false") +
        ", isMarkedByUI:" + std::string(param_.isMarkedByUI_ ? "true" : "false") + "}";
}

void HardwareEnableHintCmdModifier::DumpParam(std::string& out) const
{
    out += "{isHardwareEnabled:" + std::string(param_.isHardwareEnabled_ ? "true" : "false") + "}";
}

void HardwareEnabledCmdModifier::DumpParam(std::string& out) const
{
    out += "{isHardwareEnabled:" + std::string(param_.isHardwareEnabled_ ? "true" : "false") +
        ", isSelfDrawingNodeType:" + std::to_string(static_cast<int>(param_.isSelfDrawingNodeType_)) +
        ", isDynamicHardwareEnable:" + std::string(param_.isDynamicHardwareEnable_ ? "true" : "false") + "}";
}

void BootAnimationCmdModifier::DumpParam(std::string& out) const
{
    out += "{isBootAnimation:" + std::string(param_.isBootAnimation_ ? "true" : "false") + "}";
}

void GlobalPositionEnabledCmdModifier::DumpParam(std::string& out) const
{
    out += "{isGlobalPositionEnabled:" + std::string(param_.isGlobalPositionEnabled_ ? "true" : "false") + "}";
}

void ClonedNodeInfoCmdModifier::DumpParam(std::string& out) const
{
    out += "{clonedNodeId:" + std::to_string(param_.clonedNodeId_) +
        ", clonedNeedOffscreen:" + std::string(param_.clonedNeedOffscreen_ ? "true" : "false") +
        ", clonedIsRelated:" + std::string(param_.clonedIsRelated_ ? "true" : "false") + "}";
}

void ForceUIFirstCmdModifier::DumpParam(std::string& out) const
{
    out += "{forceUIFirst:" + std::string(param_.forceUIFirst_ ? "true" : "false") + "}";
}

void AncoFlagsCmdModifier::DumpParam(std::string& out) const
{
    out += "{ancoFlags:" + std::to_string(param_.ancoFlags_) + "}";
}

void SkipDrawCmdModifier::DumpParam(std::string& out) const
{
    out += "{skipDraw:" + std::string(param_.skipDraw_ ? "true" : "false") + "}";
}

void WatermarkEnabledCmdModifier::DumpParam(std::string& out) const
{
    out += "{watermarkName:" + param_.watermarkName_ +
        ", watermarkEnabled:" + std::string(param_.watermarkEnabled_ ? "true" : "false") + "}";
}

void AbilityStateCmdModifier::DumpParam(std::string& out) const
{
    out += "{abilityState:" + std::to_string(static_cast<int>(param_.abilityState_)) + "}";
}

void HidePrivacyContentCmdModifier::DumpParam(std::string& out) const
{
    out += "{hidePrivacyContent:" + std::string(param_.hidePrivacyContent_ ? "true" : "false") + "}";
}

void ApiCompatibleVersionCmdModifier::DumpParam(std::string& out) const
{
    out += "{apiCompatibleVersion:" + std::to_string(param_.apiCompatibleVersion_) + "}";
}

void VirtualDisplayIdCmdModifier::DumpParam(std::string& out) const
{
    out += "{virtualDisplayId:" + std::to_string(param_.virtualDisplayId_) + "}";
}

void AttachToWindowContainerCmdModifier::DumpParam(std::string& out) const
{
    out += "{attachToWindowContainer:" + std::to_string(param_.attachToWindowContainer_) + "}";
}

void RegionToBeMagnifiedCmdModifier::DumpParam(std::string& out) const
{
    out += "{regionToBeMagnified:[" + std::to_string(param_.regionToBeMagnified_.x_) + "," +
        std::to_string(param_.regionToBeMagnified_.y_) + "," +
        std::to_string(param_.regionToBeMagnified_.z_) + "," +
        std::to_string(param_.regionToBeMagnified_.w_) + "]}";
}

void DetachFromWindowContainerCmdModifier::DumpParam(std::string& out) const
{
    out += "{detachFromWindowContainer:" + std::to_string(param_.detachFromWindowContainer_) + "}";
}

void CompositeLayerCmdModifier::DumpParam(std::string& out) const
{
    out += "{isCompositeLayer:" + std::string(param_.isCompositeLayer_ ? "true" : "false") +
        ", isMarkedByUI:" + std::string(param_.isMarkedByUI_ ? "true" : "false") + "}";
}

void StaticCachedCmdModifier::DumpParam(std::string& out) const
{
    out += "{isStaticCached:" + std::string(param_.isStaticCached_ ? "true" : "false") + "}";
}

void FrameGravityNewVersionEnabledCmdModifier::DumpParam(std::string& out) const
{
    out += "{isFrameGravityNewVersionEnabled:" +
        std::string(param_.isFrameGravityNewVersionEnabled_ ? "true" : "false") + "}";
}

void SurfaceBufferOpaqueCmdModifier::DumpParam(std::string& out) const
{
    out += "{isSurfaceBufferOpaque:" + std::string(param_.isSurfaceBufferOpaque_ ? "true" : "false") + "}";
}

void ContainerWindowTransparentCmdModifier::DumpParam(std::string& out) const
{
    out += "{isContainerWindowTransparent:" +
        std::string(param_.isContainerWindowTransparent_ ? "true" : "false") + "}";
}

void BufferAvailableCallbackCmdModifier::DumpParam(std::string& out) const
{
    out += "{callback:";
    if (param_.callback) {
        out += "[valid]";
    } else {
        out += "null";
    }
    out += "}";
}

void SurfaceDefaultSizeCmdModifier::DumpParam(std::string& out) const
{
    out += "{width:" + std::to_string(param_.width_) + ", height:" + std::to_string(param_.height_) + "}";
}

void IsDepthResourceCmdModifier::UpdateToRender()
{
    auto node = GetNode();
    if (!node) return;
    std::unique_ptr<RSCommand> command = std::make_unique<RSSetIsDepthResource>(
        node->GetId(), param_.isDepthResource_);
    AddCommand(command, node->IsRenderServiceNode());
}

void IsDepthResourceCmdModifier::DumpParam(std::string& out) const
{
    out += "{isDepthResource:" + std::string(param_.isDepthResource_ ? "true" : "false") + "}";
}

void ForceHardwareAndFixRotationCmdModifier::UpdateToRender()
{
    auto node = std::static_pointer_cast<RSSurfaceNode>(GetNode());
    if (!node) return;
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetHardwareAndFixRotation>(
        node->GetId(), param_.flag_);
    AddCommand(command, true);
}

void ForceHardwareAndFixRotationCmdModifier::DumpParam(std::string& out) const
{
    out += "{flag:" + std::string(param_.flag_ ? "true" : "false") + "}";
}

void AppRotationCorrectionCmdModifier::UpdateToRender()
{
    auto node = std::static_pointer_cast<RSSurfaceNode>(GetNode());
    if (!node) return;
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetAppRotationCorrection>(
        node->GetId(), param_.appRotationCorrection_);
    AddCommand(command, true);
}

void AppRotationCorrectionCmdModifier::DumpParam(std::string& out) const
{
    out += "{appRotationCorrection:" + std::to_string(static_cast<uint32_t>(param_.appRotationCorrection_)) + "}";
}

void HDRTypeCmdModifier::UpdateToRender()
{
    auto node = std::static_pointer_cast<RSSurfaceNode>(GetNode());
    if (!node) return;
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetHDRType>(
        node->GetId(), param_.hdrType_);
    AddCommand(command, true);
}

void HDRTypeCmdModifier::DumpParam(std::string& out) const
{
    out += "{hdrType:" + std::to_string(param_.hdrType_) + "}";
}

void DarkColorModeCmdModifier::UpdateToRender()
{
    auto node = std::static_pointer_cast<RSSurfaceNode>(GetNode());
    if (!node) return;
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetDarkColorMode>(
        node->GetId(), param_.isDarkColorMode_);
    AddCommand(command, true);
}

void DarkColorModeCmdModifier::DumpParam(std::string& out) const
{
    out += "{isDarkColorMode:" + std::string(param_.isDarkColorMode_ ? "true" : "false") + "}";
}

} // namespace Rosen
} // namespace OHOS
