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

#include "pipeline/hardware_thread/rs_hardware_thread.h"
#include "pipeline/render_thread/rs_base_render_engine.h"
#include "pipeline/render_thread/rs_render_engine.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "params/rs_surface_render_params.h"
#include "drawable/rs_display_render_node_drawable.h"
#include "drawable/rs_surface_render_node_drawable.h"
#include "drawable/rs_render_node_drawable.h"
#include <parameters.h>

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace OHOS::Rosen {
class RSUniRenderThreadExtTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUniRenderThreadExtTest::SetUpTestCase() {}
void RSUniRenderThreadExtTest::TearDownTestCase() {}
void RSUniRenderThreadExtTest::SetUp() {}
void RSUniRenderThreadExtTest::TearDown() {}

/**
 * @tc.name: SetAndGetCaptureParam001
 * @tc.desc: Test SetAndGetCaptureParam
 * @tc.type: FUNC
 * @tc.require: issueIAE59W
 */
HWTEST_F(RSUniRenderThreadExtTest, SetAndGetCaptureParam001, TestSize.Level1)
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
HWTEST_F(RSUniRenderThreadExtTest, InitGrContext001, TestSize.Level1)
{
    RSUniRenderThread& instance = RSUniRenderThread::Instance();
    instance.InitGrContext();
    EXPECT_TRUE(instance.uniRenderEngine_);
}

/**
 * @tc.name: Start001
 * @tc.desc: Test Start
 * @tc.type: FUNC
 * @tc.require: issueIAE59W
 */
HWTEST_F(RSUniRenderThreadExtTest, Start001, TestSize.Level1)
{
    RSUniRenderThread& instance = RSUniRenderThread::Instance();
    instance.rootNodeDrawable_ = nullptr;
    instance.Start();
    EXPECT_TRUE(instance.rootNodeDrawable_);

    instance.Start();
    EXPECT_TRUE(instance.handler_);
}

/**
 * @tc.name: GetRenderEngine001
 * @tc.desc: Test GetRenderEngine
 * @tc.type: FUNC
 * @tc.require: issueIAE59W
 */
HWTEST_F(RSUniRenderThreadExtTest, GetRenderEngine001, TestSize.Level1)
{
    RSUniRenderThread& instance = RSUniRenderThread::Instance();
    instance.uniRenderEngine_ = std::make_shared<RSRenderEngine>();
    instance.GetRenderEngine();
    EXPECT_TRUE(instance.uniRenderEngine_);
}

/**
 * @tc.name: PostTask001
 * @tc.desc: Test PostTask
 * @tc.type: FUNC
 * @tc.require: issueIAE59W
 */
HWTEST_F(RSUniRenderThreadExtTest, PostTask001, TestSize.Level1)
{
    RSUniRenderThread& instance = RSUniRenderThread::Instance();
    std::function<void()> task = []() {};
    instance.PostTask(task);
    EXPECT_TRUE(instance.handler_);

    instance.handler_ = nullptr;
    instance.PostTask(task);
    EXPECT_FALSE(instance.handler_);
}

/**
 * @tc.name: PostRTTask001
 * @tc.desc: Test PostRTTask
 * @tc.type: FUNC
 * @tc.require: issueIAE59W
 */
HWTEST_F(RSUniRenderThreadExtTest, PostRTTask001, TestSize.Level1)
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
HWTEST_F(RSUniRenderThreadExtTest, PostImageReleaseTask001, TestSize.Level1)
{
    RSUniRenderThread& instance = RSUniRenderThread::Instance();
    std::function<void()> task = []() {};
    instance.postImageReleaseTaskFlag_ = true;
    instance.PostImageReleaseTask(task);
    EXPECT_TRUE(instance.postImageReleaseTaskFlag_);

    instance.postImageReleaseTaskFlag_ = false;
    auto product = system::GetParameter("const.build.product", "");
    system::SetParameter("const.build.product", "ALN");
    instance.PostImageReleaseTask(task);
    EXPECT_FALSE(instance.postImageReleaseTaskFlag_);

    instance.tid_ = 0;
    instance.PostImageReleaseTask(task);
    EXPECT_FALSE(instance.postImageReleaseTaskFlag_);
    system::SetParameter("const.build.product", product);
}

/**
 * @tc.name: RunImageReleaseTask001
 * @tc.desc: Test RunImageReleaseTask
 * @tc.type: FUNC
 * @tc.require: issueIAE59W
 */
HWTEST_F(RSUniRenderThreadExtTest, RunImageReleaseTask001, TestSize.Level1)
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
HWTEST_F(RSUniRenderThreadExtTest, PostTask002, TestSize.Level1)
{
    RSUniRenderThread& instance = RSUniRenderThread::Instance();
    std::function<void()> task = []() {};
    std::string name = "test";
    instance.PostTask(task, name, 0, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    EXPECT_FALSE(instance.handler_);

    instance.handler_ = std::make_shared<AppExecFwk::EventHandler>(instance.runner_);
    instance.PostTask(task, name, 0, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    EXPECT_TRUE(instance.handler_);
}

/**
 * @tc.name: Render001
 * @tc.desc: Test Render
 * @tc.type: FUNC
 * @tc.require: issueIAE59W
 */
HWTEST_F(RSUniRenderThreadExtTest, Render001, TestSize.Level1)
{
    RSUniRenderThread& instance = RSUniRenderThread::Instance();
    RSMainThread::Instance()->handler_ = std::make_shared<AppExecFwk::EventHandler>(instance.runner_);
    instance.Render();
    EXPECT_TRUE(instance.rootNodeDrawable_);

    instance.Render();
    EXPECT_TRUE(instance.rootNodeDrawable_);
}

#ifdef RES_SCHED_ENABLE
/**
 * @tc.name: SubScribeSystemAbility001
 * @tc.desc: Test SubScribeSystemAbility
 * @tc.type: FUNC
 * @tc.require: issueIAE59W
 */
HWTEST_F(RSUniRenderThreadExtTest, SubScribeSystemAbility001, TestSize.Level1)
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
HWTEST_F(RSUniRenderThreadExtTest, TrimMem001, TestSize.Level1)
{
    RSUniRenderThread& instance = RSUniRenderThread::Instance();
    std::string dumpString = "";
    std::string type = "";
    instance.uniRenderEngine_ = std::make_shared<RSRenderEngine>();
    instance.uniRenderEngine_->renderContext_ = std::make_shared<RenderContext>();
    instance.uniRenderEngine_->renderContext_->drGPUContext_ = std::make_shared<Drawing::GPUContext>();
    instance.TrimMem(dumpString, type);
    EXPECT_TRUE(type.empty());

    type = "cpu";
    instance.TrimMem(dumpString, type);
    type = "gpu";
    instance.TrimMem(dumpString, type);
    EXPECT_FALSE(type.empty());

    type = "uihidden";
    instance.TrimMem(dumpString, type);
    type = "unlock";
    instance.TrimMem(dumpString, type);
    EXPECT_FALSE(type.empty());

    type = "shader";
    instance.TrimMem(dumpString, type);
    type = "flushcache";
    instance.TrimMem(dumpString, type);
    EXPECT_FALSE(type.empty());
}

/**
 * @tc.name: ClearMemoryCache001
 * @tc.desc: Test ClearMemoryCache
 * @tc.type: FUNC
 * @tc.require: issueIAE59W
 */
HWTEST_F(RSUniRenderThreadExtTest, ClearMemoryCache001, TestSize.Level1)
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
HWTEST_F(RSUniRenderThreadExtTest, DefaultClearMemoryCache001, TestSize.Level1)
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
HWTEST_F(RSUniRenderThreadExtTest, ResetClearMemoryTask001, TestSize.Level1)
{
    RSUniRenderThread& instance = RSUniRenderThread::Instance();
    std::unordered_map<NodeId, bool> ids1;
    instance.ResetClearMemoryTask(std::move(ids1));
    EXPECT_FALSE(instance.clearMemoryFinished_);

    instance.clearMemoryFinished_ = true;
    std::unordered_map<NodeId, bool> ids2;
    instance.ResetClearMemoryTask(std::move(ids2));
    EXPECT_TRUE(instance.clearMemoryFinished_);
}

/**
 * @tc.name: PurgeCacheBetweenFrames001
 * @tc.desc: Test PurgeCacheBetweenFrames
 * @tc.type: FUNC
 * @tc.require: issueIAE59W
 */
HWTEST_F(RSUniRenderThreadExtTest, PurgeCacheBetweenFrames001, TestSize.Level1)
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
 * @tc.name: RemoveTask001
 * @tc.desc: Test RemoveTask
 * @tc.type: FUNC
 * @tc.require: issueIAE59W
 */
HWTEST_F(RSUniRenderThreadExtTest, RemoveTask001, TestSize.Level1)
{
    RSUniRenderThread& instance = RSUniRenderThread::Instance();
    std::string name = "test";
    instance.RemoveTask(name);
    EXPECT_TRUE(instance.handler_);

    instance.handler_ = nullptr;
    instance.RemoveTask(name);
    EXPECT_FALSE(instance.handler_);
}

/**
 * @tc.name: PostSyncTask001
 * @tc.desc: Test PostSyncTask
 * @tc.type: FUNC
 * @tc.require: issueIAE59W
 */
HWTEST_F(RSUniRenderThreadExtTest, PostSyncTask001, TestSize.Level1)
{
    RSUniRenderThread& instance = RSUniRenderThread::Instance();
    std::function<void()> task = []() {};
    instance.PostSyncTask(task);
    EXPECT_FALSE(instance.handler_);

    instance.handler_ = std::make_shared<AppExecFwk::EventHandler>(instance.runner_);
    instance.PostSyncTask(task);
    EXPECT_TRUE(instance.handler_);
}

/**
 * @tc.name: IsIdleAndSync001
 * @tc.desc: Test IsIdle And Sync
 * @tc.type: FUNC
 * @tc.require: issueIAE59W
 */
HWTEST_F(RSUniRenderThreadExtTest, IsIdleAndSync001, TestSize.Level1)
{
    RSUniRenderThread& instance = RSUniRenderThread::Instance();
    bool res = instance.IsIdle();
    EXPECT_TRUE(res);

    instance.Sync(std::make_unique<RSRenderThreadParams>());
    EXPECT_TRUE(instance.GetRSRenderThreadParams());
}

/**
 * @tc.name: RenderServiceTreeDump001
 * @tc.desc: Test RenderServiceTreeDump
 * @tc.type: FUNC
 * @tc.require: issueIAE59W
 */
HWTEST_F(RSUniRenderThreadExtTest, RenderServiceTreeDump001, TestSize.Level1)
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
HWTEST_F(RSUniRenderThreadExtTest, UpdateDisplayNodeScreenId001, TestSize.Level1)
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
HWTEST_F(RSUniRenderThreadExtTest, GetDynamicRefreshRate001, TestSize.Level1)
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
HWTEST_F(RSUniRenderThreadExtTest, WaitUntilDisplayNodeBufferReleased001, TestSize.Level1)
{
    RSUniRenderThread& instance = RSUniRenderThread::Instance();
    auto node = std::make_shared<RSRenderNode>(0);
    auto displayNodeDrawable = std::make_shared<RSDisplayRenderNodeDrawable>(std::move(node));
    bool res = instance.WaitUntilDisplayNodeBufferReleased(*displayNodeDrawable);
    EXPECT_TRUE(res);

    displayNodeDrawable->surfaceHandler_ = std::make_shared<RSSurfaceHandler>(0);
    displayNodeDrawable->surfaceHandler_->consumer_ = IConsumerSurface::Create();
    res = instance.WaitUntilDisplayNodeBufferReleased(*displayNodeDrawable);
    EXPECT_TRUE(res);
}

/**
 * @tc.name: GetRefreshRate001
 * @tc.desc: Test GetRefreshRate
 * @tc.type: FUNC
 * @tc.require: issueIAE59W
 */
HWTEST_F(RSUniRenderThreadExtTest, GetRefreshRate001, TestSize.Level1)
{
    RSUniRenderThread& instance = RSUniRenderThread::Instance();
    uint32_t res = instance.GetRefreshRate();
    EXPECT_TRUE(res == 0);
}

/**
 * @tc.name: ReleaseSelfDrawingNodeBuffer001
 * @tc.desc: Test ReleaseSelfDrawingNodeBuffer
 * @tc.type: FUNC
 * @tc.require: issueIAE59W
 */
HWTEST_F(RSUniRenderThreadExtTest, ReleaseSelfDrawingNodeBuffer001, TestSize.Level1)
{
    RSUniRenderThread& instance = RSUniRenderThread::Instance();
    auto surfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(0);
    std::shared_ptr<const RSRenderNode> node = std::make_shared<const RSRenderNode>(0);
    surfaceRenderNode->renderDrawable_ = std::make_shared<RSRenderNodeDrawable>(std::move(node));
    surfaceRenderNode->renderDrawable_->renderParams_ = std::make_unique<RSSurfaceRenderParams>(0);
    auto surfaceDrawable =
        std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(surfaceRenderNode->renderDrawable_);
    surfaceDrawable->consumerOnDraw_ = IConsumerSurface::Create();
    instance.GetRSRenderThreadParams()->selfDrawables_.push_back(surfaceRenderNode->renderDrawable_);
    auto params = static_cast<RSSurfaceRenderParams*>(surfaceRenderNode->GetRenderParams().get());
    instance.ReleaseSelfDrawingNodeBuffer();
    EXPECT_FALSE(params->GetPreBuffer());

    params->isLastFrameHardwareEnabled_ = true;
    instance.ReleaseSelfDrawingNodeBuffer();
    params->preBuffer_ = SurfaceBuffer::Create();
    instance.ReleaseSelfDrawingNodeBuffer();
    EXPECT_FALSE(params->GetPreBuffer());

    RSHardwareThread::Instance().delayTime_ = 1;
    instance.ReleaseSelfDrawingNodeBuffer();
    params->isHardwareEnabled_ = true;
    params->layerCreated_ = true;
    instance.ReleaseSelfDrawingNodeBuffer();
    EXPECT_TRUE(params->isHardwareEnabled_);
}

/**
 * @tc.name: IsColorFilterModeOn
 * @tc.desc: Test IsColorFilterModeOn
 * @tc.type: FUNC
 * @tc.require: issueIALVZN
 */
HWTEST_F(RSUniRenderThreadExtTest, IsColorFilterModeOn, TestSize.Level1)
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
HWTEST_F(RSUniRenderThreadExtTest, IsHighContrastTextModeOn, TestSize.Level1)
{
    RSUniRenderThread& instance = RSUniRenderThread::Instance();
    instance.uniRenderEngine_ = std::make_shared<RSRenderEngine>();
    ASSERT_NE(instance.uniRenderEngine_, nullptr);
    instance.uniRenderEngine_->SetHighContrast(true);
    ASSERT_TRUE(instance.IsHighContrastTextModeOn());
    instance.uniRenderEngine_->SetHighContrast(false);
    ASSERT_FALSE(instance.IsHighContrastTextModeOn());
}
}