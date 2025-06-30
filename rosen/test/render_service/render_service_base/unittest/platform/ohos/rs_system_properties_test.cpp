/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <cstdlib>
#include <gtest/gtest.h>
#include <parameter.h>
#include <parameters.h>
#include <unistd.h>

#include "param/sys_param.h"
#include "platform/common/rs_system_properties.h"
#include "transaction/rs_render_service_client.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSSystemPropertiesTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSystemPropertiesTest::SetUpTestCase() {}
void RSSystemPropertiesTest::TearDownTestCase() {}
void RSSystemPropertiesTest::SetUp() {}
void RSSystemPropertiesTest::TearDown() {}

/**
 * @tc.name: IsSceneBoardEnabled
 * @tc.desc: IsSceneBoardEnabled Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, IsSceneBoardEnabled, TestSize.Level1)
{
    ASSERT_FALSE(RSSystemProperties::IsSceneBoardEnabled());
}

/**
 * @tc.name: GetDumpFrameNum
 * @tc.desc: GetDumpFrameNum Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetDumpFrameNum, TestSize.Level1)
{
    ASSERT_EQ(RSSystemProperties::GetDumpFrameNum(), 0);
}

/**
 * @tc.name: GetRecordingEnabled
 * @tc.desc: GetRecordingEnabled Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetRecordingEnabled, TestSize.Level1)
{
    ASSERT_EQ(RSSystemProperties::GetRecordingEnabled(), 0);
}

/**
 * @tc.name: GetUniRenderEnabled
 * @tc.desc: GetUniRenderEnabled Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetUniRenderEnabled, TestSize.Level1)
{
    auto isUniRenderEnabled = std::static_pointer_cast<RSRenderServiceClient>
        (RSIRenderClient::CreateRenderServiceClient())->GetUniRenderEnabled();
    ASSERT_EQ(RSSystemProperties::GetUniRenderEnabled(), isUniRenderEnabled);
    // it is not possible to change RSSystemProperties::isUniRenderEnabled from test
    // check the second API calling returns the same value
    ASSERT_EQ(RSSystemProperties::GetUniRenderEnabled(), isUniRenderEnabled);
}

/**
 * @tc.name: SetRecordingDisenabled
 * @tc.desc: SetRecordingDisenabled Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, SetRecordingDisenabled, TestSize.Level1)
{
    RSSystemProperties::SetRecordingDisenabled();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: GetProfilerEnabled
 * @tc.desc: GetProfilerEnabled Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetProfilerEnabled, TestSize.Level1)
{
    ASSERT_EQ(RSSystemProperties::GetProfilerEnabled(), 0);
}

/**
 * @tc.name: GetPixelCheckEnabled
 * @tc.desc: GetPixelCheckEnabled Test
 * @tc.type:FUNC
 * @tc.require: issueIC7V62
 */
HWTEST_F(RSSystemPropertiesTest, GetPixelCheckEnabled, TestSize.Level1)
{
    ASSERT_EQ(RSSystemProperties::GetProfilerPixelCheckMode(), 0);
}

/**
 * @tc.name: SetInstantRecording
 * @tc.desc: SetInstantRecording Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, SetInstantRecording, TestSize.Level1)
{
    bool flag = true;
    RSSystemProperties::SetInstantRecording(flag);
    ASSERT_TRUE(flag);
    flag = false;
    RSSystemProperties::SetInstantRecording(flag);
}

/**
 * @tc.name: GetSaveRDC
 * @tc.desc: GetSaveRDC Test
 * @tc.type:FUNC
 * @tc.require: issueI9VPIT
 */
HWTEST_F(RSSystemPropertiesTest, GetSaveRDC, TestSize.Level1)
{
    RSSystemProperties::SetSaveRDC(true);
    ASSERT_TRUE(RSSystemProperties::GetSaveRDC());
}

/**
 * @tc.name: SetSaveRDC
 * @tc.desc: SetSaveRDC Test
 * @tc.type:FUNC
 * @tc.require: issueI9VPIT
 */
HWTEST_F(RSSystemPropertiesTest, SetSaveRDC, TestSize.Level1)
{
    RSSystemProperties::SetSaveRDC(false);
    ASSERT_FALSE(RSSystemProperties::GetSaveRDC());
}

