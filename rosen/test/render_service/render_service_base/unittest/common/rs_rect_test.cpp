/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <new>
#include <limits>
#include "gtest/gtest.h"

#include "common/rs_rect.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRectTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRectTest::SetUpTestCase() {}
void RSRectTest::TearDownTestCase() {}
void RSRectTest::SetUp() {}
void RSRectTest::TearDown() {}

/**
 * @tc.name: FilterRectIComparator
 * @tc.desc: test results of FilterRectIComparator of equal rect
 * @tc.type:FUNC
 * @tc.require: issueI9LJFJ
 */
HWTEST_F(RSRectTest, FilterRectIComparator, TestSize.Level1)
{
    NodeId id = 1;
    RectI rect;
    auto p1 = std::pair<NodeId, RectI>(id, rect);
    id++;
    auto p2 = std::pair<NodeId, RectI>(id, rect);
    FilterRectIComparator comp;
    ASSERT_TRUE(comp(p1, p2));
}

/**
 * @tc.name: RectIComparator
 * @tc.desc: test results of RectIComparator of rectI with MakeOutSet
 * @tc.type:FUNC
 * @tc.require: issueI9LJFJ
 */
HWTEST_F(RSRectTest, RectIComparator, TestSize.Level1)
{
    NodeId id = 1;
    RectI rect;
    auto p1 = std::pair<NodeId, RectI>(id, rect);
    id++;
    auto p2 = std::pair<NodeId, RectI>(id, rect.MakeOutset(1));
    RectIComparator comp;
    ASSERT_FALSE(comp(p1, p2));
}

/**
 * @tc.name: RectI_Hash_Func
 * @tc.desc: test results of RectI_Hash_Func
 * @tc.type:FUNC
 * @tc.require: issueI9LJFJ
 */
HWTEST_F(RSRectTest, RectI_Hash_Func, TestSize.Level1)
{
    NodeId id = 1;
    RectI rect1(0, 0, 10, 10);
    auto p1 = std::pair<NodeId, RectI>(id, rect1);
    id++;
    RectI rect2(0, 0, 5, 5);
    auto p2 = std::pair<NodeId, RectI>(id, rect2);
    OcclusionRectISet occRectISet;
    occRectISet.insert(p1);
    occRectISet.insert(p2);
    RectIComparator comp;
    ASSERT_TRUE(comp(p1, p2));
}

/**
 * @tc.name: Filter_RectI_Hash_Func
 * @tc.desc: test results of Filter_RectI_Hash_Func of equal nodeid
 * @tc.type:FUNC
 * @tc.require: issueI9LJFJ
 */
HWTEST_F(RSRectTest, Filter_RectI_Hash_Func, TestSize.Level1)
{
    NodeId id = 1;
    RectI rect;
    auto p1 = std::pair<NodeId, RectI>(id, rect);
    auto p2 = std::pair<NodeId, RectI>(id, rect.MakeOutset(1));
    Filter_RectI_Hash_Func hashFunc;
    ASSERT_EQ(hashFunc(p1), hashFunc(p2));
}

