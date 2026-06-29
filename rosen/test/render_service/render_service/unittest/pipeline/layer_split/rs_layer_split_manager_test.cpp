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
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_test_util.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSLayerSplitManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSLayerSplitManagerTest::SetUpTestCase() {}
void RSLayerSplitManagerTest::TearDownTestCase() {}
void RSLayerSplitManagerTest::SetUp() {}
void RSLayerSplitManagerTest::TearDown() {}

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
    EXPECT_FALSE(instance->selectorVec_.empty());
    instance->SetEnabled(false);
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
    instance->Reset(0);
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
    instance->Reset(0);

    auto parent = RSTestUtil::CreateSurfaceNode();
    auto child = RSTestUtil::CreateSurfaceNode();
    parent->AddChild(child);
    child->GetOpincRootCache().isOpincRootFlag_ = true;
    child->GetMutableRenderProperties().curGeoDirty_ = true;

    instance->RecordSplitNode(child);
    instance->CheckNeedLeave();

    instance->MoveSplitSurfaceNode();

    instance->Reset(0);
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
    instance->Reset(0);
    instance->RecordSplitNode(nullptr);
    EXPECT_TRUE(instance->plannerMap_.empty());
    instance->Reset(0);
}

/**
 * @tc.name: RecordSplitNode002
 * @tc.desc: Check if RecordSplitNode works with valid node
 * @tc.type: FUNC
 */
HWTEST_F(RSLayerSplitManagerTest, RecordSplitNode002, TestSize.Level1)
{
    auto instance = RSLayerSplitManager::GetInstance();
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
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
    instance->Reset(0);

    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    EXPECT_NE(surfaceNode, nullptr);
    surfaceNode->GetOpincRootCache().isOpincRootFlag_ = true;
    surfaceNode->GetMutableRenderProperties().curGeoDirty_ = true;
    instance->RecordSplitNode(surfaceNode);

    instance->Reset(0);
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
    instance->CheckNeedLeave();
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
    instance->Reset(0);

    auto parent = RSTestUtil::CreateSurfaceNode();
    auto child = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(parent, nullptr);
    ASSERT_NE(child, nullptr);

    parent->AddChild(child);

    child->GetOpincRootCache().isOpincRootFlag_ = true;
    child->GetMutableRenderProperties().curGeoDirty_ = true;

    instance->RecordSplitNode(child);
    instance->CheckNeedLeave();

    EXPECT_FALSE(instance->plannerMap_.empty());
    EXPECT_FALSE(instance->processorMap_.empty());
    EXPECT_FALSE(instance->parentNodeMap_.empty());

    instance->Reset(0);
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
    instance->Reset(0);

    auto parent = RSTestUtil::CreateSurfaceNode();
    auto child1 = RSTestUtil::CreateSurfaceNode();
    auto child2 = RSTestUtil::CreateSurfaceNode();
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
    instance->CheckNeedLeave();

    EXPECT_FALSE(instance->plannerMap_.empty());
    EXPECT_EQ(instance->plannerMap_.size(), 1);

    instance->Reset(0);
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
    instance->Reset(0);

    auto parent1 = RSTestUtil::CreateSurfaceNode();
    auto parent2 = RSTestUtil::CreateSurfaceNode();
    auto child1 = RSTestUtil::CreateSurfaceNode();
    auto child2 = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(parent1, nullptr);
    ASSERT_NE(parent2, nullptr);
    ASSERT_NE(child1, nullptr);
    ASSERT_NE(child2, nullptr);

    parent1->AddChild(child1);
    child1->GetOpincRootCache().isOpincRootFlag_ = true;
    child1->GetMutableRenderProperties().curGeoDirty_ = true;

    instance->RecordSplitNode(child1);
    instance->CheckNeedLeave();

    EXPECT_EQ(instance->plannerMap_.size(), 1);
    auto parent1Id = parent1->GetId();
    EXPECT_NE(instance->plannerMap_.find(parent1Id), instance->plannerMap_.end());

    parent2->AddChild(child2);
    child2->GetOpincRootCache().isOpincRootFlag_ = true;
    child2->GetMutableRenderProperties().curGeoDirty_ = true;

    instance->RecordSplitNode(child2);
    instance->CheckNeedLeave();

    EXPECT_EQ(instance->plannerMap_.size(), 2);
    auto parent2Id = parent2->GetId();
    EXPECT_NE(instance->plannerMap_.find(parent2Id), instance->plannerMap_.end());

    instance->Reset(0);
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
    instance->Reset(0);
    instance->CheckSplitNodeIntersectFilter(nullptr);
    EXPECT_TRUE(instance->plannerMap_.empty());
    instance->Reset(0);
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
    instance->Reset(0);

    auto parent = RSTestUtil::CreateSurfaceNode();
    auto child = RSTestUtil::CreateSurfaceNode();
    parent->AddChild(child);
    child->GetOpincRootCache().isOpincRootFlag_ = true;
    child->GetMutableRenderProperties().curGeoDirty_ = true;

    instance->RecordSplitNode(child);
    instance->CheckNeedLeave();

    auto hwcNode = RSTestUtil::CreateSurfaceNode();
    EXPECT_NE(hwcNode, nullptr);
    instance->CheckSplitNodeIntersectFilter(hwcNode);

    instance->Reset(0);
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
    instance->Reset(0);
    instance->UpdatePlanAndDirtyRegion(nullptr);
    EXPECT_TRUE(instance->plannerMap_.empty());
    instance->Reset(0);
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
    instance->Reset(0);

    auto parent = RSTestUtil::CreateSurfaceNode();
    auto child = RSTestUtil::CreateSurfaceNode();
    parent->AddChild(child);
    child->GetOpincRootCache().isOpincRootFlag_ = true;
    child->GetMutableRenderProperties().curGeoDirty_ = true;

    instance->RecordSplitNode(child);
    instance->CheckNeedLeave();

    instance->UpdatePlanAndDirtyRegion(nullptr);

    instance->Reset(0);
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
    instance->Reset(0);
    instance->Sync(0);
    EXPECT_TRUE(instance->plannerMap_.empty());
    instance->Reset(0);
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
    instance->Reset(0);

    auto parent = RSTestUtil::CreateSurfaceNode();
    auto child = RSTestUtil::CreateSurfaceNode();
    parent->AddChild(child);
    child->GetOpincRootCache().isOpincRootFlag_ = true;
    child->GetMutableRenderProperties().curGeoDirty_ = true;

    instance->RecordSplitNode(child);
    instance->CheckNeedLeave();

    instance->Sync(0);

    instance->Reset(0);
}

