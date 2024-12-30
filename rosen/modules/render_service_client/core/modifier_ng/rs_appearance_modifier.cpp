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

#include "modifier_ng/rs_appearance_modifier.h"

namespace OHOS::Rosen::ModifierNG {
// ===========================================================================
// RSMaskModifier
void RSMaskModifier::SetMask(const std::shared_ptr<RSMask>& mask)
{
    Setter<RSProperty, std::shared_ptr<RSMask>>(RSPropertyType::MASK, mask);
}
const std::shared_ptr<RSMask> RSMaskModifier::GetMask() const
{
    return Getter(RSPropertyType::MASK, std::shared_ptr<RSMask>(nullptr));
}

// ===========================================================================
// RSTransitionStyleModifier

// ===========================================================================
// RSOutlineModifier
void RSOutlineModifier::SetOutlineColor(const Vector4<Color>& color)
{
    Setter(RSPropertyType::OUTLINE_COLOR, color);
}
const Vector4<Color> RSOutlineModifier::GetOutlineColor() const
{
    return Getter(RSPropertyType::OUTLINE_COLOR, Vector4<Color>(RgbPalette::Transparent()));
}
void RSOutlineModifier::SetOutlineWidth(const Vector4f& width)
{
    Setter(RSPropertyType::OUTLINE_WIDTH, width);
}
const Vector4f RSOutlineModifier::GetOutlineWidth() const
{
    return Getter(RSPropertyType::OUTLINE_WIDTH, Vector4f(0.f));
}
void RSOutlineModifier::SetOutlineStyle(const Vector4<uint32_t>& style)
{
    Setter<RSProperty>(RSPropertyType::OUTLINE_STYLE, style);
}
const Vector4<uint32_t> RSOutlineModifier::GetOutlineStyle() const
{
    return Getter(RSPropertyType::OUTLINE_STYLE, Vector4<uint32_t>());
}
void RSOutlineModifier::SetOutlineDashWidth(const Vector4f& dashWidth)
{
    Setter<RSProperty>(RSPropertyType::OUTLINE_DASH_WIDTH, dashWidth);
}
const Vector4f RSOutlineModifier::GetOutlineDashWidth() const
{
    return Getter(RSPropertyType::OUTLINE_DASH_WIDTH, Vector4f(0.f));
}
void RSOutlineModifier::SetOutlineDashGap(const Vector4f& dashGap)
{
    Setter<RSProperty>(RSPropertyType::OUTLINE_DASH_GAP, dashGap);
}
const Vector4f RSOutlineModifier::GetOutlineDashGap() const
{
    return Getter(RSPropertyType::OUTLINE_DASH_GAP, Vector4f(0.f));
}
void RSOutlineModifier::SetOutlineRadius(const Vector4f& radius)
{
    Setter(RSPropertyType::OUTLINE_RADIUS, radius);
}
const Vector4f RSOutlineModifier::GetOutlineRadius() const
{
    return Getter(RSPropertyType::OUTLINE_RADIUS, Vector4f(0.f));
}

// ===========================================================================
// RSShadowModifier
void RSShadowModifier::SetShadowColor(Color color)
{
    Setter(RSPropertyType::SHADOW_COLOR, color);
}
Color RSShadowModifier::GetShadowColor() const
{
    return Getter(RSPropertyType::SHADOW_COLOR, Color(RgbPalette::Transparent()));
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
    Setter(RSPropertyType::SHADOW_ALPHA, alpha);
}
float RSShadowModifier::GetShadowAlpha() const
{
    return Getter(RSPropertyType::SHADOW_ALPHA, 0.f);
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
void RSShadowModifier::SetShadowMask(bool shadowMask)
{
    Setter<RSProperty>(RSPropertyType::SHADOW_MASK, shadowMask);
}
bool RSShadowModifier::GetShadowMask() const
{
    return Getter(RSPropertyType::SHADOW_MASK, false);
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
void RSShadowModifier::SetUseShadowBatching(bool useShadowBatching)
{
    Setter<RSProperty>(RSPropertyType::USE_SHADOW_BATCHING, useShadowBatching);
}
bool RSShadowModifier::GetUseShadowBatching() const
{
    return Getter(RSPropertyType::USE_SHADOW_BATCHING, false);
}
} // namespace OHOS::Rosen::ModifierNG