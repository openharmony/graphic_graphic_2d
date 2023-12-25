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

#ifndef RS_VULKAN_VULKAN_DEVICE_H_
#define RS_VULKAN_VULKAN_DEVICE_H_

#include <vector>

#include "vulkan_handle.h"

namespace OHOS::Rosen::vulkan {

class RSVulkanProcTable;
class RSVulkanSurface;

class RSVulkanDevice {
public:
    RSVulkanDevice(RSVulkanProcTable& vk, RSVulkanHandle<VkPhysicalDevice> physicalDevice);

    ~RSVulkanDevice();

    bool IsValid() const;

    const RSVulkanHandle<VkDevice>& GetHandle() const;

    const RSVulkanHandle<VkPhysicalDevice>& GetPhysicalDeviceHandle() const;

    const RSVulkanHandle<VkQueue>& GetQueueHandle() const;

    const RSVulkanHandle<VkCommandPool>& GetCommandPool() const;

    uint32_t GetGraphicsQueueIndex() const;

    void ReleaseDeviceOwnership();

    bool GetSurfaceCapabilities(const RSVulkanSurface& surface, VkSurfaceCapabilitiesKHR* capabilities) const;

    bool GetPhysicalDeviceFeatures(VkPhysicalDeviceFeatures* features) const;

    bool GetPhysicalDeviceFeaturesSkia(uint32_t* features) const;

    int ChooseSurfaceFormat(
        const RSVulkanSurface& surface, std::vector<VkFormat> desiredFormats, VkSurfaceFormatKHR* format) const;

    bool ChoosePresentMode(const RSVulkanSurface& surface, VkPresentModeKHR* presentMode) const;

    bool QueueSubmit(std::vector<VkPipelineStageFlags> waitDestPipelineStages,
        const std::vector<VkSemaphore>& waitSemaphores, const std::vector<VkSemaphore>& signalSemaphores,
        const std::vector<VkCommandBuffer>& commandBuffers, const RSVulkanHandle<VkFence>& fence) const;

    bool WaitIdle() const;
    RSVulkanProcTable& vk;
    RSVulkanHandle<VkPhysicalDevice> physicalDevice_ = VK_NULL_HANDLE;
    RSVulkanHandle<VkDevice> device_ = VK_NULL_HANDLE;

private:
    RSVulkanHandle<VkQueue> queue_ = VK_NULL_HANDLE;
    RSVulkanHandle<VkCommandPool> commandPool_ = VK_NULL_HANDLE;
    uint32_t graphicQueueIndex_ = 0;
    uint32_t computeQueueIndex_ = 0;
    bool valid_ = false;

    std::vector<VkQueueFamilyProperties> GetQueueFamilyProperties() const;
};

} // namespace OHOS::Rosen::vulkan

#endif // RS_VULKAN_VULKAN_DEVICE_H_
