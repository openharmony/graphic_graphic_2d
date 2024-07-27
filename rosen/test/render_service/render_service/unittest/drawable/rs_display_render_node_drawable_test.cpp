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

#include "drawable/rs_display_render_node_drawable.h"
#include "params/rs_display_render_params.h"
#include "params/rs_render_thread_params.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_uni_render_thread.h"
#include "pipeline/rs_uni_render_virtual_processor.h"
#include "platform/drawing/rs_surface_converter.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace OHOS::Rosen {
constexpr int32_t DEFAULT_CANVAS_SIZE = 100;
constexpr NodeId DEFAULT_ID = 0xFFFF;

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
    if (!rtThread.renderThreadParams_) {
        rtThread.renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
    }
    NodeId id = 1;
    auto surfaceNode1 = std::make_shared<RSSurfaceRenderNode>(id);
    auto drawable1 = RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode1);
    id = 2;
    auto surfaceNode2 = std::make_shared<RSSurfaceRenderNode>(id);
    auto drawable2 = RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode2);
    surfaceNode2->InitRenderParams();
    rtThread.renderThreadParams_->hardwareEnabledTypeDrawables_.push_back(drawable1);
    rtThread.renderThreadParams_->hardwareEnabledTypeDrawables_.push_back(drawable2);
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
    if (params) {
        displayDrawable_->DrawWatermarkIfNeed(*params, *canvas_);
    }
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
    if (rtThread.renderThreadParams_) {
        rtThread.renderThreadParams_->isVirtualDirtyEnabled_ = false;
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
    if (rtThread.renderThreadParams_) {
        rtThread.renderThreadParams_->isVirtualDirtyEnabled_ = true;
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
    if (rtThread.renderThreadParams_) {
        rtThread.renderThreadParams_->isVirtualDirtyEnabled_ = true;
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
    if (rtThread.renderThreadParams_) {
        rtThread.renderThreadParams_->isVirtualDirtyEnabled_ = true;
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
    if (rtThread.renderThreadParams_) {
        rtThread.renderThreadParams_->isVirtualDirtyEnabled_ = true;
        rtThread.renderThreadParams_->isVirtualDirtyDfxEnabled_ = true;
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
    auto result = displayDrawable_->CheckDisplayNodeSkip(*params, processor);
    ASSERT_EQ(result, true);
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
    if (!rtThread.renderThreadParams_) {
        rtThread.renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
    }
    rtThread.renderThreadParams_->isVirtualDirtyEnabled_ = true;
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
    if (!rtThread.renderThreadParams_) {
        rtThread.renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
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
 * @tc.name: SkipDisplayIfScreenOff
 * @tc.desc: Test SkipDisplayIfScreenOff, corner case (node is nullptr), return false
 * @tc.type: FUNC
 * @tc.require: #I9UNQP
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, SkipDisplayIfScreenOff001, TestSize.Level1)
{
    if (!RSSystemProperties::GetSkipDisplayIfScreenOffEnabled() || !RSSystemProperties::IsPhoneType()) {
        return;
    }
    drawable_->renderNode_.reset();
    ASSERT_FALSE(displayDrawable_->SkipDisplayIfScreenOff());
}

/**
 * @tc.name: SkipDisplayIfScreenOff
 * @tc.desc: Test SkipDisplayIfScreenOff, if power off, return true
 * @tc.type: FUNC
 * @tc.require: #I9UNQP
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, SkipDisplayIfScreenOff002, TestSize.Level1)
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
    ASSERT_TRUE(displayDrawable_->SkipDisplayIfScreenOff());
    screenManagerImpl.screenPowerStatus_[screenId] = ScreenPowerStatus::POWER_STATUS_SUSPEND;
    ASSERT_TRUE(displayDrawable_->SkipDisplayIfScreenOff());
}

/**
 * @tc.name: SkipDisplayIfScreenOff
 * @tc.desc: Test SkipDisplayIfScreenOff, if power off and render one more frame, return true
 * @tc.type: FUNC
 * @tc.require: #I9UNQP
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, SkipDisplayIfScreenOff003, TestSize.Level1)
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
    ASSERT_TRUE(displayDrawable_->SkipDisplayIfScreenOff());
    screenManager->ResetPowerOffNeedProcessOneFrame();
    screenManagerImpl.screenPowerStatus_[screenId] = ScreenPowerStatus::POWER_STATUS_SUSPEND;
    ASSERT_TRUE(displayDrawable_->SkipDisplayIfScreenOff());
}
}
