/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_PROPERTY_NODE_COMMAND_H
#define ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_PROPERTY_NODE_COMMAND_H

#include "command/rs_command_templates.h"
#include "pipeline/rs_property_render_node.h"
#include "property/rs_properties.h"

namespace OHOS {
namespace Rosen {

enum RSPropertyNodeCommandType : uint16_t {
    BOUNDS_WIDTH_CHANGED,
    BOUNDS_HEIGHT_CHANGED,
    BOUNDS_POSITION_CHANGED,
    BOUNDS_POSITION_X_CHANGED,
    BOUNDS_POSITION_Y_CHANGED,
    FRAME_WIDTH_CHANGED,
    FRAME_HEIGHT_CHANGED,
    FRAME_POSITION_CHANGED,
    FRAME_POSITION_X_CHANGED,
    FRAME_POSITION_Y_CHANGED,
    POSITION_Z_CHANGED,
    PIVOT_X_CHANGED,
    PIVOT_Y_CHANGED,
    CORNER_RADIUS_CHANGED,
    QUATERNION_CHANGED,
    ROTATION_CHANGED,
    ROTATION_X_CHANGED,
    ROTATION_Y_CHANGED,
    SCALE_X_CHANGED,
    SCALE_Y_CHANGED,
    TRANSLATE_CHANGED,
    TRANSLATE_X_CHANGED,
    TRANSLATE_Y_CHANGED,
    TRANSLATE_Z_CHANGED,
    ALPHA_CHANGED,
    FG_COLOR_CHANGED,
    BG_COLOR_CHANGED,
    BG_SHADER_CHANGED,
    BG_IMAGE_CHANGED,
    BG_IMAGE_WIDTH_CHANGED,
    BG_IMAGE_HEIGHT_CHANGED,
    BG_IMAGE_POSITION_X_CHANGED,
    BG_IMAGE_POSITION_Y_CHANGED,
    BORDER_COLOR_CHANGED,
    BORDER_WIDTH_CHANGED,
    BORDER_STYLE_CHANGED,
    SUBLAYER_TRANSFORM_CHANGED,
    BG_FILTER_CHANGED,
    FILTER_CHANGED,
    COMPOSITING_FILTER_CHANGED,
    FRAME_GRAVITY_CHANGED,
    CLIP_BOUNDS_CHANGED,
    CLIP_TO_BOUNDS_CHANGED,
    CLIP_TO_FRAME_CHANGED,
    VISIBLE_CHANGED,
    SHADOW_COLOR_CHANGED,
    SHADOW_OFFSET_X_CHANGED,
    SHADOW_OFFSET_Y_CHANGED,
    SHADOW_ALPHA_CHANGED,
    SHADOW_ELEVATION_CHANGED,
    SHADOW_RADIUS_CHANGED,
    SHADOW_PATH_CHANGED,

    BOUNDS_WIDTH_DELTA,
    BOUNDS_HEIGHT_DELTA,
    BOUNDS_POSITION_DELTA,
    BOUNDS_POSITION_X_DELTA,
    BOUNDS_POSITION_Y_DELTA,
    FRAME_WIDTH_DELTA,
    FRAME_HEIGHT_DELTA,
    FRAME_POSITION_DELTA,
    FRAME_POSITION_X_DELTA,
    FRAME_POSITION_Y_DELTA,
    POSITION_Z_DELTA,
    PIVOT_X_DELTA,
    PIVOT_Y_DELTA,
    CORNER_RADIUS_DELTA,
    QUATERNION_DELTA,
    ROTATION_DELTA,
    ROTATION_X_DELTA,
    ROTATION_Y_DELTA,
    SCALE_X_DELTA,
    SCALE_Y_DELTA,
    TRANSLATE_DELTA,
    TRANSLATE_X_DELTA,
    TRANSLATE_Y_DELTA,
    TRANSLATE_Z_DELTA,
    ALPHA_DELTA,
    FG_COLOR_DELTA,
    BG_COLOR_DELTA,
    BG_SHADER_DELTA,
    BG_IMAGE_WIDTH_DELTA,
    BG_IMAGE_HEIGHT_DELTA,
    BG_IMAGE_POSITION_X_DELTA,
    BG_IMAGE_POSITION_Y_DELTA,
    BORDER_COLOR_DELTA,
    BORDER_WIDTH_DELTA,
    SUBLAYER_TRANSFORM_DELTA,
    BG_FILTER_DELTA,
    FILTER_DELTA,
    COMPOSITING_FILTER_DELTA,
    FRAME_GRAVITY_DELTA,
    SHADOW_COLOR_DELTA,
    SHADOW_OFFSET_X_DELTA,
    SHADOW_OFFSET_Y_DELTA,
    SHADOW_ALPHA_DELTA,
    SHADOW_ELEVATION_DELTA,
    SHADOW_RADIUS_DELTA,
};

class PropertyNodeCommandHelper {
public:
    // second parameter of SETTER is useless, should be removed
    template<typename T, void (RSProperties::*setter)(T value, bool)>
    static void SetProperty(RSContext& context, NodeId nodeId, const T& value)
    {
        if (auto node = context.GetNodeMap().GetRenderNode<RSPropertyRenderNode>(nodeId)) {
            (node->GetRenderProperties().*setter)(value, false);
        }
    }

    template<typename T, void (RSProperties::*setter)(T value, bool), T (RSProperties::*getter)() const>
    static void SetPropertyDelta(RSContext& context, NodeId nodeId, const T& value)
    {
        if (auto node = context.GetNodeMap().GetRenderNode<RSPropertyRenderNode>(nodeId)) {
            T newValue = (node->GetRenderProperties().*getter)() + value;
            (node->GetRenderProperties().*setter)(newValue, false);
        }
    }
};

// declear commands like RSPropertyRenderNodeAlphaChanged and RSPropertyRenderNodeAlphaDelta
#define DECLARE_COMMAND(COMMAND_NAME, SUBCOMMAND, TYPE, SETTER) \
    ADD_COMMAND(COMMAND_NAME, ARG(PROPERTY_NODE, SUBCOMMAND,    \
                                  PropertyNodeCommandHelper::SetProperty<TYPE, &RSProperties::SETTER>, NodeId, TYPE))
#define DECLARE_DELTA_COMMAND(COMMAND_NAME, SUBCOMMAND, TYPE, SETTER, GETTER)                                        \
    ADD_COMMAND(COMMAND_NAME,                                                                                        \
        ARG(PROPERTY_NODE, SUBCOMMAND,                                                                               \
            PropertyNodeCommandHelper::SetPropertyDelta<TYPE, &RSProperties::SETTER, &RSProperties::GETTER>, NodeId, \
            TYPE))

#include "rs_property_node_command.in"

#undef DECLARE_COMMAND
#undef DECLARE_DELTA_COMMAND

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_PROPERTY_NODE_COMMAND_H
