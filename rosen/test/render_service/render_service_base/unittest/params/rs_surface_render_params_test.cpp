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
#include "pipeline/rs_display_render_node.h"
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
class RSSurfaceRenderParamsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};
void RSSurfaceRenderParamsTest::SetUpTestCase() {}
void RSSurfaceRenderParamsTest::TearDownTestCase() {}
void RSSurfaceRenderParamsTest::SetUp() {}
void RSSurfaceRenderParamsTest::TearDown() {}

/**
 * @tc.name: SetOccludedByFilterCache
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderParamsTest, SetOccludedByFilterCache, TestSize.Level1)
{
    RSSurfaceRenderParams params(100);
    params.SetOccludedByFilterCache(false);
    EXPECT_EQ(params.needSync_, false);

    params.SetOccludedByFilterCache(true);
    EXPECT_EQ(params.needSync_, true);
}

/**
 * @tc.name: SetFilterCacheFullyCovered
 * @tc.desc: test SetFilterCacheFullyCovered successfully
 * @tc.type: FUNC
 * @tc.require: issueIAL2EA
 */
HWTEST_F(RSSurfaceRenderParamsTest, SetFilterCacheFullyCovered, TestSize.Level1)
{
    RSSurfaceRenderParams params(DEFAULT_NODEID);
    params.SetFilterCacheFullyCovered(true);
    EXPECT_EQ(params.GetFilterCacheFullyCovered(), true);
}

/**
 * @tc.name: CheckValidFilterCacheFullyCoverTarget
 * @tc.desc: test CheckValidFilterCacheFullyCoverTarget correctly
 * @tc.type: FUNC
 * @tc.require: issueIAL2EA
 */
HWTEST_F(RSSurfaceRenderParamsTest, CheckValidFilterCacheFullyCoverTarget, TestSize.Level1)
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
 * @tc.name: SetHardwareEnabled
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderParamsTest, SetHardwareEnabled, TestSize.Level1)
{
    RSSurfaceRenderParams params(101);
    params.SetHardwareEnabled(false);
    EXPECT_EQ(params.needSync_, false);

    params.SetHardwareEnabled(true);
    EXPECT_EQ(params.needSync_, true);
}

/**
 * @tc.name: SetLastFrameHardwareEnabled
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderParamsTest, SetLastFrameHardwareEnabled, TestSize.Level1)
{
    RSSurfaceRenderParams params(102);
    params.SetLastFrameHardwareEnabled(false);
    EXPECT_EQ(params.needSync_, false);

    params.SetLastFrameHardwareEnabled(true);
    EXPECT_EQ(params.needSync_, true);
}

/**
 * @tc.name: SetLayerSourceTuning
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderParamsTest, SetLayerSourceTuning, TestSize.Level1)
{
    RSSurfaceRenderParams params(103);
    params.SetLayerSourceTuning(false);
    EXPECT_EQ(params.needSync_, false);

    params.SetLayerSourceTuning(true);
    EXPECT_EQ(params.needSync_, true);
}

/**
 * @tc.name: SetTunnelLayerId
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderParamsTest, SetTunnelLayerId, TestSize.Level1)
{
    RSSurfaceRenderParams params(DEFAULT_NODEID);
    EXPECT_EQ(params.GetTunnelLayerId(), 0);
    params.SetTunnelLayerId(1);
    EXPECT_EQ(params.GetTunnelLayerId(), 1);
}

/**
 * @tc.name: SetFixRotationByUser
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderParamsTest, SetFixRotationByUser, TestSize.Level1)
{
    RSSurfaceRenderParams params(104);
    params.SetFixRotationByUser(false);
    EXPECT_EQ(params.needSync_, false);

    params.SetFixRotationByUser(true);
    EXPECT_EQ(params.needSync_, true);
}


/**
 * @tc.name: SetSurfaceCacheContentStatic
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderParamsTest, SetSurfaceCacheContentStatic, TestSize.Level1)
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
 * @tc.name: SetSurfaceSubTreeDirty
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderParamsTest, SetSurfaceSubTreeDirty, TestSize.Level1)
{
    RSSurfaceRenderParams params(107);
    params.SetSurfaceSubTreeDirty(false);
    EXPECT_EQ(params.needSync_, false);

    params.SetSurfaceSubTreeDirty(true);
    EXPECT_EQ(params.needSync_, true);
}

/**
* @tc.name: SetGpuOverDrawBufferOptimizeNode
* @tc.desc:
* @tc.type:FUNC
* @tc.require:
*/
HWTEST_F(RSSurfaceRenderParamsTest, SetGpuOverDrawBufferOptimizeNode, TestSize.Level1)
{
    RSSurfaceRenderParams params(108);
    params.SetGpuOverDrawBufferOptimizeNode(false);
    EXPECT_EQ(params.needSync_, false);

    params.SetGpuOverDrawBufferOptimizeNode(true);
    EXPECT_EQ(params.needSync_, true);
}

/**
 * @tc.name: SetOverDrawBufferNodeCornerRadius
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderParamsTest, SetOverDrawBufferNodeCornerRadius, TestSize.Level1)
{
    RSSurfaceRenderParams params(109);
    params.SetOverDrawBufferNodeCornerRadius(false);
    EXPECT_EQ(params.needSync_, false);

    params.SetOverDrawBufferNodeCornerRadius(true);
    EXPECT_EQ(params.needSync_, true);
}

/**
 * @tc.name: DisplayNitTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderParamsTest, DisplayNitTest, TestSize.Level1)
{
    RSSurfaceRenderParams params(110);
    params.SetDisplayNit(SET_DISPLAY_NITS);
    EXPECT_EQ(params.GetDisplayNit(), SET_DISPLAY_NITS);
}

/**
 * @tc.name: BrightnessRatioTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderParamsTest, BrightnessRatioTest, TestSize.Level1)
{
    RSSurfaceRenderParams params(111);
    params.SetBrightnessRatio(SET_BRIGHTNESS_RATIO);
    EXPECT_EQ(params.GetBrightnessRatio(), SET_BRIGHTNESS_RATIO);
}

/**
 * @tc.name: SetGlobalPositionEnabled
 * @tc.desc: SetGlobalPositionEnabled and GetGlobalPositionEnabled test
 * @tc.type:FUNC
 * @tc.require: issueIATYMW
 */
HWTEST_F(RSSurfaceRenderParamsTest, SetGlobalPositionEnabled, TestSize.Level1)
{
    RSSurfaceRenderParams params(112);
    params.SetGlobalPositionEnabled(false);
    EXPECT_EQ(params.GetGlobalPositionEnabled(), false);

    params.SetGlobalPositionEnabled(true);
    EXPECT_EQ(params.GetGlobalPositionEnabled(), true);
}

/**
 * @tc.name: NeedCacheSurface
 * @tc.desc: SetNeedCacheSurface and GetNeedCacheSurface test
 * @tc.type:FUNC
 * @tc.require: issueIAVLLE
 */
HWTEST_F(RSSurfaceRenderParamsTest, SetNeedCacheSurface, TestSize.Level1)
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
 * @tc.name: SetHardCursorStatus
 * @tc.desc: SetHardCursorStatus and GetHardCursorStatus test
 * @tc.type:FUNC
 * @tc.require: issueIAX2SN
 */
HWTEST_F(RSSurfaceRenderParamsTest, SetHardCursorStatusTest, TestSize.Level1)
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
 * @tc.name: SetLayerTop_001
 * @tc.desc: Test function SetLayerTop
 * @tc.type:FUNC
 * @tc.require:issueIB1KXV
 */
HWTEST_F(RSSurfaceRenderParamsTest, SetLayerTop_001, TestSize.Level2)
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
 * @tc.name: SetLayerTop_002
 * @tc.desc: Test function SetLayerTop
 * @tc.type:FUNC
 * @tc.require:issueIB1KXV
 */
HWTEST_F(RSSurfaceRenderParamsTest, SetLayerTop_002, TestSize.Level2)
{
    RSSurfaceRenderParams params(115);
    params.needSync_ = false;
    params.isLayerTop_  = false;

    bool isLayerTop = !params.isLayerTop_;
    params.SetLayerTop(isLayerTop);
    EXPECT_EQ(params.needSync_, true);
    EXPECT_EQ(params.isLayerTop_, isLayerTop);
}

/**
 * @tc.name: SetForceRefresh_001
 * @tc.desc: Test function SetForceRefresh
 * @tc.type:FUNC
 * @tc.require:issueIB1KXV
 */
HWTEST_F(RSSurfaceRenderParamsTest, SetForceRefresh_001, TestSize.Level2)
{
    RSSurfaceRenderParams params(115);
    params.needSync_ = false;
    params.isForceRefresh_  = false;

    bool isForceRefresh = params.isForceRefresh_;
    params.SetForceRefresh(isForceRefresh);
    EXPECT_EQ(params.needSync_, false);
    EXPECT_EQ(params.isForceRefresh_, isForceRefresh);
}

/**
 * @tc.name: SetForceRefresh_002
 * @tc.desc: Test function SetForceRefresh
 * @tc.type:FUNC
 * @tc.require:issueIB1KXV
 */
HWTEST_F(RSSurfaceRenderParamsTest, SetForceRefresh_002, TestSize.Level2)
{
    RSSurfaceRenderParams params(115);
    params.needSync_ = false;
    params.isForceRefresh_  = false;

    bool isForceRefresh = !params.isForceRefresh_;
    params.SetForceRefresh(isForceRefresh);
    EXPECT_EQ(params.needSync_, true);
    EXPECT_EQ(params.isForceRefresh_, isForceRefresh);
}

/**
 * @tc.name: SetIsBufferFlushed
 * @tc.desc: SetIsBufferFlushed and GetIsBufferFlushed test
 * @tc.type:FUNC
 * @tc.require: issueIBEBTA
 */
HWTEST_F(RSSurfaceRenderParamsTest, SetIsBufferFlushed, TestSize.Level1)
{
    RSSurfaceRenderParams params(116);
    params.SetIsBufferFlushed(false);
    EXPECT_EQ(params.GetIsBufferFlushed(), false);

    params.SetIsBufferFlushed(true);
    EXPECT_EQ(params.GetIsBufferFlushed(), true);
}

/**
 * @tc.name: GetClonedNodeRenderDrawable
 * @tc.desc: Test function GetClonedNodeRenderDrawable
 * @tc.type:FUNC
 * @tc.require: issueIBH7WD
 */
HWTEST_F(RSSurfaceRenderParamsTest, GetClonedNodeRenderDrawable, TestSize.Level2)
{
    RSSurfaceRenderParams params(117);
    auto clonedNodeRenderDrawable = params.GetClonedNodeRenderDrawable();
    EXPECT_TRUE(clonedNodeRenderDrawable.expired());
}

/**
 * @tc.name: IsCloneNode
 * @tc.desc: Test function IsCloneNode
 * @tc.type:FUNC
 * @tc.require: issueIBH7WD
 */
HWTEST_F(RSSurfaceRenderParamsTest, IsCloneNode, TestSize.Level2)
{
    RSSurfaceRenderParams params(118);
    auto result = params.IsCloneNode();
    EXPECT_FALSE(result);
}

/**
 * @tc.name: ClonedSourceNode
 * @tc.desc: Test function ClonedSourceNode
 * @tc.type:FUNC
 * @tc.require: issueIBH7WD
 */
HWTEST_F(RSSurfaceRenderParamsTest, ClonedSourceNode, TestSize.Level2)
{
    RSSurfaceRenderParams params(119);
    auto result = params.ClonedSourceNode();
    EXPECT_FALSE(result);
}

/**
 * @tc.name: SetIsCloned
 * @tc.desc: Test function SetIsCloned
 * @tc.type:FUNC
 * @tc.require: issueIBH7WD
 */
HWTEST_F(RSSurfaceRenderParamsTest, SetIsCloned, TestSize.Level2)
{
    RSSurfaceRenderParams params(120);
    params.SetIsCloned(true);
    EXPECT_TRUE(params.clonedSourceNode_);
}

/**
 * @tc.name: isClonedNodeOnTheTree
 * @tc.desc: Test function isClonedNodeOnTheTree
 * @tc.type:FUNC
 * @tc.require: issueIBKU7U
 */
HWTEST_F(RSSurfaceRenderParamsTest, isClonedNodeOnTheTree, TestSize.Level2)
{
    RSSurfaceRenderParams params(121);
    auto result = params.IsClonedNodeOnTheTree();
    EXPECT_FALSE(result);
}

/**
 * @tc.name: SdrNitTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderParamsTest, SdrNitTest, TestSize.Level1)
{
    RSSurfaceRenderParams params(122);
    params.SetSdrNit(SET_DISPLAY_NITS);
    EXPECT_EQ(params.GetSdrNit(), SET_DISPLAY_NITS);
}

/**
 * @tc.name: LayerLinearMatrixTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderParamsTest, LayerLinearMatrixTest, TestSize.Level1)
{
    RSSurfaceRenderParams params(123);
    std::vector<float> matrix = {1.0f, 2.0f, 3.0f};
    params.SetLayerLinearMatrix(matrix);
    EXPECT_EQ(params.GetLayerLinearMatrix(), matrix);
}

/**
 * @tc.name: GetSourceDisplayRenderNodeDrawable
 * @tc.desc: Test function GetSourceDisplayRenderNodeDrawable
 * @tc.type:FUNC
 * @tc.require: issueIBKU7U
 */
HWTEST_F(RSSurfaceRenderParamsTest, GetSourceDisplayRenderNodeDrawable, TestSize.Level1)
{
    constexpr NodeId id = 124;
    RSSurfaceRenderParams params(id);
    params.sourceDisplayRenderNodeDrawable_ = std::weak_ptr<DrawableV2::RSRenderNodeDrawableAdapter>();
    EXPECT_TRUE(params.GetSourceDisplayRenderNodeDrawable().expired());
}

/**
 * @tc.name: SetOffsetXTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderParamsTest, SetOffsetXTest, TestSize.Level1)
{
    RSSurfaceRenderParams params(125);
    params.SetOffsetX(1);
    EXPECT_EQ(params.GetOffsetX(), 1);
}

/**
 * @tc.name: SetOffsetY
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderParamsTest, SetOffsetYTest, TestSize.Level1)
{
    RSSurfaceRenderParams params(126);
    params.SetOffsetY(1);
    EXPECT_EQ(params.GetOffsetY(), 1);
}

/**
 * @tc.name: SetOffsetY
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderParamsTest, SetRogWidthRatioTest, TestSize.Level1)
{
    RSSurfaceRenderParams params(127);
    params.SetRogWidthRatio(1.0);
    EXPECT_EQ(params.GetRogWidthRatio(), 1.0);
}

/**
 * @tc.name: SetFrameGravityNewVersionEnabledTest
 * @tc.desc: SetFrameGravityNewVersionEnabled and GetFrameGravityNewVersionEnabled
 * @tc.type: FUNC
 * @tc.require: issueIC8CDF
 */
HWTEST_F(RSSurfaceRenderParamsTest, SetFrameGravityNewVersionEnabledTest, TestSize.Level1)
{
    RSSurfaceRenderParams params(128);
    params.SetFrameGravityNewVersionEnabled(false);
    EXPECT_EQ(params.GetFrameGravityNewVersionEnabled(), false);

    params.SetFrameGravityNewVersionEnabled(true);
    EXPECT_EQ(params.GetFrameGravityNewVersionEnabled(), true);
}
} // namespace OHOS::Rosen