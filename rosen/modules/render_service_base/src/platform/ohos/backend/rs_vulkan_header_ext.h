/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef RS_VULKAN_HEADER_EXT_H
#define RS_VULKAN_HEADER_EXT_H

#ifdef __cplusplus
extern "C" {
#endif

namespace OHOS {
namespace Rosen {

typedef enum VkStructureTypeHUAWEI {
    VK_STRUCTURE_TYPE_DEVICE_MEMORY_EXCLUSIVE_THRESHOLD_INFO = VK_STRUCTURE_TYPE_MAX_ENUM - 37,
} VkstructureTypeHUAWEI;

typedef struct VkDeviceMemoryExclusiveThresholdHUAWEI {
    VkStructureTypeHUAWEI sType;
    const void*           pNext;
    uint32_t              threshold;
} VkDeviceMemoryExclusiveThresholdHUAWEI;

}
}

#ifdef __cplusplus
}
#endif

#endif