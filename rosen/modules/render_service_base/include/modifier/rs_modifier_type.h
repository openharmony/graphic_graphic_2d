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
    MASK,                          // 49
    SPHERIZE,                      // 50
    LIGHT_UP_EFFECT,               // 51
    PIXEL_STRETCH,                 // 52
    PIXEL_STRETCH_PERCENT,         // 53
    USE_EFFECT,                    // 54
    SANDBOX,                       // 55
    GRAY_SCALE,                    // 56
    BRIGHTNESS,                    // 57
    CONTRAST,                      // 58
    SATURATE,                      // 59
    SEPIA,                         // 60
    INVERT,                        // 61
    HUE_ROTATE,                    // 62
    COLOR_BLEND,                   // 63
    PARTICLE,                      // 64
    CUSTOM,                        // 65
    EXTENDED,                      // 66
    TRANSITION,                    // 67
    BACKGROUND_STYLE,              // 68
    CONTENT_STYLE,                 // 69
    FOREGROUND_STYLE,              // 70
    OVERLAY_STYLE,                 // 71
    NODE_MODIFIER,                 // 72
    ENV_FOREGROUND_COLOR,          // 73
    ENV_FOREGROUND_COLOR_STRATEGY, // 74
    GEOMETRYTRANS,                 // 75
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
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_MODIFIER_TYPE_H
