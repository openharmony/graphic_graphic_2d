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

#include "rsinterfaces005_fuzzer.h"

#include <securec.h>

#include "transaction/rs_interfaces.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t DO_SET_SCREEN_ACTIVE_MODE = 0;
const uint8_t DO_SET_SCREEN_REFRESH_RATE = 1;
const uint8_t DO_SET_REFRESH_RATE_MODE = 2;
const uint8_t DO_SYNC_FRAME_RATE_RANGE = 3;
const uint8_t DO_GET_SCREEN_CURRENT_REFRESH_RATE = 4;
const uint8_t DO_GET_SCREEN_SUPPORTED_REFRESH_RATES = 5;
const uint8_t DO_SET_SHOW_REFRESH_RATE_ENABLED = 6;
const uint8_t DO_GET_REALTIME_REFRESH_RATE = 7;
const uint8_t DO_GET_REFRESH_INFO = 8;
const uint8_t DO_REFRESH_RATE_UPDATE_CALLBACK = 9;
const uint8_t DO_REGISTER_FRAME_RATE_LINKER_EXPECTED_FPS_CALLBACK = 10;
const uint8_t DO_REFRESH_RATE_MODE_CHANGE_CALLBACK = 11;
const uint8_t TARGET_SIZE = 12;

const uint8_t* DATA = nullptr;
size_t g_size = 0;
size_t g_pos;

template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (DATA == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, DATA + g_pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_pos += objectSize;
    return object;
}

bool Init(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    DATA = data;
    g_size = size;
    g_pos = 0;
    return true;
}
} // namespace

void DoSetScreenActiveMode()
{
#ifndef ROSEN_ARKUI_X
    ScreenId id = GetData<uint64_t>();
    uint32_t modeId = GetData<uint32_t>();
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SetScreenActiveMode(id, modeId);
#endif
}

void DoSetScreenRefreshRate()
{
    ScreenId id = GetData<ScreenId>();
    int32_t sceneId = GetData<int32_t>();
    int32_t rate = GetData<int32_t>();
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SetScreenRefreshRate(id, sceneId, rate);
}

void DoSetRefreshRateMode()
{
    int32_t refreshRateMode = GetData<int32_t>();
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SetRefreshRateMode(refreshRateMode);
}

void DoSyncFrameRateRange()
{
    uint64_t id = GetData<uint64_t>();
    FrameRateRange range;
    int32_t animatorExpectedFrameRate = GetData<int32_t>();
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SyncFrameRateRange(id, range, animatorExpectedFrameRate);
}

void DoGetScreenCurrentRefreshRate()
{
    ScreenId id = GetData<ScreenId>();
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.GetScreenCurrentRefreshRate(id);
}

void DoGetScreenSupportedRefreshRates()
{
    ScreenId id = GetData<ScreenId>();
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.GetScreenSupportedRefreshRates(id);
}

void DoSetShowRefreshRateEnabled()
{
    bool enabled = GetData<bool>();
    int32_t type = GetData<int32_t>();
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SetShowRefreshRateEnabled(enabled, type);
}

void DoGetRealtimeRefreshRate()
{
    ScreenId id = GetData<ScreenId>();
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.GetRealtimeRefreshRate(id);
}

void DoGetRefreshInfo()
{
    pid_t pid = GetData<pid_t>();
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.GetRefreshInfo(pid);
}

void DoRegisterHgmRefreshRateUpdateCallback()
{
    HgmRefreshRateUpdateCallback callback;
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.RegisterHgmRefreshRateUpdateCallback(callback);
}

void DoRegisterFrameRateLinkerExpectedFpsUpdateCallback()
{
    uint32_t dstPid = GetData<uint32_t>();
    FrameRateLinkerExpectedFpsUpdateCallback callback;
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.RegisterFrameRateLinkerExpectedFpsUpdateCallback(dstPid, callback);
}

void DoRegisterHgmRefreshRateModeChangeCallback()
{
    HgmRefreshRateModeChangeCallback callback;
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.RegisterHgmRefreshRateModeChangeCallback(callback);
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (!OHOS::Rosen::Init(data, size)) {
        return -1;
    }
    /* Run your code on data */
    uint8_t tarPos = OHOS::Rosen::GetData<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_SET_SCREEN_ACTIVE_MODE:
            OHOS::Rosen::DoSetScreenActiveMode();
            break;
        case OHOS::Rosen::DO_SET_SCREEN_REFRESH_RATE:
            OHOS::Rosen::DoSetScreenRefreshRate();
            break;
        case OHOS::Rosen::DO_SET_REFRESH_RATE_MODE:
            OHOS::Rosen::DoSetRefreshRateMode();
            break;
        case OHOS::Rosen::DO_SYNC_FRAME_RATE_RANGE:
            OHOS::Rosen::DoSyncFrameRateRange();
            break;
        case OHOS::Rosen::DO_GET_SCREEN_CURRENT_REFRESH_RATE:
            OHOS::Rosen::DoGetScreenCurrentRefreshRate();
            break;
        case OHOS::Rosen::DO_GET_SCREEN_SUPPORTED_REFRESH_RATES:
            OHOS::Rosen::DoGetScreenSupportedRefreshRates();
            break;
        case OHOS::Rosen::DO_SET_SHOW_REFRESH_RATE_ENABLED:
            OHOS::Rosen::DoSetShowRefreshRateEnabled();
            break;
        case OHOS::Rosen::DO_GET_REALTIME_REFRESH_RATE:
            OHOS::Rosen::DoGetRealtimeRefreshRate();
            break;
        case OHOS::Rosen::DO_GET_REFRESH_INFO:
            OHOS::Rosen::DoGetRefreshInfo();
            break;
        case OHOS::Rosen::DO_REFRESH_RATE_UPDATE_CALLBACK:
            OHOS::Rosen::DoRegisterHgmRefreshRateUpdateCallback();
            break;
        case OHOS::Rosen::DO_REGISTER_FRAME_RATE_LINKER_EXPECTED_FPS_CALLBACK:
            OHOS::Rosen::DoRegisterFrameRateLinkerExpectedFpsUpdateCallback();
            break;
        case OHOS::Rosen::DO_REFRESH_RATE_MODE_CHANGE_CALLBACK:
            OHOS::Rosen::DoRegisterHgmRefreshRateModeChangeCallback();
            break;
        default:
            return -1;
    }
    return 0;
}
