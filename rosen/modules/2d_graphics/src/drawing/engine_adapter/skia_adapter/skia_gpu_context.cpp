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
#include "include/gpu/gl/GrGLInterface.h"
#include "src/gpu/GrDirectContextPriv.h"
#include "include/core/SkTypes.h"

#include "skia_data.h"
#include "utils/data.h"
#include "utils/log.h"
#include "skia_trace_memory_dump.h"
#include "utils/system_properties.h"
#include "skia_task_executor.h"
#ifdef ROSEN_OHOS
#include "parameters.h"
#endif

namespace OHOS {
namespace Rosen {
namespace Drawing {
static std::mutex g_registarMutex;
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

class CommonPoolExecutor : public SkExecutor {
public:
    void add(std::function<void(void)> func) override
    {
        TaskPoolExecutor::PostTask(std::move(func));
    }
};

static CommonPoolExecutor g_defaultExecutor;

bool SkiaGPUContext::BuildFromGL(const GPUContextOptions& options)
{
    sk_sp<const GrGLInterface> glInterface(GrGLCreateNativeInterface());
    if (options.GetPersistentCache() != nullptr) {
        skiaPersistentCache_ = std::make_shared<SkiaPersistentCache>(options.GetPersistentCache());
    }

    GrContextOptions grOptions;
    grOptions.fGpuPathRenderers &= ~GpuPathRenderers::kCoverageCounting;
    // fix svg antialiasing bug
    grOptions.fGpuPathRenderers &= ~GpuPathRenderers::kAtlas;
    grOptions.fPreferExternalImagesOverES3 = true;
    grOptions.fDisableDistanceFieldPaths = true;
    grOptions.fAllowPathMaskCaching = options.GetAllowPathMaskCaching();
    grOptions.fPersistentCache = skiaPersistentCache_.get();
    grOptions.fExecutor = &g_defaultExecutor;
#ifdef SKIA_OHOS
    grOptions.clearSmallTexture = options.GetIsUniRender();
#endif
    grContext_ = GrDirectContext::MakeGL(std::move(glInterface), grOptions);
    return grContext_ != nullptr ? true : false;
}

#ifdef RS_ENABLE_VK
bool SkiaGPUContext::BuildFromVK(const GrVkBackendContext& context)
{
    if (!SystemProperties::IsUseVulkan()) {
        return false;
    }
    GrContextOptions grOptions;
    grOptions.fExecutor = &g_defaultExecutor;
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
    GrContextOptions grOptions;
    grOptions.fGpuPathRenderers &= ~GpuPathRenderers::kCoverageCounting;
    // fix svg antialiasing bug
    grOptions.fGpuPathRenderers &= ~GpuPathRenderers::kAtlas;
    grOptions.fPreferExternalImagesOverES3 = true;
    grOptions.fDisableDistanceFieldPaths = true;
    grOptions.fAllowPathMaskCaching = options.GetAllowPathMaskCaching();
    grOptions.fPersistentCache = skiaPersistentCache_.get();
    grOptions.fExecutor = &g_defaultExecutor;
#ifdef ROSEN_OHOS
    grOptions.fRuntimeProgramCacheSize =
        std::atoi(OHOS::system::GetParameter("persist.sys.graphics.skiapipelinelimit", "512").c_str());
#endif
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

void SkiaGPUContext::SetPurgeableResourceLimit(int purgeableMaxCount)
{
#ifdef SKIA_OHOS
    if (!grContext_) {
        LOGD("SkiaGPUContext::SetPurgeableResourceLimit, grContext_ is nullptr");
        return;
    }
    grContext_->setPurgeableResourceLimit(purgeableMaxCount);
#else
    static_cast<void>(purgeableMaxCount);
    LOGD("SkiaGPUContext::SetPurgeableResourceLimit, unsupported");
#endif
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

void SkiaGPUContext::ReclaimResources()
{
    //Skia Not Implement ReclaimResources.
}

void SkiaGPUContext::DumpGpuStats(std::string& out)
{
    if (!grContext_) {
        LOGD("SkiaGPUContext::DumpGpuStats, grContext_ is nullptr");
        return;
    }
    SkString stat;
    grContext_->priv().dumpGpuStats(&stat);
    grContext_->dumpVmaStats(&stat);
    out = stat.c_str();
}

void SkiaGPUContext::DumpAllResource(std::stringstream& dump)
{
    if (!grContext_) {
        LOGD("SkiaGPUContext::DumpAllResource, grContext_ is nullptr");
        return;
    }
    grContext_->dumpAllResource(dump);
}

void SkiaGPUContext::DumpAllCoreTrace(std::stringstream& dump)
{
    if (!grContext_) {
        LOGD("SkiaGPUContext::DumpAllCoreTrace, grContext_ is nullptr");
        return;
    }
    grContext_->dumpAllCoreTrace(dump);
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
    GrGpuResourceTag grTag(tag.fPid, tag.fTid, tag.fWid, tag.fFid, tag.fSid, tag.fName);
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

void SkiaGPUContext::RegisterVulkanErrorCallback(const std::function<void()>& vulkanErrorCallback)
{
    if (!grContext_) {
        LOGD("SkiaGPUContext::RegisterVulkanErrorCallback, grContext_ is nullptr");
        return;
    }
    grContext_->registerVulkanErrorCallback(vulkanErrorCallback);
}

void SkiaGPUContext::PurgeUnlockAndSafeCacheGpuResources()
{
    if (!grContext_) {
        LOGD("SkiaGPUContext::PurgeUnlockAndSafeCacheGpuResources, grContext_ is nullptr");
        return;
    }
    grContext_->purgeUnlockAndSafeCacheGpuResources();
}

void SkiaGPUContext::PurgeCacheBetweenFrames(bool scratchResourcesOnly, const std::set<pid_t>& exitedPidSet,
    const std::set<pid_t>& protectedPidSet)
{
    if (!grContext_) {
        LOGD("SkiaGPUContext::PurgeCacheBetweenFrames,grContext_ is nullptr");
        return;
    }
    grContext_->purgeCacheBetweenFrames(scratchResourcesOnly, exitedPidSet, protectedPidSet);
}

void SkiaGPUContext::ReleaseByTag(const GPUResourceTag &tag)
{
    if (!grContext_) {
        LOGD("SkiaGPUContext::ReleaseByTag, grContext_ is nullptr");
        return;
    }
    GrGpuResourceTag grTag(tag.fPid, tag.fTid, tag.fWid, tag.fFid, tag.fSid, tag.fName);
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
    if (!skTraceMemoryDump) {
        LOGD("SkiaGPUContext::DumpMemoryStatisticsByTag, sktraceMemoryDump is nullptr");
        return;
    }
    GrGpuResourceTag grTag(tag.fPid, tag.fTid, tag.fWid, tag.fFid, tag.fSid, tag.fName);
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
    if (!skTraceMemoryDump) {
        LOGD("SkiaGPUContext::DumpMemoryStatistics, sktraceMemoryDump is nullptr");
        return;
    }
    grContext_->dumpMemoryStatistics(skTraceMemoryDump);
}

void SkiaGPUContext::SetCurrentGpuResourceTag(const GPUResourceTag &tag)
{
    if (!grContext_) {
        LOGD("SkiaGPUContext::SetCurrentGpuResourceTag, grContext_ is nullptr");
        return;
    }
    GrGpuResourceTag grTag(tag.fPid, tag.fTid, tag.fWid, tag.fFid, tag.fSid, tag.fName);
    grContext_->setCurrentGrResourceTag(grTag);
}

GPUResourceTag SkiaGPUContext::GetCurrentGpuResourceTag() const
{
    if (!grContext_) {
        LOGD("SkiaGPUContext::GetCurrentGpuResourceTag, grContext_ is nullptr");
        return {};
    }
    GrGpuResourceTag grTag = grContext_->getCurrentGrResourceTag();
    GPUResourceTag tag(grTag.fPid, grTag.fTid, grTag.fWid, grTag.fFid, grTag.fName);
    return tag;
}

sk_sp<GrDirectContext> SkiaGPUContext::GetGrContext() const
{
    return grContext_;
}

void SkiaGPUContext::SetGrContext(const sk_sp<GrDirectContext>& grContext)
{
    grContext_ = grContext;
}

void SkiaGPUContext::GetUpdatedMemoryMap(std::unordered_map<pid_t, size_t> &out)
{
    if (!grContext_) {
        LOGD("SkiaGPUContext::GetUpdatedMemoryMap, grContext_ is nullptr");
        return;
    }
    grContext_->getUpdatedMemoryMap(out);
}

void SkiaGPUContext::InitGpuMemoryLimit(MemoryOverflowCalllback callback, uint64_t size)
{
    if (!grContext_) {
        LOGD("SkiaGPUContext::InitGpuMemoryLimit, grContext_ is nullptr");
        return;
    }
    grContext_->initGpuMemoryLimit(callback, size);
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

void SkiaGPUContext::BeginFrame()
{
    if (!grContext_) {
        LOGD("SkiaGPUContext::BeginFrame, grContext_ is nullptr");
        return;
    }
    grContext_->beginFrame();
}

void SkiaGPUContext::EndFrame()
{
    if (!grContext_) {
        LOGD("SkiaGPUContext::EndFrame, grContext_ is nullptr");
        return;
    }
    grContext_->endFrame();
}

void SkiaGPUContext::SetGpuCacheSuppressWindowSwitch(bool enabled)
{
    if (!grContext_) {
        LOGD("SkiaGPUContext::SetGpuCacheSuppressWindowSwitch, grContext_ is nullptr");
        return;
    }
    grContext_->setGpuCacheSuppressWindowSwitch(enabled);
}

void SkiaGPUContext::SetGpuMemoryAsyncReclaimerSwitch(bool enabled, const std::function<void()>& setThreadPriority)
{
    if (!grContext_) {
        LOGD("SkiaGPUContext::SetGpuMemoryAsyncReclaimerSwitch, grContext_ is nullptr");
        return;
    }
    grContext_->setGpuMemoryAsyncReclaimerSwitch(enabled, setThreadPriority);
}

void SkiaGPUContext::FlushGpuMemoryInWaitQueue()
{
    if (!grContext_) {
        LOGD("SkiaGPUContext::FlushGpuMemoryInWaitQueue, grContext_ is nullptr");
        return;
    }
    grContext_->flushGpuMemoryInWaitQueue();
}

void SkiaGPUContext::SuppressGpuCacheBelowCertainRatio(const std::function<bool(void)>& nextFrameHasArrived)
{
    if (!grContext_) {
        LOGD("SkiaGPUContext::SuppressGpuCacheBelowCertainRatio, grContext_ is nullptr");
        return;
    }
    grContext_->suppressGpuCacheBelowCertainRatio(nextFrameHasArrived);
}

std::unordered_map<uintptr_t, std::function<void(const std::function<void()>& task)>>
    SkiaGPUContext::contextPostMap_ = {};

void SkiaGPUContext::RegisterPostFunc(const std::function<void(const std::function<void()>& task)>& func)
{
    std::unique_lock lock(g_registarMutex);
    if (grContext_ != nullptr) {
        contextPostMap_[uintptr_t(grContext_.get())] = func;
    }
}

std::function<void(const std::function<void()>& task)> SkiaGPUContext::GetPostFunc(sk_sp<GrDirectContext> grContext)
{
    std::unique_lock lock(g_registarMutex);
    if (grContext != nullptr && contextPostMap_.count(uintptr_t(grContext.get())) > 0) {
        return contextPostMap_[uintptr_t(grContext.get())];
    }
    return nullptr;
}

void SkiaGPUContext::VmaDefragment()
{
    if (grContext_ != nullptr) {
        grContext_->vmaDefragment();
    }
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
