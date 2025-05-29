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

#include "ndktextblob_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "get_object.h"

#include "drawing_font.h"
#include "drawing_rect.h"
#include "drawing_text_blob.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t DATA_MIN_SIZE = 2;
constexpr size_t ENCODING_SIZE = 4;
constexpr size_t MAX_ARRAY_SIZE = 5000;
} // namespace

namespace Drawing {

void NativeDrawingTextBlobTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    OH_Drawing_TextBlobBuilder* textBlobBuilder = OH_Drawing_TextBlobBuilderCreate();
    OH_Drawing_TextBlob* textBlob = OH_Drawing_TextBlobBuilderMake(textBlobBuilder);
    OH_Drawing_TextBlob* textBlobTwo = OH_Drawing_TextBlobBuilderMake(nullptr);

    float left = GetObject<float>();
    float top = GetObject<float>();
    float right = GetObject<float>();
    float bottom = GetObject<float>();
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(left, top, right, bottom);
    OH_Drawing_TextBlobGetBounds(nullptr, rect);
    OH_Drawing_TextBlobGetBounds(textBlob, nullptr);
    OH_Drawing_TextBlobGetBounds(textBlob, rect);

    OH_Drawing_TextBlobUniqueID(nullptr);
    OH_Drawing_TextBlobUniqueID(textBlob);

    OH_Drawing_TextBlobDestroy(textBlob);
    OH_Drawing_TextBlobDestroy(textBlobTwo);
    OH_Drawing_TextBlobBuilderDestroy(textBlobBuilder);
    OH_Drawing_FontDestroy(font);
    OH_Drawing_RectDestroy(rect);
}

void NativeDrawingTextBlobTest002(const uint8_t* data, size_t size)
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
    OH_Drawing_Point2D* point = new OH_Drawing_Point2D[count];
    for (size_t i = 0; i < count; i++) {
        str[i] = GetObject<char>();
        point[i].x = GetObject<float>();
        point[i].y = GetObject<float>();
    }
    str[count - 1] = '\0';

    OH_Drawing_TextBlobCreateFromText(nullptr, strlen(str), nullptr, TEXT_ENCODING_UTF8);
    OH_Drawing_TextBlobCreateFromPosText(nullptr, strlen(str), nullptr, nullptr, TEXT_ENCODING_UTF8);
    OH_Drawing_TextBlobCreateFromString(nullptr, nullptr, TEXT_ENCODING_UTF8);
    OH_Drawing_TextBlobCreateFromString(str, font, static_cast<OH_Drawing_TextEncoding>(count + ENCODING_SIZE));
    OH_Drawing_TextBlobCreateFromPosText(str, strlen(str), point,
        font, static_cast<OH_Drawing_TextEncoding>(count + ENCODING_SIZE));
    OH_Drawing_TextBlobCreateFromText(str, strlen(str), font,
        static_cast<OH_Drawing_TextEncoding>(count + ENCODING_SIZE));
    OH_Drawing_TextBlob* textBlob1 = OH_Drawing_TextBlobCreateFromText(str, strlen(str), font, TEXT_ENCODING_UTF8);
    OH_Drawing_TextBlob* textBlob2 =
        OH_Drawing_TextBlobCreateFromPosText(str, strlen(str), point, font, TEXT_ENCODING_UTF8);
    OH_Drawing_TextBlob* textBlob3 = OH_Drawing_TextBlobCreateFromString(str, font, TEXT_ENCODING_UTF8);
    OH_Drawing_TextBlobBuilderAllocRunPos(nullptr, nullptr, count, nullptr);

    uint16_t glyphs[count];
    OH_Drawing_TextBlobBuilder *builder = OH_Drawing_TextBlobBuilderCreate();
    uint32_t glyphsCount =
            OH_Drawing_FontTextToGlyphs(font, str, strlen(str), TEXT_ENCODING_UTF8, glyphs, count);
    OH_Drawing_TextBlobBuilderAllocRunPos(nullptr, font, glyphsCount, nullptr);
    OH_Drawing_TextBlobBuilderAllocRunPos(builder, nullptr, glyphsCount, nullptr);
    OH_Drawing_TextBlobBuilderAllocRunPos(builder, font, glyphsCount, nullptr);
    OH_Drawing_TextBlob *textBlob4 = OH_Drawing_TextBlobBuilderMake(builder);

    if (str != nullptr) {
        delete [] str;
        str = nullptr;
    }
    if (point != nullptr) {
        delete [] point;
        point = nullptr;
    }
    OH_Drawing_TextBlobDestroy(textBlob1);
    OH_Drawing_TextBlobDestroy(textBlob2);
    OH_Drawing_TextBlobDestroy(textBlob3);
    OH_Drawing_TextBlobDestroy(textBlob4);
    OH_Drawing_TextBlobBuilderDestroy(builder);
    OH_Drawing_FontDestroy(font);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::NativeDrawingTextBlobTest001(data, size);
    OHOS::Rosen::Drawing::NativeDrawingTextBlobTest002(data, size);
    return 0;
}
