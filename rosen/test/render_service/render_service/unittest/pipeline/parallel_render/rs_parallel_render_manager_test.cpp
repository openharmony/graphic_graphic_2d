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

#include <cstdint>
#include <list>
#include <memory>

#include "gtest/gtest.h"
#include "limit_number.h"

#include "pipeline/parallel_render/rs_parallel_render_manager.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_uni_render_visitor.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSParallelRenderManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    uint32_t threadNum_;
};

void RSParallelRenderManagerTest::SetUpTestCase() {}
void RSParallelRenderManagerTest::TearDownTestCase() {}
void RSParallelRenderManagerTest::SetUp()
{
    threadNum_ = RSParallelRenderManager::Instance()->GetParallelThreadNumber();
}
void RSParallelRenderManagerTest::TearDown()
{
    threadNum_ = 0;
}

/**
 * @tc.name: SetParallelModeTest
 * @tc.desc: Test RSParallelRenderManagerTest.SetParallelModeTest
 * @tc.type: FUNC
 * @tc.require: issueI69JAV
 */
HWTEST_F(RSParallelRenderManagerTest, SetParallelModeTest, TestSize.Level1)
{
    auto instance = RSParallelRenderManager::Instance();
    ParallelStatus status = instance->GetParallelRenderingStatus();
    ASSERT_EQ(ParallelStatus::OFF, status);

    instance->SetParallelMode(false);
    status = instance->GetParallelRenderingStatus();
    ASSERT_EQ(ParallelStatus::OFF, status);

    instance->StartSubRenderThread(threadNum_, nullptr);
    status = instance->GetParallelRenderingStatus();
    ASSERT_EQ(ParallelStatus::FIRSTFLUSH, status);

    for (uint32_t i = 0; i < threadNum_; i++) {
        instance->ReadySubThreadNumIncrement();
    }
    status = instance->GetParallelRenderingStatus();
    ASSERT_EQ(ParallelStatus::ON, status);

    instance->GetParallelMode();
    instance->GetParallelModeSafe();
    bool mode = instance->GetParallelMode();
    ASSERT_EQ(true, mode);
}

/**
 * @tc.name: CopyPrepareVisitorAndPackTaskTest
 * @tc.desc: Test RSParallelRenderManagerTest.CopyPrepareVisitorAndPackTaskTest
 * @tc.type: FUNC
 * @tc.require: issueI69JAV
 */
HWTEST_F(RSParallelRenderManagerTest, CopyPrepareVisitorAndPackTaskTest, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    RSDisplayNodeConfig displayConfig;
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(11, displayConfig, rsContext->weak_from_this());
    RSParallelRenderManager::Instance()->CopyPrepareVisitorAndPackTask(*rsUniRenderVisitor, *rsDisplayRenderNode);
    auto taskType = RSParallelRenderManager::Instance()->GetTaskType();
    ASSERT_EQ(taskType, TaskType::PREPARE_TASK);
}

/**
 * @tc.name: PackPrepareRenderTaskTest
 * @tc.desc: Test RSParallelRenderManagerTest.PackPrepareRenderTaskTest
 * @tc.type: FUNC
 * @tc.require: issueI69JAV
 */
HWTEST_F(RSParallelRenderManagerTest, PackPrepareRenderTaskTest, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config1;
    config1.id = 10;
    auto rsSurfaceRenderNode1 = std::make_shared<RSSurfaceRenderNode>(config1, rsContext->weak_from_this());
    rsSurfaceRenderNode1->SetSrcRect(RectI(0, 0, 10, 10));
    RSParallelRenderManager::Instance()->PackRenderTask(*rsSurfaceRenderNode1, TaskType::PREPARE_TASK);

    RSSurfaceRenderNodeConfig config2;
    config2.id = 20;
    auto rsSurfaceRenderNode2 = std::make_shared<RSSurfaceRenderNode>(config2, rsContext->weak_from_this());
    rsSurfaceRenderNode2->SetSrcRect(RectI(20, 20, 10, 10));
    RSParallelRenderManager::Instance()->PackRenderTask(*rsSurfaceRenderNode2, TaskType::PREPARE_TASK);

    RSSurfaceRenderNodeConfig config3;
    config3.id = 30;
    auto rsSurfaceRenderNode3 = std::make_shared<RSSurfaceRenderNode>(config3, rsContext->weak_from_this());
    rsSurfaceRenderNode3->SetSrcRect(RectI(40, 40, 10, 10));
    RSParallelRenderManager::Instance()->PackRenderTask(*rsSurfaceRenderNode3, TaskType::PREPARE_TASK);
    RSParallelRenderManager::Instance()->CommitSurfaceNum(10);
}

