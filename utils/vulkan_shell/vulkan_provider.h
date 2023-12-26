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

#ifndef RS_VULKAN_VULKAN_PROVIDER_H_
#define RS_VULKAN_VULKAN_PROVIDER_H_

#include "vulkan_handle.h"
#include "vulkan_proc_table.h"

namespace OHOS::Rosen::vulkan {

class RSVulkanProvider {
public:
    virtual const vulkan::RSVulkanProcTable& vk() = 0;
    virtual const vulkan::RSVulkanHandle<VkDevice>& vk_device() = 0;

    vulkan::RSVulkanHandle<VkFence> CreateFence()
    {
        const VkFenceCreateInfo create_info = {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
        };
        VkFence fence;
        if (VK_CALL_LOG_ERROR(vk().CreateFence(vk_device(), &create_info, nullptr, &fence)) != VK_SUCCESS)
            return vulkan::RSVulkanHandle<VkFence>();

        return { fence, [this](VkFence fence) { vk().DestroyFence(vk_device(), fence, nullptr); } };
    }
};

} // namespace OHOS::Rosen::vulkan

#endif // RS_VULKAN_VULKAN_PROVIDER_H_
