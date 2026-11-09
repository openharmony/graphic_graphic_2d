/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.. All rights reserved.
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

#include <algorithm>

#include "platform.h"
#include "text_style.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen;

namespace txt {
class DefaultFamilyNameMgrTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};

// The singleton must hand out one and the same instance for the whole process.
// A duplicated definition (one copy per so) would give each so its own static
// instance, and symbol interposition could bind reads and writes to different
// copies.
HWTEST_F(DefaultFamilyNameMgrTest, GetInstanceReturnsSameInstance, TestSize.Level0)
{
    SPText::DefaultFamilyNameMgr& first = SPText::DefaultFamilyNameMgr::GetInstance();
    SPText::DefaultFamilyNameMgr& second = SPText::DefaultFamilyNameMgr::GetInstance();
    EXPECT_EQ(&first, &second);
}

// Regression guard for the watch bold-weight issue: a never-constructed
// (all-zero) instance used to be read here and returned an empty list, which
// then wiped the skparagraph default family seed and dropped every wght axis.
HWTEST_F(DefaultFamilyNameMgrTest, GetDefaultFontFamiliesIsNeverEmpty, TestSize.Level0)
{
    std::vector<std::string> families = SPText::DefaultFamilyNameMgr::GetInstance().GetDefaultFontFamilies();
    EXPECT_FALSE(families.empty());
    bool hasSeed = false;
    for (const std::string& family : families) {
        EXPECT_FALSE(family.empty());
        if (family == "HarmonyOS-Sans") {
            hasSeed = true;
        }
    }
    EXPECT_TRUE(hasSeed);
}

// Writes through ModifyThemeFontFamilies must be visible to subsequent reads;
// the combined list is exactly the theme entries followed by an unchanged
// default seed, whatever earlier cases left the theme list in.
HWTEST_F(DefaultFamilyNameMgrTest, ThemeFamiliesWriteThenReadIsConsistent, TestSize.Level0)
{
    SPText::DefaultFamilyNameMgr& mgr = SPText::DefaultFamilyNameMgr::GetInstance();
    // take a clean theme baseline so expectations do not depend on execution order
    mgr.ModifyThemeFontFamilies(0);
    const std::vector<std::string> defaultSeed = mgr.GetDefaultFontFamilies();
    ASSERT_FALSE(defaultSeed.empty());

    const size_t themeCount = 2;
    mgr.ModifyThemeFontFamilies(themeCount);
    EXPECT_EQ(mgr.GetThemeFontFamilies().size(), themeCount);
    EXPECT_EQ(mgr.GetThemeFontFamilies().front(), SPText::OHOS_THEME_FONT);
    std::string indexedFamily = SPText::OHOS_THEME_FONT;
    indexedFamily.push_back('1');
    EXPECT_EQ(mgr.GetThemeFontFamilies().back(), indexedFamily);

    std::vector<std::string> combined = mgr.GetDefaultFontFamilies();
    ASSERT_EQ(combined.size(), themeCount + defaultSeed.size());
    EXPECT_TRUE(std::equal(combined.begin(), combined.begin() + themeCount, mgr.GetThemeFontFamilies().begin()));
    EXPECT_TRUE(std::equal(combined.begin() + themeCount, combined.end(), defaultSeed.begin()));

    // restore shared state for other test cases
    mgr.ModifyThemeFontFamilies(0);
    EXPECT_TRUE(mgr.GetThemeFontFamilies().empty());
    EXPECT_EQ(mgr.GetDefaultFontFamilies(), defaultSeed);
}

HWTEST_F(DefaultFamilyNameMgrTest, IsThemeFontFamilyMatchesPrefix, TestSize.Level0)
{
    EXPECT_TRUE(SPText::DefaultFamilyNameMgr::IsThemeFontFamily(SPText::OHOS_THEME_FONT));
    EXPECT_TRUE(SPText::DefaultFamilyNameMgr::IsThemeFontFamily("OhosThemeFont2"));
    EXPECT_FALSE(SPText::DefaultFamilyNameMgr::IsThemeFontFamily("HarmonyOS-Sans"));
    EXPECT_FALSE(SPText::DefaultFamilyNameMgr::IsThemeFontFamily(""));
}

HWTEST_F(DefaultFamilyNameMgrTest, GenerateThemeFamilyName, TestSize.Level0)
{
    EXPECT_EQ(SPText::DefaultFamilyNameMgr::GenerateThemeFamilyName(0), SPText::OHOS_THEME_FONT);
    std::string indexedFamily = SPText::OHOS_THEME_FONT;
    indexedFamily.push_back('1');
    EXPECT_EQ(SPText::DefaultFamilyNameMgr::GenerateThemeFamilyName(1), indexedFamily);
}

// TextStyle construction is the first consumer of the default family list;
// its default must mirror exactly what the singleton reports.
HWTEST_F(DefaultFamilyNameMgrTest, TextStyleDefaultsFollowSingleton, TestSize.Level0)
{
    TextStyle style;
    EXPECT_FALSE(style.fontFamilies.empty());
    EXPECT_EQ(style.fontFamilies, SPText::DefaultFamilyNameMgr::GetInstance().GetDefaultFontFamilies());
}

// GetDefaultFontManager moved to the 2d_graphics-owned copy together with the
// singleton; make sure it still resolves and returns a usable manager.
HWTEST_F(DefaultFamilyNameMgrTest, GetDefaultFontManagerIsUsable, TestSize.Level0)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr = SPText::GetDefaultFontManager();
    EXPECT_NE(fontMgr, nullptr);
}
} // namespace txt
