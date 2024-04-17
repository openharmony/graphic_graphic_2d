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
    INVALID = 0,
    BOUNDS,
    FRAME,
    POSITION_Z,
    PIVOT,
    PIVOT_Z,
    QUATERNION,
    ROTATION,
    ROTATION_X,
    ROTATION_Y,
    CAMERA_DISTANCE,
    SCALE,
    SKEW,
    PERSP,
    TRANSLATE,
    TRANSLATE_Z,
    SUBLAYER_TRANSFORM,
    CORNER_RADIUS,
    ALPHA,
    ALPHA_OFFSCREEN,
    FOREGROUND_COLOR,
    BACKGROUND_COLOR,
    BACKGROUND_SHADER,
    BG_IMAGE,
    BG_IMAGE_INNER_RECT,
    BG_IMAGE_WIDTH,
    BG_IMAGE_HEIGHT,
    BG_IMAGE_POSITION_X,
    BG_IMAGE_POSITION_Y,
    SURFACE_BG_COLOR,
    BORDER_COLOR,
    BORDER_WIDTH,
    BORDER_STYLE,
    FILTER,
    BACKGROUND_FILTER,
    LINEAR_GRADIENT_BLUR_PARA,
    DYNAMIC_LIGHT_UP_RATE,
    DYNAMIC_LIGHT_UP_DEGREE,
    FRAME_GRAVITY,
    CLIP_RRECT,
    CLIP_BOUNDS,
    CLIP_TO_BOUNDS,
    CLIP_TO_FRAME,
    VISIBLE,
    SHADOW_COLOR,
    SHADOW_OFFSET_X,
    SHADOW_OFFSET_Y,
    SHADOW_ALPHA,
    SHADOW_ELEVATION,
    SHADOW_RADIUS,
    SHADOW_PATH,
    SHADOW_MASK,
    SHADOW_COLOR_STRATEGY,
    MASK,
    SPHERIZE,
    LIGHT_UP_EFFECT,
    PIXEL_STRETCH,
    PIXEL_STRETCH_PERCENT,
    USE_EFFECT,
    COLOR_BLEND_MODE,
    COLOR_BLEND_APPLY_TYPE,
    SANDBOX,
    GRAY_SCALE,
    BRIGHTNESS,
    CONTRAST,
    SATURATE,
    SEPIA,
    INVERT,
    AIINVERT,
    SYSTEMBAREFFECT,
    HUE_ROTATE,
    COLOR_BLEND,
    PARTICLE,
    SHADOW_IS_FILLED,
    OUTLINE_COLOR,
    OUTLINE_WIDTH,
    OUTLINE_STYLE,
    OUTLINE_RADIUS,
    USE_SHADOW_BATCHING,
    GREY_COEF,
    LIGHT_INTENSITY,
    LIGHT_COLOR,
    LIGHT_POSITION,
    ILLUMINATED_BORDER_WIDTH,
    ILLUMINATED_TYPE,
    BLOOM,
    PARTICLE_EMITTER_UPDATER,
    FOREGROUND_EFFECT_RADIUS,
    MOTION_BLUR_PARA,
    DYNAMIC_DIM_DEGREE,
    BACKGROUND_BLUR_RADIUS,
    BACKGROUND_BLUR_SATURATION,
    BACKGROUND_BLUR_BRIGHTNESS,
    BACKGROUND_BLUR_MASK_COLOR,
    BACKGROUND_BLUR_COLOR_MODE,
    BACKGROUND_BLUR_RADIUS_X,
    BACKGROUND_BLUR_RADIUS_Y,
    FOREGROUND_BLUR_RADIUS,
    FOREGROUND_BLUR_SATURATION,
    FOREGROUND_BLUR_BRIGHTNESS,
    FOREGROUND_BLUR_MASK_COLOR,
    FOREGROUND_BLUR_COLOR_MODE,
    FOREGROUND_BLUR_RADIUS_X,
    FOREGROUND_BLUR_RADIUS_Y,
    CUSTOM,
    EXTENDED,
    TRANSITION,
    BACKGROUND_STYLE,
    CONTENT_STYLE,
    FOREGROUND_STYLE,
    OVERLAY_STYLE,
    NODE_MODIFIER,
    ENV_FOREGROUND_COLOR,
    ENV_FOREGROUND_COLOR_STRATEGY,
    GEOMETRYTRANS,
    CHILDREN, // PLACEHOLDER, no such modifier, but we need a dirty flag
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
