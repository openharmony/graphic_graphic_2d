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

#include "customized/random_animation.h"

#include "customized/random_rs_path.h"
#include "customized/random_rs_render_particle.h"
#include "random/random_data.h"
#include "random/random_engine.h"

namespace OHOS {
namespace Rosen {
namespace {
void UpdateParamToRenderAnimation(const std::shared_ptr<RSRenderAnimation>& animation)
{
    if (animation == nullptr) {
        return;
    }
    int duration = RandomData::GetRandomInt();
    int delay = RandomData::GetRandomInt();
    int repeatCount = RandomData::GetRandomInt();
    bool autoReverse = RandomData::GetRandomBool();
    float speed = RandomData::GetRandomFloat();
    bool isForward = RandomData::GetRandomBool();
    int fillMode = RandomEngine::GetRandomIndex(static_cast<int>(FillMode::BOTH));
    animation->SetDuration(duration);
    animation->SetStartDelay(delay);
    animation->SetRepeatCount(repeatCount);
    animation->SetAutoReverse(autoReverse);
    animation->SetSpeed(speed);
    animation->SetDirection(isForward);
    animation->SetFillMode(static_cast<FillMode>(fillMode));
}

std::shared_ptr<RSRenderPropertyBase> GetAnimatableProperty()
{
    int randomIndex = RandomEngine::GetRandomIndex(static_cast<int>(RSPropertyType::RRECT));
    RSPropertyType propertyTypeRandom = static_cast<RSPropertyType>(randomIndex);
    switch (propertyTypeRandom) {
        case RSPropertyType::FLOAT: {
            float value = RandomData::GetRandomFloat();
            return std::make_shared<RSRenderAnimatableProperty<float>>(value);
        }
        case RSPropertyType::RS_COLOR: {
            Color value = RandomData::GetRandomColor();
            return std::make_shared<RSRenderAnimatableProperty<Color>>(value);
        }
        case RSPropertyType::MATRIX3F: {
            Matrix3f value = RandomData::GetRandomMatrix3f();
            return std::make_shared<RSRenderAnimatableProperty<Matrix3f>>(value);
        }
        case RSPropertyType::QUATERNION: {
            Quaternion value = RandomData::GetRandomQuaternion();
            return std::make_shared<RSRenderAnimatableProperty<Quaternion>>(value);
        }
        case RSPropertyType::VECTOR2F: {
            Vector2f value = RandomData::GetRandomVector2f();
            return std::make_shared<RSRenderAnimatableProperty<Vector2f>>(value);
        }
        case RSPropertyType::VECTOR3F: {
            Vector3f value = RandomData::GetRandomVector3f();
            return std::make_shared<RSRenderAnimatableProperty<Vector3f>>(value);
        }
        case RSPropertyType::VECTOR4F: {
            Vector4f value = RandomData::GetRandomVector4f();
            return std::make_shared<RSRenderAnimatableProperty<Vector4f>>(value);
        }
        case RSPropertyType::VECTOR4_COLOR: {
            Vector4<Color> value = RandomData::GetRandomColorVector4();
            return std::make_shared<RSRenderAnimatableProperty<Vector4<Color>>>(value);
        }
        case RSPropertyType::RRECT: {
            RRect value = RandomData::GetRandomRRect();
            return std::make_shared<RSRenderAnimatableProperty<RRect>>(value);
        }
        default: {
            float value = RandomData::GetRandomFloat();
            return std::make_shared<RSRenderAnimatableProperty<float>>(value);
        }
    }
}

std::shared_ptr<RSInterpolator> GetRSInterpolator()
{
    static constexpr int RS_INTERPOLATOR_SUBCLASS_INDEX_MAX = 4; // Number of subclasses of RSInterpolator
    int randomIndex = RandomEngine::GetRandomIndex(RS_INTERPOLATOR_SUBCLASS_INDEX_MAX);
    switch (randomIndex) {
        case 0: {
            float x1 = RandomData::GetRandomFloat();
            float x2 = RandomData::GetRandomFloat();
            float y1 = RandomData::GetRandomFloat();
            float y2 = RandomData::GetRandomFloat();
            return std::make_shared<RSCubicBezierInterpolator>(x1, y1, x2, y2);
        }
        case 1: {
            return std::make_shared<LinearInterpolator>();
        }
        case 2: {
            int duration = RandomData::GetRandomInt();
            auto lambda = [](float value) -> float { return value; };
            return std::make_shared<RSCustomInterpolator>(lambda, duration);
        }
        case 3: {
            float response = RandomData::GetRandomFloat();
            float dampingRatio = RandomData::GetRandomFloat();
            float initialVelocity = RandomData::GetRandomFloat();
            return std::make_shared<RSSpringInterpolator>(response, dampingRatio, initialVelocity);
        }
        case 4: {
            int32_t steps = RandomData::GetRandomInt32();
            int position = RandomEngine::GetRandomIndex(static_cast<int>(StepsCurvePosition::END));
            return std::make_shared<RSStepsInterpolator>(steps, static_cast<StepsCurvePosition>(position));
        }
        default: {
            return std::make_shared<LinearInterpolator>();
        }
    }
}
} // namespace

std::shared_ptr<RSSyncTask> RandomAnimation::GetRandomRSSyncTask()
{
    static constexpr int RS_SYNC_TASK_SUBCLASS_INDEX_MAX = 2; // Number of subclasses of RSSyncTask
    int randomIndex = RandomEngine::GetRandomIndex(RS_SYNC_TASK_SUBCLASS_INDEX_MAX);

    NodeId nodeId = RandomData::GetRandomUint64();
    AnimationId animationId = RandomData::GetRandomUint64();
    uint64_t timeoutNS = RandomData::GetRandomUint64();

    auto property = GetAnimatableProperty();
    switch (randomIndex) {
        case 0: {
            return std::make_shared<RSNodeGetShowingPropertyAndCancelAnimation>(nodeId, property, timeoutNS);
        }
        case 1: {
            RSNodeGetShowingPropertiesAndCancelAnimation::PropertiesMap propertiesMap;
            int mapSize = RandomEngine::GetRandomIndex(10);
            for (int i = 0; i < mapSize; i++) {
                NodeId subNodeId = RandomData::GetRandomUint64();
                PropertyId subPropertyId = RandomData::GetRandomUint64();
                auto key = std::make_pair(subNodeId, subPropertyId);
                auto subProperty = GetAnimatableProperty();
                int vectorSize = RandomEngine::GetRandomIndex(10);
                std::vector<AnimationId> animationIds;
                for (int j = 0; j < vectorSize; j++) {
                    AnimationId subAnimationId = RandomData::GetRandomUint64();
                    animationIds.emplace_back(subAnimationId);
                }
                auto value = std::make_pair(subProperty, animationIds);
                propertiesMap.insert({key, value});
            }
            return std::make_shared<RSNodeGetShowingPropertiesAndCancelAnimation>(timeoutNS, std::move(propertiesMap));
        }
        case 2: {
            return std::make_shared<RSNodeGetAnimationsValueFraction>(timeoutNS, nodeId, animationId);
        }
        default: {
            return std::make_shared<RSNodeGetShowingPropertyAndCancelAnimation>(nodeId, property, timeoutNS);
        }
    }
}

std::shared_ptr<RSRenderCurveAnimation> RandomAnimation::GetRandomRSRenderCurveAnimation()
{
    PropertyId propertyId = RandomData::GetRandomUint64();
    AnimationId animationId = RandomData::GetRandomUint64();
    auto originValue = GetAnimatableProperty();
    auto startValue = GetAnimatableProperty();
    auto endValue = GetAnimatableProperty();
    bool additive = RandomData::GetRandomBool();
    auto animation =
        std::make_shared<RSRenderCurveAnimation>(animationId, propertyId, originValue, startValue, endValue);
    animation->SetInterpolator(GetRSInterpolator());
    animation->SetAdditive(additive);
    UpdateParamToRenderAnimation(animation);
    return animation;
}

std::shared_ptr<RSRenderParticleAnimation> RandomAnimation::GetRandomRSRenderParticleAnimation()
{
    PropertyId propertyId = RandomData::GetRandomUint64();
    AnimationId animationId = RandomData::GetRandomUint64();
    auto particlesRenderParams = RandomRSRenderParticle::GetRandomParticleRenderParamsVector();
    auto animation = std::make_shared<RSRenderParticleAnimation>(animationId, propertyId, particlesRenderParams);
    bool additive = RandomData::GetRandomBool();
    animation->SetAdditive(additive);
    UpdateParamToRenderAnimation(animation);
    return animation;
}

std::shared_ptr<RSRenderKeyframeAnimation> RandomAnimation::GetRandomRSRenderKeyframeAnimation()
{
    PropertyId propertyId = RandomData::GetRandomUint64();
    AnimationId animationId = RandomData::GetRandomUint64();
    auto originValue = GetAnimatableProperty();
    auto animation = std::make_shared<RSRenderKeyframeAnimation>(animationId, propertyId, originValue);

    bool isDurationKeyframe = RandomData::GetRandomBool();

    animation->SetDurationKeyframe(isDurationKeyframe);
    int vectorSize = RandomEngine::GetRandomIndex(10);
    for (int j = 0; j < vectorSize; j++) {
        if (isDurationKeyframe) {
            int startDuration = RandomData::GetRandomInt();
            int endDuration = RandomData::GetRandomInt();
            animation->AddKeyframe(startDuration, endDuration, GetAnimatableProperty(), GetRSInterpolator());
        } else {
            float fraction = RandomData::GetRandomFloat();
            animation->AddKeyframe(fraction, GetAnimatableProperty(), GetRSInterpolator());
        }
    }
    bool additive = RandomData::GetRandomBool();
    animation->SetAdditive(additive);
    UpdateParamToRenderAnimation(animation);
    return animation;
}

std::shared_ptr<RSRenderPathAnimation> RandomAnimation::GetRandomRSRenderPathAnimation()
{
    PropertyId propertyId = RandomData::GetRandomUint64();
    AnimationId animationId = RandomData::GetRandomUint64();
    auto property = GetAnimatableProperty();
    auto startValue = GetAnimatableProperty();
    auto endValue = GetAnimatableProperty();
    float originRotation = RandomData::GetRandomFloat();
    std::shared_ptr<RSPath> path = RandomRSPath::GetRandomRSPath();
    auto animation = std::make_shared<RSRenderPathAnimation>(
        animationId, propertyId, property, startValue, endValue, originRotation, path);

    bool needAddOrigin = RandomData::GetRandomBool();
    bool isNeedPath = RandomData::GetRandomBool();
    int randomIndex = RandomEngine::GetRandomIndex(static_cast<int>(RotationMode::ROTATE_AUTO_REVERSE));
    float beginFraction = RandomData::GetRandomFloat();
    float endFraction = RandomData::GetRandomFloat();
    bool additive = RandomData::GetRandomBool();

    animation->SetEndFraction(endFraction);
    animation->SetBeginFraction(beginFraction);
    animation->SetRotationMode(static_cast<RotationMode>(randomIndex));
    animation->SetPathNeedAddOrigin(needAddOrigin);
    animation->SetIsNeedPath(isNeedPath);

    animation->SetInterpolator(GetRSInterpolator());
    animation->SetAdditive(additive);
    UpdateParamToRenderAnimation(animation);
    return animation;
}

std::shared_ptr<RSRenderTransition> RandomAnimation::GetRandomRSRenderTransition()
{
    static constexpr int TRANSITION_EFFECT_INDEX_MAX = 3;  // Number of TransitionEffect
    int vectorSize = RandomEngine::GetRandomIndex(10);
    std::vector<std::shared_ptr<RSRenderTransitionEffect>> vec;
    for (int j = 0; j < vectorSize; j++) {
        int randomIndex = RandomEngine::GetRandomIndex(TRANSITION_EFFECT_INDEX_MAX);
        std::shared_ptr<RSRenderTransitionEffect> effect;
        switch (randomIndex) {
            case 0: {
                float alpha = RandomData::GetRandomFloat();
                effect = std::make_shared<RSTransitionFade>(alpha);
                break;
            }
            case 1: {
                float scaleX = RandomData::GetRandomFloat();
                float scaleY = RandomData::GetRandomFloat();
                float scaleZ = RandomData::GetRandomFloat();
                effect = std::make_shared<RSTransitionScale>(scaleX, scaleY, scaleZ);
                break;
            }
            case 2: {
                float translateX = RandomData::GetRandomFloat();
                float translateY = RandomData::GetRandomFloat();
                float translateZ = RandomData::GetRandomFloat();
                effect = std::make_shared<RSTransitionTranslate>(translateX, translateY, translateZ);
                break;
            }
            case 3: {
                float dx = RandomData::GetRandomFloat();
                float dy = RandomData::GetRandomFloat();
                float dz = RandomData::GetRandomFloat();
                float radian = RandomData::GetRandomFloat();
                effect = std::make_shared<RSTransitionRotate>(dx, dy, dz, radian);
                break;
            }
            default: {
                float alpha = RandomData::GetRandomFloat();
                effect = std::make_shared<RSTransitionFade>(alpha);
                break;
            }
        }
        vec.emplace_back(effect);
    }

    AnimationId animationId = RandomData::GetRandomUint64();
    bool isTransitionIn = RandomData::GetRandomBool();
    auto animation = std::make_shared<RSRenderTransition>(animationId, vec, isTransitionIn);
    animation->SetInterpolator(GetRSInterpolator());
    UpdateParamToRenderAnimation(animation);
    return animation;
}

std::shared_ptr<RSRenderSpringAnimation> RandomAnimation::GetRandomRSRenderSpringAnimation()
{
    PropertyId propertyId = RandomData::GetRandomUint64();
    AnimationId animationId = RandomData::GetRandomUint64();
    auto originValue = GetAnimatableProperty();
    auto startValue = GetAnimatableProperty();
    auto endValue = GetAnimatableProperty();
    auto initialVelocity = GetAnimatableProperty();
    auto animation =
        std::make_shared<RSRenderSpringAnimation>(animationId, propertyId, originValue, startValue, endValue);
    float response = RandomData::GetRandomFloat();
    float dampingRatio = RandomData::GetRandomFloat();
    float blendDuration = RandomData::GetRandomFloat();
    float zeroThreshold = RandomData::GetRandomFloat();
    bool additive = RandomData::GetRandomBool();
    animation->SetSpringParameters(response, dampingRatio, blendDuration);
    animation->SetZeroThreshold(zeroThreshold);
    animation->SetInitialVelocity(initialVelocity);
    animation->SetAdditive(additive);
    UpdateParamToRenderAnimation(animation);
    return animation;
}

std::shared_ptr<RSRenderInterpolatingSpringAnimation> RandomAnimation::GetRandomRSRenderInterpolatingSpringAnimation()
{
    PropertyId propertyId = RandomData::GetRandomUint64();
    AnimationId animationId = RandomData::GetRandomUint64();
    auto originValue = GetAnimatableProperty();
    auto startValue = GetAnimatableProperty();
    auto endValue = GetAnimatableProperty();
    auto animation = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        animationId, propertyId, originValue, startValue, endValue);

    float response = RandomData::GetRandomFloat();
    float dampingRatio = RandomData::GetRandomFloat();
    float normalizedInitialVelocity = RandomData::GetRandomFloat();
    float minimumAmplitudeRatio = RandomData::GetRandomFloat();
    float zeroThreshold = RandomData::GetRandomFloat();
    bool additive = RandomData::GetRandomBool();
    animation->SetSpringParameters(response, dampingRatio, normalizedInitialVelocity, minimumAmplitudeRatio);
    animation->SetZeroThreshold(zeroThreshold);
    animation->SetAdditive(additive);
    UpdateParamToRenderAnimation(animation);
    return animation;
}
} // namespace Rosen
} // namespace OHOS
