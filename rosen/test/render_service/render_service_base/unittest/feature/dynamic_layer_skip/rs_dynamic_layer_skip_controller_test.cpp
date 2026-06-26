/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"
#include "feature/dynamic_layer_skip/rs_dynamic_layer_skip_controller.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
constexpr int SCREEN_LAYER_Z_ORDER = 1;
class RSDynamicLayerSkipControllerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
private:
    NodeId curNodeId_ = 1;
    std::shared_ptr<RSSurfaceRenderNode> GetSurfaceRenderNode(RSSurfaceNodeType nodeType);
    std::shared_ptr<RSRenderNode> GetRenderNode();
    void AddSelfDrawingSurfaceNodeChild(std::shared_ptr<RSSurfaceRenderNode> rootNode, const RectI& drawRect);
    void AddRenderNodeChild(
        std::shared_ptr<RSRenderNode> rootNode, bool hasContent, bool subTreeContent, const RectI& drawRect);
    std::vector<std::shared_ptr<RSRenderNode>> allNodesRef_;
};

void RSDynamicLayerSkipControllerTest::SetUpTestCase() {}
void RSDynamicLayerSkipControllerTest::TearDownTestCase() {}
void RSDynamicLayerSkipControllerTest::SetUp() {}
void RSDynamicLayerSkipControllerTest::TearDown() {}

std::shared_ptr<RSSurfaceRenderNode> RSDynamicLayerSkipControllerTest::GetSurfaceRenderNode(RSSurfaceNodeType nodeType)
{
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(curNodeId_++);
    surfaceNode->nodeType_ = nodeType;
    allNodesRef_.emplace_back(surfaceNode);
    return surfaceNode;
}

std::shared_ptr<RSRenderNode> RSDynamicLayerSkipControllerTest::GetRenderNode()
{
    auto renderNode = std::make_shared<RSRenderNode>(curNodeId_++);
    allNodesRef_.emplace_back(renderNode);
    return renderNode;
}

void RSDynamicLayerSkipControllerTest::AddSelfDrawingSurfaceNodeChild(
    std::shared_ptr<RSSurfaceRenderNode> rootNode, const RectI& drawRect)
{
    auto surfaceNode = GetSurfaceRenderNode(RSSurfaceNodeType::SELF_DRAWING_NODE);
    surfaceNode->selfDrawRect_ = drawRect.ConvertTo<float>();
    surfaceNode->absDrawRect_ = drawRect;
    surfaceNode->dstRect_ = drawRect;
    rootNode->AddChild(surfaceNode);
    rootNode->AddChildHardwareEnabledNode(std::weak_ptr<RSSurfaceRenderNode>(surfaceNode));
    rootNode->GenerateFullChildrenList();
}

void RSDynamicLayerSkipControllerTest::AddRenderNodeChild(
    std::shared_ptr<RSRenderNode> rootNode, bool hasContent, bool subTreeContent, const RectI& drawRect)
{
    auto renderNode = GetRenderNode();
    renderNode->layerContentBits_[LayerDrawContent::SELF] = hasContent;
    renderNode->layerContentBits_[LayerDrawContent::SUBTREE] = subTreeContent;
    renderNode->selfDrawRect_ = drawRect.ConvertTo<float>();
    renderNode->absDrawRect_ = drawRect;
    renderNode->instanceRootNodeId_ = rootNode->GetId();
    rootNode->AddChild(renderNode);
    rootNode->GenerateFullChildrenList();
}

/**
 * @tc.name: LayerSkipContextSyncTest
 * @tc.desc: test LayerSkipContextSync func
 * @tc.type:FUNC
 * @tc.require: issueI5QZ7E
 */
HWTEST_F(RSDynamicLayerSkipControllerTest, LayerSkipContextSyncTest, TestSize.Level1)
{
    LayerSkipContext context;
    RSDynamicLayerSkipController controller;
    // case 1: sync context with empty target self-drawing surface.
    {
        controller.targetSelfDrawingSurface_.clear();
        context.SyncFrom(controller);
        EXPECT_TRUE(context.relevantSurfaceNodeIds_.empty());
    }
    // case 2: sync context with non-empty target self-drawing surface.
    {
        auto surfaceNode = GetSurfaceRenderNode(RSSurfaceNodeType::SELF_DRAWING_NODE);
        controller.targetSelfDrawingSurface_.emplace_back(surfaceNode);
        controller.targetSelfDrawingSurface_.emplace_back();
        context.SyncFrom(controller);
        EXPECT_FALSE(context.relevantSurfaceNodeIds_.empty());
        EXPECT_EQ(context.relevantSurfaceNodeIds_.size(), 1);
    }
}

