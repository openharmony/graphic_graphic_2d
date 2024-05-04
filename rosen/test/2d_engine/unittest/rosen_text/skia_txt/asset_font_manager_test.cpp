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
#include "asset_font_manager.h"
#include "include/core/SkString.h"
#include "include/core/SkTypeface.h"
#include "typeface_font_asset_provider.h"

using namespace testing;
using namespace testing::ext;

namespace txt {
class AssetFontManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    static inline std::shared_ptr<AssetFontManager> assetFontManager_ = nullptr;
};

void AssetFontManagerTest::SetUpTestCase()
{
    std::unique_ptr<TypefaceFontAssetProvider> fontProvider = std::make_unique<TypefaceFontAssetProvider>();
    if (!fontProvider) {
        std::cout << "AssetFontManagerTest::SetUpTestCase error fontProvider is nullptr" << std::endl;
        return;
    }
    std::string familyName("test");
    fontProvider->familyNames_.emplace_back(familyName);
    assetFontManager_ = std::make_shared<AssetFontManager>(std::move(fontProvider));
    if (!assetFontManager_) {
        std::cout << "AssetFontManagerTest::SetUpTestCase error assetFontManager_ is nullptr" << std::endl;
        return;
    }
}

void AssetFontManagerTest::TearDownTestCase()
{
}

/*
 * @tc.name: AssetFontManagerTest001
 * @tc.desc: test for onMatchFamily
 * @tc.type: FUNC
 */
HWTEST_F(AssetFontManagerTest, AssetFontManagerTest001, TestSize.Level1)
{
    EXPECT_EQ(assetFontManager_ != nullptr, true);
    std::string familyName("test");
    EXPECT_EQ(assetFontManager_->onMatchFamily(familyName.c_str()), nullptr);
}

/*
 * @tc.name: AssetFontManagerTest002
 * @tc.desc: test for onCountFamilies
 * @tc.type: FUNC
 */
HWTEST_F(AssetFontManagerTest, AssetFontManagerTest002, TestSize.Level1)
{
    EXPECT_EQ(assetFontManager_ != nullptr, true);
    std::string testFamilyName("test");
    SkString familyName(testFamilyName);
    assetFontManager_->onGetFamilyName(0, &familyName);
    EXPECT_EQ(assetFontManager_->onCountFamilies() > 0, true);
}

/*
 * @tc.name: AssetFontManagerTest003
 * @tc.desc: test for onCreateStyleSet
 * @tc.type: FUNC
 */
HWTEST_F(AssetFontManagerTest, AssetFontManagerTest003, TestSize.Level1)
{
    EXPECT_EQ(assetFontManager_ != nullptr, true);
    EXPECT_EQ(assetFontManager_->onCreateStyleSet(0), nullptr);
}

/*
 * @tc.name: AssetFontManagerTest004
 * @tc.desc: test for onMatchFamilyStyle
 * @tc.type: FUNC
 */
HWTEST_F(AssetFontManagerTest, AssetFontManagerTest004, TestSize.Level1)
{
    EXPECT_EQ(assetFontManager_ != nullptr, true);
    std::string familyName("test");
    SkFontStyle fontStyle;
    EXPECT_EQ(assetFontManager_->onMatchFamilyStyle(familyName.c_str(), fontStyle), nullptr);
}

/*
 * @tc.name: AssetFontManagerTest005
 * @tc.desc: test for onMatchFamilyStyleCharacter
 * @tc.type: FUNC
 */
HWTEST_F(AssetFontManagerTest, AssetFontManagerTest005, TestSize.Level1)
{
    EXPECT_EQ(assetFontManager_ != nullptr, true);
    std::string familyName("test");
    SkFontStyle fontStyle;
    EXPECT_EQ(assetFontManager_->onMatchFamilyStyleCharacter(familyName.c_str(), fontStyle, nullptr, 0, 0), nullptr);
}

/*
 * @tc.name: AssetFontManagerTest006
 * @tc.desc: test for onMakeFromData
 * @tc.type: FUNC
 */
HWTEST_F(AssetFontManagerTest, AssetFontManagerTest006, TestSize.Level1)
{
    EXPECT_EQ(assetFontManager_ != nullptr, true);
    sk_sp<SkData> data = SkData::MakeEmpty();
    EXPECT_EQ(assetFontManager_->onMakeFromData(data, 0), nullptr);
}

/*
 * @tc.name: AssetFontManagerTest007
 * @tc.desc: test for onMakeFromStreamIndex
 * @tc.type: FUNC
 */
HWTEST_F(AssetFontManagerTest, AssetFontManagerTest007, TestSize.Level1)
{
    EXPECT_EQ(assetFontManager_ != nullptr, true);
    EXPECT_EQ(assetFontManager_->onMakeFromStreamIndex(nullptr, 0), nullptr);
}

/*
 * @tc.name: AssetFontManagerTest008
 * @tc.desc: test for onMakeFromStreamArgs
 * @tc.type: FUNC
 */
HWTEST_F(AssetFontManagerTest, AssetFontManagerTest008, TestSize.Level1)
{
    EXPECT_EQ(assetFontManager_ != nullptr, true);
    SkFontArguments fontArguments;
    EXPECT_EQ(assetFontManager_->onMakeFromStreamArgs(nullptr, fontArguments), nullptr);
}

/*
 * @tc.name: AssetFontManagerTest009
 * @tc.desc: test for onMakeFromFile
 * @tc.type: FUNC
 */
HWTEST_F(AssetFontManagerTest, AssetFontManagerTest009, TestSize.Level1)
{
    EXPECT_EQ(assetFontManager_ != nullptr, true);
    EXPECT_EQ(assetFontManager_->onMakeFromFile(nullptr, 0), nullptr);
}

/*
 * @tc.name: AssetFontManagerTest010
 * @tc.desc: test for onLegacyMakeTypeface
 * @tc.type: FUNC
 */
HWTEST_F(AssetFontManagerTest, AssetFontManagerTest010, TestSize.Level1)
{
    EXPECT_EQ(assetFontManager_ != nullptr, true);
    SkFontStyle fontStyle;
    EXPECT_EQ(assetFontManager_->onLegacyMakeTypeface(nullptr, fontStyle), nullptr);
}

/*
 * @tc.name: AssetFontManagerTest011
 * @tc.desc: test for DynamicFontManager
 * @tc.type: FUNC
 */
HWTEST_F(AssetFontManagerTest, AssetFontManagerTest011, TestSize.Level1)
{
    DynamicFontManager dynamicFontManager;
    dynamicFontManager.font_provider();
    EXPECT_EQ(dynamicFontManager.fontProvider_ != nullptr, true);
}

/*
 * @tc.name: AssetFontManagerTest012
 * @tc.desc: test for TestFontManager
 * @tc.type: FUNC
 */
HWTEST_F(AssetFontManagerTest, AssetFontManagerTest012, TestSize.Level1)
{
    std::unique_ptr<FontAssetProvider> fontProvider = std::make_unique<TypefaceFontAssetProvider>();
    EXPECT_EQ(fontProvider != nullptr, true);
    std::string familyName("test");
    std::vector<std::string> familyNames;
    familyNames.emplace_back(familyName);
    std::shared_ptr<TestFontManager> testFontManager =
        std::make_shared<TestFontManager>(std::move(fontProvider), familyNames);
    EXPECT_EQ(testFontManager != nullptr, true);
    EXPECT_EQ(testFontManager->onMatchFamily(familyName.c_str()), nullptr);
}
} // namespace txt