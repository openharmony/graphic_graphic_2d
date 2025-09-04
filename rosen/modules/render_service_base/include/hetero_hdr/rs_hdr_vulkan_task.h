/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#ifndef RS_HDR_VULKAN_TASK_H
#define RS_HDR_VULKAN_TASK_H

#ifdef RS_ENABLE_VK
#ifdef USE_M133_SKIA
#include "include/gpu/ganesh/vk/GrVkBackendSemaphore.h"
#include "include/gpu/ganesh/vk/GrVkBackendSurface.h"
#else
#include "include/gpu/GrBackendSurface.h"
#endif // USE_M133_SKIA
#include "hetero_hdr/rs_hdr_pattern_manager.h"
#include "platform/ohos/backend/native_buffer_utils.h"
#include "platform/ohos/backend/rs_surface_ohos_vulkan.h"
#include "platform/ohos/backend/rs_vulkan_context.h"

namespace OHOS {
namespace Rosen {

class RSB_EXPORT RSHDRVulkanTask {
public:
    static void InsertHTSWaitSemaphore(std::shared_ptr<Drawing::Surface>& surfaceDrawing, uint64_t frameId);
    static bool GetHTSNotifySemaphore(std::shared_ptr<VkSemaphore>& notifySemaphore, uint64_t frameId);
    static void SubmitWaitEventToGPU(uint64_t frameId);
    static void PrepareHDRSemaphoreVector(std::vector<GrBackendSemaphore>& semaphoreVec,
        std::shared_ptr<Drawing::Surface>& surface, std::vector<uint64_t>& frameIdVec);

private:
    RSHDRVulkanTask() = default;
    ~RSHDRVulkanTask() = default;
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_ENABLE_VK
#endif // RS_HDR_VULKAN_TASK_H