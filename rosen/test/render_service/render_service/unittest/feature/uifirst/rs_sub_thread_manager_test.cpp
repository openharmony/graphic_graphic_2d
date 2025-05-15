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

#include "feature/uifirst/rs_sub_thread_manager.h"
#include "gtest/gtest.h"

#include "pipeline/main_thread/rs_main_thread.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RsSubThreadManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RsSubThreadManagerTest::SetUpTestCase() {}
void RsSubThreadManagerTest::TearDownTestCase() {}
void RsSubThreadManagerTest::SetUp() {}
void RsSubThreadManagerTest::TearDown() {}

/**
 * @tc.name: StartTest
 * @tc.desc: Test RsSubThreadManagerTest.StartTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RsSubThreadManagerTest, StartTest, TestSize.Level1)
{
    auto rsSubThreadManager = RSSubThreadManager::Instance();
    rsSubThreadManager->Start(nullptr);
    EXPECT_TRUE(rsSubThreadManager->threadList_.empty());

    auto renderContext = std::make_shared<RenderContext>();
    std::shared_ptr<RSSubThread> subThread = std::make_shared<RSSubThread>(renderContext.get(), 0);
    rsSubThreadManager->threadList_.push_back(subThread);
    rsSubThreadManager->Start(renderContext.get());
    EXPECT_FALSE(rsSubThreadManager->threadList_.empty());
    rsSubThreadManager->threadList_.clear();
}

/**
 * @tc.name: PostTaskTest
 * @tc.desc: Test RsSubThreadManagerTest.PostTaskTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RsSubThreadManagerTest, PostTaskTest, TestSize.Level1)
{
    auto rsSubThreadManager = RSSubThreadManager::Instance();
    rsSubThreadManager->PostTask([] {}, 5);
    auto renderContext = std::make_shared<RenderContext>();
    auto curThread = std::make_shared<RSSubThread>(renderContext.get(), 0);
    rsSubThreadManager->threadList_.push_back(curThread);
    rsSubThreadManager->PostTask([] {}, 1);
    rsSubThreadManager->PostTask([] {}, 1, true);
    rsSubThreadManager->threadList_.clear();
    EXPECT_TRUE(rsSubThreadManager->threadList_.empty());
}

/**
 * @tc.name: DumpMemTest001
 * @tc.desc: Verify function DumpMem
 * @tc.type:FUNC
 */
HWTEST_F(RsSubThreadManagerTest, DumpMemTest001, TestSize.Level1)
{
    auto rsSubThreadManager = RSSubThreadManager::Instance();
    DfxString log;
    auto renderContext = std::make_shared<RenderContext>();
    std::shared_ptr<RSSubThread> curThread = nullptr;
    auto curThreadf = std::make_shared<RSSubThread>(renderContext.get(), 0);
    rsSubThreadManager->DumpMem(log);
    rsSubThreadManager->threadList_.push_back(curThread);
    rsSubThreadManager->threadList_.push_back(curThreadf);
    rsSubThreadManager->DumpMem(log);
    EXPECT_FALSE(rsSubThreadManager->threadList_.empty());
    rsSubThreadManager->threadList_.clear();
}

/**
 * @tc.name: GetAppGpuMemoryInMBTest001
 * @tc.desc: Verify function GetAppGpuMemoryInMB
 * @tc.type:FUNC
 */
HWTEST_F(RsSubThreadManagerTest, GetAppGpuMemoryInMBTest001, TestSize.Level1)
{
    auto rsSubThreadManager = RSSubThreadManager::Instance();
    std::shared_ptr<RSSubThread> curThreadf = nullptr;
    auto renderContextt = std::make_shared<RenderContext>();
    auto curThread = std::make_shared<RSSubThread>(renderContextt.get(), 0);
    EXPECT_EQ(rsSubThreadManager->GetAppGpuMemoryInMB(), 0.f);
    rsSubThreadManager->threadList_.push_back(curThreadf);
    rsSubThreadManager->threadList_.push_back(curThread);
    EXPECT_EQ(rsSubThreadManager->GetAppGpuMemoryInMB(), 0.f);
    rsSubThreadManager->threadList_.clear();
}

/**
 * @tc.name: WaitNodeTaskTest001
 * @tc.desc: Verify function WaitNodeTask
 * @tc.type:FUNC
 */
HWTEST_F(RsSubThreadManagerTest, WaitNodeTaskTest001, TestSize.Level1)
{
    auto rsSubThreadManager = RSSubThreadManager::Instance();
    rsSubThreadManager->WaitNodeTask(1);
    EXPECT_TRUE(rsSubThreadManager->threadList_.empty());
}

/**
 * @tc.name: NodeTaskNotifyTest001
 * @tc.desc: Verify function NodeTaskNotify
 * @tc.type:FUNC
 */
HWTEST_F(RsSubThreadManagerTest, NodeTaskNotifyTest001, TestSize.Level1)
{
    auto rsSubThreadManager = RSSubThreadManager::Instance();
    rsSubThreadManager->NodeTaskNotify(1);
    EXPECT_FALSE(rsSubThreadManager->nodeTaskState_.empty());
}

/**
 * @tc.name: ResetSubThreadGrContextTest001
 * @tc.desc: Verify function ResetSubThreadGrContext
 * @tc.type:FUNC
 */
HWTEST_F(RsSubThreadManagerTest, ResetSubThreadGrContextTest001, TestSize.Level1)
{
    auto rsSubThreadManager = RSSubThreadManager::Instance();
    rsSubThreadManager->threadList_.clear();
    auto renderContextf = std::make_shared<RenderContext>();
    auto curThreadf = std::make_shared<RSSubThread>(renderContextf.get(), 0);
    auto renderContexts = std::make_shared<RenderContext>();
    auto curThreads = std::make_shared<RSSubThread>(renderContexts.get(), 1);
    auto renderContextt = std::make_shared<RenderContext>();
    auto curThreadt = std::make_shared<RSSubThread>(renderContextt.get(), 2);
    rsSubThreadManager->threadList_.clear();
    rsSubThreadManager->ResetSubThreadGrContext();
    EXPECT_TRUE(rsSubThreadManager->threadList_.empty());

    rsSubThreadManager->threadList_.push_back(curThreadf);
    rsSubThreadManager->threadList_.push_back(curThreads);
    rsSubThreadManager->threadList_.push_back(curThreadt);
    rsSubThreadManager->ResetSubThreadGrContext();
    EXPECT_FALSE(rsSubThreadManager->nodeTaskState_.empty());

    rsSubThreadManager->needResetContext_ = true;
    rsSubThreadManager->ResetSubThreadGrContext();
    EXPECT_FALSE(rsSubThreadManager->needResetContext_);
    rsSubThreadManager->threadList_.clear();
}

/**
 * @tc.name: CancelReleaseResourceTaskTest001
 * @tc.desc: Verify function CancelReleaseResourceTask
 * @tc.type:FUNC
 */
HWTEST_F(RsSubThreadManagerTest, CancelReleaseResourceTaskTest001, TestSize.Level1)
{
    auto rsSubThreadManager = RSSubThreadManager::Instance();
    EXPECT_TRUE(rsSubThreadManager->needCancelTask_);
    auto renderContextf = std::make_shared<RenderContext>();
    auto curThreadf = std::make_shared<RSSubThread>(renderContextf.get(), 0);
    auto renderContexts = std::make_shared<RenderContext>();
    auto curThreads = std::make_shared<RSSubThread>(renderContexts.get(), 1);
    auto renderContextt = std::make_shared<RenderContext>();
    auto curThreadt = std::make_shared<RSSubThread>(renderContextt.get(), 2);
    rsSubThreadManager->CancelReleaseResourceTask();
    EXPECT_TRUE(rsSubThreadManager->threadList_.empty());

    rsSubThreadManager->threadList_.push_back(curThreadf);
    rsSubThreadManager->threadList_.push_back(curThreads);
    rsSubThreadManager->threadList_.push_back(curThreadt);
    rsSubThreadManager->CancelReleaseResourceTask();
    EXPECT_FALSE(rsSubThreadManager->threadList_.empty());

    rsSubThreadManager->needCancelTask_ = false;
    rsSubThreadManager->CancelReleaseResourceTask();
    EXPECT_FALSE(rsSubThreadManager->threadList_.empty());
    rsSubThreadManager->threadList_.clear();
}

/**
 * @tc.name: ReleaseTextureTest001
 * @tc.desc: Verify function ReleaseTexture
 * @tc.type:FUNC
 */
HWTEST_F(RsSubThreadManagerTest, ReleaseTextureTest001, TestSize.Level1)
{
    auto rsSubThreadManager = RSSubThreadManager::Instance();
    auto renderContextf = std::make_shared<RenderContext>();
    auto curThreadf = std::make_shared<RSSubThread>(renderContextf.get(), 0);
    auto renderContexts = std::make_shared<RenderContext>();
    auto curThreads = std::make_shared<RSSubThread>(renderContexts.get(), 1);
    auto renderContextt = std::make_shared<RenderContext>();
    auto curThreadt = std::make_shared<RSSubThread>(renderContextt.get(), 2);
    rsSubThreadManager->ReleaseTexture();
    EXPECT_TRUE(rsSubThreadManager->threadList_.empty());

    rsSubThreadManager->threadList_.push_back(curThreadf);
    rsSubThreadManager->threadList_.push_back(curThreads);
    rsSubThreadManager->threadList_.push_back(curThreadt);
    rsSubThreadManager->ReleaseTexture();
    EXPECT_FALSE(rsSubThreadManager->threadList_.empty());
    rsSubThreadManager->threadList_.clear();
}

/**
 * @tc.name: CancelReleaseTextureTaskTest001
 * @tc.desc: Verify function CancelReleaseTextureTask
 * @tc.type:FUNC
 */
HWTEST_F(RsSubThreadManagerTest, CancelReleaseTextureTaskTest001, TestSize.Level1)
{
    auto rsSubThreadManager = RSSubThreadManager::Instance();
    auto renderContextf = std::make_shared<RenderContext>();
    auto curThreadf = std::make_shared<RSSubThread>(renderContextf.get(), 0);
    auto renderContexts = std::make_shared<RenderContext>();
    auto curThreads = std::make_shared<RSSubThread>(renderContexts.get(), 1);
    auto renderContextt = std::make_shared<RenderContext>();
    auto curThreadt = std::make_shared<RSSubThread>(renderContextt.get(), 2);
    rsSubThreadManager->CancelReleaseTextureTask();
    EXPECT_TRUE(rsSubThreadManager->threadList_.empty());
    EXPECT_TRUE(rsSubThreadManager->needCancelReleaseTextureTask_);

    rsSubThreadManager->threadList_.push_back(curThreadf);
    rsSubThreadManager->threadList_.push_back(curThreads);
    rsSubThreadManager->threadList_.push_back(curThreadt);
    rsSubThreadManager->CancelReleaseTextureTask();
    EXPECT_FALSE(rsSubThreadManager->threadList_.empty());
    EXPECT_FALSE(rsSubThreadManager->needCancelReleaseTextureTask_);

    rsSubThreadManager->CancelReleaseTextureTask();
    EXPECT_FALSE(rsSubThreadManager->threadList_.empty());
    rsSubThreadManager->threadList_.clear();
}

/**
 * @tc.name: ReleaseSurfaceTest001
 * @tc.desc: Verify function ReleaseSurface
 * @tc.type:FUNC
 */
HWTEST_F(RsSubThreadManagerTest, ReleaseSurfaceTest001, TestSize.Level1)
{
    auto rsSubThreadManager = RSSubThreadManager::Instance();
    auto renderContext = std::make_shared<RenderContext>();
    auto curThread = std::make_shared<RSSubThread>(renderContext.get(), 0);
    rsSubThreadManager->threadList_.push_back(curThread);
    rsSubThreadManager->ReleaseSurface(2);
    rsSubThreadManager->ReleaseSurface(0);
    EXPECT_FALSE(rsSubThreadManager->threadList_.empty());
    rsSubThreadManager->threadList_.clear();
}

