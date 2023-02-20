/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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


#include <assert.h>
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
#include <window.h>
#include <graphic_common.h>
#include <native_window.h>


#include <vulkan/vulkan.h>
#include "vulkan/vk_ohos_native_buffer.h"
#include "vk_dispatch_table_helper.h"
#include "vk_layer_dispatch_table.h"
#include "vk_layer_extension_utils.h"
#define SWAPCHAIN_SURFACE_NAME "VK_LAYER_OpenHarmony_OHOS_surface"


using namespace OHOS;
struct LayerData {
    VkInstance instance = VK_NULL_HANDLE;
    uint32_t instance_version = VK_API_VERSION_1_0;
    std::unique_ptr<VkLayerDispatchTable> device_dispatch_table;
    std::unique_ptr<VkLayerInstanceDispatchTable> instance_dispatch_table;
    std::unordered_map<VkDebugUtilsMessengerEXT, VkDebugUtilsMessengerCreateInfoEXT> debug_callbacks;
    PFN_vkSetDeviceLoaderData fpSetDeviceLoaderData = nullptr;
};

namespace {

constexpr uint32_t MIN_BUFFER_SIZE = 3;
// The loader dispatch table may be a different version from the layer's, and can't be used
// directly.
struct LoaderVkLayerDispatchTable;
typedef void* dispatch_key;

// The first value in a dispatchable object should be a pointer to a VkLayerDispatchTable.
// According to the layer documentation: 
// third_party/Vulkan-Loader/docs/LoaderLayerInterface.md.
template <typename T>
inline dispatch_key GetDispatchKey(const T object)
{
    return static_cast<dispatch_key>(*reinterpret_cast<LoaderVkLayerDispatchTable* const*>(object));
}

VkLayerInstanceCreateInfo* GetChainInfo(const VkInstanceCreateInfo* pCreateInfo, VkLayerFunction func)
{
    auto chainInfo = static_cast<const VkLayerInstanceCreateInfo*>(pCreateInfo->pNext);
    while (chainInfo != NULL) {
        if (chainInfo->sType == VK_STRUCTURE_TYPE_LOADER_INSTANCE_CREATE_INFO && chainInfo->function == func) {
            return const_cast<VkLayerInstanceCreateInfo*>(chainInfo);
        }
        chainInfo = static_cast<const VkLayerInstanceCreateInfo*>(chainInfo->pNext);
    }
    assert(false && "Failed to find VkLayerInstanceCreateInfo");
    return nullptr;
}

VkLayerDeviceCreateInfo* GetChainInfo(const VkDeviceCreateInfo* pCreateInfo, VkLayerFunction func)
{
    auto chainInfo = static_cast<const VkLayerDeviceCreateInfo*>(pCreateInfo->pNext);
    while (chainInfo != NULL) {
        if (chainInfo->sType == VK_STRUCTURE_TYPE_LOADER_DEVICE_CREATE_INFO && chainInfo->function == func) {
            return const_cast<VkLayerDeviceCreateInfo*>(chainInfo);
        }
        chainInfo = static_cast<const VkLayerDeviceCreateInfo*>(chainInfo->pNext);
    }
    assert(false && "Failed to find VkLayerDeviceCreateInfo");
    return nullptr;
}
}  // namespace

#define VK_LAYER_API_VERSION VK_MAKE_VERSION(1, 1, VK_HEADER_VERSION)

static inline uint32_t to_uint32(uint64_t val) {
  assert(val <= std::numeric_limits<uint32_t>::max());
  return static_cast<uint32_t>(val);
}

namespace SWAPCHAIN {

// Global because thats how the layer code in the loader works and I dont know
// how to make it work otherwise
std::unordered_map<void*, LayerData*> layer_data_map;
PFN_vkGetNativeFenceFdOpenHarmony pfn_vkGetNativeFenceFdOpenHarmony = nullptr;
PFN_vkGetPhysicalDeviceProperties2KHR fpn_vkGetPhysicalDeviceProperties2KHR = nullptr;


// For the given data key, look up the layer_data instance from given layer_data_map
template <typename DATA_T>
DATA_T *GetLayerDataPtr(void *data_key, std::unordered_map<void *, DATA_T *> &layer_data_map) {
    DATA_T *debug_data;
    /* TODO: We probably should lock here, or have caller lock */
    auto got = layer_data_map.find(data_key);

    if (got == layer_data_map.end()) {
        debug_data = new DATA_T;
        layer_data_map[(void *)data_key] = debug_data;
    } else {
        debug_data = got->second;
    }
    return debug_data;
}
template <typename DATA_T>
void FreeLayerDataPtr(void *data_key, std::unordered_map<void *, DATA_T *> &layer_data_map) {
    auto got = layer_data_map.find(data_key);
    assert(got != layer_data_map.end());

    delete got->second;
    layer_data_map.erase(got);
}

static const VkExtensionProperties instance_extensions[] = {
    {
        .extensionName = VK_KHR_SURFACE_EXTENSION_NAME,
        .specVersion = 25,
    },
    {
        .extensionName = VK_OPENHARMONY_OHOS_SURFACE_EXTENSION_NAME,
        .specVersion = 1,
    }};

static const VkExtensionProperties device_extensions[] = {{
    .extensionName = VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    .specVersion = 70,
}};

constexpr VkLayerProperties swapchain_layer = {
    SWAPCHAIN_SURFACE_NAME,
    VK_LAYER_API_VERSION,
    1,
    "Vulkan Swapchain",
};
///////////////////////////////////////////////////////////////////////////////

struct Surface {
    NativeWindow* window;
    VkSwapchainKHR swapchainHandle;
    int32_t consumerUsage;
};


struct Swapchain {
    Swapchain(Surface& surface, uint32_t numImages, VkPresentModeKHR presentMode, int preTransform)
        : surface(surface), numImages(numImages), mailboxMode(presentMode == VK_PRESENT_MODE_MAILBOX_KHR),
          preTransform(preTransform), frameTimestampsEnabled(false),
          shared(presentMode == VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR ||
                 presentMode == VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR) {}

