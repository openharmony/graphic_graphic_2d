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

#ifndef RENDER_SERVICE_CLIENT_CORE_MODIFIER_NG_APPEARANCE_RS_SHADOW_MODIFIER_H
#define RENDER_SERVICE_CLIENT_CORE_MODIFIER_NG_APPEARANCE_RS_SHADOW_MODIFIER_H

#include "modifier_ng/rs_modifier_ng.h"

namespace OHOS::Rosen::ModifierNG {
class RSC_EXPORT RSShadowModifier : public RSModifier {
public:
    RSShadowModifier() = default;
    ~RSShadowModifier() override = default;

    static inline constexpr auto Type = RSModifierType::SHADOW;
    RSModifierType GetType() const override
    {
        return Type;
    };

    void SetShadowColor(Color color);
    void SetShadowOffsetX(float offsetX);
    void SetShadowOffsetY(float offsetY);
    void SetShadowAlpha(float alpha);
    void SetShadowElevation(float elevation);
    void SetShadowRadius(float radius);
    void SetShadowPath(const std::shared_ptr<RSPath>& shadowPath);
    void SetShadowMask(bool shadowMask);
    void SetShadowColorStrategy(int shadowColorStrategy);
    void SetShadowIsFilled(bool shadowIsFilled);
    void SetUseShadowBatching(bool useShadowBatching);

    Color GetShadowColor() const;
    float GetShadowOffsetX() const;
    float GetShadowOffsetY() const;
    float GetShadowAlpha() const;
    float GetShadowElevation() const;
    float GetShadowRadius() const;
    std::shared_ptr<RSPath> GetShadowPath() const;
    bool GetShadowMask() const;
    int GetShadowColorStrategy() const;
    bool GetShadowIsFilled() const;
    bool GetUseShadowBatching() const;
};
} // namespace OHOS::Rosen::ModifierNG
#endif // RENDER_SERVICE_CLIENT_CORE_MODIFIER_NG_APPEARANCE_RS_SHADOW_MODIFIER_H
