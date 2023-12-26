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

#include "vulkan_backbuffer.h"

#include <limits>

#include "include/gpu/vk/GrVkTypes.h"
#include "vulkan/vulkan.h"
#include "vulkan_hilog.h"
#include "vulkan_proc_table.h"

namespace OHOS::Rosen::vulkan {

RSVulkanBackbuffer::RSVulkanBackbuffer(
    const RSVulkanProcTable& procVk, const RSVulkanHandle<VkDevice>& device, const RSVulkanHandle<VkCommandPool>& pool)
    : vk_(procVk), device_(device), usageCommandBuffer_(procVk, device, pool),
    renderCommandBuffer_(procVk, device, pool), valid_(false)
{
    if (!usageCommandBuffer_.IsValid() || !renderCommandBuffer_.IsValid()) {
        LOGE("Command buffers were not valid.");
        return;
    }

    if (!CreateSemaphores()) {
        LOGE("Could not create semaphores.");
        return;
    }

    if (!CreateFences()) {
        LOGE("Could not create fences.");
        return;
    }

    valid_ = true;
}

RSVulkanBackbuffer::~RSVulkanBackbuffer()
{
    WaitFences();
}

bool RSVulkanBackbuffer::IsValid() const
{
    return valid_;
}

bool RSVulkanBackbuffer::CreateSemaphores()
{
    const VkSemaphoreCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
    };

    auto semaphoreCollect = [this](VkSemaphore semaphore) { vk_.DestroySemaphore(device_, semaphore, nullptr); };

    for (size_t i = 0; i < semaphores_.size(); i++) {
        VkSemaphore semaphore = VK_NULL_HANDLE;
        if (VK_CALL_LOG_ERROR(vk_.CreateSemaphore(device_, &createInfo, nullptr, &semaphore)) != VK_SUCCESS) {
            return false;
        }
        semaphores_[i] = { semaphore, semaphoreCollect };
    }

    return true;
}

bool RSVulkanBackbuffer::CreateFences()
{
    const VkFenceCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };

    auto fenceCollect = [this](VkFence fence) { vk_.DestroyFence(device_, fence, nullptr); };

    for (size_t i = 0; i < useFences_.size(); i++) {
        VkFence fence = VK_NULL_HANDLE;

        if (VK_CALL_LOG_ERROR(vk_.CreateFence(device_, &createInfo, nullptr, &fence)) != VK_SUCCESS) {
            return false;
        }

        useFences_[i] = { fence, fenceCollect };
    }

    return true;
}

bool RSVulkanBackbuffer::WaitFences()
{
    VkFence fences[useFences_.size()];

    for (size_t i = 0; i < useFences_.size(); i++) {
        fences[i] = useFences_[i];
    }

    VkResult result = VK_CALL_LOG_ERROR(vk_.WaitForFences(
        device_, static_cast<uint32_t>(useFences_.size()), fences, true, std::numeric_limits<uint64_t>::max()));

    return result == VK_SUCCESS;
}

bool RSVulkanBackbuffer::ResetFences()
{
    VkFence fences[useFences_.size()];

    for (size_t i = 0; i < useFences_.size(); i++) {
        fences[i] = useFences_[i];
    }

    return VK_CALL_LOG_ERROR(vk_.ResetFences(device_, static_cast<uint32_t>(useFences_.size()), fences)) == VK_SUCCESS;
}

const RSVulkanHandle<VkFence>& RSVulkanBackbuffer::GetUsageFence() const
{
    return useFences_[0];
}

const RSVulkanHandle<VkFence>& RSVulkanBackbuffer::GetRenderFence() const
{
    return useFences_[1];
}

const RSVulkanHandle<VkSemaphore>& RSVulkanBackbuffer::GetUsageSemaphore() const
{
    return semaphores_[0];
}

const RSVulkanHandle<VkSemaphore>& RSVulkanBackbuffer::GetRenderSemaphore() const
{
    return semaphores_[1];
}

RSVulkanCommandBuffer& RSVulkanBackbuffer::GetUsageCommandBuffer()
{
    return usageCommandBuffer_;
}

RSVulkanCommandBuffer& RSVulkanBackbuffer::GetRenderCommandBuffer()
{
    return renderCommandBuffer_;
}

} // namespace OHOS::Rosen::vulkan
