/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "params/rs_surface_render_params.h"
#include "pipeline/hardware_thread/rs_hardware_thread.h"
#include "pipeline/render_thread/rs_base_render_engine.h"
#include "pipeline/render_thread/rs_render_engine.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "drawable/rs_display_render_node_drawable.h"
#include "drawable/rs_surface_render_node_drawable.h"
#include "drawable/rs_render_node_drawable.h"
#include <parameters.h>

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace OHOS::Rosen {
class RSUniRenderThreadTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline RSUniRenderThread& uniRenderThread = RSUniRenderThread::Instance();
};

void RSUniRenderThreadTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
    uniRenderThread.runner_ = AppExecFwk::EventRunner::Create("RSUniRenderThread");
    if (!uniRenderThread.runner_) {
        RS_LOGE("RSUniRenderThread Start runner null");
        return;
    }
    uniRenderThread.handler_ = std::make_shared<AppExecFwk::EventHandler>(uniRenderThread.runner_);
    uniRenderThread.runner_->Run();
}

void RSUniRenderThreadTest::TearDownTestCase()
{
    uniRenderThread.uniRenderEngine_->renderContext_ = std::make_shared<RenderContext>();
    uniRenderThread.uniRenderEngine_->renderContext_->drGPUContext_ = std::make_shared<Drawing::GPUContext>();
}

void RSUniRenderThreadTest::SetUp() {}
void RSUniRenderThreadTest::TearDown() {}

/**
 * @tc.name: SetAndGetCaptureParam001
 * @tc.desc: Test SetAndGetCaptureParam
 * @tc.type: FUNC
 * @tc.require: issueIAE59W
 */
HWTEST_F(RSUniRenderThreadTest, SetAndGetCaptureParam001, TestSize.Level1)
{
    RSUniRenderThread& instance = RSUniRenderThread::Instance();
    CaptureParam param;
    param.isSingleSurface_ = true;
    instance.SetCaptureParam(param);
    EXPECT_TRUE(instance.GetCaptureParam().isSingleSurface_);
}

/**
 * @tc.name: InitGrContext001
 * @tc.desc: Test InitGrContext
 * @tc.type: FUNC
 * @tc.require: issueIAE59W
 */
HWTEST_F(RSUniRenderThreadTest, InitGrContext001, TestSize.Level1)
{
    RSUniRenderThread& instance = RSUniRenderThread::Instance();
    instance.InitGrContext();
    ASSERT_NE(instance.uniRenderEngine_, nullptr);
}

/**
 * @tc.name: GetRenderEngine001
 * @tc.desc: Test GetRenderEngine
 * @tc.type: FUNC
 * @tc.require: issueIAE59W
 */
HWTEST_F(RSUniRenderThreadTest, GetRenderEngine001, TestSize.Level1)
{
    RSUniRenderThread& instance = RSUniRenderThread::Instance();
    instance.uniRenderEngine_ = std::make_shared<RSRenderEngine>();
    instance.GetRenderEngine();
    ASSERT_NE(instance.uniRenderEngine_, nullptr);
}

/**
 * @tc.name: PostTask001
 * @tc.desc: Test PostTask
 * @tc.type: FUNC
 * @tc.require: issueIAE59W
 */
HWTEST_F(RSUniRenderThreadTest, PostTask001, TestSize.Level1)
{
    RSUniRenderThread& instance = RSUniRenderThread::Instance();
    std::function<void()> task = []() {};
    instance.PostTask(task);
    ASSERT_NE(instance.handler_, nullptr);
}

/**
 * @tc.name: GetVisibleRect001
 * @tc.desc: Test GetVisibleRect with default
 * @tc.type: FUNC
 * @tc.require: issueIBM5U5
 */
