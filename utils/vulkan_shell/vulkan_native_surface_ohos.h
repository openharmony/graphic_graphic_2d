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

#ifndef RS_VULKAN_VULKAN_NATIVE_SURFACE_OHOS_H_
#define RS_VULKAN_VULKAN_NATIVE_SURFACE_OHOS_H_

#include "vulkan_native_surface.h"
#include "window.h"

namespace OHOS::Rosen::vulkan {

class RSVulkanNativeSurfaceOHOS : public RSVulkanNativeSurface {
public:
    /// Create a native surface from the valid NativeWindow reference. Ownership
    /// of the NativeWindow is assumed by this instance.
    explicit RSVulkanNativeSurfaceOHOS(struct NativeWindow* nativeWindow);

    ~RSVulkanNativeSurfaceOHOS();

    const char* GetExtensionName() const override;

    uint32_t GetSkiaExtensionName() const override;

    VkSurfaceKHR CreateSurfaceHandle(RSVulkanProcTable& vk, const RSVulkanHandle<VkInstance>& instance) const override;

    bool IsValid() const override;

    SkISize GetSize() const override;

private:
    struct NativeWindow* nativeWindow_;
};

} // namespace OHOS::Rosen::vulkan

#endif // RS_VULKAN_VULKAN_NATIVE_SURFACE_OHOS_H_
