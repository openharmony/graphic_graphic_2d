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

} // namespace Rosen
} // namespace OHOS