HWTEST_F(RSUniRenderThreadTest, GetVisibleRect001, TestSize.Level1)
{
    RSUniRenderThread& instance = RSUniRenderThread::Instance();
    const auto &visibleRectGet = instance.GetVisibleRect();
    EXPECT_EQ(visibleRectGet.GetLeft(), 0);
    EXPECT_EQ(visibleRectGet.GetTop(), 0);
    EXPECT_EQ(visibleRectGet.GetRight(), 0);
    EXPECT_EQ(visibleRectGet.GetBottom(), 0);
}

/**
 * @tc.name: SetVisibleRect001
 * @tc.desc: Test SetVisibleRect and GetVisibleRect
 * @tc.type: FUNC
 * @tc.require: issueIBM5U5
 */
HWTEST_F(RSUniRenderThreadTest, SetVisibleRect001, TestSize.Level1)
{
    RSUniRenderThread& instance = RSUniRenderThread::Instance();
    // 100 test value for Rect right and bottom
    Drawing::RectI visibleRectSet = Drawing::RectI(0, 0, 100, 100);
    RSUniRenderThread::Instance().SetVisibleRect(visibleRectSet);
    const auto &visibleRectGet = instance.GetVisibleRect();
    EXPECT_EQ(visibleRectGet.GetLeft(), visibleRectSet.GetLeft());
    EXPECT_EQ(visibleRectGet.GetTop(), visibleRectSet.GetTop());
    EXPECT_EQ(visibleRectGet.GetRight(), visibleRectSet.GetRight());
    EXPECT_EQ(visibleRectGet.GetBottom(), visibleRectSet.GetBottom());
}

/**
 * @tc.name: PostRTTask001
 * @tc.desc: Test PostRTTask
 * @tc.type: FUNC
 * @tc.require: issueIAE59W
 */
HWTEST_F(RSUniRenderThreadTest, PostRTTask001, TestSize.Level1)
{
    RSUniRenderThread& instance = RSUniRenderThread::Instance();
    std::function<void()> task = []() {};
    auto tid = gettid();
    instance.PostRTTask(task);
    EXPECT_FALSE(instance.tid_ == tid);

    instance.tid_ = tid;
    instance.PostRTTask(task);
    EXPECT_TRUE(instance.tid_ == tid);
}

/**
 * @tc.name: PostImageReleaseTask001
 * @tc.desc: Test PostImageReleaseTask
 * @tc.type: FUNC
 * @tc.require: issueIAE59W
 */
HWTEST_F(RSUniRenderThreadTest, PostImageReleaseTask001, TestSize.Level1)
{
    RSUniRenderThread& instance = RSUniRenderThread::Instance();
    std::function<void()> task = []() {};
    instance.postImageReleaseTaskFlag_ = true;
    instance.PostImageReleaseTask(task);
    EXPECT_TRUE(instance.postImageReleaseTaskFlag_);

    instance.postImageReleaseTaskFlag_ = false;
    instance.PostImageReleaseTask(task);
    EXPECT_FALSE(instance.postImageReleaseTaskFlag_);

    instance.tid_ = 0;
    instance.PostImageReleaseTask(task);
    EXPECT_FALSE(instance.postImageReleaseTaskFlag_);
}

/**
 * @tc.name: RunImageReleaseTask001
 * @tc.desc: Test RunImageReleaseTask
 * @tc.type: FUNC
 * @tc.require: issueIAE59W
 */
HWTEST_F(RSUniRenderThreadTest, RunImageReleaseTask001, TestSize.Level1)
{
    RSUniRenderThread& instance = RSUniRenderThread::Instance();
    instance.postImageReleaseTaskFlag_ = true;
    instance.RunImageReleaseTask();
    EXPECT_TRUE(instance.imageReleaseCount_ == 0);

    instance.postImageReleaseTaskFlag_ = false;
    instance.RunImageReleaseTask();
    EXPECT_TRUE(instance.imageReleaseTasks_.empty());
}

/**
 * @tc.name: PostTask002
 * @tc.desc: Test PostTask
 * @tc.type: FUNC
 * @tc.require: issueIAE59W
 */
HWTEST_F(RSUniRenderThreadTest, PostTask002, TestSize.Level1)
{
    RSUniRenderThread& instance = RSUniRenderThread::Instance();
    std::function<void()> task = []() {};
    std::string name = "test";
    instance.PostTask(task, name, 0, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    ASSERT_NE(instance.handler_, nullptr);
}

/**
 * @tc.name: RemoveTask001
 * @tc.desc: Test RemoveTask
 * @tc.type: FUNC
 * @tc.require: issueIAE59W
 */
HWTEST_F(RSUniRenderThreadTest, RemoveTask001, TestSize.Level1)
{
    RSUniRenderThread& instance = RSUniRenderThread::Instance();
    std::string name = "test";
    instance.RemoveTask(name);
    EXPECT_TRUE(instance.handler_);
}

/**
 * @tc.name: PostSyncTask001
 * @tc.desc: Test PostSyncTask
 * @tc.type: FUNC
 * @tc.require: issueIAE59W
 */
HWTEST_F(RSUniRenderThreadTest, PostSyncTask001, TestSize.Level1)
{
    RSUniRenderThread& instance = RSUniRenderThread::Instance();
    std::function<void()> task = []() {};
    instance.PostSyncTask(task);
    ASSERT_NE(instance.handler_, nullptr);
}

/**
 * @tc.name: IsIdleTestTest
 * @tc.desc: Test IsIdleTest
 * @tc.type: FUNC
 * @tc.require: issueIAE59W
 */
HWTEST_F(RSUniRenderThreadTest, IsIdleTestTest, TestSize.Level1)
{
    RSUniRenderThread& instance = RSUniRenderThread::Instance();
    EXPECT_TRUE(instance.IsIdle());
}

/**
 * @tc.name: Render001
 * @tc.desc: Test Render
 * @tc.type: FUNC
 * @tc.require: issueIAE59W
 */
HWTEST_F(RSUniRenderThreadTest, Render001, TestSize.Level1)
{
    RSUniRenderThread& instance = RSUniRenderThread::Instance();
    const std::shared_ptr<RSBaseRenderNode> rootNode = RSMainThread::Instance()->GetContext().GetGlobalRootRenderNode();
    auto ptr = DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(rootNode);
    instance.rootNodeDrawable_ = std::static_pointer_cast<DrawableV2::RSRenderNodeDrawable>(ptr);
    instance.Render();
    EXPECT_TRUE(instance.rootNodeDrawable_);
    instance.vmaOptimizeFlag_ = true;
    instance.vmaCacheCount_ = 1;
    instance.Render();
    ASSERT_EQ(instance.vmaCacheCount_, 0);
}

#ifdef RES_SCHED_ENABLE
/**
 * @tc.name: SubScribeSystemAbility001
 * @tc.desc: Test SubScribeSystemAbility
 * @tc.type: FUNC
 * @tc.require: issueIAE59W
 */
HWTEST_F(RSUniRenderThreadTest, SubScribeSystemAbility001, TestSize.Level1)
{
    RSUniRenderThread& instance = RSUniRenderThread::Instance();
    instance.SubScribeSystemAbility();
    EXPECT_TRUE(instance.rootNodeDrawable_);
}
#endif

/**
 * @tc.name: TrimMem001
 * @tc.desc: Test TrimMem
 * @tc.type: FUNC
 * @tc.require: issueIAE59W
 */
HWTEST_F(RSUniRenderThreadTest, TrimMem001, TestSize.Level1)
{
    RSUniRenderThread& instance = RSUniRenderThread::Instance();
    instance.uniRenderEngine_ = std::make_shared<RSRenderEngine>();
    instance.uniRenderEngine_->renderContext_ = std::make_shared<RenderContext>();
    instance.uniRenderEngine_->renderContext_->drGPUContext_ = std::make_shared<Drawing::GPUContext>();
    std::string dumpString = "";
    std::string type = "";
    instance.TrimMem(dumpString, type);
    EXPECT_TRUE(type.empty());

    dumpString = "";
    type = "cpu";
    instance.TrimMem(dumpString, type);
    dumpString = "";
    type = "gpu";
    instance.TrimMem(dumpString, type);
    EXPECT_FALSE(type.empty());

    dumpString = "";
    type = "uihidden";
    instance.TrimMem(dumpString, type);
    dumpString = "";
    type = "unlock";
    instance.TrimMem(dumpString, type);
    EXPECT_FALSE(type.empty());

    dumpString = "";
    type = "shader";
    instance.TrimMem(dumpString, type);
    dumpString = "";
    type = "flushcache";
    instance.TrimMem(dumpString, type);
    EXPECT_FALSE(type.empty());

    dumpString = "";
    type = "setgpulimit";
    instance.TrimMem(dumpString, type);
    EXPECT_FALSE(type.empty());
}

/**
 * @tc.name: ClearMemoryCache001
 * @tc.desc: Test ClearMemoryCache
 * @tc.type: FUNC
 * @tc.require: issueIAE59W
 */
HWTEST_F(RSUniRenderThreadTest, ClearMemoryCache001, TestSize.Level1)
{
    RSUniRenderThread& instance = RSUniRenderThread::Instance();
    instance.ClearMemoryCache(ClearMemoryMoment::FILTER_INVALID, true, 0);
    EXPECT_FALSE(instance.clearMemoryFinished_);
}

/**
 * @tc.name: DefaultClearMemoryCache001
 * @tc.desc: Test DefaultClearMemoryCache
 * @tc.type: FUNC
 * @tc.require: issueIAE59W
 */
HWTEST_F(RSUniRenderThreadTest, DefaultClearMemoryCache001, TestSize.Level1)
{
    RSUniRenderThread& instance = RSUniRenderThread::Instance();
    instance.DefaultClearMemoryCache();
    EXPECT_FALSE(instance.clearMemoryFinished_);
}

/**
 * @tc.name: ResetClearMemoryTask001
 * @tc.desc: Test ResetClearMemoryTask
 * @tc.type: FUNC
 * @tc.require: issueIAE59W
 */
HWTEST_F(RSUniRenderThreadTest, ResetClearMemoryTask001, TestSize.Level1)
{
    RSUniRenderThread& instance = RSUniRenderThread::Instance();
    instance.ResetClearMemoryTask();
    EXPECT_FALSE(instance.clearMemoryFinished_);

    instance.clearMemoryFinished_ = true;
    instance.ResetClearMemoryTask();
    EXPECT_TRUE(instance.clearMemoryFinished_);
}

/**
 * @tc.name: PurgeCacheBetweenFrames001
 * @tc.desc: Test PurgeCacheBetweenFrames
 * @tc.type: FUNC
 * @tc.require: issueIAE59W
 */
HWTEST_F(RSUniRenderThreadTest, PurgeCacheBetweenFrames001, TestSize.Level1)
{
    RSUniRenderThread& instance = RSUniRenderThread::Instance();
    instance.PurgeCacheBetweenFrames();
    EXPECT_TRUE(instance.clearMemoryFinished_);

    instance.uniRenderEngine_ = std::make_shared<RSRenderEngine>();
    instance.uniRenderEngine_->renderContext_ = std::make_shared<RenderContext>();
    instance.uniRenderEngine_->renderContext_->drGPUContext_ = std::make_shared<Drawing::GPUContext>();
    instance.PurgeCacheBetweenFrames();
    EXPECT_TRUE(instance.uniRenderEngine_);
}

/**
 * @tc.name: RenderServiceTreeDump001
 * @tc.desc: Test RenderServiceTreeDump
 * @tc.type: FUNC
 * @tc.require: issueIAE59W
 */
HWTEST_F(RSUniRenderThreadTest, RenderServiceTreeDump001, TestSize.Level1)
{
    RSUniRenderThread& instance = RSUniRenderThread::Instance();
    std::string dumpString = "test";
    instance.RenderServiceTreeDump(dumpString);
    EXPECT_TRUE(instance.clearMemoryFinished_);

    instance.rootNodeDrawable_ = nullptr;
    instance.RenderServiceTreeDump(dumpString);
    EXPECT_TRUE(instance.uniRenderEngine_);
}

/**
 * @tc.name: UpdateDisplayNodeScreenId001
 * @tc.desc: Test UpdateDisplayNodeScreenId
 * @tc.type: FUNC
 * @tc.require: issueIAE59W
 */
HWTEST_F(RSUniRenderThreadTest, UpdateDisplayNodeScreenId001, TestSize.Level1)
{
    RSUniRenderThread& instance = RSUniRenderThread::Instance();
    instance.UpdateDisplayNodeScreenId();
    std::shared_ptr<RSRenderNode> node = std::make_shared<RSRenderNode>(0);
    RSMainThread::Instance()->context_->globalRootRenderNode_->children_.push_back(std::weak_ptr<RSRenderNode>(node));
    instance.UpdateDisplayNodeScreenId();
    EXPECT_TRUE(instance.clearMemoryFinished_);

    RSMainThread::Instance()->context_->globalRootRenderNode_->children_.clear();
    RSDisplayNodeConfig config;
    std::shared_ptr<RSDisplayRenderNode> renderNode = std::make_shared<RSDisplayRenderNode>(0, config);
    RSMainThread::Instance()->context_->globalRootRenderNode_->children_.push_back(
        std::weak_ptr<RSDisplayRenderNode>(renderNode));
    instance.UpdateDisplayNodeScreenId();
    EXPECT_TRUE(RSMainThread::Instance()->GetContext().GetGlobalRootRenderNode());

    RSMainThread::Instance()->context_->globalRootRenderNode_ = nullptr;
    instance.UpdateDisplayNodeScreenId();
    EXPECT_FALSE(RSMainThread::Instance()->GetContext().GetGlobalRootRenderNode());
}

/**
 * @tc.name: GetDynamicRefreshRate001
 * @tc.desc: Test GetDynamicRefreshRate
 * @tc.type: FUNC
 * @tc.require: issueIAE59W
 */
HWTEST_F(RSUniRenderThreadTest, GetDynamicRefreshRate001, TestSize.Level1)
{
    RSUniRenderThread& instance = RSUniRenderThread::Instance();
    uint32_t res = instance.GetDynamicRefreshRate();
    EXPECT_EQ(res, STANDARD_REFRESH_RATE);
}

/**
 * @tc.name: WaitUntilDisplayNodeBufferReleased001
 * @tc.desc: Test WaitUntilDisplayNodeBufferReleased
 * @tc.type: FUNC
 * @tc.require: issueIAE59W
 */
HWTEST_F(RSUniRenderThreadTest, WaitUntilDisplayNodeBufferReleased001, TestSize.Level1)
{
    RSUniRenderThread& instance = RSUniRenderThread::Instance();
    auto node = std::make_shared<RSRenderNode>(0);
    auto displayNodeDrawable = std::make_shared<RSDisplayRenderNodeDrawable>(std::move(node));
    bool res = instance.WaitUntilDisplayNodeBufferReleased(*displayNodeDrawable);
    EXPECT_TRUE(res);

    displayNodeDrawable->surfaceHandler_ = std::make_shared<RSSurfaceHandler>(0);
    displayNodeDrawable->surfaceHandler_->consumer_ = IConsumerSurface::Create();
    displayNodeDrawable->surfaceCreated_ = true;
    res = instance.WaitUntilDisplayNodeBufferReleased(*displayNodeDrawable);
    EXPECT_TRUE(res);
}

/**
 * @tc.name: GetRefreshRate001
 * @tc.desc: Test GetRefreshRate
 * @tc.type: FUNC
 * @tc.require: issueIAE59W
 */
HWTEST_F(RSUniRenderThreadTest, GetRefreshRate001, TestSize.Level1)
{
    RSUniRenderThread& instance = RSUniRenderThread::Instance();
    uint32_t res = instance.GetRefreshRate();
    EXPECT_TRUE(res == 0);
}

/**
 * @tc.name: GetWatermarkImg
 * @tc.desc: Test GetWatermarkImg
 * @tc.type: FUNC
 * @tc.require: issueIB2I9E
 */
HWTEST_F(RSUniRenderThreadTest, GetWatermarkImg, TestSize.Level1)
{
    RSUniRenderThread& instance = RSUniRenderThread::Instance();
    auto drawingImage = instance.GetWatermarkImg();
    ASSERT_EQ(drawingImage, nullptr);
}

/**
 * @tc.name: ReleaseSelfDrawingNodeBuffer001
 * @tc.desc: Test ReleaseSelfDrawingNodeBuffer
 * @tc.type: FUNC
 * @tc.require: issueIB2I9E
 */
HWTEST_F(RSUniRenderThreadTest, ReleaseSelfDrawingNodeBuffer001, TestSize.Level1)
{
    RSUniRenderThread& instance = RSUniRenderThread::Instance();
    auto surfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(1);
    auto adapter = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceRenderNode));

    adapter->consumerOnDraw_ = IConsumerSurface::Create();
    RSRenderThreadParamsManager::Instance().renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
    auto renderThreadParams = std::make_unique<RSRenderThreadParams>();
    renderThreadParams->selfDrawables_.push_back(surfaceRenderNode->renderDrawable_);
    instance.Sync(move(renderThreadParams));
    auto params = static_cast<RSSurfaceRenderParams*>(surfaceRenderNode->GetRenderParams().get());
    instance.ReleaseSelfDrawingNodeBuffer();
    ASSERT_EQ(params->GetPreBuffer(), nullptr);

    params->isLastFrameHardwareEnabled_ = true;
    instance.ReleaseSelfDrawingNodeBuffer();
    params->preBuffer_ = SurfaceBuffer::Create();
    instance.ReleaseSelfDrawingNodeBuffer();
    ASSERT_EQ(params->GetPreBuffer(), nullptr);

    RSHardwareThread::Instance().delayTime_ = 1;
    instance.ReleaseSelfDrawingNodeBuffer();
    params->isHardwareEnabled_ = true;
    params->layerCreated_ = true;
    instance.ReleaseSelfDrawingNodeBuffer();
    EXPECT_TRUE(params->isHardwareEnabled_);
}

/**
 * @tc.name: AddToReleaseQueueTest
 * @tc.desc: Test AddToReleaseQueue
 * @tc.type: FUNC
 * @tc.require: issueIB2I9E
 */
HWTEST_F(RSUniRenderThreadTest, AddToReleaseQueueTest, TestSize.Level1)
{
    RSUniRenderThread& instance = RSUniRenderThread::Instance();
    auto surface = std::make_shared<Drawing::Surface>();
    instance.AddToReleaseQueue(move(surface));
    ASSERT_EQ(instance.tmpSurfaces_.size(), 1);
}

/**
 * @tc.name: ReleaseSurfaceTest
 * @tc.desc: Test ReleaseSurface
 * @tc.type: FUNC
 * @tc.require: issueIB2I9E
 */
HWTEST_F(RSUniRenderThreadTest, ReleaseSurfaceTest, TestSize.Level1)
{
    RSUniRenderThread& instance = RSUniRenderThread::Instance();
    instance.ReleaseSurface();
    ASSERT_EQ(instance.tmpSurfaces_.size(), 0);
}

/**
 * @tc.name: PostClearMemoryTask001
 * @tc.desc: Test PostClearMemoryTask
 * @tc.type: FUNC
 * @tc.require: issueIB2I9E
 */
HWTEST_F(RSUniRenderThreadTest, PostClearMemoryTask001, TestSize.Level1)
{
    RSUniRenderThread& instance = RSUniRenderThread::Instance();
    ClearMemoryMoment moment = ClearMemoryMoment::COMMON_SURFACE_NODE_HIDE;
    bool deeply = true;
    bool isDefaultClean = true;
    instance.PostClearMemoryTask(moment, deeply, isDefaultClean);
    ASSERT_NE(instance.GetRenderEngine()->GetRenderContext()->GetDrGPUContext(), nullptr);

    isDefaultClean = false;
    instance.PostClearMemoryTask(moment, deeply, isDefaultClean);
    EXPECT_TRUE(instance.exitedPidSet_.size());

    instance.PostClearMemoryTask(ClearMemoryMoment::FILTER_INVALID, true, true);
    EXPECT_TRUE(instance.exitedPidSet_.size());

    instance.GetRenderEngine()->GetRenderContext()->drGPUContext_ = nullptr;
    instance.PostClearMemoryTask(moment, deeply, isDefaultClean);
    ASSERT_EQ(instance.GetRenderEngine()->GetRenderContext()->GetDrGPUContext(), nullptr);
}

/**
 * @tc.name: PostReclaimMemoryTaskTest
 * @tc.desc: Test PostReclaimMemoryTask
 * @tc.type: FUNC
 * @tc.require: issueIB2I9E
 */
HWTEST_F(RSUniRenderThreadTest, PostReclaimMemoryTaskTest, TestSize.Level1)
{
    RSUniRenderThread& instance = RSUniRenderThread::Instance();
    ClearMemoryMoment moment = ClearMemoryMoment::RECLAIM_CLEAN;
    bool isReclaim = true;
    instance.PostReclaimMemoryTask(moment, isReclaim);
    EXPECT_FALSE(instance.isTimeToReclaim_);
}

/**
 * @tc.name: FlushGpuMemoryInWaitQueueBetweenFramesTest
 * @tc.desc: Test FlushGpuMemoryInWaitQueueBetweenFrames
 * @tc.type: FUNC
 * @tc.require: issueIB2I9E
 */
HWTEST_F(RSUniRenderThreadTest, FlushGpuMemoryInWaitQueueBetweenFramesTest, TestSize.Level1)
{
    RSUniRenderThread& instance = RSUniRenderThread::Instance();
    instance.FlushGpuMemoryInWaitQueueBetweenFrames();
    ASSERT_NE(instance.uniRenderEngine_, nullptr);
    ASSERT_NE(instance.uniRenderEngine_->GetRenderContext(), nullptr);
}

