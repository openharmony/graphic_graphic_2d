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

#include "rsspringmodelstandard_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <fuzzer/FuzzedDataProvider.h>
#include <hilog/log.h>
#include <memory>
#include <securec.h>
#include <unistd.h>

#include "animation/rs_spring_model.h"

namespace OHOS {
namespace Rosen {

bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    FuzzedDataProvider FD(data, size);
    float response = FD.ConsumeFloatingPoint<float>();
    float dampingRatio = FD.ConsumeFloatingPoint<float>();
    float initialOffset = FD.ConsumeFloatingPoint<float>();
    float initialVelocity = FD.ConsumeFloatingPoint<float>();
    float minimumAmplitude = FD.ConsumeFloatingPoint<float>();
    double time = FD.ConsumeFloatingPoint<double>();
    auto model = std::make_shared<RSSpringModel<float>>(
        response, dampingRatio, initialOffset, initialVelocity, minimumAmplitude);
    model->CalculateDisplacement(time);
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}