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

#ifndef RENDER_SERVICE_BASE_MODIFIER_NG_RS_BACKGROUND_RENDER_MODIFIER_H
#define RENDER_SERVICE_BASE_MODIFIER_NG_RS_BACKGROUND_RENDER_MODIFIER_H

#include "modifier_ng/rs_render_modifier_ng.h"
#include "render/rs_filter.h"

namespace OHOS::Rosen::ModifierNG {
class RSB_EXPORT RSBackgroundFilterRenderModifier : public RSFilterRenderModifier {
public:
    RSBackgroundFilterRenderModifier() = default;
    ~RSBackgroundFilterRenderModifier() override = default;

    static inline constexpr auto Type = ModifierNG::RSModifierType::BACKGROUND_FILTER;
    ModifierNG::RSModifierType GetType() const override
    {
        return Type;
    };

    void ResetProperties(RSProperties& properties) override;

private:
    static const LegacyPropertyApplierMap LegacyPropertyApplierMap_;
    const LegacyPropertyApplierMap& GetLegacyPropertyApplierMap() const override
    {
        return LegacyPropertyApplierMap_;
    }

    inline float DecreasePrecision(float value)
    {
        // preserve two digital precision when calculating hash, this can reuse filterCache as much as possible.
        return 0.01 * round(value * 100);
    }
    bool IsBackgroundMaterialFilterValid() const;
    bool IsBackgroundBlurRadiusXValid() const;
    bool IsBackgroundBlurRadiusYValid() const;
    bool IsWaterRippleValid() const;
    void GenerateBackgroundBlurFilter();
    void GenerateBackgroundMaterialBlurFilter();
    void GenerateBackgroundMaterialFuzedBlurFilter();
    void GenerateAIBarFilter();
    void GenerateMagnifierFilter();
    void GenerateWaterRippleFilter();
    bool OnApply(RSModifierContext& context) override;
};
} // namespace OHOS::Rosen::ModifierNG
#endif // RENDER_SERVICE_BASE_MODIFIER_NG_RS_BACKGROUND_RENDER_MODIFIER_H
