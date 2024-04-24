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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_VALUE_ESTIMATOR_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_VALUE_ESTIMATOR_H

#include <memory>

#include "rs_spring_model.h"

#include "animation/rs_animation_common.h"
#include "animation/rs_interpolator.h"
#include "common/rs_color.h"
#include "common/rs_macros.h"
#include "common/rs_matrix3.h"
#include "common/rs_vector2.h"
#include "common/rs_vector4.h"
#include "modifier/rs_modifier_type.h"
#include "render/rs_filter.h"

namespace OHOS {
namespace Rosen {
class RSRenderPropertyBase;
template<typename T>
class RSRenderAnimatableProperty;

class RSB_EXPORT RSValueEstimator {
public:
    template<typename T>
    T Estimate(float fraction, const T& startValue, const T& endValue)
    {
        return startValue * (1.0f - fraction) + endValue * fraction;
    }

    Quaternion Estimate(float fraction, const Quaternion& startValue, const Quaternion& endValue);

    std::shared_ptr<RSFilter> Estimate(
        float fraction, const std::shared_ptr<RSFilter>& startValue, const std::shared_ptr<RSFilter>& endValue);

    virtual float EstimateFraction(const std::shared_ptr<RSInterpolator>& interpolator)
    {
        return 0.0f;
    }

    virtual void InitCurveAnimationValue(const std::shared_ptr<RSRenderPropertyBase>& property,
        const std::shared_ptr<RSRenderPropertyBase>& startValue,
        const std::shared_ptr<RSRenderPropertyBase>& endValue,
        const std::shared_ptr<RSRenderPropertyBase>& lastValue) {}

    virtual void InitKeyframeAnimationValue(const std::shared_ptr<RSRenderPropertyBase>& property,
        std::vector<std::tuple<float, std::shared_ptr<RSRenderPropertyBase>,
        std::shared_ptr<RSInterpolator>>>& keyframes,
        const std::shared_ptr<RSRenderPropertyBase>& lastValue) {}
    
    virtual void InitDurationKeyframeAnimationValue(const std::shared_ptr<RSRenderPropertyBase>& property,
        std::vector<std::tuple<float, float, std::shared_ptr<RSRenderPropertyBase>,
        std::shared_ptr<RSInterpolator>>>& keyframes,
        const std::shared_ptr<RSRenderPropertyBase>& lastValue) {}

    virtual void UpdateAnimationValue(const float fraction, const bool isAdditive) = 0;
};

template<typename T>
class RSB_EXPORT_TMP RSCurveValueEstimator : public RSValueEstimator {
public:
    RSCurveValueEstimator() = default;
    virtual ~RSCurveValueEstimator() = default;

    void InitCurveAnimationValue(const std::shared_ptr<RSRenderPropertyBase>& property,
        const std::shared_ptr<RSRenderPropertyBase>& startValue,
        const std::shared_ptr<RSRenderPropertyBase>& endValue,
        const std::shared_ptr<RSRenderPropertyBase>& lastValue) override
    {
        auto animatableProperty = std::static_pointer_cast<RSRenderAnimatableProperty<T>>(property);
        auto animatableStartValue = std::static_pointer_cast<RSRenderAnimatableProperty<T>>(startValue);
        auto animatableEndValue = std::static_pointer_cast<RSRenderAnimatableProperty<T>>(endValue);
        auto animatableLastValue = std::static_pointer_cast<RSRenderAnimatableProperty<T>>(lastValue);
        if (animatableProperty && animatableStartValue && animatableEndValue && animatableLastValue) {
            property_ = animatableProperty;
            startValue_ = animatableStartValue->Get();
            endValue_ = animatableEndValue->Get();
            lastValue_ = animatableLastValue->Get();
        }
    }

    void UpdateAnimationValue(const float fraction, const bool isAdditive) override
    {
        auto animationValue = GetAnimationValue(fraction, isAdditive);
        if (property_ != nullptr) {
            property_->Set(animationValue);
        }
    }

    T GetAnimationValue(const float fraction, const bool isAdditive)
    {
        auto interpolationValue = RSValueEstimator::Estimate(fraction, startValue_, endValue_);
        auto animationValue = interpolationValue;
        if (isAdditive && property_ != nullptr) {
            animationValue = property_->Get() + (interpolationValue - lastValue_);
        }
        lastValue_ = interpolationValue;
        return animationValue;
    }

