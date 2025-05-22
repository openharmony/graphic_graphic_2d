/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "pipeline/parallel_render/rs_sub_thread.h"
#include "pipeline/rs_uifirst_manager.h"
#include "pipeline/rs_base_render_engine.h"
#include "pipeline/rs_uni_render_thread.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RsSubThreadTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RsSubThreadTest::SetUpTestCase() {}
void RsSubThreadTest::TearDownTestCase() {}
void RsSubThreadTest::SetUp() {}
void RsSubThreadTest::TearDown() {}

/**
 * @tc.name: PostTaskTest
 * @tc.desc: Test RsSubThreadTest.PostTaskTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RsSubThreadTest, PostTaskTest, TestSize.Level1)
{
    auto renderContext = new RenderContext();
    ASSERT_TRUE(renderContext != nullptr);
    renderContext->InitializeEglContext();
    auto curThread = std::make_shared<RSSubThread>(renderContext, 0);
    curThread->PostTask([] {});
    delete renderContext;
    renderContext = nullptr;
    usleep(1000 * 1000); // 1000 * 1000us
}

/**
 * @tc.name: CreateShareEglContextTest
 * @tc.desc: Test RsSubThreadTest.CreateShareEglContextTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RsSubThreadTest, CreateShareEglContextTest, TestSize.Level1)
{
    auto curThread1 = std::make_shared<RSSubThread>(nullptr, 0);
    curThread1->CreateShareEglContext();
    auto renderContext = new RenderContext();
    ASSERT_TRUE(renderContext != nullptr);
    renderContext->InitializeEglContext();
    auto curThread2 = std::make_shared<RSSubThread>(renderContext, 0);
    curThread2->CreateShareEglContext();
    delete renderContext;
    renderContext = nullptr;
}

/**
 * @tc.name: DestroyShareEglContextgTest
 * @tc.desc: Test RsSubThreadTest.DestroyShareEglContextgTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RsSubThreadTest, DestroyShareEglContextTest, TestSize.Level1)
{
    auto curThread1 = std::make_shared<RSSubThread>(nullptr, 0);
    curThread1->DestroyShareEglContext();
    auto renderContext = new RenderContext();
    ASSERT_TRUE(renderContext != nullptr);
    renderContext->InitializeEglContext();
    auto curThread2 = std::make_shared<RSSubThread>(renderContext, 0);
    curThread2->DestroyShareEglContext();
    delete renderContext;
    renderContext = nullptr;
}

/**
 * @tc.name: ResetGrContext
 * @tc.desc: Test RsSubThreadTest.ResetGrContext
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RsSubThreadTest, ResetGrContext, TestSize.Level1)
{
    auto curThread = std::make_shared<RSSubThread>(nullptr, 0);
    ASSERT_TRUE(curThread != nullptr);
    curThread->ResetGrContext();
}

/**
 * @tc.name: AddToReleaseQueue
 * @tc.desc: Test RsSubThreadTest.AddToReleaseQueue
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RsSubThreadTest, AddToReleaseQueue, TestSize.Level1)
{
    const Drawing::ImageInfo info = Drawing::ImageInfo { 200, 200, Drawing::COLORTYPE_N32, Drawing::ALPHATYPE_OPAQUE };
    auto surface(Drawing::Surface::MakeRaster(info));
    auto curThread = std::make_shared<RSSubThread>(nullptr, 0);
    ASSERT_TRUE(curThread != nullptr);
    curThread->AddToReleaseQueue(std::move(surface));
}

/**
 * @tc.name: RenderCache
 * @tc.desc: Test RsSubThreadTest.RenderCache
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RsSubThreadTest, RenderCache, TestSize.Level1)
{
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(id, config);
    const std::shared_ptr<RSSuperRenderTask> threadTask_ = std::make_shared<RSSuperRenderTask>(rsDisplayRenderNode);

    auto curThread = std::make_shared<RSSubThread>(nullptr, 0);
    ASSERT_TRUE(curThread != nullptr);
    curThread->RenderCache(threadTask_);
}

/**
 * @tc.name: PostSyncTaskTest001
 * @tc.desc: Verify function PostSyncTask
 * @tc.type:FUNC
 */
HWTEST_F(RsSubThreadTest, PostSyncTaskTest001, TestSize.Level1)
{
    auto renderContext = std::make_shared<RenderContext>();
    auto curThread = std::make_shared<RSSubThread>(renderContext.get(), 0);
    curThread->PostSyncTask([] {});
    curThread->Start();
    curThread->PostSyncTask([] {});
    EXPECT_TRUE(curThread->handler_);
}

