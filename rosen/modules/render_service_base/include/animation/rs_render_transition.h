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

#ifndef RENDER_SERVICE_CLIENT_CORE_TRANSITION_RS_RENDER_TRANSITION_H
#define RENDER_SERVICE_CLIENT_CORE_TRANSITION_RS_RENDER_TRANSITION_H

#include "animation/rs_interpolator.h"
#include "animation/rs_render_animation.h"
#include "animation/rs_render_transition_effect.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSRenderTransition : public RSRenderAnimation {
public:
    RSRenderTransition(
        AnimationId id, const std::vector<std::shared_ptr<RSRenderTransitionEffect>>& effects, bool isTransitionIn);

    void DumpAnimationType(std::string& out) const override;

    void SetInterpolator(const std::shared_ptr<RSInterpolator>& interpolator)
    {
        interpolator_ = interpolator;
    }

    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static RSRenderTransition* Unmarshalling(Parcel& parcel);

protected:
    bool ParseParam(Parcel& parcel) override;
    void OnAnimate(float fraction) override;
    void OnAttach() override;
    void OnDetach() override;

    std::vector<std::shared_ptr<RSRenderTransitionEffect>> effects_;
    std::shared_ptr<RSInterpolator> interpolator_ { RSInterpolator::DEFAULT };

private:
    RSRenderTransition() = default;
    bool isTransitionIn_;
};
} // namespace Rosen
} // namespace OHOS

#endif
