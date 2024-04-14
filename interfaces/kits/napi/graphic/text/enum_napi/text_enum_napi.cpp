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

#include <map>
#include <vector>
#include "text_enum_napi.h"

#include "utils/log.h"

namespace OHOS::Rosen {
struct JsEnumInt {
    std::string_view enumName;
    size_t enumInt;
};

static const std::vector<struct JsEnumInt> g_textAlign = {
    { "LEFT", static_cast<size_t>(TextAlign::LEFT) },
    { "RIGHT", static_cast<size_t>(TextAlign::RIGHT) },
    { "CENTER", static_cast<size_t>(TextAlign::CENTER) },
    { "JUSTIFY", static_cast<size_t>(TextAlign::JUSTIFY) },
    { "START", static_cast<size_t>(TextAlign::START) },
    { "END", static_cast<size_t>(TextAlign::END) },
};

static const std::vector<struct JsEnumInt> g_textDecorationStyle = {
    { "SOLID", static_cast<size_t>(TextDecorationStyle::SOLID) },
    { "DOUBLE", static_cast<size_t>(TextDecorationStyle::DOUBLE) },
    { "DOTTED", static_cast<size_t>(TextDecorationStyle::DOTTED) },
    { "DASHED", static_cast<size_t>(TextDecorationStyle::DASHED) },
    { "WAVY", static_cast<size_t>(TextDecorationStyle::WAVY) },
};

static const std::vector<struct JsEnumInt> g_fontWeight = {
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

static const std::vector<struct JsEnumInt> g_fontStyle = {
    { "NORMAL", static_cast<size_t>(FontStyle::NORMAL) },
    { "ITALIC", static_cast<size_t>(FontStyle::ITALIC) },
};

static const std::vector<struct JsEnumInt> g_textBaseline = {
    { "ALPHABETIC", static_cast<size_t>(TextBaseline::ALPHABETIC) },
    { "IDEOGRAPHIC", static_cast<size_t>(TextBaseline::IDEOGRAPHIC) },
};

static const std::vector<struct JsEnumInt> g_textDirection = {
    { "RTL", static_cast<size_t>(TextDirection::RTL) },
    { "LTR", static_cast<size_t>(TextDirection::LTR) },
};

static const std::vector<struct JsEnumInt> g_wordBreakType = {
    { "NORMAL", static_cast<size_t>(WordBreakType::NORMAL) },
    { "BREAK_ALL", static_cast<size_t>(WordBreakType::BREAK_ALL) },
    { "BREAK_WORD", static_cast<size_t>(WordBreakType::BREAK_WORD) },
};

static const std::vector<struct JsEnumInt> g_breakStrategy = {
    { "GREEDY", static_cast<size_t>(BreakStrategy::GREEDY) },
    { "HIGH_QUALITY", static_cast<size_t>(BreakStrategy::HIGH_QUALITY) },
    { "BALANCED", static_cast<size_t>(BreakStrategy::BALANCED) },
};

static const std::vector<struct JsEnumInt> g_ellipsisModal = {
    { "START", static_cast<size_t>(EllipsisModal::HEAD) },
    { "MIDDLE", static_cast<size_t>(EllipsisModal::MIDDLE) },
    { "END", static_cast<size_t>(EllipsisModal::TAIL) },
};

static const std::vector<struct JsEnumInt> g_textDecoration = {
    { "NONE", static_cast<size_t>(TextDecoration::NONE) },
    { "UNDERLINE", static_cast<size_t>(TextDecoration::UNDERLINE) },
    { "OVERLINE", static_cast<size_t>(TextDecoration::OVERLINE) },
    { "LINE_THROUGH", static_cast<size_t>(TextDecoration::LINE_THROUGH) },
};

static const std::vector<struct JsEnumInt> g_placeholderVerticalAlignment = {
    { "OFFSET_AT_BASELINE", static_cast<size_t>(PlaceholderVerticalAlignment::OFFSET_AT_BASELINE) },
    { "ABOVE_BASELINE", static_cast<size_t>(PlaceholderVerticalAlignment::ABOVE_BASELINE) },
    { "BELOW_BASELINE", static_cast<size_t>(PlaceholderVerticalAlignment::BELOW_BASELINE) },
    { "TOP_OF_ROW_BOX", static_cast<size_t>(PlaceholderVerticalAlignment::TOP_OF_ROW_BOX) },
    { "BOTTOM_OF_ROW_BOX", static_cast<size_t>(PlaceholderVerticalAlignment::BOTTOM_OF_ROW_BOX) },
    { "CENTER_OF_ROW_BOX", static_cast<size_t>(PlaceholderVerticalAlignment::CENTER_OF_ROW_BOX) },
};

static const std::vector<struct JsEnumInt> g_rectWidthStyle = {
    { "TIGHT", static_cast<size_t>(TextRectWidthStyle::TIGHT) },
    { "MAX", static_cast<size_t>(TextRectWidthStyle::MAX) },
};

static const std::vector<struct JsEnumInt> g_rectHeightStyle = {
    { "TIGHT", static_cast<size_t>(TextRectHeightStyle::TIGHT) },
    { "MAX", static_cast<size_t>(TextRectHeightStyle::COVER_TOP_AND_BOTTOM) },
    { "INCLUDE_LINE_SPACE_MIDDLE", static_cast<size_t>(TextRectHeightStyle::COVER_HALF_TOP_AND_BOTTOM) },
    { "INCLUDE_LINE_SPACE_TOP", static_cast<size_t>(TextRectHeightStyle::COVER_TOP) },
    { "INCLUDE_LINE_SPACE_BOTTOM", static_cast<size_t>(TextRectHeightStyle::COVER_BOTTOM) },
    { "STRUCT", static_cast<size_t>(TextRectHeightStyle::FOLLOW_BY_STRUT) },
};

static const std::map<std::string_view, const std::vector<struct JsEnumInt>&> g_intEnumClassMap = {
    { "TextAlign", g_textAlign },
    { "TextDecorationStyle", g_textDecorationStyle },
    { "FontWeight", g_fontWeight },
    { "FontStyle", g_fontStyle },
    { "TextBaseline", g_textBaseline },
    { "TextDirection", g_textDirection },
    { "WordBreak", g_wordBreakType },
    { "BreakStrategy", g_breakStrategy },
    { "EllipsisMode", g_ellipsisModal },
    { "TextDecorationType", g_textDecoration },
    { "PlaceholderAlignment", g_placeholderVerticalAlignment },
    { "RectWidthStyle", g_rectWidthStyle},
    { "RectHeightStyle", g_rectHeightStyle},
};

napi_value JsEnum::JsEnumIntInit(napi_env env, napi_value exports)
{
    for (auto it = g_intEnumClassMap.begin(); it != g_intEnumClassMap.end(); it++) {
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
            LOGE("napi_define_class falied");
            return nullptr;
        }

        napiStatus = napi_set_named_property(env, exports, enumClassName.data(), result);
        if (napiStatus != napi_ok) {
            LOGE("napi_set_named_property falied");
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