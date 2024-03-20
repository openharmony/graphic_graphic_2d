/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include <sstream>

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
class VulkanLoaderUnitTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase();
    void SetUp() {}
    void TearDown() {}
    uint32_t GetQueueFamilyIndex(VkQueueFlagBits queueFlags);
    OHNativeWindow* CreateNativeWindow(std::string name);
    VkSwapchainCreateInfoKHR GetSwapchainCreateInfo(VkFormat imageFormat, VkColorSpaceKHR imageColorSpace);
    static VkBool32 UserCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);

    static inline void DLOpenLibVulkan();
    static inline void TrytoCreateVkInstance();

    static inline PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties;
    static inline PFN_vkEnumerateDeviceExtensionProperties vkEnumerateDeviceExtensionProperties;
    static inline PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties;
    static inline PFN_vkEnumerateDeviceLayerProperties vkEnumerateDeviceLayerProperties;
    static inline PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT;
    static inline PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT;
    static inline PFN_vkCreateInstance vkCreateInstance;
    static inline PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr;
    static inline PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr;

    static inline PFN_vkDestroyInstance vkDestroyInstance;
    static inline PFN_vkDestroySurfaceKHR vkDestroySurfaceKHR;
    static inline PFN_vkCreateDevice vkCreateDevice;
    static inline PFN_vkDestroyDevice vkDestroyDevice;
    static inline PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices;
    static inline PFN_vkCreateSurfaceOHOS vkCreateSurfaceOHOS;
    static inline PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR fpGetPhysicalDeviceSurfaceCapabilitiesKHR;
    static inline PFN_vkGetPhysicalDeviceSurfacePresentModesKHR fpGetPhysicalDeviceSurfacePresentModesKHR;
    static inline PFN_vkGetPhysicalDeviceSurfaceFormatsKHR fpGetPhysicalDeviceSurfaceFormatsKHR;
    static inline PFN_vkCreateSwapchainKHR fpCreateSwapchainKHR;
    static inline PFN_vkDestroySwapchainKHR fpDestroySwapchainKHR;
    static inline PFN_vkAcquireNextImage2KHR fpAcquireNextImage2KHR;
    static inline PFN_vkQueuePresentKHR fpQueuePresentKHR;
    static inline PFN_vkGetPhysicalDeviceQueueFamilyProperties vkGetPhysicalDeviceQueueFamilyProperties;
    static inline PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties;
    static inline PFN_vkGetPhysicalDeviceFeatures vkGetPhysicalDeviceFeatures;
    static inline PFN_vkGetPhysicalDeviceMemoryProperties vkGetPhysicalDeviceMemoryProperties;
    static inline PFN_vkGetPhysicalDeviceSurfaceSupportKHR vkGetPhysicalDeviceSurfaceSupportKHR;
    static inline PFN_vkGetSwapchainImagesKHR fpGetSwapchainImagesKHR;

    static inline void *libVulkan_ = nullptr;
    static inline VkInstance instance_ = nullptr;
    static inline VkSurfaceKHR surface_ = VK_NULL_HANDLE;
    static inline VkPhysicalDevice physicalDevice_ = nullptr;
    static inline VkDevice device_ = nullptr;
    static inline VkSurfaceCapabilitiesKHR surfCaps_ = {};
    static inline VkSurfaceFormatKHR surfaceFormat_ = {};
    static inline VkSwapchainKHR swapChain_ = VK_NULL_HANDLE;
    static inline VkSwapchainKHR swapChain2_ = VK_NULL_HANDLE;
    static inline VkSemaphore semaphore_ = VK_NULL_HANDLE;
    static inline bool isSupportedVulkan_ = false;
    static inline std::vector<VkQueueFamilyProperties> queueProps_;
    static inline uint32_t queueCount_;
    static inline VkDebugUtilsMessengerEXT debugUtilsMessenger = VK_NULL_HANDLE;
    static inline std::stringstream debugMessage_;
};

void VulkanLoaderUnitTest::DLOpenLibVulkan()
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

