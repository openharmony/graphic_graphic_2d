/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "pipeline/rs_surface_render_node.h"

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
};

void RSSurfaceRenderNodeTest::SetUpTestCase()
{
    canvas_ = new RSPaintFilterCanvas(&drawingCanvas_);
}
void RSSurfaceRenderNodeTest::TearDownTestCase()
{
    delete canvas_;
}
void RSSurfaceRenderNodeTest::SetUp() {}
void RSSurfaceRenderNodeTest::TearDown() {}

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
}

/**
 * @tc.name: SetVisibleDirtyRegion
 * @tc.desc: function test
 * @tc.type:FUNC
 * @tc.require: I68IPR
 */
HWTEST_F(RSSurfaceRenderNodeTest, SetVisibleDirtyRegion, TestSize.Level1)
{
    RSSurfaceRenderNode surfaceRenderNode(id, context);
    Occlusion::Rect rect {0, 0, 200, 200};
    Occlusion::Region region {rect};
    surfaceRenderNode.SetVisibleDirtyRegion(region);
    auto vdRegion = surfaceRenderNode.GetVisibleDirtyRegion();
}

/**
 * @tc.name: SetAlignedVisibleDirtyRegion
 * @tc.desc: function test
 * @tc.type:FUNC
 * @tc.require: I68IPR
 */
HWTEST_F(RSSurfaceRenderNodeTest, SetAlignedVisibleDirtyRegion, TestSize.Level1)
{
    RSSurfaceRenderNode surfaceRenderNode(id, context);
    Occlusion::Rect rect {0, 0, 256, 256};
    Occlusion::Region region {rect};
    surfaceRenderNode.SetAlignedVisibleDirtyRegion(region);
    auto vdRegion = surfaceRenderNode.GetAlignedVisibleDirtyRegion();
}

/**
 * @tc.name: SetDirtyRegionBelowCurrentLayer
 * @tc.desc: function test
 * @tc.type:FUNC
 * @tc.require: I68IPR
 */
HWTEST_F(RSSurfaceRenderNodeTest, SetDirtyRegionBelowCurrentLayer, TestSize.Level1)
{
    RSSurfaceRenderNode surfaceRenderNode(id, context);
    Occlusion::Rect rect {0, 0, 256, 256};
    Occlusion::Region region {rect};
    surfaceRenderNode.SetDirtyRegionBelowCurrentLayer(region);
    auto vdRegion = surfaceRenderNode.GetDirtyRegionBelowCurrentLayer();
}

/**
 * @tc.name: SetGlobalDirtyRegion
 * @tc.desc: function test
 * @tc.type:FUNC
 * @tc.require: I68IPR
 */
HWTEST_F(RSSurfaceRenderNodeTest, SetGlobalDirtyRegion, TestSize.Level1)
{
    RSSurfaceRenderNode surfaceRenderNode(id, context);
    RectI rect {0, 0, 256, 256};
    surfaceRenderNode.SetGlobalDirtyRegion(rect);
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
    RectI screenRect {0, 0, 2560, 1600};
    RectI absRect {0, 100, 400, 500};
    surfaceRenderNode.SetAbilityBGAlpha(255);
    surfaceRenderNode.SetGlobalAlpha(1.0f);
    surfaceRenderNode.SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    surfaceRenderNode.SetContainerWindow(true, 1.0f);
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
    RectI screenRect {0, 0, 2560, 1600};
    RectI absRect {0, 100, 400, 500};
    surfaceRenderNode.SetAbilityBGAlpha(255);
    surfaceRenderNode.SetGlobalAlpha(1.0f);
    surfaceRenderNode.SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    surfaceRenderNode.SetContainerWindow(true, 1.0f);
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
    RectI screenRect {0, 0, 2560, 1600};
    RectI absRect {0, 100, 400, 500};
    surfaceRenderNode.SetAbilityBGAlpha(255);
    surfaceRenderNode.SetGlobalAlpha(1.0f);
    surfaceRenderNode.SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    surfaceRenderNode.SetContainerWindow(true, 1.0f);
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
    RectI screenRect {0, 0, 2560, 1600};
    RectI absRect {0, 100, 400, 500};
    surfaceRenderNode.SetAbilityBGAlpha(255);
    surfaceRenderNode.SetGlobalAlpha(1.0f);
    surfaceRenderNode.SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    surfaceRenderNode.SetContainerWindow(true, 1.0f);
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
    node->IsStartingWindow();
    node->CollectSurface(rsBaseRenderNode, vec, isUniRender, false);
    ASSERT_FALSE(vec.empty());
}

