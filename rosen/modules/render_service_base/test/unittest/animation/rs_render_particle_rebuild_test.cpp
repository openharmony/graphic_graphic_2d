/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#include "animation/rs_render_particle.h"
#include "animation/rs_render_particle_animation.h"
#include "animation/rs_render_particle_effector.h"
#include "animation/rs_render_particle_system.h"
#include "common/rs_vector2.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
constexpr int64_t NS_PER_SEC = 1000000000LL; // matches NS_TO_S in the effector
constexpr uint64_t ANIMATION_ID = 12345;
constexpr uint64_t PROPERTY_ID = 54321;
constexpr int64_t PARTICLE_LIFETIME_MS = 10000; // 10s, long enough not to die in tests
} // namespace

/*
 * Tests for the particle "symmetric rebuild" feature: when an infinite particle
 * animation is rebuilt on foreground, each surviving particle must be advanced to
 * its true age in a single step. These cover the three behaviours that distinguish
 * UpdateToActiveTime from the live per-frame Update path:
 *   1. analytic position integration (v0*t + 0.5*a*t^2), not a single big Euler step;
 *   2. curve appearance evaluated at the real age, holding the end value once passed;
 *   3. the rebuild running-time (T) carry contract on RSRenderParticleAnimation.
 */
class RSRenderParticleRebuildTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override
    {
        effector_ = std::make_shared<RSRenderParticleEffector>();
    }
    void TearDown() override
    {
        effector_.reset();
    }

    // Build an emitter anchored at the origin with zero spread, so a particle's
    // initial position is deterministically (0, 0).
    EmitterConfig MakePointEmitter()
    {
        int emitRate = 20;
        ShapeType emitShape = ShapeType::RECT;
        Vector2f position = Vector2f(0.f, 0.f);
        Vector2f emitSize = Vector2f(0.f, 0.f);
        int particleCount = 20;
        Range<int64_t> lifeTime = Range<int64_t>(PARTICLE_LIFETIME_MS, PARTICLE_LIFETIME_MS);
        ParticleType type = ParticleType::POINTS;
        float radius = 1.f;
        std::shared_ptr<RSImage> image = nullptr;
        Vector2f imageSize = Vector2f(1.f, 1.f);
        return EmitterConfig(
            emitRate, emitShape, position, emitSize, particleCount, lifeTime, type, radius, image, imageSize);
    }

    // Constant velocity = (value, 0); constant acceleration = (value, 0). No randomness.
    std::shared_ptr<RSRenderParticle> MakeKinematicParticle(float velValue, float accValue)
    {
        ParticleVelocity velocity(Range<float>(velValue, velValue), Range<float>(0.f, 0.f));
        auto accVal = RenderParticleParaType<float>(
            Range<float>(accValue, accValue), ParticleUpdator::NONE, Range<float>(0.f, 0.f), {});
        auto accAngle = RenderParticleParaType<float>(
            Range<float>(0.f, 0.f), ParticleUpdator::NONE, Range<float>(0.f, 0.f), {});
        RenderParticleAcceleration acceleration(accVal, accAngle);
        RenderParticleColorParaType color;
        RenderParticleParaType<float> opacity;
        RenderParticleParaType<float> scale;
        RenderParticleParaType<float> spin;
        auto params = std::make_shared<ParticleRenderParams>(
            MakePointEmitter(), velocity, acceleration, color, opacity, scale, spin);
        return std::make_shared<RSRenderParticle>(params);
    }

    // Build render params for a point emitter with the given rate / count / lifetime,
    // no motion and no curve updators, so warmed-up particles simply age and stay alive.
    std::shared_ptr<ParticleRenderParams> MakeWarmupParams(int emitRate, int particleCount, int64_t lifeTimeMs)
    {
        EmitterConfig config(emitRate, ShapeType::RECT, Vector2f(0.f, 0.f), Vector2f(0.f, 0.f), particleCount,
            Range<int64_t>(lifeTimeMs, lifeTimeMs), ParticleType::POINTS, 1.f, nullptr, Vector2f(1.f, 1.f));
        ParticleVelocity velocity;
        RenderParticleAcceleration acceleration;
        RenderParticleColorParaType color;
        RenderParticleParaType<float> opacity;
        RenderParticleParaType<float> scale;
        RenderParticleParaType<float> spin;
        return std::make_shared<ParticleRenderParams>(config, velocity, acceleration, color, opacity, scale, spin);
    }

    std::shared_ptr<RSRenderParticleEffector> effector_;
};

/**
 * @tc.name: UpdateToActiveTimeAnalyticIntegration001
 * @tc.desc: UpdateToActiveTime advances position by v0*t + 0.5*a*t^2, not by a single big
 *           Euler step (which would double the acceleration term to a*t^2).
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleRebuildTest, UpdateToActiveTimeAnalyticIntegration001, TestSize.Level1)
{
    ASSERT_TRUE(effector_ != nullptr);
    const float v0 = 100.f;
    const float a = 200.f;
    auto particle = MakeKinematicParticle(v0, a);
    ASSERT_TRUE(particle != nullptr);

    const int64_t activeTimeNs = 2 * NS_PER_SEC; // t = 2.0s
    const float t = 2.0f;
    std::shared_ptr<ParticleFieldCollection> noFields = nullptr;
    effector_->UpdateToActiveTime(particle, noFields, activeTimeNs);

    // Analytic: x = v0*t + 0.5*a*t^2 = 200 + 400 = 600. Euler bug would give 1000.
    const float expectedX = v0 * t + 0.5f * a * t * t;
    EXPECT_NEAR(particle->GetPosition().x_, expectedX, 1.0f);
    // Velocity is integrated to v0 + a*t = 500.
    EXPECT_NEAR(particle->GetVelocity().x_, v0 + a * t, 1.0f);
    // activeTime is set to the real age (in ns) up front.
    EXPECT_EQ(particle->GetActiveTime(), activeTimeNs);
}

/**
 * @tc.name: UpdateToActiveTimeNullFields001
 * @tc.desc: UpdateToActiveTime tolerates a null field collection and still integrates motion.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleRebuildTest, UpdateToActiveTimeNullFields001, TestSize.Level1)
{
    ASSERT_TRUE(effector_ != nullptr);
    auto particle = MakeKinematicParticle(50.f, 0.f);
    std::shared_ptr<ParticleFieldCollection> noFields = nullptr;
    effector_->UpdateToActiveTime(particle, noFields, NS_PER_SEC);
    // Zero acceleration => pure drift: x = v0 * t = 50.
    EXPECT_NEAR(particle->GetPosition().x_, 50.f, 1.0f);

    std::shared_ptr<RSRenderParticle> nullParticle = nullptr;
    effector_->UpdateToActiveTime(nullParticle, noFields, NS_PER_SEC); // must not crash
}

/**
 * @tc.name: UpdateToActiveTimeHoldsCurveEndValue001
 * @tc.desc: When a particle is rebuilt to an age past its last opacity-curve segment,
 *           the appearance holds the segment's end value instead of being skipped
 *           (the "all red" / stuck-at-initial regression this PR fixes).
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleRebuildTest, UpdateToActiveTimeHoldsCurveEndValue001, TestSize.Level1)
{
    ASSERT_TRUE(effector_ != nullptr);
    const float fromValue = 0.2f;
    const float toValue = 0.9f;
    auto change = std::make_shared<ChangeInOverLife<float>>(
        fromValue, toValue, 0 /*startMillis*/, 1000 /*endMillis*/, nullptr);
    std::vector<std::shared_ptr<ChangeInOverLife<float>>> opacityChangeOverLife { change };
    auto opacity = RenderParticleParaType<float>(
        Range<float>(fromValue, fromValue), ParticleUpdator::CURVE, Range<float>(0.f, 0.f), opacityChangeOverLife);

    ParticleVelocity velocity;
    RenderParticleAcceleration acceleration;
    RenderParticleColorParaType color;
    RenderParticleParaType<float> scale;
    RenderParticleParaType<float> spin;
    auto params = std::make_shared<ParticleRenderParams>(
        MakePointEmitter(), velocity, acceleration, color, opacity, scale, spin);
    auto particle = std::make_shared<RSRenderParticle>(params);
    ASSERT_TRUE(particle != nullptr);

    // Age = 2000ms, well past the curve's 1000ms end => opacity holds toValue (0.9).
    std::shared_ptr<ParticleFieldCollection> noFields = nullptr;
    effector_->UpdateToActiveTime(particle, noFields, 2 * NS_PER_SEC);
    EXPECT_NEAR(particle->GetOpacity(), toValue, 0.01f);
}

