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

#include <gtest/gtest.h>

#include "ge_visual_effect_impl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace GraphicsEffectEngine {

using namespace Rosen;

class GEVisualEffectImplTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void GEVisualEffectImplTest::SetUpTestCase(void) {}
void GEVisualEffectImplTest::TearDownTestCase(void) {}

void GEVisualEffectImplTest::SetUp() {}

void GEVisualEffectImplTest::TearDown() {}

/**
 * @tc.name: GetFilterType001
 * @tc.desc: Verify function GetFilterType
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, GetFilterType001, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl1(Drawing::GE_FILTER_KAWASE_BLUR);
    EXPECT_EQ(geVisualEffectImpl1.GetFilterType(), Drawing::GEVisualEffectImpl::FilterType::KAWASE_BLUR);

    Drawing::GEVisualEffectImpl geVisualEffectImpl2(Drawing::GE_FILTER_GREY);
    EXPECT_EQ(geVisualEffectImpl2.GetFilterType(), Drawing::GEVisualEffectImpl::FilterType::GREY);

    Drawing::GEVisualEffectImpl geVisualEffectImpl3(Drawing::GE_FILTER_AI_BAR);
    EXPECT_EQ(geVisualEffectImpl3.GetFilterType(), Drawing::GEVisualEffectImpl::FilterType::AIBAR);

    Drawing::GEVisualEffectImpl geVisualEffectImpl4(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR);
    EXPECT_EQ(geVisualEffectImpl4.GetFilterType(), Drawing::GEVisualEffectImpl::FilterType::LINEAR_GRADIENT_BLUR);

    Drawing::GEVisualEffectImpl geVisualEffectImplMESA(Drawing::GE_FILTER_MESA_BLUR);
    EXPECT_EQ(geVisualEffectImplMESA.GetFilterType(), Drawing::GEVisualEffectImpl::FilterType::MESA_BLUR);

    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GE_FILTER_WATER_RIPPLE);
    EXPECT_EQ(geVisualEffectImpl.GetFilterType(), Drawing::GEVisualEffectImpl::FilterType::WATER_RIPPLE);
}

/**
 * @tc.name: SetParam001
 * @tc.desc: Verify function SetParam
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetParam001, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl1(Drawing::GE_FILTER_KAWASE_BLUR);

    // 1 set for Kawase blur RADIUS, linear gradient blur DIRECTION
    int32_t paramInt32 { 1 };
    geVisualEffectImpl1.SetParam(Drawing::GE_FILTER_KAWASE_BLUR_RADIUS, paramInt32);
    ASSERT_NE(geVisualEffectImpl1.GetKawaseParams(), nullptr);
    EXPECT_EQ(geVisualEffectImpl1.GetKawaseParams()->radius, paramInt32);

    Drawing::GEVisualEffectImpl geVisualEffectImpl2(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR);
    geVisualEffectImpl2.SetParam(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR_DIRECTION, paramInt32);
    ASSERT_NE(geVisualEffectImpl2.GetLinearGradientBlurParams(), nullptr);
    EXPECT_EQ(geVisualEffectImpl2.GetLinearGradientBlurParams()->direction, paramInt32);

    geVisualEffectImpl2.SetParam(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR_IS_OFF_SCREEN, true);
    EXPECT_EQ(geVisualEffectImpl2.GetLinearGradientBlurParams()->isOffscreenCanvas, true);

    // 1.f is linear gradient blur params：blurRadius,geoWidth,geoHeight,tranX,tranY
    float paramfloat { 1.f };
    geVisualEffectImpl2.SetParam(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR_RADIUS, paramfloat);
    geVisualEffectImpl2.SetParam(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR_GEO_WIDTH, paramfloat);
    geVisualEffectImpl2.SetParam(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR_GEO_HEIGHT, paramfloat);
    geVisualEffectImpl2.SetParam(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR_TRAN_X, paramfloat);
    geVisualEffectImpl2.SetParam(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR_TRAN_Y, paramfloat);
    EXPECT_EQ(geVisualEffectImpl2.GetLinearGradientBlurParams()->blurRadius, paramfloat);
    EXPECT_EQ(geVisualEffectImpl2.GetLinearGradientBlurParams()->geoWidth, paramfloat);
    EXPECT_EQ(geVisualEffectImpl2.GetLinearGradientBlurParams()->geoHeight, paramfloat);
    EXPECT_EQ(geVisualEffectImpl2.GetLinearGradientBlurParams()->tranX, paramfloat);
    EXPECT_EQ(geVisualEffectImpl2.GetLinearGradientBlurParams()->tranY, paramfloat);

    auto image = std::make_shared<Drawing::Image>();
    auto colorFilter = std::make_shared<Drawing::ColorFilter>(Drawing::ColorFilter::FilterType::NO_TYPE);
    Drawing::Matrix mat;
    int64_t paramInt64 { 1 }; // 1 is linear gradient blur params：CANVAS_MAT
    geVisualEffectImpl1.SetParam(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR_CANVAS_MAT, paramInt64);
    geVisualEffectImpl2.SetParam(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR, image);
    geVisualEffectImpl2.SetParam(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR, colorFilter);

    geVisualEffectImpl2.SetParam(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR_CANVAS_MAT, mat);
    EXPECT_EQ(geVisualEffectImpl2.GetLinearGradientBlurParams()->mat, mat);

    // 0.1f, 0.1f is linear gradient blur params: FRACTION_STOPS
    geVisualEffectImpl2.SetParam(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR_FRACTION_STOPS, { { 0.1f, 0.1f } });
    std::vector<std::pair<float, float>> expectFraStops { { 0.1f, 0.1f } };
    EXPECT_EQ(geVisualEffectImpl2.GetLinearGradientBlurParams()->fractionStops, expectFraStops);
}

/**
 * @tc.name: SetParam002
 * @tc.desc: Verify function SetParam
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetParam002, TestSize.Level1)
{
    // 1.0f is params of AI Bar blur, Grey blur
    float paramfloat { 1.0f };
    Drawing::GEVisualEffectImpl geVisualEffectImpl1(Drawing::GE_FILTER_AI_BAR);
    geVisualEffectImpl1.SetParam(Drawing::GE_FILTER_AI_BAR_LOW, paramfloat);
    geVisualEffectImpl1.SetParam(Drawing::GE_FILTER_AI_BAR_HIGH, paramfloat);
    geVisualEffectImpl1.SetParam(Drawing::GE_FILTER_AI_BAR_THRESHOLD, paramfloat);
    geVisualEffectImpl1.SetParam(Drawing::GE_FILTER_AI_BAR_OPACITY, paramfloat);
    geVisualEffectImpl1.SetParam(Drawing::GE_FILTER_AI_BAR_SATURATION, paramfloat);
    ASSERT_NE(geVisualEffectImpl1.GetAIBarParams(), nullptr);
    EXPECT_EQ(geVisualEffectImpl1.GetAIBarParams()->aiBarLow, paramfloat);
    EXPECT_EQ(geVisualEffectImpl1.GetAIBarParams()->aiBarHigh, paramfloat);
    EXPECT_EQ(geVisualEffectImpl1.GetAIBarParams()->aiBarThreshold, paramfloat);
    EXPECT_EQ(geVisualEffectImpl1.GetAIBarParams()->aiBarOpacity, paramfloat);
    EXPECT_EQ(geVisualEffectImpl1.GetAIBarParams()->aiBarSaturation, paramfloat);

    Drawing::GEVisualEffectImpl geVisualEffectImpl2(Drawing::GE_FILTER_GREY);
    geVisualEffectImpl2.SetParam(Drawing::GE_FILTER_GREY_COEF_1, paramfloat);
    geVisualEffectImpl2.SetParam(Drawing::GE_FILTER_GREY_COEF_2, paramfloat);
    ASSERT_NE(geVisualEffectImpl2.GetGreyParams(), nullptr);
    EXPECT_EQ(geVisualEffectImpl2.GetGreyParams()->greyCoef1, paramfloat);
    EXPECT_EQ(geVisualEffectImpl2.GetGreyParams()->greyCoef2, paramfloat);
}

/**
 * @tc.name: SetParam003
 * @tc.desc: Verify function SetParam  no filter type
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetParam003, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl("");
    geVisualEffectImpl.SetParam("", 0); // 0 invalid params
    geVisualEffectImpl.SetParam("", false);
    geVisualEffectImpl.SetParam("", 1.0f); // 1.0f invalid params
    geVisualEffectImpl.SetParam("", 1.0f); // 1.0f invalid params
    Drawing::Matrix blurMat;
    geVisualEffectImpl.SetParam("", blurMat);
    std::vector<std::pair<float, float>> blurFractionStops;
    geVisualEffectImpl.SetParam("", blurFractionStops);
    geVisualEffectImpl.SetAIBarParams("", 1.0f);              // 1.0f invalid params
    geVisualEffectImpl.SetGreyParams("", 1.0f);               // 1.0f invalid params
    geVisualEffectImpl.SetLinearGradientBlurParams("", 1.0f); // 1.0f invalid params
    EXPECT_EQ(geVisualEffectImpl.GetFilterType(), Drawing::GEVisualEffectImpl::FilterType::NONE);
}

/**
 * @tc.name: SetParam004
 * @tc.desc: Verify function SetParam for param is nullptr when filtertype is NONE
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetParam004, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl("");
    geVisualEffectImpl.SetFilterType(Drawing::GEVisualEffectImpl::FilterType::KAWASE_BLUR);
    geVisualEffectImpl.SetParam("", 0); // 0 invalid params

    geVisualEffectImpl.SetFilterType(Drawing::GEVisualEffectImpl::FilterType::LINEAR_GRADIENT_BLUR);
    geVisualEffectImpl.SetParam("", 0); // 0 invalid params
    geVisualEffectImpl.SetParam("", false);
    Drawing::Matrix blurMat;
    geVisualEffectImpl.SetParam("", blurMat);
    std::vector<std::pair<float, float>> blurFractionStops;
    geVisualEffectImpl.SetParam("", blurFractionStops);

    EXPECT_EQ(geVisualEffectImpl.GetFilterType(), Drawing::GEVisualEffectImpl::FilterType::LINEAR_GRADIENT_BLUR);
}

/**
 * @tc.name: SetParam005
 * @tc.desc: Verify function SetParam for tag is invalid
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetParam005, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GE_FILTER_KAWASE_BLUR);
    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_KAWASE_BLUR_RADIUS, 2); // 2 blur radius
    EXPECT_EQ(geVisualEffectImpl.GetKawaseParams()->radius, 2);
    geVisualEffectImpl.SetParam("", 3); // 3 blur radius, but invalid
    EXPECT_NE(geVisualEffectImpl.GetKawaseParams()->radius, 3);
}

/**
 * @tc.name: SetParam006
 * @tc.desc: Verify function SetParam for tag is invalid
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetParam006, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR);
    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR_DIRECTION, 2); // 2 blur direction
    EXPECT_EQ(geVisualEffectImpl.GetLinearGradientBlurParams()->direction, 2);
    geVisualEffectImpl.SetParam("", 3); // 3 blur direction, but invalid
    EXPECT_NE(geVisualEffectImpl.GetLinearGradientBlurParams()->direction, 3);

    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR_IS_OFF_SCREEN, true);
    EXPECT_TRUE(geVisualEffectImpl.GetLinearGradientBlurParams()->isOffscreenCanvas);
    geVisualEffectImpl.SetParam("", false);
    EXPECT_TRUE(geVisualEffectImpl.GetLinearGradientBlurParams()->isOffscreenCanvas);

    Drawing::Matrix blurMat;
    blurMat.Set(Drawing::Matrix::SKEW_X, 0.002f); // 0.002f skew x
    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR_CANVAS_MAT, blurMat);
    EXPECT_EQ(geVisualEffectImpl.GetLinearGradientBlurParams()->mat, blurMat);
    Drawing::Matrix mat;
    mat.Set(Drawing::Matrix::SKEW_X, 0.005f); // 0.005f skew x
    geVisualEffectImpl.SetParam("", mat);
    EXPECT_EQ(geVisualEffectImpl.GetLinearGradientBlurParams()->mat, blurMat);

    std::vector<std::pair<float, float>> blurFractionStops { { 0.1f, 0.1f } };
    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR_FRACTION_STOPS, blurFractionStops);
    EXPECT_EQ(geVisualEffectImpl.GetLinearGradientBlurParams()->fractionStops, blurFractionStops);
    std::vector<std::pair<float, float>> expectFractionStops { { 0.2f, 0.2f } };
    geVisualEffectImpl.SetParam("", expectFractionStops);
    EXPECT_EQ(geVisualEffectImpl.GetLinearGradientBlurParams()->fractionStops, blurFractionStops);
}

/**
 * @tc.name: SetParam007
 * @tc.desc: Verify function SetParam for action is invalid
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetParam007, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GE_FILTER_AI_BAR);
    geVisualEffectImpl.SetAIBarParams(Drawing::GE_FILTER_AI_BAR_LOW, 1.0f);
    EXPECT_EQ(geVisualEffectImpl.GetAIBarParams()->aiBarLow, 1.0f);
    geVisualEffectImpl.SetAIBarParams("", 2.0f);
    EXPECT_NE(geVisualEffectImpl.GetAIBarParams()->aiBarLow, 2.0f);
}

/**
 * @tc.name: SetParam008
 * @tc.desc: Verify function SetParam for action is invalid
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetParam008, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GE_FILTER_GREY);
    geVisualEffectImpl.SetGreyParams(Drawing::GE_FILTER_GREY_COEF_1, 1.0f);
    EXPECT_EQ(geVisualEffectImpl.GetGreyParams()->greyCoef1, 1.0f);
    geVisualEffectImpl.SetGreyParams("", 2.0f);
    EXPECT_NE(geVisualEffectImpl.GetGreyParams()->greyCoef1, 2.0f);
}

/**
 * @tc.name: SetParam009
 * @tc.desc: Verify function SetParam for action is invalid
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetParam009, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR);
    geVisualEffectImpl.SetLinearGradientBlurParams(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR_RADIUS, 1.0f);
    EXPECT_EQ(geVisualEffectImpl.GetLinearGradientBlurParams()->blurRadius, 1.0f);
    geVisualEffectImpl.SetLinearGradientBlurParams("", 2.0f);
    EXPECT_NE(geVisualEffectImpl.GetLinearGradientBlurParams()->blurRadius, 2.0f);
}

/**
 * @tc.name: SetParam010
 * @tc.desc: Verify function SetParam for action is invalid
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetParam010, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GE_FILTER_WATER_RIPPLE);
    geVisualEffectImpl.SetWaterRippleParams(Drawing::GE_FILTER_WATER_RIPPLE_PROGRESS, 0.5f);
    EXPECT_EQ(geVisualEffectImpl.GetWaterRippleParams()->progress, 0.5f);
    geVisualEffectImpl.SetWaterRippleParams(Drawing::GE_FILTER_WATER_RIPPLE_RIPPLE_CENTER_X, 0.5f);
    EXPECT_EQ(geVisualEffectImpl.GetWaterRippleParams()->rippleCenterX, 0.5f);
    geVisualEffectImpl.SetWaterRippleParams(Drawing::GE_FILTER_WATER_RIPPLE_RIPPLE_CENTER_Y, 0.5f);
    EXPECT_EQ(geVisualEffectImpl.GetWaterRippleParams()->rippleCenterY, 0.5f);
    geVisualEffectImpl.SetWaterRippleParams(Drawing::GE_FILTER_WATER_RIPPLE_RIPPLE_MODE, 1.0f);
    EXPECT_EQ(geVisualEffectImpl.GetWaterRippleParams()->rippleMode, 1.0f);
}

/**
 * @tc.name: SetParam011
 * @tc.desc: Verify function SetParam for action is invalid
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetParam011, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImplWaterRipple(Drawing::GE_FILTER_WATER_RIPPLE);
    Drawing::GEVisualEffectImpl geVisualEffectImplKawaseBulr(Drawing::GE_FILTER_KAWASE_BLUR);

    // test invalid params setting
    geVisualEffectImplWaterRipple.SetParam(Drawing::GE_FILTER_WATER_RIPPLE_PROGRESS, 0.5);
    EXPECT_NE(geVisualEffectImplWaterRipple.GetWaterRippleParams()->progress, 0.5);

    // test invalid params setting
    uint32_t paramUint32 { 1 };
    geVisualEffectImplKawaseBulr.SetParam("GE_FILTER_KAWASE_BLUR_RADIUS", paramUint32);
    ASSERT_NE(geVisualEffectImplKawaseBulr.GetKawaseParams(), nullptr);
    EXPECT_NE(geVisualEffectImplKawaseBulr.GetKawaseParams()->radius, paramUint32);

    geVisualEffectImplKawaseBulr.SetParam("GE_FILTER_KAWASE_BLUR_RADIUS", "1");
    ASSERT_NE(geVisualEffectImplKawaseBulr.GetKawaseParams(), nullptr);
    EXPECT_NE(geVisualEffectImplKawaseBulr.GetKawaseParams()->radius, paramUint32);
}

/**
 * @tc.name: SetWaterRippleParams001
 * @tc.desc: Verify function SetWaterRippleParams is invalid
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetWaterRippleParams001, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GE_FILTER_WATER_RIPPLE);
    ASSERT_NE(geVisualEffectImpl.waterRippleParams_, nullptr);
    geVisualEffectImpl.SetWaterRippleParams(Drawing::GE_FILTER_WATER_RIPPLE_PROGRESS, 0.5f);
    EXPECT_EQ(geVisualEffectImpl.GetWaterRippleParams()->progress, 0.5f);
    geVisualEffectImpl.SetWaterRippleParams(Drawing::GE_FILTER_WATER_RIPPLE_RIPPLE_CENTER_X, 0.5f);
    EXPECT_EQ(geVisualEffectImpl.GetWaterRippleParams()->rippleCenterX, 0.5f);
    geVisualEffectImpl.SetWaterRippleParams(Drawing::GE_FILTER_WATER_RIPPLE_RIPPLE_CENTER_Y, 0.5f);
    EXPECT_EQ(geVisualEffectImpl.GetWaterRippleParams()->rippleCenterY, 0.5f);
    geVisualEffectImpl.SetWaterRippleParams(Drawing::GE_FILTER_WATER_RIPPLE_RIPPLE_MODE, 1.0f);
    EXPECT_EQ(geVisualEffectImpl.GetWaterRippleParams()->rippleMode, 1.0f);
}

/**
 * @tc.name: SetWaterRippleParams002
 * @tc.desc: Verify function SetWaterRippleParams
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetWaterRippleParams002, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GE_FILTER_WATER_RIPPLE);

    // test invalid params setting
    geVisualEffectImpl.SetWaterRippleParams("GE_FILTER_WATER_RIPPLE_PROGRESS", 0.5f);
    EXPECT_NE(geVisualEffectImpl.GetWaterRippleParams()->progress, 0.5f);
}

/**
 * @tc.name: MakeWaterRippleParams001
 * @tc.desc: Verify function MakeWaterRippleParams is invalid
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, MakeWaterRippleParams001, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl("");
    geVisualEffectImpl.SetFilterType(Drawing::GEVisualEffectImpl::FilterType::WATER_RIPPLE);
    ASSERT_EQ(geVisualEffectImpl.GetWaterRippleParams(), nullptr);
    geVisualEffectImpl.MakeWaterRippleParams();
    ASSERT_NE(geVisualEffectImpl.GetWaterRippleParams(), nullptr);
}

/**
 * @tc.name: GetMESAParams001
 * @tc.desc: Verify function GetMESAParams
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, GetMESAParams001, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl(Drawing::GE_FILTER_MESA_BLUR);
    // 0.0f, 1, 1.0f: mesa blur params
    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_MESA_BLUR_RADIUS, 1);
    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_MESA_BLUR_GREY_COEF_1, 1.0f);
    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_MESA_BLUR_GREY_COEF_2, 1.0f);
    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_MESA_BLUR_STRETCH_OFFSET_X, 0.0f);
    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_MESA_BLUR_STRETCH_OFFSET_Y, 0.0f);
    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_MESA_BLUR_STRETCH_OFFSET_Z, 0.0f);
    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_MESA_BLUR_STRETCH_OFFSET_W, 0.0f);
    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_MESA_BLUR_STRETCH_TILE_MODE, 0);
    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_MESA_BLUR_STRETCH_WIDTH, 0.0f);
    geVisualEffectImpl.SetParam(Drawing::GE_FILTER_MESA_BLUR_STRETCH_HEIGHT, 0.0f);
    EXPECT_EQ(geVisualEffectImpl.GetMESAParams()->radius, 1);
    EXPECT_EQ(geVisualEffectImpl.GetMESAParams()->greyCoef1, 1.0f);
    EXPECT_EQ(geVisualEffectImpl.GetMESAParams()->greyCoef1, 1.0f);
    EXPECT_EQ(geVisualEffectImpl.GetMESAParams()->offsetX, 0.0f);
    EXPECT_EQ(geVisualEffectImpl.GetMESAParams()->offsetY, 0.0f);
    EXPECT_EQ(geVisualEffectImpl.GetMESAParams()->offsetZ, 0.0f);
    EXPECT_EQ(geVisualEffectImpl.GetMESAParams()->offsetW, 0.0f);
    EXPECT_EQ(geVisualEffectImpl.GetMESAParams()->tileMode, 0);
    EXPECT_EQ(geVisualEffectImpl.GetMESAParams()->width, 0.0f);
    EXPECT_EQ(geVisualEffectImpl.GetMESAParams()->height, 0.0f);
}

/**
 * @tc.name: SetParam012
 * @tc.desc: Verify function SetParam for param is nullptr
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetParam012, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl("");
    int32_t paramInt32 { 0 }; // 0 invalid params
    geVisualEffectImpl.SetFilterType(Drawing::GEVisualEffectImpl::FilterType::KAWASE_BLUR);
    geVisualEffectImpl.SetParam("", paramInt32);

    geVisualEffectImpl.SetFilterType(Drawing::GEVisualEffectImpl::FilterType::MESA_BLUR);
    geVisualEffectImpl.SetParam("", paramInt32);

    geVisualEffectImpl.SetFilterType(Drawing::GEVisualEffectImpl::FilterType::MAGNIFIER);
    geVisualEffectImpl.SetParam("", paramInt32);

    uint32_t paramUint32 { 0 }; // 0 invalid params
    geVisualEffectImpl.SetFilterType(Drawing::GEVisualEffectImpl::FilterType::WATER_RIPPLE);
    geVisualEffectImpl.SetParam("", paramUint32);

    geVisualEffectImpl.SetFilterType(Drawing::GEVisualEffectImpl::FilterType::LINEAR_GRADIENT_BLUR);
    geVisualEffectImpl.SetParam("", paramInt32);

    EXPECT_EQ(geVisualEffectImpl.GetFilterType(), Drawing::GEVisualEffectImpl::FilterType::LINEAR_GRADIENT_BLUR);
}

/**
 * @tc.name: SetXXXParam001
 * @tc.desc: Verify function Set XXX Param for param is nullptr
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetXXXParam001, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl("");
    uint32_t paramUint32 { 0 };
    float paramFloat { 1.0f };
    EXPECT_EQ(geVisualEffectImpl.GetMESAParams(), nullptr);

    geVisualEffectImpl.SetMESABlurParams("", paramFloat);
    geVisualEffectImpl.SetMagnifierParamsFloat("", paramFloat);
    geVisualEffectImpl.SetMagnifierParamsUint32("", paramUint32);
    geVisualEffectImpl.SetWaterRippleParams("", paramFloat);
}

/**
 * @tc.name: SetXXXParam002
 * @tc.desc: Verify function Set XXX Param for param is not nullptr and tag is invalid
 * @tc.type:FUNC
 */
HWTEST_F(GEVisualEffectImplTest, SetXXXParam002, TestSize.Level1)
{
    Drawing::GEVisualEffectImpl geVisualEffectImpl("");
    uint32_t paramUint32 { 0 };
    float paramFloat { 1.0f };

    geVisualEffectImpl.MakeMESAParams();
    EXPECT_NE(geVisualEffectImpl.GetMESAParams(), nullptr);
    geVisualEffectImpl.SetMESABlurParams("", paramFloat);

    geVisualEffectImpl.MakeMagnifierParams();
    EXPECT_NE(geVisualEffectImpl.GetMagnifierParams(), nullptr);
    geVisualEffectImpl.SetMagnifierParamsFloat("", paramFloat);
    geVisualEffectImpl.SetMagnifierParamsUint32("", paramUint32);

    int32_t paramInt32 { 0 };
    geVisualEffectImpl.SetFilterType(Drawing::GEVisualEffectImpl::FilterType::MAGNIFIER);
    geVisualEffectImpl.SetParam("", paramInt32);
}

} // namespace GraphicsEffectEngine
} // namespace OHOS
