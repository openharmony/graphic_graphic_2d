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

#include <cstdint>

namespace OHOS {
namespace Rosen {
// NOTE:
// 1. MUST update g_propertyResetterLUT in rs_properties.cpp when new properties are added
// 2. property modifier MUST be added before CUSTOM, else wise it will be treated as draw modifier
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
    BG_IMAGE_INNER_RECT,           // 23
    BG_IMAGE_WIDTH,                // 24
    BG_IMAGE_HEIGHT,               // 25
    BG_IMAGE_POSITION_X,           // 26
    BG_IMAGE_POSITION_Y,           // 27
    SURFACE_BG_COLOR,              // 28
    BORDER_COLOR,                  // 29
    BORDER_WIDTH,                  // 30
    BORDER_STYLE,                  // 31
    FILTER,                        // 32
    BACKGROUND_FILTER,             // 33
    LINEAR_GRADIENT_BLUR_PARA,     // 34
    DYNAMIC_LIGHT_UP_RATE,         // 35
    DYNAMIC_LIGHT_UP_DEGREE,       // 36
    FRAME_GRAVITY,                 // 37
    CLIP_RRECT,                    // 38
    CLIP_BOUNDS,                   // 39
    CLIP_TO_BOUNDS,                // 40
    CLIP_TO_FRAME,                 // 41
    VISIBLE,                       // 42
    SHADOW_COLOR,                  // 43
    SHADOW_OFFSET_X,               // 44
    SHADOW_OFFSET_Y,               // 45
    SHADOW_ALPHA,                  // 46
    SHADOW_ELEVATION,              // 47
    SHADOW_RADIUS,                 // 48
    SHADOW_PATH,                   // 49
    SHADOW_MASK,                   // 50
    SHADOW_COLOR_STRATEGY,         // 51
    MASK,                          // 52
    SPHERIZE,                      // 53
    LIGHT_UP_EFFECT,               // 54
    PIXEL_STRETCH,                 // 55
    PIXEL_STRETCH_PERCENT,         // 56
    USE_EFFECT,                    // 57
    COLOR_BLEND_MODE,              // 58
    COLOR_BLEND_APPLY_TYPE,        // 59
    SANDBOX,                       // 60
    GRAY_SCALE,                    // 61
    BRIGHTNESS,                    // 62
    CONTRAST,                      // 63
    SATURATE,                      // 64
    SEPIA,                         // 65
    INVERT,                        // 66
    AIINVERT,                      // 67
    SYSTEMBAREFFECT,               // 68
    HUE_ROTATE,                    // 69
    COLOR_BLEND,                   // 70
    PARTICLE,                      // 71
    SHADOW_IS_FILLED,              // 72
    OUTLINE_COLOR,                 // 73
    OUTLINE_WIDTH,                 // 74
    OUTLINE_STYLE,                 // 75
    OUTLINE_RADIUS,                // 76
    USE_SHADOW_BATCHING,           // 77
    GREY_COEF,                     // 78
    LIGHT_INTENSITY,               // 79
    LIGHT_POSITION,                // 80
    ILLUMINATED_BORDER_WIDTH,      // 81
    ILLUMINATED_TYPE,              // 82
    BLOOM,                         // 83
    PARTICLE_EMITTER_UPDATER,      // 84
    DYNAMIC_DIM_DEGREE,            // 85
    CUSTOM,                        // 86
    EXTENDED,                      // 87
    TRANSITION,                    // 88
    BACKGROUND_STYLE,              // 89
    CONTENT_STYLE,                 // 90
    FOREGROUND_STYLE,              // 91
    OVERLAY_STYLE,                 // 92
    NODE_MODIFIER,                 // 93
    ENV_FOREGROUND_COLOR,          // 94
    ENV_FOREGROUND_COLOR_STRATEGY, // 95
    GEOMETRYTRANS,                 // 96
    MAX_RS_MODIFIER_TYPE,
};

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
