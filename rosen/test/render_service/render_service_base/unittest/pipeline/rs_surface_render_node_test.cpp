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

#include "pipeline/rs_context.h"
#include "params/rs_surface_render_params.h"
#include "pipeline/rs_render_thread_visitor.h"
#include "pipeline/rs_effect_render_node.h"
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
    canvas_ = nullptr;
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
    ASSERT_TRUE(node->GetSkipLayer());
}

/**
 * @tc.name: SetSkipLayer002
 * @tc.desc: Test SetSkipLayer for surface node which is skip layer
    and has leash window node as instant parent
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

    rsContext->GetMutableNodeMap().renderNodeMap_[parentNode->GetId()] = parentNode;
    rsContext->GetMutableNodeMap().renderNodeMap_[skipLayerNode->GetId()] = skipLayerNode;

    parentNode->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    parentNode->AddChild(skipLayerNode);
    parentNode->SetIsOnTheTree(true);
    skipLayerNode->SetSkipLayer(true);

    ASSERT_TRUE(parentNode->GetSkipLayer() && parentNode->GetHasSkipLayer());
}

/**
 * @tc.name: SetSkipLayer003
 * @tc.desc: Test SetSkipLayer for surface node which is skip layer
        and don't have leash window node as instant parent
 * @tc.type: FUNC
 * @tc.require: issueI9ABGS
 */
HWTEST_F(RSSurfaceRenderNodeTest, SetSkipLayer003, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto ancestorNode = std::make_shared<RSSurfaceRenderNode>(id, rsContext);
    auto parentNode = std::make_shared<RSSurfaceRenderNode>(id + 1, rsContext);
    auto skipLayerNode = std::make_shared<RSSurfaceRenderNode>(id + 2, rsContext);
    ASSERT_NE(ancestorNode, nullptr);
    ASSERT_NE(parentNode, nullptr);
    ASSERT_NE(skipLayerNode, nullptr);

    rsContext->GetMutableNodeMap().renderNodeMap_[ancestorNode->GetId()] = ancestorNode;
    rsContext->GetMutableNodeMap().renderNodeMap_[parentNode->GetId()] = parentNode;
    rsContext->GetMutableNodeMap().renderNodeMap_[skipLayerNode->GetId()] = skipLayerNode;

    ancestorNode->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    ancestorNode->AddChild(parentNode);
    parentNode->AddChild(skipLayerNode);
    ancestorNode->SetIsOnTheTree(true);
    skipLayerNode->SetSkipLayer(true);

    ASSERT_TRUE(!ancestorNode->GetSkipLayer() && ancestorNode->GetHasSkipLayer());
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
    ASSERT_TRUE(node->GetSecurityLayer());
}

/**
 * @tc.name: SetSecurityLayer002
 * @tc.desc: Test SetSecurityLayer for surface node which is security layer
    and has leash window node as instant parent
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

    rsContext->GetMutableNodeMap().renderNodeMap_[parentNode->GetId()] = parentNode;
    rsContext->GetMutableNodeMap().renderNodeMap_[securityLayerNode->GetId()] = securityLayerNode;

    parentNode->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    parentNode->AddChild(securityLayerNode);
    parentNode->SetIsOnTheTree(true);
    securityLayerNode->SetSecurityLayer(true);

    ASSERT_TRUE(parentNode->GetSecurityLayer() && parentNode->GetHasSecurityLayer());
}

/**
 * @tc.name: SetSecurityLayer003
 * @tc.desc: Test SetSecurityLayer for surface node which is security layer
        and don't have leash window node as instant parent
 * @tc.type: FUNC
 * @tc.require: issueI9ABGS
 */
HWTEST_F(RSSurfaceRenderNodeTest, SetSecurityLayer003, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto ancestorNode = std::make_shared<RSSurfaceRenderNode>(id, rsContext);
    auto parentNode = std::make_shared<RSSurfaceRenderNode>(id + 1, rsContext);
    auto securityLayerNode = std::make_shared<RSSurfaceRenderNode>(id + 2, rsContext);
    ASSERT_NE(ancestorNode, nullptr);
    ASSERT_NE(parentNode, nullptr);
    ASSERT_NE(securityLayerNode, nullptr);

    rsContext->GetMutableNodeMap().renderNodeMap_[ancestorNode->GetId()] = ancestorNode;
    rsContext->GetMutableNodeMap().renderNodeMap_[parentNode->GetId()] = parentNode;
    rsContext->GetMutableNodeMap().renderNodeMap_[securityLayerNode->GetId()] = securityLayerNode;

    ancestorNode->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    ancestorNode->AddChild(parentNode);
    parentNode->AddChild(securityLayerNode);
    ancestorNode->SetIsOnTheTree(true);
    securityLayerNode->SetSecurityLayer(true);

    ASSERT_TRUE(!ancestorNode->GetSecurityLayer() && ancestorNode->GetHasSecurityLayer());
}

/**
 * @tc.name: StoreMustRenewedInfo001
 * @tc.desc: Test StoreMustRenewedInfo while has filter
 * @tc.type: FUNC
 * @tc.require: issueI9ABGS
 */
HWTEST_F(RSSurfaceRenderNodeTest, StoreMustRenewedInfo001, TestSize.Level2)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    ASSERT_NE(node, nullptr);

    node->SetHasFilter(true);
    node->RSRenderNode::StoreMustRenewedInfo();
    node->StoreMustRenewedInfo();
    ASSERT_FALSE(node->HasMustRenewedInfo());
}

/**
 * @tc.name: StoreMustRenewedInfo002
 * @tc.desc: Test StoreMustRenewedInfo while has effect node
 * @tc.type: FUNC
 * @tc.require: issueI9ABGS
 */
HWTEST_F(RSSurfaceRenderNodeTest, StoreMustRenewedInfo002, TestSize.Level2)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    ASSERT_NE(node, nullptr);

    node->stagingRenderParams_ = std::make_unique<RSRenderParams>(id);
    node->SetChildHasVisibleEffect(true);
    node->RSRenderNode::StoreMustRenewedInfo();
    node->StoreMustRenewedInfo();
    ASSERT_TRUE(node->HasMustRenewedInfo());
}

/**
 * @tc.name: StoreMustRenewedInfo003
 * @tc.desc: Test StoreMustRenewedInfo while has hardware node
 * @tc.type: FUNC
 * @tc.require: issueI9ABGS
 */
HWTEST_F(RSSurfaceRenderNodeTest, StoreMustRenewedInfo003, TestSize.Level2)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    ASSERT_NE(node, nullptr);

    node->SetHasHardwareNode(true);
    node->RSRenderNode::StoreMustRenewedInfo();
    node->StoreMustRenewedInfo();
    ASSERT_TRUE(node->HasMustRenewedInfo());
}

/**
 * @tc.name: StoreMustRenewedInfo004
 * @tc.desc: Test StoreMustRenewedInfo while is skip layer
 * @tc.type: FUNC
 * @tc.require: issueI9ABGS
 */
HWTEST_F(RSSurfaceRenderNodeTest, StoreMustRenewedInfo004, TestSize.Level2)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    ASSERT_NE(node, nullptr);

    node->SetSkipLayer(true);
    node->RSRenderNode::StoreMustRenewedInfo();
    node->StoreMustRenewedInfo();
    ASSERT_TRUE(node->HasMustRenewedInfo());
}

/**
 * @tc.name: StoreMustRenewedInfo005
 * @tc.desc: Test StoreMustRenewedInfo while is security layer
 * @tc.type: FUNC
 * @tc.require: issueI9ABGS
 */
HWTEST_F(RSSurfaceRenderNodeTest, StoreMustRenewedInfo005, TestSize.Level2)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    ASSERT_NE(node, nullptr);

    node->SetSecurityLayer(true);
    node->RSRenderNode::StoreMustRenewedInfo();
    node->StoreMustRenewedInfo();
    ASSERT_TRUE(node->HasMustRenewedInfo());
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

    rsContext->GetMutableNodeMap().renderNodeMap_[node->GetId()] = node;
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

    rsContext->GetMutableNodeMap().renderNodeMap_[childNode->GetId()] = childNode;
    rsContext->GetMutableNodeMap().renderNodeMap_[parentNode->GetId()] = parentNode;

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
 * @tc.name: SetForceHardwareAndFixRotation001
 * @tc.desc: Test SetForceHardwareAndFixRotation true
 * @tc.type: FUNC
 * @tc.require: issueI9HWLB
 */
