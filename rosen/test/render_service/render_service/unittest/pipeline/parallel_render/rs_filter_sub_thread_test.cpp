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

#include "pipeline/parallel_render/rs_filter_sub_thread.h"
#include "render/rs_filter.h"
#include "pipeline/rs_main_thread.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RsFilterSubThreadTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RsFilterSubThreadTest::SetUpTestCase() {}
void RsFilterSubThreadTest::TearDownTestCase() {}
void RsFilterSubThreadTest::SetUp() {}
void RsFilterSubThreadTest::TearDown() {}

/**
 * @tc.name: PostTaskTest
 * @tc.desc: Test RsFilterSubThreadTest.PostTaskTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RsFilterSubThreadTest, PostTaskTest, TestSize.Level1)
{
    auto renderContext = std::make_shared<RenderContext>();
    auto curThread = std::make_shared<RSFilterSubThread>(renderContext.get());
    curThread->PostTask([] {});
    curThread->Start();
    curThread->PostTask([] {});
}

/**
 * @tc.name: CreateShareEglContextTest
 * @tc.desc: Test RsFilterSubThreadTest.CreateShareEglContextTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RsFilterSubThreadTest, CreateShareEglContextTest, TestSize.Level1)
{
    auto curThread1 = std::make_shared<RSFilterSubThread>(nullptr);
    curThread1->CreateShareEglContext();
    curThread1->DestroyShareEglContext();
    auto renderContext = std::make_shared<RenderContext>();
    auto curThread2 = std::make_shared<RSFilterSubThread>(renderContext.get());
    curThread2->CreateShareEglContext();
    curThread2->DestroyShareEglContext();
}

/**
 * @tc.name: StartColorPickerTest001
 * @tc.desc: Verify function StartColorPicker
 * @tc.type:FUNC
 */
HWTEST_F(RsFilterSubThreadTest, StartColorPickerTest001, TestSize.Level1)
{
    auto renderContext = std::make_shared<RenderContext>();
    auto curThread = std::make_shared<RSFilterSubThread>(renderContext.get());
    curThread->StartColorPicker();
    RSMainThread::Instance()->SetNoNeedToPostTask(true);
    curThread->StartColorPicker();
    EXPECT_TRUE(curThread->runner_);
}

/**
 * @tc.name: PostSyncTaskTest001
 * @tc.desc: Verify function PostSyncTask
 * @tc.type:FUNC
 */
HWTEST_F(RsFilterSubThreadTest, PostSyncTaskTest001, TestSize.Level1)
{
    auto renderContext = std::make_shared<RenderContext>();
    auto curThread = std::make_shared<RSFilterSubThread>(renderContext.get());
    curThread->PostSyncTask([] {});
    curThread->Start();
    curThread->PostSyncTask([] {});
    EXPECT_TRUE(curThread->handler_);
}

/**
 * @tc.name: DumpMemTest001
 * @tc.desc: Verify function DumpMem
 * @tc.type:FUNC
 */
HWTEST_F(RsFilterSubThreadTest, DumpMemTest001, TestSize.Level1)
{
    auto renderContext = std::make_shared<RenderContext>();
    auto curThread = std::make_shared<RSFilterSubThread>(renderContext.get());
    DfxString log;
    curThread->grContext_ = std::make_shared<Drawing::GPUContext>();
    curThread->DumpMem(log);
    EXPECT_TRUE(curThread->grContext_);
}

/**
 * @tc.name: GetAppGpuMemoryInMBTest001
 * @tc.desc: Verify function GetAppGpuMemoryInMB
 * @tc.type:FUNC
 */
HWTEST_F(RsFilterSubThreadTest, GetAppGpuMemoryInMBTest001, TestSize.Level1)
{
    auto renderContext = std::make_shared<RenderContext>();
    auto curThread = std::make_shared<RSFilterSubThread>(renderContext.get());
    curThread->grContext_ = std::make_shared<Drawing::GPUContext>();
    EXPECT_EQ(curThread->GetAppGpuMemoryInMB(), 0.f);
}

/**
 * @tc.name: RenderCacheTest001
 * @tc.desc: Verify function RenderCache
 * @tc.type:FUNC
 */