/**
 * @tc.name: SuppressGpuCacheBelowCertainRatioBetweenFramesTest
 * @tc.desc: Test SuppressGpuCacheBelowCertainRatioBetweenFrames
 * @tc.type: FUNC
 * @tc.require: issueIB2I9E
 */
HWTEST_F(RSUniRenderThreadTest, SuppressGpuCacheBelowCertainRatioBetweenFramesTest, TestSize.Level1)
{
    RSUniRenderThread& instance = RSUniRenderThread::Instance();
    instance.SuppressGpuCacheBelowCertainRatioBetweenFrames();
    ASSERT_NE(instance.uniRenderEngine_, nullptr);
    ASSERT_NE(instance.uniRenderEngine_->GetRenderContext(), nullptr);
}

/**
 * @tc.name: IsColorFilterModeOn
 * @tc.desc: Test IsColorFilterModeOn
 * @tc.type: FUNC
 * @tc.require: issueIALVZN
 */
HWTEST_F(RSUniRenderThreadTest, IsColorFilterModeOn, TestSize.Level1)
{
    RSUniRenderThread& instance = RSUniRenderThread::Instance();
    instance.uniRenderEngine_ = std::make_shared<RSRenderEngine>();
    ASSERT_NE(instance.uniRenderEngine_, nullptr);
    instance.uniRenderEngine_->SetColorFilterMode(ColorFilterMode::COLOR_FILTER_END);
    ASSERT_FALSE(instance.IsColorFilterModeOn());
    instance.uniRenderEngine_->SetColorFilterMode(ColorFilterMode::INVERT_COLOR_ENABLE_MODE);
    ASSERT_TRUE(instance.IsColorFilterModeOn());
}

