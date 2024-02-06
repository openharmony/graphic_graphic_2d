
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
#include "utils/log.h"
#include "utils/object_mgr.h"

#ifndef USE_GRAPHIC_TEXT_GINE
#include "rosen_text/ui/font_collection.h"
#include "rosen_text/ui/typography.h"
#include "rosen_text/ui/typography_create.h"
#else
#include "rosen_text/font_collection.h"
#include "rosen_text/typography.h"
#include "rosen_text/typography_create.h"
#include "unicode/putil.h"
#endif

#include <codecvt>
#include <locale>
#include <vector>
#include <string>

#ifndef USE_GRAPHIC_TEXT_GINE
using namespace rosen;
#else
using namespace OHOS::Rosen;

namespace {
__attribute__((constructor)) void init()
{
#if !defined(_WIN32) && !defined(CROSS_PLATFORM)
    u_setDataDirectory("/system/usr/ohos_icu");
#else
    u_setDataDirectory(".");
#endif
}
} // namespace
#endif

static std::shared_ptr<OHOS::Rosen::Drawing::ObjectMgr> objectMgr = OHOS::Rosen::Drawing::ObjectMgr::GetInstance();

template<typename T1, typename T2>
inline T1* ConvertToOriginalText(T2* ptr)
{
    return reinterpret_cast<T1*>(ptr);
}

template<typename T1, typename T2>
inline T1* ConvertToNDKText(T2* ptr)
{
    return reinterpret_cast<T1*>(ptr);
}

OH_Drawing_TypographyStyle* OH_Drawing_CreateTypographyStyle(void)
{
    return (OH_Drawing_TypographyStyle*)new TypographyStyle;
}

void OH_Drawing_DestroyTypographyStyle(OH_Drawing_TypographyStyle* style)
{
    delete ConvertToOriginalText<TypographyStyle>(style);
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
#ifndef USE_GRAPHIC_TEXT_GINE
    ConvertToOriginalText<TypographyStyle>(style)->textDirection_ = textDirection;
#else
    ConvertToOriginalText<TypographyStyle>(style)->textDirection = textDirection;
#endif
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
#ifndef USE_GRAPHIC_TEXT_GINE
    ConvertToOriginalText<TypographyStyle>(style)->textAlign_ = textAlign;
#else
    ConvertToOriginalText<TypographyStyle>(style)->textAlign = textAlign;
#endif
}

void OH_Drawing_SetTypographyTextMaxLines(OH_Drawing_TypographyStyle* style, int lineNumber)
{
#ifndef USE_GRAPHIC_TEXT_GINE
    ConvertToOriginalText<TypographyStyle>(style)->maxLines_ = static_cast<size_t>(lineNumber);
#else
    ConvertToOriginalText<TypographyStyle>(style)->maxLines = static_cast<size_t>(lineNumber);
#endif
}

OH_Drawing_TextStyle* OH_Drawing_CreateTextStyle(void)
{
    return (OH_Drawing_TextStyle*)new TextStyle;
}

void OH_Drawing_DestroyTextStyle(OH_Drawing_TextStyle* style)
{
    delete ConvertToOriginalText<TextStyle>(style);
}

void OH_Drawing_SetTextStyleColor(OH_Drawing_TextStyle* style, uint32_t color)
{
#ifndef USE_GRAPHIC_TEXT_GINE
    ConvertToOriginalText<TextStyle>(style)->color_.SetColorQuad(color);
#else
    ConvertToOriginalText<TextStyle>(style)->color.SetColorQuad(color);
#endif
}

void OH_Drawing_SetTextStyleFontSize(OH_Drawing_TextStyle* style, double fontSize)
{
#ifndef USE_GRAPHIC_TEXT_GINE
    ConvertToOriginalText<TextStyle>(style)->fontSize_ = fontSize;
#else
    ConvertToOriginalText<TextStyle>(style)->fontSize = fontSize;
#endif
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
#ifndef USE_GRAPHIC_TEXT_GINE
    ConvertToOriginalText<TextStyle>(style)->fontWeight_ = rosenFontWeight;
#else
    ConvertToOriginalText<TextStyle>(style)->fontWeight = rosenFontWeight;
#endif
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
#ifndef USE_GRAPHIC_TEXT_GINE
    ConvertToOriginalText<TextStyle>(style)->textBaseline_ = rosenBaseLine;
#else
    ConvertToOriginalText<TextStyle>(style)->baseline = rosenBaseLine;
#endif
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
#ifndef USE_GRAPHIC_TEXT_GINE
            rosenDecoration = TextDecoration::LINETHROUGH;
#else
            rosenDecoration = TextDecoration::LINE_THROUGH;
#endif
            break;
        }
        default: {
            rosenDecoration = TextDecoration::NONE;
        }
    }
