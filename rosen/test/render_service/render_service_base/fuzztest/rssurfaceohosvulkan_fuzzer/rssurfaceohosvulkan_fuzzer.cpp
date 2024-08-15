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

#include "rssurfaceohosvulkan_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <hilog/log.h>
#include <memory>
#include <securec.h>
#include <unistd.h>

#include "image/gpu_context.h"
#include "core/ui/rs_surface_node.h"
#include "platform/ohos/backend/rs_surface_ohos_vulkan.h"
#include "platform/ohos/rs_render_service_connect_hub.h"
namespace OHOS {
namespace Rosen {

namespace {
const uint8_t* DATA = nullptr;
size_t g_size = 0;
size_t g_pos;
} // namespace

template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (DATA == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, DATA + g_pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_pos += objectSize;
    return object;
}
bool DoIsValid(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    sptr<Surface> surface;
    auto rsSurfaceOhosVulkan = std::make_shared<RSSurfaceOhosVulkan>(surface);

    rsSurfaceOhosVulkan->IsValid();
    rsSurfaceOhosVulkan->ClearBuffer();
    rsSurfaceOhosVulkan->ResetBufferAge();
    return true;
}

bool DoRequestFrame(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;
    
    sptr<Surface> surface;
    auto rsSurfaceOhosVulkan = std::make_shared<RSSurfaceOhosVulkan>(surface);

    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    uint64_t uiTimestamp = GetData<uint64_t>();
    rsSurfaceOhosVulkan->RequestFrame(width, height, uiTimestamp);
    return true;
}
bool DoFlushFrame(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;
    
    sptr<Surface> surface;
    auto rsSurfaceOhosVulkan = std::make_shared<RSSurfaceOhosVulkan>(surface);

    uint64_t uiTimestamp = GetData<uint64_t>();
    std::unique_ptr<RSSurfaceFrame> frame = nullptr;
    rsSurfaceOhosVulkan->FlushFrame(frame, uiTimestamp);
    return true;
}
bool DoSetColorSpace(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;
    
    sptr<Surface> surface;
    auto rsSurfaceOhosVulkan = std::make_shared<RSSurfaceOhosVulkan>(surface);

    GraphicColorGamut colorSpacel = GetData<GraphicColorGamut>();
    rsSurfaceOhosVulkan->SetColorSpace(colorSpacel);
    return true;
}
bool DoSetSurfaceBufferUsage(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;
    
    sptr<Surface> surface;
    auto rsSurfaceOhosVulkan = std::make_shared<RSSurfaceOhosVulkan>(surface);

    uint64_t usage = GetData<uint64_t>();
    rsSurfaceOhosVulkan->SetSurfaceBufferUsage(usage);
    return true;
}
bool DoSetSurfacePixelFormat(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;
    
    sptr<Surface> surface;
    auto rsSurfaceOhosVulkan = std::make_shared<RSSurfaceOhosVulkan>(surface);

    int32_t pixelFormat = GetData<int32_t>();
    rsSurfaceOhosVulkan->SetSurfacePixelFormat(pixelFormat);
    return true;
}
bool DoSetUiTimeStamp(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;
    
    sptr<Surface> surface;
    auto rsSurfaceOhosVulkan = std::make_shared<RSSurfaceOhosVulkan>(surface);

    uint64_t uiTimestamp = GetData<uint64_t>();
    std::unique_ptr<RSSurfaceFrame> frame = nullptr;
    rsSurfaceOhosVulkan->SetUiTimeStamp(frame, uiTimestamp);
    return true;
}
bool DoSetSkContext(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;
    
    sptr<Surface> surface;
    auto rsSurfaceOhosVulkan = std::make_shared<RSSurfaceOhosVulkan>(surface);

    std::shared_ptr<Drawing::GPUContext> skContext = nullptr;
    rsSurfaceOhosVulkan->SetSkContext(skContext);
    return true;
}
bool DoSetNativeWindowInfo(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;
    
    sptr<Surface> surface;
    auto rsSurfaceOhosVulkan = std::make_shared<RSSurfaceOhosVulkan>(surface);

    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    bool useAFBC = GetData<bool>();
    rsSurfaceOhosVulkan->SetNativeWindowInfo(width, height, useAFBC);
    return true;
}
bool DoRequestNativeWindowBuffer(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;
    
    sptr<Surface> surface;
    auto rsSurfaceOhosVulkan = std::make_shared<RSSurfaceOhosVulkan>(surface);

    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    int fenceFd = GetData<int>();
    bool useAFBC = GetData<bool>();

    NativeWindowBuffer* nativeWindowBuffer;

    rsSurfaceOhosVulkan->RequestNativeWindowBuffer(&nativeWindowBuffer, width, height, fenceFd, useAFBC);
    return true;
}
bool DoCreateVkSemaphore(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;
    
    sptr<Surface> surface;
    auto rsSurfaceOhosVulkan = std::make_shared<RSSurfaceOhosVulkan>(surface);

    NativeBufferUtils::NativeSurfaceInfo nativeSurface;
    auto& vkContext = RsVulkanContext::GetSingleton();
    VkSemaphore semaphore;
    rsSurfaceOhosVulkan->CreateVkSemaphore(semaphore, vkContext, nativeSurface);
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoIsValid(data, size);                   // IsValid
    return 0;
}
