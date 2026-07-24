/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "rs_frame_report.h"

#include <dlfcn.h>
#include <cstdio>
#include <unistd.h>

#include "hilog/log.h"
#include "frame_ui_intf.h"

namespace OHOS {
namespace Rosen {
#undef LOG_DOMAIN
#define LOG_DOMAIN 0xD001400

#undef LOG_TAG
#define LOG_TAG "OHOS::RS"
#define LOGI(fmt, ...) HILOG_INFO(LOG_CORE, fmt, ##__VA_ARGS__)
#define LOGE(fmt, ...) HILOG_ERROR(LOG_CORE, fmt, ##__VA_ARGS__)
namespace {
#if defined (RS_ENABLE_VK) && !defined(ROSEN_ARKUI_X)
const std::string LIB_VULKAN_PATH = "/system/lib64/libvulkan.so";
const uint32_t MAX_INITIALIZATION_COUNT = 3;
#endif
}

std::once_flag RsFrameReport::initFlag_;
bool RsFrameReport::inited = false;

static void GraphReportSchedEvent(OHOS::RME::FrameSchedEvent event,
    const std::unordered_map<std::string, std::string> &payload)
{
    OHOS::RME::FrameUiIntf::GetInstance().ReportSchedEvent(event, payload);
}

void RsFrameReport::InitSched()
{
    OHOS::RME::FrameUiIntf::GetInstance().Init();
    inited = true;
}

bool RsFrameReport::IsInitSchedCompleted()
{
    return inited;
}

void RsFrameReport::InitDeadline()
{
    std::call_once(initFlag_, &RsFrameReport::InitSched);
    LOGI("RsFrameReport init Deadline");
    GraphReportSchedEvent(OHOS::RME::FrameSchedEvent::INIT, {});
}

void RsFrameReport::SetFrameParam(int requestId, int load, int schedFrameNum, int value)
{
    std::call_once(initFlag_, &RsFrameReport::InitSched);
    std::unordered_map<std::string, std::string> payload;
    payload["requestId"] = std::to_string(requestId);
    payload["load"] = std::to_string(load);
    payload["schedFrameNum"] = std::to_string(schedFrameNum);
    payload["value"] = std::to_string(value);
    GraphReportSchedEvent(OHOS::RME::FrameSchedEvent::SET_FRAME_PARAM, payload);
}

void RsFrameReport::SendCommandsStart()
{
    std::call_once(initFlag_, &RsFrameReport::InitSched);
    GraphReportSchedEvent(OHOS::RME::FrameSchedEvent::RS_SEND_COMMANDS_START, {});
}

void RsFrameReport::RenderStart(uint64_t timestamp, int skipFirstFrame)
{
    std::call_once(initFlag_, &RsFrameReport::InitSched);
    std::unordered_map<std::string, std::string> payload = {};
    payload["vsyncTime"] = std::to_string(timestamp);
    payload["skipFirstFrame"] = std::to_string(skipFirstFrame);
    GraphReportSchedEvent(OHOS::RME::FrameSchedEvent::RS_RENDER_START, payload);
}

void RsFrameReport::RenderEnd()
{
    std::call_once(initFlag_, &RsFrameReport::InitSched);
    GraphReportSchedEvent(OHOS::RME::FrameSchedEvent::RS_RENDER_END, {});
}

void RsFrameReport::DirectRenderEnd()
{
    std::call_once(initFlag_, &RsFrameReport::InitSched);
    GraphReportSchedEvent(OHOS::RME::FrameSchedEvent::RS_UNI_RENDER_END, {});
}

void RsFrameReport::UniRenderStart()
{
    std::call_once(initFlag_, &RsFrameReport::InitSched);
    GraphReportSchedEvent(OHOS::RME::FrameSchedEvent::RS_UNI_RENDER_START, {});
}

void RsFrameReport::UniRenderEnd()
{
    std::call_once(initFlag_, &RsFrameReport::InitSched);
    GraphReportSchedEvent(OHOS::RME::FrameSchedEvent::RS_UNI_RENDER_END, {});
}

void RsFrameReport::CheckUnblockMainThreadPoint()
{
    std::call_once(initFlag_, &RsFrameReport::InitSched);
    GraphReportSchedEvent(OHOS::RME::FrameSchedEvent::RS_UNBLOCK_MAINTHREAD, {});
}

void RsFrameReport::CheckPostAndWaitPoint()
{
    std::call_once(initFlag_, &RsFrameReport::InitSched);
    GraphReportSchedEvent(OHOS::RME::FrameSchedEvent::RS_POST_AND_WAIT, {});
}

void RsFrameReport::CheckBeginFlushPoint()
{
    std::call_once(initFlag_, &RsFrameReport::InitSched);
    GraphReportSchedEvent(OHOS::RME::FrameSchedEvent::RS_BEGIN_FLUSH, {});
}

void RsFrameReport::ReportBufferCount(uint32_t count)
{
    std::call_once(initFlag_, &RsFrameReport::InitSched);
    static uint32_t  bufferCount = 0;
    if (bufferCount == count) {
        return;
    }
    bufferCount = count;
    std::unordered_map<std::string, std::string> payload = {};
    payload["bufferCount"] = std::to_string(count);
    GraphReportSchedEvent(OHOS::RME::FrameSchedEvent::RS_BUFFER_COUNT, payload);
}

void RsFrameReport::ReportComposerInfo(const int screenId, const int composerTid)
{
    std::unordered_map<std::string, std::string> payload;
    payload.emplace("screenId", std::to_string(screenId));
    payload.emplace("composerTid", std::to_string(composerTid));
    GraphReportSchedEvent(OHOS::RME::FrameSchedEvent::RS_COMPOSER_INFO, payload);
}

void RsFrameReport::ReportFrameDeadline(int deadline, uint32_t currentRate)
{
    std::call_once(initFlag_, &RsFrameReport::InitSched);
    std::unordered_map<std::string, std::string> payload = {};
    payload["rsFrameDeadline"] = std::to_string(deadline);
    payload["currentRate"] = std::to_string(currentRate);
    GraphReportSchedEvent(OHOS::RME::FrameSchedEvent::RS_FRAME_DEADLINE, payload);
}

void RsFrameReport::ReportUnmarshalData(int unmarshalTid, size_t dataSize)
{
    std::call_once(initFlag_, &RsFrameReport::InitSched);
    if (unmarshalTid <= 0) {
        return;
    }
    std::unordered_map<std::string, std::string> payload = {};
    payload["unmarshalTid"] = std::to_string(unmarshalTid);
    payload["dataSize"] = std::to_string(dataSize);
    GraphReportSchedEvent(OHOS::RME::FrameSchedEvent::RS_UNMARSHAL_DATA, payload);
}

void RsFrameReport::ReportDDGRTaskInfo()
{
    std::call_once(initFlag_, &RsFrameReport::InitSched);
    GraphReportSchedEvent(OHOS::RME::FrameSchedEvent::RS_DDGR_TASK, {});
}

void RsFrameReport::ReportScbSceneInfo(const std::string& description, bool eventStatus)
{
    std::call_once(initFlag_, &RsFrameReport::InitSched);
    std::unordered_map<std::string, std::string> payload = {};
    payload["description"] = description;
    payload["eventStatus"] = eventStatus ? "1" : "0"; // true:enter false:exit
    GraphReportSchedEvent(OHOS::RME::FrameSchedEvent::GPU_SCB_SCENE_INFO, payload);
}

void RsFrameReport::BlurPredict(const std::unordered_map<std::string, std::string>& payload)
{
    std::call_once(initFlag_, &RsFrameReport::InitSched);
    GraphReportSchedEvent(OHOS::RME::FrameSchedEvent::RS_BLUR_PREDICT, payload);
}

void RsFrameReport::ReceiveVSync()
{
    std::call_once(initFlag_, &RsFrameReport::InitSched);
    GraphReportSchedEvent(OHOS::RME::FrameSchedEvent::RS_DDGR_TASK, {});
}

void RsFrameReport::RequestNextVSync()
{
    std::call_once(initFlag_, &RsFrameReport::InitSched);
    GraphReportSchedEvent(OHOS::RME::FrameSchedEvent::RS_DDGR_TASK, {});
}

void RsFrameReport::ReportAddScreenId(const int screenId)
{
    std::call_once(initFlag_, &RsFrameReport::InitSched);
    GraphReportSchedEvent(OHOS::RME::FrameSchedEvent::RS_ADD_SCREENID, {});
}

void RsFrameReport::ReportDelScreenId(const int screenId)
{
    std::call_once(initFlag_, &RsFrameReport::InitSched);
    GraphReportSchedEvent(OHOS::RME::FrameSchedEvent::RS_DEL_SCREENID, {});
}

#if defined (RS_ENABLE_VK) && !defined(ROSEN_ARKUI_X)
std::atomic<bool> RsFrameReport::isInit{false};
uint32_t RsFrameReport::initCount_ = 0;
VkDevice RsFrameReport::device_ = VK_NULL_HANDLE;
VkInstance RsFrameReport::instance_ = VK_NULL_HANDLE;
std::shared_mutex RsFrameReport::initMutex_;
PFN_vkSetFrontWindowStatusHUAWEI RsFrameReport::mSetFrontWindowStatusHUAWEI = nullptr;
PFN_vkGetInstanceProcAddr RsFrameReport::vkGetInstanceProcAddr = nullptr;
PFN_vkGetDeviceProcAddr RsFrameReport::vkGetDeviceProcAddr = nullptr;
PFN_vkCreateInstance RsFrameReport::vkCreateInstance = nullptr;
PFN_vkDestroyInstance RsFrameReport::vkDestroyInstance = nullptr;
PFN_vkEnumeratePhysicalDevices RsFrameReport::vkEnumeratePhysicalDevices = nullptr;
PFN_vkCreateDevice RsFrameReport::vkCreateDevice = nullptr;
PFN_vkDestroyDevice RsFrameReport::vkDestroyDevice = nullptr;
PFN_vkGetPhysicalDeviceQueueFamilyProperties RsFrameReport::vkGetPhysicalDeviceQueueFamilyProperties = nullptr;
std::unique_ptr<void, RsFrameReport::VkHandleDeleter> RsFrameReport::vkhandle = nullptr;
std::function<void*(const char*, int)> RsFrameReport::dlopenFunc = ::dlopen;
std::function<void*(void*, const char*)> RsFrameReport::dlsymFunc = ::dlsym;

void RsFrameReport::VkHandleDeleter::operator()(void* ptr) const
{
    if (ptr) {
        dlclose(ptr);
    }
}

bool RsFrameReport::GetVulkanFunctionPointersByLibrary()
{
    vkhandle.reset(dlopenFunc(LIB_VULKAN_PATH.c_str(), RTLD_NOW | RTLD_LOCAL));
    if (vkhandle == nullptr) {
        LOGE("Failed to load Vulkan library: %{public}s", dlerror());
        return false;
    }
    vkGetInstanceProcAddr =
        reinterpret_cast<PFN_vkGetInstanceProcAddr>(dlsymFunc(vkhandle.get(), "vkGetInstanceProcAddr"));
    vkGetDeviceProcAddr = reinterpret_cast<PFN_vkGetDeviceProcAddr>(dlsymFunc(vkhandle.get(), "vkGetDeviceProcAddr"));
    vkCreateInstance = reinterpret_cast<PFN_vkCreateInstance>(dlsymFunc(vkhandle.get(), "vkCreateInstance"));
    vkDestroyInstance = reinterpret_cast<PFN_vkDestroyInstance>(dlsymFunc(vkhandle.get(), "vkDestroyInstance"));

    return true;
}
 
bool RsFrameReport::GetVulkanFunctionPointersByInstance()
{
    if (!instance_) {
        LOGE("Vulkan instance is null");
        return false;
    }
    if (!vkGetInstanceProcAddr) {
        LOGE("Failed to get Vulkan function pointers : vkGetInstanceProcAddr");
        return false;
    }
 
    vkCreateDevice = reinterpret_cast<PFN_vkCreateDevice>(vkGetInstanceProcAddr(instance_, "vkCreateDevice"));
    vkDestroyDevice = reinterpret_cast<PFN_vkDestroyDevice>(vkGetInstanceProcAddr(instance_, "vkDestroyDevice"));
    vkEnumeratePhysicalDevices = reinterpret_cast<PFN_vkEnumeratePhysicalDevices>
        (vkGetInstanceProcAddr(instance_, "vkEnumeratePhysicalDevices"));
    vkGetPhysicalDeviceQueueFamilyProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceQueueFamilyProperties>
        (vkGetInstanceProcAddr(instance_, "vkGetPhysicalDeviceQueueFamilyProperties"));
 
    return true;
}
 
bool RsFrameReport::CreateVulkanInstance()
{
    if (!vkCreateInstance) {
        LOGE("Failed to get Vulkan function pointers : vkCreateInstance");
        return false;
    }
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "OHOS";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_2;
 
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    if (vkCreateInstance(&createInfo, nullptr, &instance_) != VK_SUCCESS) {
        LOGE("Failed to create Vulkan instance");
        return false;
    }
    return GetVulkanFunctionPointersByInstance();
}
 
uint32_t RsFrameReport::FindQueueFamilyIndex(VkPhysicalDevice physicalDevice)
{
    if (!vkGetPhysicalDeviceQueueFamilyProperties) {
        LOGE("Failed to get Vulkan function pointers : vkGetPhysicalDeviceQueueFamilyProperties");
        return UINT32_MAX;
    }
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());
    for (uint32_t i = 0; i < queueFamilyCount; i++) {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            return i;
        }
    }
    return UINT32_MAX;
}
 
