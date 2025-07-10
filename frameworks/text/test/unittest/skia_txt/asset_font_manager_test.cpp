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
    ASSERT_NE(fontProvider, nullptr);
    std::string familyName("test");
    fontProvider->familyNames_.emplace_back(familyName);
    assetFontManager_ = std::make_shared<AssetFontManager>(std::move(fontProvider));
    ASSERT_NE(assetFontManager_, nullptr);
}

void AssetFontManagerTest::TearDownTestCase()
{
}

/*
 * @tc.name: AssetFontManagerTest001
 * @tc.desc: test for onMatchFamily and some nullptr functions
 * @tc.type: FUNC
 */
HWTEST_F(AssetFontManagerTest, AssetFontManagerTest001, TestSize.Level0)
{
    std::string familyName("test");
    EXPECT_EQ(assetFontManager_->onMatchFamily(familyName.c_str()), nullptr);
    SkString skFamilyName(familyName);
    assetFontManager_->onGetFamilyName(0, &skFamilyName);
    EXPECT_EQ(assetFontManager_->onCountFamilies(), 1);
    EXPECT_EQ(assetFontManager_->onCreateStyleSet(0), nullptr);
    SkFontStyle fontStyle;
    EXPECT_EQ(assetFontManager_->onMatchFamilyStyle(familyName.c_str(), fontStyle), nullptr);
    EXPECT_EQ(assetFontManager_->onMatchFamilyStyleCharacter(familyName.c_str(), fontStyle, nullptr, 0, 0), nullptr);
    sk_sp<SkData> data = SkData::MakeEmpty();
    EXPECT_EQ(assetFontManager_->onMakeFromData(data, 0), nullptr);
    EXPECT_EQ(assetFontManager_->onMakeFromStreamIndex(nullptr, 0), nullptr);
    SkFontArguments fontArguments;
    EXPECT_EQ(assetFontManager_->onMakeFromStreamArgs(nullptr, fontArguments), nullptr);
    EXPECT_EQ(assetFontManager_->onMakeFromFile(nullptr, 0), nullptr);
    EXPECT_EQ(assetFontManager_->onLegacyMakeTypeface(nullptr, fontStyle), nullptr);
    DynamicFontManager dynamicFontManager;
    dynamicFontManager.font_provider();
    EXPECT_NE(dynamicFontManager.fontProvider_, nullptr);
}

/*
 * @tc.name: AssetFontManagerTest012
 * @tc.desc: test for TestFontManager
 * @tc.type: FUNC
 */
HWTEST_F(AssetFontManagerTest, AssetFontManagerTest012, TestSize.Level0)
{
    std::unique_ptr<FontAssetProvider> fontProvider = std::make_unique<TypefaceFontAssetProvider>();
    EXPECT_NE(fontProvider, nullptr);
    std::string familyName("test");
    std::vector<std::string> familyNames;
    familyNames.emplace_back(familyName);
    std::shared_ptr<TestFontManager> testFontManager =
        std::make_shared<TestFontManager>(std::move(fontProvider), familyNames);
    EXPECT_NE(testFontManager, nullptr);
    EXPECT_EQ(testFontManager->onMatchFamily(familyName.c_str()), nullptr);
}

/*
 * @tc.name: AssetFontManagerTest013
 * @tc.desc: test for match family and style
 * @tc.type: FUNC
 */
HWTEST_F(AssetFontManagerTest, AssetFontManagerTest013, TestSize.Level0)
{
    std::unique_ptr<TypefaceFontAssetProvider> fontProvider = std::make_unique<TypefaceFontAssetProvider>();
    std::string familyName("test");
    fontProvider->familyNames_.emplace_back(familyName);
    sk_sp<SkTypeface> skTypeface = SkTypeface::MakeDefault();
    fontProvider->RegisterTypeface(skTypeface, familyName);
    std::shared_ptr<AssetFontManager> assetFontManager =
        std::make_shared<AssetFontManager>(std::move(fontProvider));

    SkFontStyle fontStyle;
    EXPECT_NE(assetFontManager->onMatchFamilyStyle(familyName.c_str(), fontStyle), nullptr);
}
} // namespace txt