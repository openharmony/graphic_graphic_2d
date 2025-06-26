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

#include "rsanimationmarshalling_fuzzer.h"

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

#include "animation/rs_animation_timing_protocol.h"
#include "animation/rs_cubic_bezier_interpolator.h"
#include "animation/rs_interpolator.h"
#include "animation/rs_frame_rate_range.h"
#include "animation/rs_render_animation.h"
#include "animation/rs_render_curve_animation.h"
#include "animation/rs_render_interpolating_spring_animation.h"
#include "animation/rs_render_keyframe_animation.h"
#include "animation/rs_render_transition.h"
#include "animation/rs_render_transition_effect.h"
#include "animation/rs_render_path_animation.h"
#include "animation/rs_render_spring_animation.h"
#include "animation/rs_spring_interpolator.h"
#include "animation/rs_steps_interpolator.h"
#include "render/rs_path.h"

namespace OHOS {
namespace Rosen {
namespace {
enum class FuzzMethod {
    CUBIC_BEZIER_INTERPOLATOR = 0,
    LINEAR_INTERPOLATOR,
    CUSTOM_INTERPOLATOR,
    SPRING_INTERPOLATOR,
    STEPS_INTERPOLATOR,
    RENDER_ANIMATION,
    RENDER_CURVE_ANIMATION,
    RENDER_INTERPOLATING_SPRING_ANIMATION,
    RENDER_KEYFRAMEANIMATION,
    RENDER_PATH_ANIMATION,
    RENDER_PROPERTY_ANIMATION,
    RENDER_SPRING_ANIMATION,
    RENDER_TRANSITION_EFFECT,
    RENDER_TRANSITION,
    END
};
enum RSTransitionEffectType : uint16_t {
    FADE = 1,
    SCALE,
    TRANSLATE,
    ROTATE,
    UNDEFINED,
};
} // namespace

/*
 * Test Marshalling and Unmarshalling method of the RSCubicBezierInterpolator class.
 */
void RSCubicBezierInterpolatorMarshalling(FuzzedDataProvider& FD)
{
    float ctlX1 = FD.ConsumeFloatingPoint<float>();
    float ctlY1 = FD.ConsumeFloatingPoint<float>();
    float ctlX2 = FD.ConsumeFloatingPoint<float>();
    float ctlY2 = FD.ConsumeFloatingPoint<float>();
    std::shared_ptr<RSInterpolator> interpolator =
        std::make_shared<RSCubicBezierInterpolator>(ctlX1, ctlY1, ctlX2, ctlY2);
    Parcel parcel;
    interpolator->Marshalling(parcel);
    interpolator = std::shared_ptr<RSInterpolator>(RSCubicBezierInterpolator::Unmarshalling(parcel));
    interpolator = std::shared_ptr<RSInterpolator>(RSInterpolator::Unmarshalling(parcel));
    interpolator.reset();
}

/*
 * Test the Unmarshalling method of the RSCubicBezierInterpolator class.
 */
void RSCubicBezierInterpolatorUnmarshalling(FuzzedDataProvider& FD)
{
    Parcel parcel;
    parcel.WriteUint16(InterpolatorType::CUBIC_BEZIER);
    parcel.WriteUint64(FD.ConsumeIntegral<uint64_t>());
    parcel.WriteFloat(FD.ConsumeFloatingPoint<float>());
    parcel.WriteFloat(FD.ConsumeFloatingPoint<float>());
    parcel.WriteFloat(FD.ConsumeFloatingPoint<float>());
    parcel.WriteFloat(FD.ConsumeFloatingPoint<float>());
    auto interpolator = std::shared_ptr<RSInterpolator>(RSCubicBezierInterpolator::Unmarshalling(parcel));
    interpolator = std::shared_ptr<RSInterpolator>(RSInterpolator::Unmarshalling(parcel));
    interpolator.reset();
}

/*
 * Test Marshalling and Unmarshalling method of the LinearInterpolator class.
 */
void LinearInterpolatorMarshalling(FuzzedDataProvider& FD)
{
    Parcel parcel;
    auto interpolator = std::make_shared<LinearInterpolator>();
    interpolator->Marshalling(parcel);
    auto parsedInterpolator = std::shared_ptr<RSInterpolator>(LinearInterpolator::Unmarshalling(parcel));
    parsedInterpolator = std::shared_ptr<RSInterpolator>(RSInterpolator::Unmarshalling(parcel));
    parsedInterpolator.reset();
}

/*
 * Test the Unmarshalling method of the LinearInterpolator class.
 */
void LinearInterpolatorUnmarshalling(FuzzedDataProvider& FD)
{
    Parcel parcel;
    parcel.WriteUint16(InterpolatorType::LINEAR);
    parcel.WriteUint64(FD.ConsumeIntegral<uint64_t>());
    auto interpolator = std::shared_ptr<RSInterpolator>(LinearInterpolator::Unmarshalling(parcel));
    interpolator = std::shared_ptr<RSInterpolator>(RSInterpolator::Unmarshalling(parcel));
    interpolator.reset();
}

/*
 * Test Marshalling and Unmarshalling method of the RSCustomInterpolator class.
 */
void RSCustomInterpolatorMarshalling(FuzzedDataProvider& FD)
{
    auto func = [](float value) -> bool { return value; };
    int duration = FD.ConsumeIntegral<int>();
    auto interpolator = std::make_shared<RSCustomInterpolator>(func, duration);
    Parcel parcel;
    interpolator->Marshalling(parcel);
    auto parsedInterpolator = std::shared_ptr<RSInterpolator>(RSCustomInterpolator::Unmarshalling(parcel));
    parsedInterpolator = std::shared_ptr<RSInterpolator>(RSInterpolator::Unmarshalling(parcel));
    parsedInterpolator.reset();
}

/*
 * Test Marshalling and Unmarshalling method of the RSSpringInterpolator class.
 */
void RSSpringInterpolatorMarshalling(FuzzedDataProvider& FD)
{
    uint64_t id = FD.ConsumeIntegral<uint64_t>();
    float response = FD.ConsumeFloatingPoint<float>();
    float dampingRatio = FD.ConsumeFloatingPoint<float>();
    float initialVelocity = FD.ConsumeFloatingPoint<float>();
    auto interpolator = std::make_shared<RSSpringInterpolator>(id, response, dampingRatio, initialVelocity);
    Parcel parcel;
    interpolator->Marshalling(parcel);
    auto parsedInterpolator = std::shared_ptr<RSInterpolator>(RSSpringInterpolator::Unmarshalling(parcel));
    parsedInterpolator = std::shared_ptr<RSInterpolator>(RSInterpolator::Unmarshalling(parcel));
    parsedInterpolator.reset();
}

/*
 * Test Marshalling and Unmarshalling method of the RSStepsInterpolator class.
 */
void RSStepsInterpolatorMarshalling(FuzzedDataProvider& FD)
{
    int32_t steps = FD.ConsumeIntegral<int32_t>();
    bool isEnd = FD.ConsumeBool();
    auto interpolator =
        std::make_shared<RSStepsInterpolator>(steps, isEnd ? StepsCurvePosition::END : StepsCurvePosition::START);
    Parcel parcel;
    interpolator->Marshalling(parcel);
    auto parsedInterpolator = std::shared_ptr<RSInterpolator>(RSStepsInterpolator::Unmarshalling(parcel));
    parsedInterpolator = std::shared_ptr<RSInterpolator>(RSInterpolator::Unmarshalling(parcel));
    parsedInterpolator.reset();
}

/*
 * Test Marshalling and Unmarshalling method of the RSRenderAnimation class.
 */
void RSRenderAnimationMarshalling(FuzzedDataProvider& FD)
{
    auto animationId = FD.ConsumeIntegral<AnimationId>();
    auto renderAnimation = std::make_shared<RSRenderAnimation>(animationId);
    renderAnimation->SetDuration(FD.ConsumeIntegral<int>());
    renderAnimation->SetStartDelay(FD.ConsumeIntegral<int>());
    renderAnimation->SetSpeed(FD.ConsumeFloatingPoint<float>());
    renderAnimation->SetRepeatCount(FD.ConsumeIntegral<int>());
    renderAnimation->SetAutoReverse(FD.ConsumeBool());
    renderAnimation->SetDirection(FD.ConsumeBool());
    std::vector<FillMode> fillModeVec = {FillMode::NONE, FillMode::FORWARDS, FillMode::BACKWARDS, FillMode::BOTH};
    renderAnimation->SetFillMode(fillModeVec[FD.ConsumeIntegralInRange<int>(0, fillModeVec.size() - 1)]);
    renderAnimation->SetRepeatCallbackEnable(FD.ConsumeBool());
    std::vector<ComponentScene> sceneVec = {UNKNOWN_SCENE, SWIPER_FLING};
    FrameRateRange frameRateRange(FD.ConsumeIntegral<int>(), FD.ConsumeIntegral<int>(), FD.ConsumeIntegral<int>(),
        FD.ConsumeIntegral<uint32_t>(), sceneVec[FD.ConsumeIntegralInRange<int>(0, sceneVec.size() - 1)]);
    renderAnimation->SetFrameRateRange(frameRateRange);
    renderAnimation->SetToken(FD.ConsumeIntegral<uint64_t>());
    Parcel parcel;
    renderAnimation->Marshalling(parcel);
    renderAnimation->ParseParam(parcel);
}

/*
 * Test Marshalling and Unmarshalling method of the RSRenderCurveAnimation class.
 */
void RSRenderCurveAnimationMarshalling(FuzzedDataProvider& FD)
{
    auto propertyId = FD.ConsumeIntegral<PropertyId>();
    auto originValue = std::make_shared<RSRenderAnimatableProperty<float>>(
        FD.ConsumeFloatingPoint<float>(), propertyId);
    auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(
        FD.ConsumeFloatingPoint<float>(), propertyId);
    auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(
        FD.ConsumeFloatingPoint<float>(), propertyId);
    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        FD.ConsumeIntegral<AnimationId>(), propertyId, originValue, startValue, endValue);
    auto interpolator = std::make_shared<LinearInterpolator>();
    bool needInterpolator = FD.ConsumeBool();
    renderCurveAnimation->SetInterpolator(needInterpolator ? interpolator : nullptr);
    Parcel parcel;
    renderCurveAnimation->Marshalling(parcel);
    auto renderAnimation = std::shared_ptr<RSRenderAnimation>(RSRenderCurveAnimation::Unmarshalling(parcel));
    renderAnimation.reset();
}

/*
 * Test Marshalling and Unmarshalling method of the RSRenderInterpolatingSpringAnimation class.
 */
void RSRenderInterpolatingSpringAnimationMarshalling(FuzzedDataProvider& FD)
{
    auto propertyId = FD.ConsumeIntegral<PropertyId>();
    auto originValue = std::make_shared<RSRenderAnimatableProperty<float>>(
        FD.ConsumeFloatingPoint<float>(), propertyId);
    auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(
        FD.ConsumeFloatingPoint<float>(), propertyId);
    auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(
        FD.ConsumeFloatingPoint<float>(), propertyId);
    auto renderInterpolatingSpringAnimation = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        FD.ConsumeIntegral<AnimationId>(), propertyId, originValue, startValue, endValue);
    float response = FD.ConsumeFloatingPoint<float>();
    float dampingRatio = FD.ConsumeFloatingPoint<float>();
    float normalizedInitialVelocity = FD.ConsumeFloatingPoint<float>();
    float minimumAmplitudeRatio = FD.ConsumeFloatingPoint<float>();
    renderInterpolatingSpringAnimation->SetSpringParameters(
        response, dampingRatio, normalizedInitialVelocity, minimumAmplitudeRatio);
    renderInterpolatingSpringAnimation->SetZeroThreshold(FD.ConsumeFloatingPoint<float>());
    Parcel parcel;
    renderInterpolatingSpringAnimation->Marshalling(parcel);
    auto renderAnimation =
        std::shared_ptr<RSRenderAnimation>(RSRenderInterpolatingSpringAnimation::Unmarshalling(parcel));
    renderAnimation.reset();
}

/*
 * Test Marshalling and Unmarshalling method of the RSRenderKeyframeAnimation class.
 */
void RSRenderKeyframeAnimationMarshalling(FuzzedDataProvider& FD)
{
    auto propertyId = FD.ConsumeIntegral<PropertyId>();
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(
        FD.ConsumeFloatingPoint<float>(), propertyId);
    auto renderKeyframeAnimation = std::make_shared<RSRenderKeyframeAnimation>(
        FD.ConsumeIntegral<AnimationId>(), propertyId, property);
    auto keyFrameNum = FD.ConsumeIntegralInRange<int>(0, 5); // max key frame number
    for (int i = 0; i <= keyFrameNum; i++) {
        auto fraction = FD.ConsumeFloatingPoint<float>();
        auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(FD.ConsumeFloatingPoint<float>());
        auto interpolator = std::make_shared<RSStepsInterpolator>(FD.ConsumeIntegral<int32_t>());
        renderKeyframeAnimation->AddKeyframe(fraction, endValue, interpolator);
    }
    auto durationKeyFrameNum = FD.ConsumeIntegralInRange<int>(0, 5); // max key frame number
    for (int i = 0; i <= durationKeyFrameNum; i++) {
        auto startDuration = FD.ConsumeIntegral<int>();
        auto endDuration = FD.ConsumeIntegral<int>();
        auto value = std::make_shared<RSRenderAnimatableProperty<float>>(
            FD.ConsumeFloatingPoint<float>(), FD.ConsumeIntegral<PropertyId>());
        auto interpolator = std::make_shared<RSStepsInterpolator>(FD.ConsumeIntegral<int32_t>());
        renderKeyframeAnimation->AddKeyframe(startDuration, endDuration, value, interpolator);
    }
    renderKeyframeAnimation->SetDurationKeyframe(FD.ConsumeBool());
    Parcel parcel;
    renderKeyframeAnimation->Marshalling(parcel);
    auto renderAnimation = std::shared_ptr<RSRenderAnimation>(RSRenderKeyframeAnimation::Unmarshalling(parcel));
    renderAnimation.reset();
}

