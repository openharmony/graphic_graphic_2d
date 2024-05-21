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

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <limits>
#include <thread>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <malloc.h>
#include <algorithm>
#include <new>
#include <securec.h>
#include <cstring>
#include <cinttypes>
#include <refbase.h>

#include <vulkan/vulkan_core.h>
#include <window.h>
#include <graphic_common.h>
#include <native_window.h>
#include <vulkan/vulkan.h>
#include "vk_dispatch_table_helper.h"
#include "vk_layer_dispatch_table.h"
#include "swapchain_layer_log.h"
#include "sync_fence.h"
#if USE_APS_IGAMESERVICE_FUNC
#include "vulkan_slice_report.h"
#endif

#define SWAPCHAIN_SURFACE_NAME "VK_LAYER_OHOS_surface"
using namespace OHOS;

enum Extension {
    OHOS_SURFACE,
    OHOS_NATIVE_BUFFER,
    KHR_SURFACE,
    KHR_SWAPCHAIN,
    EXT_SWAPCHAIN_COLOR_SPACE,
    KHR_GET_SURFACE_CAPABILITIES_2,
    EXT_HDR_METADATA,
    EXT_SWAPCHAIN_MAINTENANCE_1,
    EXTENSION_COUNT,
    EXTENSION_UNKNOWN,
};

struct LayerData {
    VkInstance instance = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    uint32_t instanceVersion = VK_API_VERSION_1_0;
    std::unique_ptr<VkLayerDispatchTable> deviceDispatchTable;
    std::unique_ptr<VkLayerInstanceDispatchTable> instanceDispatchTable;
    std::unordered_map<VkDebugUtilsMessengerEXT, VkDebugUtilsMessengerCreateInfoEXT> debugCallbacks;
    PFN_vkSetDeviceLoaderData fpSetDeviceLoaderData = nullptr;
    std::bitset<Extension::EXTENSION_COUNT> enabledExtensions;
};

namespace {
constexpr uint32_t MIN_BUFFER_SIZE = SURFACE_DEFAULT_QUEUE_SIZE;
constexpr uint32_t MAX_BUFFER_SIZE = SURFACE_MAX_QUEUE_SIZE;
struct LoaderVkLayerDispatchTable;
typedef uintptr_t DispatchKey;

template <typename T>
inline DispatchKey GetDispatchKey(const T object)
{
    return reinterpret_cast<DispatchKey>(*reinterpret_cast<LoaderVkLayerDispatchTable* const*>(object));
}

VkLayerInstanceCreateInfo* GetChainInfo(const VkInstanceCreateInfo* pCreateInfo, VkLayerFunction func)
{
    auto chainInfo = static_cast<const VkLayerInstanceCreateInfo*>(pCreateInfo->pNext);
    while (chainInfo != nullptr) {
        if (chainInfo->sType == VK_STRUCTURE_TYPE_LOADER_INSTANCE_CREATE_INFO && chainInfo->function == func) {
            return const_cast<VkLayerInstanceCreateInfo*>(chainInfo);
        }
        chainInfo = static_cast<const VkLayerInstanceCreateInfo*>(chainInfo->pNext);
    }
    SWLOGE("SwapchainLayer Find VkLayerInstanceCreateInfo Failed");
    return nullptr;
}

VkLayerDeviceCreateInfo* GetChainInfo(const VkDeviceCreateInfo* pCreateInfo, VkLayerFunction func)
{
    auto chainInfo = static_cast<const VkLayerDeviceCreateInfo*>(pCreateInfo->pNext);
    while (chainInfo != nullptr) {
        if (chainInfo->sType == VK_STRUCTURE_TYPE_LOADER_DEVICE_CREATE_INFO && chainInfo->function == func) {
            return const_cast<VkLayerDeviceCreateInfo*>(chainInfo);
        }
        chainInfo = static_cast<const VkLayerDeviceCreateInfo*>(chainInfo->pNext);
    }
    SWLOGE("SwapchainLayer Find VkLayerDeviceCreateInfo Failed");
    return nullptr;
}
}  // namespace

#define VK_LAYER_API_VERSION VK_MAKE_VERSION(1, 1, VK_HEADER_VERSION)

static inline uint32_t ToUint32(uint64_t val)
{
    if (val > UINT32_MAX) {
        SWLOGE("%{public}" PRIu64 " is too large to convert to uint32", val);
    }
    return static_cast<uint32_t>(val);
}

namespace SWAPCHAIN {
std::unordered_map<DispatchKey, LayerData*> g_layerDataMap;
const VkSurfaceTransformFlagsKHR g_supportedTransforms =
    VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR |
    VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR |
    VK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR |
    VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR |
    VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_BIT_KHR |
    VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR |
    VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR |
    VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR |
    VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR;

LayerData* GetLayerDataPtr(DispatchKey dataKey)
{
    LayerData* layerData = nullptr;
    auto it = g_layerDataMap.find(dataKey);
    if (it == g_layerDataMap.end()) {
        layerData = new LayerData;
        g_layerDataMap[dataKey] = layerData;
    } else {
        layerData = it->second;
    }
    return layerData;
}

void FreeLayerDataPtr(DispatchKey dataKey)
{
    auto it = g_layerDataMap.find(dataKey);
    if (it == g_layerDataMap.end()) {
        return;
    }
    delete it->second;
    g_layerDataMap.erase(it);
}

void DebugMessageToUserCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, const char* message)
{
    VkDebugUtilsMessengerCallbackDataEXT callbackData = {};
    callbackData.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CALLBACK_DATA_EXT;
    callbackData.pMessageIdName = "SwapchainLayer";
    callbackData.pMessage = message;

    for (auto [key, layerData] : g_layerDataMap) {
        if (layerData->debugCallbacks.empty()) {
            continue;
        }
        for (auto& callback : layerData->debugCallbacks) {
            if (!(severity & callback.second.messageSeverity)) {
                continue;
            }
            if (!(VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT & callback.second.messageType)) {
                continue;
            }
            callback.second.pfnUserCallback(severity, VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT,
                                            &callbackData, callback.second.pUserData);
        }
    }
}

VkResult GetExtensionProperties(const uint32_t count, const VkExtensionProperties* layerExtensions,
                                uint32_t* pCount, VkExtensionProperties* pProperties)
{
    if (pProperties == nullptr || layerExtensions == nullptr) {
        *pCount = count;
        return VK_SUCCESS;
    }

    uint32_t copySize = *pCount < count ? *pCount : count;
    errno_t ret = memcpy_s(pProperties, copySize * sizeof(VkExtensionProperties),
                           layerExtensions, copySize * sizeof(VkExtensionProperties));
    if (ret != EOK) {
        return VK_INCOMPLETE;
    }
    *pCount = copySize;
    if (copySize < count) {
        return VK_INCOMPLETE;
    }

    return VK_SUCCESS;
}

VkResult GetLayerProperties(const uint32_t count, const VkLayerProperties* layerProperties,
                            uint32_t* pCount, VkLayerProperties* pProperties)
{
    if (pProperties == nullptr || layerProperties == nullptr) {
        *pCount = count;
        return VK_SUCCESS;
    }

    uint32_t copySize = *pCount < count ? *pCount : count;
    errno_t ret = memcpy_s(pProperties, copySize * sizeof(VkLayerProperties),
                           layerProperties, copySize * sizeof(VkLayerProperties));
    if (ret != EOK) {
        return VK_INCOMPLETE;
    }
    *pCount = copySize;
    if (copySize < count) {
        return VK_INCOMPLETE;
    }

    return VK_SUCCESS;
}

static const VkExtensionProperties g_instanceExtensions[] = {
    {
        .extensionName = VK_KHR_SURFACE_EXTENSION_NAME,
        .specVersion = 25,
    },
    {
        .extensionName = VK_OHOS_SURFACE_EXTENSION_NAME,
        .specVersion = 1,
    },
    {
        .extensionName = VK_EXT_SWAPCHAIN_COLOR_SPACE_EXTENSION_NAME,
        .specVersion = 4,
    },
    {
        .extensionName = VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME,
        .specVersion = 1,
    }
};

static const VkExtensionProperties g_deviceExtensions[] = {
    {
        .extensionName = VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        .specVersion = 70,
    },
    {
        .extensionName = VK_EXT_HDR_METADATA_EXTENSION_NAME,
        .specVersion = 2,
    },
    {
        .extensionName = VK_EXT_SWAPCHAIN_MAINTENANCE_1_EXTENSION_NAME,
        .specVersion = 1,
    }
};

constexpr VkLayerProperties swapchainLayer = {
    SWAPCHAIN_SURFACE_NAME,
    VK_LAYER_API_VERSION,
    1,
    "Vulkan Swapchain",
};

struct Surface {
    NativeWindow* window;
    VkSwapchainKHR swapchainHandle;
    uint64_t usage;
};

struct Swapchain {
    Swapchain(Surface &surface, uint32_t numImages, VkPresentModeKHR presentMode,
        OH_NativeBuffer_TransformType preTransform)
        : surface(surface), numImages(numImages), mailboxMode(presentMode == VK_PRESENT_MODE_MAILBOX_KHR),
          preTransform(preTransform),
          shared(presentMode == VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR ||
                 presentMode == VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR) {}

    Surface &surface;
    uint32_t numImages;
    bool mailboxMode;
    OH_NativeBuffer_TransformType preTransform;
    bool shared;

    struct Image {
        Image() : image(VK_NULL_HANDLE), buffer(nullptr), requestFence(-1), releaseFence(-1), requested(false) {}
        VkImage image;
        NativeWindowBuffer* buffer;
        int requestFence;
        int releaseFence;
        bool requested;
    } images[MAX_BUFFER_SIZE];
};

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL GetDeviceProcAddr(VkDevice device, const char* funcName);
VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL GetInstanceProcAddr(VkInstance instance,
                                                             const char* funcName);

VkSurfaceKHR HandleFromSurface(Surface* surface)
{
    return VkSurfaceKHR(reinterpret_cast<uint64_t>(surface));
}

Surface* SurfaceFromHandle(VkSurfaceKHR handle)
{
    return reinterpret_cast<Surface*>(handle);
}

Swapchain* SwapchainFromHandle(VkSwapchainKHR handle)
{
    return reinterpret_cast<Swapchain*>(handle);
}

VkSwapchainKHR HandleFromSwapchain(Swapchain* swapchain)
{
    return VkSwapchainKHR(reinterpret_cast<uint64_t>(swapchain));
}

