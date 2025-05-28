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
#include "rsvkimagemanager_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>

#include "securec.h"

#include "feature/gpuComposition/rs_vk_image_manager.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos = 0;
} // namespace

/*
 * describe: get data from outside untrusted data(g_data) which size is according to sizeof(T)
 * tips: only support basic type
 */
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

void LocalInit()
{
    RsVulkanContext::SetRecyclable(false);
}

sptr<SurfaceBuffer> CreateBuffer()
{
    sptr<SurfaceBuffer> buffer = SurfaceBuffer::Create();
    if (!buffer) {
        return nullptr;
    }
    BufferRequestConfig requestConfig = {
        .width = GetData<uint32_t>() % 100,
        .height = GetData<uint32_t>() % 100,
        .strideAlignment = 0x8, // set 0x8 as default value to alloc SurfaceBufferImpl
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888, // PixelFormat
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_HW_RENDER | BUFFER_USAGE_MEM_MMZ_CACHE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
        .colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB,
        .transform = GraphicTransformType::GRAPHIC_ROTATE_NONE,
    };
    GSError ret = buffer->Alloc(requestConfig);
    if (ret != GSERROR_OK) {
        return nullptr;
    }
    return buffer;
}

void RSVKImageManagerFuzztest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto vkImageManager = std::make_shared<RSVkImageManager>();
    auto fakeSeqNum = GetData<uint32_t>();
    auto fakeTid = GetData<uint32_t>();
    (void)vkImageManager->MapVkImageFromSurfaceBuffer(nullptr, nullptr, fakeTid);
    vkImageManager->UnMapVkImageFromSurfaceBuffer(fakeSeqNum);

    // valid data
    auto buffer1 = CreateBuffer();
    if (!buffer1) {
        return;
    }
    sptr<SyncFence> bufferFence1 = SyncFence::INVALID_FENCE;
    auto tid = gettid();
    (void)vkImageManager->MapVkImageFromSurfaceBuffer(buffer1, bufferFence1, tid); // map buffer
    (void)vkImageManager->MapVkImageFromSurfaceBuffer(buffer1, bufferFence1, tid); // map same buffer again
    (void)vkImageManager->MapVkImageFromSurfaceBuffer(buffer1, nullptr, tid); // map buffer without fence
    vkImageManager->UnMapVkImageFromSurfaceBuffer(buffer1->GetSeqNum());

    auto buffer2 = CreateBuffer();
    if (!buffer2) {
        return;
    }
    sptr<SyncFence> bufferFence2 = SyncFence::INVALID_FENCE;
    (void)vkImageManager->CreateImageCacheFromBuffer(buffer2, bufferFence2);

    std::string dumpString = "";
    vkImageManager->DumpVkImageInfo(dumpString);
}

void RSVKImageManagerFuzztestVKSemaphore(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto vkImageManager = std::make_shared<RSVkImageManager>();
    auto buffer1 = CreateBuffer();
    auto drawingSurface = std::make_unique<Drawing::Surface>();
    sptr<SyncFence> bufferFence1 = SyncFence::INVALID_FENCE;
    auto fakeTid = GetData<uint32_t>();
    if (vkImageManager && buffer1 && drawingSurface) {
        (void)vkImageManager->MapVkImageFromSurfaceBuffer(buffer1, bufferFence1, fakeTid, drawingSurface.get());
        (void)vkImageManager->UnMapVkImageFromSurfaceBuffer(buffer1->GetSeqNum());
        (void)vkImageManager->MapVkImageFromSurfaceBuffer(buffer1, nullptr, fakeTid, nullptr);
        (void)vkImageManager->UnMapVkImageFromSurfaceBuffer(buffer1->GetSeqNum());

        VkDevice device = RsVulkanContext::GetSingleton().GetRsVulkanInterface().device_;
        RsVulkanContext::GetSingleton().GetRsVulkanInterface().device_ = VK_NULL_HANDLE;
        (void)vkImageManager->MapVkImageFromSurfaceBuffer(buffer1, bufferFence1, fakeTid, drawingSurface.get());
        RsVulkanContext::GetSingleton().GetRsVulkanInterface().device_ = device;
        (void)vkImageManager->UnMapVkImageFromSurfaceBuffer(buffer1->GetSeqNum());
    }
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    OHOS::Rosen::LocalInit();
    OHOS::Rosen::RSVKImageManagerFuzztest(data, size);
    OHOS::Rosen::RSVKImageManagerFuzztestVKSemaphore(data, size);
    return 0;
}