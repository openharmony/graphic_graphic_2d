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

#include "rsmemorymanager_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>
#include <memory>

#include "memory/rs_memory_manager.h"
#include "memory/rs_memory_snapshot.h"
#include "image/gpu_context.h"

namespace OHOS {
namespace Rosen {

// Global GPU context, initialized in LLVMFuzzerInitialize, managed by unique_ptr (RAII)
std::unique_ptr<OHOS::Rosen::Drawing::GPUContext> g_gpuContext;

namespace {
const uint8_t DO_RELEASE_ALL_GPU_RES_BY_TAG = 0;
const uint8_t DO_RELEASE_ALL_GPU_RES_BY_PID = 1;
const uint8_t DO_RELEASE_UNLOCK_GPU_RES_BY_PID = 2;
const uint8_t DO_RELEASE_UNLOCK_GPU_RES_BY_NODE_ID = 3;
const uint8_t DO_PURGE_CACHE_BETWEEN_FRAMES = 4;
const uint8_t DO_MEMORY_OVERFLOW = 5;
const uint8_t DO_ERASE_PID_INFO = 7;
const uint8_t DO_RELEASE_UNLOCK_GPU_RES_BY_BOOL = 8;
const uint8_t DO_SET_GPU_MEMORY_LIMIT = 9;
const uint8_t TARGET_SIZE = 10;
constexpr uint8_t MAX_FUZZ_PID_SET_SIZE = 8;

// Construct GPUResourceTag from fuzz data
Drawing::GPUResourceTag CreateFuzzedTag(FuzzedDataProvider& fdp)
{
    pid_t pid = static_cast<pid_t>(fdp.ConsumeIntegral<int32_t>());
    uint32_t tid = fdp.ConsumeIntegral<uint32_t>();
    uint32_t wid = fdp.ConsumeIntegral<uint32_t>();
    uint32_t fid = fdp.ConsumeIntegral<uint32_t>();
    std::string tagName = fdp.ConsumeRandomLengthString(32);
    return Drawing::GPUResourceTag(pid, tid, wid, fid, tagName);
}

// Get gpuContext: null or valid, controlled by fuzz data
OHOS::Rosen::Drawing::GPUContext* GetFuzzedGpuContext(FuzzedDataProvider& fdp)
{
    bool useNull = fdp.ConsumeBool();
    return useNull ? nullptr : g_gpuContext.get();
}

// Construct pid set from fuzz data
std::set<pid_t> CreateFuzzedPidSet(FuzzedDataProvider& fdp, uint8_t maxCount)
{
    std::set<pid_t> pidSet;
    uint8_t count = fdp.ConsumeIntegralInRange<uint8_t>(0, maxCount);
    for (uint8_t i = 0; i < count; i++) {
        pidSet.insert(static_cast<pid_t>(fdp.ConsumeIntegral<int32_t>()));
    }
    return pidSet;
}

void DoReleaseAllGpuResourceByTag(FuzzedDataProvider& fdp)
{
    Drawing::GPUResourceTag tag = CreateFuzzedTag(fdp);
    OHOS::Rosen::Drawing::GPUContext* gpuContext = GetFuzzedGpuContext(fdp);
    MemoryManager::ReleaseAllGpuResource(gpuContext, tag);
}

void DoReleaseAllGpuResourceByPid(FuzzedDataProvider& fdp)
{
    pid_t pid = static_cast<pid_t>(fdp.ConsumeIntegral<int32_t>());
    OHOS::Rosen::Drawing::GPUContext* gpuContext = GetFuzzedGpuContext(fdp);
    MemoryManager::ReleaseAllGpuResource(gpuContext, pid);
}

void DoReleaseUnlockGpuResourceByPid(FuzzedDataProvider& fdp)
{
    pid_t pid = static_cast<pid_t>(fdp.ConsumeIntegral<int32_t>());
    OHOS::Rosen::Drawing::GPUContext* gpuContext = GetFuzzedGpuContext(fdp);
    MemoryManager::ReleaseUnlockGpuResource(gpuContext, pid);
}

void DoReleaseUnlockGpuResourceByNodeId(FuzzedDataProvider& fdp)
{
    NodeId nodeId = fdp.ConsumeIntegral<uint64_t>();
    OHOS::Rosen::Drawing::GPUContext* gpuContext = GetFuzzedGpuContext(fdp);
    MemoryManager::ReleaseUnlockGpuResource(gpuContext, nodeId);
}

void DoPurgeCacheBetweenFrames(FuzzedDataProvider& fdp)
{
    bool scratchResourceOnly = fdp.ConsumeBool();
    std::set<pid_t> exitedPidSet = CreateFuzzedPidSet(fdp, MAX_FUZZ_PID_SET_SIZE);
    std::set<pid_t> protectedPidSet = CreateFuzzedPidSet(fdp, MAX_FUZZ_PID_SET_SIZE);
    OHOS::Rosen::Drawing::GPUContext* gpuContext = GetFuzzedGpuContext(fdp);
    MemoryManager::PurgeCacheBetweenFrames(gpuContext, scratchResourceOnly, exitedPidSet, protectedPidSet);
}

void DoMemoryOverflow(FuzzedDataProvider& fdp)
{
    pid_t pid = static_cast<pid_t>(fdp.ConsumeIntegral<int32_t>());
    size_t overflowMemory = fdp.ConsumeIntegral<size_t>();
    bool isGpu = fdp.ConsumeBool();
    MemoryManager::MemoryOverflow(pid, overflowMemory, isGpu);
}

void DoErasePidInfo(FuzzedDataProvider& fdp)
{
    std::set<pid_t> exitedPidSet = CreateFuzzedPidSet(fdp, MAX_FUZZ_PID_SET_SIZE);
    MemoryManager::ErasePidInfo(exitedPidSet);
}

void DoReleaseUnlockGpuResourceByBool(FuzzedDataProvider& fdp)
{
    bool scratchResourcesOnly = fdp.ConsumeBool();
    OHOS::Rosen::Drawing::GPUContext* gpuContext = GetFuzzedGpuContext(fdp);
    MemoryManager::ReleaseUnlockGpuResource(gpuContext, scratchResourcesOnly);
}

void DoSetGpuMemoryLimit(FuzzedDataProvider& fdp)
{
    OHOS::Rosen::Drawing::GPUContext* gpuContext = GetFuzzedGpuContext(fdp);
    MemoryManager::SetGpuMemoryLimit(gpuContext);
}

} // namespace

} // namespace Rosen
} // namespace OHOS

/* Fuzzer environment initialization */
extern "C" int LLVMFuzzerInitialize(int* argc, char*** argv)
{
    // Pre-initialize GPU context to avoid repeated allocation overhead, managed by unique_ptr (RAII)
    OHOS::Rosen::g_gpuContext = std::make_unique<OHOS::Rosen::Drawing::GPUContext>();
    if (!OHOS::Rosen::g_gpuContext) {
        return -1;
    }
    return 0;
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return -1;
    }

    FuzzedDataProvider fdp(data, size);

    /* Run your code on data */
    uint8_t tarPos = fdp.ConsumeIntegral<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_RELEASE_ALL_GPU_RES_BY_TAG:
            OHOS::Rosen::DoReleaseAllGpuResourceByTag(fdp);
            break;
        case OHOS::Rosen::DO_RELEASE_ALL_GPU_RES_BY_PID:
            OHOS::Rosen::DoReleaseAllGpuResourceByPid(fdp);
            break;
        case OHOS::Rosen::DO_RELEASE_UNLOCK_GPU_RES_BY_PID:
            OHOS::Rosen::DoReleaseUnlockGpuResourceByPid(fdp);
            break;
        case OHOS::Rosen::DO_RELEASE_UNLOCK_GPU_RES_BY_NODE_ID:
            OHOS::Rosen::DoReleaseUnlockGpuResourceByNodeId(fdp);
            break;
        case OHOS::Rosen::DO_PURGE_CACHE_BETWEEN_FRAMES:
            OHOS::Rosen::DoPurgeCacheBetweenFrames(fdp);
            break;
        case OHOS::Rosen::DO_MEMORY_OVERFLOW:
            OHOS::Rosen::DoMemoryOverflow(fdp);
            break;
        case OHOS::Rosen::DO_ERASE_PID_INFO:
            OHOS::Rosen::DoErasePidInfo(fdp);
            break;
        case OHOS::Rosen::DO_RELEASE_UNLOCK_GPU_RES_BY_BOOL:
            OHOS::Rosen::DoReleaseUnlockGpuResourceByBool(fdp);
            break;
        case OHOS::Rosen::DO_SET_GPU_MEMORY_LIMIT:
            OHOS::Rosen::DoSetGpuMemoryLimit(fdp);
            break;
        default:
            return -1;
    }
    return 0;
}