/*
 * Test Marshalling and Unmarshalling method of the RSRenderPathAnimation class.
 */
void RSRenderPathAnimationMarshalling(FuzzedDataProvider& FD)
{
    auto originPosition = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(
        Vector2f(FD.ConsumeFloatingPoint<float>(), FD.ConsumeFloatingPoint<float>()));
    auto startPosition = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(
        Vector2f(FD.ConsumeFloatingPoint<float>(), FD.ConsumeFloatingPoint<float>()));
    auto endPosition = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(
        Vector2f(FD.ConsumeFloatingPoint<float>(), FD.ConsumeFloatingPoint<float>()));
    auto animationId = FD.ConsumeIntegral<AnimationId>();
    auto propertyId = FD.ConsumeIntegral<PropertyId>();
    float originRotation = FD.ConsumeFloatingPoint<float>();
    auto path = RSPath::CreateRSPath("L350 0 L150 100");
    auto renderPathAnimation = std::make_shared<RSRenderPathAnimation>(
        animationId, propertyId, originPosition, startPosition, endPosition, originRotation, path);
    auto interpolator = std::make_shared<RSStepsInterpolator>(FD.ConsumeIntegral<int32_t>());
    bool needInterpolator = FD.ConsumeBool();
    renderPathAnimation->SetInterpolator(needInterpolator ? interpolator : nullptr);
    Parcel parcel;
    renderPathAnimation->Marshalling(parcel);
    auto renderAnimation = std::shared_ptr<RSRenderAnimation>(RSRenderPathAnimation::Unmarshalling(parcel));
    renderAnimation.reset();
}

