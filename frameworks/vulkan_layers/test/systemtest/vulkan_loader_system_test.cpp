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

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen;

namespace vulkan::loader {
class VulkanLoaderSystemTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase()
    {
        if (libVulkan_ != nullptr) {
            dlclose(libVulkan_);
            libVulkan_ = nullptr;
        }
    }
    static inline void DLOpenLibVulkan();
    static inline void TrytoCreateVkInstance();

    static inline PFN_vkCreateInstance vkCreateInstance;
    static inline PFN_vkDestroySurfaceKHR vkDestroySurfaceKHR;
    static inline PFN_vkCreateDevice vkCreateDevice;
    static inline PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties;
    static inline PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties;
    static inline PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr;
    static inline PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr;
    static inline PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices;
    static inline PFN_vkCreateSurfaceOHOS vkCreateSurfaceOHOS;
    static inline PFN_vkGetPhysicalDeviceSurfaceSupportKHR fpGetPhysicalDeviceSurfaceSupportKHR;
    static inline PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR fpGetPhysicalDeviceSurfaceCapabilitiesKHR;
    static inline PFN_vkGetPhysicalDeviceSurfaceFormatsKHR fpGetPhysicalDeviceSurfaceFormatsKHR;
    static inline PFN_vkGetPhysicalDeviceSurfacePresentModesKHR fpGetPhysicalDeviceSurfacePresentModesKHR;
    static inline PFN_vkCreateSwapchainKHR fpCreateSwapchainKHR;
    static inline PFN_vkDestroySwapchainKHR fpDestroySwapchainKHR;
    static inline PFN_vkGetSwapchainImagesKHR fpGetSwapchainImagesKHR;
    static inline PFN_vkAcquireNextImageKHR fpAcquireNextImageKHR;
    static inline PFN_vkQueuePresentKHR fpQueuePresentKHR;

    static inline void *libVulkan_ = nullptr;
    static inline VkInstance instance_ = nullptr;
    static inline VkSurfaceKHR surface_ = VK_NULL_HANDLE;
    static inline VkPhysicalDevice physicalDevice_ = nullptr;
    static inline VkDevice device_ = nullptr;
    static inline VkSwapchainKHR swapChain_ = VK_NULL_HANDLE;
    static inline bool isSupportedVulkan_ = false;
};

void VulkanLoaderSystemTest::DLOpenLibVulkan()
{
#if (defined(__aarch64__) || defined(__x86_64__))
    const char *path = "/system/lib64/platformsdk/libvulkan.so";
#else
    const char *path = "/system/lib/platformsdk/libvulkan.so";
#endif
    libVulkan_ = dlopen(path, RTLD_NOW | RTLD_LOCAL);
    if (libVulkan_ == nullptr) {
        std::cout << "dlerror: " << dlerror() << std::endl;
        isSupportedVulkan_ = false;
        return;
    }
    isSupportedVulkan_ = true;
}

void VulkanLoaderSystemTest::TrytoCreateVkInstance()
{
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

    VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &instance_);
    if (result == VK_ERROR_INCOMPATIBLE_DRIVER) {
        isSupportedVulkan_ = false;
    } else {
        isSupportedVulkan_ = true;
    }
    std::cout << "TrytoCreateVkInstance result: " << result << std::endl;
}

/**
 * @tc.name: Load base Vulkan functions
 * @tc.desc: Load base Vulkan functions
 * @tc.type: FUNC
 * @tc.require: issueI6SKRO
 */
HWTEST_F(VulkanLoaderSystemTest, LoadBaseFuncPtr, TestSize.Level1)
{
    DLOpenLibVulkan();
    if (isSupportedVulkan_) {
        EXPECT_NE(libVulkan_, nullptr);

        // Load base functions
        vkEnumerateInstanceExtensionProperties = reinterpret_cast<PFN_vkEnumerateInstanceExtensionProperties>(
            dlsym(libVulkan_, "vkEnumerateInstanceExtensionProperties"));
        EXPECT_NE(vkEnumerateInstanceExtensionProperties, nullptr);
        vkEnumerateInstanceLayerProperties = reinterpret_cast<PFN_vkEnumerateInstanceLayerProperties>(
            dlsym(libVulkan_, "vkEnumerateInstanceLayerProperties"));
        EXPECT_NE(vkEnumerateInstanceLayerProperties, nullptr);
        vkCreateInstance = reinterpret_cast<PFN_vkCreateInstance>(dlsym(libVulkan_, "vkCreateInstance"));
        EXPECT_NE(vkCreateInstance, nullptr);
        vkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(dlsym(libVulkan_, "vkGetInstanceProcAddr"));
        EXPECT_NE(vkGetInstanceProcAddr, nullptr);
        vkGetDeviceProcAddr = reinterpret_cast<PFN_vkGetDeviceProcAddr>(dlsym(libVulkan_, "vkGetDeviceProcAddr"));
        EXPECT_NE(vkGetDeviceProcAddr, nullptr);
        TrytoCreateVkInstance();
    }
}

