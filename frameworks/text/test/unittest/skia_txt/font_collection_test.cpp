/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "font_collection.h"
#include "paragraph_builder.h"
#include "platform.h"
#include "rosen_text/font_collection.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen;

namespace txt {
constexpr float TEST_FONT_SIZE = 50;
constexpr float TEST_LAYOUT_WIDTH = 500;

class FontCollectionTest : public testing::Test {
public:
    static void BuildAndLayout(const std::shared_ptr<txt::FontCollection>& fontCollection,
        const std::u16string& text)
    {
        SPText::ParagraphStyle paragraphStyle;
        auto paragraphBuilder = SPText::ParagraphBuilder::Create(paragraphStyle, fontCollection);
        ASSERT_NE(paragraphBuilder, nullptr);
        OHOS::Rosen::SPText::TextStyle style;
        style.fontSize = TEST_FONT_SIZE;
        paragraphBuilder->PushStyle(style);
        paragraphBuilder->AddText(text);
        auto paragraph = paragraphBuilder->Build();
        ASSERT_NE(paragraph, nullptr);
        paragraph->Layout(TEST_LAYOUT_WIDTH);
    }
};

/*
 * @tc.name: FontCollectionTest001
 * @tc.desc: test for GetFontManagersCount and SetupDefaultFontManager and SetDefaultFontManager
 * @tc.type: FUNC
 */
HWTEST_F(FontCollectionTest, FontCollectionTest001, TestSize.Level0)
{
    FontCollection fontCollection;
    EXPECT_EQ(fontCollection.GetFontManagersCount(), 0);
    fontCollection.SetupDefaultFontManager();
    EXPECT_EQ(fontCollection.GetFontManagersCount(), 1);
    EXPECT_NE(fontCollection.defaultFontManager_, nullptr);
    fontCollection.SetDefaultFontManager(nullptr);
    EXPECT_EQ(fontCollection.GetFontManagersCount(), 0);
    EXPECT_EQ(fontCollection.defaultFontManager_, nullptr);
}

/*
 * @tc.name: FontCollectionTest004
 * @tc.desc: test for SetAssetFontManager
 * @tc.type: FUNC
 */
HWTEST_F(FontCollectionTest, FontCollectionTest004, TestSize.Level0)
{
    FontCollection fontCollection;
    fontCollection.SetAssetFontManager(OHOS::Rosen::SPText::GetDefaultFontManager());
    EXPECT_NE(fontCollection.assetFontManager_, nullptr);
    EXPECT_EQ(fontCollection.GetFontManagersCount(), 1);
}

/*
 * @tc.name: FontCollectionTest005
 * @tc.desc: test for SetDynamicFontManager
 * @tc.type: FUNC
 */
HWTEST_F(FontCollectionTest, FontCollectionTest005, TestSize.Level0)
{
    FontCollection fontCollection;
    fontCollection.SetDynamicFontManager(OHOS::Rosen::Drawing::FontMgr::CreateDynamicFontMgr());
    EXPECT_NE(fontCollection.dynamicFontManager_, nullptr);
    EXPECT_EQ(fontCollection.GetFontManagersCount(), 1);
}

/*
 * @tc.name: FontCollectionTest006
 * @tc.desc: test for SetTestFontManager
 * @tc.type: FUNC
 */
HWTEST_F(FontCollectionTest, FontCollectionTest006, TestSize.Level0)
{
    FontCollection fontCollection;
    fontCollection.SetTestFontManager(OHOS::Rosen::SPText::GetDefaultFontManager());
    EXPECT_NE(fontCollection.testFontManager_, nullptr);
    EXPECT_EQ(fontCollection.GetFontManagersCount(), 1);
}

/*
 * @tc.name: FontCollectionTest007
 * @tc.desc: test for GetFontManagerOrder
 * @tc.type: FUNC
 */
HWTEST_F(FontCollectionTest, FontCollectionTest007, TestSize.Level0)
{
    FontCollection fontCollection;
    fontCollection.SetupDefaultFontManager();
    fontCollection.SetAssetFontManager(OHOS::Rosen::SPText::GetDefaultFontManager());
    fontCollection.SetDynamicFontManager(OHOS::Rosen::Drawing::FontMgr::CreateDynamicFontMgr());
    fontCollection.SetTestFontManager(OHOS::Rosen::SPText::GetDefaultFontManager());
    fontCollection.SetGlobalFontManager(OHOS::Rosen::FontCollection::Create()->GetFontMgr());
    EXPECT_EQ(fontCollection.GetFontManagersCount(), 5);
    std::vector<std::shared_ptr<RSFontMgr>> fontManagerOrder = fontCollection.GetFontManagerOrder();
    EXPECT_EQ(fontManagerOrder.at(0), fontCollection.dynamicFontManager_);
    EXPECT_EQ(fontManagerOrder.at(1), OHOS::Rosen::FontCollection::Create()->GetFontMgr());
    EXPECT_EQ(fontManagerOrder.at(2), fontCollection.assetFontManager_);
    EXPECT_EQ(fontManagerOrder.at(3), fontCollection.testFontManager_);
    EXPECT_EQ(fontManagerOrder.at(4), fontCollection.defaultFontManager_);
}

/*
 * @tc.name: FontCollectionTest008
 * @tc.desc: test for DisableFontFallback
 * @tc.type: FUNC
 */
HWTEST_F(FontCollectionTest, FontCollectionTest008, TestSize.Level0)
{
    FontCollection fontCollection;
    fontCollection.CreateSktFontCollection();
    ASSERT_NE(fontCollection.sktFontCollection_, nullptr);
    fontCollection.DisableFontFallback();
    EXPECT_EQ(fontCollection.sktFontCollection_->fontFallbackEnabled(), false);
    EXPECT_EQ(fontCollection.enableFontFallback_, false);
}

/*
 * @tc.name: FontCollectionTest009
 * @tc.desc: test for ClearFontFamilyCache
 * @tc.type: FUNC
 */
HWTEST_F(FontCollectionTest, FontCollectionTest009, TestSize.Level0)
{
    FontCollection fontCollection;
    fontCollection.CreateSktFontCollection();
    ASSERT_NE(fontCollection.sktFontCollection_, nullptr);
    fontCollection.ClearFontFamilyCache();
    EXPECT_EQ(fontCollection.sktFontCollection_->getParagraphCache()->count(), 0);
}

/*
 * @tc.name: FontCollectionTest010
 * @tc.desc: test for SetCachesEnabled(false) - clears existing cache entries
 * @tc.type: FUNC
 */
HWTEST_F(FontCollectionTest, FontCollectionTest010, TestSize.Level0)
{
    auto fontCollection = std::make_shared<txt::FontCollection>();
    ASSERT_NE(fontCollection, nullptr);
    fontCollection->SetupDefaultFontManager();
    auto paragraphCache = fontCollection->CreateSktFontCollection()->getParagraphCache();
    ASSERT_NE(paragraphCache, nullptr);

    BuildAndLayout(fontCollection, u"Test Cache Clear, This is an english text.");
    // Verify cache has entries before disabling
    EXPECT_GT(paragraphCache->count(), 0);

    // Disable cache should clear all existing entries
    fontCollection->SetCachesEnabled(false);
    EXPECT_EQ(paragraphCache->count(), 0);
}

/*
 * @tc.name: FontCollectionTest011
 * @tc.desc: test for SetCachesEnabled(false) - cache disabled, Layout does not write to cache
 * @tc.type: FUNC
 */
HWTEST_F(FontCollectionTest, FontCollectionTest011, TestSize.Level0)
{
    auto fontCollection = std::make_shared<txt::FontCollection>();
    ASSERT_NE(fontCollection, nullptr);
    fontCollection->SetupDefaultFontManager();
    auto paragraphCache = fontCollection->CreateSktFontCollection()->getParagraphCache();
    ASSERT_NE(paragraphCache, nullptr);

    // Disable cache before any Layout
    fontCollection->SetCachesEnabled(false);
    EXPECT_EQ(paragraphCache->count(), 0);

    BuildAndLayout(fontCollection, u"Cache disabled test, This is an english text.");
    // Cache is off, updateParagraph should not write, count stays 0
    EXPECT_EQ(paragraphCache->count(), 0);
}

/*
 * @tc.name: FontCollectionTest012
 * @tc.desc: test for SetCachesEnabled - cache re-enabled after disable, Layout writes to cache
 * @tc.type: FUNC
 */
HWTEST_F(FontCollectionTest, FontCollectionTest012, TestSize.Level0)
{
    auto fontCollection = std::make_shared<txt::FontCollection>();
    ASSERT_NE(fontCollection, nullptr);
    fontCollection->SetupDefaultFontManager();
    auto paragraphCache = fontCollection->CreateSktFontCollection()->getParagraphCache();
    ASSERT_NE(paragraphCache, nullptr);

    // Disable then re-enable cache
    fontCollection->SetCachesEnabled(false);
    fontCollection->SetCachesEnabled(true);
    EXPECT_EQ(paragraphCache->count(), 0);

    BuildAndLayout(fontCollection, u"Cache re-enable test, This is an english text.");
    // Cache re-enabled, updateParagraph should write after Layout
    EXPECT_GT(paragraphCache->count(), 0);
}

/*
 * @tc.name: FontCollectionTest014
 * @tc.desc: test for SetCachesEnabled called before CreateSktFontCollection has no effect
 * @tc.type: FUNC
 */
HWTEST_F(FontCollectionTest, FontCollectionTest014, TestSize.Level0)
{
    auto fontCollection = std::make_shared<txt::FontCollection>();
    ASSERT_NE(fontCollection, nullptr);
    fontCollection->SetupDefaultFontManager();

    // Disable cache BEFORE creating skFontCollection - this should have no effect
    fontCollection->SetCachesEnabled(false);

    auto skFontCollection = fontCollection->CreateSktFontCollection();
    ASSERT_NE(skFontCollection, nullptr);
    auto paragraphCache = skFontCollection->getParagraphCache();
    ASSERT_NE(paragraphCache, nullptr);

    SPText::ParagraphStyle paragraphStyle;
    std::unique_ptr<SPText::ParagraphBuilder> paragraphBuilder =
        SPText::ParagraphBuilder::Create(paragraphStyle, fontCollection);
    ASSERT_NE(paragraphBuilder, nullptr);

    OHOS::Rosen::SPText::TextStyle style;
    style.fontSize = TEST_FONT_SIZE;
    paragraphBuilder->PushStyle(style);
    paragraphBuilder->AddText(u"Test with cache pre-disabled.");
    auto paragraph = paragraphBuilder->Build();
    ASSERT_NE(paragraph, nullptr);
    paragraph->Layout(TEST_LAYOUT_WIDTH);

    // SetCachesEnabled(false) was called before CreateSktFontCollection, so it had no effect
    // Cache is still enabled, updateParagraph should have written after Layout
    EXPECT_GT(paragraphCache->count(), 0);
}
} // namespace txt