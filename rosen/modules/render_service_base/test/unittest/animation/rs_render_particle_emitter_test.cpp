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
    void SetEmitConfig();
    void SetColor();
    void SetOpacity();
    void SetUp() override;
    void TearDown() override;
    static constexpr uint64_t ANIMATION_ID = 12345;
    static constexpr uint64_t PROPERTY_ID = 54321;
    EmitterConfig emitterConfig_;
    ParticleVelocity velocity_;
    RenderParticleAcceleration acceleration_;
    RenderParticleColorParaType color_;
    RenderParticleParaType<float> opacity_;
    RenderParticleParaType<float> scale_;
    RenderParticleParaType<float> spin_;
    std::shared_ptr<ParticleRenderParams> params;
    std::shared_ptr<RSRenderParticleEmitter> particleEmitter;
};

void RSRenderParticleEmitterTest::SetUpTestCase() {}
void RSRenderParticleEmitterTest::TearDownTestCase() {}
void RSRenderParticleEmitterTest::SetEmitConfig()
{
    int emitRate = 20;
    ShapeType emitShape = ShapeType::RECT;
    Vector2f position = Vector2f(0.f, 0.f);
    Vector2f emitSize = Vector2f(10.f, 10.f);
    int particleCount = 20;
    Range<int64_t> lifeTime = Range<int64_t>(3000, 3000); // 3000 is lifeTime.
    ParticleType type = ParticleType::POINTS;
    float radius = 1;
    std::shared_ptr<RSImage> image;
    Vector2f imageSize = Vector2f(1.f, 1.f);
    emitterConfig_ =
        EmitterConfig(emitRate, emitShape, position, emitSize, particleCount, lifeTime, type, radius, image, imageSize);
}

void RSRenderParticleEmitterTest::SetColor()
{
    Color start = RSColor(200, 0, 0, 100);
    Color end = RSColor(255, 255, 255, 255);
    Range<Color> colorVal = Range<Color>(start, end);
    DistributionType distribution = DistributionType::UNIFORM;
    ParticleUpdator colorUpdator = ParticleUpdator::RANDOM;
    Range<float> redRandom = Range<float>(0.1f, 1.f);
    Range<float> greenRandom = Range<float>(0.1f, 1.f);
    Range<float> blueRandom = Range<float>(0.1f, 1.f);
    Range<float> alphaRandom = Range<float>(0.1f, 1.f);
    std::vector<std::shared_ptr<ChangeInOverLife<Color>>> colorChangeOverLife = {};
    color_ = RenderParticleColorParaType(
        colorVal, distribution, colorUpdator, redRandom, greenRandom, blueRandom, alphaRandom, colorChangeOverLife);
}

void RSRenderParticleEmitterTest::SetOpacity()
{
    Range<float> opacityVal = Range<float>(0.5, 1);
    ParticleUpdator opacityUpdator = ParticleUpdator::CURVE;
    Range<float> opacityRandom;
    float fromValue = 0.1f;
    float toValue = 1.f;
    int startMillis = 0;
    int endMillis = 3000;
    std::shared_ptr<RSInterpolator> interpolator = nullptr;
    auto change1 = std::make_shared<ChangeInOverLife<float>>(fromValue, toValue, startMillis, endMillis, interpolator);
    auto change2 =
        std::make_shared<ChangeInOverLife<float>>(toValue, fromValue, endMillis + 4000, endMillis + 7000, interpolator);
    std::vector<std::shared_ptr<ChangeInOverLife<float>>> opacityChangeOverLife;
    opacityChangeOverLife.push_back(change1);
    opacityChangeOverLife.push_back(change2);
    opacity_ = RenderParticleParaType(opacityVal, opacityUpdator, opacityRandom, opacityChangeOverLife);
}

void RSRenderParticleEmitterTest::SetUp()
{
    SetEmitConfig();
    SetColor();
    SetOpacity();
    Range<float> scaleVal = Range<float>(0.5, 1);
    ParticleUpdator randomUpdator = ParticleUpdator::RANDOM;
    Range<float> scaleRandom = Range<float>(0.1f, 1.f);
    std::vector<std::shared_ptr<ChangeInOverLife<float>>> scaleChangeOverLife;
    scale_ = RenderParticleParaType(scaleVal, randomUpdator, scaleRandom, scaleChangeOverLife);
    Range<float> spinRandom = Range<float>(1.f, 0.f);
    spin_ = RenderParticleParaType(scaleVal, randomUpdator, spinRandom, scaleChangeOverLife);
    params = std::make_shared<ParticleRenderParams>(
        emitterConfig_, velocity_, acceleration_, color_, opacity_, scale_, spin_);
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
    ASSERT_TRUE(particleEmitter != nullptr);
    bool isFinish = particleEmitter->IsEmitterFinish();
    EXPECT_TRUE(isFinish == false);
    GTEST_LOG_(INFO) << "RSRenderParticleEmitterTest EmitParticle001 end";
}

