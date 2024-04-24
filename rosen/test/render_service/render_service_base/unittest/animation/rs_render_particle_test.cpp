/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
 * @tc.name: SetPositionTest
 * @tc.desc: test results of SetPosition
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, SetPositionTest, Level1)
{
    Vector2f position;
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    rsRenderParticle.SetPosition(position);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetVelocityTest
 * @tc.desc: test results of SetVelocity
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, SetVelocityTest, Level1)
{
    Vector2f velocity;
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    rsRenderParticle.SetVelocity(velocity);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetAccelerationTest
 * @tc.desc: test results of SetAcceleration
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, SetAccelerationTest, Level1)
{
    Vector2f acceleration;
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    rsRenderParticle.SetAcceleration(acceleration);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetSpinTest
 * @tc.desc: test results of SetSpin
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, SetSpinTest, Level1)
{
    float spin = 1.0f;
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    rsRenderParticle.SetSpin(spin);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetOpacityTest
 * @tc.desc: test results of SetOpacity
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, SetOpacityTest, Level1)
{
    float opacity = 1.0f;
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    rsRenderParticle.SetOpacity(opacity);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetColorTest
 * @tc.desc: test results of SetColor
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, SetColorTest, Level1)
{
    Color color;
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    rsRenderParticle.SetColor(color);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetScaleTest
 * @tc.desc: test results of SetScale
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, SetScaleTest, Level1)
{
    float scale = 1.0f;
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    rsRenderParticle.SetScale(scale);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetRadiusTest
 * @tc.desc: test results of SetRadius
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, SetRadiusTest, Level1)
{
    float radius = 1.0f;
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    rsRenderParticle.SetRadius(radius);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetImageTest
 * @tc.desc: test results of SetImage
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, SetImageTest, Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    auto image = std::make_shared<RSImage>();
    rsRenderParticle.SetImage(image);
    EXPECT_NE(image, nullptr);
}

