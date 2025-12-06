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

#include <fstream>
#include <gtest/gtest.h>

#include "font_config.h"
#include "font_parser.h"
#include "font_tool_set.h"
#include "cmap_table_parser.h"
#include "locale_config.h"
#include "name_table_parser.h"
#include "post_table_parser.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace TextEngine {
static const std::string FILE_NAME = "/system/fonts/visibility_list.json";
static const std::string TEST_FONT_PATH = "/system/fonts/NotoSans[wdth,wght].ttf";
static const std::string TEST_TTC_PATH = "/system/fonts/NotoSansCJK-Regular.ttc";
static const std::string NON_EXISTENT_PATH = "/system/fonts/nonexistent.ttf";

class FontParserTest : public testing::Test {
};

std::vector<std::string> GetFontSet(const char* fname)
{
    FontConfig fontConfig(fname);
    return fontConfig.GetFontSet();
}

/**
 * @tc.name: FontParserTest1
 * @tc.desc: test get fontSet file parser
 * @tc.type:FUNC
 */
HWTEST_F(FontParserTest, FontParserTest1, TestSize.Level0)
{
    auto fontSet1 = GetFontSet(nullptr);
    EXPECT_EQ(fontSet1.size(), 0);

    std::ifstream fileStream(FILE_NAME.c_str());
    if (fileStream.is_open()) {
        auto fontSet2 = GetFontSet(FILE_NAME.c_str());
        EXPECT_NE(fontSet2.size(), 0);
        fileStream.close();
    } else {
        auto fontSet2 = GetFontSet(FILE_NAME.c_str());
        EXPECT_EQ(fontSet2.size(), 0);
    }
}

/**
 * @tc.name: FontParserTest2
 * @tc.desc: test font file parser
 * @tc.type:FUNC
 */
HWTEST_F(FontParserTest, FontParserTest2, TestSize.Level0)
{
    FontParser fontParser;
    auto visibilityFonts = fontParser.GetVisibilityFonts();
    fontParser.GetVisibilityFontByName("Noto Sans Regular");
    std::ifstream fileStream(FILE_NAME.c_str());
    if (fileStream.is_open()) {
        EXPECT_NE(visibilityFonts.size(), 0);
        fileStream.close();
    } else {
        EXPECT_EQ(visibilityFonts.size(), 0);
    }
}

/**
 * @tc.name: FontParserTest3
 * @tc.desc: test font file parser
 * @tc.type:FUNC
 */
HWTEST_F(FontParserTest, FontParserTest3, TestSize.Level0)
{
    FontParser fontParser;
    std::unique_ptr<FontParser::FontDescriptor> font =
        fontParser.GetVisibilityFontByName("Noto Sans Regular");
    ASSERT_NE(font, nullptr);
    EXPECT_EQ(font->fontFamily, "Noto Sans");
}

/**
 * @tc.name: FontConfigTest1
 * @tc.desc: test font file parser
 * @tc.type:FUNC
 */
HWTEST_F(FontParserTest, FontConfigTest1, TestSize.Level0)
{
    FontConfigJson fontConfigJson;
    EXPECT_EQ(fontConfigJson.ParseFile(), 0);
    auto info = fontConfigJson.GetFontConfigJsonInfo();
    ASSERT_NE(info, nullptr);
    EXPECT_EQ(info->fontDirSet.size(), 1);
    fontConfigJson.Dump();
}

/**
 * @tc.name: FontConfigTest2
 * @tc.desc: test font file parser
 * @tc.type:FUNC
 */
HWTEST_F(FontParserTest, FontConfigTest2, TestSize.Level0)
{
    FontConfigJson fontConfigJson;
    EXPECT_EQ(fontConfigJson.ParseFontFileMap(), 0);
    auto map = fontConfigJson.GetFontFileMap();
    ASSERT_NE(map, nullptr);
    EXPECT_EQ(map->size(), 281);
    for (auto& it: *map) {
        ASSERT_GT(it.second.size(), 3);
        std::string end = it.second.substr(it.second.size() - 3, 3);
        EXPECT_TRUE(end == "ttf" || end == "oft" || end == "ttc");
    }
    fontConfigJson.Dump();
}

/**
 * @tc.name: OpenTypeBasicTypeTest1
 * @tc.desc: opentype parser test
 * @tc.type:FUNC
 */
HWTEST_F(FontParserTest, OpenTypeBasicTypeTest1, TestSize.Level0)
{
    char test[] = {'a', 'b', 'c', 'd', 0};
    struct OpenTypeBasicType::Tag tag;
    EXPECT_EQ(tag.Get(), "\0\0\0\0");
    struct OpenTypeBasicType::Int16 int16;
    EXPECT_EQ(int16.Get(), 0);
    struct OpenTypeBasicType::Uint16 uint16;
    EXPECT_EQ(uint16.Get(), 0);
    struct OpenTypeBasicType::Int32 int32;
    EXPECT_EQ(int32.Get(), 0);
    struct OpenTypeBasicType::Uint32 uint32;
    EXPECT_EQ(uint32.Get(), 0);
    struct OpenTypeBasicType::Fixed fixed;
    EXPECT_EQ(fixed.Get(), 0);
    std::copy(test, test + sizeof(tag.tags), tag.tags);
    EXPECT_EQ(tag.Get(), test);
}

/**
 * @tc.name: CheckFullNameParamInvalidTest1
 * @tc.desc: test for check full name
 * @tc.type: FUNC
 */
HWTEST_F(FontParserTest, CheckFullNameParamInvalidTest1, TestSize.Level0)
{
    FontParser::FontDescriptor fd;
    fd.requestedFullname = "testName";
    fd.fullName = "testName";
    fd.requestedLid = LANGUAGE_SC;
    EXPECT_TRUE(FontParser::CheckFullNameParamInvalid(fd, LANGUAGE_SC, "non-empty"));

    fd.requestedFullname = "";
    EXPECT_TRUE(FontParser::CheckFullNameParamInvalid(fd, LANGUAGE_SC, ""));
    fd.requestedLid = LANGUAGE_EN;
    EXPECT_TRUE(FontParser::CheckFullNameParamInvalid(fd, LANGUAGE_SC, "non-empty"));
    fd.fullName = "";
    EXPECT_FALSE(FontParser::CheckFullNameParamInvalid(fd, LANGUAGE_TC, "non-empty"));

    fd.requestedFullname = "testName";
    fd.fullName = "otherName";
    fd.requestedLid = LANGUAGE_EN;
    EXPECT_FALSE(FontParser::CheckFullNameParamInvalid(fd, LANGUAGE_EN, "non-empty"));
    fd.fullNameLid = LANGUAGE_EN;
    EXPECT_TRUE(FontParser::CheckFullNameParamInvalid(fd, LANGUAGE_EN, "non-empty"));

    fd.requestedFullname = "";
    fd.requestedLid = LANGUAGE_DEFAULT;
    EXPECT_FALSE(FontParser::CheckFullNameParamInvalid(fd, LANGUAGE_TC, "non-empty"));
}

/**
 * @tc.name: GetFontTypefaceUnicodeFromPathTest
 * @tc.desc: Test getting font Unicode information from file path
 * @tc.type: FUNC
 */
HWTEST_F(FontParserTest, GetFontTypefaceUnicodeFromPathTest, TestSize.Level1)
{
    FontParser fontParser;

    // Test case 1: Valid font file path
    std::ifstream testFile(TEST_FONT_PATH.c_str(), std::ios::binary);
    if (!testFile.is_open()) {
        // If test font file does not exist, skip this test
        GTEST_SKIP();
    }

    auto unicodeList = fontParser.GetFontTypefaceUnicode(TEST_FONT_PATH, 0);
    // Check if the returned Unicode list is not empty
    EXPECT_FALSE(unicodeList.empty());
    testFile.close();

    // Test case 2: Invalid font file path
    auto unicodeList2 = fontParser.GetFontTypefaceUnicode(NON_EXISTENT_PATH, 0);
    EXPECT_TRUE(unicodeList2.empty());

    // Test case 3: TTC font file, test different indices
    std::ifstream ttcFile(TEST_TTC_PATH.c_str());
    if (ttcFile.is_open()) {
        auto unicodeList3 = fontParser.GetFontTypefaceUnicode(TEST_TTC_PATH, 0);
        // For TTC file, index 0 should return valid data
        EXPECT_FALSE(unicodeList3.empty());

        // Test out-of-bounds index case
        auto unicodeList4 = fontParser.GetFontTypefaceUnicode(TEST_TTC_PATH, 100);
        EXPECT_TRUE(unicodeList4.empty());
        ttcFile.close();
    }
}

/**
 * @tc.name: GetFontTypefaceUnicodeFromDataTest
 * @tc.desc: Test getting font Unicode information from memory data
 * @tc.type: FUNC
 */
HWTEST_F(FontParserTest, GetFontTypefaceUnicodeFromDataTest, TestSize.Level1)
{
    FontParser fontParser;
    std::ifstream file(TEST_FONT_PATH.c_str(), std::ios::binary);
    if (!file.is_open()) {
        GTEST_SKIP();
    }
    std::stringstream fileStream;
    fileStream << file.rdbuf();
    std::string fileData = fileStream.str();
    std::vector<uint8_t> testFontData(fileData.begin(), fileData.end());

    // Test case 1: Valid font data
    if (!testFontData.empty()) {
        auto unicodeList = fontParser.GetFontTypefaceUnicode(testFontData.data(), testFontData.size(), 0);
        EXPECT_FALSE(unicodeList.empty());
    }

    // Test case 2: Null data pointer
    auto unicodeList2 = fontParser.GetFontTypefaceUnicode(nullptr, 0, 0);
    EXPECT_TRUE(unicodeList2.empty());

    // Test case 3: Zero length data
    auto unicodeList3 = fontParser.GetFontTypefaceUnicode(testFontData.data(), 0, 0);
    EXPECT_TRUE(unicodeList3.empty());

    // Test case 4: Negative index
    auto unicodeList4 = fontParser.GetFontTypefaceUnicode(testFontData.data(), testFontData.size(), -1);
    EXPECT_TRUE(unicodeList4.empty());
    file.close();
}

/**
 * @tc.name: GetFontFullNameTest
 * @tc.desc: Test getting font full name
 * @tc.type: FUNC
 */
HWTEST_F(FontParserTest, GetFontFullNameTest, TestSize.Level1)
{
    FontParser fontParser;

    // Test case 1: Valid font file path
    std::ifstream testFile(TEST_FONT_PATH.c_str());
    if (!testFile.is_open()) {
        GTEST_SKIP();
    }
    auto fullNames = fontParser.GetFontFullName(TEST_FONT_PATH);
    // Check returned font full name list
    EXPECT_FALSE(fullNames.empty());
    // Each full name should not be empty
    for (const auto& name : fullNames) {
        EXPECT_FALSE(name.empty());
    }
    testFile.close();

    // Test case 2: TTC font file (contains multiple fonts)
    std::ifstream ttcFile(TEST_TTC_PATH.c_str());
    if (ttcFile.is_open()) {
        auto fullNames = fontParser.GetFontFullName(TEST_TTC_PATH);
        // TTC file should return multiple font full names
        EXPECT_FALSE(fullNames.empty());
        // TEST_TTC_PATH has 10 fonts
        EXPECT_EQ(fullNames.size(), 10);
        ttcFile.close();
    }

    // Test case 3: Invalid file path
    auto fullNames3 = fontParser.GetFontFullName(NON_EXISTENT_PATH);
    EXPECT_TRUE(fullNames3.empty());

    // Test case 4: Empty string path
    auto fullNames4 = fontParser.GetFontFullName("");
    EXPECT_TRUE(fullNames4.empty());

    // Test case 5: export function
    auto fullNames5 = FontToolSet::GetInstance().GetFontFullName(TEST_FONT_PATH);
    ASSERT_FALSE(fullNames5.empty());
    EXPECT_STREQ(fullNames[0].c_str(), fullNames5[0].c_str());
}

/**
 * @tc.name: GetBcpTagListTest
 * @tc.desc: Test getting BCP tag list
 * @tc.type: FUNC
 */
HWTEST_F(FontParserTest, GetBcpTagListTest, TestSize.Level1)
{
    FontParser fontParser;

    // Get BCP tag list for current system language
    auto bcpTagList = fontParser.GetBcpTagList();

    // Test case 1: Verify BCP tag list is not empty
    EXPECT_FALSE(bcpTagList.empty());

    // Test case 2: Verify each BCP tag is not empty
    for (const auto& tag : bcpTagList) {
        EXPECT_FALSE(tag.empty());
    }

    // Test case 3: Verify consistency of multiple function calls
    auto bcpTagList2 = fontParser.GetBcpTagList();
    EXPECT_EQ(bcpTagList.size(), bcpTagList2.size());
}

/**
 * @tc.name: FillFontDescriptorWithLocalInfoTest
 * @tc.desc: Test filling font descriptor with local information
 * @tc.type: FUNC
 */
HWTEST_F(FontParserTest, FillFontDescriptorWithLocalInfoTest, TestSize.Level1)
{
    FontParser fontParser;
    FontParser::FontDescriptor desc;

    // Create a Typeface object (using test font file)
    std::ifstream testFile(TEST_FONT_PATH.c_str());
    if (!testFile.is_open()) {
        GTEST_SKIP();
    }

    auto typeface = Drawing::Typeface::MakeFromFile(TEST_FONT_PATH.c_str());
    ASSERT_NE(typeface, nullptr);

    // Test case 1: Fill descriptor with valid typeface
    fontParser.FillFontDescriptorWithLocalInfo(typeface, desc);

    EXPECT_FALSE(desc.localFamilyName.empty());
    EXPECT_FALSE(desc.localFullName.empty());
    EXPECT_FALSE(desc.localSubFamilyName.empty());
    EXPECT_FALSE(desc.localPostscriptName.empty());
    EXPECT_FALSE(desc.version.empty());
    EXPECT_FALSE(desc.manufacture.empty());
    EXPECT_FALSE(desc.copyright.empty());
    EXPECT_FALSE(desc.trademark.empty());
    EXPECT_FALSE(desc.license.empty());

    // Test case 2: Use nullptr typeface
    // Should not crash, all fields should remain unchanged
    FontParser::FontDescriptor desc2;
    fontParser.FillFontDescriptorWithLocalInfo(nullptr, desc2);
    testFile.close();

    // Test case 3: Test field priority in descriptor
    // If descriptor already has some fields, FillFontDescriptorWithLocalInfo should not overwrite them
    FontParser::FontDescriptor desc3;
    desc3.localFamilyName = "PredefinedFamily";
    desc3.localFullName = "PredefinedFullName";

    std::ifstream testFile2(TEST_FONT_PATH.c_str());
    if (testFile2.is_open()) {
        auto typeface = Drawing::Typeface::MakeFromFile(TEST_FONT_PATH.c_str());
        ASSERT_NE(typeface, nullptr);

        fontParser.FillFontDescriptorWithLocalInfo(typeface, desc3);

        // Predefined fields should remain unchanged
        EXPECT_EQ(desc3.localFamilyName, "PredefinedFamily");
        EXPECT_EQ(desc3.localFullName, "PredefinedFullName");

        testFile2.close();
    }

    // improved the test case coverage rate
    std::shared_ptr<Drawing::TypefaceImpl> swapTypefaceImpl = nullptr;
    typeface->typefaceImpl_.swap(swapTypefaceImpl);
    EXPECT_EQ(typeface->GetFontIndex(), 0);
    typeface->typefaceImpl_.swap(swapTypefaceImpl);
}

/**
 * @tc.name: GetFontCountFromPathTest
 * @tc.desc: Test getting font count from file path
 * @tc.type: FUNC
 */
HWTEST_F(FontParserTest, GetFontCountFromPathTest, TestSize.Level1)
{
    FontParser fontParser;

    // Test case 1: Valid font file path (single font)
    std::ifstream testFile(TEST_FONT_PATH.c_str());
    if (!testFile.is_open()) {
        GTEST_SKIP();
    }
    int32_t count = fontParser.GetFontCount(TEST_FONT_PATH);
    // Single font file should return 1
    EXPECT_EQ(count, 1);
    testFile.close();

    // Test case 2: TTC font file (multiple fonts)
    std::ifstream ttcFile(TEST_TTC_PATH.c_str());
    if (ttcFile.is_open()) {
        count = fontParser.GetFontCount(TEST_TTC_PATH);
        // TTC file should return count greater than 1
        EXPECT_GT(count, 0);
        ttcFile.close();
    }

    // Test case 3: Invalid file path
    int32_t count3 = fontParser.GetFontCount(NON_EXISTENT_PATH);
    EXPECT_EQ(count3, 0);

    // Test case 4: Empty string path
    int32_t count4 = fontParser.GetFontCount("");
    EXPECT_EQ(count4, 0);
}

/**
 * @tc.name: GetFontCountFromDataTest
 * @tc.desc: Test getting font count from memory data
 * @tc.type: FUNC
 */
HWTEST_F(FontParserTest, GetFontCountFromDataTest, TestSize.Level1)
{
    FontParser fontParser;

    // Test case 1: Valid font data
    std::ifstream file(TEST_FONT_PATH.c_str(), std::ios::binary);
    if (!file.is_open()) {
        GTEST_SKIP();
    }
    std::stringstream fileStream;
    fileStream << file.rdbuf();
    std::string fileData = fileStream.str();
    std::vector<uint8_t> testFontData(fileData.begin(), fileData.end());

    int32_t count = fontParser.GetFontCount(testFontData);
    EXPECT_GE(count, 0);

    // Test case 2: Empty data
    std::vector<uint8_t> emptyData;
    int32_t count2 = fontParser.GetFontCount(emptyData);
    EXPECT_EQ(count2, 0);
    file.close();
}

/**
 * @tc.name: GetFontCountEdgeCasesTest
 * @tc.desc: Test edge cases for GetFontCount
 * @tc.type: FUNC
 */
HWTEST_F(FontParserTest, GetFontCountEdgeCasesTest, TestSize.Level1)
{
    FontParser fontParser;

    // Test case 1: Very long path (buffer boundary test)
    std::string longPath(4096, 'A');
    int32_t count1 = fontParser.GetFontCount(longPath);
    EXPECT_EQ(count1, 0);

    // Test case 2: Path containing special characters
    std::string specialPath = "/system/fonts/test\0font.ttf";
    int32_t count2 = fontParser.GetFontCount(specialPath);
    EXPECT_EQ(count2, 0);

    // Test case 3: Relative path
    std::string relativePath = "./fonts/test.ttf";
    int32_t count3 = fontParser.GetFontCount(relativePath);
    EXPECT_EQ(count3, 0);
}

/**
 * @tc.name: FontParserIntegrationTest
 * @tc.desc: Font parser integration test
 * @tc.type: FUNC
 */
HWTEST_F(FontParserTest, FontParserIntegrationTest, TestSize.Level1)
{
    FontParser fontParser;

    // Test combined usage of multiple functions
    std::ifstream testFile(TEST_FONT_PATH.c_str());
    if (testFile.is_open()) {
        // 1. Get font count
        int32_t count = fontParser.GetFontCount(TEST_FONT_PATH);
        EXPECT_GT(count, 0);

        // 2. Get font full names
        auto fullNames = fontParser.GetFontFullName(TEST_FONT_PATH);
        EXPECT_FALSE(fullNames.empty());

        // 3. Get Unicode information
        for (int32_t i = 0; i < count; i++) {
            auto unicodeList = fontParser.GetFontTypefaceUnicode(TEST_FONT_PATH, i);
            // For valid font indices, should return non-empty list
            EXPECT_FALSE(unicodeList.empty());
        }

        testFile.close();
    } else {
        GTEST_SKIP();
    }
}

// System language guard class, used to temporarily set system language during tests and restore it after test end
class SystemLanguageGuard {
public:
    explicit SystemLanguageGuard(const std::string& newLang)
    {
        // Save current system language
        originalLang_ = Global::I18n::LocaleConfig::GetSystemLanguage();
        // Set new system language
        Global::I18n::LocaleConfig::SetSystemLanguage(newLang);
    }

    ~SystemLanguageGuard()
    {
        // Restore original system language
        Global::I18n::LocaleConfig::SetSystemLanguage(originalLang_);
    }

    // Disable copy and move
    SystemLanguageGuard(const SystemLanguageGuard&) = delete;
    SystemLanguageGuard& operator=(const SystemLanguageGuard&) = delete;
    SystemLanguageGuard(SystemLanguageGuard&&) = delete;
    SystemLanguageGuard& operator=(SystemLanguageGuard&&) = delete;
    
private:
    std::string originalLang_;
};

/**
 * @tc.name: GetBcpTagListWithZhHansTest
 * @tc.desc: Test getting BCP tag list when system language is zh-Hans
 * @tc.type: FUNC
 */
HWTEST_F(FontParserTest, GetBcpTagListWithZhHansTest, TestSize.Level1)
{
    FontParser fontParser;
    std::string originalLang = Global::I18n::LocaleConfig::GetSystemLanguage();
    // Use SystemLanguageGuard to temporarily set system language to zh-Hans
    {
        SystemLanguageGuard guard("zh-Hans");

        // Get BCP tag list
        auto bcpTagList = fontParser.GetBcpTagList();

        // Verify returned BCP tag list
        std::vector<std::string> expectedTags = {"zh-Hans", "zh", "zh-CN"};

        // Check returned tag list size
        EXPECT_EQ(bcpTagList.size(), expectedTags.size());

        // Check if each tag exists
        for (const auto& expectedTag : expectedTags) {
            bool found = false;
            for (const auto& actualTag : bcpTagList) {
                if (actualTag == expectedTag) {
                    found = true;
                    break;
                }
            }
            EXPECT_TRUE(found);
        }
    }
    // guard destructor, system language automatically restored
    EXPECT_STREQ(originalLang.c_str(), Global::I18n::LocaleConfig::GetSystemLanguage().c_str());
}

/**
 * @tc.name: GetBcpTagListWithEnLatnUSTest
 * @tc.desc: Test getting BCP tag list when system language is en-Latn-US
 * @tc.type: FUNC
 */
HWTEST_F(FontParserTest, GetBcpTagListWithEnLatnUSTest, TestSize.Level1)
{
    FontParser fontParser;
    std::string originalLang = Global::I18n::LocaleConfig::GetSystemLanguage();
    // Use SystemLanguageGuard to temporarily set system language to en-Latn-US
    {
        SystemLanguageGuard guard("en-Latn-US");

        // Get BCP tag list
        auto bcpTagList = fontParser.GetBcpTagList();

        // Verify returned BCP tag list
        // According to g_localeToBcpTable, en-Latn-US should correspond to {"en-US", "en"}
        std::vector<std::string> expectedTags = {"en-US", "en"};

        // Check returned tag list size
        EXPECT_EQ(bcpTagList.size(), expectedTags.size());

        // Check if each tag exists
        for (const auto& expectedTag : expectedTags) {
            bool found = false;
            for (const auto& actualTag : bcpTagList) {
                if (actualTag == expectedTag) {
                    found = true;
                    break;
                }
            }
            EXPECT_TRUE(found);
        }
    }
    // guard destructor, system language automatically restored
    EXPECT_STREQ(originalLang.c_str(), Global::I18n::LocaleConfig::GetSystemLanguage().c_str());
}

/**
 * @tc.name: GetBcpTagListWithUnsupportedLangTest
 * @tc.desc: Test getting BCP tag list when system language is not in mapping table
 * @tc.type: FUNC
 */
HWTEST_F(FontParserTest, GetBcpTagListWithUnsupportedLangTest, TestSize.Level1)
{
    FontParser fontParser;
    std::string originalLang = Global::I18n::LocaleConfig::GetSystemLanguage();
    // Use SystemLanguageGuard to temporarily set system language to non-existent language
    {
        SystemLanguageGuard guard("xx-XX");

        // Get BCP tag list
        auto bcpTagList = fontParser.GetBcpTagList();

        // Verify returned BCP tag list is empty
        EXPECT_TRUE(bcpTagList.empty());
    }
    // guard destructor, system language automatically restored
    EXPECT_STREQ(originalLang.c_str(), Global::I18n::LocaleConfig::GetSystemLanguage().c_str());
}


/**
 * @tc.name: FillFontDescriptorWithLocalInfoWithDifferentSystemLangTest
 * @tc.desc: Test filling font descriptor with different system languages
 * @tc.type: FUNC
 */
HWTEST_F(FontParserTest, FillFontDescriptorWithLocalInfoWithDifferentSystemLangTest, TestSize.Level1)
{
    FontParser fontParser;
    std::string testFontPath = "/system/fonts/HYQiHeiL3.ttf";
    std::string originalLang = Global::I18n::LocaleConfig::GetSystemLanguage();
    std::ifstream testFile(testFontPath.c_str());
    if (!testFile.is_open()) {
        GTEST_SKIP();
    }
    testFile.close();

    auto typeface = Drawing::Typeface::MakeFromFile(testFontPath.c_str());
    ASSERT_NE(typeface, nullptr);
    
    // Test case 1: Fill descriptor when system language is zh-Hans
    {
        SystemLanguageGuard guard("zh-Hans");

        FontParser::FontDescriptor desc1;
        fontParser.FillFontDescriptorWithLocalInfo(typeface, desc1);
        EXPECT_STREQ(desc1.localFamilyName.c_str(), "汉仪旗黑 L3");
    }
    
    // Test case 2: Fill descriptor when system language is en-Latn-US
    {
        SystemLanguageGuard guard("en-Latn-US");

        FontParser::FontDescriptor desc2;
        fontParser.FillFontDescriptorWithLocalInfo(typeface, desc2);
        EXPECT_STREQ(desc2.localFamilyName.c_str(), "HYQiHei L3");
    }
    
    // Test case 3: Fill descriptor when system language is unsupported language
    {
        SystemLanguageGuard guard("xx-XX");

        FontParser::FontDescriptor desc3;
        fontParser.FillFontDescriptorWithLocalInfo(typeface, desc3);
        EXPECT_STREQ(desc3.localFamilyName.c_str(), "HYQiHei L3");
    }
    // guard destructor, system language automatically restored
    EXPECT_STREQ(originalLang.c_str(), Global::I18n::LocaleConfig::GetSystemLanguage().c_str());
}

} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
