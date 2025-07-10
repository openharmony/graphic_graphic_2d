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

#include "image/gpu_context.h"

#include "config/DrawingConfig.h"
#include "impl_factory.h"
#include "static_factory.h"
#ifdef RS_ENABLE_VK
#ifdef USE_M133_SKIA
#include "include/gpu/vk/VulkanBackendContext.h"
#else
#include "include/gpu/vk/GrVkBackendContext.h"
#endif
#endif
#include "utils/system_properties.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

void GPUResourceTag::SetCurrentNodeId(uint64_t nodeId)
{
#ifdef ROSEN_OHOS
    if (SystemProperties::IsVkImageDfxEnabled()) {
        StaticFactory::SetCurrentNodeId(nodeId);
    }
#endif
}

GPUContext::GPUContext() : impl_(ImplFactory::CreateGPUContextImpl()) {}

bool GPUContext::BuildFromGL(const GPUContextOptions& options)
{
    return impl_->BuildFromGL(options);
}

#ifdef RS_ENABLE_VK
#ifdef USE_M133_SKIA
bool GPUContext::BuildFromVK(const skgpu::VulkanBackendContext& context)
#else
bool GPUContext::BuildFromVK(const GrVkBackendContext& context)
#endif
{
    if (!SystemProperties::IsUseVulkan()) {
        return false;
    }
    return impl_->BuildFromVK(context);
}

#ifdef USE_M133_SKIA
bool GPUContext::BuildFromVK(const skgpu::VulkanBackendContext& context, const GPUContextOptions& options)
#else
bool GPUContext::BuildFromVK(const GrVkBackendContext& context, const GPUContextOptions& options)
#endif
{
    if (!SystemProperties::IsUseVulkan()) {
        return false;
    }
    return impl_->BuildFromVK(context, options);
}
#endif

void GPUContext::GetResourceCacheLimits(int* maxResource, size_t* maxResourceBytes) const
{
    if (impl_ != nullptr) {
        impl_->GetResourceCacheLimits(maxResource, maxResourceBytes);
    }
}

void GPUContext::SetResourceCacheLimits(int maxResource, size_t maxResourceBytes)
{
    impl_->SetResourceCacheLimits(maxResource, maxResourceBytes);
}

void GPUContext::SetPurgeableResourceLimit(int purgeableMaxCount)
{
    if (impl_) {
        impl_->SetPurgeableResourceLimit(purgeableMaxCount);
    }
}

void GPUContext::Flush()
{
    impl_->Flush();
}

void GPUContext::FlushAndSubmit(bool syncCpu)
{
    impl_->FlushAndSubmit(syncCpu);
#ifdef DRAWING_DISABLE_API
    DrawingConfig::UpdateDrawingProperties();
#endif
}

void GPUContext::Submit()
{
    impl_->Submit();
}

void GPUContext::PerformDeferredCleanup(std::chrono::milliseconds msNotUsed)
{
    impl_->PerformDeferredCleanup(msNotUsed);
}

void GPUContextOptions::SetPersistentCache(PersistentCache* persistentCache)
{
    persistentCache_ = persistentCache;
}

void GPUContextOptions::SetAllowPathMaskCaching(bool allowPathMaskCaching)
{
    allowPathMaskCaching_ = allowPathMaskCaching;
}

bool GPUContextOptions::GetAllowPathMaskCaching() const
{
    return allowPathMaskCaching_;
}

void GPUContext::GetResourceCacheUsage(int* resourceCount, size_t* resourceBytes) const
{
    impl_->GetResourceCacheUsage(resourceCount, resourceBytes);
}

void GPUContext::FreeGpuResources()
{
    impl_->FreeGpuResources();
}

void GPUContext::ReclaimResources()
{
    impl_->ReclaimResources();
}

void GPUContext::DumpAllResource(std::stringstream& dump) const
{
#ifdef ROSEN_OHOS
    if (SystemProperties::IsVkImageDfxEnabled()) {
        impl_->DumpAllResource(dump);
    }
#endif
}

void GPUContext::DumpGpuStats(std::string& out) const
{
    impl_->DumpGpuStats(out);
}

void GPUContext::ReleaseResourcesAndAbandonContext()
{
    impl_->ReleaseResourcesAndAbandonContext();
}

void GPUContext::PurgeUnlockedResources(bool scratchResourcesOnly)
{
    impl_->PurgeUnlockedResources(scratchResourcesOnly);
}

void GPUContext::PurgeUnlockedResourcesByTag(bool scratchResourcesOnly, const GPUResourceTag &tag)
{
    impl_->PurgeUnlockedResourcesByTag(scratchResourcesOnly, tag);
}

