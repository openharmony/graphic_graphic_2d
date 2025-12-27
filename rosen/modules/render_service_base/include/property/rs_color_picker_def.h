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

namespace OHOS {
namespace Rosen {
enum class ColorPlaceholder : uint8_t {
    NONE = 0,
    SURFACE,
    SURFACE_CONTRAST,
    TEXT_CONTRAST,
    ACCENT,
    FOREGROUND,
    MAX = FOREGROUND
};

enum class ColorPickStrategyType : int16_t { NONE, DOMINANT, AVERAGE, CONTRAST, MAX = CONTRAST };

struct ColorPickerParam {
    ColorPlaceholder placeholder = ColorPlaceholder::NONE;
    ColorPickStrategyType strategy = ColorPickStrategyType::NONE;
    uint64_t interval = 0;

    ColorPickerParam() = default;
    ColorPickerParam(ColorPlaceholder ph, ColorPickStrategyType st, uint64_t itv)
        : placeholder(ph), strategy(st), interval(itv) {}

    bool operator==(const ColorPickerParam& other) const
    {
        return placeholder == other.placeholder && strategy == other.strategy && interval == other.interval;
    }
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_PROPERTY_RS_COLOR_PICKER_DEF_H
