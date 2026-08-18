/*
 * Copyright (c) 2023-2026 Huawei Device Co., Ltd.
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

#ifndef RS_VULKAN_CONTEXT_REFACTORED_H
#define RS_VULKAN_CONTEXT_REFACTORED_H

#include <atomic>
#include <memory>
#include <mutex>

#include "include/gpu/vk/VulkanBackendContext.h"
#include "include/gpu/vk/VulkanExtensions.h"

#include "image/gpu_context.h"
#include "rs_vulkan_interface.h"
#include "render_context/memory_handler.h"

namespace OHOS {
namespace Rosen {
class MemoryHandler;
class RsVulkanContext {
public:
    static RsVulkanContext& Get(RenderEngineType type = RenderEngineType::BASIC_RENDER);

    RsVulkanContext(bool isProtected, bool isHtsEnable, RenderEngineType type);
    ~RsVulkanContext();

    // 禁止复制和移动
    RsVulkanContext(const RsVulkanContext&) = delete;
    RsVulkanContext& operator=(const RsVulkanContext&) = delete;
    RsVulkanContext(RsVulkanContext&&) = delete;
    RsVulkanContext& operator=(RsVulkanContext&&) = delete;

    // 状态查询
    bool IsValid() const
    {
        return vulkanInterface_ && vulkanInterface_->IsValid();
    }
    RenderEngineType GetType() const
    {
        return type_;
    }

    // Vulkan对象获取
    std::shared_ptr<RsVulkanInterface>& GetRsVulkanInterface()
    {
        return vulkanInterface_;
    }

    inline const skgpu::VulkanBackendContext& GetGrVkBackendContext() const noexcept
    {
        return vulkanInterface_->backendContext_;
    }

    // Semaphore管理
    VkSemaphore RequireSemaphore()
    {
        return vulkanInterface_->RequireSemaphore();
    }
    void SendSemaphoreWithFd(VkSemaphore semaphore, int fenceFd)
    {
        vulkanInterface_->SendSemaphoreWithFd(semaphore, fenceFd);
    }

    bool QueryMaxGpuBufferSize(uint32_t& maxWidth, uint32_t& maxHeight);

    // gpuContext管理
    std::shared_ptr<Drawing::GPUContext> CreateDrawingGPUContext(const std::string& cacheDir = "");
    void ReleaseDrawingGPUContext(std::shared_ptr<Drawing::GPUContext>& gpuContext);
private:
    std::shared_ptr<RsVulkanInterface> vulkanInterface_;
    RenderEngineType type_ = RenderEngineType::BASIC_RENDER;
};
} // namespace Rosen
} // namespace OHOS

#endif
