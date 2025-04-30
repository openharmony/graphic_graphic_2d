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

#include "rsanimationkeyframe_fuzzer.h"

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
#include "pipeline/rs_context.h"
#include "pipeline/rs_render_node.h"
#include "render/rs_path.h"

namespace OHOS {
using namespace Rosen;
namespace {
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

void RSRenderKeyframeAnimationFuzzerTest1()
{
    // get data
    auto animationId = GetData<AnimationId>();
    auto propertyId = GetData<PropertyId>();
    int32_t steps = GetData<int32_t>();
    StepsCurvePosition position = GetData<StepsCurvePosition>();
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
    auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
    auto fraction = GetData<float>();
    auto startDuration = GetData<int>();
    auto endDuration = GetData<int>();
    auto isDuration = GetData<bool>();
    std::vector<std::tuple<float, std::shared_ptr<RSRenderPropertyBase>, std::shared_ptr<RSInterpolator>>> keyframes;
    std::string out;

    // test
    auto animation = std::make_shared<RSRenderKeyframeAnimation>(animationId, propertyId, property);
    animation->DumpAnimationInfo(out);
    auto interpolator = std::make_shared<RSStepsInterpolator>(steps, position);
    animation->AddKeyframe(fraction, startValue, interpolator);
    keyframes.push_back({ fraction, startValue, interpolator });
    animation->AddKeyframes(keyframes);
    animation->AddKeyframe(startDuration, endDuration, startValue, interpolator);
    animation->SetDurationKeyframe(isDuration);
    Parcel parcel;
    animation->Marshalling(parcel);
    auto copyAnimation = std::shared_ptr<RSRenderKeyframeAnimation>(RSRenderKeyframeAnimation::Unmarshalling(parcel));
}

void RSRenderKeyframeAnimationFuzzerTest2()
{
    // get data
    auto animationId = GetData<AnimationId>();
    auto propertyId = GetData<PropertyId>();
    int32_t steps = GetData<int32_t>();
    StepsCurvePosition position = GetData<StepsCurvePosition>();
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
    auto fraction = GetData<float>();

    // test
    auto animation = std::make_shared<RSRenderKeyframeAnimation>(animationId, propertyId, property);
    auto interpolator = std::make_shared<RSStepsInterpolator>(steps, position);
    animation->property_ = property;
    Parcel parcel;
    parcel.WriteFloat(fraction);
    parcel.WriteFloat(fraction);
    RSRenderPropertyBase::Marshalling(parcel, property);
    interpolator->Marshalling(parcel);
    animation->ParseDurationKeyframesParam(parcel, 1);
    animation->OnAnimate(fraction);
    animation->InitValueEstimator();
    animation->OnAnimate(fraction);
}

void RSKeyframeValueEstimatorFuzzerTest()
{
    // get data
    bool isAdditive = GetData<bool>();
    float fraction = GetData<float>();
    int32_t steps = GetData<int32_t>();
    StepsCurvePosition position = GetData<StepsCurvePosition>();
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
    auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
    auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
    auto lastValue = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
    std::vector<std::tuple<float, std::shared_ptr<RSRenderPropertyBase>, std::shared_ptr<RSInterpolator>>> keyframes;
    std::vector<std::tuple<float, float, std::shared_ptr<RSRenderPropertyBase>, std::shared_ptr<RSInterpolator>>>
        durationKeyframes;

    // test
    auto keyFrameValueEstimator = std::make_shared<RSKeyframeValueEstimator<float>>();
    auto interpolator = std::make_shared<RSStepsInterpolator>(steps, position);
    keyframes.push_back({ fraction, property, interpolator });
    durationKeyframes.push_back({ fraction, fraction, property, interpolator });
    keyFrameValueEstimator->InitKeyframeAnimationValue(property, keyframes, lastValue);
    keyFrameValueEstimator->InitDurationKeyframeAnimationValue(property, durationKeyframes, lastValue);
    keyFrameValueEstimator->UpdateAnimationValue(fraction, isAdditive);
    keyFrameValueEstimator->GetAnimationValue(fraction, isAdditive);
    keyFrameValueEstimator->GetDurationKeyframeAnimationValue(fraction, isAdditive);
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

    RSRenderKeyframeAnimationFuzzerTest1();
    RSRenderKeyframeAnimationFuzzerTest2();
    RSKeyframeValueEstimatorFuzzerTest();
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
