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
#include "include/command/rs_base_node_command.h"
#include "include/command/rs_surface_node_command.h"
#include "include/command/rs_ui_director_command.h"
#include "include/pipeline/rs_surface_render_node.h"
#include "params/rs_surface_render_params.h"
#include "pipeline/rs_logical_display_render_node.h"
#include "pipeline/rs_screen_render_node.h"
#include "common/rs_anco_type.h"
#include "common/rs_common_def.h"
#ifndef ROSEN_CROSS_PLATFORM
#include "iconsumer_surface.h"
#include "pipeline/rs_surface_handler.h"
#include "ibuffer_consumer_listener.h"
#endif

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSSurfaceNodeCommandTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static constexpr float outerRadius = 30.4f;
    RRect rrect = RRect({0, 0, 0, 0}, outerRadius, outerRadius);
};

void RSSurfaceNodeCommandTest::SetUpTestCase() {}
void RSSurfaceNodeCommandTest::TearDownTestCase() {}
void RSSurfaceNodeCommandTest::SetUp() {}
void RSSurfaceNodeCommandTest::TearDown() {}

/**
 * @tc.name: TestCreate
 * @tc.desc: Create test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, TestCreate, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 5;
    NodeId childNodeId = 7;
    int32_t index = static_cast<int32_t>(0);
    SurfaceNodeCommandHelper::Create(context, nodeId);
    BaseNodeCommandHelper::AddChild(context, nodeId, childNodeId, index);
    SurfaceNodeCommandHelper::Create(context, childNodeId);
    ASSERT_EQ(childNodeId, static_cast<NodeId>(7));
}

/**
 * @tc.name: TestCreateWithSceneBoardToken
 * @tc.desc: Non-SCB Create paths: token registration on miss, no-op on zero token.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, TestCreateWithSceneBoardToken, TestSize.Level1)
{
    constexpr pid_t pid = 800;
    constexpr uint64_t unknownToken = 802;
    constexpr NodeId missNodeId = MakeNodeId(pid, 2);
    constexpr NodeId zeroTokenNodeId = MakeNodeId(pid, 3);

    RSContext context;

    // Unmarked token: not marked, token still registered.
    SurfaceNodeCommandHelper::Create(context, missNodeId, RSSurfaceNodeType::DEFAULT, false, unknownToken);
    auto missNode = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(missNodeId);
    ASSERT_NE(missNode, nullptr);
    EXPECT_FALSE(missNode->IsSceneBoard());
    EXPECT_EQ(missNode->GetUIContextToken(), unknownToken);

    // Zero token: neither marked nor registered.
    SurfaceNodeCommandHelper::Create(context, zeroTokenNodeId, RSSurfaceNodeType::DEFAULT, false, 0);
    auto zeroNode = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(zeroTokenNodeId);
    ASSERT_NE(zeroNode, nullptr);
    EXPECT_FALSE(zeroNode->IsSceneBoard());
    EXPECT_EQ(zeroNode->GetUIContextToken(), 0);
}

/**
 * @tc.name: TestCreateSceneBoardFactoryPath
 * @tc.desc: SCB-marked Create paths (blocked on SceneBoard factory implementation).
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, TestCreateSceneBoardFactoryPath, TestSize.Level1)
{
    constexpr pid_t pid = 800;
    constexpr uint64_t token = 801;
    constexpr NodeId directorNodeId = MakeNodeId(pid, 0);
    constexpr NodeId nodeId = MakeNodeId(pid, 1);
    enum SurfaceWindowType windowType = SurfaceWindowType::DEFAULT_WINDOW;

    RSContext context;
    RSUIDirectorCommandHelper::GoCreate(context, directorNodeId, token);
    RSUIDirectorCommandHelper::MarkSceneBoard(context, directorNodeId, token);

    // Marked token: marked and token-registered.
    SurfaceNodeCommandHelper::Create(context, nodeId, RSSurfaceNodeType::DEFAULT, false, token);
    auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId);
    ASSERT_NE(node, nullptr);
    EXPECT_TRUE(node->IsSceneBoard());
    EXPECT_EQ(node->GetUIContextToken(), token);

    // CreateWithConfig path.
    constexpr NodeId configNodeId = MakeNodeId(pid, 4);
    SurfaceNodeCommandHelper::CreateWithConfig(context, configNodeId, "scb", 1, windowType, token);
    auto configNode = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(configNodeId);
    ASSERT_NE(configNode, nullptr);
    EXPECT_TRUE(configNode->IsSceneBoard());
}

/**
 * @tc.name: TestCreateWithConfig
 * @tc.desc: CreateWithConfig test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, TestCreateWithConfig, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 9;
    NodeId childNodeId = 11;
    int32_t index = static_cast<int32_t>(0);
    std::string name = "name";
    std::string name2 = "name2";
    enum SurfaceWindowType windowType = SurfaceWindowType::DEFAULT_WINDOW;
    SurfaceNodeCommandHelper::CreateWithConfig(context, nodeId, name, 1, windowType);
    BaseNodeCommandHelper::AddChild(context, nodeId, childNodeId, index);
    SurfaceNodeCommandHelper::CreateWithConfig(context, childNodeId, name2, 1, windowType);
    ASSERT_EQ(childNodeId, static_cast<NodeId>(11));
}

/**
 * @tc.name: TestRSSurfaceNodeCommand002
 * @tc.desc: ConnectToNodeInRenderService test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, TestRSSurfaceNodeCommand002, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(-1);
    SurfaceNodeCommandHelper::ConnectToNodeInRenderService(context, id);
    NodeId id2 = 10;
    auto context2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*context2, id2);
    SurfaceNodeCommandHelper::ConnectToNodeInRenderService(*context2, id2);
    ASSERT_EQ(id2, static_cast<NodeId>(10));
}

/**
 * @tc.name: TestRSSurfaceNodeCommand003
 * @tc.desc: SetContextClipRegion test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, TestRSSurfaceNodeCommand003, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(-1);
    Drawing::Rect clipRect;
    SurfaceNodeCommandHelper::SetContextClipRegion(context, id, clipRect);
    NodeId id2 = 10;
    auto context2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*context2, id2);
    SurfaceNodeCommandHelper::SetContextClipRegion(*context2, id2, clipRect);
    ASSERT_EQ(id2, static_cast<NodeId>(10));
}

/**
 * @tc.name: TestSetWatermarkEnabled
 * @tc.desc: SetWatermarkEnabled test
 * @tc.type: FUNC
 * @tc.require: issueIB209E
 */
HWTEST_F(RSSurfaceNodeCommandTest, SetWatermarkEnabledTest, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(-1);
    SurfaceNodeCommandHelper::SetWatermarkEnabled(context, nodeId, "test", true);
    ASSERT_EQ(context.GetNodeMap().GetRenderNode<RSRenderNode>(nodeId), nullptr);
    nodeId = 10;
    SurfaceNodeCommandHelper::Create(context, nodeId);
    SurfaceNodeCommandHelper::SetWatermarkEnabled(context, nodeId, "test", true);
    auto surfaceNode = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId);
    ASSERT_NE(surfaceNode, nullptr);
    ASSERT_NE(surfaceNode->stagingRenderParams_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNode->stagingRenderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    EXPECT_EQ(surfaceParams->GetWatermarksEnabledMap().at("test"), true);
    SurfaceNodeCommandHelper::SetWatermarkEnabled(context, nodeId, "test", false);
    auto watermarkMap = surfaceParams->GetWatermarksEnabledMap();
    EXPECT_EQ(watermarkMap.find("test") == watermarkMap.end(), true);
}

/**
 * @tc.name: TestSetLeashPersistentId
 * @tc.desc: SetLeashPersistentId test
 * @tc.type: FUNC
 * @tc.require: issueIB209E
 */
HWTEST_F(RSSurfaceNodeCommandTest, SetLeashPersistentIdTest, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(-1);
    LeashPersistentId leashPersistentId = 1;
    SurfaceNodeCommandHelper::SetLeashPersistentId(context, nodeId, leashPersistentId);
    ASSERT_EQ(context.GetNodeMap().GetRenderNode<RSRenderNode>(nodeId), nullptr);
    nodeId = 10;
    SurfaceNodeCommandHelper::Create(context, nodeId);
    SurfaceNodeCommandHelper::SetLeashPersistentId(context, nodeId, leashPersistentId);
    auto surfaceNode = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId);
    ASSERT_NE(surfaceNode, nullptr);
    EXPECT_EQ(surfaceNode->leashPersistentId_, leashPersistentId);
}

/**
 * @tc.name: TestRSSurfaceNodeCommand004
 * @tc.desc: SetSecurityLayer test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, TestRSSurfaceNodeCommand004, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(-1);
    bool isSecurityLayer = false;
    SurfaceNodeCommandHelper::SetSecurityLayer(context, id, isSecurityLayer);
    NodeId id2 = 10;
    auto context2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*context2, id2);
    SurfaceNodeCommandHelper::SetSecurityLayer(*context2, id2, isSecurityLayer);
    ASSERT_EQ(id2, static_cast<NodeId>(10));
}

/**
 * @tc.name: TestRSSurfaceNodeCommand005
 * @tc.desc: SetColorSpace test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, TestRSSurfaceNodeCommand005, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(-1);
    GraphicColorGamut colorSpace = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_INVALID;
    SurfaceNodeCommandHelper::SetColorSpace(context, id, colorSpace);
    NodeId id2 = 10;
    auto context2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*context2, id2);
    SurfaceNodeCommandHelper::SetColorSpace(*context2, id2, colorSpace);
    ASSERT_EQ(id2, static_cast<NodeId>(10));
}

/**
 * @tc.name: TestRSSurfaceNodeCommand006
 * @tc.desc: UpdateSurfaceDefaultSize test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, TestRSSurfaceNodeCommand006, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(-1);
    float width = 0;
    float height = 0;
    SurfaceNodeCommandHelper::UpdateSurfaceDefaultSize(context, id, width, height);
    NodeId id2 = 10;
    auto context2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*context2, id2);
    SurfaceNodeCommandHelper::UpdateSurfaceDefaultSize(*context2, id2, width, height);
    ASSERT_EQ(id2, static_cast<NodeId>(10));
}

/**
 * @tc.name: TestRSSurfaceNodeCommand007
 * @tc.desc: ConnectToNodeInRenderService test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, TestRSSurfaceNodeCommand007, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(-1);
    SurfaceNodeCommandHelper::ConnectToNodeInRenderService(context, id);
    NodeId id2 = 10;
    auto context2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*context2, id2);
    SurfaceNodeCommandHelper::ConnectToNodeInRenderService(*context2, id2);
    ASSERT_EQ(id2, static_cast<NodeId>(10));
}

/**
 * @tc.name: TestRSSurfaceNodeCommand008
 * @tc.desc: SetCallbackForRenderThreadRefresh test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, TestRSSurfaceNodeCommand008, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(-1);
    SurfaceNodeCommandHelper::SetCallbackForRenderThreadRefresh(context, id, true);
    NodeId id2 = 10;
    auto context2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*context2, id2);
    SurfaceNodeCommandHelper::SetCallbackForRenderThreadRefresh(*context2, id2, false);
    ASSERT_EQ(id2, static_cast<NodeId>(10));
}

/**
 * @tc.name: TestRSSurfaceNodeCommand009
 * @tc.desc: SetContextBounds test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, TestRSSurfaceNodeCommand009, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(-1);
    Vector4f bounds;
    SurfaceNodeCommandHelper::SetContextBounds(context, id, bounds);
    NodeId id2 = 10;
    auto context2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*context2, id2);
    SurfaceNodeCommandHelper::SetContextBounds(*context2, id2, bounds);
    ASSERT_EQ(id2, static_cast<NodeId>(10));
}

/**
 * @tc.name: TestRSSurfaceNodeCommand010
 * @tc.desc: SetAbilityBGAlpha test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, TestRSSurfaceNodeCommand010, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(-1);
    uint8_t alpha = static_cast<uint8_t>(0);
    SurfaceNodeCommandHelper::SetAbilityBGAlpha(context, id, alpha);
    NodeId id2 = 10;
    auto context2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*context2, id2);
    SurfaceNodeCommandHelper::SetAbilityBGAlpha(*context2, id2, alpha);
    ASSERT_EQ(id2, static_cast<NodeId>(10));
}

/**
 * @tc.name: TestRSSurfaceNodeCommand012
 * @tc.desc: SetIsNotifyUIBufferAvailable test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, TestRSSurfaceNodeCommand012, TestSize.Level1)
{
    RSContext context;
    NodeId id = -10;
    bool available = false;
    SurfaceNodeCommandHelper::SetIsNotifyUIBufferAvailable(context, id, available);
    ASSERT_EQ(id, static_cast<NodeId>(-10));
    NodeId id2 = 10;
    auto context2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*context2, id2);
    SurfaceNodeCommandHelper::SetIsNotifyUIBufferAvailable(*context2, id2, available);
    ASSERT_EQ(id2, static_cast<NodeId>(10));
}

/**
 * @tc.name: TestRSSurfaceNodeCommand013
 * @tc.desc: SetSurfaceNodeType test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, TestRSSurfaceNodeCommand013, TestSize.Level1)
{
    RSContext context;
    NodeId id = -10;
    RSSurfaceNodeType type = RSSurfaceNodeType::DEFAULT;
    SurfaceNodeCommandHelper::SetSurfaceNodeType(context, id, static_cast<uint8_t>(type));
    ASSERT_EQ(id, static_cast<NodeId>(-10));
    NodeId id2 = 10;
    auto context2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*context2, id2);
    SurfaceNodeCommandHelper::SetSurfaceNodeType(*context2, id2, static_cast<uint8_t>(type));
    type = RSSurfaceNodeType::ABILITY_COMPONENT_NODE;
    SurfaceNodeCommandHelper::SetSurfaceNodeType(*context2, id2, static_cast<uint8_t>(type));
    ASSERT_EQ(id2, static_cast<NodeId>(10));
}

/**
 * @tc.name: SetSurfaceNodeTypeInvalidTest
 * @tc.desc: Verify SetSurfaceNodeType rejects out-of-range enum value
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, SetSurfaceNodeTypeInvalidTest, TestSize.Level1)
{
    RSContext context;
    NodeId id = 20;
    SurfaceNodeCommandHelper::Create(context, id);
    uint8_t invalidType = static_cast<uint8_t>(RSSurfaceNodeType::NODE_MAX);
    SurfaceNodeCommandHelper::SetSurfaceNodeType(context, id, invalidType);
    uint8_t overRangeType = static_cast<uint8_t>(RSSurfaceNodeType::NODE_MAX) + 10;
    SurfaceNodeCommandHelper::SetSurfaceNodeType(context, id, overRangeType);
    auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(id);
    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->GetSurfaceNodeType(), RSSurfaceNodeType::DEFAULT);
}

/**
 * @tc.name: TestRSSurfaceNodeCommand015
 * @tc.desc: SetContextAlpha test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, TestRSSurfaceNodeCommand015, TestSize.Level1)
{
    RSContext context;
    NodeId id = -10;
    float alpha = 0;
    SurfaceNodeCommandHelper::SetContextAlpha(context, id, alpha);
    ASSERT_EQ(id, static_cast<NodeId>(-10));
    NodeId id2 = 10;
    auto context2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*context2, id2);
    SurfaceNodeCommandHelper::SetContextAlpha(*context2, id2, alpha);
    ASSERT_EQ(id2, static_cast<NodeId>(10));
}

/**
 * @tc.name: SetContextMatrix001
 * @tc.desc: test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, SetContextMatrix001, TestSize.Level1)
{
    RSContext context;
    NodeId id = -10;
    Drawing::Matrix matrix;
    SurfaceNodeCommandHelper::SetContextMatrix(context, id, matrix);
    ASSERT_EQ(id, static_cast<NodeId>(-10));
    NodeId id2 = 10;
    auto context2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*context2, id2);
    SurfaceNodeCommandHelper::SetContextMatrix(*context2, id2, matrix);
    SurfaceNodeCommandHelper::Create(*context2, id2);
    ASSERT_EQ(id2, static_cast<NodeId>(10));
}

/**
 * @tc.name: SetContainerWindow001
 * @tc.desc: test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, SetContainerWindow001, TestSize.Level1)
{
    RSContext context;
    NodeId id = -10;
    bool hasContainerWindow = false;
    SurfaceNodeCommandHelper::SetContainerWindow(context, id, hasContainerWindow, rrect);
    ASSERT_EQ(id, static_cast<NodeId>(-10));
    NodeId id2 = 10;
    auto context2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*context2, id2);
    SurfaceNodeCommandHelper::SetContainerWindow(*context2, id2, hasContainerWindow, rrect);
    ASSERT_EQ(id2, static_cast<NodeId>(10));
}

/**
 * @tc.name: Fingerprint Test
 * @tc.desc: SetFingerprint and GetFingerprint
 * @tc.type: FUNC
 * @tc.require: issueI6Z3YK
 */
HWTEST_F(RSSurfaceNodeCommandTest, FingerprintTest001, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(-1);
    SurfaceNodeCommandHelper::SetFingerprint(context, id, true);
    SurfaceNodeCommandHelper::SetFingerprint(context, id, false);

    NodeId id2 = 1;
    SurfaceNodeCommandHelper::Create(context, id2);
    SurfaceNodeCommandHelper::SetFingerprint(context, id2, true);
    ASSERT_EQ(id2, static_cast<NodeId>(1));
}

/**
 * @tc.name: SetSkipLayerTest001
 * @tc.desc: SetSkipLayer test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, SetSkipLayerTest001, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(-1);
    bool isSkipLayer = false;
    SurfaceNodeCommandHelper::SetSkipLayer(context, id, isSkipLayer);
    NodeId id2 = 10;
    SurfaceNodeCommandHelper::Create(context, id2);
    SurfaceNodeCommandHelper::SetSkipLayer(context, id2, isSkipLayer);
    ASSERT_EQ(id2, static_cast<NodeId>(10));
}

/**
 * @tc.name: SetSnapshotSkipLayerTest001
 * @tc.desc: SetSnapshotSkipLayer test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, SetSnapshotSkipLayerTest001, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(-1);
    bool isSnapshotSkipLayer = false;
    SurfaceNodeCommandHelper::SetSnapshotSkipLayer(context, id, isSnapshotSkipLayer);
    NodeId id2 = 10;
    SurfaceNodeCommandHelper::Create(context, id2);
    SurfaceNodeCommandHelper::SetSnapshotSkipLayer(context, id2, isSnapshotSkipLayer);
    ASSERT_EQ(id2, static_cast<NodeId>(10));
}

/**
 * @tc.name: SetBootAnimation001
 * @tc.desc: SetBootAnimation test.
 * @tc.type: FUNC
 * @tc.require:SR000HSUII
 */
HWTEST_F(RSSurfaceNodeCommandTest, SetBootAnimation001, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(-1);
    SurfaceNodeCommandHelper::SetBootAnimation(context, id, false);
    NodeId id2 = 10;
    SurfaceNodeCommandHelper::Create(context, id2);
    SurfaceNodeCommandHelper::SetBootAnimation(context, id2, true);
    ASSERT_EQ(id2, static_cast<NodeId>(10));
}

/**
 * @tc.name: SetGlobalPositionEnabled001
 * @tc.desc: SetGlobalPositionEnabled test.
 * @tc.type: FUNC
 * @tc.require: issueIATYMW
 */
HWTEST_F(RSSurfaceNodeCommandTest, SetGlobalPositionEnabled001, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(-1);
    SurfaceNodeCommandHelper::SetGlobalPositionEnabled(context, id, false);
    NodeId id2 = 10;
    SurfaceNodeCommandHelper::Create(context, id2);
    SurfaceNodeCommandHelper::SetGlobalPositionEnabled(context, id2, true);
    ASSERT_EQ(id2, static_cast<NodeId>(10));
}

/**
 * @tc.name: AttachToDisplay001
 * @tc.desc: AttachToDisplay001
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSSurfaceNodeCommandTest, AttachToDisplay001, TestSize.Level1)
{
    RSContext context;
    SurfaceNodeCommandHelper::AttachToDisplay(context, 0, 1);
    context.nodeMap.renderNodeMap_[0][0] = std::make_shared<RSSurfaceRenderNode>(0);
    std::shared_ptr<RSScreenRenderNode> screenNode = nullptr;
    context.nodeMap.screenNodeMap_.emplace(0, screenNode);
    context.nodeMap.logicalDisplayNodeMap_.clear();
    EXPECT_TRUE(context.nodeMap.logicalDisplayNodeMap_.empty());

    auto logicalDisplayNode = std::make_shared<RSLogicalDisplayRenderNode>(0, RSDisplayNodeConfig());
    logicalDisplayNode.reset();
    context.nodeMap.logicalDisplayNodeMap_[0] = logicalDisplayNode;
    SurfaceNodeCommandHelper::AttachToDisplay(context, 0, 1);

    logicalDisplayNode = std::make_shared<RSLogicalDisplayRenderNode>(0, RSDisplayNodeConfig());
    logicalDisplayNode->SetScreenId(2);
    context.nodeMap.logicalDisplayNodeMap_[0] = logicalDisplayNode;
    SurfaceNodeCommandHelper::AttachToDisplay(context, 0, 1);

    context.nodeMap.logicalDisplayNodeMap_[0]->SetScreenId(1);
    context.nodeMap.logicalDisplayNodeMap_[0]->isBootAnimation_ = true;
    SurfaceNodeCommandHelper::AttachToDisplay(context, 0, 1);

    context.nodeMap.logicalDisplayNodeMap_[0]->isBootAnimation_ = false;
    context.nodeMap.logicalDisplayNodeMap_[0]->isOnTheTree_ = false;
    SurfaceNodeCommandHelper::AttachToDisplay(context, 0, 1);

    context.nodeMap.logicalDisplayNodeMap_[0]->isOnTheTree_ = true;
    SurfaceNodeCommandHelper::AttachToDisplay(context, 0, 1);
    EXPECT_EQ(context.nodeMap.logicalDisplayNodeMap_[0]->children_.size(), 0);
}

/**
 * @tc.name: DetachToDisplay001
 * @tc.desc: DetachToDisplay001
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSSurfaceNodeCommandTest, DetachToDisplay001, TestSize.Level1)
{
    RSContext context;
    SurfaceNodeCommandHelper::DetachToDisplay(context, 0, 1);
    context.nodeMap.renderNodeMap_[0][0] = std::make_shared<RSSurfaceRenderNode>(0);
    std::shared_ptr<RSScreenRenderNode> screenNode = nullptr;
    context.nodeMap.screenNodeMap_.emplace(0, screenNode);
    context.nodeMap.logicalDisplayNodeMap_.clear();
    EXPECT_TRUE(context.nodeMap.logicalDisplayNodeMap_.empty());

    auto logicalDisplayNode = std::make_shared<RSLogicalDisplayRenderNode>(0, RSDisplayNodeConfig());
    logicalDisplayNode.reset();
    context.nodeMap.logicalDisplayNodeMap_[0] = logicalDisplayNode;
    SurfaceNodeCommandHelper::DetachToDisplay(context, 0, 1);

    logicalDisplayNode = std::make_shared<RSLogicalDisplayRenderNode>(0, RSDisplayNodeConfig());
    logicalDisplayNode->SetScreenId(2);
    context.nodeMap.logicalDisplayNodeMap_[0] = logicalDisplayNode;
    SurfaceNodeCommandHelper::DetachToDisplay(context, 0, 1);

    context.nodeMap.logicalDisplayNodeMap_[0]->SetScreenId(1);
    context.nodeMap.logicalDisplayNodeMap_[0]->isBootAnimation_ = true;
    SurfaceNodeCommandHelper::DetachToDisplay(context, 0, 1);

    context.nodeMap.logicalDisplayNodeMap_[0]->isBootAnimation_ = false;
    SurfaceNodeCommandHelper::DetachToDisplay(context, 0, 1);
    EXPECT_EQ(context.nodeMap.logicalDisplayNodeMap_[0]->children_.size(), 0);
}

/**
 * @tc.name: SetIsTextureExportNode001
 * @tc.desc: DetachToDisplay test.
 * @tc.type: FUNC
 * @tc.require:SR000HSUII
 */
HWTEST_F(RSSurfaceNodeCommandTest, SetIsTextureExportNode001, TestSize.Level1)
{
    NodeId id = 10;
    RSContext context;
    SurfaceNodeCommandHelper::Create(context, id);
    SurfaceNodeCommandHelper::SetIsTextureExportNode(context, id, true);
    SurfaceNodeCommandHelper::SetIsTextureExportNode(context, 0, true);
    ASSERT_EQ(id, static_cast<NodeId>(10));
}

