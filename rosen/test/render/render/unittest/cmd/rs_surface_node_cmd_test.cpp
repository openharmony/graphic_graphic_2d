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
#include "pipeline/rs_display_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSSurfaceNodeCmdTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static constexpr float outerRadius = 30.4f;
    RRect rrect = RRect({0, 0, 0, 0}, outerRadius, outerRadius);
};

void RSSurfaceNodeCmdTest::SetUpTestCase() {}
void RSSurfaceNodeCmdTest::TearDownTestCase() {}
void RSSurfaceNodeCmdTest::SetUp() {}
void RSSurfaceNodeCmdTest::TearDown() {}

/**
 * @tc.name: SetContextMatrixUniTest001
 * @tc.desc: test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCmdTest, SetContextMatrixUniTest001, TestSize.Level1)
{
    RSContext context;
    NodeId id = -10;
    Drawing::Matrix matrix;
    SurfaceNodeCommandHelper::SetContextMatrix(context, id, matrix);
    NodeId id2 = 10;
    auto context2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*context2, id2);
    SurfaceNodeCommandHelper::SetContextMatrix(*context2, id2, matrix);
    SurfaceNodeCommandHelper::Create(*context2, id2);
}

/**
 * @tc.name: TestRSSurfaceNodeCmdUniTest002
 * @tc.desc: ConnectToNodeInRenderService test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCmdTest, TestRSSurfaceNodeCmdUniTest002, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(-1);
    SurfaceNodeCommandHelper::ConnectToNodeInRenderService(context, id);
    NodeId id2 = 10;
    auto context2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*context2, id2);
    SurfaceNodeCommandHelper::ConnectToNodeInRenderService(*context2, id2);
}

/**
 * @tc.name: TestRSSurfaceNodeCmdUniTest003
 * @tc.desc: SetContextClipRegion test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCmdTest, TestRSSurfaceNodeCmdUniTest003, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(-1);
    Drawing::Rect clipRect;
    SurfaceNodeCommandHelper::SetContextClipRegion(context, id, clipRect);
    NodeId id2 = 10;
    auto context2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*context2, id2);
    SurfaceNodeCommandHelper::SetContextClipRegion(*context2, id2, clipRect);
}

/**
 * @tc.name: TestRSSurfaceNodeCmdUniTest004
 * @tc.desc: SetSecurityLayer test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCmdTest, TestRSSurfaceNodeCmdUniTest004, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(-1);
    bool isSecurityLayer = false;
    SurfaceNodeCommandHelper::SetSecurityLayer(context, id, isSecurityLayer);
    NodeId id2 = 10;
    auto context2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*context2, id2);
    SurfaceNodeCommandHelper::SetSecurityLayer(*context2, id2, isSecurityLayer);
}

/**
 * @tc.name: TestRSSurfaceNodeCmdUniTest005
 * @tc.desc: SetColorSpace test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCmdTest, TestRSSurfaceNodeCmdUniTest005, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(-1);
    GraphicColorGamut colorSpace = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_INVALID;
    SurfaceNodeCommandHelper::SetColorSpace(context, id, colorSpace);
    NodeId id2 = 10;
    auto context2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*context2, id2);
    SurfaceNodeCommandHelper::SetColorSpace(*context2, id2, colorSpace);
}

/**
 * @tc.name: SetGlobalPositionEnabledUniTest001
 * @tc.desc: SetGlobalPositionEnabled test.
 * @tc.type: FUNC
 * @tc.require: issueIATYMW
 */
HWTEST_F(RSSurfaceNodeCmdTest, SetGlobalPositionEnabledUniTest001, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(-1);
    SurfaceNodeCommandHelper::SetGlobalPositionEnabled(context, id, false);
    NodeId id2 = 10;
    SurfaceNodeCommandHelper::Create(context, id2);
    SurfaceNodeCommandHelper::SetGlobalPositionEnabled(context, id2, true);
}

/**
 * @tc.name: TestRSSurfaceNodeCmdUniTest006
 * @tc.desc: UpdateSurfaceDefaultSize test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCmdTest, TestRSSurfaceNodeCmdUniTest006, TestSize.Level1)
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
}

/**
 * @tc.name: SetSurfaceIdUniTest001
 * @tc.desc: SetIsNotifyUIBufferAvailable test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCmdTest, SetSurfaceIdUniTest001, TestSize.Level1)
{
    RSContext context;
    NodeId id = -2;
    bool available = false;
    SurfaceNodeCommandHelper::SetSurfaceId(context, id, available);
    NodeId id2 = 2;
    SurfaceNodeCommandHelper::Create(context, id2);
    SurfaceNodeCommandHelper::SetSurfaceId(context, id2, available);
}

/**
 * @tc.name: TestRSSurfaceNodeCmdUniTest007
 * @tc.desc: ConnectToNodeInRenderService test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCmdTest, TestRSSurfaceNodeCmdUniTest007, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(-1);
    SurfaceNodeCommandHelper::ConnectToNodeInRenderService(context, id);
    NodeId id2 = 10;
    auto context2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*context2, id2);
    SurfaceNodeCommandHelper::ConnectToNodeInRenderService(*context2, id2);
}

/**
 * @tc.name: AttachToDisplayUniTest001
 * @tc.desc: AttachToDisplay test.
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSSurfaceNodeCmdTest, AttachToDisplayUniTest001, TestSize.Level1)
{
    RSContext context;
    SurfaceNodeCommandHelper::AttachToDisplay(context, -1, 0);

    std::shared_ptr<RSSurfaceRenderNode> renderNode = std::make_shared<RSSurfaceRenderNode>(0);
    EXPECT_NE(renderNode, nullptr);
    context.nodeMap.renderNodeMap_.[0][0] = renderNode;
    std::shared_ptr<RSDisplayRenderNode> displayNodeTest1 = nullptr;
    context.nodeMap.displayNodeMap_.emplace(0, displayNodeTest1);
    SurfaceNodeCommandHelper::AttachToDisplay(context, 0, 1);

    RSDisplayNodeConfig config;
    std::shared_ptr<RSDisplayRenderNode> displayNodeTest2 = std::make_shared<RSDisplayRenderNode>(0, config);
    displayNodeTest2->screenId_ = 0;
    context.nodeMap.displayNodeMap_.at(0) = displayNodeTest2;
    EXPECT_NE(displayNodeTest2, nullptr);
    SurfaceNodeCommandHelper::AttachToDisplay(context, 0, 1);
    SurfaceNodeCommandHelper::AttachToDisplay(context, 0, 0);
}

/**
 * @tc.name: TestRSSurfaceNodeCmdUniTest008
 * @tc.desc: SetCallbackForRenderThreadRefresh test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCmdTest, TestRSSurfaceNodeCmdUniTest008, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(-1);
    SurfaceNodeCommandHelper::SetCallbackForRenderThreadRefresh(context, id, true);
    NodeId id2 = 10;
    auto context2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*context2, id2);
    SurfaceNodeCommandHelper::SetCallbackForRenderThreadRefresh(*context2, id2, false);
}

/**
 * @tc.name: SetContainerWindowUniTest001
 * @tc.desc: test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCmdTest, SetContainerWindow001, TestSize.Level1)
{
    RSContext context;
    NodeId id = -10;
    bool hasContainerWindow = false;
    SurfaceNodeCommandHelper::SetContainerWindow(context, id, hasContainerWindow, rrect);
    NodeId id2 = 10;
    auto context2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*context2, id2);
    SurfaceNodeCommandHelper::SetContainerWindow(*context2, id2, hasContainerWindow, rrect);
}

/**
 * @tc.name: TestRSSurfaceNodeCmdUniTest009
 * @tc.desc: SetContextBounds test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCmdTest, TestRSSurfaceNodeCmdUniTest009, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(-1);
    Vector4f bounds;
    SurfaceNodeCommandHelper::SetContextBounds(context, id, bounds);
    NodeId id2 = 10;
    auto context2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*context2, id2);
    SurfaceNodeCommandHelper::SetContextBounds(*context2, id2, bounds);
}

/**
 * @tc.name: TestRSSurfaceNodeCmdUniTest010
 * @tc.desc: SetAbilityBGAlpha test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCmdTest, TestRSSurfaceNodeCmdUniTest010, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(-1);
    uint8_t alpha = static_cast<uint8_t>(0);
    SurfaceNodeCommandHelper::SetAbilityBGAlpha(context, id, alpha);
    NodeId id2 = 10;
    auto context2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*context2, id2);
    SurfaceNodeCommandHelper::SetAbilityBGAlpha(*context2, id2, alpha);
}

/**
 * @tc.name: TestRSSurfaceNodeCmdUniTest012
 * @tc.desc: SetIsNotifyUIBufferAvailable test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCmdTest, TestRSSurfaceNodeCmdUniTest012, TestSize.Level1)
{
    RSContext context;
    NodeId id = -10;
    bool available = false;
    SurfaceNodeCommandHelper::SetIsNotifyUIBufferAvailable(context, id, available);
    NodeId id2 = 10;
    auto context2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*context2, id2);
    SurfaceNodeCommandHelper::SetIsNotifyUIBufferAvailable(*context2, id2, available);
}

/**
 * @tc.name: TestRSSurfaceNodeCmdUniTest013
 * @tc.desc: SetSurfaceNodeType test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCmdTest, TestRSSurfaceNodeCmdUniTest013, TestSize.Level1)
{
    RSContext context;
    NodeId id = -10;
    RSSurfaceNodeType type = RSSurfaceNodeType::DEFAULT;
    SurfaceNodeCommandHelper::SetSurfaceNodeType(context, id, static_cast<uint8_t>(type));
    NodeId id2 = 10;
    auto context2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*context2, id2);
    SurfaceNodeCommandHelper::SetSurfaceNodeType(*context2, id2, static_cast<uint8_t>(type));
    type = RSSurfaceNodeType::ABILITY_COMPONENT_NODE;
    SurfaceNodeCommandHelper::SetSurfaceNodeType(*context2, id2, static_cast<uint8_t>(type));
}

/**
 * @tc.name: TestRSSurfaceNodeCmdUniTest015
 * @tc.desc: SetContextAlpha test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCmdTest, TestRSSurfaceNodeCmdUniTest015, TestSize.Level1)
{
    RSContext context;
    NodeId id = -10;
    float alpha = 0;
    SurfaceNodeCommandHelper::SetContextAlpha(context, id, alpha);
    NodeId id2 = 10;
    auto context2 = std::make_shared<RSContext>();
    SurfaceNodeCommandHelper::Create(*context2, id2);
    SurfaceNodeCommandHelper::SetContextAlpha(*context2, id2, alpha);
}

/**
 * @tc.name: FingerprintUniTest
 * @tc.desc: SetFingerprint and GetFingerprint
 * @tc.type: FUNC
 * @tc.require: issueI6Z3YK
 */
