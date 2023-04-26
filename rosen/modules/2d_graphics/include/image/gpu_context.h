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

#include "impl_interface/gpu_context_impl.h"
#include "utils/data.h"

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

/*
 * @brief  Option to create a GPUContext. Currently only supports setting persistent cache,
           other options may be expanded in the future
 */
class GPUContextOptions {
public:
    /*
     * @brief  Cache compiled shaders for use between sessions.
     */
    class PersistentCache {
    public:
        /*
         * @brief  Returns the data for the key if it exists in the cache.
         */
        virtual std::shared_ptr<Data> Load(const Data& key) = 0;

        /*
         * @brief  Stores the data and key.
         */
        virtual void Store(const Data& key, const Data& data) = 0;
    };

    /*
     * @brief  Gets persistent cache object.
     */
    PersistentCache* GetPersistentCache() const;
    /*
     * @brief                  Sets persistent cache object.
     * @param persistentCache  A pointer to persistent cache object.
     */
    void SetPersistentCache(PersistentCache* persistentCache);

private:
    PersistentCache* persistentCache_ = nullptr;
};

class GPUContext {
public:
    GPUContext();
    ~GPUContext() {}

    /*
     * @brief           Creates a GL GPUContext for a backend context.
     * @param options   Option to create a GL GPUContext.
     */
    bool BuildFromGL(const GPUContextOptions& options);

    /*
     * @brief   Call to ensure all drawing to the context has been flushed to underlying 3D API specific objects.
     */
    void Flush();

    /*
     * @brief             Purge GPU resources that haven't been used in the past 'msNotUsed' milliseconds or are otherwise
                          marked for deletion.
     * @param msNotUsed   Only unlocked resources not used in these last milliseconds will be cleaned up.
     */
    void PerformDeferredCleanup(std::chrono::milliseconds msNotUsed);

    /*
     * @brief                   Gets the current GPU resource cache limits.
     * @param maxResource       If non-null, returns maximum number of resources that can be held in the cache.
     * @param maxResourceBytes  If non-null, returns maximum number of bytes of video memory that can be held in the cache.
     */
    void GetResourceCacheLimits(int& maxResource, size_t& maxResourceBytes) const;

    /*
     * @brief                   Specify the GPU resource cache limits.
     * @param maxResource       The maximum number of resources that can be held in the cache.
     * @param maxResourceBytes  The maximum number of bytes of video memory that can be held in the cache.
     */
    void SetResourceCacheLimits(int maxResource, size_t maxResourceBytes);

    /*
     * @brief   Get the adaptation layer instance, called in the adaptation layer.
     * @return  Adaptation Layer instance.
     */
    template<typename T>
    const std::shared_ptr<T> GetImpl() const
    {
        return (impl_ == nullptr) ? nullptr : impl_->DowncastingTo<T>();
    }
private:
    std::shared_ptr<GPUContextImpl> impl_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif // !GPU_CONTEXT_H
