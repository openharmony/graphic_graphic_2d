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

#include "typography_style.h"

using namespace OHOS::Rosen;

namespace OHOS::Rosen::Text {
    typedef OH_Drawing_ErrorCode (*TypographyStyleDoubleSetter)(TypographyStyle*, double);

    typedef OH_Drawing_ErrorCode (*TypographyStyleDoubleGetter)(const TypographyStyle*, double*);

    typedef OH_Drawing_ErrorCode (*TypographyStyleIntSetter)(TypographyStyle*, int);

    typedef OH_Drawing_ErrorCode (*TypographyStyleIntGetter)(const TypographyStyle*, int*);

    typedef OH_Drawing_ErrorCode (*TypographyStyleBoolSetter)(TypographyStyle*, bool);

    typedef OH_Drawing_ErrorCode (*TypographyStyleBoolGetter)(const TypographyStyle*, bool*);

    OH_Drawing_ErrorCode SetLineSpacing(TypographyStyle* style, double value)
    {
        style->lineSpacing = value;
        return OH_DRAWING_SUCCESS;
    }

    OH_Drawing_ErrorCode GetLineSpacing(const TypographyStyle* style, double* value)
    {
        *value = style->lineSpacing;
        return OH_DRAWING_SUCCESS;
    }

    OH_Drawing_ErrorCode SetLineHeightMaximum(TypographyStyle* style, double value)
    {
        style->maxLineHeight = value;
        return OH_DRAWING_SUCCESS;
    }

    OH_Drawing_ErrorCode GetLineHeightMaximum(const TypographyStyle* style, double* value)
    {
        *value = style->maxLineHeight;
        return OH_DRAWING_SUCCESS;
    }

    OH_Drawing_ErrorCode SetLineHeightMinimum(TypographyStyle* style, double value)
    {
        style->minLineHeight = value;
        return OH_DRAWING_SUCCESS;
    }

    OH_Drawing_ErrorCode GetLineHeightMinimum(const TypographyStyle* style, double* value)
    {
        *value = style->minLineHeight;
        return OH_DRAWING_SUCCESS;
    }

    OH_Drawing_ErrorCode SetLineHeightStyle(TypographyStyle* style, int value)
    {
        if (value < 0 || value > static_cast<int>(OHOS::Rosen::LineHeightStyle::kFontHeight)) {
            return OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE;
        }
        style->lineHeightStyle = static_cast<OHOS::Rosen::LineHeightStyle>(value);
        return OH_DRAWING_SUCCESS;
    }

    OH_Drawing_ErrorCode GetLineHeightStyle(const TypographyStyle* style, int* value)
    {
        *value = static_cast<int>(style->lineHeightStyle);
        return OH_DRAWING_SUCCESS;
    }

    OH_Drawing_ErrorCode SetTypographyStyleFontWidth(TypographyStyle* style, int value)
    {
        if (value < FONT_WIDTH_ULTRA_CONDENSED || value > FONT_WIDTH_ULTRA_EXPANDED) {
            return OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE;
        }
        style->fontWidth = FontWidth(value);
        return OH_DRAWING_SUCCESS;
    }

    OH_Drawing_ErrorCode GetTypographyStyleFontWidth(const TypographyStyle* style, int* value)
    {
        *value = static_cast<int>(style->fontWidth);
        return OH_DRAWING_SUCCESS;
    }
    
    OH_Drawing_ErrorCode SetCompressHeadPunctuation(TypographyStyle* style, bool value)
    {
        style->compressHeadPunctuation = value;
        return OH_DRAWING_SUCCESS;
    }

    OH_Drawing_ErrorCode GetCompressHeadPunctuation(const TypographyStyle* style, bool* value)
    {
        *value = static_cast<int>(style->compressHeadPunctuation);
        return OH_DRAWING_SUCCESS;
    }

    static std::unordered_map<OH_Drawing_TypographyStyleAttributeId, TypographyStyleDoubleSetter>
        g_typographyStyleDoubleSetters = {
            { TYPOGRAPHY_STYLE_ATTR_D_LINE_HEIGHT_MAXIMUM, SetLineHeightMaximum },
            { TYPOGRAPHY_STYLE_ATTR_D_LINE_HEIGHT_MINIMUM, SetLineHeightMinimum },
            { TYPOGRAPHY_STYLE_ATTR_D_LINE_SPACING, SetLineSpacing },
        };

    static std::unordered_map<OH_Drawing_TypographyStyleAttributeId, TypographyStyleDoubleGetter>
        g_typographyStyleDoubleGetters = {
            { TYPOGRAPHY_STYLE_ATTR_D_LINE_HEIGHT_MAXIMUM, GetLineHeightMaximum },
            { TYPOGRAPHY_STYLE_ATTR_D_LINE_HEIGHT_MINIMUM, GetLineHeightMinimum },
            { TYPOGRAPHY_STYLE_ATTR_D_LINE_SPACING, GetLineSpacing },
    };

