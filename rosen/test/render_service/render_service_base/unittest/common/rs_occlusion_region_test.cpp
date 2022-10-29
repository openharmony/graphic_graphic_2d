/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
} // namespace OHOS::Rosen