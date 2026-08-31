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

/**
 * RsVulkanInterface实现（重构版）
 *
 * 从RsVulkanContext拆分出来的独立文件
 * 只负责Vulkan API封装，不创建GPUContext
 */

#include "vulkan_context/rs_vulkan_interface.h"

#include <dlfcn.h>
#include <memory>
#include <mutex>
#include <vector>

#include "sync_fence.h"
#include "unistd.h"
#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_ohos.h"

#include "common/rs_optional_trace.h"
#include "platform/common/rs_log.h"
#include "utils/system_properties.h"
#include "vulkan_context/rs_vulkan_context.h"

#ifdef HETERO_HDR_ENABLE
#include "rs_hdr_pattern_manager.h"
#endif

#include "include/gpu/vk/VulkanExtensions.h"

#define ACQUIRE_PROC(name, context)                         \
    if (!(vk##name = AcquireProc("vk" #name, context))) {   \
        ROSEN_LOGE("Could not acquire proc: vk" #name);     \
    }

namespace OHOS {
namespace Rosen {

// 静态成员初始化
VkInstance RsVulkanInterface::instance_ = VK_NULL_HANDLE;
std::atomic<uint64_t> RsVulkanInterface::callbackSemaphoreInfofdDupCnt_ = 0;
std::atomic<uint64_t> RsVulkanInterface::callbackSemaphoreInfoRSDerefCnt_ = 0;
std::atomic<uint64_t> RsVulkanInterface::callbackSemaphoreInfo2DEngineDerefCnt_ = 0;
std::atomic<uint64_t> RsVulkanInterface::callbackSemaphoreInfo2DEngineDefensiveDerefCnt_ = 0;
std::atomic<uint64_t> RsVulkanInterface::callbackSemaphoreInfoFlushCnt_ = 0;
std::atomic<uint64_t> RsVulkanInterface::callbackSemaphoreInfo2DEngineCallCnt_ = 0;

// Vulkan扩展配置
static std::vector<const char*> gInstanceExtensions = {
    VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
};

static std::vector<const char*> gMandatoryDeviceExtensions = {
    VK_KHR_EXTERNAL_SEMAPHORE_FD_EXTENSION_NAME,
    VK_KHR_SAMPLER_YCBCR_CONVERSION_EXTENSION_NAME,
    VK_KHR_MAINTENANCE1_EXTENSION_NAME,
    VK_KHR_MAINTENANCE2_EXTENSION_NAME,
    VK_KHR_MAINTENANCE3_EXTENSION_NAME,
    VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME,
    VK_KHR_BIND_MEMORY_2_EXTENSION_NAME,
    VK_OHOS_NATIVE_BUFFER_EXTENSION_NAME,
    VK_OHOS_EXTERNAL_MEMORY_EXTENSION_NAME,
};

static std::vector<const char*> gOptionalDeviceExtensions = {
    VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
    VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME,
    VK_EXT_DEVICE_FAULT_EXTENSION_NAME,
    VK_EXT_PIPELINE_CREATION_CACHE_CONTROL_EXTENSION_NAME,
};

// enabled when persist.sys.graphic.openVkImageMemoryDfx is true
static const std::vector<const char*> gOptionalDeviceExtensionsDebug = {
    VK_KHR_MAINTENANCE_5_EXTENSION_NAME,
    VK_EXT_DEVICE_ADDRESS_BINDING_REPORT_EXTENSION_NAME,
};

// ==================== RsVulkanInterface 实现 ====================

const std::vector<const char*>& RsVulkanInterface::GetInstanceExtensions()
{
    return gInstanceExtensions;
}

RsVulkanInterface::RsVulkanInterface(bool isProtected, bool isHtsEnable, RenderEngineType type)
{
    isProtected_ = isProtected;
    type_ = type;
    acquiredMandatoryProcAddresses_ = OpenLibraryHandle() && SetupLoaderProcAddresses();
    if (!acquiredMandatoryProcAddresses_) {
        ROSEN_LOGE("RsVulkanInterface:: failed to acquire mandatory Vulkan proc addresses");
        return;
    }
    ROSEN_LOGI("RsVulkanInterface:: Init CreateInstance begin for type %{public}d", static_cast<int>(type_));
    if (!CreateInstance()) {
        ROSEN_LOGE("RsVulkanInterface:: CreateInstance failed");
        return;
    }
    ROSEN_LOGI("RsVulkanInterface:: Init SelectPhysicalDevice begin for type %{public}d", static_cast<int>(type_));
    if (!SelectPhysicalDevice()) {
        ROSEN_LOGE("RsVulkanInterface:: SelectPhysicalDevice failed");
        return;
    }
    ROSEN_LOGI("RsVulkanInterface:: Init CreateDevice begin for type %{public}d", static_cast<int>(type_));
    if (!CreateDevice(isProtected, isHtsEnable)) {
        ROSEN_LOGE("RsVulkanInterface:: CreateDevice failed");
        return;
    }
    ROSEN_LOGI("RsVulkanInterface:: Init CreateSkiaBackendContext begin for type %{public}d", static_cast<int>(type_));
    if (!CreateSkiaBackendContext(isProtected)) {
        ROSEN_LOGE("RsVulkanInterface:: CreateSkiaBackendContext failed");
        return;
    }
    ROSEN_LOGI("RsVulkanInterface:: Init success for type %{public}d", static_cast<int>(type_));
}

RsVulkanInterface::~RsVulkanInterface()
{
    CloseLibraryHandle();
}

bool RsVulkanInterface::IsValid() const
{
    return instance_ != VK_NULL_HANDLE && device_ != VK_NULL_HANDLE;
}

VulkanDeviceStatus RsVulkanInterface::GetVulkanDeviceStatus()
{
    return deviceStatus_.load();
}

void RsVulkanInterface::SetVulkanDeviceStatus(VulkanDeviceStatus status)
{
    deviceStatus_ = status;
}

bool RsVulkanInterface::OpenLibraryHandle()
{
    dlerror();
    handle_ = dlopen("libvulkan.so", RTLD_NOW | RTLD_LOCAL);
    if (handle_ == nullptr) {
        ROSEN_LOGE("Could not open the vulkan library: %{public}s", dlerror());
        return false;
    }
    return true;
}

bool RsVulkanInterface::CloseLibraryHandle()
{
    if (handle_ == nullptr) {
        return true;
    }
    dlerror();
    if (dlclose(handle_) != 0) {
        ROSEN_LOGE("Could not close the vulkan lib handle. %{public}s", dlerror());
        return false;
    }
    return true;
}

bool RsVulkanInterface::SetupLoaderProcAddresses()
{
    if (handle_ == nullptr) {
        return false;
    }
    vkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(dlsym(handle_, "vkGetInstanceProcAddr"));
    vkGetDeviceProcAddr = reinterpret_cast<PFN_vkGetDeviceProcAddr>(dlsym(handle_, "vkGetDeviceProcAddr"));
    vkEnumerateInstanceExtensionProperties = reinterpret_cast<PFN_vkEnumerateInstanceExtensionProperties>(
        dlsym(handle_, "vkEnumerateInstanceExtensionProperties"));
    vkCreateInstance = reinterpret_cast<PFN_vkCreateInstance>(dlsym(handle_, "vkCreateInstance"));

    bool isProcNotValid =
        !vkGetInstanceProcAddr || !vkGetDeviceProcAddr || !vkEnumerateInstanceExtensionProperties || !vkCreateInstance;
    if (isProcNotValid) {
        ROSEN_LOGE("Could not acquire proc addresses");
        return false;
    }
    return true;
}

// ==================== Semaphore回调实现 ====================

void RsVulkanInterface::CallbackSemaphoreInfo::DestroyCallbackRefs(void* context)
{
    if (context == nullptr) {
        return;
    }
    CallbackSemaphoreInfo* info = reinterpret_cast<CallbackSemaphoreInfo*>(context);
    --info->mRefs;
    if (info->mRefs <= 0) {
        info->mVkInterface->SendSemaphoreWithFd(info->mSemaphore, info->mFenceFd);
        delete info;
    }
}

void RsVulkanInterface::CallbackSemaphoreInfo::DestroyCallbackRefsFromRS(void* context)
{
    if (context == nullptr) {
        return;
    }
    CallbackSemaphoreInfo* info = reinterpret_cast<CallbackSemaphoreInfo*>(context);
    --info->mRSRefs;
    RsVulkanInterface::callbackSemaphoreInfoRSDerefCnt_.fetch_add(1, std::memory_order_relaxed);
    DestroyCallbackRefsInner(info);
}

void RsVulkanInterface::CallbackSemaphoreInfo::DestroyCallbackRefsFrom2DEngine(void* context)
{
    RsVulkanInterface::callbackSemaphoreInfo2DEngineCallCnt_.fetch_add(1, std::memory_order_relaxed);
    if (context == nullptr) {
        return;
    }
    CallbackSemaphoreInfo* info = reinterpret_cast<CallbackSemaphoreInfo*>(context);
    --info->m2DEngineRefs;
    RsVulkanInterface::callbackSemaphoreInfo2DEngineDerefCnt_.fetch_add(1, std::memory_order_relaxed);
    DestroyCallbackRefsInner(info);
}

void RsVulkanInterface::CallbackSemaphoreInfo::DestroyCallbackRefsInner(CallbackSemaphoreInfo* info)
{
    if (info == nullptr) {
        return;
    }
    if (info->mRSRefs <= 0 && info->m2DEngineRefs <= 0) {
        info->mVkInterface->SendSemaphoreWithFd(info->mSemaphore, info->mFenceFd);
        delete info;
    }
}

// ==================== Semaphore管理实现 ====================

VkSemaphore RsVulkanInterface::RequireSemaphore()
{
    {
        std::lock_guard<std::mutex> lock(semaphoreLock_);
        // 3000 means too many used semaphore fences
        if (usedSemaphoreFenceList_.size() >= 3000) {
            RS_LOGE("Too many used semaphore fences, count [%{public}zu] ", usedSemaphoreFenceList_.size());
            for (auto&& semaphoreFence : usedSemaphoreFenceList_) {
                if (semaphoreFence.fence != nullptr) {
                    semaphoreFence.fence->Wait(-1);
                }
                vkDestroySemaphore(device_, semaphoreFence.semaphore, nullptr);
            }
            usedSemaphoreFenceList_.clear();
        }
        for (auto it = usedSemaphoreFenceList_.begin(); it != usedSemaphoreFenceList_.end();) {
            auto& fence = it->fence;
            if (fence == nullptr || fence->GetStatus() == FenceStatus::SIGNALED) {
                vkDestroySemaphore(device_, it->semaphore, nullptr);
                it->semaphore = VK_NULL_HANDLE;
                it = usedSemaphoreFenceList_.erase(it);
            } else {
                it++;
            }
        }
        // 144000 : print once every 20min at most.
        if (OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::VULKAN &&
            RsVulkanInterface::callbackSemaphoreInfofdDupCnt_.load(std::memory_order_relaxed) % 144000 == 0) {
            RS_LOGI("used fences, surface flush count[%{public}" PRIu64 "],"
                "dup fence count[%{public}" PRIu64 "], rs deref count[%{public}" PRIu64 "],"
                "call 2DEngineDeref count[%{public}" PRIu64 "], 2DEngine deref count[%{public}" PRIu64 "],"
                "Defensive 2DEngine deref count[%{public}" PRIu64 "], wait close fence count[%{public}zu]",
                RsVulkanInterface::callbackSemaphoreInfoFlushCnt_.load(std::memory_order_relaxed),
                RsVulkanInterface::callbackSemaphoreInfofdDupCnt_.load(std::memory_order_relaxed),
                RsVulkanInterface::callbackSemaphoreInfoRSDerefCnt_.load(std::memory_order_relaxed),
                RsVulkanInterface::callbackSemaphoreInfo2DEngineCallCnt_.load(std::memory_order_relaxed),
                RsVulkanInterface::callbackSemaphoreInfo2DEngineDerefCnt_.load(std::memory_order_relaxed),
                RsVulkanInterface::callbackSemaphoreInfo2DEngineDefensiveDerefCnt_.load(std::memory_order_relaxed),
                usedSemaphoreFenceList_.size());
        }
    }

    VkSemaphoreCreateInfo semaphoreInfo;
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreInfo.pNext = nullptr;
    semaphoreInfo.flags = 0;
    VkSemaphore semaphore;
    auto err = vkCreateSemaphore(device_, &semaphoreInfo, nullptr, &semaphore);
    if (err != VK_SUCCESS) {
        return VK_NULL_HANDLE;
    }
    return semaphore;
}

VkSemaphore RsVulkanInterface::RequireTimelineSemaphore()
{
    // Timeline semaphore实现
    VkSemaphore semaphore;
    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkSemaphoreTypeCreateInfo timelineInfo = {};
    timelineInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
    timelineInfo.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
    timelineInfo.initialValue = 0;
    semaphoreInfo.pNext = &timelineInfo;

    if (vkCreateSemaphore(device_, &semaphoreInfo, nullptr, &semaphore) != VK_SUCCESS) {
        ROSEN_LOGE("Failed to create timeline semaphore");
        return VK_NULL_HANDLE;
    }
    return semaphore;
}

void RsVulkanInterface::SendSemaphoreWithFd(VkSemaphore semaphore, int fenceFd)
{
    std::lock_guard<std::mutex> lock(semaphoreLock_);
    auto& semaphoreFence = usedSemaphoreFenceList_.emplace_back();
    semaphoreFence.semaphore = semaphore;
    semaphoreFence.fence = (fenceFd != -1 ? std::make_unique<SyncFence>(fenceFd) : nullptr);
}

void RsVulkanInterface::DestroyAllSemaphoreFence()
{
    std::lock_guard<std::mutex> lock(semaphoreLock_);
    ROSEN_LOGE("Device lost clear all semaphore fences, count [%{public}zu] ", usedSemaphoreFenceList_.size());
    for (auto&& semaphoreFence : usedSemaphoreFenceList_) {
        vkDestroySemaphore(device_, semaphoreFence.semaphore, nullptr);
    }
    usedSemaphoreFenceList_.clear();
}

PFN_vkVoidFunction RsVulkanInterface::AcquireProc(const char* procName, const VkInstance& instance) const
{
    if (procName == nullptr || !vkGetInstanceProcAddr) {
        ROSEN_LOGE("Could not acquire instance proc: %{public}s", procName);
        return nullptr;
    }
    return vkGetInstanceProcAddr(instance, procName);
}

PFN_vkVoidFunction RsVulkanInterface::AcquireProc(const char* procName, const VkDevice& device) const
{
    if (procName == nullptr || !device || !vkGetDeviceProcAddr) {
        ROSEN_LOGE("Could not acquire device proc: %{public}s", procName);
        return nullptr;
    }
    return vkGetDeviceProcAddr(device, procName);
}

// ==================== M133版本专用方法 ====================

skgpu::VulkanGetProc RsVulkanInterface::CreateSkiaGetProc() const
{
    if (!IsValid()) {
        return nullptr;
    }

    return [this](const char* proc_name, VkInstance instance, VkDevice device) {
        if (device == VK_NULL_HANDLE) {
            return reinterpret_cast<PFN_vkVoidFunction>(vkGetInstanceProcAddr(instance, proc_name));
        }
        std::string_view s { proc_name };
        if (s.find("vkQueueSubmit") == 0) {
            if (type_ == RenderEngineType::BASIC_RENDER) {
                return (PFN_vkVoidFunction)RsVulkanInterface::HookedVkQueueSubmitForBasicRender;
            } else if (type_ == RenderEngineType::UNPROTECTED_REDRAW) {
                return (PFN_vkVoidFunction)RsVulkanInterface::HookedVkQueueSubmitForUnprotectedRedraw;
            } else {
                return (PFN_vkVoidFunction)RsVulkanInterface::HookedVkQueueSubmitForProtectedRedraw;
            }
        }
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetDeviceProcAddr(device, proc_name));
    };
}

bool RsVulkanInterface::CreateInstance()
{
    if (!acquiredMandatoryProcAddresses_) {
        ROSEN_LOGE("RsVulkanInterface::CreateInstance: mandatory proc addresses not acquired");
        return false;
    }
    if (instance_ == VK_NULL_HANDLE) {
        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "OpenHarmony RenderService";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "Rosen";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_2;

        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(gInstanceExtensions.size());
        createInfo.ppEnabledExtensionNames = gInstanceExtensions.data();

        VkResult result = vkCreateInstance(&createInfo, nullptr, &instance_);
        if (result != VK_SUCCESS) {
            ROSEN_LOGE("Failed to create Vulkan instance: %{public}d", result);
            return false;
        }
        SetVulkanDeviceStatus(VulkanDeviceStatus::CREATE_SUCCESS);
    }
    ACQUIRE_PROC(CreateDevice, instance_);
    ACQUIRE_PROC(DestroyDevice, instance_);
    ACQUIRE_PROC(EnumerateDeviceExtensionProperties, instance_);
    ACQUIRE_PROC(EnumeratePhysicalDevices, instance_);
    ACQUIRE_PROC(GetPhysicalDeviceQueueFamilyProperties, instance_);
    ACQUIRE_PROC(GetPhysicalDeviceMemoryProperties, instance_);
    ACQUIRE_PROC(GetPhysicalDeviceMemoryProperties2, instance_);
    ACQUIRE_PROC(GetPhysicalDeviceFeatures2, instance_);
    return true;
}

bool RsVulkanInterface::SelectPhysicalDevice()
{
    if (instance_ == VK_NULL_HANDLE) {
        ROSEN_LOGE("RsVulkanInterface::SelectPhysicalDevice: instance is null");
        return false;
    }
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance_, &deviceCount, nullptr);

    if (deviceCount == 0) {
        ROSEN_LOGE("Failed to find GPUs with Vulkan support");
        return false;
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance_, &deviceCount, devices.data());

    // 选择第一个合适的物理设备
    physicalDevice_ = devices[0];

    // 获取图形队列族索引
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice_, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice_, &queueFamilyCount, queueFamilies.data());

    for (uint32_t i = 0; i < queueFamilyCount; i++) {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            graphicsQueueFamilyIndex_ = i;
            break;
        }
    }

    return true;
}

