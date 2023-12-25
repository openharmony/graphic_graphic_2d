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

#include "vulkan_interface.h"

namespace OHOS::Rosen::vulkan {

std::string VulkanResultToString(VkResult result)
{
    switch (result) {
        case VK_SUCCESS:
            return "RS_VK_SUCCESS";
        case VK_NOT_READY:
            return "RS_VK_NOT_READY";
        case VK_TIMEOUT:
            return "RS_VK_TIMEOUT";
        case VK_EVENT_SET:
            return "RS_VK_EVENT_SET";
        case VK_EVENT_RESET:
            return "RS_VK_EVENT_RESET";
        case VK_INCOMPLETE:
            return "RS_VK_INCOMPLETE";
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            return "RS_VK_ERROR_OUT_OF_HOST_MEMORY";
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            return "RS_VK_ERROR_OUT_OF_DEVICE_MEMORY";
        case VK_ERROR_INITIALIZATION_FAILED:
            return "RS_VK_ERROR_INITIALIZATION_FAILED";
        case VK_ERROR_DEVICE_LOST:
            return "RS_VK_ERROR_DEVICE_LOST";
        case VK_ERROR_MEMORY_MAP_FAILED:
            return "RS_VK_ERROR_MEMORY_MAP_FAILED";
        case VK_ERROR_LAYER_NOT_PRESENT:
            return "RS_VK_ERROR_LAYER_NOT_PRESENT";
        case VK_ERROR_EXTENSION_NOT_PRESENT:
            return "RS_VK_ERROR_EXTENSION_NOT_PRESENT";
        case VK_ERROR_FEATURE_NOT_PRESENT:
            return "RS_VK_ERROR_FEATURE_NOT_PRESENT";
        case VK_ERROR_INCOMPATIBLE_DRIVER:
            return "RS_VK_ERROR_INCOMPATIBLE_DRIVER";
        case VK_ERROR_TOO_MANY_OBJECTS:
            return "RS_VK_ERROR_TOO_MANY_OBJECTS";
        case VK_ERROR_FORMAT_NOT_SUPPORTED:
            return "RS_VK_ERROR_FORMAT_NOT_SUPPORTED";
        case VK_ERROR_FRAGMENTED_POOL:
            return "RS_VK_ERROR_FRAGMENTED_POOL";
        case VK_ERROR_SURFACE_LOST_KHR:
            return "RS_VK_ERROR_SURFACE_LOST_KHR";
        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
            return "RS_VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
        case VK_SUBOPTIMAL_KHR:
            return "RS_VK_SUBOPTIMAL_KHR";
        case VK_ERROR_OUT_OF_DATE_KHR:
            return "RS_VK_ERROR_OUT_OF_DATE_KHR";
        case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
            return "RS_VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
        case VK_ERROR_VALIDATION_FAILED_EXT:
            return "RS_VK_ERROR_VALIDATION_FAILED_EXT";
        case VK_ERROR_INVALID_SHADER_NV:
            return "RS_VK_ERROR_INVALID_SHADER_NV";
        case VK_RESULT_MAX_ENUM:
            return "RS_VK_RESULT_MAX_ENUM";
        case VK_ERROR_INVALID_EXTERNAL_HANDLE:
            return "RS_VK_ERROR_INVALID_EXTERNAL_HANDLE";
        case VK_ERROR_OUT_OF_POOL_MEMORY:
            return "RS_VK_ERROR_OUT_OF_POOL_MEMORY";
        default:
            return "RS_Unknown Error";
    }
    return "";
}

} // namespace OHOS::Rosen::vulkan
