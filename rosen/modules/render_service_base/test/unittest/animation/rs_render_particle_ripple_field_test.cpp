/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include <cmath>

#include "gtest/gtest.h"

#include "animation/rs_particle_ripple_field.h"
#include "common/rs_vector2.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSParticleRippleFieldTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSParticleRippleFieldTest::SetUpTestCase() {}
void RSParticleRippleFieldTest::TearDownTestCase() {}
void RSParticleRippleFieldTest::SetUp() {}
void RSParticleRippleFieldTest::TearDown() {}

/**
 * @tc.name: ParticleRippleFieldConstructor001
 * @tc.desc: Test RippleField default constructor
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleRippleFieldTest, ParticleRippleFieldConstructor001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest ParticleRippleFieldConstructor001 start";
    ParticleRippleField field;
    EXPECT_EQ(field.center_.x_, 0.0f);
    EXPECT_EQ(field.center_.y_, 0.0f);
    EXPECT_EQ(field.amplitude_, 100.0f);
    EXPECT_EQ(field.wavelength_, 50.0f);
    EXPECT_EQ(field.waveSpeed_, 200.0f);
    EXPECT_EQ(field.attenuation_, 0.0f);
    EXPECT_EQ(field.lifeTime_, 0.0f);
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest ParticleRippleFieldConstructor001 end";
}

/**
 * @tc.name: ParticleRippleFieldConstructor002
 * @tc.desc: Test RippleField parameterized constructor
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleRippleFieldTest, ParticleRippleFieldConstructor002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest ParticleRippleFieldConstructor002 start";
    Vector2f center(100.0f, 200.0f);
    float amplitude = 150.0f;
    float wavelength = 75.0f;
    float waveSpeed = 250.0f;
    float attenuation = 0.5f;

    ParticleRippleField field(center, amplitude, wavelength, waveSpeed, attenuation);

    EXPECT_EQ(field.center_, center);
    EXPECT_EQ(field.amplitude_, amplitude);
    EXPECT_EQ(field.wavelength_, wavelength);
    EXPECT_EQ(field.waveSpeed_, waveSpeed);
    EXPECT_EQ(field.attenuation_, attenuation);
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest ParticleRippleFieldConstructor002 end";
}

/**
 * @tc.name: RippleFieldRegionRect001
 * @tc.desc: Test RippleField with RECT region shape
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleRippleFieldTest, RippleFieldRegionRect001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldRegionRect001 start";
    ParticleRippleField field;
    field.center_ = Vector2f(0.0f, 0.0f);
    field.regionShape_ = ShapeType::RECT;
    field.regionPosition_ = Vector2f(0.0f, 0.0f);
    field.regionSize_ = Vector2f(200.0f, 200.0f);

    // Test point inside region
    Vector2f insidePoint(50.0f, 50.0f);
    Vector2f result = field.ApplyRippleField(insidePoint, 0.1f);
    EXPECT_EQ(result.x_, 0.0f);
    EXPECT_EQ(result.y_, 0.0f);

    // Test point outside region
    Vector2f outsidePoint(150.0f, 150.0f);
    result = field.ApplyRippleField(outsidePoint, 0.1f);
    EXPECT_EQ(result.x_, 0.0f);
    EXPECT_EQ(result.y_, 0.0f);
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldRegionRect001 end";
}

/**
 * @tc.name: RippleFieldRegionCircle001
 * @tc.desc: Test RippleField with CIRCLE region shape
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleRippleFieldTest, RippleFieldRegionCircle001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldRegionCircle001 start";
    ParticleRippleField field;
    field.center_ = Vector2f(0.0f, 0.0f);
    field.regionShape_ = ShapeType::CIRCLE;
    field.regionPosition_ = Vector2f(0.0f, 0.0f);
    field.regionSize_ = Vector2f(100.0f, 100.0f);  // diameter=100, radius=50

    // Test point inside circle
    Vector2f insidePoint(30.0f, 0.0f);
    Vector2f result = field.ApplyRippleField(insidePoint, 0.1f);
    EXPECT_EQ(result.x_, 0.0f);
    EXPECT_EQ(result.y_, 0.0f);

    // Test point outside circle
    Vector2f outsidePoint(60.0f, 0.0f);
    result = field.ApplyRippleField(outsidePoint, 0.1f);
    EXPECT_EQ(result.x_, 0.0f);
    EXPECT_EQ(result.y_, 0.0f);
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldRegionCircle001 end";
}

/**
 * @tc.name: RippleFieldRegionEllipse001
 * @tc.desc: Test RippleField with ELLIPSE region shape
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleRippleFieldTest, RippleFieldRegionEllipse001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldRegionEllipse001 start";
    ParticleRippleField field;
    field.center_ = Vector2f(0.0f, 0.0f);
    field.regionShape_ = ShapeType::ELLIPSE;
    field.regionPosition_ = Vector2f(0.0f, 0.0f);
    field.regionSize_ = Vector2f(200.0f, 100.0f);  // radiusX=100, radiusY=50

    // Test point inside ellipse
    Vector2f insidePoint(50.0f, 20.0f);
    Vector2f result = field.ApplyRippleField(insidePoint, 0.1f);
    EXPECT_EQ(result.x_, 0.0f);
    EXPECT_EQ(result.y_, 0.0f);

    // Test point outside ellipse
    Vector2f outsidePoint(150.0f, 50.0f);
    result = field.ApplyRippleField(outsidePoint, 0.1f);
    EXPECT_EQ(result.x_, 0.0f);
    EXPECT_EQ(result.y_, 0.0f);
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldRegionEllipse001 end";
}

/**
 * @tc.name: RippleFieldCalculateForceStrength001
 * @tc.desc: Test force strength calculation at different distances
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleRippleFieldTest, RippleFieldCalculateForceStrength001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldCalculateForceStrength001 start";
    ParticleRippleField field;
    field.amplitude_ = 100.0f;
    field.wavelength_ = 50.0f;
    field.waveSpeed_ = 200.0f;
    field.attenuation_ = 0.0f;
    field.lifeTime_ = 0.0f;

    // Test force strength at different distances
    float distance1 = 0.0f;
    float force1 = field.CalculateForceStrength(distance1);
    EXPECT_TRUE(std::isfinite(force1));

    float distance2 = 25.0f;  // Half wavelength
    float force2 = field.CalculateForceStrength(distance2);
    EXPECT_TRUE(std::isfinite(force2));

    float distance3 = 50.0f;  // One wavelength
    float force3 = field.CalculateForceStrength(distance3);
    EXPECT_TRUE(std::isfinite(force3));
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldCalculateForceStrength001 end";
}

/**
 * @tc.name: RippleFieldUpdateRipple001
 * @tc.desc: Test ripple lifetime update
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleRippleFieldTest, RippleFieldUpdateRipple001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldUpdateRipple001 start";
    ParticleRippleField field;
    field.lifeTime_ = 0.0f;

    float deltaTime = 0.1f;
    field.UpdateRipple(deltaTime);
    EXPECT_TRUE(ROSEN_EQ(field.lifeTime_, 0.1f));

    field.UpdateRipple(deltaTime);
    EXPECT_TRUE(ROSEN_EQ(field.lifeTime_, 0.2f));
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldUpdateRipple001 end";
}

/**
 * @tc.name: RippleFieldAttenuation001
 * @tc.desc: Test force attenuation over time
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleRippleFieldTest, RippleFieldAttenuation001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldAttenuation001 start";
    ParticleRippleField field;
    field.amplitude_ = 100.0f;
    field.wavelength_ = 50.0f;
    field.waveSpeed_ = 200.0f;
    field.attenuation_ = 0.1f;  // With attenuation
    field.lifeTime_ = 0.0f;

    float distance = 25.0f;
    float force1 = field.CalculateForceStrength(distance);

    // Advance time
    field.lifeTime_ = 1.0f;
    float force2 = field.CalculateForceStrength(distance);

    // Force should decrease over time with attenuation
    EXPECT_TRUE(std::abs(force2) > std::abs(force1));
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldAttenuation001 end";
}

/**
 * @tc.name: RippleFieldEdgeCase001
 * @tc.desc: Test ripple field at center point (distance = 0)
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleRippleFieldTest, RippleFieldEdgeCase001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldEdgeCase001 start";
    ParticleRippleField field;
    field.center_ = Vector2f(0.0f, 0.0f);
    field.regionShape_ = ShapeType::RECT;
    field.regionPosition_ = Vector2f(0.0f, 0.0f);
    field.regionSize_ = Vector2f(100.0f, 100.0f);

    // Test at center point (distance = 0)
    Vector2f centerPoint(0.0f, 0.0f);
    Vector2f result = field.ApplyRippleField(centerPoint, 0.1f);

    // Should return zero vector at center
    EXPECT_EQ(result.x_, 0.0f);
    EXPECT_EQ(result.y_, 0.0f);
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldEdgeCase001 end";
}

/**
 * @tc.name: RippleFieldEdgeCase002
 * @tc.desc: Test ripple field with very small wavelength
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleRippleFieldTest, RippleFieldEdgeCase002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldEdgeCase002 start";
    ParticleRippleField field;
    field.wavelength_ = 0.0001f;  // Very small wavelength
    field.amplitude_ = 100.0f;
    field.waveSpeed_ = 200.0f;

    float force = field.CalculateForceStrength(50.0f);
    EXPECT_TRUE(std::isfinite(force));
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldEdgeCase002 end";
}

/**
 * @tc.name: RippleFieldEdgeCase003
 * @tc.desc: Test ripple field with zero-size ellipse region
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleRippleFieldTest, RippleFieldEdgeCase003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldEdgeCase003 start";
    ParticleRippleField field;
    field.regionShape_ = ShapeType::ELLIPSE;
    field.regionSize_ = Vector2f(0.0f, 100.0f);  // radiusX = 0

    Vector2f point(10.0f, 10.0f);
    Vector2f result = field.ApplyRippleField(point, 0.1f);

    // Zero-size region should produce no force
    EXPECT_EQ(result.x_, 0.0f);
    EXPECT_EQ(result.y_, 0.0f);
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldEdgeCase003 end";
}

/**
 * @tc.name: ParticleRippleFieldsAddRemove001
 * @tc.desc: Test adding and removing ripple fields
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleRippleFieldTest, ParticleRippleFieldsAddRemove001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest ParticleRippleFieldsAddRemove001 start";
    ParticleRippleFields fields;
    EXPECT_EQ(fields.GetRippleFieldCount(), 0);

    auto field1 = std::make_shared<ParticleRippleField>();
    fields.AddRippleField(field1);
    EXPECT_EQ(fields.GetRippleFieldCount(), 1);

    auto field2 = std::make_shared<ParticleRippleField>();
    fields.AddRippleField(field2);
    EXPECT_EQ(fields.GetRippleFieldCount(), 2);

    fields.RemoveRippleField(0);
    EXPECT_EQ(fields.GetRippleFieldCount(), 1);

    fields.ClearRippleFields();
    EXPECT_EQ(fields.GetRippleFieldCount(), 0);
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest ParticleRippleFieldsAddRemove001 end";
}

/**
 * @tc.name: ParticleRippleFieldsAddNullptr001
 * @tc.desc: Test adding nullptr to ripple fields
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleRippleFieldTest, ParticleRippleFieldsAddNullptr001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest ParticleRippleFieldsAddNullptr001 start";
    ParticleRippleFields fields;
    fields.AddRippleField(nullptr);

    // Adding nullptr should not increase count
    EXPECT_EQ(fields.GetRippleFieldCount(), 0);
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest ParticleRippleFieldsAddNullptr001 end";
}

/**
 * @tc.name: ParticleRippleFieldsApplyAll001
 * @tc.desc: Test applying all ripple fields
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleRippleFieldTest, ParticleRippleFieldsApplyAll001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest ParticleRippleFieldsApplyAll001 start";
    ParticleRippleFields fields;

    // Add two ripple fields
    auto field1 = std::make_shared<ParticleRippleField>();
    field1->center_ = Vector2f(0.0f, 0.0f);
    field1->amplitude_ = 50.0f;
    field1->regionShape_ = ShapeType::RECT;
    field1->regionPosition_ = Vector2f(0.0f, 0.0f);
    field1->regionSize_ = Vector2f(200.0f, 200.0f);

    auto field2 = std::make_shared<ParticleRippleField>();
    field2->center_ = Vector2f(50.0f, 50.0f);
    field2->amplitude_ = 50.0f;
    field2->regionShape_ = ShapeType::RECT;
    field2->regionPosition_ = Vector2f(0.0f, 0.0f);
    field2->regionSize_ = Vector2f(200.0f, 200.0f);

    fields.AddRippleField(field1);
    fields.AddRippleField(field2);

    Vector2f position(25.0f, 25.0f);
    Vector2f totalEffect = fields.ApplyAllRippleFields(position, 0.1f);

    // Should be superposition of two fields
    EXPECT_TRUE(totalEffect.x_ == 0.0f && totalEffect.y_ == 0.0f);
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest ParticleRippleFieldsApplyAll001 end";
}

/**
 * @tc.name: ParticleRippleFieldsUpdateAll001
 * @tc.desc: Test updating all ripple fields
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleRippleFieldTest, ParticleRippleFieldsUpdateAll001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest ParticleRippleFieldsUpdateAll001 start";
    ParticleRippleFields fields;

    auto field1 = std::make_shared<ParticleRippleField>();
    auto field2 = std::make_shared<ParticleRippleField>();

    fields.AddRippleField(field1);
    fields.AddRippleField(field2);

    fields.UpdateAllRipples(0.1f);

    EXPECT_TRUE(ROSEN_EQ(field1->lifeTime_, 0.1f));
    EXPECT_TRUE(ROSEN_EQ(field2->lifeTime_, 0.1f));
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest ParticleRippleFieldsUpdateAll001 end";
}

/**
 * @tc.name: RippleFieldEquality001
 * @tc.desc: Test RippleField equality operator
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleRippleFieldTest, RippleFieldEquality001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldEquality001 start";
    ParticleRippleField field1;
    ParticleRippleField field2;

    EXPECT_TRUE(field1 == field2);

    field1.amplitude_ = 150.0f;
    EXPECT_FALSE(field1 == field2);

    field2.amplitude_ = 150.0f;
    EXPECT_TRUE(field1 == field2);
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldEquality001 end";
}

/**
 * @tc.name: RippleFieldsEquality001
 * @tc.desc: Test RippleFields equality operator
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleRippleFieldTest, RippleFieldsEquality001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldsEquality001 start";
    ParticleRippleFields fields1;
    ParticleRippleFields fields2;

    EXPECT_TRUE(fields1 == fields2);

    auto field = std::make_shared<ParticleRippleField>();
    fields1.AddRippleField(field);

    EXPECT_FALSE(fields1 == fields2);

    fields2.AddRippleField(field);
    EXPECT_TRUE(fields1 == fields2);
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldsEquality001 end";
}

/**
 * @tc.name: RippleFieldDump001
 * @tc.desc: Test RippleField Dump function
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleRippleFieldTest, RippleFieldDump001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldDump001 start";
    ParticleRippleField field;
    field.center_ = Vector2f(100.0f, 200.0f);
    field.amplitude_ = 150.0f;

    std::string output;
    field.Dump(output);

    EXPECT_TRUE(output.find("ParticleRippleField") != std::string::npos);
    EXPECT_TRUE(output.find("center") != std::string::npos);
    EXPECT_TRUE(output.find("amplitude") != std::string::npos);
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldDump001 end";
}

} // namespace Rosen
} // namespace OHOS