    float EstimateFraction(const std::shared_ptr<RSInterpolator>& interpolator) override
    {
        return 0.0f;
    }

private:
    T startValue_ {};
    T endValue_ {};
    T lastValue_ {};
    std::shared_ptr<RSRenderAnimatableProperty<T>> property_;
};

template<>
float RSCurveValueEstimator<float>::EstimateFraction(const std::shared_ptr<RSInterpolator>& interpolator);

extern template class RSCurveValueEstimator<float>;

template<typename T>
class RSKeyframeValueEstimator : public RSValueEstimator {
public:
    RSKeyframeValueEstimator() = default;
    virtual ~RSKeyframeValueEstimator() = default;

    void InitKeyframeAnimationValue(const std::shared_ptr<RSRenderPropertyBase>& property,
        std::vector<std::tuple<float, std::shared_ptr<RSRenderPropertyBase>,
        std::shared_ptr<RSInterpolator>>>& keyframes,
        const std::shared_ptr<RSRenderPropertyBase>& lastValue) override
    {
        auto animatableProperty = std::static_pointer_cast<RSRenderAnimatableProperty<T>>(property);
        auto animatableLastValue = std::static_pointer_cast<RSRenderAnimatableProperty<T>>(lastValue);
        if (animatableProperty && animatableLastValue) {
            property_ = animatableProperty;
            lastValue_ = animatableLastValue->Get();
        }
        for (const auto& keyframe : keyframes) {
            auto keyframeValue = std::static_pointer_cast<RSRenderAnimatableProperty<T>>(std::get<1>(keyframe));
            if (keyframeValue != nullptr) {
                keyframes_.push_back({ std::get<0>(keyframe), keyframeValue->Get(), std::get<2>(keyframe) });
            }
        }
    }

    void InitDurationKeyframeAnimationValue(const std::shared_ptr<RSRenderPropertyBase>& property,
        std::vector<std::tuple<float, float, std::shared_ptr<RSRenderPropertyBase>,
        std::shared_ptr<RSInterpolator>>>& keyframes,
        const std::shared_ptr<RSRenderPropertyBase>& lastValue) override
    {
        auto animatableProperty = std::static_pointer_cast<RSRenderAnimatableProperty<T>>(property);
        auto animatableLastValue = std::static_pointer_cast<RSRenderAnimatableProperty<T>>(lastValue);
        if (animatableProperty && animatableLastValue) {
            property_ = animatableProperty;
            lastValue_ = animatableLastValue->Get();
        }
        for (const auto& keyframe : keyframes) {
            auto keyframeValue = std::static_pointer_cast<RSRenderAnimatableProperty<T>>(std::get<2>(keyframe));
            if (keyframeValue != nullptr) {
                durationKeyframes_.push_back(
                    { std::get<0>(keyframe), std::get<1>(keyframe), keyframeValue->Get(), std::get<3>(keyframe) });
            }
        }
    }

    void UpdateAnimationValue(const float fraction, const bool isAdditive) override
    {
        auto animationValue = GetAnimationValue(fraction, isAdditive);
        if (property_ != nullptr) {
            property_->Set(animationValue);
        }
    }

    T GetAnimationValue(const float fraction, const bool isAdditive)
    {
        if (!(durationKeyframes_.empty())) {
            return GetDurationKeyframeAnimationValue(fraction, isAdditive);
        }
        float preKeyframeFraction = std::get<0>(keyframes_.front());
        auto preKeyframeValue = std::get<1>(keyframes_.front());
        for (const auto& keyframe : keyframes_) {
            // the index of tuple
            float keyframeFraction = std::get<0>(keyframe);
            auto keyframeValue = std::get<1>(keyframe);
            auto keyframeInterpolator = std::get<2>(keyframe);
            if (fraction <= keyframeFraction) {
                if (ROSEN_EQ(keyframeFraction, preKeyframeFraction)) {
                    continue;
                }

                float intervalFraction = (fraction - preKeyframeFraction) / (keyframeFraction - preKeyframeFraction);
                auto interpolationValue = RSValueEstimator::Estimate(
                    keyframeInterpolator->Interpolate(intervalFraction), preKeyframeValue, keyframeValue);
                auto animationValue = interpolationValue;
                if (isAdditive && property_ != nullptr) {
                    animationValue = property_->Get() + (interpolationValue - lastValue_);
                }
                lastValue_ = interpolationValue;
                return animationValue;
            }

            preKeyframeFraction = keyframeFraction;
            preKeyframeValue = keyframeValue;
        }
        return preKeyframeValue;
    }

