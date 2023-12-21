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

#ifndef RS_VULKAN_VULKAN_BACKBUFFER_H_
#define RS_VULKAN_VULKAN_BACKBUFFER_H_

#include <array>

#include "include/core/SkSize.h"
#include "include/core/SkSurface.h"
#include "vulkan_command_buffer.h"
#include "vulkan_handle.h"

namespace OHOS::Rosen::vulkan {

class RSVulkanBackbuffer {
public:
    RSVulkanBackbuffer(
        const RSVulkanProcTable& vk, const RSVulkanHandle<VkDevice>& device, const RSVulkanHandle<VkCommandPool>& pool);

    ~RSVulkanBackbuffer();

    bool IsValid() const;

    bool WaitFences();

    bool ResetFences();

    const RSVulkanHandle<VkFence>& GetUsageFence() const;

    const RSVulkanHandle<VkFence>& GetRenderFence() const;

    const RSVulkanHandle<VkSemaphore>& GetUsageSemaphore() const;

    const RSVulkanHandle<VkSemaphore>& GetRenderSemaphore() const;

    RSVulkanCommandBuffer& GetUsageCommandBuffer();

    RSVulkanCommandBuffer& GetRenderCommandBuffer();

    void SetMultiThreading()
    {
        multiThreading_ = true;
    }

    void UnsetMultiThreading()
    {
        multiThreading_ = false;
    }

    bool IsMultiThreading()
    {
        return multiThreading_;
    }

private:
    const RSVulkanProcTable& vk_;
    const RSVulkanHandle<VkDevice>& device_;
    static const size_t SEMAPHORE_SIZE = 2;
    static const size_t FENCE_SIZE = 2;
    std::array<RSVulkanHandle<VkSemaphore>, SEMAPHORE_SIZE> semaphores_;
    std::array<RSVulkanHandle<VkFence>, SEMAPHORE_SIZE> useFences_;
    RSVulkanCommandBuffer usageCommandBuffer_;
    RSVulkanCommandBuffer renderCommandBuffer_;
    bool valid_;

    bool CreateSemaphores();

    bool CreateFences();

    bool multiThreading_ = false;
};

} // namespace OHOS::Rosen::vulkan

#endif // RS_VULKAN_VULKAN_BACKBUFFER_H_