/**
 * @tc.name: GetRecordingFile
 * @tc.desc: GetRecordingFile Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetRecordingFile, TestSize.Level1)
{
    ASSERT_EQ(RSSystemProperties::GetRecordingFile().size(), 0);
}

/**
 * @tc.name: GetDrawOpTraceEnabled
 * @tc.desc: GetDrawOpTraceEnabled Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetDrawOpTraceEnabled, TestSize.Level1)
{
    ASSERT_FALSE(RSSystemProperties::GetDrawOpTraceEnabled());
}

/**
 * @tc.name: GetRenderNodeTraceEnabled
 * @tc.desc: GetRenderNodeTraceEnabled Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetRenderNodeTraceEnabled, TestSize.Level1)
{
    ASSERT_FALSE(RSSystemProperties::GetRenderNodeTraceEnabled());
}

/**
 * @tc.name: GetRSScreenRoundCornerEnable
 * @tc.desc: GetRSScreenRoundCornerEnable Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetRSScreenRoundCornerEnable, TestSize.Level1)
{
    ASSERT_TRUE(RSSystemProperties::GetRSScreenRoundCornerEnable());
}

/**
 * @tc.name: GetDirtyRegionDebugType
 * @tc.desc: GetDirtyRegionDebugType Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetDirtyRegionDebugType, TestSize.Level1)
{
    ASSERT_EQ(RSSystemProperties::GetDirtyRegionDebugType(), DirtyRegionDebugType::DISABLED);
}

/**
 * @tc.name: GetPartialRenderEnabled
 * @tc.desc: GetPartialRenderEnabled Test
 * @tc.type: FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetPartialRenderEnabled, TestSize.Level1)
{
    ASSERT_NE(RSSystemProperties::GetPartialRenderEnabled(), PartialRenderType::DISABLED);
}

/**
 * @tc.name: GetUniPartialRenderEnabled
 * @tc.desc: GetUniPartialRenderEnabled Test
 * @tc.type: FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetUniPartialRenderEnabled, TestSize.Level1)
{
    ASSERT_EQ(
        RSSystemProperties::GetUniPartialRenderEnabled(), PartialRenderType::SET_DAMAGE_AND_DROP_OP_NOT_VISIBLEDIRTY);
}

/**
 * @tc.name: GetVirtualDirtyEnabled
 * @tc.desc: GetVirtualDirtyEnabled Test
 * @tc.type: FUNC
 * @tc.require: issueICCV9N
 */
HWTEST_F(RSSystemPropertiesTest, GetVirtualDirtyEnabled, TestSize.Level1)
{
    auto type = system::GetParameter("rosen.uni.virtualdirty.enabled", "1");
    ASSERT_EQ(std::to_string(RSSystemProperties::GetVirtualDirtyEnabled()), type);
}

/**
 * @tc.name: GetVirtualExpandScreenSkipEnabled
 * @tc.desc: GetVirtualExpandScreenSkipEnabled Test
 * @tc.type: FUNC
 * @tc.require: issueICCV9N
 */
HWTEST_F(RSSystemPropertiesTest, GetVirtualExpandScreenSkipEnabled, TestSize.Level1)
{
    auto type = system::GetParameter("rosen.uni.virtualexpandscreenskip.enabled", "1");
    ASSERT_EQ(std::to_string(RSSystemProperties::GetVirtualExpandScreenSkipEnabled()), type);
}

/**
 * @tc.name: GetStencilPixelOcclusionCullingEnabled
 * @tc.desc: GetStencilPixelOcclusionCullingEnabled Test
 * @tc.type:FUNC
 * @tc.require: issueIBROZ2
 */
HWTEST_F(RSSystemPropertiesTest, GetStencilPixelOcclusionCullingEnabled, TestSize.Level1)
{
    EXPECT_EQ(
        RSSystemProperties::GetStencilPixelOcclusionCullingEnabled(), StencilPixelOcclusionCullingType::DEFAULT);
}

