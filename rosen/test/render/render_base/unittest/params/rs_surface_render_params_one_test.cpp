 /*
 * Copyright (c) 2024-2024 Huawei Device Co., Ltd.
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
class RSSurfaceRenderParamsOneTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};
void RSSurfaceRenderParamsOneTest::SetUpTestCase() {}
void RSSurfaceRenderParamsOneTest::TearDownTestCase() {}
void RSSurfaceRenderParamsOneTest::SetUp() {}
void RSSurfaceRenderParamsOneTest::TearDown() {}

/**
 * @tc.name: SetFilterCacheFullyCoveredTest
 * @tc.desc: test SetFilterCacheFullyCovered successfully
 * @tc.type: FUNC
 * @tc.require: issueIB7RF8
 */
HWTEST_F(RSSurfaceRenderParamsOneTest, SetFilterCacheFullyCoveredTest, TestSize.Level1)
{
    RSSurfaceRenderParams params(DEFAULT_NODEID);
    params.SetFilterCacheFullyCovered(true);
    EXPECT_EQ(params.GetFilterCacheFullyCovered(), true);
}

/**
 * @tc.name: SetOccludedByFilterCacheTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require: issueIB7RF8
 */
HWTEST_F(RSSurfaceRenderParamsOneTest, SetOccludedByFilterCacheTest, TestSize.Level1)
{
    RSSurfaceRenderParams params(100);
    params.SetOccludedByFilterCache(false);
    EXPECT_EQ(params.needSync_, false);

    params.SetOccludedByFilterCache(true);
    EXPECT_EQ(params.needSync_, true);
}

/**
 * @tc.name: SetHardwareEnabledTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require: issueIB7RF8
 */
HWTEST_F(RSSurfaceRenderParamsOneTest, SetHardwareEnabledTest, TestSize.Level1)
{
    RSSurfaceRenderParams params(101);
    params.SetHardwareEnabled(false);
    EXPECT_EQ(params.needSync_, false);

    params.SetHardwareEnabled(true);
    EXPECT_EQ(params.needSync_, true);
}

/**
 * @tc.name: CheckValidFilterCacheFullyCoverTargetTest
 * @tc.desc: test CheckValidFilterCacheFullyCoverTarget correctly
 * @tc.type: FUNC
 * @tc.require: issueIB7RF8
 */
HWTEST_F(RSSurfaceRenderParamsOneTest, CheckValidFilterCacheFullyCoverTargetTest, TestSize.Level1)
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
 * @tc.name: SetLayerSourceTuningTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require: issueIB7RF8
 */
HWTEST_F(RSSurfaceRenderParamsOneTest, SetLayerSourceTuningTest, TestSize.Level1)
{
    RSSurfaceRenderParams params(103);
    params.SetLayerSourceTuning(false);
    EXPECT_EQ(params.needSync_, false);

    params.SetLayerSourceTuning(true);
    EXPECT_EQ(params.needSync_, true);
}

/**
 * @tc.name: SetLastFrameHardwareEnabledTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require: issueIB7RF8
 */
HWTEST_F(RSSurfaceRenderParamsOneTest, SetLastFrameHardwareEnabledTest, TestSize.Level1)
{
    RSSurfaceRenderParams params(102);
    params.SetLastFrameHardwareEnabled(false);
    EXPECT_EQ(params.needSync_, false);

    params.SetLastFrameHardwareEnabled(true);
    EXPECT_EQ(params.needSync_, true);
}

/**
 * @tc.name: SetSurfaceCacheContentStaticTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require: issueIB7RF8
 */
HWTEST_F(RSSurfaceRenderParamsOneTest, SetSurfaceCacheContentStaticTest, TestSize.Level1)
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
 * @tc.name: SetFixRotationByUserTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require: issueIB7RF8
 */
HWTEST_F(RSSurfaceRenderParamsOneTest, SetFixRotationByUserTest, TestSize.Level1)
{
    RSSurfaceRenderParams params(104);
    params.SetFixRotationByUser(false);
    EXPECT_EQ(params.needSync_, false);

    params.SetFixRotationByUser(true);
    EXPECT_EQ(params.needSync_, true);
}

/**
* @tc.name: SetGpuOverDrawBufferOptimizeNodeTest
* @tc.desc:
* @tc.type:FUNC
* @tc.require: issueIB7RF8
*/
HWTEST_F(RSSurfaceRenderParamsOneTest, SetGpuOverDrawBufferOptimizeNodeTest, TestSize.Level1)
{
    RSSurfaceRenderParams params(108);
    params.SetGpuOverDrawBufferOptimizeNode(false);
    EXPECT_EQ(params.needSync_, false);

    params.SetGpuOverDrawBufferOptimizeNode(true);
    EXPECT_EQ(params.needSync_, true);
}

/**
 * @tc.name: SetSurfaceSubTreeDirtyTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require: issueIB7RF8
 */
HWTEST_F(RSSurfaceRenderParamsOneTest, SetSurfaceSubTreeDirtyTest, TestSize.Level1)
{
    RSSurfaceRenderParams params(107);
    params.SetSurfaceSubTreeDirty(false);
    EXPECT_EQ(params.needSync_, false);

    params.SetSurfaceSubTreeDirty(true);
    EXPECT_EQ(params.needSync_, true);
}

/**
 * @tc.name: DisplayNitTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require: issueIB7RF8
 */
HWTEST_F(RSSurfaceRenderParamsOneTest, DisplayNitTest, TestSize.Level1)
{
    RSSurfaceRenderParams params(110);
    params.SetDisplayNit(SET_DISPLAY_NITS);
    EXPECT_EQ(params.GetDisplayNit(), SET_DISPLAY_NITS);
}
/**
 * @tc.name: SetOverDrawBufferNodeCornerRadiusTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require: issueIB7RF8
 */
HWTEST_F(RSSurfaceRenderParamsOneTest, SetOverDrawBufferNodeCornerRadiusTest, TestSize.Level1)
{
    RSSurfaceRenderParams params(109);
    params.SetOverDrawBufferNodeCornerRadius(false);
    EXPECT_EQ(params.needSync_, false);

    params.SetOverDrawBufferNodeCornerRadius(true);
    EXPECT_EQ(params.needSync_, true);
}
/**
 * @tc.name: BrightnessRatioTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require: issueIB7RF8
 */
HWTEST_F(RSSurfaceRenderParamsOneTest, BrightnessRatioTest, TestSize.Level1)
{
    RSSurfaceRenderParams params(111);
    params.SetBrightnessRatio(SET_BRIGHTNESS_RATIO);
    EXPECT_EQ(params.GetBrightnessRatio(), SET_BRIGHTNESS_RATIO);
}
/**
 * @tc.name: SetGlobalPositionEnabledTest
 * @tc.desc: SetGlobalPositionEnabled and GetGlobalPositionEnabled test
 * @tc.type:FUNC
 * @tc.require: issueIB7RF8
 */
HWTEST_F(RSSurfaceRenderParamsOneTest, SetGlobalPositionEnabledTest, TestSize.Level1)
{
    RSSurfaceRenderParams params(112);
    params.SetGlobalPositionEnabled(false);
    EXPECT_EQ(params.GetGlobalPositionEnabled(), false);

    params.SetGlobalPositionEnabled(true);
    EXPECT_EQ(params.GetGlobalPositionEnabled(), true);
}
/**
 * @tc.name: SetHardCursorStatusTest
 * @tc.desc: SetHardCursorStatus and GetHardCursorStatus test
 * @tc.type:FUNC
 * @tc.require: issueIB7RF8
 */
HWTEST_F(RSSurfaceRenderParamsOneTest, SetHardCursorStatusTest, TestSize.Level1)
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
 * @tc.name: NeedCacheSurfaceTest
 * @tc.desc: SetNeedCacheSurface and GetNeedCacheSurface test
 * @tc.type:FUNC
 * @tc.require: issueIB7RF8
 */
HWTEST_F(RSSurfaceRenderParamsOneTest, SetNeedCacheSurfaceTest, TestSize.Level1)
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
 * @tc.name: SetLayerTopTest_001
 * @tc.desc: Test function SetLayerTop
 * @tc.type:FUNC
 * @tc.require:issueIB7RF8
 */
HWTEST_F(RSSurfaceRenderParamsOneTest, SetLayerTopTest_001, TestSize.Level2)
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
 * @tc.name: SetLayerTopTest_002
 * @tc.desc: Test function SetLayerTop
 * @tc.type:FUNC
 * @tc.require:issueIB7RF8
 */
HWTEST_F(RSSurfaceRenderParamsOneTest, SetLayerTopTest_002, TestSize.Level2)
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