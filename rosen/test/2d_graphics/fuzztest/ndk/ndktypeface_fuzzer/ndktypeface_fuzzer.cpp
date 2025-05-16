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

#include "ndktypeface_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "get_object.h"

#include "drawing_memory_stream.h"
#include "drawing_typeface.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t MAX_ARRAY_SIZE = 5000;
} // namespace

namespace Drawing {

void NativeDrawingTypefaceTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    int index = GetObject<int>();
    uint32_t path_size = GetObject<uint32_t>() % MAX_ARRAY_SIZE + 1;
    char* path = new char[path_size];
    for (size_t i = 0; i < path_size; i++) {
        path[i] = GetObject<char>();
    }
    path[path_size - 1] = '\0';
    OH_Drawing_Typeface* typeface = OH_Drawing_TypefaceCreateDefault();
    OH_Drawing_TypefaceCreateFromFile(nullptr, 0);
    OH_Drawing_Typeface* typefaceOne = OH_Drawing_TypefaceCreateFromFile(path, index);

    char pathTwo[] = "system/fonts/NotoSansBengaliUI-Bold.ttf";
    OH_Drawing_Typeface* typefaceTwo = OH_Drawing_TypefaceCreateFromFile(pathTwo, index);

    bool copyData = GetObject<bool>();
    uint32_t str_size = GetObject<uint32_t>() % MAX_ARRAY_SIZE;
    char* str = new char[str_size];
    for (size_t i = 0; i < str_size; i++) {
        str[i] = GetObject<char>();
    }
    int32_t streamIndex = GetObject<int32_t>() % MAX_ARRAY_SIZE;
    OH_Drawing_MemoryStream* memoryStream = OH_Drawing_MemoryStreamCreate(str, str_size, copyData);
    OH_Drawing_TypefaceCreateFromStream(nullptr, streamIndex);
    if (path != nullptr) {
        delete [] path;
        path = nullptr;
    }
    if (str != nullptr) {
        delete [] str;
        str = nullptr;
    }

    OH_Drawing_TypefaceDestroy(typeface);
    OH_Drawing_TypefaceDestroy(typefaceOne);
    OH_Drawing_TypefaceDestroy(typefaceTwo);
    OH_Drawing_MemoryStreamDestroy(memoryStream);
}

void NativeDrawingTypefaceTest002(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    bool copyData = GetObject<bool>();
    uint32_t str_size = GetObject<uint32_t>() % MAX_ARRAY_SIZE;
    char* str = new char[str_size];
    for (size_t i = 0; i < str_size; i++) {
        str[i] = GetObject<char>();
    }
    int32_t streamIndex = GetObject<int32_t>() % MAX_ARRAY_SIZE;
    OH_Drawing_MemoryStream* memoryStream = OH_Drawing_MemoryStreamCreate(str, str_size, copyData);
    OH_Drawing_Typeface* typeface = OH_Drawing_TypefaceCreateFromStream(memoryStream, streamIndex);

    if (str != nullptr) {
        delete [] str;
        str = nullptr;
    }
    OH_Drawing_TypefaceDestroy(typeface);
}

void NativeDrawingTypefaceTest003(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t str_size = GetObject<uint32_t>() % MAX_ARRAY_SIZE;
    char* axis = new char[str_size];
    for (size_t i = 0; i < str_size; i++) {
        axis[i] = GetObject<char>();
    }
    float value = GetObject<float>();
    OH_Drawing_FontArguments* fontArguments = OH_Drawing_FontArgumentsCreate();
    OH_Drawing_FontArgumentsAddVariation(fontArguments, axis, value);

    str_size = GetObject<uint32_t>() % MAX_ARRAY_SIZE;
    char* path = new char[str_size];
    for (size_t i = 0; i < str_size; i++) {
        path[i] = GetObject<char>();
    }
    OH_Drawing_Typeface* typeFace = OH_Drawing_TypefaceCreateFromFileWithArguments(path, fontArguments);
    OH_Drawing_Typeface* typeFace2 = OH_Drawing_TypefaceCreateFromCurrent(typeFace, fontArguments);

    if (axis != nullptr) {
        delete [] axis;
        axis = nullptr;
    }
    if (path != nullptr) {
        delete [] path;
        path = nullptr;
    }
    OH_Drawing_FontArgumentsDestroy(fontArguments);
    OH_Drawing_TypefaceDestroy(typeFace);
    OH_Drawing_TypefaceDestroy(typeFace2);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::NativeDrawingTypefaceTest001(data, size);
    OHOS::Rosen::Drawing::NativeDrawingTypefaceTest002(data, size);
    OHOS::Rosen::Drawing::NativeDrawingTypefaceTest003(data, size);
    return 0;
}
