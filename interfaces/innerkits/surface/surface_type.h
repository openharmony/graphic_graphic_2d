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

#ifndef INTERFACES_INNERKITS_SURFACE_SURFACE_TYPE_H
#define INTERFACES_INNERKITS_SURFACE_SURFACE_TYPE_H

#include <cstdint>
#include <string>

#include <graphic_common.h>

namespace OHOS {
#define SURFACE_MAX_USER_DATA_COUNT 1000
#define SURFACE_MAX_WIDTH 7680
#define SURFACE_MAX_HEIGHT 7680
#define SURFACE_MAX_QUEUE_SIZE 10
#define SURFACE_DEFAULT_QUEUE_SIZE 3
#define SURFACE_MAX_STRIDE_ALIGNMENT 32
#define SURFACE_MIN_STRIDE_ALIGNMENT 4
#define SURFACE_DEFAULT_STRIDE_ALIGNMENT 4
#define SURFACE_MAX_SIZE 58982400 // 8K * 8K

using Rect = struct Rect {
    int32_t x;
    int32_t y;
    int32_t w;
    int32_t h;
};

typedef enum {
    COLOR_GAMUT_INVALID = -1,
    COLOR_GAMUT_NATIVE = 0,
    COLOR_GAMUT_SATNDARD_BT601 = 1,
    COLOR_GAMUT_STANDARD_BT709 = 2,
    COLOR_GAMUT_DCI_P3 = 3,
    COLOR_GAMUT_SRGB = 4,
    COLOR_GAMUT_ADOBE_RGB = 5,
    COLOR_GAMUT_DISPLAY_P3 = 6,
    COLOR_GAMUT_BT2020 = 7,
    COLOR_GAMUT_BT2100_PQ = 8,
    COLOR_GAMUT_BT2100_HLG = 9,
    COLOR_GAMUT_DISPLAY_BT2020 = 10,
} SurfaceColorGamut;

using BufferRequestConfig = struct BufferRequestConfig {
    int32_t width;
    int32_t height;
    int32_t strideAlignment;
    int32_t format; // PixelFormat
    int32_t usage;
    int32_t timeout;
    SurfaceColorGamut colorGamut = SurfaceColorGamut::COLOR_GAMUT_SRGB;
    bool operator ==(const struct BufferRequestConfig &config) const
    {
        return width == config.width &&
               height == config.height &&
               strideAlignment == config.strideAlignment &&
               format == config.format &&
               usage == config.usage &&
               colorGamut == config.colorGamut;
    }
    bool operator != (const struct BufferRequestConfig &config) const
    {
        return !(*this == config);
    }
};

using BufferFlushConfig = struct BufferFlushConfig {
    Rect damage;
    int64_t timestamp;
};
} // namespace OHOS

#endif // INTERFACES_INNERKITS_SURFACE_SURFACE_TYPE_H
