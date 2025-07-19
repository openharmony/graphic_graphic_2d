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

#ifndef COMPOSER_CLIENT_RS_LAYER_CMD_TYPE_H
#define COMPOSER_CLIENT_RS_LAYER_CMD_TYPE_H

#include <bitset>
#include <cstdint>

namespace OHOS {
namespace Rosen {
enum class RSLayerCmdType : uint16_t {
    INVALID = 0,
    BOUNDS = 1,
    FRAME = 2,
    POSITION_Z = 3,
    POSITION_Z_APPLICABLE_CAMERA3D = 4,
    PIVOT = 5,
    PIVOT_Z = 6,
    QUATERNION = 7,
    ROTATION = 8,
    ROTATION_X = 9,
    ROTATION_Y = 10,
    CAMERA_DISTANCE = 11,
    SCALE = 12,
    SCALE_Z = 13,
    SKEW = 14,
    PERSP = 15,
    TRANSLATE = 16,
    TRANSLATE_Z = 17,
    SUBLAYER_TRANSFORM = 18,
    CORNER_RADIUS = 19,
    ALPHA = 20,
    ALPHA_OFFSCREEN = 21,

    MAX_RS_LAYER_CMD_TYPE = 255,
};

} // namespace Rosen
} // namespace OHOS

#endif // COMPOSER_CLIENT_RS_LAYER_CMD_TYPE_H