bool RsVulkanInterface::CreateDevice(bool isProtected, bool isHtsEnable)
{
    if (physicalDevice_ == VK_NULL_HANDLE) {
        ROSEN_LOGE("RsVulkanInterface::CreateDevice: physicalDevice_ is null");
        return false;
    }
    if (graphicsQueueFamilyIndex_ == UINT32_MAX) {
        ROSEN_LOGE("graphicsQueueFamilyIndex_ is not valid");
        return false;
    }

    ConfigureExtensions();
    ConfigureFeatures(isProtected);
    vkGetPhysicalDeviceFeatures2(physicalDevice_, &physicalDeviceFeatures2_);

    static constexpr float QUEUE_PRIORITY = 1.0f;
    const VkDeviceQueueCreateFlags deviceQueueCreateFlags = isProtected ? VK_DEVICE_QUEUE_CREATE_PROTECTED_BIT : 0;
    const VkDeviceQueueCreateInfo queueCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, .pNext = nullptr, .flags = deviceQueueCreateFlags,
        .queueFamilyIndex = graphicsQueueFamilyIndex_, .queueCount = 1, .pQueuePriorities = &QUEUE_PRIORITY,
    };

    const VkDeviceCreateFlags deviceCreateFlags = isHtsEnable ? VK_DEVICE_CREATE_HTS_ENABLE_BIT : 0;
    const VkDeviceCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO, .pNext = &physicalDeviceFeatures2_, .flags = deviceCreateFlags,
        .queueCreateInfoCount = 1, .pQueueCreateInfos = &queueCreateInfo, .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr, .enabledExtensionCount = static_cast<uint32_t>(deviceExtensions_.size()),
        .ppEnabledExtensionNames = deviceExtensions_.data(), .pEnabledFeatures = nullptr,
    };
    if (vkCreateDevice(physicalDevice_, &createInfo, nullptr, &device_) != VK_SUCCESS) {
        ROSEN_LOGE("vkCreateDevice failed");
        SetVulkanDeviceStatus(VulkanDeviceStatus::CREATE_FAIL);
        return false;
    }
    SetVulkanDeviceStatus(VulkanDeviceStatus::CREATE_SUCCESS);

    if (!SetupDeviceProcAddresses(device_)) {
        return false;
    }

    const VkDeviceQueueInfo2 deviceQueueInfo2 = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2, .pNext = nullptr, .flags = deviceQueueCreateFlags,
        .queueFamilyIndex = graphicsQueueFamilyIndex_, .queueIndex = 0,
    };
    vkGetDeviceQueue2(device_, &deviceQueueInfo2, &queue_);

    return true;
}

