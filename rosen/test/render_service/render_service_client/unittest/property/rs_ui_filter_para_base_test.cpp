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
#include "ui_effect/filter/include/filter_blur_para.h"
#include "ui_effect/filter/include/filter_color_gradient_para.h"
#include "ui_effect/filter/include/filter_direction_light_para.h"
#include "ui_effect/filter/include/filter_dispersion_para.h"
#include "ui_effect/filter/include/filter_displacement_distort_para.h"
#include "ui_effect/filter/include/filter_edge_light_para.h"
#include "ui_effect/filter/include/filter_variable_radius_blur_para.h"
#include "ui_effect/property/include/rs_ui_blur_filter.h"
#include "ui_effect/property/include/rs_ui_filter_base.h"
#include "ui_effect/property/include/rs_ui_filter_para_base.h"


using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSUIFilterParaBaseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUIFilterParaBaseTest::SetUpTestCase() {}
void RSUIFilterParaBaseTest::TearDownTestCase() {}
void RSUIFilterParaBaseTest::SetUp() {}
void RSUIFilterParaBaseTest::TearDown() {}

/**
 * @tc.name: RSNGFilterBaseCreate
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIFilterParaBaseTest, RSNGFilterBaseCreate001, TestSize.Level1)
{
    auto filter1 = RSNGFilterBase::Create(RSNGEffectType::BLUR);
    EXPECT_NE(filter1, nullptr);
    EXPECT_EQ(filter1->GetType(), RSNGEffectType::BLUR);

    auto filter2 = RSNGFilterBase::Create(RSNGEffectType::DIRECTION_LIGHT);
    EXPECT_NE(filter2, nullptr);
    EXPECT_EQ(filter2->GetType(), RSNGEffectType::DIRECTION_LIGHT);

    auto filter3 = RSNGFilterBase::Create(RSNGEffectType::VARIABLE_RADIUS_BLUR);
    EXPECT_NE(filter3, nullptr);
    EXPECT_EQ(filter3->GetType(), RSNGEffectType::VARIABLE_RADIUS_BLUR);
}

/**
 * @tc.name: RSNGFilterBaseCreate
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIFilterParaBaseTest, RSNGFilterBaseCreate002, TestSize.Level1)
{
    EXPECT_EQ(RSNGFilterBase::Create(nullptr), nullptr);

    auto para1 = std::make_shared<DisplacementDistortPara>();
    auto filter1 = RSNGFilterBase::Create(para1);
    EXPECT_EQ(filter1, nullptr);

    auto para2 = std::make_shared<EdgeLightPara>();
    auto filter2 = RSNGFilterBase::Create(para2);
    EXPECT_EQ(filter2, nullptr);

    auto para3 = std::make_shared<DirectionLightPara>();
    auto filter3 = RSNGFilterBase::Create(para3);
    EXPECT_NE(filter3, nullptr);

    auto para4 = std::make_shared<DispersionPara>();
    auto filter4 = RSNGFilterBase::Create(para4);
    EXPECT_EQ(filter4, nullptr);

    auto para5 = std::make_shared<ColorGradientPara>();
    auto filter5 = RSNGFilterBase::Create(para5);
    EXPECT_EQ(filter5, nullptr);

    auto para6 = std::make_shared<VariableRadiusBlurPara>();
    auto filter6 = RSNGFilterBase::Create(para6);
    EXPECT_NE(filter6, nullptr);
}

/**
 * @tc.name: GetType001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIFilterParaBaseTest, GetType001, TestSize.Level1)
{
    auto rsUIBlurFilterPara = std::make_shared<RSUIBlurFilterPara>();
    auto rsUIFilterParaBase = static_cast<std::shared_ptr<RSUIFilterParaBase>>(rsUIBlurFilterPara);

    auto type = rsUIFilterParaBase->GetType();
    EXPECT_EQ(type, RSUIFilterType::BLUR);
}

/**
 * @tc.name: IsValid001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIFilterParaBaseTest, IsValid001, TestSize.Level1)
{
    auto rsUIBlurFilterPara = std::make_shared<RSUIBlurFilterPara>();
    auto rsUIFilterParaBase = static_cast<std::shared_ptr<RSUIFilterParaBase>>(rsUIBlurFilterPara);
    EXPECT_TRUE(rsUIFilterParaBase->IsValid());
}

/**
 * @tc.name: GetDescription001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIFilterParaBaseTest, GetDescription001, TestSize.Level1)
{
    std::string out;
    auto rsUIBlurFilterPara = std::make_shared<RSUIBlurFilterPara>();
    auto rsUIFilterParaBase = static_cast<std::shared_ptr<RSUIFilterParaBase>>(rsUIBlurFilterPara);
    rsUIFilterParaBase->GetDescription(out);
    EXPECT_EQ(out.length(), 0);
}

/**
 * @tc.name: Dump001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIFilterParaBaseTest, Dump001, TestSize.Level1)
{
    std::string out;
    auto rsUIBlurFilterPara = std::make_shared<RSUIBlurFilterPara>();
    auto rsUIFilterParaBase = static_cast<std::shared_ptr<RSUIFilterParaBase>>(rsUIBlurFilterPara);
    rsUIFilterParaBase->Dump(out);
    EXPECT_EQ(out.length(), 0);
}

/**
 * @tc.name: CreateRSRenderFilter001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIFilterParaBaseTest, CreateRSRenderFilter001, TestSize.Level1)
{
    auto rsUIBlurFilterPara = std::make_shared<RSUIBlurFilterPara>();
    auto rsUIFilterParaBase = static_cast<std::shared_ptr<RSUIFilterParaBase>>(rsUIBlurFilterPara);
    EXPECT_EQ(rsUIFilterParaBase->CreateRSRenderFilter(), nullptr);
}

/**
 * @tc.name: GetFilterProperties001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIFilterParaBaseTest, GetFilterProperties001, TestSize.Level1)
{
    auto rsUIBlurFilterPara = std::make_shared<RSUIBlurFilterPara>();
    auto rsUIFilterParaBase = static_cast<std::shared_ptr<RSUIFilterParaBase>>(rsUIBlurFilterPara);
    auto filterProperties = rsUIFilterParaBase->GetFilterProperties();
    EXPECT_EQ(filterProperties.size(), 0);
}

/**
 * @tc.name: Setter001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIFilterParaBaseTest, Setter001, TestSize.Level1)
{
    auto rsUIBlurFilterPara = std::make_shared<RSUIBlurFilterPara>();
    auto rsUIFilterParaBase = static_cast<std::shared_ptr<RSUIFilterParaBase>>(rsUIBlurFilterPara);

    float radiusX = 1.0f;
    rsUIFilterParaBase->Setter<RSAnimatableProperty<float>>(RSUIFilterType::BLUR_RADIUS_X, radiusX);

    auto iter = rsUIFilterParaBase->properties_.find(RSUIFilterType::BLUR_RADIUS_X);
    ASSERT_NE(iter, rsUIFilterParaBase->properties_.end());

    auto property = std::static_pointer_cast<RSAnimatableProperty<float>>(iter->second);
    ASSERT_NE(property, nullptr);
    EXPECT_EQ(property->Get(), radiusX);
}

/**
 * @tc.name: Clear001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIFilterParaBaseTest, Clear001, TestSize.Level1)
{
    auto rsUIBlurFilterPara = std::make_shared<RSUIBlurFilterPara>();
    auto rsUIFilterParaBase = static_cast<std::shared_ptr<RSUIFilterParaBase>>(rsUIBlurFilterPara);

    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSProperty<float>>();
    rsUIFilterParaBase->properties_[RSUIFilterType::NONE] = property;
    EXPECT_NE(rsUIFilterParaBase->properties_.size(), 0);
    rsUIFilterParaBase->Clear();
    EXPECT_EQ(rsUIFilterParaBase->properties_.size(), 0);
}

/**
 * @tc.name: GetLeafProperties001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIFilterParaBaseTest, GetLeafProperties001, TestSize.Level1)
{
    auto rsUIBlurFilterPara = std::make_shared<RSUIBlurFilterPara>();
    auto rsUIFilterParaBase = static_cast<std::shared_ptr<RSUIFilterParaBase>>(rsUIBlurFilterPara);

    auto leafProperties = rsUIFilterParaBase->GetLeafProperties();
    EXPECT_EQ(leafProperties.size(), 0);
}
} // namespace OHOS::Rosen