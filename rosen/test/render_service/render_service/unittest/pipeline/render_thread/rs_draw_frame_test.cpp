/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include <thread>
#include "drawable/rs_canvas_drawing_render_node_drawable.h"
#include "foundation/graphic/graphic_2d/rosen/test/render_service/render_service/unittest/pipeline/rs_test_util.h"
#include "pipeline/render_thread/rs_draw_frame.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "recording/recording_canvas.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace OHOS::Rosen {
class RSDrawFrameTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSDrawFrameTest::SetUpTestCase()
{
    RSTestUtil::InitRenderNodeGC();
}

void RSDrawFrameTest::TearDownTestCase() {}
void RSDrawFrameTest::SetUp() {}
void RSDrawFrameTest::TearDown() {}

/**
 * @tc.name: PostDirectCompositionJankStatsTest
 * @tc.desc: test PostDirectCompositionJankStats
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST_F(RSDrawFrameTest, PostDirectCompositionJankStatsTest, TestSize.Level1)
{
    RSDrawFrame drawFrame_;
    JankDurationParams rsParams;
    bool optimizeLoadTrue = true;
    drawFrame_.PostDirectCompositionJankStats(rsParams, optimizeLoadTrue);
    EXPECT_TRUE(optimizeLoadTrue);
    bool optimizeLoadFalse = false;
    drawFrame_.PostDirectCompositionJankStats(rsParams, optimizeLoadFalse);
    EXPECT_FALSE(optimizeLoadFalse);
}

/**
 * @tc.name: NotifyClearGpuCacheTest
 * @tc.desc: test NotifyClearGpuCache
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST_F(RSDrawFrameTest, NotifyClearGpuCacheTest, TestSize.Level1)
{
    RSDrawFrame drawFrame_;
    drawFrame_.NotifyClearGpuCache();
    ASSERT_FALSE(RSFilterCacheManager::filterInvalid_);
    RSFilterCacheManager::filterInvalid_ = true;
    drawFrame_.NotifyClearGpuCache();
    ASSERT_TRUE(RSFilterCacheManager::filterInvalid_);
}

/**
 * @tc.name: CheckCanvasSkipSyncTest
 * @tc.desc: test CheckCanvasSkipSync
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST_F(RSDrawFrameTest, CheckCanvasSkipSyncTest, TestSize.Level1)
{
    RSDrawFrame drawFrame_;
    NodeId nodeId = 100;
    auto surfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(nodeId);
    ASSERT_TRUE(drawFrame_.CheckCanvasSkipSync(surfaceRenderNode));
    auto canvasRenderNode = std::make_shared<RSCanvasDrawingRenderNode>(++nodeId);
    ASSERT_TRUE(drawFrame_.CheckCanvasSkipSync(canvasRenderNode));
    auto canvasNodeDrawable = std::static_pointer_cast<RSCanvasDrawingRenderNodeDrawable>(
        RSRenderNodeDrawableAdapter::OnGenerate(canvasRenderNode));
    canvasNodeDrawable->needDraw_ = true;
    ASSERT_FALSE(drawFrame_.CheckCanvasSkipSync(canvasRenderNode));
    ASSERT_EQ(drawFrame_.stagingSyncCanvasDrawingNodes_.size(), 1);
}

/**
 * @tc.name: SyncTest
 * @tc.desc: test Sync
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST_F(RSDrawFrameTest, SyncTest, TestSize.Level1)
{
    RSDrawFrame drawFrame_;
    auto node = std::make_shared<RSCanvasDrawingRenderNode>(200);
    drawFrame_.stagingSyncCanvasDrawingNodes_.emplace(node->GetId(), node);
    drawFrame_.Sync();
    ASSERT_EQ(drawFrame_.stagingSyncCanvasDrawingNodes_.size(), 0);
    ASSERT_EQ(RSMainThread::Instance()->GetContext().pendingSyncNodes_.size(), 0);
}

/**
 * @tc.name: SyncTest002
 * @tc.desc: test Sync
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST_F(RSDrawFrameTest, SyncTest002, TestSize.Level1)
{
    RSDrawFrame drawFrame_;
    drawFrame_.Sync();

    auto param = std::unique_ptr<RSRenderThreadParams>();
    param.reset(new RSRenderThreadParams());
    ASSERT_NE(param, nullptr);
    drawFrame_.SetRenderThreadParams(param);
    drawFrame_.Sync();

    auto param2 = std::unique_ptr<RSRenderThreadParams>();
    param2.reset(new RSRenderThreadParams());
    ASSERT_NE(param2, nullptr);
    param2->hasDisplayHdrOn_ = true;
    drawFrame_.SetRenderThreadParams(param2);
    drawFrame_.Sync();
}

/**
 * @tc.name: UnlockMainThreadTest
 * @tc.desc: test UnlockMainThread
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST_F(RSDrawFrameTest, UnlockMainThreadTest, TestSize.Level1)
{
    RSDrawFrame drawFrame_;
    drawFrame_.UnblockMainThread();
    ASSERT_TRUE(drawFrame_.canUnblockMainThread);
}

/**
 * @tc.name: EndCheck
 * @tc.desc: test EndCheck
 * @tc.type: FUNC
 * @tc.require: issueIC0B60
 */
