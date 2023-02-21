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
#include "swapchain.h"
#include <algorithm>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <new>
#include <malloc.h>
#include <native_window.h>
#include <window.h>
#include <native_buffer.h>
#include <wrapper_log.h>
#include <graphic_common.h>
#include "vulkan/vk_ohos_native_buffer.h"
#include <securec.h>

namespace vulkan {
namespace driver {
namespace {
constexpr uint32_t MIN_BUFFER_SIZE = 3;
}
struct Surface {
    NativeWindow* window;
    VkSwapchainKHR swapchainHandle;
    int32_t consumerUsage;
};

using namespace OHOS;

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

VKAPI_ATTR VkResult CreateOHOSSurfaceOpenHarmony(VkInstance instance,
    const VkOHOSSurfaceCreateInfoOpenHarmony* pCreateInfo,
    const VkAllocationCallbacks* allocator,
    VkSurfaceKHR* outSurface)
{
    if (!allocator) {
        allocator = &vulkan::driver::GetDefaultAllocator();
    }
    void* mem = allocator->pfnAllocation(allocator->pUserData, sizeof(Surface), alignof(Surface),
                                         VK_SYSTEM_ALLOCATION_SCOPE_OBJECT);
    if (!mem) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    Surface* surface = new (mem) Surface;
    surface->window = pCreateInfo->window;
    surface->swapchainHandle = VK_NULL_HANDLE;
    int32_t res = NativeWindowHandleOpt(pCreateInfo->window, GET_USAGE, &(surface->consumerUsage));

    if (surface->consumerUsage == 0) {
        WLOGE("native window get usage failed, error num : %{public}d", res);
        surface->~Surface();
        allocator->pfnFree(allocator->pUserData, surface);
        return VK_ERROR_SURFACE_LOST_KHR;
    }

    *outSurface = HandleFromSurface(surface);
    return VK_SUCCESS;
}


VKAPI_ATTR void DestroySurfaceKHR(VkInstance instance, VkSurfaceKHR surfaceHandle,
    const VkAllocationCallbacks* allocator)
{
    Surface* surface = SurfaceFromHandle(surfaceHandle);
    if (!surface) {
        return;
    }

    surface->~Surface();
    if (!allocator) {
        allocator = &vulkan::driver::GetDefaultAllocator();
    }
    allocator->pfnFree(allocator->pUserData, surface);
}

VKAPI_ATTR
VkResult GetPhysicalDeviceSurfaceSupportKHR(VkPhysicalDevice, uint32_t, VkSurfaceKHR, VkBool32* supported)
{
    *supported = VK_TRUE;
    return VK_SUCCESS;
}

VKAPI_ATTR
VkResult GetPhysicalDeviceSurfaceCapabilitiesKHR(VkPhysicalDevice, VkSurfaceKHR surface,
                                                 VkSurfaceCapabilitiesKHR* capabilities)
{
    int width = 0;
    int height = 0;
    uint32_t maxBufferCount = 10;
    if (surface != VK_NULL_HANDLE) {
        NativeWindow* window = SurfaceFromHandle(surface)->window;
        int err = NativeWindowHandleOpt(window, GET_BUFFER_GEOMETRY, &height, &width);
        if (err != OHOS::GSERROR_OK) {
            WLOGE("NATIVE_WINDOW_DEFAULT_WIDTH query failed: (%{public}d)", err);
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


VKAPI_ATTR
VkResult GetPhysicalDeviceSurfaceFormatsKHR(VkPhysicalDevice pdev, VkSurfaceKHR surfaceHandle, uint32_t* count,
                                            VkSurfaceFormatKHR* formats)
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

VKAPI_ATTR
VkResult GetPhysicalDeviceSurfacePresentModesKHR(VkPhysicalDevice pdev, VkSurfaceKHR surface,  uint32_t* count,
    VkPresentModeKHR* modes)
{
    std::vector<VkPresentModeKHR> presentModes;
    presentModes.push_back(VK_PRESENT_MODE_MAILBOX_KHR);
    presentModes.push_back(VK_PRESENT_MODE_FIFO_KHR);

    VkPhysicalDevicePresentationPropertiesOpenHarmony present_properties;
    QueryPresentationProperties(pdev, &present_properties);
    if (present_properties.sharedImage) {
        presentModes.push_back(VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR);
        presentModes.push_back(VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR);
    }

    uint32_t numModes = uint32_t(presentModes.size());
    VkResult result = VK_SUCCESS;
    if (modes) {
        if (*count < numModes) {
            result = VK_INCOMPLETE;
        }
        *count = std::min(*count, numModes);
        std::copy_n(presentModes.data(), *count, modes);
    } else {
        *count = numModes;
    }
    return result;
}

VKAPI_ATTR
VkResult GetSwapchainImagesKHR(VkDevice, VkSwapchainKHR swapchainHandle, uint32_t* count, VkImage* images)
{
    const Swapchain& swapchain = *SwapchainFromHandle(swapchainHandle);
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
            WLOGE("unsupported swapchain format %{public}d", format);
            break;
    }
    return nativeFormat;
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
            return static_cast<GraphicColorDataSpace>(
                GRAPHIC_GAMUT_ADOBE_RGB | GRAPHIC_TRANSFORM_FUNC_LINEAR | GRAPHIC_PRECISION_FULL);
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
        vulkan::driver::DestroyImage(device, image.image, nullptr);
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

int TranslateVulkanToNativeTransform(VkSurfaceTransformFlagBitsKHR transform)
{
    return ROTATE_NONE;
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
        allocator = &vulkan::driver::GetDefaultAllocator();
    }

    swapchain->~Swapchain();
    allocator->pfnFree(allocator->pUserData, swapchain);
}

VKAPI_ATTR VkResult SetWindowInfo(const VkSwapchainCreateInfoKHR* createInfo, int32_t* numImages)
{
    GraphicPixelFormat pixelFormat = GetPixelFormat(createInfo->imageFormat);
    Surface& surface = *SurfaceFromHandle(createInfo->surface);

    NativeWindow* window = surface.window;
    int err = NativeWindowHandleOpt(window, SET_FORMAT, pixelFormat);
    if (err != OHOS::GSERROR_OK) {
        WLOGE("native_window_set_buffers_format(%{public}d) failed: (%{public}d)", pixelFormat, err);
        return VK_ERROR_SURFACE_LOST_KHR;
    }

    err = NativeWindowHandleOpt(window, SET_BUFFER_GEOMETRY,
                                static_cast<int>(createInfo->imageExtent.width),
                                static_cast<int>(createInfo->imageExtent.height));
    if (err != OHOS::GSERROR_OK) {
        WLOGE("native_window_set_buffers_dimensions(%{public}d,%{public}d) failed: (%{public}d)",
            createInfo->imageExtent.width, createInfo->imageExtent.height, err);
        return VK_ERROR_SURFACE_LOST_KHR;
    }

    if (createInfo->presentMode == VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR ||
        createInfo->presentMode == VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR) {
        *numImages = 1;
    }

    uint64_t native_usage = OHOS::BUFFER_USAGE_CPU_READ | OHOS::BUFFER_USAGE_CPU_WRITE | OHOS::BUFFER_USAGE_MEM_DMA;
    err = NativeWindowHandleOpt(window, SET_USAGE, native_usage);
    if (err != OHOS::GSERROR_OK) {
        WLOGE("native_window_set_buffer_count(%{public}d) failed: (%{public}d)", *numImages, err);
        return VK_ERROR_SURFACE_LOST_KHR;
    }
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult SetSwapchainCreateInfo(VkDevice device, const VkSwapchainCreateInfoKHR* createInfo,
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
    Surface& surface = *SurfaceFromHandle(createInfo->surface);
    NativeWindow* window = surface.window;
    if (createInfo->oldSwapchain != VK_NULL_HANDLE) {
        WLOGI("recreate swapchain ,clean buffer queue");
        window->surface->CleanCache();
    }
    VkResult result = VK_SUCCESS;
    for (int32_t i = 0; i < numImages; i++) {
        Swapchain::Image& img = swapchain->images[i];

        NativeWindowBuffer* buffer;
        int err = NativeWindowRequestBuffer(window, &buffer, &img.requestFence);
        if (err != OHOS::GSERROR_OK) {
            WLOGE("dequeueBuffer[%{public}u] failed: (%{public}d)", i, err);
            result = VK_ERROR_SURFACE_LOST_KHR;
            break;
        }
        img.buffer = buffer;
        img.requested = true;
        imageCreate.extent = VkExtent3D {static_cast<uint32_t>(img.buffer->sfbuffer->GetSurfaceBufferWidth()),
                                          static_cast<uint32_t>(img.buffer->sfbuffer->GetSurfaceBufferHeight()), 1};
        ((VkNativeBufferOpenHarmony*)(imageCreate.pNext))->handle = img.buffer->sfbuffer->GetBufferHandle();
        result = vulkan::driver::CreateImage(device, &imageCreate, nullptr, &img.image);
        if (result != VK_SUCCESS) {
            WLOGD("vkCreateImage native buffer failed: %{public}u", result);
            break;
        }
    }

    WLOGD("swapchain init shared %{public}d", swapchain->shared);
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

VKAPI_ATTR VkResult CreateSwapchainKHR(VkDevice device, const VkSwapchainCreateInfoKHR* createInfo,
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
        allocator = &vulkan::driver::GetDefaultAllocator();
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

VKAPI_ATTR
void DestroySwapchainKHR(VkDevice device, VkSwapchainKHR swapchainHandle,
                         const VkAllocationCallbacks* allocator)
{
    DestroySwapchainInternal(device, swapchainHandle, allocator);
}


VKAPI_ATTR VkResult AcquireNextImageKHR(VkDevice device, VkSwapchainKHR swapchainHandle, uint64_t timeout,
    VkSemaphore semaphore, VkFence vkFence, uint32_t* imageIndex)
{
    Swapchain& swapchain = *SwapchainFromHandle(swapchainHandle);
    NativeWindow* nativeWindow = swapchain.surface.window;
    VkResult result;

    if (swapchain.surface.swapchainHandle != swapchainHandle) {
        return VK_ERROR_OUT_OF_DATE_KHR;
    }

    if (swapchain.shared) {
        *imageIndex = 0;
        result = vulkan::driver::SetNativeFenceFdOpenHarmony(device, -1, semaphore, vkFence);
        return result;
    }

    NativeWindowBuffer* nativeWindowBuffer;
    int fence;
    int32_t ret = NativeWindowRequestBuffer(nativeWindow, &nativeWindowBuffer, &fence);
    if (ret != OHOS::GSERROR_OK) {
        WLOGE("dequeueBuffer failed: (%{public}d)", ret);
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
        WLOGE("dequeueBuffer returned unrecognized buffer");
        if (NativeWindowCancelBuffer(nativeWindow, nativeWindowBuffer) != OHOS::GSERROR_OK) {
            WLOGE("NativeWindowCancelBuffer failed: (%{public}d)", ret);
        }
        return VK_ERROR_OUT_OF_DATE_KHR;
    }

    result = vulkan::driver::SetNativeFenceFdOpenHarmony(device, -1, semaphore, vkFence);
    if (result != VK_SUCCESS) {
        if (NativeWindowCancelBuffer(nativeWindow, nativeWindowBuffer) != OHOS::GSERROR_OK) {
            WLOGE("NativeWindowCancelBuffer failed: (%{public}d)", ret);
        }
        swapchain.images[index].requested = false;
        swapchain.images[index].requestFence = -1;
        return result;
    }

    *imageIndex = index;
    return VK_SUCCESS;
}

VKAPI_ATTR
VkResult AcquireNextImage2KHR(VkDevice device, const VkAcquireNextImageInfoKHR* pAcquireInfo, uint32_t* pImageIndex)
{
    return AcquireNextImageKHR(device, pAcquireInfo->swapchain, pAcquireInfo->timeout,
                               pAcquireInfo->semaphore, pAcquireInfo->fence, pImageIndex);
}

const VkPresentRegionKHR* GetPresentRegion(
    const VkPresentRegionKHR* regions, const Swapchain& swapchain, uint32_t index)
{
    return (regions && !swapchain.mailboxMode) ? &regions[index] : nullptr;
}

VkResult GetReleaseFence(VkQueue queue, const VkPresentInfoKHR* presentInfo,
    Swapchain::Image& img, int32_t &fence)
{
    VkResult result = vulkan::driver::GetNativeFenceFdOpenHarmony(queue, presentInfo->waitSemaphoreCount,
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

VkResult FlushBuffer(const VkPresentRegionKHR* region, struct Region::Rect* rects,
    Swapchain& swapchain, Swapchain::Image& img, int32_t fence)
{
    const VkAllocationCallbacks* defaultAllocator = &vulkan::driver::GetDefaultAllocator();
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
        WLOGE("queueBuffer failed: (%{public}d)", err);
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

VKAPI_ATTR VkResult QueuePresentKHR(VkQueue queue, const VkPresentInfoKHR* presentInfo)
{
    VkResult ret = VK_SUCCESS;

    const VkPresentRegionKHR* regions = GetPresentRegions(presentInfo);
    const VkAllocationCallbacks* defaultAllocator = &vulkan::driver::GetDefaultAllocator();
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
            WLOGE("QueuePresentKHR swapchainHandle != pSwapchains[%{public}d]", i);
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
}  // namespace driver
}  // namespace vulkan
