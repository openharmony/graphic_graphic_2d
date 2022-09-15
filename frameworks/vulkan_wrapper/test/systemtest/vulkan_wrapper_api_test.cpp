/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include <chrono>
#include <thread>
#include <unistd.h>
#include <window.h>
#include <gtest/gtest.h>
#include <dlfcn.h>
#include <iostream>
#include <string>
#include <vector>

#include "refbase.h"
#include "surface.h"
#include "vulkan/vulkan.h"
#include "render_context/render_context.h"
#include "transaction/rs_transaction.h"
#include "ui/rs_surface_extractor.h"
#include "ui/rs_surface_node.h"
#include "wm/window.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

typedef bool (*PFN_IsSupportedVulkan)();

class VulkanWrapperApiTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase()
    {
        if (libVulkan != nullptr) {
            dlclose(libVulkan);
            libVulkan = nullptr;
        }
    }

    static inline PFN_vkCreateInstance vkCreateInstance;
    static inline PFN_vkDestroySurfaceKHR vkDestroySurfaceKHR;
    static inline PFN_vkCreateDevice vkCreateDevice;
    static inline PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties;
    static inline PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr;
    static inline PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr;
    static inline PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices;
    static inline PFN_vkCreateOHOSSurfaceOpenHarmony vkCreateOHOSSurfaceOpenHarmony;
    static inline PFN_vkGetPhysicalDeviceSurfaceSupportKHR fpGetPhysicalDeviceSurfaceSupportKHR;
    static inline PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR fpGetPhysicalDeviceSurfaceCapabilitiesKHR;
    static inline PFN_vkGetPhysicalDeviceSurfaceFormatsKHR fpGetPhysicalDeviceSurfaceFormatsKHR;
    static inline PFN_vkGetPhysicalDeviceSurfacePresentModesKHR fpGetPhysicalDeviceSurfacePresentModesKHR;
    static inline PFN_vkCreateSwapchainKHR fpCreateSwapchainKHR;
    static inline PFN_vkDestroySwapchainKHR fpDestroySwapchainKHR;
    static inline PFN_vkGetSwapchainImagesKHR fpGetSwapchainImagesKHR;
    static inline PFN_vkAcquireNextImageKHR fpAcquireNextImageKHR;
    static inline PFN_vkQueuePresentKHR fpQueuePresentKHR;
    static inline PFN_IsSupportedVulkan fpIsSupportedVulkan;

    static inline void *libVulkan = nullptr;
    static inline VkInstance instance = nullptr;
    static inline VkSurfaceKHR surface = VK_NULL_HANDLE;
    static inline VkPhysicalDevice physicalDevice = nullptr;
    static inline VkDevice device = nullptr;
    static inline VkSwapchainKHR swapChain = VK_NULL_HANDLE;
    static inline bool isSupportedVulkan = false;
};

/**
 * @tc.name: dlopen libvulkan.so
 * @tc.desc: dlopen libvulkan.so
 * @tc.type: FUNC
 * @tc.require: issueI5ODXM
 */
HWTEST_F(VulkanWrapperApiTest, dlopen_Test, TestSize.Level1)
{
#ifdef __aarch64__
    const char *path = "/system/lib64/libvulkan.so";
#else
    const char *path = "/system/lib/libvulkan.so";
#endif
    libVulkan = dlopen(path, RTLD_NOW | RTLD_LOCAL);
    EXPECT_NE(libVulkan, nullptr);
}

/**
 * @tc.name: Load base function pointers
 * @tc.desc: Load base function pointers
 * @tc.type: FUNC
 * @tc.require: issueI5ODXM
 */
