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

enum class RSAnimatableProperty : unsigned long long {
    // Invalid
    INVALID             = 0,
    // Opacity
    ALPHA               = 1ull << 1,
    // Bounds
    BOUNDS_WIDTH        = 1ull << 2,
    BOUNDS_HEIGHT       = 1ull << 3,
    BOUNDS_SIZE         = BOUNDS_WIDTH | BOUNDS_HEIGHT,
    BOUNDS_POSITION_X   = 1ull << 4,
    BOUNDS_POSITION_Y   = 1ull << 5,
    BOUNDS_POSITION     = BOUNDS_POSITION_X | BOUNDS_POSITION_Y,
    BOUNDS              = BOUNDS_POSITION | BOUNDS_SIZE,
    // Frame
    FRAME_WIDTH         = 1ull << 6,
    FRAME_HEIGHT        = 1ull << 7,
    FRAME_SIZE          = FRAME_WIDTH | FRAME_HEIGHT,
    FRAME_POSITION_X    = 1ull << 8,
    FRAME_POSITION_Y    = 1ull << 9,
    FRAME_POSITION      = FRAME_POSITION_X | FRAME_POSITION_Y,
    FRAME               = FRAME_POSITION | FRAME_SIZE,
    // Pivot
    PIVOT_X             = 1ull << 10,
    PIVOT_Y             = 1ull << 11,
    PIVOT               = PIVOT_X | PIVOT_Y,
    // others
    CORNER_RADIUS       = 1ull << 12,
    POSITION_Z          = 1ull << 13,
    // Transform
    TRANSLATE_X         = 1ull << 14,
    TRANSLATE_Y         = 1ull << 15,
    TRANSLATE_Z         = 1ull << 16,
    TRANSLATE           = TRANSLATE_X | TRANSLATE_Y | TRANSLATE_Z,
    SCALE_X             = 1ull << 17,
    SCALE_Y             = 1ull << 18,
    SCALE               = SCALE_X | SCALE_Y,
    ROTATION            = 1ull << 19,
    ROTATION_X          = 1ull << 20,
    ROTATION_Y          = 1ull << 21,
    ROTATION_3D         = ROTATION_X | ROTATION_Y | ROTATION,
    // Background
    BACKGROUND_COLOR    = 1ull << 22,
    BGIMAGE_WIDTH       = 1ull << 23,
    BGIMAGE_HEIGHT      = 1ull << 24,
    BGIMAGE_POSITION_X  = 1ull << 25,
    BGIMAGE_POSITION_Y  = 1ull << 26,
    // Foreground
    FOREGROUND_COLOR    = 1ull << 27,
    // Shadow
    SHADOW_OFFSET_X     = 1ull << 28,
    SHADOW_OFFSET_Y     = 1ull << 29,
    SHADOW_ALPHA        = 1ull << 30,
    SHADOW_ELEVATION    = 1ull << 31,
    SHADOW_RADIUS       = 1ull << 32,
    SHADOW_COLOR        = 1ull << 33,
    // Border
    BORDER_COLOR        = 1ull << 34,
    BORDER_WIDTH        = 1ull << 35,
    // Filter
    FILTER              = 1ull << 36,
    BACKGROUND_FILTER   = 1ull << 37,
    // SubLayer
    SUB_LAYER_TRANSFORM = 1ull << 38,
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATABLE_PROPERTY_H
