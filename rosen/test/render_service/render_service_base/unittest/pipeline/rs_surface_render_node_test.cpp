/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>

#include <if_system_ability_manager.h>
#include <iremote_stub.h>
#include <iservice_registry.h>
#include <mutex>
#include <system_ability_definition.h>
#include <unistd.h>

#include "display_engine/rs_luminance_control.h"
#include "ipc_callbacks/buffer_clear_callback_proxy.h"
#include "gmock/gmock.h"
#include "pipeline/rs_context.h"
#include "params/rs_surface_render_params.h"
#include "render_thread/rs_render_thread_visitor.h"
#include "pipeline/rs_effect_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_root_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSSurfaceRenderNodeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline NodeId id;
    static inline std::weak_ptr<RSContext> context = {};
    static inline RSPaintFilterCanvas* canvas_;
    static inline Drawing::Canvas drawingCanvas_;
    uint8_t MAX_ALPHA = 255;
    static constexpr float outerRadius = 30.4f;
    RRect rrect = RRect({0, 0, 0, 0}, outerRadius, outerRadius);
    RectI defaultLargeRect = {0, 0, 100, 100};
    RectI defaultSmallRect = {0, 0, 20, 20};
};

void RSSurfaceRenderNodeTest::SetUpTestCase()
{
    canvas_ = new RSPaintFilterCanvas(&drawingCanvas_);
}
void RSSurfaceRenderNodeTest::TearDownTestCase()
{
    delete canvas_;
    canvas_ = nullptr;
}
void RSSurfaceRenderNodeTest::SetUp() {}
void RSSurfaceRenderNodeTest::TearDown() {}

class TestDrawableAdapter : public DrawableV2::RSRenderNodeDrawableAdapter {
public:
    explicit TestDrawableAdapter(std::shared_ptr<const RSRenderNode> node)
        : RSRenderNodeDrawableAdapter(std::move(node))
    {}
    void Draw(Drawing::Canvas& canvas) {}
};

class MockRSSurfaceRenderNode : public RSSurfaceRenderNode {
public:
    explicit MockRSSurfaceRenderNode(NodeId id,
        const std::weak_ptr<RSContext>& context = {}, bool isTextureExportNode = false)
        : RSSurfaceRenderNode(id, context, isTextureExportNode) {}
    ~MockRSSurfaceRenderNode() override {}
    MOCK_CONST_METHOD0(NeedDrawBehindWindow, bool());
    MOCK_CONST_METHOD0(GetFilterRect, RectI());
};

/**
 * @tc.name: RSSurfaceRenderNodeCreate001
 * @tc.desc: Test RSSurfaceRenderNode
 * @tc.type: FUNC
 * @tc.require: issueIAI1VN
 */
HWTEST_F(RSSurfaceRenderNodeTest, RSSurfaceRenderNodeCreate001, TestSize.Level1)
{
    NodeId id = 1;
    RSSurfaceNodeType type = RSSurfaceNodeType::ABILITY_MAGNIFICATION_NODE;
    RSSurfaceRenderNodeConfig config = { .id = id, .nodeType = type };
    auto node = std::make_shared<RSSurfaceRenderNode>(config);
    EXPECT_NE(node, nullptr);
    if (node != nullptr) {
        auto& properties = node->GetMutableRenderProperties();
        EXPECT_EQ(properties.localMagnificationCap_, true);
        EXPECT_EQ(properties.filterNeedUpdate_, true);
    }
}

/**
 * @tc.name: SetContextMatrix001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeTest, SetContextMatrix001, TestSize.Level1)
{
    RSSurfaceRenderNode surfaceRenderNode(id, context);
    Drawing::Matrix matrix;
    bool sendMsg = false;
    surfaceRenderNode.SetContextMatrix(matrix, sendMsg);
    ASSERT_EQ(surfaceRenderNode.GetId(), 0);
}

/**
 * @tc.name: SetContextClipRegion001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeTest, SetContextClipRegion001, TestSize.Level1)
{
    RSSurfaceRenderNode surfaceRenderNode(id, context);
    Drawing::Rect clipRegion { 0, 0, 0, 0 };
    bool sendMsg = false;
    surfaceRenderNode.SetContextClipRegion(clipRegion, sendMsg);
    ASSERT_EQ(surfaceRenderNode.GetId(), 0);
}

/**
 * @tc.name: ConnectToNodeInRenderService001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeTest, ConnectToNodeInRenderService001, TestSize.Level1)
{
    RSSurfaceRenderNode surfaceRenderNode(id, context);
    surfaceRenderNode.ConnectToNodeInRenderService();
    ASSERT_EQ(surfaceRenderNode.GetId(), 0);
}

/**
 * @tc.name: SetSurfaceNodeType001
 * @tc.desc: Test SetSurfaceNodeType
 * @tc.type: FUNC
 * @tc.require: issueIAI1VN
 */
HWTEST_F(RSSurfaceRenderNodeTest, SetSurfaceNodeType001, TestSize.Level1)
{
    NodeId id = 1;
    RSSurfaceNodeType type = RSSurfaceNodeType::DEFAULT;
    RSSurfaceRenderNodeConfig config = { .id = id, .nodeType = type };
    auto node = std::make_shared<RSSurfaceRenderNode>(config);
    EXPECT_NE(node, nullptr);
    node->SetSurfaceNodeType(RSSurfaceNodeType::ABILITY_COMPONENT_NODE);
    bool isSameType = (node->GetSurfaceNodeType() == RSSurfaceNodeType::ABILITY_COMPONENT_NODE);
    EXPECT_TRUE(isSameType);
    node->SetSurfaceNodeType(RSSurfaceNodeType::DEFAULT);
    isSameType = (node->GetSurfaceNodeType() == RSSurfaceNodeType::DEFAULT);
    EXPECT_FALSE(isSameType);
}

/**
 * @tc.name: SetSurfaceNodeType002
 * @tc.desc: Test SetSurfaceNodeType
 * @tc.type: FUNC
 * @tc.require: issueIAN19G
 */
HWTEST_F(RSSurfaceRenderNodeTest, SetSurfaceNodeType002, TestSize.Level1)
{
    NodeId id = 1;
    RSSurfaceNodeType type = RSSurfaceNodeType::DEFAULT;
    RSSurfaceRenderNodeConfig config = { .id = id, .nodeType = type };
    auto node = std::make_shared<RSSurfaceRenderNode>(config);
    EXPECT_NE(node, nullptr);
    node->SetSurfaceNodeType(RSSurfaceNodeType::UI_EXTENSION_COMMON_NODE);
    bool isSameType = (node->GetSurfaceNodeType() == RSSurfaceNodeType::DEFAULT);
    EXPECT_TRUE(isSameType);
    node->SetSurfaceNodeType(RSSurfaceNodeType::UI_EXTENSION_SECURE_NODE);
    isSameType = (node->GetSurfaceNodeType() == RSSurfaceNodeType::DEFAULT);
    EXPECT_TRUE(isSameType);
}

/**
 * @tc.name: SetSurfaceNodeType003
 * @tc.desc: Test SetSurfaceNodeType
 * @tc.type: FUNC
 * @tc.require: issueIAN19G
 */
HWTEST_F(RSSurfaceRenderNodeTest, SetSurfaceNodeType003, TestSize.Level1)
{
    NodeId id = 1;
    RSSurfaceNodeType type = RSSurfaceNodeType::UI_EXTENSION_COMMON_NODE;
    RSSurfaceRenderNodeConfig config = { .id = id, .nodeType = type };
    auto node = std::make_shared<RSSurfaceRenderNode>(config);
    EXPECT_NE(node, nullptr);
    node->SetSurfaceNodeType(RSSurfaceNodeType::DEFAULT);
    bool isSameType = (node->GetSurfaceNodeType() == RSSurfaceNodeType::UI_EXTENSION_COMMON_NODE);
    EXPECT_TRUE(isSameType);
}

/**
 * @tc.name: SetSurfaceNodeType004
 * @tc.desc: Test SetSurfaceNodeType
 * @tc.type: FUNC
 * @tc.require: issueIAN19G
 */
HWTEST_F(RSSurfaceRenderNodeTest, SetSurfaceNodeType004, TestSize.Level1)
{
    NodeId id = 1;
    RSSurfaceNodeType type = RSSurfaceNodeType::UI_EXTENSION_SECURE_NODE;
    RSSurfaceRenderNodeConfig config = { .id = id, .nodeType = type };
    auto node = std::make_shared<RSSurfaceRenderNode>(config);
    EXPECT_NE(node, nullptr);
    node->SetSurfaceNodeType(RSSurfaceNodeType::DEFAULT);
    bool isSameType = (node->GetSurfaceNodeType() == RSSurfaceNodeType::UI_EXTENSION_SECURE_NODE);
    EXPECT_TRUE(isSameType);
}

/**
 * @tc.name: ClearChildrenCache001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeTest, ClearChildrenCache001, TestSize.Level1)
{
    RSSurfaceRenderNode surfaceRenderNode(id, context);
    surfaceRenderNode.ResetParent();
    ASSERT_EQ(surfaceRenderNode.GetId(), 0);
}

/**
 * @tc.name: ResetSurfaceOpaqueRegion02
 * @tc.desc: function test
 * @tc.type:FUNC
 * @tc.require: I6HF6Y
 */
HWTEST_F(RSSurfaceRenderNodeTest, ResetSurfaceOpaqueRegion02, TestSize.Level1)
{
    RSSurfaceRenderNode surfaceRenderNode(id, context);
    ASSERT_EQ(surfaceRenderNode.GetId(), 0);
    RectI screenRect {0, 0, 2560, 1600};
    RectI absRect {0, 100, 400, 500};
    surfaceRenderNode.SetAbilityBGAlpha(0);
    Vector4f cornerRadius;
    Vector4f::Max(
        surfaceRenderNode.GetWindowCornerRadius(), surfaceRenderNode.GetGlobalCornerRadius(), cornerRadius);
    Vector4<int> dstCornerRadius(static_cast<int>(std::ceil(cornerRadius.x_)),
                                 static_cast<int>(std::ceil(cornerRadius.y_)),
                                 static_cast<int>(std::ceil(cornerRadius.z_)),
                                 static_cast<int>(std::ceil(cornerRadius.w_)));
    surfaceRenderNode.ResetSurfaceOpaqueRegion(
        screenRect, absRect, ScreenRotation::ROTATION_0, false, dstCornerRadius);
    surfaceRenderNode.ResetSurfaceOpaqueRegion(
        screenRect, absRect, ScreenRotation::ROTATION_0, true, dstCornerRadius);
}

/**
 * @tc.name: ResetSurfaceOpaqueRegion03
 * @tc.desc: function test
 * @tc.type:FUNC
 * @tc.require: I6HF6Y
 */
HWTEST_F(RSSurfaceRenderNodeTest, ResetSurfaceOpaqueRegion03, TestSize.Level1)
{
    RSSurfaceRenderNode surfaceRenderNode(id, context);
    ASSERT_EQ(surfaceRenderNode.GetId(), 0);
    RectI screenRect {0, 0, 2560, 1600};
    RectI absRect {0, 100, 400, 500};
    surfaceRenderNode.SetAbilityBGAlpha(255);
    surfaceRenderNode.SetGlobalAlpha(1.0f);
    surfaceRenderNode.SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    surfaceRenderNode.SetContainerWindow(true, rrect);
    Vector4f cornerRadius;
    Vector4f::Max(
        surfaceRenderNode.GetWindowCornerRadius(), surfaceRenderNode.GetGlobalCornerRadius(), cornerRadius);
    Vector4<int> dstCornerRadius(static_cast<int>(std::ceil(cornerRadius.x_)),
                                 static_cast<int>(std::ceil(cornerRadius.y_)),
                                 static_cast<int>(std::ceil(cornerRadius.z_)),
                                 static_cast<int>(std::ceil(cornerRadius.w_)));
    surfaceRenderNode.ResetSurfaceOpaqueRegion(
        screenRect, absRect, ScreenRotation::ROTATION_0, false, dstCornerRadius);
    surfaceRenderNode.ResetSurfaceOpaqueRegion(
        screenRect, absRect, ScreenRotation::ROTATION_0, true, dstCornerRadius);
}

/**
 * @tc.name: ResetSurfaceOpaqueRegion04
 * @tc.desc: function test
 * @tc.type:FUNC
 * @tc.require: I6HF6Y
 */
HWTEST_F(RSSurfaceRenderNodeTest, ResetSurfaceOpaqueRegion04, TestSize.Level1)
{
    RSSurfaceRenderNode surfaceRenderNode(id, context);
    ASSERT_EQ(surfaceRenderNode.GetId(), 0);
    RectI screenRect {0, 0, 2560, 1600};
    RectI absRect {0, 100, 400, 500};
    surfaceRenderNode.SetAbilityBGAlpha(255);
    surfaceRenderNode.SetGlobalAlpha(1.0f);
    surfaceRenderNode.GetMutableRenderProperties().SetCornerRadius(Vector4f(15.0f));
    Vector4f cornerRadius;
    Vector4f::Max(
        surfaceRenderNode.GetWindowCornerRadius(), surfaceRenderNode.GetGlobalCornerRadius(), cornerRadius);
    Vector4<int> dstCornerRadius(static_cast<int>(std::ceil(cornerRadius.x_)),
                                 static_cast<int>(std::ceil(cornerRadius.y_)),
                                 static_cast<int>(std::ceil(cornerRadius.z_)),
                                 static_cast<int>(std::ceil(cornerRadius.w_)));
    surfaceRenderNode.ResetSurfaceOpaqueRegion(
        screenRect, absRect, ScreenRotation::ROTATION_0, false, dstCornerRadius);
    surfaceRenderNode.ResetSurfaceOpaqueRegion(
        screenRect, absRect, ScreenRotation::ROTATION_0, true, dstCornerRadius);
}

/**
 * @tc.name: ResetSurfaceOpaqueRegion05
 * @tc.desc: function test
 * @tc.type:FUNC
 * @tc.require: I6HF6Y
 */
HWTEST_F(RSSurfaceRenderNodeTest, ResetSurfaceOpaqueRegion05, TestSize.Level1)
{
    RSSurfaceRenderNode surfaceRenderNode(id, context);
    ASSERT_EQ(surfaceRenderNode.GetId(), 0);
    RectI screenRect {0, 0, 2560, 1600};
    RectI absRect {0, 100, 400, 500};
    surfaceRenderNode.SetAbilityBGAlpha(255);
    surfaceRenderNode.SetGlobalAlpha(1.0f);
    surfaceRenderNode.SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    surfaceRenderNode.SetContainerWindow(true, rrect);
    Vector4f cornerRadius;
    Vector4f::Max(
        surfaceRenderNode.GetWindowCornerRadius(), surfaceRenderNode.GetGlobalCornerRadius(), cornerRadius);
    Vector4<int> dstCornerRadius(static_cast<int>(std::ceil(cornerRadius.x_)),
                                 static_cast<int>(std::ceil(cornerRadius.y_)),
                                 static_cast<int>(std::ceil(cornerRadius.z_)),
                                 static_cast<int>(std::ceil(cornerRadius.w_)));
    surfaceRenderNode.ResetSurfaceOpaqueRegion(
        screenRect, absRect, ScreenRotation::ROTATION_90, false, dstCornerRadius);
    surfaceRenderNode.ResetSurfaceOpaqueRegion(
        screenRect, absRect, ScreenRotation::ROTATION_90, true, dstCornerRadius);
}

/**
 * @tc.name: ResetSurfaceOpaqueRegion06
 * @tc.desc: function test
 * @tc.type:FUNC
 * @tc.require: I6HF6Y
 */
HWTEST_F(RSSurfaceRenderNodeTest, ResetSurfaceOpaqueRegion06, TestSize.Level1)
{
    RSSurfaceRenderNode surfaceRenderNode(id, context);
    ASSERT_EQ(surfaceRenderNode.GetId(), 0);
    RectI screenRect {0, 0, 2560, 1600};
    RectI absRect {0, 100, 400, 500};
    surfaceRenderNode.SetAbilityBGAlpha(255);
    surfaceRenderNode.SetGlobalAlpha(1.0f);
    surfaceRenderNode.SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    surfaceRenderNode.SetContainerWindow(true, rrect);
    Vector4f cornerRadius;
    Vector4f::Max(
        surfaceRenderNode.GetWindowCornerRadius(), surfaceRenderNode.GetGlobalCornerRadius(), cornerRadius);
    Vector4<int> dstCornerRadius(static_cast<int>(std::ceil(cornerRadius.x_)),
                                 static_cast<int>(std::ceil(cornerRadius.y_)),
                                 static_cast<int>(std::ceil(cornerRadius.z_)),
                                 static_cast<int>(std::ceil(cornerRadius.w_)));
    surfaceRenderNode.ResetSurfaceOpaqueRegion(
        screenRect, absRect, ScreenRotation::ROTATION_180, false, dstCornerRadius);
    surfaceRenderNode.ResetSurfaceOpaqueRegion(
        screenRect, absRect, ScreenRotation::ROTATION_180, true, dstCornerRadius);
}

/**
 * @tc.name: ResetSurfaceOpaqueRegion07
 * @tc.desc: function test
 * @tc.type:FUNC
 * @tc.require: I6HF6Y
 */
HWTEST_F(RSSurfaceRenderNodeTest, ResetSurfaceOpaqueRegion07, TestSize.Level1)
{
    RSSurfaceRenderNode surfaceRenderNode(id, context);
    ASSERT_EQ(surfaceRenderNode.GetId(), 0);
    RectI screenRect {0, 0, 2560, 1600};
    RectI absRect {0, 100, 400, 500};
    surfaceRenderNode.SetAbilityBGAlpha(255);
    surfaceRenderNode.SetGlobalAlpha(1.0f);
    surfaceRenderNode.SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    surfaceRenderNode.SetContainerWindow(true, rrect);
    Vector4f cornerRadius;
    Vector4f::Max(
        surfaceRenderNode.GetWindowCornerRadius(), surfaceRenderNode.GetGlobalCornerRadius(), cornerRadius);
    Vector4<int> dstCornerRadius(static_cast<int>(std::ceil(cornerRadius.x_)),
                                 static_cast<int>(std::ceil(cornerRadius.y_)),
                                 static_cast<int>(std::ceil(cornerRadius.z_)),
                                 static_cast<int>(std::ceil(cornerRadius.w_)));
    surfaceRenderNode.ResetSurfaceOpaqueRegion(
        screenRect, absRect, ScreenRotation::ROTATION_270, false, dstCornerRadius);
    surfaceRenderNode.ResetSurfaceOpaqueRegion(
        screenRect, absRect, ScreenRotation::ROTATION_270, true, dstCornerRadius);
}

/**
 * @tc.name: SetNodeCostTest
 * @tc.desc: function test
 * @tc.type:FUNC
 * @tc.require: issueI6FZHQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, SetNodeCostTest, TestSize.Level1)
{
    RSSurfaceRenderNode surfaceRenderNode(id, context);
    auto result = surfaceRenderNode.nodeCost_;
    ASSERT_EQ(0, result);
    surfaceRenderNode.SetNodeCost(6);
    result = surfaceRenderNode.nodeCost_;
    ASSERT_EQ(6, result);
}

/**
 * @tc.name: GetNodeCostTest
 * @tc.desc: function test
 * @tc.type:FUNC
 * @tc.require: issueI6FZHQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, GetNodeCostTest, TestSize.Level1)
{
    RSSurfaceRenderNode surfaceRenderNode(id, context);
    auto result = surfaceRenderNode.nodeCost_;
    ASSERT_EQ(0, result);
    surfaceRenderNode.SetNodeCost(6);
    result = surfaceRenderNode.GetNodeCost();
    ASSERT_EQ(6, result);
}

/**
 * @tc.name: Fingerprint Test
 * @tc.desc: SetFingerprint and GetFingerprint
 * @tc.type:FUNC
 * @tc.require: issueI6Z3YK
 */
HWTEST_F(RSSurfaceRenderNodeTest, FingerprintTest, TestSize.Level1)
{
    RSSurfaceRenderNode surfaceRenderNode(id, context);
    surfaceRenderNode.SetFingerprint(true);
    auto result = surfaceRenderNode.GetFingerprint();
    ASSERT_EQ(true, result);
    surfaceRenderNode.SetFingerprint(false);
    result = surfaceRenderNode.GetFingerprint();
    ASSERT_EQ(false, result);
}

/**
 * @tc.name: IsCloneNode
 * @tc.desc: function test IsCloneNode
 * @tc.type:FUNC
 * @tc.require: issueIBKU7U
 */
HWTEST_F(RSSurfaceRenderNodeTest, IsCloneNode, TestSize.Level1)
{
    RSSurfaceRenderNode surfaceRenderNode(id, context);
    surfaceRenderNode.isCloneNode_ = true;
    ASSERT_TRUE(surfaceRenderNode.IsCloneNode());
}

/**
 * @tc.name: SetClonedNodeInfo
 * @tc.desc: function test SetClonedNodeInfo
 * @tc.type:FUNC
 * @tc.require: issueIBKU7U
 */
HWTEST_F(RSSurfaceRenderNodeTest, SetClonedNodeInfo, TestSize.Level1)
{
    RSSurfaceRenderNode surfaceRenderNode(id, context);
    surfaceRenderNode.SetClonedNodeInfo(id + 1, true);
    bool result = surfaceRenderNode.clonedSourceNodeId_ == id + 1;
    ASSERT_TRUE(result);
}

/**
 * @tc.name: SetClonedNodeRenderDrawable
 * @tc.desc: function test SetClonedNodeRenderDrawable
 * @tc.type:FUNC
 * @tc.require: issueIBKU7U
 */
HWTEST_F(RSSurfaceRenderNodeTest, SetClonedNodeRenderDrawable, TestSize.Level1)
{
    RSSurfaceRenderNode surfaceRenderNode(id, context);
    surfaceRenderNode.stagingRenderParams_ = nullptr;
    DrawableV2::RSRenderNodeDrawableAdapter::WeakPtr clonedNodeRenderDrawable;
    surfaceRenderNode.SetClonedNodeRenderDrawable(clonedNodeRenderDrawable);

    ASSERT_EQ(surfaceRenderNode.stagingRenderParams_, nullptr);
}

/**
 * @tc.name: SetIsCloned
 * @tc.desc: function test SetIsCloned
 * @tc.type:FUNC
 * @tc.require: issueIBKU7U
 */
HWTEST_F(RSSurfaceRenderNodeTest, SetIsCloned, TestSize.Level1)
{
    RSSurfaceRenderNode surfaceRenderNode(id, context);
    surfaceRenderNode.stagingRenderParams_ = nullptr;
    surfaceRenderNode.SetIsCloned(true);
    ASSERT_EQ(surfaceRenderNode.stagingRenderParams_, nullptr);

    surfaceRenderNode.stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(id + 1);
    ASSERT_NE(surfaceRenderNode.stagingRenderParams_, nullptr);
    surfaceRenderNode.SetIsCloned(true);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceRenderNode.stagingRenderParams_.get());
    ASSERT_TRUE(surfaceParams->clonedSourceNode_);
}

/**
 * @tc.name: ShouldPrepareSubnodesTest
 * @tc.desc: function test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeTest, ShouldPrepareSubnodesTest, TestSize.Level1)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    node->ShouldPrepareSubnodes();
    ASSERT_TRUE(node->ShouldPrepareSubnodes());
}

/**
 * @tc.name: CollectSurfaceTest001
 * @tc.desc: function test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeTest, CollectSurfaceTest001, TestSize.Level1)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    std::shared_ptr<RSBaseRenderNode> rsBaseRenderNode;
    std::vector<RSBaseRenderNode::SharedPtr> vec;
    bool isUniRender = true;
    node->nodeType_ = RSSurfaceNodeType::STARTING_WINDOW_NODE;
    node->CollectSurface(rsBaseRenderNode, vec, isUniRender, false);
    ASSERT_FALSE(vec.empty());
}

/**
 * @tc.name: CollectSurfaceTest002
 * @tc.desc: function test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeTest, CollectSurfaceTest002, TestSize.Level1)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    std::shared_ptr<RSBaseRenderNode> rsBaseRenderNode;
    std::vector<RSBaseRenderNode::SharedPtr> vec;
    bool isUniRender = true;
    node->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    node->CollectSurface(rsBaseRenderNode, vec, isUniRender, true);
    ASSERT_FALSE(vec.empty());
}

/**
 * @tc.name: ProcessAnimatePropertyBeforeChildrenTest
 * @tc.desc: function test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeTest, ProcessAnimatePropertyBeforeChildrenTest, TestSize.Level1)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    node->ProcessAnimatePropertyBeforeChildren(*canvas_, true);
    ASSERT_EQ(node->GetId(), 0);
}

/**
 * @tc.name: ProcessAnimatePropertyAfterChildrenTest
 * @tc.desc: function test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeTest, ProcessAnimatePropertyAfterChildrenTest, TestSize.Level1)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    node->ProcessAnimatePropertyAfterChildren(*canvas_);
    ASSERT_EQ(node->GetId(), 0);
}

/**
 * @tc.name: SetContextMatrixTest
 * @tc.desc: function test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeTest, SetContextMatrixTest, TestSize.Level1)
{
    std::optional<Drawing::Matrix> matrix;
    bool sendMsg = false;
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    node->SetContextMatrix(matrix, sendMsg);
    ASSERT_EQ(node->GetId(), 0);
}

/**
 * @tc.name: RegisterBufferAvailableListenerTest
 * @tc.desc: function test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeTest, RegisterBufferAvailableListenerTest, TestSize.Level1)
{
    sptr<RSIBufferAvailableCallback> callback;
    bool isFromRenderThread = true;
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    node->RegisterBufferAvailableListener(callback, isFromRenderThread);
    ASSERT_EQ(node->GetId(), 0);
}

/**
 * @tc.name: SetBootAnimationTest
 * @tc.desc: SetBootAnimation and GetBootAnimation
 * @tc.type:FUNC
 * @tc.require:SR000HSUII
 */
