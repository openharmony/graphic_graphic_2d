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

#include "modifier_ng/appearance/rs_shadow_modifier.h"

namespace OHOS::Rosen::ModifierNG {
RSShadowModifier::RSShadowModifier() : shadowAlpha_(0.f)
{}

void RSShadowModifier::SetShadowColor(Color color)
{
    shadowAlpha_ = color.GetAlphaF();
    Setter(RSPropertyType::SHADOW_COLOR, color);
}

Color RSShadowModifier::GetShadowColor() const
{
    return Getter(RSPropertyType::SHADOW_COLOR, Color::FromArgbInt(DEFAULT_SPOT_COLOR));
}

void RSShadowModifier::SetShadowOffsetX(float offsetX)
{
    Setter(RSPropertyType::SHADOW_OFFSET_X, offsetX);
}

float RSShadowModifier::GetShadowOffsetX() const
{
    return Getter(RSPropertyType::SHADOW_OFFSET_X, 0.f);
}

void RSShadowModifier::SetShadowOffsetY(float offsetY)
{
    Setter(RSPropertyType::SHADOW_OFFSET_Y, offsetY);
}

float RSShadowModifier::GetShadowOffsetY() const
{
    return Getter(RSPropertyType::SHADOW_OFFSET_Y, 0.f);
}

void RSShadowModifier::SetShadowAlpha(float alpha)
{
    auto color = GetShadowColor();
    // Saving alpha value on the client side and ensure it is between 0 and 1
    shadowAlpha_ = std::clamp(alpha, 0.0f, 1.0f);
    color.SetAlpha(shadowAlpha_ * UINT8_MAX);
    Setter(RSPropertyType::SHADOW_COLOR, color);
}

float RSShadowModifier::GetShadowAlpha() const
{
    return shadowAlpha_;
}

void RSShadowModifier::SetShadowElevation(float elevation)
{
    Setter(RSPropertyType::SHADOW_ELEVATION, elevation);
}

float RSShadowModifier::GetShadowElevation() const
{
    return Getter(RSPropertyType::SHADOW_ELEVATION, 0.f);
}

void RSShadowModifier::SetShadowRadius(float radius)
{
    Setter(RSPropertyType::SHADOW_RADIUS, radius);
}

float RSShadowModifier::GetShadowRadius() const
{
    return Getter(RSPropertyType::SHADOW_RADIUS, 0.f);
}

void RSShadowModifier::SetShadowPath(const std::shared_ptr<RSPath>& shadowPath)
{
    Setter<RSProperty, std::shared_ptr<RSPath>>(RSPropertyType::SHADOW_PATH, shadowPath);
}

std::shared_ptr<RSPath> RSShadowModifier::GetShadowPath() const
{
    return Getter(RSPropertyType::SHADOW_PATH, std::shared_ptr<RSPath>(nullptr));
}

void RSShadowModifier::SetShadowMask(int shadowMask)
{
    Setter<RSProperty>(RSPropertyType::SHADOW_MASK, shadowMask);
}

int RSShadowModifier::GetShadowMask() const
{
    return Getter(RSPropertyType::SHADOW_MASK, SHADOW_MASK_STRATEGY::MASK_NONE);
}

void RSShadowModifier::SetShadowColorStrategy(int shadowColorStrategy)
{
    Setter<RSProperty>(RSPropertyType::SHADOW_COLOR_STRATEGY, shadowColorStrategy);
}

int RSShadowModifier::GetShadowColorStrategy() const
{
    return Getter(RSPropertyType::SHADOW_COLOR_STRATEGY, 0);
}

void RSShadowModifier::SetShadowIsFilled(bool shadowIsFilled)
{
    Setter<RSProperty>(RSPropertyType::SHADOW_IS_FILLED, shadowIsFilled);
}

bool RSShadowModifier::GetShadowIsFilled() const
{
    return Getter(RSPropertyType::SHADOW_IS_FILLED, false);
}

void RSShadowModifier::SetShadowDisableSDFBlur(bool disable)
{
    Setter<RSProperty>(RSPropertyType::SHADOW_DISABLE_SDF_BLUR, disable);
}

bool RSShadowModifier::GetShadowDisableSDFBlur() const
{
    return Getter(RSPropertyType::SHADOW_DISABLE_SDF_BLUR, false);
}

void RSShadowModifier::SetUseShadowBatching(bool useShadowBatching)
{
    Setter<RSProperty>(RSPropertyType::USE_SHADOW_BATCHING, useShadowBatching);
}

bool RSShadowModifier::GetUseShadowBatching() const
{
    return Getter(RSPropertyType::USE_SHADOW_BATCHING, false);
}
} // namespace OHOS::Rosen::ModifierNG