/**
 * @tc.name: MarkUIHidden001
 * @tc.desc: SetIsNotifyUIBufferAvailable test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, MarkUIHidden001, TestSize.Level1)
{
    RSContext context;
    NodeId id = -10;
    bool available = false;
    SurfaceNodeCommandHelper::MarkUIHidden(context, id, available);
    ASSERT_EQ(id, static_cast<NodeId>(-10));
    NodeId id2 = 10;
    SurfaceNodeCommandHelper::Create(context, id2);
    SurfaceNodeCommandHelper::MarkUIHidden(context, id2, available);
    ASSERT_EQ(id2, static_cast<NodeId>(10));
}

/**
 * @tc.name: SetForeground001
 * @tc.desc: SetIsNotifyUIBufferAvailable test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, SetForeground001, TestSize.Level1)
{
    RSContext context;
    NodeId id = -2;
    bool available = false;
    SurfaceNodeCommandHelper::SetForeground(context, id, available);
    ASSERT_EQ(id, static_cast<NodeId>(-2));
    NodeId id2 = 2;
    SurfaceNodeCommandHelper::Create(context, id2);
    SurfaceNodeCommandHelper::SetForeground(context, id2, available);
    ASSERT_EQ(id2, static_cast<NodeId>(2));
}

/**
 * @tc.name: SetDarkColorModeTest
 * @tc.desc: Verify function SetDarkColorMode
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, SetDarkColorModeTest, TestSize.Level1)
{
    RSContext context;
    SurfaceNodeCommandHelper::SetDarkColorMode(context, 0, true);
    SurfaceNodeCommandHelper::Create(context, 1);
    SurfaceNodeCommandHelper::SetDarkColorMode(context, 1, true);

    auto surfaceNode = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(1);
    ASSERT_NE(surfaceNode, nullptr);
    EXPECT_TRUE(surfaceNode->GetDarkColorMode());

    SurfaceNodeCommandHelper::SetDarkColorMode(context, 1, false);
    EXPECT_FALSE(surfaceNode->GetDarkColorMode());
}

/**
 * @tc.name: SetSurfaceId001
 * @tc.desc: SetIsNotifyUIBufferAvailable test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, SetSurfaceId001, TestSize.Level1)
{
    RSContext context;
    NodeId id = -2;
    bool available = false;
    SurfaceNodeCommandHelper::SetSurfaceId(context, id, available);
    ASSERT_EQ(id, static_cast<NodeId>(-2));
    NodeId id2 = 2;
    SurfaceNodeCommandHelper::Create(context, id2);
    SurfaceNodeCommandHelper::SetSurfaceId(context, id2, available);
    ASSERT_EQ(id2, static_cast<NodeId>(2));
}

/**
 * @tc.name: CreateWithConfigTest
 * @tc.desc: Verify function CreateWithConfig
 * @tc.type:FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSSurfaceNodeCommandTest, CreateWithConfigTest, TestSize.Level1)
{
    RSContext context;
    std::string name = "name";             // for test
    enum SurfaceWindowType windowType = SurfaceWindowType::DEFAULT_WINDOW;
    SurfaceNodeCommandHelper::CreateWithConfig(context, 1, name, 1, windowType);
    EXPECT_TRUE(context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(1) != nullptr);
}

/**
 * @tc.name: SetForceHardwareAndFixRotationTest
 * @tc.desc: Verify function SetForceHardwareAndFixRotation
 * @tc.type:FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSSurfaceNodeCommandTest, SetForceHardwareAndFixRotationTest, TestSize.Level1)
{
    RSContext context;

    SurfaceNodeCommandHelper::SetForceHardwareAndFixRotation(context, 0, false);
    SurfaceNodeCommandHelper::Create(context, 1);
    SurfaceNodeCommandHelper::SetForceHardwareAndFixRotation(context, 1, false);
    EXPECT_TRUE(context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(1) != nullptr);
}

/**
 * @tc.name: SetForceUIFirstTest
 * @tc.desc: Verify function SetForceUIFirst
 * @tc.type:FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSSurfaceNodeCommandTest, SetForceUIFirstTest, TestSize.Level1)
{
    RSContext context;
    SurfaceNodeCommandHelper::SetForceUIFirst(context, 0, false);
    SurfaceNodeCommandHelper::Create(context, 1);
    SurfaceNodeCommandHelper::SetForceUIFirst(context, 1, false);
    EXPECT_TRUE(context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(1) != nullptr);
}

/**
 * @tc.name: SetAncoFlagsTest
 * @tc.desc: Verify function SetAncoFlags
 * @tc.type:FUNC
 * @tc.require: issueIAIIEP
 */
HWTEST_F(RSSurfaceNodeCommandTest, SetAncoFlagsTest, TestSize.Level1)
{
    RSContext context;
    SurfaceNodeCommandHelper::SetAncoFlags(context, 0, 0);
    SurfaceNodeCommandHelper::Create(context, 1);
    SurfaceNodeCommandHelper::SetAncoFlags(context, 1, 1);
    EXPECT_TRUE(context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(1) != nullptr);
}

/**
 * @tc.name: SetAncoSrcCropTest
 * @tc.desc: Verify function SetAncoSrcCrop
 * @tc.type:FUNC
 * @tc.require: issueICA0I8
 */
HWTEST_F(RSSurfaceNodeCommandTest, SetAncoSrcCropTest, TestSize.Level1)
{
    RSContext context;
    Rect rect{0, 0, 0, 0};
    SurfaceNodeCommandHelper::SetAncoSrcCrop(context, 0, rect);
    SurfaceNodeCommandHelper::Create(context, 1);
    SurfaceNodeCommandHelper::SetAncoSrcCrop(context, 1, rect);
    EXPECT_TRUE(context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(1) != nullptr);
}

/**
 * @tc.name: SetSkipDraw
 * @tc.desc: Verify function SetSkipDraw
 * @tc.type: FUNC
 * @tc.require: issueI9U6LX
 */
HWTEST_F(RSSurfaceNodeCommandTest, SetSkipDrawTest, TestSize.Level1)
{
    RSContext context;
    SurfaceNodeCommandHelper::SetSkipDraw(context, 0, true);
    SurfaceNodeCommandHelper::Create(context, 1);
    SurfaceNodeCommandHelper::SetSkipDraw(context, 1, true);
    EXPECT_NE(context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(1), nullptr);
}

/**
 * @tc.name: SetAbilityState
 * @tc.desc: Verify function SetAbilityState
 * @tc.type: FUNC
 * @tc.require: issueIAQL48
 */
HWTEST_F(RSSurfaceNodeCommandTest, SetAbilityState, TestSize.Level1)
{
    RSContext context;
    SurfaceNodeCommandHelper::SetAbilityState(context, 0, RSSurfaceNodeAbilityState::FOREGROUND);
    SurfaceNodeCommandHelper::Create(context, 1);
    SurfaceNodeCommandHelper::SetAbilityState(context, 1, RSSurfaceNodeAbilityState::FOREGROUND);
    EXPECT_TRUE(context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(1) != nullptr);
}

/**
 * @tc.name: SetHardwareEnableHint
 * @tc.desc: Verify function SetHardwareEnableHint
 * @tc.type:FUNC
 * @tc.require: issueIAHFXD
 */
HWTEST_F(RSSurfaceNodeCommandTest, SetHardwareEnableHint, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 1;
    SurfaceNodeCommandHelper::Create(context, nodeId);
    SurfaceNodeCommandHelper::SetHardwareEnableHint(context, nodeId, true);
    EXPECT_TRUE(context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId) != nullptr);
}

/**
 * @tc.name: SetSourceVirtualScreenId
 * @tc.desc: Verify function SetSourceVirtualScreenId
 * @tc.type:FUNC
 * @tc.require: issueIAHFXD
 */
HWTEST_F(RSSurfaceNodeCommandTest, SetSourceVirtualScreenId, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 1;
    ScreenId screenId = {};
    SurfaceNodeCommandHelper::Create(context, nodeId);
    SurfaceNodeCommandHelper::SetSourceVirtualScreenId(context, nodeId, screenId);
    EXPECT_TRUE(context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId) != nullptr);
}

/**
 * @tc.name: AttachToWindowContainer
 * @tc.desc: Verify function AttachToWindowContainer
 * @tc.type:FUNC
 * @tc.require: issueIBIK1X
 */
HWTEST_F(RSSurfaceNodeCommandTest, AttachToWindowContainer, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 1;
    ScreenId screenId = {};
    SurfaceNodeCommandHelper::Create(context, nodeId);
    SurfaceNodeCommandHelper::AttachToWindowContainer(context, nodeId, screenId);
    EXPECT_TRUE(context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId) != nullptr);
}

/**
 * @tc.name: DetachFromWindowContainer001
 * @tc.desc: Verify function DetachFromWindowContainer001
 * @tc.type:FUNC
 * @tc.require: issueIBIK1X
 */
HWTEST_F(RSSurfaceNodeCommandTest, DetachFromWindowContainer001, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 1;
    ScreenId screenId = {};
    SurfaceNodeCommandHelper::Create(context, nodeId);
    SurfaceNodeCommandHelper::DetachFromWindowContainer(context, nodeId, screenId);
    EXPECT_TRUE(context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId) != nullptr);
}

