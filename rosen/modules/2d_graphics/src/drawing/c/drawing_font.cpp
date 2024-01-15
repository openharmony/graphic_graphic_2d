/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "c/drawing_font.h"

#include "text/font.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

static Font* CastToFont(OH_Drawing_Font* cFont)
{
    return reinterpret_cast<Font*>(cFont);
}

static Typeface* CastToTypeface(OH_Drawing_Typeface* cTypeface)
{
    return reinterpret_cast<Typeface*>(cTypeface);
}

OH_Drawing_Font* OH_Drawing_FontCreate()
{
    return (OH_Drawing_Font*)new Font;
}

void OH_Drawing_FontSetTypeface(OH_Drawing_Font* cFont, OH_Drawing_Typeface* cTypeface)
{
    Font* font = CastToFont(cFont);
    if (font == nullptr) {
        return;
    }
    font->SetTypeface(std::shared_ptr<Typeface>{CastToTypeface(cTypeface), [](auto p) {}});
}

void OH_Drawing_FontSetTextSize(OH_Drawing_Font* cFont, float textSize)
{
    Font* font = CastToFont(cFont);
    if (font == nullptr) {
        return;
    }
    font->SetSize(textSize);
}

void OH_Drawing_FontSetLinearText(OH_Drawing_Font* cFont, bool isLinearText)
{
    Font* font = CastToFont(cFont);
    if (font == nullptr) {
        return;
    }
    font->SetLinearMetrics(isLinearText);
}

void OH_Drawing_FontSetTextSkewX(OH_Drawing_Font* cFont, float skewX)
{
    Font* font = CastToFont(cFont);
    if (font == nullptr) {
        return;
    }
    font->SetSkewX(skewX);
}

void OH_Drawing_FontSetFakeBoldText(OH_Drawing_Font* cFont, bool isFakeBoldText)
{
    Font* font = CastToFont(cFont);
    if (font == nullptr) {
        return;
    }
    font->SetEmbolden(isFakeBoldText);
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