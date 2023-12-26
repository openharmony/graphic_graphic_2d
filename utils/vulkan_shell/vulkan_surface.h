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

#ifndef RS_VULKAN_VULKAN_SURFACE_H_
#define RS_VULKAN_VULKAN_SURFACE_H_

#include "include/core/SkSize.h"
#include "vulkan_handle.h"

namespace OHOS::Rosen::vulkan {

class RSVulkanProcTable;
class RSVulkanApplication;
class RSVulkanNativeSurface;

class RSVulkanSurface {
public:
    RSVulkanSurface(
        RSVulkanProcTable& vk, RSVulkanApplication& application, std::unique_ptr<RSVulkanNativeSurface> nativeSurface);

    ~RSVulkanSurface();

    bool IsValid() const;

    /// Returns the current size of the surface or (0, 0) if invalid.
    SkISize GetSize() const;

    const RSVulkanHandle<VkSurfaceKHR>& Handle() const;

    const RSVulkanNativeSurface& GetNativeSurface() const;

private:
    RSVulkanProcTable& vk;
    RSVulkanApplication& application_;
    std::unique_ptr<RSVulkanNativeSurface> nativeSurface_;
    RSVulkanHandle<VkSurfaceKHR> surface_;
    bool valid_ = false;
};

} // namespace OHOS::Rosen::vulkan

#endif // RS_VULKAN_VULKAN_SURFACE_H_
