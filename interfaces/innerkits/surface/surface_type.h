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

#include <stdint.h>
#include <string>

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

using BufferRequestConfig = struct BufferRequestConfig {
    int32_t width;
    int32_t height;
    int32_t strideAlignment;
    int32_t format; // PixelFormat
    int32_t usage;
    int32_t timeout;
    bool operator == (const struct BufferRequestConfig& config) const
    {
        return width == config.width &&
               height == config.height &&
               strideAlignment == config.strideAlignment &&
               format == config.format &&
               usage == config.usage;
    }
    bool operator != (const struct BufferRequestConfig& config) const
    {
        return !(*this == config);
    }
};

using BufferFlushConfig = struct BufferFlushConfig {
    Rect damage;
    int64_t timestamp;
};

using SurfaceError = enum SurfaceError {
    SURFACE_ERROR_OK = 0,
    SURFACE_ERROR_ERROR,
    SURFACE_ERROR_BINDER_ERROR,
    SURFACE_ERROR_NULLPTR,
    SURFACE_ERROR_REMOTE_NULLPTR,
    SURFACE_ERROR_NO_ENTRY,
    SURFACE_ERROR_INVALID_OPERATING,
    SURFACE_ERROR_NO_BUFFER,
    SURFACE_ERROR_INVALID_PARAM,
    SURFACE_ERROR_INIT,
    SURFACE_ERROR_NOMEM,
    SURFACE_ERROR_API_FAILED,
    SURFACE_ERROR_NOT_SUPPORT,
    SURFACE_ERROR_OUT_OF_RANGE,
    SURFACE_ERROR_TYPE_ERROR,
    SURFACE_ERROR_NO_CONSUMER,
    _SURFACE_ERROR_MAX,
};

static const std::string SurfaceErrorStrs[] = {
    [SURFACE_ERROR_OK]                = "<no error>",
    [SURFACE_ERROR_ERROR]             = "<occur error>",
    [SURFACE_ERROR_BINDER_ERROR]      = "<binder occur error>",
    [SURFACE_ERROR_NULLPTR]           = "<param exists nullptr>",
    [SURFACE_ERROR_REMOTE_NULLPTR]    = "<remote give nullptr>",
    [SURFACE_ERROR_NO_ENTRY]          = "<no entry>",
    [SURFACE_ERROR_INVALID_OPERATING] = "<operating is invalid>",
    [SURFACE_ERROR_NO_BUFFER]         = "<no buffer>",
    [SURFACE_ERROR_INVALID_PARAM]     = "<param is invalid>",
    [SURFACE_ERROR_INIT]              = "<init occur error>",
    [SURFACE_ERROR_NOMEM]             = "<no mem>",
    [SURFACE_ERROR_API_FAILED]        = "<api failed>",
    [SURFACE_ERROR_NOT_SUPPORT]       = "<not support>",
    [SURFACE_ERROR_OUT_OF_RANGE]      = "<index out of range>",
    [SURFACE_ERROR_TYPE_ERROR]        = "<invalid type>",
    [SURFACE_ERROR_NO_CONSUMER]       = "<no consumer listener>",
};

static inline std::string SurfaceErrorStr(SurfaceError err)
{
    if (err >= _SURFACE_ERROR_MAX) {
        return "<SurfaceErrorStr error index out of range>";
    }

    return SurfaceErrorStrs[err];
}
} // namespace OHOS

#endif // INTERFACES_INNERKITS_SURFACE_SURFACE_TYPE_H
