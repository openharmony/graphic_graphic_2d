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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "font_collection.h"
#include "texgine/font_providers.h"
#include "texgine/system_font_provider.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class MockFontProvider : public IFontProvider {
public:
    MOCK_METHOD(std::shared_ptr<VariantFontStyleSet>, MatchFamily, (const std::string &familyName), (noexcept(true)));
};

struct MockVars {
    std::shared_ptr<VariantFontStyleSet> systemFontProviderMatchFamilyRetval;
    std::vector<std::shared_ptr<VariantFontStyleSet>> catchedFontStyleSets;
} g_fpMockvars;

void InitFpMockVars(struct MockVars &&vars)
{
    g_fpMockvars = std::move(vars);
}

FontCollection::FontCollection(std::vector<std::shared_ptr<VariantFontStyleSet>> &&fontStyleSets)
{
    g_fpMockvars.catchedFontStyleSets = std::move(fontStyleSets);
}

std::shared_ptr<VariantFontStyleSet> SystemFontProvider::MatchFamily(const std::string &familyName) noexcept(true)
{
    return g_fpMockvars.systemFontProviderMatchFamilyRetval;
}

class FontProvidersTest : public testing::Test {
public:
    std::shared_ptr<FontProviders> fontProviders_ = FontProviders::Create();
    std::shared_ptr<MockFontProvider> msfp1_ = std::make_shared<MockFontProvider>();
    std::shared_ptr<MockFontProvider> msfp2_ = std::make_shared<MockFontProvider>();
    std::shared_ptr<VariantFontStyleSet> fontStyleSet1_ = std::make_shared<VariantFontStyleSet>(nullptr);
    std::shared_ptr<VariantFontStyleSet> fontStyleSet2_ = std::make_shared<VariantFontStyleSet>(nullptr);
};

/**
 * @tc.name: CreateAndSystemOnly
 * @tc.desc: Verify the CreateAndSystemOnly
 * @tc.type:FUNC
 */
HWTEST_F(FontProvidersTest, CreateAndSystemOnly, TestSize.Level1)
{
    std::shared_ptr<VariantFontStyleSet> fss1 = std::make_shared<VariantFontStyleSet>(nullptr);

    // Create
    InitFpMockVars({});
    auto fp1 = FontProviders::Create();
    ASSERT_NE(fp1, nullptr);
    auto fc1 = fp1->GenerateFontCollection({"Create"});
    ASSERT_NE(fc1, nullptr);
    ASSERT_EQ(g_fpMockvars.catchedFontStyleSets.size(), 0u);

    // SystemFontOnly
    InitFpMockVars({.systemFontProviderMatchFamilyRetval = fss1});
    auto fp2 = FontProviders::SystemFontOnly();
    ASSERT_NE(fp2, nullptr);
    auto fc2 = fp2->GenerateFontCollection({"SystemFontOnly"});
    ASSERT_NE(fc2, nullptr);
    ASSERT_EQ(g_fpMockvars.catchedFontStyleSets.size(), 1u);
    ASSERT_EQ(g_fpMockvars.catchedFontStyleSets[0], fss1);
}

/**
 * @tc.name: AppendFontProvider1
 * @tc.desc: Verify the AppendFontProvider
 * @tc.type:FUNC
 */
HWTEST_F(FontProvidersTest, AppendFontProvider1, TestSize.Level1)
{
    // AppendFontProvider nullptr
    InitFpMockVars({});
    auto fp1 = FontProviders::Create();
    ASSERT_NE(fp1, nullptr);
    fp1->AppendFontProvider(nullptr);
    auto fc1 = fp1->GenerateFontCollection({"AppendFontProvider1"});
    ASSERT_NE(fc1, nullptr);
    ASSERT_EQ(g_fpMockvars.catchedFontStyleSets.size(), 0u);
}

/**
 * @tc.name: AppendFontProvider2
 * @tc.desc: Verify the AppendFontProvider
 * @tc.type:FUNC
 */
HWTEST_F(FontProvidersTest, AppendFontProvider2, TestSize.Level1)
{
    std::shared_ptr<VariantFontStyleSet> fss1 = std::make_shared<VariantFontStyleSet>(nullptr);

    // AppendFontProvider mock1 once
    InitFpMockVars({});
    auto fp2 = FontProviders::Create();
    ASSERT_NE(fp2, nullptr);
    auto mfp1 = std::make_shared<MockFontProvider>();
    EXPECT_CALL(*mfp1, MatchFamily("AppendFontProvider2")).Times(1).WillOnce(testing::Return(fss1));
    fp2->AppendFontProvider(mfp1);
    auto fc2 = fp2->GenerateFontCollection({"AppendFontProvider2"});
    ASSERT_NE(fc2, nullptr);
    ASSERT_EQ(g_fpMockvars.catchedFontStyleSets.size(), 1u);
    ASSERT_EQ(g_fpMockvars.catchedFontStyleSets[0], fss1);
}

/**
 * @tc.name: AppendFontProvider3
 * @tc.desc: Verify the AppendFontProvider
 * @tc.type:FUNC
 */
HWTEST_F(FontProvidersTest, AppendFontProvider3, TestSize.Level1)
{
    std::shared_ptr<VariantFontStyleSet> fss1 = std::make_shared<VariantFontStyleSet>(nullptr);
    std::shared_ptr<VariantFontStyleSet> fss2 = std::make_shared<VariantFontStyleSet>(nullptr);

    // AppendFontProvider mock1 mock2
    InitFpMockVars({});
    auto fp4 = FontProviders::Create();
    ASSERT_NE(fp4, nullptr);
    auto mfp3 = std::make_shared<MockFontProvider>();
    EXPECT_CALL(*mfp3, MatchFamily)
        .Times(2)
        .WillOnce(testing::Return(fss1))
        .WillOnce(testing::Return(nullptr)); // for mfp4
    auto mfp4 = std::make_shared<MockFontProvider>();
    EXPECT_CALL(*mfp4, MatchFamily("AppendFontProvider5"))
        .Times(1)
        .WillOnce(testing::Return(fss2));
    fp4->AppendFontProvider(mfp3);
    fp4->AppendFontProvider(mfp4);
    auto fc4 = fp4->GenerateFontCollection({"AppendFontProvider4", "AppendFontProvider5"});
    ASSERT_NE(fc4, nullptr);
    ASSERT_EQ(g_fpMockvars.catchedFontStyleSets.size(), 2u);
    ASSERT_EQ(g_fpMockvars.catchedFontStyleSets[0], fss1);
    ASSERT_EQ(g_fpMockvars.catchedFontStyleSets[1], fss2);
}

/**
 * @tc.name: GenerateFontCollection1
 * @tc.desc: Verify the GenerateFontCollection
 * @tc.type:FUNC
 */
HWTEST_F(FontProvidersTest, GenerateFontCollection1, TestSize.Level1)
{
    // GenerateFontCollection set=nullptr
    InitFpMockVars({});
    auto fp1 = FontProviders::Create();
    ASSERT_NE(fp1, nullptr);
    auto mfp1 = std::make_shared<MockFontProvider>();
    EXPECT_CALL(*mfp1, MatchFamily("GenerateFontCollection1")).Times(1).WillOnce(testing::Return(nullptr));
    fp1->AppendFontProvider(mfp1);
    auto fc1 = fp1->GenerateFontCollection({"GenerateFontCollection1"});
    ASSERT_NE(fc1, nullptr);
    ASSERT_EQ(g_fpMockvars.catchedFontStyleSets.size(), 0u);
}

/**
 * @tc.name: GenerateFontCollection2
 * @tc.desc: Verify the GenerateFontCollection
 * @tc.type:FUNC
 */
HWTEST_F(FontProvidersTest, GenerateFontCollection2, TestSize.Level1)
{
    std::shared_ptr<VariantFontStyleSet> fss1 = std::make_shared<VariantFontStyleSet>(nullptr);

    // GenerateFontCollection use cache
    InitFpMockVars({});
    auto fp2 = FontProviders::Create();
    ASSERT_NE(fp2, nullptr);
    auto mfp2 = std::make_shared<MockFontProvider>();
    EXPECT_CALL(*mfp2, MatchFamily("GenerateFontCollection2")).Times(1).WillOnce(testing::Return(fss1));
    fp2->AppendFontProvider(mfp2);
    auto fc21 = fp2->GenerateFontCollection({"GenerateFontCollection2"});
    ASSERT_NE(fc21, nullptr);
    ASSERT_EQ(g_fpMockvars.catchedFontStyleSets.size(), 1u);
    ASSERT_EQ(g_fpMockvars.catchedFontStyleSets[0], fss1);
    auto fc22 = fp2->GenerateFontCollection({"GenerateFontCollection2"});
    ASSERT_EQ(g_fpMockvars.catchedFontStyleSets.size(), 1u);
    ASSERT_EQ(g_fpMockvars.catchedFontStyleSets[0], fss1);
}

/**
 * @tc.name: GenerateFontCollection3
 * @tc.desc: Verify the GenerateFontCollection
 * @tc.type:FUNC
 */
HWTEST_F(FontProvidersTest, GenerateFontCollection3, TestSize.Level1)
{
    std::shared_ptr<VariantFontStyleSet> fss2 = std::make_shared<VariantFontStyleSet>(nullptr);

    // GenerateFontCollection first failed, second success
    InitFpMockVars({});
    auto fp3 = FontProviders::Create();
    ASSERT_NE(fp3, nullptr);
    auto mfp3 = std::make_shared<MockFontProvider>();
    EXPECT_CALL(*mfp3, MatchFamily("GenerateFontCollection3")).Times(1).WillOnce(testing::Return(nullptr));
    fp3->AppendFontProvider(mfp3);
    auto mfp4 = std::make_shared<MockFontProvider>();
    EXPECT_CALL(*mfp4, MatchFamily("GenerateFontCollection3")).Times(1).WillOnce(testing::Return(fss2));
    fp3->AppendFontProvider(mfp4);
    auto fc3 = fp3->GenerateFontCollection({"GenerateFontCollection3"});
    ASSERT_NE(fc3, nullptr);
    ASSERT_EQ(g_fpMockvars.catchedFontStyleSets.size(), 1u);
    ASSERT_EQ(g_fpMockvars.catchedFontStyleSets[0], fss2);
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