void GPUContext::PurgeUnlockedResourcesByPid(bool scratchResourcesOnly, const std::set<pid_t>& exitedPidSet)
{
    impl_->PurgeUnlockedResourcesByPid(scratchResourcesOnly, exitedPidSet);
}

void GPUContext::RegisterVulkanErrorCallback(const std::function<void()>& vulkanErrorCallback)
{
    impl_->RegisterVulkanErrorCallback(vulkanErrorCallback);
}

void GPUContext::PurgeUnlockAndSafeCacheGpuResources()
{
    impl_->PurgeUnlockAndSafeCacheGpuResources();
}
void GPUContext::PurgeCacheBetweenFrames(bool scratchResourcesOnly, const std::set<pid_t>& exitedPidSet,
    const std::set<pid_t>& protectedPidSet)
{
    impl_->PurgeCacheBetweenFrames(scratchResourcesOnly, exitedPidSet, protectedPidSet);
}

void GPUContext::ReleaseByTag(const GPUResourceTag &tag)
{
    impl_->ReleaseByTag(tag);
}

void GPUContext::DumpMemoryStatisticsByTag(TraceMemoryDump* traceMemoryDump, GPUResourceTag &tag) const
{
    impl_->DumpMemoryStatisticsByTag(traceMemoryDump, tag);
}

void GPUContext::DumpMemoryStatistics(TraceMemoryDump* traceMemoryDump) const
{
    impl_->DumpMemoryStatistics(traceMemoryDump);
}

void GPUContext::SetCurrentGpuResourceTag(const GPUResourceTag &tag)
{
    impl_->SetCurrentGpuResourceTag(tag);
}

GPUResourceTag GPUContext::GetCurrentGpuResourceTag() const
{
    return impl_->GetCurrentGpuResourceTag();
}

void GPUContext::GetUpdatedMemoryMap(std::unordered_map<pid_t, size_t> &out)
{
    impl_->GetUpdatedMemoryMap(out);
}

void GPUContext::InitGpuMemoryLimit(MemoryOverflowCalllback callback, uint64_t size)
{
    impl_->InitGpuMemoryLimit(callback, size);
}

void GPUContext::ResetContext()
{
    impl_->ResetContext();
}

void GPUContext::GenerateSubmitInfo(int seq)
{
    impl_->GenerateSubmitInfo(seq);
}

void GPUContext::FlushCommands()
{
    impl_->FlushCommands();
}

#ifdef RS_ENABLE_VK
void GPUContext::StoreVkPipelineCacheData()
{
    impl_->StoreVkPipelineCacheData();
}
#endif

void GPUContext::RegisterPostFunc(const std::function<void(const std::function<void()>& task)>& func)
{
    impl_->RegisterPostFunc(func);
}

GPUContextOptions::PersistentCache* GPUContextOptions::GetPersistentCache() const
{
    return persistentCache_;
}

void GPUContext::VmaDefragment()
{
    impl_->VmaDefragment();
}

void GPUContext::BeginFrame()
{
    impl_->BeginFrame();
}

void GPUContext::EndFrame()
{
    impl_->EndFrame();
}

void GPUContext::SetGpuCacheSuppressWindowSwitch(bool enabled)
{
    impl_->SetGpuCacheSuppressWindowSwitch(enabled);
}

void GPUContext::SetGpuMemoryAsyncReclaimerSwitch(bool enabled, const std::function<void()>& setThreadPriority)
{
    impl_->SetGpuMemoryAsyncReclaimerSwitch(enabled, setThreadPriority);
}

void GPUContext::FlushGpuMemoryInWaitQueue()
{
    impl_->FlushGpuMemoryInWaitQueue();
}

void GPUContext::SetEarlyZFlag(bool flag)
{
    impl_->SetEarlyZFlag(flag);
}

void GPUContext::SuppressGpuCacheBelowCertainRatio(const std::function<bool(void)>& nextFrameHasArrived)
{
    impl_->SuppressGpuCacheBelowCertainRatio(nextFrameHasArrived);
}

void GPUContext::GetHpsEffectSupport(std::vector<const char*>& instanceExtensions)
{
    impl_->GetHpsEffectSupport(instanceExtensions);
}

void GPUContextOptions::SetStoreCachePath(const std::string& filePath)
{
    filePath_ = filePath;
}

std::string GPUContextOptions::GetStoreCachePath() const
{
    return filePath_;
}

void GPUContextOptions::SetIsUniRender(bool isUniRender)
{
    isUniRender_ = isUniRender;
}

bool GPUContextOptions::GetIsUniRender() const
{
    return isUniRender_;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
