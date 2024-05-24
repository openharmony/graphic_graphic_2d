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
    int32_t paramInt32{1};
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
    float paramfloat{1.f};
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
    int64_t paramInt64{1}; // 1 is linear gradient blur params：CANVAS_MAT
    geVisualEffectImpl1.SetParam(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR_CANVAS_MAT, paramInt64);
    geVisualEffectImpl2.SetParam(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR, image);
    geVisualEffectImpl2.SetParam(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR, colorFilter);

    geVisualEffectImpl2.SetParam(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR_CANVAS_MAT, mat);
    EXPECT_EQ(geVisualEffectImpl2.GetLinearGradientBlurParams()->mat, mat);

    // 0.1f, 0.1f is linear gradient blur params: FRACTION_STOPS
    geVisualEffectImpl2.SetParam(Drawing::GE_FILTER_LINEAR_GRADIENT_BLUR_FRACTION_STOPS, {{0.1f, 0.1f}});
    std::vector<std::pair<float, float>> expectFraStops {{0.1f, 0.1f}};
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

} // namespace GraphicsEffectEngine
} // namespace OHOS
