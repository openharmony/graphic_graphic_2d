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

#include "animation/rs_transition_effect.h"

#include "animation/rs_render_transition_effect.h"

namespace OHOS {
namespace Rosen {
const RSTransitionEffect RSTransitionEffect::OPACITY = RSTransitionEffect::Create().Opacity(0);

const RSTransitionEffect RSTransitionEffect::SCALE =
    RSTransitionEffect::Create().Scale({ 0.f, 0.f, 0.f }, { 0.5f, 0.5f });

const RSTransitionEffect RSTransitionEffect::EMPTY = RSTransitionEffect::Create();

RSTransitionEffect RSTransitionEffect::Create()
{
    return RSTransitionEffect();
}

RSTransitionEffect& RSTransitionEffect::Opacity(float opacity)
{
    auto opacityEffect = std::make_shared<RSTransitionFade>(opacity);
    transitionIn_.push_back(opacityEffect);
    transitionOut_.push_back(opacityEffect);
    return *this;
}

RSTransitionEffect& RSTransitionEffect::Scale(Vector3f scale, Vector2f pivot)
{
    auto scaleEffect = std::make_shared<RSTransitionScale>(scale.x_, scale.y_, scale.z_, pivot.x_, pivot.y_);
    transitionIn_.push_back(scaleEffect);
    transitionOut_.push_back(scaleEffect);
    return *this;
}

RSTransitionEffect& RSTransitionEffect::Translate(Vector3f translate)
{
    auto translateEffect = std::make_shared<RSTransitionTranslate>(translate.x_, translate.y_, translate.z_);
    transitionIn_.push_back(translateEffect);
    transitionOut_.push_back(translateEffect);
    return *this;
}

RSTransitionEffect& RSTransitionEffect::Rotate(Vector4f axisAngle, Vector2f pivot)
{
    auto rotateEffect = std::make_shared<RSTransitionRotate>(axisAngle.x_, axisAngle.y_, axisAngle.z_, axisAngle.w_, pivot.x_, pivot.y_);
    transitionIn_.push_back(rotateEffect);
    transitionOut_.push_back(rotateEffect);
    return *this;
}

RSTransitionEffect RSTransitionEffect::Asymmetric(
    const RSTransitionEffect& transitionIn, const RSTransitionEffect& transitionOut)
{
    return RSTransitionEffect(transitionIn, transitionOut);
}

RSTransitionEffect::RSTransitionEffect(const RSTransitionEffect& transitionIn, const RSTransitionEffect& transitionOut)
    : transitionIn_(transitionIn.GetTransitionInEffect()), transitionOut_(transitionOut.GetTransitionOutEffect())
{}

const std::vector<std::shared_ptr<RSRenderTransitionEffect>>& RSTransitionEffect::GetTransitionInEffect() const
{
    return transitionIn_;
}

const std::vector<std::shared_ptr<RSRenderTransitionEffect>>& RSTransitionEffect::GetTransitionOutEffect() const
{
    return transitionOut_;
}

bool RSTransitionEffect::Empty() const
{
    return transitionIn_.empty() && transitionOut_.empty();
}
} // namespace Rosen
} // namespace OHOS
