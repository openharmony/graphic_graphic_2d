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

#include "vulkan_proc_table.h"

#include <dlfcn.h>

#include "vulkan_hilog.h"

#define ACQUIRE_PROC(name, context)                   \
    if (!((name) = AcquireProc("vk" #name, (context)))) { \
        LOGE("Could not acquire proc: vk" #name);     \
    }

namespace OHOS::Rosen::vulkan {

RSVulkanProcTable::RSVulkanProcTable() : vkHandle_(nullptr), acquiredMandatoryProcAddresses_(false)
{
    acquiredMandatoryProcAddresses_ = OpenLibraryHandle() && SetupLoaderProcAddresses();
}

RSVulkanProcTable::~RSVulkanProcTable()
{
    CloseLibraryHandle();
}

bool RSVulkanProcTable::HasAcquiredMandatoryProcAddresses() const
{
    return acquiredMandatoryProcAddresses_;
}

bool RSVulkanProcTable::IsValid() const
{
    return instance_ && device_;
}

bool RSVulkanProcTable::AreInstanceProcsSetup() const
{
    return instance_;
}

bool RSVulkanProcTable::AreDeviceProcsSetup() const
{
    return device_;
}

bool RSVulkanProcTable::SetupLoaderProcAddresses()
{
    if (!vkHandle_) {
        return true;
    }

    GetInstanceProcAddr =
#if VULKAN_LINK_STATICALLY
        &vkGetInstanceProcAddr;
#else
        reinterpret_cast<PFN_vkGetInstanceProcAddr>(dlsym(vkHandle_, "vkGetInstanceProcAddr"));
    GetDeviceProcAddr = reinterpret_cast<PFN_vkGetDeviceProcAddr>(dlsym(vkHandle_, "vkGetDeviceProcAddr"));
    EnumerateInstanceExtensionProperties = reinterpret_cast<PFN_vkEnumerateInstanceExtensionProperties>(
        dlsym(vkHandle_, "vkEnumerateInstanceExtensionProperties"));
    CreateInstance = reinterpret_cast<PFN_vkCreateInstance>(dlsym(vkHandle_, "vkCreateInstance"));
#endif

    if (!GetInstanceProcAddr) {
        LOGE("Could not acquire vkGetInstanceProcAddr.");
        return false;
    }

    RSVulkanHandle<VkInstance> null_instance(VK_NULL_HANDLE, nullptr);

    ACQUIRE_PROC(EnumerateInstanceLayerProperties, null_instance);

    return true;
}

bool RSVulkanProcTable::SetupInstanceProcAddresses(const RSVulkanHandle<VkInstance>& vkHandle)
{
    ACQUIRE_PROC(CreateDevice, vkHandle);
    ACQUIRE_PROC(DestroyDevice, vkHandle);
    ACQUIRE_PROC(DestroyInstance, vkHandle);
    ACQUIRE_PROC(EnumerateDeviceLayerProperties, vkHandle);
    ACQUIRE_PROC(EnumeratePhysicalDevices, vkHandle);
    ACQUIRE_PROC(GetPhysicalDeviceFeatures, vkHandle);
    ACQUIRE_PROC(GetPhysicalDeviceQueueFamilyProperties, vkHandle);
    ACQUIRE_PROC(GetPhysicalDeviceSurfaceCapabilitiesKHR, vkHandle);
    ACQUIRE_PROC(GetPhysicalDeviceSurfaceFormatsKHR, vkHandle);
    ACQUIRE_PROC(GetPhysicalDeviceSurfacePresentModesKHR, vkHandle);
    ACQUIRE_PROC(GetPhysicalDeviceSurfaceSupportKHR, vkHandle);
    ACQUIRE_PROC(DestroySurfaceKHR, vkHandle);
    ACQUIRE_PROC(CreateSurfaceOHOS, vkHandle);
    ACQUIRE_PROC(GetPhysicalDeviceMemoryProperties, vkHandle);
    ACQUIRE_PROC(GetPhysicalDeviceMemoryProperties2, vkHandle);

    // The debug report functions are optional. We don't want proc acquisition to
    // fail here because the optional methods were not present (since ACQUIRE_PROC
    // returns false on failure). Wrap the optional proc acquisitions in an
    // anonymous lambda and invoke it. We don't really care about the result since
    // users of Debug reporting functions check for their presence explicitly.

    instance_ = { vkHandle, nullptr };
    return true;
}

bool RSVulkanProcTable::SetupDeviceProcAddresses(const RSVulkanHandle<VkDevice>& vkHandle)
{
    ACQUIRE_PROC(AllocateCommandBuffers, vkHandle);
    ACQUIRE_PROC(AllocateMemory, vkHandle);
    ACQUIRE_PROC(BeginCommandBuffer, vkHandle);
    ACQUIRE_PROC(BindImageMemory, vkHandle);
    ACQUIRE_PROC(BindImageMemory2, vkHandle);
    ACQUIRE_PROC(CmdPipelineBarrier, vkHandle);
    ACQUIRE_PROC(CreateCommandPool, vkHandle);
    ACQUIRE_PROC(CreateFence, vkHandle);
    ACQUIRE_PROC(CreateImage, vkHandle);
    ACQUIRE_PROC(CreateImageView, vkHandle);
    ACQUIRE_PROC(CreateSemaphore, vkHandle);
    ACQUIRE_PROC(DestroyCommandPool, vkHandle);
    ACQUIRE_PROC(DestroyFence, vkHandle);
    ACQUIRE_PROC(DestroyImage, vkHandle);
    ACQUIRE_PROC(DestroySemaphore, vkHandle);
    ACQUIRE_PROC(DeviceWaitIdle, vkHandle);
    ACQUIRE_PROC(EndCommandBuffer, vkHandle);
    ACQUIRE_PROC(FreeCommandBuffers, vkHandle);
    ACQUIRE_PROC(FreeMemory, vkHandle);
    ACQUIRE_PROC(GetDeviceQueue, vkHandle);
    ACQUIRE_PROC(GetImageMemoryRequirements, vkHandle);
    ACQUIRE_PROC(QueueSubmit, vkHandle);
    ACQUIRE_PROC(QueueWaitIdle, vkHandle);
    ACQUIRE_PROC(ResetCommandBuffer, vkHandle);
    ACQUIRE_PROC(ResetFences, vkHandle);
    ACQUIRE_PROC(WaitForFences, vkHandle);
    ACQUIRE_PROC(AcquireNextImageKHR, vkHandle);
    ACQUIRE_PROC(CreateSwapchainKHR, vkHandle);
    ACQUIRE_PROC(DestroySwapchainKHR, vkHandle);
    ACQUIRE_PROC(GetSwapchainImagesKHR, vkHandle);
    ACQUIRE_PROC(QueuePresentKHR, vkHandle);
    ACQUIRE_PROC(GetNativeBufferPropertiesOHOS, vkHandle);
    ACQUIRE_PROC(QueueSignalReleaseImageOHOS, vkHandle);

    device_ = { vkHandle, nullptr };
    return true;
}

bool RSVulkanProcTable::OpenLibraryHandle()
{
    LOGI("VulkanProcTable OpenLibararyHandle.");
#if VULKAN_LINK_STATICALLY
    static char kDummyLibraryHandle = '\0';
    handle_ = reinterpret_cast<decltype(handle_)>(&kDummyLibraryHandle);
    return true;
#else  // VULKAN_LINK_STATICALLY
    LOGI("VulkanProcTable OpenLibararyHandle: dlopen libvulkan.so.");
    dlerror(); // clear existing errors on thread.
    vkHandle_ = dlopen("/system/lib64/libvulkan.so", RTLD_NOW | RTLD_LOCAL);
    if (vkHandle_ == nullptr) {
        LOGE("Could not open the vulkan library: %s", dlerror());
        return false;
    }
    return true;
#endif // VULKAN_LINK_STATICALLY
}

bool RSVulkanProcTable::CloseLibraryHandle()
{
#if VULKAN_LINK_STATICALLY
    handle_ = nullptr;
    return true;
#else
    if (vkHandle_ != nullptr) {
        dlerror(); // clear existing errors on thread.
        if (dlclose(vkHandle_) != 0) {
            LOGE("Could not close the vulkan library handle. This indicates a leak. %s", dlerror());
        }
        vkHandle_ = nullptr;
    }
    return vkHandle_ == nullptr;
#endif
}

PFN_vkVoidFunction RSVulkanProcTable::AcquireProc(
    const char* procName, const RSVulkanHandle<VkInstance>& instance) const
{
    if (procName == nullptr || !GetInstanceProcAddr) {
        return nullptr;
    }

    // A VK_NULL_HANDLE as the instance is an acceptable parameter.
    return GetInstanceProcAddr(instance, procName);
}

PFN_vkVoidFunction RSVulkanProcTable::AcquireProc(const char* procName, const RSVulkanHandle<VkDevice>& device) const
{
    if (procName == nullptr || !device || !GetDeviceProcAddr) {
        return nullptr;
    }

    return GetDeviceProcAddr(device, procName);
}

GrVkGetProc RSVulkanProcTable::CreateSkiaGetProc() const
{
    if (!IsValid()) {
        return nullptr;
    }

    return [this](const char* procName, VkInstance instance, VkDevice device) {
        if (device != VK_NULL_HANDLE) {
            auto result = AcquireProc(procName, { device, nullptr });
            if (result != nullptr) {
                return result;
            }
        }

        return AcquireProc(procName, { instance, nullptr });
    };
}

} // namespace OHOS::Rosen::vulkan
