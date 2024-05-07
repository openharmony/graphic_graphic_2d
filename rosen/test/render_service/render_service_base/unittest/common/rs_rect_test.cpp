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
    std::string str = std::string("(") + std::to_string(rect.GetLeft()) + ", " + std::to_string(rect.GetTop())
        + ", " + std::to_string(rect.GetWidth()) + ", " + std::to_string(rect.GetHeight()) + ")";
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
} // namespace OHOS::Rosen