/**
 * @tc.name: CheckNodeDrawProperty001
 * @tc.desc: test CheckNodeDrawProperty func, node not visible.
 * @tc.type:FUNC
 * @tc.require: issueI5QZ7E
 */
HWTEST_F(RSDynamicLayerSkipControllerTest, CheckNodeDrawProperty001, TestSize.Level1)
{
    RSDynamicLayerSkipController controller;
    auto node = GetRenderNode();
    node->SetGlobalAlpha(0.f);
    node->layerContentBits_[LayerDrawContent::UPDATE] = true;
    controller.CheckNodeDrawProperty(*node);
    EXPECT_FALSE(node->layerContentBits_[LayerDrawContent::SELF]);
}

/**
 * @tc.name: CheckNodeDrawProperty002
 * @tc.desc: test CheckNodeDrawProperty func, node visible but no valid drawable slot
 * @tc.type:FUNC
 * @tc.require: issueI5QZ7E
 */
HWTEST_F(RSDynamicLayerSkipControllerTest, CheckNodeDrawProperty002, TestSize.Level1)
{
    RSDynamicLayerSkipController controller;
    auto node = GetRenderNode();
    node->SetGlobalAlpha(1.f);
    node->renderProperties_.isDrawn_ = true;
    node->shouldPaint_ = true;
    node->layerContentBits_[LayerDrawContent::UPDATE] = true;
    controller.CheckNodeDrawProperty(*node);
    EXPECT_FALSE(node->layerContentBits_[LayerDrawContent::SELF]);
    node->layerContentBits_[LayerDrawContent::UPDATE] = true;
    node->drawableVec_ = nullptr;
    controller.CheckNodeDrawProperty(*node);
    EXPECT_FALSE(node->layerContentBits_[LayerDrawContent::SELF]);
}

/**
 * @tc.name: HasFullScreenSelfDrawingSurface001
 * @tc.desc: test HasFullScreenSelfDrawingSurface func
 * @tc.type:FUNC
 * @tc.require: issueI5QZ7E
 */
HWTEST_F(RSDynamicLayerSkipControllerTest, HasFullScreenSelfDrawingSurface001, TestSize.Level1)
{
    RectI fullscreenRect { 0, 0, 1080, 1920 };
    RSDynamicLayerSkipController controller;
    controller.Init(fullscreenRect, false);
    // case 1 : root node has no child.
    {
        auto rootNode = GetSurfaceRenderNode(RSSurfaceNodeType::APP_WINDOW_NODE);
        EXPECT_FALSE(controller.HasFullScreenSelfDrawingSurface(*rootNode));
    }
    // case 2 : not full-screen self-drawing surface exists.
    {
        auto rootNode = GetSurfaceRenderNode(RSSurfaceNodeType::APP_WINDOW_NODE);
        AddSelfDrawingSurfaceNodeChild(rootNode, { 0, 0, 500, 500 });
        EXPECT_FALSE(controller.HasFullScreenSelfDrawingSurface(*rootNode));
    }
    // case 3 : full-screen self-drawing surface exists.
    {
        auto rootNode = GetSurfaceRenderNode(RSSurfaceNodeType::APP_WINDOW_NODE);
        AddSelfDrawingSurfaceNodeChild(rootNode, fullscreenRect);
        EXPECT_TRUE(controller.HasFullScreenSelfDrawingSurface(*rootNode));
    }
}

/**
 * @tc.name: DetectScreenLayerValidity001
 * @tc.desc: test DetectScreenLayerValidity func
 * @tc.type:FUNC
 * @tc.require: issueI5QZ7E
 */
HWTEST_F(RSDynamicLayerSkipControllerTest, DetectScreenLayerValidity001, TestSize.Level1)
{
    RectI fullscreenRect { 0, 0, 1080, 1920 };
    RSDynamicLayerSkipController controller;
    controller.Init(fullscreenRect, false);
    controller.lastFrameHasFullScreenSurface_ = true;
    // root node has no valid self-drawing surface.
    {
        auto rootNode = GetSurfaceRenderNode(RSSurfaceNodeType::APP_WINDOW_NODE);
        controller.DetectScreenLayerValidity(*rootNode);
        controller.VerifyScreenLayerValidity(SCREEN_LAYER_Z_ORDER);
        EXPECT_FALSE(controller.IsScreenLayerInvalid());
    }
}

