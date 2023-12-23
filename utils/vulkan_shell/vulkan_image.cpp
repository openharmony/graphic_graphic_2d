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

#include "vulkan_image.h"

#include "vulkan_command_buffer.h"
#include "vulkan_proc_table.h"

namespace OHOS::Rosen::vulkan {

RSVulkanImage::RSVulkanImage(RSVulkanHandle<VkImage> image)
    : handle_(std::move(image)), layout_(VK_IMAGE_LAYOUT_UNDEFINED), accessFlags_(0), valid_(false)
{
    if (!handle_) {
        return;
    }

    valid_ = true;
}

RSVulkanImage::~RSVulkanImage() = default;

bool RSVulkanImage::IsValid() const
{
    return valid_;
}

bool RSVulkanImage::InsertImageMemoryBarrier(const RSVulkanCommandBuffer& commandBuffer,
    VkPipelineStageFlagBits srcPiplineBits, VkPipelineStageFlagBits destPiplineBits,
    VkAccessFlagBits destAccessFlags, VkImageLayout destLayout)
{
    const VkImageMemoryBarrier imageMemoryBarrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .pNext = nullptr,
        .srcAccessMask = accessFlags_,
        .dstAccessMask = destAccessFlags,
        .oldLayout = layout_,
        .newLayout = destLayout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = handle_,
        .subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 },
    };

    bool success = commandBuffer.InsertPipelineBarrier(srcPiplineBits, destPiplineBits, 0, 0, nullptr, 0, nullptr, 1,
        &imageMemoryBarrier);
    if (success) {
        accessFlags_ = destAccessFlags;
        layout_ = destLayout;
    }

    return success;
}

} // namespace OHOS::Rosen::vulkan
