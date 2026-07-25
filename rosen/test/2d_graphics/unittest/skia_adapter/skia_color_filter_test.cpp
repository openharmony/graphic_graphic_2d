/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <cstddef>
#include "gtest/gtest.h"
#include "skia_adapter/skia_color_filter.h"
#include "effect/color_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaColorFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SkiaColorFilterTest::SetUpTestCase() {}
void SkiaColorFilterTest::TearDownTestCase() {}
void SkiaColorFilterTest::SetUp() {}
void SkiaColorFilterTest::TearDown() {}

/**
 * @tc.name: InitWithCompose001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaColorFilterTest, InitWithCompose001, TestSize.Level1)
{
    auto colorFilter1 = ColorFilter::FilterType::LINEAR_TO_SRGB_GAMMA;
    auto colorFilter2 = ColorFilter::FilterType::SRGB_GAMMA_TO_LINEAR;
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithCompose(ColorFilter(colorFilter1), ColorFilter(colorFilter2));
    EXPECT_TRUE(skiaColorFilter.GetSkColorFilter() != nullptr);
}

/**
 * @tc.name: Compose001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaColorFilterTest, Compose001, TestSize.Level1)
{
    auto colorFilter = ColorFilter::FilterType::SRGB_GAMMA_TO_LINEAR;
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithLinearToSrgbGamma();
    skiaColorFilter.Compose(ColorFilter(colorFilter));
    EXPECT_TRUE(skiaColorFilter.GetSkColorFilter() != nullptr);
}

/**
 * @tc.name: InitWithBlendMode001
 * @tc.desc: Test InitWithBlendMode
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaColorFilterTest, InitWithBlendMode001, TestSize.Level1)
{
    ColorQuad c = 0xFF000000;
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithBlendMode(c, BlendMode::CLEAR);
    EXPECT_TRUE(skiaColorFilter.GetSkColorFilter() != nullptr);
}

/**
 * @tc.name: InitWithColorMatrix001
 * @tc.desc: Test InitWithColorMatrix
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaColorFilterTest, InitWithColorMatrix001, TestSize.Level1)
{
    ColorMatrix cm;
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithColorMatrix(cm);
    EXPECT_TRUE(skiaColorFilter.GetSkColorFilter() != nullptr);
}

/**
 * @tc.name: InitWithColorFloat001
 * @tc.desc: Test InitWithColorFloat
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaColorFilterTest, InitWithColorFloat001, TestSize.Level1)
{
    float colorMatrixArray[20] = {
        0.402,  -1.174, -0.228, 1.0, 0.0,
        -0.598, -0.174, -0.228, 1.0, 0.0,
        -0.599, -1.175, 0.772,  1.0, 0.0,
        0.0,    0.0,    0.0,    1.0, 0.0
    };
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithColorFloat(colorMatrixArray);
    EXPECT_TRUE(skiaColorFilter.GetSkColorFilter() != nullptr);
}

/**
 * @tc.name: InitWithSrgbGammaToLinear001
 * @tc.desc: Test InitWithSrgbGammaToLinear
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaColorFilterTest, InitWithSrgbGammaToLinear001, TestSize.Level1)
{
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithSrgbGammaToLinear();
    EXPECT_TRUE(skiaColorFilter.GetSkColorFilter() != nullptr);
}

/**
 * @tc.name: InitWithLuma001
 * @tc.desc: Test InitWithLuma
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaColorFilterTest, InitWithLuma001, TestSize.Level1)
{
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithLuma();
    EXPECT_TRUE(skiaColorFilter.GetSkColorFilter() != nullptr);
}

/**
 * @tc.name: Serialize001
 * @tc.desc: Test Serialize
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaColorFilterTest, Serialize001, TestSize.Level1)
{
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.Serialize();
    skiaColorFilter.Deserialize(nullptr);
    EXPECT_TRUE(skiaColorFilter.GetSkColorFilter() == nullptr);
}

/**
 * @tc.name: AsAColorMatrix001
 * @tc.desc: Test AsAColorMatrix
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaColorFilterTest, AsAColorMatrix001, TestSize.Level1)
{
    SkiaColorFilter skiaColorFilter;
    scalar colorMatrixArray[20] = {
        0.402,  -1.174, -0.228, 1.0, 0.0,
        -0.598, -0.174, -0.228, 1.0, 0.0,
        -0.599, -1.175, 0.772,  1.0, 0.0,
        0.0,    0.0,    0.0,    1.0, 0.0
    };
    skiaColorFilter.AsAColorMatrix(colorMatrixArray);
    skiaColorFilter.InitWithLuma();
    skiaColorFilter.AsAColorMatrix(colorMatrixArray);
    EXPECT_TRUE(skiaColorFilter.GetSkColorFilter() != nullptr);
}

/**
 * @tc.name: CreateLightingColorFilter001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: IAZ845
 * @tc.author:
 */
 HWTEST_F(SkiaColorFilterTest, CreateLightingColorFilter001, TestSize.Level1)
 {
     auto colorFilter = ColorFilter::CreateLightingColorFilter(0xff00c800, 0xff001400);
     EXPECT_FALSE(nullptr == colorFilter);
 }

 /**
 * @tc.name: SkiaColorFilterGetSkColorFilter001
 * @tc.desc: Test SkiaColorFilter::GetSkColorFilter (renamed from GetColorFilter)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, SkiaColorFilterGetSkColorFilter001, TestSize.Level1)
{
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithLinearToSrgbGamma();
    auto skColorFilter = skiaColorFilter.GetSkColorFilter();
    EXPECT_NE(skColorFilter, nullptr);
}

/**
 * @tc.name: SkiaColorFilterGetSkColorFilter002
 * @tc.desc: Test SkiaColorFilter::GetSkColorFilter with blend mode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, SkiaColorFilterGetSkColorFilter002, TestSize.Level1)
{
    SkiaColorFilter skiaColorFilter;
    ColorQuad c = 0xFF000000;
    skiaColorFilter.InitWithBlendMode(c, BlendMode::CLEAR);
    auto skColorFilter = skiaColorFilter.GetSkColorFilter();
    EXPECT_NE(skColorFilter, nullptr);
}

/**
 * @tc.name: SkiaColorFilterGetSkColorFilter003
 * @tc.desc: Test SkiaColorFilter::GetSkColorFilter with color matrix
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, SkiaColorFilterGetSkColorFilter003, TestSize.Level1)
{
    SkiaColorFilter skiaColorFilter;
    ColorMatrix cm;
    skiaColorFilter.InitWithColorMatrix(cm);
    auto skColorFilter = skiaColorFilter.GetSkColorFilter();
    EXPECT_NE(skColorFilter, nullptr);
}

/**
 * @tc.name: SkiaColorFilterGetSkColorFilter004
 * @tc.desc: Test SkiaColorFilter::GetSkColorFilter returns nullptr when not initialized
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, SkiaColorFilterGetSkColorFilter004, TestSize.Level1)
{
    SkiaColorFilter skiaColorFilter;
    auto skColorFilter = skiaColorFilter.GetSkColorFilter();
    EXPECT_EQ(skColorFilter, nullptr);
}

/**
 * @tc.name: SkiaColorFilterGetSkColorFilter005
 * @tc.desc: Test SkiaColorFilter::GetSkColorFilter with luma
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, SkiaColorFilterGetSkColorFilter005, TestSize.Level1)
{
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithLuma();
    auto skColorFilter = skiaColorFilter.GetSkColorFilter();
    EXPECT_NE(skColorFilter, nullptr);
}

/**
 * @tc.name: SkiaColorFilterGetSkColorFilter006
 * @tc.desc: Test SkiaColorFilter::GetSkColorFilter with lighting
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, SkiaColorFilterGetSkColorFilter006, TestSize.Level1)
{
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithLighting(0xff00c800, 0xff001400);
    auto skColorFilter = skiaColorFilter.GetSkColorFilter();
    EXPECT_NE(skColorFilter, nullptr);
}

/**
 * @tc.name: SkiaColorFilterGetSkColorFilter007
 * @tc.desc: Test SkiaColorFilter::GetSkColorFilter with Compose
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, SkiaColorFilterGetSkColorFilter007, TestSize.Level1)
{
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithLinearToSrgbGamma();
    skiaColorFilter.Compose(ColorFilter(ColorFilter::FilterType::SRGB_GAMMA_TO_LINEAR));
    auto skColorFilter = skiaColorFilter.GetSkColorFilter();
    EXPECT_NE(skColorFilter, nullptr);
}

/**
 * @tc.name: InitWithBlendMode002
 * @tc.desc: Test InitWithBlendMode with SRC blend mode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, InitWithBlendMode002, TestSize.Level1)
{
    constexpr ColorQuad color = 0xFFFF0000;
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithBlendMode(color, BlendMode::SRC);
    EXPECT_NE(skiaColorFilter.GetSkColorFilter(), nullptr);
}

/**
 * @tc.name: InitWithBlendMode003
 * @tc.desc: Test InitWithBlendMode with DST blend mode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, InitWithBlendMode003, TestSize.Level1)
{
    constexpr ColorQuad color = 0xFF00FF00;
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithBlendMode(color, BlendMode::DST);
    EXPECT_EQ(skiaColorFilter.GetSkColorFilter(), nullptr);
}

/**
 * @tc.name: InitWithBlendMode004
 * @tc.desc: Test InitWithBlendMode with SRC_OVER blend mode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, InitWithBlendMode004, TestSize.Level1)
{
    constexpr ColorQuad color = 0xFF0000FF;
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithBlendMode(color, BlendMode::SRC_OVER);
    EXPECT_NE(skiaColorFilter.GetSkColorFilter(), nullptr);
}

/**
 * @tc.name: InitWithBlendMode005
 * @tc.desc: Test InitWithBlendMode with DST_OVER blend mode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, InitWithBlendMode005, TestSize.Level1)
{
    constexpr ColorQuad color = 0xFFFFFF00;
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithBlendMode(color, BlendMode::DST_OVER);
    EXPECT_NE(skiaColorFilter.GetSkColorFilter(), nullptr);
}

/**
 * @tc.name: InitWithBlendMode006
 * @tc.desc: Test InitWithBlendMode with SRC_IN blend mode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, InitWithBlendMode006, TestSize.Level1)
{
    constexpr ColorQuad color = 0xFFFF00FF;
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithBlendMode(color, BlendMode::SRC_IN);
    EXPECT_NE(skiaColorFilter.GetSkColorFilter(), nullptr);
}

/**
 * @tc.name: InitWithBlendMode007
 * @tc.desc: Test InitWithBlendMode with DST_IN blend mode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, InitWithBlendMode007, TestSize.Level1)
{
    constexpr ColorQuad color = 0xFF00FFFF;
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithBlendMode(color, BlendMode::DST_IN);
    EXPECT_EQ(skiaColorFilter.GetSkColorFilter(), nullptr);
}

/**
 * @tc.name: InitWithBlendMode008
 * @tc.desc: Test InitWithBlendMode with MULTIPLY blend mode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, InitWithBlendMode008, TestSize.Level1)
{
    constexpr ColorQuad color = 0xFF808080;
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithBlendMode(color, BlendMode::MULTIPLY);
    EXPECT_NE(skiaColorFilter.GetSkColorFilter(), nullptr);
}

/**
 * @tc.name: InitWithBlendMode009
 * @tc.desc: Test InitWithBlendMode with SCREEN blend mode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, InitWithBlendMode009, TestSize.Level1)
{
    constexpr ColorQuad color = 0xFF404040;
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithBlendMode(color, BlendMode::SCREEN);
    EXPECT_NE(skiaColorFilter.GetSkColorFilter(), nullptr);
}

/**
 * @tc.name: InitWithBlendMode010
 * @tc.desc: Test InitWithBlendMode with OVERLAY blend mode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, InitWithBlendMode010, TestSize.Level1)
{
    constexpr ColorQuad color = 0xFFC0C0C0;
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithBlendMode(color, BlendMode::OVERLAY);
    EXPECT_NE(skiaColorFilter.GetSkColorFilter(), nullptr);
}

/**
 * @tc.name: InitWithBlendMode011
 * @tc.desc: Test InitWithBlendMode with DARKEN blend mode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, InitWithBlendMode011, TestSize.Level1)
{
    constexpr ColorQuad color = 0xFF333333;
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithBlendMode(color, BlendMode::DARKEN);
    EXPECT_NE(skiaColorFilter.GetSkColorFilter(), nullptr);
}

/**
 * @tc.name: InitWithBlendMode012
 * @tc.desc: Test InitWithBlendMode with LIGHTEN blend mode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, InitWithBlendMode012, TestSize.Level1)
{
    constexpr ColorQuad color = 0xFFCCCCCC;
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithBlendMode(color, BlendMode::LIGHTEN);
    EXPECT_NE(skiaColorFilter.GetSkColorFilter(), nullptr);
}

/**
 * @tc.name: InitWithBlendMode013
 * @tc.desc: Test InitWithBlendMode with COLOR_DODGE blend mode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, InitWithBlendMode013, TestSize.Level1)
{
    constexpr ColorQuad color = 0xFF666666;
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithBlendMode(color, BlendMode::COLOR_DODGE);
    EXPECT_NE(skiaColorFilter.GetSkColorFilter(), nullptr);
}

/**
 * @tc.name: InitWithBlendMode014
 * @tc.desc: Test InitWithBlendMode with COLOR_BURN blend mode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, InitWithBlendMode014, TestSize.Level1)
{
    constexpr ColorQuad color = 0xFF999999;
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithBlendMode(color, BlendMode::COLOR_BURN);
    EXPECT_NE(skiaColorFilter.GetSkColorFilter(), nullptr);
}

/**
 * @tc.name: InitWithBlendMode015
 * @tc.desc: Test InitWithBlendMode with HARD_LIGHT blend mode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, InitWithBlendMode015, TestSize.Level1)
{
    constexpr ColorQuad color = 0xFF1A1A1A;
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithBlendMode(color, BlendMode::HARD_LIGHT);
    EXPECT_NE(skiaColorFilter.GetSkColorFilter(), nullptr);
}

/**
 * @tc.name: InitWithBlendMode016
 * @tc.desc: Test InitWithBlendMode with SOFT_LIGHT blend mode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, InitWithBlendMode016, TestSize.Level1)
{
    constexpr ColorQuad color = 0xFFE5E5E5;
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithBlendMode(color, BlendMode::SOFT_LIGHT);
    EXPECT_NE(skiaColorFilter.GetSkColorFilter(), nullptr);
}

/**
 * @tc.name: InitWithColorMatrix002
 * @tc.desc: Test InitWithColorMatrix with custom matrix
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, InitWithColorMatrix002, TestSize.Level1)
{
    ColorMatrix cm;
    cm.SetIdentity();
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithColorMatrix(cm);
    EXPECT_NE(skiaColorFilter.GetSkColorFilter(), nullptr);
}

/**
 * @tc.name: InitWithColorMatrix003
 * @tc.desc: Test InitWithColorMatrix with saturation matrix
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, InitWithColorMatrix003, TestSize.Level1)
{
    ColorMatrix cm;
    constexpr scalar saturation = 0.5f;
    cm.SetSaturation(saturation);
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithColorMatrix(cm);
    EXPECT_NE(skiaColorFilter.GetSkColorFilter(), nullptr);
}

/**
 * @tc.name: InitWithColorMatrix004
 * @tc.desc: Test InitWithColorMatrix with scale matrix
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, InitWithColorMatrix004, TestSize.Level1)
{
    ColorMatrix cm;
    cm.SetScale(1.5f, 0.8f, 1.2f, 1.0f);
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithColorMatrix(cm);
    EXPECT_NE(skiaColorFilter.GetSkColorFilter(), nullptr);
}

/**
 * @tc.name: InitWithColorMatrix005
 * @tc.desc: Test InitWithColorMatrix with concat matrix
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, InitWithColorMatrix005, TestSize.Level1)
{
    ColorMatrix cm1;
    ColorMatrix cm2;
    ColorMatrix result;
    cm1.SetSaturation(0.0f);
    cm2.SetScale(1.2f, 1.0f, 0.8f, 1.0f);
    result.SetConcat(cm1, cm2);
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithColorMatrix(result);
    EXPECT_NE(skiaColorFilter.GetSkColorFilter(), nullptr);
}

/**
 * @tc.name: InitWithColorFloat002
 * @tc.desc: Test InitWithColorFloat with grayscale matrix
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, InitWithColorFloat002, TestSize.Level1)
{
    constexpr float grayscaleMatrix[20] = {
        0.33f, 0.33f, 0.33f, 0.0f, 0.0f,
        0.33f, 0.33f, 0.33f, 0.0f, 0.0f,
        0.33f, 0.33f, 0.33f, 0.0f, 0.0f,
        0.0f,  0.0f,  0.0f,  1.0f, 0.0f
    };
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithColorFloat(grayscaleMatrix);
    EXPECT_NE(skiaColorFilter.GetSkColorFilter(), nullptr);
}

/**
 * @tc.name: InitWithColorFloat003
 * @tc.desc: Test InitWithColorFloat with invert matrix
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, InitWithColorFloat003, TestSize.Level1)
{
    constexpr float invertMatrix[20] = {
        -1.0f, 0.0f,  0.0f,  0.0f, 255.0f,
        0.0f,  -1.0f, 0.0f,  0.0f, 255.0f,
        0.0f,  0.0f,  -1.0f, 0.0f, 255.0f,
        0.0f,  0.0f,  0.0f,  1.0f, 0.0f
    };
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithColorFloat(invertMatrix);
    EXPECT_NE(skiaColorFilter.GetSkColorFilter(), nullptr);
}

/**
 * @tc.name: InitWithColorFloat004
 * @tc.desc: Test InitWithColorFloat with sepia tone matrix
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, InitWithColorFloat004, TestSize.Level1)
{
    constexpr float sepiaMatrix[20] = {
        0.393f, 0.769f, 0.189f, 0.0f, 0.0f,
        0.349f, 0.686f, 0.168f, 0.0f, 0.0f,
        0.272f, 0.534f, 0.131f, 0.0f, 0.0f,
        0.0f,   0.0f,   0.0f,   1.0f, 0.0f
    };
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithColorFloat(sepiaMatrix);
    EXPECT_NE(skiaColorFilter.GetSkColorFilter(), nullptr);
}

/**
 * @tc.name: InitWithColorFloat005
 * @tc.desc: Test InitWithColorFloat with identity matrix
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, InitWithColorFloat005, TestSize.Level1)
{
    constexpr float identityMatrix[20] = {
        1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f, 0.0f
    };
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithColorFloat(identityMatrix);
    EXPECT_NE(skiaColorFilter.GetSkColorFilter(), nullptr);
}

/**
 * @tc.name: InitWithCompose002
 * @tc.desc: Test InitWithCompose with two linear gamma filters
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, InitWithCompose002, TestSize.Level1)
{
    auto colorFilter1 = ColorFilter::FilterType::LINEAR_TO_SRGB_GAMMA;
    auto colorFilter2 = ColorFilter::FilterType::LINEAR_TO_SRGB_GAMMA;
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithCompose(ColorFilter(colorFilter1), ColorFilter(colorFilter2));
    EXPECT_NE(skiaColorFilter.GetSkColorFilter(), nullptr);
}

/**
 * @tc.name: InitWithCompose003
 * @tc.desc: Test InitWithCompose with blend mode and matrix filter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, InitWithCompose003, TestSize.Level1)
{
    ColorFilter blendFilter(ColorFilter::FilterType::BLEND_MODE, 0xFFFF0000, BlendMode::SRC_OVER);
    ColorMatrix cm;
    ColorFilter matrixFilter(ColorFilter::FilterType::MATRIX, cm);
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithCompose(blendFilter, matrixFilter);
    EXPECT_NE(skiaColorFilter.GetSkColorFilter(), nullptr);
}

/**
 * @tc.name: InitWithCompose004
 * @tc.desc: Test InitWithCompose with float arrays
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, InitWithCompose004, TestSize.Level1)
{
    constexpr float matrix1[20] = {
        1.0f, 0.0f, 0.0f, 0.0f, 10.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 10.0f,
        0.0f, 0.0f, 1.0f, 0.0f, 10.0f,
        0.0f, 0.0f, 0.0f, 1.0f, 0.0f
    };
    constexpr float matrix2[20] = {
        1.5f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.5f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f, 0.0f
    };
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithCompose(matrix1, matrix2);
    EXPECT_NE(skiaColorFilter.GetSkColorFilter(), nullptr);
}

/**
 * @tc.name: InitWithCompose005
 * @tc.desc: Test InitWithCompose with float arrays and NO_CLAMP
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, InitWithCompose005, TestSize.Level1)
{
    constexpr float matrix1[20] = {
        0.5f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f, 0.0f
    };
    constexpr float matrix2[20] = {
        2.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f, 0.0f
    };
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithCompose(matrix1, matrix2, Clamp::NO_CLAMP);
    EXPECT_NE(skiaColorFilter.GetSkColorFilter(), nullptr);
}

/**
 * @tc.name: Compose002
 * @tc.desc: Test Compose with matrix filter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, Compose002, TestSize.Level1)
{
    ColorMatrix cm;
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithColorMatrix(cm);
    ColorFilter composeFilter(ColorFilter::FilterType::MATRIX, cm);
    skiaColorFilter.Compose(composeFilter);
    EXPECT_NE(skiaColorFilter.GetSkColorFilter(), nullptr);
}

/**
 * @tc.name: Compose003
 * @tc.desc: Test Compose with luma filter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, Compose003, TestSize.Level1)
{
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithLuma();
    ColorFilter lumaFilter(ColorFilter::FilterType::LUMA);
    skiaColorFilter.Compose(lumaFilter);
    EXPECT_NE(skiaColorFilter.GetSkColorFilter(), nullptr);
}

/**
 * @tc.name: Compose004
 * @tc.desc: Test Compose multiple times
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, Compose004, TestSize.Level1)
{
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithSrgbGammaToLinear();
    ColorFilter filter1(ColorFilter::FilterType::LINEAR_TO_SRGB_GAMMA);
    ColorFilter filter2(ColorFilter::FilterType::SRGB_GAMMA_TO_LINEAR);
    skiaColorFilter.Compose(filter1);
    skiaColorFilter.Compose(filter2);
    EXPECT_NE(skiaColorFilter.GetSkColorFilter(), nullptr);
}

/**
 * @tc.name: InitWithOverDrawColor001
 * @tc.desc: Test InitWithOverDrawColor
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, InitWithOverDrawColor001, TestSize.Level1)
{
    constexpr ColorQuad colors[6] = {
        0xFFFFFFFF,
        0xFFFF0000,
        0xFF00FF00,
        0xFF0000FF,
        0xFFFFFF00,
        0xFFFF00FF
    };
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithOverDrawColor(colors);
    EXPECT_NE(skiaColorFilter.GetSkColorFilter(), nullptr);
}

/**
 * @tc.name: InitWithOverDrawColor002
 * @tc.desc: Test InitWithOverDrawColor with transparent colors
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, InitWithOverDrawColor002, TestSize.Level1)
{
    constexpr ColorQuad colors[6] = {
        0x00FFFFFF,
        0x33FF0000,
        0x6600FF00,
        0x990000FF,
        0xCCFFFF00,
        0xFFFF00FF
    };
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithOverDrawColor(colors);
    EXPECT_NE(skiaColorFilter.GetSkColorFilter(), nullptr);
}

/**
 * @tc.name: InitWithOverDrawColor003
 * @tc.desc: Test InitWithOverDrawColor with black colors
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, InitWithOverDrawColor003, TestSize.Level1)
{
    constexpr ColorQuad colors[6] = {
        0xFF000000,
        0xFF111111,
        0xFF222222,
        0xFF333333,
        0xFF444444,
        0xFF555555
    };
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithOverDrawColor(colors);
    EXPECT_NE(skiaColorFilter.GetSkColorFilter(), nullptr);
}

/**
 * @tc.name: InitWithLighting002
 * @tc.desc: Test InitWithLighting with different multiply and add values
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, InitWithLighting002, TestSize.Level1)
{
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithLighting(0x80808080, 0xFFFFFFFF);
    EXPECT_NE(skiaColorFilter.GetSkColorFilter(), nullptr);
}

/**
 * @tc.name: InitWithLighting003
 * @tc.desc: Test InitWithLighting with zero multiply
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, InitWithLighting003, TestSize.Level1)
{
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithLighting(0x00000000, 0x80808080);
    EXPECT_NE(skiaColorFilter.GetSkColorFilter(), nullptr);
}

/**
 * @tc.name: InitWithLighting004
 * @tc.desc: Test InitWithLighting with zero add
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, InitWithLighting004, TestSize.Level1)
{
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithLighting(0xFFFFFFFF, 0x00000000);
    EXPECT_NE(skiaColorFilter.GetSkColorFilter(), nullptr);
}

/**
 * @tc.name: InitWithLighting005
 * @tc.desc: Test InitWithLighting with red channel only
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, InitWithLighting005, TestSize.Level1)
{
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithLighting(0xFFFF0000, 0x00FF0000);
    EXPECT_NE(skiaColorFilter.GetSkColorFilter(), nullptr);
}

/**
 * @tc.name: InitWithLighting006
 * @tc.desc: Test InitWithLighting with green channel only
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, InitWithLighting006, TestSize.Level1)
{
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithLighting(0xFF00FF00, 0x0000FF00);
    EXPECT_NE(skiaColorFilter.GetSkColorFilter(), nullptr);
}

/**
 * @tc.name: InitWithLighting007
 * @tc.desc: Test InitWithLighting with blue channel only
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, InitWithLighting007, TestSize.Level1)
{
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithLighting(0xFF0000FF, 0x000000FF);
    EXPECT_NE(skiaColorFilter.GetSkColorFilter(), nullptr);
}

/**
 * @tc.name: AsAColorMatrix002
 * @tc.desc: Test AsAColorMatrix with initialized matrix filter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, AsAColorMatrix002, TestSize.Level1)
{
    ColorMatrix cm;
    cm.SetIdentity();
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithColorMatrix(cm);
    scalar resultMatrix[20] = { 0 };
    skiaColorFilter.AsAColorMatrix(resultMatrix);
    EXPECT_NE(skiaColorFilter.GetSkColorFilter(), nullptr);
}

/**
 * @tc.name: AsAColorMatrix003
 * @tc.desc: Test AsAColorMatrix with custom matrix
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, AsAColorMatrix003, TestSize.Level1)
{
    constexpr float customMatrix[20] = {
        0.5f, 0.0f, 0.0f, 0.0f, 50.0f,
        0.0f, 0.5f, 0.0f, 0.0f, 50.0f,
        0.0f, 0.0f, 0.5f, 0.0f, 50.0f,
        0.0f, 0.0f, 0.0f, 1.0f, 0.0f
    };
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithColorFloat(customMatrix);
    scalar resultMatrix[20] = { 0 };
    skiaColorFilter.AsAColorMatrix(resultMatrix);
    EXPECT_NE(skiaColorFilter.GetSkColorFilter(), nullptr);
}

/**
 * @tc.name: AsAColorMatrix004
 * @tc.desc: Test AsAColorMatrix with srgb gamma filter (non-matrix)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, AsAColorMatrix004, TestSize.Level1)
{
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithSrgbGammaToLinear();
    scalar resultMatrix[20] = { 0 };
    skiaColorFilter.AsAColorMatrix(resultMatrix);
    EXPECT_NE(skiaColorFilter.GetSkColorFilter(), nullptr);
}

/**
 * @tc.name: SetColorFilter001
 * @tc.desc: Test SetColorFilter with valid filter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, SetColorFilter001, TestSize.Level1)
{
    SkiaColorFilter skiaColorFilter1;
    skiaColorFilter1.InitWithLuma();
    auto filter = skiaColorFilter1.GetSkColorFilter();
    
    SkiaColorFilter skiaColorFilter2;
    skiaColorFilter2.SetColorFilter(filter);
    EXPECT_NE(skiaColorFilter2.GetSkColorFilter(), nullptr);
}

/**
 * @tc.name: SetColorFilter002
 * @tc.desc: Test SetColorFilter with nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, SetColorFilter002, TestSize.Level1)
{
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithLuma();
    skiaColorFilter.SetColorFilter(nullptr);
    EXPECT_EQ(skiaColorFilter.GetSkColorFilter(), nullptr);
}

/**
 * @tc.name: SetColorFilter003
 * @tc.desc: Test SetColorFilter overwriting existing filter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, SetColorFilter003, TestSize.Level1)
{
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithLinearToSrgbGamma();
    EXPECT_NE(skiaColorFilter.GetSkColorFilter(), nullptr);
    
    SkiaColorFilter tempFilter;
    tempFilter.InitWithLuma();
    skiaColorFilter.SetColorFilter(tempFilter.GetSkColorFilter());
    EXPECT_NE(skiaColorFilter.GetSkColorFilter(), nullptr);
}

/**
 * @tc.name: MultipleOperations001
 * @tc.desc: Test multiple operations on same filter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, MultipleOperations001, TestSize.Level1)
{
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithLinearToSrgbGamma();
    EXPECT_NE(skiaColorFilter.GetSkColorFilter(), nullptr);
    
    ColorFilter composeFilter(ColorFilter::FilterType::SRGB_GAMMA_TO_LINEAR);
    skiaColorFilter.Compose(composeFilter);
    EXPECT_NE(skiaColorFilter.GetSkColorFilter(), nullptr);
    
    ColorFilter secondCompose(ColorFilter::FilterType::LINEAR_TO_SRGB_GAMMA);
    skiaColorFilter.Compose(secondCompose);
    EXPECT_NE(skiaColorFilter.GetSkColorFilter(), nullptr);
}

/**
 * @tc.name: MultipleOperations002
 * @tc.desc: Test multiple init operations (last one wins)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, MultipleOperations002, TestSize.Level1)
{
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithLuma();
    EXPECT_NE(skiaColorFilter.GetSkColorFilter(), nullptr);
    
    skiaColorFilter.InitWithSrgbGammaToLinear();
    EXPECT_NE(skiaColorFilter.GetSkColorFilter(), nullptr);
}

/**
 * @tc.name: GetTypeTest001
 * @tc.desc: Test GetType returns correct adapter type
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, GetTypeTest001, TestSize.Level1)
{
    SkiaColorFilter skiaColorFilter;
    EXPECT_EQ(skiaColorFilter.GetType(), AdapterType::SKIA_ADAPTER);
}

/**
 * @tc.name: GetTypeTest002
 * @tc.desc: Test GetType after initialization
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, GetTypeTest002, TestSize.Level1)
{
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithLuma();
    EXPECT_EQ(skiaColorFilter.GetType(), AdapterType::SKIA_ADAPTER);
}

/**
 * @tc.name: DeserializeWithValidData001
 * @tc.desc: Test Deserialize with valid serialized data
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, DeserializeWithValidData001, TestSize.Level1)
{
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithLuma();
    auto data = skiaColorFilter.Serialize();
    
    SkiaColorFilter newFilter;
    bool result = newFilter.Deserialize(data);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: DeserializeWithValidData002
 * @tc.desc: Test Deserialize with matrix filter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, DeserializeWithValidData002, TestSize.Level1)
{
    ColorMatrix cm;
    cm.SetSaturation(0.5f);
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithColorMatrix(cm);
    auto data = skiaColorFilter.Serialize();
    
    SkiaColorFilter newFilter;
    bool result = newFilter.Deserialize(data);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: InitWithColorMatrixWithClamp001
 * @tc.desc: Test InitWithColorMatrix with YES_CLAMP
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, InitWithColorMatrixWithClamp001, TestSize.Level1)
{
    ColorMatrix cm;
    cm.SetScale(2.0f, 2.0f, 2.0f, 1.0f);
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithColorMatrix(cm, Clamp::YES_CLAMP);
    EXPECT_NE(skiaColorFilter.GetSkColorFilter(), nullptr);
}

/**
 * @tc.name: InitWithColorMatrixWithClamp002
 * @tc.desc: Test InitWithColorMatrix with NO_CLAMP
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, InitWithColorMatrixWithClamp002, TestSize.Level1)
{
    ColorMatrix cm;
    cm.SetScale(2.0f, 2.0f, 2.0f, 1.0f);
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithColorMatrix(cm, Clamp::NO_CLAMP);
    EXPECT_NE(skiaColorFilter.GetSkColorFilter(), nullptr);
}

/**
 * @tc.name: InitWithColorFloatWithClamp001
 * @tc.desc: Test InitWithColorFloat with YES_CLAMP
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, InitWithColorFloatWithClamp001, TestSize.Level1)
{
    constexpr float brightnessMatrix[20] = {
        1.0f, 0.0f, 0.0f, 0.0f, 50.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 50.0f,
        0.0f, 0.0f, 1.0f, 0.0f, 50.0f,
        0.0f, 0.0f, 0.0f, 1.0f, 0.0f
    };
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithColorFloat(brightnessMatrix, Clamp::YES_CLAMP);
    EXPECT_NE(skiaColorFilter.GetSkColorFilter(), nullptr);
}

/**
 * @tc.name: InitWithColorFloatWithClamp002
 * @tc.desc: Test InitWithColorFloat with NO_CLAMP
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, InitWithColorFloatWithClamp002, TestSize.Level1)
{
    constexpr float contrastMatrix[20] = {
        1.5f, 0.0f, 0.0f, 0.0f, -25.0f,
        0.0f, 1.5f, 0.0f, 0.0f, -25.0f,
        0.0f, 0.0f, 1.5f, 0.0f, -25.0f,
        0.0f, 0.0f, 0.0f, 1.0f, 0.0f
    };
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithColorFloat(contrastMatrix, Clamp::NO_CLAMP);
    EXPECT_NE(skiaColorFilter.GetSkColorFilter(), nullptr);
}

/**
 * @tc.name: CreateBlendModeColorFilterTest001
 * @tc.desc: Test ColorFilter::CreateBlendModeColorFilter static method
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, CreateBlendModeColorFilterTest001, TestSize.Level1)
{
    auto colorFilter = ColorFilter::CreateBlendModeColorFilter(0xFF0000FF, BlendMode::SRC_OVER);
    EXPECT_NE(colorFilter, nullptr);
}

/**
 * @tc.name: CreateComposeColorFilterTest001
 * @tc.desc: Test ColorFilter::CreateComposeColorFilter static method
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, CreateComposeColorFilterTest001, TestSize.Level1)
{
    ColorFilter filter1(ColorFilter::FilterType::LINEAR_TO_SRGB_GAMMA);
    ColorFilter filter2(ColorFilter::FilterType::SRGB_GAMMA_TO_LINEAR);
    auto colorFilter = ColorFilter::CreateComposeColorFilter(filter1, filter2);
    EXPECT_NE(colorFilter, nullptr);
}

/**
 * @tc.name: CreateMatrixColorFilterTest001
 * @tc.desc: Test ColorFilter::CreateMatrixColorFilter static method
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, CreateMatrixColorFilterTest001, TestSize.Level1)
{
    ColorMatrix cm;
    cm.SetIdentity();
    auto colorFilter = ColorFilter::CreateMatrixColorFilter(cm);
    EXPECT_NE(colorFilter, nullptr);
}

/**
 * @tc.name: CreateFloatColorFilterTest001
 * @tc.desc: Test ColorFilter::CreateFloatColorFilter static method
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, CreateFloatColorFilterTest001, TestSize.Level1)
{
    constexpr float matrix[20] = {
        1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f, 0.0f
    };
    auto colorFilter = ColorFilter::CreateFloatColorFilter(matrix);
    EXPECT_NE(colorFilter, nullptr);
}

/**
 * @tc.name: CreateLinearToSrgbGammaTest001
 * @tc.desc: Test ColorFilter::CreateLinearToSrgbGamma static method
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, CreateLinearToSrgbGammaTest001, TestSize.Level1)
{
    auto colorFilter = ColorFilter::CreateLinearToSrgbGamma();
    EXPECT_NE(colorFilter, nullptr);
}

/**
 * @tc.name: CreateSrgbGammaToLinearTest001
 * @tc.desc: Test ColorFilter::CreateSrgbGammaToLinear static method
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, CreateSrgbGammaToLinearTest001, TestSize.Level1)
{
    auto colorFilter = ColorFilter::CreateSrgbGammaToLinear();
    EXPECT_NE(colorFilter, nullptr);
}

/**
 * @tc.name: CreateOverDrawColorFilterTest001
 * @tc.desc: Test ColorFilter::CreateOverDrawColorFilter static method
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, CreateOverDrawColorFilterTest001, TestSize.Level1)
{
    constexpr ColorQuad colors[6] = {
        0xFFFFFFFF, 0xFFFF0000, 0xFF00FF00,
        0xFF0000FF, 0xFFFFFF00, 0xFFFF00FF
    };
    auto colorFilter = ColorFilter::CreateOverDrawColorFilter(colors);
    EXPECT_NE(colorFilter, nullptr);
}

/**
 * @tc.name: CreateLumaColorFilterTest001
 * @tc.desc: Test ColorFilter::CreateLumaColorFilter static method
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, CreateLumaColorFilterTest001, TestSize.Level1)
{
    auto colorFilter = ColorFilter::CreateLumaColorFilter();
    EXPECT_NE(colorFilter, nullptr);
}

/**
 * @tc.name: FilterTypeTest001
 * @tc.desc: Test ColorFilter::GetType method
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, FilterTypeTest001, TestSize.Level1)
{
    ColorFilter colorFilter(ColorFilter::FilterType::LUMA);
    EXPECT_EQ(colorFilter.GetType(), ColorFilter::FilterType::LUMA);
}

/**
 * @tc.name: FilterTypeTest002
 * @tc.desc: Test ColorFilter::GetType with blend mode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, FilterTypeTest002, TestSize.Level1)
{
    ColorFilter colorFilter(ColorFilter::FilterType::BLEND_MODE, 0xFFFF0000, BlendMode::SRC);
    EXPECT_EQ(colorFilter.GetType(), ColorFilter::FilterType::BLEND_MODE);
}

/**
 * @tc.name: FilterTypeTest003
 * @tc.desc: Test ColorFilter::GetType with matrix
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, FilterTypeTest003, TestSize.Level1)
{
    ColorMatrix cm;
    ColorFilter colorFilter(ColorFilter::FilterType::MATRIX, cm);
    EXPECT_EQ(colorFilter.GetType(), ColorFilter::FilterType::MATRIX);
}

/**
 * @tc.name: ColorFilterConstructorTest001
 * @tc.desc: Test ColorFilter constructor with FilterType
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, ColorFilterConstructorTest001, TestSize.Level1)
{
    ColorFilter colorFilter(ColorFilter::FilterType::LINEAR_TO_SRGB_GAMMA);
    EXPECT_EQ(colorFilter.GetType(), ColorFilter::FilterType::LINEAR_TO_SRGB_GAMMA);
}

/**
 * @tc.name: ColorFilterConstructorTest002
 * @tc.desc: Test ColorFilter constructor with float array
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, ColorFilterConstructorTest002, TestSize.Level1)
{
    constexpr float matrix[20] = {
        1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f, 0.0f
    };
    ColorFilter colorFilter(ColorFilter::FilterType::MATRIX, matrix);
    EXPECT_EQ(colorFilter.GetType(), ColorFilter::FilterType::MATRIX);
}

/**
 * @tc.name: ColorFilterComposeTest001
 * @tc.desc: Test ColorFilter::Compose method
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, ColorFilterComposeTest001, TestSize.Level1)
{
    ColorFilter colorFilter(ColorFilter::FilterType::LINEAR_TO_SRGB_GAMMA);
    ColorFilter otherFilter(ColorFilter::FilterType::SRGB_GAMMA_TO_LINEAR);
    colorFilter.Compose(otherFilter);
    EXPECT_NE(colorFilter.GetSkColorFilter(), nullptr);
}

/**
 * @tc.name: ColorFilterAsAColorMatrixTest001
 * @tc.desc: Test ColorFilter::AsAColorMatrix method
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, ColorFilterAsAColorMatrixTest001, TestSize.Level1)
{
    ColorMatrix cm;
    ColorFilter colorFilter(ColorFilter::FilterType::MATRIX, cm);
    scalar result[20] = { 0 };
    bool isMatrix = colorFilter.AsAColorMatrix(result);
    EXPECT_TRUE(isMatrix);
}

/**
 * @tc.name: ColorFilterAsAColorMatrixTest002
 * @tc.desc: Test ColorFilter::AsAColorMatrix returns false for non-matrix filter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorFilterTest, ColorFilterAsAColorMatrixTest002, TestSize.Level1)
{
    ColorFilter colorFilter(ColorFilter::FilterType::LUMA);
    scalar result[20] = { 0 };
    bool isMatrix = colorFilter.AsAColorMatrix(result);
    EXPECT_FALSE(isMatrix);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS