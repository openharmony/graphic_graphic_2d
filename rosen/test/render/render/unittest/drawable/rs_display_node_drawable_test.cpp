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
#include "pipeline/hardware_thread/rs_hardware_thread.h"
#include "pipeline/render_thread/rs_render_engine.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "pipeline/render_thread/rs_uni_render_virtual_processor.h"
#include "pipeline/rs_uifirst_manager.h"
#include "platform/drawing/rs_surface_converter.h"

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
class RSDisplayNodeDrawableTest : public testing::Test {
public:
    std::shared_ptr<RSDisplayRenderNode> renderNode_;
    std::shared_ptr<RSDisplayRenderNode> mirroredNode_;
    RSRenderNodeDrawableAdapter* drawable_ = nullptr;
    RSRenderNodeDrawableAdapter* mirroredDrawable_ = nullptr;
    RSDisplayRenderNodeDrawable* displayDrawable_ = nullptr;
    RSDisplayRenderNodeDrawable* mirroredDisplayDrawable_ = nullptr;

    std::shared_ptr<Drawing::Canvas> drawingCanvas_ = nullptr;
    std::shared_ptr<Drawing::Canvas> drawingCanvasForMirror_ = nullptr;

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline NodeId id = DEFAULT_ID;
};

void RSDisplayNodeDrawableTest::SetUp()
{
    RSDisplayNodeConfig config;
    renderNode_ = std::make_shared<RSDisplayRenderNode>(DEFAULT_ID, config);
    mirroredNode_ = std::make_shared<RSDisplayRenderNode>(DEFAULT_ID + 1, config);
    if (!renderNode_ || !mirroredNode_) {
        RS_LOGE("RSDisplayNodeDrawableTest: failed to create display node.");
        return;
    }

    drawable_ = RSDisplayRenderNodeDrawable::OnGenerate(renderNode_);
    mirroredDrawable_ = RSDisplayRenderNodeDrawable::OnGenerate(mirroredNode_);
    if (!drawable_ || !mirroredDrawable_) {
        RS_LOGE("RSDisplayNodeDrawableTest: failed to create RSRenderNodeDrawableAdapter.");
        return;
    }

    displayDrawable_ = static_cast<RSDisplayRenderNodeDrawable*>(drawable_);
    mirroredDisplayDrawable_ = static_cast<RSDisplayRenderNodeDrawable*>(mirroredDrawable_);
    if (!displayDrawable_ || !mirroredDisplayDrawable_) {
        RS_LOGE("RSDisplayNodeDrawableTest: failed to create drawable.");
        return;
    }

    displayDrawable_->renderParams_ = std::make_unique<RSDisplayRenderParams>(id);
    mirroredDisplayDrawable_->renderParams_ = std::make_unique<RSDisplayRenderParams>(id + 1);
    if (!drawable_->renderParams_ || !mirroredDrawable_->renderParams_) {
        RS_LOGE("RSDisplayNodeDrawableTest: failed to init render params.");
        return;
    }

    // generate canvas for displayDrawable_
    drawingCanvas_ = std::make_unique<Drawing::Canvas>(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    if (drawingCanvas_) {
        displayDrawable_->curCanvas_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvas_.get());
    }
    // generate canvas for mirroredDisplayDrawable_
    drawingCanvasForMirror_ = std::make_unique<Drawing::Canvas>(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    if (drawingCanvasForMirror_) {
        mirroredDisplayDrawable_->curCanvas_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvasForMirror_.get());
    }
}
void RSDisplayNodeDrawableTest::SetUpTestCase() {}
void RSDisplayNodeDrawableTest::TearDownTestCase() {}
void RSDisplayNodeDrawableTest::TearDown() {}

