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

#ifndef ROSEN_ENGINE_CORE_ANIMATION_RS_SPRING_INTERPOLATOR_H
#define ROSEN_ENGINE_CORE_ANIMATION_RS_SPRING_INTERPOLATOR_H

#include "animation/rs_interpolator.h"
#include "animation/rs_spring_model.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSSpringInterpolator : public RSSpringModel<float>, public RSInterpolator {
public:
    RSSpringInterpolator(float response, float dampingRatio, float initialVelocity);

    ~RSSpringInterpolator() override {};

    float InterpolateImpl(float fraction) const override;
    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static RSSpringInterpolator* Unmarshalling(Parcel& parcel);

private:
    RSSpringInterpolator(uint64_t id, float response, float dampingRatio, float initialVelocity);
    float estimatedDuration_ = 0.0f;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_ENGINE_CORE_ANIMATION_RS_SPRING_INTERPOLATOR_H
