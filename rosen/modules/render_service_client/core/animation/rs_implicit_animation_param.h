/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include "modifier/rs_property.h"

namespace OHOS {
namespace Rosen {
enum class ImplicitAnimationParamType { NONE, CURVE, KEYFRAME, PATH, SPRING, TRANSITION };

class RSAnimation;
class RSPropertyBase;
class RSMotionPathOption;
class RSTransitionEffect;

class RSImplicitAnimationParam {
public:
    explicit RSImplicitAnimationParam(const RSAnimationTimingProtocol& timingProtocol);
    virtual ~RSImplicitAnimationParam() = default;
    ImplicitAnimationParamType GetType() const;

    std::shared_ptr<RSAnimation> CreateAnimation(std::shared_ptr<RSPropertyBase> property,
        const std::shared_ptr<RSPropertyBase>& startValue, const std::shared_ptr<RSPropertyBase>& endValue) const
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

    std::shared_ptr<RSAnimation> CreateAnimation(std::shared_ptr<RSPropertyBase> property,
        const std::shared_ptr<RSPropertyBase>& startValue, const std::shared_ptr<RSPropertyBase>& endValue) const;

private:
    RSAnimationTimingCurve timingCurve_;
};

class RSImplicitKeyframeAnimationParam : public RSImplicitAnimationParam {
public:
    RSImplicitKeyframeAnimationParam(
        const RSAnimationTimingProtocol& timingProtocol, const RSAnimationTimingCurve& timingCurve, float fraction);

    virtual ~RSImplicitKeyframeAnimationParam() = default;

    std::shared_ptr<RSAnimation> CreateAnimation(std::shared_ptr<RSPropertyBase> property,
        const std::shared_ptr<RSPropertyBase>& startValue, const std::shared_ptr<RSPropertyBase>& endValue) const;

    void AddKeyframe(std::shared_ptr<RSAnimation>& animation, const std::shared_ptr<RSPropertyBase>& startValue,
        const std::shared_ptr<RSPropertyBase>& endValue) const;

private:
    RSAnimationTimingCurve timingCurve_;
    float fraction_;
};

class RSImplicitPathAnimationParam : public RSImplicitAnimationParam {
public:
    RSImplicitPathAnimationParam(const RSAnimationTimingProtocol& timingProtocol,
        const RSAnimationTimingCurve& timingCurve, const std::shared_ptr<RSMotionPathOption>& motionPathOption);

    virtual ~RSImplicitPathAnimationParam() = default;

    std::shared_ptr<RSAnimation> CreateAnimation(std::shared_ptr<RSPropertyBase> property,
        const std::shared_ptr<RSPropertyBase>& startValue, const std::shared_ptr<RSPropertyBase>& endValue) const;

private:
    RSAnimationTimingCurve timingCurve_;
    std::shared_ptr<RSMotionPathOption> motionPathOption_;
};

class RSImplicitSpringAnimationParam : public RSImplicitAnimationParam {
public:
    RSImplicitSpringAnimationParam(
        const RSAnimationTimingProtocol& timingProtocol, const RSAnimationTimingCurve& timingCurve);
    virtual ~RSImplicitSpringAnimationParam() = default;

    std::shared_ptr<RSAnimation> CreateAnimation(std::shared_ptr<RSPropertyBase> property,
        const std::shared_ptr<RSPropertyBase>& startValue, const std::shared_ptr<RSPropertyBase>& endValue) const;

private:
    RSAnimationTimingCurve timingCurve_;
};

class RSImplicitTransitionParam : public RSImplicitAnimationParam {
public:
    RSImplicitTransitionParam(const RSAnimationTimingProtocol& timingProtocol,
        const RSAnimationTimingCurve& timingCurve, const std::shared_ptr<const RSTransitionEffect>& effect);
    virtual ~RSImplicitTransitionParam() = default;

    std::shared_ptr<RSAnimation> CreateAnimation(bool isTransitionIn);

private:
    RSAnimationTimingCurve timingCurve_;
    const std::shared_ptr<const RSTransitionEffect> effect_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_IMPLICIT_ANIMATION_PARAM_H