/**
 * @tc.name: GetReleaseResourceEnabled
 * @tc.desc: GetReleaseResourceEnabled Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetReleaseResourceEnabled, TestSize.Level1)
{
    ASSERT_TRUE(RSSystemProperties::GetReleaseResourceEnabled());
}

/**
 * @tc.name: GetOcclusionEnabled
 * @tc.desc: GetOcclusionEnabled Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetOcclusionEnabled, TestSize.Level1)
{
    ASSERT_TRUE(RSSystemProperties::GetOcclusionEnabled());
}

/**
 * @tc.name: GetHardwareComposerEnabled
 * @tc.desc: GetHardwareComposerEnabled Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetHardwareComposerEnabled, TestSize.Level1)
{
    ASSERT_TRUE(RSSystemProperties::GetHardwareComposerEnabled());
}

/**
 * @tc.name: GetHwcRegionDfxEnabled
 * @tc.desc: GetHwcRegionDfxEnabled Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetHwcRegionDfxEnabled, TestSize.Level1)
{
    ASSERT_FALSE(RSSystemProperties::GetHwcRegionDfxEnabled());
}

/**
 * @tc.name: GetDrawMirrorCacheImageEnabled
 * @tc.desc: GetDrawMirrorCacheImageEnabled Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetDrawMirrorCacheImageEnabled, TestSize.Level1)
{
    ASSERT_TRUE(RSSystemProperties::GetDrawMirrorCacheImageEnabled());
}

/**
 * @tc.name: GetPixelmapDfxEnabled
 * @tc.desc: GetPixelmapDfxEnabled Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetPixelmapDfxEnabled, TestSize.Level1)
{
    ASSERT_FALSE(RSSystemProperties::GetPixelmapDfxEnabled());
}

/**
 * @tc.name: GetAFBCEnabled
 * @tc.desc: GetAFBCEnabled Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetAFBCEnabled, TestSize.Level1)
{
    ASSERT_TRUE(RSSystemProperties::GetAFBCEnabled());
}

/**
 * @tc.name: GetRSEventProperty
 * @tc.desc: GetRSEventProperty Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetRSEventProperty, TestSize.Level1)
{
    ASSERT_EQ(RSSystemProperties::GetRSEventProperty(std::string("0")), std::string("0"));
}

/**
 * @tc.name: GetHighContrastStatus
 * @tc.desc: GetHighContrastStatus Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetHighContrastStatus, TestSize.Level1)
{
    ASSERT_FALSE(RSSystemProperties::GetHighContrastStatus());
}

/**
 * @tc.name: GetDrmEnabled
 * @tc.desc: GetDrmEnabled Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetDrmEnabled, TestSize.Level1)
{
    ASSERT_TRUE(RSSystemProperties::GetDrmEnabled());
}

/**
 * @tc.name: GetTargetDirtyRegionDfxEnabled
 * @tc.desc: GetDrmEGetTargetDirtyRegionDfxEnablednabled Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetTargetDirtyRegionDfxEnabled, TestSize.Level1)
{
    std::vector<std::string> dfxTargetSurfaceNames;
    system::SetParameter("rosen.dirtyregiondebug.surfacenames", "0");
    EXPECT_FALSE(RSSystemProperties::GetTargetDirtyRegionDfxEnabled(dfxTargetSurfaceNames));

    std::string targetStr("A,B,C,D");
    system::SetParameter("rosen.dirtyregiondebug.surfacenames", targetStr);
    EXPECT_TRUE(RSSystemProperties::GetTargetDirtyRegionDfxEnabled(dfxTargetSurfaceNames));
    system::SetParameter("rosen.dirtyregiondebug.surfacenames", "");
}

/**
 * @tc.name: GetOpaqueRegionDfxEnabled
 * @tc.desc: GetOpaqueRegionDfxEnabled Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetOpaqueRegionDfxEnabled, TestSize.Level1)
{
    ASSERT_FALSE(RSSystemProperties::GetOpaqueRegionDfxEnabled());
}

/**
 * @tc.name: GetVisibleRegionDfxEnabled
 * @tc.desc: GetVisibleRegionDfxEnabled Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetVisibleRegionDfxEnabled, TestSize.Level1)
{
    ASSERT_FALSE(RSSystemProperties::GetVisibleRegionDfxEnabled());
}

/**
 * @tc.name: GetSurfaceRegionDfxType
 * @tc.desc: GetSurfaceRegionDfxType Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetSurfaceRegionDfxType, TestSize.Level1)
{
    ASSERT_EQ(RSSystemProperties::GetSurfaceRegionDfxType(), SurfaceRegionDebugType::DISABLED);
}

/**
 * @tc.name: GetCorrectionMode
 * @tc.desc: GetCorrectionMode Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetCorrectionMode, TestSize.Level1)
{
    ASSERT_TRUE(RSSystemProperties::GetCorrectionMode());
}

/**
 * @tc.name: GetDumpSurfaceType
 * @tc.desc: GetDumpSurfaceType Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetDumpSurfaceType, TestSize.Level1)
{
    ASSERT_EQ(RSSystemProperties::GetDumpSurfaceType(), DumpSurfaceType::DISABLED);
}

/**
 * @tc.name: GetDumpSurfaceId
 * @tc.desc: GetDumpSurfaceId Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetDumpSurfaceId, TestSize.Level1)
{
    ASSERT_EQ(RSSystemProperties::GetDumpSurfaceId(), 0);
}

/**
 * @tc.name: GetDumpLayersEnabled
 * @tc.desc: GetDumpLayersEnabled Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetDumpLayersEnabled, TestSize.Level1)
{
    ASSERT_FALSE(RSSystemProperties::GetDumpLayersEnabled());
}

/**
 * @tc.name: GetDrawTextAsBitmap
 * @tc.desc: GetDrawTextAsBitmap Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetDrawTextAsBitmap, TestSize.Level1)
{
    RSSystemProperties::SetDrawTextAsBitmap(true);
    ASSERT_TRUE(RSSystemProperties::GetDrawTextAsBitmap());
}

/**
 * @tc.name: GetCacheEnabledForRotation
 * @tc.desc: GetCacheEnabledForRotation Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetCacheEnabledForRotation, TestSize.Level1)
{
    RSSystemProperties::SetCacheEnabledForRotation(true);
    ASSERT_TRUE(RSSystemProperties::GetCacheEnabledForRotation());
}

/**
 * @tc.name: GetPrepareParallelRenderingEnabled
 * @tc.desc: GetPrepareParallelRenderingEnabled Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetPrepareParallelRenderingEnabled, TestSize.Level1)
{
    ASSERT_EQ(RSSystemProperties::GetPrepareParallelRenderingEnabled(), ParallelRenderingType::DISABLE);
}

/**
 * @tc.name: GetParallelRenderingEnabled
 * @tc.desc: GetParallelRenderingEnabled Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetParallelRenderingEnabled, TestSize.Level1)
{
    ASSERT_EQ(RSSystemProperties::GetParallelRenderingEnabled(), ParallelRenderingType::AUTO);
}

/**
 * @tc.name: GetHgmRefreshRatesEnabled
 * @tc.desc: GetHgmRefreshRatesEnabled and SetHgmRefreshRateModesEnabled Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetHgmRefreshRatesEnabled, TestSize.Level1)
{
    RSSystemProperties::SetHgmRefreshRateModesEnabled(std::string("0"));
    ASSERT_EQ(RSSystemProperties::GetHgmRefreshRatesEnabled(), HgmRefreshRates::SET_RATE_NULL);
}

/**
 * @tc.name: GetHgmRefreshRateModesEnabled
 * @tc.desc: GetHgmRefreshRateModesEnabled Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetHgmRefreshRateModesEnabled, TestSize.Level1)
{
    ASSERT_EQ(RSSystemProperties::GetHgmRefreshRateModesEnabled(), HgmRefreshRateModes::SET_RATE_MODE_NULL);
}

/**
 * @tc.name: GetSkipForAlphaZeroEnabled
 * @tc.desc: GetSkipForAlphaZeroEnabled Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetSkipForAlphaZeroEnabled, TestSize.Level1)
{
    ASSERT_TRUE(RSSystemProperties::GetSkipForAlphaZeroEnabled());
}

/**
 * @tc.name: GetSkipGeometryNotChangeEnabled
 * @tc.desc: GetSkipGeometryNotChangeEnabled Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetSkipGeometryNotChangeEnabled, TestSize.Level1)
{
    ASSERT_TRUE(RSSystemProperties::GetSkipGeometryNotChangeEnabled());
}

/**
 * @tc.name: GetAnimationCacheEnabled
 * @tc.desc: GetAnimationCacheEnabled Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetAnimationCacheEnabled, TestSize.Level1)
{
    ASSERT_FALSE(RSSystemProperties::GetAnimationCacheEnabled());
}

/**
 * @tc.name: GetAnimationScale
 * @tc.desc: GetAnimationScale Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetAnimationScale, TestSize.Level1)
{
    ASSERT_EQ(RSSystemProperties::GetAnimationScale(), 1.0f);
}

/**
 * @tc.name: GetAnimationDelayOptimizeEnabled
 * @tc.desc: GetAnimationDelayOptimizeEnabled Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetAnimationDelayOptimizeEnabled, TestSize.Level1)
{
    ASSERT_TRUE(RSSystemProperties::GetAnimationDelayOptimizeEnabled());
}

/**
 * @tc.name: GetHdrImageEnabled
 * @tc.desc: GetHdrImageEnabled Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetHdrImageEnabledTest, TestSize.Level1)
{
    ASSERT_TRUE(RSSystemProperties::GetHdrImageEnabled());
}

/**
 * @tc.name: GetHdrVideoEnabled
 * @tc.desc: GetHdrVideoEnabled Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetHdrVideoEnabledTest, TestSize.Level1)
{
    ASSERT_TRUE(RSSystemProperties::GetHdrVideoEnabled());
}

/**
 * @tc.name: GetFilterCacheEnabled
 * @tc.desc: GetFilterCacheEnabled Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetFilterCacheEnabled, TestSize.Level1)
{
    ASSERT_TRUE(RSSystemProperties::GetFilterCacheEnabled());
}

/**
 * @tc.name: GetFilterCacheUpdateInterval
 * @tc.desc: GetFilterCacheUpdateInterval Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetFilterCacheUpdateInterval, TestSize.Level1)
{
    ASSERT_EQ(RSSystemProperties::GetFilterCacheUpdateInterval(), 1);
}

/**
 * @tc.name: GetFilterCacheSizeThreshold
 * @tc.desc: GetFilterCacheSizeThreshold Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetFilterCacheSizeThreshold, TestSize.Level1)
{
    ASSERT_EQ(RSSystemProperties::GetFilterCacheSizeThreshold(), 400);
}

/**
 * @tc.name: GetMaskLinearBlurEnabled
 * @tc.desc: GetMaskLinearBlurEnabled Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetMaskLinearBlurEnabled, TestSize.Level1)
{
    ASSERT_EQ(RSSystemProperties::GetMaskLinearBlurEnabled(), 1);
}

/**
 * @tc.name: GetMotionBlurEnabled
 * @tc.desc: GetMotionBlurEnabled Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetMotionBlurEnabled, TestSize.Level1)
{
    ASSERT_TRUE(RSSystemProperties::GetMotionBlurEnabled());
}

/**
 * @tc.name: GetKawaseEnabled
 * @tc.desc: GetKawaseEnabled Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetKawaseEnabled, TestSize.Level1)
{
    ASSERT_TRUE(RSSystemProperties::GetKawaseEnabled());
}

/**
 * @tc.name: GetMESABlurFuzedEnabled
 * @tc.desc: GetMESABlurFuzedEnabled Test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSystemPropertiesTest, GetMESABlurFuzedEnabled, TestSize.Level1)
{
    ASSERT_TRUE(RSSystemProperties::GetMESABlurFuzedEnabled());
}

/**
 * @tc.name: GetForceKawaseDisabled
 * @tc.desc: GetForceKawaseDisabled Test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSystemPropertiesTest, GetForceKawaseDisabled, TestSize.Level1)
{
    ASSERT_FALSE(RSSystemProperties::GetForceKawaseDisabled());
}

/**
 * @tc.name: GetSimplifiedMesaEnabled
 * @tc.desc: GetSimplifiedMesaEnabled Test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSystemPropertiesTest, GetSimplifiedMesaEnabled, TestSize.Level1)
{
    ASSERT_EQ(RSSystemProperties::GetSimplifiedMesaEnabled(), 0);
}

/**
 * @tc.name: SetForceHpsBlurDisabled
 * @tc.desc: SetForceHpsBlurDisabled Test
 * @tc.type:FUNC
 * @tc.require: issueIAAI9H
 */
