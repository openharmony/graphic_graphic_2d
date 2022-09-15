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

#include <window.h>
#include <gtest/gtest.h>
#include <dlfcn.h>
#include <iostream>
#include <string>

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
class VulkanWrapperUnitTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}

    static inline PFN_vkCreateInstance vkCreateInstance;
    static inline PFN_vkCreateDevice vkCreateDevice;
    static inline PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr;
    static inline PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr;
    static inline PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices;
    static inline PFN_vkCreateOHOSSurfaceOpenHarmony vkCreateOHOSSurfaceOpenHarmony;
    static inline PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR fpGetPhysicalDeviceSurfaceCapabilitiesKHR;
    static inline PFN_vkGetPhysicalDeviceSurfaceFormatsKHR fpGetPhysicalDeviceSurfaceFormatsKHR;
    static inline PFN_vkCreateSwapchainKHR fpCreateSwapchainKHR;

    static inline void *libVulkan = nullptr;
    static inline VkInstance instance = nullptr;
    static inline VkSurfaceKHR surface = VK_NULL_HANDLE;
    static inline VkPhysicalDevice physicalDevice = nullptr;
    static inline VkDevice device = nullptr;
    static inline VkSurfaceCapabilitiesKHR surfCaps = {};
    static inline VkSurfaceFormatKHR surfaceFormat = {};
    static inline VkSwapchainKHR swapChain = VK_NULL_HANDLE;
};

/**
 * @tc.name: dlopen
 * @tc.desc: dlopen
 * @tc.type: FUNC
 * @tc.require: issueI5ODXM
 */
HWTEST_F(VulkanWrapperUnitTest, dlopen_Test, Level1)
{
    const char* path = "/system/lib64/libvulkan.so";
    libVulkan = dlopen(path, RTLD_NOW | RTLD_LOCAL);
    EXPECT_NE(libVulkan, nullptr);
}

/**
 * @tc.name: load function pointer 001
 * @tc.desc: load function pointer 001
 * @tc.type: FUNC
 * @tc.require: issueI5ODXM
 */
HWTEST_F(VulkanWrapperUnitTest, LoadFuncPtr001, Level1)
{
    vkCreateInstance = reinterpret_cast<PFN_vkCreateInstance>(dlsym(libVulkan, "vkCreateInstance"));
    EXPECT_NE(vkCreateInstance, nullptr);
    vkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(dlsym(libVulkan, "vkGetInstanceProcAddr"));
    EXPECT_NE(vkGetInstanceProcAddr, nullptr);
    vkGetDeviceProcAddr = reinterpret_cast<PFN_vkGetDeviceProcAddr>(dlsym(libVulkan, "vkGetDeviceProcAddr"));
    EXPECT_NE(vkGetDeviceProcAddr, nullptr);
}

/**
 * @tc.name: test vkCreateInstance
 * @tc.desc: test vkCreateInstance
 * @tc.type: FUNC
 * @tc.require: issueI5ODXM
 */
