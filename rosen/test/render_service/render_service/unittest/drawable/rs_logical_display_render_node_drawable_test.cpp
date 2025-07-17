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
#include "render/rs_pixel_map_util.h"
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

/**
 * @tc.name: CheckDirtyRefreshTest001
 * @tc.desc: Test CheckDirtyRefresh when params is nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, CheckDirtyRefreshTest001, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);

    displayDrawable_->renderParams_ = nullptr;
    displayDrawable_->CheckDirtyRefresh(CompositeType::UNI_RENDER_COMPOSITE, false);
    EXPECT_EQ(displayDrawable_->GetRenderParams(), nullptr);
}

/**
 * @tc.name: CheckDirtyRefreshTest001
 * @tc.desc: Test CheckDirtyRefresh when mirroredDrawable is nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, CheckDirtyRefreshTest002, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);

    mirroredDisplayDrawable_ = nullptr;
    displayDrawable_->CheckDirtyRefresh(CompositeType::UNI_RENDER_COMPOSITE, false);
}

/**
 * @tc.name: CheckDirtyRefreshTest003
 * @tc.desc: Test CheckDirtyRefresh when mirroredDrawable is nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, CheckDirtyRefreshTest003, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(mirroredDisplayDrawable_, nullptr);

    mirroredDisplayDrawable_->renderParams_ = nullptr;
    displayDrawable_->CheckDirtyRefresh(CompositeType::UNI_RENDER_COMPOSITE, false);
    EXPECT_EQ(mirroredDisplayDrawable_->GetRenderParams(), nullptr);
}

/**
 * @tc.name: CheckDirtyRefreshTest004
 * @tc.desc: Test CheckDirtyRefresh when uniParam is nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, CheckDirtyRefreshTest004, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(mirroredDisplayDrawable_, nullptr);

    RSRenderThreadParamsManager::Instance().renderThreadParams_ = nullptr;
    displayDrawable_->CheckDirtyRefresh(CompositeType::UNI_RENDER_COMPOSITE, false);
    EXPECT_EQ(RSUniRenderThread::Instance().GetRSRenderThreadParams(), nullptr);
}

/**
 * @tc.name: CheckDirtyRefreshTest005
 * @tc.desc: Test CheckDirtyRefresh when uniParam is not nullptr and type is UNI_RENDER_COMPOSITE
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, CheckDirtyRefreshTest005, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(mirroredDisplayDrawable_, nullptr);

    // when hasSecSurface is false
    displayDrawable_->CheckDirtyRefresh(CompositeType::UNI_RENDER_COMPOSITE, false);
    // when hasSecSurface is true
    auto mirroredRenderParams =
        static_cast<RSLogicalDisplayRenderParams*>(mirroredDisplayDrawable_->GetRenderParams().get());
    ASSERT_NE(mirroredRenderParams, nullptr);
    RSSpecialLayerManager slManager;
    slManager.Set(SpecialLayerType::HAS_SECURITY, true);
    mirroredRenderParams->specialLayerManager_ = slManager;
    displayDrawable_->CheckDirtyRefresh(CompositeType::UNI_RENDER_COMPOSITE, false);
    EXPECT_TRUE(mirroredRenderParams->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_SECURITY));
}

/**
 * @tc.name: CheckDirtyRefreshTest006
 * @tc.desc: Test CheckDirtyRefresh when uniParam is not nullptr and type is UNI_RENDER_MIRROR_COMPOSITE
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, CheckDirtyRefreshTest006, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(mirroredDisplayDrawable_, nullptr);

    auto uniParams1 = std::make_unique<RSRenderThreadParams>();
    RSUniRenderThread::Instance().Sync(std::move(uniParams1));

    // when enableVisibleRect_ is false and hasSecSurface is false
    displayDrawable_->CheckDirtyRefresh(CompositeType::UNI_RENDER_MIRROR_COMPOSITE, false);
    // when enableVisibleRect_ is false and hasSecSurface is true
    auto mirroredRenderParams =
        static_cast<RSLogicalDisplayRenderParams*>(mirroredDisplayDrawable_->GetRenderParams().get());
    EXPECT_NE(mirroredRenderParams, nullptr);
    RSSpecialLayerManager slManager;
    slManager.Set(SpecialLayerType::HAS_SECURITY, true);
    mirroredRenderParams->specialLayerManager_ = slManager;
    displayDrawable_->CheckDirtyRefresh(CompositeType::UNI_RENDER_MIRROR_COMPOSITE, false);
    EXPECT_TRUE(mirroredRenderParams->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_SECURITY));

    // when GetSecExemption is true
    auto uniParams2 = std::make_unique<RSRenderThreadParams>();
    uniParams2->SetSecExemption(true);
    RSUniRenderThread::Instance().Sync(std::move(uniParams2));
    displayDrawable_->CheckDirtyRefresh(CompositeType::UNI_RENDER_MIRROR_COMPOSITE, false);
}

/**
 * @tc.name: CheckDirtyRefreshTest007
 * @tc.desc: Test CheckDirtyRefresh when enableVisibleRect is true
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, CheckDirtyRefreshTest007, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(mirroredDisplayDrawable_, nullptr);

    displayDrawable_->enableVisibleRect_ = true;
    // when hasSecLayerInVisibleRect is false
    displayDrawable_->CheckDirtyRefresh(CompositeType::UNI_RENDER_MIRROR_COMPOSITE, false);
    // when hasSecLayerInVisibleRect is true
    displayDrawable_->CheckDirtyRefresh(CompositeType::UNI_RENDER_MIRROR_COMPOSITE, true);
}

/**
 * @tc.name: CheckDirtyRefreshTest008
 * @tc.desc: Test CheckDirtyRefresh when GetVirtualScreenMuteStatus is true/false
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, CheckDirtyRefreshTest008, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(mirroredDisplayDrawable_, nullptr);

    auto uniParams1 = std::make_unique<RSRenderThreadParams>();
    RSUniRenderThread::Instance().Sync(std::move(uniParams1));
    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    // when GetVirtualScreenMuteStatus is false
    renderParams->virtualScreenMuteStatus_ = true;
    displayDrawable_->CheckDirtyRefresh(CompositeType::UNI_RENDER_MIRROR_COMPOSITE, false);
    // when GetVirtualScreenMuteStatus is true, reset uniParams
    auto uniParams2 = std::make_unique<RSRenderThreadParams>();
    RSUniRenderThread::Instance().Sync(std::move(uniParams2));
    renderParams->virtualScreenMuteStatus_ = false;
    displayDrawable_->CheckDirtyRefresh(CompositeType::UNI_RENDER_MIRROR_COMPOSITE, false);
}

/**
 * @tc.name: DrawWiredMirrorOnDrawTest001
 * @tc.desc: Test DrawWiredMirrorOnDraw when uniParam is nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawWiredMirrorOnDrawTest001, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(mirroredDisplayDrawable_, nullptr);

    RSRenderThreadParamsManager::Instance().renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    displayDrawable_->DrawWiredMirrorOnDraw(*mirroredDisplayDrawable_, *renderParams);
}

/**
 * @tc.name: DrawWiredMirrorOnDrawTest001
 * @tc.desc: Test DrawWiredMirrorOnDraw when mirroredParams is nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawWiredMirrorOnDrawTest002, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(mirroredDisplayDrawable_, nullptr);

    mirroredDisplayDrawable_->renderParams_ = nullptr;
    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    displayDrawable_->DrawWiredMirrorOnDraw(*mirroredDisplayDrawable_, *renderParams);
    EXPECT_EQ(mirroredDisplayDrawable_->GetRenderParams(), nullptr);
}

/**
 * @tc.name: DrawWiredMirrorOnDrawTest001
 * @tc.desc: Test DrawWiredMirrorOnDraw when IsExternalScreenSecure is true
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawWiredMirrorOnDrawTest003, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(mirroredDisplayDrawable_, nullptr);

    auto isExternalScreenSecure = MultiScreenParam::IsExternalScreenSecure();
    MultiScreenParam::SetExternalScreenSecure(true);
    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    displayDrawable_->DrawWiredMirrorOnDraw(*mirroredDisplayDrawable_, *renderParams);
    EXPECT_TRUE(MultiScreenParam::IsExternalScreenSecure());
    MultiScreenParam::SetExternalScreenSecure(isExternalScreenSecure);
}

/**
 * @tc.name: DrawWiredMirrorOnDrawTest001
 * @tc.desc: Test DrawWiredMirrorOnDraw when IsExternalScreenSecure is false and hasSecSurface is true
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawWiredMirrorOnDrawTest004, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(mirroredDisplayDrawable_, nullptr);

    RSSpecialLayerManager slManager;
    slManager.Set(SpecialLayerType::HAS_SECURITY, true);
    auto mirroredRenderParams =
        static_cast<RSLogicalDisplayRenderParams*>(mirroredDisplayDrawable_->GetRenderParams().get());
    mirroredRenderParams->specialLayerManager_ = slManager;
    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    displayDrawable_->DrawWiredMirrorOnDraw(*mirroredDisplayDrawable_, *renderParams);
    EXPECT_TRUE(mirroredRenderParams->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_SECURITY));
}

/**
 * @tc.name: DrawWiredMirrorOnDrawTest001
 * @tc.desc: Test DrawWiredMirrorOnDraw when mirroredScreenParams is nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawWiredMirrorOnDrawTest005, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(mirroredDisplayDrawable_, nullptr);
    ASSERT_NE(mirroredScreenDrawable_, nullptr);

    mirroredScreenDrawable_->renderParams_ = nullptr;
    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    displayDrawable_->DrawWiredMirrorOnDraw(*mirroredDisplayDrawable_, *renderParams);
    EXPECT_EQ(mirroredScreenDrawable_->GetRenderParams(), nullptr);
}

/**
 * @tc.name: DrawWiredMirrorOnDrawTest001
 * @tc.desc: Test DrawWiredMirrorOnDraw with default
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawWiredMirrorOnDrawTest006, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(mirroredDisplayDrawable_, nullptr);
    ASSERT_NE(mirroredScreenDrawable_, nullptr);

    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    displayDrawable_->DrawWiredMirrorOnDraw(*mirroredDisplayDrawable_, *renderParams);
}

/**
 * @tc.name: DrawMirrorScreenTest001
 * @tc.desc: Test DrawMirrorScreen when mirroredDrawable is nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawMirrorScreenTest001, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);

    mirroredDisplayDrawable_ = nullptr;
    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    displayDrawable_->DrawMirrorScreen(*renderParams, nullptr);
}

/**
 * @tc.name: DrawMirrorScreenTest002
 * @tc.desc: Test DrawMirrorScreen when mirroredParams is nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawMirrorScreenTest002, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(mirroredDisplayDrawable_, nullptr);

    mirroredDisplayDrawable_->renderParams_ = nullptr;
    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    displayDrawable_->DrawMirrorScreen(*renderParams, nullptr);
}

/**
 * @tc.name: DrawMirrorScreenTest003
 * @tc.desc: Test DrawMirrorScreen when virtualProcesser is nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawMirrorScreenTest003, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);

    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    displayDrawable_->DrawMirrorScreen(*renderParams, nullptr);
}

/**
 * @tc.name: DrawMirrorScreenTest004
 * @tc.desc: Test DrawMirrorScreen when screenParams is nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawMirrorScreenTest004, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(screenDrawable_, nullptr);

    screenDrawable_->renderParams_ = nullptr;
    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    virtualProcesser->canvas_ = drawingFilterCanvas_;
    auto uniParams = std::make_unique<RSRenderThreadParams>();
    RSUniRenderThread::Instance().Sync(std::move(uniParams));
    displayDrawable_->DrawMirrorScreen(*renderParams, virtualProcesser);
}

/**
 * @tc.name: DrawMirrorScreenTest005
 * @tc.desc: Test DrawMirrorScreen when mirroredParams isSecurityDisplay != params isSecurityDisplay
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawMirrorScreenTest005, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(screenDrawable_, nullptr);

    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    renderParams->isSecurityDisplay_ = true;
    auto mirroredRenderParams =
        static_cast<RSLogicalDisplayRenderParams*>(mirroredDisplayDrawable_->GetRenderParams().get());
    mirroredRenderParams->isSecurityDisplay_ = false;
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    virtualProcesser->canvas_ = drawingFilterCanvas_;
    auto uniParams = std::make_unique<RSRenderThreadParams>();
    RSUniRenderThread::Instance().Sync(std::move(uniParams));
    displayDrawable_->DrawMirrorScreen(*renderParams, virtualProcesser);
    EXPECT_FALSE(virtualProcesser->GetDrawVirtualMirrorCopy());

    // when specialLayerType is HAS_SPECIAL_LAYER
    RSSpecialLayerManager slManager;
    slManager.Set(SpecialLayerType::HAS_SECURITY, true);
    mirroredRenderParams->specialLayerManager_ = slManager;
    displayDrawable_->DrawMirrorScreen(*renderParams, virtualProcesser);
    EXPECT_FALSE(virtualProcesser->GetDrawVirtualMirrorCopy());
}

/**
 * @tc.name: DrawMirrorScreenTest006
 * @tc.desc: Test DrawMirrorScreen when cacheImage is not nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawMirrorScreenTest006, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(mirroredScreenDrawable_, nullptr);

    mirroredScreenDrawable_->cacheImgForCapture_ = std::make_shared<Drawing::Image>();
    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    virtualProcesser->canvas_ = drawingFilterCanvas_;
    auto uniParams = std::make_unique<RSRenderThreadParams>();
    RSUniRenderThread::Instance().Sync(std::move(uniParams));
    displayDrawable_->DrawMirrorScreen(*renderParams, virtualProcesser);
    EXPECT_TRUE(virtualProcesser->GetDrawVirtualMirrorCopy());
}

/**
 * @tc.name: DrawMirrorScreenTest006
 * @tc.desc: Test DrawMirrorScreen when GetVirtualScreenMuteStatus is true
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawMirrorScreenTest008, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(screenDrawable_, nullptr);

    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    renderParams->virtualScreenMuteStatus_ = true;
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    virtualProcesser->canvas_ = drawingFilterCanvas_;
    auto uniParams = std::make_unique<RSRenderThreadParams>();
    RSUniRenderThread::Instance().Sync(std::move(uniParams));
    displayDrawable_->DrawMirrorScreen(*renderParams, virtualProcesser);
    EXPECT_FALSE(virtualProcesser->GetDrawVirtualMirrorCopy());
}

/**
 * @tc.name: DrawMirrorScreenTest006
 * @tc.desc: Test DrawMirrorScreen when default, redraw
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawMirrorScreenTest009, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(screenDrawable_, nullptr);

    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    virtualProcesser->canvas_ = drawingFilterCanvas_;
    auto uniParams = std::make_unique<RSRenderThreadParams>();
    RSUniRenderThread::Instance().Sync(std::move(uniParams));
    displayDrawable_->DrawMirrorScreen(*renderParams, virtualProcesser);
    EXPECT_FALSE(virtualProcesser->GetDrawVirtualMirrorCopy());
}

/**
 * @tc.name: DrawMirrorScreenTest006
 * @tc.desc: Test DrawMirrorScreen when default, redraw
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawMirrorScreenTest010, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(screenDrawable_, nullptr);

    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    virtualProcesser->canvas_ = drawingFilterCanvas_;
    auto uniParams = std::make_unique<RSRenderThreadParams>();
    RSUniRenderThread::Instance().Sync(std::move(uniParams));
    displayDrawable_->DrawMirrorScreen(*renderParams, virtualProcesser);
    EXPECT_FALSE(virtualProcesser->GetDrawVirtualMirrorCopy());
}

/**
 * @tc.name: DrawMirrorScreenTest006
 * @tc.desc: Test DrawMirrorScreen when default, redraw
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawMirrorScreenTest011, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(screenDrawable_, nullptr);

    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    virtualProcesser->canvas_ = drawingFilterCanvas_;
    auto uniParams = std::make_unique<RSRenderThreadParams>();
    RSUniRenderThread::Instance().Sync(std::move(uniParams));
    displayDrawable_->DrawMirrorScreen(*renderParams, virtualProcesser);
    EXPECT_FALSE(virtualProcesser->GetDrawVirtualMirrorCopy());
}

/**
 * @tc.name: DrawMirrorCopyTest001
 * @tc.desc: Test DrawMirrorCopy when curScreenDrawable is nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawMirrorCopyTest001, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);

    screenDrawable_ = nullptr;
    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    auto uniParams = std::make_unique<RSRenderThreadParams>();
    RSUniRenderThread::Instance().Sync(std::move(uniParams));
    auto& uniParamsRet = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    ASSERT_NE(uniParamsRet, nullptr);
    displayDrawable_->DrawMirrorCopy(*renderParams, virtualProcesser, *uniParamsRet);
}

/**
 * @tc.name: DrawMirrorCopyTest002
 * @tc.desc: Test DrawMirrorCopy when curScreenParams is nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawMirrorCopyTest002, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(screenDrawable_, nullptr);

    screenDrawable_->renderParams_ = nullptr;
    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    auto uniParams = std::make_unique<RSRenderThreadParams>();
    RSUniRenderThread::Instance().Sync(std::move(uniParams));
    auto& uniParamsRet = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    ASSERT_NE(uniParamsRet, nullptr);
    displayDrawable_->DrawMirrorCopy(*renderParams, virtualProcesser, *uniParamsRet);
    EXPECT_EQ(screenDrawable_->GetRenderParams(), nullptr);
}

/**
 * @tc.name: DrawMirrorCopyTest002
 * @tc.desc: Test DrawMirrorCopy when mirroredDrawable is nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawMirrorCopyTest003, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);

    mirroredDisplayDrawable_ = nullptr;
    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    auto uniParams = std::make_unique<RSRenderThreadParams>();
    RSUniRenderThread::Instance().Sync(std::move(uniParams));
    auto& uniParamsRet = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    ASSERT_NE(uniParamsRet, nullptr);
    displayDrawable_->DrawMirrorCopy(*renderParams, virtualProcesser, *uniParamsRet);
}

/**
 * @tc.name: DrawMirrorCopyTest002
 * @tc.desc: Test DrawMirrorCopy when mirroredParams is nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawMirrorCopyTest004, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(mirroredDisplayDrawable_, nullptr);

    mirroredDisplayDrawable_->renderParams_ = nullptr;
    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    auto uniParams = std::make_unique<RSRenderThreadParams>();
    RSUniRenderThread::Instance().Sync(std::move(uniParams));
    auto& uniParamsRet = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    ASSERT_NE(uniParamsRet, nullptr);
    displayDrawable_->DrawMirrorCopy(*renderParams, virtualProcesser, *uniParamsRet);
    EXPECT_EQ(mirroredDisplayDrawable_->GetRenderParams(), nullptr);
}

/**
 * @tc.name: DrawMirrorCopyTest002
 * @tc.desc: Test DrawMirrorCopy when mirroredScreenDrawable is nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawMirrorCopyTest005, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);

    mirroredScreenDrawable_ = nullptr;
    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    auto uniParams = std::make_unique<RSRenderThreadParams>();
    RSUniRenderThread::Instance().Sync(std::move(uniParams));
    auto& uniParamsRet = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    ASSERT_NE(uniParamsRet, nullptr);
    displayDrawable_->DrawMirrorCopy(*renderParams, virtualProcesser, *uniParamsRet);
}

/**
 * @tc.name: DrawMirrorCopyTest002
 * @tc.desc: Test DrawMirrorCopy when mirroredScreenParams is nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawMirrorCopyTest006, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(mirroredScreenDrawable_, nullptr);

    mirroredScreenDrawable_->renderParams_ = nullptr;
    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    auto uniParams = std::make_unique<RSRenderThreadParams>();
    RSUniRenderThread::Instance().Sync(std::move(uniParams));
    auto& uniParamsRet = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    ASSERT_NE(uniParamsRet, nullptr);
    displayDrawable_->DrawMirrorCopy(*renderParams, virtualProcesser, *uniParamsRet);
    EXPECT_EQ(mirroredScreenDrawable_->GetRenderParams(), nullptr);
}

/**
 * @tc.name: DrawMirrorCopyTest002
 * @tc.desc: Test DrawMirrorCopy when IsVirtualDirtyEnabled is true
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawMirrorCopyTest007, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);

    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    RSUniRenderThread::Instance().GetRSRenderThreadParams()->isVirtualDirtyEnabled_ = true;
    auto& uniParams = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    displayDrawable_->DrawMirrorCopy(*renderParams, virtualProcesser, *uniParams);
    EXPECT_TRUE(RSUniRenderThread::Instance().GetRSRenderThreadParams()->IsVirtualDirtyEnabled());
}

/**
 * @tc.name: DrawMirrorCopyTest007_SKP
 * @tc.desc: Test DrawMirrorCopy when IsVirtualDirtyEnabled is true (with SKP capture triggering)
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawMirrorCopyTest007_SKP, TestSize.Level1)
{
#ifdef RS_PROFILER_ENABLED
    RSCaptureRecorder::testingTriggering_ = true;
    RSCaptureRecorder::GetInstance().SetCaptureType(SkpCaptureType::IMG_CACHED);
#endif
    ASSERT_NE(displayDrawable_, nullptr);
    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    RSUniRenderThread::Instance().GetRSRenderThreadParams()->isVirtualDirtyEnabled_ = true;
    auto& uniParams = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    displayDrawable_->DrawMirrorCopy(*renderParams, virtualProcesser, *uniParams);
    EXPECT_TRUE(RSUniRenderThread::Instance().GetRSRenderThreadParams()->IsVirtualDirtyEnabled());
#ifdef RS_PROFILER_ENABLED
    RSCaptureRecorder::testingTriggering_ = false;
#endif
}

/**
 * @tc.name: DrawMirrorCopyTest002
 * @tc.desc: Test DrawMirrorCopy when enableVisibleRect_ is true
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawMirrorCopyTest008, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);

    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    auto uniParams = std::make_unique<RSRenderThreadParams>();
    RSUniRenderThread::Instance().Sync(std::move(uniParams));
    auto& uniParamsRet = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    ASSERT_NE(uniParamsRet, nullptr);
    displayDrawable_->enableVisibleRect_ = true;
    // when GetTop > 0
    displayDrawable_->curVisibleRect_ = {1, 1, 1, 1};
    displayDrawable_->DrawMirrorCopy(*renderParams, virtualProcesser, *uniParamsRet);
    // when GetTop > 0
    displayDrawable_->curVisibleRect_ = {0, 0, 0, 0};
    displayDrawable_->DrawMirrorCopy(*renderParams, virtualProcesser, *uniParamsRet);
}

/**
 * @tc.name: DrawMirrorCopyTest002
 * @tc.desc: Test DrawMirrorCopy when curCanvas_ is nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawMirrorCopyTest009, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);

    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    auto uniParams = std::make_unique<RSRenderThreadParams>();
    RSUniRenderThread::Instance().Sync(std::move(uniParams));
    auto& uniParamsRet = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    displayDrawable_->DrawMirrorCopy(*renderParams, virtualProcesser, *uniParamsRet);
}

/**
 * @tc.name: DrawMirrorCopyTest002
 * @tc.desc: Test DrawMirrorCopy when curCanvas_ is not nullptr and slrManager is not null
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawMirrorCopyTest010, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);

    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    virtualProcesser->canvas_ = drawingFilterCanvas_;
    auto uniParams = std::make_unique<RSRenderThreadParams>();
    RSUniRenderThread::Instance().Sync(std::move(uniParams));
    auto& uniParamsRet = RSUniRenderThread::Instance().GetRSRenderThreadParams();

    // when slrManager is nullptr
    displayDrawable_->DrawMirrorCopy(*renderParams, virtualProcesser, *uniParamsRet);
    // when slrManager is not nullptr
    virtualProcesser->slrManager_ = std::make_shared<RSSLRScaleFunction>(1.0f, 1.0f, 1.0f, 1.0f);
    displayDrawable_->DrawMirrorCopy(*renderParams, virtualProcesser, *uniParamsRet);
}

/**
 * @tc.name: DrawMirrorCopyTest010_SKP
 * @tc.desc: Test DrawMirrorCopy when curCanvas_ is not nullptr and slrManager is not null
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawMirrorCopyTest010_SKP, TestSize.Level1)
{
#ifdef RS_PROFILER_ENABLED
    RSCaptureRecorder::testingTriggering_ = true;
    RSCaptureRecorder::GetInstance().SetCaptureType(SkpCaptureType::IMG_CACHED);
#endif
    ASSERT_NE(displayDrawable_, nullptr);
    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    virtualProcesser->canvas_ = drawingFilterCanvas_;
    auto uniParams = std::make_unique<RSRenderThreadParams>();
    RSUniRenderThread::Instance().Sync(std::move(uniParams));
    auto& uniParamsRet = RSUniRenderThread::Instance().GetRSRenderThreadParams();

    // when slrManager is nullptr
    displayDrawable_->DrawMirrorCopy(*renderParams, virtualProcesser, *uniParamsRet);
    // when slrManager is not nullptr
    virtualProcesser->slrManager_ = std::make_shared<RSSLRScaleFunction>(1.0f, 1.0f, 1.0f, 1.0f);
    displayDrawable_->DrawMirrorCopy(*renderParams, virtualProcesser, *uniParamsRet);
#ifdef RS_PROFILER_ENABLED
    RSCaptureRecorder::testingTriggering_ = false;
#endif
}

/**
 * @tc.name: DrawMirrorCopyTest002
 * @tc.desc: Test DrawMirrorCopy when cacheImg is not nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawMirrorCopyTest011, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(mirroredScreenDrawable_, nullptr);

    mirroredScreenDrawable_->cacheImgForCapture_ = std::make_shared<Drawing::Image>();
    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    virtualProcesser->canvas_ = drawingFilterCanvas_;
    auto uniParams = std::make_unique<RSRenderThreadParams>();
    RSUniRenderThread::Instance().Sync(std::move(uniParams));
    auto& uniParamsRet = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    // enableVisibleRect_ is false
    displayDrawable_->enableVisibleRect_ = false;
    displayDrawable_->DrawMirrorCopy(*renderParams, virtualProcesser, *uniParamsRet);
    EXPECT_FALSE(displayDrawable_->enableVisibleRect_);
    // enableVisibleRect_ is true
    displayDrawable_->enableVisibleRect_ = true;
    displayDrawable_->DrawMirrorCopy(*renderParams, virtualProcesser, *uniParamsRet);
    EXPECT_TRUE(displayDrawable_->enableVisibleRect_);
}

/**
 * @tc.name: DrawMirrorCopyTest002
 * @tc.desc: Test DrawMirrorCopy when GetDrawMirrorCacheImageEnabled is true/false
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawMirrorCopyTest012, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(mirroredScreenDrawable_, nullptr);
    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    virtualProcesser->canvas_ = drawingFilterCanvas_;
    auto uniParams = std::make_unique<RSRenderThreadParams>();
    RSUniRenderThread::Instance().Sync(std::move(uniParams));
    auto& uniParamsRet = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    // when isSamplingOn is false
    auto mirroredScreenParams = static_cast<RSScreenRenderParams*>(screenDrawable_->GetRenderParams().get());
    ScreenInfo screenInfo;
    screenInfo.isSamplingOn = false;
    mirroredScreenParams->screenInfo_ = screenInfo;
    displayDrawable_->DrawMirrorCopy(*renderParams, virtualProcesser, *uniParamsRet);
    EXPECT_FALSE(mirroredScreenParams->GetScreenInfo().isSamplingOn);
    // when isSamplingOn is true
    screenInfo.isSamplingOn = true;
    mirroredScreenParams->screenInfo_ = screenInfo;
    displayDrawable_->DrawMirrorCopy(*renderParams, virtualProcesser, *uniParamsRet);
    EXPECT_TRUE(mirroredScreenParams->GetScreenInfo().isSamplingOn);
}

/**
 * @tc.name: DrawMirror
 * @tc.desc: Test DrawMirror
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawMirrorTest, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);

    displayDrawable_->PrepareOffscreenRender(*displayDrawable_, false);
    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto processor = RSProcessorFactory::CreateProcessor(params->GetCompositeType());
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    RSRenderThreadParams uniParam;
    displayDrawable_->DrawMirror(*params, virtualProcesser,
        &RSLogicalDisplayRenderNodeDrawable::OnCapture, uniParam);

    drawingCanvas_ = std::make_unique<Drawing::Canvas>(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    ASSERT_NE(drawingCanvas_, nullptr);
    displayDrawable_->curCanvas_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvas_.get()).get();
    displayDrawable_->DrawMirror(*params, virtualProcesser,
        &RSLogicalDisplayRenderNodeDrawable::OnCapture, uniParam);
}

/**
 * @tc.name: DrawMirror001
 * @tc.desc: Test DrawMirror
 * @tc.type: FUNC
 * @tc.require: issueICCRA8
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawMirrorTest001, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
    ASSERT_NE(mirroredDisplayDrawable_, nullptr);
    ASSERT_NE(mirroredDisplayDrawable_->renderParams_, nullptr);

    displayDrawable_->PrepareOffscreenRender(*displayDrawable_, false);
    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto processor = RSProcessorFactory::CreateProcessor(params->GetCompositeType());
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    RSRenderThreadParams uniParam;

    Drawing::Canvas drawingCanvas;
    virtualProcesser->canvas_ = std::make_unique<RSPaintFilterCanvas>(&drawingCanvas);
    ASSERT_NE(virtualProcesser->GetCanvas(), nullptr);

    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);
    auto id = screenManager->CreateVirtualScreen("virtualScreen01", 480, 320, psurface);
    params->screenId_ = id;
    ASSERT_EQ(params->GetScreenId(), id);
    int32_t virtualSecLayerOption = screenManager->GetVirtualScreenSecLayerOption(params->GetScreenId());
    ASSERT_EQ(virtualSecLayerOption, 0);

    auto drawable = DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(mirroredNode_);
    ASSERT_NE(drawable, nullptr);
    params->mirrorSourceDrawable_ = drawable;
    RSSpecialLayerManager slManager;
    slManager.specialLayerType_ = SpecialLayerType::HAS_SECURITY;
    auto mirroredParams = static_cast<RSLogicalDisplayRenderParams*>(drawable->GetRenderParams().get());
    ASSERT_NE(mirroredParams, nullptr);
    mirroredParams->specialLayerManager_ = slManager;
    bool hasSecSurface = mirroredParams->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_SECURITY);
    ASSERT_EQ(hasSecSurface, true);
    displayDrawable_->DrawMirror(*params, virtualProcesser, &RSLogicalDisplayRenderNodeDrawable::OnCapture, uniParam);
}

/**
 * @tc.name: DrawMirror001_SKP
 * @tc.desc: Test DrawMirror (with SKP capture triggering)
 * @tc.type: FUNC
 * @tc.require: issueICCRA8
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawMirrorTest001_SKP, TestSize.Level1)
{
#ifdef RS_PROFILER_ENABLED
    RSCaptureRecorder::testingTriggering_ = true;
    RSCaptureRecorder::GetInstance().SetCaptureType(SkpCaptureType::ON_CAPTURE);
#endif
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
    ASSERT_NE(mirroredDisplayDrawable_, nullptr);
    ASSERT_NE(mirroredDisplayDrawable_->renderParams_, nullptr);

    displayDrawable_->PrepareOffscreenRender(*displayDrawable_, false);
    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto processor = RSProcessorFactory::CreateProcessor(params->GetCompositeType());
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    RSRenderThreadParams uniParam;

    Drawing::Canvas drawingCanvas;
    virtualProcesser->canvas_ = std::make_unique<RSPaintFilterCanvas>(&drawingCanvas);
    ASSERT_NE(virtualProcesser->GetCanvas(), nullptr);

    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);
    auto id = screenManager->CreateVirtualScreen("virtualScreen01", 480, 320, psurface);
    params->screenId_ = id;
    ASSERT_EQ(params->GetScreenId(), id);
    int32_t virtualSecLayerOption = screenManager->GetVirtualScreenSecLayerOption(params->GetScreenId());
    ASSERT_EQ(virtualSecLayerOption, 0);

    auto drawable = DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(mirroredNode_);
    ASSERT_NE(drawable, nullptr);
    params->mirrorSourceDrawable_ = drawable;
    RSSpecialLayerManager slManager;
    slManager.specialLayerType_ = SpecialLayerType::HAS_SECURITY;
    auto mirroredParams = static_cast<RSLogicalDisplayRenderParams*>(drawable->GetRenderParams().get());
    ASSERT_NE(mirroredParams, nullptr);
    mirroredParams->specialLayerManager_ = slManager;
    bool hasSecSurface = mirroredParams->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_SECURITY);
    ASSERT_EQ(hasSecSurface, true);
    displayDrawable_->DrawMirror(*params, virtualProcesser, &RSLogicalDisplayRenderNodeDrawable::OnCapture, uniParam);
#ifdef RS_PROFILER_ENABLED
    RSCaptureRecorder::testingTriggering_ = false;
#endif
}

/**
 * @tc.name: DrawMirror002
 * @tc.desc: Test DrawMirror
 * @tc.type: FUNC
 * @tc.require: issueICCRA8
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawMirrorTest002, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
    ASSERT_NE(mirroredDisplayDrawable_, nullptr);
    ASSERT_NE(mirroredDisplayDrawable_->renderParams_, nullptr);

    displayDrawable_->PrepareOffscreenRender(*displayDrawable_, false);
    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto processor = RSProcessorFactory::CreateProcessor(params->GetCompositeType());
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    RSRenderThreadParams uniParam;

    Drawing::Canvas drawingCanvas;
    virtualProcesser->canvas_ = std::make_unique<RSPaintFilterCanvas>(&drawingCanvas);
    ASSERT_NE(virtualProcesser->GetCanvas(), nullptr);

    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);
    auto id = screenManager->CreateVirtualScreen("virtualScreen02", 480, 320, psurface, 0, 1);
    params->screenId_ = id;
    ASSERT_EQ(params->GetScreenId(), id);
    int32_t virtualSecLayerOption = screenManager->GetVirtualScreenSecLayerOption(params->GetScreenId());
    ASSERT_EQ(virtualSecLayerOption, 1);

    auto drawable = DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(mirroredNode_);
    ASSERT_NE(drawable, nullptr);
    params->mirrorSourceDrawable_ = drawable;
    RSSpecialLayerManager slManager;
    slManager.specialLayerType_ = SpecialLayerType::HAS_SECURITY;
    auto mirroredParams = static_cast<RSLogicalDisplayRenderParams*>(drawable->GetRenderParams().get());
    ASSERT_NE(mirroredParams, nullptr);
    mirroredParams->specialLayerManager_ = slManager;
    bool hasSecSurface = mirroredParams->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_SECURITY);
    ASSERT_EQ(hasSecSurface, true);
    displayDrawable_->DrawMirror(*params, virtualProcesser, &RSLogicalDisplayRenderNodeDrawable::OnCapture, uniParam);
}

/**
 * @tc.name: DrawMirror002_SKP
 * @tc.desc: Test DrawMirror (with SKP capture triggering)
 * @tc.type: FUNC
 * @tc.require: issueICCRA8
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawMirrorTest002_SKP, TestSize.Level1)
{
#ifdef RS_PROFILER_ENABLED
    RSCaptureRecorder::testingTriggering_ = true;
    RSCaptureRecorder::GetInstance().SetCaptureType(SkpCaptureType::ON_CAPTURE);
#endif
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
    ASSERT_NE(mirroredDisplayDrawable_, nullptr);
    ASSERT_NE(mirroredDisplayDrawable_->renderParams_, nullptr);

    displayDrawable_->PrepareOffscreenRender(*displayDrawable_, false);
    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto processor = RSProcessorFactory::CreateProcessor(params->GetCompositeType());
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    RSRenderThreadParams uniParam;

    Drawing::Canvas drawingCanvas;
    virtualProcesser->canvas_ = std::make_unique<RSPaintFilterCanvas>(&drawingCanvas);
    ASSERT_NE(virtualProcesser->GetCanvas(), nullptr);

    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);
    auto id = screenManager->CreateVirtualScreen("virtualScreen02", 480, 320, psurface, 0, 1);
    params->screenId_ = id;
    ASSERT_EQ(params->GetScreenId(), id);
    int32_t virtualSecLayerOption = screenManager->GetVirtualScreenSecLayerOption(params->GetScreenId());
    ASSERT_EQ(virtualSecLayerOption, 1);

    auto drawable = DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(mirroredNode_);
    ASSERT_NE(drawable, nullptr);
    params->mirrorSourceDrawable_ = drawable;
    RSSpecialLayerManager slManager;
    slManager.specialLayerType_ = SpecialLayerType::HAS_SECURITY;
    auto mirroredParams = static_cast<RSLogicalDisplayRenderParams*>(drawable->GetRenderParams().get());
    ASSERT_NE(mirroredParams, nullptr);
    mirroredParams->specialLayerManager_ = slManager;
    bool hasSecSurface = mirroredParams->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_SECURITY);
    ASSERT_EQ(hasSecSurface, true);
    displayDrawable_->DrawMirror(*params, virtualProcesser, &RSLogicalDisplayRenderNodeDrawable::OnCapture, uniParam);
#ifdef RS_PROFILER_ENABLED
    RSCaptureRecorder::testingTriggering_ = false;
#endif
}

/**
 * @tc.name: DrawMirror003
 * @tc.desc: Test DrawMirror when curScreenParams/curScreenDrawable is nullptr
 * @tc.type: FUNC
 * @tc.require: issueICCRA8
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawMirrorTest003, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);

    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    RSRenderThreadParams uniParam;
    // when curScreenParams is nullptr
    ASSERT_NE(screenDrawable_, nullptr);
    screenDrawable_->renderParams_ = nullptr;
    displayDrawable_->DrawMirror(
        *renderParams, virtualProcesser, &RSLogicalDisplayRenderNodeDrawable::OnCapture, uniParam);
    // when curScreenDrawable is nullptr
    screenDrawable_ = nullptr;
    displayDrawable_->DrawMirror(
        *renderParams, virtualProcesser, &RSLogicalDisplayRenderNodeDrawable::OnCapture, uniParam);
}

/**
 * @tc.name: DrawMirrorTest004
 * @tc.desc: Test DrawMirror when mirroredParams/mirroredDrawable is nullptr
 * @tc.type: FUNC
 * @tc.require: issueICCRA8
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawMirrorTest004, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);

    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    RSRenderThreadParams uniParam;
    // when mirroredParams is nullptr
    ASSERT_NE(mirroredDisplayDrawable_, nullptr);
    mirroredDisplayDrawable_->renderParams_ = nullptr;
    displayDrawable_->DrawMirror(
        *renderParams, virtualProcesser, &RSLogicalDisplayRenderNodeDrawable::OnCapture, uniParam);
    // when mirroredDrawable is nullptr
    mirroredDisplayDrawable_ = nullptr;
    displayDrawable_->DrawMirror(
        *renderParams, virtualProcesser, &RSLogicalDisplayRenderNodeDrawable::OnCapture, uniParam);
}

/**
 * @tc.name: DrawMirror005
 * @tc.desc: Test DrawMirror when mirroredScreenParams is nullptr
 * @tc.type: FUNC
 * @tc.require: issueICCRA8
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawMirrorTest005, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);

    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    RSRenderThreadParams uniParam;
    ASSERT_NE(mirroredScreenDrawable_, nullptr);
    mirroredScreenDrawable_->renderParams_ = nullptr;
    displayDrawable_->DrawMirror(
        *renderParams, virtualProcesser, &RSLogicalDisplayRenderNodeDrawable::OnCapture, uniParam);
}

/**
 * @tc.name: DrawMirrorTest006
 * @tc.desc: Test DrawMirror when mirroredScreenParams is nullptr
 * @tc.type: FUNC
 * @tc.require: issueICCRA8
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawMirrorTest006, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);

    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    RSRenderThreadParams uniParam;

    displayDrawable_->DrawMirror(
        *renderParams, virtualProcesser, &RSLogicalDisplayRenderNodeDrawable::OnCapture, uniParam);
}

/**
 * @tc.name: PrepareOffscreenRender001
 * @tc.desc: Test PrepareOffscreenRender, if offscreenWidth/offscreenHeight were not initialized.
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, PrepareOffscreenRender001, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    displayDrawable_->PrepareOffscreenRender(*displayDrawable_);

    auto type = RotateOffScreenParam::GetRotateOffScreenScreenNodeEnable();
    RotateOffScreenParam::SetRotateOffScreenDisplayNodeEnable(true);

    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    params->frameRect_ = { 0.f, 0.f, 1.f, 0.f};
    displayDrawable_->PrepareOffscreenRender(*displayDrawable_);
    ASSERT_FALSE(params->IsRotationChanged());

    params->frameRect_ = { 0.f, 0.f, 1.f, 1.f};
    displayDrawable_->PrepareOffscreenRender(*displayDrawable_);
    ASSERT_FALSE(displayDrawable_->curCanvas_->GetSurface());

    auto surface = std::make_shared<Drawing::Surface>();
    displayDrawable_->curCanvas_->surface_ = surface.get();
    displayDrawable_->PrepareOffscreenRender(*displayDrawable_);
    ASSERT_TRUE(displayDrawable_->curCanvas_->GetSurface());
    RotateOffScreenParam::SetRotateOffScreenDisplayNodeEnable(type);
}

/**
 * @tc.name: PrepareOffscreenRender002
 * @tc.desc: Test PrepareOffscreenRender, offscreenWidth/offscreenHeight is/are correctly initialized.
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, PrepareOffscreenRender002, TestSize.Level1)
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
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, ClearTransparentBeforeSaveLayer, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    auto displayParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(displayParams, nullptr);
    auto surfaceNode1 = std::make_shared<RSSurfaceRenderNode>(1);
    auto drawable1 = std::make_shared<DrawableV2::RSSurfaceRenderNodeDrawable>(std::move(surfaceNode1));
    auto surfaceNode2 = std::make_shared<RSSurfaceRenderNode>(2);
    auto drawable2 = RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode2);
    surfaceNode2->InitRenderParams();

    auto renderParams = std::make_unique<RSRenderThreadParams>();
    renderParams->hardwareEnabledTypeDrawables_.push_back(std::make_tuple(1, 0, nullptr));
    renderParams->hardwareEnabledTypeDrawables_.push_back(std::make_tuple(1, displayParams->GetId() + 1, drawable1));
    renderParams->hardwareEnabledTypeDrawables_.push_back(std::make_tuple(1, displayParams->GetId(), drawable1));
    renderParams->hardwareEnabledTypeDrawables_.push_back(std::make_tuple(1, displayParams->GetId(), drawable2));
    auto surfaceParam = static_cast<RSSurfaceRenderParams*>(drawable2->GetRenderParams().get());
    surfaceParam->SetHardwareEnabled(true);
    renderParams->hardwareEnabledTypeDrawables_.push_back(std::make_tuple(1, displayParams->GetId(), drawable2));
    RSUniRenderThread::Instance().Sync(std::move(renderParams));

    auto canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas_.get());
    displayDrawable_->canvasBackup_ = canvas.get();
    displayDrawable_->ClearTransparentBeforeSaveLayer();
}

/**
 * @tc.name: PrepareOffscreenRender_InvalidSize
 * @tc.desc: Test PrepareOffscreenRender with invalid screen size
 * @tc.type: FUNC
 * @tc.require: issue
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, PrepareOffscreenRender_InvalidSize, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(params, nullptr);
    params->frameRect_ = {0, 0, 0, 0};
    displayDrawable_->PrepareOffscreenRender(*displayDrawable_, false);
}

/**
 * @tc.name: PrepareOffscreenRender_ValidSize
 * @tc.desc: Test PrepareOffscreenRender with valid screen size
 * @tc.type: FUNC
 * @tc.require: issue
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, PrepareOffscreenRender_ValidSize, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(params, nullptr);
    params->frameRect_ = {0, 0, 100, 100};
    auto canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas_.get());
    displayDrawable_->curCanvas_ = canvas.get();
    displayDrawable_->PrepareOffscreenRender(*displayDrawable_, false);
}

/**
 * @tc.name: PrepareOffscreenRender_NullSurface
 * @tc.desc: Test PrepareOffscreenRender when current surface is null
 * @tc.type: FUNC
 * @tc.require: issue
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, PrepareOffscreenRender_NullSurface, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(params, nullptr);
    params->frameRect_ = {0, 0, 100, 100};

    // Create canvas without surface
    auto drawingCanvas = std::make_unique<Drawing::Canvas>(100, 100);
    auto canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    displayDrawable_->curCanvas_ = canvas.get();
    displayDrawable_->PrepareOffscreenRender(*displayDrawable_, false);
}

/**
 * @tc.name: MakeBrightnessAdjustmentShader
 * @tc.desc: Test MakeBrightnessAdjustmentShader
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, MakeBrightnessAdjustmentShader, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    auto image = std::make_shared<Drawing::Image>();
    Drawing::SamplingOptions sampling;
    ASSERT_NE(displayDrawable_->MakeBrightnessAdjustmentShader(image, sampling, 0.5f), nullptr);
}

/**
 * @tc.name: MakeBrightnessAdjustmentShader001
 * @tc.desc: Test MakeBrightnessAdjustmentShader001
 * @tc.type: FUNC
 * @tc.require: issue
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, MakeBrightnessAdjustmentShader001, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    auto image = std::make_shared<Drawing::Image>();
    Drawing::SamplingOptions sampling;
    auto shader = displayDrawable_->MakeBrightnessAdjustmentShader(image, sampling, 0.8f);
    // shader may be nullptr, should not crash
}

/**
 * @tc.name: ClearTransparentBeforeSaveLayer_NullBackup
 * @tc.desc: Test ClearTransparentBeforeSaveLayer when canvasBackup_ is null
 * @tc.type: FUNC
 * @tc.require: issue
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, ClearTransparentBeforeSaveLayer_NullBackup, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);

    displayDrawable_->canvasBackup_ = nullptr;
    displayDrawable_->ClearTransparentBeforeSaveLayer();
}

/**
 * @tc.name: ClearTransparentBeforeSaveLayer_NullRenderParams
 * @tc.desc: Test ClearTransparentBeforeSaveLayer when renderParams is null
 * @tc.type: FUNC
 * @tc.require: issue
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, ClearTransparentBeforeSaveLayer_NullRenderParams, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);

    auto canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas_.get());
    displayDrawable_->canvasBackup_ = canvas.get();
    displayDrawable_->renderParams_ = nullptr;
    displayDrawable_->ClearTransparentBeforeSaveLayer();
}

/**
 * @tc.name: ClearTransparentBeforeSaveLayer_ValidParams
 * @tc.desc: Test ClearTransparentBeforeSaveLayer with valid parameters
 * @tc.type: FUNC
 * @tc.require: issue
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, ClearTransparentBeforeSaveLayer_ValidParams, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);

    auto canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas_.get());
    displayDrawable_->canvasBackup_ = canvas.get();

    // Initialize uniParam
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if (!uniParam) {
        RSUniRenderThread::Instance().Sync(std::make_unique<RSRenderThreadParams>());
    }

    displayDrawable_->ClearTransparentBeforeSaveLayer();
}

/**
 * @tc.name: FinishOffscreenRender
 * @tc.desc: Test FinishOffscreenRender
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, FinishOffscreenRender, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
    Drawing::SamplingOptions sampling;
    displayDrawable_->FinishOffscreenRender(sampling);
    ASSERT_FALSE(displayDrawable_->canvasBackup_);
}


/**
 * @tc.name: FinishOffscreenRender_NullBackup
 * @tc.desc: Test FinishOffscreenRender when canvasBackup_ is null
 * @tc.type: FUNC
 * @tc.require: issue
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, FinishOffscreenRender_NullBackup, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    Drawing::SamplingOptions sampling;
    displayDrawable_->FinishOffscreenRender(sampling);
    // Should complete without crash when canvasBackup_ is null
}

/**
 * @tc.name: FinishOffscreenRender_NullOffscreenSurface
 * @tc.desc: Test FinishOffscreenRender when offscreenSurface_ is null
 * @tc.type: FUNC
 * @tc.require: issue
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, FinishOffscreenRender_NullOffscreenSurface, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    auto canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas_.get());
    displayDrawable_->canvasBackup_ = canvas.get();
    displayDrawable_->offscreenSurface_ = nullptr;
    Drawing::SamplingOptions sampling;
    displayDrawable_->FinishOffscreenRender(sampling);
    // Should complete without crash when offscreenSurface_ is null
}

/**
 * @tc.name: FinishOffscreenRender_ScreenSurface
 * @tc.desc: Test FinishOffscreenRender when offscreenSurface_
 * @tc.type: FUNC
 * @tc.require: issue
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, FinishOffscreenRender_ScreenSurface, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    auto canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas_.get());
    displayDrawable_->canvasBackup_ = canvas.get();
    displayDrawable_->offscreenSurface_ = std::make_shared<Drawing::Surface>();
    Drawing::SamplingOptions sampling;
    displayDrawable_->FinishOffscreenRender(sampling);
    ASSERT_EQ(displayDrawable_->offscreenSurface_->GetImageSnapshot(), nullptr);
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat bitmapFormat { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_OPAQUE };
    bitmap.Build(10, 10, bitmapFormat);
    ASSERT_TRUE(displayDrawable_->offscreenSurface_->Bind(bitmap));
    displayDrawable_->FinishOffscreenRender(sampling);
    // Should complete without crash when offscreenSurface_ is null
}

/**
 * @tc.name: GetSpecialLayerType
 * @tc.desc: Test GetSpecialLayerType
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, GetSpecialLayerType, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);

    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(params, nullptr);
    int32_t result = displayDrawable_->GetSpecialLayerType(*params);
    ASSERT_EQ(result, 0);

    displayDrawable_->currentBlackList_.insert(0);
    result = displayDrawable_->GetSpecialLayerType(*params);
    ASSERT_EQ(result, 1);
    RSUniRenderThread::Instance().whiteList_.insert(0);
    result = displayDrawable_->GetSpecialLayerType(*params);
    ASSERT_EQ(result, 1);

    RSUniRenderThread::GetCaptureParam().isSnapshot_ = true;
    result = displayDrawable_->GetSpecialLayerType(*params);
    ASSERT_EQ(result, 0);
    RSUniRenderThread::GetCaptureParam().isSnapshot_ = false;
    displayDrawable_->currentBlackList_.clear();
    RSUniRenderThread::Instance().whiteList_.clear();
}

/**
 * @tc.name: UpdateSlrScale001
 * @tc.desc: Test UpdateSlrScale
 * @tc.type: FUNC
 * @tc.require: #IBIOQ4
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, UpdateSlrScale001, TestSize.Level1)
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
    // when scaleManager is nullptr
    displayDrawable_->UpdateSlrScale(screenInfo);
    ASSERT_NE(displayDrawable_->scaleManager_, nullptr);
    EXPECT_EQ(screenInfo.samplingDistance, displayDrawable_->scaleManager_->GetKernelSize());
    // when scaleManager is not nullptr
    displayDrawable_->scaleManager_ = std::make_unique<RSSLRScaleFunction>(
        screenInfo.phyWidth, screenInfo.phyHeight, screenInfo.width, screenInfo.height);
    displayDrawable_->UpdateSlrScale(screenInfo);
    ASSERT_NE(displayDrawable_->scaleManager_, nullptr);
    EXPECT_EQ(screenInfo.samplingDistance, displayDrawable_->scaleManager_->GetKernelSize());

    system::SetParameter("rosen.SLRScale.enabled", "0");
    displayDrawable_->UpdateSlrScale(screenInfo);
    EXPECT_EQ(displayDrawable_->scaleManager_, nullptr);
    system::SetParameter("rosen.SLRScale.enabled", param);
}

/**
 * @tc.name: UpdateSlrScale002
 * @tc.desc: Test UpdateSlrScale when params is not nullptr
 * @tc.type: FUNC
 * @tc.require: #IBIOQ4
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, UpdateSlrScale002, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    auto param = system::GetParameter("rosen.SLRScale.enabled", "");
    ScreenInfo screenInfo = {
        .phyWidth = DEFAULT_CANVAS_SIZE,
        .phyHeight = DEFAULT_CANVAS_SIZE,
        .width = DEFAULT_CANVAS_SIZE,
        .height = DEFAULT_CANVAS_SIZE,
        .isSamplingOn = true,
    };
    system::SetParameter("rosen.SLRScale.enabled", "1");
    // when params is not nullptr
    auto screenParams = static_cast<RSScreenRenderParams*>(screenDrawable_->GetRenderParams().get());
    displayDrawable_->UpdateSlrScale(screenInfo, screenParams);
    ASSERT_NE(displayDrawable_->scaleManager_, nullptr);
    EXPECT_EQ(screenInfo.samplingDistance, displayDrawable_->scaleManager_->GetKernelSize());
    // recover rosen.SLRScale.enabled
    system::SetParameter("rosen.SLRScale.enabled", param);
}

/**
 * @tc.name: ScaleCanvasIfNeeded
 * @tc.desc: Test ScaleCanvasIfNeeded
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, ScaleCanvasIfNeeded001, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->curCanvas_, nullptr);
    auto param = system::GetParameter("rosen.SLRScale.enabled", "");
    ScreenInfo screenInfo = {
        .phyWidth = DEFAULT_CANVAS_SIZE,
        .phyHeight = DEFAULT_CANVAS_SIZE,
        .width = DEFAULT_CANVAS_SIZE,
        .height = DEFAULT_CANVAS_SIZE,
        .isSamplingOn = false,
    };
    system::SetParameter("rosen.SLRScale.enabled", "1");
    // when scaleManager_ is nullptr
    displayDrawable_->ScaleCanvasIfNeeded(screenInfo);
    ASSERT_EQ(displayDrawable_->scaleManager_, nullptr);
    // when scaleManager is not nullptr
    displayDrawable_->scaleManager_ = std::make_unique<RSSLRScaleFunction>(
        screenInfo.phyWidth, screenInfo.phyHeight, screenInfo.width, screenInfo.height);
    displayDrawable_->ScaleCanvasIfNeeded(screenInfo);
    ASSERT_EQ(displayDrawable_->scaleManager_, nullptr);

    screenInfo.isSamplingOn = true;
    system::SetParameter("rosen.SLRScale.enabled", "0");
    displayDrawable_->ScaleCanvasIfNeeded(screenInfo);
    ASSERT_EQ(displayDrawable_->scaleManager_, nullptr);
    system::SetParameter("rosen.SLRScale.enabled", param);

    displayDrawable_->enableVisibleRect_ = true;
    displayDrawable_->ScaleCanvasIfNeeded(screenInfo);
    displayDrawable_->enableVisibleRect_ = false;
}

/**
 * @tc.name: RotateMirrorCanvas
 * @tc.desc: Test RotateMirrorCanvas
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, RotateMirrorCanvas, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);ASSERT_NE(displayDrawable_->curCanvas_, nullptr);

    ScreenRotation rotation = ScreenRotation::ROTATION_0;
    displayDrawable_->RotateMirrorCanvas(rotation, 1.f, 1.f);

    rotation = ScreenRotation::ROTATION_90;
    displayDrawable_->RotateMirrorCanvas(rotation, 1.f, -1.f);

    rotation = ScreenRotation::ROTATION_180;
    displayDrawable_->RotateMirrorCanvas(rotation, 2.f, 2.f);

    rotation = ScreenRotation::ROTATION_270;
    displayDrawable_->RotateMirrorCanvas(rotation, -1.f, 1.f);

    rotation = ScreenRotation::INVALID_SCREEN_ROTATION;
    displayDrawable_->RotateMirrorCanvas(rotation, 0, 0);
    ASSERT_TRUE(displayDrawable_->curCanvas_);
}

/**
 * @tc.name: PrepareOffscreenRenderTest001
 * @tc.desc: Test PrepareOffscreenRender, if offscreenWidth/offscreenHeight were not initialized.
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, PrepareOffscreenRenderTest001, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(renderNode_, nullptr);
    displayDrawable_->PrepareOffscreenRender(*displayDrawable_, false);

    auto type = RotateOffScreenParam::GetRotateOffScreenScreenNodeEnable();
    RotateOffScreenParam::SetRotateOffScreenDisplayNodeEnable(true);

    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    params->isRotationChanged_ = true;
    params->frameRect_ = { 0.f, 0.f, 1.f, 0.f };
    displayDrawable_->PrepareOffscreenRender(*displayDrawable_, false);
    ASSERT_TRUE(params->IsRotationChanged());

    params->frameRect_ = { 0.f, 0.f, 1.f, 1.f };
    displayDrawable_->PrepareOffscreenRender(*displayDrawable_, false);
    ASSERT_FALSE(displayDrawable_->curCanvas_->GetSurface());

    auto surface = std::make_shared<Drawing::Surface>();
    displayDrawable_->curCanvas_->surface_ = surface.get();
    displayDrawable_->PrepareOffscreenRender(*displayDrawable_, false);
    ASSERT_TRUE(displayDrawable_->curCanvas_->GetSurface());
    RotateOffScreenParam::SetRotateOffScreenDisplayNodeEnable(type);
}

/**
 * @tc.name: PrepareOffscreenRenderTest002
 * @tc.desc: Test PrepareOffscreenRender, offscreenWidth/offscreenHeight is/are correctly initialized.
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, PrepareOffscreenRenderTest002, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(renderNode_, nullptr);
    renderNode_->GetMutableRenderProperties().SetFrameWidth(DEFAULT_CANVAS_SIZE);
    renderNode_->GetMutableRenderProperties().SetFrameHeight(DEFAULT_CANVAS_SIZE);
    displayDrawable_->PrepareOffscreenRender(*displayDrawable_, false);
}

/**
 * @tc.name: PrepareOffscreenRenderTest003
 * @tc.desc: Test PrepareOffscreenRender when params is nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, PrepareOffscreenRenderTest003, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(renderNode_, nullptr);
    displayDrawable_->renderParams_ = nullptr;
    displayDrawable_->PrepareOffscreenRender(*displayDrawable_, false);
}

/**
 * @tc.name: PrepareOffscreenRenderTest004
 * @tc.desc: Test PrepareOffscreenRender when useFixedSize is true
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, PrepareOffscreenRenderTest004, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);

    // when GetRotateOffScreenDisplayNodeEnable is false
    RotateOffScreenParam::SetRotateOffScreenDisplayNodeEnable(true);
    displayDrawable_->PrepareOffscreenRender(*displayDrawable_, true);

    // when GetRotateOffScreenDisplayNodeEnable is true and IsRotationChanged is false
    RotateOffScreenParam::SetRotateOffScreenDisplayNodeEnable(true);
    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    renderParams->SetRotationChanged(false);
    displayDrawable_->PrepareOffscreenRender(*displayDrawable_, true);

    // when GetRotateOffScreenDisplayNodeEnable is true and IsRotationChanged is true
    renderParams->SetRotationChanged(true);
    displayDrawable_->PrepareOffscreenRender(*displayDrawable_, true);

    // RotateOffScreenDisplayNodeEnable is true, IsRotationChanged is true, CacheOptimizeRotateEnable is true
    bool defaultEnable = system::GetBoolParameter("const.cache.optimize.rotate.enable", false);
    system::SetParameter("const.cache.optimize.rotate.enable", "true");
    displayDrawable_->PrepareOffscreenRender(*displayDrawable_, true);
    // RotateOffScreenDisplayNodeEnable is true, IsRotationChanged is true, CacheOptimizeRotateEnable is false
    system::SetParameter("const.cache.optimize.rotate.enable", "false");
    displayDrawable_->PrepareOffscreenRender(*displayDrawable_, true);
    // RotateOffScreenDisplayNodeEnable is true, IsRotationChanged is true, CacheOptimizeRotateEnable is false
    // offscreenSurface is nullptr
    displayDrawable_->offscreenSurface_ = nullptr;
    displayDrawable_->PrepareOffscreenRender(*displayDrawable_, true);
    // RotateOffScreenDisplayNodeEnable is true, IsRotationChanged is true, CacheOptimizeRotateEnable is false
    // offscreenSurface is nullptr
    displayDrawable_->offscreenSurface_ = std::make_shared<Drawing::Surface>();
    displayDrawable_->PrepareOffscreenRender(*displayDrawable_, true);

    // recover const.optimize.rotate.enable
    system::SetParameter("const.cache.optimize.rotate.enable", defaultEnable ? "true" : "false");
}

/**
 * @tc.name: PrepareOffscreenRenderTest005
 * @tc.desc: Test PrepareOffscreenRender when isRotationChanged is true and VmaOptimizeFlag is false
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, PrepareOffscreenRenderTest005, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    renderParams->SetRotationChanged(true);
    RSUniRenderThread::Instance().vmaOptimizeFlag_ = true;
    displayDrawable_->PrepareOffscreenRender(*displayDrawable_, false);
}

/**
 * @tc.name: PrepareOffscreenRenderTest006
 * @tc.desc: Test PrepareOffscreenRender when needOffscreen_ is true
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, PrepareOffscreenRenderTest006, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    if (!RSSystemProperties::GetSurfaceOffscreenEnadbled()) {
        return;
    }
    renderParams->SetNeedOffscreen(true);
    displayDrawable_->PrepareOffscreenRender(*displayDrawable_, false);
}

/**
 * @tc.name: PrepareOffscreenRenderTest007
 * @tc.desc: Test PrepareOffscreenRender when useFixedOffscreenSurfaceSize_ is true/false
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, PrepareOffscreenRenderTest007, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    if (!RSSystemProperties::GetSurfaceOffscreenEnadbled()) {
        return;
    }
    renderParams->frameRect_ = {0, 0, 100, 100};
    renderParams->SetNeedOffscreen(true);
    // when useFixedOffscreenSurfaceSize_ is true
    displayDrawable_->useFixedOffscreenSurfaceSize_ = true;
    displayDrawable_->PrepareOffscreenRender(*displayDrawable_, false);
    // when useFixedOffscreenSurfaceSize_ is false
    displayDrawable_->useFixedOffscreenSurfaceSize_ = false;
    displayDrawable_->PrepareOffscreenRender(*displayDrawable_, false);
}

/**
 * @tc.name: PrepareOffscreenRenderTest008
 * @tc.desc: Test PrepareOffscreenRender when needOffscreen_ is offscreenSurface_ is/not nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, PrepareOffscreenRenderTest008, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    auto renderParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    if (!RSSystemProperties::GetSurfaceOffscreenEnadbled()) {
        return;
    }
    renderParams->frameRect_ = {0, 0, 100, 100};
    renderParams->SetNeedOffscreen(true);
    displayDrawable_->useFixedOffscreenSurfaceSize_ = true;
    // when offscreenSurface is nullptr
    displayDrawable_->offscreenSurface_ = nullptr;
    displayDrawable_->PrepareOffscreenRender(*displayDrawable_, false);
    // when offscreenSurface is not nullptr
    displayDrawable_->offscreenSurface_ = std::make_shared<Drawing::Surface>();;
    displayDrawable_->PrepareOffscreenRender(*displayDrawable_, false);
}

/**
 * @tc.name: PrepareOffscreenRenderTest009
 * @tc.desc: Test PrepareOffscreenRender when screenParams is/not nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, PrepareOffscreenRenderTest009, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    auto displayParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());

    auto surface = std::make_shared<Drawing::Surface>();
    displayDrawable_->curCanvas_->surface_ = surface.get();
    displayParams->frameRect_ = {0, 0, 100, 100};
    displayParams->SetAncestorScreenDrawable(nullptr);
    displayDrawable_->PrepareOffscreenRender(*displayDrawable_, false);

    auto screenDrawable = screenNode_->GetRenderDrawable();
    auto screenParam = static_cast<RSScreenRenderParams*>(screenDrawable->GetRenderParams().get());
    screenParam->SetHDRPresent(false);

    displayParams->SetAncestorScreenDrawable(screenDrawable);
    displayParams->needOffscreen_ = true;
    displayDrawable_->useFixedOffscreenSurfaceSize_ = true;
    displayDrawable_->offscreenSurface_ = std::make_shared<Drawing::Surface>();
    displayDrawable_->PrepareOffscreenRender(*displayDrawable_, false);
    EXPECT_NE(displayParams->GetAncestorScreenDrawable().lock(), nullptr);
}

/**
 * @tc.name: PrepareOffscreenRenderTest010
 * @tc.desc: Test PrepareOffscreenRender when screenParams is/not nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, PrepareOffscreenRenderTest010, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    auto displayParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());

    auto surface = std::make_shared<Drawing::Surface>();
    displayDrawable_->curCanvas_->surface_ = surface.get();
    displayParams->frameRect_ = {0, 0, 100, 100};

    auto screenDrawable = screenNode_->GetRenderDrawable();
    auto screenParam = static_cast<RSScreenRenderParams*>(screenDrawable->GetRenderParams().get());
    screenParam->SetHDRPresent(true);

    displayParams->SetAncestorScreenDrawable(screenDrawable);
    displayParams->needOffscreen_ = false;
    displayDrawable_->useFixedOffscreenSurfaceSize_ = true;
    displayDrawable_->offscreenSurface_ = std::make_shared<Drawing::Surface>();
    displayDrawable_->PrepareOffscreenRender(*displayDrawable_, false);
    EXPECT_NE(displayParams->GetAncestorScreenDrawable().lock(), nullptr);
}

/**
 * @tc.name: FinishOffscreenRender
 * @tc.desc: Test FinishOffscreenRender when canvasBackup is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, FinishOffscreenRenderTest001, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);

    Drawing::SamplingOptions sampling;
    // when canvasBackup is nullptr
    displayDrawable_->canvasBackup_ = nullptr;
    displayDrawable_->FinishOffscreenRender(sampling, false);
    EXPECT_EQ(displayDrawable_->canvasBackup_, nullptr);
    // when canvasBackup is not nullptr
    displayDrawable_->canvasBackup_ = drawingFilterCanvas_.get();
    displayDrawable_->FinishOffscreenRender(sampling, false);
    EXPECT_NE(displayDrawable_->canvasBackup_, nullptr);
}

/**
 * @tc.name: FinishOffscreenRenderTest002
 * @tc.desc: Test FinishOffscreenRender when offscreenSurface_ is/not nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, FinishOffscreenRenderTest002, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);

    displayDrawable_->canvasBackup_ = drawingFilterCanvas_.get();
    Drawing::SamplingOptions sampling;
    // when offscreenSurface_ is nullptr
    displayDrawable_->offscreenSurface_ = nullptr;
    displayDrawable_->FinishOffscreenRender(sampling, false);
    EXPECT_EQ(displayDrawable_->offscreenSurface_, nullptr);
    // when offscreenSurface_ is not nullptr
    displayDrawable_->offscreenSurface_ = std::make_shared<Drawing::Surface>();
    displayDrawable_->FinishOffscreenRender(sampling, false);
    EXPECT_NE(displayDrawable_->offscreenSurface_, nullptr);
}

/**
 * @tc.name: FinishOffscreenRenderTest003
 * @tc.desc: Test FinishOffscreenRender when offscreenSurface_ is/not nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, FinishOffscreenRenderTest003, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    displayDrawable_->canvasBackup_ = drawingFilterCanvas_.get();
    displayDrawable_->canvasBackup_->SetHDREnabledVirtualScreen(true);
    Drawing::SamplingOptions sampling;
    displayDrawable_->offscreenSurface_ = std::make_shared<Drawing::Surface>();
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat bitmapFormat { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_OPAQUE };
    bitmap.Build(10, 10, bitmapFormat);
    ASSERT_TRUE(displayDrawable_->offscreenSurface_->Bind(bitmap));
    displayDrawable_->FinishOffscreenRender(sampling, true, 0.5f);
    EXPECT_NE(displayDrawable_->offscreenSurface_, nullptr);
}

/**
 * @tc.name: FinishOffscreenRenderTest004
 * @tc.desc: Test FinishOffscreenRender
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, FinishOffscreenRenderTest004, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);

    displayDrawable_->canvasBackup_ = drawingFilterCanvas_.get();
    displayDrawable_->canvasBackup_->SetHDREnabledVirtualScreen(true);
    displayDrawable_->offscreenSurface_ = std::make_shared<Drawing::Surface>();
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat bitmapFormat { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_OPAQUE };
    bitmap.Build(10, 10, bitmapFormat);
    ASSERT_TRUE(displayDrawable_->offscreenSurface_->Bind(bitmap));
    Drawing::SamplingOptions sampling;

    displayDrawable_->FinishOffscreenRender(sampling);
    EXPECT_NE(displayDrawable_->offscreenSurface_, nullptr);
}

/**
 * @tc.name: FinishOffscreenRenderTest005
 * @tc.desc: Test FinishOffscreenRender
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, FinishOffscreenRenderTest005, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);

    displayDrawable_->canvasBackup_ = drawingFilterCanvas_.get();
    displayDrawable_->canvasBackup_->SetHDREnabledVirtualScreen(true);
    displayDrawable_->offscreenSurface_ = std::make_shared<Drawing::Surface>();
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat bitmapFormat { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_OPAQUE };
    bitmap.Build(10, 10, bitmapFormat);
    ASSERT_TRUE(displayDrawable_->offscreenSurface_->Bind(bitmap));
    Drawing::SamplingOptions sampling;

    displayDrawable_->FinishOffscreenRender(sampling, false, 0.5f);
    EXPECT_NE(displayDrawable_->offscreenSurface_, nullptr);
}

/**
 * @tc.name: MirrorRedrawDFXTest001
 * @tc.desc: Test MirrorRedrawDFX when mirrorRedraw_ has/not have value
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, MirrorRedrawDFXTest001, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);

    // when mirrorRedraw_ not have value
    displayDrawable_->MirrorRedrawDFX(true, INVALID_SCREEN_ID);
    // when mirrorRedraw_ have value and mirrorRedraw_ == mirrorRedraw
    displayDrawable_->mirrorRedraw_ = true;
    displayDrawable_->MirrorRedrawDFX(true, INVALID_SCREEN_ID);
    EXPECT_TRUE(displayDrawable_->mirrorRedraw_);
    // when mirrorRedraw_ have value and mirrorRedraw_ != mirrorRedraw
    displayDrawable_->MirrorRedrawDFX(false, INVALID_SCREEN_ID);
    EXPECT_TRUE(displayDrawable_->mirrorRedraw_);
}

/**
 * @tc.name: DrawHardwareEnabledNodesTest001
 * @tc.desc: Test DrawHardwareEnabledNodes
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawHardwareEnabledNodesTest001, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
    Drawing::Canvas canvas;
    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    displayDrawable_->DrawHardwareEnabledNodes(canvas, *params);

    auto screenDrawablePtr = params->GetAncestorScreenDrawable().lock();
    ASSERT_NE(screenDrawablePtr, nullptr);
    auto screenDrawable = static_cast<DrawableV2::RSScreenRenderNodeDrawable*>(screenDrawablePtr.get());

    ASSERT_FALSE(screenDrawable->GetRSSurfaceHandlerOnDraw()->GetBuffer());

    screenDrawable->surfaceHandler_->buffer_.buffer = SurfaceBuffer::Create();
    displayDrawable_->DrawHardwareEnabledNodes(canvas, *params);
    ASSERT_TRUE(screenDrawable->GetRSSurfaceHandlerOnDraw()->GetBuffer());

    NodeId id = 1;
    auto rsSurfaceNode = std::make_shared<RSSurfaceRenderNode>(id);
    auto drawableAdapter = RSRenderNodeDrawableAdapter::OnGenerate(rsSurfaceNode);
    id = 2;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id);
    auto drawable = RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode);
    displayDrawable_->DrawHardwareEnabledNodes(canvas, *params);

    RSUniRenderThread::Instance().uniRenderEngine_ = std::make_shared<RSRenderEngine>();
    RSUniRenderThread::Instance().uniRenderEngine_->colorFilterMode_ = ColorFilterMode::INVERT_COLOR_DISABLE_MODE;
    displayDrawable_->DrawHardwareEnabledNodes(canvas, *params);

    RSUniRenderThread::Instance().uniRenderEngine_->colorFilterMode_ = ColorFilterMode::DALTONIZATION_TRITANOMALY_MODE;
    displayDrawable_->DrawHardwareEnabledNodes(canvas, *params);
    ASSERT_TRUE(RSUniRenderThread::Instance().GetRenderEngine());
    RSUniRenderThread::Instance().uniRenderEngine_ = nullptr;
}

/**
 * @tc.name: DrawWatermarkIfNeed
 * @tc.desc: Test DrawWatermarkIfNeed
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawWatermarkIfNeedTest, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(drawingFilterCanvas_, nullptr);
    // test WatermarkFlag is false
    displayDrawable_->DrawWatermarkIfNeed(*drawingFilterCanvas_);

    // test WatermarkFlag is true and image is nullptr
    auto uniParams1 = std::make_unique<RSRenderThreadParams>();
    uniParams1->SetWatermark(true, nullptr);
    RSUniRenderThread::Instance().Sync(std::move(uniParams1));
    EXPECT_TRUE(RSUniRenderThread::Instance().GetWatermarkFlag());
    displayDrawable_->DrawWatermarkIfNeed(*drawingFilterCanvas_);

    // test WatermarkFlag is true and image is not nullptr
    auto uniParams2 = std::make_unique<RSRenderThreadParams>();
    uniParams2->SetWatermark(true, std::make_shared<Drawing::Image>());
    RSUniRenderThread::Instance().Sync(std::move(uniParams2));
    displayDrawable_->DrawWatermarkIfNeed(*drawingFilterCanvas_);
    EXPECT_NE(RSUniRenderThread::Instance().GetWatermarkImg(), nullptr);
}

/**
 * @tc.name: SetScreenRotationForPointLight
 * @tc.desc: Test SetScreenRotationForPointLight
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, SetScreenRotationForPointLight, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);

    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(params, nullptr);
    displayDrawable_->SetScreenRotationForPointLight(*params);
}

/**
 * @tc.name: DrawWatermarkIfNeed001
 * @tc.desc: Test DrawWatermarkIfNeed001
 * @tc.type: FUNC
 * @tc.require: ICMVVO
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawWatermarkIfNeed001, TestSize.Level2)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    std::string name = "virtualScreen01";
    uint32_t width = 400;
    uint32_t height = 400;
    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);
    auto screenId = screenManager->CreateVirtualScreen(name, width, height, psurface);
    ASSERT_NE(INVALID_SCREEN_ID, screenId);
    screenManager->SetDefaultScreenId(screenId);

    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->curCanvas_, nullptr);
    auto params = static_cast<RSDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_TRUE(params);
    params->SetScreenId(screenId);
    // Create waterMask pixelMap
    Media::InitializationOptions opts;
    opts.size.width = width;
    opts.size.height = height;
    opts.editable = true;
    std::shared_ptr<Media::PixelMap> pixelMap = Media::PixelMap::Create(opts);

    ASSERT_TRUE(pixelMap != nullptr);
    auto img = RSPixelMapUtil::ExtractDrawingImage(pixelMap);
    ASSERT_TRUE(img != nullptr);

    // Create canvas
    auto rsRenderThreadParams = std::make_unique<RSRenderThreadParams>();
    rsRenderThreadParams->SetWatermark(true, img);
    RSUniRenderThread::Instance().Sync(std::move(rsRenderThreadParams));

    Drawing::Canvas drawingCanvas(400, 400);
    RSPaintFilterCanvas canvas(&drawingCanvas);
    auto screen = static_cast<impl::RSScreenManager*>(screenManager.GetRefPtr())->GetScreen(screenId);
    auto screenRawPtr = static_cast<impl::RSScreen*>(screen.get());
    ASSERT_TRUE(screenRawPtr != nullptr);

    // Test0 ScreenCorrection = INVALID_SCREEN_ROTATION && ScreenRotation = ROTATION_0
    params->screenRotation_ = ScreenRotation::ROTATION_0;
    screenRawPtr->screenRotation_ = ScreenRotation::INVALID_SCREEN_ROTATION;
    displayDrawable_->DrawWatermarkIfNeed(canvas);

    // Test1 ScreenCorrection = ROTATION_0 && ScreenRotation = ROTATION_180
    params->screenRotation_ = ScreenRotation::ROTATION_180;
    screenRawPtr->screenRotation_ = ScreenRotation::ROTATION_0;
    displayDrawable_->DrawWatermarkIfNeed(canvas);

    // Test2 ScreenCorrection = ROTATION_0 && ScreenRotation = ROTATION_90
    params->screenRotation_ = ScreenRotation::ROTATION_90;
    screenRawPtr->screenRotation_ = ScreenRotation::ROTATION_0;
    displayDrawable_->DrawWatermarkIfNeed(canvas);

    // Test3 ScreenCorrection = ROTATION_270 && ScreenRotation = ROTATION_180
    params->screenRotation_ = ScreenRotation::ROTATION_180;
    screenRawPtr->screenRotation_ = ScreenRotation::ROTATION_270;
    displayDrawable_->DrawWatermarkIfNeed(canvas);

    // Reset
    rsRenderThreadParams = std::make_unique<RSRenderThreadParams>();
    rsRenderThreadParams->SetWatermark(false, nullptr);
    RSUniRenderThread::Instance().Sync(std::move(rsRenderThreadParams));
}
}