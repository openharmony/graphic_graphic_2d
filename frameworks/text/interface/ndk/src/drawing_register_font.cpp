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
#include <cstdlib>
#include <fstream>

#include "drawing_register_font.h"
#include "rosen_text/font_collection.h"
#include "txt/platform.h"

#include "text/font_mgr.h"
#include "text/typeface.h"
#include "utils/log.h"

using namespace OHOS::Rosen;

#define SUCCESSED 0
#define ERROR_FILE_NOT_EXISTS 1
#define ERROR_OPEN_FILE_FAILED 2
#define ERROR_READ_FILE_FAILED 3
#define ERROR_SEEK_FAILED 4
#define ERROR_GET_SIZE_FAILED 5
#define ERROR_NULL_FONT_BUFFER 6
#define ERROR_BUFFER_SIZE_ZERO 7
#define ERROR_NULL_FONT_COLLECTION 8
#define ERROR_FILE_CORRUPTION 9

#ifdef BUILD_NON_SDK_VER
static bool StdFilesystemExists(const std::string& p, std::error_code& ec)
{
    return std::filesystem::exists(p, ec);
}
#else
static bool StdFilesystemExists(const std::string& p)
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
    const std::string& familyName, const uint8_t* data, size_t dataLength)
{
    if (fontCollection == nullptr) {
        return ERROR_NULL_FONT_COLLECTION;
    }
    if ((data == nullptr) != (dataLength == 0)) {
        return ERROR_BUFFER_SIZE_ZERO;
    }
    auto fc = ConvertToOriginalText<FontCollection>(fontCollection);
    auto face = fc->LoadFont(familyName, data, dataLength);
    if (face == nullptr) {
        return ERROR_FILE_CORRUPTION;
    }
    return 0;
}

uint32_t LoadFontDataFromFile(const std::string& path, std::unique_ptr<char[]>& buffer, std::streamsize& size)
{
#ifdef BUILD_NON_SDK_VER
    std::error_code ec;
    bool ret = StdFilesystemExists(path, ec);
    if (ec) {
        return ERROR_OPEN_FILE_FAILED;
    }
#else
    bool ret = StdFilesystemExists(path);
#endif
    char tmpPath[PATH_MAX] = { 0 };
    if (ret == false || realpath(path.c_str(), tmpPath) == nullptr) {
        return ERROR_FILE_NOT_EXISTS;
    }
    std::ifstream ifs(tmpPath, std::ios_base::in);
    if (!ifs.is_open()) {
        return ERROR_OPEN_FILE_FAILED;
    }
    ifs.seekg(0, ifs.end);
    if (!ifs.good()) {
        ifs.close();
        return ERROR_SEEK_FAILED;
    }
    size = ifs.tellg();
    if (ifs.fail()) {
        ifs.close();
        return ERROR_GET_SIZE_FAILED;
    }
    if (size <= 0) {
        ifs.close();
        return ERROR_BUFFER_SIZE_ZERO;
    }
    ifs.seekg(ifs.beg);
    if (!ifs.good()) {
        ifs.close();
        return ERROR_SEEK_FAILED;
    }
    buffer = std::make_unique<char[]>(size);
    ifs.read(buffer.get(), size);
    if (!ifs.good()) {
        ifs.close();
        return ERROR_READ_FILE_FAILED;
    }
    ifs.close();
    return 0;
}

uint32_t OH_Drawing_RegisterFont(
    OH_Drawing_FontCollection* fontCollection, const char* fontFamily, const char* familySrc)
{
    if (fontCollection == nullptr || familySrc == nullptr) {
        return ERROR_NULL_FONT_COLLECTION;
    }
    const std::string path = familySrc;
    std::unique_ptr<char[]> buffer;
    std::streamsize size = 0;
    uint32_t result = LoadFontDataFromFile(path, buffer, size);
    if (result != 0) {
        return result;
    }
    const std::string familyName = fontFamily;
    const uint8_t* data = reinterpret_cast<uint8_t*>(buffer.get());
    return LoadFromFontCollection(fontCollection, familyName, data, size);
}

uint32_t OH_Drawing_RegisterFontBuffer(OH_Drawing_FontCollection* fontCollection, const char* fontFamily,
    uint8_t* fontBuffer, size_t length)
{
    if (fontCollection == nullptr) {
        return ERROR_NULL_FONT_COLLECTION;
    }
    if (fontBuffer == nullptr) {
        return ERROR_NULL_FONT_BUFFER;
    }

    if (length == 0) {
        return ERROR_BUFFER_SIZE_ZERO;
    }

    const std::string familyName = fontFamily;
    return LoadFromFontCollection(fontCollection, familyName, fontBuffer, length);
}

uint32_t OH_Drawing_UnregisterFont(OH_Drawing_FontCollection* fontCollection, const char* fontFamily)
{
    if (fontCollection == nullptr || fontFamily == nullptr) {
        return ERROR_NULL_FONT_COLLECTION;
    }
    std::string family(fontFamily);
    if (family.empty() || SPText::DefaultFamilyNameMgr::IsThemeFontFamily(family)) {
    }

    auto fc = ConvertToOriginalText<FontCollection>(fontCollection);
    return !fc->UnloadFont(fontFamily);
}