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
#include "params/rs_display_render_params.h"
#include "limit_number.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSDisplayRenderParamsOneTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static void DisplayTestInfo();
};

void RSDisplayRenderParamsOneTest::SetUpTestCase() {}
void RSDisplayRenderParamsOneTest::TearDownTestCase() {}
void RSDisplayRenderParamsOneTest::SetUp() {}
void RSDisplayRenderParamsOneTest::TearDown() {}
void RSDisplayRenderParamsOneTest::DisplayTestInfo()
{
    return;
}

/**
 * @tc.name: OnSyncTest001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require: issuesIB7RKW
 */
HWTEST_F(RSDisplayRenderParamsOneTest, OnSyncTest001, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[2];
    std::unique_ptr<RSRenderParams> target = nullptr;
    RSDisplayRenderParams params(id);
    params.OnSync(target);
    EXPECT_FALSE(params.isMainAndLeashSurfaceDirty_);
}

/**
 * @tc.name: SetRotationChangedTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require: issuesIB7RKW
 */
HWTEST_F(RSDisplayRenderParamsOneTest, SetRotationChangedTest, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[4];
    RSDisplayRenderParams params(id);
    params.SetRotationChanged(params.IsRotationChanged());
    EXPECT_FALSE(params.needSync_);

    params.SetRotationChanged(true);
}

/**
 * @tc.name: SetMainAndLeashSurfaceDirtyTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require: issuesIB7RKW
 */
HWTEST_F(RSDisplayRenderParamsOneTest, SetMainAndLeashSurfaceDirtyTest, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[3];
    RSDisplayRenderParams params(id);
    params.SetMainAndLeashSurfaceDirty(params.GetMainAndLeashSurfaceDirty());
    EXPECT_FALSE(params.needSync_);

    params.SetMainAndLeashSurfaceDirty(true);
}

/**
 * @tc.name: SetNewColorSpaceTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require: issuesIB7RKW
 */
HWTEST_F(RSDisplayRenderParamsOneTest, SetNewColorSpaceTest, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[6];
    RSDisplayRenderParams params(id);
    params.SetNewColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    EXPECT_FALSE(params.needSync_);

    params.SetNewColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_ADOBE_RGB);
}

/**
 * @tc.name: SetHDRPresentTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require: issuesIB7RKW
 */
HWTEST_F(RSDisplayRenderParamsOneTest, SetHDRPresentTest, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[5];
    RSDisplayRenderParams params(id);
    params.SetHDRPresent(params.GetHDRPresent());
    EXPECT_FALSE(params.needSync_);

    params.SetHDRPresent(true);
}

/**
 * @tc.name: HasSecurityLayerTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require: issuesIB7RKW
 */
HWTEST_F(RSDisplayRenderParamsOneTest, HasSecurityLayerTest, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[0];
    RSDisplayRenderParams params(id);
    auto displayHasSecSurface = params.GetDisplayHasSecSurface();
    EXPECT_FALSE(params.HasSecurityLayer());

    params.displayHasSecSurface_[params.screenId_] = true;
    EXPECT_TRUE(params.HasSecurityLayer());
}

/**
 * @tc.name: SetNewPixelFormatTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require: issuesIB7RKW
 */
HWTEST_F(RSDisplayRenderParamsOneTest, SetNewPixelFormatTest, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[0];
    RSDisplayRenderParams params(id);
    params.SetNewPixelFormat(params.GetNewPixelFormat());
    EXPECT_FALSE(params.needSync_);

    params.SetNewPixelFormat(GraphicPixelFormat::GRAPHIC_PIXEL_FMT_BUTT);
}

/**
 * @tc.name: HasSkipLayerTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require: issuesIB7RKW
 */
HWTEST_F(RSDisplayRenderParamsOneTest, HasSkipLayerTest, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[0];
    RSDisplayRenderParams params(id);
    EXPECT_FALSE(params.HasSkipLayer());

    params.displayHasSkipSurface_[params.screenId_] = true;
    EXPECT_TRUE(params.HasSkipLayer());
}

/**
 * @tc.name: HasSnapshotSkipLayerTest001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require: issuesIB7RKW
 */
HWTEST_F(RSDisplayRenderParamsOneTest, HasSnapshotSkipLayerTest001, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[0];
    RSDisplayRenderParams params(id);
    EXPECT_FALSE(params.HasSnapshotSkipLayer());

    params.displayHasSnapshotSkipSurface_[params.screenId_] = true;
    EXPECT_TRUE(params.HasSnapshotSkipLayer());
}

/**
 * @tc.name: HasCaptureWindowTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require: issuesIB7RKW
 */
HWTEST_F(RSDisplayRenderParamsOneTest, HasCaptureWindowTest, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[3];
    RSDisplayRenderParams params(id);
    EXPECT_FALSE(params.HasCaptureWindow());

    params.hasCaptureWindow_[params.screenId_] = true;
    EXPECT_TRUE(params.HasCaptureWindow());
}

/**
 * @tc.name: HasProtectedLayerTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require: issuesIB7RKW
 */
HWTEST_F(RSDisplayRenderParamsOneTest, HasProtectedLayerTest, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[2];
    RSDisplayRenderParams params(id);
    EXPECT_FALSE(params.HasProtectedLayer());

    params.displayHasProtectedSurface_[params.screenId_] = true;
    EXPECT_TRUE(params.HasProtectedLayer());
}

/**
 * @tc.name: SetNeedOffscreenTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require: issuesIB7RKW
 */
HWTEST_F(RSDisplayRenderParamsOneTest, SetNeedOffscreenTest, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[3];
    RSDisplayRenderParams params(id);
    auto needOffscreen = params.GetNeedOffscreen();
    params.SetNeedOffscreen(needOffscreen);
    EXPECT_FALSE(params.needSync_);

    params.SetNeedOffscreen(true);
}

/**
 * @tc.name: FingerprintTest001
 * @tc.desc: test SetFingerprint and GetFingerprint
 * @tc.type: FUNC
 * @tc.require: issuesIB7RKW
 */
HWTEST_F(RSDisplayRenderParamsOneTest, FingerprintTest001, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[3];
    RSDisplayRenderParams params(id);
    EXPECT_FALSE(params.GetFingerprint());

    params.SetFingerprint(true);
    EXPECT_TRUE(params.GetFingerprint());
}

/**
 * @tc.name: IsSpecialLayerChangedTest001
 * @tc.desc: test result of IsSpecialLayerChanged
 * @tc.type: FUNC
 * @tc.require: issuesIB7RKW
 */
HWTEST_F(RSDisplayRenderParamsOneTest, IsSpecialLayerChangedTest001, TestSize.Level1)
{
    constexpr NodeId id = 0;
    RSDisplayRenderParams params(id);
    EXPECT_FALSE(params.IsSpecialLayerChanged());
}

/**
 * @tc.name: GetSecurityExemptionTest001
 * @tc.desc: test result of GetSecurityExemption
 * @tc.type: FUNC
 * @tc.require: issuesIB7RKW
 */
HWTEST_F(RSDisplayRenderParamsOneTest, GetSecurityExemptionTest001, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[0];
    RSDisplayRenderParams params(id);
    EXPECT_FALSE(params.GetSecurityExemption());
}
} // namespace OHOS::Rosen