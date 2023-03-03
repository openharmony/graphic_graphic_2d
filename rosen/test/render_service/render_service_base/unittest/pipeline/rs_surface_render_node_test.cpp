/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>

#include "pipeline/rs_surface_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSSurfaceRenderNodeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline NodeId id;
    static inline std::weak_ptr<RSContext> context = {};
};

void RSSurfaceRenderNodeTest::SetUpTestCase() {}
void RSSurfaceRenderNodeTest::TearDownTestCase() {}
void RSSurfaceRenderNodeTest::SetUp() {}
void RSSurfaceRenderNodeTest::TearDown() {}

/**
 * @tc.name: SetContextMatrix001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeTest, SetContextMatrix001, TestSize.Level1)
{
    RSSurfaceRenderNode surfaceRenderNode(id, context);
    SkMatrix matrix;
    bool sendMsg = false;
    surfaceRenderNode.SetContextMatrix(matrix, sendMsg);
}

/**
 * @tc.name: SetContextClipRegion001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeTest, SetContextClipRegion001, TestSize.Level1)
{
    RSSurfaceRenderNode surfaceRenderNode(id, context);
    SkRect clipRegion { 0, 0, 0, 0 };
    bool sendMsg = false;
    surfaceRenderNode.SetContextClipRegion(clipRegion, sendMsg);
}

/**
 * @tc.name: ConnectToNodeInRenderService001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeTest, ConnectToNodeInRenderService001, TestSize.Level1)
{
    RSSurfaceRenderNode surfaceRenderNode(id, context);
    surfaceRenderNode.ConnectToNodeInRenderService();
}

/**
 * @tc.name: ClearChildrenCache001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeTest, ClearChildrenCache001, TestSize.Level1)
{
    RSSurfaceRenderNode surfaceRenderNode(id, context);
    surfaceRenderNode.ResetParent();
}

/**
 * @tc.name: SetVisibleDirtyRegion
 * @tc.desc: function test
 * @tc.type:FUNC
 * @tc.require: I68IPR
 */
HWTEST_F(RSSurfaceRenderNodeTest, SetVisibleDirtyRegion, TestSize.Level1)
{
    RSSurfaceRenderNode surfaceRenderNode(id, context);
    Occlusion::Rect rect {0, 0, 200, 200};
    Occlusion::Region region {rect};
    surfaceRenderNode.SetVisibleDirtyRegion(region);
    auto vdRegion = surfaceRenderNode.GetVisibleDirtyRegion();
}

/**
 * @tc.name: SetAlignedVisibleDirtyRegion
 * @tc.desc: function test
 * @tc.type:FUNC
 * @tc.require: I68IPR
 */
HWTEST_F(RSSurfaceRenderNodeTest, SetAlignedVisibleDirtyRegion, TestSize.Level1)
{
    RSSurfaceRenderNode surfaceRenderNode(id, context);
    Occlusion::Rect rect {0, 0, 256, 256};
    Occlusion::Region region {rect};
    surfaceRenderNode.SetAlignedVisibleDirtyRegion(region);
    auto vdRegion = surfaceRenderNode.GetAlignedVisibleDirtyRegion();
}

/**
 * @tc.name: SetDirtyRegionBelowCurrentLayer
 * @tc.desc: function test
 * @tc.type:FUNC
 * @tc.require: I68IPR
 */
HWTEST_F(RSSurfaceRenderNodeTest, SetDirtyRegionBelowCurrentLayer, TestSize.Level1)
{
    RSSurfaceRenderNode surfaceRenderNode(id, context);
    Occlusion::Rect rect {0, 0, 256, 256};
    Occlusion::Region region {rect};
    surfaceRenderNode.SetDirtyRegionBelowCurrentLayer(region);
    auto vdRegion = surfaceRenderNode.GetDirtyRegionBelowCurrentLayer();
}

/**
 * @tc.name: SetGlobalDirtyRegion
 * @tc.desc: function test
 * @tc.type:FUNC
 * @tc.require: I68IPR
 */