HWTEST_F(RSDrawFrameTest, EndCheckTest, TestSize.Level1)
{
    RSDrawFrame drawFrame_;
    drawFrame_.exceptionCheck_.isUpload_ = false; // This value cannot be modified.
    for (int i = 0; i < 6; i++) { // 6: Simulate six timeouts
        drawFrame_.timer_ = std::make_shared<RSTimer>("RenderFrame", 2500); // 2500ms
        usleep(2500 * 1000); // 2500ms
        drawFrame_.EndCheck();
    }
    ASSERT_EQ(drawFrame_.longFrameCount_, 6);
}

/**
 * @tc.name: ClearDrawableResource
 * @tc.desc: test ClearDrawableResource
 * @tc.type: FUNC
 */
HWTEST_F(RSDrawFrameTest, ClearDrawableResourceTest, TestSize.Level1)
{
    RSDrawFrame drawFrame;
    drawFrame.rsParallelType_ = RsParallelType::RS_PARALLEL_TYPE_SYNC;
    drawFrame.ClearDrawableResource();
    ASSERT_TRUE(DrawableV2::RSRenderNodeDrawableAdapter::toClearCmdListVec_.empty());
    drawFrame.rsParallelType_ = RsParallelType::RS_PARALLEL_TYPE_SINGLE_THREAD;
    drawFrame.ClearDrawableResource();
    ASSERT_TRUE(DrawableV2::RSRenderNodeDrawableAdapter::toClearCmdListVec_.empty());
    drawFrame.rsParallelType_ = RsParallelType::RS_PARALLEL_TYPE_ASYNC;
    drawFrame.ClearDrawableResource();
    ASSERT_TRUE(DrawableV2::RSRenderNodeDrawableAdapter::toClearCmdListVec_.empty());
    drawFrame.rsParallelType_ = static_cast<RsParallelType>(100);
    drawFrame.ClearDrawableResource();
    ASSERT_TRUE(DrawableV2::RSRenderNodeDrawableAdapter::toClearCmdListVec_.empty());
}

/**
 * @tc.name: ClearDrawableResource_WithResourcesToClear
 * @tc.desc: Test ClearDrawableResource when NeedClearResource returns true
 * @tc.type: FUNC
 */
HWTEST_F(RSDrawFrameTest, ClearDrawableResource_WithResourcesToClear, TestSize.Level1)
{
    // Set needClearResource_=true via AddToClearCmdList to reach the else branch
    DrawableV2::RSRenderNodeDrawableAdapter::CmdListVec cmdListVec;
    auto cmdList = std::make_shared<RSSimpleDrawCmdList>(1, 1);
    cmdListVec.emplace_back(cmdList);
    DrawableV2::RSRenderNodeDrawableAdapter::AddToClearCmdList(cmdListVec);
    ASSERT_TRUE(DrawableV2::RSRenderNodeDrawableAdapter::NeedClearResource());

    // SINGLE_THREAD calls ClearResource() directly (no PostTask dependency)
    RSDrawFrame drawFrame;
    drawFrame.rsParallelType_ = RsParallelType::RS_PARALLEL_TYPE_SINGLE_THREAD;
    drawFrame.ClearDrawableResource();
    EXPECT_FALSE(DrawableV2::RSRenderNodeDrawableAdapter::NeedClearResource());
    EXPECT_TRUE(DrawableV2::RSRenderNodeDrawableAdapter::toClearDrawableVec_.empty());
    EXPECT_TRUE(DrawableV2::RSRenderNodeDrawableAdapter::toClearCmdListVec_.empty());
}

/**
 * @tc.name: ClearDrawableMemory
 * @tc.desc: test ClearDrawableMemory
 * @tc.type: FUNC
 */
HWTEST_F(RSDrawFrameTest, ClearDrawableMemoryTest, TestSize.Level1)
{
    RSDrawFrame drawFrame;
    drawFrame.rsParallelType_ = RsParallelType::RS_PARALLEL_TYPE_SYNC;
    drawFrame.ClearDrawableMemory(false);
    drawFrame.ClearDrawableMemory(true);
    ASSERT_TRUE(drawFrame.rsParallelType_ == RsParallelType::RS_PARALLEL_TYPE_SYNC);
    drawFrame.rsParallelType_ = RsParallelType::RS_PARALLEL_TYPE_SINGLE_THREAD;
    drawFrame.ClearDrawableMemory(false);
    drawFrame.ClearDrawableMemory(true);
    ASSERT_TRUE(drawFrame.rsParallelType_ == RsParallelType::RS_PARALLEL_TYPE_SINGLE_THREAD);
    drawFrame.rsParallelType_ = RsParallelType::RS_PARALLEL_TYPE_ASYNC;
    drawFrame.ClearDrawableMemory(false);
    drawFrame.ClearDrawableMemory(true);
    ASSERT_TRUE(drawFrame.rsParallelType_ == RsParallelType::RS_PARALLEL_TYPE_ASYNC);
    drawFrame.rsParallelType_ = static_cast<RsParallelType>(100);
    drawFrame.ClearDrawableMemory(false);
    drawFrame.ClearDrawableMemory(true);
    ASSERT_TRUE(drawFrame.rsParallelType_ != RsParallelType::RS_PARALLEL_TYPE_ASYNC);
}

/**
 * @tc.name: StartEndCheckShortPathTest
 * @tc.desc: StartCheck then EndCheck quickly should not increase longFrameCount_
 * @tc.type: FUNC
 */
HWTEST_F(RSDrawFrameTest, StartEndCheckShortPathTest, TestSize.Level1)
{
    RSDrawFrame drawFrame;
    drawFrame.StartCheck();
    drawFrame.EndCheck();
    // When EndCheck runs quickly, longFrameCount_ should reset to 0
    ASSERT_EQ(drawFrame.longFrameCount_, 0);
}

/**
 * @tc.name: SetEarlyZEnabled_NullContext
 * @tc.desc: SetEarlyZEnabled should early return when GPUContext is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSDrawFrameTest, SetEarlyZEnabled_NullContext, TestSize.Level1)
{
    RSDrawFrame drawFrame;
    // Private method exposed in test due to access relaxation
    drawFrame.SetEarlyZEnabled(nullptr);
    // early return should not mutate internal counters
    ASSERT_EQ(drawFrame.longFrameCount_, 0);
}

/**
 * @tc.name: PostAndWait_AsyncBlockAndReleaseTest
 * @tc.desc: PostAndWait ASYNC path blocks on frameCV_ and UnblockMainThread releases it
 * @tc.type: FUNC
 * @tc.require: issue23488
 */
HWTEST_F(RSDrawFrameTest, PostAndWait_AsyncBlockAndReleaseTest, TestSize.Level1)
{
    RSDrawFrame drawFrame;
    drawFrame.rsParallelType_ = RsParallelType::RS_PARALLEL_TYPE_ASYNC;
    ASSERT_FALSE(drawFrame.canUnblockMainThread);
    // PostAndWait blocks on frameCV_.wait() in ASYNC mode; UnblockMainThread releases it
    std::thread helper([&drawFrame]() {
        usleep(50 * 1000); // 50ms: ensure PostAndWait enters wait before UnblockMainThread
        drawFrame.UnblockMainThread();
    });
    drawFrame.PostAndWait();
    helper.join();
    ASSERT_TRUE(drawFrame.canUnblockMainThread);
}

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
/**
 * @tc.name: TimeoutRender_StateTransitionTest
 * @tc.desc: TimeoutRender all branches: Update(timeout/normal), IsStatFinished, Reset, and EndCheck integration
 * @tc.type: FUNC
 */
