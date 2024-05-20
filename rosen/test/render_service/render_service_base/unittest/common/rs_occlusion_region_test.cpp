/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "platform/common/rs_innovation.h"

#include "common/rs_occlusion_region.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen::Occlusion {
class RSOcclusionRegionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline std::shared_ptr<Node> testnode;
};

void RSOcclusionRegionTest::SetUpTestCase() {}
void RSOcclusionRegionTest::TearDownTestCase() {}
void RSOcclusionRegionTest::SetUp()
{
    constexpr int s = 0, e = 2;
    testnode = std::make_shared<Node>(s, e);
}
void RSOcclusionRegionTest::TearDown() {}

/**
 * @tc.name: OstreamOperator001
 * @tc.desc: test results of operator<<
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionRegionTest, OstreamOperator001, Function | MediumTest | Level2)
{
    /**
     * @tc.steps: step1. operator<<
     */
    Rect rect;
    std::cout << rect;
}

/**
 * @tc.name: OstreamOperator002
 * @tc.desc: test results of operator<<
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionRegionTest, OstreamOperator002, Function | MediumTest | Level2)
{
    /**
     * @tc.steps: step1. operator<<
     */
    Rect rect;
    Region region(rect);
    std::cout << region;
}

/**
 * @tc.name: EventSortBy001
 * @tc.desc: test results of EventSortBy
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSOcclusionRegionTest, EventSortBy001, Function | MediumTest | Level2)
{
    /**
     * @tc.steps: step1. EventSortBy
     */
    Event e1 = Event(0, Event::Type::OPEN, 0, 100);
    Event e2 = Event(1, Event::Type::CLOSE, 0, 100);
    ASSERT_TRUE(EventSortByY(e1, e2));

    Event e3 = Event(0, Event::Type::CLOSE, 0, 100);
    ASSERT_FALSE(EventSortByY(e1, e3));
}

/**
 * @tc.name: GetAndRange001
 * @tc.desc: test results of GetAndRange001
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSOcclusionRegionTest, GetAndRange001, Function | MediumTest | Level2)
{
    /**
     * @tc.steps: step1. GetAndRange
     */
    std::vector<Range> res;
    testnode->GetAndRange(res, true, true);
    ASSERT_EQ(res.size(), 1);
    ASSERT_TRUE(res[0] == Range(0, 2));

    testnode->GetAndRange(res, true, false);
    ASSERT_EQ(res.size(), 1);

    //left_ and right_ will be delated when deleting testnode
    testnode->left_ = new Node(0, 1);
    testnode->right_ = new Node(0, 1);
    testnode->GetAndRange(res, true, false);
    ASSERT_EQ(res.size(), 1);
    ASSERT_TRUE(res[0] == Range(0, 2));
}

/**
 * @tc.name: GetOrRange001
 * @tc.desc: test results of GetOrRange001
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSOcclusionRegionTest, GetOrRange001, Function | MediumTest | Level2)
{
    /**
     * @tc.steps: step1. GetOrRange
     */
    std::vector<Range> res;
    testnode->GetOrRange(res, false, false);
    ASSERT_EQ(res.size(), 0);

    testnode->GetOrRange(res, true, false);
    ASSERT_EQ(res.size(), 1);
    ASSERT_TRUE(res[0] == Range(0, 2));
    
    //left_ and right_ will be delated when deleting testnode
    testnode->left_ = new Node(0, 1);
    testnode->right_ = new Node(0, 1);
    testnode->GetOrRange(res, false, false);
    ASSERT_EQ(res.size(), 1);
    ASSERT_TRUE(res[0] == Range(0, 2));
}

