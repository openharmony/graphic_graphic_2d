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

#ifndef ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_TYPOGRAPHY_TYPES_H
#define ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_TYPOGRAPHY_TYPES_H

#include <memory>
#include <optional>
#include <vector>

#include "texgine/font_providers.h"
#include "texgine_paint.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
/*
 * @brief BreakStrategy is a strategy on how to wrap when
 *        there are multiple lines of text.
 */
enum class BreakStrategy {
    GREEDY,                  // this strategy wraps lines when they have to.
    HIGH_QUALITY,            // this strategy aims to make the line-wrapped result
                             // look as neat as possible, not jagged.
    BALANCED = HIGH_QUALITY, // to be done.
};

/*
 * @brief FontStyle is font italic style.
 */
enum class FontStyle {
    NORMAL,
    ITALIC,
    MAX,
};

/*
 * @brief FontWeight is font weight style.
 */
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
    MAX,      // use for check.
};

/*
 * @brief TextAlign is text alignment style.
 *        The bit field has an alignment style and trim style.
 */
enum class TextAlign {
    DEFAULT = 0,   // alias for Start.
    START = 1,     // LTR: align left, RTL: align right.
    END = 2,       // LTR: align right, RTL: align left.
    LEFT = 3,      // align left.
    RIGHT = 4,     // align right.
    CENTER = 5,    // center alignment.
    JUSTIFY = 6,   // justified alignment.
    SCATTERED = 7, // scatter alignment.

    // trim part
    TRIM = 0x8,    // when trim is enabled, whitespace is not considered.
    STARTTRIM = START | TRIM,
    ENDTRIM = END | TRIM,
    LEFTTRIM = LEFT | TRIM,
    RIGHTTRIM = RIGHT | TRIM,
    CENTERTRIM = CENTER | TRIM,
    JUSTIFYTRIM = JUSTIFY | TRIM,
    SCATTEREDTRIM = SCATTERED | TRIM,
};
TextAlign operator |(TextAlign lhs, TextAlign rhs);
TextAlign operator &(TextAlign lhs, TextAlign rhs);
TextAlign operator ^(TextAlign lhs, TextAlign rhs);
TextAlign operator ~(TextAlign lhs);
void operator &=(TextAlign &lhs, const TextAlign &rhs);
void operator |=(TextAlign &lhs, const TextAlign &rhs);
void operator ^=(TextAlign &lhs, const TextAlign &rhs);

/*
 * @brief TextBaseline is text baseline style.
 */
enum class TextBaseline {
    ALPHABETIC,
    IDEOGRAPHIC,
};

/*
 * @brief TextDecoration is text decoration style.
 *        Use bit fields for storage.
 */
enum class TextDecoration : int {
    NONE = 0x0,
    UNDERLINE = 0x1,
    OVERLINE = 0x2,
    LINE_THROUGH = 0x4,
    BASELINE = 0x8,
};
TextDecoration operator &(TextDecoration const &lhs, TextDecoration const &rhs);
TextDecoration operator |(TextDecoration const &lhs, TextDecoration const &rhs);
TextDecoration operator ^(TextDecoration const &lhs, TextDecoration const &rhs);
TextDecoration operator +(TextDecoration const &lhs, TextDecoration const &rhs);
void operator &=(TextDecoration &lhs, TextDecoration const &rhs);
void operator |=(TextDecoration &lhs, TextDecoration const &rhs);
void operator ^=(TextDecoration &lhs, TextDecoration const &rhs);
void operator +=(TextDecoration &lhs, TextDecoration const &rhs);

/*
 * @brief TextDecorationStyle is decoration line style.
 */
enum class TextDecorationStyle {
    SOLID,
    DOUBLE,
    DOTTED,
    DASHED,
    WAVY,
};

/*
 * @brief TextDirection is text direction style.
 */
enum class TextDirection {
    LTR, // left to right
    RTL, // right to left
};

/*
 * @brief WordBreakType is the type of word break when multiline text wraps.
 */
enum class WordBreakType {
    NORMAL,     // to be done.
    BREAK_ALL,  // break occur after any characters.
    BREAK_WORD, // break only occur after word.
};

/*
 * @brief EllipsisModal is the pattern of ellipsis.
 */
enum class EllipsisModal {
    HEAD = 0,
    MIDDLE = 1,
    TAIL = 2,
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_TYPOGRAPHY_TYPES_H
