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
#ifdef RS_ENABLE_VK
const std::string LIB_VULKAN_PATH = "/system/lib64/libvulkan.so";
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

#ifdef RS_ENABLE_VK
std::atomic<bool> isInit{false};
PFN_vkSetFrontWindowStatusHUAWEI mSetFrontWindowStatusHUAWEI = nullptr;
PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr = nullptr;
std::unique_ptr<void, RsFrameReport::VkHandleDeleter> RsFrameReport::vkhandle = nullptr;
std::function<void*(const char*, int)> RsFrameReport::dlopenFunc = ::dlopen;
std::function<void*(void*, const char*)> RsFrameReport::dlsymFunc = ::dlsym;

void RsFrameReport::VkHandleDeleter::operator()(void* ptr) const
{
    if (ptr) {
        dlclose(ptr);
    }
}

bool RsFrameReport::InitializeVulkanExtensions(VkDevice device)
{
    if (isInit.load() && mSetFrontWindowStatusHUAWEI != nullptr) {
        return true;
    }
    if (device == nullptr) {
        LOGE("Obtain vkdevice fail");
        return false;
    }

    vkhandle.reset(dlopenFunc(LIB_VULKAN_PATH.c_str(), RTLD_NOW | RTLD_LOCAL));
    if (vkhandle == nullptr) {
        char* err = dlerror();
        LOGE("Failed to load Vulkan library: %{public}s", err ? err : "unknow error");
        return false;
    }

    vkGetDeviceProcAddr = reinterpret_cast<PFN_vkGetDeviceProcAddr>(dlsymFunc(vkhandle.get(), "vkGetDeviceProcAddr"));
    if (vkGetDeviceProcAddr == nullptr) {
        LOGE("Failed to obtain vkGetDeviceProcAddr");
        vkhandle.reset();
        return false;
    }

    mSetFrontWindowStatusHUAWEI = reinterpret_cast<PFN_vkSetFrontWindowStatusHUAWEI>(
        vkGetDeviceProcAddr(device, "vkSetFrontWindowStatusHUAWEI"));
    if (mSetFrontWindowStatusHUAWEI == nullptr) {
        LOGE("Failed to obtain vkSetFrontWindowStatusHUAWEI");
        vkhandle.reset();
        return false;
    }

    isInit.store(true);
    return true;
}

void RsFrameReport::ReportWindowInfo(VkDevice device, bool isSingleFullScreenApp, const char* firstFrontBundleName)
{
    if (!InitializeVulkanExtensions(device)) {
        LOGE("Failed to initialize Vulkan extensions");
        return;
    }
    mSetFrontWindowStatusHUAWEI(nullptr, isSingleFullScreenApp, firstFrontBundleName);
}
#endif
} // namespace Rosen
} // namespace OHOS
