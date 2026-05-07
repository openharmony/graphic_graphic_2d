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
 * @tc.name: GetEffectTypeString_001
 * @tc.desc: Verify GetEffectTypeString for all effect types
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, GetEffectTypeString_001, TestSize.Level1)
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
 * @tc.name: GetEffectTypeString_002
 * @tc.desc: Verify GetEffectTypeString for mask types
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, GetEffectTypeString_002, TestSize.Level1)
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
}

/**
 * @tc.name: GetEffectTypeString_003
 * @tc.desc: Verify GetEffectTypeString for SDF shape types
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, GetEffectTypeString_003, TestSize.Level1)
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
}

/**
 * @tc.name: GetEffectTypeString_004
 * @tc.desc: Verify GetEffectTypeString for shader effect types
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, GetEffectTypeString_004, TestSize.Level1)
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
}

/**
 * @tc.name: GetEffectTypeString_005
 * @tc.desc: Verify GetEffectTypeString for glow and flowlight types
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, GetEffectTypeString_005, TestSize.Level1)
{
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::AIBAR_GLOW), "AIBarGlow");
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::AIBAR_RECT_HALO), "AIBarRectHalo");
    EXPECT_EQ(
        RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::ROUNDED_RECT_FLOWLIGHT), "RoundedRectFlowlight");
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::GRADIENT_FLOW_COLORS), "GradientFlowColors");
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::CIRCLE_FLOWLIGHT), "CircleFlowlight");
}

/**
 * @tc.name: GetEffectTypeString_006
 * @tc.desc: Verify GetEffectTypeString for frosted glass types
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, GetEffectTypeString_006, TestSize.Level1)
{
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::FROSTED_GLASS), "FrostedGlass");
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::FROSTED_GLASS_EFFECT), "FrostedGlassEffect");
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::FROSTED_GLASS_BLUR), "FrostedGlassBlur");
}

/**
 * @tc.name: GetEffectTypeString_007
 * @tc.desc: Verify GetEffectTypeString for unknown type
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, GetEffectTypeString_007, TestSize.Level1)
{
    constexpr int32_t unknownValue = 999;
    RSNGEffectType unknownType = static_cast<RSNGEffectType>(unknownValue);
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(unknownType), "UNKNOWN");
}

/**
 * @tc.name: CreateGEVisualEffect_001
 * @tc.desc: Verify CreateGEVisualEffect for blur type
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, CreateGEVisualEffect_001, TestSize.Level1)
{
    auto geEffect = RSNGRenderEffectHelper::CreateGEVisualEffect(RSNGEffectType::BLUR);
    EXPECT_NE(geEffect, nullptr);
}

/**
 * @tc.name: CreateGEVisualEffect_002
 * @tc.desc: Verify CreateGEVisualEffect for displacement distort type
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, CreateGEVisualEffect_002, TestSize.Level1)
{
    auto geEffect = RSNGRenderEffectHelper::CreateGEVisualEffect(RSNGEffectType::DISPLACEMENT_DISTORT);
    EXPECT_NE(geEffect, nullptr);
}

/**
 * @tc.name: CreateGEVisualEffect_003
 * @tc.desc: Verify CreateGEVisualEffect for sound wave type
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, CreateGEVisualEffect_003, TestSize.Level1)
{
    auto geEffect = RSNGRenderEffectHelper::CreateGEVisualEffect(RSNGEffectType::SOUND_WAVE);
    EXPECT_NE(geEffect, nullptr);
}

/**
 * @tc.name: CreateGEVisualEffect_004
 * @tc.desc: Verify CreateGEVisualEffect for edge light type
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, CreateGEVisualEffect_004, TestSize.Level1)
{
    auto geEffect = RSNGRenderEffectHelper::CreateGEVisualEffect(RSNGEffectType::EDGE_LIGHT);
    EXPECT_NE(geEffect, nullptr);
}

/**
 * @tc.name: CreateGEVisualEffect_005
 * @tc.desc: Verify CreateGEVisualEffect for dispersion type
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, CreateGEVisualEffect_005, TestSize.Level1)
{
    auto geEffect = RSNGRenderEffectHelper::CreateGEVisualEffect(RSNGEffectType::DISPERSION);
    EXPECT_NE(geEffect, nullptr);
}

/**
 * @tc.name: CreateGEVisualEffect_006
 * @tc.desc: Verify CreateGEVisualEffect for mask types
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, CreateGEVisualEffect_006, TestSize.Level1)
{
    auto rippleMask = RSNGRenderEffectHelper::CreateGEVisualEffect(RSNGEffectType::RIPPLE_MASK);
    EXPECT_NE(rippleMask, nullptr);

    auto doubleRippleMask = RSNGRenderEffectHelper::CreateGEVisualEffect(RSNGEffectType::DOUBLE_RIPPLE_MASK);
    EXPECT_NE(doubleRippleMask, nullptr);

    auto pixelMapMask = RSNGRenderEffectHelper::CreateGEVisualEffect(RSNGEffectType::PIXEL_MAP_MASK);
    EXPECT_NE(pixelMapMask, nullptr);

    auto radialGradientMask = RSNGRenderEffectHelper::CreateGEVisualEffect(RSNGEffectType::RADIAL_GRADIENT_MASK);
    EXPECT_NE(radialGradientMask, nullptr);
}

/**
 * @tc.name: AppendToGEContainer_001
 * @tc.desc: Verify AppendToGEContainer with valid container
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, AppendToGEContainer_001, TestSize.Level1)
{
    auto geContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    auto geEffect = RSNGRenderEffectHelper::CreateGEVisualEffect(RSNGEffectType::BLUR);

    RSNGRenderEffectHelper::AppendToGEContainer(geContainer, geEffect);

    auto filters = geContainer->GetFilters();
    EXPECT_FALSE(filters.empty());
}

/**
 * @tc.name: AppendToGEContainer_002
 * @tc.desc: Verify AppendToGEContainer with null effect
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, AppendToGEContainer_002, TestSize.Level1)
{
    auto geContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    std::shared_ptr<Drawing::GEVisualEffect> nullEffect = nullptr;

    RSNGRenderEffectHelper::AppendToGEContainer(geContainer, nullEffect);
}

/**
 * @tc.name: CalculatePropTagHashImpl_001
 * @tc.desc: Verify CalculatePropTagHashImpl with int value
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, CalculatePropTagHashImpl_001, TestSize.Level1)
{
    uint32_t hash = 0;
    constexpr int testValue = 10;
    RSNGRenderEffectHelper::CalculatePropTagHashImpl(hash, testValue);
    EXPECT_NE(hash, 0);
}

/**
 * @tc.name: CalculatePropTagHashImpl_002
 * @tc.desc: Verify CalculatePropTagHashImpl with float value
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, CalculatePropTagHashImpl_002, TestSize.Level1)
{
    uint32_t hash = 0;
    constexpr float testValue = 1.5f;
    RSNGRenderEffectHelper::CalculatePropTagHashImpl(hash, testValue);
    EXPECT_NE(hash, 0);
}

/**
 * @tc.name: CalculatePropTagHashImpl_003
 * @tc.desc: Verify CalculatePropTagHashImpl with bool value
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, CalculatePropTagHashImpl_003, TestSize.Level1)
{
    uint32_t hash = 0;
    RSNGRenderEffectHelper::CalculatePropTagHashImpl(hash, true);
    EXPECT_NE(hash, 0);
}

/**
 * @tc.name: CalculatePropTagHashImpl_004
 * @tc.desc: Verify CalculatePropTagHashImpl with Vector2f value
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, CalculatePropTagHashImpl_004, TestSize.Level1)
{
    uint32_t hash = 0;
    Vector2f testValue(1.0f, 2.0f);
    RSNGRenderEffectHelper::CalculatePropTagHashImpl(hash, testValue);
    EXPECT_NE(hash, 0);
}

/**
 * @tc.name: CalculatePropTagHashImpl_005
 * @tc.desc: Verify CalculatePropTagHashImpl with Vector3f value
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, CalculatePropTagHashImpl_005, TestSize.Level1)
{
    uint32_t hash = 0;
    Vector3f testValue(1.0f, 2.0f, 3.0f);
    RSNGRenderEffectHelper::CalculatePropTagHashImpl(hash, testValue);
    EXPECT_NE(hash, 0);
}

/**
 * @tc.name: CalculatePropTagHashImpl_006
 * @tc.desc: Verify CalculatePropTagHashImpl with Vector4f value
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, CalculatePropTagHashImpl_006, TestSize.Level1)
{
    uint32_t hash = 0;
    Vector4f testValue(1.0f, 2.0f, 3.0f, 4.0f);
    RSNGRenderEffectHelper::CalculatePropTagHashImpl(hash, testValue);
    EXPECT_NE(hash, 0);
}

/**
 * @tc.name: GetType_001
 * @tc.desc: Verify GetType for shader base
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, GetType_001, TestSize.Level1)
{
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::CONTOUR_DIAGONAL_FLOW_LIGHT);
    EXPECT_NE(shader, nullptr);
    EXPECT_EQ(shader->GetType(), RSNGEffectType::CONTOUR_DIAGONAL_FLOW_LIGHT);
}

/**
 * @tc.name: GetType_002
 * @tc.desc: Verify GetType for mask base
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, GetType_002, TestSize.Level1)
{
    auto mask = RSNGRenderMaskBase::Create(RSNGEffectType::RIPPLE_MASK);
    EXPECT_NE(mask, nullptr);
    EXPECT_EQ(mask->GetType(), RSNGEffectType::RIPPLE_MASK);
}

/**
 * @tc.name: ContainsType_001
 * @tc.desc: Verify ContainsType for single effect
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, ContainsType_001, TestSize.Level1)
{
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::CONTOUR_DIAGONAL_FLOW_LIGHT);
    EXPECT_NE(shader, nullptr);
    EXPECT_TRUE(shader->ContainsType(RSNGEffectType::CONTOUR_DIAGONAL_FLOW_LIGHT));
    EXPECT_FALSE(shader->ContainsType(RSNGEffectType::BLUR));
}

/**
 * @tc.name: GetEffectCount_001
 * @tc.desc: Verify GetEffectCount for single effect
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, GetEffectCount_001, TestSize.Level1)
{
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::LIGHT_CAVE);
    EXPECT_NE(shader, nullptr);
    EXPECT_EQ(shader->GetEffectCount(), 1);
}

/**
 * @tc.name: Dump_001
 * @tc.desc: Verify Dump method for shader effect
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, Dump_001, TestSize.Level1)
{
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::AURORA_NOISE);
    EXPECT_NE(shader, nullptr);
    std::string dumpStr;
    shader->Dump(dumpStr);
    EXPECT_FALSE(dumpStr.empty());
    EXPECT_TRUE(dumpStr.find("AuroraNoise") != std::string::npos);
}

/**
 * @tc.name: Dump_002
 * @tc.desc: Verify Dump method for mask effect
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, Dump_002, TestSize.Level1)
{
    auto mask = RSNGRenderMaskBase::Create(RSNGEffectType::WAVE_GRADIENT_MASK);
    EXPECT_NE(mask, nullptr);
    std::string dumpStr;
    mask->Dump(dumpStr);
    EXPECT_FALSE(dumpStr.empty());
    EXPECT_TRUE(dumpStr.find("WaveGradientMask") != std::string::npos);
}

/**
 * @tc.name: CalculateHash_001
 * @tc.desc: Verify CalculateHash method
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, CalculateHash_001, TestSize.Level1)
{
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::PARTICLE_CIRCULAR_HALO);
    EXPECT_NE(shader, nullptr);
    uint32_t hash = shader->CalculateHash();
    EXPECT_NE(hash, 0);
}

/**
 * @tc.name: Marshalling_001
 * @tc.desc: Verify Marshalling method
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderEffectTemplateTest, Marshalling_001, TestSize.Level1)
{
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::WAVY_RIPPLE_LIGHT);
    EXPECT_NE(shader, nullptr);
    Parcel parcel;
    bool result = shader->Marshalling(parcel);
    EXPECT_TRUE(result);
}
} // namespace OHOS::Rosen