HWTEST_F(RSSystemPropertiesTest, SetForceHpsBlurDisabled, TestSize.Level1)
{
    RSSystemProperties::SetForceHpsBlurDisabled(true);
    ASSERT_TRUE(!RSSystemProperties::GetHpsBlurEnabled());
    RSSystemProperties::SetForceHpsBlurDisabled(false);
    ASSERT_TRUE(RSSystemProperties::GetHpsBlurEnabled());
}

/**
 * @tc.name: GetHpsBlurEnabled
 * @tc.desc: GetHpsBlurEnabled Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetHpsBlurEnabled, TestSize.Level1)
{
    RSSystemProperties::SetForceHpsBlurDisabled(false);
    ASSERT_TRUE(RSSystemProperties::GetHpsBlurEnabled());
}

/**
 * @tc.name: GetHpsBlurNoiseFactor
 * @tc.desc: GetHpsBlurNoiseFactor Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetHpsBlurNoiseFactor, TestSize.Level1)
{
    ASSERT_TRUE(RSSystemProperties::GetHpsBlurNoiseFactor());
}

/**
 * @tc.name: GetKawaseRandomColorFactor
 * @tc.desc: GetKawaseRandomColorFactor Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetKawaseRandomColorFactor, TestSize.Level1)
{
    ASSERT_TRUE(RSSystemProperties::GetKawaseRandomColorFactor());
}

/**
 * @tc.name: GetRandomColorEnabled
 * @tc.desc: GetRandomColorEnabled Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetRandomColorEnabled, TestSize.Level1)
{
    ASSERT_TRUE(RSSystemProperties::GetRandomColorEnabled());
}

/**
 * @tc.name: GetKawaseOriginalEnabled
 * @tc.desc: GetKawaseOriginalEnabled Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetKawaseOriginalEnabled, TestSize.Level1)
{
    ASSERT_FALSE(RSSystemProperties::GetKawaseOriginalEnabled());
}

/**
 * @tc.name: GetRenderParallelEnabled
 * @tc.desc: GetRenderParallelEnabled Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetRenderParallelEnabled, TestSize.Level1)
{
    ASSERT_TRUE(RSSystemProperties::GetRenderParallelEnabled());
}

/**
 * @tc.name: GetBlurEnabled
 * @tc.desc: GetBlurEnabled Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetBlurEnabled, TestSize.Level1)
{
    ASSERT_TRUE(RSSystemProperties::GetBlurEnabled());
}

/**
 * @tc.name: GetAiInvertCoef
 * @tc.desc: GetAiInvertCoef Test
 * @tc.type:FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSSystemPropertiesTest, GetAiInvertCoef, TestSize.Level1)
{
    ASSERT_NE(RSSystemProperties::GetAiInvertCoef().front(), 0);
}

/**
 * @tc.name: GetProxyNodeDebugEnabled
 * @tc.desc: GetProxyNodeDebugEnabled Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetProxyNodeDebugEnabled, TestSize.Level1)
{
    ASSERT_FALSE(RSSystemProperties::GetProxyNodeDebugEnabled());
}

/**
 * @tc.name: GetUIFirstEnabled
 * @tc.desc: GetUIFirstEnabled Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetUIFirstEnabled, TestSize.Level1)
{
    ASSERT_TRUE(RSSystemProperties::GetUIFirstEnabled());
}

/**
 * @tc.name: GetWideColorSpaceEnabled
 * @tc.desc: GetWideColorSpaceEnabled Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetWideColorSpaceEnabled, TestSize.Level1)
{
    ASSERT_TRUE(RSSystemProperties::GetWideColorSpaceEnabled());
}

/**
 * @tc.name: GetUIFirstDebugEnabled
 * @tc.desc: GetUIFirstDebugEnabled Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetUIFirstDebugEnabled, TestSize.Level1)
{
    ASSERT_FALSE(RSSystemProperties::GetUIFirstDebugEnabled());
}

/**
 * @tc.name: GetUIFirstBehindWindowFilterEnabled
 * @tc.desc: GetUIFirstBehindWindowFilterEnabled Test
 * @tc.type:FUNC
 * @tc.require: issuesIC0HM8
 */
HWTEST_F(RSSystemPropertiesTest, GetUIFirstBehindWindowFilterEnabled, TestSize.Level1)
{
    ASSERT_TRUE(RSSystemProperties::GetUIFirstBehindWindowFilterEnabled());
}

/**
 * @tc.name: GetDumpImgEnabled
 * @tc.desc: GetDumpImgEnabled Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetDumpImgEnabled, TestSize.Level1)
{
    ASSERT_EQ(RSSystemProperties::GetDumpImgEnabled(), 0);
}

/**
 * @tc.name: FindNodeInTargetListSuccess
 * @tc.desc: FindNodeInTargetListSuccess Test
 * @tc.type:FUNC
 * @tc.require: issueI9V3Y2
 */
HWTEST_F(RSSystemPropertiesTest, FindNodeInTargetListSuccess, TestSize.Level1)
{
    std::string targetStr("A;B;C;D");
    system::SetParameter("persist.sys.graphic.traceTargetList", targetStr);
    std::string nodeStr("A");
    EXPECT_TRUE(RSSystemProperties::FindNodeInTargetList(nodeStr));
    system::SetParameter("persist.sys.graphic.traceTargetList", "");
}

/**
 * @tc.name: IsFoldScreenFlag
 * @tc.desc: IsFoldScreenFlag Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, IsFoldScreenFlag, TestSize.Level1)
{
    std::string foldScreenFlag = system::GetParameter("const.window.foldscreen.type", "");
    system::SetParameter("const.window.foldscreen.type", "0");
    EXPECT_TRUE(RSSystemProperties::IsFoldScreenFlag());
    system::SetParameter("const.window.foldscreen.type", foldScreenFlag);
}

/**
 * @tc.name: GetCacheCmdEnabled
 * @tc.desc: GetCacheCmdEnabled Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetCacheCmdEnabled, TestSize.Level1)
{
    RSSystemProperties::GetCacheCmdEnabled();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: GetASTCEnabled
 * @tc.desc: GetASTCEnabled Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetASTCEnabled, TestSize.Level1)
{
    ASSERT_FALSE(RSSystemProperties::GetASTCEnabled());
}

/**
 * @tc.name: GetCachedBlurPartialRenderEnabled
 * @tc.desc: GetCachedBlurPartialRenderEnabled Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetCachedBlurPartialRenderEnabled, TestSize.Level1)
{
    ASSERT_FALSE(RSSystemProperties::GetCachedBlurPartialRenderEnabled());
}

/**
 * @tc.name: GetParallelUploadTexture
 * @tc.desc: GetParallelUploadTexture Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetParallelUploadTexture, TestSize.Level1)
{
    ASSERT_TRUE(RSSystemProperties::GetParallelUploadTexture());
}

/**
 * @tc.name: GetImageGpuResourceCacheEnable
 * @tc.desc: GetImageGpuResourceCacheEnable Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetImageGpuResourceCacheEnable, TestSize.Level1)
{
    int width = 1;
    int height = 1;
    ASSERT_FALSE(RSSystemProperties::GetImageGpuResourceCacheEnable(width, height));

    width = 1350;
    height = 2810;
    ASSERT_TRUE(RSSystemProperties::GetImageGpuResourceCacheEnable(width, height));
}

/**
 * @tc.name: GetBoolSystemProperty
 * @tc.desc: GetBoolSystemProperty Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetBoolSystemProperty, TestSize.Level1)
{
    EXPECT_FALSE(RSSystemProperties::GetBoolSystemProperty(std::string("noName").c_str(), false));
}

/**
 * @tc.name: WatchSystemProperty
 * @tc.desc: WatchSystemProperty Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, WatchSystemProperty, TestSize.Level1)
{
    OnSystemPropertyChanged func = [](const char*, const char*, void*) {};
    int context = 1;
    ASSERT_EQ(RSSystemProperties::WatchSystemProperty(std::string("noName").c_str(), func, &context), 0);
}

/**
 * @tc.name: IsPhoneType
 * @tc.desc: IsPhoneType Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, IsPhoneType, TestSize.Level1)
{
    ASSERT_FALSE(RSSystemProperties::IsPhoneType());
}

/**
 * @tc.name: GetSyncTransactionEnabled
 * @tc.desc: GetSyncTransactionEnabled Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetSyncTransactionEnabled, TestSize.Level1)
{
    ASSERT_TRUE(RSSystemProperties::GetSyncTransactionEnabled());
}

/**
 * @tc.name: GetSyncTransactionWaitDelay
 * @tc.desc: GetSyncTransactionWaitDelay Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetSyncTransactionWaitDelay, TestSize.Level1)
{
    ASSERT_EQ(RSSystemProperties::GetSyncTransactionWaitDelay(), 1500);
}

/**
 * @tc.name: GetSingleFrameComposerEnabled
 * @tc.desc: GetSingleFrameComposerEnabled Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetSingleFrameComposerEnabled, TestSize.Level1)
{
    ASSERT_FALSE(RSSystemProperties::GetSingleFrameComposerEnabled());
}

/**
 * @tc.name: GetSingleFrameComposerCanvasNodeEnabled
 * @tc.desc: GetSingleFrameComposerCanvasNodeEnabled Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetSingleFrameComposerCanvasNodeEnabled, TestSize.Level1)
{
    ASSERT_FALSE(RSSystemProperties::GetSingleFrameComposerCanvasNodeEnabled());
}

/**
 * @tc.name: GetSubTreePrepareCheckType
 * @tc.desc: GetSubTreePrepareCheckType Test
 * @tc.type:FUNC
 * @tc.require: issueI9JZWC
 */
HWTEST_F(RSSystemPropertiesTest, GetSubTreePrepareCheckType, TestSize.Level1)
{
    EXPECT_FALSE(RSSystemProperties::GetSecurityPermissionCheckEnabled());
    EXPECT_TRUE(RSSystemProperties::GetEffectMergeEnabled());
    EXPECT_FALSE(RSSystemProperties::GetDumpUICaptureEnabled());
    EXPECT_FALSE(RSSystemProperties::GetDumpUIPixelmapEnabled());
    EXPECT_EQ(RSSystemProperties::GetVirtualScreenScaleModeDFX(), 2);
    ASSERT_EQ(RSSystemProperties::GetSubTreePrepareCheckType(), SubTreePrepareCheckType::ENABLED);
    EXPECT_TRUE(RSSystemProperties::GetBlurEffectTerminateLimit() > 0);
}


/**
 * @tc.name: GetOptimizeParentNodeRegionEnabled
 * @tc.desc: GetOptimizeParentNodeRegionEnabled Test
 * @tc.type:FUNC
 * @tc.require: issuesIBIA3V
 */
HWTEST_F(RSSystemPropertiesTest, GetOptimizeParentNodeRegionEnabled, TestSize.Level1)
{
    ASSERT_TRUE(RSSystemProperties::GetOptimizeParentNodeRegionEnabled());
}