HWTEST_F(RSSurfaceRenderNodeTest, SetBootAnimationTest, TestSize.Level1)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    node->SetBootAnimation(true);
    ASSERT_EQ(node->GetBootAnimation(), true);
    node->SetBootAnimation(false);
    ASSERT_FALSE(node->GetBootAnimation());
}

/**
 * @tc.name: AncestorDisplayNodeTest
 * @tc.desc: SetAncestorDisplayNode and GetAncestorDisplayNode
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeTest, AncestorDisplayNodeTest, TestSize.Level1)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    auto displayNode = std::make_shared<RSBaseRenderNode>(0, context);
    node->SetAncestorDisplayNode(displayNode);
    ASSERT_EQ(node->GetAncestorDisplayNode().lock(), displayNode);
}

/**
 * @tc.name: UpdateSurfaceCacheContentStatic
 * @tc.desc: Set dirty subNode and check if surfacenode static
 * @tc.type:FUNC
 * @tc.require:I8W7ZS
 */
HWTEST_F(RSSurfaceRenderNodeTest, UpdateSurfaceCacheContentStatic, TestSize.Level1)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    auto subNode = std::make_shared<RSRenderNode>(id + 1, context);
    ASSERT_NE(node, nullptr);
    ASSERT_NE(subNode, nullptr);
    std::unordered_map<NodeId, std::weak_ptr<RSRenderNode>> activeNodeIds;
    node->UpdateSurfaceCacheContentStatic(activeNodeIds);
    std::shared_ptr<RSRenderNode> nullNode = nullptr;
    activeNodeIds[subNode->GetId()] = nullNode;
    node->UpdateSurfaceCacheContentStatic(activeNodeIds);
    activeNodeIds[subNode->GetId()] = subNode;
    node->AddChild(subNode, 0);
    subNode->isContentDirty_ = true;
    node->UpdateSurfaceCacheContentStatic(activeNodeIds);
    ASSERT_EQ(node->GetSurfaceCacheContentStatic(), false);
    ASSERT_EQ(node->IsContentDirtyNodeLimited(), true);
}

/**
 * @tc.name: IsContentDirtyNodeLimited
 * @tc.desc: Set content dirty subnode new on the tree and check if it is in count
 * @tc.type:FUNC
 * @tc.require:I8XIJH
 */
HWTEST_F(RSSurfaceRenderNodeTest, IsContentDirtyNodeLimited, TestSize.Level1)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    auto subnode = std::make_shared<RSRenderNode>(id + 1, context);
    ASSERT_NE(node, nullptr);
    ASSERT_NE(subnode, nullptr);
    node->AddChild(subnode, 0);
    subnode->isContentDirty_ = true;
    subnode->isNewOnTree_ = true;
    std::unordered_map<NodeId, std::weak_ptr<RSRenderNode>> activeNodeIds = {{subnode->GetId(), subnode}};
    node->UpdateSurfaceCacheContentStatic(activeNodeIds);
    ASSERT_EQ(node->IsContentDirtyNodeLimited(), false);
}

/**
 * @tc.name: SetSkipLayer001
 * @tc.desc: Test SetSkipLayer for single surface node which is skip layer
 * @tc.type: FUNC
 * @tc.require: issueI9ABGS
 */
HWTEST_F(RSSurfaceRenderNodeTest, SetSkipLayer001, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto node = std::make_shared<RSSurfaceRenderNode>(id, rsContext);
    ASSERT_NE(node, nullptr);

    node->SetSkipLayer(true);
    ASSERT_TRUE(node->GetSpecialLayerMgr().Find(SpecialLayerType::SKIP));
}

/**
 * @tc.name: SetBlackListWithScreen001
 * @tc.desc: Test UpdateBlackListStatus
 * @tc.type: FUNC
 * @tc.require: issueIC9I11
 */
HWTEST_F(RSSurfaceRenderNodeTest, SetBlackListWithScreen001, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto node = std::make_shared<RSSurfaceRenderNode>(id, rsContext);
    ASSERT_NE(node, nullptr);
    node->InitRenderParams();
    node->addedToPendingSyncList_ = true;
    auto params = static_cast<RSSurfaceRenderParams*>(node->stagingRenderParams_.get());
    EXPECT_NE(params, nullptr);

    auto virtualScreenId = 1;
    node->UpdateBlackListStatus(virtualScreenId, true);
    node->UpdateBlackListStatus(virtualScreenId, false);
}

/**
 * @tc.name: SyncBlackListInfoToFirstLevelNode001
 * @tc.desc: Test SyncBlackListInfoToFirstLevelNode
 * @tc.type: FUNC
 * @tc.require: issueIC9I11
 */
HWTEST_F(RSSurfaceRenderNodeTest, SyncBlackListInfoToFirstLevelNode001, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto parentNode = std::make_shared<RSSurfaceRenderNode>(id, rsContext);
    auto childNode = std::make_shared<RSSurfaceRenderNode>(id + 1, rsContext);
    ASSERT_NE(parentNode, nullptr);
    ASSERT_NE(childNode, nullptr);

    NodeId parentNodeId = parentNode->GetId();
    pid_t parentNodePid = ExtractPid(parentNodeId);
    NodeId childNodeId = childNode->GetId();
    pid_t childNodePid = ExtractPid(childNodeId);
    rsContext->GetMutableNodeMap().renderNodeMap_[parentNodePid][parentNodeId] = parentNode;
    rsContext->GetMutableNodeMap().renderNodeMap_[childNodePid][childNodeId] = childNode;
    childNode->firstLevelNodeId_ = parentNodeId;
    parentNode->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    parentNode->AddChild(childNode);

    auto virtualScreenId = 1;
    childNode->UpdateBlackListStatus(virtualScreenId, true);
    parentNode->SetIsOnTheTree(true);
    parentNode->SyncBlackListInfoToFirstLevelNode();
    childNode->SetIsOnTheTree(true);
    childNode->SyncBlackListInfoToFirstLevelNode();
    childNode->SetIsOnTheTree(false);
    childNode->SyncBlackListInfoToFirstLevelNode();
}

/**
 * @tc.name: SetSnapshotSkipLayer001
 * @tc.desc: Test SetSnapshotSkipLayer for single surface node which is skip layer
 * @tc.type: FUNC
 * @tc.require: issueI9ABGS
 */
HWTEST_F(RSSurfaceRenderNodeTest, SetSnapshotSkipLayer001, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto node = std::make_shared<RSSurfaceRenderNode>(id, rsContext);
    ASSERT_NE(node, nullptr);

    node->SetSnapshotSkipLayer(true);
    ASSERT_TRUE(node->GetSpecialLayerMgr().Find(SpecialLayerType::SNAPSHOT_SKIP));
}

/**
 * @tc.name: SetSkipLayer002
 * @tc.desc: Test SetSkipLayer for surface node while skip Layer isn't first level node
 * @tc.type: FUNC
 * @tc.require: issueI9ABGS
 */
HWTEST_F(RSSurfaceRenderNodeTest, SetSkipLayer002, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto parentNode = std::make_shared<RSSurfaceRenderNode>(id, rsContext);
    auto skipLayerNode = std::make_shared<RSSurfaceRenderNode>(id + 1, rsContext);
    ASSERT_NE(parentNode, nullptr);
    ASSERT_NE(skipLayerNode, nullptr);

    NodeId parentNodeId = parentNode->GetId();
    pid_t parentNodePid = ExtractPid(parentNodeId);
    NodeId skipLayerNodeId = skipLayerNode->GetId();
    pid_t skipLayerNodePid = ExtractPid(skipLayerNodeId);
    rsContext->GetMutableNodeMap().renderNodeMap_[parentNodePid][parentNodePid] = parentNode;
    rsContext->GetMutableNodeMap().renderNodeMap_[skipLayerNodePid][skipLayerNodeId] = skipLayerNode;

    parentNode->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    parentNode->AddChild(skipLayerNode);
    parentNode->SetIsOnTheTree(true);
    skipLayerNode->SetSkipLayer(true);

    ASSERT_TRUE(skipLayerNode->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_SKIP));
}