#ifndef USE_GRAPHIC_TEXT_GINE
    ConvertToOriginalText<TextStyle>(style)->decoration_ = rosenDecoration;
#else
    ConvertToOriginalText<TextStyle>(style)->decoration = rosenDecoration;
#endif
}

void OH_Drawing_SetTextStyleDecorationColor(OH_Drawing_TextStyle* style, uint32_t color)
{
#ifndef USE_GRAPHIC_TEXT_GINE
    ConvertToOriginalText<TextStyle>(style)->decorationColor_.SetColorQuad(color);
#else
    ConvertToOriginalText<TextStyle>(style)->decorationColor.SetColorQuad(color);
#endif
}

void OH_Drawing_SetTextStyleFontHeight(OH_Drawing_TextStyle* style, double fontHeight)
{
#ifndef USE_GRAPHIC_TEXT_GINE
    ConvertToOriginalText<TextStyle>(style)->height_ = fontHeight;
    ConvertToOriginalText<TextStyle>(style)->hasHeightOverride_ = true;
#else
    ConvertToOriginalText<TextStyle>(style)->heightScale = fontHeight;
    ConvertToOriginalText<TextStyle>(style)->heightOnly = true;
#endif
}

void OH_Drawing_SetTextStyleFontFamilies(OH_Drawing_TextStyle* style,
    int fontFamiliesNumber, const char* fontFamilies[])
{
    std::vector<std::string> rosenFontFamilies;
    for (int i = 0; i < fontFamiliesNumber; i++) {
        rosenFontFamilies.emplace_back(fontFamilies[i]);
    }
#ifndef USE_GRAPHIC_TEXT_GINE
    ConvertToOriginalText<TextStyle>(style)->fontFamilies_ = rosenFontFamilies;
#else
    ConvertToOriginalText<TextStyle>(style)->fontFamilies = rosenFontFamilies;
#endif
}

void OH_Drawing_SetTextStyleFontStyle(OH_Drawing_TextStyle* style, int fontStyle)
{
#ifndef USE_GRAPHIC_TEXT_GINE
    rosen::FontStyle rosenFontStyle;
    switch (fontStyle) {
        case FONT_STYLE_NORMAL: {
            rosenFontStyle = rosen::FontStyle::NORMAL;
            break;
        }
        case FONT_STYLE_ITALIC: {
            rosenFontStyle = rosen::FontStyle::ITALIC;
            break;
        }
        default: {
            rosenFontStyle = rosen::FontStyle::NORMAL;
        }
    }
    ConvertToOriginalText<TextStyle>(style)->fontStyle_ = rosenFontStyle;
#else
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
    ConvertToOriginalText<TextStyle>(style)->fontStyle = rosenFontStyle;
#endif
}

void OH_Drawing_SetTextStyleLocale(OH_Drawing_TextStyle* style, const char* locale)
{
#ifndef USE_GRAPHIC_TEXT_GINE
    ConvertToOriginalText<TextStyle>(style)->locale_ = locale;
#else
    ConvertToOriginalText<TextStyle>(style)->locale = locale;
#endif
}

OH_Drawing_TypographyCreate* OH_Drawing_CreateTypographyHandler(OH_Drawing_TypographyStyle* style,
    OH_Drawing_FontCollection* fontCollection)
{
    if (!style || !fontCollection) {
        return nullptr;
    }

    objectMgr->RemoveObject(fontCollection);
#ifndef USE_GRAPHIC_TEXT_GINE
    const TypographyStyle* typoStyle = ConvertToOriginalText<TypographyStyle>(style);
    std::unique_ptr<TypographyCreate> handler = TypographyCreate::CreateRosenBuilder(*typoStyle,
        std::shared_ptr<FontCollection>(ConvertToOriginalText<FontCollection>(fontCollection)));
#else
    const TypographyStyle *typoStyle = ConvertToOriginalText<TypographyStyle>(style);
    if (!typoStyle) {
        return nullptr;
    }
    std::unique_ptr<TypographyCreate> handler = TypographyCreate::Create(*typoStyle,
        std::shared_ptr<FontCollection>(ConvertToOriginalText<FontCollection>(fontCollection)));
#endif
    return ConvertToNDKText<OH_Drawing_TypographyCreate>(handler.release());
}

void OH_Drawing_DestroyTypographyHandler(OH_Drawing_TypographyCreate* handler)
{
    delete ConvertToOriginalText<TypographyCreate>(handler);
}

void OH_Drawing_TypographyHandlerPushTextStyle(OH_Drawing_TypographyCreate* handler, OH_Drawing_TextStyle* style)
{
    const TextStyle* rosenTextStyle = ConvertToOriginalText<TextStyle>(style);
    ConvertToOriginalText<TypographyCreate>(handler)->PushStyle(*rosenTextStyle);
}

