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

#ifndef RS_VULKAN_VULKAN_SWAPCHAIN_H_
#define RS_VULKAN_VULKAN_SWAPCHAIN_H_

#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

#include "include/core/SkSize.h"
#include "include/core/SkSurface.h"
#include "vulkan_handle.h"

namespace OHOS::Rosen::vulkan {

class RSVulkanProcTable;
class RSVulkanDevice;
class RSVulkanSurface;
class RSVulkanBackbuffer;
class RSVulkanImage;

class RSVulkanSwapchain {
public:
    RSVulkanSwapchain(const RSVulkanProcTable& vk, const RSVulkanDevice& device, const RSVulkanSurface& surface,
        GrDirectContext* skiaContext, std::unique_ptr<RSVulkanSwapchain> oldSwapchain, uint32_t queueFamilyIndex);

    ~RSVulkanSwapchain();

    bool IsValid() const;

    enum class AcquireStatus {
        SUCCESS,
        ERROR_SURFACE_LOST,
        ERROR_SURFACE_OUT_OF_DATE,
    };

    using AcquireResult = std::pair<AcquireStatus, sk_sp<SkSurface>>;

    AcquireResult AcquireSurface(int bufferCount = -1);

    /// Submit a previously acquired. There must not be consecutive calls to
    /// |Submit| without and interleaving |AcquireFrame|.
    bool Submit();

    SkISize GetSize() const;

    bool FlushCommands();

    void AddToPresent();

    static void PresentAll(RSVulkanHandle<VkFence>& sharedFence);

private:
    const RSVulkanProcTable& vk;
    const RSVulkanDevice& device_;
    VkSurfaceCapabilitiesKHR capabilities_;
    VkSurfaceFormatKHR surfaceFormat_;
    RSVulkanHandle<VkSwapchainKHR> swapchain_;
    std::vector<std::unique_ptr<RSVulkanBackbuffer>> backbuffers_;
    std::vector<std::unique_ptr<RSVulkanImage>> images_;
    std::vector<sk_sp<SkSurface>> surfaces_;
    VkPipelineStageFlagBits currentPipelineStage_;
    size_t currentBackbufferIndex_ = 0;
    size_t currentImageIndex_ = 0;
    bool valid_ = false;
    static std::mutex mapMutex_;
    static std::unordered_map<std::thread::id, RSVulkanSwapchain*> toBePresent_;

    std::vector<VkImage> GetImages() const;

    bool CreateSwapchainImages(GrDirectContext* skiaContext, SkColorType colorType, sk_sp<SkColorSpace> colorSpace);

    sk_sp<SkSurface> CreateSkiaSurface(GrDirectContext* skiaContext, VkImage image, const SkISize& size,
        SkColorType colorType, sk_sp<SkColorSpace> colorSpace) const;

    RSVulkanBackbuffer* GetNextBackbuffer();
};

} // namespace OHOS::Rosen::vulkan

#endif // RS_VULKAN_VULKAN_SWAPCHAIN_H_
