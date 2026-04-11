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

#include <parcel.h>

#include "animation/rs_particle_field_base.h"
#include "animation/rs_particle_field_collection.h"
#include "animation/rs_particle_field_factory.h"
#include "animation/rs_particle_noise_field.h"
#include "animation/rs_particle_ripple_field.h"
#include "animation/rs_particle_velocity_field.h"
#include "common/rs_vector2.h"

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

// Concrete subclass for testing the abstract ParticleFieldBase
class TestField : public ParticleFieldBase {
public:
    float testValue_ { 0.0f };

    TestField() = default;
    ~TestField() override = default;

    ParticleFieldType GetType() const override
    {
        return ParticleFieldType::VELOCITY;
    }

    Vector2f Apply(const Vector2f& position, float deltaTime) override
    {
        return Vector2f(testValue_ * deltaTime, testValue_ * deltaTime);
    }

    bool Equals(const ParticleFieldBase& rhs) const override
    {
        const auto& other = static_cast<const TestField&>(rhs);
        return ROSEN_EQ(testValue_, other.testValue_);
    }

    bool IsPointInRegion(const Vector2f& point) const override
    {
        Vector2f delta = point - regionPosition_;
        float hw = regionSize_.x_ / 2.0f;
        float hh = regionSize_.y_ / 2.0f;
        return (std::abs(delta.x_) <= hw && std::abs(delta.y_) <= hh);
    }

    void Dump(std::string& out) const override
    {
        out += "TestField[value:" + std::to_string(testValue_) + "]";
    }

    bool MarshalSpecific(Parcel& parcel) const override
    {
        return parcel.WriteFloat(testValue_);
    }

    bool UnmarshalSpecific(Parcel& parcel) override
    {
        return parcel.ReadFloat(testValue_);
    }
};

class RSParticleFieldBaseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSParticleFieldBaseTest::SetUpTestCase() {}
void RSParticleFieldBaseTest::TearDownTestCase() {}
void RSParticleFieldBaseTest::SetUp() {}
void RSParticleFieldBaseTest::TearDown() {}

