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

#include "rsanimationbase_fuzzer.h"

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

void RSAnimationManagerFuzzerTest()
{
    // get data
    AnimationId keyId = GetData<AnimationId>();
    AnimationId id = GetData<AnimationId>();
    pid_t pid = GetData<pid_t>();
    int64_t time = GetData<int64_t>();
    int64_t delayTime = GetData<int64_t>();
    bool nodeIsOnTheTree = GetData<bool>();
    RSSurfaceNodeAbilityState abilityState = GetData<RSSurfaceNodeAbilityState>();
    bool isEnable = GetData<bool>();
    const FrameRateGetFunc func;
    float width = GetData<float>();
    float height = GetData<float>();
    float scaleX = GetData<float>();
    float scaleY = GetData<float>();

    PropertyId propertyId = GetData<PropertyId>();
    AnimationId animId = GetData<AnimationId>();

    // test
    auto animationManager = std::make_shared<RSAnimationManager>();
    animationManager->RemoveAnimation(keyId);
    animationManager->CancelAnimationByPropertyId(propertyId);
    animationManager->GetAnimation(id);
    animationManager->FilterAnimationByPid(pid);
    animationManager->GetAnimationsSize();
    animationManager->GetAnimationPid();
    animationManager->Animate(time, delayTime, nodeIsOnTheTree, abilityState);
    animationManager->RegisterSpringAnimation(propertyId, animId);
    animationManager->UnregisterSpringAnimation(propertyId, animId);
    animationManager->QuerySpringAnimation(propertyId);
    animationManager->RegisterPathAnimation(propertyId, animId);
    animationManager->UnregisterPathAnimation(propertyId, animId);
    animationManager->QueryPathAnimation(propertyId);
    animationManager->RegisterParticleAnimation(propertyId, animId);
    animationManager->UnregisterParticleAnimation(propertyId, animId);
    animationManager->GetParticleAnimations();
    animationManager->GetParticleAnimation();
    animationManager->GetFrameRateRange();
    animationManager->GetDecideFrameRateRange();
    animationManager->SetRateDeciderEnable(isEnable, func);
    animationManager->SetRateDeciderSize(width, height);
    animationManager->SetRateDeciderScale(scaleX, scaleY);
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
    FrameRateRange range;
    FinishCallbackType finishCallbackType = FinishCallbackType::TIME_SENSITIVE;

    // test
    auto animationTimeProtocol = std::make_shared<RSAnimationTimingProtocol>();
    animationTimeProtocol->SetDuration(duration);
    animationTimeProtocol->SetStartDelay(startDelay);
    animationTimeProtocol->SetSpeed(speed);
    animationTimeProtocol->SetRepeatCount(repeatCount);
    animationTimeProtocol->SetAutoReverse(autoReverse);
    animationTimeProtocol->SetFillMode(fillMode);
    animationTimeProtocol->SetDirection(isForward);
    animationTimeProtocol->SetFrameRateRange(range);
    animationTimeProtocol->SetFinishCallbackType(finishCallbackType);
    animationTimeProtocol->GetDuration();
    animationTimeProtocol->GetStartDelay();
    animationTimeProtocol->GetSpeed();
    animationTimeProtocol->GetRepeatCount();
    animationTimeProtocol->GetAutoReverse();
    animationTimeProtocol->GetFillMode();
    animationTimeProtocol->GetDirection();
    animationTimeProtocol->GetFrameRateRange();
    animationTimeProtocol->GetFinishCallbackType();
}

void RSAnimationTraceUtilsFuzzerTest()
{
    // get data
    const std::string nodeName = GetStringFromData(STR_LEN);
    const std::string info = GetStringFromData(STR_LEN);
    const uint64_t nodeId = GetData<uint64_t>();
    const uint64_t propertyId = GetData<uint64_t>();
    const uint64_t animationId = GetData<uint64_t>();
    const int propertyType = GetData<int>();
    const int animationType = GetData<int>();
    const int animationDelay = GetData<int>();
    const int animationDur = GetData<int>();
    const int repeat = GetData<int>();
    bool isAddLogInfo = GetData<bool>();
    auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
    auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
    auto value = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
    auto initialVelocity = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
    const float fraction = GetData<float>();
    const int64_t time = GetData<int64_t>();

    // test
    RSAnimationTraceUtils::GetInstance().addAnimationNameTrace(nodeName);
    RSAnimationTraceUtils::GetInstance().addAnimationFinishTrace(info, nodeId, animationId, isAddLogInfo);
    RSAnimationTraceUtils::GetInstance().addAnimationCreateTrace(
        nodeId, nodeName, propertyId, animationId, animationType, propertyType, startValue, endValue,
        animationDelay, animationDur, repeat);
    RSAnimationTraceUtils::GetInstance().addAnimationFrameTrace(
        nodeId, nodeName, animationId, propertyId, fraction, value, time, animationDur, repeat);
    RSAnimationTraceUtils::GetInstance().addSpringInitialVelocityTrace(
        propertyId, animationId, initialVelocity, value);
    RSAnimationTraceUtils::GetInstance().ParseRenderPropertyVaule(value);
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

    RSAnimationManagerFuzzerTest();
    RSAnimationTimingProtocolFuzzerTest();
    RSAnimationTraceUtilsFuzzerTest();
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