HWTEST_F(RSSurfaceRenderNodeTest, SetForceHardwareAndFixRotation001, TestSize.Level2)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    ASSERT_NE(node, nullptr);

    node->SetForceHardwareAndFixRotation(true);
    ASSERT_EQ(node->isForceHardwareByUser_, true);
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

    node->SetForceHardwareAndFixRotation(false);
    ASSERT_EQ(node->isForceHardwareByUser_, false);
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
 * @tc.name: UpdateHwcDisabledBySrcRectTest
 * @tc.desc: test results of UpdateHwcDisabledBySrcRect
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, UpdateHwcDisabledBySrcRectTest, TestSize.Level1)
{
    bool hasRotation = false;
    auto buffer = SurfaceBuffer::Create();
    auto renderNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    renderNode->UpdateHwcDisabledBySrcRect(hasRotation);

    renderNode->buffer_.buffer = buffer;
    renderNode->UpdateHwcDisabledBySrcRect(hasRotation);
    ASSERT_FALSE(renderNode->isHardwareForcedDisabledBySrcRect_);
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
    testNode->buffer_.buffer = buffer;
    EXPECT_NE(testNode->GetBuffer(), nullptr);
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
    ASSERT_FALSE(testNode->isSubSurfaceEnabled_);
}

/**
 * @tc.name: CollectSurfaceForUIFirstSwitchTest
 * @tc.desc: test results of CollectSurfaceForUIFirstSwitchTest
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, CollectSurfaceForUIFirstSwitchTest, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    uint32_t leashWindowCount = 0;
    uint32_t minNodeNum = 5;
    testNode->CollectSurfaceForUIFirstSwitch(leashWindowCount, minNodeNum);
    ASSERT_EQ(leashWindowCount, 0);
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
    EXPECT_TRUE(node->isSkipLayer_);
    node->SetSkipLayer(false);
    EXPECT_FALSE(node->isSkipLayer_);
}

/**
 * @tc.name: SyncSecurityInfoToFirstLevelNodeTest
 * @tc.desc: test results of SyncSecurityInfoToFirstLevelNode
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, SyncSecurityInfoToFirstLevelNodeTest, TestSize.Level1)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    node->SyncSecurityInfoToFirstLevelNode();
    EXPECT_FALSE(node->isSkipLayer_);
}

/**
 * @tc.name: SyncSkipInfoToFirstLevelNode
 * @tc.desc: test results of SyncSkipInfoToFirstLevelNode
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, SyncSkipInfoToFirstLevelNode, TestSize.Level1)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    node->SyncSkipInfoToFirstLevelNode();
    EXPECT_FALSE(node->isSkipLayer_);
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
    node->consumer_ = IConsumerSurface::Create();
    node->UpdateSurfaceDefaultSize(1920.0f, 1080.0f);
    ASSERT_NE(node->consumer_, nullptr);
}

/**
 * @tc.name: NeedClearBufferCache
 * @tc.desc: test results of NeedClearBufferCache
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, NeedClearBufferCache, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    testNode->stagingRenderParams_ = std::make_unique<RSRenderParams>(id);
    testNode->addedToPendingSyncList_ = true;
    testNode->NeedClearBufferCache();
    EXPECT_FALSE(testNode->isSkipLayer_);
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
    EXPECT_FALSE(testNode->isSkipLayer_);
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
    EXPECT_FALSE(testNode->isSkipLayer_);
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
    testNode->isCurrentFrameBufferConsumed_ = true;
    bool resultOne = testNode->UpdateDirtyIfFrameBufferConsumed();
    EXPECT_TRUE(resultOne);

    testNode->isCurrentFrameBufferConsumed_ = false;
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
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, GetVisibleLevelForWMS, TestSize.Level1)
{
    RSSurfaceRenderNode node(id);

    EXPECT_EQ(node.GetVisibleLevelForWMS(RSVisibleLevel::RS_INVISIBLE), WINDOW_LAYER_INFO_TYPE::SEMI_VISIBLE);

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
        region, visibleVec, visMapForVsyncRate, needSetVisibleRegion, visibleLevel, isSystemAnimatedScenes);
    ASSERT_TRUE(node->visibleRegionForCallBack_.IsEmpty());
}

/**
 * @tc.name: SubNodeIntersectWithExtraDirtyRegion
 * @tc.desc: test results of SubNodeIntersectWithExtraDirtyRegion
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, SubNodeIntersectWithExtraDirtyRegion, TestSize.Level1)
{
    RectI testRect { 0, 0, 10, 10 };
    auto mockNode = std::make_shared<RSSurfaceRenderNode>(id);
    mockNode->isDirtyRegionAlignedEnable_ = false;
    EXPECT_FALSE(mockNode->SubNodeIntersectWithExtraDirtyRegion(testRect));

    mockNode->isDirtyRegionAlignedEnable_ = true;
    mockNode->extraDirtyRegionAfterAlignmentIsEmpty_ = true;
    EXPECT_FALSE(mockNode->SubNodeIntersectWithExtraDirtyRegion(testRect));

    mockNode->extraDirtyRegionAfterAlignment_ = Occlusion::Region();
    mockNode->extraDirtyRegionAfterAlignmentIsEmpty_ = false;
    EXPECT_FALSE(mockNode->SubNodeIntersectWithExtraDirtyRegion(testRect));
}

/**
 * @tc.name: SubNodeIntersectWithDirty
 * @tc.desc: test results of SubNodeIntersectWithDirty
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, SubNodeIntersectWithDirty, TestSize.Level1)
{
    RectI testRect { 0, 0, 10, 10 };
    auto mockNode = std::make_shared<RSSurfaceRenderNode>(id);

    mockNode->globalDirtyRegionIsEmpty_ = true;
    Occlusion::Rect rect;
    mockNode->visibleDirtyRegion_ = Occlusion::Region(rect);
    EXPECT_FALSE(mockNode->SubNodeIntersectWithDirty(testRect));
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
 * @tc.name: CalcFilterCacheValidForOcclusion
 * @tc.desc: test results of CalcFilterCacheValidForOcclusion
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, UpdateFilterNodesTest, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    std::shared_ptr<RSRenderNode> renderNode;
    node->UpdateFilterNodes(renderNode);

    renderNode = std::make_shared<RSRenderNode>(id + 1);
    node->UpdateFilterNodes(renderNode);
    EXPECT_EQ(node->filterNodes_.size(), 1);
}

/**
 * @tc.name: CalcFilterCacheValidForOcclusion
 * @tc.desc: test results of CalcFilterCacheValidForOcclusion
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, CheckValidFilterCacheFullyCoverTargetTest, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    RSRenderNode filterNode(id + 1);
    RectI targetRect;
    node->CheckValidFilterCacheFullyCoverTarget(filterNode, targetRect);
    EXPECT_FALSE(node->isFilterCacheStatusChanged_);
}

/**
 * @tc.name: UpdateSurfaceCacheContentStaticFlag
 * @tc.desc: test results of UpdateSurfaceCacheContentStaticFlag
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, UpdateSurfaceCacheContentStaticFlag, TestSize.Level1)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id);
    node->stagingRenderParams_ = std::make_unique<RSRenderParams>(id);
    node->addedToPendingSyncList_ = true;
    node->UpdateSurfaceCacheContentStaticFlag();

    node->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    node->UpdateSurfaceCacheContentStaticFlag();
    EXPECT_EQ(node->nodeType_, RSSurfaceNodeType::LEASH_WINDOW_NODE);
}

/**
 * @tc.name: UpdateSurfaceSubTreeDirtyFlag
 * @tc.desc: test results of UpdateSurfaceSubTreeDirtyFlag
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, UpdateSurfaceSubTreeDirtyFlag, TestSize.Level1)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id);
    node->stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(id + 1);
    node->addedToPendingSyncList_ = true;
    node->UpdateSurfaceSubTreeDirtyFlag();
    ASSERT_NE(node->stagingRenderParams_, nullptr);
}

/**
 * @tc.name: UpdateDrawingCacheNodes
 * @tc.desc: test results of UpdateDrawingCacheNodes
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, UpdateDrawingCacheNodes, TestSize.Level1)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id);
    auto renderNode = std::make_shared<RSRenderNode>(id + 1);
    node->UpdateDrawingCacheNodes(renderNode);
    node->UpdateDrawingCacheNodes(nullptr);
    EXPECT_EQ(node->drawingCacheNodes_.size(), 1);
}

/**
 * @tc.name: ResetDrawingCacheStatusIfNodeStatic
 * @tc.desc: test results of ResetDrawingCacheStatusIfNodeStatic
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, ResetDrawingCacheStatusIfNodeStatic, TestSize.Level1)
{
    auto renderNode = std::make_shared<RSSurfaceRenderNode>(id);
    std::unordered_map<NodeId, std::unordered_set<NodeId>> allRects;
    renderNode->ResetDrawingCacheStatusIfNodeStatic(allRects);
    EXPECT_EQ(renderNode->drawingCacheNodes_.size(), 0);
}

/**
 * @tc.name: UpdateFilterCacheStatusWithVisible
 * @tc.desc: test results of UpdateFilterCacheStatusWithVisible
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, UpdateFilterCacheStatusWithVisible, TestSize.Level1)
{
    auto renderNode = std::make_shared<RSSurfaceRenderNode>(id);
    bool visible = true;
    renderNode->UpdateFilterCacheStatusWithVisible(visible);
    renderNode->UpdateFilterCacheStatusWithVisible(visible);
    ASSERT_TRUE(renderNode->prevVisible_);
}

/**
 * @tc.name: UpdateFilterCacheStatusIfNodeStatic
 * @tc.desc: test results of UpdateFilterCacheStatusIfNodeStatic
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, UpdateFilterCacheStatusIfNodeStatic, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    node->UpdateFilterCacheStatusIfNodeStatic(RectI(0, 0, 100, 100), true);

    std::shared_ptr<RSRenderNode> mockNode = std::make_shared<RSEffectRenderNode>(id);
    node->filterNodes_.emplace_back(mockNode);
    node->UpdateFilterCacheStatusIfNodeStatic(RectI(0, 0, 100, 100), false);
    ASSERT_NE(node->filterNodes_.size(), 0);
}

/**
 * @tc.name: ResetOpaqueRegion
 * @tc.desc: test results of ResetOpaqueRegion
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, ResetOpaqueRegion, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> testNode = std::make_shared<RSSurfaceRenderNode>(id);
    RectI absRect { 0, 0, 100, 100 };
    ScreenRotation screenRotation = ScreenRotation::ROTATION_0;
    bool isFocusWindow = true;
    Occlusion::Region res= testNode->ResetOpaqueRegion(absRect, screenRotation, isFocusWindow);
    EXPECT_NE(res.rects_.size(), 0);
    isFocusWindow = false;
    res =testNode->ResetOpaqueRegion(absRect, screenRotation, isFocusWindow);
    EXPECT_NE(res.rects_.size(), 0);
}

/**
 * @tc.name: SetUnfocusedWindowOpaqueRegion
 * @tc.desc: test results of SetUnfocusedWindowOpaqueRegion
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, SetUnfocusedWindowOpaqueRegion, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> surfaceNode = std::make_shared<RSSurfaceRenderNode>(id);
    RectI absRect { 0, 0, 100, 100 };
    ScreenRotation rotationCases[] = { ScreenRotation::ROTATION_0, ScreenRotation::ROTATION_90,
        ScreenRotation::ROTATION_180, ScreenRotation::ROTATION_270, ScreenRotation::INVALID_SCREEN_ROTATION };
    for (ScreenRotation rotation : rotationCases) {
        Occlusion::Region opaqueRegion = surfaceNode->SetUnfocusedWindowOpaqueRegion(absRect, rotation);
        EXPECT_NE(opaqueRegion.rects_.size(), 0);
    }
}

/**
 * @tc.name: SetFocusedWindowOpaqueRegion
 * @tc.desc: test results of SetFocusedWindowOpaqueRegion
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, SetFocusedWindowOpaqueRegion, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> renderNode = std::make_shared<RSSurfaceRenderNode>(id);
    RectI absRect { 0, 0, 100, 100 };
    ScreenRotation rotationCases[] = { ScreenRotation::ROTATION_0, ScreenRotation::ROTATION_90,
    ScreenRotation::ROTATION_180, ScreenRotation::ROTATION_270, ScreenRotation::INVALID_SCREEN_ROTATION };
    for (ScreenRotation rotation : rotationCases) {
        Occlusion::Region opaqueRegion = renderNode->SetFocusedWindowOpaqueRegion(absRect, rotation);
        EXPECT_NE(opaqueRegion.rects_.size(), 0);
    }
}

/**
 * @tc.name: ResetSurfaceContainerRegion
 * @tc.desc: test results of ResetSurfaceContainerRegion
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, ResetSurfaceContainerRegion, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> renderNode = std::make_shared<RSSurfaceRenderNode>(id);
    RectI screeninfo { 0, 0, 100, 100 };
    RectI absRect { 0, 0, 50, 50 };
    ScreenRotation rotationCases[] = { ScreenRotation::ROTATION_0, ScreenRotation::ROTATION_90,
    ScreenRotation::ROTATION_180, ScreenRotation::ROTATION_270, ScreenRotation::INVALID_SCREEN_ROTATION };
    renderNode->ResetSurfaceContainerRegion(screeninfo, absRect, rotationCases[0]);
    renderNode->containerConfig_.hasContainerWindow_ = true;
    for (ScreenRotation rotation : rotationCases) {
        renderNode->ResetSurfaceContainerRegion(screeninfo, absRect, rotation);
        EXPECT_FALSE(renderNode->containerRegion_.IsEmpty());
    }
}
} // namespace Rosen
} // namespace OHOS
