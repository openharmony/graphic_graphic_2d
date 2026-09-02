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
#include "ui_effect/mask/include/binocular_mask_para.h"
#include "ui_effect/mask/include/fractal_glass_mask_para.h"
#include "ui_effect/mask/include/image_mask_para.h"
#include "ui_effect/mask/include/mask_para.h"
#include "ui_effect/mask/include/pixel_map_mask_para.h"
#include "ui_effect/mask/include/radial_gradient_mask_para.h"
#include "ui_effect/mask/include/ripple_mask_para.h"
#include "ui_effect/mask/include/sweep_refraction_mask_para.h"
#include "ui_effect/mask/include/use_effect_mask_para.h"
#include "ui_effect/mask/include/wave_disturbance_mask_para.h"
#include "ui_effect/mask/include/wave_gradient_mask_para.h"
#include "ui_effect/property/include/rs_ui_mask_base.h"
#include "ui_effect/property/include/rs_ui_mask_to_para.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSUIMaskToParaTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUIMaskToParaTest::SetUpTestCase() {}
void RSUIMaskToParaTest::TearDownTestCase() {}
void RSUIMaskToParaTest::SetUp() {}
void RSUIMaskToParaTest::TearDown() {}

/**
 * @tc.name: ConvertMaskToParaNullptrInput
 * @tc.desc: test RSNGMaskToParaHelper::ConvertMaskToPara with nullptr input
 * @tc.type: FUNC
 */
HWTEST_F(RSUIMaskToParaTest, ConvertMaskToParaNullptrInput, TestSize.Level1)
{
    std::shared_ptr<RSNGMaskBase> nullMask = nullptr;
    auto result = RSNGMaskToParaHelper::ConvertMaskToPara(nullMask);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: ConvertMaskToParaRippleMask
 * @tc.desc: test RSNGMaskToParaHelper::ConvertMaskToPara with RIPPLE_MASK
 * @tc.type: FUNC
 */
HWTEST_F(RSUIMaskToParaTest, ConvertMaskToParaRippleMask, TestSize.Level1)
{
    auto mask = RSNGMaskBase::Create(RSNGEffectType::RIPPLE_MASK);
    EXPECT_NE(mask, nullptr);

    auto rippleMask = std::static_pointer_cast<RSNGRippleMask>(mask);
    Vector2f center = { 0.5f, 0.5f };
    constexpr float radius = 1.0f;
    constexpr float width = 0.5f;
    constexpr float offset = 0.2f;

    rippleMask->Setter<RippleMaskCenterTag>(center);
    rippleMask->Setter<RippleMaskRadiusTag>(radius);
    rippleMask->Setter<RippleMaskWidthTag>(width);
    rippleMask->Setter<RippleMaskOffsetTag>(offset);

    auto result = RSNGMaskToParaHelper::ConvertMaskToPara(mask);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->GetMaskParaType(), MaskPara::Type::RIPPLE_MASK);

    auto para = std::static_pointer_cast<RippleMaskPara>(result);
    EXPECT_FLOAT_EQ(para->GetCenter().x_, center.x_);
    EXPECT_FLOAT_EQ(para->GetCenter().y_, center.y_);
    EXPECT_FLOAT_EQ(para->GetRadius(), radius);
    EXPECT_FLOAT_EQ(para->GetWidth(), width);
    EXPECT_FLOAT_EQ(para->GetWidthCenterOffset(), offset);
}

/**
 * @tc.name: ConvertMaskToParaRadialGradientMask
 * @tc.desc: test RSNGMaskToParaHelper::ConvertMaskToPara with RADIAL_GRADIENT_MASK
 * @tc.type: FUNC
 */
HWTEST_F(RSUIMaskToParaTest, ConvertMaskToParaRadialGradientMask, TestSize.Level1)
{
    auto mask = RSNGMaskBase::Create(RSNGEffectType::RADIAL_GRADIENT_MASK);
    EXPECT_NE(mask, nullptr);

    auto radialGradientMask = std::static_pointer_cast<RSNGRadialGradientMask>(mask);
    Vector2f center = { 0.5f, 0.5f };
    constexpr float radiusX = 1.0f;
    constexpr float radiusY = 0.8f;
    std::vector<float> colors = { 0.0f, 0.5f, 1.0f };
    std::vector<float> positions = { 0.0f, 0.5f, 1.0f };

    radialGradientMask->Setter<RadialGradientMaskCenterTag>(center);
    radialGradientMask->Setter<RadialGradientMaskRadiusXTag>(radiusX);
    radialGradientMask->Setter<RadialGradientMaskRadiusYTag>(radiusY);
    radialGradientMask->Setter<RadialGradientMaskColorsTag>(colors);
    radialGradientMask->Setter<RadialGradientMaskPositionsTag>(positions);

    auto result = RSNGMaskToParaHelper::ConvertMaskToPara(mask);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->GetMaskParaType(), MaskPara::Type::RADIAL_GRADIENT_MASK);

    auto para = std::static_pointer_cast<RadialGradientMaskPara>(result);
    EXPECT_FLOAT_EQ(para->GetCenter().x_, center.x_);
    EXPECT_FLOAT_EQ(para->GetCenter().y_, center.y_);
    EXPECT_FLOAT_EQ(para->GetRadiusX(), radiusX);
    EXPECT_FLOAT_EQ(para->GetRadiusY(), radiusY);
    EXPECT_EQ(para->GetColors().size(), colors.size());
    EXPECT_EQ(para->GetPositions().size(), positions.size());
}

/**
 * @tc.name: ConvertMaskToParaWaveGradientMask
 * @tc.desc: test RSNGMaskToParaHelper::ConvertMaskToPara with WAVE_GRADIENT_MASK
 * @tc.type: FUNC
 */
HWTEST_F(RSUIMaskToParaTest, ConvertMaskToParaWaveGradientMask, TestSize.Level1)
{
    auto mask = RSNGMaskBase::Create(RSNGEffectType::WAVE_GRADIENT_MASK);
    EXPECT_NE(mask, nullptr);

    auto waveGradientMask = std::static_pointer_cast<RSNGWaveGradientMask>(mask);
    Vector2f waveCenter = { 0.5f, 0.5f };
    constexpr float waveWidth = 1.0f;
    constexpr float propagationRadius = 2.0f;
    constexpr float blurRadius = 0.5f;
    constexpr float turbulenceStrength = 0.3f;

    waveGradientMask->Setter<WaveGradientMaskWaveCenterTag>(waveCenter);
    waveGradientMask->Setter<WaveGradientMaskWaveWidthTag>(waveWidth);
    waveGradientMask->Setter<WaveGradientMaskPropagationRadiusTag>(propagationRadius);
    waveGradientMask->Setter<WaveGradientMaskBlurRadiusTag>(blurRadius);
    waveGradientMask->Setter<WaveGradientMaskTurbulenceStrengthTag>(turbulenceStrength);

    auto result = RSNGMaskToParaHelper::ConvertMaskToPara(mask);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->GetMaskParaType(), MaskPara::Type::WAVE_GRADIENT_MASK);

    auto para = std::static_pointer_cast<WaveGradientMaskPara>(result);
    EXPECT_FLOAT_EQ(para->GetWaveCenter().x_, waveCenter.x_);
    EXPECT_FLOAT_EQ(para->GetWaveCenter().y_, waveCenter.y_);
    EXPECT_FLOAT_EQ(para->GetWaveWidth(), waveWidth);
    EXPECT_FLOAT_EQ(para->GetPropagationRadius(), propagationRadius);
    EXPECT_FLOAT_EQ(para->GetBlurRadius(), blurRadius);
    EXPECT_FLOAT_EQ(para->GetTurbulenceStrength(), turbulenceStrength);
}

/**
 * @tc.name: ConvertMaskToParaWaveDisturbanceMask
 * @tc.desc: test RSNGMaskToParaHelper::ConvertMaskToPara with WAVE_DISTURBANCE_MASK
 * @tc.type: FUNC
 */
HWTEST_F(RSUIMaskToParaTest, ConvertMaskToParaWaveDisturbanceMask, TestSize.Level1)
{
    auto mask = RSNGMaskBase::Create(RSNGEffectType::WAVE_DISTURBANCE_MASK);
    EXPECT_NE(mask, nullptr);

    auto waveDisturbanceMask = std::static_pointer_cast<RSNGWaveDisturbanceMask>(mask);
    constexpr float progress = 0.5f;
    Vector2f clickPos = { 100.0f, 200.0f };
    Vector2f waveRD = { 5.0f, 0.5f };
    Vector3f waveLWH = { 300.0f, 100.0f, 50.0f };

    waveDisturbanceMask->Setter<WaveDisturbanceMaskProgressTag>(progress);
    waveDisturbanceMask->Setter<WaveDisturbanceMaskClickPosTag>(clickPos);
    waveDisturbanceMask->Setter<WaveDisturbanceMaskWaveRDTag>(waveRD);
    waveDisturbanceMask->Setter<WaveDisturbanceMaskWaveLWHTag>(waveLWH);

    auto result = RSNGMaskToParaHelper::ConvertMaskToPara(mask);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->GetMaskParaType(), MaskPara::Type::WAVE_DISTURBANCE_MASK);

    auto para = std::static_pointer_cast<WaveDisturbanceMaskPara>(result);
    EXPECT_FLOAT_EQ(para->GetProgress(), progress);
    EXPECT_FLOAT_EQ(para->GetClickPos().x_, clickPos.x_);
    EXPECT_FLOAT_EQ(para->GetClickPos().y_, clickPos.y_);
    EXPECT_FLOAT_EQ(para->GetWaveRD().x_, waveRD.x_);
    EXPECT_FLOAT_EQ(para->GetWaveRD().y_, waveRD.y_);
    EXPECT_FLOAT_EQ(para->GetWaveLWH().x_, waveLWH.x_);
    EXPECT_FLOAT_EQ(para->GetWaveLWH().y_, waveLWH.y_);
    EXPECT_FLOAT_EQ(para->GetWaveLWH().z_, waveLWH.z_);
}

/**
 * @tc.name: ConvertMaskToParaImageMask
 * @tc.desc: test RSNGMaskToParaHelper::ConvertMaskToPara with IMAGE_MASK
 * @tc.type: FUNC
 */
HWTEST_F(RSUIMaskToParaTest, ConvertMaskToParaImageMask, TestSize.Level1)
{
    auto mask = RSNGMaskBase::Create(RSNGEffectType::IMAGE_MASK);
    EXPECT_NE(mask, nullptr);

    auto imageMask = std::static_pointer_cast<RSNGImageMask>(mask);
    std::shared_ptr<Media::PixelMap> nullPixelMap = nullptr;
    imageMask->Setter<ImageMaskImageTag>(nullPixelMap);

    auto result = RSNGMaskToParaHelper::ConvertMaskToPara(mask);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->GetMaskParaType(), MaskPara::Type::IMAGE_MASK);

    auto para = std::static_pointer_cast<ImageMaskPara>(result);
    EXPECT_EQ(para->GetPixelMap(), nullptr);
}

/**
 * @tc.name: ConvertMaskToParaUseEffectMask
 * @tc.desc: test RSNGMaskToParaHelper::ConvertMaskToPara with USE_EFFECT_MASK
 * @tc.type: FUNC
 */
HWTEST_F(RSUIMaskToParaTest, ConvertMaskToParaUseEffectMask, TestSize.Level1)
{
    auto mask = RSNGMaskBase::Create(RSNGEffectType::USE_EFFECT_MASK);
    EXPECT_NE(mask, nullptr);

    auto useEffectMask = std::static_pointer_cast<RSNGUseEffectMask>(mask);
    std::shared_ptr<Media::PixelMap> nullPixelMap = nullptr;
    constexpr bool useEffect = true;

    useEffectMask->Setter<UseEffectMaskImageTag>(nullPixelMap);
    useEffectMask->Setter<UseEffectMaskUseEffectTag>(useEffect);

    auto result = RSNGMaskToParaHelper::ConvertMaskToPara(mask);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->GetMaskParaType(), MaskPara::Type::USE_EFFECT_MASK);

    auto para = std::static_pointer_cast<UseEffectMaskPara>(result);
    EXPECT_EQ(para->GetPixelMap(), nullptr);
    EXPECT_EQ(para->GetUseEffect(), useEffect);
}

/**
 * @tc.name: ConvertMaskToParaPixelMapMask
 * @tc.desc: test RSNGMaskToParaHelper::ConvertMaskToPara with PIXEL_MAP_MASK
 * @tc.type: FUNC
 */
HWTEST_F(RSUIMaskToParaTest, ConvertMaskToParaPixelMapMask, TestSize.Level1)
{
    auto mask = RSNGMaskBase::Create(RSNGEffectType::PIXEL_MAP_MASK);
    EXPECT_NE(mask, nullptr);

    auto pixelMapMask = std::static_pointer_cast<RSNGPixelMapMask>(mask);
    std::shared_ptr<Media::PixelMap> nullPixelMap = nullptr;
    Vector4f src = { 0.0f, 0.0f, 100.0f, 100.0f };
    Vector4f dst = { 0.0f, 0.0f, 50.0f, 50.0f };
    Vector4f fillColor = { 1.0f, 1.0f, 1.0f, 1.0f };

    pixelMapMask->Setter<PixelMapMaskImageTag>(nullPixelMap);
    pixelMapMask->Setter<PixelMapMaskSrcTag>(src);
    pixelMapMask->Setter<PixelMapMaskDstTag>(dst);
    pixelMapMask->Setter<PixelMapMaskFillColorTag>(fillColor);

    auto result = RSNGMaskToParaHelper::ConvertMaskToPara(mask);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->GetMaskParaType(), MaskPara::Type::PIXEL_MAP_MASK);

    auto para = std::static_pointer_cast<PixelMapMaskPara>(result);
    EXPECT_EQ(para->GetPixelMap(), nullptr);
    EXPECT_FLOAT_EQ(para->GetSrc().x_, src.x_);
    EXPECT_FLOAT_EQ(para->GetSrc().y_, src.y_);
    EXPECT_FLOAT_EQ(para->GetSrc().z_, src.z_);
    EXPECT_FLOAT_EQ(para->GetSrc().w_, src.w_);
    EXPECT_FLOAT_EQ(para->GetDst().x_, dst.x_);
    EXPECT_FLOAT_EQ(para->GetDst().y_, dst.y_);
    EXPECT_FLOAT_EQ(para->GetDst().z_, dst.z_);
    EXPECT_FLOAT_EQ(para->GetDst().w_, dst.w_);
    EXPECT_FLOAT_EQ(para->GetFillColor().x_, fillColor.x_);
    EXPECT_FLOAT_EQ(para->GetFillColor().y_, fillColor.y_);
    EXPECT_FLOAT_EQ(para->GetFillColor().z_, fillColor.z_);
    EXPECT_FLOAT_EQ(para->GetFillColor().w_, fillColor.w_);
}

/**
 * @tc.name: ConvertMaskToParaFractalGlassMask
 * @tc.desc: test RSNGMaskToParaHelper::ConvertMaskToPara with FRACTAL_GLASS_MASK
 * @tc.type: FUNC
 */
HWTEST_F(RSUIMaskToParaTest, ConvertMaskToParaFractalGlassMask, TestSize.Level1)
{
    auto mask = RSNGMaskBase::Create(RSNGEffectType::FRACTAL_GLASS_MASK);
    EXPECT_NE(mask, nullptr);

    auto fractalGlassMask = std::static_pointer_cast<RSNGFractalGlassMask>(mask);
    std::shared_ptr<Media::PixelMap> nullPixelMap = nullptr;
    Vector4f src = { 0.0f, 0.0f, 1.0f, 1.0f };
    Vector4f dst = { 0.0f, 0.0f, 0.5f, 0.5f };
    constexpr float glassNum = 25.0f;
    constexpr float glassStrength = 1.0f;
    constexpr float glassSoftness = 0.001f;
    constexpr bool isSymmetric = true;

    fractalGlassMask->Setter<FractalGlassMaskImageTag>(nullPixelMap);
    fractalGlassMask->Setter<FractalGlassMaskSrcTag>(src);
    fractalGlassMask->Setter<FractalGlassMaskDstTag>(dst);
    fractalGlassMask->Setter<FractalGlassMaskNumTag>(glassNum);
    fractalGlassMask->Setter<FractalGlassMaskStrengthTag>(glassStrength);
    fractalGlassMask->Setter<FractalGlassMaskSoftnessTag>(glassSoftness);
    fractalGlassMask->Setter<FractalGlassMaskSymmetricTag>(isSymmetric);

    auto result = RSNGMaskToParaHelper::ConvertMaskToPara(mask);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->GetMaskParaType(), MaskPara::Type::FRACTAL_GLASS_MASK);

    auto para = std::static_pointer_cast<FractalGlassMaskPara>(result);
    EXPECT_EQ(para->GetPixelMap(), nullptr);
    EXPECT_FLOAT_EQ(para->GetSrc().x_, src.x_);
    EXPECT_FLOAT_EQ(para->GetSrc().z_, src.z_);
    EXPECT_FLOAT_EQ(para->GetDst().x_, dst.x_);
    EXPECT_FLOAT_EQ(para->GetDst().z_, dst.z_);
    EXPECT_FLOAT_EQ(para->GetGlassNum(), glassNum);
    EXPECT_FLOAT_EQ(para->GetGlassStrength(), glassStrength);
    EXPECT_FLOAT_EQ(para->GetGlassSoftness(), glassSoftness);
    EXPECT_EQ(para->GetIsSymmetric(), isSymmetric);
}

/**
 * @tc.name: ConvertMaskToParaFractalGlassMaskAsymmetric
 * @tc.desc: test RSNGMaskToParaHelper::ConvertMaskToPara with FRACTAL_GLASS_MASK isSymmetric=false
 * @tc.type: FUNC
 */
HWTEST_F(RSUIMaskToParaTest, ConvertMaskToParaFractalGlassMaskAsymmetric, TestSize.Level1)
{
    auto mask = RSNGMaskBase::Create(RSNGEffectType::FRACTAL_GLASS_MASK);
    EXPECT_NE(mask, nullptr);

    auto fractalGlassMask = std::static_pointer_cast<RSNGFractalGlassMask>(mask);
    fractalGlassMask->Setter<FractalGlassMaskSymmetricTag>(false);

    auto result = RSNGMaskToParaHelper::ConvertMaskToPara(mask);
    EXPECT_NE(result, nullptr);
    auto para = std::static_pointer_cast<FractalGlassMaskPara>(result);
    EXPECT_EQ(para->GetIsSymmetric(), false);
}

/**
 * @tc.name: ConvertMaskToParaBinocularMask
 * @tc.desc: test RSNGMaskToParaHelper::ConvertMaskToPara with BINOCULAR_MASK
 * @tc.type: FUNC
 */
HWTEST_F(RSUIMaskToParaTest, ConvertMaskToParaBinocularMask, TestSize.Level1)
{
    auto mask = RSNGMaskBase::Create(RSNGEffectType::BINOCULAR_MASK);
    EXPECT_NE(mask, nullptr);

    auto binocularMask = std::static_pointer_cast<RSNGBinocularMask>(mask);
    constexpr float radiusX = 0.5f;
    constexpr float radiusY = 0.8f;
    constexpr float gap = 0.3f;
    constexpr float softness = 0.4f;

    binocularMask->Setter<BinocularMaskRadiusXTag>(radiusX);
    binocularMask->Setter<BinocularMaskRadiusYTag>(radiusY);
    binocularMask->Setter<BinocularMaskGapTag>(gap);
    binocularMask->Setter<BinocularMaskSoftnessTag>(softness);

    auto result = RSNGMaskToParaHelper::ConvertMaskToPara(mask);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->GetMaskParaType(), MaskPara::Type::BINOCULAR_MASK);

    auto para = std::static_pointer_cast<BinocularMaskPara>(result);
    EXPECT_FLOAT_EQ(para->GetRadiusX(), radiusX);
    EXPECT_FLOAT_EQ(para->GetRadiusY(), radiusY);
    EXPECT_FLOAT_EQ(para->GetGap(), gap);
    EXPECT_FLOAT_EQ(para->GetSoftness(), softness);
}

/**
* @tc.name: ConvertMaskToParaSweepRefractionMask
* @tc.desc: test RSNGMaskToParaHelper::ConvertMaskToPara with SWEEP_REFRACTION_MASK
* @tc.type: FUNC
*/
HWTEST_F(RSUIMaskToParaTest, ConvertMaskToParaSweepRefractionMask, TestSize.Level1)
{
    auto mask = RSNGMaskBase::Create(RSNGEffectType::SWEEP_REFRACTION_MASK);
    EXPECT_NE(mask, nullptr);

    auto sweepRefractionMask = std::static_pointer_cast<RSNGSweepRefractionMask>(mask);
    constexpr float maskRadius = 1.5f;
    constexpr float edgeThickness = 500.0f;
    constexpr float refractAmount = 0.3f;
    constexpr float rippleWidth = 0.4f;
    constexpr float sweepOffset = 0.25f;
    constexpr float chromaDelta = 0.08f;
    constexpr int32_t shapeType = 1;
    constexpr float cornerRadius = 0.16f;
    constexpr float prismWidth = 1.0f;
    constexpr float prismHeight = 1.0f;
    Vector2f sweepCenter = { 0.5f, 0.5f };

    sweepRefractionMask->Setter<SweepRefractionMaskMaskRadiusTag>(maskRadius);
    sweepRefractionMask->Setter<SweepRefractionMaskEdgeThicknessTag>(edgeThickness);
    sweepRefractionMask->Setter<SweepRefractionMaskRefractAmountTag>(refractAmount);
    sweepRefractionMask->Setter<SweepRefractionMaskRippleWidthTag>(rippleWidth);
    sweepRefractionMask->Setter<SweepRefractionMaskSweepOffsetTag>(sweepOffset);
    sweepRefractionMask->Setter<SweepRefractionMaskChromaDeltaTag>(chromaDelta);
    sweepRefractionMask->Setter<SweepRefractionMaskShapeTypeTag>(shapeType);
    sweepRefractionMask->Setter<SweepRefractionMaskCornerRadiusTag>(cornerRadius);
    sweepRefractionMask->Setter<SweepRefractionMaskPrismWidthTag>(prismWidth);
    sweepRefractionMask->Setter<SweepRefractionMaskPrismHeightTag>(prismHeight);
    sweepRefractionMask->Setter<SweepRefractionMaskSweepCenterXTag>(sweepCenter.x_);
    sweepRefractionMask->Setter<SweepRefractionMaskSweepCenterYTag>(sweepCenter.y_);

    auto result = RSNGMaskToParaHelper::ConvertMaskToPara(mask);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->GetMaskParaType(), MaskPara::Type::SWEEP_REFRACTION_MASK);

    auto para = std::static_pointer_cast<SweepRefractionMaskPara>(result);
    EXPECT_FLOAT_EQ(para->GetMaskRadius(), maskRadius);
    EXPECT_FLOAT_EQ(para->GetEdgeThickness(), edgeThickness);
    EXPECT_FLOAT_EQ(para->GetRefractAmount(), refractAmount);
    EXPECT_FLOAT_EQ(para->GetRippleWidth(), rippleWidth);
    EXPECT_FLOAT_EQ(para->GetSweepOffset(), sweepOffset);
    EXPECT_FLOAT_EQ(para->GetChromaDelta(), chromaDelta);
    EXPECT_EQ(para->GetShapeType(), shapeType);
    EXPECT_FLOAT_EQ(para->GetCornerRadius(), cornerRadius);
    EXPECT_FLOAT_EQ(para->GetPrismWidth(), prismWidth);
    EXPECT_FLOAT_EQ(para->GetPrismHeight(), prismHeight);
    EXPECT_FLOAT_EQ(para->GetSweepCenterX(), sweepCenter.x_);
    EXPECT_FLOAT_EQ(para->GetSweepCenterY(), sweepCenter.y_);
}

} // namespace OHOS::Rosen