bool RsVulkanInterface::SetupDeviceProcAddresses(VkDevice device)
{
    // 获取设备级别的函数指针
    ACQUIRE_PROC(AllocateMemory, device);
    ACQUIRE_PROC(BindImageMemory, device);
    ACQUIRE_PROC(BindImageMemory2, device);
    ACQUIRE_PROC(CreateImage, device);
    ACQUIRE_PROC(CreateSemaphore, device);
    ACQUIRE_PROC(DestroyImage, device);
    ACQUIRE_PROC(DestroySemaphore, device);
    ACQUIRE_PROC(DeviceWaitIdle, device);
    ACQUIRE_PROC(FreeMemory, device);
    ACQUIRE_PROC(GetImageMemoryRequirements, device);
    ACQUIRE_PROC(GetNativeBufferPropertiesOHOS, device);
    ACQUIRE_PROC(GetSemaphoreFdKHR, device);
    ACQUIRE_PROC(ImportSemaphoreFdKHR, device);
    ACQUIRE_PROC(QueueSignalReleaseImageOHOS, device);
    ACQUIRE_PROC(QueueSubmit, device);
    bool isProcNotValid = !vkAllocateMemory || !vkBindImageMemory || !vkBindImageMemory2 || !vkCreateImage ||
        !vkCreateSemaphore || !vkDestroyImage || !vkDestroySemaphore || !vkDeviceWaitIdle || !vkFreeMemory ||
        !vkGetImageMemoryRequirements || !vkGetNativeBufferPropertiesOHOS || !vkGetSemaphoreFdKHR ||
        !vkImportSemaphoreFdKHR || !vkQueueSignalReleaseImageOHOS || !vkQueueSubmit;
    if (isProcNotValid) {
        ROSEN_LOGE("Could not acquire device proc addresses");
        return false;
    }
    return true;
}

