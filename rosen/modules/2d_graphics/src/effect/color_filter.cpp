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

#include "effect/color_filter.h"

#include "impl_factory.h"
#include "impl_interface/color_filter_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
ColorFilter::ColorFilter(FilterType t, ColorQuad c, BlendMode mode) noexcept : ColorFilter()
{
    type_ = t;
    impl_->InitWithBlendMode(c, mode);
}

ColorFilter::ColorFilter(FilterType t, ColorMatrix& m) noexcept : ColorFilter()
{
    type_ = t;
    impl_->InitWithColorMatrix(m);
}

ColorFilter::ColorFilter(FilterType t, ColorFilter& f1, ColorFilter& f2) noexcept : ColorFilter()
{
    type_ = t;
    impl_->InitWithCompose(f1, f2);
}

ColorFilter::ColorFilter(FilterType t) noexcept : ColorFilter()
{
    type_ = t;
    switch (type_) {
        case ColorFilter::FilterType::LINEAR_TO_SRGB_GAMMA:
            impl_->InitWithLinearToSrgbGamma();
            break;
        case ColorFilter::FilterType::SRGB_GAMMA_TO_LINEAR:
            impl_->InitWithSrgbGammaToLinear();
            break;
        default:
            break;
    }
}

ColorFilter::ColorFilter() noexcept
    : type_(ColorFilter::FilterType::NO_TYPE),
    impl_(ImplFactory::CreateColorFilterImpl()) {}

ColorFilter::FilterType ColorFilter::GetType() const
{
    return type_;
}

void ColorFilter::Compose(ColorFilter& filter)
{
    impl_->Compose(filter);
}

std::shared_ptr<ColorFilter> ColorFilter::CreateBlendModeColorFilter(ColorQuad c, BlendMode mode)
{
    return std::make_shared<ColorFilter>(ColorFilter::FilterType::BLEND_MODE, c, mode);
}

std::shared_ptr<ColorFilter> ColorFilter::CreateComposeColorFilter(ColorFilter& f1, ColorFilter& f2)
{
    return std::make_shared<ColorFilter>(ColorFilter::FilterType::COMPOSE, f1, f2);
}

std::shared_ptr<ColorFilter> ColorFilter::CreateMatrixColorFilter(ColorMatrix& m)
{
    return std::make_shared<ColorFilter>(ColorFilter::FilterType::MATRIX, m);
}

std::shared_ptr<ColorFilter> ColorFilter::CreateLinearToSrgbGamma()
{
    return std::make_shared<ColorFilter>(ColorFilter::FilterType::LINEAR_TO_SRGB_GAMMA);
}

std::shared_ptr<ColorFilter> ColorFilter::CreateSrgbGammaToLinear()
{
    return std::make_shared<ColorFilter>(ColorFilter::FilterType::SRGB_GAMMA_TO_LINEAR);
}
}
}
}