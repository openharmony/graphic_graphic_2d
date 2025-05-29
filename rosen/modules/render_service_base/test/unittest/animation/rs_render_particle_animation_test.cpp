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
#include "include/command/rs_animation_command.h"

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
    ParticleVelocity velocity;
    RenderParticleAcceleration acceleration;
    RenderParticleColorParaType color;
    RenderParticleParaType<float> opacity;
    RenderParticleParaType<float> scale;
    RenderParticleParaType<float> spin;
    std::shared_ptr<ParticleRenderParams> param;
    std::shared_ptr<ParticleRenderParams> param1;
    std::shared_ptr<ParticleRenderParams> param2;
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
    Vector2f emitSize = Vector2f(10.f, 20.f); // 10.f is width, 20.f is height
    int particleCount = 20;
    Range<int64_t> lifeTime = Range<int64_t>(1000, 3000); // 1000 is lifeTime range start, 3000 is lifeTime range end.
    ParticleType type = ParticleType::POINTS;
    float radius = 10.f;
    std::shared_ptr<RSImage> image;
    Vector2f imageSize = Vector2f(1.f, 1.f);
    EmitterConfig emitterConfig =
        EmitterConfig(emitRate, emitShape, position, emitSize, particleCount, lifeTime, type, radius, image, imageSize);

    param = std::make_shared<ParticleRenderParams>(emitterConfig, velocity, acceleration, color, opacity, scale, spin);

    int emitRate1 = 0;
    ShapeType emitShape1 = ShapeType::CIRCLE;
    Vector2f position1 = Vector2f(100.f, 100.f);
    Vector2f emitSize1 = Vector2f(200.f, 200.f);
    int particleCount1 = -1;
    Range<int64_t> lifeTime1 = Range<int64_t>(-1, -1);
    ParticleType type1 = ParticleType::IMAGES;
    float radius1 = 0.f;
    std::shared_ptr<RSImage> image1;
    Vector2f imageSize1 = Vector2f(1.f, 1.f);
    EmitterConfig emitterConfig1 = EmitterConfig(
        emitRate1, emitShape1, position1, emitSize1, particleCount1, lifeTime1, type1, radius1, image1, imageSize1);
    param1 =
        std::make_shared<ParticleRenderParams>(emitterConfig1, velocity, acceleration, color, opacity, scale, spin);

    int particleCount2 = -10;
    Range<int64_t> lifeTime2 = Range<int64_t>(0, 0);
    EmitterConfig emitterConfig2 = EmitterConfig(
        emitRate, emitShape, position, emitSize, particleCount2, lifeTime2, type, radius1, image, imageSize);

    param2 =
        std::make_shared<ParticleRenderParams>(emitterConfig2, velocity, acceleration, color, opacity, scale, spin);

    particlesRenderParams.push_back(param);
    particlesRenderParams.push_back(param1);
    particlesRenderParams.push_back(param2);
}
void RSRenderParticleAnimationTest::TearDown() {}

/**
 * @tc.name: Animate001
 * @tc.desc: Verify the Animate
 * @tc.type:FUNC
 * @tc.require: issueIA6IWR
 */
HWTEST_F(RSRenderParticleAnimationTest, Animate001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderParticleAnimationTest Animate001 start";
    auto renderParticleAnimation =
        std::make_shared<RSRenderParticleAnimation>(ANIMATION_ID, PROPERTY_ID, particlesRenderParams);
    ASSERT_TRUE(renderParticleAnimation != nullptr);
    int64_t leftDelayTime = 0;
    auto particleAnimate = renderParticleAnimation->Animate(NS_TO_S, leftDelayTime);
    EXPECT_TRUE(particleAnimate);

    particleSystem_ = std::make_shared<RSRenderParticleSystem>(particlesRenderParams);
    ASSERT_TRUE(particleSystem_ != nullptr);

    std::vector<std::shared_ptr<RSRenderParticle>> activeParticles;
    std::vector<std::shared_ptr<RSImage>> imageVector;
    particleSystem_->Emit(NS_TO_S, activeParticles, imageVector);
    particleSystem_->UpdateParticle(NS_TO_S, activeParticles);
    bool finish = particleSystem_->IsFinish(activeParticles);
    EXPECT_TRUE(finish);
    particleSystem_->particlesRenderParams_ = {};
    particleSystem_->CreateEmitter();
    particleSystem_->Emit(NS_TO_S, activeParticles, imageVector);
    auto particleParams = std::make_shared<ParticleRenderParams>();
    std::shared_ptr<RSRenderParticle> newParam = std::make_shared<RSRenderParticle>(particleParams);
    activeParticles.push_back(newParam);
    activeParticles.push_back(nullptr);
    particleSystem_->UpdateParticle(NS_TO_S, activeParticles);
    finish = particleSystem_->IsFinish(activeParticles);
    EXPECT_TRUE(finish);
    GTEST_LOG_(INFO) << "RSRenderParticleAnimationTest Animate001 end";
}

/**
 * @tc.name: Animate002
 * @tc.desc: Verify the Animate
 * @tc.type:FUNC
 * @tc.require: issueIA6IWR
 */
HWTEST_F(RSRenderParticleAnimationTest, Animate002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderParticleAnimationTest Animate002 start";
    auto renderParticleAnimation =
        std::make_shared<RSRenderParticleAnimation>(ANIMATION_ID, PROPERTY_ID, particlesRenderParams);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    ASSERT_TRUE(renderParticleAnimation != nullptr);
    renderParticleAnimation->Attach(renderNode.get());
    renderParticleAnimation->OnAttach();
    ASSERT_TRUE(renderParticleAnimation != nullptr);
    GTEST_LOG_(INFO) << "RSRenderParticleAnimationTest Animate002 end";
}

/**
 * @tc.name: Animate003
 * @tc.desc: Verify the Animate
 * @tc.type:FUNC
 * @tc.require: issueIA6IWR
 */
HWTEST_F(RSRenderParticleAnimationTest, Animate003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderParticleAnimationTest Animate003 start";
    int emitRate = 20;
    ShapeType emitShape = ShapeType::RECT;
    Vector2f position = Vector2f(0.f, 0.f);
    Vector2f emitSize = Vector2f(10.f, 20.f); // 10.f is width, 20.f is height
    int particleCount = 0;
    Range<int64_t> lifeTime = Range<int64_t>(0, 0); // 1000 is lifeTime range start, 3000 is lifeTime range end.
    ParticleType type = ParticleType::POINTS;
    float radius = 10.f;
    std::shared_ptr<RSImage> image;
    Vector2f imageSize = Vector2f(1.f, 1.f);
    EmitterConfig emitterConfig =
        EmitterConfig(emitRate, emitShape, position, emitSize, particleCount, lifeTime, type, radius, image, imageSize);

    param = std::make_shared<ParticleRenderParams>(emitterConfig, velocity, acceleration, color, opacity, scale, spin);

    std::vector<std::shared_ptr<ParticleRenderParams>> particlesRenderParams1;
    particlesRenderParams1.push_back(param);

    NodeId targetId = static_cast<NodeId>(PROPERTY_ID);
    auto animation =
        std::make_shared<RSRenderParticleAnimation>(ANIMATION_ID, PROPERTY_ID, std::move(particlesRenderParams1));
    OHOS::Rosen::RSContext context;
    AnimationCommandHelper::CreateParticleAnimation(context, targetId, animation);
    RSRenderParticleVector particles;
    auto particleParams = std::make_shared<ParticleRenderParams>();
    std::shared_ptr<RSRenderParticle> newParam = std::make_shared<RSRenderParticle>(particleParams);
    particles.renderParticleVector_.push_back(newParam);
    auto property = std::make_shared<RSRenderProperty<RSRenderParticleVector>>(particles, PROPERTY_ID);
    ASSERT_TRUE(animation != nullptr);
    animation->AttachRenderProperty(property);
    int64_t leftDelayTime = 0;
    EXPECT_TRUE(animation->Animate(NS_TO_S, leftDelayTime));
    animation->particleSystem_ = nullptr;
    animation->SetPropertyValue(nullptr);
    EXPECT_TRUE(animation->Animate(NS_TO_S, leftDelayTime));
    GTEST_LOG_(INFO) << "RSRenderParticleAnimationTest Animate003 end";
}

/**
 * @tc.name: UpdateEmitter001
 * @tc.desc: Verify the UpdateEmitter
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderParticleAnimationTest, UpdateEmitter001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderParticleAnimationTest UpdateEmitter001 start";
    uint32_t emitterIndex = 0;
    Vector2f position = { 200.f, 300.f };
    Vector2f emitSize = { 400.f, 500.f };
    int emitRate = 100;
    auto para = std::make_shared<EmitterUpdater>(emitterIndex, position, emitSize, emitRate);
    std::vector<std::shared_ptr<EmitterUpdater>> emitUpdate;
    emitUpdate.push_back(para);
    auto renderParticleAnimation =
        std::make_shared<RSRenderParticleAnimation>(ANIMATION_ID, PROPERTY_ID, particlesRenderParams);
    ASSERT_TRUE(renderParticleAnimation != nullptr);
    renderParticleAnimation->UpdateEmitter(emitUpdate);
    auto particleSystem = renderParticleAnimation->GetParticleSystem();
    ASSERT_TRUE(particleSystem != nullptr);
    std::vector<std::shared_ptr<RSRenderParticle>> activeParticles;
    auto particle = std::make_shared<RSRenderParticle>(param);
    activeParticles.push_back(particle);
    particleSystem->UpdateParticle(NS_TO_S, activeParticles);
    auto emitters = particleSystem->GetParticleEmitter();
    ASSERT_TRUE(emitters.size() != 0);
    ASSERT_TRUE(emitters[emitterIndex] != nullptr);
    auto particleParams = emitters[emitterIndex]->GetParticleParams();
    ASSERT_TRUE(particleParams != nullptr);
    EXPECT_TRUE(particleParams->emitterConfig_.position_ == position);
    EXPECT_TRUE(particleParams->emitterConfig_.emitSize_ == emitSize);
    EXPECT_TRUE(particleParams->emitterConfig_.emitRate_ == emitRate);
    GTEST_LOG_(INFO) << "RSRenderParticleAnimationTest UpdateEmitter001 end";
}

/**
 * @tc.name: UpdateEmitter002
 * @tc.desc: Verify the UpdateEmitter
 * @tc.type:FUNC
 * @tc.require: issueIA6IWR
 */
HWTEST_F(RSRenderParticleAnimationTest, UpdateEmitter002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderParticleAnimationTest UpdateEmitter002 start";
    std::vector<std::shared_ptr<EmitterUpdater>> emitUpdate;
    auto renderParticleAnimation =
        std::make_shared<RSRenderParticleAnimation>(ANIMATION_ID, PROPERTY_ID, particlesRenderParams);
    ASSERT_TRUE(renderParticleAnimation != nullptr);
    renderParticleAnimation->UpdateEmitter(emitUpdate);
    auto particleSystem = renderParticleAnimation->GetParticleSystem();
    ASSERT_TRUE(particleSystem != nullptr);
    auto emitters = particleSystem->GetParticleEmitter();
    EXPECT_TRUE(emitters.size() != 0);
    GTEST_LOG_(INFO) << "RSRenderParticleAnimationTest UpdateEmitter002 end";
}

/**
 * @tc.name: UpdateEmitter003
 * @tc.desc: Verify the UpdateEmitter emitterUpdater is nullptr
 * @tc.type:FUNC
 * @tc.require: issueIA6IWR
 */
