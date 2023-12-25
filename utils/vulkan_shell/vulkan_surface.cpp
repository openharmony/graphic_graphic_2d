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

#include "vulkan_surface.h"

#include "vulkan_application.h"
#include "vulkan_hilog.h"
#include "vulkan_native_surface.h"

namespace OHOS::Rosen::vulkan {

RSVulkanSurface::RSVulkanSurface(
    RSVulkanProcTable& procVk, RSVulkanApplication& application, std::unique_ptr<RSVulkanNativeSurface> nativeSurface)
    : vk(procVk), application_(application), nativeSurface_(std::move(nativeSurface)), valid_(false)
{
    if (nativeSurface_ == nullptr || !nativeSurface_->IsValid()) {
        LOGE("Native surface was invalid.");
        return;
    }

    VkSurfaceKHR surface = nativeSurface_->CreateSurfaceHandle(vk, application.GetInstance());
    if (surface == VK_NULL_HANDLE) {
        LOGE("Could not create the surface handle.");
        return;
    }

    surface_ = { surface,
        [this](VkSurfaceKHR surface) { vk.DestroySurfaceKHR(application_.GetInstance(), surface, nullptr); } };

    valid_ = true;
}

RSVulkanSurface::~RSVulkanSurface() = default;

bool RSVulkanSurface::IsValid() const
{
    return valid_;
}

const RSVulkanHandle<VkSurfaceKHR>& RSVulkanSurface::Handle() const
{
    return surface_;
}

const RSVulkanNativeSurface& RSVulkanSurface::GetNativeSurface() const
{
    return *nativeSurface_;
}

SkISize RSVulkanSurface::GetSize() const
{
    return valid_ ? nativeSurface_->GetSize() : SkISize::Make(0, 0);
}

} // namespace OHOS::Rosen::vulkan
