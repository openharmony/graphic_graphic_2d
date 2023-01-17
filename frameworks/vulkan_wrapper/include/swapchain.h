/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#ifndef LIBVULKAN_SWAPCHAIN_H
#define LIBVULKAN_SWAPCHAIN_H 1

#include <vulkan/vulkan.h>
#include "driver.h"

namespace vulkan {
namespace driver {
const VkAllocationCallbacks& GetDefaultAllocator();
VKAPI_ATTR VkResult CreateOHOSSurfaceOpenHarmony(VkInstance instance,
    const VkOHOSSurfaceCreateInfoOpenHarmony* pCreateInfo,
    const VkAllocationCallbacks* allocator, VkSurfaceKHR* surface);
VKAPI_ATTR void DestroySurfaceKHR(VkInstance instance, VkSurfaceKHR surface, const VkAllocationCallbacks* allocator);
VKAPI_ATTR VkResult GetPhysicalDeviceSurfaceSupportKHR(VkPhysicalDevice pdev,
    uint32_t queue_family, VkSurfaceKHR surface, VkBool32* pSupported);
VKAPI_ATTR VkResult GetPhysicalDeviceSurfaceCapabilitiesKHR(VkPhysicalDevice pdev, VkSurfaceKHR surface,
    VkSurfaceCapabilitiesKHR* capabilities);
VKAPI_ATTR VkResult GetPhysicalDeviceSurfaceFormatsKHR(VkPhysicalDevice pdev,
    VkSurfaceKHR surface_handle, uint32_t* count, VkSurfaceFormatKHR* formats);
VKAPI_ATTR VkResult GetPhysicalDeviceSurfacePresentModesKHR(VkPhysicalDevice pdev,
    VkSurfaceKHR surface, uint32_t* count, VkPresentModeKHR* modes);
VKAPI_ATTR VkResult GetSwapchainImagesKHR(VkDevice device, VkSwapchainKHR swapchain_handle,
    uint32_t* count, VkImage* images);
VKAPI_ATTR VkResult CreateSwapchainKHR(VkDevice device, const VkSwapchainCreateInfoKHR* create_info,
    const VkAllocationCallbacks* allocator, VkSwapchainKHR* swapchain_handle);
VKAPI_ATTR void DestroySwapchainKHR(VkDevice device, VkSwapchainKHR swapchain_handle,
    const VkAllocationCallbacks* allocator);
VKAPI_ATTR VkResult AcquireNextImageKHR(VkDevice device, VkSwapchainKHR swapchain_handle,
    uint64_t timeout, VkSemaphore semaphore, VkFence fence, uint32_t* image_index);
VKAPI_ATTR VkResult AcquireNextImage2KHR(VkDevice device,
    const VkAcquireNextImageInfoKHR* pAcquireInfo, uint32_t* pImageIndex);

VKAPI_ATTR VkResult QueuePresentKHR(VkQueue queue, const VkPresentInfoKHR* present_info);
}  // namespace driver
}  // namespace vulkan

#endif  // LIBVULKAN_SWAPCHAIN_H
