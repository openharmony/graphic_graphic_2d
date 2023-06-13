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

#include <gtest/gtest.h>

#include "font_config.h"
#include "font_parser.h"
#include "texgine/utils/exlog.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace TextEngine {
static const std::string fileName = "/system/fonts/visibility_list.json";

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

    auto fontSet2 = GetFontSet(fileName.c_str());
    EXPECT_NE(fontSet2.size(), 0);
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
    EXPECT_NE(visibilityFonts.size(), 0);
    ShowVisibilityFonts(visibilityFonts);
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
