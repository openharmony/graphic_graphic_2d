/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef RENDER_SERVICE_CLIENT_CORE_MODIFIER_NG_APPEARANCE_RS_USE_UNION_MODIFIER_H
#define RENDER_SERVICE_CLIENT_CORE_MODIFIER_NG_APPEARANCE_RS_USE_UNION_MODIFIER_H

#include "modifier_ng/rs_modifier_ng.h"

namespace OHOS::Rosen::ModifierNG {
class RSC_EXPORT RSUseUnionModifier : public RSModifier {
public:
    RSUseUnionModifier() = default;
    ~RSUseUnionModifier() override = default;

    static inline constexpr auto Type = RSModifierType::USE_UNION;
    RSModifierType GetType() const override
    {
        return Type;
    };

    void SetUseUnion(bool useUnion);
    void SetUnionSpacing(float unionSpacing);
    void SetSDFUnionMode(int sdfUnionMode);
    void SetGravityPullCenterFlag(bool gravityPullCenterFlag);
    void SetGravityPullStrength(float gravityPullStrength);
    void SetGravityHotZone(float gravityHotZone);

    bool GetUseUnion() const;
    float GetUnionSpacing() const;
    int GetSDFUnionMode() const;
    bool GetGravityPullCenterFlag() const;
    float GetGravityPullStrength() const;
    float GetGravityHotZone() const;
};
} // namespace OHOS::Rosen::ModifierNG
#endif // RENDER_SERVICE_CLIENT_CORE_MODIFIER_NG_APPEARANCE_RS_USE_UNION_MODIFIER_H