/**
 * @tc.name: PrepareOffscreenRenderExt001
 * @tc.desc: Test PrepareOffscreenRender, if offscreenWidth/offscreenHeight were not initialized.
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSDisplayNodeDrawableTest, PrepareOffscreenRenderExt001, TestSize.Level1)
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
 * @tc.name: CreateDisplayRenderNodeDrawableExtTest
 * @tc.desc: Test If DisplayRenderNodeDrawable Can Be Created
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSDisplayNodeDrawableTest, CreateDisplayRenderNodeDrawableExtTest, TestSize.Level1)
{
    RSDisplayNodeConfig config;
    NodeId id = 1;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    auto drawable = RSDisplayRenderNodeDrawable::OnGenerate(displayNode);
    ASSERT_NE(drawable, nullptr);
}

/**
 * @tc.name: ClearTransparentBeforeSaveLayerExt
 * @tc.desc: Test ClearTransparentBeforeSaveLayer, with two surface with/without param initialization
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSDisplayNodeDrawableTest, ClearTransparentBeforeSaveLayerExt, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    auto& rtThread = RSUniRenderThread::Instance();
    if (!rtThread.GetRSRenderThreadParams()) {
        rtThread.Sync(std::make_unique<RSRenderThreadParams>());
    }
    NodeId id = 1;
    auto surfaceNode1 = std::make_shared<RSSurfaceRenderNode>(id);
    auto drawable1 = RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode1);
    id = 2;
    auto surfaceNode2 = std::make_shared<RSSurfaceRenderNode>(id);
    auto drawable2 = RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode2);
    surfaceNode2->InitRenderParams();
    rtThread.GetRSRenderThreadParams()->hardwareEnabledTypeDrawables_.push_back(drawable1);
    rtThread.GetRSRenderThreadParams()->hardwareEnabledTypeDrawables_.push_back(drawable2);
    ASSERT_NE(renderNode_, nullptr);
    renderNode_->GetMutableRenderProperties().SetFrameWidth(DEFAULT_CANVAS_SIZE);
    renderNode_->GetMutableRenderProperties().SetFrameHeight(DEFAULT_CANVAS_SIZE);
    displayDrawable_->ClearTransparentBeforeSaveLayer();
}

/**
 * @tc.name: PrepareOffscreenRenderExt002
 * @tc.desc: Test PrepareOffscreenRender, offscreenWidth/offscreenHeight is/are correctly initialized.
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSDisplayNodeDrawableTest, PrepareOffscreenRenderExt002, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(renderNode_, nullptr);
    renderNode_->GetMutableRenderProperties().SetFrameWidth(DEFAULT_CANVAS_SIZE);
    renderNode_->GetMutableRenderProperties().SetFrameHeight(DEFAULT_CANVAS_SIZE);
    displayDrawable_->PrepareOffscreenRender(*displayDrawable_);
}

/**
 * @tc.name: DrawWatermarkIfNeedExt
 * @tc.desc: Test DrawWatermarkIfNeed
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSDisplayNodeDrawableTest, DrawWatermarkIfNeedExt, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->curCanvas_, nullptr);
    auto params = static_cast<RSDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_TRUE(params);
    if (params) {
        displayDrawable_->DrawWatermarkIfNeed(*params, *displayDrawable_->curCanvas_);
    }

    ASSERT_TRUE(RSUniRenderThread::Instance().GetRSRenderThreadParams());

    RSUniRenderThread::Instance().GetRSRenderThreadParams()->watermarkFlag_ = true;
    displayDrawable_->DrawWatermarkIfNeed(*params, *displayDrawable_->curCanvas_);

    RSUniRenderThread::Instance().GetRSRenderThreadParams()->watermarkImg_ = std::make_shared<Drawing::Image>();
    displayDrawable_->DrawWatermarkIfNeed(*params, *displayDrawable_->curCanvas_);
    RSUniRenderThread::Instance().GetRSRenderThreadParams()->watermarkFlag_ = false;
    RSUniRenderThread::Instance().GetRSRenderThreadParams()->watermarkImg_ = nullptr;
}

/**
 * @tc.name: DrawCurtainScreenExt
 * @tc.desc: Test DrawCurtainScreen
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSDisplayNodeDrawableTest, DrawCurtainScreenExt, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    displayDrawable_->DrawCurtainScreen();
    ASSERT_TRUE(!RSUniRenderThread::Instance().IsCurtainScreenOn());

    RSUniRenderThread::Instance().GetRSRenderThreadParams()->isCurtainScreenOn_ = true;
    displayDrawable_->DrawCurtainScreen();
    ASSERT_TRUE(RSUniRenderThread::Instance().IsCurtainScreenOn());
    RSUniRenderThread::Instance().GetRSRenderThreadParams()->isCurtainScreenOn_ = false;
}

/**
 * @tc.name: CalculateVirtualDirtyForWiredScreenExt002
 * @tc.desc: Test CalculateVirtualDirtyForWiredScreen, isVirtualDirtyEnabled_ false
 * @tc.type: FUNC
 * @tc.require: #IA76UC
 */
