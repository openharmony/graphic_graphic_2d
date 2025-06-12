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

#include "modifier_ng/appearance/rs_border_modifier.h"

namespace OHOS::Rosen::ModifierNG {
void RSBorderModifier::SetBorderColor(const Vector4<Color>& color)
{
    Setter(RSPropertyType::BORDER_COLOR, color);
}

Vector4<Color> RSBorderModifier::GetBorderColor() const
{
    return Getter<Vector4<Color>>(RSPropertyType::BORDER_COLOR, {});
}

void RSBorderModifier::SetBorderWidth(const Vector4f& width)
{
    Setter(RSPropertyType::BORDER_WIDTH, width);
}

Vector4f RSBorderModifier::GetBorderWidth() const
{
    return Getter<Vector4f>(RSPropertyType::BORDER_WIDTH, {});
}

void RSBorderModifier::SetBorderStyle(const Vector4<uint32_t>& style)
{
    Setter<RSProperty>(RSPropertyType::BORDER_STYLE, style);
}

Vector4<uint32_t> RSBorderModifier::GetBorderStyle() const
{
    return Getter<Vector4<uint32_t>>(RSPropertyType::BORDER_STYLE, {});
}

void RSBorderModifier::SetBorderDashWidth(const Vector4f& dashWidth)
{
    Setter<RSProperty>(RSPropertyType::BORDER_DASH_WIDTH, dashWidth);
}

Vector4f RSBorderModifier::GetBorderDashWidth() const
{
    return Getter<Vector4f>(RSPropertyType::BORDER_DASH_WIDTH, {});
}

void RSBorderModifier::SetBorderDashGap(const Vector4f& dashGap)
{
    Setter<RSProperty>(RSPropertyType::BORDER_DASH_GAP, dashGap);
}

Vector4f RSBorderModifier::GetBorderDashGap() const
{
    return Getter<Vector4f>(RSPropertyType::BORDER_DASH_GAP, {});
}
} // namespace OHOS::Rosen::ModifierNG