void VulkanLoaderUnitTest::TrytoCreateVkInstance()
{
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "pApplicationName";
    appInfo.pEngineName = "pEngineName";
    appInfo.apiVersion = VK_API_VERSION_1_0;

    std::vector<const char*> instanceExtensions = {
        VK_KHR_SURFACE_EXTENSION_NAME,
        VK_OHOS_SURFACE_EXTENSION_NAME,
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME
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

VkSwapchainCreateInfoKHR VulkanLoaderUnitTest::GetSwapchainCreateInfo(
    VkFormat imageFormat, VkColorSpaceKHR imageColorSpace)
{
        VkSwapchainCreateInfoKHR swapchainCI = {};
        swapchainCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCI.surface = surface_;
        uint32_t desiredNumberOfSwapchainImages = surfCaps_.minImageCount + 1;
        if ((surfCaps_.maxImageCount > 0) &&
            (desiredNumberOfSwapchainImages > surfCaps_.maxImageCount)) {
            desiredNumberOfSwapchainImages = surfCaps_.maxImageCount;
        }
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
        swapchainCI.oldSwapchain = swapChain_;
        swapchainCI.clipped = VK_TRUE;
        swapchainCI.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

        return swapchainCI;
}

OHNativeWindow* VulkanLoaderUnitTest::CreateNativeWindow(std::string name)
{
    struct RSSurfaceNodeConfig rsSurfaceNodeConfig;
    rsSurfaceNodeConfig.SurfaceNodeName = "createSurface_test";
    auto surfaceNode = RSSurfaceNode::Create(rsSurfaceNodeConfig, RSSurfaceNodeType::DEFAULT);
    OHOS::sptr<OHOS::Surface> surf = surfaceNode->GetSurface();
    OHNativeWindow* nativeWindow = CreateNativeWindowFromSurface(&surf);
    return nativeWindow;
}

uint32_t VulkanLoaderUnitTest::GetQueueFamilyIndex(VkQueueFlagBits queueFlags)
{
    decltype(queueProps_.size()) i = 0;
    if (queueFlags & VK_QUEUE_COMPUTE_BIT) {
        for (i = 0; i < queueProps_.size(); i++) {
            if ((queueProps_[i].queueFlags & queueFlags) &&
                ((queueProps_[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0)) {
                return i;
            }
        }
    }
    if (queueFlags & VK_QUEUE_TRANSFER_BIT) {
        for (i = 0; i < queueProps_.size(); i++) {
            if ((queueProps_[i].queueFlags & queueFlags) &&
                ((queueProps_[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) &&
                ((queueProps_[i].queueFlags & VK_QUEUE_COMPUTE_BIT) == 0)) {
                return i;
            }
        }
    }
    for (i = 0; i < queueProps_.size(); i++) {
        if (queueProps_[i].queueFlags & queueFlags) {
            return i;
        }
    }
    std::cout << "Could not find a matching queue family index" << std::endl;
    return -1;
}

VkBool32 VulkanLoaderUnitTest::UserCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    std::string prefix("");
    if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        prefix = "ERROR: ";
    } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        prefix = "WARN: ";
    } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
        prefix = "INFO: ";
    } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
        prefix = "DEBUG: ";
    }
    debugMessage_ << prefix << "[" << pCallbackData->messageIdNumber << "]["
        << pCallbackData->pMessageIdName << "] : " << pCallbackData->pMessage;
    return VK_FALSE;
}

void VulkanLoaderUnitTest::TearDownTestCase()
{
    if (device_ != nullptr) {
        if (swapChain_ != VK_NULL_HANDLE) {
            fpDestroySwapchainKHR(device_, swapChain_, nullptr);
        }
        vkDestroyDevice(device_, nullptr);
    }
    if (instance_ != nullptr) {
        if (surface_ != VK_NULL_HANDLE) {
            vkDestroySurfaceKHR(instance_, surface_, nullptr);
        }
        vkDestroyInstance(instance_, nullptr);
    }
}

/**
 * @tc.name: Load base Vulkan functions
 * @tc.desc: Load base Vulkan functions
 * @tc.type: FUNC
 * @tc.require: issueI6SKRO
 */
HWTEST_F(VulkanLoaderUnitTest, LoadBaseFuncPtr, TestSize.Level1)
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
        vkEnumerateDeviceExtensionProperties = reinterpret_cast<PFN_vkEnumerateDeviceExtensionProperties>(
            dlsym(libVulkan_, "vkEnumerateDeviceExtensionProperties"));
        EXPECT_NE(vkEnumerateDeviceExtensionProperties, nullptr);
        vkEnumerateDeviceLayerProperties = reinterpret_cast<PFN_vkEnumerateDeviceLayerProperties>(
            dlsym(libVulkan_, "vkEnumerateDeviceLayerProperties"));
        EXPECT_NE(vkEnumerateDeviceLayerProperties, nullptr);
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
 * @tc.name: test vkEnumerateInstanceExtensionProperties
 * @tc.desc: test vkEnumerateInstanceExtensionProperties
 * @tc.type: FUNC
 * @tc.require: issueI6SKRO
 */
HWTEST_F(VulkanLoaderUnitTest, vkEnumerateInstanceExtensionProperties_Test, TestSize.Level1)
{
    if (isSupportedVulkan_) {
        uint32_t extCount = 0;
        VkResult err = vkEnumerateInstanceExtensionProperties(nullptr, &extCount, nullptr);
        EXPECT_EQ(err, VK_SUCCESS);
        if (extCount > 0) {
            std::vector<VkExtensionProperties> extensions(extCount);
            err = vkEnumerateInstanceExtensionProperties(nullptr, &extCount, extensions.data());
            EXPECT_EQ(err, VK_SUCCESS);
        }
    }
}

/**
 * @tc.name: test vkEnumerateInstanceLayerProperties
 * @tc.desc: test vkEnumerateInstanceLayerProperties
 * @tc.type: FUNC
 * @tc.require: issueI6SKRO
 */
HWTEST_F(VulkanLoaderUnitTest, vkEnumerateInstanceLayerProperties_Test, TestSize.Level1)
{
    if (isSupportedVulkan_) {
        uint32_t propertyCount = 0;
        VkResult err = vkEnumerateInstanceLayerProperties(&propertyCount, nullptr);
        EXPECT_EQ(err, VK_SUCCESS);
        if (propertyCount > 0) {
            std::vector<VkLayerProperties> properties(propertyCount);
            err = vkEnumerateInstanceLayerProperties(&propertyCount, properties.data());
            EXPECT_EQ(err, VK_SUCCESS);
        }
    }
}

/**
 * @tc.name: load instance based function pointer
 * @tc.desc: load instance based function pointer
 * @tc.type: FUNC
 * @tc.require: issueI6SKRO
 */
HWTEST_F(VulkanLoaderUnitTest, LoadInstanceFuncPtr, TestSize.Level1)
{
    if (isSupportedVulkan_) {
        vkDestroyInstance = reinterpret_cast<PFN_vkDestroyInstance>(
            vkGetInstanceProcAddr(instance_, "vkDestroyInstance"));
        EXPECT_NE(vkDestroyInstance, nullptr);
        vkEnumeratePhysicalDevices = reinterpret_cast<PFN_vkEnumeratePhysicalDevices>(
            vkGetInstanceProcAddr(instance_, "vkEnumeratePhysicalDevices"));
        EXPECT_NE(vkEnumeratePhysicalDevices, nullptr);
        vkCreateDevice = reinterpret_cast<PFN_vkCreateDevice>(vkGetInstanceProcAddr(instance_, "vkCreateDevice"));
        EXPECT_NE(vkCreateDevice, nullptr);
        vkDestroyDevice = reinterpret_cast<PFN_vkDestroyDevice>(vkGetInstanceProcAddr(instance_, "vkDestroyDevice"));
        EXPECT_NE(vkDestroyDevice, nullptr);
        vkDestroySurfaceKHR = reinterpret_cast<PFN_vkDestroySurfaceKHR>(
            vkGetInstanceProcAddr(instance_, "vkDestroySurfaceKHR"));
        EXPECT_NE(vkDestroySurfaceKHR, nullptr);

        vkCreateSurfaceOHOS = reinterpret_cast<PFN_vkCreateSurfaceOHOS>(
            vkGetInstanceProcAddr(instance_, "vkCreateSurfaceOHOS"));
        EXPECT_NE(vkCreateSurfaceOHOS, nullptr);

        fpGetPhysicalDeviceSurfaceCapabilitiesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR>(
            vkGetInstanceProcAddr(instance_, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR"));
        EXPECT_NE(fpGetPhysicalDeviceSurfaceCapabilitiesKHR, nullptr);
        fpGetPhysicalDeviceSurfacePresentModesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfacePresentModesKHR>(
            vkGetInstanceProcAddr(instance_, "vkGetPhysicalDeviceSurfacePresentModesKHR"));
        EXPECT_NE(fpGetPhysicalDeviceSurfacePresentModesKHR, nullptr);
        fpGetPhysicalDeviceSurfaceFormatsKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceFormatsKHR>(
            vkGetInstanceProcAddr(instance_, "vkGetPhysicalDeviceSurfaceFormatsKHR"));
        EXPECT_NE(fpGetPhysicalDeviceSurfaceFormatsKHR, nullptr);
        vkGetPhysicalDeviceQueueFamilyProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceQueueFamilyProperties>(
            vkGetInstanceProcAddr(instance_, "vkGetPhysicalDeviceQueueFamilyProperties"));
        EXPECT_NE(vkGetPhysicalDeviceQueueFamilyProperties, nullptr);
        vkGetPhysicalDeviceProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties>(
            vkGetInstanceProcAddr(instance_, "vkGetPhysicalDeviceProperties"));
        EXPECT_NE(vkGetPhysicalDeviceProperties, nullptr);
        vkGetPhysicalDeviceFeatures = reinterpret_cast<PFN_vkGetPhysicalDeviceFeatures>(
            vkGetInstanceProcAddr(instance_, "vkGetPhysicalDeviceFeatures"));
        EXPECT_NE(vkGetPhysicalDeviceFeatures, nullptr);
        vkGetPhysicalDeviceMemoryProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceMemoryProperties>(
            vkGetInstanceProcAddr(instance_, "vkGetPhysicalDeviceMemoryProperties"));
        EXPECT_NE(vkGetPhysicalDeviceMemoryProperties, nullptr);
        vkGetPhysicalDeviceSurfaceSupportKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceSupportKHR>(
            vkGetInstanceProcAddr(instance_, "vkGetPhysicalDeviceSurfaceSupportKHR"));
        EXPECT_NE(vkGetPhysicalDeviceSurfaceSupportKHR, nullptr);
        vkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(instance_, "vkCreateDebugUtilsMessengerEXT"));
        EXPECT_NE(vkCreateDebugUtilsMessengerEXT, nullptr);
        vkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(instance_, "vkDestroyDebugUtilsMessengerEXT"));
        EXPECT_NE(vkDestroyDebugUtilsMessengerEXT, nullptr);
    }
}

/**
 * @tc.name: test vkEnumeratePhysicalDevices
 * @tc.desc: test vkEnumeratePhysicalDevices
 * @tc.type: FUNC
 * @tc.require: issueI6SKRO
 */
HWTEST_F(VulkanLoaderUnitTest, vkEnumeratePhysicalDevices_Test, TestSize.Level1)
{
    if (isSupportedVulkan_) {
        EXPECT_NE(instance_, nullptr);

        uint32_t gpuCount = 0;
        VkResult err = vkEnumeratePhysicalDevices(instance_, &gpuCount, nullptr);
        EXPECT_EQ(err, VK_SUCCESS);
        EXPECT_NE(gpuCount, 0);
        std::vector<VkPhysicalDevice> physicalDevices(gpuCount);
        err = vkEnumeratePhysicalDevices(instance_, &gpuCount, physicalDevices.data());
        EXPECT_EQ(err, VK_SUCCESS);
        physicalDevice_ = physicalDevices[0];
        EXPECT_NE(physicalDevice_, nullptr);

        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
        vkGetPhysicalDeviceProperties(physicalDevice_, &deviceProperties);
        vkGetPhysicalDeviceFeatures(physicalDevice_, &deviceFeatures);
        vkGetPhysicalDeviceMemoryProperties(physicalDevice_, &deviceMemoryProperties);
    }
}

/**
 * @tc.name: test vkEnumerateDeviceExtensionProperties
 * @tc.desc: test vkEnumerateDeviceExtensionProperties
 * @tc.type: FUNC
 * @tc.require: issueI6SKRO
 */
HWTEST_F(VulkanLoaderUnitTest, vkEnumerateDeviceExtensionProperties_Test, TestSize.Level1)
{
    if (isSupportedVulkan_) {
        uint32_t extCount = 0;
        VkResult err = vkEnumerateDeviceExtensionProperties(physicalDevice_, nullptr, &extCount, nullptr);
        EXPECT_EQ(err, VK_SUCCESS);
        if (extCount > 0) {
            std::vector<VkExtensionProperties> extensions(extCount);
            err = vkEnumerateDeviceExtensionProperties(physicalDevice_, nullptr, &extCount, extensions.data());
            EXPECT_EQ(err, VK_SUCCESS);
        }
    }
}

/**
 * @tc.name: test vkEnumerateDeviceLayerProperties
 * @tc.desc: test vkEnumerateDeviceLayerProperties
 * @tc.type: FUNC
 * @tc.require: issueI6SKRO
 */
HWTEST_F(VulkanLoaderUnitTest, vkEnumerateDeviceLayerProperties_Test, TestSize.Level1)
{
    if (isSupportedVulkan_) {
        uint32_t propertyCount = 0;
        VkResult err = vkEnumerateDeviceLayerProperties(physicalDevice_, &propertyCount, nullptr);
        EXPECT_EQ(err, VK_SUCCESS);
        if (propertyCount > 0) {
            std::vector<VkLayerProperties> properties(propertyCount);
            err = vkEnumerateDeviceLayerProperties(physicalDevice_, &propertyCount, properties.data());
            EXPECT_EQ(err, VK_SUCCESS);
        }
    }
}

/**
 * @tc.name: test vkGetPhysicalDeviceQueueFamilyProperties
 * @tc.desc: test vkGetPhysicalDeviceQueueFamilyProperties
 * @tc.type: FUNC
 * @tc.require: issueI6SKRO
 */
HWTEST_F(VulkanLoaderUnitTest, vkGetPhysicalDeviceQueueFamilyProperties_Test, TestSize.Level1)
{
    if (isSupportedVulkan_) {
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice_, &queueCount_, NULL);
        EXPECT_GT(queueCount_, 0);

        queueProps_.resize(queueCount_);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice_, &queueCount_, queueProps_.data());
    }
}

