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
#include "texgine/utils/exlog.h"

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

void ShowVisibilityFonts(std::vector<FontParser::FontDescriptor>& visibilityFonts)
{
    for (auto &it : visibilityFonts) {
        LOGSO_FUNC_LINE(INFO) << "\n fontFamily: " << it.fontFamily
                              << "\n fontSubfamily: " << it.fontSubfamily
                              << "\n fullName: " << it.fullName
                              << "\n italic: " << it.italic
                              << "\n monoSpace: " << it.monoSpace
                              << "\n path: " << it.path
                              << "\n postScriptName: " << it.postScriptName
                              << "\n symbolic: " << it.symbolic
                              << "\n weight: " << it.weight
                              << "\n width: " << it.width;
    }
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
    std::ifstream fileStream(FILE_NAME.c_str());
    if (fileStream.is_open()) {
        EXPECT_NE(visibilityFonts.size(), 0);
        ShowVisibilityFonts(visibilityFonts);
        fileStream.close();
    } else {
        EXPECT_EQ(visibilityFonts.size(), 0);
    }
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
    fontConfigJson.Dump();
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
