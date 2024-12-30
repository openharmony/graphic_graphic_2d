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

#include "gtest/gtest.h"
#include "include/command/rs_canvas_node_command.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_render_node_gc.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSCanvasNodeCommandUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSCanvasNodeCommandUnitTest::SetUpTestCase() {}
void RSCanvasNodeCommandUnitTest::TearDownTestCase() {}
void RSCanvasNodeCommandUnitTest::SetUp() {}
void RSCanvasNodeCommandUnitTest::TearDown() {}

/**
 * @tc.name: TestRSCanvasNodeCommand01
 * @tc.desc: Verify function RSCanvasNodeCommand
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasNodeCommandUnitTest, TestRSCanvasNodeCommand01, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(-1);
    std::shared_ptr<Drawing::DrawCmdList> drawCmds = nullptr;
    RSModifierType type = RSModifierType::INVALID;
    RSCanvasNodeCommandHelper::UpdateRecording(context, nodeId, drawCmds, static_cast<uint16_t>(type));
    ASSERT_EQ(nodeId, static_cast<NodeId>(-1));
}

/**
 * @tc.name: TestRSCanvasNodeCommand02
 * @tc.desc: Verify function RSCanvasNodeCommand
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasNodeCommandUnitTest, TestRSCanvasNodeCommand02, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(-1);
    RSCanvasNodeCommandHelper::ClearRecording(context, nodeId);
    ASSERT_EQ(context.GetNodeMap().GetRenderNode<RSCanvasRenderNode>(nodeId), nullptr);
}

/**
 * @tc.name: TestCreate01
 * @tc.desc: Verify function Create
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasNodeCommandUnitTest, TestCreate01, TestSize.Level1)
{
    RSContext context;
    NodeId targetId = static_cast<NodeId>(-1);
    RSCanvasNodeCommandHelper::Create(context, targetId, false);
    ASSERT_EQ(targetId, static_cast<NodeId>(-1));
}

/**
 * @tc.name: TestCreate02
 * @tc.desc: Verify function Create
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasNodeCommandUnitTest, TestCreate02, TestSize.Level1)
{
    RSContext context;
    NodeId targetId = 0;
    RSCanvasNodeCommandHelper::Create(context, targetId, false);
    ASSERT_EQ(targetId, static_cast<NodeId>(0));
}

/**
 * @tc.name: TestAddCmdToSingleFrameComposer01
 * @tc.desc: Verify function AddCmdToSingleFrameComposer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasNodeCommandUnitTest, TestAddCmdToSingleFrameComposer01, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    RSCanvasNodeCommandHelper::Create(context, id, true);
    auto node = context.GetNodeMap().GetRenderNode<RSCanvasRenderNode>(id);
    std::shared_ptr<Drawing::DrawCmdList> drawCmds;
    RSModifierType type = RSModifierType::INVALID;
    bool res = RSCanvasNodeCommandHelper::AddCmdToSingleFrameComposer(node, drawCmds, type);
    EXPECT_TRUE(res == false);

    std::thread::id thisThreadId = std::this_thread::get_id();
    RSSingleFrameComposer::SetSingleFrameFlag(thisThreadId);
    res = RSCanvasNodeCommandHelper::AddCmdToSingleFrameComposer(node, drawCmds, type);
    EXPECT_TRUE(res);

    node->isNodeSingleFrameComposer_ = true;
    res = RSCanvasNodeCommandHelper::AddCmdToSingleFrameComposer(node, drawCmds, type);
    EXPECT_TRUE(res == false);

    std::thread::id threadId = std::this_thread::get_id();
    RSSingleFrameComposer::SetSingleFrameFlag(threadId);
    res = RSCanvasNodeCommandHelper::AddCmdToSingleFrameComposer(node, drawCmds, type);
    EXPECT_TRUE(res == false);
}

/**
 * @tc.name: TestUpdateRecording01
 * @tc.desc: Verify function UpdateRecording
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasNodeCommandUnitTest, TestUpdateRecording01, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    RSCanvasNodeCommandHelper::Create(context, id, true);
    std::shared_ptr<Drawing::DrawCmdList> drawCmds = nullptr;
    uint16_t modifierType = 1;
    RSCanvasNodeCommandHelper::UpdateRecording(context, id, drawCmds, modifierType);
    EXPECT_TRUE(drawCmds == nullptr);

    RSCanvasNodeCommandHelper::UpdateRecording(context, 0, drawCmds, modifierType);
    EXPECT_TRUE(drawCmds == nullptr);

    drawCmds = std::make_shared<Drawing::DrawCmdList>();
    RSCanvasNodeCommandHelper::UpdateRecording(context, id, drawCmds, modifierType);
    EXPECT_TRUE(drawCmds != nullptr);
}

/**
 * @tc.name: ClearRecording01
 * @tc.desc: Verify function ClearRecording
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasNodeCommandUnitTest, TestClearRecording01, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    RSCanvasNodeCommandHelper::Create(context, id, true);
    RSCanvasNodeCommandHelper::ClearRecording(context, id);

    RSCanvasNodeCommandHelper::ClearRecording(context, 0);
    EXPECT_TRUE(id);
}
} // namespace OHOS::Rosen
