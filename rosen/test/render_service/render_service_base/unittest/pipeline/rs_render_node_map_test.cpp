/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#include "animation/rs_render_curve_animation.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_render_node_map.h"
#include "pipeline/rs_surface_handler.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_logical_display_render_node.h"
#include "pipeline/rs_context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderNodeMapTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline NodeId id;
    static inline std::weak_ptr<RSContext> context = {};
};

void RSRenderNodeMapTest::SetUpTestCase() {}
void RSRenderNodeMapTest::TearDownTestCase() {}
void RSRenderNodeMapTest::SetUp() {}
void RSRenderNodeMapTest::TearDown() {}

/**
 * @tc.name: ObtainScreenLockWindowNodeIdTest
 * @tc.desc: test results of ObtainScreenLockWindowNodeIdTest
 * @tc.type:FUNC
 * @tc.require: issueI9VAI2
 */
HWTEST_F(RSRenderNodeMapTest, ObtainScreenLockWindowNodeIdTest, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> surfaceNode = nullptr;
    RSRenderNodeMap rsRenderNodeMap;
    rsRenderNodeMap.ObtainScreenLockWindowNodeId(surfaceNode);
    NodeId id = 1;
    surfaceNode = std::make_shared<RSSurfaceRenderNode>(id);
    rsRenderNodeMap.ObtainScreenLockWindowNodeId(surfaceNode);
    ASSERT_EQ(rsRenderNodeMap.screenLockWindowNodeId_, 0);

    RSSurfaceRenderNodeConfig config = { .id = id, .surfaceWindowType = SurfaceWindowType::SCB_SCREEN_LOCK };
    auto node = std::make_shared<RSSurfaceRenderNode>(config);
    rsRenderNodeMap.ObtainScreenLockWindowNodeId(node);
    ASSERT_EQ(rsRenderNodeMap.screenLockWindowNodeId_, 1);
}

/**
 * @tc.name: ObtainLauncherNodeId
 * @tc.desc: test results of ObtainLauncherNodeId
 * @tc.type:FUNC
 * @tc.require: issueI9VAI2
 */
HWTEST_F(RSRenderNodeMapTest, ObtainLauncherNodeId, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> surfaceNode = nullptr;
    RSRenderNodeMap rsRenderNodeMap;
    rsRenderNodeMap.ObtainLauncherNodeId(surfaceNode);
    ASSERT_EQ(rsRenderNodeMap.GetEntryViewNodeId(), 0);

    NodeId id = 1;
    RSSurfaceRenderNodeConfig config = { .id = id, .surfaceWindowType = SurfaceWindowType::SCB_DESKTOP };
    auto node = std::make_shared<RSSurfaceRenderNode>(config);
    rsRenderNodeMap.ObtainLauncherNodeId(node);

    config.surfaceWindowType = SurfaceWindowType::SCB_WALLPAPER;
    node = std::make_shared<RSSurfaceRenderNode>(config);
    rsRenderNodeMap.ObtainLauncherNodeId(node);

    config.surfaceWindowType = SurfaceWindowType::SCB_NEGATIVE_SCREEN;
    node = std::make_shared<RSSurfaceRenderNode>(config);
    rsRenderNodeMap.ObtainLauncherNodeId(node);

    ASSERT_EQ(rsRenderNodeMap.entryViewNodeId_, 1);
    ASSERT_EQ(rsRenderNodeMap.wallpaperViewNodeId_, 1);
    ASSERT_EQ(rsRenderNodeMap.negativeScreenNodeId_, 1);
}

/**
 * @tc.name: GetVisibleLeashWindowCountTest
 * @tc.desc: test results of GetVisibleLeashWindowCount
 * @tc.type:FUNC
 * @tc.require: issueI9VAI2
 */
HWTEST_F(RSRenderNodeMapTest, GetVisibleLeashWindowCountTest, TestSize.Level1)
{
    RSRenderNodeMap rsRenderNodeMap;
    rsRenderNodeMap.GetVisibleLeashWindowCount();
    EXPECT_TRUE(rsRenderNodeMap.surfaceNodeMap_.empty());
    NodeId id = 1;
    auto rssurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(id);
    rsRenderNodeMap.surfaceNodeMap_[id] = rssurfaceRenderNode;
    rsRenderNodeMap.GetVisibleLeashWindowCount();
    EXPECT_FALSE(rsRenderNodeMap.surfaceNodeMap_.empty());
}

/**
 * @tc.name: RegisterRenderNode
 * @tc.desc: test results of RegisterRenderNode
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSRenderNodeMapTest, RegisterRenderNode, TestSize.Level1)
{
    NodeId id = 1;
    auto node = std::make_shared<OHOS::Rosen::RSRenderNode>(id);
    RSRenderNodeMap rsRenderNodeMap;
    rsRenderNodeMap.RegisterRenderNode(node);
    auto result = rsRenderNodeMap.RegisterRenderNode(node);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: UnregisterRenderNodeTest
 * @tc.desc: test results of UnregisterRenderNode
 * @tc.type:FUNC
 * @tc.require: issueI9VAI2
 */
HWTEST_F(RSRenderNodeMapTest, UnregisterRenderNodeTest, TestSize.Level1)
{
    RSRenderNodeMap rsRenderNodeMap;
    NodeId id = 0;
    rsRenderNodeMap.UnregisterRenderNode(id);
    auto rssurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(id);
    rsRenderNodeMap.surfaceNodeMap_[id] = rssurfaceRenderNode;
    rsRenderNodeMap.UnregisterRenderNode(id);
    rssurfaceRenderNode->name_ = "ShellAssistantAnco";
    rsRenderNodeMap.UnregisterRenderNode(id);
    EXPECT_TRUE(rsRenderNodeMap.renderNodeMap_.empty());
}

/**
 * @tc.name: MoveRenderNodeMap
 * @tc.desc: test results of MoveRenderNodeMap
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSRenderNodeMapTest, MoveRenderNodeMap, TestSize.Level1)
{
    NodeId id = 1;
    auto node = std::make_shared<OHOS::Rosen::RSRenderNode>(id);
    RSRenderNodeMap rsRenderNodeMap;
    auto subRenderNodeMap = std::make_shared<std::unordered_map<NodeId, std::shared_ptr<RSBaseRenderNode>>>();
    pid_t pid = 1;
    rsRenderNodeMap.MoveRenderNodeMap(subRenderNodeMap, pid);
    rsRenderNodeMap.renderNodeMap_[pid][id] = node;
    rsRenderNodeMap.MoveRenderNodeMap(subRenderNodeMap, pid);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: FilterNodeByPid
 * @tc.desc: test results of FilterNodeByPid
 * @tc.type:FUNC
 * @tc.require: issueI9VAI2
 */
HWTEST_F(RSRenderNodeMapTest, FilterNodeByPid, TestSize.Level1)
{
    NodeId id = 0;
    ScreenId screenId = 1;
    pid_t pid = ExtractPid(id);
    auto node = std::make_shared<OHOS::Rosen::RSRenderNode>(id);
    RSRenderNodeMap rsRenderNodeMap;
    rsRenderNodeMap.FilterNodeByPid(1);

    rsRenderNodeMap.renderNodeMap_[pid][id] = node;
    rsRenderNodeMap.FilterNodeByPid(1);
    auto screenRenderNode = std::make_shared<RSScreenRenderNode>(id, screenId, context);
    rsRenderNodeMap.screenNodeMap_.emplace(id, screenRenderNode);
    rsRenderNodeMap.FilterNodeByPid(1);
    rsRenderNodeMap.renderNodeMap_.clear();
    rsRenderNodeMap.FilterNodeByPid(1);
    EXPECT_TRUE(true);
}

/*
 * @tc.name: FilterNodeByPid
 * @tc.desc: Test FilterNodeByPid
 * @tc.require:
 */
HWTEST_F(RSRenderNodeMapTest, FilterNodeByPidImmediate, Level1)
{
    auto renderNodeMap = RSRenderNodeMap();
    auto displayId = 1;
    auto displayRenderNode = std::make_shared<RSScreenRenderNode>(displayId, 0, context);
    renderNodeMap.RegisterRenderNode(displayRenderNode);

    constexpr uint32_t bits = 32u;
    constexpr uint64_t nodeId = 1;
    constexpr uint64_t pid1 = 1;
    constexpr uint64_t pid2 = 2;

    auto registerNode = [&renderNodeMap](uint64_t pid, uint64_t nodeId) {
        auto renderNodeId = NodeId((pid << bits) | nodeId);
        auto renderNode = std::make_shared<RSRenderNode>(renderNodeId);
        renderNodeMap.RegisterRenderNode(renderNode);
    };

    registerNode(pid1, nodeId);
    registerNode(pid2, nodeId);

    EXPECT_EQ(renderNodeMap.GetSize(), 4);
    renderNodeMap.FilterNodeByPid(pid1, true);
    EXPECT_EQ(renderNodeMap.GetSize(), 3);
    renderNodeMap.FilterNodeByPid(pid2, true);
    EXPECT_EQ(renderNodeMap.GetSize(), 2);
}

/**
 * @tc.name: FilterNodeByPid
 * @tc.desc: test results of GetRenderNode
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSRenderNodeMapTest, GetRenderNode, TestSize.Level1)
{
    NodeId id = 0;
    auto node = std::make_shared<OHOS::Rosen::RSRenderNode>(id);
    RSRenderNodeMap rsRenderNodeMap;
    rsRenderNodeMap.GetRenderNode(1);
    EXPECT_EQ(rsRenderNodeMap.GetRenderNode(1), nullptr);
}

/**
 * @tc.name: GetAnimationFallbackNode
 * @tc.desc: test results of GetAnimationFallbackNode
 * @tc.type:FUNC
 * @tc.require: issueI9VAI2
 */
HWTEST_F(RSRenderNodeMapTest, GetAnimationFallbackNode, TestSize.Level1)
{
    NodeId id = 0;
    pid_t pid = ExtractPid(id);
    auto node = std::make_shared<OHOS::Rosen::RSRenderNode>(id);
    RSRenderNodeMap rsRenderNodeMap;
    rsRenderNodeMap.renderNodeMap_.clear();
    rsRenderNodeMap.GetAnimationFallbackNode();

    rsRenderNodeMap.renderNodeMap_[pid][id] = node;
    EXPECT_NE(rsRenderNodeMap.GetAnimationFallbackNode(), nullptr);
}

/**
 * @tc.name: IsUIExtensionSurfaceNode001
 * @tc.desc: test results of IsUIExtensionSurfaceNode
 * @tc.type: FUNC
 * @tc.require: issueIAI1VN
 */
HWTEST_F(RSRenderNodeMapTest, IsUIExtensionSurfaceNode001, TestSize.Level1)
{
    NodeId id = 1;
    auto node = std::make_shared<OHOS::Rosen::RSRenderNode>(id);
    EXPECT_NE(node, nullptr);
    RSRenderNodeMap rsRenderNodeMap;
    bool isUIExtensionSurfaceNode = rsRenderNodeMap.IsUIExtensionSurfaceNode(id);
    EXPECT_FALSE(isUIExtensionSurfaceNode);
    bool isRegisterSuccess = rsRenderNodeMap.RegisterRenderNode(node);
    EXPECT_TRUE(isRegisterSuccess);
    isUIExtensionSurfaceNode = rsRenderNodeMap.IsUIExtensionSurfaceNode(id);
    EXPECT_FALSE(isUIExtensionSurfaceNode);
    rsRenderNodeMap.UnregisterRenderNode(id);
    isUIExtensionSurfaceNode = rsRenderNodeMap.IsUIExtensionSurfaceNode(id);
    EXPECT_FALSE(isUIExtensionSurfaceNode);
}

/**
 * @tc.name: IsUIExtensionSurfaceNode002
 * @tc.desc: test results of IsUIExtensionSurfaceNode
 * @tc.type: FUNC
 * @tc.require: issueIAI1VN
 */
HWTEST_F(RSRenderNodeMapTest, IsUIExtensionSurfaceNode002, TestSize.Level1)
{
    NodeId id = 1;
    RSSurfaceRenderNodeConfig config = { .id = id };
    auto node = std::make_shared<RSSurfaceRenderNode>(config);
    EXPECT_NE(node, nullptr);
    RSRenderNodeMap rsRenderNodeMap;
    bool isRegisterSuccess = rsRenderNodeMap.RegisterRenderNode(node);
    EXPECT_TRUE(isRegisterSuccess);
    bool isUIExtensionSurfaceNode = rsRenderNodeMap.IsUIExtensionSurfaceNode(id);
    EXPECT_FALSE(isUIExtensionSurfaceNode);
    rsRenderNodeMap.UnregisterRenderNode(id);
    isUIExtensionSurfaceNode = rsRenderNodeMap.IsUIExtensionSurfaceNode(id);
    EXPECT_FALSE(isUIExtensionSurfaceNode);
}

/**
 * @tc.name: IsUIExtensionSurfaceNode003
 * @tc.desc: test results of IsUIExtensionSurfaceNode
 * @tc.type: FUNC
 * @tc.require: issueIAI1VN
 */
HWTEST_F(RSRenderNodeMapTest, IsUIExtensionSurfaceNode003, TestSize.Level1)
{
    NodeId id = 1;
    RSSurfaceRenderNodeConfig config = { .id = id, .nodeType = RSSurfaceNodeType::UI_EXTENSION_COMMON_NODE };
    auto node = std::make_shared<RSSurfaceRenderNode>(config);
    EXPECT_NE(node, nullptr);
    RSRenderNodeMap rsRenderNodeMap;
    bool isRegisterSuccess = rsRenderNodeMap.RegisterRenderNode(node);
    EXPECT_TRUE(isRegisterSuccess);
    bool isUIExtensionSurfaceNode = rsRenderNodeMap.IsUIExtensionSurfaceNode(id);
    EXPECT_TRUE(isUIExtensionSurfaceNode);
    rsRenderNodeMap.UnregisterRenderNode(id);
    isUIExtensionSurfaceNode = rsRenderNodeMap.IsUIExtensionSurfaceNode(id);
    EXPECT_FALSE(isUIExtensionSurfaceNode);
}

/**
 * @tc.name: IsUIExtensionSurfaceNode004
 * @tc.desc: test results of IsUIExtensionSurfaceNode
 * @tc.type: FUNC
 * @tc.require: issueIAI1VN
 */
HWTEST_F(RSRenderNodeMapTest, IsUIExtensionSurfaceNode004, TestSize.Level1)
{
    NodeId id = 1;
    RSSurfaceRenderNodeConfig config = { .id = id, .nodeType = RSSurfaceNodeType::UI_EXTENSION_SECURE_NODE };
    auto node = std::make_shared<RSSurfaceRenderNode>(config);
    EXPECT_NE(node, nullptr);
    RSRenderNodeMap rsRenderNodeMap;
    bool isRegisterSuccess = rsRenderNodeMap.RegisterRenderNode(node);
    EXPECT_TRUE(isRegisterSuccess);
    bool isUIExtensionSurfaceNode = rsRenderNodeMap.IsUIExtensionSurfaceNode(id);
    EXPECT_TRUE(isUIExtensionSurfaceNode);
    rsRenderNodeMap.UnregisterRenderNode(id);
    isUIExtensionSurfaceNode = rsRenderNodeMap.IsUIExtensionSurfaceNode(id);
    EXPECT_FALSE(isUIExtensionSurfaceNode);
}

/**
 * @tc.name: AttachToDisplayTest
 * @tc.desc: test results of AttachToDisplay
 * @tc.type:FUNC
 * @tc.require: issueI9VAI2
 */
HWTEST_F(RSRenderNodeMapTest, AttachToDisplayTest, TestSize.Level1)
{
    NodeId id = 0;
    RSSurfaceRenderNodeConfig config = { .id = id, .nodeType = RSSurfaceNodeType::UI_EXTENSION_COMMON_NODE };
    auto node = std::make_shared<RSSurfaceRenderNode>(config);
    EXPECT_NE(node, nullptr);
    ScreenId screenId = 1;
    RSRenderNodeMap rsRenderNodeMap;
    rsRenderNodeMap.AttachToDisplay(node, screenId, false);
}

/**
 * @tc.name: FilterNodeByPid003
 * @tc.desc: test results of FilterNodeByPid
 * @tc.type:FUNC
 * @tc.require: issueI9VAI2
 */
HWTEST_F(RSRenderNodeMapTest, FilterNodeByPid003, TestSize.Level1)
{
    auto pid = getpid();
    uint32_t curId = 1;
    NodeId id = ((NodeId)pid << 32) | curId;

    RSRenderNodeMap rsRenderNodeMap;
    rsRenderNodeMap.FilterNodeByPid(1);
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSLogicalDisplayRenderNode>(id, config);
    rsRenderNodeMap.logicalDisplayNodeMap_[id] = displayNode;
    rsRenderNodeMap.FilterNodeByPid(pid);
    EXPECT_FALSE(!rsRenderNodeMap.logicalDisplayNodeMap_.empty());
}

/**
 * @tc.name: RegisterNeedAttachedNodeTest
 * @tc.desc: test results of RegisterNeedAttachedNode
 * @tc.type:FUNC
 * @tc.require: issueI9VAI2
 */
HWTEST_F(RSRenderNodeMapTest, RegisterNeedAttachedNodeTest, TestSize.Level1)
{
    auto pid = getpid();
    uint32_t curId = 1;
    NodeId id = ((NodeId)pid << 32) | curId;
    auto node = std::make_shared<RSSurfaceRenderNode>(id);
    RSRenderNodeMap rsRenderNodeMap;
    rsRenderNodeMap.RegisterNeedAttachedNode(node);
    EXPECT_FALSE(rsRenderNodeMap.needAttachedNode_.empty());
}

/**
 * @tc.name: AttachToDisplayTest002
 * @tc.desc: test results of AttachToDisplay
 * @tc.type: FUNC
 * @tc.require: issueIAI1VN
 */
HWTEST_F(RSRenderNodeMapTest, AttachToDisplayTest002, TestSize.Level1)
{
    auto pid = getpid();
    uint32_t curId = 1;
    NodeId id = ((NodeId)pid << 32) | curId;

    RSRenderNodeMap rsRenderNodeMap;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSLogicalDisplayRenderNode>(id, config);
    rsRenderNodeMap.logicalDisplayNodeMap_[id] = displayNode;
    RSSurfaceRenderNodeConfig surfaceRenderNodeconfig = {
        .id = 0, .nodeType = RSSurfaceNodeType::UI_EXTENSION_SECURE_NODE};
    auto node = std::make_shared<RSSurfaceRenderNode>(surfaceRenderNodeconfig);
    EXPECT_NE(node, nullptr);
    ScreenId screenId = 1;
    rsRenderNodeMap.AttachToDisplay(node, screenId, false);
}

/**
 * @tc.name: AttachToDisplayTest003
 * @tc.desc: test results of AttachToDisplay
 * @tc.type: FUNC
 * @tc.require: issueIAI1VN
 */
HWTEST_F(RSRenderNodeMapTest, AttachToDisplayTest003, TestSize.Level1)
{
    NodeId id = 1;
    RSSurfaceRenderNodeConfig config = {.id = id};
    auto node = std::make_shared<RSSurfaceRenderNode>(config);
    EXPECT_NE(node, nullptr);
    ScreenId screenId = 1;
    RSRenderNodeMap rsRenderNodeMap;
    rsRenderNodeMap.AttachToDisplay(node, screenId, true);
}

/**
 * @tc.name: FilterNodeByPidWithAnimationManager
 * @tc.desc: Cover branch: animationManager non-null in FilterNodeByPid
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeMapTest, FilterNodeByPidWithAnimationManager, TestSize.Level1)
{
    RSRenderNodeMap rsRenderNodeMap;
    pid_t pid = 1;
    NodeId nodeId = ((NodeId)pid << 32) | 1;

    auto node = std::make_shared<RSRenderNode>(nodeId);
    // Make animationManager_ non-null by adding an animation
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto animation = std::make_shared<RSRenderCurveAnimation>(1, 1, property, property1, property2);
    node->AddAnimation(animation);
    ASSERT_NE(node->GetAnimationManager(), nullptr);

    rsRenderNodeMap.renderNodeMap_[pid][nodeId] = node;

    // Add fallback node at renderNodeMap_[0][0] with animation to cover second FilterNodeByPid branch
    constexpr NodeId fallbackNodeId = 0;
    auto fallbackNode = std::make_shared<RSRenderNode>(fallbackNodeId);
    auto fallbackProperty = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto fallbackProperty1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto fallbackProperty2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto fallbackAnimation = std::make_shared<RSRenderCurveAnimation>(2, 2, fallbackProperty,
        fallbackProperty1, fallbackProperty2);
    fallbackNode->AddAnimation(fallbackAnimation);
    ASSERT_NE(fallbackNode->GetAnimationManager(), nullptr);
    rsRenderNodeMap.renderNodeMap_[0][0] = fallbackNode;

    rsRenderNodeMap.FilterNodeByPid(pid);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: DestroyTokenNodeEmptyAndMismatch
 * @tc.desc: Cover branches: pid not found and token mismatch in DestroyTokenNode.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeMapTest, DestroyTokenNodeEmptyAndMismatch, TestSize.Level1)
{
    RSRenderNodeMap rsRenderNodeMap;
    constexpr pid_t pid = 1;
    constexpr uint64_t token = 1;
    constexpr uint64_t otherToken = 2;
    constexpr NodeId nodeId = (static_cast<NodeId>(pid) << 32) | 1;

    // Branch: renderNodeMap has no entry for the pid.
    rsRenderNodeMap.DestroyTokenNode(pid, token);
    EXPECT_EQ(rsRenderNodeMap.GetRenderNode(nodeId), nullptr);

    // Branch: token does not match, node should be kept.
    auto node = std::make_shared<RSRenderNode>(nodeId);
    node->SetUIContextToken(token);
    rsRenderNodeMap.RegisterRenderNode(node);
    rsRenderNodeMap.DestroyTokenNode(pid, otherToken);
    EXPECT_NE(rsRenderNodeMap.GetRenderNode(nodeId), nullptr);
}

/**
 * @tc.name: DestroyTokenNodeRenderNodeMatch
 * @tc.desc: Cover branch: normal render node is erased when token matches.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeMapTest, DestroyTokenNodeRenderNodeMatch, TestSize.Level1)
{
    RSRenderNodeMap rsRenderNodeMap;
    constexpr pid_t pid = 1;
    constexpr uint64_t token = 1;
    constexpr NodeId nodeId = (static_cast<NodeId>(pid) << 32) | 1;

    auto node = std::make_shared<RSRenderNode>(nodeId);
    node->SetUIContextToken(token);
    rsRenderNodeMap.RegisterRenderNode(node);

    rsRenderNodeMap.DestroyTokenNode(pid, token);
    EXPECT_EQ(rsRenderNodeMap.GetRenderNode(nodeId), nullptr);
}

/**
 * @tc.name: DestroyTokenNodeNullNode
 * @tc.desc: Cover branch: null node entry is skipped in DestroyTokenNode.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeMapTest, DestroyTokenNodeNullNode, TestSize.Level1)
{
    RSRenderNodeMap rsRenderNodeMap;
    constexpr pid_t pid = 1;
    constexpr uint64_t token = 1;
    constexpr NodeId nodeId = (static_cast<NodeId>(pid) << 32) | 1;

    rsRenderNodeMap.renderNodeMap_[pid][nodeId] = nullptr;
    rsRenderNodeMap.DestroyTokenNode(pid, token);
    EXPECT_EQ(rsRenderNodeMap.GetRenderNode(nodeId), nullptr);
}

/**
 * @tc.name: DestroyTokenNodeAppWindowNotSelfDrawing
 * @tc.desc: Cover branches: app window surface node sets rebuild flag and is kept.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeMapTest, DestroyTokenNodeAppWindowNotSelfDrawing, TestSize.Level1)
{
    RSRenderNodeMap rsRenderNodeMap;
    constexpr pid_t pid = 1;
    constexpr uint64_t token = 1;
    constexpr NodeId nodeId = (static_cast<NodeId>(pid) << 32) | 1;

    RSSurfaceRenderNodeConfig config = { .id = nodeId, .nodeType = RSSurfaceNodeType::APP_WINDOW_NODE };
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(config);
    surfaceNode->SetUIContextToken(token);
    rsRenderNodeMap.RegisterRenderNode(surfaceNode);

    rsRenderNodeMap.DestroyTokenNode(pid, token);
    EXPECT_NE(rsRenderNodeMap.GetRenderNode(nodeId), nullptr);
    EXPECT_TRUE(surfaceNode->HasDestoryRebuild());
}

/**
 * @tc.name: DestroyTokenNodeSelfDrawingSurface
 * @tc.desc: Cover branches: self-drawing surface node is erased from renderNodeMap and surfaceNodeMap.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeMapTest, DestroyTokenNodeSelfDrawingSurface, TestSize.Level1)
{
    RSRenderNodeMap rsRenderNodeMap;
    constexpr pid_t pid = 1;
    constexpr uint64_t token = 1;
    constexpr NodeId nodeId = (static_cast<NodeId>(pid) << 32) | 1;
    constexpr NodeId parentId = (static_cast<NodeId>(pid) << 32) | 2;

    RSSurfaceRenderNodeConfig config = { .id = nodeId, .nodeType = RSSurfaceNodeType::SELF_DRAWING_NODE };
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(config);
    surfaceNode->SetUIContextToken(token);
    rsRenderNodeMap.RegisterRenderNode(surfaceNode);

    auto parent = std::make_shared<RSRenderNode>(parentId);
    parent->AddChild(surfaceNode);

    rsRenderNodeMap.DestroyTokenNode(pid, token);
    EXPECT_EQ(rsRenderNodeMap.GetRenderNode(nodeId), nullptr);
}

/**
 * @tc.name: DestroyTokenNodeRootNodeWithParent
 * @tc.desc: Cover branch: root node with parent triggers RemoveChildFromFulllist.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeMapTest, DestroyTokenNodeRootNodeWithParent, TestSize.Level1)
{
    RSRenderNodeMap rsRenderNodeMap;
    constexpr pid_t pid = 1;
    constexpr uint64_t token = 1;
    constexpr NodeId rootId = (static_cast<NodeId>(pid) << 32) | 1;
    constexpr NodeId parentId = (static_cast<NodeId>(pid) << 32) | 2;

    auto parent = std::make_shared<RSRenderNode>(parentId);
    auto rootNode = std::make_shared<RSRootRenderNode>(rootId);
    rootNode->SetUIContextToken(token);
    parent->AddChild(rootNode);
    rsRenderNodeMap.RegisterRenderNode(rootNode);

    rsRenderNodeMap.DestroyTokenNode(pid, token);
    EXPECT_EQ(rsRenderNodeMap.GetRenderNode(rootId), nullptr);
}

/**
 * @tc.name: DestroyTokenNodeCanvasDrawingNode
 * @tc.desc: Cover branch: canvas drawing node path in renderNodeMap lambda.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeMapTest, DestroyTokenNodeCanvasDrawingNode, TestSize.Level1)
{
    RSRenderNodeMap rsRenderNodeMap;
    constexpr pid_t pid = 1;
    constexpr uint64_t token = 1;
    constexpr NodeId nodeId = (static_cast<NodeId>(pid) << 32) | 1;

    auto canvasNode = std::make_shared<RSCanvasDrawingRenderNode>(nodeId);
    canvasNode->SetUIContextToken(token);
    rsRenderNodeMap.RegisterRenderNode(canvasNode);

    rsRenderNodeMap.DestroyTokenNode(pid, token);
    // With RS_MODIFIERS_DRAW_ENABLE undefined or hybrid disabled, the node is kept.
    EXPECT_NE(rsRenderNodeMap.GetRenderNode(nodeId), nullptr);
}

/**
 * @tc.name: DestroyTokenNodeResidentSurfaceNodeMap
 * @tc.desc: Cover branch: resident surface node map is erased while renderNodeMap keeps the node.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeMapTest, DestroyTokenNodeResidentSurfaceNodeMap, TestSize.Level1)
{
    RSRenderNodeMap rsRenderNodeMap;
    constexpr pid_t pid = 1;
    constexpr uint64_t token = 1;
    constexpr NodeId nodeId = (static_cast<NodeId>(pid) << 32) | 1;

    RSSurfaceRenderNodeConfig config = {
        .id = nodeId,
        .nodeType = RSSurfaceNodeType::APP_WINDOW_NODE,
        .surfaceWindowType = SurfaceWindowType::SCB_DESKTOP,
    };
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(config);
    surfaceNode->SetUIContextToken(token);
    rsRenderNodeMap.RegisterRenderNode(surfaceNode);
    ASSERT_FALSE(rsRenderNodeMap.GetResidentSurfaceNodeMap().empty());

    rsRenderNodeMap.DestroyTokenNode(pid, token);
    EXPECT_NE(rsRenderNodeMap.GetRenderNode(nodeId), nullptr);
    EXPECT_TRUE(rsRenderNodeMap.GetResidentSurfaceNodeMap().empty());
}

/**
 * @tc.name: DestroyTokenNodeSelfDrawingNodeInProcess
 * @tc.desc: Cover branches in selfDrawingNodeInProcess_ erase path.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeMapTest, DestroyTokenNodeSelfDrawingNodeInProcess, TestSize.Level1)
{
    RSRenderNodeMap rsRenderNodeMap;
    constexpr pid_t pid = 1;
    constexpr uint64_t token = 1;
    constexpr uint64_t otherToken = 2;
    constexpr NodeId nodeId1 = (static_cast<NodeId>(pid) << 32) | 1;
    constexpr NodeId nodeId2 = (static_cast<NodeId>(pid) << 32) | 2;

    RSSurfaceRenderNodeConfig config = { .id = nodeId1, .nodeType = RSSurfaceNodeType::SELF_DRAWING_NODE };
    auto node1 = std::make_shared<RSSurfaceRenderNode>(config);
    node1->SetUIContextToken(token);
    config.id = nodeId2;
    auto node2 = std::make_shared<RSSurfaceRenderNode>(config);
    node2->SetUIContextToken(otherToken);

    rsRenderNodeMap.selfDrawingNodeInProcess_[pid][nodeId1] = node1;
    rsRenderNodeMap.selfDrawingNodeInProcess_[pid][nodeId2] = node2;

    rsRenderNodeMap.DestroyTokenNode(pid, token);
    EXPECT_EQ(rsRenderNodeMap.selfDrawingNodeInProcess_[pid].find(nodeId1),
              rsRenderNodeMap.selfDrawingNodeInProcess_[pid].end());
    EXPECT_NE(rsRenderNodeMap.selfDrawingNodeInProcess_[pid].find(nodeId2),
              rsRenderNodeMap.selfDrawingNodeInProcess_[pid].end());
}

/**
 * @tc.name: DestroyTokenNodeUnInTreeNodeSet
 * @tc.desc: Cover branch: unInTreeNodeSet_ erases nodes belonging to the pid.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeMapTest, DestroyTokenNodeUnInTreeNodeSet, TestSize.Level1)
{
    RSRenderNodeMap rsRenderNodeMap;
    constexpr pid_t pid = 1;
    constexpr uint64_t token = 1;
    constexpr NodeId nodeId = (static_cast<NodeId>(pid) << 32) | 1;
    constexpr NodeId otherNodeId = (static_cast<NodeId>(2) << 32) | 1;

    rsRenderNodeMap.RegisterUnTreeNode(nodeId);
    rsRenderNodeMap.RegisterUnTreeNode(otherNodeId);

    rsRenderNodeMap.DestroyTokenNode(pid, token);
    EXPECT_FALSE(rsRenderNodeMap.UnRegisterUnTreeNode(nodeId));
    EXPECT_TRUE(rsRenderNodeMap.UnRegisterUnTreeNode(otherNodeId));
}

/**
 * @tc.name: SurfaceHandlerTest
 * @tc.desc: Cover all if branches in RegisterSurfaceHandler / GetSurfaceHandler / UnregisterSurfaceHandlerByPid.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeMapTest, SurfaceHandlerTest, TestSize.Level1)
{
    RSRenderNodeMap rsRenderNodeMap;
    constexpr pid_t pid = 1;
    constexpr NodeId nodeId = (static_cast<NodeId>(pid) << 32) | 1;
    constexpr NodeId otherNodeId = (static_cast<NodeId>(2) << 32) | 1;

    // Branch: nodeId not found.
    EXPECT_EQ(rsRenderNodeMap.GetSurfaceHandler(nodeId, false), nullptr);

    auto handler = std::make_shared<RSSurfaceHandler>(nodeId);
    rsRenderNodeMap.RegisterSurfaceHandler(nodeId, handler);

    // Branch: found without unregister.
    EXPECT_EQ(rsRenderNodeMap.GetSurfaceHandler(nodeId, false), handler);

    // Branch: found with unregister.
    EXPECT_EQ(rsRenderNodeMap.GetSurfaceHandler(nodeId, true), handler);
    EXPECT_EQ(rsRenderNodeMap.GetSurfaceHandler(nodeId, false), nullptr);

    // Branch: UnregisterSurfaceHandlerByPid erases only entries for the given pid.
    rsRenderNodeMap.RegisterSurfaceHandler(nodeId, handler);
    rsRenderNodeMap.RegisterSurfaceHandler(otherNodeId, std::make_shared<RSSurfaceHandler>(otherNodeId));
    rsRenderNodeMap.UnregisterSurfaceHandlerByPid(pid);
    EXPECT_EQ(rsRenderNodeMap.GetSurfaceHandler(nodeId, false), nullptr);
    EXPECT_NE(rsRenderNodeMap.GetSurfaceHandler(otherNodeId, false), nullptr);
}

/**
 * @tc.name: SurfaceHandlerInfoTest
 * @tc.desc: Cover if branches in SaveSurfaceHandlerInfo / GetSurfaceHandlerInfo / RemoveSurfaceHandlerInfo.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeMapTest, SurfaceHandlerInfoTest, TestSize.Level1)
{
    RSRenderNodeMap rsRenderNodeMap;
    constexpr pid_t pid = 1;
    constexpr NodeId nodeId = (static_cast<NodeId>(pid) << 32) | 1;

    // Branch: not found.
    auto emptyInfo = rsRenderNodeMap.GetSurfaceHandlerInfo(nodeId);
    EXPECT_EQ(emptyInfo.first, nullptr);

    auto handler = std::make_shared<RSSurfaceHandler>(nodeId);
    std::pair<std::shared_ptr<RSSurfaceHandler>, sptr<IBufferConsumerListener>> info =
        { handler, sptr<IBufferConsumerListener>() };
    rsRenderNodeMap.SaveSurfaceHandlerInfo(nodeId, info);

    // Branch: found.
    auto retrieved = rsRenderNodeMap.GetSurfaceHandlerInfo(nodeId);
    EXPECT_EQ(retrieved.first, handler);

    // Branch: removed by UnregisterRenderNode.
    rsRenderNodeMap.UnregisterRenderNode(nodeId);
    EXPECT_EQ(rsRenderNodeMap.GetSurfaceHandlerInfo(nodeId).first, nullptr);
}

/**
 * @tc.name: DestroyTokenNodeRootNodeWithoutParent
 * @tc.desc: Cover branch: root node without parent skips RemoveChildFromFulllist.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeMapTest, DestroyTokenNodeRootNodeWithoutParent, TestSize.Level1)
{
    RSRenderNodeMap rsRenderNodeMap;
    constexpr pid_t pid = 1;
    constexpr uint64_t token = 1;
    constexpr NodeId rootId = (static_cast<NodeId>(pid) << 32) | 1;

    auto rootNode = std::make_shared<RSRootRenderNode>(rootId);
    rootNode->SetUIContextToken(token);
    rsRenderNodeMap.RegisterRenderNode(rootNode);

    rsRenderNodeMap.DestroyTokenNode(pid, token);
    EXPECT_EQ(rsRenderNodeMap.GetRenderNode(rootId), nullptr);
}

/**
 * @tc.name: DestroyTokenNodeSurfaceNodeMapBranches
 * @tc.desc: Cover every condition in surfaceNodeMap EraseIf of DestroyTokenNode.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeMapTest, DestroyTokenNodeSurfaceNodeMapBranches, TestSize.Level1)
{
    RSRenderNodeMap rsRenderNodeMap;
    constexpr pid_t pid = 1;
    constexpr uint64_t token = 1;
    constexpr uint64_t otherToken = 2;
    constexpr NodeId selfDrawId = (static_cast<NodeId>(pid) << 32) | 1;
    constexpr NodeId appWindowId = (static_cast<NodeId>(pid) << 32) | 2;
    constexpr NodeId wrongTokenId = (static_cast<NodeId>(pid) << 32) | 3;
    constexpr NodeId nullId = (static_cast<NodeId>(pid) << 32) | 4;
    constexpr NodeId otherPidId = (static_cast<NodeId>(2) << 32) | 1;

    RSSurfaceRenderNodeConfig selfDrawConfig = {
        .id = selfDrawId, .nodeType = RSSurfaceNodeType::SELF_DRAWING_NODE };
    auto selfDrawNode = std::make_shared<RSSurfaceRenderNode>(selfDrawConfig);
    selfDrawNode->SetUIContextToken(token);
    rsRenderNodeMap.RegisterRenderNode(selfDrawNode);

    RSSurfaceRenderNodeConfig appConfig = {
        .id = appWindowId, .nodeType = RSSurfaceNodeType::APP_WINDOW_NODE };
    auto appNode = std::make_shared<RSSurfaceRenderNode>(appConfig);
    appNode->SetUIContextToken(token);
    rsRenderNodeMap.RegisterRenderNode(appNode);

    RSSurfaceRenderNodeConfig wrongTokenConfig = {
        .id = wrongTokenId, .nodeType = RSSurfaceNodeType::SELF_DRAWING_NODE };
    auto wrongTokenNode = std::make_shared<RSSurfaceRenderNode>(wrongTokenConfig);
    wrongTokenNode->SetUIContextToken(otherToken);
    rsRenderNodeMap.RegisterRenderNode(wrongTokenNode);

    RSSurfaceRenderNodeConfig otherPidConfig = {
        .id = otherPidId, .nodeType = RSSurfaceNodeType::SELF_DRAWING_NODE };
    auto otherPidNode = std::make_shared<RSSurfaceRenderNode>(otherPidConfig);
    otherPidNode->SetUIContextToken(token);
    rsRenderNodeMap.RegisterRenderNode(otherPidNode);

    rsRenderNodeMap.surfaceNodeMap_[nullId] = nullptr;

    rsRenderNodeMap.DestroyTokenNode(pid, token);
    EXPECT_EQ(rsRenderNodeMap.GetRenderNode(selfDrawId), nullptr);
    EXPECT_NE(rsRenderNodeMap.GetRenderNode(appWindowId), nullptr);
    EXPECT_NE(rsRenderNodeMap.GetRenderNode(wrongTokenId), nullptr);
    EXPECT_NE(rsRenderNodeMap.GetRenderNode(otherPidId), nullptr);
    EXPECT_NE(rsRenderNodeMap.surfaceNodeMap_.find(nullId), rsRenderNodeMap.surfaceNodeMap_.end());
}

/**
 * @tc.name: DestroyTokenNodeResidentSurfaceNodeMapKeep
 * @tc.desc: Cover residentSurfaceNodeMap branches: erase matching, keep mismatched/null.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeMapTest, DestroyTokenNodeResidentSurfaceNodeMapKeep, TestSize.Level1)
{
    RSRenderNodeMap rsRenderNodeMap;
    constexpr pid_t pid = 1;
    constexpr uint64_t token = 1;
    constexpr uint64_t otherToken = 2;
    constexpr NodeId eraseId = (static_cast<NodeId>(pid) << 32) | 1;
    constexpr NodeId otherPidId = (static_cast<NodeId>(2) << 32) | 1;
    constexpr NodeId wrongTokenId = (static_cast<NodeId>(pid) << 32) | 2;
    constexpr NodeId nullId = (static_cast<NodeId>(pid) << 32) | 3;

    RSSurfaceRenderNodeConfig eraseConfig = {
        .id = eraseId,
        .nodeType = RSSurfaceNodeType::APP_WINDOW_NODE,
        .surfaceWindowType = SurfaceWindowType::SCB_DESKTOP,
    };
    auto eraseNode = std::make_shared<RSSurfaceRenderNode>(eraseConfig);
    eraseNode->SetUIContextToken(token);
    rsRenderNodeMap.RegisterRenderNode(eraseNode);

    RSSurfaceRenderNodeConfig otherPidConfig = {
        .id = otherPidId,
        .nodeType = RSSurfaceNodeType::APP_WINDOW_NODE,
        .surfaceWindowType = SurfaceWindowType::SCB_DESKTOP,
    };
    auto otherPidNode = std::make_shared<RSSurfaceRenderNode>(otherPidConfig);
    otherPidNode->SetUIContextToken(token);
    rsRenderNodeMap.RegisterRenderNode(otherPidNode);

    RSSurfaceRenderNodeConfig wrongTokenConfig = {
        .id = wrongTokenId,
        .nodeType = RSSurfaceNodeType::APP_WINDOW_NODE,
        .surfaceWindowType = SurfaceWindowType::SCB_DESKTOP,
    };
    auto wrongTokenNode = std::make_shared<RSSurfaceRenderNode>(wrongTokenConfig);
    wrongTokenNode->SetUIContextToken(otherToken);
    rsRenderNodeMap.RegisterRenderNode(wrongTokenNode);

    rsRenderNodeMap.residentSurfaceNodeMap_[nullId] = nullptr;

    rsRenderNodeMap.DestroyTokenNode(pid, token);
    EXPECT_EQ(rsRenderNodeMap.GetResidentSurfaceNodeMap().find(eraseId),
              rsRenderNodeMap.GetResidentSurfaceNodeMap().end());
    EXPECT_NE(rsRenderNodeMap.GetResidentSurfaceNodeMap().find(otherPidId),
              rsRenderNodeMap.GetResidentSurfaceNodeMap().end());
    EXPECT_NE(rsRenderNodeMap.GetResidentSurfaceNodeMap().find(wrongTokenId),
              rsRenderNodeMap.GetResidentSurfaceNodeMap().end());
    EXPECT_NE(rsRenderNodeMap.GetResidentSurfaceNodeMap().find(nullId),
              rsRenderNodeMap.GetResidentSurfaceNodeMap().end());
}

/**
 * @tc.name: DestroyTokenNodeSelfDrawingNodeInProcessBranches
 * @tc.desc: Cover selfDrawingNodeInProcess_ branches: parent exists, null node, token mismatch.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeMapTest, DestroyTokenNodeSelfDrawingNodeInProcessBranches, TestSize.Level1)
{
    RSRenderNodeMap rsRenderNodeMap;
    constexpr pid_t pid = 1;
    constexpr uint64_t token = 1;
    constexpr uint64_t otherToken = 2;
    constexpr NodeId withParentId = (static_cast<NodeId>(pid) << 32) | 1;
    constexpr NodeId nullId = (static_cast<NodeId>(pid) << 32) | 2;
    constexpr NodeId mismatchId = (static_cast<NodeId>(pid) << 32) | 3;

    RSSurfaceRenderNodeConfig config = {
        .id = withParentId, .nodeType = RSSurfaceNodeType::SELF_DRAWING_NODE };
    auto withParentNode = std::make_shared<RSSurfaceRenderNode>(config);
    withParentNode->SetUIContextToken(token);
    auto parent = std::make_shared<RSRenderNode>((static_cast<NodeId>(pid) << 32) | 10);
    parent->AddChild(withParentNode);

    config.id = mismatchId;
    auto mismatchNode = std::make_shared<RSSurfaceRenderNode>(config);
    mismatchNode->SetUIContextToken(otherToken);

    rsRenderNodeMap.selfDrawingNodeInProcess_[pid][withParentId] = withParentNode;
    rsRenderNodeMap.selfDrawingNodeInProcess_[pid][nullId] = nullptr;
    rsRenderNodeMap.selfDrawingNodeInProcess_[pid][mismatchId] = mismatchNode;

    rsRenderNodeMap.DestroyTokenNode(pid, token);
    EXPECT_EQ(rsRenderNodeMap.selfDrawingNodeInProcess_[pid].find(withParentId),
              rsRenderNodeMap.selfDrawingNodeInProcess_[pid].end());
    EXPECT_NE(rsRenderNodeMap.selfDrawingNodeInProcess_[pid].find(nullId),
              rsRenderNodeMap.selfDrawingNodeInProcess_[pid].end());
    EXPECT_NE(rsRenderNodeMap.selfDrawingNodeInProcess_[pid].find(mismatchId),
              rsRenderNodeMap.selfDrawingNodeInProcess_[pid].end());
}

/**
 * @tc.name: GetProtectiveSolidNodeMapSize001
 * @tc.desc: Test GetProtectiveSolidNodeMapSize with empty map
 * @tc.type: FUNC
 * @tc.require: issueI9NBLA
 */
HWTEST_F(RSRenderNodeMapTest, GetProtectiveSolidNodeMapSize001, TestSize.Level1)
{
    RSRenderNodeMap rsRenderNodeMap;
    EXPECT_EQ(rsRenderNodeMap.GetProtectiveSolidNodeMapSize(), 0);
}

/**
 * @tc.name: GetProtectiveSolidNodeMapSize002
 * @tc.desc: Test GetProtectiveSolidNodeMapSize with nodes in map
 * @tc.type: FUNC
 * @tc.require: issueI9NBLA
 */
HWTEST_F(RSRenderNodeMapTest, GetProtectiveSolidNodeMapSize002, TestSize.Level1)
{
    RSRenderNodeMap rsRenderNodeMap;
    auto rsContext = std::make_shared<RSContext>();
    
    NodeId id1 = 100;
    auto node1 = std::make_shared<RSSurfaceRenderNode>(id1);
    rsRenderNodeMap.protectiveSolidNodeMap_[id1] = node1;
    EXPECT_EQ(rsRenderNodeMap.GetProtectiveSolidNodeMapSize(), 1);
    
    NodeId id2 = 200;
    auto node2 = std::make_shared<RSSurfaceRenderNode>(id2);
    rsRenderNodeMap.protectiveSolidNodeMap_[id2] = node2;
    EXPECT_EQ(rsRenderNodeMap.GetProtectiveSolidNodeMapSize(), 2);
}

/**
 * @tc.name: TraverseProtectiveSolidNodes001
 * @tc.desc: Test TraverseProtectiveSolidNodes with empty map
 * @tc.type: FUNC
 * @tc.require: issueI9NBLA
 */
HWTEST_F(RSRenderNodeMapTest, TraverseProtectiveSolidNodes001, TestSize.Level1)
{
    RSRenderNodeMap rsRenderNodeMap;
    int count = 0;
    rsRenderNodeMap.TraverseProtectiveSolidNodes([&count](const std::shared_ptr<RSSurfaceRenderNode>& node) {
        count++;
    });
    EXPECT_EQ(count, 0);
}

/**
 * @tc.name: TraverseProtectiveSolidNodes002
 * @tc.desc: Test TraverseProtectiveSolidNodes with nodes in map
 * @tc.type: FUNC
 * @tc.require: issueI9NBLA
 */
HWTEST_F(RSRenderNodeMapTest, TraverseProtectiveSolidNodes002, TestSize.Level1)
{
    RSRenderNodeMap rsRenderNodeMap;
    
    NodeId id1 = 100;
    auto node1 = std::make_shared<RSSurfaceRenderNode>(id1);
    NodeId id2 = 200;
    auto node2 = std::make_shared<RSSurfaceRenderNode>(id2);
    NodeId id3 = 300;
    auto node3 = std::make_shared<RSSurfaceRenderNode>(id3);
    
    rsRenderNodeMap.protectiveSolidNodeMap_[id1] = node1;
    rsRenderNodeMap.protectiveSolidNodeMap_[id2] = node2;
    rsRenderNodeMap.protectiveSolidNodeMap_[id3] = node3;
    
    int count = 0;
    std::vector<NodeId> visitedIds;
    rsRenderNodeMap.TraverseProtectiveSolidNodes([&count, &visitedIds](const std::shared_ptr<RSSurfaceRenderNode>& node)
    {
        count++;
        visitedIds.push_back(node->GetId());
    });
    EXPECT_EQ(count, 3);
    EXPECT_EQ(visitedIds.size(), 3);
}

/**
 * @tc.name: TraverseProtectiveSolidNodes003
 * @tc.desc: Test TraverseProtectiveSolidNodes with null nodes
 * @tc.type: FUNC
 * @tc.require: issueI9NBLA
 */
HWTEST_F(RSRenderNodeMapTest, TraverseProtectiveSolidNodes003, TestSize.Level1)
{
    RSRenderNodeMap rsRenderNodeMap;
    
    NodeId id1 = 100;
    auto node1 = std::make_shared<RSSurfaceRenderNode>(id1);
    NodeId id2 = 200;
    
    rsRenderNodeMap.protectiveSolidNodeMap_[id1] = node1;
    rsRenderNodeMap.protectiveSolidNodeMap_[id2] = nullptr;
    
    int count = 0;
    rsRenderNodeMap.TraverseProtectiveSolidNodes([&count](const std::shared_ptr<RSSurfaceRenderNode>& node) {
        if (node != nullptr) {
            count++;
        }
    });
    EXPECT_EQ(count, 1);
}

} // namespace OHOS::Rosen