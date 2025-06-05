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
#include "effect/color_matrix.h"

namespace OHOS {
namespace Rosen {
RSLightUpEffectFilter::RSLightUpEffectFilter(float lightUpDegree)
    : RSDrawingFilterOriginal(RSLightUpEffectFilter::CreateLightUpEffectFilter(lightUpDegree)),
      lightUpDegree_(lightUpDegree)
{
    type_ = FilterType::LIGHT_UP_EFFECT;

    hash_ = SkOpts::hash(&type_, sizeof(type_), 0);
    hash_ = SkOpts::hash(&lightUpDegree_, sizeof(lightUpDegree_), hash_);
}

RSLightUpEffectFilter::~RSLightUpEffectFilter() = default;

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

float RSLightUpEffectFilter::GetLightUpDegree()
{
    return lightUpDegree_;
}

std::string RSLightUpEffectFilter::GetDescription()
{
    return "RSLightUpEffectFilter light up degree is " + std::to_string(lightUpDegree_);
}

std::shared_ptr<RSDrawingFilterOriginal> RSLightUpEffectFilter::Compose(
    const std::shared_ptr<RSDrawingFilterOriginal>& other) const
{
    std::shared_ptr<RSLightUpEffectFilter> result = std::make_shared<RSLightUpEffectFilter>(lightUpDegree_);
    result->imageFilter_ = Drawing::ImageFilter::CreateComposeImageFilter(imageFilter_, other->GetImageFilter());
    auto otherHash = other->Hash();
    result->hash_ = SkOpts::hash(&otherHash, sizeof(otherHash), hash_);
    return result;
}
} // namespace Rosen
} // namespace OHOS
