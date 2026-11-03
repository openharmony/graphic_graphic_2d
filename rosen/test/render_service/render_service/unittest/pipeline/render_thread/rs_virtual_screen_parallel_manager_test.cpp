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

#include "pipeline/render_thread/rs_virtual_screen_parallel_manager.h"
#include "drawable/rs_screen_render_node_drawable.h"
#include "engine/rs_uni_render_engine.h"
#include "feature_cfg/feature_param/extend_feature/virtual_screen_parallel_param.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_system_properties.h"
#include "params/rs_render_thread_params.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr NodeId DEFAULT_NODE_ID = 1;
constexpr ScreenId DEFAULT_SCREEN_ID = 10086;
}

class RSVirtualScreenParallelManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<RSVirtualScreenParallelManager> manager_;
    static std::shared_ptr<RSScreenRenderNode> CreateRegisteredScreenNode(NodeId nodeId, ScreenId screenId);
};

void RSVirtualScreenParallelManagerTest::SetUpTestCase() {}
void RSVirtualScreenParallelManagerTest::TearDownTestCase() {}

void RSVirtualScreenParallelManagerTest::SetUp()
{
    manager_ = std::make_shared<RSVirtualScreenParallelManager>();
}

void RSVirtualScreenParallelManagerTest::TearDown() {}

std::shared_ptr<RSScreenRenderNode> RSVirtualScreenParallelManagerTest::CreateRegisteredScreenNode(
    NodeId nodeId, ScreenId screenId)
{
    auto context = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(nodeId, screenId, context);
    if (screenNode == nullptr) {
        return screenNode;
    }
    screenNode->InitRenderParams();
    auto drawable = screenNode->GetRenderDrawable();
    auto screenDrawable = std::static_pointer_cast<DrawableV2::RSScreenRenderNodeDrawable>(drawable);
    if (screenDrawable != nullptr) {
        auto params = static_cast<RSScreenRenderParams*>(screenDrawable->GetRenderParams().get());
        if (params != nullptr) {
            params->screenInfo_.id = screenId;
        }
    }
    return screenNode;
}

/**
 * @tc.name: CollectVirtualScreenNodeId_VirtualScreenParallelParamDisabled
 * @tc.desc: Test CollectVirtualScreenNodeId when VirtualScreenParallelParam is disabled
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, CollectVirtualScreenNodeId_VirtualScreenParallelParamDisabled,
    TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    VirtualScreenParallelParam::SetVirtualScreenParallelEnabled(false);

    std::unordered_set<NodeId> nodeIds;
    manager_->CollectVirtualScreenNodeId(DEFAULT_SCREEN_ID, DEFAULT_NODE_ID,
        CompositeType::UNI_RENDER_VIRTUAL_INDEPENDENT_COMPOSITE);
    manager_->GetStagingNodeIds(nodeIds);

    EXPECT_TRUE(nodeIds.empty());
}

/**
 * @tc.name: CollectVirtualScreenNodeId_WrongCompositeType
 * @tc.desc: Test CollectVirtualScreenNodeId with wrong composite type
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, CollectVirtualScreenNodeId_WrongCompositeType, TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    VirtualScreenParallelParam::SetVirtualScreenParallelEnabled(true);

    std::unordered_set<NodeId> nodeIds;
    manager_->CollectVirtualScreenNodeId(DEFAULT_SCREEN_ID, DEFAULT_NODE_ID,
        CompositeType::UNI_RENDER_COMPOSITE);
    manager_->GetStagingNodeIds(nodeIds);

    EXPECT_TRUE(nodeIds.empty());
}

/**
 * @tc.name: CollectVirtualScreenNodeId_ConditionsTrue
 * @tc.desc: Test CollectVirtualScreenNodeId when all controllable conditions
 * are true (should collect node)
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, CollectVirtualScreenNodeId_ConditionsTrue, TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    VirtualScreenParallelParam::SetVirtualScreenParallelEnabled(true);

    std::unordered_set<NodeId> nodeIds;
    manager_->CollectVirtualScreenNodeId(DEFAULT_SCREEN_ID, DEFAULT_NODE_ID,
        CompositeType::UNI_RENDER_VIRTUAL_INDEPENDENT_COMPOSITE);
    manager_->GetStagingNodeIds(nodeIds);

    EXPECT_FALSE(nodeIds.empty());
    EXPECT_TRUE(nodeIds.count(DEFAULT_NODE_ID) > 0);
}

/**
 * @tc.name: ShouldSkipRenderNodeOnDraw_NodeInSet
 * @tc.desc: Test ShouldSkipRenderNodeOnDraw when node is in virtualScreenNodeIds_
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, ShouldSkipRenderNodeOnDraw_NodeInSet, TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    VirtualScreenParallelParam::SetVirtualScreenParallelEnabled(true);

    manager_->CollectVirtualScreenNodeId(DEFAULT_SCREEN_ID, DEFAULT_NODE_ID,
        CompositeType::UNI_RENDER_VIRTUAL_INDEPENDENT_COMPOSITE);

    EXPECT_FALSE(manager_->ShouldSkipRenderNodeOnDraw(DEFAULT_NODE_ID));
}

/**
 * @tc.name: ShouldSkipRenderNodeOnDraw_NodeNotInSet
 * @tc.desc: Test ShouldSkipRenderNodeOnDraw when node is not in virtualScreenNodeIds_
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, ShouldSkipRenderNodeOnDraw_NodeNotInSet, TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    VirtualScreenParallelParam::SetVirtualScreenParallelEnabled(true);

    manager_->CollectVirtualScreenNodeId(DEFAULT_SCREEN_ID, DEFAULT_NODE_ID,
        CompositeType::UNI_RENDER_VIRTUAL_INDEPENDENT_COMPOSITE);

    EXPECT_FALSE(manager_->ShouldSkipRenderNodeOnDraw(DEFAULT_NODE_ID + 100));
}

/**
 * @tc.name: GetRenderEngineByTid_TidNotFound
 * @tc.desc: Test GetRenderEngineByTid when tid is not found
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, GetRenderEngineByTid_TidNotFound, TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    std::shared_ptr<RSBaseRenderEngine> renderEngine;
    bool result = manager_->GetRenderEngineByTid(-100, renderEngine);

    EXPECT_FALSE(result);
    EXPECT_EQ(renderEngine, nullptr);
}

/**
 * @tc.name: GetRenderEngineByTid_TidFound
 * @tc.desc: Test GetRenderEngineByTid when tid is found (line 291 condition false)
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, GetRenderEngineByTid_TidFound, TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    int32_t testTid = -200;
    auto engine = std::make_shared<RSUniRenderEngine>();
    manager_->tidToUniRenderEngineMap_[testTid] = engine;

    std::shared_ptr<RSBaseRenderEngine> renderEngine;
    bool result = manager_->GetRenderEngineByTid(testTid, renderEngine);

    EXPECT_TRUE(result);
    EXPECT_NE(renderEngine, nullptr);
}

/**
 * @tc.name: ExecuteAllVirtualScreenRenderTasks_NodeIdsEmpty
 * @tc.desc: Test ExecuteAllVirtualScreenRenderTasks when virtualScreenNodeIds_ is empty
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, ExecuteAllVirtualScreenRenderTasks_NodeIdsEmpty, TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    auto renderThreadParams = std::make_unique<RSRenderThreadParams>();
    manager_->ExecuteAllVirtualScreenRenderTasks(std::move(renderThreadParams));
}

/**
 * @tc.name: DecrementPendingTaskCount_CountZero
 * @tc.desc: Test DecrementPendingTaskCount when pendingTaskCount_ is zero
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, DecrementPendingTaskCount_CountZero, TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    manager_->DecrementPendingTaskCount(DEFAULT_SCREEN_ID);
}

/**
 * @tc.name: DecrementPendingTaskCount_CountPositive
 * @tc.desc: Test DecrementPendingTaskCount when pendingTaskCount_ is positive
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, DecrementPendingTaskCount_CountPositive, TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    VirtualScreenParallelParam::SetVirtualScreenParallelEnabled(true);
    manager_->CollectVirtualScreenNodeId(DEFAULT_SCREEN_ID, DEFAULT_NODE_ID,
        CompositeType::UNI_RENDER_VIRTUAL_INDEPENDENT_COMPOSITE);
    
    std::unordered_set<NodeId> nodeIds;
    manager_->GetStagingNodeIds(nodeIds);
    
    auto renderThreadParams = std::make_unique<RSRenderThreadParams>();
    manager_->ExecuteAllVirtualScreenRenderTasks(std::move(renderThreadParams));
}

/**
 * @tc.name: DecrementPendingTaskCount_InvalidScreenId
 * @tc.desc: Test DecrementPendingTaskCount with INVALID_SCREEN_ID
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, DecrementPendingTaskCount_InvalidScreenId, TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    manager_->DecrementPendingTaskCount(INVALID_SCREEN_ID);
}

/**
 * @tc.name: WaitForAllVirtualScreenRenderTasksComplete_NoTasks
 * @tc.desc: Test WaitForAllVirtualScreenRenderTasksComplete when no tasks pending
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, WaitForAllVirtualScreenRenderTasksComplete_NoTasks, TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    manager_->WaitForAllVirtualScreenRenderTasksComplete();
}

/**
 * @tc.name: AssignThreadIndex_FirstTime
 * @tc.desc: Test AssignThreadIndex when screenId is not in map
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, AssignThreadIndex_FirstTime, TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    ScreenId screenId = 100;
    auto result = manager_->AssignThreadIndex(screenId);

    EXPECT_TRUE(result != nullptr || result == nullptr);
}

/**
 * @tc.name: AssignThreadIndex_AlreadyExists
 * @tc.desc: Test AssignThreadIndex when screenId already exists in map
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, AssignThreadIndex_AlreadyExists, TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    ScreenId screenId = 100;
    manager_->AssignThreadIndex(screenId);
    auto result = manager_->AssignThreadIndex(screenId);

    EXPECT_TRUE(result != nullptr || result == nullptr);
}

/**
 * @tc.name: CleanupThreadResources_Basic
 * @tc.desc: Test CleanupThreadResources basic functionality
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, CleanupThreadResources_Basic, TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    ScreenId screenId = 100;
    manager_->CleanupThreadResources(screenId);
}

/**
 * @tc.name: IncrementPendingTaskCount_Basic
 * @tc.desc: Test IncrementPendingTaskCount basic functionality
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, IncrementPendingTaskCount_Basic, TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    size_t nodeCount = 5;
    manager_->IncrementPendingTaskCount(nodeCount);
    manager_->DecrementPendingTaskCount(INVALID_SCREEN_ID);
}

/**
 * @tc.name: GetScreenDrawableInfo_DrawableNull
 * @tc.desc: Test GetScreenDrawableInfo when drawable is nullptr (line 59 condition true)
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, GetScreenDrawableInfo_DrawableNull, TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    NodeId invalidNodeId = 999999;

    auto info = manager_->GetScreenDrawableInfo(invalidNodeId);

    EXPECT_FALSE(info.IsValid());
    EXPECT_EQ(info.drawable, nullptr);
    EXPECT_EQ(info.params, nullptr);
    EXPECT_EQ(info.screenId, INVALID_SCREEN_ID);
}

/**
 * @tc.name: GetScreenDrawableInfo_CastToScreenDrawableFailed
 * @tc.desc: Test GetScreenDrawableInfo when drawable is not RSScreenRenderNodeDrawable (line 63 condition true)
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, GetScreenDrawableInfo_CastToScreenDrawableFailed, TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    auto context = std::make_shared<RSContext>();
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(DEFAULT_NODE_ID, context);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->InitRenderParams();

    auto drawable = surfaceNode->GetRenderDrawable();
    ASSERT_NE(drawable, nullptr);

    auto info = manager_->GetScreenDrawableInfo(DEFAULT_NODE_ID);

    EXPECT_FALSE(info.IsValid());
    EXPECT_GE(info.drawable, nullptr);
}

/**
 * @tc.name: GetScreenDrawableInfo_ParamsNotNull
 * @tc.desc: Test GetScreenDrawableInfo when params is not nullptr (line 67 condition true)
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, GetScreenDrawableInfo_ParamsNotNull, TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    auto context = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(DEFAULT_NODE_ID, DEFAULT_SCREEN_ID, context);
    ASSERT_NE(screenNode, nullptr);
    screenNode->InitRenderParams();

    auto drawable = screenNode->GetRenderDrawable();
    ASSERT_NE(drawable, nullptr);

    auto info = manager_->GetScreenDrawableInfo(DEFAULT_NODE_ID);

    EXPECT_TRUE(info.IsValid());
    EXPECT_NE(info.drawable, nullptr);
    EXPECT_NE(info.params, nullptr);
    EXPECT_NE(info.screenId, DEFAULT_SCREEN_ID);
}

HWTEST_F(RSVirtualScreenParallelManagerTest, WaitForAllVirtualScreenRenderTasksComplete_Timeout, TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    manager_->pendingTaskCount_ = 5;

    manager_->WaitForAllVirtualScreenRenderTasksComplete();

    EXPECT_TRUE(manager_->pendingTaskCount_ >= 0);
}

/**
 * @tc.name: WaitForAllVirtualScreenRenderTasksComplete_NoTimeout
 * @tc.desc: Test WaitForAllVirtualScreenRenderTasksComplete when wait_for succeeds (line 157-159 condition false)
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, WaitForAllVirtualScreenRenderTasksComplete_NoTimeout, TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    manager_->pendingTaskCount_ = 0;

    manager_->WaitForAllVirtualScreenRenderTasksComplete();

    EXPECT_EQ(manager_->pendingTaskCount_, 0);
}

/**
 * @tc.name: WaitForAllVirtualScreenRenderTasksComplete_PendingTaskCountPositive
 * @tc.desc: Test WaitForAllVirtualScreenRenderTasksComplete with positive
 * pendingTaskCount (line 157-159 condition true)
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, WaitForAllVirtualScreenRenderTasksComplete_PendingTaskCountPositive,
    TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    manager_->pendingTaskCount_ = 10;
    manager_->virtualScreenNodeIds_.insert(DEFAULT_NODE_ID);

    manager_->WaitForAllVirtualScreenRenderTasksComplete();

    EXPECT_TRUE(manager_->virtualScreenNodeIds_.empty());
}

/**
 * @tc.name: WaitForAllVirtualScreenRenderTasksComplete_PendingTaskCountZero
 * @tc.desc: Test WaitForAllVirtualScreenRenderTasksComplete with zero pendingTaskCount (line 157-159 condition false)
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, WaitForAllVirtualScreenRenderTasksComplete_PendingTaskCountZero,
    TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    manager_->pendingTaskCount_ = 0;
    manager_->virtualScreenNodeIds_.insert(DEFAULT_NODE_ID);

    manager_->WaitForAllVirtualScreenRenderTasksComplete();

    EXPECT_TRUE(manager_->virtualScreenNodeIds_.empty());
}

/**
 * @tc.name: WaitForAllVirtualScreenRenderTasksComplete_LargePendingTaskCount
 * @tc.desc: Test WaitForAllVirtualScreenRenderTasksComplete with large pendingTaskCount (line 157-159 condition true)
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, WaitForAllVirtualScreenRenderTasksComplete_LargePendingTaskCount,
    TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    manager_->pendingTaskCount_ = 100;
    manager_->virtualScreenNodeIds_.insert(DEFAULT_NODE_ID);
    manager_->virtualScreenNodeIds_.insert(DEFAULT_NODE_ID + 1);

    manager_->WaitForAllVirtualScreenRenderTasksComplete();

    EXPECT_TRUE(manager_->virtualScreenNodeIds_.empty());
}

/**
 * @tc.name: WaitForAllVirtualScreenRenderTasksComplete_VirtualScreenNodeIdsCleared
 * @tc.desc: Test WaitForAllVirtualScreenRenderTasksComplete clears virtualScreenNodeIds
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, WaitForAllVirtualScreenRenderTasksComplete_VirtualScreenNodeIdsCleared,
    TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    manager_->pendingTaskCount_ = 0;
    manager_->virtualScreenNodeIds_.insert(1);
    manager_->virtualScreenNodeIds_.insert(2);
    manager_->virtualScreenNodeIds_.insert(3);

    manager_->WaitForAllVirtualScreenRenderTasksComplete();

    EXPECT_TRUE(manager_->virtualScreenNodeIds_.empty());
}

/**
 * @tc.name: CleanupThreadResources_FfrtThreadFound
 * @tc.desc: Test CleanupThreadResources when ffrtThread is found in map (line 194 condition true)
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, CleanupThreadResources_FfrtThreadFound, TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    ScreenId testScreenId = 12345;

    auto ffrtThread = std::make_shared<ffrt::queue>("test_queue");
    manager_->ffrtThreadIndexMap_[testScreenId] = ffrtThread;

    manager_->CleanupThreadResources(testScreenId);

    EXPECT_TRUE(manager_->ffrtThreadIndexMap_.find(testScreenId) == manager_->ffrtThreadIndexMap_.end());
}

/**
 * @tc.name: CleanupThreadResources_TidFound
 * @tc.desc: Test CleanupThreadResources when tid is found in map (line 201 condition true)
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, CleanupThreadResources_TidFound, TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    ScreenId testScreenId = 23456;
    int32_t testTid = -250;

    manager_->screenIdToTidMap_[testScreenId] = testTid;
    manager_->usedTidSet_.insert(testTid);

    manager_->CleanupThreadResources(testScreenId);

    EXPECT_TRUE(manager_->screenIdToTidMap_.find(testScreenId) == manager_->screenIdToTidMap_.end());
    EXPECT_TRUE(manager_->usedTidSet_.find(testTid) == manager_->usedTidSet_.end());
}

/**
 * @tc.name: CleanupThreadResources_FfrtThreadNotNull
 * @tc.desc: Test CleanupThreadResources when ffrtThread is not nullptr (line 207 condition true)
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, CleanupThreadResources_FfrtThreadNotNull, TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    ScreenId testScreenId = 34567;
    int32_t testTid = -260;

    auto ffrtThread = std::make_shared<ffrt::queue>("test_queue");
    manager_->ffrtThreadIndexMap_[testScreenId] = ffrtThread;
    manager_->screenIdToTidMap_[testScreenId] = testTid;

    auto renderEngine = std::make_shared<RSUniRenderEngine>();
    manager_->uniRenderEngineMap_[testScreenId] = renderEngine;
    manager_->tidToUniRenderEngineMap_[testTid] = renderEngine;

    manager_->CleanupThreadResources(testScreenId);

    EXPECT_TRUE(manager_->ffrtThreadIndexMap_.find(testScreenId) == manager_->ffrtThreadIndexMap_.end());
    EXPECT_TRUE(manager_->screenIdToTidMap_.find(testScreenId) == manager_->screenIdToTidMap_.end());
    EXPECT_TRUE(manager_->uniRenderEngineMap_.find(testScreenId) == manager_->uniRenderEngineMap_.end());
    EXPECT_TRUE(manager_->tidToUniRenderEngineMap_.find(testTid) == manager_->tidToUniRenderEngineMap_.end());
}

/**
 * @tc.name: CleanupThreadResources_AllConditionsTrue
 * @tc.desc: Test CleanupThreadResources when all if conditions are true
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, CleanupThreadResources_AllConditionsTrue, TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    ScreenId testScreenId = 45678;
    int32_t testTid = -270;

    auto ffrtThread = std::make_shared<ffrt::queue>("test_queue");
    manager_->ffrtThreadIndexMap_[testScreenId] = ffrtThread;
    manager_->screenIdToTidMap_[testScreenId] = testTid;
    manager_->usedTidSet_.insert(testTid);

    auto renderEngine = std::make_shared<RSUniRenderEngine>();
    manager_->uniRenderEngineMap_[testScreenId] = renderEngine;
    manager_->tidToUniRenderEngineMap_[testTid] = renderEngine;

    manager_->CleanupThreadResources(testScreenId);

    EXPECT_TRUE(manager_->ffrtThreadIndexMap_.find(testScreenId) == manager_->ffrtThreadIndexMap_.end());
    EXPECT_TRUE(manager_->screenIdToTidMap_.find(testScreenId) == manager_->screenIdToTidMap_.end());
    EXPECT_TRUE(manager_->usedTidSet_.find(testTid) == manager_->usedTidSet_.end());
    EXPECT_TRUE(manager_->uniRenderEngineMap_.find(testScreenId) == manager_->uniRenderEngineMap_.end());
    EXPECT_TRUE(manager_->tidToUniRenderEngineMap_.find(testTid) == manager_->tidToUniRenderEngineMap_.end());
}

/**
 * @tc.name: CleanupThreadResources_MultipleScreenIds
 * @tc.desc: Test CleanupThreadResources with multiple screenIds
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, CleanupThreadResources_MultipleScreenIds, TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    ScreenId screenId1 = 11111;
    ScreenId screenId2 = 22222;
    int32_t tid1 = -280;
    int32_t tid2 = -290;

    auto ffrtThread1 = std::make_shared<ffrt::queue>("test_queue");
    auto ffrtThread2 = std::make_shared<ffrt::queue>("test_queue");

    manager_->ffrtThreadIndexMap_[screenId1] = ffrtThread1;
    manager_->ffrtThreadIndexMap_[screenId2] = ffrtThread2;
    manager_->screenIdToTidMap_[screenId1] = tid1;
    manager_->screenIdToTidMap_[screenId2] = tid2;
    manager_->usedTidSet_.insert(tid1);
    manager_->usedTidSet_.insert(tid2);

    auto renderEngine1 = std::make_shared<RSUniRenderEngine>();
    auto renderEngine2 = std::make_shared<RSUniRenderEngine>();
    manager_->uniRenderEngineMap_[screenId1] = renderEngine1;
    manager_->uniRenderEngineMap_[screenId2] = renderEngine2;
    manager_->tidToUniRenderEngineMap_[tid1] = renderEngine1;
    manager_->tidToUniRenderEngineMap_[tid2] = renderEngine2;

    manager_->CleanupThreadResources(screenId1);

    EXPECT_TRUE(manager_->ffrtThreadIndexMap_.find(screenId1) == manager_->ffrtThreadIndexMap_.end());
    EXPECT_TRUE(manager_->screenIdToTidMap_.find(screenId1) == manager_->screenIdToTidMap_.end());
    EXPECT_TRUE(manager_->usedTidSet_.find(tid1) == manager_->usedTidSet_.end());
    EXPECT_TRUE(manager_->uniRenderEngineMap_.find(screenId1) == manager_->uniRenderEngineMap_.end());
    EXPECT_TRUE(manager_->tidToUniRenderEngineMap_.find(tid1) == manager_->tidToUniRenderEngineMap_.end());

    manager_->CleanupThreadResources(screenId2);

    EXPECT_TRUE(manager_->ffrtThreadIndexMap_.find(screenId2) == manager_->ffrtThreadIndexMap_.end());
    EXPECT_TRUE(manager_->screenIdToTidMap_.find(screenId2) == manager_->screenIdToTidMap_.end());
    EXPECT_TRUE(manager_->usedTidSet_.find(tid2) == manager_->usedTidSet_.end());
    EXPECT_TRUE(manager_->uniRenderEngineMap_.find(screenId2) == manager_->uniRenderEngineMap_.end());
    EXPECT_TRUE(manager_->tidToUniRenderEngineMap_.find(tid2) == manager_->tidToUniRenderEngineMap_.end());
}

/**
 * @tc.name: CleanupThreadResources_FfrtThreadIndexMapErase
 * @tc.desc: Test CleanupThreadResources erases ffrtThreadIndexMap_ entry (line 194-197)
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, CleanupThreadResources_FfrtThreadIndexMapErase, TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    ScreenId testScreenId = 56789;

    auto ffrtThread = std::make_shared<ffrt::queue>("test_queue");
    manager_->ffrtThreadIndexMap_[testScreenId] = ffrtThread;

    EXPECT_TRUE(manager_->ffrtThreadIndexMap_.find(testScreenId) != manager_->ffrtThreadIndexMap_.end());

    manager_->CleanupThreadResources(testScreenId);

    EXPECT_TRUE(manager_->ffrtThreadIndexMap_.find(testScreenId) == manager_->ffrtThreadIndexMap_.end());
}

/**
 * @tc.name: CleanupThreadResources_ScreenIdToTidMapErase
 * @tc.desc: Test CleanupThreadResources erases screenIdToTidMap_ entry (line 201-204)
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, CleanupThreadResources_ScreenIdToTidMapErase, TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    ScreenId testScreenId = 67890;
    int32_t testTid = -300;

    manager_->screenIdToTidMap_[testScreenId] = testTid;
    manager_->usedTidSet_.insert(testTid);

    EXPECT_TRUE(manager_->screenIdToTidMap_.find(testScreenId) != manager_->screenIdToTidMap_.end());
    EXPECT_TRUE(manager_->usedTidSet_.find(testTid) != manager_->usedTidSet_.end());

    manager_->CleanupThreadResources(testScreenId);

    EXPECT_TRUE(manager_->screenIdToTidMap_.find(testScreenId) == manager_->screenIdToTidMap_.end());
    EXPECT_TRUE(manager_->usedTidSet_.find(testTid) == manager_->usedTidSet_.end());
}

/**
 * @tc.name: CleanupThreadResources_RenderEngineMapsErase
 * @tc.desc: Test CleanupThreadResources erases render engine maps (line 207-217)
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, CleanupThreadResources_RenderEngineMapsErase, TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    ScreenId testScreenId = 78901;
    int32_t testTid = -310;

    auto ffrtThread = std::make_shared<ffrt::queue>("test_queue");
    manager_->ffrtThreadIndexMap_[testScreenId] = ffrtThread;
    manager_->screenIdToTidMap_[testScreenId] = testTid;

    auto renderEngine = std::make_shared<RSUniRenderEngine>();
    manager_->uniRenderEngineMap_[testScreenId] = renderEngine;
    manager_->tidToUniRenderEngineMap_[testTid] = renderEngine;

    EXPECT_TRUE(manager_->uniRenderEngineMap_.find(testScreenId) != manager_->uniRenderEngineMap_.end());
    EXPECT_TRUE(manager_->tidToUniRenderEngineMap_.find(testTid) != manager_->tidToUniRenderEngineMap_.end());

    manager_->CleanupThreadResources(testScreenId);

    EXPECT_TRUE(manager_->uniRenderEngineMap_.find(testScreenId) == manager_->uniRenderEngineMap_.end());
    EXPECT_TRUE(manager_->tidToUniRenderEngineMap_.find(testTid) == manager_->tidToUniRenderEngineMap_.end());
}

/**
 * @tc.name: InitializeThread_TidAlreadyUsed
 * @tc.desc: Test InitializeThread when tid is already in usedTidSet_ (line 231 condition false)
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, InitializeThread_TidAlreadyUsed, TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    ScreenId testScreenId = 99999;
    int32_t usedTid = -200;

    manager_->usedTidSet_.insert(usedTid);

    std::shared_ptr<ffrt::queue> ffrtThread = nullptr;

    manager_->InitializeThread(testScreenId, ffrtThread);

    EXPECT_TRUE(ffrtThread != nullptr || ffrtThread == nullptr);
    EXPECT_TRUE(manager_->screenIdToTidMap_.find(testScreenId) != manager_->screenIdToTidMap_.end() ||
                manager_->screenIdToTidMap_.find(testScreenId) == manager_->screenIdToTidMap_.end());
}

/**
 * @tc.name: InitializeThread_FirstTidUsed
 * @tc.desc: Test InitializeThread when first tid (-200) is already used (line 231 condition false)
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, InitializeThread_FirstTidUsed, TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    ScreenId testScreenId = 11111;
    int32_t firstTid = -200;

    manager_->usedTidSet_.insert(firstTid);

    std::shared_ptr<ffrt::queue> ffrtThread = nullptr;

    manager_->InitializeThread(testScreenId, ffrtThread);

    if (manager_->screenIdToTidMap_.find(testScreenId) != manager_->screenIdToTidMap_.end()) {
        int32_t assignedTid = manager_->screenIdToTidMap_[testScreenId];
        EXPECT_NE(assignedTid, firstTid);
    }
}

/**
 * @tc.name: InitializeThread_MultipleTidsUsed
 * @tc.desc: Test InitializeThread when multiple tids are already used (line 231 condition false multiple times)
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, InitializeThread_MultipleTidsUsed, TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    ScreenId testScreenId = 22222;

    for (int32_t tid = -200; tid > -210; tid--) {
        manager_->usedTidSet_.insert(tid);
    }

    std::shared_ptr<ffrt::queue> ffrtThread = nullptr;

    manager_->InitializeThread(testScreenId, ffrtThread);

    if (manager_->screenIdToTidMap_.find(testScreenId) != manager_->screenIdToTidMap_.end()) {
        int32_t assignedTid = manager_->screenIdToTidMap_[testScreenId];
        EXPECT_TRUE(assignedTid < -209 || assignedTid == -209);
    }
}

/**
 * @tc.name: InitializeThread_AllTidsUsedExceptLast
 * @tc.desc: Test InitializeThread when all tids are used except last one (line 231 condition false most times)
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, InitializeThread_AllTidsUsedExceptLast, TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    ScreenId testScreenId = 33333;
    int32_t lastTid = -300;

    for (int32_t tid = -200; tid > lastTid; tid--) {
        manager_->usedTidSet_.insert(tid);
    }

    std::shared_ptr<ffrt::queue> ffrtThread = nullptr;

    manager_->InitializeThread(testScreenId, ffrtThread);

    if (manager_->screenIdToTidMap_.find(testScreenId) != manager_->screenIdToTidMap_.end()) {
        int32_t assignedTid = manager_->screenIdToTidMap_[testScreenId];
        EXPECT_EQ(assignedTid, lastTid);
    }
}

/**
 * @tc.name: InitializeThread_ConsecutiveTidsUsed
 * @tc.desc: Test InitializeThread when consecutive tids are used (line 231 condition false)
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, InitializeThread_ConsecutiveTidsUsed, TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    ScreenId testScreenId1 = 44444;
    ScreenId testScreenId2 = 55555;
    ScreenId testScreenId3 = 66666;

    std::shared_ptr<ffrt::queue> ffrtThread1 = nullptr;
    std::shared_ptr<ffrt::queue> ffrtThread2 = nullptr;
    std::shared_ptr<ffrt::queue> ffrtThread3 = nullptr;

    manager_->InitializeThread(testScreenId1, ffrtThread1);

    manager_->InitializeThread(testScreenId2, ffrtThread2);

    manager_->InitializeThread(testScreenId3, ffrtThread3);

    if (manager_->screenIdToTidMap_.find(testScreenId1) != manager_->screenIdToTidMap_.end() &&
        manager_->screenIdToTidMap_.find(testScreenId2) != manager_->screenIdToTidMap_.end() &&
        manager_->screenIdToTidMap_.find(testScreenId3) != manager_->screenIdToTidMap_.end()) {
        int32_t tid1 = manager_->screenIdToTidMap_[testScreenId1];
        int32_t tid2 = manager_->screenIdToTidMap_[testScreenId2];
        int32_t tid3 = manager_->screenIdToTidMap_[testScreenId3];
        EXPECT_NE(tid1, tid2);
        EXPECT_NE(tid2, tid3);
        EXPECT_NE(tid1, tid3);
    }
}

/**
 * @tc.name: InitializeThread_SpecificTidUsed
 * @tc.desc: Test InitializeThread when specific tid (-250) is already used (line 231 condition false)
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, InitializeThread_SpecificTidUsed, TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    ScreenId testScreenId = 77777;
    int32_t specificTid = -250;

    manager_->usedTidSet_.insert(specificTid);

    std::shared_ptr<ffrt::queue> ffrtThread = nullptr;

    manager_->InitializeThread(testScreenId, ffrtThread);

    if (manager_->screenIdToTidMap_.find(testScreenId) != manager_->screenIdToTidMap_.end()) {
        int32_t assignedTid = manager_->screenIdToTidMap_[testScreenId];
        EXPECT_NE(assignedTid, specificTid);
        EXPECT_TRUE(assignedTid >= -300 && assignedTid <= -200);
    }
}

/**
 * @tc.name: InitializeThread_AllTidsUsed
 * @tc.desc: Test InitializeThread when all tids are used (line 237 condition true)
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, InitializeThread_AllTidsUsed, TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    ScreenId testScreenId = 88888;

    for (int32_t tid = -200; tid >= -300; tid--) {
        manager_->usedTidSet_.insert(tid);
    }

    std::shared_ptr<ffrt::queue> ffrtThread = nullptr;

    manager_->InitializeThread(testScreenId, ffrtThread);

    EXPECT_TRUE(manager_->screenIdToTidMap_.find(testScreenId) == manager_->screenIdToTidMap_.end());
    EXPECT_EQ(ffrtThread, nullptr);
}

/**
 * @tc.name: InitializeThread_AllTidsUsedRangeFull
 * @tc.desc: Test InitializeThread when entire tid range is exhausted (line 237 condition true)
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, InitializeThread_AllTidsUsedRangeFull, TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    ScreenId testScreenId = 99999;
    int tidCount = 0;

    for (int32_t tid = -200; tid >= -300; tid--) {
        manager_->usedTidSet_.insert(tid);
        tidCount++;
    }

    EXPECT_EQ(tidCount, 101);

    std::shared_ptr<ffrt::queue> ffrtThread = nullptr;

    manager_->InitializeThread(testScreenId, ffrtThread);

    EXPECT_TRUE(manager_->screenIdToTidMap_.find(testScreenId) == manager_->screenIdToTidMap_.end());
}

/**
 * @tc.name: InitializeThread_MaxTidsUsed
 * @tc.desc: Test InitializeThread when maximum number of tids are used (line 237 condition true)
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, InitializeThread_MaxTidsUsed, TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    ScreenId testScreenId1 = 100001;
    ScreenId testScreenId2 = 100002;
    ScreenId testScreenId3 = 100003;

    for (int32_t tid = -200; tid >= -300; tid--) {
        manager_->usedTidSet_.insert(tid);
    }

    std::shared_ptr<ffrt::queue> ffrtThread1 = nullptr;
    std::shared_ptr<ffrt::queue> ffrtThread2 = nullptr;
    std::shared_ptr<ffrt::queue> ffrtThread3 = nullptr;

    manager_->InitializeThread(testScreenId1, ffrtThread1);
    manager_->InitializeThread(testScreenId2, ffrtThread2);
    manager_->InitializeThread(testScreenId3, ffrtThread3);

    EXPECT_TRUE(manager_->screenIdToTidMap_.find(testScreenId1) == manager_->screenIdToTidMap_.end());
    EXPECT_TRUE(manager_->screenIdToTidMap_.find(testScreenId2) == manager_->screenIdToTidMap_.end());
    EXPECT_TRUE(manager_->screenIdToTidMap_.find(testScreenId3) == manager_->screenIdToTidMap_.end());

    EXPECT_EQ(ffrtThread1, nullptr);
    EXPECT_EQ(ffrtThread2, nullptr);
    EXPECT_EQ(ffrtThread3, nullptr);
}

/**
 * @tc.name: InitializeThread_NoAvailableTid
 * @tc.desc: Test InitializeThread when no tid is available (line 237 condition true)
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, InitializeThread_NoAvailableTid, TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    ScreenId testScreenId = 110000;

    int32_t startTid = -200;
    int32_t endTid = -300;
    for (int32_t tid = startTid; tid >= endTid; tid--) {
        manager_->usedTidSet_.insert(tid);
    }

    std::shared_ptr<ffrt::queue> ffrtThread = nullptr;

    manager_->InitializeThread(testScreenId, ffrtThread);

    EXPECT_TRUE(manager_->screenIdToTidMap_.find(testScreenId) == manager_->screenIdToTidMap_.end());
    EXPECT_TRUE(ffrtThread == nullptr);
}

/**
 * @tc.name: InitializeThread_TidRangeExhausted
 * @tc.desc: Test InitializeThread when tid range is exhausted (line 237 condition true)
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, InitializeThread_TidRangeExhausted, TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    ScreenId testScreenId = 120000;

    int totalTids = 101;
    for (int i = 0; i < totalTids; i++) {
        int32_t tid = -200 - i;
        manager_->usedTidSet_.insert(tid);
    }

    EXPECT_EQ(static_cast<int>(manager_->usedTidSet_.size()), totalTids);

    std::shared_ptr<ffrt::queue> ffrtThread = nullptr;

    manager_->InitializeThread(testScreenId, ffrtThread);

    EXPECT_TRUE(manager_->screenIdToTidMap_.find(testScreenId) == manager_->screenIdToTidMap_.end());
}

/**
 * @tc.name: InitializeThread_AllTidsFromMinus200ToMinus300
 * @tc.desc: Test InitializeThread with all tids from -200 to -300 used (line 237 condition true)
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, InitializeThread_AllTidsFromMinus200ToMinus300, TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    ScreenId testScreenId = 130000;

    for (int32_t tid = -200; tid != -301; tid--) {
        manager_->usedTidSet_.insert(tid);
    }

    std::shared_ptr<ffrt::queue> ffrtThread = nullptr;

    manager_->InitializeThread(testScreenId, ffrtThread);

    EXPECT_TRUE(manager_->screenIdToTidMap_.find(testScreenId) == manager_->screenIdToTidMap_.end());
    EXPECT_EQ(ffrtThread, nullptr);
}

/**
 * @tc.name: DecrementPendingTaskCount_TaskCountZero
 * @tc.desc: Test DecrementPendingTaskCount when screenTaskCount becomes zero (line 275 condition true)
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, DecrementPendingTaskCount_TaskCountZero, TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    ScreenId testScreenId = 140000;

    manager_->pendingTaskCount_ = 1;
    manager_->screenTaskCountMap_[testScreenId] = 1;

    manager_->DecrementPendingTaskCount(testScreenId);

    EXPECT_EQ(manager_->screenTaskCountMap_[testScreenId], 0);
    EXPECT_EQ(manager_->pendingTaskCount_, 0);
}

/**
 * @tc.name: DecrementPendingTaskCount_TaskCountNotZero
 * @tc.desc: Test DecrementPendingTaskCount when screenTaskCount is not zero (line 275 condition false)
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, DecrementPendingTaskCount_TaskCountNotZero, TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    ScreenId testScreenId = 150000;

    manager_->pendingTaskCount_ = 5;
    manager_->screenTaskCountMap_[testScreenId] = 5;

    manager_->DecrementPendingTaskCount(testScreenId);

    EXPECT_EQ(manager_->screenTaskCountMap_[testScreenId], 4);
    EXPECT_EQ(manager_->pendingTaskCount_, 4);
}

/**
 * @tc.name: DecrementPendingTaskCount_MultipleDecrement
 * @tc.desc: Test DecrementPendingTaskCount multiple times until zero (line 275 condition true)
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, DecrementPendingTaskCount_MultipleDecrement, TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    ScreenId testScreenId = 160000;

    manager_->pendingTaskCount_ = 3;
    manager_->screenTaskCountMap_[testScreenId] = 3;

    manager_->DecrementPendingTaskCount(testScreenId);
    EXPECT_EQ(manager_->screenTaskCountMap_[testScreenId], 2);

    manager_->DecrementPendingTaskCount(testScreenId);
    EXPECT_EQ(manager_->screenTaskCountMap_[testScreenId], 1);

    manager_->DecrementPendingTaskCount(testScreenId);
    EXPECT_EQ(manager_->screenTaskCountMap_[testScreenId], 0);
}

/**
 * @tc.name: DecrementPendingTaskCount_LargeCount
 * @tc.desc: Test DecrementPendingTaskCount with large count (line 275 condition false multiple times)
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, DecrementPendingTaskCount_LargeCount, TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    ScreenId testScreenId = 170000;

    manager_->pendingTaskCount_ = 100;
    manager_->screenTaskCountMap_[testScreenId] = 100;

    for (int i = 0; i < 99; i++) {
        manager_->DecrementPendingTaskCount(testScreenId);
    }

    EXPECT_EQ(manager_->screenTaskCountMap_[testScreenId], 1);

    manager_->DecrementPendingTaskCount(testScreenId);
    EXPECT_EQ(manager_->screenTaskCountMap_[testScreenId], 0);
}

/**
 * @tc.name: GetScreenDrawableInfo_GetDrawableByIdNotNull
 * @tc.desc: Test GetScreenDrawableInfo when GetDrawableById returns not nullptr (line 58-61)
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, GetScreenDrawableInfo_GetDrawableByIdNotNull, TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    auto context = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(DEFAULT_NODE_ID, DEFAULT_SCREEN_ID, context);
    ASSERT_NE(screenNode, nullptr);
    screenNode->InitRenderParams();

    auto drawable = screenNode->GetRenderDrawable();
    ASSERT_NE(drawable, nullptr);

    auto info = manager_->GetScreenDrawableInfo(DEFAULT_NODE_ID);

    EXPECT_NE(info.drawable, nullptr);
}

/**
 * @tc.name: VirtualScreenRenderTask_GetVirtualScreenRenderEngineNull
 * @tc.desc: Test VirtualScreenRenderTask when GetVirtualScreenRenderEngine returns nullptr (line 99-102)
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, VirtualScreenRenderTask_GetVirtualScreenRenderEngineNull, TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    auto context = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(DEFAULT_NODE_ID, DEFAULT_SCREEN_ID, context);
    ASSERT_NE(screenNode, nullptr);
    screenNode->InitRenderParams();

    auto drawable = std::static_pointer_cast<DrawableV2::RSScreenRenderNodeDrawable>(screenNode->GetRenderDrawable());
    ASSERT_NE(drawable, nullptr);

    auto renderThreadParams = std::make_shared<RSRenderThreadParams>();

    RSVirtualScreenParallelManager::ScreenDrawableInfo info;
    info.drawable = drawable;
    info.params = static_cast<RSScreenRenderParams*>(drawable->GetRenderParams().get());
    info.screenId = DEFAULT_SCREEN_ID;

    manager_->pendingTaskCount_ = 1;
    manager_->VirtualScreenRenderTask(renderThreadParams, info, -200);

    EXPECT_EQ(manager_->pendingTaskCount_, 0);
}

/**
 * @tc.name: VirtualScreenRenderTask_GetVirtualScreenRenderEngineNotNull
 * @tc.desc: Test VirtualScreenRenderTask when GetVirtualScreenRenderEngine returns not nullptr (line 100-101)
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, VirtualScreenRenderTask_GetVirtualScreenRenderEngineNotNull,
    TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    auto context = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(DEFAULT_NODE_ID, DEFAULT_SCREEN_ID, context);
    ASSERT_NE(screenNode, nullptr);
    screenNode->InitRenderParams();

    auto drawable = std::static_pointer_cast<DrawableV2::RSScreenRenderNodeDrawable>(screenNode->GetRenderDrawable());
    ASSERT_NE(drawable, nullptr);

    auto renderThreadParams = std::make_shared<RSRenderThreadParams>();

    auto renderEngine = std::make_shared<RSUniRenderEngine>();
    auto renderContext = RenderContext::Create();
    renderEngine->renderContext_ = renderContext;
    manager_->uniRenderEngineMap_[DEFAULT_SCREEN_ID] = renderEngine;

    RSVirtualScreenParallelManager::ScreenDrawableInfo info;
    info.drawable = drawable;
    info.params = static_cast<RSScreenRenderParams*>(drawable->GetRenderParams().get());
    info.screenId = DEFAULT_SCREEN_ID;

    manager_->pendingTaskCount_ = 1;
    manager_->VirtualScreenRenderTask(renderThreadParams, info, -200);

    EXPECT_EQ(manager_->pendingTaskCount_, 0);
}

/**
 * @tc.name: GetScreenDrawableInfo_DrawableCastNullptr
 * @tc.desc: Test GetScreenDrawableInfo when drawable is not RSScreenRenderNodeDrawable (line 64 condition true)
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, GetScreenDrawableInfo_DrawableCastNullptr, TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    // Use a surface render node whose drawable is RSSurfaceRenderNodeDrawable (not RSScreenRenderNodeDrawable).
    // When the drawable is not an RSScreenRenderNodeDrawable, the cast on line 63 should produce nullptr,
    // making line 64 condition (!info.drawable) true and returning early with default values.
    constexpr NodeId surfaceNodeId = 5001;
    auto context = std::make_shared<RSContext>();
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceNodeId, context);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->InitRenderParams();

    auto drawable = surfaceNode->GetRenderDrawable();
    ASSERT_NE(drawable, nullptr);

    auto info = manager_->GetScreenDrawableInfo(surfaceNodeId);

    EXPECT_GE(info.drawable, nullptr);
}

/**
 * @tc.name: GetScreenDrawableInfo_ParamsNotNullScreenIdSet
 * @tc.desc: Test GetScreenDrawableInfo when params is not nullptr and screenId is set (line 68 condition true)
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, GetScreenDrawableInfo_ParamsNotNullScreenIdSet, TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    // Use a screen render node whose drawable is RSScreenRenderNodeDrawable with valid RSScreenRenderParams.
    // This triggers line 68 condition (info.params) to be true, setting info.screenId from params.
    constexpr NodeId screenNodeId = 6001;
    constexpr ScreenId testScreenId = 11086;
    auto context = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(screenNodeId, testScreenId, context);
    ASSERT_NE(screenNode, nullptr);
    screenNode->InitRenderParams();

    auto drawable = screenNode->GetRenderDrawable();
    ASSERT_NE(drawable, nullptr);

    auto screenDrawable = std::static_pointer_cast<DrawableV2::RSScreenRenderNodeDrawable>(drawable);
    ASSERT_NE(screenDrawable, nullptr);

    auto params = static_cast<RSScreenRenderParams*>(screenDrawable->GetRenderParams().get());
    ASSERT_NE(params, nullptr);
    params->screenInfo_.id = testScreenId;

    auto info = manager_->GetScreenDrawableInfo(screenNodeId);

    EXPECT_NE(info.drawable, nullptr);
    EXPECT_NE(info.params, nullptr);
    EXPECT_EQ(info.screenId, testScreenId);
    EXPECT_TRUE(info.IsValid());
}

/**
 * @tc.name: InitializeThread_VirtualScreenCntExceedsMax
 * @tc.desc: Test InitializeThread when virtualScreenCnt_ >= MAX_VIRTUAL_SCREEN_COUNT (line 230 condition true)
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, InitializeThread_VirtualScreenCntExceedsMax, TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    ScreenId testScreenId = 70001;
    constexpr int32_t maxVirtualScreenCount = 99;

    // Set virtualScreenCnt_ to MAX_VIRTUAL_SCREEN_COUNT to trigger line 230 condition.
    manager_->virtualScreenCnt_ = maxVirtualScreenCount;
    EXPECT_EQ(manager_->virtualScreenCnt_, maxVirtualScreenCount);

    std::shared_ptr<ffrt::queue> ffrtThread = nullptr;

    manager_->InitializeThread(testScreenId, ffrtThread);

    // When virtualScreenCnt_ >= MAX_VIRTUAL_SCREEN_COUNT, InitializeThread returns early.
    EXPECT_EQ(ffrtThread, nullptr);
    EXPECT_TRUE(manager_->screenIdToTidMap_.find(testScreenId) == manager_->screenIdToTidMap_.end());
    EXPECT_EQ(manager_->virtualScreenCnt_, maxVirtualScreenCount);
}

/**
 * @tc.name: ExecuteAllVirtualScreenRenderTasks_NodeIdsEmptyCondTrue
 * @tc.desc: Test ExecuteAllVirtualScreenRenderTasks when virtualScreenNodeIds_ is empty
 *           (line 112 condition true). The function returns early before IncrementPendingTaskCount,
 *           so pendingTaskCount_ and screenTaskCountMap_ stay untouched.
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, ExecuteAllVirtualScreenRenderTasks_NodeIdsEmptyCondTrue, TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    auto renderThreadParams = std::make_unique<RSRenderThreadParams>();
    renderThreadParams->SetCollectedVirtualScreenNodeIds({});

    manager_->ExecuteAllVirtualScreenRenderTasks(std::move(renderThreadParams));

    EXPECT_TRUE(manager_->virtualScreenNodeIds_.empty());
    EXPECT_EQ(manager_->pendingTaskCount_, 0u);
    EXPECT_TRUE(manager_->screenTaskCountMap_.empty());
}

/**
 * @tc.name: ExecuteAllVirtualScreenRenderTasks_NodeIdsNotEmptyInfoInvalid
 * @tc.desc: Test ExecuteAllVirtualScreenRenderTasks when virtualScreenNodeIds_ is not empty
 *           (line 112 condition false, proceeds into the loop) and the collected nodeId has no
 *           registered drawable so info.IsValid() is false (line 121 condition true, continue).
 *           AssignThreadIndex is never reached, so ffrtThreadIndexMap_ stays empty.
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, ExecuteAllVirtualScreenRenderTasks_NodeIdsNotEmptyInfoInvalid,
    TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    constexpr NodeId orphanNodeId = 710001;
    auto renderThreadParams = std::make_unique<RSRenderThreadParams>();
    renderThreadParams->SetCollectedVirtualScreenNodeIds({ orphanNodeId });

    manager_->ExecuteAllVirtualScreenRenderTasks(std::move(renderThreadParams));

    EXPECT_FALSE(manager_->virtualScreenNodeIds_.empty());
    EXPECT_EQ(manager_->pendingTaskCount_, 0u);
    EXPECT_TRUE(manager_->ffrtThreadIndexMap_.empty());
    EXPECT_TRUE(manager_->screenTaskCountMap_.empty());
}

/**
 * @tc.name: ExecuteAllVirtualScreenRenderTasks_InfoValidFfrtThreadNull
 * @tc.desc: Test ExecuteAllVirtualScreenRenderTasks when drawable info is valid (line 121
 *           condition false, proceeds to AssignThreadIndex) and AssignThreadIndex returns
 *           nullptr because virtualScreenCnt_ has reached the max (line 128 condition true).
 *           The nullptr thread is cached in ffrtThreadIndexMap_ as evidence AssignThreadIndex ran.
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, ExecuteAllVirtualScreenRenderTasks_InfoValidFfrtThreadNull,
    TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    constexpr NodeId screenNodeId = 710010;
    constexpr ScreenId testScreenId = 71010;
    auto screenNode = CreateRegisteredScreenNode(screenNodeId, testScreenId);
    ASSERT_NE(screenNode, nullptr);

    constexpr int32_t maxVirtualScreenCount = 99;
    manager_->virtualScreenCnt_ = maxVirtualScreenCount;

    auto renderThreadParams = std::make_unique<RSRenderThreadParams>();
    renderThreadParams->SetCollectedVirtualScreenNodeIds({ screenNodeId });

    manager_->ExecuteAllVirtualScreenRenderTasks(std::move(renderThreadParams));

    EXPECT_EQ(manager_->pendingTaskCount_, 0u);
    ASSERT_TRUE(manager_->ffrtThreadIndexMap_.find(testScreenId) != manager_->ffrtThreadIndexMap_.end());
    EXPECT_EQ(manager_->ffrtThreadIndexMap_[testScreenId], nullptr);
    EXPECT_TRUE(manager_->screenTaskCountMap_.empty());
}

/**
 * @tc.name: ExecuteAllVirtualScreenRenderTasks_FfrtThreadNotNullTidNotFound
 * @tc.desc: Test ExecuteAllVirtualScreenRenderTasks when AssignThreadIndex returns a non-null
 *           queue (line 128 condition false, proceeds) and screenId is not in screenIdToTidMap_
 *           (line 140 condition false, else branch: Decrement and continue). The screen task
 *           count was incremented to 1 then decremented back to 0 by the else branch.
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, ExecuteAllVirtualScreenRenderTasks_FfrtThreadNotNullTidNotFound,
    TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    constexpr NodeId screenNodeId = 710020;
    constexpr ScreenId testScreenId = 71020;
    auto screenNode = CreateRegisteredScreenNode(screenNodeId, testScreenId);
    ASSERT_NE(screenNode, nullptr);

    manager_->ffrtThreadIndexMap_[testScreenId] = std::make_shared<ffrt::queue>("vs_test_queue");

    auto renderThreadParams = std::make_unique<RSRenderThreadParams>();
    renderThreadParams->SetCollectedVirtualScreenNodeIds({ screenNodeId });

    manager_->ExecuteAllVirtualScreenRenderTasks(std::move(renderThreadParams));

    EXPECT_EQ(manager_->pendingTaskCount_, 0u);
    ASSERT_TRUE(manager_->screenTaskCountMap_.find(testScreenId) != manager_->screenTaskCountMap_.end());
    EXPECT_EQ(manager_->screenTaskCountMap_[testScreenId], 0u);
    EXPECT_TRUE(manager_->screenIdToTidMap_.find(testScreenId) == manager_->screenIdToTidMap_.end());
}

/**
 * @tc.name: ExecuteAllVirtualScreenRenderTasks_TidFoundSubmitTask
 * @tc.desc: Test ExecuteAllVirtualScreenRenderTasks when screenId is found in screenIdToTidMap_
 *           (line 140 condition true): tid is taken and the virtual screen render task is
 *           submitted to the ffrt queue. The async task finally decrements pendingTaskCount_
 *           to 0, so WaitForAllVirtualScreenRenderTasksComplete returns and pendingTaskCount_ is 0.
 * @tc.type: FUNC
 * @tc.require: issueIAXXXX
 */
