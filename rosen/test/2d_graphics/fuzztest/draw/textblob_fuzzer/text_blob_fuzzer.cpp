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

#include "text_blob_fuzzer.h"
#include <cstddef>
#include <cstdint>
#include "get_object.h"
#include "text/text_blob.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t ENCODING_SIZE = 4;
constexpr size_t MAX_SIZE = 5000;
constexpr size_t TEXT_SIZE = 130;
constexpr size_t TEXTUTF8_SIZE = 128;
} // namespace
namespace Drawing {

bool TextBlobFuzzTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    const char* str = "as";
    Font font;
    auto textblob = TextBlob::MakeFromText(str, strlen(str), font, TextEncoding::UTF8);
    textblob->Bounds();
    textblob->Serialize(nullptr);
    textblob->Deserialize(nullptr, 0, nullptr);

    std::vector<uint16_t> glyphIds = {};
    TextBlob::GetDrawingGlyphIDforTextBlob(textblob.get(), glyphIds);
    TextBlob::GetDrawingPathforTextBlob(0, textblob.get());

    return true;
}

bool TextBlobFuzzTest002(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    uint32_t count = GetObject<uint32_t>() % MAX_SIZE + 1;
    char* text = new char[count];
    for (size_t i = 0; i < count - 1; i++) {
        text[i] = GetObject<char>() % TEXTUTF8_SIZE; // Skia问题，非Drawing。 TextEncoding::UTF8 text 传>128会导致崩溃
    }
    text[count - 1] = '\0';
    Point* points = new Point[count - 1];
    for (size_t i = 0; i < count - 1; i++) {
        points[i] = { GetObject<scalar>(), GetObject<scalar>() };
    }
    Font font;
    scalar fSize = GetObject<scalar>();
    font.SetSize(fSize);
    auto textBlob = TextBlob::MakeFromPosText(text, count, points, font, TextEncoding::UTF8);
    std::vector<Point> pointsVector;
    Point ptOne { GetObject<scalar>(), GetObject<scalar>() };
    Point ptTwo { GetObject<scalar>(), GetObject<scalar>() };
    pointsVector.push_back(ptOne);
    pointsVector.push_back(ptTwo);
    TextBlob::GetDrawingPointsForTextBlob(textBlob.get(), pointsVector);
    textBlob->UniqueID();
    if (text != nullptr) {
        delete [] text;
        text = nullptr;
    }
    if (points != nullptr) {
        delete [] points;
        points = nullptr;
    }
    return true;
}

bool TextBlobFuzzTest003(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t count = GetObject<uint32_t>() % MAX_SIZE + 1;
    char* text = new char[count];
    for (size_t i = 0; i < count; i++) {
        text[i] = GetObject<char>();
    }
    text[count - 1] = '\0';
    Font font;
    scalar fSize = GetObject<scalar>();
    font.SetSize(fSize);
    uint32_t encoding = GetObject<uint32_t>();
    auto textBlob = TextBlob::MakeFromString(text, font, static_cast<TextEncoding>(encoding % ENCODING_SIZE));
    if (text != nullptr) {
        delete [] text;
        text = nullptr;
    }
    return true;
}

bool TextBlobFuzzTest004(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* path = new char[length];
    for (size_t i = 0; i < length; i++) {
        path[i] = GetObject<char>();
    }
    path[length - 1] = '\0';
    std::shared_ptr<Typeface> typeface = Typeface::MakeFromFile(path, length);
    bool isCustomTypeface = GetObject<bool>();
    TextBlob::Context context = TextBlob::Context(typeface, isCustomTypeface);
    context.SetTypeface(typeface);
    context.GetTypeface();
    context.SetIsCustomTypeface(isCustomTypeface);
    context.IsCustomTypeface();
    if (path != nullptr) {
        delete [] path;
        path = nullptr;
    }
    return true;
}

bool TextBlobFuzzTest005(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    scalar cos = GetObject<scalar>();
    scalar sin = GetObject<scalar>();
    scalar tx = GetObject<scalar>();
    scalar ty = GetObject<scalar>();
    RSXform xform[] = { RSXform::Make(cos, sin, tx, ty), RSXform::Make(cos, sin, tx, ty),
        RSXform::Make(cos, sin, tx, ty) };
    uint32_t count = GetObject<uint32_t>() % TEXT_SIZE + 1;
    char* text = new char[count];
    for (size_t i = 0; i < count - 1; i++) {
        text[i] = GetObject<char>() % TEXTUTF8_SIZE; // Skia问题，非Drawing。TextEncoding::UTF8 text 传>128会导致崩溃
    }
    text[count - 1] = '\0';
    Font font;
    scalar fSize = GetObject<scalar>();
    font.SetSize(fSize);
    auto textBlob = TextBlob::MakeFromRSXform(text, count, xform, font, TextEncoding::UTF8);
    uint32_t countT = GetObject<uint32_t>() % MAX_SIZE + 1;
    float* bounds = new float[countT];
    float* intervals = new float[countT];
    for (size_t i = 0; i < countT; i++) {
        bounds[i] = GetObject<float>();
        intervals[i] = GetObject<float>();
    }
    uint32_t alpha = GetObject<uint32_t>();
    uint32_t red = GetObject<uint32_t>();
    uint32_t blue = GetObject<uint32_t>();
    uint32_t green = GetObject<uint32_t>();
    Color color = Color(red, green, blue, alpha);
    Paint paint = Paint(color);
    textBlob->GetIntercepts(bounds, intervals, &paint);
    if (text != nullptr) {
        delete [] text;
        text = nullptr;
    }
    if (bounds != nullptr) {
        delete [] bounds;
        bounds = nullptr;
    }
    if (intervals != nullptr) {
        delete [] intervals;
        intervals = nullptr;
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
    OHOS::Rosen::Drawing::TextBlobFuzzTest001(data, size);
    OHOS::Rosen::Drawing::TextBlobFuzzTest002(data, size);
    OHOS::Rosen::Drawing::TextBlobFuzzTest003(data, size);
    OHOS::Rosen::Drawing::TextBlobFuzzTest004(data, size);
    OHOS::Rosen::Drawing::TextBlobFuzzTest005(data, size);
    return 0;
}
