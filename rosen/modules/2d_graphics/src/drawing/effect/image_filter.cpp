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
ImageFilter::ImageFilter(FilterType t, scalar x, scalar y, std::shared_ptr<ImageFilter> input) noexcept : ImageFilter()
{
    type_ = t;
    impl_->InitWithOffset(x, y, input);
}

ImageFilter::ImageFilter(FilterType t, scalar x, scalar y, TileMode mode, std::shared_ptr<ImageFilter> input) noexcept
    : ImageFilter()
{
    type_ = t;
    impl_->InitWithBlur(x, y, mode, input);
}

ImageFilter::ImageFilter(FilterType t, const ColorFilter& cf, std::shared_ptr<ImageFilter> input) noexcept
    : ImageFilter()
{
    type_ = t;
    impl_->InitWithColor(cf, input);
}

ImageFilter::ImageFilter(FilterType t, const ColorFilter& cf, scalar x, scalar y) noexcept
    : ImageFilter()
{
    type_ = t;
    impl_->InitWithColorBlur(cf, x, y);
}

void ImageFilter::InitWithColorBlur(const ColorFilter& cf, scalar x, scalar y)
{
    type_ = ImageFilter::FilterType::COLOR_FILTER;
    impl_->InitWithColorBlur(cf, x, y);
}

ImageFilter::ImageFilter(FilterType t, const std::vector<scalar>& coefficients, bool enforcePMColor,
    std::shared_ptr<ImageFilter> background, std::shared_ptr<ImageFilter> foreground) noexcept
    :ImageFilter()
{
    type_ = t;
    impl_->InitWithArithmetic(coefficients, enforcePMColor, background, foreground);
}

ImageFilter::ImageFilter(FilterType t, std::shared_ptr<ImageFilter> f1, std::shared_ptr<ImageFilter> f2) noexcept
    : ImageFilter()
{
    type_ = t;
    impl_->InitWithCompose(f1, f2);
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

std::shared_ptr<ImageFilter> ImageFilter::CreateBlurImageFilter(scalar sigmaX, scalar sigmaY, TileMode mode,
    std::shared_ptr<ImageFilter> input)
{
    return std::make_shared<ImageFilter>(ImageFilter::FilterType::BLUR, sigmaX, sigmaY, mode, input);
}

std::shared_ptr<ImageFilter> ImageFilter::CreateColorFilterImageFilter(
    const ColorFilter& cf, std::shared_ptr<ImageFilter> input)
{
    return std::make_shared<ImageFilter>(ImageFilter::FilterType::COLOR_FILTER, cf, input);
}

std::shared_ptr<ImageFilter> ImageFilter::CreateColorBlurImageFilter(const ColorFilter& cf,
    scalar sigmaX, scalar sigmaY)
{
    return std::make_shared<ImageFilter>(ImageFilter::FilterType::COLOR_FILTER, cf, sigmaX, sigmaY);
}

std::shared_ptr<ImageFilter> ImageFilter::CreateOffsetImageFilter(
    scalar dx, scalar dy, std::shared_ptr<ImageFilter> input)
{
    return std::make_shared<ImageFilter>(ImageFilter::FilterType::OFFSET, dx, dy, input);
}

std::shared_ptr<ImageFilter> ImageFilter::CreateArithmeticImageFilter(const std::vector<scalar>& coefficients,
    bool enforcePMColor, std::shared_ptr<ImageFilter> background, std::shared_ptr<ImageFilter> foreground)
{
    return std::make_shared<ImageFilter>(
        ImageFilter::FilterType::ARITHMETIC, coefficients, enforcePMColor, background, foreground);
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

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS