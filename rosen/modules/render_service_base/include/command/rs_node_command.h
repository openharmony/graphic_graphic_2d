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

#ifndef ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_NODE_COMMAND_H
#define ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_NODE_COMMAND_H

#include "animation/rs_frame_rate_range.h"
#include "command/rs_command_templates.h"
#include "common/rs_macros.h"
#include "pipeline/rs_render_node.h"
#include "property/rs_properties.h"

namespace OHOS {
namespace Rosen {

//Each command HAVE TO have UNIQUE ID in ALL HISTORY
//If a command is not used and you want to delete it,
//just COMMENT it - and never use this value anymore
enum RSNodeCommandType : uint16_t {
    ADD_MODIFIER = 0x0000,
    REMOVE_MODIFIER = 0x0001,

    UPDATE_MODIFIER_BOOL = 0x0100,
    UPDATE_MODIFIER_FLOAT = 0x0101,
    UPDATE_MODIFIER_INT = 0x0102,
    UPDATE_MODIFIER_COLOR = 0x0103,
    UPDATE_MODIFIER_GRAVITY = 0x0104,
    UPDATE_MODIFIER_MATRIX3F = 0x0105,
    UPDATE_MODIFIER_QUATERNION = 0x0106,
    UPDATE_MODIFIER_FILTER_PTR = 0x0107,
    UPDATE_MODIFIER_IMAGE_PTR = 0x0108,
    UPDATE_MODIFIER_MASK_PTR = 0x0109,
    UPDATE_MODIFIER_PATH_PTR = 0x010A,
    UPDATE_MODIFIER_DYNAMIC_BRIGHTNESS = 0x010B,
    UPDATE_MODIFIER_GRADIENT_BLUR_PTR = 0x010C,
    UPDATE_MODIFIER_EMITTER_UPDATER_PTR = 0x010D,
    UPDATE_MODIFIER_NOISE_FIELD_PTR = 0x010E,
    UPDATE_MODIFIER_SHADER_PTR = 0x010F,
    UPDATE_MODIFIER_VECTOR2F = 0x0110,
    UPDATE_MODIFIER_VECTOR3F = 0x0111,
    UPDATE_MODIFIER_VECTOR4_BORDER_STYLE = 0x0112,
    UPDATE_MODIFIER_VECTOR4_COLOR = 0x0113,
    UPDATE_MODIFIER_VECTOR4F = 0x0114,
    UPDATE_MODIFIER_RRECT = 0x0115,
    UPDATE_MODIFIER_DRAW_CMD_LIST = 0x0116,
    UPDATE_MODIFIER_DRAWING_MATRIX = 0x0117,

    SET_FREEZE = 0x0200,
    SET_DRAW_REGION = 0x0201,
    SET_OUT_OF_PARENT = 0x0202,
    SET_TAKE_SURFACE_CAPTURE_FOR_UI_FLAG = 0x0203,
    SET_UIFIRST_SWITCH = 0x0204,

    REGISTER_GEOMETRY_TRANSITION = 0x0300,
    UNREGISTER_GEOMETRY_TRANSITION = 0x0301,

    MARK_NODE_GROUP = 0x0400,
    MARK_NODE_SINGLE_FRAME_COMPOSER = 0x0401,
    MARK_SUGGEST_OPINC_NODE = 0x0402,

    MARK_UIFIRST_NODE = 0x0500,
    MARK_UIFIRST_NODE_FORCE = 0x0501,

    SET_NODE_NAME = 0x0600,
    UPDATE_MODIFIER_MOTION_BLUR_PTR = 0x0601,
    UPDATE_MODIFIER_MAGNIFIER_PTR = 0x0602,
    UPDATE_MODIFIER_WATER_RIPPLE = 0x0603,
    UPDATE_MODIFIER_FLY_OUT = 0x0604,
    REMOVE_ALL_MODIFIERS = 0x0605,

    DUMP_CLIENT_NODE_TREE = 0x0700,
    COMMIT_DUMP_CLIENT_NODE_TREE = 0x0701,