/*
 * Test Marshalling and Unmarshalling method of the RSRenderPropertyAnimation class.
 */
void RSRenderPropertyAnimationMarshalling(FuzzedDataProvider& FD)
{
    auto animationId = FD.ConsumeIntegral<AnimationId>();
    auto propertyId = FD.ConsumeIntegral<PropertyId>();
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(
        FD.ConsumeFloatingPoint<float>(), propertyId);
    auto renderPropertyAnimation = std::make_shared<RSRenderPropertyAnimation>(
        animationId, propertyId, property);
    renderPropertyAnimation->SetAdditive(FD.ConsumeBool());
    Parcel parcel;
    renderPropertyAnimation->Marshalling(parcel);
    renderPropertyAnimation->ParseParam(parcel);
}

/*
 * Test Marshalling and Unmarshalling method of the RSRenderSpringAnimation class.
 */
void RSRenderSpringAnimationMarshalling(FuzzedDataProvider& FD)
{
    auto originValue = std::make_shared<RSRenderAnimatableProperty<float>>(
        FD.ConsumeFloatingPoint<float>(), FD.ConsumeIntegral<PropertyId>());
    auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(
        FD.ConsumeFloatingPoint<float>(), FD.ConsumeIntegral<PropertyId>());
    auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(
        FD.ConsumeFloatingPoint<float>(), FD.ConsumeIntegral<PropertyId>());
    auto animationId = FD.ConsumeIntegral<AnimationId>();
    auto propertyId = FD.ConsumeIntegral<PropertyId>();
    auto renderSpringAnimation = std::make_shared<RSRenderSpringAnimation>(
        animationId, propertyId, originValue, startValue, endValue);
    float response = FD.ConsumeFloatingPoint<float>();
    float dampingRatio = FD.ConsumeFloatingPoint<float>();
    float blendDuration = FD.ConsumeFloatingPoint<float>();
    float minimumAmplitudeRatio = FD.ConsumeFloatingPoint<float>();
    renderSpringAnimation->SetSpringParameters(response, dampingRatio, blendDuration, minimumAmplitudeRatio);
    renderSpringAnimation->SetZeroThreshold(FD.ConsumeFloatingPoint<float>());
    Parcel parcel;
    renderSpringAnimation->Marshalling(parcel);
    auto renderAnimation = std::shared_ptr<RSRenderAnimation>(RSRenderSpringAnimation::Unmarshalling(parcel));
    renderAnimation.reset();
}

/*
 * Create RSRenderTransitionEffect object.
 */
std::shared_ptr<RSRenderTransitionEffect> CreateTransitionEffect(FuzzedDataProvider& FD)
{
    std::vector<int> typeVec = { RSTransitionEffectType::FADE, RSTransitionEffectType::SCALE,
        RSTransitionEffectType::TRANSLATE, RSTransitionEffectType::ROTATE };
    auto transitionType = typeVec[FD.ConsumeIntegralInRange<int>(0, typeVec.size() - 1)];
    std::shared_ptr<RSRenderTransitionEffect> transitionEffect = nullptr;
    if (transitionType == RSTransitionEffectType::FADE) {
        transitionEffect = std::make_shared<RSTransitionFade>(FD.ConsumeFloatingPoint<float>());
    } else if (transitionType == RSTransitionEffectType::SCALE) {
        transitionEffect = std::make_shared<RSTransitionScale>(
            FD.ConsumeFloatingPoint<float>(), FD.ConsumeFloatingPoint<float>(), FD.ConsumeFloatingPoint<float>());
    } else if (transitionType == RSTransitionEffectType::TRANSLATE) {
        transitionEffect = std::make_shared<RSTransitionTranslate>(
            FD.ConsumeFloatingPoint<float>(), FD.ConsumeFloatingPoint<float>(), FD.ConsumeFloatingPoint<float>());
    } else {
        transitionEffect = std::make_shared<RSTransitionRotate>(FD.ConsumeFloatingPoint<float>(),
            FD.ConsumeFloatingPoint<float>(), FD.ConsumeFloatingPoint<float>(), FD.ConsumeFloatingPoint<float>());
    }
    return transitionEffect;
}

/*
 * Test Marshalling and Unmarshalling method of the RSRenderTransitionEffect class.
 */
