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

#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_draw_cmd.h"
#include "render_thread/rs_render_thread_visitor.h"
#include "platform/common/rs_log.h"
#include "property/rs_properties_painter.h"
#include "command/rs_surface_node_command.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSCanvasRenderNodeTest : public testing::Test {
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

void RSCanvasRenderNodeTest::SetUpTestCase()
{
    canvas_ = new RSPaintFilterCanvas(&drawingCanvas_);
}
void RSCanvasRenderNodeTest::TearDownTestCase()
{
    delete canvas_;
    canvas_ = nullptr;
}
void RSCanvasRenderNodeTest::SetUp() {}
void RSCanvasRenderNodeTest::TearDown() {}

/**
 * @tc.name: UpdateRecording001
 * @tc.desc: test results of UpdateRecording
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasRenderNodeTest, UpdateRecording001, TestSize.Level1)
{
    auto canvasRenderNode = std::make_shared<RSCanvasRenderNode>(id, context);
    canvasRenderNode->UpdateRecording(nullptr, RSModifierType::INVALID);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: UpdateRecording002
 * @tc.desc: test results of UpdateRecording
 * @tc.type: FUNC
 * @tc.require: issueI9VPPN
 */
HWTEST_F(RSCanvasRenderNodeTest, UpdateRecording002, TestSize.Level1)
{
    int32_t w;
    int32_t h;
    auto canvasRenderNode = std::make_shared<RSCanvasRenderNode>(id + 1);
    auto drawCmds = std::make_shared<Drawing::DrawCmdList>(w, h);
    Drawing::Paint paint;
    auto drawOpItem = std::make_shared<Drawing::DrawWithPaintOpItem>(paint, Drawing::DrawOpItem::Type::POINT_OPITEM);
    drawCmds->drawOpItems_.emplace_back(drawOpItem);
    EXPECT_TRUE(!drawCmds->IsEmpty());
    canvasRenderNode->UpdateRecording(drawCmds, RSModifierType::INVALID);
}

/**
 * @tc.name: PrepareTest
 * @tc.desc: test results of Prepare
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasRenderNodeTest, PrepareTest, TestSize.Level1)
{
    std::shared_ptr<RSNodeVisitor> visitor = nullptr;
    NodeId nodeId = 0;
    std::weak_ptr<RSContext> context;
    RSCanvasRenderNode rsCanvasRenderNode(nodeId, context);
    rsCanvasRenderNode.Prepare(visitor);

    visitor = std::make_shared<RSRenderThreadVisitor>();
    rsCanvasRenderNode.Prepare(visitor);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: ProcessTest
 * @tc.desc: test results of Process
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasRenderNodeTest, ProcessTest, TestSize.Level1)
{
    std::shared_ptr<RSNodeVisitor> visitor = nullptr;
    NodeId nodeId = 0;
    std::weak_ptr<RSContext> context;
    RSCanvasRenderNode rsCanvasRenderNode(nodeId, context);
    rsCanvasRenderNode.Process(visitor);

    visitor = std::make_shared<RSRenderThreadVisitor>();
    rsCanvasRenderNode.Process(visitor);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: ProcessRenderBeforeChildrenTest001
 * @tc.desc: test results of ProcessRenderBeforeChildren
 * @tc.type: FUNC
 * @tc.require: issueI9VPPN
 */
HWTEST_F(RSCanvasRenderNodeTest, ProcessRenderBeforeChildrenTest001, TestSize.Level1)
{
    auto canvasRenderNode = std::make_shared<RSCanvasRenderNode>(id, context);
    ASSERT_TRUE(canvas_ != nullptr);
    canvasRenderNode->ProcessRenderBeforeChildren(*canvas_);
}

/**
 * @tc.name: ProcessAnimatePropertyAfterChildrenTest001
 * @tc.desc: test results of ProcessRenderAfterChildren
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasRenderNodeTest, ProcessAnimatePropertyAfterChildrenTest001, TestSize.Level1)
{
    auto canvasRenderNode = std::make_shared<RSCanvasRenderNode>(id, context);
    canvasRenderNode->ProcessRenderAfterChildren(*canvas_);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: ProcessTransitionAfterChildren001
 * @tc.desc: test results of ProcessTransitionAfterChildren
 * @tc.type: FUNC
 * @tc.require: issueI9VPPN
 */