/**
 * @tc.name: test vkCreateDevice
 * @tc.desc: test vkCreateDevice
 * @tc.type: FUNC
 * @tc.require: issueI6SKRO
 */
HWTEST_F(VulkanLoaderUnitTest, vkCreateDevice_Test, TestSize.Level1)
{
    if (isSupportedVulkan_) {
        EXPECT_NE(vkCreateDevice, nullptr);
        EXPECT_NE(physicalDevice_, nullptr);

        VkDeviceCreateInfo deviceCreateInfo = {};
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};
        const float defaultQueuePriority(0.0f);
        VkDeviceQueueCreateInfo queueInfo{};
        queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueInfo.queueFamilyIndex = GetQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT);
        queueInfo.queueCount = 1;
        queueInfo.pQueuePriorities = &defaultQueuePriority;
        queueCreateInfos.push_back(queueInfo);
        deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();

        std::vector<const char*> deviceExtensions;
        deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
        deviceCreateInfo.enabledExtensionCount = (uint32_t)deviceExtensions.size();
        deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
        VkDevice logicalDevice;
        VkResult err = vkCreateDevice(physicalDevice_, &deviceCreateInfo, nullptr, &logicalDevice);
        EXPECT_EQ(err, VK_SUCCESS);
        EXPECT_NE(logicalDevice, nullptr);
        device_ = logicalDevice;
    }
}

