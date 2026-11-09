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
#include "include/command/rs_canvas_node_command.h"
#include "common/rs_common_def.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_render_node_allocator.h"
#include "pipeline/rs_render_node_gc.h"
#include "parameters.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSCanvasNodeCommandTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSCanvasNodeCommandTest::SetUpTestCase() {}
void RSCanvasNodeCommandTest::TearDownTestCase() {}
void RSCanvasNodeCommandTest::SetUp() {}
void RSCanvasNodeCommandTest::TearDown() {}

/**
 * @tc.name: TestRSCanvasNodeCommand001
 * @tc.desc: UpdateRecording test.
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasNodeCommandTest, TestRSCanvasNodeCommand001, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(-1);
    std::shared_ptr<Drawing::DrawCmdList> drawCmds = nullptr;
    ModifierNG::RSModifierType type = ModifierNG::RSModifierType::INVALID;
    EXPECT_EQ(context.GetNodeMap().GetRenderNode<RSCanvasRenderNode>(nodeId), nullptr);
    RSCanvasNodeCommandHelper::UpdateRecording(context, nodeId, drawCmds, static_cast<uint16_t>(type));
}

/**
 * @tc.name: TestRSCanvasNodeCommand002
 * @tc.desc: ClearRecording test.
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasNodeCommandTest, TestRSCanvasNodeCommand002, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(-1);
    RSCanvasNodeCommandHelper::ClearRecording(context, nodeId);
    ASSERT_EQ(context.GetNodeMap().GetRenderNode<RSCanvasRenderNode>(nodeId), nullptr);
}

/**
 * @tc.name: Create001
 * @tc.desc: Create test.
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasNodeCommandTest, Create001, TestSize.Level1)
{
    RSContext context;
    NodeId targetId = static_cast<NodeId>(-1);
    RSCanvasNodeCommandHelper::Create(context, targetId, false);
    EXPECT_NE(context.GetNodeMap().GetRenderNode<RSCanvasRenderNode>(targetId), nullptr);
    context.GetMutableNodeMap().UnregisterRenderNode(targetId);
}

/**
 * @tc.name: Create002
 * @tc.desc: test results of Create
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSCanvasNodeCommandTest, Create002, TestSize.Level1)
{
    RSContext context;
    NodeId targetId = 0;
    RSCanvasNodeCommandHelper::Create(context, targetId, false);
    EXPECT_EQ(context.GetNodeMap().GetRenderNode<RSCanvasRenderNode>(targetId), nullptr);
    context.GetMutableNodeMap().UnregisterRenderNode(targetId);
}

/**
 * @tc.name: AddCmdToSingleFrameComposer001
 * @tc.desc: test results of AddCmdToSingleFrameComposer
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSCanvasNodeCommandTest, AddCmdToSingleFrameComposer001, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    RSCanvasNodeCommandHelper::Create(context, id, true);
    auto node = context.GetNodeMap().GetRenderNode<RSCanvasRenderNode>(id);
    SimpleDrawCmdListPtr drawCmds = nullptr;
    uint16_t type = 0;
    bool res = RSCanvasNodeCommandHelper::AddCmdToSingleFrameComposer(node, drawCmds, type);
    EXPECT_TRUE(res == false);

    std::thread::id thisThreadId = std::this_thread::get_id();
    RSSingleFrameComposer::SetSingleFrameFlag(thisThreadId);
    res = RSCanvasNodeCommandHelper::AddCmdToSingleFrameComposer(node, drawCmds, type);
    EXPECT_TRUE(res);
}

/**
 * @tc.name: UpdateRecording001
 * @tc.desc: test results of UpdateRecording
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSCanvasNodeCommandTest, UpdateRecording001, TestSize.Level1)
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
    EXPECT_TRUE(drawCmds == nullptr);
    context.GetMutableNodeMap().UnregisterRenderNode(id);
}

/**
 * @tc.name: UpdateRecordingTest001
 * @tc.desc: test results of UpdateRecordingTest001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasNodeCommandTest, UpdateRecordingTest001, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    RSCanvasNodeCommandHelper::Create(context, id, true);
    std::shared_ptr<Drawing::DrawCmdList> drawCmds = std::make_shared<Drawing::DrawCmdList>();
    uint16_t modifierType = 1;
    RSCanvasNodeCommandHelper::UpdateRecording(context, -102, drawCmds, modifierType);
    EXPECT_TRUE(drawCmds != nullptr);
}
/**
 * @tc.name: ClearRecording001
 * @tc.desc: test results of ClearRecording
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSCanvasNodeCommandTest, ClearRecording001, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    RSCanvasNodeCommandHelper::Create(context, id, true);
    RSCanvasNodeCommandHelper::ClearRecording(context, id);

    RSCanvasNodeCommandHelper::ClearRecording(context, 0);
    EXPECT_TRUE(id);
    context.GetMutableNodeMap().UnregisterRenderNode(id);
}

/**
 * @tc.name: SetColorGamut001
 * @tc.desc: test results of SetColorGamut
 * @tc.type: FUNC
 * @tc.require: issueICGKPE
 */
HWTEST_F(RSCanvasNodeCommandTest, SetColorGamut001, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    RSCanvasNodeCommandHelper::Create(context, id, true);
    auto node = context.GetNodeMap().GetRenderNode<RSCanvasRenderNode>(id);

    RSCanvasNodeCommandHelper::SetColorGamut(context, id, 4); // 4 is SRGB
#ifdef RS_ENABLE_UNI_RENDER
    ASSERT_EQ(node->colorGamut_, 4); // 4 is SRGB
    RSCanvasNodeCommandHelper::SetColorGamut(context, id, 3); // 3 is DISPLAY_P3
    ASSERT_EQ(node->colorGamut_, 3); // 3 is DISPLAY_P3
#endif
}

/**
 * @tc.name: SetPixelmap
 * @tc.desc: test results of SetPixelmap
 * @tc.type: FUNC
 * @tc.require: issueICGKPE
 */
HWTEST_F(RSCanvasNodeCommandTest, SetPixelmap, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    RSCanvasNodeCommandHelper::SetPixelmap(context, id, nullptr);
    auto pixelmap = std::make_shared<OHOS::Media::PixelMap>();
    EXPECT_NE(pixelmap, nullptr);
    RSCanvasNodeCommandHelper::SetPixelmap(context, id, pixelmap);
    RSCanvasNodeCommandHelper::Create(context, id, true);
    auto node = context.GetNodeMap().GetRenderNode<RSCanvasRenderNode>(id);
    EXPECT_NE(node, nullptr);
    RSCanvasNodeCommandHelper::SetPixelmap(context, id, pixelmap);
    context.GetMutableNodeMap().UnregisterRenderNode(id);
}

/**
 * @tc.name: UpdateRecordingModifierTypeOutOfRange001
 * @tc.desc: test UpdateRecording with modifierType exceeding ModifierNG::RSModifierType::MAX
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasNodeCommandTest, UpdateRecordingModifierTypeOutOfRange001, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    RSCanvasNodeCommandHelper::Create(context, id, true);
    std::shared_ptr<Drawing::DrawCmdList> drawCmds = std::make_shared<Drawing::DrawCmdList>();
    uint16_t outOfRangeType = static_cast<uint16_t>(ModifierNG::RSModifierType::MAX);
    RSCanvasNodeCommandHelper::UpdateRecording(context, id, drawCmds, outOfRangeType);
    EXPECT_TRUE(drawCmds == nullptr);
    context.GetMutableNodeMap().UnregisterRenderNode(id);
}

/**
 * @tc.name: AddCmdToSingleFrameComposerModifierTypeOutOfRange001
 * @tc.desc: test AddCmdToSingleFrameComposer with modifierType exceeding ModifierNG::RSModifierType::MAX
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasNodeCommandTest, AddCmdToSingleFrameComposerModifierTypeOutOfRange001, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    RSCanvasNodeCommandHelper::Create(context, id, true);
    auto node = context.GetNodeMap().GetRenderNode<RSCanvasRenderNode>(id);
    SimpleDrawCmdListPtr drawCmds = nullptr;
    uint16_t outOfRangeType = static_cast<uint16_t>(ModifierNG::RSModifierType::MAX);
    bool res = RSCanvasNodeCommandHelper::AddCmdToSingleFrameComposer(node, drawCmds, outOfRangeType);
    EXPECT_FALSE(res);
    context.GetMutableNodeMap().UnregisterRenderNode(id);
}
/**
 * @tc.name: CreateScbRoute001
 * @tc.desc: test Create routes to CreateScbCanvasRenderNode when the SCB pool is enabled
 *           and the node's pid equals scbPid_ (the useScbPool==true branch).
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasNodeCommandTest, CreateScbRoute001, TestSize.Level1)
{
    RSContext context;
    auto& nodeGC = RSRenderNodeGC::Instance();
    auto& allocator = RSRenderNodeAllocator::Instance();
    pid_t originalPid = nodeGC.GetScbPid();
 
    constexpr pid_t scbPid = 42;
    nodeGC.SetScbPid(scbPid);
    // Ensure the SCB pool switch is enabled via system property.
    system::SetParameter("persist.debug.rs.scb_pool_enabled", "1");
    // NodeId high 32 bits encode the pid -> ExtractPid(id) == scbPid
    NodeId id = (static_cast<NodeId>(scbPid) << 32) | 1;
    RSCanvasNodeCommandHelper::Create(context, id, false);
    // Restore global state immediately so a failed assertion below cannot leak it.
    nodeGC.SetScbPid(originalPid);
    system::SetParameter("persist.debug.rs.scb_pool_enabled", "1");
 
    auto node = context.GetNodeMap().GetRenderNode<RSCanvasRenderNode>(id);
    ASSERT_NE(node, nullptr);
    // Node must have been allocated from the SCB contiguous pool
    EXPECT_TRUE(allocator.IsScbNodePool(node.get()));
 
    context.GetMutableNodeMap().UnregisterRenderNode(id);
}
 
/**
 * @tc.name: CreateScbRouteDisabled001
 * @tc.desc: test Create routes to the generic path when the SCB pool switch is off
 *           even if the pid matches scbPid_ (the useScbPool==false branch via the flag).
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasNodeCommandTest, CreateScbRouteDisabled001, TestSize.Level1)
{
    RSContext context;
    auto& nodeGC = RSRenderNodeGC::Instance();
    auto& allocator = RSRenderNodeAllocator::Instance();
    pid_t originalPid = nodeGC.GetScbPid();
 
    constexpr pid_t scbPid = 43;
    nodeGC.SetScbPid(scbPid);
    // Disable the SCB pool switch via system property.
    system::SetParameter("persist.debug.rs.scb_pool_enabled", "0");
    NodeId id = (static_cast<NodeId>(scbPid) << 32) | 2;
    RSCanvasNodeCommandHelper::Create(context, id, false);
    // Restore global state immediately so a failed assertion below cannot leak it.
    nodeGC.SetScbPid(originalPid);
    system::SetParameter("persist.debug.rs.scb_pool_enabled", "1");
 
    auto node = context.GetNodeMap().GetRenderNode<RSCanvasRenderNode>(id);
    ASSERT_NE(node, nullptr);
    // Switch off -> must NOT come from the SCB pool
    EXPECT_FALSE(allocator.IsScbNodePool(node.get()));
 
    context.GetMutableNodeMap().UnregisterRenderNode(id);
}
 
/**
 * @tc.name: CreateUnRegisterUnTreeNode001
 * @tc.desc: test Create takes the UnRegisterUnTreeNode==true branch (node pre-registered
 *           as an un-tree node) and still registers the node successfully.
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasNodeCommandTest, CreateUnRegisterUnTreeNode001, TestSize.Level1)
{
    RSContext context;
    NodeId id = (static_cast<NodeId>(7) << 32) | 3;
    // Pre-register the node as an un-tree node so UnRegisterUnTreeNode returns true
    context.GetMutableNodeMap().RegisterUnTreeNode(id);
    RSCanvasNodeCommandHelper::Create(context, id, false);
    auto node = context.GetNodeMap().GetRenderNode<RSCanvasRenderNode>(id);
    ASSERT_NE(node, nullptr);
    // The un-tree registration must have been consumed by Create
    EXPECT_FALSE(context.GetMutableNodeMap().UnRegisterUnTreeNode(id));
    context.GetMutableNodeMap().UnregisterRenderNode(id);
}
 
} // namespace OHOS::Rosen