HWTEST_F(RSDisplayNodeDrawableTest, CalculateVirtualDirtyForWiredScreenExt002, TestSize.Level1)
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
 * @tc.name: CalculateVirtualDirtyForWiredScreenExt001
 * @tc.desc: Test CalculateVirtualDirtyForWiredScreen, without mirrorNode
 * @tc.type: FUNC
 * @tc.require: #IA76UC
 */
HWTEST_F(RSDisplayNodeDrawableTest, CalculateVirtualDirtyForWiredScreenExt001, TestSize.Level1)
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
    sleep(1);
}

/**
 * @tc.name: CalculateVirtualDirtyForWiredScreenExt004
 * @tc.desc: Test CalculateVirtualDirtyForWiredScreen, canvasMatrix not equals to lastMatrix_
 * @tc.type: FUNC
 * @tc.require: #IA76UC
 */
HWTEST_F(RSDisplayNodeDrawableTest, CalculateVirtualDirtyForWiredScreenExt004, TestSize.Level1)
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
 * @tc.name: CalculateVirtualDirtyForWiredScreenExt003
 * @tc.desc: Test CalculateVirtualDirtyForWiredScreen, without syncDirtyManager
 * @tc.type: FUNC
 * @tc.require: #IA76UC
 */
HWTEST_F(RSDisplayNodeDrawableTest, CalculateVirtualDirtyForWiredScreenExt003, TestSize.Level1)
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
 * @tc.name: CheckFilterCacheFullyCoveredExtTest
 * @tc.desc: Test CheckFilterCacheFullyCovered
 * @tc.type: FUNC
 * @tc.require: issueIAL2EA
 */
HWTEST_F(RSDisplayNodeDrawableTest, CheckFilterCacheFullyCoveredExtTest, TestSize.Level1)
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

    surfaceParams->isTransparent_ = true;
    surfaceParams->isRotating_ = true;
    RSDisplayRenderNodeDrawable::CheckFilterCacheFullyCovered(*surfaceParams, screenRect);

    surfaceParams->isRotating_ = false;
    RSDisplayRenderNodeDrawable::CheckFilterCacheFullyCovered(*surfaceParams, screenRect);

    surfaceParams->isTransparent_ = false;
    renderDrawableAdapter->renderParams_->SetHasBlurFilter(true);
    RSDisplayRenderNodeDrawable::CheckFilterCacheFullyCovered(*surfaceParams, screenRect);

    renderDrawableAdapter->renderParams_->SetEffectNodeShouldPaint(false);
    RSDisplayRenderNodeDrawable::CheckFilterCacheFullyCovered(*surfaceParams, screenRect);

    renderDrawableAdapter->renderParams_->SetNodeType(RSRenderNodeType::EFFECT_NODE);
    RSDisplayRenderNodeDrawable::CheckFilterCacheFullyCovered(*surfaceParams, screenRect);

    renderDrawableAdapter->renderParams_->SetHasGlobalCorner(true);
    RSDisplayRenderNodeDrawable::CheckFilterCacheFullyCovered(*surfaceParams, screenRect);

    renderDrawableAdapter->renderParams_->SetGlobalAlpha(0.f);
    RSDisplayRenderNodeDrawable::CheckFilterCacheFullyCovered(*surfaceParams, screenRect);
}

/**
 * @tc.name: CalculateVirtualDirtyExtTest
 * @tc.desc: Test CalculateVirtualDirty
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSDisplayNodeDrawableTest, CalculateVirtualDirtyExtTest, TestSize.Level1)
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
    sleep(1);
}

/**
 * @tc.name: DrawMirrorScreenExtTest
 * @tc.desc: Test DrawMirrorScreen
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSDisplayNodeDrawableTest, DrawMirrorScreenExtTest, TestSize.Level1)
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
    sleep(1);
}

/**
 * @tc.name: DrawExpandScreenExtTest
 * @tc.desc: Test DrawExpandScreen
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSDisplayNodeDrawableTest, DrawExpandScreenExtTest, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);

    auto virtualProcesser = new RSUniRenderVirtualProcessor();
    displayDrawable_->DrawExpandScreen(*virtualProcesser);
}

/**
 * @tc.name: DrawMirrorExtTest
 * @tc.desc: Test DrawMirror
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSDisplayNodeDrawableTest, DrawMirrorExtTest, TestSize.Level1)
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
 * @tc.name: SkipDisplayIfScreenOffExt001
 * @tc.desc: Test SkipDisplayIfScreenOff, corner case (node is nullptr), return false
 * @tc.type: FUNC
 * @tc.require: #I9UNQP
 */
HWTEST_F(RSDisplayNodeDrawableTest, SkipDisplayIfScreenOffExt001, TestSize.Level1)
{
    if (!RSSystemProperties::GetSkipDisplayIfScreenOffEnabled() || !RSSystemProperties::IsPhoneType()) {
        return;
    }
    drawable_->renderNode_.reset();
    ASSERT_FALSE(displayDrawable_->SkipDisplayIfScreenOff());
}

/**
 * @tc.name: WiredScreenProjectionExtTest
 * @tc.desc: Test WiredScreenProjection
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSDisplayNodeDrawableTest, WiredScreenProjectionExtTest, TestSize.Level1)
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
 * @tc.name: SkipDisplayIfScreenOffExt003
 * @tc.desc: Test SkipDisplayIfScreenOff, if power off and render one more frame, return true
 * @tc.type: FUNC
 * @tc.require: #I9UNQP
 */
HWTEST_F(RSDisplayNodeDrawableTest, SkipDisplayIfScreenOffExt003, TestSize.Level1)
{
    if (!RSSystemProperties::GetSkipDisplayIfScreenOffEnabled() || !RSSystemProperties::IsPhoneType()) {
        return;
    }

    ScreenId screenId = 1;
    renderNode_->SetScreenId(screenId);

    auto screenManager = CreateOrGetScreenManager();
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);
    screenManager->MarkPowerOffNeedProcessOneFrame();
    screenManagerImpl.screenPowerStatus_[screenId] = ScreenPowerStatus::POWER_STATUS_OFF;
    ASSERT_FALSE(displayDrawable_->SkipDisplayIfScreenOff());
    screenManager->MarkPowerOffNeedProcessOneFrame();
    screenManagerImpl.screenPowerStatus_[screenId] = ScreenPowerStatus::POWER_STATUS_SUSPEND;
    ASSERT_FALSE(displayDrawable_->SkipDisplayIfScreenOff());

    screenManager->ResetPowerOffNeedProcessOneFrame();
    screenManagerImpl.screenPowerStatus_[screenId] = ScreenPowerStatus::POWER_STATUS_OFF;
    ASSERT_FALSE(displayDrawable_->SkipDisplayIfScreenOff());
    screenManager->ResetPowerOffNeedProcessOneFrame();
    screenManagerImpl.screenPowerStatus_[screenId] = ScreenPowerStatus::POWER_STATUS_SUSPEND;
    ASSERT_FALSE(displayDrawable_->SkipDisplayIfScreenOff());
}

/**
 * @tc.name: SkipDisplayIfScreenOffExt002
 * @tc.desc: Test SkipDisplayIfScreenOff, if power off, return true
 * @tc.type: FUNC
 * @tc.require: #I9UNQP
 */
HWTEST_F(RSDisplayNodeDrawableTest, SkipDisplayIfScreenOffExt002, TestSize.Level1)
{
    if (!RSSystemProperties::GetSkipDisplayIfScreenOffEnabled() || !RSSystemProperties::IsPhoneType()) {
        return;
    }

    ScreenId screenId = 1;
    renderNode_->SetScreenId(screenId);

    auto screenManager = CreateOrGetScreenManager();
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);
    screenManagerImpl.screenPowerStatus_[screenId] = ScreenPowerStatus::POWER_STATUS_ON;
    ASSERT_FALSE(displayDrawable_->SkipDisplayIfScreenOff());
    screenManagerImpl.screenPowerStatus_[screenId] = ScreenPowerStatus::POWER_STATUS_OFF;
    ASSERT_FALSE(displayDrawable_->SkipDisplayIfScreenOff());
    screenManagerImpl.screenPowerStatus_[screenId] = ScreenPowerStatus::POWER_STATUS_SUSPEND;
    ASSERT_FALSE(displayDrawable_->SkipDisplayIfScreenOff());
}

/**
 * @tc.name: RotateMirrorCanvasExtTest
 * @tc.desc: Test RotateMirrorCanvas
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSDisplayNodeDrawableTest, RotateMirrorCanvasExtTest, TestSize.Level1)
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
 * @tc.name: GetSpecialLayerTypeExtTest
 * @tc.desc: Test GetSpecialLayerType
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSDisplayNodeDrawableTest, GetSpecialLayerTypeExtTest, TestSize.Level1)
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
 * @tc.name: ScaleAndRotateMirrorForWiredScreenExt002
 * @tc.desc: Test ScaleAndRotateMirrorForWiredScreen while scale of width smaller
 * @tc.type: FUNC
 * @tc.require: issueIANDBE
 */
HWTEST_F(RSDisplayNodeDrawableTest, ScaleAndRotateMirrorForWiredScreenExt002, TestSize.Level2)
{
    // set drawable screenInfo
    ASSERT_NE(displayDrawable_, nullptr);
    auto mainParams = displayDrawable_->GetRenderParams() ?
        static_cast<RSDisplayRenderParams*>(displayDrawable_->GetRenderParams().get()) : nullptr;
    ASSERT_NE(mainParams, nullptr);
    mainParams->screenInfo_.width = 100;
    mainParams->screenInfo_.height = 100;

    ASSERT_NE(mirroredDrawable_, nullptr);
    auto mirroredParams = mirroredDrawable_->GetRenderParams() ?
        static_cast<RSDisplayRenderParams*>(mirroredDrawable_->GetRenderParams().get()) : nullptr;
    ASSERT_NE(mirroredParams, nullptr);
    mirroredParams->screenInfo_.width = 50;
    mirroredParams->screenInfo_.height = 100;

    // sacle mirror screen
    mirroredDisplayDrawable_->ScaleAndRotateMirrorForWiredScreen(*displayDrawable_);
    ASSERT_NE(mirroredDisplayDrawable_->curCanvas_, nullptr);
    ASSERT_EQ(mirroredDisplayDrawable_->curCanvas_->GetTotalMatrix().Get(Drawing::Matrix::SCALE_X),
        static_cast<float>(mirroredParams->screenInfo_.width) / static_cast<float>(mainParams->screenInfo_.width));
}

/**
 * @tc.name: ScaleAndRotateMirrorForWiredScreenExtTest
 * @tc.desc: Test ScaleAndRotateMirrorForWiredScreen
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSDisplayNodeDrawableTest, ScaleAndRotateMirrorForWiredScreenExtTest, TestSize.Level1)
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
    screenManager->screens_.insert(std::make_pair(screenPtr->id_, std::move(screenPtr)));
    auto mainScreenInfo = mirroredDrawable->renderParams_->GetScreenInfo();
    mainScreenInfo.width = 1;
    displayDrawable_->ScaleAndRotateMirrorForWiredScreen(*mirroredDrawable);
    ASSERT_FALSE(screenManager->screens_.empty());

    screenManager->screens_.clear();
    auto managerPtr = std::make_unique<OHOS::Rosen::impl::RSScreen>(configs);
    managerPtr->screenRotation_ = ScreenRotation::ROTATION_270;
    screenManager->screens_.insert(std::make_pair(managerPtr->id_, std::move(managerPtr)));
    mainScreenInfo.height = 1;
    displayDrawable_->ScaleAndRotateMirrorForWiredScreen(*mirroredDrawable);
    mirroredDrawable->renderParams_ = nullptr;
    displayDrawable_->ScaleAndRotateMirrorForWiredScreen(*mirroredDrawable);
    ASSERT_FALSE(mirroredDrawable->renderParams_);
}

/**
 * @tc.name: ScaleAndRotateMirrorForWiredScreenExt004
 * @tc.desc: Test ScaleAndRotateMirrorForWiredScreen while mainHeight / mainWidth invalid
 * @tc.type: FUNC
 * @tc.require: issueIANDBE
 */