    SET_UICONTEXT_TOKEN = 0x0800,

    SET_DRAW_NODE_TYPE = 0x0900,
};

class RSB_EXPORT RSNodeCommandHelper {
public:
    static void AddModifier(RSContext& context, NodeId nodeId, const std::shared_ptr<RSRenderModifier>& modifier);
    static void RemoveModifier(RSContext& context, NodeId nodeId, PropertyId propertyId);
    static void RemoveAllModifiers(RSContext& context, NodeId nodeId);

    template<typename T>
    static void UpdateModifier(RSContext& context, NodeId nodeId, T value, PropertyId id, PropertyUpdateType type)
    {
        std::shared_ptr<RSRenderPropertyBase> prop = std::make_shared<RSRenderProperty<T>>(value, id);
        auto& nodeMap = context.GetNodeMap();
        auto node = nodeMap.GetRenderNode<RSRenderNode>(nodeId);
        if (!node) {
            return;
        }
        auto modifier = node->GetModifier(id);
        if (!modifier) {
            return;
        }
        switch (type) {
            case UPDATE_TYPE_OVERWRITE:
                modifier->Update(prop, false);
                break;
            case UPDATE_TYPE_INCREMENTAL:
                modifier->Update(prop, true);
                break;
            case UPDATE_TYPE_FORCE_OVERWRITE:
                modifier->Update(prop, false);
                node->GetAnimationManager().CancelAnimationByPropertyId(id);
                break;
            default:
                break;
        }
    }
    static void UpdateModifierDrawCmdList(
        RSContext& context, NodeId nodeId, Drawing::DrawCmdListPtr value, PropertyId id, bool isDelta)
    {
        std::shared_ptr<RSRenderPropertyBase> prop =
            std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>(value, id);
        auto& nodeMap = context.GetNodeMap();
        auto node = nodeMap.GetRenderNode<RSRenderNode>(nodeId);
        if (!node) {
            return;
        }
        auto modifier = node->GetModifier(id);
        if (!modifier) {
            return;
        }
        modifier->Update(prop, isDelta);
        if (value) {
            value->UpdateNodeIdToPicture(nodeId);
        }
    }

    static void SetFreeze(RSContext& context, NodeId nodeId, bool isFreeze);
    static void SetNodeName(RSContext& context, NodeId nodeId, std::string& nodeName);
    static void MarkNodeGroup(RSContext& context, NodeId nodeId, bool isNodeGroup, bool isForced,
        bool includeProperty);
    static void MarkNodeSingleFrameComposer(RSContext& context, NodeId nodeId, bool isNodeFasterDraw, pid_t pid);
    static void MarkSuggestOpincNode(RSContext& context, NodeId nodeId, bool isOpincNode, bool isNeedCalculate);

    static void MarkUifirstNode(RSContext& context, NodeId nodeId, bool isUifirstNode);
    static void ForceUifirstNode(RSContext& context, NodeId nodeId, bool isForceFlag, bool isUifirstEnable);
    static void SetUIFirstSwitch(RSContext& context, NodeId nodeId, RSUIFirstSwitch uiFirstSwitch);
    static void SetDrawRegion(RSContext& context, NodeId nodeId, std::shared_ptr<RectF> rect);
    static void SetOutOfParent(RSContext& context, NodeId nodeId, OutOfParentType outOfParent);
    static void SetTakeSurfaceForUIFlag(RSContext& context, NodeId nodeId);

    static void RegisterGeometryTransitionPair(RSContext& context, NodeId inNodeId, NodeId outNodeId,
        const bool isInSameWindow);
    static void UnregisterGeometryTransitionPair(RSContext& context, NodeId inNodeId, NodeId outNodeId);

    using DumpNodeTreeProcessor = std::function<void(NodeId, pid_t, uint32_t)>;
    static void DumpClientNodeTree(RSContext& context, NodeId nodeId, pid_t pid, uint32_t taskId);
    static RSB_EXPORT void SetDumpNodeTreeProcessor(DumpNodeTreeProcessor processor);

