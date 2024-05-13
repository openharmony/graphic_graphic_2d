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

#ifndef GPU_CONTEXT_H
#define GPU_CONTEXT_H
#include <functional>
#include <set>

#include "trace_memory_dump.h"

#include "impl_interface/gpu_context_impl.h"
#include "utils/data.h"
#include "utils/drawing_macros.h"

typedef void* EGLContext;
namespace OHOS {
namespace Rosen {
namespace Drawing {
enum class PathRenderers : uint32_t {
    NONE              = 0,
    DASHLINE          = 1 << 0,
    STENCILANDCOVER   = 1 << 1,
    COVERAGECOUNTING  = 1 << 2,
    AAHAIRLINE        = 1 << 3,
    AACONVEX          = 1 << 4,
    AALINEARIZING     = 1 << 5,
    SMALL             = 1 << 6,
    TESSELLATING      = 1 << 7,

    ALL               = (TESSELLATING | (TESSELLATING - 1)),
    DEFAULT           = ALL & ~COVERAGECOUNTING
};

struct GPUResourceTag {
    GPUResourceTag()
        : fPid(0), fTid(0), fWid(0), fFid(0) {}
    GPUResourceTag(uint32_t pid, uint32_t tid, uint32_t wid, uint32_t fid)
        : fPid(pid), fTid(tid), fWid(wid), fFid(fid) {}
    uint32_t fPid;
    uint32_t fTid;
    uint32_t fWid;
    uint32_t fFid;
};

/**
 * @brief Option to create a GPUContext. Currently only supports setting persistent cache,
 * other options may be expanded in the future
 */
class DRAWING_API GPUContextOptions {
public:
    /**
     * @brief Cache compiled shaders for use between sessions.
     */
    class PersistentCache {
    public:
        PersistentCache() = default;
        virtual ~PersistentCache() = default;

        /**
         * @brief Returns the data for the key if it exists in the cache.
         */
        virtual std::shared_ptr<Data> Load(const Data& key) = 0;

        /**
         * @brief Stores the data and key.
         */
        virtual void Store(const Data& key, const Data& data) = 0;
    };

    /**
     * @brief Gets persistent cache object.
     */
    PersistentCache* GetPersistentCache() const;

    /**
     * @brief Sets persistent cache object.
     * @param persistentCache A pointer to persistent cache object.
     */
    void SetPersistentCache(PersistentCache* persistentCache);

    void SetAllowPathMaskCaching(bool allowPathMaskCaching);
    bool GetAllowPathMaskCaching() const;

private:
    PersistentCache* persistentCache_ = nullptr;
    bool allowPathMaskCaching_ = true;
};

class DRAWING_API GPUContext {
public:
    GPUContext();
    ~GPUContext() {}

    /**
     * @brief           Creates a GL GPUContext for a backend context.
     * @param options   Option to create a GL GPUContext.
     */
    bool BuildFromGL(const GPUContextOptions& options);

#ifdef RS_ENABLE_VK
    bool BuildFromVK(const GrVkBackendContext& context);

    /**
     * @brief           Creates a VK GPUContext for a backend context.
     * @param context   An existed VK Context used to create a VK GPUContext.
     * @param options   Option to create a VK GPUContext.
     */
    bool BuildFromVK(const GrVkBackendContext& context, const GPUContextOptions& options);
#endif

    /**
     * @brief   Call to ensure all drawing to the context has been flushed to underlying 3D API specific objects.
     */
    void Flush();

    /**
     * @brief   Call to ensure all drawing to the context has been submitted to underlying 3D API.
     */
    void Submit();

    /**
     * @brief           Call to ensure all drawing to the context has been flushed and submitted to underlying 3D API.
     * @param syncCpu   Whether to sync CPU or not.
     */
    void FlushAndSubmit(bool syncCpu = false);

    /**
     * @brief             Purge GPU resources that haven't been used in the past 'msNotUsed' milliseconds
                          or are otherwise marked for deletion.
     * @param msNotUsed   Only unlocked resources not used in these last milliseconds will be cleaned up.
     */
    void PerformDeferredCleanup(std::chrono::milliseconds msNotUsed);

    /**
     * @brief                   Gets the current GPU resource cache limits.
     * @param maxResource       If non-null, returns maximum number of resources that can be held in the cache.
     * @param maxResourceBytes  If non-null, returns maximum number of bytes of video memory
                                that can be held in the cache.
     */
    void GetResourceCacheLimits(int* maxResource, size_t* maxResourceBytes) const;

