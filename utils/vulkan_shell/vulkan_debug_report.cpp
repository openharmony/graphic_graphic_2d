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

#include "vulkan_debug_report.h"

#include <iomanip>
#include <vector>

#include "vulkan_utilities.h"

namespace OHOS::Rosen::vulkan {
static const VkDebugReportFlagsEXT kVulkanErrorFlags =
    VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT | VK_DEBUG_REPORT_ERROR_BIT_EXT;

static const VkDebugReportFlagsEXT kVulkanInfoFlags =
    VK_DEBUG_REPORT_INFORMATION_BIT_EXT | VK_DEBUG_REPORT_DEBUG_BIT_EXT;

std::string RSVulkanDebugReport::DebugExtensionName()
{
    return VK_EXT_DEBUG_REPORT_EXTENSION_NAME;
}

static const char* VkDebugReportFlagsEXTToString(VkDebugReportFlagsEXT reportFlags)
{
    if (reportFlags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT) {
        return "Flag: Information";
    } else if (reportFlags & VK_DEBUG_REPORT_WARNING_BIT_EXT) {
        return "Flag: Warning";
    } else if (reportFlags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) {
        return "Flag: Performance Warning";
    } else if (reportFlags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
        return "Flag: Error";
    } else if (reportFlags & VK_DEBUG_REPORT_DEBUG_BIT_EXT) {
        return "Flag: Debug";
    }
    return "Flag: UNKNOWN";
}

static const char* VkDebugReportObjectTypeEXTToString(VkDebugReportObjectTypeEXT type)
{
    switch (type) {
        case VK_DEBUG_REPORT_OBJECT_TYPE_UNKNOWN_EXT:
            return "Type: Unknown";
        case VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT:
            return "Type: Instance";
        case VK_DEBUG_REPORT_OBJECT_TYPE_PHYSICAL_DEVICE_EXT:
            return "Type: Physical Device";
        case VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_EXT:
            return "Type: Device";
        case VK_DEBUG_REPORT_OBJECT_TYPE_QUEUE_EXT:
            return "Type: Queue";
        case VK_DEBUG_REPORT_OBJECT_TYPE_SEMAPHORE_EXT:
            return "Type: Semaphore";
        case VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_BUFFER_EXT:
            return "Type: Command Buffer";
        case VK_DEBUG_REPORT_OBJECT_TYPE_FENCE_EXT:
            return "Type: Fence";
        case VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_MEMORY_EXT:
            return "Type: Device Memory";
        case VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT:
            return "Type: Buffer";
        case VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_EXT:
            return "Type: Image";
        case VK_DEBUG_REPORT_OBJECT_TYPE_EVENT_EXT:
            return "Type: Event";
        case VK_DEBUG_REPORT_OBJECT_TYPE_QUERY_POOL_EXT:
            return "Type: Query Pool";
        case VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_VIEW_EXT:
            return "Type: Buffer View";
        case VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_VIEW_EXT:
            return "Type: Image_view";
        case VK_DEBUG_REPORT_OBJECT_TYPE_SHADER_MODULE_EXT:
            return "Type: Shader Module";
        case VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_CACHE_EXT:
            return "Type: Pipeline Cache";
        case VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_LAYOUT_EXT:
            return "Type: Pipeline Layout";
        case VK_DEBUG_REPORT_OBJECT_TYPE_RENDER_PASS_EXT:
            return "Type: Render Pass";
        case VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_EXT:
            return "Type: Pipeline";
        case VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT_EXT:
            return "Type: Descriptor Set Layout";
        case VK_DEBUG_REPORT_OBJECT_TYPE_SAMPLER_EXT:
            return "Type: Sampler";
        case VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_POOL_EXT:
            return "Type: Descriptor Pool";
        case VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_EXT:
            return "Type: Descriptor Set";
        case VK_DEBUG_REPORT_OBJECT_TYPE_FRAMEBUFFER_EXT:
            return "Type: Framebuffer";
        case VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_POOL_EXT:
            return "Type: Command Pool";
        case VK_DEBUG_REPORT_OBJECT_TYPE_SURFACE_KHR_EXT:
            return "Type: Surface";
        case VK_DEBUG_REPORT_OBJECT_TYPE_SWAPCHAIN_KHR_EXT:
            return "Type: Swapchain";
        case VK_DEBUG_REPORT_OBJECT_TYPE_DEBUG_REPORT_EXT:
            return "Type: Debug";
        default:
            break;
    }

    return "Type: Unknown";
}

static VKAPI_ATTR VkBool32 OnVulkanDebugReportCallback(VkDebugReportFlagsEXT reportFlags,
    VkDebugReportObjectTypeEXT object_type, uint64_t object, size_t location, int32_t message_code,
    const char* layer_prefix, const char* message, void* user_data)
{
    std::vector<std::pair<std::string, std::string>> reportItems;

    reportItems.emplace_back("Severity", VkDebugReportFlagsEXTToString(reportFlags));

    reportItems.emplace_back("Object Type", VkDebugReportObjectTypeEXTToString(object_type));

    reportItems.emplace_back("Object Handle", std::to_string(object));

    if (location != 0) {
        reportItems.emplace_back("Location", std::to_string(location));
    }

    if (message_code != 0) {
        reportItems.emplace_back("Message Code", std::to_string(message_code));
    }

    if (layer_prefix != nullptr) {
        reportItems.emplace_back("Layer", layer_prefix);
    }

    if (message != nullptr) {
        reportItems.emplace_back("Message", message);
    }

    size_t padding = 0;

    for (const auto& item : reportItems) {
        padding = std::max(padding, item.first.size());
    }

    padding += 1;
    return VK_FALSE;
}

RSVulkanDebugReport::RSVulkanDebugReport(const RSVulkanProcTable& p_vk, const RSVulkanHandle<VkInstance>& application)
    : vk(p_vk), application_(application), valid_(false)
{
    if (!IsDebuggingEnabled() || !vk.CreateDebugReportCallbackEXT || !vk.DestroyDebugReportCallbackEXT) {
        return;
    }

    if (!application_) {
        return;
    }

    VkDebugReportFlagsEXT reportFlags = kVulkanErrorFlags;
    if (ValidationLayerInfoMessagesEnabled())
        reportFlags |= kVulkanInfoFlags;
    const VkDebugReportCallbackCreateInfoEXT createInfo = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT,
        .pNext = nullptr,
        .flags = reportFlags,
        .pfnCallback = &vulkan::OnVulkanDebugReportCallback,
        .pUserData = nullptr,
    };

    VkDebugReportCallbackEXT handle = VK_NULL_HANDLE;
    if (VK_CALL_LOG_ERROR(vk.CreateDebugReportCallbackEXT(application_, &createInfo, nullptr, &handle)) !=
        VK_SUCCESS) {
        return;
    }

    handle_ = { handle,
        [this](VkDebugReportCallbackEXT handle) { vk.DestroyDebugReportCallbackEXT(application_, handle, nullptr); } };

    valid_ = true;
}

RSVulkanDebugReport::~RSVulkanDebugReport() = default;

bool RSVulkanDebugReport::IsValid() const
{
    return valid_;
}

} // namespace OHOS::Rosen::vulkan
