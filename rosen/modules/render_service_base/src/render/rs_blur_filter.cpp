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


namespace OHOS {
namespace Rosen {
const bool KAWASE_BLUR_ENABLED = RSSystemProperties::GetKawaseEnabled();
const auto BLUR_TYPE = KAWASE_BLUR_ENABLED ? Drawing::ImageBlurType::KAWASE : Drawing::ImageBlurType::GAUSS;
RSBlurFilter::RSBlurFilter(float blurRadiusX, float blurRadiusY, bool disableSystemAdaptation) :
    RSDrawingFilterOriginal(Drawing::ImageFilter::CreateBlurImageFilter(blurRadiusX, blurRadiusY,
        Drawing::TileMode::CLAMP, nullptr, BLUR_TYPE)),
    blurRadiusX_(blurRadiusX),
    blurRadiusY_(blurRadiusY),
    disableSystemAdaptation_(disableSystemAdaptation)
{
    type_ = FilterType::BLUR;

    float blurRadiusXForHash = DecreasePrecision(blurRadiusX);
    float blurRadiusYForHash = DecreasePrecision(blurRadiusY);
#ifndef ENABLE_M133_SKIA
    hash_ = SkOpts::hash(&type_, sizeof(type_), 0);
    hash_ = SkOpts::hash(&blurRadiusXForHash, sizeof(blurRadiusXForHash), hash_);
    hash_ = SkOpts::hash(&blurRadiusYForHash, sizeof(blurRadiusYForHash), hash_);
    hash_ = SkOpts::hash(&disableSystemAdaptation, sizeof(disableSystemAdaptation), hash_);
#else
    hash_ = SkChecksum::Hash32(&type_, sizeof(type_), 0);
    hash_ = SkChecksum::Hash32(&blurRadiusXForHash, sizeof(blurRadiusXForHash), hash_);
    hash_ = SkChecksum::Hash32(&blurRadiusYForHash, sizeof(blurRadiusYForHash), hash_);
    hash_ = SkChecksum::Hash32(&disableSystemAdaptation, sizeof(disableSystemAdaptation), hash_);
#endif
}

RSBlurFilter::~RSBlurFilter() = default;

float RSBlurFilter::GetBlurRadiusX()
{
    return blurRadiusX_;
}

float RSBlurFilter::GetBlurRadiusY()
{
    return blurRadiusY_;
}

bool RSBlurFilter::GetDisableSystemAdaptation()
{
    return disableSystemAdaptation_;
}

std::string RSBlurFilter::GetDescription()
{
    return "RSBlurFilter blur radius is " + std::to_string(blurRadiusX_) + " sigma";
}

std::string RSBlurFilter::GetDetailedDescription()
{
    return "RSBlurFilterBlur, radius: " + std::to_string(blurRadiusX_) + " sigma" +
        ", greyCoef1: " + std::to_string(greyCoef_ == std::nullopt ? 0.0f : greyCoef_->x_) +
        ", greyCoef2: " + std::to_string(greyCoef_ == std::nullopt ? 0.0f : greyCoef_->y_);
}

bool RSBlurFilter::IsValid() const
{
    constexpr float epsilon = 0.999f;
    return blurRadiusX_ > epsilon || blurRadiusY_ > epsilon;
}

std::shared_ptr<RSDrawingFilterOriginal> RSBlurFilter::Compose(
    const std::shared_ptr<RSDrawingFilterOriginal>& other) const
{
    std::shared_ptr<RSBlurFilter> result = std::make_shared<RSBlurFilter>(blurRadiusX_, blurRadiusY_,
        disableSystemAdaptation_);
    result->imageFilter_ = Drawing::ImageFilter::CreateComposeImageFilter(imageFilter_, other->GetImageFilter());
    auto otherHash = other->Hash();
#ifndef ENABLE_M133_SKIA
    result->hash_ = SkOpts::hash(&otherHash, sizeof(otherHash), hash_);
#else
    result->hash_ = SkChecksum::Hash32(&otherHash, sizeof(otherHash), hash_);
#endif
    return result;
}

std::shared_ptr<RSFilter> RSBlurFilter::Add(const std::shared_ptr<RSFilter>& rhs)
{
    if ((rhs == nullptr) || (rhs->GetFilterType() != FilterType::BLUR)) {
        return shared_from_this();
    }
    auto blurR = std::static_pointer_cast<RSBlurFilter>(rhs);
    return std::make_shared<RSBlurFilter>(
        blurRadiusX_ + blurR->GetBlurRadiusX(), blurRadiusY_ + blurR->GetBlurRadiusY(), disableSystemAdaptation_);
}

std::shared_ptr<RSFilter> RSBlurFilter::Sub(const std::shared_ptr<RSFilter>& rhs)
{
    if ((rhs == nullptr) || (rhs->GetFilterType() != FilterType::BLUR)) {
        return shared_from_this();
    }
    auto blurR = std::static_pointer_cast<RSBlurFilter>(rhs);
    return std::make_shared<RSBlurFilter>(
        blurRadiusX_ - blurR->GetBlurRadiusX(), blurRadiusY_ - blurR->GetBlurRadiusY(), disableSystemAdaptation_);
}

std::shared_ptr<RSFilter> RSBlurFilter::Multiply(float rhs)
{
    return std::make_shared<RSBlurFilter>(blurRadiusX_ * rhs, blurRadiusY_ * rhs, disableSystemAdaptation_);
}

std::shared_ptr<RSFilter> RSBlurFilter::Negate()
{
    return std::make_shared<RSBlurFilter>(-blurRadiusX_, -blurRadiusY_, disableSystemAdaptation_);
}

bool RSBlurFilter::IsNearEqual(const std::shared_ptr<RSFilter>& other, float threshold) const
{
    auto otherBlurFilter = std::static_pointer_cast<RSBlurFilter>(other);
    if (otherBlurFilter == nullptr) {
        ROSEN_LOGE("RSBlurFilter::IsNearEqual: the types of filters are different.");
        return true;
    }
    return ROSEN_EQ(blurRadiusX_, otherBlurFilter->GetBlurRadiusX(), threshold) &&
           ROSEN_EQ(blurRadiusY_, otherBlurFilter->GetBlurRadiusY(), threshold) &&
           disableSystemAdaptation_ == otherBlurFilter->GetDisableSystemAdaptation();
}

bool RSBlurFilter::IsNearZero(float threshold) const
{
    return ROSEN_EQ(blurRadiusX_, 0.0f, threshold) && ROSEN_EQ(blurRadiusY_, 0.0f, threshold);
}

bool RSBlurFilter::IsEqual(const std::shared_ptr<RSFilter>& other) const
{
    auto otherBlurFilter = std::static_pointer_cast<RSBlurFilter>(other);
    if (otherBlurFilter == nullptr) {
        ROSEN_LOGE("RSBlurFilter::IsEqual: the types of filters are different.");
        return true;
    }
    return ROSEN_EQ(blurRadiusX_, otherBlurFilter->GetBlurRadiusX()) &&
           ROSEN_EQ(blurRadiusY_, otherBlurFilter->GetBlurRadiusY()) &&
           disableSystemAdaptation_ == otherBlurFilter->GetDisableSystemAdaptation();
}

bool RSBlurFilter::IsEqualZero() const
{
    return ROSEN_EQ(blurRadiusX_, 0.0f) && ROSEN_EQ(blurRadiusY_, 0.0f);
}

void RSBlurFilter::DrawImageRect(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
    const Drawing::Rect& src, const Drawing::Rect& dst) const
{
    auto brush = GetBrush();
    std::shared_ptr<Drawing::Image> greyImage = image;
    if (greyCoef_.has_value()) {
        greyImage = RSPropertiesPainter::DrawGreyAdjustment(canvas, image, greyCoef_.value());
    }
    if (greyImage == nullptr) {
        greyImage = image;
    }
    // if kawase blur failed, use gauss blur
    static bool DDGR_ENABLED = RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR;
    KawaseParameter param = KawaseParameter(src, dst, blurRadiusX_, nullptr, brush.GetColor().GetAlphaF());
    if (!DDGR_ENABLED && KAWASE_BLUR_ENABLED &&
        KawaseBlurFilter::GetKawaseBlurFilter()->ApplyKawaseBlur(canvas, greyImage, param)) {
        return;
    }
    canvas.AttachBrush(brush);
    canvas.DrawImageRect(*greyImage, src, dst, Drawing::SamplingOptions());
    canvas.DetachBrush();
}

void RSBlurFilter::SetGreyCoef(const std::optional<Vector2f>& greyCoef)
{
    greyCoef_ = greyCoef;
}

bool RSBlurFilter::CanSkipFrame() const
{
    constexpr float HEAVY_BLUR_THRESHOLD = 25.0f;
    return blurRadiusX_ > HEAVY_BLUR_THRESHOLD && blurRadiusY_ > HEAVY_BLUR_THRESHOLD;
};
} // namespace Rosen
} // namespace OHOS
