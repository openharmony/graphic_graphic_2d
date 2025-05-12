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

#include "rsanimationratedeciderstandard_fuzzer.h"

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

#include "animation/rs_animation_rate_decider.h"
#include "modifier/rs_render_property.h"

namespace OHOS {
namespace Rosen {
namespace {
enum class FuzzMethod {
    SETENABLE = 0,
    SETNODESIZE,
    SETNODESCALE,
    ADDDECISIONELEMENT,
    MAKEDECISION,
    CALCULATEPREFERREDRATE,
    END
};
} // namespace

void CalculatePreferredRate(FuzzedDataProvider& FD, RSAnimationRateDecider& decider)
{
    PropertyValue property = std::make_shared<RSRenderAnimatableProperty<float>>(
        0.0, 1, RSRenderPropertyType::PROPERTY_FLOAT, RSPropertyUnit::PIXEL_POSITION);
    property->SetPropertyType(RSRenderPropertyType::PROPERTY_VECTOR4F);
    int32_t frameRate = FD.ConsumeIntegral<int32_t>();
    auto frameRateGetFunc =
        [frameRate](const RSPropertyUnit unit, float velocity, int32_t area, int32_t length) -> int32_t {
            return frameRate;
        };
    decider.CalculatePreferredRate(property, frameRateGetFunc);
}

void AddDecisionElement(FuzzedDataProvider& FD, RSAnimationRateDecider& decider)
{
    PropertyValue velocity;
    FrameRateRange range(FD.ConsumeIntegral<int>(), FD.ConsumeIntegral<int>(), FD.ConsumeIntegral<int>());
    decider.AddDecisionElement(FD.ConsumeIntegral<uint64_t>(), velocity, range);
}

void MakeDecision(FuzzedDataProvider& FD, RSAnimationRateDecider& decider)
{
    int32_t frameRate = FD.ConsumeIntegral<int32_t>();
    auto frameRateGetFunc =
        [frameRate](const RSPropertyUnit unit, float velocity, int32_t area, int32_t length) -> int32_t {
            return frameRate;
        };
    decider.MakeDecision(frameRateGetFunc);
}

bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    FuzzedDataProvider FD(data, size);
    RSAnimationRateDecider decider;
    uint8_t methodCount = static_cast<uint8_t>(FuzzMethod::END) + 1;
    switch (static_cast<FuzzMethod>(FD.ConsumeIntegral<uint8_t>() % methodCount)) {
        case FuzzMethod::SETENABLE: {
            decider.SetEnable(FD.ConsumeBool());
            break;
        }
        case FuzzMethod::SETNODESIZE: {
            decider.SetNodeSize(FD.ConsumeFloatingPoint<float>(), FD.ConsumeFloatingPoint<float>());
            break;
        }
        case FuzzMethod::SETNODESCALE: {
            decider.SetNodeScale(FD.ConsumeFloatingPoint<float>(), FD.ConsumeFloatingPoint<float>());
            break;
        }
        case FuzzMethod::ADDDECISIONELEMENT: {
            AddDecisionElement(FD, decider);
            break;
        }
        case FuzzMethod::MAKEDECISION: {
            MakeDecision(FD, decider);
            break;
        }
        case FuzzMethod::CALCULATEPREFERREDRATE: {
            CalculatePreferredRate(FD, decider);
            break;
        }
        default:
            return false;
    }
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
