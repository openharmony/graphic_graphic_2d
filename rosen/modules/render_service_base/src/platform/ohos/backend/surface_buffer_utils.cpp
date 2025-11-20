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

#include "platform/ohos/backend/surface_buffer_utils.h"

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
#include <sys/ioctl.h>
#include <unistd.h>

#include "native_buffer_utils.h"
#include "native_window.h"

#include "platform/common/rs_log.h"

namespace OHOS::Rosen {
namespace {
// Canvas DMA buffer size limit (consistent with Vulkan image size limit)
// Maximum total pixel count for Canvas: 10000 x 10000 = 100,000,000 pixels
constexpr uint32_t CANVAS_BUFFER_SIZE_LIMIT = 10000 * 10000;

// DMA buffer ioctl command base
constexpr int DMA_BUF_BASE = 'b';

const char* DMA_BUF_NAME = "canvas";

// DMA buffer ioctl commands
#define DMA_BUF_SET_LEAK_TYPE _IOW(DMA_BUF_BASE, 5, const char*)
} // namespace

sptr<SurfaceBuffer> SurfaceBufferUtils::CreateCanvasSurfaceBuffer(pid_t pid, int width, int height)
{
    // Validate Canvas size against Vulkan image size limit
    if (width <= 0 || height <= 0 ||
        static_cast<uint64_t>(width) * static_cast<uint64_t>(height) > CANVAS_BUFFER_SIZE_LIMIT) {
        RS_LOGE("CreateCanvasSurfaceBuffer: Canvas size invalid or exceeds limit, width=%{public}d, height=%{public}d, "
            "limit=%{public}" PRIu32, width, height, CANVAS_BUFFER_SIZE_LIMIT);
        return nullptr;
    }

    sptr<SurfaceBuffer> buffer = SurfaceBuffer::Create();
    if (buffer == nullptr) {
        RS_LOGE("CreateCanvasSurfaceBuffer: SurfaceBuffer::Create failed");
        return nullptr;
    }

    // Configure DMA buffer request parameters
    BufferRequestConfig config = {
        .width = width,
        .height = height,
        .strideAlignment = 0x8, // 8-byte alignment
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE |
                 BUFFER_USAGE_MEM_DMA |         // DMA access
                 BUFFER_USAGE_MEM_MMZ_CACHE,    // MMZ cache
        .timeout = 0,
    };
    GSError ret = buffer->Alloc(config);
    if (ret != GSERROR_OK) {
        RS_LOGE("CreateCanvasSurfaceBuffer: Alloc failed, ret=%{public}d", ret);
        return nullptr;
    }

    // Verify virtual address mapping
    void* virAddr = buffer->GetVirAddr();
    if (virAddr == nullptr) {
        RS_LOGE("CreateCanvasSurfaceBuffer: GetVirAddr failed");
        return nullptr;
    }

    // Set DMA buffer name for debugging
    SetCanvasSurfaceBufferName(buffer, pid, width, height);
    return buffer;
}

void SurfaceBufferUtils::SetCanvasSurfaceBufferName(sptr<SurfaceBuffer> buffer, pid_t pid, int width, int height)
{
    if (buffer == nullptr) {
        RS_LOGE("SetCanvasSurfaceBufferName: buffer is nullptr");
        return;
    }

    // Get file descriptor for ioctl operations
    int fd = buffer->GetFileDescriptor();
    if (fd < 0) {
        RS_LOGE("SetCanvasSurfaceBufferName: Invalid file descriptor");
        return;
    }

    // Format: WIDTHxHEIGHT_pid (e.g., 1920x1080_1234)
    std::string bufferName = std::to_string(width) + "x" + std::to_string(height) + "_" + std::to_string(pid);
    int ret = TEMP_FAILURE_RETRY(ioctl(fd, DMA_BUF_SET_LEAK_TYPE, bufferName.c_str()));
    if (ret != 0) {
        RS_LOGE("SetCanvasSurfaceBufferName: Failed to set DMA buffer name, fd=%{public}d, width=%{public}d, "
            "height=%{public}d, pid=%{public}d", fd, width, height, pid);
        return;
    }

    // Set leak type (optional, for memory leak debugging)
    ret = TEMP_FAILURE_RETRY(ioctl(fd, DMA_BUF_SET_LEAK_TYPE, DMA_BUF_NAME));
    if (ret != 0) {
        RS_LOGE("SetCanvasSurfaceBufferName: Failed to set DMA buffer leak type, fd=%{public}d, width=%{public}d, "
            "height=%{public}d, pid=%{public}d", fd, width, height, pid);
    }
}

Drawing::BackendTexture SurfaceBufferUtils::ConvertSurfaceBufferToBackendTexture(sptr<SurfaceBuffer> buffer)
{
    if (buffer == nullptr) {
        RS_LOGE("ConvertSurfaceBufferToBackendTexture: buffer is nullptr");
        return {};
    }

    // Convert to NativeWindowBuffer
    NativeWindowBuffer* nativeWindowBuffer = CreateNativeWindowBufferFromSurfaceBuffer(&buffer);
    if (nativeWindowBuffer == nullptr) {
        RS_LOGE("ConvertSurfaceBufferToBackendTexture: CreateNativeWindowBufferFromSurfaceBuffer failed");
        return {};
    }

    // Create BackendTexture
    Drawing::BackendTexture texture = NativeBufferUtils::MakeBackendTextureFromNativeBuffer(
        nativeWindowBuffer, buffer->GetWidth(), buffer->GetHeight());
    // Destroy nativeWindowBuffer after use
    DestroyNativeWindowBuffer(nativeWindowBuffer);

    if (!texture.IsValid()) {
        RS_LOGE("ConvertSurfaceBufferToBackendTexture: MakeBackendTextureFromNativeBuffer failed");
        return {};
    }
    return texture;
}
} // namespace OHOS::Rosen
#endif // ROSEN_OHOS && RS_ENABLE_VK