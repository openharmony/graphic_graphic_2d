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
    PERSP,                         // 13
    TRANSLATE,                     // 14
    TRANSLATE_Z,                   // 15
    SUBLAYER_TRANSFORM,            // 16
    CORNER_RADIUS,                 // 17
    ALPHA,                         // 18
    ALPHA_OFFSCREEN,               // 19
    FOREGROUND_COLOR,              // 20
    BACKGROUND_COLOR,              // 21
    BACKGROUND_SHADER,             // 22
    BG_IMAGE,                      // 23
    BG_IMAGE_INNER_RECT,           // 24
    BG_IMAGE_WIDTH,                // 25
    BG_IMAGE_HEIGHT,               // 26
    BG_IMAGE_POSITION_X,           // 27
    BG_IMAGE_POSITION_Y,           // 28
    SURFACE_BG_COLOR,              // 29
    BORDER_COLOR,                  // 30
    BORDER_WIDTH,                  // 31
    BORDER_STYLE,                  // 32
    FILTER,                        // 33
    BACKGROUND_FILTER,             // 34
    LINEAR_GRADIENT_BLUR_PARA,     // 35
    DYNAMIC_LIGHT_UP_RATE,         // 36
    DYNAMIC_LIGHT_UP_DEGREE,       // 37
    FRAME_GRAVITY,                 // 38
    CLIP_RRECT,                    // 39
    CLIP_BOUNDS,                   // 40
    CLIP_TO_BOUNDS,                // 41
    CLIP_TO_FRAME,                 // 42
    VISIBLE,                       // 43
    SHADOW_COLOR,                  // 44
    SHADOW_OFFSET_X,               // 45
    SHADOW_OFFSET_Y,               // 46
    SHADOW_ALPHA,                  // 47
    SHADOW_ELEVATION,              // 48
    SHADOW_RADIUS,                 // 49
    SHADOW_PATH,                   // 50
    SHADOW_MASK,                   // 51
    SHADOW_COLOR_STRATEGY,         // 52
    MASK,                          // 53
    SPHERIZE,                      // 54
    LIGHT_UP_EFFECT,               // 55
    PIXEL_STRETCH,                 // 56
    PIXEL_STRETCH_PERCENT,         // 57
    USE_EFFECT,                    // 58
    COLOR_BLEND_MODE,              // 59
    COLOR_BLEND_APPLY_TYPE,        // 60
    SANDBOX,                       // 61
    GRAY_SCALE,                    // 62
    BRIGHTNESS,                    // 63
    CONTRAST,                      // 64
    SATURATE,                      // 65
    SEPIA,                         // 66
    INVERT,                        // 67
    AIINVERT,                      // 68
    SYSTEMBAREFFECT,               // 69
    HUE_ROTATE,                    // 70
    COLOR_BLEND,                   // 71
    PARTICLE,                      // 72
    SHADOW_IS_FILLED,              // 73
    OUTLINE_COLOR,                 // 74
    OUTLINE_WIDTH,                 // 75
    OUTLINE_STYLE,                 // 76
    OUTLINE_RADIUS,                // 77
    USE_SHADOW_BATCHING,           // 78
    GREY_COEF,                     // 79
    LIGHT_INTENSITY,               // 80
    LIGHT_COLOR,                   // 81
    LIGHT_POSITION,                // 82
    ILLUMINATED_BORDER_WIDTH,      // 83
    ILLUMINATED_TYPE,              // 84
    BLOOM,                         // 85
    PARTICLE_EMITTER_UPDATER,      // 86
    FOREGROUND_EFFECT_RADIUS,      // 87
    MOTION_BLUR_PARA,              // 88
    DYNAMIC_DIM_DEGREE,            // 89
    BACKGROUND_BLUR_RADIUS,        // 90
    BACKGROUND_BLUR_SATURATION,    // 91
    BACKGROUND_BLUR_BRIGHTNESS,    // 92
    BACKGROUND_BLUR_MASK_COLOR,    // 93
    BACKGROUND_BLUR_COLOR_MODE,    // 94
    BACKGROUND_BLUR_RADIUS_X,      // 95
    BACKGROUND_BLUR_RADIUS_Y,      // 96
    FOREGROUND_BLUR_RADIUS,        // 97
    FOREGROUND_BLUR_SATURATION,    // 98
    FOREGROUND_BLUR_BRIGHTNESS,    // 99
    FOREGROUND_BLUR_MASK_COLOR,    // 100
    FOREGROUND_BLUR_COLOR_MODE,    // 101
    FOREGROUND_BLUR_RADIUS_X,      // 102
    FOREGROUND_BLUR_RADIUS_Y,      // 103
    CUSTOM,                        // 104
    EXTENDED,                      // 105
    TRANSITION,                    // 106
    BACKGROUND_STYLE,              // 107
    CONTENT_STYLE,                 // 108
    FOREGROUND_STYLE,              // 109
    OVERLAY_STYLE,                 // 110
    NODE_MODIFIER,                 // 111
    ENV_FOREGROUND_COLOR,          // 112
    ENV_FOREGROUND_COLOR_STRATEGY, // 113
    GEOMETRYTRANS,                 // 114
    CHILDREN,                      // 115, PLACEHOLDER, no such modifier, but we need a dirty flag
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