HWTEST_F(RSDrawFrameTest, TimeoutRender_StateTransitionTest, TestSize.Level1)
{
    RSDrawFrame drawFrame;
    auto& tr = drawFrame.timeoutRender_;
    // Branch: Update with no prior timeout and no current timeout → no increment
    tr.Update(1000, false);
    ASSERT_EQ(tr.renderTime, 0);
    ASSERT_EQ(tr.frameCount, 0);
    ASSERT_FALSE(tr.IsStatFinished());
    // Branch: Update with timeout=true and frameCount=0 → sets renderTime, increments frameCount
    tr.Update(3000, true);
    ASSERT_EQ(tr.renderTime, 3000);
    ASSERT_EQ(tr.frameCount, 1);
    // Branch: Update with timeout=true and frameCount!=0 → only increments frameCount, renderTime unchanged
    tr.Update(2600, true);
    ASSERT_EQ(tr.renderTime, 3000);
    ASSERT_EQ(tr.frameCount, 2);
    ASSERT_TRUE(tr.IsStatFinished());
    // Branch: Reset clears both fields
    tr.Reset();
    ASSERT_EQ(tr.renderTime, 0);
    ASSERT_EQ(tr.frameCount, 0);
    ASSERT_FALSE(tr.IsStatFinished());
    // EndCheck integration: timeout frame feeds Update via real timer
    drawFrame.StartCheck();
    usleep(2600 * 1000); // 2600ms > 2500ms RENDER_TIMEOUT threshold
    drawFrame.EndCheck();
    ASSERT_EQ(tr.frameCount, 1);
    ASSERT_GT(tr.renderTime, 0);
}

/**
 * @tc.name: LockClient_BranchesTest
 * @tc.desc: LockClient early-return branches (IsStatFinished, HybridEnabled) and main path with Reset verification
 * @tc.type: FUNC
 */
HWTEST_F(RSDrawFrameTest, LockClient_BranchesTest, TestSize.Level1)
{
    RSDrawFrame drawFrame;
    auto& tr = drawFrame.timeoutRender_;
    // Branch: IsStatFinished()=false → early return, hasLockedClient_ unchanged
    ASSERT_FALSE(tr.IsStatFinished());
    ASSERT_FALSE(drawFrame.hasLockedClient_.load());
    drawFrame.LockClient();
    ASSERT_FALSE(drawFrame.hasLockedClient_.load());
#ifdef RS_MODIFIERS_DRAW_ENABLE
    // Branch: IsHybridEnabled()=true → early return before IsStatFinished check
    RSCanvasDrawingRenderNode::hybridEnabled_ = true;
    drawFrame.LockClient();
    bool stillFalse = !drawFrame.hasLockedClient_.load();
    RSCanvasDrawingRenderNode::hybridEnabled_ = false;
    ASSERT_TRUE(stillFalse);
#endif
    // Setup: drive TimeoutRender to IsStatFinished()=true
    tr.Update(3000, true);
    tr.Update(1000, false);
    ASSERT_TRUE(tr.IsStatFinished());
    // Branch: main path → store(true) synchronously, PostTask queued, Reset() clears TimeoutRender
    drawFrame.LockClient();
    ASSERT_TRUE(drawFrame.hasLockedClient_.load());
    ASSERT_EQ(tr.renderTime, 0); // Reset() called
    ASSERT_EQ(tr.frameCount, 0);
}

/**
 * @tc.name: LockThenUnlock_SequenceTest
 * @tc.desc: UnlockClient early return when not locked; Lock→Unlock sequence; lockedClientSet_ non-empty unlock path
 * @tc.type: FUNC
 */
HWTEST_F(RSDrawFrameTest, LockThenUnlock_SequenceTest, TestSize.Level1)
{
    RSDrawFrame drawFrame;
    // Branch: UnlockClient with hasLockedClient_=false → early return
    ASSERT_FALSE(drawFrame.hasLockedClient_.load());
    drawFrame.UnlockClient();
    ASSERT_FALSE(drawFrame.hasLockedClient_.load());
    // Setup: LockClient main path with empty opCountMap → lockedClientSet_ stays empty
    auto& tr = drawFrame.timeoutRender_;
    tr.Update(3000, true);
    tr.Update(1000, false);
    drawFrame.LockClient();
    ASSERT_TRUE(drawFrame.hasLockedClient_.load()); // synchronously set
    ASSERT_EQ(drawFrame.lockedClientSet_.size(), static_cast<size_t>(0));
    // Simulate PostTask execution: lock a pid into lockedClientSet_ (covers opCount>=LIMIT branch)
    pid_t testPid = 1234;
    drawFrame.lockedClientSet_.emplace(testPid);
    ASSERT_EQ(drawFrame.lockedClientSet_.size(), static_cast<size_t>(1));
    // Branch: UnlockClient with hasLockedClient_=true and non-empty lockedClientSet_ → posts unlock task
    drawFrame.UnlockClient();
    ASSERT_TRUE(drawFrame.hasLockedClient_.load()); // still true before PostTask runs
}
#endif
}