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
    INVALID = 0,

    BOUNDS,
    FRAME,
    POSITION_Z,

    PIVOT,
    QUATERNION,
    ROTATION,
    ROTATION_X,
    ROTATION_Y,
    SCALE,
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

    FRAME_GRAVITY,

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
    MASK,

    CUSTOM,
    EXTENDED,
    BACKGROUND_STYLE,
    CONTENT_STYLE,
    FOREGROUND_STYLE,
    OVERLAY_STYLE,
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
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_MODIFIER_TYPE_H
