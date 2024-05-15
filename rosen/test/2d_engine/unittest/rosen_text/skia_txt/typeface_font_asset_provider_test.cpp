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
    static void SetUpTestCase();
    static void TearDownTestCase();
    static inline std::shared_ptr<TypefaceFontAssetProvider> typefaceFontAssetProvider_ = nullptr;
};

void TypefaceFontAssetProviderTest::SetUpTestCase()
{
    typefaceFontAssetProvider_ = std::make_shared<TypefaceFontAssetProvider>();
    if (!typefaceFontAssetProvider_) {
        std::cout << "TypefaceFontAssetProviderTest::SetUpTestCase error typefaceFontAssetProvider_ is nullptr"
            << std::endl;
    }
}

void TypefaceFontAssetProviderTest::TearDownTestCase()
{
}

/*
 * @tc.name: TypefaceFontAssetProviderTest001
 * @tc.desc: test for RegisterTypeface
 * @tc.type: FUNC
 */
HWTEST_F(TypefaceFontAssetProviderTest, TypefaceFontAssetProviderTest001, TestSize.Level1)
{
    EXPECT_EQ(typefaceFontAssetProvider_ != nullptr, true);
    sk_sp<SkTypeface> skTypeface = SkTypeface::MakeDefault();
    EXPECT_EQ(skTypeface != nullptr, true);
    typefaceFontAssetProvider_->RegisterTypeface(skTypeface);
    EXPECT_EQ(typefaceFontAssetProvider_->GetFamilyCount() > 0, true);
}

/*
 * @tc.name: TypefaceFontAssetProviderTest002
 * @tc.desc: test for GetFamilyName
 * @tc.type: FUNC
 */
HWTEST_F(TypefaceFontAssetProviderTest, TypefaceFontAssetProviderTest002, TestSize.Level1)
{
    EXPECT_EQ(typefaceFontAssetProvider_ != nullptr, true);
    EXPECT_EQ(typefaceFontAssetProvider_->GetFamilyName(0).empty(), false);
}

/*
 * @tc.name: TypefaceFontAssetProviderTest003
 * @tc.desc: test for MatchFamily
 * @tc.type: FUNC
 */
HWTEST_F(TypefaceFontAssetProviderTest, TypefaceFontAssetProviderTest003, TestSize.Level1)
{
    EXPECT_EQ(typefaceFontAssetProvider_ != nullptr, true);
    std::string familyName = typefaceFontAssetProvider_->GetFamilyName(0);
    EXPECT_EQ(typefaceFontAssetProvider_->MatchFamily(familyName) != nullptr, true);
}

/*
 * @tc.name: TypefaceFontAssetProviderTest004
 * @tc.desc: test for TypefaceFontStyleSet count
 * @tc.type: FUNC
 */
HWTEST_F(TypefaceFontAssetProviderTest, TypefaceFontAssetProviderTest004, TestSize.Level1)
{
    EXPECT_EQ(typefaceFontAssetProvider_ != nullptr, true);
    std::string familyName = typefaceFontAssetProvider_->GetFamilyName(0);
    SkFontStyleSet* skFontStyleSet = typefaceFontAssetProvider_->MatchFamily(familyName);
    EXPECT_EQ(skFontStyleSet != nullptr, true);
    TypefaceFontStyleSet* typefaceFontStyleSet = static_cast<TypefaceFontStyleSet*>(skFontStyleSet);
    EXPECT_EQ(typefaceFontStyleSet != nullptr, true);
    EXPECT_EQ(typefaceFontStyleSet->count() > 0, true);
}

/*
 * @tc.name: TypefaceFontAssetProviderTest005
 * @tc.desc: test for TypefaceFontStyleSet getStyle
 * @tc.type: FUNC
 */
HWTEST_F(TypefaceFontAssetProviderTest, TypefaceFontAssetProviderTest005, TestSize.Level1)
{
    EXPECT_EQ(typefaceFontAssetProvider_ != nullptr, true);
    std::string familyName = typefaceFontAssetProvider_->GetFamilyName(0);
    SkFontStyleSet* skFontStyleSet = typefaceFontAssetProvider_->MatchFamily(familyName);
    EXPECT_EQ(skFontStyleSet != nullptr, true);
    TypefaceFontStyleSet* typefaceFontStyleSet = static_cast<TypefaceFontStyleSet*>(skFontStyleSet);
    EXPECT_EQ(typefaceFontStyleSet != nullptr, true);
    SkFontStyle style;
    SkString name;
    typefaceFontStyleSet->getStyle(0, &style, &name);
    EXPECT_EQ(name.isEmpty(), true);
}

/*
 * @tc.name: TypefaceFontAssetProviderTest006
 * @tc.desc: test for TypefaceFontStyleSet matchStyle
 * @tc.type: FUNC
 */
HWTEST_F(TypefaceFontAssetProviderTest, TypefaceFontAssetProviderTest006, TestSize.Level1)
{
    EXPECT_EQ(typefaceFontAssetProvider_ != nullptr, true);
    std::string familyName = typefaceFontAssetProvider_->GetFamilyName(0);
    SkFontStyleSet* skFontStyleSet = typefaceFontAssetProvider_->MatchFamily(familyName);
    EXPECT_EQ(skFontStyleSet != nullptr, true);
    TypefaceFontStyleSet* typefaceFontStyleSet = static_cast<TypefaceFontStyleSet*>(skFontStyleSet);
    EXPECT_EQ(typefaceFontStyleSet != nullptr, true);
    SkFontStyle pattern;
    EXPECT_EQ(typefaceFontStyleSet->matchStyle(pattern) != nullptr, true);
}

/*
 * @tc.name: TypefaceFontAssetProviderTest007
 * @tc.desc: test for TypefaceFontStyleSet createTypeface
 * @tc.type: FUNC
 */
HWTEST_F(TypefaceFontAssetProviderTest, TypefaceFontAssetProviderTest007, TestSize.Level1)
{
    EXPECT_EQ(typefaceFontAssetProvider_ != nullptr, true);
    std::string familyName = typefaceFontAssetProvider_->GetFamilyName(0);
    SkFontStyleSet* skFontStyleSet = typefaceFontAssetProvider_->MatchFamily(familyName);
    EXPECT_EQ(skFontStyleSet != nullptr, true);
    TypefaceFontStyleSet* typefaceFontStyleSet = static_cast<TypefaceFontStyleSet*>(skFontStyleSet);
    EXPECT_EQ(typefaceFontStyleSet != nullptr, true);
    EXPECT_EQ(typefaceFontStyleSet->createTypeface(0) != nullptr, true);
    EXPECT_EQ(typefaceFontStyleSet->count() > 0, true);
}

/*
 * @tc.name: TypefaceFontAssetProviderTest008
 * @tc.desc: test for TypefaceFontStyleSet unregisterTypefaces
 * @tc.type: FUNC
 */
HWTEST_F(TypefaceFontAssetProviderTest, TypefaceFontAssetProviderTest008, TestSize.Level1)
{
    EXPECT_EQ(typefaceFontAssetProvider_ != nullptr, true);
    std::string familyName = typefaceFontAssetProvider_->GetFamilyName(0);
    SkFontStyleSet* skFontStyleSet = typefaceFontAssetProvider_->MatchFamily(familyName);
    EXPECT_EQ(skFontStyleSet != nullptr, true);
    TypefaceFontStyleSet* typefaceFontStyleSet = static_cast<TypefaceFontStyleSet*>(skFontStyleSet);
    EXPECT_EQ(typefaceFontStyleSet != nullptr, true);
    typefaceFontStyleSet->unregisterTypefaces();
    EXPECT_EQ(typefaceFontStyleSet->count(), 0);
}
} // namespace txt