/**
 * @tc.name: CollectSurfaceTest
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
    node->IsLeashWindow();
    node->CollectSurface(rsBaseRenderNode, vec, isUniRender, false);
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
 * @tc.desc: Set dirty subnode and check if surfacenode static
 * @tc.type:FUNC
 * @tc.require:I8W7ZS
 */
HWTEST_F(RSSurfaceRenderNodeTest, UpdateSurfaceCacheContentStatic, TestSize.Level1)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    auto subnode = std::make_shared<RSRenderNode>(id + 1, context);
    if (node == nullptr || subnode == nullptr) {
        return;
    }
    node->AddChild(subnode, 0);
    subnode->isContentDirty_ = true;
    std::unordered_map<NodeId, std::weak_ptr<RSRenderNode>> activeNodeIds = {{subnode->GetId(), subnode}};
    node->UpdateSurfaceCacheContentStatic(activeNodeIds);
    ASSERT_EQ(node->GetSurfaceCacheContentStatic(), false);
    ASSERT_EQ(node->IsContentDirtyNodeLimited(), true);
}

/**
 * @tc.name: SubNodeNeedDraw001
 * @tc.desc: check if subnode need draw when PartialRenderType is SET_DAMAGE_AND_DROP_OP，global dirty
 * @tc.type: FUNC
 * @tc.require: I999FR
 */
HWTEST_F(RSSurfaceRenderNodeTest, SubNodeNeedDraw001, TestSize.Level1)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    auto subnode = std::make_shared<RSRenderNode>(id + 1, context);
    if (node == nullptr || subnode == nullptr) {
        return;
    }
    node->AddChild(subnode, 0);
    PartialRenderType partialRenderType = PartialRenderType::SET_DAMAGE_AND_DROP_OP;
    RectI rect = RectI(0, 0, 100, 100);
    subnode->oldDirtyInSurface_ = rect;
    node->visibleRegion_ = Occlusion::Region(rect);
    node->SetGlobalDirtyRegion(rect);
    bool needDraw = node->SubNodeNeedDraw(subnode->GetOldDirtyInSurface(), partialRenderType);
    ASSERT_EQ(needDraw, true);
}

/**
 * @tc.name: SubNodeNeedDraw002
 * @tc.desc: check if subnode need draw when PartialRenderType is SET_DAMAGE_AND_DROP_OP，visible dirty
 * @tc.type: FUNC
 * @tc.require: I999FR
 */
HWTEST_F(RSSurfaceRenderNodeTest, SubNodeNeedDraw002, TestSize.Level1)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    auto subnode = std::make_shared<RSRenderNode>(id + 1, context);
    if (node == nullptr || subnode == nullptr) {
        return;
    }
    node->AddChild(subnode, 0);
    PartialRenderType partialRenderType = PartialRenderType::SET_DAMAGE_AND_DROP_OP;
    RectI rect = RectI(0, 0, 100, 100);
    subnode->oldDirtyInSurface_ = rect;
    node->SetVisibleDirtyRegion(Occlusion::Region(rect));
    bool needDraw = node->SubNodeNeedDraw(subnode->GetOldDirtyInSurface(), partialRenderType);
    ASSERT_EQ(needDraw, true);
}

/**
 * @tc.name: SubNodeNeedDraw003
 * @tc.desc: check if subnode need draw when PartialRenderType is SET_DAMAGE_AND_DROP_OP，transparent
 * @tc.type: FUNC
 * @tc.require: I999FR
 */
