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

#include "setFocusAppInfo_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "core/transaction/rs_interfaces.h"

namespace OHOS {
namespace Rosen {

// Global RSInterfaces instance, initialized in LLVMFuzzerInitialize
RSInterfaces* g_rsInterfaces = nullptr;

namespace {
const uint8_t DO_SET_FOCUS_APP_INFO = 0;
const uint8_t TARGET_SIZE = 1;
constexpr uint32_t STR_LEN = 10;

void DoSetFocusAppInfo(FuzzedDataProvider& fdp)
{
    int32_t pid = fdp.ConsumeIntegral<int32_t>();
    int32_t uid = fdp.ConsumeIntegral<int32_t>();
    std::string bundleName = fdp.ConsumeRandomLengthString(STR_LEN);
    std::string abilityName = fdp.ConsumeRandomLengthString(STR_LEN);
    uint64_t focusNodeId = fdp.ConsumeIntegral<uint64_t>();

    FocusAppInfo info = {
        .pid = pid,
        .uid = uid,
        .bundleName = bundleName,
        .abilityName = abilityName,
        .focusNodeId = focusNodeId
    };

    g_rsInterfaces->SetFocusAppInfo(info);
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
        case OHOS::Rosen::DO_SET_FOCUS_APP_INFO:
            OHOS::Rosen::DoSetFocusAppInfo(fdp);
            break;
        default:
            return -1;
    }
    return 0;
}
