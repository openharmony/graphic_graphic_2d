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

#ifndef RS_VULKAN_CONTEXT_H
#define RS_VULKAN_CONTEXT_H

#include <memory>
#include <mutex>
#include <string>
#include "include/gpu/vk/GrVkExtensions.h"
#include "vulkan/vulkan_core.h"

#define VK_NO_PROTOTYPES 1

#include "vulkan/vulkan.h"
#include "include/gpu/vk/GrVkBackendContext.h"
#include "include/gpu/GrDirectContext.h"

#include "image/gpu_context.h"

namespace OHOS {
namespace Rosen {
class MemoryHandler;
class RsVulkanContext {
public:
    template <class T>
    class Func {
    public:
        using Proto = T;
        explicit Func(T proc = nullptr) : func_(proc) {}
        ~Func() { func_ = nullptr; }

        Func operator=(T proc)
        {
            func_ = proc;
            return *this;
        }

        Func operator=(PFN_vkVoidFunction proc)
        {
            func_ = reinterpret_cast<Proto>(proc);
            return *this;
        }

        operator bool() const { return func_ != nullptr; }
        operator T() const { return func_; }
    private:
        T func_;
    };

    static RsVulkanContext& GetSingleton();

    RsVulkanContext();
    ~RsVulkanContext();
    bool CreateInstance();
    bool SelectPhysicalDevice();
    bool CreateDevice();
    bool CreateSkiaBackendContext(GrVkBackendContext* context, bool createNew = false);

    bool IsValid() const;
    GrVkGetProc CreateSkiaGetProc() const;
    const std::shared_ptr<MemoryHandler> GetMemoryHandler() const
    {
        return memHandler_;
    }

#define DEFINE_FUNC(name) Func<PFN_vk##name> vk##name

    DEFINE_FUNC(AcquireNextImageKHR);
    DEFINE_FUNC(AllocateCommandBuffers);
    DEFINE_FUNC(AllocateMemory);
    DEFINE_FUNC(BeginCommandBuffer);
    DEFINE_FUNC(BindImageMemory);
    DEFINE_FUNC(BindImageMemory2);
    DEFINE_FUNC(CmdPipelineBarrier);
    DEFINE_FUNC(CreateCommandPool);
    DEFINE_FUNC(CreateDebugReportCallbackEXT);
    DEFINE_FUNC(CreateDevice);
    DEFINE_FUNC(CreateFence);
    DEFINE_FUNC(CreateImage);
    DEFINE_FUNC(CreateImageView);
    DEFINE_FUNC(CreateInstance);
    DEFINE_FUNC(CreateSemaphore);
    DEFINE_FUNC(CreateSwapchainKHR);
    DEFINE_FUNC(DestroyCommandPool);
    DEFINE_FUNC(DestroyDebugReportCallbackEXT);
    DEFINE_FUNC(DestroyDevice);
    DEFINE_FUNC(DestroyFence);
    DEFINE_FUNC(DestroyImage);
    DEFINE_FUNC(DestroyImageView);
    DEFINE_FUNC(DestroyInstance);
    DEFINE_FUNC(DestroySemaphore);
    DEFINE_FUNC(DestroySurfaceKHR);
    DEFINE_FUNC(DestroySwapchainKHR);
    DEFINE_FUNC(DeviceWaitIdle);
    DEFINE_FUNC(EndCommandBuffer);
    DEFINE_FUNC(EnumerateDeviceLayerProperties);
    DEFINE_FUNC(EnumerateInstanceExtensionProperties);
    DEFINE_FUNC(EnumerateInstanceLayerProperties);
    DEFINE_FUNC(EnumeratePhysicalDevices);
    DEFINE_FUNC(FreeCommandBuffers);
    DEFINE_FUNC(FreeMemory);
    DEFINE_FUNC(GetDeviceProcAddr);
    DEFINE_FUNC(GetDeviceQueue);
    DEFINE_FUNC(GetImageMemoryRequirements);
    DEFINE_FUNC(GetInstanceProcAddr);
    DEFINE_FUNC(GetPhysicalDeviceFeatures);
    DEFINE_FUNC(GetPhysicalDeviceQueueFamilyProperties);
    DEFINE_FUNC(QueueSubmit);
    DEFINE_FUNC(QueueWaitIdle);
    DEFINE_FUNC(ResetCommandBuffer);
    DEFINE_FUNC(ResetFences);
    DEFINE_FUNC(WaitForFences);
    DEFINE_FUNC(GetPhysicalDeviceSurfaceCapabilitiesKHR);
    DEFINE_FUNC(GetPhysicalDeviceSurfaceFormatsKHR);
    DEFINE_FUNC(GetPhysicalDeviceSurfacePresentModesKHR);
    DEFINE_FUNC(GetPhysicalDeviceSurfaceSupportKHR);
    DEFINE_FUNC(GetSwapchainImagesKHR);
    DEFINE_FUNC(QueuePresentKHR);
    DEFINE_FUNC(CreateSurfaceOHOS);
    DEFINE_FUNC(GetPhysicalDeviceMemoryProperties);
    DEFINE_FUNC(GetPhysicalDeviceMemoryProperties2);
    DEFINE_FUNC(GetNativeBufferPropertiesOHOS);
    DEFINE_FUNC(QueueSignalReleaseImageOHOS);
    DEFINE_FUNC(ImportSemaphoreFdKHR);
    DEFINE_FUNC(GetPhysicalDeviceFeatures2);
#undef DEFINE_FUNC

