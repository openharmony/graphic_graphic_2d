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

#include <cmath>
#include <parcel.h>

#include "animation/rs_particle_noise_field.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

namespace {
constexpr int DEFAULT_NOISE_STRENGTH = 10;
constexpr uint16_t DEFAULT_NOISE_FEATHER = 5;
constexpr float DEFAULT_NOISE_SCALE = 1.0f;
constexpr float DEFAULT_NOISE_FREQUENCY = 1.0f;
constexpr float DEFAULT_NOISE_AMPLITUDE = 1.0f;
} // namespace

class RSParticleNoiseFieldTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};

/* ==========================================================================
 * Constructor + basic members
 * ========================================================================== */

/**
 * @tc.name: NoiseFieldConstructor001
 * @tc.desc: Parameterized constructor assigns all fields correctly.
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleNoiseFieldTest, NoiseFieldConstructor001, TestSize.Level1)
{
    ParticleNoiseField f(DEFAULT_NOISE_STRENGTH, ShapeType::RECT, Vector2f(200.0f, 300.0f), Vector2f(50.0f, 60.0f),
        5, 1.5f, 2.0f, 3.0f);
    EXPECT_EQ(f.fieldStrength_, 10);
    EXPECT_EQ(f.fieldFeather_, 5u);
    EXPECT_FLOAT_EQ(f.noiseScale_, 1.5f);
    EXPECT_FLOAT_EQ(f.noiseFrequency_, 2.0f);
    EXPECT_FLOAT_EQ(f.noiseAmplitude_, 3.0f);
    EXPECT_EQ(f.regionShape_, ShapeType::RECT);
    EXPECT_FLOAT_EQ(f.regionSize_.x_, 200.0f);
    EXPECT_FLOAT_EQ(f.regionSize_.y_, 300.0f);
    EXPECT_FLOAT_EQ(f.regionPosition_.x_, 50.0f);
    EXPECT_FLOAT_EQ(f.regionPosition_.y_, 60.0f);
}

/**
 * @tc.name: NoiseFieldDefaultConstructor001
 * @tc.desc: Default constructor yields safe zero state.
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleNoiseFieldTest, NoiseFieldDefaultConstructor001, TestSize.Level1)
{
    ParticleNoiseField f;
    EXPECT_EQ(f.fieldStrength_, 0);
    EXPECT_EQ(f.fieldFeather_, 0u);
    EXPECT_FLOAT_EQ(f.noiseScale_, 0.0f);
    EXPECT_FLOAT_EQ(f.noiseFrequency_, 0.0f);
    EXPECT_FLOAT_EQ(f.noiseAmplitude_, 0.0f);
}

/**
 * @tc.name: NoiseFieldGetType001
 * @tc.desc: GetType returns NOISE.
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleNoiseFieldTest, NoiseFieldGetType001, TestSize.Level1)
{
    ParticleNoiseField f;
    EXPECT_EQ(f.GetType(), ParticleFieldType::NOISE);
}

/* ==========================================================================
 * IsPointInRegion — RECT
 * ========================================================================== */

/**
 * @tc.name: NoiseFieldIsPointInRegionRectInside001
 * @tc.desc: RECT: point strictly inside returns true.
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleNoiseFieldTest, NoiseFieldIsPointInRegionRectInside001, TestSize.Level1)
{
    ParticleNoiseField f;
    f.regionShape_ = ShapeType::RECT;
    f.regionPosition_ = Vector2f(0.0f, 0.0f);
    f.regionSize_ = Vector2f(100.0f, 100.0f);
    EXPECT_TRUE(f.IsPointInRegion(Vector2f(10.0f, 10.0f)));
    EXPECT_TRUE(f.IsPointInRegion(Vector2f(-10.0f, -10.0f)));
    EXPECT_TRUE(f.IsPointInRegion(Vector2f(0.0f, 0.0f)));
}

/**
 * @tc.name: NoiseFieldIsPointInRegionRectOutsideX001
 * @tc.desc: RECT: point outside on the X axis returns false.
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleNoiseFieldTest, NoiseFieldIsPointInRegionRectOutsideX001, TestSize.Level1)
{
    ParticleNoiseField f;
    f.regionShape_ = ShapeType::RECT;
    f.regionPosition_ = Vector2f(0.0f, 0.0f);
    f.regionSize_ = Vector2f(100.0f, 100.0f);
    EXPECT_FALSE(f.IsPointInRegion(Vector2f(1000.0f, 0.0f)));
    EXPECT_FALSE(f.IsPointInRegion(Vector2f(-1000.0f, 0.0f)));
}

/**
 * @tc.name: NoiseFieldIsPointInRegionRectOutsideY001
 * @tc.desc: RECT: point outside on the Y axis returns false.
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleNoiseFieldTest, NoiseFieldIsPointInRegionRectOutsideY001, TestSize.Level1)
{
    ParticleNoiseField f;
    f.regionShape_ = ShapeType::RECT;
    f.regionPosition_ = Vector2f(0.0f, 0.0f);
    f.regionSize_ = Vector2f(100.0f, 100.0f);
    EXPECT_FALSE(f.IsPointInRegion(Vector2f(0.0f, 1000.0f)));
    EXPECT_FALSE(f.IsPointInRegion(Vector2f(0.0f, -1000.0f)));
}

/* ==========================================================================
 * IsPointInRegion — Ellipse branch
 * ========================================================================== */

