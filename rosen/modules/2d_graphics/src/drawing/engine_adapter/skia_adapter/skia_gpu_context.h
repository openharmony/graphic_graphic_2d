/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.. All rights reserved.
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

#ifndef SKIA_GPUCONTEXT_H
#define SKIA_GPUCONTEXT_H

#include "include/gpu/GrContext.h"
#include "include/gpu/GrContextOptions.h"

#include "impl_interface/gpu_context_impl.h"
#include "image/gpu_context.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaPersistentCache : public GrContextOptions::PersistentCache {
public:
    SkiaPersistentCache(GPUContextOptions::PersistentCache* cache);
    ~SkiaPersistentCache() {}

    sk_sp<SkData> load(const SkData& key) override;
    void store(const SkData& key, const SkData& data) override;
private:
    GPUContextOptions::PersistentCache* cache_;
};

class SkiaGPUContext : public GPUContextImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;
    SkiaGPUContext();
    ~SkiaGPUContext() override {};
    bool BuildFromGL(const GPUContextOptions& options) override;

    void Flush() override;
    void PerformDeferredCleanup(std::chrono::milliseconds msNotUsed) override;

    void GetResourceCacheLimits(int& maxResource, size_t& maxResourceBytes) const override;
    void SetResourceCacheLimits(int maxResource, size_t maxResourceBytes) override;

    sk_sp<GrContext> GetGrContext() const;
    void SetGrContext(const sk_sp<GrContext>& grContext);

private:
    sk_sp<GrContext> grContext_;
    std::shared_ptr<SkiaPersistentCache> skiaPersistentCache_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif // SKIA_GPUCONTEXT_H