HWTEST_F(RSCanvasRenderNodeTest, ProcessTransitionAfterChildren001, TestSize.Level1)
{
    auto canvasRenderNode = std::make_shared<RSCanvasRenderNode>(id, context);
    ASSERT_TRUE(canvas_ != nullptr);
    canvasRenderNode->ProcessTransitionAfterChildren(*canvas_);
}

/**
 * @tc.name: ColorBlendModeTest
 * @tc.desc: test results of ColorBlendMode
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasRenderNodeTest, ColorBlendModeTest, TestSize.Level1)
{
    auto canvasRenderNode = std::make_shared<RSCanvasRenderNode>(id, context);
    canvas_->SaveLayer({ nullptr, nullptr });

    Drawing::BlendMode drawingBlendMode = Drawing::BlendMode::SRC;
    Drawing::Brush maskBrush;
    maskBrush.SetBlendMode(drawingBlendMode);
    Drawing::SaveLayerOps maskLayerRec(nullptr, &maskBrush, 0);
    canvas_->SaveLayer(maskLayerRec);

    canvas_->Restore();
    canvas_->Restore();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: ProcessShadowBatchingTest
 * @tc.desc: test results of ProcessShadowBatching
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasRenderNodeTest, ProcessShadowBatchingTest, TestSize.Level1)
{
    NodeId nodeId = 0;
    std::weak_ptr<RSContext> context;
    RSCanvasRenderNode rsCanvasRenderNode(nodeId, context);
    rsCanvasRenderNode.ProcessShadowBatching(*canvas_);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: UpdateDisplayHDRNodeList001
 * @tc.desc: test UpdateDisplayHDRNodeList
 * @tc.type: FUNC
 * @tc.require: #IBPVN9
 */
HWTEST_F(RSCanvasRenderNodeTest, UpdateDisplayHDRNodeList001, TestSize.Level1)
{
    NodeId nodeId = 0;
    RSCanvasRenderNode node1(nodeId);
    node1.UpdateDisplayHDRNodeList(false, 0);

    NodeId displayRenderNodeId = 1;
    auto context = std::make_shared<RSContext>();
    RSCanvasRenderNode node(nodeId, context);
    node.UpdateDisplayHDRNodeList(false, 1);

    auto& nodeMap = context->GetMutableNodeMap();
    struct RSDisplayNodeConfig config;
    auto displayRenderNode = std::make_shared<RSDisplayRenderNode>(displayRenderNodeId, config);
    bool res = nodeMap.RegisterRenderNode(displayRenderNode);
    ASSERT_EQ(res, true);
    node.displayNodeId_ = displayRenderNodeId;

    node.UpdateDisplayHDRNodeList(true, 1);
    EXPECT_NE(displayRenderNode->hdrNodeList_.find(nodeId), displayRenderNode->hdrNodeList_.end());

    node.UpdateDisplayHDRNodeList(false, 1);
    EXPECT_EQ(displayRenderNode->hdrNodeList_.find(nodeId), displayRenderNode->hdrNodeList_.end());

    displayRenderNode->GetHDRNodeList();
}

/**
 * @tc.name: GetHDRNodeList001
 * @tc.desc: test GetHDRNodeList
 * @tc.type: FUNC
 * @tc.require: #IBPVN9
 */
HWTEST_F(RSCanvasRenderNodeTest, GetHDRNodeList001, TestSize.Level1)
{
    NodeId displayRenderNodeId = 1;
    struct RSDisplayNodeConfig config;
    auto displayRenderNode = std::make_shared<RSDisplayRenderNode>(displayRenderNodeId, config);

    auto& hdrNodeList = displayRenderNode->GetHDRNodeList();
    EXPECT_TRUE(hdrNodeList.empty());

    displayRenderNode->InsertHDRNode(1);
    EXPECT_NE(displayRenderNode->hdrNodeList_.find(1), displayRenderNode->hdrNodeList_.end());
}

/**
 * @tc.name: OnTreeStateChanged
 * @tc.desc: test results of OnTreeStateChanged
 * @tc.type: FUNC
 * @tc.require: issueI9VPPN
 */
HWTEST_F(RSCanvasRenderNodeTest, OnTreeStateChanged, TestSize.Level1)
{
    NodeId nodeId = 0;
    std::weak_ptr<RSContext> context;
    RSCanvasRenderNode rsCanvasRenderNode(nodeId, context);
    rsCanvasRenderNode.SetCacheType(CacheType::CONTENT);
    EXPECT_EQ(rsCanvasRenderNode.IsOnTheTree(), false);
    EXPECT_FALSE(rsCanvasRenderNode.needClearSurface_);
    rsCanvasRenderNode.OnTreeStateChanged();
    EXPECT_EQ(rsCanvasRenderNode.GetCacheType(), CacheType::NONE);
    EXPECT_TRUE(rsCanvasRenderNode.needClearSurface_);
}

/**
 * @tc.name: SetHDRPresent001
 * @tc.desc: test true of SetHDRPresent
 * @tc.type: FUNC
 * @tc.require: issueIB6Y6O
 */
HWTEST_F(RSCanvasRenderNodeTest, SetHDRPresent001, TestSize.Level1)
{
    NodeId nodeId = 1;
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    auto node = std::make_shared<RSCanvasRenderNode>(nodeId, context, true);
    node->context_ = context;
    EXPECT_TRUE(node->GetContext().lock() != nullptr);
    NodeId surfaceNodeId = 2;
    SurfaceNodeCommandHelper::Create(*context, surfaceNodeId);
    auto surfaceNode = context->GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(surfaceNodeId);
    node->instanceRootNodeId_ = surfaceNodeId;
    node->isOnTheTree_ = true;
    node->SetHDRPresent(true);
    EXPECT_TRUE(node->GetHDRPresent());
}

/**
 * @tc.name: SetHDRPresent002
 * @tc.desc: test false of SetHDRPresent
 * @tc.type: FUNC
 * @tc.require: issueIB6Y6O
 */
HWTEST_F(RSCanvasRenderNodeTest, SetHDRPresent002, TestSize.Level1)
{
    NodeId nodeId = 0;
    std::weak_ptr<RSContext> context;
    RSCanvasRenderNode rsCanvasRenderNode(nodeId, context);
    rsCanvasRenderNode.SetHDRPresent(false);
    EXPECT_FALSE(rsCanvasRenderNode.GetHDRPresent());
}

/**
 * @tc.name: SetIsWideColorGamut001
 * @tc.desc: test true of SetIsWideColorGamut
 * @tc.type: FUNC
 * @tc.require: issueIB6Y6O
 */
HWTEST_F(RSCanvasRenderNodeTest, SetIsWideColorGamut001, TestSize.Level1)
{
    NodeId nodeId = 0;
    std::weak_ptr<RSContext> context;
    RSCanvasRenderNode rsCanvasRenderNode(nodeId, context);
    rsCanvasRenderNode.SetIsWideColorGamut(true);
    EXPECT_TRUE(rsCanvasRenderNode.GetIsWideColorGamut());
}

/**
 * @tc.name: SetIsWideColorGamut002
 * @tc.desc: test false of SetIsWideColorGamut
 * @tc.type: FUNC
 * @tc.require: issueIB6Y6O
 */
HWTEST_F(RSCanvasRenderNodeTest, SetIsWideColorGamut002, TestSize.Level1)
{
    NodeId nodeId = 0;
    std::weak_ptr<RSContext> context;
    RSCanvasRenderNode rsCanvasRenderNode(nodeId, context);
    rsCanvasRenderNode.SetIsWideColorGamut(false);
    EXPECT_FALSE(rsCanvasRenderNode.GetIsWideColorGamut());
}

/**
 * @tc.name: ModifyWideWindowColorGamutNum001
 * @tc.desc: test ModifyWideWindowColorGamutNum
 * @tc.type: FUNC
 * @tc.require: issueIBF3VR
 */
HWTEST_F(RSCanvasRenderNodeTest, ModifyWideWindowColorGamutNum001, TestSize.Level1)
{
    NodeId testId = 0;
    std::shared_ptr<RSCanvasRenderNode> testNode = std::make_shared<RSCanvasRenderNode>(testId);
    testNode->ModifyWideWindowColorGamutNum(false);
    EXPECT_TRUE(testNode->GetContext().lock() == nullptr);

    NodeId nodeId = 1;
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    auto node = std::make_shared<RSCanvasRenderNode>(nodeId, context, true);
    node->context_ = context;
    EXPECT_TRUE(node->GetContext().lock() != nullptr);
    NodeId surfaceNodeId = 2;
    SurfaceNodeCommandHelper::Create(*context, surfaceNodeId);
    auto surfaceNode = context->GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(surfaceNodeId);
    node->instanceRootNodeId_ = surfaceNodeId;
    
    node->ModifyWideWindowColorGamutNum(true);
    ASSERT_EQ(surfaceNode->wideColorGamutNum_, 1);
    node->ModifyWideWindowColorGamutNum(false);
    ASSERT_EQ(surfaceNode->wideColorGamutNum_, 0);
}

/**
 * @tc.name: DrawShadow
 * @tc.desc: test results of DrawShadow
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasRenderNodeTest, DrawShadow, TestSize.Level1)
{
    NodeId nodeId = 0;
    std::weak_ptr<RSContext> context;
    RSCanvasRenderNode rsCanvasRenderNode(nodeId, context);
    RSProperties property;
    RSModifierContext modifierContext(property);
    Drawing::Canvas canvasArgs(1, 1);
    RSPaintFilterCanvas canvas(&canvasArgs);
    rsCanvasRenderNode.DrawShadow(modifierContext, canvas);

    RSContext* rsContext = new RSContext();
    std::shared_ptr<RSContext> sharedContext(rsContext);
    std::weak_ptr<RSRenderNode> parent = std::make_shared<RSRenderNode>(1, sharedContext);
    rsCanvasRenderNode.SetParent(parent);
    rsCanvasRenderNode.DrawShadow(modifierContext, canvas);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: PropertyDrawableRender
 * @tc.desc: test results of PropertyDrawableRender
 * @tc.type: FUNC
 * @tc.require: issueI9VPPN
 */
HWTEST_F(RSCanvasRenderNodeTest, PropertyDrawableRender, TestSize.Level1)
{
    NodeId nodeId = 0;
    std::weak_ptr<RSContext> context;
    RSCanvasRenderNode rsCanvasRenderNode(nodeId, context);
    bool includeProperty = false;
    Drawing::Canvas canvasArgs(1, 1);
    RSPaintFilterCanvas canvas(&canvasArgs);
    rsCanvasRenderNode.PropertyDrawableRender(canvas, includeProperty);
    includeProperty = true;
    rsCanvasRenderNode.PropertyDrawableRender(canvas, includeProperty);

    auto sharedContext = std::make_shared<RSContext>();
    auto rSCanvasRenderNodeParent = std::make_shared<RSCanvasRenderNode>(nodeId + 1, sharedContext);
    rsCanvasRenderNode.SetParent(rSCanvasRenderNodeParent);
    rsCanvasRenderNode.PropertyDrawableRender(canvas, includeProperty);
    includeProperty = false;
    rsCanvasRenderNode.PropertyDrawableRender(canvas, includeProperty);

    auto& parentProperty = rSCanvasRenderNodeParent->GetMutableRenderProperties();
    parentProperty.SetUseShadowBatching(true);
    EXPECT_TRUE(rsCanvasRenderNode.GetParent().lock()->GetRenderProperties().GetUseShadowBatching());
    rsCanvasRenderNode.PropertyDrawableRender(canvas, includeProperty);
    includeProperty = true;
    rsCanvasRenderNode.PropertyDrawableRender(canvas, includeProperty);

    rsCanvasRenderNode.ProcessAnimatePropertyBeforeChildren(canvas, includeProperty);
}

/**
 * @tc.name: ApplyDrawCmdModifier
 * @tc.desc: test results of ApplyDrawCmdModifier
 * @tc.type: FUNC
 * @tc.require: issueI9VPPN
 */
HWTEST_F(RSCanvasRenderNodeTest, ApplyDrawCmdModifier, TestSize.Level1)
{
    NodeId nodeId = 0;
    std::weak_ptr<RSContext> context;
    RSCanvasRenderNode rsCanvasRenderNode(nodeId, context);
    RSProperties property;
    RSModifierContext modifierContext(property);
    RSModifierType type = RSModifierType::INVALID;
    rsCanvasRenderNode.ApplyDrawCmdModifier(modifierContext, type);
    EXPECT_TRUE(rsCanvasRenderNode.drawCmdModifiers_.empty());

    std::shared_ptr<Drawing::DrawCmdList> drawCmdList = std::make_shared<Drawing::DrawCmdList>();
    drawCmdList->SetWidth(2024);
    drawCmdList->SetHeight(2090);
    auto propertyTwo = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    propertyTwo->GetRef() = drawCmdList;
    std::list<std::shared_ptr<RSRenderModifier>> listModifier { std::make_shared<RSDrawCmdListRenderModifier>(
        propertyTwo) };
    type = RSModifierType::FOREGROUND_STYLE;
    rsCanvasRenderNode.drawCmdModifiers_.emplace(type, listModifier);
    EXPECT_FALSE(RSSystemProperties::GetSingleFrameComposerEnabled());
    rsCanvasRenderNode.ApplyDrawCmdModifier(modifierContext, type);
}

/**
 * @tc.name: InternalDrawContent
 * @tc.desc: test results of InternalDrawContent
 * @tc.type: FUNC
 * @tc.require: issueI9VPPN
 */
HWTEST_F(RSCanvasRenderNodeTest, InternalDrawContent, TestSize.Level1)
{
    NodeId nodeId = 0;
    std::weak_ptr<RSContext> context;
    RSCanvasRenderNode rsCanvasRenderNode(nodeId, context);
    rsCanvasRenderNode.InternalDrawContent(*canvas_, false);
    RSProperties* properties = const_cast<RSProperties*>(&rsCanvasRenderNode.GetRenderProperties());
    properties->SetClipToFrame(true);
    rsCanvasRenderNode.InternalDrawContent(*canvas_, false);
    EXPECT_TRUE(rsCanvasRenderNode.GetSortedChildren()->empty());

    auto rSCanvasRenderNodeChild = std::make_shared<RSCanvasRenderNode>(nodeId + 1);
    auto fullChildrenList = std::make_shared<std::vector<std::shared_ptr<RSRenderNode>>>();
    fullChildrenList->emplace_back(rSCanvasRenderNodeChild);
    rsCanvasRenderNode.fullChildrenList_ = std::move(fullChildrenList);
    EXPECT_TRUE(!rsCanvasRenderNode.GetSortedChildren()->empty());
    rsCanvasRenderNode.InternalDrawContent(*canvas_, false);
}

/**
 * @tc.name: QuickPrepare001
 * @tc.desc: test results of QuickPrepare
 * @tc.type: FUNC
 * @tc.require: issueI9VPPN
 */
HWTEST_F(RSCanvasRenderNodeTest, QuickPrepare001, TestSize.Level1)
{
    NodeId nodeId = 1;
    auto rsCanvasRenderNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    std::shared_ptr<RSNodeVisitor> visitor;
    EXPECT_TRUE(visitor == nullptr);
    rsCanvasRenderNode->QuickPrepare(visitor);

    visitor = std::make_shared<RSRenderThreadVisitor>();
    rsCanvasRenderNode->QuickPrepare(visitor);
}

/**
 * @tc.name: OpincGetNodeSupportFlag001
 * @tc.desc: test results of OpincGetNodeSupportFlag
 * @tc.type: FUNC
 * @tc.require: issueI9VPPN
 */
HWTEST_F(RSCanvasRenderNodeTest, OpincGetNodeSupportFlag001, TestSize.Level1)
{
    NodeId nodeId = 2;
    auto rsCanvasRenderNode = std::make_shared<RSCanvasRenderNode>(nodeId);

    EXPECT_TRUE(rsCanvasRenderNode->OpincGetNodeSupportFlag());

    rsCanvasRenderNode->childHasVisibleFilter_ = true;
    EXPECT_FALSE(rsCanvasRenderNode->OpincGetNodeSupportFlag());
    rsCanvasRenderNode->childHasVisibleFilter_ = false;
    EXPECT_TRUE(rsCanvasRenderNode->OpincGetNodeSupportFlag());

    rsCanvasRenderNode->childHasVisibleEffect_ = true;
    EXPECT_FALSE(rsCanvasRenderNode->OpincGetNodeSupportFlag());
    rsCanvasRenderNode->childHasVisibleEffect_ = false;
    EXPECT_TRUE(rsCanvasRenderNode->OpincGetNodeSupportFlag());

    auto& property = rsCanvasRenderNode->GetMutableRenderProperties();
    Color color(255, 0, 0);
    std::optional<Color> colorBlend = color;
    property.SetColorBlend(colorBlend);
    EXPECT_FALSE(rsCanvasRenderNode->OpincGetNodeSupportFlag());
}

/**
 * @tc.name: SetLinkedRootNodeId
 * @tc.desc: test SetLinkedRootNodeId
 * @tc.type: FUNC
 * @tc.require: #IBPVN9
 */
HWTEST_F(RSCanvasRenderNodeTest, SetLinkedRootNodeId, TestSize.Level1)
{
    NodeId nodeId = static_cast<NodeId>(2);
    NodeId rootNodeId = static_cast<NodeId>(3);
    auto rsCanvasRenderNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    EXPECT_EQ(rsCanvasRenderNode->GetLinkedRootNodeId(), INVALID_NODEID);

    rsCanvasRenderNode->SetLinkedRootNodeId(rootNodeId);
    EXPECT_EQ(rsCanvasRenderNode->GetLinkedRootNodeId(), rootNodeId);
}
} // namespace OHOS::Rosen