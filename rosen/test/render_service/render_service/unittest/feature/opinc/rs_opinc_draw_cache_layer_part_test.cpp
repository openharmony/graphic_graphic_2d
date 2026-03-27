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

#include "feature/opinc/rs_opinc_draw_cache.h"
#include "params/rs_render_params.h"
#include "parameters.h"
#include "platform/common/rs_system_properties.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr NodeId DEFAULT_ID = 0xFFFF;
constexpr int32_t DIRTY_LEFT = 10;
constexpr int32_t DIRTY_TOP = 10;
constexpr int32_t DIRTY_WIDTH = 100;
constexpr int32_t DIRTY_HEIGHT = 100;
constexpr int32_t TEST_NODE_COUNT = 5;
const RectI DEFAULT_DIRTY_RECT = { DIRTY_LEFT, DIRTY_TOP, DIRTY_WIDTH, DIRTY_HEIGHT };
}

class RSOpincDrawCacheLayerPartTest : public testing::Test {
public:
    static std::unique_ptr<RSRenderParams> CreateRenderParams(bool enabled)
    {
        auto params = std::make_unique<RSRenderParams>(DEFAULT_ID);
        params->SetLayerPartRenderEnabled(enabled);
        params->SetLayerPartRenderCurrentFrameDirtyRegion(DEFAULT_DIRTY_RECT);
        return params;
    }
};

/**
 * @tc.name: PushLayerPartRenderDirtyRegionDisabled
 * @tc.desc: Verify PushLayerPartRenderDirtyRegion returns early when layer-part render is disabled
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, PushLayerPartRenderDirtyRegionDisabled, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    auto params = CreateRenderParams(false);

    opincDrawCache.PushLayerPartRenderDirtyRegion(*params, paintFilterCanvas, TEST_NODE_COUNT);

    EXPECT_TRUE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
}

/**
 * @tc.name: PushLayerPartRenderDirtyRegionEnabled
 * @tc.desc: Verify PushLayerPartRenderDirtyRegion pushes current dirty region into stack
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, PushLayerPartRenderDirtyRegionEnabled, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    auto params = CreateRenderParams(true);

    opincDrawCache.PushLayerPartRenderDirtyRegion(*params, paintFilterCanvas, TEST_NODE_COUNT);

    EXPECT_FALSE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
}

/**
 * @tc.name: LayerPartRenderClipDirtyRegionDisabled
 * @tc.desc: Verify LayerPartRenderClipDirtyRegion returns early when layer-part render is disabled
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, LayerPartRenderClipDirtyRegionDisabled, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    auto params = CreateRenderParams(false);

    opincDrawCache.LayerPartRenderClipDirtyRegion(*params, paintFilterCanvas);

    EXPECT_TRUE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
}

/**
 * @tc.name: LayerPartRenderClipDirtyRegionWithEmptyDirty
 * @tc.desc: Verify LayerPartRenderClipDirtyRegion handles enabled state when dirty region is empty
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, LayerPartRenderClipDirtyRegionWithEmptyDirty, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    auto params = CreateRenderParams(true);

    opincDrawCache.LayerPartRenderClipDirtyRegion(*params, paintFilterCanvas);

    EXPECT_TRUE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
}

/**
 * @tc.name: LayerPartRenderClipDirtyRegionWithNonEmptyDirty
 * @tc.desc: Verify LayerPartRenderClipDirtyRegion handles enabled state when dirty region is non-empty
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, LayerPartRenderClipDirtyRegionWithNonEmptyDirty, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    auto params = CreateRenderParams(true);

    opincDrawCache.PushLayerPartRenderDirtyRegion(*params, paintFilterCanvas, TEST_NODE_COUNT);
    opincDrawCache.LayerPartRenderClipDirtyRegion(*params, paintFilterCanvas);

    EXPECT_FALSE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
}

/**
 * @tc.name: PopLayerPartRenderDirtyRegionDisabled
 * @tc.desc: Verify PopLayerPartRenderDirtyRegion returns early when layer-part render is disabled
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, PopLayerPartRenderDirtyRegionDisabled, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    auto enabledParams = CreateRenderParams(true);
    auto disabledParams = CreateRenderParams(false);

    opincDrawCache.PushLayerPartRenderDirtyRegion(*enabledParams, paintFilterCanvas, TEST_NODE_COUNT);
    EXPECT_FALSE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());

    opincDrawCache.PopLayerPartRenderDirtyRegion(*disabledParams, paintFilterCanvas);

    EXPECT_FALSE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
}

/**
 * @tc.name: PopLayerPartRenderDirtyRegionEmptyStack
 * @tc.desc: Verify PopLayerPartRenderDirtyRegion handles empty stack gracefully
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, PopLayerPartRenderDirtyRegionEmptyStack, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    auto params = CreateRenderParams(true);

    opincDrawCache.PopLayerPartRenderDirtyRegion(*params, paintFilterCanvas);

    EXPECT_TRUE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
}

/**
 * @tc.name: PopLayerPartRenderDirtyRegionNonEmptyStack
 * @tc.desc: Verify PopLayerPartRenderDirtyRegion pops one layer-part dirty region when stack is non-empty
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, PopLayerPartRenderDirtyRegionNonEmptyStack, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    auto params = CreateRenderParams(true);

    opincDrawCache.PushLayerPartRenderDirtyRegion(*params, paintFilterCanvas, TEST_NODE_COUNT);
    EXPECT_FALSE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());

    opincDrawCache.PopLayerPartRenderDirtyRegion(*params, paintFilterCanvas);

    EXPECT_TRUE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
}

/**
 * @tc.name: LayerDirtyRegionDfxWithDebugDisabled
 * @tc.desc: Verify LayerDirtyRegionDfx keeps no side effect when debug switch is disabled
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, LayerDirtyRegionDfxWithDebugDisabled, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    Drawing::RectI dirtyRect(DIRTY_LEFT, DIRTY_TOP, DIRTY_WIDTH, DIRTY_HEIGHT);
    const std::string debugKey = "rosen.layerPartRenderDfx.enabled";
    const std::string oldDebugValue = system::GetParameter(debugKey, "0");
    (void)system::SetParameter(debugKey, "0");

    opincDrawCache.LayerDirtyRegionDfx(paintFilterCanvas, dirtyRect);

    EXPECT_FALSE(RSSystemProperties::GetLayerPartRenderDebugEnabled());
    (void)system::SetParameter(debugKey, oldDebugValue);
}

/**
 * @tc.name: LayerDirtyRegionDfxWithDebugEnabled
 * @tc.desc: Verify LayerDirtyRegionDfx enters drawing branch when debug switch is enabled
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, LayerDirtyRegionDfxWithDebugEnabled, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    Drawing::RectI dirtyRect(DIRTY_LEFT, DIRTY_TOP, DIRTY_WIDTH, DIRTY_HEIGHT);
    const std::string debugKey = "rosen.layerPartRenderDfx.enabled";
    const std::string oldDebugValue = system::GetParameter(debugKey, "0");

    (void)system::SetParameter(debugKey, "1");
    EXPECT_TRUE(RSSystemProperties::GetLayerPartRenderDebugEnabled());

    opincDrawCache.LayerDirtyRegionDfx(paintFilterCanvas, dirtyRect);

    (void)system::SetParameter(debugKey, oldDebugValue);
}
} // namespace OHOS::Rosen
