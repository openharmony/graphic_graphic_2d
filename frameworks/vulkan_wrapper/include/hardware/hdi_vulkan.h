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

/**
 * @addtogroup vulkan
 * @{
 *
 * @brief Defines driver functions of the vulkan moudle.
 *
 * @since 1.0
 * @version 2.0
 */

/**
* @file HDI_VULKAN_H.h
*
* @brief Declares the driver functions for vulkan.
*
* @since 1.0
* @version 2.0
*/

#ifndef HDI_VULKAN_H
#define HDI_VULKAN_H

#include "vulkan/vulkan.h"

#ifdef __cplusplus
extern "C" {
#endif
#define HDIVULKAN_DISPATCH_MAGIC 0x01CDC0DE

typedef struct {
    VkResult (VKAPI_PTR *PFN_vkEnumerateInstanceExtensionProperties)(const char* pLayerName,
        uint32_t* pPropertyCount, VkExtensionProperties* pProperties);
    VkResult (VKAPI_PTR *PFN_vkCreateInstance)(const VkInstanceCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator, VkInstance* pInstance);
    PFN_vkVoidFunction (VKAPI_PTR *PFN_vkGetInstanceProcAddr)(VkInstance instance, const char* pName);
} VulkanFuncs;

/**
 * @brief Initialize the vulkan module to obtain the pointer to functions for memory operation.
 *
 * @param funcs Indicates the double pointer to functions for vulkan
 *
 * @return Returns <b>0</b> if the operation is successful.
 * otherwise.
 * @since 1.0
 * @version 1.0
 */
__attribute__((visibility("default"))) int32_t VulkanInitialize(VulkanFuncs **funcs);

/**
 * @brief Deinitialize the functions module to release the resource of vulkan.
 *
 * @param funcs Indicates the pointer to functions for vulkan.
 *
 * @return Returns <b>0</b> if the operation is successful.
 * @since 1.0
 * @version 1.0
 */
__attribute__((visibility("default"))) int32_t VulkanUnInitialize(VulkanFuncs *funcs);

#ifdef __cplusplus
}
#endif
#endif
/** @} */
