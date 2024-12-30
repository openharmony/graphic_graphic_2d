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

#ifndef RENDER_SERVICE_BASE_MODIFIER_NG_BACKGROUND_RS_USE_EFFECT_RENDER_MODIFIER_H
#define RENDER_SERVICE_BASE_MODIFIER_NG_BACKGROUND_RS_USE_EFFECT_RENDER_MODIFIER_H

#include "modifier_ng/rs_render_modifier_ng.h"
#include "property/rs_properties_def.h"

namespace OHOS::Rosen::DrawableV2 {
class RSRenderNodeDrawableAdapter;
}

namespace OHOS::Rosen::ModifierNG {
class RSB_EXPORT RSUseEffectRenderModifier : public RSRenderModifier {
public:
    RSUseEffectRenderModifier() = default;
    ~RSUseEffectRenderModifier() override = default;

    static inline constexpr auto Type = ModifierNG::RSModifierType::USE_EFFECT;
    ModifierNG::RSModifierType GetType() const override
    {
        return Type;
    }

    void Draw(RSPaintFilterCanvas& canvas, Drawing::Rect& rect) override;

    void ResetProperties(RSProperties& properties) override;

private:
    static const LegacyPropertyApplierMap LegacyPropertyApplierMap_;
    const LegacyPropertyApplierMap& GetLegacyPropertyApplierMap() const override
    {
        return LegacyPropertyApplierMap_;
    }

    bool OnApply(RSModifierContext& context) override;
    void OnSync() override;

    UseEffectType stagingUseEffectType_ = UseEffectType::DEFAULT;
    UseEffectType renderUseEffectType_ = UseEffectType::DEFAULT;
    std::weak_ptr<Rosen::DrawableV2::RSRenderNodeDrawableAdapter> effectRenderNodeDrawableWeakRef_;
};
} // namespace OHOS::Rosen::ModifierNG
#endif // RENDER_SERVICE_BASE_MODIFIER_NG_BACKGROUND_RS_USE_EFFECT_RENDER_MODIFIER_H