/**
 * @tc.name: RemoveTaskTest001
 * @tc.desc: Verify function RemoveTask
 * @tc.type:FUNC
 */
HWTEST_F(RsSubThreadTest, RemoveTaskTest001, TestSize.Level1)
{
    auto renderContext = std::make_shared<RenderContext>();
    auto curThread = std::make_shared<RSSubThread>(renderContext.get(), 0);
    curThread->RemoveTask("1");
    curThread->Start();
    curThread->RemoveTask("2");
    EXPECT_TRUE(curThread->handler_);
}

/**
 * @tc.name: DumpMemTest001
 * @tc.desc: Verify function DumpMem
 * @tc.type:FUNC
 */
HWTEST_F(RsSubThreadTest, DumpMemTest001, TestSize.Level1)
{
    auto renderContext = std::make_shared<RenderContext>();
    auto curThread = std::make_shared<RSSubThread>(renderContext.get(), 0);
    DfxString log;
    auto grContext = std::make_shared<Drawing::GPUContext>();
    curThread->grContext_ = grContext;
    curThread->DumpMem(log);
    if (grContext) {
        EXPECT_TRUE(curThread->grContext_);
    } else {
        EXPECT_FALSE(curThread->grContext_);
    }
}

/**
 * @tc.name: GetAppGpuMemoryInMBTest001
 * @tc.desc: Verify function GetAppGpuMemoryInMB
 * @tc.type:FUNC
 */
HWTEST_F(RsSubThreadTest, GetAppGpuMemoryInMBTest001, TestSize.Level1)
{
    auto renderContext = std::make_shared<RenderContext>();
    auto curThread = std::make_shared<RSSubThread>(renderContext.get(), 0);
    curThread->grContext_ = std::make_shared<Drawing::GPUContext>();
    curThread->GetAppGpuMemoryInMB();
    EXPECT_TRUE(curThread->grContext_);
}

/**
 * @tc.name: ThreadSafetyReleaseTextureTest001
 * @tc.desc: Verify function ThreadSafetyReleaseTexture
 * @tc.type:FUNC
 */
HWTEST_F(RsSubThreadTest, ThreadSafetyReleaseTextureTest001, TestSize.Level1)
{
    auto renderContext = std::make_shared<RenderContext>();
    auto curThread = std::make_shared<RSSubThread>(renderContext.get(), 0);
    curThread->grContext_ = std::make_shared<Drawing::GPUContext>();
    curThread->ThreadSafetyReleaseTexture();
    EXPECT_TRUE(curThread->grContext_);
}

/**
 * @tc.name: ReleaseSurfaceTest001
 * @tc.desc: Verify function ReleaseSurface
 * @tc.type:FUNC
 */
HWTEST_F(RsSubThreadTest, ReleaseSurfaceTest001, TestSize.Level1)
{
    auto renderContext = std::make_shared<RenderContext>();
    auto curThread = std::make_shared<RSSubThread>(renderContext.get(), 0);
    auto graphicsSurface = std::make_shared<Drawing::Surface>();
    curThread->AddToReleaseQueue(std::move(graphicsSurface));
    curThread->ReleaseSurface();
    EXPECT_TRUE(curThread->tmpSurfaces_.empty());
}

/**
 * @tc.name: CreateShareEglContext001
 * @tc.desc: Verify function CreateShareEglContext
 * @tc.type: FUNC
 * @tc.require: issueIAE59W
 */
HWTEST_F(RsSubThreadTest, CreateShareEglContext001, TestSize.Level1)
{
    auto renderContext = std::make_shared<RenderContext>();
    auto curThread = std::make_shared<RSSubThread>(renderContext.get(), 0);
    curThread->CreateShareEglContext();
    EXPECT_TRUE(curThread->renderContext_);

#ifdef RS_ENABLE_GL
    curThread->CreateShareEglContext();
    EXPECT_TRUE(curThread->renderContext_);
#endif

    curThread->renderContext_ = nullptr;
    curThread->CreateShareEglContext();
    EXPECT_FALSE(curThread->renderContext_);
}

/**
 * @tc.name: RenderCache001
 * @tc.desc: Verify function RenderCache
 * @tc.type: FUNC
 * @tc.require: issueIAE59W
 */
