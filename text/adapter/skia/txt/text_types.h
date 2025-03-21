/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.. All rights reserved.
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

#ifndef ROSEN_MODULES_SPTEXT_TEXT_TYPES_H
#define ROSEN_MODULES_SPTEXT_TEXT_TYPES_H

namespace OHOS {
namespace Rosen {
namespace SPText {
// FontStyle is font italic style.
enum class FontStyle {
    NORMAL,
    ITALIC,
    OBLIQUE,
};

// FontWeight is font weight style.
enum class FontWeight {
    W100 = 0, // weight value 100 thin.
    W200 = 1, // weight value 200.
    W300 = 2, // weight value 300.
    W400 = 3, // weight value 400 normal.
    W500 = 4, // weight value 500.
    W600 = 5, // weight value 600.
    W700 = 6, // weight value 700 bold.
    W800 = 7, // weight value 800.
    W900 = 8, // weight value 900.
};

// FontWidth is font width style.
enum class FontWidth {
    ULTRA_CONDENSED = 1,
    EXTRA_CONDENSED = 2,
    CONDENSED = 3,
    SEMI_CONDENSED = 4,
    NORMAL = 5,
    SEMI_EXPANDED = 6,
    EXPANDED = 7,
    EXTRA_EXPANDED = 8,
    ULTRA_EXPANDED = 9,
};

// TextAlign is text alignment style.
enum class TextAlign {
    LEFT,
    RIGHT,
    CENTER,
    JUSTIFY,
    START,
    END,
};

// TextBaseline is text baseline style.
enum class TextBaseline {
    ALPHABETIC,
    IDEOGRAPHIC,
};

// TextDecoration is text decoration style.
// Use bit fields for storage.
enum class TextDecoration {
    NONE = 0X0,
    UNDERLINE = 0X1,
    OVERLINE = 0X2,
    LINETHROUGH = 0X4,
};

// TextDecorationStyle is decoration line style.
enum class TextDecorationStyle {
    SOLID,
    DOUBLE,
    DOTTED,
    DASHED,
    WAVY,
};

// TextDirection is text direction style.
enum class TextDirection {
    RTL,
    LTR,
};

// Adjusts the leading over and under text.
enum TextHeightBehavior {
    ALL = 0X0,
    DISABLE_FIRST_ASCENT = 0X1,
    DISABLE_LAST_DESCENT = 0X2,
    DISABLE_ALL = 0X1 | 0X2,
};

//Add different mode ellipses in the appropriate position of the text
enum class EllipsisModal {
    HEAD = 0,
    MIDDLE = 1,
    TAIL = 2,
};
} // namespace SPText
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_SPTEXT_TEXT_TYPES_H
