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
#include <set>
#include <dlfcn.h>
#include <vector>
#include "platform/common/rs_log.h"
#include "render_context/memory_handler.h"
#include "include/gpu/vk/GrVkExtensions.h"
#include "unistd.h"
#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_ohos.h"

#define ACQUIRE_PROC(name, context)                         \
    if (!(vk##name = AcquireProc("vk" #name, context))) {   \
        ROSEN_LOGE("Could not acquire proc: vk" #name);     \
        return false;                                       \
    }

namespace OHOS {
namespace Rosen {

thread_local std::shared_ptr<Drawing::GPUContext> RsVulkanContext::drawingContext_ = nullptr;
thread_local std::shared_ptr<Drawing::GPUContext> RsVulkanContext::protectedDrawingContext_ = nullptr;
thread_local bool RsVulkanContext::isProtected_ = false;
static RsVulkanInterface rsVulkanInterface;
static RsVulkanInterface rsProtectedVulkanInterface;
static RsVulkanInterface& GetRsVulkanInterfaceInternal(bool isProtected)
{
    return isProtected ? rsProtectedVulkanInterface : rsVulkanInterface;
}

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

void RsVulkanInterface::Init(bool isProtected)
{
    handle_ = nullptr;
    acquiredMandatoryProcAddresses_ = false;
    memHandler_ = nullptr;
    acquiredMandatoryProcAddresses_ = OpenLibraryHandle() && SetupLoaderProcAddresses();
    CreateInstance();
    SelectPhysicalDevice(isProtected);
    CreateDevice(isProtected);
    std::unique_lock<std::mutex> lock(vkMutex_);
    CreateSkiaBackendContext(&backendContext_, false, isProtected);
}

RsVulkanInterface::~RsVulkanInterface()
{
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
    const float priorities[1] = {1.0f};
    VkDeviceQueueCreateFlags deviceQueueCreateFlags = isProtected ? VK_DEVICE_QUEUE_CREATE_PROTECTED_BIT : 0;
    std::vector<VkDeviceQueueCreateInfo> queueCreate {{
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, .pNext = nullptr,
        .flags = deviceQueueCreateFlags, .queueFamilyIndex = graphicsQueueFamilyIndex_, .queueCount = 2,
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
    const VkDeviceQueueInfo2 deviceQueueInfo2HardW = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2, nullptr,
        deviceQueueCreateFlags, static_cast<uint32_t>(graphicsQueueFamilyIndex_), 1};
    vkGetDeviceQueue2(device_, &deviceQueueInfo2, &queue_);
    vkGetDeviceQueue2(device_, &deviceQueueInfo2HardW, &hardwareQueue_);
    return true;
}

bool RsVulkanInterface::CreateSkiaBackendContext(GrVkBackendContext* context, bool createNew, bool isProtected)
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
    if (createNew) {
        context->fQueue = hardwareQueue_;
    } else {
        context->fQueue = queue_;
    }
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
    context->fOwnsInstanceAndDevice = false;
    context->fProtectedContext = isProtected ? GrProtected::kYes : GrProtected::kNo;

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

    return true;
}

bool RsVulkanInterface::OpenLibraryHandle()
{
    ROSEN_LOGI("VulkanProcTable OpenLibararyHandle: dlopen libvulkan.so.");
    dlerror();
    handle_ = dlopen("/system/lib64/platformsdk/libvulkan.so", RTLD_NOW | RTLD_LOCAL);
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

GrVkGetProc RsVulkanInterface::CreateSkiaGetProc() const
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

std::shared_ptr<Drawing::GPUContext> RsVulkanInterface::CreateDrawingContext(bool independentContext, bool isProtected)
{
    std::unique_lock<std::mutex> lock(vkMutex_);
    if (independentContext) {
        return CreateNewDrawingContext(isProtected);
    }

    auto drawingContext = std::make_shared<Drawing::GPUContext>();
    Drawing::GPUContextOptions options;
    memHandler_ = std::make_shared<MemoryHandler>();
    std::string vkVersion = std::to_string(VK_API_VERSION_1_2);
    auto size = vkVersion.size();
    memHandler_->ConfigureContext(&options, vkVersion.c_str(), size);
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

std::shared_ptr<Drawing::GPUContext> RsVulkanInterface::CreateNewDrawingContext(bool isProtected)
{
    CreateSkiaBackendContext(&hbackendContext_, true, isProtected);
    auto drawingContext = std::make_shared<Drawing::GPUContext>();
    Drawing::GPUContextOptions options;
    memHandler_ = std::make_shared<MemoryHandler>();
    std::string vkVersion = std::to_string(VK_API_VERSION_1_2);
    auto size = vkVersion.size();
    memHandler_->ConfigureContext(&options, vkVersion.c_str(), size);
    drawingContext->BuildFromVK(hbackendContext_, options);
    int maxResources = 0;
    size_t maxResourcesSize = 0;
    int cacheLimitsTimes = CACHE_LIMITS_TIMES;
    drawingContext->GetResourceCacheLimits(&maxResources, &maxResourcesSize);
    if (maxResourcesSize > 0) {
        drawingContext->SetResourceCacheLimits(cacheLimitsTimes * maxResources, cacheLimitsTimes *
            std::fmin(maxResourcesSize, GR_CACHE_MAX_BYTE_SIZE));
    } else {
        drawingContext->SetResourceCacheLimits(GR_CACHE_MAX_COUNT, GR_CACHE_MAX_BYTE_SIZE);
    }
    hcontext_ = drawingContext;
    return drawingContext;
}

RsVulkanContext::RsVulkanContext()
{
    rsVulkanInterface.Init();
    drawingContext_ = rsVulkanInterface.CreateDrawingContext();
    isProtected_ = true;
    rsProtectedVulkanInterface.Init(true);
    protectedDrawingContext_ = rsProtectedVulkanInterface.CreateDrawingContext(false, true);
    isProtected_ = false;
}

RsVulkanContext& RsVulkanContext::GetSingleton()
{
    static RsVulkanContext singleton {};
    return singleton;
}

RsVulkanInterface& RsVulkanContext::GetRsVulkanInterface()
{
    return GetRsVulkanInterfaceInternal(isProtected_);
}

VKAPI_ATTR VkResult RsVulkanContext::HookedVkQueueSubmit(VkQueue queue, uint32_t submitCount,
    VkSubmitInfo* pSubmits, VkFence fence)
{
    bool isProtected = RsVulkanContext::GetSingleton().GetIsProtected();
    VkProtectedSubmitInfo protectedSubmitInfo;
    if (isProtected) {
        memset_s(&protectedSubmitInfo, sizeof(VkProtectedSubmitInfo), 0, sizeof(VkProtectedSubmitInfo));
        protectedSubmitInfo.sType = VK_STRUCTURE_TYPE_PROTECTED_SUBMIT_INFO;
        protectedSubmitInfo.pNext = nullptr;
        protectedSubmitInfo.protectedSubmit = VK_TRUE;
        pSubmits->pNext = &protectedSubmitInfo;
    }

    RsVulkanInterface& vkInterface = RsVulkanContext::GetSingleton().GetRsVulkanInterface();
    if (queue == vkInterface.GetHardwareQueue()) {
        return vkInterface.vkQueueSubmit(queue, submitCount, pSubmits, fence);
    }
    std::lock_guard<std::mutex> lock(vkInterface.graphicsQueueMutex_);
    return vkInterface.vkQueueSubmit(queue, submitCount, pSubmits, fence);
}

VKAPI_ATTR VkResult RsVulkanContext::HookedVkQueueSignalReleaseImageOHOS(VkQueue queue, uint32_t waitSemaphoreCount,
    const VkSemaphore* pWaitSemaphores, VkImage image, int32_t* pNativeFenceFd)
{
    RsVulkanInterface& vkInterface = RsVulkanContext::GetSingleton().GetRsVulkanInterface();
    if (queue == vkInterface.GetHardwareQueue()) {
        return vkInterface.vkQueueSignalReleaseImageOHOS(queue, waitSemaphoreCount,
            pWaitSemaphores, image, pNativeFenceFd);
    }
    std::lock_guard<std::mutex> lock(vkInterface.graphicsQueueMutex_);
    return vkInterface.vkQueueSignalReleaseImageOHOS(queue, waitSemaphoreCount, pWaitSemaphores, image, pNativeFenceFd);
}

std::shared_ptr<Drawing::GPUContext> RsVulkanContext::CreateDrawingContext(bool independentContext)
{
    auto& drawingContext = isProtected_ ? protectedDrawingContext_ : drawingContext_;
    if (drawingContext != nullptr && !independentContext) {
        return drawingContext;
    }
    drawingContext = GetRsVulkanInterface().CreateDrawingContext(independentContext, isProtected_);
    return drawingContext;
}

std::shared_ptr<Drawing::GPUContext> RsVulkanContext::GetDrawingContext()
{
    auto& drawingContext = isProtected_ ? protectedDrawingContext_ : drawingContext_;
    if (drawingContext != nullptr) {
        return drawingContext;
    }
    drawingContext = GetRsVulkanInterface().CreateDrawingContext(false, isProtected_);
    return drawingContext;
}

void RsVulkanContext::SetIsProtected(bool isProtected)
{
    if (isProtected_ != isProtected) {
        ClearGrContext(isProtected);
    }
}

void RsVulkanContext::ClearGrContext(bool isProtected)
{
    GetDrawingContext()->PurgeUnlockedResources(true);
    isProtected_ = isProtected;
    GetDrawingContext()->ResetContext();
}
}
}
