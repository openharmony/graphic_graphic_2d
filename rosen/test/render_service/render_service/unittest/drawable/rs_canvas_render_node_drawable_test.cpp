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
#include "drawable/rs_canvas_render_node_drawable.h"
#include "feature/opinc/rs_opinc_manager.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/rs_canvas_render_node.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace OHOS::Rosen {
static constexpr uint64_t TEST_ID = 124;
class RSCanvasRenderNodeDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSCanvasRenderNodeDrawableTest::SetUpTestCase() {}
void RSCanvasRenderNodeDrawableTest::TearDownTestCase() {}
void RSCanvasRenderNodeDrawableTest::SetUp() {}
void RSCanvasRenderNodeDrawableTest::TearDown() {}

/**
 * @tc.name: CreateCanvasRenderNodeDrawableTest
 * @tc.desc: Test If CanvasRenderNodeDrawable Can Be Created
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST(RSCanvasRenderNodeDrawableTest, CreateCanvasRenderNodeDrawable, TestSize.Level1)
{
    NodeId nodeId = 1;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto drawable = RSCanvasRenderNodeDrawable::OnGenerate(canvasNode);
    ASSERT_NE(drawable, nullptr);
}

/**
 * @tc.name: OnDrawTest
 * @tc.desc: Test If OnDraw Can Run
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST(RSCanvasRenderNodeDrawableTest, OnDrawTest, TestSize.Level1)
{
    NodeId nodeId = 0;
    auto node = std::make_shared<RSRenderNode>(nodeId);
    auto drawable = std::make_shared<RSCanvasRenderNodeDrawable>(std::move(node));
    ASSERT_NE(drawable, nullptr);
    Drawing::Canvas canvas;
    drawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    drawable->OnDraw(canvas);
    ASSERT_TRUE(drawable->GetRenderParams());

    drawable->isOpincDropNodeExt_ = false;
    drawable->renderParams_->isOpincStateChanged_ = false;
    drawable->renderParams_->startingWindowFlag_ = false;
    drawable->OnDraw(canvas);
    ASSERT_TRUE(drawable->GetRenderParams());

    drawable->isOpDropped_ = false;
    drawable->isDrawingCacheEnabled_ = true;
    RSOpincManager::Instance().SetOPIncSwitch(false);
    drawable->drawBlurForCache_ = false;
    drawable->OnDraw(canvas);
    ASSERT_TRUE(drawable->isDrawingCacheEnabled_);

    drawable->drawBlurForCache_ = true;
    drawable->OnDraw(canvas);
    ASSERT_TRUE(drawable->isDrawingCacheEnabled_);
}

/**
 * @tc.name: OnCaptureTest
 * @tc.desc: Test If OnCapture Can Run
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST(RSCanvasRenderNodeDrawableTest, OnCaptureTest001, TestSize.Level1)
{
    // set render thread param
    auto uniParams = std::make_unique<RSRenderThreadParams>();
    uniParams->SetSecurityDisplay(true);
    RSUniRenderThread::Instance().Sync(std::move(uniParams));

    NodeId nodeId = 0;
    auto node = std::make_shared<RSRenderNode>(nodeId);
    auto drawable = std::make_shared<RSCanvasRenderNodeDrawable>(std::move(node));
    ASSERT_NE(drawable, nullptr);
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    drawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    drawable->OnCapture(canvas);
    ASSERT_FALSE(drawable->ShouldPaint());

    drawable->renderParams_->shouldPaint_ = true;
    drawable->renderParams_->contentEmpty_ = false;
    ASSERT_TRUE(drawable->ShouldPaint());
    CaptureParam param;
    param.isMirror_ = false;
    RSUniRenderThread::SetCaptureParam(param);
    drawable->OnCapture(canvas);
    ASSERT_TRUE(drawable->ShouldPaint());
}

/**
 * @tc.name: OnCaptureTest
 * @tc.desc: Test If OnCapture Can Run
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST(RSCanvasRenderNodeDrawableTest, OnCaptureTest002, TestSize.Level1)
{
    NodeId nodeId = 0;
    auto node = std::make_shared<RSRenderNode>(nodeId);
    auto drawable = std::make_shared<RSCanvasRenderNodeDrawable>(std::move(node));
    ASSERT_NE(drawable, nullptr);
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    RSUniRenderThread::GetCaptureParam().isMirror_ = true;
    drawable->renderParams_ = nullptr;
    drawable->OnCapture(canvas);
    drawable->isDrawingCacheEnabled_ = false;
    drawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    ASSERT_TRUE(drawable->GetRenderParams());
    drawable->renderParams_->shouldPaint_ = true;
    drawable->renderParams_->contentEmpty_ = false;
    ASSERT_FALSE(drawable->isDrawingCacheEnabled_);
    ASSERT_TRUE(drawable->GetRenderParams());
    drawable->OnCapture(canvas);
    ASSERT_TRUE(drawable->ShouldPaint());
    nodeId = TEST_ID;
    RSUniRenderThread::GetCaptureParam().endNodeId_ = TEST_ID;
    canvas.SetUICapture(true);
    drawable->OnCapture(canvas);
    ASSERT_TRUE(drawable->ShouldPaint());
    RSUniRenderThread::GetCaptureParam().endNodeId_ = INVALID_NODEID;
    drawable->OnCapture(canvas);
    ASSERT_TRUE(drawable->ShouldPaint());
    
    CaptureParam params;
    std::unordered_set<NodeId> whiteList = {nodeId};
    RSUniRenderThread::Instance().SetWhiteList(whiteList);
    ScreenId screenid = 1;
    params.virtualScreenId_ = screenid;
    std::unordered_map<ScreenId, bool> info;
    info[screenid] = true;
    drawable->renderParams_->SetVirtualScreenWhiteListInfo(info);
    RSUniRenderThread::SetCaptureParam(params);
    drawable->OnCapture(canvas);
}

/**
 * @tc.name: OnCaptureTest
 * @tc.desc: Test If OnCapture Can Run
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST(RSCanvasRenderNodeDrawableTest, OnCaptureTest003, TestSize.Level1)
{
    NodeId nodeId = 0;
    auto node = std::make_shared<RSRenderNode>(nodeId);
    auto drawable = std::make_shared<RSCanvasRenderNodeDrawable>(std::move(node));
    ASSERT_NE(drawable, nullptr);
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    RSUniRenderThread::GetCaptureParam().isMirror_ = true;
    drawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    drawable->renderParams_->shouldPaint_ = true;
    drawable->renderParams_->contentEmpty_ = false;
    RSRenderThreadParamsManager::Instance().renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
    ASSERT_TRUE(RSUniRenderThread::Instance().GetRSRenderThreadParams());
    drawable->OnCapture(canvas);
    ASSERT_TRUE(drawable->ShouldPaint());
    RSUniRenderThread::Instance().Sync(nullptr);
}

/**
 * @tc.name: OnDrawTest001
 * @tc.desc: Test OnDraw
 * @tc.type: FUNC
 * @tc.require: IC8TIV
 */
HWTEST(RSCanvasRenderNodeDrawableTest, OnDrawTest001, TestSize.Level1)
{
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(0);
    auto canvasDrawable = static_cast<RSCanvasRenderNodeDrawable*>(
        RSCanvasRenderNodeDrawable::OnGenerate(canvasNode));
    ASSERT_NE(canvasDrawable, nullptr);
    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(1);
    canvasDrawable->renderParams_->shouldPaint_ = true;
    canvasDrawable->renderParams_->contentEmpty_ = false;
    canvasDrawable->renderParams_->startingWindowFlag_ = false;
    canvasDrawable->SetOcclusionCullingEnabled(false);
    auto drawingCanvas = std::make_unique<Drawing::Canvas>(1, 1);
    ASSERT_TRUE(drawingCanvas);
    auto canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    ASSERT_TRUE(canvas);
    canvasDrawable->OnDraw(*canvas);
    ASSERT_NE(canvasDrawable->renderParams_, nullptr);
    canvas->SetSubTreeParallelState(RSPaintFilterCanvas::SubTreeStatus::SUBTREE_QUICK_DRAW_STATE);
    canvasDrawable->OnDraw(*canvas);
    ASSERT_NE(canvasDrawable->renderParams_, nullptr);
}

#ifdef SUBTREE_PARALLEL_ENABLE
/**
 * @tc.name: QuickDrawTest001
 * @tc.desc: Test QuickGetDrawState
 * @tc.type: FUNC
 * @tc.require: IC8TIV
 */
HWTEST(RSCanvasRenderNodeDrawableTest, QuickDrawTest001, TestSize.Level1)
{
    Drawing::Canvas canvas;
    auto pCanvas = std::make_shared<RSPaintFilterCanvas>(&canvas);
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(0);
    auto canvasDrawable = static_cast<RSCanvasRenderNodeDrawable*>(
        RSCanvasRenderNodeDrawable::OnGenerate(canvasNode));
    pCanvas->SetSubTreeParallelState(RSPaintFilterCanvas::SubTreeStatus::DEFAULT_STATE);
    canvasDrawable->QuickGetDrawState(*pCanvas);

    pCanvas->SetSubTreeParallelState(RSPaintFilterCanvas::SubTreeStatus::SUBTREE_QUICK_DRAW_STATE);
    canvasDrawable->nodeId_ = 0;
    canvasDrawable->QuickGetDrawState(*pCanvas);

    canvasDrawable->nodeId_ = 1;
    canvasDrawable->occlusionCullingEnabled_ = true;
    pCanvas->culledEntireSubtree_.insert(1);
    canvasDrawable->QuickGetDrawState(*pCanvas);
}
#endif

