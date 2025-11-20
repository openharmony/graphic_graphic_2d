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

#include "render_context_vk.h"

#include "render_context/render_context_log.h"
#include "platform/ohos/backend/native_buffer_utils.h"
#include "platform/ohos/backend/rs_vulkan_context.h"

#ifdef USE_M133_SKIA
#include "include/gpu/ganesh/vk/GrVkBackendSemaphore.h"
#endif

namespace OHOS {
namespace Rosen {

RenderContextVK::~RenderContextVK()
{
    AbandonContext();
}

bool RenderContextVK::Init()
{
    return true;
}

bool RenderContextVK::AbandonContext()
{
    if (drGPUContext_ == nullptr) {
        LOGD("grContext is nullptr.");
        return true;
    }
    drGPUContext_->FlushAndSubmit(true);
    drGPUContext_->PurgeUnlockAndSafeCacheGpuResources();
    return true;
}

bool RenderContextVK::SetUpGpuContext(std::shared_ptr<Drawing::GPUContext> drawingContext)
{
    if (drGPUContext_ != nullptr) {
        LOGD("Drawing GPUContext has already created!!");
        return true;
    }
    if (drawingContext == nullptr) {
        drawingContext = RsVulkanContext::GetSingleton(cacheDir_).CreateDrawingContext();
        if (drawingContext == nullptr) {
            LOGE("Fail to create vulkan GPUContext");
            return false;
        }
    }
    std::shared_ptr<Drawing::GPUContext> drGPUContext(drawingContext);
    drGPUContext_ = std::move(drGPUContext);
    return true;
}

std::string RenderContextVK::GetShaderCacheSize() const
{
    if (RsVulkanContext::GetSingleton().GetMemoryHandler()) {
        return RsVulkanContext::GetSingleton().GetMemoryHandler()->QuerryShader();
    }
    LOGD("GetShaderCacheSize no shader cache");
    return "";
}

std::string RenderContextVK::CleanAllShaderCache() const
{
    if (RsVulkanContext::GetSingleton().GetMemoryHandler()) {
        return RsVulkanContext::GetSingleton().GetMemoryHandler()->ClearShader();
    }
    LOGD("CleanAllShaderCache no shader cache");
    return "";
}
} // namespace Rosen
} // namespace OHOS
