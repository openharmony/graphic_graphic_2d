/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "c/drawing_text_typography.h"

#include "rosen_text/ui/font_collection.h"
#include "rosen_text/ui/typography.h"
#include "rosen_text/ui/typography_create.h"

#include <codecvt>
#include <vector>
#include <string>

using namespace rosen;

static FontCollection* ConvertToOriginalText(OH_Drawing_FontCollection* fontCollection)
{
    return reinterpret_cast<FontCollection*>(fontCollection);
}

static TypographyStyle* ConvertToOriginalText(OH_Drawing_TypographyStyle* style)
{
    return reinterpret_cast<TypographyStyle*>(style);
}

static TypographyCreate* ConvertToOriginalText(OH_Drawing_TypographyCreate* handler)
{
    return reinterpret_cast<TypographyCreate*>(handler);
}

static OH_Drawing_TypographyCreate* ConvertToNDKText(TypographyCreate* handler)
{
    return reinterpret_cast<OH_Drawing_TypographyCreate*>(handler);
}

static TextStyle* ConvertToOriginalText(OH_Drawing_TextStyle* style)
{
    return reinterpret_cast<TextStyle*>(style);
}

static Typography* ConvertToOriginalText(OH_Drawing_Typography* typography)
{
    return reinterpret_cast<Typography*>(typography);
}

static OH_Drawing_Typography* ConvertToNDKText(Typography* typography)
{
    return reinterpret_cast<OH_Drawing_Typography*>(typography);
}

OH_Drawing_TypographyStyle* OH_Drawing_CreateTypographyStyle(void)
{
    return (OH_Drawing_TypographyStyle*)new TypographyStyle;
}

void OH_Drawing_DestroyTypographyStyle(OH_Drawing_TypographyStyle* style)
{
    delete ConvertToOriginalText(style);
}

void OH_Drawing_SetTypographyTextDirection(OH_Drawing_TypographyStyle* style, int direction)
{
    TextDirection textDirection;
    switch (direction) {
        case TEXT_DIRECTION_RTL: {
            textDirection = TextDirection::RTL;
            break;
        }
        case TEXT_DIRECTION_LTR: {
            textDirection = TextDirection::LTR;
            break;
        }
        default: {
            textDirection = TextDirection::LTR;
            break;
        }
    }
    ConvertToOriginalText(style)->textDirection_ = textDirection;
}

void OH_Drawing_SetTypographyTextAlign(OH_Drawing_TypographyStyle* style, int align)
{
    TextAlign textAlign;
    switch (align) {
        case TEXT_ALIGN_LEFT: {
            textAlign = TextAlign::LEFT;
            break;
        }
        case TEXT_ALIGN_RIGHT: {
            textAlign = TextAlign::RIGHT;
            break;
        }
        case TEXT_ALIGN_CENTER: {
            textAlign = TextAlign::CENTER;
            break;
        }
        case TEXT_ALIGN_JUSTIFY: {
            textAlign = TextAlign::JUSTIFY;
            break;
        }
        case TEXT_ALIGN_START: {
            textAlign = TextAlign::START;
            break;
        }
        case TEXT_ALIGN_END: {
            textAlign = TextAlign::END;
            break;
        }
        default: {
            textAlign = TextAlign::LEFT;
        }
    }
    ConvertToOriginalText(style)->textAlign_ = textAlign;
}

void OH_Drawing_SetTypographyTextMaxLines(OH_Drawing_TypographyStyle* style, int lineNumber)
{
    ConvertToOriginalText(style)->maxLines_ = static_cast<size_t>(lineNumber);
}

OH_Drawing_TextStyle* OH_Drawing_CreateTextStyle(void)
{
    return (OH_Drawing_TextStyle*)new TextStyle;
}

void OH_Drawing_DestroyTextStyle(OH_Drawing_TextStyle* style)
{
    delete ConvertToOriginalText(style);
}

void OH_Drawing_SetTextStyleColor(OH_Drawing_TextStyle* style, uint32_t color)
{
    ConvertToOriginalText(style)->color_.SetColorQuad(color);
}

void OH_Drawing_SetTextStyleFontSize(OH_Drawing_TextStyle* style, double fontSize)
{
    ConvertToOriginalText(style)->fontSize_ = fontSize;
}

void OH_Drawing_SetTextStyleFontWeight(OH_Drawing_TextStyle* style, int fontWeight)
{
    FontWeight rosenFontWeight;
    switch (fontWeight) {
        case FONT_WEIGHT_100: {
            rosenFontWeight = FontWeight::W100;
            break;
        }
        case FONT_WEIGHT_200: {
            rosenFontWeight = FontWeight::W200;
            break;
        }
        case FONT_WEIGHT_300: {
            rosenFontWeight = FontWeight::W300;
            break;
        }
        case FONT_WEIGHT_400: {
            rosenFontWeight = FontWeight::W400;
            break;
        }
        case FONT_WEIGHT_500: {
            rosenFontWeight = FontWeight::W500;
            break;
        }
        case FONT_WEIGHT_600: {
            rosenFontWeight = FontWeight::W600;
            break;
        }
        case FONT_WEIGHT_700: {
            rosenFontWeight = FontWeight::W700;
            break;
        }
        case FONT_WEIGHT_800: {
            rosenFontWeight = FontWeight::W800;
            break;
        }
        case FONT_WEIGHT_900: {
            rosenFontWeight = FontWeight::W900;
            break;
        }
        default: {
            rosenFontWeight = FontWeight::W400;
        }
    }
    ConvertToOriginalText(style)->fontWeight_ = rosenFontWeight;
}

