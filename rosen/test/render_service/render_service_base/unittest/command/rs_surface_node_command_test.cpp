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
#include "include/command/rs_surface_node_command.h"
#include "include/pipeline/rs_surface_render_node.h"
#include "params/rs_surface_render_params.h"
#include "pipeline/rs_screen_render_node.h"

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
    EXPECT_EQ(surfaceParams->GetWatermarksEnabled().at("test"), true);
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
 * @tc.desc: AttachToDisplay test.
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSSurfaceNodeCommandTest, AttachToDisplay001, TestSize.Level1)
{
    RSContext context;
    ScreenId screenId = 1;
    SurfaceNodeCommandHelper::AttachToDisplay(context, -1, 0);

    std::shared_ptr<RSSurfaceRenderNode> renderNode = std::make_shared<RSSurfaceRenderNode>(0);
    EXPECT_NE(renderNode, nullptr);
    context.nodeMap.renderNodeMap_[0][0] = renderNode;
    std::shared_ptr<RSScreenRenderNode> screenNodeTest1 = nullptr;
    context.nodeMap.screenNodeMap_.emplace(0, screenNodeTest1);
    SurfaceNodeCommandHelper::AttachToDisplay(context, 0, 1);

    std::shared_ptr<RSContext> newContext = std::make_shared<RSContext>();
    std::shared_ptr<RSScreenRenderNode> screenNodeTest2 = std::make_shared<RSScreenRenderNode>(0, screenId, newContext);
    screenNodeTest2->screenId_ = 0;
    context.nodeMap.screenNodeMap_.at(0) = screenNodeTest2;
    EXPECT_NE(screenNodeTest2, nullptr);
    SurfaceNodeCommandHelper::AttachToDisplay(context, 0, 1);
    SurfaceNodeCommandHelper::AttachToDisplay(context, 0, 0);
}

/**
 * @tc.name: DetachToDisplay001
 * @tc.desc: DetachToDisplay test.
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSSurfaceNodeCommandTest, DetachToDisplay001, TestSize.Level1)
{
    RSContext context;
    ScreenId screenId = 1;
    SurfaceNodeCommandHelper::DetachToDisplay(context, -1, 0);
    std::shared_ptr<RSSurfaceRenderNode> renderNode = std::make_shared<RSSurfaceRenderNode>(0);
    EXPECT_NE(renderNode, nullptr);
    context.nodeMap.renderNodeMap_[0][0] = renderNode;
    std::shared_ptr<RSScreenRenderNode> screenNodeTest1 = nullptr;
    context.nodeMap.screenNodeMap_.emplace(0, screenNodeTest1);
    SurfaceNodeCommandHelper::DetachToDisplay(context, 0, 1);

    std::shared_ptr<RSContext> newContext = std::make_shared<RSContext>();
    std::shared_ptr<RSScreenRenderNode> screenNodeTest2 = std::make_shared<RSScreenRenderNode>(0, screenId, newContext);
    screenNodeTest2->screenId_ = 0;
    context.nodeMap.screenNodeMap_.at(0) = screenNodeTest2;
    EXPECT_NE(screenNodeTest2, nullptr);
    SurfaceNodeCommandHelper::DetachToDisplay(context, 0, 1);
    SurfaceNodeCommandHelper::DetachToDisplay(context, 0, 0);
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
 * @tc.name: SetAnimationFinished001
 * @tc.desc: SetIsNotifyUIBufferAvailable test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandTest, SetAnimationFinished001, TestSize.Level1)
{
    RSContext context;
    NodeId id = -10;
    SurfaceNodeCommandHelper::SetAnimationFinished(context, id);
    ASSERT_EQ(id, static_cast<NodeId>(-10));
    NodeId id2 = 10;
    SurfaceNodeCommandHelper::Create(context, id2);
    SurfaceNodeCommandHelper::SetAnimationFinished(context, id2);
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
 * @tc.name: SetHDRPresentTest
 * @tc.desc: Verify function SetHDRPresent
 * @tc.type:FUNC
 * @tc.require: issueI9SBEZ
 */
HWTEST_F(RSSurfaceNodeCommandTest, SetHDRPresentTest, TestSize.Level1)
{
    RSContext context;
    SurfaceNodeCommandHelper::SetHDRPresent(context, 0, false);
    SurfaceNodeCommandHelper::Create(context, 1);
    SurfaceNodeCommandHelper::SetHDRPresent(context, 1, false);
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
 * @tc.name: SetSourceVirtualDisplayId
 * @tc.desc: Verify function SetSourceVirtualDisplayId
 * @tc.type:FUNC
 * @tc.require: issueIAHFXD
 */
HWTEST_F(RSSurfaceNodeCommandTest, SetSourceVirtualDisplayId, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 1;
    ScreenId screenId = {};
    SurfaceNodeCommandHelper::Create(context, nodeId);
    SurfaceNodeCommandHelper::SetSourceVirtualDisplayId(context, nodeId, screenId);
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
 * @tc.name: DetachFromWindowContainer
 * @tc.desc: Verify function DetachFromWindowContainer
 * @tc.type:FUNC
 * @tc.require: issueIBIK1X
 */
HWTEST_F(RSSurfaceNodeCommandTest, DetachFromWindowContainer, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 1;
    ScreenId screenId = {};
    SurfaceNodeCommandHelper::Create(context, nodeId);
    SurfaceNodeCommandHelper::DetachFromWindowContainer(context, nodeId, screenId);
    EXPECT_TRUE(context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId) != nullptr);
}

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
    Vector4f regionToBeMagnified = {0.0, 0.0, 200.0, 200.0};
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
} // namespace OHOS::Rosen
