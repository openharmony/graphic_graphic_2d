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
#include "memory/rs_memory_snapshot.h"

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

    DrawableV2::RSOpincDrawCache::SetOpincBlockNodeSkip(false);
    drawable->renderParams_->isOpincStateChanged_ = false;
    drawable->renderParams_->startingWindowFlag_ = false;
    drawable->OnDraw(canvas);
    ASSERT_TRUE(drawable->GetRenderParams());

    drawable->isOpDropped_ = false;
    drawable->isDrawingCacheEnabled_ = true;
    RSOpincManager::Instance().SetOPIncSwitch(false);
    drawable->SetDrawBlurForCache(false);
    drawable->OnDraw(canvas);
    ASSERT_TRUE(drawable->isDrawingCacheEnabled_);

    drawable->SetDrawBlurForCache(true);
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
    RSUniRenderThread::GetCaptureParam().captureFinished_ = true;
    drawable->OnCapture(canvas);
    ASSERT_TRUE(drawable->ShouldPaint());
    
    CaptureParam params;
    std::unordered_set<NodeId> whiteList = {nodeId};
    RSUniRenderThread::Instance().SetWhiteList(whiteList);
    ScreenId screenId = 1;
    params.virtualScreenId_ = screenId;
    std::unordered_set<ScreenId> info;
    info.insert(screenId);
    drawable->renderParams_->SetScreensWithSubTreeWhitelist(info);
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

/**
 * @tc.name: OnDraw004
 * @tc.desc: Test OnDraw while renderThreadParams_ is nullptr
 * @tc.type: FUNC
 * @tc.require: issue20602
 */
HWTEST(RSCanvasRenderNodeDrawableTest, OnDrawTest004, TestSize.Level2)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto canvasDrawable = static_cast<RSCanvasRenderNodeDrawable*>(RSCanvasRenderNodeDrawable::OnGenerate(canvasNode));
    ASSERT_NE(canvasDrawable, nullptr);
    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    canvasDrawable->renderParams_->shouldPaint_ = true;
    canvasDrawable->renderParams_->startingWindowFlag_ = false;

    // set render thread param
    RSUniRenderThread::Instance().Sync(nullptr);
    RSUniRenderThread::Instance().SetWhiteList({});

    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    canvasDrawable->renderParams_->UpdateHDRStatus(HdrStatus::HDR_EFFECT, true);
    canvasDrawable->OnDraw(canvas);
    ASSERT_FALSE(canvasDrawable->SkipDrawByWhiteList(canvas));

    // restore
    RSUniRenderThread::Instance().Sync(std::make_unique<RSRenderThreadParams>());
}

/**
 * @tc.name: OnCapture005
 * @tc.desc: Test OnCapture while renderThreadParams_ is nullptr
 * @tc.type: FUNC
 * @tc.require: issue20602
 */
HWTEST(RSCanvasRenderNodeDrawableTest, OnCapture005, TestSize.Level2)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto canvasDrawable = static_cast<RSCanvasRenderNodeDrawable*>(RSCanvasRenderNodeDrawable::OnGenerate(canvasNode));
    ASSERT_NE(canvasDrawable, nullptr);
    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    canvasDrawable->renderParams_->shouldPaint_ = true;

    RSUniRenderThread::Instance().Sync(nullptr);
    RSUniRenderThread::Instance().SetWhiteList({});

    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    canvasDrawable->OnCapture(canvas);
    ASSERT_FALSE(canvasDrawable->SkipDrawByWhiteList(canvas));

    // restore
    RSUniRenderThread::Instance().Sync(std::make_unique<RSRenderThreadParams>());
}

/**
 * @tc.name: OnDraw005
 * @tc.desc: Test OnDraw while isDrawingCacheEnabled_ is true
 * @tc.type: FUNC
 * @tc.require: issue20602
 */
HWTEST(RSCanvasRenderNodeDrawableTest, OnDrawTest005, TestSize.Level2)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto canvasDrawable = static_cast<RSCanvasRenderNodeDrawable*>(RSCanvasRenderNodeDrawable::OnGenerate(canvasNode));
    ASSERT_NE(canvasDrawable, nullptr);
    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    canvasDrawable->renderParams_->shouldPaint_ = true;
    canvasDrawable->renderParams_->startingWindowFlag_ = false;

    canvasDrawable->SetOcclusionCullingEnabled(false);
    RSRenderNodeDrawable::InitDfxForCacheInfo();

    // set render thread param
    auto uniParams = std::make_unique<RSRenderThreadParams>();
    uniParams->SetSecurityDisplay(false);
    RSUniRenderThread::Instance().Sync(std::move(uniParams));

    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    canvasDrawable->OnDraw(canvas);
    EXPECT_EQ(canvasDrawable->GetOpincDrawCache().GetNodeCacheType(), NodeStrategyType::CACHE_NONE);

    // restore
    RSUniRenderThread::Instance().Sync(std::make_unique<RSRenderThreadParams>());
}

/**
 * @tc.name: OnDrawAbnormalProcessTest
 * @tc.desc: Test OnDraw with abnormal process check
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST(RSCanvasRenderNodeDrawableTest, OnDrawAbnormalProcessTest, TestSize.Level1)
{
    NodeId nodeId = 1;
    auto node = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto drawable = std::make_shared<RSCanvasRenderNodeDrawable>(std::move(node));
    drawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    drawable->renderParams_->shouldPaint_ = true;
    drawable->renderParams_->contentEmpty_ = false;

    // Mark process as abnormal
    pid_t pid = ExtractPid(nodeId);
    MemorySnapshot::Instance().SetAbnormalProcess(pid);

    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    
    // OnDraw should return early for abnormal process
    drawable->OnDraw(canvas);
    bool isAbnormal = MemorySnapshot::Instance().IsAbnormalProcess(pid);
    ASSERT_TRUE(isAbnormal);
    
    // Clean up
    std::set<pid_t> exitedPids = {pid};
    MemorySnapshot::Instance().EraseSnapshotInfoByPid(exitedPids);
}

/**
 * @tc.name: IsBackFaceTest001
 * @tc.desc: Test IsBackFace with horizontal mirror (scaleX = -1)
 * @tc.type: FUNC
 * @tc.require: issueIXXXXX
 */
HWTEST(RSCanvasRenderNodeDrawableTest, IsBackFaceTest001, TestSize.Level1)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto canvasDrawable = static_cast<RSCanvasRenderNodeDrawable*>(RSCanvasRenderNodeDrawable::OnGenerate(canvasNode));
    ASSERT_NE(canvasDrawable, nullptr);

    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    ASSERT_NE(canvasDrawable->renderParams_, nullptr);

    Drawing::Matrix matrix;
    matrix.SetScale(-1.0f, 1.0f);
    canvasDrawable->renderParams_->SetMatrix(matrix);

    bool isBackFace = canvasDrawable->IsBackFace(matrix);
    ASSERT_TRUE(isBackFace);
}

/**
 * @tc.name: IsBackFaceTest002
 * @tc.desc: Test IsBackFace with vertical mirror (scaleY = -1)
 * @tc.type: FUNC
 * @tc.require: issueIXXXXX
 */
HWTEST(RSCanvasRenderNodeDrawableTest, IsBackFaceTest002, TestSize.Level1)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto canvasDrawable = static_cast<RSCanvasRenderNodeDrawable*>(RSCanvasRenderNodeDrawable::OnGenerate(canvasNode));
    ASSERT_NE(canvasDrawable, nullptr);

    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    ASSERT_NE(canvasDrawable->renderParams_, nullptr);

    Drawing::Matrix matrix;
    matrix.SetScale(1.0f, -1.0f);
    canvasDrawable->renderParams_->SetMatrix(matrix);

    bool isBackFace = canvasDrawable->IsBackFace(matrix);
    ASSERT_TRUE(isBackFace);
}

/**
 * @tc.name: IsBackFaceTest003
 * @tc.desc: Test IsBackFace with normal scale (scaleX = 2, scaleY = 2)
 * @tc.type: FUNC
 * @tc.require: issueIXXXXX
 */
HWTEST(RSCanvasRenderNodeDrawableTest, IsBackFaceTest003, TestSize.Level1)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto canvasDrawable = static_cast<RSCanvasRenderNodeDrawable*>(RSCanvasRenderNodeDrawable::OnGenerate(canvasNode));
    ASSERT_NE(canvasDrawable, nullptr);

    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    ASSERT_NE(canvasDrawable->renderParams_, nullptr);

    Drawing::Matrix matrix;
    matrix.SetScale(2.0f, 2.0f);
    canvasDrawable->renderParams_->SetMatrix(matrix);

    bool isBackFace = canvasDrawable->IsBackFace(matrix);
    ASSERT_FALSE(isBackFace);
}

/**
 * @tc.name: IsBackFaceTest004
 * @tc.desc: Test IsBackFace with double mirror (scaleX = -1, scaleY = -1)
 * @tc.type: FUNC
 * @tc.require: issueIXXXXX
 */
HWTEST(RSCanvasRenderNodeDrawableTest, IsBackFaceTest004, TestSize.Level1)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto canvasDrawable = static_cast<RSCanvasRenderNodeDrawable*>(RSCanvasRenderNodeDrawable::OnGenerate(canvasNode));
    ASSERT_NE(canvasDrawable, nullptr);

    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    ASSERT_NE(canvasDrawable->renderParams_, nullptr);

    Drawing::Matrix matrix;
    matrix.SetScale(-1.0f, -1.0f);
    canvasDrawable->renderParams_->SetMatrix(matrix);

    bool isBackFace = canvasDrawable->IsBackFace(matrix);
    ASSERT_FALSE(isBackFace);
}

/**
 * @tc.name: IsBackFaceTest005
 * @tc.desc: Test IsBackFace with identity matrix
 * @tc.type: FUNC
 * @tc.require: issueIXXXXX
 */
HWTEST(RSCanvasRenderNodeDrawableTest, IsBackFaceTest005, TestSize.Level1)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto canvasDrawable = static_cast<RSCanvasRenderNodeDrawable*>(RSCanvasRenderNodeDrawable::OnGenerate(canvasNode));
    ASSERT_NE(canvasDrawable, nullptr);

    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    ASSERT_NE(canvasDrawable->renderParams_, nullptr);

    Drawing::Matrix matrix;
    matrix.Reset();
    canvasDrawable->renderParams_->SetMatrix(matrix);

    bool isBackFace = canvasDrawable->IsBackFace(matrix);
    ASSERT_FALSE(isBackFace);
}

/**
 * @tc.name: IsBackFaceTest007
 * @tc.desc: Test IsBackFace with rotation transformation
 * @tc.type: FUNC
 * @tc.require: issueIXXXXX
 */
HWTEST(RSCanvasRenderNodeDrawableTest, IsBackFaceTest007, TestSize.Level1)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto canvasDrawable = static_cast<RSCanvasRenderNodeDrawable*>(RSCanvasRenderNodeDrawable::OnGenerate(canvasNode));
    ASSERT_NE(canvasDrawable, nullptr);

    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    ASSERT_NE(canvasDrawable->renderParams_, nullptr);

    Drawing::Matrix matrix;
    matrix.PostRotate(180.0f);
    canvasDrawable->renderParams_->SetMatrix(matrix);

    bool isBackFace = canvasDrawable->IsBackFace(matrix);
    ASSERT_FALSE(isBackFace);
}

/**
 * @tc.name: IsBackFaceTest008
 * @tc.desc: Test IsBackFace with skew transformation
 * @tc.type: FUNC
 * @tc.require: issueIXXXXX
 */
HWTEST(RSCanvasRenderNodeDrawableTest, IsBackFaceTest008, TestSize.Level1)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto canvasDrawable = static_cast<RSCanvasRenderNodeDrawable*>(RSCanvasRenderNodeDrawable::OnGenerate(canvasNode));
    ASSERT_NE(canvasDrawable, nullptr);

    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    ASSERT_NE(canvasDrawable->renderParams_, nullptr);

    Drawing::Matrix matrix;
    matrix.SetSkew(0.5f, 0.5f);
    canvasDrawable->renderParams_->SetMatrix(matrix);

    bool isBackFace = canvasDrawable->IsBackFace(matrix);
    ASSERT_FALSE(isBackFace);
}

/**
 * @tc.name: DoubleSidedTest001
 * @tc.desc: Test default double sided enabled value
 * @tc.type: FUNC
 * @tc.require: issueIXXXXX
 */
HWTEST(RSCanvasRenderNodeDrawableTest, DoubleSidedTest001, TestSize.Level1)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto canvasDrawable = static_cast<RSCanvasRenderNodeDrawable*>(RSCanvasRenderNodeDrawable::OnGenerate(canvasNode));
    ASSERT_NE(canvasDrawable, nullptr);

    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    ASSERT_NE(canvasDrawable->renderParams_, nullptr);

    bool isDoubleSided = canvasDrawable->renderParams_->GetDoubleSidedEnabled();
    ASSERT_TRUE(isDoubleSided);
}

/**
 * @tc.name: DoubleSidedTest002
 * @tc.desc: Test set double sided enabled to false
 * @tc.type: FUNC
 * @tc.require: issueIXXXXX
 */
HWTEST(RSCanvasRenderNodeDrawableTest, DoubleSidedTest002, TestSize.Level1)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto canvasDrawable = static_cast<RSCanvasRenderNodeDrawable*>(RSCanvasRenderNodeDrawable::OnGenerate(canvasNode));
    ASSERT_NE(canvasDrawable, nullptr);

    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    ASSERT_NE(canvasDrawable->renderParams_, nullptr);

    canvasDrawable->renderParams_->SetDoubleSidedEnabled(false);
    bool isDoubleSided = canvasDrawable->renderParams_->GetDoubleSidedEnabled();
    ASSERT_FALSE(isDoubleSided);
}

/**
 * @tc.name: DoubleSidedTest003
 * @tc.desc: Test double sided enabled with back face (should draw)
 * @tc.type: FUNC
 * @tc.require: issueIXXXXX
 */
HWTEST(RSCanvasRenderNodeDrawableTest, DoubleSidedTest003, TestSize.Level1)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto canvasDrawable = static_cast<RSCanvasRenderNodeDrawable*>(RSCanvasRenderNodeDrawable::OnGenerate(canvasNode));
    ASSERT_NE(canvasDrawable, nullptr);
    
    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    ASSERT_NE(canvasDrawable->renderParams_, nullptr);
    
    Drawing::Matrix matrix;
    matrix.SetScale(-1.0f, 1.0f);
    canvasDrawable->renderParams_->SetMatrix(matrix);
    canvasDrawable->renderParams_->SetDoubleSidedEnabled(true);
    
    bool isDoubleSided = canvasDrawable->renderParams_->GetDoubleSidedEnabled();
    bool isBackFace = canvasDrawable->IsBackFace(matrix);
    ASSERT_TRUE(isDoubleSided);
    ASSERT_TRUE(isBackFace);
}

/**
 * @tc.name: DoubleSidedTest004
 * @tc.desc: Test single sided enabled with back face (should skip)
 * @tc.type: FUNC
 * @tc.require: issueIXXXXX
 */
HWTEST(RSCanvasRenderNodeDrawableTest, DoubleSidedTest004, TestSize.Level1)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto canvasDrawable = static_cast<RSCanvasRenderNodeDrawable*>(RSCanvasRenderNodeDrawable::OnGenerate(canvasNode));
    ASSERT_NE(canvasDrawable, nullptr);
    
    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    ASSERT_NE(canvasDrawable->renderParams_, nullptr);
    
    Drawing::Matrix matrix;
    matrix.SetScale(-1.0f, 1.0f);
    canvasDrawable->renderParams_->SetMatrix(matrix);
    canvasDrawable->renderParams_->SetDoubleSidedEnabled(false);
    
    bool isDoubleSided = canvasDrawable->renderParams_->GetDoubleSidedEnabled();
    bool isBackFace = canvasDrawable->IsBackFace(matrix);
    ASSERT_FALSE(isDoubleSided);
    ASSERT_TRUE(isBackFace);
}

/**
 * @tc.name: DoubleSidedTest005
 * @tc.desc: Test single sided enabled with front face (should draw)
 * @tc.type: FUNC
 * @tc.require: issueIXXXXX
 */
HWTEST(RSCanvasRenderNodeDrawableTest, DoubleSidedTest005, TestSize.Level1)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto canvasDrawable = static_cast<RSCanvasRenderNodeDrawable*>(RSCanvasRenderNodeDrawable::OnGenerate(canvasNode));
    ASSERT_NE(canvasDrawable, nullptr);
    
    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    ASSERT_NE(canvasDrawable->renderParams_, nullptr);
    
    Drawing::Matrix matrix;
    matrix.SetScale(1.0f, 1.0f);
    canvasDrawable->renderParams_->SetMatrix(matrix);
    canvasDrawable->renderParams_->SetDoubleSidedEnabled(false);
    
    bool isDoubleSided = canvasDrawable->renderParams_->GetDoubleSidedEnabled();
    bool isBackFace = canvasDrawable->IsBackFace(matrix);
    ASSERT_FALSE(isDoubleSided);
    ASSERT_FALSE(isBackFace);
}

/**
 * @tc.name: DoubleSidedTest006
 * @tc.desc: Test SetDoubleSidedEnabled with same value (branch not taken)
 * @tc.type: FUNC
 * @tc.require: issueIXXXXX
 */
HWTEST(RSCanvasRenderNodeDrawableTest, DoubleSidedTest006, TestSize.Level1)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto canvasDrawable = static_cast<RSCanvasRenderNodeDrawable*>(RSCanvasRenderNodeDrawable::OnGenerate(canvasNode));
    ASSERT_NE(canvasDrawable, nullptr);
    
    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    ASSERT_NE(canvasDrawable->renderParams_, nullptr);
    
    canvasDrawable->renderParams_->SetDoubleSidedEnabled(true);
    ASSERT_TRUE(canvasDrawable->renderParams_->GetDoubleSidedEnabled());
    
    canvasDrawable->renderParams_->SetDoubleSidedEnabled(true);
    ASSERT_TRUE(canvasDrawable->renderParams_->GetDoubleSidedEnabled());
}

/**
 * @tc.name: IsBackFaceTest011
 * @tc.desc: Test IsBackFace with det near EPSILON boundary (slightly below -EPSILON)
 * @tc.type: FUNC
 * @tc.require: issueIXXXXX
 */
HWTEST(RSCanvasRenderNodeDrawableTest, IsBackFaceTest011, TestSize.Level1)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto canvasDrawable = static_cast<RSCanvasRenderNodeDrawable*>(RSCanvasRenderNodeDrawable::OnGenerate(canvasNode));
    ASSERT_NE(canvasDrawable, nullptr);
    
    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    ASSERT_NE(canvasDrawable->renderParams_, nullptr);
    
    Drawing::Matrix matrix;
    constexpr float slightlyBelowEpsilon = -(1e-6f) - 1e-7f;
    std::array<float, 9> values = {1.0f, 0.0f, 0.0f, 0.0f, slightlyBelowEpsilon, 0.0f, 0.0f, 0.0f, 1.0f};
    matrix.SetAll(values);
    canvasDrawable->renderParams_->SetMatrix(matrix);

    bool result = canvasDrawable->IsBackFace(matrix);
    ASSERT_TRUE(result);
}

/**
 * @tc.name: IsBackFaceTest012
 * @tc.desc: Test IsBackFace with det within EPSILON boundary (treated as front face)
 * @tc.type: FUNC
 * @tc.require: issueIXXXXX
 */
HWTEST(RSCanvasRenderNodeDrawableTest, IsBackFaceTest012, TestSize.Level1)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto canvasDrawable = static_cast<RSCanvasRenderNodeDrawable*>(RSCanvasRenderNodeDrawable::OnGenerate(canvasNode));
    ASSERT_NE(canvasDrawable, nullptr);

    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    ASSERT_NE(canvasDrawable->renderParams_, nullptr);

    Drawing::Matrix matrix;
    constexpr float withinEpsilon = -(1e-6f) + 1e-7f;
    std::array<float, 9> values = {1.0f, 0.0f, 0.0f, 0.0f, withinEpsilon, 0.0f, 0.0f, 0.0f, 1.0f};
    matrix.SetAll(values);
    canvasDrawable->renderParams_->SetMatrix(matrix);

    bool result = canvasDrawable->IsBackFace(matrix);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: BackFaceSkipTest001
 * @tc.desc: Test OnDraw skips with BACKFACE_SKIP when single sided + back face
 * @tc.type: FUNC
 * @tc.require: issueIXXXXX
 */
HWTEST(RSCanvasRenderNodeDrawableTest, BackFaceSkipTest001, TestSize.Level2)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto canvasDrawable = static_cast<RSCanvasRenderNodeDrawable*>(RSCanvasRenderNodeDrawable::OnGenerate(canvasNode));
    ASSERT_NE(canvasDrawable, nullptr);

    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    ASSERT_NE(canvasDrawable->renderParams_, nullptr);

    canvasDrawable->renderParams_->shouldPaint_ = true;
    canvasDrawable->renderParams_->contentEmpty_ = false;
    canvasDrawable->renderParams_->startingWindowFlag_ = false;

    Drawing::Matrix matrix;
    matrix.SetScale(-1.0f, 1.0f);
    canvasDrawable->renderParams_->SetMatrix(matrix);
    canvasDrawable->renderParams_->SetDoubleSidedEnabled(false);
    ASSERT_FALSE(canvasDrawable->renderParams_->GetDoubleSidedEnabled());

    auto uniParams = std::make_unique<RSRenderThreadParams>();
    uniParams->SetSecurityDisplay(false);
    RSUniRenderThread::Instance().Sync(std::move(uniParams));
    RSUniRenderThread::Instance().SetWhiteList({});

    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    canvasDrawable->OnDraw(canvas);

    ASSERT_EQ(canvasDrawable->GetDrawSkipType(), DrawSkipType::BACKFACE_SKIP);

    RSUniRenderThread::Instance().Sync(std::make_unique<RSRenderThreadParams>());
}

/**
 * @tc.name: BackFaceSkipTest002
 * @tc.desc: Test OnDraw does NOT skip when double sided + back face
 * @tc.type: FUNC
 * @tc.require: issueIXXXXX
 */
HWTEST(RSCanvasRenderNodeDrawableTest, BackFaceSkipTest002, TestSize.Level2)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto canvasDrawable = static_cast<RSCanvasRenderNodeDrawable*>(RSCanvasRenderNodeDrawable::OnGenerate(canvasNode));
    ASSERT_NE(canvasDrawable, nullptr);

    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    ASSERT_NE(canvasDrawable->renderParams_, nullptr);

    canvasDrawable->renderParams_->shouldPaint_ = true;
    canvasDrawable->renderParams_->contentEmpty_ = false;
    canvasDrawable->renderParams_->startingWindowFlag_ = false;

    Drawing::Matrix matrix;
    matrix.SetScale(-1.0f, 1.0f);
    canvasDrawable->renderParams_->SetMatrix(matrix);
    canvasDrawable->renderParams_->SetDoubleSidedEnabled(true);
    ASSERT_TRUE(canvasDrawable->renderParams_->GetDoubleSidedEnabled());

    auto uniParams = std::make_unique<RSRenderThreadParams>();
    uniParams->SetSecurityDisplay(false);
    RSUniRenderThread::Instance().Sync(std::move(uniParams));
    RSUniRenderThread::Instance().SetWhiteList({});

    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    canvasDrawable->OnDraw(canvas);

    ASSERT_NE(canvasDrawable->GetDrawSkipType(), DrawSkipType::BACKFACE_SKIP);

    RSUniRenderThread::Instance().Sync(std::make_unique<RSRenderThreadParams>());
}

/**
 * @tc.name: BackFaceSkipTest003
 * @tc.desc: Test OnDraw does NOT skip when single sided + front face
 * @tc.type: FUNC
 * @tc.require: issueIXXXXX
 */
HWTEST(RSCanvasRenderNodeDrawableTest, BackFaceSkipTest003, TestSize.Level2)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto canvasDrawable = static_cast<RSCanvasRenderNodeDrawable*>(RSCanvasRenderNodeDrawable::OnGenerate(canvasNode));
    ASSERT_NE(canvasDrawable, nullptr);

    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    ASSERT_NE(canvasDrawable->renderParams_, nullptr);

    canvasDrawable->renderParams_->shouldPaint_ = true;
    canvasDrawable->renderParams_->contentEmpty_ = false;
    canvasDrawable->renderParams_->startingWindowFlag_ = false;

    Drawing::Matrix matrix;
    matrix.SetScale(1.0f, 1.0f);
    canvasDrawable->renderParams_->SetMatrix(matrix);
    canvasDrawable->renderParams_->SetDoubleSidedEnabled(false);

    auto uniParams = std::make_unique<RSRenderThreadParams>();
    uniParams->SetSecurityDisplay(false);
    RSUniRenderThread::Instance().Sync(std::move(uniParams));
    RSUniRenderThread::Instance().SetWhiteList({});

    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    canvasDrawable->OnDraw(canvas);

    ASSERT_NE(canvasDrawable->GetDrawSkipType(), DrawSkipType::BACKFACE_SKIP);

    RSUniRenderThread::Instance().Sync(std::make_unique<RSRenderThreadParams>());
}

/**
 * @tc.name: AccumulatedMatrixTest001
 * @tc.desc: Parent scale(-1,1) + child identity → accumulated back face
 * @tc.type: FUNC
 * @tc.require: issueIXXXXX
 *
 * Scenario: Parent is mirrored horizontally, child has no local transform.
 * Canvas (parent) matrix: scale(-1, 1),  renderParams (child) matrix: identity
 * Accumulated = parent × child = scale(-1,1), det = -1 → back face
 */
HWTEST(RSCanvasRenderNodeDrawableTest, AccumulatedMatrixTest001, TestSize.Level1)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto canvasDrawable = static_cast<RSCanvasRenderNodeDrawable*>(RSCanvasRenderNodeDrawable::OnGenerate(canvasNode));
    ASSERT_NE(canvasDrawable, nullptr);

    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    ASSERT_NE(canvasDrawable->renderParams_, nullptr);

    Drawing::Matrix parentMatrix;
    parentMatrix.SetScale(-1.0f, 1.0f);
    Drawing::Matrix childMatrix;
    childMatrix.Reset();

    Drawing::Matrix accumulated = parentMatrix;
    accumulated.PreConcat(childMatrix);

    bool isBackFace = canvasDrawable->IsBackFace(accumulated);
    ASSERT_TRUE(isBackFace);
}

/**
 * @tc.name: AccumulatedMatrixTest002
 * @tc.desc: Parent identity + child scale(-1,1) → accumulated back face
 * @tc.type: FUNC
 * @tc.require: issueIXXXXX
 *
 * Scenario: Parent has no transform, child is mirrored horizontally.
 * Accumulated = identity × scale(-1,1) = scale(-1,1), det = -1 → back face
 */
HWTEST(RSCanvasRenderNodeDrawableTest, AccumulatedMatrixTest002, TestSize.Level1)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto canvasDrawable = static_cast<RSCanvasRenderNodeDrawable*>(RSCanvasRenderNodeDrawable::OnGenerate(canvasNode));
    ASSERT_NE(canvasDrawable, nullptr);

    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    ASSERT_NE(canvasDrawable->renderParams_, nullptr);

    Drawing::Matrix parentMatrix;
    parentMatrix.Reset();
    Drawing::Matrix childMatrix;
    childMatrix.SetScale(-1.0f, 1.0f);

    Drawing::Matrix accumulated = parentMatrix;
    accumulated.PreConcat(childMatrix);

    bool isBackFace = canvasDrawable->IsBackFace(accumulated);
    ASSERT_TRUE(isBackFace);
}

/**
 * @tc.name: AccumulatedMatrixTest003
 * @tc.desc: Parent scale(-1,1) + child scale(-1,1) → double flip = front face
 * @tc.type: FUNC
 * @tc.require: issueIXXXXX
 *
 * Scenario: Both parent and child are mirrored horizontally.
 * Accumulated = scale(-1,1) × scale(-1,1) = scale(1,1), det = 1 → front face
 * This is the key case the committer review caught: child alone has det=-1,
 * but parent+child cancels out.
 */
HWTEST(RSCanvasRenderNodeDrawableTest, AccumulatedMatrixTest003, TestSize.Level1)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto canvasDrawable = static_cast<RSCanvasRenderNodeDrawable*>(RSCanvasRenderNodeDrawable::OnGenerate(canvasNode));
    ASSERT_NE(canvasDrawable, nullptr);

    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    ASSERT_NE(canvasDrawable->renderParams_, nullptr);

    Drawing::Matrix parentMatrix;
    parentMatrix.SetScale(-1.0f, 1.0f);
    Drawing::Matrix childMatrix;
    childMatrix.SetScale(-1.0f, 1.0f);

    Drawing::Matrix accumulated = parentMatrix;
    accumulated.PreConcat(childMatrix);

    bool isBackFace = canvasDrawable->IsBackFace(accumulated);
    ASSERT_FALSE(isBackFace);
}

/**
 * @tc.name: AccumulatedMatrixTest004
 * @tc.desc: Parent scale(-1,1) + child scale(1,-1) → det = 1 → front face
 * @tc.type: FUNC
 * @tc.require: issueIXXXXX
 *
 * Accumulated = scale(-1,1) × scale(1,-1) = scale(-1,-1), det = (-1)*(-1) = 1 → front face
 */
HWTEST(RSCanvasRenderNodeDrawableTest, AccumulatedMatrixTest004, TestSize.Level1)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto canvasDrawable = static_cast<RSCanvasRenderNodeDrawable*>(RSCanvasRenderNodeDrawable::OnGenerate(canvasNode));
    ASSERT_NE(canvasDrawable, nullptr);

    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    ASSERT_NE(canvasDrawable->renderParams_, nullptr);

    Drawing::Matrix parentMatrix;
    parentMatrix.SetScale(-1.0f, 1.0f);
    Drawing::Matrix childMatrix;
    childMatrix.SetScale(1.0f, -1.0f);

    Drawing::Matrix accumulated = parentMatrix;
    accumulated.PreConcat(childMatrix);

    bool isBackFace = canvasDrawable->IsBackFace(accumulated);
    ASSERT_FALSE(isBackFace);
}

/**
 * @tc.name: AccumulatedMatrixTest005
 * @tc.desc: Parent scale(1,-1) + child scale(-1,1) → det = 1 → front face
 * @tc.type: FUNC
 * @tc.require: issueIXXXXX
 *
 * Same as 004 but reversed: two single-axis flips cancel each other.
 */
HWTEST(RSCanvasRenderNodeDrawableTest, AccumulatedMatrixTest005, TestSize.Level1)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto canvasDrawable = static_cast<RSCanvasRenderNodeDrawable*>(RSCanvasRenderNodeDrawable::OnGenerate(canvasNode));
    ASSERT_NE(canvasDrawable, nullptr);

    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    ASSERT_NE(canvasDrawable->renderParams_, nullptr);

    Drawing::Matrix parentMatrix;
    parentMatrix.SetScale(1.0f, -1.0f);
    Drawing::Matrix childMatrix;
    childMatrix.SetScale(-1.0f, 1.0f);

    Drawing::Matrix accumulated = parentMatrix;
    accumulated.PreConcat(childMatrix);

    bool isBackFace = canvasDrawable->IsBackFace(accumulated);
    ASSERT_FALSE(isBackFace);
}

/**
 * @tc.name: AccumulatedMatrixTest006
 * @tc.desc: Parent rotate(90) + child scale(-1,1) → accumulated back face
 * @tc.type: FUNC
 * @tc.require: issueIXXXXX
 *
 * rotate(90°): det = 1 (positive, front), scale(-1,1): det = -1
 * accumulated det = 1 × (-1) = -1 → back face
 */
HWTEST(RSCanvasRenderNodeDrawableTest, AccumulatedMatrixTest006, TestSize.Level1)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto canvasDrawable = static_cast<RSCanvasRenderNodeDrawable*>(RSCanvasRenderNodeDrawable::OnGenerate(canvasNode));
    ASSERT_NE(canvasDrawable, nullptr);

    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    ASSERT_NE(canvasDrawable->renderParams_, nullptr);

    Drawing::Matrix parentMatrix;
    parentMatrix.PostRotate(90.0f);
    Drawing::Matrix childMatrix;
    childMatrix.SetScale(-1.0f, 1.0f);

    Drawing::Matrix accumulated = parentMatrix;
    accumulated.PreConcat(childMatrix);

    bool isBackFace = canvasDrawable->IsBackFace(accumulated);
    ASSERT_TRUE(isBackFace);
}

/**
 * @tc.name: AccumulatedMatrixTest007
 * @tc.desc: Parent rotate(90) + child rotate(90) → det = 1 → front face
 * @tc.type: FUNC
 * @tc.require: issueIXXXXX
 *
 * Two 90° rotations: each has det = 1, accumulated det = 1 → front face
 */
HWTEST(RSCanvasRenderNodeDrawableTest, AccumulatedMatrixTest007, TestSize.Level1)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto canvasDrawable = static_cast<RSCanvasRenderNodeDrawable*>(RSCanvasRenderNodeDrawable::OnGenerate(canvasNode));
    ASSERT_NE(canvasDrawable, nullptr);

    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    ASSERT_NE(canvasDrawable->renderParams_, nullptr);

    Drawing::Matrix parentMatrix;
    parentMatrix.PostRotate(90.0f);
    Drawing::Matrix childMatrix;
    childMatrix.PostRotate(90.0f);

    Drawing::Matrix accumulated = parentMatrix;
    accumulated.PreConcat(childMatrix);

    bool isBackFace = canvasDrawable->IsBackFace(accumulated);
    ASSERT_FALSE(isBackFace);
}

/**
 * @tc.name: AccumulatedMatrixTest008
 * @tc.desc: Parent scale(-1,1) + child rotate(180) → det = -1 → back face
 * @tc.type: FUNC
 * @tc.require: issueIXXXXX
 *
 * scale(-1,1): det = -1, rotate(180°): det = cos²(180)+sin²(180) = 1
 * accumulated det = -1 × 1 = -1 → back face
 */
HWTEST(RSCanvasRenderNodeDrawableTest, AccumulatedMatrixTest008, TestSize.Level1)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto canvasDrawable = static_cast<RSCanvasRenderNodeDrawable*>(RSCanvasRenderNodeDrawable::OnGenerate(canvasNode));
    ASSERT_NE(canvasDrawable, nullptr);

    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    ASSERT_NE(canvasDrawable->renderParams_, nullptr);

    Drawing::Matrix parentMatrix;
    parentMatrix.SetScale(-1.0f, 1.0f);
    Drawing::Matrix childMatrix;
    childMatrix.PostRotate(180.0f);

    Drawing::Matrix accumulated = parentMatrix;
    accumulated.PreConcat(childMatrix);

    bool isBackFace = canvasDrawable->IsBackFace(accumulated);
    ASSERT_TRUE(isBackFace);
}

