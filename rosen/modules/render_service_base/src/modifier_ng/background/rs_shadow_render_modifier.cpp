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

#include "modifier_ng/background/rs_shadow_render_modifier.h"

#include "drawable/rs_property_drawable_utils.h"
#include "platform/common/rs_log.h"

namespace OHOS::Rosen::ModifierNG {
const RSShadowRenderModifier::LegacyPropertyApplierMap RSShadowRenderModifier::LegacyPropertyApplierMap_ = {
    { RSPropertyType::SHADOW_COLOR, RSRenderModifier::PropertyApplyHelper<Color, &RSProperties::SetShadowColor> },
    { RSPropertyType::SHADOW_OFFSET_X, RSRenderModifier::PropertyApplyHelper<float, &RSProperties::SetShadowOffsetX> },
    { RSPropertyType::SHADOW_OFFSET_Y, RSRenderModifier::PropertyApplyHelper<float, &RSProperties::SetShadowOffsetY> },
    { RSPropertyType::SHADOW_ALPHA, RSRenderModifier::PropertyApplyHelper<float, &RSProperties::SetShadowAlpha> },
    { RSPropertyType::SHADOW_ELEVATION,
        RSRenderModifier::PropertyApplyHelper<float, &RSProperties::SetShadowElevation> },
    { RSPropertyType::SHADOW_RADIUS, RSRenderModifier::PropertyApplyHelper<float, &RSProperties::SetShadowRadius> },
    { RSPropertyType::SHADOW_PATH,
        RSRenderModifier::PropertyApplyHelper<std::shared_ptr<RSPath>, &RSProperties::SetShadowPath> },
    { RSPropertyType::SHADOW_MASK, RSRenderModifier::PropertyApplyHelper<bool, &RSProperties::SetShadowMask> },
    { RSPropertyType::SHADOW_COLOR_STRATEGY,
        RSRenderModifier::PropertyApplyHelper<int, &RSProperties::SetShadowColorStrategy> },
    { RSPropertyType::SHADOW_IS_FILLED, RSRenderModifier::PropertyApplyHelper<bool, &RSProperties::SetShadowIsFilled> },
    { RSPropertyType::USE_SHADOW_BATCHING,
        RSRenderModifier::PropertyApplyHelper<bool, &RSProperties::SetUseShadowBatching> },
};

void RSShadowRenderModifier::ResetProperties(RSProperties& properties)
{
    properties.SetShadowColor({});
    properties.SetShadowOffsetX(0.f);
    properties.SetShadowOffsetY(0.f);
    properties.SetShadowAlpha(0.f);
    properties.SetShadowElevation(0.f);
    properties.SetShadowRadius(0.f);
    properties.SetShadowPath({});
    properties.SetShadowMask(false);
    properties.SetShadowColorStrategy(SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_NONE);
    properties.SetShadowIsFilled(false);
    properties.SetUseShadowBatching(false);
}

bool RSShadowRenderModifier::OnApply(RSModifierContext& context)
{
    if (!(HasProperty(RSPropertyType::SHADOW_PATH) || HasProperty(RSPropertyType::SHADOW_OFFSET_X) ||
            HasProperty(RSPropertyType::SHADOW_OFFSET_Y) || HasProperty(RSPropertyType::SHADOW_ELEVATION) ||
            HasProperty(RSPropertyType::SHADOW_COLOR) || HasProperty(RSPropertyType::SHADOW_IS_FILLED) ||
            HasProperty(RSPropertyType::SHADOW_COLOR_STRATEGY) || HasProperty(RSPropertyType::SHADOW_RADIUS) ||
            HasProperty(RSPropertyType::SHADOW_MASK) || HasProperty(RSPropertyType::SHADOW_ALPHA))) {
        return false;
    }

    std::shared_ptr<RSPath> rsPath = Getter<std::shared_ptr<RSPath>>(RSPropertyType::SHADOW_PATH, nullptr);
    statingPath_ =
        RSPropertyDrawableUtils::CreateShadowPath(rsPath, context.clipBoundsParams_.clipBounds_, context.GetRRect());
    statingOffsetX_ = Getter<float>(RSPropertyType::SHADOW_OFFSET_X, 0.f);
    statingOffsetY_ = Getter<float>(RSPropertyType::SHADOW_OFFSET_Y, 0.f);
    statingElevation_ = Getter<float>(RSPropertyType::SHADOW_ELEVATION, 0.f);
    statingColor_ = Getter<Color>(RSPropertyType::SHADOW_COLOR, Color(RgbPalette::Transparent()));
    statingIsFilled_ = Getter<bool>(RSPropertyType::SHADOW_IS_FILLED, true);
    statingColorStrategy_ = Getter<int>(RSPropertyType::SHADOW_COLOR_STRATEGY, 0);
    statingRadius_ = Getter<float>(RSPropertyType::SHADOW_RADIUS, 0.f);
    statingMask_ = Getter<bool>(RSPropertyType::SHADOW_MASK, false);
    statingAlpha_ = Getter<bool>(RSPropertyType::SHADOW_ALPHA, 1.f);
    statingUseShadowBatching_ = Getter<bool>(RSPropertyType::USE_SHADOW_BATCHING, false);

    return true;
}

void RSShadowRenderModifier::OnSync()
{
    renderPath_ = std::move(statingPath_);
    renderColor_ = std::move(statingColor_);
    renderOffsetX_ = statingOffsetX_;
    renderOffsetY_ = statingOffsetY_;
    renderElevation_ = statingElevation_;
    renderIsFilled_ = statingIsFilled_;
    renderRadius_ = statingRadius_;
    renderColorStrategy_ = statingColorStrategy_;
    renderAlpha_ = statingAlpha_;
    renderMask_ = statingMask_;
    renderUseShadowBatching_ = statingUseShadowBatching_;
}

void RSShadowRenderModifier::Draw(RSPaintFilterCanvas& canvas, Drawing::Rect& rect)
{
    // skip shadow if cache is enabled
    if (canvas.GetCacheType() == Drawing::CacheType::ENABLED) {
        ROSEN_LOGD("RSShadowRenderModifier::Draw cache type enabled.");
        return;
    }
    Drawing::Path path = renderPath_;
    Color shadowColor = renderColor_;
    if (renderColorStrategy_ != SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_NONE) {
        shadowColor = RSPropertyDrawableUtils::GetColorForShadowSyn(&canvas, path, renderColor_, renderColorStrategy_);
    }

    if (ROSEN_GNE(renderElevation_, 0.f)) {
        RSPropertyDrawableUtils::DrawShadow(
            &canvas, path, renderOffsetX_, renderOffsetY_, renderElevation_, renderIsFilled_, shadowColor);
        return;
    }
    if (ROSEN_GNE(renderRadius_, 0.f)) {
        RSPropertyDrawableUtils::DrawShadowMaskFilter(
            &canvas, path, renderOffsetX_, renderOffsetY_, renderRadius_, renderIsFilled_, shadowColor);
        return;
    }
}
} // namespace OHOS::Rosen::ModifierNG