    Surface& surface;
    uint32_t numImages;
    bool mailboxMode;
    int preTransform;
    bool frameTimestampsEnabled;
    bool shared;

    struct Image {
        Image() : image(VK_NULL_HANDLE), buffer(nullptr), requestFence(-1), releaseFence(-1), requested(false) {}
        VkImage image;
        NativeWindowBuffer* buffer;
        int requestFence;
        int releaseFence;
        bool requested;
    } images[32];

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
    // Vulkan requires 'alignment' to be a power of two, but posix_memalign
    // additionally requires that it be at least sizeof(void*).
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

    if (ptr) {
        auto ret = memcpy_s(newPtr, size, ptr, oldSize);
        if (ret != EOK) {
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


const VkAllocationCallbacks& GetDefaultAllocator()
{
    static const VkAllocationCallbacks kDefaultAllocCallbacks = {
        .pUserData = nullptr,
        .pfnAllocation = DefaultAllocate,
        .pfnReallocation = DefaultReallocate,
        .pfnFree = DefaultFree,
    };

    return kDefaultAllocCallbacks;
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
    return false;
}


void ReleaseSwapchainImage(VkDevice device, NativeWindow* window, int releaseFence, Swapchain::Image& image,
                           bool deferIfPending)
{
    VkLayerDispatchTable* pDisp =
        GetLayerDataPtr(GetDispatchKey(device), layer_data_map)->device_dispatch_table.get();

    // Wait for device to be idle to ensure no QueueSubmit operations caused by Present are pending.
    pDisp->DeviceWaitIdle(device);
    if (image.requested) {
        if (releaseFence >= 0) {
            if (image.requestFence >= 0) {
                close(image.requestFence);
            }
        } else {
            releaseFence = image.requestFence;
        }
        image.requestFence = -1;

        if (window) {
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

    if (image.image) {
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
    GraphicPixelFormat nativeFormat = GRAPHIC_PIXEL_FMT_RGBA_8888;
    switch (format) {
        case VK_FORMAT_R8G8B8A8_UNORM:
        case VK_FORMAT_R8G8B8A8_SRGB:
            nativeFormat = GRAPHIC_PIXEL_FMT_RGBA_8888;
            break;
        case VK_FORMAT_R5G6B5_UNORM_PACK16:
            nativeFormat = GRAPHIC_PIXEL_FMT_RGB_565;
            break;
        default:
            // WLOGE("unsupported swapchain format %{public}d", format);
            break;
    }
    return nativeFormat;
}

VKAPI_ATTR VkResult SetWindowInfo(const VkSwapchainCreateInfoKHR* createInfo, int32_t* numImages)
{
    GraphicPixelFormat pixelFormat = GetPixelFormat(createInfo->imageFormat);
    Surface& surface = *SurfaceFromHandle(createInfo->surface);

    NativeWindow* window = surface.window;
    int err = NativeWindowHandleOpt(window, SET_FORMAT, pixelFormat);
    if (err != OHOS::GSERROR_OK) {
        // WLOGE("native_window_set_buffers_format(%{public}d) failed: (%{public}d)", pixelFormat, err);
        return VK_ERROR_SURFACE_LOST_KHR;
    }

    err = NativeWindowHandleOpt(window, SET_BUFFER_GEOMETRY,
                                static_cast<int>(createInfo->imageExtent.width),
                                static_cast<int>(createInfo->imageExtent.height));
    if (err != OHOS::GSERROR_OK) {
        // WLOGE("native_window_set_buffers_dimensions(%{public}d,%{public}d) failed: (%{public}d)",
            // createInfo->imageExtent.width, createInfo->imageExtent.height, err);
        return VK_ERROR_SURFACE_LOST_KHR;
    }

    if (createInfo->presentMode == VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR ||
        createInfo->presentMode == VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR) {
        *numImages = 1;
    }

    uint64_t native_usage = OHOS::BUFFER_USAGE_CPU_READ | OHOS::BUFFER_USAGE_CPU_WRITE | OHOS::BUFFER_USAGE_MEM_DMA;
    err = NativeWindowHandleOpt(window, SET_USAGE, native_usage);
    if (err != OHOS::GSERROR_OK) {
        // WLOGE("native_window_set_buffer_count(%{public}d) failed: (%{public}d)", *numImages, err);
        return VK_ERROR_SURFACE_LOST_KHR;
    }
    return VK_SUCCESS;
}

VkResult SetSwapchainCreateInfo(VkDevice device, const VkSwapchainCreateInfoKHR* createInfo,
    int32_t* numImages)
{
    GraphicColorDataSpace color_data_space = GetColorDataspace(createInfo->imageColorSpace);
    if (color_data_space == GRAPHIC_COLOR_DATA_SPACE_UNKNOWN) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    if (createInfo->oldSwapchain != VK_NULL_HANDLE) {
        ReleaseSwapchain(device, SwapchainFromHandle(createInfo->oldSwapchain));
    }

    return SetWindowInfo(createInfo, numImages);
}


int TranslateVulkanToNativeTransform(VkSurfaceTransformFlagBitsKHR transform)
{
    return ROTATE_NONE;
}

void InitImageCreateInfo(const VkSwapchainCreateInfoKHR* createInfo, VkImageCreateInfo* imageCreate)
{
    imageCreate->sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreate->flags = 0u;
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


VKAPI_ATTR VkResult CreateImages(int32_t& numImages, Swapchain* swapchain, const VkSwapchainCreateInfoKHR* createInfo,
    VkImageCreateInfo& imageCreate, VkDevice device)
{
    VkLayerDispatchTable* pDisp =
        GetLayerDataPtr(GetDispatchKey(device), layer_data_map)->device_dispatch_table.get();
    Surface& surface = *SurfaceFromHandle(createInfo->surface);
    NativeWindow* window = surface.window;
    if (createInfo->oldSwapchain != VK_NULL_HANDLE) {
        // WLOGI("recreate swapchain ,clean buffer queue");
        window->surface->CleanCache();
    }
    VkResult result = VK_SUCCESS;
    for (int32_t i = 0; i < numImages; i++) {
        Swapchain::Image& img = swapchain->images[i];

        NativeWindowBuffer* buffer;
        int err = NativeWindowRequestBuffer(window, &buffer, &img.requestFence);
        if (err != OHOS::GSERROR_OK) {
            // WLOGE("dequeueBuffer[%{public}u] failed: (%{public}d)", i, err);
            result = VK_ERROR_SURFACE_LOST_KHR;
            break;
        }
        img.buffer = buffer;
        img.requested = true;
        imageCreate.extent = VkExtent3D {static_cast<uint32_t>(img.buffer->sfbuffer->GetSurfaceBufferWidth()),
                                          static_cast<uint32_t>(img.buffer->sfbuffer->GetSurfaceBufferHeight()), 1};
        ((VkNativeBufferOpenHarmony*)(imageCreate.pNext))->handle = img.buffer->sfbuffer->GetBufferHandle();
        result = pDisp->CreateImage(device, &imageCreate, nullptr, &img.image);
        if (result != VK_SUCCESS) {
            // WLOGD("vkCreateImage native buffer failed: %{public}u", result);
            break;
        }
    }

    // WLOGD("swapchain init shared %{public}d", swapchain->shared);
    for (int32_t i = 0; i < numImages; i++) {
        Swapchain::Image& img = swapchain->images[i];
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
    if (!swapchain) {
        return;
    }

    bool active = swapchain->surface.swapchainHandle == swapchainHandle;
    NativeWindow* window = active ? swapchain->surface.window : nullptr;

    for (uint32_t i = 0; i < swapchain->numImages; i++) {
        ReleaseSwapchainImage(device, window, -1, swapchain->images[i], false);
    }

    if (active) {
        swapchain->surface.swapchainHandle = VK_NULL_HANDLE;
    }
    if (!allocator) {
        allocator = &GetDefaultAllocator();
    }
    swapchain->~Swapchain();
    allocator->pfnFree(allocator->pUserData, swapchain);
}

/////////////////////////////////////////////////////////////////////////////////


VKAPI_ATTR VkResult VKAPI_CALL CreateSwapchainKHR(VkDevice device, const VkSwapchainCreateInfoKHR* createInfo,
    const VkAllocationCallbacks* allocator, VkSwapchainKHR* swapchainHandle)
{
    Surface& surface = *SurfaceFromHandle(createInfo->surface);
    if (surface.swapchainHandle != createInfo->oldSwapchain) {
        return VK_ERROR_NATIVE_WINDOW_IN_USE_KHR;
    }

    int32_t numImages = static_cast<int32_t>(MIN_BUFFER_SIZE);
    VkResult result = SetSwapchainCreateInfo(device, createInfo, &numImages);
    if (result != VK_SUCCESS) {
        return result;
    }

    if (!allocator) {
        allocator = &GetDefaultAllocator();
    }
    void* mem = allocator->pfnAllocation(allocator->pUserData, sizeof(Swapchain), alignof(Swapchain),
        VK_SYSTEM_ALLOCATION_SCOPE_OBJECT);
    if (!mem) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    Swapchain* swapchain = new (mem) Swapchain(surface, numImages, createInfo->presentMode,
        TranslateVulkanToNativeTransform(createInfo->preTransform));
    VkSwapchainImageCreateInfoOpenHarmony swapchainImageCreate = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_IMAGE_CREATE_INFO_OPENHARMONY,
        .pNext = nullptr,
    };
    VkNativeBufferOpenHarmony imageNativeBuffer = {
        .sType = VK_STRUCTURE_TYPE_NATIVE_BUFFER_OPENHARMONY,
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
    VkDevice device, VkSwapchainKHR vk_swapchain,const VkAllocationCallbacks* pAllocator)
{
    DestroySwapchainInternal(device, vk_swapchain, pAllocator);
}



VKAPI_ATTR VkResult VKAPI_CALL GetSwapchainImagesKHR(
    VkDevice device, VkSwapchainKHR vk_swapchain, uint32_t* count, VkImage* images)
{
    const Swapchain& swapchain = *SwapchainFromHandle(vk_swapchain);
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

VKAPI_ATTR VkResult SetNativeFenceFdOpenHarmony(VkDevice device,
    int nativeFenceFd,
    VkSemaphore semaphore,
    VkFence fence)
{
    PFN_vkSetNativeFenceFdOpenHarmony acquireImage = reinterpret_cast<PFN_vkSetNativeFenceFdOpenHarmony>(
        GetDeviceProcAddr(device, "vkSetNativeFenceFdOpenHarmony"));
    if (acquireImage) {
        return acquireImage(device, nativeFenceFd, semaphore, fence);
    }
    return VK_ERROR_INITIALIZATION_FAILED;
}


VKAPI_ATTR VkResult VKAPI_CALL AcquireNextImageKHR(VkDevice device, VkSwapchainKHR swapchainHandle,
    uint64_t timeout, VkSemaphore semaphore, VkFence vk_fence, uint32_t* imageIndex)
{
    Swapchain& swapchain = *SwapchainFromHandle(swapchainHandle);
    NativeWindow* nativeWindow = swapchain.surface.window;
    VkResult result;

    if (swapchain.surface.swapchainHandle != swapchainHandle) {
        return VK_ERROR_OUT_OF_DATE_KHR;
    }

    if (swapchain.shared) {
        *imageIndex = 0;
        result = SetNativeFenceFdOpenHarmony(device, -1, semaphore, vk_fence);
        return result;
    }

    NativeWindowBuffer* nativeWindowBuffer;
    int fence;
    int32_t ret = NativeWindowRequestBuffer(nativeWindow, &nativeWindowBuffer, &fence);
    if (ret != OHOS::GSERROR_OK) {
        // WLOGE("dequeueBuffer failed: (%{public}d)", ret);
        return VK_ERROR_SURFACE_LOST_KHR;
    }

    uint32_t index = 0;
    while (index < swapchain.numImages) {
        if (swapchain.images[index].buffer->sfbuffer == nativeWindowBuffer->sfbuffer) {
            swapchain.images[index].requested = true;
            swapchain.images[index].requestFence = fence;
            break;
        }
        index++;
    }

    if (index == swapchain.numImages) {
        // WLOGE("dequeueBuffer returned unrecognized buffer");
        if (NativeWindowCancelBuffer(nativeWindow, nativeWindowBuffer) != OHOS::GSERROR_OK) {
            // WLOGE("NativeWindowCancelBuffer failed: (%{public}d)", ret);
        }
        return VK_ERROR_OUT_OF_DATE_KHR;
    }

    result = SetNativeFenceFdOpenHarmony(device, -1, semaphore, vk_fence);
    if (result != VK_SUCCESS) {
        if (NativeWindowCancelBuffer(nativeWindow, nativeWindowBuffer) != OHOS::GSERROR_OK) {
            // WLOGE("NativeWindowCancelBuffer failed: (%{public}d)", ret);
        }
        swapchain.images[index].requested = false;
        swapchain.images[index].requestFence = -1;
        return result;
    }

    *imageIndex = index;
    return VK_SUCCESS;
}

const VkPresentRegionKHR* GetPresentRegion(
    const VkPresentRegionKHR* regions, const Swapchain& swapchain, uint32_t index)
{
    return (regions && !swapchain.mailboxMode) ? &regions[index] : nullptr;
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
        return presentRegions->pRegions;
    }
}


VkResult GetNativeFenceFdOpenHarmony(
    VkQueue queue,
    uint32_t waitSemaphoreCount,
    const VkSemaphore* pWaitSemaphores,
    VkImage image,
    int* pNativeFenceFd)
{
    if (pfn_vkGetNativeFenceFdOpenHarmony) {
        return pfn_vkGetNativeFenceFdOpenHarmony(queue, waitSemaphoreCount, pWaitSemaphores, image, pNativeFenceFd);
    }
    return VK_ERROR_INITIALIZATION_FAILED;
}

VkResult GetReleaseFence(VkQueue queue, const VkPresentInfoKHR* presentInfo,
    Swapchain::Image& img, int32_t &fence)
{
    VkResult result = GetNativeFenceFdOpenHarmony(queue, presentInfo->waitSemaphoreCount,
        presentInfo->pWaitSemaphores, img.image, &fence);
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
                    sizeof(Region::Rect) * rectangleCount,
                    alignof(Region::Rect), VK_SYSTEM_ALLOCATION_SCOPE_COMMAND));
}

void InitRegionRect(const VkRectLayerKHR* layer, struct Region::Rect* rect)
{
    rect->x = layer->offset.x;
    rect->y = layer->offset.y;
    rect->w = layer->extent.width;
    rect->h = layer->extent.height;
}


VkResult FlushBuffer(const VkPresentRegionKHR* region, struct Region::Rect* rects,
    Swapchain& swapchain, Swapchain::Image& img, int32_t fence)
{

    const VkAllocationCallbacks* defaultAllocator = &GetDefaultAllocator();
    Region localRegion;
    if (memset_s(&localRegion, sizeof(localRegion), 0, sizeof(Region)) != EOK) {
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
            InitRegionRect(&region->pRectangles[r], &rects[r]);
        }

        localRegion.rects = rects;
        localRegion.rectNumber = rectangleCount;
    }
    NativeWindow* window = swapchain.surface.window;
    // the acquire fence will be close by BufferQueue module
    int err = NativeWindowFlushBuffer(window, img.buffer, fence, localRegion);
    VkResult scResult = VK_SUCCESS;
    if (err != OHOS::GSERROR_OK) {
        // WLOGE("queueBuffer failed: (%{public}d)", err);
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
    VkQueue queue,const VkPresentInfoKHR* presentInfo)
{
    VkResult ret = VK_SUCCESS;

    const VkPresentRegionKHR* regions = GetPresentRegions(presentInfo);
    const VkAllocationCallbacks* defaultAllocator = &GetDefaultAllocator();
    struct Region::Rect* rects = nullptr;

    for (uint32_t i = 0; i < presentInfo->swapchainCount; i++) {
        Swapchain& swapchain = *(reinterpret_cast<Swapchain*>(presentInfo->pSwapchains[i]));
        Swapchain::Image& img = swapchain.images[presentInfo->pImageIndices[i]];
        const VkPresentRegionKHR* region = GetPresentRegion(regions, swapchain, i);
        int32_t fence = -1;
        ret = GetReleaseFence(queue, presentInfo, img, fence);
        if (swapchain.surface.swapchainHandle == presentInfo->pSwapchains[i]) {
            if (ret == VK_SUCCESS) {
                ret = FlushBuffer(region, rects, swapchain, img, fence);
            } else {
                ReleaseSwapchain(nullptr, &swapchain);
            }
        } else {
            // WLOGE("QueuePresentKHR swapchainHandle != pSwapchains[%{public}d]", i);
            ReleaseSwapchainImage(nullptr, nullptr, fence, img, true);
            ret = VK_ERROR_OUT_OF_DATE_KHR;
        }

        if (presentInfo->pResults) {
            presentInfo->pResults[i] = ret;
        }
    }
    if (rects != nullptr) {
        defaultAllocator->pfnFree(defaultAllocator->pUserData, rects);
    }
    return ret;
}

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceSurfaceSupportKHR(
    VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, const VkSurfaceKHR surface, VkBool32* pSupported)
{
    *pSupported = VK_TRUE;
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL CreateOHOSSurfaceOpenHarmony(VkInstance instance,
    const VkOHOSSurfaceCreateInfoOpenHarmony* pCreateInfo,
    const VkAllocationCallbacks* allocator, VkSurfaceKHR* outSurface)
{
    if (!allocator) {
        allocator = &GetDefaultAllocator();
    }
    void* mem = allocator->pfnAllocation(allocator->pUserData, sizeof(Surface), alignof(Surface),
                                         VK_SYSTEM_ALLOCATION_SCOPE_OBJECT);
    if (!mem) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    Surface* surface = new (mem) Surface;
    surface->window = pCreateInfo->window;
    surface->swapchainHandle = VK_NULL_HANDLE;
    NativeWindowHandleOpt(pCreateInfo->window, GET_USAGE, &(surface->consumerUsage));

    if (surface->consumerUsage == 0) {
        // WLOGE("native window get usage failed, error num : %{public}d", res);
        surface->~Surface();
        allocator->pfnFree(allocator->pUserData, surface);
        return VK_ERROR_SURFACE_LOST_KHR;
    }

    *outSurface = HandleFromSurface(surface);
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL DestroySurfaceKHR(
    VkInstance instance, VkSurfaceKHR vk_surface, const VkAllocationCallbacks* pAllocator)
{
    Surface* surface = SurfaceFromHandle(vk_surface);
    if (!surface) {
        return;
    }
    if (!pAllocator) {
        pAllocator = &GetDefaultAllocator();
    }
    surface->~Surface();
    pAllocator->pfnFree(pAllocator->pUserData, surface);
}

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceSurfaceCapabilitiesKHR(
    VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkSurfaceCapabilitiesKHR* capabilities)
{
    int width = 0;
    int height = 0;
    uint32_t maxBufferCount = 10;
    if (surface != VK_NULL_HANDLE) {
        NativeWindow* window = SurfaceFromHandle(surface)->window;
        int err = NativeWindowHandleOpt(window, GET_BUFFER_GEOMETRY, &height, &width);
        if (err != OHOS::GSERROR_OK) {
            // WLOGE("NATIVE_WINDOW_DEFAULT_WIDTH query failed: (%{public}d)", err);
            return VK_ERROR_SURFACE_LOST_KHR;
        }
        maxBufferCount = window->surface->GetQueueSize();
    }

    capabilities->minImageCount = std::min(maxBufferCount, MIN_BUFFER_SIZE);
    capabilities->maxImageCount = maxBufferCount;
    capabilities->currentExtent = VkExtent2D {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
    capabilities->minImageExtent = VkExtent2D {1, 1};
    capabilities->maxImageExtent = VkExtent2D {4096, 4096};
    capabilities->maxImageArrayLayers = 1;
    capabilities->supportedCompositeAlpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
    capabilities->supportedUsageFlags = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                        VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT |
                                        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceSurfaceFormatsKHR(
    VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface, uint32_t* count, VkSurfaceFormatKHR* formats)
{
    std::vector<VkSurfaceFormatKHR> allFormats = {{VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
        {VK_FORMAT_R8G8B8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR}};
    VkResult result = VK_SUCCESS;
    if (formats) {
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


void GetPhysicalDeviceProperties2(VkPhysicalDevice physicalDevice, VkPhysicalDeviceProperties2* pProperties)
{
    if (fpn_vkGetPhysicalDeviceProperties2KHR) {
        fpn_vkGetPhysicalDeviceProperties2KHR(physicalDevice, pProperties);
    }
}

void QueryPresentationProperties(
    VkPhysicalDevice physicalDevice,
    VkPhysicalDevicePresentationPropertiesOpenHarmony* presentation_properties)
{
    VkPhysicalDeviceProperties2 properties = {
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
        presentation_properties,
        {},
    };

    presentation_properties->sType =
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENTATION_PROPERTIES_OPENHARMONY;
    presentation_properties->pNext = nullptr;
    presentation_properties->sharedImage = VK_FALSE;

    GetPhysicalDeviceProperties2(physicalDevice, &properties);
}

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceSurfacePresentModesKHR(
    VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface, uint32_t* count, VkPresentModeKHR* pPresentModes)
{
    std::vector<VkPresentModeKHR> presentModes;
    presentModes.push_back(VK_PRESENT_MODE_MAILBOX_KHR);
    presentModes.push_back(VK_PRESENT_MODE_FIFO_KHR);

    VkPhysicalDevicePresentationPropertiesOpenHarmony present_properties;
    QueryPresentationProperties(physicalDevice, &present_properties);
    if (present_properties.sharedImage) {
        presentModes.push_back(VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR);
        presentModes.push_back(VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR);
    }

    uint32_t numModes = uint32_t(presentModes.size());
    VkResult result = VK_SUCCESS;
    if (pPresentModes) {
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

VKAPI_ATTR VkResult VKAPI_CALL CreateInstance(
    const VkInstanceCreateInfo* pCreateInfo,const VkAllocationCallbacks* pAllocator, VkInstance* pInstance)
{
    VkLayerInstanceCreateInfo* chain_info = GetChainInfo(pCreateInfo, VK_LAYER_LINK_INFO);

    assert(chain_info->u.pLayerInfo);
    PFN_vkGetInstanceProcAddr fpGetInstanceProcAddr =
        chain_info->u.pLayerInfo->pfnNextGetInstanceProcAddr;
    PFN_vkCreateInstance fpCreateInstance =
        (PFN_vkCreateInstance)fpGetInstanceProcAddr(NULL, "vkCreateInstance");
    if (fpCreateInstance == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    // Advance the link info for the next element on the chain
    chain_info->u.pLayerInfo = chain_info->u.pLayerInfo->pNext;

    VkResult result = fpCreateInstance(pCreateInfo, pAllocator, pInstance);
    if (result != VK_SUCCESS)
        return result;

    LayerData* instance_layer_data = GetLayerDataPtr(GetDispatchKey(*pInstance), layer_data_map);
    instance_layer_data->instance = *pInstance;
    instance_layer_data->instance_dispatch_table = std::make_unique<VkLayerInstanceDispatchTable>();
    layer_init_instance_dispatch_table(*pInstance, instance_layer_data->instance_dispatch_table.get(),
                                        fpGetInstanceProcAddr);

    fpn_vkGetPhysicalDeviceProperties2KHR = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties2KHR>(
            fpGetInstanceProcAddr(*pInstance, "vkGetPhysicalDeviceProperties2KHR"));
    if (fpn_vkGetPhysicalDeviceProperties2KHR == nullptr) {
        std::printf("libvulkan_swapchain :: CreateInstance fpn_vkGetPhysicalDeviceProperties2KHR is %p", fpn_vkGetPhysicalDeviceProperties2KHR);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyInstance(
    VkInstance instance, const VkAllocationCallbacks* pAllocator)
{
    dispatch_key instance_key = GetDispatchKey(instance);
    LayerData* curLayerData = GetLayerDataPtr(instance_key, layer_data_map);
    curLayerData->instance_dispatch_table->DestroyInstance(instance, pAllocator);
    // Remove from |layer_data_map| and free LayerData struct.
    FreeLayerDataPtr(instance_key, layer_data_map);
}

VKAPI_ATTR VkResult VKAPI_CALL CreateDevice(VkPhysicalDevice gpu,
    const VkDeviceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDevice* pDevice)
{
    void* gpu_key = GetDispatchKey(gpu);
    LayerData* gpu_layer_data = GetLayerDataPtr(gpu_key, layer_data_map);

    VkResult result;
    bool native_buffer_extension_available = false;
    bool external_memory_extension_available = false;

    uint32_t device_extension_count;
    result = gpu_layer_data->instance_dispatch_table->EnumerateDeviceExtensionProperties(
        gpu, nullptr, &device_extension_count, nullptr);
    if (result == VK_SUCCESS && device_extension_count > 0) {
        std::vector<VkExtensionProperties> device_extensions(device_extension_count);
        result = gpu_layer_data->instance_dispatch_table->EnumerateDeviceExtensionProperties(
            gpu, nullptr, &device_extension_count, device_extensions.data());
        if (result == VK_SUCCESS) {
            for (uint32_t i = 0; i < device_extension_count; i++) {
                if (!strcmp(VK_OPENHARMONY_EXTERNAL_MEMORY_OHOS_NATIVE_BUFFER_EXTENSION_NAME,
                            device_extensions[i].extensionName)) {
                    external_memory_extension_available = true;
                }
                if (!strcmp(VK_OHOS_NATIVE_BUFFER_EXTENSION_NAME,
                            device_extensions[i].extensionName)) {
                    native_buffer_extension_available = true;
                }
            
            }
        }
    }
    if (!native_buffer_extension_available) {
        fprintf(stderr, "Native Buffer extension not available\n");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    VkDeviceCreateInfo create_info = *pCreateInfo;
    std::vector<const char*> enabled_extensions;
    for (uint32_t i = 0; i < create_info.enabledExtensionCount; i++) {
        enabled_extensions.push_back(create_info.ppEnabledExtensionNames[i]);
    }
    if (native_buffer_extension_available) {
        enabled_extensions.push_back(VK_OHOS_NATIVE_BUFFER_EXTENSION_NAME);
    }
    fprintf(stderr, "native buffer extension %d available\n", native_buffer_extension_available);

    if (external_memory_extension_available) {
        enabled_extensions.push_back(VK_OPENHARMONY_EXTERNAL_MEMORY_OHOS_NATIVE_BUFFER_EXTENSION_NAME);
    }
    fprintf(stderr, "external memory extension %d available\n", external_memory_extension_available);

    create_info.enabledExtensionCount = to_uint32(enabled_extensions.size());
    create_info.ppEnabledExtensionNames = enabled_extensions.data();

    VkLayerDeviceCreateInfo* link_info = GetChainInfo(pCreateInfo, VK_LAYER_LINK_INFO);

    assert(link_info->u.pLayerInfo);
    PFN_vkGetInstanceProcAddr fpGetInstanceProcAddr =
        link_info->u.pLayerInfo->pfnNextGetInstanceProcAddr;
    PFN_vkGetDeviceProcAddr fpGetDeviceProcAddr = link_info->u.pLayerInfo->pfnNextGetDeviceProcAddr;
    PFN_vkCreateDevice fpCreateDevice =
        (PFN_vkCreateDevice)fpGetInstanceProcAddr(gpu_layer_data->instance, "vkCreateDevice");
    if (fpCreateDevice == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    // Advance the link info for the next element on the chain
    link_info->u.pLayerInfo = link_info->u.pLayerInfo->pNext;

    result = fpCreateDevice(gpu, &create_info, pAllocator, pDevice);
    if (result != VK_SUCCESS) {
        return result;
    }

    LayerData* device_layer_data = GetLayerDataPtr(GetDispatchKey(*pDevice), layer_data_map);

    // Setup device dispatch table
    device_layer_data->device_dispatch_table = std::make_unique<VkLayerDispatchTable>();
    device_layer_data->instance = gpu_layer_data->instance;
    layer_init_device_dispatch_table(*pDevice, device_layer_data->device_dispatch_table.get(),
                                    fpGetDeviceProcAddr);

    VkLayerDeviceCreateInfo* callback_info = GetChainInfo(pCreateInfo, VK_LOADER_DATA_CALLBACK);
    assert(callback_info->u.pfnSetDeviceLoaderData);
    device_layer_data->fpSetDeviceLoaderData = callback_info->u.pfnSetDeviceLoaderData;
    if (fpGetDeviceProcAddr != NULL) {
        pfn_vkGetNativeFenceFdOpenHarmony = reinterpret_cast<PFN_vkGetNativeFenceFdOpenHarmony>(fpGetDeviceProcAddr(*pDevice, "vkGetNativeFenceFdOpenHarmony"));
    }
    
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL DestroyDevice(VkDevice device, const VkAllocationCallbacks* pAllocator)
{
    dispatch_key device_key = GetDispatchKey(device);
    LayerData* device_data = GetLayerDataPtr(device_key, layer_data_map);
    device_data->device_dispatch_table->DestroyDevice(device, pAllocator);

    // Remove from |layer_data_map| and free LayerData struct.
    FreeLayerDataPtr(device_key, layer_data_map);
}

VKAPI_ATTR VkResult VKAPI_CALL CreateDebugUtilsMessengerEXT( VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pMessenger)
{
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL DestroyDebugUtilsMessengerEXT(
    VkInstance instance, VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks* pAllocator)
{

}





//////////////////////////////////////////////////////////////////////////////////////

VKAPI_ATTR VkResult VKAPI_CALL EnumerateInstanceExtensionProperties(
    const char* pLayerName, uint32_t* pCount, VkExtensionProperties* pProperties)
{
    if (pLayerName && !strcmp(pLayerName, swapchain_layer.layerName)) {
        return util_GetExtensionProperties(
            std::size(instance_extensions), instance_extensions, pCount, pProperties);
    }

    return VK_ERROR_LAYER_NOT_PRESENT;
}

VKAPI_ATTR VkResult VKAPI_CALL EnumerateInstanceLayerProperties( uint32_t* pCount,VkLayerProperties* pProperties)
{
    return util_GetLayerProperties(1, &swapchain_layer, pCount, pProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL EnumerateDeviceLayerProperties(
    VkPhysicalDevice physicalDevice, uint32_t* pCount, VkLayerProperties* pProperties)
{
    return util_GetLayerProperties(1, &swapchain_layer, pCount, pProperties);
}


VKAPI_ATTR VkResult VKAPI_CALL EnumerateDeviceExtensionProperties(
    VkPhysicalDevice physicalDevice, const char* pLayerName, uint32_t* pCount, VkExtensionProperties* pProperties)
{
    if (pLayerName && !strcmp(pLayerName, swapchain_layer.layerName)) {
        return util_GetExtensionProperties(
            std::size(device_extensions), device_extensions, pCount,pProperties);
    }

    assert(physicalDevice);

    dispatch_key key = GetDispatchKey(physicalDevice);
    LayerData* my_data = GetLayerDataPtr(key, layer_data_map);
    return my_data->instance_dispatch_table->EnumerateDeviceExtensionProperties(physicalDevice, NULL,
                                                                                pCount, pProperties);
}

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL GetPhysicalDeviceProcAddr(VkInstance instance,
                                                                   const char* funcName)
{
    assert(instance);

    LayerData* layerData = GetLayerDataPtr(GetDispatchKey(instance), layer_data_map);
    VkLayerInstanceDispatchTable* pTable = layerData->instance_dispatch_table.get();

    if (pTable->GetPhysicalDeviceProcAddr == NULL) {
        return NULL;
    }
    return pTable->GetPhysicalDeviceProcAddr(instance, funcName);
}


static inline PFN_vkVoidFunction LayerInterceptProc(const char* name) {
    if (!name || name[0] != 'v' || name[1] != 'k') {
        return NULL;
    }
    name += 2;
    if (!strcmp(name, "GetDeviceProcAddr")) {
        return reinterpret_cast<PFN_vkVoidFunction>(GetDeviceProcAddr);
    }
    if (!strcmp(name, "CreateInstance")) {
        return reinterpret_cast<PFN_vkVoidFunction>(CreateInstance);
    }
    if (!strcmp(name, "DestroyInstance")) {
        return reinterpret_cast<PFN_vkVoidFunction>(DestroyInstance);
    }
    if (!strcmp(name, "CreateDevice")) {
        return reinterpret_cast<PFN_vkVoidFunction>(CreateDevice);
    }
    if (!strcmp(name, "DestroyDevice")) {
        return reinterpret_cast<PFN_vkVoidFunction>(DestroyDevice);
    }
    if (!strcmp("CreateSwapchainKHR", name)) {
        return reinterpret_cast<PFN_vkVoidFunction>(CreateSwapchainKHR);
    }
    if (!strcmp("DestroySwapchainKHR", name)) {
        return reinterpret_cast<PFN_vkVoidFunction>(DestroySwapchainKHR);
    }
    if (!strcmp("GetSwapchainImagesKHR", name)) {
        return reinterpret_cast<PFN_vkVoidFunction>(GetSwapchainImagesKHR);
    }
    if (!strcmp("AcquireNextImageKHR", name)) {
        return reinterpret_cast<PFN_vkVoidFunction>(AcquireNextImageKHR);
    }
    if (!strcmp("QueuePresentKHR", name)) {
        return reinterpret_cast<PFN_vkVoidFunction>(QueuePresentKHR);
    }
    if (!strcmp("EnumerateDeviceExtensionProperties", name)) {
        return reinterpret_cast<PFN_vkVoidFunction>(EnumerateDeviceExtensionProperties);
    }
    if (!strcmp("EnumerateInstanceExtensionProperties", name)) {
        return reinterpret_cast<PFN_vkVoidFunction>(EnumerateInstanceExtensionProperties);
    }
    if (!strcmp("EnumerateDeviceLayerProperties", name)) {
        return reinterpret_cast<PFN_vkVoidFunction>(EnumerateDeviceLayerProperties);
    }
    if (!strcmp("EnumerateInstanceLayerProperties", name)) {
        return reinterpret_cast<PFN_vkVoidFunction>(EnumerateInstanceLayerProperties);
    }
    if (!strcmp(name, "CreateDebugUtilsMessengerEXT")) {
        return reinterpret_cast<PFN_vkVoidFunction>(CreateDebugUtilsMessengerEXT);
    }
    if (!strcmp(name, "DestroyDebugUtilsMessengerEXT")) {
        return reinterpret_cast<PFN_vkVoidFunction>(DestroyDebugUtilsMessengerEXT);
    }
    return NULL;
}

static inline PFN_vkVoidFunction LayerInterceptInstanceProc(const char* name) {
    if (!name || name[0] != 'v' || name[1] != 'k') {
        return NULL;
    }
    name += 2;
    if (!strcmp(name, "GetInstanceProcAddr")) {
        return reinterpret_cast<PFN_vkVoidFunction>(GetInstanceProcAddr);
    }
    if (!strcmp(name, "CreateInstance")) {
        return reinterpret_cast<PFN_vkVoidFunction>(CreateInstance);
    }
    if (!strcmp(name, "DestroyInstance")) {
        return reinterpret_cast<PFN_vkVoidFunction>(DestroyInstance);
    }
    if (!strcmp("GetPhysicalDeviceSurfaceSupportKHR", name)) {
        return reinterpret_cast<PFN_vkVoidFunction>(GetPhysicalDeviceSurfaceSupportKHR);
    }
    if (!strcmp("GetPhysicalDeviceSurfaceCapabilitiesKHR", name)) {
        return reinterpret_cast<PFN_vkVoidFunction>(GetPhysicalDeviceSurfaceCapabilitiesKHR);
    }
    if (!strcmp("GetPhysicalDeviceSurfaceFormatsKHR", name)) {
        return reinterpret_cast<PFN_vkVoidFunction>(GetPhysicalDeviceSurfaceFormatsKHR);
    }
    if (!strcmp("GetPhysicalDeviceSurfacePresentModesKHR", name)) {
        return reinterpret_cast<PFN_vkVoidFunction>(GetPhysicalDeviceSurfacePresentModesKHR);
    }
    if (!strcmp("CreateOHOSSurfaceOpenHarmony", name)) {
        return reinterpret_cast<PFN_vkVoidFunction>(CreateOHOSSurfaceOpenHarmony);
    }
    if (!strcmp("DestroySurfaceKHR", name)) {
        return reinterpret_cast<PFN_vkVoidFunction>(DestroySurfaceKHR);
    }
    if (!strcmp(name, "CreateDebugUtilsMessengerEXT")) {
        return reinterpret_cast<PFN_vkVoidFunction>(CreateDebugUtilsMessengerEXT);
    }
    if (!strcmp(name, "DestroyDebugUtilsMessengerEXT")) {
        return reinterpret_cast<PFN_vkVoidFunction>(DestroyDebugUtilsMessengerEXT);
    }
    return NULL;
}


VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL GetDeviceProcAddr(VkDevice device, const char* funcName)
{
    assert(device);
    PFN_vkVoidFunction addr = LayerInterceptProc(funcName);
    if (addr != NULL) {
        return addr;
    }
    LayerData* dev_data = GetLayerDataPtr(GetDispatchKey(device), layer_data_map);
    VkLayerDispatchTable* pTable = dev_data->device_dispatch_table.get();
    if (pTable->GetDeviceProcAddr == NULL) {
        return NULL;
    }
    return pTable->GetDeviceProcAddr(device, funcName);
}

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL GetInstanceProcAddr(VkInstance instance, const char* funcName)
{
    PFN_vkVoidFunction addr = LayerInterceptInstanceProc(funcName);
    if (addr == NULL) {
        addr = LayerInterceptProc(funcName);
    }
    if (addr != NULL) {
        return addr;
    }
    if (instance == NULL) {
        std::cout << "libvulkan_swapchain GetInstanceProcAddr instance is NULL" << std::endl;
        return NULL;
    }
    LayerData* layerData = GetLayerDataPtr(GetDispatchKey(instance), layer_data_map);
    if (!layerData) {
        printf("libvulkan_swapchain GetInstanceProcAddr layerData is NULL \n");
        return NULL;
    }

    VkLayerInstanceDispatchTable* pTable = layerData->instance_dispatch_table.get();
    if (pTable == NULL) {
        printf("libvulkan_swapchain GetInstanceProcAddr pTable = NULL WHY???\n");
        return NULL;
    }
    if (pTable->GetInstanceProcAddr == NULL) {
        printf("libvulkan_swapchain GetInstanceProcAddr pTable->GetInstanceProcAddr = NULL \n");
        return NULL;
    }
    addr = pTable->GetInstanceProcAddr(instance, funcName);

    return addr;
}



///////////////////////////////////////////////////////////////////////////////////////////


}  // namespace SWAPCHAIN
extern "C" {
__attribute__((visibility("default")))
VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceExtensionProperties(
    const char* pLayerName, uint32_t* pCount, VkExtensionProperties* pProperties)
{
    return SWAPCHAIN::EnumerateInstanceExtensionProperties(pLayerName, pCount,
                                                                    pProperties);
}

__attribute__((visibility("default")))
VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkEnumerateInstanceLayerProperties(uint32_t* pCount, VkLayerProperties* pProperties)
{
    return SWAPCHAIN::EnumerateInstanceLayerProperties(pCount, pProperties);
}

__attribute__((visibility("default")))
VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateDeviceLayerProperties(
    VkPhysicalDevice physicalDevice, uint32_t* pCount, VkLayerProperties* pProperties)
{
    assert(physicalDevice == VK_NULL_HANDLE);
    return SWAPCHAIN::EnumerateDeviceLayerProperties(VK_NULL_HANDLE, pCount, pProperties);
}

__attribute__((visibility("default")))
VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateDeviceExtensionProperties(
    VkPhysicalDevice physicalDevice, const char* pLayerName, uint32_t* pCount, VkExtensionProperties* pProperties)
{
    assert(physicalDevice == VK_NULL_HANDLE);
    return SWAPCHAIN::EnumerateDeviceExtensionProperties(VK_NULL_HANDLE, pLayerName, pCount, pProperties);
}

__attribute__((visibility("default")))
VK_LAYER_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vkGetDeviceProcAddr(VkDevice dev,
                                                                             const char* funcName) {
    return SWAPCHAIN::GetDeviceProcAddr(dev, funcName);
}

__attribute__((visibility("default")))
VK_LAYER_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL
vkGetInstanceProcAddr(VkInstance instance, const char* funcName) {
  return SWAPCHAIN::GetInstanceProcAddr(instance, funcName);
}
}