/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.. All rights reserved.
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

#ifndef ROSEN_TEXT_EXPORT_ROSEN_TEXT_TYPOGRAPHY_TYPES_H
#define ROSEN_TEXT_EXPORT_ROSEN_TEXT_TYPOGRAPHY_TYPES_H

namespace OHOS {
namespace Rosen {
enum class TextDirection {
    RTL,
    LTR,
};

enum class TextAlign {
    LEFT,
    RIGHT,
    CENTER,
    JUSTIFY,
    START,
    END,
};

enum class BreakStrategy {
    GREEDY = 0,
    HIGH_QUALITY = 1,
    BALANCED = 2
};

enum class WordBreakType {
    NORMAL = 0,
    BREAK_ALL = 1,
    BREAK_WORD = 2
};

enum TextDecoration {
    NONE = 0x0,
    UNDERLINE = 0x1,
    OVERLINE = 0x2,
    LINE_THROUGH = 0x4,
};

enum class TextDecorationStyle {
    SOLID,
    DOUBLE,
    DOTTED,
    DASHED,
    WAVY,
};

enum class FontWeight {
    W100, // thin
    W200,
    W300,
    W400, // normal
    W500,
    W600,
    W700, // bold
    W800,
    W900,
};

enum class FontStyle {
    NORMAL,
    ITALIC,
};

enum class TextBaseline {
    ALPHABETIC,
    IDEOGRAPHIC,
};

enum class EllipsisModal {
    HEAD = 0,
    MIDDLE = 1,
    TAIL = 2,
};

enum TextHeightBehavior {
    ALL = 0x0,
    DISABLE_FIRST_ASCENT = 0x1,
    DISABLE_LAST_ASCENT = 0x2,
    DISABLE_ALL = 0x1 | 0x2,
};

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_TEXT_EXPORT_ROSEN_TEXT_TYPOGRAPHY_TYPES_H
