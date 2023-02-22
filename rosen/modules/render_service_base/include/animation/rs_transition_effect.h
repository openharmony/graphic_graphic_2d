/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_TRANSITION_EFFECT_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_TRANSITION_EFFECT_H

#include <memory>
#include <vector>

#include "common/rs_macros.h"
#include "common/rs_vector3.h"
#include "common/rs_vector4.h"

namespace OHOS {
namespace Rosen {
class RSRenderTransitionEffect;

class RSB_EXPORT RSTransitionEffect final : public std::enable_shared_from_this<RSTransitionEffect> {
public:
    static const std::shared_ptr<const RSTransitionEffect> OPACITY;

    static const std::shared_ptr<const RSTransitionEffect> SCALE;

    static const std::shared_ptr<const RSTransitionEffect> EMPTY;

    static std::shared_ptr<RSTransitionEffect> Create();

    static std::shared_ptr<RSTransitionEffect> Asymmetric(const std::shared_ptr<RSTransitionEffect>& transitionIn,
        const std::shared_ptr<RSTransitionEffect>& transitionOut);

    std::shared_ptr<RSTransitionEffect> Opacity(float opacity = 0.0f);
    std::shared_ptr<RSTransitionEffect> Scale(const Vector3f& scale);
    std::shared_ptr<RSTransitionEffect> Translate(const Vector3f& translate);
    std::shared_ptr<RSTransitionEffect> Rotate(const Vector4f& axisAngle);

private:
    RSTransitionEffect() = default;

    RSTransitionEffect(const std::shared_ptr<RSTransitionEffect>& transitionIn,
        const std::shared_ptr<RSTransitionEffect>& transitionOut);

    const std::vector<std::shared_ptr<RSRenderTransitionEffect>>& GetTransitionInEffects() const;
    const std::vector<std::shared_ptr<RSRenderTransitionEffect>>& GetTransitionOutEffects() const;

    std::vector<std::shared_ptr<RSRenderTransitionEffect>> transitionInEffects_;
    std::vector<std::shared_ptr<RSRenderTransitionEffect>> transitionOutEffects_;

    friend class RSRenderTransition;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_TRANSITION_EFFECT_H
