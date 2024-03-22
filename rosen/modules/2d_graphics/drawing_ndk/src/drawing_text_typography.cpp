
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

#include "drawing_text_typography.h"
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
#include "font_parser.h"
#include <codecvt>
#include <locale>
#include <vector>
#include <string>
#include <unicode/brkiter.h>

#ifndef USE_GRAPHIC_TEXT_GINE
using namespace rosen;
#else
using namespace OHOS::Rosen;

namespace {
__attribute__((constructor)) void init()
{
#ifndef _WIN32
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

    std::unique_ptr<TypographyCreate> handler;
    const TypographyStyle* typoStyle = ConvertToOriginalText<TypographyStyle>(style);

    if (auto fc = OHOS::Rosen::Drawing::FontCollectionMgr::GetInstance().Find(fontCollection)) {
#ifndef USE_GRAPHIC_TEXT_GINE
        handler = TypographyCreate::CreateRosenBuilder(*typoStyle, fc);
#else
        handler = TypographyCreate::Create(*typoStyle, fc);
#endif
    } else {
        objectMgr->RemoveObject(fontCollection);

#ifndef USE_GRAPHIC_TEXT_GINE
        handler = TypographyCreate::CreateRosenBuilder(*typoStyle,
            std::shared_ptr<FontCollection>(ConvertToOriginalText<FontCollection>(fontCollection)));
#else
        handler = TypographyCreate::Create(*typoStyle,
            std::shared_ptr<FontCollection>(ConvertToOriginalText<FontCollection>(fontCollection)));
#endif
    }

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

static void ConvertFontMetrics(const Drawing::FontMetrics& fontMetrics, OH_Drawing_Font_Metrics& drawingFontMetrics)
{
    drawingFontMetrics.flags = fontMetrics.fFlags;
    drawingFontMetrics.top = fontMetrics.fTop;
    drawingFontMetrics.ascent = fontMetrics.fAscent;
    drawingFontMetrics.descent = fontMetrics.fDescent;
    drawingFontMetrics.bottom = fontMetrics.fBottom;
    drawingFontMetrics.leading = fontMetrics.fLeading;
    drawingFontMetrics.avgCharWidth = fontMetrics.fAvgCharWidth;
    drawingFontMetrics.maxCharWidth = fontMetrics.fMaxCharWidth;
    drawingFontMetrics.xMin = fontMetrics.fXMin;
    drawingFontMetrics.xMax = fontMetrics.fXMax;
    drawingFontMetrics.xHeight = fontMetrics.fXHeight;
    drawingFontMetrics.capHeight = fontMetrics.fCapHeight;
    drawingFontMetrics.underlineThickness = fontMetrics.fUnderlineThickness;
    drawingFontMetrics.underlinePosition = fontMetrics.fUnderlinePosition;
    drawingFontMetrics.strikeoutThickness = fontMetrics.fStrikeoutThickness;
    drawingFontMetrics.strikeoutPosition = fontMetrics.fStrikeoutPosition;
}

static void ConvertLineMetrics(const LineMetrics &lineMetrics, OH_Drawing_LineMetrics& drawingLineMetrics)
{
    drawingLineMetrics.ascender = lineMetrics.ascender;
    drawingLineMetrics.descender = lineMetrics.descender;
    drawingLineMetrics.capHeight = lineMetrics.capHeight;
    drawingLineMetrics.xHeight = lineMetrics.xHeight;
    drawingLineMetrics.width = lineMetrics.width;
    drawingLineMetrics.height = lineMetrics.height;
    drawingLineMetrics.x = lineMetrics.x;
    drawingLineMetrics.y = lineMetrics.y;
    drawingLineMetrics.startIndex = lineMetrics.startIndex;
    drawingLineMetrics.endIndex = lineMetrics.endIndex;

    ConvertFontMetrics(lineMetrics.firstCharMetrics, drawingLineMetrics.firstCharMetrics);
}

bool OH_Drawing_TypographyGetLineInfo(OH_Drawing_Typography* typography,
    int lineNumber, bool oneLine, bool includeWhitespace, OH_Drawing_LineMetrics* drawingLineMetrics)
{
    Typography* typographyInner = ConvertToOriginalText<Typography>(typography);
    if (typographyInner == nullptr || drawingLineMetrics == nullptr) {
        return false;
    }

    LineMetrics lineMetrics;
    if (!typographyInner->GetLineInfo(lineNumber, oneLine, includeWhitespace, &lineMetrics)) {
        return false;
    }

    ConvertLineMetrics(lineMetrics, *drawingLineMetrics);
    return true;
}

void OH_Drawing_SetTextStyleForegroundBrush(OH_Drawing_TextStyle* style, OH_Drawing_Brush* foregroundBrush)
{
    if (style == nullptr || foregroundBrush == nullptr) {
        return;
    }
    ConvertToOriginalText<TextStyle>(style)->foregroundBrush = *reinterpret_cast<Drawing::Brush*>(foregroundBrush);
}

void OH_Drawing_TextStyleGetForegroundBrush(OH_Drawing_TextStyle* style, OH_Drawing_Brush* foregroundBrush)
{
    if (style == nullptr || foregroundBrush == nullptr) {
        return;
    }

    Drawing::Brush* brush = reinterpret_cast<Drawing::Brush*>(foregroundBrush);
    *brush = *ConvertToOriginalText<TextStyle>(style)->foregroundBrush;
}

void OH_Drawing_SetTextStyleForegroundPen(OH_Drawing_TextStyle* style, OH_Drawing_Pen* foregroundPen)
{
    if (style == nullptr || foregroundPen == nullptr) {
        return;
    }
    ConvertToOriginalText<TextStyle>(style)->foregroundPen = *reinterpret_cast<Drawing::Pen*>(foregroundPen);
}

void OH_Drawing_TextStyleGetForegroundPen(OH_Drawing_TextStyle* style, OH_Drawing_Pen* foregroundPen)
{
    if (style == nullptr || foregroundPen == nullptr) {
        return;
    }

    Drawing::Pen* pen = reinterpret_cast<Drawing::Pen*>(foregroundPen);
    *pen = *ConvertToOriginalText<TextStyle>(style)->foregroundPen;
}

void OH_Drawing_SetTextStyleBackgroundBrush(OH_Drawing_TextStyle* style, OH_Drawing_Brush* backgroundBrush)
{
    if (style == nullptr || backgroundBrush == nullptr) {
        return;
    }
    ConvertToOriginalText<TextStyle>(style)->backgroundBrush = *reinterpret_cast<Drawing::Brush*>(backgroundBrush);
}

void OH_Drawing_TextStyleGetBackgroundBrush(OH_Drawing_TextStyle* style, OH_Drawing_Brush* backgroundBrush)
{
    if (style == nullptr || backgroundBrush == nullptr) {
        return;
    }

    Drawing::Brush* brush = reinterpret_cast<Drawing::Brush*>(backgroundBrush);
    *brush = *ConvertToOriginalText<TextStyle>(style)->backgroundBrush;
}

void OH_Drawing_SetTextStyleBackgroundPen(OH_Drawing_TextStyle* style, OH_Drawing_Pen* backgroundPen)
{
    if (style == nullptr || backgroundPen == nullptr) {
        return;
    }
    ConvertToOriginalText<TextStyle>(style)->backgroundPen = *reinterpret_cast<Drawing::Pen*>(backgroundPen);
}

void OH_Drawing_TextStyleGetBackgroundPen(OH_Drawing_TextStyle* style, OH_Drawing_Pen* backgroundPen)
{
    if (style == nullptr || backgroundPen == nullptr) {
        return;
    }

    Drawing::Pen* pen = reinterpret_cast<Drawing::Pen*>(backgroundPen);
    *pen = *ConvertToOriginalText<TextStyle>(style)->backgroundPen;
}

OH_Drawing_FontDescriptor* OH_Drawing_CreateFontDescriptor(void)
{
    auto fontDescriptor = new TextEngine::FontParser::FontDescriptor;
    if (fontDescriptor == nullptr) {
        return nullptr;
    }
    return (OH_Drawing_FontDescriptor*)fontDescriptor;
}

void OH_Drawing_DestroyFontDescriptor(OH_Drawing_FontDescriptor* descriptor)
{
    if (descriptor) {
        delete ConvertToOriginalText<TextEngine::FontParser::FontDescriptor>(descriptor);
        descriptor = nullptr;
    }
}

OH_Drawing_FontParser* OH_Drawing_CreateFontParser(void)
{
    auto fontParser = new TextEngine::FontParser;
    if (fontParser == nullptr) {
        return nullptr;
    }
    return (OH_Drawing_FontParser*)fontParser;
}

void OH_Drawing_DestroyFontParser(OH_Drawing_FontParser* parser)
{
    if (parser) {
        delete ConvertToOriginalText<TextEngine::FontParser>(parser);
        parser = nullptr;
    }
}

static bool CopyStrData(char** destination, const std::string& source)
{
    if (destination == nullptr || source.empty()) {
        return false;
    }
    size_t destinationSize = source.size() + 1;
    *destination = new char[destinationSize];
    if (*destination == nullptr) {
        return false;
    }
    auto retMemset = memset_s(*destination, destinationSize, '\0', destinationSize);
    if (retMemset != 0) {
        delete[] *destination;
        return false;
    }
    auto retCopy = strcpy_s(*destination, destinationSize, source.c_str());
    if (retCopy != 0) {
        delete[] *destination;
        return false;
    }
    return true;
}

char** OH_Drawing_FontParserGetSystemFontList(OH_Drawing_FontParser* fontParser, size_t* num)
{
    if (fontParser == nullptr || num == nullptr) {
        return nullptr;
    }
    char** fontList = nullptr;
    icu::Locale locale = icu::Locale::getDefault();
    std::vector<TextEngine::FontParser::FontDescriptor> systemFontList =
        ConvertToOriginalText<TextEngine::FontParser>(fontParser)->GetVisibilityFonts(std::string(locale.getName()));
    fontList = new char* [systemFontList.size()];
    if (fontList == nullptr) {
        return nullptr;
    }
    for (size_t i = 0; i < systemFontList.size(); ++i) {
        fontList[i] = nullptr;
        bool res = CopyStrData(&fontList[i], systemFontList[i].fullName);
        if (!res) {
            for (size_t j = i; j >= 0; j--) {
                delete fontList[j];
                fontList[j] = nullptr;
            }
            delete[] fontList;
            fontList = nullptr;
            return nullptr;
        }
    }
    *num = systemFontList.size();
    return fontList;
}

void OH_Drawing_DestroySystemFontList(char** fontList, size_t num)
{
    if (fontList == nullptr) {
        return;
    }
    for (size_t i = 0; i < num; ++i) {
        delete[] fontList[i];
        fontList[i] = nullptr;
    }
    delete[] fontList;
    fontList = nullptr;
}

OH_Drawing_FontDescriptor* OH_Drawing_FontParserGetFontByName(OH_Drawing_FontParser* fontParser, const char* name)
{
    if (fontParser == nullptr || name == nullptr) {
        return nullptr;
    }
    std::vector<TextEngine::FontParser::FontDescriptor> systemFontList =
        ConvertToOriginalText<TextEngine::FontParser>(fontParser)->GetVisibilityFonts();
    TextEngine::FontParser::FontDescriptor *descriptor = new TextEngine::FontParser::FontDescriptor;
    for (size_t i = 0; i < systemFontList.size(); ++i) {
        if (strcmp(name, systemFontList[i].fullName.c_str()) == 0) {
            *descriptor = systemFontList[i];
            return (OH_Drawing_FontDescriptor*)descriptor;
        }
    }
    return nullptr;
}

OH_Drawing_LineMetrics* OH_Drawing_TypographyGetLineMetrics(OH_Drawing_Typography* typography)
{
    if (typography == nullptr) {
        return nullptr;
    }
    std::vector<LineMetrics>* lineMetrics = new std::vector<LineMetrics>;
    if (lineMetrics == nullptr) {
        return nullptr;
    }
    *lineMetrics = ConvertToOriginalText<Typography>(typography)->GetLineMetrics();
    return (OH_Drawing_LineMetrics*)lineMetrics;
}

size_t OH_Drawing_LineMetricsGetSize(OH_Drawing_LineMetrics* lineMetrics)
{
    if (lineMetrics == nullptr) {
        return 0;
    }
    std::vector<LineMetrics>* innerLineMetrics = ConvertToOriginalText<std::vector<LineMetrics>>(lineMetrics);
    return innerLineMetrics->size();
}

void OH_Drawing_DestroyLineMetrics(OH_Drawing_LineMetrics* lineMetrics)
{
    if (lineMetrics) {
        delete ConvertToOriginalText<std::vector<LineMetrics>>(lineMetrics);
        lineMetrics = nullptr;
    }
}

bool OH_Drawing_TypographyGetLineMetricsAt(OH_Drawing_Typography* typography, int lineNumber,
    OH_Drawing_LineMetrics* lineMetric)
{
    if (typography == nullptr || lineMetric == nullptr) {
        return false;
    }
    LineMetrics* metric = ConvertToOriginalText<LineMetrics>(lineMetric);
    if (ConvertToOriginalText<Typography>(typography)->GetLineMetricsAt(lineNumber, metric)) {
        return true;
    }
    return false;
}

float OH_Drawing_TypographyGetIndentsWithIndex(OH_Drawing_Typography* typography, int index)
{
    if (typography == nullptr || index < 0) {
        return 0.0;
    }
    Typography* innerTypography = ConvertToOriginalText<Typography>(typography);
    if (innerTypography == nullptr) {
        return 0.0;
    }
    return innerTypography->DetectIndents(static_cast<size_t>(index));
}

void OH_Drawing_TypographySetIndents(OH_Drawing_Typography* typography, int indentsNumber, const float indents[])
{
    if (typography == nullptr || indents == nullptr) {
        return;
    }
    std::vector<float> rosenIndents;
    for (int i = 0; i < indentsNumber; i++) {
        rosenIndents.emplace_back(indents[i]);
    }
    ConvertToOriginalText<Typography>(typography)->SetIndents(rosenIndents);
}

OH_Drawing_TextShadow* OH_Drawing_CreateTextShadow(void)
{
    return (OH_Drawing_TextShadow*)new TextShadow;
}

void OH_Drawing_DestroyTextShadow(OH_Drawing_TextShadow* shadow)
{
    if (shadow == nullptr) {
        return;
    }
    delete ConvertToOriginalText<TextShadow>(shadow);
    shadow = NULL;
}

int OH_Drawing_TextStyleGetShadowCount(OH_Drawing_TextStyle* style)
{
    if (style == nullptr) {
        return 0;
    }
    return ConvertToOriginalText<TextStyle>(style)->shadows.size();
}

OH_Drawing_TextShadow* OH_Drawing_TextStyleGetShadows(OH_Drawing_TextStyle* style)
{
    if (style) {
        std::vector<TextShadow>* originalShadows = new std::vector<TextShadow>;
        *originalShadows = ConvertToOriginalText<TextStyle>(style)->shadows;
        return (OH_Drawing_TextShadow*)originalShadows;
    }
    return nullptr;
}

void OH_Drawing_TextStyleAddShadow(OH_Drawing_TextStyle* style, OH_Drawing_TextShadow* shadow)
{
    if (shadow == nullptr || style == nullptr) {
        return;
    }
    ConvertToOriginalText<TextStyle>(style)->shadows.emplace_back(*(ConvertToOriginalText<TextShadow>(shadow)));
}

void OH_Drawing_TextStyleClearShadows(OH_Drawing_TextStyle* style)
{
    if (style) {
        ConvertToOriginalText<TextStyle>(style)->shadows.clear();
    }
}

OH_Drawing_TextShadow* OH_Drawing_TextStyleGetShadowWithIndex(OH_Drawing_TextStyle* style, int index)
{
    if (style == nullptr || index < 0) {
        return nullptr;
    }
    TextShadow* elementShadow = new TextShadow;
    if (index >= (ConvertToOriginalText<TextStyle>(style)->shadows.size())) {
        return nullptr;
    }
    elementShadow = &(ConvertToOriginalText<TextStyle>(style)->shadows.at(index));
    return (OH_Drawing_TextShadow*)(elementShadow);
}

void OH_Drawing_DestroyTextShadows(OH_Drawing_TextShadow* shadow)
{
    if (shadow == nullptr) {
        return;
    }
    delete ConvertToOriginalText<std::vector<TextShadow>>(shadow);
    shadow = NULL;
}

void OH_Drawing_SetTypographyTextFontWeight(OH_Drawing_TypographyStyle* style, int weight)
{
    if (style == nullptr) {
        return;
    }
    FontWeight fontWeight;
    switch (weight) {
        case FONT_WEIGHT_100: {
            fontWeight = FontWeight::W100;
            break;
        }
        case FONT_WEIGHT_200: {
            fontWeight = FontWeight::W200;
            break;
        }
        case FONT_WEIGHT_300: {
            fontWeight = FontWeight::W300;
            break;
        }
        case FONT_WEIGHT_400: {
            fontWeight = FontWeight::W400;
            break;
        }
        case FONT_WEIGHT_500: {
            fontWeight = FontWeight::W500;
            break;
        }
        case FONT_WEIGHT_600: {
            fontWeight = FontWeight::W600;
            break;
        }
        case FONT_WEIGHT_700: {
            fontWeight = FontWeight::W700;
            break;
        }
        case FONT_WEIGHT_800: {
            fontWeight = FontWeight::W800;
            break;
        }
        case FONT_WEIGHT_900: {
            fontWeight = FontWeight::W900;
            break;
        }
        default: {
            fontWeight = FontWeight::W400;
        }
    }
    ConvertToOriginalText<TypographyStyle>(style)->fontWeight = fontWeight;
}

void OH_Drawing_SetTypographyTextFontStyle(OH_Drawing_TypographyStyle* style, int fontStyle)
{
    if (style == nullptr) {
        return;
    }
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
    ConvertToOriginalText<TypographyStyle>(style)->fontStyle = rosenFontStyle;
}

void OH_Drawing_SetTypographyTextFontFamily(OH_Drawing_TypographyStyle* style, const char* fontFamily)
{
    if (style) {
        ConvertToOriginalText<TypographyStyle>(style)->fontFamily = fontFamily;
    }
}

void OH_Drawing_SetTypographyTextFontSize(OH_Drawing_TypographyStyle* style, double fontSize)
{
    if (style) {
        ConvertToOriginalText<TypographyStyle>(style)->fontSize = fontSize;
    }
}

void OH_Drawing_SetTypographyTextFontHeight(OH_Drawing_TypographyStyle* style, double fontHeight)
{
    if (style) {
        ConvertToOriginalText<TypographyStyle>(style)->heightScale = fontHeight;
        ConvertToOriginalText<TypographyStyle>(style)->heightOnly = true;
    }
}

void OH_Drawing_SetTypographyTextHalfLeading(OH_Drawing_TypographyStyle* style, bool halfLeading)
{
    if (style) {
        ConvertToOriginalText<TypographyStyle>(style)->halfLeading = halfLeading;
    }
}

void OH_Drawing_SetTypographyTextUseLineStyle(OH_Drawing_TypographyStyle* style, bool useLineStyle)
{
    if (style) {
        ConvertToOriginalText<TypographyStyle>(style)->useLineStyle = useLineStyle;
    }
}

void OH_Drawing_SetTypographyTextLineStyleFontWeight(OH_Drawing_TypographyStyle* style, int weight)
{
    if (style == nullptr) {
        return;
    }
    FontWeight fontWeight;
    switch (weight) {
        case FONT_WEIGHT_100: {
            fontWeight = FontWeight::W100;
            break;
        }
        case FONT_WEIGHT_200: {
            fontWeight = FontWeight::W200;
            break;
        }
        case FONT_WEIGHT_300: {
            fontWeight = FontWeight::W300;
            break;
        }
        case FONT_WEIGHT_400: {
            fontWeight = FontWeight::W400;
            break;
        }
        case FONT_WEIGHT_500: {
            fontWeight = FontWeight::W500;
            break;
        }
        case FONT_WEIGHT_600: {
            fontWeight = FontWeight::W600;
            break;
        }
        case FONT_WEIGHT_700: {
            fontWeight = FontWeight::W700;
            break;
        }
        case FONT_WEIGHT_800: {
            fontWeight = FontWeight::W800;
            break;
        }
        case FONT_WEIGHT_900: {
            fontWeight = FontWeight::W900;
            break;
        }
        default: {
            fontWeight = FontWeight::W400;
        }
    }
    ConvertToOriginalText<TypographyStyle>(style)->lineStyleFontWeight = fontWeight;
}

void OH_Drawing_SetTypographyTextLineStyleFontStyle(OH_Drawing_TypographyStyle* style, int fontStyle)
{
    if (style == nullptr) {
        return;
    }
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
    ConvertToOriginalText<TypographyStyle>(style)->lineStyleFontStyle = rosenFontStyle;
}

void OH_Drawing_SetTypographyTextLineStyleFontFamilies(OH_Drawing_TypographyStyle* style,
    int fontFamiliesNumber, const char* fontFamilies[])
{
    if (style != nullptr && fontFamilies != nullptr) {
        std::vector<std::string> rosenFontFamilies;
        for (int i = 0; i < fontFamiliesNumber; i++) {
            rosenFontFamilies.emplace_back(fontFamilies[i]);
        }
    ConvertToOriginalText<TypographyStyle>(style)->lineStyleFontFamilies = rosenFontFamilies;
    }
}

void OH_Drawing_SetTypographyTextLineStyleFontSize(OH_Drawing_TypographyStyle* style, double lineStyleFontSize)
{
    if (style == nullptr) {
        return;
    }
    TypographyStyle* typoStyle = ConvertToOriginalText<TypographyStyle>(style);
    if (typoStyle == nullptr) {
        return;
    }
    typoStyle->lineStyleFontSize = lineStyleFontSize;
}

void OH_Drawing_SetTypographyTextLineStyleFontHeight(OH_Drawing_TypographyStyle* style, double lineStyleFontHeight)
{
    if (style == nullptr) {
        return;
    }
    TypographyStyle* typographyStyle = ConvertToOriginalText<TypographyStyle>(style);
    if (typographyStyle == nullptr) {
        return;
    }
    typographyStyle->lineStyleHeightScale = lineStyleFontHeight;
    if (!typographyStyle->lineStyleHeightOnlyInit) {
        typographyStyle->lineStyleHeightOnly = true;
    }
}

void OH_Drawing_SetTypographyTextLineStyleHalfLeading(OH_Drawing_TypographyStyle* style, bool lineStyleHalfLeading)
{
    if (style == nullptr) {
        return;
    }
    TypographyStyle* typoStyle = ConvertToOriginalText<TypographyStyle>(style);
    if (typoStyle == nullptr) {
        return;
    }
    typoStyle->lineStyleHalfLeading = lineStyleHalfLeading;
}

void OH_Drawing_SetTypographyTextLineStyleSpacingScale(OH_Drawing_TypographyStyle* style, double spacingScale)
{
    if (style) {
        ConvertToOriginalText<TypographyStyle>(style)->lineStyleSpacingScale = spacingScale;
    }
}

void OH_Drawing_SetTypographyTextLineStyleOnly(OH_Drawing_TypographyStyle* style, bool lineStyleOnly)
{
    if (style) {
        ConvertToOriginalText<TypographyStyle>(style)->lineStyleOnly = lineStyleOnly;
    }
}

bool OH_Drawing_TextStyleGetFontMetrics(OH_Drawing_Typography* typography, OH_Drawing_TextStyle* style,
    OH_Drawing_Font_Metrics* fontmetrics)
{
    bool startGetMetrics = false;
    if (!typography || !style || !fontmetrics) {
        return false;
    }
    auto textstyle = ConvertToOriginalText<const OHOS::Rosen::TextStyle>(style);
    auto txtSKTypograph = ConvertToOriginalText<Typography>(typography);
    const OHOS::Rosen::Drawing::FontMetrics fontmetricsResult = txtSKTypograph->GetFontMetrics(*textstyle);
    ConvertFontMetrics(fontmetricsResult, *fontmetrics);
    startGetMetrics = true;
    return startGetMetrics;
}

void OH_Drawing_SetTypographyTextStyle(OH_Drawing_TypographyStyle* handler, OH_Drawing_TextStyle* style)
{
    if (!handler || !style) {
        return;
    }
    auto rosenTypographStyle = ConvertToOriginalText<OHOS::Rosen::TypographyStyle>(handler);
    auto rosenTextStyle = ConvertToOriginalText<OHOS::Rosen::TextStyle>(style);
    rosenTypographStyle->SetTextStyle(*rosenTextStyle);
    return;
}

void OH_Drawing_SetTypographyTextLocale(OH_Drawing_TypographyStyle* style, const char* locale)
{
#ifndef USE_GRAPHIC_TEXT_GINE
    ConvertToOriginalText<TypographyStyle>(style)->locale_ = locale;
#else
    ConvertToOriginalText<TypographyStyle>(style)->locale = locale;
#endif
}

void OH_Drawing_SetTypographyTextSplitRatio(OH_Drawing_TypographyStyle* style, float textSplitRatio)
{
    ConvertToOriginalText<TypographyStyle>(style)->textSplitRatio = textSplitRatio;
}

OH_Drawing_TextStyle* OH_Drawing_TypographyGetTextStyle(OH_Drawing_TypographyStyle* style)
{
#ifndef USE_GRAPHIC_TEXT_GINE
    TextStyle* originalTextStyle = new TextStyle;
    *originalTextStyle = ConvertToOriginalText<TypographyStyle>(style)->GetTextStyle();
#else
    TextStyle* originalTextStyle = new TextStyle;
    *originalTextStyle = ConvertToOriginalText<TypographyStyle>(style)->GetTextStyle();
#endif
    return (OH_Drawing_TextStyle*)originalTextStyle;
}

int OH_Drawing_TypographyGetEffectiveAlignment(OH_Drawing_TypographyStyle* style)
{
#ifndef USE_GRAPHIC_TEXT_GINE
    TextAlign originalTextAlign = ConvertToOriginalText<TypographyStyle>(style)->EffectiveAlign();
#else
    TextAlign originalTextAlign = ConvertToOriginalText<TypographyStyle>(style)->GetEffectiveAlign();
#endif
    return static_cast<int> (originalTextAlign);
}

bool OH_Drawing_TypographyIsLineUnlimited(OH_Drawing_TypographyStyle* style)
{
#ifndef USE_GRAPHIC_TEXT_GINE
    return ConvertToOriginalText<TypographyStyle>(style)->UnlimitedLines();
#else
    return ConvertToOriginalText<TypographyStyle>(style)->IsUnlimitedLines();
#endif
}

bool OH_Drawing_TypographyIsEllipsized(OH_Drawing_TypographyStyle* style)
{
#ifndef USE_GRAPHIC_TEXT_GINE
    return ConvertToOriginalText<TypographyStyle>(style)->Ellipsized();
#else
    return ConvertToOriginalText<TypographyStyle>(style)->IsEllipsized();
#endif
}

void OH_Drawing_SetTypographyTextEllipsis(OH_Drawing_TypographyStyle* style, const char* ellipsis)
{
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
    std::u16string u16Ellipsis = converter.from_bytes(ellipsis);
#ifndef USE_GRAPHIC_TEXT_GINE
    ConvertToOriginalText<TypographyStyle>(style)->ellipsis_ = u16Ellipsis;
#else
    ConvertToOriginalText<TypographyStyle>(style)->ellipsis = u16Ellipsis;
#endif
}

void OH_Drawing_TextStyleSetBackgroundRect(OH_Drawing_TextStyle* style, const OH_Drawing_RectStyle_Info* rectStyleInfo,
    int styleId)
{
    if (style == nullptr || rectStyleInfo == nullptr) {
        return;
    }
    TextStyle* convertStyle = ConvertToOriginalText<TextStyle>(style);
    if (convertStyle == nullptr) {
        return;
    }
    RectStyle& rectStyle = convertStyle->backgroundRect;
    rectStyle.color = rectStyleInfo->color;
    rectStyle.leftTopRadius = rectStyleInfo->leftTopRadius;
    rectStyle.rightTopRadius = rectStyleInfo->rightTopRadius;
    rectStyle.rightBottomRadius = rectStyleInfo->rightBottomRadius;
    rectStyle.leftBottomRadius = rectStyleInfo->leftBottomRadius;
    convertStyle->styleId = styleId;
}

void OH_Drawing_TypographyHandlerAddSymbol(OH_Drawing_TypographyCreate* handler, uint32_t symbol)
{
    if (!handler) {
        return;
    }
    TypographyCreate* convertHandler = ConvertToOriginalText<TypographyCreate>(handler);
    if (convertHandler) {
        convertHandler->AppendSymbol(symbol);
    }
}

void OH_Drawing_TextStyleAddFontFeature(OH_Drawing_TextStyle* style, const char* tag, int value)
{
    if (style == nullptr || tag == nullptr) {
        return;
    }
    TextStyle* convertStyle = ConvertToOriginalText<TextStyle>(style);
    if (convertStyle) {
        convertStyle->fontFeatures.SetFeature(tag, value);
    }
}

size_t OH_Drawing_TextStyleGetFontFeatureSize(OH_Drawing_TextStyle* style)
{
    if (style == nullptr) {
        return 0;
    }
    TextStyle* convertStyle = ConvertToOriginalText<TextStyle>(style);
    if (convertStyle) {
        return (convertStyle->fontFeatures.GetFontFeatures()).size();
    }
    return 0;
}

void OH_Drawing_TextStyleClearFontFeature(OH_Drawing_TextStyle* style)
{
    if (style == nullptr) {
        return;
    }
    TextStyle* convertStyle = ConvertToOriginalText<TextStyle>(style);
    if (convertStyle == nullptr) {
        return;
    }
    convertStyle->fontFeatures.Clear();
}

OH_Drawing_FontFeature* OH_Drawing_TextStyleGetFontFeatures(OH_Drawing_TextStyle* style)
{
    TextStyle* convertStyle = ConvertToOriginalText<TextStyle>(style);
    if (style == nullptr || convertStyle == nullptr) {
        return nullptr;
    }
    auto& originMap = convertStyle->fontFeatures.GetFontFeatures();
    size_t fontFeatureSize = originMap.size();
    if (fontFeatureSize <= 0) {
        return nullptr;
    }
    OH_Drawing_FontFeature *fontFeatureArray = new OH_Drawing_FontFeature[fontFeatureSize];
    size_t index = 0;
    for (auto& kv : originMap) {
        (fontFeatureArray + index)->tag = new char[(kv.first).size() + 1];
        auto result = strcpy_s((fontFeatureArray + index)->tag, ((kv.first).size() + 1), (kv.first).c_str());
        if (result != 0) {
            OH_Drawing_TextStyleDestroyFontFeatures(fontFeatureArray, index);
            return nullptr;
        }
        (fontFeatureArray + index)->value = kv.second;
        index++;
    }
    return fontFeatureArray;
}

void OH_Drawing_TextStyleDestroyFontFeatures(OH_Drawing_FontFeature* fontFeature, size_t fontFeatureSize)
{
    if (fontFeature == nullptr) {
        return;
    }
    for (int i = 0; i < fontFeatureSize; i++) {
        if ((fontFeature + i)->tag == nullptr) {
            continue;
        }
        delete[] (fontFeature + i)->tag;
        (fontFeature + i)->tag = nullptr;
    }
    delete[] fontFeature;
    fontFeature = nullptr;
}

void OH_Drawing_TextStyleSetBaselineShift(OH_Drawing_TextStyle* style, double lineShift)
{
    if (style == nullptr) {
        return;
    }
    TextStyle* convertStyle = ConvertToOriginalText<TextStyle>(style);
    if (convertStyle) {
        convertStyle->baseLineShift = lineShift;
    }
}

double OH_Drawing_TextStyleGetBaselineShift(OH_Drawing_TextStyle* style)
{
    TextStyle* convertStyle = ConvertToOriginalText<TextStyle>(style);
    if (convertStyle == nullptr) {
        return 0.0;
    }
    return convertStyle->baseLineShift;
}

uint32_t OH_Drawing_TextStyleGetColor(OH_Drawing_TextStyle* style)
{
    if (style == nullptr) {
        return 0xFFFFFFFF;
    }
    TextStyle* textStyle = ConvertToOriginalText<TextStyle>(style);
    if (textStyle == nullptr) {
        return 0xFFFFFFFF;
    }
    return textStyle->color.CastToColorQuad();
}

OH_Drawing_TextDecorationStyle OH_Drawing_TextStyleGetDecorationStyle(OH_Drawing_TextStyle* style)
{
    if (style == nullptr) {
        return TEXT_DECORATION_STYLE_SOLID;
    }
    TextStyle* textStyle = ConvertToOriginalText<TextStyle>(style);
    if (textStyle == nullptr) {
        return TEXT_DECORATION_STYLE_SOLID;
    }
    return static_cast<OH_Drawing_TextDecorationStyle>(textStyle->decorationStyle);
}

OH_Drawing_FontWeight OH_Drawing_TextStyleGetFontWeight(OH_Drawing_TextStyle* style)
{
    if (style == nullptr) {
        return FONT_WEIGHT_400;
    }
    TextStyle* textStyle = ConvertToOriginalText<TextStyle>(style);
    if (textStyle == nullptr) {
        return FONT_WEIGHT_400;
    }
    return static_cast<OH_Drawing_FontWeight>(textStyle->fontWeight);
}

OH_Drawing_FontStyle OH_Drawing_TextStyleGetFontStyle(OH_Drawing_TextStyle* style)
{
    if (style == nullptr) {
        return FONT_STYLE_NORMAL;
    }
    TextStyle* textStyle = ConvertToOriginalText<TextStyle>(style);
    if (textStyle == nullptr) {
        return FONT_STYLE_NORMAL;
    }
    return static_cast<OH_Drawing_FontStyle>(textStyle->fontStyle);
}

OH_Drawing_TextBaseline OH_Drawing_TextStyleGetBaseLine(OH_Drawing_TextStyle* style)
{
    if (style == nullptr) {
        return TEXT_BASELINE_ALPHABETIC;
    }
    TextStyle* textStyle = ConvertToOriginalText<TextStyle>(style);
    if (textStyle == nullptr) {
        return TEXT_BASELINE_ALPHABETIC;
    }
    return static_cast<OH_Drawing_TextBaseline>(textStyle->baseline);
}

char** OH_Drawing_TextStyleGetFontFamilies(OH_Drawing_TextStyle* style, size_t* num)
{
    if (style == nullptr || num == nullptr || ConvertToOriginalText<TextStyle>(style) == nullptr) {
        return nullptr;
    }
    TextStyle* textStyle = ConvertToOriginalText<TextStyle>(style);
    char** fontFamilies = nullptr;
    std::vector<std::string>& textStyleFontFamilies = textStyle->fontFamilies;
    fontFamilies = new char* [textStyleFontFamilies.size()];
    if (fontFamilies == nullptr) {
        return nullptr;
    }
    for (size_t i = 0; i < textStyleFontFamilies.size(); ++i) {
        bool res = CopyStrData(&fontFamilies[i], textStyleFontFamilies[i]);
        if (!res) {
            for (size_t j = 0; j < i; j++) {
                delete[] fontFamilies[j];
                fontFamilies[j] = nullptr;
            }
            delete[] fontFamilies;
            fontFamilies = nullptr;
            return nullptr;
        }
    }
    *num = textStyleFontFamilies.size();
    return fontFamilies;
}

void OH_Drawing_TextStyleDestroyFontFamilies(char** fontFamilies, size_t num)
{
    if (fontFamilies == nullptr) {
        return;
    }
    for (size_t i = 0; i < num; ++i) {
        if (fontFamilies[i] != nullptr) {
            delete[] fontFamilies[i];
            fontFamilies[i] = nullptr;
        }
    }
    delete[] fontFamilies;
    fontFamilies = nullptr;
}

double OH_Drawing_TextStyleGetFontSize(OH_Drawing_TextStyle* style)
{
    if (style == nullptr) {
        return 0.0;
    }
    TextStyle* textStyle = ConvertToOriginalText<TextStyle>(style);
    if (textStyle == nullptr) {
        return 0.0;
    }
    return textStyle->fontSize;
}

double OH_Drawing_TextStyleGetLetterSpacing(OH_Drawing_TextStyle* style)
{
    if (style == nullptr) {
        return 0.0;
    }
    TextStyle* textStyle = ConvertToOriginalText<TextStyle>(style);
    if (textStyle == nullptr) {
        return 0.0;
    }
    return textStyle->letterSpacing;
}

double OH_Drawing_TextStyleGetWordSpacing(OH_Drawing_TextStyle* style)
{
    if (style == nullptr) {
        return 0.0;
    }
    TextStyle* textStyle = ConvertToOriginalText<TextStyle>(style);
    if (textStyle == nullptr) {
        return 0.0;
    }
    return textStyle->wordSpacing;
}

double OH_Drawing_TextStyleGetFontHeight(OH_Drawing_TextStyle* style)
{
    if (style == nullptr) {
        return 0.0;
    }
    TextStyle* textStyle = ConvertToOriginalText<TextStyle>(style);
    if (textStyle == nullptr) {
        return 0.0;
    }
    return textStyle->heightScale;
}

bool OH_Drawing_TextStyleGetHalfLeading(OH_Drawing_TextStyle* style)
{
    if (style == nullptr) {
        return false;
    }
    TextStyle* textStyle = ConvertToOriginalText<TextStyle>(style);
    if (textStyle == nullptr) {
        return false;
    }
    return textStyle->halfLeading;
}

const char* OH_Drawing_TextStyleGetLocale(OH_Drawing_TextStyle* style)
{
    if (style == nullptr) {
        return nullptr;
    }
    TextStyle* textStyle = ConvertToOriginalText<TextStyle>(style);
    if (textStyle == nullptr) {
        return nullptr;
    }
    return textStyle->locale.c_str();
}

void OH_Drawing_TypographyTextSetHeightMode(OH_Drawing_TypographyStyle* style, OH_Drawing_TextHeightBehavior heightMode)
{
    TypographyStyle* convertStyle = ConvertToOriginalText<TypographyStyle>(style);
    if (style == nullptr || convertStyle == nullptr) {
        return;
    }
    TextHeightBehavior rosenHeightBehavior;
    switch (heightMode) {
        case TEXT_HEIGHT_ALL: {
            rosenHeightBehavior = TextHeightBehavior::ALL;
            break;
        }
        case TEXT_HEIGHT_DISABLE_FIRST_ASCENT: {
            rosenHeightBehavior = TextHeightBehavior::DISABLE_FIRST_ASCENT;
            break;
        }
        case TEXT_HEIGHT_DISABLE_LAST_ASCENT: {
            rosenHeightBehavior = TextHeightBehavior::DISABLE_LAST_ASCENT;
            break;
        }
        case TEXT_HEIGHT_DISABLE_ALL: {
            rosenHeightBehavior = TextHeightBehavior::DISABLE_ALL;
            break;
        }
        default: {
            rosenHeightBehavior = TextHeightBehavior::ALL;
        }
    }
    convertStyle->textHeightBehavior = rosenHeightBehavior;
}

OH_Drawing_TextHeightBehavior OH_Drawing_TypographyTextGetHeightMode(OH_Drawing_TypographyStyle* style)
{
    TypographyStyle* convertStyle = ConvertToOriginalText<TypographyStyle>(style);
    if (style == nullptr || convertStyle == nullptr) {
        return TEXT_HEIGHT_ALL;
    }
    return static_cast<OH_Drawing_TextHeightBehavior>(ConvertToOriginalText<TypographyStyle>(style)->textHeightBehavior);
}