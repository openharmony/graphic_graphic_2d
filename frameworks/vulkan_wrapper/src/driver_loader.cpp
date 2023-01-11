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
#include <mutex>
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
    static std::mutex loadLock;
    std::lock_guard<std::mutex> lock(loadLock);

    if (loader_.vulkanFuncs_ != nullptr) {
        return true;
    }
    loader_.supported_ = false;

    if (!LoadDriver()) {
        WLOGE("LoadDriver failed");
        return false;
    }

    PFN_VulkanInitialize VulkanInitializeFunc
        = reinterpret_cast<PFN_VulkanInitialize>(dlsym(loader_.handle_, HDI_VULKAN_MODULE_INIT));
    if (VulkanInitializeFunc == nullptr) {
        WLOGE("DriverLoader:: couldn't find symbol(VulkanInitializeFunc) in library : %{public}s.", dlerror());
        UnloadDriver();
        return false;
    }

    if (VulkanInitializeFunc(&loader_.vulkanFuncs_) != 0) {
        WLOGE("DriverLoader:: Initialize Vulkan Func fail");
        UnloadDriver();
        return false;
    }

    loader_.vulkanUnInitializeFunc_ = reinterpret_cast<PFN_VulkanUnInitialize>(
        dlsym(loader_.handle_, HDI_VULKAN_MODULE_UNINIT));

    if (loader_.vulkanUnInitializeFunc_ == nullptr) {
        WLOGE("DriverLoader:: couldn't find symbol(VulkanUnInitialize) in library : %{public}s.", dlerror());
        UnloadDriver();
        return false;
    }
    loader_.supported_ = true;
    WLOGI("DriverLoader:: Found Vulkan Func success");
    return true;
}

bool DriverLoader::Unload()
{
    if (loader_.vulkanUnInitializeFunc_ == nullptr) {
        WLOGE("DriverLoader::Unload can not find vulkan UnInitialize Func.");
        return false;
    }
    if (loader_.vulkanUnInitializeFunc_(loader_.vulkanFuncs_) != 0) {
        WLOGE("DriverLoader::Unload vulkan UnInitialize Func failed.");
        return false;
    }
    loader_.vulkanFuncs_ = nullptr;
    loader_.vulkanUnInitializeFunc_ = nullptr;
    WLOGD("DriverLoader::Unload vulkan UnInitialize Func successfully.");
    return UnloadDriver();
}

bool DriverLoader::LoadDriver()
{
    if (loader_.handle_ != nullptr) {
        WLOGD("handle is not null, no need to dlopen.");
        return true;
    }
    std::string venderPath = std::string(VENDOR_LIB_PATH) + std::string(LIB_NAME);
    std::string systemPath = std::string(SYSTEM_LIB_PATH) + std::string(LIB_NAME);
    if (!LoadDriverFromFile(venderPath)) {
        if (!LoadDriverFromFile(systemPath)) {
            return false;
        }
    }
    WLOGD("DriverLoader::LoadDriver successfully.");
    return true;
}

bool DriverLoader::UnloadDriver()
{
    if (loader_.handle_ == nullptr) {
        WLOGD("DriverLoader::UnloadDriver handle is null already.");
        return true;
    }
    if (dlclose(loader_.handle_) != 0) {
        WLOGE("DriverLoader::UnloadDriver dlclose failed.");
        return false;
    }
    loader_.handle_ = nullptr;
    WLOGD("DriverLoader::UnloadDriver dlclose success.");
    return true;
}

bool DriverLoader::LoadDriverFromFile(std::string path)
{
    std::string realPath;
    if (!OHOS::PathToRealPath(path, realPath)) {
        WLOGE("file is not in path: %{private}s", path.c_str());
        return false;
    }
    loader_.handle_ = dlopen(realPath.c_str(), RTLD_LOCAL | RTLD_NOW);
    if (loader_.handle_ == nullptr) {
        WLOGE("dlopen failed. error: %{public}s.", dlerror());
        return false;
    }
    return true;
}
}  // namespace driver
}  // namespace vulkan
