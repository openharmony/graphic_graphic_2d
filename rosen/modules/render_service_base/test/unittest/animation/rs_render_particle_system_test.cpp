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

#include "gtest/gtest.h"

#include "animation/rs_particle_field_base.h"
#include "animation/rs_particle_field_collection.h"
#include "animation/rs_particle_noise_field.h"
#include "animation/rs_particle_ripple_field.h"
#include "animation/rs_particle_velocity_field.h"
#include "animation/rs_render_particle.h"
#include "animation/rs_render_particle_system.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

namespace {
constexpr uint16_t DEFAULT_NOISE_FEATHER = 5;
constexpr float DEFAULT_NOISE_FREQUENCY = 1.0f;
constexpr float DEFAULT_NOISE_AMPLITUDE = 1.0f;

// Helper: create a NoiseField with default region covering a large rectangle.
std::shared_ptr<ParticleNoiseField> MakeNoiseField(int strength = 10, float scale = 1.0f)
{
    return std::make_shared<ParticleNoiseField>(
        strength, ShapeType::RECT, Vector2f(1000.0f, 1000.0f), Vector2f(0.0f, 0.0f),
        DEFAULT_NOISE_FEATHER, scale, DEFAULT_NOISE_FREQUENCY, DEFAULT_NOISE_AMPLITUDE);
}

// Helper: create a RippleField with specified parameters.
std::shared_ptr<ParticleRippleField> MakeRippleField(
    const Vector2f& center = Vector2f(50.0f, 50.0f),
    float amplitude = 100.0f, float wavelength = 50.0f, float waveSpeed = 200.0f,
    float attenuation = 0.0f)
{
    auto ripple = std::make_shared<ParticleRippleField>(center, amplitude, wavelength, waveSpeed, attenuation);
    ripple->regionShape_ = ShapeType::RECT;
    ripple->regionPosition_ = Vector2f(0.0f, 0.0f);
    ripple->regionSize_ = Vector2f(1000.0f, 1000.0f);
    return ripple;
}

// Helper: create a VelocityField.
std::shared_ptr<ParticleVelocityField> MakeVelocityField(const Vector2f& vel = Vector2f(10.0f, 20.0f))
{
    auto vf = std::make_shared<ParticleVelocityField>(vel);
    vf->regionShape_ = ShapeType::RECT;
    vf->regionPosition_ = Vector2f(0.0f, 0.0f);
    vf->regionSize_ = Vector2f(1000.0f, 1000.0f);
    return vf;
}

// Helper: count how many fields of given type exist in the collection.
size_t CountByType(const std::shared_ptr<ParticleFieldCollection>& col, ParticleFieldType type)
{
    if (!col) {
        return 0;
    }
    size_t n = 0;
    for (const auto& f : col->fields_) {
        if (f != nullptr && f->GetType() == type) {
            ++n;
        }
    }
    return n;
}
} // namespace

class RSRenderParticleSystemTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override
    {
        system_ = std::make_shared<RSRenderParticleSystem>();
    }
    void TearDown() override
    {
        system_.reset();
    }

    std::shared_ptr<RSRenderParticleSystem> system_;
};

/* ==========================================================================
 * UpdateFields — self-assignment branch
 * ========================================================================== */

/**
 * @tc.name: UpdateFieldsSelfAssignment001
 * @tc.desc: If the same Collection pointer is passed again, the function
 *           returns early without mutating the collection.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleSystemTest, UpdateFieldsSelfAssignment001, TestSize.Level1)
{
    auto col = std::make_shared<ParticleFieldCollection>();
    col->Add(MakeNoiseField());
    col->Add(MakeRippleField());
    system_->UpdateFields(col);
    size_t before = col->Count();

    // Pass the SAME pointer again — should short-circuit the self-assignment guard.
    system_->UpdateFields(col);
    EXPECT_EQ(col->Count(), before);
}

/* ==========================================================================
 * UpdateFields — per-type incremental merge (core logic)
 * ========================================================================== */

/**
 * @tc.name: UpdateFieldsNoiseOnly001
 * @tc.desc: Sending only NOISE replaces existing NOISE and preserves
 *           RIPPLE and VELOCITY entries.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleSystemTest, UpdateFieldsNoiseOnly001, TestSize.Level1)
{
    // Initial: 1 NOISE + 1 RIPPLE + 1 VELOCITY
    auto initial = std::make_shared<ParticleFieldCollection>();
    initial->Add(MakeNoiseField(10));
    initial->Add(MakeRippleField());
    initial->Add(MakeVelocityField());
    system_->UpdateFields(initial);

    // Second call: only 2 NOISE fields
    auto noiseOnly = std::make_shared<ParticleFieldCollection>();
    noiseOnly->Add(MakeNoiseField(20));
    noiseOnly->Add(MakeNoiseField(30));
    system_->UpdateFields(noiseOnly);

    // Expectation: particleFields_ now has 2 NOISE (replaced) + 1 RIPPLE + 1 VELOCITY (kept) = 4
    // We access the system's merged collection via the shared ptr `initial` which was the
    // first-time-assigned collection. initial is NOT the snapshot — the guard uses get() equality,
    // so mutations happened on `initial`.
    EXPECT_EQ(CountByType(initial, ParticleFieldType::NOISE), 2u);
    EXPECT_EQ(CountByType(initial, ParticleFieldType::RIPPLE), 1u);
    EXPECT_EQ(CountByType(initial, ParticleFieldType::VELOCITY), 1u);
    EXPECT_EQ(initial->Count(), 4u);
}

/**
 * @tc.name: UpdateFieldsRippleOnly001
 * @tc.desc: Sending only RIPPLE replaces RIPPLE and preserves others.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleSystemTest, UpdateFieldsRippleOnly001, TestSize.Level1)
{
    auto initial = std::make_shared<ParticleFieldCollection>();
    initial->Add(MakeNoiseField());
    initial->Add(MakeRippleField());
    initial->Add(MakeVelocityField());
    system_->UpdateFields(initial);

    auto rippleOnly = std::make_shared<ParticleFieldCollection>();
    rippleOnly->Add(MakeRippleField(Vector2f(100.0f, 100.0f), 200.0f));
    system_->UpdateFields(rippleOnly);

    EXPECT_EQ(CountByType(initial, ParticleFieldType::NOISE), 1u);
    EXPECT_EQ(CountByType(initial, ParticleFieldType::RIPPLE), 1u);
    EXPECT_EQ(CountByType(initial, ParticleFieldType::VELOCITY), 1u);
    EXPECT_EQ(initial->Count(), 3u);
}

/**
 * @tc.name: UpdateFieldsVelocityOnly001
 * @tc.desc: Sending only VELOCITY replaces VELOCITY and preserves others.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleSystemTest, UpdateFieldsVelocityOnly001, TestSize.Level1)
{
    auto initial = std::make_shared<ParticleFieldCollection>();
    initial->Add(MakeNoiseField());
    initial->Add(MakeRippleField());
    initial->Add(MakeVelocityField(Vector2f(5.0f, 5.0f)));
    system_->UpdateFields(initial);

    auto velocityOnly = std::make_shared<ParticleFieldCollection>();
    velocityOnly->Add(MakeVelocityField(Vector2f(50.0f, 50.0f)));
    velocityOnly->Add(MakeVelocityField(Vector2f(60.0f, 60.0f)));
    system_->UpdateFields(velocityOnly);

    EXPECT_EQ(CountByType(initial, ParticleFieldType::NOISE), 1u);
    EXPECT_EQ(CountByType(initial, ParticleFieldType::RIPPLE), 1u);
    EXPECT_EQ(CountByType(initial, ParticleFieldType::VELOCITY), 2u);
    EXPECT_EQ(initial->Count(), 4u);
}

/**
 * @tc.name: UpdateFieldsAllTypes001
 * @tc.desc: A collection containing all three types replaces every type.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleSystemTest, UpdateFieldsAllTypes001, TestSize.Level1)
{
    auto initial = std::make_shared<ParticleFieldCollection>();
    initial->Add(MakeNoiseField());
    initial->Add(MakeRippleField());
    initial->Add(MakeVelocityField());
    system_->UpdateFields(initial);

    auto full = std::make_shared<ParticleFieldCollection>();
    full->Add(MakeNoiseField(99));
    full->Add(MakeRippleField(Vector2f(99.0f, 99.0f), 99.0f));
    full->Add(MakeVelocityField(Vector2f(99.0f, 99.0f)));
    system_->UpdateFields(full);

    EXPECT_EQ(CountByType(initial, ParticleFieldType::NOISE), 1u);
    EXPECT_EQ(CountByType(initial, ParticleFieldType::RIPPLE), 1u);
    EXPECT_EQ(CountByType(initial, ParticleFieldType::VELOCITY), 1u);
    EXPECT_EQ(initial->Count(), 3u);
}

/**
 * @tc.name: UpdateFieldsWithNullptrInside001
 * @tc.desc: A collection containing nullptr entries is processed gracefully.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleSystemTest, UpdateFieldsWithNullptrInside001, TestSize.Level1)
{
    auto initial = std::make_shared<ParticleFieldCollection>();
    initial->Add(MakeNoiseField());
    system_->UpdateFields(initial);

    // Hand-craft a collection with a null entry (bypassing Collection::Add which skips nulls).
    auto withNull = std::make_shared<ParticleFieldCollection>();
    withNull->fields_.push_back(nullptr);
    withNull->fields_.push_back(MakeNoiseField(77));
    system_->UpdateFields(withNull);

    EXPECT_EQ(CountByType(initial, ParticleFieldType::NOISE), 1u);
}

/* ==========================================================================
 * UpdateFields — ripple lifeTime preservation branches
 * ========================================================================== */

/**
 * @tc.name: UpdateFieldsRippleLifeTimeKeep001
 * @tc.desc: When new ripple has identical params to old, lifeTime_ is inherited.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleSystemTest, UpdateFieldsRippleLifeTimeKeep001, TestSize.Level1)
{
    auto initial = std::make_shared<ParticleFieldCollection>();
    auto oldRipple = MakeRippleField(Vector2f(50.0f, 50.0f), 100.0f, 50.0f, 200.0f, 0.1f);
    oldRipple->lifeTime_ = 5.0f;
    initial->Add(oldRipple);
    system_->UpdateFields(initial);

    // New ripple with IDENTICAL params (fresh lifeTime_ = 0).
    auto newCol = std::make_shared<ParticleFieldCollection>();
    auto newRipple = MakeRippleField(Vector2f(50.0f, 50.0f), 100.0f, 50.0f, 200.0f, 0.1f);
    EXPECT_FLOAT_EQ(newRipple->lifeTime_, 0.0f);
    newCol->Add(newRipple);
    system_->UpdateFields(newCol);

    // The newRipple should have inherited lifeTime_ == 5.0f from oldRipple.
    EXPECT_FLOAT_EQ(newRipple->lifeTime_, 5.0f);
}

/**
 * @tc.name: UpdateFieldsRippleLifeTimeResetOnCenterChange001
 * @tc.desc: Center change → do not inherit lifeTime_.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleSystemTest, UpdateFieldsRippleLifeTimeResetOnCenterChange001, TestSize.Level1)
{
    auto initial = std::make_shared<ParticleFieldCollection>();
    auto oldRipple = MakeRippleField(Vector2f(50.0f, 50.0f), 100.0f);
    oldRipple->lifeTime_ = 5.0f;
    initial->Add(oldRipple);
    system_->UpdateFields(initial);

    auto newCol = std::make_shared<ParticleFieldCollection>();
    auto newRipple = MakeRippleField(Vector2f(99.0f, 99.0f), 100.0f);  // center changed
    newCol->Add(newRipple);
    system_->UpdateFields(newCol);

    EXPECT_FLOAT_EQ(newRipple->lifeTime_, 0.0f);
}

/**
 * @tc.name: UpdateFieldsRippleLifeTimeResetOnAmplitudeChange001
 * @tc.desc: Amplitude change → do not inherit lifeTime_.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleSystemTest, UpdateFieldsRippleLifeTimeResetOnAmplitudeChange001, TestSize.Level1)
{
    auto initial = std::make_shared<ParticleFieldCollection>();
    auto oldRipple = MakeRippleField(Vector2f(50.0f, 50.0f), 100.0f);
    oldRipple->lifeTime_ = 5.0f;
    initial->Add(oldRipple);
    system_->UpdateFields(initial);

    auto newCol = std::make_shared<ParticleFieldCollection>();
    auto newRipple = MakeRippleField(Vector2f(50.0f, 50.0f), 200.0f);  // amplitude changed
    newCol->Add(newRipple);
    system_->UpdateFields(newCol);

    EXPECT_FLOAT_EQ(newRipple->lifeTime_, 0.0f);
}

/**
 * @tc.name: UpdateFieldsRippleLifeTimeResetOnWavelengthChange001
 * @tc.desc: Wavelength change → do not inherit lifeTime_.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleSystemTest, UpdateFieldsRippleLifeTimeResetOnWavelengthChange001, TestSize.Level1)
{
    auto initial = std::make_shared<ParticleFieldCollection>();
    auto oldRipple = MakeRippleField(Vector2f(50.0f, 50.0f), 100.0f, 50.0f);
    oldRipple->lifeTime_ = 5.0f;
    initial->Add(oldRipple);
    system_->UpdateFields(initial);

    auto newCol = std::make_shared<ParticleFieldCollection>();
    auto newRipple = MakeRippleField(Vector2f(50.0f, 50.0f), 100.0f, 99.0f);  // wavelength changed
    newCol->Add(newRipple);
    system_->UpdateFields(newCol);

    EXPECT_FLOAT_EQ(newRipple->lifeTime_, 0.0f);
}

/**
 * @tc.name: UpdateFieldsRippleLifeTimeResetOnWaveSpeedChange001
 * @tc.desc: WaveSpeed change → do not inherit lifeTime_.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleSystemTest, UpdateFieldsRippleLifeTimeResetOnWaveSpeedChange001, TestSize.Level1)
{
    auto initial = std::make_shared<ParticleFieldCollection>();
    auto oldRipple = MakeRippleField(Vector2f(50.0f, 50.0f), 100.0f, 50.0f, 200.0f);
    oldRipple->lifeTime_ = 5.0f;
    initial->Add(oldRipple);
    system_->UpdateFields(initial);

    auto newCol = std::make_shared<ParticleFieldCollection>();
    auto newRipple = MakeRippleField(Vector2f(50.0f, 50.0f), 100.0f, 50.0f, 999.0f);  // waveSpeed changed
    newCol->Add(newRipple);
    system_->UpdateFields(newCol);

    EXPECT_FLOAT_EQ(newRipple->lifeTime_, 0.0f);
}

/**
 * @tc.name: UpdateFieldsRippleLifeTimeResetOnAttenuationChange001
 * @tc.desc: Attenuation change → do not inherit lifeTime_.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleSystemTest, UpdateFieldsRippleLifeTimeResetOnAttenuationChange001, TestSize.Level1)
{
    auto initial = std::make_shared<ParticleFieldCollection>();
    auto oldRipple = MakeRippleField(Vector2f(50.0f, 50.0f), 100.0f, 50.0f, 200.0f, 0.1f);
    oldRipple->lifeTime_ = 5.0f;
    initial->Add(oldRipple);
    system_->UpdateFields(initial);

    auto newCol = std::make_shared<ParticleFieldCollection>();
    auto newRipple = MakeRippleField(Vector2f(50.0f, 50.0f), 100.0f, 50.0f, 200.0f, 0.9f);  // attenuation changed
    newCol->Add(newRipple);
    system_->UpdateFields(newCol);

    EXPECT_FLOAT_EQ(newRipple->lifeTime_, 0.0f);
}

/**
 * @tc.name: UpdateFieldsRippleLifeTimeResetOnRegionShapeChange001
 * @tc.desc: Region shape change → do not inherit lifeTime_.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleSystemTest, UpdateFieldsRippleLifeTimeResetOnRegionShapeChange001, TestSize.Level1)
{
    auto initial = std::make_shared<ParticleFieldCollection>();
    auto oldRipple = MakeRippleField();
    oldRipple->lifeTime_ = 5.0f;
    initial->Add(oldRipple);
    system_->UpdateFields(initial);

    auto newCol = std::make_shared<ParticleFieldCollection>();
    auto newRipple = MakeRippleField();
    newRipple->regionShape_ = ShapeType::CIRCLE;  // shape changed
    newCol->Add(newRipple);
    system_->UpdateFields(newCol);

    EXPECT_FLOAT_EQ(newRipple->lifeTime_, 0.0f);
}

/**
 * @tc.name: UpdateFieldsRippleLifeTimeResetOnRegionPositionChange001
 * @tc.desc: Region position change → do not inherit lifeTime_.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleSystemTest, UpdateFieldsRippleLifeTimeResetOnRegionPositionChange001, TestSize.Level1)
{
    auto initial = std::make_shared<ParticleFieldCollection>();
    auto oldRipple = MakeRippleField();
    oldRipple->lifeTime_ = 5.0f;
    initial->Add(oldRipple);
    system_->UpdateFields(initial);

    auto newCol = std::make_shared<ParticleFieldCollection>();
    auto newRipple = MakeRippleField();
    newRipple->regionPosition_ = Vector2f(100.0f, 100.0f);  // position changed
    newCol->Add(newRipple);
    system_->UpdateFields(newCol);

    EXPECT_FLOAT_EQ(newRipple->lifeTime_, 0.0f);
}

/**
 * @tc.name: UpdateFieldsRippleLifeTimeResetOnRegionSizeChange001
 * @tc.desc: Region size change → do not inherit lifeTime_.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleSystemTest, UpdateFieldsRippleLifeTimeResetOnRegionSizeChange001, TestSize.Level1)
{
    auto initial = std::make_shared<ParticleFieldCollection>();
    auto oldRipple = MakeRippleField();
    oldRipple->lifeTime_ = 5.0f;
    initial->Add(oldRipple);
    system_->UpdateFields(initial);

    auto newCol = std::make_shared<ParticleFieldCollection>();
    auto newRipple = MakeRippleField();
    newRipple->regionSize_ = Vector2f(2000.0f, 2000.0f);  // size changed
    newCol->Add(newRipple);
    system_->UpdateFields(newCol);

    EXPECT_FLOAT_EQ(newRipple->lifeTime_, 0.0f);
}

/**
 * @tc.name: UpdateFieldsRippleCountNewMoreThanOld001
 * @tc.desc: New has more ripples than old — extras get default lifeTime.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleSystemTest, UpdateFieldsRippleCountNewMoreThanOld001, TestSize.Level1)
{
    auto initial = std::make_shared<ParticleFieldCollection>();
    auto r1 = MakeRippleField(Vector2f(10.0f, 10.0f));
    r1->lifeTime_ = 3.0f;
    initial->Add(r1);
    auto r2 = MakeRippleField(Vector2f(20.0f, 20.0f));
    r2->lifeTime_ = 4.0f;
    initial->Add(r2);
    system_->UpdateFields(initial);

    auto newCol = std::make_shared<ParticleFieldCollection>();
    auto n1 = MakeRippleField(Vector2f(10.0f, 10.0f));  // matches r1
    auto n2 = MakeRippleField(Vector2f(20.0f, 20.0f));  // matches r2
    auto n3 = MakeRippleField(Vector2f(30.0f, 30.0f));  // extra, no match
    newCol->Add(n1);
    newCol->Add(n2);
    newCol->Add(n3);
    system_->UpdateFields(newCol);

    EXPECT_FLOAT_EQ(n1->lifeTime_, 3.0f);
    EXPECT_FLOAT_EQ(n2->lifeTime_, 4.0f);
    EXPECT_FLOAT_EQ(n3->lifeTime_, 0.0f);   // out of range — no preservation
}

/**
 * @tc.name: UpdateFieldsRippleCountNewLessThanOld001
 * @tc.desc: New has fewer ripples than old — only first N are preserved.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleSystemTest, UpdateFieldsRippleCountNewLessThanOld001, TestSize.Level1)
{
    auto initial = std::make_shared<ParticleFieldCollection>();
    auto r1 = MakeRippleField(Vector2f(10.0f, 10.0f));
    r1->lifeTime_ = 3.0f;
    initial->Add(r1);
    auto r2 = MakeRippleField(Vector2f(20.0f, 20.0f));
    r2->lifeTime_ = 4.0f;
    initial->Add(r2);
    auto r3 = MakeRippleField(Vector2f(30.0f, 30.0f));
    r3->lifeTime_ = 5.0f;
    initial->Add(r3);
    system_->UpdateFields(initial);

    auto newCol = std::make_shared<ParticleFieldCollection>();
    auto n1 = MakeRippleField(Vector2f(10.0f, 10.0f));  // matches r1
    newCol->Add(n1);
    system_->UpdateFields(newCol);

    EXPECT_FLOAT_EQ(n1->lifeTime_, 3.0f);
}

/* ==========================================================================
 * Legacy setter paths — UpdateNoiseField / UpdateRippleField / UpdateVelocityField
 * ========================================================================== */

/**
 * @tc.name: UpdateNoiseField001
 * @tc.desc: Legacy UpdateNoiseField assigns without crashing.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleSystemTest, UpdateNoiseField001, TestSize.Level1)
{
    auto noiseFields = std::make_shared<ParticleNoiseFields>();
    noiseFields->AddField(MakeNoiseField());
    system_->UpdateNoiseField(noiseFields);
    ASSERT_NE(system_->GetParticleNoiseFields(), nullptr);
    EXPECT_EQ(system_->GetParticleNoiseFields()->GetFieldCount(), 1u);

    // Overwrite with nullptr.
    system_->UpdateNoiseField(nullptr);
    EXPECT_EQ(system_->GetParticleNoiseFields(), nullptr);
}

/**
 * @tc.name: UpdateRippleFieldNonIncremental001
 * @tc.desc: UpdateRippleField with isIncrementalUpdate=false replaces directly.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleSystemTest, UpdateRippleFieldNonIncremental001, TestSize.Level1)
{
    auto first = std::make_shared<ParticleRippleFields>();
    first->AddRippleField(MakeRippleField());
    system_->UpdateRippleField(first, false);
    EXPECT_EQ(system_->GetParticleRippleFields()->GetRippleFieldCount(), 1u);

    auto second = std::make_shared<ParticleRippleFields>();
    second->AddRippleField(MakeRippleField());
    second->AddRippleField(MakeRippleField());
    system_->UpdateRippleField(second, false);
    EXPECT_EQ(system_->GetParticleRippleFields()->GetRippleFieldCount(), 2u);
}

/**
 * @tc.name: UpdateRippleFieldIncrementalNullNew001
 * @tc.desc: Incremental update with nullptr new collection replaces directly.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleSystemTest, UpdateRippleFieldIncrementalNullNew001, TestSize.Level1)
{
    auto first = std::make_shared<ParticleRippleFields>();
    first->AddRippleField(MakeRippleField());
    system_->UpdateRippleField(first, true);

    system_->UpdateRippleField(nullptr, true);
    EXPECT_EQ(system_->GetParticleRippleFields(), nullptr);
}

/**
 * @tc.name: UpdateRippleFieldIncrementalNullExisting001
 * @tc.desc: Incremental update when existing is null — direct assignment.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleSystemTest, UpdateRippleFieldIncrementalNullExisting001, TestSize.Level1)
{
    auto first = std::make_shared<ParticleRippleFields>();
    first->AddRippleField(MakeRippleField());
    system_->UpdateRippleField(first, true);
    EXPECT_NE(system_->GetParticleRippleFields(), nullptr);
}

/**
 * @tc.name: UpdateRippleFieldIncrementalLifeTimeKeep001
 * @tc.desc: Incremental update with identical params preserves lifeTime_.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleSystemTest, UpdateRippleFieldIncrementalLifeTimeKeep001, TestSize.Level1)
{
    auto existing = std::make_shared<ParticleRippleFields>();
    auto oldRipple = MakeRippleField();
    oldRipple->lifeTime_ = 7.0f;
    existing->AddRippleField(oldRipple);
    system_->UpdateRippleField(existing, true);

    auto incoming = std::make_shared<ParticleRippleFields>();
    auto newRipple = MakeRippleField();   // identical
    incoming->AddRippleField(newRipple);
    system_->UpdateRippleField(incoming, true);

    EXPECT_FLOAT_EQ(newRipple->lifeTime_, 7.0f);
}

/**
 * @tc.name: UpdateRippleFieldIncrementalLifeTimeMismatch001
 * @tc.desc: Incremental update with different params does not inherit lifeTime_.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleSystemTest, UpdateRippleFieldIncrementalLifeTimeMismatch001, TestSize.Level1)
{
    auto existing = std::make_shared<ParticleRippleFields>();
    auto oldRipple = MakeRippleField(Vector2f(50.0f, 50.0f));
    oldRipple->lifeTime_ = 7.0f;
    existing->AddRippleField(oldRipple);
    system_->UpdateRippleField(existing, true);

    auto incoming = std::make_shared<ParticleRippleFields>();
    auto newRipple = MakeRippleField(Vector2f(99.0f, 99.0f));   // different center
    incoming->AddRippleField(newRipple);
    system_->UpdateRippleField(incoming, true);

    EXPECT_FLOAT_EQ(newRipple->lifeTime_, 0.0f);
}

/**
 * @tc.name: UpdateVelocityField001
 * @tc.desc: Legacy UpdateVelocityField assigns without crashing.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleSystemTest, UpdateVelocityField001, TestSize.Level1)
{
    auto vf = std::make_shared<ParticleVelocityFields>();
    vf->AddVelocityField(MakeVelocityField());
    system_->UpdateVelocityField(vf);
    ASSERT_NE(system_->GetParticleVelocityFields(), nullptr);
    EXPECT_EQ(system_->GetParticleVelocityFields()->GetVelocityFieldCount(), 1u);

    system_->UpdateVelocityField(nullptr);
    EXPECT_EQ(system_->GetParticleVelocityFields(), nullptr);
}

/* ==========================================================================
 * IsFinish — empty vs active particles
 * ========================================================================== */

/**
 * @tc.name: IsFinishEmpty001
 * @tc.desc: IsFinish returns true when there are no emitters nor particles.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleSystemTest, IsFinishEmpty001, TestSize.Level1)
{
    std::vector<std::shared_ptr<RSRenderParticle>> particles;
    EXPECT_TRUE(system_->IsFinish(particles));
}

/**
 * @tc.name: IsFinishNonEmpty001
 * @tc.desc: IsFinish returns false when there are active particles.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleSystemTest, IsFinishNonEmpty001, TestSize.Level1)
{
    std::vector<std::shared_ptr<RSRenderParticle>> particles(1, nullptr);
    EXPECT_FALSE(system_->IsFinish(particles));
}

/* ==========================================================================
 * UpdateParticle branch selection (new vs legacy path)
 * ========================================================================== */

/**
 * @tc.name: UpdateParticleEmpty001
 * @tc.desc: UpdateParticle with empty active particles is a no-op, does not crash.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleSystemTest, UpdateParticleEmpty001, TestSize.Level1)
{
    std::vector<std::shared_ptr<RSRenderParticle>> particles;
    system_->UpdateParticle(16000000, particles);
    EXPECT_TRUE(particles.empty());
}

/**
 * @tc.name: UpdateParticleLegacyPath001
 * @tc.desc: When particleFields_ is null, UpdateParticle runs legacy path with nullptr particles.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleSystemTest, UpdateParticleLegacyPath001, TestSize.Level1)
{
    // particleFields_ is null (no UpdateFields called). A dead particle gets erased.
    std::vector<std::shared_ptr<RSRenderParticle>> particles;
    particles.push_back(nullptr);  // null particle is "not alive" and will be erased
    system_->UpdateParticle(16000000, particles);
    EXPECT_TRUE(particles.empty());
}

/**
 * @tc.name: UpdateParticleLegacyRippleFieldsSet001
 * @tc.desc: Legacy path with ripple fields set — UpdateAllRipples gets called.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleSystemTest, UpdateParticleLegacyRippleFieldsSet001, TestSize.Level1)
{
    auto ripples = std::make_shared<ParticleRippleFields>();
    ripples->AddRippleField(MakeRippleField());
    system_->UpdateRippleField(ripples, false);

    std::vector<std::shared_ptr<RSRenderParticle>> particles;
    particles.push_back(nullptr);
    system_->UpdateParticle(16000000, particles);
    EXPECT_TRUE(particles.empty());
}

/**
 * @tc.name: UpdateParticleNewPath001
 * @tc.desc: When particleFields_ is set, UpdateParticle runs the unified path.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleSystemTest, UpdateParticleNewPath001, TestSize.Level1)
{
    auto col = std::make_shared<ParticleFieldCollection>();
    col->Add(MakeNoiseField());
    system_->UpdateFields(col);

    std::vector<std::shared_ptr<RSRenderParticle>> particles;
    particles.push_back(nullptr);
    system_->UpdateParticle(16000000, particles);
    EXPECT_TRUE(particles.empty());
}

} // namespace Rosen
} // namespace OHOS
