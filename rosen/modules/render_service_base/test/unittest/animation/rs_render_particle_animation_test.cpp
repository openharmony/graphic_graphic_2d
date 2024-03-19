/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
class RSRenderParticleAnimationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static constexpr uint64_t ANIMATION_ID = 12345;
    static constexpr uint64_t PROPERTY_ID = 54321;
    std::shared_ptr<ParticleRenderParams> params;
    std::vector<std::shared_ptr<ParticleRenderParams>> particlesRenderParams;
    std::shared_ptr<RSRenderParticleSystem> particleSystem_;
};

void RSRenderParticleAnimationTest::SetUpTestCase() {}
void RSRenderParticleAnimationTest::TearDownTestCase() {}
void RSRenderParticleAnimationTest::SetUp()
{
    int emitRate = 20;
    ShapeType emitShape = ShapeType::RECT;
    Vector2f position = Vector2f(0.f, 0.f);
    Vector2f emitSize = Vector2f(10.f, 10.f);
    int particleCount = 20;
    Range<int64_t> lifeTime = Range<int64_t>(3000, 3000); // 3000 is lifeTime.
    ParticleType type = ParticleType::POINTS;
    float radius = 10.f;
    std::shared_ptr<RSImage> image;
    Vector2f imageSize = Vector2f(1.f, 1.f);
    EmitterConfig emitterConfig = EmitterConfig(
        emitRate, emitShape, position, emitSize, particleCount, lifeTime, type, radius, image, imageSize);

    ParticleVelocity velocity;
    RenderParticleAcceleration acceleration;
    RenderParticleColorParaType color;
    RenderParticleParaType<float> opacity;
    RenderParticleParaType<float> scale;
    RenderParticleParaType<float> spin;
    params = std::make_shared<ParticleRenderParams>(emitterConfig, velocity, acceleration, color, opacity, scale, spin);
}
void RSRenderParticleAnimationTest::TearDown() {}

/**
 * @tc.name: Animate001
 * @tc.desc: Verify the Animate
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderParticleAnimationTest, Animate001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderParticleAnimationTest Animate001 start";
    auto renderParticleAnimation =
        std::make_shared<RSRenderParticleAnimation>(ANIMATION_ID, PROPERTY_ID, particlesRenderParams);
    auto particleAnimate = renderParticleAnimation->Animate(NS_TO_S);
    particlesRenderParams.push_back(params);
    renderParticleAnimation =
        std::make_shared<RSRenderParticleAnimation>(ANIMATION_ID, PROPERTY_ID, particlesRenderParams);
    particleSystem_ = std::make_shared<RSRenderParticleSystem>(particlesRenderParams);
    EXPECT_TRUE(particleSystem_ != nullptr);
    particleSystem_->CreateEmitter();
    auto activeParticles = renderParticleAnimation->GetRenderParticle().GetParticleVector();
    particleSystem_->Emit(NS_TO_S, activeParticles);
    particleSystem_->UpdateParticle(NS_TO_S, activeParticles);
    particleAnimate = renderParticleAnimation->Animate(NS_TO_S);
    EXPECT_TRUE(particleAnimate);
    GTEST_LOG_(INFO) << "RSRenderParticleAnimationTest Animate001 end";
}

/**
 * @tc.name: Marshalling001
 * @tc.desc: Verify the Marshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderParticleAnimationTest, Marshalling001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderParticleAnimationTest Marshalling001 start";

    auto renderParticleAnimation =
        std::make_shared<RSRenderParticleAnimation>(ANIMATION_ID, PROPERTY_ID, particlesRenderParams);

    Parcel parcel;
    renderParticleAnimation->Marshalling(parcel);
    EXPECT_TRUE(renderParticleAnimation != nullptr);
    GTEST_LOG_(INFO) << "RSRenderParticleAnimationTest Marshalling001 end";
}

/**
 * @tc.name: Unmarshalling001
 * @tc.desc: Verify the Unmarshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderParticleAnimationTest, Unmarshalling001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderParticleAnimationTest Unmarshalling001 start";

    auto renderParticleAnimation =
        std::make_shared<RSRenderParticleAnimation>(ANIMATION_ID, PROPERTY_ID, particlesRenderParams);

    Parcel parcel;
    renderParticleAnimation->Marshalling(parcel);
    std::shared_ptr<RSRenderAnimation>(RSRenderParticleAnimation::Unmarshalling(parcel));
    EXPECT_TRUE(renderParticleAnimation != nullptr);
    GTEST_LOG_(INFO) << "RSRenderParticleAnimationTest Unmarshalling001 end";
}
} // namespace Rosen
} // namespace OHOS