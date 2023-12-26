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

#ifndef RS_VULKAN_VULKAN_WINDOW_H_
#define RS_VULKAN_VULKAN_WINDOW_H_

#include <memory>
#include <native_window.h>
#include <thread>
#include <tuple>
#include <utility>
#include <vector>

#include "include/core/SkRefCnt.h"
#include "include/core/SkSize.h"
#include "include/core/SkSurface.h"
#include "include/gpu/GrDirectContext.h"
#include "include/gpu/vk/GrVkBackendContext.h"
#include "vulkan_proc_table.h"

namespace OHOS::Rosen::vulkan {

class RSVulkanNativeSurface;
class RSVulkanDevice;
class RSVulkanSurface;
class RSVulkanSwapchain;
class RSVulkanImage;
class RSVulkanApplication;
class RSVulkanBackbuffer;

class RSVulkanWindow {
public:
    typedef std::shared_ptr<RSVulkanWindow> Ptr;
    explicit RSVulkanWindow(std::unique_ptr<RSVulkanNativeSurface> nativeSurface, bool isOffscreen = false);

    ~RSVulkanWindow();

    bool IsValid() const;

    GrDirectContext* GetSkiaGrContext();
    GrVkBackendContext& GetSkiaBackendContext();

    sk_sp<SkSurface> AcquireSurface(int bufferCount = -1);

    bool SwapBuffers();

    bool FlushCommands();
    static void PresentAll();
    static void InitializeVulkan(size_t threadNum = 0);
    static bool WaitForSharedFence();
    static bool ResetSharedFence();
    static VkDevice GetDevice();
    static VkPhysicalDevice GetPhysicalDevice();
    static RSVulkanProcTable& GetVkProcTable();

public:
    static RSVulkanProcTable* vk;
    static std::unique_ptr<RSVulkanApplication> application;
    static std::unique_ptr<RSVulkanDevice> logicalDevice;
    static std::thread::id deviceThread;
    static std::vector<RSVulkanHandle<VkFence>> sharedFences;
    static uint32_t sharedFenceIndex;
    static bool presenting_;
    bool valid_ = false;
    bool isOffscreen_ = false;
    std::unique_ptr<RSVulkanSurface> surface_;
    std::unique_ptr<RSVulkanSwapchain> swapchain_;
    sk_sp<GrDirectContext> skiaGrContext_;

    GrVkBackendContext skBackendContext_;

    bool CreateSkiaGrContext();

    bool CreateSkiaBackendContext(GrVkBackendContext* context);

    bool RecreateSwapchain();
};

} // namespace OHOS::Rosen::vulkan

#endif // RS_VULKAN_VULKAN_WINDOW_H_
