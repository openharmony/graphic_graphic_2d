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

#include "fontmgr_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "get_object.h"
#include "text/font_mgr.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t MAX_SIZE = 5000;
constexpr size_t SLANT_SIZE = 3;
} // namespace
namespace Drawing {
bool FontMgrFuzzTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    std::shared_ptr<FontMgr> FontMgr = FontMgr::CreateDefaultFontMgr();
    uint32_t count = GetObject<uint32_t>() % MAX_SIZE + 1;
    char* familyName = new char[count];
    if (familyName == nullptr) {
        return false;
    }
    for (size_t i = 0; i < count; i++) {
        familyName[i] =  GetObject<char>();
    }
    familyName[count - 1] = '\0';
    std::string str(familyName);
    int index = GetObject<int>();
    FontMgr->GetFamilyName(index, str);
    if (familyName != nullptr) {
        delete [] familyName;
        familyName = nullptr;
    }
    return true;
}

bool FontMgrFuzzTest002(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<FontMgr> fontMgr = FontMgr::CreateDefaultFontMgr();
    int index = GetObject<int>() % MAX_SIZE;
    fontMgr->CreateStyleSet(index);
    return true;
}

bool FontMgrFuzzTest003(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<FontMgr> fontMgr = FontMgr::CreateDynamicFontMgr();
    uint32_t count = GetObject<uint32_t>() % MAX_SIZE + 1;
    char* familyName = new char[count];
    if (familyName == nullptr) {
        return false;
    }
    for (size_t i = 0; i < count; i++) {
        familyName[i] =  GetObject<char>();
    }
    familyName[count - 1] = '\0';
    std::string str(familyName);
    size_t dataLength = GetObject<size_t>() % MAX_SIZE + 1;
    uint8_t* dataText = new uint8_t[dataLength];
    if (dataText == nullptr) {
        if (familyName != nullptr) {
            delete [] familyName;
            familyName = nullptr;
        }
        return false;
    }
    for (size_t i = 0; i < dataLength; i++) {
        dataText[i] =  GetObject<uint8_t>();
    }
    fontMgr->LoadDynamicFont(str, dataText, dataLength);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    if (familyName != nullptr) {
        delete [] familyName;
        familyName = nullptr;
    }
    return true;
}

bool FontMgrFuzzTest004(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<FontMgr> fontMgr = FontMgr::CreateDynamicFontMgr();
    uint32_t count = GetObject<uint32_t>() % MAX_SIZE + 1;
    char* familyName = new char[count];
    if (familyName == nullptr) {
        return false;
    }
    for (size_t i = 0; i < count; i++) {
        familyName[i] =  GetObject<char>();
    }
    familyName[count - 1] = '\0';
    std::string str(familyName);
    size_t dataLength = GetObject<size_t>() % MAX_SIZE + 1;
    uint8_t* dataText = new uint8_t[dataLength];
    if (dataText == nullptr) {
        if (familyName != nullptr) {
            delete [] familyName;
            familyName = nullptr;
        }
        return false;
    }
    for (size_t i = 0; i < dataLength; i++) {
        dataText[i] =  GetObject<uint8_t>();
    }
    fontMgr->LoadDynamicFont(str, dataText, dataLength);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    if (familyName != nullptr) {
        delete [] familyName;
        familyName = nullptr;
    }
    return true;
}

bool FontMgrFuzzTest005(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<FontMgr> fontMgr = FontMgr::CreateDynamicFontMgr();
    uint32_t count = GetObject<uint32_t>() % MAX_SIZE + 1;
    char* familyName = new char[count];
    if (familyName == nullptr) {
        return false;
    }
    for (size_t i = 0; i < count; i++) {
        familyName[i] =  GetObject<char>();
    }
    familyName[count - 1] = '\0';
    std::string str(familyName);
    uint32_t countT = GetObject<uint32_t>() % MAX_SIZE + 1;
    char* themeName = new char[countT];
    if (themeName == nullptr) {
        if (familyName != nullptr) {
            delete [] familyName;
            familyName = nullptr;
        }
        return false;
    }
    for (size_t i = 0; i < countT; i++) {
        themeName[i] =  GetObject<char>();
    }
    themeName[countT - 1] = '\0';
    std::string strTheme(themeName);
    size_t dataLength = GetObject<size_t>() % MAX_SIZE + 1;
    uint8_t* dataText = new uint8_t[dataLength];
    if (dataText == nullptr) {
        if (themeName != nullptr) {
            delete [] themeName;
            themeName = nullptr;
        }
        if (familyName != nullptr) {
            delete [] familyName;
            familyName = nullptr;
        }
        return false;
    }
    for (size_t i = 0; i < dataLength; i++) {
        dataText[i] =  GetObject<uint8_t>();
    }
    fontMgr->LoadThemeFont(str, strTheme, dataText, dataLength);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    if (themeName != nullptr) {
        delete [] themeName;
        themeName = nullptr;
    }
    if (familyName != nullptr) {
        delete [] familyName;
        familyName = nullptr;
    }
    return true;
}

bool FontMgrFuzzTest006(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<FontMgr> fontMgr = FontMgr::CreateDynamicFontMgr();
    uint32_t count = GetObject<uint32_t>() % MAX_SIZE + 1;
    char* familyName = new char[count];
    if (familyName == nullptr) {
        return false;
    }
    for (size_t i = 0; i < count; i++) {
        familyName[i] =  GetObject<char>();
    }
    familyName[count - 1] = '\0';
    int weight = GetObject<int>() % MAX_SIZE;
    int width = GetObject<int>() % MAX_SIZE;
    uint32_t slant = GetObject<uint32_t>();
    FontStyle fontStyle = FontStyle(weight, width, static_cast<FontStyle::Slant>(slant % SLANT_SIZE));
    size_t bcp47Count = GetObject<size_t>() % MAX_SIZE;
    const char* bcp47[] = {"zh-Hans", "zh-CN"};
    int32_t character = GetObject<int32_t>();
    fontMgr->MatchFamilyStyleCharacter(familyName, fontStyle, bcp47, bcp47Count, character);
    if (familyName != nullptr) {
        delete [] familyName;
        familyName = nullptr;
    }
    return true;
}

bool FontMgrFuzzTest007(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<FontMgr> fontMgr = FontMgr::CreateDynamicFontMgr();
    uint32_t count = GetObject<uint32_t>() % MAX_SIZE + 1;
    char* familyName = new char[count];
    if (familyName == nullptr) {
        return false;
    }
    for (size_t i = 0; i < count; i++) {
        familyName[i] =  GetObject<char>();
    }
    familyName[count - 1] = '\0';
    fontMgr->MatchFamily(familyName);
    if (familyName != nullptr) {
        delete [] familyName;
        familyName = nullptr;
    }
    return true;
}

bool FontMgrFuzzTest008(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<FontMgr> fontMgr = FontMgr::CreateDynamicFontMgr();
    uint32_t count = GetObject<uint32_t>() % MAX_SIZE + 1;
    char* familyName = new char[count];
    if (familyName == nullptr) {
        return false;
    }
    for (size_t i = 0; i < count; i++) {
        familyName[i] =  GetObject<char>();
    }
    familyName[count - 1] = '\0';
    int weight = GetObject<int>() % MAX_SIZE;
    int width = GetObject<int>() % MAX_SIZE;
    uint32_t slant = GetObject<uint32_t>();
    FontStyle fontStyle = FontStyle(weight, width, static_cast<FontStyle::Slant>(slant % SLANT_SIZE));
    fontMgr->MatchFamilyStyle(familyName, fontStyle);
    fontMgr->CountFamilies();
    if (familyName != nullptr) {
        delete [] familyName;
        familyName = nullptr;
    }
    return true;
}

bool FontMgrFuzzTest009(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<FontMgr> fontMgr = FontMgr::CreateDefaultFontMgr();
    int fd = GetObject<int>() % MAX_SIZE;
    std::vector<FontByteArray> fullnameVec;
    fontMgr->GetFontFullName(fd, fullnameVec);
    return true;
}

bool FontMgrFuzzTest010(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

   // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<FontMgr> fontMgr = FontMgr::CreateDynamicFontMgr();
    std::vector<std::string> fontPathVec;
    fontMgr->ParseInstallFontConfig("strPath", fontPathVec);
    return true;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::FontMgrFuzzTest001(data, size);
    OHOS::Rosen::Drawing::FontMgrFuzzTest002(data, size);
    OHOS::Rosen::Drawing::FontMgrFuzzTest003(data, size);
    OHOS::Rosen::Drawing::FontMgrFuzzTest004(data, size);
    OHOS::Rosen::Drawing::FontMgrFuzzTest005(data, size);
    OHOS::Rosen::Drawing::FontMgrFuzzTest006(data, size);
    OHOS::Rosen::Drawing::FontMgrFuzzTest007(data, size);
    OHOS::Rosen::Drawing::FontMgrFuzzTest008(data, size);
    OHOS::Rosen::Drawing::FontMgrFuzzTest009(data, size);
    OHOS::Rosen::Drawing::FontMgrFuzzTest010(data, size);
    return 0;
}