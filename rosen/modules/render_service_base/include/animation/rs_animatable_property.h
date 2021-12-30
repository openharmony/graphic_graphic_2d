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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATABLE_PROPERTY_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATABLE_PROPERTY_H

#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {

enum class RSAnimatableProperty {
    // Invalid
    INVALID = -1,

    // Opacity
    ALPHA = 0,

    // Geometry
    BOUNDS_WIDTH,
    BOUNDS_HEIGHT,
    BOUNDS_POSITION,
    BOUNDS_POSITION_X,
    BOUNDS_POSITION_Y,
    FRAME_WIDTH,
    FRAME_HEIGHT,
    FRAME_POSITION,
    FRAME_POSITION_X,
    FRAME_POSITION_Y,
    PIVOT_X,
    PIVOT_Y,
    CORNER_RADIUS,
    POSITION_Z,

    // Transform
    TRANSLATE,
    TRANSLATE_X,
    TRANSLATE_Y,
    TRANSLATE_Z,
    SCALE_X,
    SCALE_Y,
    ROTATION,
    ROTATION_X,
    ROTATION_Y,
    ROTATION_3D,

    // Background
    BACKGROUND_COLOR,
    BGIMAGE_WIDTH,
    BGIMAGE_HEIGHT,
    BGIMAGE_POSITION_X,
    BGIMAGE_POSITION_Y,

    // Foreground
    FOREGROUND_COLOR,

    // Shadow
    SHADOW_OFFSET_X,
    SHADOW_OFFSET_Y,
    SHADOW_ALPHA,
    SHADOW_ELEVATION,
    SHADOW_RADIUS,
    SHADOW_COLOR,

    // Border
    BORDER_COLOR,
    BORDER_WIDTH,

    // Filter
    FILTER,
    BACKGROUND_FILTER,

    // SubNodes
    SUB_LAYER_TRANSFORM,
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATABLE_PROPERTY_H
