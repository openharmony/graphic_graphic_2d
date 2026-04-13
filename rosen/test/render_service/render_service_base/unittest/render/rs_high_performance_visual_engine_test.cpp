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

#include "utils/rect.h"
#include "render/rs_high_performance_visual_engine.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_effct_render_node.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_paint_filter_canvas.h"

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
    std::shared_ptr<RSEffectRenderNode> validTargetNode;
    std::shared_ptr<RSEffectRenderNode> invalidTargetNode;
    std::shared_ptr<RSRenderNode> renderNode;
    std::shared_ptr<RSRenderNode> hwcNode;
    std::pair<NodeId, RectI> testFilter;
    const int MAX_FILTER_SIZE = 500;
};

void RSHveFilterTest::SetUpTestCase() {}
void RSHveFilterTest::TearDownTestCase() {}
void RSHveFilterTest::SetUp() {
    rootSurfaceNode = std::make_shared<RSSurfaceRenderNode>(1);
    effectNode = std::make_shared<RSEffectRenderNode>(2);
    validTargetNode = std::make_shared<RSEffectRenderNode>(3);
    invalidTargetNode = std::make_shared<RSEffectRenderNode>(4);
    renderNode = std::make_shared<RSRenderNode>(5);
    hwcNode = std::make_shared<RSRenderNode>(6);

    validTargetNode->GetMutableRenderProperties().SetUseEffect(true);
    validTargetNode->SetGlobalAlpha(1.0f);
    validTargetNode->UpdateChildrenOutOfRectFlag(false);
    invalidTargetNode->GetMutableRenderProperties().SetUseEffect(false);

    rootSurfaceNode->AddChild(effectNode);
    effectNode->AddChild(validTargetNode);
    validTargetNode->AddChild(renderNode);

    testFilter = {123, RectI{0, 0, 100, 100}};
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
    filter.HasValidEffect(rootSurfaceNode.get());
    EXPECT_FALSE(filter.HasValidEffect(invalidTargetNode.get()));
    filter.HasValidEffect(validTargetNode.get());
    validTargetNode->SetGlobalAlpha(0.5f);
    EXPECT_FALSE(filter.HasValidEffect(validTargetNode.get()));
    validTargetNode->SetGlobalAlpha(1.0f);
    validTargetNode->UpdateChildrenOutOfRectFlag(true);
    EXPECT_FALSE(filter.HasValidEffect(validTargetNode.get()));
    rootSurfaceNode->RemoveChild(effectNode);
    EXPECT_FALSE(filter.HasValidEffect(rootSurfaceNode.get()));
    filter.HasValidEffect(renderNode.get());
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
    renderNode->GetMutableRenderProperties().SetUseEffect(true);
    filter.CheckPrecondition(*renderNode, testFilter, *hwcNode);
    testFilter.second.width_ = 600;
    testFilter.second.height_ = 600;
    EXPECT_FALSE(filter.CheckPrecondition(*renderNode, testFilter, *hwcNode));
    renderNode->GetMutableRenderProperties().SetUseEffect(false);
    filter.CheckPrecondition(*renderNode, testFilter, *hwcNode);
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
    auto canvas = std::make_unique<Drawing::Canvas>();
    RSPaintFilterCanvas paintFilterCanvas(canvas.get());
    EXPECT_EQ(paintFilterCanvas.GetSurface(), nullptr);
    const Drawing::RectI srcRect = Drawing::RectI(0, 0, 350, 20);
    auto outImage = filter.SampleLayer(paintFilterCanvas, srcRect);
    EXPECT_EQ(outImage, nullptr);

    std::shared_ptr<Drawing::Surface> surfacePtr = std::make_shared<Drawing::Surface>();
    std::shared_ptr<RSPaintFilterCanvas> canvasPtr = std::make_shared<RSPaintFilterCanvas>(canvas.get());
    paintFilterCanvas.ReplaceMainScreenData(surfacePtr, canvasPtr);
    EXPECT_EQ(paintFilterCanvas.GetSurface(), surfacePtr.get());
    outImage = filter.SampleLayer(paintFilterCanvas, srcRect);
    EXPECT_EQ(outImage, nullptr);
}

} // namespace Rosen
} // namespace OHOS