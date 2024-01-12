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

#include "animation/rs_animation_rate_decider.h"

#include <cmath>

#include "modifier/rs_render_property.h"

namespace OHOS {
namespace Rosen {
void RSAnimationRateDecider::Reset()
{
    frameRateRange_.Reset();
    decisionElements_.clear();
}

void RSAnimationRateDecider::AddDecisionElement(PropertyId id, const PropertyValue& velocity, FrameRateRange range)
{
    if (!velocity && !range.IsValid()) {
        return;
    }
    PropertyValue data = nullptr;
    if (velocity != nullptr) {
        data = velocity->Clone();
    }
    if (decisionElements_.find(id) != decisionElements_.end()) {
        auto& element = decisionElements_[id];
        if (data != nullptr) {
            element.first = element.first ? element.first + data : data;
        }
        element.second.Merge(range);
    } else {
        decisionElements_.emplace(id, std::make_pair(data, range));
    }
}

void RSAnimationRateDecider::MakeDecision(const FrameRateGetFunc& func)
{
    for (const auto& [_1, element] : decisionElements_) {
        FrameRateRange propertyRange;
        if (element.first != nullptr && func != nullptr) {
            int32_t preferred = CalculatePreferredRate(element.first, func);
            if (preferred > 0) {
                propertyRange = {0, RANGE_MAX_REFRESHRATE, preferred};
            }
        }
        FrameRateRange finalRange;
        if (propertyRange.IsValid()) {
            finalRange = propertyRange;
            if (element.second.IsValid() && element.second.preferred_ < propertyRange.preferred_) {
                finalRange = element.second;
            }
        } else {
            finalRange = element.second;
        }
        frameRateRange_.Merge(finalRange);
    }
}

const FrameRateRange& RSAnimationRateDecider::GetFrameRateRange() const
{
    return frameRateRange_;
}

int32_t RSAnimationRateDecider::CalculatePreferredRate(const PropertyValue& property, const FrameRateGetFunc& func)
{
    switch (property->GetPropertyType()) {
        case RSRenderPropertyType::PROPERTY_VECTOR4F:
            return ProcessVector4f(property, func);
        case RSRenderPropertyType::PROPERTY_VECTOR2F:
        case RSRenderPropertyType::PROPERTY_FLOAT:
            return func(property->GetPropertyUnit(), property->ToFloat());
        default:
            return 0;
    }
}

int32_t RSAnimationRateDecider::ProcessVector4f(const PropertyValue& property, const FrameRateGetFunc& func)
{
    auto animatableProperty = std::static_pointer_cast<RSRenderAnimatableProperty<Vector4f>>(property);
    auto propertyUnit = property->GetPropertyUnit();
    if (!animatableProperty || propertyUnit != RSPropertyUnit::PIXEL_POSITION) {
        return 0;
    }
    auto data = animatableProperty->Get();
    // Vector4f data include data[0], data[1], data[2], data[3]
    int32_t positionRate = func(propertyUnit, sqrt(data[0] * data[0] + data[1] * data[1]));
    int32_t sizeRate = func(propertyUnit, sqrt(data[2] * data[2] + data[3] * data[3]));
    return std::max(positionRate, sizeRate);
}
} // namespace Rosen
} // namespace OHOS
