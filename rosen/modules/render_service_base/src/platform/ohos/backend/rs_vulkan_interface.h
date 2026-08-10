/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef RS_VULKAN_INTERFACE_H
#define RS_VULKAN_INTERFACE_H

#include <atomic>
#include <list>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "image/gpu_context.h"
#include "include/gpu/ganesh/GrDirectContext.h"
#include "include/gpu/vk/VulkanBackendContext.h"
#include "include/gpu/vk/VulkanExtensions.h"
#include "platform/ohos/backend/rs_engine_header_ext.h"
#include "platform/ohos/backend/rs_vulkan_header_ext.h"
#include "rs_vulkan_mem_statistic.h"
#include "sync_fence.h"
#include "vulkan/vulkan.h"
#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_xeg.h"

#define VK_NO_PROTOTYPES 1

namespace OHOS {
namespace Rosen {

/**
 * RsVulkanInterface - Vulkan API封装层
 * 负责Vulkan实例、设备、队列的创建和管理
 *
 * 重构要点：
 * 1. 移除HybridRender相关代码
 * 2. 移除CreateDrawingContext能力（移至RenderContextVK）
 * 3. 只保留M133 Skia版本
 * 4. 移除MemoryHandler相关代码
 */
class RsVulkanInterface {
public:
    // CallbackSemaphoreInfo - Semaphore回调信息结构
    struct CallbackSemaphoreInfo {
        std::shared_ptr<RsVulkanInterface> mVkInterface;
        VkSemaphore mSemaphore;
        int mFenceFd;

        int mRefs = 2;         // Skia和RS各持有一个fence fd
        int mRSRefs = 1;       // RS持有的fence fd
        int m2DEngineRefs = 1; // 2D引擎(Skia/DDGR)持有的fence fd

        CallbackSemaphoreInfo(std::shared_ptr<RsVulkanInterface> vkInterface, VkSemaphore semaphore, int fenceFd)
            : mVkInterface(vkInterface), mSemaphore(semaphore), mFenceFd(fenceFd)
        {}

        static void DestroyCallbackRefs(void* context);
        static void DestroyCallbackRefsFromRS(void* context);
        static void DestroyCallbackRefsFrom2DEngine(void* context);
        static void DestroyCallbackRefsInner(CallbackSemaphoreInfo* info);
    };

    // Func - Vulkan函数指针封装模板
    template<class T>
    class Func {
    public:
        using Proto = T;
        explicit Func(T proc = nullptr) : func_(proc) {}
        ~Func()
        {
            func_ = nullptr;
        }

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

        operator bool() const
        {
            return func_ != nullptr;
        }
        operator T() const
        {
            return func_;
        }

    private:
        T func_;
    };

    explicit RsVulkanInterface(bool isProtected = false, bool isHtsEnable = false,
        RenderEngineType type = RenderEngineType::BASIC_RENDER);
    ~RsVulkanInterface();

    // Vulkan核心对象创建
    bool CreateInstance();
    bool SelectPhysicalDevice();
    bool CreateDevice(bool isProtected = false, bool isHtsEnable = false);

    // 状态查询
    bool IsValid() const;
    VulkanDeviceStatus GetVulkanDeviceStatus();
    RsVulkanMemStat& GetRsVkMemStat()
    {
        return mVkMemStat;
    }

    // Skia GetProc（M133版本）
    skgpu::VulkanGetProc CreateSkiaGetProc() const;

    // Vulkan对象获取
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

    // 用于CreateSkiaBackendContext的数据访问
    uint32_t GetGraphicsQueueFamilyIndex() const
    {
        return graphicsQueueFamilyIndex_;
    }
    const VkPhysicalDeviceFeatures2& GetPhysicalDeviceFeatures2() const
    {
        return physicalDeviceFeatures2_;
    }
    const std::vector<const char*>& GetDeviceExtensions() const
    {
        return deviceExtensions_;
    }
    static VkInstance GetInstance()
    {
        return instance_;
    }
    static const std::vector<const char*>& GetInstanceExtensions();

    inline const std::string GetVulkanVersion() const
    {
        return std::to_string(VK_API_VERSION_1_2);
    }

    // Semaphore管理
    VkSemaphore RequireSemaphore();
    VkSemaphore RequireTimelineSemaphore();
    void SendSemaphoreWithFd(VkSemaphore semaphore, int fenceFd);
    void DestroyAllSemaphoreFence();

    // Vulkan函数指针定义
#define DEFINE_FUNC(name) Func<PFN_vk##name> vk##name
    DEFINE_FUNC(AllocateMemory);
    DEFINE_FUNC(BindImageMemory);
    DEFINE_FUNC(BindImageMemory2);
    DEFINE_FUNC(CreateDevice);
    DEFINE_FUNC(CreateImage);
    DEFINE_FUNC(CreateInstance);
    DEFINE_FUNC(CreateSemaphore);
    DEFINE_FUNC(DestroyDevice);
    DEFINE_FUNC(DestroyImage);
    DEFINE_FUNC(DestroySemaphore);
    DEFINE_FUNC(DeviceWaitIdle);
    DEFINE_FUNC(EnumerateDeviceExtensionProperties);
    DEFINE_FUNC(EnumerateInstanceExtensionProperties);
    DEFINE_FUNC(EnumeratePhysicalDevices);
    DEFINE_FUNC(FreeMemory);
    DEFINE_FUNC(GetDeviceProcAddr);
    DEFINE_FUNC(GetImageMemoryRequirements);
    DEFINE_FUNC(GetInstanceProcAddr);
    DEFINE_FUNC(GetNativeBufferPropertiesOHOS);
    DEFINE_FUNC(GetPhysicalDeviceFeatures2);
    DEFINE_FUNC(GetPhysicalDeviceMemoryProperties);
    DEFINE_FUNC(GetPhysicalDeviceMemoryProperties2);
    DEFINE_FUNC(GetPhysicalDeviceQueueFamilyProperties);
    DEFINE_FUNC(GetSemaphoreFdKHR);
    DEFINE_FUNC(ImportSemaphoreFdKHR);
    DEFINE_FUNC(QueueSignalReleaseImageOHOS);
    DEFINE_FUNC(QueueSubmit);
#undef DEFINE_FUNC

