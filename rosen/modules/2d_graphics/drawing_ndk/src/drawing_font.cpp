/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#include "drawing_canvas_utils.h"
#include "text/font.h"

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

static const Font& CastToFont(const OH_Drawing_Font& cFont)
{
    return reinterpret_cast<const Font&>(cFont);
}

static Typeface* CastToTypeface(OH_Drawing_Typeface* cTypeface)
{
    return reinterpret_cast<Typeface*>(cTypeface);
}

static OH_Drawing_FontEdging EdgingCastToCEdging(FontEdging edging)
{
    OH_Drawing_FontEdging cEdging = FONT_EDGING_ALIAS;
    switch (edging) {
        case FontEdging::ALIAS:
            cEdging = FONT_EDGING_ALIAS;
            break;
        case FontEdging::ANTI_ALIAS:
            cEdging = FONT_EDGING_ANTI_ALIAS;
            break;
        case FontEdging::SUBPIXEL_ANTI_ALIAS:
            cEdging = FONT_EDGING_SUBPIXEL_ANTI_ALIAS;
            break;
        default:
            break;
    }
    return cEdging;
}

static FontEdging CEdgingCastToEdging(OH_Drawing_FontEdging cEdging)
{
    FontEdging edging = FontEdging::ALIAS;
    switch (cEdging) {
        case FONT_EDGING_ALIAS:
            edging = FontEdging::ALIAS;
            break;
        case FONT_EDGING_ANTI_ALIAS:
            edging = FontEdging::ANTI_ALIAS;
            break;
        case FONT_EDGING_SUBPIXEL_ANTI_ALIAS:
            edging = FontEdging::SUBPIXEL_ANTI_ALIAS;
            break;
        default:
            break;
    }
    return edging;
}

static OH_Drawing_FontHinting HintingCastToCHinting(FontHinting hinting)
{
    OH_Drawing_FontHinting cHinting = FONT_HINTING_NONE;
    switch (hinting) {
        case FontHinting::NONE:
            cHinting = FONT_HINTING_NONE;
            break;
        case FontHinting::SLIGHT:
            cHinting = FONT_HINTING_SLIGHT;
            break;
        case FontHinting::NORMAL:
            cHinting = FONT_HINTING_NORMAL;
            break;
        case FontHinting::FULL:
            cHinting = FONT_HINTING_FULL;
            break;
        default:
            break;
    }
    return cHinting;
}

static FontHinting CHintingCastToHinting(OH_Drawing_FontHinting cHinting)
{
    FontHinting hinting = FontHinting::NONE;
    switch (cHinting) {
        case FONT_HINTING_NONE:
            hinting = FontHinting::NONE;
            break;
        case FONT_HINTING_SLIGHT:
            hinting = FontHinting::SLIGHT;
            break;
        case FONT_HINTING_NORMAL:
            hinting = FontHinting::NORMAL;
            break;
        case FONT_HINTING_FULL:
            hinting = FontHinting::FULL;
            break;
        default:
            break;
    }
    return hinting;
}

void OH_Drawing_FontSetEdging(OH_Drawing_Font* cFont, OH_Drawing_FontEdging cEdging)
{
    Font* font = CastToFont(cFont);
    if (font == nullptr) {
        return;
    }
    font->SetEdging(CEdgingCastToEdging(cEdging));
}

OH_Drawing_FontEdging OH_Drawing_FontGetEdging(const OH_Drawing_Font* cFont)
{
    const Font* font = CastToFont(cFont);
    if (font == nullptr) {
        return FONT_EDGING_ALIAS;
    }
    return EdgingCastToCEdging(font->GetEdging());
}

void OH_Drawing_FontSetHinting(OH_Drawing_Font* cFont, OH_Drawing_FontHinting cHinting)
{
    Font* font = CastToFont(cFont);
    if (font == nullptr) {
        return;
    }
    font->SetHinting(CHintingCastToHinting(cHinting));
}

OH_Drawing_FontHinting OH_Drawing_FontGetHinting(const OH_Drawing_Font* cFont)
{
    const Font* font = CastToFont(cFont);
    if (font == nullptr) {
        return FONT_HINTING_NONE;
    }
    return HintingCastToCHinting(font->GetHinting());
}

void OH_Drawing_FontSetForceAutoHinting(OH_Drawing_Font* cFont, bool isForceAutoHinting)
{
    Font* font = CastToFont(cFont);
    if (font == nullptr) {
        return;
    }
    font->SetForceAutoHinting(isForceAutoHinting);
}

bool OH_Drawing_FontIsForceAutoHinting(const OH_Drawing_Font* cFont)
{
    const Font* font = CastToFont(cFont);
    if (font == nullptr) {
        return false;
    }
    return font->IsForceAutoHinting();
}

void OH_Drawing_FontSetBaselineSnap(OH_Drawing_Font* cFont, bool baselineSnap)
{
    Font* font = CastToFont(cFont);
    if (font == nullptr) {
        return;
    }
    font->SetBaselineSnap(baselineSnap);
}

bool OH_Drawing_FontIsBaselineSnap(const OH_Drawing_Font* cFont)
{
    const Font* font = CastToFont(cFont);
    if (font == nullptr) {
        return false;
    }
    return font->IsBaselineSnap();
}

void OH_Drawing_FontSetSubpixel(OH_Drawing_Font* cFont, bool isSubpixel)
{
    Font* font = CastToFont(cFont);
    if (font == nullptr) {
        return;
    }
    font->SetSubpixel(isSubpixel);
}

bool OH_Drawing_FontIsSubpixel(const OH_Drawing_Font* cFont)
{
    const Font* font = CastToFont(cFont);
    if (font == nullptr) {
        return false;
    }
    return font->IsSubpixel();
}

OH_Drawing_Font* OH_Drawing_FontCreate()
{
    Font* font = new Font();
    font->SetTypeface(g_LoadZhCnTypeface());
    return (OH_Drawing_Font*)font;
}

void OH_Drawing_FontSetTypeface(OH_Drawing_Font* cFont, OH_Drawing_Typeface* cTypeface)
{
    Font* font = CastToFont(cFont);
    if (font == nullptr) {
        return;
    }
    font->SetTypeface(std::shared_ptr<Typeface>{CastToTypeface(cTypeface), [](auto p) {}});
}

OH_Drawing_Typeface* OH_Drawing_FontGetTypeface(OH_Drawing_Font* cFont)
{
    Font* font = CastToFont(cFont);
    if (font == nullptr) {
        return nullptr;
    }
    return (OH_Drawing_Typeface*)(font->GetTypeface().get());
}

void OH_Drawing_FontSetTextSize(OH_Drawing_Font* cFont, float textSize)
{
    Font* font = CastToFont(cFont);
    if (font == nullptr) {
        return;
    }
    font->SetSize(textSize);
}

float OH_Drawing_FontGetTextSize(const OH_Drawing_Font* cFont)
{
    const Font* font = CastToFont(cFont);
    if (font == nullptr) {
        return -1.0f;
    }
    return font->GetSize();
}

int OH_Drawing_FontCountText(OH_Drawing_Font* cFont, const void* text, size_t byteLength,
    OH_Drawing_TextEncoding encoding)
{
    if (cFont == nullptr || text == nullptr) {
        return 0;
    }
    Font* font = CastToFont(cFont);
    return font->CountText(text, byteLength, static_cast<TextEncoding>(encoding));
}

uint32_t OH_Drawing_FontTextToGlyphs(const OH_Drawing_Font* cFont, const void* text, uint32_t byteLength,
    OH_Drawing_TextEncoding encoding, uint16_t* glyphs, int maxGlyphCount)
{
    if (cFont == nullptr || text == nullptr || glyphs == nullptr || byteLength <= 0 || maxGlyphCount <= 0) {
        return 0;
    }
    return CastToFont(*cFont).TextToGlyphs(text, byteLength,
        static_cast<TextEncoding>(encoding), glyphs, maxGlyphCount);
}

