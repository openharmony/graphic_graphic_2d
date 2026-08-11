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

#include "gtest/gtest.h"
#include "pipeline/layer_split/rs_layer_split_manager.h"
#include "pipeline/layer_split/splitter/opinc/rs_opinc_layer_splitter_planner.h"
#include "pipeline/layer_split/splitter/opinc/rs_opinc_layer_splitter_processor.h"
#include "pipeline/layer_split/surface/rs_split_surface.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_test_util.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "modifier/rs_render_property.h"
#include "modifier_ng/rs_render_modifier_ng.h"
#include "transaction/rs_transaction_data.h"
#include "command/rs_delegate_composite_command.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class RSLayerSplitManagerTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override
    {
        ResetSingletonState();
    }
    void TearDown() override
    {
        ResetSingletonState();
    }
 
private:
    void ResetSingletonState()
    {
        auto& mgr = *RSLayerSplitManager::GetInstance();
        mgr.selectorVec_.clear();
        mgr.plannerMap_.clear();
        mgr.processorMap_.clear();
    }
    int testNodeId = 0;
    RSScreenRenderNode screenNode_{9999, 0};
    std::shared_ptr<RSRenderNode> CreateSurfaceNode()
    {
        return std::make_shared<RSRenderNode>(testNodeId++);
    }
};

/**
 * @tc.name: GetInstance001
 * @tc.desc: Check if RSLayerSplitManager GetInstance returns valid instance
 * @tc.type: FUNC
 */
HWTEST_F(RSLayerSplitManagerTest, GetInstance001, TestSize.Level1)
{
    auto instance1 = RSLayerSplitManager::GetInstance();
    EXPECT_NE(instance1, nullptr);
    auto instance2 = RSLayerSplitManager::GetInstance();
    EXPECT_EQ(instance1, instance2);
    EXPECT_TRUE(instance1->plannerMap_.empty());
    EXPECT_TRUE(instance1->processorMap_.empty());
}

/**
 * @tc.name: SetEnabled001
 * @tc.desc: Check if SetEnabled works correctly
 * @tc.type: FUNC
 */
HWTEST_F(RSLayerSplitManagerTest, SetEnabled001, TestSize.Level1)
{
    auto instance = RSLayerSplitManager::GetInstance();
    instance->isGlobalEnabled_ = true;
    instance->SetEnabled(true);
    instance->Reset();
    EXPECT_FALSE(instance->selectorVec_.empty());
    instance->SetEnabled(false);
    instance->Reset();
    EXPECT_TRUE(instance->selectorVec_.empty());
}

/**
 * @tc.name: Reset001
 * @tc.desc: Check if Reset works correctly
 * @tc.type: FUNC
 */
HWTEST_F(RSLayerSplitManagerTest, Reset001, TestSize.Level1)
{
    auto instance = RSLayerSplitManager::GetInstance();
    instance->isGlobalEnabled_ = true;
    instance->SetEnabled(true);
    instance->Reset();
    EXPECT_TRUE(instance->plannerMap_.empty());
    EXPECT_TRUE(instance->processorMap_.empty());
}

/**
 * @tc.name: MoveSplitSurfaceNode001
 * @tc.desc: Check if MoveSplitSurfaceNode works correctly
 * @tc.type: FUNC
 */
HWTEST_F(RSLayerSplitManagerTest, MoveSplitSurfaceNode001, TestSize.Level1)
{
    auto instance = RSLayerSplitManager::GetInstance();
    instance->isGlobalEnabled_ = false;
    instance->MoveSplitSurfaceNode();
    EXPECT_TRUE(instance->plannerMap_.empty());
    EXPECT_TRUE(instance->processorMap_.empty());
}

/**
 * @tc.name: MoveSplitSurfaceNode002
 * @tc.desc: Check if MoveSplitSurfaceNode works with populated plannerMap
 * @tc.type: FUNC
 */
HWTEST_F(RSLayerSplitManagerTest, MoveSplitSurfaceNode002, TestSize.Level1)
{
    auto instance = RSLayerSplitManager::GetInstance();
    instance->isGlobalEnabled_ = true;
    instance->SetEnabled(true);
    instance->Reset();

    auto parent = CreateSurfaceNode();
    auto child = CreateSurfaceNode();
    parent->AddChild(child);
    child->GetOpincRootCache().isOpincRootFlag_ = true;
    child->GetMutableRenderProperties().curGeoDirty_ = true;

    instance->RecordSplitNode(child);
    instance->CheckNeedLeave(screenNode_);

    EXPECT_FALSE(instance->plannerMap_.empty());
    instance->MoveSplitSurfaceNode();

    instance->Reset();
}

/**
 * @tc.name: RecordSplitNode001
 * @tc.desc: Check if RecordSplitNode works with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSLayerSplitManagerTest, RecordSplitNode001, TestSize.Level1)
{
    auto instance = RSLayerSplitManager::GetInstance();
    instance->isGlobalEnabled_ = true;
    instance->SetEnabled(true);
    instance->Reset();
    instance->RecordSplitNode(nullptr);
    EXPECT_TRUE(instance->plannerMap_.empty());
    instance->Reset();
}

/**
 * @tc.name: RecordSplitNode002
 * @tc.desc: Check if RecordSplitNode works with valid node
 * @tc.type: FUNC
 */
HWTEST_F(RSLayerSplitManagerTest, RecordSplitNode002, TestSize.Level1)
{
    auto instance = RSLayerSplitManager::GetInstance();
    auto surfaceNode = CreateSurfaceNode();
    EXPECT_NE(surfaceNode, nullptr);
    surfaceNode->GetOpincRootCache().isOpincRootFlag_ = false;
    instance->RecordSplitNode(surfaceNode);
    EXPECT_TRUE(instance->plannerMap_.empty());
}

/**
 * @tc.name: RecordSplitNode003
 * @tc.desc: Check if RecordSplitNode works with valid node and populated selectorVec
 * @tc.type: FUNC
 */
HWTEST_F(RSLayerSplitManagerTest, RecordSplitNode003, TestSize.Level1)
{
    auto instance = RSLayerSplitManager::GetInstance();
    instance->isGlobalEnabled_ = true;
    instance->SetEnabled(true);
    instance->Reset();

    auto parent = CreateSurfaceNode();
    auto child = CreateSurfaceNode();
    ASSERT_NE(parent, nullptr);
    ASSERT_NE(child, nullptr);
    parent->AddChild(child);
    child->GetOpincRootCache().isOpincRootFlag_ = true;
    child->GetMutableRenderProperties().curGeoDirty_ = true;
    instance->RecordSplitNode(child);
    instance->CheckNeedLeave(screenNode_);

    EXPECT_FALSE(instance->plannerMap_.empty());
    instance->Reset();
}

/**
 * @tc.name: CheckNeedLeave001
 * @tc.desc: Check if CheckNeedLeave works correctly
 * @tc.type: FUNC
 */
HWTEST_F(RSLayerSplitManagerTest, CheckNeedLeave001, TestSize.Level1)
{
    auto instance = RSLayerSplitManager::GetInstance();
    instance->isGlobalEnabled_ = false;
    instance->SetEnabled(false);
    instance->CheckNeedLeave(screenNode_);
    EXPECT_TRUE(instance->plannerMap_.empty());
    EXPECT_TRUE(instance->processorMap_.empty());
}

/**
 * @tc.name: CheckNeedLeaveWithParentChild001
 * @tc.desc: Check if CheckNeedLeave creates planner/processor with parent-child nodes
 * @tc.type: FUNC
 */
HWTEST_F(RSLayerSplitManagerTest, CheckNeedLeaveWithParentChild001, TestSize.Level1)
{
    auto instance = RSLayerSplitManager::GetInstance();
    instance->isGlobalEnabled_ = true;
    instance->SetEnabled(true);
    instance->Reset();

    auto parent = CreateSurfaceNode();
    auto child = CreateSurfaceNode();
    ASSERT_NE(parent, nullptr);
    ASSERT_NE(child, nullptr);

    parent->AddChild(child);

    child->GetOpincRootCache().isOpincRootFlag_ = true;
    child->GetMutableRenderProperties().curGeoDirty_ = true;

    instance->RecordSplitNode(child);
    instance->CheckNeedLeave(screenNode_);

    EXPECT_FALSE(instance->plannerMap_.empty());
    EXPECT_FALSE(instance->processorMap_.empty());
    instance->Reset();
}

/**
 * @tc.name: CheckNeedLeaveWithMultipleChildren001
 * @tc.desc: Check if CheckNeedLeave works with multiple children under same parent
 * @tc.type: FUNC
 */
HWTEST_F(RSLayerSplitManagerTest, CheckNeedLeaveWithMultipleChildren001, TestSize.Level1)
{
    auto instance = RSLayerSplitManager::GetInstance();
    instance->isGlobalEnabled_ = true;
    instance->SetEnabled(true);
    instance->Reset();

    auto parent = CreateSurfaceNode();
    auto child1 = CreateSurfaceNode();
    auto child2 = CreateSurfaceNode();
    ASSERT_NE(parent, nullptr);
    ASSERT_NE(child1, nullptr);
    ASSERT_NE(child2, nullptr);

    parent->AddChild(child1);
    parent->AddChild(child2);

    child1->GetOpincRootCache().isOpincRootFlag_ = true;
    child1->GetMutableRenderProperties().curGeoDirty_ = true;
    child2->GetOpincRootCache().isOpincRootFlag_ = true;
    child2->GetMutableRenderProperties().curGeoDirty_ = true;

    instance->RecordSplitNode(child1);
    instance->RecordSplitNode(child2);
    instance->CheckNeedLeave(screenNode_);

    EXPECT_FALSE(instance->plannerMap_.empty());
    EXPECT_EQ(instance->plannerMap_.size(), 1);

    instance->Reset();
}

