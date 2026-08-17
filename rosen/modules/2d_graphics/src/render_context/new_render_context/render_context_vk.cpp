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
    drGPUContext_ = nullptr;
}

bool RenderContextVK::Init(RenderEngineType type, const std::string& cacheDir)
{
    contextType_ = type;
    return SetUpGpuContext(cacheDir);
}

bool RenderContextVK::AbandonContext()
{
    if (drGPUContext_ == nullptr) {
        LOGD("grContext is nullptr.");
        return false;
    }
    drGPUContext_->FlushAndSubmit(true);
    drGPUContext_->PurgeUnlockAndSafeCacheGpuResources();
    return true;
}

bool RenderContextVK::SetUpGpuContext(const std::string& cacheDir)
{
    if (drGPUContext_ != nullptr) {
        LOGD("Drawing GPUContext has already created!!");
        return true;
    }
    drGPUContext_ = CreateDrawingGPUContext(cacheDir);
    return true;
}

bool RenderContextVK::QueryMaxGpuBufferSize(uint32_t& maxWidth, uint32_t& maxHeight)
{
    return RsVulkanContext::Get(contextType_).QueryMaxGpuBufferSize(maxWidth, maxHeight);
}

std::shared_ptr<Drawing::GPUContext> RenderContextVK::CreateDrawingGPUContext(const std::string& cacheDir)
{
    return RsVulkanContext::Get(contextType_).CreateDrawingGPUContext(cacheDir);
}

void RenderContextVK::ReleaseDrawingGPUContext(std::shared_ptr<Drawing::GPUContext> gpuContext)
{
    return RsVulkanContext::Get(contextType_).ReleaseDrawingGPUContext(gpuContext);
}
} // namespace Rosen
} // namespace OHOS