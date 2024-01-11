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
    TRANSLATE,                     // 12
    TRANSLATE_Z,                   // 13
    SUBLAYER_TRANSFORM,            // 14
    CORNER_RADIUS,                 // 15
    ALPHA,                         // 16
    ALPHA_OFFSCREEN,               // 17
    FOREGROUND_COLOR,              // 18
    BACKGROUND_COLOR,              // 19
    BACKGROUND_SHADER,             // 20
    BG_IMAGE,                      // 21
    BG_IMAGE_WIDTH,                // 22
    BG_IMAGE_HEIGHT,               // 23
    BG_IMAGE_POSITION_X,           // 24
    BG_IMAGE_POSITION_Y,           // 25
    SURFACE_BG_COLOR,              // 26
    BORDER_COLOR,                  // 27
    BORDER_WIDTH,                  // 28
    BORDER_STYLE,                  // 29
    FILTER,                        // 30
    BACKGROUND_FILTER,             // 31
    LINEAR_GRADIENT_BLUR_PARA,     // 32
    DYNAMIC_LIGHT_UP_RATE,         // 33
    DYNAMIC_LIGHT_UP_DEGREE,       // 34
    FRAME_GRAVITY,                 // 35
    CLIP_RRECT,                    // 36
    CLIP_BOUNDS,                   // 37
    CLIP_TO_BOUNDS,                // 38
    CLIP_TO_FRAME,                 // 39
    VISIBLE,                       // 40
    SHADOW_COLOR,                  // 41
    SHADOW_OFFSET_X,               // 42
    SHADOW_OFFSET_Y,               // 43
    SHADOW_ALPHA,                  // 44
    SHADOW_ELEVATION,              // 45
    SHADOW_RADIUS,                 // 46
    SHADOW_PATH,                   // 47
    SHADOW_MASK,                   // 48
    SHADOW_COLOR_STRATEGY,         // 49
    MASK,                          // 50
    SPHERIZE,                      // 51
    LIGHT_UP_EFFECT,               // 52
    PIXEL_STRETCH,                 // 53
    PIXEL_STRETCH_PERCENT,         // 54
    USE_EFFECT,                    // 55
    COLOR_BLEND_MODE,              // 56
    COLOR_BLEND_APPLY_TYPE,        // 57
    SANDBOX,                       // 58
    GRAY_SCALE,                    // 59
    BRIGHTNESS,                    // 60
    CONTRAST,                      // 61
    SATURATE,                      // 62
    SEPIA,                         // 63
    INVERT,                        // 64
    AIINVERT,                      // 65
    HUE_ROTATE,                    // 66
    COLOR_BLEND,                   // 67
    PARTICLE,                      // 68
    SHADOW_IS_FILLED,              // 69
    OUTLINE_COLOR,                 // 70
    OUTLINE_WIDTH,                 // 71
    OUTLINE_STYLE,                 // 72
    OUTLINE_RADIUS,                // 73
    USE_SHADOW_BATCHING,           // 74
    GREY_COEF1,                    // 75
    GREY_COEF2,                    // 76
    LIGHT_INTENSITY,               // 77
    LIGHT_POSITION,                // 78
    ILLUMINATED_BORDER_WIDTH,      // 79
    ILLUMINATED_TYPE,              // 80
    BLOOM,                         // 81
    CUSTOM,                        // 82
    EXTENDED,                      // 83
    TRANSITION,                    // 84
    BACKGROUND_STYLE,              // 85
    CONTENT_STYLE,                 // 86
    FOREGROUND_STYLE,              // 87
    OVERLAY_STYLE,                 // 88
    NODE_MODIFIER,                 // 89
    ENV_FOREGROUND_COLOR,          // 90
    ENV_FOREGROUND_COLOR_STRATEGY, // 91
    GEOMETRYTRANS,                 // 92
#ifdef USE_ROSEN_DRAWING
    MAX_RS_MODIFIER_TYPE,
#endif
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
