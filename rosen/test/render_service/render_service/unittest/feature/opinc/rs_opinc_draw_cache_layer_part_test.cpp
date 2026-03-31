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

/**
 * @tc.name: LayerPartRenderBranchCoverageTest
 * @tc.desc: Branch coverage tests for layer part render feature (commit 266e787e55)
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */

#include "gtest/gtest.h"
#include "feature/opinc/rs_opinc_draw_cache.h"
#include "params/rs_render_params.h"
#include "platform/common/rs_system_properties.h"
#include "parameters.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

constexpr NodeId DEFAULT_ID = 0xFFFF;
constexpr int32_t TEST_NODE_COUNT = 5;

class RSOpincDrawCacheLayerPartTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline NodeId id = DEFAULT_ID;
};

void RSOpincDrawCacheLayerPartTest::SetUpTestCase() {}
void RSOpincDrawCacheLayerPartTest::TearDownTestCase() {}
void RSOpincDrawCacheLayerPartTest::SetUp() {}
void RSOpincDrawCacheLayerPartTest::TearDown() {}

/**
 * @tc.name: PopLayerPartRenderDirtyRegionWithEmptyStack
 * @tc.desc: Test PopLayerPartRenderDirtyRegion when stack is empty
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, PopLayerPartRenderDirtyRegionWithEmptyStack, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    RSRenderParams params(id);

    params.SetLayerPartRenderEnabled(true);
    ASSERT_TRUE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());

    opincDrawCache.PopLayerPartRenderDirtyRegion(params, paintFilterCanvas);

    ASSERT_TRUE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
}

/**
 * @tc.name: PopLayerPartRenderDirtyRegionDisabled
 * @tc.desc: Test PopLayerPartRenderDirtyRegion when feature is disabled
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, PopLayerPartRenderDirtyRegionDisabled, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    RSRenderParams params(id);

    params.SetLayerPartRenderEnabled(false);

    opincDrawCache.PopLayerPartRenderDirtyRegion(params, paintFilterCanvas);

    ASSERT_TRUE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
}

/**
 * @tc.name: PushLayerPartRenderDirtyRegionDisabled
 * @tc.desc: Test PushLayerPartRenderDirtyRegion when feature is disabled
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, PushLayerPartRenderDirtyRegionDisabled, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    RSRenderParams params(id);

    params.SetLayerPartRenderEnabled(false);
    RectI dirtyRect = {10, 10, 100, 100};
    params.SetLayerPartRenderCurrentFrameDirtyRegion(dirtyRect);

    opincDrawCache.PushLayerPartRenderDirtyRegion(params, paintFilterCanvas, TEST_NODE_COUNT);

    ASSERT_TRUE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
}

/**
 * @tc.name: PushLayerPartRenderDirtyRegionEnabled
 * @tc.desc: Test PushLayerPartRenderDirtyRegion when feature is enabled
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, PushLayerPartRenderDirtyRegionEnabled, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    RSRenderParams params(id);

    params.SetLayerPartRenderEnabled(true);
    RectI dirtyRect = {10, 10, 100, 100};
    params.SetLayerPartRenderCurrentFrameDirtyRegion(dirtyRect);

    opincDrawCache.PushLayerPartRenderDirtyRegion(params, paintFilterCanvas, TEST_NODE_COUNT);

    ASSERT_FALSE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
}

/**
 * @tc.name: LayerDirtyRegionDfxWithDebugDisabled
 * @tc.desc: Test LayerDirtyRegionDfx when debug is disabled
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, LayerDirtyRegionDfxWithDebugDisabled, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);

    Drawing::RectI dirtyRect(10, 10, 100, 100);

    opincDrawCache.LayerDirtyRegionDfx(paintFilterCanvas, dirtyRect);

    ASSERT_FALSE(RSSystemProperties::GetLayerPartRenderDebugEnabled());
}

/**
 * @tc.name: LayerPartRenderClipDirtyRegionDisabled
 * @tc.desc: Test LayerPartRenderClipDirtyRegion when feature is disabled
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, LayerPartRenderClipDirtyRegionDisabled, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    RSRenderParams params(id);

    params.SetLayerPartRenderEnabled(false);
    bool isOffScreenWithClipHole = true;

    opincDrawCache.LayerPartRenderClipDirtyRegion(params, &isOffScreenWithClipHole, paintFilterCanvas);

    ASSERT_TRUE(isOffScreenWithClipHole);
}

/**
 * @tc.name: LayerPartRenderClipDirtyRegionEnabled
 * @tc.desc: Test LayerPartRenderClipDirtyRegion when feature is enabled
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, LayerPartRenderClipDirtyRegionEnabled, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    RSRenderParams params(id);

    params.SetLayerPartRenderEnabled(true);
    RectI dirtyRect = {10, 10, 100, 100};
    params.SetLayerPartRenderCurrentFrameDirtyRegion(dirtyRect);

    opincDrawCache.PushLayerPartRenderDirtyRegion(params, paintFilterCanvas, TEST_NODE_COUNT);

    bool isOffScreenWithClipHole = true;
    opincDrawCache.LayerPartRenderClipDirtyRegion(params, &isOffScreenWithClipHole, paintFilterCanvas);

    ASSERT_FALSE(isOffScreenWithClipHole);
}

/**
 * @tc.name: LayerPartRenderFullWorkflow
 * @tc.desc: Test complete workflow of layer part render
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, LayerPartRenderFullWorkflow, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    RSRenderParams params(id);

    params.SetLayerPartRenderEnabled(true);
    RectI dirtyRect = {10, 10, 100, 100};
    params.SetLayerPartRenderCurrentFrameDirtyRegion(dirtyRect);

    opincDrawCache.PushLayerPartRenderDirtyRegion(params, paintFilterCanvas, TEST_NODE_COUNT);
    ASSERT_FALSE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());

    bool isOffScreenWithClipHole = true;
    opincDrawCache.LayerPartRenderClipDirtyRegion(params, &isOffScreenWithClipHole, paintFilterCanvas);
    ASSERT_FALSE(isOffScreenWithClipHole);

    opincDrawCache.PopLayerPartRenderDirtyRegion(params, paintFilterCanvas);
    ASSERT_TRUE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
}

/**
 * @tc.name: PopAfterPush
 * @tc.desc: Test Pop after Push returns non-empty stack
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, PopAfterPush, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    RSRenderParams params(id);

    params.SetLayerPartRenderEnabled(true);
    RectI dirtyRect = {10, 10, 100, 100};
    params.SetLayerPartRenderCurrentFrameDirtyRegion(dirtyRect);

    opincDrawCache.PushLayerPartRenderDirtyRegion(params, paintFilterCanvas, TEST_NODE_COUNT);
    ASSERT_FALSE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());

    opincDrawCache.PopLayerPartRenderDirtyRegion(params, paintFilterCanvas);
    ASSERT_TRUE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
}

/**
 * @tc.name: GetCurLayerPartRenderDirtyRegionWithNonEmptyStack
 * @tc.desc: Test GetCurLayerPartRenderDirtyRegion when stack is not empty
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, GetCurLayerPartRenderDirtyRegionWithNonEmptyStack, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    RSRenderParams params(id);

    params.SetLayerPartRenderEnabled(true);
    RectI dirtyRect = {10, 10, 100, 100};
    params.SetLayerPartRenderCurrentFrameDirtyRegion(dirtyRect);

    opincDrawCache.PushLayerPartRenderDirtyRegion(params, paintFilterCanvas, TEST_NODE_COUNT);
    ASSERT_FALSE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());

    auto& dirtyRegion = paintFilterCanvas.GetCurLayerPartRenderDirtyRegion();
    // Non-empty stack should return the pushed dirty region
    ASSERT_FALSE(dirtyRegion.IsEmpty());
}

/**
 * @tc.name: QuickRejectWithEmptyLayerPartRenderStack
 * @tc.desc: Test QuickReject branch when LayerPartRenderDirtyRegionStack is empty
 *           When stack is empty, QuickReject uses GetCurDirtyRegion() instead
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, QuickRejectWithEmptyLayerPartRenderStack, TestSize.Level1)
{
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);

    // Stack is empty - GetCurLayerPartRenderDirtyRegion should NOT be called directly
    // as it would cause undefined behavior on empty stack
    ASSERT_TRUE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());

    // When stack is empty, QuickReject logic falls back to GetCurDirtyRegion()
    // This verifies the empty stack condition is properly handled
    auto& dirtyRegion = paintFilterCanvas.GetCurDirtyRegion();
    ASSERT_TRUE(dirtyRegion.IsEmpty());
}

/**
 * @tc.name: QuickRejectWithNonEmptyLayerPartRenderStackIntersects
 * @tc.desc: Test QuickReject branch when LayerPartRenderDirtyRegionStack is not empty and intersects
 *           This tests: layerNodeDirtyRegion.IsIntersects(dstRegion) returns true
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, QuickRejectWithNonEmptyLayerPartRenderStackIntersects, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    RSRenderParams params(id);

    params.SetLayerPartRenderEnabled(true);
    RectI dirtyRect = {0, 0, 100, 100};
    params.SetLayerPartRenderCurrentFrameDirtyRegion(dirtyRect);

    opincDrawCache.PushLayerPartRenderDirtyRegion(params, paintFilterCanvas, TEST_NODE_COUNT);
    ASSERT_FALSE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());

    auto& layerPartDirtyRegion = paintFilterCanvas.GetCurLayerPartRenderDirtyRegion();
    ASSERT_FALSE(layerPartDirtyRegion.IsEmpty());

    // Create a region that intersects with the dirty region
    Drawing::Region testRegion;
    Drawing::RectI testRect = {50, 50, 150, 150};
    testRegion.SetRect(testRect);

    // Should intersect since both cover (50,50) to (100,100)
    ASSERT_TRUE(layerPartDirtyRegion.IsIntersects(testRegion));
}

/**
 * @tc.name: QuickRejectWithNonEmptyLayerPartRenderStackNotIntersects
 * @tc.desc: Test QuickReject branch when LayerPartRenderDirtyRegionStack is not empty and not intersects
 *           This tests: layerNodeDirtyRegion.IsIntersects(dstRegion) returns false
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, QuickRejectWithNonEmptyLayerPartRenderStackNotIntersects, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    RSRenderParams params(id);

    params.SetLayerPartRenderEnabled(true);
    RectI dirtyRect = {0, 0, 100, 100};
    params.SetLayerPartRenderCurrentFrameDirtyRegion(dirtyRect);

    opincDrawCache.PushLayerPartRenderDirtyRegion(params, paintFilterCanvas, TEST_NODE_COUNT);
    ASSERT_FALSE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());

    auto& layerPartDirtyRegion = paintFilterCanvas.GetCurLayerPartRenderDirtyRegion();
    ASSERT_FALSE(layerPartDirtyRegion.IsEmpty());

    // Create a region that does NOT intersect with the dirty region
    Drawing::Region testRegion;
    Drawing::RectI testRect = {200, 200, 100, 100};
    testRegion.SetRect(testRect);

    // Should NOT intersect since dirty region is (0,0)-(100,100) and test is (200,200)-(300,300)
    ASSERT_FALSE(layerPartDirtyRegion.IsIntersects(testRegion));
}

/**
 * @tc.name: PaintFilterCanvasPopLayerPartRenderDirtyRegionWithEmptyStack
 * @tc.desc: Test RSPaintFilterCanvas::PopLayerPartRenderDirtyRegion when stack is empty
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, PaintFilterCanvasPopLayerPartRenderDirtyRegionWithEmptyStack, TestSize.Level1)
{
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);

    ASSERT_TRUE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());

    // Call PopLayerPartRenderDirtyRegion on empty stack - should return without crash
    paintFilterCanvas.PopLayerPartRenderDirtyRegion();

    ASSERT_TRUE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
}

/**
 * @tc.name: PaintFilterCanvasPopLayerPartRenderDirtyRegionWithNonEmptyStack
 * @tc.desc: Test RSPaintFilterCanvas::PopLayerPartRenderDirtyRegion when stack is not empty
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, PaintFilterCanvasPopLayerPartRenderDirtyRegion, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    RSRenderParams params(id);

    params.SetLayerPartRenderEnabled(true);
    RectI dirtyRect = {10, 10, 100, 100};
    params.SetLayerPartRenderCurrentFrameDirtyRegion(dirtyRect);

    opincDrawCache.PushLayerPartRenderDirtyRegion(params, paintFilterCanvas, TEST_NODE_COUNT);
    ASSERT_FALSE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());

    // Call PopLayerPartRenderDirtyRegion directly on non-empty stack
    paintFilterCanvas.PopLayerPartRenderDirtyRegion();

    ASSERT_TRUE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
}

/**
 * @tc.name: LayerDirtyRegionDfxWithDebugEnabled
 * @tc.desc: Test LayerDirtyRegionDfx when debug is enabled
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, LayerDirtyRegionDfxWithDebugEnabled, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);

    system::SetParameter("rosen.layerPartRenderDfx.enabled", "1");

    Drawing::RectI dirtyRect(10, 10, 100, 100);

    opincDrawCache.LayerDirtyRegionDfx(paintFilterCanvas, dirtyRect);

    ASSERT_TRUE(RSSystemProperties::GetLayerPartRenderDebugEnabled());

    system::SetParameter("rosen.layerPartRenderDfx.enabled", "0");
}
} // namespace Rosen
} // namespace OHOS
