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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_VALUE_ESTIMATOR_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_VALUE_ESTIMATOR_H

#include <memory>

#include "animation/rs_animation_common.h"
#include "animation/rs_interpolator.h"
#include "common/rs_color.h"
#include "common/rs_matrix3.h"
#include "common/rs_vector2.h"
#include "common/rs_vector4.h"
#include "render/rs_filter.h"

namespace OHOS {
namespace Rosen {
class RSRenderPropertyBase;

class RSValueEstimator {
public:
    static std::shared_ptr<RSRenderPropertyBase> Estimate(float fraction,
        const std::shared_ptr<RSRenderPropertyBase>& startValue,
        const std::shared_ptr<RSRenderPropertyBase>& endValue);

    static Quaternion EstimateQuaternion(float fraction, const Quaternion& startValue, const Quaternion& endValue);

    static std::shared_ptr<RSFilter> EstimateFilter(
        float fraction, const std::shared_ptr<RSFilter>& startValue, const std::shared_ptr<RSFilter>& endValue);

    static float EstimateFraction(
        const std::shared_ptr<RSInterpolator>& interpolator, const std::shared_ptr<RSRenderPropertyBase>& value,
        const std::shared_ptr<RSRenderPropertyBase>& startValue,
        const std::shared_ptr<RSRenderPropertyBase>& endValue);

    static float EstimateFloatFraction(
        const std::shared_ptr<RSInterpolator>& interpolator, const float value,
        const float startValue, const float endValue);
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_VALUE_ESTIMATOR_H
