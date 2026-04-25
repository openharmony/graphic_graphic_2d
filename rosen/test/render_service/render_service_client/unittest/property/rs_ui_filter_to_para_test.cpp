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
#include "ui_effect/property/include/rs_ui_filter_to_para.h"
#include "ui_effect/property/include/rs_ui_filter_base.h"
#include "ui_effect/property/include/rs_ui_mask_base.h"
#include "ui_effect/filter/include/filter_para.h"
#include "ui_effect/mask/include/mask_para.h"
#include "ui_effect/filter/include/filter_frosted_glass_blur_para.h"
#include "ui_effect/filter/include/filter_frosted_glass_para.h"
#include "common/rs_vector2.h"
#include "common/rs_vector3.h"
#include "common/rs_vector4.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSUIFilterToParaTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUIFilterToParaTest::SetUpTestCase() {}
void RSUIFilterToParaTest::TearDownTestCase() {}
void RSUIFilterToParaTest::SetUp() {}
void RSUIFilterToParaTest::TearDown() {}

/**
 * @tc.name: ConvertFilterToParaNullptrInput
 * @tc.desc: test RSNGFilterToParaHelper::ConvertFilterToPara with nullptr input
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterToParaTest, ConvertFilterToParaNullptrInput, TestSize.Level1)
{
    std::shared_ptr<RSNGFilterBase> nullFilter = nullptr;
    auto result = RSNGFilterToParaHelper::ConvertFilterToPara(nullFilter);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: ConvertFilterToParaFrostedGlassBlurFilter
 * @tc.desc: test RSNGFilterToParaHelper::ConvertFilterToPara with FROSTED_GLASS_BLUR filter
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterToParaTest, ConvertFilterToParaFrostedGlassBlurFilter, TestSize.Level1)
{
    auto filter = RSNGFilterBase::Create(RSNGEffectType::FROSTED_GLASS_BLUR);
    EXPECT_NE(filter, nullptr);

    auto frostedGlassBlurFilter = std::static_pointer_cast<RSNGFrostedGlassBlurFilter>(filter);
    constexpr float testRadius = 5.0f;
    constexpr float testRadiusScaleK = 1.5f;
    constexpr float testRefractOutPx = 10.0f;
    constexpr bool testSkipFrameEnable = true;

    frostedGlassBlurFilter->Setter<FrostedGlassBlurRadiusTag>(testRadius);
    frostedGlassBlurFilter->Setter<FrostedGlassBlurRadiusScaleKTag>(testRadiusScaleK);
    frostedGlassBlurFilter->Setter<FrostedGlassBlurRefractOutPxTag>(testRefractOutPx);
    frostedGlassBlurFilter->Setter<FrostedGlassBlurSkipFrameEnableTag>(testSkipFrameEnable);

    auto result = RSNGFilterToParaHelper::ConvertFilterToPara(filter);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->GetParaType(), FilterPara::ParaType::FROSTED_GLASS_BLUR);

    auto para = std::static_pointer_cast<FrostedGlassBlurPara>(result);
    EXPECT_FLOAT_EQ(para->GetBlurRadius(), testRadius);
    EXPECT_FLOAT_EQ(para->GetBlurRadiusScaleK(), testRadiusScaleK);
    EXPECT_FLOAT_EQ(para->GetRefractOutPx(), testRefractOutPx);
    EXPECT_EQ(para->GetSkipFrameEnable(), testSkipFrameEnable);
}

/**
 * @tc.name: ConvertFilterToParaFrostedGlassBlurFilterDefaultValues
 * @tc.desc: test RSNGFilterToParaHelper::ConvertFilterToPara with default values
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterToParaTest, ConvertFilterToParaFrostedGlassBlurFilterDefaultValues, TestSize.Level1)
{
    auto filter = RSNGFilterBase::Create(RSNGEffectType::FROSTED_GLASS_BLUR);
    EXPECT_NE(filter, nullptr);

    auto result = RSNGFilterToParaHelper::ConvertFilterToPara(filter);
    EXPECT_NE(result, nullptr);

    auto para = std::static_pointer_cast<FrostedGlassBlurPara>(result);
    EXPECT_FLOAT_EQ(para->GetBlurRadius(), 0.0f);
    EXPECT_FLOAT_EQ(para->GetBlurRadiusScaleK(), 0.0f);
    EXPECT_FLOAT_EQ(para->GetRefractOutPx(), 0.0f);
    EXPECT_EQ(para->GetSkipFrameEnable(), false);
}

/**
 * @tc.name: ConvertFilterToParaFrostedGlassFilter
 * @tc.desc: test RSNGFilterToParaHelper::ConvertFilterToPara with FROSTED_GLASS filter
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterToParaTest, ConvertFilterToParaFrostedGlassFilter, TestSize.Level1)
{
    auto filter = RSNGFilterBase::Create(RSNGEffectType::FROSTED_GLASS);
    EXPECT_NE(filter, nullptr);

    auto frostedGlassFilter = std::static_pointer_cast<RSNGFrostedGlassFilter>(filter);
    Vector2f blurParams = { 48.0f, 4.0f };
    Vector2f weightsEmboss = { 1.0f, 1.0f };
    Vector2f weightsEdl = { 0.5f, 0.5f };
    Vector2f bgRates = { -0.00003f, 1.2f };
    Vector3f bgKBS = { 0.010834f, 0.007349f, 1.2f };
    Vector3f bgPos = { 0.3f, 0.5f, 1.0f };
    Vector3f bgNeg = { 0.5f, 0.5f, 1.0f };
    constexpr float bgAlpha = 1.0f;
    Vector3f refractParams = { 1.0f, 2.0f, 3.0f };
    Vector3f sdParams = { 0.1f, 0.2f, 0.3f };
    Vector2f sdRates = { 0.5f, 0.6f };
    Vector3f sdKBS = { 0.7f, 0.8f, 0.9f };
    Vector3f sdPos = { 0.4f, 0.5f, 0.6f };
    Vector3f sdNeg = { 0.1f, 0.2f, 0.3f };
    Vector3f envLightParams = { 0.1f, 0.2f, 0.3f };
    Vector2f envLightRates = { 0.4f, 0.5f };
    Vector3f envLightKBS = { 0.6f, 0.7f, 0.8f };
    Vector3f envLightPos = { 0.9f, 1.0f, 1.1f };
    Vector3f envLightNeg = { 0.1f, 0.2f, 0.3f };
    Vector2f edLightParams = { 0.1f, 0.2f };
    Vector2f edLightAngles = { 30.0f, 45.0f };
    Vector2f edLightDir = { 1.0f, 0.0f };
    Vector2f edLightRates = { 0.5f, 0.6f };
    Vector3f edLightKBS = { 0.7f, 0.8f, 0.9f };
    Vector3f edLightPos = { 0.4f, 0.5f, 0.6f };
    Vector3f edLightNeg = { 0.1f, 0.2f, 0.3f };
    constexpr bool baseVibrancyEnabled = true;
    constexpr float baseMaterialType = 1.0f;
    Vector4f materialColor = { 0.5f, 0.5f, 0.5f, 1.0f };
    constexpr float samplingScale = 1.5f;
    constexpr bool skipFrameEnable = true;

    frostedGlassFilter->Setter<FrostedGlassBlurParamsTag>(blurParams);
    frostedGlassFilter->Setter<FrostedGlassWeightsEmbossTag>(weightsEmboss);
    frostedGlassFilter->Setter<FrostedGlassWeightsEdlTag>(weightsEdl);
    frostedGlassFilter->Setter<FrostedGlassBgRatesTag>(bgRates);
    frostedGlassFilter->Setter<FrostedGlassBgKBSTag>(bgKBS);
    frostedGlassFilter->Setter<FrostedGlassBgPosTag>(bgPos);
    frostedGlassFilter->Setter<FrostedGlassBgNegTag>(bgNeg);
    frostedGlassFilter->Setter<FrostedGlassBgAlphaTag>(bgAlpha);
    frostedGlassFilter->Setter<FrostedGlassRefractParamsTag>(refractParams);
    frostedGlassFilter->Setter<FrostedGlassSdParamsTag>(sdParams);
    frostedGlassFilter->Setter<FrostedGlassSdRatesTag>(sdRates);
    frostedGlassFilter->Setter<FrostedGlassSdKBSTag>(sdKBS);
    frostedGlassFilter->Setter<FrostedGlassSdPosTag>(sdPos);
    frostedGlassFilter->Setter<FrostedGlassSdNegTag>(sdNeg);
    frostedGlassFilter->Setter<FrostedGlassEnvLightParamsTag>(envLightParams);
    frostedGlassFilter->Setter<FrostedGlassEnvLightRatesTag>(envLightRates);
    frostedGlassFilter->Setter<FrostedGlassEnvLightKBSTag>(envLightKBS);
    frostedGlassFilter->Setter<FrostedGlassEnvLightPosTag>(envLightPos);
    frostedGlassFilter->Setter<FrostedGlassEnvLightNegTag>(envLightNeg);
    frostedGlassFilter->Setter<FrostedGlassEdLightParamsTag>(edLightParams);
    frostedGlassFilter->Setter<FrostedGlassEdLightAnglesTag>(edLightAngles);
    frostedGlassFilter->Setter<FrostedGlassEdLightDirTag>(edLightDir);
    frostedGlassFilter->Setter<FrostedGlassEdLightRatesTag>(edLightRates);
    frostedGlassFilter->Setter<FrostedGlassEdLightKBSTag>(edLightKBS);
    frostedGlassFilter->Setter<FrostedGlassEdLightPosTag>(edLightPos);
    frostedGlassFilter->Setter<FrostedGlassEdLightNegTag>(edLightNeg);
    frostedGlassFilter->Setter<FrostedGlassBaseVibrancyEnabledTag>(baseVibrancyEnabled);
    frostedGlassFilter->Setter<FrostedGlassBaseMaterialTypeTag>(baseMaterialType);
    frostedGlassFilter->Setter<FrostedGlassMaterialColorTag>(materialColor);
    frostedGlassFilter->Setter<FrostedGlassSamplingScaleTag>(samplingScale);
    frostedGlassFilter->Setter<FrostedGlassWaveMaskTag>(std::shared_ptr<RSNGMaskBase>(nullptr));
    frostedGlassFilter->Setter<FrostedGlassSkipFrameEnableTag>(skipFrameEnable);

    auto result = RSNGFilterToParaHelper::ConvertFilterToPara(filter);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->GetParaType(), FilterPara::ParaType::FROSTED_GLASS);

    auto para = std::static_pointer_cast<FrostedGlassPara>(result);
    EXPECT_FLOAT_EQ(para->GetBlurParams().x_, blurParams.x_);
    EXPECT_FLOAT_EQ(para->GetBlurParams().y_, blurParams.y_);
    EXPECT_FLOAT_EQ(para->GetWeightsEmboss().x_, weightsEmboss.x_);
    EXPECT_FLOAT_EQ(para->GetWeightsEmboss().y_, weightsEmboss.y_);
    EXPECT_FLOAT_EQ(para->GetWeightsEdl().x_, weightsEdl.x_);
    EXPECT_FLOAT_EQ(para->GetWeightsEdl().y_, weightsEdl.y_);
    EXPECT_FLOAT_EQ(para->GetBgRates().x_, bgRates.x_);
    EXPECT_FLOAT_EQ(para->GetBgRates().y_, bgRates.y_);
    EXPECT_FLOAT_EQ(para->GetBgKBS().x_, bgKBS.x_);
    EXPECT_FLOAT_EQ(para->GetBgKBS().y_, bgKBS.y_);
    EXPECT_FLOAT_EQ(para->GetBgKBS().z_, bgKBS.z_);
    EXPECT_FLOAT_EQ(para->GetBgPos().x_, bgPos.x_);
    EXPECT_FLOAT_EQ(para->GetBgPos().y_, bgPos.y_);
    EXPECT_FLOAT_EQ(para->GetBgPos().z_, bgPos.z_);
    EXPECT_FLOAT_EQ(para->GetBgNeg().x_, bgNeg.x_);
    EXPECT_FLOAT_EQ(para->GetBgNeg().y_, bgNeg.y_);
    EXPECT_FLOAT_EQ(para->GetBgNeg().z_, bgNeg.z_);
    EXPECT_FLOAT_EQ(para->GetBgAlpha(), bgAlpha);
    EXPECT_FLOAT_EQ(para->GetRefractParams().x_, refractParams.x_);
    EXPECT_FLOAT_EQ(para->GetRefractParams().y_, refractParams.y_);
    EXPECT_FLOAT_EQ(para->GetRefractParams().z_, refractParams.z_);
    EXPECT_FLOAT_EQ(para->GetSdParams().x_, sdParams.x_);
    EXPECT_FLOAT_EQ(para->GetSdParams().y_, sdParams.y_);
    EXPECT_FLOAT_EQ(para->GetSdParams().z_, sdParams.z_);
    EXPECT_FLOAT_EQ(para->GetSdRates().x_, sdRates.x_);
    EXPECT_FLOAT_EQ(para->GetSdRates().y_, sdRates.y_);
    EXPECT_FLOAT_EQ(para->GetSdKBS().x_, sdKBS.x_);
    EXPECT_FLOAT_EQ(para->GetSdKBS().y_, sdKBS.y_);
    EXPECT_FLOAT_EQ(para->GetSdKBS().z_, sdKBS.z_);
    EXPECT_FLOAT_EQ(para->GetSdPos().x_, sdPos.x_);
    EXPECT_FLOAT_EQ(para->GetSdPos().y_, sdPos.y_);
    EXPECT_FLOAT_EQ(para->GetSdPos().z_, sdPos.z_);
    EXPECT_FLOAT_EQ(para->GetSdNeg().x_, sdNeg.x_);
    EXPECT_FLOAT_EQ(para->GetSdNeg().y_, sdNeg.y_);
    EXPECT_FLOAT_EQ(para->GetSdNeg().z_, sdNeg.z_);
    EXPECT_FLOAT_EQ(para->GetEnvLightParams().x_, envLightParams.x_);
    EXPECT_FLOAT_EQ(para->GetEnvLightParams().y_, envLightParams.y_);
    EXPECT_FLOAT_EQ(para->GetEnvLightParams().z_, envLightParams.z_);
    EXPECT_FLOAT_EQ(para->GetEnvLightRates().x_, envLightRates.x_);
    EXPECT_FLOAT_EQ(para->GetEnvLightRates().y_, envLightRates.y_);
    EXPECT_FLOAT_EQ(para->GetEnvLightKBS().x_, envLightKBS.x_);
    EXPECT_FLOAT_EQ(para->GetEnvLightKBS().y_, envLightKBS.y_);
    EXPECT_FLOAT_EQ(para->GetEnvLightKBS().z_, envLightKBS.z_);
    EXPECT_FLOAT_EQ(para->GetEnvLightPos().x_, envLightPos.x_);
    EXPECT_FLOAT_EQ(para->GetEnvLightPos().y_, envLightPos.y_);
    EXPECT_FLOAT_EQ(para->GetEnvLightPos().z_, envLightPos.z_);
    EXPECT_FLOAT_EQ(para->GetEnvLightNeg().x_, envLightNeg.x_);
    EXPECT_FLOAT_EQ(para->GetEnvLightNeg().y_, envLightNeg.y_);
    EXPECT_FLOAT_EQ(para->GetEnvLightNeg().z_, envLightNeg.z_);
    EXPECT_FLOAT_EQ(para->GetEdLightParams().x_, edLightParams.x_);
    EXPECT_FLOAT_EQ(para->GetEdLightParams().y_, edLightParams.y_);
    EXPECT_FLOAT_EQ(para->GetEdLightAngles().x_, edLightAngles.x_);
    EXPECT_FLOAT_EQ(para->GetEdLightAngles().y_, edLightAngles.y_);
    EXPECT_FLOAT_EQ(para->GetEdLightDir().x_, edLightDir.x_);
    EXPECT_FLOAT_EQ(para->GetEdLightDir().y_, edLightDir.y_);
    EXPECT_FLOAT_EQ(para->GetEdLightRates().x_, edLightRates.x_);
    EXPECT_FLOAT_EQ(para->GetEdLightRates().y_, edLightRates.y_);
    EXPECT_FLOAT_EQ(para->GetEdLightKBS().x_, edLightKBS.x_);
    EXPECT_FLOAT_EQ(para->GetEdLightKBS().y_, edLightKBS.y_);
    EXPECT_FLOAT_EQ(para->GetEdLightKBS().z_, edLightKBS.z_);
    EXPECT_FLOAT_EQ(para->GetEdLightPos().x_, edLightPos.x_);
    EXPECT_FLOAT_EQ(para->GetEdLightPos().y_, edLightPos.y_);
    EXPECT_FLOAT_EQ(para->GetEdLightPos().z_, edLightPos.z_);
    EXPECT_FLOAT_EQ(para->GetEdLightNeg().x_, edLightNeg.x_);
    EXPECT_FLOAT_EQ(para->GetEdLightNeg().y_, edLightNeg.y_);
    EXPECT_FLOAT_EQ(para->GetEdLightNeg().z_, edLightNeg.z_);
    EXPECT_EQ(para->GetBaseVibrancyEnabled(), baseVibrancyEnabled);
    EXPECT_FLOAT_EQ(para->GetBaseMaterialType(), baseMaterialType);
    EXPECT_FLOAT_EQ(para->GetMaterialColor().x_, materialColor.x_);
    EXPECT_FLOAT_EQ(para->GetMaterialColor().y_, materialColor.y_);
    EXPECT_FLOAT_EQ(para->GetMaterialColor().z_, materialColor.z_);
    EXPECT_FLOAT_EQ(para->GetMaterialColor().w_, materialColor.w_);
    EXPECT_FLOAT_EQ(para->GetSamplingScale(), samplingScale);
    EXPECT_EQ(para->GetMask(), nullptr);
    EXPECT_EQ(para->GetSkipFrameEnable(), skipFrameEnable);
}

/**
 * @tc.name: ConvertFilterToParaFrostedGlassFilterWithDarkModeParams
 * @tc.desc: test RSNGFilterToParaHelper::ConvertFilterToPara with dark mode params
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterToParaTest, ConvertFilterToParaFrostedGlassFilterWithDarkModeParams, TestSize.Level1)
{
    auto filter = RSNGFilterBase::Create(RSNGEffectType::FROSTED_GLASS);
    EXPECT_NE(filter, nullptr);

    auto frostedGlassFilter = std::static_pointer_cast<RSNGFrostedGlassFilter>(filter);
    Vector2f darkModeBlurParams = { 50.0f, 30.0f };
    Vector2f darkModeWeightsEmboss = { 1.0f, 2.0f };
    Vector2f darkModeBgRates = { 0.1f, 0.2f };
    Vector3f darkModeBgKBS = { 0.3f, 0.4f, 0.5f };
    Vector3f darkModeBgPos = { 0.6f, 0.7f, 0.8f };
    Vector3f darkModeBgNeg = { 0.9f, 1.0f, 1.1f };

    frostedGlassFilter->Setter<FrostedGlassDarkModeBlurParamsTag>(darkModeBlurParams);
    frostedGlassFilter->Setter<FrostedGlassDarkModeWeightsEmbossTag>(darkModeWeightsEmboss);
    frostedGlassFilter->Setter<FrostedGlassDarkModeBgRatesTag>(darkModeBgRates);
    frostedGlassFilter->Setter<FrostedGlassDarkModeBgKBSTag>(darkModeBgKBS);
    frostedGlassFilter->Setter<FrostedGlassDarkModeBgPosTag>(darkModeBgPos);
    frostedGlassFilter->Setter<FrostedGlassDarkModeBgNegTag>(darkModeBgNeg);

    auto result = RSNGFilterToParaHelper::ConvertFilterToPara(filter);
    EXPECT_NE(result, nullptr);

    auto para = std::static_pointer_cast<FrostedGlassPara>(result);
    EXPECT_NE(para->GetDarkAdaptiveParams(), nullptr);
    EXPECT_FLOAT_EQ(para->GetDarkAdaptiveParams()->blurParams.x_, darkModeBlurParams.x_);
    EXPECT_FLOAT_EQ(para->GetDarkAdaptiveParams()->blurParams.y_, darkModeBlurParams.y_);
    EXPECT_FLOAT_EQ(para->GetDarkAdaptiveParams()->weightsEmboss.x_, darkModeWeightsEmboss.x_);
    EXPECT_FLOAT_EQ(para->GetDarkAdaptiveParams()->weightsEmboss.y_, darkModeWeightsEmboss.y_);
    EXPECT_FLOAT_EQ(para->GetDarkAdaptiveParams()->bgRates.x_, darkModeBgRates.x_);
    EXPECT_FLOAT_EQ(para->GetDarkAdaptiveParams()->bgRates.y_, darkModeBgRates.y_);
    EXPECT_FLOAT_EQ(para->GetDarkAdaptiveParams()->bgKBS.x_, darkModeBgKBS.x_);
    EXPECT_FLOAT_EQ(para->GetDarkAdaptiveParams()->bgKBS.y_, darkModeBgKBS.y_);
    EXPECT_FLOAT_EQ(para->GetDarkAdaptiveParams()->bgKBS.z_, darkModeBgKBS.z_);
    EXPECT_FLOAT_EQ(para->GetDarkAdaptiveParams()->bgPos.x_, darkModeBgPos.x_);
    EXPECT_FLOAT_EQ(para->GetDarkAdaptiveParams()->bgPos.y_, darkModeBgPos.y_);
    EXPECT_FLOAT_EQ(para->GetDarkAdaptiveParams()->bgPos.z_, darkModeBgPos.z_);
    EXPECT_FLOAT_EQ(para->GetDarkAdaptiveParams()->bgNeg.x_, darkModeBgNeg.x_);
    EXPECT_FLOAT_EQ(para->GetDarkAdaptiveParams()->bgNeg.y_, darkModeBgNeg.y_);
    EXPECT_FLOAT_EQ(para->GetDarkAdaptiveParams()->bgNeg.z_, darkModeBgNeg.z_);
}

/**
 * @tc.name: ConvertFilterToParaFrostedGlassFilterNoDarkModeParams
 * @tc.desc: test RSNGFilterToParaHelper::ConvertFilterToPara without dark mode params
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterToParaTest, ConvertFilterToParaFrostedGlassFilterNoDarkModeParams, TestSize.Level1)
{
    auto filter = RSNGFilterBase::Create(RSNGEffectType::FROSTED_GLASS);
    EXPECT_NE(filter, nullptr);

    auto frostedGlassFilter = std::static_pointer_cast<RSNGFrostedGlassFilter>(filter);
    Vector2f darkModeBlurParams = { 0.0f, 0.0f };
    frostedGlassFilter->Setter<FrostedGlassDarkModeBlurParamsTag>(darkModeBlurParams);

    auto result = RSNGFilterToParaHelper::ConvertFilterToPara(filter);
    EXPECT_NE(result, nullptr);

    auto para = std::static_pointer_cast<FrostedGlassPara>(result);
    EXPECT_EQ(para->GetDarkAdaptiveParams(), nullptr);
}

/**
 * @tc.name: ConvertFilterToParaFrostedGlassFilterDarkModeOnlyXPositive
 * @tc.desc: test with dark mode params where only x is positive
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterToParaTest, ConvertFilterToParaFrostedGlassFilterDarkModeOnlyXPositive, TestSize.Level1)
{
    auto filter = RSNGFilterBase::Create(RSNGEffectType::FROSTED_GLASS);
    EXPECT_NE(filter, nullptr);

    auto frostedGlassFilter = std::static_pointer_cast<RSNGFrostedGlassFilter>(filter);
    Vector2f darkModeBlurParams = { 50.0f, 0.0f };
    frostedGlassFilter->Setter<FrostedGlassDarkModeBlurParamsTag>(darkModeBlurParams);

    auto result = RSNGFilterToParaHelper::ConvertFilterToPara(filter);
    EXPECT_NE(result, nullptr);

    auto para = std::static_pointer_cast<FrostedGlassPara>(result);
    EXPECT_NE(para->GetDarkAdaptiveParams(), nullptr);
}

/**
 * @tc.name: ConvertFilterToParaFrostedGlassFilterDarkModeOnlyYPositive
 * @tc.desc: test with dark mode params where only y is positive
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterToParaTest, ConvertFilterToParaFrostedGlassFilterDarkModeOnlyYPositive, TestSize.Level1)
{
    auto filter = RSNGFilterBase::Create(RSNGEffectType::FROSTED_GLASS);
    EXPECT_NE(filter, nullptr);

    auto frostedGlassFilter = std::static_pointer_cast<RSNGFrostedGlassFilter>(filter);
    Vector2f darkModeBlurParams = { 0.0f, 30.0f };
    frostedGlassFilter->Setter<FrostedGlassDarkModeBlurParamsTag>(darkModeBlurParams);

    auto result = RSNGFilterToParaHelper::ConvertFilterToPara(filter);
    EXPECT_NE(result, nullptr);

    auto para = std::static_pointer_cast<FrostedGlassPara>(result);
    EXPECT_NE(para->GetDarkAdaptiveParams(), nullptr);
}

/**
 * @tc.name: ConvertFilterToParaUnsupportedType
 * @tc.desc: test RSNGFilterToParaHelper::ConvertFilterToPara with unsupported type
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterToParaTest, ConvertFilterToParaUnsupportedType, TestSize.Level1)
{
    auto blurFilter = RSNGFilterBase::Create(RSNGEffectType::BLUR);
    EXPECT_NE(blurFilter, nullptr);

    auto result = RSNGFilterToParaHelper::ConvertFilterToPara(blurFilter);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: ConvertFilterToParaHeatDistortionType
 * @tc.desc: test RSNGFilterToParaHelper::ConvertFilterToPara with HEAT_DISTORTION type
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterToParaTest, ConvertFilterToParaHeatDistortionType, TestSize.Level1)
{
    auto heatDistortionFilter = RSNGFilterBase::Create(RSNGEffectType::HEAT_DISTORTION);
    EXPECT_NE(heatDistortionFilter, nullptr);

    auto result = RSNGFilterToParaHelper::ConvertFilterToPara(heatDistortionFilter);
    EXPECT_EQ(result, nullptr);
}

class FrostedGlassBlurParaTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void FrostedGlassBlurParaTest::SetUpTestCase() {}
void FrostedGlassBlurParaTest::TearDownTestCase() {}
void FrostedGlassBlurParaTest::SetUp() {}
void FrostedGlassBlurParaTest::TearDown() {}

/**
 * @tc.name: SetSkipFrameEnable001
 * @tc.desc: test FrostedGlassBlurPara::SetSkipFrameEnable with true
 * @tc.type: FUNC
 */
HWTEST_F(FrostedGlassBlurParaTest, SetSkipFrameEnable001, TestSize.Level1)
{
    auto para = std::make_shared<FrostedGlassBlurPara>();
    EXPECT_EQ(para->GetSkipFrameEnable(), false);

    para->SetSkipFrameEnable(true);
    EXPECT_EQ(para->GetSkipFrameEnable(), true);
}

/**
 * @tc.name: SetSkipFrameEnable002
 * @tc.desc: test FrostedGlassBlurPara::SetSkipFrameEnable with false
 * @tc.type: FUNC
 */
HWTEST_F(FrostedGlassBlurParaTest, SetSkipFrameEnable002, TestSize.Level1)
{
    auto para = std::make_shared<FrostedGlassBlurPara>();
    para->SetSkipFrameEnable(true);
    EXPECT_EQ(para->GetSkipFrameEnable(), true);

    para->SetSkipFrameEnable(false);
    EXPECT_EQ(para->GetSkipFrameEnable(), false);
}

/**
 * @tc.name: SetSkipFrameEnableToggle
 * @tc.desc: test FrostedGlassBlurPara::SetSkipFrameEnable toggle multiple times
 * @tc.type: FUNC
 */
HWTEST_F(FrostedGlassBlurParaTest, SetSkipFrameEnableToggle, TestSize.Level1)
{
    auto para = std::make_shared<FrostedGlassBlurPara>();

    para->SetSkipFrameEnable(true);
    EXPECT_EQ(para->GetSkipFrameEnable(), true);

    para->SetSkipFrameEnable(false);
    EXPECT_EQ(para->GetSkipFrameEnable(), false);

    para->SetSkipFrameEnable(true);
    EXPECT_EQ(para->GetSkipFrameEnable(), true);

    para->SetSkipFrameEnable(false);
    EXPECT_EQ(para->GetSkipFrameEnable(), false);
}

/**
 * @tc.name: FrostedGlassBlurParaDefaultValues
 * @tc.desc: test FrostedGlassBlurPara default values
 * @tc.type: FUNC
 */
HWTEST_F(FrostedGlassBlurParaTest, FrostedGlassBlurParaDefaultValues, TestSize.Level1)
{
    auto para = std::make_shared<FrostedGlassBlurPara>();
    EXPECT_FLOAT_EQ(para->GetBlurRadius(), 0.0f);
    EXPECT_FLOAT_EQ(para->GetBlurRadiusScaleK(), 0.0f);
    EXPECT_FLOAT_EQ(para->GetRefractOutPx(), 0.0f);
    EXPECT_EQ(para->GetSkipFrameEnable(), false);
    EXPECT_EQ(para->GetParaType(), FilterPara::ParaType::FROSTED_GLASS_BLUR);
}

class FrostedGlassParaTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void FrostedGlassParaTest::SetUpTestCase() {}
void FrostedGlassParaTest::TearDownTestCase() {}
void FrostedGlassParaTest::SetUp() {}
void FrostedGlassParaTest::TearDown() {}

/**
 * @tc.name: SetSkipFrameEnable001
 * @tc.desc: test FrostedGlassPara::SetSkipFrameEnable with true
 * @tc.type: FUNC
 */
HWTEST_F(FrostedGlassParaTest, SetSkipFrameEnable001, TestSize.Level1)
{
    auto para = std::make_shared<FrostedGlassPara>();
    EXPECT_EQ(para->GetSkipFrameEnable(), false);

    para->SetSkipFrameEnable(true);
    EXPECT_EQ(para->GetSkipFrameEnable(), true);
}

/**
 * @tc.name: SetSkipFrameEnable002
 * @tc.desc: test FrostedGlassPara::SetSkipFrameEnable with false
 * @tc.type: FUNC
 */
HWTEST_F(FrostedGlassParaTest, SetSkipFrameEnable002, TestSize.Level1)
{
    auto para = std::make_shared<FrostedGlassPara>();
    para->SetSkipFrameEnable(true);
    EXPECT_EQ(para->GetSkipFrameEnable(), true);

    para->SetSkipFrameEnable(false);
    EXPECT_EQ(para->GetSkipFrameEnable(), false);
}

/**
 * @tc.name: SetSkipFrameEnableToggle
 * @tc.desc: test FrostedGlassPara::SetSkipFrameEnable toggle multiple times
 * @tc.type: FUNC
 */
HWTEST_F(FrostedGlassParaTest, SetSkipFrameEnableToggle, TestSize.Level1)
{
    auto para = std::make_shared<FrostedGlassPara>();

    para->SetSkipFrameEnable(true);
    EXPECT_EQ(para->GetSkipFrameEnable(), true);

    para->SetSkipFrameEnable(false);
    EXPECT_EQ(para->GetSkipFrameEnable(), false);

    para->SetSkipFrameEnable(true);
    EXPECT_EQ(para->GetSkipFrameEnable(), true);

    para->SetSkipFrameEnable(false);
    EXPECT_EQ(para->GetSkipFrameEnable(), false);
}

/**
 * @tc.name: FrostedGlassParaDefaultValues
 * @tc.desc: test FrostedGlassPara default values
 * @tc.type: FUNC
 */
HWTEST_F(FrostedGlassParaTest, FrostedGlassParaDefaultValues, TestSize.Level1)
{
    auto para = std::make_shared<FrostedGlassPara>();
    EXPECT_EQ(para->GetSkipFrameEnable(), false);
    EXPECT_EQ(para->GetParaType(), FilterPara::ParaType::FROSTED_GLASS);
}

} // namespace OHOS::Rosen