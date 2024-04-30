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

#include "pipeline/parallel_render/rs_sub_thread_manager.h"
#include "pipeline/rs_main_thread.h"

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
}

/**
 * @tc.name: StartRCDThreadTest001
 * @tc.desc: Verify function StartRCDThread
 * @tc.type:FUNC
 */
HWTEST_F(RsSubThreadManagerTest, StartRCDThreadTest001, TestSize.Level1)
{
    auto rsSubThreadManager = RSSubThreadManager::Instance();
    auto renderContext = std::make_shared<RenderContext>();
    rsSubThreadManager->StartRCDThread(renderContext.get());
    EXPECT_TRUE(rsSubThreadManager->threadList_.empty());
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
    rsSubThreadManager->threadList_.clear();
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
 * @tc.name: SubmitFilterSubThreadTaskTest001
 * @tc.desc: Verify function SubmitFilterSubThreadTask
 * @tc.type:FUNC
 */
HWTEST_F(RsSubThreadManagerTest, SubmitFilterSubThreadTaskTest001, TestSize.Level1)
{
    auto rsSubThreadManager = RSSubThreadManager::Instance();
    rsSubThreadManager->SubmitFilterSubThreadTask();
    EXPECT_TRUE(rsSubThreadManager->threadList_.empty());
}

/**
 * @tc.name: SubmitSubThreadTaskTest
 * @tc.desc: Test RsSubThreadManagerTest.SubmitSubThreadTaskTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RsSubThreadManagerTest, SubmitSubThreadTaskTest, TestSize.Level1)
{
    std::list<std::shared_ptr<RSSurfaceRenderNode>> list;
    auto rsSubThreadManager = RSSubThreadManager::Instance();
    rsSubThreadManager->SubmitSubThreadTask(nullptr, list);
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(1, config);
    rsSubThreadManager->SubmitSubThreadTask(node, list);
    EXPECT_TRUE(list.empty());
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
    rsSubThreadManager->needResetContext_ = true;
    rsSubThreadManager->ResetSubThreadGrContext();
    rsSubThreadManager->threadList_.push_back(curThreadf);
    rsSubThreadManager->threadList_.push_back(curThreads);
    rsSubThreadManager->threadList_.push_back(curThreadt);
    rsSubThreadManager->ResetSubThreadGrContext();
}

/**
 * @tc.name: CancelReleaseResourceTaskTest001
 * @tc.desc: Verify function CancelReleaseResourceTask
 * @tc.type:FUNC
 */
HWTEST_F(RsSubThreadManagerTest, CancelReleaseResourceTaskTest001, TestSize.Level1)
{
    auto rsSubThreadManager = RSSubThreadManager::Instance();
    auto renderContext = std::make_shared<RenderContext>();
    auto curThread = std::make_shared<RSSubThread>(renderContext.get(), 0);
    rsSubThreadManager->CancelReleaseResourceTask();
    rsSubThreadManager->needCancelTask_ = true;
    rsSubThreadManager->CancelReleaseResourceTask();
    rsSubThreadManager->CancelReleaseResourceTask();
    EXPECT_FALSE(rsSubThreadManager->threadList_.empty());
}

/**
 * @tc.name: ReleaseTextureTest001
 * @tc.desc: Verify function ReleaseTexture
 * @tc.type:FUNC
 */
HWTEST_F(RsSubThreadManagerTest, ReleaseTextureTest001, TestSize.Level1)
{
    auto rsSubThreadManager = RSSubThreadManager::Instance();
    auto renderContext = std::make_shared<RenderContext>();
    auto curThread = std::make_shared<RSSubThread>(renderContext.get(), 0);
    rsSubThreadManager->ReleaseTexture();
    rsSubThreadManager->ReleaseTexture();
    EXPECT_FALSE(rsSubThreadManager->threadList_.empty());
}

/**
 * @tc.name: CancelReleaseTextureTaskTest001
 * @tc.desc: Verify function CancelReleaseTextureTask
 * @tc.type:FUNC
 */
HWTEST_F(RsSubThreadManagerTest, CancelReleaseTextureTaskTest001, TestSize.Level1)
{
    auto rsSubThreadManager = RSSubThreadManager::Instance();
    auto renderContext = std::make_shared<RenderContext>();
    auto curThread = std::make_shared<RSSubThread>(renderContext.get(), 0);
    rsSubThreadManager->CancelReleaseTextureTask();
    rsSubThreadManager->needCancelReleaseTextureTask_ = true;
    rsSubThreadManager->CancelReleaseTextureTask();
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

} // namespace OHOS::Rosen