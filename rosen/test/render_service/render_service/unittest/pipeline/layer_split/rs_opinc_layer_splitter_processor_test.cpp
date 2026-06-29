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

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSOpincLayerSplitterProcessorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

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
 * @tc.name: Sync001
 * @tc.desc: Check if Sync works with empty planner
 * @tc.type: FUNC
 */
HWTEST_F(RSOpincLayerSplitterProcessorTest, Sync001, TestSize.Level1)
{
    RSOpincLayerSplitterProcessor processor;
    processor.planStatus_ = PlanStatus::PREPARE;
    processor.opIncNodes_.insert(1);
    auto planner = std::make_shared<RSOpincLayerSplitterPlanner>();
    processor.Sync(planner);
    EXPECT_TRUE(processor.opIncNodes_.empty());
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
 * @tc.name: RecordNodeWithCacheImage002
 * @tc.desc: Check if RecordNodeWithCacheImage works when planStatus is OFF and node is in opIncNodes
 * @tc.type: FUNC
 */
HWTEST_F(RSOpincLayerSplitterProcessorTest, RecordNodeWithCacheImage002, TestSize.Level1)
{
    RSOpincLayerSplitterProcessor processor;
    processor.planStatus_ = PlanStatus::OFF;
    processor.opIncNodes_.insert(1);
    processor.splitSurface_ = std::make_shared<SplitSurface>(100, 100);
    processor.requestController_ = std::make_shared<RequestController>();

    processor.RecordNodeWithCacheImage(1);

    EXPECT_TRUE(processor.opIncNodes_.empty());
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
    processor.requestController_ = std::make_shared<RequestController>();

    processor.RecordNodeWithCacheImage(2);

    EXPECT_FALSE(processor.opIncNodes_.empty());
}
} // namespace OHOS::Rosen