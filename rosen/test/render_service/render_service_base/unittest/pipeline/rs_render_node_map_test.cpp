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

#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_render_node_map.h"
#include "pipeline/rs_surface_render_node.h"

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
 * @tc.name: RegisterDisplayRenderNode
 * @tc.desc: test results of RegisterDisplayRenderNode
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSRenderNodeMapTest, RegisterDisplayRenderNode, TestSize.Level1)
{
    NodeId id = 1;
    RSDisplayNodeConfig config;
    RSDisplayRenderNode* rsDisplayRenderNode = new RSDisplayRenderNode(id, config);
    std::shared_ptr<RSDisplayRenderNode> node(rsDisplayRenderNode);
    RSRenderNodeMap rsRenderNodeMap;
    auto result = rsRenderNodeMap.RegisterDisplayRenderNode(node);
    EXPECT_TRUE(result);
    result = rsRenderNodeMap.RegisterDisplayRenderNode(node);
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
    pid_t pid = ExtractPid(id);
    auto node = std::make_shared<OHOS::Rosen::RSRenderNode>(id);
    RSRenderNodeMap rsRenderNodeMap;
    rsRenderNodeMap.FilterNodeByPid(1);

    rsRenderNodeMap.renderNodeMap_[pid][id] = node;
    rsRenderNodeMap.FilterNodeByPid(1);
    RSDisplayNodeConfig config;
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(id, config);
    rsRenderNodeMap.displayNodeMap_.emplace(id, rsDisplayRenderNode);
    rsRenderNodeMap.FilterNodeByPid(1);
    rsRenderNodeMap.renderNodeMap_.clear();
    rsRenderNodeMap.FilterNodeByPid(1);
    EXPECT_TRUE(true);
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
} // namespace OHOS::Rosen