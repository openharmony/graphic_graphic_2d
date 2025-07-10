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

#include "modifier_ng/appearance/rs_outline_modifier.h"

namespace OHOS::Rosen::ModifierNG {
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
} // namespace OHOS::Rosen::ModifierNG
