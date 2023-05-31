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
    type_ = FilterType::LIGHTUPEFFECT;
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
    Drawing::ColorMatrix cm;
    cm.SetArray(lightUp);
    std::shared_ptr<Drawing::ColorFilter> lightUpFilter = Drawing::ColorFilter::CreateMatrixColorFilter(cm);

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

std::shared_ptr<RSSkiaFilter> RSLightUpEffectFilter::Compose(const std::shared_ptr<RSSkiaFilter>& inner)
{
    std::shared_ptr<RSLightUpEffectFilter> lightUp = std::make_shared<RSLightUpEffectFilter>(lightUpDegree_);
    lightUp->imageFilter_ = SkImageFilters::Compose(imageFilter_, inner->GetImageFilter());
    return lightUp;
}

std::shared_ptr<RSFilter> RSLightUpEffectFilter::Add(const std::shared_ptr<RSFilter>& rhs)
{
    if ((rhs == nullptr) || (rhs->GetFilterType() != FilterType::LIGHTUPEFFECT)) {
        return shared_from_this();
    }
    auto lightUpFilter = std::static_pointer_cast<RSLightUpEffectFilter>(rhs);
    return std::make_shared<RSLightUpEffectFilter>(lightUpDegree_ + lightUpFilter->GetLightUpDegree());
}

std::shared_ptr<RSFilter> RSLightUpEffectFilter::Sub(const std::shared_ptr<RSFilter>& rhs)
{
    if ((rhs == nullptr) || (rhs->GetFilterType() != FilterType::LIGHTUPEFFECT)) {
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
} // namespace Rosen
} // namespace OHOS
