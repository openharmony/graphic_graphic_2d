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

#include "render/rs_blur_filter.h"
#include "render/rs_kawase_blur.h"

#include "src/core/SkOpts.h"

#include "common/rs_common_def.h"

#include "platform/common/rs_system_properties.h"

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

    hash_ = SkOpts::hash(&type_, sizeof(type_), 0);
    hash_ = SkOpts::hash(&blurRadiusX, sizeof(blurRadiusX), hash_);
    hash_ = SkOpts::hash(&blurRadiusY, sizeof(blurRadiusY), hash_);
    useKawase = RSSystemProperties::GetKawaseEnabled();
}
#else
RSBlurFilter::RSBlurFilter(float blurRadiusX, float blurRadiusY): RSSkiaFilter(SkBlurImageFilter::Make(blurRadiusX,
    blurRadiusY, nullptr, nullptr, SkBlurImageFilter::kClamp_TileMode)), blurRadiusX_(blurRadiusX),
    blurRadiusY_(blurRadiusY)
{
    type_ = FilterType::BLUR;

    hash_ = SkOpts::hash(&type_, sizeof(type_), 0);
    hash_ = SkOpts::hash(&blurRadiusX, sizeof(blurRadiusX), hash_);
    hash_ = SkOpts::hash(&blurRadiusY, sizeof(blurRadiusY), hash_);

    useKawase = RSSystemProperties::GetKawaseEnabled();
}
#endif
#else
RSBlurFilter::RSBlurFilter(float blurRadiusX, float blurRadiusY) : RSDrawingFilter(
    Drawing::ImageFilter::CreateBlurImageFilter(blurRadiusX, blurRadiusY, Drawing::TileMode::CLAMP, nullptr)),
    blurRadiusX_(blurRadiusX),
    blurRadiusY_(blurRadiusY)
{
    type_ = FilterType::BLUR;

    hash_ = SkOpts::hash(&type_, sizeof(type_), 0);
    hash_ = SkOpts::hash(&blurRadiusX, sizeof(blurRadiusX), hash_);
    hash_ = SkOpts::hash(&blurRadiusY, sizeof(blurRadiusY), hash_);
}
#endif

RSBlurFilter::~RSBlurFilter() = default;

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

bool RSBlurFilter::IsValid() const
{
    constexpr float epsilon = 0.001f;
    if (ROSEN_EQ(blurRadiusX_, 0.f, epsilon) && ROSEN_EQ(blurRadiusY_, 0.f, epsilon)) {
        return false;
    }
    return true;
}

#ifndef USE_ROSEN_DRAWING
std::shared_ptr<RSSkiaFilter> RSBlurFilter::Compose(const std::shared_ptr<RSSkiaFilter>& other) const
#else
std::shared_ptr<RSDrawingFilter> RSBlurFilter::Compose(const std::shared_ptr<RSDrawingFilter>& other) const
#endif
{
    std::shared_ptr<RSBlurFilter> result = std::make_shared<RSBlurFilter>(blurRadiusX_, blurRadiusY_);
#ifndef USE_ROSEN_DRAWING
    result->imageFilter_ = SkImageFilters::Compose(imageFilter_, other->GetImageFilter());
#else
    result->imageFilter_ = Drawing::ImageFilter::CreateComposeImageFilter(imageFilter_, other->GetImageFilter());
#endif
    auto otherHash = other->Hash();
    result->hash_ = SkOpts::hash(&otherHash, sizeof(otherHash), hash_);
    return result;
}

std::shared_ptr<RSFilter> RSBlurFilter::Add(const std::shared_ptr<RSFilter>& rhs)
{
    if ((rhs == nullptr) || (rhs->GetFilterType() != FilterType::BLUR)) {
        return shared_from_this();
    }
    auto blurR = std::static_pointer_cast<RSBlurFilter>(rhs);
    return std::make_shared<RSBlurFilter>(
        blurRadiusX_ + blurR->GetBlurRadiusX(), blurRadiusY_ + blurR->GetBlurRadiusY());
}

std::shared_ptr<RSFilter> RSBlurFilter::Sub(const std::shared_ptr<RSFilter>& rhs)
{
    if ((rhs == nullptr) || (rhs->GetFilterType() != FilterType::BLUR)) {
        return shared_from_this();
    }
    auto blurR = std::static_pointer_cast<RSBlurFilter>(rhs);
    return std::make_shared<RSBlurFilter>(
        blurRadiusX_ - blurR->GetBlurRadiusX(), blurRadiusY_ - blurR->GetBlurRadiusY());
}

std::shared_ptr<RSFilter> RSBlurFilter::Multiply(float rhs)
{
    return std::make_shared<RSBlurFilter>(blurRadiusX_ * rhs, blurRadiusY_ * rhs);
}

std::shared_ptr<RSFilter> RSBlurFilter::Negate()
{
    return std::make_shared<RSBlurFilter>(-blurRadiusX_, -blurRadiusY_);
}

bool RSBlurFilter::IsNearEqual(const std::shared_ptr<RSFilter>& other, float threshold) const
{
    auto otherBlurFilter = std::static_pointer_cast<RSBlurFilter>(other);
    if (otherBlurFilter == nullptr) {
        return true;
    }
    return ROSEN_EQ(blurRadiusX_, otherBlurFilter->GetBlurRadiusX(), threshold) &&
           ROSEN_EQ(blurRadiusY_, otherBlurFilter->GetBlurRadiusY(), threshold);
}

bool RSBlurFilter::IsNearZero(float threshold) const
{
    return ROSEN_EQ(blurRadiusX_, 0.0f, threshold) && ROSEN_EQ(blurRadiusY_, 0.0f, threshold);
}

void RSBlurFilter::DrawImageRect(
    SkCanvas& canvas, const sk_sp<SkImage>& image, const SkRect& src, const SkRect& dst) const
{
    if (!useKawase) {
        auto paint = GetPaint();
#ifdef NEW_SKIA
        canvas.drawImageRect(image.get(), src, dst, SkSamplingOptions(), &paint, SkCanvas::kStrict_SrcRectConstraint);
#else
        canvas.drawImageRect(image.get(), src, dst, &paint);
#endif
        return;
    }
    KawaseBlur kawase;
    kawase.ApplyKawaseBlur(canvas, image, src, dst, static_cast<int>(blurRadiusX_));
}
} // namespace Rosen
} // namespace OHOS