/**
 * @tc.name: GetXOrRange001
 * @tc.desc: test results of GetXOrRange001
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSOcclusionRegionTest, GetXOrRange001, Function | MediumTest | Level2)
{
    /**
     * @tc.steps: step1. GetXOrRange
     */
    std::vector<Range> res;
    testnode->GetXOrRange(res, true, false);
    ASSERT_EQ(res.size(), 1);
    ASSERT_TRUE(res[0] == Range(0, 2));

    testnode->GetXOrRange(res, true, true);
    ASSERT_EQ(res.size(), 1);
    ASSERT_TRUE(res[0] == Range(0, 2));

    //left_ and right_ will be delated when deleting testnode
    testnode->left_ = new Node(0, 1);
    testnode->right_ = new Node(0, 1);
    testnode->GetXOrRange(res, true, false);
    ASSERT_EQ(res.size(), 3);
    ASSERT_TRUE(res[2] == Range(0, 1));

    testnode->GetXOrRange(res, false, false);
    ASSERT_EQ(res.size(), 3);
    ASSERT_TRUE(res[2] == Range(0, 1));
}

/**
 * @tc.name: GetSubRange001
 * @tc.desc: test results of GetSubRange001
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSOcclusionRegionTest, GetSubRange001, Function | MediumTest | Level2)
{
    /**
     * @tc.steps: step1. GetSubRange
     */
    std::vector<Range> res;
    testnode->GetSubRange(res, true, false);
    ASSERT_EQ(res.size(), 1);
    ASSERT_TRUE(res[0] == Range(0, 2));

    testnode->GetSubRange(res, false, true);
    ASSERT_EQ(res.size(), 1);
    ASSERT_TRUE(res[0] == Range(0, 2));

    //left_ and right_ will be delated when deleting testnode
    testnode->left_ = new Node(0, 1);
    testnode->right_ = new Node(0, 1);
    testnode->GetSubRange(res, true, false);
    ASSERT_EQ(res.size(), 3);
    ASSERT_TRUE(res[2] == Range(0, 1));
}

/**
 * @tc.name: RegionOp001
 * @tc.desc: test results of RegionOp
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionRegionTest, RegionOp001, Function | MediumTest | Level2)
{
    /**
     * @tc.steps: step1. RegionOp
     */
    Region region;
    Region region1;
    Region region2;
    Region res;
    Region::OP op = Region::OP::SUB;
    region.RegionOp(region1, region2, res, op);
}

/**
 * @tc.name: RegionOpLocal001
 * @tc.desc: test results of RegionOpLocal
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionRegionTest, RegionOpLocal001, Function | MediumTest | Level2)
{
    /**
     * @tc.steps: step1. RegionOpLocal
     */
    Region region;
    Region region1;
    Region region2;
    Region res;
    Region::OP op = Region::OP::AND;
    region.RegionOpLocal(region1, region2, res, op);
}

/**
 * @tc.name: RegionOpLocal002
 * @tc.desc: test results of RegionOpLocal
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionRegionTest, RegionOpLocal002, Function | MediumTest | Level2)
{
    /**
     * @tc.steps: step1. RegionOpLocal
     */
    Rect rect;
    Region region;
    Region region1(rect);
    Region region2(rect);
    Region res;
    Region::OP op = static_cast<Region::OP>(0);
    region.RegionOpLocal(region1, region2, res, op);
}

/**
 * @tc.name: XOrSelf001
 * @tc.desc: test results of XOrSelf
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionRegionTest, XOrSelf001, Function | MediumTest | Level2)
{
    /**
     * @tc.steps: step1. XOrSelf
     */
    Rect rect;
    Region region1(rect);
    Region region2(rect);
    region1.XOrSelf(region2);
}

/**
 * @tc.name: Area
 * @tc.desc: test results of Area
 * @tc.type:FUNC
 * @tc.require: I80I2D
 */
HWTEST_F(RSOcclusionRegionTest, Area, Function | MediumTest | Level2)
{
    /**
     * @tc.steps: step1. Area
     */
    Rect rect1{0, 0, 100, 100};
    Rect rect2{50, 50, 150, 150};
    Rect rect3{0, 0, 0, 0};
    Region region1(rect1);
    Region region2(rect2);
    Region region3 = region1.Or(region2);
    Region region4 = region1.And(region2);
    Region region5(rect3);
    ASSERT_TRUE(rect1.Area() == 10000);
    ASSERT_TRUE(rect2.Area() == 10000);
    ASSERT_TRUE(rect3.Area() == 0);
    ASSERT_TRUE(region1.Area() == 10000);
    ASSERT_TRUE(region2.Area() == 10000);
    ASSERT_TRUE(region3.Area() == 17500);
    ASSERT_TRUE(region4.Area() == 2500);
    ASSERT_TRUE(region5.Area() == 0);
}

/**
 * @tc.name: IntersectArea
 * @tc.desc: test results of IntersectArea
 * @tc.type:FUNC
 * @tc.require: I80I2D
 */
HWTEST_F(RSOcclusionRegionTest, IntersectArea, Function | MediumTest | Level2)
{
    /**
     * @tc.steps: step. IntersectArea
     */
    Rect rect1{0, 0, 100, 100};
    Rect rect2{50, 50, 150, 150};
    Rect rect3{60, 60, 70, 80};
    Region region1(rect1);
    Region region2(rect2);
    Region region3(rect3);

    Rect testRect{65, 64, 82, 95};
    Region testRegion(testRect);

    Region region4 = region1.Or(region2);
    Region region5 = region4.Sub(region3);
    Region region6 = region5.And(testRegion);
    ASSERT_TRUE(region5.IntersectArea(testRect) == region6.Area());
}

/**
 * @tc.name: ResetSurfaceOpaqueRegionTest1
 * @tc.desc: test abnormal value of RectI input
 * @tc.type:FUNC
 * @tc.require: I8K5SW
 */
HWTEST_F(RSOcclusionRegionTest, ResetSurfaceOpaqueRegionTest1, Function | MediumTest | Level2)
{
    Rect trans{};
    Rect opaque{353284384, 2147483647, 1184, 2147483647};
    Rect screen{0, 0, 1260, 2720};
    Region transparentRegion{trans};
    Region opaqueRegion{opaque};
    Region screenRegion{screen};
    std::cout << "transparentRegion " << transparentRegion.GetRegionInfo() << std::endl;
    std::cout << "opaqueRegion " << opaqueRegion.GetRegionInfo() << std::endl;
    std::cout << "screenRegion " << screenRegion.GetRegionInfo() << std::endl;
    transparentRegion.AndSelf(screenRegion);
    opaqueRegion.AndSelf(screenRegion);
    std::cout << "transparentRegion " << transparentRegion.GetRegionInfo() << std::endl;
    std::cout << "opaqueRegion " << opaqueRegion.GetRegionInfo() << std::endl;
}

/**
 * @tc.name: ResetSurfaceOpaqueRegionTest2
 * @tc.desc: test abnormal value of RectI input
 * @tc.type:FUNC
 * @tc.require: I8K5SW
 */
HWTEST_F(RSOcclusionRegionTest, ResetSurfaceOpaqueRegionTest2, Function | MediumTest | Level2)
{
    Rect trans{};
    Rect opaque{1021068672, 2147483647, 1024, 2147483647};
    Rect screen{0, 0, 1260, 2720};
    Region transparentRegion{trans};
    Region opaqueRegion{opaque};
    Region screenRegion{screen};
    std::cout << "transparentRegion " << transparentRegion.GetRegionInfo() << std::endl;
    std::cout << "opaqueRegion " << opaqueRegion.GetRegionInfo() << std::endl;
    std::cout << "screenRegion " << screenRegion.GetRegionInfo() << std::endl;
    transparentRegion.AndSelf(screenRegion);
    opaqueRegion.AndSelf(screenRegion);
    std::cout << "transparentRegion " << transparentRegion.GetRegionInfo() << std::endl;
    std::cout << "opaqueRegion " << opaqueRegion.GetRegionInfo() << std::endl;
}

/**
 * @tc.name: ResetSurfaceOpaqueRegionTest3
 * @tc.desc: test abnormal value of RectI input
 * @tc.type:FUNC
 * @tc.require: I8K5SW
 */
HWTEST_F(RSOcclusionRegionTest, ResetSurfaceOpaqueRegionTest3, Function | MediumTest | Level2)
{
    Rect trans{};
    Rect opaque{2147483647, 2147483647, 24964864, 2147483647};
    Rect screen{0, 0, 1260, 2720};
    Region transparentRegion{trans};
    Region opaqueRegion{opaque};
    Region screenRegion{screen};
    std::cout << "transparentRegion " << transparentRegion.GetRegionInfo() << std::endl;
    std::cout << "opaqueRegion " << opaqueRegion.GetRegionInfo() << std::endl;
    std::cout << "screenRegion " << screenRegion.GetRegionInfo() << std::endl;
    transparentRegion.AndSelf(screenRegion);
    opaqueRegion.AndSelf(screenRegion);
    std::cout << "transparentRegion " << transparentRegion.GetRegionInfo() << std::endl;
    std::cout << "opaqueRegion " << opaqueRegion.GetRegionInfo() << std::endl;
}

/**
 * @tc.name: UpdateTest
 * @tc.desc: Verify function Update
 * @tc.type: FUNC
 * @tc.require: issuesI9OX7J
 */
HWTEST_F(RSOcclusionRegionTest, UpdateTest, Function | MediumTest | Level2)
{
    // for test
    Node node(0, 10);
    node.Update(10, 10, Event::Type::OPEN);
    node.Update(0, 10, Event::Type::OPEN);
    EXPECT_EQ(node.positive_count_, 1);
    node.Update(0, 10, Event::Type::CLOSE);
    EXPECT_EQ(node.positive_count_, 0);
    node.Update(0, 10, Event::Type::VOID_CLOSE);
    EXPECT_EQ(node.negative_count_, Event::Type::VOID_CLOSE);
    node.Update(0, 5, Event::Type::OPEN);
    EXPECT_EQ(node.left_ != nullptr, true);
    EXPECT_EQ(node.right_ != nullptr, true);
}

/**
 * @tc.name: getRangeTest
 * @tc.desc: Verify function getRange
 * @tc.type: FUNC
 * @tc.require: issuesI9OX7J
 */
HWTEST_F(RSOcclusionRegionTest, getRangeTest, Function | MediumTest | Level2)
{
    // for test
    Node node(0, 10);
    std::vector<Range> ranges;
    Region region;
    region.getRange(ranges, node, Region::OP::AND);
    region.getRange(ranges, node, Region::OP::OR);
    region.getRange(ranges, node, Region::OP::XOR);
    region.getRange(ranges, node, Region::OP::SUB);
    EXPECT_EQ(node.right_ != nullptr, false);
}

/**
 * @tc.name: UpdateRectsTest
 * @tc.desc: Verify function UpdateRects
 * @tc.type: FUNC
 * @tc.require: issuesI9OX7J
 */
HWTEST_F(RSOcclusionRegionTest, UpdateRectsTest, Function | MediumTest | Level2)
{
    Region::Rects rects;
    std::vector<Range> ranges;
    std::vector<int> indexAt;
    Region region;
    Region resultRegion;
    // for test
    rects.preRects.push_back(Rect { 0, 0, 100, 50 });
    rects.preRects.push_back(Rect { 150, 0, 100, 50 });
    rects.preRects.push_back(Rect { 150, 0, 250, 50 });
    rects.curY = 100;
    rects.preY = 50;

    ranges.emplace_back(Range { 0, 1 });
    ranges.emplace_back(Range { 1, 2 });
    ranges.emplace_back(Range { 1, 2 });
    indexAt.push_back(0);
    indexAt.push_back(100);
    indexAt.push_back(250);
    region.UpdateRects(rects, ranges, indexAt, resultRegion);
    EXPECT_EQ(rects.preRects[0], (Rect { 0, 0, 100, 100 }));
}

/**
 * @tc.name: RegionOpTest
 * @tc.desc: Verify function RegionOp
 * @tc.type: FUNC
 * @tc.require: issuesI9OX7J
 */
HWTEST_F(RSOcclusionRegionTest, RegionOpTest, Function | MediumTest | Level2)
{
    Region emptyRegion(Rect { 0, 0, 0, 0 });
    Region baseRegion;
    Region resultRegion;
    Region::OP operationType = Region::OP::AND;
    Region regionOperator;
    regionOperator.RegionOp(emptyRegion, baseRegion, resultRegion, operationType);
}
} // namespace OHOS::Rosen