/**
 * @tc.name: DetectScreenLayerValidity002
 * @tc.desc: test DetectScreenLayerValidity func with valid self-drawing surface, no occluder.
 * @tc.type:FUNC
 * @tc.require: issueI5QZ7E
 */
HWTEST_F(RSDynamicLayerSkipControllerTest, DetectScreenLayerValidity002, TestSize.Level1)
{
    RectI fullscreenRect { 0, 0, 1080, 1920 };
    RSDynamicLayerSkipController controller;
    controller.Init(fullscreenRect, false);
    controller.lastFrameHasFullScreenSurface_ = true;
    // root node has valid self-drawing surface, and no occluder exists.
    {
        auto rootNode = GetSurfaceRenderNode(RSSurfaceNodeType::APP_WINDOW_NODE);
        AddSelfDrawingSurfaceNodeChild(rootNode, fullscreenRect);
        EXPECT_TRUE(controller.HasFullScreenSelfDrawingSurface(*rootNode));
        controller.DetectScreenLayerValidity(*rootNode);
        controller.VerifyScreenLayerValidity(SCREEN_LAYER_Z_ORDER);
        EXPECT_TRUE(controller.IsScreenLayerInvalid());
    }
}

/**
 * @tc.name: DetectScreenLayerValidity003
 * @tc.desc: test DetectScreenLayerValidity func with valid self-drawing surface, occluder exists.
 * @tc.type:FUNC
 * @tc.require: issueI5QZ7E
 */
HWTEST_F(RSDynamicLayerSkipControllerTest, DetectScreenLayerValidity003, TestSize.Level1)
{
    RectI fullscreenRect { 0, 0, 1080, 1920 };
    RSDynamicLayerSkipController controller;
    controller.Init(fullscreenRect, false);
    controller.lastFrameHasFullScreenSurface_ = true;
    // root node has valid self-drawing surface, but occluder exists.
    {
        auto rootNode = GetSurfaceRenderNode(RSSurfaceNodeType::APP_WINDOW_NODE);
        AddSelfDrawingSurfaceNodeChild(rootNode, fullscreenRect);
        EXPECT_TRUE(controller.HasFullScreenSelfDrawingSurface(*rootNode));
        AddRenderNodeChild(rootNode, true, false, { 0, 0, 100, 100 });
        controller.DetectScreenLayerValidity(*rootNode);
        controller.VerifyScreenLayerValidity(SCREEN_LAYER_Z_ORDER);
        EXPECT_FALSE(controller.IsScreenLayerInvalid());
    }
}

/**
 * @tc.name: DetectScreenLayerValidity004
 * @tc.desc: test DetectScreenLayerValidity func with valid self-drawing surface, occluder is out-of-screen.
 * @tc.type:FUNC
 * @tc.require: issueI5QZ7E
 */
HWTEST_F(RSDynamicLayerSkipControllerTest, DetectScreenLayerValidity004, TestSize.Level1)
{
    RectI fullscreenRect { 0, 0, 1080, 1920 };
    RSDynamicLayerSkipController controller;
    controller.Init(fullscreenRect, false);
    controller.lastFrameHasFullScreenSurface_ = true;
    // root node has valid self-drawing surface, but occluder exists with out-of-screen draw rect.
    {
        auto rootNode = GetSurfaceRenderNode(RSSurfaceNodeType::APP_WINDOW_NODE);
        AddSelfDrawingSurfaceNodeChild(rootNode, fullscreenRect);
        EXPECT_TRUE(controller.HasFullScreenSelfDrawingSurface(*rootNode));
        AddRenderNodeChild(rootNode, true, false,
            fullscreenRect.Offset(fullscreenRect.GetWidth() + 1, fullscreenRect.GetHeight() + 1));
        controller.DetectScreenLayerValidity(*rootNode);
        controller.VerifyScreenLayerValidity(SCREEN_LAYER_Z_ORDER);
        EXPECT_TRUE(controller.IsScreenLayerInvalid());
    }
}

/**
 * @tc.name: DetectScreenLayerValidity005
 * @tc.desc: test DetectScreenLayerValidity func with valid self-drawing surface, with transparent occluder.
 * @tc.type:FUNC
 * @tc.require: issueI5QZ7E
 */
HWTEST_F(RSDynamicLayerSkipControllerTest, DetectScreenLayerValidity005, TestSize.Level1)
{
    RectI fullscreenRect { 0, 0, 1080, 1920 };
    RSDynamicLayerSkipController controller;
    controller.Init(fullscreenRect, false);
    controller.lastFrameHasFullScreenSurface_ = true;
    // root node has valid self-drawing surface, but occluder exists with transparent property.
    {
        auto rootNode = GetSurfaceRenderNode(RSSurfaceNodeType::APP_WINDOW_NODE);
        AddSelfDrawingSurfaceNodeChild(rootNode, fullscreenRect);
        EXPECT_TRUE(controller.HasFullScreenSelfDrawingSurface(*rootNode));
        AddRenderNodeChild(rootNode, false, false, fullscreenRect);
        controller.DetectScreenLayerValidity(*rootNode);
        controller.VerifyScreenLayerValidity(SCREEN_LAYER_Z_ORDER);
        EXPECT_TRUE(controller.IsScreenLayerInvalid());
    }
}

/**
 * @tc.name: DetectScreenLayerValidity006
 * @tc.desc: test DetectScreenLayerValidity func with multiple nodes.
 * @tc.type:FUNC
 * @tc.require: issueI5QZ7E
 */
HWTEST_F(RSDynamicLayerSkipControllerTest, DetectScreenLayerValidity006, TestSize.Level1)
{
    RectI fullscreenRect { 0, 0, 1080, 1920 };
    RSDynamicLayerSkipController controller;
    controller.Init(fullscreenRect, false);
    controller.lastFrameHasFullScreenSurface_ = true;
    // non-full-screen surface/occluder/full screen surface/invalid occluder.
    {
        auto rootNode = GetSurfaceRenderNode(RSSurfaceNodeType::APP_WINDOW_NODE);
        AddSelfDrawingSurfaceNodeChild(rootNode, {0, 0, 1, 1});
        AddRenderNodeChild(rootNode, true, false, fullscreenRect);
        AddSelfDrawingSurfaceNodeChild(rootNode, fullscreenRect);
        AddRenderNodeChild(rootNode, false, false, fullscreenRect);
        controller.DetectScreenLayerValidity(*rootNode);
        controller.VerifyScreenLayerValidity(SCREEN_LAYER_Z_ORDER);
        EXPECT_TRUE(controller.IsScreenLayerInvalid());
    }
}

/**
 * @tc.name: DetectScreenLayerValidity007
 * @tc.desc: test DetectScreenLayerValidity func with multiple nodes.
 * @tc.type:FUNC
 * @tc.require: issueI5QZ7E
 */
HWTEST_F(RSDynamicLayerSkipControllerTest, DetectScreenLayerValidity007, TestSize.Level1)
{
    RectI fullscreenRect { 0, 0, 1080, 1920 };
    RectI partialRect { 0, 0, 500, 500 };
    RSDynamicLayerSkipController controller;
    controller.Init(fullscreenRect, false);
    controller.lastFrameHasFullScreenSurface_ = true;
    // full-screen surface/occluder/non-full-screen surface.
    // in this case, occluder and non-full-screen surface with equal region.
    {
        auto rootNode = GetSurfaceRenderNode(RSSurfaceNodeType::APP_WINDOW_NODE);
        AddSelfDrawingSurfaceNodeChild(rootNode, fullscreenRect);
        AddRenderNodeChild(rootNode, true, false, partialRect);
        AddSelfDrawingSurfaceNodeChild(rootNode, partialRect);
        controller.DetectScreenLayerValidity(*rootNode);
        controller.VerifyScreenLayerValidity(SCREEN_LAYER_Z_ORDER);
        EXPECT_TRUE(controller.IsScreenLayerInvalid());
    }
}

/**
 * @tc.name: VisitRenderNode001
 * @tc.desc: test VisitRenderNode func
 * @tc.type:FUNC
 * @tc.require: issueI5QZ7E
 */