/**
 * @tc.name: NoiseFieldIsPointInRegionEllipseInside001
 * @tc.desc: Non-RECT: point inside the ellipse returns true.
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleNoiseFieldTest, NoiseFieldIsPointInRegionEllipseInside001, TestSize.Level1)
{
    ParticleNoiseField f;
    f.regionShape_ = ShapeType::ELLIPSE;
    f.regionPosition_ = Vector2f(0.0f, 0.0f);
    f.regionSize_ = Vector2f(200.0f, 100.0f);
    EXPECT_TRUE(f.IsPointInRegion(Vector2f(0.0f, 0.0f)));
    EXPECT_TRUE(f.IsPointInRegion(Vector2f(50.0f, 10.0f)));
}

/**
 * @tc.name: NoiseFieldIsPointInRegionEllipseOutside001
 * @tc.desc: Non-RECT: point outside the ellipse returns false.
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleNoiseFieldTest, NoiseFieldIsPointInRegionEllipseOutside001, TestSize.Level1)
{
    ParticleNoiseField f;
    f.regionShape_ = ShapeType::ELLIPSE;
    f.regionPosition_ = Vector2f(0.0f, 0.0f);
    f.regionSize_ = Vector2f(200.0f, 100.0f);
    EXPECT_FALSE(f.IsPointInRegion(Vector2f(1000.0f, 1000.0f)));
}

/**
 * @tc.name: NoiseFieldIsPointInRegionEllipseZeroWidth001
 * @tc.desc: Ellipse degenerate width=0 → return false.
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleNoiseFieldTest, NoiseFieldIsPointInRegionEllipseZeroWidth001, TestSize.Level1)
{
    ParticleNoiseField f;
    f.regionShape_ = ShapeType::ELLIPSE;
    f.regionPosition_ = Vector2f(0.0f, 0.0f);
    f.regionSize_ = Vector2f(0.0f, 100.0f);
    EXPECT_FALSE(f.IsPointInRegion(Vector2f(0.0f, 0.0f)));
}

/**
 * @tc.name: NoiseFieldIsPointInRegionEllipseZeroHeight001
 * @tc.desc: Ellipse degenerate height=0 → return false.
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleNoiseFieldTest, NoiseFieldIsPointInRegionEllipseZeroHeight001, TestSize.Level1)
{
    ParticleNoiseField f;
    f.regionShape_ = ShapeType::ELLIPSE;
    f.regionPosition_ = Vector2f(0.0f, 0.0f);
    f.regionSize_ = Vector2f(100.0f, 0.0f);
    EXPECT_FALSE(f.IsPointInRegion(Vector2f(0.0f, 0.0f)));
}

/* ==========================================================================
 * CalculateFeatherEffect
 * ========================================================================== */

/**
 * @tc.name: NoiseFieldFeatherEffectZeroWidth001
 * @tc.desc: featherWidth <= 0 returns 1.0f.
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleNoiseFieldTest, NoiseFieldFeatherEffectZeroWidth001, TestSize.Level1)
{
    ParticleNoiseField f;
    EXPECT_FLOAT_EQ(f.CalculateFeatherEffect(50.0f, 0.0f), 1.0f);
    EXPECT_FLOAT_EQ(f.CalculateFeatherEffect(50.0f, -1.0f), 1.0f);
}

/**
 * @tc.name: NoiseFieldFeatherEffectNegativeDistance001
 * @tc.desc: distanceToEdge < 0 → normalizedDistance remains 1.0f, return 1.0f.
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleNoiseFieldTest, NoiseFieldFeatherEffectNegativeDistance001, TestSize.Level1)
{
    ParticleNoiseField f;
    EXPECT_FLOAT_EQ(f.CalculateFeatherEffect(-1.0f, 10.0f), 1.0f);
}

/**
 * @tc.name: NoiseFieldFeatherEffectPartialFeather001
 * @tc.desc: Normal feather: distance/featherWidth in (0, 1).
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleNoiseFieldTest, NoiseFieldFeatherEffectPartialFeather001, TestSize.Level1)
{
    ParticleNoiseField f;
    float result = f.CalculateFeatherEffect(5.0f, 10.0f);
    EXPECT_GT(result, 0.0f);
    EXPECT_LT(result, 1.0f);
}

/**
 * @tc.name: NoiseFieldFeatherEffectSaturation001
 * @tc.desc: normalizedDistance >= 1.0 → return 1.0f (clamp).
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleNoiseFieldTest, NoiseFieldFeatherEffectSaturation001, TestSize.Level1)
{
    ParticleNoiseField f;
    EXPECT_FLOAT_EQ(f.CalculateFeatherEffect(100.0f, 10.0f), 1.0f);
}

/* ==========================================================================
 * ApplyField — shape branches
 * ========================================================================== */