/**
 * @tc.name: SetSnapshotSkipLayer002
 * @tc.desc: Test SetSnapshotSkipLayer for surface node while skip Layer isn't first level node
 * @tc.type: FUNC
 * @tc.require: issueI9ABGS
 */
HWTEST_F(RSSurfaceRenderNodeTest, SetSnapshotSkipLayer002, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto parentNode = std::make_shared<RSSurfaceRenderNode>(id, rsContext);
    auto snapshotSkipLayerNode = std::make_shared<RSSurfaceRenderNode>(id + 1, rsContext);
    ASSERT_NE(parentNode, nullptr);
    ASSERT_NE(snapshotSkipLayerNode, nullptr);

    NodeId parentNodeId = parentNode->GetId();
    pid_t parentNodePid = ExtractPid(parentNodeId);
    NodeId skipLayerNodeId = snapshotSkipLayerNode->GetId();
    pid_t skipLayerNodePid = ExtractPid(skipLayerNodeId);
    rsContext->GetMutableNodeMap().renderNodeMap_[parentNodePid][parentNodeId] = parentNode;
    rsContext->GetMutableNodeMap().renderNodeMap_[skipLayerNodePid][skipLayerNodeId] = snapshotSkipLayerNode;

    parentNode->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    parentNode->AddChild(snapshotSkipLayerNode);
    parentNode->SetIsOnTheTree(true);
    snapshotSkipLayerNode->SetSnapshotSkipLayer(true);

    ASSERT_TRUE(parentNode->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_SNAPSHOT_SKIP));
}

/**
 * @tc.name: SetSecurityLayer001
 * @tc.desc: Test SetSecurityLayer for single surface node which is security layer
 * @tc.type: FUNC
 * @tc.require: issueI9ABGS
 */
HWTEST_F(RSSurfaceRenderNodeTest, SetSecurityLayer001, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto node = std::make_shared<RSSurfaceRenderNode>(id, rsContext);
    ASSERT_NE(node, nullptr);

    node->SetSecurityLayer(true);
    ASSERT_TRUE(node->GetSpecialLayerMgr().Find(SpecialLayerType::SECURITY));
}

/**
 * @tc.name: SetSecurityLayer002
 * @tc.desc: Test SetSecurityLayer for surface node while security Layer isn't first level node
 * @tc.type: FUNC
 * @tc.require: issueI9ABGS
 */
HWTEST_F(RSSurfaceRenderNodeTest, SetSecurityLayer002, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto parentNode = std::make_shared<RSSurfaceRenderNode>(id, rsContext);
    auto securityLayerNode = std::make_shared<RSSurfaceRenderNode>(id + 1, rsContext);
    ASSERT_NE(parentNode, nullptr);
    ASSERT_NE(securityLayerNode, nullptr);

    NodeId parentNodeId = parentNode->GetId();
    pid_t parentNodePid = ExtractPid(parentNodeId);
    NodeId secLayerNodeId = securityLayerNode->GetId();
    pid_t secLayerNodePid = ExtractPid(secLayerNodeId);
    rsContext->GetMutableNodeMap().renderNodeMap_[parentNodePid][parentNodeId] = parentNode;
    rsContext->GetMutableNodeMap().renderNodeMap_[secLayerNodePid][secLayerNodeId] = securityLayerNode;

    parentNode->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    parentNode->AddChild(securityLayerNode);
    parentNode->SetIsOnTheTree(true);
    securityLayerNode->SetSecurityLayer(true);

    ASSERT_TRUE(parentNode->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_SECURITY));
}

/**
 * @tc.name: CornerRadiusInfoForDRMTest
 * @tc.desc: Test SetCornerRadiusInfoForDRM and GetCornerRadiusInfoForDRM
 * @tc.type: FUNC
 * @tc.require: issueIAX2NE
 */
HWTEST_F(RSSurfaceRenderNodeTest, CornerRadiusInfoForDRMTest, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto node = std::make_shared<RSSurfaceRenderNode>(id, rsContext);
    ASSERT_NE(node, nullptr);
    std::vector<float> cornerRadiusInfo = {};
    node->SetCornerRadiusInfoForDRM(cornerRadiusInfo);
    ASSERT_TRUE(node->GetCornerRadiusInfoForDRM().empty());
}

/**
 * @tc.name: GetFirstLevelNodeId001
 * @tc.desc: Test GetFirstLevelNode for single app window node
 * @tc.type: FUNC
 * @tc.require: issueI9ABGS
 */
HWTEST_F(RSSurfaceRenderNodeTest, GetFirstLevelNodeId001, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto node = std::make_shared<RSSurfaceRenderNode>(id, rsContext);
    ASSERT_NE(node, nullptr);
    NodeId nodeId = node->GetId();
    pid_t pid = ExtractPid(nodeId);
    rsContext->GetMutableNodeMap().renderNodeMap_[pid][nodeId] = node;
    node->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    node->SetIsOnTheTree(true);
    ASSERT_EQ(node->GetFirstLevelNodeId(), node->GetId());
}

/**
 * @tc.name: GetFirstLevelNodeId002
 * @tc.desc: Test GetFirstLevelNode for app window node which parent is leash window node
 * @tc.type: FUNC
 * @tc.require: issueI9ABGS
 */
HWTEST_F(RSSurfaceRenderNodeTest, GetFirstLevelNodeId002, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto childNode = std::make_shared<RSSurfaceRenderNode>(id, rsContext);
    auto parentNode = std::make_shared<RSSurfaceRenderNode>(id + 1, rsContext);
    ASSERT_NE(childNode, nullptr);
    ASSERT_NE(parentNode, nullptr);

    NodeId childNodeId = childNode->GetId();
    pid_t childNodePid = ExtractPid(childNodeId);
    NodeId parentNodeId = parentNode->GetId();
    pid_t parentNodePid = ExtractPid(parentNodeId);
    rsContext->GetMutableNodeMap().renderNodeMap_[childNodePid][childNodeId] = childNode;
    rsContext->GetMutableNodeMap().renderNodeMap_[parentNodePid][parentNodeId] = parentNode;

    parentNode->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    childNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    parentNode->AddChild(childNode);
    parentNode->SetIsOnTheTree(true);
    ASSERT_EQ(childNode->GetFirstLevelNodeId(), parentNode->GetId());
}

/**
 * @tc.name: SetHasSharedTransitionNode
 * @tc.desc: Test SetHasSharedTransitionNode
 * @tc.type: FUNC
 * @tc.require: issueI98VTC
 */
HWTEST_F(RSSurfaceRenderNodeTest, SetHasSharedTransitionNode, TestSize.Level2)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    ASSERT_NE(node, nullptr);
    node->SetHasSharedTransitionNode(true);
    
    ASSERT_EQ(node->GetHasSharedTransitionNode(), true);
}

/**
 * @tc.name: QuerySubAssignable001
 * @tc.desc: Test QuerySubAssignable
 * @tc.type: FUNC
 * @tc.require: issueI98VTC
 */
HWTEST_F(RSSurfaceRenderNodeTest, QuerySubAssignable001, TestSize.Level2)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    ASSERT_NE(node, nullptr);
    
    ASSERT_EQ(node->QuerySubAssignable(false), true);
}

/**
 * @tc.name: QuerySubAssignable002
 * @tc.desc: Test QuerySubAssignable while has filter
 * @tc.type: FUNC
 * @tc.require: issueI9LOXQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, QuerySubAssignable002, TestSize.Level2)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    ASSERT_NE(node, nullptr);
    
    if (RSUniRenderJudgement::IsUniRender()) {
        node->InitRenderParams();
    }
    node->SetHasFilter(true);
    
    ASSERT_EQ(node->QuerySubAssignable(false), false);
}

/**
 * @tc.name: QuerySubAssignable003
 * @tc.desc: Test QuerySubAssignable while node's child has filter and child is transparent
 * @tc.type: FUNC
 * @tc.require: issueI9LOXQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, QuerySubAssignable003, TestSize.Level2)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    auto childNode = std::make_shared<RSSurfaceRenderNode>(id + 1, context);
    ASSERT_NE(node, nullptr);
    ASSERT_NE(childNode, nullptr);
    RSUniRenderJudgement::uniRenderEnabledType_ = UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL;
    if (RSUniRenderJudgement::IsUniRender()) {
        node->InitRenderParams();
        childNode->InitRenderParams();
    }
    childNode->SetHasFilter(true);
    node->SetChildHasVisibleFilter(true);
    
    ASSERT_EQ(node->QuerySubAssignable(false), false);
}

/**
 * @tc.name: QuerySubAssignable004
 * @tc.desc: Test QuerySubAssignable while node's child has filter and is not transparent
 * @tc.type: FUNC
 * @tc.require: issueI9LOXQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, QuerySubAssignable004, TestSize.Level2)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    auto childNode = std::make_shared<RSSurfaceRenderNode>(id + 1, context);
    ASSERT_NE(node, nullptr);
    ASSERT_NE(childNode, nullptr);

    RSUniRenderJudgement::uniRenderEnabledType_ = UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL;
    if (RSUniRenderJudgement::IsUniRender()) {
        node->InitRenderParams();
        childNode->InitRenderParams();
    }
    childNode->SetHasFilter(true);
    node->SetChildHasVisibleFilter(true);
    node->SetAbilityBGAlpha(MAX_ALPHA);
    
    ASSERT_EQ(node->QuerySubAssignable(false), true);
}

/**
 * @tc.name: SetForceHardwareAndFixRotation001
 * @tc.desc: Test SetForceHardwareAndFixRotation true
 * @tc.type: FUNC
 * @tc.require: issueI9HWLB
 */
HWTEST_F(RSSurfaceRenderNodeTest, SetForceHardwareAndFixRotation001, TestSize.Level2)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    ASSERT_NE(node, nullptr);

    node->InitRenderParams();
    node->SetForceHardwareAndFixRotation(true);
    ASSERT_EQ(node->isFixRotationByUser_, true);
}

/**
 * @tc.name: SetForceHardwareAndFixRotation002
 * @tc.desc: Test SetForceHardwareAndFixRotation false
 * @tc.type: FUNC
 * @tc.require: issueI9HWLB
 */
HWTEST_F(RSSurfaceRenderNodeTest, SetForceHardwareAndFixRotation002, TestSize.Level2)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    ASSERT_NE(node, nullptr);

    node->InitRenderParams();
    node->SetForceHardwareAndFixRotation(false);
    ASSERT_EQ(node->isFixRotationByUser_, false);
}

/**
 * @tc.name: UpdateSrcRectTest
 * @tc.desc: test results of UpdateSrcRect
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, UpdateSrcRectTest, TestSize.Level1)
{
    Drawing::Canvas canvas;
    Drawing::RectI dstRect(0, 0, 100, 100);
    bool hasRotation = false;

    auto renderNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    renderNode->UpdateSrcRect(canvas, dstRect, hasRotation);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: IsYUVBufferFormatTest
 * @tc.desc: test results of IsYUVBufferFormat
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, IsYUVBufferFormatTest, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    EXPECT_FALSE(testNode->IsYUVBufferFormat());

    auto buffer = SurfaceBuffer::Create();
    testNode->GetRSSurfaceHandler()->buffer_.buffer = buffer;
    EXPECT_NE(testNode->GetRSSurfaceHandler()->GetBuffer(), nullptr);
    EXPECT_FALSE(testNode->IsYUVBufferFormat());
}

/**
 * @tc.name: ShouldPrepareSubnodesTest001
 * @tc.desc: test results of ShouldPrepareSubnodes
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, ShouldPrepareSubnodesTest001, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    testNode->SetDstRect(RectI());
    testNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    EXPECT_FALSE(testNode->ShouldPrepareSubnodes());

    testNode->SetDstRect(RectI(0, 0, 100, 100));
    testNode->nodeType_ = RSSurfaceNodeType::SCB_SCREEN_NODE;
    EXPECT_TRUE(testNode->ShouldPrepareSubnodes());
}

/**
 * @tc.name: DirtyRegionDumpTest
 * @tc.desc: test results of DirtyRegionDump
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, DirtyRegionDumpTest, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id, context);
    node->DirtyRegionDump();

    node->dirtyManager_.reset();
    std::string dump = node->DirtyRegionDump();
    ASSERT_NE(dump, "");
}

/**
 * @tc.name: PrepareRenderBeforeChildren
 * @tc.desc: test results of PrepareRenderBeforeChildren
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, PrepareRenderBeforeChildren, TestSize.Level1)
{
    Drawing::Canvas canvas;
    RSPaintFilterCanvas rsPaintFilterCanvas(&canvas);
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id, context);
    node->InitRenderParams();
    node->PrepareRenderBeforeChildren(rsPaintFilterCanvas);
    ASSERT_NE(node->GetRenderProperties().GetBoundsGeometry(), nullptr);
}

/**
 * @tc.name: CollectSurfaceTest
 * @tc.desc: test results of CollectSurface
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, CollectSurfaceTest, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> testNode = std::make_shared<RSSurfaceRenderNode>(id, context);

    testNode->nodeType_ = RSSurfaceNodeType::SCB_SCREEN_NODE;
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    ASSERT_NE(node, nullptr);
    Drawing::Canvas canvasArgs;
    RSPaintFilterCanvas canvas(&canvasArgs);
    std::vector<std::shared_ptr<RSRenderNode>> vec;
    testNode->CollectSurface(node, vec, true, false);

    testNode->nodeType_ = RSSurfaceNodeType::STARTING_WINDOW_NODE;
    testNode->CollectSurface(node, vec, true, false);
    testNode->CollectSurface(node, vec, false, false);
    testNode->nodeType_ = RSSurfaceNodeType::SCB_SCREEN_NODE;
    testNode->CollectSurface(node, vec, true, false);
    testNode->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    testNode->CollectSurface(node, vec, true, true);
}

/**
 * @tc.name: ClearChildrenCache
 * @tc.desc: test results of ClearChildrenCache
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, ClearChildrenCache, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    testNode->ClearChildrenCache();
    ASSERT_EQ(testNode->nodeType_, RSSurfaceNodeType::DEFAULT);
}

/**
 * @tc.name: OnTreeStateChangedTest
 * @tc.desc: test results of OnTreeStateChanged
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, OnTreeStateChangedTest, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id, context);
    node->OnTreeStateChanged();
    node->nodeType_ = RSSurfaceNodeType::ABILITY_COMPONENT_NODE;
    node->OnTreeStateChanged();
    ASSERT_EQ(node->nodeType_, RSSurfaceNodeType::ABILITY_COMPONENT_NODE);
}

/**
 * @tc.name: OnResetParentTest
 * @tc.desc: test results of OnResetParent
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, OnResetParentTest, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id, context);

    node->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    node->OnResetParent();

    node->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    node->OnResetParent();
    ASSERT_EQ(node->nodeType_, RSSurfaceNodeType::SELF_DRAWING_NODE);
}

/**
 * @tc.name: SetIsNotifyUIBufferAvailableTest
 * @tc.desc: test results of SetIsNotifyUIBufferAvailable
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, SetIsNotifyUIBufferAvailableTest, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id, context);
    node->SetIsNotifyUIBufferAvailable(true);
    ASSERT_TRUE(node->isNotifyUIBufferAvailable_.load());
}

/**
 * @tc.name: IsSubTreeNeedPrepareTest
 * @tc.desc: test results of IsSubTreeNeedPrepare
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, IsSubTreeNeedPrepareTest, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id, context);
    node->IsSubTreeNeedPrepare(false, false);
    node->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    EXPECT_TRUE(node->IsSubTreeNeedPrepare(true, true));
}

/**
 * @tc.name: PrepareTest
 * @tc.desc: test results of QuickPrepare
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, PrepareTest, TestSize.Level1)
{
    std::shared_ptr<RSRenderThreadVisitor> visitor;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    surfaceNode->QuickPrepare(visitor);

    visitor = std::make_shared<RSRenderThreadVisitor>();
    surfaceNode->QuickPrepare(visitor);
    ASSERT_EQ(surfaceNode->nodeType_, RSSurfaceNodeType::DEFAULT);
}

/**
 * @tc.name: PrepareTest
 * @tc.desc: test results of Process
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, ProcessTest, TestSize.Level1)
{
    std::shared_ptr<RSRenderThreadVisitor> visitor;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    surfaceNode->Process(visitor);
    visitor = std::make_shared<RSRenderThreadVisitor>();
    surfaceNode->Process(visitor);
    ASSERT_EQ(surfaceNode->nodeType_, RSSurfaceNodeType::DEFAULT);
}

/**
 * @tc.name: ProcessAnimatePropertyBeforeChildren
 * @tc.desc: test results of ProcessAnimatePropertyBeforeChildren
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, ProcessAnimatePropertyBeforeChildren, TestSize.Level1)
{
    Drawing::Canvas canvasArgs;
    auto canvas = std::make_shared<RSPaintFilterCanvas>(&canvasArgs);
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    surfaceNode->InitRenderParams();
    surfaceNode->ProcessAnimatePropertyBeforeChildren(*canvas, true);
    surfaceNode->cacheType_ = CacheType::ANIMATE_PROPERTY;
    surfaceNode->needDrawAnimateProperty_ = true;
    surfaceNode->ProcessAnimatePropertyBeforeChildren(*canvas, true);
    ASSERT_EQ(surfaceNode->nodeType_, RSSurfaceNodeType::DEFAULT);
}

/**
 * @tc.name: ProcessRenderAfterChildrenTest
 * @tc.desc: test results of ProcessRenderAfterChildren
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, ProcessRenderAfterChildrenTest, TestSize.Level1)
{
    Drawing::Canvas canvasArgs;
    auto canvas = std::make_shared<RSPaintFilterCanvas>(&canvasArgs);
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    node->ProcessRenderAfterChildren(*canvas);
    EXPECT_FALSE(node->needDrawAnimateProperty_);
}

/**
 * @tc.name: SetContextAlphaTest
 * @tc.desc: test results of SetContextAlpha
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, SetContextAlphaTest, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    testNode->SetContextAlpha(1.0f, true);
    testNode->SetContextAlpha(0.5f, true);
    testNode->SetContextAlpha(0.5f, false);
    EXPECT_EQ(testNode->contextAlpha_, 0.5f);
}

/**
 * @tc.name: HdrVideoTest
 * @tc.desc: test results of SetVideoHdrStatus, GetVideoHdrStatus
 * @tc.type: FUNC
 * @tc.require: issuesIBANP9
 */
HWTEST_F(RSSurfaceRenderNodeTest, HdrVideoTest, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    testNode->SetVideoHdrStatus(HdrStatus::HDR_VIDEO);
    EXPECT_EQ(testNode->GetVideoHdrStatus(), HdrStatus::HDR_VIDEO);
    testNode->SetVideoHdrStatus(HdrStatus::NO_HDR);
    EXPECT_EQ(testNode->GetVideoHdrStatus(), HdrStatus::NO_HDR);
    testNode->SetVideoHdrStatus(HdrStatus::AI_HDR_VIDEO);
    EXPECT_EQ(testNode->GetVideoHdrStatus(), HdrStatus::AI_HDR_VIDEO);
}

/**
 * @tc.name: SetContextClipRegionTest
 * @tc.desc: test results of GetContextClipRegion
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, SetContextClipRegionTest, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    Drawing::Rect rect(0, 0, 100, 100);
    testNode->SetContextClipRegion(rect, true);
    testNode->SetContextClipRegion(rect, false);
    EXPECT_EQ(testNode->contextClipRect_->left_, rect.left_);
    testNode->SetContextClipRegion(Drawing::Rect(1, 1, 1, 1), true);
    EXPECT_NE(testNode->contextClipRect_->left_, rect.left_);
}

/**
 * @tc.name: SetSkipLayerTest
 * @tc.desc: test results of SetSkipLayer
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, SetSkipLayerTest, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id, context);
    node->SetSkipLayer(true);
    EXPECT_TRUE(node->GetSpecialLayerMgr().Find(SpecialLayerType::SKIP));
    node->SetSkipLayer(false);
    EXPECT_FALSE(node->GetSpecialLayerMgr().Find(SpecialLayerType::SKIP));
}

/**
 * @tc.name: SetSnapshotSkipLayerTest
 * @tc.desc: test results of SetSnapshotSkipLayer
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, SetSnapshotSkipLayerTest, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id, context);
    node->SetSnapshotSkipLayer(true);
    EXPECT_TRUE(node->GetSpecialLayerMgr().Find(SpecialLayerType::SNAPSHOT_SKIP));
    node->SetSnapshotSkipLayer(false);
    EXPECT_FALSE(node->GetSpecialLayerMgr().Find(SpecialLayerType::SNAPSHOT_SKIP));
}

/**
 * @tc.name: NotifyTreeStateChange
 * @tc.desc: test results of NotifyTreeStateChange
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, NotifyTreeStateChange, TestSize.Level1)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    node->NotifyTreeStateChange();
    bool callbackCalled = false;
    node->RegisterTreeStateChangeCallback(
        [&callbackCalled](const RSSurfaceRenderNode& node) { callbackCalled = true; });
    node->NotifyTreeStateChange();
    EXPECT_TRUE(callbackCalled);
}

/**
 * @tc.name: UpdateSurfaceDefaultSize
 * @tc.desc: test results of UpdateSurfaceDefaultSize
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, UpdateSurfaceDefaultSize, TestSize.Level1)
{
    auto context = std::make_shared<RSContext>();
    auto node = std::make_shared<RSSurfaceRenderNode>(1, context, true);
    node->UpdateSurfaceDefaultSize(1920.0f, 1080.0f);
    node->GetRSSurfaceHandler()->consumer_ = IConsumerSurface::Create();
    node->UpdateSurfaceDefaultSize(1920.0f, 1080.0f);
    ASSERT_NE(node->GetRSSurfaceHandler()->consumer_, nullptr);
}

/**
 * @tc.name: RegisterBufferAvailableListenerTest001
 * @tc.desc: test results of RegisterBufferAvailableListener
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, RegisterBufferAvailableListenerTest001, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    sptr<RSIBufferAvailableCallback> callback;
    bool isFromRenderThread = false;
    testNode->RegisterBufferAvailableListener(callback, isFromRenderThread);
    EXPECT_FALSE(testNode->GetSpecialLayerMgr().Find(SpecialLayerType::SKIP));
}

/**
 * @tc.name: SetNotifyRTBufferAvailable
 * @tc.desc: test results of SetNotifyRTBufferAvailable
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, SetNotifyRTBufferAvailable, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    testNode->SetNotifyRTBufferAvailable(true);
    ASSERT_TRUE(testNode->isNotifyRTBufferAvailable_);
}

/**
 * @tc.name: ConnectToNodeInRenderServiceTest
 * @tc.desc: test results of ConnectToNodeInRenderService
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, ConnectToNodeInRenderServiceTest, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    testNode->ConnectToNodeInRenderService();
    EXPECT_FALSE(testNode->GetSpecialLayerMgr().Find(SpecialLayerType::SKIP));
}

/**
 * @tc.name: NotifyRTBufferAvailable
 * @tc.desc: test results of NotifyRTBufferAvailable
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, NotifyRTBufferAvailable, TestSize.Level1)
{
    auto testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    testNode->NotifyRTBufferAvailable(false);
    ASSERT_FALSE(testNode->isNotifyRTBufferAvailablePre_);
    testNode->NotifyRTBufferAvailable(true);
    testNode->isRefresh_ = true;
    testNode->NotifyRTBufferAvailable(true);
    ASSERT_FALSE(testNode->isNotifyRTBufferAvailable_);
}

/**
 * @tc.name: NotifyRTBufferAvailable
 * @tc.desc: test results of NotifyRTBufferAvailable
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, NotifyUIBufferAvailable, TestSize.Level1)
{
    auto testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    testNode->NotifyUIBufferAvailable();
    testNode->isNotifyUIBufferAvailable_ = false;
    testNode->NotifyUIBufferAvailable();
    ASSERT_TRUE(testNode->isNotifyUIBufferAvailable_);
}

/**
 * @tc.name: UpdateDirtyIfFrameBufferConsumed
 * @tc.desc: test results of UpdateDirtyIfFrameBufferConsumed
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, UpdateDirtyIfFrameBufferConsumed, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    testNode->GetRSSurfaceHandler()->isCurrentFrameBufferConsumed_ = true;
    bool resultOne = testNode->UpdateDirtyIfFrameBufferConsumed();
    EXPECT_TRUE(resultOne);

    testNode->GetRSSurfaceHandler()->isCurrentFrameBufferConsumed_ = false;
    bool resultTwo = testNode->UpdateDirtyIfFrameBufferConsumed();
    EXPECT_FALSE(resultTwo);
}

/**
 * @tc.name: IsSurfaceInStartingWindowStage
 * @tc.desc: test results of IsSurfaceInStartingWindowStage
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, IsSurfaceInStartingWindowStage, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    auto parentSurfaceNode = std::make_shared<RSSurfaceRenderNode>(id + 1, context);
    EXPECT_FALSE(testNode->IsSurfaceInStartingWindowStage());

    testNode->SetParent(parentSurfaceNode);
    testNode->isNotifyUIBufferAvailable_ = false;
    testNode->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    bool resultOne = testNode->IsSurfaceInStartingWindowStage();
    EXPECT_FALSE(resultOne);

    parentSurfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::SURFACE_TEXTURE_NODE);
    bool resultTwo = testNode->IsSurfaceInStartingWindowStage();
    EXPECT_FALSE(resultTwo);
}

/**
 * @tc.name: IsParentLeashWindowInScale
 * @tc.desc: test results of IsParentLeashWindowInScale
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, IsParentLeashWindowInScale, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    std::shared_ptr<RSSurfaceRenderNode> parentSurfaceNode = std::make_shared<RSSurfaceRenderNode>(id + 1, context);
    testNode->SetParent(parentSurfaceNode);

    bool resultOne = testNode->IsParentLeashWindowInScale();
    EXPECT_FALSE(resultOne);

    parentSurfaceNode->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    parentSurfaceNode->isScale_ = true;
    bool resultTwo = testNode->IsParentLeashWindowInScale();
    EXPECT_TRUE(resultTwo);
}

/**
 * @tc.name: GetSurfaceOcclusionRect
 * @tc.desc: test results of GetSurfaceOcclusionRect
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, GetSurfaceOcclusionRect, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    bool isUniRender = true;
    Occlusion::Rect expectedRect1 = testNode->GetOldDirtyInSurface();
    Occlusion::Rect resultOne = testNode->GetSurfaceOcclusionRect(isUniRender);
    EXPECT_EQ(expectedRect1, resultOne);

    isUniRender = false;
    Occlusion::Rect expectedRect2 = testNode->GetDstRect();
    Occlusion::Rect resultTwo = testNode->GetSurfaceOcclusionRect(isUniRender);
    EXPECT_EQ(expectedRect2, resultTwo);
}

/**
 * @tc.name: QueryIfAllHwcChildrenForceDisabledByFilter
 * @tc.desc: test results of QueryIfAllHwcChildrenForceDisabledByFilter
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, QueryIfAllHwcChildrenForceDisabledByFilter, TestSize.Level1)
{
    auto testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    EXPECT_TRUE(testNode->QueryIfAllHwcChildrenForceDisabledByFilter());

    auto childNode1 = std::make_shared<RSSurfaceRenderNode>(id + 1, context);
    auto childNode2 = std::make_shared<RSSurfaceRenderNode>(id + 2, context);
    testNode->AddChildHardwareEnabledNode(childNode1);
    testNode->AddChildHardwareEnabledNode(childNode2);
    childNode1->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    EXPECT_TRUE(testNode->QueryIfAllHwcChildrenForceDisabledByFilter());
}

/**
 * @tc.name: AccumulateOcclusionRegion
 * @tc.desc: test results of AccumulateOcclusionRegion
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, AccumulateOcclusionRegion, TestSize.Level1)
{
    auto testNode = std::make_shared<RSSurfaceRenderNode>(id, context);

    Occlusion::Region accumulatedRegion;
    Occlusion::Region curRegion;
    bool hasFilterCacheOcclusion = false;
    bool isUniRender = true;
    bool filterCacheOcclusionEnabled = true;

    testNode->isNotifyUIBufferAvailable_ = false;
    testNode->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    testNode->isNotifyUIBufferAvailable_ = false;
    testNode->AccumulateOcclusionRegion(
        accumulatedRegion, curRegion, hasFilterCacheOcclusion, isUniRender, filterCacheOcclusionEnabled);
    EXPECT_TRUE(accumulatedRegion.IsEmpty());

    auto parentSurfaceNode = std::make_shared<RSSurfaceRenderNode>(id + 1, context);
    testNode->SetParent(parentSurfaceNode);
    testNode->isOcclusionInSpecificScenes_ = true;
    testNode->AccumulateOcclusionRegion(
        accumulatedRegion, curRegion, hasFilterCacheOcclusion, isUniRender, filterCacheOcclusionEnabled);
    EXPECT_FALSE(hasFilterCacheOcclusion);
}

/**
 * @tc.name: GetVisibleLevelForWMS
 * @tc.desc: test results of GetVisibleLevelForWMS
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSSurfaceRenderNodeTest, GetVisibleLevelForWMS, TestSize.Level1)
{
    RSSurfaceRenderNode node(id);

    EXPECT_EQ(node.GetVisibleLevelForWMS(RSVisibleLevel::RS_INVISIBLE), WINDOW_LAYER_INFO_TYPE::INVISIBLE);

    EXPECT_EQ(node.GetVisibleLevelForWMS(RSVisibleLevel::RS_ALL_VISIBLE), WINDOW_LAYER_INFO_TYPE::ALL_VISIBLE);

    EXPECT_EQ(
        node.GetVisibleLevelForWMS(RSVisibleLevel::RS_SEMI_NONDEFAULT_VISIBLE), WINDOW_LAYER_INFO_TYPE::SEMI_VISIBLE);

    EXPECT_EQ(
        node.GetVisibleLevelForWMS(RSVisibleLevel::RS_SEMI_DEFAULT_VISIBLE), WINDOW_LAYER_INFO_TYPE::SEMI_VISIBLE);

    EXPECT_EQ(
        node.GetVisibleLevelForWMS(RSVisibleLevel::RS_UNKNOW_VISIBLE_LEVEL), WINDOW_LAYER_INFO_TYPE::SEMI_VISIBLE);
}

/**
 * @tc.name: SetVisibleRegionRecursive
 * @tc.desc: test results of SetVisibleRegionRecursive
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, SetVisibleRegionRecursive, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    Occlusion::Rect rect(1, 1, 1, 1);
    Occlusion::Region region(rect);
    Occlusion::Rect rect2(0, 0, 1, 1);
    Occlusion::Region region2(rect2);
    VisibleData visibleVec;
    std::map<NodeId, RSVisibleLevel> visMapForVsyncRate;
    bool needSetVisibleRegion = true;
    RSVisibleLevel visibleLevel = RSVisibleLevel::RS_ALL_VISIBLE;
    bool isSystemAnimatedScenes = false;

    node->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    node->SetVisibleRegionRecursive(
        region, visibleVec, visMapForVsyncRate, needSetVisibleRegion, visibleLevel, isSystemAnimatedScenes);
    node->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;

    node->SetVisibleRegionRecursive(
        region, visibleVec, visMapForVsyncRate, needSetVisibleRegion, visibleLevel, isSystemAnimatedScenes);

    needSetVisibleRegion = false;
    node->SetVisibleRegionRecursive(
        region2, visibleVec, visMapForVsyncRate, needSetVisibleRegion, visibleLevel, isSystemAnimatedScenes);
    ASSERT_FALSE(node->visibleRegionForCallBack_.IsEmpty());

    node->qosPidCal_ = true;
    needSetVisibleRegion = true;
    node->SetVisibleRegionRecursive(
        region2, visibleVec, visMapForVsyncRate, needSetVisibleRegion, visibleLevel, isSystemAnimatedScenes);
    ASSERT_FALSE(node->visibleRegionForCallBack_.IsEmpty());

    auto child1 = std::make_shared<RSSurfaceRenderNode>(id + 1);
    auto child2 = std::make_shared<RSRenderNode>(id + 2);
    std::vector<std::shared_ptr<RSRenderNode>> children;
    children.push_back(child1);
    children.push_back(child2);
    node->fullChildrenList_ = std::make_shared<std::vector<std::shared_ptr<RSRenderNode>>>(children);
    node->SetVisibleRegionRecursive(
        region2, visibleVec, visMapForVsyncRate, needSetVisibleRegion, visibleLevel, isSystemAnimatedScenes);
    ASSERT_FALSE(node->visibleRegionForCallBack_.IsEmpty());
}

/**
 * @tc.name: CalcFilterCacheValidForOcclusion
 * @tc.desc: test results of CalcFilterCacheValidForOcclusion
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, CalcFilterCacheValidForOcclusionTest, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    node->isFilterCacheFullyCovered_ = true;
    node->dirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    node->CalcFilterCacheValidForOcclusion();
    EXPECT_TRUE(node->isFilterCacheStatusChanged_);

    node->isFilterCacheFullyCovered_ = false;
    node->isFilterCacheValidForOcclusion_ = false;
    node->CalcFilterCacheValidForOcclusion();
    EXPECT_FALSE(node->isFilterCacheStatusChanged_);
}

/**
 * @tc.name: UpdateFilterNodesTest
 * @tc.desc: test results of UpdateFilterNodesTest
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, UpdateFilterNodesTest, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    std::shared_ptr<RSRenderNode> renderNode;
    node->UpdateFilterNodes(renderNode);
    EXPECT_EQ(node->filterNodes_.size(), 0);
    renderNode = std::make_shared<RSRenderNode>(id + 1);
    node->UpdateFilterNodes(renderNode);
    EXPECT_EQ(node->filterNodes_.size(), 1);
}
} // namespace Rosen
} // namespace OHOS