bool RsFrameReport::CreateVulkanDevice()
{
    if (!vkEnumeratePhysicalDevices) {
        LOGE("Failed to get Vulkan function pointers : vkEnumeratePhysicalDevices");
        return false;
    }
    if (!vkCreateDevice) {
        LOGE("Failed to get Vulkan function pointers : vkCreateDevice");
        return false;
    }
 
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance_, &deviceCount, nullptr);
    if (deviceCount == 0) {
        LOGE("Failed to find GPUs with Vulkan support");
        return false;
    }
 
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance_, &deviceCount, devices.data());
    VkPhysicalDevice physicalDevice = devices[0];
 
    uint32_t queueFamilyIndex = FindQueueFamilyIndex(physicalDevice);
    if (queueFamilyIndex == UINT32_MAX) {
        LOGE("Failed to find a suitable queue family");
        return false;
    }
 
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
    queueCreateInfo.queueCount = 1;
    float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;
 
    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    deviceCreateInfo.queueCreateInfoCount = 1;
    if (vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device_) != VK_SUCCESS) {
        LOGE("vkCreateDevice failed");
        return false;
    }
    return true;
}
 
bool RsFrameReport::GetSetFrontWindowStatusHUAWEI()
{
    if (!vkGetDeviceProcAddr) {
        LOGE("Failed to get Vulkan function pointers : vkGetDeviceProcAddr");
        return false;
    }
    mSetFrontWindowStatusHUAWEI = reinterpret_cast<PFN_vkSetFrontWindowStatusHUAWEI>(
        vkGetDeviceProcAddr(device_, "vkSetFrontWindowStatusHUAWEI"));
    if (mSetFrontWindowStatusHUAWEI == nullptr) {
        LOGE("Failed to obtain vkSetFrontWindowStatusHUAWEI");
        return false;
    }
    return true;
}
 
