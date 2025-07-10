/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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

#include "drawing_font.h"
#include "drawing_path.h"
#include "drawing_rect.h"

#ifdef USE_M133_SKIA
#include "src/base/SkUTF.h"
#else
#include "src/utils/SkUTF.h"
#endif

#include "drawing_canvas_utils.h"
#include "drawing_font_utils.h"
#include "drawing_path.h"
#include "text/font.h"
#include "utils/log.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

static Font* CastToFont(OH_Drawing_Font* cFont)
{
    return reinterpret_cast<Font*>(cFont);
}

static const Font* CastToFont(const OH_Drawing_Font* cFont)
{
    return reinterpret_cast<const Font*>(cFont);
}

static Drawing::DrawingFontFeatures* CastToFontFeatures(OH_Drawing_FontFeatures* fontFeatures)
{
    return reinterpret_cast<Drawing::DrawingFontFeatures*>(fontFeatures);
}

static const Drawing::DrawingFontFeatures* CastToFontFeatures(const OH_Drawing_FontFeatures* fontFeatures)
{
    return reinterpret_cast<const Drawing::DrawingFontFeatures*>(fontFeatures);
}

static Typeface* CastToTypeface(OH_Drawing_Typeface* cTypeface)
{
    return reinterpret_cast<Typeface*>(cTypeface);
}

