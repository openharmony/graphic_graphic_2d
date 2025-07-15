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
        auto tempFilterCanvas = std::make_shared<RSPaintFilterCanvas>(tempCanvas_.get());
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
}