/**
 * @tc.name: CopyVisitorAndPackTaskTest
 * @tc.desc: Test RSParallelRenderManagerTest.CopyVisitorAndPackTaskTest
 * @tc.type: FUNC
 * @tc.require: issueI69JAV
 */
HWTEST_F(RSParallelRenderManagerTest, CopyVisitorAndPackTaskTest, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    RSDisplayNodeConfig displayConfig;
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(11, displayConfig, rsContext->weak_from_this());

    RSSurfaceRenderNodeConfig config1;
    config1.id = 10;
    auto rsSurfaceRenderNode1 = std::make_shared<RSSurfaceRenderNode>(config1, rsContext->weak_from_this());
    rsSurfaceRenderNode1->SetSrcRect(RectI(0, 0, 10, 10));
    RSParallelRenderManager::Instance()->PackRenderTask(*rsSurfaceRenderNode1, TaskType::PREPARE_TASK);

    RSSurfaceRenderNodeConfig config2;
    config2.id = 20;
    auto rsSurfaceRenderNode2 = std::make_shared<RSSurfaceRenderNode>(config2, rsContext->weak_from_this());
    rsSurfaceRenderNode2->SetSrcRect(RectI(20, 20, 10, 10));
    RSParallelRenderManager::Instance()->PackRenderTask(*rsSurfaceRenderNode2, TaskType::PREPARE_TASK);

    RSSurfaceRenderNodeConfig config3;
    config3.id = 30;
    auto rsSurfaceRenderNode3 = std::make_shared<RSSurfaceRenderNode>(config3, rsContext->weak_from_this());
    rsSurfaceRenderNode3->SetSrcRect(RectI(40, 40, 10, 10));

    rsDisplayRenderNode->AddChild(rsSurfaceRenderNode1);
    rsDisplayRenderNode->AddChild(rsSurfaceRenderNode2);
    rsDisplayRenderNode->AddChild(rsSurfaceRenderNode3);

    RSParallelRenderManager::Instance()->CopyVisitorAndPackTask(*rsUniRenderVisitor, *rsDisplayRenderNode);
    auto taskType = RSParallelRenderManager::Instance()->GetTaskType();
    ASSERT_EQ(taskType, TaskType::PROCESS_TASK);
}

/**
 * @tc.name: PackProcessRenderTaskTest
 * @tc.desc: Test RSParallelRenderManagerTest.PackProcessRenderTaskTest
 * @tc.type: FUNC
 * @tc.require: issueI69JAV
 */
HWTEST_F(RSParallelRenderManagerTest, PackProcessRenderTaskTest, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config1;
    config1.id = 10;
    auto rsSurfaceRenderNode1 = std::make_shared<RSSurfaceRenderNode>(config1, rsContext->weak_from_this());
    rsSurfaceRenderNode1->SetSrcRect(RectI(0, 0, 10, 10));
    RSParallelRenderManager::Instance()->PackRenderTask(*rsSurfaceRenderNode1, TaskType::PREPARE_TASK);

    RSSurfaceRenderNodeConfig config2;
    config2.id = 20;
    auto rsSurfaceRenderNode2 = std::make_shared<RSSurfaceRenderNode>(config2, rsContext->weak_from_this());
    rsSurfaceRenderNode2->SetSrcRect(RectI(20, 20, 10, 10));
    RSParallelRenderManager::Instance()->PackRenderTask(*rsSurfaceRenderNode2, TaskType::PREPARE_TASK);

    RSSurfaceRenderNodeConfig config3;
    config3.id = 30;
    auto rsSurfaceRenderNode3 = std::make_shared<RSSurfaceRenderNode>(config3, rsContext->weak_from_this());
    rsSurfaceRenderNode3->SetSrcRect(RectI(40, 40, 10, 10));

    RSParallelRenderManager::Instance()->PackRenderTask(*rsSurfaceRenderNode1, TaskType::PROCESS_TASK);
    RSParallelRenderManager::Instance()->PackRenderTask(*rsSurfaceRenderNode2, TaskType::PROCESS_TASK);
    RSParallelRenderManager::Instance()->PackRenderTask(*rsSurfaceRenderNode3, TaskType::PROCESS_TASK);

#ifndef USE_ROSEN_DRAWING
    SkCanvas tmpCanvas;
#else
    Drawing::Canvas tmpCanvas;
#endif
    RSPaintFilterCanvas canvas(&tmpCanvas);
    RSParallelRenderManager::Instance()->MergeRenderResult(canvas);
}

/**
 * @tc.name: SubmitSuperTaskTest
 * @tc.desc: Test RSParallelRenderManagerTest.SubmitSuperTaskTest
 * @tc.type: FUNC
 * @tc.require: issueI69JAV
 */
HWTEST_F(RSParallelRenderManagerTest, SubmitSuperTaskTest, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    RSDisplayNodeConfig displayConfig;
    config.id = 10;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(11, displayConfig, rsContext->weak_from_this());
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();

    rsSurfaceRenderNode->SetSrcRect(RectI(0, 0, 10, 10));
    rsSurfaceRenderNode->SetStaticCached(false);
    rsDisplayRenderNode->AddChild(rsSurfaceRenderNode, -1);
    auto instance = RSParallelRenderManager::Instance();
    auto superTask = std::make_unique<RSSuperRenderTask>(*rsDisplayRenderNode);
    for (uint32_t i = 0; i < threadNum_; i++) {
        instance->SubmitSuperTask(i, std::move(superTask));
        instance->SubMainThreadNotify(i);
        instance->WaitSubMainThread(i);
    }
}

/**
 * @tc.name: SetFrameSizeTest
 * @tc.desc: Test RSParallelRenderManagerTest.SetFrameSizeTest
 * @tc.type: FUNC
 * @tc.require: issueI69JAV
 */
HWTEST_F(RSParallelRenderManagerTest, SetFrameSizeTest, TestSize.Level1)
{
    int width = 2560;
    int height = 1600;
    RSParallelRenderManager::Instance()->SetFrameSize(width, height);
    int width0 = 0;
    int height0 = 0;
    RSParallelRenderManager::Instance()->GetFrameSize(width0, height0);
    ASSERT_EQ(width, width0);
    ASSERT_EQ(height, height0);
}

/**
 * @tc.name: AddSelfDrawingSurfaceTest
 * @tc.desc: Test RSParallelRenderManagerTest.AddSelfDrawingSurfaceTest
 * @tc.type: FUNC
 * @tc.require: issueI69JAV
 */
HWTEST_F(RSParallelRenderManagerTest, AddSelfDrawingSurfaceTest, TestSize.Level1)
{
#ifndef USE_ROSEN_DRAWING
    SkCanvas tmpCanvas;
#else
    Drawing::Canvas tmpCanvas;
#endif
    RSPaintFilterCanvas canvas(&tmpCanvas);
    auto instance = RSParallelRenderManager::Instance();
    instance->AddSelfDrawingSurface(0, false, { 0.f, 0.f, 100.f, 100.f });
    instance->ClearSelfDrawingSurface(canvas, 0);
}

/**
 * @tc.name: SetParallelModeTest2
 * @tc.desc: Test RSParallelRenderManagerTest.SetParallelModeTest2
 * @tc.type: FUNC
 * @tc.require: issueI69JAV
 */
HWTEST_F(RSParallelRenderManagerTest, SetParallelModeTest2, TestSize.Level1)
{
    auto instance = RSParallelRenderManager::Instance();
    instance->SetParallelMode(false);
    for (uint32_t i = 0; i < threadNum_; i++) {
        instance->SubMainThreadWait(i);
    }
    bool mode = instance->GetParallelMode();
    ASSERT_EQ(false, mode);
}

/**
 * @tc.name: LoadBalanceAndNotifyTest
 * @tc.desc: Test RSParallelRenderManagerTest.LoadBalanceAndNotifyTest
 * @tc.type: FUNC
 * @tc.require: issueI69JAV
 */
HWTEST_F(RSParallelRenderManagerTest, LoadBalanceAndNotifyTest, TestSize.Level1)
{
    auto renderManager = std::make_shared<RSParallelRenderManager>();
    renderManager->LoadBalanceAndNotify(TaskType::PREPARE_TASK);
    renderManager->LoadBalanceAndNotify(TaskType::PROCESS_TASK);
    renderManager->LoadBalanceAndNotify(TaskType::CALC_COST_TASK);
}

/**
 * @tc.name: TimingTest
 * @tc.desc: Test RSParallelRenderManagerTest.TimingTest
 * @tc.type: FUNC
 * @tc.require: issueI69JAV
 */
HWTEST_F(RSParallelRenderManagerTest, TimingTest, TestSize.Level1)
{
    auto renderManager = std::make_shared<RSParallelRenderManager>();
    renderManager->StartTiming(0);
    renderManager->StartTiming(1);
    renderManager->StartTiming(2);
    usleep(2000);
    renderManager->StopTimingAndSetRenderTaskCost(0, 10, TaskType::PREPARE_TASK);
    renderManager->StopTimingAndSetRenderTaskCost(1, 20, TaskType::PROCESS_TASK);
    renderManager->StopTimingAndSetRenderTaskCost(2, 30, TaskType::CALC_COST_TASK);
}


/**
 * @tc.name: AddAppWindiwTest
 * @tc.desc: Test RSParallelRenderManagerTest.AddAppWindiwTest
 * @tc.type: FUNC
 * @tc.require: issueI69JAV
 */
HWTEST_F(RSParallelRenderManagerTest, AddAppWindiwTest, TestSize.Level1)
{
    auto renderManager = std::make_shared<RSParallelRenderManager>();
    renderManager->InitAppWindowNodeMap();
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config1;
    config1.id = 10;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config1, rsContext->weak_from_this());
    renderManager->AddAppWindowNode(10, std::move(rsSurfaceRenderNode));
    RSSurfaceRenderNodeConfig config2;
    config2.id = 100;
    auto rsSurfaceRenderNode2 = std::make_shared<RSSurfaceRenderNode>(config2, rsContext->weak_from_this());
    renderManager->AddAppWindowNode(1, std::move(rsSurfaceRenderNode2));
}

/**
 * @tc.name: CopyCalcCostVisitorAndPackTaskTest
 * @tc.desc: Test RSParallelRenderManagerTest.CopyCalcCostVisitorAndPackTaskTest
 * @tc.type: FUNC
 * @tc.require: issueI6FZHQ
 */
HWTEST_F(RSParallelRenderManagerTest, CopyCalcCostVisitorAndPackTaskTest, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    RSDisplayNodeConfig displayConfig;
    auto instance = std::make_shared<RSParallelRenderManager>();
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(11, displayConfig, rsContext->weak_from_this());
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    auto result = instance->calcCostCount_;
    ASSERT_EQ(result, 0);
    instance->CopyCalcCostVisitorAndPackTask(*rsUniRenderVisitor, *rsDisplayRenderNode, true, true, true);
    result = instance->calcCostCount_;
    ASSERT_EQ(result, 19);
    auto result3 = instance->IsOpDropped();
    ASSERT_TRUE(result3);
    auto result4 = instance->IsSecurityDisplay();
    ASSERT_FALSE(result4);
}

/**
 * @tc.name: WaitCalcCostEndTest
 * @tc.desc: Test RSParallelRenderManagerTest.WaitCalcCostEndTest
 * @tc.type: FUNC
 * @tc.require: issueI6FZHQ
 */
HWTEST_F(RSParallelRenderManagerTest, WaitCalcCostEndTest, TestSize.Level1)
{
    auto instance = RSParallelRenderManager::Instance();
    instance->WaitCalcCostEnd();
}

/**
 * @tc.name: UpdateNodeCostTest
 * @tc.desc: Test RSParallelRenderManagerTest.UpdateNodeCostTest
 * @tc.type: FUNC
 * @tc.require: issueI6FZHQ
 */
HWTEST_F(RSParallelRenderManagerTest, UpdateNodeCostTest, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    RSDisplayNodeConfig displayConfig;
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(11, displayConfig, rsContext->weak_from_this());
    auto instance = std::make_shared<RSParallelRenderManager>();
    instance->parallelPolicy_ = { 1, 2, 3 };
    instance->UpdateNodeCost(*rsDisplayRenderNode);
    auto result = instance->parallelPolicy_.size();
    ASSERT_EQ(result, 0);
}

/**
 * @tc.name: IsNeedCalcCostTest
 * @tc.desc: Test RSParallelRenderManagerTest.IsNeedCalcCostTest
 * @tc.type: FUNC
 * @tc.require: issueI6FZHQ
 */
HWTEST_F(RSParallelRenderManagerTest, IsNeedCalcCostTest, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    RSDisplayNodeConfig displayConfig;
    auto instance = std::make_shared<RSParallelRenderManager>();
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(11, displayConfig, rsContext->weak_from_this());
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    instance->CopyCalcCostVisitorAndPackTask(*rsUniRenderVisitor, *rsDisplayRenderNode, true, true, true);
    auto result = instance->IsNeedCalcCost();
    ASSERT_EQ(result, true);
}

/**
 * @tc.name: GetCostTest
 * @tc.desc: Test RSParallelRenderManagerTest.GetCostTest
 * @tc.type: FUNC
 * @tc.require: issueI6FZHQ
 */
HWTEST_F(RSParallelRenderManagerTest, GetCostTest1, TestSize.Level1)
{
    RSRenderNode rsRenderNode(1);
    auto instance = std::make_shared<RSParallelRenderManager>();
    rsRenderNode.GetMutableRenderProperties().SetAlpha(1.0f);
    auto cost = instance->GetCost(rsRenderNode);
    ASSERT_EQ(cost, 2);
}

/**
 * @tc.name: GetCostTest
 * @tc.desc: Test RSParallelRenderManagerTest.GetCostTest
 * @tc.type: FUNC
 * @tc.require: issueI6FZHQ
 */
HWTEST_F(RSParallelRenderManagerTest, GetCostTest2, TestSize.Level1)
{
    RSRenderNode rsRenderNode(1);
    auto instance = std::make_shared<RSParallelRenderManager>();
    rsRenderNode.GetMutableRenderProperties().SetAlpha(1.0f);
    rsRenderNode.GetMutableRenderProperties().backgroundFilter_ = std::make_shared<RSFilter>();
    auto cost = instance->GetCost(rsRenderNode);
    ASSERT_EQ(cost, 2);
}

/**
 * @tc.name: GetCostTest
 * @tc.desc: Test RSParallelRenderManagerTest.GetCostTest
 * @tc.type: FUNC
 * @tc.require: issueI6FZHQ
 */
HWTEST_F(RSParallelRenderManagerTest, GetCostTest, TestSize.Level1)
{
    RSRenderNode rsRenderNode(1);
    auto instance = std::make_shared<RSParallelRenderManager>();
    rsRenderNode.GetMutableRenderProperties().SetAlpha(1.0f);
    rsRenderNode.GetMutableRenderProperties().backgroundFilter_ = std::make_shared<RSFilter>();
    rsRenderNode.GetMutableRenderProperties().decoration_ = std::make_optional<Decoration>();
    rsRenderNode.GetMutableRenderProperties().decoration_->bgImage_ = std::make_shared<RSImage>();
    auto cost = instance->GetCost(rsRenderNode);
    ASSERT_EQ(cost, 2);
}

/**
 * @tc.name: GetSelfDrawNodeCostTest
 * @tc.desc: Test RSParallelRenderManagerTest.GetSelfDrawNodeCostTest
 * @tc.type: FUNC
 * @tc.require: issueI6FZHQ
 */
HWTEST_F(RSParallelRenderManagerTest, GetSelfDrawNodeCostTest, TestSize.Level1)
{
    auto instance = std::make_shared<RSParallelRenderManager>();
    auto result = instance->GetSelfDrawNodeCost();
    ASSERT_EQ(result, 1);
}

/**
 * @tc.name: GetCostFactorTest
 * @tc.desc: Test RSParallelRenderManagerTest.GetCostFactorTest
 * @tc.type: FUNC
 * @tc.require: issueI6FZHQ
 */
HWTEST_F(RSParallelRenderManagerTest, GetCostFactorTest, TestSize.Level1)
{
    auto instance = std::make_shared<RSParallelRenderManager>();
    instance->GetCostFactor();
    if (instance->calcCostTaskManager_.isParallelRenderExtEnabled_) {
        ASSERT_TRUE(instance->costFactor_.size() > 0);
        ASSERT_TRUE(instance->imageFactor_.size() > 0);
    } else {
        ASSERT_FALSE(instance->costFactor_.size() > 0);
        ASSERT_FALSE(instance->imageFactor_.size() > 0);
    }
}

/**
 * @tc.name: IsSecurityDisplayTest
 * @tc.desc: Test RSParallelRenderManagerTest.IsSecurityDisplayTest
 * @tc.type: FUNC
 * @tc.require: issueI6FZHQ
 */
HWTEST_F(RSParallelRenderManagerTest, IsSecurityDisplayTest, TestSize.Level1)
{
    auto instance = std::make_shared<RSParallelRenderManager>();
    auto result = instance->IsSecurityDisplay();
    ASSERT_FALSE(result);
}

