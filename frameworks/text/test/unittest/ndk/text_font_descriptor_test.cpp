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
#include "unicode/unistr.h"

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

// "Noto Sans Mono CJK KR" exchange with "Noto Sans Mono CJK JP", make "Noto Sans Mono CJK HK" invalid index
const std::string INSTALL_CONFIG = R"(
{
  "fontlist": [
    { "fontfullpath": "/system/fonts/NotoSansHebrew[wdth,wght].ttf", "fullname": [""] },
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
        "Unknown",
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

std::string ConvertUtf16ToUtf8(const uint8_t* utf16Data, uint32_t utf16Len)
{
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
    std::u16string utf16String(reinterpret_cast<const char16_t*>(utf16Data), utf16Len / 2);
    return converter.to_bytes(utf16String);
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
    std::ofstream ofs(file, std::ios::trunc);
    ofs << INSTALL_CONFIG;
}

void InitInstallConfig()
{
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

class InstallConfig {
public:
    InstallConfig()
    {
        InitInstallConfig();
    }

    ~InstallConfig()
    {
        DestroyInstallConfig();
    }
};

/*
 * @tc.name: NdkFontFullDescriptorTest001
 * @tc.desc: test for get font full descriptor from url when path is nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(NdkFontDescriptorTest, NdkFontFullDescriptorTest001, TestSize.Level0)
{
    OH_Drawing_Array* fontFullDescArr = OH_Drawing_GetFontFullDescriptorsFromPath(nullptr);
    EXPECT_EQ(fontFullDescArr, nullptr);
    size_t num = OH_Drawing_GetDrawingArraySize(fontFullDescArr);
    EXPECT_EQ(num, 0);
 
    const OH_Drawing_FontFullDescriptor *desc = OH_Drawing_GetFontFullDescriptorByIndex(fontFullDescArr, 0);
    EXPECT_EQ(desc, nullptr);
 
    OH_Drawing_DestroyFontFullDescriptors(fontFullDescArr);
}
 
/*
 * @tc.name: NdkFontFullDescriptorTest002
 * @tc.desc: test for get font full descriptor from url when path is invalid.
 * @tc.type: FUNC
 */
HWTEST_F(NdkFontDescriptorTest, NdkFontFullDescriptorTest002, TestSize.Level0)
{
    char *invalidPath = strdup("/test/invalid.ttf");
    OH_Drawing_Array* fontFullDescArr = OH_Drawing_GetFontFullDescriptorsFromPath(invalidPath);
    EXPECT_EQ(fontFullDescArr, nullptr);
 
    size_t num = OH_Drawing_GetDrawingArraySize(fontFullDescArr);
    EXPECT_EQ(num, 0);
 
    const OH_Drawing_FontFullDescriptor *desc1 = OH_Drawing_GetFontFullDescriptorByIndex(fontFullDescArr, 1);
    EXPECT_EQ(desc1, nullptr);
 
    OH_Drawing_DestroyFontFullDescriptors(fontFullDescArr);
    free(invalidPath);
}

/*
 * @tc.name: NdkFontFullDescriptorTest003
 * @tc.desc: test for get font full descriptor from url when path is empty string.
 * @tc.type: FUNC
 */
HWTEST_F(NdkFontDescriptorTest, NdkFontFullDescriptorTest003, TestSize.Level0)
{
    char *invalidPath = strdup("");
    OH_Drawing_Array* fontFullDescArr = OH_Drawing_GetFontFullDescriptorsFromPath(invalidPath);
    EXPECT_EQ(fontFullDescArr, nullptr);
 
    size_t num = OH_Drawing_GetDrawingArraySize(fontFullDescArr);
    EXPECT_EQ(num, 0);
 
    const OH_Drawing_FontFullDescriptor *desc1 = OH_Drawing_GetFontFullDescriptorByIndex(fontFullDescArr, 1);
    EXPECT_EQ(desc1, nullptr);
 
    OH_Drawing_DestroyFontFullDescriptors(fontFullDescArr);
    OH_Drawing_DestroyFontFullDescriptors(nullptr);
    free(invalidPath);
}
 
/*
 * @tc.name: NdkFontFullDescriptorTest004
 * @tc.desc: test for get the ttf fontFullDescriptor and test get font full descriptor attribute string.
 * @tc.type: FUNC
 */
HWTEST_F(NdkFontDescriptorTest, NdkFontFullDescriptorTest004, TestSize.Level0)
{
    char *validPath = strdup("/system/fonts/HarmonyOS_Sans.ttf");
    OH_Drawing_Array* fontFullDescArr = OH_Drawing_GetFontFullDescriptorsFromPath(validPath);
    EXPECT_NE(fontFullDescArr, nullptr);
    EXPECT_EQ(OH_Drawing_GetDrawingArraySize(fontFullDescArr), 1);
 
    const OH_Drawing_FontFullDescriptor *desc = OH_Drawing_GetFontFullDescriptorByIndex(fontFullDescArr, 0);
    EXPECT_NE(desc, nullptr);
    const OH_Drawing_FontFullDescriptor *desc1 = OH_Drawing_GetFontFullDescriptorByIndex(fontFullDescArr, 1);
    EXPECT_EQ(desc1, nullptr);
 
    OH_Drawing_String path = {.strData = nullptr, .strLen = 0};
    OH_Drawing_ErrorCode errorCode0 = OH_Drawing_GetFontFullDescriptorAttributeString(desc, FULL_DESCRIPTOR_ATTR_S_PATH,
        &path);
    EXPECT_EQ(errorCode0, 0);
    EXPECT_STREQ(ConvertUtf16ToUtf8(path.strData, path.strLen).c_str(), validPath);
 
    OH_Drawing_String postscriptName = {.strData = nullptr, .strLen = 0};
    errorCode0 = OH_Drawing_GetFontFullDescriptorAttributeString(desc, FULL_DESCRIPTOR_ATTR_S_POSTSCRIPT_NAME,
        &postscriptName);
    EXPECT_EQ(errorCode0, 0);
    char *realPostscriptName = strdup("HarmonyOS_Sans");
    EXPECT_STREQ(ConvertUtf16ToUtf8(postscriptName.strData, postscriptName.strLen).c_str(), realPostscriptName);
 
    char *realFullAndFamilyName = strdup("HarmonyOS Sans");
    OH_Drawing_String fullName = {.strData = nullptr, .strLen = 0};
    errorCode0 = OH_Drawing_GetFontFullDescriptorAttributeString(desc, FULL_DESCRIPTOR_ATTR_S_FULL_NAME, &fullName);
    EXPECT_EQ(errorCode0, 0);
    EXPECT_STREQ(ConvertUtf16ToUtf8(fullName.strData, fullName.strLen).c_str(), realFullAndFamilyName);
 
    OH_Drawing_String familyName = {.strData = nullptr, .strLen = 0};
    errorCode0 = OH_Drawing_GetFontFullDescriptorAttributeString(desc, FULL_DESCRIPTOR_ATTR_S_FAMILY_NAME, &familyName);
    EXPECT_EQ(errorCode0, 0);
    EXPECT_STREQ(ConvertUtf16ToUtf8(familyName.strData, familyName.strLen).c_str(), realFullAndFamilyName);
 
    char *realSubFamilyName = strdup("Regular");
    OH_Drawing_String subFamilyName = {.strData = nullptr, .strLen = 0};
    errorCode0 = OH_Drawing_GetFontFullDescriptorAttributeString(desc, FULL_DESCRIPTOR_ATTR_S_SUB_FAMILY_NAME,
        &subFamilyName);
    EXPECT_EQ(errorCode0, 0);
    EXPECT_STREQ(ConvertUtf16ToUtf8(subFamilyName.strData, subFamilyName.strLen).c_str(), realSubFamilyName);
 
    OH_Drawing_DestroyFontFullDescriptors(fontFullDescArr);
    free(validPath);
    free(realPostscriptName);
    free(realFullAndFamilyName);
    free(realSubFamilyName);
}

/*
 * @tc.name: NdkFontFullDescriptorTest005
 * @tc.desc: test for get the ttf fontFullDescriptor and test get font full descriptor attribute int and bool.
 * @tc.type: FUNC
 */
HWTEST_F(NdkFontDescriptorTest, NdkFontFullDescriptorTest005, TestSize.Level0)
{
    char *validPath = strdup("/system/fonts/HarmonyOS_Sans.ttf");
    int weight, width, italic = 0;
    bool mono, symblic = false;
    OH_Drawing_Array* fontFullDescArr = OH_Drawing_GetFontFullDescriptorsFromPath(validPath);
    EXPECT_NE(fontFullDescArr, nullptr);

    const OH_Drawing_FontFullDescriptor *desc = OH_Drawing_GetFontFullDescriptorByIndex(fontFullDescArr, 0);

    OH_Drawing_ErrorCode errorCode0 = OH_Drawing_GetFontFullDescriptorAttributeInt(desc, FULL_DESCRIPTOR_ATTR_I_WEIGHT,
        &weight);
    EXPECT_EQ(errorCode0, 0);
    constexpr int realFontWeight = 400;
    EXPECT_EQ(weight, realFontWeight);

    errorCode0 = OH_Drawing_GetFontFullDescriptorAttributeInt(desc, FULL_DESCRIPTOR_ATTR_I_WIDTH, &width);
    EXPECT_EQ(errorCode0, 0);
    constexpr int realFontWidth = 5;
    EXPECT_EQ(width, realFontWidth);

    errorCode0 = OH_Drawing_GetFontFullDescriptorAttributeInt(desc, FULL_DESCRIPTOR_ATTR_I_ITALIC, &italic);
    EXPECT_EQ(errorCode0, 0);
    EXPECT_EQ(italic, 0);

    errorCode0 = OH_Drawing_GetFontFullDescriptorAttributeBool(desc, FULL_DESCRIPTOR_ATTR_B_MONO, &mono);
    EXPECT_EQ(errorCode0, 0);
    EXPECT_FALSE(mono);

    errorCode0 = OH_Drawing_GetFontFullDescriptorAttributeBool(desc, FULL_DESCRIPTOR_ATTR_B_SYMBOLIC, &symblic);
    EXPECT_EQ(errorCode0, 0);
    EXPECT_FALSE(symblic);

    OH_Drawing_DestroyFontFullDescriptors(fontFullDescArr);
    free(validPath);
}

/*
 * @tc.name: NdkFontFullDescriptorTest006
 * @tc.desc: test for OH_Drawing_GetFontFullDescriptorAttribute int and bool errorCode.
 * @tc.type: FUNC
 */
HWTEST_F(NdkFontDescriptorTest, NdkFontFullDescriptorTest006, TestSize.Level0)
{
    char *validPath = strdup("/system/fonts/HarmonyOS_Sans.ttf");
    // Assign it an initial value
    int weight = -1, italic = -1;
    bool mono = false, symblic = false;
    constexpr int initialValue = -1;
    OH_Drawing_Array* fontFullDescArr = OH_Drawing_GetFontFullDescriptorsFromPath(validPath);
    EXPECT_NE(fontFullDescArr, nullptr);
    const OH_Drawing_FontFullDescriptor *desc = OH_Drawing_GetFontFullDescriptorByIndex(fontFullDescArr, 0);

    OH_Drawing_ErrorCode errorCode = OH_Drawing_GetFontFullDescriptorAttributeInt(desc, FULL_DESCRIPTOR_ATTR_S_PATH,
        &weight);
    EXPECT_EQ(errorCode, OH_DRAWING_ERROR_ATTRIBUTE_ID_MISMATCH);
    EXPECT_EQ(weight, initialValue);
    errorCode = OH_Drawing_GetFontFullDescriptorAttributeBool(desc, FULL_DESCRIPTOR_ATTR_I_WEIGHT, &symblic);
    EXPECT_EQ(errorCode, OH_DRAWING_ERROR_ATTRIBUTE_ID_MISMATCH);
    EXPECT_FALSE(symblic);

    errorCode = OH_Drawing_GetFontFullDescriptorAttributeInt(nullptr, FULL_DESCRIPTOR_ATTR_I_ITALIC, &italic);
    EXPECT_EQ(errorCode, OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(weight, initialValue);
    errorCode = OH_Drawing_GetFontFullDescriptorAttributeBool(nullptr, FULL_DESCRIPTOR_ATTR_B_SYMBOLIC, &mono);
    EXPECT_EQ(errorCode, OH_DRAWING_ERROR_INCORRECT_PARAMETER);

    errorCode = OH_Drawing_GetFontFullDescriptorAttributeInt(desc, FULL_DESCRIPTOR_ATTR_I_ITALIC, nullptr);
    EXPECT_EQ(errorCode, OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    errorCode = OH_Drawing_GetFontFullDescriptorAttributeBool(desc, FULL_DESCRIPTOR_ATTR_B_SYMBOLIC, nullptr);
    EXPECT_EQ(errorCode, OH_DRAWING_ERROR_INCORRECT_PARAMETER);

    OH_Drawing_DestroyFontFullDescriptors(fontFullDescArr);
    free(validPath);
}

/*
 * @tc.name: NdkFontFullDescriptorTest007
 * @tc.desc: test for OH_Drawing_GetFontFullDescriptorAttributeString ErrorCode.
 * @tc.type: FUNC
 */
HWTEST_F(NdkFontDescriptorTest, NdkFontFullDescriptorTest007, TestSize.Level0)
{
    char *validPath = strdup("/system/fonts/HarmonyOS_Sans.ttf");
    OH_Drawing_Array* fontFullDescArr = OH_Drawing_GetFontFullDescriptorsFromPath(validPath);
    EXPECT_NE(fontFullDescArr, nullptr);
    const OH_Drawing_FontFullDescriptor *desc = OH_Drawing_GetFontFullDescriptorByIndex(fontFullDescArr, 0);

    // Assign it an initial value
    OH_Drawing_String path = {.strData = nullptr, .strLen = -1};
    OH_Drawing_ErrorCode errorCode = OH_Drawing_GetFontFullDescriptorAttributeString(desc,
        FULL_DESCRIPTOR_ATTR_I_WIDTH, &path);
    EXPECT_EQ(errorCode, OH_DRAWING_ERROR_ATTRIBUTE_ID_MISMATCH);
    constexpr int initialValue = -1;
    EXPECT_EQ(path.strData, nullptr);
    EXPECT_EQ(path.strLen, initialValue);

    errorCode = OH_Drawing_GetFontFullDescriptorAttributeString(nullptr, FULL_DESCRIPTOR_ATTR_I_ITALIC, &path);
    EXPECT_EQ(errorCode, OH_DRAWING_ERROR_INCORRECT_PARAMETER);

    errorCode = OH_Drawing_GetFontFullDescriptorAttributeInt(desc, FULL_DESCRIPTOR_ATTR_I_ITALIC, nullptr);
    EXPECT_EQ(errorCode, OH_DRAWING_ERROR_INCORRECT_PARAMETER);

    OH_Drawing_DestroyFontFullDescriptors(fontFullDescArr);
    free(validPath);
}

/*
 * @tc.name: NdkFontFullDescriptorTest008
 * @tc.desc: test for the ttc fontFullDescriptor valid path.
 * @tc.type: FUNC
 */
HWTEST_F(NdkFontDescriptorTest, NdkFontFullDescriptorTest008, TestSize.Level0)
{
    char* ttcPath = strdup("/system/fonts/NotoSerifCJK-Regular.ttc");
    OH_Drawing_Array* fontFullDescArr = OH_Drawing_GetFontFullDescriptorsFromPath(ttcPath);
    EXPECT_NE(fontFullDescArr, nullptr);
    constexpr int fontFullDescriptorsSize = 5;
    EXPECT_EQ(OH_Drawing_GetDrawingArraySize(fontFullDescArr), fontFullDescriptorsSize);

    const OH_Drawing_FontFullDescriptor* desc0 = OH_Drawing_GetFontFullDescriptorByIndex(fontFullDescArr, 0);
    EXPECT_NE(desc0, nullptr);
    const OH_Drawing_FontFullDescriptor* desc1 = OH_Drawing_GetFontFullDescriptorByIndex(fontFullDescArr, 1);
    EXPECT_NE(desc1, nullptr);
    const OH_Drawing_FontFullDescriptor* desc4 = OH_Drawing_GetFontFullDescriptorByIndex(fontFullDescArr,
        fontFullDescriptorsSize - 1);
    EXPECT_NE(desc4, nullptr);

    OH_Drawing_String path = { .strData = nullptr, .strLen = 0 };
    OH_Drawing_ErrorCode errorCode0 =
        OH_Drawing_GetFontFullDescriptorAttributeString(desc0, FULL_DESCRIPTOR_ATTR_S_PATH, &path);
    EXPECT_EQ(errorCode0, 0);
    EXPECT_STREQ(ConvertUtf16ToUtf8(path.strData, path.strLen).c_str(), ttcPath);

    OH_Drawing_String postscriptName = { .strData = nullptr, .strLen = 0 };
    errorCode0 = OH_Drawing_GetFontFullDescriptorAttributeString(desc0, FULL_DESCRIPTOR_ATTR_S_POSTSCRIPT_NAME,
        &postscriptName);
    EXPECT_EQ(errorCode0, 0);
    char* realPostscriptName = strdup("NotoSerifCJKjp-Regular");
    EXPECT_STREQ(ConvertUtf16ToUtf8(postscriptName.strData, postscriptName.strLen).c_str(), realPostscriptName);

    errorCode0 = OH_Drawing_GetFontFullDescriptorAttributeString(desc1, FULL_DESCRIPTOR_ATTR_S_POSTSCRIPT_NAME,
        &postscriptName);
    EXPECT_EQ(errorCode0, 0);
    char* realPostscriptName1 = strdup("NotoSerifCJKkr-Regular");
    EXPECT_STREQ(ConvertUtf16ToUtf8(postscriptName.strData, postscriptName.strLen).c_str(), realPostscriptName1);

    OH_Drawing_String fullName = { .strData = nullptr, .strLen = 0 };
    errorCode0 = OH_Drawing_GetFontFullDescriptorAttributeString(desc4, FULL_DESCRIPTOR_ATTR_S_FULL_NAME, &fullName);
    EXPECT_EQ(errorCode0, 0);
    char* realFullAndFamilyName = strdup("Noto Serif CJK HK");
    EXPECT_STREQ(ConvertUtf16ToUtf8(fullName.strData, fullName.strLen).c_str(), realFullAndFamilyName);

    int weight = 0;
    errorCode0 = OH_Drawing_GetFontFullDescriptorAttributeInt(desc4, FULL_DESCRIPTOR_ATTR_I_WEIGHT, &weight);
    EXPECT_EQ(errorCode0, 0);
    constexpr int realFontWeight = 400;
    EXPECT_EQ(weight, realFontWeight);

    OH_Drawing_DestroyFontFullDescriptors(fontFullDescArr);
    free(ttcPath);
    free(realPostscriptName);
    free(realPostscriptName1);
    free(realFullAndFamilyName);
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
    EXPECT_GE(num, 1);
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
    EXPECT_GE(num, 1);
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
    InstallConfig installConfig;
    std::unordered_set<std::string> fullnames { "Noto Sans CJK JP", "Noto Sans CJK KR", "Noto Sans CJK SC",
        "Noto Sans CJK TC", "Noto Sans CJK HK", "Noto Sans Mono CJK JP", "Noto Sans Mono CJK KR",
        "Noto Sans Mono CJK SC", "Noto Sans Mono CJK TC", "Noto Sans Mono CJK HK", "Noto Sans Regular" };
    OH_Drawing_SystemFontType fontType = OH_Drawing_SystemFontType::INSTALLED;
    OH_Drawing_Array* fontList = OH_Drawing_GetSystemFontFullNamesByType(fontType);
    EXPECT_NE(fontList, nullptr);
    size_t size = OH_Drawing_GetDrawingArraySize(fontList);
    EXPECT_EQ(size, 12);
    for (size_t i = 0; i < size; i++) {
        const OH_Drawing_String* fullName = OH_Drawing_GetSystemFontFullNameByIndex(fontList, i);
        ASSERT_NE(fullName, nullptr);
        OH_Drawing_FontDescriptor* fd = OH_Drawing_GetFontDescriptorByFullName(fullName, INSTALLED);
        if (fd != nullptr) {
            EXPECT_TRUE(fullnames.count(fd->fullName));
            OH_Drawing_DestroyFontDescriptor(fd);
        } else {
            std::string s;
            icu::UnicodeString ustr(reinterpret_cast<UChar*>(fullName->strData), fullName->strLen / sizeof(char16_t));
            ustr.toUTF8String(s);
            EXPECT_EQ(s, "Unknown");
        }
    }
    std::u16string fullName = u"not exist";
    OH_Drawing_String fn { reinterpret_cast<uint8_t*>(fullName.data()), fullName.size() * sizeof(char16_t) };
    OH_Drawing_FontDescriptor* fd = OH_Drawing_GetFontDescriptorByFullName(&fn, INSTALLED);
    EXPECT_EQ(fd, nullptr);
    OH_Drawing_DestroySystemFontFullNames(fontList);
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