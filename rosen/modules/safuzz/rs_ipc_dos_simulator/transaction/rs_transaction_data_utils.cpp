/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "transaction/rs_transaction_data_utils.h"

#include <cstdlib>
#include <functional>
#include <unordered_map>

#include "configs/safuzz_config.h"
#include "command/rs_animation_command_utils.h"
#include "command/rs_base_node_command_utils.h"
#include "command/rs_canvas_drawing_node_command_utils.h"
#include "command/rs_canvas_node_command_utils.h"
#include "command/rs_display_node_command_utils.h"
#include "command/rs_effect_node_command_utils.h"
#include "command/rs_frame_rate_linker_command_utils.h"
#include "command/rs_node_command_utils.h"
#include "command/rs_node_showing_command_utils.h"
#include "command/rs_proxy_node_command_utils.h"
#include "command/rs_root_node_command_utils.h"
#include "command/rs_surface_node_command_utils.h"
#include "common/safuzz_log.h"
#include "random/random_engine.h"
#include "tools/common_utils.h"

namespace OHOS {
namespace Rosen {
namespace {
const std::unordered_map<std::string, std::function<bool(std::unique_ptr<RSTransactionData>&)>> \
    COMMAND_NAME_TO_ADD_FUNCTION_MAPPING = {
    /********** RSBaseNodeCommand **********/
    DECLARE_ADD_RANDOM(RSBaseNodeCommand, RSBaseNodeDestroy),
    DECLARE_ADD_RANDOM(RSBaseNodeCommand, RSBaseNodeAddChild),
    DECLARE_ADD_RANDOM(RSBaseNodeCommand, RSBaseNodeMoveChild),
    DECLARE_ADD_RANDOM(RSBaseNodeCommand, RSBaseNodeRemoveChild),
    DECLARE_ADD_RANDOM(RSBaseNodeCommand, RSBaseNodeAddCrossParentChild),
    DECLARE_ADD_RANDOM(RSBaseNodeCommand, RSBaseNodeRemoveCrossParentChild),
    DECLARE_ADD_RANDOM(RSBaseNodeCommand, RSBaseNodeSetIsCrossNode),
    DECLARE_ADD_RANDOM(RSBaseNodeCommand, RSBaseNodeAddCrossScreenChild),
    DECLARE_ADD_RANDOM(RSBaseNodeCommand, RSBaseNodeRemoveCrossScreenChild),
    DECLARE_ADD_RANDOM(RSBaseNodeCommand, RSBaseNodeRemoveFromTree),
    DECLARE_ADD_RANDOM(RSBaseNodeCommand, RSBaseNodeClearChild),

    /********** RSNodeCommand **********/
    DECLARE_ADD_RANDOM(RSNodeCommand, RSAddModifier),
    DECLARE_ADD_RANDOM(RSNodeCommand, RSRemoveModifier),
    DECLARE_ADD_RANDOM(RSNodeCommand, RSUpdatePropertyBool),
    DECLARE_ADD_RANDOM(RSNodeCommand, RSUpdatePropertyFloat),
    DECLARE_ADD_RANDOM(RSNodeCommand, RSUpdatePropertyInt),
    DECLARE_ADD_RANDOM(RSNodeCommand, RSUpdatePropertyColor),
    DECLARE_ADD_RANDOM(RSNodeCommand, RSUpdatePropertyGravity),
    DECLARE_ADD_RANDOM(RSNodeCommand, RSUpdatePropertyMatrix3f),
    DECLARE_ADD_RANDOM(RSNodeCommand, RSUpdatePropertyQuaternion),
    DECLARE_ADD_RANDOM(RSNodeCommand, RSUpdatePropertyImage),
    DECLARE_ADD_RANDOM(RSNodeCommand, RSUpdatePropertyMask),
    DECLARE_ADD_RANDOM(RSNodeCommand, RSUpdatePropertyPath),
    DECLARE_ADD_RANDOM(RSNodeCommand, RSUpdatePropertyDynamicBrightness),
    DECLARE_ADD_RANDOM(RSNodeCommand, RSUpdatePropertyWaterRipple),
    DECLARE_ADD_RANDOM(RSNodeCommand, RSUpdatePropertyFlyOut),
    DECLARE_ADD_RANDOM(RSNodeCommand, RSUpdatePropertyLinearGradientBlurPara),
    DECLARE_ADD_RANDOM(RSNodeCommand, RSUpdatePropertyMotionBlurPara),
    DECLARE_ADD_RANDOM(RSNodeCommand, RSUpdatePropertyMagnifierPara),
    DECLARE_ADD_RANDOM(RSNodeCommand, RSUpdatePropertyEmitterUpdater),
    DECLARE_ADD_RANDOM(RSNodeCommand, RSUpdatePropertyParticleNoiseFields),
    DECLARE_ADD_RANDOM(RSNodeCommand, RSUpdatePropertyShader),
    DECLARE_ADD_RANDOM(RSNodeCommand, RSUpdatePropertyVector2f),
    DECLARE_ADD_RANDOM(RSNodeCommand, RSUpdatePropertyVector3f),
    DECLARE_ADD_RANDOM(RSNodeCommand, RSUpdatePropertyBorderStyle),
    DECLARE_ADD_RANDOM(RSNodeCommand, RSUpdatePropertyVector4Color),
    DECLARE_ADD_RANDOM(RSNodeCommand, RSUpdatePropertyVector4f),
    DECLARE_ADD_RANDOM(RSNodeCommand, RSUpdatePropertyRRect),
    DECLARE_ADD_RANDOM(RSNodeCommand, RSUpdatePropertyDrawCmdList),
    DECLARE_ADD_RANDOM(RSNodeCommand, RSUpdatePropertyDrawingMatrix),
    DECLARE_ADD_RANDOM(RSNodeCommand, RSSetFreeze),
    DECLARE_ADD_RANDOM(RSNodeCommand, RSSetNodeName),
    DECLARE_ADD_RANDOM(RSNodeCommand, RSMarkNodeGroup),
    DECLARE_ADD_RANDOM(RSNodeCommand, RSMarkNodeSingleFrameComposer),
    DECLARE_ADD_RANDOM(RSNodeCommand, RSMarkSuggestOpincNode),
    DECLARE_ADD_RANDOM(RSNodeCommand, RSMarkUifirstNode),
    DECLARE_ADD_RANDOM(RSNodeCommand, RSForceUifirstNode),
    DECLARE_ADD_RANDOM(RSNodeCommand, RSSetUIFirstSwitch),
    DECLARE_ADD_RANDOM(RSNodeCommand, RSSetDrawRegion),
    DECLARE_ADD_RANDOM(RSNodeCommand, RSSetOutOfParent),
    DECLARE_ADD_RANDOM(RSNodeCommand, RSRegisterGeometryTransitionNodePair),
    DECLARE_ADD_RANDOM(RSNodeCommand, RSUnregisterGeometryTransitionNodePair),
    DECLARE_ADD_RANDOM(RSNodeCommand, RSRemoveAllModifiers),
    DECLARE_ADD_RANDOM(RSNodeCommand, RSDumpClientNodeTree),
    DECLARE_ADD_RANDOM(RSNodeCommand, RSCommitDumpClientNodeTree),

    /********** RSCanvasNodeCommand **********/
    DECLARE_ADD_RANDOM(RSCanvasNodeCommand, RSCanvasNodeCreate),
    DECLARE_ADD_RANDOM(RSCanvasNodeCommand, RSCanvasNodeUpdateRecording),
    DECLARE_ADD_RANDOM(RSCanvasNodeCommand, RSCanvasNodeClearRecording),
    DECLARE_ADD_RANDOM(RSCanvasNodeCommand, RSCanvasNodeSetHDRPresent),

    /********** RSSurfaceNodeCommand **********/
    DECLARE_ADD_RANDOM(RSSurfaceNodeCommand, RSSurfaceNodeCreate),
    DECLARE_ADD_RANDOM(RSSurfaceNodeCommand, RSSurfaceNodeCreateWithConfig),
    DECLARE_ADD_RANDOM(RSSurfaceNodeCommand, RSSurfaceNodeSetContextMatrix),
    DECLARE_ADD_RANDOM(RSSurfaceNodeCommand, RSSurfaceNodeSetContextAlpha),
    DECLARE_ADD_RANDOM(RSSurfaceNodeCommand, RSSurfaceNodeSetContextClipRegion),
    DECLARE_ADD_RANDOM(RSSurfaceNodeCommand, RSSurfaceNodeSetHardwareAndFixRotation),
    DECLARE_ADD_RANDOM(RSSurfaceNodeCommand, RSSurfaceNodeSetBootAnimation),
    DECLARE_ADD_RANDOM(RSSurfaceNodeCommand, RSSurfaceNodeSetGlobalPositionEnabled),
    DECLARE_ADD_RANDOM(RSSurfaceNodeCommand, RSSurfaceNodeSetSecurityLayer),
    DECLARE_ADD_RANDOM(RSSurfaceNodeCommand, RSSurfaceNodeSetSkipLayer),
    DECLARE_ADD_RANDOM(RSSurfaceNodeCommand, RSSurfaceNodeSetSnapshotSkipLayer),
    DECLARE_ADD_RANDOM(RSSurfaceNodeCommand, RSSurfaceNodeSetFingerprint),
    DECLARE_ADD_RANDOM(RSSurfaceNodeCommand, RSSurfaceNodeUpdateSurfaceDefaultSize),
    DECLARE_ADD_RANDOM(RSSurfaceNodeCommand, RSSurfaceNodeConnectToNodeInRenderService),
    DECLARE_ADD_RANDOM(RSSurfaceNodeCommand, RSSurfaceNodeSetCallbackForRenderThreadRefresh),
    DECLARE_ADD_RANDOM(RSSurfaceNodeCommand, RSSurfaceNodeSetBounds),
    DECLARE_ADD_RANDOM(RSSurfaceNodeCommand, RSSurfaceNodeSetAbilityBGAlpha),
    DECLARE_ADD_RANDOM(RSSurfaceNodeCommand, RSSurfaceNodeSetIsNotifyUIBufferAvailable),
    DECLARE_ADD_RANDOM(RSSurfaceNodeCommand, RSSurfaceNodeMarkUIHidden),
    DECLARE_ADD_RANDOM(RSSurfaceNodeCommand, RSSurfaceNodeSetIsTextureExportNode),
    DECLARE_ADD_RANDOM(RSSurfaceNodeCommand, RSSurfaceNodeSetSurfaceNodeType),
    DECLARE_ADD_RANDOM(RSSurfaceNodeCommand, RSSurfaceNodeSetContainerWindow),
    DECLARE_ADD_RANDOM(RSSurfaceNodeCommand, RSSurfaceNodeSetAnimationFinished),
    DECLARE_ADD_RANDOM(RSSurfaceNodeCommand, RSSurfaceNodeAttachToDisplay),
    DECLARE_ADD_RANDOM(RSSurfaceNodeCommand, RSSurfaceNodeDetachToDisplay),
    DECLARE_ADD_RANDOM(RSSurfaceNodeCommand, RSurfaceNodeSetSurfaceId),
    DECLARE_ADD_RANDOM(RSSurfaceNodeCommand, RSurfaceNodeSetLeashPersistentId),
    DECLARE_ADD_RANDOM(RSSurfaceNodeCommand, RSSurfaceNodeSetColorSpace),
    DECLARE_ADD_RANDOM(RSSurfaceNodeCommand, RSSurfaceNodeSetForeground),
    DECLARE_ADD_RANDOM(RSSurfaceNodeCommand, RSSurfaceNodeSetForceUIFirst),
    DECLARE_ADD_RANDOM(RSSurfaceNodeCommand, RSSurfaceNodeSetAncoFlags),
    DECLARE_ADD_RANDOM(RSSurfaceNodeCommand, RSSurfaceNodeSetHDRPresent),
    DECLARE_ADD_RANDOM(RSSurfaceNodeCommand, RSSurfaceNodeSetSkipDraw),
    DECLARE_ADD_RANDOM(RSSurfaceNodeCommand, RSSurfaceNodeSetWatermarkEnabled),
    DECLARE_ADD_RANDOM(RSSurfaceNodeCommand, RSSurfaceNodeSetAbilityState),
    DECLARE_ADD_RANDOM(RSSurfaceNodeCommand, RSSurfaceNodeSetApiCompatibleVersion),
    DECLARE_ADD_RANDOM(RSSurfaceNodeCommand, RSSurfaceNodeSetHardwareEnableHint),
    DECLARE_ADD_RANDOM(RSSurfaceNodeCommand, RSSurfaceNodeAttachToWindowContainer),
    DECLARE_ADD_RANDOM(RSSurfaceNodeCommand, RSSurfaceNodeDetachFromWindowContainer),

    /********** RSProxyNodeCommand **********/
    DECLARE_ADD_RANDOM(RSProxyNodeCommand, RSProxyNodeCreate),
    DECLARE_ADD_RANDOM(RSProxyNodeCommand, RSProxyNodeResetContextVariableCache),

    /********** RSRootNodeCommand **********/
    DECLARE_ADD_RANDOM(RSRootNodeCommand, RSRootNodeCreate),
    DECLARE_ADD_RANDOM(RSRootNodeCommand, RSRootNodeAttachRSSurfaceNode),
    DECLARE_ADD_RANDOM(RSRootNodeCommand, RSRootNodeSetEnableRender),
    DECLARE_ADD_RANDOM(RSRootNodeCommand, RSRootNodeAttachToUniSurfaceNode),
    DECLARE_ADD_RANDOM(RSRootNodeCommand, RSRootNodeUpdateSuggestedBufferSize),

    /********** RSDisplayNodeCommand **********/
    DECLARE_ADD_RANDOM(RSDisplayNodeCommand, RSDisplayNodeCreate),
    DECLARE_ADD_RANDOM(RSDisplayNodeCommand, RSDisplayNodeSetScreenId),
    DECLARE_ADD_RANDOM(RSDisplayNodeCommand, RSDisplayNodeSetSecurityDisplay),
    DECLARE_ADD_RANDOM(RSDisplayNodeCommand, RSDisplayNodeSetDisplayMode),
    DECLARE_ADD_RANDOM(RSDisplayNodeCommand, RSDisplayNodeSetScreenRotation),
    DECLARE_ADD_RANDOM(RSDisplayNodeCommand, RSDisplayNodeSetBootAnimation),
    DECLARE_ADD_RANDOM(RSDisplayNodeCommand, RSDisplayNodeAddToTree),
    DECLARE_ADD_RANDOM(RSDisplayNodeCommand, RSDisplayNodeRemoveFromTree),
    DECLARE_ADD_RANDOM(RSDisplayNodeCommand, RSDisplayNodeSetNodePid),

    /********** RSEffectNodeCommand **********/
    DECLARE_ADD_RANDOM(RSEffectNodeCommand, RSEffectNodeCreate),

    /********** RSCanvasDrawingNodeCommand **********/
    DECLARE_ADD_RANDOM(RSCanvasDrawingNodeCommand, RSCanvasDrawingNodeCreate),
    DECLARE_ADD_RANDOM(RSCanvasDrawingNodeCommand, RSCanvasDrawingNodeResetSurface),

    /********** RSAnimationCommand **********/
    DECLARE_ADD_RANDOM(RSAnimationCommand, RSAnimationStart),
    DECLARE_ADD_RANDOM(RSAnimationCommand, RSAnimationPause),
    DECLARE_ADD_RANDOM(RSAnimationCommand, RSAnimationResume),
    DECLARE_ADD_RANDOM(RSAnimationCommand, RSAnimationFinish),
    DECLARE_ADD_RANDOM(RSAnimationCommand, RSAnimationReverse),
    DECLARE_ADD_RANDOM(RSAnimationCommand, RSAnimationSetFraction),
    DECLARE_ADD_RANDOM(RSAnimationCommand, RSAnimationCancel),
    DECLARE_ADD_RANDOM(RSAnimationCommand, RSAnimationCallback),
    DECLARE_ADD_RANDOM(RSAnimationCommand, RSAnimationCreateCurve),
    DECLARE_ADD_RANDOM(RSAnimationCommand, RSAnimationCreateParticle),
    DECLARE_ADD_RANDOM(RSAnimationCommand, RSAnimationCreateKeyframe),
    DECLARE_ADD_RANDOM(RSAnimationCommand, RSAnimationCreatePath),
    DECLARE_ADD_RANDOM(RSAnimationCommand, RSAnimationCreateTransition),
    DECLARE_ADD_RANDOM(RSAnimationCommand, RSAnimationCreateSpring),
    DECLARE_ADD_RANDOM(RSAnimationCommand, RSAnimationCreateInterpolatingSpring),
    DECLARE_ADD_RANDOM(RSAnimationCommand, RSInteractiveAnimatorCreate),
    DECLARE_ADD_RANDOM(RSAnimationCommand, RSInteractiveAnimatorDestory),
    DECLARE_ADD_RANDOM(RSAnimationCommand, RSInteractiveAnimatorPause),
    DECLARE_ADD_RANDOM(RSAnimationCommand, RSInteractiveAnimatorContinue),
    DECLARE_ADD_RANDOM(RSAnimationCommand, RSInteractiveAnimatorFinish),
    DECLARE_ADD_RANDOM(RSAnimationCommand, RSInteractiveAnimatorReverse),
    DECLARE_ADD_RANDOM(RSAnimationCommand, RSInteractiveAnimatorSetFraction),

    /********** RSNodeShowingCommand **********/
    DECLARE_ADD_RANDOM(RSNodeShowingCommand, RSNodeGetShowingPropertyAndCancelAnimation),
    DECLARE_ADD_RANDOM(RSNodeShowingCommand, RSNodeGetShowingPropertiesAndCancelAnimation),
    DECLARE_ADD_RANDOM(RSNodeShowingCommand, RSNodeGetAnimationsValueFraction),

    /********** RSFrameRateLinkerCommand **********/
    DECLARE_ADD_RANDOM(RSFrameRateLinkerCommand, RSFrameRateLinkerDestroy),
    DECLARE_ADD_RANDOM(RSFrameRateLinkerCommand, RSFrameRateLinkerUpdateRange),
};
}

std::unique_ptr<RSTransactionData> RSTransactionDataUtils::CreateTransactionDataFromCommandList(
    const std::string& commandList, int commandListRepeat)
{
    static constexpr float CHOOSE_VARIANT_PROBABILITY = 0.5;

    std::vector<std::pair<std::string, int>> commandRepeatVec;
    if (!GetCommandRepeatVecFromCommandList(commandRepeatVec, commandList)) {
        SAFUZZ_LOGE("RSTransactionDataUtils::CreateTransactionDataFromCommandList "
            "GetCommandRepeatVecFromCommandList failed");
        return nullptr;
    }

    std::unique_ptr<RSTransactionData> rsTransactionData = nullptr;
    if (RandomEngine::ChooseByProbability(CHOOSE_VARIANT_PROBABILITY)) {
        rsTransactionData = std::make_unique<RSTransactionDataVariant>();
    } else {
        rsTransactionData = std::make_unique<RSTransactionData>();
    }
    for (int commandListRepeatIndex = 0; commandListRepeatIndex < commandListRepeat; ++commandListRepeatIndex) {
        for (const auto& [ commandName, commandRepeat ] : commandRepeatVec) {
            std::function<bool(std::unique_ptr<RSTransactionData>&)> addRandomCommandFunction =
                GetAddFunctionByCommandName(commandName);
            if (addRandomCommandFunction == nullptr) {
                SAFUZZ_LOGE("RSTransactionDataUtils::CreateTransactionDataFromCommandList "
                    "command %s can not be identified", commandName.c_str());
                return nullptr;
            }
            for (int commandRepeatIndex = 0; commandRepeatIndex < commandRepeat; ++commandRepeatIndex) {
                if (!addRandomCommandFunction(rsTransactionData)) {
                    SAFUZZ_LOGE("RSTransactionDataUtils::CreateTransactionDataFromCommandList "
                        "AddRandom%s failed", commandName.c_str());
                    return nullptr;
                }
            }
        }
    }
    return rsTransactionData;
}

bool RSTransactionDataUtils::GetCommandRepeatVecFromCommandList(
    std::vector<std::pair<std::string, int>>& commandRepeatVec, const std::string& commandList)
{
    commandRepeatVec.clear();
    std::vector<std::string> commands = CommonUtils::SplitString(commandList, ';');
    for (const std::string& commandRepeatDesc : commands) {
        std::vector<std::string> commandAndRepeat = CommonUtils::SplitString(commandRepeatDesc, '*');
        std::string commandName;
        int commandRepeat = 1;
        switch (static_cast<int>(commandAndRepeat.size())) {
            case 1: {
                commandName = commandAndRepeat[0];
                break;
            }
            case 2: {
                commandName = commandAndRepeat[0];
                commandRepeat = atoi(commandAndRepeat[1].c_str());
                if (commandRepeat <= 0) {
                    SAFUZZ_LOGE("RSTransactionDataUtils::GetCommandRepeatVecFromCommandList "
                        "commandRepeat %d is not positive", commandRepeat);
                    return false;
                }
                break;
            }
            default: {
                SAFUZZ_LOGE("RSTransactionDataUtils::GetCommandRepeatVecFromCommandList "
                    "commandRepeatDesc %s is not legal", commandRepeatDesc.c_str());
                return false;
            }
        }
        commandRepeatVec.emplace_back(commandName, commandRepeat);
    }
    return true;
}

std::function<bool(std::unique_ptr<RSTransactionData>&)> RSTransactionDataUtils::GetAddFunctionByCommandName(
    const std::string& commandName)
{
    auto it = COMMAND_NAME_TO_ADD_FUNCTION_MAPPING.find(commandName);
    if (it == COMMAND_NAME_TO_ADD_FUNCTION_MAPPING.end()) {
        return nullptr;
    }
    return it->second;
}

bool RSTransactionDataVariant::Marshalling(Parcel& parcel) const
{
    bool success = true;
    parcel.SetMaxCapacity(PARCEL_MAX_CPACITY_VARIANT);
    // to correct actual marshaled command size later, record its position in parcel
    size_t recordPosition = parcel.GetWritePosition();
    std::unique_lock<std::mutex> lock(commandMutex_);
    success = success && parcel.WriteInt32(static_cast<int32_t>(payload_.size()));
    size_t marshaledSize = 0;
    static bool isUniRender = RSSystemProperties::GetUniRenderEnabled();
    success = success && parcel.WriteBool(isUniRender);
    while (marshallingIndex_ < payload_.size()) {
        auto& [nodeId, followType, command] = payload_[marshallingIndex_];
        
        if (!isUniRender) {
            success = success && parcel.WriteUint64(nodeId);
            success = success && parcel.WriteUint8(static_cast<uint8_t>(followType));
        }
        if (!command) {
            parcel.WriteUint8(0);
            RS_LOGW("failed RSTransactionDataVariant::Marshalling, command is nullptr");
        } else if (command->indexVerifier_ != marshallingIndex_) {
            parcel.WriteUint8(0);
            RS_LOGW("failed RSTransactionDataVariant::Marshalling, indexVerifier is wrong, SIGSEGV may have occurred");
        } else {
            parcel.WriteUint8(1);
            if (!parcel.WriteUint32(static_cast<uint32_t>(parcel.GetWritePosition()))) {
                RS_LOGE("RSTransactionDataVariant::Marshalling failed to write begin position "
                    "marshallingIndex:%{public}zu", marshallingIndex_);
                success = false;
            }
            success = success && command->Marshalling(parcel);
            if (!parcel.WriteUint32(static_cast<uint32_t>(parcel.GetWritePosition()))) {
                RS_LOGE("RSTransactionDataVariant::Marshalling failed to write end position "
                    "marshallingIndex:%{public}zu", marshallingIndex_);
                success = false;
            }
        }
        if (!success) {
            if (command != nullptr) {
                ROSEN_LOGE("failed RSTransactionDataVariant::Marshalling type:%{public}s",
                    command->PrintType().c_str());
            } else {
                ROSEN_LOGE("failed RSTransactionDataVariant::Marshalling, parcel write error");
            }
            return false;
        }
        ++marshallingIndex_;
        ++marshaledSize;
        if ((RSSystemProperties::GetUnmarshParallelFlag() &&
            parcel.GetDataSize() > RSSystemProperties::GetUnMarshParallelSize()) ||
            parcel.GetDataSize() > PARCEL_SPLIT_THRESHOLD_VARIANT) {
            break;
        }
    }
    if (marshaledSize < payload_.size()) {
        // correct command size recorded in Parcel
        *reinterpret_cast<int32_t*>(parcel.GetData() + recordPosition) = static_cast<int32_t>(marshaledSize);
        ROSEN_LOGW("RSTransactionDataVariant::Marshalling data split to several parcels"
                   ", marshaledSize:%{public}zu, marshallingIndex_:%{public}zu, total count:%{public}zu"
                   ", parcel size:%{public}zu, threshold:%{public}zu.",
            marshaledSize, marshallingIndex_, payload_.size(), parcel.GetDataSize(), PARCEL_SPLIT_THRESHOLD_VARIANT);

        AlarmRsNodeLog();
    }
    success = success && parcel.WriteBool(needSync_);
    success = success && parcel.WriteBool(needCloseSync_);
    success = success && parcel.WriteInt32(syncTransactionCount_);
    success = success && parcel.WriteUint64(timestamp_);
    success = success && parcel.WriteInt32(pid_);
    success = success && parcel.WriteUint64(index_);
    success = success && parcel.WriteUint64(syncId_);
    success = success && parcel.WriteInt32(parentPid_);
    if (!success) {
        ROSEN_LOGE("RSTransactionDataVariant::Marshalling failed");
    }
    return success;
}
} // namespace Rosen
} // namespace OHOS