/**
 * @tc.name: PackParallelCompositionTask
 * @tc.desc: Test RSParallelRenderManagerTest.PackParallelCompositionTask
 * @tc.type: FUNC
 * @tc.require: AR000HQ6GH
 */
HWTEST_F(RSParallelRenderManagerTest, PackParallelCompositionTask, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    NodeId id = 1;
    RSDisplayNodeConfig mainNodeConfig;
    auto mainDisplayNode = std::make_shared<RSDisplayRenderNode>(id, mainNodeConfig, rsContext->weak_from_this());
    auto rsBaseRenderNode = std::make_shared<RSBaseRenderNode>(11, rsContext->weak_from_this());
    RSDisplayNodeConfig displayConfig1 = {
        .screenId = 0,
        .isMirrored = false,
        .mirrorNodeId = 0,
    };
    auto rsDisplayRenderNode1 = std::make_shared<RSDisplayRenderNode>(21, displayConfig1, rsContext->weak_from_this());
    rsBaseRenderNode->AddChild(rsDisplayRenderNode1);
    RSDisplayNodeConfig displayConfig2 = {
        .screenId = 1,
        .isMirrored = true,
        .mirrorNodeId = 21,
    };
    auto rsDisplayRenderNode2 = std::make_shared<RSDisplayRenderNode>(31, displayConfig2, rsContext->weak_from_this());
    rsBaseRenderNode->AddChild(rsDisplayRenderNode2);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    auto parallelRenderManager = RSParallelRenderManager::Instance();
    ParallelStatus status = parallelRenderManager->GetParallelRenderingStatus();
    ASSERT_EQ(status, ParallelStatus::OFF);
    parallelRenderManager->SetParallelMode(true);
    status = parallelRenderManager->GetParallelRenderingStatus();
    ASSERT_EQ(status, ParallelStatus::FIRSTFLUSH);
    auto visitor = parallelRenderManager->GetUniParallelCompositionVisitor();
    ASSERT_EQ(visitor, nullptr);
    parallelRenderManager->mainDisplayNode_ = mainDisplayNode;
    parallelRenderManager->PackParallelCompositionTask(rsUniRenderVisitor, rsBaseRenderNode);
    visitor = parallelRenderManager->GetUniParallelCompositionVisitor();
    ASSERT_NE(visitor, nullptr);
    auto taskType = parallelRenderManager->GetTaskType();
    ASSERT_EQ(taskType, TaskType::COMPOSITION_TASK);
    parallelRenderManager->LoadBalanceAndNotify(TaskType::COMPOSITION_TASK);
    parallelRenderManager->WaitCompositionEnd();
    parallelRenderManager->SetParallelMode(false);
    status = parallelRenderManager->GetParallelRenderingStatus();
    ASSERT_EQ(status, ParallelStatus::FIRSTFLUSH);
    parallelRenderManager->mainDisplayNode_ = nullptr;
}

#ifdef USE_ROSEN_DRAWING
/**
 * @tc.name: DrawImageMergeFuncForRosenDrawing
 * @tc.desc: Test RSParallelRenderManagerTest.DrawImageMergeFuncForRosenDrawing
 * @tc.type: FUNC
 * @tc.require: AR000HQ6GH
 */
HWTEST_F(RSParallelRenderManagerTest, DrawImageMergeFuncForRosenDrawing, TestSize.Level1)
{
    std::shared_ptr<Drawing::Image> img;
    Drawing::Canvas tmpCanvas;
    RSPaintFilterCanvas canvas(&tmpCanvas);
    bool ret = RSParallelRenderManager::Instance()->DrawImageMergeFuncForRosenDrawing(canvas, img);
    EXPECT_NE(ret, true);
}
#endif

/**
 * @tc.name: SetParallelMode
 * @tc.desc: Test RSParallelRenderManagerTest.SetParallelMode
 * @tc.type: FUNC
 * @tc.require: issueI69JAV
 */
HWTEST_F(RSParallelRenderManagerTest, SetParallelMode, TestSize.Level1)
{
    auto instance = RSParallelRenderManager::Instance();
    instance->SetParallelMode(false);
    instance->StartSubRenderThread(threadNum_, nullptr);

#ifndef USE_ROSEN_DRAWING
    SkCanvas tmpCanvas;
#else
    Drawing::Canvas tmpCanvas;
#endif
    RSPaintFilterCanvas canvas(&tmpCanvas);
    RSParallelRenderManager::Instance()->DrawImageMergeFunc(canvas);
}
} // namespace OHOS::Rosen