HWTEST_F(RsSubThreadTest, RenderCache001, TestSize.Level1)
{
    auto renderContext = std::make_shared<RenderContext>();
    auto curThread = std::make_shared<RSSubThread>(renderContext.get(), 0);
    std::shared_ptr<RSSuperRenderTask> threadTask = nullptr;
    curThread->RenderCache(threadTask);
    EXPECT_TRUE(curThread->renderContext_);

    auto renderNode = std::make_shared<RSRenderNode>(0);
    threadTask = std::make_shared<RSSuperRenderTask>(renderNode);
    curThread->RenderCache(threadTask);
    EXPECT_FALSE(threadTask->GetTaskSize());

    RSSurfaceRenderNodeConfig config;
    config.id = 1;
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(config);
    RSRenderTask::RenderNodeStage stage;
    auto task = std::make_unique<RSRenderTask>(*node, stage);
    task->node_ = node;
    task->SetIdx(1);
    threadTask->AddTask(std::move(task));
    curThread->RenderCache(threadTask);
    EXPECT_TRUE(threadTask->GetTaskSize());
    EXPECT_FALSE(curThread->grContext_);

    curThread->grContext_ = std::make_shared<Drawing::GPUContext>();
    curThread->RenderCache(threadTask);
    EXPECT_TRUE(curThread->grContext_);
    threadTask->frameCount_ = 1;
    curThread->RenderCache(threadTask);
    EXPECT_TRUE(curThread->grContext_);

    auto renderTask = std::make_unique<RSRenderTask>(*node, stage);
    renderTask->node_ = nullptr;
    threadTask->AddTask(std::move(renderTask));
    curThread->RenderCache(threadTask);

    auto rsRenderTask = std::make_unique<RSRenderTask>(*node, stage);
    rsRenderTask->SetIdx(0);
    threadTask->AddTask(std::move(rsRenderTask));
    curThread->RenderCache(threadTask);
}

/**
 * @tc.name: DrawableCache001
 * @tc.desc: Verify function DrawableCache
 * @tc.type: FUNC
 * @tc.require: issueIAE59W
 */
HWTEST_F(RsSubThreadTest, DrawableCache001, TestSize.Level1)
{
    auto renderContext = std::make_shared<RenderContext>();
    auto curThread = std::make_shared<RSSubThread>(renderContext.get(), 0);
    auto node = std::make_shared<const RSSurfaceRenderNode>(0);
    std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable> nodeDrawable = nullptr;
    EXPECT_FALSE(curThread->grContext_);
    curThread->DrawableCache(nodeDrawable);

    nodeDrawable = std::make_shared<DrawableV2::RSSurfaceRenderNodeDrawable>(std::move(node));
    curThread->grContext_ = std::make_shared<Drawing::GPUContext>();
    curThread->DrawableCache(nodeDrawable);
    EXPECT_TRUE(curThread->grContext_);

    nodeDrawable->renderParams_ = std::make_unique<RSRenderParams>(0);
    curThread->DrawableCache(nodeDrawable);
    EXPECT_TRUE(nodeDrawable->GetRenderParams());

    nodeDrawable->SetTaskFrameCount(1);
    curThread->DrawableCache(nodeDrawable);
    EXPECT_TRUE(nodeDrawable->GetTaskFrameCount());
}

/**
 * @tc.name: CreateShareGrContext001
 * @tc.desc: Verify function CreateShareGrContext
 * @tc.type: FUNC
 * @tc.require: issueIAE59W
 */
HWTEST_F(RsSubThreadTest, CreateShareGrContext001, TestSize.Level1)
{
    auto renderContext = std::make_shared<RenderContext>();
    auto curThread = std::make_shared<RSSubThread>(renderContext.get(), 0);
    EXPECT_FALSE(curThread->CreateShareGrContext());
}

/**
 * @tc.name: ReleaseCacheSurfaceOnly001
 * @tc.desc: Verify function ReleaseCacheSurfaceOnly
 * @tc.type: FUNC
 * @tc.require: issueIAE59W
 */
HWTEST_F(RsSubThreadTest, ReleaseCacheSurfaceOnly001, TestSize.Level1)
{
    auto renderContext = std::make_shared<RenderContext>();
    auto curThread = std::make_shared<RSSubThread>(renderContext.get(), 0);
    auto node = std::make_shared<const RSSurfaceRenderNode>(0);
    std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable> nodeDrawable = nullptr;
    curThread->ReleaseCacheSurfaceOnly(nodeDrawable);

    nodeDrawable = std::make_shared<DrawableV2::RSSurfaceRenderNodeDrawable>(std::move(node));
    curThread->ReleaseCacheSurfaceOnly(nodeDrawable);
    EXPECT_FALSE(nodeDrawable->GetRenderParams());

    nodeDrawable->renderParams_ = std::make_unique<RSRenderParams>(0);
    curThread->ReleaseCacheSurfaceOnly(nodeDrawable);
    EXPECT_TRUE(nodeDrawable->GetRenderParams());
}
} // namespace OHOS::Rosen