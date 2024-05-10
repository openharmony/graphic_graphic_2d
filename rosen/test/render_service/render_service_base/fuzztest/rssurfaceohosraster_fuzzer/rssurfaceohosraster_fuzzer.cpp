/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "rssurfaceohosraster_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <memory>
#include <securec.h>
#include <unistd.h>

#include "platform/ohos/backend/rs_surface_frame_ohos_raster.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;
} // namespace

template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (g_data == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, g_data + g_pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_pos += objectSize;
    return object;
}
bool DoGetCanvas(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    auto rsSurfaceFrameOhosRaster = std::make_shared<RSSurfaceFrameOhosRaster>(width, height);
    rsSurfaceFrameOhosRaster->GetCanvas();
    return true;
}
bool DoGetSurface(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    auto rsSurfaceFrameOhosRaster = std::make_shared<RSSurfaceFrameOhosRaster>(width, height);
    rsSurfaceFrameOhosRaster->GetSurface();
    return true;
}
bool DoGetBuffer(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    auto rsSurfaceFrameOhosRaster = std::make_shared<RSSurfaceFrameOhosRaster>(width, height);
    rsSurfaceFrameOhosRaster->GetBuffer();
    return true;
}

bool DoSetDamageRegion(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    auto rsSurfaceFrameOhosRaster = std::make_shared<RSSurfaceFrameOhosRaster>(width, height);

    int32_t left = GetData<int32_t>();
    int32_t top = GetData<int32_t>();
    int32_t width1 = GetData<int32_t>();
    int32_t height1 = GetData<int32_t>();

    rsSurfaceFrameOhosRaster->SetDamageRegion(left, top, width1, height1);

    std::vector<RectI> rects;
    rsSurfaceFrameOhosRaster->SetDamageRegion(rects);
    return true;
}
bool DoGetBufferAge(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    auto rsSurfaceFrameOhosRaster = std::make_shared<RSSurfaceFrameOhosRaster>(width, height);
    rsSurfaceFrameOhosRaster->GetBufferAge();
    return true;
}
bool DoGetReleaseFence(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    auto rsSurfaceFrameOhosRaster = std::make_shared<RSSurfaceFrameOhosRaster>(width, height);
    rsSurfaceFrameOhosRaster->GetReleaseFence();
    return true;
}
bool DoSetReleaseFence(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    auto rsSurfaceFrameOhosRaster = std::make_shared<RSSurfaceFrameOhosRaster>(width, height);
    int32_t fence = GetData<int32_t>();
    rsSurfaceFrameOhosRaster->SetReleaseFence(fence);
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoGetCanvas(data, size);
    OHOS::Rosen::DoGetSurface(data, size);      // GetSurface
    OHOS::Rosen::DoGetBuffer(data, size);       // GetBuffer
    OHOS::Rosen::DoSetDamageRegion(data, size); // SetDamageRegion
    OHOS::Rosen::DoGetBufferAge(data, size);    // GetBufferAge
    OHOS::Rosen::DoGetReleaseFence(data, size); // GetReleaseFence
    OHOS::Rosen::DoSetReleaseFence(data, size); // SetReleaseFence
    return 0;
}