HWTEST_F(RSSurfaceNodeCmdTest, FingerprintUniTest001, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(-1);
    SurfaceNodeCommandHelper::SetFingerprint(context, id, true);
    SurfaceNodeCommandHelper::SetFingerprint(context, id, false);

    NodeId id2 = static_cast<NodeId>(1);
    SurfaceNodeCommandHelper::Create(context, id2);
    SurfaceNodeCommandHelper::SetFingerprint(context, id2, true);
}

/**
 * @tc.name: SetSkipLayerUniTest001
 * @tc.desc: SetSkipLayer test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCmdTest, SetSkipLayerUniTest001, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(-1);
    bool isSkipLayer = false;
    SurfaceNodeCommandHelper::SetSkipLayer(context, id, isSkipLayer);
    NodeId id2 = 10;
    SurfaceNodeCommandHelper::Create(context, id2);
    SurfaceNodeCommandHelper::SetSkipLayer(context, id2, isSkipLayer);
}

/**
 * @tc.name: SetSnapshotSkipLayerUniTest001
 * @tc.desc: SetSnapshotSkipLayer test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCmdTest, SetSnapshotSkipLayerUniTest001, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(-1);
    bool isSnapshotSkipLayer = false;
    SurfaceNodeCommandHelper::SetSnapshotSkipLayer(context, id, isSnapshotSkipLayer);
    NodeId id2 = 10;
    SurfaceNodeCommandHelper::Create(context, id2);
    SurfaceNodeCommandHelper::SetSnapshotSkipLayer(context, id2, isSnapshotSkipLayer);
}

/**
 * @tc.name: SetBootAnimationUniTest001
 * @tc.desc: SetBootAnimation test.
 * @tc.type: FUNC
 * @tc.require:SR000HSUII
 */
HWTEST_F(RSSurfaceNodeCmdTest, SetBootAnimationUniTest001, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(-1);
    SurfaceNodeCommandHelper::SetBootAnimation(context, id, false);
    NodeId id2 = 10;
    SurfaceNodeCommandHelper::Create(context, id2);
    SurfaceNodeCommandHelper::SetBootAnimation(context, id2, true);
}