static bool IsUtf8(const char* text)
{
    int len = strlen(text);
    int n;
    for (int i = 0; i < len; i++) {
        uint32_t c = text[i];
        if (c <= 0x7F) { // 0x00 and 0x7F is the range of utf-8
            n = 0;
        } else if ((c & 0xE0) == 0xC0) { // 0xE0 and 0xC0 is the range of utf-8
            n = 1;
        } else if (c == 0xED && i < (len - 1) &&
            (text[i + 1] & 0xA0) == 0xA0) { // 0xA0 and 0xED is the range of utf-8
            return false;
        } else if ((c & 0xF0) == 0xE0) { // 0xE0 and 0xF0 is the range of utf-8
            n = 2; // 2 means the size of range
        } else if ((c & 0xF8) == 0xF0) { // 0xF0 and 0xF8 is the range of utf-8
            n = 3; // 3 means the size of range
        } else {
            return false;
        }

        for (int j = 0; j < n && i < len; j++) {
            // 0x80 and 0xC0 is the range of utf-8
            if ((++i == len) || ((text[i] & 0xC0) != 0x80)) {
                return false;
            }
        }
    }
    return true;
}

void OH_Drawing_TypographyHandlerAddText(OH_Drawing_TypographyCreate* handler, const char* text)
{
    if (!text) {
        LOGE("null text");
        return;
    } else if (!IsUtf8(text)) {
        LOGE("text is not utf-8, text: %{public}s", text);
        return;
    }

    const std::u16string wideText =
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> {}.from_bytes(text);
#ifndef USE_GRAPHIC_TEXT_GINE
    ConvertToOriginalText<TypographyCreate>(handler)->AddText(wideText);
#else
    ConvertToOriginalText<TypographyCreate>(handler)->AppendText(wideText);
#endif
}

void OH_Drawing_TypographyHandlerPopTextStyle(OH_Drawing_TypographyCreate* handler)
{
#ifndef USE_GRAPHIC_TEXT_GINE
    ConvertToOriginalText<TypographyCreate>(handler)->Pop();
#else
    ConvertToOriginalText<TypographyCreate>(handler)->PopStyle();
#endif
}

OH_Drawing_Typography* OH_Drawing_CreateTypography(OH_Drawing_TypographyCreate* handler)
{
    TypographyCreate* rosenHandler = ConvertToOriginalText<TypographyCreate>(handler);
#ifndef USE_GRAPHIC_TEXT_GINE
    std::unique_ptr<Typography> typography = rosenHandler->Build();
#else
    std::unique_ptr<Typography> typography = rosenHandler->CreateTypography();
#endif
    return ConvertToNDKText<OH_Drawing_Typography>(typography.release());
}

void OH_Drawing_DestroyTypography(OH_Drawing_Typography* typography)
{
    delete ConvertToOriginalText<Typography>(typography);
}

void OH_Drawing_TypographyLayout(OH_Drawing_Typography* typography, double maxWidth)
{
    ConvertToOriginalText<Typography>(typography)->Layout(maxWidth);
}

void OH_Drawing_TypographyPaint(OH_Drawing_Typography* typography, OH_Drawing_Canvas* canvas,
    double potisionX, double potisionY)
{
    ConvertToOriginalText<Typography>(typography)->Paint(reinterpret_cast<OHOS::Rosen::Drawing::Canvas*>(canvas),
        potisionX, potisionY);
}

double OH_Drawing_TypographyGetMaxWidth(OH_Drawing_Typography* typography)
{
    return ConvertToOriginalText<Typography>(typography)->GetMaxWidth();
}

double OH_Drawing_TypographyGetHeight(OH_Drawing_Typography* typography)
{
    return ConvertToOriginalText<Typography>(typography)->GetHeight();
}

double OH_Drawing_TypographyGetLongestLine(OH_Drawing_Typography* typography)
{
#ifndef USE_GRAPHIC_TEXT_GINE
    return ConvertToOriginalText<Typography>(typography)->GetLongestLine();
#else
    return ConvertToOriginalText<Typography>(typography)->GetActualWidth();
#endif
}

double OH_Drawing_TypographyGetMinIntrinsicWidth(OH_Drawing_Typography* typography)
{
    return ConvertToOriginalText<Typography>(typography)->GetMinIntrinsicWidth();
}

double OH_Drawing_TypographyGetMaxIntrinsicWidth(OH_Drawing_Typography* typography)
{
    return ConvertToOriginalText<Typography>(typography)->GetMaxIntrinsicWidth();
}

double OH_Drawing_TypographyGetAlphabeticBaseline(OH_Drawing_Typography* typography)
{
    return ConvertToOriginalText<Typography>(typography)->GetAlphabeticBaseline();
}

double OH_Drawing_TypographyGetIdeographicBaseline(OH_Drawing_Typography* typography)
{
    return ConvertToOriginalText<Typography>(typography)->GetIdeographicBaseline();
}

void OH_Drawing_TypographyHandlerAddPlaceholder(OH_Drawing_TypographyCreate* handler,
    OH_Drawing_PlaceholderSpan* span)
{
#ifndef USE_GRAPHIC_TEXT_GINE
    auto originalAlignment = ConvertToOriginalText<PlaceholderAlignment>(&span->alignment);
    auto originalBaseline = ConvertToOriginalText<TextBaseline>(&span->baseline);
    PlaceholderRun rosenPlaceholderRun(
        span->width, span->height, originalAlignment, originalBaseline, span->baselineOffset);
    ConvertToOriginalText<TypographyCreate>(handler)->AddPlaceholder(rosenPlaceholderRun);
#else
    auto originalPlaceholderSpan = ConvertToOriginalText<PlaceholderSpan>(span);
    ConvertToOriginalText<TypographyCreate>(handler)->AppendPlaceholder(*originalPlaceholderSpan);
#endif
}

bool OH_Drawing_TypographyDidExceedMaxLines(OH_Drawing_Typography* typography)
{
    return ConvertToOriginalText<Typography>(typography)->DidExceedMaxLines();
}

OH_Drawing_TextBox* OH_Drawing_TypographyGetRectsForRange(OH_Drawing_Typography* typography,
    size_t start, size_t end, OH_Drawing_RectHeightStyle heightStyle, OH_Drawing_RectWidthStyle widthStyle)
{
#ifndef USE_GRAPHIC_TEXT_GINE
    std::vector<TypographyProperties::TextBox>* originalVector =
        new std::vector<TypographyProperties::TextBox>;
    auto originalRectHeightStyle = ConvertToOriginalText<TypographyProperties::RectHeightStyle>(&heightStyle);
    auto originalRectWidthStyle = ConvertToOriginalText<TypographyProperties::RectWidthStyle>(&widthStyle);
    *originalVector = ConvertToOriginalText<Typography>(typography)->GetRectsForRange(start, end,
        *originalRectHeightStyle, *originalRectWidthStyle);
#else
    std::vector<TextRect>* originalVector = new std::vector<TextRect>;
    auto originalRectHeightStyle = ConvertToOriginalText<TextRectHeightStyle>(&heightStyle);
    auto originalRectWidthStyle = ConvertToOriginalText<TextRectWidthStyle>(&widthStyle);
    *originalVector = ConvertToOriginalText<Typography>(typography)->GetTextRectsByBoundary(start, end,
        *originalRectHeightStyle, *originalRectWidthStyle);
#endif
    return (OH_Drawing_TextBox*)originalVector;
}

OH_Drawing_TextBox* OH_Drawing_TypographyGetRectsForPlaceholders(OH_Drawing_Typography* typography)
{
#ifndef USE_GRAPHIC_TEXT_GINE
    std::vector<TypographyProperties::TextBox>* originalVector =
        new std::vector<TypographyProperties::TextBox>;
    *originalVector = ConvertToOriginalText<Typography>(typography)->GetRectsForPlaceholders();
#else
    std::vector<TextRect>* originalVector = new std::vector<TextRect>;
    *originalVector = ConvertToOriginalText<Typography>(typography)->GetTextRectsOfPlaceholders();
#endif
    return (OH_Drawing_TextBox*)originalVector;
}

float OH_Drawing_GetLeftFromTextBox(OH_Drawing_TextBox* textbox, int index)
{
#ifndef USE_GRAPHIC_TEXT_GINE
    std::vector<TypographyProperties::TextBox>* textboxVector =
        ConvertToOriginalText<std::vector<TypographyProperties::TextBox>>(textbox);
    if (index >= 0 && index < static_cast<int>(textboxVector->size())) {
        return (*textboxVector)[index].rect_.left_;
    } else {
        return 0.0;
    }
#else
    std::vector<TextRect>* textboxVector = ConvertToOriginalText<std::vector<TextRect>>(textbox);
    if (index >= 0 && index < static_cast<int>(textboxVector->size())) {
        return (*textboxVector)[index].rect.left_;
    } else {
        return 0.0;
    }
#endif
}

float OH_Drawing_GetRightFromTextBox(OH_Drawing_TextBox* textbox, int index)
{
#ifndef USE_GRAPHIC_TEXT_GINE
    std::vector<TypographyProperties::TextBox>* textboxVector =
        ConvertToOriginalText<std::vector<TypographyProperties::TextBox>>(textbox);
    if (index >= 0 && index < static_cast<int>(textboxVector->size())) {
        return (*textboxVector)[index].rect_.right_;
    } else {
        return 0.0;
    }
#else
    std::vector<TextRect>* textboxVector = ConvertToOriginalText<std::vector<TextRect>>(textbox);
    if (index >= 0 && index < static_cast<int>(textboxVector->size())) {
        return (*textboxVector)[index].rect.right_;
    } else {
        return 0.0;
    }
#endif
}

