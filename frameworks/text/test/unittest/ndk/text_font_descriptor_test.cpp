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

#include <codecvt>
#include <filesystem>
#include <fstream>

#include "drawing_font_collection.h"
#include "drawing_register_font.h"
#include "drawing_text_font_descriptor.h"
#include "font_descriptor_mgr.h"
#include "gtest/gtest.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace {
namespace fs = std::filesystem;

const std::string STYLISH_FONT_CONFIG_FILE = "/system/fonts/visibility_list.json";
const std::string STYLISH_FONT_CONFIG_PROD_FILE = "/sys_prod/fonts/visibility_list.json";
const std::string INSTALLED_FONT_CONFIG_FILE = "/data/service/el1/public/for-all-app/fonts/install_fontconfig.json";
const std::string INSTALLED_FONT_CONFIG_FILE_BAK =
    "/data/service/el1/public/for-all-app/fonts/install_fontconfig.json.bak";

// "Noto Sans Mono CJK KR" exchange with "Noto Sans Mono CJK JP"
const std::string INSTALL_CONFIG = R"(
{
  "fontlist": [
    {
      "fontfullpath": "/system/fonts/NotoSansCJK-Regular.ttc",
      "fullname": [
        "Noto Sans CJK JP",
        "Noto Sans CJK KR",
        "Noto Sans CJK SC",
        "Noto Sans CJK TC",
        "Noto Sans CJK HK",
        "Noto Sans Mono CJK KR",
        "Noto Sans Mono CJK JP",
        "Noto Sans Mono CJK SC",
        "Noto Sans Mono CJK TC",
        "Noto Sans Mono CJK HK"
      ]
    },
    { "fontfullpath": "/system/fonts/NotoSans[wdth,wght].ttf", "fullname": ["Noto Sans Regular"] }
  ]
}
)";

bool ExistStylishFontConfigFile()
{
    return fs::exists(STYLISH_FONT_CONFIG_FILE) || fs::exists(STYLISH_FONT_CONFIG_PROD_FILE);
}
} // namespace

class NdkFontDescriptorTest : public testing::Test {};

void CreateFile(const std::string& file) 
{
    fs::path filePath(file);
    fs::path dirPath = filePath.parent_path();

    if (!fs::exists(dirPath)) {
        fs::create_directories(dirPath);
    }

    std::ofstream ofs(INSTALLED_FONT_CONFIG_FILE, std::ios::trunc);
    ofs << INSTALL_CONFIG;
}

void InitInstallConfig()
{;
    if (fs::exists(INSTALLED_FONT_CONFIG_FILE)) {
        fs::rename(INSTALLED_FONT_CONFIG_FILE, INSTALLED_FONT_CONFIG_FILE_BAK);
    }
    CreateFile(INSTALLED_FONT_CONFIG_FILE);
}

void DestroyInstallConfig()
{
    if (fs::exists(INSTALLED_FONT_CONFIG_FILE_BAK)) {
        fs::copy_file(INSTALLED_FONT_CONFIG_FILE_BAK, INSTALLED_FONT_CONFIG_FILE, fs::copy_options::overwrite_existing);
        fs::remove(INSTALLED_FONT_CONFIG_FILE_BAK);
    } else {
        fs::remove(INSTALLED_FONT_CONFIG_FILE);
    }
}

/*
 * @tc.name: NdkFontDescriptorTest001
 * @tc.desc: test for the fontDescriptor.
 * @tc.type: FUNC
 */
HWTEST_F(NdkFontDescriptorTest, NdkFontDescriptorTest001, TestSize.Level0)
{
    OH_Drawing_FontDescriptor* descArr = OH_Drawing_MatchFontDescriptors(nullptr, nullptr);
    EXPECT_EQ(descArr, nullptr);
    OH_Drawing_DestroyFontDescriptors(descArr, 0);
}

/*
 * @tc.name: NdkFontDescriptorTest002
 * @tc.desc: test for the fontDescriptor.
 * @tc.type: FUNC
 */
HWTEST_F(NdkFontDescriptorTest, NdkFontDescriptorTest002, TestSize.Level0)
{
    OH_Drawing_FontDescriptor* desc = OH_Drawing_CreateFontDescriptor();
    size_t num = 0;
    OH_Drawing_FontDescriptor* descArr = OH_Drawing_MatchFontDescriptors(desc, &num);
    OH_Drawing_DestroyFontDescriptor(desc);
    EXPECT_NE(descArr, nullptr);
    if (ExistStylishFontConfigFile()) {
        EXPECT_EQ(num, 143);
    } else {
        EXPECT_EQ(num, 142);
    }
    OH_Drawing_DestroyFontDescriptors(descArr, num);
}

/*
 * @tc.name: NdkFontDescriptorTest003
 * @tc.desc: test for the fontDescriptor.
 * @tc.type: FUNC
 */
HWTEST_F(NdkFontDescriptorTest, NdkFontDescriptorTest003, TestSize.Level0)
{
    OH_Drawing_FontDescriptor* desc = OH_Drawing_CreateFontDescriptor();
    desc->weight = -1;
    size_t num = 0;
    OH_Drawing_FontDescriptor* descArr = OH_Drawing_MatchFontDescriptors(desc, &num);
    OH_Drawing_DestroyFontDescriptor(desc);
    EXPECT_EQ(descArr, nullptr);
    EXPECT_EQ(num, 0);
    OH_Drawing_DestroyFontDescriptors(descArr, num);
}

/*
 * @tc.name: NdkFontDescriptorTest004
 * @tc.desc: test for the fontDescriptor.
 * @tc.type: FUNC
 */
HWTEST_F(NdkFontDescriptorTest, NdkFontDescriptorTest004, TestSize.Level0)
{
    OH_Drawing_FontDescriptor* desc = OH_Drawing_CreateFontDescriptor();
    char* fontFamily = strdup("HarmonyOS Sans");
    desc->fontFamily = fontFamily;
    size_t num = 0;
    OH_Drawing_FontDescriptor* descArr = OH_Drawing_MatchFontDescriptors(desc, &num);
    ASSERT_NE(descArr, nullptr);
    EXPECT_LE(1, num);
    EXPECT_STREQ(descArr[0].fontFamily, fontFamily);
    OH_Drawing_DestroyFontDescriptors(descArr, num);
    free(fontFamily);

    fontFamily = strdup("HarmonyOS Sans Condensed");
    desc->fontFamily = fontFamily;
    descArr = OH_Drawing_MatchFontDescriptors(desc, &num);
    ASSERT_NE(descArr, nullptr);
    EXPECT_EQ(num, 1);
    EXPECT_STREQ(descArr[0].fontFamily, fontFamily);
    OH_Drawing_DestroyFontDescriptors(descArr, num);
    OH_Drawing_DestroyFontDescriptor(desc);
}

/*
 * @tc.name: NdkFontDescriptorTest005
 * @tc.desc: test for the fontDescriptor.
 * @tc.type: FUNC
 */
HWTEST_F(NdkFontDescriptorTest, NdkFontDescriptorTest005, TestSize.Level0)
{
    OH_Drawing_FontDescriptor* desc = OH_Drawing_CreateFontDescriptor();
    char* fontFamily = strdup("HarmonyOS Sans");
    desc->fontFamily = fontFamily;
    desc->weight = 400;

    size_t num = 0;
    OH_Drawing_FontDescriptor* descArr = OH_Drawing_MatchFontDescriptors(desc, &num);
    ASSERT_NE(descArr, nullptr);
    EXPECT_LE(1, num);
    EXPECT_STREQ(descArr[0].fontFamily, fontFamily);
    EXPECT_EQ(descArr[0].weight, 400);
    OH_Drawing_DestroyFontDescriptors(descArr, num);
    OH_Drawing_DestroyFontDescriptor(desc);
}

/*
 * @tc.name: NdkFontDescriptorTest006
 * @tc.desc: test for abnormal parameters when obtaining the font list and obtaining the FontDescriptor by fullName.
 * @tc.type: FUNC
 */
