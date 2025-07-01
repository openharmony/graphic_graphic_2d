/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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
#include "common/rs_occlusion_region.h"
#include "params/rs_logical_display_render_params.h"
#include "limit_number.h"
#include "surface_type.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSLogicalDisplayRenderParamsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static void DisplayTestInfo();
};

void RSLogicalDisplayRenderParamsTest::SetUpTestCase() {}
void RSLogicalDisplayRenderParamsTest::TearDownTestCase() {}
void RSLogicalDisplayRenderParamsTest::SetUp() {}
void RSLogicalDisplayRenderParamsTest::TearDown() {}
void RSLogicalDisplayRenderParamsTest::DisplayTestInfo()
{
    return;
}

/**
 * @tc.name: OnSync001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderParamsTest, OnSync001, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[2];
    std::unique_ptr<RSRenderParams> target = nullptr;
    RSLogicalDisplayRenderParams params(id);
    params.OnSync(target);
    target = std::make_unique<RSRenderParams>(id);
    ASSERT_NE(target, nullptr);
    params.OnSync(target);
}

/**
 * @tc.name: SetMainAndLeashSurfaceDirty
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderParamsTest, SetMainAndLeashSurfaceDirty, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[3];
    RSLogicalDisplayRenderParams params(id);
    EXPECT_EQ(params.needSync_, false);
}

/**
 * @tc.name: SetRotationChanged
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderParamsTest, SetRotationChanged, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[4];
    RSLogicalDisplayRenderParams params(id);
    params.SetRotationChanged(params.IsRotationChanged());
    EXPECT_EQ(params.needSync_, false);

    params.SetRotationChanged(true);
    EXPECT_EQ(params.needSync_, true);
}

/**
 * @tc.name: SetHDRPresent
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderParamsTest, SetHDRPresent, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[5];
    RSScreenRenderParams params(id);
    params.SetHDRPresent(params.GetHDRPresent());
    EXPECT_EQ(params.needSync_, false);

    params.SetHDRPresent(true);
    EXPECT_EQ(params.needSync_, true);
}

/**
 * @tc.name: SetHDRStatusChanged
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderParamsTest, SetHDRStatusChanged, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[0];
    RSScreenRenderParams params(id);
    params.SetHDRStatusChanged(params.IsHDRStatusChanged());
    EXPECT_EQ(params.needSync_, false);

    params.SetHDRStatusChanged(true);
    EXPECT_EQ(params.needSync_, true);
}

/**
 * @tc.name: SetNewColorSpace
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderParamsTest, SetNewColorSpace, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[6];
    RSScreenRenderParams params(id);
    params.SetNewColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    EXPECT_EQ(params.needSync_, false);

    params.SetNewColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_ADOBE_RGB);
    EXPECT_EQ(params.needSync_, true);
}

/**
 * @tc.name: SetNewPixelFormat
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderParamsTest, SetNewPixelFormat, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[0];
    RSScreenRenderParams params(id);
    params.SetNewPixelFormat(params.GetNewPixelFormat());
    EXPECT_EQ(params.needSync_, false);

    params.SetNewPixelFormat(GraphicPixelFormat::GRAPHIC_PIXEL_FMT_BUTT);
    EXPECT_EQ(params.needSync_, true);
}

/**
 * @tc.name: HasCaptureWindow
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderParamsTest, HasCaptureWindow, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[3];
    RSLogicalDisplayRenderParams params(id);
    EXPECT_FALSE(params.HasCaptureWindow());

    params.hasCaptureWindow_ = true;
    EXPECT_TRUE(params.HasCaptureWindow());
}

/**
 * @tc.name: Fingerprint001
 * @tc.desc: test SetFingerprint and GetFingerprint
 * @tc.type: FUNC
 * @tc.require: issuesIB2QEK
 */
HWTEST_F(RSLogicalDisplayRenderParamsTest, Fingerprint001, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[3];
    RSLogicalDisplayRenderParams params(id);
    EXPECT_FALSE(params.GetFingerprint());

    params.SetFingerprint(true);
    EXPECT_TRUE(params.GetFingerprint());
}

/**
 * @tc.name: SetNeedOffscreen
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderParamsTest, SetNeedOffscreen, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[3];
    RSLogicalDisplayRenderParams params(id);
    auto needOffscreen = params.GetNeedOffscreen();
    params.SetNeedOffscreen(needOffscreen);
    EXPECT_EQ(params.needSync_, false);

    params.SetNeedOffscreen(true);
    EXPECT_EQ(params.needSync_, true);
}

/**
 * @tc.name: IsSpecialLayerChanged001
 * @tc.desc: test result of IsSpecialLayerChanged
 * @tc.type: FUNC
 * @tc.require: issuesIACYVJ
 */
HWTEST_F(RSLogicalDisplayRenderParamsTest, IsSpecialLayerChanged001, TestSize.Level1)
{
    constexpr NodeId id = 0;
    RSLogicalDisplayRenderParams params(id);
    EXPECT_FALSE(params.IsSpecialLayerChanged());
}

/**
 * @tc.name: GetSecurityExemption001
 * @tc.desc: test result of GetSecurityExemption
 * @tc.type: FUNC
 * @tc.require: issuesIAKMJP
 */
HWTEST_F(RSLogicalDisplayRenderParamsTest, GetSecurityExemption001, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[0];
    RSLogicalDisplayRenderParams params(id);
    EXPECT_FALSE(params.GetSecurityExemption());
}

/**
 * @tc.name: HasSecLayerInVisibleRect001
 * @tc.desc: test result of HasSecLayerInVisibleRect
 * @tc.type: FUNC
 * @tc.require: issueIB2KBH
 */
HWTEST_F(RSLogicalDisplayRenderParamsTest, HasSecLayerInVisibleRect001, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[0];
    RSLogicalDisplayRenderParams params(id);
    EXPECT_EQ(params.HasSecLayerInVisibleRect(), false);
}

/**
 * @tc.name: HasSecLayerInVisibleRect002
 * @tc.desc: test result of HasSecLayerInVisibleRect
 * @tc.type: FUNC
 * @tc.require: issueIB2KBH
 */
HWTEST_F(RSLogicalDisplayRenderParamsTest, HasSecLayerInVisibleRect002, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[0];
    RSLogicalDisplayRenderParams params(id);
    params.hasSecLayerInVisibleRect_ = true;
    EXPECT_EQ(params.HasSecLayerInVisibleRect(), true);
}

/**
 * @tc.name: HasSecLayerInVisibleRectChanged001
 * @tc.desc: test result of HasSecLayerInVisibleRectChanged
 * @tc.type: FUNC
 * @tc.require: issueIB2KBH
 */
HWTEST_F(RSLogicalDisplayRenderParamsTest, HasSecLayerInVisibleRectChanged001, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[0];
    RSLogicalDisplayRenderParams params(id);
    EXPECT_EQ(params.HasSecLayerInVisibleRectChanged(), false);
}

/**
 * @tc.name: HasSecLayerInVisibleRectChanged002
 * @tc.desc: test result of HasSecLayerInVisibleRectChanged
 * @tc.type: FUNC
 * @tc.require: issueIB2KBH
 */
HWTEST_F(RSLogicalDisplayRenderParamsTest, HasSecLayerInVisibleRectChanged002, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[0];
    RSLogicalDisplayRenderParams params(id);
    params.hasSecLayerInVisibleRectChanged_ = true;
    EXPECT_EQ(params.HasSecLayerInVisibleRectChanged(), true);
}

/**
 * @tc.name: ResetVirtualExpandAccumulatedParams
 * @tc.desc: test ResetVirtualExpandAccumulatedParams can set target params to false
 * @tc.type: FUNC
 * @tc.require: issueICCV9N
 */
HWTEST_F(RSLogicalDisplayRenderParamsTest, ResetVirtualExpandAccumulatedParams, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[0];
    RSScreenRenderParams params(id);
    params.SetAccumulatedDirty(true);
    ASSERT_TRUE(params.GetAccumulatedDirty());
    params.SetAccumulatedHdrStatusChanged(true);
    ASSERT_TRUE(params.GetAccumulatedHdrStatusChanged());
    params.ResetVirtualExpandAccumulatedParams();
    ASSERT_FALSE(params.GetAccumulatedDirty());
    ASSERT_FALSE(params.GetAccumulatedHdrStatusChanged());
}

/**
 * @tc.name: GetTargetSurfaceRenderNodeDrawable
 * @tc.desc: test result of Set/GetTargetSurfaceRenderNodeDrawable with invalid params
 * @tc.type: FUNC
 * @tc.require: issueIB2KBH
 */
HWTEST_F(RSLogicalDisplayRenderParamsTest, GetTargetSurfaceRenderNodeDrawable, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[0];
    RSScreenRenderParams params(id);
    params.SetTargetSurfaceRenderNodeDrawable(std::weak_ptr<DrawableV2::RSRenderNodeDrawableAdapter>());
    EXPECT_TRUE(params.GetTargetSurfaceRenderNodeDrawable().expired());
}

/**
 * @tc.name: GetVirtualScreenMuteStatus
 * @tc.desc: test result of GetVirtualScreenMuteStatus
 * @tc.type: FUNC
 * @tc.require: issueIBTNC3
 */
HWTEST_F(RSLogicalDisplayRenderParamsTest, GetVirtualScreenMuteStatus, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[0];
    RSLogicalDisplayRenderParams params(id);
    params.virtualScreenMuteStatus_ = true;
    ASSERT_TRUE(params.GetVirtualScreenMuteStatus());
}

/**
 * @tc.name: SetNeedForceUpdateHwcNodes
 * @tc.desc: test result of SetNeedForceUpdateHwcNodes
 * @tc.type: FUNC
 * @tc.require: issueIC0AQO
 */
HWTEST_F(RSLogicalDisplayRenderParamsTest, SetNeedForceUpdateHwcNodes, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[0];
    RSScreenRenderParams params(id);
    params.needForceUpdateHwcNodes_ = false;
    params.SetNeedForceUpdateHwcNodes(true);
    ASSERT_TRUE(params.needForceUpdateHwcNodes_);
    params.SetNeedForceUpdateHwcNodes(false);
    ASSERT_FALSE(params.needForceUpdateHwcNodes_);
}
} // namespace OHOS::Rosen