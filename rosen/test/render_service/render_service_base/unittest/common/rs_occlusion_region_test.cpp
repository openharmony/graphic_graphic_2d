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
 * @tc.name: InitDynamicLibraryFunction001
 * @tc.desc: test results of InitDynamicLibraryFunction
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionRegionTest, InitDynamicLibraryFunction001, Function | MediumTest | Level2)
{
    /**
     * @tc.steps: step1. InitDynamicLibraryFunction
     */
    RSInnovation::OpenInnovationSo();
    Region::InitDynamicLibraryFunction();
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
 * @tc.name: RegionOp002
 * @tc.desc: test results of RegionOp
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionRegionTest, RegionOp002, Function | MediumTest | Level2)
{
    /**
     * @tc.steps: step1. RegionOp
     */
    RSInnovation::OpenInnovationSo();
    RSInnovation::UpdateOcclusionCullingSoEnabled();
    Occlusion::Region::InitDynamicLibraryFunction();
    Rect rect;
    Region region;
    Region region1(rect);
    Region region2(rect);
    Region res;
    Region::OP op = Region::OP::AND;
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
} // namespace OHOS::Rosen