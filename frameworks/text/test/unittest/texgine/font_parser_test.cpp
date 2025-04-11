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
#include <gmock/gmock.h>

#include "font_config.h"
#include "font_parser.h"
#include "cmap_table_parser.h"
#include "name_table_parser.h"
#include "post_table_parser.h"
#include "ranges.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace TextEngine {
static const std::string FILE_NAME = "/system/fonts/visibility_list.json";

class FontParserTest : public testing::Test {
};

class MockCmapTableParser : public CmapTableParser {
public:
    MockCmapTableParser() {}
    MOCK_METHOD0(Dump, void());
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
HWTEST_F(FontParserTest, FontParserTest1, TestSize.Level1)
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
HWTEST_F(FontParserTest, FontParserTest2, TestSize.Level1)
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
HWTEST_F(FontParserTest, FontParserTest3, TestSize.Level1)
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
HWTEST_F(FontParserTest, FontConfigTest1, TestSize.Level1)
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
HWTEST_F(FontParserTest, FontConfigTest2, TestSize.Level1)
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
 * @tc.name: CmapTableParserTest1
 * @tc.desc: opentype parser test
 * @tc.type:FUNC
 */
HWTEST_F(FontParserTest, CmapTableParserTest1, TestSize.Level1)
{
    MockCmapTableParser mockCmapTableParser;
    CmapTableParser cmapTableParser_default;
    char* data = new char[sizeof(CmapTables) + sizeof(EncodingRecord)];
    CmapTables* p = reinterpret_cast<CmapTables*>(data);
    p->version = { 0 };
    p->numTables = { 1 };
    p->encodingRecords[0].platformID = { 0 };
    p->encodingRecords[0].encodingID = { 0 };
    p->encodingRecords[0].subtableOffset = { 0 };
    CmapTableParser cmapTableParser(data, sizeof(CmapTables) + sizeof(EncodingRecord));
    cmapTableParser.Dump();
    struct NameRecord nameRecord;
    struct NameTable nameTable;
    nameRecord.encodingId = nameTable.count;
    EXPECT_EQ(CmapTableParser::Parse(nullptr, 0), nullptr);
    EXPECT_CALL(mockCmapTableParser, Dump()).Times(1);
    mockCmapTableParser.Dump();
}

/**
 * @tc.name: NameTableParserTest1
 * @tc.desc: opentype parser test
 * @tc.type:FUNC
 */
HWTEST_F(FontParserTest, NameTableParserTest1, TestSize.Level1)
{
    NameTableParser nameTableParser(nullptr, 0);
    struct NameRecord nameRecord;
    struct NameTable nameTable;
    nameRecord.encodingId = nameTable.count;
    EXPECT_EQ(NameTableParser::Parse(nullptr, 0), nullptr);
    nameTableParser.Dump();
}

/**
 * @tc.name: NameTableParserTest2
 * @tc.desc: opentype parser test
 * @tc.type:FUNC
 */
HWTEST_F(FontParserTest, NameTableParserTest2, TestSize.Level1)
{
    auto typeface = Drawing::Typeface::MakeDefault();
    ASSERT_NE(typeface, nullptr);
    auto tag = HB_TAG('n', 'a', 'm', 'e');
    auto size = typeface->GetTableSize(tag);
    ASSERT_GT(size, 0);
    std::unique_ptr<char[]> tableData = nullptr;
    tableData = std::make_unique<char[]>(size);
    auto retTableData = typeface->GetTableData(tag, 0, size, tableData.get());
    ASSERT_EQ(size, retTableData);
    hb_blob_t* hblob = nullptr;
    hblob = hb_blob_create(
            reinterpret_cast<const char *>(tableData.get()), size, HB_MEMORY_MODE_WRITABLE, tableData.get(), nullptr);
    ASSERT_NE(hblob, nullptr);
    const char* data = nullptr;
    unsigned int length = 0;
    data = hb_blob_get_data(hblob, nullptr);
    length = hb_blob_get_length(hblob);
    auto parseName = std::make_shared<NameTableParser>(data, length);
    auto nameTable = parseName->Parse(data, length);
    parseName->Dump();
    hb_blob_destroy(hblob);
    EXPECT_NE(nameTable, nullptr);
}

/**
 * @tc.name: PostTableParserTest1
 * @tc.desc: opentype parser test
 * @tc.type:FUNC
 */
HWTEST_F(FontParserTest, PostTableParserTest1, TestSize.Level1)
{
    PostTable postTable;
    PostTableParser postTableParser(reinterpret_cast<char*>(&postTable), sizeof(postTable));
    const PostTable* res = postTableParser.Parse(reinterpret_cast<char*>(&postTable), sizeof(postTable));
    EXPECT_EQ(res->version.Get(), postTable.version.Get());
    EXPECT_EQ(res->italicAngle.Get(), postTable.italicAngle.Get());
    EXPECT_EQ(res->underlinePosition.Get(), postTable.underlinePosition.Get());
    EXPECT_EQ(res->underlineThickness.Get(), postTable.underlineThickness.Get());
    EXPECT_EQ(res->isFixedPitch.Get(), postTable.isFixedPitch.Get());
    EXPECT_EQ(res->minMemType42.Get(), postTable.minMemType42.Get());
    EXPECT_EQ(res->maxMemType42.Get(), postTable.maxMemType42.Get());
    EXPECT_EQ(res->minMemType1.Get(), postTable.minMemType1.Get());
    postTableParser.Dump();
}

/**
 * @tc.name: OpenTypeBasicTypeTest1
 * @tc.desc: opentype parser test
 * @tc.type:FUNC
 */
HWTEST_F(FontParserTest, OpenTypeBasicTypeTest1, TestSize.Level1)
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
 * @tc.name: RangesTest1
 * @tc.desc: opentype parser test
 * @tc.type:FUNC
 */
HWTEST_F(FontParserTest, RangesTest1, TestSize.Level1)
{
    Ranges ranges;
    struct Ranges::Range range = { 0, 2, 1 };
    ranges.AddRange(range);
    struct Ranges::Range range2 = { 4, 5, 2 };
    ranges.AddRange(range2);
    EXPECT_EQ(ranges.GetGlyphId(3), Ranges::INVALID_GLYPH_ID);
    EXPECT_EQ(ranges.GetGlyphId(0), 1);
    EXPECT_EQ(ranges.GetGlyphId(4), 6);
    ranges.Dump();
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