/**
 * @tc.name: EmitParticle002
 * @tc.desc: Verify the EmitParticle when particleEmitter->particleParams_->opacity_.updator_ == NONE
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderParticleEmitterTest, EmitParticle002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderParticleEmitterTest EmitParticle002 start";
    ASSERT_TRUE(particleEmitter != nullptr);
    ASSERT_TRUE(particleEmitter->particleParams_ != nullptr);
    particleEmitter->particleParams_->opacity_.updator_ = ParticleUpdator::NONE;
    particleEmitter->EmitParticle(NS_TO_S);
    bool isFinish = particleEmitter->IsEmitterFinish();
    EXPECT_TRUE(isFinish == false);
    GTEST_LOG_(INFO) << "RSRenderParticleEmitterTest EmitParticle002 end";
}

/**
 * @tc.name: EmitParticle003
 * @tc.desc: Verify the EmitParticle when particleEmitter->particleParams_ == nullptr
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderParticleEmitterTest, EmitParticle003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderParticleEmitterTest EmitParticle003 start";
    ASSERT_TRUE(particleEmitter != nullptr);
    particleEmitter->particleParams_ = nullptr;
    particleEmitter->EmitParticle(NS_TO_S);
    bool isFinish = particleEmitter->IsEmitterFinish();
    EXPECT_TRUE(isFinish == true);
    GTEST_LOG_(INFO) << "RSRenderParticleEmitterTest EmitParticle003 end";
}

/**
 * @tc.name: PreEmitTest001
 * @tc.desc: Verify the PreEmit
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderParticleEmitterTest, PreEmitTest001, TestSize.Level1)
{
    auto params = std::make_shared<ParticleRenderParams>();
    RSRenderParticleEmitter emitter(params);

    // case1: updater random and end value <= 0 and random end <= 0
    params->opacity_.updator_ = ParticleUpdator::RANDOM;
    params->opacity_.val_.end_ = 0.f;
    params->opacity_.random_.end_ = 0.f;
    emitter.emitFinish_ = false;
    emitter.PreEmit();
    ASSERT_TRUE(emitter.emitFinish_);

    // case2: updater random and end value <= 0 and random end > 0
    params->opacity_.val_.end_ = 0.f;
    params->opacity_.random_.end_ = 1.f;
    emitter.emitFinish_ = false;
    emitter.PreEmit();
    ASSERT_TRUE(emitter.emitFinish_);

    // case3: updater random and end value > 0 and random end > 0
    params->opacity_.val_.end_ = 1.f;
    params->opacity_.random_.end_ = 1.f;
    emitter.emitFinish_ = false;
    emitter.PreEmit();
    ASSERT_TRUE(emitter.emitFinish_);

    // case4: not random updater
    params->opacity_.updator_ = ParticleUpdator::CURVE;
    emitter.emitFinish_ = false;
    emitter.PreEmit();
    ASSERT_TRUE(emitter.emitFinish_);
}

/**
 * @tc.name: PreEmitTest002
 * @tc.desc: Verify the PreEmit
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderParticleEmitterTest, PreEmitTest002, TestSize.Level1)
{
    auto params = std::make_shared<ParticleRenderParams>();
    RSRenderParticleEmitter emitter(params);
    params->opacity_.updator_ = ParticleUpdator::CURVE;

    // case1: updator none and endvalue <= 0
    params->scale_.updator_ = ParticleUpdator::NONE;
    params->scale_.val_.end_ = 0.f;
    emitter.emitFinish_ = false;
    emitter.PreEmit();
    ASSERT_TRUE(emitter.emitFinish_);

    // case2: updator none and endvalue > 0
    params->scale_.val_.end_ = 1.f;
    emitter.emitFinish_ = false;
    emitter.PreEmit();
    ASSERT_TRUE(emitter.emitFinish_);

    // case3: updater random and end value <= 0 and random end <= 0
    params->scale_.updator_ = ParticleUpdator::RANDOM;
    params->scale_.val_.end_ = 0.f;
    params->scale_.random_.end_ = 0.f;
    emitter.emitFinish_ = false;
    emitter.PreEmit();
    ASSERT_TRUE(emitter.emitFinish_);

    // case4: updater random and end value <= 0 and random end > 0
    params->scale_.val_.end_ = 0.f;
    params->scale_.random_.end_ = 1.f;
    emitter.emitFinish_ = false;
    emitter.PreEmit();
    ASSERT_TRUE(emitter.emitFinish_);

    // case5: updater random and end value > 0 and random end > 0
    params->scale_.val_.end_ = 1.f;
    params->scale_.random_.end_ = 1.f;
    emitter.emitFinish_ = false;
    emitter.PreEmit();
    ASSERT_TRUE(emitter.emitFinish_);

    // case6: not random updater
    params->scale_.updator_ = ParticleUpdator::CURVE;
    emitter.emitFinish_ = false;
    emitter.PreEmit();
    ASSERT_TRUE(emitter.emitFinish_);
}

/**
 * @tc.name: PreEmitTest003
 * @tc.desc: Verify the PreEmit
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderParticleEmitterTest, PreEmitTest003, TestSize.Level1)
{
    auto params = std::make_shared<ParticleRenderParams>();
    RSRenderParticleEmitter emitter(params);
    params->opacity_.updator_ = ParticleUpdator::CURVE;
    params->scale_.updator_ = ParticleUpdator::CURVE;

    // case1: image is null
    params->emitterConfig_.type_ = ParticleType::IMAGES;
    emitter.emitFinish_ = false;
    emitter.PreEmit();
    ASSERT_TRUE(emitter.emitFinish_);

    // case2: size.x <= 0
    params->emitterConfig_.image_ = std::make_shared<RSImage>();
    params->emitterConfig_.imageSize_[0] = 0.f;
    params->emitterConfig_.imageSize_[1] = 0.f;
    emitter.emitFinish_ = false;
    emitter.PreEmit();
    ASSERT_TRUE(emitter.emitFinish_);

    // case3: size.x > 0 and size.y <= 0
    params->emitterConfig_.imageSize_[0] = 1.f;
    params->emitterConfig_.imageSize_[1] = 0.f;
    emitter.emitFinish_ = false;
    emitter.PreEmit();
    ASSERT_TRUE(emitter.emitFinish_);

    // case4: test radius
    params->emitterConfig_.type_ = ParticleType::POINTS;
    params->emitterConfig_.radius_ = 0.f;
    emitter.emitFinish_ = false;
    emitter.PreEmit();
    params->emitterConfig_.radius_ = 1.f;
    emitter.PreEmit();
    ASSERT_TRUE(emitter.emitFinish_);
}

/**
 * @tc.name: EmitParticleTest
 * @tc.desc: Verify the EmitParticle
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderParticleEmitterTest, EmitParticleTest, TestSize.Level1)
{
    auto params = std::make_shared<ParticleRenderParams>();
    RSRenderParticleEmitter emitter(params);
    emitter.emitFinish_ = false;

    // case1: particleCount_ < 0
    params->emitterConfig_.particleCount_ = -1;
    emitter.EmitParticle(0);
    params->emitterConfig_.particleCount_ = -2;
    emitter.EmitParticle(0);
    ASSERT_TRUE(emitter.emitFinish_);

    // case2: lifeTime end
    params->emitterConfig_.particleCount_ = 1;
    params->emitterConfig_.lifeTime_.start_ = 0;
    params->emitterConfig_.lifeTime_.end_ = 0;
    emitter.emitFinish_ = false;
    emitter.EmitParticle(0);
    ASSERT_TRUE(emitter.emitFinish_);

    // case3: particleCount_ > particleCount_
    params->emitterConfig_.particleCount_ = 1;
    params->emitterConfig_.lifeTime_.start_ = 0;
    params->emitterConfig_.lifeTime_.end_ = 1;
    emitter.particleCount_ = 2;
    emitter.emitFinish_ = false;
    emitter.EmitParticle(0);
    ASSERT_TRUE(emitter.emitFinish_);
}
} // namespace Rosen
} // namespace OHOS