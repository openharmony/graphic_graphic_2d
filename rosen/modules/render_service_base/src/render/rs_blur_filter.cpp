/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "render/rs_blur_filter.h"

#ifndef USE_ROSEN_DRAWING
#if defined(NEW_SKIA)
#include "include/core/SkTileMode.h"
#include "include/effects/SkImageFilters.h"
#else
#include "include/effects/SkBlurImageFilter.h"
#endif
#endif
namespace OHOS {
namespace Rosen {
#ifndef USE_ROSEN_DRAWING
#if defined(NEW_SKIA)
RSBlurFilter::RSBlurFilter(float blurRadiusX, float blurRadiusY): RSSkiaFilter(SkImageFilters::Blur(blurRadiusX,
    blurRadiusY, SkTileMode::kClamp, nullptr)), blurRadiusX_(blurRadiusX),
    blurRadiusY_(blurRadiusY)
{
    type_ = FilterType::BLUR;
}
#else
RSBlurFilter::RSBlurFilter(float blurRadiusX, float blurRadiusY): RSSkiaFilter(SkBlurImageFilter::Make(blurRadiusX,
    blurRadiusY, nullptr, nullptr, SkBlurImageFilter::kClamp_TileMode)), blurRadiusX_(blurRadiusX),
    blurRadiusY_(blurRadiusY)
{
    type_ = FilterType::BLUR;
}
#endif
#else
RSBlurFilter::RSBlurFilter(float blurRadiusX, float blurRadiusY)
    : RSDrawingFilter(Drawing::ImageFilter::CreateBlurImageFilter(blurRadiusX, blurRadiusY, Drawing::TileMode::CLAMP,
    nullptr)), blurRadiusX_(blurRadiusX), blurRadiusY_(blurRadiusY)
{
    type_ = FilterType::BLUR;
}
#endif

RSBlurFilter::~RSBlurFilter() {}

float RSBlurFilter::GetBlurRadiusX()
{
    return blurRadiusX_;
}

float RSBlurFilter::GetBlurRadiusY()
{
    return blurRadiusY_;
}

std::string RSBlurFilter::GetDescription()
{
    return "RSBlurFilter blur radius is " + std::to_string(blurRadiusX_) + " sigma";
}

std::shared_ptr<RSSkiaFilter> RSBlurFilter::Compose(const std::shared_ptr<RSSkiaFilter>& inner)
{
    std::shared_ptr<RSBlurFilter> blur = std::make_shared<RSBlurFilter>(blurRadiusX_, blurRadiusY_);
    blur->imageFilter_ = SkImageFilters::Compose(imageFilter_, inner->GetImageFilter());
    return blur;
}

std::shared_ptr<RSFilter> RSBlurFilter::Add(const std::shared_ptr<RSFilter>& rhs)
{
    if ((rhs == nullptr) || (rhs->GetFilterType() != FilterType::BLUR)) {
        return shared_from_this();
    }
    auto blurR = std::static_pointer_cast<RSBlurFilter>(rhs);
    return std::make_shared<RSBlurFilter>(blurRadiusX_ + blurR->GetBlurRadiusX(),
        blurRadiusY_ + blurR->GetBlurRadiusY());
}

std::shared_ptr<RSFilter> RSBlurFilter::Sub(const std::shared_ptr<RSFilter>& rhs)
{
    if ((rhs == nullptr) || (rhs->GetFilterType() != FilterType::BLUR)) {
        return shared_from_this();
    }
    auto blurR = std::static_pointer_cast<RSBlurFilter>(rhs);
    return std::make_shared<RSBlurFilter>(blurRadiusX_ - blurR->GetBlurRadiusX(),
        blurRadiusY_ - blurR->GetBlurRadiusY());
}

std::shared_ptr<RSFilter> RSBlurFilter::Multiply(float rhs)
{
    return std::make_shared<RSBlurFilter>(blurRadiusX_ * rhs, blurRadiusY_ * rhs);
}

std::shared_ptr<RSFilter> RSBlurFilter::Negate()
{
    return std::make_shared<RSBlurFilter>(-blurRadiusX_, -blurRadiusY_);
}
} // namespace Rosen
} // namespace OHOS
