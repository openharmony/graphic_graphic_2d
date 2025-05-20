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

#include "rsanimationcurve_fuzzer.h"


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

void RSRenderCurveAnimationFuzzerTest1()
{
    // get data
    auto animationId = GetData<AnimationId>();
    auto propertyId = GetData<PropertyId>();
    int32_t steps = GetData<int32_t>();
    StepsCurvePosition position = GetData<StepsCurvePosition>();
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
    auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
    auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
    std::string out;

    // test
    auto animation = std::make_shared<RSRenderCurveAnimation>(animationId, propertyId, property, startValue, endValue);
    animation->DumpAnimationInfo(out);
    auto interpolator = std::make_shared<RSStepsInterpolator>(steps, position);
    animation->SetInterpolator(interpolator);
    animation->GetInterpolator();
    Parcel parcel;
    animation->Marshalling(parcel);
    auto copyAnimation = std::shared_ptr<RSRenderCurveAnimation>(RSRenderCurveAnimation::Unmarshalling(parcel));
}

void RSRenderCurveAnimationFuzzerTest2()
{
    // get data
    auto animationId = GetData<AnimationId>();
    auto propertyId = GetData<PropertyId>();
    int32_t steps = GetData<int32_t>();
    StepsCurvePosition position = GetData<StepsCurvePosition>();
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
    auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
    auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
    auto fraction = GetData<float>();

    // test
    auto animation = std::make_shared<RSRenderCurveAnimation>(animationId, propertyId, property, startValue, endValue);
    auto interpolator = std::make_shared<RSStepsInterpolator>(steps, position);
    animation->property_ = property;
    animation->SetInterpolator(interpolator);
    animation->OnSetFraction(fraction);
    animation->OnAnimateInner(fraction, interpolator);
    animation->InitValueEstimator();
    animation->OnSetFraction(fraction);
    animation->UpdateFractionAfterContinue();
    animation->OnAnimate(fraction);
    animation->OnAnimateInner(fraction, interpolator);
}

void RSCurveValueEstimatorFuzzerTest()
{
    // get data
    bool isAdditive = GetData<bool>();
    float fraction = GetData<float>();
    int32_t steps = GetData<int32_t>();
    int duration = GetData<int>();
    StepsCurvePosition position = GetData<StepsCurvePosition>();
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
    auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
    auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
    auto lastValue = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());

    // test
    auto curveValueEstimator = std::make_shared<RSCurveValueEstimator<float>>();
    curveValueEstimator->InitCurveAnimationValue(property, startValue, endValue, lastValue);
    curveValueEstimator->UpdateAnimationValue(fraction, isAdditive);
    curveValueEstimator->GetAnimationValue(fraction, isAdditive);
    auto interpolator = std::make_shared<RSStepsInterpolator>(steps, position);
    curveValueEstimator->EstimateFraction(interpolator);
    curveValueEstimator->EstimateFraction(interpolator, fraction, duration);
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

    RSRenderCurveAnimationFuzzerTest1();
    RSRenderCurveAnimationFuzzerTest2();
    RSCurveValueEstimatorFuzzerTest();

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