void OH_Drawing_FontSetEdging(OH_Drawing_Font* cFont, OH_Drawing_FontEdging cEdging)
{
    Font* font = CastToFont(cFont);
    if (font == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    if (cEdging < FONT_EDGING_ALIAS || cEdging > FONT_EDGING_SUBPIXEL_ANTI_ALIAS) {
        g_drawingErrorCode = OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE;
        return;
    }
    font->SetEdging(static_cast<FontEdging>(cEdging));
}

OH_Drawing_FontEdging OH_Drawing_FontGetEdging(const OH_Drawing_Font* cFont)
{
    const Font* font = CastToFont(cFont);
    if (font == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return FONT_EDGING_ALIAS;
    }
    return static_cast<OH_Drawing_FontEdging>(font->GetEdging());
}

void OH_Drawing_FontSetHinting(OH_Drawing_Font* cFont, OH_Drawing_FontHinting cHinting)
{
    Font* font = CastToFont(cFont);
    if (font == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    if (cHinting < FONT_HINTING_NONE || cHinting > FONT_HINTING_FULL) {
        g_drawingErrorCode = OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE;
        return;
    }
    font->SetHinting(static_cast<FontHinting>(cHinting));
}

OH_Drawing_FontHinting OH_Drawing_FontGetHinting(const OH_Drawing_Font* cFont)
{
    const Font* font = CastToFont(cFont);
    if (font == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return FONT_HINTING_NONE;
    }
    return static_cast<OH_Drawing_FontHinting>(font->GetHinting());
}

void OH_Drawing_FontSetForceAutoHinting(OH_Drawing_Font* cFont, bool isForceAutoHinting)
{
    Font* font = CastToFont(cFont);
    if (font == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    font->SetForceAutoHinting(isForceAutoHinting);
}

bool OH_Drawing_FontIsForceAutoHinting(const OH_Drawing_Font* cFont)
{
    const Font* font = CastToFont(cFont);
    if (font == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return false;
    }
    return font->IsForceAutoHinting();
}

void OH_Drawing_FontSetBaselineSnap(OH_Drawing_Font* cFont, bool baselineSnap)
{
    Font* font = CastToFont(cFont);
    if (font == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    font->SetBaselineSnap(baselineSnap);
}

bool OH_Drawing_FontIsBaselineSnap(const OH_Drawing_Font* cFont)
{
    const Font* font = CastToFont(cFont);
    if (font == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return false;
    }
    return font->IsBaselineSnap();
}

void OH_Drawing_FontSetSubpixel(OH_Drawing_Font* cFont, bool isSubpixel)
{
    Font* font = CastToFont(cFont);
    if (font == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    font->SetSubpixel(isSubpixel);
}

bool OH_Drawing_FontIsSubpixel(const OH_Drawing_Font* cFont)
{
    const Font* font = CastToFont(cFont);
    if (font == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return false;
    }
    return font->IsSubpixel();
}

OH_Drawing_Font* OH_Drawing_FontCreate()
{
    Font* font = new Font();
    font->SetTypeface(DrawingFontUtils::GetZhCnTypeface());
    return (OH_Drawing_Font*)font;
}

void OH_Drawing_FontSetTypeface(OH_Drawing_Font* cFont, OH_Drawing_Typeface* cTypeface)
{
    Font* font = CastToFont(cFont);
    if (font == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    font->SetTypeface(std::shared_ptr<Typeface>{CastToTypeface(cTypeface), [](auto p) {}});
}

OH_Drawing_Typeface* OH_Drawing_FontGetTypeface(OH_Drawing_Font* cFont)
{
    Font* font = CastToFont(cFont);
    if (font == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return nullptr;
    }
    return (OH_Drawing_Typeface*)(font->GetTypeface().get());
}

void OH_Drawing_FontSetTextSize(OH_Drawing_Font* cFont, float textSize)
{
    Font* font = CastToFont(cFont);
    if (font == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    font->SetSize(textSize);
}

float OH_Drawing_FontGetTextSize(const OH_Drawing_Font* cFont)
{
    const Font* font = CastToFont(cFont);
    if (font == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return -1.0f;
    }
    return font->GetSize();
}

int OH_Drawing_FontCountText(OH_Drawing_Font* cFont, const void* text, size_t byteLength,
    OH_Drawing_TextEncoding encoding)
{
    if (cFont == nullptr || text == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return 0;
    }
    const Font* font = CastToFont(cFont);
    std::shared_ptr<Font> themeFont = DrawingFontUtils::GetThemeFont(font);
    if (themeFont != nullptr) {
        font = themeFont.get();
    }
    return font->CountText(text, byteLength, static_cast<TextEncoding>(encoding));
}

uint32_t OH_Drawing_FontTextToGlyphs(const OH_Drawing_Font* cFont, const void* text, uint32_t byteLength,
    OH_Drawing_TextEncoding encoding, uint16_t* glyphs, int maxGlyphCount)
{
    if (cFont == nullptr || text == nullptr || glyphs == nullptr || byteLength == 0 || maxGlyphCount <= 0) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return 0;
    }
    const Font* font = CastToFont(cFont);
    std::shared_ptr<Font> themeFont = DrawingFontUtils::GetThemeFont(font);
    if (themeFont != nullptr) {
        font = themeFont.get();
    }
    return font->TextToGlyphs(text, byteLength,
        static_cast<TextEncoding>(encoding), glyphs, maxGlyphCount);
}

void OH_Drawing_FontGetWidths(const OH_Drawing_Font* cFont, const uint16_t* glyphs, int count, float* widths)
{
    if (cFont == nullptr || glyphs == nullptr || widths == nullptr || count <= 0) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    const Font* font = CastToFont(cFont);
    std::shared_ptr<Font> themeFont = DrawingFontUtils::GetThemeFont(font);
    if (themeFont != nullptr) {
        font = themeFont.get();
    }
    font->GetWidths(glyphs, count, widths);
}

OH_Drawing_ErrorCode OH_Drawing_FontMeasureSingleCharacter(const OH_Drawing_Font* cFont, const char* str,
    float* textWidth)
{
    if (cFont == nullptr || str == nullptr || textWidth == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    size_t len = strlen(str);
    if (len == 0) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    const Font* font = CastToFont(cFont);
    const char* currentStr = str;
    int32_t unicode = SkUTF::NextUTF8(&currentStr, currentStr + len);
    std::shared_ptr<Font> themeFont = DrawingFontUtils::MatchThemeFont(font, unicode);
    if (themeFont != nullptr) {
        font = themeFont.get();
    }
    *textWidth = font->MeasureSingleCharacter(unicode);
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode OH_Drawing_FontMeasureSingleCharacterWithFeatures(const OH_Drawing_Font* font, const char* str,
    const OH_Drawing_FontFeatures* fontFeatures, float* textWidth)
{
    const DrawingFontFeatures* features = CastToFontFeatures(fontFeatures);
    if (font == nullptr || str == nullptr || features == nullptr ||textWidth == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }

    size_t len = strlen(str);
    if (len == 0) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    const Font* drawingFont = CastToFont(font);
    const char* currentStr = str;
    int32_t unicode = SkUTF::NextUTF8(&currentStr, currentStr + len);
    std::shared_ptr<Font> themeFont = DrawingFontUtils::MatchThemeFont(drawingFont, unicode);
    if (themeFont != nullptr) {
        drawingFont = themeFont.get();
    }
    int32_t utfCharLen = currentStr - str;
    std::vector<char> strBuffer(utfCharLen + 1);
    for (int32_t i = 0; i < utfCharLen; ++i) {
        strBuffer[i] = str[i];
    }
    strBuffer[utfCharLen] = 0;
    std::shared_ptr<Drawing::DrawingFontFeatures> featureCopy = std::make_shared<Drawing::DrawingFontFeatures>();
    for (const auto& entry : *features) {
        featureCopy->push_back(entry);
    }
    *textWidth = drawingFont->MeasureSingleCharacterWithFeatures(strBuffer.data(), unicode, featureCopy);
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_FontFeatures* OH_Drawing_FontFeaturesCreate(void)
{
    Drawing::DrawingFontFeatures* features = new Drawing::DrawingFontFeatures();
    return (OH_Drawing_FontFeatures*)features;
}

OH_Drawing_ErrorCode OH_Drawing_FontFeaturesDestroy(OH_Drawing_FontFeatures* fontFeatures)
{
    Drawing::DrawingFontFeatures* features = CastToFontFeatures(fontFeatures);
    if (features == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    delete features;
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode OH_Drawing_FontFeaturesAddFeature(OH_Drawing_FontFeatures* fontFeatures,
    const char* name, float value)
{
    Drawing::DrawingFontFeatures* features = CastToFontFeatures(fontFeatures);
    if (features == nullptr || name == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    features->push_back({{name, value}});
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode OH_Drawing_FontMeasureText(const OH_Drawing_Font* cFont, const void* text, size_t byteLength,
    OH_Drawing_TextEncoding encoding, OH_Drawing_Rect* bounds, float* textWidth)
{
    if (cFont == nullptr || text == nullptr || byteLength == 0 || textWidth == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    const Font* font = CastToFont(cFont);
    std::shared_ptr<Font> themeFont = DrawingFontUtils::GetThemeFont(font);
    if (themeFont != nullptr) {
        font = themeFont.get();
    }

    *textWidth = font->MeasureText(text, byteLength,
        static_cast<TextEncoding>(encoding), reinterpret_cast<Drawing::Rect*>(bounds));
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode OH_Drawing_FontMeasureTextWithBrushOrPen(const OH_Drawing_Font* cFont, const void* text,
    size_t byteLength, OH_Drawing_TextEncoding encoding, const OH_Drawing_Brush* brush, const OH_Drawing_Pen* pen,
    OH_Drawing_Rect* bounds, float* textWidth)
{
    if (cFont == nullptr || text == nullptr || byteLength == 0 || (brush != nullptr && pen != nullptr) ||
        textWidth == nullptr) {
        LOGE("OH_Drawing_FontMeasureTextWithBrushOrPen: Any of font, text and textWidth is nullptr or "
            "byteLength is 0 or brush and pen are both not nullptr.");
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    const Font* font = CastToFont(cFont);
    std::shared_ptr<Font> themeFont = DrawingFontUtils::GetThemeFont(font);
    if (themeFont != nullptr) {
        font = themeFont.get();
    }
    *textWidth = font->MeasureText(text, byteLength, static_cast<TextEncoding>(encoding),
        reinterpret_cast<Drawing::Rect*>(bounds), reinterpret_cast<const Drawing::Brush*>(brush),
        reinterpret_cast<const Drawing::Pen*>(pen));
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode OH_Drawing_FontGetWidthsBounds(const OH_Drawing_Font* cFont, const uint16_t* glyphs, int count,
    const OH_Drawing_Brush* brush, const OH_Drawing_Pen* pen, float* widths, OH_Drawing_Array* bounds)
{
    if (cFont == nullptr || glyphs == nullptr || count <= 0 || (brush != nullptr && pen != nullptr) ||
        (widths == nullptr && bounds == nullptr)) {
        LOGE("OH_Drawing_FontGetWidthsBounds: Any of font and glyphs is nullptr or count is no larger than 0 or "
            "brush and pen are both not nullptr.");
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    size_t size = 0;
    OH_Drawing_Rect* rectArr = nullptr;
    if (bounds) {
        if (OH_Drawing_RectGetArraySize(bounds, &size) != OH_DRAWING_SUCCESS) {
            return OH_DRAWING_ERROR_INVALID_PARAMETER;
        }
        if (size < static_cast<uint32_t>(count)) {
            return OH_DRAWING_ERROR_INVALID_PARAMETER;
        }
        if (OH_Drawing_RectGetArrayElement(bounds, 0, &rectArr) != OH_DRAWING_SUCCESS) {
            return OH_DRAWING_ERROR_INVALID_PARAMETER;
        }
        if (rectArr == nullptr) {
            return OH_DRAWING_ERROR_INVALID_PARAMETER;
        }
    }
    const Font* font = CastToFont(cFont);
    std::shared_ptr<Font> themeFont = DrawingFontUtils::GetThemeFont(font);
    if (themeFont != nullptr) {
        font = themeFont.get();
    }
    font->GetWidthsBounds(glyphs, count, widths, reinterpret_cast<Drawing::Rect*>(rectArr),
        reinterpret_cast<const Drawing::Brush*>(brush), reinterpret_cast<const Drawing::Pen*>(pen));
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode OH_Drawing_FontGetPos(const OH_Drawing_Font* cFont, const uint16_t* glyphs, int count,
    const OH_Drawing_Point* cOrigin, OH_Drawing_Point2D* cPoints)
{
    if (cFont == nullptr || glyphs == nullptr || count <= 0 || cPoints == nullptr) {
        LOGE("OH_Drawing_FontGetPos: Any of font, glyphs and points is nullptr or count is not larger than 0.");
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    const Font* font = CastToFont(cFont);
    std::shared_ptr<Font> themeFont = DrawingFontUtils::GetThemeFont(font);
    if (themeFont != nullptr) {
        font = themeFont.get();
    }

    Point* points = reinterpret_cast<Point*>(cPoints);
    Point origin(0, 0);
    if (cOrigin) {
        auto tempOrigin = reinterpret_cast<const Point*>(cOrigin);
        origin.SetX(tempOrigin->GetX());
        origin.SetY(tempOrigin->GetY());
    }
    font->GetPos(glyphs, count, points, origin);
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode OH_Drawing_FontGetSpacing(const OH_Drawing_Font* cFont, float* spacing)
{
    if (cFont == nullptr || spacing == nullptr) {
        LOGE("OH_Drawing_FontGetSpacing: any of font and spacing is nullptr.");
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    const Font* font = CastToFont(cFont);
    std::shared_ptr<Font> themeFont = DrawingFontUtils::GetThemeFont(font);
    if (themeFont != nullptr) {
        font = themeFont.get();
    }
    *spacing = font->GetSpacing();
    return OH_DRAWING_SUCCESS;
}

void OH_Drawing_FontSetLinearText(OH_Drawing_Font* cFont, bool isLinearText)
{
    Font* font = CastToFont(cFont);
    if (font == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    font->SetLinearMetrics(isLinearText);
}

bool OH_Drawing_FontIsLinearText(const OH_Drawing_Font* cFont)
{
    const Font* font = CastToFont(cFont);
    if (font == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return false;
    }
    return font->IsLinearMetrics();
}

void OH_Drawing_FontSetTextSkewX(OH_Drawing_Font* cFont, float skewX)
{
    Font* font = CastToFont(cFont);
    if (font == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    font->SetSkewX(skewX);
}

float OH_Drawing_FontGetTextSkewX(const OH_Drawing_Font* cFont)
{
    const Font* font = CastToFont(cFont);
    if (font == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return -1.0f;
    }
    return font->GetSkewX();
}

void OH_Drawing_FontSetFakeBoldText(OH_Drawing_Font* cFont, bool isFakeBoldText)
{
    Font* font = CastToFont(cFont);
    if (font == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    font->SetEmbolden(isFakeBoldText);
}

bool OH_Drawing_FontIsFakeBoldText(const OH_Drawing_Font* cFont)
{
    const Font* font = CastToFont(cFont);
    if (font == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return false;
    }
    return font->IsEmbolden();
}

void OH_Drawing_FontSetScaleX(OH_Drawing_Font* cFont, float scaleX)
{
    Font* font = CastToFont(cFont);
    if (font == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    font->SetScaleX(scaleX);
}

float OH_Drawing_FontGetScaleX(const OH_Drawing_Font* cFont)
{
    const Font* font = CastToFont(cFont);
    if (font == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return -1.0f;
    }
    return font->GetScaleX();
}

void OH_Drawing_FontSetEmbeddedBitmaps(OH_Drawing_Font* cFont, bool isEmbeddedBitmaps)
{
    Font* font = CastToFont(cFont);
    if (font == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    font->SetEmbeddedBitmaps(isEmbeddedBitmaps);
}

bool OH_Drawing_FontIsEmbeddedBitmaps(const OH_Drawing_Font* cFont)
{
    const Font* font = CastToFont(cFont);
    if (font == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return false;
    }
    return font->IsEmbeddedBitmaps();
}

void OH_Drawing_FontDestroy(OH_Drawing_Font* cFont)
{
    if (!cFont) {
        return;
    }
    delete CastToFont(cFont);
}

float OH_Drawing_FontGetMetrics(OH_Drawing_Font* cFont, OH_Drawing_Font_Metrics* cFontMetrics)
{
    float ret = -1;
    const Font* font = CastToFont(cFont);
    if (font == nullptr || cFontMetrics == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return ret;
    }
    FontMetrics metrics;
    std::shared_ptr<Font> themeFont = DrawingFontUtils::GetThemeFont(font);
    if (themeFont != nullptr) {
        font = themeFont.get();
    }
    ret = font->GetMetrics(&metrics);

    cFontMetrics->flags = metrics.fFlags;
    cFontMetrics->top = metrics.fTop;
    cFontMetrics->ascent = metrics.fAscent;
    cFontMetrics->descent = metrics.fDescent;
    cFontMetrics->leading = metrics.fLeading;
    cFontMetrics->bottom = metrics.fBottom;
    cFontMetrics->avgCharWidth = metrics.fAvgCharWidth;
    cFontMetrics->maxCharWidth = metrics.fMaxCharWidth;
    cFontMetrics->xMin = metrics.fXMin;
    cFontMetrics->xMax = metrics.fXMax;
    cFontMetrics->xHeight = metrics.fXHeight;
    cFontMetrics->capHeight = metrics.fCapHeight;
    cFontMetrics->underlineThickness = metrics.fUnderlineThickness;
    cFontMetrics->underlinePosition = metrics.fUnderlinePosition;
    cFontMetrics->strikeoutThickness = metrics.fStrikeoutThickness;
    cFontMetrics->strikeoutPosition = metrics.fStrikeoutPosition;
    return ret;
}

OH_Drawing_ErrorCode OH_Drawing_FontGetBounds(const OH_Drawing_Font* cFont, const uint16_t* glyphs, uint32_t count,
    OH_Drawing_Array* bounds)
{
    if (cFont == nullptr || glyphs == nullptr || bounds == nullptr || count == 0) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    size_t size = 0;
    if (OH_Drawing_RectGetArraySize(bounds, &size) != OH_DRAWING_SUCCESS) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    if (size < count) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    OH_Drawing_Rect* rectArr = nullptr;
    if (OH_Drawing_RectGetArrayElement(bounds, 0, &rectArr) != OH_DRAWING_SUCCESS) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    if (rectArr == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    const Font* font = CastToFont(cFont);
    std::shared_ptr<Font> themeFont = DrawingFontUtils::GetThemeFont(font);
    if (themeFont != nullptr) {
        font = themeFont.get();
    }
    font->GetWidths(glyphs, count, nullptr, reinterpret_cast<Drawing::Rect*>(rectArr));
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode OH_Drawing_FontGetPathForGlyph(const OH_Drawing_Font* cFont, uint16_t glyph,
    OH_Drawing_Path* path)
{
    const Font* font = CastToFont(cFont);
    if (font == nullptr || path == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    std::shared_ptr<Font> themeFont = DrawingFontUtils::GetThemeFont(font);
    if (themeFont != nullptr) {
        font = themeFont.get();
    }
    if (!font->GetPathForGlyph(glyph, reinterpret_cast<Path*>(path))) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode OH_Drawing_FontGetTextPath(const OH_Drawing_Font* cFont, const void* text, size_t byteLength,
    OH_Drawing_TextEncoding encoding, float x, float y, OH_Drawing_Path* path)
{
    const Font* font = CastToFont(cFont);
    if (font == nullptr || text == nullptr || byteLength == 0 || path == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    std::shared_ptr<Font> themeFont = DrawingFontUtils::GetThemeFont(font);
    if (themeFont != nullptr) {
        font = themeFont.get();
    }
    font->GetTextPath(text, byteLength, static_cast<TextEncoding>(encoding), x, y, reinterpret_cast<Path*>(path));
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode OH_Drawing_FontSetThemeFontFollowed(OH_Drawing_Font* cFont, bool followed)
{
    Font* font = CastToFont(cFont);
    if (font == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    font->SetThemeFontFollowed(followed);
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode OH_Drawing_FontIsThemeFontFollowed(const OH_Drawing_Font* cFont, bool* followed)
{
    const Font* font = CastToFont(cFont);
    if (followed == nullptr || font == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    *followed = font->IsThemeFontFollowed();
    return OH_DRAWING_SUCCESS;
}