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

#include "platform/ohos/backend/rs_vulkan_context.h"

#include <dlfcn.h>
#include <memory>
#include <mutex>
#include <string_view>
#include <unordered_set>
#include <vector>

#include "common/rs_optional_trace.h"
#include "platform/common/rs_log.h"
#include "render_context/memory_handler.h"
#ifdef HETERO_HDR_ENABLE
#include "rs_hdr_pattern_manager.h"
#endif
#include "include/gpu/vk/VulkanExtensions.h"
#include "unistd.h"
#include "utils/system_properties.h"
#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_ohos.h"

#define ACQUIRE_PROC(name, context)                         \
    if (!(vk##name = AcquireProc("vk" #name, context))) {   \
        ROSEN_LOGE("Could not acquire proc: vk" #name);     \
    }

namespace OHOS {
namespace Rosen {
thread_local bool RsVulkanContext::isProtected_ = false;
thread_local VulkanInterfaceType RsVulkanContext::vulkanInterfaceType_ = VulkanInterfaceType::BASIC_RENDER;
std::map<int, DrawingContextProperty> RsVulkanContext::drawingContextMap_;
std::mutex RsVulkanContext::drawingContextMutex_;
std::recursive_mutex RsVulkanContext::recyclableSingletonMutex_;
bool RsVulkanContext::isRecyclable_ = true;
std::atomic<bool> RsVulkanContext::isRecyclableSingletonValid_ = false;
std::atomic<bool> RsVulkanContext::isInited_ = false;
void* RsVulkanInterface::handle_ = nullptr;
VkInstance RsVulkanInterface::instance_ = VK_NULL_HANDLE;

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
};

// enabled when persist.sys.graphic.openVkImageMemoryDfx is true
static const std::vector<const char*> gOptionalDeviceExtensionsDebug = {
    VK_KHR_MAINTENANCE_5_EXTENSION_NAME,
    VK_EXT_DEVICE_ADDRESS_BINDING_REPORT_EXTENSION_NAME,
};

static const int GR_CHUNK_SIZE = 1048576;
static const int GR_CACHE_MAX_COUNT = 8192;
static const size_t GR_CACHE_MAX_BYTE_SIZE = 96 * (1 << 20);
static const int32_t CACHE_LIMITS_TIMES = 5;  // this will change RS memory!
static const int32_t MAX_SEMAPHORE_FENCE_COUNT = 3000;
static const int32_t SEMAPHORE_STATS_INTERVAL = 144000;
std::atomic<uint64_t> RsVulkanInterface::callbackSemaphoreInfofdDupCnt_ = 0;
std::atomic<uint64_t> RsVulkanInterface::callbackSemaphoreInfoRSDerefCnt_ = 0;
std::atomic<uint64_t> RsVulkanInterface::callbackSemaphoreInfo2DEngineDerefCnt_ = 0;
std::atomic<uint64_t> RsVulkanInterface::callbackSemaphoreInfo2DEngineDefensiveDerefCnt_ = 0;
std::atomic<uint64_t> RsVulkanInterface::callbackSemaphoreInfoFlushCnt_ = 0;
std::atomic<uint64_t> RsVulkanInterface::callbackSemaphoreInfo2DEngineCallCnt_ = 0;

bool RsVulkanInterface::Init(VulkanInterfaceType vulkanInterfaceType, bool isProtected, bool isHtsEnable)
{
    RS_TRACE_NAME("RsVulkanInterface::Init");
    RS_LOGI("RsVulkanInterface::Init: Starting initialization for interface type %{public}d",
        static_cast<int>(vulkanInterfaceType));

    acquiredMandatoryProcAddresses_ = false;

    acquiredMandatoryProcAddresses_ = OpenLibraryHandle() && SetupLoaderProcAddresses();
    if (!acquiredMandatoryProcAddresses_) {
        RS_LOGE("RsVulkanInterface::Init: Failed to acquire mandatory proc addresses");
        return false;
    }

    interfaceType_ = vulkanInterfaceType;

    RS_LOGI("RsVulkanInterface::Init: Step 1 - CreateInstance started");
    if (!CreateInstance()) {
        RS_LOGE("RsVulkanInterface::Init: Failed to create instance");
        return false;
    }
    RS_LOGI("RsVulkanInterface::Init: Step 1 - CreateInstance completed");

    RS_LOGI("RsVulkanInterface::Init: Step 2 - SelectPhysicalDevice started");
    if (!SelectPhysicalDevice(isProtected)) {
        RS_LOGE("RsVulkanInterface::Init: Failed to select physical device");
        return false;
    }
    RS_LOGI("RsVulkanInterface::Init: Step 2 - SelectPhysicalDevice completed");

    RS_LOGI("RsVulkanInterface::Init: Step 3 - CreateDevice started");
    if (!CreateDevice(isProtected, isHtsEnable)) {
        RS_LOGE("RsVulkanInterface::Init: Failed to create device");
        return false;
    }
    RS_LOGI("RsVulkanInterface::Init: Step 3 - CreateDevice completed");

    RS_LOGI("RsVulkanInterface::Init: Step 4 - CreateSkiaBackendContext started");
    std::unique_lock<std::mutex> lock(vkMutex_);
    if (!CreateSkiaBackendContext(&backendContext_, isProtected)) {
        RS_LOGE("RsVulkanInterface::Init: Failed to create Skia backend context");
        return false;
    }
    RS_LOGI("RsVulkanInterface::Init: Step 4 - CreateSkiaBackendContext completed");
    return true;
}

RsVulkanInterface::~RsVulkanInterface()
{
    for (auto&& semaphoreFence : usedSemaphoreFenceList_) {
        if (semaphoreFence.fence != nullptr) {
            semaphoreFence.fence->Wait(-1);
        }
        vkDestroySemaphore(device_, semaphoreFence.semaphore, nullptr);
    }
    usedSemaphoreFenceList_.clear();
    if (protectedMemoryFeatures_) {
        delete protectedMemoryFeatures_;
        protectedMemoryFeatures_ = nullptr;
    }

    if (device_ != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(device_);
        vkDestroyDevice(device_, nullptr);
        device_ = VK_NULL_HANDLE;
    }
    CloseLibraryHandle();
}

bool RsVulkanInterface::IsValid() const
{
    return instance_ != VK_NULL_HANDLE && device_ != VK_NULL_HANDLE;
}

bool RsVulkanInterface::SetupLoaderProcAddresses()
{
    if (handle_ == nullptr) {
        return true;
    }
    vkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(dlsym(handle_, "vkGetInstanceProcAddr"));
    vkGetDeviceProcAddr = reinterpret_cast<PFN_vkGetDeviceProcAddr>(dlsym(handle_, "vkGetDeviceProcAddr"));
    vkEnumerateInstanceExtensionProperties = reinterpret_cast<PFN_vkEnumerateInstanceExtensionProperties>(
        dlsym(handle_, "vkEnumerateInstanceExtensionProperties"));
    vkCreateInstance = reinterpret_cast<PFN_vkCreateInstance>(dlsym(handle_, "vkCreateInstance"));

    if (!vkGetInstanceProcAddr) {
        ROSEN_LOGE("Could not acquire vkGetInstanceProcAddr");
        return false;
    }

    VkInstance null_instance = VK_NULL_HANDLE;
    return true;
}

bool RsVulkanInterface::CreateInstance()
{
    RS_TRACE_NAME("RsVulkanInterface::CreateInstance");
    if (!acquiredMandatoryProcAddresses_) {
        ROSEN_LOGE("CreateInstance: Mandatory proc addresses not acquired");
        return false;
    }

    if (instance_ == VK_NULL_HANDLE) {
        const VkApplicationInfo info = {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pNext = nullptr,
            .pApplicationName = "OHOS",
            .applicationVersion = 0,
            .pEngineName = "Rosen",
            .engineVersion = VK_MAKE_VERSION(1, 0, 0),
            .apiVersion = VK_API_VERSION_1_2,
        };

        const VkInstanceCreateInfo create_info = {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .pApplicationInfo = &info,
            .enabledLayerCount = 0,
            .ppEnabledLayerNames = nullptr,
            .enabledExtensionCount = static_cast<uint32_t>(gInstanceExtensions.size()),
            .ppEnabledExtensionNames = gInstanceExtensions.data(),
        };
        if (vkCreateInstance(&create_info, nullptr, &instance_) != VK_SUCCESS) {
            ROSEN_LOGE("Could not create vulkan instance");
            return false;
        }
        ROSEN_LOGI("Vulkan instance created successfully");
    }

    ACQUIRE_PROC(CreateDevice, instance_);
    ACQUIRE_PROC(DestroyDevice, instance_);
    ACQUIRE_PROC(DestroyInstance, instance_);
    ACQUIRE_PROC(EnumerateDeviceExtensionProperties, instance_);
    ACQUIRE_PROC(EnumeratePhysicalDevices, instance_);
    ACQUIRE_PROC(GetPhysicalDeviceFeatures, instance_);
    ACQUIRE_PROC(GetPhysicalDeviceQueueFamilyProperties, instance_);
    ACQUIRE_PROC(GetPhysicalDeviceMemoryProperties, instance_);
    ACQUIRE_PROC(GetPhysicalDeviceMemoryProperties2, instance_);
    ACQUIRE_PROC(GetPhysicalDeviceFeatures2, instance_);

    return true;
}

bool RsVulkanInterface::SelectPhysicalDevice(bool isProtected)
{
    RS_TRACE_NAME("RsVulkanInterface::SelectPhysicalDevice");
    if (!instance_) {
        ROSEN_LOGE("SelectPhysicalDevice: Instance is null");
        return false;
    }
    uint32_t deviceCount = 0;
    if (vkEnumeratePhysicalDevices(instance_, &deviceCount, nullptr) != VK_SUCCESS) {
        ROSEN_LOGE("SelectPhysicalDevice: Failed to enumerate physical devices (count)");
        return false;
    }

    if (deviceCount == 0) {
        ROSEN_LOGE("SelectPhysicalDevice: No physical devices found");
        return false;
    }

    std::vector<VkPhysicalDevice> physicalDevices;
    physicalDevices.resize(deviceCount);
    if (vkEnumeratePhysicalDevices(instance_, &deviceCount, physicalDevices.data()) != VK_SUCCESS) {
        ROSEN_LOGE("SelectPhysicalDevice: Failed to enumerate physical devices (data)");
        return false;
    }
    physicalDevice_ = physicalDevices[0];
    ROSEN_LOGI("SelectPhysicalDevice: Selected physical device %{public}u of %{public}u", 0, deviceCount);

    VkPhysicalDeviceProperties physDevProps;
    vkGetPhysicalDeviceProperties(physicalDevice_, &physDevProps);
    physicalDeviceApiVersion_ = physDevProps.apiVersion;
    ROSEN_LOGI("SelectPhysicalDevice: Physical device API version %{public}u.%{public}u.%{public}u",
        VK_VERSION_MAJOR(physicalDeviceApiVersion_), VK_VERSION_MINOR(physicalDeviceApiVersion_),
        VK_VERSION_PATCH(physicalDeviceApiVersion_));

    VkPhysicalDeviceProperties2 physDevProps2 = {
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
        0,
        {},
    };
    VkPhysicalDeviceProtectedMemoryProperties protMemProps = {
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROTECTED_MEMORY_PROPERTIES,
        0,
        {},
    };
    if (isProtected) {
        physDevProps2.pNext = &protMemProps;
    }
    vkGetPhysicalDeviceProperties2(physicalDevice_, &physDevProps2);
    return true;
}

void RsVulkanInterface::ConfigureFeatures(bool isProtected)
{
    CleanupFeatureChain();
    if (!InitializeFeatureChain(isProtected)) {
        ROSEN_LOGE("ConfigureFeatures: Failed to initialize feature chain");
    }
}

bool RsVulkanInterface::InitializeFeatureChain(bool isProtected)
{
    ycbcrFeature_.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES;
    ycbcrFeature_.pNext = nullptr;
    sync2Feature_.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
    sync2Feature_.pNext = &ycbcrFeature_;
    bindlessFeature_.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
    bindlessFeature_.pNext = &sync2Feature_;
    timelineFeature_.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES;
    timelineFeature_.pNext = &bindlessFeature_;
    physicalDeviceFeatures2_.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    physicalDeviceFeatures2_.pNext = &timelineFeature_;

    protectedMemoryFeatures_ = new VkPhysicalDeviceProtectedMemoryFeatures;
    if (isProtected) {
        if (physicalDeviceApiVersion_ < VK_API_VERSION_1_1) {
            ROSEN_LOGE("InitializeFeatureChain: Protected memory requires Vulkan API version 1.1 or higher");
            return false;
        }
        protectedMemoryFeatures_->sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROTECTED_MEMORY_FEATURES;
        protectedMemoryFeatures_->pNext = nullptr;
        ycbcrFeature_.pNext = protectedMemoryFeatures_;
    }

    return true;
}

void RsVulkanInterface::CleanupFeatureChain()
{
    if (protectedMemoryFeatures_ != nullptr) {
        delete protectedMemoryFeatures_;
        protectedMemoryFeatures_ = nullptr;
    }
}

void RsVulkanInterface::ConfigureExtensions()
{
    RS_TRACE_NAME("RsVulkanInterface::ConfigureExtensions");
    deviceExtensions_ = gMandatoryDeviceExtensions;

    uint32_t count = 0;
    if (vkEnumerateDeviceExtensionProperties(physicalDevice_, nullptr, &count, nullptr) != VK_SUCCESS) {
        ROSEN_LOGE("ConfigureExtensions: Failed to get device extension count");
        return;
    }

    if (count == 0) {
        ROSEN_LOGW("ConfigureExtensions: No device extensions found");
        return;
    }

    std::vector<VkExtensionProperties> supportedExtensions(count);
    if (vkEnumerateDeviceExtensionProperties(physicalDevice_, nullptr, &count,
        supportedExtensions.data()) != VK_SUCCESS) {
        ROSEN_LOGE("ConfigureExtensions: Failed to get device extensions");
        return;
    }

    std::unordered_set<std::string_view> extensionNames;
    for (const auto& prop : supportedExtensions) {
        extensionNames.emplace(prop.extensionName);
    }

    uint32_t optionalEnabledCount = 0;
    for (const auto& ext : gOptionalDeviceExtensions) {
        if (extensionNames.find(ext) != extensionNames.end()) {
            deviceExtensions_.emplace_back(ext);
            optionalEnabledCount++;
        }
    }

#ifdef ROSEN_OHOS
    if (Drawing::SystemProperties::IsVkImageDfxEnabled()) {
        uint32_t debugEnabledCount = 0;
        for (const auto& ext : gOptionalDeviceExtensionsDebug) {
            if (extensionNames.find(ext) != extensionNames.end()) {
                deviceExtensions_.emplace_back(ext);
                debugEnabledCount++;
            } else {
                ROSEN_LOGW("ConfigureExtensions: Optional debug extension %{public}s not found", ext);
            }
        }
        ROSEN_LOGI("ConfigureExtensions: Enabled %{public}u debug extensions", debugEnabledCount);
    }
#endif

    for (const auto& ext : gMandatoryDeviceExtensions) {
        if (extensionNames.find(ext) == extensionNames.end()) {
            ROSEN_LOGE("ConfigureExtensions: Mandatory extension %{public}s not found!", ext);
        }
    }

    ROSEN_LOGI("ConfigureExtensions: Enabled %{public}u optional extensions", optionalEnabledCount);
}

bool RsVulkanInterface::CreateDevice(bool isProtected, bool isHtsEnable)
{
    RS_TRACE_NAME("RsVulkanInterface::CreateDevice");
    if (!physicalDevice_) {
        ROSEN_LOGE("CreateDevice: Physical device is null");
        return false;
    }
    uint32_t queueCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice_, &queueCount, nullptr);

    if (queueCount == 0) {
        ROSEN_LOGE("CreateDevice: No queue families found");
        return false;
    }

    std::vector<VkQueueFamilyProperties> queueProps(queueCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice_, &queueCount, queueProps.data());

    graphicsQueueFamilyIndex_ = UINT32_MAX;
    for (uint32_t i = 0; i < queueCount; i++) {
        if (queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            graphicsQueueFamilyIndex_ = i;
            break;
        }
    }

    if (graphicsQueueFamilyIndex_ == UINT32_MAX) {
        ROSEN_LOGE("CreateDevice: No graphics queue family found");
        return false;
    }
    // If multiple queues are needed, queue priorities should be set.	 
    // when it is greater than 0.5 indicates high priority and less than 0.5 indicates low priority
    const float priorities[1] = {1.0f};
    VkDeviceQueueCreateFlags deviceQueueCreateFlags = isProtected ? VK_DEVICE_QUEUE_CREATE_PROTECTED_BIT : 0;
    std::vector<VkDeviceQueueCreateInfo> queueCreate {{
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, .pNext = nullptr,
        .flags = deviceQueueCreateFlags, .queueFamilyIndex = graphicsQueueFamilyIndex_, .queueCount = 1,
        .pQueuePriorities = priorities,
    }};
    ConfigureExtensions();
    ConfigureFeatures(isProtected);

    vkGetPhysicalDeviceFeatures2(physicalDevice_, &physicalDeviceFeatures2_);

    VkDeviceCreateFlags deviceCreateFlags = isHtsEnable ? VK_DEVICE_CREATE_HTS_ENABLE_BIT : 0;
    const VkDeviceCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO, .pNext = &physicalDeviceFeatures2_,
        .flags = deviceCreateFlags,
        .queueCreateInfoCount = queueCreate.size(),
        .pQueueCreateInfos = queueCreate.data(),
        .enabledLayerCount = 0, .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = static_cast<uint32_t>(deviceExtensions_.size()),
        .ppEnabledExtensionNames = deviceExtensions_.data(), .pEnabledFeatures = nullptr,
    };
    if (vkCreateDevice(physicalDevice_, &createInfo, nullptr, &device_) != VK_SUCCESS) {
        ROSEN_LOGE("CreateDevice: vkCreateDevice failed");
        SetVulkanDeviceStatus(VulkanDeviceStatus::CREATE_FAIL);
        return false;
    }
    SetVulkanDeviceStatus(VulkanDeviceStatus::CREATE_SUCCESS);
    if (!SetupDeviceProcAddresses(device_)) {
        return false;
    }

    const VkDeviceQueueInfo2 deviceQueueInfo2 = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2, nullptr,
        deviceQueueCreateFlags, static_cast<uint32_t>(graphicsQueueFamilyIndex_), 0};
    vkGetDeviceQueue2(device_, &deviceQueueInfo2, &queue_);
    return true;
}

