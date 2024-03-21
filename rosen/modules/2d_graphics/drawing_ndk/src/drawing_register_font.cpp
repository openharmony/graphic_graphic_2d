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

#ifdef BUILD_NON_SDK_VER
#include <filesystem>
#endif
#include <fstream>
#include "drawing_register_font.h"
#ifndef USE_GRAPHIC_TEXT_GINE
#include "rosen_text/ui/font_collection.h"
#else
#include "rosen_text/font_collection.h"
#endif

#ifndef USE_GRAPHIC_TEXT_GINE
using namespace rosen;
#else
using namespace OHOS::Rosen;
#endif

#define SUCCESSED 0
#define ERROR_FILE_NOT_EXISTS 1
#define ERROR_OPEN_FILE_FAILED 2
#define ERROR_READ_FILE_FAILED 3
#define ERROR_SEEK_FAILED 4
#define ERROR_GET_SIZE_FAILED 5
#define ERROR_NULL_FONT_BUFFER 6
#define ERROR_BUFFER_SIZE_ZERO 7
#define ERROR_NULL_FONT_COLLECTION 8

#ifdef BUILD_NON_SDK_VER
static bool StdFilesystemExists(const std::string &p, std::error_code &ec)
{
    return std::filesystem::exists(p, ec);
}
#else
static bool StdFilesystemExists(const std::string &p)
{
    std::ifstream f(p.c_str());
    return f.good();
}
#endif

template<typename T1, typename T2>
inline T1* ConvertToOriginalText(T2* ptr)
{
    return reinterpret_cast<T1*>(ptr);
}

static uint32_t LoadFromFontCollection(OH_Drawing_FontCollection* fontCollection,
    const std::string &familyName, const uint8_t* data, size_t dataLength)
{
    if (fontCollection == nullptr) {
        return ERROR_NULL_FONT_COLLECTION;
    }
    auto fc = ConvertToOriginalText<FontCollection>(fontCollection);
#ifndef USE_GRAPHIC_TEXT_GINE
    fc->LoadFontFromList(data, dataLength, familyName);
#else
    fc->LoadFont(familyName, data, dataLength);
#endif
    return 0;
}

uint32_t OH_Drawing_RegisterFont(OH_Drawing_FontCollection* fontCollection, const char* fontFamily,
    const char* familySrc)
{
    const std::string path = familySrc;
#ifdef BUILD_NON_SDK_VER
    std::error_code ec;
    auto ret = StdFilesystemExists(path, ec);
    if (ec) {
        return ERROR_OPEN_FILE_FAILED;
    }
#else
    auto ret = StdFilesystemExists(path);
#endif
    if (!ret) {
        return ERROR_FILE_NOT_EXISTS;
    }

    std::ifstream ifs(path, std::ios_base::in);
    if (!ifs.is_open()) {
        return ERROR_OPEN_FILE_FAILED;
    }

    ifs.seekg(0, ifs.end);
    if (!ifs.good()) {
        ifs.close();
        return ERROR_SEEK_FAILED;
    }

    auto size = ifs.tellg();
    if (ifs.fail()) {
        ifs.close();
        return ERROR_GET_SIZE_FAILED;
    }

    ifs.seekg(ifs.beg);
    if (!ifs.good()) {
        ifs.close();
        return ERROR_SEEK_FAILED;
    }

    std::unique_ptr<char[]> buffer = std::make_unique<char[]>(size);
    ifs.read(buffer.get(), size);
    if (!ifs.good()) {
        ifs.close();
        return ERROR_READ_FILE_FAILED;
    }

    ifs.close();
    const std::string familyName = fontFamily;
    const uint8_t* data = reinterpret_cast<uint8_t*>(buffer.get());
    return LoadFromFontCollection(fontCollection, familyName, data, size);
}

uint32_t OH_Drawing_RegisterFontBuffer(OH_Drawing_FontCollection* fontCollection, const char* fontFamily,
    uint8_t* fontBuffer, size_t length)
{
    if (fontBuffer == nullptr) {
        return ERROR_NULL_FONT_BUFFER;
    }

    if (length == 0) {
        return ERROR_BUFFER_SIZE_ZERO;
    }

    const std::string familyName = fontFamily;
    return LoadFromFontCollection(fontCollection, familyName, fontBuffer, length);
}