/**
 * @tc.name: SetAll001
 * @tc.desc: test results of SetAll
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRectTest, SetAll001, TestSize.Level1)
{
    RectF rect;
    rect.SetAll(1.f, 1.f, 1.f, 1.f);
    EXPECT_EQ(rect.data_[0], 1.f);
    EXPECT_EQ(rect.data_[1], 1.f);
    EXPECT_EQ(rect.data_[2], 1.f);
    EXPECT_EQ(rect.data_[3], 1.f);
}

/**
 * @tc.name: GetRight001
 * @tc.desc: test results of GetRight
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRectTest, GetRight001, TestSize.Level1)
{
    RectF rect;
    rect.SetRight(1.f);
    EXPECT_EQ(rect.GetRight(), 1.f);
}

/**
 * @tc.name: GetLeft001
 * @tc.desc: test results of GetLeft
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRectTest, GetLeft001, TestSize.Level1)
{
    RectF rect;
    EXPECT_EQ(rect.GetLeft(), 0.f);
}

/**
 * @tc.name: GetBottom001
 * @tc.desc: test results of GetBottom
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRectTest, GetBottom001, TestSize.Level1)
{
    RectF rect;
    rect.SetBottom(1.f);
    EXPECT_EQ(rect.GetBottom(), 1.f);
}

/**
 * @tc.name: GetTop001
 * @tc.desc: test results of GetTop
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRectTest, GetTop001, TestSize.Level1)
{
    RectF rect;
    EXPECT_EQ(rect.GetTop(), 0.f);
}

/**
 * @tc.name: GetWidth001
 * @tc.desc: test results of GetWidth
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRectTest, GetWidth001, TestSize.Level1)
{
    RectF rect;
    EXPECT_EQ(rect.GetWidth(), 0.f);
}

/**
 * @tc.name: Move001
 * @tc.desc: test results of Move
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRectTest, Move001, TestSize.Level1)
{
    RectF rect;
    EXPECT_EQ(rect.GetLeft(), 0.f);
    EXPECT_EQ(rect.GetTop(), 0.f);
    rect.Move(1.f, 1.f);
    EXPECT_EQ(rect.GetLeft(), 1.f);
    EXPECT_EQ(rect.GetTop(), 1.f);
}

/**
 * @tc.name: Clear001
 * @tc.desc: test results of Clear
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRectTest, Clear001, TestSize.Level1)
{
    RectF rect;
    rect.SetAll(1.f, 1.f, 1.f, 1.f);
    EXPECT_EQ(rect.GetLeft(), 1.f);
    EXPECT_EQ(rect.GetTop(), 1.f);
    EXPECT_EQ(rect.GetHeight(), 1.f);
    EXPECT_EQ(rect.GetWidth(), 1.f);
    rect.Clear();
    EXPECT_EQ(rect.GetLeft(), 0.f);
    EXPECT_EQ(rect.GetTop(), 0.f);
    EXPECT_EQ(rect.GetHeight(), 0.f);
    EXPECT_EQ(rect.GetWidth(), 0.f);
}

/**
 * @tc.name: IsEmpty001
 * @tc.desc: test results of IsEmpty
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRectTest, IsEmpty001, TestSize.Level1)
{
    RectF rect;
    ASSERT_TRUE(rect.IsEmpty());
    rect.SetAll(1.f, 1.f, 1.f, 1.f);
    ASSERT_FALSE(rect.IsEmpty());
}

/**
 * @tc.name: Intersect001
 * @tc.desc: test results of Intersect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRectTest, Intersect001, TestSize.Level1)
{
    RectF rect1;
    RectF rect2;
    rect1.SetAll(1.f, 1.f, 1.f, 1.f);
    rect2.SetAll(0.f, 0.f, 2.f, 2.f);
    ASSERT_TRUE(rect1.Intersect(rect2));
}

/**
 * @tc.name: Intersect002
 * @tc.desc: test results of Intersect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRectTest, Intersect002, TestSize.Level1)
{
    RectF rect;
    rect.SetAll(1.f, 1.f, 1.f, 1.f);
    ASSERT_TRUE(rect.Intersect(1.5, 1.5));
}

/**
 * @tc.name: IsInsideOf001
 * @tc.desc: test results of IsInsideOf
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRectTest, IsInsideOf001, TestSize.Level1)
{
    RectF rect1;
    RectF rect2;
    rect1.SetAll(1.f, 1.f, 1.f, 1.f);
    rect2.SetAll(0.f, 0.f, 3.f, 3.f);
    ASSERT_TRUE(rect1.IsInsideOf(rect2));
}

/**
 * @tc.name: IntersectRect001
 * @tc.desc: test results of IntersectRect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRectTest, IntersectRect001, TestSize.Level1)
{
    RectF rect1;
    RectF rect2;
    rect1.SetAll(0.f, 0.f, 2.f, 2.f);
    rect2.SetAll(1.f, 1.f, 3.f, 3.f);
    RectF rect;
    EXPECT_NE(rect1.IntersectRect(rect2), rect);
}

/**
 * @tc.name: JoinRect001
 * @tc.desc: test results of JoinRect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRectTest, JoinRect001, TestSize.Level1)
{
    RectF rect1;
    RectF rect2;
    rect1.SetAll(1.f, 1.f, 1.f, 1.f);
    rect2.SetAll(0.f, 0.f, 2.f, 2.f);
    RectF rect;
    EXPECT_NE(rect1.JoinRect(rect2), rect);
}

/**
 * @tc.name: Offset001
 * @tc.desc: test results of Offset
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRectTest, Offset001, TestSize.Level1)
{
    RectF rect;
    RectF rect1;
    rect.SetAll(1.f, 1.f, 1.f, 1.f);
    EXPECT_NE(rect.Offset(1.f, 1.f), rect1);
}

/**
 * @tc.name: ToString001
 * @tc.desc: Verify function ToString
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRectTest, ToString001, TestSize.Level1)
{
    RectF rect;
    rect.SetAll(1.f, 1.f, 1.f, 1.f);
    std::string str = std::string("[") + std::to_string(rect.GetLeft()) + ", " + std::to_string(rect.GetTop())
        + ", " + std::to_string(rect.GetWidth()) + ", " + std::to_string(rect.GetHeight()) + "]";
    EXPECT_EQ(rect.ToString(), str);
}

/**
 * @tc.name: MakeOutset001
 * @tc.desc: Verify function MakeOutset
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRectTest, MakeOutset001, TestSize.Level1)
{
    RectF rect;
    RectF rect1;
    rect.SetAll(1.f, 1.f, 1.f, 1.f);
    Vector4f outset = { 1.f, 1.f, 0.f, 0.f };
    EXPECT_NE(rect.MakeOutset(outset), rect1);
}

/**
 * @tc.name: Marshalling001
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRectTest, Marshalling001, TestSize.Level1)
{
    Parcel parcel;
    RectF rect;
    rect.SetAll(1.f, 1.f, 1.f, 1.f);
    ASSERT_TRUE(rect.Marshalling(parcel));
}

/**
 * @tc.name: Unmarshalling001
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRectTest, Unmarshalling001, TestSize.Level1)
{
    Parcel parcel;
    RectF rect;
    rect.SetAll(1.f, 1.f, 1.f, 1.f);
    ASSERT_FALSE(rect.Unmarshalling(parcel));
}

/**
 * @tc.name: Inset001
 * @tc.desc: Verify function RRect Inset
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRectTest, Inset001, TestSize.Level1)
{
    RRect rrect;
    rrect.rect_.SetAll(1.f, 1.f, 10.f, 10.f); // width_ and height_ set 10.f
    Vector4f width = { 1.f };
    RRect rrect1 = rrect.Inset(width);
    EXPECT_EQ(rrect1.rect_.GetLeft(), 2.f); // 2.f is left + width.x_
    EXPECT_EQ(rrect1.rect_.GetWidth(), 8.f); // 8.f is width_ - (width.x_ + width.y_)
    EXPECT_NE(rrect1.radius_[0], rrect.radius_[0]);
}

/**
 * @tc.name: IsNaN
 * @tc.desc: test results of IsNaN
 * @tc.type: FUNC
 */
HWTEST_F(RSRectTest, IsNaN, TestSize.Level1)
{
    float nanNum = std::numeric_limits<float>::quiet_NaN();
    RectF rect1(nanNum, nanNum, nanNum, nanNum);
    EXPECT_TRUE(rect1.IsNaN());
    float normalNum = 1.0f;
    RectF rect2(normalNum, normalNum, normalNum, normalNum);
    EXPECT_FALSE(rect2.IsNaN());
}

/**
 * @tc.name: IsValid
 * @tc.desc: test results of IsValid
 * @tc.type: FUNC
 */
HWTEST_F(RSRectTest, IsValid, TestSize.Level1)
{
    float infiniteNum = std::numeric_limits<float>::infinity();
    RectF rect1(infiniteNum, infiniteNum, infiniteNum, infiniteNum);
    EXPECT_FALSE(rect1.IsValid());
    float nanNum = std::numeric_limits<float>::quiet_NaN();
    RectF rect2(nanNum, nanNum, nanNum, nanNum);
    EXPECT_FALSE(rect2.IsValid());
    float normalNum = 1.0f;
    RectF rect3(normalNum, normalNum, normalNum, normalNum);
    EXPECT_TRUE(rect3.IsValid());
}

/**
 * @tc.name: RRectOperatorMultiply001
 * @tc.desc: test results of RRectT::operator*(const RRectT&)
 * @tc.type: FUNC
 */
HWTEST_F(RSRectTest, RRectOperatorMultiply001, TestSize.Level1)
{
    RectF rect1;
    rect1.SetAll(2.0f, 3.0f, 4.0f, 5.0f);
    RRect a(rect1, 6.0f, 7.0f);

    RectF rect2;
    rect2.SetAll(10.0f, 20.0f, 30.0f, 40.0f);
    RRect b(rect2, 50.0f, 60.0f);

    RRect result = a * b;
    EXPECT_FLOAT_EQ(result.rect_.GetLeft(), 20.0f);
    EXPECT_FLOAT_EQ(result.rect_.GetTop(), 60.0f);
    EXPECT_FLOAT_EQ(result.rect_.GetWidth(), 120.0f);
    EXPECT_FLOAT_EQ(result.rect_.GetHeight(), 200.0f);
    EXPECT_FLOAT_EQ(result.radius_[0].x_, 300.0f);
    EXPECT_FLOAT_EQ(result.radius_[0].y_, 420.0f);
}

/**
 * @tc.name: RRectOperatorMultiply002
 * @tc.desc: test RRectT::operator*(const RRectT&) with zeros
 * @tc.type: FUNC
 */
HWTEST_F(RSRectTest, RRectOperatorMultiply002, TestSize.Level1)
{
    RectF zeroRect;
    zeroRect.SetAll(0.0f, 0.0f, 0.0f, 0.0f);
    RRect zero(zeroRect, 0.0f, 0.0f);

    RectF rect;
    rect.SetAll(5.0f, 6.0f, 7.0f, 8.0f);
    RRect other(rect, 9.0f, 10.0f);

    RRect result = zero * other;
    EXPECT_FLOAT_EQ(result.rect_.GetLeft(), 0.0f);
    EXPECT_FLOAT_EQ(result.rect_.GetTop(), 0.0f);
    EXPECT_FLOAT_EQ(result.rect_.GetWidth(), 0.0f);
    EXPECT_FLOAT_EQ(result.rect_.GetHeight(), 0.0f);
    EXPECT_FLOAT_EQ(result.radius_[0].x_, 0.0f);
    EXPECT_FLOAT_EQ(result.radius_[0].y_, 0.0f);
}

/**
 * @tc.name: RectIsAbsNearEqual001
 * @tc.desc: test results of RectT<float>::IsAbsNearEqual
 * @tc.type: FUNC
 */
HWTEST_F(RSRectTest, RectIsAbsNearEqual001, TestSize.Level1)
{
    RectF value(10.0f, 20.0f, 30.0f, 40.0f);
    RectF target(12.0f, 18.0f, 33.0f, 38.0f);
    // all diffs are <= 5 -> true
    RectF threshold(5.0f, 5.0f, 5.0f, 5.0f);
    EXPECT_TRUE(value.IsAbsNearEqual(target, threshold));

    // all diffs are > 1 -> false
    RectF smallThreshold(1.0f, 1.0f, 1.0f, 1.0f);
    EXPECT_FALSE(value.IsAbsNearEqual(target, smallThreshold));
}

/**
 * @tc.name: RectIsAbsNearEqual002
 * @tc.desc: test RectT<float>::IsAbsNearEqual with exact match and negative threshold
 * @tc.type: FUNC
 */
HWTEST_F(RSRectTest, RectIsAbsNearEqual002, TestSize.Level1)
{
    RectF value(10.0f, 20.0f, 30.0f, 40.0f);
    // exact match with zero threshold -> true
    RectF zeroThreshold(0.0f, 0.0f, 0.0f, 0.0f);
    EXPECT_TRUE(value.IsAbsNearEqual(value, zeroThreshold));

    // negative threshold: abs is used, so -5 behaves like 5
    RectF target(12.0f, 18.0f, 33.0f, 38.0f);
    RectF negThreshold(-5.0f, -5.0f, -5.0f, -5.0f);
    EXPECT_TRUE(value.IsAbsNearEqual(target, negThreshold));
}

/**
 * @tc.name: RectTakeAbsMaxFrom001
 * @tc.desc: test results of RectT<float>::TakeAbsMaxFrom with mixed values
 * @tc.type: FUNC
 */
HWTEST_F(RSRectTest, RectTakeAbsMaxFrom001, TestSize.Level1)
{
    RectF value(10.0f, 20.0f, 30.0f, 40.0f);
    RectF target(50.0f, 5.0f, 60.0f, 3.0f);
    value.TakeAbsMaxFrom(target);
    // left: |10| < |50| -> 50; top: |20| > |5| -> stays 20
    // width: |30| < |60| -> 60; height: |40| > |3| -> stays 40
    EXPECT_FLOAT_EQ(value.GetLeft(), 50.0f);
    EXPECT_FLOAT_EQ(value.GetTop(), 20.0f);
    EXPECT_FLOAT_EQ(value.GetWidth(), 60.0f);
    EXPECT_FLOAT_EQ(value.GetHeight(), 40.0f);
}

/**
 * @tc.name: RectTakeAbsMaxFrom002
 * @tc.desc: test RectT<float>::TakeAbsMaxFrom with all smaller target (no change)
 * @tc.type: FUNC
 */
HWTEST_F(RSRectTest, RectTakeAbsMaxFrom002, TestSize.Level1)
{
    RectF value(100.0f, 100.0f, 100.0f, 100.0f);
    RectF target(10.0f, 20.0f, 30.0f, 40.0f);
    value.TakeAbsMaxFrom(target);
    EXPECT_FLOAT_EQ(value.GetLeft(), 100.0f);
    EXPECT_FLOAT_EQ(value.GetTop(), 100.0f);
    EXPECT_FLOAT_EQ(value.GetWidth(), 100.0f);
    EXPECT_FLOAT_EQ(value.GetHeight(), 100.0f);
}

/**
 * @tc.name: RectTakeAbsMaxFrom003
 * @tc.desc: test RectT<float>::TakeAbsMaxFrom with negative values (sign preserved)
 * @tc.type: FUNC
 */
HWTEST_F(RSRectTest, RectTakeAbsMaxFrom003, TestSize.Level1)
{
    RectF value(10.0f, 10.0f, 10.0f, 10.0f);
    RectF target(-50.0f, -5.0f, -60.0f, -3.0f);
    value.TakeAbsMaxFrom(target);
    // left: |10| < |−50|=50 -> -50; top: |10| > |−5|=5 -> stays 10
    EXPECT_FLOAT_EQ(value.GetLeft(), -50.0f);
    EXPECT_FLOAT_EQ(value.GetTop(), 10.0f);
    EXPECT_FLOAT_EQ(value.GetWidth(), -60.0f);
    EXPECT_FLOAT_EQ(value.GetHeight(), 10.0f);
}

/**
 * @tc.name: RectTakeAbsMaxFrom004
 * @tc.desc: test RectT<float>::TakeAbsMaxFrom with equal magnitude (strict <, no change)
 * @tc.type: FUNC
 */
HWTEST_F(RSRectTest, RectTakeAbsMaxFrom004, TestSize.Level1)
{
    RectF value(50.0f, -50.0f, 50.0f, -50.0f);
    RectF target(-50.0f, 50.0f, -50.0f, 50.0f);
    value.TakeAbsMaxFrom(target);
    // all |50| == |−50|, strict < means no change
    EXPECT_FLOAT_EQ(value.GetLeft(), 50.0f);
    EXPECT_FLOAT_EQ(value.GetTop(), -50.0f);
    EXPECT_FLOAT_EQ(value.GetWidth(), 50.0f);
    EXPECT_FLOAT_EQ(value.GetHeight(), -50.0f);
}

/**
 * @tc.name: RRectIsAbsNearEqual001
 * @tc.desc: test results of RRectT<float>::IsAbsNearEqual
 * @tc.type: FUNC
 */
HWTEST_F(RSRectTest, RRectIsAbsNearEqual001, TestSize.Level1)
{
    RectF rect1(10.0f, 20.0f, 30.0f, 40.0f);
    RRect value(rect1, 5.0f, 6.0f);
    RectF rect2(12.0f, 18.0f, 33.0f, 38.0f);
    RRect target(rect2, 7.0f, 3.0f);
    RectF rect3(5.0f, 5.0f, 5.0f, 5.0f);
    RRect threshold(rect3, 5.0f, 5.0f);
    // all rect diffs <= 5, all radius diffs <= 5 -> true
    EXPECT_TRUE(value.IsAbsNearEqual(target, threshold));

    RectF rect4(1.0f, 1.0f, 1.0f, 1.0f);
    RRect smallThreshold(rect4, 1.0f, 1.0f);
    // radius diff |5-7|=2 > 1 -> false
    EXPECT_FALSE(value.IsAbsNearEqual(target, smallThreshold));
}

/**
 * @tc.name: RRectIsAbsNearEqual002
 * @tc.desc: test RRectT<float>::IsAbsNearEqual with exact match
 * @tc.type: FUNC
 */
HWTEST_F(RSRectTest, RRectIsAbsNearEqual002, TestSize.Level1)
{
    RectF rect(10.0f, 20.0f, 30.0f, 40.0f);
    RRect value(rect, 5.0f, 6.0f);
    RectF zeroRect(0.0f, 0.0f, 0.0f, 0.0f);
    RRect zeroThreshold(zeroRect, 0.0f, 0.0f);
    // exact match with zero threshold -> true
    EXPECT_TRUE(value.IsAbsNearEqual(value, zeroThreshold));
}

/**
 * @tc.name: RRectTakeAbsMaxFrom001
 * @tc.desc: test results of RRectT<float>::TakeAbsMaxFrom with mixed values
 * @tc.type: FUNC
 */
HWTEST_F(RSRectTest, RRectTakeAbsMaxFrom001, TestSize.Level1)
{
    RectF rect1(10.0f, 20.0f, 30.0f, 40.0f);
    RRect value(rect1, 5.0f, 10.0f);
    RectF rect2(100.0f, 5.0f, 110.0f, 3.0f);
    RRect target(rect2, 50.0f, 3.0f);
    value.TakeAbsMaxFrom(target);
    // rect: left |10|<|100|->100, top |20|>|5|->20, width |30|<|110|->110, height |40|>|3|->40
    EXPECT_FLOAT_EQ(value.rect_.GetLeft(), 100.0f);
    EXPECT_FLOAT_EQ(value.rect_.GetTop(), 20.0f);
    EXPECT_FLOAT_EQ(value.rect_.GetWidth(), 110.0f);
    EXPECT_FLOAT_EQ(value.rect_.GetHeight(), 40.0f);
    // radius: x |5|<|50|->50, y |10|>|3|->10
    for (int i = 0; i < 4; i++) {
        EXPECT_FLOAT_EQ(value.radius_[i].x_, 50.0f);
        EXPECT_FLOAT_EQ(value.radius_[i].y_, 10.0f);
    }
}

/**
 * @tc.name: RRectTakeAbsMaxFrom002
 * @tc.desc: test RRectT<float>::TakeAbsMaxFrom with all smaller target (no change)
 * @tc.type: FUNC
 */
HWTEST_F(RSRectTest, RRectTakeAbsMaxFrom002, TestSize.Level1)
{
    RectF rect1(100.0f, 100.0f, 100.0f, 100.0f);
    RRect value(rect1, 50.0f, 60.0f);
    RectF rect2(10.0f, 20.0f, 30.0f, 40.0f);
    RRect target(rect2, 5.0f, 3.0f);
    value.TakeAbsMaxFrom(target);
    // all target abs values are smaller -> no change
    EXPECT_FLOAT_EQ(value.rect_.GetLeft(), 100.0f);
    EXPECT_FLOAT_EQ(value.rect_.GetTop(), 100.0f);
    EXPECT_FLOAT_EQ(value.rect_.GetWidth(), 100.0f);
    EXPECT_FLOAT_EQ(value.rect_.GetHeight(), 100.0f);
    for (int i = 0; i < 4; i++) {
        EXPECT_FLOAT_EQ(value.radius_[i].x_, 50.0f);
        EXPECT_FLOAT_EQ(value.radius_[i].y_, 60.0f);
    }
}
} // namespace OHOS::Rosen