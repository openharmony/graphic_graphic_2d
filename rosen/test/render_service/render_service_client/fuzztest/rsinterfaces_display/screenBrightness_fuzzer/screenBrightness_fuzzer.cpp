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

#include "screenBrightness_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "transaction/rs_interfaces.h"

namespace OHOS {
namespace Rosen {

RSInterfaces* g_rsInterfaces = nullptr;

namespace {
const uint8_t DO_SET_BRIGHTNESS_INFO_CHANGE_CALLBACK = 0;
const uint8_t DO_GET_BRIGHTNESS_INFO = 1;
const uint8_t TARGET_SIZE = 2;

void DoSetBrightnessInfoChangeCallback(FuzzedDataProvider& fdp)
{
    BrightnessInfoChangeCallback callback = [](ScreenId id, BrightnessInfo info) {
        (void)id;
        (void)info;
    };
    g_rsInterfaces->SetBrightnessInfoChangeCallback(callback);
}

void DoGetBrightnessInfo(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    BrightnessInfo brightnessInfo;
    brightnessInfo.currentHeadroom = fdp.ConsumeFloatingPoint<float>();
    brightnessInfo.maxHeadroom = fdp.ConsumeFloatingPoint<float>();
    brightnessInfo.sdrNits = fdp.ConsumeFloatingPoint<float>();
    g_rsInterfaces->GetBrightnessInfo(id, brightnessInfo);
}

} // namespace

} // namespace Rosen
} // namespace OHOS

/* Fuzzer environment initialization */
extern "C" int LLVMFuzzerInitialize(int* argc, char*** argv)
{
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
    uint8_t tarPos = fdp.ConsumeIntegral<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_SET_BRIGHTNESS_INFO_CHANGE_CALLBACK:
            OHOS::Rosen::DoSetBrightnessInfoChangeCallback(fdp);
            break;
        case OHOS::Rosen::DO_GET_BRIGHTNESS_INFO:
            OHOS::Rosen::DoGetBrightnessInfo(fdp);
            break;
        default:
            return -1;
    }
    return 0;
}
