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
}
