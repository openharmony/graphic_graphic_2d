/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"

#include <cstdio>
#include <string>
#include <vector>
#include <memory>

#include "text/font_language_query.h"
#include "text/typeface.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {

class FontLanguageQueryTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};

inline std::string GenerateRelativePathForFontResource(const std::string& ttfName)
{
    std::string prefix = "/system/fonts/";
    return prefix + ttfName;
}

static bool IsFileExists(const std::string& filePath)
{
    auto fp = std::fopen(filePath.c_str(), "rb");
    if (!fp) {
        std::printf("Failed to open file: %s\n", filePath.c_str());
        return false;
    }
    std::fclose(fp);
    return true;
}

static void getDataStream(const std::string& filePath, std::vector<uint8_t>& fontData)
{
    auto fp = std::fopen(filePath.c_str(), "rb");
    if (!fp) {
        std::printf("Failed to open file: %s\n", filePath.c_str());
        return;
    }

    if (std::fseek(fp, 0, SEEK_END) != 0) {
        std::printf("Failed to seek to end of file: %s\n", filePath.c_str());
        std::fclose(fp);
        return;
    }

    int64_t size = std::ftell(fp);
    if (size <= 0) {
        std::printf("Invalid file size: %s", filePath.c_str());
        std::fclose(fp);
        return;
    }

    if (std::fseek(fp, 0, SEEK_SET) != 0) {
        std::printf("Failed to seek to start of file: %s\n", filePath.c_str());
        std::fclose(fp);
        return;
    }

    fontData.resize(static_cast<size_t>(size));
    size_t rsize = std::fread(fontData.data(), 1, static_cast<size_t>(size), fp);
    std::fclose(fp);
    if (rsize != static_cast<size_t>(size)) {
        std::printf("Failed to read entire file: %s, expected=%zu, actual=%zu\n",
            filePath.c_str(), static_cast<size_t>(size), rsize);
        fontData.clear();
        return;
    }
}

/**
 * @tc.name: GenerateFontSupportedLanguagesEmptyTypeface
 * @tc.desc: Test GenerateFontSupportedLanguages
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontLanguageQueryTest, GenerateFontSupportedLanguagesEmptyTypeface, TestSize.Level1)
{
    auto languages = FontLanguageQuery::GenerateFontSupportedLanguages(std::shared_ptr<Typeface>(nullptr));
    EXPECT_TRUE(languages.empty());
}

/**
 * @tc.name: GenerateFontSupportedLanguagesEmptyFontPath
 * @tc.desc: Test GenerateFontSupportedLanguages
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontLanguageQueryTest, GenerateFontSupportedLanguagesEmptyFontPath, TestSize.Level1)
{
    auto languages = FontLanguageQuery::GenerateFontSupportedLanguages(std::string(""));
    EXPECT_TRUE(languages.empty());
}

/**
 * @tc.name: GenerateFontSupportedLanguagesInvalidPath
 * @tc.desc: Test GenerateFontSupportedLanguages
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontLanguageQueryTest, GenerateFontSupportedLanguagesInvalidPath, TestSize.Level1)
{
    std::string invalidPath = "/system/fonts/NotExistFont.ttf";
    auto languages = FontLanguageQuery::GenerateFontSupportedLanguages(invalidPath);
    EXPECT_TRUE(languages.empty());
}

/**
 * @tc.name: GenerateFontSupportedLanguagesEmptyFontData
 * @tc.desc: Test GenerateFontSupportedLanguages
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontLanguageQueryTest, GenerateFontSupportedLanguagesEmptyFontData, TestSize.Level1)
{
    std::vector<uint8_t> emptyData;
    auto languages = FontLanguageQuery::GenerateFontSupportedLanguages(emptyData);
    EXPECT_TRUE(languages.empty());
}

/**
 * @tc.name: GenerateFontSupportedLanguagesInvalidFontData
 * @tc.desc: Test GenerateFontSupportedLanguages
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontLanguageQueryTest, GenerateFontSupportedLanguagesInvalidFontData, TestSize.Level1)
{
    std::vector<uint8_t> invalidData = {1};
    auto languages = FontLanguageQuery::GenerateFontSupportedLanguages(invalidData);
    EXPECT_TRUE(languages.empty());
}

/**
 * @tc.name: GenerateFontSupportedLanguagesHarmonyOSSansByTypeface
 * @tc.desc: Test GenerateFontSupportedLanguages
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontLanguageQueryTest, GenerateFontSupportedLanguagesHarmonyOSSansByTypeface, TestSize.Level1)
{
    std::string ttfName = "HarmonyOS_Sans.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
        if (!IsFileExists(path)) {
        GTEST_SKIP() << "Font file not found: " << path;
    }

    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);

    auto languages = FontLanguageQuery::GenerateFontSupportedLanguages(typeface);
    std::vector<std::string> languagesExp = {"af", "ain", "be", "bg", "bm", "cs", "da", "de", "el", "el-polyton", "en",
        "es", "et", "fi", "fr", "grc", "ha", "hr", "hu", "id", "it", "ja", "ko", "lt", "lv", "mk", "ng", "nl", "no",
        "pl", "pt", "ro", "ru", "sk", "sl", "sr", "sv", "uk", "vi", "za", "zh-Hans", "zh-Hant"};
    EXPECT_EQ(languages, languagesExp);
}

/**
 * @tc.name: GenerateFontSupportedLanguagesHarmonyOSSansByPath
 * @tc.desc: Test GenerateFontSupportedLanguages
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontLanguageQueryTest, GenerateFontSupportedLanguagesHarmonyOSSansByPath, TestSize.Level1)
{
    std::string ttfName = "HarmonyOS_Sans.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
            if (!IsFileExists(path)) {
        GTEST_SKIP() << "Font file not found: " << path;
    }

    auto languages = FontLanguageQuery::GenerateFontSupportedLanguages(path);
    std::vector<std::string> languagesExp = {"af", "ain", "be", "bg", "bm", "cs", "da", "de", "el", "el-polyton", "en",
        "es", "et", "fi", "fr", "grc", "ha", "hr", "hu", "id", "it", "ja", "ko", "lt", "lv", "mk", "ng", "nl", "no",
        "pl", "pt", "ro", "ru", "sk", "sl", "sr", "sv", "uk", "vi", "za", "zh-Hans", "zh-Hant"};
    EXPECT_EQ(languages, languagesExp);
}

/**
 * @tc.name: GenerateFontSupportedLanguagesHarmonyOSSansByVector
 * @tc.desc: Test GenerateFontSupportedLanguages
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontLanguageQueryTest, GenerateFontSupportedLanguagesHarmonyOSSansByVector, TestSize.Level1)
{
    std::string ttfName = "HarmonyOS_Sans.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    std::vector<uint8_t> fontData;
    getDataStream(path, fontData);
    ASSERT_FALSE(fontData.empty());

    auto languages = FontLanguageQuery::GenerateFontSupportedLanguages(fontData);
    std::vector<std::string> languagesExp = {"af", "ain", "be", "bg", "bm", "cs", "da", "de", "el", "el-polyton", "en",
        "es", "et", "fi", "fr", "grc", "ha", "hr", "hu", "id", "it", "ja", "ko", "lt", "lv", "mk", "ng", "nl", "no",
        "pl", "pt", "ro", "ru", "sk", "sl", "sr", "sv", "uk", "vi", "za", "zh-Hans", "zh-Hant"};
    EXPECT_EQ(languages, languagesExp);
}

/**
 * @tc.name: GenerateFontSupportedLanguagesHarmonyOSSansItalicByTypeface
 * @tc.desc: Test GenerateFontSupportedLanguages
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontLanguageQueryTest, GenerateFontSupportedLanguagesHarmonyOSSansItalicByTypeface, TestSize.Level1)
{
    std::string ttfName = "HarmonyOS_Sans_Italic.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
        if (!IsFileExists(path)) {
        GTEST_SKIP() << "Font file not found: " << path;
    }

    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);

    auto languages = FontLanguageQuery::GenerateFontSupportedLanguages(typeface);
    std::vector<std::string> languagesExp = {"af", "be", "bg", "bm", "cs", "da", "de", "el", "el-polyton", "en", "es",
        "et", "fi", "fr", "grc", "ha", "hr", "hu", "id", "it", "ja", "ko", "lt", "lv", "mk", "ng", "nl", "no", "pl",
        "pt", "ro", "ru", "sk", "sl", "sr", "sv", "uk", "vi", "za", "zh-Hans", "zh-Hant"};
    EXPECT_EQ(languages, languagesExp);
}

/**
 * @tc.name: GenerateFontSupportedLanguagesHarmonyOSSansItalicByPath
 * @tc.desc: Test GenerateFontSupportedLanguages
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontLanguageQueryTest, GenerateFontSupportedLanguagesHarmonyOSSansItalicByPath, TestSize.Level1)
{
    std::string ttfName = "HarmonyOS_Sans_Italic.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
        if (!IsFileExists(path)) {
        GTEST_SKIP() << "Font file not found: " << path;
    }

    auto languages = FontLanguageQuery::GenerateFontSupportedLanguages(path);
    std::vector<std::string> languagesExp = {"af", "be", "bg", "bm", "cs", "da", "de", "el", "el-polyton", "en", "es",
        "et", "fi", "fr", "grc", "ha", "hr", "hu", "id", "it", "ja", "ko", "lt", "lv", "mk", "ng", "nl", "no", "pl",
        "pt", "ro", "ru", "sk", "sl", "sr", "sv", "uk", "vi", "za", "zh-Hans", "zh-Hant"};
    EXPECT_EQ(languages, languagesExp);
}

/**
 * @tc.name: GenerateFontSupportedLanguagesHarmonyOSSansItalicByVector
 * @tc.desc:Test GenerateFontSupportedLanguages
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontLanguageQueryTest, GenerateFontSupportedLanguagesHarmonyOSSansItalicByVector, TestSize.Level1)
{
    std::string ttfName = "HarmonyOS_Sans_Italic.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    std::vector<uint8_t> fontData;
    getDataStream(path, fontData);
    ASSERT_FALSE(fontData.empty());

    auto languages = FontLanguageQuery::GenerateFontSupportedLanguages(fontData);
    std::vector<std::string> languagesExp = {"af", "be", "bg", "bm", "cs", "da", "de", "el", "el-polyton", "en", "es",
        "et", "fi", "fr", "grc", "ha", "hr", "hu", "id", "it", "ja", "ko", "lt", "lv", "mk", "ng", "nl", "no", "pl",
        "pt", "ro", "ru", "sk", "sl", "sr", "sv", "uk", "vi", "za", "zh-Hans", "zh-Hant"};
    EXPECT_EQ(languages, languagesExp);
}

/**
 * @tc.name: GenerateFontSupportedLanguagesHarmonyOSSansTCByTypeface
 * @tc.desc: Test GenerateFontSupportedLanguages
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontLanguageQueryTest, GenerateFontSupportedLanguagesHarmonyOSSansTCByTypeface, TestSize.Level1)
{
    std::string ttfName = "HarmonyOS_Sans_TC.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
        if (!IsFileExists(path)) {
        GTEST_SKIP() << "Font file not found: " << path;
    }

    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);

    auto languages = FontLanguageQuery::GenerateFontSupportedLanguages(typeface);
    std::vector<std::string> languagesExp = {"af", "ain", "be", "bg", "bm", "cs", "da", "de", "el", "el-polyton", "en",
        "es", "et", "fi", "fr", "grc", "ha", "hr", "hu", "id", "it", "ja", "ko", "lt", "lv", "mk", "ng", "nl", "no",
        "pl", "pt", "ro", "ru", "sk", "sl", "sr", "sv", "uk", "vi", "za", "zh-Hans", "zh-Hant"};
    EXPECT_EQ(languages, languagesExp);
}

/**
 * @tc.name: GenerateFontSupportedLanguagesHarmonyOSSansTCByPath
 * @tc.desc: Test GenerateFontSupportedLanguages
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontLanguageQueryTest, GenerateFontSupportedLanguagesHarmonyOSSansTCByPath, TestSize.Level1)
{
    std::string ttfName = "HarmonyOS_Sans_TC.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
        if (!IsFileExists(path)) {
        GTEST_SKIP() << "Font file not found: " << path;
    }

    auto languages = FontLanguageQuery::GenerateFontSupportedLanguages(path);
    std::vector<std::string> languagesExp = {"af", "ain", "be", "bg", "bm", "cs", "da", "de", "el", "el-polyton", "en",
        "es", "et", "fi", "fr", "grc", "ha", "hr", "hu", "id", "it", "ja", "ko", "lt", "lv", "mk", "ng", "nl", "no",
        "pl", "pt", "ro", "ru", "sk", "sl", "sr", "sv", "uk", "vi", "za", "zh-Hans", "zh-Hant"};
    EXPECT_EQ(languages, languagesExp);
}

/**
 * @tc.name: GenerateFontSupportedLanguagesHarmonyOSSansTCByVector
 * @tc.desc: Test GenerateFontSupportedLanguages
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontLanguageQueryTest, GenerateFontSupportedLanguagesHarmonyOSSansTCByVector, TestSize.Level1)
{
    std::string ttfName = "HarmonyOS_Sans_TC.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    std::vector<uint8_t> fontData;
    getDataStream(path, fontData);
    ASSERT_FALSE(fontData.empty());

    auto languages = FontLanguageQuery::GenerateFontSupportedLanguages(fontData);
    std::vector<std::string> languagesExp = {"af", "ain", "be", "bg", "bm", "cs", "da", "de", "el", "el-polyton", "en",
        "es", "et", "fi", "fr", "grc", "ha", "hr", "hu", "id", "it", "ja", "ko", "lt", "lv", "mk", "ng", "nl", "no",
        "pl", "pt", "ro", "ru", "sk", "sl", "sr", "sv", "uk", "vi", "za", "zh-Hans", "zh-Hant"};
    EXPECT_EQ(languages, languagesExp);
}

/**
 * @tc.name: GenerateFontSupportedLanguagesNotoSansSylotiNagriRegularByTypeface
 * @tc.desc: Test GenerateFontSupportedLanguages
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontLanguageQueryTest, GenerateFontSupportedLanguagesNotoSansSylotiNagriRegularByTypeface, TestSize.Level1)
{
    std::string ttfName = "NotoSansSylotiNagri-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
        if (!IsFileExists(path)) {
        GTEST_SKIP() << "Font file not found: " << path;
    }

    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);

    auto languages = FontLanguageQuery::GenerateFontSupportedLanguages(typeface);
    std::vector<std::string> languagesExp = {"af", "as", "bh", "bm", "bn", "cs", "da", "de", "en", "es", "et", "fi",
        "fr", "ha", "hi", "hr", "hu", "id", "it", "kok", "lt", "lv", "mai", "mr", "ne", "ng", "nl", "no", "pl", "pt",
        "ro", "sa", "sk", "sl", "sv", "syl-Sylo", "vi", "za"};
    EXPECT_EQ(languages, languagesExp);
}

/**
 * @tc.name: GenerateFontSupportedLanguagesNotoSansSylotiNagriRegularByPath
 * @tc.desc: Test GenerateFontSupportedLanguages
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontLanguageQueryTest, GenerateFontSupportedLanguagesNotoSansSylotiNagriRegularByPath, TestSize.Level1)
{
    std::string ttfName = "NotoSansSylotiNagri-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
        if (!IsFileExists(path)) {
        GTEST_SKIP() << "Font file not found: " << path;
    }

    auto languages = FontLanguageQuery::GenerateFontSupportedLanguages(path);
    std::vector<std::string> languagesExp = {"af", "as", "bh", "bm", "bn", "cs", "da", "de", "en", "es", "et", "fi",
        "fr", "ha", "hi", "hr", "hu", "id", "it", "kok", "lt", "lv", "mai", "mr", "ne", "ng", "nl", "no", "pl", "pt",
        "ro", "sa", "sk", "sl", "sv", "syl-Sylo", "vi", "za"};
    EXPECT_EQ(languages, languagesExp);
}

/**
 * @tc.name: GenerateFontSupportedLanguagesNotoSansSylotiNagriRegularByVector
 * @tc.desc: Test GenerateFontSupportedLanguages
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontLanguageQueryTest, GenerateFontSupportedLanguagesNotoSansSylotiNagriRegularByVector, TestSize.Level1)
{
    std::string ttfName = "NotoSansSylotiNagri-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    std::vector<uint8_t> fontData;
    getDataStream(path, fontData);
    ASSERT_FALSE(fontData.empty());

    auto languages = FontLanguageQuery::GenerateFontSupportedLanguages(fontData);
    std::vector<std::string> languagesExp = {"af", "as", "bh", "bm", "bn", "cs", "da", "de", "en", "es", "et", "fi",
        "fr", "ha", "hi", "hr", "hu", "id", "it", "kok", "lt", "lv", "mai", "mr", "ne", "ng", "nl", "no", "pl", "pt",
        "ro", "sa", "sk", "sl", "sv", "syl-Sylo", "vi", "za"};
    EXPECT_EQ(languages, languagesExp);
}

/**
 * @tc.name: GenerateFontSupportedLanguagesDejaVuMathTeXGyreByTypeface
 * @tc.desc: Test GenerateFontSupportedLanguages
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontLanguageQueryTest, GenerateFontSupportedLanguagesDejaVuMathTeXGyreByTypeface, TestSize.Level1)
{
    std::string ttfName = "DejaVuMathTeXGyre.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
        if (!IsFileExists(path)) {
        GTEST_SKIP() << "Font file not found: " << path;
    }

    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);

    auto languages = FontLanguageQuery::GenerateFontSupportedLanguages(typeface);
    std::vector<std::string> languagesExp = {"af", "ar", "bm", "cs", "da", "de", "el", "en", "es", "et", "fi", "fr",
        "grc", "ha", "he", "hr", "hu", "id", "it", "ja", "ko", "lt", "lv", "ng", "nl", "no", "pl", "pt", "ro", "sk",
        "sl", "sv", "th", "vi", "za", "zh-Hans", "zh-Hant"};
    EXPECT_EQ(languages, languagesExp);
}

/**
 * @tc.name: GenerateFontSupportedLanguagesDejaVuMathTeXGyreByPath
 * @tc.desc: Test GenerateFontSupportedLanguages
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontLanguageQueryTest, GenerateFontSupportedLanguagesDejaVuMathTeXGyreByPath, TestSize.Level1)
{
    std::string ttfName = "DejaVuMathTeXGyre.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
        if (!IsFileExists(path)) {
        GTEST_SKIP() << "Font file not found: " << path;
    }

    auto languages = FontLanguageQuery::GenerateFontSupportedLanguages(path);
    std::vector<std::string> languagesExp = {"af", "ar", "bm", "cs", "da", "de", "el", "en", "es", "et", "fi", "fr",
        "grc", "ha", "he", "hr", "hu", "id", "it", "ja", "ko", "lt", "lv", "ng", "nl", "no", "pl", "pt", "ro", "sk",
        "sl", "sv", "th", "vi", "za", "zh-Hans", "zh-Hant"};
    EXPECT_EQ(languages, languagesExp);
}

/**
 * @tc.name: GenerateFontSupportedLanguagesDejaVuMathTeXGyreByVector
 * @tc.desc: Test GenerateFontSupportedLanguages
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontLanguageQueryTest, GenerateFontSupportedLanguagesDejaVuMathTeXGyreByVector, TestSize.Level1)
{
    std::string ttfName = "DejaVuMathTeXGyre.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    std::vector<uint8_t> fontData;
    getDataStream(path, fontData);
    ASSERT_FALSE(fontData.empty());

    auto languages = FontLanguageQuery::GenerateFontSupportedLanguages(fontData);
    std::vector<std::string> languagesExp = {"af", "ar", "bm", "cs", "da", "de", "el", "en", "es", "et", "fi", "fr",
        "grc", "ha", "he", "hr", "hu", "id", "it", "ja", "ko", "lt", "lv", "ng", "nl", "no", "pl", "pt", "ro", "sk",
        "sl", "sv", "th", "vi", "za", "zh-Hans", "zh-Hant"};
    EXPECT_EQ(languages, languagesExp);
}

/**
 * @tc.name: GenerateFontSupportedLanguagesHMSymbolVFByTypeface
 * @tc.desc: Test GenerateFontSupportedLanguages
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontLanguageQueryTest, GenerateFontSupportedLanguagesHMSymbolVFByTypeface, TestSize.Level1)
{
    std::string ttfName = "HMSymbolVF.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
        if (!IsFileExists(path)) {
        GTEST_SKIP() << "Font file not found: " << path;
    }

    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);

    auto languages = FontLanguageQuery::GenerateFontSupportedLanguages(typeface);
    std::vector<std::string> languagesExp = {"da", "de", "en", "es", "fi", "fr", "id", "it", "nl", "no", "pt", "sv"};
    EXPECT_EQ(languages, languagesExp);
}

/**
 * @tc.name: GenerateFontSupportedLanguagesHMSymbolVFByPath
 * @tc.desc: Test GenerateFontSupportedLanguages
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontLanguageQueryTest, GenerateFontSupportedLanguagesHMSymbolVFByPath, TestSize.Level1)
{
    std::string ttfName = "HMSymbolVF.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
        if (!IsFileExists(path)) {
        GTEST_SKIP() << "Font file not found: " << path;
    }

    auto languages = FontLanguageQuery::GenerateFontSupportedLanguages(path);
    std::vector<std::string> languagesExp = {"da", "de", "en", "es", "fi", "fr", "id", "it", "nl", "no", "pt", "sv"};
    EXPECT_EQ(languages, languagesExp);
}

/**
 * @tc.name: GenerateFontSupportedLanguagesHMSymbolVFByVector
 * @tc.desc: Test GenerateFontSupportedLanguages
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontLanguageQueryTest, GenerateFontSupportedLanguagesHMSymbolVFByVector, TestSize.Level1)
{
    std::string ttfName = "HMSymbolVF.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    std::vector<uint8_t> fontData;
    getDataStream(path, fontData);
    ASSERT_FALSE(fontData.empty());

    auto languages = FontLanguageQuery::GenerateFontSupportedLanguages(fontData);
    std::vector<std::string> languagesExp = {"da", "de", "en", "es", "fi", "fr", "id", "it", "nl", "no", "pt", "sv"};
    EXPECT_EQ(languages, languagesExp);
}

/**
 * @tc.name: GenerateFontSupportedLanguagesUnique
 * @tc.desc: Test GenerateFontSupportedLanguages
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontLanguageQueryTest, GenerateFontSupportedLanguagesUnique, TestSize.Level1)
{
    std::string ttfName = "HarmonyOS_Sans.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
        if (!IsFileExists(path)) {
        GTEST_SKIP() << "Font file not found: " << path;
    }

    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);

    auto languages = FontLanguageQuery::GenerateFontSupportedLanguages(typeface);

    // Check for uniqueness by using a set
    std::set<std::string> uniqueLanguages(languages.begin(), languages.end());
    EXPECT_EQ(uniqueLanguages.size(), languages.size());
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS