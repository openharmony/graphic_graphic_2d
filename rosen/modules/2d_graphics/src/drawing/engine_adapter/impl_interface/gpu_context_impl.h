/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef GPU_CONTEXT_IMPL_H
#define GPU_CONTEXT_IMPL_H

#include <chrono>
#include <functional>
#include <set>

#include "base_impl.h"

#include "image/trace_memory_dump.h"
#ifdef RS_ENABLE_VK
#include "include/gpu/vk/GrVkBackendContext.h"
#endif

namespace OHOS {
namespace Rosen {
namespace Drawing {
using pid_t = int;
struct GPUResourceTag;
class GPUContext;
class GPUContextOptions;
class GPUContextImpl : public BaseImpl {
public:
    GPUContextImpl() {};
    ~GPUContextImpl() override {};

    virtual bool BuildFromGL(const GPUContextOptions& options) = 0;
#ifdef RS_ENABLE_VK
    virtual bool BuildFromVK(const GrVkBackendContext& context) = 0;
    virtual bool BuildFromVK(const GrVkBackendContext& context, const GPUContextOptions& options) = 0;
#endif
    virtual void Flush() = 0;
    virtual void FlushAndSubmit(bool syncCpu) = 0;
    virtual void Submit() = 0;
    virtual void PerformDeferredCleanup(std::chrono::milliseconds msNotUsed) = 0;

    virtual void GetResourceCacheLimits(int* maxResource, size_t* maxResourceBytes) const = 0;
    virtual void SetResourceCacheLimits(int maxResource, size_t maxResourceBytes) = 0;

    virtual void GetResourceCacheUsage(int* resourceCount, size_t* resourceBytes) const = 0;

    virtual void FreeGpuResources() = 0;

    virtual void DumpGpuStats(std::string& out) = 0;

    virtual void ReleaseResourcesAndAbandonContext() = 0;

    virtual void PurgeUnlockedResources(bool scratchResourcesOnly) = 0;

    virtual void PurgeUnlockedResourcesByTag(bool scratchResourcesOnly, const GPUResourceTag &tag) = 0;

    virtual void PurgeUnlockedResourcesByPid(bool scratchResourcesOnly, const std::set<pid_t>& exitedPidSet) = 0;

    virtual void PurgeUnlockAndSafeCacheGpuResources() = 0;

    virtual void ReleaseByTag(const GPUResourceTag &tag) = 0;

    virtual void DumpMemoryStatisticsByTag(TraceMemoryDump* traceMemoryDump, GPUResourceTag &tag) = 0;

    virtual void DumpMemoryStatistics(TraceMemoryDump* traceMemoryDump) = 0;

    virtual void SetCurrentGpuResourceTag(const GPUResourceTag &tag) = 0;

#ifdef RS_ENABLE_VK
    virtual void StoreVkPipelineCacheData() = 0;
#endif

    virtual void RegisterPostFunc(const std::function<void(const std::function<void()>& task)>& func) = 0;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif // GPU_CONTEXT_IMPL_H
