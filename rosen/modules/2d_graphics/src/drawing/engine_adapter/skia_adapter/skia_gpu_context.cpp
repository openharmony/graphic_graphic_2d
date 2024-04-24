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

#include "skia_gpu_context.h"
#include <mutex>
#include "include/gpu/gl/GrGLInterface.h"
#include "src/gpu/GrDirectContextPriv.h"
#include "include/core/SkTypes.h"

#include "skia_data.h"
#include "utils/data.h"
#include "utils/log.h"
#include "skia_trace_memory_dump.h"
#include "utils/system_properties.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
SkiaPersistentCache::SkiaPersistentCache(GPUContextOptions::PersistentCache* cache) : cache_(cache) {}

sk_sp<SkData> SkiaPersistentCache::load(const SkData& key)
{
    Data keyData;
    if (!cache_) {
        LOGD("SkiaPersistentCache::load, failed! cache or key invalid");
        return nullptr;
    }
    auto skiaKeyDataImpl = keyData.GetImpl<SkiaData>();
    skiaKeyDataImpl->SetSkData(sk_ref_sp(&key));

    auto retData = cache_->Load(keyData);
    if (retData == nullptr) {
        LOGD("SkiaPersistentCache::load, failed! load data invalid");
        return nullptr;
    }

    return retData->GetImpl<SkiaData>()->GetSkData();
}

void SkiaPersistentCache::store(const SkData& key, const SkData& data)
{
    Data keyData;
    Data storeData;
    if (!cache_) {
        LOGD("SkiaPersistentCache::store, failed! cache or {key,data} invalid");
        return;
    }

    keyData.GetImpl<SkiaData>()->SetSkData(sk_ref_sp(&key));
    storeData.GetImpl<SkiaData>()->SetSkData(sk_ref_sp(&data));
    cache_->Store(keyData, storeData);
}

SkiaGPUContext::SkiaGPUContext() : grContext_(nullptr), skiaPersistentCache_(nullptr) {}

std::unique_ptr<SkExecutor> SkiaGPUContext::threadPool = nullptr;
void SkiaGPUContext::InitSkExecutor()
{
    static std::mutex mtx;
    mtx.lock();
    if (threadPool == nullptr) {
        threadPool = SkExecutor::MakeFIFOThreadPool(2); // 2 threads async task
    }
    mtx.unlock();
}

bool SkiaGPUContext::BuildFromGL(const GPUContextOptions& options)
{
    sk_sp<const GrGLInterface> glInterface(GrGLCreateNativeInterface());
    if (options.GetPersistentCache() != nullptr) {
        skiaPersistentCache_ = std::make_shared<SkiaPersistentCache>(options.GetPersistentCache());
    }

    InitSkExecutor();
    GrContextOptions grOptions;
    grOptions.fGpuPathRenderers &= ~GpuPathRenderers::kCoverageCounting;
    // fix svg antialiasing bug
    grOptions.fGpuPathRenderers &= ~GpuPathRenderers::kAtlas;
    grOptions.fPreferExternalImagesOverES3 = true;
    grOptions.fDisableDistanceFieldPaths = true;
    grOptions.fAllowPathMaskCaching = options.GetAllowPathMaskCaching();
    grOptions.fPersistentCache = skiaPersistentCache_.get();
    grOptions.fExecutor = threadPool.get();
#ifdef NEW_SKIA
    grContext_ = GrDirectContext::MakeGL(std::move(glInterface), grOptions);
#else
    grContext_ = GrContext::MakeGL(std::move(glInterface), grOptions);
#endif
    return grContext_ != nullptr ? true : false;
}

#ifdef RS_ENABLE_VK
bool SkiaGPUContext::BuildFromVK(const GrVkBackendContext& context)
{
    if (!SystemProperties::IsUseVulkan()) {
        return false;
    }
    InitSkExecutor();
    GrContextOptions grOptions;
    grOptions.fExecutor = threadPool.get();
    grContext_ = GrDirectContext::MakeVulkan(context, grOptions);
    return grContext_ != nullptr;
}

bool SkiaGPUContext::BuildFromVK(const GrVkBackendContext& context, const GPUContextOptions& options)
{
    if (!SystemProperties::IsUseVulkan()) {
        return false;
    }
    if (options.GetPersistentCache() != nullptr) {
        skiaPersistentCache_ = std::make_shared<SkiaPersistentCache>(options.GetPersistentCache());
    }
    InitSkExecutor();
    GrContextOptions grOptions;
    grOptions.fGpuPathRenderers &= ~GpuPathRenderers::kCoverageCounting;
    // fix svg antialiasing bug
    grOptions.fGpuPathRenderers &= ~GpuPathRenderers::kAtlas;
    grOptions.fPreferExternalImagesOverES3 = true;
    grOptions.fDisableDistanceFieldPaths = true;
    grOptions.fAllowPathMaskCaching = options.GetAllowPathMaskCaching();
    grOptions.fPersistentCache = skiaPersistentCache_.get();
    grOptions.fExecutor = threadPool.get();
    grContext_ = GrDirectContext::MakeVulkan(context, grOptions);
    return grContext_ != nullptr;
}
#endif