bool RsVulkanInterface::CreateSkiaBackendContext(skgpu::VulkanBackendContext* context, bool isProtected)
{
    auto getProc = CreateSkiaGetProc();
    if (getProc == nullptr) {
        ROSEN_LOGE("CreateSkiaBackendContext getProc is null");
        return false;
    }

    context->fInstance = instance_;
    context->fPhysicalDevice = physicalDevice_;
    context->fDevice = device_;
    context->fQueue = queue_;
    context->fGraphicsQueueIndex = graphicsQueueFamilyIndex_;

    context->fMaxAPIVersion = VK_API_VERSION_1_2;

    skVkExtensions_.init(getProc, instance_, physicalDevice_,
        gInstanceExtensions.size(), gInstanceExtensions.data(),
        deviceExtensions_.size(), deviceExtensions_.data());

    context->fVkExtensions = &skVkExtensions_;
    context->fDeviceFeatures2 = &physicalDeviceFeatures2_;

    context->fGetProc = std::move(getProc);

    context->fProtectedContext = isProtected ? skgpu::Protected::kYes : skgpu::Protected::kNo;

    return true;
}

bool RsVulkanInterface::SetupDeviceProcAddresses(VkDevice device)
{
    ACQUIRE_PROC(AllocateMemory, device_);
    ACQUIRE_PROC(BindImageMemory, device_);
    ACQUIRE_PROC(BindImageMemory2, device_);
    ACQUIRE_PROC(CreateImage, device_);
    ACQUIRE_PROC(CreateSemaphore, device_);
    ACQUIRE_PROC(DestroyImage, device_);
    ACQUIRE_PROC(DestroySemaphore, device_);
    ACQUIRE_PROC(DeviceWaitIdle, device_);
    ACQUIRE_PROC(FreeMemory, device_);
    ACQUIRE_PROC(GetImageMemoryRequirements, device_);
    ACQUIRE_PROC(QueueSubmit, device_);
    ACQUIRE_PROC(GetNativeBufferPropertiesOHOS, device_);
    ACQUIRE_PROC(QueueSignalReleaseImageOHOS, device_);
    ACQUIRE_PROC(ImportSemaphoreFdKHR, device_);
    ACQUIRE_PROC(GetSemaphoreFdKHR, device_);

    if (!vkBindImageMemory2 || !vkCreateImage) {
        ROSEN_LOGE("SetupDeviceProcAddresses: get function pointer fail!");
        return false;
    }
    return true;
}

