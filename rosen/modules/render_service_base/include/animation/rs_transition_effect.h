/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "common/rs_vector2.h"
#include "common/rs_vector3.h"
#include "common/rs_vector4.h"

namespace OHOS {
namespace Rosen {
class RSRenderTransitionEffect;

class RS_EXPORT RSTransitionEffect final {
public:
    static const RSTransitionEffect OPACITY;

    static const RSTransitionEffect SCALE;

    static const RSTransitionEffect EMPTY;

    static RSTransitionEffect Create();

    static RSTransitionEffect Asymmetric(
        const RSTransitionEffect& transitionIn, const RSTransitionEffect& transitionOut);

    RSTransitionEffect& Opacity(float opacity = 0.0f);

    RSTransitionEffect& Scale(Vector3f scale, Vector2f pivot = { 0.5f, 0.5f });

    RSTransitionEffect& Translate(Vector3f translate);

    RSTransitionEffect& Rotate(Vector4f axisAngle, Vector2f pivot = { 0.5f, 0.5f });

    bool Empty() const;

    int GetSize() const
    {
        return transitionIn_.size();
    }

private:
    RSTransitionEffect() = default;

    RSTransitionEffect(const RSTransitionEffect& transitionIn, const RSTransitionEffect& transitionOut);

    const std::vector<std::shared_ptr<RSRenderTransitionEffect>>& GetTransitionInEffect() const;
    const std::vector<std::shared_ptr<RSRenderTransitionEffect>>& GetTransitionOutEffect() const;

    std::vector<std::shared_ptr<RSRenderTransitionEffect>> transitionIn_;
    std::vector<std::shared_ptr<RSRenderTransitionEffect>> transitionOut_;

    friend class RSRenderTransition;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_TRANSITION_EFFECT_H