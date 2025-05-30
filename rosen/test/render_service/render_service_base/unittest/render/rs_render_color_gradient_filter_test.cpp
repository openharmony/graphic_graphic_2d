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

#include "ge_visual_effect.h"
#include "ge_visual_effect_container.h"
#include "render/rs_render_color_gradient_filter.h"
#include "render/rs_shader_mask.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSRenderColorGradientFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderColorGradientFilterTest::SetUpTestCase() {}
void RSRenderColorGradientFilterTest::TearDownTestCase() {}
void RSRenderColorGradientFilterTest::SetUp() {}
void RSRenderColorGradientFilterTest::TearDown() {}

/**
 * @tc.name: RSRenderColorGradientFilterTest001
 * @tc.desc: Verify function RSRenderColorGradientFilterTest
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderColorGradientFilterTest, GetDescription001, TestSize.Level1)
{
    auto rsRenderFilterPara = std::make_shared<RSRenderColorGradientFilterPara>(0);
    std::string out;
    rsRenderFilterPara->GetDescription(out);
    std::cout << out << std::endl;
    EXPECT_TRUE(out.length() > 0);
}

/**
 * @tc.name: WriteToParcel001
 * @tc.desc: test WriteToParcel
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderColorGradientFilterTest, WriteToParcel001, TestSize.Level1)
{
    auto fliterPara = std::make_shared<RSRenderColorGradientFilterPara>(0);
    Parcel parcel;
    auto ret = fliterPara->WriteToParcel(parcel);
    EXPECT_FALSE(ret);

    // 1.0, 0.0, 0.0, 1.0 is the color rgba params
    std::vector<float> colors = { 1.0, 0.0, 0.0, 1.0 };
    std::vector<float> positions = { 1.0, 1.0 }; // 1.0, 1.0 is poition xy params
    std::vector<float> strengths = { 0.5 }; // 0.5 is strength params

    auto color = std::make_shared<RSRenderAnimatableProperty<std::vector<float>>>(colors);
    fliterPara->Setter(RSUIFilterType::COLOR_GRADIENT_COLOR, color);
    ret = fliterPara->WriteToParcel(parcel);
    EXPECT_FALSE(ret);

    auto position = std::make_shared<RSRenderAnimatableProperty<std::vector<float>>>(positions);
    fliterPara->Setter(RSUIFilterType::COLOR_GRADIENT_POSITION, position);
    ret = fliterPara->WriteToParcel(parcel);
    EXPECT_FALSE(ret);

    auto strength = std::make_shared<RSRenderAnimatableProperty<std::vector<float>>>();
    fliterPara->Setter(RSUIFilterType::COLOR_GRADIENT_STRENGTH, strength);
    ret = fliterPara->WriteToParcel(parcel);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: ReadFromParcel001
 * @tc.desc: test ReadFromParcel
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderColorGradientFilterTest, ReadFromParcel001, TestSize.Level1)
{
    auto fliterPara = std::make_shared<RSRenderColorGradientFilterPara>(0);
    Parcel parcel;
    auto ret = fliterPara->ReadFromParcel(parcel);
    EXPECT_FALSE(ret);

    ret = fliterPara->WriteToParcel(parcel);
    EXPECT_FALSE(ret);

    ret = fliterPara->ReadFromParcel(parcel);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: CreateMaskRenderProperty001
 * @tc.desc: test CreateMaskRenderProperty
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderColorGradientFilterTest, CreateMaskRenderProperty001, TestSize.Level1)
{
    auto fliterPara = std::make_shared<RSRenderColorGradientFilterPara>(0);

    auto renderPropert = fliterPara->CreateMaskRenderProperty(RSUIFilterType::RIPPLE_MASK);
    EXPECT_NE(renderPropert, nullptr);

    renderPropert = fliterPara->CreateMaskRenderProperty(RSUIFilterType::BLUR);
    EXPECT_EQ(renderPropert, nullptr);
}

/**
 * @tc.name: GetLeafRenderProperties001
 * @tc.desc: test GetLeafRenderProperties
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderColorGradientFilterTest, GetLeafRenderProperties001, TestSize.Level1)
{
    auto fliterPara = std::make_shared<RSRenderColorGradientFilterPara>(0);
    auto rsRenderPropertyBaseVec = fliterPara->GetLeafRenderProperties();
    EXPECT_TRUE(rsRenderPropertyBaseVec.empty());

    // 1.0, 0.0, 0.0, 1.0 is the color rgba params
    std::vector<float> colors = { 1.0, 0.0, 0.0, 1.0 };
    std::vector<float> positions = { 1.0, 1.0 }; // 1.0, 1.0 is poition xy params
    std::vector<float> strengths = { 0.5 }; // 0.5 is strength params

    auto color = std::make_shared<RSRenderAnimatableProperty<std::vector<float>>>(colors);
    fliterPara->Setter(RSUIFilterType::COLOR_GRADIENT_COLOR, color);
    rsRenderPropertyBaseVec = fliterPara->GetLeafRenderProperties();
    EXPECT_TRUE(rsRenderPropertyBaseVec.empty());

    auto position = std::make_shared<RSRenderAnimatableProperty<std::vector<float>>>(positions);
    fliterPara->Setter(RSUIFilterType::COLOR_GRADIENT_POSITION, position);
    rsRenderPropertyBaseVec = fliterPara->GetLeafRenderProperties();
    EXPECT_TRUE(rsRenderPropertyBaseVec.empty());

    auto strength = std::make_shared<RSRenderAnimatableProperty<std::vector<float>>>();
    fliterPara->Setter(RSUIFilterType::COLOR_GRADIENT_STRENGTH, strength);
    rsRenderPropertyBaseVec = fliterPara->GetLeafRenderProperties();
    EXPECT_FALSE(rsRenderPropertyBaseVec.empty());
}

/**
 * @tc.name: RSRenderColorGradientFilterTest001
 * @tc.desc: Verify function RSRenderColorGradientFilterTest
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderColorGradientFilterTest, RSRenderColorGradientFilterTest001, TestSize.Level1)
{
    // 1.0, 0.0, 0.0, 1.0 is the color rgba params
    std::vector<float> colors = { 1.0, 0.0, 0.0, 1.0 };
    std::vector<float> positions = { 1.0, 1.0 }; // 1.0, 1.0 is position xy params
    std::vector<float> strengths = { 0.5 };      // 0.5 is strength params

    auto filter = std::make_shared<RSRenderColorGradientFilterPara>(0);
    filter->GenerateGEVisualEffect(nullptr);

    filter->colors_ = { 1.0, 0.0, 0.0, 1.0 };
    filter->positions_ = { 1.0, 1.0 }; // 1.0, 1.0 is position xy params
    filter->strengths_ = { 0.5 };      // 0.5 is strength params

    EXPECT_EQ(filter->GetMask(), nullptr);
    EXPECT_FALSE(filter->GetColors().empty());
    EXPECT_FALSE(filter->GetPositions().empty());
    EXPECT_FALSE(filter->GetStrengths().empty());

    filter->GenerateGEVisualEffect(container);
    EXPECT_FALSE(container->GetFilters().empty());

    filter->mask_ = std::make_shared<RSShaderMask>(std::make_shared<RSRenderMaskPara>(RSUIFilterType::RIPPLE_MASK));
    EXPECT_NE(filter->GetMask(), nullptr);

    auto container = std::make_shared<Drawing::GEVisualEffectContainer>();
    filter->GenerateGEVisualEffect(container);
    EXPECT_FALSE(container->GetFilters().empty());
}

/**
 * @tc.name: ParseFilterValuesTest001
 * @tc.desc: Verify function ParseFilterValues
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderColorGradientFilterTest, ParseFilterValuesTest001, TestSize.Level1)
{
    std::vector<float> colors = { 1.0, 0.0, 0.0, 1.0 };
    std::vector<float> positions = { 1.0, 1.0 }; // 1.0, 1.0 is position xy params
    std::vector<float> strengths = { 0.5 };      // 0.5 is strength params
    auto filter = std::make_shared<RSRenderColorGradientFilterPara>(0, RSUIFilterType::RIPPLE_MASK);
    EXPECT_FALSE(filter->ParseFilterValues());

    auto colorsProperty = std::make_shared<RSRenderAnimatableProperty<std::vector<float>>>(
        colors, 0, RSRenderPropertyType::PROPERTY_SHADER_PARAM);
    filter->Setter(RSUIFilterType::COLOR_GRADIENT_COLOR, colorsProperty);
    EXPECT_FALSE(filter->ParseFilterValues());

    auto positionsProperty = std::make_shared<RSRenderAnimatableProperty<std::vector<float>>>(
        positions, 0, RSRenderPropertyType::PROPERTY_SHADER_PARAM);
    filter->Setter(RSUIFilterType::COLOR_GRADIENT_POSITION, positionsProperty);
    EXPECT_FALSE(filter->ParseFilterValues());

    auto strengthsProperty = std::make_shared<RSRenderAnimatableProperty<std::vector<float>>>(
        strengths, 0, RSRenderPropertyType::PROPERTY_SHADER_PARAM);
    filter->Setter(RSUIFilterType::COLOR_GRADIENT_STRENGTH, strengthsProperty);
    EXPECT_TRUE(filter->ParseFilterValues());

    auto maskRenderProperty = std::make_shared<RSRenderMaskPara>(RSUIFilterType::RIPPLE_MASK);
    filter->Setter(RSUIFilterType::RIPPLE_MASK, maskRenderProperty);
    EXPECT_TRUE(filter->ParseFilterValues());
}
} // namespace Rosen
} // namespace OHOS