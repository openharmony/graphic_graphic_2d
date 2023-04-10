/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef ROSEN_MODULES_TEXGINE_SRC_FONT_STYLES_H
#define ROSEN_MODULES_TEXGINE_SRC_FONT_STYLES_H

#include <texgine/typography_types.h>

#include "texgine_font_style.h"

namespace Texgine {
class FontStyles {
public:
    enum class Weight {
        INVISIBLE = 0,
        THIN = 1,
        EXTRALIGHT = 2,
        LIGHT = 3,
        NORMAL = 4,
        MEDIUM = 5,
        SEMIBOLD = 6,
        BOLD = 7,
        EXTRABOLD = 8,
        BLACK = 9,
        EXTRABLACK = 10,
        MAX,
    };

    enum class Width {
        ULTRACONDENSED = 0,
        EXTRACONDENSED = 1,
        CONDENSED = 2,
        SEMICONDENSED = 3,
        NORMAL = 4,
        SEMIEXPANDED = 5,
        EXPANDED = 6,
        EXTRAEXPENDED = 7,
        ULTRAEXPANDED = 8,
        MAX,
    };

    enum class Slant {
        UPRIGHT = 0,
        ITALIC = 1,
        OBLIQUE = 2,
        MAX,
    };

    FontStyles() = default;
    FontStyles(FontWeight weight, FontStyle style);
    FontStyles(Weight weight, Width width, Slant slant);

    TexgineFontStyle ToTexgineFontStyle() const;

    bool operator ==(const FontStyles &rhs) const;
    bool operator !=(const FontStyles &rhs) const;
    bool operator <(const FontStyles& rhs) const;

private:
    Weight weight_ = Weight::NORMAL;
    Width width_ = Width::NORMAL;
    Slant slant_ = Slant::UPRIGHT;
};
} // namespace Texgine

#endif // ROSEN_MODULES_TEXGINE_SRC_FONT_STYLES_H
