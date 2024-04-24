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

#include "gtest/gtest.h"
#include "limit_number.h"
#include "pipeline/parallel_render/rs_render_task.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderTaskTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    void CreateRenderTask();
    std::unique_ptr<RSRenderTask> renderTask_;
    std::unique_ptr<RSSuperRenderTask> supRenderTask_;
    std::shared_ptr<RSDisplayRenderNode> rsDisplayRenderNode_;
    std::shared_ptr<RSSurfaceRenderNode> rsSurfRenderNode_;
};

void RSRenderTaskTest::SetUpTestCase() {}
void RSRenderTaskTest::TearDownTestCase() {}
void RSRenderTaskTest::SetUp()
{
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.id = 1;
    rsSurfRenderNode_ = std::make_shared<RSSurfaceRenderNode>(surfaceNodeConfig, rsContext->weak_from_this());
    ASSERT_NE(rsSurfRenderNode_, nullptr);
    
    RSDisplayNodeConfig displayNodeConfig;
    rsDisplayRenderNode_ = std::make_shared<RSDisplayRenderNode>(0, displayNodeConfig, rsContext->weak_from_this());
    ASSERT_NE(rsDisplayRenderNode_, nullptr);

    renderTask_ = std::make_unique<RSRenderTask>(*rsSurfRenderNode_, RSRenderTask::RenderNodeStage::PROCESS);
    supRenderTask_ = std::make_unique<RSSuperRenderTask>(*rsDisplayRenderNode_);
}
void RSRenderTaskTest::TearDown()
{
    renderTask_ = nullptr;
    supRenderTask_ = nullptr;
    rsDisplayRenderNode_ = nullptr;
    rsSurfRenderNode_ = nullptr;
}

/**
 * @tc.name: ChangeRenderTaskIdx
 * @tc.desc: Test RSRenderTaskBase.SetIdx, RSRenderTaskBase.GetIdx
 * @tc.type: FUNC
 * @tc.require: issueI69JAV
 */
HWTEST_F(RSRenderTaskTest, IndexTest, TestSize.Level1)
{
    uint64_t setIdx = 10;
    renderTask_->SetIdx(setIdx);
    auto getIdx = renderTask_->GetIdx();
    ASSERT_EQ(setIdx, getIdx);
}

/**
 * @tc.name: CreateRenderTask
 * @tc.desc: Test Create RSRenderTask
 * @tc.type: FUNC
 * @tc.require: issueI69JAV
 */
HWTEST_F(RSRenderTaskTest, CreateRenderTaskTest, TestSize.Level1)
{
    auto prepRenderTask = std::make_unique<RSRenderTask>(*rsSurfRenderNode_, RSRenderTask::RenderNodeStage::PREPARE);
    auto prepareTaskType = prepRenderTask->GetNodeStage();
    ASSERT_EQ(prepareTaskType, RSRenderTask::RenderNodeStage::PREPARE);
    auto procRenderTask = std::make_unique<RSRenderTask>(*rsSurfRenderNode_, RSRenderTask::RenderNodeStage::PROCESS);
    auto processTaskType = procRenderTask->GetNodeStage();
    ASSERT_EQ(processTaskType, RSRenderTask::RenderNodeStage::PROCESS);
}

/**
 * @tc.name: AddTaskTest
 * @tc.desc: Test RSSuperRenderTaskTest.AddTaskTest
 * @tc.type: FUNC
 * @tc.require: issueI69JAV
 */
HWTEST_F(RSRenderTaskTest, AddTaskTest, TestSize.Level1)
{
    auto renderTask0 = std::make_unique<RSRenderTask>(*rsSurfRenderNode_, RSRenderTask::RenderNodeStage::PREPARE);
    renderTask0->SetIdx(0);
    supRenderTask_->AddTask(std::move(renderTask0));
    auto renderTask1 = std::make_unique<RSRenderTask>(*rsSurfRenderNode_, RSRenderTask::RenderNodeStage::PROCESS);
    renderTask1->SetIdx(1);
    supRenderTask_->AddTask(std::move(renderTask1));
    auto taskSize = supRenderTask_->GetTaskSize();
    auto actualTaskSize = 2;
    ASSERT_EQ(taskSize, actualTaskSize);
    supRenderTask_->AddTask(nullptr);
    ASSERT_EQ(taskSize, actualTaskSize);
}

/**
 * @tc.name: GetSurfaceNode
 * @tc.desc: Test RSSuperRenderTaskTest.GetSurfaceNode
 * @tc.type: FUNC
 * @tc.require: issueI69JAV
 */
HWTEST_F(RSRenderTaskTest, GetSurfaceNode, TestSize.Level1)
{
    auto renderTask = std::make_unique<RSRenderTask>(*rsSurfRenderNode_, RSRenderTask::RenderNodeStage::PREPARE);
    supRenderTask_->AddTask(std::move(renderTask));
    auto surfaceNode = supRenderTask_->GetSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
}

/**
 * @tc.name: GetNextRenderTask
 * @tc.desc: Test RSSuperRenderTaskTest.GetNextRenderTask
 * @tc.type: FUNC
 * @tc.require: issueI69JAV
 */
HWTEST_F(RSRenderTaskTest, GetNextRenderTask, TestSize.Level1)
{
    auto renderTask = std::make_unique<RSRenderTask>(*rsSurfRenderNode_, RSRenderTask::RenderNodeStage::PREPARE);
    renderTask->SetIdx(0);
    supRenderTask_->AddTask(std::move(renderTask));
    auto task = supRenderTask_->GetNextRenderTask();
    auto id = task->GetIdx();
    auto actualId = 0;
    ASSERT_EQ(id, actualId);
}

/**
 * @tc.name: GetSurfaceNodeEmpty
 * @tc.desc: Test RSSuperRenderTaskTest.GetSurfaceNode empty
 * @tc.type: FUNC
 * @tc.require: issueI69JAV
 */
HWTEST_F(RSRenderTaskTest, GetSurfaceNodeEmpty, TestSize.Level1)
{
    auto surfaceNode = supRenderTask_->GetSurfaceNode();
    ASSERT_EQ(surfaceNode, nullptr);
}

/**
 * @tc.name: GetNextRenderTaskEmpty
 * @tc.desc: Test RSSuperRenderTaskTest.GetNextRenderTask empty
 * @tc.type: FUNC
 * @tc.require: issueI69JAV
 */
HWTEST_F(RSRenderTaskTest, GetNextRenderTaskEmpty, TestSize.Level1)
{
    auto task = supRenderTask_->GetNextRenderTask();
    ASSERT_EQ(task, nullptr);
}

/**
 * @tc.name: RSCompositionTask
 * @tc.desc: Test RSSuperRenderTaskTest.RSCompositionTask construct OK
 * @tc.type: FUNC
 * @tc.require: AR000HQ6GH
 */
HWTEST_F(RSRenderTaskTest, RSCompositionTask, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    RSDisplayNodeConfig displayConfig;
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(10, displayConfig, rsContext->weak_from_this());
    auto compositionTask = std::make_shared<RSCompositionTask>(rsDisplayRenderNode);
    ASSERT_NE(compositionTask->GetIdx(), 10);
    compositionTask->SetIdx(20);
    ASSERT_EQ(compositionTask->GetIdx(), 20);
}

} // namespace OHOS::Rosen