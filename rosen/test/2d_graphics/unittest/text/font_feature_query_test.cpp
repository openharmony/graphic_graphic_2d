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

#include "text/font_feature_query.h"
#include "text/typeface.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {

class FontFeatureQueryTest : public testing::Test {
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
 * @tc.name: GenerateFontSupportedFeaturesEmptyTypeface
 * @tc.desc: Test GenerateFontSupportedFeatures
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontFeatureQueryTest, GenerateFontSupportedFeaturesEmptyTypeface, TestSize.Level1)
{
    auto features = FontFeatureQuery::GenerateFontSupportedFeatures(std::shared_ptr<Typeface>(nullptr));
    EXPECT_TRUE(features.empty());
}

/**
 * @tc.name: GenerateFontSupportedFeaturesEmptyFontPath
 * @tc.desc: Test GenerateFontSupportedFeatures
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontFeatureQueryTest, GenerateFontSupportedFeaturesEmptyFontPath, TestSize.Level1)
{
    auto features = FontFeatureQuery::GenerateFontSupportedFeatures(std::string(""));
    EXPECT_TRUE(features.empty());
}

/**
 * @tc.name: GenerateFontSupportedFeaturesInvalidPath
 * @tc.desc: Test GenerateFontSupportedFeatures
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontFeatureQueryTest, GenerateFontSupportedFeaturesInvalidPath, TestSize.Level1)
{
    std::string invalidPath = "/system/fonts/NotExistFont.ttf";
    auto features = FontFeatureQuery::GenerateFontSupportedFeatures(invalidPath);
    EXPECT_TRUE(features.empty());
}

/**
 * @tc.name: GenerateFontSupportedFeaturesEmptyFontData
 * @tc.desc: Test GenerateFontSupportedFeatures
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontFeatureQueryTest, GenerateFontSupportedFeaturesEmptyFontData, TestSize.Level1)
{
    std::vector<uint8_t> emptyData;
    auto features = FontFeatureQuery::GenerateFontSupportedFeatures(emptyData);
    EXPECT_TRUE(features.empty());
}

/**
 * @tc.name: GenerateFontSupportedFeaturesHarmonyOSSansByTypeface
 * @tc.desc: Test GenerateFontSupportedFeatures
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontFeatureQueryTest, GenerateFontSupportedFeaturesHarmonyOSSansByTypeface, TestSize.Level1)
{
    std::string ttfName = "HarmonyOS_Sans.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    if (!IsFileExists(path)) {
        GTEST_SKIP() << "Font file not found: " << path;
    }

    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);

    auto features = FontFeatureQuery::GenerateFontSupportedFeatures(typeface);
    std::vector<std::string> featuresExp = {"aalt", "calt", "case", "ccmp", "dnom", "frac", "kern", "liga", "locl",
        "numr", "ordn", "pnum", "sinf", "ss01", "ss02", "ss03", "ss04", "ss06", "ss09", "ss10", "subs", "sups", "tnum"};
    EXPECT_EQ(features, featuresExp);
}

/**
 * @tc.name: GenerateFontSupportedFeaturesHarmonyOSSansByPath
 * @tc.desc: Test GenerateFontSupportedFeatures
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontFeatureQueryTest, GenerateFontSupportedFeaturesHarmonyOSSansByPath, TestSize.Level1)
{
    std::string ttfName = "HarmonyOS_Sans.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    if (!IsFileExists(path)) {
        GTEST_SKIP() << "Font file not found: " << path;
    }

    auto features = FontFeatureQuery::GenerateFontSupportedFeatures(path);
    std::vector<std::string> featuresExp = {"aalt", "calt", "case", "ccmp", "dnom", "frac", "kern", "liga", "locl",
        "numr", "ordn", "pnum", "sinf", "ss01", "ss02", "ss03", "ss04", "ss06", "ss09", "ss10", "subs", "sups", "tnum"};
    EXPECT_EQ(features, featuresExp);
}

/**
 * @tc.name: GenerateFontSupportedFeaturesHarmonyOSSansByVector
 * @tc.desc: Test GenerateFontSupportedFeatures
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontFeatureQueryTest, GenerateFontSupportedFeaturesHarmonyOSSansByVector, TestSize.Level1)
{
    std::string ttfName = "HarmonyOS_Sans.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    std::vector<uint8_t> fontData;
    getDataStream(path, fontData);
    ASSERT_FALSE(fontData.empty());

    auto features = FontFeatureQuery::GenerateFontSupportedFeatures(fontData);
    std::vector<std::string> featuresExp = {"aalt", "calt", "case", "ccmp", "dnom", "frac", "kern", "liga", "locl",
        "numr", "ordn", "pnum", "sinf", "ss01", "ss02", "ss03", "ss04", "ss06", "ss09", "ss10", "subs", "sups", "tnum"};
    EXPECT_EQ(features, featuresExp);
}

/**
 * @tc.name: GenerateFontSupportedFeaturesDefaultTypeface
 * @tc.desc: Test GenerateFontSupportedFeatures
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontFeatureQueryTest, GenerateFontSupportedFeaturesDefaultTypeface, TestSize.Level1)
{
    auto typeface = Typeface::MakeDefault();
    ASSERT_TRUE(typeface);

    auto features = FontFeatureQuery::GenerateFontSupportedFeatures(typeface);
    for (const auto& feature : features) {
        EXPECT_EQ(feature.length(), 4u);
    }
}

/**
 * @tc.name: GenerateFontSupportedFeaturesUnique
 * @tc.desc: Test GenerateFontSupportedFeatures
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontFeatureQueryTest, GenerateFontSupportedFeaturesUnique, TestSize.Level1)
{
    std::string ttfName = "HarmonyOS_Sans.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    if (!IsFileExists(path)) {
        GTEST_SKIP() << "Font file not found: " << path;
    }

    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);

    auto features = FontFeatureQuery::GenerateFontSupportedFeatures(typeface);
    std::set<std::string> uniqueFeatures(features.begin(), features.end());
    EXPECT_EQ(uniqueFeatures.size(), features.size());
}

/**
 * @tc.name: GenerateFontSupportedFeaturesHarmonyOSSansTCByTypeface
 * @tc.desc: Test GenerateFontSupportedFeatures
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontFeatureQueryTest, GenerateFontSupportedFeaturesHarmonyOSSansTCByTypeface, TestSize.Level1)
{
    std::string ttfName = "HarmonyOS_Sans_TC.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    if (!IsFileExists(path)) {
        GTEST_SKIP() << "Font file not found: " << path;
    }

    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);

    auto features = FontFeatureQuery::GenerateFontSupportedFeatures(typeface);
    std::vector<std::string> featuresExp = {"aalt", "calt", "case", "ccmp", "dnom", "frac", "fwid", "hist", "hwid",
        "kern", "liga", "locl", "numr", "ordn", "pnum", "sinf", "ss01", "ss02", "ss03", "ss04", "ss05", "ss07", "ss08",
        "ss09", "ss10", "subs", "sups", "tnum", "vert", "vrt2"};
    EXPECT_EQ(features, featuresExp);
}

/**
 * @tc.name: GenerateFontSupportedFeaturesHarmonyOSSansTCByPath
 * @tc.desc: Test GenerateFontSupportedFeatures
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontFeatureQueryTest, GenerateFontSupportedFeaturesHarmonyOSSansTCByPath, TestSize.Level1)
{
    std::string ttfName = "HarmonyOS_Sans_TC.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    if (!IsFileExists(path)) {
        GTEST_SKIP() << "Font file not found: " << path;
    }

    auto features = FontFeatureQuery::GenerateFontSupportedFeatures(path);
    std::vector<std::string> featuresExp = {"aalt", "calt", "case", "ccmp", "dnom", "frac", "fwid", "hist", "hwid",
        "kern", "liga", "locl", "numr", "ordn", "pnum", "sinf", "ss01", "ss02", "ss03", "ss04", "ss05", "ss07", "ss08",
        "ss09", "ss10", "subs", "sups", "tnum", "vert", "vrt2"};
    EXPECT_EQ(features, featuresExp);
}

/**
 * @tc.name: GenerateFontSupportedFeaturesHarmonyOSSansTCByVector
 * @tc.desc: Test GenerateFontSupportedFeatures
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontFeatureQueryTest, GenerateFontSupportedFeaturesHarmonyOSSansTCByVector, TestSize.Level1)
{
    std::string ttfName = "HarmonyOS_Sans_TC.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    std::vector<uint8_t> fontData;
    getDataStream(path, fontData);
    ASSERT_FALSE(fontData.empty());

    auto features = FontFeatureQuery::GenerateFontSupportedFeatures(fontData);
    std::vector<std::string> featuresExp = {"aalt", "calt", "case", "ccmp", "dnom", "frac", "fwid", "hist", "hwid",
        "kern", "liga", "locl", "numr", "ordn", "pnum", "sinf", "ss01", "ss02", "ss03", "ss04", "ss05", "ss07", "ss08",
        "ss09", "ss10", "subs", "sups", "tnum", "vert", "vrt2"};
    EXPECT_EQ(features, featuresExp);
}

/**
 * @tc.name: GenerateFontSupportedFeaturesHarmonyOSSansItalicByTypeface
 * @tc.desc: Test GenerateFontSupportedFeatures
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontFeatureQueryTest, GenerateFontSupportedFeaturesHarmonyOSSansItalicByTypeface, TestSize.Level1)
{
    std::string ttfName = "HarmonyOS_Sans_Italic.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    if (!IsFileExists(path)) {
        GTEST_SKIP() << "Font file not found: " << path;
    }

    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);

    auto features = FontFeatureQuery::GenerateFontSupportedFeatures(typeface);
    std::vector<std::string> featuresExp = {"aalt", "calt", "case", "ccmp", "dnom", "frac", "kern", "liga", "locl",
        "numr", "ordn", "pnum", "sinf", "ss04", "subs", "sups", "tnum"};
    EXPECT_EQ(features, featuresExp);
}

/**
 * @tc.name: GenerateFontSupportedFeaturesHarmonyOSSansItalicByPath
 * @tc.desc: Test GenerateFontSupportedFeatures
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontFeatureQueryTest, GenerateFontSupportedFeaturesHarmonyOSSansItalicByPath, TestSize.Level1)
{
    std::string ttfName = "HarmonyOS_Sans_Italic.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    if (!IsFileExists(path)) {
        GTEST_SKIP() << "Font file not found: " << path;
    }
    
    auto features = FontFeatureQuery::GenerateFontSupportedFeatures(path);
    std::vector<std::string> featuresExp = {"aalt", "calt", "case", "ccmp", "dnom", "frac", "kern", "liga", "locl",
        "numr", "ordn", "pnum", "sinf", "ss04", "subs", "sups", "tnum"};
    EXPECT_EQ(features, featuresExp);
}

/**
 * @tc.name: GenerateFontSupportedFeaturesHarmonyOSSansItalicByVector
 * @tc.desc: Test GenerateFontSupportedFeatures
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontFeatureQueryTest, GenerateFontSupportedFeaturesHarmonyOSSansItalicByVector, TestSize.Level1)
{
    std::string ttfName = "HarmonyOS_Sans_Italic.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    std::vector<uint8_t> fontData;
    getDataStream(path, fontData);
    ASSERT_FALSE(fontData.empty());

    auto features = FontFeatureQuery::GenerateFontSupportedFeatures(fontData);
    std::vector<std::string> featuresExp = {"aalt", "calt", "case", "ccmp", "dnom", "frac", "kern", "liga", "locl",
        "numr", "ordn", "pnum", "sinf", "ss04", "subs", "sups", "tnum"};
    EXPECT_EQ(features, featuresExp);
}

/**
 * @tc.name: GenerateFontSupportedFeaturesNotoSansSylotiNagriRegularByTypeface
 * @tc.desc: Test GenerateFontSupportedFeatures
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontFeatureQueryTest, GenerateFontSupportedFeaturesNotoSansSylotiNagriRegularByTypeface, TestSize.Level1)
{
    std::string ttfName = "NotoSansSylotiNagri-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    if (!IsFileExists(path)) {
        GTEST_SKIP() << "Font file not found: " << path;
    }

    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);

    auto features = FontFeatureQuery::GenerateFontSupportedFeatures(typeface);
    std::vector<std::string> featuresExp = {"aalt", "abvm", "ccmp", "kern", "locl", "mark", "mkmk", "ordn"};
    EXPECT_EQ(features, featuresExp);
}

/**
 * @tc.name: GenerateFontSupportedFeaturesNotoSansSylotiNagriRegularByPath
 * @tc.desc: Test GenerateFontSupportedFeatures
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontFeatureQueryTest, GenerateFontSupportedFeaturesNotoSansSylotiNagriRegularByPath, TestSize.Level1)
{
    std::string ttfName = "NotoSansSylotiNagri-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    if (!IsFileExists(path)) {
        GTEST_SKIP() << "Font file not found: " << path;
    }

    auto features = FontFeatureQuery::GenerateFontSupportedFeatures(path);
    std::vector<std::string> featuresExp = {"aalt", "abvm", "ccmp", "kern", "locl", "mark", "mkmk", "ordn"};
    EXPECT_EQ(features, featuresExp);
}

/**
 * @tc.name: GenerateFontSupportedFeaturesNotoSansSylotiNagriRegularByVector
 * @tc.desc: Test GenerateFontSupportedFeatures
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontFeatureQueryTest, GenerateFontSupportedFeaturesNotoSansSylotiNagriRegularByVector, TestSize.Level1)
{
    std::string ttfName = "NotoSansSylotiNagri-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    std::vector<uint8_t> fontData;
    getDataStream(path, fontData);
    ASSERT_FALSE(fontData.empty());

    auto features = FontFeatureQuery::GenerateFontSupportedFeatures(fontData);
    std::vector<std::string> featuresExp = {"aalt", "abvm", "ccmp", "kern", "locl", "mark", "mkmk", "ordn"};
    EXPECT_EQ(features, featuresExp);
}

/**
 * @tc.name: GenerateFontSupportedFeaturesDejaVuMathTeXGyreByTypeface
 * @tc.desc: Test GenerateFontSupportedFeatures
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontFeatureQueryTest, GenerateFontSupportedFeaturesDejaVuMathTeXGyreByTypeface, TestSize.Level1)
{
    std::string ttfName = "DejaVuMathTeXGyre.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    if (!IsFileExists(path)) {
        GTEST_SKIP() << "Font file not found: " << path;
    }

    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);

    auto features = FontFeatureQuery::GenerateFontSupportedFeatures(typeface);
    std::vector<std::string> featuresExp = {"aalt", "dtls", "ssty"};
    EXPECT_EQ(features, featuresExp);
}

/**
 * @tc.name: GenerateFontSupportedFeaturesDejaVuMathTeXGyreByPath
 * @tc.desc: Test GenerateFontSupportedFeatures
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontFeatureQueryTest, GenerateFontSupportedFeaturesDejaVuMathTeXGyreByPath, TestSize.Level1)
{
    std::string ttfName = "DejaVuMathTeXGyre.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    if (!IsFileExists(path)) {
        GTEST_SKIP() << "Font file not found: " << path;
    }

    auto features = FontFeatureQuery::GenerateFontSupportedFeatures(path);
    std::vector<std::string> featuresExp = {"aalt", "dtls", "ssty"};
    EXPECT_EQ(features, featuresExp);
}

/**
 * @tc.name: GenerateFontSupportedFeaturesDejaVuMathTeXGyreByVector
 * @tc.desc: Test GenerateFontSupportedFeatures
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontFeatureQueryTest, GenerateFontSupportedFeaturesDejaVuMathTeXGyreByVector, TestSize.Level1)
{
    std::string ttfName = "DejaVuMathTeXGyre.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    std::vector<uint8_t> fontData;
    getDataStream(path, fontData);
    ASSERT_FALSE(fontData.empty());

    auto features = FontFeatureQuery::GenerateFontSupportedFeatures(fontData);
    std::vector<std::string> featuresExp = {"aalt", "dtls", "ssty"};
    EXPECT_EQ(features, featuresExp);
}

/**
 * @tc.name: GenerateFontSupportedFeaturesHMSymbolVFByTypeface
 * @tc.desc: Test GenerateFontSupportedFeatures
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontFeatureQueryTest, GenerateFontSupportedFeaturesHMSymbolVFByTypeface, TestSize.Level1)
{
    std::string ttfName = "HMSymbolVF.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    if (!IsFileExists(path)) {
        GTEST_SKIP() << "Font file not found: " << path;
    }

    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);

    auto features = FontFeatureQuery::GenerateFontSupportedFeatures(typeface);
    std::vector<std::string> featuresExp = {"ss01", "ss02"};
    EXPECT_EQ(features, featuresExp);
}

/**
 * @tc.name: GenerateFontSupportedFeaturesHMSymbolVFByPath
 * @tc.desc: Test GenerateFontSupportedFeatures
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontFeatureQueryTest, GenerateFontSupportedFeaturesHMSymbolVFByPath, TestSize.Level1)
{
    std::string ttfName = "HMSymbolVF.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    if (!IsFileExists(path)) {
        GTEST_SKIP() << "Font file not found: " << path;
    }

    auto features = FontFeatureQuery::GenerateFontSupportedFeatures(path);
    std::vector<std::string> featuresExp = {"ss01", "ss02"};
    EXPECT_EQ(features, featuresExp);
}

/**
 * @tc.name: GenerateFontSupportedFeaturesHMSymbolVFByVector
 * @tc.desc: Test GenerateFontSupportedFeatures
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontFeatureQueryTest, GenerateFontSupportedFeaturesHMSymbolVFByVector, TestSize.Level1)
{
    std::string ttfName = "HMSymbolVF.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    std::vector<uint8_t> fontData;
    getDataStream(path, fontData);
    ASSERT_FALSE(fontData.empty());

    auto features = FontFeatureQuery::GenerateFontSupportedFeatures(fontData);
    std::vector<std::string> featuresExp = {"ss01", "ss02"};
    EXPECT_EQ(features, featuresExp);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS