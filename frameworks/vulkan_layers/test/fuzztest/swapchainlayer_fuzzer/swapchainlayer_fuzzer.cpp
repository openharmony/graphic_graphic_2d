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

#include "swapchainlayer_fuzzer.h"

#include <securec.h>

#include <string>
#include <vector>
#include <iostream>
#include <dlfcn.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_ohos.h>

namespace OHOS {
    namespace {
        void* g_libVulkan = nullptr;
        VkInstance instance = nullptr;
        PFN_vkCreateInstance vkCreateInstance = nullptr;
        PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = nullptr;
        PFN_vkDestroyInstance vkDestroyInstance = nullptr;
        PFN_vkCreateSurfaceOHOS vkCreateSurfaceOHOS = nullptr;
        PFN_vkDestroySurfaceKHR vkDestroySurfaceKHR = nullptr;
        bool g_isSupportedVulkan = false;
        const uint8_t* g_data = nullptr;
        size_t g_size = 0;
        size_t g_pos = 0;
    }

    /*
    * describe: get data from outside untrusted data(g_data) which size is according to sizeof(T)
    * tips: only support basic type
    */
    template<class T>
    T GetData()
    {
        T object {};
        size_t objectSize = sizeof(object);
        if (g_data == nullptr || objectSize > g_size - g_pos) {
            return object;
        }
        errno_t ret = memcpy_s(&object, objectSize, g_data + g_pos, objectSize);
        if (ret != EOK) {
            return {};
        }
        g_pos += objectSize;
        return object;
    }

    void DLOpenLibVulkan()
    {
        static bool firstComeIn = true;
        if (firstComeIn) {
#if (defined(__aarch64__) || defined(__x86_64__))
            const char *path = "/system/lib64/platformsdk/libvulkan.so";
#else
            const char *path = "/system/lib/platformsdk/libvulkan.so";
#endif
            g_libVulkan = dlopen(path, RTLD_NOW | RTLD_LOCAL);
            if (g_libVulkan == nullptr) {
                std::cout << "dlerror: " << dlerror() << std::endl;
                g_isSupportedVulkan = false;
            } else {
                g_isSupportedVulkan = true;
            }
            firstComeIn = false;
        }
    }

    void CreateVkInstance()
    {
        static bool firstComeIn = true;
        if (firstComeIn) {
            vkCreateInstance = reinterpret_cast<PFN_vkCreateInstance>(dlsym(g_libVulkan, "vkCreateInstance"));
            vkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(
                dlsym(g_libVulkan, "vkGetInstanceProcAddr"));
            VkApplicationInfo appInfo = {};
            appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            appInfo.pApplicationName = "pApplicationName";
            appInfo.pEngineName = "pEngineName";
            appInfo.apiVersion = VK_API_VERSION_1_0;

            std::vector<const char*> instanceExtensions = {
                VK_KHR_SURFACE_EXTENSION_NAME,
                VK_OHOS_SURFACE_EXTENSION_NAME
            };

            VkInstanceCreateInfo instanceCreateInfo = {};
            instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            instanceCreateInfo.pNext = NULL;
            instanceCreateInfo.pApplicationInfo = &appInfo;

            instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
            instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.data();

            VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &instance);
            if (result == VK_ERROR_INCOMPATIBLE_DRIVER) {
                g_isSupportedVulkan = false;
            } else {
                g_isSupportedVulkan = true;
                vkDestroyInstance = reinterpret_cast<PFN_vkDestroyInstance>(
                    vkGetInstanceProcAddr(instance, "vkDestroyInstance"));
                vkCreateSurfaceOHOS = reinterpret_cast<PFN_vkCreateSurfaceOHOS>(
                    vkGetInstanceProcAddr(instance, "vkCreateSurfaceOHOS"));
                vkDestroySurfaceKHR = reinterpret_cast<PFN_vkDestroySurfaceKHR>(
                    vkGetInstanceProcAddr(instance, "vkDestroySurfaceKHR"));
            }
            firstComeIn = false;
        }
    }

    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        g_data = data;
        g_size = size;
        g_pos = 0;

        // test
        DLOpenLibVulkan();
        if (g_isSupportedVulkan) {
            CreateVkInstance();
        }
        if (g_isSupportedVulkan) {
            VkSurfaceCreateInfoOHOS surfaceCreateInfo = {};
            surfaceCreateInfo.sType = GetData<VkStructureType>();
            surfaceCreateInfo.flags = GetData<VkSurfaceCreateFlagsOHOS>();
            VkSurfaceKHR surface;
            vkCreateSurfaceOHOS(instance, &surfaceCreateInfo, NULL, &surface);
            vkDestroySurfaceKHR(instance, surface, nullptr);
        }

        return true;
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}

