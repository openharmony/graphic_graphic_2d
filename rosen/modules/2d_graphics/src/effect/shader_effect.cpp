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

#include "effect/shader_effect.h"

#include "impl_factory.h"
#include "impl_interface/mask_filter_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
ShaderEffect::ShaderEffect(ShaderEffectType t, ColorQuad color ) noexcept : ShaderEffect()
{
    type_ = t;
    impl_->InitWithColor(color);
}

ShaderEffect::ShaderEffect(ShaderEffectType t, ShaderEffect& dst, ShaderEffect& src, BlendMode mode) noexcept
    : ShaderEffect()
{
    type_ = t;
    impl_->InitWithBlend(dst, src, mode);
}

ShaderEffect::ShaderEffect(ShaderEffectType t, const Image& image, TileMode tileX, TileMode tileY,
    const SamplingOptions& sampling, const Matrix& matrix) noexcept : ShaderEffect()
{
    type_ = t;
    impl_->InitWithImage(image, tileX, tileY, sampling, matrix);
}

ShaderEffect::ShaderEffect(ShaderEffectType t, const Point& startPt, const Point& endPt,
    const std::vector<ColorQuad>& colors, const std::vector<scalar>& pos, TileMode mode) noexcept : ShaderEffect()
{
    type_ = t;
    impl_->InitWithLinearGradient(startPt, endPt, colors, pos, mode);
}

ShaderEffect::ShaderEffect(ShaderEffectType t, const Point& centerPt, scalar radius,
    const std::vector<ColorQuad>& colors, const std::vector<scalar>& pos, TileMode mode) noexcept : ShaderEffect()
{
    type_ = t;
    impl_->InitWithRadialGradient(centerPt, radius, colors, pos, mode);
}

ShaderEffect::ShaderEffect(ShaderEffectType t, const Point& startPt, scalar startRadius,
    const Point& endPt, scalar endRadius,
    const std::vector<ColorQuad>& colors, const std::vector<scalar>& pos, TileMode mode) noexcept
    : ShaderEffect()
{
    type_ = t;
    impl_->InitWithTwoPointConical(startPt, startRadius, endPt, endRadius, colors, pos, mode);
}

ShaderEffect::ShaderEffect(ShaderEffectType t, const Point& centerPt,
    const std::vector<ColorQuad>& colors, const std::vector<scalar>& pos, TileMode mode,
    scalar startAngle, scalar endAngle) noexcept : ShaderEffect()
{
    type_ = t;
    impl_->InitWithSweepGradient(centerPt, colors, pos, mode, startAngle, endAngle);
}

ShaderEffect::ShaderEffect() noexcept
    : type_(ShaderEffect::ShaderEffectType::NO_TYPE),
    impl_(ImplFactory::CreateShaderEffectImpl()) {}

ShaderEffect::ShaderEffectType ShaderEffect::GetType() const
{
    return type_;
}

std::shared_ptr<ShaderEffect> ShaderEffect::CreateColorShader(ColorQuad color)
{
    return std::make_shared<ShaderEffect>(ShaderEffect::ShaderEffectType::COLOR, color);
}

std::shared_ptr<ShaderEffect> ShaderEffect::CreateBlendShader(ShaderEffect& dst, ShaderEffect& src, BlendMode mode)
{
    return std::make_shared<ShaderEffect>(ShaderEffect::ShaderEffectType::BLEND, dst, src, mode);
}

std::shared_ptr<ShaderEffect> ShaderEffect::CreateImageShader(const Image& image, TileMode tileX, TileMode tileY,
    const SamplingOptions& sampling, const Matrix& matrix)
{
    return std::make_shared<ShaderEffect>(ShaderEffect::ShaderEffectType::IMAGE, image, tileX, tileY, sampling, matrix);
}

std::shared_ptr<ShaderEffect> ShaderEffect::CreateLinearGradient(const Point& startPt, const Point& endPt,
    const std::vector<ColorQuad>& colors, const std::vector<scalar>& pos, TileMode mode)
{
    return std::make_shared<ShaderEffect>(ShaderEffect::ShaderEffectType::LINEAR_GRADIENT, startPt, endPt,
        colors, pos, mode);
}

std::shared_ptr<ShaderEffect> ShaderEffect::CreateRadialGradient(const Point& centerPt, scalar radius,
    const std::vector<ColorQuad>& colors, const std::vector<scalar>& pos, TileMode mode)
{
    return std::make_shared<ShaderEffect>(ShaderEffect::ShaderEffectType::RADIAL_GRADIENT, centerPt, radius,
        colors, pos, mode);
}

std::shared_ptr<ShaderEffect> ShaderEffect::CreateTwoPointConical(const Point& startPt, scalar startRadius,
    const Point& endPt, scalar endRadius,
    const std::vector<ColorQuad>& colors, const std::vector<scalar>& pos, TileMode mode)
{
    return std::make_shared<ShaderEffect>(ShaderEffect::ShaderEffectType::CONICAL_GRADIENT, startPt, startRadius,
        endPt, endRadius, colors, pos, mode);
}

std::shared_ptr<ShaderEffect> ShaderEffect::CreateSweepGradient(const Point& centerPt,
    const std::vector<ColorQuad>& colors, const std::vector<scalar>& pos, TileMode mode,
    scalar startAngle, scalar endAngle)
{
    return std::make_shared<ShaderEffect>(ShaderEffect::ShaderEffectType::SWEEP_GRADIENT, centerPt,
        colors, pos, mode, startAngle, endAngle);
}
}
}
}
