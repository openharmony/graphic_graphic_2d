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
#include "pipeline/layer_split/splitter/opinc/controller/rs_opinc_split_controller.h"
#include "pipeline/layer_split/surface/rs_split_surface.h"
#include "pipeline/layer_split/selector/opinc/rs_opinc_split_node_selector.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "params/rs_surface_render_params.h"
#include "pipeline/rs_test_util.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {

class RSOpincLayerSplitterProcessorTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};

HWTEST_F(RSOpincLayerSplitterProcessorTest, Constructor001, TestSize.Level1)
{
    auto processor = std::make_shared<RSOpincLayerSplitterProcessor>();
    ASSERT_NE(processor, nullptr);
}

HWTEST_F(RSOpincLayerSplitterProcessorTest, GetPlanStatus_DefaultOFF, TestSize.Level1)
{
    auto processor = std::make_shared<RSOpincLayerSplitterProcessor>();
    ASSERT_NE(processor, nullptr);
    ASSERT_EQ(processor->GetPlanStatus(), PlanStatus::OFF);
}

HWTEST_F(RSOpincLayerSplitterProcessorTest, Sync001, TestSize.Level1)
{
    auto processor = std::make_shared<RSOpincLayerSplitterProcessor>();
    ASSERT_NE(processor, nullptr);
    auto planner = OpincSplitNodeSelector::GetInstance()->MakePlanner();
    ASSERT_NE(planner, nullptr);
    processor->Sync(planner);
    ASSERT_EQ(processor->GetPlanStatus(), PlanStatus::OFF);
}

HWTEST_F(RSOpincLayerSplitterProcessorTest, RequestFrame001, TestSize.Level1)
{
    auto processor = std::make_shared<RSOpincLayerSplitterProcessor>();
    ASSERT_NE(processor, nullptr);
    RSSurfaceRenderParams params(1);
    processor->RequestFrame(params);
}

HWTEST_F(RSOpincLayerSplitterProcessorTest, DrawDfx001, TestSize.Level1)
{
    auto processor = std::make_shared<RSOpincLayerSplitterProcessor>();
    ASSERT_NE(processor, nullptr);
    processor->DrawDfx();
}

HWTEST_F(RSOpincLayerSplitterProcessorTest, NeedDrawSplitCanvas001, TestSize.Level1)
{
    auto processor = std::make_shared<RSOpincLayerSplitterProcessor>();
    ASSERT_NE(processor, nullptr);
    Drawing::Canvas canvas;
    auto result = processor->NeedDrawSplitCanvas(canvas, 1);
    ASSERT_FALSE(result);
}

HWTEST_F(RSOpincLayerSplitterProcessorTest, CanSkipOpIncNodeDraw001, TestSize.Level1)
{
    auto processor = std::make_shared<RSOpincLayerSplitterProcessor>();
    ASSERT_NE(processor, nullptr);
    auto result = processor->CanSkipOpIncNodeDraw(1);
    ASSERT_FALSE(result);
}

HWTEST_F(RSOpincLayerSplitterProcessorTest, GetSplitCanvas001, TestSize.Level1)
{
    auto processor = std::make_shared<RSOpincLayerSplitterProcessor>();
    ASSERT_NE(processor, nullptr);
    auto canvas = processor->GetSplitCanvas();
    ASSERT_EQ(canvas, nullptr);
}

HWTEST_F(RSOpincLayerSplitterProcessorTest, RecordNodeWithCacheImage001, TestSize.Level1)
{
    auto processor = std::make_shared<RSOpincLayerSplitterProcessor>();
    ASSERT_NE(processor, nullptr);
    processor->RecordNodeWithCacheImage(1);
}

HWTEST_F(RSOpincLayerSplitterProcessorTest, CalSplitCanvasMatrix001, TestSize.Level1)
{
    auto processor = std::make_shared<RSOpincLayerSplitterProcessor>();
    ASSERT_NE(processor, nullptr);
    Drawing::Canvas canvas;
    processor->CalSplitCanvasMatrix(canvas, 1);
}

HWTEST_F(RSOpincLayerSplitterProcessorTest, FullLifecycle001, TestSize.Level1)
{
    auto processor = std::make_shared<RSOpincLayerSplitterProcessor>();
    ASSERT_NE(processor, nullptr);

    auto planner = OpincSplitNodeSelector::GetInstance()->MakePlanner();
    ASSERT_NE(planner, nullptr);

    processor->Sync(planner);
    ASSERT_EQ(processor->GetPlanStatus(), PlanStatus::OFF);

    Drawing::Canvas canvas;
    processor->NeedDrawSplitCanvas(canvas, 1);
    processor->CanSkipOpIncNodeDraw(1);
    processor->GetSplitCanvas();
    processor->DrawDfx();
    processor->RecordNodeWithCacheImage(1);
    processor->CalSplitCanvasMatrix(canvas, 1);

    RSSurfaceRenderParams params(1);
    processor->RequestFrame(params);
}

HWTEST_F(RSOpincLayerSplitterProcessorTest, Sync002_WithPlanStatus, TestSize.Level1)
{
    auto processor = std::make_shared<RSOpincLayerSplitterProcessor>();
    ASSERT_NE(processor, nullptr);
    auto planner = OpincSplitNodeSelector::GetInstance()->MakePlanner();
    ASSERT_NE(planner, nullptr);

    processor->Sync(planner);
    processor->Sync(planner);
}

HWTEST_F(RSOpincLayerSplitterProcessorTest, CanSkipOpIncNodeDraw_MultipleNodes, TestSize.Level1)
{
    auto processor = std::make_shared<RSOpincLayerSplitterProcessor>();
    ASSERT_NE(processor, nullptr);
    ASSERT_FALSE(processor->CanSkipOpIncNodeDraw(1));
    ASSERT_FALSE(processor->CanSkipOpIncNodeDraw(2));
    ASSERT_FALSE(processor->CanSkipOpIncNodeDraw(3));
}

HWTEST_F(RSOpincLayerSplitterProcessorTest, NeedDrawSplitCanvas_MultipleNodes, TestSize.Level1)
{
    auto processor = std::make_shared<RSOpincLayerSplitterProcessor>();
    ASSERT_NE(processor, nullptr);
    Drawing::Canvas canvas;
    ASSERT_FALSE(processor->NeedDrawSplitCanvas(canvas, 1));
    ASSERT_FALSE(processor->NeedDrawSplitCanvas(canvas, 2));
}

} // namespace
} // namespace OHOS::Rosen