HWTEST_F(RsFilterSubThreadTest, RenderCacheTest001, TestSize.Level1)
{
    class RSFilterCacheTask : public RSFilter::RSFilterTask {
    public:
        bool initSurfaceSuccess = false;
        virtual ~RSFilterCacheTask() {}
        bool InitSurface(Drawing::GPUContext* grContext) override
        {
            bool success = initSurfaceSuccess;
            initSurfaceSuccess = true;
            return success;
        }
        bool Render() override
        {
            initSurfaceSuccess = false;
            return false;
        }
        bool SaveFilteredImage() override
        {
            bool success = initSurfaceSuccess;
            initSurfaceSuccess = true;
            return success;
        }
        bool SetDone() override
        {
            return false;
        }
        void SwapInit() {};
    };
    auto renderContext = std::make_shared<RenderContext>();
    auto curThread = std::make_shared<RSFilterSubThread>(renderContext.get());
    std::vector<std::weak_ptr<RSFilter::RSFilterTask>> filterTaskList;
    std::shared_ptr<RSFilterCacheTask> weakTask1 = nullptr;
    std::weak_ptr<RSFilterCacheTask> weakTask2 = std::make_shared<RSFilterCacheTask>();
    std::weak_ptr<RSFilterCacheTask> weakTask3 = std::make_shared<RSFilterCacheTask>();
    curThread->grContext_ = std::make_shared<Drawing::GPUContext>();
    sptr<SyncFence> fence(new SyncFence(-1));
    curThread->SetFence(fence);
    filterTaskList.push_back(weakTask1);
    filterTaskList.push_back(weakTask2);
    filterTaskList.push_back(weakTask3);
    curThread->RenderCache(filterTaskList);
    sptr<SyncFence> fencee(new SyncFence(0));
    curThread->SetFence(fencee);
    curThread->RenderCache(filterTaskList);
    EXPECT_TRUE(filterTaskList.empty());
}

/**
 * @tc.name: FlushAndSubmitTest001
 * @tc.desc: Verify function FlushAndSubmit
 * @tc.type:FUNC
 */
HWTEST_F(RsFilterSubThreadTest, FlushAndSubmitTest001, TestSize.Level1)
{
    class RSFilterCacheTask : public RSFilter::RSFilterTask {
    public:
        bool initSurfaceSuccess = false;
        virtual ~RSFilterCacheTask() {}
        bool InitSurface(Drawing::GPUContext* grContext) override
        {
            bool success = initSurfaceSuccess;
            initSurfaceSuccess = true;
            return success;
        }
        bool Render() override
        {
            initSurfaceSuccess = false;
            return false;
        }
        bool SaveFilteredImage() override
        {
            bool success = initSurfaceSuccess;
            initSurfaceSuccess = true;
            return success;
        }
        bool SetDone() override
        {
            return false;
        }
        void SwapInit() {};
    };
    auto renderContext = std::make_shared<RenderContext>();
    auto curThread = std::make_shared<RSFilterSubThread>(renderContext.get());
    curThread->FlushAndSubmit();
    std::shared_ptr<RSFilterCacheTask> weakTask1 = nullptr;
    std::weak_ptr<RSFilterCacheTask> weakTask2 = std::make_shared<RSFilterCacheTask>();
    curThread->filterTaskList_.push_back(weakTask1);
    curThread->filterTaskList_.push_back(weakTask2);
    curThread->FlushAndSubmit();
    EXPECT_FALSE(curThread->grContext_);
}

/**
 * @tc.name: ColorPickerRenderCacheTest001
 * @tc.desc: Verify function ColorPickerRenderCache
 * @tc.type:FUNC
 */
HWTEST_F(RsFilterSubThreadTest, ColorPickerRenderCacheTest001, TestSize.Level1)
{
    auto renderContext = std::make_shared<RenderContext>();
    auto curThread = std::make_shared<RSFilterSubThread>(renderContext.get());
    std::shared_ptr<RSColorPickerCacheTask> colorTask = nullptr;
    curThread->ColorPickerRenderCache(colorTask);
    EXPECT_FALSE(curThread->grContext_);
}

/**
 * @tc.name: ResetGrContextTest001
 * @tc.desc: Verify function ResetGrContext
 * @tc.type:FUNC
 */
HWTEST_F(RsFilterSubThreadTest, ResetGrContextTest001, TestSize.Level1)
{
    auto renderContext = std::make_shared<RenderContext>();
    auto curThread = std::make_shared<RSFilterSubThread>(renderContext.get());
    curThread->ResetGrContext();
    curThread->grContext_ = std::make_shared<Drawing::GPUContext>();
    curThread->ResetGrContext();
    EXPECT_TRUE(curThread->grContext_);
}
} // namespace OHOS::Rosen