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
#include <memory>
#include <mutex>
#include <unordered_set>
#include <string_view>
#include <dlfcn.h>
#include <vector>
#include "common/rs_optional_trace.h"
#include "platform/common/rs_log.h"
#include "render_context/memory_handler.h"
#ifdef USE_M133_SKIA
#include "include/gpu/vk/VulkanExtensions.h"
#else
#include "include/gpu/vk/GrVkExtensions.h"
#endif
#include "unistd.h"
#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_ohos.h"
#include "sync_fence.h"

#include "third_party/skia/include/gpu/GrDirectContext.h"
#include "third_party/skia/include/gpu/vk/GrVkBackendContext.h"
#include "third_party/skia/src/gpu/GrDirectContextPriv.h"
#include "third_party/skia/src/gpu/vk/GrVkCommandBuffer.h"
#include "third_party/skia/src/gpu/vk/GrVkGpu.h"
#include "third_party/skia/src/gpu/vk/GrVkSemaphore.h"

#include "hetero_hdr/rs_hdr_pattern_manager.h"

#define ACQUIRE_PROC(name, context)                         \
    if (!(vk##name = AcquireProc("vk" #name, context))) {   \
        ROSEN_LOGE("Could not acquire proc: vk" #name);     \
    }

namespace OHOS {
namespace Rosen {
thread_local bool RsVulkanContext::isProtected_ = false;
thread_local VulkanInterfaceType RsVulkanContext::vulkanInterfaceType_ = VulkanInterfaceType::BASIC_RENDER;
std::map<int, std::pair<std::shared_ptr<Drawing::GPUContext>, bool>> RsVulkanContext::drawingContextMap_;
std::map<int, std::pair<std::shared_ptr<Drawing::GPUContext>, bool>> RsVulkanContext::protectedDrawingContextMap_;
std::mutex RsVulkanContext::drawingContextMutex_;
std::recursive_mutex RsVulkanContext::recyclableSingletonMutex_;
bool RsVulkanContext::isRecyclable_ = true;
std::atomic RsVulkanContext::isRecyclableSingletonValid_ = false;
std::atomic RsVulkanContext::isInited_ = false;
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
};

static const int GR_CHUNK_SIZE = 1048576;
static const int GR_CACHE_MAX_COUNT = 8192;
static const size_t GR_CACHE_MAX_BYTE_SIZE = 96 * (1 << 20);
static const int32_t CACHE_LIMITS_TIMES = 5;  // this will change RS memory!
std::atomic<uint64_t> RsVulkanInterface::callbackSemaphoreInfoCnt_ = 0;
std::atomic<uint64_t> RsVulkanInterface::callbackSemaphoreInfoRSDerefCnt_ = 0;
std::atomic<uint64_t> RsVulkanInterface::callbackSemaphoreInfo2DEngineDerefCnt_ = 0;

void RsVulkanInterface::Init(VulkanInterfaceType vulkanInterfaceType, bool isProtected, bool isHtsEnable)
{
    acquiredMandatoryProcAddresses_ = false;
    memHandler_ = nullptr;
    acquiredMandatoryProcAddresses_ = OpenLibraryHandle() && SetupLoaderProcAddresses();
    interfaceType_ = vulkanInterfaceType;
    CreateInstance();
    SelectPhysicalDevice(isProtected);
    CreateDevice(isProtected, isHtsEnable);
    std::unique_lock<std::mutex> lock(vkMutex_);
    CreateSkiaBackendContext(&backendContext_, isProtected);
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
    ACQUIRE_PROC(EnumerateInstanceLayerProperties, null_instance);
    return true;
}

bool RsVulkanInterface::CreateInstance()
{
    if (!acquiredMandatoryProcAddresses_) {
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
    }

    ACQUIRE_PROC(CreateDevice, instance_);
    ACQUIRE_PROC(DestroyDevice, instance_);
    ACQUIRE_PROC(DestroyInstance, instance_);
    ACQUIRE_PROC(EnumerateDeviceExtensionProperties, instance_);
    ACQUIRE_PROC(EnumerateDeviceLayerProperties, instance_);
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
    if (!instance_) {
        return false;
    }
    uint32_t deviceCount = 0;
    if (vkEnumeratePhysicalDevices(instance_, &deviceCount, nullptr) != VK_SUCCESS) {
        ROSEN_LOGE("vkEnumeratePhysicalDevices failed");
        return false;
    }

    std::vector<VkPhysicalDevice> physicalDevices;
    physicalDevices.resize(deviceCount);
    if (vkEnumeratePhysicalDevices(instance_, &deviceCount, physicalDevices.data()) != VK_SUCCESS) {
        ROSEN_LOGE("vkEnumeratePhysicalDevices failed");
        return false;
    }
    physicalDevice_ = physicalDevices[0];
    VkPhysicalDeviceProperties2 physDevProps = {
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
        physDevProps.pNext = &protMemProps;
    }
    vkGetPhysicalDeviceProperties2(physicalDevice_, &physDevProps);
    return true;
}

void RsVulkanInterface::ConfigureFeatures(bool isProtected)
{
    ycbcrFeature_.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES,
    ycbcrFeature_.pNext = nullptr;
    sync2Feature_.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
    sync2Feature_.pNext = &ycbcrFeature_;
    bindlessFeature_.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
    bindlessFeature_.pNext = &sync2Feature_;
    timelineFeature_.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES;
    timelineFeature_.pNext = &bindlessFeature_;
    physicalDeviceFeatures2_.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    physicalDeviceFeatures2_.pNext = &timelineFeature_;
    void** tailPnext = &ycbcrFeature_.pNext;
    protectedMemoryFeatures_ = new VkPhysicalDeviceProtectedMemoryFeatures;
    if (isProtected) {
        protectedMemoryFeatures_->sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROTECTED_MEMORY_FEATURES;
        protectedMemoryFeatures_->pNext = nullptr;
        *tailPnext = protectedMemoryFeatures_;
        tailPnext = &protectedMemoryFeatures_->pNext;
    }
}

void RsVulkanInterface::ConfigureExtensions()
{
    deviceExtensions_ = gMandatoryDeviceExtensions;
    uint32_t count = 0;
    std::vector<VkExtensionProperties> supportedExtensions;
    if (vkEnumerateDeviceExtensionProperties(physicalDevice_, nullptr, &count, nullptr) != VK_SUCCESS) {
        ROSEN_LOGE("Failed to get device extension count, try to create device with mandatory extensions only!");
        return;
    }
    supportedExtensions.resize(count);
    if (vkEnumerateDeviceExtensionProperties(physicalDevice_, nullptr, &count,
        supportedExtensions.data()) != VK_SUCCESS) {
        ROSEN_LOGE("Failed to get device extensions, try to create device with mandatory extensions only!");
        return;
    }
    std::unordered_set<std::string_view> extensionNames;
    for (auto& prop: supportedExtensions) {
        extensionNames.emplace(prop.extensionName);
    }
    for (auto& ext: gOptionalDeviceExtensions) {
        if (extensionNames.find(ext) != extensionNames.end()) {
            deviceExtensions_.emplace_back(ext);
        }
    }
    for (auto& ext: gMandatoryDeviceExtensions) {
        if (extensionNames.find(ext) == extensionNames.end()) {
            ROSEN_LOGE("Mandatory device extension %{public}s not found! Try to enable it anyway.", ext);
        }
    }
}

bool RsVulkanInterface::CreateDevice(bool isProtected, bool isHtsEnable)
{
    if (!physicalDevice_) {
        return false;
    }
    uint32_t queueCount;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice_, &queueCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueProps(queueCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice_, &queueCount, queueProps.data());

    for (uint32_t i = 0; i < queueCount; i++) {
        if (queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            graphicsQueueFamilyIndex_ = i;
            break;
        }
    }

    if (graphicsQueueFamilyIndex_ == UINT32_MAX) {
        ROSEN_LOGE("graphicsQueueFamilyIndex_ is not valid");
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
        ROSEN_LOGE("vkCreateDevice failed");
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

#ifdef USE_M133_SKIA
bool RsVulkanInterface::CreateSkiaBackendContext(skgpu::VulkanBackendContext* context, bool isProtected)
#else
bool RsVulkanInterface::CreateSkiaBackendContext(GrVkBackendContext* context, bool isProtected)
#endif
{
    auto getProc = CreateSkiaGetProc();
    if (getProc == nullptr) {
        ROSEN_LOGE("CreateSkiaBackendContext getProc is null");
        return false;
    }
#ifndef USE_M133_SKIA
    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(physicalDevice_, &features);

    uint32_t fFeatures = 0;
    if (features.geometryShader) {
        fFeatures |= kGeometryShader_GrVkFeatureFlag;
    }
    if (features.dualSrcBlend) {
        fFeatures |= kDualSrcBlend_GrVkFeatureFlag;
    }
    if (features.sampleRateShading) {
        fFeatures |= kSampleRateShading_GrVkFeatureFlag;
    }
#endif

    context->fInstance = instance_;
    context->fPhysicalDevice = physicalDevice_;
    context->fDevice = device_;
    context->fQueue = queue_;
    context->fGraphicsQueueIndex = graphicsQueueFamilyIndex_;
#ifndef USE_M133_SKIA
    context->fMinAPIVersion = VK_API_VERSION_1_2;

    uint32_t extensionFlags = kKHR_surface_GrVkExtensionFlag;
    extensionFlags |= kKHR_ohos_surface_GrVkExtensionFlag;

    context->fExtensions = extensionFlags;
#else
    context->fMaxAPIVersion = VK_API_VERSION_1_2;
#endif

    skVkExtensions_.init(getProc, instance_, physicalDevice_,
        gInstanceExtensions.size(), gInstanceExtensions.data(),
        deviceExtensions_.size(), deviceExtensions_.data());

    context->fVkExtensions = &skVkExtensions_;
    context->fDeviceFeatures2 = &physicalDeviceFeatures2_;
#ifndef USE_M133_SKIA
    context->fFeatures = fFeatures;
#endif
    context->fGetProc = std::move(getProc);
#ifdef USE_M133_SKIA
    context->fProtectedContext = isProtected ? skgpu::Protected::kYes : skgpu::Protected::kNo;
#else
    context->fOwnsInstanceAndDevice = false;
    context->fProtectedContext = isProtected ? GrProtected::kYes : GrProtected::kNo;
#endif

    return true;
}

bool RsVulkanInterface::SetupDeviceProcAddresses(VkDevice device)
{
    ACQUIRE_PROC(AllocateCommandBuffers, device_);
    ACQUIRE_PROC(AllocateMemory, device_);
    ACQUIRE_PROC(BeginCommandBuffer, device_);
    ACQUIRE_PROC(BindImageMemory, device_);
    ACQUIRE_PROC(BindImageMemory2, device_);
    ACQUIRE_PROC(CmdPipelineBarrier, device_);
    ACQUIRE_PROC(CreateCommandPool, device_);
    ACQUIRE_PROC(CreateFence, device_);
    ACQUIRE_PROC(CreateImage, device_);
    ACQUIRE_PROC(CreateImageView, device_);
    ACQUIRE_PROC(CreateSemaphore, device_);
    ACQUIRE_PROC(DestroyCommandPool, device_);
    ACQUIRE_PROC(DestroyFence, device_);
    ACQUIRE_PROC(DestroyImage, device_);
    ACQUIRE_PROC(DestroyImageView, device_);
    ACQUIRE_PROC(DestroySemaphore, device_);
    ACQUIRE_PROC(DeviceWaitIdle, device_);
    ACQUIRE_PROC(EndCommandBuffer, device_);
    ACQUIRE_PROC(FreeCommandBuffers, device_);
    ACQUIRE_PROC(FreeMemory, device_);
    ACQUIRE_PROC(GetDeviceQueue, device_);
    ACQUIRE_PROC(GetImageMemoryRequirements, device_);
    ACQUIRE_PROC(QueueSubmit, device_);
    ACQUIRE_PROC(QueueWaitIdle, device_);
    ACQUIRE_PROC(ResetCommandBuffer, device_);
    ACQUIRE_PROC(ResetFences, device_);
    ACQUIRE_PROC(WaitForFences, device_);
    ACQUIRE_PROC(GetNativeBufferPropertiesOHOS, device_);
    ACQUIRE_PROC(QueueSignalReleaseImageOHOS, device_);
    ACQUIRE_PROC(ImportSemaphoreFdKHR, device_);
    ACQUIRE_PROC(SetFreqAdjustEnable, device_);
    ACQUIRE_PROC(GetSemaphoreFdKHR, device_);

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

#ifdef USE_M133_SKIA
skgpu::VulkanGetProc RsVulkanInterface::CreateSkiaGetProc() const
#else
GrVkGetProc RsVulkanInterface::CreateSkiaGetProc() const
#endif
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

std::shared_ptr<Drawing::GPUContext> RsVulkanInterface::CreateDrawingContext(std::string cacheDir)
{
    std::unique_lock<std::mutex> lock(vkMutex_);

    auto drawingContext = std::make_shared<Drawing::GPUContext>();
    Drawing::GPUContextOptions options;
    memHandler_ = std::make_unique<MemoryHandler>();
    std::string vkVersion = std::to_string(VK_API_VERSION_1_2);
    auto size = vkVersion.size();
    memHandler_->ConfigureContext(&options, vkVersion.c_str(), size, cacheDir);
    drawingContext->BuildFromVK(backendContext_, options);
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
    return drawingContext;
}

std::shared_ptr<Drawing::GPUContext> RsVulkanInterface::CreateDrawingContext(std::string cacheDir)
{
    auto drawingContext = DoCreateDrawingContext(cacheDir);
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
        // 120 : 120fps print once every 1s.
        if (RsVulkanInterface::callbackSemaphoreInfoCnt_.load() % 120 == 0) {
            RS_LOGI("used fences, dup fence count[%{public}" PRIu64 "], rs deref count[%{public}" PRIu64 "],"
                "skia deref count[%{public}" PRIu64 "], wait close fence count[%{public}zu]",
                RsVulkanInterface::callbackSemaphoreInfoCnt_.load(),
                RsVulkanInterface::callbackSemaphoreInfoRSDerefCnt_.load(),
                RsVulkanInterface::callbackSemaphoreInfo2DEngineDerefCnt_.load(),
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

void RsVulkanInterface::SendSemaphoreWithFd(VkSemaphore semaphore, int fenceFd)
{
    std::lock_guard<std::mutex> lock(semaphoreLock_);
    auto& semaphoreFence = usedSemaphoreFenceList_.emplace_back();
    semaphoreFence.semaphore = semaphore;
    semaphoreFence.fence = (fenceFd != -1 ? std::make_unique<SyncFence>(fenceFd) : nullptr);
}

RsVulkanContext::RsVulkanContext(std::string cacheDir)
{
    vulkanInterfaceVec_.resize(size_t(VulkanInterfaceType::MAX_INTERFACE_TYPE));
    if (RsVulkanContext::IsRecyclable()) {
        InitVulkanContextForHybridRender(cacheDir);
    } else {
        InitVulkanContextForUniRender(cacheDir);
    }
    RsVulkanContext::isInited_ = true;
    RsVulkanContext::isRecyclableSingletonValid_ = true;
}

RsVulkanContext::~RsVulkanContext()
{
    std::lock_guard<std::mutex> lock(drawingContextMutex_);
    drawingContextMap_.clear();
    protectedDrawingContextMap_.clear();
    RsVulkanContext::isRecyclableSingletonValid_ = false;
}

void RsVulkanContext::InitVulkanContextForHybridRender(const std::string& cacheDir)
{
    auto vulkanInterface = std::make_shared<RsVulkanInterface>();
    vulkanInterface->Init(VulkanInterfaceType::BASIC_RENDER, false);
    // init drawing context for RT thread bind to backendContext.
    vulkanInterface->CreateDrawingContext(cacheDir);

    vulkanInterfaceVec_[size_t(VulkanInterfaceType::BASIC_RENDER)] = std::move(vulkanInterface);
}

void RsVulkanContext::InitVulkanContextForUniRender(const std::string& cacheDir)
{
    // create vulkan interface for render thread.
    auto uniRenderVulkanInterface = std::make_shared<RsVulkanInterface>();
    uniRenderVulkanInterface->Init(VulkanInterfaceType::BASIC_RENDER, false, true);
    // init drawing context for RT thread bind to backendContext.
    uniRenderVulkanInterface->CreateDrawingContext(cacheDir);
    // create vulkan interface for hardware thread (unprotected).
    auto unprotectedReDrawVulkanInterface = std::make_shared<RsVulkanInterface>();
    unprotectedReDrawVulkanInterface->Init(VulkanInterfaceType::UNPROTECTED_REDRAW, false, false);
    vulkanInterfaceVec_[size_t(VulkanInterfaceType::BASIC_RENDER)] = std::move(uniRenderVulkanInterface);
    vulkanInterfaceVec_[size_t(VulkanInterfaceType::UNPROTECTED_REDRAW)] = std::move(unprotectedReDrawVulkanInterface);
#ifdef IS_ENABLE_DRM
    isProtected_ = true;
    auto protectedReDrawVulkanInterface = std::make_shared<RsVulkanInterface>();
    protectedReDrawVulkanInterface->Init(VulkanInterfaceType::PROTECTED_REDRAW, true, false);
    // DRM needs to adapt vkQueue in the future.
    protectedReDrawVulkanInterface->CreateDrawingContext(cacheDir);
    vulkanInterfaceVec_[size_t(VulkanInterfaceType::PROTECTED_REDRAW)] = std::move(protectedReDrawVulkanInterface);
    isProtected_ = false;
#endif
}

std::unique_ptr<RsVulkanContext>& RsVulkanContext::GetRecyclableSingletonPtr(const std::string& cacheDir)
{
    std::lock_guard<std::recursive_mutex> lock(recyclableSingletonMutex_);
    static std::unique_ptr<RsVulkanContext> recyclableSingleton = std::make_unique<RsVulkanContext>(cacheDir);
    return recyclableSingleton;
}

RsVulkanContext& RsVulkanContext::GetRecyclableSingleton(const std::string& cacheDir)
{
    std::lock_guard<std::recursive_mutex> lock(recyclableSingletonMutex_);
    static std::string cacheDirInit = cacheDir;
    std::unique_ptr<RsVulkanContext>& recyclableSingleton = GetRecyclableSingletonPtr(cacheDirInit);
    if (recyclableSingleton == nullptr) {
        recyclableSingleton = std::make_unique<RsVulkanContext>(cacheDirInit);
    }
    return *recyclableSingleton;
}

RsVulkanContext& RsVulkanContext::GetSingleton(const std::string& cacheDir)
{
    if (isRecyclable_) {
        return RsVulkanContext::GetRecyclableSingleton(cacheDir);
    }
    static RsVulkanContext singleton = RsVulkanContext(cacheDir);
    return singleton;
}

bool RsVulkanContext::CheckDrawingContextRecyclable()
{
    std::lock_guard<std::mutex> lock(drawingContextMutex_);
    for (const auto& iter : RsVulkanContext::drawingContextMap_) {
        // check the tag only set to true when GetRecyclableDrawingContext
        if (!iter.second.second) {
            return false;
        }
    }
    for (const auto& iter : RsVulkanContext::protectedDrawingContextMap_) {
        // check the tag only set to true when GetRecyclableDrawingContext
        if (!iter.second.second) {
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
    auto& drawingContextMap = isProtected_ ?
        RsVulkanContext::protectedDrawingContextMap_ : RsVulkanContext::drawingContextMap_;
    std::lock_guard<std::mutex> lock(drawingContextMutex_);
    auto iter = drawingContextMap.find(tidForRecyclable);
    if (iter != drawingContextMap.end()) {
        iter->second.second = true;
    }
    return drawingContext;
}

void RsVulkanContext::ReleaseDrawingContextMap()
{
    std::lock_guard<std::mutex> lock(drawingContextMutex_);
    for (auto& iter : drawingContextMap_) {
        auto context = iter.second.first;
        if (context == nullptr) {
            continue;
        }
        context->FlushAndSubmit(true);
    }
    drawingContextMap_.clear();

    for (auto& protectedIter : protectedDrawingContextMap_) {
        auto protectedContext = protectedIter.second.first;
        if (protectedContext == nullptr) {
            continue;
        }
        protectedContext->FlushAndSubmit(true);
    }
    protectedDrawingContextMap_.clear();
}

void RsVulkanContext::ReleaseRecyclableDrawingContext()
{
    auto& drawingContextMap = isProtected_ ?
        RsVulkanContext::protectedDrawingContextMap_ : RsVulkanContext::drawingContextMap_;
    std::lock_guard<std::mutex> lock(drawingContextMutex_);
    for (auto iter = drawingContextMap.begin(); iter != drawingContextMap.end();) {
        if (iter->second.second) {
            iter = drawingContextMap.erase(iter);
        } else {
            ++iter;
        }
    }
}

void RsVulkanContext::ReleaseDrawingContextForThread(int tid)
{
    std::lock_guard<std::mutex> lock(drawingContextMutex_);
    drawingContextMap_.erase(tid);
    protectedDrawingContextMap_.erase(tid);
}

void RsVulkanContext::SaveNewDrawingContext(int tid, std::shared_ptr<Drawing::GPUContext> drawingContext)
{
    std::lock_guard<std::mutex> lock(drawingContextMutex_);
    static thread_local auto func = [tid]() {
        RsVulkanContext::ReleaseDrawingContextForThread(tid);
    };
    static thread_local auto drawContextHolder = std::make_shared<DrawContextHolder>(func);
    if (isProtected_) {
        protectedDrawingContextMap_[tid] = std::make_pair(drawingContext, false);
    } else {
        drawingContextMap_[tid] = std::make_pair(drawingContext, false);
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
        RS_LOGD("%{public}s queue", __func__);
        RS_OPTIONAL_TRACE_NAME_FMT("%s queue", __func__);
        return vkInterface.vkQueueSubmit(queue, submitCount, pSubmits, fence);
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
                // protectedDrawingContextMap_ : <tid, <drawingContext, isRecyclable>>
                auto protectedIter = protectedDrawingContextMap_.find(tidForRecyclable);
                if (protectedIter != protectedDrawingContextMap_.end() && protectedIter->second.first != nullptr) {
                    return protectedIter->second.first;
                }
                break;
            }
            case VulkanInterfaceType::BASIC_RENDER:
            case VulkanInterfaceType::UNPROTECTED_REDRAW:
            default: {
                // drawingContextMap_ : <tid, <drawingContext, isRecyclable>>
                auto iter = drawingContextMap_.find(tidForRecyclable);
                if (iter != drawingContextMap_.end() && iter->second.first != nullptr) {
                    return iter->second.first;
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
            // protectedDrawingContextMap_ : <tid, <drawingContext, isRecyclable>>
            auto protectedIter = protectedDrawingContextMap_.find(tidForRecyclable);
            if (protectedIter != protectedDrawingContextMap_.end() && protectedIter->second.first != nullptr) {
                return protectedIter->second.first;
            }
        } else {
            // drawingContextMap_ : <tid, <drawingContext, isRecyclable>>
            auto iter = drawingContextMap_.find(tidForRecyclable);
            if (iter != drawingContextMap_.end() && iter->second.first != nullptr) {
                return iter->second.first;
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
}
}
