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

#ifndef RENDER_SERVICE_CLIENT_CORE_TRANSITION_RS_TRANSITION_H
#define RENDER_SERVICE_CLIENT_CORE_TRANSITION_RS_TRANSITION_H

#include "animation/rs_animation.h"
#include "animation/rs_animation_timing_curve.h"
#include "animation/rs_interpolator.h"
#include "animation/rs_animation_common.h"

namespace OHOS {
namespace Rosen {

class RS_EXPORT RSTransition : public RSAnimation {
public:
    RSTransition(const RSTransitionEffect& effect);
    virtual ~RSTransition() = default;

    void SetTransitionEffect(const RSTransitionEffect& effect)
    {
        effect_ = effect;
    }

    void SetTimingCurve(const RSAnimationTimingCurve& timingCurve)
    {
        timingCurve_ = timingCurve;
    }

protected:
    void OnStart() override;

private:
    RSTransitionEffect effect_;
    RSAnimationTimingCurve timingCurve_ { RSAnimationTimingCurve::DEFAULT };
};
} // namespace Rosen
} // namespace OHOS

#endif
