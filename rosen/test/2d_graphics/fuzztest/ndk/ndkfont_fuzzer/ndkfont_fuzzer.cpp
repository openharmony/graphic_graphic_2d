/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#include "ndkfont_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "get_object.h"

#include "drawing_font.h"
#include "drawing_typeface.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t DATA_MIN_SIZE = 2;
constexpr size_t MAX_ARRAY_SIZE = 5000;
constexpr size_t FONTENDING_SIZE = 3;
constexpr size_t FONTHINTING_SIZE = 4;
constexpr size_t TEXTENCODING_SIZE = 4;
} // namespace

namespace Drawing {

void NativeDrawingFontTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    uint32_t edging = GetObject<uint32_t>();
    OH_Drawing_FontSetEdging(nullptr, static_cast<OH_Drawing_FontEdging>(edging % FONTENDING_SIZE));
    OH_Drawing_FontSetEdging(font, static_cast<OH_Drawing_FontEdging>(edging % FONTENDING_SIZE));
    OH_Drawing_FontGetEdging(nullptr);
    OH_Drawing_FontGetEdging(font);

    uint32_t hinting = GetObject<uint32_t>();
    OH_Drawing_FontSetHinting(nullptr, static_cast<OH_Drawing_FontHinting>(hinting % FONTHINTING_SIZE));
    OH_Drawing_FontSetHinting(font, static_cast<OH_Drawing_FontHinting>(hinting % FONTHINTING_SIZE));
    OH_Drawing_FontGetHinting(nullptr);
    OH_Drawing_FontGetHinting(font);

    bool isAntiAlias = GetObject<bool>();
    OH_Drawing_FontSetForceAutoHinting(nullptr, isAntiAlias);
    OH_Drawing_FontSetForceAutoHinting(font, isAntiAlias);
    OH_Drawing_FontIsForceAutoHinting(nullptr);
    OH_Drawing_FontIsForceAutoHinting(font);
    OH_Drawing_FontDestroy(font);
}

void NativeDrawingFontTest002(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    bool isBaselineSnap = GetObject<bool>();
    OH_Drawing_FontSetBaselineSnap(nullptr, isBaselineSnap);
    OH_Drawing_FontSetBaselineSnap(font, isBaselineSnap);
    OH_Drawing_FontIsBaselineSnap(nullptr);
    OH_Drawing_FontIsBaselineSnap(font);

    bool isSubpixel = GetObject<bool>();
    OH_Drawing_FontSetSubpixel(nullptr, isSubpixel);
    OH_Drawing_FontSetSubpixel(font, isSubpixel);
    OH_Drawing_FontIsSubpixel(nullptr);
    OH_Drawing_FontIsSubpixel(font);

    OH_Drawing_Typeface* typeFace = OH_Drawing_TypefaceCreateDefault();
    OH_Drawing_FontSetTypeface(nullptr, typeFace);
    OH_Drawing_FontSetTypeface(font, typeFace);
    OH_Drawing_Typeface* getTypeFace = OH_Drawing_FontGetTypeface(nullptr);
    OH_Drawing_Typeface* getTypeFace1 = OH_Drawing_FontGetTypeface(font);

    float textSize = GetObject<float>();
    OH_Drawing_FontSetTextSize(nullptr, textSize);
    OH_Drawing_FontSetTextSize(font, textSize);
    OH_Drawing_FontGetTextSize(nullptr);
    OH_Drawing_FontGetTextSize(font);

    OH_Drawing_FontDestroy(font);
    OH_Drawing_TypefaceDestroy(typeFace);
    OH_Drawing_TypefaceDestroy(getTypeFace);
    OH_Drawing_TypefaceDestroy(getTypeFace1);
}

void NativeDrawingFontTest003(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    uint32_t str_size = GetObject<uint32_t>() % MAX_ARRAY_SIZE;
    char* str = new char[str_size];
    for (size_t i = 0; i < str_size; i++) {
        str[i] = GetObject<char>();
    }
    uint32_t encoding = GetObject<uint32_t>();
    OH_Drawing_FontCountText(nullptr, str, str_size,
        static_cast<OH_Drawing_TextEncoding>(encoding % TEXTENCODING_SIZE));
    OH_Drawing_FontCountText(font, str, str_size, static_cast<OH_Drawing_TextEncoding>(encoding % TEXTENCODING_SIZE));
    if (str != nullptr) {
        delete [] str;
        str = nullptr;
    }
    OH_Drawing_FontDestroy(font);
}
void NativeDrawingFontTest004(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    OH_Drawing_Font* font = OH_Drawing_FontCreate();

    bool isLinearText = GetObject<bool>();
    OH_Drawing_FontSetLinearText(nullptr, isLinearText);
    OH_Drawing_FontSetLinearText(font, isLinearText);
    OH_Drawing_FontIsLinearText(nullptr);
    OH_Drawing_FontIsLinearText(font);

    float skewX = GetObject<float>();
    OH_Drawing_FontSetTextSkewX(nullptr, skewX);
    OH_Drawing_FontSetTextSkewX(font, skewX);
    OH_Drawing_FontGetTextSkewX(nullptr);
    OH_Drawing_FontGetTextSkewX(font);

    bool isFakeBoldText = GetObject<bool>();
    OH_Drawing_FontSetFakeBoldText(nullptr, isFakeBoldText);
    OH_Drawing_FontSetFakeBoldText(font, isFakeBoldText);
    OH_Drawing_FontIsFakeBoldText(nullptr);
    OH_Drawing_FontIsFakeBoldText(font);

    bool scaleX = GetObject<bool>();
    OH_Drawing_FontSetScaleX(nullptr, scaleX);
    OH_Drawing_FontSetScaleX(font, scaleX);
    OH_Drawing_FontGetScaleX(nullptr);
    OH_Drawing_FontGetScaleX(font);

    bool isEmbeddedBitmaps = GetObject<bool>();
    OH_Drawing_FontSetEmbeddedBitmaps(nullptr, isEmbeddedBitmaps);
    OH_Drawing_FontSetEmbeddedBitmaps(font, isEmbeddedBitmaps);
    OH_Drawing_FontIsEmbeddedBitmaps(nullptr);
    OH_Drawing_FontIsEmbeddedBitmaps(font);

    OH_Drawing_FontDestroy(font);
}
void NativeDrawingFontTest005(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    uint32_t count = GetObject<uint32_t>() % MAX_ARRAY_SIZE + 1;
    char* str = new char[count];
    for (size_t i = 0; i < count; i++) {
        str[i] = GetObject<char>();
    }
    str[count - 1] = '\0';
    uint32_t encoding = GetObject<uint32_t>();
    OH_Drawing_FontCountText(nullptr, str, count,
        static_cast<OH_Drawing_TextEncoding>(encoding % TEXTENCODING_SIZE));
    OH_Drawing_FontCountText(font, str, count,
        static_cast<OH_Drawing_TextEncoding>(encoding % TEXTENCODING_SIZE));
    if (str != nullptr) {
        delete [] str;
        str = nullptr;
    }
    OH_Drawing_FontDestroy(font);
}

void NativeDrawingFontTest006(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    uint32_t count = GetObject<uint32_t>() % MAX_ARRAY_SIZE + 1;
    float textWidth;
    char* str = new char[count];
    for (size_t i = 0; i < count; i++) {
        str[i] = GetObject<char>();
    }
    str[count - 1] = '\0';

    uint16_t glyphs[count];
    OH_Drawing_FontTextToGlyphs(font, str, strlen(str), TEXT_ENCODING_UTF8, glyphs, count);
    OH_Drawing_FontTextToGlyphs(nullptr, str, strlen(str), TEXT_ENCODING_UTF8, glyphs, count);
    OH_Drawing_Font_Metrics metrics;
    OH_Drawing_FontGetMetrics(font, &metrics);
    OH_Drawing_FontGetMetrics(nullptr, &metrics);
    OH_Drawing_FontMeasureText(font, str, strlen(str), TEXT_ENCODING_UTF8, nullptr, &textWidth);
    OH_Drawing_FontMeasureText(nullptr, str, strlen(str), TEXT_ENCODING_UTF8, nullptr, &textWidth);

    if (str != nullptr) {
        delete [] str;
        str = nullptr;
    }
    OH_Drawing_FontDestroy(font);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::NativeDrawingFontTest001(data, size);
    OHOS::Rosen::Drawing::NativeDrawingFontTest002(data, size);
    OHOS::Rosen::Drawing::NativeDrawingFontTest003(data, size);
    OHOS::Rosen::Drawing::NativeDrawingFontTest004(data, size);
    OHOS::Rosen::Drawing::NativeDrawingFontTest005(data, size);
    OHOS::Rosen::Drawing::NativeDrawingFontTest006(data, size);
    return 0;
}
