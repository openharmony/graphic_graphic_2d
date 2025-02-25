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

#include "ipc_callbacks/buffer_clear_callback_proxy.h"
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
class RSSurfaceRenderNodeUniTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline NodeId id;
    static inline std::weak_ptr<RSContext> context = {};
    static inline RSPaintFilterCanvas* canvas_;
    static inline Drawing::Canvas drawingCanvas_;
    static constexpr uint8_t maxAlpha = 255;
    static constexpr float outerRadius = 30.4f;
    RRect rrect = RRect({0, 0, 0, 0}, outerRadius, outerRadius);
};

void RSSurfaceRenderNodeUniTest::SetUpTestCase()
{
    canvas_ = new RSPaintFilterCanvas(&drawingCanvas_);
}
void RSSurfaceRenderNodeUniTest::TearDownTestCase()
{
    delete canvas_;
    canvas_ = nullptr;
}
void RSSurfaceRenderNodeUniTest::SetUp() {}
void RSSurfaceRenderNodeUniTest::TearDown() {}

class TestDrawableAdapter : public DrawableV2::RSRenderNodeDrawableAdapter {
public:
    explicit TestDrawableAdapter(std::shared_ptr<const RSRenderNode> node)
        : RSRenderNodeDrawableAdapter(std::move(node))
    {}
    void Draw(Drawing::Canvas& canvas) {}
};

