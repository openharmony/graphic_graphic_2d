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
#include "ui_effect/property/include/rs_ui_dot_matrix_shader.h"
#include "platform/common/rs_log.h"
#include "ui_effect/property/include/rs_ui_shader_base.h"

namespace OHOS {
namespace Rosen {

namespace {
// Effect type values for DotMatrixShaderEffectTypeTag
constexpr int DOT_MATRIX_EFFECT_TYPE_NORMAL = 0;
constexpr int DOT_MATRIX_EFFECT_TYPE_ROTATE = 1;
constexpr int DOT_MATRIX_EFFECT_TYPE_RIPPLE = 2;
} // namespace

RSNGDotMatrixShaderAdapter::RSNGDotMatrixShaderAdapter()
{
    shader_ = std::make_shared<Rosen::RSNGDotMatrixShader>();
}

RSNGDotMatrixShaderAdapter::~RSNGDotMatrixShaderAdapter(){};

void RSNGDotMatrixShaderAdapter::SetNormalParams(Rosen::Drawing::Color dotColor, float dotSpacing, float dotRadius)
{
    auto dotShader = std::static_pointer_cast<Rosen::RSNGDotMatrixShader>(shader_);
    Vector4f localDotColor = { dotColor.GetRedF(), dotColor.GetGreenF(), dotColor.GetBlueF(), dotColor.GetAlphaF() };
    dotShader->Setter<Rosen::DotMatrixShaderDotColorTag>(localDotColor);
    dotShader->Setter<Rosen::DotMatrixShaderDotSpacingTag>(dotSpacing);
    dotShader->Setter<Rosen::DotMatrixShaderDotRadiusTag>(dotRadius);
    dotShader->Setter<Rosen::DotMatrixShaderEffectTypeTag>(DOT_MATRIX_EFFECT_TYPE_NORMAL);
}

void RSNGDotMatrixShaderAdapter::SetRippleEffect(std::vector<Rosen::Drawing::Color> effectColors,
    Vector2f colorFractions, std::vector<Rosen::Drawing::Point> startPoints, float pathWidth, bool inverseEffect)
{
    std::vector<Vector4f> effectColorsVec;
    for (uint32_t i = 0; i < effectColors.size(); i++) {
        Vector4f color = { effectColors[i].GetRedF(), effectColors[i].GetGreenF(),
            effectColors[i].GetBlueF(), effectColors[i].GetAlphaF() };
        effectColorsVec.emplace_back(color);
    }
    std::vector<Vector2f> startPointsVec;
    for (uint32_t i = 0; i < startPoints.size(); i++) {
        Vector2f point = { startPoints[i].GetX(), startPoints[i].GetY() };
        startPointsVec.emplace_back(point);
    }
    auto dotShader = std::static_pointer_cast<Rosen::RSNGDotMatrixShader>(shader_);
    dotShader->Setter<Rosen::DotMatrixShaderEffectColorsTag>(effectColorsVec);
    dotShader->Setter<Rosen::DotMatrixShaderColorFractionsTag>(colorFractions);
    dotShader->Setter<Rosen::DotMatrixShaderStartPointsTag>(startPointsVec);
    dotShader->Setter<Rosen::DotMatrixShaderPathWidthTag>(pathWidth);
    dotShader->Setter<Rosen::DotMatrixShaderInverseEffectTag>(inverseEffect);
    dotShader->Setter<Rosen::DotMatrixShaderEffectTypeTag>(DOT_MATRIX_EFFECT_TYPE_RIPPLE);
}

void RSNGDotMatrixShaderAdapter::SetRotateEffect(int pathDirection,
    std::vector<Rosen::Drawing::Color> effectColors)
{
    std::vector<Vector4f> effectColorsVec;
    for (uint32_t i = 0; i < effectColors.size(); i++) {
        Vector4f color = { effectColors[i].GetRedF(), effectColors[i].GetGreenF(),
            effectColors[i].GetBlueF(), effectColors[i].GetAlphaF() };
        effectColorsVec.emplace_back(color);
    }
    auto dotShader = std::static_pointer_cast<Rosen::RSNGDotMatrixShader>(shader_);
    dotShader->Setter<Rosen::DotMatrixShaderPathDirectionTag>(pathDirection);
    dotShader->Setter<Rosen::DotMatrixShaderEffectColorsTag>(effectColorsVec);
    dotShader->Setter<Rosen::DotMatrixShaderEffectTypeTag>(DOT_MATRIX_EFFECT_TYPE_ROTATE);
}

void RSNGDotMatrixShaderAdapter::SetProgress(float progress)
{
    auto dotShader = std::static_pointer_cast<Rosen::RSNGDotMatrixShader>(shader_);
    dotShader->Setter<Rosen::DotMatrixShaderProgressTag>(progress);
}

std::shared_ptr<RSAnimatableProperty<float>> RSNGDotMatrixShaderAdapter::GetProgress()
{
    auto dotShader = std::static_pointer_cast<Rosen::RSNGDotMatrixShader>(shader_);
    return dotShader->Getter<Rosen::DotMatrixShaderProgressTag>();
}

std::shared_ptr<Rosen::RSNGShaderBase> RSNGDotMatrixShaderAdapter::GetNGShaderInstance()
{
    return shader_;
}

} // namespace Rosen
} // namespace OHOS
