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

#include "effect/image_filter.h"

#include "impl_factory.h"

#include "impl_interface/image_filter_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
ImageFilter::ImageFilter(FilterType t, scalar x, scalar y,
    std::shared_ptr<ImageFilter> input, const Rect& cropRect) noexcept : ImageFilter()
{
    type_ = t;
    impl_->InitWithOffset(x, y, input, cropRect);
}

ImageFilter::ImageFilter(FilterType t, scalar x, scalar y, TileMode mode, std::shared_ptr<ImageFilter> input,
    ImageBlurType blurType, const Rect& cropRect) noexcept
    : ImageFilter()
{
    type_ = t;
    impl_->InitWithBlur(x, y, mode, input, blurType, cropRect);
}

ImageFilter::ImageFilter(FilterType t, const ColorFilter& cf,
    std::shared_ptr<ImageFilter> input, const Rect& cropRect) noexcept
    : ImageFilter()
{
    type_ = t;
    impl_->InitWithColor(cf, input, cropRect);
}

ImageFilter::ImageFilter(FilterType t, const ColorFilter& cf, scalar x, scalar y,
    ImageBlurType blurType, const Rect& cropRect) noexcept
    : ImageFilter()
{
    type_ = t;
    impl_->InitWithColorBlur(cf, x, y, blurType, cropRect);
}

void ImageFilter::InitWithColorBlur(const ColorFilter& cf, scalar x, scalar y,
    ImageBlurType blurType, const Rect& cropRect)
{
    type_ = ImageFilter::FilterType::COLOR_FILTER;
    impl_->InitWithColorBlur(cf, x, y, blurType, cropRect);
}

ImageFilter::ImageFilter(FilterType t, const std::vector<scalar>& coefficients, bool enforcePMColor,
    std::shared_ptr<ImageFilter> background, std::shared_ptr<ImageFilter> foreground, const Rect& cropRect) noexcept
    :ImageFilter()
{
    type_ = t;
    impl_->InitWithArithmetic(coefficients, enforcePMColor, background, foreground, cropRect);
}

ImageFilter::ImageFilter(FilterType t, std::shared_ptr<ImageFilter> f1, std::shared_ptr<ImageFilter> f2) noexcept
    : ImageFilter()
{
    type_ = t;
    impl_->InitWithCompose(f1, f2);
}

ImageFilter::ImageFilter(FilterType t, float radius, const std::vector<std::pair<float, float>>& fractionStops,
    GradientDir direction, GradientBlurType blurType, std::shared_ptr<ImageFilter> input) noexcept
    : ImageFilter()
{
    type_ = t;
    impl_->InitWithGradientBlur(radius, fractionStops, direction, blurType, input);
}

ImageFilter::ImageFilter(FilterType t, const std::shared_ptr<Image>& image, const Rect& srcRect, const Rect& dstRect,
    const SamplingOptions& options) noexcept : ImageFilter()
{
    type_ = t;
    impl_->InitWithImage(image, srcRect, dstRect, options);
}

ImageFilter::ImageFilter(FilterType t) noexcept
    : type_(t), impl_(ImplFactory::CreateImageFilterImpl())
{}

ImageFilter::ImageFilter() noexcept
    : type_(ImageFilter::FilterType::NO_TYPE), impl_(ImplFactory::CreateImageFilterImpl())
{}

ImageFilter::FilterType ImageFilter::GetType() const
{
    return type_;
}

std::shared_ptr<ImageFilter> ImageFilter::CreateImageImageFilter(
    const std::shared_ptr<Image>& image, const Rect& srcRect, const Rect& dstRect, const SamplingOptions& options)
{
    return std::make_shared<ImageFilter>(ImageFilter::FilterType::IMAGE, image, srcRect, dstRect, options);
}

