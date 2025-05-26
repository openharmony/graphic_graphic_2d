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
#include "ui_effect/filter/include/filter_dispersion_para.h"
#include "ui_effect/filter/include/filter_edge_light_para.h"
#include "ui_effect/property/include/rs_ui_dispersion_filter.h"
#include "ui_effect/property/include/rs_ui_edge_light_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSUIDispersionFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUIDispersionFilterTest::SetUpTestCase() {}
void RSUIDispersionFilterTest::TearDownTestCase() {}
void RSUIDispersionFilterTest::SetUp() {}
void RSUIDispersionFilterTest::TearDown() {}

/**
 * @tc.name: EqualTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDispersionFilterTest, EqualTest, TestSize.Level1)
{
    auto rsUIDispersionFilterPara1 = std::make_shared<RSUIEdgeLightFilterPara>();
    auto rsUIDispersionFilterPara2 = std::make_shared<RSUIEdgeLightFilterPara>();
    auto rsUIFilterParaBase = static_cast<std::shared_ptr<RSUIFilterParaBase>>(rsUIDispersionFilterPara2);
    
    EXPECT_TRUE(rsUIDispersionFilterPara1->Equals(rsUIFilterParaBase));
}

/**
 * @tc.name: DumpTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDispersionFilterTest, DumpTest, TestSize.Level1)
{
    auto dispersionPara = std::make_shared<DispersionPara>();
    dispersionPara->SetOpacity(0.5f);
    Vector2f offset = Vector2f(0.5f, 0.5f);
    dispersionPara->SetRedOffset(offset);
    dispersionPara->SetGreenOffset(offset);
    dispersionPara->SetBlueOffset(offset);

    auto rsUIDispersionFilterPara = std::make_shared<RSUIDispersionFilterPara>();
    rsUIDispersionFilterPara->SetDispersion(dispersionPara);

    std::string out;
    rsUIDispersionFilterPara->Dump(out);
    EXPECT_NE(out, "");
}

/**
 * @tc.name: SetPropertyTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDispersionFilterTest, SetPropertyTest, TestSize.Level1)
{
    auto dispersionPara = std::make_shared<DispersionPara>();
    dispersionPara->SetMask(std::make_shared<MaskPara>());
    dispersionPara->SetOpacity(0.5f);
    Vector2f offset = Vector2f(0.5f, 0.5f);
    dispersionPara->SetRedOffset(offset);
    dispersionPara->SetGreenOffset(offset);
    dispersionPara->SetBlueOffset(offset);

    auto rsUIDispersionFilterPara = std::make_shared<RSUIDispersionFilterPara>();
    rsUIDispersionFilterPara->SetProperty(nullptr);
    ASSERT_EQ(rsUIDispersionFilterPara->properties_.find(RSUIFilterType::DISPERSION_OPACITY), 
        rsUIDispersionFilterPara->properties_.end());

    auto rsUIEdgeLightFilterPara = std::make_shared<RSUIEdgeLightFilterPara>();
    rsUIDispersionFilterPara->SetProperty(rsUIEdgeLightFilterPara);
    ASSERT_EQ(rsUIDispersionFilterPara->properties_.find(RSUIFilterType::DISPERSION_OPACITY), 
        rsUIDispersionFilterPara->properties_.end());

    rsUIEdgeLightFilterPara->type_ = RSUIFilterType::DISPERSION;
    rsUIDispersionFilterPara->SetProperty(rsUIEdgeLightFilterPara);
    ASSERT_EQ(rsUIDispersionFilterPara->properties_.find(RSUIFilterType::DISPERSION_OPACITY), 
        rsUIDispersionFilterPara->properties_.end());

    rsUIDispersionFilterPara->SetDispersion(dispersionPara);
    rsUIDispersionFilterPara->SetProperty(rsUIDispersionFilterPara);
    ASSERT_EQ(rsUIDispersionFilterPara->properties_.find(RSUIFilterType::DISPERSION_OPACITY), 
        rsUIDispersionFilterPara->properties_.end());
}

/**
 * @tc.name: SetMaskTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDispersionFilterTest, SetMaskTest, TestSize.Level1)
{
    auto rsUIDispersionFilterPara = std::make_shared<RSUIDispersionFilterPara>();

    std::shared_ptr<RSUIMaskPara> maskProperty = rsUIDispersionFilterPara->CreateMask(RSUIFilterType::PIXEL_MAP_MASK);
    rsUIDispersionFilterPara->SetMask(maskProperty);
    auto iter = rsUIDispersionFilterPara->properties_.find(RSUIFilterType::PIXEL_MAP_MASK);
    ASSERT_EQ(iter, rsUIDispersionFilterPara->properties_.end());

    maskProperty = rsUIDispersionFilterPara->CreateMask(RSUIFilterType::RIPPLE_MASK);
    rsUIDispersionFilterPara->SetMask(maskProperty);
    iter = rsUIDispersionFilterPara->properties_.find(RSUIFilterType::RIPPLE_MASK);
    ASSERT_NE(iter, rsUIDispersionFilterPara->properties_.end());
}

/**
 * @tc.name: SetOpacityTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDispersionFilterTest, SetOpacityTest, TestSize.Level1)
{
    auto rsUIDispersionFilterPara = std::make_shared<RSUIDispersionFilterPara>();

    rsUIDispersionFilterPara->SetOpacity(0.5f);
    auto iter = rsUIDispersionFilterPara->properties_.find(RSUIFilterType::DISPERSION_OPACITY);
    ASSERT_NE(iter, rsUIDispersionFilterPara->properties_.end());

    auto property = std::static_pointer_cast<RSAnimatableProperty<float>>(iter->second);
    ASSERT_NE(property, nullptr);
    EXPECT_EQ(property->Get(), 0.5f);
}

/**
 * @tc.name: SetOffsetTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDispersionFilterTest, SetOffsetTest, TestSize.Level1)
{
    auto rsUIDispersionFilterPara = std::make_shared<RSUIDispersionFilterPara>();

    Vector2f offset = Vector2f(0.5f, 0.5f);
    rsUIDispersionFilterPara->SetRedOffset(offset);
    rsUIDispersionFilterPara->SetGreenOffset(offset);
    rsUIDispersionFilterPara->SetBlueOffset(offset);

    auto iter = rsUIDispersionFilterPara->properties_.find(RSUIFilterType::DISPERSION_BLUE_OFFSET);
    ASSERT_NE(iter, rsUIDispersionFilterPara->properties_.end());
    auto property = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(iter->second);
    ASSERT_NE(property, nullptr);
    EXPECT_EQ(property->Get(), offset);
}

/**
 * @tc.name: CreateRSRenderFilterTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDispersionFilterTest, CreateRSRenderFilterTest, TestSize.Level1)
{
    auto dispersionPara = std::make_shared<DispersionPara>();

    dispersionPara->SetMask(std::make_shared<MaskPara>());
    dispersionPara->SetOpacity(0.5f);
    Vector2f offset = Vector2f(0.5f, 0.5f);
    dispersionPara->SetRedOffset(offset);
    dispersionPara->SetGreenOffset(offset);
    dispersionPara->SetBlueOffset(offset);

    auto rsUIDispersionFilterPara = std::make_shared<RSUIDispersionFilterPara>();
    rsUIDispersionFilterPara->SetDispersion(dispersionPara);

    auto rsRenderFilterParaBase = rsUIDispersionFilterPara->CreateRSRenderFilter();
    EXPECT_EQ(rsRenderFilterParaBase, nullptr);
}

/**
 * @tc.name: GetLeafPropertiesTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDispersionFilterTest, GetLeafPropertiesTest, TestSize.Level1)
{
    auto dispersionPara = std::make_shared<DispersionPara>();

    dispersionPara->SetMask(std::make_shared<MaskPara>());
    dispersionPara->SetOpacity(0.5f);
    Vector2f offset = Vector2f(0.5f, 0.5f);
    dispersionPara->SetRedOffset(offset);
    dispersionPara->SetGreenOffset(offset);
    dispersionPara->SetBlueOffset(offset);

    auto rsUIDispersionFilterPara = std::make_shared<RSUIDispersionFilterPara>();
    rsUIDispersionFilterPara->SetDispersion(dispersionPara);

    auto rsPropertyBaseVector = rsUIDispersionFilterPara->GetLeafProperties();
    EXPECT_EQ(rsPropertyBaseVector.size(), 0);
}

} // namespace OHOS::Rosen