HWTEST_F(RSRenderParticleAnimationTest, UpdateEmitter003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderParticleAnimationTest UpdateEmitter003 start";
    std::shared_ptr<EmitterUpdater> para = nullptr;
    std::vector<std::shared_ptr<EmitterUpdater>> emitUpdate;
    emitUpdate.push_back(para);
    auto renderParticleAnimation =
        std::make_shared<RSRenderParticleAnimation>(ANIMATION_ID, PROPERTY_ID, particlesRenderParams);
    ASSERT_TRUE(renderParticleAnimation != nullptr);
    renderParticleAnimation->UpdateEmitter(emitUpdate);
    auto particleSystem = renderParticleAnimation->GetParticleSystem();
    EXPECT_TRUE(particleSystem != nullptr);
    GTEST_LOG_(INFO) << "RSRenderParticleAnimationTest UpdateEmitter003 end";
}

/**
 * @tc.name: UpdateEmitter004
 * @tc.desc: Verify the UpdateEmitter emitterIndex >= particlesRenderParams_.size()
 * @tc.type:FUNC
 * @tc.require: issueIA6IWR
 */
HWTEST_F(RSRenderParticleAnimationTest, UpdateEmitter004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderParticleAnimationTest UpdateEmitter004 start";
    uint32_t emitterIndex = 3;
    Vector2f position = { 200.f, 300.f };
    Vector2f emitSize = { 400.f, 500.f };
    int emitRate = 10;
    auto para = std::make_shared<EmitterUpdater>(emitterIndex, position, emitSize, emitRate);
    std::vector<std::shared_ptr<EmitterUpdater>> emitUpdate;
    emitUpdate.push_back(para);
    auto renderParticleAnimation =
        std::make_shared<RSRenderParticleAnimation>(ANIMATION_ID, PROPERTY_ID, particlesRenderParams);
    ASSERT_TRUE(renderParticleAnimation != nullptr);
    renderParticleAnimation->UpdateEmitter(emitUpdate);
    auto particleSystem = renderParticleAnimation->GetParticleSystem();
    ASSERT_TRUE(particleSystem != nullptr);
    auto emitters = particleSystem->GetParticleEmitter();
    ASSERT_TRUE(emitters.size() != 0);
    ASSERT_TRUE(emitters[0] != nullptr);
    auto particleParams = emitters[0]->GetParticleParams();
    ASSERT_TRUE(particleParams != nullptr);
    EXPECT_TRUE(particleParams->emitterConfig_.position_ != position);
    EXPECT_TRUE(particleParams->emitterConfig_.emitSize_ != emitSize);
    EXPECT_TRUE(particleParams->emitterConfig_.emitRate_ != emitRate);
    GTEST_LOG_(INFO) << "RSRenderParticleAnimationTest UpdateEmitter004 end";
}

/**
 * @tc.name: UpdateEmitter005
 * @tc.desc: Verify the UpdateEmitter emitterIndex >= particlesRenderParams_.size()
 * @tc.type:FUNC
 * @tc.require: issueIA6IWR
 */
HWTEST_F(RSRenderParticleAnimationTest, UpdateEmitter005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderParticleAnimationTest UpdateEmitter005 start";
    int emitRate = 20;
    ShapeType emitShape = ShapeType::RECT;
    Vector2f position = Vector2f(0.f, 0.f);
    Vector2f emitSize = Vector2f(10.f, 20.f); // 10.f is width, 20.f is height
    int particleCount = 0;
    Range<int64_t> lifeTime = Range<int64_t>(0, 0); // 1000 is lifeTime range start, 3000 is lifeTime range end.
    ParticleType type = ParticleType::POINTS;
    float radius = 10.f;
    std::shared_ptr<RSImage> image;
    Vector2f imageSize = Vector2f(1.f, 1.f);
    EmitterConfig emitterConfig =
        EmitterConfig(emitRate, emitShape, position, emitSize, particleCount, lifeTime, type, radius, image, imageSize);
    param = std::make_shared<ParticleRenderParams>(emitterConfig, velocity, acceleration, color, opacity, scale, spin);
    std::vector<std::shared_ptr<ParticleRenderParams>> particlesRenderParams1;
    particlesRenderParams1.push_back(param);
    particlesRenderParams1.push_back(nullptr);
    auto renderParticleAnimation =
        std::make_shared<RSRenderParticleAnimation>(ANIMATION_ID, PROPERTY_ID, particlesRenderParams1);
    uint32_t emitterIndex = 0;
    position = Vector2f(0.f, 0.f);
    emitSize = Vector2f(10.f, 20.f);
    emitRate = 100;
    auto para = std::make_shared<EmitterUpdater>(emitterIndex, position, emitSize, emitRate);
    std::vector<std::shared_ptr<EmitterUpdater>> emitUpdate;
    emitUpdate.push_back(para);
    uint32_t emitterIndex1 = 1;
    Vector2f position1 = { 200.f, 300.f };
    Vector2f emitSize1 = { 400.f, 500.f };
    int emitRate1 = 10;
    auto para1 = std::make_shared<EmitterUpdater>(emitterIndex1, position1, emitSize1, emitRate1);
    emitUpdate.push_back(para1);
    ASSERT_TRUE(renderParticleAnimation != nullptr);
    renderParticleAnimation->UpdateEmitter(emitUpdate);
    auto particleSystem = renderParticleAnimation->GetParticleSystem();
    ASSERT_TRUE(particleSystem != nullptr);
    auto emitters = particleSystem->GetParticleEmitter();
    ASSERT_TRUE(emitters.size() != 0);
    ASSERT_TRUE(emitters[0] != nullptr);
    auto particleParams = emitters[0]->GetParticleParams();
    ASSERT_TRUE(particleParams != nullptr);
    EXPECT_TRUE(particleParams->emitterConfig_.position_ == position);
    EXPECT_TRUE(particleParams->emitterConfig_.emitSize_ == emitSize);
    EXPECT_TRUE(particleParams->emitterConfig_.emitRate_ == emitRate);
    GTEST_LOG_(INFO) << "RSRenderParticleAnimationTest UpdateEmitter005 end";
}

/**
 * @tc.name: UpdateEmitter006
 * @tc.desc: Verify the UpdateEmitter EmitterUpdater = particlesRenderParams_[index]->emitterConfig
 * @tc.type:FUNC
 * @tc.require: issueIA6IWR
 */
HWTEST_F(RSRenderParticleAnimationTest, UpdateEmitter006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderParticleAnimationTest UpdateEmitter006 start";
    uint32_t emitterIndex = 0;
    Vector2f position = Vector2f(0.f, 0.f);
    Vector2f emitSize = Vector2f(10.f, 10.f);
    int emitRate = 20;
    auto para = std::make_shared<EmitterUpdater>(emitterIndex, position, emitSize, emitRate);
    uint32_t emitterIndex1 = 1;
    auto para1 = std::make_shared<EmitterUpdater>(emitterIndex1);
    std::vector<std::shared_ptr<EmitterUpdater>> emitUpdate;
    emitUpdate.push_back(para);
    emitUpdate.push_back(para1);
    auto renderParticleAnimation =
        std::make_shared<RSRenderParticleAnimation>(ANIMATION_ID, PROPERTY_ID, particlesRenderParams);
    ASSERT_TRUE(renderParticleAnimation != nullptr);
    renderParticleAnimation->UpdateEmitter(emitUpdate);
    auto particleSystem = renderParticleAnimation->GetParticleSystem();
    ASSERT_TRUE(particleSystem != nullptr);
    auto emitters = particleSystem->GetParticleEmitter();
    ASSERT_TRUE(emitters.size() != 0);
    ASSERT_TRUE(emitters[emitterIndex] != nullptr);
    auto particleParams = emitters[emitterIndex]->GetParticleParams();
    ASSERT_TRUE(particleParams != nullptr);
    EXPECT_TRUE(particleParams->emitterConfig_.position_ == position);
    EXPECT_TRUE(particleParams->emitterConfig_.emitSize_ == emitSize);
    EXPECT_TRUE(particleParams->emitterConfig_.emitRate_ == emitRate);
    GTEST_LOG_(INFO) << "RSRenderParticleAnimationTest UpdateEmitter006 end";
}

/**
 * @tc.name: UpdateParamsIfChanged001
 * @tc.desc: Verify the UpdateParamsIfChanged
 * @tc.type:FUNC
 * @tc.require: issueIA6IWR
 */
HWTEST_F(RSRenderParticleAnimationTest, UpdateParamsIfChanged001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderParticleAnimationTest UpdateParamsIfChanged001 start";
    uint32_t emitterIndex = 0;
    Vector2f position = Vector2f(0.f, 0.f);
    Vector2f emitSize = Vector2f(10.f, 10.f);
    int emitRate = 20;
    auto para = std::make_shared<EmitterUpdater>(emitterIndex, position, emitSize, emitRate);
    AnnulusRegion annulusRegion;
    std::shared_ptr<AnnulusRegion> region = std::make_shared<AnnulusRegion>(annulusRegion);
    para->SetShape(region);
    std::vector<std::shared_ptr<EmitterUpdater>> emitUpdate;
    emitUpdate.push_back(para);
    auto renderParticleAnimation =
        std::make_shared<RSRenderParticleAnimation>(ANIMATION_ID, PROPERTY_ID, particlesRenderParams);
    renderParticleAnimation->UpdateEmitter(emitUpdate);
    auto particleSystem = renderParticleAnimation->GetParticleSystem();
    auto shape = para->shape_;
    auto annulusPtr = std::static_pointer_cast<AnnulusRegion>(shape);
    annulusPtr->innerRadius_ = 10;
    annulusPtr->outerRadius_ = 20;
    auto configRegion = particlesRenderParams[emitterIndex]->emitterConfig_.shape_;
    auto annulusRegionPtr = std::static_pointer_cast<AnnulusRegion>(configRegion);
    renderParticleAnimation->UpdateParamsIfChanged(para->shape_, configRegion);
    EXPECT_TRUE(annulusRegionPtr->innerRadius_ == 10);
    EXPECT_TRUE(annulusRegionPtr->outerRadius_ == 20);
    GTEST_LOG_(INFO) << "RSRenderParticleAnimationTest UpdateParamsIfChanged001 end";
}

/**
 * @tc.name: UpdateNoiseField001
 * @tc.desc: Verify the UpdateNoiseField
 * @tc.type:FUNC
 * @tc.require: issueIA6IWR
 */
HWTEST_F(RSRenderParticleAnimationTest, UpdateNoiseField001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderParticleAnimationTest UpdateNoiseField001 start";
    auto renderParticleAnimation =
        std::make_shared<RSRenderParticleAnimation>(ANIMATION_ID, PROPERTY_ID, particlesRenderParams);
    ASSERT_TRUE(renderParticleAnimation != nullptr);
    int fieldStrength = 10;
    ShapeType fieldShape = ShapeType::RECT;
    Vector2f fieldSize = { 200.f, 200.f };
    Vector2f fieldCenter = { 40.f, 50.f };
    uint16_t fieldFeather = 50;
    float noiseScale = 8.f;
    float noiseFrequency = 2.f;
    float noiseAmplitude = 4.f;
    auto noiseFiled = std::make_shared<ParticleNoiseField>(
        fieldStrength, fieldShape, fieldSize, fieldCenter, fieldFeather, noiseScale, noiseFrequency, noiseAmplitude);
    auto particleNoiseFields = std::make_shared<ParticleNoiseFields>();
    ASSERT_TRUE(particleNoiseFields != nullptr);
    particleNoiseFields->AddField(noiseFiled);
    renderParticleAnimation->UpdateNoiseField(particleNoiseFields);
    auto particleSystem = renderParticleAnimation->GetParticleSystem();
    ASSERT_TRUE(particleSystem != nullptr);
    auto noiseFields = particleSystem->GetParticleNoiseFields();
    ASSERT_TRUE(noiseFields != nullptr);
    EXPECT_TRUE(noiseFields->GetFieldCount() == particleNoiseFields->GetFieldCount());
    EXPECT_TRUE(noiseFields->GetField(0) == particleNoiseFields->GetField(0));
    GTEST_LOG_(INFO) << "RSRenderParticleAnimationTest UpdateNoiseField001 end";
}

/**
 * @tc.name: UpdateNoiseField002
 * @tc.desc: Verify the UpdateNoiseField particleNoiseFields == nullptr
 * @tc.type:FUNC
 * @tc.require: issueIA6IWR
 */
HWTEST_F(RSRenderParticleAnimationTest, UpdateNoiseField002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderParticleAnimationTest UpdateNoiseField002 start";
    auto renderParticleAnimation =
        std::make_shared<RSRenderParticleAnimation>(ANIMATION_ID, PROPERTY_ID, particlesRenderParams);
    ASSERT_TRUE(renderParticleAnimation != nullptr);
    std::shared_ptr<ParticleNoiseFields> particleNoiseFields = nullptr;
    renderParticleAnimation->UpdateNoiseField(particleNoiseFields);
    auto particleSystem = renderParticleAnimation->GetParticleSystem();
    ASSERT_TRUE(particleSystem != nullptr);
    auto noiseFields = particleSystem->GetParticleNoiseFields();
    EXPECT_TRUE(noiseFields == nullptr);
    GTEST_LOG_(INFO) << "RSRenderParticleAnimationTest UpdateNoiseField002 end";
}

/**
 * @tc.name: UpdateNoiseField003
 * @tc.desc: Verify the UpdateNoiseField *particleNoiseFields_ == *particleNoiseFields
 * @tc.type:FUNC
 * @tc.require: issueIA6IWR
 */
HWTEST_F(RSRenderParticleAnimationTest, UpdateNoiseField003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderParticleAnimationTest UpdateNoiseField003 start";
    auto renderParticleAnimation =
        std::make_shared<RSRenderParticleAnimation>(ANIMATION_ID, PROPERTY_ID, particlesRenderParams);
    ASSERT_TRUE(renderParticleAnimation != nullptr);
    int fieldStrength = 10;
    ShapeType fieldShape = ShapeType::RECT;
    Vector2f fieldSize = { 200.f, 200.f };
    Vector2f fieldCenter = { 40.f, 50.f };
    uint16_t fieldFeather = 50;
    float noiseScale = 8.f;
    float noiseFrequency = 2.f;
    float noiseAmplitude = 4.f;
    auto noiseFiled = std::make_shared<ParticleNoiseField>(
        fieldStrength, fieldShape, fieldSize, fieldCenter, fieldFeather, noiseScale, noiseFrequency, noiseAmplitude);
    auto particleNoiseFields = std::make_shared<ParticleNoiseFields>();
    particleNoiseFields->AddField(noiseFiled);
    renderParticleAnimation->UpdateNoiseField(particleNoiseFields);
    auto particleSystem = renderParticleAnimation->GetParticleSystem();
    ASSERT_TRUE(particleSystem != nullptr);
    auto noiseFields = particleSystem->GetParticleNoiseFields();
    ASSERT_TRUE(noiseFields != nullptr);
    EXPECT_TRUE(noiseFields->GetFieldCount() == particleNoiseFields->GetFieldCount());
    EXPECT_TRUE(noiseFields->GetField(0) == particleNoiseFields->GetField(0));
    renderParticleAnimation->particleSystem_ = nullptr;
    renderParticleAnimation->UpdateNoiseField(particleNoiseFields);
    GTEST_LOG_(INFO) << "RSRenderParticleAnimationTest UpdateNoiseField003 end";
}

