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
#include "skia_adapter/skia_shader_effect.h"
namespace OHOS {
namespace Rosen {
namespace Drawing {
ShaderEffect::ShaderEffect(ShaderEffectType t, ColorQuad color) noexcept : ShaderEffect()
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
    const SamplingOptions& sampling, const Matrix& matrix) noexcept
    : ShaderEffect()
{
    type_ = t;
    impl_->InitWithImage(image, tileX, tileY, sampling, matrix);
}

ShaderEffect::ShaderEffect(ShaderEffectType t, const Picture& picture, TileMode tileX, TileMode tileY, FilterMode mode,
    const Matrix& matrix, const Rect& rect) noexcept
    : ShaderEffect()
{
    type_ = t;
    impl_->InitWithPicture(picture, tileX, tileY, mode, matrix, rect);
}

ShaderEffect::ShaderEffect(ShaderEffectType t, const Point& startPt, const Point& endPt,
    const std::vector<ColorQuad>& colors, const std::vector<scalar>& pos, TileMode mode) noexcept
    : ShaderEffect()
{
    type_ = t;
    impl_->InitWithLinearGradient(startPt, endPt, colors, pos, mode);
}

ShaderEffect::ShaderEffect(ShaderEffectType t, const Point& centerPt, scalar radius,
    const std::vector<ColorQuad>& colors, const std::vector<scalar>& pos, TileMode mode) noexcept
    : ShaderEffect()
{
    type_ = t;
    impl_->InitWithRadialGradient(centerPt, radius, colors, pos, mode);
}

ShaderEffect::ShaderEffect(ShaderEffectType t, const Point& startPt, scalar startRadius, const Point& endPt,
    scalar endRadius, const std::vector<ColorQuad>& colors, const std::vector<scalar>& pos, TileMode mode,
    const Matrix *matrix) noexcept
    : ShaderEffect()
{
    type_ = t;
    impl_->InitWithTwoPointConical(startPt, startRadius, endPt, endRadius, colors, pos, mode, matrix);
}

ShaderEffect::ShaderEffect(ShaderEffectType t, const Point& centerPt, const std::vector<ColorQuad>& colors,
    const std::vector<scalar>& pos, TileMode mode, scalar startAngle, scalar endAngle, const Matrix *matrix) noexcept
    : ShaderEffect()
{
    type_ = t;
    impl_->InitWithSweepGradient(centerPt, colors, pos, mode, startAngle, endAngle, matrix);
}

ShaderEffect::ShaderEffect(ShaderEffectType t, const float& lightUpDeg, ShaderEffect& imageShader) noexcept
    : ShaderEffect()
{
    type_ = t;
    impl_->InitWithLightUp(lightUpDeg, imageShader);
}

ShaderEffect::ShaderEffect() noexcept
    : type_(ShaderEffect::ShaderEffectType::NO_TYPE), impl_(ImplFactory::CreateShaderEffectImpl())
{}

ShaderEffect::ShaderEffect(ShaderEffectType t) noexcept
    : type_(t), impl_(ImplFactory::CreateShaderEffectImpl())
{}

ShaderEffect::ShaderEffectType ShaderEffect::GetType() const
{
    return type_;
}

std::shared_ptr<ShaderEffect> ShaderEffect::CreateColorShader(ColorQuad color)
{
    return std::make_shared<ShaderEffect>(ShaderEffect::ShaderEffectType::COLOR_EFFECT, color);
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
    const std::vector<ColorQuad>& colors, const std::vector<scalar>& pos, TileMode mode)
{
    return std::make_shared<ShaderEffect>(
        ShaderEffect::ShaderEffectType::LINEAR_GRADIENT, startPt, endPt, colors, pos, mode);
}

std::shared_ptr<ShaderEffect> ShaderEffect::CreateRadialGradient(const Point& centerPt, scalar radius,
    const std::vector<ColorQuad>& colors, const std::vector<scalar>& pos, TileMode mode)
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

std::shared_ptr<Data> ShaderEffect::Serialize() const
{
    return impl_->Serialize();
}

bool ShaderEffect::Deserialize(std::shared_ptr<Data> data)
{
    return impl_->Deserialize(data);
}

const sk_sp<SkShader> ShaderEffect::ExportSkShader()
{
    return GetImpl<SkiaShaderEffect>()->GetShader();
}

void ShaderEffect::SetSkShader(sk_sp<SkShader> shader)
{
    GetImpl<SkiaShaderEffect>()->SetSkShader(shader);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