bool RsFrameReport::InitializeVulkanExtensions()
{
    if (isInit.load() && mSetFrontWindowStatusHUAWEI) {
        return true;
    }
    if (!GetVulkanFunctionPointersByLibrary()) {
        return false;
    }
    if (!CreateVulkanInstance()) {
        vkhandle.reset();
        return false;
    }
    if (!CreateVulkanDevice()) {
        vkhandle.reset();
        return false;
    }
    if (!GetSetFrontWindowStatusHUAWEI()) {
        vkDestroyDevice(device_, nullptr);
        device_ = VK_NULL_HANDLE;
        vkhandle.reset();
        return false;
    }
    initCount_ = 0;
    isInit.store(true);
    return true;
}

void RsFrameReport::ReportWindowInfo(bool isSingleFullScreenApp, const char* firstFrontBundleName)
{
    std::unique_lock<std::shared_mutex> lock(initMutex_);
    if (initCount_ >= MAX_INITIALIZATION_COUNT) {
        return;
    }
    if (!InitializeVulkanExtensions()) {
        LOGE("Failed to initialize Vulkan extensions");
        initCount_++;
        isInit.store(false);
        return;
    }
    mSetFrontWindowStatusHUAWEI(device_, isSingleFullScreenApp, firstFrontBundleName);
}
#endif
} // namespace Rosen
} // namespace OHOS
