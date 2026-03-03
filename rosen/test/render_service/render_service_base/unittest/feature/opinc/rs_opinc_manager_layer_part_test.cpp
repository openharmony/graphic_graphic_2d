/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

/**
 * @tc.name: RSOpincManagerLayerPartBranchTest
 * @tc.desc: Branch coverage tests for layer part render manager (commit 266e787e55)
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */

#include "gtest/gtest.h"
#include "feature/opinc/rs_opinc_manager.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_dirty_region_manager.h"
#include "params/rs_render_params.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class RSOpincManagerLayerPartTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    RSOpincManager& opincManager_ = RSOpincManager::Instance();
};

void RSOpincManagerLayerPartTest::SetUpTestCase() {}
void RSOpincManagerLayerPartTest::TearDownTestCase() {}
void RSOpincManagerLayerPartTest::SetUp() {}
void RSOpincManagerLayerPartTest::TearDown() {}

/**
 * @tc.name: InitLayerPartRenderNodeDisabled
 * @tc.desc: Test InitLayerPartRenderNode when CCM layer part render is disabled
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, InitLayerPartRenderNodeDisabled, TestSize.Level1)
{
    NodeId id = 1;
    auto node = std::make_shared<RSCanvasRenderNode>(id);
    std::shared_ptr<RSDirtyRegionManager> layerPartRenderDirtyManager = nullptr;

    bool isCCMLayerPartRenderEnabled = false;
    opincManager_.InitLayerPartRenderNode(isCCMLayerPartRenderEnabled, *node, layerPartRenderDirtyManager);
    ASSERT_EQ(layerPartRenderDirtyManager, nullptr);
    ASSERT_FALSE(node->GetOpincCache().IsLayerPartRender());
}

/**
 * @tc.name: InitLayerPartRenderNodeNotSuggested
 * @tc.desc: Test InitLayerPartRenderNode when node is not suggested for layer part render
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, InitLayerPartRenderNodeNotSuggested, TestSize.Level1)
{
    NodeId id = 2;
    auto node = std::make_shared<RSCanvasRenderNode>(id);
    std::shared_ptr<RSDirtyRegionManager> layerPartRenderDirtyManager = nullptr;

    bool isCCMLayerPartRenderEnabled = true;
    opincManager_.InitLayerPartRenderNode(isCCMLayerPartRenderEnabled, *node, layerPartRenderDirtyManager);

    ASSERT_FALSE(node->GetOpincCache().IsSuggestLayerPartRenderNode());
    ASSERT_FALSE(node->GetOpincCache().IsLayerPartRender());
}

/**
 * @tc.name: InitLayerPartRenderNodeWithSuggested
 * @tc.desc: Test InitLayerPartRenderNode when node is suggested for layer part render
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, InitLayerPartRenderNodeWithSuggested, TestSize.Level1)
{
    NodeId id = 3;
    auto node = std::make_shared<RSCanvasRenderNode>(id);
    std::shared_ptr<RSDirtyRegionManager> layerPartRenderDirtyManager = nullptr;

    node->GetOpincCache().MarkSuggestLayerPartRenderNode(true);
    ASSERT_TRUE(node->GetOpincCache().IsSuggestLayerPartRenderNode());

    bool isCCMLayerPartRenderEnabled = true;
    opincManager_.InitLayerPartRenderNode(isCCMLayerPartRenderEnabled, *node, layerPartRenderDirtyManager);

    ASSERT_TRUE(node->GetOpincCache().IsLayerPartRender());
    ASSERT_NE(layerPartRenderDirtyManager, nullptr);
}

/**
 * @tc.name: InitLayerPartRenderNodeWithDirtyManagerClear
 * @tc.desc: Test InitLayerPartRenderNode when dirty manager already exists and Clear() is called
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, InitLayerPartRenderNodeWithDirtyManagerClear, TestSize.Level1)
{
    NodeId id = 4;
    auto node = std::make_shared<RSCanvasRenderNode>(id);
    node->GetOpincCache().MarkSuggestLayerPartRenderNode(true);

    auto& dirtyManager = node->GetOpincCache().GetLayerPartRenderDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);

    dirtyManager->MergeDirtyRect(RectI(0, 0, 100, 100));
    ASSERT_FALSE(dirtyManager->GetCurrentFrameDirtyRegion().IsEmpty());

    std::shared_ptr<RSDirtyRegionManager> layerPartRenderDirtyManager = nullptr;
    bool isCCMLayerPartRenderEnabled = true;
    opincManager_.InitLayerPartRenderNode(isCCMLayerPartRenderEnabled, *node, layerPartRenderDirtyManager);

    ASSERT_TRUE(node->GetOpincCache().IsLayerPartRender());
    ASSERT_NE(layerPartRenderDirtyManager, nullptr);
}

/**
 * @tc.name: CalculateLayerPartRenderDirtyRegionNullDirtyManager
 * @tc.desc: Test CalculateLayerPartRenderDirtyRegion with null dirty manager
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, CalculateLayerPartRenderDirtyRegionNullDirtyManager, TestSize.Level1)
{
    NodeId id = 10;
    auto node = std::make_shared<RSCanvasRenderNode>(id);
    node->stagingRenderParams_ = std::make_unique<RSRenderParams>(id);

    std::shared_ptr<RSDirtyRegionManager> layerPartRenderDirtyManager = nullptr;

    opincManager_.CalculateLayerPartRenderDirtyRegion(*node, layerPartRenderDirtyManager);

    ASSERT_EQ(layerPartRenderDirtyManager, nullptr);
}

/**
 * @tc.name: CalculateLayerPartRenderDirtyRegionNullStagingParams
 * @tc.desc: Test CalculateLayerPartRenderDirtyRegion with null staging params
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, CalculateLayerPartRenderDirtyRegionNullStagingParams, TestSize.Level1)
{
    NodeId id = 11;
    auto node = std::make_shared<RSCanvasRenderNode>(id);
    node->stagingRenderParams_ = nullptr;

    auto layerPartRenderDirtyManager = std::make_shared<RSDirtyRegionManager>(id);
    ASSERT_NE(layerPartRenderDirtyManager, nullptr);

    opincManager_.CalculateLayerPartRenderDirtyRegion(*node, layerPartRenderDirtyManager);

    ASSERT_NE(layerPartRenderDirtyManager, nullptr);
}

/**
 * @tc.name: CalculateLayerPartRenderDirtyRegionLayerPartDisabled
 * @tc.desc: Test CalculateLayerPartRenderDirtyRegion when layer part render is disabled
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, CalculateLayerPartRenderDirtyRegionLayerPartDisabled, TestSize.Level1)
{
    NodeId id = 12;
    auto node = std::make_shared<RSCanvasRenderNode>(id);
    node->stagingRenderParams_ = std::make_unique<RSRenderParams>(id);

    node->GetOpincCache().SetLayerPartRender(false);
    ASSERT_FALSE(node->GetOpincCache().IsLayerPartRender());

    auto layerPartRenderDirtyManager = std::make_shared<RSDirtyRegionManager>(id);
    ASSERT_NE(layerPartRenderDirtyManager, nullptr);

    opincManager_.CalculateLayerPartRenderDirtyRegion(*node, layerPartRenderDirtyManager);

    ASSERT_FALSE(node->stagingRenderParams_->GetLayerPartRenderEnabled());
}

/**
 * @tc.name: CalculateLayerPartRenderDirtyRegionWithDisableAnimation
 * @tc.desc: Test CalculateLayerPartRenderDirtyRegion when animation is disabled
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, CalculateLayerPartRenderDirtyRegionWithDisableAnimation, TestSize.Level1)
{
    NodeId id = 13;
    auto node = std::make_shared<RSCanvasRenderNode>(id);
    node->stagingRenderParams_ = std::make_unique<RSRenderParams>(id);
    node->GetOpincCache().SetLayerPartRender(true);

    auto layerPartRenderDirtyManager = std::make_shared<RSDirtyRegionManager>(id);
    ASSERT_NE(layerPartRenderDirtyManager, nullptr);

    opincManager_.CalculateLayerPartRenderDirtyRegion(*node, layerPartRenderDirtyManager);

    ASSERT_FALSE(node->stagingRenderParams_->GetLayerPartRenderEnabled());
}

/**
 * @tc.name: CalculateLayerPartRenderDirtyRegionNotGroupedByUser
 * @tc.desc: Test CalculateLayerPartRenderDirtyRegion when node is not grouped by user
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, CalculateLayerPartRenderDirtyRegionNotGroupedByUser, TestSize.Level1)
{
    NodeId id = 14;
    auto node = std::make_shared<RSCanvasRenderNode>(id);
    node->stagingRenderParams_ = std::make_unique<RSRenderParams>(id);
    node->GetOpincCache().SetLayerPartRender(true);
    node->GetOpincCache().MarkSuggestLayerPartRenderNode(true);

    ASSERT_NE(node->GetNodeGroupType(), RSRenderNode::NodeGroupType::GROUPED_BY_USER);

    auto layerPartRenderDirtyManager = std::make_shared<RSDirtyRegionManager>(id);
    ASSERT_NE(layerPartRenderDirtyManager, nullptr);

    opincManager_.CalculateLayerPartRenderDirtyRegion(*node, layerPartRenderDirtyManager);

    ASSERT_EQ(node->GetNodeGroupType(), RSRenderNode::NodeGroupType::GROUPED_BY_USER);
    ASSERT_TRUE(node->stagingRenderParams_->GetLayerPartRenderEnabled());
}

/**
 * @tc.name: CalculateLayerPartRenderDirtyRegionAlreadyGrouped
 * @tc.desc: Test CalculateLayerPartRenderDirtyRegion when node is already grouped by user
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, CalculateLayerPartRenderDirtyRegionAlreadyGrouped, TestSize.Level1)
{
    NodeId id = 15;
    auto node = std::make_shared<RSCanvasRenderNode>(id);
    node->stagingRenderParams_ = std::make_unique<RSRenderParams>(id);
    node->GetOpincCache().SetLayerPartRender(true);

    node->MarkNodeGroup(RSRenderNode::NodeGroupType::GROUPED_BY_USER, true, false);
    ASSERT_EQ(node->GetNodeGroupType(), RSRenderNode::NodeGroupType::GROUPED_BY_USER);

    auto layerPartRenderDirtyManager = std::make_shared<RSDirtyRegionManager>(id);
    ASSERT_NE(layerPartRenderDirtyManager, nullptr);

    opincManager_.CalculateLayerPartRenderDirtyRegion(*node, layerPartRenderDirtyManager);

    ASSERT_TRUE(node->stagingRenderParams_->GetLayerPartRenderEnabled());
}

/**
 * @tc.name: CalculateLayerPartRenderDirtyRegionUnchangeStateFalse
 * @tc.desc: Test CalculateLayerPartRenderDirtyRegion when unchange state is false
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, CalculateLayerPartRenderDirtyRegionUnchangeStateFalse, TestSize.Level1)
{
    NodeId id = 16;
    auto node = std::make_shared<RSCanvasRenderNode>(id);
    node->stagingRenderParams_ = std::make_unique<RSRenderParams>(id);
    node->GetOpincCache().SetLayerPartRender(true);
    node->GetOpincCache().MarkSuggestLayerPartRenderNode(true);

    node->MarkNodeGroup(RSRenderNode::NodeGroupType::GROUPED_BY_USER, true, false);

    auto layerPartRenderDirtyManager = std::make_shared<RSDirtyRegionManager>(id);
    ASSERT_NE(layerPartRenderDirtyManager, nullptr);

    opincManager_.CalculateLayerPartRenderDirtyRegion(*node, layerPartRenderDirtyManager);

    ASSERT_TRUE(node->stagingRenderParams_->GetLayerPartRenderEnabled());
}

/**
 * @tc.name: CalculateLayerPartRenderDirtyRegionUnchangeStateTrue
 * @tc.desc: Test CalculateLayerPartRenderDirtyRegion when unchange state is true
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, CalculateLayerPartRenderDirtyRegionUnchangeStateTrue, TestSize.Level1)
{
    NodeId id = 17;
    auto node = std::make_shared<RSCanvasRenderNode>(id);
    node->stagingRenderParams_ = std::make_unique<RSRenderParams>(id);
    node->GetOpincCache().SetLayerPartRender(true);
    node->GetOpincCache().MarkSuggestLayerPartRenderNode(true);
    node->MarkNodeGroup(RSRenderNode::NodeGroupType::GROUPED_BY_USER, true, false);

    auto layerPartRenderDirtyManager = std::make_shared<RSDirtyRegionManager>(id);
    ASSERT_NE(layerPartRenderDirtyManager, nullptr);

    for (int i = 0; i < 5; i++) {
        node->GetOpincCache().IsLayerPartRenderUnchangeState();
    }

    opincManager_.CalculateLayerPartRenderDirtyRegion(*node, layerPartRenderDirtyManager);

    ASSERT_TRUE(node->stagingRenderParams_->GetLayerPartRenderEnabled());
}

/**
 * @tc.name: CalculateLayerPartRenderDirtyRegionSuccess
 * @tc.desc: Test CalculateLayerPartRenderDirtyRegion success path
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, CalculateLayerPartRenderDirtyRegionSuccess, TestSize.Level1)
{
    NodeId id = 18;
    auto node = std::make_shared<RSCanvasRenderNode>(id);
    node->stagingRenderParams_ = std::make_unique<RSRenderParams>(id);
    node->GetOpincCache().SetLayerPartRender(true);
    node->GetOpincCache().MarkSuggestLayerPartRenderNode(true);
    node->MarkNodeGroup(RSRenderNode::NodeGroupType::GROUPED_BY_USER, true, false);

    auto layerPartRenderDirtyManager = std::make_shared<RSDirtyRegionManager>(id);
    ASSERT_NE(layerPartRenderDirtyManager, nullptr);

    opincManager_.CalculateLayerPartRenderDirtyRegion(*node, layerPartRenderDirtyManager);

    ASSERT_TRUE(node->stagingRenderParams_->GetLayerPartRenderEnabled());
}

/**
 * @tc.name: MarkSuggestLayerPartRenderNode001
 * @tc.desc: Test MarkSuggestLayerPartRenderNode with non-SURFACE node
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, MarkSuggestLayerPartRenderNode001, TestSize.Level1)
{
    NodeId id = 20;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(id);
    ASSERT_NE(canvasNode, nullptr);
    ASSERT_NE(canvasNode->GetType(), RSRenderNodeType::SURFACE_NODE);

    canvasNode->MarkSuggestLayerPartRenderNode(true);
    canvasNode->MarkSuggestLayerPartRenderNode(false);

    ASSERT_FALSE(canvasNode->GetOpincCache().IsSuggestLayerPartRenderNode());
}

/**
 * @tc.name: MarkSuggestLayerPartRenderNode002
 * @tc.desc: Test MarkSuggestLayerPartRenderNode with SURFACE node but no parent
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, MarkSuggestLayerPartRenderNode002, TestSize.Level1)
{
    NodeId id = 21;
    std::weak_ptr<RSContext> context;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    ASSERT_NE(surfaceNode, nullptr);
    ASSERT_EQ(surfaceNode->GetType(), RSRenderNodeType::SURFACE_NODE);

    surfaceNode->MarkSuggestLayerPartRenderNode(true);

    ASSERT_FALSE(surfaceNode->GetOpincCache().IsSuggestLayerPartRenderNode());
}

/**
 * @tc.name: MarkSuggestLayerPartRenderNode003
 * @tc.desc: Test MarkSuggestLayerPartRenderNode with SURFACE node but parent is not SURFACE
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, MarkSuggestLayerPartRenderNode003, TestSize.Level1)
{
    NodeId id = 22;
    std::weak_ptr<RSContext> context;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(id + 1);
    ASSERT_NE(surfaceNode, nullptr);
    ASSERT_NE(canvasNode, nullptr);

    surfaceNode->SetParent(canvasNode);

    surfaceNode->MarkSuggestLayerPartRenderNode(true);

    ASSERT_FALSE(surfaceNode->GetOpincCache().IsSuggestLayerPartRenderNode());
}

/**
 * @tc.name: MarkSuggestLayerPartRenderNode004
 * @tc.desc: Test MarkSuggestLayerPartRenderNode with proper hierarchy but no ground parent
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, MarkSuggestLayerPartRenderNode004, TestSize.Level1)
{
    NodeId id = 23;
    std::weak_ptr<RSContext> context;
    auto parentSurface = std::make_shared<RSSurfaceRenderNode>(id, context);
    auto childSurface = std::make_shared<RSSurfaceRenderNode>(id + 1, context);
    ASSERT_NE(parentSurface, nullptr);
    ASSERT_NE(childSurface, nullptr);

    childSurface->SetParent(parentSurface);

    childSurface->MarkSuggestLayerPartRenderNode(true);

    ASSERT_FALSE(childSurface->GetOpincCache().IsSuggestLayerPartRenderNode());
}

/**
 * @tc.name: MarkSuggestLayerPartRenderNode005
 * @tc.desc: Test MarkSuggestLayerPartRenderNode with proper hierarchy but ground parent is not CANVAS
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, MarkSuggestLayerPartRenderNode005, TestSize.Level1)
{
    NodeId id = 24;
    std::weak_ptr<RSContext> context;
    auto grandParentSurface = std::make_shared<RSSurfaceRenderNode>(id, context);
    auto parentSurface = std::make_shared<RSSurfaceRenderNode>(id + 1, context);
    auto childSurface = std::make_shared<RSSurfaceRenderNode>(id + 2, context);
    ASSERT_NE(grandParentSurface, nullptr);
    ASSERT_NE(parentSurface, nullptr);
    ASSERT_NE(childSurface, nullptr);

    parentSurface->SetParent(grandParentSurface);
    childSurface->SetParent(parentSurface);

    childSurface->MarkSuggestLayerPartRenderNode(true);

    ASSERT_FALSE(childSurface->GetOpincCache().IsSuggestLayerPartRenderNode());
}

/**
 * @tc.name: MarkSuggestLayerPartRenderNode006
 * @tc.desc: Test MarkSuggestLayerPartRenderNode with complete hierarchy and isLayerPartRender=true
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, MarkSuggestLayerPartRenderNode006, TestSize.Level1)
{
    NodeId id = 25;
    std::weak_ptr<RSContext> context;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(id);
    auto parentSurface = std::make_shared<RSSurfaceRenderNode>(id + 1, context);
    auto childSurface = std::make_shared<RSSurfaceRenderNode>(id + 2, context);
    ASSERT_NE(canvasNode, nullptr);
    ASSERT_NE(parentSurface, nullptr);
    ASSERT_NE(childSurface, nullptr);

    parentSurface->SetParent(canvasNode);
    childSurface->SetParent(parentSurface);

    // Before marking, groundParent (canvasNode) should not be suggested
    ASSERT_FALSE(canvasNode->GetOpincCache().IsSuggestLayerPartRenderNode());

    childSurface->MarkSuggestLayerPartRenderNode(true);

    // groundParent's (canvasNode's) opincCache should be marked, not childSurface's
    ASSERT_TRUE(canvasNode->GetOpincCache().IsSuggestLayerPartRenderNode());
    ASSERT_FALSE(childSurface->GetOpincCache().IsSuggestLayerPartRenderNode());
    // When isLayerPartRender=true, MarkNodeGroup should NOT be called
    ASSERT_NE(canvasNode->GetNodeGroupType(), RSRenderNode::NodeGroupType::GROUPED_BY_USER);
}

/**
 * @tc.name: MarkSuggestLayerPartRenderNode007
 * @tc.desc: Test MarkSuggestLayerPartRenderNode with complete hierarchy and isLayerPartRender=false
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, MarkSuggestLayerPartRenderNode007, TestSize.Level1)
{
    NodeId id = 26;
    std::weak_ptr<RSContext> context;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(id);
    auto parentSurface = std::make_shared<RSSurfaceRenderNode>(id + 1, context);
    auto childSurface = std::make_shared<RSSurfaceRenderNode>(id + 2, context);
    ASSERT_NE(canvasNode, nullptr);
    ASSERT_NE(parentSurface, nullptr);
    ASSERT_NE(childSurface, nullptr);

    parentSurface->SetParent(canvasNode);
    childSurface->SetParent(parentSurface);

    // Before marking, groundParent (canvasNode) should not be suggested
    ASSERT_FALSE(canvasNode->GetOpincCache().IsSuggestLayerPartRenderNode());

    childSurface->MarkSuggestLayerPartRenderNode(false);

    // groundParent's (canvasNode's) opincCache should be marked as false
    ASSERT_FALSE(canvasNode->GetOpincCache().IsSuggestLayerPartRenderNode());
    ASSERT_FALSE(childSurface->GetOpincCache().IsSuggestLayerPartRenderNode());
    // When isLayerPartRender=false, MarkNodeGroup should be called on groundParent
    ASSERT_EQ(canvasNode->GetNodeGroupType(), RSRenderNode::NodeGroupType::GROUPED_BY_USER);
}

/**
 * @tc.name: UpdateLayerPartRenderDirtyRegion001
 * @tc.desc: Test UpdateLayerPartRenderDirtyRegion with null dirty manager
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, UpdateLayerPartRenderDirtyRegion001, TestSize.Level1)
{
    NodeId id = 30;
    std::weak_ptr<RSContext> context;
    auto node = std::make_shared<RSCanvasRenderNode>(id, context);
    std::shared_ptr<RSDirtyRegionManager> dirtyManager = nullptr;

    bool result = node->UpdateLayerPartRenderDirtyRegion(dirtyManager);

    ASSERT_FALSE(result);
}

/**
 * @tc.name: UpdateLayerPartRenderDirtyRegion002
 * @tc.desc: Test UpdateLayerPartRenderDirtyRegion with flag false
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, UpdateLayerPartRenderDirtyRegion002, TestSize.Level1)
{
    NodeId id = 31;
    std::weak_ptr<RSContext> context;
    auto node = std::make_shared<RSCanvasRenderNode>(id, context);
    auto dirtyManager = std::make_shared<RSDirtyRegionManager>();
    ASSERT_NE(dirtyManager, nullptr);

    node->layerPartRenderDirtyFlag_ = false;
    bool result = node->UpdateLayerPartRenderDirtyRegion(dirtyManager);

    ASSERT_TRUE(result);
}

/**
 * @tc.name: UpdateLayerPartRenderDirtyRegion003
 * @tc.desc: Test UpdateLayerPartRenderDirtyRegion with flag true
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, UpdateLayerPartRenderDirtyRegion003, TestSize.Level1)
{
    NodeId id = 32;
    std::weak_ptr<RSContext> context;
    auto node = std::make_shared<RSCanvasRenderNode>(id, context);
    auto dirtyManager = std::make_shared<RSDirtyRegionManager>();
    ASSERT_NE(dirtyManager, nullptr);

    node->absDrawRect_ = RectI(10, 10, 100, 100);
    node->layerPartRenderDirtyFlag_ = true;

    bool result = node->UpdateLayerPartRenderDirtyRegion(dirtyManager);

    ASSERT_TRUE(result);
    ASSERT_FALSE(dirtyManager->GetCurrentFrameDirtyRegion().IsEmpty());
}

} // namespace OHOS::Rosen
