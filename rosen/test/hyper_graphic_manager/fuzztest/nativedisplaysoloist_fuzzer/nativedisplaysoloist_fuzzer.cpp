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

#include "nativedisplaysoloist_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "feature/hyper_graphic_manager/rs_ui_display_soloist.h"
#include "native_display_soloist.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t DO_CREATE = 0;
const uint8_t DO_START = 1;
const uint8_t DO_STOP = 2;
const uint8_t DO_SET_EXPECTED_FRAME_RATE_RANGE = 3;
const uint8_t TARGET_SIZE = 4;

OH_DisplaySoloist* g_displaySoloist = nullptr;

void DoCreate(FuzzedDataProvider& fdp)
{
    if (g_displaySoloist != nullptr) {
        OH_DisplaySoloist_Destroy(g_displaySoloist);
        g_displaySoloist = nullptr;
    }
    bool useExclusiveThread = fdp.ConsumeBool();
    g_displaySoloist = OH_DisplaySoloist_Create(useExclusiveThread);
    RSDisplaySoloistManager::GetInstance().idToSoloistMap_.clear();
    OH_DisplaySoloist_Destroy(g_displaySoloist);
    g_displaySoloist = nullptr;
}

void DoStart(FuzzedDataProvider& fdp)
{
    bool useNullSoloist = fdp.ConsumeBool();
    OH_DisplaySoloist* soloist = useNullSoloist ? nullptr : g_displaySoloist;
    bool useNullCallback = fdp.ConsumeBool();
    OH_DisplaySoloist_FrameCallback callback = nullptr;

    if (!useNullCallback) {
        callback = [](long long timestamp, long long targetTimestamp, void* data) {
            (void)timestamp;
            (void)targetTimestamp;
            (void)data;
        };
    }
    OH_DisplaySoloist_Start(soloist, callback, nullptr);
}

void DoStop(FuzzedDataProvider& fdp)
{
    bool useNullSoloist = fdp.ConsumeBool();
    OH_DisplaySoloist* soloist = useNullSoloist ? nullptr : g_displaySoloist;
    OH_DisplaySoloist_Stop(soloist);
}

void DoSetExpectedFrameRateRange(FuzzedDataProvider& fdp)
{
    bool useNullSoloist = fdp.ConsumeBool();
    OH_DisplaySoloist* soloist = useNullSoloist ? nullptr : g_displaySoloist;

    bool useNullRange = fdp.ConsumeBool();
    if (useNullRange) {
        OH_DisplaySoloist_SetExpectedFrameRateRange(soloist, nullptr);
        return;
    }

    DisplaySoloist_ExpectedRateRange range;
    range.min = fdp.ConsumeIntegral<int32_t>();
    range.max = fdp.ConsumeIntegral<int32_t>();
    range.expected = fdp.ConsumeIntegral<int32_t>();
    OH_DisplaySoloist_SetExpectedFrameRateRange(soloist, &range);
}
} // namespace
} // namespace Rosen
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0) {
        return -1;
    }

    FuzzedDataProvider fdp(data, size);

    uint8_t tarPos = fdp.ConsumeIntegral<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_CREATE:
            OHOS::Rosen::DoCreate(fdp);
            break;
        case OHOS::Rosen::DO_START:
            OHOS::Rosen::DoStart(fdp);
            break;
        case OHOS::Rosen::DO_STOP:
            OHOS::Rosen::DoStop(fdp);
            break;
        case OHOS::Rosen::DO_SET_EXPECTED_FRAME_RATE_RANGE:
            OHOS::Rosen::DoSetExpectedFrameRateRange(fdp);
            break;
        default:
            return -1;
    }
    return 0;
}
