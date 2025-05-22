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
#include "ui_effect/filter/include/filter_edge_light_para.h"
#include "ui_effect/property/include/rs_ui_edge_light_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSUIEdgeLightFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUIEdgeLightFilterTest::SetUpTestCase() {}
void RSUIEdgeLightFilterTest::TearDownTestCase() {}
void RSUIEdgeLightFilterTest::SetUp() {}
void RSUIEdgeLightFilterTest::TearDown() {}

/**
 * @tc.name: Equal001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIEdgeLightFilterTest, Equal001, TestSize.Level1)
{
    auto rsUIEdgeLightFilterPara1 = std::make_shared<RSUIEdgeLightFilterPara>();
    auto rsUIEdgeLightFilterPara2 = std::make_shared<RSUIEdgeLightFilterPara>();
    auto rsUIFilterParaBase = static_cast<std::shared_ptr<RSUIFilterParaBase>>(rsUIEdgeLightFilterPara2);
    
    EXPECT_TRUE(rsUIEdgeLightFilterPara1->Equals(rsUIFilterParaBase));
}

/**
 * @tc.name: Dump001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIEdgeLightFilterTest, Dump001, TestSize.Level1)
{
    std::string temp = 
        "RSUIEdgeLightFilterPara:[alpha:0.500000 color:rgba[0.500000, 0.500000, 0.500000, 0.500000]]";

    auto edgeLightPara = std::make_shared<EdgeLightPara>();

    auto maskPara = std::make_shared<MaskPara>();
    maskPara->type_ = MaskPara::Type::RIPPLE_MASK;
    edgeLightPara->SetMask(maskPara);
    edgeLightPara->SetAlpha(0.5f);
    Vector4f color = Vector4f(0.5f, 0.5f, 0.5f, 0.5f);
    edgeLightPara->SetColor(color);

    auto rsUIEdgeLightFilterPara = std::make_shared<RSUIEdgeLightFilterPara>();
    rsUIEdgeLightFilterPara->SetEdgeLight(edgeLightPara);

    std::string out;
    rsUIEdgeLightFilterPara->Dump(out);
    EXPECT_EQ(temp, out);
}

/**
 * @tc.name: SetProperty001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIEdgeLightFilterTest, SetProperty001, TestSize.Level1)
{
    auto edgeLightPara1 = std::make_shared<EdgeLightPara>();
    auto maskPara1 = std::make_shared<MaskPara>();
    edgeLightPara1->SetMask(maskPara1);
    edgeLightPara1->SetAlpha(0.5f);
    Vector4f color1 = Vector4f(0.5f, 0.5f, 0.5f, 0.5f);
    edgeLightPara1->SetColor(color1);

    auto edgeLightPara2 = std::make_shared<EdgeLightPara>();
    auto maskPara2 = std::make_shared<MaskPara>();
    edgeLightPara2->SetMask(maskPara2);
    edgeLightPara2->SetAlpha(0.5f);
    Vector4f color2 = Vector4f(0.5f, 0.5f, 0.5f, 0.5f);
    edgeLightPara2->SetColor(color2);

    auto rsUIEdgeLightFilterPara1 = std::make_shared<RSUIEdgeLightFilterPara>();
    rsUIEdgeLightFilterPara1->SetEdgeLight(edgeLightPara1);

    auto rsUIEdgeLightFilterPara2 = std::make_shared<RSUIEdgeLightFilterPara>();
    rsUIEdgeLightFilterPara2->SetEdgeLight(edgeLightPara2);

    auto rsUIFilterParaBase = static_cast<std::shared_ptr<RSUIFilterParaBase>>(rsUIEdgeLightFilterPara2);
    rsUIEdgeLightFilterPara1->SetProperty(rsUIFilterParaBase);

    auto iter = rsUIEdgeLightFilterPara1->properties_.find(RSUIFilterType::EDGE_LIGHT_ALPHA);
    ASSERT_NE(iter, rsUIEdgeLightFilterPara1->properties_.end());

    auto property = std::static_pointer_cast<RSAnimatableProperty<float>>(iter->second);
    ASSERT_NE(property, nullptr);
    EXPECT_EQ(property->Get(), 0.5f);
}

/**
 * @tc.name: SetAlpha001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIEdgeLightFilterTest, SetAlpha001, TestSize.Level1)
{
    auto rsUIEdgeLightFilterPara = std::make_shared<RSUIEdgeLightFilterPara>();

    rsUIEdgeLightFilterPara->SetAlpha(0.5f);
    auto iter = rsUIEdgeLightFilterPara->properties_.find(RSUIFilterType::EDGE_LIGHT_ALPHA);
    ASSERT_NE(iter, rsUIEdgeLightFilterPara->properties_.end());

    auto property = std::static_pointer_cast<RSAnimatableProperty<float>>(iter->second);
    ASSERT_NE(property, nullptr);
    EXPECT_EQ(property->Get(), 0.5f);
}

/**
 * @tc.name: SetColor001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIEdgeLightFilterTest, SetColor001, TestSize.Level1)
{
    auto rsUIEdgeLightFilterPara = std::make_shared<RSUIEdgeLightFilterPara>();

    Vector4f color = Vector4f(0.5f, 0.5f, 0.5f, 0.5f);
    rsUIEdgeLightFilterPara->SetColor(color);
    auto iter = rsUIEdgeLightFilterPara->properties_.find(RSUIFilterType::EDGE_LIGHT_COLOR);
    ASSERT_NE(iter, rsUIEdgeLightFilterPara->properties_.end());

    auto property = std::static_pointer_cast<RSAnimatableProperty<Vector4f>>(iter->second);
    ASSERT_NE(property, nullptr);
    EXPECT_EQ(property->Get(), color);
}

/**
 * @tc.name: SetMask001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIEdgeLightFilterTest, SetMask001, TestSize.Level1)
{
    auto rsUIEdgeLightFilterPara = std::make_shared<RSUIEdgeLightFilterPara>();

    auto mask = std::make_shared<RSUIMaskPara>(RSUIFilterType::EDGE_LIGHT);
    rsUIEdgeLightFilterPara->SetMask(mask);

    EXPECT_EQ(rsUIEdgeLightFilterPara->properties_[RSUIFilterType::EDGE_LIGHT], mask);
}

/**
 * @tc.name: CreateRSRenderFilter001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIEdgeLightFilterTest, CreateRSRenderFilter001, TestSize.Level1)
{
    auto edgeLightPara = std::make_shared<EdgeLightPara>();

    auto maskPara = std::make_shared<MaskPara>();
    maskPara->type_ = MaskPara::Type::RIPPLE_MASK;
    edgeLightPara->SetMask(maskPara);
    edgeLightPara->SetAlpha(0.5f);
    Vector4f color = Vector4f(0.5f, 0.5f, 0.5f, 0.5f);
    edgeLightPara->SetColor(color);

    auto rsUIEdgeLightFilterPara = std::make_shared<RSUIEdgeLightFilterPara>();
    rsUIEdgeLightFilterPara->SetEdgeLight(edgeLightPara);

    auto rsRenderFilterParaBase = rsUIEdgeLightFilterPara->CreateRSRenderFilter();
    EXPECT_NE(rsRenderFilterParaBase, nullptr);
}

/**
 * @tc.name: GetLeafProperties001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIEdgeLightFilterTest, GetLeafProperties001, TestSize.Level1)
{
    auto edgeLightPara = std::make_shared<EdgeLightPara>();
;
    auto maskPara = std::make_shared<MaskPara>();
    maskPara->type_ = MaskPara::Type::RIPPLE_MASK;
    edgeLightPara->SetMask(maskPara);
    edgeLightPara->SetAlpha(0.5f);
    Vector4f color = Vector4f(0.5f, 0.5f, 0.5f, 0.5f);
    edgeLightPara->SetColor(color);

    auto rsUIEdgeLightFilterPara = std::make_shared<RSUIEdgeLightFilterPara>();
    rsUIEdgeLightFilterPara->SetEdgeLight(edgeLightPara);

    auto rsPropertyBaseVector = rsUIEdgeLightFilterPara->GetLeafProperties();
    EXPECT_NE(rsPropertyBaseVector.size(), 0);
}
} // namespace OHOS::Rosen