    T GetDurationKeyframeAnimationValue(const float fraction, const bool isAdditive)
    {
        auto preKeyframeValue = std::get<2>(durationKeyframes_.front());
        auto animationValue = preKeyframeValue;
        bool bInFraction = false;
        for (const auto& keyframe : durationKeyframes_) {
            float startFraction = std::get<0>(keyframe);
            float endFraction = std::get<1>(keyframe);
            auto keyframeValue = std::get<2>(keyframe);
            auto keyframeInterpolator = std::get<3>(keyframe);
            if (fraction < startFraction) {
                break;
            }
            if ((fraction > startFraction) && (fraction <= endFraction)) {
                bInFraction = true;
                float intervalFraction = (fraction - startFraction) / (endFraction - startFraction);
                auto interpolationValue = RSValueEstimator::Estimate(
                    keyframeInterpolator->Interpolate(intervalFraction), preKeyframeValue, keyframeValue);
                animationValue = interpolationValue;
                if (isAdditive && property_ != nullptr) {
                    animationValue = property_->Get() + (interpolationValue - lastValue_);
                }
                lastValue_ = interpolationValue;
                preKeyframeValue = animationValue;
                continue;
            }
            if (fraction == startFraction && startFraction == endFraction) {
                bInFraction = true;
                animationValue = keyframeValue;
                preKeyframeValue = keyframeValue;
                lastValue_ = keyframeValue;
                continue;
            }
            preKeyframeValue = keyframeValue;
        }
        if (!bInFraction) {
            animationValue = preKeyframeValue;
            lastValue_ = preKeyframeValue;
        }
        return animationValue;
    }

private:
    std::vector<std::tuple<float, T, std::shared_ptr<RSInterpolator>>> keyframes_;
    std::vector<std::tuple<float, float, T, std::shared_ptr<RSInterpolator>>> durationKeyframes_;
    T lastValue_ {};
    std::shared_ptr<RSRenderAnimatableProperty<T>> property_;
};

enum class RSValueEstimatorType : int16_t {
    INVALID = 0,
    CURVE_VALUE_ESTIMATOR,
    KEYFRAME_VALUE_ESTIMATOR,
    PATH_VALUE_ESTIMATOR,
};

class RSB_EXPORT RSSpringValueEstimatorBase {
public:
    RSSpringValueEstimatorBase() = default;
    virtual ~RSSpringValueEstimatorBase() = default;

    virtual void SetResponse(const float response) {}
    virtual void SetDampingRatio(const float dampingRatio) {}
    virtual float GetResponse() const
    {
        return 0.0f;
    }
    virtual float GetDampingRatio() const
    {
        return 0.0f;
    }
    virtual void SetInitialVelocity(const std::shared_ptr<RSRenderPropertyBase>& initialVelocity) {}
    virtual void InitRSSpringValueEstimator(const std::shared_ptr<RSRenderPropertyBase>& property,
        const std::shared_ptr<RSRenderPropertyBase>& startValue, const std::shared_ptr<RSRenderPropertyBase>& endValue,
        const std::shared_ptr<RSRenderPropertyBase>& lastValue)
    {}
    virtual void UpdateStartValueAndLastValue(
        const std::shared_ptr<RSRenderPropertyBase>& startValue, const std::shared_ptr<RSRenderPropertyBase>& lastValue)
    {}
    virtual void UpdateAnimationValue(const float time, const bool isAdditive) {}
    virtual std::shared_ptr<RSRenderPropertyBase> GetAnimationProperty() const
    {
        return nullptr;
    }
    virtual std::shared_ptr<RSRenderPropertyBase> GetPropertyVelocity(float time) const
    {
        return nullptr;
    }
    virtual float UpdateDuration()
    {
        return 0.0f;
    }
    virtual void UpdateSpringParameters() {}
};

template<typename T>
class RSSpringValueEstimator : public RSSpringValueEstimatorBase {
public:
    RSSpringValueEstimator() : RSSpringValueEstimatorBase()
    {
        InitSpringModel();
    }

    ~RSSpringValueEstimator() override = default;

    void InitSpringModel()
    {
        if (!springModel_) {
            springModel_ = std::make_shared<RSSpringModel<T>>();
        }
    }

    void SetResponse(const float response) override
    {
        if (springModel_) {
            springModel_->response_ = response;
        }
    }

    void SetDampingRatio(const float dampingRatio) override
    {
        if (springModel_) {
            springModel_->dampingRatio_ = dampingRatio;
        }
    }

    float GetResponse() const override
    {
        if (springModel_) {
            return springModel_->response_;
        }
        return 0.0f;
    }

