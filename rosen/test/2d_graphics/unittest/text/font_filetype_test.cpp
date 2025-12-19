/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gtest/gtest.h"

#include <cstddef>
#include <fstream>

#include "text/font_filetype.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class FontFileTypeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void FontFileTypeTest::SetUpTestCase() {}
void FontFileTypeTest::TearDownTestCase() {}
void FontFileTypeTest::SetUp() {}
void FontFileTypeTest::TearDown() {}

// ----------------------------------------------------------------------------------
// Test for FontFileFormat GetFontFileType(const std::shared_ptr<Typeface>& typeface)
// ----------------------------------------------------------------------------------

/**
 * @tc.name: GetFontFileTypeByFaceNullptr
 * @tc.desc: Test GetFontFileTypeByFace
 * @tc.type: FUNC
 * @tc.require: SR20250918053090
 */
HWTEST_F(FontFileTypeTest, GetFontFileTypeByFaceNullptr, TestSize.Level1)
{
    auto filetype = FontFileType::GetFontFileType(nullptr);
    EXPECT_EQ(filetype, FontFileType::FontFileFormat::UNKNOWN);
}

/**
 * @tc.name: GetFontFileTypeByFaceInvalid
 * @tc.desc: Test GetFontFileTypeByFace
 * @tc.type: FUNC
 * @tc.require: SR20250918053090
 */
HWTEST_F(FontFileTypeTest, GetFontFileTypeByFaceInvalid, TestSize.Level1)
{
    auto typeface = Typeface::MakeDefault();
    ASSERT_TRUE(typeface);
    auto data = typeface->Serialize();
    auto typefaceInvalid = Typeface::Deserialize(data->GetData(), 1); // wrong size for invalid tf
    auto filetype = FontFileType::GetFontFileType(typefaceInvalid);
    EXPECT_EQ(filetype, FontFileType::FontFileFormat::UNKNOWN);
}

/**
 * @tc.name: GetFontFileTypeByFaceValid01
 * @tc.desc: Test GetFontFileTypeByFace
 * @tc.type: FUNC
 * @tc.require: SR20250918053090
 */
HWTEST_F(FontFileTypeTest, GetFontFileTypeByFaceValid01, TestSize.Level1)
{
    std::string pathDefault = "/system/fonts/HarmonyOS_Sans_SC.ttf";
    auto typeface = Typeface::MakeFromFile(pathDefault.c_str());
    ASSERT_TRUE(typeface);
    auto filetype = FontFileType::GetFontFileType(typeface);
    EXPECT_EQ(filetype, FontFileType::FontFileFormat::TTF);
}

/**
 * @tc.name: GetFontFileTypeByFaceValid02
 * @tc.desc: Test GetFontFileTypeByFace
 * @tc.type: FUNC
 * @tc.require: SR20250918053090
 */
HWTEST_F(FontFileTypeTest, GetFontFileTypeByFaceValid02, TestSize.Level1)
{
    std::string pathDefault = "/system/fonts/NotoSansCJK-Regular.ttc";
    auto typeface = Typeface::MakeFromFile(pathDefault.c_str());
    ASSERT_TRUE(typeface);
    auto filetype = FontFileType::GetFontFileType(typeface);
    EXPECT_EQ(filetype, FontFileType::FontFileFormat::OTF);
}

// ----------------------------------------------------------------------------------
// Test for FontFileFormat GetFontFileType(const std::string& path, int& fileCount)
// ----------------------------------------------------------------------------------

/**
 * @tc.name: GetFontFileTypeByPathEmptyWithInput0
 * @tc.desc: Test GetFontFileTypeByPath
 * @tc.type: FUNC
 * @tc.require: SR20250918053090
 */
HWTEST_F(FontFileTypeTest, GetFontFileTypeByPathEmptyWithInput0, TestSize.Level1)
{
    std::string pathDefault = "";
    int fileCount = 0;
    auto filetype = FontFileType::GetFontFileType(pathDefault, fileCount);
    EXPECT_EQ(filetype, FontFileType::FontFileFormat::UNKNOWN);
    EXPECT_EQ(fileCount, 0);
}

/**
 * @tc.name: GetFontFileTypeByPathEmptyWithInputPosHuge
 * @tc.desc: Test GetFontFileTypeByPath
 * @tc.type: FUNC
 * @tc.require: SR20250918053090
 */
