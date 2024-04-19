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
#include "ge_render.h"
#include "ge_visual_effect.h"

namespace OHOS {
namespace Rosen {
const bool KAWASE_BLUR_ENABLED = RSSystemProperties::GetKawaseEnabled();
const auto BLUR_TYPE = KAWASE_BLUR_ENABLED ? Drawing::ImageBlurType::KAWASE : Drawing::ImageBlurType::GAUSS;
RSBlurFilter::RSBlurFilter(float blurRadiusX, float blurRadiusY) : RSDrawingFilter(
    Drawing::ImageFilter::CreateBlurImageFilter(blurRadiusX, blurRadiusY, Drawing::TileMode::CLAMP, nullptr,
        BLUR_TYPE)),
    blurRadiusX_(blurRadiusX),
    blurRadiusY_(blurRadiusY)
{
    type_ = FilterType::BLUR;

    float blurRadiusXForHash = DecreasePrecision(blurRadiusX);
    float blurRadiusYForHash = DecreasePrecision(blurRadiusY);
    hash_ = SkOpts::hash(&type_, sizeof(type_), 0);
    hash_ = SkOpts::hash(&blurRadiusXForHash, sizeof(blurRadiusXForHash), hash_);
    hash_ = SkOpts::hash(&blurRadiusYForHash, sizeof(blurRadiusYForHash), hash_);
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

std::shared_ptr<RSDrawingFilter> RSBlurFilter::Compose(const std::shared_ptr<RSDrawingFilter>& other) const
{
    std::shared_ptr<RSBlurFilter> result = std::make_shared<RSBlurFilter>(blurRadiusX_, blurRadiusY_);
    result->imageFilter_ = Drawing::ImageFilter::CreateComposeImageFilter(imageFilter_, other->GetImageFilter());
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

void RSBlurFilter::DrawImageRect(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
    const Drawing::Rect& src, const Drawing::Rect& dst) const
{
    if (!image) {
        return;
    }
    auto brush = GetBrush();
    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    if (!visualEffectContainer) {
        return;
    }
    if (greyCoef_.has_value()) {
        auto greyFilter = std::make_shared<Drawing::GEVisualEffect>("GREY", Drawing::DrawingPaintType::BRUSH);
        greyFilter->SetParam("GREY_COEF_1", greyCoef_.value()[0]); // 模糊半径
        greyFilter->SetParam("GREY_COEF_2", greyCoef_.value()[1]); // 模糊半径
        visualEffectContainer->AddToChainedFilter(greyFilter);
    }
    auto geRender = std::make_shared<GraphicsEffectEngine::GERender>();
    if (!geRender) {
        return;
    }
    // if kawase blur failed, use gauss blur
    static bool DDGR_ENABLED = RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR;
    KawaseParameter param = KawaseParameter(src, dst, blurRadiusX_, nullptr, brush.GetColor().GetAlphaF());
    if (!DDGR_ENABLED && KAWASE_BLUR_ENABLED) {
        auto kawaseFilter = std::make_shared<Drawing::GEVisualEffect>("KAWASE_BLUR", Drawing::DrawingPaintType::BRUSH);
        kawaseFilter->SetParam("KAWASE_BLUR_RADIUS", (int)blurRadiusX_); // 模糊半径
        visualEffectContainer->AddToChainedFilter(kawaseFilter);

        auto outImage = geRender->ApplyImageEffect(canvas, *visualEffectContainer,
            image, src, src, Drawing::SamplingOptions());
        Drawing::Brush brushKawas;
        brushKawas.SetAlphaF(brush.GetColor().GetAlphaF());
        canvas.AttachBrush(brushKawas);
        canvas.DrawImageRect(*outImage, src, dst, Drawing::SamplingOptions());
        canvas.DetachBrush();
        return;
    }
    auto greyImage = geRender->ApplyImageEffect(canvas, *visualEffectContainer,
        image, src, src, Drawing::SamplingOptions());
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
