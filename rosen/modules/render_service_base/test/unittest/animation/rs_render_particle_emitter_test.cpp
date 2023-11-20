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

#include <vector>

#include "gtest/gtest.h"

#include "animation/rs_render_particle.h"
#include "animation/rs_render_particle_animation.h"
#include "animation/rs_render_particle_emitter.h"
#include "common/rs_vector2.h"
#include "modifier/rs_render_property.h"
#include "pipeline/rs_canvas_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSRenderParticleEmitterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static constexpr uint64_t ANIMATION_ID = 12345;
    static constexpr uint64_t PROPERTY_ID = 54321;
    std::shared_ptr<ParticleRenderParams> params;
    std::shared_ptr<RSRenderParticleEmitter> particleEmitter;;
};

void RSRenderParticleEmitterTest::SetUpTestCase() {}
void RSRenderParticleEmitterTest::TearDownTestCase() {}
void RSRenderParticleEmitterTest::SetUp()
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
    EmitterConfig emitterConfig = EmitterConfig(
        emitRate, emitShape, position, emitSize, particleCount, lifeTime, type, radius, image, imageSize);

    ParticleVelocity velocity;
    RenderParticleAcceleration acceleration;
    RenderParticleColorParaType color;
    RenderParticleParaType<float> opacity;
    RenderParticleParaType<float> scale;
    RenderParticleParaType<float> spin;
    params =
        std::make_shared<ParticleRenderParams>(emitterConfig, velocity, acceleration, color, opacity, scale, spin);
    particleEmitter = std::make_shared<RSRenderParticleEmitter>(params);
}
void RSRenderParticleEmitterTest::TearDown() {}

/**
 * @tc.name: EmitParticle001
 * @tc.desc: Verify the EmitParticle
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderParticleEmitterTest, EmitParticle001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderParticleEmitterTest EmitParticle001 start";
    particleEmitter->EmitParticle(NS_TO_S);
    EXPECT_TRUE(particleEmitter != nullptr);
    GTEST_LOG_(INFO) << "RSRenderParticleEmitterTest EmitParticle001 end";
}

} // namespace Rosen
} // namespace OHOS