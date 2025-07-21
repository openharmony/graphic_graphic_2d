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

#ifndef RS_CHIPSET_VSYNC_H
#define RS_CHIPSET_VSYNC_H

#include <cinttypes>
#include <functional>
#include <vector>


namespace OHOS::Rosen {
using InitChipsetVsyncFunc = int32_t (*)();
using SetVsyncFunc = int32_t (*)(int64_t, uint64_t, int64_t, bool);

class RsChipsetVsync{
public:
    static RsChipsetVsync& Instance();
    void LoadLibrary();
    void :CloseLibrary();
    int32_t InitChipsetVsync();
    int32_t SetVsync(int64_t timeStamp, uint64_t curTime, int64_t period, bool allowFramerateChange);

private:
    void GetChipsetVsyncFunc();
    void ResetChipsetVsyncFunc();
    void* chipsetVsyncLibHandle_ = nullptr;

    InitChipsetVsyncFunc initChipsetVsyncFunc_ = nullptr;
    SetVsyncFunc setVsyncFunc_ = nullptr;
    bool chipsetVsyncFuncLoaded = false;
}

} // OHOS rosen