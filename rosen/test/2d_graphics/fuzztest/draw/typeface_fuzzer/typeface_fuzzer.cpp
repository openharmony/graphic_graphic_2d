/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "typeface_fuzzer.h"
#include <cstddef>
#include <cstdint>
#include "get_object.h"
#include "text/font_arguments.h"
#include "text/typeface.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t MAX_SIZE = 5000;
constexpr size_t SLANT_SIZE = 3;
} // namespace
namespace Drawing {
/*
 * 测试以下 Typeface 接口：
 * 1. MakeDefault()
 * 2. GetFamilyName()
 * 3. GetFontStyle()
 * 4. GetTableSize(uint32_t tag)
 * 5. GetTableData(uint32_t tag, size_t offset, size_t length, void* data)
 * 6. GetItalic()
 * 7. GetUniqueID()
 * 8. GetUnitsPerEm()
 * 9. IsCustomTypeface()
 * 10. IsThemeTypeface()
 */
bool TypefaceFuzzTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<Typeface> typeface = Typeface::MakeDefault();
    typeface->GetFamilyName();
    typeface->GetFontStyle();
    uint32_t tag = GetObject<uint32_t>();
    typeface->GetTableSize(tag);
    uint32_t tag2 = GetObject<uint32_t>();
    size_t offset = GetObject<size_t>();
    size_t length = GetObject<size_t>();
    typeface->GetTableData(tag2, offset, length, nullptr);
    typeface->GetItalic();
    typeface->GetUniqueID();
    typeface->GetUnitsPerEm();
    typeface->IsCustomTypeface();
    typeface->IsThemeTypeface();
    return true;
}

/*
 * 测试以下 Typeface 接口：
 * 1. MakeFromFile(const char* path, size_t length)
 */
bool TypefaceFuzzTest002(const uint8_t* data, size_t size)
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
    if (path != nullptr) {
        delete [] path;
        path = nullptr;
    }
    return true;
}

/*
 * 测试以下 Typeface 接口：
 * 1. MakeFromStream(std::unique_ptr<MemoryStream> memoryStream)
 * 2. MakeFromStream(std::unique_ptr<MemoryStream> memoryStream,
 *       const FontArguments& fontArguments);
 */
bool TypefaceFuzzTest003(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    size_t index = GetObject<size_t>() % MAX_SIZE + 1;
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* fontData = new char[length];
    for (size_t i = 0; i < length; i++) {
        fontData[i] = GetObject<char>();
    }
    fontData[length - 1] = '\0';
    bool copyData = GetObject<bool>();
    std::unique_ptr<MemoryStream> memoryStream = std::make_unique<MemoryStream>(
        reinterpret_cast<const void*>(fontData), length, copyData);
    std::shared_ptr<Typeface> typeface = Typeface::MakeFromStream(std::move(memoryStream));

    FontArguments args;
    args.SetCollectionIndex(index);
    std::shared_ptr<Typeface> typeface2 = Typeface::MakeFromStream(std::move(memoryStream), args);
    std::shared_ptr<Typeface> typeface3 = Typeface::MakeFromStream(nullptr, args);
    if (fontData != nullptr) {
        delete [] fontData;
        fontData = nullptr;
    }
    return true;
}

/*
 * 测试以下 Typeface 接口：
 * 1. MakeDefault()
 * 2. GetFamilyName()
 * 3. GetFontStyle()
 * 4. GetTableSize(uint32_t tag)
 * 5. GetTableData(uint32_t tag, size_t offset, size_t length, void* data)
 * 6. GetItalic()
 * 7. GetUniqueID()
 * 8. GetUnitsPerEm()
 * 9. IsCustomTypeface()
 * 10. IsThemeTypeface()
 * 11. MakeClone(const FontArguments& arg)
 */
bool TypefaceFuzzTest004(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<Typeface> typeface = Typeface::MakeDefault();
    FontArguments arg;
    int fontCollectionIndex = GetObject<int>();
    arg.SetCollectionIndex(fontCollectionIndex);
    size_t coordinateCount = GetObject<size_t>() % MAX_SIZE + 1;
    FontArguments::VariationPosition::Coordinate* coordinates = new
        FontArguments::VariationPosition::Coordinate[coordinateCount];
    for (size_t i = 0; i < coordinateCount; i++) {
        coordinates[i] = {GetObject<uint32_t>(), GetObject<float>()};
    }
    FontArguments::VariationPosition variationPosition = { 0 };
    variationPosition.coordinates = coordinates;
    arg.SetVariationDesignPosition(variationPosition);
    int index = GetObject<int>();
    size_t overrideCount = GetObject<size_t>() % MAX_SIZE + 1;
    FontArguments::Palette::Override* overrides = new FontArguments::Palette::Override[overrideCount];
    for (size_t i = 0; i < overrideCount; i++) {
        overrides[i] = {GetObject<int>(), GetObject<uint32_t>()};
    }
    FontArguments::Palette palette = {index, overrides, overrideCount};
    arg.SetPalette(palette);
    typeface->MakeClone(arg);
    if (coordinates != nullptr) {
        delete [] coordinates;
        coordinates = nullptr;
    }
    if (overrides != nullptr) {
        delete [] overrides;
        overrides = nullptr;
    }
    return true;
}

/*
 * 测试以下 Typeface 接口：
 * 1. MakeFromName(const char* familyName, const FontStyle& style)
 */
bool TypefaceFuzzTest005(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t count = GetObject<uint32_t>() % MAX_SIZE + 1;
    char* familyName = new char[count];
    for (size_t i = 0; i < count; i++) {
        familyName[i] =  GetObject<char>();
    }
    familyName[count - 1] = '\0';
    int weight = GetObject<int>();
    int width = GetObject<int>();
    uint32_t slant = GetObject<uint32_t>();
    FontStyle fontStyle = FontStyle(weight, width, static_cast<FontStyle::Slant>(slant % SLANT_SIZE));
    std::shared_ptr<Typeface> typeface = Typeface::MakeFromName(familyName, fontStyle);
    if (familyName != nullptr) {
        delete [] familyName;
        familyName = nullptr;
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
    OHOS::Rosen::Drawing::TypefaceFuzzTest001(data, size);
    OHOS::Rosen::Drawing::TypefaceFuzzTest002(data, size);
    OHOS::Rosen::Drawing::TypefaceFuzzTest003(data, size);
    OHOS::Rosen::Drawing::TypefaceFuzzTest004(data, size);
    OHOS::Rosen::Drawing::TypefaceFuzzTest005(data, size);
    return 0;
}
