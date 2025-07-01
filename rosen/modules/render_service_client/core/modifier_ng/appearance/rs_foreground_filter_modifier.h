/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_MODIFIER_NG_APPEARANCE_RS_FOREGROUND_FILTER_MODIFIER_H
#define RENDER_SERVICE_CLIENT_CORE_MODIFIER_NG_APPEARANCE_RS_FOREGROUND_FILTER_MODIFIER_H

#include "ui_effect/property/include/rs_ui_filter.h"

#include "modifier_ng/rs_modifier_ng.h"

namespace OHOS::Rosen::ModifierNG {
class RSC_EXPORT RSForegroundFilterModifier : public RSModifier {
public:
    RSForegroundFilterModifier() = default;
    ~RSForegroundFilterModifier() override = default;

    static inline constexpr auto Type = RSModifierType::FOREGROUND_FILTER;
    RSModifierType GetType() const override
    {
        return Type;
    };

    void MarkNodeDirty() override
    {
        if (auto node = node_.lock()) {
            node->MarkDirty(NodeDirtyType::APPEARANCE, true);
        }
    }

    void SetSpherize(float spherize);
    void SetForegroundEffectRadius(float blurRadius);
    void SetMotionBlurParam(std::shared_ptr<MotionBlurParam> param);
    void SetFlyOutParams(const std::optional<RSFlyOutPara>& params);
    void SetFlyOutDegree(float degree);
    void SetDistortionK(const std::optional<float> distortionK);
    void SetAttractionFraction(float fraction);
    void SetAttractionDstPoint(const Vector2f& dstPoint);
    void SetUIFilter(std::shared_ptr<RSUIFilter> foregroundFilter);

    float GetSpherize() const;
    float GetForegroundEffectRadius() const;
    std::shared_ptr<MotionBlurParam> GetMotionBlurParam() const;
    std::optional<RSFlyOutPara> GetFlyOutParams() const;
    float GetFlyOutDegree() const;
    std::optional<float> GetDistortionK() const;
    float GetAttractionFraction() const;
    Vector2f GetAttractionDstPoint() const;

private:
    void AttachUIFilterProperty(std::shared_ptr<RSUIFilter> uiFilter);
};
} // namespace OHOS::Rosen::ModifierNG
#endif // RENDER_SERVICE_CLIENT_CORE_MODIFIER_NG_APPEARANCE_RS_FOREGROUND_FILTER_MODIFIER_H