    VkPhysicalDevice GetPhysicalDevice() const
    {
        return physicalDevice_;
    }

    VkDevice GetDevice() const
    {
        return device_;
    }

    VkQueue GetQueue() const
    {
        return queue_;
    }

    inline const GrVkBackendContext& GetGrVkBackendContext() const noexcept
    {
        return backendContext_;
    }

    inline const std::string GetVulkanVersion() const
    {
        return std::to_string(VK_API_VERSION_1_2);
    }

    std::shared_ptr<Drawing::GPUContext> CreateDrawingContext(bool independentContext = false);
    std::shared_ptr<Drawing::GPUContext> GetDrawingContext();

    static VKAPI_ATTR VkResult HookedVkQueueSubmit(VkQueue queue, uint32_t submitCount,
        const VkSubmitInfo* pSubmits, VkFence fence);

    static VKAPI_ATTR VkResult HookedVkQueueSignalReleaseImageOHOS(VkQueue queue, uint32_t waitSemaphoreCount,
        const VkSemaphore* pWaitSemaphores, VkImage image, int32_t* pNativeFenceFd);
    std::shared_ptr<Drawing::GPUContext> GetHardWareGrContext() const
    {
        return hcontext_;
    }

    VkQueue GetHardwareQueue() const
    {
        return hbackendContext_.fQueue;
    }
private:
    std::mutex vkMutex_;
    std::mutex graphicsQueueMutex_;
    void* handle_;
    bool acquiredMandatoryProcAddresses_;
    VkInstance instance_ = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice_ = VK_NULL_HANDLE;
    uint32_t graphicsQueueFamilyIndex_ = UINT32_MAX;
    VkDevice device_ = VK_NULL_HANDLE;
    VkQueue hardwareQueue_ = VK_NULL_HANDLE;
    VkQueue queue_ = VK_NULL_HANDLE;
    VkPhysicalDeviceFeatures2 physicalDeviceFeatures2_;
    VkPhysicalDeviceSamplerYcbcrConversionFeatures ycbcrFeature_;
    GrVkExtensions skVkExtensions_;
    static thread_local std::shared_ptr<Drawing::GPUContext> drawingContext_;
    std::shared_ptr<Drawing::GPUContext> hcontext_ = nullptr;
    // static thread_local GrVkBackendContext backendContext_;
    GrVkBackendContext backendContext_;
    GrVkBackendContext hbackendContext_;

    RsVulkanContext(const RsVulkanContext &) = delete;
    RsVulkanContext &operator=(const RsVulkanContext &) = delete;

    RsVulkanContext(RsVulkanContext &&) = delete;
    RsVulkanContext &operator=(RsVulkanContext &&) = delete;

    bool OpenLibraryHandle();
    bool SetupLoaderProcAddresses();
    bool CloseLibraryHandle();
    bool SetupDeviceProcAddresses(VkDevice device);
    PFN_vkVoidFunction AcquireProc(
        const char* proc_name,
        const VkInstance& instance) const;
    PFN_vkVoidFunction AcquireProc(const char* proc_name, const VkDevice& device) const;
    std::shared_ptr<Drawing::GPUContext> CreateNewDrawingContext();
    std::shared_ptr<MemoryHandler> memHandler_;
};

} // namespace Rosen
} // namespace OHOS

#endif