    /**
     * @brief                   Specify the GPU resource cache limits.
     * @param maxResource       The maximum number of resources that can be held in the cache.
     * @param maxResourceBytes  The maximum number of bytes of video memory that can be held in the cache.
     */
    void SetResourceCacheLimits(int maxResource, size_t maxResourceBytes);

    /**
     * @brief                   Gets the current GPU resource cache usage.
     * @param resourceCount     If non-null, returns the number of resources that are held in the cache.
     * @param resourceBytes     If non-null, returns the total number of bytes of video memory held in the cache.
     */
    void GetResourceCacheUsage(int* resourceCount, size_t* resourceBytes) const;

    /**
     * @brief                   Free GPU created by the contetx.
     */
    void FreeGpuResources();

    /**
     * @brief                   Dump GPU stats.
     * @param out               Dump GPU stat string.
     */
    void DumpGpuStats(std::string& out) const;

    /**
     * @brief                   After returning it will assume that the underlying context may no longer be valid.
     */
    void ReleaseResourcesAndAbandonContext();

    /**
     * @brief                         Purge unlocked resources from the cache until
     *                                the provided byte count has been reached or we have purged all unlocked resources.
     * @param scratchResourcesOnly    Whether to scratch the resources only or not.
     */
    void PurgeUnlockedResources(bool scratchResourcesOnly);

    /**
     * @brief                         Purge unlocked resources by tag from the cache until
     *                                the provided byte count has been reached or we have purged all unlocked resources.
     * @param scratchResourcesOnly    Whether to scratch the resources only or not.
     * @param tag                     GPU resource tag used to purge unlocked resources.
     */
    void PurgeUnlockedResourcesByTag(bool scratchResourcesOnly, const GPUResourceTag &tag);

    /**
     * @brief                         Purge unlocked resources by pid from the cache until
     *                                the provided byte count has been reached or we have purged all unlocked resources.
     * @param scratchResourcesOnly    Whether to scratch the resources only or not.
     * @param exitedPidSet            GPU resource exitedPidSet used to purge unlocked resources.
     */
    void PurgeUnlockedResourcesByPid(bool scratchResourcesOnly, const std::set<pid_t>& exitedPidSet);

    /**
     * @brief                       Purge unlocked resources in every frame
     * @param scratchResourcesOnly  Whether to scratch the resources only or not.
     * @param exitedPidSet          GPU resource of exited PidSet used to purge unlocked resources.
     * @param protectedPidSet       GPU resource of protectedPidSet will not be purged.
     */
    void PurgeResourcesEveryFrame(bool scratchResourcesOnly, const std::set<pid_t>& exitedPidSet,
        const std::set<pid_t>& protectedPidSet);

    /**
     * @brief                   Purge unlocked resources from the safe cache until
     *                          the provided byte count has been reached or we have purged all unlocked resources.
     */
    void PurgeUnlockAndSafeCacheGpuResources();

    /**
     * @brief                   Releases GPUResource objects and removes them from the cache by tag.
     * @param tag               GPU resource tag used to release.
     */
    void ReleaseByTag(const GPUResourceTag &tag);

    /**
     * @brief                   Enumerates all cached GPU resources and dumps their memory to traceMemoryDump.
     * @param traceMemoryDump   A trace to memory dump.
     * @param tag               GPU resource tag used to dump memory statistics.
     */
    void DumpMemoryStatisticsByTag(TraceMemoryDump* traceMemoryDump, GPUResourceTag &tag) const;

    /**
     * @brief                   Enumerates all cached GPU resources and dumps their memory to traceMemoryDump.
     * @param traceMemoryDump   A trace to memory dump.
     */
    void DumpMemoryStatistics(TraceMemoryDump* traceMemoryDump) const;

    /**
     * @brief                   Reset GPU contect cache.
     */
    void ResetContext();

    /**
     * @brief                   Set current resource tag for gpu cache recycle.
     * @param tag               GPU resource tag used to set current GPU resource tag.
     */
    void SetCurrentGpuResourceTag(const GPUResourceTag &tag);

#ifdef RS_ENABLE_VK
    /**
     * @brief                   Store vulkan pipeline cache
     */
    void StoreVkPipelineCacheData();
#endif

    /**
     * @brief       Get the adaptation layer instance, called in the adaptation layer.
     * @param T     The name of Impl class.
     * @return      Adaptation Layer instance.
     */
    template<typename T>
    T* GetImpl() const
    {
        return impl_->DowncastingTo<T>();
    }

    void RegisterPostFunc(const std::function<void(const std::function<void()>& task)>& func);
private:
    std::shared_ptr<GPUContextImpl> impl_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif // !GPU_CONTEXT_H
