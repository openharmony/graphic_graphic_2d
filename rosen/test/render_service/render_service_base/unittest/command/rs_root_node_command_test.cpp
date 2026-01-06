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
#include "include/command/rs_root_node_command.h"
#include "include/command/rs_surface_node_command.h"
#include "include/pipeline/rs_surface_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRootNodeCommandTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRootNodeCommandTest::SetUpTestCase() {}
void RSRootNodeCommandTest::TearDownTestCase() {}
void RSRootNodeCommandTest::SetUp() {}
void RSRootNodeCommandTest::TearDown() {}

/**
 * @tc.name: TestRSBaseNodeCommand001
 * @tc.desc: AttachRSSurfaceNode test.
 * @tc.type: FUNC
 */
HWTEST_F(RSRootNodeCommandTest, TestRSRootNodeCommand001, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(-1);
    NodeId surfaceNodeId = static_cast<NodeId>(-2);
    RootNodeCommandHelper::AttachRSSurfaceNode(context, id, surfaceNodeId);
    RootNodeCommandHelper::Create(context, id);
    RootNodeCommandHelper::AttachRSSurfaceNode(context, id, surfaceNodeId);
    EXPECT_TRUE(id != -2);
}

/**
 * @tc.name: TestRSRootNodeCommand002
 * @tc.desc: SetEnableRender test.
 * @tc.type: FUNC
 */
HWTEST_F(RSRootNodeCommandTest, TestRSRootNodeCommand002, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(-1);
    bool flag = false;
    RootNodeCommandHelper::SetEnableRender(context, id, flag);
    RootNodeCommandHelper::Create(context, id);
    RootNodeCommandHelper::SetEnableRender(context, id, flag);
    EXPECT_TRUE(id != -2);
}

/**
 * @tc.name: TestRSRootNodeCommand003
 * @tc.desc: AttachToUniSurfaceNode test.
 * @tc.type: FUNC
 */
HWTEST_F(RSRootNodeCommandTest, TestRSRootNodeCommand003, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(-1);
    NodeId surfaceNodeId = static_cast<NodeId>(-2);
    RootNodeCommandHelper::AttachToUniSurfaceNode(context, id, surfaceNodeId);
    RootNodeCommandHelper::Create(context, id);
    SurfaceNodeCommandHelper::Create(context, surfaceNodeId);
    RootNodeCommandHelper::AttachToUniSurfaceNode(context, id, surfaceNodeId);
    EXPECT_TRUE(id != -2);
}

/**
 * @tc.name: TestRSRootNodeCommand004
 * @tc.desc: UpdateSuggestedBufferSize test.
 * @tc.type: FUNC
 */
HWTEST_F(RSRootNodeCommandTest, TestRSRootNodeCommand004, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(-1);
    int32_t width = static_cast<int32_t>(0);
    int32_t height = static_cast<int32_t>(0);
    RootNodeCommandHelper::UpdateSuggestedBufferSize(context, id, width, height);
    RootNodeCommandHelper::Create(context, id);
    RootNodeCommandHelper::UpdateSuggestedBufferSize(context, id, width, height);
    EXPECT_TRUE(id != -2);
}

/**
 * @tc.name: CreateTest001
 * @tc.desc: Verify function Create
 * @tc.type:FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSRootNodeCommandTest, CreateTest001, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    RootNodeCommandHelper::Create(context, id, false);
    EXPECT_TRUE(id != -2);
}

/**
 * @tc.name: CreateTest002
 * @tc.desc: Verify function Create
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRootNodeCommandTest, CreateTest002, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    RootNodeCommandHelper::Create(context, id);
    EXPECT_FALSE(context.GetMutableNodeMap().UnRegisterUnTreeNode(id));
}
 
/**
 * @tc.name: CreateTest003
 * @tc.desc: Verify function Create
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRootNodeCommandTest, CreateTest003, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    context.GetMutableNodeMap().RegisterUnTreeNode(id);
    RootNodeCommandHelper::Create(context, id);
    EXPECT_FALSE(context.GetMutableNodeMap().UnRegisterUnTreeNode(id));
}
 
/**
 * @tc.name: AttachToUniSurfaceNodeTest001
 * @tc.desc: Verify function AttachToUniSurfaceNode
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRootNodeCommandTest, AttachToUniSurfaceNodeTest001, TestSize.Level1)
{
    RSContext context;
    NodeId parentId = static_cast<NodeId>(1);
    NodeId childId = static_cast<NodeId>(2);
    SurfaceNodeCommandHelper::Create(context, parentId);
    ASSERT_NE(context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(parentId), nullptr);
    RootNodeCommandHelper::AttachToUniSurfaceNode(context, childId, parentId);
    EXPECT_TRUE(context.GetMutableNodeMap().UnRegisterUnTreeNode(childId));
}
 
/**
 * @tc.name: AttachToUniSurfaceNodeTest002
 * @tc.desc: Verify function AttachToUniSurfaceNode
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRootNodeCommandTest, AttachToUniSurfaceNodeTest002, TestSize.Level1)
{
    RSContext context;
    NodeId parentId = static_cast<NodeId>(1);
    NodeId childId = static_cast<NodeId>(2);
    SurfaceNodeCommandHelper::Create(context, parentId);
    ASSERT_NE(context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(parentId), nullptr);
    RootNodeCommandHelper::Create(context, childId);
    ASSERT_NE(context.GetNodeMap().GetRenderNode<RSRootRenderNode>(childId), nullptr);
    RootNodeCommandHelper::AttachToUniSurfaceNode(context, childId, parentId);
    EXPECT_FALSE(context.GetMutableNodeMap().UnRegisterUnTreeNode(childId));
}
 
/**
 * @tc.name: AttachToUniSurfaceNodeTest003
 * @tc.desc: Verify function AttachToUniSurfaceNode
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRootNodeCommandTest, AttachToUniSurfaceNodeTest003, TestSize.Level1)
{
    RSContext context;
    NodeId parentId = static_cast<NodeId>(1);
    NodeId childId = static_cast<NodeId>(2);
    RootNodeCommandHelper::Create(context, childId);
    ASSERT_NE(context.GetNodeMap().GetRenderNode<RSRootRenderNode>(childId), nullptr);
    RootNodeCommandHelper::AttachToUniSurfaceNode(context, childId, parentId);
    EXPECT_TRUE(context.GetMutableNodeMap().UnRegisterUnTreeNode(parentId));
}
} // namespace OHOS::Rosen