void SkiaGPUContext::Flush()
{
    if (!grContext_) {
        LOGD("SkiaGPUContext::Flush, grContext_ is nullptr");
        return;
    }
    grContext_->flush();
}

void SkiaGPUContext::FlushAndSubmit(bool syncCpu)
{
    if (!grContext_) {
        LOGD("SkiaGPUContext::FlushAndSubmit, grContext_ is nullptr");
        return;
    }
    grContext_->flushAndSubmit(syncCpu);
}

void SkiaGPUContext::Submit()
{
    if (!grContext_) {
        LOGD("SkiaGPUContext::Submit, grContext_ is nullptr");
        return;
    }
    grContext_->submit();
}

void SkiaGPUContext::PerformDeferredCleanup(std::chrono::milliseconds msNotUsed)
{
    if (!grContext_) {
        LOGD("SkiaGPUContext::PerformDeferredCleanup, grContext_ is nullptr");
        return;
    }
    grContext_->performDeferredCleanup(msNotUsed);
}

void SkiaGPUContext::GetResourceCacheLimits(int* maxResource, size_t* maxResourceBytes) const
{
    if (!grContext_) {
        LOGD("SkiaGPUContext::GetResourceCacheLimits, grContext_ is nullptr");
        return;
    }
    grContext_->getResourceCacheLimits(maxResource, maxResourceBytes);
}

void SkiaGPUContext::SetResourceCacheLimits(int maxResource, size_t maxResourceBytes)
{
    if (!grContext_) {
        LOGD("SkiaGPUContext::SetResourceCacheLimits, grContext_ is nullptr");
        return;
    }
    grContext_->setResourceCacheLimits(maxResource, maxResourceBytes);
}

void SkiaGPUContext::GetResourceCacheUsage(int* resourceCount, size_t* resourceBytes) const
{
    if (!grContext_) {
        LOGD("SkiaGPUContext::GetResourceCacheUsage, grContext_ is nullptr");
        return;
    }
    grContext_->getResourceCacheUsage(resourceCount, resourceBytes);
}

void SkiaGPUContext::FreeGpuResources()
{
    if (!grContext_) {
        LOGD("SkiaGPUContext::FreeGpuResources, grContext_ is nullptr");
        return;
    }
    grContext_->freeGpuResources();
}

void SkiaGPUContext::DumpGpuStats(std::string& out)
{
    if (!grContext_) {
        LOGD("SkiaGPUContext::DumpGpuStats, grContext_ is nullptr");
        return;
    }
    SkString stat;
    grContext_->priv().dumpGpuStats(&stat);
    out = stat.c_str();
}

void SkiaGPUContext::ReleaseResourcesAndAbandonContext()
{
    if (!grContext_) {
        LOGD("SkiaGPUContext::ReleaseResourcesAndAbandonContext, grContext_ is nullptr");
        return;
    }
    grContext_->releaseResourcesAndAbandonContext();
}

void SkiaGPUContext::PurgeUnlockedResources(bool scratchResourcesOnly)
{
    if (!grContext_) {
        LOGD("SkiaGPUContext::PurgeUnlockedResources, grContext_ is nullptr");
        return;
    }
    grContext_->purgeUnlockedResources(scratchResourcesOnly);
}

void SkiaGPUContext::PurgeUnlockedResourcesByTag(bool scratchResourcesOnly, const GPUResourceTag &tag)
{
    if (!grContext_) {
        LOGD("SkiaGPUContext::PurgeUnlockedResourcesByTag, grContext_ is nullptr");
        return;
    }
    GrGpuResourceTag grTag(tag.fPid, tag.fTid, tag.fWid, tag.fFid);
    grContext_->purgeUnlockedResourcesByTag(scratchResourcesOnly, grTag);
}

void SkiaGPUContext::PurgeUnlockedResourcesByPid(bool scratchResourcesOnly, const std::set<pid_t>& exitedPidSet)
{
    if (!grContext_) {
        LOGD("SkiaGPUContext::PurgeUnlockedResourcesByPid, grContext_ is nullptr");
        return;
    }
    grContext_->purgeUnlockedResourcesByPid(scratchResourcesOnly, exitedPidSet);
}

void SkiaGPUContext::PurgeUnlockAndSafeCacheGpuResources()
{
    if (!grContext_) {
        LOGD("SkiaGPUContext::PurgeUnlockAndSafeCacheGpuResources, grContext_ is nullptr");
        return;
    }
    grContext_->purgeUnlockAndSafeCacheGpuResources();
}

void SkiaGPUContext::ReleaseByTag(const GPUResourceTag &tag)
{
    if (!grContext_) {
        LOGD("SkiaGPUContext::ReleaseByTag, grContext_ is nullptr");
        return;
    }
    GrGpuResourceTag grTag(tag.fPid, tag.fTid, tag.fWid, tag.fFid);
    grContext_->releaseByTag(grTag);
}

void SkiaGPUContext::ResetContext()
{
    if (!grContext_) {
        LOGD("SkiaGPUContext::ResetContext, grContext_ is nullptr");
        return;
    }
    grContext_->resetContext();
}

void SkiaGPUContext::DumpMemoryStatisticsByTag(TraceMemoryDump* traceMemoryDump, GPUResourceTag &tag)
{
    if (!grContext_) {
        LOGD("SkiaGPUContext::DumpMemoryStatisticsByTag, grContext_ is nullptr");
        return;
    }

    if (!traceMemoryDump) {
        LOGD("SkiaGPUContext::DumpMemoryStatisticsByTag, traceMemoryDump is nullptr");
        return;
    }
    SkTraceMemoryDump* skTraceMemoryDump = traceMemoryDump->GetImpl<SkiaTraceMemoryDump>()->GetTraceMemoryDump().get();
    GrGpuResourceTag grTag(tag.fPid, tag.fTid, tag.fWid, tag.fFid);
    grContext_->dumpMemoryStatisticsByTag(skTraceMemoryDump, grTag);
}

void SkiaGPUContext::DumpMemoryStatistics(TraceMemoryDump* traceMemoryDump)
{
    if (!grContext_) {
        LOGD("SkiaGPUContext::DumpMemoryStatistics, grContext_ is nullptr");
        return;
    }

    if (!traceMemoryDump) {
        LOGD("SkiaGPUContext::DumpMemoryStatistics, traceMemoryDump is nullptr");
        return;
    }
    SkTraceMemoryDump* skTraceMemoryDump = traceMemoryDump->GetImpl<SkiaTraceMemoryDump>()->GetTraceMemoryDump().get();
    grContext_->dumpMemoryStatistics(skTraceMemoryDump);
}

void SkiaGPUContext::SetCurrentGpuResourceTag(const GPUResourceTag &tag)
{
    if (!grContext_) {
        LOGD("SkiaGPUContext::ReleaseByTag, grContext_ is nullptr");
        return;
    }
    GrGpuResourceTag grTag(tag.fPid, tag.fTid, tag.fWid, tag.fFid);
    grContext_->setCurrentGrResourceTag(grTag);
}

#ifdef NEW_SKIA
sk_sp<GrDirectContext> SkiaGPUContext::GetGrContext() const
#else
sk_sp<GrContext> SkiaGPUContext::GetGrContext() const
#endif
{
    return grContext_;
}
#ifdef NEW_SKIA
void SkiaGPUContext::SetGrContext(const sk_sp<GrDirectContext>& grContext)
#else
void SkiaGPUContext::SetGrContext(const sk_sp<GrContext>& grContext)
#endif
{
    grContext_ = grContext;
}

#ifdef RS_ENABLE_VK
void SkiaGPUContext::StoreVkPipelineCacheData()
{
    if (!grContext_) {
        LOGD("SkiaGPUContext::StoreVkPipelineCacheData, grContext_ is nullptr");
        return;
    }
    grContext_->storeVkPipelineCacheData();
}
#endif

std::unordered_map<uintptr_t, std::function<void(const std::function<void()>& task)>>
    SkiaGPUContext::contextPostMap_ = {};

void SkiaGPUContext::RegisterPostFunc(const std::function<void(const std::function<void()>& task)>& func)
{
    if (grContext_ != nullptr) {
        contextPostMap_[uintptr_t(grContext_.get())] = func;
    }
}

std::function<void(const std::function<void()>& task)> SkiaGPUContext::GetPostFunc(sk_sp<GrDirectContext> grContext)
{
    if (grContext != nullptr && contextPostMap_.count(uintptr_t(grContext.get())) > 0) {
        return contextPostMap_[uintptr_t(grContext.get())];
    }
    return nullptr;
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
