/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "vsyncreceiverapi_fuzzer.h"
#include <fuzzer/FuzzedDataProvider.h>
#include <cstdint>
#include <memory>
#include <securec.h>
#include <sstream>
#include <sys/socket.h>

#include <event_handler.h>
#include <sys/types.h>
#include "graphic_common_c.h"
#include "graphic_common.h"
#include "vsync_receiver.h"
#include "vsync_distributor.h"
#include "vsync_controller.h"

namespace OHOS {
namespace Rosen {

namespace {
constexpr int64_t OFFSET = 0;
constexpr int32_t RATE_MIN = -1;
constexpr int32_t RATE_MAX = 60;
constexpr int32_t BUFFER_COUNT_MIN = 0;
constexpr int32_t BUFFER_COUNT_MAX = 10;
constexpr size_t STRING_LIST_SIZE_MAX = 3;
constexpr size_t STRING_LEN_MAX = 32;
constexpr int32_t TOUCH_TYPE_MIN = 0;
constexpr int32_t TOUCH_TYPE_MAX = 3;

const uint8_t DO_INIT = 0;
const uint8_t DO_SET_VSYNC_RATE = 1;
const uint8_t DO_GET_VSYNC_PERIOD_AND_LAST_TIME_STAMP = 2;
const uint8_t DO_REQUEST_NEXT_VSYNC_WITH_PARAMS = 3;
const uint8_t DO_SET_UI_DVSYNC_SWITCH = 4;
const uint8_t DO_SET_VSYNC_CALL_BACK_FOR_EVERY_FRAME = 5;
const uint8_t DO_SET_UI_DVSYNC_CONFIG = 6;
const uint8_t DO_SET_NATIVE_DVSYNC_SWITCH = 7;
const uint8_t DO_SET_TOUCH_EVENT = 8;
const uint8_t TARGET_SIZE = 9;

const uint8_t CALLBACK_TYPE_A = 0;
const uint8_t CALLBACK_TYPE_B = 1;
const uint8_t CALLBACK_TYPE_C = 2;
const uint8_t CALLBACK_TYPE_SIZE = 3;

void OnVSyncCallbackA(int64_t now, void* data) {}
void OnVSyncCallbackB(int64_t now, void* data) {}
void OnVSyncCallbackC(int64_t now, void* data) {}

VSyncReceiver::FrameCallback CreateFrameCallback(FuzzedDataProvider& fdp)
{
    uint8_t callbackType = fdp.ConsumeIntegral<uint8_t>() % CALLBACK_TYPE_SIZE;
    VSyncReceiver::FrameCallback callback;
    callback.userData_ = nullptr;
    switch (callbackType) {
        case CALLBACK_TYPE_A:
            callback.callback_ = OnVSyncCallbackA;
            break;
        case CALLBACK_TYPE_B:
            callback.callback_ = OnVSyncCallbackB;
            break;
        case CALLBACK_TYPE_C:
            callback.callback_ = OnVSyncCallbackC;
            break;
        default:
            callback.callback_ = OnVSyncCallbackA;
            break;
    }
    return callback;
}

std::vector<std::string> CreateStringList(FuzzedDataProvider& fdp)
{
    std::vector<std::string> result;
    size_t listSize = static_cast<size_t>(fdp.ConsumeIntegral<uint8_t>() % STRING_LIST_SIZE_MAX);
    for (size_t i = 0; i < listSize; ++i) {
        result.push_back(fdp.ConsumeRandomLengthString(STRING_LEN_MAX));
    }
    return result;
}

sptr<VSyncReceiver> CreateVSyncReceiver()
{
    sptr<VSyncGenerator> vsyncGenerator = CreateVSyncGenerator();
    if (vsyncGenerator == nullptr) {
        return nullptr;
    }
    sptr<VSyncController> vsyncController = new VSyncController(vsyncGenerator, OFFSET);
    if (vsyncController == nullptr) {
        return nullptr;
    }
    sptr<VSyncDistributor> vsyncDistributor = new VSyncDistributor(vsyncController, "FuzzDistributor");
    if (vsyncDistributor == nullptr) {
        return nullptr;
    }
    sptr<VSyncConnection> vsyncConnection = new VSyncConnection(vsyncDistributor, "FuzzConnection");
    if (vsyncConnection == nullptr) {
        return nullptr;
    }
    sptr<VSyncReceiver> vsyncReceiver = new VSyncReceiver(vsyncConnection);
    return vsyncReceiver;
}

void DoInit(FuzzedDataProvider& fdp)
{
    auto vsyncReceiver = CreateVSyncReceiver();
    if (vsyncReceiver == nullptr) {
        return;
    }
    bool needAddFd = fdp.ConsumeBool();
    vsyncReceiver->Init(needAddFd);
}

void DoSetVsyncRate(FuzzedDataProvider& fdp)
{
    auto vsyncReceiver = CreateVSyncReceiver();
    if (vsyncReceiver == nullptr) {
        return;
    }
    vsyncReceiver->Init();
    auto callback = CreateFrameCallback(fdp);
    int32_t rate = fdp.ConsumeIntegralInRange<int32_t>(RATE_MIN, RATE_MAX);
    vsyncReceiver->SetVSyncRate(callback, rate);
}

void DoGetVsyncPeriodAndLastTimeStamp(FuzzedDataProvider& fdp)
{
    auto vsyncReceiver = CreateVSyncReceiver();
    if (vsyncReceiver == nullptr) {
        return;
    }
    vsyncReceiver->Init();
    int64_t period = 0;
    int64_t timeStamp = 0;
    bool isThreadShared = fdp.ConsumeBool();
    vsyncReceiver->GetVSyncPeriodAndLastTimeStamp(period, timeStamp, isThreadShared);
}

void DoRequestNextVsyncWithParams(FuzzedDataProvider& fdp)
{
    auto vsyncReceiver = CreateVSyncReceiver();
    if (vsyncReceiver == nullptr) {
        return;
    }
    vsyncReceiver->Init();
    auto callback = CreateFrameCallback(fdp);
    std::string fromWhom = fdp.ConsumeRandomLengthString(STRING_LEN_MAX);
    int64_t lastVSyncTS = fdp.ConsumeIntegral<int64_t>();
    int64_t requestVsyncTime = fdp.ConsumeIntegral<int64_t>();
    vsyncReceiver->RequestNextVSync(callback, fromWhom, lastVSyncTS, requestVsyncTime);
}

void DoSetUiDvsyncSwitch(FuzzedDataProvider& fdp)
{
    auto vsyncReceiver = CreateVSyncReceiver();
    if (vsyncReceiver == nullptr) {
        return;
    }
    vsyncReceiver->Init();
    bool dvsyncSwitch = fdp.ConsumeBool();
    vsyncReceiver->SetUiDvsyncSwitch(dvsyncSwitch);
}

void DoSetVsyncCallBackForEveryFrame(FuzzedDataProvider& fdp)
{
    auto vsyncReceiver = CreateVSyncReceiver();
    if (vsyncReceiver == nullptr) {
        return;
    }
    vsyncReceiver->Init();
    auto callback = CreateFrameCallback(fdp);
    bool isOpen = fdp.ConsumeBool();
    vsyncReceiver->SetVsyncCallBackForEveryFrame(callback, isOpen);
}

void DoSetUiDvsyncConfig(FuzzedDataProvider& fdp)
{
    auto vsyncReceiver = CreateVSyncReceiver();
    if (vsyncReceiver == nullptr) {
        return;
    }
    vsyncReceiver->Init();
    int32_t bufferCount = fdp.ConsumeIntegralInRange<int32_t>(BUFFER_COUNT_MIN, BUFFER_COUNT_MAX);
    bool compositeSceneEnable = fdp.ConsumeBool();
    bool nativeDelayEnable = fdp.ConsumeBool();
    auto rsDvsyncAnimationList = CreateStringList(fdp);
    vsyncReceiver->SetUiDvsyncConfig(bufferCount, compositeSceneEnable, nativeDelayEnable, rsDvsyncAnimationList);
}

void DoSetNativeDvsyncSwitch(FuzzedDataProvider& fdp)
{
    auto vsyncReceiver = CreateVSyncReceiver();
    if (vsyncReceiver == nullptr) {
        return;
    }
    vsyncReceiver->Init();
    bool dvsyncSwitch = fdp.ConsumeBool();
    vsyncReceiver->SetNativeDVSyncSwitch(dvsyncSwitch);
}

void DoSetTouchEvent(FuzzedDataProvider& fdp)
{
    auto vsyncReceiver = CreateVSyncReceiver();
    if (vsyncReceiver == nullptr) {
        return;
    }
    vsyncReceiver->Init();
    int32_t touchType = fdp.ConsumeIntegralInRange<int32_t>(TOUCH_TYPE_MIN, TOUCH_TYPE_MAX);
    vsyncReceiver->SetTouchEvent(touchType);
}

} // namespace

} // namespace Rosen
} // namespace OHOS

