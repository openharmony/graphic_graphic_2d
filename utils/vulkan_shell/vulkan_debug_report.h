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

#ifndef RS_VULKAN_VULKAN_DEBUG_REPORT_H_
#define RS_VULKAN_VULKAN_DEBUG_REPORT_H_

#include "vulkan_handle.h"
#include "vulkan_interface.h"
#include "vulkan_proc_table.h"

namespace OHOS::Rosen::vulkan {

class RSVulkanDebugReport {
public:
    static std::string DebugExtensionName();

    RSVulkanDebugReport(const RSVulkanProcTable& vk, const RSVulkanHandle<VkInstance>& application);

    ~RSVulkanDebugReport();

    bool IsValid() const;

private:
    const RSVulkanProcTable& vk;
    const RSVulkanHandle<VkInstance>& application_;
    RSVulkanHandle<VkDebugReportCallbackEXT> handle_ = VK_NULL_HANDLE;
    bool valid_ = false;
};

} // namespace OHOS::Rosen::vulkan

#endif // RS_VULKAN_VULKAN_DEBUG_REPORT_H_