    float GetDampingRatio() const override
    {
        if (springModel_) {
            return springModel_->dampingRatio_;
        }
        return 0.0f;
    }

    void SetInitialVelocity(const std::shared_ptr<RSRenderPropertyBase>& initialVelocity) override
    {
        auto animatableInitialVelocity = std::static_pointer_cast<RSRenderAnimatableProperty<T>>(initialVelocity);
        if (animatableInitialVelocity != nullptr && springModel_ != nullptr) {
            springModel_->initialVelocity_ = animatableInitialVelocity->Get();
        }
    }

    void InitRSSpringValueEstimator(const std::shared_ptr<RSRenderPropertyBase>& property,
        const std::shared_ptr<RSRenderPropertyBase>& startValue, const std::shared_ptr<RSRenderPropertyBase>& endValue,
        const std::shared_ptr<RSRenderPropertyBase>& lastValue) override
    {
        auto animatableProperty = std::static_pointer_cast<RSRenderAnimatableProperty<T>>(property);
        auto animatableStartValue = std::static_pointer_cast<RSRenderAnimatableProperty<T>>(startValue);
        auto animatableEndValue = std::static_pointer_cast<RSRenderAnimatableProperty<T>>(endValue);
        auto animatableLastValue = std::static_pointer_cast<RSRenderAnimatableProperty<T>>(lastValue);
        if (animatableProperty && animatableStartValue && animatableEndValue && animatableLastValue) {
            property_ = animatableProperty;
            startValue_ = animatableStartValue->Get();
            endValue_ = animatableEndValue->Get();
            lastValue_ = animatableLastValue->Get();
            if (springModel_) {
                springModel_->initialOffset_ = startValue_ - endValue_;
            }
        }
    }

    void UpdateStartValueAndLastValue(const std::shared_ptr<RSRenderPropertyBase>& startValue,
        const std::shared_ptr<RSRenderPropertyBase>& lastValue) override
    {
        auto animatableStartValue = std::static_pointer_cast<RSRenderAnimatableProperty<T>>(startValue);
        auto animatableLastValue = std::static_pointer_cast<RSRenderAnimatableProperty<T>>(lastValue);
        if (animatableStartValue && animatableLastValue) {
            startValue_ = animatableStartValue->Get();
            lastValue_ = animatableLastValue->Get();
            if (springModel_) {
                springModel_->initialOffset_ = startValue_ - endValue_;
            }
        }
    }

    void UpdateAnimationValue(const float time, const bool isAdditive) override
    {
        auto animationValue = GetAnimationValue(time, isAdditive);
        if (property_ != nullptr) {
            property_->Set(animationValue);
        }
    }

    T GetAnimationValue(const float time, const bool isAdditive)
    {
        T currentValue = startValue_;
        constexpr static float TIME_THRESHOLD = 1e-3f;
        if (ROSEN_EQ(time, duration_, TIME_THRESHOLD)) {
            currentValue = endValue_;
        } else if (springModel_) {
            currentValue = springModel_->CalculateDisplacement(time) + endValue_;
        }

        auto animationValue = currentValue;
        if (isAdditive && property_) {
            animationValue = property_->Get() + currentValue - lastValue_;
        }
        lastValue_ = currentValue;
        return animationValue;
    }

    std::shared_ptr<RSRenderPropertyBase> GetAnimationProperty() const override
    {
        return std::make_shared<RSRenderAnimatableProperty<T>>(lastValue_);
    }

    std::shared_ptr<RSRenderPropertyBase> GetPropertyVelocity(float time) const override
    {
        if (!springModel_) {
            return nullptr;
        }
        constexpr float TIME_INTERVAL = 1e-6f; // 1 microsecond
        T velocity = (springModel_->CalculateDisplacement(time + TIME_INTERVAL) -
            springModel_->CalculateDisplacement(time)) * (1 / TIME_INTERVAL);
        return std::make_shared<RSRenderAnimatableProperty<T>>(velocity);
    }

    float UpdateDuration() override
    {
        if (springModel_) {
            duration_ = springModel_->EstimateDuration();
        }
        return duration_;
    }

    void UpdateSpringParameters() override
    {
        if (springModel_) {
            springModel_->CalculateSpringParameters();
        }
    }

private:
    T startValue_ {};
    T endValue_ {};
    T lastValue_ {};
    float duration_ = 0.3f;
    std::shared_ptr<RSSpringModel<T>> springModel_;
    std::shared_ptr<RSRenderAnimatableProperty<T>> property_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_VALUE_ESTIMATOR_H
