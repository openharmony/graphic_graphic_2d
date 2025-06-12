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

#ifndef RENDER_SERVICE_BASE_MODIFIER_NG_APPEARANCE_RS_SHADOW_RENDER_MODIFIER_H
#define RENDER_SERVICE_BASE_MODIFIER_NG_APPEARANCE_RS_SHADOW_RENDER_MODIFIER_H

#include "modifier_ng/rs_render_modifier_ng.h"

namespace OHOS::Rosen::ModifierNG {
class RSB_EXPORT RSShadowRenderModifier : public RSRenderModifier {
public:
    RSShadowRenderModifier() = default;
    ~RSShadowRenderModifier() override = default;

    static inline constexpr auto Type = RSModifierType::SHADOW;
    RSModifierType GetType() const override
    {
        return Type;
    }

    static void ResetProperties(RSProperties& properties);

private:
    static const LegacyPropertyApplierMap LegacyPropertyApplierMap_;
    const LegacyPropertyApplierMap& GetLegacyPropertyApplierMap() const override
    {
        return LegacyPropertyApplierMap_;
    }
};
} // namespace OHOS::Rosen::ModifierNG
#endif // RENDER_SERVICE_BASE_MODIFIER_NG_APPEARANCE_RS_SHADOW_RENDER_MODIFIER_H
