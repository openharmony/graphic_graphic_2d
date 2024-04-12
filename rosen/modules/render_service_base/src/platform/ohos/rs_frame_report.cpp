/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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


#include "pipeline/rs_frame_report.h"

#include <dlfcn.h>
#include <cstdio>
#include <unistd.h>

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
namespace {
#if (defined(__aarch64__) || defined(__x86_64__))
    const std::string FRAME_AWARE_SO_PATH = "/system/lib64/platformsdk/libframe_ui_intf.z.so";
#else
    const std::string FRAME_AWARE_SO_PATH = "/system/lib/platformsdk/libframe_ui_intf.z.so";
#endif
}
RsFrameReport& RsFrameReport::GetInstance()
{
    static RsFrameReport instance;
    return instance;
}

RsFrameReport::RsFrameReport() {}

RsFrameReport::~RsFrameReport()
{
    CloseLibrary();
}

void RsFrameReport::Init()
{
    int ret = LoadLibrary();
    if (!ret) {
        ROSEN_LOGE("RsFrameReport:[Init] dlopen libframe_ui_intf.so failed!");
        return;
    }
    ROSEN_LOGI("RsFrameReport:[Init] dlopen libframe_ui_intf.so success!");
    initFunc_ = (InitFunc)LoadSymbol("Init");
    if (initFunc_ != nullptr) {
        initFunc_();
    }
}

bool RsFrameReport::LoadLibrary()
{
    if (!frameSchedSoLoaded_) {
        frameSchedHandle_ = dlopen(FRAME_AWARE_SO_PATH.c_str(), RTLD_LAZY);
        if (frameSchedHandle_ == nullptr) {
            ROSEN_LOGE("RsFrameReport:[LoadLibrary]dlopen libframe_ui_intf.so failed!"
                " error = %{public}s\n", dlerror());
            return false;
        }
        frameSchedSoLoaded_ = true;
    }
    ROSEN_LOGI("RsFrameReport:[LoadLibrary] load library success!");
    return true;
}

void RsFrameReport::CloseLibrary()
{
    if (dlclose(frameSchedHandle_) != 0) {
        ROSEN_LOGE("RsFrameReport:[CloseLibrary]libframe_ui_intf.so failed!\n");
        return;
    }
    frameSchedHandle_ = nullptr;
    frameSchedSoLoaded_ = false;
    ROSEN_LOGI("RsFrameReport:[CloseLibrary]libframe_ui_intf.so close success!\n");
}

void *RsFrameReport::LoadSymbol(const char *symName)
{
    if (!frameSchedSoLoaded_) {
        ROSEN_LOGE("RsFrameReport:[loadSymbol]libframe_ui_intf.so not loaded.\n");
        return nullptr;
    }

    void *funcSym = dlsym(frameSchedHandle_, symName);
    if (funcSym == nullptr) {
        ROSEN_LOGE("RsFrameReport:[loadSymbol]Get %{public}s symbol failed: %{public}s\n", symName, dlerror());
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
        ROSEN_LOGE("RsFrameReport:[GetEnable]load GetSenseSchedEnable function failed!");
        return 0;
    }
}

void RsFrameReport::ProcessCommandsStart()
{
    if (processCommandsStartFun_ == nullptr) {
        processCommandsStartFun_ = (ProcessCommandsStartFunc)LoadSymbol("ProcessCommandsStart");
    }
    if (processCommandsStartFun_ != nullptr) {
        processCommandsStartFun_();
    } else {
        ROSEN_LOGE("RsFrameReport:[ProcessCommandsStart]load ProcessCommandsStart function failed!");
    }
}

void RsFrameReport::AnimateStart()
{
    if (animateStartFunc_ == nullptr) {
        animateStartFunc_ = (AnimateStartFunc)LoadSymbol("AnimateStart");
    }
    if (animateStartFunc_ != nullptr) {
        animateStartFunc_();
    } else {
        ROSEN_LOGE("RsFrameReport:[AnimateStart]load AnimateStart function failed!");
    }
}

void RsFrameReport::RenderStart(uint64_t timestamp)
{
    if (renderStartFunc_ == nullptr) {
        renderStartFunc_ = (RenderStartFunc)LoadSymbol("RenderStart");
    }

    if (renderStartFunc_ != nullptr) {
        renderStartFunc_(timestamp);
    } else {
        ROSEN_LOGE("RsFrameReport:[RenderStart]load RenderStart function failed!");
    }
}

void RsFrameReport::RenderEnd()
{
    if (renderEndFunc_ == nullptr) {
        renderEndFunc_ = (RenderEndFunc)LoadSymbol("RenderEnd");
    }

    if (renderEndFunc_ != nullptr) {
        renderEndFunc_();
    } else {
        ROSEN_LOGE("RsFrameReport:[RenderEnd]load RenderEnd function failed!");
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
        ROSEN_LOGE("RsFrameReport:[SendCommandsStart]load SendCommandsStart function failed!");
    }
}

void RsFrameReport::SetFrameParam(int requestId, int load, int schedFrameNum, int value)
{
    if (setFrameParamFunc_ == nullptr) {
        setFrameParamFunc_ = (SetFrameParamFunc)LoadSymbol("SetFrameParam");
    }

    if (setFrameParamFunc_ != nullptr) {
        setFrameParamFunc_(requestId, load, schedFrameNum, value);
    } else {
        ROSEN_LOGE("RsFrameReport:[SetFrameParam]load SetFrameParam function failed");
    }
}
} // namespace Rosen
} // namespace OHOS