HWTEST_F(NdkFontDescriptorTest, NdkFontDescriptorTest006, TestSize.Level0)
{
    OH_Drawing_SystemFontType fontType = OH_Drawing_SystemFontType(0b10000);
    OH_Drawing_Array *fontList = OH_Drawing_GetSystemFontFullNamesByType(fontType);
    EXPECT_EQ(fontList, nullptr);

    OH_Drawing_FontDescriptor *descriptor = OH_Drawing_GetFontDescriptorByFullName(nullptr, fontType);
    EXPECT_EQ(descriptor, nullptr);

    // The array ttfFullName represents the UTF-16 encoded version of a non-existent font full name "你好openharmony".
    const uint8_t ttfFullName[] = { 0x4F, 0x60, 0x59, 0x7D, 0x00, 0x6F, 0x00, 0x70, 0x00, 0x65, 0x00, 0x6E, 0x00, 0x68,
        0x00, 0x61, 0x00, 0x72, 0x00, 0x6D, 0x00, 0x6F, 0x00, 0x6E, 0x00, 0x79 };
    OH_Drawing_String drawingString;
    drawingString.strData = const_cast<uint8_t*>(ttfFullName);
    drawingString.strLen = sizeof(ttfFullName);
    OH_Drawing_FontDescriptor *descriptor1 =
        OH_Drawing_GetFontDescriptorByFullName(&drawingString, OH_Drawing_SystemFontType::ALL);
    EXPECT_EQ(descriptor1, nullptr);
}

/*
 * @tc.name: NdkFontDescriptorTest007
 * @tc.desc: test for obtaining the array of installed fonts.
 * @tc.type: FUNC
 */
HWTEST_F(NdkFontDescriptorTest, NdkFontDescriptorTest007, TestSize.Level0)
{
    InitInstallConfig();
    std::unordered_set<std::string> fullnames { "Noto Sans CJK JP", "Noto Sans CJK KR", "Noto Sans CJK SC",
        "Noto Sans CJK TC", "Noto Sans CJK HK", "Noto Sans Mono CJK JP", "Noto Sans Mono CJK KR",
        "Noto Sans Mono CJK SC", "Noto Sans Mono CJK TC", "Noto Sans Mono CJK HK", "Noto Sans Regular" };
    OH_Drawing_SystemFontType fontType = OH_Drawing_SystemFontType::INSTALLED;
    OH_Drawing_Array* fontList = OH_Drawing_GetSystemFontFullNamesByType(fontType);
    EXPECT_NE(fontList, nullptr);
    size_t size = OH_Drawing_GetDrawingArraySize(fontList);
    EXPECT_EQ(size, 11);
    for (size_t i = 0; i < size; i++) {
        const OH_Drawing_String* fullName = OH_Drawing_GetSystemFontFullNameByIndex(fontList, i);
        OH_Drawing_FontDescriptor* fd = OH_Drawing_GetFontDescriptorByFullName(fullName, INSTALLED);
        EXPECT_TRUE(fullnames.count(fd->fullName));
        OH_Drawing_DestroyFontDescriptor(fd);
    }
    OH_Drawing_DestroySystemFontFullNames(fontList);
    DestroyInstallConfig();
}

/*
 * @tc.name: NdkFontDescriptorTest008
 * @tc.desc: test for obtaining the array of stylish fonts.
 * @tc.type: FUNC
 */
HWTEST_F(NdkFontDescriptorTest, NdkFontDescriptorTest008, TestSize.Level0)
{
    if (!ExistStylishFontConfigFile()) {
        return;
    }
    OH_Drawing_SystemFontType fontType = OH_Drawing_SystemFontType::STYLISH;
    OH_Drawing_Array *fontList = OH_Drawing_GetSystemFontFullNamesByType(fontType);
    ASSERT_NE(fontList, nullptr);
    size_t size = OH_Drawing_GetDrawingArraySize(fontList);
    EXPECT_EQ(size, 1);
    for (size_t i = 0; i < size; i++) {
        const OH_Drawing_String *fontFullName = OH_Drawing_GetSystemFontFullNameByIndex(fontList, i);
        EXPECT_NE(fontFullName, nullptr);
        OH_Drawing_FontDescriptor *descriptor = OH_Drawing_GetFontDescriptorByFullName(fontFullName, fontType);
        ASSERT_NE(descriptor, nullptr);
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
        char16_t* begin = reinterpret_cast<char16_t*>(fontFullName->strData);
        char16_t* end = reinterpret_cast<char16_t*>(fontFullName->strData + fontFullName->strLen);
        EXPECT_EQ(convert.to_bytes(begin, end), descriptor->fullName);
        OH_Drawing_DestroyFontDescriptor(descriptor);
    }
    OH_Drawing_DestroySystemFontFullNames(fontList);
}

/*
 * @tc.name: NdkFontDescriptorTest009
 * @tc.desc: test for obtaining the array of system generic fonts.
 * @tc.type: FUNC
 */
HWTEST_F(NdkFontDescriptorTest, NdkFontDescriptorTest009, TestSize.Level0)
{
    OH_Drawing_SystemFontType fontType = OH_Drawing_SystemFontType::GENERIC;
    OH_Drawing_Array *fontList = OH_Drawing_GetSystemFontFullNamesByType(fontType);
    ASSERT_NE(fontList, nullptr);
    size_t size = OH_Drawing_GetDrawingArraySize(fontList);
    if (ExistStylishFontConfigFile()) {
        EXPECT_EQ(size, 142);
    } else {
        EXPECT_EQ(size, 141);
    }
    for (size_t i = 0; i < size; i++) {
        const OH_Drawing_String *fontFullName = OH_Drawing_GetSystemFontFullNameByIndex(fontList, i);
        EXPECT_NE(fontFullName, nullptr);
        OH_Drawing_FontDescriptor *descriptor = OH_Drawing_GetFontDescriptorByFullName(fontFullName, fontType);
        ASSERT_NE(descriptor, nullptr);
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
        char16_t* begin = reinterpret_cast<char16_t*>(fontFullName->strData);
        char16_t* end = reinterpret_cast<char16_t*>(fontFullName->strData + fontFullName->strLen);
        EXPECT_EQ(convert.to_bytes(begin, end), descriptor->fullName);
        OH_Drawing_DestroyFontDescriptor(descriptor);
    }
    OH_Drawing_DestroySystemFontFullNames(fontList);
}

/*
 * @tc.name: NdkFontDescriptorTest010
 * @tc.desc: test for obtaining the list of composite fonts.
 * @tc.type: FUNC
 */
HWTEST_F(NdkFontDescriptorTest, NdkFontDescriptorTest010, TestSize.Level0)
{
    if (!ExistStylishFontConfigFile()) {
        return;
    }
    OH_Drawing_SystemFontType fontType = OH_Drawing_SystemFontType(STYLISH);
    OH_Drawing_Array *fontList = OH_Drawing_GetSystemFontFullNamesByType(fontType);
    ASSERT_NE(fontList, nullptr);
    size_t size = OH_Drawing_GetDrawingArraySize(fontList);
    EXPECT_EQ(size, 1);
    for (size_t i = 0; i < size; i++) {
        const OH_Drawing_String *fontFullName = OH_Drawing_GetSystemFontFullNameByIndex(fontList, i);
        EXPECT_NE(fontFullName, nullptr);
        OH_Drawing_FontDescriptor *descriptor = OH_Drawing_GetFontDescriptorByFullName(fontFullName, fontType);
        ASSERT_NE(descriptor, nullptr);
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
        char16_t* begin = reinterpret_cast<char16_t*>(fontFullName->strData);
        char16_t* end = reinterpret_cast<char16_t*>(fontFullName->strData + fontFullName->strLen);
        EXPECT_EQ(convert.to_bytes(begin, end), descriptor->fullName);
        OH_Drawing_DestroyFontDescriptor(descriptor);
    }
    OH_Drawing_DestroySystemFontFullNames(fontList);
}

/*
 * @tc.name: NdkFontDescriptorTest011
 * @tc.desc: test for obtaining the list of composite fonts that include "ALL".
 * @tc.type: FUNC
 */
HWTEST_F(NdkFontDescriptorTest, NdkFontDescriptorTest011, TestSize.Level0)
{
    OH_Drawing_SystemFontType fontType = OH_Drawing_SystemFontType(ALL | STYLISH);
    OH_Drawing_Array* fontList = OH_Drawing_GetSystemFontFullNamesByType(fontType);
    ASSERT_NE(fontList, nullptr);
    size_t size = OH_Drawing_GetDrawingArraySize(fontList);
    if (ExistStylishFontConfigFile()) {
        EXPECT_EQ(size, 143);
    } else {
        EXPECT_EQ(size, 141);
    }
    for (size_t i = 0; i < size; i++) {
        const OH_Drawing_String *fontFullName = OH_Drawing_GetSystemFontFullNameByIndex(fontList, i);
        EXPECT_NE(fontFullName, nullptr);
        OH_Drawing_FontDescriptor *descriptor = OH_Drawing_GetFontDescriptorByFullName(fontFullName, fontType);
        ASSERT_NE(descriptor, nullptr);
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
        char16_t* begin = reinterpret_cast<char16_t*>(fontFullName->strData);
        char16_t* end = reinterpret_cast<char16_t*>(fontFullName->strData + fontFullName->strLen);
        EXPECT_EQ(convert.to_bytes(begin, end), descriptor->fullName);
        OH_Drawing_DestroyFontDescriptor(descriptor);
    }
    OH_Drawing_DestroySystemFontFullNames(fontList);
}

/*
 * @tc.name: NdkFontDescriptorTest012
 * @tc.desc: test for abnormal parameters when obtaining the fullName by index and releasing array memory.
 * @tc.type: FUNC
 */
HWTEST_F(NdkFontDescriptorTest, NdkFontDescriptorTest012, TestSize.Level0)
{
    OH_Drawing_SystemFontType fontType = OH_Drawing_SystemFontType::GENERIC;
    OH_Drawing_Array *fontList = OH_Drawing_GetSystemFontFullNamesByType(fontType);
    ASSERT_NE(fontList, nullptr);
    const OH_Drawing_String* fullName = OH_Drawing_GetSystemFontFullNameByIndex(fontList, 500);
    EXPECT_EQ(fullName, nullptr);
    OH_Drawing_DestroySystemFontFullNames(fontList);

    const OH_Drawing_String* fullName1 = OH_Drawing_GetSystemFontFullNameByIndex(nullptr, 0);
    EXPECT_EQ(fullName1, nullptr);

    const OH_Drawing_String* fullName2 = OH_Drawing_GetSystemFontFullNameByIndex(nullptr, 500);
    EXPECT_EQ(fullName2, nullptr);

    OH_Drawing_DestroySystemFontFullNames(nullptr);
}

/*
 * @tc.name: NdkFontDescriptorTest013
 * @tc.desc: test for obtaining the list of composite fonts that include "ALL".
 * @tc.type: FUNC
 */
HWTEST_F(NdkFontDescriptorTest, NdkFontDescriptorTest013, TestSize.Level0)
{
    OH_Drawing_FontCollection *fc = OH_Drawing_CreateSharedFontCollection();
    const char* fontFamily = "HM Symbol Regular";
    const char* fontPath = "/system/fonts/HMSymbolVF.ttf";
    OH_Drawing_RegisterFont(fc, fontFamily, fontPath);

    OH_Drawing_Array *ttfs = OH_Drawing_GetSystemFontFullNamesByType(ALL);
    size_t num = OH_Drawing_GetDrawingArraySize(ttfs);
    if (ExistStylishFontConfigFile()) {
        EXPECT_EQ(num, 143);
    } else {
        EXPECT_EQ(num, 141);
    }
    FontDescriptorMgrInstance.ClearFontFileCache();
    OH_Drawing_DestroyFontCollection(fc);
}

/*
 * @tc.name: NdkFontDescriptorTest014
 * @tc.desc: test for registering a font once and query it.
 * @tc.type: FUNC
 */
HWTEST_F(NdkFontDescriptorTest, NdkFontDescriptorTest014, TestSize.Level0)
{
    OH_Drawing_FontCollection *fc = OH_Drawing_CreateSharedFontCollection();
    const char* fontFamily = "HM Symbol Regular";
    const char* fontPath = "/system/fonts/HMSymbolVF.ttf";
    OH_Drawing_RegisterFont(fc, fontFamily, fontPath);

    OH_Drawing_Array *ttfs = OH_Drawing_GetSystemFontFullNamesByType(CUSTOMIZED);
    size_t num = OH_Drawing_GetDrawingArraySize(ttfs);
    EXPECT_EQ(num, 1);
    for (size_t i = 0; i < num; i++) {
        const OH_Drawing_String *fullName = OH_Drawing_GetSystemFontFullNameByIndex(ttfs, i);
        OH_Drawing_FontDescriptor *fd = OH_Drawing_GetFontDescriptorByFullName(fullName, CUSTOMIZED);
        ASSERT_STREQ(fd->fullName, "HM Symbol Regular");
    }
    FontDescriptorMgrInstance.ClearFontFileCache();
    OH_Drawing_DestroyFontCollection(fc);
}

/*
 * @tc.name: NdkFontDescriptorTest015
 * @tc.desc: test for registering a font five times and query it.
 * @tc.type: FUNC
 */
HWTEST_F(NdkFontDescriptorTest, NdkFontDescriptorTest015, TestSize.Level0)
{
    OH_Drawing_FontCollection *fc = OH_Drawing_CreateSharedFontCollection();
    const char* fontFamily = "HM Symbol Regular";
    const char* fontPath = "/system/fonts/HMSymbolVF.ttf";
    OH_Drawing_RegisterFont(fc, fontFamily, fontPath);
    OH_Drawing_RegisterFont(fc, fontFamily, fontPath);
    OH_Drawing_RegisterFont(fc, fontFamily, fontPath);
    OH_Drawing_RegisterFont(fc, fontFamily, fontPath);
    OH_Drawing_RegisterFont(fc, fontFamily, fontPath);

    OH_Drawing_Array *ttfs = OH_Drawing_GetSystemFontFullNamesByType(CUSTOMIZED);
    size_t num = OH_Drawing_GetDrawingArraySize(ttfs);
    EXPECT_EQ(num, 1);
    for (size_t i = 0; i < num; i++) {
        const OH_Drawing_String *fullName = OH_Drawing_GetSystemFontFullNameByIndex(ttfs, i);
        OH_Drawing_FontDescriptor *fd = OH_Drawing_GetFontDescriptorByFullName(fullName, CUSTOMIZED);
        ASSERT_STREQ(fd->fullName, "HM Symbol Regular");
    }
    FontDescriptorMgrInstance.ClearFontFileCache();
    OH_Drawing_DestroyFontCollection(fc);
}

/*
 * @tc.name: NdkFontDescriptorTest016
 * @tc.desc: test for registering a TTC font and query it.
 * @tc.type: FUNC
 */
HWTEST_F(NdkFontDescriptorTest, NdkFontDescriptorTest016, TestSize.Level0)
{
    OH_Drawing_FontCollection *fc = OH_Drawing_CreateSharedFontCollection();
    const char* fontFamily = "NotoSansCJKjp-Regular-Alphabetic";
    const char* fontPath = "/system/fonts/NotoSansCJK-Regular.ttc";
    OH_Drawing_RegisterFont(fc, fontFamily, fontPath);

    OH_Drawing_Array *ttfs = OH_Drawing_GetSystemFontFullNamesByType(CUSTOMIZED);
    size_t num = OH_Drawing_GetDrawingArraySize(ttfs);
    EXPECT_EQ(num, 1);
    for (size_t i = 0; i < num; i++) {
        const OH_Drawing_String *fullName = OH_Drawing_GetSystemFontFullNameByIndex(ttfs, i);
        OH_Drawing_FontDescriptor *fd = OH_Drawing_GetFontDescriptorByFullName(fullName, CUSTOMIZED);
        ASSERT_STREQ(fd->fullName, "Noto Sans CJK JP");
    }
    FontDescriptorMgrInstance.ClearFontFileCache();
    OH_Drawing_DestroyFontCollection(fc);
}

/*
 * @tc.name: NdkFontDescriptorTest017
 * @tc.desc: test for registering a OTF font and query it.
 * @tc.type: FUNC
 */
HWTEST_F(NdkFontDescriptorTest, NdkFontDescriptorTest017, TestSize.Level0)
{
    OH_Drawing_FontCollection *fc = OH_Drawing_CreateSharedFontCollection();
    const char* fontFamily = "Birch std";
    const char* fontPath = "/system/fonts/Birchstd.otf";
    if (!fs::exists(fontPath)) {
        return;
    }
    OH_Drawing_RegisterFont(fc, fontFamily, fontPath);

    OH_Drawing_Array *ttfs = OH_Drawing_GetSystemFontFullNamesByType(CUSTOMIZED);
    size_t num = OH_Drawing_GetDrawingArraySize(ttfs);
    EXPECT_EQ(num, 1);
    for (size_t i = 0; i < num; i++) {
        const OH_Drawing_String *fullName = OH_Drawing_GetSystemFontFullNameByIndex(ttfs, i);
        OH_Drawing_FontDescriptor *fd = OH_Drawing_GetFontDescriptorByFullName(fullName, CUSTOMIZED);
        ASSERT_STREQ(fd->fullName, "Birch std");
    }
    FontDescriptorMgrInstance.ClearFontFileCache();
    OH_Drawing_DestroyFontCollection(fc);
}

/*
 * @tc.name: NdkFontDescriptorTest018
 * @tc.desc: test for registering failed.
 * @tc.type: FUNC
 */
HWTEST_F(NdkFontDescriptorTest, NdkFontDescriptorTest018, TestSize.Level0)
{
    OH_Drawing_FontCollection *fc = OH_Drawing_CreateSharedFontCollection();
    const char* fontFamily = "xxxxxxx";
    const char* fontPath = "/system/fonts/xxxxxxx.ttf";
    OH_Drawing_RegisterFont(fc, fontFamily, fontPath);

    OH_Drawing_Array *ttfs = OH_Drawing_GetSystemFontFullNamesByType(CUSTOMIZED);
    size_t num = OH_Drawing_GetDrawingArraySize(ttfs);
    EXPECT_EQ(num, 0);
    FontDescriptorMgrInstance.ClearFontFileCache();
    OH_Drawing_DestroyFontCollection(fc);
}

/*
 * @tc.name: NdkFontDescriptorTest019
 * @tc.desc: test for registering a font with a local fontCollection.
 * @tc.type: FUNC
 */
HWTEST_F(NdkFontDescriptorTest, NdkFontDescriptorTest019, TestSize.Level0)
{
    OH_Drawing_FontCollection *fc = OH_Drawing_CreateFontCollection();
    const char* fontFamily = "HM Symbol Regular";
    const char* fontPath = "/system/fonts/HMSymbolVF.ttf";
    OH_Drawing_RegisterFont(fc, fontFamily, fontPath);

    OH_Drawing_Array *ttfs = OH_Drawing_GetSystemFontFullNamesByType(CUSTOMIZED);
    size_t num = OH_Drawing_GetDrawingArraySize(ttfs);
    EXPECT_EQ(num, 1);
    for (size_t i = 0; i < num; i++) {
        const OH_Drawing_String *fullName = OH_Drawing_GetSystemFontFullNameByIndex(ttfs, i);
        OH_Drawing_FontDescriptor *fd = OH_Drawing_GetFontDescriptorByFullName(fullName, CUSTOMIZED);
        ASSERT_STREQ(fd->fullName, "HM Symbol Regular");
    }

    OH_Drawing_TypographyStyle *typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_CreateTypographyHandler(typoStyle, fc);
    OH_Drawing_Array *ttfs1 = OH_Drawing_GetSystemFontFullNamesByType(CUSTOMIZED);
    size_t num1 = OH_Drawing_GetDrawingArraySize(ttfs1);
    EXPECT_EQ(num1, 0);
    FontDescriptorMgrInstance.ClearFontFileCache();
    OH_Drawing_DestroyFontCollection(fc);
}
} // namespace OHOS