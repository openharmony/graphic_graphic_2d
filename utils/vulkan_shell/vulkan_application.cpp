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

#include "vulkan_application.h"

#include <utility>
#include <vector>

#include "vulkan_device.h"
#include "vulkan_hilog.h"
#include "vulkan_proc_table.h"
#include "vulkan_utilities.h"

namespace OHOS::Rosen::vulkan {

RSVulkanApplication::RSVulkanApplication(RSVulkanProcTable& procVk, const std::string& applicationName,
    std::vector<std::string> enabledExtensions, uint32_t applicationVersion, uint32_t apiVersion)
    : vk_(procVk), apiVersion_(apiVersion), valid_(false)
{
    std::vector<VkExtensionProperties> supportedExtensions = GetSupportedInstanceExtensions(vk_);
    bool enableInstanceDebugging =
        IsDebuggingEnabled() && ExtensionSupported(supportedExtensions, RSVulkanDebugReport::DebugExtensionName());
    if (enableInstanceDebugging) {
        enabledExtensions.emplace_back(RSVulkanDebugReport::DebugExtensionName());
    }

    const char* extensions[enabledExtensions.size()];

    for (size_t i = 0; i < enabledExtensions.size(); i++) {
        extensions[i] = enabledExtensions[i].c_str();
    }

    const std::vector<std::string> enabledLayers = InstanceLayersToEnable(vk_);

    const char* layers[enabledLayers.size()];

    for (size_t i = 0; i < enabledLayers.size(); i++) {
        layers[i] = enabledLayers[i].c_str();
    }

    const VkApplicationInfo info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName = applicationName.c_str(),
        .applicationVersion = applicationVersion,
        .pEngineName = "Rosen",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = apiVersion_,
    };

    const VkInstanceCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .pApplicationInfo = &info,
        .enabledLayerCount = static_cast<uint32_t>(enabledLayers.size()),
        .ppEnabledLayerNames = layers,
        .enabledExtensionCount = static_cast<uint32_t>(enabledExtensions.size()),
        .ppEnabledExtensionNames = extensions,
    };

    VkInstance instance = VK_NULL_HANDLE;

    if (VK_CALL_LOG_ERROR(vk_.CreateInstance(&createInfo, nullptr, &instance)) != VK_SUCCESS) {
        LOGE("Could not create application instance.");
        return;
    }

    if (!vk_.SetupInstanceProcAddresses(instance)) {
        LOGE("Could not setup instance proc addresses.");
        return;
    }

    instance_ = { instance, [this](VkInstance i) {
        LOGE("Destroying Vulkan instance");
        vk_.DestroyInstance(i, nullptr);
    } };

    if (enableInstanceDebugging) {
        auto debug_report = std::make_unique<RSVulkanDebugReport>(vk_, instance_);
        if (!debug_report->IsValid()) {
            LOGE("Vulkan debugging was enabled but could not be setup for this instance.");
        } else {
            debugReport_ = std::move(debug_report);
            LOGE("Debug reporting is enabled.");
        }
    }

    valid_ = true;
}

RSVulkanApplication::~RSVulkanApplication() = default;

bool RSVulkanApplication::IsValid() const
{
    return valid_;
}

uint32_t RSVulkanApplication::GetAPIVersion() const
{
    return apiVersion_;
}

const RSVulkanHandle<VkInstance>& RSVulkanApplication::GetInstance() const
{
    return instance_;
}

void RSVulkanApplication::ReleaseInstanceOwnership()
{
    instance_.ReleaseOwnership();
}

std::vector<VkPhysicalDevice> RSVulkanApplication::GetPhysicalDevices() const
{
    if (!IsValid()) {
        return {};
    }

    uint32_t deviceCount = 0;
    if (VK_CALL_LOG_ERROR(vk_.EnumeratePhysicalDevices(instance_, &deviceCount, nullptr)) != VK_SUCCESS) {
        LOGE("Could not enumerate physical device.");
        return {};
    }

    if (deviceCount == 0) {
        // No available devices.
        LOGE("No physical devices found.");
        return {};
    }

    std::vector<VkPhysicalDevice> physical_devices;

    physical_devices.resize(deviceCount);

    if (VK_CALL_LOG_ERROR(vk_.EnumeratePhysicalDevices(instance_, &deviceCount, physical_devices.data())) !=
        VK_SUCCESS) {
        LOGE("Could not enumerate physical device.");
        return {};
    }

    return physical_devices;
}

std::unique_ptr<RSVulkanDevice> RSVulkanApplication::AcquireFirstCompatibleLogicalDevice() const
{
    for (auto device_handle : GetPhysicalDevices()) {
        auto logical_device = std::make_unique<RSVulkanDevice>(vk_, device_handle);
        if (logical_device->IsValid()) {
            return logical_device;
        }
    }
    LOGE("Could not acquire compatible logical device.");
    return nullptr;
}

std::vector<VkExtensionProperties> RSVulkanApplication::GetSupportedInstanceExtensions(
    const RSVulkanProcTable& vk) const
{
    if (!vk.EnumerateInstanceExtensionProperties) {
        return std::vector<VkExtensionProperties>();
    }

    uint32_t count = 0;
    if (VK_CALL_LOG_ERROR(vk.EnumerateInstanceExtensionProperties(nullptr, &count, nullptr)) != VK_SUCCESS) {
        return std::vector<VkExtensionProperties>();
    }

    if (count == 0) {
        return std::vector<VkExtensionProperties>();
    }

    std::vector<VkExtensionProperties> properties;
    properties.resize(count);
    if (VK_CALL_LOG_ERROR(vk.EnumerateInstanceExtensionProperties(nullptr, &count, properties.data())) != VK_SUCCESS) {
        return std::vector<VkExtensionProperties>();
    }

    return properties;
}

bool RSVulkanApplication::ExtensionSupported(
    const std::vector<VkExtensionProperties>& supportedInstanceExtensions, std::string extensionName)
{
    uint32_t count = supportedInstanceExtensions.size();
    for (size_t i = 0; i < count; i++) {
        if (strncmp(supportedInstanceExtensions[i].extensionName, extensionName.c_str(), extensionName.size()) == 0) {
            return true;
        }
    }

    return false;
}

} // namespace OHOS::Rosen::vulkan
