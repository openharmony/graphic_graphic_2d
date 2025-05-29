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

namespace OHOS {
namespace Rosen {
#undef LOG_DOMAIN
#define LOG_DOMAIN 0xD001400

#undef LOG_TAG
#define LOG_TAG "OHOS::RS"
#define LOGI(fmt, ...) HILOG_INFO(LOG_CORE, fmt, ##__VA_ARGS__)
#define LOGE(fmt, ...) HILOG_ERROR(LOG_CORE, fmt, ##__VA_ARGS__)
namespace {
const std::string FRAME_AWARE_SO_PATH = "libframe_ui_intf.z.so";
}
RsFrameReport& RsFrameReport::GetInstance()
{
    static RsFrameReport instance;
    return instance;
}

RsFrameReport::RsFrameReport()
{
    LOGI("RsFrameReport:[Init] LoadLibrary");
    int ret = LoadLibrary();
    if (!ret) {
        LOGE("RsFrameReport:[Init] dlopen libframe_ui_intf.so failed!");
        return;
    }
    LOGI("RsFrameReport:[Init] dlopen libframe_ui_intf.so success!");
    initFunc_ = (InitFunc)LoadSymbol("Init");
    if (initFunc_ != nullptr) {
        initFunc_();
        LOGI("RsFrameReport:[Init] Init success");
    }
}

RsFrameReport::~RsFrameReport() {}

void RsFrameReport::Init()
{
    LOGI("RsFrameReport init");
    ReportSchedEvent(FrameSchedEvent::INIT, {});
}

bool RsFrameReport::LoadLibrary()
{
    if (!frameSchedSoLoaded_) {
        frameSchedHandle_ = dlopen(FRAME_AWARE_SO_PATH.c_str(), RTLD_LAZY);
        if (frameSchedHandle_ == nullptr) {
            LOGE("RsFrameReport:[LoadLibrary]dlopen libframe_ui_intf.so failed!"
                " error = %{public}s\n", dlerror());
            return false;
        }
        frameSchedSoLoaded_ = true;
    }
    LOGI("RsFrameReport:[LoadLibrary] load library success!");
    return true;
}

void RsFrameReport::CloseLibrary()
{
    if (dlclose(frameSchedHandle_) != 0) {
        LOGE("RsFrameReport:[CloseLibrary]libframe_ui_intf.so failed!\n");
        return;
    }
    frameSchedHandle_ = nullptr;
    frameSchedSoLoaded_ = false;
    LOGI("RsFrameReport:[CloseLibrary]libframe_ui_intf.so close success!\n");
}

void *RsFrameReport::LoadSymbol(const char *symName)
{
    if (!frameSchedSoLoaded_) {
        LOGE("RsFrameReport:[loadSymbol]libframe_ui_intf.so not loaded.\n");
        return nullptr;
    }
    void *funcSym = dlsym(frameSchedHandle_, symName);
    if (funcSym == nullptr) {
        LOGE("RsFrameReport:[loadSymbol]Get %{public}s symbol failed: %{public}s\n", symName, dlerror());
        return nullptr;
    }
    return funcSym;
}

int RsFrameReport::GetEnable()
{
    if (!frameSchedSoLoaded_) {
        return 0;
    }
    if (frameGetEnableFunc_ == nullptr) {
        frameGetEnableFunc_ = (FrameGetEnableFunc)LoadSymbol("GetSenseSchedEnable");
    }
    if (frameGetEnableFunc_ != nullptr) {
        return frameGetEnableFunc_();
    } else {
        LOGE("RsFrameReport:[GetEnable]load GetSenseSchedEnable function failed!");
        return 0;
    }
}

void RsFrameReport::ReportSchedEvent(FrameSchedEvent event, const std::unordered_map<std::string, std::string>& payload)
{
    if (reportSchedEventFunc_ == nullptr) {
        reportSchedEventFunc_ = (ReportSchedEventFunc)LoadSymbol("ReportSchedEvent");
    }
    if (reportSchedEventFunc_ != nullptr) {
        reportSchedEventFunc_(event, payload);
    } else {
        LOGE("RsFrameReport load ReportSchedEvent function failed!");
    }
}

#ifdef RS_ENABLE_VK
void RsFrameReport::ModifierReportSchedEvent(
    FrameSchedEvent event, const std::unordered_map<std::string, std::string> &payload)
{
    if (!frameSchedSoLoaded_) {
        LoadLibrary();
    }
    ReportSchedEvent(event, payload);
}
#endif

void RsFrameReport::SetFrameParam(int requestId, int load, int schedFrameNum, int value)
{
    if (setFrameParamFunc_ == nullptr) {
        setFrameParamFunc_ = (SetFrameParamFunc)LoadSymbol("SetFrameParam");
    }

    if (setFrameParamFunc_ != nullptr) {
        setFrameParamFunc_(requestId, load, schedFrameNum, value);
    } else {
        LOGE("RsFrameReport:[SetFrameParam]load SetFrameParam function failed");
    }
}

void RsFrameReport::SendCommandsStart()
{
    if (sendCommandsStartFunc_ == nullptr) {
        sendCommandsStartFunc_ = (SendCommandsStartFunc)LoadSymbol("SendCommandsStart");
    }
    if (sendCommandsStartFunc_ != nullptr) {
        sendCommandsStartFunc_();
    } else {
        LOGE("RsFrameReport:[SendCommandsStart]load SendCommandsStart function failed!");
    }
}

void RsFrameReport::RenderStart(uint64_t timestamp, int skipFirstFrame)
{
    std::unordered_map<std::string, std::string> payload = {};
    payload["vsyncTime"] = std::to_string(timestamp);
    payload["skipFirstFrame"] = std::to_string(skipFirstFrame);
    ReportSchedEvent(FrameSchedEvent::RS_RENDER_START, payload);
}

void RsFrameReport::RenderEnd()
{
    ReportSchedEvent(FrameSchedEvent::RS_RENDER_END, {});
}

void RsFrameReport::DirectRenderEnd()
{
    ReportSchedEvent(FrameSchedEvent::RS_UNI_RENDER_END, {});
}

void RsFrameReport::UniRenderStart()
{
    ReportSchedEvent(FrameSchedEvent::RS_UNI_RENDER_START, {});
}

void RsFrameReport::UniRenderEnd()
{
    ReportSchedEvent(FrameSchedEvent::RS_UNI_RENDER_END, {});
}

void RsFrameReport::UnblockMainThread()
{
    ReportSchedEvent(FrameSchedEvent::RS_UNBLOCK_MAINTHREAD, {});
}

void RsFrameReport::PostAndWait()
{
    ReportSchedEvent(FrameSchedEvent::RS_POST_AND_WAIT, {});
}

void RsFrameReport::BeginFlush()
{
    ReportSchedEvent(FrameSchedEvent::RS_BEGIN_FLUSH, {});
}

void RsFrameReport::ReportBufferCount(int count)
{
    if (bufferCount_ == count) {
        return;
    }
    bufferCount_ = count;
    std::unordered_map<std::string, std::string> payload = {};
    payload["bufferCount"] = std::to_string(count);
    ReportSchedEvent(FrameSchedEvent::RS_BUFFER_COUNT, payload);
}

void RsFrameReport::ReportHardwareInfo(int tid)
{
    if (hardwareTid_ == tid) {
        return;
    }
    hardwareTid_ = tid;
    std::unordered_map<std::string, std::string> payload = {};
    payload["hardwareTid"] = std::to_string(tid);
    ReportSchedEvent(FrameSchedEvent::RS_HARDWARE_INFO, payload);
}

void RsFrameReport::ReportFrameDeadline(int deadline, uint32_t currentRate)
{
    std::unordered_map<std::string, std::string> payload = {};
    payload["rsFrameDeadline"] = std::to_string(deadline);
    payload["currentRate"] = std::to_string(currentRate);
    ReportSchedEvent(FrameSchedEvent::RS_FRAME_DEADLINE, payload);
}

void RsFrameReport::ReportDDGRTaskInfo()
{
    ReportSchedEvent(FrameSchedEvent::RS_DDGR_TASK, {});
}

void RsFrameReport::ReportScbSceneInfo(std::string description, bool eventStatus)
{
    std::unordered_map<std::string, std::string> payload = {};
    payload["description"] = description;
    payload["eventStatus"] = eventStatus ? "1" : "0"; // true:enter false:exit
    LOGI("RsFrameReport:[ReportScbSceneInfo]description %{public}s, eventStatus %{public}s",
        description.c_str(), payload["eventStatus"].c_str());
    ReportSchedEvent(FrameSchedEvent::GPU_SCB_SCENE_INFO, payload);
}
} // namespace Rosen
} // namespace OHOS