HWTEST_F(FontFileTypeTest, GetFontFileTypeByPathEmptyWithInputPosHuge, TestSize.Level1)
{
    std::string pathDefault = "";
    int fileCount = 100;
    auto filetype = FontFileType::GetFontFileType(pathDefault, fileCount);
    EXPECT_EQ(filetype, FontFileType::FontFileFormat::UNKNOWN);
    EXPECT_EQ(fileCount, 0);
}

/**
 * @tc.name: GetFontFileTypeByPathEmptyWithInputNegHuge
 * @tc.desc: Test GetFontFileTypeByPath
 * @tc.type: FUNC
 * @tc.require: SR20250918053090
 */
HWTEST_F(FontFileTypeTest, GetFontFileTypeByPathEmptyWithInputNegHuge, TestSize.Level1)
{
    std::string pathDefault = "";
    int fileCount = -100;
    auto filetype = FontFileType::GetFontFileType(pathDefault, fileCount);
    EXPECT_EQ(filetype, FontFileType::FontFileFormat::UNKNOWN);
    EXPECT_EQ(fileCount, 0);
}

/**
 * @tc.name: GetFontFileTypeByPathInvalidWithInput0
 * @tc.desc: Test GetFontFileTypeByPath
 * @tc.type: FUNC
 * @tc.require: SR20250918053090
 */
HWTEST_F(FontFileTypeTest, GetFontFileTypeByPathInvalidWithInput0, TestSize.Level1)
{
    std::string pathDefault = "hm_symbol_config.json";
    int fileCount = 0;
    auto filetype = FontFileType::GetFontFileType(pathDefault, fileCount);
    EXPECT_EQ(filetype, FontFileType::FontFileFormat::UNKNOWN);
    EXPECT_EQ(fileCount, 0);
}

/**
 * @tc.name: GetFontFileTypeByPathInvalidWithInputPosHuge
 * @tc.desc: Test GetFontFileTypeByPath
 * @tc.type: FUNC
 * @tc.require: SR20250918053090
 */
HWTEST_F(FontFileTypeTest, GetFontFileTypeByPathInvalidWithInputPosHuge, TestSize.Level1)
{
    std::string pathDefault = "hm_symbol_config.json";
    int fileCount = 100;
    auto filetype = FontFileType::GetFontFileType(pathDefault, fileCount);
    EXPECT_EQ(filetype, FontFileType::FontFileFormat::UNKNOWN);
    EXPECT_EQ(fileCount, 0);
}

/**
 * @tc.name: GetFontFileTypeByPathInvalidWithInputNegHuge
 * @tc.desc: Test GetFontFileTypeByPath
 * @tc.type: FUNC
 * @tc.require: SR20250918053090
 */
HWTEST_F(FontFileTypeTest, GetFontFileTypeByPathInvalidWithInputNegHuge, TestSize.Level1)
{
    std::string pathDefault = "hm_symbol_config.json";
    int fileCount = -100;
    auto filetype = FontFileType::GetFontFileType(pathDefault, fileCount);
    EXPECT_EQ(filetype, FontFileType::FontFileFormat::UNKNOWN);
    EXPECT_EQ(fileCount, 0);
}

/**
 * @tc.name: GetFontFileTypeByPathValidWithInput0
 * @tc.desc: Test GetFontFileTypeByPath
 * @tc.type: FUNC
 * @tc.require: SR20250918053090
 */
HWTEST_F(FontFileTypeTest, GetFontFileTypeByPathValidWithInput0, TestSize.Level1)
{
    std::string pathDefault = "/system/fonts/HarmonyOS_Sans_SC.ttf";
    int fileCount = 0;
    auto filetype = FontFileType::GetFontFileType(pathDefault, fileCount);
    EXPECT_EQ(filetype, FontFileType::FontFileFormat::TTF);
    EXPECT_EQ(fileCount, 1);
}

/**
 * @tc.name: GetFontFileTypeByPathValidWithInputPosHuge
 * @tc.desc: Test GetFontFileTypeByPath
 * @tc.type: FUNC
 * @tc.require: SR20250918053090
 */
HWTEST_F(FontFileTypeTest, GetFontFileTypeByPathValidWithInputPosHuge, TestSize.Level1)
{
    std::string pathDefault = "/system/fonts/HarmonyOS_Sans_SC.ttf";
    int fileCount = 100;
    auto filetype = FontFileType::GetFontFileType(pathDefault, fileCount);
    EXPECT_EQ(filetype, FontFileType::FontFileFormat::TTF);
    EXPECT_EQ(fileCount, 1);
}

/**
 * @tc.name: GetFontFileTypeByPathValidWithInputNegHuge
 * @tc.desc: Test GetFontFileTypeByPath
 * @tc.type: FUNC
 * @tc.require: SR20250918053090
 */
HWTEST_F(FontFileTypeTest, GetFontFileTypeByPathValidWithInputNegHuge, TestSize.Level1)
{
    std::string pathDefault = "/system/fonts/HarmonyOS_Sans_SC.ttf";
    int fileCount = -100;
    auto filetype = FontFileType::GetFontFileType(pathDefault, fileCount);
    EXPECT_EQ(filetype, FontFileType::FontFileFormat::TTF);
    EXPECT_EQ(fileCount, 1);
}

/**
 * @tc.name: GetFontFileTypeByPathValidWithInputOTC
 * @tc.desc: Test GetFontFileTypeByPath
 * @tc.type: FUNC
 * @tc.require: SR20250918053090
 */
HWTEST_F(FontFileTypeTest, GetFontFileTypeByPathValidWithInputOTC, TestSize.Level1)
{
    std::string pathDefault = "/system/fonts/NotoSansCJK-Regular.ttc";
    int fileCount = 0;
    auto filetype = FontFileType::GetFontFileType(pathDefault, fileCount);
    EXPECT_EQ(filetype, FontFileType::FontFileFormat::OTC);
    EXPECT_EQ(fileCount, 10);
}

/**
 * @tc.name: GetFontFileTypeByPathValidWithWrongFile
 * @tc.desc: Test GetFontFileTypeByPath
 * @tc.type: FUNC
 * @tc.require: SR20250918053090
 */
HWTEST_F(FontFileTypeTest, GetFontFileTypeByPathValidWithWrongFile, TestSize.Level1)
{
    std::string pathDefault = "/system/fonts/hm_symbol_config.json";
    int fileCount = 0;
    auto filetype = FontFileType::GetFontFileType(pathDefault, fileCount);
    EXPECT_EQ(filetype, FontFileType::FontFileFormat::UNKNOWN);
    EXPECT_EQ(fileCount, 0);
}
// ----------------------------------------------------------------------------------
// Test for FontFileFormat GetFontFileType(const std::vector<uint8_t>& data, int& fileCount)
// ----------------------------------------------------------------------------------

/**
 * @tc.name: GetFontFileTypeByDataEmptyWithInput0
 * @tc.desc: Test GetFontFileTypeByData
 * @tc.type: FUNC
 * @tc.require: SR20250918053090
 */
HWTEST_F(FontFileTypeTest, GetFontFileTypeByDataEmptyWithInput0, TestSize.Level1)
{
    int fileCount = 0;
    std::vector<uint8_t> fontData = {};
    auto filetype = FontFileType::GetFontFileType(fontData, fileCount);
    EXPECT_EQ(filetype, FontFileType::FontFileFormat::UNKNOWN);
    EXPECT_EQ(fileCount, 0);
}

/**
 * @tc.name: GetFontFileTypeByDataEmptyWithInputPosHuge
 * @tc.desc: Test GetFontFileTypeByData
 * @tc.type: FUNC
 * @tc.require: SR20250918053090
 */
HWTEST_F(FontFileTypeTest, GetFontFileTypeByDataEmptyWithInputPosHuge, TestSize.Level1)
{
    int fileCount = 100;
    std::vector<uint8_t> fontData = {};
    auto filetype = FontFileType::GetFontFileType(fontData, fileCount);
    EXPECT_EQ(filetype, FontFileType::FontFileFormat::UNKNOWN);
    EXPECT_EQ(fileCount, 0);
}

/**
 * @tc.name: GetFontFileTypeByDataEmptyWithInputNegHuge
 * @tc.desc: Test GetFontFileTypeByData
 * @tc.type: FUNC
 * @tc.require: SR20250918053090
 */
HWTEST_F(FontFileTypeTest, GetFontFileTypeByDataEmptyWithInputNegHuge, TestSize.Level1)
{
    int fileCount = -100;
    std::vector<uint8_t> fontData = {};
    auto filetype = FontFileType::GetFontFileType(fontData, fileCount);
    EXPECT_EQ(filetype, FontFileType::FontFileFormat::UNKNOWN);
    EXPECT_EQ(fileCount, 0);
}

/**
 * @tc.name: GetFontFileTypeByDataInvalidWithInput0
 * @tc.desc: Test GetFontFileTypeByData
 * @tc.type: FUNC
 * @tc.require: SR20250918053090
 */
HWTEST_F(FontFileTypeTest, GetFontFileTypeByDataInvalidWithInput0, TestSize.Level1)
{
    int fileCount = 0;
    std::vector<uint8_t> fontData = {0, 0, 0};
    auto filetype = FontFileType::GetFontFileType(fontData, fileCount);
    EXPECT_EQ(filetype, FontFileType::FontFileFormat::UNKNOWN);
    EXPECT_EQ(fileCount, 0);
}

/**
 * @tc.name: GetFontFileTypeByDataInvalidWithInputPosHuge
 * @tc.desc: Test GetFontFileTypeByData
 * @tc.type: FUNC
 * @tc.require: SR20250918053090
 */
HWTEST_F(FontFileTypeTest, GetFontFileTypeByDataInvalidWithInputPosHuge, TestSize.Level1)
{
    int fileCount = 100;
    std::vector<uint8_t> fontData = {0, 0, 0};
    auto filetype = FontFileType::GetFontFileType(fontData, fileCount);
    EXPECT_EQ(filetype, FontFileType::FontFileFormat::UNKNOWN);
    EXPECT_EQ(fileCount, 0);
}

/**
 * @tc.name: GetFontFileTypeByDataInvalidWithInputNegHuge
 * @tc.desc: Test GetFontFileTypeByData
 * @tc.type: FUNC
 * @tc.require: SR20250918053090
 */
HWTEST_F(FontFileTypeTest, GetFontFileTypeByDataInvalidWithInputNegHuge, TestSize.Level1)
{
    int fileCount = -100;
    std::vector<uint8_t> fontData = {0, 0, 0};
    auto filetype = FontFileType::GetFontFileType(fontData, fileCount);
    EXPECT_EQ(filetype, FontFileType::FontFileFormat::UNKNOWN);
    EXPECT_EQ(fileCount, 0);
}

/**
 * @tc.name: GetFontFileTypeByDataValidWithInput0
 * @tc.desc: Test GetFontFileTypeByData
 * @tc.type: FUNC
 * @tc.require: SR20250918053090
 */
HWTEST_F(FontFileTypeTest, GetFontFileTypeByDataValidWithInput0, TestSize.Level1)
{
    std::string pathDefault = "/system/fonts/HarmonyOS_Sans_SC.ttf";
    int fileCount = 0;
    std::ifstream ttfFile(pathDefault, std::ios::in | std::ios::binary);
    ASSERT_TRUE(ttfFile.is_open());
    ttfFile.seekg(0, std::ios::end);
    size_t ttfLen = ttfFile.tellg();
    ttfFile.seekg(0, std::ios::beg);
    std::vector<uint8_t> fontData(ttfLen);
    ttfFile.read(reinterpret_cast<char*>(fontData.data()), ttfLen);
    ASSERT_TRUE(ttfFile.good());
    ttfFile.close();
    auto filetype = FontFileType::GetFontFileType(fontData, fileCount);
    EXPECT_EQ(filetype, FontFileType::FontFileFormat::TTF);
    EXPECT_EQ(fileCount, 1);
}

/**
 * @tc.name: GetFontFileTypeByDataValidWithInputPosHuge
 * @tc.desc: Test GetFontFileTypeByData
 * @tc.type: FUNC
 * @tc.require: SR20250918053090
 */
HWTEST_F(FontFileTypeTest, GetFontFileTypeByDataValidWithInputPosHuge, TestSize.Level1)
{
    std::string pathDefault = "/system/fonts/HarmonyOS_Sans_SC.ttf";
    int fileCount = 100;
    std::ifstream ttfFile(pathDefault, std::ios::in | std::ios::binary);
    ASSERT_TRUE(ttfFile.is_open());
    ttfFile.seekg(0, std::ios::end);
    size_t ttfLen = ttfFile.tellg();
    ttfFile.seekg(0, std::ios::beg);
    std::vector<uint8_t> fontData(ttfLen);
    ttfFile.read(reinterpret_cast<char*>(fontData.data()), ttfLen);
    ASSERT_TRUE(ttfFile.good());
    ttfFile.close();
    auto filetype = FontFileType::GetFontFileType(fontData, fileCount);
    EXPECT_EQ(filetype, FontFileType::FontFileFormat::TTF);
    EXPECT_EQ(fileCount, 1);
}

/**
 * @tc.name: GetFontFileTypeByDataValidWithInputNegHuge
 * @tc.desc: Test GetFontFileTypeByData
 * @tc.type: FUNC
 * @tc.require: SR20250918053090
 */
HWTEST_F(FontFileTypeTest, GetFontFileTypeByDataValidWithInputNegHuge, TestSize.Level1)
{
    std::string pathDefault = "/system/fonts/HarmonyOS_Sans_SC.ttf";
    int fileCount = -100;
    std::ifstream ttfFile(pathDefault, std::ios::in | std::ios::binary);
    ASSERT_TRUE(ttfFile.is_open());
    ttfFile.seekg(0, std::ios::end);
    size_t ttfLen = ttfFile.tellg();
    ttfFile.seekg(0, std::ios::beg);
    std::vector<uint8_t> fontData(ttfLen);
    ttfFile.read(reinterpret_cast<char*>(fontData.data()), ttfLen);
    ASSERT_TRUE(ttfFile.good());
    ttfFile.close();
    auto filetype = FontFileType::GetFontFileType(fontData, fileCount);
    EXPECT_EQ(filetype, FontFileType::FontFileFormat::TTF);
    EXPECT_EQ(fileCount, 1);
}

/**
 * @tc.name: GetFontFileTypeByDataValidWithInputOTC
 * @tc.desc: Test GetFontFileTypeByData
 * @tc.type: FUNC
 * @tc.require: SR20250918053090
 */
HWTEST_F(FontFileTypeTest, GetFontFileTypeByDataValidWithInputOTC, TestSize.Level1)
{
    std::string pathDefault = "/system/fonts/NotoSansCJK-Regular.ttc";
    int fileCount = 0;
    std::ifstream ttfFile(pathDefault, std::ios::in | std::ios::binary);
    ASSERT_TRUE(ttfFile.is_open());
    ttfFile.seekg(0, std::ios::end);
    size_t ttfLen = ttfFile.tellg();
    ttfFile.seekg(0, std::ios::beg);
    std::vector<uint8_t> fontData(ttfLen);
    ttfFile.read(reinterpret_cast<char*>(fontData.data()), ttfLen);
    ASSERT_TRUE(ttfFile.good());
    ttfFile.close();
    auto filetype = FontFileType::GetFontFileType(fontData, fileCount);
    EXPECT_EQ(filetype, FontFileType::FontFileFormat::OTC);
    EXPECT_EQ(fileCount, 10);
}

/**
 * @tc.name: GetFontFileTypeByDataValidWithInputWrongFile
 * @tc.desc: Test GetFontFileTypeByData
 * @tc.type: FUNC
 * @tc.require: SR20250918053090
 */
HWTEST_F(FontFileTypeTest, GetFontFileTypeByDataValidWithInputWrongFile, TestSize.Level1)
{
    std::string pathDefault = "/system/fonts/hm_symbol_config.json";
    int fileCount = 0;
    std::ifstream ttfFile(pathDefault, std::ios::in | std::ios::binary);
    ASSERT_TRUE(ttfFile.is_open());
    ttfFile.seekg(0, std::ios::end);
    size_t ttfLen = ttfFile.tellg();
    ttfFile.seekg(0, std::ios::beg);
    std::vector<uint8_t> fontData(ttfLen);
    ttfFile.read(reinterpret_cast<char*>(fontData.data()), ttfLen);
    ASSERT_TRUE(ttfFile.good());
    ttfFile.close();
    auto filetype = FontFileType::GetFontFileType(fontData, fileCount);
    EXPECT_EQ(filetype, FontFileType::FontFileFormat::UNKNOWN);
    EXPECT_EQ(fileCount, 0);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS