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

#include "vulkan_device.h"

#include <limits>
#include <map>
#include <vector>

#include "include/gpu/vk/GrVkBackendContext.h"
#include "vulkan_hilog.h"
#include "vulkan_proc_table.h"
#include "vulkan_surface.h"
#include "vulkan_utilities.h"

namespace OHOS::Rosen::vulkan {

constexpr auto K_VULKAN_INVALID_GRAPHICS_QUEUE_INDEX = std::numeric_limits<uint32_t>::max();

static uint32_t FindQueueIndex(const std::vector<VkQueueFamilyProperties>& properties, VkQueueFlagBits flagBits)
{
    for (uint32_t i = 0, count = static_cast<uint32_t>(properties.size()); i < count; i++) {
        if (properties[i].queueFlags & flagBits) {
            return i;
        }
    }
    return K_VULKAN_INVALID_GRAPHICS_QUEUE_INDEX;
}

RSVulkanDevice::RSVulkanDevice(RSVulkanProcTable& procVk, RSVulkanHandle<VkPhysicalDevice> physicalDevice)
    : vk(procVk), physicalDevice_(std::move(physicalDevice)), graphicQueueIndex_(std::numeric_limits<uint32_t>::max()),
      computeQueueIndex_(std::numeric_limits<uint32_t>::max()), valid_(false)
{
    if (!physicalDevice_ || !vk.AreInstanceProcsSetup()) {
        return;
    }

    std::vector<VkQueueFamilyProperties> properties = GetQueueFamilyProperties();
    graphicQueueIndex_ = FindQueueIndex(properties, VK_QUEUE_GRAPHICS_BIT);
    computeQueueIndex_ = FindQueueIndex(properties, VK_QUEUE_COMPUTE_BIT);

    if (graphicQueueIndex_ == K_VULKAN_INVALID_GRAPHICS_QUEUE_INDEX) {
        LOGE("Could not find the graphics queue index.");
        return;
    }

    const float priorities[1] = { 1.0f };

    std::vector<VkDeviceQueueCreateInfo> queueCreate { {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueFamilyIndex = graphicQueueIndex_,
        .queueCount = 1,
        .pQueuePriorities = priorities,
    } };

    if (computeQueueIndex_ != K_VULKAN_INVALID_GRAPHICS_QUEUE_INDEX && computeQueueIndex_ != graphicQueueIndex_) {
        queueCreate.push_back({
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueFamilyIndex = graphicQueueIndex_,
            .queueCount = 1,
            .pQueuePriorities = priorities,
        });
    }

    const char* extensions[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };

    auto enabledLayers = DeviceLayersToEnable(vk, physicalDevice_);

    const char* layers[enabledLayers.size()];

    for (size_t i = 0; i < enabledLayers.size(); i++) {
        layers[i] = enabledLayers[i].c_str();
    }

    const VkDeviceCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueCreateInfoCount = queueCreate.size(),
        .pQueueCreateInfos = queueCreate.data(),
        .enabledLayerCount = static_cast<uint32_t>(enabledLayers.size()),
        .ppEnabledLayerNames = layers,
        .enabledExtensionCount = sizeof(extensions) / sizeof(const char*),
        .ppEnabledExtensionNames = extensions,
        .pEnabledFeatures = nullptr,
    };

    VkDevice device = VK_NULL_HANDLE;

    if (VK_CALL_LOG_ERROR(vk.CreateDevice(physicalDevice_, &create_info, nullptr, &device)) != VK_SUCCESS) {
        LOGE("Could not create device.");
        return;
    }

    device_ = { device, [this](VkDevice device) { vk.DestroyDevice(device, nullptr); } };
    if (!vk.SetupDeviceProcAddresses(device_)) {
        LOGE("Could not setup device proc addresses.");
        return;
    }

    VkQueue queue = VK_NULL_HANDLE;

    vk.GetDeviceQueue(device_, graphicQueueIndex_, 0, &queue);

    if (queue == VK_NULL_HANDLE) {
        LOGE("Could not get the device queue handle.");
        return;
    }

    queue_ = queue;

    const VkCommandPoolCreateInfo commandPoolCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = 0,
    };

    VkCommandPool commandPool = VK_NULL_HANDLE;
    if (VK_CALL_LOG_ERROR(vk.CreateCommandPool(device_, &commandPoolCreateInfo, nullptr, &commandPool)) !=
        VK_SUCCESS) {
        LOGE("Could not create the command pool.");
        return;
    }

    commandPool_ = { commandPool, [this](VkCommandPool pool) { vk.DestroyCommandPool(device_, pool, nullptr); } };

    valid_ = true;
}

RSVulkanDevice::~RSVulkanDevice()
{
    WaitIdle();
}

bool RSVulkanDevice::IsValid() const
{
    return valid_;
}

bool RSVulkanDevice::WaitIdle() const
{
    return VK_CALL_LOG_ERROR(vk.DeviceWaitIdle(device_)) == VK_SUCCESS;
}

const RSVulkanHandle<VkDevice>& RSVulkanDevice::GetHandle() const
{
    return device_;
}

void RSVulkanDevice::ReleaseDeviceOwnership()
{
    device_.ReleaseOwnership();
}

const RSVulkanHandle<VkPhysicalDevice>& RSVulkanDevice::GetPhysicalDeviceHandle() const
{
    return physicalDevice_;
}

const RSVulkanHandle<VkQueue>& RSVulkanDevice::GetQueueHandle() const
{
    return queue_;
}

const RSVulkanHandle<VkCommandPool>& RSVulkanDevice::GetCommandPool() const
{
    return commandPool_;
}

uint32_t RSVulkanDevice::GetGraphicsQueueIndex() const
{
    return graphicQueueIndex_;
}

bool RSVulkanDevice::GetSurfaceCapabilities(
    const RSVulkanSurface& surface, VkSurfaceCapabilitiesKHR* capabilities) const
{
    if (!surface.IsValid() || capabilities == nullptr) {
        LOGE("GetSurfaceCapabilities surface is not valid or capabilities is null");
        return false;
    }

    bool success = VK_CALL_LOG_ERROR(vk.GetPhysicalDeviceSurfaceCapabilitiesKHR(
        physicalDevice_, surface.Handle(), capabilities)) == VK_SUCCESS;
    if (!success) {
        LOGE("GetPhysicalDeviceSurfaceCapabilitiesKHR not success");
        return false;
    }

    // Check if the physical device surface capabilities are valid. If so, there
    // is nothing more to do.
    if (capabilities->currentExtent.width != 0xFFFFFFFF && capabilities->currentExtent.height != 0xFFFFFFFF) {
        return true;
    }

    // Ask the native surface for its size as a fallback.
    SkISize size = surface.GetSize();
    if (size.width() == 0 || size.height() == 0) {
        LOGE("GetSurfaceCapabilities surface size is 0");
        return false;
    }

    capabilities->currentExtent.width = size.width();
    capabilities->currentExtent.height = size.height();
    return true;
}

bool RSVulkanDevice::GetPhysicalDeviceFeatures(VkPhysicalDeviceFeatures* DeviceFeatures) const
{
    if (DeviceFeatures == nullptr || !physicalDevice_) {
        return false;
    }
    vk.GetPhysicalDeviceFeatures(physicalDevice_, DeviceFeatures);
    return true;
}

bool RSVulkanDevice::GetPhysicalDeviceFeaturesSkia(uint32_t* skFeatures) const
{
    if (skFeatures == nullptr) {
        return false;
    }

    VkPhysicalDeviceFeatures DeviceFeatures;

    if (!GetPhysicalDeviceFeatures(&DeviceFeatures)) {
        return false;
    }

    uint32_t flags = 0;

    if (DeviceFeatures.geometryShader) {
        flags |= kGeometryShader_GrVkFeatureFlag;
    }
    if (DeviceFeatures.dualSrcBlend) {
        flags |= kDualSrcBlend_GrVkFeatureFlag;
    }
    if (DeviceFeatures.sampleRateShading) {
        flags |= kSampleRateShading_GrVkFeatureFlag;
    }

    *skFeatures = flags;
    return true;
}