/**
 * @tc.name: IsHighContrastTextModeOn
 * @tc.desc: Test IsHighContrastTextModeOn
 * @tc.type: FUNC
 * @tc.require: issueIALVZN
 */
HWTEST_F(RSUniRenderThreadTest, IsHighContrastTextModeOn, TestSize.Level1)
{
    RSUniRenderThread& instance = RSUniRenderThread::Instance();
    instance.uniRenderEngine_ = std::make_shared<RSRenderEngine>();
    ASSERT_NE(instance.uniRenderEngine_, nullptr);
    instance.uniRenderEngine_->SetHighContrast(true);
    ASSERT_TRUE(instance.IsHighContrastTextModeOn());
    instance.uniRenderEngine_->SetHighContrast(false);
    ASSERT_FALSE(instance.IsHighContrastTextModeOn());
}

/**
 * @tc.name: GetActualTimestamp
 * @tc.desc: Test GetActualTimestamp
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderThreadTest, GetActualTimestamp, TestSize.Level1)
{
    auto& instance = RSUniRenderThread::Instance();
    auto& renderThreadParams = instance.GetRSRenderThreadParams();
    int64_t actualTimestamp = instance.GetActualTimestamp();
    if (renderThreadParams) {
        EXPECT_EQ(actualTimestamp, renderThreadParams->GetActualTimestamp());
    } else {
        EXPECT_EQ(actualTimestamp, 0);
    }
}

/**
 * @tc.name: GetVsyncId
 * @tc.desc: Test GetVsyncId
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderThreadTest, GetVsyncId, TestSize.Level1)
{
    auto& instance = RSUniRenderThread::Instance();
    auto& renderThreadParams = instance.GetRSRenderThreadParams();
    int64_t vsyncId = instance.GetVsyncId();
    if (renderThreadParams) {
        EXPECT_EQ(vsyncId, renderThreadParams->GetVsyncId());
    } else {
        EXPECT_EQ(vsyncId, 0);
    }
}

/**
 * @tc.name: GetForceRefreshFlag
 * @tc.desc: Test GetForceRefreshFlag
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderThreadTest, GetForceRefreshFlag, TestSize.Level1)
{
    auto& instance = RSUniRenderThread::Instance();
    auto& renderThreadParams = instance.GetRSRenderThreadParams();
    int64_t forceRefreshFlag = instance.GetForceRefreshFlag();
    if (renderThreadParams) {
        EXPECT_EQ(forceRefreshFlag, renderThreadParams->GetForceRefreshFlag());
    } else {
        EXPECT_FALSE(forceRefreshFlag);
    }
}

/**
 * @tc.name: GetFastComposeTimeStampDiff
 * @tc.desc: Test GetFastComposeTimeStampDiff
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderThreadTest, GetFastComposeTimeStampDiff, TestSize.Level1)
{
    auto& instance = RSUniRenderThread::Instance();
    auto& renderThreadParams = instance.GetRSRenderThreadParams();
    uint64_t fastComposeTimeStampDiff = instance.GetFastComposeTimeStampDiff();
    if (renderThreadParams) {
        EXPECT_EQ(fastComposeTimeStampDiff, renderThreadParams->GetFastComposeTimeStampDiff());
    } else {
        EXPECT_EQ(fastComposeTimeStampDiff, 0);
    }
}
}