bool RsVulkanInterface::OpenLibraryHandle()
{
    if (handle_) {
        ROSEN_LOGI("RsVulkanInterface OpenLibararyHandle: vk so has already been loaded.");
        return true;
    }
    ROSEN_LOGI("VulkanProcTable OpenLibararyHandle: dlopen libvulkan.so.");
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
    if (handle_ != nullptr) {
        dlerror();
        if (dlclose(handle_) != 0) {
            ROSEN_LOGE("Could not close the vulkan lib handle. This indicates a leak. %{public}s", dlerror());
        }
        handle_ = nullptr;
    }
    return handle_ == nullptr;
}

PFN_vkVoidFunction RsVulkanInterface::AcquireProc(
    const char* procName,
    const VkInstance& instance) const
{
    if (procName == nullptr || !vkGetInstanceProcAddr) {
        return nullptr;
    }

    return vkGetInstanceProcAddr(instance, procName);
}

PFN_vkVoidFunction RsVulkanInterface::AcquireProc(
    const char* procName,
    const VkDevice& device) const
{
    if (procName == nullptr || !device || !vkGetInstanceProcAddr) {
        return nullptr;
    }
    return vkGetDeviceProcAddr(device, procName);
}

skgpu::VulkanGetProc RsVulkanInterface::CreateSkiaGetProc() const
{
    if (!IsValid()) {
        return nullptr;
    }

    return [this](const char* procName, VkInstance instance, VkDevice device) {
        if (device != VK_NULL_HANDLE) {
            std::string_view s{procName};
            if (s.find("vkQueueSubmit") == 0) {
                return (PFN_vkVoidFunction)RsVulkanContext::HookedVkQueueSubmit;
            }
            auto result = AcquireProc(procName, device);
            if (result != nullptr) {
                return result;
            }
        }
        return AcquireProc(procName, instance);
    };
}

