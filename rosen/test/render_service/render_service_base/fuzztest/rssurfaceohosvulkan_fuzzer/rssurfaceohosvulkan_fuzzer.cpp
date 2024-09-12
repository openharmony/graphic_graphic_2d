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

#include "generated/vk_loader_extensions.h"

#include "platform/ohos/backend/rs_surface_ohos_vulkan.h"
#include "platform/ohos/rs_render_service_connect_hub.h"

namespace OHOS {
namespace Rosen {
using namespace Drawing;

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
bool DoIsValid(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    sptr<Surface> surface = renderService->CreateNodeAndSurface(config);
    auto rsSurfaceOhosVulkan = std::make_shared<RSSurfaceOhosVulkan>(surface);

    rsSurfaceOhosVulkan->IsValid();
    rsSurfaceOhosVulkan->ClearBuffer();
    rsSurfaceOhosVulkan->ResetBufferAge();
    return true;
}
bool DoDestroySemaphore(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    sptr<Surface> surface = renderService->CreateNodeAndSurface(config);
    auto rsSurfaceOhosVulkan = std::make_shared<RSSurfaceOhosVulkan>(surface);

    void* context = nullptr;
    RSSurfaceOhosVulkan::DestroySemaphore(context);
    return true;
}
bool DoRequestFrame(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    sptr<Surface> surface = renderService->CreateNodeAndSurface(config);
    auto rsSurfaceOhosVulkan = std::make_shared<RSSurfaceOhosVulkan>(surface);

    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    uint64_t uiTimestamp = GetData<uint64_t>();
    rsSurfaceOhosVulkan->RequestFrame();
    return true;
}
bool DoFlushFrame(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    sptr<Surface> surface = renderService->CreateNodeAndSurface(config);
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
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    sptr<Surface> surface = renderService->CreateNodeAndSurface(config);
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
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    sptr<Surface> surface = renderService->CreateNodeAndSurface(config);
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
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    sptr<Surface> surface = renderService->CreateNodeAndSurface(config);
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
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    sptr<Surface> surface = renderService->CreateNodeAndSurface(config);
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
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    sptr<Surface> surface = renderService->CreateNodeAndSurface(config);
    auto rsSurfaceOhosVulkan = std::make_shared<RSSurfaceOhosVulkan>(surface);

    std::shared_ptr<GPUContext> skContext = nullptr;
    rsSurfaceOhosVulkan->SetSkContext(skContext);
    return true;
}
bool DoSetNativeWindowInfo(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    sptr<Surface> surface = renderService->CreateNodeAndSurface(config);
    auto rsSurfaceOhosVulkan = std::make_shared<RSSurfaceOhosVulkan>(surface);

    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    bool useAFBC = GetData<bool>();
    rsSurfaceOhosVulkan->SetSkContext(width, height, useAFBC);
    return true;
}
bool DoRequestNativeWindowBuffer(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    sptr<Surface> surface = renderService->CreateNodeAndSurface(config);
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
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    sptr<Surface> surface = renderService->CreateNodeAndSurface(config);
    auto rsSurfaceOhosVulkan = std::make_shared<RSSurfaceOhosVulkan>(surface);

    NativeBufferUtils::NativeSurfaceInfo nativeSurface;
    const auto& vkContext = RsVulkanContext::GetSingleton();
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
    OHOS::Rosen::DoDestroySemaphore(data, size);          // DestroySemaphore
    OHOS::Rosen::DoRequestFrame(data, size);              // RequestFrame
    OHOS::Rosen::DoFlushFrame(data, size);                // FlushFrame
    OHOS::Rosen::DoSetColorSpace(data, size);             // SetColorSpace
    OHOS::Rosen::DoSetSurfaceBufferUsage(data, size);     // SetSurfaceBufferUsage
    OHOS::Rosen::DoSetSurfacePixelFormat(data, size);     // SetSurfacePixelFormat
    OHOS::Rosen::DoSetUiTimeStamp(data, size);            // SetUiTimeStamp
    OHOS::Rosen::DoSetSkContext(data, size);              // SetSkContext
    OHOS::Rosen::DoSetNativeWindowInfo(data, size);       // SetNativeWindowInfo
    OHOS::Rosen::DoRequestNativeWindowBuffer(data, size); // RequestNativeWindowBuffer
    OHOS::Rosen::DoCreateVkSemaphore(data, size);         // CreateVkSemaphore
    return 0;
}
