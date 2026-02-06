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

#ifndef NG_SHADER_TEST_UTILS_H
#define NG_SHADER_TEST_UTILS_H

#include <memory>
#include <tuple>
#include <vector>

#include "ui_effect/property/include/rs_ui_shader_base.h"

namespace OHOS::Rosen {

// 1. InitAIBarGlow - RSNGAIBarGlow (15 parameters)
inline void InitAIBarGlow(std::shared_ptr<RSNGAIBarGlow>& aiBarGlow)
{
    if (!aiBarGlow) {
        return;
    }
    // LTWH: left, top, width, height
    aiBarGlow->Setter<AIBarGlowLTWHTag>(Vector4f{0.0f, 0.0f, 100.0f, 100.0f});
    // StretchFactor
    aiBarGlow->Setter<AIBarGlowStretchFactorTag>(1.0f);
    // BarAngle
    aiBarGlow->Setter<AIBarGlowBarAngleTag>(0.0f);
    // Colors
    aiBarGlow->Setter<AIBarGlowColor0Tag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
    aiBarGlow->Setter<AIBarGlowColor1Tag>(Vector4f{0.8f, 0.8f, 1.0f, 1.0f});
    aiBarGlow->Setter<AIBarGlowColor2Tag>(Vector4f{0.6f, 0.6f, 1.0f, 1.0f});
    aiBarGlow->Setter<AIBarGlowColor3Tag>(Vector4f{0.4f, 0.4f, 1.0f, 1.0f});
    // Positions (distributed to avoid overlap)
    aiBarGlow->Setter<AIBarGlowPosition0Tag>(Vector2f{0.2f, 0.2f});
    aiBarGlow->Setter<AIBarGlowPosition1Tag>(Vector2f{0.8f, 0.2f});
    aiBarGlow->Setter<AIBarGlowPosition2Tag>(Vector2f{0.8f, 0.8f});
    aiBarGlow->Setter<AIBarGlowPosition3Tag>(Vector2f{0.2f, 0.8f});
    // Strength
    aiBarGlow->Setter<AIBarGlowStrengthTag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
    // Brightness
    aiBarGlow->Setter<AIBarGlowBrightnessTag>(1.0f);
    // Progress
    aiBarGlow->Setter<AIBarGlowProgressTag>(0.5f);
}

// 2. InitAIBarRectHalo - RSNGAIBarRectHalo (12 parameters)
inline void InitAIBarRectHalo(std::shared_ptr<RSNGAIBarRectHalo>& aiBarRectHalo)
{
    if (!aiBarRectHalo) {
        return;
    }
    // LTWH: left, top, width, height
    aiBarRectHalo->Setter<AIBarRectHaloLTWHTag>(Vector4f{0.0f, 0.0f, 100.0f, 100.0f});
    // Colors
    aiBarRectHalo->Setter<AIBarRectHaloColor0Tag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
    aiBarRectHalo->Setter<AIBarRectHaloColor1Tag>(Vector4f{0.8f, 0.8f, 1.0f, 1.0f});
    aiBarRectHalo->Setter<AIBarRectHaloColor2Tag>(Vector4f{0.6f, 0.6f, 1.0f, 1.0f});
    aiBarRectHalo->Setter<AIBarRectHaloColor3Tag>(Vector4f{0.4f, 0.4f, 1.0f, 1.0f});
    // Positions (distributed to avoid overlap)
    aiBarRectHalo->Setter<AIBarRectHaloPosition0Tag>(Vector2f{0.2f, 0.2f});
    aiBarRectHalo->Setter<AIBarRectHaloPosition1Tag>(Vector2f{0.8f, 0.2f});
    aiBarRectHalo->Setter<AIBarRectHaloPosition2Tag>(Vector2f{0.8f, 0.8f});
    aiBarRectHalo->Setter<AIBarRectHaloPosition3Tag>(Vector2f{0.2f, 0.8f});
    // Strength
    aiBarRectHalo->Setter<AIBarRectHaloStrengthTag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
    // Brightness
    aiBarRectHalo->Setter<AIBarRectHaloBrightnessTag>(1.0f);
    // Progress
    aiBarRectHalo->Setter<AIBarRectHaloProgressTag>(0.5f);
}

// 3. InitAuroraNoise - RSNGAuroraNoise (1 parameter)
inline void InitAuroraNoise(std::shared_ptr<RSNGAuroraNoise>& auroraNoise)
{
    if (!auroraNoise) {
        return;
    }
    // Noise
    auroraNoise->Setter<AuroraNoiseNoiseTag>(0.5f);
}

// 4. InitBorderLight - RSNGBorderLight (6 parameters)
inline void InitBorderLight(std::shared_ptr<RSNGBorderLight>& borderLight)
{
    if (!borderLight) {
        return;
    }
    // Position (x, y, z)
    borderLight->Setter<BorderLightPositionTag>(Vector3f{0.5f, 0.5f, 1.0f});
    // Color
    borderLight->Setter<BorderLightColorTag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
    // Intensity
    borderLight->Setter<BorderLightIntensityTag>(1.0f);
    // Width
    borderLight->Setter<BorderLightWidthTag>(10.0f);
    // RotationAngle (x, y, z)
    borderLight->Setter<BorderLightRotationAngleTag>(Vector3f{0.0f, 0.0f, 0.0f});
    // CornerRadius
    borderLight->Setter<BorderLightCornerRadiusTag>(10.0f);
}

// 5. InitCircleFlowlight - RSNGCircleFlowlight (14 parameters)
inline void InitCircleFlowlight(std::shared_ptr<RSNGCircleFlowlight>& circleFlowlight)
{
    if (!circleFlowlight) {
        return;
    }
    // Colors
    circleFlowlight->Setter<CircleFlowlightColor0Tag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
    circleFlowlight->Setter<CircleFlowlightColor1Tag>(Vector4f{0.8f, 0.8f, 1.0f, 1.0f});
    circleFlowlight->Setter<CircleFlowlightColor2Tag>(Vector4f{0.6f, 0.6f, 1.0f, 1.0f});
    circleFlowlight->Setter<CircleFlowlightColor3Tag>(Vector4f{0.4f, 0.4f, 1.0f, 1.0f});
    // RotationFrequency
    circleFlowlight->Setter<CircleFlowlightRotationFrequencyTag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
    // RotationAmplitude
    circleFlowlight->Setter<CircleFlowlightRotationAmplitudeTag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
    // RotationSeed
    circleFlowlight->Setter<CircleFlowlightRotationSeedTag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
    // GradientX
    circleFlowlight->Setter<CircleFlowlightGradientXTag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
    // GradientY
    circleFlowlight->Setter<CircleFlowlightGradientYTag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
    // Progress
    circleFlowlight->Setter<CircleFlowlightProgressTag>(0.5f);
    // Strength
    circleFlowlight->Setter<CircleFlowlightStrengthTag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
    // DistortStrength
    circleFlowlight->Setter<CircleFlowlightDistortStrengthTag>(1.0f);
    // BlendGradient
    circleFlowlight->Setter<CircleFlowlightBlendGradientTag>(1.0f);
}

// 6. InitColorGradientEffect - RSNGColorGradientEffect (37 parameters)
inline void InitColorGradientEffect(std::shared_ptr<RSNGColorGradientEffect>& colorGradientEffect)
{
    if (!colorGradientEffect) {
        return;
    }
    // Colors (12 colors)
    colorGradientEffect->Setter<ColorGradientEffectColor0Tag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
    colorGradientEffect->Setter<ColorGradientEffectColor1Tag>(Vector4f{0.9f, 0.9f, 1.0f, 1.0f});
    colorGradientEffect->Setter<ColorGradientEffectColor2Tag>(Vector4f{0.8f, 0.8f, 1.0f, 1.0f});
    colorGradientEffect->Setter<ColorGradientEffectColor3Tag>(Vector4f{0.7f, 0.7f, 1.0f, 1.0f});
    colorGradientEffect->Setter<ColorGradientEffectColor4Tag>(Vector4f{0.6f, 0.6f, 1.0f, 1.0f});
    colorGradientEffect->Setter<ColorGradientEffectColor5Tag>(Vector4f{0.5f, 0.5f, 1.0f, 1.0f});
    colorGradientEffect->Setter<ColorGradientEffectColor6Tag>(Vector4f{0.4f, 0.4f, 1.0f, 1.0f});
    colorGradientEffect->Setter<ColorGradientEffectColor7Tag>(Vector4f{0.3f, 0.3f, 1.0f, 1.0f});
    colorGradientEffect->Setter<ColorGradientEffectColor8Tag>(Vector4f{0.2f, 0.2f, 1.0f, 1.0f});
    colorGradientEffect->Setter<ColorGradientEffectColor9Tag>(Vector4f{0.1f, 0.1f, 1.0f, 1.0f});
    colorGradientEffect->Setter<ColorGradientEffectColor10Tag>(Vector4f{0.0f, 0.0f, 1.0f, 1.0f});
    colorGradientEffect->Setter<ColorGradientEffectColor11Tag>(Vector4f{1.0f, 0.0f, 1.0f, 1.0f});
    // Positions (12 positions, distributed)
    colorGradientEffect->Setter<ColorGradientEffectPosition0Tag>(Vector2f{0.0f, 0.0f});
    colorGradientEffect->Setter<ColorGradientEffectPosition1Tag>(Vector2f{0.1f, 0.1f});
    colorGradientEffect->Setter<ColorGradientEffectPosition2Tag>(Vector2f{0.2f, 0.2f});
    colorGradientEffect->Setter<ColorGradientEffectPosition3Tag>(Vector2f{0.3f, 0.3f});
    colorGradientEffect->Setter<ColorGradientEffectPosition4Tag>(Vector2f{0.4f, 0.4f});
    colorGradientEffect->Setter<ColorGradientEffectPosition5Tag>(Vector2f{0.5f, 0.5f});
    colorGradientEffect->Setter<ColorGradientEffectPosition6Tag>(Vector2f{0.6f, 0.6f});
    colorGradientEffect->Setter<ColorGradientEffectPosition7Tag>(Vector2f{0.7f, 0.7f});
    colorGradientEffect->Setter<ColorGradientEffectPosition8Tag>(Vector2f{0.8f, 0.8f});
    colorGradientEffect->Setter<ColorGradientEffectPosition9Tag>(Vector2f{0.9f, 0.9f});
    colorGradientEffect->Setter<ColorGradientEffectPosition10Tag>(Vector2f{1.0f, 1.0f});
    colorGradientEffect->Setter<ColorGradientEffectPosition11Tag>(Vector2f{0.5f, 1.0f});
    // Strengths (12 strengths)
    colorGradientEffect->Setter<ColorGradientEffectStrength0Tag>(1.0f);
    colorGradientEffect->Setter<ColorGradientEffectStrength1Tag>(1.0f);
    colorGradientEffect->Setter<ColorGradientEffectStrength2Tag>(1.0f);
    colorGradientEffect->Setter<ColorGradientEffectStrength3Tag>(1.0f);
    colorGradientEffect->Setter<ColorGradientEffectStrength4Tag>(1.0f);
    colorGradientEffect->Setter<ColorGradientEffectStrength5Tag>(1.0f);
    colorGradientEffect->Setter<ColorGradientEffectStrength6Tag>(1.0f);
    colorGradientEffect->Setter<ColorGradientEffectStrength7Tag>(1.0f);
    colorGradientEffect->Setter<ColorGradientEffectStrength8Tag>(1.0f);
    colorGradientEffect->Setter<ColorGradientEffectStrength9Tag>(1.0f);
    colorGradientEffect->Setter<ColorGradientEffectStrength10Tag>(1.0f);
    colorGradientEffect->Setter<ColorGradientEffectStrength11Tag>(1.0f);
    // ColorNumber
    colorGradientEffect->Setter<ColorGradientEffectColorNumberTag>(4.0f);
    // Blend
    colorGradientEffect->Setter<ColorGradientEffectBlendTag>(1.0f);
    // BlendK
    colorGradientEffect->Setter<ColorGradientEffectBlendKTag>(1.0f);
    // Brightness
    colorGradientEffect->Setter<ColorGradientEffectBrightnessTag>(1.0f);
}

// 7. InitContourDiagonalFlowLight - RSNGContourDiagonalFlowLight (11 parameters)
inline void InitContourDiagonalFlowLight(std::shared_ptr<RSNGContourDiagonalFlowLight>& contourDiagonalFlowLight)
{
    if (!contourDiagonalFlowLight) {
        return;
    }
    // Contour (vector of Vector2f points)
    std::vector<Vector2f> contour = {
        Vector2f{0.2f, 0.2f},
        Vector2f{0.8f, 0.2f},
        Vector2f{0.8f, 0.8f},
        Vector2f{0.2f, 0.8f}
    };
    contourDiagonalFlowLight->Setter<ContourDiagonalFlowLightContourTag>(contour);
    // Line1Start
    contourDiagonalFlowLight->Setter<ContourDiagonalFlowLightLine1StartTag>(0.0f);
    // Line1Length
    contourDiagonalFlowLight->Setter<ContourDiagonalFlowLightLine1LengthTag>(0.5f);
    // Line1Color
    contourDiagonalFlowLight->Setter<ContourDiagonalFlowLightLine1ColorTag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
    // Line2Start
    contourDiagonalFlowLight->Setter<ContourDiagonalFlowLightLine2StartTag>(0.5f);
    // Line2Length
    contourDiagonalFlowLight->Setter<ContourDiagonalFlowLightLine2LengthTag>(0.5f);
    // Line2Color
    contourDiagonalFlowLight->Setter<ContourDiagonalFlowLightLine2ColorTag>(Vector4f{0.8f, 0.8f, 1.0f, 1.0f});
    // Thickness
    contourDiagonalFlowLight->Setter<ContourDiagonalFlowLightThicknessTag>(2.0f);
    // HaloRadius
    contourDiagonalFlowLight->Setter<ContourDiagonalFlowLightHaloRadiusTag>(10.0f);
    // LightWeight
    contourDiagonalFlowLight->Setter<ContourDiagonalFlowLightLightWeightTag>(1.0f);
    // HaloWeight
    contourDiagonalFlowLight->Setter<ContourDiagonalFlowLightHaloWeightTag>(1.0f);
}

// 8. InitDistortChroma - RSNGDistortChroma (11 parameters)
inline void InitDistortChroma(std::shared_ptr<RSNGDistortChroma>& distortChroma)
{
    if (!distortChroma) {
        return;
    }
    // Progress
    distortChroma->Setter<DistortChromaProgressTag>(0.5f);
    // Granularity
    distortChroma->Setter<DistortChromaGranularityTag>(Vector2f{1.0f, 1.0f});
    // Frequency
    distortChroma->Setter<DistortChromaFrequencyTag>(1.0f);
    // Sharpness
    distortChroma->Setter<DistortChromaSharpnessTag>(1.0f);
    // Brightness
    distortChroma->Setter<DistortChromaBrightnessTag>(1.0f);
    // Dispersion
    distortChroma->Setter<DistortChromaDispersionTag>(0.5f);
    // DistortFactor
    distortChroma->Setter<DistortChromaDistortFactorTag>(Vector2f{1.0f, 1.0f});
    // Saturation
    distortChroma->Setter<DistortChromaSaturationTag>(1.0f);
    // ColorStrength
    distortChroma->Setter<DistortChromaColorStrengthTag>(Vector3f{1.0f, 1.0f, 1.0f});
    // VerticalMoveDistance
    distortChroma->Setter<DistortChromaVerticalMoveDistanceTag>(0.0f);
}

// 9. InitDotMatrixShader - RSNGDotMatrixShader (13 parameters)
inline void InitDotMatrixShader(std::shared_ptr<RSNGDotMatrixShader>& dotMatrixShader)
{
    if (!dotMatrixShader) {
        return;
    }
    // PathDirection
    dotMatrixShader->Setter<DotMatrixShaderPathDirectionTag>(0);
    // EffectColors (vector of Vector4f)
    std::vector<Vector4f> effectColors = {
        Vector4f{1.0f, 1.0f, 1.0f, 1.0f},
        Vector4f{0.8f, 0.8f, 1.0f, 1.0f}
    };
    dotMatrixShader->Setter<DotMatrixShaderEffectColorsTag>(effectColors);
    // ColorFractions
    dotMatrixShader->Setter<DotMatrixShaderColorFractionsTag>(Vector2f{0.0f, 1.0f});
    // StartPoints (vector of Vector2f)
    std::vector<Vector2f> startPoints = {
        Vector2f{0.2f, 0.2f},
        Vector2f{0.8f, 0.8f}
    };
    dotMatrixShader->Setter<DotMatrixShaderStartPointsTag>(startPoints);
    // PathWidth
    dotMatrixShader->Setter<DotMatrixShaderPathWidthTag>(10.0f);
    // InverseEffect
    dotMatrixShader->Setter<DotMatrixShaderInverseEffectTag>(false);
    // DotColor
    dotMatrixShader->Setter<DotMatrixShaderDotColorTag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
    // DotSpacing
    dotMatrixShader->Setter<DotMatrixShaderDotSpacingTag>(5.0f);
    // DotRadius
    dotMatrixShader->Setter<DotMatrixShaderDotRadiusTag>(2.0f);
    // BgColor
    dotMatrixShader->Setter<DotMatrixShaderBgColorTag>(Vector4f{0.0f, 0.0f, 0.0f, 1.0f});
    // EffectType
    dotMatrixShader->Setter<DotMatrixShaderEffectTypeTag>(0);
    // Progress
    dotMatrixShader->Setter<DotMatrixShaderProgressTag>(0.5f);
}

// 10. InitFrostedGlassEffect - RSNGFrostedGlassEffect (26 parameters)
inline void InitFrostedGlassEffect(std::shared_ptr<RSNGFrostedGlassEffect>& frostedGlassEffect)
{
    if (!frostedGlassEffect) {
        return;
    }
    // WeightsEmboss
    frostedGlassEffect->Setter<FrostedGlassEffectWeightsEmbossTag>(Vector2f{1.0f, 0.5f});
    // WeightsEdl
    frostedGlassEffect->Setter<FrostedGlassEffectWeightsEdlTag>(Vector2f{1.0f, 1.0f});
    // BgRates
    frostedGlassEffect->Setter<FrostedGlassEffectBgRatesTag>(Vector2f{-1.8792225f, 2.7626955f});
    // BgKBS
    frostedGlassEffect->Setter<FrostedGlassEffectBgKBSTag>(Vector3f{0.0073494f, 0.0998859f, 1.2f});
    // BgPos
    frostedGlassEffect->Setter<FrostedGlassEffectBgPosTag>(Vector3f{0.3f, 0.5f, 0.5f});
    // BgNeg
    frostedGlassEffect->Setter<FrostedGlassEffectBgNegTag>(Vector3f{0.5f, 1.0f, 1.0f});
    // RefractParams
    frostedGlassEffect->Setter<FrostedGlassEffectRefractParamsTag>(Vector3f{1.0f, 0.3f, 0.3f});
    // SdParams
    frostedGlassEffect->Setter<FrostedGlassEffectSdParamsTag>(Vector3f{-50.0f, 6.0f, 6.62f});
    // SdRates
    frostedGlassEffect->Setter<FrostedGlassEffectSdRatesTag>(Vector2f{0.0f, 0.0f});
    // SdKBS
    frostedGlassEffect->Setter<FrostedGlassEffectSdKBSTag>(Vector3f{0.9f, 0.0f, 1.0f});
    // SdPos
    frostedGlassEffect->Setter<FrostedGlassEffectSdPosTag>(Vector3f{1.0f, 1.7f, 1.5f});
    // SdNeg
    frostedGlassEffect->Setter<FrostedGlassEffectSdNegTag>(Vector3f{3.0f, 2.0f, 1.0f});
    // EnvLightParams
    frostedGlassEffect->Setter<FrostedGlassEffectEnvLightParamsTag>(Vector2f{20.0f, 5.0f});
    // EnvLightRates
    frostedGlassEffect->Setter<FrostedGlassEffectEnvLightRatesTag>(Vector2f{0.0f, 0.0f});
    // EnvLightKBS
    frostedGlassEffect->Setter<FrostedGlassEffectEnvLightKBSTag>(Vector3f{0.8f, 0.27451f, 2.0f});
    // EnvLightPos
    frostedGlassEffect->Setter<FrostedGlassEffectEnvLightPosTag>(Vector3f{1.0f, 1.7f, 1.5f});
    // EnvLightNeg
    frostedGlassEffect->Setter<FrostedGlassEffectEnvLightNegTag>(Vector3f{3.0f, 2.0f, 1.0f});
    // EdLightParams
    frostedGlassEffect->Setter<FrostedGlassEffectEdLightParamsTag>(Vector2f{2.0f, 2.0f});
    // EdLightAngles
    frostedGlassEffect->Setter<FrostedGlassEffectEdLightAnglesTag>(Vector2f{40.0f, 20.0f});
    // EdLightDir
    frostedGlassEffect->Setter<FrostedGlassEffectEdLightDirTag>(Vector2f{2.5f, 2.5f});
    // EdLightRates
    frostedGlassEffect->Setter<FrostedGlassEffectEdLightRatesTag>(Vector2f{0.0f, 0.0f});
    // EdLightKBS
    frostedGlassEffect->Setter<FrostedGlassEffectEdLightKBSTag>(Vector3f{0.6027f, 0.627451f, 2.0f});
    // EdLightPos
    frostedGlassEffect->Setter<FrostedGlassEffectEdLightPosTag>(Vector3f{1.0f, 1.7f, 1.5f});
    // EdLightNeg
    frostedGlassEffect->Setter<FrostedGlassEffectEdLightNegTag>(Vector3f{3.2f, 2.0f, 1.0f});
    // MaterialColor
    frostedGlassEffect->Setter<FrostedGlassEffectMaterialColorTag>(Vector4f{0.0f, 0.0f, 0.0f, 0.0f});
}

// 11. InitGradientFlowColors - RSNGGradientFlowColors (8 parameters)
inline void InitGradientFlowColors(std::shared_ptr<RSNGGradientFlowColors>& gradientFlowColors)
{
    if (!gradientFlowColors) {
        return;
    }
    // Colors
    gradientFlowColors->Setter<GradientFlowColorsColor0Tag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
    gradientFlowColors->Setter<GradientFlowColorsColor1Tag>(Vector4f{0.8f, 0.8f, 1.0f, 1.0f});
    gradientFlowColors->Setter<GradientFlowColorsColor2Tag>(Vector4f{0.6f, 0.6f, 1.0f, 1.0f});
    gradientFlowColors->Setter<GradientFlowColorsColor3Tag>(Vector4f{0.4f, 0.4f, 1.0f, 1.0f});
    // GradientBegin
    gradientFlowColors->Setter<GradientFlowColorsGradientBeginTag>(0.0f);
    // GradientEnd
    gradientFlowColors->Setter<GradientFlowColorsGradientEndTag>(1.0f);
    // EffectAlpha
    gradientFlowColors->Setter<GradientFlowColorsEffectAlphaTag>(1.0f);
    // Progress
    gradientFlowColors->Setter<GradientFlowColorsProgressTag>(0.5f);
}

// 12. InitLightCave - RSNGLightCave (6 parameters)
inline void InitLightCave(std::shared_ptr<RSNGLightCave>& lightCave)
{
    if (!lightCave) {
        return;
    }
    // Colors
    lightCave->Setter<LightCaveColorATag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
    lightCave->Setter<LightCaveColorBTag>(Vector4f{0.8f, 0.8f, 1.0f, 1.0f});
    lightCave->Setter<LightCaveColorCTag>(Vector4f{0.6f, 0.6f, 1.0f, 1.0f});
    // Position
    lightCave->Setter<LightCavePositionTag>(Vector2f{0.5f, 0.5f});
    // RadiusXY
    lightCave->Setter<LightCaveRadiusXYTag>(Vector2f{50.0f, 50.0f});
    // Progress
    lightCave->Setter<LightCaveProgressTag>(0.5f);
}

// 13. InitParticleCircularHalo - RSNGParticleCircularHalo (3 parameters)
inline void InitParticleCircularHalo(std::shared_ptr<RSNGParticleCircularHalo>& particleCircularHalo)
{
    if (!particleCircularHalo) {
        return;
    }
    // Center
    particleCircularHalo->Setter<ParticleCircularHaloCenterTag>(Vector2f{0.5f, 0.5f});
    // Radius
    particleCircularHalo->Setter<ParticleCircularHaloRadiusTag>(50.0f);
    // Noise
    particleCircularHalo->Setter<ParticleCircularHaloNoiseTag>(0.5f);
}

// 14. InitRoundedRectFlowlight - RSNGRoundedRectFlowlight (11 parameters)
inline void InitRoundedRectFlowlight(std::shared_ptr<RSNGRoundedRectFlowlight>& roundedRectFlowlight)
{
    if (!roundedRectFlowlight) {
        return;
    }
    // StartEndPosition
    roundedRectFlowlight->Setter<RoundedRectFlowlightStartEndPositionTag>(Vector2f{0.0f, 1.0f});
    // WaveLength
    roundedRectFlowlight->Setter<RoundedRectFlowlightWaveLengthTag>(0.2f);
    // WaveTop
    roundedRectFlowlight->Setter<RoundedRectFlowlightWaveTopTag>(0.2f);
    // CornerRadius
    roundedRectFlowlight->Setter<RoundedRectFlowlightCornerRadiusTag>(0.1f);
    // Brightness
    roundedRectFlowlight->Setter<RoundedRectFlowlightBrightnessTag>(1.0f);
    // Scale
    roundedRectFlowlight->Setter<RoundedRectFlowlightScaleTag>(1.0f);
    // Sharping
    roundedRectFlowlight->Setter<RoundedRectFlowlightSharpingTag>(1.0f);
    // Feathering
    roundedRectFlowlight->Setter<RoundedRectFlowlightFeatheringTag>(1.0f);
    // FeatheringBezierControlPoints
    roundedRectFlowlight->Setter<RoundedRectFlowlightFeatheringBezierControlPointsTag>(Vector4f{0.0f, 0.0f, 1.0f, 1.0f});
    // GradientBezierControlPoints
    roundedRectFlowlight->Setter<RoundedRectFlowlightGradientBezierControlPointsTag>(Vector4f{0.0f, 0.0f, 1.0f, 1.0f});
    // Color
    roundedRectFlowlight->Setter<RoundedRectFlowlightColorTag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
    // Progress
    roundedRectFlowlight->Setter<RoundedRectFlowlightProgressTag>(0.5f);
}

// 15. InitWavyRippleLight - RSNGWavyRippleLight (3 parameters)
inline void InitWavyRippleLight(std::shared_ptr<RSNGWavyRippleLight>& wavyRippleLight)
{
    if (!wavyRippleLight) {
        return;
    }
    // Center
    wavyRippleLight->Setter<WavyRippleLightCenterTag>(Vector2f{0.5f, 0.5f});
    // Radius
    wavyRippleLight->Setter<WavyRippleLightRadiusTag>(50.0f);
    // Thickness
    wavyRippleLight->Setter<WavyRippleLightThicknessTag>(10.0f);
}

}  // namespace OHOS::Rosen

#endif // NG_SHADER_TEST_UTILS_H