HWTEST_F(VulkanWrapperApiTest, LoadFuncPtr001, TestSize.Level1)
{
    vkEnumerateInstanceExtensionProperties = reinterpret_cast<PFN_vkEnumerateInstanceExtensionProperties>(
        dlsym(libVulkan, "vkEnumerateInstanceExtensionProperties"));
    EXPECT_NE(vkEnumerateInstanceExtensionProperties, nullptr);
    vkCreateInstance = reinterpret_cast<PFN_vkCreateInstance>(dlsym(libVulkan, "vkCreateInstance"));
    EXPECT_NE(vkCreateInstance, nullptr);
    vkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(dlsym(libVulkan, "vkGetInstanceProcAddr"));
    EXPECT_NE(vkGetInstanceProcAddr, nullptr);
    vkGetDeviceProcAddr = reinterpret_cast<PFN_vkGetDeviceProcAddr>(dlsym(libVulkan, "vkGetDeviceProcAddr"));
    EXPECT_NE(vkGetDeviceProcAddr, nullptr);
    fpIsSupportedVulkan = reinterpret_cast<PFN_IsSupportedVulkan>(dlsym(libVulkan, "IsSupportedVulkan"));
    EXPECT_NE(fpIsSupportedVulkan, nullptr);
    isSupportedVulkan = fpIsSupportedVulkan();
    std::cout << "support vulkan :" << isSupportedVulkan << std::endl;
}

/**
 * @tc.name: create vkInstance
 * @tc.desc: create vkInstance
 * @tc.type: FUNC
 * @tc.require: issueI5ODXM
 */
HWTEST_F(VulkanWrapperApiTest, createVkInstance_Test, TestSize.Level1)
{
    if (isSupportedVulkan) {
        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "pApplicationName";
        appInfo.pEngineName = "pEngineName";
        appInfo.apiVersion = VK_API_VERSION_1_0;

        std::vector<const char*> instanceExtensions = { VK_KHR_SURFACE_EXTENSION_NAME };
        instanceExtensions.push_back(VK_OPENHARMONY_OHOS_SURFACE_EXTENSION_NAME);

        VkInstanceCreateInfo instanceCreateInfo = {};
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pNext = NULL;
        instanceCreateInfo.pApplicationInfo = &appInfo;

        if (instanceExtensions.size() > 0) {
            instanceCreateInfo.enabledExtensionCount = (uint32_t)instanceExtensions.size();
            instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.data();
        }

        VkResult err = vkCreateInstance(&instanceCreateInfo, nullptr, &instance);
        EXPECT_EQ(err, VK_SUCCESS);
        EXPECT_NE(instance, nullptr);
    }
}

/**
 * @tc.name: Load instance based Vulkan function pointers
 * @tc.desc: Load instance based Vulkan function pointers
 * @tc.type: FUNC
 * @tc.require: issueI5ODXM
 */
