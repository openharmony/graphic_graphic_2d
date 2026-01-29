/*
* Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "rscanvasdmabuffercache_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "ipc_callbacks/rs_canvas_surface_buffer_callback_stub.h"
#include "memory/rs_canvas_dma_buffer_cache.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t DO_NOTIFY_CANVAS_SURFACE_BUFFER_CHANGED = 0;
const uint8_t DO_ADD_PENDING_BUFFER = 1;
const uint8_t DO_ACQUIRE_PENDING_BUFFER = 2;
const uint8_t DO_REMOVE_PENDING_BUFFER = 3;
const uint8_t DO_CLEAR_PENDING_BUFFER_BY_NODEID = 4;
const uint8_t DO_CLEAR_PENDING_BUFFER_BY_PID = 5;
const uint8_t TARGET_SIZE = 6;

const uint8_t* DATA = nullptr;
size_t g_size = 0;
size_t g_pos;

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

bool Init(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    DATA = data;
    g_size = size;
    g_pos = 0;
    return true;
}
} // namespace

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

void DoNotifyCanvasSurfaceBufferChanged()
{
    NodeId nodeId = GetData<NodeId>();
    auto& bufferCache = RSCanvasDmaBufferCache::GetInstance();
    sptr<SurfaceBuffer> buffer = SurfaceBuffer::Create();
    uint32_t resetSurfaceIndex = GetData<uint32_t>();
    bufferCache.NotifyCanvasSurfaceBufferChanged(nodeId, buffer, resetSurfaceIndex);
}

void DoAddPendingBuffer()
{
    NodeId nodeId = GetData<NodeId>();
    auto& bufferCache = RSCanvasDmaBufferCache::GetInstance();
    sptr<SurfaceBuffer> buffer = SurfaceBuffer::Create();
    uint32_t resetSurfaceIndex = GetData<uint32_t>();
    bufferCache.AddPendingBuffer(nodeId, buffer, resetSurfaceIndex);
}

void DoAcquirePendingBuffer()
{
    NodeId nodeId = GetData<NodeId>();
    auto& bufferCache = RSCanvasDmaBufferCache::GetInstance();
    sptr<SurfaceBuffer> buffer = SurfaceBuffer::Create();
    uint32_t resetSurfaceIndex = GetData<uint32_t>();
    bufferCache.AcquirePendingBuffer(nodeId, resetSurfaceIndex);
}

void DoRemovePendingBuffer()
{
    NodeId nodeId = GetData<NodeId>();
    auto& bufferCache = RSCanvasDmaBufferCache::GetInstance();
    sptr<SurfaceBuffer> buffer = SurfaceBuffer::Create();
    uint32_t resetSurfaceIndex = GetData<uint32_t>();
    bufferCache.RemovePendingBuffer(nodeId, resetSurfaceIndex);
}

void DoClearPendingBufferByNodeId()
{
    NodeId nodeId = GetData<NodeId>();
    auto& bufferCache = RSCanvasDmaBufferCache::GetInstance();
    bufferCache.ClearPendingBufferByNodeId(nodeId);
}

void DoClearPendingBufferByPid()
{
    auto& bufferCache = RSCanvasDmaBufferCache::GetInstance();
    pid_t pid = GetData<pid_t>();
    bufferCache.ClearPendingBufferByPid(pid);
}

} // namespace Rosen
} // namespace OHOS

/* Fuzzer envirement */
extern "C" int LLVMFuzzerInitialize(int *argc, char ***argv)
{
    return 0;
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (!OHOS::Rosen::Init(data, size)) {
        return -1;
    }
    /* Run your code on data */
    uint8_t tarPos = OHOS::Rosen::GetData<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_NOTIFY_CANVAS_SURFACE_BUFFER_CHANGED:
            OHOS::Rosen::DoNotifyCanvasSurfaceBufferChanged();
            break;
        case OHOS::Rosen::DO_ADD_PENDING_BUFFER:
            OHOS::Rosen::DoAddPendingBuffer();
            break;
        case OHOS::Rosen::DO_ACQUIRE_PENDING_BUFFER:
            OHOS::Rosen::DoAcquirePendingBuffer();
            break;
        case OHOS::Rosen::DO_REMOVE_PENDING_BUFFER:
            OHOS::Rosen::DoRemovePendingBuffer();
            break;
        case OHOS::Rosen::DO_CLEAR_PENDING_BUFFER_BY_NODEID:
            OHOS::Rosen::DoClearPendingBufferByNodeId();
            break;
        case OHOS::Rosen::DO_CLEAR_PENDING_BUFFER_BY_PID:
            OHOS::Rosen::DoClearPendingBufferByPid();
            break;
        default:
            return -1;
    }
    return 0;
}