extern "C" int LLVMFuzzerInitialize(int* argc, char*** argv)
{
    return 0;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < 1) {
        return 0;
    }

    FuzzedDataProvider fdp(data, size);

    uint8_t tarPos = fdp.ConsumeIntegral<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_INIT:
            OHOS::Rosen::DoInit(fdp);
            break;
        case OHOS::Rosen::DO_SET_VSYNC_RATE:
            OHOS::Rosen::DoSetVsyncRate(fdp);
            break;
        case OHOS::Rosen::DO_GET_VSYNC_PERIOD_AND_LAST_TIME_STAMP:
            OHOS::Rosen::DoGetVsyncPeriodAndLastTimeStamp(fdp);
            break;
        case OHOS::Rosen::DO_REQUEST_NEXT_VSYNC_WITH_PARAMS:
            OHOS::Rosen::DoRequestNextVsyncWithParams(fdp);
            break;
        case OHOS::Rosen::DO_SET_UI_DVSYNC_SWITCH:
            OHOS::Rosen::DoSetUiDvsyncSwitch(fdp);
            break;
        case OHOS::Rosen::DO_SET_VSYNC_CALL_BACK_FOR_EVERY_FRAME:
            OHOS::Rosen::DoSetVsyncCallBackForEveryFrame(fdp);
            break;
        case OHOS::Rosen::DO_SET_UI_DVSYNC_CONFIG:
            OHOS::Rosen::DoSetUiDvsyncConfig(fdp);
            break;
        case OHOS::Rosen::DO_SET_NATIVE_DVSYNC_SWITCH:
            OHOS::Rosen::DoSetNativeDvsyncSwitch(fdp);
            break;
        case OHOS::Rosen::DO_SET_TOUCH_EVENT:
            OHOS::Rosen::DoSetTouchEvent(fdp);
            break;
        default:
            break;
    }
    return 0;
}