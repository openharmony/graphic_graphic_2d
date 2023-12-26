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

#ifndef RS_VULKAN_VULKAN_NATIVE_SURFACE_H_
#define RS_VULKAN_VULKAN_NATIVE_SURFACE_H_

#include "include/core/SkSize.h"
#include "vulkan_handle.h"
#include "vulkan_proc_table.h"

namespace OHOS::Rosen::vulkan {

class RSVulkanNativeSurface {
public:
    virtual ~RSVulkanNativeSurface() = default;

    virtual const char* GetExtensionName() const = 0;

    virtual uint32_t GetSkiaExtensionName() const = 0;

    virtual VkSurfaceKHR CreateSurfaceHandle(
        RSVulkanProcTable& vk, const RSVulkanHandle<VkInstance>& instance) const = 0;

    virtual bool IsValid() const = 0;

    virtual SkISize GetSize() const = 0;
};

} // namespace OHOS::Rosen::vulkan

#endif // RS_VULKAN_VULKAN_NATIVE_SURFACE_H_
