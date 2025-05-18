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

#include <list>
#include <memory>
#include <mutex>
#include <string>
#include "sync_fence.h"
#include "include/gpu/vk/GrVkExtensions.h"
#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_xeg.h"

#define VK_NO_PROTOTYPES 1

#include "vulkan/vulkan.h"
#include "rs_vulkan_mem_statistic.h"

#include "image/gpu_context.h"

#ifdef USE_M133_SKIA
#include "include/gpu/vk/VulkanExtensions.h"
#include "include/gpu/vk/VulkanBackendContext.h"
#include "include/gpu/ganesh/GrDirectContext.h"
#else
#include "include/gpu/vk/GrVkExtensions.h"
#include "include/gpu/vk/GrVkBackendContext.h"
#include "include/gpu/GrDirectContext.h"
#endif

namespace OHOS {
namespace Rosen {
enum class VulkanInterfaceType : uint32_t {
    UNI_RENDER = 0,
    PROTECTED_REDRAW,
    UNPROTECTED_REDRAW,
    MAX_INTERFACE_TYPE,
};
class MemoryHandler;
class RsVulkanInterface {
public:
    struct CallbackSemaphoreInfo {
        RsVulkanInterface& mVkContext;
        VkSemaphore mSemaphore;
        int mFenceFd;
        
        int mRefs = 2; // 2 : both skia and rs hold fence fd
        CallbackSemaphoreInfo(RsVulkanInterface& vkContext, VkSemaphore semaphore, int fenceFd)
            : mVkContext(vkContext),
            mSemaphore(semaphore),
            mFenceFd(fenceFd)
        {
        }

        static void DestroyCallbackRefs(void* context)
        {
            if (context == nullptr) {
                return;
            }
            CallbackSemaphoreInfo* info = reinterpret_cast<CallbackSemaphoreInfo*>(context);
            --info->mRefs;
            if (!info->mRefs) {
                info->mVkContext.SendSemaphoreWithFd(info->mSemaphore, info->mFenceFd);
                delete info;
            }
        }
    };
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

    RsVulkanInterface() {};
    ~RsVulkanInterface();
    void Init(VulkanInterfaceType vulkanInterfaceType, bool isProtected = false);
    bool CreateInstance();
    bool SelectPhysicalDevice(bool isProtected = false);
    bool CreateDevice(bool isProtected = false);
#ifdef USE_M133_SKIA
    bool CreateSkiaBackendContext(skgpu::VulkanBackendContext* context, bool isProtected = false);
#else
    bool CreateSkiaBackendContext(GrVkBackendContext* context, bool isProtected = false);
#endif
    RsVulkanMemStat& GetRsVkMemStat()
    {
        return mVkMemStat;
    }

    bool IsValid() const;
#ifdef USE_M133_SKIA
    skgpu::VulkanGetProc CreateSkiaGetProc() const;
#else
    GrVkGetProc CreateSkiaGetProc() const;
#endif
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
    DEFINE_FUNC(SetFreqAdjustEnable);
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
        return backendContext_.fQueue;
    }

#ifdef USE_M133_SKIA
    inline const skgpu::VulkanBackendContext& GetGrVkBackendContext() const noexcept
#else
    inline const GrVkBackendContext& GetGrVkBackendContext() const noexcept
#endif
    {
        return backendContext_;
    }

    inline const std::string GetVulkanVersion() const
    {
        return std::to_string(VK_API_VERSION_1_2);
    }

    std::shared_ptr<Drawing::GPUContext> CreateDrawingContext(std::string cacheDir = "");
    std::shared_ptr<Drawing::GPUContext> GetDrawingContext();

    VulkanInterfaceType GetInterfaceType() const
    {
        return interfaceType_;
    }

    VkSemaphore RequireSemaphore();
    void SendSemaphoreWithFd(VkSemaphore semaphore, int fenceFd);
    void DestroyAllSemaphoreFence();

friend class RsVulkanContext;
private:
    std::mutex vkMutex_;
    std::mutex graphicsQueueMutex_;
    std::mutex hGraphicsQueueMutex_;
    static void* handle_;
    bool acquiredMandatoryProcAddresses_ = false;
    VkInstance instance_ = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice_ = VK_NULL_HANDLE;
    uint32_t graphicsQueueFamilyIndex_ = UINT32_MAX;
    VkDevice device_ = VK_NULL_HANDLE;
    VkQueue queue_ = VK_NULL_HANDLE;
    VkPhysicalDeviceFeatures2 physicalDeviceFeatures2_;
    VkPhysicalDeviceProtectedMemoryFeatures* protectedMemoryFeatures_ = nullptr;
    VkPhysicalDeviceSamplerYcbcrConversionFeatures ycbcrFeature_;
#ifdef USE_M133_SKIA
    skgpu::VulkanExtensions skVkExtensions_;
    skgpu::VulkanBackendContext backendContext_;
#else
    GrVkExtensions skVkExtensions_;
    GrVkBackendContext backendContext_;
#endif
    RsVulkanMemStat mVkMemStat;

    // static thread_local GrVkBackendContext backendContext_;
    VulkanInterfaceType interfaceType_ = VulkanInterfaceType::UNI_RENDER;
    RsVulkanInterface(const RsVulkanInterface &) = delete;
    RsVulkanInterface &operator=(const RsVulkanInterface &) = delete;

    RsVulkanInterface(RsVulkanInterface &&) = delete;
    RsVulkanInterface &operator=(RsVulkanInterface &&) = delete;

    bool OpenLibraryHandle();
    bool SetupLoaderProcAddresses();
    bool CloseLibraryHandle();
    bool SetupDeviceProcAddresses(VkDevice device);
    PFN_vkVoidFunction AcquireProc(
        const char* proc_name,
        const VkInstance& instance) const;
    PFN_vkVoidFunction AcquireProc(const char* proc_name, const VkDevice& device) const;
    std::shared_ptr<Drawing::GPUContext> CreateNewDrawingContext(bool isProtected = false);
    std::shared_ptr<MemoryHandler> memHandler_;

    struct semaphoreFence {
        VkSemaphore semaphore;
        std::unique_ptr<SyncFence> fence;
    };
    std::list<semaphoreFence> usedSemaphoreFenceList_;
    std::mutex semaphoreLock_;
};

class RsVulkanContext {
public:
    static RsVulkanContext& GetSingleton(const std::string& cacheDir = "");
    explicit RsVulkanContext(std::string cacheDir = "");
    ~RsVulkanContext() {};

    RsVulkanContext(const RsVulkanContext&) = delete;
    RsVulkanContext &operator=(const RsVulkanContext&) = delete;

    RsVulkanContext(const RsVulkanContext&&) = delete;
    RsVulkanContext &operator=(const RsVulkanContext&&) = delete;

    void SetIsProtected(bool isProtected);

    RsVulkanInterface& GetRsVulkanInterface();

    bool IsValid()
    {
        return GetRsVulkanInterface().IsValid();
    }

#ifdef USE_M133_SKIA
    skgpu::VulkanGetProc CreateSkiaGetProc()
#else
    GrVkGetProc CreateSkiaGetProc()
#endif
    {
        return GetRsVulkanInterface().CreateSkiaGetProc();
    }

    RsVulkanMemStat& GetRsVkMemStat()
    {
        return GetRsVulkanInterface().GetRsVkMemStat();
    }

    VkPhysicalDevice GetPhysicalDevice()
    {
        return GetRsVulkanInterface().GetPhysicalDevice();
    }

    VkDevice GetDevice()
    {
        return GetRsVulkanInterface().GetDevice();
    }

    VkQueue GetQueue()
    {
        return GetRsVulkanInterface().GetQueue();
    }

#ifdef USE_M133_SKIA
    inline const skgpu::VulkanBackendContext& GetGrVkBackendContext() noexcept
#else
    inline const GrVkBackendContext& GetGrVkBackendContext() noexcept
#endif
    {
        return GetRsVulkanInterface().GetGrVkBackendContext();
    }

    inline const std::string GetVulkanVersion()
    {
        return std::to_string(VK_API_VERSION_1_2);
    }

    std::shared_ptr<Drawing::GPUContext> CreateDrawingContext();
    std::shared_ptr<Drawing::GPUContext> GetDrawingContext();

    void ClearGrContext(bool isProtected = false);

    static VKAPI_ATTR VkResult HookedVkQueueSubmit(VkQueue queue, uint32_t submitCount,
        VkSubmitInfo* pSubmits, VkFence fence);

    static VKAPI_ATTR VkResult HookedVkQueueSignalReleaseImageOHOS(VkQueue queue, uint32_t waitSemaphoreCount,
        const VkSemaphore* pWaitSemaphores, VkImage image, int32_t* pNativeFenceFd);

    const std::shared_ptr<MemoryHandler> GetMemoryHandler()
    {
        return GetRsVulkanInterface().GetMemoryHandler();
    }

    bool GetIsProtected() const
    {
        return isProtected_;
    }

private:
    static thread_local bool isProtected_;
    static thread_local VulkanInterfaceType vulkanInterfaceType_;
    std::vector<std::shared_ptr<RsVulkanInterface>> vulkanInterfaceVec;
    static thread_local std::shared_ptr<Drawing::GPUContext> drawingContext_;
    static thread_local std::shared_ptr<Drawing::GPUContext> protectedDrawingContext_;
};

} // namespace Rosen
} // namespace OHOS

#endif