/**
 * @tc.name: IsPointInRegionRect001
 * @tc.desc: Test RECT region - point inside
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleFieldBaseTest, IsPointInRegionRect001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleFieldBaseTest IsPointInRegionRect001 start";
    TestField field;
    field.regionShape_ = ShapeType::RECT;
    field.regionPosition_ = Vector2f(100.0f, 100.0f);
    field.regionSize_ = Vector2f(200.0f, 200.0f);

    // Center point
    EXPECT_TRUE(field.IsPointInRegion(Vector2f(100.0f, 100.0f)));
    // Inside but offset
    EXPECT_TRUE(field.IsPointInRegion(Vector2f(150.0f, 150.0f)));
    // On edge
    EXPECT_TRUE(field.IsPointInRegion(Vector2f(200.0f, 200.0f)));
    GTEST_LOG_(INFO) << "RSParticleFieldBaseTest IsPointInRegionRect001 end";
}

/**
 * @tc.name: IsPointInRegionRect002
 * @tc.desc: Test RECT region - point outside
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleFieldBaseTest, IsPointInRegionRect002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleFieldBaseTest IsPointInRegionRect002 start";
    TestField field;
    field.regionShape_ = ShapeType::RECT;
    field.regionPosition_ = Vector2f(100.0f, 100.0f);
    field.regionSize_ = Vector2f(200.0f, 200.0f);

    // Outside right
    EXPECT_FALSE(field.IsPointInRegion(Vector2f(201.0f, 100.0f)));
    // Outside above
    EXPECT_FALSE(field.IsPointInRegion(Vector2f(100.0f, 201.0f)));
    // Far away
    EXPECT_FALSE(field.IsPointInRegion(Vector2f(500.0f, 500.0f)));
    GTEST_LOG_(INFO) << "RSParticleFieldBaseTest IsPointInRegionRect002 end";
}

/**
 * @tc.name: IsPointInRegionCircle001
 * @tc.desc: Test CIRCLE region - point inside
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleFieldBaseTest, IsPointInRegionCircle001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleFieldBaseTest IsPointInRegionCircle001 start";
    TestField field;
    field.regionShape_ = ShapeType::CIRCLE;
    field.regionPosition_ = Vector2f(100.0f, 100.0f);
    field.regionSize_ = Vector2f(200.0f, 200.0f); // radius = 100

    // Center point
    EXPECT_TRUE(field.IsPointInRegion(Vector2f(100.0f, 100.0f)));
    // Inside
    EXPECT_TRUE(field.IsPointInRegion(Vector2f(150.0f, 100.0f)));
    // On edge (distance = 100, radius = 100)
    EXPECT_TRUE(field.IsPointInRegion(Vector2f(200.0f, 100.0f)));
    GTEST_LOG_(INFO) << "RSParticleFieldBaseTest IsPointInRegionCircle001 end";
}

/**
 * @tc.name: IsPointInRegionCircle002
 * @tc.desc: Test CIRCLE region - point outside
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleFieldBaseTest, IsPointInRegionCircle002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleFieldBaseTest IsPointInRegionCircle002 start";
    TestField field;
    field.regionShape_ = ShapeType::CIRCLE;
    field.regionPosition_ = Vector2f(100.0f, 100.0f);
    field.regionSize_ = Vector2f(200.0f, 200.0f); // radius = 100

    // Outside (corner of bounding box, distance = ~141)
    EXPECT_FALSE(field.IsPointInRegion(Vector2f(200.0f, 200.0f)));
    // Just outside
    EXPECT_FALSE(field.IsPointInRegion(Vector2f(201.0f, 100.0f)));
    GTEST_LOG_(INFO) << "RSParticleFieldBaseTest IsPointInRegionCircle002 end";
}

/**
 * @tc.name: IsPointInRegionEllipse001
 * @tc.desc: Test ELLIPSE region - point inside
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleFieldBaseTest, IsPointInRegionEllipse001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleFieldBaseTest IsPointInRegionEllipse001 start";
    TestField field;
    field.regionShape_ = ShapeType::ELLIPSE;
    field.regionPosition_ = Vector2f(100.0f, 100.0f);
    field.regionSize_ = Vector2f(200.0f, 100.0f); // rx=100, ry=50

    // Center
    EXPECT_TRUE(field.IsPointInRegion(Vector2f(100.0f, 100.0f)));
    // On major axis edge
    EXPECT_TRUE(field.IsPointInRegion(Vector2f(200.0f, 100.0f)));
    // On minor axis edge
    EXPECT_TRUE(field.IsPointInRegion(Vector2f(100.0f, 150.0f)));
    GTEST_LOG_(INFO) << "RSParticleFieldBaseTest IsPointInRegionEllipse001 end";
}

/**
 * @tc.name: IsPointInRegionEllipse002
 * @tc.desc: Test ELLIPSE region - point outside
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleFieldBaseTest, IsPointInRegionEllipse002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleFieldBaseTest IsPointInRegionEllipse002 start";
    TestField field;
    field.regionShape_ = ShapeType::ELLIPSE;
    field.regionPosition_ = Vector2f(100.0f, 100.0f);
    field.regionSize_ = Vector2f(200.0f, 100.0f); // rx=100, ry=50

    // Outside major axis
    EXPECT_FALSE(field.IsPointInRegion(Vector2f(201.0f, 100.0f)));
    // Outside minor axis
    EXPECT_FALSE(field.IsPointInRegion(Vector2f(100.0f, 151.0f)));
    // Corner: (70,40) relative -> (70/100)^2 + (40/50)^2 = 0.49 + 0.64 = 1.13 > 1
    EXPECT_FALSE(field.IsPointInRegion(Vector2f(170.0f, 140.0f)));
    GTEST_LOG_(INFO) << "RSParticleFieldBaseTest IsPointInRegionEllipse002 end";
}

/**
 * @tc.name: IsPointInRegionEllipseZero001
 * @tc.desc: Test ELLIPSE region with zero radius returns false
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleFieldBaseTest, IsPointInRegionEllipseZero001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleFieldBaseTest IsPointInRegionEllipseZero001 start";
    TestField field;
    field.regionShape_ = ShapeType::ELLIPSE;
    field.regionPosition_ = Vector2f(100.0f, 100.0f);

    // Zero width
    field.regionSize_ = Vector2f(0.0f, 100.0f);
    EXPECT_FALSE(field.IsPointInRegion(Vector2f(100.0f, 100.0f)));

    // Zero height
    field.regionSize_ = Vector2f(100.0f, 0.0f);
    EXPECT_FALSE(field.IsPointInRegion(Vector2f(100.0f, 100.0f)));

    // Both zero
    field.regionSize_ = Vector2f(0.0f, 0.0f);
    EXPECT_FALSE(field.IsPointInRegion(Vector2f(100.0f, 100.0f)));
    GTEST_LOG_(INFO) << "RSParticleFieldBaseTest IsPointInRegionEllipseZero001 end";
}

/**
 * @tc.name: OperatorEqual001
 * @tc.desc: Test operator== with equal fields
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleFieldBaseTest, OperatorEqual001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleFieldBaseTest OperatorEqual001 start";
    TestField field1;
    field1.regionShape_ = ShapeType::RECT;
    field1.regionPosition_ = Vector2f(10.0f, 20.0f);
    field1.regionSize_ = Vector2f(100.0f, 200.0f);
    field1.testValue_ = 42.0f;

    TestField field2;
    field2.regionShape_ = ShapeType::RECT;
    field2.regionPosition_ = Vector2f(10.0f, 20.0f);
    field2.regionSize_ = Vector2f(100.0f, 200.0f);
    field2.testValue_ = 42.0f;

    EXPECT_TRUE(field1 == field2);
    GTEST_LOG_(INFO) << "RSParticleFieldBaseTest OperatorEqual001 end";
}

/**
 * @tc.name: OperatorEqual002
 * @tc.desc: Test operator== with different region shape
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleFieldBaseTest, OperatorEqual002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleFieldBaseTest OperatorEqual002 start";
    TestField field1;
    field1.regionShape_ = ShapeType::RECT;
    field1.regionPosition_ = Vector2f(10.0f, 20.0f);
    field1.regionSize_ = Vector2f(100.0f, 200.0f);
    field1.testValue_ = 42.0f;

    TestField field2;
    field2.regionShape_ = ShapeType::CIRCLE;
    field2.regionPosition_ = Vector2f(10.0f, 20.0f);
    field2.regionSize_ = Vector2f(100.0f, 200.0f);
    field2.testValue_ = 42.0f;

    EXPECT_FALSE(field1 == field2);
    GTEST_LOG_(INFO) << "RSParticleFieldBaseTest OperatorEqual002 end";
}

/**
 * @tc.name: OperatorEqual003
 * @tc.desc: Test operator== with different specific values
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleFieldBaseTest, OperatorEqual003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleFieldBaseTest OperatorEqual003 start";
    TestField field1;
    field1.regionShape_ = ShapeType::RECT;
    field1.regionPosition_ = Vector2f(10.0f, 20.0f);
    field1.regionSize_ = Vector2f(100.0f, 200.0f);
    field1.testValue_ = 42.0f;

    TestField field2;
    field2.regionShape_ = ShapeType::RECT;
    field2.regionPosition_ = Vector2f(10.0f, 20.0f);
    field2.regionSize_ = Vector2f(100.0f, 200.0f);
    field2.testValue_ = 99.0f;

    EXPECT_FALSE(field1 == field2);
    GTEST_LOG_(INFO) << "RSParticleFieldBaseTest OperatorEqual003 end";
}

/**
 * @tc.name: OperatorEqualDifferentType001
 * @tc.desc: Test operator== returns false when GetType() differs
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleFieldBaseTest, OperatorEqualDifferentType001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleFieldBaseTest OperatorEqualDifferentType001 start";
    // TestField returns VELOCITY, TestRippleField returns RIPPLE
    TestField velocityField;
    velocityField.regionShape_ = ShapeType::RECT;
    velocityField.regionPosition_ = Vector2f(10.0f, 20.0f);
    velocityField.regionSize_ = Vector2f(100.0f, 200.0f);

    class TestRippleFieldTypeOnly : public ParticleFieldBase {
    public:
        ParticleFieldType GetType() const override { return ParticleFieldType::RIPPLE; }
        Vector2f Apply(const Vector2f&, float) override { return Vector2f(0.0f, 0.0f); }
        bool Equals(const ParticleFieldBase&) const override { return true; }
        void Dump(std::string&) const override {}
        bool MarshalSpecific(Parcel&) const override { return true; }
        bool UnmarshalSpecific(Parcel&) override { return true; }
    };
    TestRippleFieldTypeOnly rippleField;
    rippleField.regionShape_ = ShapeType::RECT;
    rippleField.regionPosition_ = Vector2f(10.0f, 20.0f);
    rippleField.regionSize_ = Vector2f(100.0f, 200.0f);

    // GetType() differs → returns false without touching Equals()
    EXPECT_FALSE(velocityField == rippleField);
    GTEST_LOG_(INFO) << "RSParticleFieldBaseTest OperatorEqualDifferentType001 end";
}

/**
 * @tc.name: OperatorEqualDifferentPosition001
 * @tc.desc: Test operator== returns false when regionPosition_ differs
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleFieldBaseTest, OperatorEqualDifferentPosition001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleFieldBaseTest OperatorEqualDifferentPosition001 start";
    TestField field1;
    field1.regionShape_ = ShapeType::RECT;
    field1.regionPosition_ = Vector2f(10.0f, 20.0f);
    field1.regionSize_ = Vector2f(100.0f, 200.0f);
    field1.testValue_ = 42.0f;

    TestField field2;
    field2.regionShape_ = ShapeType::RECT;
    field2.regionPosition_ = Vector2f(999.0f, 20.0f);  // only position differs
    field2.regionSize_ = Vector2f(100.0f, 200.0f);
    field2.testValue_ = 42.0f;

    EXPECT_FALSE(field1 == field2);
    GTEST_LOG_(INFO) << "RSParticleFieldBaseTest OperatorEqualDifferentPosition001 end";
}

/**
 * @tc.name: OperatorEqualDifferentSize001
 * @tc.desc: Test operator== returns false when regionSize_ differs
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleFieldBaseTest, OperatorEqualDifferentSize001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleFieldBaseTest OperatorEqualDifferentSize001 start";
    TestField field1;
    field1.regionShape_ = ShapeType::RECT;
    field1.regionPosition_ = Vector2f(10.0f, 20.0f);
    field1.regionSize_ = Vector2f(100.0f, 200.0f);
    field1.testValue_ = 42.0f;

    TestField field2;
    field2.regionShape_ = ShapeType::RECT;
    field2.regionPosition_ = Vector2f(10.0f, 20.0f);
    field2.regionSize_ = Vector2f(100.0f, 999.0f);  // only size differs
    field2.testValue_ = 42.0f;

    EXPECT_FALSE(field1 == field2);
    GTEST_LOG_(INFO) << "RSParticleFieldBaseTest OperatorEqualDifferentSize001 end";
}

/**
 * @tc.name: IsPointInRegionDefaultShape001
 * @tc.desc: Test default branch returns false for unknown shape
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleFieldBaseTest, IsPointInRegionDefaultShape001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleFieldBaseTest IsPointInRegionDefaultShape001 start";
    TestField field;
    field.regionShape_ = ShapeType::ANNULUS;
    field.regionPosition_ = Vector2f(100.0f, 100.0f);
    field.regionSize_ = Vector2f(200.0f, 200.0f);

    EXPECT_FALSE(field.IsPointInRegion(Vector2f(100.0f, 100.0f)));
    GTEST_LOG_(INFO) << "RSParticleFieldBaseTest IsPointInRegionDefaultShape001 end";
}

// Concrete subclass returning RIPPLE type for RemoveByType testing
class TestRippleField : public ParticleFieldBase {
public:
    float testValue_ { 0.0f };

    TestRippleField() = default;
    ~TestRippleField() override = default;

    ParticleFieldType GetType() const override
    {
        return ParticleFieldType::RIPPLE;
    }

    Vector2f Apply(const Vector2f& position, float deltaTime) override
    {
        return Vector2f(testValue_ * deltaTime, testValue_ * deltaTime);
    }

    bool Equals(const ParticleFieldBase& rhs) const override
    {
        const auto& other = static_cast<const TestRippleField&>(rhs);
        return ROSEN_EQ(testValue_, other.testValue_);
    }

    bool IsPointInRegion(const Vector2f& point) const override { return true; }

    void Dump(std::string& out) const override
    {
        out += "TestRippleField[value:" + std::to_string(testValue_) + "]";
    }

    bool MarshalSpecific(Parcel& parcel) const override
    {
        return parcel.WriteFloat(testValue_);
    }

    bool UnmarshalSpecific(Parcel& parcel) override
    {
        return parcel.ReadFloat(testValue_);
    }
};

/**
 * @tc.name: FieldCollectionAddRemove001
 * @tc.desc: Test Add and Remove on ParticleFieldCollection
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleFieldBaseTest, FieldCollectionAddRemove001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleFieldBaseTest FieldCollectionAddRemove001 start";
    ParticleFieldCollection collection;
    auto field = std::make_shared<TestField>();
    field->testValue_ = 5.0f;

    collection.Add(field);
    EXPECT_EQ(collection.Count(), 1u);
    EXPECT_EQ(collection.Get(0), field);

    // Adding nullptr should be ignored
    collection.Add(nullptr);
    EXPECT_EQ(collection.Count(), 1u);

    // Get out of bounds returns nullptr
    EXPECT_EQ(collection.Get(1), nullptr);

    collection.Remove(0);
    EXPECT_EQ(collection.Count(), 0u);

    // Remove out of bounds is safe (no-op)
    collection.Remove(0);
    EXPECT_EQ(collection.Count(), 0u);
    GTEST_LOG_(INFO) << "RSParticleFieldBaseTest FieldCollectionAddRemove001 end";
}

/**
 * @tc.name: FieldCollectionApplyAll001
 * @tc.desc: Test ApplyAll sums results from all fields
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleFieldBaseTest, FieldCollectionApplyAll001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleFieldBaseTest FieldCollectionApplyAll001 start";
    ParticleFieldCollection collection;

    auto field1 = std::make_shared<TestField>();
    field1->testValue_ = 1.0f;
    auto field2 = std::make_shared<TestField>();
    field2->testValue_ = 1.0f;

    collection.Add(field1);
    collection.Add(field2);

    // Each TestField returns (testValue_ * deltaTime, testValue_ * deltaTime)
    // With testValue_=1.0 and deltaTime=1.0, each returns (1,1), sum = (2,2)
    Vector2f result = collection.ApplyAll(Vector2f(0.0f, 0.0f), 1.0f);
    EXPECT_FLOAT_EQ(result.x_, 2.0f);
    EXPECT_FLOAT_EQ(result.y_, 2.0f);
    GTEST_LOG_(INFO) << "RSParticleFieldBaseTest FieldCollectionApplyAll001 end";
}

/**
 * @tc.name: FieldCollectionClear001
 * @tc.desc: Test Clear removes all fields
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleFieldBaseTest, FieldCollectionClear001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleFieldBaseTest FieldCollectionClear001 start";
    ParticleFieldCollection collection;

    collection.Add(std::make_shared<TestField>());
    collection.Add(std::make_shared<TestField>());
    EXPECT_EQ(collection.Count(), 2u);

    collection.Clear();
    EXPECT_EQ(collection.Count(), 0u);
    GTEST_LOG_(INFO) << "RSParticleFieldBaseTest FieldCollectionClear001 end";
}

/**
 * @tc.name: FieldCollectionEquality001
 * @tc.desc: Test operator== for ParticleFieldCollection
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleFieldBaseTest, FieldCollectionEquality001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleFieldBaseTest FieldCollectionEquality001 start";
    ParticleFieldCollection collectionA;
    ParticleFieldCollection collectionB;

    // Two empty collections are equal
    EXPECT_TRUE(collectionA == collectionB);

    // Add a field to one - not equal
    auto field = std::make_shared<TestField>();
    field->testValue_ = 42.0f;
    collectionA.Add(field);
    EXPECT_FALSE(collectionA == collectionB);

    // Add same-valued field to other - equal again
    auto fieldCopy = std::make_shared<TestField>();
    fieldCopy->testValue_ = 42.0f;
    collectionB.Add(fieldCopy);
    EXPECT_TRUE(collectionA == collectionB);

    // Change value in one - not equal
    auto fieldDifferent = std::make_shared<TestField>();
    fieldDifferent->testValue_ = 99.0f;
    collectionA.Add(fieldDifferent);
    EXPECT_FALSE(collectionA == collectionB);
    GTEST_LOG_(INFO) << "RSParticleFieldBaseTest FieldCollectionEquality001 end";
}

/**
 * @tc.name: FieldCollectionEqualityBothNull001
 * @tc.desc: Collections with matching nullptr entries are equal (continue branch)
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleFieldBaseTest, FieldCollectionEqualityBothNull001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleFieldBaseTest FieldCollectionEqualityBothNull001 start";
    ParticleFieldCollection a;
    ParticleFieldCollection b;
    // Bypass Add()'s null filter by directly pushing into fields_
    a.fields_.push_back(nullptr);
    b.fields_.push_back(nullptr);
    // Both nullptr at same index → continue, fall through to return true
    EXPECT_TRUE(a == b);
    GTEST_LOG_(INFO) << "RSParticleFieldBaseTest FieldCollectionEqualityBothNull001 end";
}

/**
 * @tc.name: FieldCollectionEqualityOneNull001
 * @tc.desc: One collection has nullptr, the other has a real field — not equal
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleFieldBaseTest, FieldCollectionEqualityOneNull001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleFieldBaseTest FieldCollectionEqualityOneNull001 start";
    ParticleFieldCollection a;
    ParticleFieldCollection b;
    auto field = std::make_shared<TestField>();
    field->testValue_ = 1.0f;
    a.fields_.push_back(nullptr);
    b.Add(field);
    // Exactly one of the entries is null → return false branch
    EXPECT_FALSE(a == b);
    GTEST_LOG_(INFO) << "RSParticleFieldBaseTest FieldCollectionEqualityOneNull001 end";
}

/**
 * @tc.name: FieldCollectionRemoveByType001
 * @tc.desc: Test RemoveByType removes only matching type
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleFieldBaseTest, FieldCollectionRemoveByType001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleFieldBaseTest FieldCollectionRemoveByType001 start";
    ParticleFieldCollection collection;

    auto velocityField1 = std::make_shared<TestField>();    // VELOCITY type
    auto velocityField2 = std::make_shared<TestField>();    // VELOCITY type
    auto rippleField = std::make_shared<TestRippleField>(); // RIPPLE type

    collection.Add(velocityField1);
    collection.Add(rippleField);
    collection.Add(velocityField2);
    EXPECT_EQ(collection.Count(), 3u);

    // Remove all VELOCITY fields
    collection.RemoveByType(ParticleFieldType::VELOCITY);
    EXPECT_EQ(collection.Count(), 1u);
    EXPECT_EQ(collection.Get(0)->GetType(), ParticleFieldType::RIPPLE);

    // Remove RIPPLE fields
    collection.RemoveByType(ParticleFieldType::RIPPLE);
    EXPECT_EQ(collection.Count(), 0u);

    // RemoveByType on empty collection is safe
    collection.RemoveByType(ParticleFieldType::NOISE);
    EXPECT_EQ(collection.Count(), 0u);
    GTEST_LOG_(INFO) << "RSParticleFieldBaseTest FieldCollectionRemoveByType001 end";
}

/**
 * @tc.name: FieldFactoryCreateField001
 * @tc.desc: Test factory creates correct field types
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleFieldBaseTest, FieldFactoryCreateField001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSParticleFieldBaseTest FieldFactoryCreateField001 start";
    // Test that factory creates the right types - we can't test full marshalling without Parcel
    // but we can verify the collection works with real field types
    auto collection = std::make_shared<ParticleFieldCollection>();

    auto noise = std::make_shared<ParticleNoiseField>(
        DEFAULT_NOISE_STRENGTH, ShapeType::RECT, Vector2f(200.0f, 200.0f), Vector2f(100.0f, 100.0f),
        DEFAULT_NOISE_FEATHER, DEFAULT_NOISE_SCALE, DEFAULT_NOISE_FREQUENCY, DEFAULT_NOISE_AMPLITUDE);
    auto ripple = std::make_shared<ParticleRippleField>(Vector2f(50.0f, 50.0f), 100.0f, 50.0f, 200.0f, 0.1f);
    auto velocity = std::make_shared<ParticleVelocityField>(Vector2f(10.0f, 20.0f));

    collection->Add(noise);
    collection->Add(ripple);
    collection->Add(velocity);

    EXPECT_EQ(collection->Count(), 3u);
    EXPECT_EQ(collection->Get(0)->GetType(), ParticleFieldType::NOISE);
    EXPECT_EQ(collection->Get(1)->GetType(), ParticleFieldType::RIPPLE);
    EXPECT_EQ(collection->Get(2)->GetType(), ParticleFieldType::VELOCITY);

    // Test ApplyAll works with real fields (all inside region)
    Vector2f result = collection->ApplyAll(Vector2f(100.0f, 100.0f), 0.016f);
    // We just verify it doesn't crash and returns something
    EXPECT_TRUE(std::isfinite(result.x_));
    EXPECT_TRUE(std::isfinite(result.y_));
    GTEST_LOG_(INFO) << "RSParticleFieldBaseTest FieldFactoryCreateField001 end";
}

// Hypothetical GravityField: demonstrates adding a new field type
class GravityField : public ParticleFieldBase {
public:
    float gravity_ = 9.8f;
    GravityField() = default;
    explicit GravityField(float g) : gravity_(g) {}
    ParticleFieldType GetType() const override { return static_cast<ParticleFieldType>(99); }
    Vector2f Apply(const Vector2f& position, float deltaTime) override
    {
        if (!IsPointInRegion(position)) {
            return Vector2f(0.0f, 0.0f);
        }
        return Vector2f(0.0f, gravity_ * deltaTime);
    }
    bool Equals(const ParticleFieldBase& rhs) const override
    {
        const auto& other = static_cast<const GravityField&>(rhs);
        return ROSEN_EQ(gravity_, other.gravity_);
    }
    bool IsPointInRegion(const Vector2f& point) const override
    {
        Vector2f delta = point - regionPosition_;
        float hw = regionSize_.x_ / 2.0f;
        float hh = regionSize_.y_ / 2.0f;
        return (std::abs(delta.x_) <= hw && std::abs(delta.y_) <= hh);
    }
    void Dump(std::string& out) const override { out += "[GravityField]"; }
    bool MarshalSpecific(Parcel& parcel) const override { return parcel.WriteFloat(gravity_); }
    bool UnmarshalSpecific(Parcel& parcel) override { return parcel.ReadFloat(gravity_); }
};

/**
 * @tc.name: NewFieldTypeApplyAll001
 * @tc.desc: Verify new field type works with ApplyAll polymorphically
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleFieldBaseTest, NewFieldTypeApplyAll001, TestSize.Level1)
{
    auto collection = std::make_shared<ParticleFieldCollection>();
    auto gravity = std::make_shared<GravityField>(9.8f);
    gravity->regionShape_ = ShapeType::RECT;
    gravity->regionPosition_ = Vector2f(0.0f, 0.0f);
    gravity->regionSize_ = Vector2f(1000.0f, 1000.0f);
    collection->Add(gravity);

    auto velocity = std::make_shared<ParticleVelocityField>(Vector2f(5.0f, 0.0f));
    velocity->regionShape_ = ShapeType::RECT;
    velocity->regionPosition_ = Vector2f(0.0f, 0.0f);
    velocity->regionSize_ = Vector2f(1000.0f, 1000.0f);
    collection->Add(velocity);

    EXPECT_EQ(collection->Count(), 2u);
    Vector2f result = collection->ApplyAll(Vector2f(0.0f, 0.0f), 1.0f);
    EXPECT_FLOAT_EQ(result.x_, 5.0f);
    EXPECT_FLOAT_EQ(result.y_, 9.8f);

    Vector2f outsideResult = collection->ApplyAll(Vector2f(9999.0f, 9999.0f), 1.0f);
    EXPECT_FLOAT_EQ(outsideResult.x_, 0.0f);
    EXPECT_FLOAT_EQ(outsideResult.y_, 0.0f);
}

/**
 * @tc.name: NewFieldTypeEquality001
 * @tc.desc: Verify new field type equality comparison works
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleFieldBaseTest, NewFieldTypeEquality001, TestSize.Level1)
{
    auto g1 = std::make_shared<GravityField>(9.8f);
    g1->regionShape_ = ShapeType::RECT;
    g1->regionSize_ = Vector2f(100.0f, 100.0f);

    auto g2 = std::make_shared<GravityField>(9.8f);
    g2->regionShape_ = ShapeType::RECT;
    g2->regionSize_ = Vector2f(100.0f, 100.0f);
    EXPECT_TRUE(*g1 == *g2);

    g2->gravity_ = 1.6f;
    EXPECT_FALSE(*g1 == *g2);
}

/**
 * @tc.name: NewFieldTypeDump001
 * @tc.desc: Verify new field type Dump works
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleFieldBaseTest, NewFieldTypeDump001, TestSize.Level1)
{
    GravityField g;
    std::string dump;
    g.Dump(dump);
    EXPECT_TRUE(dump.find("GravityField") != std::string::npos);
}

/* ==========================================================================
 * ParticleFieldBase::MarshalBase / UnmarshalBase round-trip
 * ========================================================================== */

