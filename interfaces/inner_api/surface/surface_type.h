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
#include <display_type.h>

namespace OHOS {
#define SURFACE_MAX_USER_DATA_COUNT 1000
#define SURFACE_MAX_QUEUE_SIZE 32
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

using BufferRequestConfig = struct BufferRequestConfig {
    int32_t width;
    int32_t height;
    int32_t strideAlignment;
    int32_t format; // PixelFormat
    int32_t usage;
    int32_t timeout;
    int32_t pid;
    ColorGamut colorGamut = ColorGamut::COLOR_GAMUT_SRGB;
    TransformType transform = TransformType::ROTATE_NONE;
    bool operator ==(const struct BufferRequestConfig &config) const
    {
        return width == config.width &&
               height == config.height &&
               strideAlignment == config.strideAlignment &&
               format == config.format &&
               usage == config.usage &&
               colorGamut == config.colorGamut &&
               transform == config.transform;
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
