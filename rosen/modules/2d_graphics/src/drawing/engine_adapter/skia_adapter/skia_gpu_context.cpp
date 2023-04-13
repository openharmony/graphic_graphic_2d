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

#include "skia_data.h"
#include "utils/data.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
SkiaPersistentCache::SkiaPersistentCache(GPUContextOptions::PersistentCache* cache) : cache_(cache) {}

sk_sp<SkData> SkiaPersistentCache::load(const SkData& key)
{
    Data keyData;
    if (!cache_ || !keyData.GetImpl<SkiaData>()) {
        LOGE("SkiaPersistentCache::load, failed! cache or key invalid");
        return nullptr;
    }
    auto skiaKeyDataImpl = keyData.GetImpl<SkiaData>();
    skiaKeyDataImpl->SetSkData(sk_ref_sp(&key));

    auto retData = cache_->Load(keyData);
    if (!retData || !retData->GetImpl<SkiaData>()) {
        LOGE("SkiaPersistentCache::load, failed! load data invalid");
        return nullptr;
    }

    return retData->GetImpl<SkiaData>()->GetSkData();
}

void SkiaPersistentCache::store(const SkData& key, const SkData& data)
{
    Data keyData;
    Data storeData;
    if (!cache_ || !keyData.GetImpl<SkiaData>() || !storeData.GetImpl<SkiaData>()) {
        LOGE("SkiaPersistentCache::store, failed! cache or {key,data} invalid");
        return;
    }

    keyData.GetImpl<SkiaData>()->SetSkData(sk_ref_sp(&key));
    storeData.GetImpl<SkiaData>()->SetSkData(sk_ref_sp(&data));

    cache_->Store(keyData, storeData);
}

SkiaGPUContext::SkiaGPUContext() : grContext_(nullptr), skiaPersistentCache_(nullptr) {}

bool SkiaGPUContext::BuildFromGL(const GPUContextOptions& options)
{
    sk_sp<const GrGLInterface> glInterface(GrGLCreateNativeInterface());
    if (options.GetPersistentCache() != nullptr) {
        skiaPersistentCache_ = std::make_shared<SkiaPersistentCache>(options.GetPersistentCache());
    }

    GrContextOptions grOptions;
    grOptions.fGpuPathRenderers &= ~GpuPathRenderers::kCoverageCounting;
    grOptions.fPreferExternalImagesOverES3 = true;
    grOptions.fDisableDistanceFieldPaths = true;
    grOptions.fAllowPathMaskCaching = true;
    grOptions.fPersistentCache = skiaPersistentCache_.get();

    grContext_ = GrContext::MakeGL(std::move(glInterface), grOptions);
    if (grContext_) {
        return true;
    }
    return false;
}

void SkiaGPUContext::Flush()
{
    if (!grContext_) {
        LOGE("SkiaGPUContext::Flush, grContext_ is nullptr");
        return;
    }
    grContext_->flush();
}

void SkiaGPUContext::PerformDeferredCleanup(std::chrono::milliseconds msNotUsed)
{
    if (!grContext_) {
        LOGE("SkiaGPUContext::PerformDeferredCleanup, grContext_ is nullptr");
        return;
    }
    grContext_->performDeferredCleanup(msNotUsed);
}

void SkiaGPUContext::GetResourceCacheLimits(int& maxResource, size_t& maxResourceBytes) const
{
    if (!grContext_) {
        LOGE("SkiaGPUContext::GetResourceCacheLimits, grContext_ is nullptr");
        return;
    }
    grContext_->getResourceCacheLimits(&maxResource, &maxResourceBytes);
}

void SkiaGPUContext::SetResourceCacheLimits(int maxResource, size_t maxResourceBytes)
{
    if (!grContext_) {
        LOGE("SkiaGPUContext::SetResourceCacheLimits, grContext_ is nullptr");
        return;
    }
    grContext_->setResourceCacheLimits(maxResource, maxResourceBytes);
}

sk_sp<GrContext> SkiaGPUContext::GetGrContext() const
{
    return grContext_;
}

void SkiaGPUContext::SetGrContext(const sk_sp<GrContext>& grContext)
{
    grContext_ = grContext;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