/**
 * @tc.name: AccumulatedMatrixTest009
 * @tc.desc: Parent skew(0.5,0.5) + child scale(-1,1) → accumulated back face
 * @tc.type: FUNC
 * @tc.require: issueIXXXXX
 *
 * skew(0.5,0.5): det = 1*1 - 0.5*0.5 = 0.75, scale(-1,1): det = -1
 * accumulated det = 0.75 × (-1) = -0.75 → back face
 */
HWTEST(RSCanvasRenderNodeDrawableTest, AccumulatedMatrixTest009, TestSize.Level1)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto canvasDrawable = static_cast<RSCanvasRenderNodeDrawable*>(RSCanvasRenderNodeDrawable::OnGenerate(canvasNode));
    ASSERT_NE(canvasDrawable, nullptr);

    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    ASSERT_NE(canvasDrawable->renderParams_, nullptr);

    Drawing::Matrix parentMatrix;
    parentMatrix.SetSkew(0.5f, 0.5f);
    Drawing::Matrix childMatrix;
    childMatrix.SetScale(-1.0f, 1.0f);

    Drawing::Matrix accumulated = parentMatrix;
    accumulated.PreConcat(childMatrix);

    bool isBackFace = canvasDrawable->IsBackFace(accumulated);
    ASSERT_TRUE(isBackFace);
}

/**
 * @tc.name: AccumulatedMatrixTest010
 * @tc.desc: Three-level: grandparent scale(-1,1) + parent scale(-1,1) + child identity
 *           → front face (two flips cancel)
 * @tc.type: FUNC
 * @tc.require: issueIXXXXX
 */
HWTEST(RSCanvasRenderNodeDrawableTest, AccumulatedMatrixTest010, TestSize.Level1)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto canvasDrawable = static_cast<RSCanvasRenderNodeDrawable*>(RSCanvasRenderNodeDrawable::OnGenerate(canvasNode));
    ASSERT_NE(canvasDrawable, nullptr);

    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    ASSERT_NE(canvasDrawable->renderParams_, nullptr);

    Drawing::Matrix grandparentMatrix;
    grandparentMatrix.SetScale(-1.0f, 1.0f);
    Drawing::Matrix parentMatrix;
    parentMatrix.SetScale(-1.0f, 1.0f);
    Drawing::Matrix childMatrix;
    childMatrix.Reset();

    Drawing::Matrix accumulated = grandparentMatrix;
    accumulated.PreConcat(parentMatrix);
    accumulated.PreConcat(childMatrix);

    bool isBackFace = canvasDrawable->IsBackFace(accumulated);
    ASSERT_FALSE(isBackFace);
}

/**
 * @tc.name: AccumulatedMatrixTest011
 * @tc.desc: Three-level: grandparent scale(-1,1) + parent rotate(45) + child identity
 *           → one flip → back face
 * @tc.type: FUNC
 * @tc.require: issueIXXXXX
 */
HWTEST(RSCanvasRenderNodeDrawableTest, AccumulatedMatrixTest011, TestSize.Level1)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto canvasDrawable = static_cast<RSCanvasRenderNodeDrawable*>(RSCanvasRenderNodeDrawable::OnGenerate(canvasNode));
    ASSERT_NE(canvasDrawable, nullptr);

    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    ASSERT_NE(canvasDrawable->renderParams_, nullptr);

    Drawing::Matrix grandparentMatrix;
    grandparentMatrix.SetScale(-1.0f, 1.0f);
    Drawing::Matrix parentMatrix;
    parentMatrix.PostRotate(45.0f);
    Drawing::Matrix childMatrix;
    childMatrix.Reset();

    Drawing::Matrix accumulated = grandparentMatrix;
    accumulated.PreConcat(parentMatrix);
    accumulated.PreConcat(childMatrix);

    bool isBackFace = canvasDrawable->IsBackFace(accumulated);
    ASSERT_TRUE(isBackFace);
}

/**
 * @tc.name: AccumulatedMatrixTest012
 * @tc.desc: Three-level: grandparent scale(-1,1) + parent scale(-1,1) + child scale(-1,1)
 *           → odd number of flips (3) → back face
 * @tc.type: FUNC
 * @tc.require: issueIXXXXX
 */
HWTEST(RSCanvasRenderNodeDrawableTest, AccumulatedMatrixTest012, TestSize.Level1)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto canvasDrawable = static_cast<RSCanvasRenderNodeDrawable*>(RSCanvasRenderNodeDrawable::OnGenerate(canvasNode));
    ASSERT_NE(canvasDrawable, nullptr);

    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    ASSERT_NE(canvasDrawable->renderParams_, nullptr);

    Drawing::Matrix grandparentMatrix;
    grandparentMatrix.SetScale(-1.0f, 1.0f);
    Drawing::Matrix parentMatrix;
    parentMatrix.SetScale(-1.0f, 1.0f);
    Drawing::Matrix childMatrix;
    childMatrix.SetScale(-1.0f, 1.0f);

    Drawing::Matrix accumulated = grandparentMatrix;
    accumulated.PreConcat(parentMatrix);
    accumulated.PreConcat(childMatrix);

    bool isBackFace = canvasDrawable->IsBackFace(accumulated);
    ASSERT_TRUE(isBackFace);
}

/**
 * @tc.name: AccumulatedMatrixTest013
 * @tc.desc: Child alone is back face, but parent cancels → front face
 * @tc.type: FUNC
 * @tc.require: issueIXXXXX
 *
 * This is the exact scenario from the committer review:
 * child scale(-1,1) has det = -1 (local back face), but parent scale(-1,1)
 * makes the accumulated det = 1 (front face). The old code using only the
 * child's local matrix would wrongly skip this node.
 */
HWTEST(RSCanvasRenderNodeDrawableTest, AccumulatedMatrixTest013, TestSize.Level1)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto canvasDrawable = static_cast<RSCanvasRenderNodeDrawable*>(RSCanvasRenderNodeDrawable::OnGenerate(canvasNode));
    ASSERT_NE(canvasDrawable, nullptr);

    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    ASSERT_NE(canvasDrawable->renderParams_, nullptr);

    Drawing::Matrix childMatrix;
    childMatrix.SetScale(-1.0f, 1.0f);
    canvasDrawable->renderParams_->SetMatrix(childMatrix);

    Drawing::Matrix parentMatrix;
    parentMatrix.SetScale(-1.0f, 1.0f);

    bool childAloneBackFace = canvasDrawable->IsBackFace(childMatrix);
    ASSERT_TRUE(childAloneBackFace);

    Drawing::Matrix accumulated = parentMatrix;
    accumulated.PreConcat(childMatrix);
    bool accumulatedBackFace = canvasDrawable->IsBackFace(accumulated);
    ASSERT_FALSE(accumulatedBackFace);
}

/**
 * @tc.name: AccumulatedMatrixTest014
 * @tc.desc: Parent with translation + child scale(-1,1) → still back face
 * @tc.type: FUNC
 * @tc.require: issueIXXXXX
 *
 * Translation does not affect the determinant.
 * translate(100,200) × scale(-1,1): det = 1 × (-1) = -1 → back face
 */
HWTEST(RSCanvasRenderNodeDrawableTest, AccumulatedMatrixTest014, TestSize.Level1)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto canvasDrawable = static_cast<RSCanvasRenderNodeDrawable*>(RSCanvasRenderNodeDrawable::OnGenerate(canvasNode));
    ASSERT_NE(canvasDrawable, nullptr);

    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    ASSERT_NE(canvasDrawable->renderParams_, nullptr);

    Drawing::Matrix parentMatrix;
    parentMatrix.PostTranslate(100.0f, 200.0f);
    Drawing::Matrix childMatrix;
    childMatrix.SetScale(-1.0f, 1.0f);

    Drawing::Matrix accumulated = parentMatrix;
    accumulated.PreConcat(childMatrix);

    bool isBackFace = canvasDrawable->IsBackFace(accumulated);
    ASSERT_TRUE(isBackFace);
}

/**
 * @tc.name: AccumulatedMatrixTest015
 * @tc.desc: Parent with translation + child rotate(45) → front face
 * @tc.type: FUNC
 * @tc.require: issueIXXXXX
 *
 * Translation and rotation both preserve det > 0 → front face.
 */
HWTEST(RSCanvasRenderNodeDrawableTest, AccumulatedMatrixTest015, TestSize.Level1)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto canvasDrawable = static_cast<RSCanvasRenderNodeDrawable*>(RSCanvasRenderNodeDrawable::OnGenerate(canvasNode));
    ASSERT_NE(canvasDrawable, nullptr);

    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    ASSERT_NE(canvasDrawable->renderParams_, nullptr);

    Drawing::Matrix parentMatrix;
    parentMatrix.PostTranslate(50.0f, 100.0f);
    Drawing::Matrix childMatrix;
    childMatrix.PostRotate(45.0f);

    Drawing::Matrix accumulated = parentMatrix;
    accumulated.PreConcat(childMatrix);

    bool isBackFace = canvasDrawable->IsBackFace(accumulated);
    ASSERT_FALSE(isBackFace);
}

/**
 * @tc.name: AccumulatedMatrixTest016
 * @tc.desc: Parent scale(2,3) + child scale(-1,1) → accumulated back face
 * @tc.type: FUNC
 * @tc.require: issueIXXXXX
 *
 * det(scale(2,3)) = 6 > 0, det(scale(-1,1)) = -1
 * accumulated det = 6 × (-1) = -6 → back face
 */
HWTEST(RSCanvasRenderNodeDrawableTest, AccumulatedMatrixTest016, TestSize.Level1)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto canvasDrawable = static_cast<RSCanvasRenderNodeDrawable*>(RSCanvasRenderNodeDrawable::OnGenerate(canvasNode));
    ASSERT_NE(canvasDrawable, nullptr);

    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    ASSERT_NE(canvasDrawable->renderParams_, nullptr);

    Drawing::Matrix parentMatrix;
    parentMatrix.SetScale(2.0f, 3.0f);
    Drawing::Matrix childMatrix;
    childMatrix.SetScale(-1.0f, 1.0f);

    Drawing::Matrix accumulated = parentMatrix;
    accumulated.PreConcat(childMatrix);

    bool isBackFace = canvasDrawable->IsBackFace(accumulated);
    ASSERT_TRUE(isBackFace);
}

/**
 * @tc.name: AccumulatedMatrixTest017
 * @tc.desc: Parent scale(2,3) + child scale(-1,-1) → accumulated front face
 * @tc.type: FUNC
 * @tc.require: issueIXXXXX
 *
 * det(scale(2,3)) = 6, det(scale(-1,-1)) = 1
 * accumulated det = 6 × 1 = 6 → front face
 */
HWTEST(RSCanvasRenderNodeDrawableTest, AccumulatedMatrixTest017, TestSize.Level1)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto canvasDrawable = static_cast<RSCanvasRenderNodeDrawable*>(RSCanvasRenderNodeDrawable::OnGenerate(canvasNode));
    ASSERT_NE(canvasDrawable, nullptr);

    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    ASSERT_NE(canvasDrawable->renderParams_, nullptr);

    Drawing::Matrix parentMatrix;
    parentMatrix.SetScale(2.0f, 3.0f);
    Drawing::Matrix childMatrix;
    childMatrix.SetScale(-1.0f, -1.0f);

    Drawing::Matrix accumulated = parentMatrix;
    accumulated.PreConcat(childMatrix);

    bool isBackFace = canvasDrawable->IsBackFace(accumulated);
    ASSERT_FALSE(isBackFace);
}

/**
 * @tc.name: BackFaceSkipTest004
 * @tc.desc: OnDraw with parent canvas scale(-1,1) + child identity → skip
 * @tc.type: FUNC
 * @tc.require: issueIXXXXX
 *
 * Simulates real rendering: canvas has parent transform, renderParams has child's local matrix.
 */
HWTEST(RSCanvasRenderNodeDrawableTest, BackFaceSkipTest004, TestSize.Level2)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto canvasDrawable = static_cast<RSCanvasRenderNodeDrawable*>(RSCanvasRenderNodeDrawable::OnGenerate(canvasNode));
    ASSERT_NE(canvasDrawable, nullptr);

    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    ASSERT_NE(canvasDrawable->renderParams_, nullptr);

    canvasDrawable->renderParams_->shouldPaint_ = true;
    canvasDrawable->renderParams_->contentEmpty_ = false;
    canvasDrawable->renderParams_->startingWindowFlag_ = false;

    Drawing::Matrix childMatrix;
    childMatrix.Reset();
    canvasDrawable->renderParams_->SetMatrix(childMatrix);
    canvasDrawable->renderParams_->SetDoubleSidedEnabled(false);

    auto uniParams = std::make_unique<RSRenderThreadParams>();
    uniParams->SetSecurityDisplay(false);
    RSUniRenderThread::Instance().Sync(std::move(uniParams));
    RSUniRenderThread::Instance().SetWhiteList({});

    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    Drawing::Matrix parentMatrix;
    parentMatrix.SetScale(-1.0f, 1.0f);
    canvas.ConcatMatrix(parentMatrix);

    canvasDrawable->OnDraw(canvas);

    ASSERT_EQ(canvasDrawable->GetDrawSkipType(), DrawSkipType::BACKFACE_SKIP);

    RSUniRenderThread::Instance().Sync(std::make_unique<RSRenderThreadParams>());
}

/**
 * @tc.name: BackFaceSkipTest005
 * @tc.desc: OnDraw with parent canvas scale(-1,1) + child scale(-1,1) → NOT skip
 * @tc.type: FUNC
 * @tc.require: issueIXXXXX
 *
 * Double flip cancels out: parent and child both mirror → accumulated det = 1 → front face.
 * The old code (using only child's local matrix) would wrongly skip this node.
 */
