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
    int catchedSize_ = -1;
    bool hasRetval_ = false;
    int hasCount_ = 0;
    std::shared_ptr<TexgineFontManager> fontMgr_ = std::make_shared<TexgineFontManager>();
    std::shared_ptr<TexgineTypeface> SCRetvalTypeface_ = std::make_shared<TexgineTypeface>();
    std::shared_ptr<TexgineTypeface> styleRetvalTypeface_ = std::make_shared<TexgineTypeface>();
    std::vector<std::shared_ptr<VariantFontStyleSet>> fontStyleSets_;
    std::shared_ptr<Texgine::FontCollection> fontCollection_;
} fcMockVars;

std::shared_ptr<TexgineFontManager> TexgineFontManager::RefDefault()
{
    return fcMockVars.fontMgr_;
}

std::shared_ptr<TexgineTypeface> TexgineFontManager::MatchFamilyStyleCharacter(const char familyName[],
    TexgineFontStyle &style, const char *bcp47[], int bcp47Count, int32_t character)
{
    fcMockVars.catchedSize_ = bcp47Count;
    return fcMockVars.SCRetvalTypeface_;
}

std::shared_ptr<TexgineTypeface> VariantFontStyleSet::MatchStyle(std::shared_ptr<TexgineFontStyle> pattern)
{
    return fcMockVars.styleRetvalTypeface_;
}

bool Typeface::Has(uint32_t ch)
{
    fcMockVars.hasCount_++;
    return fcMockVars.hasRetval_;
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
    fcMockVars = vars;
    fcMockVars.fontCollection_ = std::make_shared<FontCollection>(std::move(fcMockVars.fontStyleSets_));
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
    EXPECT_NO_THROW(FontCollection fc(std::move(fcMockVars.fontStyleSets_)));
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
    auto tf = fcMockVars.fontCollection_->GetTypefaceForChar('a', {}, "", "");
    EXPECT_NE(tf, nullptr);
    EXPECT_EQ(tf->Get(), fcMockVars.SCRetvalTypeface_);
    EXPECT_EQ(fcMockVars.catchedSize_, 0);
}

/**
 * @tc.name: GetTypefaceForChar2
 * @tc.desc: Verify the GetTypefaceForChar
 * @tc.type:FUNC
 * @tc.require: issueI6TIIY
 */
HWTEST_F(FontCollectionTest, GetTypefaceForChar2, TestSize.Level1)
{
    InitFcMockVars({.fontMgr_ = nullptr});
    EXPECT_EQ(fcMockVars.fontCollection_->GetTypefaceForChar('a', {}, "zh_CN", ""), nullptr);
    EXPECT_EQ(fcMockVars.catchedSize_, -1);
}

/**
 * @tc.name: GetTypefaceForChar3
 * @tc.desc: Verify the GetTypefaceForChar
 * @tc.type:FUNC
 * @tc.require: issueI6TIIY
 */
HWTEST_F(FontCollectionTest, GetTypefaceForChar3, TestSize.Level1)
{
    InitFcMockVars({.SCRetvalTypeface_ = nullptr});
    EXPECT_EQ(fcMockVars.fontCollection_->GetTypefaceForChar('a', {}, "", "zh_CN"), nullptr);
    EXPECT_NE(fcMockVars.catchedSize_, 0);
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
    auto tf1 = fcMockVars.fontCollection_->GetTypefaceForChar('a', {}, "", "zh_HK");
    EXPECT_NE(tf1, nullptr);
    EXPECT_NE(fcMockVars.catchedSize_, 0);
    InitFcMockVars({});
    auto tf2 = fcMockVars.fontCollection_->GetTypefaceForChar('a', {}, "", "zh_HK");
    EXPECT_NE(tf2, nullptr);
    EXPECT_EQ(fcMockVars.catchedSize_, -1);
    EXPECT_EQ(tf1->Get(), tf2->Get());
}

/**
 * @tc.name: GetTypefaceForChar5
 * @tc.desc: Verify the GetTypefaceForChar
 * @tc.type:FUNC
 * @tc.require: issueI6TIIY
 */
HWTEST_F(FontCollectionTest, GetTypefaceForChar5, TestSize.Level1)
{
    InitFcMockVars({.fontMgr_ = nullptr, .styleRetvalTypeface_ = nullptr, .fontStyleSets_ = CreateSets()});
    EXPECT_EQ(fcMockVars.fontCollection_->GetTypefaceForChar('a', {}, "", "en_US"), nullptr);
}

/**
 * @tc.name: GetTypefaceForChar6
 * @tc.desc: Verify the GetTypefaceForChar
 * @tc.type:FUNC
 * @tc.require: issueI6TIIY
 */
HWTEST_F(FontCollectionTest, GetTypefaceForChar6, TestSize.Level1)
{
    InitFcMockVars({.fontMgr_ = nullptr, .fontStyleSets_ = CreateSets()});
    EXPECT_EQ(fcMockVars.fontCollection_->GetTypefaceForChar('a', {}, "", "en_US"), nullptr);
}

/**
 * @tc.name: GetTypefaceForChar7
 * @tc.desc: Verify the GetTypefaceForCha
 * @tc.type:FUNC
 * @tc.require: issueI6TIIY
 */
HWTEST_F(FontCollectionTest, GetTypefaceForChar7, TestSize.Level1)
{
    InitFcMockVars({.hasRetval_ = true, .fontMgr_ = nullptr, .fontStyleSets_ = CreateSets()});
    auto tf1 = fcMockVars.fontCollection_->GetTypefaceForChar('a', {}, "", "locale1");
    auto tf2 = fcMockVars.fontCollection_->GetTypefaceForChar('a', {}, "", "locale1");
    EXPECT_NE(tf1, nullptr);
    EXPECT_NE(tf2, nullptr);
    EXPECT_EQ(fcMockVars.hasCount_, 2);
    EXPECT_EQ(tf1->Get(), tf2->Get());
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