/**
 * @tc.name: test vkCreateSurfaceOHOS
 * @tc.desc: test vkCreateSurfaceOHOS
 * @tc.type: FUNC
 * @tc.require: issueI6SKRO
 */
HWTEST_F(VulkanLoaderUnitTest, vkCreateSurfaceOHOS_Test, TestSize.Level1)
{
    if (isSupportedVulkan_) {
        EXPECT_NE(vkCreateSurfaceOHOS, nullptr);
        EXPECT_NE(instance_, nullptr);

        OHNativeWindow* nativeWindow = CreateNativeWindow("createSurfaceUT");
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
 * @tc.name: test vkCreateSurfaceOHOS 2
 * @tc.desc: test vkCreateSurfaceOHOS 2
 * @tc.type: FUNC
 * @tc.require: issueI6SKRO
 */
HWTEST_F(VulkanLoaderUnitTest, vkCreateSurfaceOHOS_Test2, TestSize.Level1)
{
    if (isSupportedVulkan_) {
        EXPECT_NE(vkCreateSurfaceOHOS, nullptr);
        EXPECT_NE(instance_, nullptr);

        OHNativeWindow* nativeWindow = CreateNativeWindow("createSurfaceUT2");
        EXPECT_NE(nativeWindow, nullptr);
        int ret = NativeWindowHandleOpt(nativeWindow, SET_USAGE, 0);
        EXPECT_EQ(ret, OHOS::GSERROR_OK);
        VkSurfaceCreateInfoOHOS surfaceCreateInfo = {};
        surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_SURFACE_CREATE_INFO_OHOS;
        surfaceCreateInfo.window = nativeWindow;
        VkSurfaceKHR surface2 = VK_NULL_HANDLE;
        VkResult err = vkCreateSurfaceOHOS(instance_, &surfaceCreateInfo, NULL, &surface2);
        EXPECT_NE(err, VK_SUCCESS);
        EXPECT_EQ(surface2, VK_NULL_HANDLE);
    }
}

/**
 * @tc.name: test vkGetPhysicalDeviceSurfaceSupportKHR
 * @tc.desc: test vkGetPhysicalDeviceSurfaceSupportKHR
 * @tc.type: FUNC
 * @tc.require: issueI6SKRO
 */
HWTEST_F(VulkanLoaderUnitTest, vkGetPhysicalDeviceSurfaceSupportKHR_Test, TestSize.Level1)
{
    if (isSupportedVulkan_) {
        std::vector<VkBool32> supportsPresent(queueCount_);
        for (uint32_t i = 0; i < queueCount_; i++) {
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice_, i, surface_, &supportsPresent[i]);
        }
    }
}

/**
 * @tc.name: load device based function pointer
 * @tc.desc: load device based function pointer
 * @tc.type: FUNC
 * @tc.require: issueI6SKRO
 */
HWTEST_F(VulkanLoaderUnitTest, LoadDeviceFuncPtr, TestSize.Level1)
{
    if (isSupportedVulkan_) {
        EXPECT_NE(device_, nullptr);

        fpCreateSwapchainKHR = reinterpret_cast<PFN_vkCreateSwapchainKHR>(
            vkGetDeviceProcAddr(device_, "vkCreateSwapchainKHR"));
        EXPECT_NE(fpCreateSwapchainKHR, nullptr);
        fpDestroySwapchainKHR = reinterpret_cast<PFN_vkDestroySwapchainKHR>(
            vkGetDeviceProcAddr(device_, "vkDestroySwapchainKHR"));
        EXPECT_NE(fpDestroySwapchainKHR, nullptr);
        fpAcquireNextImage2KHR = reinterpret_cast<PFN_vkAcquireNextImage2KHR>(
            vkGetDeviceProcAddr(device_, "vkAcquireNextImage2KHR"));
        EXPECT_NE(fpAcquireNextImage2KHR, nullptr);
        fpQueuePresentKHR = reinterpret_cast<PFN_vkQueuePresentKHR>(
            vkGetDeviceProcAddr(device_, "vkQueuePresentKHR"));
        EXPECT_NE(fpQueuePresentKHR, nullptr);
        fpGetSwapchainImagesKHR = reinterpret_cast<PFN_vkGetSwapchainImagesKHR>(
            vkGetDeviceProcAddr(device_, "vkGetSwapchainImagesKHR"));
        EXPECT_NE(fpGetSwapchainImagesKHR, nullptr);
    }
}

/**
 * @tc.name: test fpGetPhysicalDeviceSurfaceCapabilitiesKHR
 * @tc.desc: test fpGetPhysicalDeviceSurfaceCapabilitiesKHR
 * @tc.type: FUNC
 * @tc.require: issueI6SKRO
 */
HWTEST_F(VulkanLoaderUnitTest, fpGetPhysicalDeviceSurfaceCapabilitiesKHR_Test, TestSize.Level1)
{
    if (isSupportedVulkan_) {
        EXPECT_NE(fpGetPhysicalDeviceSurfaceCapabilitiesKHR, nullptr);
        EXPECT_NE(physicalDevice_, nullptr);
        EXPECT_NE(surface_, VK_NULL_HANDLE);

        VkResult err = fpGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice_, surface_, &surfCaps_);
        EXPECT_EQ(err, VK_SUCCESS);
    }
}

