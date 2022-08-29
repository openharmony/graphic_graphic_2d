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
#ifndef __VK_OHOS_NATIVE_BUFFER_H__
#define __VK_OHOS_NATIVE_BUFFER_H__

#include <buffer_handle.h>
#include <vulkan/vulkan.h>

#ifdef __cplusplus
extern "C" {
#endif

#define VK_OHOS_native_buffer 1

#define VK_OHOS_NATIVE_BUFFER_SPEC_VERSION 1
#define VK_OHOS_NATIVE_BUFFER_EXTENSION_NAME "VK_OHOS_native_buffer"

typedef VkFlags VkSwapchainImageUsageFlagsOpenHarmony;
typedef enum VkSwapchainImageUsageFlagBitsOpenHarmony {
    VK_SWAPCHAIN_IMAGE_USAGE_SHARED_BIT_OPENHARMONY = 0x00000001,
    VK_SWAPCHAIN_IMAGE_USAGE_FLAG_BITS_MAX_ENUM = 0x7FFFFFFF
} VkSwapchainImageUsageFlagBitsOHOS;

typedef struct {
    VkStructureType                   sType;
    const void*                       pNext;
    BufferHandle*                     handle;
} VkNativeBufferOpenHarmony;

typedef struct {
    VkStructureType                   sType;
    const void*                       pNext;
    VkSwapchainImageUsageFlagsOpenHarmony    usage;
} VkSwapchainImageCreateInfoOpenHarmony;

typedef struct {
    VkStructureType                   sType;
    const void*                       pNext;
    VkBool32                          sharedImage;
} VkPhysicalDevicePresentationPropertiesOpenHarmony;

typedef VkResult (VKAPI_PTR *PFN_vkGetSwapchainGrallocUsageOpenHarmony)(
    VkDevice                          device,
    VkFormat                          format,
    VkImageUsageFlags                 imageUsage,
    uint64_t*                         grallocUsage);

typedef VkResult (VKAPI_PTR *PFN_vkSetNativeFenceFdOpenHarmony)(
    VkDevice                          device,
    int32_t                           nativeFenceFd,
    VkSemaphore                       semaphore,
    VkFence                           fence);

typedef VkResult (VKAPI_PTR *PFN_vkGetNativeFenceFdOpenHarmony)(
    VkQueue                           queue,
    uint32_t                          waitSemaphoreCount,
    const VkSemaphore*                pWaitSemaphores,
    VkImage                           image,
    int32_t*                          pNativeFenceFd);

#ifndef VK_NO_PROTOTYPES

VKAPI_ATTR VkResult VKAPI_CALL vkGetSwapchainGrallocUsageOpenHarmony(
    VkDevice                          device,
    VkFormat                          format,
    VkImageUsageFlags                 imageUsage,
    uint64_t*                         grallocUsage
);

VKAPI_ATTR VkResult VKAPI_CALL vkSetNativeFenceFdOpenHarmony(
    VkDevice                          device,
    int32_t                           nativeFenceFd,
    VkSemaphore                       semaphore,
    VkFence                           fence
);

VKAPI_ATTR VkResult VKAPI_CALL vkGetNativeFenceFdOpenHarmony(
    VkQueue                           queue,
    uint32_t                          waitSemaphoreCount,
    const VkSemaphore*                pWaitSemaphores,
    VkImage                           image,
    int32_t*                          pNativeFenceFd
);

#endif

#ifdef __cplusplus
}
#endif

#endif /* __VK_OHOS_NATIVE_BUFFER_H__ */