    static std::unordered_map<OH_Drawing_TypographyStyleAttributeId, TypographyStyleIntSetter>
        g_typographyStyleIntSetters = {
            { TYPOGRAPHY_STYLE_ATTR_I_LINE_HEIGHT_STYLE, SetLineHeightStyle },
            { TYPOGRAPHY_STYLE_ATTR_I_FONT_WIDTH, SetTypographyStyleFontWidth},
    };

    static std::unordered_map<OH_Drawing_TypographyStyleAttributeId, TypographyStyleIntGetter>
        g_typographyStyleIntGetters = {
            { TYPOGRAPHY_STYLE_ATTR_I_LINE_HEIGHT_STYLE, GetLineHeightStyle },
            { TYPOGRAPHY_STYLE_ATTR_I_FONT_WIDTH, GetTypographyStyleFontWidth},
    };

    static std::unordered_map<OH_Drawing_TypographyStyleAttributeId, TypographyStyleBoolSetter>
        g_typographyStyleBoolSetters = {
            { TYPOGRAPHY_STYLE_ATTR_B_COMPRESS_HEAD_PUNCTUATION, SetCompressHeadPunctuation },
    };

    static std::unordered_map<OH_Drawing_TypographyStyleAttributeId, TypographyStyleBoolGetter>
        g_typographyStyleBoolGetters = {
            { TYPOGRAPHY_STYLE_ATTR_B_COMPRESS_HEAD_PUNCTUATION, GetCompressHeadPunctuation },
    };
}

OH_Drawing_ErrorCode OH_Drawing_SetTypographyStyleAttributeDouble(OH_Drawing_TypographyStyle* style,
    OH_Drawing_TypographyStyleAttributeId id, double value)
{
    if (style == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    auto it = Text::g_typographyStyleDoubleSetters.find(id);
    if (it == Text::g_typographyStyleDoubleSetters.end()) {
        return OH_DRAWING_ERROR_ATTRIBUTE_ID_MISMATCH;
    }
    return it->second(reinterpret_cast<TypographyStyle*>(style), value);
}

OH_Drawing_ErrorCode OH_Drawing_GetTypographyStyleAttributeDouble(OH_Drawing_TypographyStyle* style,
    OH_Drawing_TypographyStyleAttributeId id, double* value)
{
    if (style == nullptr || value == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    auto it = Text::g_typographyStyleDoubleGetters.find(id);
    if (it == Text::g_typographyStyleDoubleGetters.end()) {
        return OH_DRAWING_ERROR_ATTRIBUTE_ID_MISMATCH;
    }
    return it->second(reinterpret_cast<const TypographyStyle*>(style), value);
}

OH_Drawing_ErrorCode OH_Drawing_SetTypographyStyleAttributeInt(OH_Drawing_TypographyStyle* style,
    OH_Drawing_TypographyStyleAttributeId id, int value)
{
    if (style == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    auto it = Text::g_typographyStyleIntSetters.find(id);
    if (it == Text::g_typographyStyleIntSetters.end()) {
        return OH_DRAWING_ERROR_ATTRIBUTE_ID_MISMATCH;
    }
    return it->second(reinterpret_cast<TypographyStyle*>(style), value);
}

OH_Drawing_ErrorCode OH_Drawing_GetTypographyStyleAttributeInt(OH_Drawing_TypographyStyle* style,
    OH_Drawing_TypographyStyleAttributeId id, int* value)
{
    if (style == nullptr || value == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    auto it = Text::g_typographyStyleIntGetters.find(id);
    if (it == Text::g_typographyStyleIntGetters.end()) {
        return OH_DRAWING_ERROR_ATTRIBUTE_ID_MISMATCH;
    }
    return it->second(reinterpret_cast<const TypographyStyle*>(style), value);
}

OH_Drawing_ErrorCode OH_Drawing_SetTypographyStyleAttributeBool(OH_Drawing_TypographyStyle* style,
    OH_Drawing_TypographyStyleAttributeId id, bool value)
{
    if (style == nullptr) {
        return OH_DRAWING_ERROR_INCORRECT_PARAMETER;
    }
    auto it = Text::g_typographyStyleBoolSetters.find(id);
    if (it == Text::g_typographyStyleBoolSetters.end()) {
        return OH_DRAWING_ERROR_ATTRIBUTE_ID_MISMATCH;
    }
    return it->second(reinterpret_cast<TypographyStyle*>(style), value);
}

OH_Drawing_ErrorCode OH_Drawing_GetTypographyStyleAttributeBool(OH_Drawing_TypographyStyle* style,
    OH_Drawing_TypographyStyleAttributeId id, bool* value)
{
    if (style == nullptr || value == nullptr) {
        return OH_DRAWING_ERROR_INCORRECT_PARAMETER;
    }
    auto it = Text::g_typographyStyleBoolGetters.find(id);
    if (it == Text::g_typographyStyleBoolGetters.end()) {
        return OH_DRAWING_ERROR_ATTRIBUTE_ID_MISMATCH;
    }
    return it->second(reinterpret_cast<const TypographyStyle*>(style), value);
}