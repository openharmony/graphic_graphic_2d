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

#include "rsanimationinterpolator_fuzzer.h"

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
    interpolator->InterpolateImpl(input);
    interpolator->GetType();
    interpolator->Marshalling(parcel);
    auto copyInterpolator =
        std::shared_ptr<RSCubicBezierInterpolator>(RSCubicBezierInterpolator::Unmarshalling(parcel));
}

void RSInterpolatorFuzzerTest1()
{
    // get data
    Parcel parcel;
    int duration = GetData<int>();

    // test
    auto linearInterpolator = std::make_shared<LinearInterpolator>();
    linearInterpolator->Marshalling(parcel);
    auto copyLinearInterpolator = std::shared_ptr<LinearInterpolator>(LinearInterpolator::Unmarshalling(parcel));
    linearInterpolator->GetType();

    auto lambda = [](float value) -> float { return value; };
    auto customInterpolator = std::make_shared<RSCustomInterpolator>(lambda, duration);
    customInterpolator->Marshalling(parcel);
    auto copyCustomInterpolator = std::shared_ptr<RSCustomInterpolator>(RSCustomInterpolator::Unmarshalling(parcel));
    customInterpolator->GetType();
}

void RSInterpolatorFuzzerTest2()
{
    Parcel parcel1;
    uint16_t type = GetData<uint16_t>();
    auto interpolator1 = RSInterpolator::Unmarshalling(parcel1);
    parcel1.WriteUint16(type);
    interpolator1 = RSInterpolator::Unmarshalling(parcel1);

    Parcel parcel2;
    auto interpolator2 = LinearInterpolator::Unmarshalling(parcel2);
    if (interpolator2) {
        interpolator2->GetType();
    }

    uint16_t id = GetData<uint16_t>();
    float time = GetData<float>();
    std::vector<float> times = { time };
    float value = GetData<float>();
    std::vector<float> values = { value };
    float input = GetData<float>();
    auto customInterpolator2 = std::make_shared<RSCustomInterpolator>(id, std::move(times), std::move(values));
    customInterpolator2->InterpolateImpl(input);
    customInterpolator2->InterpolateImpl(time);
    Parcel parcel3;
    customInterpolator2->Marshalling(parcel3);
    auto copyCustomInterpolator2 = RSCustomInterpolator::Unmarshalling(parcel3);
    if (copyCustomInterpolator2) {
        copyCustomInterpolator2->GetType();
    }
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
    animation->InterpolateImpl(fraction);
    animation->GetType();
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

    RSCubicBezierInterpolatorFuzzerTest();
    RSInterpolatorFuzzerTest1();
    RSInterpolatorFuzzerTest2();
    RSSpringInterpolatorFuzzerTest();
    RSStepsInterpolatorFuzzerTest();
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
