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
    RSDisplayNodeConfig config { 0, DisplayMode::EXPAND, 0 };
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

    RSDisplayNodeConfig config { 0, DisplayMode::EXPAND, 0 };
    DisplayNodeCommandHelper::Create(context, id, config);
    DisplayNodeCommandHelper::SetSecurityDisplay(context, id, isSecurityDisplay);
    EXPECT_NE(context.GetNodeMap().GetRenderNode<RSLogicalDisplayRenderNode>(id), nullptr);
}

/**
 * @tc.name: SetDisplayModeTest001
 * @tc.desc: Test SetDisplayMode
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayNodeCommandTest, SetDisplayModeTest001, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    RSDisplayNodeConfig config { 0, DisplayMode::MIRROR, 0 };
    DisplayNodeCommandHelper::SetDisplayMode(context, id, config);
    EXPECT_EQ(context.GetNodeMap().GetRenderNode<RSLogicalDisplayRenderNode>(id), nullptr);
}

/**
 * @tc.name: SetDisplayModeTest002
 * @tc.desc: SetScreenId test.
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayNodeCommandTest, SetDisplayModeTest002, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    RSDisplayNodeConfig config { 0, DisplayMode::EXPAND, 0 };
    DisplayNodeCommandHelper::SetDisplayMode(context, id, config);
    EXPECT_EQ(context.GetNodeMap().GetRenderNode<RSLogicalDisplayRenderNode>(id), nullptr);

    DisplayNodeCommandHelper::Create(context, id, config);
    DisplayNodeCommandHelper::SetDisplayMode(context, id, config);

    config.displayMode = DisplayMode::MIRROR;
    DisplayNodeCommandHelper::SetDisplayMode(context, id, config);

    NodeId mirrorNodeId = static_cast<NodeId>(2);
    config.mirrorNodeId = mirrorNodeId;
    DisplayNodeCommandHelper::Create(context, mirrorNodeId, config);
    DisplayNodeCommandHelper::SetDisplayMode(context, id, config);
}

/**
 * @tc.name: SetDisplayModeTest003
 * @tc.desc: Test SetDisplayMode with positionZ, screenRenderNode exists (if branch true)
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayNodeCommandTest, SetDisplayModeTest003, TestSize.Level1)
{
    RSContext context;
    NodeId screenNodeId = static_cast<NodeId>(100);
    NodeId displayNodeId = static_cast<NodeId>(1);

    auto screenRenderNode = std::make_shared<RSScreenRenderNode>(screenNodeId, 1, context.weak_from_this());
    context.GetMutableNodeMap().RegisterRenderNode(screenRenderNode);

    RSDisplayNodeConfig config;
    config.screenId = 1;
    DisplayNodeCommandHelper::Create(context, displayNodeId, config);
    DisplayNodeCommandHelper::AddDisplayNodeToTree(context, displayNodeId);
    DisplayNodeCommandHelper::SetDisplayMode(context, displayNodeId, config);

    config.positionZ = 128;
    DisplayNodeCommandHelper::SetDisplayMode(context, displayNodeId, config);
    EXPECT_NE(context.GetNodeMap().GetRenderNode<RSLogicalDisplayRenderNode>(displayNodeId), nullptr);
}

/**
 * @tc.name: SetDisplayModeTest004
 * @tc.desc: Test SetDisplayMode with positionZ, screenRenderNode null (if branch false)
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayNodeCommandTest, SetDisplayModeTest004, TestSize.Level1)
{
    RSContext context;
    NodeId displayNodeId = static_cast<NodeId>(1);

    RSDisplayNodeConfig config;
    config.positionZ = 128;

    DisplayNodeCommandHelper::SetDisplayMode(context, displayNodeId, config);
    EXPECT_EQ(context.GetNodeMap().GetRenderNode<RSLogicalDisplayRenderNode>(displayNodeId), nullptr);
}

/**
 * @tc.name: SetDisplayModeTest005
 * @tc.desc: Test SetDisplayMode with NAN positionZ (should be rejected)
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayNodeCommandTest, SetDisplayModeTest005, TestSize.Level1)
{
    RSContext context;
    NodeId screenNodeId = static_cast<NodeId>(100);
    NodeId displayNodeId = static_cast<NodeId>(1);

    auto screenRenderNode = std::make_shared<RSScreenRenderNode>(screenNodeId, 1, context.weak_from_this());
    context.GetMutableNodeMap().RegisterRenderNode(screenRenderNode);

    RSDisplayNodeConfig config;
    config.screenId = 1;
    DisplayNodeCommandHelper::Create(context, displayNodeId, config);
    DisplayNodeCommandHelper::AddDisplayNodeToTree(context, displayNodeId);

    float originalZ = screenRenderNode->GetRenderProperties().GetPositionZ();
    config.positionZ = NAN;
    DisplayNodeCommandHelper::SetDisplayMode(context, displayNodeId, config);
    // positionZ should NOT be applied when NAN
    EXPECT_EQ(screenRenderNode->GetRenderProperties().GetPositionZ(), originalZ);
}

/**
 * @tc.name: SetDisplayModeTest006
 * @tc.desc: Test SetDisplayMode with Inf positionZ (should be rejected)
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayNodeCommandTest, SetDisplayModeTest006, TestSize.Level1)
{
    RSContext context;
    NodeId screenNodeId = static_cast<NodeId>(100);
    NodeId displayNodeId = static_cast<NodeId>(1);

    auto screenRenderNode = std::make_shared<RSScreenRenderNode>(screenNodeId, 1, context.weak_from_this());
    context.GetMutableNodeMap().RegisterRenderNode(screenRenderNode);

    RSDisplayNodeConfig config;
    config.screenId = 1;
    DisplayNodeCommandHelper::Create(context, displayNodeId, config);
    DisplayNodeCommandHelper::AddDisplayNodeToTree(context, displayNodeId);

    float originalZ = screenRenderNode->GetRenderProperties().GetPositionZ();
    config.positionZ = INFINITY;
    DisplayNodeCommandHelper::SetDisplayMode(context, displayNodeId, config);
    // positionZ should NOT be applied when Inf
    EXPECT_EQ(screenRenderNode->GetRenderProperties().GetPositionZ(), originalZ);
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
    RSDisplayNodeConfig config { 0, DisplayMode::EXPAND, 0 };
    DisplayNodeCommandHelper::Create(context, id, config);
    EXPECT_NE(context.GetNodeMap().GetRenderNode<RSLogicalDisplayRenderNode>(id), nullptr);

    NodeId id2 = static_cast<NodeId>(2);
    config.displayMode = DisplayMode::MIRROR;
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
    RSDisplayNodeConfig config { 0, DisplayMode::MIRROR, 0 };
    DisplayNodeCommandHelper::Create(context, id, config);
    DisplayNodeCommandHelper::SetBootAnimation(context, id, true);

    DisplayNodeCommandHelper::SetBootAnimation(context, 5, true);
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

    RSDisplayNodeConfig config { 0, DisplayMode::EXPAND, 0 };
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
    RSDisplayNodeConfig config { 0, DisplayMode::EXPAND, 0 };
    DisplayNodeCommandHelper::SetDisplayMode(context, id, config);
    EXPECT_EQ(context.GetNodeMap().GetRenderNode<RSLogicalDisplayRenderNode>(id), nullptr);

    DisplayNodeCommandHelper::Create(context, id, config);
    DisplayNodeCommandHelper::SetDisplayMode(context, id, config);

    config.displayMode = DisplayMode::MIRROR;
    DisplayNodeCommandHelper::SetDisplayMode(context, id, config);

    NodeId mirrorNodeId = static_cast<NodeId>(2);
    config.mirrorNodeId = mirrorNodeId;
    DisplayNodeCommandHelper::Create(context, mirrorNodeId, config);
    DisplayNodeCommandHelper::SetDisplayMode(context, id, config);
}

/**
 * @tc.name: ClearModifiersByPidTest001
 * @tc.desc: SetScreenId test.
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayNodeCommandTest, ClearModifiersByPidTest001, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    int32_t pid = getpid();
    DisplayNodeCommandHelper::ClearModifiersByPid(context, id, pid);
    EXPECT_EQ(context.GetNodeMap().GetRenderNode<RSLogicalDisplayRenderNode>(id), nullptr);
}

/**
 * @tc.name: ClearModifiersByPidTest002
 * @tc.desc: SetScreenId test.
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayNodeCommandTest, ClearModifiersByPidTest002, TestSize.Level1)
{
    RSContext context;
    int32_t pid = getpid();
    NodeId id = static_cast<NodeId>(1);
    RSDisplayNodeConfig config;
    std::shared_ptr<RSLogicalDisplayRenderNode> renderNode = std::make_shared<RSLogicalDisplayRenderNode>(id, config);
    context.GetMutableNodeMap().RegisterRenderNode(renderNode);
    DisplayNodeCommandHelper::ClearModifiersByPid(context, id, pid);
    EXPECT_FALSE(!context.GetNodeMap().GetRenderNode<RSLogicalDisplayRenderNode>(id));
}

/**
 * @tc.name: SetVirtualScreenMuteStatus001
 * @tc.desc: SetVirtualScreenMuteStatus test.
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSDisplayNodeCommandTest, SetVirtualScreenMuteStatus001, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    RSDisplayNodeConfig config { 0, DisplayMode::MIRROR, 0 };
    DisplayNodeCommandHelper::Create(context, id, config);
    EXPECT_NE(context.GetNodeMap().GetRenderNode<RSLogicalDisplayRenderNode>(id), nullptr);

    bool virtualScreenMuteStatus = false;
    DisplayNodeCommandHelper::SetVirtualScreenMuteStatus(context, 5, virtualScreenMuteStatus);
}

/**
 * @tc.name: CreateInvalidRotation001
 * @tc.desc: Create rejects out-of-range mirrorSourceRotation.
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayNodeCommandTest, CreateInvalidRotation001, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    RSDisplayNodeConfig config;
    config.mirrorSourceRotation = static_cast<uint32_t>(ScreenRotation::INVALID_SCREEN_ROTATION) + 1;
    DisplayNodeCommandHelper::Create(context, id, config);
    EXPECT_EQ(context.GetNodeMap().GetRenderNode<RSLogicalDisplayRenderNode>(id), nullptr);
}

/**
 * @tc.name: SetDisplayModeInvalidRotation001
 * @tc.desc: SetDisplayMode rejects out-of-range mirrorSourceRotation without crash.
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayNodeCommandTest, SetDisplayModeInvalidRotation001, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    RSDisplayNodeConfig validConfig { 0, DisplayMode::MIRROR, 0 };
    DisplayNodeCommandHelper::Create(context, id, validConfig);
    ASSERT_NE(context.GetNodeMap().GetRenderNode<RSLogicalDisplayRenderNode>(id), nullptr);

    RSDisplayNodeConfig badConfig { 0, DisplayMode::MIRROR, 0 };
    badConfig.mirrorSourceRotation = static_cast<uint32_t>(ScreenRotation::INVALID_SCREEN_ROTATION) + 1;
    DisplayNodeCommandHelper::SetDisplayMode(context, id, badConfig);
    EXPECT_NE(context.GetNodeMap().GetRenderNode<RSLogicalDisplayRenderNode>(id), nullptr);
}
} // namespace OHOS::Rosen
