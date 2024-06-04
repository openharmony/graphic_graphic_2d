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
#include "rssurfaceframeohosvulkan_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <hilog/log.h>
#include <securec.h>
#include <unistd.h>

#include "draw/surface.h"
#include "platform/ohos/backend/rs_surface_frame_ohos_vulkan.cpp"
#include "platform/ohos/backend/rs_surface_frame_ohos_vulkan.h"
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
    int32_t bufferAge = GetData<int32_t>();
    auto value = std::make_shared<Drawing::Surface>();
    auto rsSurfaceFrameOhosVulkan = std::make_shared<RSSurfaceFrameOhosVulkan>(value, width, height, bufferAge);
    rsSurfaceFrameOhosVulkan->GetCanvas();
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
    int32_t bufferAge = GetData<int32_t>();
    auto value = std::make_shared<Drawing::Surface>();
    auto rsSurfaceFrameOhosVulkan = std::make_shared<RSSurfaceFrameOhosVulkan>(value, width, height, bufferAge);
    rsSurfaceFrameOhosVulkan->GetSurface();
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
    int32_t bufferAge = GetData<int32_t>();
    auto value = std::make_shared<Drawing::Surface>();
    auto rsSurfaceFrameOhosVulkan = std::make_shared<RSSurfaceFrameOhosVulkan>(value, width, height, bufferAge);

    int32_t left = GetData<int32_t>();
    int32_t top = GetData<int32_t>();
    int32_t width1 = GetData<int32_t>();
    int32_t height1 = GetData<int32_t>();
    rsSurfaceFrameOhosVulkan->SetDamageRegion(left, top, width1, height1);
    std::vector<OHOS::Rosen::RectI> rects;
    rsSurfaceFrameOhosVulkan->SetDamageRegion(rects);
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
    int32_t bufferAge = GetData<int32_t>();
    auto value = std::make_shared<Drawing::Surface>();
    auto rsSurfaceFrameOhosVulkan = std::make_shared<RSSurfaceFrameOhosVulkan>(value, width, height, bufferAge);
    rsSurfaceFrameOhosVulkan->GetReleaseFence();
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
    int32_t bufferAge = GetData<int32_t>();
    auto value = std::make_shared<Drawing::Surface>();
    auto rsSurfaceFrameOhosVulkan = std::make_shared<RSSurfaceFrameOhosVulkan>(value, width, height, bufferAge);
    int32_t fence = GetData<int32_t>();
    rsSurfaceFrameOhosVulkan->SetReleaseFence(fence);
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
    int32_t bufferAge = GetData<int32_t>();
    auto value = std::make_shared<Drawing::Surface>();
    auto rsSurfaceFrameOhosVulkan = std::make_shared<RSSurfaceFrameOhosVulkan>(value, width, height, bufferAge);
    rsSurfaceFrameOhosVulkan->GetBufferAge();
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoGetCanvas(data, size);
    OHOS::Rosen::DoGetSurface(data, size);
    OHOS::Rosen::DoSetDamageRegion(data, size);
    OHOS::Rosen::DoGetReleaseFence(data, size);
    OHOS::Rosen::DoSetReleaseFence(data, size);
    OHOS::Rosen::DoGetBufferAge(data, size);
    return 0;
}
