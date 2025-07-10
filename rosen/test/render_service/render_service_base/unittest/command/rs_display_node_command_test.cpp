/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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
#include "include/command/rs_display_node_command.h"
#include "pipeline/rs_logical_display_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSDisplayNodeCommandTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSDisplayNodeCommandTest::SetUpTestCase() {}
void RSDisplayNodeCommandTest::TearDownTestCase() {}
void RSDisplayNodeCommandTest::SetUp() {}
void RSDisplayNodeCommandTest::TearDown() {}

/**
 * @tc.name: TestRSBaseNodeCommand001
 * @tc.desc: SetScreenId test.
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayNodeCommandTest, TestRSDisplayNodeCommand001, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(-1);
    uint64_t screenId = static_cast<uint64_t>(0);
    DisplayNodeCommandHelper::SetScreenId(context, id, screenId);
    EXPECT_EQ(context.GetNodeMap().GetRenderNode<RSLogicalDisplayRenderNode>(id), nullptr);

    NodeId id2 = static_cast<NodeId>(1);
    RSDisplayNodeConfig config { 0, false, 0 };
    DisplayNodeCommandHelper::Create(context, id2, config);
    DisplayNodeCommandHelper::SetScreenId(context, id2, screenId);
    EXPECT_NE(context.GetNodeMap().GetRenderNode<RSLogicalDisplayRenderNode>(id2), nullptr);
}

/**
 * @tc.name: TestRSDisplayNodeCommand003
 * @tc.desc: SetSecurityDisplay test.
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayNodeCommandTest, TestRSDisplayNodeCommand003, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    bool isSecurityDisplay = false;
    DisplayNodeCommandHelper::SetSecurityDisplay(context, id, isSecurityDisplay);

    RSDisplayNodeConfig config { 0, false, 0 };
    DisplayNodeCommandHelper::Create(context, id, config);
    DisplayNodeCommandHelper::SetSecurityDisplay(context, id, isSecurityDisplay);
    EXPECT_NE(context.GetNodeMap().GetRenderNode<RSLogicalDisplayRenderNode>(id), nullptr);
}

/**
 * @tc.name: TestRSDisplayNodeCommand004
 * @tc.desc: SetDisplayMode test.
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayNodeCommandTest, TestRSDisplayNodeCommand004, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    RSDisplayNodeConfig config { 0, true, 0 };
    DisplayNodeCommandHelper::SetDisplayMode(context, id, config);
    EXPECT_EQ(context.GetNodeMap().GetRenderNode<RSLogicalDisplayRenderNode>(id), nullptr);
}

/**
 * @tc.name: Create001
 * @tc.desc: test.
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayNodeCommandTest, Create001, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    RSDisplayNodeConfig config { 0, false, 0 };
    DisplayNodeCommandHelper::Create(context, id, config);
    EXPECT_NE(context.GetNodeMap().GetRenderNode<RSLogicalDisplayRenderNode>(id), nullptr);

    NodeId id2 = static_cast<NodeId>(2);
    config.isMirrored = true;
    DisplayNodeCommandHelper::Create(context, id2, config);
    EXPECT_NE(context.GetNodeMap().GetRenderNode<RSLogicalDisplayRenderNode>(id2), nullptr);

    NodeId id3 = static_cast<NodeId>(3);
    config.mirrorNodeId = id2;
    DisplayNodeCommandHelper::Create(context, id3, config);
    EXPECT_NE(context.GetNodeMap().GetRenderNode<RSLogicalDisplayRenderNode>(id3), nullptr);
}

/**
 * @tc.name: AddDisplayNodeToTree001
 * @tc.desc: test.
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayNodeCommandTest, AddDisplayNodeToTree001, TestSize.Level1)
{
    RSContext context;
    auto renderNodeChildren = context.GetGlobalRootRenderNode()->children_;

    NodeId id = static_cast<NodeId>(1);
    DisplayNodeCommandHelper::AddDisplayNodeToTree(context, id);
    ASSERT_EQ(renderNodeChildren.size(), 0);

    NodeId id2 = static_cast<NodeId>(2);
    DisplayNodeCommandHelper::AddDisplayNodeToTree(context, id2);
    ASSERT_EQ(renderNodeChildren.size(), 0);
}

/**
 * @tc.name: RemoveDisplayNodeFromTree001
 * @tc.desc: test.
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayNodeCommandTest, RemoveDisplayNodeFromTree001, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    DisplayNodeCommandHelper::RemoveDisplayNodeFromTree(context, id);
    EXPECT_NE(context.GetGlobalRootRenderNode(), nullptr);

    NodeId id2 = static_cast<NodeId>(2);
    DisplayNodeCommandHelper::RemoveDisplayNodeFromTree(context, id2);
    EXPECT_NE(context.GetGlobalRootRenderNode(), nullptr);
}

/**
 * @tc.name: SetBootAnimation001
 * @tc.desc: test.
 * @tc.type: FUNC
 * @tc.require:SR000HSUII
 */
HWTEST_F(RSDisplayNodeCommandTest, SetBootAnimation001, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    RSDisplayNodeConfig config { 0, true, 0 };
    DisplayNodeCommandHelper::Create(context, id, config);
    DisplayNodeCommandHelper::SetBootAnimation(context, id, true);

    DisplayNodeCommandHelper::SetBootAnimation(context, 5, true);
    EXPECT_NE(context.GetNodeMap().GetRenderNode<RSLogicalDisplayRenderNode>(id), nullptr);
}

/**
 * @tc.name: SetRogSize001
 * @tc.desc: SetScreenId test.
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayNodeCommandTest, SetRogSize001, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    DisplayNodeCommandHelper::SetScreenId(context, id, 1);

    RSDisplayNodeConfig config { 0, false, 0 };
    DisplayNodeCommandHelper::Create(context, id, config);
    DisplayNodeCommandHelper::SetScreenId(context, id, 1);
    EXPECT_NE(context.GetNodeMap().GetRenderNode<RSLogicalDisplayRenderNode>(id), nullptr);
}

/**
 * @tc.name: SetForceCloseHdrTest
 * @tc.desc: SetForceCloseHdr test.
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayNodeCommandTest, SetForceCloseHdrTest, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    DisplayNodeCommandHelper::SetScreenId(context, id, 1);
    DisplayNodeCommandHelper::SetForceCloseHdr(context, id, true);

    RSDisplayNodeConfig config { 0, false, 0 };
    DisplayNodeCommandHelper::Create(context, id, config);
    DisplayNodeCommandHelper::SetForceCloseHdr(context, id, true);
    EXPECT_NE(context.GetNodeMap().GetRenderNode<RSLogicalDisplayRenderNode>(id), nullptr);
}

/**
 * @tc.name: SetScreenRotation001
 * @tc.desc: SetScreenId test.
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayNodeCommandTest, SetScreenRotation001, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    DisplayNodeCommandHelper::SetScreenRotation(context, id, ScreenRotation::ROTATION_0);

    RSDisplayNodeConfig config { 0, false, 0 };
    DisplayNodeCommandHelper::Create(context, id, config);
    DisplayNodeCommandHelper::SetScreenRotation(context, id, ScreenRotation::ROTATION_0);
    EXPECT_NE(context.GetNodeMap().GetRenderNode<RSLogicalDisplayRenderNode>(id), nullptr);
}

/**
 * @tc.name: SetDisplayMode001
 * @tc.desc: SetScreenId test.
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayNodeCommandTest, SetDisplayMode001, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    RSDisplayNodeConfig config { 0, false, 0 };
    DisplayNodeCommandHelper::SetDisplayMode(context, id, config);
    EXPECT_EQ(context.GetNodeMap().GetRenderNode<RSLogicalDisplayRenderNode>(id), nullptr);

    DisplayNodeCommandHelper::Create(context, id, config);
    DisplayNodeCommandHelper::SetDisplayMode(context, id, config);

    config.isMirrored = true;
    DisplayNodeCommandHelper::SetDisplayMode(context, id, config);

    NodeId mirrorNodeId = static_cast<NodeId>(2);
    config.mirrorNodeId = mirrorNodeId;
    DisplayNodeCommandHelper::Create(context, mirrorNodeId, config);
    DisplayNodeCommandHelper::SetDisplayMode(context, id, config);
}

/**
 * @tc.name: SetScbNodePid
 * @tc.desc: SetScbNodePid test.
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayNodeCommandTest, SetScbNodePid, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    std::vector<int32_t> oldScbPids = {};
    int32_t currentScbPid = -1;
    DisplayNodeCommandHelper::SetScbNodePid(context, id, oldScbPids, currentScbPid);
    oldScbPids.push_back(1);
    oldScbPids.push_back(2);
    DisplayNodeCommandHelper::SetScbNodePid(context, id, oldScbPids, currentScbPid);
    EXPECT_EQ(context.GetNodeMap().GetRenderNode<RSLogicalDisplayRenderNode>(id), nullptr);
}

/**
 * @tc.name: SetScbNodePid001
 * @tc.desc: SetScbNodePid test.
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSDisplayNodeCommandTest, SetScbNodePid001, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    std::vector<int32_t> oldScbPids = {};
    int32_t currentScbPid = -1;
    RSDisplayNodeConfig config { 0, true, 0 };
    DisplayNodeCommandHelper::Create(context, id, config);

    oldScbPids.push_back(1);
    oldScbPids.push_back(2);
    DisplayNodeCommandHelper::SetScbNodePid(context, id, oldScbPids, currentScbPid);
    DisplayNodeCommandHelper::SetScbNodePid(context, 5, oldScbPids, currentScbPid);
    EXPECT_NE(context.GetNodeMap().GetRenderNode<RSLogicalDisplayRenderNode>(id), nullptr);
}

/**
 * @tc.name: SetVirtualScreenMuteStatus
 * @tc.desc: SetVirtualScreenMuteStatus test.
 * @tc.type: FUNC
 * @tc.require: issueIBTNC3
 */
HWTEST_F(RSDisplayNodeCommandTest, SetVirtualScreenMuteStatus, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);

    DisplayNodeCommandHelper::SetVirtualScreenMuteStatus(context, id, true);
    if (auto node = context.GetNodeMap().GetRenderNode<RSLogicalDisplayRenderNode>(id)) {
        ASSERT_TRUE(node->GetVirtualScreenMuteStatus());
    }

    DisplayNodeCommandHelper::SetVirtualScreenMuteStatus(context, id, false);
    if (auto node = context.GetNodeMap().GetRenderNode<RSLogicalDisplayRenderNode>(id)) {
        ASSERT_FALSE(node->GetVirtualScreenMuteStatus());
    }
}
} // namespace OHOS::Rosen
