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
#include "gtest/gtest.h"

#include "utils/rect.h"
#include "draw/color.h"
#include "image/bitmap.h"
#include "render/rs_hps_blur.h"
#include "pipeline/rs_paint_filter_canvas.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSHpsBlurUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<Drawing::Image> MakeImage(Drawing::Canvas& canvas);

    static inline Drawing::Canvas canvas_;
    std::shared_ptr<Drawing::Image> image_ { nullptr };

    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src_ { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst_ { 1.0f, 1.0f, 2.0f, 2.0f };
};

void RSHpsBlurUnitTest::SetUpTestCase() {}
void RSHpsBlurUnitTest::TearDownTestCase() {}
void RSHpsBlurUnitTest::SetUp()
{
    canvas_.Restore();
    Drawing::Bitmap testBitmap;
    Drawing::BitmapFormat bitmapFormat { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    testBitmap.Build(50, 50, bitmapFormat); // 50, 50  bitmap size
    testBitmap.ClearWithColor(Drawing::Color::COLOR_BLUE);
    image_ = testBitmap.MakeImage();
}

void RSHpsBlurUnitTest::TearDown() {}

/**
 * @tc.name: TestGetShaderTransform
 * @tc.desc: Verify function GetShaderTransform
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHpsBlurUnitTest, TestGetShaderTransform, TestSize.Level1)
{
    Drawing::Matrix testMatrix;
    Drawing::Rect blurRect(0, 0, 100, 100);
    float scaleW = 1.0;
    float scaleH = 1.0;

    HpsBlurFilter testFilter;
    EXPECT_EQ(testFilter.GetShaderTransform(blurRect, scaleW, scaleH), testMatrix);
}

/**
 * @tc.name: TestGetMixEffect
 * @tc.desc: Verify function GetMixEffect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHpsBlurUnitTest, TestGetMixEffect, TestSize.Level1)
{
    auto testMixEffect = HpsBlurFilter::GetHpsBlurFilter().GetMixEffect();
    EXPECT_NE(testMixEffect, nullptr);
}

/**
 * @tc.name: TestSetShaderEffect
 * @tc.desc: Verify function SetShaderEffect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHpsBlurUnitTest, TestSetShaderEffect, TestSize.Level1)
{
    Drawing::Brush testBrush;

    Drawing::SamplingOptions linear(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    Drawing::Matrix testMatrix;
    auto testImageCache = std::make_shared<Drawing::Image>();
    const auto testShader = Drawing::ShaderEffect::CreateImageShader(*testImageCache, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, linear, testMatrix);

    bool result = HpsBlurFilter::GetHpsBlurFilter().SetShaderEffect(testBrush, testShader, testImageCache);
    EXPECT_EQ(result, true);
}

/**
 * @tc.name: TestApplyHpsBlur01
 * @tc.desc: Verify function ApplyHpsBlur
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHpsBlurUnitTest, TestApplyHpsBlur01, TestSize.Level1)
{
    Drawing::Canvas testCanvas;
    auto testImage = std::make_shared<Drawing::Image>();
    float radius = 10;
    float saturationForHPS = 1.1;
    float brightnessForHPS = 1.0;
    auto blurParameter = Drawing::HpsBlurParameter(src_, dst_, radius, saturationForHPS, brightnessForHPS);
    float alpha = 0.8;
    Drawing::Brush testBrush;
    auto testColorFilter = testBrush.GetFilter().GetColorFilter();

    HpsBlurFilter testBlurFilter;
    EXPECT_EQ(testBlurFilter.ApplyHpsBlur(testCanvas, testImage, blurParameter, alpha, testColorFilter), false);
}

/**
 * @tc.name: TestApplyHpsBlur02
 * @tc.desc: Verify function ApplyHpsBlur
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHpsBlurUnitTest, TestApplyHpsBlur02, TestSize.Level1)
{
    Drawing::Surface testSurface;
    Drawing::Canvas testCanvas(&testSurface);
    RSPaintFilterCanvas testFilterCanvas(&canvas);

    auto testImage = std::make_shared<Drawing::Image>();
    float radius = 10;
    float saturationForHPS = 1.1;
    float brightnessForHPS = 1.0;
    auto blurParameter = Drawing::HpsBlurParameter(src_, dst_, radius, saturationForHPS, brightnessForHPS);
    float alpha = 0.8;
    Drawing::Brush testBrush;
    auto testColorFilter = testBrush.GetFilter().GetColorFilter();

    HpsBlurFilter testBlurFilter;
    EXPECT_EQ(testBlurFilter.ApplyHpsBlur(testCanvas, testImage, blurParameter, alpha, testColorFilter), false);
}
} // namespace Rosen
} // namespace OHOS