/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <cstddef>
#include <window.h>
#include <gtest/gtest.h>
#include <dlfcn.h>
#include <iostream>
#include <string>

#include "refbase.h"
#include "surface.h"
#include "vulkan/vulkan.h"
#include "driver.h"
#include "swapchain.h"
#include "render_context/render_context.h"
#include "transaction/rs_transaction.h"
#include "ui/rs_surface_extractor.h"
#include "ui/rs_surface_node.h"
#include "wm/window.h"

using namespace testing;
using namespace testing::ext;

namespace vulkan::driver {
typedef bool (*PFN_IsSupportedVulkan)();
class VulkanWrapperUnitTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase();
    void SetUp() {}
    void TearDown() {}

    static inline PFN_vkCreateInstance vkCreateInstance;
    static inline PFN_vkDestroyInstance vkDestroyInstance;
    static inline PFN_vkDestroySurfaceKHR vkDestroySurfaceKHR;
    static inline PFN_vkCreateDevice vkCreateDevice;
    static inline PFN_vkDestroyDevice vkDestroyDevice;
    static inline PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr;
    static inline PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr;
    static inline PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices;
    static inline PFN_vkCreateOHOSSurfaceOpenHarmony vkCreateOHOSSurfaceOpenHarmony;
    static inline PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR fpGetPhysicalDeviceSurfaceCapabilitiesKHR;
    static inline PFN_vkGetPhysicalDeviceSurfacePresentModesKHR fpGetPhysicalDeviceSurfacePresentModesKHR;
    static inline PFN_vkGetPhysicalDeviceSurfaceFormatsKHR fpGetPhysicalDeviceSurfaceFormatsKHR;
    static inline PFN_vkCreateSwapchainKHR fpCreateSwapchainKHR;
    static inline PFN_vkDestroySwapchainKHR fpDestroySwapchainKHR;
    static inline PFN_vkQueuePresentKHR fpQueuePresentKHR;
    static inline PFN_IsSupportedVulkan fpIsSupportedVulkan;

    static inline void *libVulkan = nullptr;
    static inline VkInstance instance = nullptr;
    static inline VkSurfaceKHR surface = VK_NULL_HANDLE;
    static inline VkPhysicalDevice physicalDevice = nullptr;
    static inline VkDevice device = nullptr;
    static inline VkSurfaceCapabilitiesKHR surfCaps = {};
    static inline VkSurfaceFormatKHR surfaceFormat = {};
    static inline VkSwapchainKHR swapChain = VK_NULL_HANDLE;
    static inline VkSemaphore semaphore = VK_NULL_HANDLE;
    static inline bool isSupportedVulkan = false;
};

void VulkanWrapperUnitTest::TearDownTestCase()
{
    if (device != nullptr) {
        if (swapChain != VK_NULL_HANDLE) {
            fpDestroySwapchainKHR(device, swapChain, nullptr);
        }
        vkDestroyDevice(device, nullptr);
    }
    if (instance != nullptr) {
        if (surface != VK_NULL_HANDLE) {
            vkDestroySurfaceKHR(instance, surface, nullptr);
        }
        vkDestroyInstance(instance, nullptr);
    }
}

/**
 * @tc.name: dlopen
 * @tc.desc: dlopen
 * @tc.type: FUNC
 * @tc.require: issueI5ODXM
 */
HWTEST_F(VulkanWrapperUnitTest, dlopen_Test, TestSize.Level1)
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
 * @tc.name: load function pointer 001
 * @tc.desc: load function pointer 001
 * @tc.type: FUNC
 * @tc.require: issueI5ODXM
 */