void RsVulkanInterface::ConfigureExtensions()
{
    deviceExtensions_ = gMandatoryDeviceExtensions;
    uint32_t count = 0;
    if (vkEnumerateDeviceExtensionProperties(physicalDevice_, nullptr, &count, nullptr) != VK_SUCCESS) {
        ROSEN_LOGE("Failed to get device extension count, try to create device with mandatory extensions only!");
        return;
    }
    std::vector<VkExtensionProperties> supportedExtensions(count);
    if (vkEnumerateDeviceExtensionProperties(physicalDevice_, nullptr, &count, supportedExtensions.data()) !=
        VK_SUCCESS) {
        ROSEN_LOGE("Failed to get device extensions, try to create device with mandatory extensions only!");
        return;
    }
    std::unordered_set<std::string_view> supportedExtensionNames;
    for (auto& prop : supportedExtensions) {
        supportedExtensionNames.emplace(prop.extensionName);
    }
    for (auto& ext : gOptionalDeviceExtensions) {
        if (supportedExtensionNames.find(ext) != supportedExtensionNames.end()) {
            deviceExtensions_.emplace_back(ext);
        }
    }
#ifdef ROSEN_OHOS
    if (Drawing::SystemProperties::IsVkImageDfxEnabled()) {
        for (auto& ext : gOptionalDeviceExtensionsDebug) {
            if (supportedExtensionNames.find(ext) == supportedExtensionNames.end()) {
                ROSEN_LOGE("Optional device extension %{public}s not found! Skip it.", ext);
                continue;
            }
            deviceExtensions_.emplace_back(ext);
        }
    }
#endif
    for (auto& ext : gMandatoryDeviceExtensions) {
        if (supportedExtensionNames.find(ext) == supportedExtensionNames.end()) {
            ROSEN_LOGE("Mandatory device extension %{public}s not found! Try to enable it anyway.", ext);
        }
    }
}

