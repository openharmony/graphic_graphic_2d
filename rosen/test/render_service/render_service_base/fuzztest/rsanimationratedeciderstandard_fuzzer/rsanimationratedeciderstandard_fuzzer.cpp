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
    ADDDECISIONELEMENT = 0,
    MAKEDECISION,
    CALCULATEPREFERREDRATE,
    PROCESSVECTOR4F,
    PROCESSVECTOR2F,
    PROCESSFLOAT,
    END
};
} // namespace

/*
 * Test the CalculatePreferredRate method of the RSAnimationRateDecider class.
 */
void CalculatePreferredRate(FuzzedDataProvider& FD, RSAnimationRateDecider& decider)
{
    PropertyId propertyId = FD.ConsumeIntegral<PropertyId>();
    PropertyValue property = std::make_shared<RSRenderAnimatableProperty<float>>(
        FD.ConsumeFloatingPoint<float>(), propertyId, RSPropertyUnit::PIXEL_POSITION);
    int32_t frameRate = FD.ConsumeIntegral<int32_t>();
    auto frameRateGetFunc = [frameRate](const RSPropertyUnit unit, float velocity, int32_t area,
                                int32_t length) -> int32_t { return frameRate; };
    decider.CalculatePreferredRate(property, frameRateGetFunc);
}

/*
 * Test the AddDecisionElement method of the RSAnimationRateDecider class.
 */
void AddDecisionElement(FuzzedDataProvider& FD, RSAnimationRateDecider& decider)
{
    decider.SetEnable(FD.ConsumeBool());
    PropertyId propertyId = FD.ConsumeIntegral<PropertyId>();
    PropertyValue velocity = std::make_shared<RSRenderAnimatableProperty<float>>(
        FD.ConsumeFloatingPoint<float>(), propertyId, RSPropertyUnit::PIXEL_POSITION);
    int min = FD.ConsumeIntegral<int>();
    int max = FD.ConsumeIntegral<int>();
    int preferred = FD.ConsumeIntegral<int>();
    uint32_t type = FD.ConsumeIntegral<uint32_t>();
    std::vector<ComponentScene> sceneVec = { ComponentScene::UNKNOWN_SCENE, ComponentScene::SWIPER_FLING };
    ComponentScene componentScene = sceneVec[FD.ConsumeIntegralInRange<int>(0, sceneVec.size() - 1)];
    FrameRateRange range(min, max, preferred, type, componentScene);
    decider.AddDecisionElement(propertyId, velocity, range);
}

/*
 * Test the MakeDecision method of the RSAnimationRateDecider class.
 */
void MakeDecision(FuzzedDataProvider& FD, RSAnimationRateDecider& decider)
{
    AddDecisionElement(FD, decider);
    if (FD.ConsumeBool()) {
        int32_t frameRate = FD.ConsumeIntegral<int32_t>();
        auto frameRateGetFunc = [frameRate](const RSPropertyUnit unit, float velocity, int32_t area,
                                    int32_t length) -> int32_t { return frameRate; };
        decider.MakeDecision(frameRateGetFunc);
    } else {
        decider.MakeDecision(nullptr);
    }
}

/*
 * Get RSPropertyUnit.
 */
RSPropertyUnit GetRSPropertyUnit(FuzzedDataProvider& FD)
{
    std::vector<RSPropertyUnit> unitVec = { RSPropertyUnit::UNKNOWN, RSPropertyUnit::PIXEL_POSITION,
        RSPropertyUnit::PIXEL_SIZE, RSPropertyUnit::RATIO_SCALE, RSPropertyUnit::ANGLE_ROTATION };
    auto unit = unitVec[FD.ConsumeIntegralInRange<int>(0, unitVec.size() - 1)];
    return unit;
}

/*
 * Test the ProcessVector4f method of the RSAnimationRateDecider class.
 */
void ProcessVector4f(FuzzedDataProvider& FD, RSAnimationRateDecider& decider)
{
    Vector4f value(FD.ConsumeFloatingPoint<float>(), FD.ConsumeFloatingPoint<float>(), FD.ConsumeFloatingPoint<float>(),
        FD.ConsumeFloatingPoint<float>());
    auto propertyId = FD.ConsumeIntegral<PropertyId>();
    auto unit = GetRSPropertyUnit(FD);
    PropertyValue property = std::make_shared<RSRenderAnimatableProperty<Vector4f>>(value, propertyId, unit);
    int32_t frameRate = FD.ConsumeIntegral<int32_t>();
    auto frameRateGetFunc = [frameRate](const RSPropertyUnit unit, float velocity, int32_t area,
                                int32_t length) -> int32_t { return frameRate; };
    decider.ProcessVector4f(property, frameRateGetFunc);
}

/*
 * Test the ProcessVector2f method of the RSAnimationRateDecider class.
 */
void ProcessVector2f(FuzzedDataProvider& FD, RSAnimationRateDecider& decider)
{
    Vector2f value(FD.ConsumeFloatingPoint<float>(), FD.ConsumeFloatingPoint<float>());
    auto propertyId = FD.ConsumeIntegral<PropertyId>();
    auto unit = GetRSPropertyUnit(FD);
    PropertyValue property = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(value, propertyId, unit);
    int32_t frameRate = FD.ConsumeIntegral<int32_t>();
    auto frameRateGetFunc = [frameRate](const RSPropertyUnit unit, float velocity, int32_t area,
                                int32_t length) -> int32_t { return frameRate; };
    decider.ProcessVector2f(property, frameRateGetFunc);
}

/*
 * Test the ProcessFloat method of the RSAnimationRateDecider class.
 */
void ProcessFloat(FuzzedDataProvider& FD, RSAnimationRateDecider& decider)
{
    float value = FD.ConsumeFloatingPoint<float>();
    auto propertyId = FD.ConsumeIntegral<PropertyId>();
    auto unit = GetRSPropertyUnit(FD);
    PropertyValue property = std::make_shared<RSRenderAnimatableProperty<float>>(value, propertyId, unit);
    int32_t frameRate = FD.ConsumeIntegral<int32_t>();
    auto frameRateGetFunc = [frameRate](const RSPropertyUnit unit, float velocity, int32_t area,
                                int32_t length) -> int32_t { return frameRate; };
    decider.ProcessFloat(property, frameRateGetFunc);
}

/*
 * Test all method of the RSAnimationRateDecider class.
 */
bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    FuzzedDataProvider FD(data, size);
    RSAnimationRateDecider decider;
    uint8_t methodCount = static_cast<uint8_t>(FuzzMethod::END) + 1;
    switch (static_cast<FuzzMethod>(FD.ConsumeIntegral<uint8_t>() % methodCount)) {
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
        case FuzzMethod::PROCESSVECTOR4F: {
            ProcessVector4f(FD, decider);
            break;
        }
        case FuzzMethod::PROCESSVECTOR2F: {
            ProcessVector2f(FD, decider);
            break;
        }
        case FuzzMethod::PROCESSFLOAT: {
            ProcessFloat(FD, decider);
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
