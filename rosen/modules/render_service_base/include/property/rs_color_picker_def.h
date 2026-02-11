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

#ifndef RENDER_SERVICE_BASE_PROPERTY_RS_COLOR_PICKER_DEF_H
#define RENDER_SERVICE_BASE_PROPERTY_RS_COLOR_PICKER_DEF_H

#include <cstdint>
#include <optional>
#include <utility>

#include "utils/rect.h"

namespace OHOS {
namespace Rosen {
enum class ColorPlaceholder : uint8_t {
    NONE = 0,
    SURFACE,
    SURFACE_CONTRAST,
    TEXT_CONTRAST,
    ACCENT,
    FOREGROUND,
    BRAND,
    BRAND_FONT,
    WARNING,
    FONT_ON_PRIMARY,
    FONT_PRIMARY,
    FONT_SECONDARY,
    FONT_TERTIARY,
    FONT_FOURTH,
    FONT_EMPHASIZE,
    ICON_PRIMARY,
    ICON_SECONDARY,
    ICON_TERTIARY,
    ICON_FOURTH,
    ICON_EMPHASIZE,
    ICON_SUB_EMPHASIZE,
    COMP_BACKGROUND_PRIMARY_CONTRARY,
    COMP_BACKGROUND_PRIMARY_CONTRARY_SECONDARY,
    COMP_BACKGROUND_SECONDARY,
    COMP_BACKGROUND_TERTIARY,
    COMP_BACKGROUND_EMPHASIZE,
    COMP_EMPHASIZE_SECONDARY,
    COMP_EMPHASIZE_TERTIARY,
    COMP_DIVIDER,
    INTERACTIVE_HOVER,
    INTERACTIVE_FOCUS,
    INTERACTIVE_PRESSED,
    MAX = INTERACTIVE_PRESSED
};

enum class ColorPickStrategyType : int16_t {
    NONE,
    DOMINANT,
    AVERAGE,
    CONTRAST,
    CLIENT_CALLBACK, // pick average color and callback to notify client side
    MAX = CLIENT_CALLBACK
};

struct ColorPickerParam {
    ColorPlaceholder placeholder = ColorPlaceholder::NONE;
    ColorPickStrategyType strategy = ColorPickStrategyType::NONE;
    uint64_t interval = 0;
    // {darkThreshold, lightThreshold} (0-255).
    // Notify when luminance drops below darkThreshold or rises above lightThreshold.
    std::pair<uint32_t, uint32_t> notifyThreshold = { 150, 220 };
    // Optional custom rect for color picking(left, top, right, bottom)
    std::optional<Drawing::Rect> rect;

    ColorPickerParam() = default;
    ColorPickerParam(ColorPlaceholder ph, ColorPickStrategyType st, uint64_t itv)
        : placeholder(ph), strategy(st), interval(itv)
    {}

    bool operator==(const ColorPickerParam& other) const
    {
        return placeholder == other.placeholder && strategy == other.strategy && interval == other.interval &&
               notifyThreshold == other.notifyThreshold && rect == other.rect;
    }
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_PROPERTY_RS_COLOR_PICKER_DEF_H