HWTEST_F(RSSurfaceRenderNodeTest, SetGlobalDirtyRegion, TestSize.Level1)
{
    RSSurfaceRenderNode surfaceRenderNode(id, context);
    RectI rect {0, 0, 256, 256};
    surfaceRenderNode.SetGlobalDirtyRegion(rect);
}

/**
 * @tc.name: ResetSurfaceOpaqueRegion01
 * @tc.desc: function test
 * @tc.type:FUNC
 * @tc.require: I68IPR
 */
HWTEST_F(RSSurfaceRenderNodeTest, ResetSurfaceOpaqueRegion01, TestSize.Level1)
{
    RSSurfaceRenderNode surfaceRenderNode(id, context);
    RectI screenRect {0, 0, 2560, 1600};
    RectI absRect {0, 100, 400, 500};
    ContainerWindowConfigType containerWindowConfigType {ContainerWindowConfigType::ENABLED_UNFOCUSED_WINDOW_LEVEL_2};
    surfaceRenderNode.ResetSurfaceOpaqueRegion(screenRect, absRect, containerWindowConfigType, false);
    surfaceRenderNode.ResetSurfaceOpaqueRegion(screenRect, absRect, containerWindowConfigType, true);
}

/**
 * @tc.name: ResetSurfaceOpaqueRegion02
 * @tc.desc: function test
 * @tc.type:FUNC
 * @tc.require: I6HF6Y
 */
HWTEST_F(RSSurfaceRenderNodeTest, ResetSurfaceOpaqueRegion02, TestSize.Level1)
{
    RSSurfaceRenderNode surfaceRenderNode(id, context);
    RectI screenRect {0, 0, 2560, 1600};
    RectI absRect {0, 100, 400, 500};
    surfaceRenderNode.SetAbilityBGAlpha(0);
    surfaceRenderNode.ResetSurfaceOpaqueRegion(screenRect, absRect, ScreenRotation::ROTATION_0, false);
    surfaceRenderNode.ResetSurfaceOpaqueRegion(screenRect, absRect, ScreenRotation::ROTATION_0, true);
}

/**
 * @tc.name: ResetSurfaceOpaqueRegion03
 * @tc.desc: function test
 * @tc.type:FUNC
 * @tc.require: I6HF6Y
 */
HWTEST_F(RSSurfaceRenderNodeTest, ResetSurfaceOpaqueRegion03, TestSize.Level1)
{
    RSSurfaceRenderNode surfaceRenderNode(id, context);
    RectI screenRect {0, 0, 2560, 1600};
    RectI absRect {0, 100, 400, 500};
    surfaceRenderNode.SetAbilityBGAlpha(255);
    surfaceRenderNode.SetGlobalAlpha(1.0f);
    surfaceRenderNode.SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    surfaceRenderNode.SetContainerWindow(true, 1.0f);
    surfaceRenderNode.ResetSurfaceOpaqueRegion(screenRect, absRect, ScreenRotation::ROTATION_0, false);
    surfaceRenderNode.ResetSurfaceOpaqueRegion(screenRect, absRect, ScreenRotation::ROTATION_0, true);
}

/**
 * @tc.name: ResetSurfaceOpaqueRegion04
 * @tc.desc: function test
 * @tc.type:FUNC
 * @tc.require: I6HF6Y
 */
HWTEST_F(RSSurfaceRenderNodeTest, ResetSurfaceOpaqueRegion04, TestSize.Level1)
{
    RSSurfaceRenderNode surfaceRenderNode(id, context);
    RectI screenRect {0, 0, 2560, 1600};
    RectI absRect {0, 100, 400, 500};
    surfaceRenderNode.SetAbilityBGAlpha(255);
    surfaceRenderNode.SetGlobalAlpha(1.0f);
    surfaceRenderNode.GetMutableRenderProperties().SetCornerRadius(Vector4f(15.0f));
    surfaceRenderNode.ResetSurfaceOpaqueRegion(screenRect, absRect, ScreenRotation::ROTATION_0, false);
    surfaceRenderNode.ResetSurfaceOpaqueRegion(screenRect, absRect, ScreenRotation::ROTATION_0, true);
}

/**
 * @tc.name: ResetSurfaceOpaqueRegion05
 * @tc.desc: function test
 * @tc.type:FUNC
 * @tc.require: I6HF6Y
 */
HWTEST_F(RSSurfaceRenderNodeTest, ResetSurfaceOpaqueRegion05, TestSize.Level1)
{
    RSSurfaceRenderNode surfaceRenderNode(id, context);
    RectI screenRect {0, 0, 2560, 1600};
    RectI absRect {0, 100, 400, 500};
    surfaceRenderNode.SetAbilityBGAlpha(255);
    surfaceRenderNode.SetGlobalAlpha(1.0f);
    surfaceRenderNode.SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    surfaceRenderNode.SetContainerWindow(true, 1.0f);
    surfaceRenderNode.ResetSurfaceOpaqueRegion(screenRect, absRect, ScreenRotation::ROTATION_90, false);
    surfaceRenderNode.ResetSurfaceOpaqueRegion(screenRect, absRect, ScreenRotation::ROTATION_90, true);
}

/**
 * @tc.name: ResetSurfaceOpaqueRegion06
 * @tc.desc: function test
 * @tc.type:FUNC
 * @tc.require: I6HF6Y
 */
HWTEST_F(RSSurfaceRenderNodeTest, ResetSurfaceOpaqueRegion06, TestSize.Level1)
{
    RSSurfaceRenderNode surfaceRenderNode(id, context);
    RectI screenRect {0, 0, 2560, 1600};
    RectI absRect {0, 100, 400, 500};
    surfaceRenderNode.SetAbilityBGAlpha(255);
    surfaceRenderNode.SetGlobalAlpha(1.0f);
    surfaceRenderNode.SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    surfaceRenderNode.SetContainerWindow(true, 1.0f);
    surfaceRenderNode.ResetSurfaceOpaqueRegion(screenRect, absRect, ScreenRotation::ROTATION_180, false);
    surfaceRenderNode.ResetSurfaceOpaqueRegion(screenRect, absRect, ScreenRotation::ROTATION_180, true);
}

/**
 * @tc.name: ResetSurfaceOpaqueRegion07
 * @tc.desc: function test
 * @tc.type:FUNC
 * @tc.require: I6HF6Y
 */
HWTEST_F(RSSurfaceRenderNodeTest, ResetSurfaceOpaqueRegion07, TestSize.Level1)
{
    RSSurfaceRenderNode surfaceRenderNode(id, context);
    RectI screenRect {0, 0, 2560, 1600};
    RectI absRect {0, 100, 400, 500};
    surfaceRenderNode.SetAbilityBGAlpha(255);
    surfaceRenderNode.SetGlobalAlpha(1.0f);
    surfaceRenderNode.SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    surfaceRenderNode.SetContainerWindow(true, 1.0f);
    surfaceRenderNode.ResetSurfaceOpaqueRegion(screenRect, absRect, ScreenRotation::ROTATION_270, false);
    surfaceRenderNode.ResetSurfaceOpaqueRegion(screenRect, absRect, ScreenRotation::ROTATION_270, true);
}

/**
 * @tc.name: SetNodeCostTest
 * @tc.desc: function test
 * @tc.type:FUNC
 * @tc.require: issueI6FZHQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, SetNodeCostTest, TestSize.Level1)
{
    RSSurfaceRenderNode surfaceRenderNode(id, context);
    auto result = surfaceRenderNode.nodeCost_;
    ASSERT_EQ(0, result);
    surfaceRenderNode.SetNodeCost(6);
    result = surfaceRenderNode.nodeCost_;
    ASSERT_EQ(6, result);
}

/**
 * @tc.name: GetNodeCostTest
 * @tc.desc: function test
 * @tc.type:FUNC
 * @tc.require: issueI6FZHQ
 */
HWTEST_F(RSSurfaceRenderNodeTest, GetNodeCostTest, TestSize.Level1)
{
    RSSurfaceRenderNode surfaceRenderNode(id, context);
    auto result = surfaceRenderNode.nodeCost_;
    ASSERT_EQ(0, result);
    surfaceRenderNode.SetNodeCost(6);
    result = surfaceRenderNode.GetNodeCost();
    ASSERT_EQ(6, result);
}

} // namespace Rosen
} // namespace OHOS