HWTEST_F(VulkanWrapperUnitTest, vkCreateInstance_Test, Level1)
{
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

/**
 * @tc.name: load instance based function pointer
 * @tc.desc: load instance based function pointer
 * @tc.type: FUNC
 * @tc.require: issueI5ODXM
 */
HWTEST_F(VulkanWrapperUnitTest, LoadFuncPtr002, Level1)
{
    EXPECT_NE(instance, nullptr);

    vkEnumeratePhysicalDevices = reinterpret_cast<PFN_vkEnumeratePhysicalDevices>(
        vkGetInstanceProcAddr(instance, "vkEnumeratePhysicalDevices"));
    EXPECT_NE(vkEnumeratePhysicalDevices, nullptr);
    vkCreateDevice = reinterpret_cast<PFN_vkCreateDevice>(vkGetInstanceProcAddr(instance, "vkCreateDevice"));
    EXPECT_NE(vkCreateDevice, nullptr);

    vkCreateOHOSSurfaceOpenHarmony = reinterpret_cast<PFN_vkCreateOHOSSurfaceOpenHarmony>(
        vkGetInstanceProcAddr(instance, "vkCreateOHOSSurfaceOpenHarmony"));
    EXPECT_NE(vkCreateOHOSSurfaceOpenHarmony, nullptr);

    fpGetPhysicalDeviceSurfaceCapabilitiesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR>(
        vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR"));
    EXPECT_NE(fpGetPhysicalDeviceSurfaceCapabilitiesKHR, nullptr);
    fpGetPhysicalDeviceSurfaceFormatsKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceFormatsKHR>(
        vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceFormatsKHR"));
    EXPECT_NE(fpGetPhysicalDeviceSurfaceFormatsKHR, nullptr);
}

/**
 * @tc.name: test vkEnumeratePhysicalDevices
 * @tc.desc: test vkEnumeratePhysicalDevices
 * @tc.type: FUNC
 * @tc.require: issueI5ODXM
 */
HWTEST_F(VulkanWrapperUnitTest, vkEnumeratePhysicalDevices_Test, Level1)
{
    EXPECT_NE(vkEnumeratePhysicalDevices, nullptr);
    EXPECT_NE(instance, nullptr);

    uint32_t gpuCount = 0;
    VkResult err = vkEnumeratePhysicalDevices(instance, &gpuCount, nullptr);
    EXPECT_EQ(err, VK_SUCCESS);
    EXPECT_NE(gpuCount, 0);
    std::vector<VkPhysicalDevice> physicalDevices(gpuCount);
    err = vkEnumeratePhysicalDevices(instance, &gpuCount, physicalDevices.data());
    EXPECT_EQ(err, VK_SUCCESS);
    physicalDevice = physicalDevices[0];
}

/**
 * @tc.name: test vkCreateDevice
 * @tc.desc: test vkCreateDevice
 * @tc.type: FUNC
 * @tc.require: issueI5ODXM
 */
HWTEST_F(VulkanWrapperUnitTest, vkCreateDevice_Test, Level1)
{
    EXPECT_NE(vkCreateDevice, nullptr);
    EXPECT_NE(physicalDevice, nullptr);

    VkDeviceCreateInfo deviceCreateInfo = {};
    VkDevice logicalDevice;
    VkResult err = vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &logicalDevice);
    EXPECT_EQ(err, VK_SUCCESS);
    EXPECT_NE(logicalDevice, nullptr);
    device = logicalDevice;
}

/**
 * @tc.name: test vkCreateOHOSSurfaceOpenHarmony
 * @tc.desc: test vkCreateOHOSSurfaceOpenHarmony
 * @tc.type: FUNC
 * @tc.require: issueI5ODXM
 */
HWTEST_F(VulkanWrapperUnitTest, vkCreateOHOSSurfaceOpenHarmony_Test, Level1)
{
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

/**
 * @tc.name: load device based function pointer 003
 * @tc.desc: load device based function pointer 003
 * @tc.type: FUNC
 * @tc.require: issueI5ODXM
 */
HWTEST_F(VulkanWrapperUnitTest, LoadFuncPtr003, Level1)
{
    EXPECT_NE(device, nullptr);

    fpCreateSwapchainKHR = reinterpret_cast<PFN_vkCreateSwapchainKHR>(
        vkGetDeviceProcAddr(device, "vkCreateSwapchainKHR"));
    EXPECT_NE(fpCreateSwapchainKHR, nullptr);
}

/**
 * @tc.name: test fpGetPhysicalDeviceSurfaceCapabilitiesKHR
 * @tc.desc: test fpGetPhysicalDeviceSurfaceCapabilitiesKHR
 * @tc.type: FUNC
 * @tc.require: issueI5ODXM
 */
HWTEST_F(VulkanWrapperUnitTest, fpGetPhysicalDeviceSurfaceCapabilitiesKHR_Test, Level1)
{
    EXPECT_NE(fpGetPhysicalDeviceSurfaceCapabilitiesKHR, nullptr);
    EXPECT_NE(physicalDevice, nullptr);
    EXPECT_NE(surface, VK_NULL_HANDLE);

    VkResult err = fpGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfCaps);
    EXPECT_EQ(err, VK_SUCCESS);
}

/**
 * @tc.name: test fpGetPhysicalDeviceSurfaceFormatsKHR
 * @tc.desc: test fpGetPhysicalDeviceSurfaceFormatsKHR
 * @tc.type: FUNC
 * @tc.require: issueI5ODXM
 */
HWTEST_F(VulkanWrapperUnitTest, fpGetPhysicalDeviceSurfaceFormatsKHR_Test, Level1)
{
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

/**
 * @tc.name: test fpCreateSwapchainKHR
 * @tc.desc: test fpCreateSwapchainKHR
 * @tc.type: FUNC
 * @tc.require: issueI5ODXM
 */
HWTEST_F(VulkanWrapperUnitTest, fpCreateSwapchainKHR_Test, Level1)
{
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
} // OHOS::Rosen
