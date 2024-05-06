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

namespace OHOS {
namespace Rosen {
namespace Drawing {
/* ColorShader */
ShaderEffect::ShaderEffect(ShaderEffectType t, ColorQuad color) noexcept : ShaderEffect(t)
{
    impl_->InitWithColor(color);
}

/* ColorShader With ColorSpace */
ShaderEffect::ShaderEffect(ShaderEffectType t, const Color4f& color, std::shared_ptr<ColorSpace> colorSpace) noexcept
    : ShaderEffect(t)
{
    impl_->InitWithColorSpace(color, colorSpace);
}

/* BlendShader */
ShaderEffect::ShaderEffect(ShaderEffectType t, ShaderEffect& dst, ShaderEffect& src, BlendMode mode) noexcept
    : ShaderEffect(t)
{
    impl_->InitWithBlend(dst, src, mode);
}

/* ImageShader */
ShaderEffect::ShaderEffect(ShaderEffectType t, const Image& image, TileMode tileX, TileMode tileY,
    const SamplingOptions& sampling, const Matrix& matrix) noexcept
    : ShaderEffect(t)
{
    impl_->InitWithImage(image, tileX, tileY, sampling, matrix);
}

/* PictureShader */
ShaderEffect::ShaderEffect(ShaderEffectType t, const Picture& picture, TileMode tileX, TileMode tileY, FilterMode mode,
    const Matrix& matrix, const Rect& rect) noexcept
    : ShaderEffect(t)
{
    impl_->InitWithPicture(picture, tileX, tileY, mode, matrix, rect);
}

/* LinearGradient */
ShaderEffect::ShaderEffect(ShaderEffectType t, const Point& startPt, const Point& endPt,
    const std::vector<ColorQuad>& colors, const std::vector<scalar>& pos, TileMode mode, const Matrix *matrix) noexcept
    : ShaderEffect(t)
{
    impl_->InitWithLinearGradient(startPt, endPt, colors, pos, mode, matrix);
}

/* RadialGradient */
ShaderEffect::ShaderEffect(ShaderEffectType t, const Point& centerPt, scalar radius,
    const std::vector<ColorQuad>& colors, const std::vector<scalar>& pos, TileMode mode, const Matrix *matrix) noexcept
    : ShaderEffect(t)
{
    impl_->InitWithRadialGradient(centerPt, radius, colors, pos, mode, matrix);
}

/* TwoPointConicalGradient */
ShaderEffect::ShaderEffect(ShaderEffectType t, const Point& startPt, scalar startRadius, const Point& endPt,
    scalar endRadius, const std::vector<ColorQuad>& colors, const std::vector<scalar>& pos, TileMode mode,
    const Matrix *matrix) noexcept
    : ShaderEffect(t)
{
    impl_->InitWithTwoPointConical(startPt, startRadius, endPt, endRadius, colors, pos, mode, matrix);
}

/* SweepGradient */
ShaderEffect::ShaderEffect(ShaderEffectType t, const Point& centerPt, const std::vector<ColorQuad>& colors,
    const std::vector<scalar>& pos, TileMode mode, scalar startAngle, scalar endAngle, const Matrix *matrix) noexcept
    : ShaderEffect(t)
{
    impl_->InitWithSweepGradient(centerPt, colors, pos, mode, startAngle, endAngle, matrix);
}

/* LightUpShader */
ShaderEffect::ShaderEffect(ShaderEffectType t, const float& lightUpDeg, ShaderEffect& imageShader) noexcept
    : ShaderEffect(t)
{
    impl_->InitWithLightUp(lightUpDeg, imageShader);
}

/* ExtendShader */
ShaderEffect::ShaderEffect(ShaderEffectType t, std::shared_ptr<ExtendObject> object) noexcept
    : type_(t), object_(object) {}

ShaderEffect::ShaderEffect() noexcept
    : type_(ShaderEffect::ShaderEffectType::NO_TYPE), impl_(ImplFactory::CreateShaderEffectImpl())
{}

ShaderEffect::ShaderEffect(ShaderEffectType t) noexcept : type_(t), impl_(ImplFactory::CreateShaderEffectImpl()) {}

ShaderEffect::ShaderEffectType ShaderEffect::GetType() const
{
    return type_;
}

std::shared_ptr<ShaderEffect> ShaderEffect::CreateColorShader(ColorQuad color)
{
    return std::make_shared<ShaderEffect>(ShaderEffect::ShaderEffectType::COLOR_SHADER, color);
}

std::shared_ptr<ShaderEffect> ShaderEffect::CreateColorSpaceShader(const Color4f& color,
    std::shared_ptr<ColorSpace> colorSpace)
{
    return std::make_shared<ShaderEffect>(ShaderEffect::ShaderEffectType::COLOR_SHADER, color, colorSpace);
}

std::shared_ptr<ShaderEffect> ShaderEffect::CreateBlendShader(ShaderEffect& dst, ShaderEffect& src, BlendMode mode)
{
    return std::make_shared<ShaderEffect>(ShaderEffect::ShaderEffectType::BLEND, dst, src, mode);
}

std::shared_ptr<ShaderEffect> ShaderEffect::CreateImageShader(
    const Image& image, TileMode tileX, TileMode tileY, const SamplingOptions& sampling, const Matrix& matrix)
{
    return std::make_shared<ShaderEffect>(ShaderEffect::ShaderEffectType::IMAGE, image, tileX, tileY, sampling, matrix);
}

std::shared_ptr<ShaderEffect> ShaderEffect::CreatePictureShader(
    const Picture& picture, TileMode tileX, TileMode tileY, FilterMode mode, const Matrix& matrix, const Rect& rect)
{
    return std::make_shared<ShaderEffect>(
        ShaderEffect::ShaderEffectType::PICTURE, picture, tileX, tileY, mode, matrix, rect);
}

std::shared_ptr<ShaderEffect> ShaderEffect::CreateLinearGradient(const Point& startPt, const Point& endPt,
    const std::vector<ColorQuad>& colors, const std::vector<scalar>& pos, TileMode mode, const Matrix *matrix)
{
    return std::make_shared<ShaderEffect>(
        ShaderEffect::ShaderEffectType::LINEAR_GRADIENT, startPt, endPt, colors, pos, mode);
}

std::shared_ptr<ShaderEffect> ShaderEffect::CreateRadialGradient(const Point& centerPt, scalar radius,
    const std::vector<ColorQuad>& colors, const std::vector<scalar>& pos, TileMode mode, const Matrix *matrix)
{
    return std::make_shared<ShaderEffect>(
        ShaderEffect::ShaderEffectType::RADIAL_GRADIENT, centerPt, radius, colors, pos, mode);
}

std::shared_ptr<ShaderEffect> ShaderEffect::CreateTwoPointConical(const Point& startPt, scalar startRadius,
    const Point& endPt, scalar endRadius, const std::vector<ColorQuad>& colors, const std::vector<scalar>& pos,
    TileMode mode, const Matrix *matrix)
{
    return std::make_shared<ShaderEffect>(
        ShaderEffect::ShaderEffectType::CONICAL_GRADIENT, startPt, startRadius, endPt, endRadius, colors, pos, mode,
        matrix);
}

std::shared_ptr<ShaderEffect> ShaderEffect::CreateSweepGradient(const Point& centerPt,
    const std::vector<ColorQuad>& colors, const std::vector<scalar>& pos, TileMode mode, scalar startAngle,
    scalar endAngle, const Matrix *matrix)
{
    return std::make_shared<ShaderEffect>(
        ShaderEffect::ShaderEffectType::SWEEP_GRADIENT, centerPt, colors, pos, mode, startAngle, endAngle, matrix);
}

std::shared_ptr<ShaderEffect> ShaderEffect::CreateLightUp(const float& lightUpDeg, ShaderEffect& imageShader)
{
    return std::make_shared<ShaderEffect>(ShaderEffect::ShaderEffectType::LIGHT_UP, lightUpDeg, imageShader);
}

std::shared_ptr<ShaderEffect> ShaderEffect::CreateExtendShader(std::shared_ptr<ExtendObject> object)
{
    return std::make_shared<ShaderEffect>(ShaderEffect::ShaderEffectType::EXTEND_SHADER, object);
}

std::shared_ptr<Data> ShaderEffect::Serialize() const
{
    return impl_->Serialize();
}

bool ShaderEffect::Deserialize(std::shared_ptr<Data> data)
{
    return impl_->Deserialize(data);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