std::vector<VkQueueFamilyProperties> RSVulkanDevice::GetQueueFamilyProperties() const
{
    uint32_t count = 0;

    vk.GetPhysicalDeviceQueueFamilyProperties(physicalDevice_, &count, nullptr);

    std::vector<VkQueueFamilyProperties> properties;
    properties.resize(count, {});

    vk.GetPhysicalDeviceQueueFamilyProperties(physicalDevice_, &count, properties.data());

    return properties;
}

int RSVulkanDevice::ChooseSurfaceFormat(
    const RSVulkanSurface& surface, std::vector<VkFormat> desiredFormats, VkSurfaceFormatKHR* format) const
{
    if (!surface.IsValid() || format == nullptr) {
        LOGE("ChooseSurfaceFormat surface not valid or format == null");
        return -1;
    }

    uint32_t formatCount = 0;
    if (VK_CALL_LOG_ERROR(vk.GetPhysicalDeviceSurfaceFormatsKHR(
        physicalDevice_, surface.Handle(), &formatCount, nullptr)) != VK_SUCCESS) {
        LOGE("ChooseSurfaceFormat sGetPhysicalDeviceSurfaceFormatsKHR not success");
        return -1;
    }

    if (formatCount == 0) {
        LOGE("ChooseSurfaceFormat format count = 0");
        return -1;
    }

    VkSurfaceFormatKHR formats[formatCount];
    if (VK_CALL_LOG_ERROR(vk.GetPhysicalDeviceSurfaceFormatsKHR(
        physicalDevice_, surface.Handle(), &formatCount, formats)) != VK_SUCCESS) {
        LOGE("ChooseSurfaceFormat sGetPhysicalDeviceSurfaceFormatsKHR not success 2");
        return -1;
    }

    std::map<VkFormat, VkSurfaceFormatKHR> supportedFormats;
    for (uint32_t i = 0; i < formatCount; i++) {
        supportedFormats[formats[i].format] = formats[i];
    }

    // Try to find the first supported format in the list of desired formats.
    for (size_t i = 0; i < desiredFormats.size(); ++i) {
        auto found = supportedFormats.find(desiredFormats[i]);
        if (found != supportedFormats.end()) {
            *format = found->second;
            return static_cast<int>(i);
        }
    }
    LOGE("ChooseSurfaceFormat failded");
    return -1;
}

bool RSVulkanDevice::ChoosePresentMode(const RSVulkanSurface& surface, VkPresentModeKHR* presentMode) const
{
    if (!surface.IsValid() || presentMode == nullptr) {
        LOGE("ChoosePresentMode surface not valid or presentmode is null");
        return false;
    }

    *presentMode = VK_PRESENT_MODE_FIFO_KHR;
    return true;
}

bool RSVulkanDevice::QueueSubmit(std::vector<VkPipelineStageFlags> waitDestPipelineStages,
    const std::vector<VkSemaphore>& waitSemaphores, const std::vector<VkSemaphore>& signalSemaphores,
    const std::vector<VkCommandBuffer>& commandBuffers, const RSVulkanHandle<VkFence>& fence) const
{
    if (waitSemaphores.size() != waitDestPipelineStages.size()) {
        return false;
    }

    const VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = nullptr,
        .waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size()),
        .pWaitSemaphores = waitSemaphores.data(),
        .pWaitDstStageMask = waitDestPipelineStages.data(),
        .commandBufferCount = static_cast<uint32_t>(commandBuffers.size()),
        .pCommandBuffers = commandBuffers.data(),
        .signalSemaphoreCount = static_cast<uint32_t>(signalSemaphores.size()),
        .pSignalSemaphores = signalSemaphores.data(),
    };
    if (VK_CALL_LOG_ERROR(vk.QueueSubmit(queue_, 1, &submitInfo, fence)) != VK_SUCCESS) {
        return false;
    }

    return true;
}

} // namespace OHOS::Rosen::vulkan
