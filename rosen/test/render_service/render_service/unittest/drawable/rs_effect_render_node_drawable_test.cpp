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
#include "drawable/rs_color_picker_drawable.h"
#include "drawable/rs_effect_render_node_drawable.h"
#include "params/rs_render_thread_params.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_effect_render_node.h"
#include "pipeline/rs_render_node_gc.h"

#ifdef SUBTREE_PARALLEL_ENABLE
#include "rs_parallel_manager.h"
#include "rs_parallel_misc.h"
#include "rs_parallel_multiwin_policy.h"
#endif

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace OHOS::Rosen {
static constexpr uint64_t TEST_ID = 128;
constexpr int32_t DEFAULT_CANVAS_SIZE = 100;
constexpr NodeId DEFAULT_ID = 0xFFFF;

class RSEffectRenderNodeDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSEffectRenderNodeDrawableTest::SetUpTestCase()
{
    auto& renderNodeGC = RSRenderNodeGC::Instance();
    renderNodeGC.nodeBucket_ = std::queue<std::vector<RSRenderNode*>>();
    renderNodeGC.drawableBucket_ = std::queue<std::vector<DrawableV2::RSRenderNodeDrawableAdapter*>>();
}

void RSEffectRenderNodeDrawableTest::TearDownTestCase() {}
void RSEffectRenderNodeDrawableTest::SetUp()
{
    // set render thread param
    auto uniParams = std::make_unique<RSRenderThreadParams>();
    RSUniRenderThread::Instance().Sync(std::move(uniParams));
}
void RSEffectRenderNodeDrawableTest::TearDown() {}

#ifdef SUBTREE_PARALLEL_ENABLE
/**
 * @tc.name: OnDraw
 * @tc.desc: Test OnDraw
 * @tc.type: FUNC
 * @tc.require: #ICEF7K
 */
HWTEST_F(RSEffectRenderNodeDrawableTest, OnDrawTest, TestSize.Level1)
{
    auto effectNode = std::make_shared<RSEffectRenderNode>(DEFAULT_ID);
    ASSERT_NE(effectNode, nullptr);
    auto effectDrawable = static_cast<RSEffectRenderNodeDrawable*>(
        RSRenderNodeDrawableAdapter::OnGenerate(effectNode).get());
    ASSERT_NE(effectDrawable->renderParams_, nullptr);
    auto canvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    auto rsPaintFilterCanvas = std::make_shared<RSPaintFilterCanvas>(canvas.get());

    // default case, shouldpaint == false
    effectDrawable->OnDraw(*rsPaintFilterCanvas);

    // if should paint
    effectDrawable->renderParams_->shouldPaint_ = true;
    effectDrawable->OnDraw(*rsPaintFilterCanvas);

    RSParallelManager::Singleton().state_ = RSParallelManager::FrameType::PARALLEL;
    RSParallelManager::Singleton().workingPolicy_ = std::make_shared<RSParallelMultiwinPolicy>();
    rsPaintFilterCanvas->SetSubTreeParallelState(RSPaintFilterCanvas::SubTreeStatus::SUBTREE_QUICK_DRAW_STATE);
    effectDrawable->OnDraw(*rsPaintFilterCanvas);
}
#endif

/**
 * @tc.name: OnCaptureTest
 * @tc.desc: Test If OnCapture Can Run
 * @tc.type: FUNC
 * @tc.require: issueICF7P6
 */
HWTEST_F(RSEffectRenderNodeDrawableTest, OnCapture001, TestSize.Level1)
{
    NodeId nodeId = 1;
    auto node = std::make_shared<RSRenderNode>(nodeId);
    auto drawable = std::make_shared<RSEffectRenderNodeDrawable>(std::move(node));
    int width = 1024;
    int height = 1920;
    Drawing::Canvas canvas(width, height);
    drawable->renderParams_ = nullptr;
    drawable->OnCapture(canvas);
    ASSERT_FALSE(drawable->ShouldPaint());
    drawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    drawable->OnCapture(canvas);
    ASSERT_FALSE(drawable->ShouldPaint());
}

/**
 * @tc.name: OnCaptureTest002
 * @tc.desc: Test If OnCapture Can Run
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectRenderNodeDrawableTest, OnCaptureTest002, TestSize.Level1)
{
    // set render thread param
    auto uniParams = std::make_unique<RSRenderThreadParams>();
    uniParams->SetSecurityDisplay(true);
    RSUniRenderThread::Instance().Sync(std::move(uniParams));

    NodeId nodeId = 0;
    auto node = std::make_shared<RSRenderNode>(nodeId);
    auto drawable = std::make_shared<RSEffectRenderNodeDrawable>(std::move(node));
    ASSERT_NE(drawable, nullptr);
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
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
    drawable->OnDraw(canvas);
    ASSERT_TRUE(drawable->ShouldPaint());
    RSUniRenderThread::GetCaptureParam().endNodeId_ = INVALID_NODEID;
    RSUniRenderThread::GetCaptureParam().captureFinished_ = true;
    drawable->OnCapture(canvas);
    drawable->OnDraw(canvas);
    ASSERT_TRUE(drawable->ShouldPaint());
}

/**
 * @tc.name: GenerateEffectDataOnDemandIsBlurNotRequired
 * @tc.desc: GenerateEffectDataOnDemand IsBlurNotRequired branch
 * @tc.type: FUNC
 * @tc.require: issueICQL6P
 */
HWTEST_F(RSEffectRenderNodeDrawableTest, GenerateEffectDataOnDemandIsBlurNotRequired, TestSize.Level1)
{
    NodeId nodeId = 1;
    auto node = std::make_shared<RSRenderNode>(nodeId);
    auto drawable = std::make_shared<RSEffectRenderNodeDrawable>(std::move(node));
    int width = 1024;
    int height = 1920;
    Drawing::Canvas canvas(width, height);
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    drawable->drawCmdIndex_.backgroundFilterIndex_ = 0;
    drawable->drawCmdIndex_.childrenIndex_ = 0;
    RSEffectRenderParams params(nodeId);
    paintFilterCanvas.SetUICapture(false);
    paintFilterCanvas.SetIsParallelCanvas(false);
    params.SetHasEffectChildrenWithoutEmptyRect(false);
    EXPECT_TRUE(drawable->GenerateEffectDataOnDemand(&params, paintFilterCanvas, Drawing::Rect(), &paintFilterCanvas));

    paintFilterCanvas.SetIsParallelCanvas(true);
    params.SetHasEffectChildren(true);
    EXPECT_TRUE(drawable->GenerateEffectDataOnDemand(&params, paintFilterCanvas, Drawing::Rect(), &paintFilterCanvas));
}

/**
 * @tc.name: OnDraw002
 * @tc.desc: Test OnDraw while skip draw by white list
 * @tc.type: FUNC
 * @tc.require: issue19858
 */
HWTEST_F(RSEffectRenderNodeDrawableTest, OnDraw002, TestSize.Level2)
{
    auto effectNode = std::make_shared<RSEffectRenderNode>(DEFAULT_ID);
    ASSERT_NE(effectNode, nullptr);
    auto effectDrawable =
        static_cast<RSEffectRenderNodeDrawable*>(RSRenderNodeDrawableAdapter::OnGenerate(effectNode).get());
    effectDrawable->renderParams_->shouldPaint_ = true;

    auto params = std::make_unique<RSRenderThreadParams>();
    params->SetSecurityDisplay(true);
    RSUniRenderThread::Instance().Sync(std::move(params));
    std::unordered_set<NodeId> whiteList = {effectDrawable->nodeId_};
    RSUniRenderThread::Instance().SetWhiteList(whiteList);

    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    AutoSpecialLayerStateRecover whiteListRecover(INVALID_NODEID);
    effectDrawable->OnDraw(canvas);

    // restore
    RSUniRenderThread::Instance().Sync(std::make_unique<RSRenderThreadParams>());
}

/**
 * @tc.name: OnDraw003
 * @tc.desc: Test OnDraw while whiteList is empty
 * @tc.type: FUNC
 * @tc.require: issue20602
 */
HWTEST_F(RSEffectRenderNodeDrawableTest, OnDraw003, TestSize.Level2)
{
    auto effectNode = std::make_shared<RSEffectRenderNode>(DEFAULT_ID);
    ASSERT_NE(effectNode, nullptr);
    auto effectDrawable =
        static_cast<RSEffectRenderNodeDrawable*>(RSRenderNodeDrawableAdapter::OnGenerate(effectNode).get());
    effectDrawable->renderParams_->shouldPaint_ = true;

    auto params = std::make_unique<RSRenderThreadParams>();
    params->SetSecurityDisplay(true);
    RSUniRenderThread::Instance().Sync(std::move(params));
    RSUniRenderThread::Instance().SetWhiteList({});

    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    effectDrawable->OnDraw(canvas);
    ASSERT_FALSE(effectDrawable->SkipDrawByWhiteList(canvas));

    // restore
    RSUniRenderThread::Instance().Sync(std::make_unique<RSRenderThreadParams>());
}

/**
 * @tc.name: OnDraw004
 * @tc.desc: Test OnDraw while renderThreadParams_ is nullptr
 * @tc.type: FUNC
 * @tc.require: issue20602
 */
HWTEST_F(RSEffectRenderNodeDrawableTest, OnDraw004, TestSize.Level2)
{
    auto effectNode = std::make_shared<RSEffectRenderNode>(DEFAULT_ID);
    ASSERT_NE(effectNode, nullptr);
    auto effectDrawable =
        static_cast<RSEffectRenderNodeDrawable*>(RSRenderNodeDrawableAdapter::OnGenerate(effectNode).get());
    effectDrawable->renderParams_->shouldPaint_ = true;

    RSUniRenderThread::Instance().Sync(nullptr);
    RSUniRenderThread::Instance().SetWhiteList({});

    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    effectDrawable->OnDraw(canvas);
    ASSERT_FALSE(effectDrawable->SkipDrawByWhiteList(canvas));

    // restore
    RSUniRenderThread::Instance().Sync(std::make_unique<RSRenderThreadParams>());
}

/**
 * @tc.name: IsBlurNotRequired
 * @tc.desc: IsBlurNotRequired
 * @tc.type: FUNC
 */
HWTEST_F(RSEffectRenderNodeDrawableTest, IsBlurNotRequired, TestSize.Level1)
{
    NodeId nodeId = 1;
    auto node = std::make_shared<RSRenderNode>(nodeId);
    auto drawable = std::make_shared<RSEffectRenderNodeDrawable>(std::move(node));
    int width = 1024;
    int height = 1920;
    Drawing::Canvas canvas(width, height);
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    RSEffectRenderParams params(nodeId);

    ASSERT_TRUE(drawable->IsBlurNotRequired(&params, &paintFilterCanvas));
}

/**
 * @tc.name: GenerateEffectDataOnDemandWithColorPicker
 * @tc.desc: Test GenerateEffectDataOnDemand with ColorPickerDrawable
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectRenderNodeDrawableTest, GenerateEffectDataOnDemandWithColorPicker, TestSize.Level1)
{
    NodeId nodeId = 1;
    auto node = std::make_shared<RSRenderNode>(nodeId);
    ASSERT_NE(node, nullptr);

    // Create a ColorPicker drawable
    auto colorPickerDrawable = std::make_shared<DrawableV2::RSColorPickerDrawable>(false, nodeId);
    ASSERT_NE(colorPickerDrawable, nullptr);

    // Set it at the COLOR_PICKER slot
    node->GetDrawableVec(__func__).at(static_cast<int8_t>(RSDrawableSlot::COLOR_PICKER)) = colorPickerDrawable;

    // Create effect drawable
    auto drawable = std::make_shared<RSEffectRenderNodeDrawable>(node);
    ASSERT_NE(drawable, nullptr);

    // Set up children index to pass the initial check
    drawable->drawCmdIndex_.childrenIndex_ = 0;

    int width = 1024;
    int height = 1920;
    Drawing::Canvas canvas(width, height);
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    RSEffectRenderParams params(nodeId);

    // Set up params to pass IsBlurNotRequired check
    params.SetHasEffectChildren(false);

    Drawing::Rect bounds(0, 0, width, height);

    // Call GenerateEffectDataOnDemand - should not crash and return true
    EXPECT_TRUE(drawable->GenerateEffectDataOnDemand(&params, canvas, bounds, &paintFilterCanvas));
}
}
