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
#include <vector>

#include "refbase.h"
#include "surface.h"
#include "vulkan/vulkan.h"
#include "driver.h"
#include "driver_loader.h"
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
    uint32_t getQueueFamilyIndex(VkQueueFlagBits queueFlags);
    OHNativeWindow* createNativeWindow(std::string name);
    VkSwapchainCreateInfoKHR getSwapchainCreateInfo(VkFormat imageFormat, VkColorSpaceKHR imageColorSpace);

    static inline PFN_vkCreateInstance vkCreateInstance;
    static inline PFN_vkDestroyInstance vkDestroyInstance;
    static inline PFN_vkDestroySurfaceKHR vkDestroySurfaceKHR;
    static inline PFN_vkCreateDevice vkCreateDevice;
    static inline PFN_vkDestroyDevice vkDestroyDevice;
    static inline PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr;
    static inline PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr;
    static inline PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties;
    static inline PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices;
    static inline PFN_vkCreateOHOSSurfaceOpenHarmony vkCreateOHOSSurfaceOpenHarmony;
    static inline PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR fpGetPhysicalDeviceSurfaceCapabilitiesKHR;
    static inline PFN_vkGetPhysicalDeviceSurfacePresentModesKHR fpGetPhysicalDeviceSurfacePresentModesKHR;
    static inline PFN_vkGetPhysicalDeviceSurfaceFormatsKHR fpGetPhysicalDeviceSurfaceFormatsKHR;
    static inline PFN_vkCreateSwapchainKHR fpCreateSwapchainKHR;
    static inline PFN_vkDestroySwapchainKHR fpDestroySwapchainKHR;
    static inline PFN_vkAcquireNextImage2KHR fpAcquireNextImage2KHR;
    static inline PFN_vkQueuePresentKHR fpQueuePresentKHR;
    static inline PFN_IsSupportedVulkan fpIsSupportedVulkan;
    static inline PFN_vkGetPhysicalDeviceQueueFamilyProperties vkGetPhysicalDeviceQueueFamilyProperties;
    static inline PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties;
    static inline PFN_vkGetPhysicalDeviceFeatures vkGetPhysicalDeviceFeatures;
    static inline PFN_vkGetPhysicalDeviceMemoryProperties vkGetPhysicalDeviceMemoryProperties;
    static inline PFN_vkGetPhysicalDeviceSurfaceSupportKHR vkGetPhysicalDeviceSurfaceSupportKHR;
    static inline PFN_vkGetSwapchainImagesKHR fpGetSwapchainImagesKHR;

    static inline void *libVulkan = nullptr;
    static inline VkInstance instance = nullptr;
    static inline VkSurfaceKHR surface = VK_NULL_HANDLE;
    static inline VkPhysicalDevice physicalDevice = nullptr;
    static inline VkDevice device = nullptr;
    static inline VkSurfaceCapabilitiesKHR surfCaps = {};
    static inline VkSurfaceFormatKHR surfaceFormat = {};
    static inline VkSwapchainKHR swapChain = VK_NULL_HANDLE;
    static inline VkSwapchainKHR swapChain2 = VK_NULL_HANDLE;
    static inline VkSemaphore semaphore = VK_NULL_HANDLE;
    static inline bool isSupportedVulkan = false;
    static inline std::vector<VkQueueFamilyProperties> queueProps;
    static inline uint32_t queueCount;
};

VkSwapchainCreateInfoKHR VulkanWrapperUnitTest::getSwapchainCreateInfo(
    VkFormat imageFormat, VkColorSpaceKHR imageColorSpace)
{
        VkSwapchainCreateInfoKHR swapchainCI = {};
        swapchainCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCI.surface = surface;
        uint32_t desiredNumberOfSwapchainImages = surfCaps.minImageCount + 1;
        swapchainCI.minImageCount = desiredNumberOfSwapchainImages;
        swapchainCI.imageFormat = imageFormat;
        swapchainCI.imageColorSpace = imageColorSpace;
        uint32_t width = 1280;
        uint32_t height = 720;
        swapchainCI.imageExtent = { width, height };
        swapchainCI.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchainCI.preTransform = (VkSurfaceTransformFlagBitsKHR)VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        swapchainCI.imageArrayLayers = 1;
        swapchainCI.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainCI.queueFamilyIndexCount = 0;
        swapchainCI.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
        swapchainCI.oldSwapchain = swapChain;
        swapchainCI.clipped = VK_TRUE;
        swapchainCI.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

        return swapchainCI;
}

OHNativeWindow* VulkanWrapperUnitTest::createNativeWindow(std::string name)
{
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
    option->SetWindowName(name.c_str());
    OHOS::sptr<OHOS::Rosen::Window> window = OHOS::Rosen::Window::Create(option->GetWindowName(), option);
    EXPECT_NE(window, nullptr);

    OHOS::Rosen::RSTransaction::FlushImplicitTransaction();
    window->Show();

    auto surfaceNode = window->GetSurfaceNode();
    OHOS::sptr<OHOS::Surface> surf = surfaceNode->GetSurface();
    OHNativeWindow* nativeWindow = CreateNativeWindowFromSurface(&surf);
    return nativeWindow;
}

uint32_t VulkanWrapperUnitTest::getQueueFamilyIndex(VkQueueFlagBits queueFlags)
{
    decltype(queueProps.size()) i = 0;
    if (queueFlags & VK_QUEUE_COMPUTE_BIT) {
        for (i = 0; i < queueProps.size(); i++) {
            if ((queueProps[i].queueFlags & queueFlags) &&
                ((queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0)) {
                return i;
            }
        }
    }
    if (queueFlags & VK_QUEUE_TRANSFER_BIT) {
        for (i = 0; i < queueProps.size(); i++) {
            if ((queueProps[i].queueFlags & queueFlags) &&
                ((queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) &&
                ((queueProps[i].queueFlags & VK_QUEUE_COMPUTE_BIT) == 0)) {
                return i;
            }
        }
    }
    for (i = 0; i < queueProps.size(); i++) {
        if (queueProps[i].queueFlags & queueFlags) {
            return i;
        }
    }
    std::cout << "Could not find a matching queue family index" << std::endl;
    return -1;
}

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

        std::vector<const char*> instanceExtensions = { VK_KHR_SURFACE_EXTENSION_NAME };
        instanceExtensions.push_back(VK_OPENHARMONY_OHOS_SURFACE_EXTENSION_NAME);
        uint32_t extCount = 0;
        VkResult err = vkEnumerateInstanceExtensionProperties(nullptr, &extCount, nullptr);
        EXPECT_EQ(err, VK_SUCCESS);
        if (extCount > 0) {
            std::vector<VkExtensionProperties> extensions(extCount);
            err = vkEnumerateInstanceExtensionProperties(nullptr, &extCount, &extensions.front());
            EXPECT_EQ(err, VK_SUCCESS);
            for (VkExtensionProperties extension : extensions)
            {
                instanceExtensions.push_back(extension.extensionName);
            }
        }

        VkInstanceCreateInfo instanceCreateInfo = {};
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pNext = NULL;
        instanceCreateInfo.pApplicationInfo = &appInfo;

        if (instanceExtensions.size() > 0) {
            instanceCreateInfo.enabledExtensionCount = (uint32_t)instanceExtensions.size();
            instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.data();
        }

        err = vkCreateInstance(&instanceCreateInfo, nullptr, &instance);
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
        vkGetPhysicalDeviceQueueFamilyProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceQueueFamilyProperties>(
            vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceQueueFamilyProperties"));
        EXPECT_NE(vkGetPhysicalDeviceQueueFamilyProperties, nullptr);
        vkGetPhysicalDeviceProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties>(
            vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceProperties"));
        EXPECT_NE(vkGetPhysicalDeviceProperties, nullptr);
        vkGetPhysicalDeviceFeatures = reinterpret_cast<PFN_vkGetPhysicalDeviceFeatures>(
            vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceFeatures"));
        EXPECT_NE(vkGetPhysicalDeviceFeatures, nullptr);
        vkGetPhysicalDeviceMemoryProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceMemoryProperties>(
            vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceMemoryProperties"));
        EXPECT_NE(vkGetPhysicalDeviceMemoryProperties, nullptr);
        vkGetPhysicalDeviceSurfaceSupportKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceSupportKHR>(
            vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceSupportKHR"));
        EXPECT_NE(vkGetPhysicalDeviceSurfaceSupportKHR, nullptr);
    }
}

/**
 * @tc.name: test vkEnumeratePhysicalDevices
 * @tc.desc: test vkEnumeratePhysicalDevices
 * @tc.type: FUNC
 * @tc.require: issueI5ODXM
 */
HWTEST_F(VulkanWrapperUnitTest, vkEnumeratePhysicalDevices_Test, TestSize.Level1)
{
    if (isSupportedVulkan) {
        EXPECT_NE(instance, nullptr);

        uint32_t gpuCount = 0;
        VkResult err = vkEnumeratePhysicalDevices(instance, &gpuCount, nullptr);
        EXPECT_EQ(err, VK_SUCCESS);
        EXPECT_NE(gpuCount, 0);
        std::vector<VkPhysicalDevice> physicalDevices(gpuCount);
        err = vkEnumeratePhysicalDevices(instance, &gpuCount, physicalDevices.data());
        EXPECT_EQ(err, VK_SUCCESS);
        physicalDevice = physicalDevices[0];
        EXPECT_NE(physicalDevice, nullptr);

        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
        vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
        vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &deviceMemoryProperties);
    }
}

/**
 * @tc.name: test vkGetPhysicalDeviceQueueFamilyProperties
 * @tc.desc: test vkGetPhysicalDeviceQueueFamilyProperties
 * @tc.type: FUNC
 * @tc.require: issueI5ODXM
 */
HWTEST_F(VulkanWrapperUnitTest, vkGetPhysicalDeviceQueueFamilyProperties_Test, TestSize.Level1)
{
    if (isSupportedVulkan) {
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, NULL);
        EXPECT_GT(queueCount, 0);

        queueProps.resize(queueCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, queueProps.data());
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
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};
        const float defaultQueuePriority(0.0f);
        VkDeviceQueueCreateInfo queueInfo{};
        queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueInfo.queueFamilyIndex = getQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT);
        queueInfo.queueCount = 1;
        queueInfo.pQueuePriorities = &defaultQueuePriority;
        queueCreateInfos.push_back(queueInfo);
        deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());;
        deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();

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

        OHNativeWindow* nativeWindow = createNativeWindow("createSurfaceUT");
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
 * @tc.name: test vkCreateOHOSSurfaceOpenHarmony 2
 * @tc.desc: test vkCreateOHOSSurfaceOpenHarmony 2
 * @tc.type: FUNC
 * @tc.require: issueI5ODXM
 */
HWTEST_F(VulkanWrapperUnitTest, vkCreateOHOSSurfaceOpenHarmony_Test2, TestSize.Level1)
{
    if (isSupportedVulkan) {
        EXPECT_NE(vkCreateOHOSSurfaceOpenHarmony, nullptr);
        EXPECT_NE(instance, nullptr);

        OHNativeWindow* nativeWindow = createNativeWindow("createSurfaceUT2");
        EXPECT_NE(nativeWindow, nullptr);
        int ret = NativeWindowHandleOpt(nativeWindow, SET_USAGE, 0);
        EXPECT_EQ(ret, OHOS::GSERROR_OK);
        VkOHOSSurfaceCreateInfoOpenHarmony surfaceCreateInfo = {};
        surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_OHOS_SURFACE_CREATE_INFO_OPENHARMONY;
        surfaceCreateInfo.window = nativeWindow;
        VkSurfaceKHR surface2 = VK_NULL_HANDLE;
        VkResult err = vkCreateOHOSSurfaceOpenHarmony(instance, &surfaceCreateInfo, NULL, &surface2);
        EXPECT_NE(err, VK_SUCCESS);
        EXPECT_EQ(surface2, VK_NULL_HANDLE);
    }
}

/**
 * @tc.name: test vkGetPhysicalDeviceSurfaceSupportKHR
 * @tc.desc: test vkGetPhysicalDeviceSurfaceSupportKHR
 * @tc.type: FUNC
 * @tc.require: issueI5ODXM
 */
HWTEST_F(VulkanWrapperUnitTest, vkGetPhysicalDeviceSurfaceSupportKHR_Test, TestSize.Level1)
{
    if (isSupportedVulkan) {
        std::vector<VkBool32> supportsPresent(queueCount);
        for (uint32_t i = 0; i < queueCount; i++) {
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &supportsPresent[i]);
        }
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
        fpAcquireNextImage2KHR = reinterpret_cast<PFN_vkAcquireNextImage2KHR>(
            vkGetDeviceProcAddr(device, "vkAcquireNextImage2KHR"));
        EXPECT_NE(fpAcquireNextImage2KHR, nullptr);
        fpQueuePresentKHR = reinterpret_cast<PFN_vkQueuePresentKHR>(
            vkGetDeviceProcAddr(device, "vkQueuePresentKHR"));
        EXPECT_NE(fpQueuePresentKHR, nullptr);
        fpGetSwapchainImagesKHR = reinterpret_cast<PFN_vkGetSwapchainImagesKHR>(
            vkGetDeviceProcAddr(device, "vkGetSwapchainImagesKHR"));
        EXPECT_NE(fpGetSwapchainImagesKHR, nullptr);
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
 * @tc.name: test fpGetPhysicalDeviceSurfacePresentModesKHR FAIL
 * @tc.desc: test fpGetPhysicalDeviceSurfacePresentModesKHR FAIL
 * @tc.type: FUNC
 * @tc.require: issueI5ODXM
 */
HWTEST_F(VulkanWrapperUnitTest, fpGetPhysicalDeviceSurfacePresentModesKHR_FAIL_Test, TestSize.Level1)
{
    if (isSupportedVulkan) {
        EXPECT_NE(fpGetPhysicalDeviceSurfacePresentModesKHR, nullptr);
        EXPECT_NE(physicalDevice, nullptr);
        EXPECT_NE(surface, VK_NULL_HANDLE);

        uint32_t presentModeCount = 1;
        std::vector<VkPresentModeKHR> presentModes(presentModeCount);
        VkResult err = fpGetPhysicalDeviceSurfacePresentModesKHR(
            physicalDevice, surface, &presentModeCount, presentModes.data());
        EXPECT_NE(err, VK_SUCCESS);
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
 * @tc.name: test fpGetPhysicalDeviceSurfaceFormatsKHR FAIL
 * @tc.desc: test fpGetPhysicalDeviceSurfaceFormatsKHR FAIL
 * @tc.type: FUNC
 * @tc.require: issueI5ODXM
 */
HWTEST_F(VulkanWrapperUnitTest, fpGetPhysicalDeviceSurfaceFormatsKHR_FAIL_Test, TestSize.Level1)
{
    if (isSupportedVulkan) {
        EXPECT_NE(fpGetPhysicalDeviceSurfaceFormatsKHR, nullptr);
        EXPECT_NE(physicalDevice, nullptr);
        EXPECT_NE(surface, VK_NULL_HANDLE);

        uint32_t formatCount = 1;
        std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
        VkResult err = fpGetPhysicalDeviceSurfaceFormatsKHR(
            physicalDevice, surface, &formatCount, surfaceFormats.data());
        EXPECT_NE(err, VK_SUCCESS);
    }
}

/**
 * @tc.name: test fpCreateSwapchainKHR Success
 * @tc.desc: test fpCreateSwapchainKHR Success
 * @tc.type: FUNC
 * @tc.require: issueI5ODXM
 */
HWTEST_F(VulkanWrapperUnitTest, fpCreateSwapchainKHR_Success_Test, TestSize.Level1)
{
    if (isSupportedVulkan) {
        EXPECT_NE(fpCreateSwapchainKHR, nullptr);
        EXPECT_NE(device, nullptr);
        EXPECT_NE(surface, VK_NULL_HANDLE);

        std::vector<VkFormat> pixelFormatArray = {
            VK_FORMAT_R8G8B8A8_UNORM,
            VK_FORMAT_R8G8B8A8_SRGB,
        };
        std::vector<VkColorSpaceKHR> colorDataspaceArray = {
            VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
            VK_COLOR_SPACE_DISPLAY_P3_NONLINEAR_EXT,
            VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT,
            VK_COLOR_SPACE_EXTENDED_SRGB_NONLINEAR_EXT,
            VK_COLOR_SPACE_DCI_P3_LINEAR_EXT,
            VK_COLOR_SPACE_DCI_P3_NONLINEAR_EXT,
            VK_COLOR_SPACE_BT709_LINEAR_EXT,
            VK_COLOR_SPACE_BT709_NONLINEAR_EXT,
            VK_COLOR_SPACE_BT2020_LINEAR_EXT,
            VK_COLOR_SPACE_HDR10_ST2084_EXT,
            VK_COLOR_SPACE_DOLBYVISION_EXT,
            VK_COLOR_SPACE_HDR10_HLG_EXT,
            VK_COLOR_SPACE_ADOBERGB_LINEAR_EXT,
            VK_COLOR_SPACE_ADOBERGB_NONLINEAR_EXT,
            VK_COLORSPACE_SRGB_NONLINEAR_KHR,
            VK_COLOR_SPACE_DCI_P3_LINEAR_EXT
        };

        for (decltype(pixelFormatArray.size()) i = 0; i < pixelFormatArray.size(); i++) {
            for (decltype(colorDataspaceArray.size()) j = 0; j < colorDataspaceArray.size(); j++) {
                VkSwapchainCreateInfoKHR swapchainCI = getSwapchainCreateInfo(
                    pixelFormatArray[i], colorDataspaceArray[j]);

                VkSwapchainKHR swapChainSuccess = VK_NULL_HANDLE;
                VkSwapchainKHR swapChainSuccess2 = VK_NULL_HANDLE;

                VkResult err = fpCreateSwapchainKHR(device, &swapchainCI, nullptr, &swapChainSuccess);
                EXPECT_EQ(err, VK_SUCCESS);
                EXPECT_NE(swapChainSuccess, VK_NULL_HANDLE);

                swapchainCI.oldSwapchain = swapChainSuccess;
                err = fpCreateSwapchainKHR(device, &swapchainCI, nullptr, &swapChainSuccess2);
                EXPECT_EQ(err, VK_SUCCESS);
                EXPECT_NE(swapChainSuccess2, VK_NULL_HANDLE);
                fpDestroySwapchainKHR(device, swapChainSuccess, nullptr);
                fpDestroySwapchainKHR(device, swapChainSuccess2, nullptr);
            }
        }
    }
}

/**
 * @tc.name: test fpCreateSwapchainKHR fail
 * @tc.desc: test fpCreateSwapchainKHR fail
 * @tc.type: FUNC
 * @tc.require: issueI5ODXM
 */
HWTEST_F(VulkanWrapperUnitTest, fpCreateSwapchainKHR_Fail_Test, TestSize.Level1)
{
    if (isSupportedVulkan) {
        EXPECT_NE(fpCreateSwapchainKHR, nullptr);
        EXPECT_NE(device, nullptr);
        EXPECT_NE(surface, VK_NULL_HANDLE);

        std::vector<VkColorSpaceKHR> colorDataspaceArray = {
            VK_COLOR_SPACE_PASS_THROUGH_EXT,
            VK_COLOR_SPACE_DISPLAY_NATIVE_AMD,
            VK_COLOR_SPACE_MAX_ENUM_KHR
        };

        for (decltype(colorDataspaceArray.size()) i = 0; i < colorDataspaceArray.size(); i++) {
            VkSwapchainCreateInfoKHR swapchainCI = getSwapchainCreateInfo(
                VK_FORMAT_R8G8B8A8_UNORM, colorDataspaceArray[i]);

            VkSwapchainKHR swapChainFail = VK_NULL_HANDLE;
            VkSwapchainKHR swapChainFail2 = VK_NULL_HANDLE;

            VkResult err = fpCreateSwapchainKHR(device, &swapchainCI, nullptr, &swapChainFail);
            EXPECT_NE(err, VK_SUCCESS);
            EXPECT_EQ(swapChainFail, VK_NULL_HANDLE);

            swapchainCI.oldSwapchain = swapChainFail;
            err = fpCreateSwapchainKHR(device, &swapchainCI, nullptr, &swapChainFail2);
            EXPECT_NE(err, VK_SUCCESS);
            EXPECT_EQ(swapChainFail2, VK_NULL_HANDLE);
            fpDestroySwapchainKHR(device, swapChainFail, nullptr);
            fpDestroySwapchainKHR(device, swapChainFail2, nullptr);
        }
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

        VkSwapchainCreateInfoKHR swapchainCI = getSwapchainCreateInfo(
            VK_FORMAT_B8G8R8A8_UNORM, surfaceFormat.colorSpace);

        VkResult err = fpCreateSwapchainKHR(device, &swapchainCI, nullptr, &swapChain);
        EXPECT_EQ(err, VK_SUCCESS);
        EXPECT_NE(swapChain, VK_NULL_HANDLE);

        swapchainCI.oldSwapchain = swapChain;
        err = fpCreateSwapchainKHR(device, &swapchainCI, nullptr, &swapChain2);
        EXPECT_EQ(err, VK_SUCCESS);
        EXPECT_NE(swapChain2, VK_NULL_HANDLE);
    }
}

/**
 * @tc.name: test fpGetSwapchainImagesKHR
 * @tc.desc: test fpGetSwapchainImagesKHR
 * @tc.type: FUNC
 * @tc.require: issueI5ODXM
 */
HWTEST_F(VulkanWrapperUnitTest, fpGetSwapchainImagesKHR_Test, TestSize.Level1)
{
    if (isSupportedVulkan) {
        uint32_t imageCount;
        std::vector<VkImage> images;
        VkResult err = fpGetSwapchainImagesKHR(device, swapChain, &imageCount, NULL);
        EXPECT_EQ(err, VK_SUCCESS);
        images.resize(imageCount);
        err = fpGetSwapchainImagesKHR(device, swapChain, &imageCount, images.data());
        EXPECT_EQ(err, VK_SUCCESS);
    }
}

/**
 * @tc.name: test fpGetSwapchainImagesKHR FAIL
 * @tc.desc: test fpGetSwapchainImagesKHR FAIL
 * @tc.type: FUNC
 * @tc.require: issueI5ODXM
 */
HWTEST_F(VulkanWrapperUnitTest, fpGetSwapchainImagesKHR_FAIL_Test, TestSize.Level1)
{
    if (isSupportedVulkan) {
        uint32_t imageCount = 1;
        std::vector<VkImage> images(imageCount);
        VkResult err = fpGetSwapchainImagesKHR(device, swapChain2, &imageCount, images.data());
        EXPECT_NE(err, VK_SUCCESS);
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

/**
 * @tc.name: test fpAcquireNextImage2KHR
 * @tc.desc: test fpAcquireNextImage2KHR
 * @tc.type: FUNC
 * @tc.require: issueI5ODXM
 */
HWTEST_F(VulkanWrapperUnitTest, fpAcquireNextImage2KHR_Test, TestSize.Level1)
{
    if (isSupportedVulkan) {
        VkAcquireNextImageInfoKHR pAcquireInfo;
        pAcquireInfo.swapchain = swapChain2;
        pAcquireInfo.timeout = UINT64_MAX;
        pAcquireInfo.semaphore = semaphore;
        pAcquireInfo.fence = (VkFence)nullptr;
        uint32_t imageIndex = 0;
        VkResult err = fpAcquireNextImage2KHR(device, &pAcquireInfo, &imageIndex);
        EXPECT_EQ(err, VK_SUCCESS);
    }
}

/**
 * @tc.name: test fpQueuePresentKHR
 * @tc.desc: test fpQueuePresentKHR
 * @tc.type: FUNC
 * @tc.require: issueI5ODXM
 */
HWTEST_F(VulkanWrapperUnitTest, fpQueuePresentKHR_Test, TestSize.Level1)
{
    if (isSupportedVulkan) {
        VkRectLayerKHR pRectangles = {};

        std::vector<VkPresentRegionKHR> pRegions;
        VkPresentRegionKHR pRegion;
        pRegion.rectangleCount = 1;
        pRegion.pRectangles = &pRectangles;
        pRegions.push_back(pRegion);

        VkPresentRegionsKHR presentRegions;
        presentRegions.sType = VK_STRUCTURE_TYPE_PRESENT_REGIONS_KHR;
        presentRegions.pNext = NULL;
        presentRegions.swapchainCount = 1;
        presentRegions.pRegions = pRegions.data();

        VkQueue queue = nullptr;
        PFN_vkGetDeviceQueue vkGetDeviceQueue = reinterpret_cast<PFN_vkGetDeviceQueue>(
            vkGetInstanceProcAddr(instance, "vkGetDeviceQueue"));
        EXPECT_NE(vkGetDeviceQueue, nullptr);
        vkGetDeviceQueue(device, 0, 0, &queue);
        EXPECT_NE(queue, nullptr);
        uint32_t imageIndex = 0;
        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.pNext = &presentRegions;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &swapChain2;
        presentInfo.pImageIndices = &imageIndex;
        EXPECT_NE(semaphore, VK_NULL_HANDLE);
        presentInfo.pWaitSemaphores = &semaphore;
        presentInfo.waitSemaphoreCount = 1;
        VkResult err = fpQueuePresentKHR(queue, &presentInfo);
        EXPECT_EQ(err, VK_SUCCESS);
    }
}

/**
 * @tc.name: test vkDestroySurfaceKHR nullptr
 * @tc.desc: test vkDestroySurfaceKHR nullptr
 * @tc.type: FUNC
 * @tc.require: issueI5ODXM
 */
HWTEST_F(VulkanWrapperUnitTest, DestroySurface_NULL_Test, TestSize.Level1)
{
    if (isSupportedVulkan) {
        EXPECT_NE(vkDestroySurfaceKHR, nullptr);
        vkDestroySurfaceKHR(instance, VK_NULL_HANDLE, nullptr);
    }
}
} // vulkan::driver
