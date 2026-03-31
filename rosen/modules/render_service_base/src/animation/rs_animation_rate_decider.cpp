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
#include <string>

#include "common/rs_common_hook.h"
#include "common/rs_optional_trace.h"
#include "modifier/rs_render_property.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
constexpr int32_t DEFAULT_PREFERRED_FPS = 120;

void RSAnimationRateDecider::Reset()
{
    frameRateRange_.Reset();
    decisionElements_.clear();
}

void RSAnimationRateDecider::AddDecisionElement(PropertyId id, const PropertyValue& velocity, FrameRateRange range)
{
    if (!isEnabled_) {
        return;
    }
    if (!velocity && !range.IsValid() && range.componentScene_ == ComponentScene::UNKNOWN_SCENE) {
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
    if (!isEnabled_) {
        frameRateRange_.Set(0, RANGE_MAX_REFRESHRATE, DEFAULT_PREFERRED_FPS);
        return;
    }
    for (const auto& [id, element] : decisionElements_) {
        FrameRateRange propertyRange;
        RS_OPTIONAL_TRACE_BEGIN("MakeDecision property id: [" + std::to_string(id) + "]");
        if (element.first != nullptr && func != nullptr) {
            int32_t preferred = CalculatePreferredRate(element.first, func);
            if (preferred > 0) {
                propertyRange = {0, RANGE_MAX_REFRESHRATE, preferred};
                propertyRange.componentScene_ = element.second.componentScene_;
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
        RsCommonHook::Instance().GetComponentPowerFps(finalRange);
        frameRateRange_.Merge(finalRange);
        RS_OPTIONAL_TRACE_END();
    }
}

const FrameRateRange& RSAnimationRateDecider::GetFrameRateRange() const
{
    return frameRateRange_;
}

int32_t RSAnimationRateDecider::CalculatePreferredRate(const PropertyValue& property, const FrameRateGetFunc& func)
{
    switch (property->GetPropertyType()) {
        case RSPropertyType::VECTOR4F:
            return ProcessVector4f(property, func);
        case RSPropertyType::VECTOR2F:
            return ProcessVector2f(property, func);
        case RSPropertyType::FLOAT:
            return ProcessFloat(property, func);
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
    // data[0], data[1] indicate the speed of the position moves
    // data[2], data[3] indicate the speed of the width and height change
    int32_t matchFpsByPosition = func(propertyUnit, sqrt(data[0] * data[0] + data[1] * data[1]), absArea_, absLength_);
    float velocityX = data[2] * nodeScaleX_;
    float velocityY = data[3] * nodeScaleY_;
    int32_t matchFpsBySize = func(RSPropertyUnit::PIXEL_SIZE,
        sqrt(velocityX * velocityX + velocityY * velocityY), absArea_, absLength_);
    return std::max(matchFpsByPosition, matchFpsBySize);
}

int32_t RSAnimationRateDecider::ProcessVector2f(const PropertyValue& property, const FrameRateGetFunc& func)
{
    float velocity = 0.0f;
    if (property->GetPropertyUnit() == RSPropertyUnit::RATIO_SCALE) {
        auto animatableProperty = std::static_pointer_cast<RSRenderAnimatableProperty<Vector2f>>(property);
        if (animatableProperty != nullptr) {
            auto data = animatableProperty->Get();
            // Vector2f data include data[0], data[1]
            float velocityX = data[0] * nodeWidth_;
            float velocityY = data[1] * nodeHeight_;
            velocity = sqrt(velocityX * velocityX + velocityY * velocityY);
        }
    } else {
        // for other animation type such as translate
        velocity = property->ToFloat();
    }
    return func(property->GetPropertyUnit(), velocity, absArea_, absLength_);
}

int32_t RSAnimationRateDecider::ProcessFloat(const PropertyValue& property, const FrameRateGetFunc& func)
{
    auto propertyUnit = property->GetPropertyUnit();
    float propertyValue = property->ToFloat();
    if (propertyUnit == RSPropertyUnit::ANGLE_ROTATION) {
        // get the longest from height and width, record as H.
        float height = std::max(nodeWidth_ * nodeScaleX_, nodeHeight_ * nodeScaleY_);
        // V = W * R = W * (H / 2) = w * (2 * pi) * (H / 2) / 360 = w * pi * H / 360
        // let w = propertyValue => w *= H * FLOAT_PI / 360
        // 360 means 360 angle, relative to 2 * pi radian.
        propertyValue *= height * FLOAT_PI / 360;
        ROSEN_LOGD("%{public}s, ANGLE_ROTATION scene, propertyValue: %{public}f", __func__, propertyValue);
    }
    return func(propertyUnit, propertyValue, absArea_, absLength_);
}
} // namespace Rosen
} // namespace OHOS
