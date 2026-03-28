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
#include <chrono>
#include "font_collection.h"
#include "paragraph_builder.h"
#include "paragraph_impl.h"
#include "platform.h"
#include "rosen_text/font_collection.h"
#include "modules/skparagraph/src/ParagraphImpl.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen;

namespace txt {
class FontCollectionTest : public testing::Test {};

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
 * @tc.desc: test for SetCachesEnabled - verify open cache works correctly
 * @tc.type: FUNC
 */
HWTEST_F(FontCollectionTest, FontCollectionTest010, TestSize.Level0)
{
    auto fontCollection = std::make_shared<txt::FontCollection>();
    ASSERT_NE(fontCollection, nullptr);
    fontCollection->SetupDefaultFontManager();
    auto skFontCollection = fontCollection->CreateSktFontCollection();
    ASSERT_NE(skFontCollection, nullptr);
    auto paragraphCache = skFontCollection->getParagraphCache();
    EXPECT_NE(paragraphCache, nullptr);

    SPText::ParagraphStyle paragraphStyle;
    std::unique_ptr<SPText::ParagraphBuilder> paragraphBuilder =
        SPText::ParagraphBuilder::Create(paragraphStyle, fontCollection);
    ASSERT_NE(paragraphBuilder, nullptr);

    OHOS::Rosen::SPText::TextStyle style;
    style.fontSize = 50;
    paragraphBuilder->PushStyle(style);
    paragraphBuilder->AddText(u"Hello World Test, This is an english text.");
    std::shared_ptr<SPText::Paragraph> paragraph = paragraphBuilder->Build();
    ASSERT_NE(paragraph, nullptr);
    paragraph->Layout(500);

    fontCollection->SetCachesEnabled(false);

    // Get the Skia paragraph impl for findParagraph verification
    auto txtParagraphImpl = reinterpret_cast<SPText::ParagraphImpl*>(paragraph.get());
    ASSERT_NE(txtParagraphImpl, nullptr);
    auto skParagraph = reinterpret_cast<skia::textlayout::ParagraphImpl*>(txtParagraphImpl->paragraph_.get());
    ASSERT_NE(skParagraph, nullptr);

    // With cache disabled, findParagraph should return false
    EXPECT_FALSE(paragraphCache->findParagraph(skParagraph));

    // Re-enable cache for cleanup
    fontCollection->SetCachesEnabled(true);
    paragraph->Layout(500);
    EXPECT_TRUE(paragraphCache->findParagraph(skParagraph));
}

/*
 * @tc.name: FontCollectionTest011
 * @tc.desc: test for SetCachesEnabled - verify close cache works correctly
 * @tc.type: FUNC
 */
HWTEST_F(FontCollectionTest, FontCollectionTest011, TestSize.Level0)
{
    auto fontCollection = std::make_shared<txt::FontCollection>();
    ASSERT_NE(fontCollection, nullptr);
    fontCollection->SetupDefaultFontManager();
    auto skFontCollection = fontCollection->CreateSktFontCollection();
    ASSERT_NE(skFontCollection, nullptr);
    auto paragraphCache = skFontCollection->getParagraphCache();
    EXPECT_NE(paragraphCache, nullptr);

    SPText::ParagraphStyle paragraphStyle;
    std::unique_ptr<SPText::ParagraphBuilder> paragraphBuilder =
        SPText::ParagraphBuilder::Create(paragraphStyle, fontCollection);
    ASSERT_NE(paragraphBuilder, nullptr);

    OHOS::Rosen::SPText::TextStyle style;
    style.fontSize = 50;
    paragraphBuilder->PushStyle(style);
    paragraphBuilder->AddText(u"Hello World Test, This is an english text.");
    std::shared_ptr<SPText::Paragraph> paragraph = paragraphBuilder->Build();
    ASSERT_NE(paragraph, nullptr);
    paragraph->Layout(500);

    fontCollection->SetCachesEnabled(false);

    // Get the Skia paragraph impl for findParagraph verification
    auto txtParagraphImpl = reinterpret_cast<SPText::ParagraphImpl*>(paragraph.get());
    ASSERT_NE(txtParagraphImpl, nullptr);
    auto skParagraph = reinterpret_cast<skia::textlayout::ParagraphImpl*>(txtParagraphImpl->paragraph_.get());
    ASSERT_NE(skParagraph, nullptr);

    // With cache disabled, findParagraph should return false
    EXPECT_FALSE(paragraphCache->findParagraph(skParagraph));
}

/*
 * @tc.name: FontCollectionTest012
 * @tc.desc: test for SetCachesEnabled - verify cache is cleared when disabled
 * @tc.type: FUNC
 */
HWTEST_F(FontCollectionTest, FontCollectionTest012, TestSize.Level0)
{
    auto fontCollection = std::make_shared<txt::FontCollection>();
    ASSERT_NE(fontCollection, nullptr);
    fontCollection->SetupDefaultFontManager();
    auto skFontCollection = fontCollection->CreateSktFontCollection();
    ASSERT_NE(skFontCollection, nullptr);
    auto paragraphCache = skFontCollection->getParagraphCache();
    EXPECT_NE(paragraphCache, nullptr);

    EXPECT_EQ(paragraphCache->count(), 0);

    SPText::ParagraphStyle paragraphStyle;
    std::unique_ptr<SPText::ParagraphBuilder> paragraphBuilder =
        SPText::ParagraphBuilder::Create(paragraphStyle, fontCollection);
    ASSERT_NE(paragraphBuilder, nullptr);

    OHOS::Rosen::SPText::TextStyle style;
    style.fontSize = 50;
    paragraphBuilder->PushStyle(style);
    paragraphBuilder->AddText(u"Test Cache Clear, 这是一段测试文本。");
    std::shared_ptr<SPText::Paragraph> paragraph = paragraphBuilder->Build();
    ASSERT_NE(paragraph, nullptr);
    paragraph->Layout(500);

    // Cache should have entries after layout
    int countAfterLayout = paragraphCache->count();
    EXPECT_GT(countAfterLayout, 0);

    // Disable cache - this should clear the cache
    fontCollection->SetCachesEnabled(false);

    // Verify cache is cleared (count should be 0)
    EXPECT_EQ(paragraphCache->count(), 0);
}

/*
 * @tc.name: FontCollectionTest013
 * @tc.desc: test for SetCachesEnabled combined with ClearCaches - cache remains functional after clear
 * @tc.type: FUNC
 */
HWTEST_F(FontCollectionTest, FontCollectionTest013, TestSize.Level0)
{
    auto fontCollection = std::make_shared<txt::FontCollection>();
    ASSERT_NE(fontCollection, nullptr);
    fontCollection->SetupDefaultFontManager();
    fontCollection->SetCachesEnabled(true);
    auto skFontCollection = fontCollection->CreateSktFontCollection();
    ASSERT_NE(skFontCollection, nullptr);
    auto paragraphCache = skFontCollection->getParagraphCache();
    ASSERT_NE(paragraphCache, nullptr);

    SPText::ParagraphStyle paragraphStyle;
    auto buildParagraph = [&]() -> std::shared_ptr<SPText::Paragraph> {
        std::unique_ptr<SPText::ParagraphBuilder> builder =
            SPText::ParagraphBuilder::Create(paragraphStyle, fontCollection);
        EXPECT_NE(builder, nullptr);
        OHOS::Rosen::SPText::TextStyle style;
        style.fontSize = 40;
        builder->PushStyle(style);
        builder->AddText(u"ClearCaches combination test text.");
        auto paragraph = builder->Build();
        EXPECT_NE(paragraph, nullptr);
        paragraph->Layout(500);
        return paragraph;
    };

    // Create paragraph with cache enabled - cache should have entries
    fontCollection->ClearFontFamilyCache();
    EXPECT_EQ(paragraphCache->count(), 0);

    auto paragraph1 = buildParagraph();
    ASSERT_NE(paragraph1, nullptr);
    int countAfterFirst = paragraphCache->count();
    EXPECT_GT(countAfterFirst, 0);

    // verify that the cache is closed, it will not continue to cache content

    fontCollection->SetCachesEnabled(false);

    auto paragraph2 = buildParagraph();
    ASSERT_NE(paragraph2, nullptr);
    int countAfterThird = paragraphCache->count();
    EXPECT_EQ(countAfterThird, 0);

    auto paragraph3 = buildParagraph();
    ASSERT_NE(paragraph3, nullptr);
    int countAfterFour = paragraphCache->count();
    EXPECT_EQ(countAfterFour, 0);
}
} // namespace txt