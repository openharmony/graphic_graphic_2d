
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

#include "platform/ohos/backend/rs_vulkan_context.h"

namespace OHOS {
namespace Rosen {

class RSHDRVulkanTask {
public:
    static void InsertHTSWaitSemaphore(std::shared_ptr<Drawing::Surface> surfaceDrawing, uint64_t frameId);
    static bool GetHTSNotifySemaphore(VkSemaphore &notifySemaphore, uint64_t frameId);
    static void SubmitWaitEventToGPU(uint64_t frameId);
};

} // namespace Rosen
} // namespace OHOS

#endif