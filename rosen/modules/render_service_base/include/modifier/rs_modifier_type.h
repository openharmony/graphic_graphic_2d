/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_MODIFIER_TYPE_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_MODIFIER_TYPE_H

#include <bitset>
#include <cstdint>

namespace OHOS {
namespace Rosen {
// NOTE:
// 1. Following LUTs must be updated according when this enum is updated :
//    a. g_propertyResetterLUT in rs_properties.cpp
//    b. g_propertyToDrawableLut in rs_drawable_content.cpp
// 2. Property modifier(i.e. to be applied to RSProperties) MUST be added before CUSTOM enum, else wise it will not work
enum class RSModifierType : int16_t {
    INVALID = 0,                   // 0
    BOUNDS,                        // 1
    FRAME,                         // 2
    POSITION_Z,                    // 3
    PIVOT,                         // 4
    PIVOT_Z,                       // 5
    QUATERNION,                    // 6
    ROTATION,                      // 7
    ROTATION_X,                    // 8
    ROTATION_Y,                    // 9
    CAMERA_DISTANCE,               // 10
    SCALE,                         // 11
    SKEW,                          // 12
    TRANSLATE,                     // 13
    TRANSLATE_Z,                   // 14
    SUBLAYER_TRANSFORM,            // 15
    CORNER_RADIUS,                 // 16
    ALPHA,                         // 17
    ALPHA_OFFSCREEN,               // 18
    FOREGROUND_COLOR,              // 19
    BACKGROUND_COLOR,              // 20
    BACKGROUND_SHADER,             // 21
    BG_IMAGE,                      // 22
    BG_IMAGE_WIDTH,                // 23
    BG_IMAGE_HEIGHT,               // 24
    BG_IMAGE_POSITION_X,           // 25
    BG_IMAGE_POSITION_Y,           // 26
    SURFACE_BG_COLOR,              // 27
    BORDER_COLOR,                  // 28
    BORDER_WIDTH,                  // 29
    BORDER_STYLE,                  // 30
    FILTER,                        // 31
    BACKGROUND_FILTER,             // 32
    LINEAR_GRADIENT_BLUR_PARA,     // 33
    DYNAMIC_LIGHT_UP_RATE,         // 34
    DYNAMIC_LIGHT_UP_DEGREE,       // 35
    FRAME_GRAVITY,                 // 36
    CLIP_RRECT,                    // 37
    CLIP_BOUNDS,                   // 38
    CLIP_TO_BOUNDS,                // 39
    CLIP_TO_FRAME,                 // 40
    VISIBLE,                       // 41
    SHADOW_COLOR,                  // 42
    SHADOW_OFFSET_X,               // 43
    SHADOW_OFFSET_Y,               // 44
    SHADOW_ALPHA,                  // 45
    SHADOW_ELEVATION,              // 46
    SHADOW_RADIUS,                 // 47
    SHADOW_PATH,                   // 48
    SHADOW_MASK,                   // 49
    SHADOW_COLOR_STRATEGY,         // 50
    MASK,                          // 51
    SPHERIZE,                      // 52
    LIGHT_UP_EFFECT,               // 53
    PIXEL_STRETCH,                 // 54
    PIXEL_STRETCH_PERCENT,         // 55
    USE_EFFECT,                    // 56
    COLOR_BLEND_MODE,              // 57
    COLOR_BLEND_APPLY_TYPE,        // 58
    SANDBOX,                       // 59
    GRAY_SCALE,                    // 60
    BRIGHTNESS,                    // 61
    CONTRAST,                      // 62
    SATURATE,                      // 63
    SEPIA,                         // 64
    INVERT,                        // 65
    AIINVERT,                      // 66
    SYSTEMBAREFFECT,               // 67
    HUE_ROTATE,                    // 68
    COLOR_BLEND,                   // 69
    PARTICLE,                      // 70
    SHADOW_IS_FILLED,              // 71
    OUTLINE_COLOR,                 // 72
    OUTLINE_WIDTH,                 // 73
    OUTLINE_STYLE,                 // 74
    OUTLINE_RADIUS,                // 75
    USE_SHADOW_BATCHING,           // 76
    GREY_COEF1,                    // 77
    GREY_COEF2,                    // 78
    LIGHT_INTENSITY,               // 79
    LIGHT_POSITION,                // 80
    ILLUMINATED_BORDER_WIDTH,      // 81
    ILLUMINATED_TYPE,              // 82
    BLOOM,                         // 83
    CUSTOM,                        // 84
    EXTENDED,                      // 85
    TRANSITION,                    // 86
    BACKGROUND_STYLE,              // 87
    CONTENT_STYLE,                 // 88
    FOREGROUND_STYLE,              // 89
    OVERLAY_STYLE,                 // 90
    NODE_MODIFIER,                 // 91
    ENV_FOREGROUND_COLOR,          // 92
    ENV_FOREGROUND_COLOR_STRATEGY, // 93
    GEOMETRYTRANS,                 // 94
    CHILDREN,                      // 95, PLACEHOLDER, no such modifier, but we need a dirty flag
    MAX_RS_MODIFIER_TYPE,
};
using ModifierDirtyTypes = std::bitset<static_cast<int>(RSModifierType::MAX_RS_MODIFIER_TYPE)>;

enum class RSRenderPropertyType : int16_t {
    INVALID = 0,
    PROPERTY_FLOAT,
    PROPERTY_COLOR,
    PROPERTY_MATRIX3F,
    PROPERTY_QUATERNION,
    PROPERTY_FILTER,
    PROPERTY_VECTOR2F,
    PROPERTY_VECTOR4F,
    PROPERTY_VECTOR4_COLOR,
    PROPERTY_SKMATRIX,
    PROPERTY_RRECT,
};

enum class RSPropertyUnit : int16_t {
    UNKNOWN = 0,
    PIXEL_POSITION,
    PIXEL_SIZE,
    RATIO_SCALE,
    ANGLE_ROTATION,
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_MODIFIER_TYPE_H