/**
 * @tc.name: DetachToDisplayUniTest001
 * @tc.desc: DetachToDisplay test.
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSSurfaceNodeCmdTest, DetachToDisplayUniTest001, TestSize.Level1)
{
    RSContext context;
    SurfaceNodeCommandHelper::DetachToDisplay(context, -1, 0);
    std::shared_ptr<RSSurfaceRenderNode> renderNode = std::make_shared<RSSurfaceRenderNode>(0);
    EXPECT_NE(renderNode, nullptr);
    context.nodeMap.renderNodeMap_.[0][0] = renderNode;
    std::shared_ptr<RSDisplayRenderNode> displayNodeTest1 = nullptr;
    context.nodeMap.displayNodeMap_.emplace(0, displayNodeTest1);
    SurfaceNodeCommandHelper::DetachToDisplay(context, 0, 1);

    RSDisplayNodeConfig config;
    std::shared_ptr<RSDisplayRenderNode> displayNodeTest2 = std::make_shared<RSDisplayRenderNode>(0, config);
    displayNodeTest2->screenId_ = 0;
    context.nodeMap.displayNodeMap_.at(0) = displayNodeTest2;
    EXPECT_NE(displayNodeTest2, nullptr);
    SurfaceNodeCommandHelper::DetachToDisplay(context, 0, 1);
    SurfaceNodeCommandHelper::DetachToDisplay(context, 0, 0);
}

/**
 * @tc.name: MarkUIHiddenUniTest001
 * @tc.desc: SetIsNotifyUIBufferAvailable test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCmdTest, MarkUIHiddenUniTest001, TestSize.Level1)
{
    RSContext context;
    NodeId id = -10;
    bool available = false;
    SurfaceNodeCommandHelper::MarkUIHidden(context, id, available);
    NodeId id2 = 10;
    SurfaceNodeCommandHelper::Create(context, id2);
    SurfaceNodeCommandHelper::MarkUIHidden(context, id2, available);
}

/**
 * @tc.name: SetIsTextureExportNodeUniTest001
 * @tc.desc: DetachToDisplay test.
 * @tc.type: FUNC
 * @tc.require:SR000HSUII
 */
HWTEST_F(RSSurfaceNodeCmdTest, SetIsTextureExportNodeUniTest001, TestSize.Level1)
{
    NodeId id = 10;
    RSContext context;
    SurfaceNodeCommandHelper::Create(context, id);
    SurfaceNodeCommandHelper::SetIsTextureExportNode(context, id, true);
    SurfaceNodeCommandHelper::SetIsTextureExportNode(context, 0, true);
}

/**
 * @tc.name: SetAnimationFinishedUniTest001
 * @tc.desc: SetIsNotifyUIBufferAvailable test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCmdTest, SetAnimationFinishedUniTest001, TestSize.Level1)
{
    RSContext context;
    NodeId id = -10;
    SurfaceNodeCommandHelper::SetAnimationFinished(context, id);
    NodeId id2 = 10;
    SurfaceNodeCommandHelper::Create(context, id2);
    SurfaceNodeCommandHelper::SetAnimationFinished(context, id2);
}

/**
 * @tc.name: SetForegroundUniTest001
 * @tc.desc: SetIsNotifyUIBufferAvailable test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCmdTest, SetForegroundUniTest001, TestSize.Level1)
{
    RSContext context;
    NodeId id = -2;
    bool available = false;
    SurfaceNodeCommandHelper::SetForeground(context, id, available);
    NodeId id2 = 2;
    SurfaceNodeCommandHelper::Create(context, id2);
    SurfaceNodeCommandHelper::SetForeground(context, id2, available);
}

/**
 * @tc.name: SetForceHardwareAndFixRotationUniTest
 * @tc.desc: Verify function SetForceHardwareAndFixRotation
 * @tc.type:FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSSurfaceNodeCmdTest, SetForceHardwareAndFixRotationUniTest, TestSize.Level1)
{
    RSContext context;

    SurfaceNodeCommandHelper::SetForceHardwareAndFixRotation(context, 0, false);
    SurfaceNodeCommandHelper::Create(context, 1);
    SurfaceNodeCommandHelper::SetForceHardwareAndFixRotation(context, 1, false);
    EXPECT_TRUE(context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(1) != nullptr);
}

/**
 * @tc.name: CreateWithConfigUniTest
 * @tc.desc: Verify function CreateWithConfig
 * @tc.type:FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSSurfaceNodeCmdTest, CreateWithConfigUniTest, TestSize.Level1)
{
    RSContext context;
    std::string name = "name";             // for test
    enum SurfaceWindowType windowType = SurfaceWindowType::DEFAULT_WINDOW;
    SurfaceNodeCommandHelper::CreateWithConfig(context, 1, name, 1, windowType);
    EXPECT_TRUE([&context]() -> bool {
        auto& renderNodemap = context.GetMutableNodeMap().renderNodeMap_;
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
 * @tc.name: SetAbilityStateUniTest
 * @tc.desc: Verify function SetAbilityState
 * @tc.type: FUNC
 * @tc.require: issueIAQL48
 */
