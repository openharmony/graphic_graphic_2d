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

#include <memory>
#include <vector>

#include "gtest/gtest.h"

#include "animation/rs_render_particle.h"
#include "animation/rs_render_particle_animation.h"
#include "common/rs_vector2.h"
#include "modifier/rs_render_property.h"
#include "pipeline/rs_canvas_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSRenderParticleEffectorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static constexpr uint64_t ANIMATION_ID = 12345;
    static constexpr uint64_t PROPERTY_ID = 54321;
    std::shared_ptr<ParticleRenderParams> params;
    std::shared_ptr<RSRenderParticle> particle;
    std::shared_ptr<RSRenderParticleEffector> effector;
    float deltaTime = 0.1f;
    int64_t activeTime = 100000;
};

void RSRenderParticleEffectorTest::SetUpTestCase() {}
void RSRenderParticleEffectorTest::TearDownTestCase() {}
void RSRenderParticleEffectorTest::SetUp()
{
    int emitRate = 20;
    ShapeType emitShape = ShapeType::RECT;
    Vector2f position = Vector2f(0.f, 0.f);
    Vector2f emitSize = Vector2f(10.f, 10.f);
    int particleCount = 20;
    int lifeTime = 3000;
    ParticleType type = ParticleType::POINTS;
    float radius = 1;
    std::shared_ptr<RSImage> image;
    Vector2f imageSize = Vector2f(1.f, 1.f);
    EmitterConfig emitterConfig =
        EmitterConfig(emitRate, emitShape, position, emitSize, particleCount, lifeTime, type, radius, image, imageSize);

    ParticleVelocity velocity;
    RenderParticleAcceleration acceleration;
    Color start = RSColor(200, 0, 0);
    Color end = RSColor(255, 255, 255);
    Range<Color> colorVal = Range<Color>(start, end);
    ParticleUpdator colorUpdator = ParticleUpdator::RANDOM;
    Range<float> redRandom = Range<float>(0.1f, 1.f);
    Range<float> greenRandom = Range<float>(0.1f, 1.f);
    Range<float> blueRandom = Range<float>(0.1f, 1.f);
    Range<float> alphaRandom = Range<float>(0.1f, 1.f);
    std::vector<std::shared_ptr<ChangeInOverLife<Color>>> colorChangeOverLife = {};
    RenderParticleColorParaType color =
        RenderParticleColorParaType(colorVal, colorUpdator, redRandom, greenRandom, blueRandom, alphaRandom, colorChangeOverLife);

    Range<float> opacityVal = Range<float>(0.5, 1);
    ParticleUpdator opacityUpdator = ParticleUpdator::CURVE;
    Range<float> opacityRandom;
    float fromValue = 0.1f;
    float toValue = 1.f;
    int startMillis = 0;
    int endMillis = 3000;
    std::shared_ptr<RSInterpolator> interpolator = nullptr;
    auto change1 = std::make_shared<ChangeInOverLife<float>>(fromValue, toValue, startMillis, endMillis, interpolator);
    auto change2 = std::make_shared<ChangeInOverLife<float>>(toValue, fromValue, endMillis + 4000, endMillis + 7000, interpolator);
    std::vector<std::shared_ptr<ChangeInOverLife<float>>> opacityChangeOverLife;
    opacityChangeOverLife.push_back(change1);
    opacityChangeOverLife.push_back(change2);
    RenderParticleParaType<float> opacity =
        RenderParticleParaType(opacityVal, opacityUpdator, opacityRandom, opacityChangeOverLife);
    

    ParticleUpdator scaleUpdator = ParticleUpdator::RANDOM;
    Range<float> scaleRandom = Range<float>(0.1f, 1.f);
    std::vector<std::shared_ptr<ChangeInOverLife<float>>> scaleChangeOverLife;
    RenderParticleParaType<float> scale =
        RenderParticleParaType(opacityVal, scaleUpdator, scaleRandom, scaleChangeOverLife);
    RenderParticleParaType<float> spin =
        RenderParticleParaType(opacityVal, scaleUpdator, scaleRandom, scaleChangeOverLife);
    params = std::make_shared<ParticleRenderParams>(emitterConfig, velocity, acceleration, color, opacity, scale, spin);
    particle = std::make_shared<RSRenderParticle>(params);
    effector = std::make_shared<RSRenderParticleEffector>(params);
}
void RSRenderParticleEffectorTest::TearDown() {}

/**
 * @tc.name: UpdateColor001
 * @tc.desc: Verify the UpdateColor
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderParticleEffectorTest, UpdateColor001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderParticleEffectorTest UpdateColor001 start";
    effector->UpdateColor(particle, deltaTime, 16666667);
    EXPECT_TRUE(particle->GetColor() != Color::FromArgbInt(0x00000000));
    GTEST_LOG_(INFO) << "RSRenderParticleEffectorTest UpdateColor001 end";
}

/**
 * @tc.name: UpdateOpacity001
 * @tc.desc: Verify the UpdateOpacity
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderParticleEffectorTest, UpdateOpacity001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderParticleEffectorTest UpdateOpacity001 start";
    effector->UpdateOpacity(particle, deltaTime, 16666667);
    EXPECT_TRUE(particle->GetOpacity() != 0.f);
    GTEST_LOG_(INFO) << "RSRenderParticleEffectorTest UpdateOpacity001 end";
}
/**
 * @tc.name: UpdateScale001
 * @tc.desc: Verify the UpdateScale
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderParticleEffectorTest, UpdateScale001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderParticleEffectorTest UpdateScale001 start";
    effector->UpdateScale(particle, deltaTime, 16666667);
    EXPECT_TRUE(particle->GetScale() != 0.f);
    GTEST_LOG_(INFO) << "RSRenderParticleEffectorTest UpdateScale001 end";
}
/**
 * @tc.name: UpdateSpin001
 * @tc.desc: Verify the UpdateSpin
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderParticleEffectorTest, UpdateSpin001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderParticleEffectorTest UpdateSpin001 start";
    effector->UpdateSpin(particle, deltaTime, 16666667);
    EXPECT_TRUE(particle->GetSpin() != 0.f);
    GTEST_LOG_(INFO) << "RSRenderParticleEffectorTest UpdateSpin001 end";
}

} // namespace Rosen
} // namespace OHOS