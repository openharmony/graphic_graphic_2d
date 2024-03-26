/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "render/rs_dynamic_dim_filter.h"

#include "src/core/SkOpts.h"
#include "platform/common/rs_log.h"
#include "effect/color_matrix.h"

namespace OHOS {
namespace Rosen {
RSDynamicDimFilter::RSDynamicDimFilter(float dynamicdimDegree)
    : RSDrawingFilter(RSDynamicDimFilter::CreateDynamicDimFilter(dynamicdimDegree)),
      dynamicDimDegree_(dynamicdimDegree)
{
    type_ = FilterType::DYNAMIC_DIM;

    hash_ = SkOpts::hash(&type_, sizeof(type_), 0);
    hash_ = SkOpts::hash(&dynamicDimDegree_, sizeof(dynamicDimDegree_), hash_);
}

RSDynamicDimFilter::~RSDynamicDimFilter() = default;

std::shared_ptr<Drawing::ImageFilter> RSDynamicDimFilter::CreateDynamicDimFilter(float dynamicdimDegree)
{
    float normalizedDegree = dynamicdimDegree - 1.0;
    const Drawing::scalar dynamicDim[] = {
        1.000000f, 0.000000f, 0.000000f, 0.000000f, normalizedDegree,
        0.000000f, 1.000000f, 0.000000f, 0.000000f, normalizedDegree,
        0.000000f, 0.000000f, 1.000000f, 0.000000f, normalizedDegree,
        0.000000f, 0.000000f, 0.000000f, 1.000000f, 0.000000f,
    };
    std::shared_ptr<Drawing::ColorFilter> dynamicDimFilter = Drawing::ColorFilter::CreateFloatColorFilter(dynamicDim);

    return Drawing::ImageFilter::CreateColorFilterImageFilter(*dynamicDimFilter, nullptr);
}

float RSDynamicDimFilter::GetDynamicDimDegree()
{
    return dynamicDimDegree_;
}

std::string RSDynamicDimFilter::GetDescription()
{
    return "RSDynamicDimFilter dynamic dimming degree is " + std::to_string(dynamicDimDegree_);
}

std::shared_ptr<RSDrawingFilter> RSDynamicDimFilter::Compose(const std::shared_ptr<RSDrawingFilter>& other) const
{
    std::shared_ptr<RSDynamicDimFilter> result = std::make_shared<RSDynamicDimFilter>(dynamicDimDegree_);
    result->imageFilter_ = Drawing::ImageFilter::CreateComposeImageFilter(imageFilter_, other->GetImageFilter());
    auto otherHash = other->Hash();
    result->hash_ = SkOpts::hash(&otherHash, sizeof(otherHash), hash_);
    return result;
}

std::shared_ptr<RSFilter> RSDynamicDimFilter::Add(const std::shared_ptr<RSFilter>& rhs)
{
    if ((rhs == nullptr) || (rhs->GetFilterType() != FilterType::DYNAMIC_DIM)) {
        return shared_from_this();
    }
    auto dynamicDimFilter = std::static_pointer_cast<RSDynamicDimFilter>(rhs);
    return std::make_shared<RSDynamicDimFilter>(dynamicDimDegree_ + dynamicDimFilter->GetDynamicDimDegree());
}

std::shared_ptr<RSFilter> RSDynamicDimFilter::Sub(const std::shared_ptr<RSFilter>& rhs)
{
    if ((rhs == nullptr) || (rhs->GetFilterType() != FilterType::DYNAMIC_DIM)) {
        return shared_from_this();
    }
    auto dynamicDimFilter = std::static_pointer_cast<RSDynamicDimFilter>(rhs);
    return std::make_shared<RSDynamicDimFilter>(dynamicDimDegree_ + dynamicDimFilter->GetDynamicDimDegree());
}

std::shared_ptr<RSFilter> RSDynamicDimFilter::Multiply(float rhs)
{
    return std::make_shared<RSDynamicDimFilter>(dynamicDimDegree_ * rhs);
}

std::shared_ptr<RSFilter> RSDynamicDimFilter::Negate()
{
    return std::make_shared<RSDynamicDimFilter>(-dynamicDimDegree_);
}

bool RSDynamicDimFilter::IsNearEqual(const std::shared_ptr<RSFilter>& other, float threshold) const
{
    auto otherDynamicDimFilter = std::static_pointer_cast<RSDynamicDimFilter>(other);
    if (otherDynamicDimFilter == nullptr) {
        ROSEN_LOGE("RSDynamicDimFilter::IsNearEqual: the types of filters are different.");
        return true;
    }
    float otherDynamicDimDegree = otherDynamicDimFilter->GetDynamicDimDegree();
    return ROSEN_EQ(dynamicDimDegree_, otherDynamicDimDegree, threshold);
}

bool RSDynamicDimFilter::IsNearZero(float threshold) const
{
    return ROSEN_EQ(dynamicDimDegree_, 0.0f, threshold);
}
} // namespace Rosen
} // namespace OHOS
