/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include <parameters.h>

#include "surface_buffer_impl.h"

#include "params/rs_surface_render_params.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_effect_render_node.h"
#include "pipeline/rs_render_thread_visitor.h"
#include "pipeline/rs_surface_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSSurfaceRenderNodeTwoTest : public testing::Test {
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
};

void RSSurfaceRenderNodeTwoTest::SetUpTestCase()
{
    canvas_ = new RSPaintFilterCanvas(&drawingCanvas_);
}
void RSSurfaceRenderNodeTwoTest::TearDownTestCase()
{
    delete canvas_;
    canvas_ = nullptr;
}
void RSSurfaceRenderNodeTwoTest::SetUp() {}
void RSSurfaceRenderNodeTwoTest::TearDown() {}

/**
 * @tc.name: ResetSurfaceOpaqueRegion03
 * @tc.desc: function test
 * @tc.type:FUNC
 * @tc.require: issueIA4VTS
 */
HWTEST_F(RSSurfaceRenderNodeTwoTest, ResetSurfaceOpaqueRegion03, TestSize.Level1)
{
    RSSurfaceRenderNode surfaceRenderNode(id, context);
    RectI screenRect { 0, 0, 2560, 1600 };
    RectI absRect { 0, 100, 400, 500 };
    surfaceRenderNode.SetAbilityBGAlpha(255);
    surfaceRenderNode.SetGlobalAlpha(1.0f);
    surfaceRenderNode.SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    surfaceRenderNode.parent_ = std::make_shared<RSRenderNode>(1);
    surfaceRenderNode.nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    Vector4f cornerRadius;
    Vector4f::Max(surfaceRenderNode.GetWindowCornerRadius(), surfaceRenderNode.GetGlobalCornerRadius(), cornerRadius);
    Vector4<int> dstCornerRadius(static_cast<int>(std::ceil(cornerRadius.x_)),
        static_cast<int>(std::ceil(cornerRadius.y_)), static_cast<int>(std::ceil(cornerRadius.z_)),
        static_cast<int>(std::ceil(cornerRadius.w_)));
    surfaceRenderNode.ResetSurfaceOpaqueRegion(screenRect, absRect, ScreenRotation::ROTATION_0, false, dstCornerRadius);
    Vector4<int> dstCornerRadiusT(static_cast<int>(std::ceil(1)), static_cast<int>(std::ceil(1)),
        static_cast<int>(std::ceil(1)), static_cast<int>(std::ceil(1)));
    surfaceRenderNode.ResetSurfaceOpaqueRegion(
        screenRect, absRect, ScreenRotation::ROTATION_0, false, dstCornerRadiusT);
    surfaceRenderNode.SetContainerWindow(true, 1.0f);
    surfaceRenderNode.ResetSurfaceOpaqueRegion(screenRect, absRect, ScreenRotation::ROTATION_0, false, dstCornerRadius);
    surfaceRenderNode.ResetSurfaceOpaqueRegion(screenRect, absRect, ScreenRotation::ROTATION_0, true, dstCornerRadius);
}

/**
 * @tc.name: CollectSurfaceTest
 * @tc.desc: function test
 * @tc.type: FUNC
 * @tc.require: issueIA4VTS
 */
HWTEST_F(RSSurfaceRenderNodeTwoTest, CollectSurfaceTest003, TestSize.Level1)
{
    auto renderNode = std::make_shared<RSSurfaceRenderNode>(0);
    auto rsBaseRenderNode = std::make_shared<RSBaseRenderNode>(0);
    std::vector<RSBaseRenderNode::SharedPtr> vec;
    renderNode->nodeType_ = RSSurfaceNodeType::SCB_SCREEN_NODE;
    renderNode->CollectSurface(rsBaseRenderNode, vec, true, true);
    EXPECT_TRUE(renderNode->IsScbScreen());
    renderNode->nodeType_ = RSSurfaceNodeType::STARTING_WINDOW_NODE;
    renderNode->CollectSurface(rsBaseRenderNode, vec, true, true);
    EXPECT_TRUE(renderNode->IsStartingWindow());
    renderNode->CollectSurface(rsBaseRenderNode, vec, false, true);
    EXPECT_TRUE(renderNode->IsStartingWindow());
    renderNode->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    renderNode->CollectSurface(rsBaseRenderNode, vec, true, true);
    EXPECT_TRUE(renderNode->IsLeashWindow());
    renderNode->CollectSurface(rsBaseRenderNode, vec, false, false);
    EXPECT_TRUE(renderNode->IsLeashWindow());
    renderNode->nodeType_ = RSSurfaceNodeType::DEFAULT;
    renderNode->CollectSurface(rsBaseRenderNode, vec, true, true);
    EXPECT_FALSE(renderNode->IsLeashWindow());
    renderNode->CollectSurface(rsBaseRenderNode, vec, false, true);
    EXPECT_FALSE(renderNode->IsLeashWindow());
    auto surfaceBufferImpl = new SurfaceBufferImpl();
    renderNode->GetRSSurfaceHandler()->buffer_.buffer = surfaceBufferImpl;
    renderNode->CollectSurface(rsBaseRenderNode, vec, false, true);
    EXPECT_FALSE(renderNode->IsLeashWindow());
    renderNode->shouldPaint_ = false;
    renderNode->CollectSurface(rsBaseRenderNode, vec, false, true);
    EXPECT_FALSE(renderNode->IsLeashWindow());
}

/**
 * @tc.name: SyncSecurityInfoToFirstLevelNodeTest
 * @tc.desc: function test
 * @tc.type:FUNC
 * @tc.require: issueIA4VTS
 */
HWTEST_F(RSSurfaceRenderNodeTwoTest, SyncSecurityInfoToFirstLevelNode001, TestSize.Level1)
{
    auto renderNode = std::make_shared<RSSurfaceRenderNode>(0);
    renderNode->SetSecurityLayer(true);
    renderNode->SyncSecurityInfoToFirstLevelNode();
    EXPECT_TRUE(renderNode->securityLayerIds_.size() > 0);
    EXPECT_TRUE(renderNode->isSecurityLayer_);
    renderNode->SetSecurityLayer(false);
    renderNode->SyncSecurityInfoToFirstLevelNode();
    EXPECT_TRUE(!renderNode->isSecurityLayer_);
    EXPECT_TRUE(renderNode->securityLayerIds_.size() == 0);
}

/**
 * @tc.name: SyncSkipInfoToFirstLevelNodeTest
 * @tc.desc: function test
 * @tc.type:FUNC
 * @tc.require: issueIA4VTS
 */
HWTEST_F(RSSurfaceRenderNodeTwoTest, SyncSkipInfoToFirstLevelNode001, TestSize.Level1)
{
    auto renderNode = std::make_shared<RSSurfaceRenderNode>(1);
    renderNode->SetProtectedLayer(true);
    renderNode->SyncSkipInfoToFirstLevelNode();
    EXPECT_TRUE(renderNode->GetId() != 0);
    EXPECT_TRUE(renderNode->GetFirstLevelNodeId() == 0);
    renderNode->SetProtectedLayer(false);
    renderNode->SyncSkipInfoToFirstLevelNode();
    auto renderNodeSecond = std::make_shared<RSSurfaceRenderNode>(0);
    EXPECT_TRUE(renderNodeSecond->GetId() == 0);
    EXPECT_TRUE(renderNodeSecond->GetFirstLevelNodeId() == 0);
    renderNodeSecond->SyncSkipInfoToFirstLevelNode();
    EXPECT_TRUE(renderNodeSecond->skipLayerIds_.size() == 0);
}

/**
 * @tc.name: SyncSnapshotSkipInfoToFirstLevelNodeTest
 * @tc.desc: function test
 * @tc.type:FUNC
 * @tc.require: issueIA4VTS
 */
HWTEST_F(RSSurfaceRenderNodeTwoTest, SyncSnapshotSkipInfoToFirstLevelNode001, TestSize.Level1)
{
    auto renderNode = std::make_shared<RSSurfaceRenderNode>(1);
    renderNode->SetProtectedLayer(true);
    renderNode->SyncSnapshotSkipInfoToFirstLevelNode();
    EXPECT_TRUE(renderNode->GetId() != 0);
    EXPECT_TRUE(renderNode->GetFirstLevelNodeId() == 0);
    renderNode->SetProtectedLayer(false);
    renderNode->SyncSnapshotSkipInfoToFirstLevelNode();
    auto renderNodeSecond = std::make_shared<RSSurfaceRenderNode>(0);
    EXPECT_TRUE(renderNodeSecond->GetId() == 0);
    EXPECT_TRUE(renderNodeSecond->GetFirstLevelNodeId() == 0);
    renderNodeSecond->SyncSnapshotSkipInfoToFirstLevelNode();
    EXPECT_TRUE(renderNodeSecond->snapshotSkipLayerIds_.size() == 0);
}

/**
 * @tc.name: SyncProtectedInfoToFirstLevelNodeTest
 * @tc.desc: function test
 * @tc.type:FUNC
 * @tc.require: issueIA4VTS
 */
HWTEST_F(RSSurfaceRenderNodeTwoTest, SyncProtectedInfoToFirstLevelNode001, TestSize.Level1)
{
    auto renderNode = std::make_shared<RSSurfaceRenderNode>(1);
    renderNode->SetProtectedLayer(true);
    EXPECT_TRUE(renderNode->GetProtectedLayer());
    EXPECT_TRUE(renderNode->GetId() == 1);
    EXPECT_TRUE(renderNode->GetFirstLevelNodeId() == 0);
    renderNode->SyncProtectedInfoToFirstLevelNode();
}

/**
 * @tc.name: UpdateSrcRectTest
 * @tc.desc: test results of UpdateSrcRect
 * @tc.type: FUNC
 * @tc.require: issueIA4VTS
 */
HWTEST_F(RSSurfaceRenderNodeTwoTest, UpdateSrcRectTest, TestSize.Level1)
{
    Drawing::Canvas canvas;
    Drawing::RectI dstRect(0, 0, 100, 100);
    auto renderNode = std::make_shared<RSSurfaceRenderNode>(0);
    renderNode->UpdateSrcRect(canvas, dstRect, true);
    EXPECT_FALSE(renderNode->IsYUVBufferFormat());
    auto surfaceBufferImpl = new SurfaceBufferImpl();
    auto handle = new BufferHandle();
    handle->format = 22;
    surfaceBufferImpl->handle_ = handle;
    renderNode->GetRSSurfaceHandler()->buffer_.buffer = surfaceBufferImpl;
    renderNode->UpdateSrcRect(canvas, dstRect, true);
    EXPECT_TRUE(renderNode->IsYUVBufferFormat());
    if (renderNode->GetRSSurfaceHandler()->buffer_.buffer) {
        delete renderNode->GetRSSurfaceHandler()->buffer_.buffer;
    }
}

/**
 * @tc.name: UpdateHwcDisabledBySrcRectTest
 * @tc.desc: test results of UpdateHwcDisabledBySrcRect
 * @tc.type: FUNC
 * @tc.require: issueIA4VTS
 */
HWTEST_F(RSSurfaceRenderNodeTwoTest, UpdateHwcDisabledBySrcRectTest, TestSize.Level1)
{
    auto renderNode = std::make_shared<RSSurfaceRenderNode>(0);
    renderNode->UpdateHwcDisabledBySrcRect(true);
    EXPECT_FALSE(renderNode->IsYUVBufferFormat());
    auto surfaceBufferImpl = new SurfaceBufferImpl();
    renderNode->GetRSSurfaceHandler()->buffer_.buffer = surfaceBufferImpl;
    renderNode->UpdateHwcDisabledBySrcRect(true);
    EXPECT_FALSE(renderNode->IsYUVBufferFormat());
    auto handle = new BufferHandle();
    handle->format = 22;
    surfaceBufferImpl->handle_ = handle;
    renderNode->UpdateHwcDisabledBySrcRect(true);
    EXPECT_TRUE(renderNode->IsYUVBufferFormat());
    if (renderNode->GetRSSurfaceHandler()->buffer_.buffer) {
        delete renderNode->GetRSSurfaceHandler()->buffer_.buffer;
    }
}

/**
 * @tc.name: IsYUVBufferFormatTest
 * @tc.desc: test results of IsYUVBufferFormat
 * @tc.type: FUNC
 * @tc.require: issueIA4VTS
 */
HWTEST_F(RSSurfaceRenderNodeTwoTest, IsYUVBufferFormatTest, TestSize.Level1)
{
    auto renderNode = std::make_shared<RSSurfaceRenderNode>(0);
    EXPECT_FALSE(renderNode->IsYUVBufferFormat());
    auto surfaceBufferImpl = new SurfaceBufferImpl();
    renderNode->GetRSSurfaceHandler()->buffer_.buffer = surfaceBufferImpl;
    EXPECT_FALSE(renderNode->IsYUVBufferFormat());
    auto handle = new BufferHandle();
    handle->format = 20;
    surfaceBufferImpl->handle_ = handle;
    EXPECT_FALSE(renderNode->IsYUVBufferFormat());
    handle->format = 34;
    EXPECT_FALSE(renderNode->IsYUVBufferFormat());
    handle->format = 37;
    EXPECT_FALSE(renderNode->IsYUVBufferFormat());
    handle->format = 22;
    EXPECT_TRUE(renderNode->IsYUVBufferFormat());
    if (renderNode->GetRSSurfaceHandler()->buffer_.buffer) {
        delete renderNode->GetRSSurfaceHandler()->buffer_.buffer;
    }
}

/**
 * @tc.name: CollectSurfaceForUIFirstSwitchTest
 * @tc.desc: test results of CollectSurfaceForUIFirstSwitchTest
 * @tc.type: FUNC
 * @tc.require: issueIA4VTS
 */
HWTEST_F(RSSurfaceRenderNodeTwoTest, CollectSurfaceForUIFirstSwitchTest, TestSize.Level1)
{
    auto renderNode = std::make_shared<RSSurfaceRenderNode>(0);
    uint32_t leashWindowCount = 0;
    renderNode->nodeType_ = RSSurfaceNodeType::DEFAULT;
    renderNode->CollectSurfaceForUIFirstSwitch(leashWindowCount, 0);
    EXPECT_FALSE(renderNode->IsLeashWindow());
    EXPECT_FALSE(renderNode->IsStartingWindow());
    renderNode->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    renderNode->CollectSurfaceForUIFirstSwitch(leashWindowCount, 0);
    EXPECT_TRUE(renderNode->IsLeashWindow());
    renderNode->nodeType_ = RSSurfaceNodeType::STARTING_WINDOW_NODE;
    renderNode->CollectSurfaceForUIFirstSwitch(leashWindowCount, 0);
    EXPECT_TRUE(renderNode->IsStartingWindow());
}

/**
 * @tc.name: ClearChildrenCache
 * @tc.desc: test results of ClearChildrenCache
 * @tc.type: FUNC
 * @tc.require: issueIA4VTS
 */
HWTEST_F(RSSurfaceRenderNodeTwoTest, ClearChildrenCache, TestSize.Level1)
{
    auto renderNode = std::make_shared<RSSurfaceRenderNode>(0);
    std::shared_ptr<RSRenderNode> node = std::make_shared<RSRenderNode>(0);
    std::vector<std::shared_ptr<RSRenderNode>> children;
    children.push_back(node);
    renderNode->fullChildrenList_ = std::make_shared<std::vector<std::shared_ptr<RSRenderNode>>>(children);
    renderNode->ClearChildrenCache();
    EXPECT_TRUE(!children.empty());
    std::shared_ptr<RSSurfaceRenderNode> surfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(0);
    children.push_back(surfaceRenderNode);
    renderNode->ClearChildrenCache();
    EXPECT_TRUE(!children.empty());
}

/**
 * @tc.name: OnTreeStateChangedTest
 * @tc.desc: test results of OnTreeStateChanged
 * @tc.type: FUNC
 * @tc.require: issueIA4VTS
 */
HWTEST_F(RSSurfaceRenderNodeTwoTest, OnTreeStateChangedTest, TestSize.Level1)
{
    auto renderNode = std::make_shared<RSSurfaceRenderNode>(0);
    renderNode->OnTreeStateChanged();
    EXPECT_TRUE(renderNode->grContext_ == nullptr);
    renderNode->grContext_ = new Drawing::GPUContext();
    renderNode->OnTreeStateChanged();
    EXPECT_TRUE(renderNode->grContext_ != nullptr);
    EXPECT_TRUE(!renderNode->IsOnTheTree());
    std::shared_ptr<RSContext> sharedContext = std::make_shared<RSContext>();
    renderNode->context_ = sharedContext;
    renderNode->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    renderNode->OnTreeStateChanged();
    EXPECT_TRUE(renderNode->IsLeashWindow());
    renderNode->nodeType_ = RSSurfaceNodeType::DEFAULT;
    renderNode->OnTreeStateChanged();
    EXPECT_TRUE(!renderNode->IsLeashWindow());
    renderNode->isOnTheTree_ = true;
    renderNode->OnTreeStateChanged();
    EXPECT_TRUE(renderNode->IsOnTheTree());
    renderNode->nodeType_ = RSSurfaceNodeType::ABILITY_COMPONENT_NODE;
    renderNode->OnTreeStateChanged();
    EXPECT_TRUE(renderNode->IsAbilityComponent());
    renderNode->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    renderNode->isHardwareEnabledNode_ = true;
    renderNode->OnTreeStateChanged();
    EXPECT_TRUE(renderNode->IsHardwareEnabledType());
    sharedContext = nullptr;
    renderNode->context_ = sharedContext;
    renderNode->OnTreeStateChanged();
    EXPECT_TRUE(renderNode->IsHardwareEnabledType());
    renderNode->nodeType_ = RSSurfaceNodeType::ABILITY_COMPONENT_NODE;
    renderNode->OnTreeStateChanged();
    EXPECT_TRUE(renderNode->IsAbilityComponent());
    if (renderNode->grContext_) {
        delete renderNode->grContext_;
        renderNode->grContext_ = nullptr;
    }
}

/**
 * @tc.name: SetIsSubSurfaceNodeTest
 * @tc.desc: test results of SetIsSubSurfaceNode
 * @tc.type: FUNC
 * @tc.require: issueIA4VTS
 */
HWTEST_F(RSSurfaceRenderNodeTwoTest, SetIsSubSurfaceNodeTest, TestSize.Level1)
{
    auto renderNode = std::make_shared<RSSurfaceRenderNode>(0);
    renderNode->stagingRenderParams_ = nullptr;
    renderNode->SetIsSubSurfaceNode(false);
    EXPECT_TRUE(!renderNode->IsSubSurfaceNode());

    renderNode->stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(0);
    renderNode->SetIsSubSurfaceNode(true);
    EXPECT_TRUE(renderNode->IsSubSurfaceNode());
}

