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

#include "gtest/gtest.h"

#include "animation/rs_particle_velocity_field.h"
#include "common/rs_vector2.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSParticleVelocityFieldTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSParticleVelocityFieldTest::SetUpTestCase() {}
void RSParticleVelocityFieldTest::TearDownTestCase() {}
void RSParticleVelocityFieldTest::SetUp() {}
void RSParticleVelocityFieldTest::TearDown() {}

/**
 * @tc.name: ParticleVelocityFieldConstructor001
 * @tc.desc: Test VelocityField default constructor
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleVelocityFieldTest, ParticleVelocityFieldConstructor001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleVelocityFieldTest ParticleVelocityFieldConstructor001 start";
    ParticleVelocityField field;
    EXPECT_EQ(field.velocity_.x_, 0.0f);
    EXPECT_EQ(field.velocity_.y_, 0.0f);
    EXPECT_EQ(field.regionShape_, ShapeType::RECT);
    GTEST_LOG_(INFO) << "RSParticleVelocityFieldTest ParticleVelocityFieldConstructor001 end";
}

/**
 * @tc.name: ParticleVelocityFieldConstructor002
 * @tc.desc: Test VelocityField parameterized constructor
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleVelocityFieldTest, ParticleVelocityFieldConstructor002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleVelocityFieldTest ParticleVelocityFieldConstructor002 start";
    Vector2f velocity(100.0f, 200.0f);
    ParticleVelocityField field(velocity);

    EXPECT_EQ(field.velocity_, velocity);
    GTEST_LOG_(INFO) << "RSParticleVelocityFieldTest ParticleVelocityFieldConstructor002 end";
}

/**
 * @tc.name: VelocityFieldRegionRect001
 * @tc.desc: Test VelocityField with RECT region
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleVelocityFieldTest, VelocityFieldRegionRect001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleVelocityFieldTest VelocityFieldRegionRect001 start";
    ParticleVelocityField field;
    field.velocity_ = Vector2f(100.0f, 50.0f);
    field.regionShape_ = ShapeType::RECT;
    field.regionPosition_ = Vector2f(0.0f, 0.0f);
    field.regionSize_ = Vector2f(200.0f, 200.0f);

    // Inside region
    Vector2f insidePoint(50.0f, 50.0f);
    Vector2f result = field.ApplyVelocityField(insidePoint, 0.1f);
    EXPECT_EQ(result, field.velocity_);

    // Outside region
    Vector2f outsidePoint(150.0f, 150.0f);
    result = field.ApplyVelocityField(outsidePoint, 0.1f);
    EXPECT_EQ(result.x_, 0.0f);
    EXPECT_EQ(result.y_, 0.0f);
    GTEST_LOG_(INFO) << "RSParticleVelocityFieldTest VelocityFieldRegionRect001 end";
}

/**
 * @tc.name: VelocityFieldRegionCircle001
 * @tc.desc: Test VelocityField with CIRCLE region
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleVelocityFieldTest, VelocityFieldRegionCircle001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleVelocityFieldTest VelocityFieldRegionCircle001 start";
    ParticleVelocityField field;
    field.velocity_ = Vector2f(100.0f, 50.0f);
    field.regionShape_ = ShapeType::CIRCLE;
    field.regionPosition_ = Vector2f(0.0f, 0.0f);
    field.regionSize_ = Vector2f(100.0f, 100.0f);  // diameter=100

    // Inside circle
    Vector2f insidePoint(30.0f, 0.0f);
    Vector2f result = field.ApplyVelocityField(insidePoint, 0.1f);
    EXPECT_EQ(result, field.velocity_);

    // Outside circle
    Vector2f outsidePoint(60.0f, 0.0f);
    result = field.ApplyVelocityField(outsidePoint, 0.1f);
    EXPECT_EQ(result.x_, 0.0f);
    EXPECT_EQ(result.y_, 0.0f);
    GTEST_LOG_(INFO) << "RSParticleVelocityFieldTest VelocityFieldRegionCircle001 end";
}

/**
 * @tc.name: VelocityFieldRegionEllipse001
 * @tc.desc: Test VelocityField with ELLIPSE region
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleVelocityFieldTest, VelocityFieldRegionEllipse001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleVelocityFieldTest VelocityFieldRegionEllipse001 start";
    ParticleVelocityField field;
    field.velocity_ = Vector2f(100.0f, 50.0f);
    field.regionShape_ = ShapeType::ELLIPSE;
    field.regionPosition_ = Vector2f(0.0f, 0.0f);
    field.regionSize_ = Vector2f(200.0f, 100.0f);

    // Inside ellipse
    Vector2f insidePoint(50.0f, 20.0f);
    Vector2f result = field.ApplyVelocityField(insidePoint, 0.1f);
    EXPECT_EQ(result, field.velocity_);

    // Outside ellipse
    Vector2f outsidePoint(150.0f, 50.0f);
    result = field.ApplyVelocityField(outsidePoint, 0.1f);
    EXPECT_EQ(result.x_, 0.0f);
    EXPECT_EQ(result.y_, 0.0f);
    GTEST_LOG_(INFO) << "RSParticleVelocityFieldTest VelocityFieldRegionEllipse001 end";
}

/**
 * @tc.name: VelocityFieldEdgeCase001
 * @tc.desc: Test VelocityField with very small region size
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleVelocityFieldTest, VelocityFieldEdgeCase001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleVelocityFieldTest VelocityFieldEdgeCase001 start";
    ParticleVelocityField field;
    field.velocity_ = Vector2f(100.0f, 50.0f);
    field.regionShape_ = ShapeType::ELLIPSE;
    field.regionSize_ = Vector2f(1e-7f, 100.0f);  // Very small radiusX

    Vector2f point(10.0f, 10.0f);
    Vector2f result = field.ApplyVelocityField(point, 0.1f);

    EXPECT_EQ(result.x_, 0.0f);
    EXPECT_EQ(result.y_, 0.0f);
    GTEST_LOG_(INFO) << "RSParticleVelocityFieldTest VelocityFieldEdgeCase001 end";
}

/**
 * @tc.name: VelocityFieldEdgeCase002
 * @tc.desc: Test VelocityField at region boundary
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleVelocityFieldTest, VelocityFieldEdgeCase002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleVelocityFieldTest VelocityFieldEdgeCase002 start";
    ParticleVelocityField field;
    field.velocity_ = Vector2f(100.0f, 50.0f);
    field.regionShape_ = ShapeType::CIRCLE;
    field.regionPosition_ = Vector2f(0.0f, 0.0f);
    field.regionSize_ = Vector2f(100.0f, 100.0f);  // radius=50

    // Exactly on boundary
    Vector2f boundaryPoint(50.0f, 0.0f);
    Vector2f result = field.ApplyVelocityField(boundaryPoint, 0.1f);
    EXPECT_EQ(result, field.velocity_);
    GTEST_LOG_(INFO) << "RSParticleVelocityFieldTest VelocityFieldEdgeCase002 end";
}

/**
 * @tc.name: VelocityFieldEdgeCase003
 * @tc.desc: Test VelocityField with zero velocity
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleVelocityFieldTest, VelocityFieldEdgeCase003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleVelocityFieldTest VelocityFieldEdgeCase003 start";
    ParticleVelocityField field;
    field.velocity_ = Vector2f(0.0f, 0.0f);
    field.regionShape_ = ShapeType::RECT;
    field.regionPosition_ = Vector2f(0.0f, 0.0f);
    field.regionSize_ = Vector2f(100.0f, 100.0f);

    Vector2f point(50.0f, 50.0f);
    Vector2f result = field.ApplyVelocityField(point, 0.1f);

    EXPECT_EQ(result.x_, 0.0f);
    EXPECT_EQ(result.y_, 0.0f);
    GTEST_LOG_(INFO) << "RSParticleVelocityFieldTest VelocityFieldEdgeCase003 end";
}

/**
 * @tc.name: ParticleVelocityFieldsAddRemove001
 * @tc.desc: Test adding and removing velocity fields
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleVelocityFieldTest, ParticleVelocityFieldsAddRemove001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleVelocityFieldTest ParticleVelocityFieldsAddRemove001 start";
    ParticleVelocityFields fields;
    EXPECT_EQ(fields.GetVelocityFieldCount(), 0);

    auto field1 = std::make_shared<ParticleVelocityField>();
    fields.AddVelocityField(field1);
    EXPECT_EQ(fields.GetVelocityFieldCount(), 1);

    auto field2 = std::make_shared<ParticleVelocityField>();
    fields.AddVelocityField(field2);
    EXPECT_EQ(fields.GetVelocityFieldCount(), 2);

    fields.RemoveVelocityField(0);
    EXPECT_EQ(fields.GetVelocityFieldCount(), 1);

    fields.ClearVelocityFields();
    EXPECT_EQ(fields.GetVelocityFieldCount(), 0);
    GTEST_LOG_(INFO) << "RSParticleVelocityFieldTest ParticleVelocityFieldsAddRemove001 end";
}

/**
 * @tc.name: ParticleVelocityFieldsAddNullptr001
 * @tc.desc: Test adding nullptr to velocity fields
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleVelocityFieldTest, ParticleVelocityFieldsAddNullptr001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleVelocityFieldTest ParticleVelocityFieldsAddNullptr001 start";
    ParticleVelocityFields fields;
    fields.AddVelocityField(nullptr);

    // Adding nullptr should not increase count
    EXPECT_EQ(fields.GetVelocityFieldCount(), 0);
    GTEST_LOG_(INFO) << "RSParticleVelocityFieldTest ParticleVelocityFieldsAddNullptr001 end";
}

/**
 * @tc.name: ParticleVelocityFieldsApplyAll001
 * @tc.desc: Test applying all velocity fields (superposition)
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleVelocityFieldTest, ParticleVelocityFieldsApplyAll001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleVelocityFieldTest ParticleVelocityFieldsApplyAll001 start";
    ParticleVelocityFields fields;

    auto field1 = std::make_shared<ParticleVelocityField>();
    field1->velocity_ = Vector2f(100.0f, 0.0f);
    field1->regionShape_ = ShapeType::RECT;
    field1->regionPosition_ = Vector2f(0.0f, 0.0f);
    field1->regionSize_ = Vector2f(200.0f, 200.0f);

    auto field2 = std::make_shared<ParticleVelocityField>();
    field2->velocity_ = Vector2f(0.0f, 50.0f);
    field2->regionShape_ = ShapeType::RECT;
    field2->regionPosition_ = Vector2f(0.0f, 0.0f);
    field2->regionSize_ = Vector2f(200.0f, 200.0f);

    fields.AddVelocityField(field1);
    fields.AddVelocityField(field2);

    Vector2f position(50.0f, 50.0f);
    Vector2f totalVelocity = fields.ApplyAllVelocityFields(position, 0.1f);

    // Should be superposition of two velocities
    EXPECT_EQ(totalVelocity.x_, 100.0f);
    EXPECT_EQ(totalVelocity.y_, 50.0f);
    GTEST_LOG_(INFO) << "RSParticleVelocityFieldTest ParticleVelocityFieldsApplyAll001 end";
}

/**
 * @tc.name: ParticleVelocityFieldsGetField001
 * @tc.desc: Test getting velocity field by index
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleVelocityFieldTest, ParticleVelocityFieldsGetField001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleVelocityFieldTest ParticleVelocityFieldsGetField001 start";
    ParticleVelocityFields fields;

    auto field1 = std::make_shared<ParticleVelocityField>();
    field1->velocity_ = Vector2f(100.0f, 50.0f);
    fields.AddVelocityField(field1);

    auto retrievedField = fields.GetVelocityField(0);
    ASSERT_NE(retrievedField, nullptr);
    EXPECT_EQ(retrievedField->velocity_, field1->velocity_);

    // Test out of bounds
    auto nullField = fields.GetVelocityField(10);
    EXPECT_EQ(nullField, nullptr);
    GTEST_LOG_(INFO) << "RSParticleVelocityFieldTest ParticleVelocityFieldsGetField001 end";
}

/**
 * @tc.name: ParticleVelocityFieldsRemoveOutOfBounds001
 * @tc.desc: Test removing field with out of bounds index
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleVelocityFieldTest, ParticleVelocityFieldsRemoveOutOfBounds001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleVelocityFieldTest ParticleVelocityFieldsRemoveOutOfBounds001 start";
    ParticleVelocityFields fields;

    auto field1 = std::make_shared<ParticleVelocityField>();
    fields.AddVelocityField(field1);

    size_t initialCount = fields.GetVelocityFieldCount();

    // Try to remove with out of bounds index
    fields.RemoveVelocityField(10);

    // Count should not change
    EXPECT_EQ(fields.GetVelocityFieldCount(), initialCount);
    GTEST_LOG_(INFO) << "RSParticleVelocityFieldTest ParticleVelocityFieldsRemoveOutOfBounds001 end";
}

/**
 * @tc.name: VelocityFieldEquality001
 * @tc.desc: Test VelocityField equality operator
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleVelocityFieldTest, VelocityFieldEquality001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleVelocityFieldTest VelocityFieldEquality001 start";
    ParticleVelocityField field1;
    ParticleVelocityField field2;

    EXPECT_TRUE(field1 == field2);

    field1.velocity_ = Vector2f(100.0f, 50.0f);
    EXPECT_FALSE(field1 == field2);

    field2.velocity_ = Vector2f(100.0f, 50.0f);
    EXPECT_TRUE(field1 == field2);
    GTEST_LOG_(INFO) << "RSParticleVelocityFieldTest VelocityFieldEquality001 end";
}

/**
 * @tc.name: VelocityFieldsEquality001
 * @tc.desc: Test VelocityFields equality operator
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleVelocityFieldTest, VelocityFieldsEquality001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleVelocityFieldTest VelocityFieldsEquality001 start";
    ParticleVelocityFields fields1;
    ParticleVelocityFields fields2;

    EXPECT_TRUE(fields1 == fields2);

    auto field = std::make_shared<ParticleVelocityField>();
    fields1.AddVelocityField(field);

    EXPECT_FALSE(fields1 == fields2);

    fields2.AddVelocityField(field);
    EXPECT_TRUE(fields1 == fields2);
    GTEST_LOG_(INFO) << "RSParticleVelocityFieldTest VelocityFieldsEquality001 end";
}

/**
 * @tc.name: VelocityFieldDump001
 * @tc.desc: Test VelocityField Dump function
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleVelocityFieldTest, VelocityFieldDump001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleVelocityFieldTest VelocityFieldDump001 start";
    ParticleVelocityField field;
    field.velocity_ = Vector2f(100.0f, 50.0f);

    std::string output;
    field.Dump(output);

    EXPECT_TRUE(output.find("ParticleVelocityField") != std::string::npos);
    EXPECT_TRUE(output.find("velocity") != std::string::npos);
    GTEST_LOG_(INFO) << "RSParticleVelocityFieldTest VelocityFieldDump001 end";
}

/**
 * @tc.name: VelocityFieldCopyConstructor001
 * @tc.desc: Test VelocityField copy constructor
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleVelocityFieldTest, VelocityFieldCopyConstructor001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleVelocityFieldTest VelocityFieldCopyConstructor001 start";
    ParticleVelocityField field1;
    field1.velocity_ = Vector2f(100.0f, 50.0f);
    field1.regionShape_ = ShapeType::CIRCLE;
    field1.regionPosition_ = Vector2f(10.0f, 20.0f);
    field1.regionSize_ = Vector2f(100.0f, 100.0f);

    ParticleVelocityField field2(field1);

    EXPECT_EQ(field1, field2);
    EXPECT_EQ(field2.velocity_, field1.velocity_);
    EXPECT_EQ(field2.regionShape_, field1.regionShape_);
    EXPECT_EQ(field2.regionPosition_, field1.regionPosition_);
    EXPECT_EQ(field2.regionSize_, field1.regionSize_);
    GTEST_LOG_(INFO) << "RSParticleVelocityFieldTest VelocityFieldCopyConstructor001 end";
}

/**
 * @tc.name: VelocityFieldAssignmentOperator001
 * @tc.desc: Test VelocityField assignment operator
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleVelocityFieldTest, VelocityFieldAssignmentOperator001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleVelocityFieldTest VelocityFieldAssignmentOperator001 start";
    ParticleVelocityField field1;
    field1.velocity_ = Vector2f(100.0f, 50.0f);
    field1.regionShape_ = ShapeType::ELLIPSE;

    ParticleVelocityField field2;
    field2 = field1;

    EXPECT_EQ(field1, field2);
    EXPECT_EQ(field2.velocity_, field1.velocity_);
    EXPECT_EQ(field2.regionShape_, field1.regionShape_);
    GTEST_LOG_(INFO) << "RSParticleVelocityFieldTest VelocityFieldAssignmentOperator001 end";
}

} // namespace Rosen
} // namespace OHOS
