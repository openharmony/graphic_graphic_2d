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

#include "vulkan_utilities.h"

#include <algorithm>
#include <unordered_set>

namespace OHOS::Rosen::vulkan {

bool IsDebuggingEnabled()
{
#ifndef NDEBUG
    return true;
#else
    return false;
#endif
}

bool ValidationLayerInfoMessagesEnabled()
{
    return false;
}

bool ValidationErrorsFatal()
{
    return true;
}

static std::vector<std::string> InstanceOrDeviceLayersToEnable(
    const RSVulkanProcTable& vk, VkPhysicalDevice physical_device)
{
    if (!IsDebuggingEnabled()) {
        return {};
    }

    // NOTE: The loader is sensitive to the ordering here. Please do not rearrange
    // this list.
    const std::vector<std::string> candidates = { "VK_LAYER_GOOGLE_threading", "VK_LAYER_LUNARG_parameter_validation",
        "VK_LAYER_LUNARG_object_tracker", "VK_LAYER_LUNARG_core_validation", "VK_LAYER_LUNARG_device_limits",
        "VK_LAYER_LUNARG_image", "VK_LAYER_LUNARG_swapchain", "VK_LAYER_GOOGLE_unique_objects" };

    uint32_t count = 0;

    if (physical_device == VK_NULL_HANDLE) {
        if (VK_CALL_LOG_ERROR(vk.EnumerateInstanceLayerProperties(&count, nullptr)) != VK_SUCCESS) {
            return {};
        }
    } else {
        if (VK_CALL_LOG_ERROR(vk.EnumerateDeviceLayerProperties(physical_device, &count, nullptr)) != VK_SUCCESS) {
            return {};
        }
    }

    std::vector<VkLayerProperties> properties;
    properties.resize(count);

    if (physical_device == VK_NULL_HANDLE) {
        if (VK_CALL_LOG_ERROR(vk.EnumerateInstanceLayerProperties(&count, properties.data())) != VK_SUCCESS) {
            return {};
        }
    } else {
        if (VK_CALL_LOG_ERROR(vk.EnumerateDeviceLayerProperties(physical_device, &count, properties.data())) !=
            VK_SUCCESS) {
            return {};
        }
    }

    std::unordered_set<std::string> available_extensions;
    std::vector<std::string> available_candidates;

    for (size_t i = 0; i < count; i++) {
        available_extensions.emplace(properties[i].layerName);
    }

    for (const auto& candidate : candidates) {
        auto found = available_extensions.find(candidate);
        if (found != available_extensions.end()) {
            available_candidates.emplace_back(candidate);
        }
    }

    return available_candidates;
}

std::vector<std::string> InstanceLayersToEnable(const RSVulkanProcTable& vk)
{
    return InstanceOrDeviceLayersToEnable(vk, VK_NULL_HANDLE);
}

std::vector<std::string> DeviceLayersToEnable(
    const RSVulkanProcTable& vk, const RSVulkanHandle<VkPhysicalDevice>& physical_device)
{
    if (!physical_device) {
        return {};
    }

    return InstanceOrDeviceLayersToEnable(vk, physical_device);
}

} // namespace OHOS::Rosen::vulkan
