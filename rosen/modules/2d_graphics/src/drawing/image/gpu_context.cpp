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

#include "image/gpu_context.h"

#include "impl_factory.h"
#ifdef RS_ENABLE_VK
#include "include/gpu/vk/GrVkBackendContext.h"
#endif
#include "utils/system_properties.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
GPUContext::GPUContext() : impl_(ImplFactory::CreateGPUContextImpl()) {}

bool GPUContext::BuildFromGL(const GPUContextOptions& options)
{
    return impl_->BuildFromGL(options);
}

#ifdef RS_ENABLE_VK
bool GPUContext::BuildFromVK(const GrVkBackendContext& context)
{
    if (SystemProperties::GetGpuApiType() != GpuApiType::VULKAN &&
        SystemProperties::GetGpuApiType() != GpuApiType::DDGR) {
        return false;
    }
    return impl_->BuildFromVK(context);
}

bool GPUContext::BuildFromVK(const GrVkBackendContext& context, const GPUContextOptions& options)
{
    if (SystemProperties::GetGpuApiType() != GpuApiType::VULKAN &&
        SystemProperties::GetGpuApiType() != GpuApiType::DDGR) {
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

void GPUContext::Flush()
{
    impl_->Flush();
}

void GPUContext::FlushAndSubmit(bool syncCpu)
{
    impl_->FlushAndSubmit(syncCpu);
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

void GPUContext::PurgeUnlockAndSafeCacheGpuResources()
{
    impl_->PurgeUnlockAndSafeCacheGpuResources();
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

#ifdef RS_ENABLE_VK
void GPUContext::StoreVkPipelineCacheData()
{
    impl_->StoreVkPipelineCacheData();
}
#endif

GPUContextOptions::PersistentCache* GPUContextOptions::GetPersistentCache() const
{
    return persistentCache_;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