/**
 * @tc.name: DrawDfx001
 * @tc.desc: Check if DrawDfx works with null canvas
 * @tc.type: FUNC
 */
HWTEST_F(RSLayerSplitManagerTest, DrawDfx001, TestSize.Level1)
{
    auto instance = RSLayerSplitManager::GetInstance();
    instance->isGlobalEnabled_ = true;
    instance->SetEnabled(true);
    instance->Reset(0);
    instance->DrawDfx(nullptr, 0);
    EXPECT_TRUE(instance->plannerMap_.empty());
    instance->Reset(0);
}

/**
 * @tc.name: DrawDfx002
 * @tc.desc: Check if DrawDfx works with valid canvas and populated plannerMap
 * @tc.type: FUNC
 */
HWTEST_F(RSLayerSplitManagerTest, DrawDfx002, TestSize.Level1)
{
    auto instance = RSLayerSplitManager::GetInstance();
    instance->isGlobalEnabled_ = true;
    instance->SetEnabled(true);
    instance->Reset(0);

    auto parent = RSTestUtil::CreateSurfaceNode();
    auto child = RSTestUtil::CreateSurfaceNode();
    parent->AddChild(child);
    child->GetOpincRootCache().isOpincRootFlag_ = true;
    child->GetMutableRenderProperties().curGeoDirty_ = true;

    instance->RecordSplitNode(child);
    instance->CheckNeedLeave();

    auto drawingCanvas = std::make_unique<Drawing::Canvas>(100, 100);
    auto canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    EXPECT_NE(canvas, nullptr);
    instance->DrawDfx(canvas, 0);

    instance->Reset(0);
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
    instance->Reset(0);
    bool result = instance->CheckDoDirectCompositionWithSplitLayer(nullptr, false);
    EXPECT_FALSE(result);
    EXPECT_TRUE(instance->plannerMap_.empty());
    instance->Reset(0);
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
    instance->Reset(0);
    bool result = instance->CheckDoDirectCompositionWithSplitLayer(nullptr, true);
    EXPECT_FALSE(result);
    EXPECT_TRUE(instance->plannerMap_.empty());
    instance->Reset(0);
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
    instance->Reset(0);

    auto parent = RSTestUtil::CreateSurfaceNode();
    auto child = RSTestUtil::CreateSurfaceNode();
    parent->AddChild(child);
    child->GetOpincRootCache().isOpincRootFlag_ = true;
    child->GetMutableRenderProperties().curGeoDirty_ = true;

    instance->RecordSplitNode(child);
    instance->CheckNeedLeave();

    bool result = instance->CheckDoDirectCompositionWithSplitLayer(nullptr, true);
    EXPECT_FALSE(result);
    instance->Reset(0);
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
    instance->Reset(0);
    ScreenInfo screenInfo;
    screenInfo.width = 1080;
    screenInfo.height = 1920;
    instance->InitSplitSurface(screenInfo);
    EXPECT_TRUE(instance->plannerMap_.empty());
    instance->Reset(0);
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
    instance->Reset(0);

    auto parent = RSTestUtil::CreateSurfaceNode();
    auto child = RSTestUtil::CreateSurfaceNode();
    parent->AddChild(child);
    child->GetOpincRootCache().isOpincRootFlag_ = true;
    child->GetMutableRenderProperties().curGeoDirty_ = true;

    instance->RecordSplitNode(child);
    instance->CheckNeedLeave();

    ScreenInfo screenInfo;
    instance->InitSplitSurface(screenInfo);

    instance->Reset(0);
}
} // namespace OHOS::Rosen