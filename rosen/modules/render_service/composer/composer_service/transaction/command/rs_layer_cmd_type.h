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

#ifndef RENDER_SERVICE_COMPOSER_SERVICE_TRANSACTION_COMMAND_RS_LAYER_CMD_TYPE_H
#define RENDER_SERVICE_COMPOSER_SERVICE_TRANSACTION_COMMAND_RS_LAYER_CMD_TYPE_H

#include <bitset>
#include <cstdint>

namespace OHOS {
namespace Rosen {
enum class RSLayerCmdType : uint16_t {
    INVALID = 0,
    BOUNDS,
    FRAME,
    PIVOT,
    PIVOT_Z,
    QUATERNION,
    ROTATION,
    ROTATION_X,
    ROTATION_Y,
    CAMERA_DISTANCE,
    SCALE,
    SCALE_Z,
    SKEW,
    PERSP,
    TRANSLATE,
    TRANSLATE_Z,
    SUBLAYER_TRANSFORM,
    CORNER_RADIUS_INFO_FOR_DRM,
    ALPHA,
    ALPHA_OFFSCREEN,
    ZORDER,
    TYPE,
    TRANSFORM,
    COMPOSITION_TYPE,
    VISIBLE_REGIONS,
    DIRTY_REGIONS,
    BLEND_TYPE,
    CROP_RECT,
    PRE_MULTI,
    LAYER_SIZE,
    BOUND_SIZE,
    LAYER_COLOR,
    BACKGROUND_COLOR,
    COLOR_TRANSFORM,
    COLOR_DATA_SPACE,
    META_DATA,
    META_DATA_SET,
    MATRIX,
    GRAVITY,
    UNI_RENDER_FLAG,
    TUNNEL_HANDLE_CHANGE,
    TUNNEL_HANDLE,
    TUNNEL_LAYER_ID,
    TUNNEL_LAYER_PROPERTY,
    IS_SUPPORTED_PRESENT_TIMESTAMP,
    PRESENT_TIMESTAMP,
    SDR_NIT,
    DISPLAY_NIT,
    BRIGHTNESS_RATIO,
    LAYER_LINEAR_MATRIX,
    LAYER_SOURCE_TUNING,
    ROTATION_FIXED,
    LAYER_ARSR,
    LAYER_COPYBIT,
    NEED_BILINEAR_INTERPOLATION,
    IS_MASK_LAYER,
    NODE_ID,
    ANCO_FLAGS,
    BUFFER,
    PRE_BUFFER,
    ACQUIRE_FENCE,
    WINDOWS_NAME,
    LAYER_MASK_INFO,
    ID,
    CYCLE_BUFFERS_NUM,
    SURFACE_NAME,
    SURFACE_UNIQUE_ID,
    PIXEL_MAP,
    SOLID_COLOR_LAYER_PROPERTY,
    IS_NEED_COMPOSITION,
    USE_DEVICE_OFFLINE,
    IGNORE_ALPHA,
    ANCO_SRC_RECT,

    LAYER_CMD = 254,
    MAX_RS_LAYER_CMD_TYPE = 255,
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_COMPOSER_SERVICE_TRANSACTION_COMMAND_RS_LAYER_CMD_TYPE_H