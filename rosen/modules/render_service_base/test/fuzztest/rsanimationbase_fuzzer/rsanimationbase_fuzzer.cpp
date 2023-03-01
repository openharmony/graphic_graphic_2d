/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "rsanimationbase_fuzzer.h"

#include <securec.h>

#include "animation/rs_animation_fraction.h"
#include "animation/rs_animation_manager.h"
#include "animation/rs_animation_timing_protocol.h"
#include "animation/rs_cubic_bezier_interpolator.h"
#include "animation/rs_interpolator.h"
#include "animation/rs_render_curve_animation.h"
#include "animation/rs_render_keyframe_animation.h"
#include "animation/rs_render_path_animation.h"
#include "animation/rs_render_spring_animation.h"
#include "animation/rs_spring_interpolator.h"
#include "animation/rs_steps_interpolator.h"
#include "animation/rs_transition_effect.h"

namespace OHOS {
    using namespace Rosen;
    namespace {
        const uint8_t* data_ = nullptr;
        size_t size_ = 0;
        size_t pos;
    }

    /*
    * describe: get data from outside untrusted data(data_) which size is according to sizeof(T)
    * tips: only support basic type
    */
    template<class T>
    T GetData()
    {
        T object {};
        size_t objectSize = sizeof(object);
        if (data_ == nullptr || objectSize > size_ - pos) {
            return object;
        }
        errno_t ret = memcpy_s(&object, objectSize, data_ + pos, objectSize);
        if (ret != EOK) {
            return {};
        }
        pos += objectSize;
        return object;
    }

    /*
    * get a string from data_
    */
    std::string GetStringFromData(int strlen)
    {
        char cstr[strlen];
        cstr[strlen - 1] = '\0';
        for (int i = 0; i < strlen - 1; i++) {
            cstr[i] = GetData<char>();
        }
        std::string str(cstr);
        return str;
    }

    void RSAnimationFractionFuzzerTest()
    {
        // get data
        float animationScale = GetData<float>();
        int64_t time = GetData<int64_t>();
        bool isInStartDelay = GetData<bool>();
        bool isFinished = GetData<bool>();
        float fraction = GetData<float>();
        int remainTime = GetData<int>();
        ForwardDirection direction = GetData<ForwardDirection>();
        int64_t lastFrameTime = GetData<int64_t>();

        // test
        RSAnimationFraction::Init();
        RSAnimationFraction::GetAnimationScale();
        RSAnimationFraction::SetAnimationScale(animationScale);
        auto animationFraction = std::make_shared<RSAnimationFraction>();
        animationFraction->GetAnimationFraction(time, isInStartDelay, isFinished);
        animationFraction->UpdateRemainTimeFraction(fraction, remainTime);
        animationFraction->GetStartFraction();
        animationFraction->GetEndFraction();
        animationFraction->SetDirectionAfterStart(direction);
        animationFraction->SetLastFrameTime(lastFrameTime);
    }

    void RSAnimationManagerFuzzerTest()
    {
        // get data
        AnimationId keyId = GetData<AnimationId>();
        AnimationId id = GetData<AnimationId>();
        pid_t pid = GetData<pid_t>();
        int64_t time = GetData<int64_t>();

        PropertyId propertyId = GetData<PropertyId>();
        AnimationId animId = GetData<AnimationId>();

        // test
        auto animationManager = std::make_shared<RSAnimationManager>();
        animationManager->RemoveAnimation(keyId);
        animationManager->GetAnimation(id);
        animationManager->FilterAnimationByPid(pid);
        animationManager->Animate(time, false);
        animationManager->RegisterSpringAnimation(propertyId, animId);
        animationManager->UnregisterSpringAnimation(propertyId, animId);
        animationManager->QuerySpringAnimation(propertyId);
    }

    void RSAnimationTimingProtocolFuzzerTest()
    {
        // get data
        int duration = GetData<int>();
        int startDelay = GetData<int>();
        float speed = GetData<float>();
        int repeatCount = GetData<int>();
        bool autoReverse = GetData<bool>();
        FillMode fillMode = GetData<FillMode>();
        bool isForward = GetData<bool>();

        // test
        auto animationTimeProtocol = std::make_shared<RSAnimationTimingProtocol>();
        animationTimeProtocol->SetDuration(duration);
        animationTimeProtocol->SetStartDelay(startDelay);
        animationTimeProtocol->SetSpeed(speed);
        animationTimeProtocol->SetRepeatCount(repeatCount);
        animationTimeProtocol->SetAutoReverse(autoReverse);
        animationTimeProtocol->SetFillMode(fillMode);
        animationTimeProtocol->SetDirection(isForward);
        animationTimeProtocol->GetDuration();
        animationTimeProtocol->GetStartDelay();
        animationTimeProtocol->GetSpeed();
        animationTimeProtocol->GetRepeatCount();
        animationTimeProtocol->GetAutoReverse();
        animationTimeProtocol->GetFillMode();
        animationTimeProtocol->GetDirection();
    }

    void RSCubicBezierInterpolatorFuzzerTest()
    {
        // get data
        float x1 = GetData<float>();
        float x2 = GetData<float>();
        float y1 = GetData<float>();
        float y2 = GetData<float>();
        float input = GetData<float>();
        Parcel parcel;

        // test
        auto interpolator = std::make_shared<RSCubicBezierInterpolator>(x1, y1, x2, y2);
        interpolator->Interpolate(input);
        interpolator->Marshalling(parcel);
        auto copyInterpolator =
            std::shared_ptr<RSCubicBezierInterpolator>(RSCubicBezierInterpolator::Unmarshalling(parcel));
    }

    void RSSpringInterpolatorFuzzerTest()
    {
        // get data
        float response = GetData<float>();
        float dampingRatio = GetData<float>();
        float initialVelocity = GetData<float>();
        float fraction = GetData<float>();

        // test
        auto animation = std::make_shared<RSSpringInterpolator>(response, dampingRatio, initialVelocity);
        Parcel parcel;
        animation->Marshalling(parcel);
        auto copyInterpolator = std::shared_ptr<RSSpringInterpolator>(RSSpringInterpolator::Unmarshalling(parcel));
        animation->Interpolate(fraction);
    }

    void RSStepsInterpolatorFuzzerTest()
    {
        // get data
        int32_t steps = GetData<int32_t>();
        StepsCurvePosition position = GetData<StepsCurvePosition>();
        float fraction = GetData<float>();

        // test
        auto animation = std::make_shared<RSStepsInterpolator>(steps, position);
        Parcel parcel;
        animation->Marshalling(parcel);
        auto copyInterpolator = std::shared_ptr<RSStepsInterpolator>(RSStepsInterpolator::Unmarshalling(parcel));
        animation->Interpolate(fraction);
    }

    void RSTransitionEffectFuzzerTest()
    {
        // get data
        float x1 = GetData<float>();
        float x2 = GetData<float>();
        float y1 = GetData<float>();
        float y2 = GetData<float>();
        float opacity = GetData<float>();
        Vector3f scale = Vector3f(x1, x2, y1);
        Vector3f translate = Vector3f(x2, y1, y2);
        Vector4f axisAngle = Vector4f(x1, x2, y1, y2);
        // test
        auto animation = RSTransitionEffect::Create();
        animation->Opacity(opacity);
        animation->Scale(scale);
        animation->Translate(translate);
        animation->Rotate(axisAngle);
    }

    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }
        // initialize
        data_ = data;
        size_ = size;
        pos = 0;

        RSAnimationFractionFuzzerTest();
        RSAnimationManagerFuzzerTest();
        RSAnimationTimingProtocolFuzzerTest();
        RSCubicBezierInterpolatorFuzzerTest();
        RSSpringInterpolatorFuzzerTest();
        RSStepsInterpolatorFuzzerTest();
        RSTransitionEffectFuzzerTest();
        return true;
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}

