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

#include <memory>
#include "gtest/gtest.h"
#include "limit_number.h"
#include "pipeline/parallel_render/rs_parallel_sub_thread.h"
#include "pipeline/parallel_render/rs_parallel_render_manager.h"
#include "render_context/render_context.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_uni_render_visitor.h"
#include "pipeline/rs_render_node.h"
#include "screen_manager/rs_screen_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSParallelSubThreadTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSParallelSubThreadTest::SetUpTestCase() {}
void RSParallelSubThreadTest::TearDownTestCase() {}
void RSParallelSubThreadTest::SetUp() {}
void RSParallelSubThreadTest::TearDown() {}


/**
 * @tc.name: CreateResourceTest
 * @tc.desc: Test RSParallelSubThreadTest.CreateResourceTest
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
 HWTEST_F(RSParallelSubThreadTest, CreateResourceTest, TestSize.Level1)
 {
    auto renderContext = new RenderContext();
    ASSERT_TRUE(renderContext != nullptr);
    renderContext->InitializeEglContext();
    auto curThread = std::make_unique<RSParallelSubThread>(nullptr, ParallelRenderType::FLUSH_ONE_BUFFER, 0);
    RSParallelRenderManager::Instance()->SetFrameSize(100, 100);
    curThread->CreateResource();
    ASSERT_TRUE(curThread->grContext_ != nullptr);
    curThread->CreateResource();
    RSParallelRenderManager::Instance()->SetFrameSize(100, 300);
    curThread->CreateResource();
    RSParallelRenderManager::Instance()->SetFrameSize(300, 300);
    curThread->CreateResource();
    delete renderContext;
    renderContext = nullptr;
 }

/**
 * @tc.name: StartPrepareTest
 * @tc.desc: Test RSParallelSubThreadTest.StartPrepareTest
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
 HWTEST_F(RSParallelSubThreadTest, StartPrepareTest, TestSize.Level1)
 {
    auto curThread = std::make_unique<RSParallelSubThread>(nullptr, ParallelRenderType::DRAW_IMAGE, 0);
    if (RSParallelRenderManager::Instance()->uniVisitor_ == nullptr) {
        RSParallelRenderManager::Instance()->uniVisitor_ = new RSUniRenderVisitor();
        curThread->StartPrepare();
        ASSERT_TRUE(curThread->visitor_ != nullptr);
        delete RSParallelRenderManager::Instance()->uniVisitor_;
        RSParallelRenderManager::Instance()->uniVisitor_ = nullptr;
        curThread->StartPrepare();
    } else {
        curThread->StartPrepare();
        auto visitor = RSParallelRenderManager::Instance()->uniVisitor_;
        RSParallelRenderManager::Instance()->uniVisitor_ = nullptr;
        curThread->StartPrepare();
        RSParallelRenderManager::Instance()->uniVisitor_ = visitor;
    }
    ASSERT_TRUE(curThread->visitor_ != nullptr);
 }

/**
 * @tc.name: PrepareTest
 * @tc.desc: Test RSParallelSubThreadTest.PrepareTest
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
 HWTEST_F(RSParallelSubThreadTest, PrepareTest, TestSize.Level1)
{
    auto curThread = std::make_unique<RSParallelSubThread>(0);
    curThread->threadTask_ = nullptr;
    curThread->Prepare();
    auto rsContext = std::make_shared<RSContext>();
    RSDisplayNodeConfig displayConfig;
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(1, displayConfig, rsContext->weak_from_this());
    curThread->threadTask_ = std::make_unique<RSSuperRenderTask>(rsDisplayRenderNode);
    curThread->Prepare();
    RSSurfaceRenderNodeConfig config;
    config.id = 100;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    std::unique_ptr<RSRenderTask> task =
        std::make_unique<RSRenderTask>(*rsSurfaceRenderNode, RSRenderTask::RenderNodeStage::PREPARE);
    curThread->threadTask_->AddTask(std::move(task));
    auto rsSurfaceRenderNode2 = std::make_shared<RSSurfaceRenderNode>(0, rsContext->weak_from_this());
    std::unique_ptr<RSRenderTask> task2 =
        std::make_unique<RSRenderTask>(*rsSurfaceRenderNode2, RSRenderTask::RenderNodeStage::PREPARE);
    curThread->threadTask_->AddTask(std::move(task2));
    curThread->threadTask_->AddTask(nullptr);
    ASSERT_TRUE(curThread->threadTask_->GetTaskSize() > 0);
    curThread->Prepare();
}

/**
 * @tc.name: RenderTest
 * @tc.desc: Test RSParallelSubThreadTest.RenderTest
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSParallelSubThreadTest, RenderTest, TestSize.Level1)
{
    auto curThread = std::make_unique<RSParallelSubThread>(0);
    curThread->renderType_ = ParallelRenderType::DRAW_IMAGE;
    curThread->threadTask_ = nullptr;
    curThread->Render();
    auto rsContext = std::make_shared<RSContext>();
    RSDisplayNodeConfig displayConfig;
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(1, displayConfig, rsContext->weak_from_this());
    curThread->threadTask_ = std::make_unique<RSSuperRenderTask>(rsDisplayRenderNode);
    curThread->canvas_ = nullptr;
    curThread->Render();
    curThread->skCanvas_ = new SkCanvas();
    ASSERT_TRUE(curThread->skCanvas_ != nullptr);
    RSSurfaceRenderNodeConfig config;
    config.id = 100;
    curThread->canvas_ = std::make_shared<RSPaintFilterCanvas>(curThread->skCanvas_);
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    std::unique_ptr<RSRenderTask> task =
        std::make_unique<RSRenderTask>(*rsSurfaceRenderNode, RSRenderTask::RenderNodeStage::PREPARE);
    curThread->threadTask_->AddTask(std::move(task));
    auto rsSurfaceRenderNode2 = std::make_shared<RSSurfaceRenderNode>(0, rsContext->weak_from_this());
    std::unique_ptr<RSRenderTask> task2 =
        std::make_unique<RSRenderTask>(*rsSurfaceRenderNode2, RSRenderTask::RenderNodeStage::PREPARE);
    curThread->threadTask_->AddTask(std::move(task2));
    curThread->threadTask_->AddTask(nullptr);
    ASSERT_TRUE(curThread->threadTask_->GetTaskSize() > 0);
    curThread->Render();
    delete curThread->skCanvas_;
    curThread->skCanvas_ = nullptr;
}

/**
 * @tc.name: FlushTest
 * @tc.desc: Test RSParallelSubThreadTest.FlushTest
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSParallelSubThreadTest, FlushTest, TestSize.Level1)
{
    auto curThread = std::make_unique<RSParallelSubThread>(nullptr, ParallelRenderType::FLUSH_ONE_BUFFER, 0);
    curThread->skCanvas_ = nullptr;
    curThread->Flush();
    curThread->skCanvas_ = new SkCanvas();
    ASSERT_TRUE(curThread->skCanvas_ != nullptr);
    curThread->canvas_ = std::make_shared<RSPaintFilterCanvas>(curThread->skCanvas_);
    RSParallelRenderManager::Instance()->expectedSubThreadNum_ = 3;
    RSParallelRenderManager::Instance()->readySubThreadNum_ = 1;
    RSParallelRenderManager::Instance()->firstFlush_ = true;
    curThread->Flush();
    RSParallelRenderManager::Instance()->firstFlush_ = false;
    curThread->Flush();
    delete curThread->skCanvas_;
    curThread->skCanvas_ = nullptr;
}

/**
 * @tc.name: WaitReleaseFenceTest
 * @tc.desc: Test RSParallelSubThreadTest.WaitReleaseFenceTest
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
 HWTEST_F(RSParallelSubThreadTest, WaitReleaseFenceTest, TestSize.Level1)
 {
    auto renderContext = new RenderContext();
    ASSERT_TRUE(renderContext != nullptr);
    auto curThread = std::make_unique<RSParallelSubThread>(renderContext, ParallelRenderType::FLUSH_ONE_BUFFER, 0);
    curThread->eglSync_ = EGL_NO_SYNC_KHR;
    curThread->WaitReleaseFence();
    curThread->eglSync_ = (EGLSyncKHR)EGL_SIGNALED_KHR;
    curThread->WaitReleaseFence();
    delete renderContext;
    renderContext = nullptr;
 }

/**
 * @tc.name: CalcCostTest
 * @tc.desc: Test RSParallelSubThreadTest.CalcCostTest
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSParallelSubThreadTest, CalcCostTest1, TestSize.Level1)
{
    auto curThread = std::make_unique<RSParallelSubThread>(0);
    curThread->CalcCost();
    ASSERT_TRUE(curThread->threadTask_ == nullptr);
}

/**
 * @tc.name: CalcCostTest
 * @tc.desc: Test RSParallelSubThreadTest.CalcCostTest
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSParallelSubThreadTest, CalcCostTest2, TestSize.Level1)
{
    auto curThread = std::make_unique<RSParallelSubThread>(0);
    auto rsContext = std::make_shared<RSContext>();
    RSDisplayNodeConfig displayConfig;
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(1, displayConfig, rsContext->weak_from_this());
    curThread->threadTask_ = std::make_unique<RSSuperRenderTask>(rsDisplayRenderNode);
    RSParallelRenderManager::Instance()->uniVisitor_ = nullptr;
    curThread->CalcCost();
    ASSERT_FALSE(curThread->threadTask_ == nullptr);
}

/**
 * @tc.name: CalcCostTest
 * @tc.desc: Test RSParallelSubThreadTest.CalcCostTest
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSParallelSubThreadTest, CalcCostTest3, TestSize.Level1)
{
    auto curThread = std::make_unique<RSParallelSubThread>(0);
    auto rsContext = std::make_shared<RSContext>();
    RSDisplayNodeConfig displayConfig;
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(1, displayConfig, rsContext->weak_from_this());
    curThread->threadTask_ = std::make_unique<RSSuperRenderTask>(rsDisplayRenderNode);
    RSParallelRenderManager::Instance()->uniVisitor_ = new RSUniRenderVisitor();
    ASSERT_TRUE(RSParallelRenderManager::Instance()->uniVisitor_ != nullptr);
    curThread->CalcCost();
    delete RSParallelRenderManager::Instance()->uniVisitor_;
    RSParallelRenderManager::Instance()->uniVisitor_ = nullptr;
    ASSERT_FALSE(curThread->threadTask_ == nullptr);
}
/**
 * @tc.name: CalcCostTest
 * @tc.desc: Test RSParallelSubThreadTest.CalcCostTest
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSParallelSubThreadTest, CalcCostTest4, TestSize.Level1)
{
    auto curThread = std::make_unique<RSParallelSubThread>(0);
    auto rsContext = std::make_shared<RSContext>();
    RSDisplayNodeConfig displayConfig;
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(1, displayConfig, rsContext->weak_from_this());
    curThread->threadTask_ = std::make_unique<RSSuperRenderTask>(rsDisplayRenderNode);
    RSParallelRenderManager::Instance()->uniVisitor_ = new RSUniRenderVisitor();
    ASSERT_TRUE(RSParallelRenderManager::Instance()->uniVisitor_ != nullptr);
    RSSurfaceRenderNodeConfig config;
    config.id = 10;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    std::unique_ptr<RSRenderTask> task =
        std::make_unique<RSRenderTask>(*rsSurfaceRenderNode, RSRenderTask::RenderNodeStage::PREPARE);
    curThread->threadTask_->AddTask(std::move(task));
    auto rsSurfaceRenderNode2 = std::make_shared<RSSurfaceRenderNode>(0, rsContext->weak_from_this());
    std::unique_ptr<RSRenderTask> task2 =
        std::make_unique<RSRenderTask>(*rsSurfaceRenderNode2, RSRenderTask::RenderNodeStage::PREPARE);
    curThread->threadTask_->AddTask(std::move(task2));
    curThread->threadTask_->AddTask(nullptr);
    ASSERT_TRUE(curThread->threadTask_->GetTaskSize() > 0);
    curThread->CalcCost();
    ASSERT_FALSE(curThread->threadTask_ == nullptr);
    delete RSParallelRenderManager::Instance()->uniVisitor_;
    RSParallelRenderManager::Instance()->uniVisitor_ = nullptr;
}

} // namespace OHOS::Rosen