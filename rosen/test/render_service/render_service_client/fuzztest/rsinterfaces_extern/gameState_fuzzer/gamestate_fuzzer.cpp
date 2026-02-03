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

#include "gamestate_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "core/transaction/rs_interfaces.h"
#include "transaction/rs_render_service_client_info.h"

namespace OHOS {
namespace Rosen {

// Global RSInterfaces instance, initialized in LLVMFuzzerInitialize
RSInterfaces* g_rsInterfaces = nullptr;

namespace {
const uint8_t DO_REPORT_GAME_STATE_DATA = 0;
const uint8_t TARGET_SIZE = 1;
constexpr uint32_t STR_LEN = 50;

void DoReportGameStateData(FuzzedDataProvider& fdp)
{
    // Construct GameStateData from fuzz data
    GameStateData info = {
        .pid = fdp.ConsumeIntegral<int32_t>(),
        .uid = fdp.ConsumeIntegral<int32_t>(),
        .state = fdp.ConsumeIntegral<int32_t>(),
        .renderTid = fdp.ConsumeIntegral<int32_t>(),
        .bundleName = fdp.ConsumeRandomLengthString(STR_LEN)
    };

    // Call the interface
    g_rsInterfaces->ReportGameStateData(info);
}

} // anonymous namespace

} // namespace Rosen
} // namespace OHOS

/* Fuzzer environment initialization */
extern "C" int LLVMFuzzerInitialize(int* argc, char*** argv)
{
    // Initialize RSInterfaces
    OHOS::Rosen::g_rsInterfaces = &RSInterfaces::GetInstance();
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
        case OHOS::Rosen::DO_REPORT_GAME_STATE_DATA:
            OHOS::Rosen::DoReportGameStateData(fdp);
            break;
        default:
            return -1;
    }
    return 0;
}
