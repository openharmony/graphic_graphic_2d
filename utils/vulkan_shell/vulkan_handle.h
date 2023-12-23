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

#ifndef RS_VULKAN_VULKAN_HANDLE_H_
#define RS_VULKAN_VULKAN_HANDLE_H_

#include <functional>

#include "vulkan_interface.h"

namespace OHOS::Rosen::vulkan {

template<class T>
class RSVulkanHandle {
public:
    using Handle = T;
    using Disposer = std::function<void(Handle)>;

    RSVulkanHandle() : vkHandle_(VK_NULL_HANDLE) {}

    RSVulkanHandle(Handle handle, Disposer disposer = nullptr) : vkHandle_(handle), vkDisposer_(disposer) {}

    RSVulkanHandle(RSVulkanHandle&& other) : vkHandle_(other.vkHandle_), vkDisposer_(other.vkDisposer_)
    {
        other.vkHandle_ = VK_NULL_HANDLE;
        other.vkDisposer_ = nullptr;
    }

    ~RSVulkanHandle()
    {
        DisposeIfNecessary();
    }

    RSVulkanHandle& operator=(RSVulkanHandle&& other)
    {
        if (vkHandle_ != other.vkHandle_) {
            DisposeIfNecessary();
        }

        vkHandle_ = other.vkHandle_;
        other.vkHandle_ = VK_NULL_HANDLE;
        vkDisposer_ = other.vkDisposer_;
        other.vkDisposer_ = nullptr;
        return *this;
    }

    operator bool() const
    {
        return vkHandle_ != VK_NULL_HANDLE;
    }

    operator Handle() const
    {
        return vkHandle_;
    }

    /// Relinquish responsibility of collecting the underlying handle when this
    /// object is collected. It is the responsibility of the caller to ensure that
    /// the lifetime of the handle extends past the lifetime of this object.
    void ReleaseOwnership()
    {
        vkDisposer_ = nullptr;
    }

    void Reset()
    {
        DisposeIfNecessary();
    }

private:
    Handle vkHandle_ = nullptr;
    Disposer vkDisposer_ = nullptr;

    void DisposeIfNecessary()
    {
        if (vkHandle_ == VK_NULL_HANDLE) {
            return;
        }
        if (vkDisposer_) {
            vkDisposer_(vkHandle_);
        }
        vkHandle_ = VK_NULL_HANDLE;
        vkDisposer_ = nullptr;
    }
};

} // namespace OHOS::Rosen::vulkan

#endif // RS_VULKAN_VULKAN_HANDLE_H_
