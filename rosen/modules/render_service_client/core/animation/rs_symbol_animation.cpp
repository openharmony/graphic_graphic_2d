/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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


#include "animation/rs_symbol_animation.h"
#include "animation/rs_keyframe_animation.h"
#include "platform/common/rs_log.h"


namespace OHOS {
namespace Rosen {

RSSymbolAnimation::RSSymbolAnimation()
{}

bool RSSymbolAnimation::SetSymbolAnimation(
    const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig)
{
    if (!rsNode_) {
        ROSEN_LOGE("HmSymbol RSSymbolAnimation::getNode :failed");
        return false;
    }

    if (symbolAnimationConfig->effectStrategy == TextEngine::SymbolAnimationEffectStrategy::SYMBOL_SCALE) {
        return SetScaleUnitAnimation(rsNode_);
    } else if (symbolAnimationConfig->effectStrategy ==
        TextEngine::SymbolAnimationEffectStrategy::SYMBOL_HIERARCHICAL) {
        return SetNoneAnimation(rsNode_);
    }
    return false;
}

template<typename T>
bool RSSymbolAnimation::CreateOrSetModifierValue(std::shared_ptr<RSAnimatableProperty<T>>& property, const T& value)
{
    if (property == nullptr) {
        property = std::make_shared<RSAnimatableProperty<T>>(value);
        return true;
    }
    property->Set(value);
    return false;
}


bool RSSymbolAnimation::isEqual(const Vector2f val1, const Vector2f val2)
{
    return(val1.x_ == val2.x_ && val1.y_ == val2.y_);
}

bool RSSymbolAnimation::SetNoneAnimation(const std::shared_ptr<RSNode>& rsNode)
{
    if (!rsNode) {
        ROSEN_LOGE("[%{public}s] No symbol rsNode to be animated \n", __func__);
        return false;
    }
    const Vector2f pivotNone1Value = {0.25f, 0.25f};
    const Vector2f pivotNone2Value = {0.5f, 0.5f};
    auto animation = NoneSymbolAnimation(rsNode, pivotNone1Value, pivotNone2Value);
    if (!animation) {
        return false;
    }
    animation->Start(rsNode);
    return true;
}

std::shared_ptr<RSAnimation> RSSymbolAnimation::NoneSymbolAnimation(const std::shared_ptr<RSNode>& rsNode,
    const Vector2f& pivotNone1Value, const Vector2f& pivotNone2Value)
{
    bool isCreate = CreateOrSetModifierValue(pivotNone1Property_, pivotNone1Value);
    if (isCreate) {
        auto pivotNoneModifier = std::make_shared<RSPivotModifier>(pivotNone1Property_);
        rsNode->AddModifier(pivotNoneModifier);
    }
    CreateOrSetModifierValue(pivotNone2Property_, pivotNone2Value);

    auto keyframeAnimation = std::make_shared<RSKeyframeAnimation>(pivotNone1Property_);
    keyframeAnimation->SetDuration(1300); // duration is 1300ms
    keyframeAnimation->AddKeyFrame(1.f, pivotNone2Property_, RSAnimationTimingCurve::LINEAR);
    return keyframeAnimation;
}

bool RSSymbolAnimation::SetScaleUnitAnimation(const std::shared_ptr<RSNode>& rsNode)
{
    if (!rsNode) {
        ROSEN_LOGE("[%{public}s] No symbol rsNode to be animated \n", __func__);
        return false;
    }
    const Vector2f scaleValueBegin = {1.0f, 1.0f};
    const Vector2f scaleValue = {1.15f, 1.15f};
    const Vector2f scaleValueEnd = scaleValueBegin;
    auto animation = ScaleSymbolAnimation(rsNode, scaleValueBegin, scaleValue, scaleValueEnd);
    if (!animation) {
        return false;
    }
    animation->Start(rsNode);
    return true;
}

std::shared_ptr<RSAnimation> RSSymbolAnimation::ScaleSymbolAnimation(
    const std::shared_ptr<RSNode>& rsNode, const Vector2f& scaleValueBegin,
    const Vector2f& scaleValue, const Vector2f& scaleValueEnd, int delay)
{
    bool isCreate = CreateOrSetModifierValue(scaleStartProperty_, scaleValueBegin);
    if (isCreate) {
        auto scaleModifier = std::make_shared<RSScaleModifier>(scaleStartProperty_);
        rsNode->AddModifier(scaleModifier);
    }
    CreateOrSetModifierValue(scaleProperty_, scaleValue);
    CreateOrSetModifierValue(scaleEndProperty_, scaleValueEnd);
    Vector2f curNodePivot = rsNode->GetStagingProperties().GetPivot();
    if (!isEqual(curNodePivot, CENTER_NODE_COORDINATE)) {
        bool isCreate = CreateOrSetModifierValue(pivotProperty_, CENTER_NODE_COORDINATE);
        if (isCreate) {
            auto pivotModifier = std::make_shared<RSPivotModifier>(pivotProperty_);
            rsNode->AddModifier(pivotModifier);
        }
    }

    RSAnimationTimingCurve scaleCurve = SetScaleSpringTimingCurve();

    auto keyframeAnimation = std::make_shared<RSKeyframeAnimation>(scaleStartProperty_);
    keyframeAnimation->SetDuration(2000); // duration is 2000ms
    keyframeAnimation->AddKeyFrame(0.25f, scaleProperty_, scaleCurve);
    keyframeAnimation->AddKeyFrame(0.75f, scaleProperty_, scaleCurve);
    keyframeAnimation->AddKeyFrame(1.f, scaleEndProperty_, scaleCurve);
    return keyframeAnimation;
}

RSAnimationTimingCurve RSSymbolAnimation::SetScaleSpringTimingCurve()
{
    float velocity = 0;
    float mass = 1;
    float stiffness = 228;
    float damping = 22;
    RSAnimationTimingCurve scaleCurve = RSAnimationTimingCurve::CreateSpringCurve(
        velocity, mass, stiffness, damping);
    return scaleCurve;
}
} // namespace Rosen
} // namespace OHOS