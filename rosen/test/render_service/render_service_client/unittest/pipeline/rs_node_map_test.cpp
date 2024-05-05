/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "pipeline/rs_node_map.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_surface_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSNodeMapTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSNodeMapTest::SetUpTestCase() {}
void RSNodeMapTest::TearDownTestCase() {}
void RSNodeMapTest::SetUp() {}
void RSNodeMapTest::TearDown() {}

/**
 * @tc.name: MutableInstanceTest
 * @tc.desc: test results of MutableInstance
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSNodeMapTest, MutableInstance001, TestSize.Level1)
{
    RSNodeMap::MutableInstance();
    EXPECT_TRUE(true);
}

/**
 * @tc.name: InstanceTest
 * @tc.desc: test results of Instance
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSNodeMapTest, Instance001, TestSize.Level1)
{
    RSNodeMap::Instance();
    EXPECT_TRUE(true);
}

/**
 * @tc.name: RSNodeMapTest
 * @tc.desc: test results of RegisterNode
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSNodeMapTest, RegisterNode001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. RSNodeMap001
     */
    RSCanvasNode::SharedPtr node = RSCanvasNode::Create();
    ASSERT_FALSE(RSNodeMap::MutableInstance().RegisterNode(node));
}

/**
 * @tc.name: RSNodeMapTest
 * @tc.desc: test results of UnregisterNode
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSNodeMapTest, UnregisterNode001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. RSNodeMap001
     */
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr node = RSSurfaceNode::Create(config);
    RSNodeMap::MutableInstance().UnregisterNode(node->GetId());
}

/**
 * @tc.name: RSNodeMapTest
 * @tc.desc: test results of GetNode
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSNodeMapTest, GetNode001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. RSNodeMap001
     */
    RSCanvasNode::SharedPtr node = RSCanvasNode::Create();
    auto nodeone = RSNodeMap::MutableInstance().GetNode(node->GetId());
    ASSERT_NE(nodeone, nullptr);
}

/**
 * @tc.name: GetAnimationFallbackNodeTest
 * @tc.desc: test results of GetAnimationFallbackNode
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSNodeMapTest, GetAnimationFallbackNode001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. RSNodeMap001
     */
    RSCanvasNode::SharedPtr node = RSCanvasNode::Create();
    auto rsNode = RSNodeMap::MutableInstance().GetAnimationFallbackNode();
    EXPECT_NE(rsNode, nullptr);
}
} // namespace OHOS::Rosen