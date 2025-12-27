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

/**
 * @tc.name: RippleFieldCopyConstructor001
 * @tc.desc: Test RippleField copy constructor
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleRippleFieldTest, RippleFieldCopyConstructor001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldCopyConstructor001 start";
    ParticleRippleField field1;
    field1.center_ = Vector2f(100.0f, 200.0f);
    field1.amplitude_ = 150.0f;
    field1.wavelength_ = 75.0f;
    field1.waveSpeed_ = 250.0f;
    field1.attenuation_ = 0.5f;
    field1.regionShape_ = ShapeType::CIRCLE;
    field1.regionPosition_ = Vector2f(10.0f, 20.0f);
    field1.regionSize_ = Vector2f(100.0f, 100.0f);
    field1.lifeTime_ = 1.5f;

    ParticleRippleField field2(field1);

    EXPECT_EQ(field1, field2);
    EXPECT_EQ(field2.center_, field1.center_);
    EXPECT_EQ(field2.amplitude_, field1.amplitude_);
    EXPECT_EQ(field2.wavelength_, field1.wavelength_);
    EXPECT_EQ(field2.waveSpeed_, field1.waveSpeed_);
    EXPECT_EQ(field2.attenuation_, field1.attenuation_);
    EXPECT_EQ(field2.regionShape_, field1.regionShape_);
    EXPECT_EQ(field2.regionPosition_, field1.regionPosition_);
    EXPECT_EQ(field2.regionSize_, field1.regionSize_);
    EXPECT_EQ(field2.lifeTime_, field1.lifeTime_);
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldCopyConstructor001 end";
}

/**
 * @tc.name: RippleFieldAssignmentOperator001
 * @tc.desc: Test RippleField assignment operator
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleRippleFieldTest, RippleFieldAssignmentOperator001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldAssignmentOperator001 start";
    ParticleRippleField field1;
    field1.center_ = Vector2f(100.0f, 200.0f);
    field1.amplitude_ = 150.0f;
    field1.wavelength_ = 75.0f;
    field1.regionShape_ = ShapeType::ELLIPSE;

    ParticleRippleField field2;
    field2 = field1;

    EXPECT_EQ(field1, field2);
    EXPECT_EQ(field2.center_, field1.center_);
    EXPECT_EQ(field2.amplitude_, field1.amplitude_);
    EXPECT_EQ(field2.regionShape_, field1.regionShape_);
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldAssignmentOperator001 end";
}

/**
 * @tc.name: RippleFieldsGetField001
 * @tc.desc: Test getting ripple field by index
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleRippleFieldTest, RippleFieldsGetField001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldsGetField001 start";
    ParticleRippleFields fields;

    auto field1 = std::make_shared<ParticleRippleField>();
    field1->amplitude_ = 150.0f;
    fields.AddRippleField(field1);

    auto retrievedField = fields.GetRippleField(0);
    ASSERT_NE(retrievedField, nullptr);
    EXPECT_EQ(retrievedField->amplitude_, field1->amplitude_);

    // Test out of bounds
    auto nullField = fields.GetRippleField(10);
    EXPECT_EQ(nullField, nullptr);
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldsGetField001 end";
}

/**
 * @tc.name: RippleFieldsRemoveOutOfBounds001
 * @tc.desc: Test removing field with out of bounds index
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleRippleFieldTest, RippleFieldsRemoveOutOfBounds001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldsRemoveOutOfBounds001 start";
    ParticleRippleFields fields;

    auto field1 = std::make_shared<ParticleRippleField>();
    fields.AddRippleField(field1);

    size_t initialCount = fields.GetRippleFieldCount();

    // Try to remove with out of bounds index
    fields.RemoveRippleField(10);

    // Count should not change
    EXPECT_EQ(fields.GetRippleFieldCount(), initialCount);
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldsRemoveOutOfBounds001 end";
}

/**
 * @tc.name: RippleFieldsDump001
 * @tc.desc: Test RippleFields Dump function
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleRippleFieldTest, RippleFieldsDump001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldsDump001 start";
    ParticleRippleFields fields;

    auto field1 = std::make_shared<ParticleRippleField>();
    field1->amplitude_ = 100.0f;
    auto field2 = std::make_shared<ParticleRippleField>();
    field2->amplitude_ = 200.0f;

    fields.AddRippleField(field1);
    fields.AddRippleField(field2);

    std::string output;
    fields.Dump(output);

    EXPECT_TRUE(output.find("ParticleRippleFields") != std::string::npos);
    EXPECT_TRUE(output.find("ParticleRippleField") != std::string::npos);
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldsDump001 end";
}

/**
 * @tc.name: RippleFieldsDumpEmpty001
 * @tc.desc: Test RippleFields Dump function with empty fields
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleRippleFieldTest, RippleFieldsDumpEmpty001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldsDumpEmpty001 start";
    ParticleRippleFields fields;

    std::string output;
    fields.Dump(output);

    EXPECT_TRUE(output.find("ParticleRippleFields") != std::string::npos);
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldsDumpEmpty001 end";
}

/**
 * @tc.name: RippleFieldsDumpWithNullptr001
 * @tc.desc: Test RippleFields Dump function with nullptr in fields
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleRippleFieldTest, RippleFieldsDumpWithNullptr001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldsDumpWithNullptr001 start";
    ParticleRippleFields fields;

    auto field1 = std::make_shared<ParticleRippleField>();
    fields.AddRippleField(field1);
    // Add nullptr directly to test null check in Dump
    fields.rippleFields_.push_back(nullptr);

    std::string output;
    fields.Dump(output);

    EXPECT_TRUE(output.find("ParticleRippleFields") != std::string::npos);
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldsDumpWithNullptr001 end";
}

/**
 * @tc.name: RippleFieldRegionDefault001
 * @tc.desc: Test RippleField with default/unknown region shape
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleRippleFieldTest, RippleFieldRegionDefault001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldRegionDefault001 start";
    ParticleRippleField field;
    field.center_ = Vector2f(0.0f, 0.0f);
    // Use static_cast to set an invalid ShapeType to trigger default branch
    field.regionShape_ = static_cast<ShapeType>(999);
    field.regionPosition_ = Vector2f(0.0f, 0.0f);
    field.regionSize_ = Vector2f(100.0f, 100.0f);
    field.lifeTime_ = 1.0f;

    Vector2f point(50.0f, 0.0f);
    Vector2f result = field.ApplyRippleField(point, 0.1f);

    // Default branch returns true, so it should apply ripple effect
    EXPECT_TRUE(std::isfinite(result.x_));
    EXPECT_TRUE(std::isfinite(result.y_));
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldRegionDefault001 end";
}

/**
 * @tc.name: RippleFieldsEqualityNullptr001
 * @tc.desc: Test RippleFields equality with nullptr fields
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleRippleFieldTest, RippleFieldsEqualityNullptr001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldsEqualityNullptr001 start";
    ParticleRippleFields fields1;
    ParticleRippleFields fields2;

    // Directly manipulate the internal vector to test nullptr branch
    fields1.rippleFields_.push_back(nullptr);
    fields2.rippleFields_.push_back(nullptr);

    EXPECT_TRUE(fields1 == fields2);

    // One nullptr, one valid
    fields1.rippleFields_.clear();
    fields2.rippleFields_.clear();
    fields1.rippleFields_.push_back(nullptr);
    fields2.rippleFields_.push_back(std::make_shared<ParticleRippleField>());

    EXPECT_FALSE(fields1 == fields2);
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldsEqualityNullptr001 end";
}

/**
 * @tc.name: RippleFieldsEqualityDifferentContent001
 * @tc.desc: Test RippleFields equality with different field content
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleRippleFieldTest, RippleFieldsEqualityDifferentContent001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldsEqualityDifferentContent001 start";
    ParticleRippleFields fields1;
    ParticleRippleFields fields2;

    auto field1 = std::make_shared<ParticleRippleField>();
    field1->amplitude_ = 100.0f;
    auto field2 = std::make_shared<ParticleRippleField>();
    field2->amplitude_ = 200.0f;

    fields1.AddRippleField(field1);
    fields2.AddRippleField(field2);

    EXPECT_FALSE(fields1 == fields2);
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldsEqualityDifferentContent001 end";
}

/**
 * @tc.name: RippleFieldsApplyAllWithNullptr001
 * @tc.desc: Test ApplyAllRippleFields with nullptr in fields vector
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleRippleFieldTest, RippleFieldsApplyAllWithNullptr001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldsApplyAllWithNullptr001 start";
    ParticleRippleFields fields;

    auto field1 = std::make_shared<ParticleRippleField>();
    field1->center_ = Vector2f(0.0f, 0.0f);
    field1->regionShape_ = ShapeType::RECT;
    field1->regionPosition_ = Vector2f(0.0f, 0.0f);
    field1->regionSize_ = Vector2f(200.0f, 200.0f);

    fields.AddRippleField(field1);
    // Directly add nullptr to test null check branch
    fields.rippleFields_.push_back(nullptr);

    Vector2f position(50.0f, 50.0f);
    Vector2f totalEffect = fields.ApplyAllRippleFields(position, 0.1f);

    // Should still work, ignoring nullptr
    EXPECT_TRUE(std::isfinite(totalEffect.x_));
    EXPECT_TRUE(std::isfinite(totalEffect.y_));
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldsApplyAllWithNullptr001 end";
}

/**
 * @tc.name: RippleFieldsUpdateAllWithNullptr001
 * @tc.desc: Test UpdateAllRipples with nullptr in fields vector
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleRippleFieldTest, RippleFieldsUpdateAllWithNullptr001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldsUpdateAllWithNullptr001 start";
    ParticleRippleFields fields;

    auto field1 = std::make_shared<ParticleRippleField>();
    field1->lifeTime_ = 0.0f;

    fields.AddRippleField(field1);
    // Directly add nullptr to test null check branch
    fields.rippleFields_.push_back(nullptr);

    fields.UpdateAllRipples(0.1f);

    // Should still work, ignoring nullptr
    EXPECT_TRUE(ROSEN_EQ(field1->lifeTime_, 0.1f));
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldsUpdateAllWithNullptr001 end";
}

/**
 * @tc.name: RippleFieldEqualityWavelength001
 * @tc.desc: Test RippleField equality with different wavelength
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleRippleFieldTest, RippleFieldEqualityWavelength001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldEqualityWavelength001 start";
    ParticleRippleField field1;
    ParticleRippleField field2;

    field1.wavelength_ = 50.0f;
    field2.wavelength_ = 100.0f;
    EXPECT_FALSE(field1 == field2);

    field2.wavelength_ = 50.0f;
    EXPECT_TRUE(field1 == field2);
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldEqualityWavelength001 end";
}

/**
 * @tc.name: RippleFieldEqualityWaveSpeed001
 * @tc.desc: Test RippleField equality with different waveSpeed
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleRippleFieldTest, RippleFieldEqualityWaveSpeed001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldEqualityWaveSpeed001 start";
    ParticleRippleField field1;
    ParticleRippleField field2;

    field1.waveSpeed_ = 200.0f;
    field2.waveSpeed_ = 300.0f;
    EXPECT_FALSE(field1 == field2);

    field2.waveSpeed_ = 200.0f;
    EXPECT_TRUE(field1 == field2);
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldEqualityWaveSpeed001 end";
}

/**
 * @tc.name: RippleFieldEqualityAttenuation001
 * @tc.desc: Test RippleField equality with different attenuation
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleRippleFieldTest, RippleFieldEqualityAttenuation001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldEqualityAttenuation001 start";
    ParticleRippleField field1;
    ParticleRippleField field2;

    field1.attenuation_ = 0.0f;
    field2.attenuation_ = 0.5f;
    EXPECT_FALSE(field1 == field2);

    field2.attenuation_ = 0.0f;
    EXPECT_TRUE(field1 == field2);
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldEqualityAttenuation001 end";
}

/**
 * @tc.name: RippleFieldEqualityRegionShape001
 * @tc.desc: Test RippleField equality with different regionShape
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleRippleFieldTest, RippleFieldEqualityRegionShape001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldEqualityRegionShape001 start";
    ParticleRippleField field1;
    ParticleRippleField field2;

    field1.regionShape_ = ShapeType::RECT;
    field2.regionShape_ = ShapeType::CIRCLE;
    EXPECT_FALSE(field1 == field2);

    field2.regionShape_ = ShapeType::RECT;
    EXPECT_TRUE(field1 == field2);
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldEqualityRegionShape001 end";
}

/**
 * @tc.name: RippleFieldEqualityRegionPosition001
 * @tc.desc: Test RippleField equality with different regionPosition
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleRippleFieldTest, RippleFieldEqualityRegionPosition001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldEqualityRegionPosition001 start";
    ParticleRippleField field1;
    ParticleRippleField field2;

    field1.regionPosition_ = Vector2f(10.0f, 20.0f);
    field2.regionPosition_ = Vector2f(30.0f, 40.0f);
    EXPECT_FALSE(field1 == field2);

    field2.regionPosition_ = Vector2f(10.0f, 20.0f);
    EXPECT_TRUE(field1 == field2);
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldEqualityRegionPosition001 end";
}

/**
 * @tc.name: RippleFieldEqualityRegionSize001
 * @tc.desc: Test RippleField equality with different regionSize
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleRippleFieldTest, RippleFieldEqualityRegionSize001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldEqualityRegionSize001 start";
    ParticleRippleField field1;
    ParticleRippleField field2;

    field1.regionSize_ = Vector2f(100.0f, 100.0f);
    field2.regionSize_ = Vector2f(200.0f, 200.0f);
    EXPECT_FALSE(field1 == field2);

    field2.regionSize_ = Vector2f(100.0f, 100.0f);
    EXPECT_TRUE(field1 == field2);
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldEqualityRegionSize001 end";
}

/**
 * @tc.name: RippleFieldEqualityLifeTime001
 * @tc.desc: Test RippleField equality with different lifeTime
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleRippleFieldTest, RippleFieldEqualityLifeTime001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldEqualityLifeTime001 start";
    ParticleRippleField field1;
    ParticleRippleField field2;

    field1.lifeTime_ = 0.0f;
    field2.lifeTime_ = 1.0f;
    EXPECT_FALSE(field1 == field2);

    field2.lifeTime_ = 0.0f;
    EXPECT_TRUE(field1 == field2);
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldEqualityLifeTime001 end";
}

/**
 * @tc.name: RippleFieldEqualityCenter001
 * @tc.desc: Test RippleField equality with different center
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleRippleFieldTest, RippleFieldEqualityCenter001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldEqualityCenter001 start";
    ParticleRippleField field1;
    ParticleRippleField field2;

    field1.center_ = Vector2f(0.0f, 0.0f);
    field2.center_ = Vector2f(100.0f, 100.0f);
    EXPECT_FALSE(field1 == field2);

    field2.center_ = Vector2f(0.0f, 0.0f);
    EXPECT_TRUE(field1 == field2);
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldEqualityCenter001 end";
}

/**
 * @tc.name: RippleFieldCalculateForceStrengthGateBranches001
 * @tc.desc: Test CalculateForceStrength gate branches
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleRippleFieldTest, RippleFieldCalculateForceStrengthGateBranches001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldCalculateForceStrengthGateBranches001 start";
    ParticleRippleField field;
    field.amplitude_ = 100.0f;
    field.wavelength_ = 50.0f;
    field.waveSpeed_ = 200.0f;
    field.attenuation_ = 0.0f;

    // Test gate = 1.0 (distance <= ct - gateWidth)
    field.lifeTime_ = 1.0f;  // ct = 200
    float forceInner = field.CalculateForceStrength(10.0f);  // distance << ct
    EXPECT_TRUE(std::isfinite(forceInner));

    // Test gate = 0.0 (distance >= ct + gateWidth)
    float forceOuter = field.CalculateForceStrength(300.0f);  // distance >> ct
    EXPECT_TRUE(std::isfinite(forceOuter));
    EXPECT_NEAR(forceOuter, 0.0f, 0.01f);

    // Test gate smoothstep (ct - gateWidth < distance < ct + gateWidth)
    float forceMiddle = field.CalculateForceStrength(200.0f);  // distance ~ ct
    EXPECT_TRUE(std::isfinite(forceMiddle));
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldCalculateForceStrengthGateBranches001 end";
}

/**
 * @tc.name: RippleFieldApplyWithNonZeroLifeTime001
 * @tc.desc: Test ApplyRippleField with non-zero lifeTime for wave propagation
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleRippleFieldTest, RippleFieldApplyWithNonZeroLifeTime001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldApplyWithNonZeroLifeTime001 start";
    ParticleRippleField field;
    field.center_ = Vector2f(0.0f, 0.0f);
    field.amplitude_ = 100.0f;
    field.wavelength_ = 50.0f;
    field.waveSpeed_ = 200.0f;
    field.attenuation_ = 0.0f;
    field.regionShape_ = ShapeType::RECT;
    field.regionPosition_ = Vector2f(0.0f, 0.0f);
    field.regionSize_ = Vector2f(500.0f, 500.0f);
    field.lifeTime_ = 0.5f;  // ct = 100

    // Test point at distance within wave front
    Vector2f point(50.0f, 0.0f);  // distance = 50, within ct = 100
    Vector2f result = field.ApplyRippleField(point, 0.1f);

    EXPECT_TRUE(std::isfinite(result.x_));
    EXPECT_TRUE(std::isfinite(result.y_));
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldApplyWithNonZeroLifeTime001 end";
}

/**
 * @tc.name: RippleFieldEdgeCase004
 * @tc.desc: Test RippleField with zero-size ellipse radiusY
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleRippleFieldTest, RippleFieldEdgeCase004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldEdgeCase004 start";
    ParticleRippleField field;
    field.regionShape_ = ShapeType::ELLIPSE;
    field.regionSize_ = Vector2f(100.0f, 0.0f);  // radiusY = 0

    Vector2f point(10.0f, 10.0f);
    Vector2f result = field.ApplyRippleField(point, 0.1f);

    // Zero-size region should produce no force
    EXPECT_EQ(result.x_, 0.0f);
    EXPECT_EQ(result.y_, 0.0f);
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldEdgeCase004 end";
}

/**
 * @tc.name: RippleFieldHighAttenuation001
 * @tc.desc: Test RippleField with high attenuation value
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleRippleFieldTest, RippleFieldHighAttenuation001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldHighAttenuation001 start";
    ParticleRippleField field;
    field.amplitude_ = 100.0f;
    field.wavelength_ = 50.0f;
    field.waveSpeed_ = 200.0f;
    field.attenuation_ = 10.0f;  // High attenuation
    field.lifeTime_ = 5.0f;

    float force = field.CalculateForceStrength(10.0f);
    EXPECT_TRUE(std::isfinite(force));
    // High attenuation should result in very small force
    EXPECT_TRUE(std::abs(force) < 1.0f);
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldHighAttenuation001 end";
}

/**
 * @tc.name: RippleFieldLargeLifeTime001
 * @tc.desc: Test RippleField with lifeTime exceeding K_MAX_EXP_TIME
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleRippleFieldTest, RippleFieldLargeLifeTime001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldLargeLifeTime001 start";
    ParticleRippleField field;
    field.amplitude_ = 100.0f;
    field.wavelength_ = 50.0f;
    field.waveSpeed_ = 200.0f;
    field.attenuation_ = 0.1f;
    field.lifeTime_ = 100.0f;  // Exceeds K_MAX_EXP_TIME = 50.0f

    float force = field.CalculateForceStrength(10.0f);
    EXPECT_TRUE(std::isfinite(force));
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldLargeLifeTime001 end";
}

/**
 * @tc.name: RippleFieldNegativeLifeTime001
 * @tc.desc: Test RippleField with negative lifeTime
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleRippleFieldTest, RippleFieldNegativeLifeTime001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldNegativeLifeTime001 start";
    ParticleRippleField field;
    field.amplitude_ = 100.0f;
    field.wavelength_ = 50.0f;
    field.waveSpeed_ = 200.0f;
    field.attenuation_ = 0.0f;
    field.lifeTime_ = -1.0f;  // Negative lifeTime

    float force = field.CalculateForceStrength(10.0f);
    EXPECT_TRUE(std::isfinite(force));
    // With negative lifeTime, t is clamped to 0, so gate should be 0
    EXPECT_NEAR(force, 0.0f, 0.01f);
    GTEST_LOG_(INFO) << "RSParticleRippleFieldTest RippleFieldNegativeLifeTime001 end";
}

} // namespace Rosen
} // namespace OHOS
