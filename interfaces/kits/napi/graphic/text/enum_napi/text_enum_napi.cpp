/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#include "text_enum_napi.h"

#include <map>
#include <vector>
#include "font_parser.h"
#include "text_global_config.h"
#include "utils/text_log.h"

namespace OHOS::Rosen {
struct JsEnumInt {
    std::string_view enumName;
    size_t enumInt;
};

const std::vector<struct JsEnumInt> TEXT_ALIGN = {
    { "LEFT", static_cast<size_t>(TextAlign::LEFT) },
    { "RIGHT", static_cast<size_t>(TextAlign::RIGHT) },
    { "CENTER", static_cast<size_t>(TextAlign::CENTER) },
    { "JUSTIFY", static_cast<size_t>(TextAlign::JUSTIFY) },
    { "START", static_cast<size_t>(TextAlign::START) },
    { "END", static_cast<size_t>(TextAlign::END) },
};

const std::vector<struct JsEnumInt> TEXT_DECORATION_STYLE = {
    { "SOLID", static_cast<size_t>(TextDecorationStyle::SOLID) },
    { "DOUBLE", static_cast<size_t>(TextDecorationStyle::DOUBLE) },
    { "DOTTED", static_cast<size_t>(TextDecorationStyle::DOTTED) },
    { "DASHED", static_cast<size_t>(TextDecorationStyle::DASHED) },
    { "WAVY", static_cast<size_t>(TextDecorationStyle::WAVY) },
};

const std::vector<struct JsEnumInt> FONT_WEIGHT = {
    { "W100", static_cast<size_t>(FontWeight::W100) },
    { "W200", static_cast<size_t>(FontWeight::W200) },
    { "W300", static_cast<size_t>(FontWeight::W300) },
    { "W400", static_cast<size_t>(FontWeight::W400) },
    { "W500", static_cast<size_t>(FontWeight::W500) },
    { "W600", static_cast<size_t>(FontWeight::W600) },
    { "W700", static_cast<size_t>(FontWeight::W700) },
    { "W800", static_cast<size_t>(FontWeight::W800) },
    { "W900", static_cast<size_t>(FontWeight::W900) },
};

const std::vector<struct JsEnumInt> FONT_STYLE = {
    { "NORMAL", static_cast<size_t>(FontStyle::NORMAL) },
    { "ITALIC", static_cast<size_t>(FontStyle::ITALIC) },
    { "OBLIQUE", static_cast<size_t>(FontStyle::OBLIQUE) },
};

const std::vector<struct JsEnumInt> TEXT_BASE_LINE = {
    { "ALPHABETIC", static_cast<size_t>(TextBaseline::ALPHABETIC) },
    { "IDEOGRAPHIC", static_cast<size_t>(TextBaseline::IDEOGRAPHIC) },
};

const std::vector<struct JsEnumInt> TEXT_DIRECTION = {
    { "RTL", static_cast<size_t>(TextDirection::RTL) },
    { "LTR", static_cast<size_t>(TextDirection::LTR) },
};

const std::vector<struct JsEnumInt> WORD_BREAK_TYPE = {
    { "NORMAL", static_cast<size_t>(WordBreakType::NORMAL) },
    { "BREAK_ALL", static_cast<size_t>(WordBreakType::BREAK_ALL) },
    { "BREAK_WORD", static_cast<size_t>(WordBreakType::BREAK_WORD) },
    { "BREAK_HYPHEN", static_cast<size_t>(WordBreakType::BREAK_HYPHEN) },
};

const std::vector<struct JsEnumInt> BREAK_STRATEGY = {
    { "GREEDY", static_cast<size_t>(BreakStrategy::GREEDY) },
    { "HIGH_QUALITY", static_cast<size_t>(BreakStrategy::HIGH_QUALITY) },
    { "BALANCED", static_cast<size_t>(BreakStrategy::BALANCED) },
};

const std::vector<struct JsEnumInt> ELLIPSIS_MODAL = {
    { "START", static_cast<size_t>(EllipsisModal::HEAD) },
    { "MIDDLE", static_cast<size_t>(EllipsisModal::MIDDLE) },
    { "END", static_cast<size_t>(EllipsisModal::TAIL) },
};

const std::vector<struct JsEnumInt> TEXT_DECORATION = {
    { "NONE", static_cast<size_t>(TextDecoration::NONE) },
    { "UNDERLINE", static_cast<size_t>(TextDecoration::UNDERLINE) },
    { "OVERLINE", static_cast<size_t>(TextDecoration::OVERLINE) },
    { "LINE_THROUGH", static_cast<size_t>(TextDecoration::LINE_THROUGH) },
};

const std::vector<struct JsEnumInt> PLACEHOLDER_ALIGNMENT = {
    { "OFFSET_AT_BASELINE", static_cast<size_t>(PlaceholderVerticalAlignment::OFFSET_AT_BASELINE) },
    { "ABOVE_BASELINE", static_cast<size_t>(PlaceholderVerticalAlignment::ABOVE_BASELINE) },
    { "BELOW_BASELINE", static_cast<size_t>(PlaceholderVerticalAlignment::BELOW_BASELINE) },
    { "TOP_OF_ROW_BOX", static_cast<size_t>(PlaceholderVerticalAlignment::TOP_OF_ROW_BOX) },
    { "BOTTOM_OF_ROW_BOX", static_cast<size_t>(PlaceholderVerticalAlignment::BOTTOM_OF_ROW_BOX) },
    { "CENTER_OF_ROW_BOX", static_cast<size_t>(PlaceholderVerticalAlignment::CENTER_OF_ROW_BOX) },
};

const std::vector<struct JsEnumInt> RECT_WIDTH_STYLE = {
    { "TIGHT", static_cast<size_t>(TextRectWidthStyle::TIGHT) },
    { "MAX", static_cast<size_t>(TextRectWidthStyle::MAX) },
};

const std::vector<struct JsEnumInt> RECT_HEIGHT_STYLE = {
    { "TIGHT", static_cast<size_t>(TextRectHeightStyle::TIGHT) },
    { "MAX", static_cast<size_t>(TextRectHeightStyle::COVER_TOP_AND_BOTTOM) },
    { "INCLUDE_LINE_SPACE_MIDDLE", static_cast<size_t>(TextRectHeightStyle::COVER_HALF_TOP_AND_BOTTOM) },
    { "INCLUDE_LINE_SPACE_TOP", static_cast<size_t>(TextRectHeightStyle::COVER_TOP) },
    { "INCLUDE_LINE_SPACE_BOTTOM", static_cast<size_t>(TextRectHeightStyle::COVER_BOTTOM) },
    { "STRUT", static_cast<size_t>(TextRectHeightStyle::FOLLOW_BY_STRUT) },
};

static const std::vector<struct JsEnumInt> AFFINITY = {
    { "UPSTREAM", static_cast<size_t>(Affinity::PREV) },
    { "DOWNSTREAM", static_cast<size_t>(Affinity::NEXT) },
};

static const std::vector<struct JsEnumInt> FONTWIDTH = {
    { "ULTRA_CONDENSED", static_cast<size_t>(FontWidth::ULTRA_CONDENSED) },
    { "EXTRA_CONDENSED", static_cast<size_t>(FontWidth::EXTRA_CONDENSED) },
    { "CONDENSED", static_cast<size_t>(FontWidth::CONDENSED) },
    { "SEMI_CONDENSED", static_cast<size_t>(FontWidth::SEMI_CONDENSED) },
    { "NORMAL", static_cast<size_t>(FontWidth::NORMAL) },
    { "SEMI_EXPANDED", static_cast<size_t>(FontWidth::SEMI_EXPANDED) },
    { "EXPANDED", static_cast<size_t>(FontWidth::EXPANDED) },
    { "EXTRA_EXPANDED", static_cast<size_t>(FontWidth::EXTRA_EXPANDED) },
    { "ULTRA_EXPANDED", static_cast<size_t>(FontWidth::ULTRA_EXPANDED) },
};

static const std::vector<struct JsEnumInt> TEXTHEIGHTBEHAVIOR = {
    { "ALL", static_cast<size_t>(TextHeightBehavior::ALL) },
    { "DISABLE_FIRST_ASCENT", static_cast<size_t>(TextHeightBehavior::DISABLE_FIRST_ASCENT) },
    { "DISABLE_LAST_ASCENT", static_cast<size_t>(TextHeightBehavior::DISABLE_LAST_ASCENT) },
    { "DISABLE_ALL", static_cast<size_t>(TextHeightBehavior::DISABLE_ALL) },
};

static const std::vector<struct JsEnumInt> g_systemFontType = {
    { "ALL", static_cast<size_t>(TextEngine::FontParser::SystemFontType::ALL) },
    { "GENERIC", static_cast<size_t>(TextEngine::FontParser::SystemFontType::GENERIC) },
    { "STYLISH", static_cast<size_t>(TextEngine::FontParser::SystemFontType::STYLISH) },
    { "INSTALLED", static_cast<size_t>(TextEngine::FontParser::SystemFontType::INSTALLED) },
    { "CUSTOMIZED", static_cast<size_t>(TextEngine::FontParser::SystemFontType::CUSTOMIZED) },
};

static const std::vector<struct JsEnumInt> g_textHighContrast = {
    {"TEXT_FOLLOW_SYSTEM_HIGH_CONTRAST",
        static_cast<size_t>(SrvText::TextHighContrast::TEXT_FOLLOW_SYSTEM_HIGH_CONTRAST)},
    {"TEXT_APP_DISABLE_HIGH_CONTRAST",
        static_cast<size_t>(SrvText::TextHighContrast::TEXT_APP_DISABLE_HIGH_CONTRAST)},
    {"TEXT_APP_ENABLE_HIGH_CONTRAST",
        static_cast<size_t>(SrvText::TextHighContrast::TEXT_APP_ENABLE_HIGH_CONTRAST)},
};

static const std::vector<struct JsEnumInt> TEXT_BADGE_TYPE = {
    { "TEXT_BADGE_NONE", static_cast<uint32_t>(TextBadgeType::BADGE_NONE) },
    { "TEXT_SUPERSCRIPT", static_cast<uint32_t>(TextBadgeType::SUPERSCRIPT) },
    { "TEXT_SUBSCRIPT", static_cast<uint32_t>(TextBadgeType::SUBSCRIPT) },
};

static const std::vector<struct JsEnumInt> g_textNoGlyphShow = {
    { "USE_DEFAULT", static_cast<size_t>(SrvText::TextNoGlyphShow::NO_GLYPH_USE_DEFAULT) },
    { "USE_TOFU", static_cast<size_t>(SrvText::TextNoGlyphShow::NO_GLYPH_USE_TOFU) },
};

const std::map<std::string_view, const std::vector<struct JsEnumInt>&> INT_ENUM_CLASS_MAP = {
    { "TextAlign", TEXT_ALIGN },
    { "TextDecorationStyle", TEXT_DECORATION_STYLE },
    { "FontWeight", FONT_WEIGHT },
    { "FontStyle", FONT_STYLE },
    { "TextBaseline", TEXT_BASE_LINE },
    { "TextDirection", TEXT_DIRECTION },
    { "WordBreak", WORD_BREAK_TYPE },
    { "BreakStrategy", BREAK_STRATEGY },
    { "EllipsisMode", ELLIPSIS_MODAL },
    { "TextDecorationType", TEXT_DECORATION },
    { "PlaceholderAlignment", PLACEHOLDER_ALIGNMENT },
    { "RectWidthStyle", RECT_WIDTH_STYLE },
    { "RectHeightStyle", RECT_HEIGHT_STYLE },
    { "Affinity", AFFINITY },
    { "FontWidth", FONTWIDTH },
    { "TextHeightBehavior", TEXTHEIGHTBEHAVIOR },
    { "SystemFontType", g_systemFontType },
    { "TextHighContrast", g_textHighContrast},
    { "TextBadgeType", TEXT_BADGE_TYPE},
    { "TextNoGlyphShow", g_textNoGlyphShow }
};

napi_value JsEnum::JsEnumIntInit(napi_env env, napi_value exports)
{
    for (auto it = INT_ENUM_CLASS_MAP.begin(); it != INT_ENUM_CLASS_MAP.end(); it++) {
        auto &enumClassName = it->first;
        auto &enumItemVec = it->second;
        auto vecSize = enumItemVec.size();
        std::vector<napi_value> value;
        value.resize(vecSize);
        for (size_t index = 0; index < vecSize; ++index) {
            napi_create_uint32(env, enumItemVec[index].enumInt, &value[index]);
        }

        std::vector<napi_property_descriptor> property;
        property.resize(vecSize);
        for (size_t index = 0; index < vecSize; ++index) {
            property[index] = napi_property_descriptor DECLARE_NAPI_STATIC_PROPERTY(
                enumItemVec[index].enumName.data(), value[index]);
        }

        auto napiConstructor = [](napi_env env, napi_callback_info info) {
            napi_value jsThis = nullptr;
            napi_get_cb_info(env, info, nullptr, nullptr, &jsThis, nullptr);
            return jsThis;
        };

        napi_value result = nullptr;
        napi_status napiStatus = napi_define_class(env, enumClassName.data(), NAPI_AUTO_LENGTH, napiConstructor,
            nullptr, property.size(), property.data(), &result);
        if (napiStatus != napi_ok) {
            TEXT_LOGE("Failed to define class %{public}s, ret %{public}d",
                enumClassName.data(), static_cast<int>(napiStatus));
            return nullptr;
        }

        napiStatus = napi_set_named_property(env, exports, enumClassName.data(), result);
        if (napiStatus != napi_ok) {
            TEXT_LOGE("Failed to set %{public}s property, ret %{public}d",
                enumClassName.data(), static_cast<int>(napiStatus));
            return nullptr;
        }
    }
    return exports;
}

napi_value JsEnum::Init(napi_env env, napi_value exports)
{
    JsEnumIntInit(env, exports);
    return exports;
}
} // namespace OHOS::Rosen