float OH_Drawing_GetTopFromTextBox(OH_Drawing_TextBox* textbox, int index)
{
#ifndef USE_GRAPHIC_TEXT_GINE
    std::vector<TypographyProperties::TextBox>* textboxVector =
        ConvertToOriginalText<std::vector<TypographyProperties::TextBox>>(textbox);
    if (index >= 0 && index < static_cast<int>(textboxVector->size())) {
        return (*textboxVector)[index].rect_.top_;
    } else {
        return 0.0;
    }
#else
    std::vector<TextRect>* textboxVector = ConvertToOriginalText<std::vector<TextRect>>(textbox);
    if (index >= 0 && index < static_cast<int>(textboxVector->size())) {
        return (*textboxVector)[index].rect.top_;
    } else {
        return 0.0;
    }
#endif
}

float OH_Drawing_GetBottomFromTextBox(OH_Drawing_TextBox* textbox, int index)
{
#ifndef USE_GRAPHIC_TEXT_GINE
    std::vector<TypographyProperties::TextBox>* textboxVector =
        ConvertToOriginalText<std::vector<TypographyProperties::TextBox>>(textbox);
    if (index >= 0 && index < static_cast<int>(textboxVector->size())) {
        return (*textboxVector)[index].rect_.bottom_;
    } else {
        return 0.0;
    }
#else
    std::vector<TextRect>* textboxVector = ConvertToOriginalText<std::vector<TextRect>>(textbox);
    if (index >= 0 && index < static_cast<int>(textboxVector->size())) {
        return (*textboxVector)[index].rect.bottom_;
    } else {
        return 0.0;
    }
#endif
}

int OH_Drawing_GetTextDirectionFromTextBox(OH_Drawing_TextBox* textbox, int index)
{
#ifndef USE_GRAPHIC_TEXT_GINE
    std::vector<TypographyProperties::TextBox>* textboxVector =
        ConvertToOriginalText<std::vector<TypographyProperties::TextBox>>(textbox);
    if (index >= 0 && index < static_cast<int>(textboxVector->size())) {
        TextDirection textDirection = (*textboxVector)[index].direction_;
        switch (textDirection) {
            case TextDirection::RTL: {
                return 0;
            }
            case TextDirection::LTR: {
                return 1;
            }
            default: {
                return 0;
            }
        }
    } else {
        return 0;
    }
#else
    std::vector<TextRect>* textboxVector = ConvertToOriginalText<std::vector<TextRect>>(textbox);
    if (index >= 0 && index < static_cast<int>(textboxVector->size())) {
        TextDirection textDirection = (*textboxVector)[index].direction;
        switch (textDirection) {
            case TextDirection::RTL: {
                return 0;
            }
            case TextDirection::LTR: {
                return 1;
            }
            default: {
                return 0;
            }
        }
    } else {
        return 0;
    }
#endif
}

size_t OH_Drawing_GetSizeOfTextBox(OH_Drawing_TextBox* textbox)
{
#ifndef USE_GRAPHIC_TEXT_GINE
    std::vector<TypographyProperties::TextBox>* textboxVector =
        ConvertToOriginalText<std::vector<TypographyProperties::TextBox>>(textbox);
    return textboxVector->size();
#else
    std::vector<TextRect>* textboxVector = ConvertToOriginalText<std::vector<TextRect>>(textbox);
    return textboxVector->size();
#endif
}

OH_Drawing_PositionAndAffinity* OH_Drawing_TypographyGetGlyphPositionAtCoordinate(OH_Drawing_Typography* typography,
    double dx, double dy)
{
#ifndef USE_GRAPHIC_TEXT_GINE
    TypographyProperties::PositionAndAffinity* originalPositionAndAffinity =
        new TypographyProperties::PositionAndAffinity(0, TypographyProperties::Affinity::UPSTREAM);
    *originalPositionAndAffinity =
        ConvertToOriginalText<Typography>(typography)->GetGlyphPositionAtCoordinate(dx, dy);
#else
    IndexAndAffinity* originalPositionAndAffinity = new IndexAndAffinity(0, Affinity::PREV);
#endif
    return (OH_Drawing_PositionAndAffinity*)originalPositionAndAffinity;
}