HWTEST_F(RSDynamicLayerSkipControllerTest, VisitRenderNode001, TestSize.Level1)
{
    RSDynamicLayerSkipController controller;
    controller.lastFrameHasFullScreenSurface_ = true;
    // case1: rootnode is null.
    {
        auto node = GetRenderNode();
        node->layerContentBits_[LayerDrawContent::SELF] = true;
        controller.globalOccluderDetected_ = false;
        controller.VisitRenderNode(nullptr, *node);
        EXPECT_TRUE(controller.globalOccluderDetected_);
    }
    // case2: rootnode is not null, but skip draw.
    {
        auto surfaceNode = GetSurfaceRenderNode(RSSurfaceNodeType::APP_WINDOW_NODE);
        surfaceNode->SetSkipDraw(true);
        AddRenderNodeChild(surfaceNode, true, false, { 0, 0, 100, 100 });
        controller.globalOccluderDetected_ = false;
        controller.VisitRenderNode(surfaceNode, *surfaceNode);
        EXPECT_FALSE(controller.globalOccluderDetected_);
    }
    // case3: rootnode is not null, not skip draw.
    {
        auto surfaceNode = GetSurfaceRenderNode(RSSurfaceNodeType::APP_WINDOW_NODE);
        surfaceNode->layerContentBits_[LayerDrawContent::SUBTREE_UPDATE] = true;
        AddRenderNodeChild(surfaceNode, true, false, { 0, 0, 100, 100 });
        controller.globalOccluderDetected_ = false;
        controller.VisitRenderNode(surfaceNode, *surfaceNode);
        EXPECT_TRUE(controller.globalOccluderDetected_);
        EXPECT_EQ(surfaceNode->GetId(), controller.occluderInstanceRootNodeId_);
    }
    // case4: node is self-drawing surface node.
    {
        auto surfaceNode = GetSurfaceRenderNode(RSSurfaceNodeType::SELF_DRAWING_NODE);
        surfaceNode->layerContentBits_[LayerDrawContent::SELF] = false;
        surfaceNode->layerContentBits_[LayerDrawContent::UPDATE] = true;
        controller.globalOccluderDetected_ = false;
        controller.VisitRenderNode(nullptr, *surfaceNode);
        EXPECT_TRUE(controller.globalOccluderDetected_);
    }
}

/**
 * @tc.name: VisitRenderNode002
 * @tc.desc: test VisitRenderNode func
 * @tc.type: FUNC
 * @tc.require: issueI5QZ7E
 */
HWTEST_F(RSDynamicLayerSkipControllerTest, VisitRenderNode002, TestSize.Level1)
{
    RSDynamicLayerSkipController controller;
    controller.lastFrameHasFullScreenSurface_ = true;
    auto surfaceNode = GetSurfaceRenderNode(RSSurfaceNodeType::APP_WINDOW_NODE);
    auto canvasNode = GetRenderNode();
    canvasNode->layerContentBits_[LayerDrawContent::SELF] = true;
    controller.VisitRenderNode(surfaceNode, *canvasNode);
    ASSERT_FALSE(controller.globalOccluderDetected_);
}

/**
 * @tc.name: VerifyScreenLayerValidity001
 * @tc.desc: test VerifyScreenLayerValidity func
 * @tc.type:FUNC
 * @tc.require: issueI5QZ7
 */
HWTEST_F(RSDynamicLayerSkipControllerTest, VerifyScreenLayerValidity001, TestSize.Level1)
{
    // case1: global disabled, skip verifying.
    {
        RSDynamicLayerSkipController controller;
        controller.Init({ 0, 0, 1080, 1920 }, true);
        controller.VerifyScreenLayerValidity(SCREEN_LAYER_Z_ORDER);
        EXPECT_FALSE(controller.IsScreenLayerInvalid());
    }
    // case2: empty self-drawing surface, skip verifying.
    {
        RSDynamicLayerSkipController controller;
        controller.Init({ 0, 0, 1080, 1920 }, false);
        controller.VerifyScreenLayerValidity(SCREEN_LAYER_Z_ORDER);
        EXPECT_FALSE(controller.IsScreenLayerInvalid());
    }
}

/**
 * @tc.name: VerifyScreenLayerValidity002
 * @tc.desc: test VerifyScreenLayerValidity func with some self-drawing surfaces hardware disabled.
 * @tc.type:FUNC
 * @tc.require: issueI5QZ7E
 */
