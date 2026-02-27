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

#include "ani_enum.h"

#include "font_parser.h"
#include "text/font_types.h"
#include "text_global_config.h"
#include "typography_create.h"
#include "typography_types.h"
#include "utils/text_log.h"

namespace OHOS::Text::ANI {
using namespace OHOS::Rosen;

const std::vector<uint32_t> AniTextEnum::textAlign = {
    static_cast<uint32_t>(TextAlign::LEFT),
    static_cast<uint32_t>(TextAlign::RIGHT),
    static_cast<uint32_t>(TextAlign::CENTER),
    static_cast<uint32_t>(TextAlign::JUSTIFY),
    static_cast<uint32_t>(TextAlign::START),
    static_cast<uint32_t>(TextAlign::END),
};

const std::vector<uint32_t> AniTextEnum::textDecoration = {
    static_cast<uint32_t>(TextDecoration::NONE),
    static_cast<uint32_t>(TextDecoration::UNDERLINE),
    static_cast<uint32_t>(TextDecoration::OVERLINE),
    static_cast<uint32_t>(TextDecoration::LINE_THROUGH),
};

const std::vector<uint32_t> AniTextEnum::textDecorationStyle = {
    static_cast<uint32_t>(TextDecorationStyle::SOLID),
    static_cast<uint32_t>(TextDecorationStyle::DOUBLE),
    static_cast<uint32_t>(TextDecorationStyle::DOTTED),
    static_cast<uint32_t>(TextDecorationStyle::DASHED),
    static_cast<uint32_t>(TextDecorationStyle::WAVY),
};

const std::vector<uint32_t> AniTextEnum::fontWeight = {
    static_cast<uint32_t>(FontWeight::W100),
    static_cast<uint32_t>(FontWeight::W200),
    static_cast<uint32_t>(FontWeight::W300),
    static_cast<uint32_t>(FontWeight::W400),
    static_cast<uint32_t>(FontWeight::W500),
    static_cast<uint32_t>(FontWeight::W600),
    static_cast<uint32_t>(FontWeight::W700),
    static_cast<uint32_t>(FontWeight::W800),
    static_cast<uint32_t>(FontWeight::W900),
};

const std::vector<uint32_t> AniTextEnum::fontStyle = {
    static_cast<uint32_t>(FontStyle::NORMAL),
    static_cast<uint32_t>(FontStyle::ITALIC),
    static_cast<uint32_t>(FontStyle::OBLIQUE),
};

const std::vector<uint32_t> AniTextEnum::textBaseLine = {
    static_cast<uint32_t>(TextBaseline::ALPHABETIC),
    static_cast<uint32_t>(TextBaseline::IDEOGRAPHIC),
};

const std::vector<uint32_t> AniTextEnum::textDirection = {
    static_cast<uint32_t>(TextDirection::RTL),
    static_cast<uint32_t>(TextDirection::LTR),
};

const std::vector<uint32_t> AniTextEnum::wordBreakType = {
    static_cast<uint32_t>(WordBreakType::NORMAL),
    static_cast<uint32_t>(WordBreakType::BREAK_ALL),
    static_cast<uint32_t>(WordBreakType::BREAK_WORD),
    static_cast<uint32_t>(WordBreakType::BREAK_HYPHEN),
};

const std::vector<uint32_t> AniTextEnum::breakStrategy = {
    static_cast<uint32_t>(BreakStrategy::GREEDY),
    static_cast<uint32_t>(BreakStrategy::HIGH_QUALITY),
    static_cast<uint32_t>(BreakStrategy::BALANCED),
};

const std::vector<uint32_t> AniTextEnum::ellipsisModal = {
    static_cast<uint32_t>(EllipsisModal::HEAD),
    static_cast<uint32_t>(EllipsisModal::MIDDLE),
    static_cast<uint32_t>(EllipsisModal::TAIL),
    static_cast<uint32_t>(EllipsisModal::MULTILINE_HEAD),
    static_cast<uint32_t>(EllipsisModal::MULTILINE_MIDDLE),
};

const std::vector<uint32_t> AniTextEnum::placeHolderAlignment = {
    static_cast<uint32_t>(PlaceholderVerticalAlignment::OFFSET_AT_BASELINE),
    static_cast<uint32_t>(PlaceholderVerticalAlignment::ABOVE_BASELINE),
    static_cast<uint32_t>(PlaceholderVerticalAlignment::BELOW_BASELINE),
    static_cast<uint32_t>(PlaceholderVerticalAlignment::TOP_OF_ROW_BOX),
    static_cast<uint32_t>(PlaceholderVerticalAlignment::BOTTOM_OF_ROW_BOX),
    static_cast<uint32_t>(PlaceholderVerticalAlignment::CENTER_OF_ROW_BOX),
    static_cast<uint32_t>(PlaceholderVerticalAlignment::FOLLOW_PARAGRAPH),
};

const std::vector<uint32_t> AniTextEnum::rectWidthStyle = {
    static_cast<uint32_t>(TextRectWidthStyle::TIGHT),
    static_cast<uint32_t>(TextRectWidthStyle::MAX),
};

const std::vector<uint32_t> AniTextEnum::rectHeightStyle = {
    static_cast<uint32_t>(TextRectHeightStyle::TIGHT),
    static_cast<uint32_t>(TextRectHeightStyle::COVER_TOP_AND_BOTTOM),
    static_cast<uint32_t>(TextRectHeightStyle::COVER_HALF_TOP_AND_BOTTOM),
    static_cast<uint32_t>(TextRectHeightStyle::COVER_TOP),
    static_cast<uint32_t>(TextRectHeightStyle::COVER_BOTTOM),
    static_cast<uint32_t>(TextRectHeightStyle::FOLLOW_BY_STRUT),
};

const std::vector<uint32_t> AniTextEnum::affinity = {
    static_cast<uint32_t>(Affinity::PREV),
    static_cast<uint32_t>(Affinity::NEXT),
};

const std::vector<uint32_t> AniTextEnum::fontWidth = {
    static_cast<uint32_t>(FontWidth::ULTRA_CONDENSED),
    static_cast<uint32_t>(FontWidth::EXTRA_CONDENSED),
    static_cast<uint32_t>(FontWidth::CONDENSED),
    static_cast<uint32_t>(FontWidth::SEMI_CONDENSED),
    static_cast<uint32_t>(FontWidth::NORMAL),
    static_cast<uint32_t>(FontWidth::SEMI_EXPANDED),
    static_cast<uint32_t>(FontWidth::EXPANDED),
    static_cast<uint32_t>(FontWidth::EXTRA_EXPANDED),
    static_cast<uint32_t>(FontWidth::ULTRA_EXPANDED),
};

const std::vector<uint32_t> AniTextEnum::textHeightBehavior = {
    static_cast<uint32_t>(TextHeightBehavior::ALL),
    static_cast<uint32_t>(TextHeightBehavior::DISABLE_FIRST_ASCENT),
    static_cast<uint32_t>(TextHeightBehavior::DISABLE_LAST_ASCENT),
    static_cast<uint32_t>(TextHeightBehavior::DISABLE_ALL),
};

const std::vector<uint32_t> AniTextEnum::systemFontType = {
    static_cast<uint32_t>(TextEngine::FontParser::SystemFontType::ALL),
    static_cast<uint32_t>(TextEngine::FontParser::SystemFontType::GENERIC),
    static_cast<uint32_t>(TextEngine::FontParser::SystemFontType::STYLISH),
    static_cast<uint32_t>(TextEngine::FontParser::SystemFontType::INSTALLED),
    static_cast<uint32_t>(TextEngine::FontParser::SystemFontType::CUSTOMIZED),
};

const std::vector<uint32_t> AniTextEnum::textHighContrast = {
    static_cast<uint32_t>(SrvText::TextHighContrast::TEXT_FOLLOW_SYSTEM_HIGH_CONTRAST),
    static_cast<uint32_t>(SrvText::TextHighContrast::TEXT_APP_DISABLE_HIGH_CONTRAST),
    static_cast<uint32_t>(SrvText::TextHighContrast::TEXT_APP_ENABLE_HIGH_CONTRAST),
};

const std::vector<uint32_t> AniTextEnum::textBadgeType = {
    static_cast<uint32_t>(TextBadgeType::BADGE_NONE),
    static_cast<uint32_t>(TextBadgeType::SUPERSCRIPT),
    static_cast<uint32_t>(TextBadgeType::SUBSCRIPT),
};

const std::vector<uint32_t> AniTextEnum::textUndefinedGlyphDisplay = {
    static_cast<uint32_t>(SrvText::TextUndefinedGlyphDisplay::UNDEFINED_GLYPH_USE_DEFAULT),
    static_cast<uint32_t>(SrvText::TextUndefinedGlyphDisplay::UNDEFINED_GLYPH_USE_TOFU),
};

const std::vector<uint32_t> AniTextEnum::textVerticalAlign = {
    static_cast<uint32_t>(TextVerticalAlign::BASELINE),
    static_cast<uint32_t>(TextVerticalAlign::BOTTOM),
    static_cast<uint32_t>(TextVerticalAlign::CENTER),
    static_cast<uint32_t>(TextVerticalAlign::TOP),
};

const std::vector<uint32_t> AniTextEnum::lineHeightStyle = {
    static_cast<uint32_t>(LineHeightStyle::kFontSize),
    static_cast<uint32_t>(LineHeightStyle::kFontHeight),
};

const std::vector<uint32_t> AniTextEnum::fontEdging = {
    static_cast<uint32_t>(Drawing::FontEdging::ALIAS),
    static_cast<uint32_t>(Drawing::FontEdging::ANTI_ALIAS),
    static_cast<uint32_t>(Drawing::FontEdging::SUBPIXEL_ANTI_ALIAS),
};

std::optional<size_t> aniGetEnumIndex(const std::vector<uint32_t>& enumValues, uint32_t enumValue)
{
    auto pos = std::find(enumValues.begin(), enumValues.end(), enumValue);
    if (pos != enumValues.end()) {
        return static_cast<size_t>(std::distance(enumValues.begin(), pos));
    }
    return std::nullopt;
}
} // namespace OHOS::Text::ANI