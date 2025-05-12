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

#include "rsanimationtransition_fuzzer.h"


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

void RSRenderTransitionFuzzerTest()
{
    // get data
    AnimationId animationId = GetData<AnimationId>();

    bool isTransitionIn = GetData<bool>();
    std::string str = GetStringFromData(STR_LEN);
    int32_t steps = GetData<int32_t>();
    StepsCurvePosition position = GetData<StepsCurvePosition>();
    float alpha = GetData<float>();

    // test
    auto interpolator = std::make_shared<RSStepsInterpolator>(steps, position);
    std::shared_ptr<RSRenderTransitionEffect> child = std::make_shared<RSTransitionFade>(alpha);
    std::vector<std::shared_ptr<RSRenderTransitionEffect>> vec = { child };
    auto renderTransition = std::make_shared<RSRenderTransition>(animationId, vec, isTransitionIn);
    renderTransition->DumpAnimationInfo(str);
    renderTransition->SetInterpolator(interpolator);
    Parcel parcel;
    renderTransition->Marshalling(parcel);
    auto copyRenderTransition = std::shared_ptr<RSRenderTransition>(RSRenderTransition::Unmarshalling(parcel));
}

void RSRenderTransitionEffectFuzzerTest()
{
    // get data
    float scaleX = GetData<float>();
    float scaleY = GetData<float>();
    float scaleZ = GetData<float>();
    float dx = GetData<float>();
    float dy = GetData<float>();
    float dz = GetData<float>();
    float radian = GetData<float>();
    float translateX = GetData<float>();
    float translateY = GetData<float>();
    float translateZ = GetData<float>();
    float alpha = GetData<float>();
    float fraction = GetData<float>();
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
    auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
    auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
    Parcel parcel;

    // test
    auto transitionFade = std::make_shared<RSTransitionFade>(alpha);
    transitionFade->UpdateFraction(fraction);
    transitionFade->Marshalling(parcel);
    auto copyTransitionFade =
        std::shared_ptr<RSRenderTransitionEffect>(RSRenderTransitionEffect::Unmarshalling(parcel));

    auto transitionScale = std::make_shared<RSTransitionScale>(scaleX, scaleY, scaleZ);
    transitionScale->UpdateFraction(fraction);
    transitionScale->Marshalling(parcel);
    auto copyTransitionScale = std::shared_ptr<RSRenderTransitionEffect>(RSTransitionScale::Unmarshalling(parcel));

    auto transitionTranslate = std::make_shared<RSTransitionTranslate>(translateX, translateY, translateZ);
    transitionTranslate->UpdateFraction(fraction);
    transitionTranslate->Marshalling(parcel);
    auto copyTransitionTranslate =
        std::shared_ptr<RSRenderTransitionEffect>(RSTransitionTranslate::Unmarshalling(parcel));

    auto transitionRotate = std::make_shared<RSTransitionRotate>(dx, dy, dz, radian);
    transitionRotate->UpdateFraction(fraction);
    transitionRotate->Marshalling(parcel);
    auto copyTransitionRotate =
        std::shared_ptr<RSRenderTransitionEffect>(RSTransitionRotate::Unmarshalling(parcel));

    auto transitionCustom = std::make_shared<RSTransitionCustom>(property, startValue, endValue);
    transitionCustom->UpdateFraction(fraction);
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

    RSRenderTransitionFuzzerTest();
    RSRenderTransitionEffectFuzzerTest();
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
