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

#include "rs_chipset_vsync.h"
#include <dlfcn.h>
#include "platform/common/rs_log.h"

namespace {
    
}
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
        chipsetVsyncLibHandle_ = dlopen(CHIPSET_VSYNC_LIB_PATH.c_str(), RTLD_NOW);
        GetChipsetVsyncFunc();
    }
}

void RsChipsetVsync::CloseLibrary()
{
    std::unique_lock lock(mutex_);
    if (chipsetVsyncLibHandle_ != nullptr) {
        ResetChipsetVsyncFunc();
        dlclose(chipsetVsyncLibHandle_);
    }
}

void RsChipsetVsync::GetChipsetVsyncFunc()
{
    if (chipsetVsyncLibHandle_) {
        initChipsetVsyncFunc_ = (InitChipsetVsyncFunc)dlsym(chipsetVsyncLibHandle_, "InitChipsetVsyncImpl");
        setVsyncFunc_ = (SetVsyncFunc)dlsym(chipsetVsyncLibHandle_, "SetVsyncImpl");
    }
}

void RsChipsetVsync::ResetChipsetVsyncFunc()
{
    initChipsetVsyncFunc_ = nullptr;
    setVsyncFunc_ = nullptr;
}

int32_t RsChipsetVsync::InitChipsetVsync()
{
    std::unique_lock lock(mutex_);
    if (initChipsetVsyncFunc_ == nullptr) {
        return -1;
    }
    return initChipsetVsyncFunc_();
}

int32_t RsChipsetVsync::SetVsync(int64_t timeStamp, uint64_t curTime, int64_t period, bool allowFramerateChange)
{
    std::unique_lock lock(mutex_);
    if (setVsyncFunc_ == nullptr) {
        return -1;
    }
    return setVsyncFunc_(timeStamp, curTime, period, allowFramerateChange);
}

}