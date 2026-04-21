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
#include "include/core/SkString.h"
#include "include/core/SkTypeface.h"
#include "typeface_font_asset_provider.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::SPText;

namespace txt {
class TypefaceFontAssetProviderTest : public testing::Test {
public:
    void SetUp() override;
    void TearDown() override;
    std::shared_ptr<TypefaceFontAssetProvider> typefaceFontAssetProvider_ = nullptr;
};

void TypefaceFontAssetProviderTest::SetUp()
{
    typefaceFontAssetProvider_ = std::make_shared<TypefaceFontAssetProvider>();
    ASSERT_NE(typefaceFontAssetProvider_, nullptr);
}

void TypefaceFontAssetProviderTest::TearDown()
{
    typefaceFontAssetProvider_.reset();
}

/*
 * @tc.name: TypefaceFontAssetProviderTest001
 * @tc.desc: test for RegisterTypeface and getFamily
 * @tc.type: FUNC
 */
HWTEST_F(TypefaceFontAssetProviderTest, TypefaceFontAssetProviderTest001, TestSize.Level0)
{
    sk_sp<SkTypeface> skTypeface = SkTypeface::MakeDefault();
    ASSERT_NE(skTypeface, nullptr);
    typefaceFontAssetProvider_->RegisterTypeface(skTypeface);
    typefaceFontAssetProvider_->RegisterTypeface(nullptr);
    typefaceFontAssetProvider_->RegisterTypeface(nullptr, "");
    EXPECT_EQ(typefaceFontAssetProvider_->GetFamilyCount(), 1);
    std::string result = { 0x48, 0x61, 0x72, 0x6d, 0x6f, 0x6e, 0x79, 0x4f, 0x53, 0x2d, 0x53, 0x61, 0x6e, 0x73 };
    EXPECT_EQ(typefaceFontAssetProvider_->GetFamilyName(0), result);
    EXPECT_EQ(typefaceFontAssetProvider_->GetFamilyName(-1), "");
    int index = typefaceFontAssetProvider_->GetFamilyCount() + 1;
    EXPECT_EQ(typefaceFontAssetProvider_->GetFamilyName(index), "");
}

/*
 * @tc.name: TypefaceFontAssetProviderTest004
 * @tc.desc: test for TypefaceFontStyleSet
 * @tc.type: FUNC
 */
HWTEST_F(TypefaceFontAssetProviderTest, TypefaceFontAssetProviderTest004, TestSize.Level0)
{
    sk_sp<SkTypeface> skTypeface = SkTypeface::MakeDefault();
    ASSERT_NE(skTypeface, nullptr);
    typefaceFontAssetProvider_->RegisterTypeface(skTypeface);
    std::string familyName = typefaceFontAssetProvider_->GetFamilyName(0);
    SkFontStyleSet* skFontStyleSet = typefaceFontAssetProvider_->MatchFamily(familyName);
    ASSERT_NE(skFontStyleSet, nullptr);
    TypefaceFontStyleSet* typefaceFontStyleSet = static_cast<TypefaceFontStyleSet*>(skFontStyleSet);
    EXPECT_EQ(typefaceFontStyleSet->count(), 1);
    SkFontStyle style;
    SkString name;
    typefaceFontStyleSet->getStyle(INT_MAX, &style, &name);
    typefaceFontStyleSet->getStyle(0, &style, &name);
    EXPECT_TRUE(name.isEmpty());
    SkFontStyle pattern;
    EXPECT_NE(typefaceFontStyleSet->matchStyle(pattern), nullptr);
    EXPECT_NE(typefaceFontStyleSet->createTypeface(0), nullptr);
    EXPECT_EQ(typefaceFontStyleSet->count(), 1);
    typefaceFontStyleSet->unregisterTypefaces();
    EXPECT_EQ(typefaceFontStyleSet->count(), 0);
    EXPECT_EQ(typefaceFontStyleSet->createTypeface(100), nullptr);
}

/*
 * @tc.name: TypefaceFontAssetProviderTest009
 * @tc.desc: test for RegisterTypefaces with empty familyNameAlias
 * @tc.type: FUNC
 */
HWTEST_F(TypefaceFontAssetProviderTest, TypefaceFontAssetProviderTest009, TestSize.Level0)
{
    auto fontProvider = std::make_unique<TypefaceFontAssetProvider>();
    std::string familyName("test");
    fontProvider->familyNames_.emplace_back(familyName);
    sk_sp<SkTypeface> skTypeface = SkTypeface::MakeDefault();
    ASSERT_NE(skTypeface, nullptr);
    fontProvider->RegisterTypeface(skTypeface, familyName);
    EXPECT_NE(fontProvider->MatchFamily(familyName), nullptr);
    fontProvider->RegisterTypeface(nullptr, familyName);
    EXPECT_EQ(fontProvider->MatchFamily(familyName), nullptr);
}
} // namespace txt