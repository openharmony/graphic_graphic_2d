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
#include "include/command/rs_surface_node_command.h"
#include "include/pipeline/rs_surface_render_node.h"
#include "params/rs_surface_render_params.h"
#include "pipeline/rs_display_render_node.h"

using namespace testing;
using namespace testing::Text;

namespace OHOS::Rosen {
class RSSurfaceNodeCommandUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static constexpr float outerRadius = 30.4f;
    RRect rrect = RRect({0, 0, 0, 0}, outerRadius, outerRadius);
};

void RSSurfaceNodeCommandUnitTest::SetUpTestCase() {}
void RSSurfaceNodeCommandUnitTest::TearDownTestCase() {}
void RSSurfaceNodeCommandUnitTest::SetUp() {}
void RSSurfaceNodeCommandUnitTest::TearDown() {}

/**
 * @tc.name: TestRSSurfaceNodeCommand02
 * @tc.desc: Verify function ConnectToNodeInRenderService
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandUnitTest, TestRSSurfaceNodeCommand02, TestSize.Level1)
{
    RSContext testContext;
    NodeId id = static_cast<NodeId>(-1);
    SurfaceNodeCommandHelper::ConnectToNodeInRenderService(testContext, id);
    ASSERT_EQ(id, static_cast<NodeId>(-1));
    NodeId id2 = 10;
    auto testContext2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*testContext2, id2);
    SurfaceNodeCommandHelper::ConnectToNodeInRenderService(*testContext2, id2);
    ASSERT_EQ(id2, static_cast<NodeId>(10));
}

/**
 * @tc.name: TestRSSurfaceNodeCommand03
 * @tc.desc: Verify function SetContextClipRegion
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandUnitTest, TestRSSurfaceNodeCommand03, TestSize.Level1)
{
    RSContext testContext;
    NodeId id = static_cast<NodeId>(-1);
    Drawing::Rect clipRect;
    SurfaceNodeCommandHelper::SetContextClipRegion(testContext, id, clipRect);
    ASSERT_EQ(id, static_cast<NodeId>(-1));
    NodeId id2 = 10;
    auto testContext2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*testContext2, id2);
    SurfaceNodeCommandHelper::SetContextClipRegion(*testContext2, id2, clipRect);
    ASSERT_EQ(id2, static_cast<NodeId>(10));
}

/**
 * @tc.name: TestSetWatermarkEnabled
 * @tc.desc: Verify function SetWatermarkEnabled
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceNodeCommandUnitTest, TestSetWatermarkEnabled, TestSize.Level1)
{
    RSContext testContext;
    NodeId nodeId = static_cast<NodeId>(-1);
    SurfaceNodeCommandHelper::SetWatermarkEnabled(testContext, nodeId, "test", true);
    ASSERT_EQ(testContext.GetNodeMap().GetRenderNode<RSRenderNode>(nodeId), nullptr);
    nodeId = 10;
    SurfaceNodeCommandHelper::Create(testContext, nodeId);
    SurfaceNodeCommandHelper::SetWatermarkEnabled(testContext, nodeId, "test", true);
    auto surfaceNode = testContext.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId);
    ASSERT_NE(surfaceNode, nullptr);
    ASSERT_NE(surfaceNode->stagingRenderParams_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNode->stagingRenderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    EXPECT_EQ(surfaceParams->GetWatermarksEnabled().at("test"), true);
}

/**
 * @tc.name: TestSetLeashPersistentId
 * @tc.desc: Verify function SetLeashPersistentId
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceNodeCommandUnitTest, TestSetLeashPersistentId, TestSize.Level1)
{
    RSContext testContext;
    NodeId nodeId = static_cast<NodeId>(-1);
    LeashPersistentId leashPersistentId = 1;
    SurfaceNodeCommandHelper::SetLeashPersistentId(testContext, nodeId, leashPersistentId);
    ASSERT_EQ(testContext.GetNodeMap().GetRenderNode<RSRenderNode>(nodeId), nullptr);
    nodeId = 10;
    SurfaceNodeCommandHelper::Create(testContext, nodeId);
    SurfaceNodeCommandHelper::SetLeashPersistentId(testContext, nodeId, leashPersistentId);
    auto surfaceNode = testContext.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId);
    ASSERT_NE(surfaceNode, nullptr);
    EXPECT_EQ(surfaceNode->leashPersistentId_, leashPersistentId);
}

/**
 * @tc.name: TestRSSurfaceNodeCommand04
 * @tc.desc: Verify function SetSecurityLayer
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandUnitTest, TestRSSurfaceNodeCommand04, TestSize.Level1)
{
    RSContext testContext;
    NodeId id = static_cast<NodeId>(-1);
    bool isSecurityLayer = false;
    SurfaceNodeCommandHelper::SetSecurityLayer(testContext, id, isSecurityLayer);
    ASSERT_EQ(id, static_cast<NodeId>(-1));
    NodeId id2 = 10;
    auto testContext2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*testContext2, id2);
    SurfaceNodeCommandHelper::SetSecurityLayer(*testContext2, id2, isSecurityLayer);
    ASSERT_EQ(id2, static_cast<NodeId>(10));
}

/**
 * @tc.name: TestRSSurfaceNodeCommand05
 * @tc.desc: Verify function SetColorSpace
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandUnitTest, TestRSSurfaceNodeCommand05, TestSize.Level1)
{
    RSContext testContext;
    NodeId id = static_cast<NodeId>(-1);
    GraphicColorGamut colorSpace = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_INVALID;
    SurfaceNodeCommandHelper::SetColorSpace(testContext, id, colorSpace);
    ASSERT_EQ(id, static_cast<NodeId>(-1));
    NodeId id2 = 10;
    auto testContext2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*testContext2, id2);
    SurfaceNodeCommandHelper::SetColorSpace(*testContext2, id2, colorSpace);
    ASSERT_EQ(id2, static_cast<NodeId>(10));
}

/**
 * @tc.name: TestRSSurfaceNodeCommand06
 * @tc.desc: Verify function UpdateSurfaceDefaultSize
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandUnitTest, TestRSSurfaceNodeCommand06, TestSize.Level1)
{
    RSContext testContext;
    NodeId id = static_cast<NodeId>(-1);
    float width = 0;
    float height = 0;
    SurfaceNodeCommandHelper::UpdateSurfaceDefaultSize(testContext, id, width, height);
    ASSERT_EQ(id, static_cast<NodeId>(-1));
    NodeId id2 = 10;
    auto testContext2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*testContext2, id2);
    SurfaceNodeCommandHelper::UpdateSurfaceDefaultSize(*testContext2, id2, width, height);
    ASSERT_EQ(id2, static_cast<NodeId>(10));
}

/**
 * @tc.name: TestRSSurfaceNodeCommand07
 * @tc.desc: Verify function ConnectToNodeInRenderService
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandUnitTest, TestRSSurfaceNodeCommand07, TestSize.Level1)
{
    RSContext testContext;
    NodeId id = static_cast<NodeId>(-1);
    SurfaceNodeCommandHelper::ConnectToNodeInRenderService(testContext, id);
    ASSERT_EQ(id, static_cast<NodeId>(-1));
    NodeId id2 = 10;
    auto testContext2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*testContext2, id2);
    SurfaceNodeCommandHelper::ConnectToNodeInRenderService(*testContext2, id2);
    ASSERT_EQ(id2, static_cast<NodeId>(10));
}

/**
 * @tc.name: TestRSSurfaceNodeCommand08
 * @tc.desc: Verify function SetCallbackForRenderThreadRefresh
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandUnitTest, TestRSSurfaceNodeCommand08, TestSize.Level1)
{
    RSContext testContext;
    NodeId id = static_cast<NodeId>(-1);
    SurfaceNodeCommandHelper::SetCallbackForRenderThreadRefresh(testContext, id, true);
    ASSERT_EQ(id, static_cast<NodeId>(-1));
    NodeId id2 = 10;
    auto testContext2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*testContext2, id2);
    SurfaceNodeCommandHelper::SetCallbackForRenderThreadRefresh(*testContext2, id2, false);
    ASSERT_EQ(id2, static_cast<NodeId>(10));
}

/**
 * @tc.name: TestRSSurfaceNodeCommand09
 * @tc.desc: Verify function SetContextBounds
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandUnitTest, TestRSSurfaceNodeCommand09, TestSize.Level1)
{
    RSContext testContext;
    NodeId id = static_cast<NodeId>(-1);
    Vector4f bounds;
    SurfaceNodeCommandHelper::SetContextBounds(testContext, id, bounds);
    ASSERT_EQ(id, static_cast<NodeId>(-1));
    NodeId id2 = 10;
    auto testContext2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*testContext2, id2);
    SurfaceNodeCommandHelper::SetContextBounds(*testContext2, id2, bounds);
    ASSERT_EQ(id2, static_cast<NodeId>(10));
}

/**
 * @tc.name: TestRSSurfaceNodeCommand01
 * @tc.desc: Verify function SetAbilityBGAlpha
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandUnitTest, TestRSSurfaceNodeCommand01, TestSize.Level1)
{
    RSContext testContext;
    NodeId id = static_cast<NodeId>(-1);
    uint8_t alpha = static_cast<uint8_t>(0);
    SurfaceNodeCommandHelper::SetAbilityBGAlpha(testContext, id, alpha);
    ASSERT_EQ(id, static_cast<NodeId>(-1));
    NodeId id2 = 10;
    auto testContext2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*testContext2, id2);
    SurfaceNodeCommandHelper::SetAbilityBGAlpha(*testContext2, id2, alpha);
    ASSERT_EQ(id2, static_cast<NodeId>(10));
}

/**
 * @tc.name: TestRSSurfaceNodeCommand01
 * @tc.desc: Verify function SetIsNotifyUIBufferAvailable
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandUnitTest, TestRSSurfaceNodeCommand01, TestSize.Level1)
{
    RSContext testContext;
    NodeId id = -10;
    bool available = false;
    SurfaceNodeCommandHelper::SetIsNotifyUIBufferAvailable(testContext, id, available);
    ASSERT_EQ(id, static_cast<NodeId>(-10));
    NodeId id2 = 10;
    auto testContext2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*testContext2, id2);
    SurfaceNodeCommandHelper::SetIsNotifyUIBufferAvailable(*testContext2, id2, available);
    ASSERT_EQ(id2, static_cast<NodeId>(10));
}

/**
 * @tc.name: TestRSSurfaceNodeCommand01
 * @tc.desc: Verify function SetSurfaceNodeType
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandUnitTest, TestRSSurfaceNodeCommand01, TestSize.Level1)
{
    RSContext testContext;
    NodeId id = -10;
    RSSurfaceNodeType type = RSSurfaceNodeType::DEFAULT;
    SurfaceNodeCommandHelper::SetSurfaceNodeType(testContext, id, static_cast<uint8_t>(type));
    ASSERT_EQ(id, static_cast<NodeId>(-10));
    NodeId id2 = 10;
    auto testContext2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*testContext2, id2);
    SurfaceNodeCommandHelper::SetSurfaceNodeType(*testContext2, id2, static_cast<uint8_t>(type));
    type = RSSurfaceNodeType::ABILITY_COMPONENT_NODE;
    SurfaceNodeCommandHelper::SetSurfaceNodeType(*testContext2, id2, static_cast<uint8_t>(type));
    ASSERT_EQ(id2, static_cast<NodeId>(10));
}

/**
 * @tc.name: TestRSSurfaceNodeCommand01
 * @tc.desc: Verify function SetContextAlpha
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandUnitTest, TestRSSurfaceNodeCommand01, TestSize.Level1)
{
    RSContext testContext;
    NodeId id = -10;
    float alpha = 0;
    SurfaceNodeCommandHelper::SetContextAlpha(testContext, id, alpha);
    ASSERT_EQ(id, static_cast<NodeId>(-10));
    NodeId id2 = 10;
    auto testContext2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*testContext2, id2);
    SurfaceNodeCommandHelper::SetContextAlpha(*testContext2, id2, alpha);
    ASSERT_EQ(id2, static_cast<NodeId>(10));
}

/**
 * @tc.name: TestSetContextMatrix01
 * @tc.desc: Verify function SetContextMatrix
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandUnitTest, TestSetContextMatrix01, TestSize.Level1)
{
    RSContext testContext;
    NodeId id = -10;
    Drawing::Matrix matrix;
    SurfaceNodeCommandHelper::SetContextMatrix(testContext, id, matrix);
    ASSERT_EQ(id, static_cast<NodeId>(-10));
    NodeId id2 = 10;
    auto testContext2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*testContext2, id2);
    SurfaceNodeCommandHelper::SetContextMatrix(*testContext2, id2, matrix);
    SurfaceNodeCommandHelper::Create(*testContext2, id2);
    ASSERT_EQ(id2, static_cast<NodeId>(10));
}

/**
 * @tc.name: TestSetContainerWindow01
 * @tc.desc: Verify function SetContainerWindow
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandUnitTest, TestSetContainerWindow01, TestSize.Level1)
{
    RSContext testContext;
    NodeId id = -10;
    bool hasContainerWindow = false;
    SurfaceNodeCommandHelper::SetContainerWindow(testContext, id, hasContainerWindow, rrect);
    ASSERT_EQ(id, static_cast<NodeId>(-10));
    NodeId id2 = 10;
    auto testContext2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*testContext2, id2);
    SurfaceNodeCommandHelper::SetContainerWindow(*testContext2, id2, hasContainerWindow, rrect);
    ASSERT_EQ(id2, static_cast<NodeId>(10));
}

/**
 * @tc.name: TestFingerprint01
 * @tc.desc: Verify function SetFingerprint and GetFingerprint
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceNodeCommandUnitTest, TestFingerprint01, TestSize.Level1)
{
    RSContext testContext;
    NodeId id = static_cast<NodeId>(-1);
    SurfaceNodeCommandHelper::SetFingerprint(testContext, id, true);
    SurfaceNodeCommandHelper::SetFingerprint(testContext, id, false);
    ASSERT_EQ(id, static_cast<NodeId>(-1));

    NodeId id2 = static_cast<NodeId>(1);
    SurfaceNodeCommandHelper::Create(testContext, id2);
    SurfaceNodeCommandHelper::SetFingerprint(testContext, id2, true);
    ASSERT_EQ(id2, static_cast<NodeId>(1));
}

/**
 * @tc.name: TestSetSkipLayer01
 * @tc.desc: Verify function SetSkipLayer
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandUnitTest, TestSetSkipLayer01, TestSize.Level1)
{
    RSContext testContext;
    NodeId id = static_cast<NodeId>(-1);
    bool isSkipLayer = false;
    SurfaceNodeCommandHelper::SetSkipLayer(testContext, id, isSkipLayer);
    ASSERT_EQ(id, static_cast<NodeId>(-1));
    NodeId id2 = 10;
    SurfaceNodeCommandHelper::Create(testContext, id2);
    SurfaceNodeCommandHelper::SetSkipLayer(testContext, id2, isSkipLayer);
    ASSERT_EQ(id2, static_cast<NodeId>(10));
}

/**
 * @tc.name: TestSetSnapshotSkipLayer01
 * @tc.desc: Verify function SetSnapshotSkipLayer
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandUnitTest, TestSetSnapshotSkipLayer01, TestSize.Level1)
{
    RSContext testContext;
    NodeId id = static_cast<NodeId>(-1);
    bool isSnapshotSkipLayer = false;
    SurfaceNodeCommandHelper::SetSnapshotSkipLayer(testContext, id, isSnapshotSkipLayer);
    ASSERT_EQ(id, static_cast<NodeId>(-1));
    NodeId id2 = 10;
    SurfaceNodeCommandHelper::Create(testContext, id2);
    SurfaceNodeCommandHelper::SetSnapshotSkipLayer(testContext, id2, isSnapshotSkipLayer);
    ASSERT_EQ(id2, static_cast<NodeId>(10));
}

/**
 * @tc.name: TestSetBootAnimation01
 * @tc.desc: Verify function SetBootAnimation
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceNodeCommandUnitTest, TestSetBootAnimation01, TestSize.Level1)
{
    RSContext testContext;
    NodeId id = static_cast<NodeId>(-1);
    SurfaceNodeCommandHelper::SetBootAnimation(testContext, id, false);
    ASSERT_EQ(id, static_cast<NodeId>(-1));
    NodeId id2 = 10;
    SurfaceNodeCommandHelper::Create(testContext, id2);
    SurfaceNodeCommandHelper::SetBootAnimation(testContext, id2, true);
    ASSERT_EQ(id2, static_cast<NodeId>(10));
}

/**
 * @tc.name: TestSetGlobalPositionEnabled01
 * @tc.desc: Verify function SetGlobalPositionEnabled
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceNodeCommandUnitTest, TestSetGlobalPositionEnabled01, TestSize.Level1)
{
    RSContext testContext;
    NodeId id = static_cast<NodeId>(-1);
    SurfaceNodeCommandHelper::SetGlobalPositionEnabled(testContext, id, false);
    ASSERT_EQ(id, static_cast<NodeId>(-1));
    NodeId id2 = 10;
    SurfaceNodeCommandHelper::Create(testContext, id2);
    SurfaceNodeCommandHelper::SetGlobalPositionEnabled(testContext, id2, true);
    ASSERT_EQ(id2, static_cast<NodeId>(10));
}

/**
 * @tc.name: TestAttachToDisplay01
 * @tc.desc: Verify function AttachToDisplay
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceNodeCommandUnitTest, TestAttachToDisplay01, TestSize.Level1)
{
    RSContext testContext;
    SurfaceNodeCommandHelper::AttachToDisplay(testContext, -1, 0);

    std::shared_ptr<RSSurfaceRenderNode> renderNode = std::make_shared<RSSurfaceRenderNode>(0);
    EXPECT_NE(renderNode, nullptr);
    testContext.nodeMap.renderNodeMap_[0][0] = renderNode;
    std::shared_ptr<RSDisplayRenderNode> displayNodeTest1 = nullptr;
    testContext.nodeMap.displayNodeMap_.emplace(0, displayNodeTest1);
    SurfaceNodeCommandHelper::AttachToDisplay(testContext, 0, 1);

    RSDisplayNodeConfig config;
    std::shared_ptr<RSDisplayRenderNode> displayNodeTest2 = std::make_shared<RSDisplayRenderNode>(0, config);
    displayNodeTest2->screenId_ = 0;
    testContext.nodeMap.displayNodeMap_.at(0) = displayNodeTest2;
    EXPECT_NE(displayNodeTest2, nullptr);
    SurfaceNodeCommandHelper::AttachToDisplay(testContext, 0, 1);
    SurfaceNodeCommandHelper::AttachToDisplay(testContext, 0, 0);
}

/**
 * @tc.name: TestDetachToDisplay01
 * @tc.desc: Verify function DetachToDisplay
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceNodeCommandUnitTest, TestDetachToDisplay01, TestSize.Level1)
{
    RSContext testContext;
    SurfaceNodeCommandHelper::DetachToDisplay(testContext, -1, 0);
    std::shared_ptr<RSSurfaceRenderNode> renderNode = std::make_shared<RSSurfaceRenderNode>(0);
    EXPECT_NE(renderNode, nullptr);
    testContext.nodeMap.renderNodeMap_[0][0] = renderNode;
    std::shared_ptr<RSDisplayRenderNode> displayNodeTest1 = nullptr;
    testContext.nodeMap.displayNodeMap_.emplace(0, displayNodeTest1);
    SurfaceNodeCommandHelper::DetachToDisplay(testContext, 0, 1);

    RSDisplayNodeConfig config;
    std::shared_ptr<RSDisplayRenderNode> displayNodeTest2 = std::make_shared<RSDisplayRenderNode>(0, config);
    displayNodeTest2->screenId_ = 0;
    testContext.nodeMap.displayNodeMap_.at(0) = displayNodeTest2;
    EXPECT_NE(displayNodeTest2, nullptr);
    SurfaceNodeCommandHelper::DetachToDisplay(testContext, 0, 1);
    SurfaceNodeCommandHelper::DetachToDisplay(testContext, 0, 0);
}

/**
 * @tc.name: TestSetIsTextureExportNode01
 * @tc.desc: Verify function SetIsTextureExportNode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceNodeCommandUnitTest, TestSetIsTextureExportNode01, TestSize.Level1)
{
    NodeId id = 10;
    RSContext testContext;
    SurfaceNodeCommandHelper::Create(testContext, id);
    SurfaceNodeCommandHelper::SetIsTextureExportNode(testContext, id, true);
    SurfaceNodeCommandHelper::SetIsTextureExportNode(testContext, 0, true);
    ASSERT_EQ(id, static_cast<NodeId>(10));
}

/**
 * @tc.name: TestMarkUIHidden01
 * @tc.desc: Verify function MarkUIHidden
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandUnitTest, TestMarkUIHidden01, TestSize.Level1)
{
    RSContext testContext;
    NodeId id = -10;
    bool available = false;
    SurfaceNodeCommandHelper::MarkUIHidden(testContext, id, available);
    ASSERT_EQ(id, static_cast<NodeId>(-10));
    NodeId id2 = 10;
    SurfaceNodeCommandHelper::Create(testContext, id2);
    SurfaceNodeCommandHelper::MarkUIHidden(testContext, id2, available);
    ASSERT_EQ(id2, static_cast<NodeId>(10));
}

/**
 * @tc.name: TestSetAnimationFinished01
 * @tc.desc: Verify function SetAnimationFinished
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandUnitTest, TestSetAnimationFinished01, TestSize.Level1)
{
    RSContext testContext;
    NodeId id = -10;
    SurfaceNodeCommandHelper::SetAnimationFinished(testContext, id);
    ASSERT_EQ(id, static_cast<NodeId>(-10));
    NodeId id2 = 10;
    SurfaceNodeCommandHelper::Create(testContext, id2);
    SurfaceNodeCommandHelper::SetAnimationFinished(testContext, id2);
    ASSERT_EQ(id2, static_cast<NodeId>(10));
}

/**
 * @tc.name: TestSetForeground01
 * @tc.desc: Verify function SetForeground
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandUnitTest, TestSetForeground01, TestSize.Level1)
{
    RSContext testContext;
    NodeId id = -2;
    bool available = false;
    SurfaceNodeCommandHelper::SetForeground(testContext, id, available);
    ASSERT_EQ(id, static_cast<NodeId>(-2));
    NodeId id2 = 2;
    SurfaceNodeCommandHelper::Create(testContext, id2);
    SurfaceNodeCommandHelper::SetForeground(testContext, id2, available);
    ASSERT_EQ(id2, static_cast<NodeId>(2));
}

/**
 * @tc.name: TestSetSurfaceId01
 * @tc.desc: Verify function SetSurfaceId
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandUnitTest, TestSetSurfaceId01, TestSize.Level1)
{
    RSContext testContext;
    NodeId id = -2;
    bool available = false;
    SurfaceNodeCommandHelper::SetSurfaceId(testContext, id, available);
    ASSERT_EQ(id, static_cast<NodeId>(-2));
    NodeId id2 = 2;
    SurfaceNodeCommandHelper::Create(testContext, id2);
    SurfaceNodeCommandHelper::SetSurfaceId(testContext, id2, available);
    ASSERT_EQ(id2, static_cast<NodeId>(2));
}

/**
 * @tc.name: TestCreateWithConfig01
 * @tc.desc: Verify function CreateWithConfig
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceNodeCommandUnitTest, TestCreateWithConfig01, TestSize.Level1)
{
    RSContext testContext;
    std::string name = "name";             // for test
    enum SurfaceWindowType windowType = SurfaceWindowType::DEFAULT_WINDOW;
    SurfaceNodeCommandHelper::CreateWithConfig(testContext, 1, name, 1, windowType);
    EXPECT_TRUE([&testContext]() -> bool {
        auto& renderNodemap = testContext.GetMutableNodeMap().renderNodeMap_;
        auto iter = renderNodemap.find(ExtractPid(1));
        if (iter != renderNodemap.end()) {
            auto& submap = iter->second;
            if (submap.find(1) != submap.end()) {
                return true;
            }
        }
        return false;
    }());
}

/**
 * @tc.name: TestSetForceHardwareAndFixRotation01
 * @tc.desc: Verify function SetForceHardwareAndFixRotation
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceNodeCommandUnitTest, TestSetForceHardwareAndFixRotation01, TestSize.Level1)
{
    RSContext testContext;

    SurfaceNodeCommandHelper::SetForceHardwareAndFixRotation(testContext, 0, false);
    SurfaceNodeCommandHelper::Create(testContext, 1);
    SurfaceNodeCommandHelper::SetForceHardwareAndFixRotation(testContext, 1, false);
    EXPECT_TRUE(testContext.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(1) != nullptr);
}

/**
 * @tc.name: TestSetForceUIFirst01
 * @tc.desc: Verify function SetForceUIFirst
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceNodeCommandUnitTest, TestSetForceUIFirst01, TestSize.Level1)
{
    RSContext testContext;
    SurfaceNodeCommandHelper::SetForceUIFirst(testContext, 0, false);
    SurfaceNodeCommandHelper::Create(testContext, 1);
    SurfaceNodeCommandHelper::SetForceUIFirst(testContext, 1, false);
    EXPECT_TRUE(testContext.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(1) != nullptr);
}

/**
 * @tc.name: TestSetAncoFlags01
 * @tc.desc: Verify function SetAncoFlags
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceNodeCommandUnitTest, TestSetAncoFlags01, TestSize.Level1)
{
    RSContext testContext;
    SurfaceNodeCommandHelper::SetAncoFlags(testContext, 0, 0);
    SurfaceNodeCommandHelper::Create(testContext, 1);
    SurfaceNodeCommandHelper::SetAncoFlags(testContext, 1, 1);
    EXPECT_TRUE(testContext.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(1) != nullptr);
}

/**
 * @tc.name: TestSetHDRPresent01
 * @tc.desc: Verify function SetHDRPresent
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceNodeCommandUnitTest, TestSetHDRPresent01, TestSize.Level1)
{
    RSContext testContext;
    SurfaceNodeCommandHelper::SetHDRPresent(testContext, 0, false);
    SurfaceNodeCommandHelper::Create(testContext, 1);
    SurfaceNodeCommandHelper::SetHDRPresent(testContext, 1, false);
    EXPECT_TRUE(testContext.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(1) != nullptr);
}

/**
 * @tc.name: TestSetSkipDraw01
 * @tc.desc: Verify function SetSkipDraw
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceNodeCommandUnitTest, TestSetSkipDraw01, TestSize.Level1)
{
    RSContext testContext;
    SurfaceNodeCommandHelper::SetSkipDraw(testContext, 0, true);
    SurfaceNodeCommandHelper::Create(testContext, 1);
    SurfaceNodeCommandHelper::SetSkipDraw(testContext, 1, true);
    EXPECT_TRUE(testContext.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(1) != nullptr);
}

/**
 * @tc.name: TestSetAbilityState01
 * @tc.desc: Verify function SetAbilityState
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceNodeCommandUnitTest, TestSetAbilityState01, TestSize.Level1)
{
    RSContext testContext;
    SurfaceNodeCommandHelper::SetAbilityState(testContext, 0, RSSurfaceNodeAbilityState::FOREGROUND);
    SurfaceNodeCommandHelper::Create(testContext, 1);
    SurfaceNodeCommandHelper::SetAbilityState(testContext, 1, RSSurfaceNodeAbilityState::FOREGROUND);
    EXPECT_TRUE(testContext.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(1) != nullptr);
}
} // namespace OHOS::Rosen