std::shared_ptr<Drawing::GPUContext> RsVulkanInterface::DoCreateDrawingContext()
{
    std::unique_lock<std::mutex> lock(vkMutex_);

    auto drawingContext = std::make_shared<Drawing::GPUContext>();
    Drawing::GPUContextOptions options;
    std::string vkVersion = std::to_string(VK_API_VERSION_1_2);
    auto size = vkVersion.size();
    drawingContext->BuildFromVK(backendContext_, options);
    return drawingContext;
}

std::shared_ptr<Drawing::GPUContext> RsVulkanInterface::CreateDrawingContext()
{
    auto drawingContext = DoCreateDrawingContext();
    int maxResources = 0;
    size_t maxResourcesSize = 0;
    int cacheLimitsTimes = CACHE_LIMITS_TIMES;
    drawingContext->GetResourceCacheLimits(&maxResources, &maxResourcesSize);
    if (maxResourcesSize > 0) {
        drawingContext->SetResourceCacheLimits(cacheLimitsTimes * maxResources,
            cacheLimitsTimes * std::fmin(maxResourcesSize, GR_CACHE_MAX_BYTE_SIZE));
    } else {
        drawingContext->SetResourceCacheLimits(GR_CACHE_MAX_COUNT, GR_CACHE_MAX_BYTE_SIZE);
    }
    RsVulkanContext::SaveNewDrawingContext(gettid(), drawingContext);
    return drawingContext;
}