void RsVulkanInterface::ConfigureFeatures(bool isProtected)
{
    ycbcrFeature_.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES;
    ycbcrFeature_.pNext = nullptr;
    deviceMemoryExclusiveThreshold_.sType = VK_STRUCTURE_TYPE_DEVICE_MEMORY_EXCLUSIVE_THRESHOLD_INFO;
    deviceMemoryExclusiveThreshold_.pNext = &ycbcrFeature_;
    deviceMemoryExclusiveThreshold_.threshold = grChunkSize_;
    sync2Feature_.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
    sync2Feature_.pNext = &deviceMemoryExclusiveThreshold_;
    bindlessFeature_.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
    bindlessFeature_.pNext = &sync2Feature_;
    timelineFeature_.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES;
    timelineFeature_.pNext = &bindlessFeature_;
    physicalDeviceFeatures2_.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    physicalDeviceFeatures2_.pNext = &timelineFeature_;

    if (isProtected) {
        protectedMemoryFeatures_ = new VkPhysicalDeviceProtectedMemoryFeatures;
        protectedMemoryFeatures_->sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROTECTED_MEMORY_FEATURES;
        protectedMemoryFeatures_->pNext = nullptr;
        ycbcrFeature_.pNext = protectedMemoryFeatures_;
    }
}