HWTEST_F(VulkanWrapperApiTest, LoadFuncPtr002, TestSize.Level1)
{
    if (isSupportedVulkan) {
        vkEnumeratePhysicalDevices = reinterpret_cast<PFN_vkEnumeratePhysicalDevices>(
            vkGetInstanceProcAddr(instance, "vkEnumeratePhysicalDevices"));
        EXPECT_NE(vkEnumeratePhysicalDevices, nullptr);
        vkCreateDevice = reinterpret_cast<PFN_vkCreateDevice>(
            vkGetInstanceProcAddr(instance, "vkCreateDevice"));
        EXPECT_NE(vkCreateDevice, nullptr);
        vkCreateOHOSSurfaceOpenHarmony = reinterpret_cast<PFN_vkCreateOHOSSurfaceOpenHarmony>(
            vkGetInstanceProcAddr(instance, "vkCreateOHOSSurfaceOpenHarmony"));
        EXPECT_NE(vkCreateOHOSSurfaceOpenHarmony, nullptr);
        vkDestroySurfaceKHR = reinterpret_cast<PFN_vkDestroySurfaceKHR>(
            vkGetInstanceProcAddr(instance, "vkDestroySurfaceKHR"));
        EXPECT_NE(vkDestroySurfaceKHR, nullptr);
        fpGetPhysicalDeviceSurfaceSupportKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceSupportKHR>(
            vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceSupportKHR"));
        EXPECT_NE(fpGetPhysicalDeviceSurfaceSupportKHR, nullptr);
        fpGetPhysicalDeviceSurfaceCapabilitiesKHR =  reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR>(
            vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR"));
        EXPECT_NE(fpGetPhysicalDeviceSurfaceCapabilitiesKHR, nullptr);
        fpGetPhysicalDeviceSurfaceFormatsKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceFormatsKHR>(
            vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceFormatsKHR"));
        EXPECT_NE(fpGetPhysicalDeviceSurfaceFormatsKHR, nullptr);
        fpGetPhysicalDeviceSurfacePresentModesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfacePresentModesKHR>(
            vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfacePresentModesKHR"));
        EXPECT_NE(fpGetPhysicalDeviceSurfacePresentModesKHR, nullptr);
    }
}

/**
 * @tc.name: create device
 * @tc.desc: create device
 * @tc.type: FUNC
 * @tc.require: issueI5ODXM
 */
HWTEST_F(VulkanWrapperApiTest, createDevice_Test, TestSize.Level1)
{
    if (isSupportedVulkan) {
        uint32_t gpuCount = 0;
        VkResult err = vkEnumeratePhysicalDevices(instance, &gpuCount, nullptr);
        EXPECT_EQ(err, VK_SUCCESS);
        EXPECT_NE(gpuCount, 0);
        std::vector<VkPhysicalDevice> physicalDevices(gpuCount);
        err = vkEnumeratePhysicalDevices(instance, &gpuCount, physicalDevices.data());
        EXPECT_EQ(err, VK_SUCCESS);
        physicalDevice = physicalDevices[0];

        std::vector<const char*> deviceExtensions;
        deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
        VkDeviceCreateInfo deviceCreateInfo = {};
        deviceCreateInfo.enabledExtensionCount = (uint32_t)deviceExtensions.size();
        deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

        VkDevice logicalDevice;
        err = vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &logicalDevice);
        EXPECT_EQ(err, VK_SUCCESS);
        EXPECT_NE(logicalDevice, nullptr);
        device = logicalDevice;
    }
}

/**
 * @tc.name: Load device based Vulkan function pointers
 * @tc.desc: Load device based Vulkan function pointers
 * @tc.type: FUNC
 * @tc.require: issueI5ODXM
 */
HWTEST_F(VulkanWrapperApiTest, LoadFuncPtr003, TestSize.Level1)
{
    if (isSupportedVulkan) {
        fpCreateSwapchainKHR = reinterpret_cast<PFN_vkCreateSwapchainKHR>(
            vkGetDeviceProcAddr(device, "vkCreateSwapchainKHR"));
        EXPECT_NE(fpCreateSwapchainKHR, nullptr);
        fpDestroySwapchainKHR = reinterpret_cast<PFN_vkDestroySwapchainKHR>(
            vkGetDeviceProcAddr(device, "vkDestroySwapchainKHR"));
        EXPECT_NE(fpDestroySwapchainKHR, nullptr);
        fpGetSwapchainImagesKHR = reinterpret_cast<PFN_vkGetSwapchainImagesKHR>(
            vkGetDeviceProcAddr(device, "vkGetSwapchainImagesKHR"));
        EXPECT_NE(fpGetSwapchainImagesKHR, nullptr);
        fpAcquireNextImageKHR = reinterpret_cast<PFN_vkAcquireNextImageKHR>(
            vkGetDeviceProcAddr(device, "vkAcquireNextImageKHR"));
        EXPECT_NE(fpAcquireNextImageKHR, nullptr);
        fpQueuePresentKHR = reinterpret_cast<PFN_vkQueuePresentKHR>(vkGetDeviceProcAddr(device, "vkQueuePresentKHR"));
        EXPECT_NE(fpQueuePresentKHR, nullptr);
    }
}

/**
 * @tc.name: create surface
 * @tc.desc: create surface
 * @tc.type: FUNC
 * @tc.require: issueI5ODXM
 */
HWTEST_F(VulkanWrapperApiTest, createSurface_Test, TestSize.Level1)
{
    if (isSupportedVulkan) {
        constexpr int windowLeft = 100;
        constexpr int windowTop = 200;
        constexpr int windowWidth = 360;
        constexpr int windowHeight = 360;
        OHOS::Rosen::Rect rect = {windowLeft, windowTop, windowWidth, windowHeight};
        OHOS::sptr<OHOS::Rosen::WindowOption> option(new OHOS::Rosen::WindowOption());
        option->SetDisplayId(0);
        option->SetWindowRect(rect);
        option->SetWindowType(OHOS::Rosen::WindowType::APP_MAIN_WINDOW_BASE);
        option->SetWindowMode(OHOS::Rosen::WindowMode::WINDOW_MODE_FLOATING);
        option->SetWindowName("createSurface_test");
        OHOS::sptr<OHOS::Rosen::Window> window = OHOS::Rosen::Window::Create(option->GetWindowName(), option);
        EXPECT_NE(window, nullptr);

        OHOS::Rosen::RSTransaction::FlushImplicitTransaction();
        window->Show();

        auto surfaceNode = window->GetSurfaceNode();
        OHOS::sptr<OHOS::Surface> surf = surfaceNode->GetSurface();
        OHNativeWindow* nativeWindow = CreateNativeWindowFromSurface(&surf);
        EXPECT_NE(nativeWindow, nullptr);
        VkOHOSSurfaceCreateInfoOpenHarmony surfaceCreateInfo = {};
        surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_OHOS_SURFACE_CREATE_INFO_OPENHARMONY;
        surfaceCreateInfo.window = nativeWindow;
        VkResult err = vkCreateOHOSSurfaceOpenHarmony(instance, &surfaceCreateInfo, NULL, &surface);
        EXPECT_EQ(err, VK_SUCCESS);
        EXPECT_NE(surface, VK_NULL_HANDLE);
    }
}

/**
 * @tc.name: create swapChain
 * @tc.desc: create swapChain
 * @tc.type: FUNC
 * @tc.require: issueI5ODXM
 */
HWTEST_F(VulkanWrapperApiTest, createSwapChain_Test, TestSize.Level1)
{
    if (isSupportedVulkan) {
        VkSwapchainKHR oldSwapchain = swapChain;
        VkSwapchainCreateInfoKHR swapchainCI = {};
        swapchainCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCI.surface = surface;
        VkSurfaceCapabilitiesKHR surfCaps;
        VkResult err = fpGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfCaps);
        EXPECT_EQ(err, VK_SUCCESS);
        uint32_t desiredNumberOfSwapchainImages = surfCaps.minImageCount + 1;
        swapchainCI.minImageCount = desiredNumberOfSwapchainImages;
        swapchainCI.imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
        uint32_t formatCount;
        err = fpGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, NULL);
        EXPECT_EQ(err, VK_SUCCESS);
        EXPECT_GT(formatCount, 0);
        std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
        err = fpGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, surfaceFormats.data());
        EXPECT_EQ(err, VK_SUCCESS);
        swapchainCI.imageColorSpace = surfaceFormats[0].colorSpace;
        uint32_t width = 1280;
        uint32_t height = 720;
        swapchainCI.imageExtent = { width, height };
        swapchainCI.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchainCI.preTransform = (VkSurfaceTransformFlagBitsKHR)VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        swapchainCI.imageArrayLayers = 1;
        swapchainCI.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainCI.queueFamilyIndexCount = 0;
        swapchainCI.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
        swapchainCI.oldSwapchain = oldSwapchain;
        swapchainCI.clipped = VK_TRUE;
        swapchainCI.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

        err = fpCreateSwapchainKHR(device, &swapchainCI, nullptr, &swapChain);
        EXPECT_EQ(err, VK_SUCCESS);
        EXPECT_NE(swapChain, VK_NULL_HANDLE);
    }
}
}