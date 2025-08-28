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
    typedef OH_Drawing_ErrorCode (*TypographyStyleDoubleSetter)(OH_Drawing_TypographyStyle*, double);

    typedef OH_Drawing_ErrorCode (*TypographyStyleDoubleGetter)(const OH_Drawing_TypographyStyle*, double*);

    typedef OH_Drawing_ErrorCode (*TypographyStyleIntSetter)(OH_Drawing_TypographyStyle*, int);

    typedef OH_Drawing_ErrorCode (*TypographyStyleIntGetter)(const OH_Drawing_TypographyStyle*, int*);

    OH_Drawing_ErrorCode SetLineSpacing(OH_Drawing_TypographyStyle* style, double value)
    {
        reinterpret_cast<TypographyStyle*>(style)->lineSpacing = value;
        return OH_DRAWING_SUCCESS;
    }

    OH_Drawing_ErrorCode GetLineSpacing(const OH_Drawing_TypographyStyle* style, double* value)
    {
        *value = reinterpret_cast<const TypographyStyle*>(style)->lineSpacing;
        return OH_DRAWING_SUCCESS;
    }

    OH_Drawing_ErrorCode SetLineHeightMaximum(OH_Drawing_TypographyStyle* style, double value)
    {
        reinterpret_cast<TypographyStyle*>(style)->maxLineHeight = value;
        return OH_DRAWING_SUCCESS;
    }

    OH_Drawing_ErrorCode GetLineHeightMaximum(const OH_Drawing_TypographyStyle* style, double* value)
    {
        *value = reinterpret_cast<const TypographyStyle*>(style)->maxLineHeight;
        return OH_DRAWING_SUCCESS;
    }

    OH_Drawing_ErrorCode SetLineHeightMinimum(OH_Drawing_TypographyStyle* style, double value)
    {
        reinterpret_cast<TypographyStyle*>(style)->minLineHeight = value;
        return OH_DRAWING_SUCCESS;
    }

    OH_Drawing_ErrorCode GetLineHeightMinimum(const OH_Drawing_TypographyStyle* style, double* value)
    {
        *value = reinterpret_cast<const TypographyStyle*>(style)->minLineHeight;
        return OH_DRAWING_SUCCESS;
    }

    OH_Drawing_ErrorCode SetLineHeightStyle(OH_Drawing_TypographyStyle* style, int value)
    {
        if (value < 0 || value > static_cast<int>(OHOS::Rosen::LineHeightStyle::kFontHeight)) {
            return OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE;
        }
        reinterpret_cast<TypographyStyle*>(style)->lineHeightStyle =
            static_cast<OHOS::Rosen::LineHeightStyle>(value);
        return OH_DRAWING_SUCCESS;
    }

    OH_Drawing_ErrorCode GetLineHeightStyle(const OH_Drawing_TypographyStyle* style, int* value)
    {
        *value = static_cast<int>(reinterpret_cast<const TypographyStyle*>(style)->lineHeightStyle);
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
    };

    static std::unordered_map<OH_Drawing_TypographyStyleAttributeId, TypographyStyleIntGetter>
        g_typographyStyleIntGetters = {
            { TYPOGRAPHY_STYLE_ATTR_I_LINE_HEIGHT_STYLE, GetLineHeightStyle },
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
    return it->second(style, value);
}

OH_Drawing_ErrorCode OH_Drawing_GetTypographyStyleAttributeDouble(const OH_Drawing_TypographyStyle* style,
    OH_Drawing_TypographyStyleAttributeId id, double* value)
{
    if (style == nullptr || value == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    auto it = Text::g_typographyStyleDoubleGetters.find(id);
    if (it == Text::g_typographyStyleDoubleGetters.end()) {
        return OH_DRAWING_ERROR_ATTRIBUTE_ID_MISMATCH;
    }
    return it->second(style, value);
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
    return it->second(style, value);
}

OH_Drawing_ErrorCode OH_Drawing_GetTypographyStyleAttributeInt(const OH_Drawing_TypographyStyle* style,
    OH_Drawing_TypographyStyleAttributeId id, int* value)
{
    if (style == nullptr || value == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    auto it = Text::g_typographyStyleIntGetters.find(id);
    if (it == Text::g_typographyStyleIntGetters.end()) {
        return OH_DRAWING_ERROR_ATTRIBUTE_ID_MISMATCH;
    }
    return it->second(style, value);
}