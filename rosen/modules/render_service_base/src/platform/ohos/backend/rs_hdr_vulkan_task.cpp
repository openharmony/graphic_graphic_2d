/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "hetero_hdr/rs_hdr_pattern_manager.h"
#include "platform/ohos/backend/rs_vulkan_context.h"
#include "rs_hdr_vulkan_task.h"

namespace OHOS {
namespace Rosen {
void RSHDRVulkanTask::InsertHTSWaitSemaphore(std::shared_ptr<Drawing::Surface> surfaceDrawing, uint64_t frameId)
{
    auto mhcEventId = RSHDRPatternManager::Instance().MHCGetVulkanTaskWaitEvent(frameId, MHC_PATTERN_TASK_HDR_GPU);
    if (mhcEventId == 0) {
        RS_LOGE("Error, waitEvent should not be 0!");
        return;
    }
    VkDevice vkDevice = RsVulkanContext::GetSingleton().GetDevice();

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreInfo.flags = 0;

    VkSemaphoreExtTypeCreateInfoHUAWEI semaphoreExtTypeCreateInfoHUAWEI = {};
    semaphoreExtTypeCreateInfoHUAWEI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_EXT_CREATE_INFO_HUAWEI;
    semaphoreExtTypeCreateInfoHUAWEI.pNext = NULL;

    VkSemaphore waitSemaphore = {};
    semaphoreExtTypeCreateInfoHUAWEI.semaphoreExtType = VK_SEMAPHORE_EXT_TYPE_FFTS_HUAWEI;
    semaphoreExtTypeCreateInfoHUAWEI.eventId = mhcEventId;
    semaphoreInfo.pNext = &semaphoreExtTypeCreateInfoHUAWEI;
    vkCreateSemaphore(vkDevice, &semaphoreInfo, nullptr, &waitSemaphore);

    surfaceDrawing->Wait(1, waitSemaphore);
}

bool RSHDRVulkanTask::GetHTSNotifySemaphore(VkSemaphore &notifySemaphore, uint64_t frameId)
{
    auto notifyEvent = RSHDRPatternManager::Instance().MHCGetVulkanTaskNotifyEvent(frameId, MHC_PATTERN_TASK_HDR_GPU);
    if (notifyEvent == 0) {
        RS_LOGE("Error, notifyEvent should not be 0!");
        return false;
    }
    VkDevice vkDevice = RsVulkanContext::GetSingleton().GetDevice();

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreInfo.flags = 0;

    VkSemaphoreExtTypeCreateInfoHUAWEI semaphoreExtTypeCreateInfoHUAWEI = {};
    semaphoreExtTypeCreateInfoHUAWEI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_EXT_CREATE_INFO_HUAWEI;
    semaphoreExtTypeCreateInfoHUAWEI.pNext = NULL;

    semaphoreExtTypeCreateInfoHUAWEI.semaphoreExtType = VK_SEMAPHORE_EXT_TYPE_FFTS_HUAWEI;
    semaphoreExtTypeCreateInfoHUAWEI.eventId = notifyEvent;
    semaphoreInfo.pNext = &semaphoreExtTypeCreateInfoHUAWEI;
    vkCreateSemaphore(vkDevice, &semaphoreInfo, nullptr, &notifySemaphore);
    return true;
}

void RSHDRVulkanTask::SubmitWaitEventToGPU(uint64_t frameId)
{
    RSHDRPatternManager::Instance().MHCSubmitVulkanTask(frameId, MHC_PATTERN_TASK_HDR_GPU, nullptr, [frameId]() {
        RSHDRPatternManager::Instance().MHCReleaseEGraph(frameId);
    });
    return;
}

}  // namespace Rosen
}  // namespace OHOS