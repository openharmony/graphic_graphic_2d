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

#include "behindwindowfilter_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "transaction/rs_interfaces.h"

namespace OHOS {
namespace Rosen {

// Global RSInterfaces instance, initialized in LLVMFuzzerInitialize
RSInterfaces* g_rsInterfaces = nullptr;

namespace {
const uint8_t DO_SET_BEHIND_WINDOW_FILTER_ENABLED = 0;
const uint8_t DO_GET_BEHIND_WINDOW_FILTER_ENABLED = 1;
const uint8_t TARGET_SIZE = 2;

void DoSetBehindWindowFilterEnabled(FuzzedDataProvider& fdp)
{
    bool enabled = fdp.ConsumeBool();
    g_rsInterfaces->SetBehindWindowFilterEnabled(enabled);
}

void DoGetBehindWindowFilterEnabled(FuzzedDataProvider& fdp)
{
    bool enabled = fdp.ConsumeBool();
    bool result = g_rsInterfaces->GetBehindWindowFilterEnabled(enabled);
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
        case OHOS::Rosen::DO_SET_BEHIND_WINDOW_FILTER_ENABLED:
            OHOS::Rosen::DoSetBehindWindowFilterEnabled(fdp);
            break;
        case OHOS::Rosen::DO_GET_BEHIND_WINDOW_FILTER_ENABLED:
            OHOS::Rosen::DoGetBehindWindowFilterEnabled(fdp);
            break;
        default:
            return -1;
    }
    return 0;
}
