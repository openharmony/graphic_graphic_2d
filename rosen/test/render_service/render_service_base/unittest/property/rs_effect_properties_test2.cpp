/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>

#include "property/rs_effect_properties.h"

#include "effect/rs_render_shader_base.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class EffectPropertiesTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void EffectPropertiesTest::SetUpTestCase() {}
void EffectPropertiesTest::TearDownTestCase() {}
void EffectPropertiesTest::SetUp() {}
void EffectPropertiesTest::TearDown() {}

/**
 * @tc.name: ComplexShaderParam001
 * @tc.desc: test SetComplexShaderParam/GetComplexShaderParam with and without shader effect params
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(EffectPropertiesTest, ComplexShaderParam001, TestSize.Level1)
{
    RSEffectProperties properties;
    EXPECT_FALSE(properties.GetComplexShaderParam().has_value());

    std::vector<float> param { 0.5f, 0.5f };
    properties.SetComplexShaderParam(param);
    EXPECT_EQ(properties.GetComplexShaderParam(), param);

    std::vector<float> param2 { 0.1f, 0.2f, 0.3f };
    properties.SetComplexShaderParam(param2);
    EXPECT_EQ(properties.GetComplexShaderParam(), param2);
}

/**
 * @tc.name: NGRenderShader001
 * @tc.desc: test Set/Get of background/foreground/material/coverage/overlay NG shaders
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(EffectPropertiesTest, NGRenderShader001, TestSize.Level1)
{
    RSEffectProperties properties;
    EXPECT_EQ(properties.GetBackgroundNGShader(), nullptr);
    EXPECT_EQ(properties.GetForegroundShader(), nullptr);
    EXPECT_EQ(properties.GetMaterialShader(), nullptr);
    EXPECT_EQ(properties.GetCoverageNGShader(), nullptr);
    EXPECT_EQ(properties.GetOverlayNGShader(), nullptr);

    auto bgShader = RSNGRenderShaderBase::Create(RSNGEffectType::BORDER_LIGHT);
    properties.SetBackgroundNGShader(bgShader);
    EXPECT_EQ(properties.GetBackgroundNGShader(), bgShader);
    auto fgShader = RSNGRenderShaderBase::Create(RSNGEffectType::BORDER_LIGHT);
    properties.SetForegroundShader(fgShader);
    EXPECT_EQ(properties.GetForegroundShader(), fgShader);
    auto mtShader = RSNGRenderShaderBase::Create(RSNGEffectType::BORDER_LIGHT);
    properties.SetMaterialShader(mtShader);
    EXPECT_EQ(properties.GetMaterialShader(), mtShader);
    auto coShader = RSNGRenderShaderBase::Create(RSNGEffectType::BORDER_LIGHT);
    properties.SetCoverageNGShader(coShader);
    EXPECT_EQ(properties.GetCoverageNGShader(), coShader);
    auto olShader = RSNGRenderShaderBase::Create(RSNGEffectType::BORDER_LIGHT);
    properties.SetOverlayNGShader(olShader);
    EXPECT_EQ(properties.GetOverlayNGShader(), olShader);

    properties.SetBackgroundNGShader(nullptr);
    properties.SetForegroundShader(nullptr);
    properties.SetMaterialShader(nullptr);
    properties.SetCoverageNGShader(nullptr);
    properties.SetOverlayNGShader(nullptr);
    EXPECT_EQ(properties.GetBackgroundNGShader(), nullptr);
    EXPECT_EQ(properties.GetForegroundShader(), nullptr);
    EXPECT_EQ(properties.GetMaterialShader(), nullptr);
    EXPECT_EQ(properties.GetCoverageNGShader(), nullptr);
    EXPECT_EQ(properties.GetOverlayNGShader(), nullptr);
}

/**
 * @tc.name: LightSource001
 * @tc.desc: test GetLightSource/GetIlluminated and SetLightIntensity valid/invalid branches
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(EffectPropertiesTest, LightSource001, TestSize.Level1)
{
    RSEffectProperties properties;
    EXPECT_EQ(properties.GetLightSource(), nullptr);
    EXPECT_EQ(properties.GetIlluminated(), nullptr);

    // invalid intensity creates light source but skips intensity
    EXPECT_FALSE(properties.SetLightIntensity(INVALID_INTENSITY));
    EXPECT_NE(properties.GetLightSource(), nullptr);
    EXPECT_FALSE(properties.SetLightIntensity(INVALID_INTENSITY));

    // valid intensity
    EXPECT_TRUE(properties.SetLightIntensity(1.f));
    EXPECT_EQ(properties.GetLightSource()->GetLightIntensity(), 1.f);
}

/**
 * @tc.name: LightSourceParams001
 * @tc.desc: test SetLightColor and SetLightPosition with lazy light source creation
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(EffectPropertiesTest, LightSourceParams001, TestSize.Level1)
{
    RSEffectProperties properties;
    Color color = Color::FromArgbInt(0xFFFF0000);
    properties.SetLightColor(color);
    EXPECT_EQ(properties.GetLightSource()->GetLightColor(), color);

    Vector4f position { 1.f, 2.f, 3.f, 4.f };
    properties.SetLightPosition(position);
    EXPECT_EQ(properties.GetLightSource()->GetLightPosition(), position);
}

/**
 * @tc.name: Illuminated001
 * @tc.desc: test SetIlluminatedBorderWidth/SetIlluminatedType/SetBloom with lazy creation and INVALID skip branch
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(EffectPropertiesTest, Illuminated001, TestSize.Level1)
{
    RSEffectProperties properties;
    // creates illuminated and sets border width
    properties.SetIlluminatedBorderWidth(1.f);
    EXPECT_EQ(properties.GetIlluminated()->GetIlluminatedBorderWidth(), 1.f);

    // INVALID type is skipped
    properties.SetIlluminatedType(static_cast<int>(IlluminatedType::INVALID));
    EXPECT_EQ(properties.GetIlluminated()->GetIlluminatedType(), IlluminatedType::NONE);

    // valid type
    properties.SetIlluminatedType(static_cast<int>(IlluminatedType::BORDER));
    EXPECT_EQ(properties.GetIlluminated()->GetIlluminatedType(), IlluminatedType::BORDER);

    // bloom intensity
    properties.SetBloom(0.5f);
    EXPECT_EQ(properties.GetIlluminated()->GetBloomIntensity(), 0.5f);
}

/**
 * @tc.name: ParticleParams001
 * @tc.desc: test SetParticles/GetParticles/SetEmitterUpdater/GetEmitterUpdater/
 *           SetParticleNoiseFields/GetParticleNoiseFields with and without shader effect params
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(EffectPropertiesTest, ParticleParams001, TestSize.Level1)
{
    RSEffectProperties properties;
    EXPECT_TRUE(properties.GetParticles().renderParticleVector_.empty());
    EXPECT_TRUE(properties.GetEmitterUpdater().empty());
    EXPECT_EQ(properties.GetParticleNoiseFields(), nullptr);

    RSRenderParticleVector particles;
    auto particleParams = std::make_shared<ParticleRenderParams>();
    particles.renderParticleVector_.push_back(std::make_shared<RSRenderParticle>(particleParams));
    properties.SetParticles(particles);
    EXPECT_EQ(properties.GetParticles().renderParticleVector_.size(), particles.renderParticleVector_.size());

    std::vector<std::shared_ptr<EmitterUpdater>> emitters;
    emitters.push_back(std::make_shared<EmitterUpdater>(0));
    properties.SetEmitterUpdater(emitters);
    EXPECT_EQ(properties.GetEmitterUpdater().size(), emitters.size());

    auto noiseFields = std::make_shared<ParticleNoiseFields>();
    properties.SetParticleNoiseFields(noiseFields);
    EXPECT_EQ(properties.GetParticleNoiseFields(), noiseFields);

    // reset branches
    properties.SetParticleNoiseFields(nullptr);
    EXPECT_EQ(properties.GetParticleNoiseFields(), nullptr);
    properties.SetParticles(RSRenderParticleVector());
    EXPECT_TRUE(properties.GetParticles().renderParticleVector_.empty());
    properties.SetEmitterUpdater(std::vector<std::shared_ptr<EmitterUpdater>>());
    EXPECT_TRUE(properties.GetEmitterUpdater().empty());
}

/**
 * @tc.name: DepthParams001
 * @tc.desc: test SetDepthImage/GetDepthImage/SetDepthCameraPara/GetDepthCameraPara/SetDepthLightPara/
 *           GetDepthLightPara/SetDepthImageMatrix/GetDepthImageMatrix with and without depth effect params
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(EffectPropertiesTest, DepthParams001, TestSize.Level1)
{
    RSEffectProperties properties;
    EXPECT_EQ(properties.GetDepthImage(), nullptr);
    EXPECT_FALSE(properties.GetDepthCameraPara().has_value());
    EXPECT_FALSE(properties.GetDepthLightPara().has_value());
    EXPECT_FALSE(properties.GetDepthImageMatrix().has_value());

    auto image = std::make_shared<RSImage>();
    properties.SetDepthImage(image);
    EXPECT_EQ(properties.GetDepthImage(), image);

    DepthCameraPara cameraPara;
    cameraPara.yFov = 45.f;
    properties.SetDepthCameraPara(cameraPara);
    EXPECT_EQ(properties.GetDepthCameraPara(), cameraPara);

    DepthLightPara lightPara;
    lightPara.intensity = 0.5f;
    properties.SetDepthLightPara(lightPara);
    EXPECT_EQ(properties.GetDepthLightPara(), lightPara);

    Matrix3f imageMatrix;
    imageMatrix[0][0] = 1.f;
    properties.SetDepthImageMatrix(imageMatrix);
    EXPECT_EQ(properties.GetDepthImageMatrix(), imageMatrix);
}

/**
 * @tc.name: SpatialEffectDepth001
 * @tc.desc: test SetSpatialEffectDepth branches: no para, existing depth para and existing perspective para
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(EffectPropertiesTest, SpatialEffectDepth001, TestSize.Level1)
{
    RSEffectProperties properties;
    // no para branch, creates depth para
    properties.SetSpatialEffectDepth(0.5f);
    auto depthPara = properties.GetDepthEffectPara();
    ASSERT_TRUE(depthPara.has_value());
    EXPECT_EQ(depthPara->depth, 0.5f);
    EXPECT_EQ(depthPara->occlusionWeight, 0.f);
    EXPECT_FALSE(properties.GetSpatialEffectPara().has_value());

    // existing depth para branch, kept
    properties.SetSpatialEffectDepth(0.7f);
    depthPara = properties.GetDepthEffectPara();
    ASSERT_TRUE(depthPara.has_value());
    EXPECT_EQ(depthPara->depth, 0.7f);

    // existing perspective para branch, replaced by depth para
    Vector3f corner { 1.f, 1.f, 1.f };
    properties.SetSpatialEffectLeftTop(corner);
    EXPECT_TRUE(properties.GetSpatialEffectPara().has_value());
    properties.SetSpatialEffectDepth(0.8f);
    EXPECT_FALSE(properties.GetSpatialEffectPara().has_value());
    depthPara = properties.GetDepthEffectPara();
    ASSERT_TRUE(depthPara.has_value());
    EXPECT_EQ(depthPara->depth, 0.8f);
}

/**
 * @tc.name: SpatialEffectCorners001
 * @tc.desc: test SetSpatialEffectLeftTop/RightTop/LeftBottom/RightBottom branches and GetSpatialEffectPara
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(EffectPropertiesTest, SpatialEffectCorners001, TestSize.Level1)
{
    RSEffectProperties properties;
    Vector3f leftTop { 1.f, 1.f, 1.f };
    // no para branch, creates spatial effect para
    properties.SetSpatialEffectLeftTop(leftTop);
    auto spatialPara = properties.GetSpatialEffectPara();
    ASSERT_TRUE(spatialPara.has_value());
    EXPECT_EQ(spatialPara->corners[SpatialEffectPara::LEFT_TOP_INDEX], leftTop);
    EXPECT_EQ(spatialPara->spatialEffectMode, SpatialEffectMode::WORLD_XYZ_MODE);
    EXPECT_FALSE(properties.GetDepthEffectPara().has_value());

    // existing perspective para branch, kept
    Vector3f rightTop { 2.f, 2.f, 2.f };
    properties.SetSpatialEffectRightTop(rightTop);
    Vector3f leftBottom { 3.f, 3.f, 3.f };
    properties.SetSpatialEffectLeftBottom(leftBottom);
    Vector3f rightBottom { 4.f, 4.f, 4.f };
    properties.SetSpatialEffectRightBottom(rightBottom);
    spatialPara = properties.GetSpatialEffectPara();
    ASSERT_TRUE(spatialPara.has_value());
    EXPECT_EQ(spatialPara->corners[SpatialEffectPara::RIGHT_TOP_INDEX], rightTop);
    EXPECT_EQ(spatialPara->corners[SpatialEffectPara::LEFT_BOTTOM_INDEX], leftBottom);
    EXPECT_EQ(spatialPara->corners[SpatialEffectPara::RIGHT_BOTTOM_INDEX], rightBottom);
    EXPECT_EQ(spatialPara->corners[SpatialEffectPara::LEFT_TOP_INDEX], leftTop);

    // existing depth para branch, replaced by spatial effect para
    properties.SetSpatialEffectDepth(0.5f);
    properties.SetSpatialEffectLeftTop(leftTop);
    spatialPara = properties.GetSpatialEffectPara();
    ASSERT_TRUE(spatialPara.has_value());
    EXPECT_EQ(spatialPara->corners[SpatialEffectPara::LEFT_TOP_INDEX], leftTop);
}

/**
 * @tc.name: SpatialEffectOcclusion001
 * @tc.desc: test SetSpatialEffectOcclusionWeight and GetSpatialEffectOcclusionEnabled branches
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(EffectPropertiesTest, SpatialEffectOcclusion001, TestSize.Level1)
{
    RSEffectProperties properties;
    EXPECT_FALSE(properties.GetSpatialEffectOcclusionEnabled());

    // depth effect exists but no spatial effect para
    properties.SetDepthImage(std::make_shared<RSImage>());
    EXPECT_FALSE(properties.GetSpatialEffectOcclusionEnabled());

    // creates variant para, weight 0 is not enabled
    properties.SetSpatialEffectOcclusionWeight(0.f);
    EXPECT_FALSE(properties.GetSpatialEffectOcclusionEnabled());

    // existing para branch, weight greater than 0 is enabled
    properties.SetSpatialEffectOcclusionWeight(0.5f);
    EXPECT_TRUE(properties.GetSpatialEffectOcclusionEnabled());

    // depth para with occlusion weight is returned by GetDepthEffectPara
    auto depthPara = properties.GetDepthEffectPara();
    ASSERT_TRUE(depthPara.has_value());
    EXPECT_EQ(depthPara->occlusionWeight, 0.5f);
}

/**
 * @tc.name: SpatialEffectMode001
 * @tc.desc: test SetSpatialEffectMode with and without existing variant para
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(EffectPropertiesTest, SpatialEffectMode001, TestSize.Level1)
{
    RSEffectProperties properties;
    // no para branch, creates variant para
    properties.SetSpatialEffectMode(static_cast<int>(SpatialEffectMode::NDC_XY_WORLD_Z_MODE));
    auto variantPara = properties.GetSpatialEffectVariantPara();
    ASSERT_TRUE(variantPara.has_value());
    EXPECT_EQ(variantPara->spatialEffectMode, SpatialEffectMode::NDC_XY_WORLD_Z_MODE);

    // existing para branch
    properties.SetSpatialEffectMode(static_cast<int>(SpatialEffectMode::WORLD_XYZ_MODE));
    variantPara = properties.GetSpatialEffectVariantPara();
    ASSERT_TRUE(variantPara.has_value());
    EXPECT_EQ(variantPara->spatialEffectMode, SpatialEffectMode::WORLD_XYZ_MODE);
}

/**
 * @tc.name: SpatialEffectVariantPara001
 * @tc.desc: test SetSpatialEffectVariantPara/GetSpatialEffectVariantPara with and without depth effect params
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(EffectPropertiesTest, SpatialEffectVariantPara001, TestSize.Level1)
{
    RSEffectProperties properties;
    EXPECT_FALSE(properties.GetSpatialEffectVariantPara().has_value());

    SpatialEffectVariantPara variantPara;
    variantPara.occlusionWeight = 0.5f;
    properties.SetSpatialEffectVariantPara(variantPara);
    ASSERT_TRUE(properties.GetSpatialEffectVariantPara().has_value());
    EXPECT_EQ(properties.GetSpatialEffectVariantPara(), variantPara);

    properties.SetSpatialEffectVariantPara(std::nullopt);
    EXPECT_FALSE(properties.GetSpatialEffectVariantPara().has_value());
}

/**
 * @tc.name: SpatialEffectPara001
 * @tc.desc: test SetSpatialEffectPara/GetSpatialEffectPara branches
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(EffectPropertiesTest, SpatialEffectPara001, TestSize.Level1)
{
    RSEffectProperties properties;
    EXPECT_FALSE(properties.GetSpatialEffectPara().has_value());

    // no para, depth effect exists
    properties.SetDepthImage(std::make_shared<RSImage>());
    EXPECT_FALSE(properties.GetSpatialEffectPara().has_value());

    SpatialEffectPara spatialPara;
    spatialPara.spatialEffectMode = SpatialEffectMode::NDC_XY_WORLD_Z_MODE;
    spatialPara.occlusionWeight = 0.5f;
    spatialPara.corners[SpatialEffectPara::LEFT_TOP_INDEX] = Vector3f(1.f, 1.f, 1.f);
    spatialPara.xyzCorners[SpatialEffectPara::RIGHT_TOP_INDEX] = Vector3f(2.f, 2.f, 2.f);
    properties.SetSpatialEffectPara(spatialPara);
    auto gotPara = properties.GetSpatialEffectPara();
    ASSERT_TRUE(gotPara.has_value());
    EXPECT_EQ(gotPara->spatialEffectMode, SpatialEffectMode::NDC_XY_WORLD_Z_MODE);
    EXPECT_EQ(gotPara->occlusionWeight, 0.5f);
    EXPECT_EQ(gotPara->corners[SpatialEffectPara::LEFT_TOP_INDEX], Vector3f(1.f, 1.f, 1.f));
    EXPECT_EQ(gotPara->xyzCorners[SpatialEffectPara::RIGHT_TOP_INDEX], Vector3f(2.f, 2.f, 2.f));

    // depth para is not perspective, returns nullopt
    DepthEffectPara depthPara;
    depthPara.depth = 0.5f;
    properties.SetDepthEffectPara(depthPara);
    EXPECT_FALSE(properties.GetSpatialEffectPara().has_value());
}

/**
 * @tc.name: DepthEffectPara001
 * @tc.desc: test SetDepthEffectPara/GetDepthEffectPara branches
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(EffectPropertiesTest, DepthEffectPara001, TestSize.Level1)
{
    RSEffectProperties properties;
    EXPECT_FALSE(properties.GetDepthEffectPara().has_value());

    // depth effect exists but para is nullopt
    properties.SetDepthImage(std::make_shared<RSImage>());
    EXPECT_FALSE(properties.GetDepthEffectPara().has_value());

    // depth para
    DepthEffectPara depthPara;
    depthPara.depth = 0.5f;
    depthPara.occlusionWeight = 0.2f;
    properties.SetDepthEffectPara(depthPara);
    auto gotPara = properties.GetDepthEffectPara();
    ASSERT_TRUE(gotPara.has_value());
    EXPECT_EQ(gotPara->depth, 0.5f);
    EXPECT_EQ(gotPara->occlusionWeight, 0.2f);

    // reset to nullopt
    properties.SetDepthEffectPara(std::nullopt);
    EXPECT_FALSE(properties.GetDepthEffectPara().has_value());
}

/**
 * @tc.name: SpatialEffectDstPoints001
 * @tc.desc: test SetSpatialEffectDstPoints/GetSpatialEffectDstPoints with and without depth effect params
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(EffectPropertiesTest, SpatialEffectDstPoints001, TestSize.Level1)
{
    RSEffectProperties properties;
    EXPECT_FALSE(properties.GetSpatialEffectDstPoints().has_value());

    std::vector<Drawing::Point> dstPoints { Drawing::Point(1.f, 2.f), Drawing::Point(3.f, 4.f) };
    properties.SetSpatialEffectDstPoints(dstPoints);
    ASSERT_TRUE(properties.GetSpatialEffectDstPoints().has_value());
    EXPECT_EQ(properties.GetSpatialEffectDstPoints()->size(), dstPoints.size());

    properties.SetSpatialEffectDstPoints(std::nullopt);
    EXPECT_FALSE(properties.GetSpatialEffectDstPoints().has_value());
}

/**
 * @tc.name: SpatialEffectXyzCornerPoints001
 * @tc.desc: test SetSpatialEffectXyzCornerPoints with and without existing variant para
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(EffectPropertiesTest, SpatialEffectXyzCornerPoints001, TestSize.Level1)
{
    RSEffectProperties properties;
    // no para branch, does nothing
    SpatialEffectPara::CornerPositions cornerPoints;
    cornerPoints[SpatialEffectPara::LEFT_TOP_INDEX] = Vector3f(1.f, 1.f, 1.f);
    properties.SetSpatialEffectXyzCornerPoints(cornerPoints);
    EXPECT_FALSE(properties.GetSpatialEffectVariantPara().has_value());

    // existing para branch
    SpatialEffectVariantPara variantPara;
    properties.SetSpatialEffectVariantPara(variantPara);
    properties.SetSpatialEffectXyzCornerPoints(cornerPoints);
    ASSERT_TRUE(properties.GetSpatialEffectVariantPara().has_value());
    EXPECT_EQ(properties.GetSpatialEffectVariantPara()->xyzCornerPoints[SpatialEffectPara::LEFT_TOP_INDEX],
        Vector3f(1.f, 1.f, 1.f));
}
} // namespace Rosen
} // namespace OHOS