/**
 * @tc.name: RebuildRunningTimeCarry001
 * @tc.desc: The rebuild running-time (T, in ns) round-trips through the animation's
 *           setter/getter so it can be carried render -> client -> render.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleRebuildTest, RebuildRunningTimeCarry001, TestSize.Level1)
{
    std::vector<std::shared_ptr<ParticleRenderParams>> renderParams;
    renderParams.push_back(std::make_shared<ParticleRenderParams>());
    auto animation = std::make_shared<RSRenderParticleAnimation>(ANIMATION_ID, PROPERTY_ID, renderParams);
    ASSERT_TRUE(animation != nullptr);

    constexpr int64_t carriedNs = 1234500000; // 1.2345s of carried run-time
    EXPECT_EQ(animation->GetRebuildRunningTimeNs(), 0);
    animation->SetRebuildRunningTimeNs(carriedNs);
    EXPECT_EQ(animation->GetRebuildRunningTimeNs(), carriedNs);

    animation->RebuildPropertyValue(0.5f); // contract: callable, must not crash
}

/**
 * @tc.name: UpdateToActiveTimeFieldSplitOverload001
 * @tc.desc: The (noise, ripple, velocity) overload bridges to the unified collection and
 *           integrates motion identically when every field group is null.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleRebuildTest, UpdateToActiveTimeFieldSplitOverload001, TestSize.Level1)
{
    ASSERT_TRUE(effector_ != nullptr);
    const float v0 = 100.f;
    const float a = 200.f;
    auto particle = MakeKinematicParticle(v0, a);
    ASSERT_TRUE(particle != nullptr);

    std::shared_ptr<ParticleNoiseFields> noNoise = nullptr;
    std::shared_ptr<ParticleRippleFields> noRipple = nullptr;
    std::shared_ptr<ParticleVelocityFields> noVelocity = nullptr;
    const int64_t activeTimeNs = 2 * NS_PER_SEC; // t = 2.0s
    const float t = 2.0f;
    effector_->UpdateToActiveTime(particle, noNoise, noRipple, noVelocity, activeTimeNs);

    // Same analytic result as the collection overload: x = v0*t + 0.5*a*t^2 = 600.
    EXPECT_NEAR(particle->GetPosition().x_, v0 * t + 0.5f * a * t * t, 1.0f);
    EXPECT_EQ(particle->GetActiveTime(), activeTimeNs);
}

/**
 * @tc.name: UpdateToActiveTimeBeforeCurveStart001
 * @tc.desc: When the rebuilt age is earlier than a curve segment's start, the appearance is
 *           left at its initial value (the "not yet started" branch, complementing the
 *           past-the-end hold case).
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleRebuildTest, UpdateToActiveTimeBeforeCurveStart001, TestSize.Level1)
{
    ASSERT_TRUE(effector_ != nullptr);
    const float fromValue = 0.2f;
    const float toValue = 0.9f;
    // Curve only acts between 1000ms and 2000ms.
    auto change = std::make_shared<ChangeInOverLife<float>>(
        fromValue, toValue, 1000 /*startMillis*/, 2000 /*endMillis*/, nullptr);
    std::vector<std::shared_ptr<ChangeInOverLife<float>>> opacityChangeOverLife { change };
    auto opacity = RenderParticleParaType<float>(
        Range<float>(fromValue, fromValue), ParticleUpdator::CURVE, Range<float>(0.f, 0.f), opacityChangeOverLife);

    ParticleVelocity velocity;
    RenderParticleAcceleration acceleration;
    RenderParticleColorParaType color;
    RenderParticleParaType<float> scale;
    RenderParticleParaType<float> spin;
    auto params = std::make_shared<ParticleRenderParams>(
        MakePointEmitter(), velocity, acceleration, color, opacity, scale, spin);
    auto particle = std::make_shared<RSRenderParticle>(params);
    ASSERT_TRUE(particle != nullptr);

    // Age = 500ms, before the curve's 1000ms start => opacity holds its initial fromValue.
    std::shared_ptr<ParticleFieldCollection> noFields = nullptr;
    effector_->UpdateToActiveTime(particle, noFields, NS_PER_SEC / 2);
    EXPECT_NEAR(particle->GetOpacity(), fromValue, 0.01f);
}

/**
 * @tc.name: WarmupNonPositiveTime001
 * @tc.desc: Warmup with a zero or negative total time is a no-op (produces no particles).
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleRebuildTest, WarmupNonPositiveTime001, TestSize.Level1)
{
    std::vector<std::shared_ptr<ParticleRenderParams>> params { MakeWarmupParams(20, 20, PARTICLE_LIFETIME_MS) };
    auto system = std::make_shared<RSRenderParticleSystem>(params);
    ASSERT_TRUE(system != nullptr);

    std::vector<std::shared_ptr<RSRenderParticle>> active;
    std::vector<std::shared_ptr<RSImage>> images;
    system->Warmup(0, active, images);
    EXPECT_TRUE(active.empty());
    system->Warmup(-1 * NS_PER_SEC, active, images);
    EXPECT_TRUE(active.empty());
}

/**
 * @tc.name: WarmupFillsAgedParticles001
 * @tc.desc: Warmup fills a batch of already-aged live particles in one pass; the count is
 *           bounded by the emitter's particleCount and the oldest particle carries a positive age.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleRebuildTest, WarmupFillsAgedParticles001, TestSize.Level1)
{
    const int particleCount = 20;
    std::vector<std::shared_ptr<ParticleRenderParams>> params {
        MakeWarmupParams(20, particleCount, PARTICLE_LIFETIME_MS) };
    auto system = std::make_shared<RSRenderParticleSystem>(params);
    ASSERT_TRUE(system != nullptr);

    std::vector<std::shared_ptr<RSRenderParticle>> active;
    std::vector<std::shared_ptr<RSImage>> images;
    system->Warmup(NS_PER_SEC, active, images); // 1s of carried progress

    ASSERT_GE(active.size(), 2u);
    EXPECT_LE(active.size(), static_cast<size_t>(particleCount));
    // Particles are pushed oldest-last; the final one was emitted earliest, so it has aged.
    EXPECT_GT(active.back()->GetActiveTime(), 0);
}

/**
 * @tc.name: WarmupRespectsParticleCountCap001
 * @tc.desc: A finite emitter never warms up more particles than its particleCount, even when
 *           rate * time would otherwise far exceed it.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleRebuildTest, WarmupRespectsParticleCountCap001, TestSize.Level1)
{
    const int particleCount = 5;
    // rate * time = 1000 * 2 = 2000, but particleCount caps it at 5.
    std::vector<std::shared_ptr<ParticleRenderParams>> params {
        MakeWarmupParams(1000, particleCount, PARTICLE_LIFETIME_MS) };
    auto system = std::make_shared<RSRenderParticleSystem>(params);
    ASSERT_TRUE(system != nullptr);

    std::vector<std::shared_ptr<RSRenderParticle>> active;
    std::vector<std::shared_ptr<RSImage>> images;
    system->Warmup(2 * NS_PER_SEC, active, images);

    EXPECT_GT(active.size(), 0u);
    EXPECT_LE(active.size(), static_cast<size_t>(particleCount));
}

/**
 * @tc.name: HasAnyFieldFalseWhenNoFields001
 * @tc.desc: A freshly constructed system with no force fields reports HasAnyField() == false.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleRebuildTest, HasAnyFieldFalseWhenNoFields001, TestSize.Level1)
{
    std::vector<std::shared_ptr<ParticleRenderParams>> params { MakeWarmupParams(20, 20, PARTICLE_LIFETIME_MS) };
    auto system = std::make_shared<RSRenderParticleSystem>(params);
    ASSERT_TRUE(system != nullptr);
    EXPECT_FALSE(system->HasAnyField());
}

} // namespace Rosen
} // namespace OHOS
