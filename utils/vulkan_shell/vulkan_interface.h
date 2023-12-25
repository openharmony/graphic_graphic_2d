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

#ifndef RS_VULKAN_VULKAN_INTERFACE_H_
#define RS_VULKAN_VULKAN_INTERFACE_H_

#include <string>

#ifndef VK_USE_PLATFORM_OHOS
#define VK_USE_PLATFORM_OHOS
#endif // VK_USE_PLATFORM_OHOS

#if !VULKAN_LINK_STATICALLY
#define VK_NO_PROTOTYPES 1
#endif // !VULKAN_LINK_STATICALLY

#include "vulkan/vulkan.h"

#ifndef NDEBUG

#define VK_CALL_LOG_ERROR(expression)                                                                      \
    ({                                                                                                     \
        __typeof__(expression) _rc = (expression);                                                         \
        if (_rc != VK_SUCCESS) {                                                                           \
            LOGE("Vulkan call '" #expression "' failed with error %s", vulkan::VulkanResultToString(_rc)); \
        }                                                                                                  \
        _rc;                                                                                               \
    })

#else // NDEBUG

#define VK_CALL_LOG_ERROR(expression) (expression)

#endif // NDEBUG

namespace OHOS::Rosen::vulkan {

std::string VulkanResultToString(VkResult result);

} // namespace OHOS::Rosen::vulkan

#endif // RS_VULKAN_VULKAN_INTERFACE_H_
