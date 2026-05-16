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

#include "text/font_properties_query.h"
#include "text/typeface.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {

class FontPropertiesQueryTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};

static std::string GenerateRelativePathForFontResource(const std::string& ttfName)
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
 * @tc.name: GenerateFontPropertiesEmptyTypeface
 * @tc.desc: Test GenerateFontProperties
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontPropertiesQueryTest, GenerateFontPropertiesEmptyTypeface, TestSize.Level1)
{
    auto properties = FontPropertiesQuery::GenerateFontProperties(std::shared_ptr<Typeface>(nullptr));
    EXPECT_EQ(properties.weight, FontStyle::NORMAL_WEIGHT);
    EXPECT_EQ(properties.width, FontStyle::NORMAL_WIDTH);
    EXPECT_EQ(properties.slant, FontStyle::UPRIGHT_SLANT);
    EXPECT_FALSE(properties.bold);
    EXPECT_FALSE(properties.italic);
    EXPECT_FALSE(properties.monospace);
    EXPECT_FALSE(properties.colorglyphs);
}

/**
 * @tc.name: GenerateFontPropertiesEmptyFontPath
 * @tc.desc: Test GenerateFontProperties
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontPropertiesQueryTest, GenerateFontPropertiesEmptyFontPath, TestSize.Level1)
{
    auto properties = FontPropertiesQuery::GenerateFontProperties(std::string(""));
    EXPECT_EQ(properties.weight, FontStyle::NORMAL_WEIGHT);
    EXPECT_EQ(properties.width, FontStyle::NORMAL_WIDTH);
    EXPECT_EQ(properties.slant, FontStyle::UPRIGHT_SLANT);
    EXPECT_FALSE(properties.bold);
    EXPECT_FALSE(properties.italic);
    EXPECT_FALSE(properties.monospace);
    EXPECT_FALSE(properties.colorglyphs);
}

/**
 * @tc.name: GenerateFontPropertiesInvalidPath
 * @tc.desc: Test GenerateFontProperties
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontPropertiesQueryTest, GenerateFontPropertiesInvalidPath, TestSize.Level1)
{
    std::string invalidPath = "/system/fonts/NotExistFont.ttf";
    auto properties = FontPropertiesQuery::GenerateFontProperties(invalidPath);
    EXPECT_EQ(properties.weight, FontStyle::NORMAL_WEIGHT);
    EXPECT_EQ(properties.width, FontStyle::NORMAL_WIDTH);
    EXPECT_EQ(properties.slant, FontStyle::UPRIGHT_SLANT);
    EXPECT_FALSE(properties.bold);
    EXPECT_FALSE(properties.italic);
    EXPECT_FALSE(properties.monospace);
    EXPECT_FALSE(properties.colorglyphs);
}

/**
 * @tc.name: GenerateFontPropertiesEmptyFontData
 * @tc.desc: Test GenerateFontProperties
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontPropertiesQueryTest, GenerateFontPropertiesEmptyFontData, TestSize.Level1)
{
    std::vector<uint8_t> emptyData;
    auto properties = FontPropertiesQuery::GenerateFontProperties(emptyData);
    EXPECT_EQ(properties.weight, FontStyle::NORMAL_WEIGHT);
    EXPECT_EQ(properties.width, FontStyle::NORMAL_WIDTH);
    EXPECT_EQ(properties.slant, FontStyle::UPRIGHT_SLANT);
    EXPECT_FALSE(properties.bold);
    EXPECT_FALSE(properties.italic);
    EXPECT_FALSE(properties.monospace);
    EXPECT_FALSE(properties.colorglyphs);
}

/**
 * @tc.name: GenerateFontPropertiesHarmonyOSSansByTypeface
 * @tc.desc: Test GenerateFontProperties
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontPropertiesQueryTest, GenerateFontPropertiesHarmonyOSSansByTypeface, TestSize.Level1)
{
    std::string ttfName = "HarmonyOS_Sans.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    if (!IsFileExists(path)) {
        GTEST_SKIP() << "Font file not found: " << path;
    }

    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);

    auto properties = FontPropertiesQuery::GenerateFontProperties(typeface);
    EXPECT_EQ(properties.weight, FontStyle::NORMAL_WEIGHT);
    EXPECT_EQ(properties.width, FontStyle::NORMAL_WIDTH);
    EXPECT_EQ(properties.slant, FontStyle::UPRIGHT_SLANT);
    EXPECT_FALSE(properties.bold);
    EXPECT_FALSE(properties.italic);
    EXPECT_FALSE(properties.monospace);
    EXPECT_FALSE(properties.colorglyphs);
}

/**
 * @tc.name: GenerateFontPropertiesHarmonyOSSansByPath
 * @tc.desc: Test GenerateFontProperties
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontPropertiesQueryTest, GenerateFontPropertiesHarmonyOSSansByPath, TestSize.Level1)
{
    std::string ttfName = "HarmonyOS_Sans.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    if (!IsFileExists(path)) {
        GTEST_SKIP() << "Font file not found: " << path;
    }

    auto properties = FontPropertiesQuery::GenerateFontProperties(path);
    EXPECT_EQ(properties.weight, FontStyle::NORMAL_WEIGHT);
    EXPECT_EQ(properties.width, FontStyle::NORMAL_WIDTH);
    EXPECT_EQ(properties.slant, FontStyle::UPRIGHT_SLANT);
    EXPECT_FALSE(properties.bold);
    EXPECT_FALSE(properties.italic);
    EXPECT_FALSE(properties.monospace);
    EXPECT_FALSE(properties.colorglyphs);
}

/**
 * @tc.name: GenerateFontPropertiesHarmonyOSSansByVector
 * @tc.desc: Test GenerateFontProperties
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontPropertiesQueryTest, GenerateFontPropertiesHarmonyOSSansByVector, TestSize.Level1)
{
    std::string ttfName = "HarmonyOS_Sans.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    std::vector<uint8_t> fontData;
    getDataStream(path, fontData);
    ASSERT_FALSE(fontData.empty());

    auto properties = FontPropertiesQuery::GenerateFontProperties(fontData);
    EXPECT_EQ(properties.weight, FontStyle::NORMAL_WEIGHT);
    EXPECT_EQ(properties.width, FontStyle::NORMAL_WIDTH);
    EXPECT_EQ(properties.slant, FontStyle::UPRIGHT_SLANT);
    EXPECT_FALSE(properties.bold);
    EXPECT_FALSE(properties.italic);
    EXPECT_FALSE(properties.monospace);
    EXPECT_FALSE(properties.colorglyphs);
}

/**
 * @tc.name: GenerateFontPropertiesNotoSansTeluguUIByTypeface
 * @tc.desc: Test GenerateFontProperties
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontPropertiesQueryTest, GenerateFontPropertiesNotoSansTeluguUIByTypeface, TestSize.Level1)
{
    std::string ttfName = "NotoSansTeluguUI-Medium.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    if (!IsFileExists(path)) {
        GTEST_SKIP() << "Font file not found: " << path;
    }

    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);

    auto properties = FontPropertiesQuery::GenerateFontProperties(typeface);
    EXPECT_EQ(properties.weight, FontStyle::MEDIUM_WEIGHT);
    EXPECT_EQ(properties.width, FontStyle::NORMAL_WIDTH);
    EXPECT_EQ(properties.slant, FontStyle::UPRIGHT_SLANT);
    EXPECT_FALSE(properties.bold);
    EXPECT_FALSE(properties.italic);
    EXPECT_FALSE(properties.monospace);
    EXPECT_FALSE(properties.colorglyphs);
}

/**
 * @tc.name: GenerateFontPropertiesNotoSansTeluguUIByPath
 * @tc.desc: Test GenerateFontProperties
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontPropertiesQueryTest, GenerateFontPropertiesNotoSansTeluguUIByPath, TestSize.Level1)
{
    std::string ttfName = "NotoSansTeluguUI-Medium.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    if (!IsFileExists(path)) {
        GTEST_SKIP() << "Font file not found: " << path;
    }

    auto properties = FontPropertiesQuery::GenerateFontProperties(path);
    EXPECT_EQ(properties.weight, FontStyle::MEDIUM_WEIGHT);
    EXPECT_EQ(properties.width, FontStyle::NORMAL_WIDTH);
    EXPECT_EQ(properties.slant, FontStyle::UPRIGHT_SLANT);
    EXPECT_FALSE(properties.bold);
    EXPECT_FALSE(properties.italic);
    EXPECT_FALSE(properties.monospace);
    EXPECT_FALSE(properties.colorglyphs);
}

/**
 * @tc.name: GenerateFontPropertiesNotoSansTeluguUIByVector
 * @tc.desc: Test GenerateFontProperties
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontPropertiesQueryTest, GenerateFontPropertiesNotoSansTeluguUIByVector, TestSize.Level1)
{
    std::string ttfName = "NotoSansTeluguUI-Medium.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    std::vector<uint8_t> fontData;
    getDataStream(path, fontData);
    ASSERT_FALSE(fontData.empty());

    auto properties = FontPropertiesQuery::GenerateFontProperties(fontData);
    EXPECT_EQ(properties.weight, FontStyle::MEDIUM_WEIGHT);
    EXPECT_EQ(properties.width, FontStyle::NORMAL_WIDTH);
    EXPECT_EQ(properties.slant, FontStyle::UPRIGHT_SLANT);
    EXPECT_FALSE(properties.bold);
    EXPECT_FALSE(properties.italic);
    EXPECT_FALSE(properties.monospace);
    EXPECT_FALSE(properties.colorglyphs);
}

/**
 * @tc.name: GenerateFontPropertiesHarmonyOSSansItalicByTypeface
 * @tc.desc: Test GenerateFontProperties
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontPropertiesQueryTest, GenerateFontPropertiesHarmonyOSSansItalicByTypeface, TestSize.Level1)
{
    std::string ttfName = "HarmonyOS_Sans_Italic.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    if (!IsFileExists(path)) {
        GTEST_SKIP() << "Font file not found: " << path;
    }

    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);

    auto properties = FontPropertiesQuery::GenerateFontProperties(typeface);
    EXPECT_EQ(properties.weight, FontStyle::NORMAL_WEIGHT);
    EXPECT_EQ(properties.width, FontStyle::NORMAL_WIDTH);
    EXPECT_EQ(properties.slant, FontStyle::ITALIC_SLANT);
    EXPECT_FALSE(properties.bold);
    EXPECT_TRUE(properties.italic);
    EXPECT_FALSE(properties.monospace);
    EXPECT_FALSE(properties.colorglyphs);
}

/**
 * @tc.name: GenerateFontPropertiesHarmonyOSSansItalicByPath
 * @tc.desc: Test GenerateFontProperties
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontPropertiesQueryTest, GenerateFontPropertiesHarmonyOSSansItalicByPath, TestSize.Level1)
{
    std::string ttfName = "HarmonyOS_Sans_Italic.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    if (!IsFileExists(path)) {
        GTEST_SKIP() << "Font file not found: " << path;
    }

    auto properties = FontPropertiesQuery::GenerateFontProperties(path);
    EXPECT_EQ(properties.weight, FontStyle::NORMAL_WEIGHT);
    EXPECT_EQ(properties.width, FontStyle::NORMAL_WIDTH);
    EXPECT_EQ(properties.slant, FontStyle::ITALIC_SLANT);
    EXPECT_FALSE(properties.bold);
    EXPECT_TRUE(properties.italic);
    EXPECT_FALSE(properties.monospace);
    EXPECT_FALSE(properties.colorglyphs);
}

/**
 * @tc.name: GenerateFontPropertiesHarmonyOSSansItalicByVector
 * @tc.desc: Test GenerateFontProperties
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontPropertiesQueryTest, GenerateFontPropertiesHarmonyOSSansItalicByVector, TestSize.Level1)
{
    std::string ttfName = "HarmonyOS_Sans_Italic.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    std::vector<uint8_t> fontData;
    getDataStream(path, fontData);
    ASSERT_FALSE(fontData.empty());

    auto properties = FontPropertiesQuery::GenerateFontProperties(fontData);
    EXPECT_EQ(properties.weight, FontStyle::NORMAL_WEIGHT);
    EXPECT_EQ(properties.width, FontStyle::NORMAL_WIDTH);
    EXPECT_EQ(properties.slant, FontStyle::ITALIC_SLANT);
    EXPECT_FALSE(properties.bold);
    EXPECT_TRUE(properties.italic);
    EXPECT_FALSE(properties.monospace);
    EXPECT_FALSE(properties.colorglyphs);
}

/**
 * @tc.name: GenerateFontPropertiesHarmonyOSSansTCByTypeface
 * @tc.desc: Test GenerateFontProperties
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontPropertiesQueryTest, GenerateFontPropertiesHarmonyOSSansTCByTypeface, TestSize.Level1)
{
    std::string ttfName = "HarmonyOS_Sans_TC.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    if (!IsFileExists(path)) {
        GTEST_SKIP() << "Font file not found: " << path;
    }

    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);

    auto properties = FontPropertiesQuery::GenerateFontProperties(typeface);
    EXPECT_EQ(properties.weight, FontStyle::NORMAL_WEIGHT);
    EXPECT_EQ(properties.width, FontStyle::NORMAL_WIDTH);
    EXPECT_EQ(properties.slant, FontStyle::UPRIGHT_SLANT);
    EXPECT_FALSE(properties.bold);
    EXPECT_FALSE(properties.italic);
    EXPECT_FALSE(properties.monospace);
    EXPECT_FALSE(properties.colorglyphs);
}

/**
 * @tc.name: GenerateFontPropertiesHarmonyOSSansTCByPath
 * @tc.desc: Test GenerateFontProperties
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontPropertiesQueryTest, GenerateFontPropertiesHarmonyOSSansTCByPath, TestSize.Level1)
{
    std::string ttfName = "HarmonyOS_Sans_TC.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    if (!IsFileExists(path)) {
        GTEST_SKIP() << "Font file not found: " << path;
    }

    auto properties = FontPropertiesQuery::GenerateFontProperties(path);
    EXPECT_EQ(properties.weight, FontStyle::NORMAL_WEIGHT);
    EXPECT_EQ(properties.width, FontStyle::NORMAL_WIDTH);
    EXPECT_EQ(properties.slant, FontStyle::UPRIGHT_SLANT);
    EXPECT_FALSE(properties.bold);
    EXPECT_FALSE(properties.italic);
    EXPECT_FALSE(properties.monospace);
    EXPECT_FALSE(properties.colorglyphs);
}

/**
 * @tc.name: GenerateFontPropertiesHarmonyOSSansTCByVector
 * @tc.desc: Test GenerateFontProperties
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontPropertiesQueryTest, GenerateFontPropertiesHarmonyOSSansTCByVector, TestSize.Level1)
{
    std::string ttfName = "HarmonyOS_Sans_TC.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    std::vector<uint8_t> fontData;
    getDataStream(path, fontData);
    ASSERT_FALSE(fontData.empty());

    auto properties = FontPropertiesQuery::GenerateFontProperties(fontData);
    EXPECT_EQ(properties.weight, FontStyle::NORMAL_WEIGHT);
    EXPECT_EQ(properties.width, FontStyle::NORMAL_WIDTH);
    EXPECT_EQ(properties.slant, FontStyle::UPRIGHT_SLANT);
    EXPECT_FALSE(properties.bold);
    EXPECT_FALSE(properties.italic);
    EXPECT_FALSE(properties.monospace);
    EXPECT_FALSE(properties.colorglyphs);
}

/**
 * @tc.name: GenerateFontPropertiesNotoSansSylotiNagriByTypeface
 * @tc.desc: Test GenerateFontProperties
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontPropertiesQueryTest, GenerateFontPropertiesNotoSansSylotiNagriByTypeface, TestSize.Level1)
{
    std::string ttfName = "NotoSansSylotiNagri-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    if (!IsFileExists(path)) {
        GTEST_SKIP() << "Font file not found: " << path;
    }

    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);

    auto properties = FontPropertiesQuery::GenerateFontProperties(typeface);
    EXPECT_EQ(properties.weight, FontStyle::NORMAL_WEIGHT);
    EXPECT_EQ(properties.width, FontStyle::NORMAL_WIDTH);
    EXPECT_EQ(properties.slant, FontStyle::UPRIGHT_SLANT);
    EXPECT_FALSE(properties.bold);
    EXPECT_FALSE(properties.italic);
    EXPECT_FALSE(properties.monospace);
    EXPECT_FALSE(properties.colorglyphs);
}

/**
 * @tc.name: GenerateFontPropertiesNotoSansSylotiNagriByPath
 * @tc.desc: Test GenerateFontProperties
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontPropertiesQueryTest, GenerateFontPropertiesNotoSansSylotiNagriByPath, TestSize.Level1)
{
    std::string ttfName = "NotoSansSylotiNagri-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    if (!IsFileExists(path)) {
        GTEST_SKIP() << "Font file not found: " << path;
    }

    auto properties = FontPropertiesQuery::GenerateFontProperties(path);
    EXPECT_EQ(properties.weight, FontStyle::NORMAL_WEIGHT);
    EXPECT_EQ(properties.width, FontStyle::NORMAL_WIDTH);
    EXPECT_EQ(properties.slant, FontStyle::UPRIGHT_SLANT);
    EXPECT_FALSE(properties.bold);
    EXPECT_FALSE(properties.italic);
    EXPECT_FALSE(properties.monospace);
    EXPECT_FALSE(properties.colorglyphs);
}

/**
 * @tc.name: GenerateFontPropertiesNotoSansSylotiNagriByVector
 * @tc.desc: Test GenerateFontProperties
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontPropertiesQueryTest, GenerateFontPropertiesNotoSansSylotiNagriByVector, TestSize.Level1)
{
    std::string ttfName = "NotoSansSylotiNagri-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    std::vector<uint8_t> fontData;
    getDataStream(path, fontData);
    ASSERT_FALSE(fontData.empty());

    auto properties = FontPropertiesQuery::GenerateFontProperties(fontData);
    EXPECT_EQ(properties.weight, FontStyle::NORMAL_WEIGHT);
    EXPECT_EQ(properties.width, FontStyle::NORMAL_WIDTH);
    EXPECT_EQ(properties.slant, FontStyle::UPRIGHT_SLANT);
    EXPECT_FALSE(properties.bold);
    EXPECT_FALSE(properties.italic);
    EXPECT_FALSE(properties.monospace);
    EXPECT_FALSE(properties.colorglyphs);
}

/**
 * @tc.name: GenerateFontPropertiesDejaVuMathTeXGyreByTypeface
 * @tc.desc: Test GenerateFontProperties
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontPropertiesQueryTest, GenerateFontPropertiesDejaVuMathTeXGyreByTypeface, TestSize.Level1)
{
    std::string ttfName = "DejaVuMathTeXGyre.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    if (!IsFileExists(path)) {
        GTEST_SKIP() << "Font file not found: " << path;
    }

    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);

    auto properties = FontPropertiesQuery::GenerateFontProperties(typeface);
    EXPECT_EQ(properties.weight, FontStyle::NORMAL_WEIGHT);
    EXPECT_EQ(properties.width, FontStyle::NORMAL_WIDTH);
    EXPECT_EQ(properties.slant, FontStyle::UPRIGHT_SLANT);
    EXPECT_FALSE(properties.bold);
    EXPECT_FALSE(properties.italic);
    EXPECT_FALSE(properties.monospace);
    EXPECT_FALSE(properties.colorglyphs);
}

/**
 * @tc.name: GenerateFontPropertiesDejaVuMathTeXGyreByPath
 * @tc.desc: Test GenerateFontProperties
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontPropertiesQueryTest, GenerateFontPropertiesDejaVuMathTeXGyreByPath, TestSize.Level1)
{
    std::string ttfName = "DejaVuMathTeXGyre.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    if (!IsFileExists(path)) {
        GTEST_SKIP() << "Font file not found: " << path;
    }

    auto properties = FontPropertiesQuery::GenerateFontProperties(path);
    EXPECT_EQ(properties.weight, FontStyle::NORMAL_WEIGHT);
    EXPECT_EQ(properties.width, FontStyle::NORMAL_WIDTH);
    EXPECT_EQ(properties.slant, FontStyle::UPRIGHT_SLANT);
    EXPECT_FALSE(properties.bold);
    EXPECT_FALSE(properties.italic);
    EXPECT_FALSE(properties.monospace);
    EXPECT_FALSE(properties.colorglyphs);
}

/**
 * @tc.name: GenerateFontPropertiesDejaVuMathTeXGyreByVector
 * @tc.desc: Test GenerateFontProperties
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontPropertiesQueryTest, GenerateFontPropertiesDejaVuMathTeXGyreByVector, TestSize.Level1)
{
    std::string ttfName = "DejaVuMathTeXGyre.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    std::vector<uint8_t> fontData;
    getDataStream(path, fontData);
    ASSERT_FALSE(fontData.empty());

    auto properties = FontPropertiesQuery::GenerateFontProperties(fontData);
    EXPECT_EQ(properties.weight, FontStyle::NORMAL_WEIGHT);
    EXPECT_EQ(properties.width, FontStyle::NORMAL_WIDTH);
    EXPECT_EQ(properties.slant, FontStyle::UPRIGHT_SLANT);
    EXPECT_FALSE(properties.bold);
    EXPECT_FALSE(properties.italic);
    EXPECT_FALSE(properties.monospace);
    EXPECT_FALSE(properties.colorglyphs);
}

/**
 * @tc.name: GenerateFontPropertiesHMOSColorEmojiFlagsByTypeface
 * @tc.desc: Test GenerateFontProperties
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontPropertiesQueryTest, GenerateFontPropertiesHMOSColorEmojiFlagsByTypeface, TestSize.Level1)
{
    std::string ttfName = "HMOSColorEmojiFlags.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    if (!IsFileExists(path)) {
        GTEST_SKIP() << "Font file not found: " << path;
    }

    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);

    auto properties = FontPropertiesQuery::GenerateFontProperties(typeface);
    EXPECT_EQ(properties.weight, FontStyle::NORMAL_WEIGHT);
    EXPECT_EQ(properties.width, FontStyle::NORMAL_WIDTH);
    EXPECT_EQ(properties.slant, FontStyle::UPRIGHT_SLANT);
    EXPECT_FALSE(properties.bold);
    EXPECT_FALSE(properties.italic);
    EXPECT_TRUE(properties.monospace);
    EXPECT_FALSE(properties.colorglyphs);
}

/**
 * @tc.name: GenerateFontPropertiesHMOSColorEmojiFlagsByPath
 * @tc.desc: Test GenerateFontProperties
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontPropertiesQueryTest, GenerateFontPropertiesHMOSColorEmojiFlagsByPath, TestSize.Level1)
{
    std::string ttfName = "HMOSColorEmojiFlags.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    if (!IsFileExists(path)) {
        GTEST_SKIP() << "Font file not found: " << path;
    }

    auto properties = FontPropertiesQuery::GenerateFontProperties(path);
    EXPECT_EQ(properties.weight, FontStyle::NORMAL_WEIGHT);
    EXPECT_EQ(properties.width, FontStyle::NORMAL_WIDTH);
    EXPECT_EQ(properties.slant, FontStyle::UPRIGHT_SLANT);
    EXPECT_FALSE(properties.bold);
    EXPECT_FALSE(properties.italic);
    EXPECT_TRUE(properties.monospace);
    EXPECT_FALSE(properties.colorglyphs);
}

/**
 * @tc.name: GenerateFontPropertiesHMOSColorEmojiFlagsByVector
 * @tc.desc: Test GenerateFontProperties
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontPropertiesQueryTest, GenerateFontPropertiesHMOSColorEmojiFlagsByVector, TestSize.Level1)
{
    std::string ttfName = "HMOSColorEmojiFlags.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    std::vector<uint8_t> fontData;
    getDataStream(path, fontData);
    ASSERT_FALSE(fontData.empty());

    auto properties = FontPropertiesQuery::GenerateFontProperties(fontData);
    EXPECT_EQ(properties.weight, FontStyle::NORMAL_WEIGHT);
    EXPECT_EQ(properties.width, FontStyle::NORMAL_WIDTH);
    EXPECT_EQ(properties.slant, FontStyle::UPRIGHT_SLANT);
    EXPECT_FALSE(properties.bold);
    EXPECT_FALSE(properties.italic);
    EXPECT_TRUE(properties.monospace);
    EXPECT_FALSE(properties.colorglyphs);
}

/**
 * @tc.name: GenerateFontPropertiesHMOSColorEmojiCompatByTypeface
 * @tc.desc: Test GenerateFontProperties
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontPropertiesQueryTest, GenerateFontPropertiesHMOSColorEmojiCompatByTypeface, TestSize.Level1)
{
    std::string ttfName = "HMOSColorEmojiCompat.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    if (!IsFileExists(path)) {
        GTEST_SKIP() << "Font file not found: " << path;
    }

    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);

    auto properties = FontPropertiesQuery::GenerateFontProperties(typeface);
    EXPECT_EQ(properties.weight, FontStyle::NORMAL_WEIGHT);
    EXPECT_EQ(properties.width, FontStyle::NORMAL_WIDTH);
    EXPECT_EQ(properties.slant, FontStyle::UPRIGHT_SLANT);
    EXPECT_FALSE(properties.bold);
    EXPECT_FALSE(properties.italic);
    EXPECT_TRUE(properties.monospace);
    EXPECT_FALSE(properties.colorglyphs);
}

/**
 * @tc.name: GenerateFontPropertiesHMOSColorEmojiCompatByPath
 * @tc.desc: Test GenerateFontProperties
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontPropertiesQueryTest, GenerateFontPropertiesHMOSColorEmojiCompatByPath, TestSize.Level1)
{
    std::string ttfName = "HMOSColorEmojiCompat.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    if (!IsFileExists(path)) {
        GTEST_SKIP() << "Font file not found: " << path;
    }

    auto properties = FontPropertiesQuery::GenerateFontProperties(path);
    EXPECT_EQ(properties.weight, FontStyle::NORMAL_WEIGHT);
    EXPECT_EQ(properties.width, FontStyle::NORMAL_WIDTH);
    EXPECT_EQ(properties.slant, FontStyle::UPRIGHT_SLANT);
    EXPECT_FALSE(properties.bold);
    EXPECT_FALSE(properties.italic);
    EXPECT_TRUE(properties.monospace);
    EXPECT_FALSE(properties.colorglyphs);
}

/**
 * @tc.name: GenerateFontPropertiesHMOSColorEmojiCompatByVector
 * @tc.desc: Test GenerateFontProperties
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontPropertiesQueryTest, GenerateFontPropertiesHMOSColorEmojiCompatByVector, TestSize.Level1)
{
    std::string ttfName = "HMOSColorEmojiCompat.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    std::vector<uint8_t> fontData;
    getDataStream(path, fontData);
    ASSERT_FALSE(fontData.empty());

    auto properties = FontPropertiesQuery::GenerateFontProperties(fontData);
    EXPECT_EQ(properties.weight, FontStyle::NORMAL_WEIGHT);
    EXPECT_EQ(properties.width, FontStyle::NORMAL_WIDTH);
    EXPECT_EQ(properties.slant, FontStyle::UPRIGHT_SLANT);
    EXPECT_FALSE(properties.bold);
    EXPECT_FALSE(properties.italic);
    EXPECT_TRUE(properties.monospace);
    EXPECT_FALSE(properties.colorglyphs);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS