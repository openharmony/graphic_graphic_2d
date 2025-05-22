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

#include <parameters.h>

#include "drawable/rs_display_render_node_drawable.h"
#include "params/rs_display_render_params.h"
#include "params/rs_render_thread_params.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_hardware_thread.h"
#include "pipeline/rs_render_engine.h"
#include "pipeline/rs_uifirst_manager.h"
#include "pipeline/rs_uni_render_thread.h"
#include "pipeline/rs_uni_render_util.h"
#include "pipeline/rs_uni_render_virtual_processor.h"
#include "platform/drawing/rs_surface_converter.h"
#include "pipeline/rs_render_engine.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace OHOS::Rosen {
namespace {
constexpr int32_t DEFAULT_CANVAS_SIZE = 100;
constexpr NodeId DEFAULT_ID = 0xFFFF;
constexpr NodeId DEFAULT_SURFACE_NODE_ID = 1;
constexpr NodeId DEFAULT_RENDER_NODE_ID = 2;
}
class RSDisplayRenderNodeDrawableTest : public testing::Test {
public:
    std::shared_ptr<RSDisplayRenderNode> renderNode_;
    std::shared_ptr<RSDisplayRenderNode> mirroredNode_;
    RSRenderNodeDrawableAdapter* drawable_ = nullptr;
    RSRenderNodeDrawableAdapter* mirroredDrawable_ = nullptr;
    RSDisplayRenderNodeDrawable* displayDrawable_ = nullptr;
    RSDisplayRenderNodeDrawable* mirroredDisplayDrawable_ = nullptr;

    std::shared_ptr<Drawing::Canvas> drawingCanvas_;
    std::shared_ptr<RSPaintFilterCanvas> canvas_;

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline NodeId id = DEFAULT_ID;
};

void RSDisplayRenderNodeDrawableTest::SetUpTestCase() {}
void RSDisplayRenderNodeDrawableTest::TearDownTestCase() {}
void RSDisplayRenderNodeDrawableTest::SetUp()
{
    RSDisplayNodeConfig config;
    renderNode_ = std::make_shared<RSDisplayRenderNode>(DEFAULT_ID, config);
    mirroredNode_ = std::make_shared<RSDisplayRenderNode>(DEFAULT_ID + 1, config);
    if (!renderNode_ || !mirroredNode_) {
        RS_LOGE("RSDisplayRenderNodeDrawableTest: failed to create display node.");
    }
    drawable_ = RSDisplayRenderNodeDrawable::OnGenerate(renderNode_);
    mirroredDrawable_ = RSDisplayRenderNodeDrawable::OnGenerate(mirroredNode_);
    if (drawable_ && mirroredDrawable_) {
        displayDrawable_ = static_cast<RSDisplayRenderNodeDrawable*>(drawable_);
        displayDrawable_->renderParams_ = std::make_unique<RSDisplayRenderParams>(id);
        mirroredDisplayDrawable_ = static_cast<RSDisplayRenderNodeDrawable*>(mirroredDrawable_);
        if (!drawable_->renderParams_ || !mirroredDrawable_->renderParams_) {
            RS_LOGE("RSDisplayRenderNodeDrawableTest: failed to init render params.");
        }
        drawingCanvas_ = std::make_unique<Drawing::Canvas>(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
        if (drawingCanvas_) {
            canvas_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvas_.get());
        }
        if (displayDrawable_) {
            displayDrawable_->curCanvas_ = canvas_;
        }
    } else {
        RS_LOGE("RSDisplayRenderNodeDrawableTest: failed to create drawable.");
    }
}
void RSDisplayRenderNodeDrawableTest::TearDown() {}

/**
 * @tc.name: CreateDisplayRenderNodeDrawableTest
 * @tc.desc: Test If DisplayRenderNodeDrawable Can Be Created
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, CreateDisplayRenderNodeDrawable, TestSize.Level1)
{
    RSDisplayNodeConfig config;
    NodeId id = 1;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    auto drawable = RSDisplayRenderNodeDrawable::OnGenerate(displayNode);
    ASSERT_NE(drawable, nullptr);
}

/**
 * @tc.name: UpdateSlrScale001
 * @tc.desc: Test UpdateSlrScale
 * @tc.type: FUNC
 * @tc.require: #IBIOQ4
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, UpdateSlrScale001, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    auto param = system::GetParameter("rosen.SLRScale.enabled", "");
    const int32_t width = DEFAULT_CANVAS_SIZE * 2;
    const int32_t height = DEFAULT_CANVAS_SIZE * 2;
    ScreenInfo screenInfo = {
        .phyWidth = DEFAULT_CANVAS_SIZE,
        .phyHeight = DEFAULT_CANVAS_SIZE,
        .width = width,
        .height = height,
        .isSamplingOn = true,
    };
    system::SetParameter("rosen.SLRScale.enabled", "1");
    displayDrawable_->UpdateSlrScale(screenInfo);
    ASSERT_NE(displayDrawable_->slrScale_, nullptr);
    EXPECT_EQ(screenInfo.samplingDistance, displayDrawable_->slrScale_->GetKernelSize());

    system::SetParameter("rosen.SLRScale.enabled", "0");
    displayDrawable_->UpdateSlrScale(screenInfo);
    EXPECT_EQ(displayDrawable_->slrScale_, nullptr);
    system::SetParameter("rosen.SLRScale.enabled", param);
}

/**
 * @tc.name: ScaleCanvasIfNeeded
 * @tc.desc: Test ScaleCanvasIfNeeded
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, ScaleCanvasIfNeeded001, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    auto param = system::GetParameter("rosen.SLRScale.enabled", "");
    ScreenInfo screenInfo = {
        .phyWidth = DEFAULT_CANVAS_SIZE,
        .phyHeight = DEFAULT_CANVAS_SIZE,
        .width = DEFAULT_CANVAS_SIZE,
        .height = DEFAULT_CANVAS_SIZE,
        .isSamplingOn = false,
    };
    system::SetParameter("rosen.SLRScale.enabled", "1");
    displayDrawable_->ScaleCanvasIfNeeded(screenInfo);
    ASSERT_EQ(displayDrawable_->slrScale_, nullptr);

    screenInfo.isSamplingOn = true;
    system::SetParameter("rosen.SLRScale.enabled", "0");
    displayDrawable_->ScaleCanvasIfNeeded(screenInfo);
    ASSERT_EQ(displayDrawable_->slrScale_, nullptr);
    system::SetParameter("rosen.SLRScale.enabled", param);
}

/**
 * @tc.name: PrepareOffscreenRender001
 * @tc.desc: Test PrepareOffscreenRender, if offscreenWidth/offscreenHeight were not initialized.
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, PrepareOffscreenRender001, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(renderNode_, nullptr);
    displayDrawable_->PrepareOffscreenRender(*displayDrawable_);

    auto params = static_cast<RSDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto type = system::GetParameter("const.window.foldscreen.type", "");
    system::SetParameter("const.window.foldscreen.type", "1");
    params->isRotationChanged_ = true;
    params->frameRect_ = { 0.f, 0.f, 1.f, 0.f };
    displayDrawable_->PrepareOffscreenRender(*displayDrawable_);
    ASSERT_TRUE(params->IsRotationChanged());

    params->frameRect_ = { 0.f, 0.f, 1.f, 1.f };
    displayDrawable_->PrepareOffscreenRender(*displayDrawable_);
    ASSERT_FALSE(displayDrawable_->curCanvas_->GetSurface());

    auto surface = std::make_shared<Drawing::Surface>();
    displayDrawable_->curCanvas_->surface_ = surface.get();
    displayDrawable_->PrepareOffscreenRender(*displayDrawable_);
    ASSERT_TRUE(displayDrawable_->curCanvas_->GetSurface());
    system::SetParameter("const.window.foldscreen.type", type);
}

/**
 * @tc.name: PrepareOffscreenRender002
 * @tc.desc: Test PrepareOffscreenRender, offscreenWidth/offscreenHeight is/are correctly initialized.
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, PrepareOffscreenRender002, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(renderNode_, nullptr);
    renderNode_->GetMutableRenderProperties().SetFrameWidth(DEFAULT_CANVAS_SIZE);
    renderNode_->GetMutableRenderProperties().SetFrameHeight(DEFAULT_CANVAS_SIZE);
    displayDrawable_->PrepareOffscreenRender(*displayDrawable_);
}

/**
 * @tc.name: ClearTransparentBeforeSaveLayer
 * @tc.desc: Test ClearTransparentBeforeSaveLayer, with two surface with/without param initialization
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, ClearTransparentBeforeSaveLayer, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    auto& rtThread = RSUniRenderThread::Instance();
    if (!rtThread.GetRSRenderThreadParams()) {
        RSRenderThreadParamsManager::Instance().renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
    }
    NodeId id = 1;
    auto surfaceNode1 = std::make_shared<RSSurfaceRenderNode>(id);
    auto drawable1 = RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode1);
    id = 2;
    auto surfaceNode2 = std::make_shared<RSSurfaceRenderNode>(id);
    auto drawable2 = RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode2);
    surfaceNode2->InitRenderParams();
    rtThread.GetRSRenderThreadParams()->hardwareEnabledTypeDrawables_.push_back(std::make_pair(1, drawable1));
    rtThread.GetRSRenderThreadParams()->hardwareEnabledTypeDrawables_.push_back(std::make_pair(1, drawable2));
    ASSERT_NE(renderNode_, nullptr);
    renderNode_->GetMutableRenderProperties().SetFrameWidth(DEFAULT_CANVAS_SIZE);
    renderNode_->GetMutableRenderProperties().SetFrameHeight(DEFAULT_CANVAS_SIZE);
    displayDrawable_->ClearTransparentBeforeSaveLayer();
}

/**
 * @tc.name: DrawCurtainScreen
 * @tc.desc: Test DrawCurtainScreen
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, DrawCurtainScreen, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    displayDrawable_->DrawCurtainScreen();
    ASSERT_TRUE(!RSUniRenderThread::Instance().IsCurtainScreenOn());
    auto params = std::make_unique<RSRenderThreadParams>();
    params->isCurtainScreenOn_ = true;
    RSUniRenderThread::Instance().Sync(move(params));
    RSUniRenderThread::Instance().GetRSRenderThreadParams()->isCurtainScreenOn_ = true;
    displayDrawable_->DrawCurtainScreen();
    ASSERT_TRUE(RSUniRenderThread::Instance().IsCurtainScreenOn());
    RSUniRenderThread::Instance().GetRSRenderThreadParams()->isCurtainScreenOn_ = false;
}

/**
 * @tc.name: DrawWatermarkIfNeed
 * @tc.desc: Test DrawWatermarkIfNeed
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, DrawWatermarkIfNeed, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(canvas_, nullptr);
    auto params = static_cast<RSDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_TRUE(params);
    if (params) {
        displayDrawable_->DrawWatermarkIfNeed(*params, *canvas_);
    }

    ASSERT_TRUE(RSUniRenderThread::Instance().GetRSRenderThreadParams());

    RSUniRenderThread::Instance().GetRSRenderThreadParams()->watermarkFlag_ = true;
    displayDrawable_->DrawWatermarkIfNeed(*params, *canvas_);

    RSUniRenderThread::Instance().GetRSRenderThreadParams()->watermarkImg_ = std::make_shared<Drawing::Image>();
    displayDrawable_->DrawWatermarkIfNeed(*params, *canvas_);
    RSUniRenderThread::Instance().GetRSRenderThreadParams()->watermarkFlag_ = false;
    RSUniRenderThread::Instance().GetRSRenderThreadParams()->watermarkImg_ = nullptr;
}

/**
 * @tc.name: CalculateVirtualDirtyForWiredScreen001
 * @tc.desc: Test CalculateVirtualDirtyForWiredScreen, without mirrorNode
 * @tc.type: FUNC
 * @tc.require: #IA76UC
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, CalculateVirtualDirtyForWiredScreen001, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);

    auto params = std::make_shared<RSDisplayRenderParams>(0);
    ASSERT_NE(params, nullptr);
    auto renderFrame = std::make_unique<RSRenderFrame>(nullptr, nullptr);
    ASSERT_NE(renderFrame, nullptr);
    Drawing::Matrix canvasMatrix;
    auto damageRects = displayDrawable_->CalculateVirtualDirtyForWiredScreen(
        renderFrame, *params, canvasMatrix);
    ASSERT_EQ(damageRects.size(), 0);

    auto node = std::make_shared<RSRenderNode>(0);
    params->mirrorSourceDrawable_ = DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(node);
    damageRects = displayDrawable_->CalculateVirtualDirtyForWiredScreen(renderFrame, *params, canvasMatrix);
    ASSERT_EQ(damageRects.size(), 0);
}

/**
 * @tc.name: CalculateVirtualDirtyForWiredScreen002
 * @tc.desc: Test CalculateVirtualDirtyForWiredScreen, isVirtualDirtyEnabled_ false
 * @tc.type: FUNC
 * @tc.require: #IA76UC
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, CalculateVirtualDirtyForWiredScreen002, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(mirroredNode_, nullptr);

    auto params = std::make_shared<RSDisplayRenderParams>(0);
    ASSERT_NE(params, nullptr);
    auto renderFrame = std::make_unique<RSRenderFrame>(nullptr, nullptr);
    ASSERT_NE(renderFrame, nullptr);
    Drawing::Matrix canvasMatrix;
    params->mirrorSourceDrawable_ = mirroredNode_->GetRenderDrawable();
    auto& rtThread = RSUniRenderThread::Instance();
    if (rtThread.GetRSRenderThreadParams()) {
        rtThread.GetRSRenderThreadParams()->isVirtualDirtyEnabled_ = false;
    }
    auto damageRects = displayDrawable_->CalculateVirtualDirtyForWiredScreen(
        renderFrame, *params, canvasMatrix);
    ASSERT_EQ(damageRects.size(), 0);
}

/**
 * @tc.name: CalculateVirtualDirtyForWiredScreen003
 * @tc.desc: Test CalculateVirtualDirtyForWiredScreen, without syncDirtyManager
 * @tc.type: FUNC
 * @tc.require: #IA76UC
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, CalculateVirtualDirtyForWiredScreen003, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(mirroredNode_, nullptr);

    auto params = std::make_shared<RSDisplayRenderParams>(0);
    ASSERT_NE(params, nullptr);
    auto renderFrame = std::make_unique<RSRenderFrame>(nullptr, nullptr);
    ASSERT_NE(renderFrame, nullptr);
    Drawing::Matrix canvasMatrix;
    params->mirrorSourceDrawable_ = mirroredNode_->GetRenderDrawable();
    auto& rtThread = RSUniRenderThread::Instance();
    if (rtThread.GetRSRenderThreadParams()) {
        rtThread.GetRSRenderThreadParams()->isVirtualDirtyEnabled_ = true;
    }
    auto damageRects = displayDrawable_->CalculateVirtualDirtyForWiredScreen(
        renderFrame, *params, canvasMatrix);
    ASSERT_EQ(damageRects.size(), 0);
}

/**
 * @tc.name: CalculateVirtualDirtyForWiredScreen004
 * @tc.desc: Test CalculateVirtualDirtyForWiredScreen, canvasMatrix not equals to lastMatrix_
 * @tc.type: FUNC
 * @tc.require: #IA76UC
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, CalculateVirtualDirtyForWiredScreen004, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(mirroredNode_, nullptr);

    auto params = std::make_shared<RSDisplayRenderParams>(0);
    ASSERT_NE(params, nullptr);
    auto renderFrame = std::make_unique<RSRenderFrame>(nullptr, nullptr);
    ASSERT_NE(renderFrame, nullptr);
    Drawing::Matrix canvasMatrix;
    params->mirrorSourceDrawable_ = mirroredNode_->GetRenderDrawable();
    auto& rtThread = RSUniRenderThread::Instance();
    if (rtThread.GetRSRenderThreadParams()) {
        rtThread.GetRSRenderThreadParams()->isVirtualDirtyEnabled_ = true;
    }
    displayDrawable_->syncDirtyManager_ = std::make_shared<RSDirtyRegionManager>(false);
    const Drawing::scalar scale = 100.0f;
    canvasMatrix.SetScale(scale, scale);
    auto damageRects = displayDrawable_->CalculateVirtualDirtyForWiredScreen(
        renderFrame, *params, canvasMatrix);
    ASSERT_EQ(damageRects.size(), 0);
}

/**
 * @tc.name: CalculateVirtualDirtyForWiredScreen005
 * @tc.desc: Test CalculateVirtualDirtyForWiredScreen, extraDirty is not empty
 * @tc.type: FUNC
 * @tc.require: #IA76UC
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, CalculateVirtualDirtyForWiredScreen005, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(mirroredNode_, nullptr);

    auto params = std::make_shared<RSDisplayRenderParams>(0);
    ASSERT_NE(params, nullptr);
    auto renderFrame = std::make_unique<RSRenderFrame>(nullptr, nullptr);
    ASSERT_NE(renderFrame, nullptr);
    Drawing::Matrix canvasMatrix;
    params->mirrorSourceDrawable_ = mirroredNode_->GetRenderDrawable();
    auto& rtThread = RSUniRenderThread::Instance();
    if (rtThread.GetRSRenderThreadParams()) {
        rtThread.GetRSRenderThreadParams()->isVirtualDirtyEnabled_ = true;
    }
    displayDrawable_->syncDirtyManager_ = std::make_shared<RSDirtyRegionManager>(false);
    displayDrawable_->syncDirtyManager_->dirtyRegion_ = RectI(0, 0, DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    auto damageRects = displayDrawable_->CalculateVirtualDirtyForWiredScreen(renderFrame, *params, canvasMatrix);
    ASSERT_EQ(damageRects.size(), 0);
}

/**
 * @tc.name: CalculateVirtualDirtyForWiredScreen006
 * @tc.desc: Test CalculateVirtualDirtyForWiredScreen, extraDirty is not empty
 * @tc.type: FUNC
 * @tc.require: #IA76UC
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, CalculateVirtualDirtyForWiredScreen006, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(mirroredNode_, nullptr);

    auto params = std::make_shared<RSDisplayRenderParams>(0);
    ASSERT_NE(params, nullptr);
    auto renderFrame = std::make_unique<RSRenderFrame>(nullptr, nullptr);
    ASSERT_NE(renderFrame, nullptr);
    Drawing::Matrix canvasMatrix;
    params->mirrorSourceDrawable_ = mirroredNode_->GetRenderDrawable();
    auto& rtThread = RSUniRenderThread::Instance();
    if (rtThread.GetRSRenderThreadParams()) {
        rtThread.GetRSRenderThreadParams()->isVirtualDirtyEnabled_ = true;
        rtThread.GetRSRenderThreadParams()->isVirtualDirtyDfxEnabled_ = true;
    }
    displayDrawable_->syncDirtyManager_ = std::make_shared<RSDirtyRegionManager>(false);
    auto damageRects = displayDrawable_->CalculateVirtualDirtyForWiredScreen(
        renderFrame, *params, canvasMatrix);
    ASSERT_EQ(damageRects.size(), 0);
}

/**
 * @tc.name: RequestFrame
 * @tc.desc: Test RequestFrame
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, RequestFrameTest, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
    
    auto params = static_cast<RSDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto processor = RSProcessorFactory::CreateProcessor(params->GetCompositeType());
    auto result = displayDrawable_->RequestFrame(*params, processor);
    ASSERT_EQ(result, nullptr);

    RSUniRenderThread::Instance().uniRenderEngine_ = std::make_shared<RSRenderEngine>();
    result = displayDrawable_->RequestFrame(*params, processor);
    ASSERT_EQ(result, nullptr);

    displayDrawable_->surfaceCreated_ = true;
    result = displayDrawable_->RequestFrame(*params, processor);
    ASSERT_EQ(result, nullptr);
    RSUniRenderThread::Instance().uniRenderEngine_ = nullptr;
}

/**
 * @tc.name: CheckDisplayNodeSkip
 * @tc.desc: Test CheckDisplayNodeSkip
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, CheckDisplayNodeSkipTest, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
    
    auto params = static_cast<RSDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(params, nullptr);
    auto processor = RSProcessorFactory::CreateProcessor(params->GetCompositeType());
    RSUniRenderThread::Instance().Sync(std::make_unique<RSRenderThreadParams>());
    auto result = displayDrawable_->CheckDisplayNodeSkip(*params, processor);
    ASSERT_EQ(result, true);

    RSUniRenderThread::Instance().uniRenderEngine_ = std::make_shared<RSRenderEngine>();
    RSUniRenderThread::Instance().GetRSRenderThreadParams()->isForceCommitLayer_ = true;
    result = displayDrawable_->CheckDisplayNodeSkip(*params, processor);
    ASSERT_EQ(result, true);

    RSMainThread::Instance()->isDirty_ = true;
    result = displayDrawable_->CheckDisplayNodeSkip(*params, processor);
    ASSERT_EQ(result, false);

    RSUifirstManager::Instance().hasDoneNode_ = true;
    result = displayDrawable_->CheckDisplayNodeSkip(*params, processor);
    ASSERT_EQ(result, false);

    params->isMainAndLeashSurfaceDirty_ = true;
    result = displayDrawable_->CheckDisplayNodeSkip(*params, processor);
    ASSERT_EQ(result, false);

    displayDrawable_->syncDirtyManager_->currentFrameDirtyRegion_ = { 0, 0, 1, 1 };
    displayDrawable_->syncDirtyManager_->debugRect_ = { 1, 1, 1, 1 };
    result = displayDrawable_->CheckDisplayNodeSkip(*params, processor);
    ASSERT_EQ(result, false);
    RSUniRenderThread::Instance().uniRenderEngine_ = nullptr;
    RSUniRenderThread::Instance().GetRSRenderThreadParams()->isForceCommitLayer_ = false;
    RSMainThread::Instance()->isDirty_ = false;
    RSUifirstManager::Instance().hasDoneNode_ = false;
}

/**
 * @tc.name: CheckAndUpdateFilterCacheOcclusion
 * @tc.desc: Test CheckAndUpdateFilterCacheOcclusion
 * @tc.type: FUNC
 * @tc.require: issueIAL2EA
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, CheckAndUpdateFilterCacheOcclusionTest, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);

    std::shared_ptr<RSSurfaceRenderNode> surfaceNode = std::make_shared<RSSurfaceRenderNode>(DEFAULT_SURFACE_NODE_ID);
    ASSERT_NE(surfaceNode, nullptr);

    auto surfaceDrawableAdapter = RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode);
    ASSERT_NE(surfaceDrawableAdapter, nullptr);

    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawableAdapter->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    auto screenInfo = surfaceDrawableAdapter->renderParams_->GetScreenInfo();
    auto params = static_cast<RSDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(params, nullptr);
    vector<std::shared_ptr<RSRenderNodeDrawableAdapter>> allSurfaceDrawable{surfaceDrawableAdapter};
    params->SetAllMainAndLeashSurfaceDrawables(allSurfaceDrawable);

    surfaceParams->windowInfo_.isMainWindowType_ = false;
    RSDisplayRenderNodeDrawable::CheckAndUpdateFilterCacheOcclusion(*params, screenInfo);

    surfaceParams->windowInfo_.isMainWindowType_ = true;
    RSDisplayRenderNodeDrawable::CheckAndUpdateFilterCacheOcclusion(*params, screenInfo);
}

/**
 * @tc.name: CheckFilterCacheFullyCovered
 * @tc.desc: Test CheckFilterCacheFullyCovered
 * @tc.type: FUNC
 * @tc.require: issueIAL2EA
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, CheckFilterCacheFullyCoveredTest, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);

    auto surfaceParams = std::make_unique<RSSurfaceRenderParams>(DEFAULT_SURFACE_NODE_ID);
    ASSERT_NE(surfaceParams, nullptr);
    surfaceParams->visibleFilterChild_ = {DEFAULT_RENDER_NODE_ID};

    std::shared_ptr<RSRenderNode> renderNode = std::make_shared<RSRenderNode>(DEFAULT_RENDER_NODE_ID);
    ASSERT_NE(renderNode, nullptr);
    auto renderDrawableAdapter = RSRenderNodeDrawableAdapter::OnGenerate(renderNode);
    ASSERT_NE(renderDrawableAdapter, nullptr);
    renderDrawableAdapter->renderParams_ = std::make_unique<RSRenderParams>(DEFAULT_RENDER_NODE_ID);
    ASSERT_NE(renderDrawableAdapter->renderParams_, nullptr);
    RectI screenRect{0, 0, 0, 0};

    renderDrawableAdapter->renderParams_->SetHasBlurFilter(false);
    RSDisplayRenderNodeDrawable::CheckFilterCacheFullyCovered(*surfaceParams, screenRect);

    renderDrawableAdapter->renderParams_->SetHasBlurFilter(true);
    surfaceParams->isTransparent_ = true;
    RSDisplayRenderNodeDrawable::CheckFilterCacheFullyCovered(*surfaceParams, screenRect);

    renderDrawableAdapter->renderParams_->SetEffectNodeShouldPaint(false);
    RSDisplayRenderNodeDrawable::CheckFilterCacheFullyCovered(*surfaceParams, screenRect);

    renderDrawableAdapter->renderParams_->SetNodeType(RSRenderNodeType::EFFECT_NODE);
    RSDisplayRenderNodeDrawable::CheckFilterCacheFullyCovered(*surfaceParams, screenRect);

    surfaceParams->isTransparent_ = false;
    RSDisplayRenderNodeDrawable::CheckFilterCacheFullyCovered(*surfaceParams, screenRect);

    renderDrawableAdapter->renderParams_->SetHasGlobalCorner(true);
    RSDisplayRenderNodeDrawable::CheckFilterCacheFullyCovered(*surfaceParams, screenRect);

    renderDrawableAdapter->renderParams_->SetGlobalAlpha(0.f);
    RSDisplayRenderNodeDrawable::CheckFilterCacheFullyCovered(*surfaceParams, screenRect);
}

/**
 * @tc.name: OnDraw
 * @tc.desc: Test OnDraw
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, OnDrawTest, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    Drawing::Canvas canvas;
    displayDrawable_->OnDraw(canvas);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
}

/**
 * @tc.name: DrawMirrorScreen
 * @tc.desc: Test DrawMirrorScreen
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, DrawMirrorScreenTest, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
    ASSERT_NE(mirroredNode_, nullptr);

    auto& rtThread = RSUniRenderThread::Instance();
    if (!rtThread.GetRSRenderThreadParams()) {
        rtThread.Sync(std::make_unique<RSRenderThreadParams>());
    }
    rtThread.GetRSRenderThreadParams()->isVirtualDirtyEnabled_ = true;
    CaptureParam param;
    param.isSingleSurface_ = true;
    rtThread.SetCaptureParam(param);

    auto params = static_cast<RSDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    if (mirroredNode_->GetRenderDrawable() == nullptr) {
        mirroredNode_->renderDrawable_ = DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(mirroredNode_);
    }
    params->mirrorSourceDrawable_ = mirroredNode_->GetRenderDrawable();
    auto processor = RSProcessorFactory::CreateProcessor(params->GetCompositeType());
    displayDrawable_->DrawMirrorScreen(*params, processor);
}

/**
 * @tc.name: CalculateVirtualDirty
 * @tc.desc: Test CalculateVirtualDirty
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, CalculateVirtualDirtyTest, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
    ASSERT_NE(mirroredNode_, nullptr);

    auto& rtThread = RSUniRenderThread::Instance();
    if (!rtThread.GetRSRenderThreadParams()) {
        rtThread.Sync(std::make_unique<RSRenderThreadParams>());
    }
    displayDrawable_->PrepareOffscreenRender(*displayDrawable_);
    auto params = static_cast<RSDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    if (mirroredNode_->GetRenderDrawable() == nullptr) {
        mirroredNode_->renderDrawable_ = DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(mirroredNode_);
    }
    params->mirrorSourceDrawable_ = mirroredNode_->GetRenderDrawable();
    auto processor = RSProcessorFactory::CreateProcessor(params->GetCompositeType());
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    Drawing::Matrix matrix;
    displayDrawable_->CalculateVirtualDirty(virtualProcesser, *params, matrix);
}

/**
 * @tc.name: DrawMirror
 * @tc.desc: Test DrawMirror
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, DrawMirrorTest, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
    
    displayDrawable_->PrepareOffscreenRender(*displayDrawable_);
    auto params = static_cast<RSDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto processor = RSProcessorFactory::CreateProcessor(params->GetCompositeType());
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    auto uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams().get();

    displayDrawable_->DrawMirror(*params, virtualProcesser,
        &RSDisplayRenderNodeDrawable::OnCapture, *uniParam);
    displayDrawable_->DrawMirror(*params, virtualProcesser,
        &RSDisplayRenderNodeDrawable::DrawHardwareEnabledNodes, *uniParam);
}

/**
 * @tc.name: DrawExpandScreen
 * @tc.desc: Test DrawExpandScreen
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, DrawExpandScreenTest, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);

    auto virtualProcesser = new RSUniRenderVirtualProcessor();
    displayDrawable_->DrawExpandScreen(*virtualProcesser);
}

/**
 * @tc.name: WiredScreenProjection
 * @tc.desc: Test WiredScreenProjection
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, WiredScreenProjectionTest, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
    
    auto params = static_cast<RSDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto processor = RSProcessorFactory::CreateProcessor(params->GetCompositeType());
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    displayDrawable_->WiredScreenProjection(*params, virtualProcesser);
}

/**
 * @tc.name: SkipFrameByIntervalTest001
 * @tc.desc: test SkipFrameByInterval for refreshRate 0 and skipFrameInterval 0
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, SkipFrameByIntervalTest001, TestSize.Level1)
{
    uint32_t refreshRate = 0;
    uint32_t skipFrameInterval = 0;
    ASSERT_FALSE(displayDrawable_->SkipFrameByInterval(refreshRate, skipFrameInterval));
}

/**
 * @tc.name: SkipFrameByIntervalTest002
 * @tc.desc: test SkipFrameByInterval for skipFrameInterval 0
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, SkipFrameByIntervalTest002, TestSize.Level1)
{
    uint32_t refreshRate = 60; // 60hz
    uint32_t skipFrameInterval = 0;
    ASSERT_FALSE(displayDrawable_->SkipFrameByInterval(refreshRate, skipFrameInterval));
}

/**
 * @tc.name: SkipFrameByIntervalTest003
 * @tc.desc: test SkipFrameByInterval for skipFrameInterval 1
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, SkipFrameByIntervalTest003, TestSize.Level1)
{
    uint32_t refreshRate = 60; // 60hz
    uint32_t skipFrameInterval = 1;
    ASSERT_FALSE(displayDrawable_->SkipFrameByInterval(refreshRate, skipFrameInterval));
}

/**
 * @tc.name: SkipFrameByIntervalTest004
 * @tc.desc: test SkipFrameByInterval for time within skipFrameInterval 2
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, SkipFrameByIntervalTest004, TestSize.Level1)
{
    uint32_t refreshRate = 60; // 60hz
    uint32_t skipFrameInterval = 2; // skipFrameInterval 2
    displayDrawable_->SkipFrameByInterval(refreshRate, skipFrameInterval);
    ASSERT_TRUE(displayDrawable_->SkipFrameByInterval(refreshRate, skipFrameInterval));
}

/**
 * @tc.name: SkipFrameByIntervalTest005
 * @tc.desc: test SkipFrameByInterval for time over skipFrameInterval 2
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, SkipFrameByIntervalTest005, TestSize.Level1)
{
    uint32_t refreshRate = 60; // 60hz
    uint32_t skipFrameInterval = 2; // skipFrameInterval 2
    displayDrawable_->SkipFrameByInterval(refreshRate, skipFrameInterval);
    usleep(50000); // 50000us == 50ms
    ASSERT_FALSE(displayDrawable_->SkipFrameByInterval(refreshRate, skipFrameInterval));
}

/**
 * @tc.name: SkipFrameByIntervalTest006
 * @tc.desc: test SkipFrameByInterval for time within skipFrameInterval 6
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, SkipFrameByIntervalTest006, TestSize.Level1)
{
    uint32_t refreshRate = 60; // 60hz
    uint32_t skipFrameInterval = 6; // skipFrameInterval 6
    displayDrawable_->SkipFrameByInterval(refreshRate, skipFrameInterval);
    usleep(50000); // 50000us == 50ms
    ASSERT_TRUE(displayDrawable_->SkipFrameByInterval(refreshRate, skipFrameInterval));
}

/**
 * @tc.name: SkipFrameByIntervalTest007
 * @tc.desc: test SkipFrameByInterval for time over skipFrameInterval 6
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, SkipFrameByIntervalTest007, TestSize.Level1)
{
    uint32_t refreshRate = 60; // 60hz
    uint32_t skipFrameInterval = 6; // skipFrameInterval 6
    displayDrawable_->SkipFrameByInterval(refreshRate, skipFrameInterval);
    usleep(150000); // 150000us == 150ms
    ASSERT_FALSE(displayDrawable_->SkipFrameByInterval(refreshRate, skipFrameInterval));
}

/**
 * @tc.name: SkipFrameByRefreshRateTest001
 * @tc.desc: test SkipFrameByRefreshRate with expectedRefreshRate 0
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, SkipFrameByRefreshRateTest001, TestSize.Level1)
{
    uint32_t refreshRate = 60; // 60hz
    uint32_t expectedRefreshRate = 0;
    ASSERT_FALSE(displayDrawable_->SkipFrameByRefreshRate(refreshRate, expectedRefreshRate));
}

/**
 * @tc.name: SkipFrameByRefreshRateTest002
 * @tc.desc: test SkipFrameByRefreshRate with expectedRefreshRate UINT32_MAX
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, SkipFrameByRefreshRateTest002, TestSize.Level1)
{
    uint32_t refreshRate = 60; // 60hz
    uint32_t expectedRefreshRate = UINT32_MAX;
    ASSERT_FALSE(displayDrawable_->SkipFrameByRefreshRate(refreshRate, expectedRefreshRate));
}

/**
 * @tc.name: SkipFrameByRefreshRateTest003
 * @tc.desc: test SkipFrameByRefreshRate with expectedRefreshRate 60
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, SkipFrameByRefreshRateTest003, TestSize.Level1)
{
    uint32_t refreshRate = 60; // 60hz
    uint32_t expectedRefreshRate = 60; // 60hz
    displayDrawable_->SkipFrameByRefreshRate(refreshRate, expectedRefreshRate);
    usleep(5000); // 5000us == 5ms
    ASSERT_FALSE(displayDrawable_->SkipFrameByRefreshRate(refreshRate, expectedRefreshRate));
}

/**
 * @tc.name: SkipFrameByRefreshRateTest004
 * @tc.desc: test SkipFrameByRefreshRate with expectedRefreshRate 60
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, SkipFrameByRefreshRateTest004, TestSize.Level1)
{
    uint32_t refreshRate = 60; // 60hz
    uint32_t expectedRefreshRate = 60; // 60hz
    displayDrawable_->SkipFrameByRefreshRate(refreshRate, expectedRefreshRate);
    usleep(100000); // 100000us == 100ms
    ASSERT_FALSE(displayDrawable_->SkipFrameByRefreshRate(refreshRate, expectedRefreshRate));
}

/**
 * @tc.name: SkipFrameTest001
 * @tc.desc: test SkipFrame with SKIP_FRAME_BY_INTERVAL
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, SkipFrameTest001, TestSize.Level1)
{
    uint32_t refreshRate = 60; // 60hz
    ScreenInfo screenInfo;
    screenInfo.skipFrameStrategy = SKIP_FRAME_BY_INTERVAL;
    screenInfo.skipFrameInterval = 2; // skipFrameInterval 2
    displayDrawable_->SkipFrame(refreshRate, screenInfo);
    usleep(5000); // 5000us == 5ms
    ASSERT_TRUE(displayDrawable_->SkipFrame(refreshRate, screenInfo));
}

/**
 * @tc.name: SkipFrameTest002
 * @tc.desc: test SkipFrame with SKIP_FRAME_BY_REFRESH_RATE
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, SkipFrameTest002, TestSize.Level1)
{
    uint32_t refreshRate = 60; // 60hz
    ScreenInfo screenInfo;
    screenInfo.skipFrameStrategy = SKIP_FRAME_BY_REFRESH_RATE;
    screenInfo.expectedRefreshRate = 30; // expectedRefreshRate 30
    displayDrawable_->SkipFrame(refreshRate, screenInfo);
    usleep(5000); // 5000us == 5ms
    ASSERT_TRUE(displayDrawable_->SkipFrame(refreshRate, screenInfo));
}

/**
 * @tc.name: GetSpecialLayerType
 * @tc.desc: Test GetSpecialLayerType
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, GetSpecialLayerType, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);

    auto params = static_cast<RSDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(params, nullptr);
    int32_t result = displayDrawable_->GetSpecialLayerType(*params);
    ASSERT_EQ(result, 0);
    params->hasCaptureWindow_.insert(std::make_pair(params->screenId_, true));
    result = displayDrawable_->GetSpecialLayerType(*params);
    ASSERT_EQ(result, 2);

    displayDrawable_->currentBlackList_.insert(0);
    result = displayDrawable_->GetSpecialLayerType(*params);
    ASSERT_EQ(result, 1);
    RSUniRenderThread::Instance().whiteList_.insert(0);
    result = displayDrawable_->GetSpecialLayerType(*params);
    ASSERT_EQ(result, 1);

    params->hasHdrPresent_ = true;
    result = displayDrawable_->GetSpecialLayerType(*params);
    ASSERT_EQ(result, 1);
    RSUniRenderThread::GetCaptureParam().isSnapshot_ = true;
    result = displayDrawable_->GetSpecialLayerType(*params);
    ASSERT_EQ(result, 1);
    RSUniRenderThread::GetCaptureParam().isSnapshot_ = false;
    params->hasHdrPresent_ = false;
    displayDrawable_->currentBlackList_.clear();
    RSUniRenderThread::Instance().whiteList_.clear();
}

/**
 * @tc.name: RotateMirrorCanvas
 * @tc.desc: Test RotateMirrorCanvas
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, RotateMirrorCanvas, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ScreenRotation rotation = ScreenRotation::ROTATION_0;
    displayDrawable_->RotateMirrorCanvas(rotation, 1.f, 1.f);

    rotation = ScreenRotation::ROTATION_90;
    displayDrawable_->RotateMirrorCanvas(rotation, 1.f, -1.f);

    rotation = ScreenRotation::ROTATION_180;
    displayDrawable_->RotateMirrorCanvas(rotation, 2.f, 2.f);

    rotation = ScreenRotation::ROTATION_270;
    displayDrawable_->RotateMirrorCanvas(rotation, -1.f, 1.f);
    ASSERT_TRUE(displayDrawable_->curCanvas_);
}

/**
 * @tc.name: ScaleAndRotateMirrorForWiredScreen
 * @tc.desc: Test ScaleAndRotateMirrorForWiredScreen
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, ScaleAndRotateMirrorForWiredScreen, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    std::shared_ptr<RSDisplayRenderNode> renderNode;
    RSRenderNodeDrawableAdapter* drawable = nullptr;
    RSDisplayRenderNodeDrawable* mirroredDrawable = nullptr;
    RSDisplayNodeConfig config;
    renderNode = std::make_shared<RSDisplayRenderNode>(DEFAULT_ID + 2, config);
    drawable = RSDisplayRenderNodeDrawable::OnGenerate(renderNode);
    if (drawable) {
        mirroredDrawable = static_cast<RSDisplayRenderNodeDrawable*>(drawable);
    }
    displayDrawable_->ScaleAndRotateMirrorForWiredScreen(*mirroredDrawable);
    ASSERT_FALSE(mirroredDrawable->GetRenderParams());

    if (drawable) {
        mirroredDrawable->renderParams_ = std::make_unique<RSDisplayRenderParams>(id);
    }
    displayDrawable_->ScaleAndRotateMirrorForWiredScreen(*mirroredDrawable);
    ASSERT_TRUE(mirroredDrawable->GetRenderParams());
    ASSERT_TRUE(displayDrawable_->GetRenderParams());

    auto screenManagerPtr = impl::RSScreenManager::GetInstance();
    auto* screenManager = static_cast<impl::RSScreenManager*>(screenManagerPtr.GetRefPtr());
    VirtualScreenConfigs configs;
    ScreenId screenId = mirroredDrawable->renderParams_->GetScreenId();
    auto screen = std::make_unique<OHOS::Rosen::impl::RSScreen>(configs);
    screenManager->screens_.insert(std::make_pair(screenId, std::move(screen)));
    displayDrawable_->ScaleAndRotateMirrorForWiredScreen(*mirroredDrawable);
    ASSERT_FALSE(screenManager->screens_.empty());

    screenManager->screens_.clear();
    auto screenPtr = std::make_unique<OHOS::Rosen::impl::RSScreen>(configs);
    screenPtr->screenRotation_ = ScreenRotation::ROTATION_90;
    screenManager->screens_.insert(std::make_pair(screenId, std::move(screenPtr)));
    auto mainScreenInfo = mirroredDrawable->renderParams_->GetScreenInfo();
    mainScreenInfo.width = 1;
    displayDrawable_->ScaleAndRotateMirrorForWiredScreen(*mirroredDrawable);
    ASSERT_FALSE(screenManager->screens_.empty());

    screenManager->screens_.clear();
    auto managerPtr = std::make_unique<OHOS::Rosen::impl::RSScreen>(configs);
    managerPtr->screenRotation_ = ScreenRotation::ROTATION_270;
    screenManager->screens_.insert(std::make_pair(screenId, std::move(managerPtr)));
    mainScreenInfo.height = 1;
    displayDrawable_->ScaleAndRotateMirrorForWiredScreen(*mirroredDrawable);
    mirroredDrawable->renderParams_ = nullptr;
    displayDrawable_->ScaleAndRotateMirrorForWiredScreen(*mirroredDrawable);
    ASSERT_FALSE(mirroredDrawable->renderParams_);
}

/**
 * @tc.name: DrawMirrorCopy
 * @tc.desc: Test DrawMirrorCopy
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, DrawMirrorCopy, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
    ASSERT_NE(mirroredNode_, nullptr);
    std::shared_ptr<RSDisplayRenderNode> renderNode;
    RSRenderNodeDrawableAdapter* drawable = nullptr;
    RSDisplayRenderNodeDrawable* mirrorDrawable = nullptr;
    RSDisplayNodeConfig config;
    renderNode = std::make_shared<RSDisplayRenderNode>(DEFAULT_ID + 2, config);
    drawable = RSDisplayRenderNodeDrawable::OnGenerate(renderNode);
    if (drawable) {
        mirrorDrawable = static_cast<RSDisplayRenderNodeDrawable*>(drawable);
        mirrorDrawable->renderParams_ = std::make_unique<RSDisplayRenderParams>(id);
    }
    auto& rtThread = RSUniRenderThread::Instance();
    if (!rtThread.GetRSRenderThreadParams()) {
        rtThread.Sync(std::make_unique<RSRenderThreadParams>());
    }
    if (mirroredNode_->GetRenderDrawable() == nullptr) {
        mirroredNode_->renderDrawable_ = DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(mirroredNode_);
    }
    auto params = static_cast<RSDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    auto uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams().get();
    params->mirrorSourceDrawable_ = mirroredNode_->GetRenderDrawable();
    displayDrawable_->DrawMirrorCopy(*mirrorDrawable, *params, virtualProcesser, *uniParam);
    ASSERT_TRUE(uniParam->IsVirtualDirtyEnabled());

    uniParam->isVirtualDirtyEnabled_ = false;
    virtualProcesser->renderFrame_ = std::make_unique<RSRenderFrame>(nullptr, nullptr);
    mirrorDrawable->cacheImgForCapture_ = std::make_shared<Drawing::Image>();
    ASSERT_TRUE(mirrorDrawable->GetCacheImgForCapture());
    displayDrawable_->DrawMirrorCopy(*mirrorDrawable, *params, virtualProcesser, *uniParam);

    Drawing::Canvas drawingCanvas;
    virtualProcesser->canvas_ = std::make_unique<RSPaintFilterCanvas>(&drawingCanvas);
    ASSERT_TRUE(virtualProcesser->GetCanvas());
    mirrorDrawable->cacheImgForCapture_ = std::make_shared<Drawing::Image>();
    displayDrawable_->DrawMirrorCopy(*mirrorDrawable, *params, virtualProcesser, *uniParam);
    ASSERT_TRUE(virtualProcesser->GetCanvas());
    uniParam->isVirtualDirtyEnabled_ = true;
}

/**
 * @tc.name: OnCapture
 * @tc.desc: Test OnCapture
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, OnCapture, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
    Drawing::Canvas canvas;
    auto params = static_cast<RSDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    displayDrawable_->OnCapture(canvas);

    RSUniRenderThread::GetCaptureParam().isMirror_ = true;
    displayDrawable_->OnCapture(canvas);

    params->hasCaptureWindow_.insert(std::make_pair(params->screenId_, true));
    displayDrawable_->OnCapture(canvas);

    RSUniRenderThread::GetCaptureParam().isMirror_ = false;
    displayDrawable_->OnCapture(canvas);
    ASSERT_FALSE(RSUniRenderThread::GetCaptureParam().isMirror_);
    params->hasCaptureWindow_.clear();
}

/**
 * @tc.name: DrawHardwareEnabledNodes001
 * @tc.desc: Test DrawHardwareEnabledNodes
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, DrawHardwareEnabledNodes001, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
    Drawing::Canvas canvas;
    auto params = static_cast<RSDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    displayDrawable_->DrawHardwareEnabledNodes(canvas, *params);
    ASSERT_FALSE(displayDrawable_->GetRSSurfaceHandlerOnDraw()->GetBuffer());

    displayDrawable_->surfaceHandler_->buffer_.buffer = SurfaceBuffer::Create();
    displayDrawable_->DrawHardwareEnabledNodes(canvas, *params);
    ASSERT_TRUE(displayDrawable_->GetRSSurfaceHandlerOnDraw()->GetBuffer());

    NodeId id = 1;
    auto rsSurfaceNode = std::make_shared<RSSurfaceRenderNode>(id);
    auto drawableAdapter = RSRenderNodeDrawableAdapter::OnGenerate(rsSurfaceNode);
    id = 2;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id);
    auto drawable = RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode);
    params->hardwareEnabledDrawables_.push_back(drawableAdapter);
    params->hardwareEnabledTopDrawables_.push_back(drawable);
    ASSERT_TRUE(params->GetHardwareEnabledDrawables().size());
    ASSERT_TRUE(params->GetHardwareEnabledTopDrawables().size());
    displayDrawable_->DrawHardwareEnabledNodes(canvas, *params);
    ASSERT_FALSE(params->GetHardwareEnabledDrawables().size());
    ASSERT_FALSE(params->GetHardwareEnabledTopDrawables().size());

    RSUniRenderThread::Instance().uniRenderEngine_ = std::make_shared<RSRenderEngine>();
    RSUniRenderThread::Instance().uniRenderEngine_->colorFilterMode_ = ColorFilterMode::INVERT_COLOR_DISABLE_MODE;
    displayDrawable_->DrawHardwareEnabledNodes(canvas, *params);

    RSUniRenderThread::Instance().uniRenderEngine_->colorFilterMode_ = ColorFilterMode::DALTONIZATION_TRITANOMALY_MODE;
    displayDrawable_->DrawHardwareEnabledNodes(canvas, *params);
    ASSERT_TRUE(RSUniRenderThread::Instance().GetRenderEngine());
    RSUniRenderThread::Instance().uniRenderEngine_ = nullptr;
}

/**
 * @tc.name: DrawHardwareEnabledNodes002
 * @tc.desc: Test DrawHardwareEnabledNodes
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, DrawHardwareEnabledNodes002, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    Drawing::Canvas canvas;
    displayDrawable_->DrawHardwareEnabledNodes(canvas);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
}

/**
 * @tc.name: DrawHardwareEnabledNodesMissedInCacheImage
 * @tc.desc: Test DrawHardwareEnabledNodesMissedInCacheImage
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, DrawHardwareEnabledNodesMissedInCacheImage, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
    Drawing::Canvas canvas;
    auto params = static_cast<RSDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    displayDrawable_->DrawHardwareEnabledNodesMissedInCacheImage(canvas);
    ASSERT_FALSE(params->GetHardwareEnabledDrawables().size() != 0);

    NodeId id = 1;
    auto rsSurfaceNode = std::make_shared<RSSurfaceRenderNode>(id);
    auto drawableAdapter = RSRenderNodeDrawableAdapter::OnGenerate(rsSurfaceNode);
    params->hardwareEnabledDrawables_.push_back(drawableAdapter);
    ASSERT_TRUE(params->GetHardwareEnabledDrawables().size() != 0);
    displayDrawable_->DrawHardwareEnabledNodesMissedInCacheImage(canvas);
    ASSERT_FALSE(params->GetHardwareEnabledDrawables().size() != 0);
}

/**
 * @tc.name: SwitchColorFilter
 * @tc.desc: Test SwitchColorFilter
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, SwitchColorFilter, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    displayDrawable_->SwitchColorFilter(canvas);
    ASSERT_FALSE(RSUniRenderThread::Instance().GetRenderEngine());

    RSUniRenderThread::Instance().uniRenderEngine_ = std::make_shared<RSRenderEngine>();
    displayDrawable_->SwitchColorFilter(canvas);

    RSUniRenderThread::Instance().uniRenderEngine_->colorFilterMode_ = ColorFilterMode::DALTONIZATION_NORMAL_MODE;
    displayDrawable_->SwitchColorFilter(canvas);

    RSUniRenderThread::Instance().uniRenderEngine_->colorFilterMode_ = ColorFilterMode::INVERT_COLOR_DISABLE_MODE;
    displayDrawable_->SwitchColorFilter(canvas);
    displayDrawable_->SwitchColorFilter(canvas, 0.6);

    RSUniRenderThread::Instance().uniRenderEngine_->colorFilterMode_ = ColorFilterMode::INVERT_COLOR_ENABLE_MODE;
    displayDrawable_->SwitchColorFilter(canvas);
    displayDrawable_->SwitchColorFilter(canvas, 0.6);

    ASSERT_TRUE(RSUniRenderThread::Instance().GetRenderEngine());
    RSUniRenderThread::Instance().uniRenderEngine_ = nullptr;
}

/**
 * @tc.name: FindHardwareEnabledNodes
 * @tc.desc: Test FindHardwareEnabledNodes
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, FindHardwareEnabledNodes, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
    auto params = static_cast<RSDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    displayDrawable_->FindHardwareEnabledNodes(*params);
    ASSERT_EQ(RSUniRenderThread::Instance().GetRSRenderThreadParams()->hardwareEnabledTypeDrawables_.size(), 2);
}

/**
 * @tc.name: AdjustZOrderAndDrawSurfaceNode
 * @tc.desc: Test AdjustZOrderAndDrawSurfaceNode
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, AdjustZOrderAndDrawSurfaceNode, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
    if (RSUniRenderThread::Instance().uniRenderEngine_ == nullptr) {
        RSUniRenderThread::Instance().uniRenderEngine_ = std::make_shared<RSRenderEngine>();
    }
    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr> drawables;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    auto params = static_cast<RSDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto drawingCanvas = std::make_shared<Drawing::Canvas>();
    paintFilterCanvas.canvas_ = drawingCanvas.get();
    paintFilterCanvas.canvas_->gpuContext_ = std::make_shared<Drawing::GPUContext>();
    auto rscanvas = static_cast<Drawing::Canvas*>(&paintFilterCanvas);
    displayDrawable_->AdjustZOrderAndDrawSurfaceNode(drawables, *rscanvas, *params);
    ASSERT_TRUE(drawables.empty());

    std::shared_ptr<RSRenderNodeDrawableAdapter> firstAdapter = nullptr;
    std::shared_ptr<RSRenderNodeDrawableAdapter> secondAdapter = nullptr;
    drawables.push_back(firstAdapter);
    drawables.push_back(secondAdapter);
    displayDrawable_->AdjustZOrderAndDrawSurfaceNode(drawables, *rscanvas, *params);
    ASSERT_TRUE(!firstAdapter);
    ASSERT_TRUE(!secondAdapter);
    drawables.clear();

    NodeId id = 1;
    auto rsSurfaceNode = std::make_shared<RSSurfaceRenderNode>(id);
    auto drawableAdapter = RSRenderNodeDrawableAdapter::OnGenerate(rsSurfaceNode);
    ASSERT_TRUE(drawableAdapter->GetRenderParams());
    id = 2;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id);
    auto drawable = RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode);
    ASSERT_TRUE(drawable->GetRenderParams());
    drawables.push_back(drawableAdapter);
    drawables.push_back(drawable);
    displayDrawable_->AdjustZOrderAndDrawSurfaceNode(drawables, *rscanvas, *params);
    ASSERT_TRUE(drawableAdapter->GetRenderParams());
}

/**
 * @tc.name: FinishOffscreenRender
 * @tc.desc: Test FinishOffscreenRender
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, FinishOffscreenRender, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
    Drawing::SamplingOptions sampling;
    displayDrawable_->FinishOffscreenRender(sampling);
    ASSERT_FALSE(displayDrawable_->canvasBackup_);
}

/**
 * @tc.name: CreateSurface
 * @tc.desc: Test CreateSurface
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, CreateSurface, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    sptr<IBufferConsumerListener> listener;
    bool res = displayDrawable_->CreateSurface(listener);
    ASSERT_FALSE(displayDrawable_->surfaceHandler_->GetConsumer());
    ASSERT_FALSE(displayDrawable_->surface_);
    ASSERT_FALSE(res);

    displayDrawable_->surface_ = nullptr;
    ASSERT_FALSE(displayDrawable_->surface_);
    res = displayDrawable_->CreateSurface(listener);
    ASSERT_FALSE(displayDrawable_->surface_);
    ASSERT_FALSE(res);

    displayDrawable_->surfaceHandler_->consumer_ = nullptr;
    ASSERT_FALSE(displayDrawable_->surfaceHandler_->GetConsumer());
    res = displayDrawable_->CreateSurface(listener);
    ASSERT_FALSE(displayDrawable_->surfaceHandler_->GetConsumer());
    ASSERT_FALSE(res);
}

/**
 * @tc.name: SkipFrameByInterval
 * @tc.desc: Test SkipFrameByInterval
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, SkipFrameByInterval, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    bool res = displayDrawable_->SkipFrameByInterval(0, 2);
    ASSERT_FALSE(res);
    res = displayDrawable_->SkipFrameByInterval(1, 1);
    ASSERT_FALSE(res);
    res = displayDrawable_->SkipFrameByInterval(1, 2);
    ASSERT_FALSE(res);
}

HWTEST_F(RSDisplayRenderNodeDrawableTest, DrawWiredMirrorOnDraw, TestSize.Level2)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(mirroredDisplayDrawable_, nullptr);
    auto params = static_cast<RSDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    RSRenderThreadParamsManager::Instance().renderThreadParams_ = nullptr;
    displayDrawable_->DrawWiredMirrorOnDraw(*mirroredDisplayDrawable_, *params);
    RSRenderThreadParamsManager::Instance().renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
    displayDrawable_->DrawWiredMirrorOnDraw(*mirroredDisplayDrawable_, *params);
}
}
