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

#include <parameters.h>

#include "drawable/rs_logical_display_render_node_drawable.h"
#include "graphic_feature_param_manager.h"
#include "pipeline/rs_logical_display_render_node.h"
#include "pipeline/rs_render_node_gc.h"
#include "pipeline/render_thread/rs_render_engine.h"
#include "pipeline/render_thread/rs_uni_render_virtual_processor.h"
#include "platform/common/rs_system_properties.h"
#include "screen_manager/rs_screen.h"

#ifdef RS_PROFILER_ENABLED
#include "rs_profiler_capture_recorder.h"
#endif

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace OHOS::Rosen {
namespace {
constexpr int32_t DEFAULT_CANVAS_SIZE = 100;
constexpr NodeId DEFAULT_ID = 0xFFFF;
}
class RSLogicalDisplayRenderNodeDrawableTest : public testing::Test {
public:
    std::shared_ptr<RSLogicalDisplayRenderNode> renderNode_;
    std::shared_ptr<RSLogicalDisplayRenderNode> mirroredNode_;
    std::shared_ptr<RSScreenRenderNode> screenNode_;
    std::shared_ptr<RSScreenRenderNode> mirroredScreenNode_;

    RSLogicalDisplayRenderNodeDrawable* displayDrawable_ = nullptr;
    RSLogicalDisplayRenderNodeDrawable* mirroredDisplayDrawable_ = nullptr;
    RSScreenRenderNodeDrawable* screenDrawable_ = nullptr;
    RSScreenRenderNodeDrawable* mirroredScreenDrawable_ = nullptr;

    std::shared_ptr<Drawing::Canvas> drawingCanvas_ = nullptr;
    std::shared_ptr<RSPaintFilterCanvas> drawingFilterCanvas_ = nullptr;
    std::shared_ptr<Drawing::Canvas> drawingCanvasForMirror_ = nullptr;
    std::shared_ptr<RSPaintFilterCanvas> drawingFilterCanvasForMirror_ = nullptr;
    std::shared_ptr<Drawing::Canvas> drawingCanvasForScreen_ = nullptr;
    std::shared_ptr<Drawing::Canvas> drawingCanvasForMirrorScreen_ = nullptr;

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline NodeId id = DEFAULT_ID;
};

void RSLogicalDisplayRenderNodeDrawableTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
    auto& renderNodeGC = RSRenderNodeGC::Instance();
    renderNodeGC.nodeBucket_ = std::queue<std::vector<RSRenderNode*>>();
    renderNodeGC.drawableBucket_ = std::queue<std::vector<DrawableV2::RSRenderNodeDrawableAdapter*>>();
}

void RSLogicalDisplayRenderNodeDrawableTest::TearDownTestCase() {}

void RSLogicalDisplayRenderNodeDrawableTest::SetUp()
{
    ++id;
    RSRenderThreadParamsManager::Instance().renderThreadParams_ = std::make_unique<RSRenderThreadParams>();

    RSDisplayNodeConfig config;
    renderNode_ = std::make_shared<RSLogicalDisplayRenderNode>(DEFAULT_ID, config);
    mirroredNode_ = std::make_shared<RSLogicalDisplayRenderNode>(DEFAULT_ID + 1, config); // 1 is id offset
    auto context = std::make_shared<RSContext>();
    auto screenId = 2;
    screenNode_ = std::make_shared<RSScreenRenderNode>(DEFAULT_ID + 2, screenId, context); // 2 is id offset
    auto mirrorContext = std::make_shared<RSContext>();
    auto mirroredScreenId = 3;
    mirroredScreenNode_ =
        std::make_shared<RSScreenRenderNode>(DEFAULT_ID + 3, mirroredScreenId, mirrorContext); // 3 is id offset
    if (!renderNode_ || !mirroredNode_ || !screenNode_ || !mirroredScreenNode_) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawableTest: failed to create display node.");
        return;
    }
    renderNode_->InitRenderParams();
    mirroredNode_->InitRenderParams();
    screenNode_->InitRenderParams();
    mirroredScreenNode_->InitRenderParams();

    displayDrawable_ =
        std::static_pointer_cast<RSLogicalDisplayRenderNodeDrawable>(renderNode_->GetRenderDrawable()).get();
    mirroredDisplayDrawable_ =
        std::static_pointer_cast<RSLogicalDisplayRenderNodeDrawable>(mirroredNode_->GetRenderDrawable()).get();
    screenDrawable_ =
        std::static_pointer_cast<RSScreenRenderNodeDrawable>(screenNode_->GetRenderDrawable()).get();
    mirroredScreenDrawable_ =
        std::static_pointer_cast<RSScreenRenderNodeDrawable>(mirroredScreenNode_->GetRenderDrawable()).get();
    if (!displayDrawable_ || !mirroredDisplayDrawable_ ||
        !screenDrawable_ || !mirroredScreenDrawable_) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawableTest: failed to create drawable.");
        return;
    }

    displayDrawable_->renderParams_ = std::make_unique<RSLogicalDisplayRenderParams>(id);
    mirroredDisplayDrawable_->renderParams_ = std::make_unique<RSLogicalDisplayRenderParams>(++id);
    auto screenParams = std::make_unique<RSScreenRenderParams>(++id);
    screenParams->screenInfo_.id = screenId;
    screenDrawable_->renderParams_ = std::move(screenParams);
    auto mirroredScreenParams = std::make_unique<RSScreenRenderParams>(++id);
    mirroredScreenParams->screenInfo_.id = mirroredScreenId;
    mirroredScreenDrawable_->renderParams_ = std::move(mirroredScreenParams);
    if (!displayDrawable_->renderParams_ || !mirroredDisplayDrawable_->renderParams_ ||
        !screenDrawable_->renderParams_ || !mirroredScreenDrawable_->renderParams_) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawableTest: failed to init render params.");
        return;
    }

    auto displayParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    if (!displayParams) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawableTest: failed to init render params.");
        return;
    }
    if (!screenNode_->GetRenderDrawable() || !renderNode_->GetRenderDrawable()) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawableTest: failed to init render params.");
        return;
    }
    displayParams->SetAncestorScreenDrawable(screenNode_->GetRenderDrawable());
    displayParams->mirrorSourceDrawable_ = mirroredNode_->GetRenderDrawable();
    displayParams->screenId_ = screenNode_->GetScreenId();

    auto mirroredDisplayParams =
        static_cast<RSLogicalDisplayRenderParams*>(mirroredDisplayDrawable_->GetRenderParams().get());
    if (!mirroredDisplayParams || !mirroredScreenNode_->GetRenderDrawable()) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawableTest: failed to init render params.");
        return;
    }
    mirroredDisplayParams->SetAncestorScreenDrawable(mirroredScreenNode_->GetRenderDrawable());
    mirroredDisplayParams->screenId_ = mirroredScreenNode_->GetScreenId();

    auto tempScreenParams = static_cast<RSScreenRenderParams*>(screenDrawable_->GetRenderParams().get());
    if (!tempScreenParams || !mirroredScreenNode_->GetRenderDrawable()) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawableTest: failed to init render params.");
        return;
    }
    tempScreenParams->mirrorSourceDrawable_ = mirroredScreenNode_->GetRenderDrawable();

    // generate canvas for displayDrawable_
    drawingCanvas_ = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    if (drawingCanvas_) {
        drawingFilterCanvas_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvas_.get());
        displayDrawable_->curCanvas_ = drawingFilterCanvas_.get();
    }
    // generate canvas for mirroredDisplayDrawable_
    drawingCanvasForMirror_ = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    if (drawingCanvasForMirror_) {
        drawingFilterCanvasForMirror_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvasForMirror_.get());
        mirroredDisplayDrawable_->curCanvas_ = drawingFilterCanvasForMirror_.get();
    }
    // generate canvas for screenDrawable_
    drawingCanvasForScreen_ = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    if (drawingCanvasForScreen_) {
        screenDrawable_->curCanvas_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvasForScreen_.get());
    }
    // generate canvas for mirroredScreenDrawable_
    drawingCanvasForMirrorScreen_ = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    if (drawingCanvasForMirrorScreen_) {
        mirroredScreenDrawable_->curCanvas_ =
            std::make_shared<RSPaintFilterCanvas>(drawingCanvasForMirrorScreen_.get());
    }
}

