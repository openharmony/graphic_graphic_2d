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
#include <set>
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

#define ACQUIRE_PROC(name, context)                         \
    if (!(vk##name = AcquireProc("vk" #name, context))) {   \
        ROSEN_LOGE("Could not acquire proc: vk" #name);     \
    }

namespace OHOS {
namespace Rosen {

thread_local std::shared_ptr<Drawing::GPUContext> RsVulkanContext::drawingContext_ = nullptr;
thread_local std::shared_ptr<Drawing::GPUContext> RsVulkanContext::protectedDrawingContext_ = nullptr;
thread_local bool RsVulkanContext::isProtected_ = false;
thread_local VulkanInterfaceType RsVulkanContext::vulkanInterfaceType_ = VulkanInterfaceType::UNI_RENDER;
void* RsVulkanInterface::handle_ = nullptr;

static std::vector<const char*> gInstanceExtensions = {
    VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
};

static std::vector<const char*> gDeviceExtensions = {
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

static const int GR_CACHE_MAX_COUNT = 8192;
static const size_t GR_CACHE_MAX_BYTE_SIZE = 96 * (1 << 20);
static const int32_t CACHE_LIMITS_TIMES = 5;  // this will change RS memory!

void RsVulkanInterface::Init(VulkanInterfaceType vulkanInterfaceType, bool isProtected)
{
    acquiredMandatoryProcAddresses_ = false;
    memHandler_ = nullptr;
    acquiredMandatoryProcAddresses_ = OpenLibraryHandle() && SetupLoaderProcAddresses();
    interfaceType_ = vulkanInterfaceType;
    CreateInstance();
    SelectPhysicalDevice(isProtected);
    CreateDevice(isProtected);
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

    ACQUIRE_PROC(CreateDevice, instance_);
    ACQUIRE_PROC(DestroyDevice, instance_);
    ACQUIRE_PROC(DestroyInstance, instance_);
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

bool RsVulkanInterface::CreateDevice(bool isProtected)
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
    ycbcrFeature_.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES,
    ycbcrFeature_.pNext = nullptr;
    physicalDeviceFeatures2_.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    physicalDeviceFeatures2_.pNext = &ycbcrFeature_;
    void** tailPnext = &ycbcrFeature_.pNext;
    protectedMemoryFeatures_ = new VkPhysicalDeviceProtectedMemoryFeatures;
    if (isProtected) {
        protectedMemoryFeatures_->sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROTECTED_MEMORY_FEATURES;
        protectedMemoryFeatures_->pNext = nullptr;
        *tailPnext = protectedMemoryFeatures_;
        tailPnext = &protectedMemoryFeatures_->pNext;
    }

    vkGetPhysicalDeviceFeatures2(physicalDevice_, &physicalDeviceFeatures2_);

    const VkDeviceCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO, .pNext = &physicalDeviceFeatures2_,
        .flags = 0, .queueCreateInfoCount = queueCreate.size(), .pQueueCreateInfos = queueCreate.data(),
        .enabledLayerCount = 0, .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = static_cast<uint32_t>(gDeviceExtensions.size()),
        .ppEnabledExtensionNames = gDeviceExtensions.data(), .pEnabledFeatures = nullptr,
    };
    if (vkCreateDevice(physicalDevice_, &createInfo, nullptr, &device_) != VK_SUCCESS) {
        ROSEN_LOGE("vkCreateDevice failed");
        return false;
    }
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

    context->fInstance = instance_;
    context->fPhysicalDevice = physicalDevice_;
    context->fDevice = device_;
    context->fQueue = queue_;
    context->fGraphicsQueueIndex = graphicsQueueFamilyIndex_;
    context->fMinAPIVersion = VK_API_VERSION_1_2;

    uint32_t extensionFlags = kKHR_surface_GrVkExtensionFlag;
    extensionFlags |= kKHR_ohos_surface_GrVkExtensionFlag;

    context->fExtensions = extensionFlags;

    skVkExtensions_.init(getProc, instance_, physicalDevice_,
        gInstanceExtensions.size(), gInstanceExtensions.data(),
        gDeviceExtensions.size(), gDeviceExtensions.data());

    context->fVkExtensions = &skVkExtensions_;
    context->fDeviceFeatures2 = &physicalDeviceFeatures2_;
    context->fFeatures = fFeatures;
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

std::shared_ptr<Drawing::GPUContext> RsVulkanInterface::CreateDrawingContext(std::string.cacheDir)
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


void RsVulkanInterface::DestroyAllSemaphoreFence()
{
    std::lock_guard<std::mutex> lock(semaphoreLock_);
    RS_LOGE("Device lost clear all semaphore fences, count [%{public}zu] ", usedSemaphoreFenceList_.size());
    for (auto&& semaphoreFence : usedSemaphoreFenceList_) {
        vkDestroySemaphore(device_, semaphoreFence.semaphore, nullptr);
    }
    usedSemaphoreFenceList_.clear();
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
    vulkanInterfaceVec.resize(size_t(VulkanInterfaceType::MAX_INTERFACE_TYPE));
    // create vulkan interface for render thread.
    auto uniRenderVulkanInterface = std::make_shared<RsVulkanInterface>();
    uniRenderVulkanInterface->Init(VulkanInterfaceType::UNI_RENDER, false);
    // init drawing context for RT thread bind to backendContext.
    drawingContext_ = uniRenderVulkanInterface->CreateDrawingContext(cacheDir);
    // create vulkan interface for hardware thread (unprotected).
    auto unprotectedReDrawVulkanInterface = std::make_shared<RsVulkanInterface>();
    unprotectedReDrawVulkanInterface->Init(VulkanInterfaceType::UNPROTECTED_REDRAW, false);
    vulkanInterfaceVec[size_t(VulkanInterfaceType::UNI_RENDER)] = std::move(uniRenderVulkanInterface);
    vulkanInterfaceVec[size_t(VulkanInterfaceType::UNPROTECTED_REDRAW)] = std::move(unprotectedReDrawVulkanInterface);
#ifdef IS_ENABLE_DRM
    isProtected_ = true;
    auto protectedReDrawVulkanInterface = std::make_shared<RsVulkanInterface>();
    protectedReDrawVulkanInterface->Init(VulkanInterfaceType::PROTECTED_REDRAW, true);
    // DRM needs to adapt vkQueue in the future.
    protectedDrawingContext_ = protectedReDrawVulkanInterface->CreateDrawingContext(cacheDir);
    vulkanInterfaceVec[size_t(VulkanInterfaceType::PROTECTED_REDRAW)] = std::move(protectedReDrawVulkanInterface);
    isProtected_ = false;
#endif
}

RsVulkanContext& RsVulkanContext::GetSingleton(const std::string& cacheDir)
{
    static RsVulkanContext singleton = RsVulkanContext(cacheDir);
    return singleton;
}

RsVulkanInterface& RsVulkanContext::GetRsVulkanInterface()
{
    switch (vulkanInterfaceType_) {
        case VulkanInterfaceType::PROTECTED_REDRAW:
            return *(vulkanInterfaceVec[size_t(VulkanInterfaceType::PROTECTED_REDRAW)].get());
        case VulkanInterfaceType::UNPROTECTED_REDRAW:
            return *(vulkanInterfaceVec[size_t(VulkanInterfaceType::UNPROTECTED_REDRAW)].get());
        case VulkanInterfaceType::UNI_RENDER:
        default:
            return *(vulkanInterfaceVec[size_t(VulkanInterfaceType::UNI_RENDER)].get());
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
    } else if (interfaceType == VulkanInterfaceType::UNI_RENDER) {
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
    } else if (interfaceType == VulkanInterfaceType::UNI_RENDER) {
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
    switch (vulkanInterfaceType_) {
        case VulkanInterfaceType::PROTECTED_REDRAW:
            if (protectedDrawingContext_) {
                return protectedDrawingContext_;
            }
            protectedDrawingContext_ = GetRsVulkanInterface().CreateDrawingContext();
            return protectedDrawingContext_;
        case VulkanInterfaceType::UNI_RENDER:
        case VulkanInterfaceType::UNPROTECTED_REDRAW:
        default:
            if (drawingContext_) {
                return drawingContext_;
            }
            drawingContext_ = GetRsVulkanInterface().CreateDrawingContext();
            return drawingContext_;
    }
}

std::shared_ptr<Drawing::GPUContext> RsVulkanContext::GetDrawingContext()
{
    auto& drawingContext = isProtected_ ? protectedDrawingContext_ : drawingContext_;
    if (drawingContext != nullptr) {
        return drawingContext;
    }
    drawingContext = GetRsVulkanInterface().CreateDrawingContext();
    return drawingContext;
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

void RsVulkanContext::ClearGrContext(bool isProtected)
{
    RS_TRACE_NAME("RsVulkanContext ClearGrContext");
    GetDrawingContext()->PurgeUnlockedResources(true);
    isProtected_ = isProtected;
    GetDrawingContext()->ResetContext();
}
}
}
