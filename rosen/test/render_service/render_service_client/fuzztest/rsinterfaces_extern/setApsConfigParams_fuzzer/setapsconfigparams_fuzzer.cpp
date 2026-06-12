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

#include "setapsconfigparams_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "transaction/rs_interfaces.h"

namespace OHOS {
namespace Rosen {

RSInterfaces* g_rsInterfaces = nullptr;

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

    OHOS::Rosen::ApsEventType event =
        static_cast<OHOS::Rosen::ApsEventType>(fdp.ConsumeIntegralInRange<uint32_t>(0, 2000));
    uint32_t paramsSize = fdp.ConsumeIntegralInRange<uint32_t>(0, 128);
    std::unordered_map<std::string, std::string> params;
    for (uint32_t i = 0; i < paramsSize; ++i) {
        std::string key = fdp.ConsumeRandomLengthString();
        std::string value = fdp.ConsumeRandomLengthString();
        params[key] = value;
    }
    OHOS::Rosen::g_rsInterfaces->SetApsConfigParams(event, params);

    return 0;
}