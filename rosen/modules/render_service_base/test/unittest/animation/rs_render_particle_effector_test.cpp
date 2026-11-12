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

#include "animation/rs_particle_field_collection.h"
#include "animation/rs_particle_noise_field.h"
#include "animation/rs_render_particle.h"
#include "animation/rs_render_particle_effector.h"
#include "animation/rs_render_particle_pool.h"

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
    void SetEmitConfig();
    void SetColor();
    void SetOpacity();
    void CreateParticle(const std::shared_ptr<ParticleRenderParams>& renderParams);
    EmitterConfig emitterConfig_;
    ParticleVelocity velocity_;
    RenderParticleAcceleration acceleration_;
    RenderParticleColorParaType color_;
    RenderParticleParaType<float> opacity_;
    RenderParticleParaType<float> scale_;
    RenderParticleParaType<float> spin_;

    std::shared_ptr<ParticleRenderParams> params;
    std::shared_ptr<RSRenderParticlePool> pool;
    size_t slot = 0;
    std::shared_ptr<RSRenderParticleEffector> effector;
    float deltaTime = 0.1f;
    int64_t activeTime = 100000;
};

void RSRenderParticleEffectorTest::SetUpTestCase() {}
void RSRenderParticleEffectorTest::TearDownTestCase() {}
void RSRenderParticleEffectorTest::SetEmitConfig()
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

void RSRenderParticleEffectorTest::SetColor()
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

void RSRenderParticleEffectorTest::SetOpacity()
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

void RSRenderParticleEffectorTest::SetUp()
{
    SetEmitConfig();
    SetColor();
    SetOpacity();
    Range<float> scaleVal = Range<float>(0.5, 1);
    ParticleUpdator scaleUpdator = ParticleUpdator::RANDOM;
    Range<float> scaleRandom = Range<float>(0.1f, 1.f);
    std::vector<std::shared_ptr<ChangeInOverLife<float>>> scaleChangeOverLife;
    scale_ = RenderParticleParaType(scaleVal, scaleUpdator, scaleRandom, scaleChangeOverLife);
    Range<float> spinRandom = Range<float>(1.f, 0.1f);
    spin_ = RenderParticleParaType(scaleVal, scaleUpdator, spinRandom, scaleChangeOverLife);
    params = std::make_shared<ParticleRenderParams>(
        emitterConfig_, velocity_, acceleration_, color_, opacity_, scale_, spin_);
    CreateParticle(params);
    effector = std::make_shared<RSRenderParticleEffector>();
}

void RSRenderParticleEffectorTest::TearDown() {}

void RSRenderParticleEffectorTest::CreateParticle(const std::shared_ptr<ParticleRenderParams>& renderParams)
{
    params = renderParams;
    pool = std::make_shared<RSRenderParticlePool>();
    slot = pool->Acquire();
    if (params != nullptr) {
        InitParticle(*pool, slot, *params, 0);
    }
}

/**
 * @tc.name: UpdateColor001
 * @tc.desc: Verify the color update for point particles and the image skip
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderParticleEffectorTest, UpdateColor001, TestSize.Level1)
{
    ASSERT_TRUE(effector != nullptr);
    effector->UpdateColor(*pool, slot, *params, deltaTime);
    EXPECT_TRUE(pool->GetColor(slot) != Color::FromArgbInt(0x00000000));
    pool->SetParticleType(slot, ParticleType::IMAGES);
    const Color before = pool->GetColor(slot);
    effector->UpdateColor(*pool, slot, *params, deltaTime);
    EXPECT_TRUE(pool->GetColor(slot) == before);
}

/**
 * @tc.name: UpdateOpacity001
 * @tc.desc: Verify the opacity update
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderParticleEffectorTest, UpdateOpacity001, TestSize.Level1)
{
    ASSERT_TRUE(effector != nullptr);
    effector->UpdateOpacity(*pool, slot, *params, deltaTime);
    EXPECT_TRUE(pool->GetOpacity(slot) != 0.f);
}

/**
 * @tc.name: UpdateScale001
 * @tc.desc: Verify the scale update
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderParticleEffectorTest, UpdateScale001, TestSize.Level1)
{
    ASSERT_TRUE(effector != nullptr);
    effector->UpdateScale(*pool, slot, *params, deltaTime);
    EXPECT_TRUE(pool->GetScale(slot) != 0.f);
}

/**
 * @tc.name: UpdateSpin001
 * @tc.desc: Verify the spin update
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderParticleEffectorTest, UpdateSpin001, TestSize.Level1)
{
    ASSERT_TRUE(effector != nullptr);
    effector->UpdateSpin(*pool, slot, *params, deltaTime);
    EXPECT_TRUE(pool->GetSpin(slot) != 0.f);
}

/**
 * @tc.name: UpdateEffect001
 * @tc.desc: Verify the four appearance updates together
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderParticleEffectorTest, UpdateEffect001, TestSize.Level1)
{
    ASSERT_TRUE(effector != nullptr);
    effector->UpdateColor(*pool, slot, *params, deltaTime);
    effector->UpdateOpacity(*pool, slot, *params, deltaTime);
    effector->UpdateScale(*pool, slot, *params, deltaTime);
    effector->UpdateSpin(*pool, slot, *params, deltaTime);
    EXPECT_TRUE(pool->GetColor(slot) != Color::FromArgbInt(0x00000000));
    EXPECT_TRUE(pool->GetOpacity(slot) != 0.f);
    EXPECT_TRUE(pool->GetScale(slot) != 0.f);
    EXPECT_TRUE(pool->GetSpin(slot) != 0.f);
}

/**
 * @tc.name: UpdateColorCurve001
 * @tc.desc: Verify the color curve evaluation
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderParticleEffectorTest, UpdateColorCurve001, TestSize.Level1)
{
    Color from = RSColor(0, 0, 0, 255);
    Color to = RSColor(255, 0, 0, 255);
    auto change = std::make_shared<ChangeInOverLife<Color>>(from, to, 0, 3000, nullptr);
    RenderParticleColorParaType color(Range<Color>(from, from), DistributionType::UNIFORM,
        ParticleUpdator::CURVE, Range<float>(0.f, 0.f), Range<float>(0.f, 0.f), Range<float>(0.f, 0.f),
        Range<float>(0.f, 0.f), { change });
    auto renderParams = std::make_shared<ParticleRenderParams>(
        emitterConfig_, velocity_, acceleration_, color, opacity_, scale_, spin_);
    CreateParticle(renderParams);
    pool->SetActiveTime(slot, NS_TO_S / 2);
    effector->UpdateColor(*pool, slot, *renderParams, deltaTime);
    EXPECT_TRUE(pool->GetColor(slot).GetRed() > 0);
}

/**
 * @tc.name: UpdateAccelerationRandom001
 * @tc.desc: Verify the random acceleration update
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderParticleEffectorTest, UpdateAccelerationRandom001, TestSize.Level1)
{
    auto accVal = RenderParticleParaType<float>(
        Range<float>(1.f, 1.f), ParticleUpdator::RANDOM, Range<float>(0.5f, 0.5f), {});
    auto accAngle = RenderParticleParaType<float>(
        Range<float>(1.f, 1.f), ParticleUpdator::RANDOM, Range<float>(0.5f, 0.5f), {});
    RenderParticleAcceleration acceleration(accVal, accAngle);
    auto renderParams = std::make_shared<ParticleRenderParams>(
        emitterConfig_, velocity_, acceleration, color_, opacity_, scale_, spin_);
    CreateParticle(renderParams);
    const float value = pool->GetAccelerationValue(slot);
    const float angle = pool->GetAccelerationAngle(slot);
    effector->UpdateAccelerationValue(*pool, slot, *renderParams, deltaTime);
    effector->UpdateAccelerationAngle(*pool, slot, *renderParams, deltaTime);
    EXPECT_TRUE(pool->GetAccelerationValue(slot) != value);
    EXPECT_TRUE(pool->GetAccelerationAngle(slot) != angle);
}

/**
 * @tc.name: UpdateAccelerationCurve001
 * @tc.desc: Verify an expired acceleration range is tolerated
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderParticleEffectorTest, UpdateAccelerationCurve001, TestSize.Level1)
{
    auto renderParams = std::make_shared<ParticleRenderParams>(
        emitterConfig_, velocity_, acceleration_, color_, opacity_, scale_, spin_);
    CreateParticle(renderParams);
    effector->UpdateAccelerationValue(*pool, slot, *renderParams, deltaTime);
    effector->UpdateAccelerationAngle(*pool, slot, *renderParams, deltaTime);
    effector->UpdateColor(*pool, slot, *renderParams, deltaTime);
    effector->UpdateOpacity(*pool, slot, *renderParams, deltaTime);
    effector->UpdateScale(*pool, slot, *renderParams, deltaTime);
    effector->UpdateSpin(*pool, slot, *renderParams, deltaTime);
    EXPECT_GE(pool->GetScale(slot), 0.f);
}

/**
 * @tc.name: Update001
 * @tc.desc: Verify the fused update publishes a force
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderParticleEffectorTest, Update001, TestSize.Level1)
{
    int fieldStrength = 10;
    Vector2f fieldSize = { 10.f, 10.f };
    Vector2f fieldCenter = { 40.f, 50.f };
    auto noiseField = std::make_shared<ParticleNoiseField>(
        fieldStrength, ShapeType::CIRCLE, fieldSize, fieldCenter, 50, 8.f, 2.f, 4.f);
    auto fields = std::make_shared<ParticleFieldCollection>();
    fields->Add(noiseField);
    pool->SetSpinSpeed(slot, 1.f);
    const float spinBefore = pool->GetSpin(slot);
    effector->Update(*pool, slot, *params, fields, activeTime);
    EXPECT_TRUE(pool->GetSpin(slot) != spinBefore);
}

/**
 * @tc.name: Update002
 * @tc.desc: Verify Update does not advance the age (the pool pass owns the clock)
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderParticleEffectorTest, Update002, TestSize.Level1)
{
    pool->SetActiveTime(slot, activeTime);
    std::shared_ptr<ParticleFieldCollection> fields = nullptr;
    effector->Update(*pool, slot, *params, fields, activeTime);
    EXPECT_EQ(pool->GetActiveTime(slot), activeTime);
}

/**
 * @tc.name: Update003
 * @tc.desc: Verify a zero delta leaves the age untouched
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderParticleEffectorTest, Update003, TestSize.Level1)
{
    auto noiseField = std::make_shared<ParticleNoiseField>(
        10, ShapeType::CIRCLE, Vector2f(10.f, 10.f), Vector2f(40.f, 50.f), 50, 8.f, 2.f, 4.f);
    auto fields = std::make_shared<ParticleFieldCollection>();
    fields->Add(noiseField);
    effector->Update(*pool, slot, *params, fields, 0);
    EXPECT_EQ(pool->GetActiveTime(slot), 0);
}

/**
 * @tc.name: CalculateDistanceToRectangleEdge001
 * @tc.desc: Verify the CalculateDistanceToRectangleEdge
 * @tc.type:FUNC
 * @tc.require: issueIA6IWR
 */
HWTEST_F(RSRenderParticleEffectorTest, CalculateDistanceToRectangleEdge001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderParticleEffectorTest CalculateDistanceToRectangleEdge001 start";
    int fieldStrength = 10;
    ShapeType fieldShape = ShapeType::RECT;
    Vector2f fieldSize = { 10.f, 10.f };
    Vector2f fieldCenter = { 5.f, 5.f };
    uint16_t fieldFeather = 50;
    float noiseScale = 8.f;
    float noiseFrequency = 2.f;
    float noiseAmplitude = 4.f;
    auto noiseField = std::make_shared<ParticleNoiseField>(
        fieldStrength, fieldShape, fieldSize, fieldCenter, fieldFeather, noiseScale, noiseFrequency, noiseAmplitude);

    Vector2f position = { 5.f, 5.f };
    Vector2f direction = { 0.f, 0.f };
    float distance = noiseField->CalculateDistanceToRectangleEdge(position, direction, fieldCenter, fieldSize);
    EXPECT_TRUE(ROSEN_EQ(distance, 0.f));
    direction = { 1.f, 0.f };
    distance = noiseField->CalculateDistanceToRectangleEdge(position, direction, fieldCenter, fieldSize);
    EXPECT_TRUE(ROSEN_EQ(distance, 5.f));
    direction = { 0.f, 1.f };
    distance = noiseField->CalculateDistanceToRectangleEdge(position, direction, fieldCenter, fieldSize);
    EXPECT_TRUE(ROSEN_EQ(distance, 5.f));
    direction = { 1.f, 1.f };
    distance = noiseField->CalculateDistanceToRectangleEdge(position, direction, fieldCenter, fieldSize);
    EXPECT_TRUE(distance > 5.f);

    float deltaTime = 10.f;
    Vector2f force = noiseField->ApplyField(position, deltaTime);
    EXPECT_TRUE(ROSEN_EQ(force.x_, 0.f) && ROSEN_EQ(force.y_, 0.f));

    deltaTime = 0.f;
    force = noiseField->ApplyField(position, deltaTime);
    EXPECT_TRUE(ROSEN_EQ(force.x_, 0.f) && ROSEN_EQ(force.y_, 0.f));
    GTEST_LOG_(INFO) << "RSRenderParticleEffectorTest CalculateDistanceToRectangleEdge001 end";
}