HWTEST_F(VulkanWrapperUnitTest, LoadFuncPtr001, TestSize.Level1)
{
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
 * @tc.name: test vkCreateInstance
 * @tc.desc: test vkCreateInstance
 * @tc.type: FUNC
 * @tc.require: issueI5ODXM
 */
HWTEST_F(VulkanWrapperUnitTest, vkCreateInstance_Test, TestSize.Level1)
{
    if (isSupportedVulkan) {
        EXPECT_NE(vkCreateInstance, nullptr);
        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "pApplicationName";
        appInfo.pEngineName = "pEngineName";
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo instanceCreateInfo = {};
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pNext = NULL;
        instanceCreateInfo.pApplicationInfo = &appInfo;

        VkResult err = vkCreateInstance(&instanceCreateInfo, nullptr, &instance);
        EXPECT_EQ(err, VK_SUCCESS);
        EXPECT_NE(instance, nullptr);
    }
}

/**
 * @tc.name: load instance based function pointer
 * @tc.desc: load instance based function pointer
 * @tc.type: FUNC
 * @tc.require: issueI5ODXM
 */
HWTEST_F(VulkanWrapperUnitTest, LoadFuncPtr002, TestSize.Level1)
{
    if (isSupportedVulkan) {
        EXPECT_NE(instance, nullptr);

        vkDestroyInstance = reinterpret_cast<PFN_vkDestroyInstance>(
            vkGetInstanceProcAddr(instance, "vkDestroyInstance"));
        EXPECT_NE(vkDestroyInstance, nullptr);
        vkEnumeratePhysicalDevices = reinterpret_cast<PFN_vkEnumeratePhysicalDevices>(
            vkGetInstanceProcAddr(instance, "vkEnumeratePhysicalDevices"));
        EXPECT_NE(vkEnumeratePhysicalDevices, nullptr);
        vkCreateDevice = reinterpret_cast<PFN_vkCreateDevice>(vkGetInstanceProcAddr(instance, "vkCreateDevice"));
        EXPECT_NE(vkCreateDevice, nullptr);
        vkDestroyDevice = reinterpret_cast<PFN_vkDestroyDevice>(vkGetInstanceProcAddr(instance, "vkDestroyDevice"));
        EXPECT_NE(vkDestroyDevice, nullptr);
        vkDestroySurfaceKHR = reinterpret_cast<PFN_vkDestroySurfaceKHR>(
            vkGetInstanceProcAddr(instance, "vkDestroySurfaceKHR"));
        EXPECT_NE(vkDestroySurfaceKHR, nullptr);

        vkCreateOHOSSurfaceOpenHarmony = reinterpret_cast<PFN_vkCreateOHOSSurfaceOpenHarmony>(
            vkGetInstanceProcAddr(instance, "vkCreateOHOSSurfaceOpenHarmony"));
        EXPECT_NE(vkCreateOHOSSurfaceOpenHarmony, nullptr);

        fpGetPhysicalDeviceSurfaceCapabilitiesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR>(
            vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR"));
        EXPECT_NE(fpGetPhysicalDeviceSurfaceCapabilitiesKHR, nullptr);
        fpGetPhysicalDeviceSurfacePresentModesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfacePresentModesKHR>(
            vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfacePresentModesKHR"));
        EXPECT_NE(fpGetPhysicalDeviceSurfacePresentModesKHR, nullptr);
        fpGetPhysicalDeviceSurfaceFormatsKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceFormatsKHR>(
            vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceFormatsKHR"));
        EXPECT_NE(fpGetPhysicalDeviceSurfaceFormatsKHR, nullptr);
    }
}

/**
 * @tc.name: test EnumeratePhysicalDevices
 * @tc.desc: test EnumeratePhysicalDevices
 * @tc.type: FUNC
 * @tc.require: issueI5ODXM
 */
HWTEST_F(VulkanWrapperUnitTest, EnumeratePhysicalDevices_Test, TestSize.Level1)
{
    if (isSupportedVulkan) {
        EXPECT_NE(instance, nullptr);

        uint32_t gpuCount = 0;
        VkResult err = EnumeratePhysicalDevices(instance, &gpuCount, nullptr);
        EXPECT_EQ(err, VK_SUCCESS);
        EXPECT_NE(gpuCount, 0);
        std::vector<VkPhysicalDevice> physicalDevices(gpuCount);
        err = EnumeratePhysicalDevices(instance, &gpuCount, physicalDevices.data());
        EXPECT_EQ(err, VK_SUCCESS);
        physicalDevice = physicalDevices[0];
        EXPECT_NE(physicalDevice, nullptr);

        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
        GetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
        GetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);
        GetPhysicalDeviceMemoryProperties(physicalDevice, &deviceMemoryProperties);
    }
}

/**
 * @tc.name: test GetPhysicalDeviceQueueFamilyProperties
 * @tc.desc: test GetPhysicalDeviceQueueFamilyProperties
 * @tc.type: FUNC
 * @tc.require: issueI5ODXM
 */
HWTEST_F(VulkanWrapperUnitTest, GetPhysicalDeviceQueueFamilyProperties_Test, TestSize.Level1)
{
    if (isSupportedVulkan) {
        uint32_t queueCount;
        GetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, NULL);
        EXPECT_GT(queueCount, 0);

        std::vector<VkQueueFamilyProperties> queueProps(queueCount);
        GetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, queueProps.data());
    }
}

/**
 * @tc.name: test vkCreateDevice
 * @tc.desc: test vkCreateDevice
 * @tc.type: FUNC
 * @tc.require: issueI5ODXM
 */
HWTEST_F(VulkanWrapperUnitTest, vkCreateDevice_Test, TestSize.Level1)
{
    if (isSupportedVulkan) {
        EXPECT_NE(vkCreateDevice, nullptr);
        EXPECT_NE(physicalDevice, nullptr);

        VkDeviceCreateInfo deviceCreateInfo = {};
        std::vector<const char*> deviceExtensions;
        deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
        deviceCreateInfo.enabledExtensionCount = (uint32_t)deviceExtensions.size();
        deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
        VkDevice logicalDevice;
        VkResult err = vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &logicalDevice);
        EXPECT_EQ(err, VK_SUCCESS);
        EXPECT_NE(logicalDevice, nullptr);
        device = logicalDevice;
    }
}

