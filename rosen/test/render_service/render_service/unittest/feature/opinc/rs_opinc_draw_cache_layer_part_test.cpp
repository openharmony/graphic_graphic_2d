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
#include "pipeline/rs_dirty_region_manager.h"
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
    static std::shared_ptr<RSDirtyRegionManager> CreateDirtyManager(bool enabled)
    {
        auto dirtyManager = std::make_shared<RSDirtyRegionManager>();
        if (dirtyManager == nullptr) {
            return nullptr;
        }
        dirtyManager->SetLayerPartRenderEnabled(enabled);
        dirtyManager->SetLayerPartRenderCurrentFrameDirtyRegion(DEFAULT_DIRTY_RECT);
        return dirtyManager;
    }
};

/**
 * @tc.name: PushLayerPartRenderDirtyRegionNullDirtyManager
 * @tc.desc: Verify PushLayerPartRenderDirtyRegion returns early for null manager
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, PushLayerPartRenderDirtyRegionNullDirtyManager, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    RSRenderParams params(DEFAULT_ID);

    opincDrawCache.PushLayerPartRenderDirtyRegion(nullptr, params, paintFilterCanvas, TEST_NODE_COUNT);

    ASSERT_TRUE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
}

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
    RSRenderParams params(DEFAULT_ID);
    auto dirtyManager = CreateDirtyManager(false);

    opincDrawCache.PushLayerPartRenderDirtyRegion(dirtyManager, params, paintFilterCanvas, TEST_NODE_COUNT);

    ASSERT_TRUE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
}

/**
 * @tc.name: PushLayerPartRenderDirtyRegionDisabledWithStateChange
 * @tc.desc: Verify PushLayerPartRenderDirtyRegion returns true when disabled and state change is set
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, PushLayerPartRenderDirtyRegionDisabledWithStateChange, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    RSRenderParams params(DEFAULT_ID);
    auto dirtyManager = CreateDirtyManager(false);
    ASSERT_NE(dirtyManager, nullptr);

    opincDrawCache.PushLayerPartRenderDirtyRegion(dirtyManager, params, paintFilterCanvas, TEST_NODE_COUNT);

    ASSERT_TRUE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
}

/**
 * @tc.name: PushLayerPartRenderDirtyRegionEnabled
 * @tc.desc: Verify PushLayerPartRenderDirtyRegion pushes current dirty region into canvas stack
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, PushLayerPartRenderDirtyRegionEnabled, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    RSRenderParams params(DEFAULT_ID);
    auto dirtyManager = CreateDirtyManager(true);

    opincDrawCache.PushLayerPartRenderDirtyRegion(dirtyManager, params, paintFilterCanvas, TEST_NODE_COUNT);

    ASSERT_FALSE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
}

/**
 * @tc.name: LayerPartRenderClipDirtyRegionNullDirtyManager
 * @tc.desc: Verify LayerPartRenderClipDirtyRegion returns early for null manager
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, LayerPartRenderClipDirtyRegionNullDirtyManager, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);

    opincDrawCache.LayerPartRenderClipDirtyRegion(nullptr, paintFilterCanvas);

    ASSERT_TRUE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
}

/**
 * @tc.name: LayerPartRenderClipDirtyRegionDisabled
 * @tc.desc: Verify LayerPartRenderClipDirtyRegion returns early when feature is disabled
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, LayerPartRenderClipDirtyRegionDisabled, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    auto dirtyManager = CreateDirtyManager(false);

    opincDrawCache.LayerPartRenderClipDirtyRegion(dirtyManager, paintFilterCanvas);

    ASSERT_TRUE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
}

/**
 * @tc.name: PopLayerPartRenderDirtyRegionNullDirtyManager
 * @tc.desc: Verify PopLayerPartRenderDirtyRegion returns early for null manager
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, PopLayerPartRenderDirtyRegionNullDirtyManager, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);

    opincDrawCache.PopLayerPartRenderDirtyRegion(nullptr, paintFilterCanvas);

    ASSERT_TRUE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
}

/**
 * @tc.name: PopLayerPartRenderDirtyRegionEmptyStack
 * @tc.desc: Verify PopLayerPartRenderDirtyRegion handles enabled manager with empty stack
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, PopLayerPartRenderDirtyRegionEmptyStack, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    auto dirtyManager = CreateDirtyManager(true);

    opincDrawCache.PopLayerPartRenderDirtyRegion(dirtyManager, paintFilterCanvas);

    ASSERT_TRUE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
}

/**
 * @tc.name: PopLayerPartRenderDirtyRegionDisabled
 * @tc.desc: Verify PopLayerPartRenderDirtyRegion returns early when feature is disabled
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, PopLayerPartRenderDirtyRegionDisabled, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    auto dirtyManager = CreateDirtyManager(false);

    opincDrawCache.PopLayerPartRenderDirtyRegion(dirtyManager, paintFilterCanvas);

    ASSERT_TRUE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
}

/**
 * @tc.name: LayerPartRenderFullWorkflow
 * @tc.desc: Verify push clip and pop workflow with enabled layer-part dirty manager
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, LayerPartRenderFullWorkflow, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    RSRenderParams params(DEFAULT_ID);
    auto dirtyManager = CreateDirtyManager(true);

    opincDrawCache.PushLayerPartRenderDirtyRegion(dirtyManager, params, paintFilterCanvas, TEST_NODE_COUNT);
    ASSERT_FALSE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());

    opincDrawCache.LayerPartRenderClipDirtyRegion(dirtyManager, paintFilterCanvas);
    opincDrawCache.PopLayerPartRenderDirtyRegion(dirtyManager, paintFilterCanvas);

    ASSERT_TRUE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
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

    opincDrawCache.LayerDirtyRegionDfx(paintFilterCanvas, dirtyRect);

    ASSERT_FALSE(RSSystemProperties::GetLayerPartRenderDebugEnabled());
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

    system::SetParameter("rosen.layerPartRenderDfx.enabled", "1");
    ASSERT_TRUE(RSSystemProperties::GetLayerPartRenderDebugEnabled());

    opincDrawCache.LayerDirtyRegionDfx(paintFilterCanvas, dirtyRect);

    system::SetParameter("rosen.layerPartRenderDfx.enabled", "0");
}
} // namespace OHOS::Rosen