    // 统计计数器
    static std::atomic<uint64_t> callbackSemaphoreInfofdDupCnt_;
    static std::atomic<uint64_t> callbackSemaphoreInfoRSDerefCnt_;
    static std::atomic<uint64_t> callbackSemaphoreInfo2DEngineDerefCnt_;
    static std::atomic<uint64_t> callbackSemaphoreInfo2DEngineDefensiveDerefCnt_;
    static std::atomic<uint64_t> callbackSemaphoreInfoFlushCnt_;
    static std::atomic<uint64_t> callbackSemaphoreInfo2DEngineCallCnt_;

    bool IsProtected()
    {
        return isProtected_;
    }

    RenderEngineType GetInterfaceType()
    {
        return type_;
    }

    bool CreateSkiaBackendContext(bool isProtected);
    bool QueryMaxGpuBufferSize(uint32_t& maxWidth, uint32_t& maxHeight);

    VkResult QueueSignalReleaseImageOHOS(VkQueue queue, uint32_t waitSemaphoreCount,
        const VkSemaphore* pWaitSemaphores, VkImage image, int32_t* pNativeFenceFd);

    // hook 函数
    static VKAPI_ATTR VkResult HookedVkQueueSubmitForBasicRender(VkQueue queue, uint32_t submitCount,
        VkSubmitInfo* pSubmits, VkFence fence);
    static VKAPI_ATTR VkResult HookedVkQueueSubmitForUnprotectedRedraw(VkQueue queue, uint32_t submitCount,
        VkSubmitInfo* pSubmits, VkFence fence);
    static VKAPI_ATTR VkResult HookedVkQueueSubmitForProtectedRedraw(VkQueue queue, uint32_t submitCount,
        VkSubmitInfo* pSubmits, VkFence fence);

private:
    std::mutex graphicsQueueMutex_;

    RenderEngineType type_ = RenderEngineType::BASIC_RENDER;
    void* handle_;
    bool acquiredMandatoryProcAddresses_ = false;
    static VkInstance instance_;
    VkPhysicalDevice physicalDevice_ = VK_NULL_HANDLE;
    uint32_t graphicsQueueFamilyIndex_ = UINT32_MAX;
    VkDevice device_ = VK_NULL_HANDLE;
    VkQueue queue_ = VK_NULL_HANDLE;
    static const int grChunkSize_ = 1048576;

    // Vulkan特性链
    VkPhysicalDeviceFeatures2 physicalDeviceFeatures2_;
    VkPhysicalDeviceProtectedMemoryFeatures* protectedMemoryFeatures_ = nullptr;
    VkPhysicalDeviceSamplerYcbcrConversionFeatures ycbcrFeature_;
    VkPhysicalDeviceSynchronization2Features sync2Feature_;
    VkPhysicalDeviceDescriptorIndexingFeatures bindlessFeature_;
    VkPhysicalDeviceTimelineSemaphoreFeatures timelineFeature_;

    std::vector<const char*> deviceExtensions_;
    VkDeviceMemoryExclusiveThresholdHUAWEI deviceMemoryExclusiveThreshold_;

    RsVulkanMemStat mVkMemStat;
    std::atomic<VulkanDeviceStatus> deviceStatus_ = VulkanDeviceStatus::UNINITIALIZED;

    // Semaphore/Fence管理
    struct semaphoreFence {
        VkSemaphore semaphore;
        std::unique_ptr<SyncFence> fence;
    };
    std::list<semaphoreFence> usedSemaphoreFenceList_;
    std::mutex semaphoreLock_;

    // 禁止复制和移动
    RsVulkanInterface(const RsVulkanInterface&) = delete;
    RsVulkanInterface& operator=(const RsVulkanInterface&) = delete;
    RsVulkanInterface(RsVulkanInterface&&) = delete;
    RsVulkanInterface& operator=(RsVulkanInterface&&) = delete;

    // 内部方法
    void SetVulkanDeviceStatus(VulkanDeviceStatus status);
    bool OpenLibraryHandle();
    bool CloseLibraryHandle();
    bool SetupLoaderProcAddresses();
    bool SetupDeviceProcAddresses(VkDevice device);
    void ConfigureFeatures(bool isProtected);
    void ConfigureExtensions();
    PFN_vkVoidFunction AcquireProc(const char* proc_name, const VkInstance& instance) const;
    PFN_vkVoidFunction AcquireProc(const char* proc_name, const VkDevice& device) const;

    skgpu::VulkanBackendContext backendContext_;
    skgpu::VulkanExtensions skVkExtensions_;
    bool isProtected_;

    friend class RsVulkanContext;
};

} // namespace Rosen
} // namespace OHOS

#endif