void OH_Drawing_FontGetWidths(const OH_Drawing_Font* cFont, const uint16_t* glyphs, int count, float* widths)
{
    if (cFont == nullptr || glyphs == nullptr || widths == nullptr || count <= 0) {
        return;
    }
    CastToFont(*cFont).GetWidths(glyphs, count, widths);
}

void OH_Drawing_FontSetLinearText(OH_Drawing_Font* cFont, bool isLinearText)
{
    Font* font = CastToFont(cFont);
    if (font == nullptr) {
        return;
    }
    font->SetLinearMetrics(isLinearText);
}

bool OH_Drawing_FontIsLinearText(const OH_Drawing_Font* cFont)
{
    const Font* font = CastToFont(cFont);
    if (font == nullptr) {
        return false;
    }
    return font->IsLinearMetrics();
}

void OH_Drawing_FontSetTextSkewX(OH_Drawing_Font* cFont, float skewX)
{
    Font* font = CastToFont(cFont);
    if (font == nullptr) {
        return;
    }
    font->SetSkewX(skewX);
}

float OH_Drawing_FontGetTextSkewX(const OH_Drawing_Font* cFont)
{
    const Font* font = CastToFont(cFont);
    if (font == nullptr) {
        return -1.0f;
    }
    return font->GetSkewX();
}

void OH_Drawing_FontSetFakeBoldText(OH_Drawing_Font* cFont, bool isFakeBoldText)
{
    Font* font = CastToFont(cFont);
    if (font == nullptr) {
        return;
    }
    font->SetEmbolden(isFakeBoldText);
}

bool OH_Drawing_FontIsFakeBoldText(const OH_Drawing_Font* cFont)
{
    const Font* font = CastToFont(cFont);
    if (font == nullptr) {
        return false;
    }
    return font->IsEmbolden();
}

void OH_Drawing_FontSetScaleX(OH_Drawing_Font* cFont, float scaleX)
{
    Font* font = CastToFont(cFont);
    if (font == nullptr) {
        return;
    }
    font->SetScaleX(scaleX);
}

float OH_Drawing_FontGetScaleX(const OH_Drawing_Font* cFont)
{
    const Font* font = CastToFont(cFont);
    if (cFont == nullptr) {
        return -1.0f;
    }
    return font->GetScaleX();
}

void OH_Drawing_FontSetEmbeddedBitmaps(OH_Drawing_Font* cFont, bool isEmbeddedBitmaps)
{
    Font* font = CastToFont(cFont);
    if (font == nullptr) {
        return;
    }
    font->SetEmbeddedBitmaps(isEmbeddedBitmaps);
}

bool OH_Drawing_FontIsEmbeddedBitmaps(const OH_Drawing_Font* cFont)
{
    const Font* font = CastToFont(cFont);
    if (font == nullptr) {
        return false;
    }
    return font->IsEmbeddedBitmaps();
}

void OH_Drawing_FontDestroy(OH_Drawing_Font* cFont)
{
    delete CastToFont(cFont);
}

float OH_Drawing_FontGetMetrics(OH_Drawing_Font* cFont, OH_Drawing_Font_Metrics* cFontMetrics)
{
    float ret = -1;
    Font* font = CastToFont(cFont);
    if (cFont == nullptr || cFontMetrics == nullptr) {
        return ret;
    }
    FontMetrics metrics;
    ret = font->GetMetrics(&metrics);

    cFontMetrics->top = metrics.fTop;
    cFontMetrics->ascent = metrics.fAscent;
    cFontMetrics->descent = metrics.fDescent;
    cFontMetrics->leading = metrics.fLeading;
    cFontMetrics->bottom = metrics.fBottom;
    return ret;
}