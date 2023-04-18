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

namespace OHOS {
namespace Rosen {
namespace Drawing {
GPUContext::GPUContext() : impl_(ImplFactory::CreateGPUContextImpl()) {}

bool GPUContext::BuildFromGL(const GPUContextOptions& options)
{
    return (impl_ == nullptr) ? false : impl_->BuildFromGL(options);
}

void GPUContext::GetResourceCacheLimits(int& maxResource, size_t& maxResourceBytes) const
{
    if (impl_ == nullptr) {
        return;
    }
    impl_->GetResourceCacheLimits(maxResource, maxResourceBytes);
}

void GPUContext::SetResourceCacheLimits(int maxResource, size_t maxResourceBytes)
{
    if (impl_ == nullptr) {
        return;
    }
    impl_->SetResourceCacheLimits(maxResource, maxResourceBytes);
}

void GPUContext::Flush()
{
    if (impl_ == nullptr) {
        return;
    }
    impl_->Flush();
}

void GPUContext::PerformDeferredCleanup(std::chrono::milliseconds msNotUsed)
{
    if (impl_ == nullptr) {
        return;
    }
    impl_->PerformDeferredCleanup(msNotUsed);
}

void GPUContextOptions::SetPersistentCache(PersistentCache* persistentCache)
{
    persistentCache_ = persistentCache;
}

GPUContextOptions::PersistentCache* GPUContextOptions::GetPersistentCache() const
{
    return persistentCache_;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