/**
 * @tc.name: test vkCreateOHOSSurfaceOpenHarmony
 * @tc.desc: test vkCreateOHOSSurfaceOpenHarmony
 * @tc.type: FUNC
 * @tc.require: issueI5ODXM
 */
HWTEST_F(VulkanWrapperUnitTest, vkCreateOHOSSurfaceOpenHarmony_Test, TestSize.Level1)
{
    if (isSupportedVulkan) {
        EXPECT_NE(vkCreateOHOSSurfaceOpenHarmony, nullptr);
        EXPECT_NE(instance, nullptr);

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
        option->SetWindowName("createSurface_unittest");
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
 * @tc.name: load device based function pointer 003
 * @tc.desc: load device based function pointer 003
 * @tc.type: FUNC
 * @tc.require: issueI5ODXM
 */
HWTEST_F(VulkanWrapperUnitTest, LoadFuncPtr003, TestSize.Level1)
{
    if (isSupportedVulkan) {
        EXPECT_NE(device, nullptr);

        fpCreateSwapchainKHR = reinterpret_cast<PFN_vkCreateSwapchainKHR>(
            vkGetDeviceProcAddr(device, "vkCreateSwapchainKHR"));
        EXPECT_NE(fpCreateSwapchainKHR, nullptr);
        fpDestroySwapchainKHR = reinterpret_cast<PFN_vkDestroySwapchainKHR>(
            vkGetDeviceProcAddr(device, "vkDestroySwapchainKHR"));
        EXPECT_NE(fpDestroySwapchainKHR, nullptr);
        fpQueuePresentKHR = reinterpret_cast<PFN_vkQueuePresentKHR>(
            vkGetDeviceProcAddr(device, "vkQueuePresentKHR"));
        EXPECT_NE(fpQueuePresentKHR, nullptr);
    }
}

/**
 * @tc.name: test fpGetPhysicalDeviceSurfaceCapabilitiesKHR
 * @tc.desc: test fpGetPhysicalDeviceSurfaceCapabilitiesKHR
 * @tc.type: FUNC
 * @tc.require: issueI5ODXM
 */
HWTEST_F(VulkanWrapperUnitTest, fpGetPhysicalDeviceSurfaceCapabilitiesKHR_Test, TestSize.Level1)
{
    if (isSupportedVulkan) {
        EXPECT_NE(fpGetPhysicalDeviceSurfaceCapabilitiesKHR, nullptr);
        EXPECT_NE(physicalDevice, nullptr);
        EXPECT_NE(surface, VK_NULL_HANDLE);

        VkResult err = fpGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfCaps);
        EXPECT_EQ(err, VK_SUCCESS);
    }
}

/**
 * @tc.name: test fpGetPhysicalDeviceSurfacePresentModesKHR
 * @tc.desc: test fpGetPhysicalDeviceSurfacePresentModesKHR
 * @tc.type: FUNC
 * @tc.require: issueI5ODXM
 */
HWTEST_F(VulkanWrapperUnitTest, fpGetPhysicalDeviceSurfacePresentModesKHR_Test, TestSize.Level1)
{
    if (isSupportedVulkan) {
        EXPECT_NE(fpGetPhysicalDeviceSurfacePresentModesKHR, nullptr);
        EXPECT_NE(physicalDevice, nullptr);
        EXPECT_NE(surface, VK_NULL_HANDLE);

        uint32_t presentModeCount;
        VkResult err = fpGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, NULL);
        EXPECT_EQ(err, VK_SUCCESS);
        EXPECT_GT(presentModeCount, 0);

        std::vector<VkPresentModeKHR> presentModes(presentModeCount);
        err = fpGetPhysicalDeviceSurfacePresentModesKHR(
            physicalDevice, surface, &presentModeCount, presentModes.data());
        EXPECT_EQ(err, VK_SUCCESS);
    }
}

