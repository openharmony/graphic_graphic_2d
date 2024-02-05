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

#include "src/core/SkOpts.h"

#include "common/rs_common_def.h"
#include "platform/common/rs_log.h"
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
    useKawase_ = RSSystemProperties::GetKawaseEnabled();
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
}
#endif
#else
RSBlurFilter::RSBlurFilter(float blurRadiusX, float blurRadiusY) : RSDrawingFilter(
    Drawing::ImageFilter::CreateBlurImageFilter(blurRadiusX, blurRadiusY, Drawing::TileMode::CLAMP, nullptr,
        Drawing::ImageBlurType::KAWASE)),
    blurRadiusX_(blurRadiusX),
    blurRadiusY_(blurRadiusY)
{
    type_ = FilterType::BLUR;

    hash_ = SkOpts::hash(&type_, sizeof(type_), 0);
    hash_ = SkOpts::hash(&blurRadiusX, sizeof(blurRadiusX), hash_);
    hash_ = SkOpts::hash(&blurRadiusY, sizeof(blurRadiusY), hash_);
    useKawase_ = RSSystemProperties::GetKawaseEnabled();
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
    constexpr float epsilon = 0.999f;
    return blurRadiusX_ > epsilon || blurRadiusY_ > epsilon;
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
        ROSEN_LOGE("RSBlurFilter::IsNearEqual: the types of filters are different.");
        return true;
    }
    return ROSEN_EQ(blurRadiusX_, otherBlurFilter->GetBlurRadiusX(), threshold) &&
           ROSEN_EQ(blurRadiusY_, otherBlurFilter->GetBlurRadiusY(), threshold);
}

bool RSBlurFilter::IsNearZero(float threshold) const
{
    return ROSEN_EQ(blurRadiusX_, 0.0f, threshold) && ROSEN_EQ(blurRadiusY_, 0.0f, threshold);
}

#ifndef USE_ROSEN_DRAWING
void RSBlurFilter::DrawImageRect(
    SkCanvas& canvas, const sk_sp<SkImage>& image, const SkRect& src, const SkRect& dst) const
#else
void RSBlurFilter::DrawImageRect(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
    const Drawing::Rect& src, const Drawing::Rect& dst) const
#endif
{
#ifndef USE_ROSEN_DRAWING
    auto paint = GetPaint();
#ifdef NEW_SKIA
    sk_sp<SkImage> greyImage = image;
    if (isGreyCoefValid_) {
        greyImage = RSPropertiesPainter::DrawGreyAdjustment(canvas, image, greyCoef1_, greyCoef2_);
    }
    if (greyImage == nullptr) {
        greyImage = image;
    }
    // if kawase blur failed, use gauss blur
    KawaseParameter param = KawaseParameter(src, dst, blurRadiusX_, nullptr, paint.getAlphaf());
    if (useKawase_ && KawaseBlurFilter::GetKawaseBlurFilter()->ApplyKawaseBlur(canvas, greyImage, param)) {
        return;
    }
    canvas.drawImageRect(greyImage.get(), src, dst, SkSamplingOptions(), &paint, SkCanvas::kStrict_SrcRectConstraint);
#else
    canvas.drawImageRect(greyImage.get(), src, dst, &paint);
#endif
#else
    auto brush = GetBrush();
    std::shared_ptr<Drawing::Image> greyImage = image;
    if (isGreyCoefValid_) {
        greyImage = RSPropertiesPainter::DrawGreyAdjustment(canvas, image, greyCoef1_, greyCoef2_);
    }
    if (greyImage == nullptr) {
        greyImage = image;
    }
    // if kawase blur failed, use gauss blur
    static bool DDGR_ENABLED = RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR;
    KawaseParameter param = KawaseParameter(src, dst, blurRadiusX_, nullptr, brush.GetColor().GetAlphaF());
    if (!DDGR_ENABLED && useKawase_ &&
        KawaseBlurFilter::GetKawaseBlurFilter()->ApplyKawaseBlur(canvas, greyImage, param)) {
        return;
    }
    canvas.AttachBrush(brush);
    canvas.DrawImageRect(*greyImage, src, dst, Drawing::SamplingOptions());
    canvas.DetachBrush();
#endif
}

void RSBlurFilter::SetGreyCoef(float greyCoef1, float greyCoef2, bool isGreyCoefValid)
{
    if (!isGreyCoefValid) {
        isGreyCoefValid_ = isGreyCoefValid;
        return;
    }
    greyCoef1_ = greyCoef1;
    greyCoef2_ = greyCoef2;
    isGreyCoefValid_ = isGreyCoefValid;
}

bool RSBlurFilter::CanSkipFrame() const
{
    constexpr float HEAVY_BLUR_THRESHOLD = 25.0f;
    return blurRadiusX_ > HEAVY_BLUR_THRESHOLD && blurRadiusY_ > HEAVY_BLUR_THRESHOLD;
};
} // namespace Rosen
} // namespace OHOS