/**
 * @tc.name: CalculateDistanceToEllipseEdge001
 * @tc.desc: Verify the CalculateDistanceToEllipseEdge
 * @tc.type:FUNC
 * @tc.require: issueIA6IWR
 */
HWTEST_F(RSRenderParticleEffectorTest, CalculateDistanceToEllipseEdge001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderParticleEffectorTest CalculateDistanceToEllipseEdge001 start";
    int fieldStrength = 600;
    ShapeType fieldShape = ShapeType::RECT;
    Vector2f fieldSize = { 10.f, 20.f };
    Vector2f fieldCenter = { 5.f, 5.f };
    uint16_t fieldFeather = 50;
    float noiseScale = 8.f;
    float noiseFrequency = 2.f;
    float noiseAmplitude = 4.f;
    auto noiseField = std::make_shared<ParticleNoiseField>(
        fieldStrength, fieldShape, fieldSize, fieldCenter, fieldFeather, noiseScale, noiseFrequency, noiseAmplitude);

    Vector2f position = { 5.f, 5.f };
    Vector2f direction = { 0.f, 0.f };
    float distance = noiseField->CalculateDistanceToEllipseEdge(direction, fieldCenter, fieldSize);
    EXPECT_TRUE(ROSEN_EQ(distance, 0.f));
    direction = { 1.f, 0.f };
    distance = noiseField->CalculateDistanceToEllipseEdge(direction, fieldCenter, fieldSize);
    EXPECT_TRUE(ROSEN_EQ(distance, 5.f));
    direction = { 0.f, 1.f };
    distance = noiseField->CalculateDistanceToEllipseEdge(direction, fieldCenter, fieldSize);
    EXPECT_TRUE(ROSEN_EQ(distance, 10.f));

    float deltaTime = 10.f;
    Vector2f force = noiseField->ApplyField(position, deltaTime);
    EXPECT_TRUE(ROSEN_EQ(force.x_, 0.f) && ROSEN_EQ(force.y_, 0.f));
    GTEST_LOG_(INFO) << "RSRenderParticleEffectorTest CalculateDistanceToRectangleEdge001 end";
}

/**
 * @tc.name: CalculateFeatherEffect001
 * @tc.desc: Verify the CalculateFeatherEffect
 * @tc.type:FUNC
 * @tc.require: issueIA6IWR
 */
HWTEST_F(RSRenderParticleEffectorTest, CalculateFeatherEffect001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderParticleEffectorTest CalculateFeatherEffect001 start";
    int fieldStrength = -10;
    ShapeType fieldShape = ShapeType::CIRCLE;
    Vector2f fieldSize = { 10.f, 10.f };
    Vector2f fieldCenter = { 5.f, 5.f };
    uint16_t fieldFeather = 50;
    float noiseScale = 8.f;
    float noiseFrequency = 2.f;
    float noiseAmplitude = 4.f;
    auto noiseField = std::make_shared<ParticleNoiseField>(
        fieldStrength, fieldShape, fieldSize, fieldCenter, fieldFeather, noiseScale, noiseFrequency, noiseAmplitude);

    float distanceToEdge = 1.f;
    float featherWidth = 1.f;
    float distance = noiseField->CalculateFeatherEffect(distanceToEdge, featherWidth);
    EXPECT_TRUE(ROSEN_EQ(distance, 1.f));

    featherWidth = -1.f;
    distance = noiseField->CalculateFeatherEffect(distanceToEdge, featherWidth);
    EXPECT_TRUE(ROSEN_EQ(distance, 1.f));

    featherWidth = 0.f;
    distanceToEdge = -1.f;
    distance = noiseField->CalculateFeatherEffect(distanceToEdge, featherWidth);
    EXPECT_TRUE(ROSEN_EQ(distance, 1.f));

    Vector2f position = { 5.f, 5.f };
    float deltaTime = 10.f;
    Vector2f force = noiseField->ApplyField(position, deltaTime);
    EXPECT_TRUE(ROSEN_EQ(force.x_, 0.f) && ROSEN_EQ(force.y_, 0.f));

    deltaTime = 0.f;
    force = noiseField->ApplyField(position, deltaTime);
    EXPECT_TRUE(ROSEN_EQ(force.x_, 0.f) && ROSEN_EQ(force.y_, 0.f));
    GTEST_LOG_(INFO) << "RSRenderParticleEffectorTest CalculateFeatherEffect001 end";
}

/**
 * @tc.name: ApplyField001
 * @tc.desc: Verify the ApplyField
 * @tc.type:FUNC
 * @tc.require: issueIA6IWR
 */
HWTEST_F(RSRenderParticleEffectorTest, ApplyField001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderParticleEffectorTest ApplyField001 start";
    int fieldStrength = 0;
    ShapeType fieldShape = ShapeType::CIRCLE;
    Vector2f fieldSize = { 10.f, 10.f };
    Vector2f fieldCenter = { 5.f, 5.f };
    uint16_t fieldFeather = 50;
    float noiseScale = 8.f;
    float noiseFrequency = 2.f;
    float noiseAmplitude = 4.f;
    auto noiseField = std::make_shared<ParticleNoiseField>(
        fieldStrength, fieldShape, fieldSize, fieldCenter, fieldFeather, noiseScale, noiseFrequency, noiseAmplitude);

    Vector2f position = { 5.f, 5.f };
    float deltaTime = 10.f;
    Vector2f force = noiseField->ApplyField(position, deltaTime);
    EXPECT_TRUE(ROSEN_EQ(force.x_, 0.f) && ROSEN_EQ(force.y_, 0.f));

    position = { 15.f, 15.f };
    deltaTime = 0.f;
    force = noiseField->ApplyField(position, deltaTime);
    EXPECT_TRUE(ROSEN_EQ(force.x_, 0.f) && ROSEN_EQ(force.y_, 0.f));

    GTEST_LOG_(INFO) << "RSRenderParticleEffectorTest ApplyField001 end";
}

/**
 * @tc.name: UpdateOpacityGroupRandom001
 * @tc.desc: Verify UpdateOpacityGroup RANDOM covers CLAMP_AND_KILL_AT_REST KILL/SKIP/UPDATE
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderParticleEffectorTest, UpdateOpacityGroupRandom001, TestSize.Level1)
{
    auto op = RenderParticleParaType<float>(
        Range<float>(0.5f, 0.5f), ParticleUpdator::RANDOM, Range<float>(0.1f, 0.1f), {});
    auto renderParams = std::make_shared<ParticleRenderParams>(
        emitterConfig_, velocity_, acceleration_, color_, op, scale_, spin_);
    CreateParticle(renderParams);
    size_t s0 = slot;
    size_t s1 = pool->Acquire();
    size_t s2 = pool->Acquire();
    pool->SetOpacity(s0, 0.0f);
    pool->SetOpacitySpeed(s0, 0.0f);
    pool->SetOpacity(s1, 1.0f);
    pool->SetOpacitySpeed(s1, 1.0f);
    pool->SetOpacity(s2, 0.5f);
    pool->SetOpacitySpeed(s2, 0.2f);
    std::vector<size_t> slots{ s0, s1, s2 };
    effector->UpdateOpacityGroup(*pool, slots, *renderParams, deltaTime);
    EXPECT_FALSE(pool->IsAlive(s0));                              // KILL
    EXPECT_TRUE(ROSEN_EQ(pool->GetOpacity(s1), 1.0f));            // SKIP
    EXPECT_TRUE(ROSEN_EQ(pool->GetOpacity(s2), 0.5f + 0.2f * deltaTime)); // UPDATE + clamp
}

/**
 * @tc.name: UpdateScaleGroupRandom001
 * @tc.desc: Verify UpdateScaleGroup RANDOM covers KILL_AT_REST KILL/UPDATE (no clamp)
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderParticleEffectorTest, UpdateScaleGroupRandom001, TestSize.Level1)
{
    auto sc = RenderParticleParaType<float>(
        Range<float>(0.5f, 1.f), ParticleUpdator::RANDOM, Range<float>(0.1f, 0.1f), {});
    auto renderParams = std::make_shared<ParticleRenderParams>(
        emitterConfig_, velocity_, acceleration_, color_, opacity_, sc, spin_);
    CreateParticle(renderParams);
    size_t s0 = slot;
    size_t s1 = pool->Acquire();
    pool->SetScale(s0, 0.0f);
    pool->SetScaleSpeed(s0, 0.0f);
    pool->SetScale(s1, 0.5f);
    pool->SetScaleSpeed(s1, 0.2f);
    std::vector<size_t> slots{ s0, s1 };
    effector->UpdateScaleGroup(*pool, slots, *renderParams, deltaTime);
    EXPECT_FALSE(pool->IsAlive(s0));                             // KILL
    EXPECT_TRUE(ROSEN_EQ(pool->GetScale(s1), 0.5f + 0.2f * deltaTime)); // UPDATE (unbounded)
}

/**
 * @tc.name: UpdateSpinGroupRandom001
 * @tc.desc: Verify UpdateSpinGroup RANDOM covers ACCUMULATE advance and cos/sin recompute
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderParticleEffectorTest, UpdateSpinGroupRandom001, TestSize.Level1)
{
    auto sp = RenderParticleParaType<float>(
        Range<float>(1.f, 1.f), ParticleUpdator::RANDOM, Range<float>(1.f, 1.f), {});
    auto renderParams = std::make_shared<ParticleRenderParams>(
        emitterConfig_, velocity_, acceleration_, color_, opacity_, scale_, sp);
    CreateParticle(renderParams);
    size_t s0 = slot;
    pool->SetSpin(s0, 10.f);
    pool->SetSpinSpeed(s0, 5.f);
    std::vector<size_t> slots{ s0 };
    effector->UpdateSpinGroup(*pool, slots, *renderParams, deltaTime);
    EXPECT_TRUE(ROSEN_EQ(pool->GetSpin(s0), 10.f + 5.f * deltaTime)); // ACCUMULATE, no clamp
}

/**
 * @tc.name: UpdateAccelerationGroupRandom001
 * @tc.desc: Verify UpdateAccelerationGroup RANDOM covers ACCUMULATE for value and angle
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderParticleEffectorTest, UpdateAccelerationGroupRandom001, TestSize.Level1)
{
    auto accVal = RenderParticleParaType<float>(
        Range<float>(1.f, 1.f), ParticleUpdator::RANDOM, Range<float>(0.5f, 0.5f), {});
    auto accAngle = RenderParticleParaType<float>(
        Range<float>(1.f, 1.f), ParticleUpdator::RANDOM, Range<float>(0.5f, 0.5f), {});
    RenderParticleAcceleration acceleration(accVal, accAngle);
    auto renderParams = std::make_shared<ParticleRenderParams>(
        emitterConfig_, velocity_, acceleration, color_, opacity_, scale_, spin_);
    CreateParticle(renderParams);
    size_t s0 = slot;
    pool->SetAccelerationValue(s0, 2.f);
    pool->SetAccelerationAngle(s0, 3.f);
    std::vector<size_t> slots{ s0 };
    effector->UpdateAccelerationGroup(*pool, slots, *renderParams, deltaTime);
    EXPECT_TRUE(ROSEN_EQ(pool->GetAccelerationValue(s0), 2.f + 0.5f * deltaTime));
    EXPECT_TRUE(ROSEN_EQ(pool->GetAccelerationAngle(s0), 3.f + 0.5f * deltaTime));
}

/**
 * @tc.name: UpdatePositionGroupNullFields001
 * @tc.desc: Verify UpdatePositionGroup with null fields and accel-cache miss path
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderParticleEffectorTest, UpdatePositionGroupNullFields001, TestSize.Level1)
{
    std::shared_ptr<ParticleFieldCollection> fields = nullptr;
    std::vector<size_t> slots{ slot };
    pool->SetAccelerationValue(slot, 10.f);
    pool->SetAccelerationAngle(slot, 0.f);
    effector->UpdatePositionGroup(*pool, slots, fields, deltaTime);
    const Vector2f accel = pool->GetAcceleration(slot);
    EXPECT_TRUE(ROSEN_EQ(accel.x_, 10.f));
    EXPECT_TRUE(ROSEN_EQ(accel.y_, 0.f));
}

/**
 * @tc.name: UpdateToActiveTime001
 * @tc.desc: Verify UpdateToActiveTime advances activeTime with null fields
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderParticleEffectorTest, UpdateToActiveTime001, TestSize.Level1)
{
    pool->SetActiveTime(slot, activeTime);
    effector->UpdateToActiveTime(*pool, slot, *params, nullptr, activeTime);
    EXPECT_GT(pool->GetActiveTime(slot), 0);
}

/**
 * @tc.name: UpdateGroupCurve001
 * @tc.desc: Verify UpdateGroup dispatches the CURVE branches of all property groups with fields
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderParticleEffectorTest, UpdateGroupCurve001, TestSize.Level1)
{
    auto accVal = RenderParticleParaType<float>(
        Range<float>(1.f, 1.f), ParticleUpdator::CURVE, Range<float>(), {});
    auto accAngle = RenderParticleParaType<float>(
        Range<float>(1.f, 1.f), ParticleUpdator::CURVE, Range<float>(), {});
    RenderParticleAcceleration acceleration(accVal, accAngle);
    auto sc = RenderParticleParaType<float>(
        Range<float>(0.5f, 1.f), ParticleUpdator::CURVE, Range<float>(), {});
    auto sp = RenderParticleParaType<float>(
        Range<float>(0.5f, 1.f), ParticleUpdator::CURVE, Range<float>(), {});
    auto renderParams = std::make_shared<ParticleRenderParams>(
        emitterConfig_, velocity_, acceleration, color_, opacity_, sc, sp);
    CreateParticle(renderParams);
    auto noiseField = std::make_shared<ParticleNoiseField>(
        10, ShapeType::CIRCLE, Vector2f(10.f, 10.f), Vector2f(40.f, 50.f), 50, 8.f, 2.f, 4.f);
    auto fields = std::make_shared<ParticleFieldCollection>();
    fields->Add(noiseField);
    std::vector<size_t> slots{ slot };
    pool->SetActiveTime(slot, 0);
    const float opacityBefore = pool->GetOpacity(slot);
    effector->UpdateGroup(*pool, slots, *renderParams, fields, activeTime);
    EXPECT_TRUE(pool->GetOpacity(slot) != opacityBefore);
}
} // namespace Rosen
} // namespace OHOS