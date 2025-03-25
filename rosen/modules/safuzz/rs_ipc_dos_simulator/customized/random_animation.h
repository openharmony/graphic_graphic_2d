/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef SAFUZZ_RANDOM_ANIMATION_H
#define SAFUZZ_RANDOM_ANIMATION_H

#include "animation/rs_interpolator.h"
#include "animation/rs_cubic_bezier_interpolator.h"
#include "animation/rs_spring_interpolator.h"
#include "animation/rs_steps_interpolator.h"
#include "command/rs_animation_command.h"

namespace OHOS {
namespace Rosen {
class RandomAnimation {
public:
    static std::shared_ptr<RSSyncTask> GetRandomRSSyncTask();
    static std::shared_ptr<RSRenderCurveAnimation> GetRandomRSRenderCurveAnimation();
    static std::shared_ptr<RSRenderParticleAnimation> GetRandomRSRenderParticleAnimation();
    static std::shared_ptr<RSRenderKeyframeAnimation> GetRandomRSRenderKeyframeAnimation();
    static std::shared_ptr<RSRenderPathAnimation> GetRandomRSRenderPathAnimation();
    static std::shared_ptr<RSRenderTransition> GetRandomRSRenderTransition();
    static std::shared_ptr<RSRenderSpringAnimation> GetRandomRSRenderSpringAnimation();
    static std::shared_ptr<RSRenderInterpolatingSpringAnimation> GetRandomRSRenderInterpolatingSpringAnimation();
};
} // namespace Rosen
} // namespace OHOS

#endif // SAFUZZ_RANDOM_ANIMATION_H
