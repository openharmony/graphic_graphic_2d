/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "font_fuzzer.h"
#include <cstddef>
#include <cstdint>
#include "get_object.h"
#include "text/font.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t EFING_SIZE = 3;
constexpr size_t HINTING_SIZE = 4;
constexpr size_t MAX_SIZE = 5000;
constexpr size_t TEXTENCODING_SIZE = 4;
} // namespace
namespace Drawing {

bool FontFuzzTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<Typeface> typeface = Typeface::MakeDefault();
    scalar fSize = GetObject<scalar>();
    scalar scaleX = GetObject<scalar>();
    scalar skewX = GetObject<scalar>();
    Font font = Font(typeface, fSize, scaleX, skewX);
    return true;
}

bool FontFuzzTest002(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Font font;
    FontMetrics metrics;
    uint32_t edging = GetObject<uint32_t>();
    font.SetEdging(static_cast<FontEdging>(edging % EFING_SIZE));
    font.GetEdging();
    bool baselineSnap = GetObject<bool>();
    font.SetBaselineSnap(baselineSnap);
    font.IsBaselineSnap();
    bool isForceAutoHinting = GetObject<bool>();
    font.SetForceAutoHinting(isForceAutoHinting);
    font.IsForceAutoHinting();
    bool isSubpixel = GetObject<bool>();
    font.SetSubpixel(isSubpixel);
    font.IsSubpixel();
    uint32_t hintingLevel = GetObject<uint32_t>();
    font.SetHinting(static_cast<FontHinting>(hintingLevel % HINTING_SIZE));
    font.GetHinting();
    bool embeddedBitmaps = GetObject<bool>();
    font.SetEmbeddedBitmaps(embeddedBitmaps);
    font.IsEmbeddedBitmaps();
    std::shared_ptr<Typeface> typeface = Typeface::MakeDefault();
    font.SetTypeface(typeface);
    font.GetTypeface();
    scalar fSize = GetObject<scalar>();
    font.SetSize(fSize);
    font.GetSize();
    scalar scaleX = GetObject<scalar>();
    font.SetSkewX(scaleX);
    font.GetSkewX();
    scalar skewX = GetObject<scalar>();
    font.SetScaleX(skewX);
    font.GetScaleX();
    font.GetMetrics(&metrics);
    return true;
}

bool FontFuzzTest003(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Font font;
    bool isEmbolden = GetObject<bool>();
    font.SetEmbolden(isEmbolden);
    font.IsEmbolden();
    bool isLinearMetrics = GetObject<bool>();
    font.SetLinearMetrics(isLinearMetrics);
    font.IsLinearMetrics();
    return true;
}

bool FontFuzzTest004(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Font font;
    uint32_t count = GetObject<uint32_t>() % MAX_SIZE + 1;
    uint16_t* glyphs = new uint16_t[count];
    for (size_t i = 0; i < count; i++) {
        glyphs[i] = GetObject<uint16_t>();
    }
    scalar* widths = new scalar[count];
    for (size_t i = 0; i < count; i++) {
        widths[i] = GetObject<scalar>();
    }
    font.GetWidths(glyphs, count, widths);
    if (glyphs != nullptr) {
        delete [] glyphs;
        glyphs = nullptr;
    }
    if (widths != nullptr) {
        delete [] widths;
        widths = nullptr;
    }
    return true;
}

bool FontFuzzTest005(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Font font;
    uint32_t count = GetObject<uint32_t>() % MAX_SIZE;
    uint16_t* glyphs = new uint16_t[count];
    for (size_t i = 0; i < count; i++) {
        glyphs[i] = GetObject<uint16_t>();
    }
    scalar* widths = new scalar[count];
    for (size_t i = 0; i < count; i++) {
        widths[i] = GetObject<scalar>();
    }
    Rect* bounds = new Rect[count];
    for (size_t i = 0; i < count; i++) {
        RectF rect(GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>());
        bounds[i] = rect;
    }
    font.GetWidths(glyphs, count, widths, bounds);
    if (glyphs != nullptr) {
        delete [] glyphs;
        glyphs = nullptr;
    }
    if (widths != nullptr) {
        delete [] widths;
        widths = nullptr;
    }
    if (bounds != nullptr) {
        delete [] bounds;
        bounds = nullptr;
    }
    return true;
}

bool FontFuzzTest006(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Font font;
    uint32_t count = GetObject<uint32_t>() % MAX_SIZE + 1;
    char* text = new char[count];
    for (size_t i = 0; i < count; i++) {
        text[i] = GetObject<char>();
    }
    text[count - 1] = '\0';
    uint16_t* glyphs = new uint16_t[count - 1];
    for (size_t i = 0; i < count - 1; i++) {
        glyphs[i] = GetObject<uint16_t>();
    }
    font.TextToGlyphs(text, count - 1, TextEncoding::UTF8, glyphs, count);
    font.UnicharToGlyph(count);
    if (text != nullptr) {
        delete [] text;
        text = nullptr;
    }
    if (glyphs != nullptr) {
        delete [] glyphs;
        glyphs = nullptr;
    }
    return true;
}

bool FontFuzzTest007(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Font font;
    uint32_t count = GetObject<uint32_t>() % MAX_SIZE + 1;
    char* text = new char[count];
    for (size_t i = 0; i < count; i++) {
        text[i] = GetObject<char>();
    }
    text[count - 1] = '\0';
    font.CountText(text, count - 1, TextEncoding::UTF8);
    if (text != nullptr) {
        delete [] text;
        text = nullptr;
    }
    return true;
}

bool FontFuzzTest008(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Font font;
    uint32_t count = GetObject<uint32_t>() % MAX_SIZE + 1;
    char* text = new char[count];
    if (text == nullptr) {
        return false;
    }
    for (size_t i = 0; i < count; i++) {
        text[i] = GetObject<char>();
    }
    text[count - 1] = '\0';
    uint32_t textEncoding = GetObject<uint32_t>();
    RectF rect(GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>());
    font.MeasureText(text, count - 1, static_cast<TextEncoding>(textEncoding % TEXTENCODING_SIZE), &rect);
    if (text != nullptr) {
        delete [] text;
        text = nullptr;
    }
    return true;
}

bool FontFuzzTest009(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Font font;
    uint32_t count = GetObject<uint32_t>() % MAX_SIZE;
    for (size_t i = 0; i < count; i++) {
        Path *path = new Path();
        uint16_t glyph = GetObject<uint16_t>();
        font.GetPathForGlyph(glyph, path);
        delete path;
    }
    return true;
}

bool FontFuzzTest010(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Font font;
    uint32_t count = GetObject<uint32_t>() % MAX_SIZE + 1;
    char* text = new char[count];
    for (size_t i = 0; i < count; i++) {
        text[i] = GetObject<char>();
    }
    text[count - 1] = '\0';
    Path* path = new Path();
    float x = GetObject<float>();
    float y = GetObject<float>();
    font.GetTextPath(text, count - 1, TextEncoding::UTF8, x, y, path);
    delete path;
    if (text != nullptr) {
        delete [] text;
        text = nullptr;
    }
    return true;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::FontFuzzTest001(data, size);
    OHOS::Rosen::Drawing::FontFuzzTest002(data, size);
    OHOS::Rosen::Drawing::FontFuzzTest003(data, size);
    OHOS::Rosen::Drawing::FontFuzzTest004(data, size);
    OHOS::Rosen::Drawing::FontFuzzTest005(data, size);
    OHOS::Rosen::Drawing::FontFuzzTest006(data, size);
    OHOS::Rosen::Drawing::FontFuzzTest007(data, size);
    OHOS::Rosen::Drawing::FontFuzzTest008(data, size);
    OHOS::Rosen::Drawing::FontFuzzTest009(data, size);
    OHOS::Rosen::Drawing::FontFuzzTest010(data, size);
    return 0;
}
