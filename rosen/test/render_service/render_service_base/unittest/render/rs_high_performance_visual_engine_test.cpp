/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_effect_render_node.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_surface_render_node.h"
#include "render/rs_high_performance_visual_engine.h"
#include "utils/rect.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSHveFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    std::shared_ptr<RSSurfaceRenderNode> rootSurfaceNode;
    std::shared_ptr<RSEffectRenderNode> effectNode;
    std::shared_ptr<RSEffectRenderNode> testNode;
    std::shared_ptr<RSRenderNode> renderNode;
    static constexpr uint32_t MAX_FILTER_SIZE = 500;
    static constexpr uint32_t ROOT_SURFACE_NODE_ID = 1;
    static constexpr uint32_t EFFECT_NODE_ID = 2;
    static constexpr uint32_t TEST_NODE_ID = 3;
    static constexpr uint32_t RENDER_NODE_ID = 4;
};

void RSHveFilterTest::SetUpTestCase() {}
void RSHveFilterTest::TearDownTestCase() {}
void RSHveFilterTest::SetUp()
{
    rootSurfaceNode = std::make_shared<RSSurfaceRenderNode>(ROOT_SURFACE_NODE_ID);
    effectNode = std::make_shared<RSEffectRenderNode>(EFFECT_NODE_ID);
    testNode = std::make_shared<RSEffectRenderNode>(TEST_NODE_ID);
    renderNode = std::make_shared<RSRenderNode>(RENDER_NODE_ID);

    testNode->GetMutableRenderProperties().SetUseEffect(true);
    testNode->SetGlobalAlpha(1.0f);
    testNode->UpdateChildrenOutOfRectFlag(true);

    rootSurfaceNode->AddChild(effectNode);
    effectNode->AddChild(testNode);
    testNode->AddChild(renderNode);
}
void RSHveFilterTest::TearDown() {}

/**
 * @tc.name: ClearSurfaceNodeInfoTest
 * @tc.desc: Verify function ClearSurfaceNodeInfo
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSHveFilterTest, ClearSurfaceNodeInfoTest, TestSize.Level1)
{
    HveFilter filter;
    filter.ClearSurfaceNodeInfo();
    EXPECT_TRUE(filter.surfaceNodeInfo_.empty());
}

/**
 * @tc.name: PushSurfaceNodeInfoTest
 * @tc.desc: Verify function PushSurfaceNodeInfo
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSHveFilterTest, PushSurfaceNodeInfoTest, TestSize.Level1)
{
    HveFilter filter;
    SurfaceNodeInfo surfaceNodeInfo;
    filter.PushSurfaceNodeInfo(surfaceNodeInfo);
    EXPECT_NE(filter.surfaceNodeInfo_.size(), 0);
}

/**
 * @tc.name: GetSurfaceNodeInfoTest
 * @tc.desc: Verify function GetSurfaceNodeInfo
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSHveFilterTest, GetSurfaceNodeInfoTest, TestSize.Level1)
{
    HveFilter filter;
    EXPECT_TRUE(filter.GetSurfaceNodeInfo().empty());
}

/**
 * @tc.name: GetSurfaceNodeSizeTest
 * @tc.desc: Verify function GetSurfaceNodeSize
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSHveFilterTest, GetSurfaceNodeSizeTest, TestSize.Level1)
{
    HveFilter filter;
    EXPECT_EQ(filter.GetSurfaceNodeSize(), 0);
}

/**
 * @tc.name: HasValidEffectTest
 * @tc.desc: Verify function HasValidEffect
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSHveFilterTest, HasValidEffectTest, TestSize.Level1)
{
    HveFilter filter;
    EXPECT_FALSE(filter.HasValidEffect(nullptr));
    EXPECT_FALSE(filter.HasValidEffect(renderNode.get()));
    filter.HasValidEffect(rootSurfaceNode.get());
    EXPECT_FALSE(filter.HasValidEffect(testNode.get()));
    testNode->UpdateChildrenOutOfRectFlag(false);
    filter.HasValidEffect(testNode.get());
    testNode->SetGlobalAlpha(0.5f);
    EXPECT_FALSE(filter.HasValidEffect(testNode.get()));
    testNode->SetGlobalAlpha(1.0f);
    rootSurfaceNode->RemoveChild(effectNode);
    EXPECT_FALSE(filter.HasValidEffect(rootSurfaceNode.get()));
}

/**
 * @tc.name: CheckPreconditionTest
 * @tc.desc: Verify function CheckPrecondition
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSHveFilterTest, CheckPreconditionTest, TestSize.Level1)
{
    HveFilter filter;
    RectI filterRect = {0, 0, 100, 100};
    rootSurfaceNode->SetArsrTag(false);
    renderNode->GetMutableRenderProperties().backgroundFilter_ = nullptr;
    renderNode->GetMutableRenderProperties().SetUseEffect(false);
    renderNode->GetMutableRenderProperties().hasFrostedGlassEffect_ = false;
    EXPECT_FALSE(filter.CheckPrecondition(*renderNode, filterRect, *rootSurfaceNode));
    rootSurfaceNode->SetArsrTag(true);
    EXPECT_FALSE(filter.CheckPrecondition(*renderNode, filterRect, *rootSurfaceNode));
    renderNode->GetMutableRenderProperties().hasFrostedGlassEffect_ = true;
    filter.CheckPrecondition(*renderNode, filterRect, *rootSurfaceNode);
    renderNode->GetMutableRenderProperties().hasFrostedGlassEffect_ = false;
    renderNode->GetMutableRenderProperties().SetUseEffect(true);
    filter.CheckPrecondition(*renderNode, filterRect, *rootSurfaceNode);
    filterRect.SetRight(600);
    filterRect.SetBottom(600);
    EXPECT_FALSE(filter.CheckPrecondition(*renderNode, filterRect, *rootSurfaceNode));
}

/**
 * @tc.name: PushHveFilterSurfaceNodeMappingTest
 * @tc.desc: Verify function PushHveFilterSurfaceNodeMapping
 * @tc.type: FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSHveFilterTest, PushHveFilterSurfaceNodeMappingTest, TestSize.Level1)
{
    HveFilter filter;
    filter.hveFilterToSurfaceNodeMap_.clear();
    NodeId filterId = 1;
    NodeId surfaceId = 100;
    filter.PushHveFilterSurfaceNodeMapping(filterId, surfaceId);
    filter.PushHveFilterSurfaceNodeMapping(filterId, surfaceId);
}

/**
 * @tc.name: SampleLayerTest
 * @tc.desc: Verify function SampleLayer
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSHveFilterTest, SampleLayerTest, TestSize.Level1)
{
    HveFilter filter;
    int canvasHeight = 10;
    int canvasWidth = 10;
    std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(canvasHeight, canvasWidth);
    RSPaintFilterCanvas paintFilterCanvas(drawingCanvas.get());
    const Drawing::RectI srcRect = Drawing::RectI(0, 0, canvasWidth, canvasHeight);
    NodeId filterId = 1;
    auto outImage = filter.SampleLayer(paintFilterCanvas, srcRect, filterId);
    EXPECT_EQ(outImage, nullptr);

    NodeId surfaceId = 2;
    Drawing::Rect dstRect = {0, 0, canvasWidth, canvasHeight};
    Drawing::Matrix rotateMatrix = paintFilterCanvas.GetTotalMatrix();
    auto surfaceNodeImage = std::make_shared<Drawing::Image>();
    SurfaceNodeInfo surfaceNodeInfo1 = {surfaceNodeImage, rotateMatrix, srcRect, dstRect, RgbPalette::Transparent(),
        surfaceId};
    filter.PushSurfaceNodeInfo(surfaceNodeInfo1);
    SurfaceNodeInfo surfaceNodeInfo2 = {surfaceNodeImage, rotateMatrix, srcRect, dstRect, RgbPalette::Black(), 3};
    filter.PushSurfaceNodeInfo(surfaceNodeInfo2);
    filter.PushHveFilterSurfaceNodeMapping(filterId, surfaceId);
    outImage = filter.SampleLayer(paintFilterCanvas, srcRect, filterId);
    EXPECT_NE(outImage, nullptr);
}

} // namespace Rosen
} // namespace OHOS