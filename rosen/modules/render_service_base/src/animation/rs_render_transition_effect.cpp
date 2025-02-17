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

#include "animation/rs_render_transition_effect.h"

#include <climits>

#include "animation/rs_animation_common.h"
#include "animation/rs_value_estimator.h"
#include "modifier/rs_render_modifier.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_marshalling_helper.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr int PID_SHIFT = 32;

PropertyId GenerateTransitionPropertyId()
{
    // manually set pid to INT_MAX to avoid conflict with other process (note: valid pid is smaller than 2^22)
    static pid_t pid_ = INT_MAX;
    static std::atomic<uint32_t> currentId_ = 1;

    auto currentId = currentId_.fetch_add(1, std::memory_order_relaxed);
    if (currentId == UINT32_MAX) {
        // [PLANNING]:process the overflow situations
        ROSEN_LOGE("Property Id overflow");
    }

    return ((PropertyId)pid_ << PID_SHIFT) | currentId;
}
} // namespace

const std::shared_ptr<RSRenderModifier>& RSRenderTransitionEffect::GetModifier()
{
    if (modifier_ == nullptr) {
        modifier_ = CreateModifier();
    }
    return modifier_;
}

const std::shared_ptr<RSRenderModifier> RSTransitionFade::CreateModifier()
{
    property_ = std::make_shared<RSRenderAnimatableProperty<float>>(0, GenerateTransitionPropertyId());
    return std::make_shared<RSAlphaRenderModifier>(property_);
}

void RSTransitionFade::UpdateFraction(float fraction) const
{
    if (property_ == nullptr) {
        return;
    }
    float startValue(1.0f);
    float endValue(alpha_);
    auto value = startValue * (1.0f - fraction) + endValue * fraction;
    property_->Set(value);
}

const std::shared_ptr<RSRenderModifier> RSTransitionScale::CreateModifier()
{
    property_ =
        std::make_shared<RSRenderAnimatableProperty<Vector2f>>(Vector2f { 0, 0 }, GenerateTransitionPropertyId());
    return std::make_shared<RSScaleRenderModifier>(property_);
}

void RSTransitionScale::UpdateFraction(float fraction) const
{
    if (property_ == nullptr) {
        return;
    }
    Vector2f startValue(1.0f, 1.0f);
    Vector2f endValue(scaleX_, scaleY_);
    auto value = startValue * (1.0f - fraction) + endValue * fraction;
    property_->Set(value);
}

const std::shared_ptr<RSRenderModifier> RSTransitionTranslate::CreateModifier()
{
    property_ =
        std::make_shared<RSRenderAnimatableProperty<Vector2f>>(Vector2f { 0, 0 }, GenerateTransitionPropertyId());
    return std::make_shared<RSTranslateRenderModifier>(property_);
}

void RSTransitionTranslate::UpdateFraction(float fraction) const
{
    if (property_ == nullptr) {
        return;
    }
    Vector2f startValue(0.0f, 0.0f);
    Vector2f endValue(translateX_, translateY_);
    auto value = startValue * (1.0f - fraction) + endValue * fraction;
    property_->Set(value);
}

const std::shared_ptr<RSRenderModifier> RSTransitionRotate::CreateModifier()
{
    property_ = std::make_shared<RSRenderAnimatableProperty<Quaternion>>(Quaternion {}, GenerateTransitionPropertyId());
    return std::make_shared<RSQuaternionRenderModifier>(property_);
}

void RSTransitionRotate::UpdateFraction(float fraction) const
{
    if (property_ == nullptr) {
        return;
    }
    auto radian = radian_ * fraction;
    float factor = std::sin(radian / 2);
    float qx = dx_ * factor;
    float qy = dy_ * factor;
    float qz = dz_ * factor;
    float qw = std::cos(radian / 2);
    Quaternion value(qx, qy, qz, qw);
    property_->Set(value);
}
} // namespace Rosen
} // namespace OHOS