VKAPI_ATTR void* DefaultAllocate(void*, size_t size, size_t alignment, VkSystemAllocationScope)
{
    void* ptr = nullptr;
    int ret = posix_memalign(&ptr, std::max(alignment, sizeof(void*)), size);
    return ret == 0 ? ptr : nullptr;
}

VKAPI_ATTR void* DefaultReallocate(void*, void* ptr, size_t size, size_t alignment, VkSystemAllocationScope)
{
    if (size == 0) {
        free(ptr);
        return nullptr;
    }

    size_t oldSize = ptr ? malloc_usable_size(ptr) : 0;
    if (size <= oldSize) {
        return ptr;
    }

    void* newPtr = nullptr;
    if (posix_memalign(&newPtr, std::max(alignment, sizeof(void*)), size) != 0) {
        return nullptr;
    }

    if (ptr != nullptr) {
        auto ret = memcpy_s(newPtr, size, ptr, oldSize);
        if (ret != EOK) {
            free(newPtr);
            return nullptr;
        }
        free(ptr);
    }
    return newPtr;
}

VKAPI_ATTR void DefaultFree(void*, void* ptr)
{
    free(ptr);
}

const VkAllocationCallbacks &GetDefaultAllocator()
{
    static const VkAllocationCallbacks defaultAllocCallbacks = {
        .pUserData = nullptr,
        .pfnAllocation = DefaultAllocate,
        .pfnReallocation = DefaultReallocate,
        .pfnFree = DefaultFree,
    };

    return defaultAllocCallbacks;
}

GraphicColorDataSpace GetColorDataspace(VkColorSpaceKHR colorspace)
{
    switch (colorspace) {
        case VK_COLOR_SPACE_SRGB_NONLINEAR_KHR:
            return GRAPHIC_BT709_SRGB_FULL;
        case VK_COLOR_SPACE_DISPLAY_P3_NONLINEAR_EXT:
            return GRAPHIC_DCI_P3_GAMMA26_FULL;
        case VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT:
            return GRAPHIC_BT709_LINEAR_EXTENDED;
        case VK_COLOR_SPACE_EXTENDED_SRGB_NONLINEAR_EXT:
            return GRAPHIC_BT709_SRGB_EXTENDED;
        case VK_COLOR_SPACE_DCI_P3_LINEAR_EXT:
            return GRAPHIC_DCI_P3_LINEAR_FULL;
        case VK_COLOR_SPACE_DCI_P3_NONLINEAR_EXT:
            return GRAPHIC_DCI_P3_GAMMA26_FULL;
        case VK_COLOR_SPACE_BT709_LINEAR_EXT:
            return GRAPHIC_BT709_LINEAR_FULL;
        case VK_COLOR_SPACE_BT709_NONLINEAR_EXT:
            return GRAPHIC_BT709_SRGB_FULL;
        case VK_COLOR_SPACE_BT2020_LINEAR_EXT:
            return GRAPHIC_BT2020_LINEAR_FULL;
        case VK_COLOR_SPACE_HDR10_ST2084_EXT:
            return GRAPHIC_BT2020_ST2084_FULL;
        case VK_COLOR_SPACE_DOLBYVISION_EXT:
            return GRAPHIC_BT2020_ST2084_FULL;
        case VK_COLOR_SPACE_HDR10_HLG_EXT:
            return GRAPHIC_BT2020_HLG_FULL;
        case VK_COLOR_SPACE_ADOBERGB_LINEAR_EXT:
            return static_cast<GraphicColorDataSpace>(GRAPHIC_GAMUT_ADOBE_RGB |
                                                      GRAPHIC_TRANSFORM_FUNC_LINEAR | GRAPHIC_PRECISION_FULL);
        case VK_COLOR_SPACE_ADOBERGB_NONLINEAR_EXT:
            return GRAPHIC_ADOBE_RGB_GAMMA22_FULL;
        default:
            return GRAPHIC_COLOR_DATA_SPACE_UNKNOWN;
    }
}

static bool IsFencePending(int fd)
{
    if (fd < 0) {
        return false;
    }
    errno = 0;
    sptr<OHOS::SyncFence> syncFence = new OHOS::SyncFence(fd);
    return syncFence->Wait(0) == -1 && errno == ETIME;
}

void ReleaseSwapchainImage(VkDevice device, NativeWindow* window, int releaseFence, Swapchain::Image &image,
                           bool deferIfPending)
{
    if (releaseFence != -1 && !image.requested) {
        SWLOGE("%{public}s, can't provide a release fence for non-requested images", __func__);
        DebugMessageToUserCallback(VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
            "can't provide a release fence for non-requested images");
        return;
    }

    VkLayerDispatchTable* pDisp =
        GetLayerDataPtr(GetDispatchKey(device))->deviceDispatchTable.get();
    pDisp->DeviceWaitIdle(device);
    if (image.requested) {
        if (releaseFence >= 0) {
            if (image.requestFence >= 0) {
                sptr<OHOS::SyncFence> requestFence = new OHOS::SyncFence(image.requestFence);
                requestFence->Wait(-1);
                close(image.requestFence);
            }
        } else {
            releaseFence = image.requestFence;
        }
        image.requestFence = -1;

        if (window != nullptr) {
            NativeWindowCancelBuffer(window, image.buffer);
        } else {
            if (releaseFence >= 0) {
                close(releaseFence);
            }
        }
        image.requested = false;
    }

    if (deferIfPending && IsFencePending(image.releaseFence)) {
        return;
    }

    if (image.releaseFence >= 0) {
        close(image.releaseFence);
        image.releaseFence = -1;
    }

    if (image.image != VK_NULL_HANDLE) {
        NativeObjectUnreference(image.buffer);
        image.buffer = nullptr;
        pDisp->DestroyImage(device, image.image, nullptr);
        image.image = VK_NULL_HANDLE;
    }
}

void ReleaseSwapchain(VkDevice device, Swapchain* swapchain)
{
    if (swapchain->surface.swapchainHandle != HandleFromSwapchain(swapchain)) {
        return;
    }

    for (uint32_t i = 0; i < swapchain->numImages; i++) {
        if (!swapchain->images[i].requested) {
            ReleaseSwapchainImage(device, nullptr, -1, swapchain->images[i], true);
        }
    }
    swapchain->surface.swapchainHandle = VK_NULL_HANDLE;
}

GraphicPixelFormat GetPixelFormat(VkFormat format)
{
    switch (format) {
        case VK_FORMAT_R8G8B8A8_UNORM:
        case VK_FORMAT_R8G8B8A8_SRGB:
            return GRAPHIC_PIXEL_FMT_RGBA_8888;
        case VK_FORMAT_R5G6B5_UNORM_PACK16:
            return GRAPHIC_PIXEL_FMT_RGB_565;
        case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
            return GRAPHIC_PIXEL_FMT_RGBA_1010102;
        default:
            SWLOGE("Swapchain format %{public}d unsupported return GRAPHIC_PIXEL_FMT_RGBA_8888;", format);
            DebugMessageToUserCallback(VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
                ("unsupported swapchain format " + std::to_string(format)).c_str());
            return GRAPHIC_PIXEL_FMT_RGBA_8888;
    }
}

/*
    On OpenHarmony, the direction of rotation is counterclockwise
*/

VkSurfaceTransformFlagBitsKHR TranslateNativeToVulkanTransform(OH_NativeBuffer_TransformType nativeTransformType)
{
    switch (nativeTransformType) {
        case NATIVEBUFFER_ROTATE_NONE:
            return VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        case NATIVEBUFFER_ROTATE_90:
            return VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR;
        case NATIVEBUFFER_ROTATE_180:
            return VK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR;
        case NATIVEBUFFER_ROTATE_270:
            return VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR;
        case NATIVEBUFFER_FLIP_H:
        case NATIVEBUFFER_FLIP_V_ROT180:
            return VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_BIT_KHR;
        case NATIVEBUFFER_FLIP_V:
        case NATIVEBUFFER_FLIP_H_ROT180:
            return VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR;
        case NATIVEBUFFER_FLIP_H_ROT90:
        case NATIVEBUFFER_FLIP_V_ROT270:
            return VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR;
        case NATIVEBUFFER_FLIP_V_ROT90:
        case NATIVEBUFFER_FLIP_H_ROT270:
            return VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR;
        default:
            return VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    }
}

OH_NativeBuffer_TransformType TranslateVulkanToNativeTransform(VkSurfaceTransformFlagBitsKHR transform)
{
    switch (transform) {
        case VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR:
            return NATIVEBUFFER_ROTATE_NONE;
        case VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR:
            return NATIVEBUFFER_ROTATE_270;
        case VK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR:
            return NATIVEBUFFER_ROTATE_180;
        case VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR:
            return NATIVEBUFFER_ROTATE_90;
        case VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_BIT_KHR:
            return NATIVEBUFFER_FLIP_H;
        case VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR:
            return NATIVEBUFFER_FLIP_V_ROT90;
        case VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR:
            return NATIVEBUFFER_FLIP_V;
        case VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR:
            return NATIVEBUFFER_FLIP_H_ROT90;
        default:
            return NATIVEBUFFER_ROTATE_NONE;
    }
}