/**
 * @tc.name: IsUiRangeCaptureEndNodeTest
 * @tc.desc: Test IsUiRangeCaptureEndNode
 * @tc.type: FUNC
 * @tc.require: ICS709
 */
HWTEST(RSCanvasRenderNodeDrawableTest, IsUiRangeCaptureEndNodeTest, TestSize.Level1)
{
    NodeId nodeId = 3;
    auto node = std::make_shared<RSRenderNode>(nodeId);
    auto drawable = std::static_pointer_cast<DrawableV2::RSCanvasRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(node));

    CaptureParam params;
    params.endNodeId_ = INVALID_NODEID;
    RSUniRenderThread::SetCaptureParam(params);
    ASSERT_EQ(drawable->IsUiRangeCaptureEndNode(), false);

    params.endNodeId_ = 4;
    RSUniRenderThread::SetCaptureParam(params);
    ASSERT_EQ(drawable->IsUiRangeCaptureEndNode(), false);

    params.endNodeId_ = drawable->GetId();
    RSUniRenderThread::SetCaptureParam(params);
    ASSERT_EQ(drawable->IsUiRangeCaptureEndNode(), true);
}

/**
 * @tc.name: OnDrawTest002
 * @tc.desc: Test OnDraw while skip draw by white list
 * @tc.type: FUNC
 * @tc.require: issue19858
 */
HWTEST(RSCanvasRenderNodeDrawableTest, OnDrawTest002, TestSize.Level2)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto canvasDrawable = static_cast<RSCanvasRenderNodeDrawable*>(RSCanvasRenderNodeDrawable::OnGenerate(canvasNode));
    ASSERT_NE(canvasDrawable, nullptr);

    auto params = std::make_unique<RSRenderThreadParams>();
    RSUniRenderThread::Instance().Sync(std::move(params));
    RSUniRenderThread::captureParam_.isMirror_ = true;
    std::unordered_set<NodeId> whiteList = {nodeId};
    RSUniRenderThread::Instance().SetWhiteList(whiteList);

    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    AutoSpecialLayerStateRecover whiteListRecover(INVALID_NODEID);
    canvasDrawable->OnDraw(canvas);

    // restore
    RSUniRenderThread::Instance().Sync(std::make_unique<RSRenderThreadParams>());
}

/**
 * @tc.name: OnCapture004
 * @tc.desc: Test OnCapture while skip draw by white list
 * @tc.type: FUNC
 * @tc.require: issue19858
 */
HWTEST(RSCanvasRenderNodeDrawableTest, OnCapture004, TestSize.Level2)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(0);
    auto canvasDrawable = static_cast<RSCanvasRenderNodeDrawable*>(RSCanvasRenderNodeDrawable::OnGenerate(canvasNode));
    ASSERT_NE(canvasDrawable, nullptr);
    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    canvasDrawable->renderParams_->shouldPaint_ = true;

    auto params = std::make_unique<RSRenderThreadParams>();
    RSUniRenderThread::Instance().Sync(std::move(params));
    RSUniRenderThread::captureParam_.isMirror_ = true;
    std::unordered_set<NodeId> whiteList = {nodeId};
    RSUniRenderThread::Instance().SetWhiteList(whiteList);

    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    AutoSpecialLayerStateRecover whiteListRecover(INVALID_NODEID);
    canvasDrawable->OnCapture(canvas);

    // restore
    RSUniRenderThread::Instance().Sync(std::make_unique<RSRenderThreadParams>());
}

/**
 * @tc.name: OnDraw003
 * @tc.desc: Test OnDraw while isn't security display
 * @tc.type: FUNC
 * @tc.require: issue20602
 */
HWTEST(RSCanvasRenderNodeDrawableTest, OnDrawTest003, TestSize.Level2)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto canvasDrawable = static_cast<RSCanvasRenderNodeDrawable*>(RSCanvasRenderNodeDrawable::OnGenerate(canvasNode));
    ASSERT_NE(canvasDrawable, nullptr);
    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    canvasDrawable->renderParams_->shouldPaint_ = true;
    canvasDrawable->renderParams_->startingWindowFlag_ = false;

    // set render thread param
    auto uniParams = std::make_unique<RSRenderThreadParams>();
    uniParams->SetSecurityDisplay(false);
    RSUniRenderThread::Instance().Sync(std::move(uniParams));
    RSUniRenderThread::Instance().SetWhiteList({});

    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    canvasDrawable->OnDraw(canvas);
    ASSERT_FALSE(canvasDrawable->SkipDrawByWhiteList(canvas));

    // restore
    RSUniRenderThread::Instance().Sync(std::make_unique<RSRenderThreadParams>());
}
}
