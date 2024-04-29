/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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


#include "rs_frame_report_ext.h"
#include <dlfcn.h>
#include <cstdio>
#include <unistd.h>
#include <hilog/log.h>

namespace OHOS {
namespace Rosen {
namespace {
#if (defined(__aarch64__) || defined(__x86_64__))
    const std::string FRAME_AWARE_SO_PATH = "/system/lib64/libframe_ui_intf.z.so";
#else
    const std::string FRAME_AWARE_SO_PATH = "/system/lib/libframe_ui_intf.z.so";
#endif

#undef LOG_DOMAIN
#define LOG_DOMAIN 0xD001706
#undef LOG_TAG
#define LOG_TAG "RsFrameReportExt"
}
RsFrameReportExt& RsFrameReportExt::GetInstance()
{
    static RsFrameReportExt instance;
    return instance;
}

RsFrameReportExt::RsFrameReportExt()
{
    Init();
}

RsFrameReportExt::~RsFrameReportExt()
{
    CloseLibrary();
}

void RsFrameReportExt::Init()
{
    int ret = LoadLibrary();
    if (!ret) {
        HILOG_ERROR(LOG_CORE, "RsFrameReportExt:[Init] dlopen libframe_ui_intf.so failed!");
        return;
    }
    HILOG_INFO(LOG_CORE, "RsFrameReportExt:[Init] dlopen libframe_ui_intf.so success!");
    initFunc_ = reinterpret_cast<InitFunc>(LoadSymbol("Init"));
    if (initFunc_ != nullptr) {
        initFunc_();
    }
}

bool RsFrameReportExt::LoadLibrary()
{
    if (!frameSchedSoLoaded_) {
        frameSchedHandle_ = dlopen(FRAME_AWARE_SO_PATH.c_str(), RTLD_LAZY);
        if (frameSchedHandle_ == nullptr) {
            HILOG_ERROR(LOG_CORE, "RsFrameReportExt:[LoadLibrary]dlopen libframe_ui_intf.so failed!"
                " error = %{public}s", dlerror());
            return false;
        }
        frameSchedSoLoaded_ = true;
    }
    HILOG_INFO(LOG_CORE, "RsFrameReportExt:[LoadLibrary] load library success!");
    return true;
}

void RsFrameReportExt::CloseLibrary()
{
    if (dlclose(frameSchedHandle_) != 0) {
        HILOG_ERROR(LOG_CORE, "RsFrameReportExt:[CloseLibrary]libframe_ui_intf.so failed!");
        return;
    }
    frameSchedHandle_ = nullptr;
    frameSchedSoLoaded_ = false;
    HILOG_INFO(LOG_CORE, "RsFrameReportExt:[CloseLibrary]libframe_ui_intf.so close success!");
}

void *RsFrameReportExt::LoadSymbol(const char *symName)
{
    if (!frameSchedSoLoaded_) {
        HILOG_ERROR(LOG_CORE, "RsFrameReportExt:[loadSymbol]libframe_ui_intf.so not loaded.");
        return nullptr;
    }

    void *funcSym = dlsym(frameSchedHandle_, symName);
    if (funcSym == nullptr) {
        HILOG_ERROR(LOG_CORE, "RsFrameReportExt:[loadSymbol]Get %{public}s symbol failed: %{public}s",
            symName, dlerror());
        return nullptr;
    }
    return funcSym;
}

int RsFrameReportExt::GetEnable()
{
    if (!frameSchedSoLoaded_) {
        return 0;
    }
    if (frameGetEnableFunc_ == nullptr) {
        frameGetEnableFunc_ = reinterpret_cast<FrameGetEnableFunc>(LoadSymbol("GetSenseSchedEnable"));
    }
    if (frameGetEnableFunc_ != nullptr) {
        return frameGetEnableFunc_();
    } else {
        HILOG_ERROR(LOG_CORE, "RsFrameReportExt:[GetEnable]load GetSenseSchedEnable function failed!");
        return 0;
    }
}

void RsFrameReportExt::HandleSwapBuffer()
{
    if (handleSwapBufferFunc_ == nullptr) {
        handleSwapBufferFunc_ = reinterpret_cast<HandleSwapBufferFunc>(LoadSymbol("HandleSwapBuffer"));
    }
    if (handleSwapBufferFunc_ != nullptr) {
        handleSwapBufferFunc_();
    } else {
        HILOG_ERROR(LOG_CORE, "RsFrameReportExt:[]load HandleSwapBuffer function failed!");
    }
}

void RsFrameReportExt::RequestNextVSync()
{
    if (requestNextVSyncFunc_ == nullptr) {
        requestNextVSyncFunc_ = reinterpret_cast<RequestNextVSyncFunc>(LoadSymbol("RequestNextVSync"));
    }
    if (requestNextVSyncFunc_ != nullptr) {
        requestNextVSyncFunc_();
    } else {
        HILOG_ERROR(LOG_CORE, "RsFrameReportExt:[]load RequestNextVSync function failed!");
    }
}

void RsFrameReportExt::ReceiveVSync()
{
    if (receiveVSyncFunc_ == nullptr) {
        receiveVSyncFunc_ = reinterpret_cast<ReceiveVSyncFunc>(LoadSymbol("ReceiveVSync"));
    }
    if (receiveVSyncFunc_ != nullptr) {
        receiveVSyncFunc_();
    } else {
        HILOG_ERROR(LOG_CORE, "RsFrameReportExt:[]load ReceiveVSync function failed!");
    }
}
} // namespace Rosen
} // namespace OHOS