/**
 * @tc.name: ContextMatrixTest001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, ContextMatrixTest001, TestSize.Level1)
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
HWTEST_F(RSSurfaceRenderNodeUniTest, ContextClipRegionTest001, TestSize.Level1)
{
    RSSurfaceRenderNode surfaceRenderNode(id, context);
    Drawing::Rect clipRegion { 0, 0, 0, 0 };
    bool sendMsg = false;
    surfaceRenderNode.SetContextClipRegion(clipRegion, sendMsg);
    ASSERT_EQ(surfaceRenderNode.GetId(), 0);
}

/**
 * @tc.name: ConnectToNodeInRenderServiceTest001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, ConnectToNodeInRenderServiceTest001, TestSize.Level1)
{
    RSSurfaceRenderNode surfaceRenderNode(id, context);
    surfaceRenderNode.ConnectToNodeInRenderService();
    ASSERT_EQ(surfaceRenderNode.GetId(), 0);
}

/**
 * @tc.name: SurfaceNodeTypeTest001
 * @tc.desc: Test SurfaceNodeTypeTest001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, SurfaceNodeTypeTest001, TestSize.Level1)
{
    NodeId id = 1;
    RSSurfaceNodeType surfaceNodeType = RSSurfaceNodeType::DEFAULT;
    RSSurfaceRenderNodeConfig config = { .id = id, .nodeType = surfaceNodeType };
    auto testNode = std::make_shared<RSSurfaceRenderNode>(config);
    EXPECT_NE(testNode, nullptr);
    testNode->SetSurfaceNodeType(RSSurfaceNodeType::ABILITY_COMPONENT_NODE);
    bool isSameType = (testNode->GetSurfaceNodeType() == RSSurfaceNodeType::ABILITY_COMPONENT_NODE);
    EXPECT_TRUE(isSameType);
    testNode->SetSurfaceNodeType(RSSurfaceNodeType::DEFAULT);
    isSameType = (testNode->GetSurfaceNodeType() == RSSurfaceNodeType::DEFAULT);
    EXPECT_FALSE(isSameType);
}

/**
 * @tc.name: SurfaceNodeTypeTest002
 * @tc.desc: Test SurfaceNodeTypeTest002
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, SurfaceNodeTypeTest002, TestSize.Level1)
{
    NodeId id = 1;
    RSSurfaceNodeType surfaceNodeType = RSSurfaceNodeType::DEFAULT;
    RSSurfaceRenderNodeConfig config = { .id = id, .nodeType = surfaceNodeType };
    auto testNode = std::make_shared<RSSurfaceRenderNode>(config);
    EXPECT_NE(testNode, nullptr);
    testNode->SetSurfaceNodeType(RSSurfaceNodeType::UI_EXTENSION_COMMON_NODE);
    bool isSameType = (testNode->GetSurfaceNodeType() == RSSurfaceNodeType::DEFAULT);
    EXPECT_TRUE(isSameType);
    testNode->SetSurfaceNodeType(RSSurfaceNodeType::UI_EXTENSION_SECURE_NODE);
    isSameType = (testNode->GetSurfaceNodeType() == RSSurfaceNodeType::DEFAULT);
    EXPECT_TRUE(isSameType);
}

/**
 * @tc.name: SurfaceNodeTypeTest003
 * @tc.desc: Test SurfaceNodeTypeTest003
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, SurfaceNodeTypeTest003, TestSize.Level1)
{
    NodeId id = 1;
    RSSurfaceNodeType surfaceNodeType = RSSurfaceNodeType::UI_EXTENSION_COMMON_NODE;
    RSSurfaceRenderNodeConfig config = { .id = id, .nodeType = surfaceNodeType };
    auto testNode = std::make_shared<RSSurfaceRenderNode>(config);
    EXPECT_NE(testNode, nullptr);
    testNode->SetSurfaceNodeType(RSSurfaceNodeType::DEFAULT);
    bool isSameType = (testNode->GetSurfaceNodeType() == RSSurfaceNodeType::UI_EXTENSION_COMMON_NODE);
    EXPECT_TRUE(isSameType);
}

/**
 * @tc.name: SurfaceNodeTypeTest004
 * @tc.desc: Test SurfaceNodeTypeTest004
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, SurfaceNodeTypeTest004, TestSize.Level1)
{
    NodeId id = 1;
    RSSurfaceNodeType surfaceNodeType = RSSurfaceNodeType::UI_EXTENSION_SECURE_NODE;
    RSSurfaceRenderNodeConfig config = { .id = id, .nodeType = surfaceNodeType };
    auto testNode = std::make_shared<RSSurfaceRenderNode>(config);
    EXPECT_NE(testNode, nullptr);
    testNode->SetSurfaceNodeType(RSSurfaceNodeType::DEFAULT);
    bool isSameType = (testNode->GetSurfaceNodeType() == RSSurfaceNodeType::UI_EXTENSION_SECURE_NODE);
    EXPECT_TRUE(isSameType);
}

/**
 * @tc.name: ChildrenCacheTest001
 * @tc.desc: test ChildrenCacheTest001
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, ChildrenCacheTest001, TestSize.Level1)
{
    RSSurfaceRenderNode surfaceRenderNode(id, context);
    surfaceRenderNode.ResetParent();
    ASSERT_EQ(surfaceRenderNode.GetId(), 0);
}

/**
 * @tc.name: SurfaceOpaqueRegionTest001
 * @tc.desc: test SurfaceOpaqueRegionTest001
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, SurfaceOpaqueRegionTest001, TestSize.Level1)
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
 * @tc.name: SurfaceOpaqueRegionTest002
 * @tc.desc: test SurfaceOpaqueRegionTest002
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, SurfaceOpaqueRegionTest002, TestSize.Level1)
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
 * @tc.name: SurfaceOpaqueRegionTest003
 * @tc.desc: test SurfaceOpaqueRegionTest003
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, SurfaceOpaqueRegionTest003, TestSize.Level1)
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
 * @tc.name: SurfaceOpaqueRegionTest004
 * @tc.desc: test SurfaceOpaqueRegionTest004
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, SurfaceOpaqueRegionTest004, TestSize.Level1)
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
 * @tc.name: SurfaceOpaqueRegionTest005
 * @tc.desc: function test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, SurfaceOpaqueRegionTest005, TestSize.Level1)
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
 * @tc.name: SurfaceOpaqueRegionTest006
 * @tc.desc: test SurfaceOpaqueRegionTest006
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, SurfaceOpaqueRegionTest006, TestSize.Level1)
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
 * @tc.name: NodeCostTest001
 * @tc.desc: test NodeCostTest001
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, NodeCostTest001, TestSize.Level1)
{
    RSSurfaceRenderNode surfaceRenderNode(id, context);
    auto result = surfaceRenderNode.nodeCost_;
    ASSERT_EQ(0, result);
    surfaceRenderNode.SetNodeCost(6);
    result = surfaceRenderNode.nodeCost_;
    ASSERT_EQ(6, result);
}

/**
 * @tc.name: NodeCostTest002
 * @tc.desc: test NodeCostTest002
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, NodeCostTest002, TestSize.Level1)
{
    RSSurfaceRenderNode surfaceRenderNode(id, context);
    auto result = surfaceRenderNode.nodeCost_;
    ASSERT_EQ(0, result);
    surfaceRenderNode.SetNodeCost(6);
    result = surfaceRenderNode.GetNodeCost();
    ASSERT_EQ(6, result);
}

/**
 * @tc.name: FingerprintTest001
 * @tc.desc: SetFingerprint and GetFingerprint
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, FingerprintTest001, TestSize.Level1)
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
 * @tc.name: ShouldPrepareSubnodesTest001
 * @tc.desc: test ShouldPrepareSubnodesTest001
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, ShouldPrepareSubnodesTest001, TestSize.Level1)
{
    auto testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    testNode->ShouldPrepareSubnodes();
    ASSERT_TRUE(testNode->ShouldPrepareSubnodes());
}

/**
 * @tc.name: CollectSurfaceTest001
 * @tc.desc: test CollectSurfaceTest001
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, CollectSurfaceTest001, TestSize.Level1)
{
    auto testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    std::shared_ptr<RSBaseRenderNode> rsBaseRenderNode;
    std::vector<RSBaseRenderNode::SharedPtr> vec;
    bool isUniRender = true;
    testNode->nodeType_ = RSSurfaceNodeType::STARTING_WINDOW_NODE;
    testNode->CollectSurface(rsBaseRenderNode, vec, isUniRender, false);
    ASSERT_FALSE(vec.empty());
}

/**
 * @tc.name: CollectSurfaceTest002
 * @tc.desc: test CollectSurfaceTest002
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, CollectSurfaceTest002, TestSize.Level1)
{
    auto testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    std::shared_ptr<RSBaseRenderNode> rsBaseRenderNode;
    std::vector<RSBaseRenderNode::SharedPtr> vec;
    bool isUniRender = true;
    testNode->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    testNode->CollectSurface(rsBaseRenderNode, vec, isUniRender, true);
    ASSERT_FALSE(vec.empty());
}

/**
 * @tc.name: ProcessAnimatePropertyBeforeChildrenTest001
 * @tc.desc: test ProcessAnimatePropertyBeforeChildrenTest001
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, ProcessAnimatePropertyBeforeChildrenTest001, TestSize.Level1)
{
    auto testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    testNode->ProcessAnimatePropertyBeforeChildren(*canvas_, true);
    ASSERT_EQ(testNode->GetId(), 0);
}

/**
 * @tc.name: ProcessAnimatePropertyAfterChildrenTest002
 * @tc.desc: test ProcessAnimatePropertyAfterChildrenTest002
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, ProcessAnimatePropertyAfterChildrenTest002, TestSize.Level1)
{
    auto testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    testNode->ProcessAnimatePropertyAfterChildren(*canvas_);
    ASSERT_EQ(testNode->GetId(), 0);
}

/**
 * @tc.name: ContextMatrixTest002
 * @tc.desc: test ContextMatrixTest002
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, ContextMatrixTest002, TestSize.Level1)
{
    std::optional<Drawing::Matrix> matrix;
    bool sendMsg = false;
    auto testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    testNode->SetContextMatrix(matrix, sendMsg);
    ASSERT_EQ(testNode->GetId(), 0);
}

/**
 * @tc.name: RegisterBufferAvailableListenerTest001
 * @tc.desc: test RegisterBufferAvailableListenerTest001
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, RegisterBufferAvailableListenerTest001, TestSize.Level1)
{
    sptr<RSIBufferAvailableCallback> callback;
    bool isFromRenderThread = true;
    auto testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    testNode->RegisterBufferAvailableListener(callback, isFromRenderThread);
    ASSERT_EQ(testNode->GetId(), 0);
}

/**
 * @tc.name: BootAnimationTest001
 * @tc.desc: SetBootAnimation and GetBootAnimation
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, BootAnimationTest001, TestSize.Level1)
{
    auto testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    testNode->SetBootAnimation(true);
    ASSERT_EQ(testNode->GetBootAnimation(), true);
    testNode->SetBootAnimation(false);
    ASSERT_FALSE(testNode->GetBootAnimation());
}

/**
 * @tc.name: GlobalPositionEnabledTest001
 * @tc.desc: SetGlobalPositionEnabled and GetGlobalPositionEnabled
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, GlobalPositionEnabledTest001, TestSize.Level1)
{
    auto testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    testNode->stagingRenderParams_ = std::make_unique<RSRenderParams>(id);
    testNode->SetGlobalPositionEnabled(true);
    ASSERT_EQ(testNode->GetGlobalPositionEnabled(), true);
    testNode->SetGlobalPositionEnabled(false);
    ASSERT_FALSE(testNode->GetGlobalPositionEnabled());
}

/**
 * @tc.name: AncestorDisplayNodeTest001
 * @tc.desc: SetAncestorDisplayNode and GetAncestorDisplayNode
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, AncestorDisplayNodeTest001, TestSize.Level1)
{
    auto testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    auto displayNode = std::make_shared<RSBaseRenderNode>(0, context);
    testNode->SetAncestorDisplayNode(0, displayNode);
    auto ancestorDisplayNodeMap = testNode->GetAncestorDisplayNode();
    ASSERT_FALSE(ancestorDisplayNodeMap.empty());
    ASSERT_EQ(ancestorDisplayNodeMap.begin()->second.lock(), displayNode);
}

/**
 * @tc.name: UpdateSurfaceCacheContentStaticTest001
 * @tc.desc: Set dirty subNode and check if surfacenode static
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, UpdateSurfaceCacheContentStaticTest001, TestSize.Level1)
{
    auto testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    auto subNode = std::make_shared<RSRenderNode>(id + 1, context);
    if (testNode == nullptr || subNode == nullptr) {
        return;
    }
    std::unordered_map<NodeId, std::weak_ptr<RSRenderNode>> activeNodeIds;
    testNode->UpdateSurfaceCacheContentStatic(activeNodeIds);
    std::shared_ptr<RSRenderNode> nullNode = nullptr;
    activeNodeIds[subNode->GetId()] = nullNode;
    testNode->UpdateSurfaceCacheContentStatic(activeNodeIds);
    activeNodeIds[subNode->GetId()] = subNode;
    testNode->AddChild(subNode, 0);
    subNode->isContentDirty_ = true;
    testNode->UpdateSurfaceCacheContentStatic(activeNodeIds);
    ASSERT_EQ(testNode->GetSurfaceCacheContentStatic(), false);
    ASSERT_EQ(testNode->IsContentDirtyNodeLimited(), true);
}

/**
 * @tc.name: IsContentDirtyNodeLimitedTest001
 * @tc.desc: Set content dirty subnode new on the tree and check if it is in count
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, IsContentDirtyNodeLimitedTest001, TestSize.Level1)
{
    auto testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    auto subnode = std::make_shared<RSRenderNode>(id + 1, context);
    if (testNode == nullptr || subnode == nullptr) {
        return;
    }
    testNode->AddChild(subnode, 0);
    subnode->isContentDirty_ = true;
    subnode->isNewOnTree_ = true;
    std::unordered_map<NodeId, std::weak_ptr<RSRenderNode>> activeNodeIds = {{subnode->GetId(), subnode}};
    testNode->UpdateSurfaceCacheContentStatic(activeNodeIds);
    ASSERT_EQ(testNode->IsContentDirtyNodeLimited(), false);
}

/**
 * @tc.name: SkipLayerTest001
 * @tc.desc: Test SetSkipLayer for single surface node which is skip layer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, SkipLayerTest001, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto testNode = std::make_shared<RSSurfaceRenderNode>(id, rsContext);
    ASSERT_NE(testNode, nullptr);

    testNode->SetSkipLayer(true);
    ASSERT_TRUE(testNode->GetSkipLayer());
}

/**
 * @tc.name: SnapshotSkipLayerTest001
 * @tc.desc: Test SetSnapshotSkipLayer for single surface node which is skip layer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, SnapshotSkipLayerTest001, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto testNode = std::make_shared<RSSurfaceRenderNode>(id, rsContext);
    ASSERT_NE(testNode, nullptr);

    testNode->SetSnapshotSkipLayer(true);
    ASSERT_TRUE(testNode->GetSnapshotSkipLayer());
}

/**
 * @tc.name: SkipLayerTest002
 * @tc.desc: Test SetSkipLayer for surface node while skip Layer isn't first level node
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, SkipLayerTest002, TestSize.Level2)
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

    ASSERT_TRUE(parentNode->GetHasSkipLayer());
}

/**
 * @tc.name: SnapshotSkipLayerTest002
 * @tc.desc: Test SetSnapshotSkipLayer for surface node while skip Layer isn't first level node
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, SnapshotSkipLayerTest002, TestSize.Level2)
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

    ASSERT_TRUE(parentNode->GetHasSnapshotSkipLayer());
}

/**
 * @tc.name: SecurityLayerTest001
 * @tc.desc: Test SetSecurityLayer for single surface node which is security layer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, SecurityLayerTest001, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto testNode = std::make_shared<RSSurfaceRenderNode>(id, rsContext);
    ASSERT_NE(testNode, nullptr);

    testNode->SetSecurityLayer(true);
    ASSERT_TRUE(testNode->GetSecurityLayer());
}

/**
 * @tc.name: SecurityLayerTest002
 * @tc.desc: Test SetSecurityLayer for surface node while security Layer isn't first level node
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, SecurityLayerTest002, TestSize.Level2)
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

    ASSERT_TRUE(parentNode->GetHasSecurityLayer());
}

/**
 * @tc.name: StoreMustRenewedInfoTest001
 * @tc.desc: Test StoreMustRenewedInfo while has filter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, StoreMustRenewedInfoTest001, TestSize.Level2)
{
    auto testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    ASSERT_NE(testNode, nullptr);
    testNode->InitRenderParams();
    testNode->SetChildHasVisibleFilter(true);
    testNode->RSRenderNode::StoreMustRenewedInfo();
    testNode->StoreMustRenewedInfo();
    ASSERT_TRUE(testNode->HasMustRenewedInfo());
}

/**
 * @tc.name: StoreMustRenewedInfoTest002
 * @tc.desc: Test StoreMustRenewedInfo while has effect node
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, StoreMustRenewedInfoTest002, TestSize.Level2)
{
    auto testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    ASSERT_NE(testNode, nullptr);

    testNode->InitRenderParams();
    testNode->SetChildHasVisibleEffect(true);
    testNode->RSRenderNode::StoreMustRenewedInfo();
    testNode->StoreMustRenewedInfo();
    ASSERT_TRUE(testNode->HasMustRenewedInfo());
}

/**
 * @tc.name: StoreMustRenewedInfoTest003
 * @tc.desc: Test StoreMustRenewedInfo while has hardware node
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, StoreMustRenewedInfoTest003, TestSize.Level2)
{
    auto testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    ASSERT_NE(testNode, nullptr);

    testNode->SetHasHardwareNode(true);
    testNode->RSRenderNode::StoreMustRenewedInfo();
    testNode->StoreMustRenewedInfo();
    ASSERT_TRUE(testNode->HasMustRenewedInfo());
}

/**
 * @tc.name: StoreMustRenewedInfoTest004
 * @tc.desc: Test StoreMustRenewedInfo while is skip layer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, StoreMustRenewedInfoTest004, TestSize.Level2)
{
    auto testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    ASSERT_NE(testNode, nullptr);

    testNode->SetSkipLayer(true);
    testNode->RSRenderNode::StoreMustRenewedInfo();
    testNode->StoreMustRenewedInfo();
    ASSERT_TRUE(testNode->HasMustRenewedInfo());
}

/**
 * @tc.name: StoreMustRenewedInfoTest005
 * @tc.desc: Test StoreMustRenewedInfo while is security layer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, StoreMustRenewedInfoTest005, TestSize.Level2)
{
    auto testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    ASSERT_NE(testNode, nullptr);

    testNode->SetSecurityLayer(true);
    testNode->RSRenderNode::StoreMustRenewedInfo();
    testNode->StoreMustRenewedInfo();
    ASSERT_TRUE(testNode->HasMustRenewedInfo());
}

/**
 * @tc.name: StoreMustRenewedInfoTest006
 * @tc.desc: Test StoreMustRenewedInfo while is protected layer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, StoreMustRenewedInfoTest006, TestSize.Level2)
{
    auto testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    ASSERT_NE(testNode, nullptr);

    testNode->SetProtectedLayer(true);
    testNode->RSRenderNode::StoreMustRenewedInfo();
    testNode->StoreMustRenewedInfo();
    ASSERT_TRUE(testNode->HasMustRenewedInfo());
}

/**
 * @tc.name: CornerRadiusInfoForDRMTest001
 * @tc.desc: Test SetCornerRadiusInfoForDRM and GetCornerRadiusInfoForDRM
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, CornerRadiusInfoForDRMTest001, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto testNode = std::make_shared<RSSurfaceRenderNode>(id, rsContext);
    ASSERT_NE(testNode, nullptr);
    std::vector<float> cornerRadiusInfo = {};
    testNode->SetCornerRadiusInfoForDRM(cornerRadiusInfo);
    ASSERT_TRUE(testNode->GetCornerRadiusInfoForDRM().empty());
}

/**
 * @tc.name: GetFirstLevelNodeIdTest001
 * @tc.desc: Test GetFirstLevelNode for single app window node
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, GetFirstLevelNodeIdTest001, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto testNode = std::make_shared<RSSurfaceRenderNode>(id, rsContext);
    ASSERT_NE(testNode, nullptr);
    NodeId nodeId = testNode->GetId();
    pid_t pid = ExtractPid(nodeId);
    rsContext->GetMutableNodeMap().renderNodeMap_[pid][nodeId] = testNode;
    testNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    testNode->SetIsOnTheTree(true);
    ASSERT_EQ(testNode->GetFirstLevelNodeId(), testNode->GetId());
}

/**
 * @tc.name: GetFirstLevelNodeIdTest002
 * @tc.desc: Test GetFirstLevelNode for app window node which parent is leash window node
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, GetFirstLevelNodeIdTest002, TestSize.Level2)
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
 * @tc.name: HasSharedTransitionNodeTest001
 * @tc.desc: Test HasSharedTransitionNodeTest001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, HasSharedTransitionNodeTest001, TestSize.Level2)
{
    auto testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    ASSERT_NE(testNode, nullptr);
    testNode->SetHasSharedTransitionNode(true);
    
    ASSERT_EQ(testNode->GetHasSharedTransitionNode(), true);
}

/**
 * @tc.name: QuerySubAssignableTest001
 * @tc.desc: Test QuerySubAssignableTest001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, QuerySubAssignableTest001, TestSize.Level2)
{
    auto testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    ASSERT_NE(testNode, nullptr);
    
    ASSERT_EQ(testNode->QuerySubAssignable(false), true);
}

/**
 * @tc.name: QuerySubAssignableTest002
 * @tc.desc: Test QuerySubAssignable while has filter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, QuerySubAssignableTest002, TestSize.Level2)
{
    auto testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    ASSERT_NE(testNode, nullptr);
    
    if (RSUniRenderJudgement::IsUniRender()) {
        testNode->InitRenderParams();
    }
    testNode->SetHasFilter(true);
    
    ASSERT_EQ(testNode->QuerySubAssignable(false), false);
}

/**
 * @tc.name: QuerySubAssignableTest003
 * @tc.desc: Test QuerySubAssignable while node's child has filter and child is transparent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, QuerySubAssignableTest003, TestSize.Level2)
{
    auto testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    auto childNode = std::make_shared<RSSurfaceRenderNode>(id + 1, context);
    ASSERT_NE(testNode, nullptr);
    ASSERT_NE(childNode, nullptr);
    RSUniRenderJudgement::uniRenderEnabledType_ = UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL;
    if (RSUniRenderJudgement::IsUniRender()) {
        testNode->InitRenderParams();
        childNode->InitRenderParams();
    }
    childNode->SetHasFilter(true);
    testNode->SetChildHasVisibleFilter(true);
    
    ASSERT_EQ(testNode->QuerySubAssignable(false), false);
}

/**
 * @tc.name: QuerySubAssignableTest004
 * @tc.desc: Test QuerySubAssignable while node's child has filter and is not transparent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, QuerySubAssignableTest004, TestSize.Level2)
{
    auto testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    auto childNode = std::make_shared<RSSurfaceRenderNode>(id + 1, context);
    ASSERT_NE(testNode, nullptr);
    ASSERT_NE(childNode, nullptr);

    RSUniRenderJudgement::uniRenderEnabledType_ = UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL;
    if (RSUniRenderJudgement::IsUniRender()) {
        testNode->InitRenderParams();
        childNode->InitRenderParams();
    }
    childNode->SetHasFilter(true);
    testNode->SetChildHasVisibleFilter(true);
    testNode->SetAbilityBGAlpha(maxAlpha);
    
    ASSERT_EQ(testNode->QuerySubAssignable(false), true);
}

/**
 * @tc.name: SetForceHardwareAndFixRotationTest001
 * @tc.desc: Test SetForceHardwareAndFixRotationTest001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, SetForceHardwareAndFixRotationTest001, TestSize.Level2)
{
    auto testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    ASSERT_NE(testNode, nullptr);

    testNode->InitRenderParams();
    testNode->SetForceHardwareAndFixRotation(true);
    ASSERT_EQ(testNode->isFixRotationByUser_, true);
}

/**
 * @tc.name: SetForceHardwareAndFixRotationTest002
 * @tc.desc: Test SetForceHardwareAndFixRotationTest002
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, SetForceHardwareAndFixRotationTest002, TestSize.Level2)
{
    auto testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    ASSERT_NE(testNode, nullptr);

    testNode->InitRenderParams();
    testNode->SetForceHardwareAndFixRotation(false);
    ASSERT_EQ(testNode->isFixRotationByUser_, false);
}

/**
 * @tc.name: UpdateSrcRectTest001
 * @tc.desc: test results of UpdateSrcRect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, UpdateSrcRectTest001, TestSize.Level1)
{
    Drawing::Canvas canvas;
    Drawing::RectI dstRect(0, 0, 100, 100);
    bool hasRotation = false;

    auto renderNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    renderNode->UpdateSrcRect(canvas, dstRect, hasRotation);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: UpdateHwcDisabledBySrcRectTest001
 * @tc.desc: test results of UpdateHwcDisabledBySrcRect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, UpdateHwcDisabledBySrcRectTest001, TestSize.Level1)
{
    bool hasRotation = false;
    auto buffer = SurfaceBuffer::Create();
    auto renderNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    renderNode->UpdateHwcDisabledBySrcRect(hasRotation);

    renderNode->GetRSSurfaceHandler()->buffer_.buffer = buffer;
    renderNode->UpdateHwcDisabledBySrcRect(hasRotation);
    ASSERT_FALSE(renderNode->isHardwareForcedDisabledBySrcRect_);
}

/**
 * @tc.name: YUVBufferFormatTest001
 * @tc.desc: test results of IsYUVBufferFormat
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, YUVBufferFormatTest001, TestSize.Level1)
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
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, ShouldPrepareSubnodesTest001, TestSize.Level1)
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
 * @tc.name: DirtyRegionDumpTest001
 * @tc.desc: test results of DirtyRegionDump
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, DirtyRegionDumpTest001, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    testNode->DirtyRegionDump();

    testNode->dirtyManager_.reset();
    std::string dump = testNode->DirtyRegionDump();
    ASSERT_NE(dump, "");
}

/**
 * @tc.name: PrepareRenderBeforeChildrenTest001
 * @tc.desc: test results of PrepareRenderBeforeChildren
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, PrepareRenderBeforeChildrenTest001, TestSize.Level1)
{
    Drawing::Canvas canvas;
    RSPaintFilterCanvas rsPaintFilterCanvas(&canvas);
    std::shared_ptr<RSSurfaceRenderNode> testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    testNode->InitRenderParams();
    testNode->PrepareRenderBeforeChildren(rsPaintFilterCanvas);
    ASSERT_NE(testNode->GetRenderProperties().GetBoundsGeometry(), nullptr);
}

/**
 * @tc.name: CollectSurfaceTest001
 * @tc.desc: test results of CollectSurface
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, CollectSurfaceTest001, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> testNode = std::make_shared<RSSurfaceRenderNode>(id, context);

    testNode->nodeType_ = RSSurfaceNodeType::SCB_SCREEN_NODE;
    auto testNode = std::make_shared<RSBaseRenderNode>(id, context);
    Drawing::Canvas canvasArgs;
    RSPaintFilterCanvas canvas(&canvasArgs);
    std::vector<std::shared_ptr<RSRenderNode>> vec;
    testNode->CollectSurface(testNode, vec, true, false);

    testNode->nodeType_ = RSSurfaceNodeType::STARTING_WINDOW_NODE;
    testNode->CollectSurface(testNode, vec, true, false);
    testNode->CollectSurface(testNode, vec, false, false);
    testNode->nodeType_ = RSSurfaceNodeType::SCB_SCREEN_NODE;
    testNode->CollectSurface(testNode, vec, true, false);
    testNode->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    testNode->CollectSurface(testNode, vec, true, true);
    ASSERT_FALSE(testNode->isSubSurfaceEnabled_);
}

/**
 * @tc.name: ChildrenCacheTest002
 * @tc.desc: test results of ClearChildrenCache
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, ChildrenCacheTest002, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    testNode->ClearChildrenCache();
    ASSERT_EQ(testNode->nodeType_, RSSurfaceNodeType::DEFAULT);
}

/**
 * @tc.name: OnTreeStateChangedTest001
 * @tc.desc: test results of OnTreeStateChanged
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, OnTreeStateChangedTest001, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    testNode->OnTreeStateChanged();
    testNode->nodeType_ = RSSurfaceNodeType::ABILITY_COMPONENT_NODE;
    testNode->OnTreeStateChanged();
    ASSERT_EQ(testNode->nodeType_, RSSurfaceNodeType::ABILITY_COMPONENT_NODE);
}

/**
 * @tc.name: OnResetParentTest001
 * @tc.desc: test results of OnResetParent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, OnResetParentTest001, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> testNode = std::make_shared<RSSurfaceRenderNode>(id, context);

    testNode->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    testNode->OnResetParent();

    testNode->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    testNode->OnResetParent();
    ASSERT_EQ(testNode->nodeType_, RSSurfaceNodeType::SELF_DRAWING_NODE);
}

/**
 * @tc.name: SetIsNotifyUIBufferAvailableTest001
 * @tc.desc: test results of SetIsNotifyUIBufferAvailable
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, SetIsNotifyUIBufferAvailableTest001, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    testNode->SetIsNotifyUIBufferAvailable(true);
    ASSERT_TRUE(testNode->isNotifyUIBufferAvailable_.load());
}

/**
 * @tc.name: SubTreeNeedPrepareTest001
 * @tc.desc: test results of IsSubTreeNeedPrepare
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, SubTreeNeedPrepareTest001, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    testNode->IsSubTreeNeedPrepare(false, false);
    testNode->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    EXPECT_TRUE(testNode->IsSubTreeNeedPrepare(true, true));
}

/**
 * @tc.name: PrepareTest001
 * @tc.desc: test results of QuickPrepare
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, PrepareTest001, TestSize.Level1)
{
    std::shared_ptr<RSRenderThreadVisitor> visitor;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    surfaceNode->QuickPrepare(visitor);

    visitor = std::make_shared<RSRenderThreadVisitor>();
    surfaceNode->QuickPrepare(visitor);
    ASSERT_EQ(surfaceNode->nodeType_, RSSurfaceNodeType::DEFAULT);
}

/**
 * @tc.name: ProcessTest001
 * @tc.desc: test results of Process
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, ProcessTest001, TestSize.Level1)
{
    std::shared_ptr<RSRenderThreadVisitor> visitor;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    surfaceNode->Process(visitor);
    visitor = std::make_shared<RSRenderThreadVisitor>();
    surfaceNode->Process(visitor);
    ASSERT_EQ(surfaceNode->nodeType_, RSSurfaceNodeType::DEFAULT);
}

/**
 * @tc.name: ProcessAnimatePropertyBeforeChildrenTest001
 * @tc.desc: test results of ProcessAnimatePropertyBeforeChildren
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, ProcessAnimatePropertyBeforeChildrenTest001, TestSize.Level1)
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
 * @tc.name: ProcessRenderAfterChildrenTest001
 * @tc.desc: test results of ProcessRenderAfterChildren
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, ProcessRenderAfterChildrenTest001, TestSize.Level1)
{
    Drawing::Canvas canvasArgs;
    auto canvas = std::make_shared<RSPaintFilterCanvas>(&canvasArgs);
    auto testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    testNode->ProcessRenderAfterChildren(*canvas);
    EXPECT_FALSE(testNode->needDrawAnimateProperty_);
}

/**
 * @tc.name: ContextAlphaTest001
 * @tc.desc: test results of SetContextAlpha
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, ContextAlphaTest001, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    testNode->SetContextAlpha(1.0f, true);
    testNode->SetContextAlpha(0.5f, true);
    testNode->SetContextAlpha(0.5f, false);
    EXPECT_EQ(testNode->contextAlpha_, 0.5f);
}

/**
 * @tc.name: ContextClipRegionTest001
 * @tc.desc: test results of GetContextClipRegion
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, ContextClipRegionTest001, TestSize.Level1)
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
 * @tc.name: SkipLayerTest003
 * @tc.desc: test results of SetSkipLayer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, SkipLayerTest003, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    testNode->SetSkipLayer(true);
    EXPECT_TRUE(testNode->isSkipLayer_);
    testNode->SetSkipLayer(false);
    EXPECT_FALSE(testNode->isSkipLayer_);
}

/**
 * @tc.name: SnapshotSkipLayerTest003
 * @tc.desc: test results of SetSnapshotSkipLayer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, SnapshotSkipLayerTest003, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    testNode->SetSnapshotSkipLayer(true);
    EXPECT_TRUE(testNode->isSnapshotSkipLayer_);
    testNode->SetSnapshotSkipLayer(false);
    EXPECT_FALSE(testNode->isSnapshotSkipLayer_);
}

/**
 * @tc.name: SyncSecurityInfoToFirstLevelNodeTest001
 * @tc.desc: test results of SyncSecurityInfoToFirstLevelNode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, SyncSecurityInfoToFirstLevelNodeTest001, TestSize.Level1)
{
    auto testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    testNode->SyncSecurityInfoToFirstLevelNode();
    EXPECT_FALSE(testNode->isSkipLayer_);
}

/**
 * @tc.name: SyncSkipInfoToFirstLevelNodeTest001
 * @tc.desc: test results of SyncSkipInfoToFirstLevelNode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, SyncSkipInfoToFirstLevelNodeTest001, TestSize.Level1)
{
    auto testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    testNode->SyncSkipInfoToFirstLevelNode();
    EXPECT_FALSE(testNode->isSkipLayer_);
}

/**
 * @tc.name: SyncSnapshotSkipInfoToFirstLevelNodeTest001
 * @tc.desc: test results of SyncSnapshotSkipInfoToFirstLevelNode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, SyncSnapshotSkipInfoToFirstLevelNodeTest001, TestSize.Level1)
{
    auto testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    testNode->SyncSnapshotSkipInfoToFirstLevelNode();
    EXPECT_FALSE(testNode->isSnapshotSkipLayer_);
}

/**
 * @tc.name: NotifyTreeStateChangeTest001
 * @tc.desc: test results of NotifyTreeStateChange
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, NotifyTreeStateChangeTest001, TestSize.Level1)
{
    auto testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    testNode->NotifyTreeStateChange();
    bool callbackCalled = false;
    testNode->RegisterTreeStateChangeCallback(
        [&callbackCalled](const RSSurfaceRenderNode& testNode) { callbackCalled = true; });
    testNode->NotifyTreeStateChange();
    EXPECT_TRUE(callbackCalled);
}

/**
 * @tc.name: UpdateSurfaceDefaultSizeTest001
 * @tc.desc: test results of UpdateSurfaceDefaultSize
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, UpdateSurfaceDefaultSizeTest001, TestSize.Level1)
{
    auto context = std::make_shared<RSContext>();
    auto testNode = std::make_shared<RSSurfaceRenderNode>(1, context, true);
    testNode->UpdateSurfaceDefaultSize(1920.0f, 1080.0f);
    testNode->GetRSSurfaceHandler()->consumer_ = IConsumerSurface::Create();
    testNode->UpdateSurfaceDefaultSize(1920.0f, 1080.0f);
    ASSERT_NE(testNode->GetRSSurfaceHandler()->consumer_, nullptr);
}

/**
 * @tc.name: NeedClearBufferCacheTest001
 * @tc.desc: Test NeedClearBufferCache with buffer and preBuffer
 * @tc.type: FUNC
 * @tc.require: issueIBF44R
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, NeedClearBufferCacheTest001, TestSize.Level1)
{
    auto testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    ASSERT_NE(testNode, nullptr);

    testNode->InitRenderParams();
    ASSERT_NE(testNode->GetRSSurfaceHandler(), nullptr);

    auto buffer = SurfaceBuffer::Create();
    auto preBuffer = SurfaceBuffer::Create();

    testNode->GetRSSurfaceHandler()->buffer_.buffer = buffer;
    testNode->GetRSSurfaceHandler()->preBuffer_.buffer = preBuffer;

    std::set<uint32_t> bufferCacheSet;
    
    testNode->NeedClearBufferCache(bufferCacheSet);

    ASSERT_EQ(bufferCacheSet.size(), 2);
}

/**
 * @tc.name: NeedClearBufferCacheTest002
 * @tc.desc: Test NeedClearBufferCache without buffer
 * @tc.type: FUNC
 * @tc.require: issueIBF44R
 */