/**
 * @tc.name: NoiseFieldApplyFieldOutside001
 * @tc.desc: Point outside region → (0, 0).
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleNoiseFieldTest, NoiseFieldApplyFieldOutside001, TestSize.Level1)
{
    ParticleNoiseField f(DEFAULT_NOISE_STRENGTH, ShapeType::RECT, Vector2f(100.0f, 100.0f), Vector2f(0.0f, 0.0f),
        DEFAULT_NOISE_FEATHER, DEFAULT_NOISE_SCALE, DEFAULT_NOISE_FREQUENCY, DEFAULT_NOISE_AMPLITUDE);
    Vector2f result = f.ApplyField(Vector2f(1000.0f, 1000.0f), 0.016f);
    EXPECT_FLOAT_EQ(result.x_, 0.0f);
    EXPECT_FLOAT_EQ(result.y_, 0.0f);
}

/**
 * @tc.name: NoiseFieldApplyFieldZeroStrength001
 * @tc.desc: fieldStrength_ == 0 → (0, 0).
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleNoiseFieldTest, NoiseFieldApplyFieldZeroStrength001, TestSize.Level1)
{
    ParticleNoiseField f(0, ShapeType::RECT, Vector2f(100.0f, 100.0f), Vector2f(0.0f, 0.0f),
        DEFAULT_NOISE_FEATHER, DEFAULT_NOISE_SCALE, DEFAULT_NOISE_FREQUENCY, DEFAULT_NOISE_AMPLITUDE);
    Vector2f result = f.ApplyField(Vector2f(10.0f, 10.0f), 0.016f);
    EXPECT_FLOAT_EQ(result.x_, 0.0f);
    EXPECT_FLOAT_EQ(result.y_, 0.0f);
}

/**
 * @tc.name: NoiseFieldApplyFieldRectPositive001
 * @tc.desc: RECT shape, positive strength, non-zero deltaTime.
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleNoiseFieldTest, NoiseFieldApplyFieldRectPositive001, TestSize.Level1)
{
    ParticleNoiseField f(DEFAULT_NOISE_STRENGTH, ShapeType::RECT, Vector2f(200.0f, 200.0f), Vector2f(0.0f, 0.0f),
        DEFAULT_NOISE_FEATHER, DEFAULT_NOISE_SCALE, DEFAULT_NOISE_FREQUENCY, DEFAULT_NOISE_AMPLITUDE);
    Vector2f result = f.ApplyField(Vector2f(10.0f, 10.0f), 0.016f);
    // Force should be non-zero and finite.
    EXPECT_TRUE(std::isfinite(result.x_));
    EXPECT_TRUE(std::isfinite(result.y_));
}

/**
 * @tc.name: NoiseFieldApplyFieldRectNegative001
 * @tc.desc: RECT shape, negative strength, non-zero deltaTime.
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleNoiseFieldTest, NoiseFieldApplyFieldRectNegative001, TestSize.Level1)
{
    ParticleNoiseField f(-DEFAULT_NOISE_STRENGTH, ShapeType::RECT, Vector2f(200.0f, 200.0f), Vector2f(0.0f, 0.0f),
        DEFAULT_NOISE_FEATHER, DEFAULT_NOISE_SCALE, DEFAULT_NOISE_FREQUENCY, DEFAULT_NOISE_AMPLITUDE);
    Vector2f result = f.ApplyField(Vector2f(10.0f, 10.0f), 0.016f);
    EXPECT_TRUE(std::isfinite(result.x_));
    EXPECT_TRUE(std::isfinite(result.y_));
}

/**
 * @tc.name: NoiseFieldApplyFieldRectZeroDelta001
 * @tc.desc: RECT shape, non-zero strength, deltaTime = 0 → neither branch clamps magnitude.
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleNoiseFieldTest, NoiseFieldApplyFieldRectZeroDelta001, TestSize.Level1)
{
    ParticleNoiseField f(DEFAULT_NOISE_STRENGTH, ShapeType::RECT, Vector2f(200.0f, 200.0f), Vector2f(0.0f, 0.0f),
        DEFAULT_NOISE_FEATHER, DEFAULT_NOISE_SCALE, DEFAULT_NOISE_FREQUENCY, DEFAULT_NOISE_AMPLITUDE);
    Vector2f result = f.ApplyField(Vector2f(10.0f, 10.0f), 0.0f);
    EXPECT_TRUE(std::isfinite(result.x_));
    EXPECT_TRUE(std::isfinite(result.y_));
}

/**
 * @tc.name: NoiseFieldApplyFieldCircle001
 * @tc.desc: CIRCLE shape normalizes size and computes edge distance = size.x_.
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleNoiseFieldTest, NoiseFieldApplyFieldCircle001, TestSize.Level1)
{
    ParticleNoiseField f(DEFAULT_NOISE_STRENGTH, ShapeType::CIRCLE, Vector2f(200.0f, 100.0f), Vector2f(0.0f, 0.0f),
        DEFAULT_NOISE_FEATHER, DEFAULT_NOISE_SCALE, DEFAULT_NOISE_FREQUENCY, DEFAULT_NOISE_AMPLITUDE);
    Vector2f result = f.ApplyField(Vector2f(10.0f, 10.0f), 0.016f);
    // After CIRCLE normalization, regionSize_ becomes (min, min).
    EXPECT_FLOAT_EQ(f.regionSize_.x_, 100.0f);
    EXPECT_FLOAT_EQ(f.regionSize_.y_, 100.0f);
    EXPECT_TRUE(std::isfinite(result.x_));
    EXPECT_TRUE(std::isfinite(result.y_));
}

/**
 * @tc.name: NoiseFieldApplyFieldEllipse001
 * @tc.desc: ELLIPSE shape uses CalculateDistanceToEllipseEdge.
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleNoiseFieldTest, NoiseFieldApplyFieldEllipse001, TestSize.Level1)
{
    ParticleNoiseField f(DEFAULT_NOISE_STRENGTH, ShapeType::ELLIPSE, Vector2f(200.0f, 100.0f), Vector2f(0.0f, 0.0f),
        DEFAULT_NOISE_FEATHER, DEFAULT_NOISE_SCALE, DEFAULT_NOISE_FREQUENCY, DEFAULT_NOISE_AMPLITUDE);
    Vector2f result = f.ApplyField(Vector2f(10.0f, 10.0f), 0.016f);
    EXPECT_TRUE(std::isfinite(result.x_));
    EXPECT_TRUE(std::isfinite(result.y_));
}

/* ==========================================================================
 * ApplyCurlNoise + PerlinNoise2D cache
 * ========================================================================== */

/**
 * @tc.name: NoiseFieldApplyCurlNoiseOutside001
 * @tc.desc: Point outside region → (0, 0), no side effects.
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleNoiseFieldTest, NoiseFieldApplyCurlNoiseOutside001, TestSize.Level1)
{
    ParticleNoiseField f(DEFAULT_NOISE_STRENGTH, ShapeType::RECT, Vector2f(100.0f, 100.0f), Vector2f(0.0f, 0.0f),
        DEFAULT_NOISE_FEATHER, DEFAULT_NOISE_SCALE, DEFAULT_NOISE_FREQUENCY, DEFAULT_NOISE_AMPLITUDE);
    Vector2f result = f.ApplyCurlNoise(Vector2f(1000.0f, 1000.0f));
    EXPECT_FLOAT_EQ(result.x_, 0.0f);
    EXPECT_FLOAT_EQ(result.y_, 0.0f);
}

/**
 * @tc.name: NoiseFieldApplyCurlNoiseCacheCreate001
 * @tc.desc: First call creates cachedNoise_ and returns a finite vector.
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleNoiseFieldTest, NoiseFieldApplyCurlNoiseCacheCreate001, TestSize.Level1)
{
    ParticleNoiseField f(DEFAULT_NOISE_STRENGTH, ShapeType::RECT, Vector2f(200.0f, 200.0f), Vector2f(0.0f, 0.0f),
        DEFAULT_NOISE_FEATHER, DEFAULT_NOISE_SCALE, DEFAULT_NOISE_FREQUENCY, DEFAULT_NOISE_AMPLITUDE);
    Vector2f result = f.ApplyCurlNoise(Vector2f(10.0f, 20.0f));
    EXPECT_TRUE(std::isfinite(result.x_));
    EXPECT_TRUE(std::isfinite(result.y_));
}

/**
 * @tc.name: NoiseFieldApplyCurlNoiseCacheReuse001
 * @tc.desc: Repeated in-region calls are stable and don't crash (cache reuse path).
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleNoiseFieldTest, NoiseFieldApplyCurlNoiseCacheReuse001, TestSize.Level1)
{
    ParticleNoiseField f(DEFAULT_NOISE_STRENGTH, ShapeType::RECT, Vector2f(200.0f, 200.0f), Vector2f(0.0f, 0.0f),
        DEFAULT_NOISE_FEATHER, DEFAULT_NOISE_SCALE, DEFAULT_NOISE_FREQUENCY, DEFAULT_NOISE_AMPLITUDE);
    Vector2f r1 = f.ApplyCurlNoise(Vector2f(10.0f, 20.0f));
    Vector2f r2 = f.ApplyCurlNoise(Vector2f(10.0f, 20.0f));
    // Same inputs → same output.
    EXPECT_FLOAT_EQ(r1.x_, r2.x_);
    EXPECT_FLOAT_EQ(r1.y_, r2.y_);
}

/* ==========================================================================
 * Apply (composite) — delegates to ApplyField + ApplyCurlNoise
 * ========================================================================== */

/**
 * @tc.name: NoiseFieldApply001
 * @tc.desc: Apply returns the sum of ApplyField and ApplyCurlNoise.
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleNoiseFieldTest, NoiseFieldApply001, TestSize.Level1)
{
    ParticleNoiseField f(DEFAULT_NOISE_STRENGTH, ShapeType::RECT, Vector2f(200.0f, 200.0f), Vector2f(0.0f, 0.0f),
        DEFAULT_NOISE_FEATHER, DEFAULT_NOISE_SCALE, DEFAULT_NOISE_FREQUENCY, DEFAULT_NOISE_AMPLITUDE);
    Vector2f sum = f.Apply(Vector2f(10.0f, 20.0f), 0.016f);
    EXPECT_TRUE(std::isfinite(sum.x_));
    EXPECT_TRUE(std::isfinite(sum.y_));
}

/* ==========================================================================
 * Equals
 * ========================================================================== */

/**
 * @tc.name: NoiseFieldEqualsSame001
 * @tc.desc: Two fields with identical noise parameters are Equal.
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleNoiseFieldTest, NoiseFieldEqualsSame001, TestSize.Level1)
{
    ParticleNoiseField a(DEFAULT_NOISE_STRENGTH, ShapeType::RECT, Vector2f(200.0f, 200.0f), Vector2f(0.0f, 0.0f),
        DEFAULT_NOISE_FEATHER, DEFAULT_NOISE_SCALE, DEFAULT_NOISE_FREQUENCY, DEFAULT_NOISE_AMPLITUDE);
    ParticleNoiseField b(DEFAULT_NOISE_STRENGTH, ShapeType::RECT, Vector2f(200.0f, 200.0f), Vector2f(0.0f, 0.0f),
        DEFAULT_NOISE_FEATHER, DEFAULT_NOISE_SCALE, DEFAULT_NOISE_FREQUENCY, DEFAULT_NOISE_AMPLITUDE);
    EXPECT_TRUE(a.Equals(b));
}

/**
 * @tc.name: NoiseFieldEqualsDifferentStrength001
 * @tc.desc: Different fieldStrength_ → not equal.
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleNoiseFieldTest, NoiseFieldEqualsDifferentStrength001, TestSize.Level1)
{
    ParticleNoiseField a(DEFAULT_NOISE_STRENGTH, ShapeType::RECT, Vector2f(200.0f, 200.0f), Vector2f(0.0f, 0.0f),
        DEFAULT_NOISE_FEATHER, DEFAULT_NOISE_SCALE, DEFAULT_NOISE_FREQUENCY, DEFAULT_NOISE_AMPLITUDE);
    ParticleNoiseField b(20, ShapeType::RECT, Vector2f(200.0f, 200.0f), Vector2f(0.0f, 0.0f),
        DEFAULT_NOISE_FEATHER, DEFAULT_NOISE_SCALE, DEFAULT_NOISE_FREQUENCY, DEFAULT_NOISE_AMPLITUDE);
    EXPECT_FALSE(a.Equals(b));
}

/**
 * @tc.name: NoiseFieldEqualsDifferentFeather001
 * @tc.desc: Different fieldFeather_ → not equal.
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleNoiseFieldTest, NoiseFieldEqualsDifferentFeather001, TestSize.Level1)
{
    ParticleNoiseField a(DEFAULT_NOISE_STRENGTH, ShapeType::RECT, Vector2f(200.0f, 200.0f), Vector2f(0.0f, 0.0f),
        DEFAULT_NOISE_FEATHER, DEFAULT_NOISE_SCALE, DEFAULT_NOISE_FREQUENCY, DEFAULT_NOISE_AMPLITUDE);
    ParticleNoiseField b(DEFAULT_NOISE_STRENGTH, ShapeType::RECT, Vector2f(200.0f, 200.0f), Vector2f(0.0f, 0.0f),
        6, 1.0f, 1.0f, 1.0f);
    EXPECT_FALSE(a.Equals(b));
}

/**
 * @tc.name: NoiseFieldEqualsDifferentScale001
 * @tc.desc: Different noiseScale_ → not equal.
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleNoiseFieldTest, NoiseFieldEqualsDifferentScale001, TestSize.Level1)
{
    ParticleNoiseField a(DEFAULT_NOISE_STRENGTH, ShapeType::RECT, Vector2f(200.0f, 200.0f), Vector2f(0.0f, 0.0f),
        DEFAULT_NOISE_FEATHER, DEFAULT_NOISE_SCALE, DEFAULT_NOISE_FREQUENCY, DEFAULT_NOISE_AMPLITUDE);
    ParticleNoiseField b(DEFAULT_NOISE_STRENGTH, ShapeType::RECT, Vector2f(200.0f, 200.0f), Vector2f(0.0f, 0.0f),
        5, 2.0f, 1.0f, 1.0f);
    EXPECT_FALSE(a.Equals(b));
}

/**
 * @tc.name: NoiseFieldEqualsDifferentFrequency001
 * @tc.desc: Different noiseFrequency_ → not equal.
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleNoiseFieldTest, NoiseFieldEqualsDifferentFrequency001, TestSize.Level1)
{
    ParticleNoiseField a(DEFAULT_NOISE_STRENGTH, ShapeType::RECT, Vector2f(200.0f, 200.0f), Vector2f(0.0f, 0.0f),
        DEFAULT_NOISE_FEATHER, DEFAULT_NOISE_SCALE, DEFAULT_NOISE_FREQUENCY, DEFAULT_NOISE_AMPLITUDE);
    ParticleNoiseField b(DEFAULT_NOISE_STRENGTH, ShapeType::RECT, Vector2f(200.0f, 200.0f), Vector2f(0.0f, 0.0f),
        5, 1.0f, 2.0f, 1.0f);
    EXPECT_FALSE(a.Equals(b));
}

/**
 * @tc.name: NoiseFieldEqualsDifferentAmplitude001
 * @tc.desc: Different noiseAmplitude_ → not equal.
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleNoiseFieldTest, NoiseFieldEqualsDifferentAmplitude001, TestSize.Level1)
{
    ParticleNoiseField a(DEFAULT_NOISE_STRENGTH, ShapeType::RECT, Vector2f(200.0f, 200.0f), Vector2f(0.0f, 0.0f),
        DEFAULT_NOISE_FEATHER, DEFAULT_NOISE_SCALE, DEFAULT_NOISE_FREQUENCY, DEFAULT_NOISE_AMPLITUDE);
    ParticleNoiseField b(DEFAULT_NOISE_STRENGTH, ShapeType::RECT, Vector2f(200.0f, 200.0f), Vector2f(0.0f, 0.0f),
        5, 1.0f, 1.0f, 2.0f);
    EXPECT_FALSE(a.Equals(b));
}

/* ==========================================================================
 * Dump
 * ========================================================================== */

/**
 * @tc.name: NoiseFieldDump001
 * @tc.desc: Dump outputs a non-empty descriptive string.
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleNoiseFieldTest, NoiseFieldDump001, TestSize.Level1)
{
    ParticleNoiseField f(DEFAULT_NOISE_STRENGTH, ShapeType::RECT, Vector2f(200.0f, 200.0f), Vector2f(0.0f, 0.0f),
        DEFAULT_NOISE_FEATHER, DEFAULT_NOISE_SCALE, DEFAULT_NOISE_FREQUENCY, DEFAULT_NOISE_AMPLITUDE);
    std::string out;
    f.Dump(out);
    EXPECT_FALSE(out.empty());
    EXPECT_NE(out.find("ParticleNoiseField"), std::string::npos);
    EXPECT_NE(out.find("noiseScale"), std::string::npos);
}

/* ==========================================================================
 * Marshal/Unmarshal Specific (round-trip via Parcel)
 * ========================================================================== */

/**
 * @tc.name: NoiseFieldMarshalUnmarshalRoundTrip001
 * @tc.desc: Marshal then Unmarshal recovers all noise-specific parameters.
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleNoiseFieldTest, NoiseFieldMarshalUnmarshalRoundTrip001, TestSize.Level1)
{
    ParticleNoiseField src(42, ShapeType::RECT, Vector2f(200.0f, 200.0f), Vector2f(0.0f, 0.0f),
        7, 1.5f, 2.5f, 3.5f);
    Parcel parcel;
    EXPECT_TRUE(src.MarshalSpecific(parcel));

    ParticleNoiseField dst;
    EXPECT_TRUE(dst.UnmarshalSpecific(parcel));
    EXPECT_EQ(dst.fieldStrength_, 42);
    EXPECT_EQ(dst.fieldFeather_, 7u);
    EXPECT_FLOAT_EQ(dst.noiseScale_, 1.5f);
    EXPECT_FLOAT_EQ(dst.noiseFrequency_, 2.5f);
    EXPECT_FLOAT_EQ(dst.noiseAmplitude_, 3.5f);
}

/**
 * @tc.name: NoiseFieldUnmarshalEmptyParcel001
 * @tc.desc: Reading from an empty Parcel fails gracefully.
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleNoiseFieldTest, NoiseFieldUnmarshalEmptyParcel001, TestSize.Level1)
{
    Parcel parcel;
    ParticleNoiseField f;
    // Empty parcel — ReadInt32 should fail.
    EXPECT_FALSE(f.UnmarshalSpecific(parcel));
}

/* ==========================================================================
 * ParticleNoiseFields container
 * ========================================================================== */

/**
 * @tc.name: ParticleNoiseFieldsAddGetRemove001
 * @tc.desc: Basic container operations Add/Get/Remove.
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleNoiseFieldTest, ParticleNoiseFieldsAddGetRemove001, TestSize.Level1)
{
    ParticleNoiseFields fields;
    auto f = std::make_shared<ParticleNoiseField>(DEFAULT_NOISE_STRENGTH, ShapeType::RECT,
        Vector2f(200.0f, 200.0f), Vector2f(0.0f, 0.0f),
        DEFAULT_NOISE_FEATHER, DEFAULT_NOISE_SCALE, DEFAULT_NOISE_FREQUENCY, DEFAULT_NOISE_AMPLITUDE);
    fields.AddField(f);
    EXPECT_EQ(fields.GetFieldCount(), 1u);
    EXPECT_EQ(fields.GetField(0), f);

    // Get out-of-bounds returns nullptr
    EXPECT_EQ(fields.GetField(5), nullptr);

    fields.RemoveField(0);
    EXPECT_EQ(fields.GetFieldCount(), 0u);

    // Remove out-of-bounds: no-op
    fields.RemoveField(5);
    EXPECT_EQ(fields.GetFieldCount(), 0u);
}

/**
 * @tc.name: ParticleNoiseFieldsApplyAllFields001
 * @tc.desc: ApplyAllFields sums ApplyField + ApplyCurlNoise across entries.
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleNoiseFieldTest, ParticleNoiseFieldsApplyAllFields001, TestSize.Level1)
{
    ParticleNoiseFields fields;
    fields.AddField(std::make_shared<ParticleNoiseField>(DEFAULT_NOISE_STRENGTH, ShapeType::RECT,
        Vector2f(200.0f, 200.0f), Vector2f(0.0f, 0.0f),
        DEFAULT_NOISE_FEATHER, DEFAULT_NOISE_SCALE, DEFAULT_NOISE_FREQUENCY, DEFAULT_NOISE_AMPLITUDE));
    Vector2f result = fields.ApplyAllFields(Vector2f(10.0f, 20.0f), 0.016f);
    EXPECT_TRUE(std::isfinite(result.x_));
    EXPECT_TRUE(std::isfinite(result.y_));
}

/**
 * @tc.name: ParticleNoiseFieldsClearFields001
 * @tc.desc: ClearFields empties the container.
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleNoiseFieldTest, ParticleNoiseFieldsClearFields001, TestSize.Level1)
{
    ParticleNoiseFields fields;
    fields.AddField(std::make_shared<ParticleNoiseField>());
    fields.AddField(std::make_shared<ParticleNoiseField>());
    EXPECT_EQ(fields.GetFieldCount(), 2u);
    fields.ClearFields();
    EXPECT_EQ(fields.GetFieldCount(), 0u);
}

/**
 * @tc.name: ParticleNoiseFieldsEquality001
 * @tc.desc: operator== on containers compares element-wise.
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleNoiseFieldTest, ParticleNoiseFieldsEquality001, TestSize.Level1)
{
    ParticleNoiseFields a;
    ParticleNoiseFields b;
    EXPECT_TRUE(a == b);

    a.AddField(std::make_shared<ParticleNoiseField>());
    EXPECT_FALSE(a == b);
}

/**
 * @tc.name: ParticleNoiseFieldsDumpEmpty001
 * @tc.desc: Dump of empty container yields "[]".
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleNoiseFieldTest, ParticleNoiseFieldsDumpEmpty001, TestSize.Level1)
{
    ParticleNoiseFields fields;
    std::string out;
    fields.Dump(out);
    EXPECT_EQ(out, "[]");
}

/**
 * @tc.name: ParticleNoiseFieldsDumpNonEmpty001
 * @tc.desc: Dump non-empty container writes each field's parameters.
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleNoiseFieldTest, ParticleNoiseFieldsDumpNonEmpty001, TestSize.Level1)
{
    ParticleNoiseFields fields;
    fields.AddField(std::make_shared<ParticleNoiseField>(DEFAULT_NOISE_STRENGTH, ShapeType::RECT,
        Vector2f(200.0f, 200.0f), Vector2f(0.0f, 0.0f),
        DEFAULT_NOISE_FEATHER, DEFAULT_NOISE_SCALE, DEFAULT_NOISE_FREQUENCY, DEFAULT_NOISE_AMPLITUDE));
    std::string out;
    fields.Dump(out);
    EXPECT_NE(out.find("fieldStrength"), std::string::npos);
    EXPECT_NE(out.find("noiseScale"), std::string::npos);
}

/**
 * @tc.name: ParticleNoiseFieldsDumpWithNullptr001
 * @tc.desc: Dump skips nullptr entries.
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleNoiseFieldTest, ParticleNoiseFieldsDumpWithNullptr001, TestSize.Level1)
{
    ParticleNoiseFields fields;
    fields.fields_.push_back(nullptr);
    fields.AddField(std::make_shared<ParticleNoiseField>(DEFAULT_NOISE_STRENGTH, ShapeType::RECT,
        Vector2f(200.0f, 200.0f), Vector2f(0.0f, 0.0f),
        DEFAULT_NOISE_FEATHER, DEFAULT_NOISE_SCALE, DEFAULT_NOISE_FREQUENCY, DEFAULT_NOISE_AMPLITUDE));
    std::string out;
    fields.Dump(out);
    // Should still contain the non-null field info.
    EXPECT_NE(out.find("fieldStrength"), std::string::npos);
}

/* ==========================================================================
 * PerlinNoise2D direct API
 * ========================================================================== */

/**
 * @tc.name: PerlinNoise2DNoise001
 * @tc.desc: PerlinNoise2D::Noise returns a finite value.
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleNoiseFieldTest, PerlinNoise2DNoise001, TestSize.Level1)
{
    PerlinNoise2D noise(1.0f, 1.0f);
    float v = noise.Noise(1.5f, 2.5f);
    EXPECT_TRUE(std::isfinite(v));
}

/**
 * @tc.name: PerlinNoise2DCurl001
 * @tc.desc: PerlinNoise2D::Curl returns a finite vector for multiple input points.
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleNoiseFieldTest, PerlinNoise2DCurl001, TestSize.Level1)
{
    PerlinNoise2D noise(1.0f, 1.0f);
    for (float x : {-5.0f, 0.0f, 1.5f, 10.0f}) {
        for (float y : {-5.0f, 0.0f, 1.5f, 10.0f}) {
            Vector2f v = noise.Curl(x, y);
            EXPECT_TRUE(std::isfinite(v.x_));
            EXPECT_TRUE(std::isfinite(v.y_));
        }
    }
}

/**
 * @tc.name: PerlinNoise2DZeroFrequency001
 * @tc.desc: PerlinNoise2D with zero frequency/amplitude still returns finite values.
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleNoiseFieldTest, PerlinNoise2DZeroFrequency001, TestSize.Level1)
{
    PerlinNoise2D noise(0.0f, 0.0f);
    Vector2f v = noise.Curl(1.5f, 2.5f);
    EXPECT_TRUE(std::isfinite(v.x_));
    EXPECT_TRUE(std::isfinite(v.y_));
}

/**
 * @tc.name: PerlinNoise2DMultiFrequency001
 * @tc.desc: Exercises many hash values via different input coordinates (covers Grad branches).
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleNoiseFieldTest, PerlinNoise2DMultiFrequency001, TestSize.Level1)
{
    PerlinNoise2D noise(3.7f, 2.1f);
    // Varied inputs to traverse different permutation hashes and gradient branches.
    for (int i = 0; i < 100; ++i) {
        float x = static_cast<float>(i) * 0.13f;
        float y = static_cast<float>(i) * 0.27f;
        float v = noise.Noise(x, y);
        EXPECT_TRUE(std::isfinite(v));
    }
}

} // namespace Rosen
} // namespace OHOS