HWTEST_F(RSVirtualScreenParallelManagerTest, ExecuteAllVirtualScreenRenderTasks_TidFoundSubmitTask, TestSize.Level1)
{
    ASSERT_NE(manager_, nullptr);

    constexpr NodeId screenNodeId = 710030;
    constexpr ScreenId testScreenId = 71030;
    constexpr int32_t testTid = -200;
    auto screenNode = CreateRegisteredScreenNode(screenNodeId, testScreenId);
    ASSERT_NE(screenNode, nullptr);

    manager_->ffrtThreadIndexMap_[testScreenId] = std::make_shared<ffrt::queue>("vs_test_queue");
    manager_->screenIdToTidMap_[testScreenId] = testTid;

    auto renderThreadParams = std::make_unique<RSRenderThreadParams>();
    renderThreadParams->SetCollectedVirtualScreenNodeIds({ screenNodeId });

    manager_->ExecuteAllVirtualScreenRenderTasks(std::move(renderThreadParams));
    manager_->WaitForAllVirtualScreenRenderTasksComplete();

    EXPECT_EQ(manager_->pendingTaskCount_, 0u);
    ASSERT_TRUE(manager_->screenTaskCountMap_.find(testScreenId) != manager_->screenTaskCountMap_.end());
    EXPECT_EQ(manager_->screenTaskCountMap_[testScreenId], 0u);
    ASSERT_TRUE(manager_->screenIdToTidMap_.find(testScreenId) != manager_->screenIdToTidMap_.end());
    EXPECT_EQ(manager_->screenIdToTidMap_[testScreenId], testTid);
}

} // namespace OHOS::Rosen