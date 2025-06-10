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
#include "render_thread/rs_render_thread_visitor.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_root_render_node.h"

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
    static constexpr float outerRadius = 30.4f;
    RRect rrect = RRect({0, 0, 0, 0}, outerRadius, outerRadius);
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
    ASSERT_EQ(surfaceRenderNode.GetAbilityBgAlpha(), 255);
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
    surfaceRenderNode.SetContainerWindow(true, rrect);
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
    if (renderNode->GetRSSurfaceHandler()->buffer_.buffer) {
        delete renderNode->GetRSSurfaceHandler()->buffer_.buffer;
        renderNode->GetRSSurfaceHandler()->buffer_.buffer = nullptr;
    }
}

/**
 * @tc.name: UpdateSpecialLayerInfoByTypeChangeTest
 * @tc.desc: function test
 * @tc.type:FUNC
 * @tc.require: issueIB9LWV
 */
HWTEST_F(RSSurfaceRenderNodeTwoTest, UpdateSpecialLayerInfoByTypeChangeTest, TestSize.Level1)
{
    id = 1;
    auto rsContext = std::make_shared<RSContext>();
    auto renderNode = std::make_shared<RSSurfaceRenderNode>(id, rsContext);
    rsContext->GetMutableNodeMap().renderNodeMap_.clear();
    renderNode->UpdateSpecialLayerInfoByTypeChange(SpecialLayerType::SECURITY, true);
    ASSERT_EQ(renderNode->GetFirstLevelNode(), nullptr);
    rsContext->GetMutableNodeMap().renderNodeMap_[ExtractPid(id)][id] = renderNode;
    renderNode->isOnTheTree_ = true;
    renderNode->firstLevelNodeId_ = id;
    renderNode->SetSecurityLayer(true);
    renderNode->UpdateSpecialLayerInfoByTypeChange(SpecialLayerType::SECURITY, true);
    ASSERT_NE(renderNode->GetFirstLevelNode(), nullptr);
    ASSERT_FALSE(renderNode->GetFirstLevelNodeId() != renderNode->GetId());
    auto nodeTwo = std::make_shared<RSSurfaceRenderNode>(id + 1, rsContext);
    rsContext->GetMutableNodeMap().renderNodeMap_[ExtractPid(id + 1)][id + 1] = nodeTwo;
    renderNode->firstLevelNodeId_ = id + 1;
    renderNode->UpdateSpecialLayerInfoByTypeChange(SpecialLayerType::SECURITY, true);
    ASSERT_NE(renderNode->GetFirstLevelNode(), nullptr);
    ASSERT_TRUE(renderNode->GetFirstLevelNodeId() != renderNode->GetId());
    renderNode->UpdateSpecialLayerInfoByTypeChange(SpecialLayerType::SECURITY, true);
    ASSERT_TRUE(renderNode->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_SECURITY));
    ASSERT_NE(renderNode->GetFirstLevelNode(), nullptr);
    renderNode->isOnTheTree_ = true;
    renderNode->UpdateSpecialLayerInfoByTypeChange(SpecialLayerType::SECURITY, true);
    ASSERT_TRUE(renderNode->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_SECURITY));
    ASSERT_NE(renderNode->GetFirstLevelNode(), nullptr);
    ASSERT_TRUE(renderNode->GetFirstLevelNodeId() != renderNode->GetId());

    renderNode->SetProtectedLayer(true);
    renderNode->UpdateSpecialLayerInfoByTypeChange(SpecialLayerType::PROTECTED, true);
    ASSERT_TRUE(renderNode->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_PROTECTED));
}

/**
 * @tc.name: UpdateSpecialLayerInfoByOnTreeStateChangeTest
 * @tc.desc: function test
 * @tc.type:FUNC
 * @tc.require: issueIB9LWV
 */
HWTEST_F(RSSurfaceRenderNodeTwoTest, UpdateSpecialLayerInfoByOnTreeStateChange001, TestSize.Level1)
{
    auto renderNode = std::make_shared<RSSurfaceRenderNode>(0);
    renderNode->SetProtectedLayer(true);
    renderNode->UpdateSpecialLayerInfoByOnTreeStateChange();
    ASSERT_TRUE(renderNode->GetId() == renderNode->GetFirstLevelNodeId());
    renderNode->SetProtectedLayer(false);
    renderNode->UpdateSpecialLayerInfoByOnTreeStateChange();
    ASSERT_FALSE(renderNode->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_PROTECTED));

    auto renderNodeSecond = std::make_shared<RSSurfaceRenderNode>(1);
    renderNodeSecond->UpdateSpecialLayerInfoByOnTreeStateChange();
    ASSERT_TRUE(renderNodeSecond->GetId() != renderNodeSecond->GetFirstLevelNodeId());
    renderNodeSecond->SetProtectedLayer(true);
    renderNodeSecond->isOnTheTree_ = true;
    renderNodeSecond->UpdateSpecialLayerInfoByOnTreeStateChange();
    ASSERT_TRUE(renderNodeSecond->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_PROTECTED));
    renderNodeSecond->isOnTheTree_ = false;
    renderNodeSecond->UpdateSpecialLayerInfoByOnTreeStateChange();
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
        renderNode->GetRSSurfaceHandler()->buffer_.buffer = nullptr;
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
        renderNode->GetRSSurfaceHandler()->buffer_.buffer = nullptr;
    }
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
 * @tc.name: SetInFixedRotationTest001
 * @tc.desc: test results of SetInFixedRotation
 * @tc.type: FUNC
 * @tc.require: issueICCYNK
 */
HWTEST_F(RSSurfaceRenderNodeTwoTest, SetInFixedRotationTest001, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(0);
    surfaceNode->InitRenderParams();
    RectI srcRect1 { 0, 0, 10, 10 };
    RectI dstRect1 { 5, 5, 15, 15 };
    surfaceNode->SetSrcRect(srcRect1);
    surfaceNode->SetDstRect(dstRect1);
    surfaceNode->UpdateHwcNodeLayerInfo(GraphicTransformType::GRAPHIC_ROTATE_NONE, true);
    surfaceNode->SetForceHardwareAndFixRotation(true);
    EXPECT_TRUE(surfaceNode->isFixRotationByUser_);
    surfaceNode->SetInFixedRotation(true);
    EXPECT_TRUE(surfaceNode->isInFixedRotation_);
    auto oriSrcRect = surfaceNode->GetOriginalSrcRect();
    auto oriDstRect = surfaceNode->GetOriginalDstRect();
    ASSERT_TRUE(srcRect1.left_ == oriSrcRect.left_ && srcRect1.top_ == oriSrcRect.top_ &&
                srcRect1.width_ == oriSrcRect.width_ && srcRect1.height_ == oriSrcRect.height_);
    ASSERT_TRUE(dstRect1.left_ == oriDstRect.left_ && dstRect1.top_ == oriDstRect.top_ &&
                dstRect1.width_ == oriDstRect.width_ && dstRect1.height_ == oriDstRect.height_);

    RectI srcRect2 { 10, 10, 20, 20 };
    RectI dstRect2 { 20, 20, 40, 40 };
    surfaceNode->SetSrcRect(srcRect2);
    surfaceNode->SetDstRect(dstRect2);
    surfaceNode->UpdateHwcNodeLayerInfo(GraphicTransformType::GRAPHIC_ROTATE_NONE, true);
    oriSrcRect = surfaceNode->GetOriginalSrcRect();
    oriDstRect = surfaceNode->GetOriginalDstRect();
    ASSERT_TRUE(srcRect1.left_ == oriSrcRect.left_ && srcRect1.top_ == oriSrcRect.top_ &&
                srcRect1.width_ == oriSrcRect.width_ && srcRect1.height_ == oriSrcRect.height_);
    ASSERT_TRUE(dstRect1.left_ == oriDstRect.left_ && dstRect1.top_ == oriDstRect.top_ &&
                dstRect1.width_ == oriDstRect.width_ && dstRect1.height_ == oriDstRect.height_);
    ASSERT_FALSE(srcRect2.left_ == oriSrcRect.left_ || srcRect2.top_ == oriSrcRect.top_ ||
                 srcRect2.width_ == oriSrcRect.width_ || srcRect2.height_ == oriSrcRect.height_);
    ASSERT_FALSE(dstRect2.left_ == oriDstRect.left_ || dstRect2.top_ == oriDstRect.top_ ||
                 dstRect2.width_ == oriDstRect.width_ || dstRect2.height_ == oriDstRect.height_);
}

/**
 * @tc.name: SetInFixedRotationTest002
 * @tc.desc: test results of SetInFixedRotation
 * @tc.type: FUNC
 * @tc.require: issueICCYNK
 */
HWTEST_F(RSSurfaceRenderNodeTwoTest, SetInFixedRotationTest002, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(0);
    surfaceNode->InitRenderParams();
    RectI srcRect1 { 0, 0, 10, 10 };
    RectI dstRect1 { 5, 5, 15, 15 };
    surfaceNode->SetSrcRect(srcRect1);
    surfaceNode->SetDstRect(dstRect1);
    surfaceNode->UpdateHwcNodeLayerInfo(GraphicTransformType::GRAPHIC_ROTATE_NONE, true);
    surfaceNode->SetForceHardwareAndFixRotation(true);
    EXPECT_TRUE(surfaceNode->isFixRotationByUser_);
    surfaceNode->SetInFixedRotation(true);
    EXPECT_TRUE(surfaceNode->isInFixedRotation_);
    auto oriSrcRect = surfaceNode->GetOriginalSrcRect();
    auto oriDstRect = surfaceNode->GetOriginalDstRect();
    ASSERT_TRUE(srcRect1.left_ == oriSrcRect.left_ && srcRect1.top_ == oriSrcRect.top_ &&
                srcRect1.width_ == oriSrcRect.width_ && srcRect1.height_ == oriSrcRect.height_);
    ASSERT_TRUE(dstRect1.left_ == oriDstRect.left_ && dstRect1.top_ == oriDstRect.top_ &&
                dstRect1.width_ == oriDstRect.width_ && dstRect1.height_ == oriDstRect.height_);

    surfaceNode->SetInFixedRotation(false);
    RectI srcRect2 { 10, 10, 20, 20 };
    RectI dstRect2 { 20, 20, 40, 40 };
    surfaceNode->SetSrcRect(srcRect2);
    surfaceNode->SetDstRect(dstRect2);
    surfaceNode->UpdateHwcNodeLayerInfo(GraphicTransformType::GRAPHIC_ROTATE_NONE, true);
    surfaceNode->SetInFixedRotation(true);
    oriSrcRect = surfaceNode->GetOriginalSrcRect();
    oriDstRect = surfaceNode->GetOriginalDstRect();
    ASSERT_FALSE(srcRect1.left_ == oriSrcRect.left_ || srcRect1.top_ == oriSrcRect.top_ ||
                 srcRect1.width_ == oriSrcRect.width_ || srcRect1.height_ == oriSrcRect.height_);
    ASSERT_FALSE(dstRect1.left_ == oriDstRect.left_ || dstRect1.top_ == oriDstRect.top_ ||
                 dstRect1.width_ == oriDstRect.width_ || dstRect1.height_ == oriDstRect.height_);
    ASSERT_TRUE(srcRect2.left_ == oriSrcRect.left_ && srcRect2.top_ == oriSrcRect.top_ &&
                srcRect2.width_ == oriSrcRect.width_ && srcRect2.height_ == oriSrcRect.height_);
    ASSERT_TRUE(dstRect2.left_ == oriDstRect.left_ && dstRect2.top_ == oriDstRect.top_ &&
                dstRect2.width_ == oriDstRect.width_ && dstRect2.height_ == oriDstRect.height_);
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
    testNode->renderProperties_.frameGravity_ = Gravity::RESIZE;
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
        testNode->GetRSSurfaceHandler()->buffer_.buffer = nullptr;
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
    mockNode->renderProperties_.needFilter_ = true;
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
    node->SetContainerWindow(true, rrect);
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
 * @tc.name: SetHwcChildrenDisabledState
 * @tc.desc: test results of SetHwcChildrenDisabledState
 * @tc.type: FUNC
 * @tc.require: issueIA4VTS
 */
HWTEST_F(RSSurfaceRenderNodeTwoTest, SetHwcChildrenDisabledState, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    node->SetHwcChildrenDisabledState();
    node->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    node->SetHwcChildrenDisabledState();
    std::shared_ptr<RSSurfaceRenderNode> rssNode = nullptr;
    std::weak_ptr<RSSurfaceRenderNode> rssNode1 = rssNode;
    std::weak_ptr<RSSurfaceRenderNode> rssNode2 = std::make_shared<RSSurfaceRenderNode>(0);
    auto node3 = std::make_shared<RSSurfaceRenderNode>(id + 1);
    node3->GetMultableSpecialLayerMgr().Set(SpecialLayerType::PROTECTED, true);
    std::weak_ptr<RSSurfaceRenderNode> rssNode3 = node3;
    node->childHardwareEnabledNodes_.emplace_back(rssNode1);
    node->childHardwareEnabledNodes_.emplace_back(rssNode2);
    node->childHardwareEnabledNodes_.emplace_back(rssNode3);
    node->SetHwcChildrenDisabledState();
    node->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    std::shared_ptr<RSRenderNode> rsrNode1 = std::make_shared<RSRenderNode>(0);
    std::shared_ptr<RSRenderNode> rsrNode2 = std::make_shared<RSSurfaceRenderNode>(id + 2);
    std::vector<std::shared_ptr<RSRenderNode>> children;
    children.push_back(rsrNode1);
    children.push_back(rsrNode2);
    node->fullChildrenList_ = std::make_shared<std::vector<std::shared_ptr<RSRenderNode>>>(children);
    node->SetHwcChildrenDisabledState();
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
    auto rssNode1 = std::make_shared<RSSurfaceRenderNode>(id + 1);
    auto rssNode2 = std::make_shared<RSRenderNode>(id + 2);
    auto rssNode3 = std::make_shared<RSSurfaceRenderNode>(id + 3);
    Occlusion::Rect rect(0, 0, 10, 10);
    Occlusion::Region region(rect);
    rssNode3->visibleRegion_ = region;
    ASSERT_TRUE(node->LeashWindowRelatedAppWindowOccluded(appNode));
    std::vector<std::shared_ptr<RSRenderNode>> children;
    children.push_back(rssNode1);
    children.push_back(rssNode2);
    children.push_back(rssNode3);
    node->fullChildrenList_ = std::make_shared<std::vector<std::shared_ptr<RSRenderNode>>>(children);
    ASSERT_FALSE(node->LeashWindowRelatedAppWindowOccluded(appNode));
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
    node->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    node->UpdateCacheSurfaceDirtyManager(bufferAge);
    node->dirtyManager_.reset();
    node->UpdateCacheSurfaceDirtyManager(bufferAge);
    node->cacheSurfaceDirtyManager_.reset();
    node->UpdateCacheSurfaceDirtyManager(bufferAge);
    EXPECT_EQ(node->cacheSurfaceDirtyManager_, nullptr);
    node->dirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    node->UpdateCacheSurfaceDirtyManager(bufferAge);
    EXPECT_NE(node->dirtyManager_, nullptr);
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
    ASSERT_FALSE(node->HasOnlyOneRootNode());
    auto child1 = std::make_shared<RSRenderNode>(id + 1);
    node->children_.emplace_back(child1);
    EXPECT_FALSE(node->HasOnlyOneRootNode());
    auto child2 = std::make_shared<RSRootRenderNode>(id + 2);
    node->children_.clear();
    node->children_.emplace_back(child2);
    EXPECT_TRUE(node->HasOnlyOneRootNode());
    auto child3 = std::make_shared<RSRenderNode>(id + 3);
    child1->children_.emplace_back(child3);
    EXPECT_TRUE(node->HasOnlyOneRootNode());
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
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id + 1);
    bool isRotation = true;
    node->SetProtectedLayer(false);
    node->firstLevelNodeId_ = id + 1;
    bool res = node->QuerySubAssignable(isRotation);
    ASSERT_EQ(res, RSSystemProperties::GetCacheOptimizeRotateEnable());
    node->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    res = node->QuerySubAssignable(isRotation);
    ASSERT_EQ(res, RSSystemProperties::GetCacheOptimizeRotateEnable());
    std::shared_ptr<RSSurfaceRenderNode> rssNode = std::make_shared<RSSurfaceRenderNode>(0);
    std::vector<std::shared_ptr<RSRenderNode>> children;
    children.push_back(rssNode);
    node->fullChildrenList_ = std::make_shared<std::vector<std::shared_ptr<RSRenderNode>>>(children);
    res = node->QuerySubAssignable(isRotation);
    ASSERT_EQ(res, RSSystemProperties::GetCacheOptimizeRotateEnable());
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

} // namespace Rosen
} // namespace OHOS