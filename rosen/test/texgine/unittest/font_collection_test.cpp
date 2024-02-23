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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "font_collection.h"
#include "texgine_font_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace TextEngine {
struct MockVars {
    int catchedSize = -1;
    bool hasRetval = false;
    int hasCount = 0;
    std::shared_ptr<TexgineFontManager> fontMgr = std::make_shared<TexgineFontManager>();
    std::shared_ptr<TexgineTypeface> SCRetvalTypeface = std::make_shared<TexgineTypeface>();
    std::shared_ptr<TexgineTypeface> styleRetvalTypeface = std::make_shared<TexgineTypeface>();
    std::vector<std::shared_ptr<VariantFontStyleSet>> fontStyleSets;
    std::shared_ptr<TextEngine::FontCollection> fontCollection;
} g_fcMockVars;

#ifndef USE_ROSEN_DRAWING
std::shared_ptr<TexgineFontManager> TexgineFontManager::RefDefault()
{
    return g_fcMockVars.fontMgr;
}

std::shared_ptr<TexgineTypeface> TexgineFontManager::MatchFamilyStyleCharacter(const std::string &familyName,
    const TexgineFontStyle &style, const char *bcp47[], int bcp47Count, int32_t character)
{
    g_fcMockVars.catchedSize = bcp47Count;
    return g_fcMockVars.SCRetvalTypeface;
}
#endif

std::shared_ptr<TexgineTypeface> VariantFontStyleSet::MatchStyle(std::shared_ptr<TexgineFontStyle> pattern)
{
    return g_fcMockVars.styleRetvalTypeface;
}

bool Typeface::Has(uint32_t ch)
{
    g_fcMockVars.hasCount++;
    return g_fcMockVars.hasRetval;
}

std::vector<std::shared_ptr<VariantFontStyleSet>> CreateSets()
{
    std::vector<std::shared_ptr<VariantFontStyleSet>> sets;
    auto tfss = std::make_shared<TexgineFontStyleSet>(nullptr);
    sets.push_back(std::make_shared<VariantFontStyleSet>(tfss));
    return sets;
}

void InitFcMockVars(MockVars vars)
{
    g_fcMockVars = vars;
    g_fcMockVars.fontCollection = std::make_shared<FontCollection>(std::move(g_fcMockVars.fontStyleSets));
}

class FontCollectionTest : public testing::Test {
};

/**
 * @tc.name: FontCollection
 * @tc.desc: Verify the FontCollection
 * @tc.type:FUNC
 * @tc.require: issueI6TIIY
 */
HWTEST_F(FontCollectionTest, FontCollection, TestSize.Level1)
{
    InitFcMockVars({});
    EXPECT_NO_THROW(FontCollection fc(std::move(g_fcMockVars.fontStyleSets)));
}

/**
 * @tc.name: GetTypefaceForChar1
 * @tc.desc: Verify the GetTypefaceForChar
 * @tc.type:FUNC
 * @tc.require: issueI6TIIY
 */
HWTEST_F(FontCollectionTest, GetTypefaceForChar1, TestSize.Level1)
{
    InitFcMockVars({});
    FontStyles style;
    auto tf = g_fcMockVars.fontCollection->GetTypefaceForChar('a', style, "", "");
    EXPECT_NE(tf, nullptr);
    EXPECT_NE(tf->Get(), g_fcMockVars.SCRetvalTypeface);
    EXPECT_EQ(g_fcMockVars.catchedSize, -1);
}

/**
 * @tc.name: GetTypefaceForChar2
 * @tc.desc: Verify the GetTypefaceForChar
 * @tc.type:FUNC
 * @tc.require: issueI6TIIY
 */
HWTEST_F(FontCollectionTest, GetTypefaceForChar2, TestSize.Level1)
{
    InitFcMockVars({.fontMgr = nullptr});
    FontStyles style;
    EXPECT_NE(g_fcMockVars.fontCollection->GetTypefaceForChar('a', style, "zh_CN", ""), nullptr);
    EXPECT_EQ(g_fcMockVars.catchedSize, -1);
}

/**
 * @tc.name: GetTypefaceForChar3
 * @tc.desc: Verify the GetTypefaceForChar
 * @tc.type:FUNC
 * @tc.require: issueI6TIIY
 */
HWTEST_F(FontCollectionTest, GetTypefaceForChar3, TestSize.Level1)
{
    InitFcMockVars({.SCRetvalTypeface = nullptr});
    FontStyles style;
    EXPECT_NE(g_fcMockVars.fontCollection->GetTypefaceForChar('a', style, "", "zh_CN"), nullptr);
    EXPECT_NE(g_fcMockVars.catchedSize, 0);
}

/**
 * @tc.name: GetTypefaceForChar4
 * @tc.desc: Verify the GetTypefaceForChar
 * @tc.type:FUNC
 * @tc.require: issueI6TIIY
 */
HWTEST_F(FontCollectionTest, GetTypefaceForChar4, TestSize.Level1)
{
    InitFcMockVars({});
    FontStyles style;
    auto tf1 = g_fcMockVars.fontCollection->GetTypefaceForChar('a', style, "", "zh_HK");
    EXPECT_NE(tf1, nullptr);
    EXPECT_NE(g_fcMockVars.catchedSize, 0);
    InitFcMockVars({});
    auto tf2 = g_fcMockVars.fontCollection->GetTypefaceForChar('a', style, "", "zh_HK");
    EXPECT_NE(tf2, nullptr);
    EXPECT_EQ(g_fcMockVars.catchedSize, -1);
    EXPECT_NE(tf1->Get(), tf2->Get());
}

/**
 * @tc.name: GetTypefaceForChar5
 * @tc.desc: Verify the GetTypefaceForChar
 * @tc.type:FUNC
 * @tc.require: issueI6TIIY
 */
HWTEST_F(FontCollectionTest, GetTypefaceForChar5, TestSize.Level1)
{
    InitFcMockVars({.fontMgr = nullptr, .styleRetvalTypeface = nullptr, .fontStyleSets = CreateSets()});
    FontStyles style;
    EXPECT_NE(g_fcMockVars.fontCollection->GetTypefaceForChar('a', style, "", "en_US"), nullptr);
}

/**
 * @tc.name: GetTypefaceForChar6
 * @tc.desc: Verify the GetTypefaceForChar
 * @tc.type:FUNC
 * @tc.require: issueI6TIIY
 */
HWTEST_F(FontCollectionTest, GetTypefaceForChar6, TestSize.Level1)
{
    InitFcMockVars({.fontMgr = nullptr, .fontStyleSets = CreateSets()});
    FontStyles style;
    EXPECT_NE(g_fcMockVars.fontCollection->GetTypefaceForChar('a', style, "", "en_US"), nullptr);
}

/**
 * @tc.name: GetTypefaceForChar7
 * @tc.desc: Verify the GetTypefaceForCha
 * @tc.type:FUNC
 * @tc.require: issueI6TIIY
 */
HWTEST_F(FontCollectionTest, GetTypefaceForChar7, TestSize.Level1)
{
    InitFcMockVars({.hasRetval = true, .fontMgr = nullptr, .fontStyleSets = CreateSets()});
    FontStyles style;
    auto tf1 = g_fcMockVars.fontCollection->GetTypefaceForChar('a', style, "", "locale1");
    auto tf2 = g_fcMockVars.fontCollection->GetTypefaceForChar('a', style, "", "locale1");
    EXPECT_NE(tf1, nullptr);
    EXPECT_NE(tf2, nullptr);
    EXPECT_EQ(g_fcMockVars.hasCount, 2);
    EXPECT_EQ(tf1->Get(), tf2->Get());
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
