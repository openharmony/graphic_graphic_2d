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

#include "common/rs_vector2.h"
#include "common/rs_vector3.h"
#include "common/rs_vector4.h"
#include "ui_effect/filter/include/filter_frosted_glass_blur_para.h"
#include "ui_effect/filter/include/filter_frosted_glass_para.h"
#include "ui_effect/filter/include/filter_para.h"
#include "ui_effect/mask/include/mask_para.h"
#include "ui_effect/property/include/rs_ui_filter_base.h"
#include "ui_effect/property/include/rs_ui_filter_to_para.h"
#include "ui_effect/property/include/rs_ui_mask_base.h"

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

namespace {
const Vector2f FROSTED_GLASS_BLUR_PARAMS = { 48.0f, 4.0f };
const Vector2f FROSTED_GLASS_WEIGHTS_EMBOSS = { 1.0f, 1.0f };
const Vector2f FROSTED_GLASS_WEIGHTS_EDL = { 0.5f, 0.5f };
const Vector2f FROSTED_GLASS_BG_RATES = { -0.00003f, 1.2f };
const Vector3f FROSTED_GLASS_BG_KBS = { 0.010834f, 0.007349f, 1.2f };
const Vector3f FROSTED_GLASS_BG_POS = { 0.3f, 0.5f, 1.0f };
const Vector3f FROSTED_GLASS_BG_NEG = { 0.5f, 0.5f, 1.0f };
constexpr float FROSTED_GLASS_BG_ALPHA = 1.0f;
const Vector3f FROSTED_GLASS_REFRACT_PARAMS = { 1.0f, 2.0f, 3.0f };
const Vector3f FROSTED_GLASS_SD_PARAMS = { 0.1f, 0.2f, 0.3f };
const Vector2f FROSTED_GLASS_SD_RATES = { 0.5f, 0.6f };
const Vector3f FROSTED_GLASS_SD_KBS = { 0.7f, 0.8f, 0.9f };
const Vector3f FROSTED_GLASS_SD_POS = { 0.4f, 0.5f, 0.6f };
const Vector3f FROSTED_GLASS_SD_NEG = { 0.1f, 0.2f, 0.3f };
const Vector3f FROSTED_GLASS_ENV_LIGHT_PARAMS = { 0.1f, 0.2f, 0.3f };
const Vector2f FROSTED_GLASS_ENV_LIGHT_RATES = { 0.4f, 0.5f };
const Vector3f FROSTED_GLASS_ENV_LIGHT_KBS = { 0.6f, 0.7f, 0.8f };
const Vector3f FROSTED_GLASS_ENV_LIGHT_POS = { 0.9f, 1.0f, 1.1f };
const Vector3f FROSTED_GLASS_ENV_LIGHT_NEG = { 0.1f, 0.2f, 0.3f };
const Vector2f FROSTED_GLASS_ED_LIGHT_PARAMS = { 0.1f, 0.2f };
const Vector2f FROSTED_GLASS_ED_LIGHT_ANGLES = { 30.0f, 45.0f };
const Vector2f FROSTED_GLASS_ED_LIGHT_DIR = { 1.0f, 0.0f };
const Vector2f FROSTED_GLASS_ED_LIGHT_RATES = { 0.5f, 0.6f };
const Vector3f FROSTED_GLASS_ED_LIGHT_KBS = { 0.7f, 0.8f, 0.9f };
const Vector3f FROSTED_GLASS_ED_LIGHT_POS = { 0.4f, 0.5f, 0.6f };
const Vector3f FROSTED_GLASS_ED_LIGHT_NEG = { 0.1f, 0.2f, 0.3f };
constexpr bool FROSTED_GLASS_BASE_VIBRANCY_ENABLED = true;
constexpr float FROSTED_GLASS_BASE_MATERIAL_TYPE = 1.0f;
const Vector4f FROSTED_GLASS_MATERIAL_COLOR = { 0.5f, 0.5f, 0.5f, 1.0f };
constexpr float FROSTED_GLASS_SAMPLING_SCALE = 1.5f;
constexpr bool FROSTED_GLASS_SKIP_FRAME_ENABLE = true;

void SetupFrostedGlassFilter(RSNGFrostedGlassFilter& filter)
{
    filter.Setter<FrostedGlassBlurParamsTag>(FROSTED_GLASS_BLUR_PARAMS);
    filter.Setter<FrostedGlassWeightsEmbossTag>(FROSTED_GLASS_WEIGHTS_EMBOSS);
    filter.Setter<FrostedGlassWeightsEdlTag>(FROSTED_GLASS_WEIGHTS_EDL);
    filter.Setter<FrostedGlassBgRatesTag>(FROSTED_GLASS_BG_RATES);
    filter.Setter<FrostedGlassBgKBSTag>(FROSTED_GLASS_BG_KBS);
    filter.Setter<FrostedGlassBgPosTag>(FROSTED_GLASS_BG_POS);
    filter.Setter<FrostedGlassBgNegTag>(FROSTED_GLASS_BG_NEG);
    filter.Setter<FrostedGlassBgAlphaTag>(FROSTED_GLASS_BG_ALPHA);
    filter.Setter<FrostedGlassRefractParamsTag>(FROSTED_GLASS_REFRACT_PARAMS);
    filter.Setter<FrostedGlassSdParamsTag>(FROSTED_GLASS_SD_PARAMS);
    filter.Setter<FrostedGlassSdRatesTag>(FROSTED_GLASS_SD_RATES);
    filter.Setter<FrostedGlassSdKBSTag>(FROSTED_GLASS_SD_KBS);
    filter.Setter<FrostedGlassSdPosTag>(FROSTED_GLASS_SD_POS);
    filter.Setter<FrostedGlassSdNegTag>(FROSTED_GLASS_SD_NEG);
    filter.Setter<FrostedGlassEnvLightParamsTag>(FROSTED_GLASS_ENV_LIGHT_PARAMS);
    filter.Setter<FrostedGlassEnvLightRatesTag>(FROSTED_GLASS_ENV_LIGHT_RATES);
    filter.Setter<FrostedGlassEnvLightKBSTag>(FROSTED_GLASS_ENV_LIGHT_KBS);
    filter.Setter<FrostedGlassEnvLightPosTag>(FROSTED_GLASS_ENV_LIGHT_POS);
    filter.Setter<FrostedGlassEnvLightNegTag>(FROSTED_GLASS_ENV_LIGHT_NEG);
    filter.Setter<FrostedGlassEdLightParamsTag>(FROSTED_GLASS_ED_LIGHT_PARAMS);
    filter.Setter<FrostedGlassEdLightAnglesTag>(FROSTED_GLASS_ED_LIGHT_ANGLES);
    filter.Setter<FrostedGlassEdLightDirTag>(FROSTED_GLASS_ED_LIGHT_DIR);
    filter.Setter<FrostedGlassEdLightRatesTag>(FROSTED_GLASS_ED_LIGHT_RATES);
    filter.Setter<FrostedGlassEdLightKBSTag>(FROSTED_GLASS_ED_LIGHT_KBS);
    filter.Setter<FrostedGlassEdLightPosTag>(FROSTED_GLASS_ED_LIGHT_POS);
    filter.Setter<FrostedGlassEdLightNegTag>(FROSTED_GLASS_ED_LIGHT_NEG);
    filter.Setter<FrostedGlassBaseVibrancyEnabledTag>(FROSTED_GLASS_BASE_VIBRANCY_ENABLED);
    filter.Setter<FrostedGlassBaseMaterialTypeTag>(FROSTED_GLASS_BASE_MATERIAL_TYPE);
    filter.Setter<FrostedGlassMaterialColorTag>(FROSTED_GLASS_MATERIAL_COLOR);
    filter.Setter<FrostedGlassSamplingScaleTag>(FROSTED_GLASS_SAMPLING_SCALE);
    filter.Setter<FrostedGlassWaveMaskTag>(std::shared_ptr<RSNGMaskBase>(nullptr));
    filter.Setter<FrostedGlassSkipFrameEnableTag>(FROSTED_GLASS_SKIP_FRAME_ENABLE);
}

void VerifyFrostedGlassParaBlurWeightsBgSdRefract(const FrostedGlassPara& para)
{
    EXPECT_FLOAT_EQ(para.GetBlurParams().x_, FROSTED_GLASS_BLUR_PARAMS.x_);
    EXPECT_FLOAT_EQ(para.GetBlurParams().y_, FROSTED_GLASS_BLUR_PARAMS.y_);
    EXPECT_FLOAT_EQ(para.GetWeightsEmboss().x_, FROSTED_GLASS_WEIGHTS_EMBOSS.x_);
    EXPECT_FLOAT_EQ(para.GetWeightsEmboss().y_, FROSTED_GLASS_WEIGHTS_EMBOSS.y_);
    EXPECT_FLOAT_EQ(para.GetWeightsEdl().x_, FROSTED_GLASS_WEIGHTS_EDL.x_);
    EXPECT_FLOAT_EQ(para.GetWeightsEdl().y_, FROSTED_GLASS_WEIGHTS_EDL.y_);
    EXPECT_FLOAT_EQ(para.GetBgRates().x_, FROSTED_GLASS_BG_RATES.x_);
    EXPECT_FLOAT_EQ(para.GetBgRates().y_, FROSTED_GLASS_BG_RATES.y_);
    EXPECT_FLOAT_EQ(para.GetBgKBS().x_, FROSTED_GLASS_BG_KBS.x_);
    EXPECT_FLOAT_EQ(para.GetBgKBS().y_, FROSTED_GLASS_BG_KBS.y_);
    EXPECT_FLOAT_EQ(para.GetBgKBS().z_, FROSTED_GLASS_BG_KBS.z_);
    EXPECT_FLOAT_EQ(para.GetBgPos().x_, FROSTED_GLASS_BG_POS.x_);
    EXPECT_FLOAT_EQ(para.GetBgPos().y_, FROSTED_GLASS_BG_POS.y_);
    EXPECT_FLOAT_EQ(para.GetBgPos().z_, FROSTED_GLASS_BG_POS.z_);
    EXPECT_FLOAT_EQ(para.GetBgNeg().x_, FROSTED_GLASS_BG_NEG.x_);
    EXPECT_FLOAT_EQ(para.GetBgNeg().y_, FROSTED_GLASS_BG_NEG.y_);
    EXPECT_FLOAT_EQ(para.GetBgNeg().z_, FROSTED_GLASS_BG_NEG.z_);
    EXPECT_FLOAT_EQ(para.GetBgAlpha(), FROSTED_GLASS_BG_ALPHA);
    EXPECT_FLOAT_EQ(para.GetRefractParams().x_, FROSTED_GLASS_REFRACT_PARAMS.x_);
    EXPECT_FLOAT_EQ(para.GetRefractParams().y_, FROSTED_GLASS_REFRACT_PARAMS.y_);
    EXPECT_FLOAT_EQ(para.GetRefractParams().z_, FROSTED_GLASS_REFRACT_PARAMS.z_);
    EXPECT_FLOAT_EQ(para.GetSdParams().x_, FROSTED_GLASS_SD_PARAMS.x_);
    EXPECT_FLOAT_EQ(para.GetSdParams().y_, FROSTED_GLASS_SD_PARAMS.y_);
    EXPECT_FLOAT_EQ(para.GetSdParams().z_, FROSTED_GLASS_SD_PARAMS.z_);
    EXPECT_FLOAT_EQ(para.GetSdRates().x_, FROSTED_GLASS_SD_RATES.x_);
    EXPECT_FLOAT_EQ(para.GetSdRates().y_, FROSTED_GLASS_SD_RATES.y_);
    EXPECT_FLOAT_EQ(para.GetSdKBS().x_, FROSTED_GLASS_SD_KBS.x_);
    EXPECT_FLOAT_EQ(para.GetSdKBS().y_, FROSTED_GLASS_SD_KBS.y_);
    EXPECT_FLOAT_EQ(para.GetSdKBS().z_, FROSTED_GLASS_SD_KBS.z_);
    EXPECT_FLOAT_EQ(para.GetSdPos().x_, FROSTED_GLASS_SD_POS.x_);
    EXPECT_FLOAT_EQ(para.GetSdPos().y_, FROSTED_GLASS_SD_POS.y_);
    EXPECT_FLOAT_EQ(para.GetSdPos().z_, FROSTED_GLASS_SD_POS.z_);
    EXPECT_FLOAT_EQ(para.GetSdNeg().x_, FROSTED_GLASS_SD_NEG.x_);
    EXPECT_FLOAT_EQ(para.GetSdNeg().y_, FROSTED_GLASS_SD_NEG.y_);
    EXPECT_FLOAT_EQ(para.GetSdNeg().z_, FROSTED_GLASS_SD_NEG.z_);
}

void VerifyFrostedGlassParaEnvLightEdLightBase(const FrostedGlassPara& para)
{
    EXPECT_FLOAT_EQ(para.GetEnvLightParams().x_, FROSTED_GLASS_ENV_LIGHT_PARAMS.x_);
    EXPECT_FLOAT_EQ(para.GetEnvLightParams().y_, FROSTED_GLASS_ENV_LIGHT_PARAMS.y_);
    EXPECT_FLOAT_EQ(para.GetEnvLightParams().z_, FROSTED_GLASS_ENV_LIGHT_PARAMS.z_);
    EXPECT_FLOAT_EQ(para.GetEnvLightRates().x_, FROSTED_GLASS_ENV_LIGHT_RATES.x_);
    EXPECT_FLOAT_EQ(para.GetEnvLightRates().y_, FROSTED_GLASS_ENV_LIGHT_RATES.y_);
    EXPECT_FLOAT_EQ(para.GetEnvLightKBS().x_, FROSTED_GLASS_ENV_LIGHT_KBS.x_);
    EXPECT_FLOAT_EQ(para.GetEnvLightKBS().y_, FROSTED_GLASS_ENV_LIGHT_KBS.y_);
    EXPECT_FLOAT_EQ(para.GetEnvLightKBS().z_, FROSTED_GLASS_ENV_LIGHT_KBS.z_);
    EXPECT_FLOAT_EQ(para.GetEnvLightPos().x_, FROSTED_GLASS_ENV_LIGHT_POS.x_);
    EXPECT_FLOAT_EQ(para.GetEnvLightPos().y_, FROSTED_GLASS_ENV_LIGHT_POS.y_);
    EXPECT_FLOAT_EQ(para.GetEnvLightPos().z_, FROSTED_GLASS_ENV_LIGHT_POS.z_);
    EXPECT_FLOAT_EQ(para.GetEnvLightNeg().x_, FROSTED_GLASS_ENV_LIGHT_NEG.x_);
    EXPECT_FLOAT_EQ(para.GetEnvLightNeg().y_, FROSTED_GLASS_ENV_LIGHT_NEG.y_);
    EXPECT_FLOAT_EQ(para.GetEnvLightNeg().z_, FROSTED_GLASS_ENV_LIGHT_NEG.z_);
    EXPECT_FLOAT_EQ(para.GetEdLightParams().x_, FROSTED_GLASS_ED_LIGHT_PARAMS.x_);
    EXPECT_FLOAT_EQ(para.GetEdLightParams().y_, FROSTED_GLASS_ED_LIGHT_PARAMS.y_);
    EXPECT_FLOAT_EQ(para.GetEdLightAngles().x_, FROSTED_GLASS_ED_LIGHT_ANGLES.x_);
    EXPECT_FLOAT_EQ(para.GetEdLightAngles().y_, FROSTED_GLASS_ED_LIGHT_ANGLES.y_);
    EXPECT_FLOAT_EQ(para.GetEdLightDir().x_, FROSTED_GLASS_ED_LIGHT_DIR.x_);
    EXPECT_FLOAT_EQ(para.GetEdLightDir().y_, FROSTED_GLASS_ED_LIGHT_DIR.y_);
    EXPECT_FLOAT_EQ(para.GetEdLightRates().x_, FROSTED_GLASS_ED_LIGHT_RATES.x_);
    EXPECT_FLOAT_EQ(para.GetEdLightRates().y_, FROSTED_GLASS_ED_LIGHT_RATES.y_);
    EXPECT_FLOAT_EQ(para.GetEdLightKBS().x_, FROSTED_GLASS_ED_LIGHT_KBS.x_);
    EXPECT_FLOAT_EQ(para.GetEdLightKBS().y_, FROSTED_GLASS_ED_LIGHT_KBS.y_);
    EXPECT_FLOAT_EQ(para.GetEdLightKBS().z_, FROSTED_GLASS_ED_LIGHT_KBS.z_);
    EXPECT_FLOAT_EQ(para.GetEdLightPos().x_, FROSTED_GLASS_ED_LIGHT_POS.x_);
    EXPECT_FLOAT_EQ(para.GetEdLightPos().y_, FROSTED_GLASS_ED_LIGHT_POS.y_);
    EXPECT_FLOAT_EQ(para.GetEdLightPos().z_, FROSTED_GLASS_ED_LIGHT_POS.z_);
    EXPECT_FLOAT_EQ(para.GetEdLightNeg().x_, FROSTED_GLASS_ED_LIGHT_NEG.x_);
    EXPECT_FLOAT_EQ(para.GetEdLightNeg().y_, FROSTED_GLASS_ED_LIGHT_NEG.y_);
    EXPECT_FLOAT_EQ(para.GetEdLightNeg().z_, FROSTED_GLASS_ED_LIGHT_NEG.z_);
    EXPECT_EQ(para.GetBaseVibrancyEnabled(), FROSTED_GLASS_BASE_VIBRANCY_ENABLED);
    EXPECT_FLOAT_EQ(para.GetBaseMaterialType(), FROSTED_GLASS_BASE_MATERIAL_TYPE);
    EXPECT_FLOAT_EQ(para.GetMaterialColor().x_, FROSTED_GLASS_MATERIAL_COLOR.x_);
    EXPECT_FLOAT_EQ(para.GetMaterialColor().y_, FROSTED_GLASS_MATERIAL_COLOR.y_);
    EXPECT_FLOAT_EQ(para.GetMaterialColor().z_, FROSTED_GLASS_MATERIAL_COLOR.z_);
    EXPECT_FLOAT_EQ(para.GetMaterialColor().w_, FROSTED_GLASS_MATERIAL_COLOR.w_);
    EXPECT_FLOAT_EQ(para.GetSamplingScale(), FROSTED_GLASS_SAMPLING_SCALE);
    EXPECT_EQ(para.GetMask(), nullptr);
    EXPECT_EQ(para.GetSkipFrameEnable(), FROSTED_GLASS_SKIP_FRAME_ENABLE);
}
} // namespace

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
    SetupFrostedGlassFilter(*frostedGlassFilter);
    auto result = RSNGFilterToParaHelper::ConvertFilterToPara(filter);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->GetParaType(), FilterPara::ParaType::FROSTED_GLASS);
    auto para = std::static_pointer_cast<FrostedGlassPara>(result);
    VerifyFrostedGlassParaBlurWeightsBgSdRefract(*para);
    VerifyFrostedGlassParaEnvLightEdLightBase(*para);
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

/**
 * @tc.name: FrostedGlassBlurParaSetSkipFrameEnableTrue
 * @tc.desc: test FrostedGlassBlurPara::SetSkipFrameEnable with true
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterToParaTest, FrostedGlassBlurParaSetSkipFrameEnableTrue, TestSize.Level1)
{
    auto para = std::make_shared<FrostedGlassBlurPara>();
    EXPECT_EQ(para->GetSkipFrameEnable(), false);

    para->SetSkipFrameEnable(true);
    EXPECT_EQ(para->GetSkipFrameEnable(), true);
}

/**
 * @tc.name: FrostedGlassBlurParaSetSkipFrameEnableFromTrueToFalse
 * @tc.desc: test FrostedGlassBlurPara::SetSkipFrameEnable with false
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterToParaTest, FrostedGlassBlurParaSetSkipFrameEnableFromTrueToFalse, TestSize.Level1)
{
    auto para = std::make_shared<FrostedGlassBlurPara>();
    para->SetSkipFrameEnable(true);
    EXPECT_EQ(para->GetSkipFrameEnable(), true);

    para->SetSkipFrameEnable(false);
    EXPECT_EQ(para->GetSkipFrameEnable(), false);
}

/**
 * @tc.name: FrostedGlassBlurParaSetSkipFrameEnableToggle
 * @tc.desc: test FrostedGlassBlurPara::SetSkipFrameEnable toggle multiple times
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterToParaTest, FrostedGlassBlurParaSetSkipFrameEnableToggle, TestSize.Level1)
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
HWTEST_F(RSUIFilterToParaTest, FrostedGlassBlurParaDefaultValues, TestSize.Level1)
{
    auto para = std::make_shared<FrostedGlassBlurPara>();
    EXPECT_FLOAT_EQ(para->GetBlurRadius(), 0.0f);
    EXPECT_FLOAT_EQ(para->GetBlurRadiusScaleK(), 0.0f);
    EXPECT_FLOAT_EQ(para->GetRefractOutPx(), 0.0f);
    EXPECT_EQ(para->GetSkipFrameEnable(), false);
    EXPECT_EQ(para->GetParaType(), FilterPara::ParaType::FROSTED_GLASS_BLUR);
}

/**
 * @tc.name: FrostedGlassParaSetSkipFrameEnableTrue
 * @tc.desc: test FrostedGlassPara::SetSkipFrameEnable with true
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterToParaTest, FrostedGlassParaSetSkipFrameEnableTrue, TestSize.Level1)
{
    auto para = std::make_shared<FrostedGlassPara>();
    EXPECT_EQ(para->GetSkipFrameEnable(), false);

    para->SetSkipFrameEnable(true);
    EXPECT_EQ(para->GetSkipFrameEnable(), true);
}

/**
 * @tc.name: FrostedGlassParaSetSkipFrameEnableFromTrueToFalse
 * @tc.desc: test FrostedGlassPara::SetSkipFrameEnable with false
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterToParaTest, FrostedGlassParaSetSkipFrameEnableFromTrueToFalse, TestSize.Level1)
{
    auto para = std::make_shared<FrostedGlassPara>();
    para->SetSkipFrameEnable(true);
    EXPECT_EQ(para->GetSkipFrameEnable(), true);

    para->SetSkipFrameEnable(false);
    EXPECT_EQ(para->GetSkipFrameEnable(), false);
}

/**
 * @tc.name: FrostedGlassParaSetSkipFrameEnableToggle
 * @tc.desc: test FrostedGlassPara::SetSkipFrameEnable toggle multiple times
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterToParaTest, FrostedGlassParaSetSkipFrameEnableToggle, TestSize.Level1)
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
HWTEST_F(RSUIFilterToParaTest, FrostedGlassParaDefaultValues, TestSize.Level1)
{
    auto para = std::make_shared<FrostedGlassPara>();
    EXPECT_EQ(para->GetSkipFrameEnable(), false);
    EXPECT_EQ(para->GetParaType(), FilterPara::ParaType::FROSTED_GLASS);
}

} // namespace OHOS::Rosen