void OH_Drawing_SetTextStyleBaseLine(OH_Drawing_TextStyle* style, int baseline)
{
    TextBaseline rosenBaseLine;
    switch (baseline) {
        case TEXT_BASELINE_ALPHABETIC: {
            rosenBaseLine = TextBaseline::ALPHABETIC;
            break;
        }
        case TEXT_BASELINE_IDEOGRAPHIC: {
            rosenBaseLine = TextBaseline::IDEOGRAPHIC;
            break;
        }
        default: {
            rosenBaseLine = TextBaseline::ALPHABETIC;
        }
    }
    ConvertToOriginalText(style)->textBaseline_ = rosenBaseLine;
}

void OH_Drawing_SetTextStyleDecoration(OH_Drawing_TextStyle* style, int decoration)
{
    TextDecoration rosenDecoration;
    switch (decoration) {
        case TEXT_DECORATION_NONE: {
            rosenDecoration = TextDecoration::NONE;
            break;
        }
        case TEXT_DECORATION_UNDERLINE: {
            rosenDecoration = TextDecoration::UNDERLINE;
            break;
        }
        case TEXT_DECORATION_OVERLINE: {
            rosenDecoration = TextDecoration::OVERLINE;
            break;
        }
        case TEXT_DECORATION_LINE_THROUGH: {
            rosenDecoration = TextDecoration::LINETHROUGH;
            break;
        }
        default: {
            rosenDecoration = TextDecoration::NONE;
        }
    }
    ConvertToOriginalText(style)->decoration_ = rosenDecoration;
}

void OH_Drawing_SetTextStyleDecorationColor(OH_Drawing_TextStyle* style, uint32_t color)
{
    ConvertToOriginalText(style)->decorationColor_.SetColorQuad(color);
}

void OH_Drawing_SetTextStyleFontHeight(OH_Drawing_TextStyle* style, double fontHeight)
{
    ConvertToOriginalText(style)->height_ = fontHeight;
}

void OH_Drawing_SetTextStyleFontFamilies(OH_Drawing_TextStyle* style,
    int fontFamiliesNumber, const char* fontFamilies[])
{
    std::vector<std::string> rosenFontFamilies;
    for (int i = 0; i < fontFamiliesNumber; i++) {
        rosenFontFamilies.emplace_back(fontFamilies[i]);
    }
    ConvertToOriginalText(style)->fontFamilies_ = rosenFontFamilies;
}

void OH_Drawing_SetTextStyleFontStyle(OH_Drawing_TextStyle* style, int fontStyle)
{
    FontStyle rosenFontStyle;
    switch (fontStyle) {
        case FONT_STYLE_NORMAL: {
            rosenFontStyle = FontStyle::NORMAL;
            break;
        }
        case FONT_STYLE_ITALIC: {
            rosenFontStyle = FontStyle::ITALIC;
            break;
        }
        default: {
            rosenFontStyle = FontStyle::NORMAL;
        }
    }
    ConvertToOriginalText(style)->fontStyle_ = rosenFontStyle;
}

void OH_Drawing_SetTextStyleLocale(OH_Drawing_TextStyle* style, const char* locale)
{
    ConvertToOriginalText(style)->locale_ = locale;
}

OH_Drawing_TypographyCreate* OH_Drawing_CreateTypographyHandler(OH_Drawing_TypographyStyle* style,
    OH_Drawing_FontCollection* fontCollection)
{
    const TypographyStyle* typoStyle = ConvertToOriginalText(style);
    std::unique_ptr<TypographyCreate> handler = TypographyCreate::CreateRosenBuilder(*typoStyle,
        std::shared_ptr<FontCollection>(ConvertToOriginalText(fontCollection)));
    return ConvertToNDKText(handler.release());
}

void OH_Drawing_DestroyTypographyHandler(OH_Drawing_TypographyCreate* handler)
{
    delete ConvertToOriginalText(handler);
}

void OH_Drawing_TypographyHandlerPushTextStyle(OH_Drawing_TypographyCreate* handler, OH_Drawing_TextStyle* style)
{
    const TextStyle* rosenTextStyle = ConvertToOriginalText(style);
    ConvertToOriginalText(handler)->PushStyle(*rosenTextStyle);
}

void OH_Drawing_TypographyHandlerAddText(OH_Drawing_TypographyCreate* handler, const char* text)
{
    const std::u16string wideText =
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> {}.from_bytes(text);
    ConvertToOriginalText(handler)->AddText(wideText);
}

void OH_Drawing_TypographyHandlerPopTextStyle(OH_Drawing_TypographyCreate* handler)
{
    ConvertToOriginalText(handler)->Pop();
}

OH_Drawing_Typography* OH_Drawing_CreateTypography(OH_Drawing_TypographyCreate* handler)
{
    TypographyCreate* rosenHandler = ConvertToOriginalText(handler);
    std::unique_ptr<Typography> typography = rosenHandler->Build();
    return ConvertToNDKText(typography.release());
}

void OH_Drawing_DestroyTypography(OH_Drawing_Typography* typography)
{
    delete ConvertToOriginalText(typography);
}

void OH_Drawing_TypographyLayout(OH_Drawing_Typography* typography, double maxWidth)
{
    ConvertToOriginalText(typography)->Layout(maxWidth);
}

void OH_Drawing_TypographyPaint(OH_Drawing_Typography* typography, OH_Drawing_Canvas* canvas,
    double potisionX, double potisionY)
{
    ConvertToOriginalText(typography)->Paint(reinterpret_cast<OHOS::Rosen::Drawing::Canvas*>(canvas),
        potisionX, potisionY);
}