#ifdef RS_ENABLE_UNI_RENDER
/**
 * @tc.name: DetachFromWindowContainer002
 * @tc.desc: Verify function DetachFromWindowContainer002
 * @tc.type:FUNC
 * @tc.require: issueIBIK1X
 */
HWTEST_F(RSSurfaceNodeCommandTest, DetachFromWindowContainer002, TestSize.Level1)
{
    RSContext context;
    SurfaceNodeCommandHelper::DetachFromWindowContainer(context, 0, 1);
    context.nodeMap.renderNodeMap_[0][0] = std::make_shared<RSSurfaceRenderNode>(0);
    std::shared_ptr<RSScreenRenderNode> screenNode = nullptr;
    context.nodeMap.screenNodeMap_.emplace(0, screenNode);
    context.nodeMap.logicalDisplayNodeMap_.clear();
    EXPECT_TRUE(context.nodeMap.logicalDisplayNodeMap_.empty());

    auto logicalDisplayNode = std::make_shared<RSLogicalDisplayRenderNode>(0, RSDisplayNodeConfig());
    logicalDisplayNode.reset();
    context.nodeMap.logicalDisplayNodeMap_[0] = logicalDisplayNode;
    SurfaceNodeCommandHelper::DetachFromWindowContainer(context, 0, 1);

    logicalDisplayNode = std::make_shared<RSLogicalDisplayRenderNode>(0, RSDisplayNodeConfig());
    logicalDisplayNode->SetScreenId(2);
    context.nodeMap.logicalDisplayNodeMap_[0] = logicalDisplayNode;
    SurfaceNodeCommandHelper::DetachFromWindowContainer(context, 0, 1);

    context.nodeMap.logicalDisplayNodeMap_[0]->SetScreenId(1);
    context.nodeMap.logicalDisplayNodeMap_[0]->isBootAnimation_ = true;
    SurfaceNodeCommandHelper::DetachFromWindowContainer(context, 0, 1);

    context.nodeMap.logicalDisplayNodeMap_[0]->isBootAnimation_ = false;
    SurfaceNodeCommandHelper::DetachFromWindowContainer(context, 0, 1);

    std::shared_ptr<RSSurfaceRenderNode> surfaceNodeInWindow = nullptr;
    context.nodeMap.logicalDisplayNodeMap_[0]->windowContainer_ = surfaceNodeInWindow;
    SurfaceNodeCommandHelper::DetachFromWindowContainer(context, 0, 1);

    surfaceNodeInWindow = std::make_shared<RSSurfaceRenderNode>(1);
    context.nodeMap.logicalDisplayNodeMap_[0]->windowContainer_->isOnTheTree_ = false;
    SurfaceNodeCommandHelper::DetachFromWindowContainer(context, 0, 1);

    context.nodeMap.logicalDisplayNodeMap_[0]->windowContainer_->isOnTheTree_ = true;
    SurfaceNodeCommandHelper::DetachFromWindowContainer(context, 0, 1);
    EXPECT_EQ(context.nodeMap.logicalDisplayNodeMap_[0]->children_.size(), 0);
}
#endif

/**
 * @tc.name: SetRegionToBeMagnified
 * @tc.desc: Verify function SetRegionToBeMagnified
 * @tc.type:FUNC
 * @tc.require: issueIBIK1X
 */
HWTEST_F(RSSurfaceNodeCommandTest, SetRegionToBeMagnified, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 1;
    Vector4<int> regionToBeMagnified = {0, 0, 200, 200};
    SurfaceNodeCommandHelper::Create(context, nodeId);
    SurfaceNodeCommandHelper::SetRegionToBeMagnified(context, nodeId, regionToBeMagnified);
    EXPECT_TRUE(context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId) != nullptr);
}

/**
 * @tc.name: SetFrameGravityNewVersionEnabledTest
 * @tc.desc: Verify function SetFrameGravityNewVersionEnabled
 * @tc.type: FUNC
 * @tc.require: issueIC8CDF
 */
HWTEST_F(RSSurfaceNodeCommandTest, SetFrameGravityNewVersionEnabledTest, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 1;
    SurfaceNodeCommandHelper::Create(context, nodeId);
    SurfaceNodeCommandHelper::SetFrameGravityNewVersionEnabled(context, nodeId, true);
    EXPECT_TRUE(context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId) != nullptr);
}

/**
 * @tc.name: SetSurfaceBufferOpaqueTest
 * @tc.desc: Verify function SetSurfaceBufferOpaque
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, SetSurfaceBufferOpaqueTest, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 1;
    SurfaceNodeCommandHelper::Create(context, nodeId);
    SurfaceNodeCommandHelper::SetSurfaceBufferOpaque(context, nodeId, true);
    SurfaceNodeCommandHelper::SetSurfaceBufferOpaque(context, 2, true);
    EXPECT_TRUE(context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId) != nullptr);
}

/**
 * @tc.name: SetContainerWindowTransparent
 * @tc.desc: Verify function when map can find id and can't find id
 * @tc.type: FUNC
 * @tc.require: issue19957
 */
HWTEST_F(RSSurfaceNodeCommandTest, SetContainerWindowTransparent, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 1;
    SurfaceNodeCommandHelper::Create(context, nodeId);
    SurfaceNodeCommandHelper::SetContainerWindowTransparent(context, nodeId, true);
    auto surfaceNode = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId);
    ASSERT_NE(surfaceNode, nullptr);
    ASSERT_TRUE(surfaceNode->isContainerWindowTransparent_);

    NodeId InvalidNodeId = 2;
    SurfaceNodeCommandHelper::SetContainerWindowTransparent(context, InvalidNodeId, false);
    ASSERT_TRUE(context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(InvalidNodeId) == nullptr);
}

/**
 * @tc.name: SetAppRotationCorrectionTest
 * @tc.desc: Verify function SetAppRotationCorrection
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, SetAppRotationCorrectionTest, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 1;
    SurfaceNodeCommandHelper::Create(context, nodeId);
    SurfaceNodeCommandHelper::SetAppRotationCorrection(context, nodeId, ScreenRotation::ROTATION_180);
    EXPECT_TRUE(context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId) != nullptr);
}

/**
 * @tc.name: SetHDRType
 * @tc.desc: Verify function when map can find id and can't find id
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, SetHDRTypeTest, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 1;
    SurfaceNodeCommandHelper::Create(context, nodeId);
    SurfaceNodeCommandHelper::SetHDRType(context, nodeId, 1);
    auto surfaceNode = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId);
    ASSERT_NE(surfaceNode, nullptr);
    ASSERT_EQ(surfaceNode->GetHDRType(), 1);

    NodeId InvalidNodeId = 2;
    SurfaceNodeCommandHelper::SetHDRType(context, InvalidNodeId, 0);
    ASSERT_TRUE(context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(InvalidNodeId) == nullptr);
}

#ifndef ROSEN_CROSS_PLATFORM
/**
 * @tc.name: RSSurfaceBufferInterfaceTest001
 * @tc.desc: Test RSSurfaceBufferInterface interface methods
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, RSSurfaceBufferInterfaceTest001, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 1;
    SurfaceNodeCommandHelper::Create(context, nodeId);
    auto surfaceNode = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId);
    ASSERT_NE(surfaceNode, nullptr);

    RSSurfaceBufferInterface* bufferInterface = surfaceNode.get();
    ASSERT_NE(bufferInterface, nullptr);

    EXPECT_EQ(bufferInterface->GetId(), nodeId);
    EXPECT_EQ(bufferInterface->GetName(), surfaceNode->GetName());

    RSSurfaceRenderNode* castedNode = bufferInterface->AsRSSurfaceRenderNode();
    ASSERT_NE(castedNode, nullptr);
    EXPECT_EQ(castedNode->GetId(), nodeId);
    surfaceNode->SetAncoFlags(static_cast<uint32_t>(AncoFlags::FORCE_REFRESH));
    bool forceRefreshed = bufferInterface->OnBufferAvailable();
    ASSERT_EQ(forceRefreshed, true);

    std::set<uint64_t> bufferCacheSet;
    bufferInterface->OnCleanCache(bufferCacheSet);
    bufferInterface->OnSurfaceGoBackground();
    bufferInterface->OnTransformChange();
    bufferInterface->OnTunnelHandleChange();
}

/**
 * @tc.name: SaveSurfaceHandlerInfoTest001
 * @tc.desc: Test SaveSurfaceHandlerInfo and GetSurfaceHandlerInfo
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, SaveSurfaceHandlerInfoTest001, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 1;

    auto surfaceHandler = std::make_shared<RSSurfaceHandler>(nodeId);
    sptr<IBufferConsumerListener> listener = nullptr;
    std::pair<std::shared_ptr<RSSurfaceHandler>, sptr<IBufferConsumerListener>> info = { surfaceHandler, listener };

    context.GetMutableNodeMap().SaveSurfaceHandlerInfo(nodeId, info);

    auto retrievedInfo = context.GetNodeMap().GetSurfaceHandlerInfo(nodeId);
    EXPECT_EQ(retrievedInfo.first, surfaceHandler);
    EXPECT_EQ(retrievedInfo.second, listener);

    context.GetMutableNodeMap().RemoveSurfaceHandlerInfo(nodeId);
    retrievedInfo = context.GetNodeMap().GetSurfaceHandlerInfo(nodeId);
    EXPECT_EQ(retrievedInfo.first, nullptr);
    EXPECT_EQ(retrievedInfo.second, nullptr);
}

/**
 * @tc.name: RecreateNodeCallbackTest001
 * @tc.desc: Test SetRecreateNodeCallBack and InvokeRecreateNodeCallBack
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, RecreateNodeCallbackTest001, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 1;
    bool callbackInvoked = false;
    NodeId callbackNodeId = 0;

    auto callback = [&callbackInvoked, &callbackNodeId](NodeId id, std::weak_ptr<RSSurfaceBufferInterface> surface) {
        callbackInvoked = true;
        callbackNodeId = id;
        auto surfacePtr = surface.lock();
        if (surfacePtr) {
            EXPECT_EQ(surfacePtr->GetId(), id);
        }
    };

    context.SetRecreateNodeCallback(callback);

    SurfaceNodeCommandHelper::Create(context, nodeId);
    auto surfaceNode = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId);
    ASSERT_NE(surfaceNode, nullptr);

    context.InvokeRecreateNodeCallBack(nodeId, RSSurfaceBufferInterface::WeakPtr(surfaceNode));

    EXPECT_TRUE(callbackInvoked);
    EXPECT_EQ(callbackNodeId, nodeId);
}

/**
 * @tc.name: CreateWithConfigInRSTest001
 * @tc.desc: Test CreateWithConfigInRS with custom surfaceHandler
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, CreateWithConfigInRSTest001, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 1;
    RSSurfaceRenderNodeConfig config;
    config.id = nodeId;
    config.name = "TestSurfaceNode";
    config.nodeType = RSSurfaceNodeType::DEFAULT;
    config.isTextureExportNode = false;

    auto customSurfaceHandler = std::make_shared<RSSurfaceHandler>(nodeId);
    auto node = SurfaceNodeCommandHelper::CreateWithConfigInRS(config, context, false, customSurfaceHandler);

    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->GetId(), nodeId);
    EXPECT_EQ(node->GetName(), "TestSurfaceNode");
}

#ifndef ROSEN_CROSS_PLATFORM
class MockBufferConsumerListener : public IBufferConsumerListener {
public:
    MockBufferConsumerListener() = default;
    ~MockBufferConsumerListener() override = default;
    void OnBufferAvailable() override {}
};

/**
 * @tc.name: RecreateNodeAndSurfaceTest001
 * @tc.desc: Test RecreateNodeAndSurface with invalid node id
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, RecreateNodeAndSurfaceTest001, TestSize.Level1)
{
    RSContext context;
    RSSurfaceRenderNodeConfig config;
    config.id = 1;
    SurfaceId surfaceId = 100;
    bool unobscured = false;

    auto surfaceHandler = std::make_shared<RSSurfaceHandler>(config.id);
    auto consumer = IConsumerSurface::Create();
    surfaceHandler->SetConsumer(consumer);
    sptr<IBufferConsumerListener> listener = new MockBufferConsumerListener();
    consumer->RegisterConsumerListener(listener);
    surfaceHandler->SetConsumer(consumer);

    std::pair<std::shared_ptr<RSSurfaceHandler>, sptr<IBufferConsumerListener>> info = { surfaceHandler, listener };
    context.GetMutableNodeMap().SaveSurfaceHandlerInfo(config.id, info);

    SurfaceNodeCommandHelper::RecreateNodeAndSurface(context, config, surfaceId, unobscured);

    auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(config.id);
    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->GetId(), config.id);
}

/**
 * @tc.name: RSContextQueryBundleNameTest001
 * @tc.desc: Test RSContext::QueryBundleName returns value from injected func
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, RSContextQueryBundleNameTest001, TestSize.Level1)
{
    RSContext context;
    context.SetGetBundleNameFunc([](pid_t pid) -> std::string {
        return "com.context.test";
    });
    EXPECT_EQ(context.QueryBundleName(100), "com.context.test");
}

/**
 * @tc.name: RSContextQueryBundleNameTest002
 * @tc.desc: Test RSContext::QueryBundleName returns empty when func is not set
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, RSContextQueryBundleNameTest002, TestSize.Level1)
{
    RSContext context;
    EXPECT_TRUE(context.QueryBundleName(100).empty());
}

/**
 * @tc.name: RecreateNodeAndSurfaceBundleNameTest001
 * @tc.desc: Test RecreateNodeAndSurface compensates bundleName for SELF_DRAWING_NODE
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, RecreateNodeAndSurfaceBundleNameTest001, TestSize.Level1)
{
    RSContext context;
    context.SetGetBundleNameFunc([](pid_t pid) -> std::string {
        return "com.example.app";
    });
    RSSurfaceRenderNodeConfig config;
    config.id = static_cast<NodeId>(100) << 32;  // pid = 100
    config.nodeType = RSSurfaceNodeType::SELF_DRAWING_NODE;
    SurfaceId surfaceId = 100;
    bool unobscured = false;

    auto surfaceHandler = std::make_shared<RSSurfaceHandler>(config.id);
    auto consumer = IConsumerSurface::Create();
    surfaceHandler->SetConsumer(consumer);
    sptr<IBufferConsumerListener> listener = new MockBufferConsumerListener();
    consumer->RegisterConsumerListener(listener);
    surfaceHandler->SetConsumer(consumer);

    std::pair<std::shared_ptr<RSSurfaceHandler>, sptr<IBufferConsumerListener>> info = { surfaceHandler, listener };
    context.GetMutableNodeMap().SaveSurfaceHandlerInfo(config.id, info);

    SurfaceNodeCommandHelper::RecreateNodeAndSurface(context, config, surfaceId, unobscured);

    auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(config.id);
    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->GetBundleName(), "com.example.app");
}

/**
 * @tc.name: RecreateNodeAndSurfaceBundleNameTest002
 * @tc.desc: Test RecreateNodeAndSurface does not compensate bundleName for non-SELF_DRAWING_NODE
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, RecreateNodeAndSurfaceBundleNameTest002, TestSize.Level1)
{
    RSContext context;
    context.SetGetBundleNameFunc([](pid_t pid) -> std::string {
        return "com.example.app";
    });
    RSSurfaceRenderNodeConfig config;
    config.id = static_cast<NodeId>(100) << 32;  // pid = 100
    config.nodeType = RSSurfaceNodeType::DEFAULT;
    SurfaceId surfaceId = 100;
    bool unobscured = false;

    auto surfaceHandler = std::make_shared<RSSurfaceHandler>(config.id);
    auto consumer = IConsumerSurface::Create();
    surfaceHandler->SetConsumer(consumer);
    sptr<IBufferConsumerListener> listener = new MockBufferConsumerListener();
    consumer->RegisterConsumerListener(listener);
    surfaceHandler->SetConsumer(consumer);

    std::pair<std::shared_ptr<RSSurfaceHandler>, sptr<IBufferConsumerListener>> info = { surfaceHandler, listener };
    context.GetMutableNodeMap().SaveSurfaceHandlerInfo(config.id, info);

    SurfaceNodeCommandHelper::RecreateNodeAndSurface(context, config, surfaceId, unobscured);

    auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(config.id);
    ASSERT_NE(node, nullptr);
    EXPECT_TRUE(node->GetBundleName().empty());
}

/**
 * @tc.name: RecreateNodeAndSurfaceBundleNameTest003
 * @tc.desc: Test RecreateNodeAndSurface with QueryBundleName returning empty (compensation fails)
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, RecreateNodeAndSurfaceBundleNameTest003, TestSize.Level1)
{
    RSContext context;
    context.SetGetBundleNameFunc([](pid_t pid) -> std::string {
        return "";
    });
    RSSurfaceRenderNodeConfig config;
    config.id = static_cast<NodeId>(100) << 32;  // pid = 100
    config.nodeType = RSSurfaceNodeType::SELF_DRAWING_NODE;
    SurfaceId surfaceId = 100;
    bool unobscured = false;

    auto surfaceHandler = std::make_shared<RSSurfaceHandler>(config.id);
    auto consumer = IConsumerSurface::Create();
    surfaceHandler->SetConsumer(consumer);
    sptr<IBufferConsumerListener> listener = new MockBufferConsumerListener();
    consumer->RegisterConsumerListener(listener);
    surfaceHandler->SetConsumer(consumer);

    std::pair<std::shared_ptr<RSSurfaceHandler>, sptr<IBufferConsumerListener>> info = { surfaceHandler, listener };
    context.GetMutableNodeMap().SaveSurfaceHandlerInfo(config.id, info);

    SurfaceNodeCommandHelper::RecreateNodeAndSurface(context, config, surfaceId, unobscured);

    auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(config.id);
    ASSERT_NE(node, nullptr);
    EXPECT_TRUE(node->GetBundleName().empty());
}

/**
 * @tc.name: RecreateNodeAndSurfaceBundleNameTest004
 * @tc.desc: Test RecreateNodeAndSurface overwrites bundleName for SELF_DRAWING_NODE
 *           when QueryBundleName returns non-empty
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, RecreateNodeAndSurfaceBundleNameTest004, TestSize.Level1)
{
    RSContext context;
    context.SetGetBundleNameFunc([](pid_t pid) -> std::string {
        return "com.query.result";
    });
    RSSurfaceRenderNodeConfig config;
    config.id = static_cast<NodeId>(100) << 32;  // pid = 100
    config.nodeType = RSSurfaceNodeType::SELF_DRAWING_NODE;
    config.bundleName = "com.already.set";
    SurfaceId surfaceId = 100;
    bool unobscured = false;

    auto surfaceHandler = std::make_shared<RSSurfaceHandler>(config.id);
    auto consumer = IConsumerSurface::Create();
    surfaceHandler->SetConsumer(consumer);
    sptr<IBufferConsumerListener> listener = new MockBufferConsumerListener();
    consumer->RegisterConsumerListener(listener);
    surfaceHandler->SetConsumer(consumer);

    std::pair<std::shared_ptr<RSSurfaceHandler>, sptr<IBufferConsumerListener>> info = { surfaceHandler, listener };
    context.GetMutableNodeMap().SaveSurfaceHandlerInfo(config.id, info);

    SurfaceNodeCommandHelper::RecreateNodeAndSurface(context, config, surfaceId, unobscured);

    auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(config.id);
    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->GetBundleName(), "com.query.result");
}

/**
 * @tc.name: SetBundleNameTest001
 * @tc.desc: Test RSSurfaceRenderNode::SetBundleName and GetBundleName
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, SetBundleNameTest001, TestSize.Level1)
{
    RSContext context;
    RSSurfaceRenderNodeConfig config;
    config.id = 1;
    auto node = std::make_shared<RSSurfaceRenderNode>(config, context.weak_from_this());
    EXPECT_TRUE(node->GetBundleName().empty());

    node->SetBundleName("com.test.bundle");
    EXPECT_EQ(node->GetBundleName(), "com.test.bundle");
}
#endif
#endif
} // namespace OHOS::Rosen