HWTEST(RSCanvasRenderNodeDrawableTest, BackFaceSkipTest005, TestSize.Level2)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto canvasDrawable = static_cast<RSCanvasRenderNodeDrawable*>(RSCanvasRenderNodeDrawable::OnGenerate(canvasNode));
    ASSERT_NE(canvasDrawable, nullptr);

    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    ASSERT_NE(canvasDrawable->renderParams_, nullptr);

    canvasDrawable->renderParams_->shouldPaint_ = true;
    canvasDrawable->renderParams_->contentEmpty_ = false;
    canvasDrawable->renderParams_->startingWindowFlag_ = false;

    Drawing::Matrix childMatrix;
    childMatrix.SetScale(-1.0f, 1.0f);
    canvasDrawable->renderParams_->SetMatrix(childMatrix);
    canvasDrawable->renderParams_->SetDoubleSidedEnabled(false);

    auto uniParams = std::make_unique<RSRenderThreadParams>();
    uniParams->SetSecurityDisplay(false);
    RSUniRenderThread::Instance().Sync(std::move(uniParams));
    RSUniRenderThread::Instance().SetWhiteList({});

    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    Drawing::Matrix parentMatrix;
    parentMatrix.SetScale(-1.0f, 1.0f);
    canvas.ConcatMatrix(parentMatrix);

    canvasDrawable->OnDraw(canvas);

    ASSERT_NE(canvasDrawable->GetDrawSkipType(), DrawSkipType::BACKFACE_SKIP);

    RSUniRenderThread::Instance().Sync(std::make_unique<RSRenderThreadParams>());
}

/**
 * @tc.name: BackFaceSkipTest006
 * @tc.desc: OnDraw with parent canvas rotate(45) + child scale(-1,1) → skip
 * @tc.type: FUNC
 * @tc.require: issueIXXXXX
 *
 * rotate(45°) preserves orientation (det=1), child flips (det=-1).
 * Accumulated det = -1 → back face → skip.
 */
HWTEST(RSCanvasRenderNodeDrawableTest, BackFaceSkipTest006, TestSize.Level2)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto canvasDrawable = static_cast<RSCanvasRenderNodeDrawable*>(RSCanvasRenderNodeDrawable::OnGenerate(canvasNode));
    ASSERT_NE(canvasDrawable, nullptr);

    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    ASSERT_NE(canvasDrawable->renderParams_, nullptr);

    canvasDrawable->renderParams_->shouldPaint_ = true;
    canvasDrawable->renderParams_->contentEmpty_ = false;
    canvasDrawable->renderParams_->startingWindowFlag_ = false;

    Drawing::Matrix childMatrix;
    childMatrix.SetScale(-1.0f, 1.0f);
    canvasDrawable->renderParams_->SetMatrix(childMatrix);
    canvasDrawable->renderParams_->SetDoubleSidedEnabled(false);

    auto uniParams = std::make_unique<RSRenderThreadParams>();
    uniParams->SetSecurityDisplay(false);
    RSUniRenderThread::Instance().Sync(std::move(uniParams));
    RSUniRenderThread::Instance().SetWhiteList({});

    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    Drawing::Matrix parentMatrix;
    parentMatrix.PostRotate(45.0f);
    canvas.ConcatMatrix(parentMatrix);

    canvasDrawable->OnDraw(canvas);

    ASSERT_EQ(canvasDrawable->GetDrawSkipType(), DrawSkipType::BACKFACE_SKIP);

    RSUniRenderThread::Instance().Sync(std::make_unique<RSRenderThreadParams>());
}

/**
 * @tc.name: BackFaceSkipTest007
 * @tc.desc: OnDraw with parent canvas scale(-1,1) + child rotate(30) → skip
 * @tc.type: FUNC
 * @tc.require: issueIXXXXX
 *
 * Parent flips, child rotates. rotate(30°) has det=1.
 * Accumulated det = -1 × 1 = -1 → back face → skip.
 */
HWTEST(RSCanvasRenderNodeDrawableTest, BackFaceSkipTest007, TestSize.Level2)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto canvasDrawable = static_cast<RSCanvasRenderNodeDrawable*>(RSCanvasRenderNodeDrawable::OnGenerate(canvasNode));
    ASSERT_NE(canvasDrawable, nullptr);

    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    ASSERT_NE(canvasDrawable->renderParams_, nullptr);

    canvasDrawable->renderParams_->shouldPaint_ = true;
    canvasDrawable->renderParams_->contentEmpty_ = false;
    canvasDrawable->renderParams_->startingWindowFlag_ = false;

    Drawing::Matrix childMatrix;
    childMatrix.PostRotate(30.0f);
    canvasDrawable->renderParams_->SetMatrix(childMatrix);
    canvasDrawable->renderParams_->SetDoubleSidedEnabled(false);

    auto uniParams = std::make_unique<RSRenderThreadParams>();
    uniParams->SetSecurityDisplay(false);
    RSUniRenderThread::Instance().Sync(std::move(uniParams));
    RSUniRenderThread::Instance().SetWhiteList({});

    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    Drawing::Matrix parentMatrix;
    parentMatrix.SetScale(-1.0f, 1.0f);
    canvas.ConcatMatrix(parentMatrix);

    canvasDrawable->OnDraw(canvas);

    ASSERT_EQ(canvasDrawable->GetDrawSkipType(), DrawSkipType::BACKFACE_SKIP);

    RSUniRenderThread::Instance().Sync(std::make_unique<RSRenderThreadParams>());
}

/**
 * @tc.name: BackFaceSkipTest008
 * @tc.desc: OnDraw with parent canvas translate(100,200) + child scale(-1,1) → skip
 * @tc.type: FUNC
 * @tc.require: issueIXXXXX
 *
 * Translation does not affect determinant.
 * Accumulated det = 1 × (-1) = -1 → back face → skip.
 */
HWTEST(RSCanvasRenderNodeDrawableTest, BackFaceSkipTest008, TestSize.Level2)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto canvasDrawable = static_cast<RSCanvasRenderNodeDrawable*>(RSCanvasRenderNodeDrawable::OnGenerate(canvasNode));
    ASSERT_NE(canvasDrawable, nullptr);

    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    ASSERT_NE(canvasDrawable->renderParams_, nullptr);

    canvasDrawable->renderParams_->shouldPaint_ = true;
    canvasDrawable->renderParams_->contentEmpty_ = false;
    canvasDrawable->renderParams_->startingWindowFlag_ = false;

    Drawing::Matrix childMatrix;
    childMatrix.SetScale(-1.0f, 1.0f);
    canvasDrawable->renderParams_->SetMatrix(childMatrix);
    canvasDrawable->renderParams_->SetDoubleSidedEnabled(false);

    auto uniParams = std::make_unique<RSRenderThreadParams>();
    uniParams->SetSecurityDisplay(false);
    RSUniRenderThread::Instance().Sync(std::move(uniParams));
    RSUniRenderThread::Instance().SetWhiteList({});

    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    Drawing::Matrix parentMatrix;
    parentMatrix.PostTranslate(100.0f, 200.0f);
    canvas.ConcatMatrix(parentMatrix);

    canvasDrawable->OnDraw(canvas);

    ASSERT_EQ(canvasDrawable->GetDrawSkipType(), DrawSkipType::BACKFACE_SKIP);

    RSUniRenderThread::Instance().Sync(std::make_unique<RSRenderThreadParams>());
}
} // namespace OHOS::Rosen