VKAPI_ATTR VkResult SetWindowPixelFormat(NativeWindow* window, GraphicPixelFormat pixelFormat)
{
    if (window == nullptr) {
        return VK_ERROR_SURFACE_LOST_KHR;
    }
    int err = NativeWindowHandleOpt(window, SET_FORMAT, pixelFormat);
    if (err != OHOS::GSERROR_OK) {
        SWLOGE("NativeWindow Set Buffers Format(%{public}d) failed: (%{public}d)", pixelFormat, err);
        return VK_ERROR_SURFACE_LOST_KHR;
    }
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult SetWindowColorDataSpace(NativeWindow* window, GraphicColorDataSpace colorDataSpace)
{
    if (window == nullptr) {
        return VK_ERROR_SURFACE_LOST_KHR;
    }
    SWLOGE("NativeWindow Not Support Set Color dataspace, now. Set GraphicColorDataSpace is [%{public}d]",
        static_cast<int>(colorDataSpace));

    return VK_SUCCESS;
}

VKAPI_ATTR VkResult SetWindowBufferGeometry(NativeWindow* window, int width, int height)
{
    if (window == nullptr) {
        return VK_ERROR_SURFACE_LOST_KHR;
    }
    int err = NativeWindowHandleOpt(window, SET_BUFFER_GEOMETRY, width, height);
    if (err != OHOS::GSERROR_OK) {
        SWLOGE("NativeWindow Set Buffer Geometry width:%{public}d,height:%{public}d failed: %{public}d",
            width, height, err);
        return VK_ERROR_SURFACE_LOST_KHR;
    }

    return VK_SUCCESS;
}

VKAPI_ATTR VkResult SetWindowTransform(NativeWindow* window, OH_NativeBuffer_TransformType transformType)
{
    if (window == nullptr) {
        return VK_ERROR_SURFACE_LOST_KHR;
    }
    int err = NativeWindowHandleOpt(window, SET_TRANSFORM, transformType);
    if (err != OHOS::GSERROR_OK) {
        SWLOGE("NativeWindow Set Transform failed: %{public}d", err);
        return VK_ERROR_SURFACE_LOST_KHR;
    }

    return VK_SUCCESS;
}

VKAPI_ATTR VkResult SetWindowBufferUsage(NativeWindow* window, const VkSwapchainCreateInfoKHR* createInfo)
{
    uint64_t grallocUsage = 0;
    int err = NativeWindowHandleOpt(window, GET_USAGE, &grallocUsage);
    if (err != OHOS::GSERROR_OK) {
        SWLOGE("NativeWindow Get Usage %{public}" PRIu64" failed: %{public}d", grallocUsage, err);
        return VK_ERROR_SURFACE_LOST_KHR;
    }

    if (createInfo->flags & VK_SWAPCHAIN_CREATE_PROTECTED_BIT_KHR) {
        grallocUsage |= BUFFER_USAGE_PROTECTED;
    }
    err = NativeWindowHandleOpt(window, SET_USAGE, grallocUsage);
    if (err != OHOS::GSERROR_OK) {
        SWLOGE("NativeWindow Set Usage %{public}" PRIu64" failed: %{public}d", grallocUsage, err);
        return VK_ERROR_SURFACE_LOST_KHR;
    }
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult SetWindowScalingMode(NativeWindow* window, OHScalingModeV2 scalingMode)
{
    if (window == nullptr) {
        return VK_ERROR_SURFACE_LOST_KHR;
    }
    SWLOGD("NativeWindow Set OHScalingMode is [%{public}d]", static_cast<int>(scalingMode));
    int err = OH_NativeWindow_NativeWindowSetScalingModeV2(window, scalingMode);
    if (err != OHOS::GSERROR_OK) {
        SWLOGE("NativeWindow Set ScalingMode[%{public}d] failed, error: %{public}d",
            static_cast<int>(scalingMode), err);
        return VK_ERROR_SURFACE_LOST_KHR;
    }

    return VK_SUCCESS;
}

VKAPI_ATTR VkResult SetWindowQueueSize(NativeWindow* window, const VkSwapchainCreateInfoKHR* createInfo)
{
    if (window == nullptr) {
        return VK_ERROR_SURFACE_LOST_KHR;
    }
    uint32_t numImages = createInfo->minImageCount;
    if (numImages > MAX_BUFFER_SIZE) {
        SWLOGE("Swapchain init minImageCount[%{public}u] can not be more than maxBufferCount[%{public}u]",
            numImages, MAX_BUFFER_SIZE);
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    if (createInfo->presentMode == VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR ||
        createInfo->presentMode == VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR) {
        numImages = 1;
    }
    SWLOGD("NativeWindow Set Queue Size [%{public}u], Swapchain has the same number of iamge", numImages);
    window->surface->SetQueueSize(numImages);

    return VK_SUCCESS;
}

VKAPI_ATTR VkResult SetWindowInfo(VkDevice device, const VkSwapchainCreateInfoKHR* createInfo)
{
    GraphicColorDataSpace colorDataSpace = GetColorDataspace(createInfo->imageColorSpace);
    if (colorDataSpace == GRAPHIC_COLOR_DATA_SPACE_UNKNOWN) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    GraphicPixelFormat pixelFormat = GetPixelFormat(createInfo->imageFormat);
    Surface &surface = *SurfaceFromHandle(createInfo->surface);

    NativeWindow* window = surface.window;
    // Set PixelFormat
    if (SetWindowPixelFormat(window, pixelFormat) != VK_SUCCESS) {
        return VK_ERROR_SURFACE_LOST_KHR;
    }
    // Set DataSpace
    if (SetWindowColorDataSpace(window, colorDataSpace) != VK_SUCCESS) {
        return VK_ERROR_SURFACE_LOST_KHR;
    }
    // Set BufferGeometry
    if (SetWindowBufferGeometry(window, static_cast<int>(createInfo->imageExtent.width),
        static_cast<int>(createInfo->imageExtent.height)) != VK_SUCCESS) {
        return VK_ERROR_SURFACE_LOST_KHR;
    }
    // Set Transform
    OH_NativeBuffer_TransformType transformType = TranslateVulkanToNativeTransform(createInfo->preTransform);
    if (SetWindowTransform(window, transformType) != VK_SUCCESS) {
        return VK_ERROR_SURFACE_LOST_KHR;
    }

    // Set Scaling mode
    if (SetWindowScalingMode(window, OHScalingModeV2::OH_SCALING_MODE_SCALE_TO_WINDOW_V2) != VK_SUCCESS) {
        return VK_ERROR_SURFACE_LOST_KHR;
    }

    // Set Bufferqueue Size
    if (SetWindowQueueSize(window, createInfo) != VK_SUCCESS) {
        return VK_ERROR_SURFACE_LOST_KHR;
    }

    // Set Buffer Usage
    if (SetWindowBufferUsage(window, createInfo) != VK_SUCCESS) {
        return VK_ERROR_SURFACE_LOST_KHR;
    }

    return VK_SUCCESS;
}

VkResult SetSwapchainCreateInfo(VkDevice device, const VkSwapchainCreateInfoKHR* createInfo)
{
    if (createInfo->oldSwapchain != VK_NULL_HANDLE) {
        ReleaseSwapchain(device, SwapchainFromHandle(createInfo->oldSwapchain));
    }

    return SetWindowInfo(device, createInfo);
}

void InitImageCreateInfo(const VkSwapchainCreateInfoKHR* createInfo, VkImageCreateInfo* imageCreate)
{
    imageCreate->sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    bool swapchainCreateProtected = createInfo->flags & VK_SWAPCHAIN_CREATE_PROTECTED_BIT_KHR;
    imageCreate->flags = swapchainCreateProtected ? VK_IMAGE_CREATE_PROTECTED_BIT : 0u;
    imageCreate->imageType = VK_IMAGE_TYPE_2D;
    imageCreate->format = createInfo->imageFormat;
    imageCreate->extent = {0, 0, 1};
    imageCreate->mipLevels = 1;
    imageCreate->arrayLayers = 1;
    imageCreate->samples = VK_SAMPLE_COUNT_1_BIT;
    imageCreate->tiling = VK_IMAGE_TILING_OPTIMAL;
    imageCreate->usage = createInfo->imageUsage;
    imageCreate->sharingMode = createInfo->imageSharingMode;
    imageCreate->queueFamilyIndexCount = createInfo->queueFamilyIndexCount;
    imageCreate->pQueueFamilyIndices = createInfo->pQueueFamilyIndices;
}

VKAPI_ATTR VkResult CreateImages(uint32_t &numImages, Swapchain* swapchain, const VkSwapchainCreateInfoKHR* createInfo,
    VkImageCreateInfo &imageCreate, VkDevice device)
{
    VkLayerDispatchTable* pDisp =
        GetLayerDataPtr(GetDispatchKey(device))->deviceDispatchTable.get();
    Surface &surface = *SurfaceFromHandle(createInfo->surface);
    NativeWindow* window = surface.window;
    if (createInfo->oldSwapchain != VK_NULL_HANDLE) {
        SWLOGD("Swapchain Recreate ,clean buffer queue");
        window->surface->CleanCache();
    }
    VkResult result = VK_SUCCESS;
    for (uint32_t i = 0; i < numImages; i++) {
        Swapchain::Image &img = swapchain->images[i];
        NativeWindowBuffer* buffer = nullptr;
        int err = NativeWindowRequestBuffer(window, &buffer, &img.requestFence);
        if (err != OHOS::GSERROR_OK) {
            SWLOGE("NativeWindow RequestBuffer[%{public}u] failed: (%{public}d)", i, err);
            result = VK_ERROR_SURFACE_LOST_KHR;
            break;
        }
        img.buffer = buffer;
        img.requested = true;
        imageCreate.extent = VkExtent3D {static_cast<uint32_t>(img.buffer->sfbuffer->GetWidth()),
                                          static_cast<uint32_t>(img.buffer->sfbuffer->GetHeight()), 1};
        ((VkNativeBufferOHOS*)(imageCreate.pNext))->handle =
            reinterpret_cast<struct OHBufferHandle *>(img.buffer->sfbuffer->GetBufferHandle());
        result = pDisp->CreateImage(device, &imageCreate, nullptr, &img.image);
        if (result != VK_SUCCESS) {
            SWLOGD("vkCreateImage failed error: %{public}u", result);
            break;
        }
        NativeObjectReference(buffer);
    }

    SWLOGD("swapchain init shared %{public}d", swapchain->shared);
    for (uint32_t i = 0; i < numImages; i++) {
        Swapchain::Image &img = swapchain->images[i];
        if (img.requested) {
            if (!swapchain->shared) {
                NativeWindowCancelBuffer(window, img.buffer);
                img.requestFence = -1;
                img.requested = false;
            }
        }
    }
    return result;
}

static void DestroySwapchainInternal(VkDevice device, VkSwapchainKHR swapchainHandle,
                                     const VkAllocationCallbacks* allocator)
{
    Swapchain* swapchain = SwapchainFromHandle(swapchainHandle);
    if (swapchain == nullptr) {
        return;
    }

    bool active = swapchain->surface.swapchainHandle == swapchainHandle;
    NativeWindow* window = active ? swapchain->surface.window : nullptr;

    for (uint32_t i = 0; i < swapchain->numImages; i++) {
        ReleaseSwapchainImage(device, window, -1, swapchain->images[i], false);
    }

    if (active) {
        swapchain->surface.swapchainHandle = VK_NULL_HANDLE;
        window->surface->CleanCache();
    }
    if (allocator == nullptr) {
        allocator = &GetDefaultAllocator();
    }
    swapchain->~Swapchain();
    allocator->pfnFree(allocator->pUserData, swapchain);
}

VKAPI_ATTR VkResult VKAPI_CALL CreateSwapchainKHR(VkDevice device, const VkSwapchainCreateInfoKHR* createInfo,
    const VkAllocationCallbacks* allocator, VkSwapchainKHR* swapchainHandle)
{
    Surface &surface = *SurfaceFromHandle(createInfo->surface);
    if (surface.swapchainHandle != createInfo->oldSwapchain) {
        return VK_ERROR_NATIVE_WINDOW_IN_USE_KHR;
    }

    VkResult result = SetSwapchainCreateInfo(device, createInfo);
    if (result != VK_SUCCESS) {
        return result;
    }
    uint32_t numImages = surface.window->surface->GetQueueSize();

    if (allocator == nullptr) {
        allocator = &GetDefaultAllocator();
    }
    void* mem = allocator->pfnAllocation(allocator->pUserData, sizeof(Swapchain), alignof(Swapchain),
        VK_SYSTEM_ALLOCATION_SCOPE_OBJECT);
    if (mem == nullptr) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    Swapchain* swapchain = new (mem) Swapchain(surface, numImages, createInfo->presentMode,
        TranslateVulkanToNativeTransform(createInfo->preTransform));

    VkSwapchainImageCreateInfoOHOS swapchainImageCreate = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_IMAGE_CREATE_INFO_OHOS,
        .pNext = nullptr,
    };
    VkNativeBufferOHOS imageNativeBuffer = {
        .sType = VK_STRUCTURE_TYPE_NATIVE_BUFFER_OHOS,
        .pNext = &swapchainImageCreate,
    };

    VkImageCreateInfo imageCreate = {
        .pNext = &imageNativeBuffer,
    };

    InitImageCreateInfo(createInfo, &imageCreate);
    result = CreateImages(numImages, swapchain, createInfo, imageCreate, device);
    if (result != VK_SUCCESS) {
        DestroySwapchainInternal(device, HandleFromSwapchain(swapchain), allocator);
        return result;
    }

    surface.swapchainHandle = HandleFromSwapchain(swapchain);
    *swapchainHandle = surface.swapchainHandle;
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL DestroySwapchainKHR(
    VkDevice device, VkSwapchainKHR vkSwapchain, const VkAllocationCallbacks* pAllocator)
{
    DestroySwapchainInternal(device, vkSwapchain, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL GetSwapchainImagesKHR(
    VkDevice device, VkSwapchainKHR vkSwapchain, uint32_t* count, VkImage* images)
{
    const Swapchain &swapchain = *SwapchainFromHandle(vkSwapchain);
    if (images == nullptr) {
        *count = swapchain.numImages;
        return VK_SUCCESS;
    }

    VkResult result = VK_SUCCESS;
    uint32_t numImages = swapchain.numImages;
    if (*count < swapchain.numImages) {
        numImages = *count;
        result = VK_INCOMPLETE;
    }
    for (uint32_t i = 0; i < numImages; i++) {
        images[i] = swapchain.images[i].image;
    }
    *count = numImages;
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL AcquireNextImageKHR(VkDevice device, VkSwapchainKHR swapchainHandle,
    uint64_t timeout, VkSemaphore semaphore, VkFence vkFence, uint32_t* imageIndex)
{
    Swapchain &swapchain = *SwapchainFromHandle(swapchainHandle);
    NativeWindow* nativeWindow = swapchain.surface.window;
    VkResult result = VK_SUCCESS;

    if (swapchain.surface.swapchainHandle != swapchainHandle) {
        return VK_ERROR_OUT_OF_DATE_KHR;
    }

    LayerData* deviceLayerData = GetLayerDataPtr(GetDispatchKey(device));
    if (swapchain.shared) {
        *imageIndex = 0;
        return deviceLayerData->deviceDispatchTable->AcquireImageOHOS(device, swapchain.images[*imageIndex].image, -1,
                                                                      semaphore, vkFence);
    }

    NativeWindowBuffer* nativeWindowBuffer = nullptr;
    int fence = -1;
    int32_t ret = NativeWindowRequestBuffer(nativeWindow, &nativeWindowBuffer, &fence);
    if (ret != OHOS::GSERROR_OK) {
        SWLOGE("NativeWindow RequestBuffer failed: (%{public}d)", ret);
        DebugMessageToUserCallback(VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
            ("RequestBuffer failed: " + std::to_string(ret)).c_str());
        return VK_ERROR_SURFACE_LOST_KHR;
    }

    uint32_t index = 0;
    for (; index < swapchain.numImages; index++) {
        if (swapchain.images[index].buffer->sfbuffer == nativeWindowBuffer->sfbuffer) {
            swapchain.images[index].requested = true;
            swapchain.images[index].requestFence = fence;
            break;
        }
    }

    if (index == swapchain.numImages) {
        SWLOGD("NativeWindow RequestBuffer returned unrecognized buffer");
        if (NativeWindowCancelBuffer(nativeWindow, nativeWindowBuffer) != OHOS::GSERROR_OK) {
            SWLOGE("NativeWindowCancelBuffer failed: (%{public}d)", ret);
        }
        return VK_ERROR_OUT_OF_DATE_KHR;
    }
    result = deviceLayerData->deviceDispatchTable->AcquireImageOHOS(device, swapchain.images[index].image, -1,
                                                                    semaphore, vkFence);
    if (result != VK_SUCCESS) {
        if (NativeWindowCancelBuffer(nativeWindow, nativeWindowBuffer) != OHOS::GSERROR_OK) {
            SWLOGE("NativeWindowCancelBuffer failed: (%{public}d)", ret);
        }
        swapchain.images[index].requested = false;
        swapchain.images[index].requestFence = -1;
        return result;
    }

    *imageIndex = index;
    return VK_SUCCESS;
}

VKAPI_ATTR
VkResult GetPhysicalDevicePresentRectanglesKHR(VkPhysicalDevice physicalDevice,
    VkSurfaceKHR surface, uint32_t* pRectCount, VkRect2D* pRects)
{
    if (pRects == nullptr) {
        *pRectCount = 1;
        return VK_SUCCESS;
    }
    bool incomplete = *pRectCount < 1;
    *pRectCount = std::min(*pRectCount, 1u);
    if (incomplete) {
        return VK_INCOMPLETE;
    }

    NativeWindow* window = SurfaceFromHandle(surface)->window;

    int32_t width = 0;
    int32_t height = 0;
    int err = NativeWindowHandleOpt(window, GET_BUFFER_GEOMETRY, &height, &width);
    if (err != OHOS::GSERROR_OK) {
        SWLOGE("NativeWindow get buffer geometry failed: (%{public}d)", err);
    }
    pRects[0].offset.x = 0;
    pRects[0].offset.y = 0;
    pRects[0].extent = VkExtent2D{static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
    return VK_SUCCESS;
}

VKAPI_ATTR
VkResult AcquireNextImage2KHR(VkDevice device, const VkAcquireNextImageInfoKHR* pAcquireInfo, uint32_t* pImageIndex)
{
    return AcquireNextImageKHR(device, pAcquireInfo->swapchain, pAcquireInfo->timeout,
                               pAcquireInfo->semaphore, pAcquireInfo->fence, pImageIndex);
}

const VkPresentRegionKHR* GetPresentRegions(const VkPresentInfoKHR* presentInfo)
{
    const VkPresentRegionsKHR* presentRegions = nullptr;
    const VkPresentRegionsKHR* nextRegions = reinterpret_cast<const VkPresentRegionsKHR*>(presentInfo->pNext);
    while (nextRegions != nullptr) {
        if (nextRegions->sType == VK_STRUCTURE_TYPE_PRESENT_REGIONS_KHR) {
            presentRegions = nextRegions;
        }
        nextRegions = reinterpret_cast<const VkPresentRegionsKHR*>(nextRegions->pNext);
    }

    if (presentRegions == nullptr) {
        return nullptr;
    } else {
        if (presentRegions->swapchainCount != presentInfo->swapchainCount) {
            SWLOGE("vkQueuePresentKHR VkPresentRegions::swapchainCount != VkPresentInfo::swapchainCount");
        }
        return presentRegions->pRegions;
    }
}

VkResult ReleaseImage(VkQueue queue, const VkPresentInfoKHR* presentInfo,
    Swapchain::Image &img, int32_t &fence)
{
    LayerData* deviceLayerData = GetLayerDataPtr(GetDispatchKey(queue));
    VkResult result = deviceLayerData->deviceDispatchTable->QueueSignalReleaseImageOHOS(
        queue, presentInfo->waitSemaphoreCount, presentInfo->pWaitSemaphores, img.image, &fence);
    if (img.releaseFence >= 0) {
        close(img.releaseFence);
        img.releaseFence = -1;
    }
    if (fence >= 0) {
        img.releaseFence = dup(fence);
    }
    return result;
}

struct Region::Rect* GetRegionRect(
    const VkAllocationCallbacks* defaultAllocator, struct Region::Rect** rects, int32_t rectangleCount)
{
    return static_cast<struct Region::Rect*>(
                defaultAllocator->pfnReallocation(
                    defaultAllocator->pUserData, *rects,
                    sizeof(Region::Rect) *rectangleCount,
                    alignof(Region::Rect), VK_SYSTEM_ALLOCATION_SCOPE_COMMAND));
}

void InitRegionRect(const VkRectLayerKHR* layer, struct Region::Rect* rect, int32_t bufferHeight)
{
    rect->x = layer->offset.x;
    // flip rect to adapt to bottom-left coordinate
    rect->y = bufferHeight - layer->extent.height - layer->offset.y;
    rect->w = layer->extent.width;
    rect->h = layer->extent.height;
}

VkResult FlushBuffer(const VkPresentRegionKHR* region, struct Region::Rect* rects,
    Swapchain &swapchain, Swapchain::Image &img, int32_t fence)
{
    const VkAllocationCallbacks* defaultAllocator = &GetDefaultAllocator();
    Region localRegion = {};
    if (memset_s(&localRegion, sizeof(localRegion), 0, sizeof(Region)) != EOK) {
        return VK_ERROR_SURFACE_LOST_KHR;
    }
    NativeWindow* window = swapchain.surface.window;
    // Get buffer width and height for flip damage rect
    int32_t bufferWidth = 0;
    int32_t bufferHeight = 0;
    int err = NativeWindowHandleOpt(window, GET_BUFFER_GEOMETRY, &bufferHeight, &bufferWidth);
    if (err != OHOS::GSERROR_OK) {
        SWLOGE("NativeWindow get buffer geometry failed, error num: %{public}d", err);
        return VK_ERROR_SURFACE_LOST_KHR;
    }
    if (region != nullptr) {
        int32_t rectangleCount = region->rectangleCount;
        if (rectangleCount > 0) {
            struct Region::Rect* tmpRects = GetRegionRect(defaultAllocator, &rects, rectangleCount);
            if (tmpRects != nullptr) {
                rects = tmpRects;
            } else {
                rectangleCount = 0;
            }
        }
        for (int32_t r = 0; r < rectangleCount; ++r) {
            InitRegionRect(&region->pRectangles[r], &rects[r], bufferHeight);
        }

        localRegion.rects = rects;
        localRegion.rectNumber = rectangleCount;
    }
    // the acquire fence will be close by BufferQueue module
    err = NativeWindowFlushBuffer(window, img.buffer, fence, localRegion);
    VkResult scResult = VK_SUCCESS;
    if (err != OHOS::GSERROR_OK) {
        SWLOGE("NativeWindow FlushBuffer failed: (%{public}d)", err);
        scResult = VK_ERROR_SURFACE_LOST_KHR;
    } else {
        if (img.requestFence >= 0) {
            close(img.requestFence);
            img.requestFence = -1;
        }
        img.requested = false;
    }

    if (swapchain.shared && scResult == VK_SUCCESS) {
        NativeWindowBuffer* buffer = nullptr;
        int releaseFence = -1;
        err = NativeWindowRequestBuffer(window, &buffer, &releaseFence);
        if (err != OHOS::GSERROR_OK) {
            scResult = VK_ERROR_SURFACE_LOST_KHR;
        } else if (img.buffer != buffer) {
            scResult = VK_ERROR_SURFACE_LOST_KHR;
        } else {
            img.requestFence = releaseFence;
            img.requested = true;
        }
    }

    return scResult;
}

VKAPI_ATTR VkResult VKAPI_CALL QueuePresentKHR(
    VkQueue queue, const VkPresentInfoKHR* presentInfo)
{
    VkResult ret = VK_SUCCESS;

    const VkPresentRegionKHR* regions = GetPresentRegions(presentInfo);
    const VkAllocationCallbacks* defaultAllocator = &GetDefaultAllocator();
    struct Region::Rect* rects = nullptr;
    LayerData* deviceLayerData = GetLayerDataPtr(GetDispatchKey(queue));
    VkDevice device = deviceLayerData->device;

    for (uint32_t i = 0; i < presentInfo->swapchainCount; i++) {
        Swapchain &swapchain = *(reinterpret_cast<Swapchain*>(presentInfo->pSwapchains[i]));
        Swapchain::Image &img = swapchain.images[presentInfo->pImageIndices[i]];
        const VkPresentRegionKHR* region = (regions != nullptr) ? &regions[i] : nullptr;
        int32_t fence = -1;
        ret = ReleaseImage(queue, presentInfo, img, fence);
        if (swapchain.surface.swapchainHandle == presentInfo->pSwapchains[i]) {
            if (ret == VK_SUCCESS) {
                ret = FlushBuffer(region, rects, swapchain, img, fence);
            } else {
                ReleaseSwapchain(device, &swapchain);
            }
        } else {
            SWLOGE("vkQueuePresentKHR swapchainHandle != pSwapchains[%{public}d]", i);
            ReleaseSwapchainImage(device, nullptr, fence, img, true);
            ret = VK_ERROR_OUT_OF_DATE_KHR;
        }

        if (presentInfo->pResults) {
            presentInfo->pResults[i] = ret;
        }
    }
    if (rects != nullptr) {
        defaultAllocator->pfnFree(defaultAllocator->pUserData, rects);
    }
#if USE_APS_IGAMESERVICE_FUNC
    OHOS::GameService::VulkanSliceReport::GetInstance().ReportVulkanRender();
#endif
    return ret;
}

VKAPI_ATTR VkResult GetDeviceGroupPresentCapabilitiesKHR(
    VkDevice, VkDeviceGroupPresentCapabilitiesKHR* pDeviceGroupPresentCapabilities)
{
    if (pDeviceGroupPresentCapabilities == nullptr) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    memset_s(pDeviceGroupPresentCapabilities->presentMask,
        sizeof(pDeviceGroupPresentCapabilities->presentMask), 0, sizeof(pDeviceGroupPresentCapabilities->presentMask));
    pDeviceGroupPresentCapabilities->presentMask[0] = 1u;
    pDeviceGroupPresentCapabilities->modes = VK_DEVICE_GROUP_PRESENT_MODE_LOCAL_BIT_KHR;

    return VK_SUCCESS;
}

VKAPI_ATTR VkResult GetDeviceGroupSurfacePresentModesKHR(
    VkDevice, VkSurfaceKHR, VkDeviceGroupPresentModeFlagsKHR* pModes)
{
    *pModes = VK_DEVICE_GROUP_PRESENT_MODE_LOCAL_BIT_KHR;
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceSurfaceSupportKHR(
    VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, const VkSurfaceKHR surface, VkBool32* pSupported)
{
    *pSupported = VK_TRUE;
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL CreateSurfaceOHOS(VkInstance instance,
    const VkSurfaceCreateInfoOHOS* pCreateInfo,
    const VkAllocationCallbacks* allocator, VkSurfaceKHR* outSurface)
{
    if (allocator == nullptr) {
        allocator = &GetDefaultAllocator();
    }
    void* mem = allocator->pfnAllocation(allocator->pUserData, sizeof(Surface), alignof(Surface),
                                         VK_SYSTEM_ALLOCATION_SCOPE_OBJECT);
    if (mem == nullptr) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    Surface* surface = new (mem) Surface;
    surface->window = pCreateInfo->window;
    NativeObjectReference(surface->window);
    surface->swapchainHandle = VK_NULL_HANDLE;
    int err = NativeWindowHandleOpt(pCreateInfo->window, GET_USAGE, &(surface->usage));
    if (err != OHOS::GSERROR_OK) {
        NativeObjectUnreference(surface->window);
        surface->~Surface();
        allocator->pfnFree(allocator->pUserData, surface);
        SWLOGE("NativeWindow get usage failed, error num : %{public}d", err);
        return VK_ERROR_SURFACE_LOST_KHR;
    }

    *outSurface = HandleFromSurface(surface);
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL DestroySurfaceKHR(
    VkInstance instance, VkSurfaceKHR vkSurface, const VkAllocationCallbacks* pAllocator)
{
    Surface* surface = SurfaceFromHandle(vkSurface);
    if (surface == nullptr) {
        return;
    }
    if (pAllocator == nullptr) {
        pAllocator = &GetDefaultAllocator();
    }
    NativeObjectUnreference(surface->window);
    surface->~Surface();
    pAllocator->pfnFree(pAllocator->pUserData, surface);
}

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceSurfaceCapabilitiesKHR(
    VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkSurfaceCapabilitiesKHR* capabilities)
{
    int32_t width = 0;
    int32_t height = 0;
    OH_NativeBuffer_TransformType transformHint = NATIVEBUFFER_ROTATE_NONE;
    uint32_t defaultQueueSize = MAX_BUFFER_SIZE;
    if (surface != VK_NULL_HANDLE) {
        NativeWindow* window = SurfaceFromHandle(surface)->window;
        int err = NativeWindowHandleOpt(window, GET_BUFFER_GEOMETRY, &height, &width);
        if (err != OHOS::GSERROR_OK) {
            SWLOGE("NativeWindow get buffer geometry failed, error num : %{public}d", err);
            return VK_ERROR_SURFACE_LOST_KHR;
        }
        err = NativeWindowGetTransformHint(window, &transformHint);
        if (err != OHOS::GSERROR_OK) {
            SWLOGE("NativeWindow get TransformHint failed, error num : %{public}d", err);
            return VK_ERROR_SURFACE_LOST_KHR;
        }
        defaultQueueSize = window->surface->GetQueueSize();
        SWLOGD("NativeWindow default Queue Size : (%{public}d)", defaultQueueSize);
    }

    capabilities->minImageCount = std::min(defaultQueueSize, MIN_BUFFER_SIZE);
    capabilities->maxImageCount = std::max(defaultQueueSize, MAX_BUFFER_SIZE);
    capabilities->currentExtent = VkExtent2D {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
    capabilities->minImageExtent = VkExtent2D {1, 1};
    capabilities->maxImageExtent = VkExtent2D {4096, 4096};
    capabilities->maxImageArrayLayers = 1;
    capabilities->supportedTransforms = g_supportedTransforms;
    capabilities->currentTransform = TranslateNativeToVulkanTransform(transformHint);
    capabilities->supportedCompositeAlpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
    capabilities->supportedUsageFlags = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                        VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT |
                                        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
    return VK_SUCCESS;
}


/*
    VK_EXT_hdr_metadata
*/
VKAPI_ATTR void VKAPI_CALL SetHdrMetadataEXT(
    VkDevice device, uint32_t swapchainCount, const VkSwapchainKHR* pSwapchains, const VkHdrMetadataEXT* pMetadata)
{
    SWLOGE("NativeWindow Not Support Set HdrMetaData[TODO]");
}

/*
    VK_EXT_swapchain_maintenance1
*/

VKAPI_ATTR VkResult VKAPI_CALL ReleaseSwapchainImagesEXT(
    VkDevice device, const VkReleaseSwapchainImagesInfoEXT* pReleaseInfo)
{
    Swapchain& swapchain = *SwapchainFromHandle(pReleaseInfo->swapchain);
    if (swapchain.shared) {
        return VK_SUCCESS;
    }
    NativeWindow* window = swapchain.surface.window;

    for (uint32_t i = 0; i < pReleaseInfo->imageIndexCount; i++) {
        Swapchain::Image& img = swapchain.images[pReleaseInfo->pImageIndices[i]];
        NativeWindowCancelBuffer(window, img.buffer);
        img.requestFence = -1;
        img.requested = false;

        if (img.releaseFence >= 0) {
           close(img.releaseFence);
           img.releaseFence = -1;
        }
    }

    return VK_SUCCESS;
}



VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceSurfaceCapabilities2KHR(
    VkPhysicalDevice physicalDevice, const VkPhysicalDeviceSurfaceInfo2KHR* pSurfaceInfo,
    VkSurfaceCapabilities2KHR* pSurfaceCapabilities)
{
    VkResult result = GetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, pSurfaceInfo->surface,
                                                              &pSurfaceCapabilities->surfaceCapabilities);

    VkSurfaceCapabilities2KHR* caps = pSurfaceCapabilities;
    while(caps->pNext != nullptr) {
        caps = reinterpret_cast<VkSurfaceCapabilities2KHR*>(caps->pNext);
        if (caps->sType == VK_STRUCTURE_TYPE_SHARED_PRESENT_SURFACE_CAPABILITIES_KHR) {
            reinterpret_cast<VkSharedPresentSurfaceCapabilitiesKHR*>(caps)->sharedPresentSupportedUsageFlags = 
                pSurfaceCapabilities->surfaceCapabilities.supportedUsageFlags;
        } else if (caps->sType == VK_STRUCTURE_TYPE_SURFACE_PROTECTED_CAPABILITIES_KHR) {
            reinterpret_cast<VkSurfaceProtectedCapabilitiesKHR*>(caps)->supportsProtected= VK_TRUE;
        }
    }
    return result;
}


VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceSurfaceFormatsKHR(
    VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface, uint32_t* count, VkSurfaceFormatKHR* formats)
{
    if (surface == VK_NULL_HANDLE) {
        return VK_ERROR_SURFACE_LOST_KHR;
    }

    LayerData* deviceLayerData = GetLayerDataPtr(GetDispatchKey(physicalDevice));
    bool enableSwapchainColorSpace = deviceLayerData->enabledExtensions.test(Extension::EXT_SWAPCHAIN_COLOR_SPACE);

    std::vector<VkSurfaceFormatKHR> allFormats = {
        // RGBA_8888
        {VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
        {VK_FORMAT_R8G8B8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
        // RGB_565
        {VK_FORMAT_R5G6B5_UNORM_PACK16, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
        // RGBA_1010102
        {VK_FORMAT_A2B10G10R10_UNORM_PACK32, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
        {VK_FORMAT_A2B10G10R10_UNORM_PACK32, VK_COLOR_SPACE_DISPLAY_P3_NONLINEAR_EXT}
    };

    if (enableSwapchainColorSpace) {
        allFormats.emplace_back(VkSurfaceFormatKHR{
            VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_BT709_LINEAR_EXT});
        allFormats.emplace_back(VkSurfaceFormatKHR{
            VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_DISPLAY_P3_NONLINEAR_EXT});
        allFormats.emplace_back(VkSurfaceFormatKHR{
            VK_FORMAT_R8G8B8A8_SRGB, VK_COLOR_SPACE_DISPLAY_P3_NONLINEAR_EXT});
    }
    VkResult result = VK_SUCCESS;
    if (formats != nullptr) {
        uint32_t transferCount = allFormats.size();
        if (transferCount > *count) {
            transferCount = *count;
            result = VK_INCOMPLETE;
        }
        std::copy(allFormats.begin(), allFormats.begin() + transferCount, formats);
        *count = transferCount;
    } else {
        *count = allFormats.size();
    }

    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceSurfaceFormats2KHR(
    VkPhysicalDevice physicalDevice, const VkPhysicalDeviceSurfaceInfo2KHR* pSurfaceInfo,
    uint32_t* pSurfaceFormatCount, VkSurfaceFormat2KHR* pSurfaceFormats)
{
    // Get pSurfaceFormatCount
    if (pSurfaceFormats == nullptr) {
        return GetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, pSurfaceInfo->surface, pSurfaceFormatCount, nullptr);
    }
    // Get pSurfaceFormats
    uint32_t formatCount = *pSurfaceFormatCount;

    std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
    VkResult res = GetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, pSurfaceInfo->surface, pSurfaceFormatCount,
        surfaceFormats.data());

    if (res == VK_SUCCESS || res == VK_INCOMPLETE) {
        for (uint32_t i = 0; i < formatCount; i++) {
            pSurfaceFormats[i].surfaceFormat = surfaceFormats[i];
        }
    }

    return res;
}

void QueryPresentationProperties(
    VkPhysicalDevice physicalDevice,
    VkPhysicalDevicePresentationPropertiesOHOS* presentationProperties)
{
    VkPhysicalDeviceProperties2 properties = {
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
        presentationProperties,
        {},
    };

    presentationProperties->sType =
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENTATION_PROPERTIES_OHOS;
    presentationProperties->pNext = nullptr;
    presentationProperties->sharedImage = VK_FALSE;

    DispatchKey key = GetDispatchKey(physicalDevice);
    LayerData* curLayerData = GetLayerDataPtr(key);
    if (curLayerData->instanceDispatchTable->GetPhysicalDeviceProperties2) {
        curLayerData->instanceDispatchTable->GetPhysicalDeviceProperties2(physicalDevice, &properties);
    } else if (curLayerData->instanceDispatchTable->GetPhysicalDeviceProperties2KHR) {
        curLayerData->instanceDispatchTable->GetPhysicalDeviceProperties2KHR(physicalDevice, &properties);
    } else {
        SWLOGE("Func vkGetPhysicalDeviceProperties2 and vkGetPhysicalDeviceProperties2KHR are both null.");
    }
}

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceSurfacePresentModesKHR(
    VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface, uint32_t* count, VkPresentModeKHR* pPresentModes)
{
    std::vector<VkPresentModeKHR> presentModes = {
        VK_PRESENT_MODE_MAILBOX_KHR,
        VK_PRESENT_MODE_FIFO_KHR
    };

    VkPhysicalDevicePresentationPropertiesOHOS presentProperties = {};
    QueryPresentationProperties(physicalDevice, &presentProperties);
    if (presentProperties.sharedImage) {
        presentModes.push_back(VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR);
        presentModes.push_back(VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR);
    }

    uint32_t numModes = static_cast<uint32_t>(presentModes.size());
    VkResult result = VK_SUCCESS;
    if (pPresentModes != nullptr) {
        if (*count < numModes) {
            result = VK_INCOMPLETE;
        }
        *count = std::min(*count, numModes);
        std::copy_n(presentModes.data(), *count, pPresentModes);
    } else {
        *count = numModes;
    }
    return result;
}

Extension GetExtensionBitFromName(const char* name)
{
    if (name == nullptr) {
        return Extension::EXTENSION_UNKNOWN;
    }
    if (strcmp(name, VK_OHOS_SURFACE_EXTENSION_NAME) == 0) {
        return Extension::OHOS_SURFACE;
    }
    if (strcmp(name, VK_OHOS_NATIVE_BUFFER_EXTENSION_NAME) == 0) {
        return Extension::OHOS_NATIVE_BUFFER;
    }
    if (strcmp(name, VK_KHR_SURFACE_EXTENSION_NAME) == 0) {
        return Extension::KHR_SURFACE;
    }
    if (strcmp(name, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0) {
        return Extension::KHR_SWAPCHAIN;
    }
    if (strcmp(name, VK_EXT_SWAPCHAIN_COLOR_SPACE_EXTENSION_NAME) == 0) {
        return Extension::EXT_SWAPCHAIN_COLOR_SPACE;
    }
    if (strcmp(name, VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME) == 0) {
        return Extension::KHR_GET_SURFACE_CAPABILITIES_2;
    }
    if (strcmp(name, VK_EXT_HDR_METADATA_EXTENSION_NAME) == 0) {
        return Extension::EXT_HDR_METADATA;
    }
    if (strcmp(name, VK_EXT_SWAPCHAIN_MAINTENANCE_1_EXTENSION_NAME) == 0) {
        return Extension::EXT_SWAPCHAIN_MAINTENANCE_1;
    }
    return Extension::EXTENSION_UNKNOWN;
}

VKAPI_ATTR VkResult VKAPI_CALL CreateInstance(
    const VkInstanceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkInstance* pInstance)
{
    VkLayerInstanceCreateInfo* chainInfo = GetChainInfo(pCreateInfo, VK_LAYER_LINK_INFO);

    if (chainInfo == nullptr || chainInfo->u.pLayerInfo == nullptr) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    PFN_vkGetInstanceProcAddr fpGetInstanceProcAddr =
        chainInfo->u.pLayerInfo->pfnNextGetInstanceProcAddr;
    PFN_vkCreateInstance fpCreateInstance =
        (PFN_vkCreateInstance)fpGetInstanceProcAddr(nullptr, "vkCreateInstance");
    if (fpCreateInstance == nullptr) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    chainInfo->u.pLayerInfo = chainInfo->u.pLayerInfo->pNext;

    VkResult result = fpCreateInstance(pCreateInfo, pAllocator, pInstance);
    if (result != VK_SUCCESS) {
        return result;
    }
#if USE_APS_IGAMESERVICE_FUNC
    OHOS::GameService::VulkanSliceReport::GetInstance().InitVulkanReport();
#endif

    LayerData* instanceLayerData = GetLayerDataPtr(GetDispatchKey(*pInstance));
    instanceLayerData->instance = *pInstance;
    instanceLayerData->instanceDispatchTable = std::make_unique<VkLayerInstanceDispatchTable>();
    layer_init_instance_dispatch_table(*pInstance, instanceLayerData->instanceDispatchTable.get(),
                                       fpGetInstanceProcAddr);
    for (uint32_t index = 0; index < pCreateInfo->enabledExtensionCount; index++) {
        auto extBit = GetExtensionBitFromName(pCreateInfo->ppEnabledExtensionNames[index]);
        if (extBit != Extension::EXTENSION_UNKNOWN) {
            instanceLayerData->enabledExtensions.set(extBit);
        }
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyInstance(
    VkInstance instance, const VkAllocationCallbacks* pAllocator)
{
    DispatchKey instanceKey = GetDispatchKey(instance);
    LayerData* curLayerData = GetLayerDataPtr(instanceKey);
    curLayerData->instanceDispatchTable->DestroyInstance(instance, pAllocator);
    FreeLayerDataPtr(instanceKey);
}

bool CheckExtensionAvailable(const std::string &extensionName,
                             const std::vector<VkExtensionProperties> &deviceExtensions)
{
    bool extensionAvailable = false;
    for (uint32_t i = 0; i < deviceExtensions.size(); i++) {
        if (strcmp(extensionName.data(), deviceExtensions[i].extensionName) == 0) {
            extensionAvailable = true;
            break;
        }
    }
    return extensionAvailable;
}

VkResult AddDeviceExtensions(VkPhysicalDevice gpu, const LayerData* gpuLayerData,
                             std::vector<const char*> &enabledExtensions)
{
    VkResult result = VK_SUCCESS;
    uint32_t deviceExtensionCount = 0;
    result = gpuLayerData->instanceDispatchTable->EnumerateDeviceExtensionProperties(
        gpu, nullptr, &deviceExtensionCount, nullptr);
    if (result == VK_SUCCESS && deviceExtensionCount > 0) {
        std::vector<VkExtensionProperties> deviceExtensions(deviceExtensionCount);
        result = gpuLayerData->instanceDispatchTable->EnumerateDeviceExtensionProperties(
            gpu, nullptr, &deviceExtensionCount, deviceExtensions.data());
        if (result == VK_SUCCESS) {
            if (!CheckExtensionAvailable(VK_OHOS_NATIVE_BUFFER_EXTENSION_NAME, deviceExtensions)) {
                return VK_ERROR_INITIALIZATION_FAILED;
            }
            enabledExtensions.push_back(VK_OHOS_NATIVE_BUFFER_EXTENSION_NAME);
            if (CheckExtensionAvailable(VK_OHOS_EXTERNAL_MEMORY_EXTENSION_NAME,
                                        deviceExtensions)) {
                enabledExtensions.push_back(VK_OHOS_EXTERNAL_MEMORY_EXTENSION_NAME);
            }
        }
    }
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL CreateDevice(VkPhysicalDevice gpu,
    const VkDeviceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDevice* pDevice)
{
    DispatchKey gpuKey = GetDispatchKey(gpu);
    LayerData* gpuLayerData = GetLayerDataPtr(gpuKey);

    VkDeviceCreateInfo createInfo = *pCreateInfo;
    std::vector<const char*> enabledExtensions = {};
    for (uint32_t i = 0; i < createInfo.enabledExtensionCount; i++) {
        enabledExtensions.push_back(createInfo.ppEnabledExtensionNames[i]);
    }

    VkResult result = AddDeviceExtensions(gpu, gpuLayerData, enabledExtensions);
    if (result != VK_SUCCESS) {
        return result;
    }

    createInfo.enabledExtensionCount = ToUint32(enabledExtensions.size());
    createInfo.ppEnabledExtensionNames = enabledExtensions.data();

    VkLayerDeviceCreateInfo* linkInfo = GetChainInfo(pCreateInfo, VK_LAYER_LINK_INFO);

    if (linkInfo == nullptr || linkInfo->u.pLayerInfo == nullptr) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    PFN_vkGetInstanceProcAddr fpGetInstanceProcAddr = linkInfo->u.pLayerInfo->pfnNextGetInstanceProcAddr;
    PFN_vkGetDeviceProcAddr fpGetDeviceProcAddr = linkInfo->u.pLayerInfo->pfnNextGetDeviceProcAddr;
    PFN_vkCreateDevice fpCreateDevice =
        (PFN_vkCreateDevice)fpGetInstanceProcAddr(gpuLayerData->instance, "vkCreateDevice");
    if (fpCreateDevice == nullptr) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    linkInfo->u.pLayerInfo = linkInfo->u.pLayerInfo->pNext;

    result = fpCreateDevice(gpu, &createInfo, pAllocator, pDevice);
    if (result != VK_SUCCESS) {
        return result;
    }

    LayerData* deviceLayerData = GetLayerDataPtr(GetDispatchKey(*pDevice));
    for (uint32_t i = 0; i < createInfo.enabledExtensionCount; i++) {
        auto extBit = GetExtensionBitFromName(createInfo.ppEnabledExtensionNames[i]);
        if (extBit != Extension::EXTENSION_UNKNOWN) {
            deviceLayerData->enabledExtensions.set(extBit);
        }
    }

    deviceLayerData->deviceDispatchTable = std::make_unique<VkLayerDispatchTable>();
    deviceLayerData->instance = gpuLayerData->instance;
    deviceLayerData->device = *pDevice;
    layer_init_device_dispatch_table(*pDevice, deviceLayerData->deviceDispatchTable.get(), fpGetDeviceProcAddr);

    VkLayerDeviceCreateInfo* callbackInfo = GetChainInfo(pCreateInfo, VK_LOADER_DATA_CALLBACK);
    if (callbackInfo == nullptr || callbackInfo->u.pfnSetDeviceLoaderData == nullptr) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    deviceLayerData->fpSetDeviceLoaderData = callbackInfo->u.pfnSetDeviceLoaderData;

    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL DestroyDevice(VkDevice device, const VkAllocationCallbacks* pAllocator)
{
    DispatchKey deviceKey = GetDispatchKey(device);
    LayerData* deviceData = GetLayerDataPtr(deviceKey);
    deviceData->deviceDispatchTable->DestroyDevice(device, pAllocator);
    FreeLayerDataPtr(deviceKey);
}

VKAPI_ATTR VkResult VKAPI_CALL CreateDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pMessenger)
{
    DispatchKey instanceKey = GetDispatchKey(instance);
    LayerData* curLayerData = GetLayerDataPtr(instanceKey);
    VkResult res = curLayerData->instanceDispatchTable->CreateDebugUtilsMessengerEXT(
        instance, pCreateInfo, pAllocator, pMessenger);
    if (res == VK_SUCCESS) {
        curLayerData->debugCallbacks[*pMessenger] = *pCreateInfo;
    }
    return res;
}

VKAPI_ATTR void VKAPI_CALL DestroyDebugUtilsMessengerEXT(
    VkInstance instance, VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks* pAllocator)
{
    DispatchKey instanceKey = GetDispatchKey(instance);
    LayerData* curLayerData = GetLayerDataPtr(instanceKey);
    curLayerData->debugCallbacks.erase(messenger);
    curLayerData->instanceDispatchTable->DestroyDebugUtilsMessengerEXT(instance, messenger, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL EnumerateInstanceExtensionProperties(
    const char* pLayerName, uint32_t* pCount, VkExtensionProperties* pProperties)
{
    if ((pLayerName != nullptr) && (strcmp(pLayerName, swapchainLayer.layerName) == 0)) {
        return GetExtensionProperties(std::size(g_instanceExtensions), g_instanceExtensions, pCount, pProperties);
    }
    return VK_ERROR_LAYER_NOT_PRESENT;
}

// Vulkan Loader will read json and call this func
VKAPI_ATTR VkResult VKAPI_CALL EnumerateInstanceLayerProperties(uint32_t* pCount, VkLayerProperties* pProperties)
{
    return GetLayerProperties(1, &swapchainLayer, pCount, pProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL EnumerateDeviceLayerProperties(
    VkPhysicalDevice physicalDevice, uint32_t* pCount, VkLayerProperties* pProperties)
{
    return GetLayerProperties(1, &swapchainLayer, pCount, pProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL EnumerateDeviceExtensionProperties(
    VkPhysicalDevice physicalDevice, const char* pLayerName, uint32_t* pCount, VkExtensionProperties* pProperties)
{
    if ((pLayerName != nullptr) && (strcmp(pLayerName, swapchainLayer.layerName) == 0)) {
        return GetExtensionProperties(
            std::size(g_deviceExtensions), g_deviceExtensions, pCount, pProperties);
    }

    if (physicalDevice == nullptr) {
        SWLOGE("vkEnumerateDeviceExtensionProperties physicalDevice is null.");
        return VK_ERROR_LAYER_NOT_PRESENT;
    }

    DispatchKey key = GetDispatchKey(physicalDevice);
    LayerData* curLayerData = GetLayerDataPtr(key);
    return curLayerData->instanceDispatchTable->EnumerateDeviceExtensionProperties(physicalDevice, nullptr,
                                                                                   pCount, pProperties);
}

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL GetPhysicalDeviceProcAddr(VkInstance instance, const char* funcName)
{
    if (instance == VK_NULL_HANDLE) {
        SWLOGE("vkGetPhysicalDeviceProcAddr instance is null.");
        return nullptr;
    }

    LayerData* layerData = GetLayerDataPtr(GetDispatchKey(instance));
    VkLayerInstanceDispatchTable* pTable = layerData->instanceDispatchTable.get();

    if (pTable->GetPhysicalDeviceProcAddr == nullptr) {
        return nullptr;
    }
    return pTable->GetPhysicalDeviceProcAddr(instance, funcName);
}

static inline PFN_vkVoidFunction GetDebugUtilsProc(const char* name)
{
    if (name == nullptr) {
        return nullptr;
    }
    if (strcmp(name, "vkCreateDebugUtilsMessengerEXT") == 0) {
        return reinterpret_cast<PFN_vkVoidFunction>(CreateDebugUtilsMessengerEXT);
    }
    if (strcmp(name, "vkDestroyDebugUtilsMessengerEXT") == 0) {
        return reinterpret_cast<PFN_vkVoidFunction>(DestroyDebugUtilsMessengerEXT);
    }
    return nullptr;
}

static inline PFN_vkVoidFunction GetGlobalProc(const char* name)
{
    if (name == nullptr) {
        return nullptr;
    }
    if (strcmp("vkEnumerateDeviceLayerProperties", name) == 0) {
        return reinterpret_cast<PFN_vkVoidFunction>(EnumerateDeviceLayerProperties);
    }
    return nullptr;
}

static inline PFN_vkVoidFunction GetSwapchainProc(const char* name)
{
    if (name == nullptr) {
        return nullptr;
    }
    if (strcmp("vkCreateSwapchainKHR", name) == 0) {
        return reinterpret_cast<PFN_vkVoidFunction>(CreateSwapchainKHR);
    }
    if (strcmp("vkDestroySwapchainKHR", name) == 0) {
        return reinterpret_cast<PFN_vkVoidFunction>(DestroySwapchainKHR);
    }
    if (strcmp("vkAcquireNextImageKHR", name) == 0) {
        return reinterpret_cast<PFN_vkVoidFunction>(AcquireNextImageKHR);
    }
    if (strcmp("vkAcquireNextImage2KHR", name) == 0) {
        return reinterpret_cast<PFN_vkVoidFunction>(AcquireNextImage2KHR);
    }
    if (strcmp("vkQueuePresentKHR", name) == 0) {
        return reinterpret_cast<PFN_vkVoidFunction>(QueuePresentKHR);
    }
    if (strcmp("vkGetSwapchainImagesKHR", name) == 0) {
        return reinterpret_cast<PFN_vkVoidFunction>(GetSwapchainImagesKHR);
    }
    if (strcmp("vkGetDeviceGroupPresentCapabilitiesKHR", name) == 0) {
        return reinterpret_cast<PFN_vkVoidFunction>(GetDeviceGroupPresentCapabilitiesKHR);
    }
    if (strcmp("vkGetDeviceGroupSurfacePresentModesKHR", name) == 0) {
        return reinterpret_cast<PFN_vkVoidFunction>(GetDeviceGroupSurfacePresentModesKHR);
    }
    if (strcmp("vkGetPhysicalDevicePresentRectanglesKHR", name) == 0) {
        return reinterpret_cast<PFN_vkVoidFunction>(GetPhysicalDevicePresentRectanglesKHR);
    }
    return nullptr;
}

static inline PFN_vkVoidFunction LayerInterceptDeviceProc(
    std::bitset<Extension::EXTENSION_COUNT>& enabledExtensions, const char* name)
{
    if (name == nullptr) {
        return nullptr;
    }
    if (strcmp("vkGetDeviceProcAddr", name) == 0) {
        return reinterpret_cast<PFN_vkVoidFunction>(GetDeviceProcAddr);
    }
    if (strcmp("vkDestroyDevice", name) == 0) {
        return reinterpret_cast<PFN_vkVoidFunction>(DestroyDevice);
    }
    if (enabledExtensions.test(Extension::KHR_SWAPCHAIN)) {
        PFN_vkVoidFunction addr = GetSwapchainProc(name);
        if (addr != nullptr) {
            return addr;
        }
        // Extension EXT_SWAPCHAIN_MAINTENANCE_1 depends on KHR_SWAPCHAIN
        if (enabledExtensions.test(Extension::EXT_SWAPCHAIN_MAINTENANCE_1)) {
            if (strcmp("vkReleaseSwapchainImagesEXT", name) == 0) {
                return reinterpret_cast<PFN_vkVoidFunction>(ReleaseSwapchainImagesEXT);
            }
        }
    }
    if (enabledExtensions.test(Extension::EXT_HDR_METADATA)) {
        if (strcmp("vkSetHdrMetadataEXT", name) == 0) {
            return reinterpret_cast<PFN_vkVoidFunction>(SetHdrMetadataEXT);
        }
    }
    return nullptr;
}

static inline PFN_vkVoidFunction GetSurfaceKHRProc(const char* name)
{
    if (name == nullptr) {
        return nullptr;
    }
    if (strcmp("vkDestroySurfaceKHR", name) == 0) {
        return reinterpret_cast<PFN_vkVoidFunction>(DestroySurfaceKHR);
    }
    if (strcmp("vkGetPhysicalDeviceSurfacePresentModesKHR", name) == 0) {
        return reinterpret_cast<PFN_vkVoidFunction>(GetPhysicalDeviceSurfacePresentModesKHR);
    }
    if (strcmp("vkGetPhysicalDeviceSurfaceSupportKHR", name) == 0) {
        return reinterpret_cast<PFN_vkVoidFunction>(GetPhysicalDeviceSurfaceSupportKHR);
    }
    if (strcmp("vkGetPhysicalDeviceSurfaceCapabilitiesKHR", name) == 0) {
        return reinterpret_cast<PFN_vkVoidFunction>(GetPhysicalDeviceSurfaceCapabilitiesKHR);
    }
    if (strcmp("vkGetPhysicalDeviceSurfaceFormatsKHR", name) == 0) {
        return reinterpret_cast<PFN_vkVoidFunction>(GetPhysicalDeviceSurfaceFormatsKHR);
    }
    return nullptr;
}

static inline PFN_vkVoidFunction GetSurfaceCapabilities2Proc(const char* name)
{
    if (name == nullptr) {
        return nullptr;
    }
    if (strcmp("vkGetPhysicalDeviceSurfaceCapabilities2KHR", name) == 0) {
        return reinterpret_cast<PFN_vkVoidFunction>(GetPhysicalDeviceSurfaceCapabilities2KHR);
    }
    if (strcmp("vkGetPhysicalDeviceSurfaceFormats2KHR", name) == 0) {
        return reinterpret_cast<PFN_vkVoidFunction>(GetPhysicalDeviceSurfaceFormats2KHR);
    }
    return nullptr;
}

static inline PFN_vkVoidFunction LayerInterceptInstanceProc(
    std::bitset<Extension::EXTENSION_COUNT>& enabledExtensions, const char* name)
{
    if (name == nullptr) {
        return nullptr;
    }
    if (enabledExtensions.test(Extension::OHOS_SURFACE)) {
        if (strcmp("vkCreateSurfaceOHOS", name) == 0) {
            return reinterpret_cast<PFN_vkVoidFunction>(CreateSurfaceOHOS);
        }
    }
    if (enabledExtensions.test(Extension::KHR_SURFACE)) {
        PFN_vkVoidFunction addr = GetSurfaceKHRProc(name);
        if (addr != nullptr) {
            return addr;
        }
        if (enabledExtensions.test(Extension::KHR_GET_SURFACE_CAPABILITIES_2)) {
            PFN_vkVoidFunction addr = GetSurfaceCapabilities2Proc(name);
            if (addr != nullptr) {
                return addr;
            }
        }
    }

    if (enabledExtensions.test(Extension::KHR_SWAPCHAIN)) {
        PFN_vkVoidFunction addr = GetSwapchainProc(name);
        if (addr != nullptr) {
            return addr;
        }
    }

    PFN_vkVoidFunction addr = GetDebugUtilsProc(name);
    if (addr != nullptr) {
        return addr;
    }
    return GetGlobalProc(name);
}

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL GetDeviceProcAddr(VkDevice device, const char* funcName)
{
    if (funcName == nullptr) {
        return nullptr;
    }
    if (device == VK_NULL_HANDLE) {
        SWLOGE("vkGetDeviceProcAddr device is null.");
        return nullptr;
    }
    LayerData* layerData = GetLayerDataPtr(GetDispatchKey(device));
    if (layerData == nullptr) {
        SWLOGE("libvulkan_swapchain GetInstanceProcAddr layerData is null");
        return nullptr;
    }

    PFN_vkVoidFunction addr = LayerInterceptDeviceProc(layerData->enabledExtensions, funcName);
    if (addr != nullptr) {
        return addr;
    }
    LayerData* devData = GetLayerDataPtr(GetDispatchKey(device));
    VkLayerDispatchTable* pTable = devData->deviceDispatchTable.get();
    if (pTable->GetDeviceProcAddr == nullptr) {
        return nullptr;
    }
    return pTable->GetDeviceProcAddr(device, funcName);
}

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL GetInstanceProcAddr(VkInstance instance, const char* funcName)
{
    if (funcName == nullptr) {
        return nullptr;
    }

    if (strcmp("vkGetInstanceProcAddr", funcName) == 0) {
        return reinterpret_cast<PFN_vkVoidFunction>(GetInstanceProcAddr);
    }
    if (strcmp("vkCreateInstance", funcName) == 0) {
        return reinterpret_cast<PFN_vkVoidFunction>(CreateInstance);
    }
    if (strcmp("vkEnumerateInstanceExtensionProperties", funcName) == 0) {
        return reinterpret_cast<PFN_vkVoidFunction>(EnumerateInstanceExtensionProperties);
    }
    if (strcmp("vkEnumerateInstanceLayerProperties", funcName) == 0) {
        return reinterpret_cast<PFN_vkVoidFunction>(EnumerateInstanceLayerProperties);
    }
    if (strcmp("vkDestroyInstance", funcName) == 0) {
        return reinterpret_cast<PFN_vkVoidFunction>(DestroyInstance);
    }
    if (strcmp("vkCreateDevice", funcName) == 0) {
        return reinterpret_cast<PFN_vkVoidFunction>(CreateDevice);
    }
    if (strcmp("vkEnumerateDeviceExtensionProperties", funcName) == 0) {
        return reinterpret_cast<PFN_vkVoidFunction>(EnumerateDeviceExtensionProperties);
    }

    if (instance == VK_NULL_HANDLE) {
        SWLOGE("SwapchainLayer GetInstanceProcAddr(func name %{public}s) instance is null", funcName);
        return nullptr;
    }

    LayerData* layerData = GetLayerDataPtr(GetDispatchKey(instance));
    if (layerData == nullptr) {
        SWLOGE("SwapchainLayer GetInstanceProcAddr layerData is null");
        return nullptr;
    }

    PFN_vkVoidFunction addr = LayerInterceptInstanceProc(layerData->enabledExtensions, funcName);
    if (addr != nullptr) {
        return addr;
    }

    VkLayerInstanceDispatchTable* pTable = layerData->instanceDispatchTable.get();
    if (pTable == nullptr) {
        SWLOGE("SwapchainLayer GetInstanceProcAddr pTable = null");
        return nullptr;
    }
    if (pTable->GetInstanceProcAddr == nullptr) {
        SWLOGE("SwapchainLayer GetInstanceProcAddr pTable->GetInstanceProcAddr = null");
        return nullptr;
    }
    addr = pTable->GetInstanceProcAddr(instance, funcName);

    return addr;
}
}  // namespace SWAPCHAIN

#if defined(__GNUC__) && __GNUC__ >= 4
#define SWAPCHAIN_EXPORT __attribute__((visibility("default")))
#else
#define SWAPCHAIN_EXPORT
#endif

extern "C" {
SWAPCHAIN_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceExtensionProperties(
    const char* pLayerName, uint32_t* pCount, VkExtensionProperties* pProperties)
{
    return SWAPCHAIN::EnumerateInstanceExtensionProperties(pLayerName, pCount, pProperties);
}

SWAPCHAIN_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkEnumerateInstanceLayerProperties(uint32_t* pCount, VkLayerProperties* pProperties)
{
    return SWAPCHAIN::EnumerateInstanceLayerProperties(pCount, pProperties);
}

SWAPCHAIN_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateDeviceLayerProperties(
    VkPhysicalDevice physicalDevice, uint32_t* pCount, VkLayerProperties* pProperties)
{
    return SWAPCHAIN::EnumerateDeviceLayerProperties(VK_NULL_HANDLE, pCount, pProperties);
}

SWAPCHAIN_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateDeviceExtensionProperties(
    VkPhysicalDevice physicalDevice, const char* pLayerName, uint32_t* pCount, VkExtensionProperties* pProperties)
{
    return SWAPCHAIN::EnumerateDeviceExtensionProperties(VK_NULL_HANDLE, pLayerName, pCount, pProperties);
}

SWAPCHAIN_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vkGetDeviceProcAddr(VkDevice dev, const char* funcName)
{
    return SWAPCHAIN::GetDeviceProcAddr(dev, funcName);
}

SWAPCHAIN_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL
vkGetInstanceProcAddr(VkInstance instance, const char* funcName)
{
    return SWAPCHAIN::GetInstanceProcAddr(instance, funcName);
}
}