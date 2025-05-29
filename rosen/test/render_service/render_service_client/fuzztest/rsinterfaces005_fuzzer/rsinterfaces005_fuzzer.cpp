/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
const uint8_t DO_SET_SCREEN_CHANGE_CALLBACK = 0;
const uint8_t DO_SET_SCREEN_ACTIVE_MODE = 1;
const uint8_t DO_SET_SCREEN_REFRESH_RATE = 2;
const uint8_t DO_SET_REFRESH_RATE_MODE = 3;
const uint8_t DO_SYNC_FRAME_RATE_RANGE = 4;
const uint8_t DO_UNREGISTER_FRAME_RATE_LINKER = 5;
const uint8_t DO_GET_SCREEN_CURRENT_REFRESH_RATE = 6;
const uint8_t DO_GET_CURRENT_REFRESH_RATE_MODE = 7;
const uint8_t DO_GET_SCREEN_SUPPORTED_REFRESH_RATES = 8;
const uint8_t DO_GET_SHOW_REFRESH_RATE_ENABLED = 9;
const uint8_t DO_SET_SHOW_REFRESH_RATE_ENABLED = 10;
const uint8_t DO_GET_REALTIME_REFRESH_RATE = 11;
const uint8_t DO_GET_REFRESH_INFO = 12;
const uint8_t DO_REFRESH_RATE_UPDATE_CALLBACK = 13;
const uint8_t DO_REGISTER_FRAME_RATE_LINKER_EXPECTED_FPS_CALLBACK = 14;
const uint8_t DO_REFRESH_RATE_MODE_CHANGE_CALLBACK = 15;
const uint8_t DO_GET_REFRESH_INFO_TO_SP = 16;
const uint8_t TARGET_SIZE = 17;

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

template<>
std::string GetData()
{
    size_t objectSize = GetData<uint8_t>();
    std::string object(objectSize, '\0');
    if (DATA == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    object.assign(reinterpret_cast<const char*>(DATA + g_pos), objectSize);
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

namespace Mock {

} // namespace Mock

void DoSetScreenChangeCallback()
{}

void DoSetScreenActiveMode()
{}

void DoSetScreenRefreshRate()
{}

void DoSetRefreshRateMode()
{}

void DoSyncFrameRateRange()
{}

void DoUnregisterFrameRateLinker()
{}

void DoGetScreenCurrentRefreshRate()
{}

void DoGetCurrentRefreshRateMode()
{}

void DoGetScreenSupportedRefreshRates()
{}

void DoGetShowRefreshRateEnabled()
{}

void DoSetShowRefreshRateEnabled()
{}

void DoGetRealtimeRefreshRate()
{}

void DoGetRefreshInfo()
{}

void DoRegisterHgmRefreshRateUpdateCallback()
{}

void DoRegisterFrameRateLinkerExpectedFpsUpdateCallback()
{}

void DoRegisterHgmRefreshRateModeChangeCallback()
{}

void DoGetRefreshInfoToSP()
{}
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
        case OHOS::Rosen::DO_SET_SCREEN_CHANGE_CALLBACK:
            OHOS::Rosen::DoSetScreenChangeCallback();
            break;
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
        case OHOS::Rosen::DO_UNREGISTER_FRAME_RATE_LINKER:
            OHOS::Rosen::DoUnregisterFrameRateLinker();
            break;
        case OHOS::Rosen::DO_GET_SCREEN_CURRENT_REFRESH_RATE:
            OHOS::Rosen::DoGetScreenCurrentRefreshRate();
            break;
        case OHOS::Rosen::DO_GET_CURRENT_REFRESH_RATE_MODE:
            OHOS::Rosen::DoGetCurrentRefreshRateMode();
            break;
        case OHOS::Rosen::DO_GET_SCREEN_SUPPORTED_REFRESH_RATES:
            OHOS::Rosen::DoGetScreenSupportedRefreshRates();
            break;
        case OHOS::Rosen::DO_GET_SHOW_REFRESH_RATE_ENABLED:
            OHOS::Rosen::DoGetShowRefreshRateEnabled();
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
        case OHOS::Rosen::DO_GET_REFRESH_INFO_TO_SP:
            OHOS::Rosen::DoGetRefreshInfoToSP();
            break;
        default:
            return -1;
    }
    return 0;
}
