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
#include "include/feature/window_keyframe/rs_window_keyframe_node_command.h"
#include "feature/window_keyframe/rs_window_keyframe_render_node.h"
#include "pipeline/rs_render_node_gc.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSWindowKeyFrameNodeCommandTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSWindowKeyFrameNodeCommandTest::SetUpTestCase() {}
void RSWindowKeyFrameNodeCommandTest::TearDownTestCase() {}
void RSWindowKeyFrameNodeCommandTest::SetUp() {}
void RSWindowKeyFrameNodeCommandTest::TearDown() {}

/**
 * @tc.name: Create001
 * @tc.desc: Create test.
 * @tc.type: FUNC
 */
HWTEST_F(RSWindowKeyFrameNodeCommandTest, Create001, TestSize.Level1)
{
    RSContext context;
    NodeId keyframeNodeId = static_cast<NodeId>(1);
    RSWindowKeyFrameNodeCommandHelper::Create(context, keyframeNodeId, false);

    auto keyframeNode = context.GetMutableNodeMap().GetRenderNode(keyframeNodeId);
    ASSERT_NE(keyframeNode, nullptr);
    EXPECT_EQ(keyframeNode->GetId(), keyframeNodeId);
    EXPECT_EQ(keyframeNode->GetType(), RSRenderNodeType::WINDOW_KEYFRAME_NODE);
}

/**
 * @tc.name: LinkNode
 * @tc.desc: Create test.
 * @tc.type: FUNC
 */
HWTEST_F(RSWindowKeyFrameNodeCommandTest, LinkNode, TestSize.Level1)
{
    RSContext context;
    NodeId keyframeNodeId = static_cast<NodeId>(2);
    NodeId linkedNodeId = static_cast<NodeId>(3);
    RSWindowKeyFrameNodeCommandHelper::LinkNode(context, keyframeNodeId, linkedNodeId);

    RSWindowKeyFrameNodeCommandHelper::Create(context, keyframeNodeId, false);
    RSWindowKeyFrameNodeCommandHelper::LinkNode(context, keyframeNodeId, linkedNodeId);
    auto keyframeNode = context.GetMutableNodeMap().GetRenderNode<RSWindowKeyFrameRenderNode>(keyframeNodeId);
    ASSERT_NE(keyframeNode, nullptr);
    EXPECT_EQ(keyframeNode->GetLinkedNodeId(), linkedNodeId);
}

} // namespace OHOS::Rosen
