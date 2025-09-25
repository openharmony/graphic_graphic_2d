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
    RSBaseNode::SharedPtr nodePtr = std::make_shared<RSNode>(0);
    nodePtr->id_ = 0;
    bool res = RSNodeMap::MutableInstance().RegisterNode(nodePtr);
    EXPECT_TRUE(res == false);

    nodePtr->id_ = 3;
    res = RSNodeMap::MutableInstance().RegisterNode(nodePtr);
    EXPECT_TRUE(res);
}

/**
 * @tc.name: RSNodeMapTest
 * @tc.desc: test results of RegisterNodeInstanceId
 * @tc.type:FUNC
 * @tc.require: issueI9TXX3
 */
HWTEST_F(RSNodeMapTest, RegisterNodeInstanceId001, TestSize.Level1)
{
    bool res = RSNodeMap::MutableInstance().RegisterNodeInstanceId(1, 1);
    EXPECT_TRUE(res);
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
    EXPECT_TRUE(node != nullptr);
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
    auto nodeone = RSNodeMap::MutableInstance().GetNode(1);
    EXPECT_EQ(nodeone, nullptr);

    RSBaseNode::SharedPtr nodePtr = std::make_shared<RSNode>(3);
    nodePtr->id_ = 3;
    RSNodeMap::MutableInstance().RegisterNode(nodePtr);
    nodeone = RSNodeMap::MutableInstance().GetNode(3);
    EXPECT_NE(nodeone, nullptr);
}

/**
 * @tc.name: RSNodeMapTest
 * @tc.desc: test results of GetNodeInstanceId
 * @tc.type:FUNC
 * @tc.require: issueI9TXX3
 */
HWTEST_F(RSNodeMapTest, GetNodeInstanceId001, TestSize.Level1)
{
    RSNodeMap::MutableInstance().RegisterNodeInstanceId(1, 1);
    auto res = RSNodeMap::MutableInstance().GetNodeInstanceId(1);
    EXPECT_EQ(res, 1);

    res = RSNodeMap::MutableInstance().GetNodeInstanceId(0);
    EXPECT_EQ(res, -1);
}

/**
 * @tc.name: GetInstanceIdForReleasedNodeTest
 * @tc.desc: test results of GetInstanceIdForReleasedNode
 * @tc.type: FUNC
 * @tc.require: issueIA5FLZ
 */
HWTEST_F(RSNodeMapTest, GetInstanceIdForReleasedNodeTest, TestSize.Level1)
{
    AnimationId animationId = 1;
    NodeId nodeId = 1;
    RSNodeMap::MutableInstance().animationNodeIdInstanceIdMap_.insert(
        std::make_pair(animationId, std::make_pair(nodeId, 1)));
    int32_t res = RSNodeMap::MutableInstance().GetInstanceIdForReleasedNode(1);
    EXPECT_EQ(res, 1);

    res = RSNodeMap::MutableInstance().GetInstanceIdForReleasedNode(0);
    EXPECT_EQ(res, -1);
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