bool RsVulkanInterface::CreateSkiaBackendContext(bool isProtected)
{
    auto getProc = CreateSkiaGetProc();
    if (getProc == nullptr) {
        ROSEN_LOGE("CreateSkiaBackendContext getProc is null");
        return false;
    }

    backendContext_.fInstance = instance_;
    backendContext_.fPhysicalDevice = physicalDevice_;
    backendContext_.fDevice = device_;
    backendContext_.fQueue = queue_;
    backendContext_.fGraphicsQueueIndex = graphicsQueueFamilyIndex_;
    backendContext_.fMaxAPIVersion = VK_API_VERSION_1_2;

    skVkExtensions_.init(getProc, instance_, physicalDevice_,
        gInstanceExtensions.size(), gInstanceExtensions.data(),
        deviceExtensions_.size(), deviceExtensions_.data());

    backendContext_.fVkExtensions = &skVkExtensions_;
    backendContext_.fDeviceFeatures2 = &physicalDeviceFeatures2_;
    backendContext_.fGetProc = std::move(getProc);
    backendContext_.fProtectedContext = isProtected ? skgpu::Protected::kYes : skgpu::Protected::kNo;
    return true;
}

bool RsVulkanInterface::QueryMaxGpuBufferSize(uint32_t& maxWidth, uint32_t& maxHeight)
{
    if (physicalDevice_ == VK_NULL_HANDLE) {
        ROSEN_LOGE("RsVulkanInterface::QueryMaxGpuBufferSize: Vulkan physical device is null");
        return false;
    }

    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice_, &deviceProperties);

    maxWidth = deviceProperties.limits.maxImageDimension2D;
    maxHeight = deviceProperties.limits.maxImageDimension2D;

    return true;
}

