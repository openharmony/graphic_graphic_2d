/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gtest/gtest.h"
#include "pipeline/layer_split/splitter/opinc/rs_opinc_layer_splitter_processor.h"
#include "pipeline/layer_split/splitter/opinc/rs_opinc_layer_splitter_planner.h"
#include "pipeline/layer_split/common/rs_layer_split_types.h"
#include "pipeline/layer_split/surface/rs_split_surface.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "drawable/rs_render_node_drawable.h"
#include "pipeline/layer_split/splitter/opinc/controller/rs_opinc_split_controller.h"
#include "pipeline/layer_split/buffer/rs_layer_split_surface_buffer.h"
#include "params/rs_surface_render_params.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace OHOS::Rosen {
class RSOpincLayerSplitterProcessorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

inline void SetupNodeGeometry(const std::shared_ptr<RSRenderNode>& node, float width, float height)
{
    node->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, width, height));
    node->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, width, height));
    node->GetRenderProperties().GetBoundsGeometry()->UpdateByMatrixFromSelf();
}

void RSOpincLayerSplitterProcessorTest::SetUpTestCase() {}
void RSOpincLayerSplitterProcessorTest::TearDownTestCase() {}
void RSOpincLayerSplitterProcessorTest::SetUp() {}
void RSOpincLayerSplitterProcessorTest::TearDown() {}

/**
 * @tc.name: Constructor001
 * @tc.desc: Check if RSOpincLayerSplitterProcessor constructor works correctly
 * @tc.type: FUNC
 */
HWTEST_F(RSOpincLayerSplitterProcessorTest, Constructor001, TestSize.Level1)
{
    RSOpincLayerSplitterProcessor processor;
    EXPECT_EQ(processor.GetPlanStatus(), PlanStatus::OFF);
    EXPECT_EQ(processor.splitSurface_, nullptr);
    EXPECT_TRUE(processor.opIncNodes_.empty());
}

/**
 * @tc.name: GetPlanStatus001
 * @tc.desc: Check if GetPlanStatus returns OFF initially
 * @tc.type: FUNC
 */
HWTEST_F(RSOpincLayerSplitterProcessorTest, GetPlanStatus001, TestSize.Level1)
{
    RSOpincLayerSplitterProcessor processor;
    EXPECT_EQ(processor.GetPlanStatus(), PlanStatus::OFF);
    processor.planStatus_ = PlanStatus::PREPARE;
    EXPECT_EQ(processor.GetPlanStatus(), PlanStatus::PREPARE);
    processor.planStatus_ = PlanStatus::ON;
    EXPECT_EQ(processor.GetPlanStatus(), PlanStatus::ON);
}


/**
 * @tc.name: Sync002
 * @tc.desc: Check if Sync works with valid planner
 * @tc.type: FUNC
 */
HWTEST_F(RSOpincLayerSplitterProcessorTest, Sync002, TestSize.Level1)
{
    RSOpincLayerSplitterProcessor processor;
    auto planner = std::make_shared<RSOpincLayerSplitterPlanner>();
    planner->planStatus_ = PlanStatus::PREPARE;
    processor.Sync(planner);
    EXPECT_EQ(processor.planStatus_, PlanStatus::PREPARE);
    EXPECT_EQ(processor.srcRect_, planner->srcRect_);
    EXPECT_EQ(processor.dstRect_, planner->dstRect_);
}

/**
 * @tc.name: Sync003
 * @tc.desc: Check if Sync works when planStatus is LEAVE
 * @tc.type: FUNC
 */
HWTEST_F(RSOpincLayerSplitterProcessorTest, Sync003, TestSize.Level1)
{
    RSOpincLayerSplitterProcessor processor;
    processor.opIncNodes_.insert(1);
    processor.opIncNodes_.insert(2);

    auto planner = std::make_shared<RSOpincLayerSplitterPlanner>();
    planner->planStatus_ = PlanStatus::LEAVE;
    processor.Sync(planner);

    EXPECT_EQ(processor.planStatus_, PlanStatus::LEAVE);
    EXPECT_TRUE(processor.opIncNodes_.empty());
}

/**
 * @tc.name: RequestFrame001
 * @tc.desc: Check if RequestFrame works correctly
 * @tc.type: FUNC
 */
