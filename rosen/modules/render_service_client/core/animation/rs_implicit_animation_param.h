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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_IMPLICIT_ANIMATION_PARAM_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_IMPLICIT_ANIMATION_PARAM_H

#include <functional>
#include <memory>

#include "animation/rs_animation_timing_curve.h"
#include "animation/rs_animation_timing_protocol.h"
#include "animation/rs_curve_animation.h"
#include "animation/rs_keyframe_animation.h"
#include "animation/rs_motion_path_option.h"
#include "animation/rs_path_animation.h"
#include "animation/rs_transition.h"

namespace OHOS {
namespace Rosen {
enum class ImplicitAnimationParamType { NONE, CURVE, KEYFRAME, PATH, SPRING, TRANSITION };

class RSImplicitAnimationParam {
public:
    explicit RSImplicitAnimationParam(const RSAnimationTimingProtocol& timingProtocol);
    virtual ~RSImplicitAnimationParam() = default;
    ImplicitAnimationParamType GetType() const;

    template<typename T>
    std::shared_ptr<RSAnimation> CreateAnimation(
        const RSAnimatableProperty& property, const T& startValue, const T& endValue) const
    {
        return nullptr;
    }

protected:
    void ApplyTimingProtocol(const std::shared_ptr<RSAnimation>& animation) const;
    ImplicitAnimationParamType animationType_ { ImplicitAnimationParamType::NONE };

private:
    RSAnimationTimingProtocol timingProtocol_;
};

class RSImplicitCurveAnimationParam : public RSImplicitAnimationParam {
public:
    RSImplicitCurveAnimationParam(
        const RSAnimationTimingProtocol& timingProtocol, const RSAnimationTimingCurve& timingCurve);

    virtual ~RSImplicitCurveAnimationParam() = default;

    template<typename T>
    std::shared_ptr<RSAnimation> CreateAnimation(
        const RSAnimatableProperty& property, const T& startValue, const T& endValue) const
    {
        auto curveAnimation = std::make_shared<RSCurveAnimation<T>>(property, endValue - startValue);
        curveAnimation->SetTimingCurve(timingCurve_);
        ApplyTimingProtocol(curveAnimation);
        return curveAnimation;
    }

private:
    RSAnimationTimingCurve timingCurve_;
};

class RSImplicitKeyframeAnimationParam : public RSImplicitAnimationParam {
public:
    RSImplicitKeyframeAnimationParam(
        const RSAnimationTimingProtocol& timingProtocol, const RSAnimationTimingCurve& timingCurve, float fraction);

    virtual ~RSImplicitKeyframeAnimationParam() = default;

    template<typename T>
    std::shared_ptr<RSAnimation> CreateAnimation(
        const RSAnimatableProperty& property, const T& startValue, const T& endValue) const
    {
        auto keyFrameAnimation = std::make_shared<RSKeyframeAnimation<T>>(property);
        keyFrameAnimation->AddKeyFrame(fraction_, endValue, timingCurve_);
        keyFrameAnimation->SetOriginValue(startValue);
        ApplyTimingProtocol(keyFrameAnimation);
        return keyFrameAnimation;
    }

    template<typename T>
    void AddKeyframe(std::shared_ptr<RSAnimation>& animation, const T& startValue, const T& endValue) const
    {
        if (animation == nullptr) {
            return;
        }

        auto keyframeAnimation = std::static_pointer_cast<RSKeyframeAnimation<T>>(animation);
        if (keyframeAnimation != nullptr) {
            keyframeAnimation->AddKeyFrame(fraction_, endValue, timingCurve_);
        }
    }

private:
    RSAnimationTimingCurve timingCurve_;
    float fraction_;
};

class RSImplicitPathAnimationParam : public RSImplicitAnimationParam {
public:
    RSImplicitPathAnimationParam(const RSAnimationTimingProtocol& timingProtocol,
        const RSAnimationTimingCurve& timingCurve, const std::shared_ptr<RSMotionPathOption>& motionPathOption);

    virtual ~RSImplicitPathAnimationParam() = default;

    template<typename T>
    std::shared_ptr<RSAnimation> CreateAnimation(
        const RSAnimatableProperty& property, const T& startValue, const T& endValue) const
    {
        return nullptr;
    }

    std::shared_ptr<RSAnimation> CreateAnimation(
        const RSAnimatableProperty& property, const Vector2f& startValue, const Vector2f& endValue) const;

private:
    RSAnimationTimingCurve timingCurve_;
    std::shared_ptr<RSMotionPathOption> motionPathOption_;
};

class RSImplicitTransitionParam : public RSImplicitAnimationParam {
public:
    RSImplicitTransitionParam(const RSAnimationTimingProtocol& timingProtocol,
        const RSAnimationTimingCurve& timingCurve, const RSTransitionEffect& effect);
    virtual ~RSImplicitTransitionParam() = default;

    std::shared_ptr<RSAnimation> CreateAnimation();

private:
    RSAnimationTimingCurve timingCurve_;
    RSTransitionEffect effect_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_IMPLICIT_ANIMATION_PARAM_H