VKAPI_ATTR VkResult RsVulkanInterface::HookedVkQueueSubmitForBasicRender(VkQueue queue, uint32_t submitCount,
    VkSubmitInfo* pSubmits, VkFence fence)
{
    auto vkInterface = RsVulkanContext::Get(RenderEngineType::BASIC_RENDER).GetRsVulkanInterface();
    std::lock_guard<std::mutex> lock(vkInterface->graphicsQueueMutex_);
    RS_LOGD_IF(DEBUG_IPC, "%{public}s queue, interfaceType: %{public}d", __func__, 0);
    RS_OPTIONAL_TRACE_NAME_FMT("%s queue, interfaceType: %d", __func__, 0);
    VkResult ret = vkInterface->vkQueueSubmit(queue, submitCount, pSubmits, fence);
#ifdef HETERO_HDR_ENABLE
    RSHDRPatternManager::Instance().MHCSubmitGPUTask(submitCount, pSubmits);
#endif
    return ret;
}

VKAPI_ATTR VkResult RsVulkanInterface::HookedVkQueueSubmitForUnprotectedRedraw(VkQueue queue, uint32_t submitCount,
    VkSubmitInfo* pSubmits, VkFence fence)
{
    auto vkInterface = RsVulkanContext::Get(RenderEngineType::UNPROTECTED_REDRAW).GetRsVulkanInterface();
    std::lock_guard<std::mutex> lock(vkInterface->graphicsQueueMutex_);
    RS_LOGD_IF(DEBUG_IPC, "%{public}s hardware queue, interfaceType: %{public}d",
        __func__, static_cast<int>(RenderEngineType::UNPROTECTED_REDRAW));
    RS_OPTIONAL_TRACE_NAME_FMT("%s hardware queue, interfaceType: %d",
        __func__, static_cast<int>(RenderEngineType::UNPROTECTED_REDRAW));
    return vkInterface->vkQueueSubmit(queue, submitCount, pSubmits, fence);
}

