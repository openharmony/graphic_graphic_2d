/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "render/rs_light_up_effect_filter.h"

#include "src/core/SkOpts.h"

#include "platform/common/rs_log.h"
#ifdef USE_ROSEN_DRAWING
#include "effect/color_matrix.h"
#endif

namespace OHOS {
namespace Rosen {
RSLightUpEffectFilter::RSLightUpEffectFilter(float lightUpDegree)
#ifndef USE_ROSEN_DRAWING
    : RSSkiaFilter(RSLightUpEffectFilter::CreateLightUpEffectFilter(lightUpDegree)),
#else
    : RSDrawingFilter(RSLightUpEffectFilter::CreateLightUpEffectFilter(lightUpDegree)),
#endif
      lightUpDegree_(lightUpDegree)
{
    type_ = FilterType::LIGHT_UP_EFFECT;

    hash_ = SkOpts::hash(&type_, sizeof(type_), 0);
    hash_ = SkOpts::hash(&lightUpDegree_, sizeof(lightUpDegree_), hash_);
}

RSLightUpEffectFilter::~RSLightUpEffectFilter() = default;

#ifndef USE_ROSEN_DRAWING
sk_sp<SkImageFilter> RSLightUpEffectFilter::CreateLightUpEffectFilter(float lightUpDegree)
{
    float normalizedDegree = lightUpDegree - 1.0;
    const float lightUp[] = {
        1.000000f, 0.000000f, 0.000000f, 0.000000f, normalizedDegree,
        0.000000f, 1.000000f, 0.000000f, 0.000000f, normalizedDegree,
        0.000000f, 0.000000f, 1.000000f, 0.000000f, normalizedDegree,
        0.000000f, 0.000000f, 0.000000f, 1.000000f, 0.000000f,
    };
    sk_sp<SkColorFilter> lightUpFilter = SkColorFilters::Matrix(lightUp);

    return SkImageFilters::ColorFilter(lightUpFilter, nullptr);
}
#else
std::shared_ptr<Drawing::ImageFilter> RSLightUpEffectFilter::CreateLightUpEffectFilter(float lightUpDegree)
{
    float normalizedDegree = lightUpDegree - 1.0;
    const Drawing::scalar lightUp[] = {
        1.000000f, 0.000000f, 0.000000f, 0.000000f, normalizedDegree,
        0.000000f, 1.000000f, 0.000000f, 0.000000f, normalizedDegree,
        0.000000f, 0.000000f, 1.000000f, 0.000000f, normalizedDegree,
        0.000000f, 0.000000f, 0.000000f, 1.000000f, 0.000000f,
    };
    std::shared_ptr<Drawing::ColorFilter> lightUpFilter = Drawing::ColorFilter::CreateFloatColorFilter(lightUp);

    return Drawing::ImageFilter::CreateColorFilterImageFilter(*lightUpFilter, nullptr);
}
#endif

float RSLightUpEffectFilter::GetLightUpDegree()
{
    return lightUpDegree_;
}

std::string RSLightUpEffectFilter::GetDescription()
{
    return "RSLightUpEffectFilter light up degree is " + std::to_string(lightUpDegree_);
}

#ifndef USE_ROSEN_DRAWING
std::shared_ptr<RSSkiaFilter> RSLightUpEffectFilter::Compose(const std::shared_ptr<RSSkiaFilter>& other) const
#else
std::shared_ptr<RSDrawingFilter> RSLightUpEffectFilter::Compose(const std::shared_ptr<RSDrawingFilter>& other) const
#endif
{
    std::shared_ptr<RSLightUpEffectFilter> result = std::make_shared<RSLightUpEffectFilter>(lightUpDegree_);
#ifndef USE_ROSEN_DRAWING
    result->imageFilter_ = SkImageFilters::Compose(imageFilter_, other->GetImageFilter());
#else
    result->imageFilter_ = Drawing::ImageFilter::CreateComposeImageFilter(imageFilter_, other->GetImageFilter());
#endif
    auto otherHash = other->Hash();
    result->hash_ = SkOpts::hash(&otherHash, sizeof(otherHash), hash_);
    return result;
}

std::shared_ptr<RSFilter> RSLightUpEffectFilter::Add(const std::shared_ptr<RSFilter>& rhs)
{
    if ((rhs == nullptr) || (rhs->GetFilterType() != FilterType::LIGHT_UP_EFFECT)) {
        return shared_from_this();
    }
    auto lightUpFilter = std::static_pointer_cast<RSLightUpEffectFilter>(rhs);
    return std::make_shared<RSLightUpEffectFilter>(lightUpDegree_ + lightUpFilter->GetLightUpDegree());
}

std::shared_ptr<RSFilter> RSLightUpEffectFilter::Sub(const std::shared_ptr<RSFilter>& rhs)
{
    if ((rhs == nullptr) || (rhs->GetFilterType() != FilterType::LIGHT_UP_EFFECT)) {
        return shared_from_this();
    }
    auto lightUpFilter = std::static_pointer_cast<RSLightUpEffectFilter>(rhs);
    return std::make_shared<RSLightUpEffectFilter>(lightUpDegree_ - lightUpFilter->GetLightUpDegree());
}

std::shared_ptr<RSFilter> RSLightUpEffectFilter::Multiply(float rhs)
{
    return std::make_shared<RSLightUpEffectFilter>(lightUpDegree_ * rhs);
}

std::shared_ptr<RSFilter> RSLightUpEffectFilter::Negate()
{
    return std::make_shared<RSLightUpEffectFilter>(-lightUpDegree_);
}

bool RSLightUpEffectFilter::IsNearEqual(const std::shared_ptr<RSFilter>& other, float threshold) const
{
    auto otherLightUpFilter = std::static_pointer_cast<RSLightUpEffectFilter>(other);
    if (otherLightUpFilter == nullptr) {
        ROSEN_LOGE("RSLightUpEffectFilter::IsNearEqual: the types of filters are different.");
        return true;
    }
    float otherLightUpDegree = otherLightUpFilter->GetLightUpDegree();
    return ROSEN_EQ(lightUpDegree_, otherLightUpDegree, threshold);
}

bool RSLightUpEffectFilter::IsNearZero(float threshold) const
{
    return ROSEN_EQ(lightUpDegree_, 0.0f, threshold);
}
} // namespace Rosen
} // namespace OHOS