HWTEST_F(RSOpincLayerSplitterProcessorTest, RequestFrame001, TestSize.Level1)
{
    RSOpincLayerSplitterProcessor processor;
    RSSurfaceRenderParams params(0);
    processor.RequestFrame(params);
    EXPECT_EQ(processor.planStatus_, PlanStatus::OFF);
}

/**
 * @tc.name: RequestFrame002
 * @tc.desc: Check if RequestFrame works with splitSurface set
 * @tc.type: FUNC
 */
HWTEST_F(RSOpincLayerSplitterProcessorTest, RequestFrame002, TestSize.Level1)
{
    RSOpincLayerSplitterProcessor processor;
    processor.splitSurface_ = std::make_shared<SplitSurface>(100, 100);
    auto drawingCanvas = std::make_unique<Drawing::Canvas>(100, 100);
    processor.splitSurface_->splitCanvas_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    
    RSSurfaceRenderParams params(0);
    processor.RequestFrame(params);
    EXPECT_NE(processor.splitSurface_, nullptr);
    EXPECT_NE(processor.splitSurface_->splitCanvas_, nullptr);
}

/**
 * @tc.name: DrawDfx001
 * @tc.desc: Check if DrawDfx works correctly
 * @tc.type: FUNC
 */
HWTEST_F(RSOpincLayerSplitterProcessorTest, DrawDfx001, TestSize.Level1)
{
    RSOpincLayerSplitterProcessor processor;
    processor.planStatus_ = PlanStatus::OFF;
    processor.DrawDfx();
    EXPECT_EQ(processor.planStatus_, PlanStatus::OFF);
}

/**
 * @tc.name: DrawDfx002
 * @tc.desc: Check if DrawDfx works when planStatus is PREPARE
 * @tc.type: FUNC
 */
HWTEST_F(RSOpincLayerSplitterProcessorTest, DrawDfx002, TestSize.Level1)
{
    RSOpincLayerSplitterProcessor processor;
    processor.planStatus_ = PlanStatus::PREPARE;
    processor.splitSurface_ = std::make_shared<SplitSurface>(100, 100);
    processor.DrawDfx();
    EXPECT_EQ(processor.planStatus_, PlanStatus::PREPARE);
    EXPECT_NE(processor.splitSurface_, nullptr);
}

/**
 * @tc.name: NeedDrawSplitCanvas001
 * @tc.desc: Check if NeedDrawSplitCanvas returns false when planStatus is OFF
 * @tc.type: FUNC
 */
HWTEST_F(RSOpincLayerSplitterProcessorTest, NeedDrawSplitCanvas001, TestSize.Level1)
{
    RSOpincLayerSplitterProcessor processor;
    processor.planStatus_ = PlanStatus::OFF;
    processor.opIncNodes_.insert(1);
    Drawing::Canvas canvas(100, 100);
    bool result = processor.NeedDrawSplitCanvas(canvas, 1);
    EXPECT_FALSE(result);
    EXPECT_EQ(processor.planStatus_, PlanStatus::OFF);
}

/**
 * @tc.name: NeedDrawSplitCanvas002
 * @tc.desc: Check if NeedDrawSplitCanvas returns true when planStatus is PREPARE and node is in opIncNodes
 * @tc.type: FUNC
 */
HWTEST_F(RSOpincLayerSplitterProcessorTest, NeedDrawSplitCanvas002, TestSize.Level1)
{
    RSOpincLayerSplitterProcessor processor;
    processor.planStatus_ = PlanStatus::PREPARE;
    processor.opIncNodes_.insert(1);
    processor.splitSurface_ = std::make_shared<SplitSurface>(100, 100);
    auto drawingCanvas = std::make_unique<Drawing::Canvas>(100, 100);
    processor.splitSurface_->splitCanvas_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());

    Drawing::Canvas canvas(100, 100);
    bool result = processor.NeedDrawSplitCanvas(canvas, 1);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: NeedDrawSplitCanvas003
 * @tc.desc: Check if NeedDrawSplitCanvas returns false when node is not in opIncNodes
 * @tc.type: FUNC
 */
HWTEST_F(RSOpincLayerSplitterProcessorTest, NeedDrawSplitCanvas003, TestSize.Level1)
{
    RSOpincLayerSplitterProcessor processor;
    processor.planStatus_ = PlanStatus::PREPARE;
    processor.opIncNodes_.insert(1);

    Drawing::Canvas canvas(100, 100);
    bool result = processor.NeedDrawSplitCanvas(canvas, 2);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: CanSkipOpIncNodeDraw001
 * @tc.desc: Check if CanSkipOpIncNodeDraw returns false when splitSurface is null
 * @tc.type: FUNC
 */
HWTEST_F(RSOpincLayerSplitterProcessorTest, CanSkipOpIncNodeDraw001, TestSize.Level1)
{
    RSOpincLayerSplitterProcessor processor;
    processor.planStatus_ = PlanStatus::ON;
    processor.opIncNodes_.insert(1);
    bool result = processor.CanSkipOpIncNodeDraw(1);
    EXPECT_FALSE(result);
    EXPECT_EQ(processor.planStatus_, PlanStatus::ON);
}

/**
 * @tc.name: CanSkipOpIncNodeDraw002
 * @tc.desc: Check if CanSkipOpIncNodeDraw returns false when node is not in opIncNodes
 * @tc.type: FUNC
 */
HWTEST_F(RSOpincLayerSplitterProcessorTest, CanSkipOpIncNodeDraw002, TestSize.Level1)
{
    RSOpincLayerSplitterProcessor processor;
    processor.splitSurface_ = std::make_shared<SplitSurface>(100, 100);
    processor.opIncNodes_.insert(1);

    bool result = processor.CanSkipOpIncNodeDraw(2);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: CanSkipOpIncNodeDraw003
 * @tc.desc: Check if CanSkipOpIncNodeDraw returns true when planStatus is ON and node is in opIncNodes
 * @tc.type: FUNC
 */
HWTEST_F(RSOpincLayerSplitterProcessorTest, CanSkipOpIncNodeDraw003, TestSize.Level1)
{
    RSOpincLayerSplitterProcessor processor;
    processor.splitSurface_ = std::make_shared<SplitSurface>(100, 100);
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(0);
    SetupNodeGeometry(surfaceNode, 100, 100);
    auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
    processor.splitSurface_->splitSurfaceDrawable_ = surfaceDrawable;
    processor.opIncNodes_.insert(1);
    processor.planStatus_ = PlanStatus::ON;

    bool result = processor.CanSkipOpIncNodeDraw(1);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: CalSplitCanvasMatrix001
 * @tc.desc: Check if CalSplitCanvasMatrix works correctly when splitSurface is null
 * @tc.type: FUNC
 */
HWTEST_F(RSOpincLayerSplitterProcessorTest, CalSplitCanvasMatrix001, TestSize.Level1)
{
    RSOpincLayerSplitterProcessor processor;
    processor.srcRect_ = RectF(0, 0, 100, 100);
    processor.dstRect_ = RectF(0, 0, 100, 100);
    Drawing::Canvas canvas(100, 100);
    processor.CalSplitCanvasMatrix(canvas, 0);
    EXPECT_EQ(processor.srcRect_, RectF(0, 0, 100, 100));
}

/**
 * @tc.name: CalSplitCanvasMatrix002
 * @tc.desc: Check if CalSplitCanvasMatrix works correctly with splitSurface and splitCanvas set
 * @tc.type: FUNC
 */
HWTEST_F(RSOpincLayerSplitterProcessorTest, CalSplitCanvasMatrix002, TestSize.Level1)
{
    RSOpincLayerSplitterProcessor processor;
    processor.splitSurface_ = std::make_shared<SplitSurface>(100, 100);
    auto drawingCanvas = std::make_unique<Drawing::Canvas>(100, 100);
    processor.splitSurface_->splitCanvas_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    processor.srcRect_ = RectF(10, 10, 50, 50);
    processor.dstRect_ = RectF(0, 0, 100, 100);

    Drawing::Canvas canvas(100, 100);
    processor.CalSplitCanvasMatrix(canvas, 0);
    EXPECT_NE(processor.splitSurface_, nullptr);
    EXPECT_NE(processor.splitSurface_->splitCanvas_, nullptr);
}

/**
 * @tc.name: GetSplitCanvas001
 * @tc.desc: Check if GetSplitCanvas returns null when splitSurface is null
 * @tc.type: FUNC
 */
HWTEST_F(RSOpincLayerSplitterProcessorTest, GetSplitCanvas001, TestSize.Level1)
{
    RSOpincLayerSplitterProcessor processor;
    processor.splitSurface_ = nullptr;
    auto canvas = processor.GetSplitCanvas();
    EXPECT_EQ(canvas, nullptr);
    EXPECT_EQ(processor.splitSurface_, nullptr);
}

/**
 * @tc.name: GetSplitCanvas002
 * @tc.desc: Check if GetSplitCanvas returns splitCanvas when splitSurface is set
 * @tc.type: FUNC
 */
HWTEST_F(RSOpincLayerSplitterProcessorTest, GetSplitCanvas002, TestSize.Level1)
{
    RSOpincLayerSplitterProcessor processor;
    processor.splitSurface_ = std::make_shared<SplitSurface>(100, 100);
    auto drawingCanvas = std::make_unique<Drawing::Canvas>(100, 100);
    processor.splitSurface_->splitCanvas_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());

    auto canvas = processor.GetSplitCanvas();
    EXPECT_NE(canvas, nullptr);
}

/**
 * @tc.name: RecordNodeWithCacheImage001
 * @tc.desc: Check if RecordNodeWithCacheImage works correctly when planStatus is not OFF
 * @tc.type: FUNC
 */
HWTEST_F(RSOpincLayerSplitterProcessorTest, RecordNodeWithCacheImage001, TestSize.Level1)
{
    RSOpincLayerSplitterProcessor processor;
    processor.planStatus_ = PlanStatus::PREPARE;
    processor.opIncNodes_.insert(1);
    processor.RecordNodeWithCacheImage(1);
    EXPECT_EQ(processor.planStatus_, PlanStatus::PREPARE);
    EXPECT_FALSE(processor.opIncNodes_.empty());
}


/**
 * @tc.name: RecordNodeWithCacheImage003
 * @tc.desc: Check if RecordNodeWithCacheImage returns when node is not in opIncNodes
 * @tc.type: FUNC
 */
HWTEST_F(RSOpincLayerSplitterProcessorTest, RecordNodeWithCacheImage003, TestSize.Level1)
{
    RSOpincLayerSplitterProcessor processor;
    processor.planStatus_ = PlanStatus::OFF;
    processor.opIncNodes_.insert(1);

    processor.RecordNodeWithCacheImage(2);

    EXPECT_FALSE(processor.opIncNodes_.empty());
}

/**
 * @tc.name: Sync_NullPlanner
 * @tc.desc: Test Sync when layerSplitterPlanner is nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterProcessorTest, Sync_NullPlanner, TestSize.Level1)
{
    // Test Case: layerSplitterPlanner == nullptr
    RSOpincLayerSplitterProcessor processor;
    processor.planStatus_ = PlanStatus::PREPARE;
    processor.Sync(nullptr);
    EXPECT_EQ(processor.planStatus_, PlanStatus::OFF);
}

/**
 * @tc.name: Sync_CheckNeedRequest
 * @tc.desc: Test Sync when requestController_ exists and CheckNeedRequest returns true
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterProcessorTest, Sync_CheckNeedRequest, TestSize.Level1)
{
    // Test Case: requestController_ && requestController_->CheckNeedRequest() == true
    RSOpincLayerSplitterProcessor processor;
    auto planner = std::make_shared<RSOpincLayerSplitterPlanner>();
    planner->planStatus_ = PlanStatus::PREPARE;

    // Create a requestController that returns true for CheckNeedRequest
    auto controller = std::make_shared<RequestController>();
    controller->Update(false, PlanStatus::PREPARE, false, false);
    planner->requestController_ = controller;

    // Insert opIncNodes into planner
    planner->opIncNodes_.clear();
    planner->opIncNodes_.push_back({1, Vector4f()});
    planner->opIncNodes_.push_back({2, Vector4f()});

    processor.Sync(planner);
    EXPECT_EQ(processor.planStatus_, PlanStatus::PREPARE);
    EXPECT_EQ(processor.opIncNodes_.size(), 2);
    EXPECT_NE(processor.opIncNodes_.find(1), processor.opIncNodes_.end());
    EXPECT_NE(processor.opIncNodes_.find(2), processor.opIncNodes_.end());
}

/**
 * @tc.name: RequestFrame_SplitSurfaceNull
 * @tc.desc: Test RequestFrame when splitSurface_ is nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterProcessorTest, RequestFrame_SplitSurfaceNull, TestSize.Level1)
{
    // Test Case: !splitSurface_ returns early (splitSurface_ is null)
    RSOpincLayerSplitterProcessor processor;
    auto controller = std::make_shared<RequestController>();
    controller->Update(false, PlanStatus::PREPARE, false, false);
    processor.requestController_ = controller;
    processor.splitSurface_ = nullptr;

    RSSurfaceRenderParams params(0);
    processor.RequestFrame(params);
    EXPECT_EQ(processor.splitSurface_, nullptr);
}

/**
 * @tc.name: RequestFrame_SplitSurfaceOK_HappyPath
 * @tc.desc: Test RequestFrame when splitSurface_ is not null and RequestFrame is called
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterProcessorTest, RequestFrame_SplitSurfaceOK_HappyPath, TestSize.Level1)
{
    // Test Case: splitSurface_ not null -> calls splitSurface_->RequestFrame(params) and DrawDfx
    RSOpincLayerSplitterProcessor processor;
    auto controller = std::make_shared<RequestController>();
    controller->Update(false, PlanStatus::PREPARE, false, false);
    processor.requestController_ = controller;
    processor.splitSurface_ = std::make_shared<SplitSurface>(100, 100);
    processor.splitSurface_->splitSurfaceBuffer_ =
        std::make_unique<RSSplitSurfaceBuffer>("test", 0, 100, 100);

    RSSurfaceRenderParams params(0);
    processor.RequestFrame(params);
    EXPECT_NE(processor.splitSurface_, nullptr);
}

/**
 * @tc.name: DrawDfx_PlanStatusOn_SplitSurfaceNull
 * @tc.desc: Test DrawDfx when drawDfxEnabled=true, planStatus=ON, splitSurface_=nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterProcessorTest, DrawDfx_PlanStatusOn_SplitSurfaceNull, TestSize.Level1)
{
    // Test Case: drawDfxEnabled_=true, planStatus_==PlanStatus::ON, splitSurface_==nullptr
    RSOpincLayerSplitterProcessor processor;
    processor.drawDfxEnabled_ = true;
    processor.planStatus_ = PlanStatus::ON;
    processor.splitSurface_ = nullptr;
    processor.DrawDfx();
    EXPECT_EQ(processor.splitSurface_, nullptr);
}

/**
 * @tc.name: DrawDfx_PlanStatusOn_SplitSurfaceOK
 * @tc.desc: Test DrawDfx when drawDfxEnabled=true, planStatus=ON, splitSurface_ not null
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterProcessorTest, DrawDfx_PlanStatusOn_SplitSurfaceOK, TestSize.Level1)
{
    // Test Case: drawDfxEnabled_=true, planStatus_==PlanStatus::ON, splitSurface_!=nullptr
    RSOpincLayerSplitterProcessor processor;
    processor.drawDfxEnabled_ = true;
    processor.planStatus_ = PlanStatus::ON;
    processor.splitSurface_ = std::make_shared<SplitSurface>(100, 100);

    processor.DrawDfx();
    EXPECT_EQ(processor.planStatus_, PlanStatus::ON);
    EXPECT_NE(processor.splitSurface_, nullptr);
}

/**
 * @tc.name: RecordNodeWithCacheImage_FoundAndEmpty
 * @tc.desc: Test RecordNodeWithCacheImage when node found and opIncNodes_ becomes empty
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterProcessorTest, RecordNodeWithCacheImage_FoundAndEmpty, TestSize.Level1)
{
    // Test Case: node found, after erase opIncNodes_ becomes empty -> calls FlushFrame
    RSOpincLayerSplitterProcessor processor;
    processor.planStatus_ = PlanStatus::OFF;
    auto controller = std::make_shared<RequestController>();
    controller->Update(false, PlanStatus::PREPARE, false, false);
    processor.requestController_ = controller;
    processor.opIncNodes_.insert(1);
    processor.splitSurface_ = std::make_shared<SplitSurface>(100, 100);
    processor.splitSurface_->splitSurfaceBuffer_ =
        std::make_unique<RSSplitSurfaceBuffer>("test", 0, 100, 100);

    processor.RecordNodeWithCacheImage(1);
    EXPECT_TRUE(processor.opIncNodes_.empty());
}

/**
 * @tc.name: RecordNodeWithCacheImage_FoundNotEmpty
 * @tc.desc: Test RecordNodeWithCacheImage when node found and opIncNodes_ not empty after erase
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterProcessorTest, RecordNodeWithCacheImage_FoundNotEmpty, TestSize.Level1)
{
    // Test Case: node found, after erase opIncNodes_ still has other nodes
    RSOpincLayerSplitterProcessor processor;
    processor.planStatus_ = PlanStatus::OFF;
    auto controller = std::make_shared<RequestController>();
    controller->Update(false, PlanStatus::PREPARE, false, false);
    processor.requestController_ = controller;
    processor.opIncNodes_.insert(1);
    processor.opIncNodes_.insert(2);
    processor.splitSurface_ = std::make_shared<SplitSurface>(100, 100);

    processor.RecordNodeWithCacheImage(1);
    EXPECT_FALSE(processor.opIncNodes_.empty());
    EXPECT_EQ(processor.opIncNodes_.size(), 1);
    EXPECT_NE(processor.opIncNodes_.find(2), processor.opIncNodes_.end());
}

/**
 * @tc.name: CanSkipOpIncNodeDraw_NodeNotInSet
 * @tc.desc: Test CanSkipOpIncNodeDraw when nodeId not found in opIncNodes_
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterProcessorTest, CanSkipOpIncNodeDraw_NodeNotInSet, TestSize.Level1)
{
    // Test Case: opIncNodes_.find(nodeId) == opIncNodes_.end()
    RSOpincLayerSplitterProcessor processor;
    processor.splitSurface_ = std::make_shared<SplitSurface>(100, 100);
    processor.opIncNodes_.insert(1);

    bool result = processor.CanSkipOpIncNodeDraw(2);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: CanSkipOpIncNodeDraw_PlanPrepare_HardwareTrue
 * @tc.desc: Test CanSkipOpIncNodeDraw when planStatus=PREPARE and GetHardwareEnabled=true
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterProcessorTest, CanSkipOpIncNodeDraw_PlanPrepare_HardwareTrue, TestSize.Level1)
{
    // Test Case: planStatus_==PREPARE, GetHardwareEnabled()==true
    RSOpincLayerSplitterProcessor processor;
    processor.splitSurface_ = std::make_shared<SplitSurface>(100, 100);
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(0);
    SetupNodeGeometry(surfaceNode, 100, 100);
    auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
    auto renderParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable->GetRenderParams().get());
    renderParams->SetHardwareEnabled(true);
    processor.splitSurface_->splitSurfaceDrawable_ = surfaceDrawable;
    processor.opIncNodes_.insert(1);
    processor.planStatus_ = PlanStatus::PREPARE;

    bool result = processor.CanSkipOpIncNodeDraw(1);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: CanSkipOpIncNodeDraw_PlanPrepare_HardwareFalse
 * @tc.desc: Test CanSkipOpIncNodeDraw when planStatus=PREPARE and GetHardwareEnabled=false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterProcessorTest, CanSkipOpIncNodeDraw_PlanPrepare_HardwareFalse, TestSize.Level1)
{
    // Test Case: planStatus_==PREPARE, GetHardwareEnabled()==false
    RSOpincLayerSplitterProcessor processor;
    processor.splitSurface_ = std::make_shared<SplitSurface>(100, 100);
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(0);
    SetupNodeGeometry(surfaceNode, 100, 100);
    auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
    auto renderParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable->GetRenderParams().get());
    renderParams->SetHardwareEnabled(false);
    processor.splitSurface_->splitSurfaceDrawable_ = surfaceDrawable;
    processor.opIncNodes_.insert(1);
    processor.planStatus_ = PlanStatus::PREPARE;

    bool result = processor.CanSkipOpIncNodeDraw(1);
    EXPECT_FALSE(result);
}
} // namespace OHOS::Rosen