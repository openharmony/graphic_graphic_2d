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
#include "cmap_table_parser.h"
#include "name_table_parser.h"
#include "post_table_parser.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace TextEngine {
static const std::string FILE_NAME = "/system/fonts/visibility_list.json";

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
 * @tc.name: CheckFullNameParamInvalidTest1
 * @tc.desc: test for check full name
 * @tc.type: FUNC
 */
HWTEST_F(FontParserTest, CheckFullNameParamInvalidTest1, TestSize.Level1)
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
    EXPECT_TRUE(FontParser::CheckFullNameParamInvalid(fd, LANGUAGE_TC, "non-empty"));

    fd.requestedFullname = "testName";
    fd.fullName = "otherName";
    fd.requestedLid = LANGUAGE_EN;
    EXPECT_TRUE(FontParser::CheckFullNameParamInvalid(fd, LANGUAGE_EN, "non-empty"));

    fd.requestedFullname = "";
    fd.requestedLid = LANGUAGE_DEFAULT;
    EXPECT_FALSE(FontParser::CheckFullNameParamInvalid(fd, LANGUAGE_TC, "non-empty"));
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
