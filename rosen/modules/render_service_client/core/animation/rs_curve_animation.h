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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_CURVE_ANIMATION_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_CURVE_ANIMATION_H

#include <iostream>
#include <memory>

#include "animation/rs_animation_timing_curve.h"
#include "animation/rs_property_animation.h"
#include "common/rs_color.h"
#include "common/rs_matrix3.h"
#include "common/rs_vector4.h"
#include "render/rs_filter.h"

namespace OHOS {
namespace Rosen {
template<typename T>
class RS_EXPORT RSCurveAnimation : public RSPropertyAnimation<T> {
public:
    RSCurveAnimation(RSAnimatableProperty<T>& property, const T& byValue) : RSPropertyAnimation<T>(property)
    {
        RSPropertyAnimation<T>::isDelta_ = true;
        RSPropertyAnimation<T>::byValue_ = byValue;
    }

    RSCurveAnimation(RSAnimatableProperty<T>& property, const T& startValue, const T& endValue)
        : RSPropertyAnimation<T>(property)
    {
        RSPropertyAnimation<T>::isDelta_ = false;
        RSPropertyAnimation<T>::startValue_ = startValue;
        RSPropertyAnimation<T>::endValue_ = endValue;
    }

    virtual ~RSCurveAnimation() = default;

    void SetTimingCurve(const RSAnimationTimingCurve& timingCurve)
    {
        if (timingCurve.type_ != RSAnimationTimingCurve::CurveType::INTERPOLATING) {
            ROSEN_LOGE("RSCurveAnimation::SetTimingCurve: invalid timing curve type");
            return;
        }
        timingCurve_ = timingCurve;
    }

    const RSAnimationTimingCurve& GetTimingCurve() const
    {
        return timingCurve_;
    }

protected:
    void OnStart() override;

private:
    template<typename P>
    void StartAnimationImpl();

    RSAnimationTimingCurve timingCurve_ { RSAnimationTimingCurve::DEFAULT };
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_CURVE_ANIMATION_H