/**
 * @tc.name: CheckNeedLeaveParentChange001
 * @tc.desc: Check if CheckNeedLeave handles parent node change correctly
 * @tc.type: FUNC
 */
HWTEST_F(RSLayerSplitManagerTest, CheckNeedLeaveParentChange001, TestSize.Level1)
{
    auto instance = RSLayerSplitManager::GetInstance();
    instance->isGlobalEnabled_ = true;
    instance->SetEnabled(true);
    instance->Reset();

    auto parent1 = CreateSurfaceNode();
    auto parent2 = CreateSurfaceNode();
    auto child1 = CreateSurfaceNode();
    auto child2 = CreateSurfaceNode();
    ASSERT_NE(parent1, nullptr);
    ASSERT_NE(parent2, nullptr);
    ASSERT_NE(child1, nullptr);
    ASSERT_NE(child2, nullptr);

    parent1->AddChild(child1);
    child1->GetOpincRootCache().isOpincRootFlag_ = true;
    child1->GetMutableRenderProperties().curGeoDirty_ = true;

    instance->RecordSplitNode(child1);
    instance->CheckNeedLeave(screenNode_);

    EXPECT_EQ(instance->plannerMap_.size(), 1);
    EXPECT_NE(instance->plannerMap_.find(parent1), instance->plannerMap_.end());

    parent2->AddChild(child2);
    child2->GetOpincRootCache().isOpincRootFlag_ = true;
    child2->GetMutableRenderProperties().curGeoDirty_ = true;

    instance->RecordSplitNode(child2);
    instance->CheckNeedLeave(screenNode_);

    EXPECT_EQ(instance->plannerMap_.size(), 2);
    EXPECT_NE(instance->plannerMap_.find(parent2), instance->plannerMap_.end());

    instance->Reset();
}

/**
 * @tc.name: CheckSplitNodeIntersectFilter001
 * @tc.desc: Check if CheckSplitNodeIntersectFilter works with null hwcNode
 * @tc.type: FUNC
 */
HWTEST_F(RSLayerSplitManagerTest, CheckSplitNodeIntersectFilter001, TestSize.Level1)
{
    auto instance = RSLayerSplitManager::GetInstance();
    instance->isGlobalEnabled_ = true;
    instance->SetEnabled(true);
    instance->Reset();
    instance->CheckSplitNodeIntersectFilter(nullptr);
    EXPECT_TRUE(instance->plannerMap_.empty());
    instance->Reset();
}

/**
 * @tc.name: CheckSplitNodeIntersectFilter002
 * @tc.desc: Check if CheckSplitNodeIntersectFilter works with valid hwcNode
 * @tc.type: FUNC
 */
HWTEST_F(RSLayerSplitManagerTest, CheckSplitNodeIntersectFilter002, TestSize.Level1)
{
    auto instance = RSLayerSplitManager::GetInstance();
    auto hwcNode = RSTestUtil::CreateSurfaceNode();
    EXPECT_NE(hwcNode, nullptr);
    instance->isGlobalEnabled_ = false;
    instance->CheckSplitNodeIntersectFilter(hwcNode);
    EXPECT_TRUE(instance->plannerMap_.empty());
}

/**
 * @tc.name: CheckSplitNodeIntersectFilter003
 * @tc.desc: Check if CheckSplitNodeIntersectFilter works with populated plannerMap
 * @tc.type: FUNC
 */
HWTEST_F(RSLayerSplitManagerTest, CheckSplitNodeIntersectFilter003, TestSize.Level1)
{
    auto instance = RSLayerSplitManager::GetInstance();
    instance->isGlobalEnabled_ = true;
    instance->SetEnabled(true);
    instance->Reset();

    auto parent = CreateSurfaceNode();
    auto child = CreateSurfaceNode();
    parent->AddChild(child);
    child->GetOpincRootCache().isOpincRootFlag_ = true;
    child->GetMutableRenderProperties().curGeoDirty_ = true;

    instance->RecordSplitNode(child);
    instance->CheckNeedLeave(screenNode_);

    EXPECT_FALSE(instance->plannerMap_.empty());
    auto hwcNode = RSTestUtil::CreateSurfaceNode();
    EXPECT_NE(hwcNode, nullptr);
    instance->CheckSplitNodeIntersectFilter(hwcNode);

    instance->Reset();
}

/**
 * @tc.name: UpdatePlanAndDirtyRegion001
 * @tc.desc: Check if UpdatePlanAndDirtyRegion works correctly
 * @tc.type: FUNC
 */
HWTEST_F(RSLayerSplitManagerTest, UpdatePlanAndDirtyRegion001, TestSize.Level1)
{
    auto instance = RSLayerSplitManager::GetInstance();
    instance->isGlobalEnabled_ = true;
    instance->SetEnabled(true);
    instance->Reset();
    instance->UpdatePlanAndDirtyRegion(screenNode_, nullptr);
    EXPECT_TRUE(instance->plannerMap_.empty());
    instance->Reset();
}

/**
 * @tc.name: UpdatePlanAndDirtyRegion002
 * @tc.desc: Check if UpdatePlanAndDirtyRegion works with populated plannerMap
 * @tc.type: FUNC
 */
HWTEST_F(RSLayerSplitManagerTest, UpdatePlanAndDirtyRegion002, TestSize.Level1)
{
    auto instance = RSLayerSplitManager::GetInstance();
    instance->isGlobalEnabled_ = true;
    instance->SetEnabled(true);
    instance->Reset();

    auto parent = CreateSurfaceNode();
    auto child = CreateSurfaceNode();
    parent->AddChild(child);
    child->GetOpincRootCache().isOpincRootFlag_ = true;
    child->GetMutableRenderProperties().curGeoDirty_ = true;

    instance->RecordSplitNode(child);
    instance->CheckNeedLeave(screenNode_);

    EXPECT_FALSE(instance->plannerMap_.empty());
    instance->UpdatePlanAndDirtyRegion(screenNode_, nullptr);

    instance->Reset();
}

/**
 * @tc.name: Sync001
 * @tc.desc: Check if Sync works correctly
 * @tc.type: FUNC
 */
HWTEST_F(RSLayerSplitManagerTest, Sync001, TestSize.Level1)
{
    auto instance = RSLayerSplitManager::GetInstance();
    instance->isGlobalEnabled_ = true;
    instance->SetEnabled(true);
    instance->Reset();
    instance->Sync();
    EXPECT_TRUE(instance->plannerMap_.empty());
    instance->Reset();
}

/**
 * @tc.name: Sync002
 * @tc.desc: Check if Sync works with populated plannerMap
 * @tc.type: FUNC
 */
HWTEST_F(RSLayerSplitManagerTest, Sync002, TestSize.Level1)
{
    auto instance = RSLayerSplitManager::GetInstance();
    instance->isGlobalEnabled_ = true;
    instance->SetEnabled(true);
    instance->Reset();

    auto parent = CreateSurfaceNode();
    auto child = CreateSurfaceNode();
    parent->AddChild(child);
    child->GetOpincRootCache().isOpincRootFlag_ = true;
    child->GetMutableRenderProperties().curGeoDirty_ = true;

    instance->RecordSplitNode(child);
    instance->CheckNeedLeave(screenNode_);

    EXPECT_FALSE(instance->plannerMap_.empty());
    instance->Sync();

    instance->Reset();
}

/**
 * @tc.name: CheckDoDirectCompositionWithSplitLayer001
 * @tc.desc: Check if CheckDoDirectCompositionWithSplitLayer returns false when doDirectComposition is false
 * @tc.type: FUNC
 */
HWTEST_F(RSLayerSplitManagerTest, CheckDoDirectCompositionWithSplitLayer001, TestSize.Level1)
{
    auto instance = RSLayerSplitManager::GetInstance();
    instance->isGlobalEnabled_ = true;
    instance->SetEnabled(true);
    instance->Reset();
    bool result = instance->CheckDoDirectCompositionWithSplitLayer();
    EXPECT_FALSE(result);
    EXPECT_TRUE(instance->plannerMap_.empty());
    instance->Reset();
}

/**
 * @tc.name: CheckDoDirectCompositionWithSplitLayer002
 * @tc.desc: Check if CheckDoDirectCompositionWithSplitLayer returns false when plannerMap is empty
 * @tc.type: FUNC
 */
HWTEST_F(RSLayerSplitManagerTest, CheckDoDirectCompositionWithSplitLayer002, TestSize.Level1)
{
    auto instance = RSLayerSplitManager::GetInstance();
    instance->isGlobalEnabled_ = true;
    instance->SetEnabled(true);
    instance->Reset();
    bool result = instance->CheckDoDirectCompositionWithSplitLayer();
    EXPECT_FALSE(result);
    EXPECT_TRUE(instance->plannerMap_.empty());
    instance->Reset();
}

/**
 * @tc.name: CheckDoDirectCompositionWithSplitLayer003
 * @tc.desc: Check if CheckDoDirectCompositionWithSplitLayer works with populated plannerMap
 * @tc.type: FUNC
 */
HWTEST_F(RSLayerSplitManagerTest, CheckDoDirectCompositionWithSplitLayer003, TestSize.Level1)
{
    auto instance = RSLayerSplitManager::GetInstance();
    instance->isGlobalEnabled_ = true;
    instance->SetEnabled(true);
    instance->Reset();

    auto parent = CreateSurfaceNode();
    auto child = CreateSurfaceNode();
    parent->AddChild(child);
    child->GetOpincRootCache().isOpincRootFlag_ = true;
    child->GetMutableRenderProperties().curGeoDirty_ = true;

    instance->RecordSplitNode(child);
    instance->CheckNeedLeave(screenNode_);

    EXPECT_FALSE(instance->plannerMap_.empty());
    bool result = instance->CheckDoDirectCompositionWithSplitLayer();
    EXPECT_FALSE(result);
    instance->Reset();
}

/**
 * @tc.name: InitSplitSurface001
 * @tc.desc: Check if InitSplitSurface works correctly
 * @tc.type: FUNC
 */
HWTEST_F(RSLayerSplitManagerTest, InitSplitSurface001, TestSize.Level1)
{
    auto instance = RSLayerSplitManager::GetInstance();
    instance->isGlobalEnabled_ = true;
    instance->SetEnabled(true);
    instance->Reset();
    ScreenInfo screenInfo;
    screenInfo.width = 1080;
    screenInfo.height = 1920;
    instance->InitSplitSurface(screenInfo);
    EXPECT_TRUE(instance->plannerMap_.empty());
    instance->Reset();
}

/**
 * @tc.name: InitSplitSurface002
 * @tc.desc: Check if InitSplitSurface works with populated plannerMap
 * @tc.type: FUNC
 */
HWTEST_F(RSLayerSplitManagerTest, InitSplitSurface002, TestSize.Level1)
{
    auto instance = RSLayerSplitManager::GetInstance();
    instance->isGlobalEnabled_ = true;
    instance->SetEnabled(true);
    instance->Reset();

    auto parent = CreateSurfaceNode();
    auto child = CreateSurfaceNode();
    parent->AddChild(child);
    child->GetOpincRootCache().isOpincRootFlag_ = true;
    child->GetMutableRenderProperties().curGeoDirty_ = true;

    instance->RecordSplitNode(child);
    instance->CheckNeedLeave(screenNode_);

    EXPECT_FALSE(instance->plannerMap_.empty());
    ScreenInfo screenInfo;
    instance->InitSplitSurface(screenInfo);

    instance->Reset();
}

/**
 * @tc.name: CheckOpIncNodeFromCommand001
 * @tc.desc: Check if CheckOpIncNodeFromCommand returns false when plannerMap is empty
 * @tc.type: FUNC
 */
HWTEST_F(RSLayerSplitManagerTest, CheckOpIncNodeFromCommand001, TestSize.Level1)
{
    auto instance = RSLayerSplitManager::GetInstance();
    instance->isGlobalEnabled_ = true;
    instance->SetEnabled(true);
    instance->Reset();
    EXPECT_TRUE(instance->plannerMap_.empty());
    std::unique_ptr<RSTransactionData> transactionData = std::make_unique<RSTransactionData>();
    auto result = instance->CheckOpIncNodeFromCommand(transactionData);
    EXPECT_FALSE(result);
    instance->Reset();
}

/**
 * @tc.name: CheckOpIncNodeFromCommand001
 * @tc.desc: Check if CheckOpIncNodeFromCommand returns false when transactionData is empty
 * @tc.type: FUNC
 */
HWTEST_F(RSLayerSplitManagerTest, CheckOpIncNodeFromCommand002, TestSize.Level1)
{
    auto instance = RSLayerSplitManager::GetInstance();
    instance->isGlobalEnabled_ = true;
    instance->SetEnabled(true);
    instance->Reset();
    EXPECT_TRUE(instance->plannerMap_.empty());
    std::unique_ptr<RSTransactionData> transactionData = nullptr;
    auto result = instance->CheckOpIncNodeFromCommand(transactionData);
    EXPECT_FALSE(result);
    instance->Reset();
}