/**
 * @tc.name: GetOptimizeHwcComposeAreaEnabled
 * @tc.desc: GetOptimizeHwcComposeAreaEnabled Test
 * @tc.type:FUNC
 * @tc.require: issuesIBIA3V
 */
HWTEST_F(RSSystemPropertiesTest, GetOptimizeHwcComposeAreaEnabled, TestSize.Level1)
{
    ASSERT_TRUE(RSSystemProperties::GetOptimizeHwcComposeAreaEnabled());
}

/**
 * @tc.name: GetTimeVsyncDisabled
 * @tc.desc: GetTimeVsyncDisabled Test
 * @tc.type:FUNC
 * @tc.require: issuesIBIA3V
 */
HWTEST_F(RSSystemPropertiesTest, GetTimeVsyncDisabled001, TestSize.Level1)
{
    ASSERT_FALSE(RSSystemProperties::GetTimeVsyncDisabled());
}

/**
 * @tc.name: BehindWindowFilterEnabledTest
 * @tc.desc: BehindWindowFilterEnabledTest
 * @tc.type:FUNC
 * @tc.require: issuesIC5OEB
 */
HWTEST_F(RSSystemPropertiesTest, BehindWindowFilterEnabledTest, TestSize.Level1)
{
    bool enabled = RSSystemProperties::GetBehindWindowFilterEnabled();
    RSSystemProperties::SetBehindWindowFilterEnabled(!enabled);
    EXPECT_EQ(RSSystemProperties::GetBehindWindowFilterEnabled(), !enabled);
    RSSystemProperties::SetBehindWindowFilterEnabled(enabled);
}

/**
 * @tc.name: SetTypicalResidentProcessTest
 * @tc.desc: set isTypicalResidentProcess_ to true
 * @tc.type:FUNC
 * @tc.require: issuesIC5OEB
 */
HWTEST_F(RSSystemPropertiesTest, SetTypicalResidentProcessTest, TestSize.Level1)
{
    bool enabled = RSSystemProperties::GetTypicalResidentProcess();
    RSSystemProperties::SetTypicalResidentProcess(!enabled);
    EXPECT_EQ(RSSystemProperties::GetTypicalResidentProcess(), !enabled);
    RSSystemProperties::SetTypicalResidentProcess(enabled);
}

/**
 * @tc.name: SetTypicalResidentProcessTest001
 * @tc.desc: set isTypicalResidentProcess_ to true and GetHybridRenderEnabled
 * @tc.type:FUNC
 * @tc.require: issuesIC5OEB
 */
HWTEST_F(RSSystemPropertiesTest, SetTypicalResidentProcessTest001, TestSize.Level1)
{
    RSSystemProperties::SetTypicalResidentProcess(true);
    bool enbaled = RSSystemProperties::GetHybridRenderEnabled();
    EXPECT_EQ(enbaled, false);
}
} // namespace Rosen
} // namespace OHOS