/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.. All rights reserved.
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

#ifndef ROSEN_TEXT_PROPERTIES_PARAGRAPHY_STYLE_H
#define ROSEN_TEXT_PROPERTIES_PARAGRAPHY_STYLE_H

#include <climits>
#include <vector>
#include <string>
#include "rosen_text/properties/text_style.h"

namespace rosen {
enum class TextAlign {
    LEFT,
    RIGHT,
    CENTER,
    JUSTIFY,
    START,
    END,
};

enum class TextDirection {
    RTL,
    LTR,
};

enum class BreakStrategy {
    BreakStrategyGreedy = 0,
    BreakStrategyHighQuality = 1,
    BreakStrategyBalanced = 2
};

enum class WordBreakType {
    WordBreakTypeNormal = 0,
    WordBreakTypeBreakAll = 1,
    WordBreakTypeBreakWord = 2
};

class DRAWING_API TypographyStyle {
public:
    static const std::u16string ELLIPSIS;

    FontWeight fontWeight_ = FontWeight::W400;
    FontStyle fontStyle_ = FontStyle::NORMAL;
    std::string fontFamily_ = "";
    double fontSize_ = 14;
    double height_ = 1;
    bool hasHeightOverride_ = false;
    bool strutEnabled_ = false;

    FontWeight strutFontWeight_ = FontWeight::W400;
    FontStyle strutFontStyle_ = FontStyle::NORMAL;
    std::vector<std::string> strutFontFamilies_;
    double strutFontSize_ = 14;
    double strutHeight_ = 1;
    bool strutHasHeightOverride_ = false;
    double strutLeading_ = -1;
    bool forceStrutHeight_ = false;

    TextAlign textAlign_ = TextAlign::START;
    TextDirection textDirection_ = TextDirection::LTR;
    size_t maxLines_ = std::numeric_limits<size_t>::max();
    std::u16string ellipsis_;
    EllipsisModal ellipsisModal_ = EllipsisModal::TAIL;
    std::string locale_;

    BreakStrategy breakStrategy_ = BreakStrategy::BreakStrategyGreedy;
    WordBreakType wordBreakType_ = WordBreakType::WordBreakTypeBreakWord;

    TextStyle GetTextStyle() const;

    bool UnlimitedLines() const
    {
        return maxLines_ == std::numeric_limits<size_t>::max();
    }
    bool Ellipsized() const
    {
        return !ellipsis_.empty();
    }
    TextAlign EffectiveAlign() const;
};
} // namespace rosen
#endif // ROSEN_TEXT_PROPERTIES_PARAGRAPHY_STYLE_H
