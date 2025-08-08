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
#include "params/rs_screen_render_params.h"
#include "limit_number.h"
#include "surface_type.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSScreenRenderParamsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static void DisplayTestInfo();
};

void RSScreenRenderParamsTest::SetUpTestCase() {}
void RSScreenRenderParamsTest::TearDownTestCase() {}
void RSScreenRenderParamsTest::SetUp() {}
void RSScreenRenderParamsTest::TearDown() {}
void RSScreenRenderParamsTest::DisplayTestInfo()
{
    return;
}

/**
 * @tc.name: OnSync001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenRenderParamsTest, OnSync001, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[2];
    std::unique_ptr<RSRenderParams> target = nullptr;
    RSScreenRenderParams params(id);
    params.OnSync(target);
    target = std::make_unique<RSRenderParams>(id);
    ASSERT_NE(target, nullptr);
    params.OnSync(target);
}

/**
 * @tc.name: FixVirtualBuffer10Bit
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenRenderParamsTest, FixVirtualBuffer10BitTest, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[0];
    RSScreenRenderParams params(id);
    params.SetFixVirtualBuffer10Bit(params.GetFixVirtualBuffer10Bit());
    EXPECT_EQ(params.needSync_, false);

    params.SetFixVirtualBuffer10Bit(true);
    EXPECT_EQ(params.needSync_, true);
}

/**
 * @tc.name: ExistHWCNode
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenRenderParamsTest, ExistHWCNodeTest, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[0];
    RSScreenRenderParams params(id);
    params.SetExistHWCNode(params.GetExistHWCNode());
    EXPECT_EQ(params.needSync_, false);

    params.SetExistHWCNode(true);
    EXPECT_EQ(params.needSync_, true);
}

/**
 * @tc.name: SetHDRPresent
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenRenderParamsTest, SetHDRPresent, TestSize.Level1)
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
HWTEST_F(RSScreenRenderParamsTest, SetHDRStatusChanged, TestSize.Level1)
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
HWTEST_F(RSScreenRenderParamsTest, SetNewColorSpace, TestSize.Level1)
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
HWTEST_F(RSScreenRenderParamsTest, SetNewPixelFormat, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[0];
    RSScreenRenderParams params(id);
    params.SetNewPixelFormat(params.GetNewPixelFormat());
    EXPECT_EQ(params.needSync_, false);

    params.SetNewPixelFormat(GraphicPixelFormat::GRAPHIC_PIXEL_FMT_BUTT);
    EXPECT_EQ(params.needSync_, true);
}

/**
 * @tc.name: Fingerprint001
 * @tc.desc: test SetFingerprint and GetFingerprint
 * @tc.type: FUNC
 * @tc.require: issuesIB2QEK
 */
HWTEST_F(RSScreenRenderParamsTest, Fingerprint001, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[3];
    RSScreenRenderParams params(id);
    EXPECT_FALSE(params.GetFingerprint());

    params.SetFingerprint(true);
    EXPECT_TRUE(params.GetFingerprint());
}

/**
 * @tc.name: ResetVirtualExpandAccumulatedParams
 * @tc.desc: test ResetVirtualExpandAccumulatedParams can set target params to false
 * @tc.type: FUNC
 * @tc.require: issueICCV9N
 */
HWTEST_F(RSScreenRenderParamsTest, ResetVirtualExpandAccumulatedParams, TestSize.Level1)
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
HWTEST_F(RSScreenRenderParamsTest, GetTargetSurfaceRenderNodeDrawable, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[0];
    RSScreenRenderParams params(id);
    params.SetTargetSurfaceRenderNodeDrawable(std::weak_ptr<DrawableV2::RSRenderNodeDrawableAdapter>());
    EXPECT_TRUE(params.GetTargetSurfaceRenderNodeDrawable().expired());
}

/**
 * @tc.name: SetNeedForceUpdateHwcNodes
 * @tc.desc: test result of SetNeedForceUpdateHwcNodes
 * @tc.type: FUNC
 * @tc.require: issueIC0AQO
 */
HWTEST_F(RSScreenRenderParamsTest, SetNeedForceUpdateHwcNodes, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[0];
    RSScreenRenderParams params(id);
    params.needForceUpdateHwcNodes_ = false;
    params.SetNeedForceUpdateHwcNodes(true);
    ASSERT_TRUE(params.needForceUpdateHwcNodes_);
    params.SetNeedForceUpdateHwcNodes(false);
    ASSERT_FALSE(params.needForceUpdateHwcNodes_);
}

/**
 * @tc.name: SetForceFreeze
 * @tc.desc: test result of SetForceFreeze
 * @tc.type: FUNC
 * @tc.require: issueICQ74B
 */
HWTEST_F(RSScreenRenderParamsTest, SetForceFreeze, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[0];
    RSScreenRenderParams params(id);
    ASSERT_FALSE(params.GetForceFreeze());

    params.SetForceFreeze(false);
    ASSERT_FALSE(params.GetForceFreeze());

    params.SetForceFreeze(true);
    ASSERT_TRUE(params.GetForceFreeze());
}
} // namespace OHOS::Rosen