/**
 * @tc.name: OnSubSurfaceChangedTest
 * @tc.desc: test results of OnSubSurfaceChanged
 * @tc.type: FUNC
 * @tc.require: issueIA4VTS
 */
HWTEST_F(RSSurfaceRenderNodeTwoTest, OnSubSurfaceChangedTest, TestSize.Level1)
{
    auto renderNode = std::make_shared<RSSurfaceRenderNode>(0);
    renderNode->nodeType_ = RSSurfaceNodeType::ABILITY_COMPONENT_NODE;
    renderNode->OnSubSurfaceChanged();
    EXPECT_TRUE(!renderNode->IsMainWindowType());

    renderNode->nodeType_ = RSSurfaceNodeType::DEFAULT;
    renderNode->OnSubSurfaceChanged();
    EXPECT_TRUE(renderNode->IsMainWindowType());

    std::shared_ptr<RSRenderNode> rsParentNode = std::make_shared<RSRenderNode>(0);
    renderNode->parent_ = rsParentNode;
    renderNode->OnSubSurfaceChanged();
    EXPECT_TRUE(renderNode->IsMainWindowType());

    std::shared_ptr<RSContext> sharedContext = std::make_shared<RSContext>();
    auto parentNode = renderNode->GetParent().lock();
    parentNode->context_ = sharedContext;
    renderNode->OnSubSurfaceChanged();
    EXPECT_TRUE(renderNode->IsMainWindowType());

    RSUniRenderJudgement::uniRenderEnabledType_ = UniRenderEnabledType::UNI_RENDER_DISABLED;
    renderNode->OnSubSurfaceChanged();
    EXPECT_TRUE(renderNode->IsMainWindowType());
}

/**
 * @tc.name: UpdateChildSubSurfaceNodesTest
 * @tc.desc: test results of UpdateChildSubSurfaceNodes
 * @tc.type: FUNC
 * @tc.require: issueIA4VTS
 */
HWTEST_F(RSSurfaceRenderNodeTwoTest, UpdateChildSubSurfaceNodesTest, TestSize.Level1)
{
    auto renderNode = std::make_shared<RSSurfaceRenderNode>(0);
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(0);
    renderNode->nodeType_ = RSSurfaceNodeType::ABILITY_COMPONENT_NODE;
    renderNode->UpdateChildSubSurfaceNodes(node, true);
    EXPECT_TRUE(!renderNode->IsLeashWindow());
    renderNode->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    renderNode->UpdateChildSubSurfaceNodes(node, false);
    EXPECT_TRUE(renderNode->IsLeashWindow());
}

/**
 * @tc.name: GetAllSubSurfaceNodesTest
 * @tc.desc: test results of GetAllSubSurfaceNodes
 * @tc.type: FUNC
 * @tc.require: issueIA4VTS
 */
HWTEST_F(RSSurfaceRenderNodeTwoTest, GetAllSubSurfaceNodesTest, TestSize.Level1)
{
    auto renderNode = std::make_shared<RSSurfaceRenderNode>(0);
    std::shared_ptr<RSSurfaceRenderNode> node = nullptr;
    std::vector<std::pair<NodeId, RSSurfaceRenderNode::WeakPtr>> allSubSurfaceNodes;
    NodeId nodeId = 0;
    std::shared_ptr<RSSurfaceRenderNode> sharedPtr = std::make_shared<RSSurfaceRenderNode>(0);
    RSSurfaceRenderNode::WeakPtr weakPtr = sharedPtr;
    renderNode->childSubSurfaceNodes_.insert(std::make_pair(nodeId, weakPtr));
    nodeId = 1;
    RSSurfaceRenderNode::WeakPtr weak = node;
    renderNode->childSubSurfaceNodes_.insert(std::make_pair(nodeId, node));
    renderNode->GetAllSubSurfaceNodes(allSubSurfaceNodes);
    EXPECT_TRUE(!renderNode->childSubSurfaceNodes_.empty());
}

/**
 * @tc.name: IsLeashWindowSurfaceNodeVisibleTest
 * @tc.desc: test results of IsLeashWindowSurfaceNodeVisible
 * @tc.type: FUNC
 * @tc.require: issueIA4VTS
 */
HWTEST_F(RSSurfaceRenderNodeTwoTest, IsLeashWindowSurfaceNodeVisibleTest, TestSize.Level1)
{
    auto renderNode = std::make_shared<RSSurfaceRenderNode>(0);
    renderNode->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    renderNode->IsLeashWindowSurfaceNodeVisible();
    EXPECT_TRUE(renderNode->IsLeashWindow());

    renderNode->nodeType_ = RSSurfaceNodeType::DEFAULT;
    renderNode->IsLeashWindowSurfaceNodeVisible();
    EXPECT_TRUE(!renderNode->IsLeashWindow());
}

/**
 * @tc.name: SetForceHardwareTest
 * @tc.desc: test results of SetForceHardware
 * @tc.type: FUNC
 * @tc.require: issueIA4VTS
 */
HWTEST_F(RSSurfaceRenderNodeTwoTest, SetForceHardwareTest, TestSize.Level1)
{
    auto renderNode = std::make_shared<RSSurfaceRenderNode>(0);
    renderNode->SetInFixedRotation(false);
    EXPECT_TRUE(!renderNode->isFixRotationByUser_);
    renderNode->isFixRotationByUser_ = true;
    renderNode->isInFixedRotation_ = true;
    renderNode->SetInFixedRotation(false);
    EXPECT_TRUE(renderNode->isFixRotationByUser_);
    renderNode->isInFixedRotation_ = false;
    renderNode->SetInFixedRotation(false);
    EXPECT_TRUE(!renderNode->isInFixedRotation_);
    renderNode->SetInFixedRotation(true);
    EXPECT_TRUE(renderNode->isInFixedRotation_);
}

/**
 * @tc.name: SyncSecurityInfoToFirstLevelNodeTest
 * @tc.desc: test results of SyncSecurityInfoToFirstLevelNode
 * @tc.type: FUNC
 * @tc.require: issueIA4VTS
 */
HWTEST_F(RSSurfaceRenderNodeTwoTest, SyncSecurityInfoToFirstLevelNodeTest, TestSize.Level1)
{
    auto renderNode = std::make_shared<RSSurfaceRenderNode>(0);
    renderNode->SyncSecurityInfoToFirstLevelNode();
    EXPECT_FALSE(renderNode->isSecurityLayer_);
    std::shared_ptr<RSContext> sharedContext = std::make_shared<RSContext>();
    renderNode->context_ = sharedContext;
    renderNode->firstLevelNodeId_ = 0;
    renderNode->SyncSecurityInfoToFirstLevelNode();
    EXPECT_TRUE(sharedContext != nullptr);
}

/**
 * @tc.name: SetForceUIFirstTest
 * @tc.desc: test results of SetForceUIFirst
 * @tc.type: FUNC
 * @tc.require: issueIA4VTS
 */
HWTEST_F(RSSurfaceRenderNodeTwoTest, SetForceUIFirstTest, TestSize.Level1)
{
    auto renderNode = std::make_shared<RSSurfaceRenderNode>(0);
    renderNode->SetForceUIFirst(false);
    EXPECT_FALSE(renderNode->forceUIFirst_);
    renderNode->SetForceUIFirst(true);
    EXPECT_TRUE(renderNode->forceUIFirstChanged_);
}

/**
 * @tc.name: AccumulateOcclusionRegion
 * @tc.desc: test results of AccumulateOcclusionRegion
 * @tc.type: FUNC
 * @tc.require: issueIA4VTS
 */
HWTEST_F(RSSurfaceRenderNodeTwoTest, AccumulateOcclusionRegion, TestSize.Level1)
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
    isUniRender = false;
    auto surfaceBufferImpl = new SurfaceBufferImpl();
    testNode->GetRSSurfaceHandler()->buffer_.buffer = surfaceBufferImpl;
    testNode->AccumulateOcclusionRegion(
        accumulatedRegion, curRegion, hasFilterCacheOcclusion, isUniRender, filterCacheOcclusionEnabled);
    EXPECT_FALSE(testNode->IsSurfaceInStartingWindowStage());
    testNode->SetAbilityBGAlpha(255);
    testNode->SetGlobalAlpha(1.0f);
    testNode->GetMutableRenderProperties().SetCornerRadius(Vector4f(15.0f));
    testNode->AccumulateOcclusionRegion(
        accumulatedRegion, curRegion, hasFilterCacheOcclusion, isUniRender, filterCacheOcclusionEnabled);
    testNode->renderContent_->renderProperties_.frameGravity_ = Gravity::RESIZE;
    testNode->AccumulateOcclusionRegion(
        accumulatedRegion, curRegion, hasFilterCacheOcclusion, isUniRender, filterCacheOcclusionEnabled);
    testNode->name_ = "hisearch";
    testNode->AccumulateOcclusionRegion(
        accumulatedRegion, curRegion, hasFilterCacheOcclusion, isUniRender, filterCacheOcclusionEnabled);
    EXPECT_FALSE(testNode->IsSurfaceInStartingWindowStage());
    testNode->animateState_ = true;
    testNode->AccumulateOcclusionRegion(
        accumulatedRegion, curRegion, hasFilterCacheOcclusion, isUniRender, filterCacheOcclusionEnabled);
    EXPECT_TRUE(testNode->GetAnimateState());
    auto parentPtr = testNode->GetParent().lock();
    RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(parentPtr)->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    testNode->AccumulateOcclusionRegion(
        accumulatedRegion, curRegion, hasFilterCacheOcclusion, isUniRender, filterCacheOcclusionEnabled);
    EXPECT_TRUE(testNode->IsSurfaceInStartingWindowStage());
    if (testNode->GetRSSurfaceHandler()->buffer_.buffer) {
        delete testNode->GetRSSurfaceHandler()->buffer_.buffer;
    }
}

/**
 * @tc.name: UpdateSurfaceSubTreeDirtyFlag
 * @tc.desc: test results of UpdateSurfaceSubTreeDirtyFlag
 * @tc.type: FUNC
 * @tc.require: issueIA4VTS
 */
HWTEST_F(RSSurfaceRenderNodeTwoTest, UpdateSurfaceSubTreeDirtyFlag, TestSize.Level1)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id);
    node->stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(id + 1);
    node->addedToPendingSyncList_ = true;
    node->UpdateSurfaceSubTreeDirtyFlag();
    node->stagingRenderParams_->SetNeedSync(true);
    node->UpdateSurfaceSubTreeDirtyFlag();
    ASSERT_NE(node->stagingRenderParams_, nullptr);
}

/**
 * @tc.name: ResetDrawingCacheStatusIfNodeStatic
 * @tc.desc: test results of ResetDrawingCacheStatusIfNodeStatic
 * @tc.type: FUNC
 * @tc.require: issueIA4VTS
 */
HWTEST_F(RSSurfaceRenderNodeTwoTest, ResetDrawingCacheStatusIfNodeStatic, TestSize.Level1)
{
    auto renderNode = std::make_shared<RSSurfaceRenderNode>(id);
    std::unordered_map<NodeId, std::unordered_set<NodeId>> allRects;
    renderNode->ResetDrawingCacheStatusIfNodeStatic(allRects);
    auto node = std::make_shared<RSRenderNode>(1);
    renderNode->UpdateDrawingCacheNodes(node);
    renderNode->ResetDrawingCacheStatusIfNodeStatic(allRects);
    EXPECT_EQ(renderNode->drawingCacheNodes_.size(), 0);
}

/**
 * @tc.name: UpdateFilterCacheStatusWithVisible
 * @tc.desc: test results of UpdateFilterCacheStatusWithVisible
 * @tc.type: FUNC
 * @tc.require: issueIA4VTS
 */
HWTEST_F(RSSurfaceRenderNodeTwoTest, UpdateFilterCacheStatusWithVisible, TestSize.Level1)
{
    auto renderNode = std::make_shared<RSSurfaceRenderNode>(id);
    bool visible = true;
    renderNode->UpdateFilterCacheStatusWithVisible(visible);
    renderNode->UpdateFilterCacheStatusWithVisible(visible);
    RSUniRenderJudgement::uniRenderEnabledType_ = UniRenderEnabledType::UNI_RENDER_DISABLED;
    renderNode->UpdateFilterCacheStatusWithVisible(false);
    renderNode->isOcclusionVisibleWithoutFilter_ = false;
    renderNode->UpdateFilterCacheStatusWithVisible(visible);
    ASSERT_TRUE(renderNode->prevVisible_);
}

/**
 * @tc.name: UpdateFilterCacheStatusIfNodeStatic
 * @tc.desc: test results of UpdateFilterCacheStatusIfNodeStatic
 * @tc.type: FUNC
 * @tc.require: issueIA4VTS
 */
HWTEST_F(RSSurfaceRenderNodeTwoTest, UpdateFilterCacheStatusIfNodeStatic, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    node->UpdateFilterCacheStatusIfNodeStatic(RectI(0, 0, 100, 100), true);
    std::shared_ptr<RSRenderNode> mockNode = std::make_shared<RSRenderNode>(id);
    mockNode->isOnTheTree_ = true;
    mockNode->renderContent_->renderProperties_.needFilter_ = true;
    node->filterNodes_.emplace_back(mockNode);
    node->UpdateFilterCacheStatusIfNodeStatic(RectI(0, 0, 100, 100), false);
    ASSERT_NE(node->filterNodes_.size(), 0);
}

/**
 * @tc.name: CheckParticipateInOcclusion
 * @tc.desc: test results of CheckParticipateInOcclusion
 * @tc.type: FUNC
 * @tc.require: issueIA4VTS
 */
HWTEST_F(RSSurfaceRenderNodeTwoTest, CheckParticipateInOcclusion, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    auto rsnode = std::make_shared<RSRenderNode>(0);
    rsnode->SetIsScale(true);
    node->parent_ = rsnode;
    node->CheckParticipateInOcclusion();
    node->dstRectChanged_ = true;
    node->CheckParticipateInOcclusion();
    node->isOcclusionInSpecificScenes_ = true;
    node->CheckParticipateInOcclusion();
    node->SetAbilityBGAlpha(255);
    node->SetGlobalAlpha(1.0f);
    node->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    node->SetContainerWindow(true, 1.0f);
    node->CheckParticipateInOcclusion();
    node->isSubSurfaceNode_ = true;
    node->CheckParticipateInOcclusion();
    node->SetIsRotating(true);
    node->CheckParticipateInOcclusion();
    node->SetAnimateState();
    EXPECT_FALSE(node->CheckParticipateInOcclusion());
}

/**
 * @tc.name: GetChildrenNeedFilterRectsWithoutCacheValidTest
 * @tc.desc: test results of GetChildrenNeedFilterRectsWithoutCacheValid
 * @tc.type: FUNC
 * @tc.require: issueIA4VTS
 */
HWTEST_F(RSSurfaceRenderNodeTwoTest, GetChildrenNeedFilterRectsWithoutCacheValidTest, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> testNode = std::make_shared<RSSurfaceRenderNode>(id);
    std::shared_ptr<RSRenderNode> filterNode = std::make_shared<RSRenderNode>(id + 1);
    RectI rect { 0, 0, 20, 20 };
    testNode->UpdateChildrenFilterRects(filterNode, rect, false);
    testNode->UpdateChildrenFilterRects(filterNode, rect, true);
    EXPECT_FALSE(testNode->GetChildrenNeedFilterRectsWithoutCacheValid().empty());
}

/**
 * @tc.name: SetHwcChildrenDisabledStateByUifirst
 * @tc.desc: test results of SetHwcChildrenDisabledStateByUifirst
 * @tc.type: FUNC
 * @tc.require: issueIA4VTS
 */
HWTEST_F(RSSurfaceRenderNodeTwoTest, SetHwcChildrenDisabledStateByUifirst, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    node->SetHwcChildrenDisabledStateByUifirst();
    node->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    node->SetHwcChildrenDisabledStateByUifirst();
    std::shared_ptr<RSSurfaceRenderNode> rssNode = nullptr;
    std::weak_ptr<RSSurfaceRenderNode> rssNodeF = rssNode;
    std::weak_ptr<RSSurfaceRenderNode> rssNodeT = std::make_shared<RSSurfaceRenderNode>(0);
    node->childHardwareEnabledNodes_.emplace_back(rssNodeF);
    node->childHardwareEnabledNodes_.emplace_back(rssNodeT);
    node->SetHwcChildrenDisabledStateByUifirst();
    node->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    std::shared_ptr<RSRenderNode> rsrnode = std::make_shared<RSRenderNode>(0);
    std::vector<std::shared_ptr<RSRenderNode>> children;
    children.push_back(rsrnode);
    node->fullChildrenList_ = std::make_shared<std::vector<std::shared_ptr<RSRenderNode>>>(children);
    node->SetHwcChildrenDisabledStateByUifirst();
    ASSERT_FALSE(node->childHardwareEnabledNodes_.empty());
}

/**
 * @tc.name: LeashWindowRelatedAppWindowOccluded
 * @tc.desc: test results of LeashWindowRelatedAppWindowOccluded
 * @tc.type: FUNC
 * @tc.require: issueIA4VTS
 */
HWTEST_F(RSSurfaceRenderNodeTwoTest, LeashWindowRelatedAppWindowOccluded, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    std::vector<std::shared_ptr<RSSurfaceRenderNode>> appNode;
    ASSERT_FALSE(node->LeashWindowRelatedAppWindowOccluded(appNode));
    node->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    std::shared_ptr<RSSurfaceRenderNode> rssNode = std::make_shared<RSSurfaceRenderNode>(0);
    ASSERT_TRUE(node->LeashWindowRelatedAppWindowOccluded(appNode));
    std::vector<std::shared_ptr<RSRenderNode>> children;
    children.push_back(rssNode);
    node->fullChildrenList_ = std::make_shared<std::vector<std::shared_ptr<RSRenderNode>>>(children);
    ASSERT_TRUE(node->LeashWindowRelatedAppWindowOccluded(appNode));
}

/**
 * @tc.name: IsUIFirstSelfDrawCheck
 * @tc.desc: test results of IsUIFirstSelfDrawCheck
 * @tc.type: FUNC
 * @tc.require: issueIA4VTS
 */
HWTEST_F(RSSurfaceRenderNodeTwoTest, IsUIFirstSelfDrawCheck, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    ASSERT_TRUE(node->IsUIFirstSelfDrawCheck());
    node->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    std::shared_ptr<RSSurfaceRenderNode> rssNode = nullptr;
    std::weak_ptr<RSSurfaceRenderNode> rssNodeF = rssNode;
    std::shared_ptr<RSSurfaceRenderNode> rssNodeTT = std::make_shared<RSSurfaceRenderNode>(0);
    rssNodeTT->GetRSSurfaceHandler()->isCurrentFrameBufferConsumed_ = true;
    std::weak_ptr<RSSurfaceRenderNode> rssNodeT = rssNodeTT;
    node->childHardwareEnabledNodes_.emplace_back(rssNodeF);
    node->childHardwareEnabledNodes_.emplace_back(rssNodeT);
    EXPECT_FALSE(node->IsUIFirstSelfDrawCheck());
    node->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    EXPECT_TRUE(node->IsUIFirstSelfDrawCheck());
    std::shared_ptr<RSSurfaceRenderNode> rssNodeS = std::make_shared<RSSurfaceRenderNode>(0);
    rssNodeS->SetSurfaceNodeType(RSSurfaceNodeType::SCB_SCREEN_NODE);
    std::vector<std::shared_ptr<RSRenderNode>> children;
    children.push_back(rssNodeS);
    node->fullChildrenList_ = std::make_shared<std::vector<std::shared_ptr<RSRenderNode>>>(children);
    EXPECT_FALSE(node->IsUIFirstSelfDrawCheck());
    node->SetSurfaceNodeType(RSSurfaceNodeType::SELF_DRAWING_NODE);
    EXPECT_TRUE(node->IsUIFirstSelfDrawCheck());
    node->SetSurfaceNodeType(RSSurfaceNodeType::SCB_SCREEN_NODE);
    EXPECT_FALSE(node->IsUIFirstSelfDrawCheck());
}

/**
 * @tc.name: IsVisibleDirtyEmpty
 * @tc.desc: test results of IsVisibleDirtyEmpty
 * @tc.type: FUNC
 * @tc.require: issueIA4VTS
 */
HWTEST_F(RSSurfaceRenderNodeTwoTest, IsVisibleDirtyEmpty, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    DeviceType deviceType = DeviceType::PHONE;
    bool res = node->IsVisibleDirtyEmpty(deviceType);
    ASSERT_TRUE(res);
    RectI rect { 0, 0, 20, 20 };
    node->dirtyManager_->currentFrameDirtyRegion_ = rect;
    EXPECT_FALSE(node->IsVisibleDirtyEmpty(deviceType));
    deviceType = DeviceType::PC;
    node->SetSurfaceNodeType(RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE);
    EXPECT_TRUE(node->IsVisibleDirtyEmpty(deviceType));
    node->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    EXPECT_FALSE(node->IsVisibleDirtyEmpty(deviceType));
    std::shared_ptr<RSSurfaceRenderNode> rssNode = std::make_shared<RSSurfaceRenderNode>(0);
    std::vector<std::shared_ptr<RSRenderNode>> children;
    children.push_back(rssNode);
    node->fullChildrenList_ = std::make_shared<std::vector<std::shared_ptr<RSRenderNode>>>(children);
    EXPECT_TRUE(node->IsVisibleDirtyEmpty(deviceType));
    rssNode->SetSurfaceNodeType(RSSurfaceNodeType::SCB_SCREEN_NODE);
    EXPECT_FALSE(node->IsVisibleDirtyEmpty(deviceType));
    node->SetSurfaceNodeType(RSSurfaceNodeType::SCB_SCREEN_NODE);
    EXPECT_FALSE(node->IsVisibleDirtyEmpty(deviceType));
}

/**
 * @tc.name: UpdateCacheSurfaceDirtyManager
 * @tc.desc: test results of UpdateCacheSurfaceDirtyManager
 * @tc.type: FUNC
 * @tc.require: issueIA4VTS
 */
HWTEST_F(RSSurfaceRenderNodeTwoTest, UpdateCacheSurfaceDirtyManager, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    int bufferAge = 1;
    node->UpdateCacheSurfaceDirtyManager(bufferAge);
    ASSERT_NE(node->cacheSurfaceDirtyManager_, nullptr);
    std::shared_ptr<RSSurfaceRenderNode> rssNodeF = nullptr;
    std::shared_ptr<RSSurfaceRenderNode> rssNode = std::make_shared<RSSurfaceRenderNode>(0);
    std::vector<std::shared_ptr<RSRenderNode>> children;
    children.push_back(rssNodeF);
    children.push_back(rssNode);
    node->fullChildrenList_ = std::make_shared<std::vector<std::shared_ptr<RSRenderNode>>>(children);
    node->UpdateCacheSurfaceDirtyManager(bufferAge);
    node->dirtyManager_.reset();
    node->UpdateCacheSurfaceDirtyManager(bufferAge);
    node->cacheSurfaceDirtyManager_.reset();
    node->UpdateCacheSurfaceDirtyManager(bufferAge);
    EXPECT_EQ(node->cacheSurfaceDirtyManager_, nullptr);
}

/**
 * @tc.name: HasOnlyOneRootNode
 * @tc.desc: test results of HasOnlyOneRootNode
 * @tc.type: FUNC
 * @tc.require: issueIA4VTS
 */
HWTEST_F(RSSurfaceRenderNodeTwoTest, HasOnlyOneRootNode, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    bool res = node->HasOnlyOneRootNode();
    ASSERT_FALSE(res);
    auto rsrNode = std::make_shared<RSRenderNode>(0);
    node->children_.push_back(rsrNode);
    EXPECT_FALSE(node->HasOnlyOneRootNode());
}

/**
 * @tc.name: GetNodeIsSingleFrameComposer
 * @tc.desc: test results of GetNodeIsSingleFrameComposer
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSSurfaceRenderNodeTwoTest, GetNodeIsSingleFrameComposer, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    bool res = node->GetNodeIsSingleFrameComposer();
    ASSERT_FALSE(res);
    system::SetParameter("persist.sys.graphic.singleFrame", "1");
    ASSERT_FALSE(node->GetNodeIsSingleFrameComposer());
    node->isNodeSingleFrameComposer_ = true;
    node->name_ = "hwstylusfeature";
    EXPECT_TRUE(node->GetNodeIsSingleFrameComposer());
}

/**
 * @tc.name: QuerySubAssignable
 * @tc.desc: test results of QuerySubAssignable
 * @tc.type: FUNC
 * @tc.require: issueIA4VTS
 */
HWTEST_F(RSSurfaceRenderNodeTwoTest, QuerySubAssignable, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    bool isRotation = true;
    bool res = node->QuerySubAssignable(isRotation);
    ASSERT_FALSE(res);
    node->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    EXPECT_FALSE(node->QuerySubAssignable(isRotation));
    std::shared_ptr<RSSurfaceRenderNode> rssNode = std::make_shared<RSSurfaceRenderNode>(0);
    std::vector<std::shared_ptr<RSRenderNode>> children;
    children.push_back(rssNode);
    node->fullChildrenList_ = std::make_shared<std::vector<std::shared_ptr<RSRenderNode>>>(children);
    EXPECT_FALSE(node->QuerySubAssignable(isRotation));
    std::shared_ptr<RSSurfaceRenderNode> nodeF = std::make_shared<RSSurfaceRenderNode>(1);
    EXPECT_FALSE(nodeF->QuerySubAssignable(isRotation));
}

/**
 * @tc.name: GetGravityTranslate
 * @tc.desc: test results of GetGravityTranslate
 * @tc.type: FUNC
 * @tc.require: issueIA4VTS
 */
HWTEST_F(RSSurfaceRenderNodeTwoTest, GetGravityTranslate, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    float imgWidth = 1.0f;
    float imgHeight = 1.0f;
    node->GetGravityTranslate(imgWidth, imgHeight);
    ASSERT_FALSE(node->IsLeashWindow());
    node->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    std::shared_ptr<RSSurfaceRenderNode> rssNode = std::make_shared<RSSurfaceRenderNode>(0);
    std::vector<std::shared_ptr<RSRenderNode>> children;
    children.push_back(rssNode);
    node->fullChildrenList_ = std::make_shared<std::vector<std::shared_ptr<RSRenderNode>>>(children);
    node->GetGravityTranslate(imgWidth, imgHeight);
    EXPECT_TRUE(node->IsLeashWindow());
}

/**
 * @tc.name: UpdatePartialRenderParams
 * @tc.desc: test results of UpdatePartialRenderParams
 * @tc.type: FUNC
 * @tc.require: issueIA4VTS
 */
HWTEST_F(RSSurfaceRenderNodeTwoTest, UpdatePartialRenderParams, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    node->UpdatePartialRenderParams();
    node->UpdateRenderParams();
    node->UpdateAncestorDisplayNodeInRenderParams();
    node->SetUifirstChildrenDirtyRectParam(RectI());
    node->SetUifirstNodeEnableParam(MultiThreadCacheType::NONE);
    node->SetIsParentUifirstNodeEnableParam(true);
    ASSERT_EQ(node->stagingRenderParams_.get(), nullptr);
    node->stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(id + 1);
    node->UpdatePartialRenderParams();
    node->UpdateRenderParams();
    node->SetUifirstChildrenDirtyRectParam(RectI());
    node->SetSurfaceNodeType(RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE);
    node->UpdatePartialRenderParams();
    EXPECT_NE(node->stagingRenderParams_.get(), nullptr);
}

/**
 * @tc.name: IsSpecialLayerChanged001
 * @tc.desc: test result of IsSpecialLayerChanged
 * @tc.type: FUNC
 * @tc.require: issuesIACYVJ
 */
HWTEST_F(RSSurfaceRenderNodeTwoTest, IsSpecialLayerChanged001, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    node->ResetSpecialLayerChangedFlag();
    EXPECT_FALSE(node->IsSpecialLayerChanged());
}

/**
 * @tc.name: SetAbilityState
 * @tc.desc: test results of SetAbilityState
 * @tc.type:FUNC SetAbilityState
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeTwoTest, SetAbilityState, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSSurfaceRenderNode>(id, rsContext);
    node->SetAbilityState(RSSurfaceNodeAbilityState::FOREGROUND);
    ASSERT_TRUE(node->abilityState_ == RSSurfaceNodeAbilityState::FOREGROUND);
    node->SetAbilityState(RSSurfaceNodeAbilityState::BACKGROUND);
    ASSERT_FALSE(node->abilityState_ == RSSurfaceNodeAbilityState::FOREGROUND);
    node->SetAbilityState(RSSurfaceNodeAbilityState::BACKGROUND);
}

/**
 * @tc.name: GetAbilityState
 * @tc.desc: test results of GetAbilityState
 * @tc.type:FUNC GetAbilityState
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeTwoTest, GetAbilityState, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSSurfaceRenderNode>(id, rsContext);
    auto abilityState = node->GetAbilityState();
    ASSERT_TRUE(abilityState == RSSurfaceNodeAbilityState::FOREGROUND);
    node->abilityState_ = RSSurfaceNodeAbilityState::BACKGROUND;
    abilityState = node->GetAbilityState();
    ASSERT_FALSE(abilityState == RSSurfaceNodeAbilityState::FOREGROUND);
}

/**
 * @tc.name: ChildrenBlurBehindWindowTest
 * @tc.desc: Test ChildrenBlurBehindWindow and NeedUpdateDrawableBehindWindow
 * @tc.type: FUNC
 * @tc.require: issueIB0UQV
 */
HWTEST_F(RSSurfaceRenderNodeTwoTest, ChildrenBlurBehindWindowTest, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSSurfaceRenderNode>(0, rsContext);
    NodeId idOne = 1;
    NodeId idTwo = 2;
    node->AddChildBlurBehindWindow(idOne);
    ASSERT_TRUE(!node->childrenBlurBehindWindow_.empty());
    ASSERT_TRUE(node->NeedUpdateDrawableBehindWindow());
    ASSERT_TRUE(node->GetMutableRenderProperties().GetNeedDrawBehindWindow());
    ASSERT_TRUE(node->NeedDrawBehindWindow());
    node->RemoveChildBlurBehindWindow(idTwo);
    ASSERT_TRUE(node->NeedDrawBehindWindow());
    node->RemoveChildBlurBehindWindow(idOne);
    ASSERT_FALSE(node->NeedDrawBehindWindow());
}
} // namespace Rosen
} // namespace OHOS