std::shared_ptr<ImageFilter> ImageFilter::CreateBlurImageFilter(scalar sigmaX, scalar sigmaY, TileMode mode,
    std::shared_ptr<ImageFilter> input, ImageBlurType blurType, const Rect& cropRect)
{
    return std::make_shared<ImageFilter>(ImageFilter::FilterType::BLUR, sigmaX, sigmaY,
        mode, input, blurType, cropRect);
}

std::shared_ptr<ImageFilter> ImageFilter::CreateColorFilterImageFilter(
    const ColorFilter& cf, std::shared_ptr<ImageFilter> input, const Rect& cropRect)
{
    return std::make_shared<ImageFilter>(ImageFilter::FilterType::COLOR_FILTER, cf, input, cropRect);
}

std::shared_ptr<ImageFilter> ImageFilter::CreateColorBlurImageFilter(const ColorFilter& cf,
    scalar sigmaX, scalar sigmaY, ImageBlurType blurType, const Rect& cropRect)
{
    return std::make_shared<ImageFilter>(ImageFilter::FilterType::COLOR_FILTER, cf,
        sigmaX, sigmaY, blurType, cropRect);
}

std::shared_ptr<ImageFilter> ImageFilter::CreateOffsetImageFilter(
    scalar dx, scalar dy, std::shared_ptr<ImageFilter> input, const Rect& cropRect)
{
    return std::make_shared<ImageFilter>(ImageFilter::FilterType::OFFSET,
        dx, dy, input, cropRect);
}

std::shared_ptr<ImageFilter> ImageFilter::CreateGradientBlurImageFilter(float radius,
    const std::vector<std::pair<float, float>>& fractionStops, GradientDir direction,
    GradientBlurType blurType, std::shared_ptr<ImageFilter> input)
{
    return std::make_shared<ImageFilter>(
        ImageFilter::FilterType::GRADIENT_BLUR, radius, fractionStops, direction, blurType, input);
}

std::shared_ptr<ImageFilter> ImageFilter::CreateArithmeticImageFilter(const std::vector<scalar>& coefficients,
    bool enforcePMColor, std::shared_ptr<ImageFilter> background,
    std::shared_ptr<ImageFilter> foreground, const Rect& cropRect)
{
    return std::make_shared<ImageFilter>(
        ImageFilter::FilterType::ARITHMETIC, coefficients, enforcePMColor, background, foreground, cropRect);
}

std::shared_ptr<ImageFilter> ImageFilter::CreateComposeImageFilter(
    std::shared_ptr<ImageFilter> f1, std::shared_ptr<ImageFilter> f2)
{
    return std::make_shared<ImageFilter>(ImageFilter::FilterType::COMPOSE, f1, f2);
}

std::shared_ptr<Data> ImageFilter::Serialize() const
{
    return impl_->Serialize();
}

bool ImageFilter::Deserialize(std::shared_ptr<Data> data)
{
    return impl_->Deserialize(data);
}

std::shared_ptr<ImageFilter> ImageFilter::CreateBlendImageFilter(BlendMode mode,
    std::shared_ptr<ImageFilter> background, std::shared_ptr<ImageFilter> foreground, const Rect& cropRect)
{
    return std::make_shared<ImageFilter>(ImageFilter::FilterType::BLEND, mode, background, foreground, cropRect);
}

ImageFilter::ImageFilter(FilterType t, BlendMode mode, std::shared_ptr<ImageFilter> background,
    std::shared_ptr<ImageFilter> foreground, const Rect& cropRect) noexcept
    : ImageFilter()
{
    type_ = t;
    impl_->InitWithBlend(mode, cropRect, background, foreground);
}

std::shared_ptr<ImageFilter> ImageFilter::CreateShaderImageFilter(
    std::shared_ptr<ShaderEffect> shader, const Rect& cropRect)
{
    return std::make_shared<ImageFilter>(ImageFilter::FilterType::SHADER, shader, cropRect);
}

ImageFilter::ImageFilter(FilterType t, std::shared_ptr<ShaderEffect> shader, const Rect& cropRect) noexcept
    : ImageFilter()
{
    type_ = t;
    impl_->InitWithShader(shader, cropRect);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS