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

#include "refreshRate_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>
#include <string>
#include <unordered_map>

#include "transaction/rs_interfaces.h"

namespace OHOS {
namespace Rosen {

// Global RSInterfaces instance, initialized in LLVMFuzzerInitialize
RSInterfaces* g_rsInterfaces = nullptr;

namespace {
constexpr size_t STR_LEN = 10;
const uint8_t DO_SET_SCREEN_REFRESH_RATE = 0;
const uint8_t DO_SET_REFRESH_RATE_MODE = 1;
const uint8_t DO_GET_SCREEN_CURRENT_REFRESH_RATE = 2;
const uint8_t DO_GET_SCREEN_SUPPORTED_REFRESH_RATES = 3;
const uint8_t DO_SET_SHOW_REFRESH_RATE_ENABLED = 4;
const uint8_t DO_GET_REALTIME_REFRESH_RATE = 5;
const uint8_t DO_SET_WINDOW_EXPECTED_REFRESH_RATE_BY_ID = 6;
const uint8_t DO_SET_WINDOW_EXPECTED_REFRESH_RATE_BY_VSYNC = 7;
const uint8_t DO_SYNC_FRAME_RATE_RANGE = 8;
const uint8_t TARGET_SIZE = 9;

void DoSetScreenRefreshRate(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    int32_t sceneId = fdp.ConsumeIntegral<int32_t>();
    int32_t rate = fdp.ConsumeIntegral<int32_t>();
    g_rsInterfaces->SetScreenRefreshRate(id, sceneId, rate);
}

void DoSetRefreshRateMode(FuzzedDataProvider& fdp)
{
    int32_t refreshRateMode = fdp.ConsumeIntegral<int32_t>();
    g_rsInterfaces->SetRefreshRateMode(refreshRateMode);
}

void DoGetScreenCurrentRefreshRate(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    g_rsInterfaces->GetScreenCurrentRefreshRate(id);
}

void DoGetScreenSupportedRefreshRates(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    g_rsInterfaces->GetScreenSupportedRefreshRates(id);
}

void DoSetShowRefreshRateEnabled(FuzzedDataProvider& fdp)
{
    bool enabled = fdp.ConsumeBool();
    int32_t type = fdp.ConsumeIntegral<int32_t>();
    g_rsInterfaces->SetShowRefreshRateEnabled(enabled, type);
}

void DoGetRealtimeRefreshRate(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    g_rsInterfaces->GetRealtimeRefreshRate(id);
}

void DoSetWindowExpectedRefreshRateById(FuzzedDataProvider& fdp)
{
    uint8_t mapSize = fdp.ConsumeIntegral<uint8_t>();
    std::unordered_map<uint64_t, EventInfo> eventInfos;
    for (auto i = 0; i < mapSize; i++) {
        uint64_t winId = fdp.ConsumeIntegral<uint64_t>();
        EventInfo eventInfo;
        eventInfo.eventName = fdp.ConsumeRandomLengthString(STR_LEN);
        eventInfo.eventStatus = fdp.ConsumeBool();
        eventInfo.minRefreshRate = fdp.ConsumeIntegral<uint32_t>();
        eventInfo.maxRefreshRate = fdp.ConsumeIntegral<uint32_t>();
        eventInfo.description = fdp.ConsumeRandomLengthString(STR_LEN);
        eventInfos.insert({winId, eventInfo});
    }
    g_rsInterfaces->SetWindowExpectedRefreshRate(eventInfos);
}

void DoSetWindowExpectedRefreshRateByVsync(FuzzedDataProvider& fdp)
{
    uint8_t mapSize = fdp.ConsumeIntegral<uint8_t>();
    std::unordered_map<std::string, EventInfo> eventInfos;
    for (auto i = 0; i < mapSize; i++) {
        std::string vsyncName = fdp.ConsumeRandomLengthString(STR_LEN);
        EventInfo eventInfo;
        eventInfo.eventName = fdp.ConsumeRandomLengthString(STR_LEN);
        eventInfo.eventStatus = fdp.ConsumeBool();
        eventInfo.minRefreshRate = fdp.ConsumeIntegral<uint32_t>();
        eventInfo.maxRefreshRate = fdp.ConsumeIntegral<uint32_t>();
        eventInfo.description = fdp.ConsumeRandomLengthString(STR_LEN);
        eventInfos.insert({vsyncName, eventInfo});
    }
    g_rsInterfaces->SetWindowExpectedRefreshRate(eventInfos);
}

void DoSyncFrameRateRange(FuzzedDataProvider& fdp)
{
    uint64_t id = fdp.ConsumeIntegral<uint64_t>();
    int32_t min = fdp.ConsumeIntegral<int32_t>();
    int32_t max = fdp.ConsumeIntegral<int32_t>();
    int32_t preferred = fdp.ConsumeIntegral<int32_t>();
    uint32_t type = fdp.ConsumeIntegral<uint32_t>();
    FrameRateRange range(min, max, preferred, type);
    int32_t animatorExpectedFrameRate = fdp.ConsumeIntegral<int32_t>();
    g_rsInterfaces->SyncFrameRateRange(id, range, animatorExpectedFrameRate);
}

} // namespace

} // namespace Rosen
} // namespace OHOS

/* Fuzzer environment initialization */
extern "C" int LLVMFuzzerInitialize(int *argc, char ***argv)
{
    // Pre-initialize RSInterfaces singleton to avoid runtime initialization overhead
    OHOS::Rosen::g_rsInterfaces = &OHOS::Rosen::RSInterfaces::GetInstance();
    return 0;
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (OHOS::Rosen::g_rsInterfaces == nullptr || data == nullptr) {
        return -1;
    }

    FuzzedDataProvider fdp(data, size);

    /* Run your code on data */
    uint8_t tarPos = fdp.ConsumeIntegral<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_SET_SCREEN_REFRESH_RATE:
            OHOS::Rosen::DoSetScreenRefreshRate(fdp);
            break;
        case OHOS::Rosen::DO_SET_REFRESH_RATE_MODE:
            OHOS::Rosen::DoSetRefreshRateMode(fdp);
            break;
        case OHOS::Rosen::DO_GET_SCREEN_CURRENT_REFRESH_RATE:
            OHOS::Rosen::DoGetScreenCurrentRefreshRate(fdp);
            break;
        case OHOS::Rosen::DO_GET_SCREEN_SUPPORTED_REFRESH_RATES:
            OHOS::Rosen::DoGetScreenSupportedRefreshRates(fdp);
            break;
        case OHOS::Rosen::DO_SET_SHOW_REFRESH_RATE_ENABLED:
            OHOS::Rosen::DoSetShowRefreshRateEnabled(fdp);
            break;
        case OHOS::Rosen::DO_GET_REALTIME_REFRESH_RATE:
            OHOS::Rosen::DoGetRealtimeRefreshRate(fdp);
            break;
        case OHOS::Rosen::DO_SET_WINDOW_EXPECTED_REFRESH_RATE_BY_ID:
            OHOS::Rosen::DoSetWindowExpectedRefreshRateById(fdp);
            break;
        case OHOS::Rosen::DO_SET_WINDOW_EXPECTED_REFRESH_RATE_BY_VSYNC:
            OHOS::Rosen::DoSetWindowExpectedRefreshRateByVsync(fdp);
            break;
        case OHOS::Rosen::DO_SYNC_FRAME_RATE_RANGE:
            OHOS::Rosen::DoSyncFrameRateRange(fdp);
            break;
        default:
            return -1;
    }
    return 0;
}