HWTEST_F(RSSurfaceNodeCmdTest, SetAbilityStateUniTest, TestSize.Level1)
{
    RSContext context;
    SurfaceNodeCommandHelper::SetAbilityState(context, 0, RSSurfaceNodeAbilityState::FOREGROUND);
    SurfaceNodeCommandHelper::Create(context, 1);
    SurfaceNodeCommandHelper::SetAbilityState(context, 1, RSSurfaceNodeAbilityState::FOREGROUND);
    EXPECT_TRUE(context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(1) != nullptr);
}

/**
 * @tc.name: SetHDRPresentUniTest
 * @tc.desc: Verify function SetHDRPresent
 * @tc.type:FUNC
 * @tc.require: issueI9SBEZ
 */
HWTEST_F(RSSurfaceNodeCmdTest, SetHDRPresentUniTest, TestSize.Level1)
{
    RSContext context;
    SurfaceNodeCommandHelper::SetHDRPresent(context, 0, false);
    SurfaceNodeCommandHelper::Create(context, 1);
    SurfaceNodeCommandHelper::SetHDRPresent(context, 1, false);
    EXPECT_TRUE(context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(1) != nullptr);
}

/**
 * @tc.name: SetForceUIFirstUniTest
 * @tc.desc: Verify function SetForceUIFirst
 * @tc.type:FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSSurfaceNodeCmdTest, SetForceUIFirstUniTest, TestSize.Level1)
{
    RSContext context;
    SurfaceNodeCommandHelper::SetForceUIFirst(context, 0, false);
    SurfaceNodeCommandHelper::Create(context, 1);
    SurfaceNodeCommandHelper::SetForceUIFirst(context, 1, false);
    EXPECT_TRUE(context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(1) != nullptr);
}

/**
 * @tc.name: SetSkipDrawUniTest
 * @tc.desc: Verify function SetSkipDrawUniTest
 * @tc.type: FUNC
 * @tc.require: issueI9U6LX
 */
HWTEST_F(RSSurfaceNodeCmdTest, SetSkipDrawUniTest, TestSize.Level1)
{
    RSContext context;
    SurfaceNodeCommandHelper::SetSkipDraw(context, 0, true);
    SurfaceNodeCommandHelper::Create(context, 1);
    SurfaceNodeCommandHelper::SetSkipDraw(context, 1, true);
    EXPECT_TRUE(context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(1) != nullptr);
}

/**
 * @tc.name: SetAncoFlagsUniTest
 * @tc.desc: Verify function SetAncoFlags
 * @tc.type:FUNC
 * @tc.require: issueIAIIEP
 */
HWTEST_F(RSSurfaceNodeCmdTest, SetAncoFlagsUniTest, TestSize.Level1)
{
    RSContext context;
    SurfaceNodeCommandHelper::SetAncoFlags(context, 0, 0);
    SurfaceNodeCommandHelper::Create(context, 1);
    SurfaceNodeCommandHelper::SetAncoFlags(context, 1, 1);
    EXPECT_TRUE(context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(1) != nullptr);
}

/**
 * @tc.name: SetForceHardwareAndFixRotationUniTest
 * @tc.desc: Verify function SetForceHardwareAndFixRotation
 * @tc.type:FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSSurfaceNodeCmdTest, SetForceHardwareAndFixRotationUniTest, TestSize.Level1)
{
    RSContext context;

    SurfaceNodeCommandHelper::SetForceHardwareAndFixRotation(context, 0, false);
    SurfaceNodeCommandHelper::Create(context, 1);
    SurfaceNodeCommandHelper::SetForceHardwareAndFixRotation(context, 1, false);
    EXPECT_TRUE(context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(1) != nullptr);
}
} // namespace OHOS::Rosen