/**
 * @tc.name: Load instance based Vulkan function pointers
 * @tc.desc: Load instance based Vulkan function pointers
 * @tc.type: FUNC
 * @tc.require: issueI6SKRO
 */
HWTEST_F(VulkanLoaderSystemTest, LoadInstanceFuncPtr, TestSize.Level1)
{
    if (isSupportedVulkan_) {
        vkEnumeratePhysicalDevices = reinterpret_cast<PFN_vkEnumeratePhysicalDevices>(
            vkGetInstanceProcAddr(instance_, "vkEnumeratePhysicalDevices"));
        EXPECT_NE(vkEnumeratePhysicalDevices, nullptr);
        vkCreateDevice = reinterpret_cast<PFN_vkCreateDevice>(
            vkGetInstanceProcAddr(instance_, "vkCreateDevice"));
        EXPECT_NE(vkCreateDevice, nullptr);
        vkCreateSurfaceOHOS = reinterpret_cast<PFN_vkCreateSurfaceOHOS>(
            vkGetInstanceProcAddr(instance_, "vkCreateSurfaceOHOS"));
        EXPECT_NE(vkCreateSurfaceOHOS, nullptr);
        vkDestroySurfaceKHR = reinterpret_cast<PFN_vkDestroySurfaceKHR>(
            vkGetInstanceProcAddr(instance_, "vkDestroySurfaceKHR"));
        EXPECT_NE(vkDestroySurfaceKHR, nullptr);
        fpGetPhysicalDeviceSurfaceSupportKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceSupportKHR>(
            vkGetInstanceProcAddr(instance_, "vkGetPhysicalDeviceSurfaceSupportKHR"));
        EXPECT_NE(fpGetPhysicalDeviceSurfaceSupportKHR, nullptr);
        fpGetPhysicalDeviceSurfaceCapabilitiesKHR =  reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR>(
            vkGetInstanceProcAddr(instance_, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR"));
        EXPECT_NE(fpGetPhysicalDeviceSurfaceCapabilitiesKHR, nullptr);
        fpGetPhysicalDeviceSurfaceFormatsKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceFormatsKHR>(
            vkGetInstanceProcAddr(instance_, "vkGetPhysicalDeviceSurfaceFormatsKHR"));
        EXPECT_NE(fpGetPhysicalDeviceSurfaceFormatsKHR, nullptr);
        fpGetPhysicalDeviceSurfacePresentModesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfacePresentModesKHR>(
            vkGetInstanceProcAddr(instance_, "vkGetPhysicalDeviceSurfacePresentModesKHR"));
        EXPECT_NE(fpGetPhysicalDeviceSurfacePresentModesKHR, nullptr);
    }
}

/**
 * @tc.name: create device
 * @tc.desc: create device
 * @tc.type: FUNC
 * @tc.require: issueI6SKRO
 */
HWTEST_F(VulkanLoaderSystemTest, createDevice_Test, TestSize.Level1)
{
    if (isSupportedVulkan_) {
        uint32_t gpuCount = 0;
        VkResult err = vkEnumeratePhysicalDevices(instance_, &gpuCount, nullptr);
        EXPECT_EQ(err, VK_SUCCESS);
        EXPECT_NE(gpuCount, 0);
        std::vector<VkPhysicalDevice> physicalDevices(gpuCount);
        err = vkEnumeratePhysicalDevices(instance_, &gpuCount, physicalDevices.data());
        EXPECT_EQ(err, VK_SUCCESS);
        physicalDevice_ = physicalDevices[0];

        std::vector<const char*> deviceExtensions;
        deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
        VkDeviceCreateInfo deviceCreateInfo = {};
        deviceCreateInfo.enabledExtensionCount = (uint32_t)deviceExtensions.size();
        deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

        VkDevice logicalDevice;
        err = vkCreateDevice(physicalDevice_, &deviceCreateInfo, nullptr, &logicalDevice);
        EXPECT_EQ(err, VK_SUCCESS);
        EXPECT_NE(logicalDevice, nullptr);
        device_ = logicalDevice;
    }
}

/**
 * @tc.name: Load device based Vulkan function pointers
 * @tc.desc: Load device based Vulkan function pointers
 * @tc.type: FUNC
 * @tc.require: issueI6SKRO
 */
HWTEST_F(VulkanLoaderSystemTest, LoadDeviceFuncPtr, TestSize.Level1)
{
    if (isSupportedVulkan_) {
        fpCreateSwapchainKHR = reinterpret_cast<PFN_vkCreateSwapchainKHR>(
            vkGetDeviceProcAddr(device_, "vkCreateSwapchainKHR"));
        EXPECT_NE(fpCreateSwapchainKHR, nullptr);
        fpDestroySwapchainKHR = reinterpret_cast<PFN_vkDestroySwapchainKHR>(
            vkGetDeviceProcAddr(device_, "vkDestroySwapchainKHR"));
        EXPECT_NE(fpDestroySwapchainKHR, nullptr);
        fpGetSwapchainImagesKHR = reinterpret_cast<PFN_vkGetSwapchainImagesKHR>(
            vkGetDeviceProcAddr(device_, "vkGetSwapchainImagesKHR"));
        EXPECT_NE(fpGetSwapchainImagesKHR, nullptr);
        fpAcquireNextImageKHR = reinterpret_cast<PFN_vkAcquireNextImageKHR>(
            vkGetDeviceProcAddr(device_, "vkAcquireNextImageKHR"));
        EXPECT_NE(fpAcquireNextImageKHR, nullptr);
        fpQueuePresentKHR = reinterpret_cast<PFN_vkQueuePresentKHR>(vkGetDeviceProcAddr(device_, "vkQueuePresentKHR"));
        EXPECT_NE(fpQueuePresentKHR, nullptr);
    }
}

/**
 * @tc.name: create surface
 * @tc.desc: create surface
 * @tc.type: FUNC
 * @tc.require: issueI6SKRO
 */
HWTEST_F(VulkanLoaderSystemTest, createSurface_Test, TestSize.Level1)
{
    if (isSupportedVulkan_) {
        struct RSSurfaceNodeConfig rsSurfaceNodeConfig;
        rsSurfaceNodeConfig.SurfaceNodeName = "createSurface_test";
        auto surfaceNode = RSSurfaceNode::Create(rsSurfaceNodeConfig, RSSurfaceNodeType::DEFAULT);
        OHOS::sptr<OHOS::Surface> surf = surfaceNode->GetSurface();
        OHNativeWindow* nativeWindow = CreateNativeWindowFromSurface(&surf);
        EXPECT_NE(nativeWindow, nullptr);
        VkSurfaceCreateInfoOHOS surfaceCreateInfo = {};
        surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_SURFACE_CREATE_INFO_OHOS;
        surfaceCreateInfo.window = nativeWindow;
        VkResult err = vkCreateSurfaceOHOS(instance_, &surfaceCreateInfo, NULL, &surface_);
        EXPECT_EQ(err, VK_SUCCESS);
        EXPECT_NE(surface_, VK_NULL_HANDLE);
    }
}

/**
 * @tc.name: create swapChain
 * @tc.desc: create swapChain
 * @tc.type: FUNC
 * @tc.require: issueI6SKRO
 */
HWTEST_F(VulkanLoaderSystemTest, createSwapChain_Test, TestSize.Level1)
{
    if (isSupportedVulkan_) {
        VkSwapchainKHR oldSwapchain = swapChain_;
        VkSwapchainCreateInfoKHR swapchainCI = {};
        swapchainCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCI.surface = surface_;
        VkSurfaceCapabilitiesKHR surfCaps;
        VkResult err = fpGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice_, surface_, &surfCaps);
        EXPECT_EQ(err, VK_SUCCESS);
        uint32_t desiredNumberOfSwapchainImages = surfCaps.minImageCount + 1;
        swapchainCI.minImageCount = desiredNumberOfSwapchainImages;
        swapchainCI.imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
        uint32_t formatCount;
        err = fpGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice_, surface_, &formatCount, NULL);
        EXPECT_EQ(err, VK_SUCCESS);
        EXPECT_GT(formatCount, 0);
        std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
        err = fpGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice_, surface_, &formatCount, surfaceFormats.data());
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

        err = fpCreateSwapchainKHR(device_, &swapchainCI, nullptr, &swapChain_);
        EXPECT_EQ(err, VK_SUCCESS);
        EXPECT_NE(swapChain_, VK_NULL_HANDLE);
    }
}
}