/**
 * @tc.name: AddToReleaseQueue
 * @tc.desc: Test RsSubThreadManagerTest.AddToReleaseQueue
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RsSubThreadManagerTest, AddToReleaseQueue, TestSize.Level1)
{
    const Drawing::ImageInfo info =
        Drawing::ImageInfo { 200, 200, Drawing::COLORTYPE_N32, Drawing::ALPHATYPE_OPAQUE }; // image size 200*200
    auto surface(Drawing::Surface::MakeRaster(info));
    auto rsSubThreadManager = RSSubThreadManager::Instance();
    ASSERT_NE(rsSubThreadManager, nullptr);
    rsSubThreadManager->AddToReleaseQueue(std::move(surface), 10); // 10 is invalid
}

/**
 * @tc.name: CountSubMemTest001
 * @tc.desc: Verify function CountSubMem
 * @tc.type:FUNC
 */
HWTEST_F(RsSubThreadManagerTest, CountSubMemTest001, TestSize.Level1)
{
    auto rsSubThreadManager = RSSubThreadManager::Instance();
    EXPECT_TRUE(rsSubThreadManager->CountSubMem(0).empty());
    auto renderContext = std::make_shared<RenderContext>();
    std::shared_ptr<RSSubThread> curThread = nullptr;
    auto curThreads = std::make_shared<RSSubThread>(renderContext.get(), 0);
    rsSubThreadManager->threadList_.push_back(curThread);
    rsSubThreadManager->threadList_.push_back(curThreads);
    EXPECT_FALSE(rsSubThreadManager->CountSubMem(0).empty());
    rsSubThreadManager->threadList_.clear();
}

/**
 * @tc.name: GetReThreadIndexMapTest001
 * @tc.desc: Verify function GetReThreadIndexMap
 * @tc.type:FUNC
 */
HWTEST_F(RsSubThreadManagerTest, GetReThreadIndexMapTest001, TestSize.Level1)
{
    auto rsSubThreadManager = RSSubThreadManager::Instance();
    EXPECT_TRUE(rsSubThreadManager->GetReThreadIndexMap().empty());
}

/**
 * @tc.name: ScheduleRenderNodeDrawableTest
 * @tc.desc: Test RsSubThreadManagerTest.ScheduleRenderNodeDrawableTest
 * @tc.type: FUNC
 * @tc.require: issueIAE59W
 */
HWTEST_F(RsSubThreadManagerTest, ScheduleRenderNodeDrawableTest, TestSize.Level1)
{
    auto rsSubThreadManager = RSSubThreadManager::Instance();
    std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable> drawable = nullptr;
    rsSubThreadManager->ScheduleRenderNodeDrawable(drawable);
    EXPECT_FALSE(drawable);
}

/**
 * @tc.name: ScheduleReleaseCacheSurfaceOnlyTest
 * @tc.desc: Test RsSubThreadManagerTest.ScheduleReleaseCacheSurfaceOnlyTest
 * @tc.type: FUNC
 * @tc.require: issueIAE59W
 */
HWTEST_F(RsSubThreadManagerTest, ScheduleReleaseCacheSurfaceOnlyTest, TestSize.Level1)
{
    auto rsSubThreadManager = RSSubThreadManager::Instance();
    std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable> drawable = nullptr;
    rsSubThreadManager->ScheduleReleaseCacheSurfaceOnly(drawable);
    EXPECT_FALSE(drawable);
}

/**
 * @tc.name: GetGpuMemoryForReportTest
 * @tc.desc: Test RsSubThreadManagerTest.GetGpuMemoryForReportTest
 * @tc.type: FUNC
 * @tc.require: issueIAE59W
 */
HWTEST_F(RsSubThreadManagerTest, GetGpuMemoryForReportTest, TestSize.Level1)
{
    auto rsSubThreadManager = RSSubThreadManager::Instance();
    pid_t pid = 123;
    size_t size = 2048;
    auto renderContext = std::make_shared<RenderContext>();
    auto thread = std::make_shared<RSSubThread>(renderContext.get(), 0);
    thread->gpuMemoryOfPid_.insert(std::make_pair(pid, size));
    rsSubThreadManager->threadList_.push_back(thread);
    std::unordered_map<pid_t, size_t> totalGpuMemOfPid;
    rsSubThreadManager->GetGpuMemoryForReport(totalGpuMemOfPid);
    EXPECT_TRUE(totalGpuMemOfPid[pid] == size);
}
} // namespace OHOS::Rosen
