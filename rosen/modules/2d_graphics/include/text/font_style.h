/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef FONT_STYLE_H
#define FONT_STYLE_H

#include <algorithm>
#include <cstdint>
#include "utils/drawing_macros.h"

template<typename T>
static constexpr const T& TPin(const T& x, const T& lo, const T& hi)
{
    return std::max(lo, std::min(x, hi));
}

namespace OHOS {
namespace Rosen {
namespace Drawing {
// BIT16 and BIT24 used to shift operation.
constexpr static int BIT16 = 16;
constexpr static int BIT24 = 24;

class DRAWING_API FontStyle {
public:
    enum Weight {
        INVISIBLE_WEIGHT    =    0,
        THIN_WEIGHT         =  100,
        EXTRA_LIGHT_WEIGHT  =  200,
        LIGHT_WEIGHT        =  300,
        NORMAL_WEIGHT       =  400,
        MEDIUM_WEIGHT       =  500,
        SEMI_BOLD_WEIGHT    =  600,
        BOLD_WEIGHT         =  700,
        EXTRA_BOLD_WEIGHT   =  800,
        BLACK_WEIGHT        =  900,
        EXTRA_BLACK_WEIGHT  = 1000,
    };

    enum Width {
        ULTRA_CONDENSED_WIDTH   = 1,
        EXTRA_CONDENSED_WIDTH   = 2,
        CONDENSED_WIDTH         = 3,
        SEMI_CONDENSED_WIDTH    = 4,
        NORMAL_WIDTH            = 5,
        SEMI_EXPANDED_WIDTH     = 6,
        EXPANDED_WIDTH          = 7,
        EXTRA_EXPANDED_WIDTH    = 8,
        ULTRA_EXPANDED_WIDTH    = 9,
    };

    enum Slant {
        UPRIGHT_SLANT,
        ITALIC_SLANT,
        OBLIQUE_SLANT,
    };

    constexpr FontStyle(int weight, int width, Slant slant) : fValue(
        (TPin<int>(weight, INVISIBLE_WEIGHT, EXTRA_BLACK_WEIGHT)) +
        (TPin<int>(width, ULTRA_CONDENSED_WIDTH, ULTRA_EXPANDED_WIDTH) << BIT16) +
        (TPin<int>(slant, UPRIGHT_SLANT, OBLIQUE_SLANT) << BIT24)) {}

    constexpr FontStyle() : FontStyle{NORMAL_WEIGHT, NORMAL_WIDTH, UPRIGHT_SLANT} {}

    bool operator==(const FontStyle& rhs) const
    {
        return fValue == rhs.fValue;
    }

    int GetWeight() const { return fValue & 0xFFFF; }
    int GetWidth() const { return (fValue >> BIT16) & 0xFF; }
    Slant GetSlant() const { return (Slant)((fValue >> BIT24) & 0xFF); }

private:
    uint32_t fValue;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif