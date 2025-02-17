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
#include "params/rs_surface_render_params.h"
#include "limit_number.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
const RectI DEFAULT_RECT = {0, 0, 100, 100};
constexpr NodeId DEFAULT_NODEID = 1;
} // namespace

constexpr int32_t SET_DISPLAY_NITS = 100;
constexpr float_t SET_BRIGHTNESS_RATIO = 0.5f;
class RSSurfaceRenderParamsUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};
void RSSurfaceRenderParamsUnitTest::SetUpTestCase() {}
void RSSurfaceRenderParamsUnitTest::TearDownTestCase() {}
void RSSurfaceRenderParamsUnitTest::SetUp() {}
void RSSurfaceRenderParamsUnitTest::TearDown() {}

/**
 * @tc.name: TestSetOccludedByFilterCache
 * @tc.desc: Test function SetOccludedByFilterCache
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderParamsUnitTest, TestSetOccludedByFilterCache, TestSize.Level1)
{
    RSSurfaceRenderParams params(100);
    params.SetOccludedByFilterCache(false);
    EXPECT_EQ(params.needSync_, false);

    params.SetOccludedByFilterCache(true);
    EXPECT_EQ(params.needSync_, true);
}

/**
 * @tc.name: TestSetFilterCacheFullyCovered
 * @tc.desc: Test function SetFilterCacheFullyCovered and GetFilterCacheFullyCovered
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderParamsUnitTest, TestSetFilterCacheFullyCovered, TestSize.Level1)
{
    RSSurfaceRenderParams params(DEFAULT_NODEID);
    params.SetFilterCacheFullyCovered(true);
    EXPECT_EQ(params.GetFilterCacheFullyCovered(), true);
}

/**
 * @tc.name: TestCheckValidFilterCacheFullyCoverTarget
 * @tc.desc: Test function CheckValidFilterCacheFullyCoverTarget
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderParamsUnitTest, TestCheckValidFilterCacheFullyCoverTarget, TestSize.Level1)
{
    RSSurfaceRenderParams params(DEFAULT_NODEID);
    params.isFilterCacheFullyCovered_ = false;
    params.CheckValidFilterCacheFullyCoverTarget(true, DEFAULT_RECT, DEFAULT_RECT);
    EXPECT_EQ(params.isFilterCacheFullyCovered_, true);

    params.isFilterCacheFullyCovered_ = false;
    params.CheckValidFilterCacheFullyCoverTarget(false, DEFAULT_RECT, DEFAULT_RECT);
    EXPECT_EQ(params.isFilterCacheFullyCovered_, false);

    params.isFilterCacheFullyCovered_ = true;
    params.CheckValidFilterCacheFullyCoverTarget(false, DEFAULT_RECT, DEFAULT_RECT);
    EXPECT_EQ(params.isFilterCacheFullyCovered_, true);
}

/**
 * @tc.name: TestSetHardwareEnabled
 * @tc.desc: Test function SetHardwareEnabled
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderParamsUnitTest, TestSetHardwareEnabled, TestSize.Level1)
{
    RSSurfaceRenderParams params(101);
    params.SetHardwareEnabled(false);
    EXPECT_EQ(params.needSync_, false);

    params.SetHardwareEnabled(true);
    EXPECT_EQ(params.needSync_, true);
}

/**
 * @tc.name: TestSetLastFrameHardwareEnabled
 * @tc.desc: Test function SetLastFrameHardwareEnabled
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderParamsUnitTest, TestSetLastFrameHardwareEnabled, TestSize.Level1)
{
    RSSurfaceRenderParams params(102);
    params.SetLastFrameHardwareEnabled(false);
    EXPECT_EQ(params.needSync_, false);

    params.SetLastFrameHardwareEnabled(true);
    EXPECT_EQ(params.needSync_, true);
}

/**
 * @tc.name: TestSetLayerSourceTuning
 * @tc.desc: Test function SetLayerSourceTuning
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderParamsUnitTest, TestSetLayerSourceTuning, TestSize.Level1)
{
    RSSurfaceRenderParams params(103);
    params.SetLayerSourceTuning(false);
    EXPECT_EQ(params.needSync_, false);

    params.SetLayerSourceTuning(true);
    EXPECT_EQ(params.needSync_, true);
}

/**
 * @tc.name: TestSetFixRotationByUser
 * @tc.desc: Test function SetFixRotationByUser
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderParamsUnitTest, TestSetFixRotationByUser, TestSize.Level1)
{
    RSSurfaceRenderParams params(104);
    params.SetFixRotationByUser(false);
    EXPECT_EQ(params.needSync_, false);

    params.SetFixRotationByUser(true);
    EXPECT_EQ(params.needSync_, true);
}


/**
 * @tc.name: TestSetSurfaceCacheContentStatic
 * @tc.desc: Test function SetSurfaceCacheContentStatic
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderParamsUnitTest, TestSetSurfaceCacheContentStatic, TestSize.Level1)
{
    RSSurfaceRenderParams params(105);
    params.SetSurfaceCacheContentStatic(false, false);
    EXPECT_EQ(params.needSync_, false);

    params.SetSurfaceCacheContentStatic(true, false);
    EXPECT_EQ(params.needSync_, false);

    params.SetSurfaceCacheContentStatic(true, true);
    EXPECT_EQ(params.needSync_, true);

    RSSurfaceRenderParams paramsAno(106);
    paramsAno.surfaceCacheContentStatic_ = true;
    params.SetSurfaceCacheContentStatic(false, true);
    EXPECT_EQ(params.needSync_, true);

    params.SetSurfaceCacheContentStatic(false, false);
    EXPECT_EQ(params.needSync_, true);
}

/**
 * @tc.name: TestSetSurfaceSubTreeDirty
 * @tc.desc: Test function SetSurfaceSubTreeDirty
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderParamsUnitTest, TestSetSurfaceSubTreeDirty, TestSize.Level1)
{
    RSSurfaceRenderParams params(107);
    params.SetSurfaceSubTreeDirty(false);
    EXPECT_EQ(params.needSync_, false);

    params.SetSurfaceSubTreeDirty(true);
    EXPECT_EQ(params.needSync_, true);
}

/**
* @tc.name: TestSetGpuOverDrawBufferOptimizeNode
* @tc.desc: Test function SetGpuOverDrawBufferOptimizeNode
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(RSSurfaceRenderParamsUnitTest, TestSetGpuOverDrawBufferOptimizeNode, TestSize.Level1)
{
    RSSurfaceRenderParams params(108);
    params.SetGpuOverDrawBufferOptimizeNode(false);
    EXPECT_EQ(params.needSync_, false);

    params.SetGpuOverDrawBufferOptimizeNode(true);
    EXPECT_EQ(params.needSync_, true);
}

/**
 * @tc.name: TestSetOverDrawBufferNodeCornerRadius
 * @tc.desc: Test function SetOverDrawBufferNodeCornerRadius
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderParamsUnitTest, TestSetOverDrawBufferNodeCornerRadius, TestSize.Level1)
{
    RSSurfaceRenderParams params(109);
    params.SetOverDrawBufferNodeCornerRadius(false);
    EXPECT_EQ(params.needSync_, false);

    params.SetOverDrawBufferNodeCornerRadius(true);
    EXPECT_EQ(params.needSync_, true);
}

/**
 * @tc.name: TestDisplayNit
 * @tc.desc: Test function SetDisplayNit and GetDisplayNit
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderParamsUnitTest, TestDisplayNit, TestSize.Level1)
{
    RSSurfaceRenderParams params(110);
    params.SetDisplayNit(SET_DISPLAY_NITS);
    EXPECT_EQ(params.GetDisplayNit(), SET_DISPLAY_NITS);
}

/**
 * @tc.name: TestBrightnessRatio
 * @tc.desc: Test function SetBrightnessRatio and GetBrightnessRatio
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderParamsUnitTest, TestBrightnessRatio, TestSize.Level1)
{
    RSSurfaceRenderParams params(111);
    params.SetBrightnessRatio(SET_BRIGHTNESS_RATIO);
    EXPECT_EQ(params.GetBrightnessRatio(), SET_BRIGHTNESS_RATIO);
}

/**
 * @tc.name: TestSetGlobalPositionEnabled
 * @tc.desc: Test function SetGlobalPositionEnabled and GetGlobalPositionEnabled
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderParamsUnitTest, TestSetGlobalPositionEnabled, TestSize.Level1)
{
    RSSurfaceRenderParams params(112);
    params.SetGlobalPositionEnabled(false);
    EXPECT_EQ(params.GetGlobalPositionEnabled(), false);

    params.SetGlobalPositionEnabled(true);
    EXPECT_EQ(params.GetGlobalPositionEnabled(), true);
}

/**
 * @tc.name: TestSetNeedCacheSurface
 * @tc.desc: Test function SetNeedCacheSurface and GetNeedCacheSurface
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderParamsUnitTest, TestSetNeedCacheSurface, TestSize.Level1)
{
    uint64_t nodeId = 113;
    RSSurfaceRenderParams params(nodeId);
    params.SetNeedCacheSurface(true);
    EXPECT_EQ(params.GetNeedCacheSurface(), true);
    EXPECT_EQ(params.needSync_, true);

    params.SetNeedCacheSurface(false);
    EXPECT_EQ(params.GetNeedCacheSurface(), false);
    EXPECT_EQ(params.needSync_, true);
}

/**
 * @tc.name: TestSetHardCursorStatus
 * @tc.desc: Test function SetHardCursorStatus and GetHardCursorStatus
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderParamsUnitTest, TestSetHardCursorStatus, TestSize.Level1)
{
    RSSurfaceRenderParams params(114);
    params.SetHardCursorStatus(false);
    EXPECT_EQ(params.needSync_, false);
    EXPECT_EQ(params.GetHardCursorStatus(), false);

    params.SetHardCursorStatus(true);
    EXPECT_EQ(params.needSync_, true);
    EXPECT_EQ(params.GetHardCursorStatus(), true);
}

/**
 * @tc.name: TestSetLayerTop_01
 * @tc.desc: Test function SetLayerTop
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderParamsUnitTest, TestSetLayerTop_01, TestSize.Level2)
{
    RSSurfaceRenderParams params(115);
    params.needSync_ = false;
    params.isLayerTop_  = false;

    bool isLayerTop = params.isLayerTop_;
    params.SetLayerTop(isLayerTop);
    EXPECT_EQ(params.needSync_, false);
    EXPECT_EQ(params.isLayerTop_, isLayerTop);
}

/**
 * @tc.name: TestSetLayerTop_02
 * @tc.desc: Test function SetLayerTop
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderParamsUnitTest, TestSetLayerTop_02, TestSize.Level2)
{
    RSSurfaceRenderParams params(115);
    params.needSync_ = false;
    params.isLayerTop_  = false;

    bool isLayerTop = !params.isLayerTop_;
    params.SetLayerTop(isLayerTop);
    EXPECT_EQ(params.needSync_, true);
    EXPECT_EQ(params.isLayerTop_, isLayerTop);
}
} // namespace OHOS::Rosen