HWTEST_F(RSSurfaceRenderNodeTest, SubNodeNeedDraw003, TestSize.Level1)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    auto subnode = std::make_shared<RSRenderNode>(id + 1, context);
    if (node == nullptr || subnode == nullptr) {
        return;
    }
    node->AddChild(subnode, 0);
    PartialRenderType partialRenderType = PartialRenderType::SET_DAMAGE_AND_DROP_OP;
    RectI rect = RectI(0, 0, 100, 100);
    subnode->oldDirtyInSurface_ = rect;
    node->oldDirtyInSurface_ = rect;
    node->SetGlobalAlpha(0.0f);
    Occlusion::Region region = Occlusion::Region(rect);
    node->SetDirtyRegionBelowCurrentLayer(region);
    bool needDraw = node->SubNodeNeedDraw(subnode->GetOldDirtyInSurface(), partialRenderType);
    ASSERT_EQ(needDraw, true);
}

/**
 * @tc.name: SubNodeNeedDraw004
 * @tc.desc: check if subnode need draw when PartialRenderType is SET_DAMAGE_AND_DROP_OP_OCCLUSION
 * @tc.type: FUNC
 * @tc.require: I999FR
 */
HWTEST_F(RSSurfaceRenderNodeTest, SubNodeNeedDraw004, TestSize.Level1)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    auto subnode = std::make_shared<RSRenderNode>(id + 1, context);
    if (node == nullptr || subnode == nullptr) {
        return;
    }
    node->AddChild(subnode, 0);
    PartialRenderType partialRenderType = PartialRenderType::SET_DAMAGE_AND_DROP_OP_OCCLUSION;
    RectI rect = RectI(0, 0, 100, 100);
    subnode->oldDirtyInSurface_ = rect;
    node->visibleRegion_ = Occlusion::Region(rect);
    bool needDraw = node->SubNodeNeedDraw(subnode->GetOldDirtyInSurface(), partialRenderType);
    ASSERT_EQ(needDraw, true);
}

/**
 * @tc.name: SubNodeNeedDraw005
 * @tc.desc: check if subnode need draw when PartialRenderType is SET_DAMAGE_AND_DROP_OP_NOT_VISIBLEDIRTY
 * @tc.type: FUNC
 * @tc.require: I999FR
 */
HWTEST_F(RSSurfaceRenderNodeTest, SubNodeNeedDraw005, TestSize.Level1)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    auto subnode = std::make_shared<RSRenderNode>(id + 1, context);
    if (node == nullptr || subnode == nullptr) {
        return;
    }
    node->AddChild(subnode, 0);
    PartialRenderType partialRenderType = PartialRenderType::SET_DAMAGE_AND_DROP_OP_NOT_VISIBLEDIRTY;
    RectI rect = RectI(0, 0, 100, 100);
    subnode->oldDirtyInSurface_ = rect;
    bool needDraw = node->SubNodeNeedDraw(subnode->GetOldDirtyInSurface(), partialRenderType);
    // Do not have any visible region
    ASSERT_EQ(needDraw, false);
}

/**
 * @tc.name: SubNodeNeedDraw006
 * @tc.desc: check if subnode need draw when PartialRenderType is SET_DAMAGE_AND_DROP_OP_NOT_VISIBLEDIRTY
 * @tc.type: FUNC
 * @tc.require: I999FR
 */
HWTEST_F(RSSurfaceRenderNodeTest, SubNodeNeedDraw006, TestSize.Level1)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    auto subnode = std::make_shared<RSRenderNode>(id + 1, context);
    if (node == nullptr || subnode == nullptr) {
        return;
    }
    node->AddChild(subnode, 0);
    PartialRenderType partialRenderType = PartialRenderType::SET_DAMAGE_AND_DROP_OP_NOT_VISIBLEDIRTY;
    RectI rect = RectI(0, 0, 100, 100);
    subnode->oldDirtyInSurface_ = rect;
    node->oldDirtyInSurface_ = rect;
    Occlusion::Region region = Occlusion::Region(rect);
    node->visibleRegion_ = region;
    node->transparentRegion_ = region;
    node->SetDirtyRegionBelowCurrentLayer(region);
    bool needDraw = node->SubNodeNeedDraw(subnode->GetOldDirtyInSurface(), partialRenderType);
    ASSERT_EQ(needDraw, true);
}