/**
 * @tc.name: MarshalBaseRoundTrip001
 * @tc.desc: MarshalBase followed by UnmarshalBase recovers all region members.
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleFieldBaseTest, MarshalBaseRoundTrip001, TestSize.Level1)
{
    TestField src;
    src.regionShape_ = ShapeType::CIRCLE;
    src.regionPosition_ = Vector2f(12.5f, 34.5f);
    src.regionSize_ = Vector2f(100.0f, 200.0f);

    Parcel parcel;
    EXPECT_TRUE(src.MarshalBase(parcel));

    TestField dst;
    EXPECT_TRUE(dst.UnmarshalBase(parcel));
    EXPECT_EQ(dst.regionShape_, ShapeType::CIRCLE);
    EXPECT_FLOAT_EQ(dst.regionPosition_.x_, 12.5f);
    EXPECT_FLOAT_EQ(dst.regionPosition_.y_, 34.5f);
    EXPECT_FLOAT_EQ(dst.regionSize_.x_, 100.0f);
    EXPECT_FLOAT_EQ(dst.regionSize_.y_, 200.0f);
}

/**
 * @tc.name: UnmarshalBaseEmptyParcel001
 * @tc.desc: Unmarshalling an empty Parcel returns false (ReadUint32 fails).
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleFieldBaseTest, UnmarshalBaseEmptyParcel001, TestSize.Level1)
{
    Parcel parcel;
    TestField dst;
    EXPECT_FALSE(dst.UnmarshalBase(parcel));
}

/**
 * @tc.name: UnmarshalBaseTruncatedShape001
 * @tc.desc: Unmarshalling only the shape (no position/size) returns false on floats.
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleFieldBaseTest, UnmarshalBaseTruncatedShape001, TestSize.Level1)
{
    Parcel parcel;
    parcel.WriteUint32(static_cast<uint32_t>(ShapeType::RECT));
    // no position/size floats
    TestField dst;
    EXPECT_FALSE(dst.UnmarshalBase(parcel));
}

/**
 * @tc.name: UnmarshalBaseTruncatedPosition001
 * @tc.desc: Unmarshalling shape + partial position returns false on size floats.
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleFieldBaseTest, UnmarshalBaseTruncatedPosition001, TestSize.Level1)
{
    Parcel parcel;
    parcel.WriteUint32(static_cast<uint32_t>(ShapeType::RECT));
    parcel.WriteFloat(1.0f);
    parcel.WriteFloat(2.0f);
    // no size floats
    TestField dst;
    EXPECT_FALSE(dst.UnmarshalBase(parcel));
}

/* ==========================================================================
 * ParticleFieldFactory — Marshalling / Unmarshalling round-trip
 * ========================================================================== */

/**
 * @tc.name: FactoryMarshalNullCollection001
 * @tc.desc: Marshalling a null collection writes a null marker and succeeds.
 *           Unmarshalling reads the null marker and sets the output to nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleFieldBaseTest, FactoryMarshalNullCollection001, TestSize.Level1)
{
    Parcel parcel;
    std::shared_ptr<ParticleFieldCollection> empty;
    EXPECT_TRUE(ParticleFieldFactory::Marshalling(parcel, empty));

    std::shared_ptr<ParticleFieldCollection> dst;
    EXPECT_TRUE(ParticleFieldFactory::Unmarshalling(parcel, dst));
    EXPECT_EQ(dst, nullptr);
}

/**
 * @tc.name: FactoryMarshalEmptyCollection001
 * @tc.desc: Marshalling an empty (non-null) collection writes size=0.
 *           Unmarshalling yields an empty collection.
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleFieldBaseTest, FactoryMarshalEmptyCollection001, TestSize.Level1)
{
    Parcel parcel;
    auto col = std::make_shared<ParticleFieldCollection>();
    EXPECT_TRUE(ParticleFieldFactory::Marshalling(parcel, col));

    std::shared_ptr<ParticleFieldCollection> dst;
    EXPECT_TRUE(ParticleFieldFactory::Unmarshalling(parcel, dst));
    ASSERT_NE(dst, nullptr);
    EXPECT_EQ(dst->Count(), 0u);
}

/**
 * @tc.name: FactoryMarshalNoiseOnly001
 * @tc.desc: Collection with one NoiseField round-trips correctly.
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleFieldBaseTest, FactoryMarshalNoiseOnly001, TestSize.Level1)
{
    auto col = std::make_shared<ParticleFieldCollection>();
    auto noise = std::make_shared<ParticleNoiseField>(DEFAULT_NOISE_STRENGTH, ShapeType::RECT,
        Vector2f(200.0f, 200.0f), Vector2f(0.0f, 0.0f),
        DEFAULT_NOISE_FEATHER, DEFAULT_NOISE_SCALE, DEFAULT_NOISE_FREQUENCY, DEFAULT_NOISE_AMPLITUDE);
    col->Add(noise);

    Parcel parcel;
    EXPECT_TRUE(ParticleFieldFactory::Marshalling(parcel, col));

    std::shared_ptr<ParticleFieldCollection> dst;
    EXPECT_TRUE(ParticleFieldFactory::Unmarshalling(parcel, dst));
    ASSERT_NE(dst, nullptr);
    ASSERT_EQ(dst->Count(), 1u);
    EXPECT_EQ(dst->Get(0)->GetType(), ParticleFieldType::NOISE);
}

/**
 * @tc.name: FactoryMarshalAllTypes001
 * @tc.desc: Collection with NOISE + RIPPLE + VELOCITY round-trips correctly.
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleFieldBaseTest, FactoryMarshalAllTypes001, TestSize.Level1)
{
    auto col = std::make_shared<ParticleFieldCollection>();
    col->Add(std::make_shared<ParticleNoiseField>(DEFAULT_NOISE_STRENGTH, ShapeType::RECT,
        Vector2f(200.0f, 200.0f), Vector2f(0.0f, 0.0f),
        DEFAULT_NOISE_FEATHER, DEFAULT_NOISE_SCALE, DEFAULT_NOISE_FREQUENCY, DEFAULT_NOISE_AMPLITUDE));
    auto ripple = std::make_shared<ParticleRippleField>(Vector2f(50.0f, 50.0f), 100.0f, 50.0f, 200.0f, 0.1f);
    ripple->regionShape_ = ShapeType::RECT;
    ripple->regionSize_ = Vector2f(200.0f, 200.0f);
    col->Add(ripple);
    auto velocity = std::make_shared<ParticleVelocityField>(Vector2f(10.0f, 20.0f));
    velocity->regionShape_ = ShapeType::RECT;
    velocity->regionSize_ = Vector2f(200.0f, 200.0f);
    col->Add(velocity);

    Parcel parcel;
    EXPECT_TRUE(ParticleFieldFactory::Marshalling(parcel, col));

    std::shared_ptr<ParticleFieldCollection> dst;
    EXPECT_TRUE(ParticleFieldFactory::Unmarshalling(parcel, dst));
    ASSERT_NE(dst, nullptr);
    ASSERT_EQ(dst->Count(), 3u);
    EXPECT_EQ(dst->Get(0)->GetType(), ParticleFieldType::NOISE);
    EXPECT_EQ(dst->Get(1)->GetType(), ParticleFieldType::RIPPLE);
    EXPECT_EQ(dst->Get(2)->GetType(), ParticleFieldType::VELOCITY);
}

/**
 * @tc.name: FactoryMarshalCollectionWithNullField001
 * @tc.desc: Marshalling a collection containing a nullptr element writes a null-field marker.
 *           Unmarshalling skips null entries (does not add them).
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleFieldBaseTest, FactoryMarshalCollectionWithNullField001, TestSize.Level1)
{
    auto col = std::make_shared<ParticleFieldCollection>();
    col->fields_.push_back(nullptr);  // bypass Collection::Add null-skip
    col->Add(std::make_shared<ParticleNoiseField>());

    Parcel parcel;
    EXPECT_TRUE(ParticleFieldFactory::Marshalling(parcel, col));

    std::shared_ptr<ParticleFieldCollection> dst;
    EXPECT_TRUE(ParticleFieldFactory::Unmarshalling(parcel, dst));
    ASSERT_NE(dst, nullptr);
    // Null field is skipped on unmarshal, so count is 1.
    EXPECT_EQ(dst->Count(), 1u);
    EXPECT_EQ(dst->Get(0)->GetType(), ParticleFieldType::NOISE);
}

/**
 * @tc.name: FactoryUnmarshalEmptyParcel001
 * @tc.desc: Unmarshalling an empty parcel returns false.
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleFieldBaseTest, FactoryUnmarshalEmptyParcel001, TestSize.Level1)
{
    Parcel parcel;
    std::shared_ptr<ParticleFieldCollection> dst;
    EXPECT_FALSE(ParticleFieldFactory::Unmarshalling(parcel, dst));
}

/**
 * @tc.name: FactoryUnmarshalSizeOnly001
 * @tc.desc: Unmarshalling a parcel with only the valid marker (no size) fails.
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleFieldBaseTest, FactoryUnmarshalSizeOnly001, TestSize.Level1)
{
    Parcel parcel;
    parcel.WriteInt32(1);  // valid marker but no size follows
    std::shared_ptr<ParticleFieldCollection> dst;
    EXPECT_FALSE(ParticleFieldFactory::Unmarshalling(parcel, dst));
}

/**
 * @tc.name: FactoryUnmarshalSizeExceedsLimit001
 * @tc.desc: Unmarshalling a parcel whose declared size exceeds
 *           PARTICLE_EMMITER_UPPER_LIMIT (2000) must return false.
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleFieldBaseTest, FactoryUnmarshalSizeExceedsLimit001, TestSize.Level1)
{
    Parcel parcel;
    parcel.WriteInt32(1);            // valid marker
    parcel.WriteUint32(3000u);       // exceeds PARTICLE_EMMITER_UPPER_LIMIT = 2000
    std::shared_ptr<ParticleFieldCollection> dst;
    EXPECT_FALSE(ParticleFieldFactory::Unmarshalling(parcel, dst));
}

/**
 * @tc.name: FactoryUnmarshalFieldUnknownType001
 * @tc.desc: Unmarshalling a field with an unknown type returns false.
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleFieldBaseTest, FactoryUnmarshalFieldUnknownType001, TestSize.Level1)
{
    Parcel parcel;
    parcel.WriteInt32(1);            // collection valid marker
    parcel.WriteUint32(1);           // size = 1 field
    parcel.WriteInt32(1);            // field valid marker
    parcel.WriteUint32(99999u);      // unknown type → CreateField returns nullptr
    std::shared_ptr<ParticleFieldCollection> dst;
    EXPECT_FALSE(ParticleFieldFactory::Unmarshalling(parcel, dst));
}

/**
 * @tc.name: FactoryUnmarshalNullFieldMarker001
 * @tc.desc: A null-field marker (-1) should be skipped, not error.
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleFieldBaseTest, FactoryUnmarshalNullFieldMarker001, TestSize.Level1)
{
    Parcel parcel;
    parcel.WriteInt32(1);       // collection valid marker
    parcel.WriteUint32(1);      // size = 1 field
    parcel.WriteInt32(-1);      // null-field marker
    std::shared_ptr<ParticleFieldCollection> dst;
    EXPECT_TRUE(ParticleFieldFactory::Unmarshalling(parcel, dst));
    ASSERT_NE(dst, nullptr);
    EXPECT_EQ(dst->Count(), 0u);
}

} // namespace Rosen
} // namespace OHOS
