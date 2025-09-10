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

#include "hetero_hdr/rs_hdr_vulkan_task.h"

#include <memory>
#include <unordered_map>

#ifdef RS_ENABLE_VK

namespace OHOS {
namespace Rosen {
namespace {
std::unordered_map<uint16_t, std::shared_ptr<VkSemaphore>> g_semaphoreMap{};
std::mutex g_vulkanTaskMutex;
}

void RSHDRVulkanTask::InsertHTSWaitSemaphore(std::shared_ptr<Drawing::Surface>& surfaceDrawing, uint64_t frameId)
{
    auto MHCEventId = RSHDRPatternManager::Instance().MHCGetVulkanTaskWaitEvent(frameId, MHC_PATTERN_TASK_HDR_GPU);
    if (MHCEventId == 0) {
        RS_LOGE("[hdrHetero]:RSHDRVulkanTask InsertHTSWaitSemaphore waitEvent should not be 0");
        return;
    }
    VkDevice vkDevice = RsVulkanContext::GetSingleton().GetDevice();

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreInfo.flags = 0;

    VkSemaphoreExtTypeCreateInfoHUAWEI semaphoreExtTypeCreateInfoHUAWEI = {};
    semaphoreExtTypeCreateInfoHUAWEI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_EXT_CREATE_INFO;
    semaphoreExtTypeCreateInfoHUAWEI.pNext = NULL;

    VkSemaphore waitSemaphore = {};
    semaphoreExtTypeCreateInfoHUAWEI.semaphoreExtType = VK_SEMAPHORE_EXT_TYPE_FFTS;
    semaphoreExtTypeCreateInfoHUAWEI.eventId = MHCEventId;
    semaphoreInfo.pNext = &semaphoreExtTypeCreateInfoHUAWEI;
    auto err = vkCreateSemaphore(vkDevice, &semaphoreInfo, nullptr, &waitSemaphore);
    if (err != VK_SUCCESS) {
        RS_LOGE("[hdrHetero]:RSHDRVulkanTask InsertHTSWaitSemaphore waitSemaphore create failed");
        return;
    }
    surfaceDrawing->Wait(1, waitSemaphore); // 2D Engine will destroy semaphore
}

bool RSHDRVulkanTask::GetHTSNotifySemaphore(std::shared_ptr<VkSemaphore>& notifySemaphore, uint64_t frameId)
{
    auto notifyEvent = RSHDRPatternManager::Instance().MHCGetVulkanTaskNotifyEvent(frameId, MHC_PATTERN_TASK_HDR_GPU);
    if (notifyEvent == 0) {
        RS_LOGE("[hdrHetero]:RSHDRVulkanTask GetHTSNotifySemaphore notifyEvent should not be 0");
        return false;
    }
    std::lock_guard<std::mutex> lockGuard(g_vulkanTaskMutex);
    if (g_semaphoreMap.find(notifyEvent) == g_semaphoreMap.end()) {
        VkDevice vkDevice = RsVulkanContext::GetSingleton().GetDevice();
        VkSemaphoreCreateInfo semaphoreInfo = {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        semaphoreInfo.flags = 0;

        VkSemaphoreExtTypeCreateInfoHUAWEI semaphoreExtTypeCreateInfoHUAWEI = {};
        semaphoreExtTypeCreateInfoHUAWEI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_EXT_CREATE_INFO;
        semaphoreExtTypeCreateInfoHUAWEI.pNext = NULL;

        VkSemaphore notifySemaphore = {};
        semaphoreExtTypeCreateInfoHUAWEI.semaphoreExtType = VK_SEMAPHORE_EXT_TYPE_FFTS;
        semaphoreExtTypeCreateInfoHUAWEI.eventId = notifyEvent;
        semaphoreInfo.pNext = &semaphoreExtTypeCreateInfoHUAWEI;
        auto err = vkCreateSemaphore(vkDevice, &semaphoreInfo, nullptr, &notifySemaphore);
        if (err != VK_SUCCESS) {
            RS_LOGE("[hdrHetero]:RSHDRVulkanTask GetHTSNotifySemaphore waitSemaphore create failed");
            return false;
        }
        VkSemaphore* vkInnerSemaphore = new VkSemaphore(notifySemaphore);
        auto sharedSemaphore = std::make_shared<VkSemaphore>(vkInnerSemaphore,
            [vkDevice](VkSemaphore* semaphore) {
                vkDestroySemaphore(vkDevice, *semaphore, nullptr);
                delete semaphore;
                semaphore = nullptr;
            });
        g_semaphoreMap.insert_or_assign(notifyEvent, sharedSemaphore);
    }
    notifySemaphore = g_semaphoreMap[notifyEvent];
    return true;
}

void RSHDRVulkanTask::SubmitWaitEventToGPU(uint64_t frameId)
{
    RSHDRPatternManager::Instance().MHCSubmitVulkanTask(frameId, MHC_PATTERN_TASK_HDR_GPU, nullptr, [frameId]() {
        RSHDRPatternManager::Instance().MHCReleaseEGraph(frameId);
    });
}

void RSHDRVulkanTask::PrepareHDRSemaphoreVector(std::vector<GrBackendSemaphore>& semaphoreVec,
    std::shared_ptr<Drawing::Surface>& surface, std::vector<uint64_t>& frameIdVec)
{
    std::shared_ptr<VkSemaphore> notifySemaphorePtr;
    for (auto frameId : frameIdVec) {
        if (RSHDRVulkanTask::GetHTSNotifySemaphore(notifySemaphorePtr, frameId)) {
#ifdef USE_M133_SKIA
            GrBackendSemaphore htsSemaphore = GrBackendSemaphores::MakeVk(*notifySemaphorePtr);
#else
            GrBackendSemaphore htsSemaphore;
            htsSemaphore.initVulkan(*notifySemaphorePtr);
#endif
            semaphoreVec.emplace_back(std::move(htsSemaphore));
        }
        RSHDRVulkanTask::InsertHTSWaitSemaphore(surface, frameId);
    }
}
} // namespace Rosen
} // namespace OHOS

#endif