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

#include "drawing_text_typography.h"

#include <unordered_map>

#include "rosen_text/text_style.h"

using namespace OHOS::Rosen;

namespace OHOS::Rosen::Text {
    typedef OH_Drawing_ErrorCode (*TextStyleDoubleSetter)(OH_Drawing_TextStyle*, double);

    typedef OH_Drawing_ErrorCode (*TextStyleDoubleGetter)(const OH_Drawing_TextStyle*, double*);

    typedef OH_Drawing_ErrorCode (*TextStyleIntSetter)(OH_Drawing_TextStyle*, int);

    typedef OH_Drawing_ErrorCode (*TextStyleIntGetter)(const OH_Drawing_TextStyle*, int*);

    OH_Drawing_ErrorCode SetTextStyleLineHeightMaximum(OH_Drawing_TextStyle* style, double value)
    {
        reinterpret_cast<TextStyle*>(style)->maxLineHeight = value;
        return OH_DRAWING_SUCCESS;
    }

    OH_Drawing_ErrorCode GetTextStyleLineHeightMaximum(const OH_Drawing_TextStyle* style, double* value)
    {
        *value = reinterpret_cast<const TextStyle*>(style)->maxLineHeight;
        return OH_DRAWING_SUCCESS;
    }

    OH_Drawing_ErrorCode SetTextStyleLineHeightMinimum(OH_Drawing_TextStyle* style, double value)
    {
        reinterpret_cast<TextStyle*>(style)->minLineHeight = value;
        return OH_DRAWING_SUCCESS;
    }

    OH_Drawing_ErrorCode GetTextStyleLineHeightMinimum(const OH_Drawing_TextStyle* style, double* value)
    {
        *value = reinterpret_cast<const TextStyle*>(style)->minLineHeight;
        return OH_DRAWING_SUCCESS;
    }

    OH_Drawing_ErrorCode SetTextStyleFontWidth(OH_Drawing_TextStyle* style, int value)
    {
        if (value < FONT_WIDTH_ULTRA_CONDENSED || value > FONT_WIDTH_ULTRA_EXPANDED) {
            return OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE;
        }
        reinterpret_cast<TextStyle*>(style)->fontWidth = FontWidth(value);
        return OH_DRAWING_SUCCESS;
    }

    OH_Drawing_ErrorCode GetTextStyleFontWidth(const OH_Drawing_TextStyle* style, int* value)
    {
        *value = static_cast<int>(reinterpret_cast<const TextStyle*>(style)->fontWidth);
        return OH_DRAWING_SUCCESS;
    }

    OH_Drawing_ErrorCode SetTextStyleLineHeightStyle(OH_Drawing_TextStyle* style, int value)
    {
        if (value < 0 || value > static_cast<int>(OHOS::Rosen::LineHeightStyle::kFontHeight)) {
            return OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE;
        }
        reinterpret_cast<TextStyle*>(style)->lineHeightStyle = static_cast<OHOS::Rosen::LineHeightStyle>(value);
        return OH_DRAWING_SUCCESS;
    }

    OH_Drawing_ErrorCode GetTextStyleLineHeightStyle(const OH_Drawing_TextStyle* style, int* value)
    {
        *value = static_cast<int>(reinterpret_cast<const TextStyle*>(style)->lineHeightStyle);
        return OH_DRAWING_SUCCESS;
    }

    static std::unordered_map<OH_Drawing_TextStyleAttributeId, TextStyleDoubleSetter> g_textStyleDoubleSetters = {
        { TEXT_STYLE_ATTR_D_LINE_HEIGHT_MAXIMUM, SetTextStyleLineHeightMaximum },
        { TEXT_STYLE_ATTR_D_LINE_HEIGHT_MINIMUM, SetTextStyleLineHeightMinimum },
    };

    static std::unordered_map<OH_Drawing_TextStyleAttributeId, TextStyleDoubleGetter> g_textStyleDoubleGetters = {
        { TEXT_STYLE_ATTR_D_LINE_HEIGHT_MAXIMUM, GetTextStyleLineHeightMaximum },
        { TEXT_STYLE_ATTR_D_LINE_HEIGHT_MINIMUM, GetTextStyleLineHeightMinimum },
    };

    static std::unordered_map<OH_Drawing_TextStyleAttributeId, TextStyleIntSetter> g_textStyleIntSetters = {
        { TEXT_STYLE_ATTR_I_LINE_HEIGHT_STYLE, SetTextStyleLineHeightStyle },
        { TEXT_STYLE_ATTR_I_FONT_WIDTH, SetTextStyleFontWidth },
    };

    static std::unordered_map<OH_Drawing_TextStyleAttributeId, TextStyleIntGetter> g_textStyleIntGetters = {
        { TEXT_STYLE_ATTR_I_LINE_HEIGHT_STYLE, GetTextStyleLineHeightStyle },
        { TEXT_STYLE_ATTR_I_FONT_WIDTH, GetTextStyleFontWidth },
    };
}

OH_Drawing_ErrorCode OH_Drawing_SetTextStyleAttributeDouble(OH_Drawing_TextStyle* style,
    OH_Drawing_TextStyleAttributeId id, double value)
{
    if (style == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    auto it = Text::g_textStyleDoubleSetters.find(id);
    if (it == Text::g_textStyleDoubleSetters.end()) {
        return OH_DRAWING_ERROR_ATTRIBUTE_ID_MISMATCH;
    }
    return it->second(style, value);
}

OH_Drawing_ErrorCode OH_Drawing_GetTextStyleAttributeDouble(const OH_Drawing_TextStyle* style,
    OH_Drawing_TextStyleAttributeId id, double* value)
{
    if (style == nullptr || value == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    auto it = Text::g_textStyleDoubleGetters.find(id);
    if (it == Text::g_textStyleDoubleGetters.end()) {
        return OH_DRAWING_ERROR_ATTRIBUTE_ID_MISMATCH;
    }
    return it->second(style, value);
}

OH_Drawing_ErrorCode OH_Drawing_SetTextStyleAttributeInt(OH_Drawing_TextStyle* style,
    OH_Drawing_TextStyleAttributeId id, int value)
{
    if (style == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    auto it = Text::g_textStyleIntSetters.find(id);
    if (it == Text::g_textStyleIntSetters.end()) {
        return OH_DRAWING_ERROR_ATTRIBUTE_ID_MISMATCH;
    }
    return it->second(style, value);
}

OH_Drawing_ErrorCode OH_Drawing_GetTextStyleAttributeInt(const OH_Drawing_TextStyle* style,
    OH_Drawing_TextStyleAttributeId id, int* value)
{
    if (style == nullptr || value == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    auto it = Text::g_textStyleIntGetters.find(id);
    if (it == Text::g_textStyleIntGetters.end()) {
        return OH_DRAWING_ERROR_ATTRIBUTE_ID_MISMATCH;
    }
    return it->second(style, value);
}