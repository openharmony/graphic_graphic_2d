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

#ifndef ROSEN_RENDER_SERVICE_BASE_ANIMATION_RS_ANIMATION_RATE_DECIDER_H
#define ROSEN_RENDER_SERVICE_BASE_ANIMATION_RS_ANIMATION_RATE_DECIDER_H

#include "common/rs_common_def.h"
#include "modifier/rs_modifier_type.h"
#include "rs_frame_rate_range.h"

namespace OHOS {
namespace Rosen {
class RSRenderPropertyBase;
using FrameRateGetFunc = std::function<int32_t(RSPropertyUnit, float)>;
using PropertyValue = std::shared_ptr<RSRenderPropertyBase>;

class RSB_EXPORT RSAnimationRateDecider {
public:
    RSAnimationRateDecider() = default;
    ~RSAnimationRateDecider() = default;

    void SetEnable(bool enabled)
    {
        isEnabled_ = enabled;
    }
    void SetScaleReferenceSize(float width, float height);
    void Reset();
    void AddDecisionElement(PropertyId id, const PropertyValue& velocity, FrameRateRange range);
    void MakeDecision(const FrameRateGetFunc& func);
    const FrameRateRange& GetFrameRateRange() const;
private:
    int32_t CalculatePreferredRate(const PropertyValue& property, const FrameRateGetFunc& func);
    int32_t ProcessVector4f(const PropertyValue& property, const FrameRateGetFunc& func);
    int32_t ProcessVector2f(const PropertyValue& property, const FrameRateGetFunc& func);

    bool isEnabled_ = true;
    float scaleWidth_ = 0.0f;
    float scaleHeight_ = 0.0f;
    FrameRateRange frameRateRange_;
    std::unordered_map<PropertyId, std::pair<PropertyValue, FrameRateRange>> decisionElements_;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_ANIMATION_RS_ANIMATION_RATE_DECIDER_H