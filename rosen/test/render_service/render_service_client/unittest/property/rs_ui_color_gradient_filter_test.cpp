/*
* Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "ui_effect/property/include/rs_ui_color_gradient_filter.h"
#include "ui_effect/mask/include/ripple_mask_para.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSUIColorGradientFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUIColorGradientFilterTest::SetUpTestCase() {}
void RSUIColorGradientFilterTest::TearDownTestCase() {}
void RSUIColorGradientFilterTest::SetUp() {}
void RSUIColorGradientFilterTest::TearDown() {}

/**
 * @tc.name: Equal001
 * @tc.desc: Test Equal
 * @tc.type:FUNC
 */
HWTEST_F(RSUIColorGradientFilterTest, Equal001, TestSize.Level1)
{
    auto filterPara1 = std::make_shared<RSUIColorGradientFilterPara>();
    auto filterPara2 = std::make_shared<RSUIColorGradientFilterPara>();
    auto filterParaBase = static_cast<std::shared_ptr<RSUIFilterParaBase>>(filterPara2);
    
    EXPECT_TRUE(filterPara1->Equals(filterParaBase));
    EXPECT_FALSE(filterPara1->Equals(nullptr));
}

/**
 * @tc.name: Dump001
 * @tc.desc: Test Dump
 * @tc.type:FUNC
 */
HWTEST_F(RSUIColorGradientFilterTest, Dump001, TestSize.Level1)
{
    auto filterPara = std::make_shared<RSUIColorGradientFilterPara>();

    std::string temp = "RSUIColorGradientFilterPara: [";
    std::string out;
    filterPara->Dump(out);
    EXPECT_EQ(temp, out);

    std::string temp1 = "RSUIColorGradientFilterPara: [[color: 1.000000 1.000000 1.000000 1.000000]";
    std::vector<float> colors = { 1.0, 1.0, 1.0, 1.0 }; // 1.0f 1.0f 1.0f 1.0f is color rgba
    filterPara->SetColors(colors);
    std::string out1;
    filterPara->Dump(out1);
    EXPECT_EQ(temp1, out1);
}

/**
 * @tc.name: SetProperty001
 * @tc.desc: Test SetProperty
 * @tc.type:FUNC
 */
HWTEST_F(RSUIColorGradientFilterTest, SetProperty001, TestSize.Level1)
{
    auto filterPara1 = std::make_shared<RSUIColorGradientFilterPara>();
    auto filterPara2 = std::make_shared<RSUIColorGradientFilterPara>();
    auto filterParaBase = static_cast<std::shared_ptr<RSUIFilterParaBase>>(filterPara2);

    filterPara1->SetProperty(nullptr);
    filterPara1->SetProperty(filterParaBase);

    // 1.0, 0.0, 0.0, 1.0 is the color rgba params
    std::vector<float> colors = { 1.0, 0.0, 0.0, 1.0 };
    std::vector<float> positions = { 1.0, 1.0 }; // 1.0, 1.0 is poition xy params
    std::vector<float> strengths = { 0.5 }; // 0.5 is strength params
    filterPara2->SetColors(colors);
    filterPara2->SetPositions(positions);
    filterPara2->SetStrengths(strengths);
    auto mask = filterPara2->CreateMask(RSUIFilterType::NONE);
    filterPara2->SetMask(mask);
    EXPECT_EQ(mask, nullptr);
    mask = filterPara2->CreateMask(RSUIFilterType::RIPPLE_MASK);
    filterPara2->SetMask(mask);
  
    auto filterParaBase2 = static_cast<std::shared_ptr<RSUIFilterParaBase>>(filterPara2);
    filterPara1->SetProperty(filterParaBase2);

    auto out1 = filterPara1->GetLeafProperties();
    auto out2 = filterPara2->GetLeafProperties();
    EXPECT_EQ(out1.size(), out2.size());
}

/**
 * @tc.name: SetColorGradient001
 * @tc.desc: Test SetColorGradient
 * @tc.type:FUNC
 */
HWTEST_F(RSUIColorGradientFilterTest, SetColorGradient001, TestSize.Level1)
{
    auto filterPara1 = std::make_shared<RSUIColorGradientFilterPara>();
    
    // 1.0, 0.0, 0.0, 1.0 is the color rgba params
    std::vector<float> colors = { 1.0, 0.0, 0.0, 1.0 };
    std::vector<float> positions = { 1.0, 1.0 }; // 1.0, 1.0 is poition xy params
    std::vector<float> strengths = { 0.5 }; // 0.5 is strength params
    auto mask = std::make_shared<MaskPara>();

    auto colorGradientPara =  std::make_shared<ColorGradientPara>();
    colorGradientPara->SetColors(colors);
    colorGradientPara->SetPositions(positions);
    colorGradientPara->SetStrengths(strengths);
    colorGradientPara->SetMask(mask);
    filterPara1->SetColorGradient(nullptr);
    auto rsRenderFilterParaBase = filterPara1->CreateRSRenderFilter();
    EXPECT_EQ(rsRenderFilterParaBase, nullptr);

    filterPara1->SetColorGradient(colorGradientPara);
    rsRenderFilterParaBase = filterPara1->CreateRSRenderFilter();
    EXPECT_NE(rsRenderFilterParaBase, nullptr);

    colorGradientPara->SetMask(mask);
    rsRenderFilterParaBase = filterPara1->CreateRSRenderFilter();
    EXPECT_NE(rsRenderFilterParaBase, nullptr);

    auto mask2 = std::make_shared<RippleMaskPara>();
    colorGradientPara->SetMask(mask2);
    rsRenderFilterParaBase = filterPara1->CreateRSRenderFilter();
    EXPECT_NE(rsRenderFilterParaBase, nullptr);
}

/**
 * @tc.name: CreateRSRenderFilter001
 * @tc.desc: Test CreateRSRenderFilter
 * @tc.type:FUNC
 */
HWTEST_F(RSUIColorGradientFilterTest, CreateRSRenderFilter001, TestSize.Level1)
{
    auto filterPara1 = std::make_shared<RSUIColorGradientFilterPara>();

    // 1.0, 0.0, 0.0, 1.0 is the color rgba params
    std::vector<float> colors = { 1.0, 0.0, 0.0, 1.0 };
    std::vector<float> positions = { 1.0, 1.0 }; // 1.0, 1.0 is poition xy params
    std::vector<float> strengths = { 0.5 }; // 0.5 is strength params
    auto mask = filterPara1->CreateMask(RSUIFilterType::RIPPLE_MASK);

    auto rsRenderFilterParaBase = filterPara1->CreateRSRenderFilter();
    EXPECT_EQ(rsRenderFilterParaBase, nullptr);

    filterPara1->SetColors(colors);
    rsRenderFilterParaBase = filterPara1->CreateRSRenderFilter();
    EXPECT_EQ(rsRenderFilterParaBase, nullptr);

    filterPara1->SetPositions(positions);
    rsRenderFilterParaBase = filterPara1->CreateRSRenderFilter();
    EXPECT_EQ(rsRenderFilterParaBase, nullptr);

    filterPara1->SetStrengths(strengths);
    rsRenderFilterParaBase = filterPara1->CreateRSRenderFilter();
    EXPECT_NE(rsRenderFilterParaBase, nullptr);

    filterPara1->SetMask(mask);
    rsRenderFilterParaBase = filterPara1->CreateRSRenderFilter();
    EXPECT_NE(rsRenderFilterParaBase, nullptr);
}
} // namespace OHOS::Rosen