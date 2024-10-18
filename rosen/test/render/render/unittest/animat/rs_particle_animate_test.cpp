/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "animation/rs_render_particle.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderParticleTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderParticleTest::SetUpTestCase() {}
void RSRenderParticleTest::TearDownTestCase() {}
void RSRenderParticleTest::SetUp() {}
void RSRenderParticleTest::TearDown() {}

class ParticleRenderParamsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void ParticleRenderParamsTest::SetUpTestCase() {}
void ParticleRenderParamsTest::TearDownTestCase() {}
void ParticleRenderParamsTest::SetUp() {}
void ParticleRenderParamsTest::TearDown() {}

/**
 * @tc.name: GetScaleChangeOverLifeUniTest
 * @tc.desc: test results of GetScaleChangeOverLife
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsUniTest GetScaleChangeOverLifeUniTest Level1)
{
    ParticleRenderParams particleRenderParams;
    std::vector<std::shared_ptr<ChangeInOverLife<float>>> res;
    res = particleRenderParams.GetScaleChangeOverLife();
    ASSERT_EQ(res, res);
}

/**
 * @tc.name: GetSpinChangeOverLifeUniTest
 * @tc.desc: test results of GetSpinChangeOverLife
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsUniTest GetSpinChangeOverLifeUniTest Level1)
{
    ParticleRenderParams particleRenderParams;
    std::vector<std::shared_ptr<ChangeInOverLife<float>>> res;
    res = particleRenderParams.GetSpinChangeOverLife();
    ASSERT_EQ(res, res);
}

/**
 * @tc.name: GetColorChangeOverLifeUniTest
 * @tc.desc: test results of GetColorChangeOverLife
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsUniTest GetColorChangeOverLifeUniTest Level1)
{
    ParticleRenderParams particleRenderParams;
    std::vector<std::shared_ptr<ChangeInOverLife<Color>>> res;
    res = particleRenderParams.GetColorChangeOverLife();
    ASSERT_EQ(res, res);
}

/**
 * @tc.name: SetRadiusUniTest
 * @tc.desc: test results of SetRadius
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleUniTest SetRadiusUniTest Level1)
{
    float radius = 1.0f;
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    rsRenderParticle.SetRadius(radius);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetImageUniTest
 * @tc.desc: test results of SetImage
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleUniTest SetImageUniTest Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    auto image = std::make_shared<RSImage>();
    rsRenderParticle.SetImage(image);
    EXPECT_NE(image, nullptr);
}

/**
 * @tc.name: SetImageSizeUniTest
 * @tc.desc: test results of SetImageSize
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleUniTest SetImageSizeUniTest Level1)
{
    Vector2f imageSize;
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    rsRenderParticle.SetImageSize(imageSize);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetParticleTypeUniTest
 * @tc.desc: test results of SetParticleType
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleUniTest SetParticleTypeUniTest Level1)
{
    ParticleType particleType = ParticleType::POINTS;
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    rsRenderParticle.SetParticleType(particleType);
    EXPECT_EQ(rsRenderParticle.particleType_, particleType);
}

/**
 * @tc.name: SetActiveTimeUniTest
 * @tc.desc: test results of SetActiveTime
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleUniTest SetActiveTimeUniTest Level1)
{
    int64_t activeTime = 60;
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    rsRenderParticle.SetActiveTime(activeTime);
    EXPECT_EQ(rsRenderParticle.activeTime_, activeTime);
}

/**
 * @tc.name: SetAccelerationValueUniTest
 * @tc.desc: test results of SetAccelerationValue
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleUniTest SetAccelerationValueUniTest Level1)
{
    float accelerationValue = 0.f;
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    rsRenderParticle.SetAccelerationValue(accelerationValue);
    EXPECT_EQ(rsRenderParticle.accelerationValue_, accelerationValue);
}

/**
 * @tc.name: SetAccelerationAngleUniTest
 * @tc.desc: test results of SetAccelerationAngle
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleUniTest SetAccelerationAngleUniTest Level1)
{
    float accelerationAngle = 0.f;
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    rsRenderParticle.SetAccelerationAngle(accelerationAngle);
    EXPECT_EQ(rsRenderParticle.accelerationAngle_, accelerationAngle);
}

/**
 * @tc.name: SetRedFUniTest
 * @tc.desc: test results of SetRedF
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleUniTest SetRedFUniTest Level1)
{
    float redF = 0.f;
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    rsRenderParticle.SetRedF(redF);
    EXPECT_EQ(rsRenderParticle.redF_, redF);
}

/**
 * @tc.name: InitPropertyUniTest
 * @tc.desc: test results of InitProperty
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleUniTest InitPropertyUniTest Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    rsRenderParticle.InitProperty();
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetPositionUniTest
 * @tc.desc: test results of SetPosition
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleUniTest SetPositionUniTest Level1)
{
    Vector2f position;
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    rsRenderParticle.SetPosition(position);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetVelocityUniTest
 * @tc.desc: test results of SetVelocity
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleUniTest SetVelocityUniTest Level1)
{
    Vector2f velocity;
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    rsRenderParticle.SetVelocity(velocity);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetAccelerationUniTest
 * @tc.desc: test results of SetAcceleration
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleUniTest SetAccelerationUniTest Level1)
{
    Vector2f acceleration;
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    rsRenderParticle.SetAcceleration(acceleration);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetSpinUniTest
 * @tc.desc: test results of SetSpin
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleUniTest SetSpinUniTest Level1)
{
    float spin = 1.0f;
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    rsRenderParticle.SetSpin(spin);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetOpacityUniTest
 * @tc.desc: test results of SetOpacity
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleUniTest SetOpacityUniTest Level1)
{
    float opacity = 1.0f;
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    rsRenderParticle.SetOpacity(opacity);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetColorUniTest
 * @tc.desc: test results of SetColor
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleUniTest SetColorUniTest Level1)
{
    Color color;
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    rsRenderParticle.SetColor(color);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetScaleUniTest
 * @tc.desc: test results of SetScale
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleUniTest SetScaleUniTest Level1)
{
    float scale = 1.0f;
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    rsRenderParticle.SetScale(scale);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetGreenFUniTest
 * @tc.desc: test results of SetGreenF
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleUniTest SetGreenFUniTest Level1)
{
    float greenF = 0.f;
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    rsRenderParticle.SetGreenF(greenF);
    EXPECT_EQ(rsRenderParticle.greenF_, greenF);
}

/**
 * @tc.name: SetBlueFUniTest
 * @tc.desc: test results of SetBlueF
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleUniTest SetBlueFUniTest Level1)
{
    float blueF = 0.f;
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    rsRenderParticle.SetBlueF(blueF);
    EXPECT_EQ(rsRenderParticle.blueF_, blueF);
}

/**
 * @tc.name: SetAlphaFUniTest
 * @tc.desc: test results of SetAlphaF
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleUniTest SetAlphaFUniTest Level1)
{
    float alphaF = 0.f;
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    rsRenderParticle.SetAlphaF(alphaF);
    EXPECT_EQ(rsRenderParticle.alphaF_, alphaF);
}

/**
 * @tc.name: GetPositionUniTest
 * @tc.desc: test results of GetPosition
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleUniTest GetPositionUniTest Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    Vector2f v;
    v = rsRenderParticle.GetPosition();
    EXPECT_EQ(v, v);
}

/**
 * @tc.name: GetGreenSpeedUniTest
 * @tc.desc: test results of GetGreenSpeed
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleUniTest GetGreenSpeedUniTest Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    float res = rsRenderParticle.GetGreenSpeed();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetBlueSpeedUniTest
 * @tc.desc: test results of GetBlueSpeed
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleUniTest GetBlueSpeedUniTest Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    float res = rsRenderParticle.GetBlueSpeed();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetAlphaSpeedUniTest
 * @tc.desc: test results of GetAlphaSpeed
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleUniTest GetAlphaSpeedUniTest Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    float res = rsRenderParticle.GetAlphaSpeed();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetOpacitySpeedUniTest
 * @tc.desc: test results of GetOpacitySpeed
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleUniTest GetOpacitySpeedUniTest Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    float res = rsRenderParticle.GetOpacitySpeed();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetScaleSpeedUniTest
 * @tc.desc: test results of GetScaleSpeed
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleUniTest GetScaleSpeedUniTest Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    float res = rsRenderParticle.GetScaleSpeed();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetSpinSpeedUniTest
 * @tc.desc: test results of GetSpinSpeed
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleUniTest GetSpinSpeedUniTest Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    float res = rsRenderParticle.GetSpinSpeed();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetAccelerationValueSpeedUniTest
 * @tc.desc: test results of GetAccelerationValueSpeed
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleUniTest GetAccelerationValueSpeedUniTest Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    float res = rsRenderParticle.GetAccelerationValueSpeed();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetAccelerationAngleSpeedUniTest
 * @tc.desc: test results of GetAccelerationAngleSpeed
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleUniTest GetAccelerationAngleSpeedUniTest Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    float res = rsRenderParticle.GetAccelerationAngleSpeed();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetVelocityUniTest
 * @tc.desc: test results of GetVelocity
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleUniTest GetVelocityUniTest Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    Vector2f v;
    v = rsRenderParticle.GetVelocity();
    EXPECT_EQ(v, v);
}

/**
 * @tc.name: GetAccelerationUniTest
 * @tc.desc: test results of GetAcceleration
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleUniTest GetAccelerationUniTest Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    Vector2f v;
    v = rsRenderParticle.GetAcceleration();
    EXPECT_EQ(v, v);
}

/**
 * @tc.name: GetSpinUniTest
 * @tc.desc: test results of GetSpin
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleUniTest GetSpinUniTest Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    float res = rsRenderParticle.GetSpin();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetOpacityUniTest
 * @tc.desc: test results of GetOpacity
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleUniTest GetOpacityUniTest Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    float res = rsRenderParticle.GetOpacity();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetColorUniTest
 * @tc.desc: test results of GetColor
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleUniTest GetColorUniTest Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    Color color = rsRenderParticle.GetColor();
    ASSERT_EQ(color, color);
}

/**
 * @tc.name: GetScaleUniTest
 * @tc.desc: test results of GetScale
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleUniTest GetScaleUniTest Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    float res = rsRenderParticle.GetScale();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetRadiusUniTest
 * @tc.desc: test results of GetRadius
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleUniTest GetRadiusUniTest Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    float res = rsRenderParticle.GetRadius();
    ASSERT_NE(res, 1.0f);
}

/**
 * @tc.name: GetAccelerationValueUniTest
 * @tc.desc: test results of GetAccelerationValue
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleUniTest GetAccelerationValueUniTest Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    float res = rsRenderParticle.GetAccelerationValue();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetAccelerationAngleUniTest
 * @tc.desc: test results of GetAccelerationAngle
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleUniTest GetAccelerationAngleUniTest Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    float res = rsRenderParticle.GetAccelerationAngle();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetRedSpeedUniTest
 * @tc.desc: test results of GetRedSpeed
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleUniTest GetRedSpeedUniTest Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    float res = rsRenderParticle.GetRedSpeed();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: CalculateParticlePositionUniTest
 * @tc.desc: test results of CalculateParticlePosition
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleUniTest CalculateParticlePositionUniTest Level1)
{
    ShapeType emitShape_ = ShapeType::RECT;
    Vector2f position_;
    Vector2f emitSize_;
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    bool la = rsRenderParticle.IsAlive();
    rsRenderParticle.CalculateParticlePosition(emitShape_, position_, emitSize_);
    ASSERT_EQ(la, false);
}

/**
 * @tc.name: LerpUniTest
 * @tc.desc: test results of Lerp
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleUniTest LerpUniTest Level1)
{
    Color start;
    Color end;
    float t = 1.0f;
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    rsRenderParticle.Lerp(start, end, t);
    ASSERT_NE(t, 0);
}

/**
 * @tc.name: GetRedFUniTest
 * @tc.desc: test results of GetRedF
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleUniTest GetRedFUniTest Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    float res = rsRenderParticle.GetRedF();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetGreenFUniTest
 * @tc.desc: test results of GetGreenF
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleUniTest GetGreenFUniTest Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    float res = rsRenderParticle.GetGreenF();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetBlueFUniTest
 * @tc.desc: test results of GetBlueF
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleUniTest GetBlueFUniTest Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    float res = rsRenderParticle.GetBlueF();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetActiveTimeUniTest
 * @tc.desc: test results of GetActiveTime
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleUniTest GetActiveTimeUniTest Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    int64_t res = rsRenderParticle.GetActiveTime();
    ASSERT_EQ(res, 0);
}

/**
 * @tc.name: GetEmitRateUniTest
 * @tc.desc: test results of GetEmitRate
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsUniTest GetEmitRateUniTest Level1)
{
    ParticleRenderParams particleRenderParams;
    int res = particleRenderParams.GetEmitRate();
    ASSERT_EQ(res, 0);
}

/**
 * @tc.name: GetEmitSizeUniTest
 * @tc.desc: test results of GetEmitSize
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsUniTest GetEmitSizeUniTest Level1)
{
    Vector2f position;
    ParticleRenderParams particleRenderParams;
    position = particleRenderParams.GetEmitSize();
    ASSERT_EQ(position, position);
}

/**
 * @tc.name: GetAlphaFUniTest
 * @tc.desc: test results of GetAlphaF
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleUniTest GetAlphaFUniTest Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    float res = rsRenderParticle.GetAlphaF();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetImageUniTest
 * @tc.desc: test results of GetImage
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleUniTest GetImageUniTest Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    std::shared_ptr<RSImage> res;
    res = rsRenderParticle.GetImage();
    ASSERT_EQ(res, res);
}

/**
 * @tc.name: GetImageSizeUniTest
 * @tc.desc: test results of GetImageSize
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleUniTest GetImageSizeUniTest Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    Vector2f v;
    v = rsRenderParticle.GetImageSize();
    ASSERT_EQ(v, v);
}

/**
 * @tc.name: GetParticleTypeUniTest
 * @tc.desc: test results of GetParticleType
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleUniTest GetParticleTypeUniTest Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    ParticleType part;
    part = rsRenderParticle.GetParticleType();
    ASSERT_EQ(part, part);
}

/**
 * @tc.name: GetParticleCountUniTest
 * @tc.desc: test results of GetParticleCount
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsUniTest GetParticleCountUniTest Level1)
{
    ParticleRenderParams particleRenderParams;
    int32_t la = particleRenderParams.GetParticleCount();
    ASSERT_EQ(la, 0);
}

/**
 * @tc.name: GetParticleTypeUniTest
 * @tc.desc: test results of GetParticleType
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsUniTest GetParticleTypeUniTest Level1)
{
    ParticleRenderParams particleRenderParams;
    ParticleType particle;
    particle = particleRenderParams.GetParticleType();
    ASSERT_EQ(particle, ParticleType::POINTS);
}

/**
 * @tc.name: GetParticleRadiusUniTest
 * @tc.desc: test results of GetParticleRadius
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsUniTest GetParticleRadiusUniTest Level1)
{
    ParticleRenderParams particleRenderParams;
    float res = particleRenderParams.GetParticleRadius();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetParticleImageUniTest
 * @tc.desc: test results of GetParticleImage
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsUniTest GetParticleImageUniTest Level1)
{
    ParticleRenderParams particleRenderParams;
    std::shared_ptr<RSImage> res;
    res = particleRenderParams.GetParticleImage();
    ASSERT_EQ(res, res);
}

/**
 * @tc.name: GetParticleRenderParamsUniTest
 * @tc.desc: test results of GetParticleRenderParams
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleUniTest GetParticleRenderParamsUniTest Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    std::shared_ptr<ParticleRenderParams> res;
    res = rsRenderParticle.GetParticleRenderParams();
    ASSERT_EQ(res, res);
}

/**
 * @tc.name: GetAcceValChangeOverLifeUniTest
 * @tc.desc: test results of GetAcceValChangeOverLife
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsUniTest GetAcceValChangeOverLifeUniTest Level1)
{
    ParticleRenderParams particleRenderParams;
    std::vector<std::shared_ptr<ChangeInOverLife<float>>> res;
    res = particleRenderParams.GetAcceValChangeOverLife();
    ASSERT_EQ(res, res);
}

/**
 * @tc.name: GetAcceAngChangeOverLifeUniTest
 * @tc.desc: test results of GetAcceAngChangeOverLife
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsUniTest GetAcceAngChangeOverLifeUniTest Level1)
{
    ParticleRenderParams particleRenderParams;
    std::vector<std::shared_ptr<ChangeInOverLife<float>>> res;
    res = particleRenderParams.GetAcceAngChangeOverLife();
    ASSERT_EQ(res, res);
}

/**
 * @tc.name: GetOpacityChangeOverLifeUniTest
 * @tc.desc: test results of GetOpacityChangeOverLife
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsUniTest GetOpacityChangeOverLifeUniTest Level1)
{
    ParticleRenderParams particleRenderParams;
    std::vector<std::shared_ptr<ChangeInOverLife<float>>> res;
    res = particleRenderParams.GetOpacityChangeOverLife();
    ASSERT_EQ(res, res);
}

/**
 * @tc.name: GetImageSizeUniTest
 * @tc.desc: test results of GetImageSize
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsUniTest GetImageSizeUniTest Level1)
{
    Vector2f position;
    ParticleRenderParams particleRenderParams;
    position = particleRenderParams.GetImageSize();
    ASSERT_EQ(position, position);
}

/**
 * @tc.name: GetVelocityStartValueUniTest
 * @tc.desc: test results of GetVelocityStartValue
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsUniTest GetVelocityStartValueUniTest Level1)
{
    ParticleRenderParams particleRenderParams;
    float res = particleRenderParams.GetVelocityStartValue();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetEmitShapeUniTest
 * @tc.desc: test results of GetEmitShape
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsUniTest GetEmitShapeUniTest Level1)
{
    ParticleRenderParams particleRenderParams;
    ShapeType shape;
    shape = particleRenderParams.GetEmitShape();
    ASSERT_EQ(shape, ShapeType::RECT);
}

/**
 * @tc.name: GetEmitPositionUniTest
 * @tc.desc: test results of GetEmitPosition
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsUniTest GetEmitPositionUniTest Level1)
{
    Vector2f position;
    ParticleRenderParams particleRenderParams;
    position = particleRenderParams.GetEmitPosition();
    ASSERT_EQ(position, position);
}

/**
 * @tc.name: GetVelocityEndValueUniTest
 * @tc.desc: test results of GetVelocityEndValue
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsUniTest GetVelocityEndValueUniTest Level1)
{
    ParticleRenderParams particleRenderParams;
    float res = particleRenderParams.GetVelocityEndValue();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetVelocityStartAngleUniTest
 * @tc.desc: test results of GetVelocityStartAngle
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsUniTest GetVelocityStartAngleUniTest Level1)
{
    ParticleRenderParams particleRenderParams;
    float res = particleRenderParams.GetVelocityStartAngle();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: IsAliveUniTest
 * @tc.desc: test results of IsAlive
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleUniTest IsAliveUniTest Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    bool res = rsRenderParticle.IsAlive();
    EXPECT_EQ(res, false);

    rsRenderParticle.SetIsDead();
    res = rsRenderParticle.IsAlive();
    EXPECT_EQ(res, false);
}

/**
 * @tc.name: SetIsDeadUniTest
 * @tc.desc: test results of SetIsDead
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleUniTest SetIsDeadUniTest Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    rsRenderParticle.SetIsDead();
    EXPECT_TRUE(true);
}

/**
 * @tc.name: GetRandomValueUniTest
 * @tc.desc: test results of GetRandomValue
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleUniTest GetRandomValueUniTest Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    float min = -1.0f;
    float max = 0.f;
    float res = rsRenderParticle.GetRandomValue(min, max);
    EXPECT_NE(res, 0.f);

    min = 1.0f;
    res = rsRenderParticle.GetRandomValue(min, max);
    EXPECT_EQ(max, 0.f);
}
}