    using CommitDumpNodeTreeProcessor = std::function<void(NodeId, pid_t, uint32_t, const std::string&)>;
    static void CommitDumpClientNodeTree(RSContext& context, NodeId nodeId, pid_t pid, uint32_t taskId,
        const std::string& result);
    static RSB_EXPORT void SetCommitDumpNodeTreeProcessor(CommitDumpNodeTreeProcessor processor);
    static void SetUIToken(RSContext& context, NodeId nodeId, uint64_t token);
    static void SetDrawNodeType(RSContext& context, NodeId nodeId, DrawNodeType nodeType);
};

ADD_COMMAND(RSAddModifier,
    ARG(PERMISSION_APP, RS_NODE, ADD_MODIFIER,
        RSNodeCommandHelper::AddModifier, NodeId, std::shared_ptr<RSRenderModifier>))
ADD_COMMAND(RSRemoveModifier,
    ARG(PERMISSION_APP, RS_NODE, REMOVE_MODIFIER,
        RSNodeCommandHelper::RemoveModifier, NodeId, PropertyId))

ADD_COMMAND(RSUpdatePropertyBool,
    ARG(PERMISSION_APP, RS_NODE, UPDATE_MODIFIER_BOOL,
        RSNodeCommandHelper::UpdateModifier<bool>, NodeId, bool, PropertyId, PropertyUpdateType))
ADD_COMMAND(RSUpdatePropertyFloat,
    ARG(PERMISSION_APP, RS_NODE, UPDATE_MODIFIER_FLOAT,
        RSNodeCommandHelper::UpdateModifier<float>, NodeId, float, PropertyId, PropertyUpdateType))
ADD_COMMAND(RSUpdatePropertyInt,
    ARG(PERMISSION_APP, RS_NODE, UPDATE_MODIFIER_INT,
        RSNodeCommandHelper::UpdateModifier<int>, NodeId, int, PropertyId, PropertyUpdateType))
ADD_COMMAND(RSUpdatePropertyColor,
    ARG(PERMISSION_APP, RS_NODE, UPDATE_MODIFIER_COLOR,
        RSNodeCommandHelper::UpdateModifier<Color>, NodeId, Color, PropertyId, PropertyUpdateType))
ADD_COMMAND(RSUpdatePropertyGravity,
    ARG(PERMISSION_APP, RS_NODE, UPDATE_MODIFIER_GRAVITY,
        RSNodeCommandHelper::UpdateModifier<Gravity>, NodeId, Gravity, PropertyId, PropertyUpdateType))
ADD_COMMAND(RSUpdatePropertyMatrix3f,
    ARG(PERMISSION_APP, RS_NODE, UPDATE_MODIFIER_MATRIX3F,
        RSNodeCommandHelper::UpdateModifier<Matrix3f>, NodeId, Matrix3f, PropertyId, PropertyUpdateType))
ADD_COMMAND(RSUpdatePropertyQuaternion,
    ARG(PERMISSION_APP, RS_NODE, UPDATE_MODIFIER_QUATERNION,
        RSNodeCommandHelper::UpdateModifier<Quaternion>, NodeId, Quaternion, PropertyId, PropertyUpdateType))
ADD_COMMAND(RSUpdatePropertyFilter,
    ARG(PERMISSION_APP, RS_NODE, UPDATE_MODIFIER_FILTER_PTR,
        RSNodeCommandHelper::UpdateModifier<std::shared_ptr<RSFilter>>,
        NodeId, std::shared_ptr<RSFilter>, PropertyId, PropertyUpdateType))
ADD_COMMAND(RSUpdatePropertyImage,
    ARG(PERMISSION_APP, RS_NODE, UPDATE_MODIFIER_IMAGE_PTR,
        RSNodeCommandHelper::UpdateModifier<std::shared_ptr<RSImage>>,
        NodeId, std::shared_ptr<RSImage>, PropertyId, PropertyUpdateType))
ADD_COMMAND(RSUpdatePropertyMask,
    ARG(PERMISSION_APP, RS_NODE, UPDATE_MODIFIER_MASK_PTR,
        RSNodeCommandHelper::UpdateModifier<std::shared_ptr<RSMask>>,
        NodeId, std::shared_ptr<RSMask>, PropertyId, PropertyUpdateType))
ADD_COMMAND(RSUpdatePropertyPath,
    ARG(PERMISSION_APP, RS_NODE, UPDATE_MODIFIER_PATH_PTR,
        RSNodeCommandHelper::UpdateModifier<std::shared_ptr<RSPath>>,
        NodeId, std::shared_ptr<RSPath>, PropertyId, PropertyUpdateType))
ADD_COMMAND(RSUpdatePropertyDynamicBrightness,
    ARG(PERMISSION_APP, RS_NODE, UPDATE_MODIFIER_DYNAMIC_BRIGHTNESS,
        RSNodeCommandHelper::UpdateModifier<RSDynamicBrightnessPara>,
        NodeId, RSDynamicBrightnessPara, PropertyId, PropertyUpdateType))
ADD_COMMAND(RSUpdatePropertyWaterRipple,
    ARG(PERMISSION_APP, RS_NODE, UPDATE_MODIFIER_WATER_RIPPLE,
        RSNodeCommandHelper::UpdateModifier<RSWaterRipplePara>,
        NodeId, RSWaterRipplePara, PropertyId, PropertyUpdateType))
ADD_COMMAND(RSUpdatePropertyFlyOut,
    ARG(PERMISSION_APP, RS_NODE, UPDATE_MODIFIER_FLY_OUT,
        RSNodeCommandHelper::UpdateModifier<RSFlyOutPara>, NodeId, RSFlyOutPara, PropertyId, PropertyUpdateType))
ADD_COMMAND(RSUpdatePropertyLinearGradientBlurPara,
    ARG(PERMISSION_APP, RS_NODE, UPDATE_MODIFIER_GRADIENT_BLUR_PTR,
        RSNodeCommandHelper::UpdateModifier<std::shared_ptr<RSLinearGradientBlurPara>>,
        NodeId, std::shared_ptr<RSLinearGradientBlurPara>, PropertyId, PropertyUpdateType))
ADD_COMMAND(RSUpdatePropertyMotionBlurPara,
    ARG(PERMISSION_APP, RS_NODE, UPDATE_MODIFIER_MOTION_BLUR_PTR,
        RSNodeCommandHelper::UpdateModifier<std::shared_ptr<MotionBlurParam>>,
        NodeId, std::shared_ptr<MotionBlurParam>, PropertyId, PropertyUpdateType))
ADD_COMMAND(RSUpdatePropertyMagnifierPara,
    ARG(PERMISSION_APP, RS_NODE, UPDATE_MODIFIER_MAGNIFIER_PTR,
        RSNodeCommandHelper::UpdateModifier<std::shared_ptr<RSMagnifierParams>>,
        NodeId, std::shared_ptr<RSMagnifierParams>, PropertyId, PropertyUpdateType))
ADD_COMMAND(RSUpdatePropertyEmitterUpdater,
    ARG(PERMISSION_APP, RS_NODE, UPDATE_MODIFIER_EMITTER_UPDATER_PTR,
        RSNodeCommandHelper::UpdateModifier<std::vector<std::shared_ptr<EmitterUpdater>>>,
        NodeId, std::vector<std::shared_ptr<EmitterUpdater>>, PropertyId, PropertyUpdateType))
ADD_COMMAND(RSUpdatePropertyParticleNoiseFields,
    ARG(PERMISSION_APP, RS_NODE, UPDATE_MODIFIER_NOISE_FIELD_PTR,
        RSNodeCommandHelper::UpdateModifier<std::shared_ptr<ParticleNoiseFields>>,
        NodeId, std::shared_ptr<ParticleNoiseFields>, PropertyId, PropertyUpdateType))
ADD_COMMAND(RSUpdatePropertyShader,
    ARG(PERMISSION_APP, RS_NODE, UPDATE_MODIFIER_SHADER_PTR,
        RSNodeCommandHelper::UpdateModifier<std::shared_ptr<RSShader>>,
        NodeId, std::shared_ptr<RSShader>, PropertyId, PropertyUpdateType))
ADD_COMMAND(RSUpdatePropertyVector2f,
    ARG(PERMISSION_APP, RS_NODE, UPDATE_MODIFIER_VECTOR2F,
        RSNodeCommandHelper::UpdateModifier<Vector2f>, NodeId, Vector2f, PropertyId, PropertyUpdateType))
ADD_COMMAND(RSUpdatePropertyVector3f,
    ARG(PERMISSION_APP, RS_NODE, UPDATE_MODIFIER_VECTOR3F, RSNodeCommandHelper::UpdateModifier<Vector3f>,
        NodeId, Vector3f, PropertyId, PropertyUpdateType))
ADD_COMMAND(RSUpdatePropertyBorderStyle,
    ARG(PERMISSION_APP, RS_NODE, UPDATE_MODIFIER_VECTOR4_BORDER_STYLE,
        RSNodeCommandHelper::UpdateModifier<Vector4<uint32_t>>,
        NodeId, Vector4<uint32_t>, PropertyId, PropertyUpdateType))
ADD_COMMAND(RSUpdatePropertyVector4Color,
    ARG(PERMISSION_APP, RS_NODE, UPDATE_MODIFIER_VECTOR4_COLOR,
        RSNodeCommandHelper::UpdateModifier<Vector4<Color>>, NodeId, Vector4<Color>, PropertyId, PropertyUpdateType))
ADD_COMMAND(RSUpdatePropertyVector4f,
    ARG(PERMISSION_APP, RS_NODE, UPDATE_MODIFIER_VECTOR4F,
        RSNodeCommandHelper::UpdateModifier<Vector4f>, NodeId, Vector4f, PropertyId, PropertyUpdateType))
ADD_COMMAND(RSUpdatePropertyRRect,
    ARG(PERMISSION_APP, RS_NODE, UPDATE_MODIFIER_RRECT,
        RSNodeCommandHelper::UpdateModifier<RRect>, NodeId, RRect, PropertyId, PropertyUpdateType))
ADD_COMMAND(RSUpdatePropertyDrawCmdList,
    ARG(PERMISSION_APP, RS_NODE, UPDATE_MODIFIER_DRAW_CMD_LIST,
        RSNodeCommandHelper::UpdateModifierDrawCmdList,
        NodeId, Drawing::DrawCmdListPtr, PropertyId, PropertyUpdateType))
ADD_COMMAND(RSUpdatePropertyDrawingMatrix,
    ARG(PERMISSION_APP, RS_NODE, UPDATE_MODIFIER_DRAWING_MATRIX,
        RSNodeCommandHelper::UpdateModifier<Drawing::Matrix>, NodeId, Drawing::Matrix, PropertyId, PropertyUpdateType))

ADD_COMMAND(RSSetFreeze,
    ARG(PERMISSION_APP, RS_NODE, SET_FREEZE,
        RSNodeCommandHelper::SetFreeze, NodeId, bool))
ADD_COMMAND(RSSetNodeName,
    ARG(PERMISSION_APP, RS_NODE, SET_NODE_NAME,
        RSNodeCommandHelper::SetNodeName, NodeId, std::string))
ADD_COMMAND(RSSetUIContextToken,
    ARG(PERMISSION_APP, BASE_NODE, SET_UICONTEXT_TOKEN,
        RSNodeCommandHelper::SetUIToken, NodeId, uint64_t))
ADD_COMMAND(RSMarkNodeGroup,
    ARG(PERMISSION_APP, RS_NODE, MARK_NODE_GROUP,
        RSNodeCommandHelper::MarkNodeGroup, NodeId, bool, bool, bool))
ADD_COMMAND(RSMarkNodeSingleFrameComposer,
    ARG(PERMISSION_APP, RS_NODE, MARK_NODE_SINGLE_FRAME_COMPOSER,
        RSNodeCommandHelper::MarkNodeSingleFrameComposer, NodeId, bool, pid_t))
ADD_COMMAND(RSMarkSuggestOpincNode,
    ARG(PERMISSION_APP, RS_NODE, MARK_SUGGEST_OPINC_NODE,
        RSNodeCommandHelper::MarkSuggestOpincNode, NodeId, bool, bool))

ADD_COMMAND(RSMarkUifirstNode,
    ARG(PERMISSION_APP, RS_NODE, MARK_UIFIRST_NODE,
        RSNodeCommandHelper::MarkUifirstNode, NodeId, bool))

ADD_COMMAND(RSForceUifirstNode,
    ARG(PERMISSION_APP, RS_NODE, MARK_UIFIRST_NODE_FORCE,
        RSNodeCommandHelper::ForceUifirstNode, NodeId, bool, bool))

ADD_COMMAND(RSSetUIFirstSwitch,
    ARG(PERMISSION_APP, RS_NODE, SET_UIFIRST_SWITCH,
        RSNodeCommandHelper::SetUIFirstSwitch, NodeId, RSUIFirstSwitch))

ADD_COMMAND(RSSetDrawRegion,
    ARG(PERMISSION_APP, RS_NODE, SET_DRAW_REGION,
        RSNodeCommandHelper::SetDrawRegion, NodeId, std::shared_ptr<RectF>))
ADD_COMMAND(RSSetOutOfParent,
    ARG(PERMISSION_APP, RS_NODE, SET_OUT_OF_PARENT,
        RSNodeCommandHelper::SetOutOfParent, NodeId, OutOfParentType))

ADD_COMMAND(RSSetTakeSurfaceForUIFlag,
    ARG(PERMISSION_APP, RS_NODE, SET_TAKE_SURFACE_CAPTURE_FOR_UI_FLAG,
        RSNodeCommandHelper::SetTakeSurfaceForUIFlag, NodeId))

ADD_COMMAND(RSRegisterGeometryTransitionNodePair,
    ARG(PERMISSION_APP, RS_NODE, REGISTER_GEOMETRY_TRANSITION,
        RSNodeCommandHelper::RegisterGeometryTransitionPair, NodeId, NodeId, bool))
ADD_COMMAND(RSUnregisterGeometryTransitionNodePair,
    ARG(PERMISSION_APP, RS_NODE, UNREGISTER_GEOMETRY_TRANSITION,
        RSNodeCommandHelper::UnregisterGeometryTransitionPair, NodeId, NodeId))
ADD_COMMAND(RSRemoveAllModifiers,
    ARG(PERMISSION_APP, RS_NODE, REMOVE_ALL_MODIFIERS,
        RSNodeCommandHelper::RemoveAllModifiers, NodeId))

ADD_COMMAND(RSDumpClientNodeTree,
    ARG(PERMISSION_APP, RS_NODE, DUMP_CLIENT_NODE_TREE,
        RSNodeCommandHelper::DumpClientNodeTree, NodeId, pid_t, uint32_t))
ADD_COMMAND(RSCommitDumpClientNodeTree,
    ARG(PERMISSION_APP, RS_NODE, COMMIT_DUMP_CLIENT_NODE_TREE,
        RSNodeCommandHelper::CommitDumpClientNodeTree, NodeId, pid_t, uint32_t, std::string))
ADD_COMMAND(RSSetDrawNodeType,
    ARG(PERMISSION_APP, RS_NODE, SET_DRAW_NODE_TYPE,
        RSNodeCommandHelper::SetDrawNodeType, NodeId, DrawNodeType))
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_NODE_COMMAND_H