void RSLogicalDisplayRenderNodeDrawableTest::TearDown()
{
    // clear RSRenderThreadParams after each testcase
    RSRenderThreadParamsManager::Instance().renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
}

/**
 * @tc.name: OnDraw001
 * @tc.desc: Test OnDraw When ShouldPaint is false
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, OnDrawTest001, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_FALSE(displayDrawable_->ShouldPaint());
    displayDrawable_->OnDraw(*drawingFilterCanvas_);
}

/**
 * @tc.name: OnDrawTest002
 * @tc.desc: Test OnDraw When uniParam is nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, OnDrawTest002, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->GetRenderParams(), nullptr);

    RSRenderThreadParamsManager::Instance().renderThreadParams_ = nullptr;
    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(renderParams, nullptr);
    renderParams->shouldPaint_ = true;
    renderParams->contentEmpty_ = false;
    ASSERT_TRUE(displayDrawable_->ShouldPaint());
    displayDrawable_->OnDraw(*drawingFilterCanvas_);
    ASSERT_NE(displayDrawable_->curCanvas_, nullptr);
    ASSERT_EQ(RSUniRenderThread::Instance().GetRSRenderThreadParams(), nullptr);
}

/**
 * @tc.name: OnDrawTest003
 * @tc.desc: Test OnDraw When screenParams is nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, OnDrawTest003, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->GetRenderParams(), nullptr);
    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(renderParams, nullptr);
    renderParams->shouldPaint_ = true;
    renderParams->contentEmpty_ = false;
    renderParams->SetAncestorScreenDrawable(nullptr);
    auto uniParams = std::make_unique<RSRenderThreadParams>();
    RSUniRenderThread::Instance().Sync(std::move(uniParams));
    ASSERT_NE(RSUniRenderThread::Instance().GetRSRenderThreadParams(), nullptr);
    displayDrawable_->OnDraw(*drawingFilterCanvas_);
    ASSERT_TRUE(displayDrawable_->ShouldPaint());
    ASSERT_NE(displayDrawable_->curCanvas_, nullptr);
    ASSERT_NE(RSUniRenderThread::Instance().GetRSRenderThreadParams(), nullptr);
    ASSERT_NE(displayDrawable_->GetRenderParams(), nullptr);
    ASSERT_EQ(displayDrawable_->GetScreenParams(*displayDrawable_->GetRenderParams()).second, nullptr);
}

/**
 * @tc.name: OnDrawTest004
 * @tc.desc: Test OnDraw When mirroredRenderParams is not nullptr and compositeType is UNI_RENDER_COMPOSITE or others
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, OnDrawTest004, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->GetRenderParams(), nullptr);
    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(renderParams, nullptr);
    renderParams->shouldPaint_ = true;
    renderParams->contentEmpty_ = false;
    renderParams->mirrorSourceDrawable_ = mirroredNode_->GetRenderDrawable();
    mirroredDisplayDrawable_->renderParams_ = nullptr;
    auto uniParams = std::make_unique<RSRenderThreadParams>();
    auto processor = RSProcessorFactory::CreateProcessor(renderParams->GetCompositeType());
    uniParams->SetRSProcessor(processor);
    RSUniRenderThread::Instance().Sync(std::move(uniParams));
    displayDrawable_->OnDraw(*drawingFilterCanvas_);
    ASSERT_TRUE(displayDrawable_->ShouldPaint());
    ASSERT_NE(displayDrawable_->curCanvas_, nullptr);
    ASSERT_NE(RSUniRenderThread::Instance().GetRSRenderThreadParams(), nullptr);
    ASSERT_NE(displayDrawable_->GetRenderParams(), nullptr);
    ASSERT_NE(displayDrawable_->GetScreenParams(*displayDrawable_->GetRenderParams()).second, nullptr);
    renderParams->compositeType_ = CompositeType::UNI_RENDER_COMPOSITE;
    displayDrawable_->OnDraw(*drawingFilterCanvas_);
    ASSERT_EQ(renderParams->GetCompositeType(), CompositeType::UNI_RENDER_COMPOSITE);
}

/**
 * @tc.name: OnDrawTest005
 * @tc.desc: Test OnDraw When mirroredRenderParams is nullptr and compositeType is expand or others
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, OnDrawTest005, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->GetRenderParams(), nullptr);
    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(renderParams, nullptr);
    renderParams->shouldPaint_ = true;
    renderParams->contentEmpty_ = false;
    renderParams->mirrorSourceDrawable_ = mirroredNode_->GetRenderDrawable();
    mirroredDisplayDrawable_->renderParams_ = nullptr;
    auto uniParams = std::make_unique<RSRenderThreadParams>();
    auto processor = RSProcessorFactory::CreateProcessor(renderParams->GetCompositeType());
    uniParams->SetRSProcessor(processor);
    RSUniRenderThread::Instance().Sync(std::move(uniParams));
    displayDrawable_->OnDraw(*drawingFilterCanvas_);
    ASSERT_TRUE(displayDrawable_->ShouldPaint());
    ASSERT_NE(displayDrawable_->curCanvas_, nullptr);
    ASSERT_NE(RSUniRenderThread::Instance().GetRSRenderThreadParams(), nullptr);
    ASSERT_NE(displayDrawable_->GetRenderParams(), nullptr);
    ASSERT_NE(displayDrawable_->GetScreenParams(*displayDrawable_->GetRenderParams()).second, nullptr);
    ASSERT_EQ(mirroredDisplayDrawable_->GetRenderParams(), nullptr);
    renderParams->compositeType_ = CompositeType::UNI_RENDER_EXPAND_COMPOSITE;
    displayDrawable_->OnDraw(*drawingFilterCanvas_);
    ASSERT_EQ(renderParams->GetCompositeType(), CompositeType::UNI_RENDER_EXPAND_COMPOSITE);
}

/**
 * @tc.name: OnDrawTest006
 * @tc.desc: Test OnDraw When needOffscreen is true
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, OnDrawTest006, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->GetRenderParams(), nullptr);
    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(renderParams, nullptr);
    renderParams->shouldPaint_ = true;
    renderParams->contentEmpty_ = false;
    renderParams->mirrorSourceDrawable_ = mirroredNode_->GetRenderDrawable();
    renderParams->SetNeedOffscreen(true);
    mirroredDisplayDrawable_->renderParams_ = nullptr;
    auto uniParams = std::make_unique<RSRenderThreadParams>();
    RSUniRenderThread::Instance().Sync(std::move(uniParams));
    ASSERT_TRUE(displayDrawable_->ShouldPaint());
    ASSERT_TRUE(renderParams->GetNeedOffscreen());
    displayDrawable_->OnDraw(*drawingFilterCanvas_);
    ASSERT_NE(displayDrawable_->curCanvas_, nullptr);
    ASSERT_NE(RSUniRenderThread::Instance().GetRSRenderThreadParams(), nullptr);
    ASSERT_NE(displayDrawable_->GetRenderParams(), nullptr);
    ASSERT_NE(displayDrawable_->GetScreenParams(*displayDrawable_->GetRenderParams()).second, nullptr);
    ASSERT_EQ(mirroredDisplayDrawable_->GetRenderParams(), nullptr);
    ASSERT_TRUE(renderParams->GetNeedOffscreen());
}

/**
 * @tc.name: OnDrawTest006_SKP
 * @tc.desc: Test OnDraw When needOffscreen is true (with SKP capture triggering)
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, OnDrawTest006_SKP, TestSize.Level1)
{
#ifdef RS_PROFILER_ENABLED
    RSCaptureRecorder::testingTriggering_ = true;
    RSCaptureRecorder::GetInstance().SetCaptureType(SkpCaptureType::EXTENDED);
#endif
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->GetRenderParams(), nullptr);
    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(renderParams, nullptr);
    renderParams->shouldPaint_ = true;
    renderParams->contentEmpty_ = false;
    renderParams->mirrorSourceDrawable_ = mirroredNode_->GetRenderDrawable();
    renderParams->SetNeedOffscreen(true);
    mirroredDisplayDrawable_->renderParams_ = nullptr;
    auto uniParams = std::make_unique<RSRenderThreadParams>();
    RSUniRenderThread::Instance().Sync(std::move(uniParams));
    ASSERT_TRUE(displayDrawable_->ShouldPaint());
    ASSERT_TRUE(renderParams->GetNeedOffscreen());
    displayDrawable_->OnDraw(*drawingFilterCanvas_);
    ASSERT_NE(displayDrawable_->curCanvas_, nullptr);
    ASSERT_NE(RSUniRenderThread::Instance().GetRSRenderThreadParams(), nullptr);
    ASSERT_NE(displayDrawable_->GetRenderParams(), nullptr);
    ASSERT_NE(displayDrawable_->GetScreenParams(*displayDrawable_->GetRenderParams()).second, nullptr);
    ASSERT_EQ(mirroredDisplayDrawable_->GetRenderParams(), nullptr);
    ASSERT_TRUE(renderParams->GetNeedOffscreen());
#ifdef RS_PROFILER_ENABLED
    RSCaptureRecorder::testingTriggering_ = false;
#endif
}

/**
 * @tc.name: OnDrawTest007
 * @tc.desc: Test OnDraw When canvasBackup_ is not nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, OnDrawTest007, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->GetRenderParams(), nullptr);
    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(renderParams, nullptr);
    renderParams->shouldPaint_ = true;
    renderParams->contentEmpty_ = false;
    renderParams->SetNeedOffscreen(true);
    mirroredDisplayDrawable_->renderParams_ = nullptr;
    auto uniParams = std::make_unique<RSRenderThreadParams>();
    RSUniRenderThread::Instance().Sync(std::move(uniParams));
    auto tempCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    if (tempCanvas) {
        auto tempFilterCanvas = std::make_shared<RSPaintFilterCanvas>(tempCanvas.get());
        displayDrawable_->canvasBackup_ = tempFilterCanvas.get();
    }
    ASSERT_NE(displayDrawable_->canvasBackup_, nullptr);
    displayDrawable_->OnDraw(*drawingFilterCanvas_);
    ASSERT_TRUE(displayDrawable_->ShouldPaint());
    ASSERT_NE(displayDrawable_->curCanvas_, nullptr);
    ASSERT_NE(RSUniRenderThread::Instance().GetRSRenderThreadParams(), nullptr);
    ASSERT_NE(displayDrawable_->GetRenderParams(), nullptr);
    ASSERT_NE(displayDrawable_->GetScreenParams(*displayDrawable_->GetRenderParams()).second, nullptr);
    ASSERT_EQ(mirroredDisplayDrawable_->GetRenderParams(), nullptr);
    ASSERT_TRUE(renderParams->GetNeedOffscreen());
}

/**
 * @tc.name: OnDrawTest008
 * @tc.desc: Test OnDraw When GetRotateOffScreenScreenNodeEnable is true
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, OnDrawTest008, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->GetRenderParams(), nullptr);
    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(renderParams, nullptr);
    renderParams->shouldPaint_ = true;
    renderParams->contentEmpty_ = false;
    renderParams->mirrorSourceDrawable_ = mirroredNode_->GetRenderDrawable();
    mirroredDisplayDrawable_->renderParams_ = nullptr;
    auto uniParams = std::make_unique<RSRenderThreadParams>();
    RSUniRenderThread::Instance().Sync(std::move(uniParams));
    RotateOffScreenParam::SetRotateOffScreenDisplayNodeEnable(true);
    displayDrawable_->OnDraw(*drawingFilterCanvas_);
    ASSERT_TRUE(displayDrawable_->ShouldPaint());
    ASSERT_NE(displayDrawable_->curCanvas_, nullptr);
    ASSERT_NE(RSUniRenderThread::Instance().GetRSRenderThreadParams(), nullptr);
    ASSERT_NE(displayDrawable_->GetRenderParams(), nullptr);
    ASSERT_NE(displayDrawable_->GetScreenParams(*displayDrawable_->GetRenderParams()).second, nullptr);
    ASSERT_EQ(mirroredDisplayDrawable_->GetRenderParams(), nullptr);
    ASSERT_TRUE(RotateOffScreenParam::GetRotateOffScreenScreenNodeEnable());
}

/**
 * @tc.name: OnDrawTest009
 * @tc.desc: Test OnDraw When hdr is true
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, OnDrawTest009, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->GetRenderParams(), nullptr);
    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(renderParams, nullptr);
    renderParams->shouldPaint_ = true;
    renderParams->contentEmpty_ = false;
    renderParams->mirrorSourceDrawable_.reset();
    auto uniParams = std::make_unique<RSRenderThreadParams>();
    RSUniRenderThread::Instance().Sync(std::move(uniParams));
    auto screenParams = displayDrawable_->GetScreenParams(*displayDrawable_->GetRenderParams()).second;
    ASSERT_NE(screenParams, nullptr);
    screenParams->SetHDRPresent(true);
    displayDrawable_->OnDraw(*drawingFilterCanvas_);
    ASSERT_TRUE(displayDrawable_->ShouldPaint());
    ASSERT_NE(displayDrawable_->GetRenderParams(), nullptr);
    ASSERT_NE(displayDrawable_->GetScreenParams(*displayDrawable_->GetRenderParams()).second, nullptr);
}

/**
 * @tc.name: OnCaptureTest001
 * @tc.desc: Test OnCapture When ShouldPaint is false
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, OnCaptureTest001, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_FALSE(displayDrawable_->ShouldPaint());
    displayDrawable_->OnCapture(*drawingFilterCanvas_);
}

/**
 * @tc.name: OnCaptureTest002
 * @tc.desc: Test OnCapture When screenDrawable is nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, OnCaptureTest002, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->GetRenderParams(), nullptr);
    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(renderParams, nullptr);
    renderParams->shouldPaint_ = true;
    renderParams->contentEmpty_ = false;
    renderParams->SetAncestorScreenDrawable(nullptr);
    displayDrawable_->OnCapture(*drawingFilterCanvas_);
    ASSERT_TRUE(displayDrawable_->ShouldPaint());
    ASSERT_NE(displayDrawable_->GetRenderParams(), nullptr);
}

/**
 * @tc.name: OnCaptureTest003
 * @tc.desc: Test OnCapture When screenParam is nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, OnCaptureTest003, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->GetRenderParams(), nullptr);
    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    renderParams->shouldPaint_ = true;
    renderParams->contentEmpty_ = false;
    ASSERT_NE(screenDrawable_, nullptr);
    screenDrawable_->renderParams_ = nullptr;
    displayDrawable_->OnCapture(*drawingFilterCanvas_);
    ASSERT_TRUE(displayDrawable_->ShouldPaint());
    ASSERT_NE(displayDrawable_->GetRenderParams(), nullptr);
    ASSERT_EQ(screenDrawable_->GetRenderParams(), nullptr);
}

/**
 * @tc.name: OnCaptureTest004
 * @tc.desc: Test OnCapture When noBuffer is not nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, OnCaptureTest004, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->GetRenderParams(), nullptr);
    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(renderParams, nullptr);
    renderParams->shouldPaint_ = true;
    renderParams->contentEmpty_ = false;
    RSUniRenderThread::GetCaptureParam().isSnapshot_ = true;
    auto surfaceHandler = screenDrawable_->GetRSSurfaceHandlerOnDraw();
    sptr<SurfaceBuffer> buffer = SurfaceBuffer::Create();
    surfaceHandler->SetBuffer(buffer, SyncFence::InvalidFence(), {}, 0);
    displayDrawable_->OnCapture(*drawingFilterCanvas_);
    ASSERT_TRUE(displayDrawable_->ShouldPaint());
    ASSERT_NE(displayDrawable_->GetRenderParams(), nullptr);
    ASSERT_NE(screenDrawable_->GetRSSurfaceHandlerOnDraw()->GetBuffer(), nullptr);
}

/**
 * @tc.name: OnCaptureTest005
 * @tc.desc: Test OnCapture when specialLayerType is not NO_SPECIAL_LAYER
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, OnCaptureTest005, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->GetRenderParams(), nullptr);
    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(renderParams, nullptr);
    renderParams->shouldPaint_ = true;
    renderParams->contentEmpty_ = false;
    auto screenParams = displayDrawable_->GetScreenParams(*displayDrawable_->GetRenderParams()).second;
    ASSERT_NE(screenParams, nullptr);
    screenParams->SetHDRPresent(true);
    displayDrawable_->OnCapture(*drawingFilterCanvas_);
    EXPECT_TRUE(displayDrawable_->ShouldPaint());
    EXPECT_NE(displayDrawable_->GetRenderParams(), nullptr);
    EXPECT_NE(displayDrawable_->GetSpecialLayerType(*renderParams), NO_SPECIAL_LAYER);
}

/**
 * @tc.name: OnCaptureTest006
 * @tc.desc: Test OnCapture when isSamplingOn is true
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, OnCaptureTest006, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->GetRenderParams(), nullptr);
    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(renderParams, nullptr);
    renderParams->shouldPaint_ = true;
    renderParams->contentEmpty_ = false;
    auto screenParams =
        static_cast<RSScreenRenderParams*>(displayDrawable_->GetScreenParams(*renderParams).second);
    ASSERT_NE(screenParams, nullptr);
    auto screenInfo = screenParams->GetScreenInfo();
    screenInfo.isSamplingOn = true;
    screenParams->screenInfo_ = screenInfo;
    displayDrawable_->OnCapture(*drawingFilterCanvas_);
    EXPECT_TRUE(displayDrawable_->ShouldPaint());
    EXPECT_NE(displayDrawable_->GetRenderParams(), nullptr);
    EXPECT_TRUE(screenParams->GetScreenInfo().isSamplingOn);
}

/**
 * @tc.name: OnCaptureTest007
 * @tc.desc: Test OnCapture when isMirror_ is true
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, OnCaptureTest007, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->GetRenderParams(), nullptr);
    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(renderParams, nullptr);
    renderParams->shouldPaint_ = true;
    renderParams->contentEmpty_ = false;
    RSUniRenderThread::GetCaptureParam().isMirror_ = true;
    displayDrawable_->OnCapture(*drawingFilterCanvas_);
    EXPECT_TRUE(displayDrawable_->ShouldPaint());
    EXPECT_NE(displayDrawable_->GetRenderParams(), nullptr);
    EXPECT_TRUE(RSUniRenderThread::GetCaptureParam().isMirror_);
}

/**
 * @tc.name: OnCaptureTest008
 * @tc.desc: Test OnCapture when IsRenderSkipIfScreenOff is true
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, OnCaptureTest008, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->GetRenderParams(), nullptr);
    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(renderParams, nullptr);
    renderParams->shouldPaint_ = true;
    renderParams->contentEmpty_ = false;
    screenDrawable_->isRenderSkipIfScreenOff_ = true;
    displayDrawable_->OnCapture(*drawingFilterCanvas_);
    EXPECT_TRUE(displayDrawable_->ShouldPaint());
    EXPECT_NE(displayDrawable_->GetRenderParams(), nullptr);
    EXPECT_TRUE(screenDrawable_->IsRenderSkipIfScreenOff());
}

/**
 * @tc.name: OnCaptureTest009
 * @tc.desc: Test OnCapture when blackList is not empty
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, OnCaptureTest009, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->GetRenderParams(), nullptr);
    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(renderParams, nullptr);
    renderParams->shouldPaint_ = true;
    renderParams->contentEmpty_ = false;
    std::unordered_set<NodeId> blackList {0};
    RSUniRenderThread::Instance().SetBlackList(blackList);
    displayDrawable_->OnCapture(*drawingFilterCanvas_);
    EXPECT_TRUE(displayDrawable_->ShouldPaint());
    EXPECT_NE(displayDrawable_->GetRenderParams(), nullptr);
    EXPECT_FALSE(RSUniRenderThread::Instance().GetBlackList().empty());
}

/**
 * @tc.name: DrawExpandDisplayTest001
 * @tc.desc: Test DrawExpandDisplay
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawExpandDisplayTest001, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->GetRenderParams(), nullptr);
    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(renderParams, nullptr);
    renderParams->SetAncestorScreenDrawable(nullptr);
    displayDrawable_->DrawExpandDisplay(*renderParams);
    EXPECT_NE(displayDrawable_->GetRenderParams(), nullptr);
}

/**
 * @tc.name: DrawExpandDisplayTest002
 * @tc.desc: Test DrawExpandDisplay
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawExpandDisplayTest002, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->GetRenderParams(), nullptr);
    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(renderParams, nullptr);
    auto screenDrawable = screenNode_->GetRenderDrawable();
    ASSERT_NE(screenDrawable, nullptr);
    renderParams->SetAncestorScreenDrawable(screenDrawable);
    auto screenParams = static_cast<RSScreenRenderParams*>(screenDrawable->GetRenderParams().get());
    ASSERT_NE(screenParams, nullptr);
    screenParams->hasHdrPresent_ = true;
    displayDrawable_->DrawExpandDisplay(*renderParams);
    EXPECT_NE(displayDrawable_->GetRenderParams(), nullptr);
}

/**
 * @tc.name: CalculateVirtualDirtyTest001
 * @tc.desc: Test CalculateVirtualDirty when isEqualVsyncPeriod is false
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, CalculateVirtualDirtyTest001, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
    ASSERT_NE(screenDrawable_, nullptr);

    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(renderParams, nullptr);
    auto uniParams = std::make_unique<RSRenderThreadParams>();
    RSUniRenderThread::Instance().Sync(std::move(uniParams));
    auto screenParams = static_cast<RSScreenRenderParams*>(screenDrawable_->GetRenderParams().get());
    ASSERT_NE(screenParams, nullptr);
    auto screenInfo = screenParams->GetScreenInfo();
    screenInfo.isEqualVsyncPeriod = false;
    screenParams->screenInfo_ = screenInfo;
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    Drawing::Matrix matrix;
    auto ret = displayDrawable_->CalculateVirtualDirty(virtualProcesser, *screenDrawable_, *renderParams, matrix);
    ASSERT_TRUE(ret.empty());
}

/**
 * @tc.name: CalculateVirtualDirtyTest002
 * @tc.desc: Test CalculateVirtualDirty when GetForceMirrorScreenDirty is false
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, CalculateVirtualDirtyTest002, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
    ASSERT_NE(screenDrawable_, nullptr);

    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(renderParams, nullptr);
    auto uniParams = std::make_unique<RSRenderThreadParams>();
    uniParams->SetForceMirrorScreenDirty(true);
    EXPECT_TRUE(uniParams->GetForceMirrorScreenDirty());
    RSUniRenderThread::Instance().Sync(std::move(uniParams));
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    Drawing::Matrix matrix;
    displayDrawable_->CalculateVirtualDirty(virtualProcesser, *screenDrawable_, *renderParams, matrix);
}

/**
 * @tc.name: CalculateVirtualDirtyTest003
 * @tc.desc: Test CalculateVirtualDirty when IsSpecialLayerChanged is false
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, CalculateVirtualDirtyTest003, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
    ASSERT_NE(screenDrawable_, nullptr);

    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(renderParams, nullptr);
    renderParams->displaySpecialSurfaceChanged_ = true;
    auto uniParams = std::make_unique<RSRenderThreadParams>();
    RSUniRenderThread::Instance().Sync(std::move(uniParams));
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    Drawing::Matrix matrix;
    displayDrawable_->CalculateVirtualDirty(virtualProcesser, *screenDrawable_, *renderParams, matrix);
    EXPECT_TRUE(renderParams->IsSpecialLayerChanged());
}

/**
 * @tc.name: CalculateVirtualDirtyTest004
 * @tc.desc: Test CalculateVirtualDirty when GetVirtualDirtyRefresh is true
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, CalculateVirtualDirtyTest004, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
    ASSERT_NE(screenDrawable_, nullptr);

    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(renderParams, nullptr);
    auto uniParams = std::make_unique<RSRenderThreadParams>();
    uniParams->SetVirtualDirtyRefresh(true);
    EXPECT_TRUE(uniParams->virtualDirtyRefresh_);
    RSUniRenderThread::Instance().Sync(std::move(uniParams));
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    Drawing::Matrix matrix;
    displayDrawable_->CalculateVirtualDirty(virtualProcesser, *screenDrawable_, *renderParams, matrix);
}

/**
 * @tc.name: CalculateVirtualDirtyTest005
 * @tc.desc: Test CalculateVirtualDirty when enableVisibleRect_ is true
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, CalculateVirtualDirtyTest005, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
    ASSERT_NE(screenDrawable_, nullptr);

    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(renderParams, nullptr);
    auto uniParams = std::make_unique<RSRenderThreadParams>();
    RSUniRenderThread::Instance().Sync(std::move(uniParams));
    displayDrawable_->enableVisibleRect_ = true;
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    Drawing::Matrix matrix;
    displayDrawable_->CalculateVirtualDirty(virtualProcesser, *screenDrawable_, *renderParams, matrix);

    renderParams->hasSecLayerInVisibleRectChanged_ = true;
    displayDrawable_->CalculateVirtualDirty(virtualProcesser, *screenDrawable_, *renderParams, matrix);
    EXPECT_TRUE(renderParams->HasSecLayerInVisibleRectChanged());
}

/**
 * @tc.name: CalculateVirtualDirtyTest006
 * @tc.desc: Test CalculateVirtualDirty when hwcRect is not empty
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, CalculateVirtualDirtyTest006, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
    ASSERT_NE(screenDrawable_, nullptr);
    ASSERT_NE(mirroredScreenDrawable_, nullptr);

    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(renderParams, nullptr);
    auto uniParams = std::make_unique<RSRenderThreadParams>();
    RSUniRenderThread::Instance().Sync(std::move(uniParams));
    mirroredScreenDrawable_->syncDirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    mirroredScreenDrawable_->syncDirtyManager_->hwcDirtyRegion_ = {1, 1, 1, 1};
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    Drawing::Matrix matrix;
    displayDrawable_->CalculateVirtualDirty(virtualProcesser, *screenDrawable_, *renderParams, matrix);
}

/**
 * @tc.name: CalculateVirtualDirtyTest007
 * @tc.desc: Test CalculateVirtualDirty when extraDirty is not empty
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, CalculateVirtualDirtyTest007, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
    ASSERT_NE(screenDrawable_, nullptr);

    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(renderParams, nullptr);
    auto uniParams = std::make_unique<RSRenderThreadParams>();
    RSUniRenderThread::Instance().Sync(std::move(uniParams));
    screenDrawable_->syncDirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    screenDrawable_->syncDirtyManager_->dirtyRegion_ = {1, 1, 1, 1};
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    Drawing::Matrix matrix;
    displayDrawable_->CalculateVirtualDirty(virtualProcesser, *screenDrawable_, *renderParams, matrix);
}

/**
 * @tc.name: CalculateVirtualDirtyTest008
 * @tc.desc: Test CalculateVirtualDirty when IsVirtualDirtyDfxEnabled is true
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, CalculateVirtualDirtyTest008, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
    ASSERT_NE(screenDrawable_, nullptr);

    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(renderParams, nullptr);
    auto uniParams = std::make_unique<RSRenderThreadParams>();
    uniParams->isVirtualDirtyDfxEnabled_ = true;
    EXPECT_TRUE(uniParams->IsVirtualDirtyDfxEnabled());
    RSUniRenderThread::Instance().Sync(std::move(uniParams));
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    Drawing::Matrix matrix;
    displayDrawable_->CalculateVirtualDirty(virtualProcesser, *screenDrawable_, *renderParams, matrix);
}

/**
 * @tc.name: CalculateVirtualDirtyTest009
 * @tc.desc: Test CalculateVirtualDirty when IsVirtualDirtyDfxEnabled is true
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, CalculateVirtualDirtyTest009, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
    ASSERT_NE(screenDrawable_, nullptr);

    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(renderParams, nullptr);

    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    Drawing::Matrix matrix;
    auto param = static_cast<RSScreenRenderParams*>(screenDrawable_->GetRenderParams().get());
    param->mirrorSourceDrawable_.reset();
    displayDrawable_->CalculateVirtualDirty(virtualProcesser, *screenDrawable_, *renderParams, matrix);
}

/**
 * @tc.name: CalculateVirtualDirtyForWiredScreenTest001
 * @tc.desc: Test CalculateVirtualDirtyForWiredScreen, without mirrorNode
 * @tc.type: FUNC
 * @tc.require: #IA76UC
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, CalculateVirtualDirtyForWiredScreenTest001, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(screenDrawable_, nullptr);

    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(renderParams, nullptr);
    Drawing::Matrix canvasMatrix;
    auto damageRects = displayDrawable_->CalculateVirtualDirtyForWiredScreen(*screenDrawable_, canvasMatrix);
    ASSERT_EQ(damageRects.size(), 0);

    renderParams->mirrorSourceDrawable_.reset();
    damageRects = displayDrawable_->CalculateVirtualDirtyForWiredScreen(*screenDrawable_, canvasMatrix);
    ASSERT_EQ(damageRects.size(), 0);
}

/**
 * @tc.name: CalculateVirtualDirtyForWiredScreenTest002
 * @tc.desc: Test CalculateVirtualDirtyForWiredScreen, isVirtualDirtyEnabled_ false
 * @tc.type: FUNC
 * @tc.require: #IA76UC
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, CalculateVirtualDirtyForWiredScreenTest002, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(mirroredNode_, nullptr);
    ASSERT_NE(screenDrawable_, nullptr);

    Drawing::Matrix canvasMatrix;
    RSRenderThreadParamsManager::Instance().renderThreadParams_->isVirtualDirtyEnabled_ = true;
    auto& rtThread = RSUniRenderThread::Instance();
    if (rtThread.GetRSRenderThreadParams()) {
        rtThread.GetRSRenderThreadParams()->isVirtualDirtyEnabled_ = false;
    }
    auto damageRects = displayDrawable_->CalculateVirtualDirtyForWiredScreen(*screenDrawable_, canvasMatrix);
    ASSERT_EQ(damageRects.size(), 0);
}

/**
 * @tc.name: CalculateVirtualDirtyForWiredScreenTest003
 * @tc.desc: Test CalculateVirtualDirtyForWiredScreen, without syncDirtyManager
 * @tc.type: FUNC
 * @tc.require: #IA76UC
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, CalculateVirtualDirtyForWiredScreenTest003, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(mirroredNode_, nullptr);
    ASSERT_NE(screenDrawable_, nullptr);

    Drawing::Matrix canvasMatrix;
    RSRenderThreadParamsManager::Instance().renderThreadParams_->isVirtualDirtyEnabled_ = true;
    auto& rtThread = RSUniRenderThread::Instance();
    if (rtThread.GetRSRenderThreadParams()) {
        rtThread.GetRSRenderThreadParams()->isVirtualDirtyEnabled_ = true;
    }
    auto damageRects = displayDrawable_->CalculateVirtualDirtyForWiredScreen(*screenDrawable_, canvasMatrix);
    ASSERT_EQ(damageRects.size(), 0);
}

/**
 * @tc.name: CalculateVirtualDirtyForWiredScreenTest004
 * @tc.desc: Test CalculateVirtualDirtyForWiredScreen, canvasMatrix not equals to lastMatrix_
 * @tc.type: FUNC
 * @tc.require: #IA76UC
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, CalculateVirtualDirtyForWiredScreenTest004, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(mirroredNode_, nullptr);
    ASSERT_NE(screenDrawable_, nullptr);

    Drawing::Matrix canvasMatrix;
    RSRenderThreadParamsManager::Instance().renderThreadParams_->isVirtualDirtyEnabled_ = true;
    auto& rtThread = RSUniRenderThread::Instance();
    if (rtThread.GetRSRenderThreadParams()) {
        rtThread.GetRSRenderThreadParams()->isVirtualDirtyEnabled_ = true;
    }
    screenDrawable_->syncDirtyManager_ = std::make_shared<RSDirtyRegionManager>(false);
    const Drawing::scalar scale = 100.0f;
    canvasMatrix.SetScale(scale, scale);
    auto damageRects = displayDrawable_->CalculateVirtualDirtyForWiredScreen(*screenDrawable_, canvasMatrix);
    ASSERT_EQ(damageRects.size(), 0);
}

/**
 * @tc.name: CalculateVirtualDirtyForWiredScreenTest005
 * @tc.desc: Test CalculateVirtualDirtyForWiredScreen, extraDirty is not empty
 * @tc.type: FUNC
 * @tc.require: #IA76UC
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, CalculateVirtualDirtyForWiredScreenTest005, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(mirroredNode_, nullptr);
    ASSERT_NE(screenDrawable_, nullptr);

    Drawing::Matrix canvasMatrix;
    RSRenderThreadParamsManager::Instance().renderThreadParams_->isVirtualDirtyEnabled_ = true;
    auto& rtThread = RSUniRenderThread::Instance();
    if (rtThread.GetRSRenderThreadParams()) {
        rtThread.GetRSRenderThreadParams()->isVirtualDirtyEnabled_ = true;
    }
    screenDrawable_->syncDirtyManager_ = std::make_shared<RSDirtyRegionManager>(false);
    screenDrawable_->syncDirtyManager_->dirtyRegion_ = RectI(0, 0, DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    auto damageRects = displayDrawable_->CalculateVirtualDirtyForWiredScreen(*screenDrawable_, canvasMatrix);
    ASSERT_EQ(damageRects.size(), 0);
}

/**
 * @tc.name: CalculateVirtualDirtyForWiredScreenTest006
 * @tc.desc: Test CalculateVirtualDirtyForWiredScreen, extraDirty is not empty
 * @tc.type: FUNC
 * @tc.require: #IA76UC
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, CalculateVirtualDirtyForWiredScreenTest006, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(mirroredNode_, nullptr);
    ASSERT_NE(screenDrawable_, nullptr);

    Drawing::Matrix canvasMatrix;
    RSRenderThreadParamsManager::Instance().renderThreadParams_->isVirtualDirtyEnabled_ = true;
    auto& rtThread = RSUniRenderThread::Instance();
    if (rtThread.GetRSRenderThreadParams()) {
        rtThread.GetRSRenderThreadParams()->isVirtualDirtyEnabled_ = true;
        rtThread.GetRSRenderThreadParams()->isVirtualDirtyDfxEnabled_ = true;
    }
    screenDrawable_->syncDirtyManager_ = std::make_shared<RSDirtyRegionManager>(false);
    auto damageRects = displayDrawable_->CalculateVirtualDirtyForWiredScreen(*screenDrawable_, canvasMatrix);
    ASSERT_EQ(damageRects.size(), 0);
}

/**
 * @tc.name: CalculateVirtualDirtyForWiredScreenTest007
 * @tc.desc: Test CalculateVirtualDirtyForWiredScreen, extraDirty is not empty
 * @tc.type: FUNC
 * @tc.require: #IA76UC
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, CalculateVirtualDirtyForWiredScreenTest007, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(screenDrawable_, nullptr);

    displayDrawable_->enableVisibleRect_ = false;
    Drawing::Matrix canvasMatrix;
    auto temp = std::make_unique<RSRenderThreadParams>();
    temp->isVirtualDirtyEnabled_ = true;
    RSUniRenderThread::Instance().Sync(std::move(temp));

    screenDrawable_->syncDirtyManager_ = std::make_shared<RSDirtyRegionManager>(false);
    auto screenParam = static_cast<RSScreenRenderParams*>(screenDrawable_->GetRenderParams().get());
    screenParam->screenInfo_.isEqualVsyncPeriod = true;
    displayDrawable_->virtualDirtyNeedRefresh_ = false;
    auto damageRects = displayDrawable_->CalculateVirtualDirtyForWiredScreen(*screenDrawable_, canvasMatrix);

    displayDrawable_->virtualDirtyNeedRefresh_ = true;
    damageRects = displayDrawable_->CalculateVirtualDirtyForWiredScreen(*screenDrawable_, canvasMatrix);
    EXPECT_EQ(damageRects.size(), 0);
}

/**
 * @tc.name: SetSecurityMaskTest001
 * @tc.desc: Test SetSecurityMask when image is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBCH1W
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, SetSecurityMaskTest001, TestSize.Level2)
{
    ASSERT_NE(displayDrawable_, nullptr);
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    displayDrawable_->SetSecurityMask(*virtualProcesser);
}

/**
 * @tc.name: SetSecurityMaskTest002
 * @tc.desc: Test SetSecurityMask when image is not nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBCH1W
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, SetSecurityMaskTest002, TestSize.Level2)
{
    ASSERT_NE(displayDrawable_, nullptr);
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(screenManager, nullptr);
    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(renderParams, nullptr);
    // when image width is 1, height is 0
    Media::InitializationOptions opts1;
    opts1.size.width = 1;
    opts1.size.height = 0;
    std::shared_ptr<Media::PixelMap> securityMask1 = Media::PixelMap::Create(opts1);
    screenManager->SetScreenSecurityMask(renderParams->GetScreenId(), securityMask1);
    displayDrawable_->SetSecurityMask(*virtualProcesser);

    // when image width is 0, height is 1
    Media::InitializationOptions opts2;
    opts2.size.width = 0;
    opts2.size.height = 1;
    std::shared_ptr<Media::PixelMap> securityMask2 = Media::PixelMap::Create(opts2);
    screenManager->SetScreenSecurityMask(renderParams->GetScreenId(), securityMask2);
    displayDrawable_->SetSecurityMask(*virtualProcesser);

    // when image width is 1, height is 1
    Media::InitializationOptions opts3;
    opts3.size.width = 1;
    opts3.size.height = 1;
    std::shared_ptr<Media::PixelMap> securityMask3 = Media::PixelMap::Create(opts3);
    screenManager->SetScreenSecurityMask(renderParams->GetScreenId(), securityMask3);
    displayDrawable_->SetSecurityMask(*virtualProcesser);
}

/**
 * @tc.name: ScaleAndRotateMirrorForWiredScreen
 * @tc.desc: Test ScaleAndRotateMirrorForWiredScreen
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, ScaleAndRotateMirrorForWiredScreen, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(mirroredDisplayDrawable_, nullptr);

    mirroredDisplayDrawable_->renderParams_ = nullptr;
    displayDrawable_->ScaleAndRotateMirrorForWiredScreen(*mirroredDisplayDrawable_);
    ASSERT_FALSE(mirroredDisplayDrawable_->GetRenderParams());

    mirroredDisplayDrawable_->renderParams_ = std::make_unique<RSLogicalDisplayRenderParams>(id);
    displayDrawable_->ScaleAndRotateMirrorForWiredScreen(*mirroredDisplayDrawable_);
    ASSERT_TRUE(mirroredDisplayDrawable_->GetRenderParams());

    screenDrawable_->renderParams_ = nullptr;
    displayDrawable_->ScaleAndRotateMirrorForWiredScreen(*mirroredDisplayDrawable_);
    ASSERT_FALSE(screenDrawable_->GetRenderParams());

    ASSERT_TRUE(displayDrawable_->GetRenderParams());

    auto screenManagerPtr = impl::RSScreenManager::GetInstance();
    auto* screenManager = static_cast<impl::RSScreenManager*>(screenManagerPtr.GetRefPtr());
    VirtualScreenConfigs configs;
    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(renderParams, nullptr);
    ScreenId screenId = renderParams->GetScreenId();
    auto screen = std::make_unique<OHOS::Rosen::impl::RSScreen>(configs);
    screenManager->screens_.insert(std::make_pair(screenId, std::move(screen)));
    displayDrawable_->ScaleAndRotateMirrorForWiredScreen(*mirroredDisplayDrawable_);
    ASSERT_FALSE(screenManager->screens_.empty());

    screenManager->screens_.clear();
    auto screenPtr = std::make_unique<OHOS::Rosen::impl::RSScreen>(configs);
    screenPtr->screenRotation_ = ScreenRotation::ROTATION_90;
    screenManager->screens_.insert(std::make_pair(screenPtr->id_, std::move(screenPtr)));

    auto mirroredParams = mirroredDisplayDrawable_->GetRenderParams() ?
        static_cast<RSLogicalDisplayRenderParams*>(mirroredDisplayDrawable_->GetRenderParams().get()) : nullptr;
    mirroredParams->SetAncestorScreenDrawable(mirroredScreenNode_->GetRenderDrawable());
    auto [_, screenParams] = mirroredDisplayDrawable_->GetScreenParams(*mirroredParams);
    ASSERT_NE(screenParams, nullptr);
    auto mainScreenInfo = screenParams->GetScreenInfo();
    mainScreenInfo.width = 1;
    displayDrawable_->ScaleAndRotateMirrorForWiredScreen(*mirroredDisplayDrawable_);
    ASSERT_FALSE(screenManager->screens_.empty());

    screenManager->screens_.clear();
    auto managerPtr = std::make_unique<OHOS::Rosen::impl::RSScreen>(configs);
    managerPtr->screenRotation_ = ScreenRotation::ROTATION_270;
    screenManager->screens_.insert(std::make_pair(managerPtr->id_, std::move(managerPtr)));
    mainScreenInfo.height = 1;
    displayDrawable_->ScaleAndRotateMirrorForWiredScreen(*mirroredDisplayDrawable_);
    mirroredDisplayDrawable_->renderParams_ = nullptr;
    displayDrawable_->ScaleAndRotateMirrorForWiredScreen(*mirroredDisplayDrawable_);
    ASSERT_FALSE(mirroredDisplayDrawable_->renderParams_);
}

/**
 * @tc.name: ScaleAndRotateMirrorForWiredScreen002
 * @tc.desc: Test ScaleAndRotateMirrorForWiredScreen while scale of width smaller
 * @tc.type: FUNC
 * @tc.require: issueIANDBE
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, ScaleAndRotateMirrorForWiredScreen002, TestSize.Level2)
{
    // set drawable screenInfo
    ASSERT_NE(displayDrawable_, nullptr);
    auto mainParams = displayDrawable_->GetRenderParams() ?
        static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get()) : nullptr;
    ASSERT_NE(mainParams, nullptr);
    mainParams->SetBoundsRect({0, 0, 100, 100});

    ASSERT_NE(mirroredDisplayDrawable_, nullptr);
    ASSERT_NE(mirroredDisplayDrawable_->GetRenderParams(), nullptr);
    auto [_, mirroredScreenParams] =
        mirroredDisplayDrawable_->GetScreenParams(*mirroredDisplayDrawable_->GetRenderParams());
    ASSERT_NE(mirroredScreenParams, nullptr);
    mirroredScreenParams->SetBoundsRect({0, 0, 50, 100});
    // scale mirror screen
    mirroredDisplayDrawable_->ScaleAndRotateMirrorForWiredScreen(*displayDrawable_);
    ASSERT_NE(mirroredDisplayDrawable_->curCanvas_, nullptr);
    ASSERT_EQ(mirroredDisplayDrawable_->curCanvas_->GetTotalMatrix().Get(Drawing::Matrix::SCALE_X),
        static_cast<float>(mirroredScreenParams->GetBounds().GetWidth()) /
        static_cast<float>(mainParams->GetBounds().GetWidth()));
}

/**
 * @tc.name: ScaleAndRotateMirrorForWiredScreen003
 * @tc.desc: Test ScaleAndRotateMirrorForWiredScreen while scale of height smaller
 * @tc.type: FUNC
 * @tc.require: issueIANDBE
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, ScaleAndRotateMirrorForWiredScreen003, TestSize.Level2)
{
    // set drawable screenInfo
    ASSERT_NE(displayDrawable_, nullptr);
    auto mainParams = displayDrawable_->GetRenderParams() ?
        static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get()) : nullptr;
    ASSERT_NE(mainParams, nullptr);
    mainParams->SetBoundsRect({0, 0, 100, 100});

    ASSERT_NE(mirroredDisplayDrawable_, nullptr);
    ASSERT_NE(mirroredDisplayDrawable_->GetRenderParams(), nullptr);
    auto [_, mirroredScreenParams] =
        mirroredDisplayDrawable_->GetScreenParams(*mirroredDisplayDrawable_->GetRenderParams());
    ASSERT_NE(mirroredScreenParams, nullptr);
    mirroredScreenParams->SetBoundsRect({0, 0, 100, 50});

    // scale mirror screen
    mirroredDisplayDrawable_->ScaleAndRotateMirrorForWiredScreen(*displayDrawable_);
    ASSERT_NE(mirroredDisplayDrawable_->curCanvas_, nullptr);
    ASSERT_EQ(mirroredDisplayDrawable_->curCanvas_->GetTotalMatrix().Get(Drawing::Matrix::SCALE_X),
        static_cast<float>(mirroredScreenParams->GetBounds().GetHeight()) /
        static_cast<float>(mainParams->GetBounds().GetHeight()));
}

/**
 * @tc.name: ScaleAndRotateMirrorForWiredScreen004
 * @tc.desc: Test ScaleAndRotateMirrorForWiredScreen while mainHeight / mainWidth invalid
 * @tc.type: FUNC
 * @tc.require: issueIANDBE
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, ScaleAndRotateMirrorForWiredScreen004, TestSize.Level2)
{
    // set drawable screenInfo
    ASSERT_NE(displayDrawable_, nullptr);
    auto mainParams = displayDrawable_->GetRenderParams() ?
        static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get()) : nullptr;
    ASSERT_NE(mainParams, nullptr);
    mainParams->SetBoundsRect({0, 0, 0, 0});

    ASSERT_NE(mirroredDisplayDrawable_, nullptr);
    ASSERT_NE(mirroredDisplayDrawable_->GetRenderParams(), nullptr);
    auto [_, mirroredScreenParams] =
        mirroredDisplayDrawable_->GetScreenParams(*mirroredDisplayDrawable_->GetRenderParams());
    ASSERT_NE(mirroredScreenParams, nullptr);
    mirroredScreenParams->SetBoundsRect({0, 0, 100, 100});

    // scale mirror screen
    mirroredDisplayDrawable_->ScaleAndRotateMirrorForWiredScreen(*displayDrawable_);
    ASSERT_NE(mirroredDisplayDrawable_->curCanvas_, nullptr);
    ASSERT_EQ(mirroredDisplayDrawable_->curCanvas_->GetTotalMatrix().Get(Drawing::Matrix::SCALE_X), 1);
}

/**
 * @tc.name: WiredScreenProjectionTest001
 * @tc.desc: Test WiredScreenProjection when curCanvas_ is nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, WiredScreenProjectionTest001, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);

    displayDrawable_->curCanvas_ = nullptr;
    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(params, nullptr);
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    displayDrawable_->WiredScreenProjection(*params, virtualProcesser);
    EXPECT_EQ(displayDrawable_->curCanvas_, nullptr);
}

/**
 * @tc.name: WiredScreenProjectionTest002
 * @tc.desc: Test WiredScreenProjection when curScreenDrawable is nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, WiredScreenProjectionTest002, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);

    screenDrawable_ = nullptr;
    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(params, nullptr);
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    displayDrawable_->WiredScreenProjection(*params, virtualProcesser);
}

/**
 * @tc.name: WiredScreenProjectionTest003
 * @tc.desc: Test WiredScreenProjection when curScreenParams is nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, WiredScreenProjectionTest003, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(screenDrawable_, nullptr);

    screenDrawable_->renderParams_ = nullptr;
    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    displayDrawable_->WiredScreenProjection(*params, virtualProcesser);
    EXPECT_EQ(screenDrawable_->renderParams_, nullptr);
}

/**
 * @tc.name: WiredScreenProjectionTest004
 * @tc.desc: Test WiredScreenProjection when mirroredDrawable is nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, WiredScreenProjectionTest004, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);

    mirroredDisplayDrawable_ = nullptr;
    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    displayDrawable_->WiredScreenProjection(*params, virtualProcesser);
}

/**
 * @tc.name: WiredScreenProjectionTest005
 * @tc.desc: Test WiredScreenProjection when mirroredDisplayParams is nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, WiredScreenProjectionTest005, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(mirroredDisplayDrawable_, nullptr);

    mirroredDisplayDrawable_->renderParams_ = nullptr;
    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    displayDrawable_->WiredScreenProjection(*params, virtualProcesser);
    EXPECT_EQ(mirroredDisplayDrawable_->renderParams_, nullptr);
}

/**
 * @tc.name: WiredScreenProjectionTest006
 * @tc.desc: Test WiredScreenProjection when mirroredScreenDrawable is nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, WiredScreenProjectionTest006, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);

    mirroredScreenDrawable_ = nullptr;
    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    displayDrawable_->WiredScreenProjection(*params, virtualProcesser);
}

/**
 * @tc.name: WiredScreenProjectionTest007
 * @tc.desc: Test WiredScreenProjection when mirroredScreenParams is nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, WiredScreenProjectionTest007, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);

    mirroredScreenDrawable_->renderParams_ = nullptr;
    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    displayDrawable_->WiredScreenProjection(*params, virtualProcesser);
    EXPECT_EQ(mirroredScreenDrawable_->renderParams_, nullptr);
}

/**
 * @tc.name: WiredScreenProjectionTest008
 * @tc.desc: Test WiredScreenProjection when has protected layer
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, WiredScreenProjectionTest008, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(mirroredDisplayDrawable_, nullptr);

    auto mirroredRenderParams =
        static_cast<RSLogicalDisplayRenderParams*>(mirroredDisplayDrawable_->GetRenderParams().get());
    RSSpecialLayerManager slManager;
    slManager.Set(SpecialLayerType::HAS_PROTECTED, true);
    mirroredRenderParams->specialLayerManager_ = slManager;
    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    auto uniParams = std::make_unique<RSRenderThreadParams>();
    RSUniRenderThread::Instance().Sync(std::move(uniParams));
    displayDrawable_->WiredScreenProjection(*params, virtualProcesser);
    EXPECT_TRUE(mirroredRenderParams->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_PROTECTED));
}

/**
 * @tc.name: DrawWiredMirrorCopyTest001
 * @tc.desc: Test DrawWiredMirrorCopy when mirroredParams is nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawWiredMirrorCopyTest001, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);

    ASSERT_NE(mirroredDisplayDrawable_, nullptr);
    mirroredDisplayDrawable_->renderParams_ = nullptr;
    displayDrawable_->DrawWiredMirrorCopy(*mirroredDisplayDrawable_);
    EXPECT_EQ(mirroredDisplayDrawable_->GetRenderParams(), nullptr);
}

/**
 * @tc.name: DrawWiredMirrorCopyTest002
 * @tc.desc: Test DrawWiredMirrorCopy when mirroredScreenDrawable is nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawWiredMirrorCopyTest002, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);

    mirroredScreenDrawable_ = nullptr;
    displayDrawable_->DrawWiredMirrorCopy(*mirroredDisplayDrawable_);
}

/**
 * @tc.name: DrawWiredMirrorCopyTest003
 * @tc.desc: Test DrawWiredMirrorCopy when cacheImage is not nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawWiredMirrorCopyTest003, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);

    mirroredScreenDrawable_->cacheImgForCapture_ = std::make_shared<Drawing::Image>();
    displayDrawable_->DrawWiredMirrorCopy(*mirroredDisplayDrawable_);
    EXPECT_NE(mirroredScreenDrawable_->GetCacheImgForCapture(), nullptr);

    // when rosen.cacheimage.mirror.enabled is 0
    system::SetParameter("rosen.cacheimage.mirror.enabled", "0");
    displayDrawable_->DrawWiredMirrorCopy(*mirroredDisplayDrawable_);
    EXPECT_EQ(std::atoi((system::GetParameter("rosen.cacheimage.mirror.enabled", "1")).c_str()), 0);
    system::SetParameter("rosen.cacheimage.mirror.enabled", "1");

    // when enableVisibleRect is true
    displayDrawable_->enableVisibleRect_ = true;
    displayDrawable_->DrawWiredMirrorCopy(*mirroredDisplayDrawable_);
}

/**
 * @tc.name: DrawWiredMirrorCopyTest004
 * @tc.desc: Test DrawWiredMirrorCopy when cacheImage is nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawWiredMirrorCopyTest004, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);

    // enableVisibleRect is false
    displayDrawable_->DrawWiredMirrorCopy(*mirroredDisplayDrawable_);

    // enableVisibleRect is true
    displayDrawable_->enableVisibleRect_ = true;
    displayDrawable_->DrawWiredMirrorCopy(*mirroredDisplayDrawable_);
}
}