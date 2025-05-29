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
#include "ui_effect/filter/include/filter_displacement_distort_para.h"
#include "ui_effect/property/include/rs_ui_displacement_distort_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSUIDisplacementDistortFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUIDisplacementDistortFilterTest::SetUpTestCase() {}
void RSUIDisplacementDistortFilterTest::TearDownTestCase() {}
void RSUIDisplacementDistortFilterTest::SetUp() {}
void RSUIDisplacementDistortFilterTest::TearDown() {}

/**
 * @tc.name: Equal001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDisplacementDistortFilterTest, Equal001, TestSize.Level1)
{
    auto rsUIDispDistortFilterPara1 = std::make_shared<RSUIDispDistortFilterPara>();
    EXPECT_FALSE(rsUIDispDistortFilterPara1->Equals(nullptr));

    auto rsUIDispDistortFilterPara2 = std::make_shared<RSUIDispDistortFilterPara>();
    rsUIDispDistortFilterPara2->type_ = RSUIFilterType::BLUR;
    auto rsUIFilterParaBase = static_cast<std::shared_ptr<RSUIFilterParaBase>>(rsUIDispDistortFilterPara2);
    EXPECT_FALSE(rsUIDispDistortFilterPara1->Equals(rsUIFilterParaBase));

    auto rsUIDispDistortFilterPara3 = std::make_shared<RSUIDispDistortFilterPara>();
    rsUIFilterParaBase = static_cast<std::shared_ptr<RSUIFilterParaBase>>(rsUIDispDistortFilterPara3);
    EXPECT_TRUE(rsUIDispDistortFilterPara1->Equals(rsUIFilterParaBase));
}

/**
 * @tc.name: Dump001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDisplacementDistortFilterTest, Dump001, TestSize.Level1)
{
    std::string temp = "RSUIDispDistortFilterPara: [[factor: 0.500000 0.500000]";

    auto displacementDistortPara = std::make_shared<DisplacementDistortPara>();

    auto maskPara = std::make_shared<MaskPara>();
    maskPara->type_ = MaskPara::Type::RIPPLE_MASK;
    displacementDistortPara->SetMask(maskPara);
    Vector2f factor = Vector2f(0.5f, 0.5f);
    displacementDistortPara->SetFactor(factor);

    auto rsUIDispDistortFilterPara = std::make_shared<RSUIDispDistortFilterPara>();
    rsUIDispDistortFilterPara->SetDisplacementDistort(displacementDistortPara);

    std::string out;
    rsUIDispDistortFilterPara->Dump(out);
    EXPECT_EQ(temp, out);
}

/**
 * @tc.name: SetProperty001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDisplacementDistortFilterTest, SetProperty001, TestSize.Level1)
{
    auto displacementDistortPara1 = std::make_shared<DisplacementDistortPara>();
    auto maskPara1 = std::make_shared<MaskPara>();
    displacementDistortPara1->SetMask(maskPara1);
    Vector2f factor1 = Vector2f(0.5f, 0.5f);
    displacementDistortPara1->SetFactor(factor1);

    auto rsUIDispDistortFilterPara1 = std::make_shared<RSUIDispDistortFilterPara>();
    rsUIDispDistortFilterPara1->SetDisplacementDistort(displacementDistortPara1);

    auto displacementDistortPara2 = std::make_shared<DisplacementDistortPara>();
    auto maskPara2 = std::make_shared<MaskPara>();
    displacementDistortPara2->SetMask(maskPara2);
    Vector2f factor2 = Vector2f(0.5f, 0.5f);
    displacementDistortPara2->SetFactor(factor2);

    auto rsUIDispDistortFilterPara2 = std::make_shared<RSUIDispDistortFilterPara>();
    rsUIDispDistortFilterPara2->SetDisplacementDistort(displacementDistortPara2);

    rsUIDispDistortFilterPara1->SetProperty(nullptr);

    auto rsUIDispDistortFilterPara3 = std::make_shared<RSUIDispDistortFilterPara>();
    rsUIDispDistortFilterPara3->type_ = RSUIFilterType::BLUR;
    auto rsUIFilterParaBase = static_cast<std::shared_ptr<RSUIFilterParaBase>>(rsUIDispDistortFilterPara3);
    rsUIDispDistortFilterPara1->SetProperty(rsUIFilterParaBase);

    rsUIFilterParaBase = static_cast<std::shared_ptr<RSUIFilterParaBase>>(rsUIDispDistortFilterPara2);
    rsUIDispDistortFilterPara1->SetProperty(rsUIFilterParaBase);

    auto iter = rsUIDispDistortFilterPara1->properties_.find(RSUIFilterType::DISPLACEMENT_DISTORT_FACTOR);
    ASSERT_NE(iter, rsUIDispDistortFilterPara1->properties_.end());

    auto property = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(iter->second);
    ASSERT_NE(property, nullptr);
    EXPECT_EQ(property->Get(), factor2);
}

/**
 * @tc.name: SetDisplacementDistort001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDisplacementDistortFilterTest, SetDisplacementDistort001, TestSize.Level1)
{
    auto displacementDistortPara = std::make_shared<DisplacementDistortPara>();

    auto maskPara = std::make_shared<MaskPara>();
    displacementDistortPara->SetMask(maskPara);
    Vector2f factor = Vector2f(0.5f, 0.5f);
    displacementDistortPara->SetFactor(factor);

    auto rsUIDispDistortFilterPara = std::make_shared<RSUIDispDistortFilterPara>();
    rsUIDispDistortFilterPara->SetDisplacementDistort(displacementDistortPara);

    auto iter = rsUIDispDistortFilterPara->properties_.find(RSUIFilterType::DISPLACEMENT_DISTORT_FACTOR);
    ASSERT_NE(iter, rsUIDispDistortFilterPara->properties_.end());

    auto property = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(iter->second);
    ASSERT_NE(property, nullptr);
    EXPECT_EQ(property->Get(), factor);
}

/**
 * @tc.name: SetFactor001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDisplacementDistortFilterTest, SetFactor001, TestSize.Level1)
{
    auto rsUIDispDistortFilterPara = std::make_shared<RSUIDispDistortFilterPara>();

    Vector2f factor = Vector2f(0.5f, 0.5f);
    rsUIDispDistortFilterPara->SetFactor(factor);
    auto iter = rsUIDispDistortFilterPara->properties_.find(RSUIFilterType::DISPLACEMENT_DISTORT_FACTOR);
    ASSERT_NE(iter, rsUIDispDistortFilterPara->properties_.end());

    auto property = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(iter->second);
    ASSERT_NE(property, nullptr);
    EXPECT_EQ(property->Get(), factor);
}

/**
 * @tc.name: SetMask001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDisplacementDistortFilterTest, SetMask001, TestSize.Level1)
{
    auto rsUIDispDistortFilterPara = std::make_shared<RSUIDispDistortFilterPara>();

    auto mask = std::make_shared<RSUIMaskPara>(RSUIFilterType::DISPLACEMENT_DISTORT);
    rsUIDispDistortFilterPara->SetMask(mask);

    EXPECT_EQ(rsUIDispDistortFilterPara->properties_[RSUIFilterType::DISPLACEMENT_DISTORT], mask);
}

/**
 * @tc.name: CreateRSRenderFilter001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDisplacementDistortFilterTest, CreateRSRenderFilter001, TestSize.Level1)
{
    auto displacementDistortPara = std::make_shared<DisplacementDistortPara>();

    auto maskPara = std::make_shared<MaskPara>();
    maskPara->type_ = MaskPara::Type::RIPPLE_MASK;
    displacementDistortPara->SetMask(maskPara);
    Vector2f factor = Vector2f(0.5f, 0.5f);
    displacementDistortPara->SetFactor(factor);

    auto rsUIDispDistortFilterPara = std::make_shared<RSUIDispDistortFilterPara>();
    rsUIDispDistortFilterPara->SetDisplacementDistort(displacementDistortPara);

    auto rsRenderFilterParaBase = rsUIDispDistortFilterPara->CreateRSRenderFilter();
    EXPECT_NE(rsRenderFilterParaBase, nullptr);
}

/**
 * @tc.name: GetLeafProperties001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDisplacementDistortFilterTest, GetLeafProperties001, TestSize.Level1)
{
    auto displacementDistortPara = std::make_shared<DisplacementDistortPara>();

    auto maskPara = std::make_shared<MaskPara>();
    maskPara->type_ = MaskPara::Type::RIPPLE_MASK;
    displacementDistortPara->SetMask(maskPara);
    Vector2f factor = Vector2f(0.5f, 0.5f);
    displacementDistortPara->SetFactor(factor);

    auto rsUIDispDistortFilterPara = std::make_shared<RSUIDispDistortFilterPara>();
    rsUIDispDistortFilterPara->SetDisplacementDistort(displacementDistortPara);

    auto rsPropertyBaseVector = rsUIDispDistortFilterPara->GetLeafProperties();
    EXPECT_NE(rsPropertyBaseVector.size(), 0);
}
} // namespace OHOS::Rosen