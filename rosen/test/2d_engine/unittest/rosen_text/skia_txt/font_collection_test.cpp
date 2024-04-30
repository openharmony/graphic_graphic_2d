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

using namespace testing;
using namespace testing::ext;

namespace txt {
class FontCollectionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    static inline std::shared_ptr<FontCollection> fontCollection_ = nullptr;
};

void FontCollectionTest::SetUpTestCase()
{
    fontCollection_ = std::make_shared<FontCollection>();
    if (!fontCollection_) {
        std::cout << "FontCollectionTest::SetUpTestCase error fontCollection_ is nullptr" << std::endl;
    }
}

void FontCollectionTest::TearDownTestCase()
{
}

/*
 * @tc.name: FontCollectionTest001
 * @tc.desc: test for GetFontManagersCount
 * @tc.type: FUNC
 */
HWTEST_F(FontCollectionTest, FontCollectionTest001, TestSize.Level1)
{
    EXPECT_EQ(fontCollection_ != nullptr, true);
    EXPECT_EQ(fontCollection_->GetFontManagersCount(), 0);
}

/*
 * @tc.name: FontCollectionTest002
 * @tc.desc: test for SetupDefaultFontManager
 * @tc.type: FUNC
 */
HWTEST_F(FontCollectionTest, FontCollectionTest002, TestSize.Level1)
{
    EXPECT_EQ(fontCollection_ != nullptr, true);
    fontCollection_->SetupDefaultFontManager();
    EXPECT_EQ(fontCollection_->defaultFontManager_ != nullptr, true);
}

/*
 * @tc.name: FontCollectionTest003
 * @tc.desc: test for SetDefaultFontManager
 * @tc.type: FUNC
 */
HWTEST_F(FontCollectionTest, FontCollectionTest003, TestSize.Level1)
{
    EXPECT_EQ(fontCollection_ != nullptr, true);
    fontCollection_->SetDefaultFontManager(nullptr);
    EXPECT_EQ(fontCollection_->defaultFontManager_, nullptr);
}

/*
 * @tc.name: FontCollectionTest004
 * @tc.desc: test for SetAssetFontManager
 * @tc.type: FUNC
 */
HWTEST_F(FontCollectionTest, FontCollectionTest004, TestSize.Level1)
{
    EXPECT_EQ(fontCollection_ != nullptr, true);
    fontCollection_->SetAssetFontManager(nullptr);
    EXPECT_EQ(fontCollection_->assetFontManager_, nullptr);
}

/*
 * @tc.name: FontCollectionTest005
 * @tc.desc: test for SetDynamicFontManager
 * @tc.type: FUNC
 */
HWTEST_F(FontCollectionTest, FontCollectionTest005, TestSize.Level1)
{
    EXPECT_EQ(fontCollection_ != nullptr, true);
    fontCollection_->SetDynamicFontManager(nullptr);
    EXPECT_EQ(fontCollection_->dynamicFontManager_, nullptr);
}

/*
 * @tc.name: FontCollectionTest006
 * @tc.desc: test for SetTestFontManager
 * @tc.type: FUNC
 */
HWTEST_F(FontCollectionTest, FontCollectionTest006, TestSize.Level1)
{
    EXPECT_EQ(fontCollection_ != nullptr, true);
    fontCollection_->SetTestFontManager(nullptr);
    EXPECT_EQ(fontCollection_->testFontManager_, nullptr);
}


/*
 * @tc.name: FontCollectionTest007
 * @tc.desc: test for GetFontManagerOrder
 * @tc.type: FUNC
 */
HWTEST_F(FontCollectionTest, FontCollectionTest007, TestSize.Level1)
{
    EXPECT_EQ(fontCollection_ != nullptr, true);
    EXPECT_EQ(fontCollection_->GetFontManagerOrder().size(), 0);
}

/*
 * @tc.name: FontCollectionTest008
 * @tc.desc: test for DisableFontFallback
 * @tc.type: FUNC
 */
HWTEST_F(FontCollectionTest, FontCollectionTest008, TestSize.Level1)
{
    EXPECT_EQ(fontCollection_ != nullptr, true);
    fontCollection_->DisableFontFallback();
    EXPECT_EQ(fontCollection_->sktFontCollection_, nullptr);
}

/*
 * @tc.name: FontCollectionTest009
 * @tc.desc: test for ClearFontFamilyCache
 * @tc.type: FUNC
 */
HWTEST_F(FontCollectionTest, FontCollectionTest009, TestSize.Level1)
{
    EXPECT_EQ(fontCollection_ != nullptr, true);
    fontCollection_->ClearFontFamilyCache();
    EXPECT_EQ(fontCollection_->sktFontCollection_, nullptr);
}

/*
 * @tc.name: FontCollectionTest010
 * @tc.desc: test for CreateSktFontCollection
 * @tc.type: FUNC
 */
HWTEST_F(FontCollectionTest, FontCollectionTest010, TestSize.Level1)
{
    EXPECT_EQ(fontCollection_ != nullptr, true);
    EXPECT_EQ(fontCollection_->CreateSktFontCollection() != nullptr, true);
}
} // namespace txt