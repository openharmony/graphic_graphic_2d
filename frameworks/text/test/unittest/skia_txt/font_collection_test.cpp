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
#include "platform.h"

using namespace testing;
using namespace testing::ext;

namespace txt {
class FontCollectionTest : public testing::Test {};

/*
 * @tc.name: FontCollectionTest001
 * @tc.desc: test for GetFontManagersCount and SetupDefaultFontManager and SetDefaultFontManager
 * @tc.type: FUNC
 */
HWTEST_F(FontCollectionTest, FontCollectionTest001, TestSize.Level1)
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
HWTEST_F(FontCollectionTest, FontCollectionTest004, TestSize.Level1)
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
HWTEST_F(FontCollectionTest, FontCollectionTest005, TestSize.Level1)
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
HWTEST_F(FontCollectionTest, FontCollectionTest006, TestSize.Level1)
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
HWTEST_F(FontCollectionTest, FontCollectionTest007, TestSize.Level1)
{
    FontCollection fontCollection;
    fontCollection.SetupDefaultFontManager();
    fontCollection.SetAssetFontManager(OHOS::Rosen::SPText::GetDefaultFontManager());
    fontCollection.SetDynamicFontManager(OHOS::Rosen::Drawing::FontMgr::CreateDynamicFontMgr());
    fontCollection.SetTestFontManager(OHOS::Rosen::SPText::GetDefaultFontManager());
    EXPECT_EQ(fontCollection.GetFontManagersCount(), 4);
    std::vector<std::shared_ptr<RSFontMgr>> fontManagerOrder = fontCollection.GetFontManagerOrder();
    EXPECT_EQ(fontManagerOrder.at(0), fontCollection.dynamicFontManager_);
    EXPECT_EQ(fontManagerOrder.at(1), fontCollection.assetFontManager_);
    EXPECT_EQ(fontManagerOrder.at(2), fontCollection.testFontManager_);
    EXPECT_EQ(fontManagerOrder.at(3), fontCollection.defaultFontManager_);
}

/*
 * @tc.name: FontCollectionTest008
 * @tc.desc: test for DisableFontFallback
 * @tc.type: FUNC
 */
HWTEST_F(FontCollectionTest, FontCollectionTest008, TestSize.Level1)
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
HWTEST_F(FontCollectionTest, FontCollectionTest009, TestSize.Level1)
{
    FontCollection fontCollection;
    fontCollection.CreateSktFontCollection();
    ASSERT_NE(fontCollection.sktFontCollection_, nullptr);
    fontCollection.ClearFontFamilyCache();
    EXPECT_EQ(fontCollection.sktFontCollection_->getParagraphCache()->count(), 0);
}

} // namespace txt