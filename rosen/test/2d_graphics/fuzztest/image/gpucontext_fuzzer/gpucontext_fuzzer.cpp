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

#include "gpucontext_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "get_object.h"

#include "image/gpu_context.h"
#include "render_context/shader_cache.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t DATA_MIN_SIZE = 2;
constexpr size_t MAX_SIZE = 5000;
} // namespace
namespace Drawing {
/*
 * 测试以下 GPUContext 接口：
 * 1. SetResourceCacheLimits(int maxResource, size_t maxResourceBytes)
 * 2. GetResourceCacheLimits(int* maxResource, size_t* maxResourceBytes)
 * 3. ResetContext()
 * 4. Flush()
 * 5. Submit()
 * 6. FlushAndSubmit(bool syncCpu)
 * 7. GetResourceCacheUsage(int* resourceCount, size_t* resourceBytes)
 * 8. FreeGpuResources()
 */
bool GPUContextFuzzTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    GPUContextOptions options;
    bool allowPathMaskCaching = GetObject<bool>();
    options.SetAllowPathMaskCaching(allowPathMaskCaching);
    options.GetAllowPathMaskCaching();
    auto& cache = ShaderCache::Instance();
    uint32_t count = GetObject<uint32_t>() % MAX_SIZE + 1;
    char* text = new char[count];
    for (size_t i = 0; i < count; i++) {
        text[i] = GetObject<char>();
    }
    text[count - 1] = '\0';
    std::string str(text);
    cache.SetFilePath(str);
    bool isUni = GetObject<bool>();
    cache.InitShaderCache(text, count, isUni);
    options.SetPersistentCache(&cache);
    options.GetPersistentCache();
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    int maxResource = GetObject<int>();
    size_t maxResourceBytes = GetObject<size_t>();
    gpuContext->SetResourceCacheLimits(maxResource, maxResourceBytes);
    gpuContext->GetResourceCacheLimits(&maxResource, &maxResourceBytes);
    gpuContext->ResetContext();
    gpuContext->Flush();
    gpuContext->Submit();
    bool syncCpu = GetObject<bool>();
    gpuContext->FlushAndSubmit(syncCpu);
    int resourceCount = GetObject<int>();
    size_t resourceBytes = GetObject<size_t>();
    gpuContext->GetResourceCacheUsage(&resourceCount, &resourceBytes);
    gpuContext->FreeGpuResources();
    if (text != nullptr) {
        delete [] text;
        text = nullptr;
    }
    return true;
}

/*
 * 测试以下 GPUContext 接口：
 * 1. SetResourceCacheLimits(int maxResource, size_t maxResourceBytes)
 * 2. DumpGpuStats(const std::string& str)
 * 3. ReleaseResourcesAndAbandonContext()
 * 4. PurgeUnlockedResources(bool scratchResourcesOnly)
 * 5. PurgeUnlockedResourcesByTag(bool scratchResourcesOnly, const GPUResourceTag& tag)
 * 6. PerformDeferredCleanup(std::chrono::milliseconds msNotUsed)
 * 7. PurgeUnlockedResourcesByPid(bool scratchResourcesOnly, const std::set<pid_t>& exitedPidSet)
 * 8. PurgeCacheBetweenFrames(bool scratchResourcesOnly, const std::set<pid_t>& exitedPidSet,
 *      const std::set<pid_t>& protectedPidSet)
 * 9. PurgeUnlockAndSafeCacheGpuResources()
 * 10. SetCurrentGpuResourceTag(const GPUResourceTag& tag)
 * 11. ReleaseByTag(const GPUResourceTag& tag)
 */
bool GPUContextFuzzTest002(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    uint32_t count = GetObject<uint32_t>() % MAX_SIZE + 1;
    char* text = new char[count];
    for (size_t i = 0; i < count; i++) {
        text[i] = GetObject<char>();
    }
    text[count - 1] = '\0';
    std::string str(text);
    int maxResource = GetObject<int>();
    size_t maxResourceBytes = GetObject<size_t>();
    gpuContext->SetResourceCacheLimits(maxResource, maxResourceBytes);
    gpuContext->DumpGpuStats(str);
    gpuContext->ReleaseResourcesAndAbandonContext();
    bool scratchResourcesOnly = GetObject<bool>();
    gpuContext->PurgeUnlockedResources(scratchResourcesOnly);
    uint32_t fPid = GetObject<uint32_t>();
    uint32_t fTid = GetObject<uint32_t>();
    uint32_t fWid = GetObject<uint32_t>();
    uint32_t fFid = GetObject<uint32_t>();
    GPUResourceTag tag(fPid, fTid, fWid, fFid, str);
    gpuContext->PurgeUnlockedResourcesByTag(scratchResourcesOnly, tag);
    std::chrono::milliseconds msNotUsed = GetObject<std::chrono::milliseconds>();
    gpuContext->PerformDeferredCleanup(msNotUsed);
    std::set<pid_t> exitedPidSet;
    gpuContext->PurgeUnlockedResourcesByPid(scratchResourcesOnly, exitedPidSet);
    std::set<pid_t> protectedPidSet;
    gpuContext->PurgeCacheBetweenFrames(scratchResourcesOnly, exitedPidSet, protectedPidSet);
    gpuContext->PurgeUnlockAndSafeCacheGpuResources();
    gpuContext->SetCurrentGpuResourceTag(tag);
    gpuContext->ReleaseByTag(tag);
    if (text != nullptr) {
        delete [] text;
        text = nullptr;
    }
    return true;
}

/*
 * 测试以下 GPUContext 接口：
 * 1. SetResourceCacheLimits(int maxResource, size_t maxResourceBytes)
 * 2. DumpMemoryStatisticsByTag(TraceMemoryDump* traceMemoryDump, const GPUResourceTag& tag)
 * 3. DumpMemoryStatistics(TraceMemoryDump* traceMemoryDump)
 */
bool GPUContextFuzzTest003(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    int maxResource = GetObject<int>();
    size_t maxResourceBytes = GetObject<size_t>();
    gpuContext->SetResourceCacheLimits(maxResource, maxResourceBytes);
    auto dataVal = std::make_shared<Data>();
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* categoryKey = new char[length];
    for (size_t i = 0; i < length; i++) {
        categoryKey[i] = GetObject<char>();
    }
    categoryKey[length - 1] = '\0';
    std::string str(categoryKey);
    bool itemizeType = GetObject<bool>();
    TraceMemoryDump traceMemoryDump = TraceMemoryDump(categoryKey, itemizeType);
    uint32_t fPid = GetObject<uint32_t>();
    uint32_t fTid = GetObject<uint32_t>();
    uint32_t fWid = GetObject<uint32_t>();
    uint32_t fFid = GetObject<uint32_t>();
    GPUResourceTag tag(fPid, fTid, fWid, fFid, str);
    gpuContext->DumpMemoryStatisticsByTag(&traceMemoryDump, tag);
    gpuContext->DumpMemoryStatistics(&traceMemoryDump);
    if (categoryKey != nullptr) {
        delete [] categoryKey;
        categoryKey = nullptr;
    }
    return true;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::GPUContextFuzzTest001(data, size);
    OHOS::Rosen::Drawing::GPUContextFuzzTest002(data, size);
    OHOS::Rosen::Drawing::GPUContextFuzzTest003(data, size);
    return 0;
}
