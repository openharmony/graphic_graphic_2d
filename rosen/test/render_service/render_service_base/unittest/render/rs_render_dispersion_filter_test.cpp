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
#include "ge_shader_filter_params.h"
#include "ge_visual_effect.h"
#include "ge_visual_effect_container.h"
#include "gtest/gtest.h"

#include "render/rs_render_dispersion_filter.h"
#include "render/rs_shader_mask.h"


using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderDispersionFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderDispersionFilterTest::SetUpTestCase() {}
void RSRenderDispersionFilterTest::TearDownTestCase() {}
void RSRenderDispersionFilterTest::SetUp() {}
void RSRenderDispersionFilterTest::TearDown() {}

/**
 * @tc.name: GetDescription001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderDispersionFilterTest, GetDescription001, TestSize.Level1)
{
    auto rsRenderDispersionFilterPara = std::make_shared<RSRenderDispersionFilterPara>(0);
    std::string out;
    rsRenderDispersionFilterPara->GetDescription(out);
    EXPECT_FALSE(out.empty());
}

/**
 * @tc.name: CreateRenderProperty001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderDispersionFilterTest, CreateRenderProperty001, TestSize.Level1)
{
    auto rsRenderDispersionFilterPara = std::make_shared<RSRenderDispersionFilterPara>(0);

    auto renderPropert = rsRenderDispersionFilterPara->CreateRenderProperty(RSUIFilterType::PIXEL_MAP_MASK);
    EXPECT_EQ(renderPropert, nullptr);

    renderPropert = rsRenderDispersionFilterPara->CreateRenderProperty(RSUIFilterType::RADIAL_GRADIENT_MASK);
    EXPECT_EQ(renderPropert, nullptr);

    renderPropert = rsRenderDispersionFilterPara->CreateRenderProperty(RSUIFilterType::DISPERSION_RED_OFFSET);
    EXPECT_NE(renderPropert, nullptr);
    renderPropert = rsRenderDispersionFilterPara->CreateRenderProperty(RSUIFilterType::DISPERSION_GREEN_OFFSET);
    EXPECT_NE(renderPropert, nullptr);
    renderPropert = rsRenderDispersionFilterPara->CreateRenderProperty(RSUIFilterType::DISPERSION_BLUE_OFFSET);
    EXPECT_NE(renderPropert, nullptr);
}

/**
 * @tc.name: WriteToParcel001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderDispersionFilterTest, WriteToParcel001, TestSize.Level1)
{
    auto rsRenderDispersionFilterPara = std::make_shared<RSRenderDispersionFilterPara>(0);
    Parcel parcel;
    auto ret = rsRenderDispersionFilterPara->WriteToParcel(parcel);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: ReadFromParcel001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderDispersionFilterTest, ReadFromParcel001, TestSize.Level1)
{
    auto rsRenderDispersionFilterPara = std::make_shared<RSRenderDispersionFilterPara>(0);
    Parcel parcel;
    auto ret = rsRenderDispersionFilterPara->ReadFromParcel(parcel);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: GetLeafRenderProperties001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderDispersionFilterTest, GetLeafRenderProperties001, TestSize.Level1)
{
    auto rsRenderDispersionFilterPara = std::make_shared<RSRenderDispersionFilterPara>(0);
    auto rsRenderPropertyBaseVec = rsRenderDispersionFilterPara->GetLeafRenderProperties();
    EXPECT_TRUE(rsRenderPropertyBaseVec.empty());

    rsRenderDispersionFilterPara->maskType_ = RSUIFilterType::PIXEL_MAP_MASK;
    rsRenderPropertyBaseVec = rsRenderDispersionFilterPara->GetLeafRenderProperties();
    EXPECT_TRUE(rsRenderPropertyBaseVec.empty());

    rsRenderDispersionFilterPara->properties_[RSUIFilterType::PIXEL_MAP_MASK] =
        rsRenderDispersionFilterPara->CreateRenderProperty(RSUIFilterType::PIXEL_MAP_MASK);
    rsRenderDispersionFilterPara->properties_[RSUIFilterType::DISPERSION_OPACITY] =
        rsRenderDispersionFilterPara->CreateRenderProperty(RSUIFilterType::DISPERSION_OPACITY);
    rsRenderPropertyBaseVec = rsRenderDispersionFilterPara->GetLeafRenderProperties();
    EXPECT_EQ(rsRenderPropertyBaseVec.empty(), 1);
}

/**
 * @tc.name: GetRenderMask001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderDispersionFilterTest, GetRenderMask001, TestSize.Level1)
{
    auto rsRenderDispersionFilter = std::make_shared<RSRenderDispersionFilterPara>(0, RSUIFilterType::PIXEL_MAP_MASK);
    auto renderMask = rsRenderDispersionFilter->GetRenderMask();
    EXPECT_EQ(renderMask, nullptr);

    auto maskRenderProperty = std::make_shared<RSRenderMaskPara>(RSUIFilterType::PIXEL_MAP_MASK);
    rsRenderDispersionFilter->Setter(RSUIFilterType::PIXEL_MAP_MASK, maskRenderProperty);
    renderMask = rsRenderDispersionFilter->GetRenderMask();
    EXPECT_NE(renderMask, nullptr);
}

/**
 * @tc.name: GenerateGEVisualEffect001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderDispersionFilterTest, GenerateGEVisualEffect001, TestSize.Level1)
{
    auto filter = std::make_shared<RSRenderDispersionFilterPara>(0);

    filter->mask_ = nullptr;
    filter->opacity_ = 0.5f;
    filter->redOffsetX_ = 0.5f;
    filter->redOffsetY_ = 0.5f;
    filter->greenOffsetX_ = 0.5f;
    filter->greenOffsetY_ = 0.5f;
    filter->blueOffsetX_ = 0.5f;
    filter->blueOffsetY_ = 0.5f;

    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    filter->GenerateGEVisualEffect(nullptr);
    EXPECT_TRUE(visualEffectContainer->filterVec_.empty());

    filter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());

    filter->mask_ = std::make_shared<RSShaderMask>(std::make_shared<RSRenderMaskPara>(RSUIFilterType::PIXEL_MAP_MASK));
    filter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());
}

/**
 * @tc.name: GetMask001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderDispersionFilterTest, GetMask001, TestSize.Level1)
{
    auto filter = std::make_shared<RSRenderDispersionFilterPara>(0);
    EXPECT_EQ(filter->GetMask(), nullptr);

    auto mask = std::make_shared<RSShaderMask>(std::make_shared<RSRenderMaskPara>(RSUIFilterType::RIPPLE_MASK));
    filter->mask_ = mask;
    EXPECT_EQ(filter->GetMask(), mask);
}

/**
 * @tc.name: ParseFilterValuesTest001
 * @tc.desc: Verify function ParseFilterValues
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderDispersionFilterTest, ParseFilterValuesTest001, TestSize.Level1)
{
    auto filter = std::make_shared<RSRenderDispersionFilterPara>(0, RSUIFilterType::RIPPLE_MASK);
    EXPECT_FALSE(filter->ParseFilterValues());

    auto redOffsetProperty = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(Vector2f(0.5f, 0.5f), 0);
    filter->Setter(RSUIFilterType::DISPERSION_RED_OFFSET, redOffsetProperty);
    EXPECT_FALSE(filter->ParseFilterValues());

    auto greenOffsetProperty = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(Vector2f(0.5f, 0.5f), 0);
    filter->Setter(RSUIFilterType::DISPERSION_GREEN_OFFSET, greenOffsetProperty);
    EXPECT_FALSE(filter->ParseFilterValues());

    auto blueOffsetProperty = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(Vector2f(0.5f, 0.5f), 0);
    filter->Setter(RSUIFilterType::DISPERSION_BLUE_OFFSET, blueOffsetProperty);
    EXPECT_FALSE(filter->ParseFilterValues());

    auto opacityProperty = std::make_shared<RSRenderAnimatableProperty<float>>(0.5f, 0);
    filter->Setter(RSUIFilterType::DISPERSION_OPACITY, opacityProperty);
    EXPECT_TRUE(filter->ParseFilterValues());

    auto maskRenderProperty = std::make_shared<RSRenderMaskPara>(RSUIFilterType::RIPPLE_MASK);
    filter->Setter(RSUIFilterType::RIPPLE_MASK, maskRenderProperty);
    EXPECT_TRUE(filter->ParseFilterValues());
}
} // namespace OHOS::Rosen