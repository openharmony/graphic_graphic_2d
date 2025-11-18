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

#include <map>
#include <string_view>

#include "ani_common.h"
#include "typography_types.h"

namespace OHOS::Text::ANI {
static const std::vector<uint32_t> TEXT_ALIGN = {
    static_cast<uint32_t>(TextAlign::LEFT),
    static_cast<uint32_t>(TextAlign::RIGHT),
    static_cast<uint32_t>(TextAlign::CENTER),
    static_cast<uint32_t>(TextAlign::JUSTIFY),
    static_cast<uint32_t>(TextAlign::START),
    static_cast<uint32_t>(TextAlign::END),
};

static const std::vector<uint32_t> TEXT_DECORATION_STYLE = {
    static_cast<uint32_t>(TextDecorationStyle::SOLID),
    static_cast<uint32_t>(TextDecorationStyle::DOUBLE),
    static_cast<uint32_t>(TextDecorationStyle::DOTTED),
    static_cast<uint32_t>(TextDecorationStyle::DASHED),
    static_cast<uint32_t>(TextDecorationStyle::WAVY),
};

static const std::vector<uint32_t> FONT_WEIGHT = {
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

static const std::vector<uint32_t> FONT_STYLE = {
    static_cast<uint32_t>(FontStyle::NORMAL),
    static_cast<uint32_t>(FontStyle::ITALIC),
    static_cast<uint32_t>(FontStyle::OBLIQUE),
};

static const std::vector<uint32_t> TEXT_BASE_LINE = {
    static_cast<uint32_t>(TextBaseline::ALPHABETIC),
    static_cast<uint32_t>(TextBaseline::IDEOGRAPHIC),
};

static const std::vector<uint32_t> TEXT_DIRECTION = {
    static_cast<uint32_t>(TextDirection::RTL),
    static_cast<uint32_t>(TextDirection::LTR),
};

static const std::vector<uint32_t> WORD_BREAK_TYPE = {
    static_cast<uint32_t>(WordBreakType::NORMAL),
    static_cast<uint32_t>(WordBreakType::BREAK_ALL),
    static_cast<uint32_t>(WordBreakType::BREAK_WORD),
    static_cast<uint32_t>(WordBreakType::BREAK_HYPHEN),
};

static const std::vector<uint32_t> BREAK_STRATEGY = {
    static_cast<uint32_t>(BreakStrategy::GREEDY),
    static_cast<uint32_t>(BreakStrategy::HIGH_QUALITY),
    static_cast<uint32_t>(BreakStrategy::BALANCED),
};

static const std::vector<uint32_t> ELLIPSIS_MODAL = {
    static_cast<uint32_t>(EllipsisModal::HEAD),
    static_cast<uint32_t>(EllipsisModal::MIDDLE),
    static_cast<uint32_t>(EllipsisModal::TAIL),
};

static const std::vector<uint32_t> TEXT_DECORATION = {
    static_cast<uint32_t>(TextDecoration::NONE),
    static_cast<uint32_t>(TextDecoration::UNDERLINE),
    static_cast<uint32_t>(TextDecoration::OVERLINE),
    static_cast<uint32_t>(TextDecoration::LINE_THROUGH),
};

static const std::vector<uint32_t> PLACEHOLDER_ALIGNMENT = {
    static_cast<uint32_t>(PlaceholderVerticalAlignment::OFFSET_AT_BASELINE),
    static_cast<uint32_t>(PlaceholderVerticalAlignment::ABOVE_BASELINE),
    static_cast<uint32_t>(PlaceholderVerticalAlignment::BELOW_BASELINE),
    static_cast<uint32_t>(PlaceholderVerticalAlignment::TOP_OF_ROW_BOX),
    static_cast<uint32_t>(PlaceholderVerticalAlignment::BOTTOM_OF_ROW_BOX),
    static_cast<uint32_t>(PlaceholderVerticalAlignment::CENTER_OF_ROW_BOX),
    static_cast<uint32_t>(PlaceholderVerticalAlignment::FOLLOW_PARAGRAPH),
};

static const std::vector<uint32_t> RECT_WIDTH_STYLE = {
    static_cast<uint32_t>(TextRectWidthStyle::TIGHT),
    static_cast<uint32_t>(TextRectWidthStyle::MAX),
};

static const std::vector<uint32_t> RECT_HEIGHT_STYLE = {
    static_cast<uint32_t>(TextRectHeightStyle::TIGHT),
    static_cast<uint32_t>(TextRectHeightStyle::COVER_TOP_AND_BOTTOM),
    static_cast<uint32_t>(TextRectHeightStyle::COVER_HALF_TOP_AND_BOTTOM),
    static_cast<uint32_t>(TextRectHeightStyle::COVER_TOP),
    static_cast<uint32_t>(TextRectHeightStyle::COVER_BOTTOM),
    static_cast<uint32_t>(TextRectHeightStyle::FOLLOW_BY_STRUT),
};

static const std::vector<uint32_t> AFFINITY = {
    static_cast<uint32_t>(Affinity::PREV),
    static_cast<uint32_t>(Affinity::NEXT),
};

static const std::vector<uint32_t> FONTWIDTH = {
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

static const std::vector<uint32_t> TEXTHEIGHTBEHAVIOR = {
    static_cast<uint32_t>(TextHeightBehavior::ALL),
    static_cast<uint32_t>(TextHeightBehavior::DISABLE_FIRST_ASCENT),
    static_cast<uint32_t>(TextHeightBehavior::DISABLE_LAST_ASCENT),
    static_cast<uint32_t>(TextHeightBehavior::DISABLE_ALL),
};

static const std::vector<uint32_t> SYSTEM_FONT_TYPE = {
    static_cast<uint32_t>(TextEngine::FontParser::SystemFontType::ALL),
    static_cast<uint32_t>(TextEngine::FontParser::SystemFontType::GENERIC),
    static_cast<uint32_t>(TextEngine::FontParser::SystemFontType::STYLISH),
    static_cast<uint32_t>(TextEngine::FontParser::SystemFontType::INSTALLED),
    static_cast<uint32_t>(TextEngine::FontParser::SystemFontType::CUSTOMIZED),
};

static const std::vector<uint32_t> TEXT_HIGH_CONTRAST = {
    static_cast<uint32_t>(SrvText::TextHighContrast::TEXT_FOLLOW_SYSTEM_HIGH_CONTRAST),
    static_cast<uint32_t>(SrvText::TextHighContrast::TEXT_APP_DISABLE_HIGH_CONTRAST),
    static_cast<uint32_t>(SrvText::TextHighContrast::TEXT_APP_ENABLE_HIGH_CONTRAST),
};

static const std::vector<uint32_t> TEXT_BADGE_TYPE = {
    static_cast<uint32_t>(TextBadgeType::BADGE_NONE),
    static_cast<uint32_t>(TextBadgeType::SUPERSCRIPT),
    static_cast<uint32_t>(TextBadgeType::SUBSCRIPT),
};

static const std::vector<uint32_t> TEXT_UNDEFINED_GLYPH_DISPLAY = {
    static_cast<uint32_t>(SrvText::TextUndefinedGlyphDisplay::UNDEFINED_GLYPH_USE_DEFAULT),
    static_cast<uint32_t>(SrvText::TextUndefinedGlyphDisplay::UNDEFINED_GLYPH_USE_TOFU),
};

static const std::vector<uint32_t> TEXT_VERTICAL_ALIGN = {
    static_cast<uint32_t>(TextVerticalAlign::BASELINE),
    static_cast<uint32_t>(TextVerticalAlign::BOTTOM),
    static_cast<uint32_t>(TextVerticalAlign::CENTER),
    static_cast<uint32_t>(TextVerticalAlign::TOP),
};

static const std::map<std::string_view, const std::vector<uint32_t>&> INT_ENUM_CLASS_MAP = {
    { ANI_ENUM_TEXT_ALIGN, TEXT_ALIGN },
    { ANI_ENUM_TEXT_DECORATION_STYLE, TEXT_DECORATION_STYLE },
    { ANI_ENUM_FONT_WEIGHT, FONT_WEIGHT },
    { ANI_ENUM_FONT_STYLE, FONT_STYLE },
    { ANI_ENUM_TEXT_BASE_LINE, TEXT_BASE_LINE },
    { ANI_ENUM_TEXT_DIRECTION, TEXT_DIRECTION },
    { ANI_ENUM_WORD_BREAK, WORD_BREAK_TYPE },
    { ANI_ENUM_BREAK_STRATEGY, BREAK_STRATEGY },
    { ANI_ENUM_ELLIPSIS_MODE, ELLIPSIS_MODAL },
    { ANI_ENUM_TEXT_DECORATION, TEXT_DECORATION },
    { ANI_ENUM_PLACEHOLDER_ALIGNMENT, PLACEHOLDER_ALIGNMENT },
    { ANI_ENUM_RECT_WIDTH_STYLE, RECT_WIDTH_STYLE },
    { ANI_ENUM_RECT_HEIGHT_STYLE, RECT_HEIGHT_STYLE },
    { ANI_ENUM_AFFINITY, AFFINITY },
    { ANI_ENUM_FONT_WIDTH, FONTWIDTH },
    { ANI_ENUM_TEXT_HEIGHT_BEHAVIOR, TEXTHEIGHTBEHAVIOR },
    { ANI_ENUM_SYSTEM_FONT_TYPE, SYSTEM_FONT_TYPE },
    { ANI_ENUM_TEXT_HIGH_CONTRAST, TEXT_HIGH_CONTRAST },
    { ANI_ENUM_TEXT_BADGE_TYPE, TEXT_BADGE_TYPE },
    { ANI_ENUM_TEXT_UNDEFINED_GLYPH_DISPLAY, TEXT_UNDEFINED_GLYPH_DISPLAY },
    { ANI_ENUM_TEXT_VERTICAL_ALIGN, TEXT_VERTICAL_ALIGN },
};

uint32_t getEnumValue(std::string_view enumName, size_t enumIndex)
{
    auto it = INT_ENUM_CLASS_MAP.find(enumName);
    if (it != INT_ENUM_CLASS_MAP.end() && enumIndex < it->second.size()) {
        return it->second[enumIndex];
    }
    return 0;
}
size_t getEnumIndex(std::string_view enumName, uint32_t enumValue)
{
    auto it = INT_ENUM_CLASS_MAP.find(enumName);
    if (it != INT_ENUM_CLASS_MAP.end()) {
        const auto& values = it->second;
        auto pos = std::find(values.begin(), values.end(), enumValue);
        if (pos != values.end()) {
            return static_cast<size_t>(std::distance(values.begin(), pos));
        }
    }
    return -1;
}
} // namespace OHOS::Text::ANI