HWTEST_F(RSDynamicLayerSkipControllerTest, VerifyScreenLayerValidity002, TestSize.Level1)
{
    RectI fullscreenRect { 0, 0, 1080, 1920 };
    RSDynamicLayerSkipController controller;
    // some self-drawing surfaces are hardware disabled.
    {
        controller.Init(fullscreenRect, false);
        auto surfaceNode = GetSurfaceRenderNode(RSSurfaceNodeType::SELF_DRAWING_NODE);
        surfaceNode->isHardwareForcedDisabled_ = true;
        controller.targetSelfDrawingSurface_.emplace_back(surfaceNode);
        controller.VerifyScreenLayerValidity(SCREEN_LAYER_Z_ORDER);
        EXPECT_FALSE(controller.IsScreenLayerInvalid());
    }
    // all self-drawing surfaces are hardware enabled.
    {
        controller.Init(fullscreenRect, false);
        auto surfaceNode = GetSurfaceRenderNode(RSSurfaceNodeType::SELF_DRAWING_NODE);
        surfaceNode->isHardwareForcedDisabled_ = false;
        controller.targetSelfDrawingSurface_.emplace_back(surfaceNode);
        controller.VerifyScreenLayerValidity(SCREEN_LAYER_Z_ORDER);
        EXPECT_TRUE(controller.IsScreenLayerInvalid());
    }
}

/**
 * @tc.name: VerifyScreenLayerValidity003
 * @tc.desc: test VerifyScreenLayerValidity func with some self-drawing surfaces have higher z-order.
 * @tc.type:FUNC
 * @tc.require: issueI5QZ7E
 */
HWTEST_F(RSDynamicLayerSkipControllerTest, VerifyScreenLayerValidity003, TestSize.Level1)
{
    RectI fullscreenRect { 0, 0, 1080, 1920 };
    RSDynamicLayerSkipController controller;
    controller.Init(fullscreenRect, false);
    // some self-drawing surfaces have higher z-order than screen layer.
    {
        auto surfaceNode = GetSurfaceRenderNode(RSSurfaceNodeType::SELF_DRAWING_NODE);
        surfaceNode->GetMutableRSSurfaceHandler()->globalZOrder_ = SCREEN_LAYER_Z_ORDER + 1;
        surfaceNode->isHardwareForcedDisabled_ = false;
        surfaceNode->SetGlobalAlpha(0.5f);
        controller.targetSelfDrawingSurface_.emplace_back(surfaceNode);
        controller.VerifyScreenLayerValidity(SCREEN_LAYER_Z_ORDER);
        EXPECT_FALSE(controller.IsScreenLayerInvalid());
    }
}

/**
 * @tc.name: VisitRenderNodeInner_001
 * @tc.desc: test VisitRenderNodeInner_001 with invalid/valid children list.
 * @tc.type:FUNC
 * @tc.require: issueI5QZ7E
 */
HWTEST_F(RSDynamicLayerSkipControllerTest, VisitRenderNodeInner_001, TestSize.Level1)
{
    RectI fullscreenRect { 0, 0, 1080, 1920 };
    RSDynamicLayerSkipController controller;
    {
        // case1: node with invalid children list, should not crash.
        controller.Init(fullscreenRect, false);
        auto rootNode = GetSurfaceRenderNode(RSSurfaceNodeType::APP_WINDOW_NODE);
        rootNode->layerContentBits_[LayerDrawContent::SUBTREE_UPDATE] = true;
        rootNode->fullChildrenList_ = nullptr;
        controller.VisitRenderNodeInner(rootNode, true);
        EXPECT_FALSE(controller.globalOccluderDetected_);
    }
    {
        // case2: node with valid children list, should work as normal.
        controller.Init(fullscreenRect, false);
        auto rootNode = GetSurfaceRenderNode(RSSurfaceNodeType::APP_WINDOW_NODE);
        rootNode->layerContentBits_[LayerDrawContent::SUBTREE_UPDATE] = true;
        AddRenderNodeChild(rootNode, true, false, { 0, 0, 100, 100 });
        AddRenderNodeChild(rootNode, true, false, { 0, 0, 100, 100 });
        controller.VisitRenderNodeInner(rootNode, true);
        ASSERT_TRUE(controller.globalOccluderDetected_);
    }
}
} // namespace OHOS::Rosen