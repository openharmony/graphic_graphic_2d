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

#include "modifier_ng/appearance/rs_color_picker_modifier.h"

namespace OHOS::Rosen::ModifierNG {
void RSColorPickerModifier::SetColorPickerPlaceholder(ColorPlaceholder placeholder)
{
    Setter<RSProperty>(RSPropertyType::COLOR_PICKER_PLACEHOLDER, static_cast<int>(placeholder));
}

ColorPlaceholder RSColorPickerModifier::GetColorPickerPlaceholder() const
{
    return static_cast<ColorPlaceholder>(Getter(RSPropertyType::COLOR_PICKER_PLACEHOLDER, 0));
}

void RSColorPickerModifier::SetColorPickerStrategy(ColorPickStrategyType strategy)
{
    Setter<RSProperty>(RSPropertyType::COLOR_PICKER_STRATEGY, static_cast<int>(strategy));
}

ColorPickStrategyType RSColorPickerModifier::GetColorPickerStrategy() const
{
    return static_cast<ColorPickStrategyType>(Getter(RSPropertyType::COLOR_PICKER_STRATEGY, 0));
}

void RSColorPickerModifier::SetColorPickerInterval(uint64_t interval)
{
    Setter<RSProperty>(RSPropertyType::COLOR_PICKER_INTERVAL, static_cast<int>(interval));
}

uint64_t RSColorPickerModifier::GetColorPickerInterval() const
{
    return static_cast<uint64_t>(Getter(RSPropertyType::COLOR_PICKER_INTERVAL, 0));
}

void RSColorPickerModifier::SetColorPickerNotifyThreshold(uint32_t threshold)
{
    Setter<RSProperty>(RSPropertyType::COLOR_PICKER_NOTIFY_THRESHOLD, static_cast<int>(threshold));
}

uint32_t RSColorPickerModifier::GetColorPickerNotifyThreshold() const
{
    return static_cast<uint32_t>(Getter(RSPropertyType::COLOR_PICKER_NOTIFY_THRESHOLD, 0));
}

void RSColorPickerModifier::SetColorPickerRect(const Vector4f& rect)
{
    Setter<RSProperty>(RSPropertyType::COLOR_PICKER_RECT, rect);
}

Vector4f RSColorPickerModifier::GetColorPickerRect() const
{
    return Getter<Vector4f>(RSPropertyType::COLOR_PICKER_RECT, Vector4f());
}
} // namespace OHOS::Rosen::ModifierNG