/**
 * @tc.name: test fpGetPhysicalDeviceSurfaceFormatsKHR
 * @tc.desc: test fpGetPhysicalDeviceSurfaceFormatsKHR
 * @tc.type: FUNC
 * @tc.require: issueI5ODXM
 */
HWTEST_F(VulkanWrapperUnitTest, fpGetPhysicalDeviceSurfaceFormatsKHR_Test, TestSize.Level1)
{
    if (isSupportedVulkan) {
        EXPECT_NE(fpGetPhysicalDeviceSurfaceFormatsKHR, nullptr);
        EXPECT_NE(physicalDevice, nullptr);
        EXPECT_NE(surface, VK_NULL_HANDLE);

        uint32_t formatCount;
        VkResult err = fpGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, NULL);
        EXPECT_EQ(err, VK_SUCCESS);
        EXPECT_GT(formatCount, 0);
        std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
        err = fpGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, surfaceFormats.data());
        EXPECT_EQ(err, VK_SUCCESS);
        surfaceFormat = surfaceFormats[0];
    }
}

/**
 * @tc.name: test fpCreateSwapchainKHR
 * @tc.desc: test fpCreateSwapchainKHR
 * @tc.type: FUNC
 * @tc.require: issueI5ODXM
 */
HWTEST_F(VulkanWrapperUnitTest, fpCreateSwapchainKHR_Test, TestSize.Level1)
{
    if (isSupportedVulkan) {
        EXPECT_NE(fpCreateSwapchainKHR, nullptr);
        EXPECT_NE(device, nullptr);
        EXPECT_NE(surface, VK_NULL_HANDLE);

        VkSwapchainKHR oldSwapchain = swapChain;
        VkSwapchainCreateInfoKHR swapchainCI = {};
        swapchainCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCI.surface = surface;
        uint32_t desiredNumberOfSwapchainImages = surfCaps.minImageCount + 1;
        swapchainCI.minImageCount = desiredNumberOfSwapchainImages;
        swapchainCI.imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
        swapchainCI.imageColorSpace = surfaceFormat.colorSpace;
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

        VkResult err = fpCreateSwapchainKHR(device, &swapchainCI, nullptr, &swapChain);
        EXPECT_EQ(err, VK_SUCCESS);
        EXPECT_NE(swapChain, VK_NULL_HANDLE);
    }
}

/**
 * @tc.name: test vkCreateSemaphore
 * @tc.desc: test vkCreateSemaphore
 * @tc.type: FUNC
 * @tc.require: issueI5ODXM
 */
HWTEST_F(VulkanWrapperUnitTest, vkCreateSemaphore_Test, TestSize.Level1)
{
    if (isSupportedVulkan) {
        VkSemaphoreCreateInfo semaphoreCreateInfo {};
        semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        PFN_vkCreateSemaphore vkCreateSemaphore = reinterpret_cast<PFN_vkCreateSemaphore>(
            vkGetInstanceProcAddr(instance, "vkCreateSemaphore"));
        EXPECT_NE(vkCreateSemaphore, nullptr);
        VkResult err = vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &semaphore);
        EXPECT_EQ(err, VK_SUCCESS);
    }
}
} // vulkan::driver