void RsVulkanInterface::DestroyAllSemaphoreFence()
{
    std::lock_guard<std::mutex> lock(semaphoreLock_);
    RS_LOGE("Device lost clear all semaphore fences, count [%{public}zu] ", usedSemaphoreFenceList_.size());
    for (auto&& semaphoreFence : usedSemaphoreFenceList_) {
        vkDestroySemaphore(device_, semaphoreFence.semaphore, nullptr);
    }
    usedSemaphoreFenceList_.clear();
}

void RsVulkanInterface::SetVulkanDeviceStatus(VulkanDeviceStatus status)
{
    deviceStatus_ = status;
}

VulkanDeviceStatus RsVulkanInterface::GetVulkanDeviceStatus()
{
    return deviceStatus_.load();
}

VkSemaphore RsVulkanInterface::RequireSemaphore()
{
    {
        std::lock_guard<std::mutex> lock(semaphoreLock_);
        if (usedSemaphoreFenceList_.size() >= MAX_SEMAPHORE_FENCE_COUNT) {
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
        if (OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::VULKAN &&
            RsVulkanInterface::callbackSemaphoreInfofdDupCnt_.load(std::memory_order_relaxed) %
                SEMAPHORE_STATS_INTERVAL == 0) {
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
        RS_LOGE("RequireSemaphore: Failed to create semaphore, error %{public}d", err);
        return VK_NULL_HANDLE;
    }
    return semaphore;
}

VkSemaphore RsVulkanInterface::RequireTimelineSemaphore()
{
    VkSemaphoreTypeCreateInfo timelineCreateInfo;
    timelineCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
    timelineCreateInfo.pNext = nullptr;
    timelineCreateInfo.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
    timelineCreateInfo.initialValue = 0;

    VkSemaphoreCreateInfo semaphoreInfo;
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreInfo.pNext = &timelineCreateInfo;
    semaphoreInfo.flags = 0;
    VkSemaphore semaphore;
    auto err = vkCreateSemaphore(device_, &semaphoreInfo, nullptr, &semaphore);
    if (err != VK_SUCCESS) {
        RS_LOGE("RequireTimelineSemaphore: Failed to create timeline semaphore, error %{public}d", err);
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

RsVulkanContext::RsVulkanContext()
{
    vulkanInterfaceVec_.resize(size_t(VulkanInterfaceType::MAX_INTERFACE_TYPE));
    if (RsVulkanContext::IsRecyclable()) {
        InitVulkanContextForHybridRender();
    } else {
        InitVulkanContextForUniRender();
    }
    RsVulkanContext::isInited_ = true;
    RsVulkanContext::isRecyclableSingletonValid_ = true;
}

RsVulkanContext::~RsVulkanContext()
{
    std::lock_guard<std::mutex> lock(drawingContextMutex_);
    drawingContextMap_.clear();
    RsVulkanContext::isRecyclableSingletonValid_ = false;
}

void RsVulkanContext::InitVulkanContextForHybridRender()
{
    RS_TRACE_NAME("InitVulkanContextForHybridRender");
    auto vulkanInterface = std::make_shared<RsVulkanInterface>();
    // init drawing context for RT thread bind to backendContext.
    if (!vulkanInterface->Init(VulkanInterfaceType::BASIC_RENDER, false)) {
        RS_LOGE("InitVulkanContextForHybridRender: Failed to initialize Vulkan interface");
        return;
    }
    vulkanInterface->CreateDrawingContext();

    vulkanInterfaceVec_[size_t(VulkanInterfaceType::BASIC_RENDER)] = std::move(vulkanInterface);
}

void RsVulkanContext::InitVulkanContextForUniRender()
{
    RS_TRACE_NAME("InitVulkanContextForUniRender");
    // create vulkan interface for render thread.
    auto uniRenderVulkanInterface = std::make_shared<RsVulkanInterface>();
    // init drawing context for RT thread bind to backendContext.
    if (!uniRenderVulkanInterface->Init(VulkanInterfaceType::BASIC_RENDER, false, true)) {
        RS_LOGE("InitVulkanContextForUniRender: Failed to initialize basic render Vulkan interface");
        return;
    }
    uniRenderVulkanInterface->CreateDrawingContext();
    // create vulkan interface for hardware thread (unprotected).
    auto unprotectedReDrawVulkanInterface = std::make_shared<RsVulkanInterface>();
    if (!unprotectedReDrawVulkanInterface->Init(VulkanInterfaceType::UNPROTECTED_REDRAW, false, false)) {
        RS_LOGE("InitVulkanContextForUniRender: Failed to initialize unprotected redraw Vulkan interface");
        return;
    }
    vulkanInterfaceVec_[size_t(VulkanInterfaceType::BASIC_RENDER)] = std::move(uniRenderVulkanInterface);
    vulkanInterfaceVec_[size_t(VulkanInterfaceType::UNPROTECTED_REDRAW)] = std::move(unprotectedReDrawVulkanInterface);
#ifdef IS_ENABLE_DRM
    isProtected_ = true;
    auto protectedReDrawVulkanInterface = std::make_shared<RsVulkanInterface>();
    // DRM needs to adapt vkQueue in the future.
    if (!protectedReDrawVulkanInterface->Init(VulkanInterfaceType::PROTECTED_REDRAW, true, false)) {
        RS_LOGE("InitVulkanContextForUniRender: Failed to initialize protected redraw Vulkan interface");
        return;
    }
    protectedReDrawVulkanInterface->CreateDrawingContext();
    vulkanInterfaceVec_[size_t(VulkanInterfaceType::PROTECTED_REDRAW)] = std::move(protectedReDrawVulkanInterface);
    isProtected_ = false;
#endif
}

std::unique_ptr<RsVulkanContext>& RsVulkanContext::GetRecyclableSingletonPtr()
{
    std::lock_guard<std::recursive_mutex> lock(recyclableSingletonMutex_);
    static std::unique_ptr<RsVulkanContext> recyclableSingleton = std::make_unique<RsVulkanContext>();
    return recyclableSingleton;
}

RsVulkanContext& RsVulkanContext::GetRecyclableSingleton()
{
    std::lock_guard<std::recursive_mutex> lock(recyclableSingletonMutex_);
    std::unique_ptr<RsVulkanContext>& recyclableSingleton = GetRecyclableSingletonPtr();
    if (recyclableSingleton == nullptr) {
        recyclableSingleton = std::make_unique<RsVulkanContext>();
    }
    return *recyclableSingleton;
}

RsVulkanContext& RsVulkanContext::GetSingleton()
{
    if (isRecyclable_) {
        return RsVulkanContext::GetRecyclableSingleton();
    }
    static RsVulkanContext singleton = RsVulkanContext();
    return singleton;
}

bool RsVulkanContext::CheckDrawingContextRecyclable()
{
    std::lock_guard<std::mutex> lock(drawingContextMutex_);
    for (const auto& iter : RsVulkanContext::drawingContextMap_) {
        if ((iter.second.unprotectContext != nullptr && !iter.second.unprotectRecyclable) ||
            (iter.second.protectContext != nullptr && !iter.second.protectRecyclable)) {
            return false;
        }
    }
    return true;
}

void RsVulkanContext::ReleaseRecyclableSingleton()
{
    if (!isRecyclable_) {
        return;
    }
    if (!CheckDrawingContextRecyclable()) {
        ReleaseRecyclableDrawingContext();
        return;
    }
    ReleaseDrawingContextMap();
    {
        std::lock_guard<std::recursive_mutex> lock(recyclableSingletonMutex_);
        auto& recyclableSingleton = GetRecyclableSingletonPtr();
        recyclableSingleton.reset();
    }
}

std::shared_ptr<Drawing::GPUContext> RsVulkanContext::GetRecyclableDrawingContext()
{
    // 1. get or create drawing context and save it in the map
    auto drawingContext = RsVulkanContext::GetDrawingContext();

    // 2. set recyclable tag for drawingContext when it's valid (i.e it's in the map)
    static thread_local int tidForRecyclable = gettid();
    std::lock_guard<std::mutex> lock(drawingContextMutex_);
    if (auto iter = drawingContextMap_.find(tidForRecyclable); iter != drawingContextMap_.end()) {
        if (isProtected_) {
            iter->second.protectRecyclable = true;
        } else {
            iter->second.unprotectRecyclable = true;
        }
    }
    return drawingContext;
}

void RsVulkanContext::ReleaseDrawingContextMap()
{
    std::lock_guard<std::mutex> lock(drawingContextMutex_);
    for (const auto& [_, second] : drawingContextMap_) {
        if (auto context = second.unprotectContext) {
            context->FlushAndSubmit(true);
        }
        if (auto protectContext = second.protectContext) {
            protectContext->FlushAndSubmit(true);
        }
    }
    drawingContextMap_.clear();
}

void RsVulkanContext::ReleaseRecyclableDrawingContext()
{
    std::lock_guard<std::mutex> lock(drawingContextMutex_);
    for (auto iter = drawingContextMap_.begin(); iter != drawingContextMap_.end();) {
        if (iter->second.protectRecyclable) {
            iter->second.protectContext = nullptr;
        }
        if (iter->second.unprotectRecyclable) {
            iter->second.unprotectContext = nullptr;
        }
        if (iter->second.protectContext == nullptr && iter->second.unprotectContext == nullptr) {
            iter = drawingContextMap_.erase(iter);
        } else {
            ++iter;
        }
    }
}

void RsVulkanContext::ReleaseDrawingContextForThread(int tid)
{
    std::lock_guard<std::mutex> lock(drawingContextMutex_);
    drawingContextMap_.erase(tid);
}

void RsVulkanContext::SaveNewDrawingContext(int tid, std::shared_ptr<Drawing::GPUContext> drawingContext)
{
    std::lock_guard<std::mutex> lock(drawingContextMutex_);
    static thread_local auto func = [tid]() {
        RsVulkanContext::ReleaseDrawingContextForThread(tid);
    };
    static thread_local auto drawContextHolder = std::make_shared<DrawContextHolder>(func);
    auto& context = drawingContextMap_[tid];
    if (isProtected_) {
        context.protectContext = drawingContext;
        context.protectRecyclable = false;
    } else {
        context.unprotectContext = drawingContext;
        context.unprotectRecyclable = false;
    }
}

bool RsVulkanContext::GetIsInited()
{
    return isInited_.load();
}

bool RsVulkanContext::IsRecyclableSingletonValid()
{
    return isRecyclableSingletonValid_.load();
}

RsVulkanInterface& RsVulkanContext::GetRsVulkanInterface()
{
    switch (vulkanInterfaceType_) {
        case VulkanInterfaceType::PROTECTED_REDRAW:
            return *(vulkanInterfaceVec_[size_t(VulkanInterfaceType::PROTECTED_REDRAW)].get());
        case VulkanInterfaceType::UNPROTECTED_REDRAW:
            return *(vulkanInterfaceVec_[size_t(VulkanInterfaceType::UNPROTECTED_REDRAW)].get());
        case VulkanInterfaceType::BASIC_RENDER:
        default:
            return *(vulkanInterfaceVec_[size_t(VulkanInterfaceType::BASIC_RENDER)].get());
    }
}

VKAPI_ATTR VkResult RsVulkanContext::HookedVkQueueSubmit(VkQueue queue, uint32_t submitCount,
    VkSubmitInfo* pSubmits, VkFence fence)
{
    RsVulkanInterface& vkInterface = RsVulkanContext::GetSingleton().GetRsVulkanInterface();
    auto interfaceType = vkInterface.GetInterfaceType();
    if (interfaceType == VulkanInterfaceType::UNPROTECTED_REDRAW ||
        interfaceType == VulkanInterfaceType::PROTECTED_REDRAW) {
        std::lock_guard<std::mutex> lock(vkInterface.hGraphicsQueueMutex_);
        RS_LOGD("%{public}s hardware queue, interfaceType: %{public}d", __func__, static_cast<int>(interfaceType));
        RS_OPTIONAL_TRACE_NAME_FMT("%s hardware queue, interfaceType: %d", __func__, static_cast<int>(interfaceType));
        return vkInterface.vkQueueSubmit(queue, submitCount, pSubmits, fence);
    } else if (interfaceType == VulkanInterfaceType::BASIC_RENDER) {
        std::lock_guard<std::mutex> lock(vkInterface.graphicsQueueMutex_);
        RS_LOGD("%{public}s queue, interfaceType: %{public}d", __func__, static_cast<int>(interfaceType));
        RS_OPTIONAL_TRACE_NAME_FMT("%s queue, interfaceType: %d", __func__, static_cast<int>(interfaceType));
        VkResult ret = vkInterface.vkQueueSubmit(queue, submitCount, pSubmits, fence);
#ifdef HETERO_HDR_ENABLE
        RSHDRPatternManager::Instance().MHCSubmitGPUTask(submitCount, pSubmits);
#endif
        return ret;
    }
    RS_LOGE("%{public}s abnormal queue occured", __func__);
    return VK_ERROR_UNKNOWN;
}

VKAPI_ATTR VkResult RsVulkanContext::HookedVkQueueSignalReleaseImageOHOS(VkQueue queue, uint32_t waitSemaphoreCount,
    const VkSemaphore* pWaitSemaphores, VkImage image, int32_t* pNativeFenceFd)
{
    RsVulkanInterface& vkInterface = RsVulkanContext::GetSingleton().GetRsVulkanInterface();
    auto interfaceType = vkInterface.GetInterfaceType();
    if (interfaceType == VulkanInterfaceType::UNPROTECTED_REDRAW ||
        interfaceType == VulkanInterfaceType::PROTECTED_REDRAW) {
        std::lock_guard<std::mutex> lock(vkInterface.hGraphicsQueueMutex_);
        RS_LOGD("%{public}s hardware queue, interfaceType: %{public}d", __func__, static_cast<int>(interfaceType));
        RS_OPTIONAL_TRACE_NAME_FMT("%s hardware queue, interfaceType: %d", __func__, static_cast<int>(interfaceType));
        return vkInterface.vkQueueSignalReleaseImageOHOS(queue, waitSemaphoreCount,
            pWaitSemaphores, image, pNativeFenceFd);
    } else if (interfaceType == VulkanInterfaceType::BASIC_RENDER) {
        std::lock_guard<std::mutex> lock(vkInterface.graphicsQueueMutex_);
        RS_LOGD("%{public}s queue", __func__);
        RS_OPTIONAL_TRACE_NAME_FMT("%s queue", __func__);
        return vkInterface.vkQueueSignalReleaseImageOHOS(queue,
            waitSemaphoreCount, pWaitSemaphores, image, pNativeFenceFd);
    }
    RS_LOGE("%{public}s abnormal queue occured", __func__);
    return VK_ERROR_UNKNOWN;
}

std::shared_ptr<Drawing::GPUContext> RsVulkanContext::CreateDrawingContext()
{
    static thread_local int tidForRecyclable = gettid();
    {
        std::lock_guard<std::mutex> lock(drawingContextMutex_);
        switch (vulkanInterfaceType_) {
            case VulkanInterfaceType::PROTECTED_REDRAW: {
                auto protectedIter = drawingContextMap_.find(tidForRecyclable);
                if (protectedIter != drawingContextMap_.end() && protectedIter->second.protectContext != nullptr) {
                    return protectedIter->second.protectContext;
                }
                break;
            }
            case VulkanInterfaceType::BASIC_RENDER:
            case VulkanInterfaceType::UNPROTECTED_REDRAW:
            default: {
                auto iter = drawingContextMap_.find(tidForRecyclable);
                if (iter != drawingContextMap_.end() && iter->second.unprotectContext != nullptr) {
                    return iter->second.unprotectContext;
                }
                break;
            }
        }
    }
    return GetRsVulkanInterface().CreateDrawingContext();
}

std::shared_ptr<Drawing::GPUContext> RsVulkanContext::GetDrawingContext()
{
    static thread_local int tidForRecyclable = gettid();
    {
        std::lock_guard<std::mutex> lock(drawingContextMutex_);
        if (isProtected_) {
            auto protectedIter = drawingContextMap_.find(tidForRecyclable);
            if (protectedIter != drawingContextMap_.end() && protectedIter->second.protectContext != nullptr) {
                return protectedIter->second.protectContext;
            }
        } else {
            auto iter = drawingContextMap_.find(tidForRecyclable);
            if (iter != drawingContextMap_.end() && iter->second.unprotectContext != nullptr) {
                return iter->second.unprotectContext;
            }
        }
    }
    return GetRsVulkanInterface().CreateDrawingContext();
}

bool RsVulkanContext::GetIsProtected() const
{
    return isProtected_;
}

void RsVulkanContext::SetIsProtected(bool isProtected)
{
    if (isProtected) {
        vulkanInterfaceType_ = VulkanInterfaceType::PROTECTED_REDRAW;
    } else {
        vulkanInterfaceType_ = VulkanInterfaceType::UNPROTECTED_REDRAW;
    }
    if (isProtected_ != isProtected) {
        RS_LOGW("RsVulkanContext switch, isProtected: %{public}d.", isProtected);
        if (isProtected) {
            RS_TRACE_NAME("RsVulkanContext switch to protected GPU context");
        }
        ClearGrContext(isProtected);
    }
}

bool RsVulkanContext::IsRecyclable()
{
    return isRecyclable_;
}

void RsVulkanContext::SetRecyclable(bool isRecyclable)
{
    isRecyclable_ = isRecyclable;
}

void RsVulkanContext::ClearGrContext(bool isProtected)
{
    RS_TRACE_NAME("RsVulkanContext ClearGrContext");
    GetDrawingContext()->PurgeUnlockedResources(true);
    isProtected_ = isProtected;
    GetDrawingContext()->ResetContext();
}
} // namespace Rosen
} // namespace OHOS
