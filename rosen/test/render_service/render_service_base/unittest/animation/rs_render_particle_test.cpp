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
 * @tc.name: CalculateParticlePositionTest
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, CalculateParticlePositionTest, Level1)
{
    ShapeType emitShape_ = ShapeType::RECT;
    Vector2f position_;
    Vector2f emitSize_;
    auto particleParams = std::shared_ptr<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    bool la = rsRenderParticle.IsAlive();
    rsRenderParticle.CalculateParticlePosition(emitShape_, position_, emitSize_);
    ASSERT_EQ(la, true);
}

/**
 * @tc.name: LerpTest
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, LerpTest, Level1)
{
    Color start;
    Color end;
    float t = 1.0f;
    auto particleParams = std::shared_ptr<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    rsRenderParticle.Lerp(start, end, t);
    ASSERT_NE(t, 0);
}

/**
 * @tc.name: InitPropertyTest
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, InitPropertyTest, Level1)
{
    auto particleParams = std::shared_ptr<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    rsRenderParticle.InitProperty();
    bool la = rsRenderParticle.IsAlive();
    ASSERT_EQ(la, true);
}

/**
 * @tc.name: SetPositionTest
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, SetPositionTest, Level1)
{
    Vector2f position;
    auto particleParams = std::shared_ptr<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    rsRenderParticle.SetPosition(position);
    bool la = rsRenderParticle.IsAlive();
    ASSERT_EQ(la, true);
}

/**
 * @tc.name: SetVelocityTest
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, SetVelocityTest, Level1)
{
    Vector2f velocity;
    auto particleParams = std::shared_ptr<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    rsRenderParticle.SetVelocity(velocity);
    bool la = rsRenderParticle.IsAlive();
    ASSERT_EQ(la, true);
}

/**
 * @tc.name: SetAccelerationTest
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, SetAccelerationTest, Level1)
{
    Vector2f acceleration;
    auto particleParams = std::shared_ptr<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    rsRenderParticle.SetAcceleration(acceleration);
    bool la = rsRenderParticle.IsAlive();
    ASSERT_EQ(la, true);
}

/**
 * @tc.name: SetSpinTest
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, SetSpinTest, Level1)
{
    float spin = 1.0f;
    auto particleParams = std::shared_ptr<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    rsRenderParticle.SetSpin(spin);
    ASSERT_NE(spin, 0);
}

/**
 * @tc.name: SetOpacityTest
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, SetOpacityTest, Level1)
{
    float opacity = 1.0f;
    auto particleParams = std::shared_ptr<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    rsRenderParticle.SetOpacity(opacity);
    ASSERT_NE(opacity, 0);
}

/**
 * @tc.name: SetColorTest
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, SetColorTest, Level1)
{
    Color color;
    auto particleParams = std::shared_ptr<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    rsRenderParticle.SetColor(color);
    bool la = rsRenderParticle.IsAlive();
    ASSERT_EQ(la, true);
}

/**
 * @tc.name: SetScaleTest
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, SetScaleTest, Level1)
{
    float scale = 1.0f;
    auto particleParams = std::shared_ptr<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    rsRenderParticle.SetScale(scale);
    ASSERT_NE(scale, 0);
}

/**
 * @tc.name: SetRadiusTest
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, SetRadiusTest, Level1)
{
    float radius = 1.0f;
    auto particleParams = std::shared_ptr<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    rsRenderParticle.SetRadius(radius);
    ASSERT_NE(radius, 0);
}

/**
 * @tc.name: SetImageTest
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, SetImageTest, Level1)
{
    auto particleParams = std::shared_ptr<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    rsRenderParticle.SetImage(nullptr);
    bool la = rsRenderParticle.IsAlive();
    ASSERT_EQ(la, true);
}

/**
 * @tc.name: SetImageSizeTest
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, SetImageSizeTest, Level1)
{
    Vector2f imageSize;
    auto particleParams = std::shared_ptr<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    rsRenderParticle.SetImageSize(imageSize);
    bool la = rsRenderParticle.IsAlive();
    ASSERT_EQ(la, true);
}

/**
 * @tc.name: SetParticleTypeTest
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, SetParticleTypeTest, Level1)
{
    ParticleType particleType = ParticleType::POINTS;
    auto particleParams = std::shared_ptr<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    rsRenderParticle.SetParticleType(particleType);
    bool la = rsRenderParticle.IsAlive();
    ASSERT_EQ(la, true);
}

/**
 * @tc.name: SetActiveTimeTest
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleTest, SetActiveTimeTest, Level1)
{
    int64_t activeTime = 60;
    auto particleParams = std::shared_ptr<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    rsRenderParticle.SetActiveTime(activeTime);
    ASSERT_NE(activeTime, 0);
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
