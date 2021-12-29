/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_IMPLICIT_ANIMATOR_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_IMPLICIT_ANIMATOR_H

#include <stack>

#include "animation/rs_animation.h"
#include "animation/rs_animation_timing_curve.h"
#include "animation/rs_implicit_animation_param.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
class RSImplicitAnimator {
public:
    virtual ~RSImplicitAnimator() = default;
    static RSImplicitAnimator& Instance();

    void OpenImplicitAnimation(const RSAnimationTimingProtocol& timingProtocol,
        const RSAnimationTimingCurve& timingCurve, const std::function<void()>& finishCallback);
    std::vector<std::shared_ptr<RSAnimation>> CloseImplicitAnimation();
    void BeginImplicitKeyFrameAnimation(float fraction, const RSAnimationTimingCurve& timingCurve);
    void BeginImplicitKeyFrameAnimation(float fraction);
    void BeginImplicitTransition(const RSTransitionEffect& effect);
    void EndImplicitTransition();
    void EndImplicitKeyFrameAnimation();
    bool NeedImplicitAnimaton();

    template<typename T>
    std::shared_ptr<RSAnimation> CreateImplicitAnimation(
        RSPropertyNode& target, const RSAnimatableProperty& property, const T& startValue, const T& endValue)
    {
        if (globalImplicitParams_.empty() || implicitAnimations_.empty() || keyframeAnimations_.empty()) {
            ROSEN_LOGE("Failed to create implicit animation, need to open implicit animation firstly!");
            return {};
        }

        ProcessPreCreateAnimation(target, property);

        std::shared_ptr<RSAnimation> animation;
        auto params = implicitAnimationParams_.top();
        switch (params->GetType()) {
            case ImplicitAnimationParamType::CURVE: {
                auto curveImplicitParam = static_cast<RSImplicitCurveAnimationParam*>(params.get());
                animation = curveImplicitParam->CreateAnimation(property, startValue, endValue);
                break;
            }
            case ImplicitAnimationParamType::KEYFRAME: {
                auto keyframeImplicitParam = static_cast<RSImplicitKeyframeAnimationParam*>(params.get());
                auto& keyframeAnimations = keyframeAnimations_.top();
                auto keyframeIter = keyframeAnimations.find({ target.GetId(), property });
                SetPropertyValue(target, property, endValue);
                if (keyframeIter == keyframeAnimations.end()) {
                    animation = keyframeImplicitParam->CreateAnimation(property, startValue, endValue);
                    keyframeAnimations[{ target.GetId(), property }] = animation;
                } else {
                    keyframeImplicitParam->AddKeyframe(keyframeIter->second, startValue, endValue);
                    return keyframeIter->second;
                }
                break;
            }
            case ImplicitAnimationParamType::PATH: {
                auto pathImplicitParam = static_cast<RSImplicitPathAnimationParam*>(params.get());
                animation = pathImplicitParam->CreateAnimation(property, startValue, endValue);
                break;
            }
            default:
                ROSEN_LOGE("Failed to create animation, unknow type!");
                break;
        }

        ProcessPostCreateAnimation(target, property);

        if (animation == nullptr) {
            ROSEN_LOGE("Failed to create animation!");
            return nullptr;
        }

        if (params->GetType() != ImplicitAnimationParamType::KEYFRAME) {
            target.AddAnimation(animation);
        }

        implicitAnimations_.top().push_back({ animation, target.GetId() });
        return animation;
    }

    std::shared_ptr<RSAnimation> CreateImplicitTransition(RSPropertyNode& target);

private:
    RSImplicitAnimator() = default;

    void BeginImplicitCurveAnimation();
    void EndImplicitCurveAnimation();
    void BeginImplicitPathAnimation(const std::shared_ptr<RSMotionPathOption>& motionPathOption);
    void EndImplicitPathAnimation();
    void PushImplicitParam(const std::shared_ptr<RSImplicitAnimationParam>& implicitParam);
    void PopImplicitParam();
    void ProcessPreCreateAnimation(const RSPropertyNode& target, const RSAnimatableProperty& property);
    void ProcessPostCreateAnimation(const RSPropertyNode& target, const RSAnimatableProperty& property);
    void CreateEmptyAnimation();

    template<typename T>
    void SetPropertyValue(RSPropertyNode& target, const RSAnimatableProperty& property, const T& value)
    {
        std::shared_ptr<RSBasePropertyAccessors> propertyAccess =
            RSBasePropertyAccessors::PROPERTY_ACCESSOR_LUT.at(property);
        auto accessors = static_cast<RSPropertyAccessors<T>*>(propertyAccess.get());
        (target.stagingProperties_.*accessors->UseSetProp())(value, false);
    }

    std::stack<std::tuple<RSAnimationTimingProtocol, RSAnimationTimingCurve, std::function<void()>>>
        globalImplicitParams_;
    std::stack<std::shared_ptr<RSImplicitAnimationParam>> implicitAnimationParams_;
    std::stack<std::vector<std::pair<std::shared_ptr<RSAnimation>, NodeId>>> implicitAnimations_;
    std::stack<std::map<std::pair<NodeId, RSAnimatableProperty>, std::shared_ptr<RSAnimation>>> keyframeAnimations_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_IMPLICIT_ANIMATOR_H
