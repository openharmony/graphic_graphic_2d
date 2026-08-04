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

#include "setoverlaydisplaymode_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "transaction/rs_interfaces.h"

namespace OHOS {
namespace Rosen {

// Global RSInterfaces instance, initialized in LLVMFuzzerInitialize
RSInterfaces* g_rsInterfaces = nullptr;

namespace {
const uint8_t DO_SET_OVERLAY_DISPLAY_MODE = 0;
const uint8_t DO_SEND_VIDEO_RATE_INFO = 1;
const uint8_t TARGET_SIZE = 1;

void DoSetOverlayDisplayMode(FuzzedDataProvider& fdp)
{
#ifdef RS_ENABLE_OVERLAY_DISPLAY
    int32_t mode = fdp.ConsumeIntegral<int32_t>();
    g_rsInterfaces->SetOverlayDisplayMode(mode);
#endif
}

void DoSendVideoRateInfo(FuzzedDataProvider& fdp)
{
    std::unordered_map<std::string, std::string> videoRateInfo;
    uint8_t mapSize = fdp.ConsumeIntegral<uint8_t>() % 16;
    for (uint8_t i = 0; i < mapSize; i++) {
        std::string key = fdp.ConsumeRandomLengthString(64);
        std::string value = fdp.ConsumeRandomLengthString(128);
        videoRateInfo[key] = value;
    }
    g_rsInterfaces->SendVideoRateInfo(videoRateInfo);
}

} // anonymous namespace
} // namespace Rosen
} // namespace OHOS

/* Fuzzer environment initialization */
extern "C" int LLVMFuzzerInitialize(int* argc, char*** argv)
{
    // Initialize RSInterfaces
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
        case OHOS::Rosen::DO_SET_OVERLAY_DISPLAY_MODE:
            OHOS::Rosen::DoSetOverlayDisplayMode(fdp);
            break;
        case OHOS::Rosen::DO_SEND_VIDEO_RATE_INFO:
            OHOS::Rosen::DoSendVideoRateInfo(fdp);
            break;
        default:
            return -1;
    }
    return 0;
}
