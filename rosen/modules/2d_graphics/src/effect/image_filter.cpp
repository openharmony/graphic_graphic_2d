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

#include "effect/image_filter.h"

#include "impl_factory.h"
#include "impl_interface/image_filter_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
ImageFilter::ImageFilter(FilterType t, scalar x, scalar y, ImageFilter& input) noexcept : ImageFilter()
{
    type_ = t;
    if (type_ == ImageFilter::FilterType::BLUR) {
        impl_->InitWithBlur(x, y, input);
    } else if (type_ == ImageFilter::FilterType::OFFSET) {
        impl_->InitWithOffset(x, y, input);
    }
}

ImageFilter::ImageFilter(FilterType t, ColorFilter& cf, ImageFilter& input) noexcept : ImageFilter()
{
    type_ = t;
    impl_->InitWithColor(cf, input);
}

ImageFilter::ImageFilter(FilterType t, scalar k1, scalar k2, scalar k3, scalar k4, bool enforcePMColor,
                         ImageFilter& background, ImageFilter& foreground) noexcept : ImageFilter()
{
    type_ = t;
    impl_->InitWithArithmetic(k1, k2, k3, k4, enforcePMColor, background, foreground);
}

ImageFilter::ImageFilter(FilterType t, ImageFilter& f1, ImageFilter& f2) noexcept : ImageFilter()
{
    type_ = t;
    impl_->InitWithCompose(f1, f2);
}

ImageFilter::ImageFilter() noexcept
    : type_(ImageFilter::FilterType::NO_TYPE),
    impl_(ImplFactory::CreateImageFilterImpl()) {}

ImageFilter::FilterType ImageFilter::GetType() const
{
    return type_;
}

std::shared_ptr<ImageFilter> ImageFilter::CreateBlurImageFilter(scalar sigmaX, scalar sigmaY, ImageFilter& input)
{
    return std::make_shared<ImageFilter>(ImageFilter::FilterType::BLUR, sigmaX, sigmaY, input);
}

std::shared_ptr<ImageFilter> ImageFilter::CreateColorFilterImageFilter(ColorFilter& cf, ImageFilter& input)
{
    return std::make_shared<ImageFilter>(ImageFilter::FilterType::COLOR, cf, input);
}

std::shared_ptr<ImageFilter> ImageFilter::CreateOffsetImageFilter(scalar dx, scalar dy, ImageFilter& input)
{
    return std::make_shared<ImageFilter>(ImageFilter::FilterType::OFFSET, dx, dy, input);
}

std::shared_ptr<ImageFilter> ImageFilter::CreateArithmeticImageFilter(scalar k1, scalar k2, scalar k3, scalar k4,
    bool enforcePMColor, ImageFilter& background, ImageFilter& foreground)
{
    return std::make_shared<ImageFilter>(ImageFilter::FilterType::ARITHMETIC, k1, k2, k3, k4, enforcePMColor, background, foreground);
}

std::shared_ptr<ImageFilter> ImageFilter::CreateComposeImageFilter(ImageFilter& f1, ImageFilter& f2)
{
    return std::make_shared<ImageFilter>(ImageFilter::FilterType::COMPOSE, f1, f2);
}
}
}
}