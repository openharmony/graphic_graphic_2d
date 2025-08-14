/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include <dlfcn.h>
#include "platform/common/rs_log.h"
#include "rs_chipset_vsync.h"
namespace OHOS::Rosen {
const std::string CHIPSET_VSYNC_LIB_PATH = "libchipset_vsync.z.so";

RsChipsetVsync& RsChipsetVsync::Instance()
{
    static RsChipsetVsync instance;
    return instance;
}

void RsChipsetVsync::LoadLibrary()
{
    std::unique_lock lock(mutex_);
    if (chipsetVsyncLibHandle_ == nullptr) {
        dlerror(); // 清除历史报错
        chipsetVsyncLibHandle_ = dlopen(CHIPSET_VSYNC_LIB_PATH.c_str(), RTLD_NOW);
        if (chipsetVsyncLibHandle_ == nullptr) {
            const char* error = dlerror();
            if (error != nullptr) {
                RS_LOGW("fail to load chipset vsync lib error = %{public}s", error);
            }
        } else {
            GetChipsetVsyncFunc();
        }
    }
}

void RsChipsetVsync::CloseLibrary()
{
    std::unique_lock lock(mutex_);
    if (chipsetVsyncLibHandle_ != nullptr) {
        ResetChipsetVsyncFunc();
        dlclose(chipsetVsyncLibHandle_);
        chipsetVsyncLibHandle_ = nullptr;
    }
}

void RsChipsetVsync::GetChipsetVsyncFunc()
{
    std::unique_lock lock(funcMutex_);
    if (chipsetVsyncLibHandle_) {
        initChipsetVsyncFunc_ = (InitChipsetVsyncFunc)dlsym(chipsetVsyncLibHandle_, "InitChipsetVsyncImpl");
        const char* dlsymError = dlerror();
        if (dlsymError) {
            RS_LOGW("GetChipsetVsyncFunc failed to load InitChipsetVsyncImpl: %{public}s", dlsymError);
            initChipsetVsyncFunc_ = nullptr;
            return;
        }
        setVsyncFunc_ = (SetVsyncFunc)dlsym(chipsetVsyncLibHandle_, "SetVsyncImpl");
        dlsymError = dlerror();
        if (dlsymError) {
            RS_LOGW("GetChipsetVsyncFunc failed to load SetVsyncImpl: %{public}s", dlsymError);
            setVsyncFunc_ = nullptr;
            initChipsetVsyncFunc_ = nullptr; // 重置已成功获取的函数指针
            return;
        }
    }
}

void RsChipsetVsync::ResetChipsetVsyncFunc()
{
    std::unique_lock lock(funcMutex_);
    initChipsetVsyncFunc_ = nullptr;
    setVsyncFunc_ = nullptr;
}

int32_t RsChipsetVsync::InitChipsetVsync()
{
    std::unique_lock lock(mutex_);
    if (initChipsetVsyncFunc_ == nullptr) {
        RS_LOGD("InitChipsetVsyncFunc not loaded");
        return -1;
    }
    return initChipsetVsyncFunc_();
}

int32_t RsChipsetVsync::SetVsync(int64_t timeStamp, uint64_t curTime, int64_t period, bool allowFramerateChange)
{
    std::unique_lock lock(mutex_);
    if (setVsyncFunc_ == nullptr) {
        RS_LOGD("SetVsyncFunc not loaded");
        return -1;
    }
    return setVsyncFunc_(timeStamp, curTime, period, allowFramerateChange);
}

}