OH_Drawing_PositionAndAffinity* OH_Drawing_TypographyGetGlyphPositionAtCoordinateWithCluster(
    OH_Drawing_Typography* typography, double dx, double dy)
{
#ifndef USE_GRAPHIC_TEXT_GINE
    TypographyProperties::PositionAndAffinity* originalPositionAndAffinity =
        new TypographyProperties::PositionAndAffinity(0, TypographyProperties::Affinity::UPSTREAM);
    *originalPositionAndAffinity =
        ConvertToOriginalText<Typography>(typography)->GetGlyphPositionAtCoordinateWithCluster(dx, dy);
#else
    IndexAndAffinity* originalPositionAndAffinity = new IndexAndAffinity(0, Affinity::PREV);
    *originalPositionAndAffinity =
        ConvertToOriginalText<Typography>(typography)->GetGlyphIndexByCoordinate(dx, dy);
#endif
    return (OH_Drawing_PositionAndAffinity*)originalPositionAndAffinity;
}

size_t OH_Drawing_GetPositionFromPositionAndAffinity(OH_Drawing_PositionAndAffinity* positionandaffinity)
{
#ifndef USE_GRAPHIC_TEXT_GINE
    TypographyProperties::PositionAndAffinity* textPositionAndAffinity =
        ConvertToOriginalText<TypographyProperties::PositionAndAffinity>(positionandaffinity);
    return textPositionAndAffinity->pos_;
#else
    IndexAndAffinity* textIndexAndAffinity = ConvertToOriginalText<IndexAndAffinity>(positionandaffinity);
    return textIndexAndAffinity->index;
#endif
}

int OH_Drawing_GetAffinityFromPositionAndAffinity(OH_Drawing_PositionAndAffinity* positionandaffinity)
{
#ifndef USE_GRAPHIC_TEXT_GINE
    TypographyProperties::PositionAndAffinity* textPositionAndAffinity =
        ConvertToOriginalText<TypographyProperties::PositionAndAffinity>(positionandaffinity);
    switch (textPositionAndAffinity->affinity_) {
        case TypographyProperties::Affinity::UPSTREAM: {
            return 0;
        }
        case TypographyProperties::Affinity::DOWNSTREAM: {
            return 1;
        }
        default: {
            return 0;
        }
    }
#else
    IndexAndAffinity* textIndexAndAffinity = ConvertToOriginalText<IndexAndAffinity>(positionandaffinity);
    switch (textIndexAndAffinity->affinity) {
        case Affinity::PREV: {
            return 0;
        }
        case Affinity::NEXT: {
            return 1;
        }
        default: {
            return 0;
        }
    }
#endif
}

OH_Drawing_Range* OH_Drawing_TypographyGetWordBoundary(OH_Drawing_Typography* typography, size_t offset)
{
#ifndef USE_GRAPHIC_TEXT_GINE
    TypographyProperties::Range<size_t>* originalRange = new TypographyProperties::Range<size_t>;
    *originalRange = ConvertToOriginalText<Typography>(typography)->GetWordBoundary(offset);
#else
    Boundary* originalRange = new Boundary(0, 0);
    *originalRange = ConvertToOriginalText<Typography>(typography)->GetWordBoundaryByIndex(offset);
#endif
    return (OH_Drawing_Range*)originalRange;
}

size_t OH_Drawing_GetStartFromRange(OH_Drawing_Range* range)
{
#ifndef USE_GRAPHIC_TEXT_GINE
    TypographyProperties::Range<size_t>* textRange =
        ConvertToOriginalText<TypographyProperties::Range<size_t>>(range);
    return textRange->start_;
#else
    Boundary* boundary = ConvertToOriginalText<Boundary>(range);
    return boundary->leftIndex;
#endif
}

size_t OH_Drawing_GetEndFromRange(OH_Drawing_Range* range)
{
#ifndef USE_GRAPHIC_TEXT_GINE
    TypographyProperties::Range<size_t>* textRange =
        ConvertToOriginalText<TypographyProperties::Range<size_t>>(range);
    return textRange->end_;
#else
    Boundary* boundary = ConvertToOriginalText<Boundary>(range);
    return boundary->rightIndex;
#endif
}

size_t OH_Drawing_TypographyGetLineCount(OH_Drawing_Typography* typography)
{
    return ConvertToOriginalText<Typography>(typography)->GetLineCount();
}

void OH_Drawing_SetTextStyleDecorationStyle(OH_Drawing_TextStyle* style, int decorationStyle)
{
    TextDecorationStyle rosenDecorationStyle;
    switch (decorationStyle) {
        case TEXT_DECORATION_STYLE_SOLID: {
            rosenDecorationStyle = TextDecorationStyle::SOLID;
            break;
        }
        case TEXT_DECORATION_STYLE_DOUBLE: {
            rosenDecorationStyle = TextDecorationStyle::DOUBLE;
            break;
        }
        case TEXT_DECORATION_STYLE_DOTTED: {
            rosenDecorationStyle = TextDecorationStyle::DOTTED;
            break;
        }
        case TEXT_DECORATION_STYLE_DASHED: {
            rosenDecorationStyle = TextDecorationStyle::DASHED;
            break;
        }
        case TEXT_DECORATION_STYLE_WAVY: {
            rosenDecorationStyle = TextDecorationStyle::WAVY;
            break;
        }
        default: {
            rosenDecorationStyle = TextDecorationStyle::SOLID;
        }
    }
    #ifndef USE_GRAPHIC_TEXT_GINE
        ConvertToOriginalText<TextStyle>(style)->decorationStyle_ = rosenDecorationStyle;
    #else
        ConvertToOriginalText<TextStyle>(style)->decorationStyle = rosenDecorationStyle;
    #endif
}

void OH_Drawing_SetTextStyleDecorationThicknessScale(OH_Drawing_TextStyle* style, double decorationThicknessScale)
{
    #ifndef USE_GRAPHIC_TEXT_GINE
        ConvertToOriginalText<TextStyle>(style)->decorationThicknessMultiplier_ = decorationThicknessScale;
    #else
        ConvertToOriginalText<TextStyle>(style)->decorationThicknessScale = decorationThicknessScale;
    #endif
}

void OH_Drawing_SetTextStyleLetterSpacing(OH_Drawing_TextStyle* style, double letterSpacing)
{
    #ifndef USE_GRAPHIC_TEXT_GINE
        ConvertToOriginalText<TextStyle>(style)->letterSpacing_ = letterSpacing;
    #else
        ConvertToOriginalText<TextStyle>(style)->letterSpacing = letterSpacing;
    #endif
}

void OH_Drawing_SetTextStyleWordSpacing(OH_Drawing_TextStyle* style, double wordSpacing)
{
    #ifndef USE_GRAPHIC_TEXT_GINE
        ConvertToOriginalText<TextStyle>(style)->wordSpacing_ = wordSpacing;
    #else
        ConvertToOriginalText<TextStyle>(style)->wordSpacing = wordSpacing;
    #endif
}

void OH_Drawing_SetTextStyleHalfLeading(OH_Drawing_TextStyle* style, bool halfLeading)
{
    #ifndef USE_GRAPHIC_TEXT_GINE
    #else
        ConvertToOriginalText<TextStyle>(style)->halfLeading = halfLeading;
    #endif
}

void OH_Drawing_SetTextStyleEllipsis(OH_Drawing_TextStyle* style, const char* ellipsis)
{
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
    std::u16string u16Ellipsis = converter.from_bytes(ellipsis);
    #ifndef USE_GRAPHIC_TEXT_GINE
        ConvertToOriginalText<TextStyle>(style)->ellipsis_ = u16Ellipsis;
    #else
        ConvertToOriginalText<TextStyle>(style)->ellipsis = u16Ellipsis;
    #endif
}

void OH_Drawing_SetTextStyleEllipsisModal(OH_Drawing_TextStyle* style, int ellipsisModal)
{
    EllipsisModal rosenEllipsisModal;
    switch (ellipsisModal) {
        case ELLIPSIS_MODAL_HEAD: {
            rosenEllipsisModal = EllipsisModal::HEAD;
            break;
        }
        case ELLIPSIS_MODAL_MIDDLE: {
            rosenEllipsisModal = EllipsisModal::MIDDLE;
            break;
        }
        case ELLIPSIS_MODAL_TAIL: {
            rosenEllipsisModal = EllipsisModal::TAIL;
            break;
        }
        default: {
            rosenEllipsisModal = EllipsisModal::TAIL;
        }
    }
    #ifndef USE_GRAPHIC_TEXT_GINE
        ConvertToOriginalText<TextStyle>(style)->ellipsisModal_ = rosenEllipsisModal;
    #else
        ConvertToOriginalText<TextStyle>(style)->ellipsisModal = rosenEllipsisModal;
    #endif
}

void OH_Drawing_SetTypographyTextBreakStrategy(OH_Drawing_TypographyStyle* style, int breakStrategy)
{
    BreakStrategy rosenBreakStrategy;
    #ifndef USE_GRAPHIC_TEXT_GINE
        switch (breakStrategy) {
            case BREAK_STRATEGY_GREEDY: {
                rosenBreakStrategy = BreakStrategy::BreakStrategyGreedy;
                break;
            }
            case BREAK_STRATEGY_HIGH_QUALITY: {
                rosenBreakStrategy = BreakStrategy::BreakStrategyHighQuality;
                break;
            }
            case BREAK_STRATEGY_BALANCED: {
                rosenBreakStrategy = BreakStrategy::BreakStrategyBalanced;
                break;
            }
            default: {
                rosenBreakStrategy = BreakStrategy::BreakStrategyGreedy;
            }
        }
        ConvertToOriginalText<TypographyStyle>(style)->breakStrategy_ = rosenBreakStrategy;
    #else
        switch (breakStrategy) {
            case BREAK_STRATEGY_GREEDY: {
                rosenBreakStrategy = BreakStrategy::GREEDY;
                break;
            }
            case BREAK_STRATEGY_HIGH_QUALITY: {
                rosenBreakStrategy = BreakStrategy::HIGH_QUALITY;
                break;
            }
            case BREAK_STRATEGY_BALANCED: {
                rosenBreakStrategy = BreakStrategy::BALANCED;
                break;
            }
            default: {
                rosenBreakStrategy = BreakStrategy::GREEDY;
            }
        }
        ConvertToOriginalText<TypographyStyle>(style)->breakStrategy = rosenBreakStrategy;
    #endif
}

