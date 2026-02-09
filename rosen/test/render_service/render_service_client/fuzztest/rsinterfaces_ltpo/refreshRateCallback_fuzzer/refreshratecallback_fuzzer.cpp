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

#include "refreshratecallback_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>
#include <string>

#include "transaction/rs_interfaces.h"

namespace OHOS {
namespace Rosen {

// Global RSInterfaces instance, initialized in LLVMFuzzerInitialize
RSInterfaces* g_rsInterfaces = nullptr;

namespace {
constexpr size_t STR_LEN = 10;
const uint8_t DO_REGISTER_HGM_CONFIG_CHANGE_CALLBACK = 0;
const uint8_t DO_REGISTER_HGM_REFRESH_RATE_UPDATE_CALLBACK = 1;
const uint8_t DO_REGISTER_HGM_REFRESH_RATE_MODE_CHANGE_CALLBACK = 2;
const uint8_t DO_REGISTER_FRAME_RATE_LINKER_EXPECTED_FPS_CALLBACK = 3;
const uint8_t DO_UNREGISTER_FRAME_RATE_LINKER_EXPECTED_FPS_CALLBACK = 4;
const uint8_t TARGET_SIZE = 5;

void DoRegisterHgmConfigChangeCallback(FuzzedDataProvider& fdp)
{
    auto callback = [](std::shared_ptr<RSHgmConfigData> data) {
        (void)data;
    };
    g_rsInterfaces->RegisterHgmConfigChangeCallback(callback);
}

void DoRegisterHgmRefreshRateUpdateCallback(FuzzedDataProvider& fdp)
{
    auto callback = [](int32_t rate) {
        (void)rate;
    };
    g_rsInterfaces->RegisterHgmRefreshRateUpdateCallback(callback);
}

void DoRegisterHgmRefreshRateModeChangeCallback(FuzzedDataProvider& fdp)
{
    auto callback = [](int32_t mode) {
        (void)mode;
    };
    g_rsInterfaces->RegisterHgmRefreshRateModeChangeCallback(callback);
}

void DoRegisterFrameRateLinkerExpectedFpsUpdateCallback(FuzzedDataProvider& fdp)
{
    int32_t dstPid = fdp.ConsumeIntegral<int32_t>();
    int32_t actualPid = fdp.ConsumeIntegral<int32_t>();
    std::string actualXcomponentId = fdp.ConsumeRandomLengthString(STR_LEN);
    int32_t actualFps = fdp.ConsumeIntegral<int32_t>();
    auto callback = [actualPid, actualXcomponentId, actualFps](
                        int32_t pid, const std::string& xcomponentId, int32_t fps) {
        (void)actualPid;
        (void)actualXcomponentId;
        (void)actualFps;
        (void)pid;
        (void)xcomponentId;
        (void)fps;
    };
    g_rsInterfaces->RegisterFrameRateLinkerExpectedFpsUpdateCallback(dstPid, callback);
}

void DoUnRegisterFrameRateLinkerExpectedFpsUpdateCallback(FuzzedDataProvider& fdp)
{
    int32_t dstPid = fdp.ConsumeIntegral<int32_t>();
    g_rsInterfaces->UnRegisterFrameRateLinkerExpectedFpsUpdateCallback(dstPid);
}

} // namespace

} // namespace Rosen
} // namespace OHOS

/* Fuzzer environment initialization */
extern "C" int LLVMFuzzerInitialize(int* argc, char*** argv)
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
        case OHOS::Rosen::DO_REGISTER_HGM_CONFIG_CHANGE_CALLBACK:
            OHOS::Rosen::DoRegisterHgmConfigChangeCallback(fdp);
            break;
        case OHOS::Rosen::DO_REGISTER_HGM_REFRESH_RATE_UPDATE_CALLBACK:
            OHOS::Rosen::DoRegisterHgmRefreshRateUpdateCallback(fdp);
            break;
        case OHOS::Rosen::DO_REGISTER_HGM_REFRESH_RATE_MODE_CHANGE_CALLBACK:
            OHOS::Rosen::DoRegisterHgmRefreshRateModeChangeCallback(fdp);
            break;
        case OHOS::Rosen::DO_REGISTER_FRAME_RATE_LINKER_EXPECTED_FPS_CALLBACK:
            OHOS::Rosen::DoRegisterFrameRateLinkerExpectedFpsUpdateCallback(fdp);
            break;
        case OHOS::Rosen::DO_UNREGISTER_FRAME_RATE_LINKER_EXPECTED_FPS_CALLBACK:
            OHOS::Rosen::DoUnRegisterFrameRateLinkerExpectedFpsUpdateCallback(fdp);
            break;
        default:
            return -1;
    }
    return 0;
}