/**
 * @tc.name: test fpGetPhysicalDeviceSurfacePresentModesKHR
 * @tc.desc: test fpGetPhysicalDeviceSurfacePresentModesKHR
 * @tc.type: FUNC
 * @tc.require: issueI6SKRO
 */
HWTEST_F(VulkanLoaderUnitTest, fpGetPhysicalDeviceSurfacePresentModesKHR_Test, TestSize.Level1)
{
    if (isSupportedVulkan_) {
        EXPECT_NE(fpGetPhysicalDeviceSurfacePresentModesKHR, nullptr);
        EXPECT_NE(physicalDevice_, nullptr);
        EXPECT_NE(surface_, VK_NULL_HANDLE);

        uint32_t presentModeCount;
        VkResult err = fpGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice_, surface_, &presentModeCount, NULL);
        EXPECT_EQ(err, VK_SUCCESS);
        EXPECT_GT(presentModeCount, 0);

        std::vector<VkPresentModeKHR> presentModes(presentModeCount);
        err = fpGetPhysicalDeviceSurfacePresentModesKHR(
            physicalDevice_, surface_, &presentModeCount, presentModes.data());
        EXPECT_EQ(err, VK_SUCCESS);
    }
}

/**
 * @tc.name: test fpGetPhysicalDeviceSurfacePresentModesKHR FAIL
 * @tc.desc: test fpGetPhysicalDeviceSurfacePresentModesKHR FAIL
 * @tc.type: FUNC
 * @tc.require: issueI6SKRO
 */
HWTEST_F(VulkanLoaderUnitTest, fpGetPhysicalDeviceSurfacePresentModesKHR_FAIL_Test, TestSize.Level1)
{
    if (isSupportedVulkan_) {
        EXPECT_NE(fpGetPhysicalDeviceSurfacePresentModesKHR, nullptr);
        EXPECT_NE(physicalDevice_, nullptr);
        EXPECT_NE(surface_, VK_NULL_HANDLE);

        uint32_t presentModeCount = 1;
        std::vector<VkPresentModeKHR> presentModes(presentModeCount);
        VkResult err = fpGetPhysicalDeviceSurfacePresentModesKHR(
            physicalDevice_, surface_, &presentModeCount, presentModes.data());
        EXPECT_NE(err, VK_SUCCESS);
    }
}

/**
 * @tc.name: test fpGetPhysicalDeviceSurfaceFormatsKHR
 * @tc.desc: test fpGetPhysicalDeviceSurfaceFormatsKHR
 * @tc.type: FUNC
 * @tc.require: issueI6SKRO
 */
HWTEST_F(VulkanLoaderUnitTest, fpGetPhysicalDeviceSurfaceFormatsKHR_Test, TestSize.Level1)
{
    if (isSupportedVulkan_) {
        EXPECT_NE(fpGetPhysicalDeviceSurfaceFormatsKHR, nullptr);
        EXPECT_NE(physicalDevice_, nullptr);
        EXPECT_NE(surface_, VK_NULL_HANDLE);

        uint32_t formatCount;
        VkResult err = fpGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice_, surface_, &formatCount, NULL);
        EXPECT_EQ(err, VK_SUCCESS);
        EXPECT_GT(formatCount, 0);
        std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
        err = fpGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice_, surface_, &formatCount, surfaceFormats.data());
        EXPECT_EQ(err, VK_SUCCESS);
        surfaceFormat_ = surfaceFormats[0];
    }
}

/**
 * @tc.name: test fpGetPhysicalDeviceSurfaceFormatsKHR FAIL
 * @tc.desc: test fpGetPhysicalDeviceSurfaceFormatsKHR FAIL
 * @tc.type: FUNC
 * @tc.require: issueI6SKRO
 */
HWTEST_F(VulkanLoaderUnitTest, fpGetPhysicalDeviceSurfaceFormatsKHR_FAIL_Test, TestSize.Level1)
{
    if (isSupportedVulkan_) {
        EXPECT_NE(fpGetPhysicalDeviceSurfaceFormatsKHR, nullptr);
        EXPECT_NE(physicalDevice_, nullptr);
        EXPECT_NE(surface_, VK_NULL_HANDLE);

        uint32_t formatCount = 1;
        std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
        VkResult err = fpGetPhysicalDeviceSurfaceFormatsKHR(
            physicalDevice_, surface_, &formatCount, surfaceFormats.data());
        EXPECT_NE(err, VK_SUCCESS);
    }
}

/**
 * @tc.name: test fpCreateSwapchainKHR Success
 * @tc.desc: test fpCreateSwapchainKHR Success
 * @tc.type: FUNC
 * @tc.require: issueI6SKRO
 */
HWTEST_F(VulkanLoaderUnitTest, fpCreateSwapchainKHR_Success_Test, TestSize.Level1)
{
    if (isSupportedVulkan_) {
        EXPECT_NE(fpCreateSwapchainKHR, nullptr);
        EXPECT_NE(device_, nullptr);
        EXPECT_NE(surface_, VK_NULL_HANDLE);

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
                VkSwapchainCreateInfoKHR swapchainCI = GetSwapchainCreateInfo(
                    pixelFormatArray[i], colorDataspaceArray[j]);

                VkSwapchainKHR swapChainSuccess = VK_NULL_HANDLE;
                VkSwapchainKHR swapChainSuccess2 = VK_NULL_HANDLE;

                VkResult err = fpCreateSwapchainKHR(device_, &swapchainCI, nullptr, &swapChainSuccess);
                EXPECT_EQ(err, VK_SUCCESS);
                EXPECT_NE(swapChainSuccess, VK_NULL_HANDLE);

                swapchainCI.oldSwapchain = swapChainSuccess;
                err = fpCreateSwapchainKHR(device_, &swapchainCI, nullptr, &swapChainSuccess2);
                EXPECT_EQ(err, VK_SUCCESS);
                EXPECT_NE(swapChainSuccess2, VK_NULL_HANDLE);
                fpDestroySwapchainKHR(device_, swapChainSuccess, nullptr);
                fpDestroySwapchainKHR(device_, swapChainSuccess2, nullptr);
            }
        }
    }
}

/**
 * @tc.name: test fpCreateSwapchainKHR fail
 * @tc.desc: test fpCreateSwapchainKHR fail
 * @tc.type: FUNC
 * @tc.require: issueI6SKRO
 */
