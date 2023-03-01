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
    UPDATE_MODIFIER_SHADER_PTR,
    UPDATE_MODIFIER_VECTOR2F,
    UPDATE_MODIFIER_VECTOR4_BORDER_STYLE,
    UPDATE_MODIFIER_VECTOR4_COLOR,
    UPDATE_MODIFIER_VECTOR4F,
    UPDATE_MODIFIER_DRAW_CMD_LIST,
};

class RSB_EXPORT RSNodeCommandHelper {
public:
    static void AddModifier(RSContext& context, NodeId nodeId, const std::shared_ptr<RSRenderModifier>& modifier);
    static void RemoveModifier(RSContext& context, NodeId nodeId, PropertyId propertyId);

    template<typename T>
    static void UpdateModifier(RSContext& context, NodeId nodeId, T value, PropertyId id, bool isDelta)
    {
        std::shared_ptr<RSRenderPropertyBase> prop = std::make_shared<RSRenderProperty<T>>(value, id);
        auto& nodeMap = context.GetNodeMap();
        auto node = nodeMap.GetRenderNode<RSRenderNode>(nodeId);
        if (!node) {
            return;
        }
        auto modifier = node->GetModifier(id);
        if (modifier) {
            modifier->Update(prop, isDelta);
        }
    }
};

ADD_COMMAND(RSAddModifier,
    ARG(RS_NODE, ADD_MODIFIER, RSNodeCommandHelper::AddModifier, NodeId, std::shared_ptr<RSRenderModifier>))
ADD_COMMAND(RSRemoveModifier,
    ARG(RS_NODE, REMOVE_MODIFIER, RSNodeCommandHelper::RemoveModifier, NodeId, PropertyId))

ADD_COMMAND(RSUpdatePropertyBool,
    ARG(RS_NODE, UPDATE_MODIFIER_BOOL, RSNodeCommandHelper::UpdateModifier<bool>,
        NodeId, bool, PropertyId, bool))
ADD_COMMAND(RSUpdatePropertyFloat,
    ARG(RS_NODE, UPDATE_MODIFIER_FLOAT, RSNodeCommandHelper::UpdateModifier<float>,
        NodeId, float, PropertyId, bool))
ADD_COMMAND(RSUpdatePropertyInt,
    ARG(RS_NODE, UPDATE_MODIFIER_INT, RSNodeCommandHelper::UpdateModifier<int>,
        NodeId, int, PropertyId, bool))
ADD_COMMAND(RSUpdatePropertyColor,
    ARG(RS_NODE, UPDATE_MODIFIER_COLOR, RSNodeCommandHelper::UpdateModifier<Color>,
        NodeId, Color, PropertyId, bool))
ADD_COMMAND(RSUpdatePropertyGravity,
    ARG(RS_NODE, UPDATE_MODIFIER_GRAVITY, RSNodeCommandHelper::UpdateModifier<Gravity>,
        NodeId, Gravity, PropertyId, bool))
ADD_COMMAND(RSUpdatePropertyMatrix3f,
    ARG(RS_NODE, UPDATE_MODIFIER_MATRIX3F, RSNodeCommandHelper::UpdateModifier<Matrix3f>,
        NodeId, Matrix3f, PropertyId, bool))
ADD_COMMAND(RSUpdatePropertyQuaternion,
    ARG(RS_NODE, UPDATE_MODIFIER_QUATERNION, RSNodeCommandHelper::UpdateModifier<Quaternion>,
        NodeId, Quaternion, PropertyId, bool))
ADD_COMMAND(RSUpdatePropertyFilter,
    ARG(RS_NODE, UPDATE_MODIFIER_FILTER_PTR, RSNodeCommandHelper::UpdateModifier<std::shared_ptr<RSFilter>>,
        NodeId, std::shared_ptr<RSFilter>, PropertyId, bool))
ADD_COMMAND(RSUpdatePropertyImage,
    ARG(RS_NODE, UPDATE_MODIFIER_IMAGE_PTR, RSNodeCommandHelper::UpdateModifier<std::shared_ptr<RSImage>>,
        NodeId, std::shared_ptr<RSImage>, PropertyId, bool))
ADD_COMMAND(RSUpdatePropertyMask,
    ARG(RS_NODE, UPDATE_MODIFIER_MASK_PTR, RSNodeCommandHelper::UpdateModifier<std::shared_ptr<RSMask>>,
        NodeId, std::shared_ptr<RSMask>, PropertyId, bool))
ADD_COMMAND(RSUpdatePropertyPath,
    ARG(RS_NODE, UPDATE_MODIFIER_PATH_PTR, RSNodeCommandHelper::UpdateModifier<std::shared_ptr<RSPath>>,
        NodeId, std::shared_ptr<RSPath>, PropertyId, bool))
ADD_COMMAND(RSUpdatePropertyShader,
    ARG(RS_NODE, UPDATE_MODIFIER_SHADER_PTR, RSNodeCommandHelper::UpdateModifier<std::shared_ptr<RSShader>>,
        NodeId, std::shared_ptr<RSShader>, PropertyId, bool))
ADD_COMMAND(RSUpdatePropertyVector2f,
    ARG(RS_NODE, UPDATE_MODIFIER_VECTOR2F, RSNodeCommandHelper::UpdateModifier<Vector2f>,
        NodeId, Vector2f, PropertyId, bool))
ADD_COMMAND(RSUpdatePropertyBorderStyle,
    ARG(RS_NODE, UPDATE_MODIFIER_VECTOR4_BORDER_STYLE, RSNodeCommandHelper::UpdateModifier<Vector4<uint32_t>>,
        NodeId, Vector4<uint32_t>, PropertyId, bool))
ADD_COMMAND(RSUpdatePropertyVector4Color,
    ARG(RS_NODE, UPDATE_MODIFIER_VECTOR4_COLOR, RSNodeCommandHelper::UpdateModifier<Vector4<Color>>,
        NodeId, Vector4<Color>, PropertyId, bool))
ADD_COMMAND(RSUpdatePropertyVector4f,
    ARG(RS_NODE, UPDATE_MODIFIER_VECTOR4F, RSNodeCommandHelper::UpdateModifier<Vector4f>,
        NodeId, Vector4f, PropertyId, bool))
ADD_COMMAND(RSUpdatePropertyDrawCmdList,
    ARG(RS_NODE, UPDATE_MODIFIER_DRAW_CMD_LIST, RSNodeCommandHelper::UpdateModifier<DrawCmdListPtr>,
        NodeId, DrawCmdListPtr, PropertyId, bool))

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_NODE_COMMAND_H
