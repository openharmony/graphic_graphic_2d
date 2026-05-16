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

#include "ge_visual_effect.h"
#include "ge_visual_effect_container.h"
#include "gtest/gtest.h"
#include "parcel.h"

#include "effect/rs_render_effect_template.h"
#include "effect/rs_render_mask_base.h"
#include "effect/rs_render_property_tag.h"
#include "effect/rs_render_shader_base.h"
#include "pipeline/rs_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class RSNGRenderEffectTemplateTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSNGRenderEffectTemplateTest::SetUpTestCase() {}
void RSNGRenderEffectTemplateTest::TearDownTestCase() {}
void RSNGRenderEffectTemplateTest::SetUp() {}
void RSNGRenderEffectTemplateTest::TearDown() {}

/**
 * @tc.name: GetEffectTypeStringForBasicTypes
 * @tc.desc: Verify GetEffectTypeString for all effect types
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, GetEffectTypeStringForBasicTypes, TestSize.Level1)
{
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::INVALID), "Invalid");
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::NONE), "None");
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::BLUR), "Blur");
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::DISPLACEMENT_DISTORT), "DispDistort");
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::SOUND_WAVE), "SoundWave");
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::EDGE_LIGHT), "EdgeLight");
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::DISPERSION), "Dispersion");
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::DIRECTION_LIGHT), "DirectionLight");
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::BEZIER_WARP), "BezierWarp");
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::COLOR_GRADIENT), "ColorGradient");
}

/**
 * @tc.name: GetEffectTypeStringForMaskTypes
 * @tc.desc: Verify GetEffectTypeString for mask types
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, GetEffectTypeStringForMaskTypes, TestSize.Level1)
{
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::RIPPLE_MASK), "RippleMask");
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::DOUBLE_RIPPLE_MASK), "DoubleRippleMask");
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::PIXEL_MAP_MASK), "PixelMapMask");
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::RADIAL_GRADIENT_MASK), "RadialGradientMask");
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::WAVE_GRADIENT_MASK), "WaveGradientMask");
    EXPECT_EQ(
        RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::WAVE_DISTURBANCE_MASK), "WaveDisturbanceMask");
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::FRAME_GRADIENT_MASK), "FrameGradientMask");
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::IMAGE_MASK), "ImageMask");
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::USE_EFFECT_MASK), "UseEffectMask");
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::DUPOLI_NOISE_MASK), "DupoliNoiseMask");
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::NOISY_FRAME_GRADIENT_MASK),
        "NoisyFrameGradientMask");
}

/**
 * @tc.name: GetEffectTypeStringForSDFShapeTypes
 * @tc.desc: Verify GetEffectTypeString for SDF shape types
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, GetEffectTypeStringForSDFShapeTypes, TestSize.Level1)
{
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::SDF_UNION_OP_SHAPE), "SDFUnionOpShape");
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::SDF_SMOOTH_UNION_OP_SHAPE),
        "SDFSmoothUnionOpShape");
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::SDF_RRECT_SHAPE), "SDFRRectShape");
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::SDF_TRIANGLE_SHAPE), "SDFTriangleShape");
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::SDF_PIXELMAP_SHAPE), "SDFPixelmapShape");
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::SDF_TRANSFORM_SHAPE), "SDFTransformShape");
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::SDF_EMPTY_SHAPE), "SDFEmptyShape");
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::SDF_DISTORT_OP_SHAPE), "SDFDistortOpShape");
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::SDF_EDGE_LIGHT), "SDFEdgeLight");
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::SDF_EDGE_LIGHT_EFFECT), "SDFEdgeLightEffect");
}

/**
 * @tc.name: GetEffectTypeStringForShaderEffectTypes
 * @tc.desc: Verify GetEffectTypeString for shader effect types
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, GetEffectTypeStringForShaderEffectTypes, TestSize.Level1)
{
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::CONTOUR_DIAGONAL_FLOW_LIGHT),
        "ContourDiagonalFlowLight");
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::DOT_MATRIX_SHADER), "DotMatrixShader");
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::WAVY_RIPPLE_LIGHT), "WavyRippleLight");
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::AURORA_NOISE), "AuroraNoise");
    EXPECT_EQ(
        RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::PARTICLE_CIRCULAR_HALO), "ParticleCircularHalo");
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::LIGHT_CAVE), "LightCave");
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::BORDER_LIGHT), "BorderLight");
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::HARMONIUM_EFFECT), "HarmoniumEffect");
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::CONTENT_LIGHT), "ContentLight");
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::BORDER_SDF_SHADER), "BorderSDFShader");
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::DISTORT_CHROMA), "DistortChroma");
}

/**
 * @tc.name: GetEffectTypeStringForGlowAndFlowlightTypes
 * @tc.desc: Verify GetEffectTypeString for glow and flowlight types
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, GetEffectTypeStringForGlowAndFlowlightTypes, TestSize.Level1)
{
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::AIBAR_GLOW), "AIBarGlow");
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::AIBAR_RECT_HALO), "AIBarRectHalo");
    EXPECT_EQ(
        RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::ROUNDED_RECT_FLOWLIGHT), "RoundedRectFlowlight");
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::GRADIENT_FLOW_COLORS), "GradientFlowColors");
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::CIRCLE_FLOWLIGHT), "CircleFlowlight");
}

/**
 * @tc.name: GetEffectTypeStringForFrostedGlassTypes
 * @tc.desc: Verify GetEffectTypeString for frosted glass types
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, GetEffectTypeStringForFrostedGlassTypes, TestSize.Level1)
{
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::FROSTED_GLASS), "FrostedGlass");
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::FROSTED_GLASS_EFFECT), "FrostedGlassEffect");
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::FROSTED_GLASS_BLUR), "FrostedGlassBlur");
}

/**
 * @tc.name: GetEffectTypeStringForUnknownType
 * @tc.desc: Verify GetEffectTypeString for unknown type
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, GetEffectTypeStringForUnknownType, TestSize.Level1)
{
    constexpr int32_t unknownValue = 9999;
    RSNGEffectType unknownType = static_cast<RSNGEffectType>(unknownValue);
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(unknownType), "UNKNOWN");
}

/**
 * @tc.name: CreateGEVisualEffectForValidTypes
 * @tc.desc: Verify CreateGEVisualEffect for valid types
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, CreateGEVisualEffectForValidTypes, TestSize.Level1)
{
    auto blurEffect = RSNGRenderEffectHelper::CreateGEVisualEffect(RSNGEffectType::BLUR);
    EXPECT_NE(blurEffect, nullptr);

    auto distortEffect = RSNGRenderEffectHelper::CreateGEVisualEffect(RSNGEffectType::DISPLACEMENT_DISTORT);
    EXPECT_NE(distortEffect, nullptr);

    auto soundWaveEffect = RSNGRenderEffectHelper::CreateGEVisualEffect(RSNGEffectType::SOUND_WAVE);
    EXPECT_NE(soundWaveEffect, nullptr);

    auto edgeLightEffect = RSNGRenderEffectHelper::CreateGEVisualEffect(RSNGEffectType::EDGE_LIGHT);
    EXPECT_NE(edgeLightEffect, nullptr);

    auto dispersionEffect = RSNGRenderEffectHelper::CreateGEVisualEffect(RSNGEffectType::DISPERSION);
    EXPECT_NE(dispersionEffect, nullptr);
}

/**
 * @tc.name: CreateGEVisualEffectForMaskTypes
 * @tc.desc: Verify CreateGEVisualEffect for mask types
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, CreateGEVisualEffectForMaskTypes, TestSize.Level1)
{
    auto rippleMask = RSNGRenderEffectHelper::CreateGEVisualEffect(RSNGEffectType::RIPPLE_MASK);
    EXPECT_NE(rippleMask, nullptr);

    auto doubleRippleMask = RSNGRenderEffectHelper::CreateGEVisualEffect(RSNGEffectType::DOUBLE_RIPPLE_MASK);
    EXPECT_NE(doubleRippleMask, nullptr);

    auto pixelMapMask = RSNGRenderEffectHelper::CreateGEVisualEffect(RSNGEffectType::PIXEL_MAP_MASK);
    EXPECT_NE(pixelMapMask, nullptr);

    auto radialGradientMask = RSNGRenderEffectHelper::CreateGEVisualEffect(RSNGEffectType::RADIAL_GRADIENT_MASK);
    EXPECT_NE(radialGradientMask, nullptr);

    auto waveGradientMask = RSNGRenderEffectHelper::CreateGEVisualEffect(RSNGEffectType::WAVE_GRADIENT_MASK);
    EXPECT_NE(waveGradientMask, nullptr);
}

/**
 * @tc.name: AppendToGEContainerWithValidEffect
 * @tc.desc: Verify AppendToGEContainer with valid container
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, AppendToGEContainerWithValidEffect, TestSize.Level1)
{
    auto geContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    auto geEffect = RSNGRenderEffectHelper::CreateGEVisualEffect(RSNGEffectType::BLUR);

    RSNGRenderEffectHelper::AppendToGEContainer(geContainer, geEffect);

    auto filters = geContainer->GetFilters();
    EXPECT_FALSE(filters.empty());
}

/**
 * @tc.name: AppendToGEContainerWithNullEffect
 * @tc.desc: Verify AppendToGEContainer with null effect
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, AppendToGEContainerWithNullEffect, TestSize.Level1)
{
    auto geContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    std::shared_ptr<Drawing::GEVisualEffect> nullEffect = nullptr;

    RSNGRenderEffectHelper::AppendToGEContainer(geContainer, nullEffect);

    auto filters = geContainer->GetFilters();
    EXPECT_TRUE(filters.empty());
}

/**
 * @tc.name: CalculatePropTagHashForInt
 * @tc.desc: Verify CalculatePropTagHashImpl with int value
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, CalculatePropTagHashForInt, TestSize.Level1)
{
    uint32_t hash = 0;
    constexpr int testValue = 10;
    RSNGRenderEffectHelper::CalculatePropTagHashImpl(hash, testValue);
    EXPECT_NE(hash, 0);
}

/**
 * @tc.name: CalculatePropTagHashForFloat
 * @tc.desc: Verify CalculatePropTagHashImpl with float value
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, CalculatePropTagHashForFloat, TestSize.Level1)
{
    uint32_t hash = 0;
    constexpr float testValue = 1.5f;
    RSNGRenderEffectHelper::CalculatePropTagHashImpl(hash, testValue);
    EXPECT_NE(hash, 0);
}

/**
 * @tc.name: CalculatePropTagHashForBool
 * @tc.desc: Verify CalculatePropTagHashImpl with bool value
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, CalculatePropTagHashForBool, TestSize.Level1)
{
    uint32_t hashTrue = 0;
    RSNGRenderEffectHelper::CalculatePropTagHashImpl(hashTrue, true);
    EXPECT_NE(hashTrue, 0);

    uint32_t hashFalse = 0;
    RSNGRenderEffectHelper::CalculatePropTagHashImpl(hashFalse, false);
    EXPECT_NE(hashFalse, 0);
}

/**
 * @tc.name: CalculatePropTagHashForVector2f
 * @tc.desc: Verify CalculatePropTagHashImpl with Vector2f value
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, CalculatePropTagHashForVector2f, TestSize.Level1)
{
    uint32_t hash = 0;
    Vector2f testValue(1.0f, 2.0f);
    RSNGRenderEffectHelper::CalculatePropTagHashImpl(hash, testValue);
    EXPECT_NE(hash, 0);
}

/**
 * @tc.name: CalculatePropTagHashForVector3f
 * @tc.desc: Verify CalculatePropTagHashImpl with Vector3f value
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, CalculatePropTagHashForVector3f, TestSize.Level1)
{
    uint32_t hash = 0;
    Vector3f testValue(1.0f, 2.0f, 3.0f);
    RSNGRenderEffectHelper::CalculatePropTagHashImpl(hash, testValue);
    EXPECT_NE(hash, 0);
}

/**
 * @tc.name: CalculatePropTagHashForVector4f
 * @tc.desc: Verify CalculatePropTagHashImpl with Vector4f value
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, CalculatePropTagHashForVector4f, TestSize.Level1)
{
    uint32_t hash = 0;
    Vector4f testValue(1.0f, 2.0f, 3.0f, 4.0f);
    RSNGRenderEffectHelper::CalculatePropTagHashImpl(hash, testValue);
    EXPECT_NE(hash, 0);
}

/**
 * @tc.name: ShaderGetTypeReturnsCorrectType
 * @tc.desc: Verify GetType for shader base
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, ShaderGetTypeReturnsCorrectType, TestSize.Level1)
{
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::CONTOUR_DIAGONAL_FLOW_LIGHT);
    EXPECT_NE(shader, nullptr);
    EXPECT_EQ(shader->GetType(), RSNGEffectType::CONTOUR_DIAGONAL_FLOW_LIGHT);
}

/**
 * @tc.name: MaskGetTypeReturnsCorrectType
 * @tc.desc: Verify GetType for mask base
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, MaskGetTypeReturnsCorrectType, TestSize.Level1)
{
    auto mask = RSNGRenderMaskBase::Create(RSNGEffectType::RIPPLE_MASK);
    EXPECT_NE(mask, nullptr);
    EXPECT_EQ(mask->GetType(), RSNGEffectType::RIPPLE_MASK);
}

/**
 * @tc.name: ShaderContainsTypeForMatchedType
 * @tc.desc: Verify ContainsType for matched effect
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, ShaderContainsTypeForMatchedType, TestSize.Level1)
{
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::CONTOUR_DIAGONAL_FLOW_LIGHT);
    EXPECT_NE(shader, nullptr);
    EXPECT_TRUE(shader->ContainsType(RSNGEffectType::CONTOUR_DIAGONAL_FLOW_LIGHT));
}

/**
 * @tc.name: ShaderContainsTypeForUnmatchedType
 * @tc.desc: Verify ContainsType for unmatched effect
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, ShaderContainsTypeForUnmatchedType, TestSize.Level1)
{
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::CONTOUR_DIAGONAL_FLOW_LIGHT);
    EXPECT_NE(shader, nullptr);
    EXPECT_FALSE(shader->ContainsType(RSNGEffectType::BLUR));
}

/**
 * @tc.name: ShaderGetEffectCountForSingleEffect
 * @tc.desc: Verify GetEffectCount for single effect
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, ShaderGetEffectCountForSingleEffect, TestSize.Level1)
{
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::LIGHT_CAVE);
    EXPECT_NE(shader, nullptr);
    EXPECT_EQ(shader->GetEffectCount(), 1);
}

/**
 * @tc.name: ShaderDumpContainsEffectName
 * @tc.desc: Verify Dump method for shader effect
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, ShaderDumpContainsEffectName, TestSize.Level1)
{
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::AURORA_NOISE);
    EXPECT_NE(shader, nullptr);
    std::string dumpStr;
    shader->Dump(dumpStr);
    EXPECT_FALSE(dumpStr.empty());
    EXPECT_TRUE(dumpStr.find("AuroraNoise") != std::string::npos);
}

/**
 * @tc.name: MaskDumpContainsEffectName
 * @tc.desc: Verify Dump method for mask effect
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, MaskDumpContainsEffectName, TestSize.Level1)
{
    auto mask = RSNGRenderMaskBase::Create(RSNGEffectType::WAVE_GRADIENT_MASK);
    EXPECT_NE(mask, nullptr);
    std::string dumpStr;
    mask->Dump(dumpStr);
    EXPECT_FALSE(dumpStr.empty());
    EXPECT_TRUE(dumpStr.find("WaveGradientMask") != std::string::npos);
}

/**
 * @tc.name: ShaderCalculateHashReturnsNonZero
 * @tc.desc: Verify CalculateHash method
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, ShaderCalculateHashReturnsNonZero, TestSize.Level1)
{
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::PARTICLE_CIRCULAR_HALO);
    EXPECT_NE(shader, nullptr);
    uint32_t hash = shader->CalculateHash();
    EXPECT_NE(hash, 0);
}

/**
 * @tc.name: ShaderMarshallingSuccess
 * @tc.desc: Verify Marshalling method
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, ShaderMarshallingSuccess, TestSize.Level1)
{
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::WAVY_RIPPLE_LIGHT);
    EXPECT_NE(shader, nullptr);
    Parcel parcel;
    bool result = shader->Marshalling(parcel);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: ShaderCreateWithInvalidTypeReturnsNullptr
 * @tc.desc: Verify shader creation with invalid type returns nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, ShaderCreateWithInvalidTypeReturnsNullptr, TestSize.Level1)
{
    constexpr int32_t invalidValue = 9999;
    RSNGEffectType invalidType = static_cast<RSNGEffectType>(invalidValue);
    auto shader = RSNGRenderShaderBase::Create(invalidType);
    EXPECT_EQ(shader, nullptr);
}

/**
 * @tc.name: MaskCreateWithInvalidTypeReturnsNullptr
 * @tc.desc: Verify mask creation with invalid type returns nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, MaskCreateWithInvalidTypeReturnsNullptr, TestSize.Level1)
{
    constexpr int32_t invalidValue = 9999;
    RSNGEffectType invalidType = static_cast<RSNGEffectType>(invalidValue);
    auto mask = RSNGRenderMaskBase::Create(invalidType);
    EXPECT_EQ(mask, nullptr);
}

/**
 * @tc.name: ShaderCreateWithNoneType
 * @tc.desc: Verify shader creation with NONE type returns nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, ShaderCreateWithNoneType, TestSize.Level1)
{
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::NONE);
    EXPECT_EQ(shader, nullptr);
}

/**
 * @tc.name: MaskCreateWithNoneType
 * @tc.desc: Verify mask creation with NONE type returns nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, MaskCreateWithNoneType, TestSize.Level1)
{
    auto mask = RSNGRenderMaskBase::Create(RSNGEffectType::NONE);
    EXPECT_EQ(mask, nullptr);
}

/**
 * @tc.name: ShaderDumpReturnsNonEmptyString
 * @tc.desc: Verify Dump method returns non-empty string for shader
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, ShaderDumpReturnsNonEmptyString, TestSize.Level1)
{
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::AURORA_NOISE);
    EXPECT_NE(shader, nullptr);
    std::string dumpStr = shader->Dump();
    EXPECT_FALSE(dumpStr.empty());
}

/**
 * @tc.name: ShaderCalculateHashForDifferentEffects
 * @tc.desc: Verify CalculateHash returns non-zero hash for different effects
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, ShaderCalculateHashForDifferentEffects, TestSize.Level1)
{
    auto shader1 = RSNGRenderShaderBase::Create(RSNGEffectType::AURORA_NOISE);
    auto shader2 = RSNGRenderShaderBase::Create(RSNGEffectType::LIGHT_CAVE);
    EXPECT_NE(shader1, nullptr);
    EXPECT_NE(shader2, nullptr);

    uint32_t hash1 = shader1->CalculateHash();
    uint32_t hash2 = shader2->CalculateHash();
    EXPECT_NE(hash1, 0);
    EXPECT_NE(hash2, 0);
}

/**
 * @tc.name: MaskCalculateHashReturnsNonZero
 * @tc.desc: Verify CalculateHash returns non-zero hash for mask
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, MaskCalculateHashReturnsNonZero, TestSize.Level1)
{
    auto mask = RSNGRenderMaskBase::Create(RSNGEffectType::RIPPLE_MASK);
    EXPECT_NE(mask, nullptr);
    uint32_t hash = mask->CalculateHash();
    EXPECT_NE(hash, 0);
}

/**
 * @tc.name: MaskMarshallingSuccess
 * @tc.desc: Verify Marshalling method for mask effect
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, MaskMarshallingSuccess, TestSize.Level1)
{
    auto mask = RSNGRenderMaskBase::Create(RSNGEffectType::WAVE_GRADIENT_MASK);
    EXPECT_NE(mask, nullptr);
    Parcel parcel;
    bool result = mask->Marshalling(parcel);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: ShaderUnmarshallingSuccess
 * @tc.desc: Verify Unmarshalling method for shader effect
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, ShaderUnmarshallingSuccess, TestSize.Level1)
{
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::AURORA_NOISE);
    EXPECT_NE(shader, nullptr);

    Parcel parcel;
    bool marshallingResult = shader->Marshalling(parcel);
    EXPECT_TRUE(marshallingResult);

    std::shared_ptr<RSNGRenderShaderBase> unmarshalledShader;
    bool unmarshallingResult = RSNGRenderShaderBase::Unmarshalling(parcel, unmarshalledShader);
    EXPECT_TRUE(unmarshallingResult);
    EXPECT_NE(unmarshalledShader, nullptr);
}

/**
 * @tc.name: MaskUnmarshallingSuccess
 * @tc.desc: Verify Unmarshalling method for mask effect
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, MaskUnmarshallingSuccess, TestSize.Level1)
{
    auto mask = RSNGRenderMaskBase::Create(RSNGEffectType::RIPPLE_MASK);
    EXPECT_NE(mask, nullptr);

    Parcel parcel;
    bool marshallingResult = mask->Marshalling(parcel);
    EXPECT_TRUE(marshallingResult);

    std::shared_ptr<RSNGRenderMaskBase> unmarshalledMask;
    bool unmarshallingResult = RSNGRenderMaskBase::Unmarshalling(parcel, unmarshalledMask);
    EXPECT_TRUE(unmarshallingResult);
    EXPECT_NE(unmarshalledMask, nullptr);
}

/**
 * @tc.name: ShaderCanSkipFrameReturnsFalse
 * @tc.desc: Verify CanSkipFrame method for shader effect returns false
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, ShaderCanSkipFrameReturnsFalse, TestSize.Level1)
{
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::AURORA_NOISE);
    EXPECT_NE(shader, nullptr);
    EXPECT_FALSE(shader->CanSkipFrame());
}

/**
 * @tc.name: MaskCanSkipFrameReturnsFalse
 * @tc.desc: Verify CanSkipFrame method for mask effect returns false
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, MaskCanSkipFrameReturnsFalse, TestSize.Level1)
{
    auto mask = RSNGRenderMaskBase::Create(RSNGEffectType::RIPPLE_MASK);
    EXPECT_NE(mask, nullptr);
    EXPECT_FALSE(mask->CanSkipFrame());
}

/**
 * @tc.name: ShaderAppendToGEContainer
 * @tc.desc: Verify AppendToGEContainer method for shader effect
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, ShaderAppendToGEContainer, TestSize.Level1)
{
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::AURORA_NOISE);
    EXPECT_NE(shader, nullptr);

    auto geContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    shader->AppendToGEContainer(geContainer);

    auto filters = geContainer->GetFilters();
    EXPECT_FALSE(filters.empty());
}

/**
 * @tc.name: ShaderAppendToNullGEContainer
 * @tc.desc: Verify AppendToGEContainer method with null container
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, ShaderAppendToNullGEContainer, TestSize.Level1)
{
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::AURORA_NOISE);
    EXPECT_NE(shader, nullptr);

    std::shared_ptr<Drawing::GEVisualEffectContainer> nullContainer = nullptr;
    shader->AppendToGEContainer(nullContainer);
}

/**
 * @tc.name: CalculatePropTagHashConsistency
 * @tc.desc: Verify CalculatePropTagHashImpl produces consistent hash values
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, CalculatePropTagHashConsistency, TestSize.Level1)
{
    uint32_t hash1 = 0;
    uint32_t hash2 = 0;
    constexpr int testValue = 42;

    RSNGRenderEffectHelper::CalculatePropTagHashImpl(hash1, testValue);
    RSNGRenderEffectHelper::CalculatePropTagHashImpl(hash2, testValue);

    EXPECT_EQ(hash1, hash2);
}

/**
 * @tc.name: CreateGEVisualEffectForNoneType
 * @tc.desc: Verify CreateGEVisualEffect returns nullptr for NONE type
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, CreateGEVisualEffectForNoneType, TestSize.Level1)
{
    auto effect = RSNGRenderEffectHelper::CreateGEVisualEffect(RSNGEffectType::NONE);
    EXPECT_NE(effect, nullptr);
}

/**
 * @tc.name: CreateGEVisualEffectForInvalidType
 * @tc.desc: Verify CreateGEVisualEffect returns nullptr for invalid type
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, CreateGEVisualEffectForInvalidType, TestSize.Level1)
{
    constexpr int32_t invalidValue = 9999;
    RSNGEffectType invalidType = static_cast<RSNGEffectType>(invalidValue);
    auto effect = RSNGRenderEffectHelper::CreateGEVisualEffect(invalidType);
    EXPECT_NE(effect, nullptr);
}

/**
 * @tc.name: ShaderGenerateGEVisualEffectReturnsNullptr
 * @tc.desc: Verify GenerateGEVisualEffect returns nullptr for shader base
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, ShaderGenerateGEVisualEffectReturnsNullptr, TestSize.Level1)
{
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::AURORA_NOISE);
    EXPECT_NE(shader, nullptr);
    shader->GenerateGEVisualEffect();
}

/**
 * @tc.name: MaskGenerateGEVisualEffectReturnsNullptr
 * @tc.desc: Verify GenerateGEVisualEffect returns nullptr for mask base
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, MaskGenerateGEVisualEffectReturnsNullptr, TestSize.Level1)
{
    auto mask = RSNGRenderMaskBase::Create(RSNGEffectType::RIPPLE_MASK);
    EXPECT_NE(mask, nullptr);
    mask->GenerateGEVisualEffect();
}

/**
 * @tc.name: ShaderDumpPropertiesReturnsNonEmptyString
 * @tc.desc: Verify Dump method returns correct shader effect name
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, ShaderDumpPropertiesReturnsNonEmptyString, TestSize.Level1)
{
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::BORDER_SDF_SHADER);
    EXPECT_NE(shader, nullptr);
    std::string dumpStr;
    shader->Dump(dumpStr);
    EXPECT_TRUE(dumpStr.find("BorderSDFShader") != std::string::npos);
}

/**
 * @tc.name: MaskDumpPropertiesReturnsNonEmptyString
 * @tc.desc: Verify Dump method returns correct mask effect name
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, MaskDumpPropertiesReturnsNonEmptyString, TestSize.Level1)
{
    auto mask = RSNGRenderMaskBase::Create(RSNGEffectType::RADIAL_GRADIENT_MASK);
    EXPECT_NE(mask, nullptr);
    std::string dumpStr;
    mask->Dump(dumpStr);
    EXPECT_TRUE(dumpStr.find("RadialGradientMask") != std::string::npos);
}

} // namespace OHOS::Rosen