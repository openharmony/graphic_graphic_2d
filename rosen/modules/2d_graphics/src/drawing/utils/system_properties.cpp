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

#ifndef ROSEN_ARKUI_X
#include <parameter.h>
#include <parameters.h>
#endif
#include "utils/system_properties.h"
#include "platform/common/rs_log.h"
#if (defined(ROSEN_ARKUI_X) && defined(RS_ENABLE_VK))
#include <dlfcn.h>
#include <vector>
#include "include/third_party/vulkan/vulkan/vulkan_core.h"
#endif

namespace OHOS {
namespace Rosen {
namespace Drawing {

#if (defined(ROSEN_ARKUI_X) && defined(RS_ENABLE_VK))
const int VULKAN_MINOR_VERSION = 3;
// load vulkan library and check if Vulkan version is >= VULKAN_MINOR_VERSION
struct VulkanLoader {
    void* handle = nullptr;
    PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = nullptr;
    ~VulkanLoader()
    {
        if (handle != nullptr) {
            dlclose(handle);
        }
    }
};
#endif
#ifndef ROSEN_ARKUI_X
bool SystemProperties::GetHMSymbolEnable()
{
    static bool isHMSymbolEnable =
        (std::atoi(system::GetParameter("persist.sys.graphic.hmsymbolenable", "1").c_str()) != 0);
    return isHMSymbolEnable;
}
#endif
#ifdef ROSEN_ARKUI_X
const GpuApiType SystemProperties::systemGpuApiType_ =
 	     SystemProperties::CheckVulkanEnable() ? GpuApiType::VULKAN : GpuApiType::OPENGL;
#else
#if (defined (ACE_ENABLE_GL) && defined (ACE_ENABLE_VK)) || (defined (RS_ENABLE_GL) && defined (RS_ENABLE_VK))
const GpuApiType SystemProperties::systemGpuApiType_ = SystemProperties::GetSystemGraphicGpuType();
#elif defined (ACE_ENABLE_GL) || defined (RS_ENABLE_GL)
const GpuApiType SystemProperties::systemGpuApiType_ = GpuApiType::OPENGL;
#else
const GpuApiType SystemProperties::systemGpuApiType_ = GpuApiType::VULKAN;
#endif
#endif

#ifdef ROSEN_OHOS
bool SystemProperties::IsVkImageDfxEnabled()
{
    static const bool dfxEnabled =
        std::atoi(OHOS::system::GetParameter("persist.sys.graphic.openVkImageMemoryDfx", "0").c_str()) != 0;
    return dfxEnabled;
}

bool SystemProperties::IsDebugGpuMem()
{
    static bool isDebugEnabled =
        (std::atoi(OHOS::system::GetParameter("persist.sys.graphic.debug.gpumem", "0").c_str()) != 0);
    return isDebugEnabled;
}
#endif

#ifdef ROSEN_ARKUI_X
#ifdef RS_ENABLE_VK
// check if Vulkan version is >= 1.3
static bool LoadVulkanLibrary(VulkanLoader& loader)
{
    loader.handle = dlopen("libvulkan.so", RTLD_NOW | RTLD_LOCAL);
    if (loader.handle == nullptr) {
        ROSEN_LOGE("Vulkan library not found, using GL");
        return false;
    }

    loader.vkGetInstanceProcAddr =
        reinterpret_cast<PFN_vkGetInstanceProcAddr>(dlsym(loader.handle, "vkGetInstanceProcAddr"));
    if (loader.vkGetInstanceProcAddr == nullptr) {
        ROSEN_LOGE("vkGetInstanceProcAddr not found, using GL");
        return false;
    }
    return true;
}

// create a temporary Vulkan instance for version check
static bool CreateTemporaryVulkanInstance(PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr, VkInstance& instance)
{
    PFN_vkCreateInstance vkCreateInstance =
        reinterpret_cast<PFN_vkCreateInstance>(vkGetInstanceProcAddr(VK_NULL_HANDLE, "vkCreateInstance"));
    if (vkCreateInstance == nullptr) {
        ROSEN_LOGE("vkCreateInstance not found, using GL");
        return false;
    }

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "VersionCheck";
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
    if (result != VK_SUCCESS) {
        ROSEN_LOGE("Failed to create Vulkan instance for version check, using GL");
        return false;
    }
    return true;
}

// get Vulkan functions for physical device
struct PhysicalDeviceFunctions {
    PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices = nullptr;
    PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties = nullptr;
    PFN_vkDestroyInstance vkDestroyInstance = nullptr;
};

// get Vulkan functions for physical device
static bool GetPhysicalDeviceFunctions(
    PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr, VkInstance instance, PhysicalDeviceFunctions& funcs)
{
    funcs.vkEnumeratePhysicalDevices =
        reinterpret_cast<PFN_vkEnumeratePhysicalDevices>(vkGetInstanceProcAddr(instance, "vkEnumeratePhysicalDevices"));
    funcs.vkGetPhysicalDeviceProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties>(
        vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceProperties"));
    funcs.vkDestroyInstance =
        reinterpret_cast<PFN_vkDestroyInstance>(vkGetInstanceProcAddr(instance, "vkDestroyInstance"));

    if (funcs.vkEnumeratePhysicalDevices == nullptr || funcs.vkGetPhysicalDeviceProperties == nullptr ||
        funcs.vkDestroyInstance == nullptr) {
        ROSEN_LOGE("Failed to get Vulkan functions for version check, using GL");
        return false;
    }
    return true;
}

// enumerate Vulkan physical devices and check version
static bool EnumeratePhysicalDevices(
    const PhysicalDeviceFunctions& funcs, VkInstance instance, std::vector<VkPhysicalDevice>& devices)
{
    uint32_t deviceCount = 0;
    if (funcs.vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr) != VK_SUCCESS || deviceCount == 0) {
        ROSEN_LOGE("No Vulkan physical devices found, using GL");
        return false;
    }

    devices.resize(deviceCount);
    if (funcs.vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data()) != VK_SUCCESS) {
        ROSEN_LOGE("Failed to enumerate Vulkan physical devices, using GL");
        return false;
    }
    return true;
}

// check if Vulkan version is >= VULKAN_MINOR_VERSION
static bool CheckVersionGreaterThan13(const VkPhysicalDeviceProperties& props)
{
    uint32_t major = VK_VERSION_MAJOR(props.apiVersion);
    uint32_t minor = VK_VERSION_MINOR(props.apiVersion);

    ROSEN_LOGI("Vulkan version detected: %u.%u", major, minor);

    bool isVersion13OrHigher = (major > 1) || (major == 1 && minor >= VULKAN_MINOR_VERSION);
    if (!isVersion13OrHigher) {
        ROSEN_LOGE("Vulkan version %u.%u is below 1.3, using GL", major, minor);
        return false;
    }

    ROSEN_LOGI("Vulkan version %u.%u is >= 1.3, using Vulkan", major, minor);
    return true;
}
#endif

bool SystemProperties::CheckVulkanEnable()
{
#ifndef RS_ENABLE_VK
    return false;
#else
    VulkanLoader loader;
    if (!LoadVulkanLibrary(loader)) {
        return false;
    }

    VkInstance instance = VK_NULL_HANDLE;
    if (!CreateTemporaryVulkanInstance(loader.vkGetInstanceProcAddr, instance)) {
        return false;
    }

    PhysicalDeviceFunctions funcs;
    if (!GetPhysicalDeviceFunctions(loader.vkGetInstanceProcAddr, instance, funcs)) {
        funcs.vkDestroyInstance(instance, nullptr);
        return false;
    }

    std::vector<VkPhysicalDevice> devices;
    if (!EnumeratePhysicalDevices(funcs, instance, devices)) {
        funcs.vkDestroyInstance(instance, nullptr);
        return false;
    }

    VkPhysicalDeviceProperties props = {};
    funcs.vkGetPhysicalDeviceProperties(devices[0], &props);

    funcs.vkDestroyInstance(instance, nullptr);

    return CheckVersionGreaterThan13(props);
#endif
}
#endif
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS