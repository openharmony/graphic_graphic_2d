/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gtest/gtest.h"

#include "utils/region.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class RegionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RegionTest::SetUpTestCase() {}
void RegionTest::TearDownTestCase() {}
void RegionTest::SetUp() {}
void RegionTest::TearDown() {}

/**
 * @tc.name: CreateRegion001
 * @tc.desc: test for creating Regin.
 * @tc.type: FUNC
 * @tc.require: I766AZ
 */
HWTEST_F(RegionTest, CreateRegion001, TestSize.Level1)
{
    std::unique_ptr<Region> region = std::make_unique<Region>();
    ASSERT_TRUE(region != nullptr);
}

/**
 * @tc.name: SetRectTest001
 * @tc.desc: test for constructs a rectangular Region matching the bounds of rect.
 * @tc.type: FUNC
 * @tc.require: I766AZ
 */
HWTEST_F(RegionTest, SetRectTest001, TestSize.Level1)
{
    std::unique_ptr<Region> region = std::make_unique<Region>();
    ASSERT_TRUE(region != nullptr);
    RectI rectI;
    EXPECT_FALSE(region->SetRect(rectI));
}

/**
 * @tc.name: SetRectTest002
 * @tc.desc: test for constructs a rectangular Region matching the bounds of rect.
 * @tc.type: FUNC
 * @tc.require: I766AZ
 */
HWTEST_F(RegionTest, SetRectTest002, TestSize.Level1)
{
    std::unique_ptr<Region> region = std::make_unique<Region>();
    ASSERT_TRUE(region != nullptr);
    RectI rectI(0, 0, 256, 256);
    EXPECT_TRUE(region->SetRect(rectI));
}

/**
 * @tc.name: SetPathTest001
 * @tc.desc: test for constructs Region to match outline of path within clip.
 * @tc.type: FUNC
 * @tc.require: I766AZ
 */
HWTEST_F(RegionTest, SetPathTest001, TestSize.Level1)
{
    std::unique_ptr<Region> region = std::make_unique<Region>();
    ASSERT_TRUE(region != nullptr);
    Path path;
    Region clip;
    EXPECT_FALSE(region->SetPath(path, clip));
}

/**
 * @tc.name: SetPathTest002
 * @tc.desc: test for constructs Region to match outline of path within clip.
 * @tc.type: FUNC
 * @tc.require: I766AZ
 */
HWTEST_F(RegionTest, SetPathTest002, TestSize.Level1)
{
    std::unique_ptr<Region> region = std::make_unique<Region>();
    ASSERT_TRUE(region != nullptr);
    Path path;
    path.AddRect(1.0f, 4.0f, 3.0f, 2.0f, PathDirection::CCW_DIRECTION);
    Region clip;
    EXPECT_FALSE(region->SetPath(path, clip));
}

/**
 * @tc.name: IsIntersectsTest001
 * @tc.desc: test for IsIntersects function
 * @tc.type: FUNC
 * @tc.require: I766AZ
 */
HWTEST_F(RegionTest, IsIntersectsTest001, TestSize.Level1)
{
    std::unique_ptr<Region> region = std::make_unique<Region>();
    ASSERT_TRUE(region != nullptr);
    Region other;
    EXPECT_FALSE(region->IsIntersects(other));
}

/**
 * @tc.name: IsIntersectsTest002
 * @tc.desc: test for IsIntersects function
 * @tc.type: FUNC
 * @tc.require: I766AZ
 */
HWTEST_F(RegionTest, IsIntersectsTest002, TestSize.Level1)
{
    std::unique_ptr<Region> region = std::make_unique<Region>();
    ASSERT_TRUE(region != nullptr);
    RectI rectI(0, 0, 256, 256);
    region->SetRect(rectI);
    Region other;
    other.SetRect(rectI);
    EXPECT_TRUE(region->IsIntersects(other));
}

/**
 * @tc.name: OpTest001
 * @tc.desc: test for OP function with DIFFERENCE option
 * @tc.type: FUNC
 * @tc.require: I766AZ
 */
HWTEST_F(RegionTest, OpTest001, TestSize.Level1)
{
    std::unique_ptr<Region> region = std::make_unique<Region>();
    ASSERT_TRUE(region != nullptr);
    Region other;
    EXPECT_FALSE(region->Op(other, RegionOp::DIFFERENCE));
}

/**
 * @tc.name: OpTest002
 * @tc.desc: test for OP function with DIFFERENCE option
 * @tc.type: FUNC
 * @tc.require: I766AZ
 */
HWTEST_F(RegionTest, OpTest002, TestSize.Level1)
{
    std::unique_ptr<Region> region = std::make_unique<Region>();
    ASSERT_TRUE(region != nullptr);
    RectI rectI(0, 0, 256, 256);
    RectI otherRectI(0, 400, 500, 600);
    region->SetRect(rectI);
    Region other;
    other.SetRect(otherRectI);
    EXPECT_TRUE(region->Op(other, RegionOp::DIFFERENCE));
}

/**
 * @tc.name: OpTest003
 * @tc.desc: test for OP function with INTERSECT option
 * @tc.type: FUNC
 * @tc.require: I766AZ
 */
HWTEST_F(RegionTest, OpTest003, TestSize.Level1)
{
    std::unique_ptr<Region> region = std::make_unique<Region>();
    ASSERT_TRUE(region != nullptr);
    RectI rectI(0, 0, 256, 256);
    RectI otherRectI(0, 0, 256, 256);
    region->SetRect(rectI);
    Region other;
    other.SetRect(otherRectI);
    EXPECT_TRUE(region->Op(other, RegionOp::INTERSECT));
}

/**
 * @tc.name: OpTest004
 * @tc.desc: test for OP function with UNION option
 * @tc.type: FUNC
 * @tc.require: I766AZ
 */
HWTEST_F(RegionTest, OpTest004, TestSize.Level1)
{
    std::unique_ptr<Region> region = std::make_unique<Region>();
    ASSERT_TRUE(region != nullptr);
    RectI rectI(0, 0, 256, 256);
    RectI otherRectI(0, 400, 500, 600);
    region->SetRect(rectI);
    Region other;
    other.SetRect(otherRectI);
    EXPECT_TRUE(region->Op(other, RegionOp::UNION));
}

/**
 * @tc.name: OpTest005
 * @tc.desc: test for OP function with XOR option
 * @tc.type: FUNC
 * @tc.require: I766AZ
 */
HWTEST_F(RegionTest, OpTest005, TestSize.Level1)
{
    std::unique_ptr<Region> region = std::make_unique<Region>();
    ASSERT_TRUE(region != nullptr);
    RectI rectI(0, 0, 256, 256);
    RectI otherRectI(0, 400, 500, 600);
    region->SetRect(rectI);
    Region other;
    other.SetRect(otherRectI);
    EXPECT_TRUE(region->Op(other, RegionOp::XOR));
}

/**
 * @tc.name: OpTest006
 * @tc.desc: test for OP function with REVERSE_DIFFERENCE option
 * @tc.type: FUNC
 * @tc.require: I766AZ
 */
HWTEST_F(RegionTest, OpTest006, TestSize.Level1)
{
    std::unique_ptr<Region> region = std::make_unique<Region>();
    ASSERT_TRUE(region != nullptr);
    RectI rectI(0, 0, 256, 256);
    RectI otherRectI(0, 400, 500, 600);
    region->SetRect(rectI);
    Region other;
    other.SetRect(otherRectI);
    EXPECT_TRUE(region->Op(other, RegionOp::REVERSE_DIFFERENCE));
}

/**
 * @tc.name: OpTest007
 * @tc.desc: test for OP function with REPLACE option
 * @tc.type: FUNC
 * @tc.require: I766AZ
 */
HWTEST_F(RegionTest, OpTest007, TestSize.Level1)
{
    std::unique_ptr<Region> region = std::make_unique<Region>();
    ASSERT_TRUE(region != nullptr);
    RectI rectI(0, 0, 256, 256);
    RectI otherRectI(0, 400, 500, 600);
    region->SetRect(rectI);
    Region other;
    other.SetRect(otherRectI);
    EXPECT_TRUE(region->Op(other, RegionOp::REPLACE));
}

/**
 * @tc.name: CloneTest001
 * @tc.desc: test for clone func
 * @tc.type: FUNC
 * @tc.require: I766AZ
 */
HWTEST_F(RegionTest, CloneTest001, TestSize.Level1)
{
    Region region;
    // rect, left: 0, top: 0, right: 2, bottom: 2
    RectI rectI(0, 0, 2, 2);
    region.SetRect(rectI);
    // point(1, 1) is inside rect
    EXPECT_TRUE(region.Contains(1, 1));
    Region other(region);
    EXPECT_TRUE(other.Contains(1, 1));

    Region region2;
    // rect, left: 10, top: 10, right: 20, bottom: 20
    RectI rectI2(10, 10, 20, 20);
    region2.SetRect(rectI2);
    // point(15, 15) is inside rect
    EXPECT_TRUE(region2.Contains(15, 15));
    Region other2;
    other2 = region2;
    EXPECT_TRUE(region2.Contains(15, 15));
}

/**
 * @tc.name: EqualsTest001
 * @tc.desc: test for two regions are equal.
 * @tc.type: FUNC
 * @tc.require: I766AZ
 */
HWTEST_F(RegionTest, EqualsTest001, TestSize.Level1)
{
    Region region;
    Region region2;
    ASSERT_TRUE(region == region2);
    RectI rectI(0, 0, 256, 256);
    region.SetRect(rectI);
    ASSERT_FALSE(region == region2);
}

/**
 * @tc.name: SetEmptyTest001
 * @tc.desc: test for set region empty.
 * @tc.type: FUNC
 * @tc.require: I766AZ
 */
HWTEST_F(RegionTest, SetEmptyTest001, TestSize.Level1)
{
    Region region;
    RectI rectI(0, 0, 256, 256);
    region.SetRect(rectI);
    region.SetEmpty();
    ASSERT_TRUE(region.IsEmpty());
}

/**
 * @tc.name: SetRegionTest001
 * @tc.desc: test for set region.
 * @tc.type: FUNC
 * @tc.require: I766AZ
 */
HWTEST_F(RegionTest, SetRegionTest001, TestSize.Level1)
{
    Region region;
    Region region2;
    RectI rectI(0, 0, 256, 256);
    region.SetRect(rectI);
    ASSERT_FALSE(region == region2);
    region2.SetRegion(region);
    ASSERT_TRUE(region == region2);
}

/**
 * @tc.name: GetBoundsTest001
 * @tc.desc: test for set region.
 * @tc.type: FUNC
 * @tc.require: I766AZ
 */
HWTEST_F(RegionTest, GetBoundsTest001, TestSize.Level1)
{
    Region region;
    Region region2;
    RectI rectI(0, 0, 256, 256);
    region.SetRect(rectI);
    ASSERT_TRUE(rectI == region.GetBounds());
}

/**
 * @tc.name: isComplexTest001
 * @tc.desc: test for region has multi rects.
 * @tc.type: FUNC
 * @tc.require: I766AZ
 */
HWTEST_F(RegionTest, isComplexTest001, TestSize.Level1)
{
    Region region;
    Region region2;
    RectI rectI(0, 0, 256, 256);
    RectI otherRectI(50, 50, 400, 400);
    region.SetRect(rectI);
    ASSERT_FALSE(region.IsComplex());
    region2.SetRect(otherRectI);
    region.Op(region2, RegionOp::UNION);
    ASSERT_TRUE(region.IsComplex());
}

/**
 * @tc.name: QuickRejectTest001
 * @tc.desc: test for region QuickReject with other.
 * @tc.type: FUNC
 * @tc.require: I766AZ
 */
HWTEST_F(RegionTest, QuickRejectTest001, TestSize.Level1)
{
    Region region;
    Region region2;
    RectI rectI(0, 0, 256, 256);
    RectI otherRectI(50, 50, 400, 400);
    region.SetRect(rectI);
    region2.SetRect(otherRectI);
    ASSERT_FALSE(region2.QuickReject(region));
}

/**
 * @tc.name: TranslateTest001
 * @tc.desc: test for region QuickReject with other.
 * @tc.type: FUNC
 * @tc.require: I766AZ
 */
HWTEST_F(RegionTest, TranslateTest001, TestSize.Level1)
{
    Region region;
    Region region2;
    RectI rectI(0, 0, 100, 100);
    RectI otherRectI(100, 100, 200, 200);
    region.SetRect(rectI);
    region2.SetRect(otherRectI);
    region.Translate(100, 100);
    ASSERT_TRUE(region == region2);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