/**
 * @tc.name: SubNodeNeedDraw007
 * @tc.desc: check if subnode need draw when PartialRenderType is DISABLED
 * @tc.type: FUNC
 * @tc.require: I999FR
 */
HWTEST_F(RSSurfaceRenderNodeTest, SubNodeNeedDraw007, TestSize.Level1)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    auto subnode = std::make_shared<RSRenderNode>(id + 1, context);
    if (node == nullptr || subnode == nullptr) {
        return;
    }
    node->AddChild(subnode, 0);
    PartialRenderType partialRenderType = PartialRenderType::DISABLED;
    bool needDraw = node->SubNodeNeedDraw(subnode->GetOldDirtyInSurface(), partialRenderType);
    ASSERT_EQ(needDraw, true);
}

/**
 * @tc.name: SubNodeNeedDraw008
 * @tc.desc: check if subnode need draw when PartialRenderType is SET_DAMAGE
 * @tc.type: FUNC
 * @tc.require: I999FR
 */
HWTEST_F(RSSurfaceRenderNodeTest, SubNodeNeedDraw008, TestSize.Level1)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    auto subnode = std::make_shared<RSRenderNode>(id + 1, context);
    if (node == nullptr || subnode == nullptr) {
        return;
    }
    node->AddChild(subnode, 0);
    PartialRenderType partialRenderType = PartialRenderType::SET_DAMAGE;
    bool needDraw = node->SubNodeNeedDraw(subnode->GetOldDirtyInSurface(), partialRenderType);
    ASSERT_EQ(needDraw, true);
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
    if (node == nullptr || subnode == nullptr) {
        return;
    }
    node->AddChild(subnode, 0);
    subnode->isContentDirty_ = true;
    subnode->isNewOnTree_ = true;
    std::unordered_map<NodeId, std::weak_ptr<RSRenderNode>> activeNodeIds = {{subnode->GetId(), subnode}};
    node->UpdateSurfaceCacheContentStatic(activeNodeIds);
    ASSERT_EQ(node->IsContentDirtyNodeLimited(), false);
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
 * @tc.require: issueI98VTC
 */
HWTEST_F(RSSurfaceRenderNodeTest, QuerySubAssignable002, TestSize.Level2)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    ASSERT_NE(node, nullptr);
    node->SetHasFilter(true);
    
    ASSERT_EQ(node->QuerySubAssignable(false), false);
}

/**
 * @tc.name: QuerySubAssignable003
 * @tc.desc: Test QuerySubAssignable while child has filter but isn't transparent
 * @tc.type: FUNC
 * @tc.require: issueI98VTC
 */
HWTEST_F(RSSurfaceRenderNodeTest, QuerySubAssignable003, TestSize.Level2)
{
    auto parentNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    auto childNode = std::make_shared<RSSurfaceRenderNode>(id + 1, context);
    ASSERT_NE(parentNode, nullptr);
    ASSERT_NE(childNode, nullptr);

    childNode->SetHasFilter(true);
    childNode->SetParent(parentNode);
    const uint8_t opacity = 255;
    parentNode->SetAbilityBGAlpha(opacity);
    parentNode->SetChildHasFilter(true);
    
    ASSERT_EQ(parentNode->QuerySubAssignable(false), true);
}

/**
 * @tc.name: QuerySubAssignable004
 * @tc.desc: Test QuerySubAssignable while child has filter and it's transparent
 * @tc.type: FUNC
 * @tc.require: issueI98VTC
 */
HWTEST_F(RSSurfaceRenderNodeTest, QuerySubAssignable004, TestSize.Level2)
{
    auto parentNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    auto childNode = std::make_shared<RSSurfaceRenderNode>(id + 1, context);
    ASSERT_NE(parentNode, nullptr);
    ASSERT_NE(childNode, nullptr);

    childNode->SetHasFilter(true);
    childNode->SetParent(parentNode);
    parentNode->SetChildHasFilter(true);
    
    ASSERT_EQ(parentNode->QuerySubAssignable(false), false);
}
} // namespace Rosen
} // namespace OHOS
