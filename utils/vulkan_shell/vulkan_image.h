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

#ifndef RS_VULKAN_VULKAN_IMAGE_H_
#define RS_VULKAN_VULKAN_IMAGE_H_

#include "vulkan_handle.h"

namespace OHOS::Rosen::vulkan {

class RSVulkanProcTable;
class RSVulkanCommandBuffer;

class RSVulkanImage {
public:
    explicit RSVulkanImage(RSVulkanHandle<VkImage> image);

    ~RSVulkanImage();

    bool IsValid() const;

    bool InsertImageMemoryBarrier(const RSVulkanCommandBuffer& commandBuffer, VkPipelineStageFlagBits srcPiplineBits,
        VkPipelineStageFlagBits destPiplineBits, VkAccessFlagBits destAccessFlags, VkImageLayout destLayout);

private:
    RSVulkanHandle<VkImage> handle_;
    VkImageLayout layout_;
    uint32_t accessFlags_ = 0;
    bool valid_ = false;
};

} // namespace OHOS::Rosen::vulkan

#endif // RS_VULKAN_VULKAN_IMAGE_H_