void OH_Drawing_SetTypographyTextWordBreakType(OH_Drawing_TypographyStyle* style, int wordBreakType)
{
    WordBreakType rosenWordBreakType;
    #ifndef USE_GRAPHIC_TEXT_GINE
        switch (wordBreakType) {
            case WORD_BREAK_TYPE_NORMAL: {
                rosenWordBreakType = WordBreakType::WordBreakTypeNormal;
                break;
            }
            case WORD_BREAK_TYPE_BREAK_ALL: {
                rosenWordBreakType = WordBreakType::WordBreakTypeBreakAll;
                break;
            }
            case WORD_BREAK_TYPE_BREAK_WORD: {
                rosenWordBreakType = WordBreakType::WordBreakTypeBreakWord;
                break;
            }
            default: {
                rosenWordBreakType = WordBreakType::WordBreakTypeBreakWord;
            }
        }
        ConvertToOriginalText<TypographyStyle>(style)->wordBreakType_ = rosenWordBreakType;
    #else
        switch (wordBreakType) {
            case WORD_BREAK_TYPE_NORMAL: {
                rosenWordBreakType = WordBreakType::NORMAL;
                break;
            }
            case WORD_BREAK_TYPE_BREAK_ALL: {
                rosenWordBreakType = WordBreakType::BREAK_ALL;
                break;
            }
            case WORD_BREAK_TYPE_BREAK_WORD: {
                rosenWordBreakType = WordBreakType::BREAK_WORD;
                break;
            }
            default: {
                rosenWordBreakType = WordBreakType::BREAK_WORD;
            }
        }
        ConvertToOriginalText<TypographyStyle>(style)->wordBreakType = rosenWordBreakType;
    #endif
}

void OH_Drawing_SetTypographyTextEllipsisModal(OH_Drawing_TypographyStyle* style, int ellipsisModal)
{
    EllipsisModal rosenEllipsisModal;
    switch (ellipsisModal) {
        case ELLIPSIS_MODAL_HEAD: {
            rosenEllipsisModal = EllipsisModal::HEAD;
            break;
        }
        case ELLIPSIS_MODAL_MIDDLE: {
            rosenEllipsisModal = EllipsisModal::MIDDLE;
            break;
        }
        case ELLIPSIS_MODAL_TAIL: {
            rosenEllipsisModal = EllipsisModal::TAIL;
            break;
        }
        default: {
            rosenEllipsisModal = EllipsisModal::TAIL;
        }
    }
    #ifndef USE_GRAPHIC_TEXT_GINE
        ConvertToOriginalText<TypographyStyle>(style)->ellipsisModal_ = rosenEllipsisModal;
    #else
        ConvertToOriginalText<TypographyStyle>(style)->ellipsisModal = rosenEllipsisModal;
    #endif
}

double OH_Drawing_TypographyGetLineHeight(OH_Drawing_Typography* typography, int lineNumber)
{
    Typography* typographyInner = ConvertToOriginalText<Typography>(typography);
    return typographyInner->GetLineHeight(lineNumber);
}

double OH_Drawing_TypographyGetLineWidth(OH_Drawing_Typography* typography, int lineNumber)
{
    Typography* typographyInner = ConvertToOriginalText<Typography>(typography);
    return typographyInner->GetLineWidth(lineNumber);
}

OH_Drawing_Range* OH_Drawing_TypographyGetLineTextRange(OH_Drawing_Typography* typography,
    int lineNumber, bool includeSpaces)
{
#ifndef USE_GRAPHIC_TEXT_GINE
    TypographyProperties::Range<size_t>* originalRange = new TypographyProperties::Range<size_t>;
    *originalRange = ConvertToOriginalText<Typography>(typography)->GetActualTextRange(lineNumber, includeSpaces);
#else
    Boundary* originalRange = new Boundary(0, 0);
    *originalRange = ConvertToOriginalText<Typography>(typography)->GetActualTextRange(lineNumber, includeSpaces);
#endif
    return (OH_Drawing_Range*)originalRange;
}