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
#ifndef LIBVULKAN_DRIVER_LOADER_H
#define LIBVULKAN_DRIVER_LOADER_H

#include <string>

#include "hardware/hdi_vulkan.h"
namespace vulkan {
namespace driver {
typedef int32_t (*PFN_VulkanInitialize)(VulkanFuncs **funcs);
typedef int32_t (*PFN_VulkanUnInitialize)(VulkanFuncs *funcs);
class DriverLoader {
public:
static bool Load();
static bool Unload();
static bool LoadDriver();
static bool UnloadDriver();
static bool LoadDriverFromFile(std::string path);

static const DriverLoader& Get()
{
    return loader_;
}

static const VulkanFuncs& GetVulkanFuncs()
{
    return *Get().vulkanFuncs_;
}

static bool IsSupportedVulkan()
{
    return Get().supported_;
}

private:
DriverLoader() : vulkanFuncs_(nullptr), vulkanUnInitializeFunc_(nullptr) {}
DriverLoader(const DriverLoader&) = delete;
DriverLoader& operator=(const DriverLoader&) = delete;

static DriverLoader loader_;
void* handle_;

bool supported_;
VulkanFuncs* vulkanFuncs_;
PFN_VulkanUnInitialize vulkanUnInitializeFunc_;
};
}  // namespace driver
}  // namespace vulkan
#endif // LIBVULKAN_DRIVER_LOADER_H