/**
 * @tc.name: OnAttach001
 * @tc.desc: Verify the OnAttach
 * @tc.type:FUNC
 * @tc.require: issueIA6IWR
 */
HWTEST_F(RSRenderParticleAnimationTest, OnAttach001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderParticleAnimationTest OnAttach001 start";

    auto renderParticleAnimation =
        std::make_shared<RSRenderParticleAnimation>(ANIMATION_ID, PROPERTY_ID, particlesRenderParams);
    ASSERT_TRUE(renderParticleAnimation != nullptr);
    renderParticleAnimation->Attach(nullptr);
    EXPECT_TRUE(renderParticleAnimation->IsRunning());
    GTEST_LOG_(INFO) << "RSRenderParticleAnimationTest OnAttach001 end";
}

/**
 * @tc.name: OnDetach001
 * @tc.desc: Verify the OnDetach
 * @tc.type:FUNC
 * @tc.require: issueIA6IWR
 */
HWTEST_F(RSRenderParticleAnimationTest, OnDetach001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderParticleAnimationTest OnDetach001 start";

    auto renderParticleAnimation =
        std::make_shared<RSRenderParticleAnimation>(ANIMATION_ID, PROPERTY_ID, particlesRenderParams);
    ASSERT_TRUE(renderParticleAnimation != nullptr);
    renderParticleAnimation->Detach();
    auto target = renderParticleAnimation->GetTarget();
    EXPECT_TRUE(target == nullptr);
    GTEST_LOG_(INFO) << "RSRenderParticleAnimationTest OnDetach001 end";
}

/**
 * @tc.name: OnDetach002
 * @tc.desc: Verify the OnDetach
 * @tc.type:FUNC
 * @tc.require: issueIA6IWR
 */
HWTEST_F(RSRenderParticleAnimationTest, OnDetach002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderParticleAnimationTest OnDetach002 start";
    auto renderParticleAnimation =
        std::make_shared<RSRenderParticleAnimation>(ANIMATION_ID, PROPERTY_ID, particlesRenderParams);
    ASSERT_TRUE(renderParticleAnimation != nullptr);
    renderParticleAnimation->OnDetach();
    GTEST_LOG_(INFO) << "RSRenderParticleAnimationTest OnDetach002 end";
}

/**
 * @tc.name: Marshalling001
 * @tc.desc: Verify the Marshalling
 * @tc.type:FUNC
 * @tc.require: issueIA6IWR
 */
HWTEST_F(RSRenderParticleAnimationTest, Marshalling001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderParticleAnimationTest Marshalling001 start";

    auto renderParticleAnimation =
        std::make_shared<RSRenderParticleAnimation>(ANIMATION_ID, PROPERTY_ID, particlesRenderParams);
    ASSERT_TRUE(renderParticleAnimation != nullptr);
    Parcel parcel;
    renderParticleAnimation->Marshalling(parcel);
    EXPECT_TRUE(renderParticleAnimation != nullptr);
    GTEST_LOG_(INFO) << "RSRenderParticleAnimationTest Marshalling001 end";
}

/**
 * @tc.name: Unmarshalling001
 * @tc.desc: Verify the Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issueIA6IWR
 */
HWTEST_F(RSRenderParticleAnimationTest, Unmarshalling001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderParticleAnimationTest Unmarshalling001 start";

    auto renderParticleAnimation =
        std::make_shared<RSRenderParticleAnimation>(ANIMATION_ID, PROPERTY_ID, particlesRenderParams);
    ASSERT_TRUE(renderParticleAnimation != nullptr);
    Parcel parcel;
    renderParticleAnimation->Marshalling(parcel);
    std::shared_ptr<RSRenderAnimation>(RSRenderParticleAnimation::Unmarshalling(parcel));
    EXPECT_TRUE(renderParticleAnimation != nullptr);
    GTEST_LOG_(INFO) << "RSRenderParticleAnimationTest Unmarshalling001 end";
}
} // namespace Rosen
} // namespace OHOS