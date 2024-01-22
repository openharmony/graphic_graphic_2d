/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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
#include <vector>

#include "animation/rs_animation_timing_curve.h"
#include "animation/rs_animation_timing_protocol.h"
#include "modifier/rs_property.h"

namespace OHOS {
namespace Rosen {
enum class ImplicitAnimationParamType {
    INVALID,
    CURVE,
    KEYFRAME,
    PATH,
    SPRING,
    INTERPOLATING_SPRING,
    TRANSITION,
    CANCEL
};
class RSAnimation;
class RSPropertyBase;
class RSMotionPathOption;
class RSTransition;
class RSTransitionEffect;

class RSImplicitAnimationParam {
public:
    virtual ~RSImplicitAnimationParam() = default;
    ImplicitAnimationParamType GetType() const;

protected:
    explicit RSImplicitAnimationParam(const RSAnimationTimingProtocol& timingProtocol, ImplicitAnimationParamType type);
    void ApplyTimingProtocol(const std::shared_ptr<RSAnimation>& animation) const;
    ImplicitAnimationParamType animationType_ { ImplicitAnimationParamType::INVALID };

private:
    RSAnimationTimingProtocol timingProtocol_;
};

class RSImplicitCancelAnimationParam : public RSImplicitAnimationParam {
public:
    RSImplicitCancelAnimationParam(const RSAnimationTimingProtocol& timingProtocol);

    ~RSImplicitCancelAnimationParam() override = default;

    void AddPropertyToPendingSyncList(const std::shared_ptr<RSPropertyBase>& property);
    void SyncProperties();

    std::shared_ptr<RSAnimation> CreateEmptyAnimation(std::shared_ptr<RSPropertyBase> property,
        const std::shared_ptr<RSPropertyBase>& startValue, const std::shared_ptr<RSPropertyBase>& endValue) const;

private:
    void ExecuteSyncPropertiesTask(
        RSNodeGetShowingPropertiesAndCancelAnimation::PropertiesMap&& propertiesMap, bool isRenderService);
    std::vector<std::shared_ptr<RSPropertyBase>> pendingSyncList_;
};

class RSImplicitCurveAnimationParam : public RSImplicitAnimationParam {
public:
    RSImplicitCurveAnimationParam(
        const RSAnimationTimingProtocol& timingProtocol, const RSAnimationTimingCurve& timingCurve);

    ~RSImplicitCurveAnimationParam() override = default;

    std::shared_ptr<RSAnimation> CreateAnimation(std::shared_ptr<RSPropertyBase> property,
        const std::shared_ptr<RSPropertyBase>& startValue, const std::shared_ptr<RSPropertyBase>& endValue) const;

private:
    RSAnimationTimingCurve timingCurve_;
};

class RSImplicitKeyframeAnimationParam : public RSImplicitAnimationParam {
public:
    RSImplicitKeyframeAnimationParam(
        const RSAnimationTimingProtocol& timingProtocol, const RSAnimationTimingCurve& timingCurve,
        float fraction, int duration);

    ~RSImplicitKeyframeAnimationParam() override = default;

    std::shared_ptr<RSAnimation> CreateAnimation(
        std::shared_ptr<RSPropertyBase> property, const bool& isCreateDurationKeyframe, const int& startDuration,
        const std::shared_ptr<RSPropertyBase>& startValue, const std::shared_ptr<RSPropertyBase>& endValue) const;

    void AddKeyframe(std::shared_ptr<RSAnimation>& animation, const std::shared_ptr<RSPropertyBase>& startValue,
        const std::shared_ptr<RSPropertyBase>& endValue) const;

    void AddKeyframe(std::shared_ptr<RSAnimation>& animation, const int startDuration,
        const std::shared_ptr<RSPropertyBase>& startValue, const std::shared_ptr<RSPropertyBase>& endValue) const;

private:
    RSAnimationTimingCurve timingCurve_;
    float fraction_;
    int duration_;
};

class RSImplicitPathAnimationParam : public RSImplicitAnimationParam {
public:
    RSImplicitPathAnimationParam(const RSAnimationTimingProtocol& timingProtocol,
        const RSAnimationTimingCurve& timingCurve, const std::shared_ptr<RSMotionPathOption>& motionPathOption);

    ~RSImplicitPathAnimationParam() override = default;

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
    ~RSImplicitSpringAnimationParam() override = default;

    std::shared_ptr<RSAnimation> CreateAnimation(std::shared_ptr<RSPropertyBase> property,
        const std::shared_ptr<RSPropertyBase>& startValue, const std::shared_ptr<RSPropertyBase>& endValue) const;

private:
    RSAnimationTimingCurve timingCurve_;
};

class RSImplicitInterpolatingSpringAnimationParam : public RSImplicitAnimationParam {
public:
    RSImplicitInterpolatingSpringAnimationParam(
        const RSAnimationTimingProtocol& timingProtocol, const RSAnimationTimingCurve& timingCurve);
    ~RSImplicitInterpolatingSpringAnimationParam() override = default;

    std::shared_ptr<RSAnimation> CreateAnimation(std::shared_ptr<RSPropertyBase> property,
        const std::shared_ptr<RSPropertyBase>& startValue, const std::shared_ptr<RSPropertyBase>& endValue) const;

private:
    RSAnimationTimingCurve timingCurve_;
};

class RSImplicitTransitionParam : public RSImplicitAnimationParam {
public:
    RSImplicitTransitionParam(const RSAnimationTimingProtocol& timingProtocol,
        const RSAnimationTimingCurve& timingCurve, const std::shared_ptr<const RSTransitionEffect>& effect,
        bool isTransitionIn);
    ~RSImplicitTransitionParam() override = default;

    std::shared_ptr<RSAnimation> CreateAnimation();

    std::shared_ptr<RSAnimation> CreateAnimation(const std::shared_ptr<RSPropertyBase>& property,
        const std::shared_ptr<RSPropertyBase>& startValue, const std::shared_ptr<RSPropertyBase>& endValue);

private:
    RSAnimationTimingCurve timingCurve_;

    bool isTransitionIn_;
    std::shared_ptr<RSTransition> transition_;
    const std::shared_ptr<const RSTransitionEffect> effect_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_IMPLICIT_ANIMATION_PARAM_H