VKAPI_ATTR VkResult RsVulkanInterface::HookedVkQueueSubmitForProtectedRedraw(VkQueue queue, uint32_t submitCount,
    VkSubmitInfo* pSubmits, VkFence fence)
{
    auto vkInterface = RsVulkanContext::Get(RenderEngineType::PROTECTED_REDRAW).GetRsVulkanInterface();
    std::lock_guard<std::mutex> lock(vkInterface->graphicsQueueMutex_);
    RS_LOGD_IF(DEBUG_IPC, "%{public}s hardware queue, interfaceType: %{public}d", __func__, 1);
    RS_OPTIONAL_TRACE_NAME_FMT("%s hardware queue, interfaceType: %d", __func__, 1);
    return vkInterface->vkQueueSubmit(queue, submitCount, pSubmits, fence);
}

VkResult RsVulkanInterface::QueueSignalReleaseImageOHOS(VkQueue queue, uint32_t waitSemaphoreCount,
    const VkSemaphore* pWaitSemaphores, VkImage image, int32_t* pNativeFenceFd)
{
    std::lock_guard<std::mutex> lock(graphicsQueueMutex_);
    return vkQueueSignalReleaseImageOHOS(queue, waitSemaphoreCount, pWaitSemaphores, image, pNativeFenceFd);
}
} // namespace Rosen
} // namespace OHOS