HWTEST_F(RSSurfaceRenderNodeUnitTest, NeedClearBufferCacheTest002, TestSize.Level1)
{
    auto testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    ASSERT_NE(testNode, nullptr);

    testNode->InitRenderParams();
    
    std::set<uint32_t> bufferCacheSet;
    
    testNode->NeedClearBufferCache(bufferCacheSet);

    EXPECT_TRUE(bufferCacheSet.empty());
}

/**
 * @tc.name: NeedClearBufferCacheTest003
 * @tc.desc: Test NeedClearBufferCache with null surfaceHandler
 * @tc.type: FUNC
 * @tc.require: issueIBF44R
 */
HWTEST_F(RSSurfaceRenderNodeUnitTest, NeedClearBufferCacheTest003, TestSize.Level1)
{
    auto testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    ASSERT_NE(testNode, nullptr);

    testNode->surfaceHandler_ = nullptr;
    
    std::set<uint32_t> bufferCacheSet;
    
    testNode->NeedClearBufferCache(bufferCacheSet);

    EXPECT_TRUE(bufferCacheSet.empty());
}

/**
 * @tc.name: RegisterBufferAvailableListenerTest001
 * @tc.desc: test results of RegisterBufferAvailableListener
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, RegisterBufferAvailableListenerTest001, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    sptr<RSIBufferAvailableCallback> callback;
    bool isFromRenderThread = false;
    testNode->RegisterBufferAvailableListener(callback, isFromRenderThread);
    EXPECT_FALSE(testNode->isSkipLayer_);
}

/**
 * @tc.name: SetNotifyRTBufferAvailableTest001
 * @tc.desc: test results of SetNotifyRTBufferAvailable
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, SetNotifyRTBufferAvailableTest001, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    testNode->SetNotifyRTBufferAvailable(true);
    ASSERT_TRUE(testNode->isNotifyRTBufferAvailable_);
}

/**
 * @tc.name: ConnectToNodeInRenderServiceTest001
 * @tc.desc: test results of ConnectToNodeInRenderService
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, ConnectToNodeInRenderServiceTest001, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    testNode->ConnectToNodeInRenderService();
    EXPECT_FALSE(testNode->isSkipLayer_);
}

/**
 * @tc.name: NotifyRTBufferAvailableTest001
 * @tc.desc: test results of NotifyRTBufferAvailable
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUniTest, NotifyRTBufferAvailableTest001, TestSize.Level1)
{
    auto testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    testNode->NotifyRTBufferAvailable(false);
    ASSERT_FALSE(testNode->isNotifyRTBufferAvailablePre_);
    testNode->NotifyRTBufferAvailable(true);
    testNode->isRefresh_ = true;
    testNode->NotifyRTBufferAvailable(true);
    ASSERT_FALSE(testNode->isNotifyRTBufferAvailable_);
}
} // namespace Rosen
} // namespace OHOS