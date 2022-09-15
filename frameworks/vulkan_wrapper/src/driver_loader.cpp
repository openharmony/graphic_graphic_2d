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
#include "driver_loader.h"
#include <dlfcn.h>
#include <iostream>
#include "wrapper_log.h"
#include "directory_ex.h"

namespace vulkan {
namespace driver {
#ifdef __aarch64__
constexpr const char *VENDOR_LIB_PATH = "/vendor/lib64/chipsetsdk/";
constexpr const char *SYSTEM_LIB_PATH = "/system/lib64/";
#else
constexpr const char *VENDOR_LIB_PATH = "/vendor/lib/chipsetsdk/";
constexpr const char *SYSTEM_LIB_PATH = "/system/lib/";
#endif
constexpr const char *LIB_NAME = "libEGL_impl.so";
constexpr const char *HDI_VULKAN_MODULE_INIT = "VulkanInitialize";
constexpr const char *HDI_VULKAN_MODULE_UNINIT = "VulkanUnInitialize";

bool DriverLoader::Load()
{
    if (loader_.vulkanFuncs_ != nullptr) {
        return true;
    }

    std::string path = std::string(VENDOR_LIB_PATH) + std::string(LIB_NAME);
    loader_.handle_ = dlopen(path.c_str(), RTLD_LOCAL | RTLD_NOW);

    if (loader_.handle_ == nullptr) {
        path = std::string(SYSTEM_LIB_PATH) + std::string(LIB_NAME);
        std::string realPath;
        if (!OHOS::PathToRealPath(path, realPath)) {
            WLOGE("file is not real path, file path: %{private}s", path.c_str());
            return false;
        }
        loader_.handle_ = dlopen(realPath.c_str(), RTLD_NOW | RTLD_LOCAL);
        if (loader_.handle_ == nullptr) {
            WLOGE("dlopen failed. error: %{public}s.", dlerror());
            return false;
        }
    }
    WLOGD("DriverLoader::dlopen success");

    PFN_VulkanInitialize VulkanInitializeFunc
        = reinterpret_cast<PFN_VulkanInitialize>(dlsym(loader_.handle_, HDI_VULKAN_MODULE_INIT));
    if (VulkanInitializeFunc == nullptr) {
        WLOGE("DriverLoader:: couldn't find symbol(VulkanInitializeFunc) in library : %{public}s.", dlerror());
        dlclose(loader_.handle_);
        return false;
    }

    if (VulkanInitializeFunc(&loader_.vulkanFuncs_) != 0) {
        WLOGE("DriverLoader:: Initialize Vulkan Func fail");
        return false;
    }

    loader_.vulkanUnInitializeFunc_ = reinterpret_cast<PFN_VulkanUnInitialize>(
        dlsym(loader_.handle_, HDI_VULKAN_MODULE_UNINIT));

    if (loader_.vulkanUnInitializeFunc_ == nullptr) {
        WLOGE("DriverLoader:: couldn't find symbol(VulkanUnInitialize) in library : %{public}s.", dlerror());
        dlclose(loader_.handle_);
        return false;
    }
    WLOGI("DriverLoader:: Found Vulkan Func success");
    return true;
}

bool DriverLoader::Unload()
{
    if (!loader_.vulkanUnInitializeFunc_) {
        WLOGE("DriverLoader::Unload can not find vulkan UnInitialize Func ");
        return false;
    }
    if (loader_.vulkanUnInitializeFunc_(loader_.vulkanFuncs_) != 0) {
        WLOGE("DriverLoader::Unload vulkan UnInitialize Func success");
        return false;
    }
    dlclose(loader_.handle_);
    return true;
}
}  // namespace driver
}  // namespace vulkan
