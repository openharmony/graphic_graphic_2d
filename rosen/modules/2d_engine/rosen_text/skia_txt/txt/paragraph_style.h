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

#ifndef ROSEN_MODULES_SPTEXT_PARAGRAPH_STYLE_H
#define ROSEN_MODULES_SPTEXT_PARAGRAPH_STYLE_H

#include <climits>
#include <string>

#include "text_style.h"

namespace OHOS {
namespace Rosen {
namespace SPText {

enum class WordBreakType {
    NORMAL,     // to be done.
    BREAK_ALL,  // break occur after any characters.
    BREAK_WORD, // break only occur after word.
};

class ParagraphStyle {
public:
    TextStyle ConvertToTextStyle() const;
    TextAlign GetEquivalentAlign() const;

    FontWeight fontWeight = FontWeight::W400;
    FontStyle fontStyle = FontStyle::NORMAL;
    WordBreakType wordBreakType = WordBreakType::NORMAL;
    std::string fontFamily;
    double fontSize = 16;
    double height = 1;
    bool heightOverride = false;

    bool strutEnabled = false;
    FontWeight strutFontWeight = FontWeight::W400;
    FontStyle strutFontStyle = FontStyle::NORMAL;
    std::vector<std::string> strutFontFamilies;
    double strutFontSize = 16;
    double strutHeight = 1;
    bool strutHeightOverride = false;
    bool strutHalfLeading = false;
    double strutLeading = -1;
    bool forceStrutHeight = false;
    TextAlign textAlign = TextAlign::START;
    TextDirection textDirection = TextDirection::LTR;
    EllipsisModal ellipsisModal = EllipsisModal::TAIL;
    size_t maxLines = std::numeric_limits<size_t>::max();
    std::u16string ellipsis = u"\u2026";
    std::string locale;
    float textSplitRatio = 0.5;
    bool textOverflower = false;
    TextStyle spTextStyle;
    bool customSpTextStyle = false;
    TextHeightBehavior textHeightBehavior = TextHeightBehavior::ALL;
    bool hintingIsOn = true;
};
} // namespace SPText
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_SPTEXT_PARAGRAPH_STYLE_H
