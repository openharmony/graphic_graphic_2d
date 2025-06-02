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

#ifndef RENDER_SERVICE_BASE_MODIFIER_NG_RS_MODIFIER_NG_TYPE_H
#define RENDER_SERVICE_BASE_MODIFIER_NG_RS_MODIFIER_NG_TYPE_H

#include <string>
#include <bitset>

namespace OHOS::Rosen::ModifierNG {
enum class RSModifierType : uint8_t {
    INVALID = 0,
    SAVE_ALL,

    TRANSFORM,
    ALPHA,
    VISIBILITY,

    // Bounds Geometry
    MASK,
    TRANSITION,
    ENV_FOREGROUND_COLOR,
    SHADOW,
    BEGIN_FOREGROUND_FILTER,
    OUTLINE,

    // BG properties in Bounds Clip
    BG_SAVE_BOUNDS,
    BOUNDS,
    CLIP_TO_BOUNDS,
    BLENDER,
    BACKGROUND_COLOR,
    BACKGROUND_SHADER,
    BACKGROUND_IMAGE,
    BEHIND_WINDOW_FILTER,
    BACKGROUND_FILTER,
    USE_EFFECT,
    BACKGROUND_STYLE,
    DYNAMIC_LIGHT_UP,
    BG_RESTORE_BOUNDS,

    // Frame Geometry
    SAVE_FRAME,
    FRAME,
    CLIP_TO_FRAME,
    CUSTOM_CLIP_TO_FRAME,
    CONTENT_STYLE,
    CHILDREN,
    NODE_MODIFIER,
    FOREGROUND_STYLE,
    RESTORE_FRAME,

    // FG properties in Bounds clip
    FG_SAVE_BOUNDS,
    FG_CLIP_TO_BOUNDS,
    BINARIZATION,
    COLOR_FILTER,
    LIGHT_UP_EFFECT,
    DYNAMIC_DIM,
    COMPOSITING_FILTER,
    FOREGROUND_COLOR,
    FG_RESTORE_BOUNDS,

    // No clip (unless ClipToBounds is set)
    POINT_LIGHT,
    BORDER,
    OVERLAY_STYLE,
    PARTICLE_EFFECT,
    PIXEL_STRETCH,

    // Restore state
    RESTORE_BLENDER,
    FOREGROUND_FILTER,
    RESTORE_ALL,

    // Annotations: Please remember to update this when new slots are added.
    // properties before Background, not clipped
    TRANSITION_PROPERTIES_BEGIN = SHADOW,
    TRANSITION_PROPERTIES_END   = OUTLINE,
    // background properties, clipped by bounds by default
    BG_PROPERTIES_BEGIN         = BLENDER,
    BG_PROPERTIES_END           = DYNAMIC_LIGHT_UP,
    // content properties, can be clipped by ClipToFrame and ClipToBounds
    CONTENT_BEGIN               = FRAME,
    CONTENT_END                 = FOREGROUND_STYLE,
    // foreground properties, clipped by bounds by default
    FG_PROPERTIES_BEGIN         = BINARIZATION,
    FG_PROPERTIES_END           = FOREGROUND_COLOR,
    // post-foreground properties, can be clipped by ClipToBounds
    EXTRA_PROPERTIES_BEGIN      = POINT_LIGHT,
    EXTRA_PROPERTIES_END        = PIXEL_STRETCH,

    MAX = RESTORE_ALL + 1,
};

enum class RSPropertyType : uint8_t {
#define X(name) name,
#include "modifier_ng/rs_property_ng_type.in"
#undef X
};

class RSModifierTypeString {
public:
    static std::string GetModifierTypeString(RSModifierType type)
    {
        switch (type) {
            case RSModifierType::INVALID: return "Invalid";
            case RSModifierType::SAVE_ALL: return "SaveAll";
            case RSModifierType::TRANSFORM: return "Transform";
            case RSModifierType::ALPHA: return "Alpha";
            case RSModifierType::VISIBILITY: return "Visibility";
            case RSModifierType::MASK: return "Mask";
            case RSModifierType::TRANSITION: return "Transition";
            case RSModifierType::ENV_FOREGROUND_COLOR: return "EnvForegroundColor";
            case RSModifierType::SHADOW: return "Shadow";
            case RSModifierType::BEGIN_FOREGROUND_FILTER: return "BeginForegroundFilter";
            case RSModifierType::OUTLINE: return "Outline";
            case RSModifierType::BG_SAVE_BOUNDS: return "BgSaveBounds";
            case RSModifierType::BOUNDS: return "Bounds";
            case RSModifierType::CLIP_TO_BOUNDS: return "ClipToBounds";
            case RSModifierType::BLENDER: return "Blender";
            case RSModifierType::BACKGROUND_COLOR: return "BackgroundColor";
            case RSModifierType::BACKGROUND_SHADER: return "BackgroundShader";
            case RSModifierType::BACKGROUND_IMAGE: return "BackgroundImage";
            case RSModifierType::BEHIND_WINDOW_FILTER: return "BehindWindowFilter";
            case RSModifierType::BACKGROUND_FILTER: return "BackgroundFilter";
            case RSModifierType::USE_EFFECT: return "UseEffect";
            case RSModifierType::BACKGROUND_STYLE: return "BackgroundStyle";
            case RSModifierType::DYNAMIC_LIGHT_UP: return "DynamicLightUp";
            case RSModifierType::BG_RESTORE_BOUNDS: return "BgRestoreBounds";
            case RSModifierType::SAVE_FRAME: return "SaveFrame";
            case RSModifierType::FRAME: return "Frame";
            case RSModifierType::CLIP_TO_FRAME: return "ClipToFrame";
            case RSModifierType::CUSTOM_CLIP_TO_FRAME: return "CustomClipToFrame";
            case RSModifierType::CONTENT_STYLE: return "ContentStyle";
            case RSModifierType::CHILDREN: return "Children";
            case RSModifierType::NODE_MODIFIER: return "NodeModifier";
            case RSModifierType::FOREGROUND_STYLE: return "ForegroundStyle";
            case RSModifierType::RESTORE_FRAME: return "RestoreFrame";
            case RSModifierType::FG_SAVE_BOUNDS: return "FgSaveBounds";
            case RSModifierType::FG_CLIP_TO_BOUNDS: return "FgClipToBounds";
            case RSModifierType::BINARIZATION: return "Binarization";
            case RSModifierType::COLOR_FILTER: return "Color_filter";
            case RSModifierType::LIGHT_UP_EFFECT: return "LightUpEffect";
            case RSModifierType::DYNAMIC_DIM: return "DynamicDim";
            case RSModifierType::COMPOSITING_FILTER: return "CompositingFilter";
            case RSModifierType::FOREGROUND_COLOR: return "ForegroundColor";
            case RSModifierType::FG_RESTORE_BOUNDS: return "FgRestoreBounds";
            case RSModifierType::POINT_LIGHT: return "PointLight";
            case RSModifierType::BORDER: return "Border";
            case RSModifierType::OVERLAY_STYLE: return "OverlayStyle";
            case RSModifierType::PARTICLE_EFFECT: return "ParticleEffect";
            case RSModifierType::PIXEL_STRETCH: return "PixelStretch";
            case RSModifierType::RESTORE_BLENDER: return "RestoreBlender";
            case RSModifierType::FOREGROUND_FILTER: return "ForegroundFilter";
            case RSModifierType::RESTORE_ALL: return "RestoreAll";
            case RSModifierType::MAX: return "Max";
            default: return "Unknown";
        }
        return "Unknown";
    }
};

static constexpr uint8_t MODIFIER_TYPE_COUNT = static_cast<uint8_t>(RSModifierType::MAX);
using ModifierDirtyTypes = std::bitset<static_cast<int>(MODIFIER_TYPE_COUNT)>;
} // namespace OHOS::Rosen::ModifierNG
#endif // RENDER_SERVICE_BASE_MODIFIER_NG_RS_MODIFIER_NG_TYPE_H