void RSRenderTransitionEffectMarshalling(FuzzedDataProvider& FD)
{
    Parcel parcel;
    std::shared_ptr<RSRenderTransitionEffect> transitionEffect = CreateTransitionEffect(FD);
    if (transitionEffect) {
        transitionEffect->Marshalling(parcel);
    } else if (FD.ConsumeBool()) {
        parcel.WriteUint16(
            FD.ConsumeIntegralInRange<int>(RSTransitionEffectType::FADE, RSTransitionEffectType::UNDEFINED));
    }
    transitionEffect = std::shared_ptr<RSRenderTransitionEffect>(RSRenderTransitionEffect::Unmarshalling(parcel));
    transitionEffect.reset();
}

/*
 * Test Marshalling and Unmarshalling method of the RSRenderTransition class.
 */
void RSRenderTransitionMarshalling(FuzzedDataProvider& FD)
{
    int effectNum = FD.ConsumeIntegralInRange<int>(0, 8); // max effect number
    std::vector<std::shared_ptr<RSRenderTransitionEffect>> effects = {};
    for (int i = 0; i <= effectNum; i++) {
        auto effect = CreateTransitionEffect(FD);
        if (effect) {
            effects.push_back(effect);
        }
    }
    auto animationId = FD.ConsumeIntegral<AnimationId>();
    auto isTransitionIn = FD.ConsumeBool();
    auto transition = std::make_shared<RSRenderTransition>(animationId, effects, isTransitionIn);
    Parcel parcel;
    transition->Marshalling(parcel);
    auto renderAnimation = std::shared_ptr<RSRenderAnimation>(RSRenderTransition::Unmarshalling(parcel));
    renderAnimation.reset();
}

/*
 * Test all method.
 */
bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    FuzzedDataProvider FD(data, size);
    uint8_t methodCount = static_cast<uint8_t>(FuzzMethod::END) + 1;
    auto method = static_cast<FuzzMethod>(FD.ConsumeIntegral<uint8_t>() % methodCount);
    if (method == FuzzMethod::CUBIC_BEZIER_INTERPOLATOR) {
        RSCubicBezierInterpolatorMarshalling(FD);
        RSCubicBezierInterpolatorUnmarshalling(FD);
    } else if (method == FuzzMethod::LINEAR_INTERPOLATOR) {
        LinearInterpolatorMarshalling(FD);
        LinearInterpolatorUnmarshalling(FD);
    } else if (method == FuzzMethod::CUSTOM_INTERPOLATOR) {
        RSCustomInterpolatorMarshalling(FD);
    } else if (method == FuzzMethod::SPRING_INTERPOLATOR) {
        RSSpringInterpolatorMarshalling(FD);
    } else if (method == FuzzMethod::STEPS_INTERPOLATOR) {
        RSStepsInterpolatorMarshalling(FD);
    } else if (method == FuzzMethod::RENDER_ANIMATION) {
        RSRenderAnimationMarshalling(FD);
    } else if (method == FuzzMethod::RENDER_CURVE_ANIMATION) {
        RSRenderCurveAnimationMarshalling(FD);
    } else if (method == FuzzMethod::RENDER_INTERPOLATING_SPRING_ANIMATION) {
        RSRenderInterpolatingSpringAnimationMarshalling(FD);
    } else if (method == FuzzMethod::RENDER_KEYFRAMEANIMATION) {
        RSRenderKeyframeAnimationMarshalling(FD);
    } else if (method == FuzzMethod::RENDER_PATH_ANIMATION) {
        RSRenderPathAnimationMarshalling(FD);
    } else if (method == FuzzMethod::RENDER_PROPERTY_ANIMATION) {
        RSRenderPropertyAnimationMarshalling(FD);
    } else if (method == FuzzMethod::RENDER_SPRING_ANIMATION) {
        RSRenderSpringAnimationMarshalling(FD);
    } else if (method == FuzzMethod::RENDER_TRANSITION_EFFECT) {
        RSRenderTransitionEffectMarshalling(FD);
    } else if (method == FuzzMethod::RENDER_TRANSITION) {
        RSRenderTransitionMarshalling(FD);
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
