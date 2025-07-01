/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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
#include <set>
#include <functional>
#include <unordered_map>
#include "base_impl.h"
#include "image/trace_memory_dump.h"
#ifdef RS_ENABLE_VK
#ifdef USE_M133_SKIA
#include "include/gpu/vk/VulkanBackendContext.h"
#else
#include "include/gpu/vk/GrVkBackendContext.h"
#endif
#endif

namespace OHOS {
namespace Rosen {
namespace Drawing {
using pid_t = int;
using MemoryOverflowCalllback = std::function<void(pid_t, uint64_t, bool)>;
struct GPUResourceTag;
struct HpsBlurParameter;
class GPUContext;
class GPUContextOptions;
class GPUContextImpl : public BaseImpl {
public:
    GPUContextImpl() {};
    ~GPUContextImpl() override {};

    virtual bool BuildFromGL(const GPUContextOptions& options) = 0;
#ifdef RS_ENABLE_VK
#ifdef USE_M133_SKIA
    virtual bool BuildFromVK(const skgpu::VulkanBackendContext& context) = 0;
    virtual bool BuildFromVK(const skgpu::VulkanBackendContext& context, const GPUContextOptions& options) = 0;
#else
    virtual bool BuildFromVK(const GrVkBackendContext& context) = 0;
    virtual bool BuildFromVK(const GrVkBackendContext& context, const GPUContextOptions& options) = 0;
#endif
#endif
    virtual void Flush() = 0;
    virtual void FlushAndSubmit(bool syncCpu) = 0;

    virtual void GenerateSubmitInfo(int seq) {}
    virtual void FlushCommands() {}

    virtual void Submit() = 0;
    virtual void PerformDeferredCleanup(std::chrono::milliseconds msNotUsed) = 0;

    virtual void GetResourceCacheLimits(int* maxResource, size_t* maxResourceBytes) const = 0;
    virtual void SetResourceCacheLimits(int maxResource, size_t maxResourceBytes) = 0;
    virtual void SetPurgeableResourceLimit(int purgeableMaxCount) = 0;

    virtual void GetResourceCacheUsage(int* resourceCount, size_t* resourceBytes) const = 0;

    virtual void FreeGpuResources() = 0;

    virtual void ReclaimResources() = 0;

    virtual void DumpGpuStats(std::string& out) = 0;

    virtual void DumpAllResource(std::stringstream& dump) = 0;

    virtual void ReleaseResourcesAndAbandonContext() = 0;

    virtual void PurgeUnlockedResources(bool scratchResourcesOnly) = 0;

    virtual void PurgeUnlockedResourcesByTag(bool scratchResourcesOnly, const GPUResourceTag &tag) = 0;

    virtual void PurgeUnlockedResourcesByPid(bool scratchResourcesOnly, const std::set<pid_t>& exitedPidSet) = 0;

    virtual void RegisterVulkanErrorCallback(const std::function<void()>& vulkanErrorCallback) = 0;

    virtual void PurgeCacheBetweenFrames(bool scratchResourcesOnly, const std::set<pid_t>& exitedPidSet,
        const std::set<pid_t>& protectedPidSet) = 0;
    
    virtual void PurgeUnlockAndSafeCacheGpuResources() = 0;

    virtual void ReleaseByTag(const GPUResourceTag &tag) = 0;

    virtual void ResetContext() = 0;

    virtual void DumpMemoryStatisticsByTag(TraceMemoryDump* traceMemoryDump, GPUResourceTag &tag) = 0;

    virtual void DumpMemoryStatistics(TraceMemoryDump* traceMemoryDump) = 0;

    virtual void SetCurrentGpuResourceTag(const GPUResourceTag &tag) = 0;

    virtual GPUResourceTag GetCurrentGpuResourceTag() const = 0;

    virtual void GetUpdatedMemoryMap(std::unordered_map<pid_t, size_t> &out) = 0;

    virtual void InitGpuMemoryLimit(MemoryOverflowCalllback callback, uint64_t size) = 0;

#ifdef RS_ENABLE_VK
    virtual void StoreVkPipelineCacheData() = 0;
#endif

    virtual void RegisterPostFunc(const std::function<void(const std::function<void()>& task)>& func) = 0;

    virtual void VmaDefragment() = 0;

    virtual void BeginFrame() = 0;

    virtual void EndFrame() = 0;

    virtual void SetGpuCacheSuppressWindowSwitch(bool enabled) = 0;

    virtual void SetGpuMemoryAsyncReclaimerSwitch(bool enabled, const std::function<void()>& setThreadPriority) = 0;

    virtual void FlushGpuMemoryInWaitQueue() = 0;
    
    virtual void SuppressGpuCacheBelowCertainRatio(const std::function<bool(void)>& nextFrameHasArrived) = 0;

    virtual void GetHpsEffectSupport(std::vector<const char*>& instanceExtensions) = 0;

    virtual void SetEarlyZFlag(bool flag) = 0;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif // GPU_CONTEXT_IMPL_H
