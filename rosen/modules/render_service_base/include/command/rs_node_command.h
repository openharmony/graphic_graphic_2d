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

enum RSNodeCommandType : uint16_t {
    ADD_MODIFIER,
    REMOVE_MODIFIER,

    UPDATE_MODIFIER_BOOL,
    UPDATE_MODIFIER_FLOAT,
    UPDATE_MODIFIER_INT,
    UPDATE_MODIFIER_COLOR,
    UPDATE_MODIFIER_GRAVITY,
    UPDATE_MODIFIER_MATRIX3F,
    UPDATE_MODIFIER_QUATERNION,
    UPDATE_MODIFIER_FILTER_PTR,
    UPDATE_MODIFIER_IMAGE_PTR,
    UPDATE_MODIFIER_MASK_PTR,
    UPDATE_MODIFIER_PATH_PTR,
    UPDATE_MODIFIER_GRADIENT_BLUR_PTR,
    UPDATE_MODIFIER_SHADER_PTR,
    UPDATE_MODIFIER_VECTOR2F,
    UPDATE_MODIFIER_VECTOR4_BORDER_STYLE,
    UPDATE_MODIFIER_VECTOR4_COLOR,
    UPDATE_MODIFIER_VECTOR4F,
    UPDATE_MODIFIER_RRECT,
    UPDATE_MODIFIER_DRAW_CMD_LIST,
    UPDATE_MODIFIER_DRAWING_MATRIX,

    SET_FREEZE,
    SET_DRAW_REGION,
    SET_OUT_OF_PARENT,

    REGISTER_GEOMETRY_TRANSITION,
    UNREGISTER_GEOMETRY_TRANSITION,

    MARK_NODE_GROUP,
    MARK_NODE_SINGLE_FRAME_COMPOSER,
};

class RSB_EXPORT RSNodeCommandHelper {
public:
    static void AddModifier(RSContext& context, NodeId nodeId, const std::shared_ptr<RSRenderModifier>& modifier);
    static void RemoveModifier(RSContext& context, NodeId nodeId, PropertyId propertyId);

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
    static void MarkNodeGroup(RSContext& context, NodeId nodeId, bool isNodeGroup, bool isForced,
        bool includeProperty);
    static void MarkNodeSingleFrameComposer(RSContext& context, NodeId nodeId, bool isNodeFasterDraw, pid_t pid);

    static void SetDrawRegion(RSContext& context, NodeId nodeId, std::shared_ptr<RectF> rect);
    static void SetOutOfParent(RSContext& context, NodeId nodeId, OutOfParentType outOfParent);

    static void RegisterGeometryTransitionPair(RSContext& context, NodeId inNodeId, NodeId outNodeId);
    static void UnregisterGeometryTransitionPair(RSContext& context, NodeId inNodeId, NodeId outNodeId);
};

ADD_COMMAND(RSAddModifier,
    ARG(RS_NODE, ADD_MODIFIER, RSNodeCommandHelper::AddModifier, NodeId, std::shared_ptr<RSRenderModifier>))
ADD_COMMAND(RSRemoveModifier,
    ARG(RS_NODE, REMOVE_MODIFIER, RSNodeCommandHelper::RemoveModifier, NodeId, PropertyId))

ADD_COMMAND(RSUpdatePropertyBool,
    ARG(RS_NODE, UPDATE_MODIFIER_BOOL, RSNodeCommandHelper::UpdateModifier<bool>,
        NodeId, bool, PropertyId, PropertyUpdateType))
ADD_COMMAND(RSUpdatePropertyFloat,
    ARG(RS_NODE, UPDATE_MODIFIER_FLOAT, RSNodeCommandHelper::UpdateModifier<float>,
        NodeId, float, PropertyId, PropertyUpdateType))
ADD_COMMAND(RSUpdatePropertyInt,
    ARG(RS_NODE, UPDATE_MODIFIER_INT, RSNodeCommandHelper::UpdateModifier<int>,
        NodeId, int, PropertyId, PropertyUpdateType))
ADD_COMMAND(RSUpdatePropertyColor,
    ARG(RS_NODE, UPDATE_MODIFIER_COLOR, RSNodeCommandHelper::UpdateModifier<Color>,
        NodeId, Color, PropertyId, PropertyUpdateType))
ADD_COMMAND(RSUpdatePropertyGravity,
    ARG(RS_NODE, UPDATE_MODIFIER_GRAVITY, RSNodeCommandHelper::UpdateModifier<Gravity>,
        NodeId, Gravity, PropertyId, PropertyUpdateType))
ADD_COMMAND(RSUpdatePropertyMatrix3f,
    ARG(RS_NODE, UPDATE_MODIFIER_MATRIX3F, RSNodeCommandHelper::UpdateModifier<Matrix3f>,
        NodeId, Matrix3f, PropertyId, PropertyUpdateType))
ADD_COMMAND(RSUpdatePropertyQuaternion,
    ARG(RS_NODE, UPDATE_MODIFIER_QUATERNION, RSNodeCommandHelper::UpdateModifier<Quaternion>,
        NodeId, Quaternion, PropertyId, PropertyUpdateType))
ADD_COMMAND(RSUpdatePropertyFilter,
    ARG(RS_NODE, UPDATE_MODIFIER_FILTER_PTR, RSNodeCommandHelper::UpdateModifier<std::shared_ptr<RSFilter>>,
        NodeId, std::shared_ptr<RSFilter>, PropertyId, PropertyUpdateType))