/**
 * @tc.name: SetImageSizeTest
 * @tc.desc: test results of SetImageSize
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, SetImageSizeTest, Level1)
{
    Vector2f imageSize;
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    rsRenderParticle.SetImageSize(imageSize);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetParticleTypeTest
 * @tc.desc: test results of SetParticleType
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, SetParticleTypeTest, Level1)
{
    ParticleType particleType = ParticleType::POINTS;
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    rsRenderParticle.SetParticleType(particleType);
    EXPECT_EQ(rsRenderParticle.particleType_, particleType);
}

/**
 * @tc.name: SetActiveTimeTest
 * @tc.desc: test results of SetActiveTime
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, SetActiveTimeTest, Level1)
{
    int64_t activeTime = 60;
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    rsRenderParticle.SetActiveTime(activeTime);
    EXPECT_EQ(rsRenderParticle.activeTime_, activeTime);
}

/**
 * @tc.name: SetAccelerationValueTest
 * @tc.desc: test results of SetAccelerationValue
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, SetAccelerationValueTest, Level1)
{
    float accelerationValue = 0.f;
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    rsRenderParticle.SetAccelerationValue(accelerationValue);
    EXPECT_EQ(rsRenderParticle.accelerationValue_, accelerationValue);
}

/**
 * @tc.name: SetAccelerationAngleTest
 * @tc.desc: test results of SetAccelerationAngle
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, SetAccelerationAngleTest, Level1)
{
    float accelerationAngle = 0.f;
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    rsRenderParticle.SetAccelerationAngle(accelerationAngle);
    EXPECT_EQ(rsRenderParticle.accelerationAngle_, accelerationAngle);
}

/**
 * @tc.name: SetRedFTest
 * @tc.desc: test results of SetRedF
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, SetRedFTest, Level1)
{
    float redF = 0.f;
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    rsRenderParticle.SetRedF(redF);
    EXPECT_EQ(rsRenderParticle.redF_, redF);
}

/**
 * @tc.name: SetGreenFTest
 * @tc.desc: test results of SetGreenF
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, SetGreenFTest, Level1)
{
    float greenF = 0.f;
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    rsRenderParticle.SetGreenF(greenF);
    EXPECT_EQ(rsRenderParticle.greenF_, greenF);
}

/**
 * @tc.name: SetBlueFTest
 * @tc.desc: test results of SetBlueF
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, SetBlueFTest, Level1)
{
    float blueF = 0.f;
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    rsRenderParticle.SetBlueF(blueF);
    EXPECT_EQ(rsRenderParticle.blueF_, blueF);
}

/**
 * @tc.name: SetAlphaFTest
 * @tc.desc: test results of SetAlphaF
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, SetAlphaFTest, Level1)
{
    float alphaF = 0.f;
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    rsRenderParticle.SetAlphaF(alphaF);
    EXPECT_EQ(rsRenderParticle.alphaF_, alphaF);
}

/**
 * @tc.name: GetPositionTest
 * @tc.desc: test results of GetPosition
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, GetPositionTest, Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    Vector2f v;
    v = rsRenderParticle.GetPosition();
    EXPECT_EQ(v, v);
}

/**
 * @tc.name: GetVelocityTest
 * @tc.desc: test results of GetVelocity
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, GetVelocityTest, Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    Vector2f v;
    v = rsRenderParticle.GetVelocity();
    EXPECT_EQ(v, v);
}

/**
 * @tc.name: GetAccelerationTest
 * @tc.desc: test results of GetAcceleration
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, GetAccelerationTest, Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    Vector2f v;
    v = rsRenderParticle.GetAcceleration();
    EXPECT_EQ(v, v);
}

/**
 * @tc.name: GetSpinTest
 * @tc.desc: test results of GetSpin
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, GetSpinTest, Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    float res = rsRenderParticle.GetSpin();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetOpacityTest
 * @tc.desc: test results of GetOpacity
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, GetOpacityTest, Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    float res = rsRenderParticle.GetOpacity();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetColorTest
 * @tc.desc: test results of GetColor
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, GetColorTest, Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    Color color = rsRenderParticle.GetColor();
    ASSERT_EQ(color, color);
}

/**
 * @tc.name: GetScaleTest
 * @tc.desc: test results of GetScale
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, GetScaleTest, Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    float res = rsRenderParticle.GetScale();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetRadiusTest
 * @tc.desc: test results of GetRadius
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, GetRadiusTest, Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    float res = rsRenderParticle.GetRadius();
    ASSERT_NE(res, 1.0f);
}

/**
 * @tc.name: GetAccelerationValueTest
 * @tc.desc: test results of GetAccelerationValue
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, GetAccelerationValueTest, Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    float res = rsRenderParticle.GetAccelerationValue();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetAccelerationAngleTest
 * @tc.desc: test results of GetAccelerationAngle
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, GetAccelerationAngleTest, Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    float res = rsRenderParticle.GetAccelerationAngle();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetRedSpeedTest
 * @tc.desc: test results of GetRedSpeed
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, GetRedSpeedTest, Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    float res = rsRenderParticle.GetRedSpeed();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetGreenSpeedTest
 * @tc.desc: test results of GetGreenSpeed
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, GetGreenSpeedTest, Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    float res = rsRenderParticle.GetGreenSpeed();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetBlueSpeedTest
 * @tc.desc: test results of GetBlueSpeed
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, GetBlueSpeedTest, Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    float res = rsRenderParticle.GetBlueSpeed();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetAlphaSpeedTest
 * @tc.desc: test results of GetAlphaSpeed
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, GetAlphaSpeedTest, Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    float res = rsRenderParticle.GetAlphaSpeed();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetOpacitySpeedTest
 * @tc.desc: test results of GetOpacitySpeed
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, GetOpacitySpeedTest, Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    float res = rsRenderParticle.GetOpacitySpeed();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetScaleSpeedTest
 * @tc.desc: test results of GetScaleSpeed
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, GetScaleSpeedTest, Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    float res = rsRenderParticle.GetScaleSpeed();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetSpinSpeedTest
 * @tc.desc: test results of GetSpinSpeed
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, GetSpinSpeedTest, Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    float res = rsRenderParticle.GetSpinSpeed();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetAccelerationValueSpeedTest
 * @tc.desc: test results of GetAccelerationValueSpeed
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, GetAccelerationValueSpeedTest, Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    float res = rsRenderParticle.GetAccelerationValueSpeed();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetAccelerationAngleSpeedTest
 * @tc.desc: test results of GetAccelerationAngleSpeed
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, GetAccelerationAngleSpeedTest, Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    float res = rsRenderParticle.GetAccelerationAngleSpeed();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetRedFTest
 * @tc.desc: test results of GetRedF
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, GetRedFTest, Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    float res = rsRenderParticle.GetRedF();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetGreenFTest
 * @tc.desc: test results of GetGreenF
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, GetGreenFTest, Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    float res = rsRenderParticle.GetGreenF();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetBlueFTest
 * @tc.desc: test results of GetBlueF
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, GetBlueFTest, Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    float res = rsRenderParticle.GetBlueF();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetAlphaFTest
 * @tc.desc: test results of GetAlphaF
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, GetAlphaFTest, Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    float res = rsRenderParticle.GetAlphaF();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetImageTest
 * @tc.desc: test results of GetImage
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, GetImageTest, Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    std::shared_ptr<RSImage> res;
    res = rsRenderParticle.GetImage();
    ASSERT_EQ(res, res);
}

/**
 * @tc.name: GetImageSizeTest
 * @tc.desc: test results of GetImageSize
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, GetImageSizeTest, Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    Vector2f v;
    v = rsRenderParticle.GetImageSize();
    ASSERT_EQ(v, v);
}

/**
 * @tc.name: GetParticleTypeTest
 * @tc.desc: test results of GetParticleType
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, GetParticleTypeTest, Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    ParticleType part;
    part = rsRenderParticle.GetParticleType();
    ASSERT_EQ(part, part);
}

/**
 * @tc.name: GetActiveTimeTest
 * @tc.desc: test results of GetActiveTime
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, GetActiveTimeTest, Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    int64_t res = rsRenderParticle.GetActiveTime();
    ASSERT_EQ(res, 0);
}

/**
 * @tc.name: GetParticleRenderParamsTest
 * @tc.desc: test results of GetParticleRenderParams
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, GetParticleRenderParamsTest, Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    std::shared_ptr<ParticleRenderParams> res;
    res = rsRenderParticle.GetParticleRenderParams();
    ASSERT_EQ(res, res);
}

/**
 * @tc.name: GetAccelerationValueUpdatorTest
 * @tc.desc: test results of GetAccelerationValueUpdator
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, GetAccelerationValueUpdatorTest, Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    ParticleUpdator part;
    part = rsRenderParticle.GetAccelerationValueUpdator();
    ASSERT_EQ(part, part);
}

/**
 * @tc.name: GetAccelerationAngleUpdatorTest
 * @tc.desc: test results of GetAccelerationAngleUpdator
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, GetAccelerationAngleUpdatorTest, Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    ParticleUpdator part;
    part = rsRenderParticle.GetAccelerationAngleUpdator();
    ASSERT_EQ(part, part);
}

/**
 * @tc.name: GetColorUpdatorTest
 * @tc.desc: test results of GetColorUpdator
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, GetColorUpdatorTest, Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    ParticleUpdator part;
    part = rsRenderParticle.GetColorUpdator();
    ASSERT_EQ(part, part);
}

/**
 * @tc.name: GetOpacityUpdatorTest
 * @tc.desc: test results of GetOpacityUpdator
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, GetOpacityUpdatorTest, Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    ParticleUpdator part;
    part = rsRenderParticle.GetOpacityUpdator();
    ASSERT_EQ(part, part);
}

/**
 * @tc.name: GetScaleUpdatorTest
 * @tc.desc: test results of GetScaleUpdator
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, GetScaleUpdatorTest, Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    ParticleUpdator part;
    part = rsRenderParticle.GetScaleUpdator();
    ASSERT_EQ(part, part);
}

/**
 * @tc.name: GetSpinUpdatorTest
 * @tc.desc: test results of GetSpinUpdator
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, GetSpinUpdatorTest, Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    ParticleUpdator part;
    part = rsRenderParticle.GetSpinUpdator();
    ASSERT_EQ(part, part);
}

/**
 * @tc.name: GetAcceValChangeOverLifeTest
 * @tc.desc: test results of GetAcceValChangeOverLife
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, GetAcceValChangeOverLifeTest, Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    std::vector<std::shared_ptr<ChangeInOverLife<float>>> res;
    res = rsRenderParticle.GetAcceValChangeOverLife();
    ASSERT_EQ(res, res);
}

/**
 * @tc.name: GetAcceAngChangeOverLifeTest
 * @tc.desc: test results of GetAcceAngChangeOverLife
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, GetAcceAngChangeOverLifeTest, Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    std::vector<std::shared_ptr<ChangeInOverLife<float>>> res;
    res = rsRenderParticle.GetAcceAngChangeOverLife();
    ASSERT_EQ(res, res);
}

/**
 * @tc.name: GetOpacityChangeOverLifeTest
 * @tc.desc: test results of GetOpacityChangeOverLife
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, GetOpacityChangeOverLifeTest, Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    std::vector<std::shared_ptr<ChangeInOverLife<float>>> res;
    res = rsRenderParticle.GetOpacityChangeOverLife();
    ASSERT_EQ(res, res);
}

/**
 * @tc.name: GetScaleChangeOverLifeTest
 * @tc.desc: test results of GetScaleChangeOverLife
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, GetScaleChangeOverLifeTest, Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    std::vector<std::shared_ptr<ChangeInOverLife<float>>> res;
    res = rsRenderParticle.GetScaleChangeOverLife();
    ASSERT_EQ(res, res);
}

/**
 * @tc.name: GetSpinChangeOverLifeTest
 * @tc.desc: test results of GetSpinChangeOverLife
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, GetSpinChangeOverLifeTest, Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    std::vector<std::shared_ptr<ChangeInOverLife<float>>> res;
    res = rsRenderParticle.GetSpinChangeOverLife();
    ASSERT_EQ(res, res);
}

/**
 * @tc.name: GetColorChangeOverLifeTest
 * @tc.desc: test results of GetColorChangeOverLife
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, GetColorChangeOverLifeTest, Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    std::vector<std::shared_ptr<ChangeInOverLife<Color>>> res;
    res = rsRenderParticle.GetColorChangeOverLife();
    ASSERT_EQ(res, res);
}

/**
 * @tc.name: InitPropertyTest
 * @tc.desc: test results of InitProperty
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, InitPropertyTest, Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    rsRenderParticle.InitProperty();
    EXPECT_TRUE(true);
}

/**
 * @tc.name: IsAliveTest
 * @tc.desc: test results of IsAlive
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, IsAliveTest, Level1)
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
 * @tc.name: SetIsDeadTest
 * @tc.desc: test results of SetIsDead
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, SetIsDeadTest, Level1)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    rsRenderParticle.SetIsDead();
    EXPECT_TRUE(true);
}

/**
 * @tc.name: GetRandomValueTest
 * @tc.desc: test results of GetRandomValue
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, GetRandomValueTest, Level1)
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

/**
 * @tc.name: CalculateParticlePositionTest
 * @tc.desc: test results of CalculateParticlePosition
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, CalculateParticlePositionTest, Level1)
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
 * @tc.name: LerpTest
 * @tc.desc: test results of Lerp
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, LerpTest, Level1)
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
 * @tc.name: GetEmitRateTest
 * @tc.desc: test results of GetEmitRate
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, GetEmitRateTest, Level1)
{
    ParticleRenderParams particleRenderParams;
    int res = particleRenderParams.GetEmitRate();
    ASSERT_EQ(res, 0);
}

/**
 * @tc.name: GetEmitShapeTest
 * @tc.desc: test results of GetEmitShape
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, GetEmitShapeTest, Level1)
{
    ParticleRenderParams particleRenderParams;
    ShapeType shape;
    shape = particleRenderParams.GetEmitShape();
    ASSERT_EQ(shape, ShapeType::RECT);
}

/**
 * @tc.name: GetEmitPositionTest
 * @tc.desc: test results of GetEmitPosition
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, GetEmitPositionTest, Level1)
{
    Vector2f position;
    ParticleRenderParams particleRenderParams;
    position = particleRenderParams.GetEmitPosition();
    ASSERT_EQ(position, position);
}

/**
 * @tc.name: GetEmitSizeTest
 * @tc.desc: test results of GetEmitSize
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, GetEmitSizeTest, Level1)
{
    Vector2f position;
    ParticleRenderParams particleRenderParams;
    position = particleRenderParams.GetEmitSize();
    ASSERT_EQ(position, position);
}

/**
 * @tc.name: GetParticleCountTest
 * @tc.desc: test results of GetParticleCount
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, GetParticleCountTest, Level1)
{
    ParticleRenderParams particleRenderParams;
    int32_t la = particleRenderParams.GetParticleCount();
    ASSERT_EQ(la, 0);
}

/**
 * @tc.name: GetParticleTypeTest
 * @tc.desc: test results of GetParticleType
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, GetParticleTypeTest, Level1)
{
    ParticleRenderParams particleRenderParams;
    ParticleType particle;
    particle = particleRenderParams.GetParticleType();
    ASSERT_EQ(particle, ParticleType::POINTS);
}

/**
 * @tc.name: GetParticleRadiusTest
 * @tc.desc: test results of GetParticleRadius
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, GetParticleRadiusTest, Level1)
{
    ParticleRenderParams particleRenderParams;
    float res = particleRenderParams.GetParticleRadius();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetParticleImageTest
 * @tc.desc: test results of GetParticleImage
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, GetParticleImageTest, Level1)
{
    ParticleRenderParams particleRenderParams;
    std::shared_ptr<RSImage> res;
    res = particleRenderParams.GetParticleImage();
    ASSERT_EQ(res, res);
}

/**
 * @tc.name: GetImageSizeTest
 * @tc.desc: test results of GetImageSize
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, GetImageSizeTest, Level1)
{
    Vector2f position;
    ParticleRenderParams particleRenderParams;
    position = particleRenderParams.GetImageSize();
    ASSERT_EQ(position, position);
}

/**
 * @tc.name: GetVelocityStartValueTest
 * @tc.desc: test results of GetVelocityStartValue
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, GetVelocityStartValueTest, Level1)
{
    ParticleRenderParams particleRenderParams;
    float res = particleRenderParams.GetVelocityStartValue();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetVelocityEndValueTest
 * @tc.desc: test results of GetVelocityEndValue
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, GetVelocityEndValueTest, Level1)
{
    ParticleRenderParams particleRenderParams;
    float res = particleRenderParams.GetVelocityEndValue();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetVelocityStartAngleTest
 * @tc.desc: test results of GetVelocityStartAngle
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, GetVelocityStartAngleTest, Level1)
{
    ParticleRenderParams particleRenderParams;
    float res = particleRenderParams.GetVelocityStartAngle();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetVelocityEndAngleTest
 * @tc.desc: test results of GetVelocityEndAngle
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, GetVelocityEndAngleTest, Level1)
{
    ParticleRenderParams particleRenderParams;
    float res = particleRenderParams.GetVelocityEndAngle();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetAccelerationStartValueTest
 * @tc.desc: test results of GetAccelerationStartValue
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, GetAccelerationStartValueTest, Level1)
{
    ParticleRenderParams particleRenderParams;
    float res = particleRenderParams.GetAccelerationStartValue();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetAccelerationEndValueTest
 * @tc.desc: test results of GetAccelerationEndValue
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, GetAccelerationEndValueTest, Level1)
{
    ParticleRenderParams particleRenderParams;
    float res = particleRenderParams.GetAccelerationEndValue();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetAccelerationStartAngleTest
 * @tc.desc: test results of GetAccelerationStartAngle
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, GetAccelerationStartAngleTest, Level1)
{
    ParticleRenderParams particleRenderParams;
    float res = particleRenderParams.GetAccelerationStartAngle();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetAccelerationEndAngleTest
 * @tc.desc: test results of GetAccelerationEndAngle
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, GetAccelerationEndAngleTest, Level1)
{
    ParticleRenderParams particleRenderParams;
    float res = particleRenderParams.GetAccelerationEndAngle();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetAccelerationValueUpdatorTest
 * @tc.desc: test results of GetAccelerationValueUpdator
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, GetAccelerationValueUpdatorTest, Level1)
{
    ParticleRenderParams particleRenderParams;
    ParticleUpdator particleUpdator;
    particleUpdator = particleRenderParams.GetAccelerationValueUpdator();
    ASSERT_EQ(particleUpdator, ParticleUpdator::NONE);
}

/**
 * @tc.name: GetAccelerationAngleUpdatorTest
 * @tc.desc: test results of GetAccelerationAngleUpdator
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, GetAccelerationAngleUpdatorTest, Level1)
{
    ParticleRenderParams particleRenderParams;
    ParticleUpdator particleUpdator;
    particleUpdator = particleRenderParams.GetAccelerationAngleUpdator();
    ASSERT_EQ(particleUpdator, ParticleUpdator::NONE);
}

/**
 * @tc.name: GetAccelRandomValueStartTest
 * @tc.desc: test results of GetAccelRandomValueStart
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, GetAccelRandomValueStartTest, Level1)
{
    ParticleRenderParams particleRenderParams;
    float res = particleRenderParams.GetAccelRandomValueStart();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetAccelRandomValueEndTest
 * @tc.desc: test results of GetAccelRandomValueEnd
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, GetAccelRandomValueEndTest, Level1)
{
    ParticleRenderParams particleRenderParams;
    float res = particleRenderParams.GetAccelRandomValueEnd();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetAccelRandomValueStartTest
 * @tc.desc: test results of GetAccelRandomAngleStart
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, GetAccelRandomAngleStartTest, Level1)
{
    ParticleRenderParams particleRenderParams;
    float res = particleRenderParams.GetAccelRandomAngleStart();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetAccelRandomAngleEndTest
 * @tc.desc: test results of GetAccelRandomAngleEnd
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, GetAccelRandomAngleEndTest, Level1)
{
    ParticleRenderParams particleRenderParams;
    float res = particleRenderParams.GetAccelRandomAngleEnd();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetColorStartValueTest
 * @tc.desc: test results of GetColorStartValue
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, GetColorStartValueTest, Level1)
{
    ParticleRenderParams particleRenderParams;
    Color color;
    color = particleRenderParams.GetColorStartValue();
    ASSERT_EQ(color, color);
}

/**
 * @tc.name: GetColorEndValueTest
 * @tc.desc: test results of GetColorEndValue
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, GetColorEndValueTest, Level1)
{
    ParticleRenderParams particleRenderParams;
    Color color;
    color = particleRenderParams.GetColorEndValue();
    ASSERT_EQ(color, color);
}

/**
 * @tc.name: GetColorUpdatorTest
 * @tc.desc: test results of GetColorUpdator
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, GetColorUpdatorTest, Level1)
{
    ParticleRenderParams particleRenderParams;
    ParticleUpdator particleUpdator;
    particleUpdator = particleRenderParams.GetColorUpdator();
    ASSERT_EQ(particleUpdator, particleUpdator);
}

/**
 * @tc.name: GetRedRandomStartTest
 * @tc.desc: test results of GetRedRandomStart
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, GetRedRandomStartTest, Level1)
{
    ParticleRenderParams particleRenderParams;
    float res = particleRenderParams.GetRedRandomStart();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetRedRandomEndTest
 * @tc.desc: test results of GetRedRandomEnd
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, GetRedRandomEndTest, Level1)
{
    ParticleRenderParams particleRenderParams;
    float res = particleRenderParams.GetRedRandomEnd();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetGreenRandomStartTest
 * @tc.desc: test results of GetGreenRandomStart
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, GetGreenRandomStartTest, Level1)
{
    ParticleRenderParams particleRenderParams;
    float res = particleRenderParams.GetGreenRandomStart();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetGreenRandomEndTest
 * @tc.desc: test results of GetGreenRandomEnd
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, GetGreenRandomEndTest, Level1)
{
    ParticleRenderParams particleRenderParams;
    float res = particleRenderParams.GetGreenRandomEnd();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetBlueRandomStartTest
 * @tc.desc: test results of GetBlueRandomStart
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, GetBlueRandomStartTest, Level1)
{
    ParticleRenderParams particleRenderParams;
    float res = particleRenderParams.GetBlueRandomStart();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetBlueRandomEndTest
 * @tc.desc: test results of GetBlueRandomEnd
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, GetBlueRandomEndTest, Level1)
{
    ParticleRenderParams particleRenderParams;
    float res = particleRenderParams.GetBlueRandomEnd();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetAlphaRandomStartTest
 * @tc.desc: test results of GetAlphaRandomStart
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, GetAlphaRandomStartTest, Level1)
{
    ParticleRenderParams particleRenderParams;
    float res = particleRenderParams.GetAlphaRandomStart();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetAlphaRandomEndTest
 * @tc.desc: test results of GetAlphaRandomEnd
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, GetAlphaRandomEndTest, Level1)
{
    ParticleRenderParams particleRenderParams;
    float res = particleRenderParams.GetAlphaRandomEnd();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetOpacityStartValueTest
 * @tc.desc: test results of GetOpacityStartValue
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, GetOpacityStartValueTest, Level1)
{
    ParticleRenderParams particleRenderParams;
    float res = particleRenderParams.GetOpacityStartValue();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetOpacityEndValueTest
 * @tc.desc: test results of GetOpacityEndValue
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, GetOpacityEndValueTest, Level1)
{
    ParticleRenderParams particleRenderParams;
    float res = particleRenderParams.GetOpacityEndValue();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetOpacityUpdatorTest
 * @tc.desc: test results of GetOpacityUpdator
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, GetOpacityUpdatorTest, Level1)
{
    ParticleRenderParams particleRenderParams;
    ParticleUpdator particleUpdator;
    particleUpdator = particleRenderParams.GetOpacityUpdator();
    ASSERT_EQ(particleUpdator, particleUpdator);
}

/**
 * @tc.name: GetOpacityRandomStartTest
 * @tc.desc: test results of GetOpacityRandomStart
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, GetOpacityRandomStartTest, Level1)
{
    ParticleRenderParams particleRenderParams;
    float res = particleRenderParams.GetOpacityRandomStart();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetOpacityRandomEndTest
 * @tc.desc: test results of GetOpacityRandomEnd
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, GetOpacityRandomEndTest, Level1)
{
    ParticleRenderParams particleRenderParams;
    float res = particleRenderParams.GetOpacityRandomEnd();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetScaleStartValueTest
 * @tc.desc: test results of GetScaleStartValue
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, GetScaleStartValueTest, Level1)
{
    ParticleRenderParams particleRenderParams;
    float res = particleRenderParams.GetScaleStartValue();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetScaleEndValueTest
 * @tc.desc: test results of GetScaleEndValue
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, GetScaleEndValueTest, Level1)
{
    ParticleRenderParams particleRenderParams;
    float res = particleRenderParams.GetScaleEndValue();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetScaleUpdatorTest
 * @tc.desc: test results of GetScaleUpdator
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, GetScaleUpdatorTest, Level1)
{
    ParticleRenderParams particleRenderParams;
    ParticleUpdator particleUpdator;
    particleUpdator = particleRenderParams.GetScaleUpdator();
    ASSERT_EQ(particleUpdator, particleUpdator);
}

/**
 * @tc.name: GetScaleRandomStartTest
 * @tc.desc: test results of GetScaleRandomStart
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, GetScaleRandomStartTest, Level1)
{
    ParticleRenderParams particleRenderParams;
    float res = particleRenderParams.GetScaleRandomStart();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetScaleRandomEndTest
 * @tc.desc: test results of GetScaleRandomEnd
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, GetScaleRandomEndTest, Level1)
{
    ParticleRenderParams particleRenderParams;
    float res = particleRenderParams.GetScaleRandomEnd();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetSpinStartValueTest
 * @tc.desc: test results of GetSpinStartValue
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, GetSpinStartValueTest, Level1)
{
    ParticleRenderParams particleRenderParams;
    float res = particleRenderParams.GetSpinStartValue();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetSpinEndValueTest
 * @tc.desc: test results of GetSpinEndValue
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, GetSpinEndValueTest, Level1)
{
    ParticleRenderParams particleRenderParams;
    float res = particleRenderParams.GetSpinEndValue();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetSpinUpdatorTest
 * @tc.desc: test results of GetSpinUpdator
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, GetSpinUpdatorTest, Level1)
{
    ParticleRenderParams particleRenderParams;
    ParticleUpdator particleUpdator;
    particleUpdator = particleRenderParams.GetSpinUpdator();
    ASSERT_EQ(particleUpdator, particleUpdator);
}

/**
 * @tc.name: GetSpinRandomStartTest
 * @tc.desc: test results of GetSpinRandomStart
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, GetSpinRandomStartTest, Level1)
{
    ParticleRenderParams particleRenderParams;
    float res = particleRenderParams.GetSpinRandomStart();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: GetSpinRandomEndTest
 * @tc.desc: test results of GetSpinRandomEnd
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, GetSpinRandomEndTest, Level1)
{
    ParticleRenderParams particleRenderParams;
    float res = particleRenderParams.GetSpinRandomEnd();
    ASSERT_EQ(res, 0.f);
}

/**
 * @tc.name: SetEmitConfigTest
 * @tc.desc: test results of SetEmitConfig
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, SetEmitConfigTest, Level1)
{
    ParticleRenderParams particleRenderParams;
    EmitterConfig emiterConfig;
    particleRenderParams.SetEmitConfig(emiterConfig);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetParticleVelocityTest
 * @tc.desc: test results of SetParticleVelocity
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, SetParticleVelocityTest, Level1)
{
    ParticleRenderParams particleRenderParams;
    ParticleVelocity velocity;
    particleRenderParams.SetParticleVelocity(velocity);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetParticleAccelerationTest
 * @tc.desc: test results of SetParticleAcceleration
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, SetParticleAccelerationTest, Level1)
{
    ParticleRenderParams particleRenderParams;
    RenderParticleAcceleration acceleration;
    particleRenderParams.SetParticleAcceleration(acceleration);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetParticleColorTest
 * @tc.desc: test results of SetParticleColor
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, SetParticleColorTest, Level1)
{
    ParticleRenderParams particleRenderParams;
    RenderParticleColorParaType color;
    particleRenderParams.SetParticleColor(color);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetParticleOpacityTest
 * @tc.desc: test results of SetParticleOpacity
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, SetParticleOpacityTest, Level1)
{
    ParticleRenderParams particleRenderParams;
    RenderParticleParaType<float> opacity;
    particleRenderParams.SetParticleOpacity(opacity);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetParticleScaleTest
 * @tc.desc: test results of SetParticleScale
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, SetParticleScaleTest, Level1)
{
    ParticleRenderParams particleRenderParams;
    RenderParticleParaType<float> scale;
    particleRenderParams.SetParticleScale(scale);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetParticleSpinTest
 * @tc.desc: test results of SetParticleSpin
 * @tc.type: FUNC
 */
HWTEST_F(ParticleRenderParamsTest, SetParticleSpinTest, Level1)
{
    ParticleRenderParams particleRenderParams;
    RenderParticleParaType<float> spin;
    particleRenderParams.SetParticleSpin(spin);
    EXPECT_TRUE(true);
}
}
