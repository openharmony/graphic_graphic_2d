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

#include "rsanimationspring_fuzzer.h"

#include <securec.h>

#include "animation/rs_animation_fraction.h"
#include "animation/rs_animation_manager.h"
#include "animation/rs_animation_timing_protocol.h"
#include "animation/rs_animation_trace_utils.h"
#include "animation/rs_cubic_bezier_interpolator.h"
#include "animation/rs_interpolator.h"
#include "animation/rs_render_animation.h"
#include "animation/rs_render_curve_animation.h"
#include "animation/rs_render_interactive_implict_animator.h"
#include "animation/rs_render_interactive_implict_animator_map.h"
#include "animation/rs_render_interpolating_spring_animation.h"
#include "animation/rs_render_keyframe_animation.h"
#include "animation/rs_render_path_animation.h"
#include "animation/rs_render_spring_animation.h"
#include "animation/rs_render_transition.h"
#include "animation/rs_render_transition_effect.h"
#include "animation/rs_spring_interpolator.h"
#include "animation/rs_spring_model.h"
#include "animation/rs_steps_interpolator.h"
#include "animation/rs_value_estimator.h"
#include "command/rs_message_processor.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_render_node.h"
#include "render/rs_path.h"

namespace OHOS {
using namespace Rosen;
namespace {
constexpr size_t STR_LEN = 10;
const uint8_t* data_ = nullptr;
size_t size_ = 0;
size_t pos;
} // namespace

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

void RSRenderInterpolatingSpringAnimationFuzzerTest1()
{
    // get data
    auto animationId = GetData<AnimationId>();
    auto propertyId = GetData<PropertyId>();
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
    auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
    auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
    auto response = GetData<float>();
    auto dampingRatio = GetData<float>();
    auto normalizedInitialVelocity = GetData<float>();
    auto minimumAmplitudeRatio = GetData<float>();
    auto zeroThreshold = GetData<float>();
    std::string out;

    // test
    auto animation =
        std::make_shared<RSRenderInterpolatingSpringAnimation>(animationId, propertyId, property, startValue, endValue);
    animation->DumpAnimationInfo(out);
    animation->SetSpringParameters(response, dampingRatio, normalizedInitialVelocity, minimumAmplitudeRatio);
    animation->SetZeroThreshold(zeroThreshold);
    Parcel parcel;
    animation->Marshalling(parcel);
    auto copyAnimation = std::shared_ptr<RSRenderInterpolatingSpringAnimation>(
        RSRenderInterpolatingSpringAnimation::Unmarshalling(parcel));
}

void RSRenderInterpolatingSpringAnimationFuzzerTest2()
{
    // get data
    auto animationId = GetData<AnimationId>();
    auto propertyId = GetData<PropertyId>();
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
    auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
    auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
    auto response = GetData<float>();
    auto dampingRatio = GetData<float>();
    auto normalizedInitialVelocity = GetData<float>();
    auto minimumAmplitudeRatio = GetData<float>();
    auto zeroThreshold = GetData<float>();
    auto fraction = GetData<float>();
    auto time = GetData<int64_t>();

    // test
    auto animation =
        std::make_shared<RSRenderInterpolatingSpringAnimation>(animationId, propertyId, property, startValue, endValue);
    animation->property_ = property;
    animation->SetSpringParameters(response, dampingRatio, normalizedInitialVelocity, minimumAmplitudeRatio);
    animation->OnSetFraction(fraction);
    animation->OnAnimate(fraction);
    animation->CalculateVelocity(fraction);
    animation->SetZeroThreshold(zeroThreshold);

    animation->InitValueEstimator();
    animation->UpdateFractionAfterContinue();
    animation->CalculateTimeFraction(fraction);
    animation->OnInitialize(time);
    animation->GetNeedLogicallyFinishCallback();
    animation->CallLogicallyFinishCallback();
    RSMessageProcessor::Instance().GetAllTransactions().clear();
}

void RSSpringValueEstimatorFuzzerTest()
{
    // get data
    bool isAdditive = GetData<bool>();
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
    auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
    auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
    auto lastValue = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
    float response = GetData<float>();
    float dampingRatio = GetData<float>();
    float time = GetData<float>();

    // test
    auto springBaseValueEstimator = std::make_shared<RSSpringValueEstimatorBase>();
    springBaseValueEstimator->SetResponse(response);
    springBaseValueEstimator->SetDampingRatio(dampingRatio);
    springBaseValueEstimator->GetResponse();
    springBaseValueEstimator->GetDampingRatio();
    springBaseValueEstimator->SetInitialVelocity(property);
    springBaseValueEstimator->InitRSSpringValueEstimator(property, startValue, endValue, lastValue);
    springBaseValueEstimator->UpdateStartValueAndLastValue(startValue, lastValue);
    springBaseValueEstimator->UpdateAnimationValue(time, isAdditive);
    springBaseValueEstimator->GetPropertyVelocity(time);
    springBaseValueEstimator->UpdateDuration();
    springBaseValueEstimator->UpdateSpringParameters();

    auto springValueEstimator = std::make_shared<RSSpringValueEstimator<float>>();
    springValueEstimator->InitSpringModel();
    springValueEstimator->SetResponse(response);
    springValueEstimator->SetDampingRatio(dampingRatio);
    springValueEstimator->GetResponse();
    springValueEstimator->GetDampingRatio();
    springValueEstimator->SetInitialVelocity(property);
    springValueEstimator->InitRSSpringValueEstimator(property, startValue, endValue, lastValue);
    springValueEstimator->UpdateStartValueAndLastValue(startValue, lastValue);
    springValueEstimator->UpdateAnimationValue(time, isAdditive);
    springValueEstimator->GetAnimationValue(time, isAdditive);
    springValueEstimator->GetAnimationProperty();
    springValueEstimator->GetPropertyVelocity(time);
    springValueEstimator->UpdateDuration();
    springValueEstimator->UpdateSpringParameters();
}

void RSSpringModelFuzzerTest()
{
    // get data
    float initialOffset = GetData<float>();
    float initialVelocity = GetData<float>();
    float response = GetData<float>();
    float dampingRatio = GetData<float>();
    float minimumAmplitude = GetData<float>();
    double time = GetData<double>();

    // test
    std::make_shared<RSSpringModel<float>>();
    auto model1 = std::make_shared<RSSpringModel<float>>(
        response, dampingRatio, initialOffset, initialVelocity, minimumAmplitude);
    model1->CalculateDisplacement(time);
    model1->EstimateDuration();
}

void RSRenderSpringAnimationFuzzerTest()
{
    // get data
    float response = GetData<float>();
    float dampingRatio = GetData<float>();
    float blendDuration = GetData<float>();
    float minimumAmplitudeRatio = GetData<float>();
    float zeroThreshold = GetData<float>();
    AnimationId animationId = GetData<AnimationId>();
    PropertyId propertyId = GetData<PropertyId>();
    auto originValue = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
    auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
    auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
    auto velocity = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
    std::string str = GetStringFromData(STR_LEN);

    // test
    std::shared_ptr<RSRenderCurveAnimation> animation =
        std::make_shared<RSRenderCurveAnimation>(animationId, propertyId, originValue, startValue, endValue);
    auto springAnimation =
        std::make_shared<RSRenderSpringAnimation>(animationId, propertyId, originValue, startValue, endValue);
    springAnimation->DumpAnimationInfo(str);
    springAnimation->SetSpringParameters(response, dampingRatio, blendDuration, minimumAmplitudeRatio);
    springAnimation->SetZeroThreshold(zeroThreshold);
    springAnimation->SetInitialVelocity(velocity);
    springAnimation->InheritSpringAnimation(animation);
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

    RSRenderInterpolatingSpringAnimationFuzzerTest1();
    RSRenderInterpolatingSpringAnimationFuzzerTest2();
    RSSpringValueEstimatorFuzzerTest();
    RSSpringModelFuzzerTest();
    RSRenderSpringAnimationFuzzerTest();
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}
