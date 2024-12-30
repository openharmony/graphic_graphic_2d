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

#ifndef RENDER_SERVICE_BASE_MODIFIER_NG_BACKGROUND_RS_SHADOW_RENDER_MODIFIER_H
#define RENDER_SERVICE_BASE_MODIFIER_NG_BACKGROUND_RS_SHADOW_RENDER_MODIFIER_H

#include "modifier_ng/rs_render_modifier_ng.h"

namespace OHOS::Rosen::ModifierNG {
class RSB_EXPORT RSShadowRenderModifier : public RSRenderModifier {
public:
    RSShadowRenderModifier() = default;
    ~RSShadowRenderModifier() override = default;

    static inline constexpr auto Type = ModifierNG::RSModifierType::SHADOW;
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

    struct ShadowParams {
        bool isFilled { false };
        float offsetX { 0.0f };
        float offsetY { 0.0f };
        float elevation { 0.0f };
        float radius { 0.0f };
        int colorStrategy { 0 };
        Drawing::Path path;
        Color color;
        float alpha { 1.0f };
        bool mask { false };
        bool useShadowBatching { false };
    };

    bool IsShadowValid();
    bool OnApply(RSModifierContext& context) override;
    void OnSync() override;

    bool statingIsFilled_ { false };
    float statingOffsetX_ { 0.0f };
    float statingOffsetY_ { 0.0f };
    float statingElevation_ { 0.0f };
    float statingRadius_ { 0.0f };
    int statingColorStrategy_ { 0 };
    Drawing::Path statingPath_;
    Color statingColor_;
    float statingAlpha_ { 1.0f };
    bool statingMask_ { false };
    bool statingUseShadowBatching_ { false };

    bool renderIsFilled_ { false };
    float renderOffsetX_ { 0.0f };
    float renderOffsetY_ { 0.0f };
    float renderElevation_ { 0.0f };
    float renderRadius_ { 0.0f };
    int renderColorStrategy_ { 0 };
    Drawing::Path renderPath_;
    Color renderColor_;
    float renderAlpha_ { 1.0f };
    bool renderMask_ { false };
    bool renderUseShadowBatching_ { false };
};
} // namespace OHOS::Rosen::ModifierNG
#endif // RENDER_SERVICE_BASE_MODIFIER_NG_BACKGROUND_RS_SHADOW_RENDER_MODIFIER_H
