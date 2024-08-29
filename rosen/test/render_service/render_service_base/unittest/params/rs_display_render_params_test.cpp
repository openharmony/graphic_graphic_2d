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
#include "params/rs_display_render_params.h"
#include "limit_number.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSDisplayRenderParamsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static void DisplayTestInfo();
};

void RSDisplayRenderParamsTest::SetUpTestCase() {}
void RSDisplayRenderParamsTest::TearDownTestCase() {}
void RSDisplayRenderParamsTest::SetUp() {}
void RSDisplayRenderParamsTest::TearDown() {}
void RSDisplayRenderParamsTest::DisplayTestInfo()
{
    return;
}

/**
 * @tc.name: OnSync001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayRenderParamsTest, OnSync001, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[2];
    std::unique_ptr<RSRenderParams> target = nullptr;
    RSDisplayRenderParams params(id);
    params.OnSync(target);
    EXPECT_FALSE(params.isMainAndLeashSurfaceDirty_);
}

/**
 * @tc.name: SetMainAndLeashSurfaceDirty001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayRenderParamsTest, SetMainAndLeashSurfaceDirty001, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[3];
    RSDisplayRenderParams params(id);
    auto res = params.GetMainAndLeashSurfaceDirty();
    params.SetMainAndLeashSurfaceDirty(res);
}

/**
 * @tc.name: SetRotationChanged001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayRenderParamsTest, SetRotationChanged001, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[4];
    RSDisplayRenderParams params(id);
    auto res = params.IsRotationChanged();
    params.SetRotationChanged(res);
}

/**
 * @tc.name: SetHDRPresent001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayRenderParamsTest, SetHDRPresent001, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[5];
    RSDisplayRenderParams params(id);
    auto res = params.GetHDRPresent();
    params.SetHDRPresent(res);
}

/**
 * @tc.name: SetNewColorSpace001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayRenderParamsTest, SetNewColorSpace001, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[6];
    RSDisplayRenderParams params(id);
    GraphicColorGamut newColorSpace = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
    params.SetNewColorSpace(newColorSpace);
}

/**
 * @tc.name: SetNewPixelFormat001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayRenderParamsTest, SetNewPixelFormat001, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[0];
    RSDisplayRenderParams params(id);
    auto res = params.GetNewPixelFormat();
    params.SetNewPixelFormat(res);
}

/**
 * @tc.name: HasSecurityLayer001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayRenderParamsTest, HasSecurityLayer001, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[0];
    RSDisplayRenderParams params(id);
    auto displayHasSecSurface = params.GetDisplayHasSecSurface();
    EXPECT_FALSE(params.HasSecurityLayer());
}

/**
 * @tc.name: HasSkipLayer001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayRenderParamsTest, HasSkipLayer001, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[0];
    RSDisplayRenderParams params(id);
    params.displayHasSkipSurface_[1] = false;
    EXPECT_FALSE(params.HasSkipLayer());
}

/**
 * @tc.name: HasProtectedLayer001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayRenderParamsTest, HasProtectedLayer001, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[2];
    RSDisplayRenderParams params(id);
    params.displayHasProtectedSurface_[1] = false;
    EXPECT_FALSE(params.HasProtectedLayer());
}


/**
 * @tc.name: HasCaptureWindow001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayRenderParamsTest, HasCaptureWindow001, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[3];
    RSDisplayRenderParams params(id);
    params.hasCaptureWindow_[1] = false;
    EXPECT_FALSE(params.HasCaptureWindow());
}

/**
 * @tc.name: HasCaptureWindow001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayRenderParamsTest, SetNeedOffscreen001, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[3];
    RSDisplayRenderParams params(id);
    auto needOffscreen = params.GetNeedOffscreen();
    params.SetNeedOffscreen(needOffscreen);
}

/**
 * @tc.name: IsSpecialLayerChanged001
 * @tc.desc: test result of IsSpecialLayerChanged
 * @tc.type: FUNC
 * @tc.require: issuesIACYVJ
 */
HWTEST_F(RSDisplayRenderParamsTest, IsSpecialLayerChanged001, TestSize.Level1)
{
    constexpr NodeId id = 0;
    RSDisplayRenderParams params(id);
    EXPECT_FALSE(params.IsSpecialLayerChanged());
}

/**
 * @tc.name: GetSecurityExemption001
 * @tc.desc: test result of GetSecurityExemption
 * @tc.type: FUNC
 * @tc.require: issuesIAKMJP
 */
HWTEST_F(RSDisplayRenderParamsTest, GetSecurityExemption001, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[0];
    RSDisplayRenderParams params(id);
    EXPECT_FALSE(params.GetSecurityExemption());
}
} // namespace OHOS::Rosen