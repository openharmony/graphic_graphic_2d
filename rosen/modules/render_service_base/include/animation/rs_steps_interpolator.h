/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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

#ifndef ROSEN_ENGINE_CORE_ANIMATION_RS_STEPS_INTERPOLATOR_H
#define ROSEN_ENGINE_CORE_ANIMATION_RS_STEPS_INTERPOLATOR_H

#include "animation/rs_interpolator.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
enum class StepsCurvePosition {
    START,
    END,
};

class RSB_EXPORT RSStepsInterpolator : public RSInterpolator {
public:
    RSStepsInterpolator(int32_t steps, StepsCurvePosition position = StepsCurvePosition::START);
    ~RSStepsInterpolator() override = default;

    float InterpolateImpl(float fraction) const override;
    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static RSStepsInterpolator* Unmarshalling(Parcel& parcel);
private:
    RSStepsInterpolator(uint64_t id, int32_t steps, StepsCurvePosition position = StepsCurvePosition::START);
    int32_t steps_;
    const StepsCurvePosition position_;
    float fractionMin = 0.0f;
    float fractionMax = 1.0f;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_ENGINE_CORE_ANIMATION_RS_STEPS_INTERPOLATOR_H
