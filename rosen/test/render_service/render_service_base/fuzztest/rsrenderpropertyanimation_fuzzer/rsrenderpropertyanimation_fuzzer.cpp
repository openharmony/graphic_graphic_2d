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

#include "rsrenderpropertyanimation_fuzzer.h"


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

void RSRenderPropertyAnimationFuzzerTest1()
{
    // get data
    AnimationId animationId = GetData<AnimationId>();
    PropertyId propertyId = GetData<PropertyId>();
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
    bool isAdditive = GetData<bool>();
    auto frameInterval = GetData<float>();
    auto fraction = GetData<float>();
    auto time = GetData<float>();
    std::string out;

    auto renderPropertyAnimation = std::make_shared<RSRenderPropertyAnimation>(animationId, propertyId, property);
    renderPropertyAnimation->DumpAnimationInfo(out);
    renderPropertyAnimation->SetAdditive(isAdditive);
    renderPropertyAnimation->GetAdditive();
    renderPropertyAnimation->AttachRenderProperty(property);
    Parcel parcel;
    renderPropertyAnimation->Marshalling(parcel);
    renderPropertyAnimation->ParseParam(parcel);
    renderPropertyAnimation->SetPropertyValue(property);
    renderPropertyAnimation->GetPropertyValue();
    renderPropertyAnimation->GetOriginValue();
    renderPropertyAnimation->GetLastValue();
    renderPropertyAnimation->SetAnimationValue(property);
    renderPropertyAnimation->GetAnimationValue(property);
    renderPropertyAnimation->OnRemoveOnCompletion();
    renderPropertyAnimation->RecordLastAnimateValue();
    renderPropertyAnimation->UpdateAnimateVelocity(frameInterval);
    renderPropertyAnimation->ProcessAnimateVelocityUnderAngleRotation(frameInterval);
    renderPropertyAnimation->DumpFraction(fraction, time);
}

void RSRenderPropertyAnimationFuzzerTest2()
{
    // get data
    AnimationId animationId = GetData<AnimationId>();
    PropertyId propertyId = GetData<PropertyId>();
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
    bool isAdditive = GetData<bool>();
    auto frameInterval = GetData<float>();
    auto fraction = GetData<float>();
    auto time = GetData<float>();
    NodeId nodeId = GetData<NodeId>();
    auto renderNode = std::make_shared<RSRenderNode>(nodeId);

    auto renderPropertyAnimation = std::make_shared<RSRenderPropertyAnimation>(animationId, propertyId, property);

    renderPropertyAnimation->Attach(renderNode.get());
    renderPropertyAnimation->Start();

    renderPropertyAnimation->SetAdditive(isAdditive);
    renderPropertyAnimation->GetAdditive();
    renderPropertyAnimation->AttachRenderProperty(property);

    renderPropertyAnimation->SetAnimationValue(nullptr);
    renderPropertyAnimation->SetAnimationValue(property);
    renderPropertyAnimation->GetAnimationValue(nullptr);
    renderPropertyAnimation->GetAnimationValue(property);
    renderPropertyAnimation->OnRemoveOnCompletion();
    renderPropertyAnimation->RecordLastAnimateValue();
    renderPropertyAnimation->ProcessAnimateVelocityUnderAngleRotation(frameInterval);
    renderPropertyAnimation->DumpFraction(fraction, time);
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

    RSRenderPropertyAnimationFuzzerTest1();
    RSRenderPropertyAnimationFuzzerTest2();
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
