
/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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
#include "font_config.h"
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
    auto drawingCanvas = reinterpret_cast<OHOS::Rosen::Drawing::Canvas*>(canvas);
    if (drawingCanvas && drawingCanvas->GetDrawingType() == OHOS::Rosen::Drawing::DrawingType::RECORDING) {
        (static_cast<OHOS::Rosen::Drawing::RecordingCanvas*>(drawingCanvas))->SetIsCustomTypeface(true);
    }
    ConvertToOriginalText<Typography>(typography)->Paint(drawingCanvas,
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

static void SetFontConfigInfoErrorCode(const OH_Drawing_FontConfigInfoErrorCode srcCode,
    OH_Drawing_FontConfigInfoErrorCode* dstCode)
{
    if (!dstCode) {
        return;
    }
    *dstCode = srcCode;
}

static bool CopyStrData(char** destination, const std::string& source,
    OH_Drawing_FontConfigInfoErrorCode* code = nullptr)
{
    if (destination == nullptr || source.empty()) {
        SetFontConfigInfoErrorCode(OH_Drawing_FontConfigInfoErrorCode::COPY_STRING_DATA_ERROR, code);
        return false;
    }
    size_t destinationSize = source.size() + 1;
    *destination = new char[destinationSize];
    if (!(*destination)) {
        SetFontConfigInfoErrorCode(OH_Drawing_FontConfigInfoErrorCode::ALLOC_MEMORY_ERROR, code);
        return false;
    }
    auto retCopy = strcpy_s(*destination, destinationSize, source.c_str());
    if (retCopy != 0) {
        delete[] *destination;
        *destination = nullptr;
        SetFontConfigInfoErrorCode(OH_Drawing_FontConfigInfoErrorCode::COPY_STRING_DATA_ERROR, code);
        return false;
    }
    SetFontConfigInfoErrorCode(OH_Drawing_FontConfigInfoErrorCode::OK, code);
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
        if (fontList[i] == nullptr) {
            continue;
        }
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

void OH_Drawing_TypographyTextSetHeightBehavior(OH_Drawing_TypographyStyle* style,
                                                OH_Drawing_TextHeightBehavior heightMode)
{
    TypographyStyle* convertStyle = ConvertToOriginalText<TypographyStyle>(style);
    if (convertStyle == nullptr) {
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

OH_Drawing_TextHeightBehavior OH_Drawing_TypographyTextGetHeightBehavior(OH_Drawing_TypographyStyle* style)
{
    TypographyStyle* convertStyle = ConvertToOriginalText<TypographyStyle>(style);
    if (convertStyle == nullptr) {
        return TEXT_HEIGHT_ALL;
    }
    TextHeightBehavior innerHeightBehavior = ConvertToOriginalText<TypographyStyle>(style)->textHeightBehavior;
    return static_cast<OH_Drawing_TextHeightBehavior>(innerHeightBehavior);
}

void OH_Drawing_TypographyMarkDirty(OH_Drawing_Typography* typography)
{
    if (typography == nullptr || ConvertToOriginalText<Typography>(typography) == nullptr) {
        return;
    }
    ConvertToOriginalText<Typography>(typography)->MarkDirty();
}

int32_t OH_Drawing_TypographyGetUnresolvedGlyphsCount(OH_Drawing_Typography* typography)
{
    if (typography == nullptr || ConvertToOriginalText<Typography>(typography) == nullptr) {
        return 0;
    }
    return ConvertToOriginalText<Typography>(typography)->GetUnresolvedGlyphsCount();
}

void OH_Drawing_TypographyUpdateFontSize(OH_Drawing_Typography* typography, size_t from, size_t to, float fontSize)
{
    if (typography == nullptr || ConvertToOriginalText<Typography>(typography) == nullptr) {
        return;
    }
    ConvertToOriginalText<Typography>(typography)->UpdateFontSize(from, to, fontSize);
}

bool OH_Drawing_TypographyTextGetLineStyle(OH_Drawing_TypographyStyle* style)
{
    if (style == nullptr || ConvertToOriginalText<TypographyStyle>(style) == nullptr) {
        return false;
    }
    return ConvertToOriginalText<TypographyStyle>(style)->useLineStyle;
}

OH_Drawing_FontWeight OH_Drawing_TypographyTextlineStyleGetFontWeight(OH_Drawing_TypographyStyle* style)
{
    if (style == nullptr || ConvertToOriginalText<TypographyStyle>(style) == nullptr) {
        return FONT_WEIGHT_400;
    }
    return static_cast<OH_Drawing_FontWeight>(ConvertToOriginalText<TypographyStyle>(style)->lineStyleFontWeight);
}

OH_Drawing_FontStyle OH_Drawing_TypographyTextlineStyleGetFontStyle(OH_Drawing_TypographyStyle* style)
{
    if (style == nullptr || ConvertToOriginalText<TypographyStyle>(style) == nullptr) {
        return FONT_STYLE_NORMAL;
    }
    return static_cast<OH_Drawing_FontStyle>(ConvertToOriginalText<TypographyStyle>(style)->lineStyleFontStyle);
}

char** OH_Drawing_TypographyTextlineStyleGetFontFamilies(OH_Drawing_TypographyStyle* style, size_t* num)
{
    if (style == nullptr || num == nullptr || ConvertToOriginalText<TypographyStyle>(style) == nullptr) {
        return nullptr;
    }
    char** fontFamilie = nullptr;
    const std::vector<std::string> &systemFontFamilies =
        ConvertToOriginalText<TypographyStyle>(style)-> lineStyleFontFamilies;
    if (systemFontFamilies.empty()) {
        num = 0;
        return nullptr;
    }
    fontFamilie = new char* [systemFontFamilies.size()];
    for (size_t i = 0; i < systemFontFamilies.size(); ++i) {
        fontFamilie[i] = new char[systemFontFamilies[i].size() + 1];
        auto retMemset = memset_s(fontFamilie[i], systemFontFamilies[i].size() + 1,
            '\0', systemFontFamilies[i].size() + 1);
        auto retCopy = strcpy_s(fontFamilie[i], systemFontFamilies[i].size() + 1,
            systemFontFamilies[i].c_str());
        if (retMemset != 0 || retCopy != 0) {
            for (size_t j = 0; j <= i; j++) {
                delete[] fontFamilie[j];
                fontFamilie[j] = nullptr;
            }
            delete[] fontFamilie;
            fontFamilie = nullptr;
            return nullptr;
        }
    }
    *num = systemFontFamilies.size();
    return fontFamilie;
}

void OH_Drawing_DestroyFontFamilies(char** fontFamilies, size_t num)
{
    if (fontFamilies == nullptr) {
        return;
    }
    for (size_t i = 0; i < num; ++i) {
        if (fontFamilies[i] == nullptr) {
            continue;
        }
        delete[] fontFamilies[i];
        fontFamilies[i] = nullptr;
    }
    delete[] fontFamilies;
    fontFamilies = nullptr;
}

double OH_Drawing_TypographyTextlineStyleGetFontSize(OH_Drawing_TypographyStyle* style)
{
    if (style == nullptr || ConvertToOriginalText<TypographyStyle>(style) == nullptr) {
        return 0;
    }
    return ConvertToOriginalText<TypographyStyle>(style)->lineStyleFontSize;
}

double OH_Drawing_TypographyTextlineStyleGetHeightScale(OH_Drawing_TypographyStyle* style)
{
    if (style == nullptr || ConvertToOriginalText<TypographyStyle>(style) == nullptr) {
        return 0;
    }
    return ConvertToOriginalText<TypographyStyle>(style)->lineStyleHeightScale;
}

bool OH_Drawing_TypographyTextlineStyleGetHeightOnly(OH_Drawing_TypographyStyle* style)
{
    if (style == nullptr || ConvertToOriginalText<TypographyStyle>(style) == nullptr) {
        return false;
    }
    return ConvertToOriginalText<TypographyStyle>(style)->lineStyleHeightOnly;
}

bool OH_Drawing_TypographyTextlineStyleGetHalfLeading(OH_Drawing_TypographyStyle* style)
{
    if (style == nullptr || ConvertToOriginalText<TypographyStyle>(style) == nullptr) {
        return false;
    }
    return ConvertToOriginalText<TypographyStyle>(style)->lineStyleHalfLeading;
}

double OH_Drawing_TypographyTextlineStyleGetSpacingScale(OH_Drawing_TypographyStyle* style)
{
    if (style == nullptr || ConvertToOriginalText<TypographyStyle>(style) == nullptr) {
        return 0;
    }
    return ConvertToOriginalText<TypographyStyle>(style)->lineStyleSpacingScale;
}

bool OH_Drawing_TypographyTextlineGetStyleOnly(OH_Drawing_TypographyStyle* style)
{
    if (style == nullptr || ConvertToOriginalText<TypographyStyle>(style) == nullptr) {
        return false;
    }
    return ConvertToOriginalText<TypographyStyle>(style)->lineStyleOnly;
}

OH_Drawing_TextAlign OH_Drawing_TypographyGetTextAlign(OH_Drawing_TypographyStyle* style)
{
    if (style == nullptr || ConvertToOriginalText<TypographyStyle>(style) == nullptr) {
        return TEXT_ALIGN_LEFT;
    }
    return static_cast<OH_Drawing_TextAlign>(ConvertToOriginalText<TypographyStyle>(style)->textAlign);
}

OH_Drawing_TextDirection OH_Drawing_TypographyGetTextDirection(OH_Drawing_TypographyStyle* style)
{
    if (style == nullptr || ConvertToOriginalText<TypographyStyle>(style) == nullptr) {
        return TEXT_DIRECTION_LTR;
    }
    return static_cast<OH_Drawing_TextDirection>(ConvertToOriginalText<TypographyStyle>(style)->textDirection);
}

size_t OH_Drawing_TypographyGetTextMaxLines(OH_Drawing_TypographyStyle* style)
{
    if (style == nullptr || ConvertToOriginalText<TypographyStyle>(style) == nullptr) {
        return 0;
    }
    return ConvertToOriginalText<TypographyStyle>(style)->maxLines;
}

char* OH_Drawing_TypographyGetTextEllipsis(OH_Drawing_TypographyStyle* style)
{
    if (style == nullptr || ConvertToOriginalText<TypographyStyle>(style) == nullptr) {
        return nullptr;
    }
    std::u16string ellipsis = ConvertToOriginalText<TypographyStyle>(style)->ellipsis;
    const char16_t* buffer = ellipsis.c_str();
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    std::string str = convert.to_bytes(buffer);
    char* result = new char[str.size()+1];
    strcpy(result, str.c_str());
    return result;
}

void OH_Drawing_DestroyEllipsis(char* ellipsis)
{
    if (ellipsis == nullptr) {
        return;
    }
    delete[] ellipsis;
    ellipsis = nullptr;
}

bool OH_Drawing_TypographyStyleEquals(OH_Drawing_TypographyStyle* from, OH_Drawing_TypographyStyle* to)
{
    if(from  == to ) {
        return true;
    }
    if (from == nullptr || to == nullptr) {
        return false;
    }
    return *ConvertToOriginalText<TypographyStyle>(from) == *ConvertToOriginalText<TypographyStyle>(to);
}

static OH_Drawing_FontAliasInfo* InitDrawingAliasInfoSet(const size_t aliasInfoSize,
    OH_Drawing_FontConfigInfoErrorCode& code)
{
    if (!aliasInfoSize) {
        code = OK;
        return nullptr;
    }
    if (aliasInfoSize >= std::numeric_limits<int16_t>::max()) {
        code = ALLOC_MEMORY_ERROR;
        return nullptr;
    }
    OH_Drawing_FontAliasInfo* aliasInfoArray = new OH_Drawing_FontAliasInfo[aliasInfoSize];
    if (aliasInfoArray == nullptr) {
        code = ALLOC_MEMORY_ERROR;
        return nullptr;
    }

    for (size_t i = 0; i < aliasInfoSize;) {
        aliasInfoArray[i].familyName = nullptr;
        aliasInfoArray[i].weight = 0;
        i++;
    }
    code = OK;
    return aliasInfoArray;
}

static void ResetString(char** ptr)
{
    if (!ptr || !(*ptr)) {
        return;
    }
    delete[] (*ptr);
    (*ptr) = nullptr;
}

static void ResetDrawingAliasInfoSet(OH_Drawing_FontAliasInfo** aliasInfoArray, size_t& aliasInfoSize)
{
    if (aliasInfoArray == nullptr || *aliasInfoArray == nullptr) {
        return;
    }

    for (size_t i = 0; i < aliasInfoSize; i++) {
        ResetString(&((*aliasInfoArray)[i].familyName));
    }

    delete[] (*aliasInfoArray);
    (*aliasInfoArray) = nullptr;
    aliasInfoSize = 0;
}

static OH_Drawing_FontAdjustInfo* InitDrawingAdjustInfoSet(const size_t adjustInfoSize,
    OH_Drawing_FontConfigInfoErrorCode& code)
{
    if (!adjustInfoSize) {
        code = OK;
        return nullptr;
    }
    if (adjustInfoSize >= std::numeric_limits<int16_t>::max()) {
        code = ALLOC_MEMORY_ERROR;
        return nullptr;
    }
    OH_Drawing_FontAdjustInfo* adjustInfoArray = new OH_Drawing_FontAdjustInfo[adjustInfoSize];
    if (adjustInfoArray == nullptr) {
        code = ALLOC_MEMORY_ERROR;
        return nullptr;
    }

    for (size_t i = 0; i < adjustInfoSize;) {
        adjustInfoArray[i].weight = 0;
        adjustInfoArray[i].to = 0;
        i++;
    }
    code = OK;
    return adjustInfoArray;
}

static void ResetDrawingAdjustInfo(OH_Drawing_FontAdjustInfo** adjustInfoArray, size_t& adjustInfoSize)
{
    if (adjustInfoArray == nullptr || *adjustInfoArray == nullptr) {
        return;
    }
    delete[] (*adjustInfoArray);
    (*adjustInfoArray) = nullptr;
    adjustInfoSize = 0;
}

static OH_Drawing_FontGenericInfo* InitDrawingFontGenericInfoSet(const size_t fontGenericInfoSize,
    OH_Drawing_FontConfigInfoErrorCode& code)
{
    if (!fontGenericInfoSize) {
        code = PARSE_FILE_ERROR;
        return nullptr;
    }
    if (fontGenericInfoSize >= std::numeric_limits<int16_t>::max()) {
        code = ALLOC_MEMORY_ERROR;
        return nullptr;
    }

    OH_Drawing_FontGenericInfo* fontGenericInfoArray = new OH_Drawing_FontGenericInfo[fontGenericInfoSize];
    if (fontGenericInfoArray == nullptr) {
        code = ALLOC_MEMORY_ERROR;
        return nullptr;
    }

    for (size_t index = 0; index < fontGenericInfoSize;) {
        fontGenericInfoArray[index].familyName = nullptr;
        fontGenericInfoArray[index].aliasInfoSize = 0;
        fontGenericInfoArray[index].adjustInfoSize = 0;
        fontGenericInfoArray[index].aliasInfoSet = nullptr;
        fontGenericInfoArray[index].adjustInfoSet = nullptr;
        index++;
    }
    code = OK;
    return fontGenericInfoArray;
}

static void ResetDrawingFontGenericInfo(OH_Drawing_FontGenericInfo& fontGenericInfo)
{
    ResetString(&fontGenericInfo.familyName);
    ResetDrawingAliasInfoSet(&fontGenericInfo.aliasInfoSet, fontGenericInfo.aliasInfoSize);
    ResetDrawingAdjustInfo(&fontGenericInfo.adjustInfoSet, fontGenericInfo.adjustInfoSize);
}

static void ResetDrawingFontGenericInfoSet(OH_Drawing_FontGenericInfo** fontGenericInfoArray,
    size_t& fontGenericInfoSize)
{
    if (fontGenericInfoArray == nullptr || *fontGenericInfoArray == nullptr) {
        return;
    }

    for (size_t i = 0; i < fontGenericInfoSize; i++) {
        ResetDrawingFontGenericInfo((*fontGenericInfoArray)[i]);
    }

    delete[] (*fontGenericInfoArray);
    (*fontGenericInfoArray) = nullptr;
    fontGenericInfoSize = 0;
}

static OH_Drawing_FontFallbackInfo* InitDrawingDrawingFallbackInfoSet(const size_t fallbackInfoSize,
    OH_Drawing_FontConfigInfoErrorCode& code)
{
    if (!fallbackInfoSize) {
        code = OK;
        return nullptr;
    }
    if (fallbackInfoSize >= std::numeric_limits<int16_t>::max()) {
        code = ALLOC_MEMORY_ERROR;
        return nullptr;
    }
    OH_Drawing_FontFallbackInfo* fallbackInfoArray = new OH_Drawing_FontFallbackInfo[fallbackInfoSize];
    if (fallbackInfoArray == nullptr) {
        code = ALLOC_MEMORY_ERROR;
        return nullptr;
    }

    for (size_t i = 0; i < fallbackInfoSize;) {
        fallbackInfoArray[i].language = nullptr;
        fallbackInfoArray[i].familyName = nullptr;
        i++;
    }
    code = OK;
    return fallbackInfoArray;
}

static void ResetDrawingFallbackInfo(OH_Drawing_FontFallbackInfo& fallbackInfo)
{
    ResetString(&fallbackInfo.language);
    ResetString(&fallbackInfo.familyName);
}

static void ResetDrawingFallbackInfoSet(OH_Drawing_FontFallbackInfo** fallbackInfoArray, size_t& fallbackInfoSize)
{
    if (fallbackInfoArray == nullptr || *fallbackInfoArray == nullptr) {
        return;
    }

    for (size_t i = 0; i < fallbackInfoSize; i++) {
        ResetDrawingFallbackInfo((*fallbackInfoArray)[i]);
    }
    delete[] (*fallbackInfoArray);
    (*fallbackInfoArray) = nullptr;
    fallbackInfoSize = 0;
}

static OH_Drawing_FontFallbackGroup* InitDrawingFallbackGroupSet(const size_t fallbackGroupSize,
    OH_Drawing_FontConfigInfoErrorCode& code)
{
    if (!fallbackGroupSize) {
        code = OK;
        return nullptr;
    }
    if (fallbackGroupSize >= std::numeric_limits<int16_t>::max()) {
        code = ALLOC_MEMORY_ERROR;
        return nullptr;
    }
    OH_Drawing_FontFallbackGroup* fallbackGroupArray = new OH_Drawing_FontFallbackGroup[fallbackGroupSize];
    if (fallbackGroupArray == nullptr) {
        code = ALLOC_MEMORY_ERROR;
        return nullptr;
    }

    for (size_t i = 0; i < fallbackGroupSize;) {
        fallbackGroupArray[i].groupName = nullptr;
        fallbackGroupArray[i].fallbackInfoSize = 0;
        fallbackGroupArray[i].fallbackInfoSet = nullptr;
        i++;
    }
    code = OK;
    return fallbackGroupArray;
}

static void ResetDrawingFallbackGroup(OH_Drawing_FontFallbackGroup& fallbackGroup)
{
    ResetString(&fallbackGroup.groupName);
    ResetDrawingFallbackInfoSet(&fallbackGroup.fallbackInfoSet, fallbackGroup.fallbackInfoSize);
}

static void ResetDrawingFallbackGroupSet(OH_Drawing_FontFallbackGroup** fallbackGroupArray,
    size_t& fallbackGroupSize)
{
    if (fallbackGroupArray == nullptr || *fallbackGroupArray == nullptr) {
        return;
    }

    for (size_t i = 0; i < fallbackGroupSize; i++) {
        ResetDrawingFallbackGroup((*fallbackGroupArray)[i]);
    }
    delete[] (*fallbackGroupArray);
    (*fallbackGroupArray) = nullptr;
    fallbackGroupSize = 0;
}

static OH_Drawing_FontConfigInfo* InitDrawingFontConfigJsonInfo()
{
    OH_Drawing_FontConfigInfo* drawFontCfgInfo = new OH_Drawing_FontConfigInfo;
    if (drawFontCfgInfo == nullptr) {
        return nullptr;
    }

    drawFontCfgInfo->fontDirSize = 0;
    drawFontCfgInfo->fontGenericInfoSize = 0;
    drawFontCfgInfo->fallbackGroupSize = 0;
    drawFontCfgInfo->fontDirSet = nullptr;
    drawFontCfgInfo->fontGenericInfoSet = nullptr;
    drawFontCfgInfo->fallbackGroupSet = nullptr;

    return drawFontCfgInfo;
}

static char** InitStringArray(const size_t charArraySize)
{
    if (!charArraySize || charArraySize >= std::numeric_limits<int16_t>::max()) {
        return nullptr;
    }

    char** ptr = new char* [charArraySize];
    if (!ptr) {
        return nullptr;
    }

    for (size_t i = 0; i < charArraySize; i++) {
        ptr[i] = nullptr;
    }
    return ptr;
}

static void ResetStringArray(char*** ptr, size_t& charArraySize)
{
    if (ptr == nullptr || *ptr == nullptr) {
        return;
    }
    for (size_t i = 0; i < charArraySize; i++) {
        if (!((*ptr)[i])) {
            continue;
        }
        delete[] ((*ptr)[i]);
        ((*ptr)[i]) = nullptr;
    }
    delete[] (*ptr);
    (*ptr) = nullptr;
    charArraySize = 0;
}

static void ResetDrawingFontConfigJsonInfo(OH_Drawing_FontConfigInfo** drawFontCfgInfo)
{
    if (drawFontCfgInfo == nullptr || *drawFontCfgInfo == nullptr) {
        return;
    }
    delete (*drawFontCfgInfo);
    (*drawFontCfgInfo) = nullptr;
}

static bool CopyDrawingFontDirSet(char*** drawFontDirSet, size_t& fontDirSize,
    const std::vector<std::string>& fontDirSet, OH_Drawing_FontConfigInfoErrorCode& code)
{
    if (!drawFontDirSet) {
        code = ALLOC_MEMORY_ERROR;
        return false;
    }
    if (fontDirSet.empty()) {
        code = PARSE_FILE_ERROR;
        return false;
    }

    size_t size = fontDirSet.size();
    (*drawFontDirSet) = InitStringArray(size);
    if (!(*drawFontDirSet)) {
        code = ALLOC_MEMORY_ERROR;
        return false;
    }

    for (size_t i = 0; i < size; ++i) {
        bool result = CopyStrData(&((*drawFontDirSet)[i]), fontDirSet.at(i), &code);
        if (result) {
            fontDirSize++;
        } else {
            break;
        }
    }
    if (fontDirSize != size) {
        ResetStringArray(drawFontDirSet, fontDirSize);
        return false;
    }
    return true;
}

static bool CopyDrawingAliasInfo(OH_Drawing_FontAliasInfo& drawAliasInfo, const TextEngine::AliasInfo& aliasInfo,
    OH_Drawing_FontConfigInfoErrorCode& code)
{
    bool result = CopyStrData(&drawAliasInfo.familyName, aliasInfo.familyName, &code);
    if (!result) {
        return false;
    }
    drawAliasInfo.weight = aliasInfo.weight;
    code = OK;
    return true;
}

static bool CopyDrawingAliasInfoSet(OH_Drawing_FontAliasInfo** drawAliasInfoSet, size_t& aliasInfoSize,
    const std::vector<TextEngine::AliasInfo>& aliasSet, OH_Drawing_FontConfigInfoErrorCode& code)
{
    if (!drawAliasInfoSet) {
        code = ALLOC_MEMORY_ERROR;
        return false;
    }

    if (!aliasSet.empty()) {
        (*drawAliasInfoSet) = InitDrawingAliasInfoSet(aliasSet.size(), code);
        if (!(*drawAliasInfoSet)) {
            return false;
        }
        size_t aliasInfoCount = 0;
        for (; aliasInfoCount < aliasSet.size();) {
            bool result = CopyDrawingAliasInfo((*drawAliasInfoSet)[aliasInfoCount], aliasSet.at(aliasInfoCount), code);
            if (result) {
                ++aliasInfoCount;
            } else {
                break;
            }
        }
        aliasInfoSize = aliasInfoCount;
        if (aliasInfoSize != aliasSet.size()) {
            return false;
        }
    }
    return true;
}

static void CopyDrawingAdjustInfo(OH_Drawing_FontAdjustInfo& drawAdjustInfo, const TextEngine::AdjustInfo& adjustInfo)
{
    drawAdjustInfo.weight = adjustInfo.origValue;
    drawAdjustInfo.to = adjustInfo.newValue;
}

static bool CopyDrawingAdjustSet(OH_Drawing_FontAdjustInfo** drawAdjustInfoSet, size_t& adjustInfoSize,
    const std::vector<TextEngine::AdjustInfo>& adjustSet, OH_Drawing_FontConfigInfoErrorCode& code)
{
    if (!drawAdjustInfoSet) {
        code = ALLOC_MEMORY_ERROR;
        return false;
    }

    if (!adjustSet.empty()) {
        (*drawAdjustInfoSet) = InitDrawingAdjustInfoSet(adjustSet.size(), code);
        if (!(*drawAdjustInfoSet)) {
            return false;
        }
        size_t adjustInfoCount = 0;
        for (; adjustInfoCount < adjustSet.size();) {
            CopyDrawingAdjustInfo((*drawAdjustInfoSet)[adjustInfoCount], adjustSet.at(adjustInfoCount));
            ++adjustInfoCount;
        }
        adjustInfoSize = adjustInfoCount;
        if (adjustInfoSize != adjustSet.size()) {
            return false;
        }
    }
    code = OK;
    return true;
}

static bool CopyDrawingFontGenericInfo(OH_Drawing_FontGenericInfo& drawFontGenericInfo,
    const TextEngine::FontGenericInfo& genericInfo, OH_Drawing_FontConfigInfoErrorCode& code)
{
    bool result = CopyStrData(&drawFontGenericInfo.familyName, genericInfo.familyName, &code);
    if (!result) {
        return false;
    }

    result = CopyDrawingAliasInfoSet(&drawFontGenericInfo.aliasInfoSet,
        drawFontGenericInfo.aliasInfoSize, genericInfo.aliasSet, code);
    if (!result) {
        return false;
    }

    result = CopyDrawingAdjustSet(&drawFontGenericInfo.adjustInfoSet,
        drawFontGenericInfo.adjustInfoSize, genericInfo.adjustSet, code);
    if (!result) {
        return false;
    }
    return true;
}

static bool CopyDrawingFontGenericInfoSetInner(OH_Drawing_FontGenericInfo** fontGenericInfoSet,
    size_t& fontGenericInfoSize, const std::vector<TextEngine::FontGenericInfo>& genericSet,
    OH_Drawing_FontConfigInfoErrorCode& code)
{
    if (!fontGenericInfoSet || !(*fontGenericInfoSet)) {
        code = ALLOC_MEMORY_ERROR;
        return false;
    }
    size_t genericInfoNum = 0;
    for (; genericInfoNum < genericSet.size();) {
        auto& fontGenericInfo = (*fontGenericInfoSet)[genericInfoNum];
        bool result = CopyDrawingFontGenericInfo(fontGenericInfo, genericSet.at(genericInfoNum), code);
        if (!result) {
            ResetDrawingFontGenericInfo(fontGenericInfo);
            break;
        } else {
            ++genericInfoNum;
        }
    }
    fontGenericInfoSize = genericInfoNum;
    if (fontGenericInfoSize != genericSet.size()) {
        ResetDrawingFontGenericInfoSet(fontGenericInfoSet, fontGenericInfoSize);
        return false;
    }
    code = OK;
    return true;
}

static bool CopyDrawingFallbackInfo(OH_Drawing_FontFallbackInfo& drawFallbackInfo,
    const TextEngine::FallbackInfo& fallbackInfo, OH_Drawing_FontConfigInfoErrorCode& code)
{
    if (!CopyStrData(&drawFallbackInfo.language, fallbackInfo.font, &code)) {
        return false;
    }
    if (!CopyStrData(&drawFallbackInfo.familyName, fallbackInfo.familyName, &code)) {
        return false;
    }
    return true;
}

static bool CopyDrawingFallbackGroup(OH_Drawing_FontFallbackGroup& drawFallbackGroup,
    const TextEngine::FallbackGroup& fallbackGroup, OH_Drawing_FontConfigInfoErrorCode& code)
{
    if (!fallbackGroup.groupName.empty()) {
        if (!CopyStrData(&drawFallbackGroup.groupName, fallbackGroup.groupName, &code)) {
            return false;
        }
    }
    auto& fallbackInfoSet = fallbackGroup.fallbackInfoSet;
    if (fallbackInfoSet.empty()) {
        code = OK;
        return true;
    }
    drawFallbackGroup.fallbackInfoSet = InitDrawingDrawingFallbackInfoSet(fallbackInfoSet.size(), code);
    if (!drawFallbackGroup.fallbackInfoSet) {
        return false;
    }
    size_t fallbackInfoCount = 0;
    for (; fallbackInfoCount < fallbackInfoSet.size();) {
        auto& fallbackInfo = drawFallbackGroup.fallbackInfoSet[fallbackInfoCount];
        bool res = CopyDrawingFallbackInfo(fallbackInfo, fallbackInfoSet[fallbackInfoCount], code);
        if (res) {
            ++fallbackInfoCount;
        } else {
            ResetDrawingFallbackInfo(fallbackInfo);
            break;
        }
    }
    drawFallbackGroup.fallbackInfoSize = fallbackInfoCount;
    if (drawFallbackGroup.fallbackInfoSize != fallbackInfoSet.size()) {
        return false;
    }
    return true;
}

static bool CopyDrawingFallbackGroupSetInner(OH_Drawing_FontFallbackGroup** drawFallbackGroupSet,
    size_t& fallbackGroupSize, const std::vector<TextEngine::FallbackGroup>& fallbackGroupSet,
    OH_Drawing_FontConfigInfoErrorCode& code)
{
    if (!drawFallbackGroupSet) {
        code = ALLOC_MEMORY_ERROR;
        return false;
    }
    (*drawFallbackGroupSet) = InitDrawingFallbackGroupSet(fallbackGroupSet.size(), code);
    if (!(*drawFallbackGroupSet)) {
        return false;
    }
    size_t fallbackGroupNum = 0;
    for (; fallbackGroupNum < fallbackGroupSet.size();) {
        auto& fallbackGroup = (*drawFallbackGroupSet)[fallbackGroupNum];
        bool res = CopyDrawingFallbackGroup(fallbackGroup, fallbackGroupSet.at(fallbackGroupNum), code);
        if (res) {
            fallbackGroupNum++;
        } else {
            ResetDrawingFallbackGroup(fallbackGroup);
            break;
        }
    }
    fallbackGroupSize = fallbackGroupNum;
    if (fallbackGroupSize != fallbackGroupSet.size()) {
        ResetDrawingFallbackGroupSet(drawFallbackGroupSet, fallbackGroupSize);
        return false;
    }
    return true;
}

static bool CopyDrawingFontGenericInfoSet(OH_Drawing_FontConfigInfo** drawFontCfgInfo,
    const std::vector<TextEngine::FontGenericInfo>& genericSet, OH_Drawing_FontConfigInfoErrorCode& code)
{
    if (!drawFontCfgInfo || !(*drawFontCfgInfo)) {
        code = ALLOC_MEMORY_ERROR;
        return false;
    }

    size_t size = genericSet.size();
    (*drawFontCfgInfo)->fontGenericInfoSet = InitDrawingFontGenericInfoSet(size, code);
    if (!(*drawFontCfgInfo)->fontGenericInfoSet) {
        ResetStringArray(&((*drawFontCfgInfo)->fontDirSet), (*drawFontCfgInfo)->fontDirSize);
        ResetDrawingFontConfigJsonInfo(drawFontCfgInfo);
        return false;
    }

    bool result = CopyDrawingFontGenericInfoSetInner(&((*drawFontCfgInfo)->fontGenericInfoSet),
        (*drawFontCfgInfo)->fontGenericInfoSize, genericSet, code);
    if (!result) {
        ResetStringArray(&((*drawFontCfgInfo)->fontDirSet), (*drawFontCfgInfo)->fontDirSize);
        ResetDrawingFontConfigJsonInfo(drawFontCfgInfo);
        return false;
    }
    return true;
}

static bool CopyDrawingFallbackGroupSet(OH_Drawing_FontConfigInfo** drawFontCfgInfo,
    const std::vector<TextEngine::FallbackGroup>& fallbackGroupSet, OH_Drawing_FontConfigInfoErrorCode& code)
{
    if (!drawFontCfgInfo || !(*drawFontCfgInfo)) {
        code = ALLOC_MEMORY_ERROR;
        return false;
    }

    if (fallbackGroupSet.empty()) {
        code = OK;
        return true;
    }
    bool result = CopyDrawingFallbackGroupSetInner(&((*drawFontCfgInfo)->fallbackGroupSet),
        (*drawFontCfgInfo)->fallbackGroupSize, fallbackGroupSet, code);
    if (!result) {
        ResetDrawingFontGenericInfoSet(&((*drawFontCfgInfo)->fontGenericInfoSet),
            (*drawFontCfgInfo)->fontGenericInfoSize);
        ResetStringArray(&((*drawFontCfgInfo)->fontDirSet), (*drawFontCfgInfo)->fontDirSize);
        ResetDrawingFontConfigJsonInfo(drawFontCfgInfo);
        return false;
    }
    return true;
}

OH_Drawing_FontConfigInfo* OH_Drawing_GetSystemFontConfigInfo(OH_Drawing_FontConfigInfoErrorCode* errorCode)
{
    OH_Drawing_FontConfigInfoErrorCode code = UNKNOWN_ERROR;
    TextEngine::FontConfigJson fontConfigJson;
    int res = fontConfigJson.ParseFile();
    if (res) {
        code = PARSE_FILE_ERROR;
        SetFontConfigInfoErrorCode(code, errorCode);
        return nullptr;
    }
    auto fontCfgJsonInfo = fontConfigJson.GetFontConfigJsonInfo();
    if (!fontCfgJsonInfo) {
        SetFontConfigInfoErrorCode(OH_Drawing_FontConfigInfoErrorCode::PARSE_FILE_ERROR, errorCode);
        return nullptr;
    }

    OH_Drawing_FontConfigInfo* drawFontCfgInfo = InitDrawingFontConfigJsonInfo();
    if (!drawFontCfgInfo) {
        SetFontConfigInfoErrorCode(OH_Drawing_FontConfigInfoErrorCode::ALLOC_MEMORY_ERROR, errorCode);
        return nullptr;
    }

    bool result = CopyDrawingFontDirSet(&drawFontCfgInfo->fontDirSet,
        drawFontCfgInfo->fontDirSize, fontCfgJsonInfo->fontDirSet, code);
    if (!result) {
        ResetDrawingFontConfigJsonInfo(&drawFontCfgInfo);
        SetFontConfigInfoErrorCode(code, errorCode);
        return drawFontCfgInfo;
    }

    result = CopyDrawingFontGenericInfoSet(&drawFontCfgInfo, fontCfgJsonInfo->genericSet, code);
    if (!result) {
        SetFontConfigInfoErrorCode(code, errorCode);
        return drawFontCfgInfo;
    }

    CopyDrawingFallbackGroupSet(&drawFontCfgInfo, fontCfgJsonInfo->fallbackGroupSet, code);
    SetFontConfigInfoErrorCode(code, errorCode);
    return drawFontCfgInfo;
}

void OH_Drawing_DestroySystemFontConfigInfo(OH_Drawing_FontConfigInfo* drawFontCfgInfo)
{
    if (drawFontCfgInfo == nullptr) {
        return;
    }

    ResetDrawingFallbackGroupSet(&drawFontCfgInfo->fallbackGroupSet,
        drawFontCfgInfo->fallbackGroupSize);
    ResetDrawingFontGenericInfoSet(&drawFontCfgInfo->fontGenericInfoSet,
        drawFontCfgInfo->fontGenericInfoSize);
    ResetStringArray(&drawFontCfgInfo->fontDirSet, drawFontCfgInfo->fontDirSize);
    delete drawFontCfgInfo;
}

void OH_Drawing_SetTypographyStyleTextStrutStyle(OH_Drawing_TypographyStyle* style, OH_Drawing_StrutStyle* strutstyle)
{
    if (style == nullptr || strutstyle == nullptr) {
        return;
    }
    OH_Drawing_SetTypographyTextLineStyleFontWeight(style, strutstyle->weight);
    OH_Drawing_SetTypographyTextLineStyleFontStyle(style, strutstyle->style);
    OH_Drawing_SetTypographyTextLineStyleFontFamilies(style, strutstyle->familiesSize,
        const_cast<const char**>(strutstyle->families));
    OH_Drawing_SetTypographyTextLineStyleFontSize(style, strutstyle->size);
    OH_Drawing_SetTypographyTextLineStyleFontHeight(style, strutstyle->heightScale);
    OH_Drawing_SetTypographyTextLineStyleHalfLeading(style, strutstyle->halfLeading);
    OH_Drawing_SetTypographyTextLineStyleSpacingScale(style, strutstyle->leading);
    OH_Drawing_SetTypographyTextLineStyleOnly(style, strutstyle->forceStrutHeight);
}

void OH_Drawing_TypographyStyleDestroyStrutStyle(OH_Drawing_StrutStyle* strutstyle)
{
    if (strutstyle == nullptr) {
        return;
    }
    if (strutstyle->familiesSize == 0 || strutstyle->families == nullptr) {
        return;
    }
    for (size_t i = 0; i < strutstyle->familiesSize; i++) {
        if (strutstyle->families[i] != nullptr) {
            delete[] strutstyle->families[i];
        }
    }
    delete[] strutstyle->families;
    delete strutstyle;
    strutstyle = nullptr;
}

OH_Drawing_StrutStyle* OH_Drawing_TypographyStyleGetStrutStyle(OH_Drawing_TypographyStyle* style)
{
    TypographyStyle* typographyStyle = ConvertToOriginalText<TypographyStyle>(style);
    if (typographyStyle == nullptr) {
        return nullptr;
    }
    OH_Drawing_StrutStyle* strutstyle = new OH_Drawing_StrutStyle();
    if (strutstyle == nullptr) {
        return nullptr;
    }
    strutstyle->weight = (OH_Drawing_FontWeight)(typographyStyle->lineStyleFontWeight);
    strutstyle->style = (OH_Drawing_FontStyle)(typographyStyle->lineStyleFontStyle);
    strutstyle->size = typographyStyle->lineStyleFontSize;
    strutstyle->heightScale = typographyStyle->lineStyleHeightScale;
    strutstyle->heightOverride = typographyStyle->lineStyleHeightOnly;
    strutstyle->halfLeading = typographyStyle->lineStyleHalfLeading;
    strutstyle->leading = typographyStyle->lineStyleSpacingScale;
    strutstyle->forceStrutHeight = typographyStyle->lineStyleOnly;
    strutstyle->familiesSize = typographyStyle->lineStyleFontFamilies.size();
    if (strutstyle->familiesSize == 0) {
        strutstyle->families = nullptr;
        return strutstyle;
    }
    strutstyle->families = new char* [strutstyle->familiesSize];
    if (strutstyle->families == nullptr) {
        delete strutstyle;
		return nullptr;
    }
    for (size_t i = 0; i < strutstyle->familiesSize; i++) {
        int size = typographyStyle->lineStyleFontFamilies[i].size() + 1;
        strutstyle->families[i] = new char[size];
        if (!strutstyle->families[i]) {
            for (size_t j = 0; j < i ; j++) {
                delete[] strutstyle->families[j];
            }
            delete[] strutstyle->families;
            delete strutstyle;
		    return nullptr;
        }
	    if (strcpy_s(strutstyle->families[i], size, typographyStyle->lineStyleFontFamilies[i].c_str()) != 0) {
		    for (size_t j = 0; j <= i ; j++) {
			    delete[] strutstyle->families[j];
		    }
            delete[] strutstyle->families;
            delete strutstyle;
		    return nullptr;
	    }
    }
    return strutstyle;
}

bool OH_Drawing_TypographyStyleStrutStyleEquals(OH_Drawing_StrutStyle* from, OH_Drawing_StrutStyle* to)
{
    if (from == nullptr || to == nullptr) {
        return false;
    }
    if (from->weight == to->weight &&
        from->style == to->style &&
        from->size == to->size &&
        from->heightScale == to->heightScale &&
        from->heightOverride == to->heightOverride &&
        from->halfLeading == to->halfLeading &&
        from->leading == to->leading &&
        from->forceStrutHeight == to->forceStrutHeight &&
        from->familiesSize == to->familiesSize) {
        for (size_t i = 0; i < from->familiesSize; i++) {
            if (strcmp(from->families[i], to->families[i]) != 0) {
                return false;
            }
        }
        return true;
    }
    return false;
}

void OH_Drawing_TypographyStyleSetHintingEnable(OH_Drawing_TypographyStyle* style, bool hintingEnable)
{
    TypographyStyle* typographyStyle = ConvertToOriginalText<TypographyStyle>(style);
    if (typographyStyle == nullptr) {
        return;
    }
    typographyStyle->hintingIsOn = hintingEnable;
}

OH_Drawing_Font_Metrics* OH_Drawing_TypographyGetLineFontMetrics(OH_Drawing_Typography* typography,
    size_t lineNumber, size_t* fontMetricsSize)
{
    if (!typography || !fontMetricsSize || !lineNumber) {
        return nullptr;
    }

    auto txtSKTypograph = ConvertToOriginalText<Typography>(typography);
    std::vector<Drawing::FontMetrics> grabFontMetrics;
    if (!txtSKTypograph->GetLineFontMetrics(lineNumber, *fontMetricsSize, grabFontMetrics)) {
        return nullptr;
    }

    OH_Drawing_Font_Metrics* fontMetrics = new OH_Drawing_Font_Metrics[grabFontMetrics.size()];
    if (fontMetrics == nullptr || !grabFontMetrics.size()) {
        return nullptr;
    }

    for (size_t further = 0; further < grabFontMetrics.size(); further++) {
        ConvertFontMetrics(grabFontMetrics[further], fontMetrics[further]);
    }
    return fontMetrics;
}

void OH_Drawing_TypographyDestroyLineFontMetrics(OH_Drawing_Font_Metrics* lineFontMetric)
{
    if (!lineFontMetric) {
        return;
    }
    delete[] lineFontMetric;
    lineFontMetric = nullptr;
}

static FontStyle GetFontStyle(OH_Drawing_FontStyle style)
{
    FontStyle fontStyle;
    switch (style) {
        case FONT_STYLE_NORMAL: {
            fontStyle = FontStyle::NORMAL;
            break;
        }
        case FONT_STYLE_ITALIC: {
            fontStyle = FontStyle::ITALIC;
            break;
        }
        case FONT_STYLE_OBLIQUE: {
            fontStyle = FontStyle::OBLIQUE;
            break;
        }
        default: {
            fontStyle = FontStyle::NORMAL;
        }
    }
    return fontStyle;
}

static FontWeight GetFontWeight(OH_Drawing_FontWeight weight)
{
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
    return fontWeight;
}

static FontWidth GetFontWidth(OH_Drawing_FontWidth width)
{
    FontWidth fontWidth;
    switch (width) {
        case FONT_WIDTH_ULTRA_CONDENSED: {
            fontWidth = FontWidth::ULTRA_CONDENSED;
            break;
        }
        case FONT_WIDTH_EXTRA_CONDENSED: {
            fontWidth = FontWidth::EXTRA_CONDENSED;
            break;
        }
        case FONT_WIDTH_CONDENSED: {
            fontWidth = FontWidth::CONDENSED;
            break;
        }
        case FONT_WIDTH_SEMI_CONDENSED: {
            fontWidth = FontWidth::SEMI_CONDENSED;
            break;
        }
        case FONT_WIDTH_NORMAL: {
            fontWidth = FontWidth::NORMAL;
            break;
        }
        case FONT_WIDTH_SEMI_EXPANDED: {
            fontWidth = FontWidth::SEMI_EXPANDED;
            break;
        }
        case FONT_WIDTH_EXPANDED: {
            fontWidth = FontWidth::EXPANDED;
            break;
        }
        case FONT_WIDTH_EXTRA_EXPANDED: {
            fontWidth = FontWidth::EXTRA_EXPANDED;
            break;
        }
        case FONT_WIDTH_ULTRA_EXPANDED: {
            fontWidth = FontWidth::ULTRA_EXPANDED;
            break;
        }
        default: {
            fontWidth = FontWidth::NORMAL;
        }
    }
    return fontWidth;
}

void OH_Drawing_SetTextStyleFontStyleStruct(OH_Drawing_TextStyle* drawingTextStyle,
    OH_Drawing_FontStyleStruct fontStyle)
{
    if (drawingTextStyle == nullptr) {
        return;
    }
    TextStyle* style = ConvertToOriginalText<TextStyle>(drawingTextStyle);
    if (style == nullptr) {
        return;
    }
    style->fontWeight = GetFontWeight(fontStyle.weight);
    style->fontWidth = GetFontWidth(fontStyle.width);
    style->fontStyle = GetFontStyle(fontStyle.slant);
}

void OH_Drawing_TextStyleGetFontStyleStruct(OH_Drawing_TextStyle* drawingTextStyle,
    OH_Drawing_FontStyleStruct* fontStyle)
{
    if (drawingTextStyle == nullptr || fontStyle == nullptr) {
        return;
    }
    TextStyle* style = ConvertToOriginalText<TextStyle>(drawingTextStyle);
    if (style == nullptr) {
        return;
    }
    fontStyle->weight = static_cast<OH_Drawing_FontWeight>(style->fontWeight);
    fontStyle->width = static_cast<OH_Drawing_FontWidth>(style->fontWidth);
    fontStyle->slant = static_cast<OH_Drawing_FontStyle>(style->fontStyle);
}

void OH_Drawing_SetTypographyStyleFontStyleStruct(OH_Drawing_TypographyStyle* drawingStyle,
    OH_Drawing_FontStyleStruct fontStyle)
{
    if (drawingStyle == nullptr) {
        return;
    }
    TypographyStyle* style = ConvertToOriginalText<TypographyStyle>(drawingStyle);
    if (style == nullptr) {
        return;
    }
    style->fontWeight = GetFontWeight(fontStyle.weight);
    style->fontWidth = GetFontWidth(fontStyle.width);
    style->fontStyle = GetFontStyle(fontStyle.slant);
}

void OH_Drawing_TypographyStyleGetFontStyleStruct(OH_Drawing_TypographyStyle* drawingStyle,
    OH_Drawing_FontStyleStruct* fontStyle)
{
    if (drawingStyle == nullptr || fontStyle == nullptr) {
        return;
    }
    TypographyStyle* style = ConvertToOriginalText<TypographyStyle>(drawingStyle);
    if (style == nullptr) {
        return;
    }
    fontStyle->weight = static_cast<OH_Drawing_FontWeight>(style->fontWeight);
    fontStyle->width = static_cast<OH_Drawing_FontWidth>(style->fontWidth);
    fontStyle->slant = static_cast<OH_Drawing_FontStyle>(style->fontStyle);
}