HWTEST_F(VulkanLoaderUnitTest, fpCreateSwapchainKHR_Fail_Test, TestSize.Level1)
{
    if (isSupportedVulkan_) {
        EXPECT_NE(fpCreateSwapchainKHR, nullptr);
        EXPECT_NE(device_, nullptr);
        EXPECT_NE(surface_, VK_NULL_HANDLE);

        std::vector<VkColorSpaceKHR> colorDataspaceArray = {
            VK_COLOR_SPACE_PASS_THROUGH_EXT,
            VK_COLOR_SPACE_DISPLAY_NATIVE_AMD,
            VK_COLOR_SPACE_MAX_ENUM_KHR
        };

        for (decltype(colorDataspaceArray.size()) i = 0; i < colorDataspaceArray.size(); i++) {
            VkSwapchainCreateInfoKHR swapchainCI = GetSwapchainCreateInfo(
                VK_FORMAT_R8G8B8A8_UNORM, colorDataspaceArray[i]);

            VkSwapchainKHR swapChainFail = VK_NULL_HANDLE;
            VkSwapchainKHR swapChainFail2 = VK_NULL_HANDLE;

            VkResult err = fpCreateSwapchainKHR(device_, &swapchainCI, nullptr, &swapChainFail);
            EXPECT_NE(err, VK_SUCCESS);
            EXPECT_EQ(swapChainFail, VK_NULL_HANDLE);

            swapchainCI.oldSwapchain = swapChainFail;
            err = fpCreateSwapchainKHR(device_, &swapchainCI, nullptr, &swapChainFail2);
            EXPECT_NE(err, VK_SUCCESS);
            EXPECT_EQ(swapChainFail2, VK_NULL_HANDLE);
            fpDestroySwapchainKHR(device_, swapChainFail, nullptr);
            fpDestroySwapchainKHR(device_, swapChainFail2, nullptr);
        }
    }
}

/**
 * @tc.name: test vkCreateDebugUtilsMessengerEXT
 * @tc.desc: test vkCreateDebugUtilsMessengerEXT
 * @tc.type: FUNC
 * @tc.require: issueI6SKRO
 */
HWTEST_F(VulkanLoaderUnitTest, vkCreateDebugUtilsMessengerEXT_Test, TestSize.Level1)
{
    if (isSupportedVulkan_) {
        EXPECT_NE(vkCreateDebugUtilsMessengerEXT, nullptr);
        EXPECT_NE(fpCreateSwapchainKHR, nullptr);
        EXPECT_NE(instance_, nullptr);
        EXPECT_NE(device_, nullptr);

        VkDebugUtilsMessengerCreateInfoEXT createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                     VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
        createInfo.pfnUserCallback = UserCallback;
        VkResult result = vkCreateDebugUtilsMessengerEXT(instance_, &createInfo, nullptr, &debugUtilsMessenger);
        EXPECT_EQ(result, VK_SUCCESS);

        debugMessage_.str("");
        VkSwapchainCreateInfoKHR swapchainCI = GetSwapchainCreateInfo(VK_FORMAT_MAX_ENUM, surfaceFormat_.colorSpace);
        VkSwapchainKHR swapChain = VK_NULL_HANDLE;
        result = fpCreateSwapchainKHR(device_, &swapchainCI, nullptr, &swapChain);
        EXPECT_EQ(result, VK_SUCCESS);
        EXPECT_NE(swapChain, VK_NULL_HANDLE);
        fpDestroySwapchainKHR(device_, swapChain, nullptr);

        EXPECT_EQ((debugMessage_.str().find("unsupported swapchain format") != -1), true);
    }
}

/**
 * @tc.name: test vkDestroyDebugUtilsMessengerEXT
 * @tc.desc: test vkDestroyDebugUtilsMessengerEXT
 * @tc.type: FUNC
 * @tc.require: issueI6SKRO
 */
HWTEST_F(VulkanLoaderUnitTest, vkDestroyDebugUtilsMessengerEXT_Test, TestSize.Level1)
{
    if (isSupportedVulkan_) {
        EXPECT_NE(vkDestroyDebugUtilsMessengerEXT, nullptr);
        EXPECT_NE(instance_, nullptr);
        EXPECT_NE(device_, nullptr);
        EXPECT_NE(debugUtilsMessenger, VK_NULL_HANDLE);

        vkDestroyDebugUtilsMessengerEXT(instance_, debugUtilsMessenger, nullptr);

        debugMessage_.str("");
        VkSwapchainCreateInfoKHR swapchainCI = GetSwapchainCreateInfo(VK_FORMAT_MAX_ENUM, surfaceFormat_.colorSpace);
        VkSwapchainKHR swapChain = VK_NULL_HANDLE;
        VkResult result = fpCreateSwapchainKHR(device_, &swapchainCI, nullptr, &swapChain);
        EXPECT_EQ(result, VK_SUCCESS);
        EXPECT_NE(swapChain, VK_NULL_HANDLE);
        fpDestroySwapchainKHR(device_, swapChain, nullptr);

        EXPECT_EQ((debugMessage_.str() == ""), true);
    }
}

/**
 * @tc.name: test fpCreateSwapchainKHR
 * @tc.desc: test fpCreateSwapchainKHR
 * @tc.type: FUNC
 * @tc.require: issueI6SKRO
 */
HWTEST_F(VulkanLoaderUnitTest, fpCreateSwapchainKHR_Test, TestSize.Level1)
{
    if (isSupportedVulkan_) {
        EXPECT_NE(fpCreateSwapchainKHR, nullptr);
        EXPECT_NE(device_, nullptr);
        EXPECT_NE(surface_, VK_NULL_HANDLE);

        VkSwapchainCreateInfoKHR swapchainCI = GetSwapchainCreateInfo(
            VK_FORMAT_B8G8R8A8_UNORM, surfaceFormat_.colorSpace);

        VkResult err = fpCreateSwapchainKHR(device_, &swapchainCI, nullptr, &swapChain_);
        EXPECT_EQ(err, VK_SUCCESS);
        EXPECT_NE(swapChain_, VK_NULL_HANDLE);

        swapchainCI.oldSwapchain = swapChain_;
        err = fpCreateSwapchainKHR(device_, &swapchainCI, nullptr, &swapChain2_);
        EXPECT_EQ(err, VK_SUCCESS);
        EXPECT_NE(swapChain2_, VK_NULL_HANDLE);
    }
}

