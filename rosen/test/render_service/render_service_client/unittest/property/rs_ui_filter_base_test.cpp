/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#include "ui_effect/property/include/rs_ui_filter_base.h"
#include "ui_effect/filter/include/filter_heat_distortion_para.h"
#include "ui_effect/filter/include/filter_blur_bubbles_rise_para.h"
#include "ui_effect/filter/include/filter_motion_blur_para.h"
#include "ui_effect/effect/include/distortion_collapse_effect_para.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSUIFilterBaseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUIFilterBaseTest::SetUpTestCase() {}
void RSUIFilterBaseTest::TearDownTestCase() {}
void RSUIFilterBaseTest::SetUp() {}
void RSUIFilterBaseTest::TearDown() {}

/**
 * @tc.name: CreateNGBlurFilter
 * @tc.desc: test for RSNGFilterHelper::CreateNGBlurFilter
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterBaseTest, CreateNGBlurFilter, TestSize.Level1)
{
    auto blurFilter = std::static_pointer_cast<RSNGBlurFilter>(
        RSNGFilterHelper::CreateNGBlurFilter(10.f, 10.f));
    EXPECT_NE(blurFilter, nullptr);
    float blurRadiusX = blurFilter->Getter<BlurRadiusXTag>()->Get();
    float blurRadiusY = blurFilter->Getter<BlurRadiusYTag>()->Get();
    EXPECT_FLOAT_EQ(blurRadiusX, 10.f);
    EXPECT_FLOAT_EQ(blurRadiusY, 10.f);
}

/**
 * @tc.name: CreateNGBlurFilterWithExpandDrawRegion
 * @tc.desc: test for RSNGFilterHelper::CreateNGBlurFilter with expandDrawRegion
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterBaseTest, CreateNGBlurFilterWithExpandDrawRegion, TestSize.Level1)
{
    auto blurFilter = std::static_pointer_cast<RSNGBlurFilter>(
        RSNGFilterHelper::CreateNGBlurFilter(20.f, 20.f, false));
    EXPECT_NE(blurFilter, nullptr);
    float blurRadiusX = blurFilter->Getter<BlurRadiusXTag>()->Get();
    float blurRadiusY = blurFilter->Getter<BlurRadiusYTag>()->Get();
    bool expandDrawRegion = blurFilter->Getter<BlurExpandDrawRegionTag>()->Get();
    EXPECT_FLOAT_EQ(blurRadiusX, 20.f);
    EXPECT_FLOAT_EQ(blurRadiusY, 20.f);
    EXPECT_EQ(expandDrawRegion, false);
}

/**
 * @tc.name: CreateNGBlurFilterExpandDrawRegionDefault
 * @tc.desc: test for expandDrawRegion default value is false
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterBaseTest, CreateNGBlurFilterExpandDrawRegionDefault, TestSize.Level1)
{
    auto blurFilter = std::static_pointer_cast<RSNGBlurFilter>(
        RSNGFilterHelper::CreateNGBlurFilter(10.f, 10.f));
    EXPECT_NE(blurFilter, nullptr);
    bool expandDrawRegion = blurFilter->Getter<BlurExpandDrawRegionTag>()->Get();
    EXPECT_EQ(expandDrawRegion, false);
}

/**
 * @tc.name: CreateNGMaterialBlurFilter
 * @tc.desc: test for RSNGFilterHelper::CreateNGMaterialBlurFilter
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterBaseTest, CreateNGMaterialBlurFilter, TestSize.Level1)
{
    MaterialParam materialParam = { -50, -0.5, -0.5, Color(0x9fff0000), false };
    auto materialBlurFilter = std::static_pointer_cast<RSNGMaterialBlurFilter>(
        RSNGFilterHelper::CreateNGMaterialBlurFilter(materialParam));
    EXPECT_NE(materialBlurFilter, nullptr);
    float radius = materialBlurFilter->Getter<MaterialBlurRadiusTag>()->Get();
    float saturation = materialBlurFilter->Getter<MaterialBlurSaturationTag>()->Get();
    float brightness = materialBlurFilter->Getter<MaterialBlurBrightnessTag>()->Get();
    EXPECT_FLOAT_EQ(radius, -50.f);
    EXPECT_FLOAT_EQ(saturation, -0.5);
    EXPECT_FLOAT_EQ(brightness, -0.5);
}

/**
 * @tc.name: CreateHeatDistortionFilter001
 * @tc.desc: test creating HEAT_DISTORTION filter via RSNGFilterBase::Create
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterBaseTest, CreateHeatDistortionFilter001, TestSize.Level1)
{
    auto heatDistortionFilter = RSNGFilterBase::Create(RSNGEffectType::HEAT_DISTORTION);
    EXPECT_NE(heatDistortionFilter, nullptr);
    EXPECT_EQ(heatDistortionFilter->GetType(), RSNGEffectType::HEAT_DISTORTION);
}

/**
 * @tc.name: CreateBlurBubblesRiseFilter001
 * @tc.desc: test creating BLUR_BUBBLES_RISE filter via RSNGFilterBase::Create
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterBaseTest, CreateBlurBubblesRiseFilter001, TestSize.Level1)
{
    auto blurBubblesRiseFilter = RSNGFilterBase::Create(RSNGEffectType::BLUR_BUBBLES_RISE);
    EXPECT_NE(blurBubblesRiseFilter, nullptr);
    EXPECT_EQ(blurBubblesRiseFilter->GetType(), RSNGEffectType::BLUR_BUBBLES_RISE);
}

/**
 * @tc.name: CreateHeatDistortionFilterFromPara001
 * @tc.desc: test creating HEAT_DISTORTION filter from FilterPara
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterBaseTest, CreateHeatDistortionFilterFromPara001, TestSize.Level1)
{
    auto heatDistortionPara = std::make_shared<HeatDistortionPara>();
    heatDistortionPara->SetIntensity(0.8f);
    heatDistortionPara->SetNoiseScale(1.5f);
    heatDistortionPara->SetRiseWeight(0.7f);
    heatDistortionPara->SetProgress(0.5f);

    auto heatDistortionFilter = RSNGFilterBase::Create(heatDistortionPara);
    EXPECT_NE(heatDistortionFilter, nullptr);
    EXPECT_EQ(heatDistortionFilter->GetType(), RSNGEffectType::HEAT_DISTORTION);
}

/**
 * @tc.name: CreateHeatDistortionFilterFromPara002
 * @tc.desc: test creating HEAT_DISTORTION filter with nullptr FilterPara
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterBaseTest, CreateHeatDistortionFilterFromPara002, TestSize.Level1)
{
    std::shared_ptr<FilterPara> nullptrPara = nullptr;
    auto heatDistortionFilter = RSNGFilterBase::Create(nullptrPara);
    EXPECT_EQ(heatDistortionFilter, nullptr);
}

/**
 * @tc.name: CreateBlurBubblesRiseFilterFromPara001
 * @tc.desc: test creating BLUR_BUBBLES_RISE filter from FilterPara
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterBaseTest, CreateBlurBubblesRiseFilterFromPara001, TestSize.Level1)
{
    auto blurBubblesRisePara = std::make_shared<BlurBubblesRisePara>();
    blurBubblesRisePara->SetBlurIntensity(0.6f);
    blurBubblesRisePara->SetMixStrength(1.0f);
    blurBubblesRisePara->SetProgress(0.3f);

    auto blurBubblesRiseFilter = RSNGFilterBase::Create(blurBubblesRisePara);
    EXPECT_NE(blurBubblesRiseFilter, nullptr);
    EXPECT_EQ(blurBubblesRiseFilter->GetType(), RSNGEffectType::BLUR_BUBBLES_RISE);
}

/**
 * @tc.name: CreateBlurBubblesRiseFilterFromPara002
 * @tc.desc: test creating BLUR_BUBBLES_RISE filter with nullptr FilterPara
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterBaseTest, CreateBlurBubblesRiseFilterFromPara002, TestSize.Level1)
{
    std::shared_ptr<FilterPara> nullptrPara = nullptr;
    auto blurBubblesRiseFilter = RSNGFilterBase::Create(nullptrPara);
    EXPECT_EQ(blurBubblesRiseFilter, nullptr);
}

/**
 * @tc.name: HeatDistortionFilterParameterVerification
 * @tc.desc: test that HeatDistortionPara parameters are correctly set
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterBaseTest, HeatDistortionFilterParameterVerification, TestSize.Level1)
{
    auto heatDistortionPara = std::make_shared<HeatDistortionPara>();
    float testIntensity = 0.9f;
    float testNoiseScale = 2.0f;
    float testRiseWeight = 0.8f;
    float testProgress = 0.6f;

    heatDistortionPara->SetIntensity(testIntensity);
    heatDistortionPara->SetNoiseScale(testNoiseScale);
    heatDistortionPara->SetRiseWeight(testRiseWeight);
    heatDistortionPara->SetProgress(testProgress);

    auto heatDistortionFilter = RSNGFilterBase::Create(heatDistortionPara);
    EXPECT_NE(heatDistortionFilter, nullptr);

    auto retrievedIntensity = heatDistortionPara->GetIntensity();
    auto retrievedNoiseScale = heatDistortionPara->GetNoiseScale();
    auto retrievedRiseWeight = heatDistortionPara->GetRiseWeight();
    auto retrievedProgress = heatDistortionPara->GetProgress();

    EXPECT_FLOAT_EQ(retrievedIntensity, testIntensity);
    EXPECT_FLOAT_EQ(retrievedNoiseScale, testNoiseScale);
    EXPECT_FLOAT_EQ(retrievedRiseWeight, testRiseWeight);
    EXPECT_FLOAT_EQ(retrievedProgress, testProgress);
}

/**
 * @tc.name: BlurBubblesRiseFilterParameterVerification
 * @tc.desc: test that BlurBubblesRisePara parameters are correctly set
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterBaseTest, BlurBubblesRiseFilterParameterVerification, TestSize.Level1)
{
    auto blurBubblesRisePara = std::make_shared<BlurBubblesRisePara>();
    float testBlurIntensity = 0.7f;
    float testMixStrength = 0.9f;
    float testProgress = 0.4f;

    blurBubblesRisePara->SetBlurIntensity(testBlurIntensity);
    blurBubblesRisePara->SetMixStrength(testMixStrength);
    blurBubblesRisePara->SetProgress(testProgress);

    auto blurBubblesRiseFilter = RSNGFilterBase::Create(blurBubblesRisePara);
    EXPECT_NE(blurBubblesRiseFilter, nullptr);

    auto retrievedBlurIntensity = blurBubblesRisePara->GetBlurIntensity();
    auto retrievedMixStrength = blurBubblesRisePara->GetMixStrength();
    auto retrievedProgress = blurBubblesRisePara->GetProgress();

    EXPECT_FLOAT_EQ(retrievedBlurIntensity, testBlurIntensity);
    EXPECT_FLOAT_EQ(retrievedMixStrength, testMixStrength);
    EXPECT_FLOAT_EQ(retrievedProgress, testProgress);
}

/**
 * @tc.name: HeatDistortionParaDefaultValues
 * @tc.desc: test HeatDistortionPara default values
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterBaseTest, HeatDistortionParaDefaultValues, TestSize.Level1)
{
    auto heatDistortionPara = std::make_shared<HeatDistortionPara>();

    EXPECT_FLOAT_EQ(heatDistortionPara->GetIntensity(), 1.0f);
    EXPECT_FLOAT_EQ(heatDistortionPara->GetNoiseScale(), 1.0f);
    EXPECT_FLOAT_EQ(heatDistortionPara->GetRiseWeight(), 0.2f);
    EXPECT_FLOAT_EQ(heatDistortionPara->GetProgress(), 0.0f);

    EXPECT_EQ(heatDistortionPara->GetParaType(), FilterPara::ParaType::HEAT_DISTORTION);
}

/**
 * @tc.name: BlurBubblesRiseParaDefaultValues
 * @tc.desc: test BlurBubblesRisePara default values
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterBaseTest, BlurBubblesRiseParaDefaultValues, TestSize.Level1)
{
    auto blurBubblesRisePara = std::make_shared<BlurBubblesRisePara>();

    EXPECT_FLOAT_EQ(blurBubblesRisePara->GetBlurIntensity(), 0.3f);
    EXPECT_FLOAT_EQ(blurBubblesRisePara->GetMixStrength(), 1.0f);
    EXPECT_FLOAT_EQ(blurBubblesRisePara->GetProgress(), 0.0f);
    EXPECT_EQ(blurBubblesRisePara->GetMaskImage(), nullptr);

    EXPECT_EQ(blurBubblesRisePara->GetParaType(), FilterPara::ParaType::BLUR_BUBBLES_RISE);
}

/**
 * @tc.name: CreateMotionBlurFilter001
 * @tc.desc: test creating MOTION_BLUR filter via RSNGFilterBase::Create
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterBaseTest, CreateMotionBlurFilter001, TestSize.Level1)
{
    auto motionBlurFilter = RSNGFilterBase::Create(RSNGEffectType::MOTION_BLUR);
    EXPECT_NE(motionBlurFilter, nullptr);
    EXPECT_EQ(motionBlurFilter->GetType(), RSNGEffectType::MOTION_BLUR);
}

/**
 * @tc.name: CreateMotionBlurFilterFromPara001
 * @tc.desc: test creating MOTION_BLUR filter from MotionBlurPara
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterBaseTest, CreateMotionBlurFilterFromPara001, TestSize.Level1)
{
    auto motionBlurPara = std::make_shared<MotionBlurPara>();
    motionBlurPara->SetRadius(20.0f);
    motionBlurPara->SetSampleCount(16);

    auto motionBlurFilter = RSNGFilterBase::Create(motionBlurPara);
    EXPECT_NE(motionBlurFilter, nullptr);
    EXPECT_EQ(motionBlurFilter->GetType(), RSNGEffectType::MOTION_BLUR);
}

/**
 * @tc.name: MotionBlurParaDefaultValues
 * @tc.desc: test MotionBlurPara default values
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterBaseTest, MotionBlurParaDefaultValues, TestSize.Level1)
{
    auto motionBlurPara = std::make_shared<MotionBlurPara>();

    EXPECT_FLOAT_EQ(motionBlurPara->GetRadius(), 0.0f);
    EXPECT_EQ(motionBlurPara->GetAnchor(), Vector2f(0.5f, 0.5f));
    EXPECT_EQ(motionBlurPara->GetSampleCount(), 8);
    EXPECT_EQ(motionBlurPara->GetParaType(), FilterPara::ParaType::MOTION_BLUR);
}

/**
 * @tc.name: MotionBlurParaParameterVerification
 * @tc.desc: test that MotionBlurPara parameters are correctly set
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterBaseTest, MotionBlurParaParameterVerification, TestSize.Level1)
{
    auto motionBlurPara = std::make_shared<MotionBlurPara>();
    float testRadius = 25.0f;
    Vector2f testAnchor = Vector2f(0.3f, 0.7f);
    int32_t testSampleCount = 32;

    motionBlurPara->SetRadius(testRadius);
    motionBlurPara->SetAnchor(testAnchor);
    motionBlurPara->SetSampleCount(testSampleCount);

    EXPECT_FLOAT_EQ(motionBlurPara->GetRadius(), testRadius);
    EXPECT_EQ(motionBlurPara->GetAnchor(), testAnchor);
    EXPECT_EQ(motionBlurPara->GetSampleCount(), testSampleCount);
}

/**
 * @tc.name: CreateMotionBlurFilterFromPara002
 * @tc.desc: test creating MOTION_BLUR filter with nullptr MotionBlurPara
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterBaseTest, CreateMotionBlurFilterFromPara002, TestSize.Level1)
{
    std::shared_ptr<MotionBlurPara> nullptrPara = nullptr;
    auto motionBlurFilter = RSNGFilterBase::Create(nullptrPara);
    EXPECT_EQ(motionBlurFilter, nullptr);
}

/**
 * @tc.name: DistortionCollapseEffectParaDefaultValues
 * @tc.desc: test DistortionCollapseEffectPara default values
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterBaseTest, DistortionCollapseEffectParaDefaultValues, TestSize.Level1)
{
    auto distortionPara = std::make_shared<DistortionCollapseEffectPara>();
    EXPECT_EQ(distortionPara->GetParaType(), VisualEffectPara::ParaType::DISTORTION_COLLAPSE_EFFECT);
    EXPECT_EQ(distortionPara->GetLUCorner(), Vector2f(0.f, 0.f));
    EXPECT_EQ(distortionPara->GetRUCorner(), Vector2f(0.f, 0.f));
    EXPECT_EQ(distortionPara->GetLBCorner(), Vector2f(0.f, 0.f));
    EXPECT_EQ(distortionPara->GetRBCorner(), Vector2f(0.f, 0.f));
    EXPECT_EQ(distortionPara->GetBarrelDistortion(), Vector4f(0.f, 0.f, 0.f, 0.f));
}

/**
 * @tc.name: DistortionCollapseEffectParaSettersAndGetters
 * @tc.desc: test DistortionCollapseEffectPara setters and getters
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterBaseTest, DistortionCollapseEffectParaSettersAndGetters, TestSize.Level1)
{
    auto distortionPara = std::make_shared<DistortionCollapseEffectPara>();
    Vector2f luCorner(0.1f, 0.2f);
    Vector2f ruCorner(0.3f, 0.4f);
    Vector2f lbCorner(0.5f, 0.6f);
    Vector2f rbCorner(0.7f, 0.8f);
    Vector4f barrelDistortion(0.1f, 0.2f, 0.3f, 0.4f);

    distortionPara->SetLUCorner(luCorner);
    distortionPara->SetRUCorner(ruCorner);
    distortionPara->SetLBCorner(lbCorner);
    distortionPara->SetRBCorner(rbCorner);
    distortionPara->SetBarrelDistortion(barrelDistortion);

    EXPECT_EQ(distortionPara->GetLUCorner(), luCorner);
    EXPECT_EQ(distortionPara->GetRUCorner(), ruCorner);
    EXPECT_EQ(distortionPara->GetLBCorner(), lbCorner);
    EXPECT_EQ(distortionPara->GetRBCorner(), rbCorner);
    EXPECT_EQ(distortionPara->GetBarrelDistortion(), barrelDistortion);
}

/**
 * @tc.name: CreateNGDistortionCollapseFilterNullptr
 * @tc.desc: test CreateNGDistortionCollapseFilter with nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterBaseTest, CreateNGDistortionCollapseFilterNullptr, TestSize.Level1)
{
    auto filter = RSNGFilterHelper::CreateNGDistortionCollapseFilter(nullptr);
    EXPECT_EQ(filter, nullptr);

    auto filter2 = RSNGFilterHelper::CreateNGDistortionCollapseFilter(nullptr);
    EXPECT_EQ(filter2, nullptr);
    EXPECT_EQ(filter, filter2);
}

/**
 * @tc.name: CreateNGDistortionCollapseFilterWithParams
 * @tc.desc: test CreateNGDistortionCollapseFilter with custom params
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterBaseTest, CreateNGDistortionCollapseFilterWithParams, TestSize.Level1)
{
    auto distortionPara = std::make_shared<DistortionCollapseEffectPara>();
    Vector2f luCorner(0.1f, 0.2f);
    Vector2f ruCorner(0.3f, 0.4f);
    Vector2f lbCorner(0.5f, 0.6f);
    Vector2f rbCorner(0.7f, 0.8f);
    Vector4f barrelDistortion(0.1f, 0.2f, 0.3f, 0.4f);

    distortionPara->SetLUCorner(luCorner);
    distortionPara->SetRUCorner(ruCorner);
    distortionPara->SetLBCorner(lbCorner);
    distortionPara->SetRBCorner(rbCorner);
    distortionPara->SetBarrelDistortion(barrelDistortion);

    auto filter = RSNGFilterHelper::CreateNGDistortionCollapseFilter(distortionPara);
    ASSERT_NE(filter, nullptr);
    ASSERT_EQ(filter->GetType(), RSNGEffectType::DISTORTION_COLLAPSE);

    auto distortFilter = std::static_pointer_cast<RSNGDistortionCollapseFilter>(filter);
    EXPECT_EQ(distortFilter->Getter<DistortionCollapseLUCornerTag>()->Get(), luCorner);
    EXPECT_EQ(distortFilter->Getter<DistortionCollapseRUCornerTag>()->Get(), ruCorner);
    EXPECT_EQ(distortFilter->Getter<DistortionCollapseLBCornerTag>()->Get(), lbCorner);
    EXPECT_EQ(distortFilter->Getter<DistortionCollapseRBCornerTag>()->Get(), rbCorner);
    EXPECT_EQ(distortFilter->Getter<DistortionCollapseBarrelDistortionTag>()->Get(), barrelDistortion);
}

/**
 * @tc.name: CreateNGDistortionCollapseFilterDefaultValues
 * @tc.desc: test CreateNGDistortionCollapseFilter with default values
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterBaseTest, CreateNGDistortionCollapseFilterDefaultValues, TestSize.Level1)
{
    auto distortionPara = std::make_shared<DistortionCollapseEffectPara>();
    EXPECT_EQ(distortionPara->GetLUCorner(), Vector2f(0.f, 0.f));
    EXPECT_EQ(distortionPara->GetRUCorner(), Vector2f(0.f, 0.f));
    EXPECT_EQ(distortionPara->GetLBCorner(), Vector2f(0.f, 0.f));
    EXPECT_EQ(distortionPara->GetRBCorner(), Vector2f(0.f, 0.f));
    EXPECT_EQ(distortionPara->GetBarrelDistortion(), Vector4f(0.f, 0.f, 0.f, 0.f));

    auto filter = RSNGFilterHelper::CreateNGDistortionCollapseFilter(distortionPara);
    ASSERT_NE(filter, nullptr);
    ASSERT_EQ(filter->GetType(), RSNGEffectType::DISTORTION_COLLAPSE);

    auto distortFilter = std::static_pointer_cast<RSNGDistortionCollapseFilter>(filter);
    EXPECT_EQ(distortFilter->Getter<DistortionCollapseLUCornerTag>()->Get(), Vector2f(0.f, 0.f));
    EXPECT_EQ(distortFilter->Getter<DistortionCollapseRUCornerTag>()->Get(), Vector2f(0.f, 0.f));
    EXPECT_EQ(distortFilter->Getter<DistortionCollapseLBCornerTag>()->Get(), Vector2f(0.f, 0.f));
    EXPECT_EQ(distortFilter->Getter<DistortionCollapseRBCornerTag>()->Get(), Vector2f(0.f, 0.f));
    EXPECT_EQ(distortFilter->Getter<DistortionCollapseBarrelDistortionTag>()->Get(), Vector4f(0.f, 0.f, 0.f, 0.f));
}

/**
 * @tc.name: CreateNGSDFDistortOpShapeNullptr
 * @tc.desc: test CreateNGSDFDistortOpShape with nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterBaseTest, CreateNGSDFDistortOpShapeNullptr, TestSize.Level1)
{
    auto shape = RSNGFilterHelper::CreateNGSDFDistortOpShape(nullptr);
    EXPECT_EQ(shape, nullptr);

    auto shape2 = RSNGFilterHelper::CreateNGSDFDistortOpShape(nullptr);
    EXPECT_EQ(shape2, nullptr);
    EXPECT_EQ(shape, shape2);
}

/**
 * @tc.name: CreateNGSDFDistortOpShapeWithParams
 * @tc.desc: test CreateNGSDFDistortOpShape with custom params
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterBaseTest, CreateNGSDFDistortOpShapeWithParams, TestSize.Level1)
{
    auto distortionPara = std::make_shared<DistortionCollapseEffectPara>();
    Vector2f luCorner(0.1f, 0.2f);
    Vector2f ruCorner(0.3f, 0.4f);
    Vector2f lbCorner(0.5f, 0.6f);
    Vector2f rbCorner(0.7f, 0.8f);
    Vector4f barrelDistortion(0.1f, 0.2f, 0.3f, 0.4f);

    distortionPara->SetLUCorner(luCorner);
    distortionPara->SetRUCorner(ruCorner);
    distortionPara->SetLBCorner(lbCorner);
    distortionPara->SetRBCorner(rbCorner);
    distortionPara->SetBarrelDistortion(barrelDistortion);

    auto shape = RSNGFilterHelper::CreateNGSDFDistortOpShape(distortionPara);
    ASSERT_NE(shape, nullptr);
    ASSERT_EQ(shape->GetType(), RSNGEffectType::SDF_DISTORT_OP_SHAPE);

    auto distortShape = std::static_pointer_cast<RSNGSDFDistortOpShape>(shape);
    EXPECT_EQ(distortShape->Getter<SDFDistortOpShapeLUCornerTag>()->Get(), luCorner);
    EXPECT_EQ(distortShape->Getter<SDFDistortOpShapeRUCornerTag>()->Get(), ruCorner);
    EXPECT_EQ(distortShape->Getter<SDFDistortOpShapeLBCornerTag>()->Get(), lbCorner);
    EXPECT_EQ(distortShape->Getter<SDFDistortOpShapeRBCornerTag>()->Get(), rbCorner);
    EXPECT_EQ(distortShape->Getter<SDFDistortOpShapeBarrelDistortionTag>()->Get(), barrelDistortion);

    EXPECT_EQ(distortShape->Getter<SDFDistortOpShapeShapeTag>()->Get(), nullptr);
    EXPECT_EQ(distortShape->Getter<SDFDistortOpShapeSyncTag>()->Get(), false);
}

/**
 * @tc.name: CreateNGSDFDistortOpShapeDefaultValues
 * @tc.desc: test CreateNGSDFDistortOpShape with default values
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterBaseTest, CreateNGSDFDistortOpShapeDefaultValues, TestSize.Level1)
{
    auto distortionPara = std::make_shared<DistortionCollapseEffectPara>();
    EXPECT_EQ(distortionPara->GetLUCorner(), Vector2f(0.f, 0.f));
    EXPECT_EQ(distortionPara->GetRUCorner(), Vector2f(0.f, 0.f));
    EXPECT_EQ(distortionPara->GetLBCorner(), Vector2f(0.f, 0.f));
    EXPECT_EQ(distortionPara->GetRBCorner(), Vector2f(0.f, 0.f));
    EXPECT_EQ(distortionPara->GetBarrelDistortion(), Vector4f(0.f, 0.f, 0.f, 0.f));

    auto shape = RSNGFilterHelper::CreateNGSDFDistortOpShape(distortionPara);
    ASSERT_NE(shape, nullptr);
    ASSERT_EQ(shape->GetType(), RSNGEffectType::SDF_DISTORT_OP_SHAPE);

    auto distortShape = std::static_pointer_cast<RSNGSDFDistortOpShape>(shape);
    EXPECT_EQ(distortShape->Getter<SDFDistortOpShapeLUCornerTag>()->Get(), Vector2f(0.f, 0.f));
    EXPECT_EQ(distortShape->Getter<SDFDistortOpShapeRUCornerTag>()->Get(), Vector2f(0.f, 0.f));
    EXPECT_EQ(distortShape->Getter<SDFDistortOpShapeLBCornerTag>()->Get(), Vector2f(0.f, 0.f));
    EXPECT_EQ(distortShape->Getter<SDFDistortOpShapeRBCornerTag>()->Get(), Vector2f(0.f, 0.f));
    EXPECT_EQ(distortShape->Getter<SDFDistortOpShapeBarrelDistortionTag>()->Get(), Vector4f(0.f, 0.f, 0.f, 0.f));

    EXPECT_EQ(distortShape->Getter<SDFDistortOpShapeShapeTag>()->Get(), nullptr);
    EXPECT_EQ(distortShape->Getter<SDFDistortOpShapeSyncTag>()->Get(), false);
}

HWTEST_F(RSUIFilterBaseTest, CreateNGDistortionWithDisableFlag, TestSize.Level1)
{
    auto distortionPara = std::make_shared<DistortionCollapseEffectPara>();
    distortionPara->SetDisabled(true);
    auto filter = RSNGFilterHelper::CreateNGDistortionCollapseFilter(distortionPara);
    auto shape = RSNGFilterHelper::CreateNGSDFDistortOpShape(distortionPara);
    EXPECT_EQ(filter, nullptr);
    EXPECT_EQ(shape, nullptr);
}

} // namespace OHOS::Rosen