/**
 * @tc.name: Reset_DisabledWithSelectors
 * @tc.desc: Test Reset when isEnabled_=false and selectorVec_ is not empty
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLayerSplitManagerTest, Reset_DisabledWithSelectors, TestSize.Level1)
{
    auto instance = RSLayerSplitManager::GetInstance();
    instance->isGlobalEnabled_ = true;
    instance->SetEnabled(true);
    instance->Reset(); // enable + empty selector → adds selector
    EXPECT_FALSE(instance->selectorVec_.empty());
    instance->SetEnabled(false);
    instance->Reset(); // disabled + non-empty selector → clears selector
    EXPECT_TRUE(instance->selectorVec_.empty());
}

/**
 * @tc.name: CheckNeedLeave_ParentNodeNull
 * @tc.desc: Test CheckNeedLeave when selector->SelectParentNode() returns nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLayerSplitManagerTest, CheckNeedLeave_ParentNodeNull, TestSize.Level1)
{
    auto instance = RSLayerSplitManager::GetInstance();
    instance->isGlobalEnabled_ = true;
    instance->SetEnabled(true);
    instance->Reset(); // adds a selector with empty recorded nodes

    // selector has no recorded nodes → SelectParentNode() returns nullptr → continue
    instance->CheckNeedLeave(screenNode_);
    EXPECT_TRUE(instance->plannerMap_.empty());
    EXPECT_TRUE(instance->processorMap_.empty());
}

/**
 * @tc.name: CheckNeedLeave_ParentChanged_UnregisterPlanner
 * @tc.desc: Test CheckNeedLeave when parent changes, planner exists, planner->second is valid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLayerSplitManagerTest, CheckNeedLeave_ParentChanged_UnregisterPlanner, TestSize.Level1)
{
    auto instance = RSLayerSplitManager::GetInstance();
    instance->isGlobalEnabled_ = true;
    instance->SetEnabled(true);
    instance->Reset();

    // First call: record child1 under parent1 → creates planner for parent1
    auto parent1 = CreateSurfaceNode();
    auto child1 = CreateSurfaceNode();
    parent1->AddChild(child1);
    child1->GetOpincRootCache().isOpincRootFlag_ = true;
    child1->GetMutableRenderProperties().curGeoDirty_ = true;
    instance->RecordSplitNode(child1);
    instance->CheckNeedLeave(screenNode_);
    EXPECT_FALSE(instance->plannerMap_.empty());
    EXPECT_NE(instance->plannerMap_.find(parent1), instance->plannerMap_.end());

    // Remove child1 from parent1, add to parent2 → parent change triggers Unregister
    parent1->RemoveChild(child1);
    auto parent2 = CreateSurfaceNode();
    parent2->AddChild(child1);
    instance->RecordSplitNode(child1);
    instance->CheckNeedLeave(screenNode_);

    EXPECT_NE(instance->plannerMap_.find(parent1), instance->plannerMap_.end());
    EXPECT_NE(instance->plannerMap_.find(parent2), instance->plannerMap_.end());
}

/**
 * @tc.name: CheckNeedLeave_PlannerAlreadyExists
 * @tc.desc: Test CheckNeedLeave when planner for the node already exists in map
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLayerSplitManagerTest, CheckNeedLeave_PlannerAlreadyExists, TestSize.Level1)
{
    auto instance = RSLayerSplitManager::GetInstance();
    instance->isGlobalEnabled_ = true;
    instance->SetEnabled(true);
    instance->Reset();

    auto parent = CreateSurfaceNode();
    auto child = CreateSurfaceNode();
    parent->AddChild(child);
    child->GetOpincRootCache().isOpincRootFlag_ = true;
    child->GetMutableRenderProperties().curGeoDirty_ = true;

    // First CheckNeedLeave creates the planner
    instance->RecordSplitNode(child);
    instance->CheckNeedLeave(screenNode_);
    EXPECT_EQ(instance->plannerMap_.size(), 1);

    // Second CheckNeedLeave: planner already exists → skips creation
    instance->RecordSplitNode(child);
    instance->CheckNeedLeave(screenNode_);
    EXPECT_EQ(instance->plannerMap_.size(), 1);
}

/**
 * @tc.name: Sync_UnregisterSurface
 * @tc.desc: Test Sync when planner->GetSurfaceStatus() == UNREGISTER
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLayerSplitManagerTest, Sync_UnregisterSurface, TestSize.Level1)
{
    auto instance = RSLayerSplitManager::GetInstance();
    auto planner = std::make_shared<RSOpincLayerSplitterPlanner>();
    auto processor = std::make_shared<RSOpincLayerSplitterProcessor>();
    auto parent = CreateSurfaceNode();

    planner->splitSurface_ = std::make_shared<SplitSurface>(100, 100);
    planner->splitSurface_->surfaceStatus_ = SurfaceStatus::UNREGISTER;
    planner->splitSurface_->splitSurfaceNode_ = RSTestUtil::CreateSurfaceNode();
    instance->plannerMap_[parent] = planner;
    instance->processorMap_[parent] = processor;

    instance->Sync();
    EXPECT_TRUE(instance->plannerMap_.empty());
    EXPECT_TRUE(instance->processorMap_.empty());
}

/**
 * @tc.name: Sync_ParentNotFound
 * @tc.desc: Test Sync when processor not found in processorMap_
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLayerSplitManagerTest, Sync_ParentNotFound, TestSize.Level1)
{
    auto instance = RSLayerSplitManager::GetInstance();
    auto planner = std::make_shared<RSOpincLayerSplitterPlanner>();
    auto processor = std::make_shared<RSOpincLayerSplitterProcessor>();
    auto parent = CreateSurfaceNode();

    planner->splitSurface_ = std::make_shared<SplitSurface>(100, 100);
    planner->splitSurface_->splitSurfaceNode_ = RSTestUtil::CreateSurfaceNode();
    instance->plannerMap_[parent] = planner;
    instance->processorMap_[parent] = processor;

    instance->Sync();
    EXPECT_FALSE(instance->plannerMap_.empty());
}

/**
 * @tc.name: Sync_ParentNull
 * @tc.desc: Test Sync when parentIt->second is nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLayerSplitManagerTest, Sync_ParentNull, TestSize.Level1)
{
    auto instance = RSLayerSplitManager::GetInstance();
    auto planner = std::make_shared<RSOpincLayerSplitterPlanner>();
    auto processor = std::make_shared<RSOpincLayerSplitterProcessor>();
    auto parent = CreateSurfaceNode();

    planner->splitSurface_ = std::make_shared<SplitSurface>(100, 100);
    planner->splitSurface_->splitSurfaceNode_ = RSTestUtil::CreateSurfaceNode();
    instance->plannerMap_[parent] = planner;
    instance->processorMap_[parent] = processor;

    instance->Sync();
    EXPECT_FALSE(instance->plannerMap_.empty());
}

/**
 * @tc.name: Sync_ProcessorNotFound
 * @tc.desc: Test Sync when processorIt == processorMap_.end()
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLayerSplitManagerTest, Sync_ProcessorNotFound, TestSize.Level1)
{
    auto instance = RSLayerSplitManager::GetInstance();
    auto planner = std::make_shared<RSOpincLayerSplitterPlanner>();
    auto parent = CreateSurfaceNode();

    planner->splitSurface_ = std::make_shared<SplitSurface>(100, 100);
    planner->splitSurface_->splitSurfaceNode_ = RSTestUtil::CreateSurfaceNode();
    instance->plannerMap_[parent] = planner;
    // processorMap_ intentionally missing entry for parent

    instance->Sync();
    EXPECT_FALSE(instance->plannerMap_.empty());
}

/**
 * @tc.name: Sync_ProcessorNull
 * @tc.desc: Test Sync when processorIt->second is nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLayerSplitManagerTest, Sync_ProcessorNull, TestSize.Level1)
{
    auto instance = RSLayerSplitManager::GetInstance();
    auto planner = std::make_shared<RSOpincLayerSplitterPlanner>();
    auto parent = CreateSurfaceNode();

    planner->splitSurface_ = std::make_shared<SplitSurface>(100, 100);
    planner->splitSurface_->splitSurfaceNode_ = RSTestUtil::CreateSurfaceNode();
    instance->plannerMap_[parent] = planner;
    instance->processorMap_[parent] = nullptr; // processor is null

    instance->Sync();
    EXPECT_FALSE(instance->plannerMap_.empty());
}

/**
 * @tc.name: CheckOpIncNodeFromCommand_WithPlannerAndNullCommand
 * @tc.desc: Test CheckOpIncNodeFromCommand with non-empty plannerMap and null command in payload
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLayerSplitManagerTest, CheckOpIncNodeFromCommand_WithPlannerAndNullCommand, TestSize.Level1)
{
    auto instance = RSLayerSplitManager::GetInstance();
    auto planner = std::make_shared<RSOpincLayerSplitterPlanner>();
    auto parent = CreateSurfaceNode();

    planner->splitSurface_ = std::make_shared<SplitSurface>(100, 100);
    planner->splitSurface_->splitSurfaceNode_ = RSTestUtil::CreateSurfaceNode();
    instance->plannerMap_[parent] = planner;

    auto transactionData = std::make_unique<RSTransactionData>();
    // Add a tuple with null command → should be skipped via continue
    transactionData->GetPayload().emplace_back(
        static_cast<NodeId>(1), FollowType::NONE, nullptr);

    bool result = instance->CheckOpIncNodeFromCommand(transactionData);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: CheckOpIncNodeFromCommand_NullTransactionWithPlannerMap
 * @tc.desc: Test CheckOpIncNodeFromCommand when plannerMap is not empty but rsTransactionData is nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLayerSplitManagerTest, CheckOpIncNodeFromCommand_NullTransactionWithPlannerMap, TestSize.Level1)
{
    auto instance = RSLayerSplitManager::GetInstance();
    auto planner = std::make_shared<RSOpincLayerSplitterPlanner>();
    auto parent = CreateSurfaceNode();
 
    planner->splitSurface_ = std::make_shared<SplitSurface>(100, 100);
    planner->splitSurface_->splitSurfaceNode_ = RSTestUtil::CreateSurfaceNode();
    instance->plannerMap_[parent] = planner;
    std::unique_ptr<RSTransactionData> transactionData = nullptr;
    bool result = instance->CheckOpIncNodeFromCommand(transactionData);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: CheckOpIncNodeFromCommand_PlannerReturnsFalse
 * @tc.desc: Test CheckOpIncNodeFromCommand when planner->CheckOpIncNodeFromCommand returns false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLayerSplitManagerTest, CheckOpIncNodeFromCommand_PlannerReturnsFalse, TestSize.Level1)
{
    auto instance = RSLayerSplitManager::GetInstance();
    auto planner = std::make_shared<RSOpincLayerSplitterPlanner>();
    auto parent = CreateSurfaceNode();

    planner->splitSurface_ = std::make_shared<SplitSurface>(100, 100);
    planner->splitSurface_->splitSurfaceNode_ = RSTestUtil::CreateSurfaceNode();
    // planStatus_ != ON → CheckOpIncNodeFromCommand returns false
    instance->plannerMap_[parent] = planner;

    auto transactionData = std::make_unique<RSTransactionData>();
    auto command = std::make_unique<TransactionBufferCommand>();
    transactionData->GetPayload().emplace_back(
        static_cast<NodeId>(12345), FollowType::FOLLOW_TO_SELF, std::move(command));

    bool result = instance->CheckOpIncNodeFromCommand(transactionData);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: CheckDoDirectCompositionWithSplitLayer_AllTrue
 * @tc.desc: Test CheckDoDirectCompositionWithSplitLayer when all planners return true
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLayerSplitManagerTest, CheckDoDirectCompositionWithSplitLayer_AllTrue, TestSize.Level1)
{
    auto instance = RSLayerSplitManager::GetInstance();
    auto planner = std::make_shared<RSOpincLayerSplitterPlanner>();
    auto parent = CreateSurfaceNode();

    // Set planStatus_=ON so CheckCanDoDirectComposition does not early-return false
    planner->planStatus_ = PlanStatus::ON;
    planner->canDoDirectComposition_ = true;
    planner->visitedNodeId_.insert(100);
    planner->lastOpIncNodes_.emplace_back(100, Vector4f(1.0f, 2.0f, 100.0f, 100.0f));

    auto node = std::make_shared<RSSurfaceRenderNode>(100);
    Vector4f boundsValue(10.0f, 20.0f, 100.0f, 100.0f);
    auto boundsProperty = std::make_shared<RSRenderProperty<Vector4f>>(boundsValue, 1);
    auto modifier = ModifierNG::RSRenderModifier::MakeRenderModifier(
        ModifierNG::RSModifierType::BOUNDS, boundsProperty, 1, ModifierNG::RSPropertyType::BOUNDS);
    ASSERT_NE(modifier, nullptr);
    node->AddModifier(modifier);
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    nodeMap.RegisterRenderNode(node);

    planner->splitSurface_ = std::make_shared<SplitSurface>(100, 100);
    planner->splitSurface_->splitSurfaceNode_ = RSTestUtil::CreateSurfaceNode();
    planner->splitSurface_->bufferWidth_ = 1000;
    planner->splitSurface_->bufferHeight_ = 1000;
    planner->isUpdateBuffer_ = true;
    planner->srcRect_ = RectF(0, 0, 50, 50);
    planner->opIncParentNode_ = std::make_shared<RSSurfaceRenderNode>(300);
    planner->opIncParentNode_->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 100, 100));
    planner->opIncParentNode_->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 100, 100));
    planner->opIncParentNode_->GetRenderProperties().GetBoundsGeometry()->UpdateByMatrixFromSelf();

    instance->plannerMap_[parent] = planner;

    bool result = instance->CheckDoDirectCompositionWithSplitLayer();
    EXPECT_TRUE(result);

    nodeMap.UnregisterRenderNode(100);
}
} // namespace OHOS::Rosen