/**
 * @tc.name: test fpGetSwapchainImagesKHR
 * @tc.desc: test fpGetSwapchainImagesKHR
 * @tc.type: FUNC
 * @tc.require: issueI6SKRO
 */
HWTEST_F(VulkanLoaderUnitTest, fpGetSwapchainImagesKHR_Test, TestSize.Level1)
{
    if (isSupportedVulkan_) {
        uint32_t imageCount;
        std::vector<VkImage> images;
        VkResult err = fpGetSwapchainImagesKHR(device_, swapChain_, &imageCount, NULL);
        EXPECT_EQ(err, VK_SUCCESS);
        images.resize(imageCount);
        err = fpGetSwapchainImagesKHR(device_, swapChain_, &imageCount, images.data());
        EXPECT_EQ(err, VK_SUCCESS);
    }
}

/**
 * @tc.name: test fpGetSwapchainImagesKHR FAIL
 * @tc.desc: test fpGetSwapchainImagesKHR FAIL
 * @tc.type: FUNC
 * @tc.require: issueI6SKRO
 */
HWTEST_F(VulkanLoaderUnitTest, fpGetSwapchainImagesKHR_FAIL_Test, TestSize.Level1)
{
    if (isSupportedVulkan_) {
        uint32_t imageCount = 1;
        std::vector<VkImage> images(imageCount);
        VkResult err = fpGetSwapchainImagesKHR(device_, swapChain2_, &imageCount, images.data());
        EXPECT_NE(err, VK_SUCCESS);
    }
}

/**
 * @tc.name: test vkCreateSemaphore
 * @tc.desc: test vkCreateSemaphore
 * @tc.type: FUNC
 * @tc.require: issueI6SKRO
 */
HWTEST_F(VulkanLoaderUnitTest, vkCreateSemaphore_Test, TestSize.Level1)
{
    if (isSupportedVulkan_) {
        VkSemaphoreCreateInfo semaphoreCreateInfo {};
        semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        PFN_vkCreateSemaphore vkCreateSemaphore = reinterpret_cast<PFN_vkCreateSemaphore>(
            vkGetInstanceProcAddr(instance_, "vkCreateSemaphore"));
        EXPECT_NE(vkCreateSemaphore, nullptr);
        VkResult err = vkCreateSemaphore(device_, &semaphoreCreateInfo, nullptr, &semaphore_);
        EXPECT_EQ(err, VK_SUCCESS);
    }
}

/**
 * @tc.name: test fpAcquireNextImage2KHR
 * @tc.desc: test fpAcquireNextImage2KHR
 * @tc.type: FUNC
 * @tc.require: issueI6SKRO
 */
HWTEST_F(VulkanLoaderUnitTest, fpAcquireNextImage2KHR_Test, TestSize.Level1)
{
    if (isSupportedVulkan_) {
        VkAcquireNextImageInfoKHR pAcquireInfo;
        pAcquireInfo.swapchain = swapChain2_;
        pAcquireInfo.timeout = UINT64_MAX;
        pAcquireInfo.semaphore = semaphore_;
        pAcquireInfo.fence = (VkFence)nullptr;
        uint32_t imageIndex = 0;
        VkResult err = fpAcquireNextImage2KHR(device_, &pAcquireInfo, &imageIndex);
        EXPECT_EQ(err, VK_SUCCESS);
    }
}

/**
 * @tc.name: test fpQueuePresentKHR
 * @tc.desc: test fpQueuePresentKHR
 * @tc.type: FUNC
 * @tc.require: issueI6SKRO
 */
HWTEST_F(VulkanLoaderUnitTest, fpQueuePresentKHR_Test, TestSize.Level1)
{
    if (isSupportedVulkan_) {
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
            vkGetInstanceProcAddr(instance_, "vkGetDeviceQueue"));
        EXPECT_NE(vkGetDeviceQueue, nullptr);
        vkGetDeviceQueue(device_, 0, 0, &queue);
        EXPECT_NE(queue, nullptr);
        uint32_t imageIndex = 0;
        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.pNext = &presentRegions;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &swapChain2_;
        presentInfo.pImageIndices = &imageIndex;
        EXPECT_NE(semaphore_, VK_NULL_HANDLE);
        presentInfo.pWaitSemaphores = &semaphore_;
        presentInfo.waitSemaphoreCount = 1;
        VkResult err = fpQueuePresentKHR(queue, &presentInfo);
        EXPECT_EQ(err, VK_SUCCESS);
    }
}

/**
 * @tc.name: test vkDestroySurfaceKHR nullptr
 * @tc.desc: test vkDestroySurfaceKHR nullptr
 * @tc.type: FUNC
 * @tc.require: issueI6SKRO
 */
HWTEST_F(VulkanLoaderUnitTest, DestroySurface_NULL_Test, TestSize.Level1)
{
    if (isSupportedVulkan_) {
        EXPECT_NE(vkDestroySurfaceKHR, nullptr);
        vkDestroySurfaceKHR(instance_, VK_NULL_HANDLE, nullptr);
    }
}
} // vulkan::loader
