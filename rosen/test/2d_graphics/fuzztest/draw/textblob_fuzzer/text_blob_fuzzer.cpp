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
constexpr size_t TEXTUTF8_SIZE = 128;
constexpr size_t TEXTCONTRAST_SIZE = 3;
} // namespace
namespace Drawing {
/*
 * 测试以下 TextBlob 接口：
 * 1. MakeFromText(const char* text, int length, const Font& font, TextEncoding encoding)
 * 2. Bounds()
 * 3. Serialize(Data* data)
 * 4. Deserialize(Data* data, size_t size, void* ctx)
 * 5. GetDrawingGlyphIDforTextBlob(const TextBlob* blob, std::vector<uint16_t>& glyphIds)
 * 6. GetDrawingPathforTextBlob(uint16_t someParam, const TextBlob* blob)
 */
bool TextBlobFuzzTest001(const uint8_t* data, size_t size)
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
    Font font;
    auto textblob = TextBlob::MakeFromText(text, count - 1, font, TextEncoding::UTF8);
    if (textblob) {
        textblob->Bounds();
        textblob->Serialize(nullptr);
        textblob->Deserialize(nullptr, 0, nullptr);

        std::vector<uint16_t> glyphIds = {};
        TextBlob::GetDrawingGlyphIDforTextBlob(textblob.get(), glyphIds);
        TextBlob::GetDrawingPathforTextBlob(GetObject<uint16_t>() % MAX_SIZE, textblob.get());
    }
    if (text != nullptr) {
        delete [] text;
        text = nullptr;
    }
    return true;
}

/*
 * 测试以下 TextBlob 接口：
 * 1. MakeFromPosText(const char* text, int length, const Point* points, const Font& font, TextEncoding encoding)
 * 2. GetDrawingPointsForTextBlob(const TextBlob* blob, std::vector<Point>& points)
 * 3. UniqueID()
 */
bool TextBlobFuzzTest002(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    uint32_t count = GetObject<uint32_t>() % MAX_SIZE + 2;
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
    auto textBlob = TextBlob::MakeFromPosText(text, count - 1, points, font, TextEncoding::UTF8);
    std::vector<Point> pointsVector;
    Point ptOne { GetObject<scalar>(), GetObject<scalar>() };
    Point ptTwo { GetObject<scalar>(), GetObject<scalar>() };
    pointsVector.push_back(ptOne);
    pointsVector.push_back(ptTwo);
    if (textBlob) {
        TextBlob::GetDrawingPointsForTextBlob(textBlob.get(), pointsVector);
        textBlob->UniqueID();
    }
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

/*
 * 测试以下 TextBlob 接口：
 * 1. MakeFromString(const char* text, const Font& font, TextEncoding encoding)
 */
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
        text[i] = GetObject<char>() % TEXTUTF8_SIZE; // Skia问题，非Drawing。 TextEncoding::UTF8 text 传>128会导致崩溃
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

/*
 * 测试以下 TextBlob 接口：
 * 1. Context(std::shared_ptr<Typeface> typeface, bool isCustomTypeface)
 * 2. SetTypeface(std::shared_ptr<Typeface> typeface)
 * 3. GetTypeface()
 * 4. SetIsCustomTypeface(bool isCustomTypeface)
 * 5. IsCustomTypeface()
 */
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
    std::shared_ptr<Typeface> typeface = Typeface::MakeFromFile(path, length - 1);
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

/*
 * 测试以下 TextBlob 接口：
 * 1. MakeFromRSXform(const char* text, int length, const RSXform* xforms, const Font& font, TextEncoding encoding)
 * 2. GetIntercepts(float* bounds, float* intervals, const Paint* paint)
 */
bool TextBlobFuzzTest005(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t count = GetObject<uint32_t>() % MAX_SIZE + 2;
    char* text = new char[count];
    for (size_t i = 0; i < count; i++) {
        text[i] = GetObject<char>() % TEXTUTF8_SIZE; // Skia问题，非Drawing。TextEncoding::UTF8 text 传>128会导致崩溃
    }
    text[count - 1] = '\0';
    uint32_t length = (count - 1) * sizeof(RSXform);
    RSXform xform[length];
    for (size_t i = 0; i < length; i++) {
        xform[i] = RSXform::Make(GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>());
    }
    Font font;
    scalar fSize = GetObject<scalar>();
    font.SetSize(fSize);
    auto textBlob = TextBlob::MakeFromRSXform(text, count - 1, xform, font, TextEncoding::UTF8);
    uint32_t countT = GetObject<uint32_t>() % MAX_SIZE + 1;
    float* bounds = new float[countT];
    float* intervals = new float[countT];
    for (size_t i = 0; i < countT; i++) {
        bounds[i] = GetObject<float>();
        intervals[i] = GetObject<float>();
    }
    Color color = Color(GetObject<uint32_t>(), GetObject<uint32_t>(), GetObject<uint32_t>(), GetObject<uint32_t>());
    Paint paint = Paint(color);
    if (textBlob) {
        textBlob->GetIntercepts(bounds, intervals, &paint);
    }
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

bool TextBlobFuzzTest006(const uint8_t* data, size_t size)
{
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Font font;
    TextContrast textContrast = static_cast<TextContrast>(GetObject<uint32_t>() % TEXTCONTRAST_SIZE);
    auto textBlob1 = TextBlob::MakeFromString("11", font, TextEncoding::UTF8);
    textBlob1->GetTextContrast();
    ProcessTextConstrast::Instance().SetTextContrast(textContrast);
    auto textBlob2 = TextBlob::MakeFromString("11", font, TextEncoding::UTF8);
    textBlob2->GetTextContrast();
    ProcessTextConstrast::Instance().GetTextContrast();
    textBlob2->SetTextContrast(textContrast);
    textBlob2->GetTextContrast();
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
    OHOS::Rosen::Drawing::TextBlobFuzzTest006(data, size);
    return 0;
}