ADD_COMMAND(RSUpdatePropertyImage,
    ARG(RS_NODE, UPDATE_MODIFIER_IMAGE_PTR, RSNodeCommandHelper::UpdateModifier<std::shared_ptr<RSImage>>,
        NodeId, std::shared_ptr<RSImage>, PropertyId, PropertyUpdateType))
ADD_COMMAND(RSUpdatePropertyMask,
    ARG(RS_NODE, UPDATE_MODIFIER_MASK_PTR, RSNodeCommandHelper::UpdateModifier<std::shared_ptr<RSMask>>,
        NodeId, std::shared_ptr<RSMask>, PropertyId, PropertyUpdateType))
ADD_COMMAND(RSUpdatePropertyPath,
    ARG(RS_NODE, UPDATE_MODIFIER_PATH_PTR, RSNodeCommandHelper::UpdateModifier<std::shared_ptr<RSPath>>,
        NodeId, std::shared_ptr<RSPath>, PropertyId, PropertyUpdateType))
ADD_COMMAND(RSUpdatePropertyLinearGradientBlurPara,
    ARG(RS_NODE, UPDATE_MODIFIER_GRADIENT_BLUR_PTR,
        RSNodeCommandHelper::UpdateModifier<std::shared_ptr<RSLinearGradientBlurPara>>,
        NodeId, std::shared_ptr<RSLinearGradientBlurPara>, PropertyId, PropertyUpdateType))
ADD_COMMAND(RSUpdatePropertyShader,
    ARG(RS_NODE, UPDATE_MODIFIER_SHADER_PTR, RSNodeCommandHelper::UpdateModifier<std::shared_ptr<RSShader>>,
        NodeId, std::shared_ptr<RSShader>, PropertyId, PropertyUpdateType))
ADD_COMMAND(RSUpdatePropertyVector2f,
    ARG(RS_NODE, UPDATE_MODIFIER_VECTOR2F, RSNodeCommandHelper::UpdateModifier<Vector2f>,
        NodeId, Vector2f, PropertyId, PropertyUpdateType))
ADD_COMMAND(RSUpdatePropertyBorderStyle,
    ARG(RS_NODE, UPDATE_MODIFIER_VECTOR4_BORDER_STYLE, RSNodeCommandHelper::UpdateModifier<Vector4<uint32_t>>,
        NodeId, Vector4<uint32_t>, PropertyId, PropertyUpdateType))
ADD_COMMAND(RSUpdatePropertyVector4Color,
    ARG(RS_NODE, UPDATE_MODIFIER_VECTOR4_COLOR, RSNodeCommandHelper::UpdateModifier<Vector4<Color>>,
        NodeId, Vector4<Color>, PropertyId, PropertyUpdateType))
ADD_COMMAND(RSUpdatePropertyVector4f,
    ARG(RS_NODE, UPDATE_MODIFIER_VECTOR4F, RSNodeCommandHelper::UpdateModifier<Vector4f>,
        NodeId, Vector4f, PropertyId, PropertyUpdateType))
ADD_COMMAND(RSUpdatePropertyRRect,
    ARG(RS_NODE, UPDATE_MODIFIER_RRECT, RSNodeCommandHelper::UpdateModifier<RRect>,
        NodeId, RRect, PropertyId, PropertyUpdateType))
ADD_COMMAND(RSUpdatePropertyDrawCmdList,
    ARG(RS_NODE, UPDATE_MODIFIER_DRAW_CMD_LIST, RSNodeCommandHelper::UpdateModifierDrawCmdList,
        NodeId, Drawing::DrawCmdListPtr, PropertyId, PropertyUpdateType))
ADD_COMMAND(RSUpdatePropertyDrawingMatrix,
    ARG(RS_NODE, UPDATE_MODIFIER_DRAWING_MATRIX, RSNodeCommandHelper::UpdateModifier<Drawing::Matrix>,
        NodeId, Drawing::Matrix, PropertyId, PropertyUpdateType))

ADD_COMMAND(RSSetFreeze,
    ARG(RS_NODE, SET_FREEZE, RSNodeCommandHelper::SetFreeze, NodeId, bool))
ADD_COMMAND(RSMarkNodeGroup,
    ARG(RS_NODE, MARK_NODE_GROUP, RSNodeCommandHelper::MarkNodeGroup, NodeId, bool, bool, bool))
ADD_COMMAND(RSMarkNodeSingleFrameComposer,
    ARG(RS_NODE, MARK_NODE_SINGLE_FRAME_COMPOSER, RSNodeCommandHelper::MarkNodeSingleFrameComposer,
        NodeId, bool, pid_t))

ADD_COMMAND(RSSetDrawRegion,
    ARG(RS_NODE, SET_DRAW_REGION, RSNodeCommandHelper::SetDrawRegion,
        NodeId, std::shared_ptr<RectF>))
ADD_COMMAND(RSSetOutOfParent,
    ARG(RS_NODE, SET_OUT_OF_PARENT, RSNodeCommandHelper::SetOutOfParent,
        NodeId, OutOfParentType))

ADD_COMMAND(RSRegisterGeometryTransitionNodePair,
    ARG(RS_NODE, REGISTER_GEOMETRY_TRANSITION, RSNodeCommandHelper::RegisterGeometryTransitionPair, NodeId, NodeId))
ADD_COMMAND(RSUnregisterGeometryTransitionNodePair,
    ARG(RS_NODE, UNREGISTER_GEOMETRY_TRANSITION, RSNodeCommandHelper::UnregisterGeometryTransitionPair, NodeId, NodeId))
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_NODE_COMMAND_H
