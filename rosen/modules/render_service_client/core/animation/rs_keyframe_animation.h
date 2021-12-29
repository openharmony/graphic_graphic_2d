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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_KEYFRAME_ANIMATION_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_KEYFRAME_ANIMATION_H

#include <memory>

#include "animation/rs_animation_common.h"
#include "animation/rs_animation_timing_curve.h"
#include "animation/rs_property_animation.h"
#include "common/rs_color.h"
#include "common/rs_matrix3.h"
#include "common/rs_vector4.h"

namespace OHOS {
namespace Rosen {
static constexpr int FRACTION_INDEX = 0;
static constexpr int VALUE_INDEX = 1;
static constexpr int INTERPOLATOR_INDEX = 2;

template<typename T>
class RS_EXPORT RSKeyframeAnimation : public RSPropertyAnimation<T> {
public:
    RSKeyframeAnimation(const RSAnimatableProperty& property) : RSPropertyAnimation<T>(property) {}
    virtual ~RSKeyframeAnimation() = default;

    void AddKeyFrame(float fraction, const T& value, const RSAnimationTimingCurve& timingCurve)
    {
        if (fraction < FRACTION_MIN || fraction > FRACTION_MAX) {
            return;
        }

        if (RSAnimation::IsStarted()) {
            return;
        }

        keyframes_.push_back({ fraction, value, timingCurve });
    }

    void AddKeyFrames(const std::vector<std::tuple<float, T, RSAnimationTimingCurve>>& keyframes)
    {
        if (RSAnimation::IsStarted()) {
            return;
        }

        keyframes_ = keyframes;
    }

protected:
    void OnStart() override {}

    void InitInterpolationValue() override
    {
        if (keyframes_.empty()) {
            return;
        }

        auto beginKeyframe = keyframes_.front();
        if (std::abs(std::get<FRACTION_INDEX>(beginKeyframe) - FRACTION_MIN) > EPSILON) {
            keyframes_.insert(keyframes_.begin(),
                { FRACTION_MIN, RSPropertyAnimation<T>::GetOriginValue(), RSAnimationTimingCurve::LINEAR });
        }

        RSPropertyAnimation<T>::startValue_ = std::get<VALUE_INDEX>(keyframes_.front());
        RSPropertyAnimation<T>::endValue_ = std::get<VALUE_INDEX>(keyframes_.back());
        RSPropertyAnimation<T>::InitInterpolationValue();
    }

private:
    std::vector<std::tuple<float, T, RSAnimationTimingCurve>> keyframes_;

    friend class RSImplicitKeyframeAnimationParam;
};

template<>
void RSKeyframeAnimation<int>::OnStart();

template<>
void RSKeyframeAnimation<float>::OnStart();

template<>
void RSKeyframeAnimation<Color>::OnStart();

template<>
void RSKeyframeAnimation<Matrix3f>::OnStart();

template<>
void RSKeyframeAnimation<Vector2f>::OnStart();

template<>
void RSKeyframeAnimation<Vector4f>::OnStart();

template<>
void RSKeyframeAnimation<std::shared_ptr<RSFilter>>::OnStart();
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_KEYFRAME_ANIMATION_H
