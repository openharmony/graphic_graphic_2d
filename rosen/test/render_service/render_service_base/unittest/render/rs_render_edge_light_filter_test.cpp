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
#include "render/rs_render_edge_light_filter.h"
#include "render/rs_shader_mask.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderEdgeLightFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderEdgeLightFilterTest::SetUpTestCase() {}
void RSRenderEdgeLightFilterTest::TearDownTestCase() {}
void RSRenderEdgeLightFilterTest::SetUp() {}
void RSRenderEdgeLightFilterTest::TearDown() {}

/**
 * @tc.name: GetDescription001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderEdgeLightFilterTest, GetDescription001, TestSize.Level1)
{
    auto rsRenderEdgeLightFilterPara= std::make_shared<RSRenderEdgeLightFilterPara>(0);
    std::string out;
    rsRenderEdgeLightFilterPara->GetDescription(out);
    EXPECT_NE(out.find("RSRenderEdgeLightFilterPara"), string::npos);
}

/**
 * @tc.name: WriteToParcel001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderEdgeLightFilterTest, WriteToParcel001, TestSize.Level1)
{
    auto rsRenderEdgeLightFilterPara= std::make_shared<RSRenderEdgeLightFilterPara>(0);
    Parcel parcel;
    auto ret = rsRenderEdgeLightFilterPara->WriteToParcel(parcel);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: ReadFromParcel001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderEdgeLightFilterTest, ReadFromParcel001, TestSize.Level1)
{
    auto rsRenderEdgeLightFilterPara= std::make_shared<RSRenderEdgeLightFilterPara>(0);
    Parcel parcel;
    auto ret = rsRenderEdgeLightFilterPara->ReadFromParcel(parcel);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: GetLeafRenderProperties001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderEdgeLightFilterTest, GetLeafRenderProperties001, TestSize.Level1)
{
    auto rsRenderEdgeLightFilterPara= std::make_shared<RSRenderEdgeLightFilterPara>(0);
    auto rsRenderPropertyBaseVec = rsRenderEdgeLightFilterPara->GetLeafRenderProperties();
    EXPECT_TRUE(rsRenderPropertyBaseVec.empty());
}

/**
 * @tc.name: GenerateGEVisualEffect001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderEdgeLightFilterTest, GenerateGEVisualEffect001, TestSize.Level1)
{
    auto rsEdgeLightFilter = std::make_shared<RSRenderEdgeLightFilterPara>(0);
    rsEdgeLightFilter->GenerateGEVisualEffect(nullptr);

    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    rsEdgeLightFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());

    rsEdgeLightFilter->color_ = Vector4f(0.5f, 0.5f, 0.5f, 0.0f);
    rsEdgeLightFilter->alpha_ = 0.8f;
    rsEdgeLightFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());

    rsEdgeLightFilter->mask_ =
        std::make_shared<RSShaderMask>(std::make_shared<RSRenderMaskPara>(RSUIFilterType::RIPPLE_MASK));
    rsEdgeLightFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());
}

/**
 * @tc.name: ParseFilterValuesTest001
 * @tc.desc: Verify function ParseFilterValues
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderEdgeLightFilterTest, ParseFilterValuesTest001, TestSize.Level1)
{
    auto filter = std::make_shared<RSRenderEdgeLightFilterPara>(0);
    EXPECT_FALSE(filter->ParseFilterValues());

    auto alphaRenderProperty = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f, 0);
    filter->Setter(RSUIFilterType::EDGE_LIGHT_ALPHA, alphaRenderProperty);
    EXPECT_FALSE(filter->ParseFilterValues());

    auto colorRenderProperty =
        std::make_shared<RSRenderAnimatableProperty<Vector4f>>(Vector4f(0.5f, 0.5f, 0.5f, 0.0f), 0);
    filter->Setter(RSUIFilterType::EDGE_LIGHT_COLOR, colorRenderProperty);
    EXPECT_TRUE(filter->ParseFilterValues());

    filter->maskType_ = RSUIFilterType::RIPPLE_MASK;
    EXPECT_FALSE(filter->ParseFilterValues());

    auto maskRenderProperty = std::make_shared<RSRenderRippleMaskPara>(0);
    filter->Setter(RSUIFilterType::RIPPLE_MASK, maskRenderProperty);
    EXPECT_TRUE(filter->ParseFilterValues());
}
/**
 * @tc.name: CreateRenderProperty001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderEdgeLightFilterTest, CreateRenderProperty001, TestSize.Level1)
{
    auto rsRenderEdgeLightFilterPara = std::make_shared<RSRenderEdgeLightFilterPara>(0);

    auto renderProperty = rsRenderEdgeLightFilterPara->CreateRenderProperty(RSUIFilterType::RIPPLE_MASK);
    EXPECT_NE(renderProperty, nullptr);

    renderProperty = rsRenderEdgeLightFilterPara->CreateRenderProperty(RSUIFilterType::RADIAL_GRADIENT_MASK);
    EXPECT_NE(renderProperty, nullptr);

    renderProperty = rsRenderEdgeLightFilterPara->CreateRenderProperty(RSUIFilterType::PIXEL_MAP_MASK);
    EXPECT_NE(renderProperty, nullptr);
    
    renderProperty = rsRenderEdgeLightFilterPara->CreateRenderProperty(RSUIFilterType::EDGE_LIGHT_ALPHA);
    EXPECT_NE(renderProperty, nullptr);
    
    renderProperty = rsRenderEdgeLightFilterPara->CreateRenderProperty(RSUIFilterType::EDGE_LIGHT_COLOR);
    EXPECT_NE(renderProperty, nullptr);
}
} // namespace OHOS::Rosen