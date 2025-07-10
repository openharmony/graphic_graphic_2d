/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "drawing_gpu_context_manager.h"

#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif

#include "utils/log.h"
#include "utils/system_properties.h"

namespace OHOS {
namespace Rosen {

DrawingGpuContextManager::DrawingGpuContextManager()
{
    if (Drawing::SystemProperties::IsUseGl()) {
        renderContext_ = std::make_shared<RenderContext>();
    }
}
std::shared_ptr<RenderContext> DrawingGpuContextManager::GetRenderContext()
{
    return renderContext_;
}

DrawingGpuContextManager& DrawingGpuContextManager::GetInstance()
{
    static DrawingGpuContextManager instance;
    return instance;
}

std::shared_ptr<Drawing::GPUContext> DrawingGpuContextManager::CreateDrawingContext()
{
#ifdef RS_ENABLE_GPU
    std::shared_ptr<Drawing::GPUContext> context = nullptr;
    if (Drawing::SystemProperties::IsUseGl()) {
        if (renderContext_ == nullptr) {
            LOGE("CreateDrawingContext: create renderContext failed.");
            return nullptr;
        }

        renderContext_->InitializeEglContext();
        renderContext_->SetUpGpuContext();
        context = renderContext_->GetSharedDrGPUContext();
    }
#ifdef RS_ENABLE_VK
    if (Drawing::SystemProperties::IsUseVulkan()) {
        context = RsVulkanContext::GetSingleton().CreateDrawingContext();
    }
#endif

    return context;
#endif
    return nullptr;
}

void DrawingGpuContextManager::Insert(void* key, std::shared_ptr<Drawing::GPUContext> gpuContext)
{
    std::unique_lock lock(mutex_);
    gpuContext_.insert({key, std::move(gpuContext)});
}

std::shared_ptr<Drawing::GPUContext> DrawingGpuContextManager::Find(void* key)
{
    std::unique_lock lock(mutex_);
    auto iter = gpuContext_.find(key);
    if (iter != gpuContext_.end()) {
        return iter->second;
    } else {
        return {nullptr};
    }
}

bool DrawingGpuContextManager::Remove(void* key)
{
    std::unique_lock lock(mutex_);
    auto iter = gpuContext_.find(key);
    if (iter != gpuContext_.end()) {
        if (iter->second != nullptr) {
            iter->second->PurgeUnlockedResources(false);
        }
        gpuContext_.erase(iter);
        return true;
    } else {
        return false;
    }
}
} // namespace Rosen
} // namespace OHOS