HWTEST_F(RSDisplayNodeDrawableTest, ScaleAndRotateMirrorForWiredScreenExt004, TestSize.Level2)
{
    // set drawable screenInfo
    ASSERT_NE(displayDrawable_, nullptr);
    auto mainParams = displayDrawable_->GetRenderParams() ?
        static_cast<RSDisplayRenderParams*>(displayDrawable_->GetRenderParams().get()) : nullptr;
    ASSERT_NE(mainParams, nullptr);
    mainParams->screenInfo_.width = 0;
    mainParams->screenInfo_.height = 0;

    ASSERT_NE(mirroredDrawable_, nullptr);
    auto mirroredParams = mirroredDrawable_->GetRenderParams() ?
        static_cast<RSDisplayRenderParams*>(mirroredDrawable_->GetRenderParams().get()) : nullptr;
    ASSERT_NE(mirroredParams, nullptr);
    mirroredParams->screenInfo_.width = 100;
    mirroredParams->screenInfo_.height = 100;

    // sacle mirror screen
    mirroredDisplayDrawable_->ScaleAndRotateMirrorForWiredScreen(*displayDrawable_);
    ASSERT_NE(mirroredDisplayDrawable_->curCanvas_, nullptr);
    ASSERT_EQ(mirroredDisplayDrawable_->curCanvas_->GetTotalMatrix().Get(Drawing::Matrix::SCALE_X), 1);
}

/**
 * @tc.name: ScaleAndRotateMirrorForWiredScreenExt003
 * @tc.desc: Test ScaleAndRotateMirrorForWiredScreen while scale of height smaller
 * @tc.type: FUNC
 * @tc.require: issueIANDBE
 */
HWTEST_F(RSDisplayNodeDrawableTest, ScaleAndRotateMirrorForWiredScreenExt003, TestSize.Level2)
{
    // set drawable screenInfo
    ASSERT_NE(displayDrawable_, nullptr);
    auto mainParams = displayDrawable_->GetRenderParams() ?
        static_cast<RSDisplayRenderParams*>(displayDrawable_->GetRenderParams().get()) : nullptr;
    ASSERT_NE(mainParams, nullptr);
    mainParams->screenInfo_.width = 100;
    mainParams->screenInfo_.height = 100;

    ASSERT_NE(mirroredDrawable_, nullptr);
    auto mirroredParams = mirroredDrawable_->GetRenderParams() ?
        static_cast<RSDisplayRenderParams*>(mirroredDrawable_->GetRenderParams().get()) : nullptr;
    ASSERT_NE(mirroredParams, nullptr);
    mirroredParams->screenInfo_.width = 100;
    mirroredParams->screenInfo_.height = 50;

    // sacle mirror screen
    mirroredDisplayDrawable_->ScaleAndRotateMirrorForWiredScreen(*displayDrawable_);
    ASSERT_NE(mirroredDisplayDrawable_->curCanvas_, nullptr);
    ASSERT_EQ(mirroredDisplayDrawable_->curCanvas_->GetTotalMatrix().Get(Drawing::Matrix::SCALE_X),
        static_cast<float>(mirroredParams->screenInfo_.height) / static_cast<float>(mainParams->screenInfo_.height));
}

/**
 * @tc.name: DrawHardwareEnabledNodesExt001Test
 * @tc.desc: Test DrawHardwareEnabledNodes
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSDisplayNodeDrawableTest, DrawHardwareEnabledNodesExt001Test, TestSize.Level1)
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
 * @tc.name: DrawMirrorCopyExtTest
 * @tc.desc: Test DrawMirrorCopy
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSDisplayNodeDrawableTest, DrawMirrorCopyExtTest, TestSize.Level1)
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
    ASSERT_FALSE(virtualProcesser->GetCanvas());
    uniParam->isVirtualDirtyEnabled_ = true;
    sleep(1);
}

/**
 * @tc.name: DrawHardwareEnabledNodesMissedInCacheImageExtTest
 * @tc.desc: Test DrawHardwareEnabledNodesMissedInCacheImage
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSDisplayNodeDrawableTest, DrawHardwareEnabledNodesMissedInCacheImageExtTest, TestSize.Level1)
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
 * @tc.name: DrawHardwareEnabledNodesExt002
 * @tc.desc: Test DrawHardwareEnabledNodes
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSDisplayNodeDrawableTest, DrawHardwareEnabledNodesExt002, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    Drawing::Canvas canvas;
    displayDrawable_->DrawHardwareEnabledNodes(canvas);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
}

/**
 * @tc.name: FindHardwareEnabledNodesExtTest
 * @tc.desc: Test FindHardwareEnabledNodes
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSDisplayNodeDrawableTest, FindHardwareEnabledNodesExtTest, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
    auto params = static_cast<RSDisplayRenderParams*>(displayDrawable_->renderParams_.get());
    ASSERT_NE(params, nullptr);
    displayDrawable_->FindHardwareEnabledNodes(*params);
    ASSERT_EQ(RSUniRenderThread::Instance().GetRSRenderThreadParams()->hardwareEnabledTypeDrawables_.size(), 2);
}

/**
 * @tc.name: SwitchColorFilterExtTest
 * @tc.desc: Test SwitchColorFilter
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSDisplayNodeDrawableTest, SwitchColorFilterExtTest, TestSize.Level1)
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
    ASSERT_TRUE(RSUniRenderThread::Instance().GetRenderEngine());
    RSUniRenderThread::Instance().uniRenderEngine_ = nullptr;
}

/**
 * @tc.name: CreateSurfaceExtTest
 * @tc.desc: Test CreateSurface
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSDisplayNodeDrawableTest, CreateSurfaceExtTest, TestSize.Level1)
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
 * @tc.name: FinishOffscreenRenderExtTest
 * @tc.desc: Test FinishOffscreenRender
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSDisplayNodeDrawableTest, FinishOffscreenRenderExtTest, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
    Drawing::SamplingOptions sampling;
    displayDrawable_->FinishOffscreenRender(sampling);
    ASSERT_FALSE(displayDrawable_->canvasBackup_);
}

/**
 * @tc.name: CalculateVirtualDirtyForWiredScreenExt006
 * @tc.desc: Test CalculateVirtualDirtyForWiredScreen, extraDirty is not empty
 * @tc.type: FUNC
 * @tc.require: #IA76UC
 */
HWTEST_F(RSDisplayNodeDrawableTest, CalculateVirtualDirtyForWiredScreenExt006, TestSize.Level1)
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
 * @tc.name: CalculateVirtualDirtyForWiredScreenExt005
 * @tc.desc: Test CalculateVirtualDirtyForWiredScreen, extraDirty is not empty
 * @tc.type: FUNC
 * @tc.require: #IA76UC
 */
HWTEST_F(RSDisplayNodeDrawableTest, CalculateVirtualDirtyForWiredScreenExt005, TestSize.Level1)
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
 * @tc.name: CheckAndUpdateFilterCacheOcclusionExtTest
 * @tc.desc: Test CheckAndUpdateFilterCacheOcclusion
 * @tc.type: FUNC
 * @tc.require: issueIAL2EA
 */
HWTEST_F(RSDisplayNodeDrawableTest, CheckAndUpdateFilterCacheOcclusionExtTest, TestSize.Level1)
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

    surfaceParams->isMainWindowType_ = false;
    RSDisplayRenderNodeDrawable::CheckAndUpdateFilterCacheOcclusion(*params, screenInfo);

    surfaceParams->isMainWindowType_ = true;
    RSDisplayRenderNodeDrawable::CheckAndUpdateFilterCacheOcclusion(*params, screenInfo);
}

/**
 * @tc.name: CheckDisplayNodeSkipExtTest
 * @tc.desc: Test CheckDisplayNodeSkip
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSDisplayNodeDrawableTest, CheckDisplayNodeSkipExtTest, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
    
    auto params = static_cast<RSDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(params, nullptr);
    auto processor = RSProcessorFactory::CreateProcessor(params->GetCompositeType());
    auto result = displayDrawable_->CheckDisplayNodeSkip(*params, processor);
    ASSERT_EQ(result, true);

    std::shared_ptr<RSSurfaceRenderNodeDrawable> drawable = nullptr;
    RSUifirstManager::Instance().pendingPostDrawables_.push_back(drawable);
    result = displayDrawable_->CheckDisplayNodeSkip(*params, processor);
    ASSERT_EQ(result, false);

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
    RSUifirstManager::Instance().pendingPostDrawables_.clear();
}

/**
 * @tc.name: OnDrawExtTest
 * @tc.desc: Test OnDraw
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSDisplayNodeDrawableTest, OnDrawExtTest, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    Drawing::Canvas canvas;
    displayDrawable_->OnDraw(canvas);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
}
}
