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

#include "rsrenderanimation_fuzzer.h"


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

void RSRenderAnimation1FuzzerTest()
{
    // get data
    AnimationId animationId = GetData<AnimationId>();
    NodeId nodeId = GetData<NodeId>();
    bool bForceDetach = GetData<bool>();
    float fraction = GetData<float>();
    auto fillMode = GetData<FillMode>();
    auto time = GetData<int64_t>();
    auto animation = std::make_shared<RSRenderAnimation>(animationId);
    auto renderNode = std::make_shared<RSRenderNode>(nodeId);
    std::string nodeName = GetStringFromData(STR_LEN);
    renderNode->SetNodeName(nodeName);
    std::string outString;
    Parcel parcel;
    // test
    animation->Attach(renderNode.get());
    animation->DumpAnimation(outString);
    animation->DumpAnimationInfo(outString);
    animation->ParseParam(parcel);
    animation->IsStarted();
    animation->IsRunning();
    animation->IsPaused();
    animation->IsFinished();
    animation->GetPropertyId();
    animation->GetTargetName();
    animation->Detach(bForceDetach);
    animation->Attach(renderNode.get());
    animation->GetTarget();
    animation->SetFractionInner(fraction);
    animation->SetFillMode(fillMode);
    animation->ProcessFillModeOnStart(fraction);
    animation->ProcessFillModeOnFinish(fraction);
    animation->ProcessOnRepeatFinish();
    animation->SetStartTime(time);
    animation->GetAnimateVelocity();
}

void RSRenderAnimation2FuzzerTest()
{
    // get data
    AnimationId animationId = GetData<AnimationId>();
    auto pos = GetData<RSInteractiveAnimationPosition>();
    float fraction = GetData<float>();
    bool isReversed = GetData<bool>();
    auto time = GetData<int64_t>();
    auto delayTime = GetData<int64_t>();
    auto animation1 = std::make_shared<RSRenderAnimation>(animationId);

    // status error
    animation1->Finish();
    animation1->FinishOnPosition(pos);
    animation1->FinishOnCurrentPosition();
    animation1->Pause();
    animation1->Resume();
    animation1->SetFraction(fraction);
    animation1->SetReversedAndContinue();
    animation1->SetReversed(isReversed);
    animation1->Animate(time, delayTime);

    // status normal
    animation1->Start();
    animation1->Pause();
    animation1->SetFraction(fraction);
    animation1->SetReversed(isReversed);
    animation1->SetReversedAndContinue();
    animation1->Animate(time, delayTime);
    animation1->SetStartTime(time);
    animation1->Animate(time, delayTime);
    animation1->animationFraction_.SetLastFrameTime(time);
    animation1->Animate(time, delayTime);
    animation1->FinishOnPosition(pos);
    animation1->Start();

    auto animation2 = std::make_shared<RSRenderAnimation>(animationId);
    animation2->Start();
    animation2->Finish();

    auto animation3 = std::make_shared<RSRenderAnimation>(animationId);
    animation3->Start();
    animation3->FinishOnCurrentPosition();
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

    RSRenderAnimation1FuzzerTest();
    RSRenderAnimation2FuzzerTest();
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
