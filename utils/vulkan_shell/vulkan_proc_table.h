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

#ifndef RS_VULKAN_VULKAN_PROC_TABLE_H_
#define RS_VULKAN_VULKAN_PROC_TABLE_H_

#include "include/core/SkRefCnt.h"
#include "include/gpu/vk/GrVkBackendContext.h"
#include "vulkan_handle.h"
#include "vulkan_interface.h"

namespace OHOS::Rosen::vulkan {

class RSVulkanProcTable {
public:
    template<class T>
    class RSProc {
    public:
        using Proto = T;

        explicit RSProc(T proc = nullptr) : proc_(proc) {}

        ~RSProc()
        {
            proc_ = nullptr;
        }

        RSProc operator=(T proc)
        {
            proc_ = proc;
            return *this;
        }

        RSProc operator=(PFN_vkVoidFunction proc)
        {
            proc_ = reinterpret_cast<Proto>(proc);
            return *this;
        }

        operator bool() const
        {
            return proc_ != nullptr;
        }

        operator T() const
        {
            return proc_;
        }

    private:
        T proc_;
    };

    RSVulkanProcTable();

    ~RSVulkanProcTable();

    bool HasAcquiredMandatoryProcAddresses() const;

    bool IsValid() const;

    bool AreInstanceProcsSetup() const;

    bool AreDeviceProcsSetup() const;

    bool SetupInstanceProcAddresses(const RSVulkanHandle<VkInstance>& instance);

    bool SetupDeviceProcAddresses(const RSVulkanHandle<VkDevice>& device);

    GrVkGetProc CreateSkiaGetProc() const;

#define VK_DEFINE_PROC(name) RSProc<PFN_vk##name> (name)

    VK_DEFINE_PROC(AcquireNextImageKHR);
    VK_DEFINE_PROC(AllocateCommandBuffers);
    VK_DEFINE_PROC(AllocateMemory);
    VK_DEFINE_PROC(BeginCommandBuffer);
    VK_DEFINE_PROC(BindImageMemory);
    VK_DEFINE_PROC(BindImageMemory2);
    VK_DEFINE_PROC(CmdPipelineBarrier);
    VK_DEFINE_PROC(CreateCommandPool);
    VK_DEFINE_PROC(CreateDebugReportCallbackEXT);
    VK_DEFINE_PROC(CreateDevice);
    VK_DEFINE_PROC(CreateFence);
    VK_DEFINE_PROC(CreateImage);
    VK_DEFINE_PROC(CreateImageView);
    VK_DEFINE_PROC(CreateInstance);
    VK_DEFINE_PROC(CreateSemaphore);
    VK_DEFINE_PROC(CreateSwapchainKHR);
    VK_DEFINE_PROC(DestroyCommandPool);
    VK_DEFINE_PROC(DestroyDebugReportCallbackEXT);
    VK_DEFINE_PROC(DestroyDevice);
    VK_DEFINE_PROC(DestroyFence);
    VK_DEFINE_PROC(DestroyImage);
    VK_DEFINE_PROC(DestroyInstance);
    VK_DEFINE_PROC(DestroySemaphore);
    VK_DEFINE_PROC(DestroySurfaceKHR);
    VK_DEFINE_PROC(DestroySwapchainKHR);
    VK_DEFINE_PROC(DeviceWaitIdle);
    VK_DEFINE_PROC(EndCommandBuffer);
    VK_DEFINE_PROC(EnumerateDeviceLayerProperties);
    VK_DEFINE_PROC(EnumerateInstanceExtensionProperties);
    VK_DEFINE_PROC(EnumerateInstanceLayerProperties);
    VK_DEFINE_PROC(EnumeratePhysicalDevices);
    VK_DEFINE_PROC(FreeCommandBuffers);
    VK_DEFINE_PROC(FreeMemory);
    VK_DEFINE_PROC(GetDeviceProcAddr);
    VK_DEFINE_PROC(GetDeviceQueue);
    VK_DEFINE_PROC(GetImageMemoryRequirements);
    VK_DEFINE_PROC(GetInstanceProcAddr);
    VK_DEFINE_PROC(GetPhysicalDeviceFeatures);
    VK_DEFINE_PROC(GetPhysicalDeviceQueueFamilyProperties);
    VK_DEFINE_PROC(QueueSubmit);
    VK_DEFINE_PROC(QueueWaitIdle);
    VK_DEFINE_PROC(ResetCommandBuffer);
    VK_DEFINE_PROC(ResetFences);
    VK_DEFINE_PROC(WaitForFences);
    VK_DEFINE_PROC(GetPhysicalDeviceSurfaceCapabilitiesKHR);
    VK_DEFINE_PROC(GetPhysicalDeviceSurfaceFormatsKHR);
    VK_DEFINE_PROC(GetPhysicalDeviceSurfacePresentModesKHR);
    VK_DEFINE_PROC(GetPhysicalDeviceSurfaceSupportKHR);
    VK_DEFINE_PROC(GetSwapchainImagesKHR);
    VK_DEFINE_PROC(QueuePresentKHR);
    VK_DEFINE_PROC(CreateSurfaceOHOS);
    VK_DEFINE_PROC(GetPhysicalDeviceMemoryProperties);
    VK_DEFINE_PROC(GetPhysicalDeviceMemoryProperties2);
    VK_DEFINE_PROC(GetNativeBufferPropertiesOHOS);
    VK_DEFINE_PROC(QueueSignalReleaseImageOHOS);

#undef VK_DEFINE_PROC

private:
    void* vkHandle_ = nullptr;
    bool acquiredMandatoryProcAddresses_ = false;
    RSVulkanHandle<VkInstance> instance_ = VK_NULL_HANDLE;
    RSVulkanHandle<VkDevice> device_ = VK_NULL_HANDLE;

    bool OpenLibraryHandle();
    bool SetupLoaderProcAddresses();
    bool CloseLibraryHandle();
    PFN_vkVoidFunction AcquireProc(const char* procName, const RSVulkanHandle<VkInstance>& instance) const;
    PFN_vkVoidFunction AcquireProc(const char* procName, const RSVulkanHandle<VkDevice>& device) const;
};

} // namespace OHOS::Rosen::vulkan

#endif // RS_VULKAN_VULKAN_PROC_TABLE_H_
