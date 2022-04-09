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


#include "pipeline/rs_frame_report.h"

#include "platform/common/rs_log.h"

#include <dlfcn.h>
#include <cstdio>
#include <unistd.h>

namespace OHOS {
namespace Rosen {
namespace {
#ifdef __aarch64__
    const std::string FRAME_AWARE_SO_PATH = "/system/lib64/libframe_ui_intf.z.so";
#else
    const std::string FRAME_AWARE_SO_PATH = "/system/lib/libframe_ui_intf.z.so";
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
}

bool RsFrameReport::LoadLibrary()
{
    if (!frameSchedSoLoaded_) {
        frameSchedHandle_ = dlopen(FRAME_AWARE_SO_PATH.c_str(), RTLD_LAZY);
        if (frameSchedHandle_ == nullptr) {
            ROSEN_LOGE("RsFrameReport:[LoadLibrary]dlopen libframe_ui_intf.so failed! error = %s\n", dlerror());
            return false;
        }
        frameSchedSoLoaded_ = true;
    }
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
    frameGetEnableFunc_ = (FrameGetEnableFunc)LoadSymbol("GetSenseSchedEnable");
    if (frameGetEnableFunc_ != nullptr) {
        return frameGetEnableFunc_();
    } else {
        ROSEN_LOGE("RsFrameReport:[GetEnable]load GetSenseSchedEnable function failed!");
        return 0;
    }
}

void RsFrameReport::ProcessCommandsStart()
{
    processCommandsStartFun_ = (ProcessCommandsStartFunc)LoadSymbol("ProcessCommandsStart");
    if (processCommandsStartFun_ != nullptr) {
        processCommandsStartFun_();
    } else {
        ROSEN_LOGE("RsFrameReport:[ProcessCommandsStart]load ProcessCommandsStart function failed!");
    }
}

void RsFrameReport::AnimateStart()
{
    animateStartFunc_ = (AnimateStartFunc)LoadSymbol("AnimateStart");
    if (animateStartFunc_ != nullptr) {
        animateStartFunc_();
    } else {
        ROSEN_LOGE("RsFrameReport:[AnimateStart]load AnimateStart function failed!");
    }
}

void RsFrameReport::RenderStart()
{
    renderStartRunc_ = (RenderStartFunc)LoadSymbol("RenderStart");
    if (renderStartRunc_ != nullptr) {
        renderStartRunc_();
    } else {
        ROSEN_LOGE("RsFrameReport:[RenderStart]load RenderStart function failed!");
    }
}

void RsFrameReport::SendCommandsStart()
{
    sendCommandsStartFunc_ = (SendCommandsStartFunc)LoadSymbol("SendCommandsStart");
    if (sendCommandsStartFunc_ != nullptr) {
        sendCommandsStartFunc_();
    } else {
        ROSEN_LOGE("RsFrameReport:[SendCommandsStart]load SendCommandsStart function failed!");
    }
}
} // namespace Rosen
} // namespace OHOS
