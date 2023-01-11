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

#include <mutex>
#include <malloc.h>
#include <string>
#include <vector>
#include "swapchain.h"
#include "driver_loader.h"
#include "vulkan/vk_ohos_native_buffer.h"
#include "wrapper_log.h"

namespace vulkan {
namespace driver {
DriverLoader DriverLoader::loader_;
PFN_vkGetDeviceProcAddr pfn_vkGetDeviceProcAddr = nullptr;
PFN_vkGetPhysicalDeviceProperties2KHR fpn_vkGetPhysicalDeviceProperties2KHR = nullptr;
PFN_vkCreateDevice pfn_vkCreateDevice = nullptr;
PFN_vkGetNativeFenceFdOpenHarmony pfn_vkGetNativeFenceFdOpenHarmony = nullptr;
PFN_vkGetPhysicalDeviceProperties pfn_vkGetPhysicalDeviceProperties = nullptr;
PFN_vkGetPhysicalDeviceFeatures pfn_vkGetPhysicalDeviceFeatures = nullptr;
PFN_vkGetPhysicalDeviceMemoryProperties pfn_vkGetPhysicalDeviceMemoryProperties = nullptr;
PFN_vkGetPhysicalDeviceQueueFamilyProperties pfn_vkGetPhysicalDeviceQueueFamilyProperties = nullptr;

bool IsSupportedVulkan()
{
    DriverLoader::Load();
    return DriverLoader::IsSupportedVulkan();
}

VkResult CreateInstance(const VkInstanceCreateInfo* pCreateInfo,
                        const VkAllocationCallbacks* pAllocator,
                        VkInstance* pInstance)
{
    if (!DriverLoader::Load()) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    if (pAllocator == nullptr) {
        pAllocator = &GetDefaultAllocator();
    }
    VkResult result = VK_SUCCESS;

    result = DriverLoader::GetVulkanFuncs().PFN_vkCreateInstance(pCreateInfo, pAllocator, pInstance);
    if (result != VK_SUCCESS) {
        return result;
    }
    pfn_vkGetDeviceProcAddr = reinterpret_cast<PFN_vkGetDeviceProcAddr>(
        DriverLoader::GetVulkanFuncs().PFN_vkGetInstanceProcAddr(*pInstance, "vkGetDeviceProcAddr"));
    fpn_vkGetPhysicalDeviceProperties2KHR = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties2KHR>(
            DriverLoader::GetVulkanFuncs().PFN_vkGetInstanceProcAddr(*pInstance, "vkGetPhysicalDeviceProperties2KHR"));
    pfn_vkCreateDevice = reinterpret_cast<PFN_vkCreateDevice>(
        DriverLoader::GetVulkanFuncs().PFN_vkGetInstanceProcAddr(*pInstance, "vkCreateDevice"));
    pfn_vkGetPhysicalDeviceProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties>(
        DriverLoader::GetVulkanFuncs().PFN_vkGetInstanceProcAddr(*pInstance, "vkGetPhysicalDeviceProperties"));
    pfn_vkGetPhysicalDeviceFeatures = reinterpret_cast<PFN_vkGetPhysicalDeviceFeatures>(
        DriverLoader::GetVulkanFuncs().PFN_vkGetInstanceProcAddr(*pInstance, "vkGetPhysicalDeviceFeatures"));
    pfn_vkGetPhysicalDeviceMemoryProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceMemoryProperties>(
        DriverLoader::GetVulkanFuncs().PFN_vkGetInstanceProcAddr(*pInstance, "vkGetPhysicalDeviceMemoryProperties"));
    pfn_vkGetPhysicalDeviceQueueFamilyProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceQueueFamilyProperties>(
        DriverLoader::GetVulkanFuncs().PFN_vkGetInstanceProcAddr(*pInstance,
            "vkGetPhysicalDeviceQueueFamilyProperties"));
    return VK_SUCCESS;
}


VkResult EnumerateInstanceExtensionProperties(const char* pLayerName,
    uint32_t* pPropertyCount, VkExtensionProperties* pProperties)
{
    if (!DriverLoader::Load()) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    VkResult result = DriverLoader::GetVulkanFuncs().PFN_vkEnumerateInstanceExtensionProperties(
        pLayerName, pPropertyCount, pProperties);
    if (result != VK_SUCCESS) {
        return result;
    }
    return VK_SUCCESS;
}

PFN_vkVoidFunction GetInstanceProcAddr(VkInstance instance, const char* pName)
{
    if (!DriverLoader::Load()) {
        return nullptr;
    }

    if (std::strcmp(pName, "vkCreateOHOSSurfaceOpenHarmony") == 0) {
        return reinterpret_cast<PFN_vkVoidFunction>(vulkan::driver::CreateOHOSSurfaceOpenHarmony);
    }

    if (std::strcmp(pName, "vkGetPhysicalDeviceSurfaceSupportKHR") == 0) {
        return reinterpret_cast<PFN_vkVoidFunction>(vulkan::driver::GetPhysicalDeviceSurfaceSupportKHR);
    }

    if (std::strcmp(pName, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR") == 0) {
        return reinterpret_cast<PFN_vkVoidFunction>(vulkan::driver::GetPhysicalDeviceSurfaceCapabilitiesKHR);
    }
    
    if (std::strcmp(pName, "vkGetPhysicalDeviceSurfaceFormatsKHR") == 0) {
        return reinterpret_cast<PFN_vkVoidFunction>(vulkan::driver::GetPhysicalDeviceSurfaceFormatsKHR);
    }
    
    if (std::strcmp(pName, "vkGetPhysicalDeviceSurfacePresentModesKHR") == 0) {
        return reinterpret_cast<PFN_vkVoidFunction>(vulkan::driver::GetPhysicalDeviceSurfacePresentModesKHR);
    }
    
    if (std::strcmp(pName, "vkDestroySurfaceKHR") == 0) {
        return reinterpret_cast<PFN_vkVoidFunction>(vulkan::driver::DestroySurfaceKHR);
    }

    if (std::strcmp(pName, "vkCreateDevice") == 0) {
        return reinterpret_cast<PFN_vkVoidFunction>(vulkan::driver::CreateDevice);
    }

    if (std::strcmp(pName, "vkDestroyInstance") == 0) {
        return reinterpret_cast<PFN_vkVoidFunction>(vulkan::driver::DestroyInstance);
    }

    PFN_vkVoidFunction func = DriverLoader::GetVulkanFuncs().PFN_vkGetInstanceProcAddr(instance, pName);
    if (!func) {
        WLOGE("GetInstanceProcAddr %{public}s failed, please check", pName);
    }

    return func;
}

void DestroyInstance(VkInstance instance, const VkAllocationCallbacks* pAllocator)
{
    PFN_vkDestroyInstance pfn_vkDestroyInstance = reinterpret_cast<PFN_vkDestroyInstance>(
        DriverLoader::GetVulkanFuncs().PFN_vkGetInstanceProcAddr(instance, "vkDestroyInstance"));
    if (pfn_vkDestroyInstance) {
        pfn_vkDestroyInstance(instance, pAllocator);
    }

    if (!DriverLoader::Unload()) {
        WLOGE("DriverLoader::Unload() failed");
    }
}

VkResult CreateDevice(VkPhysicalDevice physicalDevice,
                      const VkDeviceCreateInfo* pCreateInfo,
                      const VkAllocationCallbacks* pAllocator,
                      VkDevice* pDevice)
{
    VkResult result = VK_SUCCESS;
    if (pfn_vkCreateDevice) {
        std::vector<const char*> deviceExtensions;

        auto& ext_names = (*pCreateInfo).ppEnabledExtensionNames;
        auto& ext_count = (*pCreateInfo).enabledExtensionCount;
        for (uint32_t i = 0; i < ext_count; i++) {
            deviceExtensions.push_back(ext_names[i]);
        }
        deviceExtensions.push_back(VK_OHOS_NATIVE_BUFFER_EXTENSION_NAME);
        deviceExtensions.push_back(VK_OPENHARMONY_EXTERNAL_MEMORY_OHOS_NATIVE_BUFFER_EXTENSION_NAME);
        VkDeviceCreateInfo createInfo(*pCreateInfo);

        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        result = pfn_vkCreateDevice(physicalDevice, &createInfo, pAllocator, pDevice);
    }

    if ((result == VK_SUCCESS) && (pfn_vkGetDeviceProcAddr != nullptr)) {
        pfn_vkGetNativeFenceFdOpenHarmony= reinterpret_cast<PFN_vkGetNativeFenceFdOpenHarmony>(
            pfn_vkGetDeviceProcAddr(*pDevice, "vkGetNativeFenceFdOpenHarmony"));
        if (!pfn_vkGetNativeFenceFdOpenHarmony) {
            WLOGE("vulkan::driver::CreateDevice Get vkGetNativeFenceFdOpenHarmony failed");
        }
    }

    return result;
}


PFN_vkVoidFunction GetDeviceProcAddr(VkDevice device, const char* pName)
{
    PFN_vkVoidFunction func = nullptr;

    if (!pfn_vkGetDeviceProcAddr) {
        WLOGE("pfn_vkGetDeviceProcAddr is null, please check");
        return nullptr;
    }
    if (std::strcmp(pName, "vkCreateSwapchainKHR") == 0) {
        return reinterpret_cast<PFN_vkVoidFunction>(vulkan::driver::CreateSwapchainKHR);
    }

    if (std::strcmp(pName, "vkDestroySwapchainKHR") == 0) {
        return reinterpret_cast<PFN_vkVoidFunction>(vulkan::driver::DestroySwapchainKHR);
    }

    if (std::strcmp(pName, "vkGetSwapchainImagesKHR") == 0) {
        return reinterpret_cast<PFN_vkVoidFunction>(vulkan::driver::GetSwapchainImagesKHR);
    }

    if (std::strcmp(pName, "vkAcquireNextImageKHR") == 0) {
        return reinterpret_cast<PFN_vkVoidFunction>(vulkan::driver::AcquireNextImageKHR);
    }

    if (std::strcmp(pName, "vkAcquireNextImage2KHR") == 0) {
        return reinterpret_cast<PFN_vkVoidFunction>(vulkan::driver::AcquireNextImage2KHR);
    }

    if (std::strcmp(pName, "vkQueuePresentKHR") == 0) {
        return reinterpret_cast<PFN_vkVoidFunction>(vulkan::driver::QueuePresentKHR);
    }
    func = pfn_vkGetDeviceProcAddr(device, pName);
    if (!func) {
        WLOGE("GetDeviceProcAddr %{public}s  is null, please check", pName);
    }

    return func;
}

void DestroyImage(VkDevice device, VkImage image, const VkAllocationCallbacks* pAllocator)
{
    PFN_vkDestroyImage destroyImage = reinterpret_cast<PFN_vkDestroyImage>(GetDeviceProcAddr(device, "vkDestroyImage"));
    if (destroyImage) {
        return destroyImage(device, image, pAllocator);
    }
    return;
}

VkResult CreateImage(VkDevice device, const VkImageCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator, VkImage* pImage)
{
    PFN_vkCreateImage createImage = reinterpret_cast<PFN_vkCreateImage>(GetDeviceProcAddr(device, "vkCreateImage"));
    if (createImage) {
        return createImage(device, pCreateInfo, pAllocator, pImage);
    }
    return VK_ERROR_INITIALIZATION_FAILED;
}

void QueryPresentationProperties(
    VkPhysicalDevice physicalDevice,
    VkPhysicalDevicePresentationPropertiesOpenHarmony* presentation_properties)
{
    VkPhysicalDeviceProperties2 properties = {
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
        presentation_properties,
        {},
    };

    presentation_properties->sType =
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENTATION_PROPERTIES_OPENHARMONY;
    presentation_properties->pNext = nullptr;
    presentation_properties->sharedImage = VK_FALSE;

    GetPhysicalDeviceProperties2(physicalDevice, &properties);
}

void GetPhysicalDeviceProperties2(VkPhysicalDevice physicalDevice, VkPhysicalDeviceProperties2* pProperties)
{
    if (fpn_vkGetPhysicalDeviceProperties2KHR) {
        fpn_vkGetPhysicalDeviceProperties2KHR(physicalDevice, pProperties);
    }
}

VkResult SetNativeFenceFdOpenHarmony(VkDevice device,
    int nativeFenceFd,
    VkSemaphore semaphore,
    VkFence fence)
{
    PFN_vkSetNativeFenceFdOpenHarmony acquireImage = reinterpret_cast<PFN_vkSetNativeFenceFdOpenHarmony>(
        GetDeviceProcAddr(device, "vkSetNativeFenceFdOpenHarmony"));
    if (acquireImage) {
        return acquireImage(device, nativeFenceFd, semaphore, fence);
    }
    return VK_ERROR_INITIALIZATION_FAILED;
}

VkResult GetNativeFenceFdOpenHarmony(
    VkQueue queue,
    uint32_t waitSemaphoreCount,
    const VkSemaphore* pWaitSemaphores,
    VkImage image,
    int* pNativeFenceFd)
{
    if (pfn_vkGetNativeFenceFdOpenHarmony) {
        return pfn_vkGetNativeFenceFdOpenHarmony(queue, waitSemaphoreCount, pWaitSemaphores, image, pNativeFenceFd);
    }
    return VK_ERROR_INITIALIZATION_FAILED;
}
} // namespace driver
} // namespace vulkan
