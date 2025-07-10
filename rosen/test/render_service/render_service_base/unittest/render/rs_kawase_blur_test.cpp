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

#include "gtest/gtest.h"

#include "render/rs_kawase_blur.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class KawaseBlurFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void KawaseBlurFilterTest::SetUpTestCase() {}
void KawaseBlurFilterTest::TearDownTestCase() {}
void KawaseBlurFilterTest::SetUp() {}
void KawaseBlurFilterTest::TearDown() {}

/**
 * @tc.name: testInterface
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(KawaseBlurFilterTest, testInterface, TestSize.Level1)
{
    KawaseBlurFilter* kawaseBlurFilter = KawaseBlurFilter::GetKawaseBlurFilter();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect src;
    Drawing::Rect dst;
    KawaseParameter param = KawaseParameter(src, dst, 1);
    std::shared_ptr<Drawing::Image> image;

    EXPECT_FALSE(kawaseBlurFilter->ApplyKawaseBlur(canvas, image, param));
}

/**
 * @tc.name: testInterfaceCanvasMax
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(KawaseBlurFilterTest, testInterfaceCanvasMax, TestSize.Level1)
{
    KawaseBlurFilter* kawaseBlurFilter = KawaseBlurFilter::GetKawaseBlurFilter();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas(INT32_MAX, INT32_MAX);
    Drawing::Rect src(0.f, 0.f, 1024.f, 1024.f);
    Drawing::Rect dst(0.f, 0.f, 1024.f, 1024.f);
    KawaseParameter param = KawaseParameter(src, dst, 1);
    std::shared_ptr<Drawing::Image> image;

    EXPECT_FALSE(kawaseBlurFilter->ApplyKawaseBlur(canvas, image, param));
}

/**
 * @tc.name: testInterfaceCanvasMin
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(KawaseBlurFilterTest, testInterfaceCanvasMin, TestSize.Level1)
{
    KawaseBlurFilter* kawaseBlurFilter = KawaseBlurFilter::GetKawaseBlurFilter();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas(INT32_MIN, INT32_MIN);
    Drawing::Rect src(0.f, 0.f, 1024.f, 1024.f);
    Drawing::Rect dst(0.f, 0.f, 1024.f, 1024.f);
    KawaseParameter param = KawaseParameter(src, dst, 1);
    std::shared_ptr<Drawing::Image> image;

    EXPECT_FALSE(kawaseBlurFilter->ApplyKawaseBlur(canvas, image, param));
}

/**
 * @tc.name: testInterfaceCanvasMinSrcRectInvalid
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(KawaseBlurFilterTest, testInterfaceCanvasMinSrcRectInvalid, TestSize.Level1)
{
    KawaseBlurFilter* kawaseBlurFilter = KawaseBlurFilter::GetKawaseBlurFilter();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas(INT32_MIN, INT32_MIN);
    Drawing::Rect src(0.f, 0.f, 1024.f, 1024.f);
    Drawing::Rect dst(0.f, 0.f, 1024.f, 1024.f);
    KawaseParameter param = KawaseParameter(src, dst, 1);
    std::shared_ptr<Drawing::Image> image;

    EXPECT_FALSE(kawaseBlurFilter->ApplyKawaseBlur(canvas, image, param));
}

/**
 * @tc.name: testInterfaceCanvasMinRectInvalidNO_TYPE
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(KawaseBlurFilterTest, testInterfaceCanvasMinRectInvalidNO_TYPE, TestSize.Level1)
{
    KawaseBlurFilter* kawaseBlurFilter = KawaseBlurFilter::GetKawaseBlurFilter();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas(INT32_MIN, INT32_MIN);
    Drawing::Rect src(FLT_MIN, FLT_MIN, FLT_MAX, FLT_MAX);
    Drawing::Rect dst(FLT_MIN, FLT_MIN, FLT_MAX, FLT_MAX);
    int radius = 5;
    std::shared_ptr<Drawing::ColorFilter> color =
        std::make_shared<Drawing::ColorFilter>(Drawing::ColorFilter::FilterType::NO_TYPE);
    float alpha = 0.f;

    KawaseParameter param = KawaseParameter(src, dst, radius, color, alpha);
    std::shared_ptr<Drawing::Image> image;

    EXPECT_FALSE(kawaseBlurFilter->ApplyKawaseBlur(canvas, image, param));
}

/**
 * @tc.name: testInterfaceNO_TYPE
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(KawaseBlurFilterTest, testInterfaceNO_TYPE, TestSize.Level1)
{
    KawaseBlurFilter* kawaseBlurFilter = KawaseBlurFilter::GetKawaseBlurFilter();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas(5, 5);
    Drawing::Rect src(0, 0, 1024, 1024);
    Drawing::Rect dst(0, 0, 1024, 1024);
    int radius = 5;
    std::shared_ptr<Drawing::ColorFilter> color =
        std::make_shared<Drawing::ColorFilter>(Drawing::ColorFilter::FilterType::NO_TYPE);
    float alpha = 0.f;

    KawaseParameter param = KawaseParameter(src, dst, radius, color, alpha);
    std::shared_ptr<Drawing::Image> image;

    EXPECT_FALSE(kawaseBlurFilter->ApplyKawaseBlur(canvas, image, param));
}

/**
 * @tc.name: testInterfaceBLEND_MODE
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(KawaseBlurFilterTest, testInterfaceBLEND_MODE, TestSize.Level1)
{
    KawaseBlurFilter* kawaseBlurFilter = KawaseBlurFilter::GetKawaseBlurFilter();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas(5, 5);
    Drawing::Rect src(0, 0, 1024, 1024);
    Drawing::Rect dst(0, 0, 1024, 1024);
    int radius = 5;
    std::shared_ptr<Drawing::ColorFilter> color =
        std::make_shared<Drawing::ColorFilter>(Drawing::ColorFilter::FilterType::BLEND_MODE);
    float alpha = 0.f;

    KawaseParameter param = KawaseParameter(src, dst, radius, color, alpha);
    std::shared_ptr<Drawing::Image> image;

    EXPECT_FALSE(kawaseBlurFilter->ApplyKawaseBlur(canvas, image, param));
}

/**
 * @tc.name: testInterfaceMATRIX
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(KawaseBlurFilterTest, testInterfaceMATRIX, TestSize.Level1)
{
    KawaseBlurFilter* kawaseBlurFilter = KawaseBlurFilter::GetKawaseBlurFilter();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas(5, 5);
    Drawing::Rect src(0, 0, 1024, 1024);
    Drawing::Rect dst(0, 0, 1024, 1024);
    int radius = 5;
    std::shared_ptr<Drawing::ColorFilter> color =
        std::make_shared<Drawing::ColorFilter>(Drawing::ColorFilter::FilterType::MATRIX);
    float alpha = 0.f;

    KawaseParameter param = KawaseParameter(src, dst, radius, color, alpha);
    std::shared_ptr<Drawing::Image> image;

    EXPECT_FALSE(kawaseBlurFilter->ApplyKawaseBlur(canvas, image, param));
}

/**
 * @tc.name: testInterfaceLINEAR_TO_SRGB_GAMMA
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(KawaseBlurFilterTest, testInterfaceLINEAR_TO_SRGB_GAMMA, TestSize.Level1)
{
    KawaseBlurFilter* kawaseBlurFilter = KawaseBlurFilter::GetKawaseBlurFilter();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas(5, 5);
    Drawing::Rect src(0, 0, 1024, 1024);
    Drawing::Rect dst(0, 0, 1024, 1024);
    int radius = 5;
    std::shared_ptr<Drawing::ColorFilter> color =
        std::make_shared<Drawing::ColorFilter>(Drawing::ColorFilter::FilterType::LINEAR_TO_SRGB_GAMMA);
    float alpha = 0.f;

    KawaseParameter param = KawaseParameter(src, dst, radius, color, alpha);
    std::shared_ptr<Drawing::Image> image;

    EXPECT_FALSE(kawaseBlurFilter->ApplyKawaseBlur(canvas, image, param));
}

/**
 * @tc.name: testInterfaceSRGB_GAMMA_TO_LINEAR
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(KawaseBlurFilterTest, testInterfaceSRGB_GAMMA_TO_LINEAR, TestSize.Level1)
{
    KawaseBlurFilter* kawaseBlurFilter = KawaseBlurFilter::GetKawaseBlurFilter();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas(5, 5);
    Drawing::Rect src(0, 0, 1024, 1024);
    Drawing::Rect dst(0, 0, 1024, 1024);
    int radius = 5;
    std::shared_ptr<Drawing::ColorFilter> color =
        std::make_shared<Drawing::ColorFilter>(Drawing::ColorFilter::FilterType::SRGB_GAMMA_TO_LINEAR);
    float alpha = 0.f;

    KawaseParameter param = KawaseParameter(src, dst, radius, color, alpha);
    std::shared_ptr<Drawing::Image> image;

    EXPECT_FALSE(kawaseBlurFilter->ApplyKawaseBlur(canvas, image, param));
}

/**
 * @tc.name: testInterfaceCOMPOSE
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(KawaseBlurFilterTest, testInterfaceCOMPOSE, TestSize.Level1)
{
    KawaseBlurFilter* kawaseBlurFilter = KawaseBlurFilter::GetKawaseBlurFilter();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas(5, 5);
    Drawing::Rect src(0, 0, 1024, 1024);
    Drawing::Rect dst(0, 0, 1024, 1024);
    int radius = 5;
    std::shared_ptr<Drawing::ColorFilter> color =
        std::make_shared<Drawing::ColorFilter>(Drawing::ColorFilter::FilterType::COMPOSE);
    float alpha = 0.f;

    KawaseParameter param = KawaseParameter(src, dst, radius, color, alpha);
    std::shared_ptr<Drawing::Image> image;

    EXPECT_FALSE(kawaseBlurFilter->ApplyKawaseBlur(canvas, image, param));
}

/**
 * @tc.name: testInterfaceLUMA
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(KawaseBlurFilterTest, testInterfaceLUMA, TestSize.Level1)
{
    KawaseBlurFilter* kawaseBlurFilter = KawaseBlurFilter::GetKawaseBlurFilter();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas(5, 5);
    Drawing::Rect src(0, 0, 1024, 1024);
    Drawing::Rect dst(0, 0, 1024, 1024);
    int radius = 5;
    std::shared_ptr<Drawing::ColorFilter> color =
        std::make_shared<Drawing::ColorFilter>(Drawing::ColorFilter::FilterType::LUMA);
    float alpha = 0.f;

    KawaseParameter param = KawaseParameter(src, dst, radius, color, alpha);
    std::shared_ptr<Drawing::Image> image;

    EXPECT_FALSE(kawaseBlurFilter->ApplyKawaseBlur(canvas, image, param));
}

/**
 * @tc.name: testInterfaceOVER_DRAW
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(KawaseBlurFilterTest, testInterfaceOVER_DRAW, TestSize.Level1)
{
    KawaseBlurFilter* kawaseBlurFilter = KawaseBlurFilter::GetKawaseBlurFilter();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas(5, 5);
    Drawing::Rect src(0, 0, 1024, 1024);
    Drawing::Rect dst(0, 0, 1024, 1024);
    int radius = 5;
    std::shared_ptr<Drawing::ColorFilter> color =
        std::make_shared<Drawing::ColorFilter>(Drawing::ColorFilter::FilterType::OVER_DRAW);
    float alpha = 0.f;

    KawaseParameter param = KawaseParameter(src, dst, radius, color, alpha);
    std::shared_ptr<Drawing::Image> image;

    EXPECT_FALSE(kawaseBlurFilter->ApplyKawaseBlur(canvas, image, param));
}

/**
 * @tc.name: setupBlurEffectAdvancedFilterTest001
 * @tc.desc: Verify function setupBlurEffectAdvancedFilter
 * @tc.type:FUNC
 */
HWTEST_F(KawaseBlurFilterTest, setupBlurEffectAdvancedFilterTest001, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    kawaseBlurFilter->setupBlurEffectAdvancedFilter();
    EXPECT_NE(kawaseBlurFilter->blurEffectAF_, nullptr);
}

/**
 * @tc.name: GetShaderTransformTest001
 * @tc.desc: Verify function GetShaderTransform
 * @tc.type:FUNC
 */
HWTEST_F(KawaseBlurFilterTest, GetShaderTransformTest001, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas(1, 1);
    Drawing::Rect blurRect;
    kawaseBlurFilter->GetShaderTransform(&canvas, blurRect, 1.0f, 1.0f);
    EXPECT_NE(kawaseBlurFilter, nullptr);
}

/**
 * @tc.name: GetShaderTransformTest001BlurRectMax
 * @tc.desc: Verify function GetShaderTransform
 * @tc.type:FUNC
 */
HWTEST_F(KawaseBlurFilterTest, GetShaderTransformTest001BlurRectMax, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas(1, 1);
    Drawing::Rect blurRect(FLT_MIN, FLT_MIN, FLT_MAX, FLT_MAX);
    kawaseBlurFilter->GetShaderTransform(&canvas, blurRect, 1.0f, 1.0f);
    EXPECT_NE(kawaseBlurFilter, nullptr);
}

/**
 * @tc.name: GetShaderTransformTest001BlurRectInvalid
 * @tc.desc: Verify function GetShaderTransform
 * @tc.type:FUNC
 */
HWTEST_F(KawaseBlurFilterTest, GetShaderTransformTest001BlurRectInvalid, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas(1, 1);
    Drawing::Rect blurRect(FLT_MAX, FLT_MAX, FLT_MIN, FLT_MIN);
    kawaseBlurFilter->GetShaderTransform(&canvas, blurRect, 1.0f, 1.0f);
    EXPECT_NE(kawaseBlurFilter, nullptr);
}

/**
 * @tc.name: GetShaderTransformTest001CanvasMax
 * @tc.desc: Verify function GetShaderTransform
 * @tc.type:FUNC
 */
HWTEST_F(KawaseBlurFilterTest, GetShaderTransformTest001CanvasMax, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas(INT32_MAX, INT32_MAX);
    Drawing::Rect blurRect(0, 0, 2048, 2048);
    kawaseBlurFilter->GetShaderTransform(&canvas, blurRect, 1.0f, 1.0f);
    EXPECT_NE(kawaseBlurFilter, nullptr);
}

/**
 * @tc.name: GetShaderTransformTest001CanvasMin
 * @tc.desc: Verify function GetShaderTransform
 * @tc.type:FUNC
 */
HWTEST_F(KawaseBlurFilterTest, GetShaderTransformTest001CanvasMin, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas(INT32_MIN, INT32_MIN);
    Drawing::Rect blurRect(0, 0, 2048, 2048);
    kawaseBlurFilter->GetShaderTransform(&canvas, blurRect, 1.0f, 1.0f);
    EXPECT_NE(kawaseBlurFilter, nullptr);
}

/**
 * @tc.name: OutputOriginalImageTest001
 * @tc.desc: Verify function OutputOriginalImage
 * @tc.type:FUNC
 */
HWTEST_F(KawaseBlurFilterTest, OutputOriginalImageTest001, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas(1, 1);
    auto image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    Drawing::Rect srcRect;
    Drawing::Rect dstRect;
    auto colorFilter = std::make_shared<Drawing::ColorFilter>();
    ASSERT_NE(colorFilter, nullptr);
    KawaseParameter param(srcRect, dstRect, 1, colorFilter, 0.f);
    kawaseBlurFilter->OutputOriginalImage(canvas, image, param);
    EXPECT_NE(kawaseBlurFilter, nullptr);
}

/**
 * @tc.name: OutputOriginalImageTest001BLEND_MODE
 * @tc.desc: Verify function OutputOriginalImage
 * @tc.type:FUNC
 */
HWTEST_F(KawaseBlurFilterTest, OutputOriginalImageTest001BLEND_MODE, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas(1, 1);
    auto image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    Drawing::Rect srcRect;
    Drawing::Rect dstRect;
    auto colorFilter = std::make_shared<Drawing::ColorFilter>(Drawing::ColorFilter::FilterType::BLEND_MODE);
    ASSERT_NE(colorFilter, nullptr);
    KawaseParameter param(srcRect, dstRect, 1, colorFilter, 0.f);
    kawaseBlurFilter->OutputOriginalImage(canvas, image, param);
    EXPECT_NE(kawaseBlurFilter, nullptr);
}

/**
 * @tc.name: OutputOriginalImageTest001NO_TYPE
 * @tc.desc: Verify function OutputOriginalImage
 * @tc.type:FUNC
 */
HWTEST_F(KawaseBlurFilterTest, OutputOriginalImageTest001NO_TYPE, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas(1, 1);
    auto image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    Drawing::Rect srcRect;
    Drawing::Rect dstRect;
    auto colorFilter = std::make_shared<Drawing::ColorFilter>(Drawing::ColorFilter::FilterType::NO_TYPE);
    ASSERT_NE(colorFilter, nullptr);
    KawaseParameter param(srcRect, dstRect, 1, colorFilter, 0.f);
    kawaseBlurFilter->OutputOriginalImage(canvas, image, param);
    EXPECT_NE(kawaseBlurFilter, nullptr);
}

/**
 * @tc.name: OutputOriginalImageTest001MATRIX
 * @tc.desc: Verify function OutputOriginalImage
 * @tc.type:FUNC
 */
HWTEST_F(KawaseBlurFilterTest, OutputOriginalImageTest001MATRIX, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas(1, 1);
    auto image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    Drawing::Rect srcRect;
    Drawing::Rect dstRect;
    auto colorFilter = std::make_shared<Drawing::ColorFilter>(Drawing::ColorFilter::FilterType::MATRIX);
    ASSERT_NE(colorFilter, nullptr);
    KawaseParameter param(srcRect, dstRect, 1, colorFilter, 0.f);
    kawaseBlurFilter->OutputOriginalImage(canvas, image, param);
    EXPECT_NE(kawaseBlurFilter, nullptr);
}

/**
 * @tc.name: OutputOriginalImageTest001LINEAR_TO_SRGB_GAMMA
 * @tc.desc: Verify function OutputOriginalImage
 * @tc.type:FUNC
 */
HWTEST_F(KawaseBlurFilterTest, OutputOriginalImageTest001LINEAR_TO_SRGB_GAMMA, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas(1, 1);
    auto image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    Drawing::Rect srcRect;
    Drawing::Rect dstRect;
    auto colorFilter = std::make_shared<Drawing::ColorFilter>(Drawing::ColorFilter::FilterType::LINEAR_TO_SRGB_GAMMA);
    ASSERT_NE(colorFilter, nullptr);
    KawaseParameter param(srcRect, dstRect, 1, colorFilter, 0.f);
    kawaseBlurFilter->OutputOriginalImage(canvas, image, param);
    EXPECT_NE(kawaseBlurFilter, nullptr);
}

/**
 * @tc.name: OutputOriginalImageTest001SRGB_GAMMA_TO_LINEAR
 * @tc.desc: Verify function OutputOriginalImage
 * @tc.type:FUNC
 */
HWTEST_F(KawaseBlurFilterTest, OutputOriginalImageTest001SRGB_GAMMA_TO_LINEAR, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas(1, 1);
    auto image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    Drawing::Rect srcRect;
    Drawing::Rect dstRect;
    auto colorFilter = std::make_shared<Drawing::ColorFilter>(Drawing::ColorFilter::FilterType::SRGB_GAMMA_TO_LINEAR);
    ASSERT_NE(colorFilter, nullptr);
    KawaseParameter param(srcRect, dstRect, 1, colorFilter, 0.f);
    kawaseBlurFilter->OutputOriginalImage(canvas, image, param);
    EXPECT_NE(kawaseBlurFilter, nullptr);
}

/**
 * @tc.name: OutputOriginalImageTest001COMPOSE
 * @tc.desc: Verify function OutputOriginalImage
 * @tc.type:FUNC
 */
HWTEST_F(KawaseBlurFilterTest, OutputOriginalImageTest001COMPOSE, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas(1, 1);
    auto image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    Drawing::Rect srcRect;
    Drawing::Rect dstRect;
    auto colorFilter = std::make_shared<Drawing::ColorFilter>(Drawing::ColorFilter::FilterType::COMPOSE);
    ASSERT_NE(colorFilter, nullptr);
    KawaseParameter param(srcRect, dstRect, 1, colorFilter, 0.f);
    kawaseBlurFilter->OutputOriginalImage(canvas, image, param);
    EXPECT_NE(kawaseBlurFilter, nullptr);
}

/**
 * @tc.name: OutputOriginalImageTest001LUMA
 * @tc.desc: Verify function OutputOriginalImage
 * @tc.type:FUNC
 */
HWTEST_F(KawaseBlurFilterTest, OutputOriginalImageTest001LUMA, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas(1, 1);
    auto image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    Drawing::Rect srcRect;
    Drawing::Rect dstRect;
    auto colorFilter = std::make_shared<Drawing::ColorFilter>(Drawing::ColorFilter::FilterType::LUMA);
    ASSERT_NE(colorFilter, nullptr);
    KawaseParameter param(srcRect, dstRect, 1, colorFilter, 0.f);
    kawaseBlurFilter->OutputOriginalImage(canvas, image, param);
    EXPECT_NE(kawaseBlurFilter, nullptr);
}

/**
 * @tc.name: OutputOriginalImageTest001OVER_DRAW
 * @tc.desc: Verify function OutputOriginalImage
 * @tc.type:FUNC
 */
HWTEST_F(KawaseBlurFilterTest, OutputOriginalImageTest001OVER_DRAW, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas(1, 1);
    auto image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    Drawing::Rect srcRect;
    Drawing::Rect dstRect;
    auto colorFilter = std::make_shared<Drawing::ColorFilter>(Drawing::ColorFilter::FilterType::OVER_DRAW);
    ASSERT_NE(colorFilter, nullptr);
    KawaseParameter param(srcRect, dstRect, 1, colorFilter, 0.f);
    kawaseBlurFilter->OutputOriginalImage(canvas, image, param);
    EXPECT_NE(kawaseBlurFilter, nullptr);
}

/**
 * @tc.name: ApplyBlurTest001
 * @tc.desc: Verify function ApplyBlur
 * @tc.type:FUNC
 */
HWTEST_F(KawaseBlurFilterTest, ApplyBlurTest001, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas(1, 1);
    auto image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    auto blurImage = std::make_shared<Drawing::Image>();
    ASSERT_NE(blurImage, nullptr);
    Drawing::Rect srcRect;
    Drawing::Rect dstRect;
    auto colorFilter = std::make_shared<Drawing::ColorFilter>();
    ASSERT_NE(colorFilter, nullptr);
    KawaseParameter param(srcRect, dstRect, 1, colorFilter, 0.f);
    EXPECT_TRUE(kawaseBlurFilter->ApplyBlur(canvas, image, blurImage, param));
}

/**
 * @tc.name: ApplyBlurTest001NO_TYPE
 * @tc.desc: Verify function ApplyBlur
 * @tc.type:FUNC
 */
HWTEST_F(KawaseBlurFilterTest, ApplyBlurTest001NO_TYPE, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas(1, 1);
    auto image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    auto blurImage = std::make_shared<Drawing::Image>();
    ASSERT_NE(blurImage, nullptr);
    Drawing::Rect srcRect;
    Drawing::Rect dstRect;
    auto colorFilter = std::make_shared<Drawing::ColorFilter>(Drawing::ColorFilter::FilterType::NO_TYPE);
    ASSERT_NE(colorFilter, nullptr);
    KawaseParameter param(srcRect, dstRect, 1, colorFilter, 0.f);
    EXPECT_TRUE(kawaseBlurFilter->ApplyBlur(canvas, image, blurImage, param));
}

/**
 * @tc.name: ApplyBlurTest001BLEND_MODE
 * @tc.desc: Verify function ApplyBlur
 * @tc.type:FUNC
 */
HWTEST_F(KawaseBlurFilterTest, ApplyBlurTest001BLEND_MODE, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas(1, 1);
    auto image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    auto blurImage = std::make_shared<Drawing::Image>();
    ASSERT_NE(blurImage, nullptr);
    Drawing::Rect srcRect;
    Drawing::Rect dstRect;
    auto colorFilter = std::make_shared<Drawing::ColorFilter>(Drawing::ColorFilter::FilterType::BLEND_MODE);
    ASSERT_NE(colorFilter, nullptr);
    KawaseParameter param(srcRect, dstRect, 1, colorFilter, 0.f);
    EXPECT_TRUE(kawaseBlurFilter->ApplyBlur(canvas, image, blurImage, param));
}

/**
 * @tc.name: ApplyBlurTest001MATRIX
 * @tc.desc: Verify function ApplyBlur
 * @tc.type:FUNC
 */
HWTEST_F(KawaseBlurFilterTest, ApplyBlurTest001MATRIX, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas(1, 1);
    auto image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    auto blurImage = std::make_shared<Drawing::Image>();
    ASSERT_NE(blurImage, nullptr);
    Drawing::Rect srcRect;
    Drawing::Rect dstRect;
    auto colorFilter = std::make_shared<Drawing::ColorFilter>(Drawing::ColorFilter::FilterType::MATRIX);
    ASSERT_NE(colorFilter, nullptr);
    KawaseParameter param(srcRect, dstRect, 1, colorFilter, 0.f);
    EXPECT_TRUE(kawaseBlurFilter->ApplyBlur(canvas, image, blurImage, param));
}

/**
 * @tc.name: ApplyBlurTest001LINEAR_TO_SRGB_GAMMA
 * @tc.desc: Verify function ApplyBlur
 * @tc.type:FUNC
 */
HWTEST_F(KawaseBlurFilterTest, ApplyBlurTest001LINEAR_TO_SRGB_GAMMA, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas(1, 1);
    auto image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    auto blurImage = std::make_shared<Drawing::Image>();
    ASSERT_NE(blurImage, nullptr);
    Drawing::Rect srcRect;
    Drawing::Rect dstRect;
    auto colorFilter = std::make_shared<Drawing::ColorFilter>(Drawing::ColorFilter::FilterType::LINEAR_TO_SRGB_GAMMA);
    ASSERT_NE(colorFilter, nullptr);
    KawaseParameter param(srcRect, dstRect, 1, colorFilter, 0.f);
    EXPECT_TRUE(kawaseBlurFilter->ApplyBlur(canvas, image, blurImage, param));
}

/**
 * @tc.name: ApplyBlurTest001SRGB_GAMMA_TO_LINEAR
 * @tc.desc: Verify function ApplyBlur
 * @tc.type:FUNC
 */
HWTEST_F(KawaseBlurFilterTest, ApplyBlurTest001SRGB_GAMMA_TO_LINEAR, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas(1, 1);
    auto image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    auto blurImage = std::make_shared<Drawing::Image>();
    ASSERT_NE(blurImage, nullptr);
    Drawing::Rect srcRect;
    Drawing::Rect dstRect;
    auto colorFilter = std::make_shared<Drawing::ColorFilter>(Drawing::ColorFilter::FilterType::SRGB_GAMMA_TO_LINEAR);
    ASSERT_NE(colorFilter, nullptr);
    KawaseParameter param(srcRect, dstRect, 1, colorFilter, 0.f);
    EXPECT_TRUE(kawaseBlurFilter->ApplyBlur(canvas, image, blurImage, param));
}

/**
 * @tc.name: ApplyBlurTest001COMPOSE
 * @tc.desc: Verify function ApplyBlur
 * @tc.type:FUNC
 */
HWTEST_F(KawaseBlurFilterTest, ApplyBlurTest001COMPOSE, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas(1, 1);
    auto image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    auto blurImage = std::make_shared<Drawing::Image>();
    ASSERT_NE(blurImage, nullptr);
    Drawing::Rect srcRect;
    Drawing::Rect dstRect;
    auto colorFilter = std::make_shared<Drawing::ColorFilter>(Drawing::ColorFilter::FilterType::COMPOSE);
    ASSERT_NE(colorFilter, nullptr);
    KawaseParameter param(srcRect, dstRect, 1, colorFilter, 0.f);
    EXPECT_TRUE(kawaseBlurFilter->ApplyBlur(canvas, image, blurImage, param));
}

/**
 * @tc.name: ApplyBlurTest001LUMA
 * @tc.desc: Verify function ApplyBlur
 * @tc.type:FUNC
 */
HWTEST_F(KawaseBlurFilterTest, ApplyBlurTest001LUMA, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas(1, 1);
    auto image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    auto blurImage = std::make_shared<Drawing::Image>();
    ASSERT_NE(blurImage, nullptr);
    Drawing::Rect srcRect;
    Drawing::Rect dstRect;
    auto colorFilter = std::make_shared<Drawing::ColorFilter>(Drawing::ColorFilter::FilterType::LUMA);
    ASSERT_NE(colorFilter, nullptr);
    KawaseParameter param(srcRect, dstRect, 1, colorFilter, 0.f);
    EXPECT_TRUE(kawaseBlurFilter->ApplyBlur(canvas, image, blurImage, param));
}

/**
 * @tc.name: ApplyBlurTest001OVER_DRAW
 * @tc.desc: Verify function ApplyBlur
 * @tc.type:FUNC
 */
HWTEST_F(KawaseBlurFilterTest, ApplyBlurTest001OVER_DRAW, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas(1, 1);
    auto image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    auto blurImage = std::make_shared<Drawing::Image>();
    ASSERT_NE(blurImage, nullptr);
    Drawing::Rect srcRect;
    Drawing::Rect dstRect;
    auto colorFilter = std::make_shared<Drawing::ColorFilter>(Drawing::ColorFilter::FilterType::OVER_DRAW);
    ASSERT_NE(colorFilter, nullptr);
    KawaseParameter param(srcRect, dstRect, 1, colorFilter, 0.f);
    EXPECT_TRUE(kawaseBlurFilter->ApplyBlur(canvas, image, blurImage, param));
}

/**
 * @tc.name: ApplyBlurTest002
 * @tc.desc: Verify function ApplyBlur
 * @tc.type:FUNC
 */
HWTEST_F(KawaseBlurFilterTest, ApplyBlurTest002, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas(1, 1);
    auto blurImage = std::make_shared<Drawing::Image>();
    ASSERT_NE(blurImage, nullptr);
    Drawing::Rect srcRect;
    Drawing::Rect dstRect;
    auto colorFilter = std::make_shared<Drawing::ColorFilter>();
    ASSERT_NE(colorFilter, nullptr);
    KawaseParameter param(srcRect, dstRect, 1, colorFilter, 0.f);
    EXPECT_TRUE(!kawaseBlurFilter->ApplyBlur(canvas, nullptr, blurImage, param));
}

/**
 * @tc.name: AdjustRadiusAndScaleTest001
 * @tc.desc: Verify function AdjustRadiusAndScale
 * @tc.type:FUNC
 */
HWTEST_F(KawaseBlurFilterTest, AdjustRadiusAndScaleTest001, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    kawaseBlurFilter->ComputeRadiusAndScale(101);
    EXPECT_EQ(kawaseBlurFilter->blurScale_, 0.0625f);
    kawaseBlurFilter->ComputeRadiusAndScale(40);
    EXPECT_EQ(kawaseBlurFilter->blurScale_, 0.125f);
    kawaseBlurFilter->ComputeRadiusAndScale(20);
    EXPECT_EQ(kawaseBlurFilter->blurScale_, 0.25f);
    kawaseBlurFilter->ComputeRadiusAndScale(1);
    EXPECT_EQ(kawaseBlurFilter->blurScale_, 0.5F);
}

/**
 * @tc.name: AdjustRadiusAndScaleTest001Invalid
 * @tc.desc: Verify function AdjustRadiusAndScale
 * @tc.type:FUNC
 */
HWTEST_F(KawaseBlurFilterTest, AdjustRadiusAndScaleTest001Invalid, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    kawaseBlurFilter->ComputeRadiusAndScale(INT8_MAX);
    EXPECT_EQ(kawaseBlurFilter->blurScale_, 0.0625f);
    kawaseBlurFilter->ComputeRadiusAndScale(INT8_MIN);
    EXPECT_NE(kawaseBlurFilter->blurScale_, 0.125f);
}

/**
 * @tc.name: GetDescriptionTest001
 * @tc.desc: Verify function GetDescription
 * @tc.type:FUNC
 */
HWTEST_F(KawaseBlurFilterTest, GetDescriptionTest001, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    EXPECT_EQ(kawaseBlurFilter->GetDescription(), "blur radius is 0.000000");
}

/**
 * @tc.name: CheckInputImageTest
 * @tc.desc: Verify function CheckInputImage
 * @tc.type:FUNC
 * @tc.require: issueI9TOXM
 */
HWTEST_F(KawaseBlurFilterTest, CheckInputImageTest, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas(1, 1);
    auto image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    auto checkedImage = std::make_shared<Drawing::Image>();
    ASSERT_NE(checkedImage, nullptr);
    Drawing::Rect srcRect;
    Drawing::Rect dstRect;
    auto colorFilter = std::make_shared<Drawing::ColorFilter>();
    ASSERT_NE(colorFilter, nullptr);
    KawaseParameter param(srcRect, dstRect, 1, colorFilter, 0.f);
    kawaseBlurFilter->CheckInputImage(canvas, image, param, checkedImage);
    EXPECT_EQ(param.src.GetBottom(), 0);
}

/**
 * @tc.name: CheckInputImageTestNO_TYPE
 * @tc.desc: Verify function CheckInputImage
 * @tc.type:FUNC
 * @tc.require: issueI9TOXM
 */
HWTEST_F(KawaseBlurFilterTest, CheckInputImageTestNO_TYPE, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas(1, 1);
    auto image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    auto checkedImage = std::make_shared<Drawing::Image>();
    ASSERT_NE(checkedImage, nullptr);
    Drawing::Rect srcRect;
    Drawing::Rect dstRect;
    auto colorFilter = std::make_shared<Drawing::ColorFilter>(Drawing::ColorFilter::FilterType::NO_TYPE);
    ASSERT_NE(colorFilter, nullptr);
    KawaseParameter param(srcRect, dstRect, 1, colorFilter, 0.f);
    kawaseBlurFilter->CheckInputImage(canvas, image, param, checkedImage);
    EXPECT_EQ(param.src.GetBottom(), 0);
}

/**
 * @tc.name: CheckInputImageTestBLEND_MODE
 * @tc.desc: Verify function CheckInputImage
 * @tc.type:FUNC
 * @tc.require: issueI9TOXM
 */
HWTEST_F(KawaseBlurFilterTest, CheckInputImageTestBLEND_MODE, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas(1, 1);
    auto image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    auto checkedImage = std::make_shared<Drawing::Image>();
    ASSERT_NE(checkedImage, nullptr);
    Drawing::Rect srcRect;
    Drawing::Rect dstRect;
    auto colorFilter = std::make_shared<Drawing::ColorFilter>(Drawing::ColorFilter::FilterType::BLEND_MODE);
    ASSERT_NE(colorFilter, nullptr);
    KawaseParameter param(srcRect, dstRect, 1, colorFilter, 0.f);
    kawaseBlurFilter->CheckInputImage(canvas, image, param, checkedImage);
    EXPECT_EQ(param.src.GetBottom(), 0);
}

/**
 * @tc.name: CheckInputImageTestMATRIX
 * @tc.desc: Verify function CheckInputImage
 * @tc.type:FUNC
 * @tc.require: issueI9TOXM
 */
HWTEST_F(KawaseBlurFilterTest, CheckInputImageTestMATRIX, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas(1, 1);
    auto image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    auto checkedImage = std::make_shared<Drawing::Image>();
    ASSERT_NE(checkedImage, nullptr);
    Drawing::Rect srcRect;
    Drawing::Rect dstRect;
    auto colorFilter = std::make_shared<Drawing::ColorFilter>(Drawing::ColorFilter::FilterType::MATRIX);
    ASSERT_NE(colorFilter, nullptr);
    KawaseParameter param(srcRect, dstRect, 1, colorFilter, 0.f);
    kawaseBlurFilter->CheckInputImage(canvas, image, param, checkedImage);
    EXPECT_EQ(param.src.GetBottom(), 0);
}

/**
 * @tc.name: CheckInputImageTestLINEAR_TO_SRGB_GAMMA
 * @tc.desc: Verify function CheckInputImage
 * @tc.type:FUNC
 * @tc.require: issueI9TOXM
 */
HWTEST_F(KawaseBlurFilterTest, CheckInputImageTestLINEAR_TO_SRGB_GAMMA, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas(1, 1);
    auto image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    auto checkedImage = std::make_shared<Drawing::Image>();
    ASSERT_NE(checkedImage, nullptr);
    Drawing::Rect srcRect;
    Drawing::Rect dstRect;
    auto colorFilter = std::make_shared<Drawing::ColorFilter>(Drawing::ColorFilter::FilterType::LINEAR_TO_SRGB_GAMMA);
    ASSERT_NE(colorFilter, nullptr);
    KawaseParameter param(srcRect, dstRect, 1, colorFilter, 0.f);
    kawaseBlurFilter->CheckInputImage(canvas, image, param, checkedImage);
    EXPECT_EQ(param.src.GetBottom(), 0);
}

/**
 * @tc.name: CheckInputImageTestSRGB_GAMMA_TO_LINEAR
 * @tc.desc: Verify function CheckInputImage
 * @tc.type:FUNC
 * @tc.require: issueI9TOXM
 */
HWTEST_F(KawaseBlurFilterTest, CheckInputImageTestSRGB_GAMMA_TO_LINEAR, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas(1, 1);
    auto image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    auto checkedImage = std::make_shared<Drawing::Image>();
    ASSERT_NE(checkedImage, nullptr);
    Drawing::Rect srcRect;
    Drawing::Rect dstRect;
    auto colorFilter = std::make_shared<Drawing::ColorFilter>(Drawing::ColorFilter::FilterType::SRGB_GAMMA_TO_LINEAR);
    ASSERT_NE(colorFilter, nullptr);
    KawaseParameter param(srcRect, dstRect, 1, colorFilter, 0.f);
    kawaseBlurFilter->CheckInputImage(canvas, image, param, checkedImage);
    EXPECT_EQ(param.src.GetBottom(), 0);
}

/**
 * @tc.name: CheckInputImageTestCOMPOSE
 * @tc.desc: Verify function CheckInputImage
 * @tc.type:FUNC
 * @tc.require: issueI9TOXM
 */
HWTEST_F(KawaseBlurFilterTest, CheckInputImageTestCOMPOSE, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas(1, 1);
    auto image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    auto checkedImage = std::make_shared<Drawing::Image>();
    ASSERT_NE(checkedImage, nullptr);
    Drawing::Rect srcRect;
    Drawing::Rect dstRect;
    auto colorFilter = std::make_shared<Drawing::ColorFilter>(Drawing::ColorFilter::FilterType::COMPOSE);
    ASSERT_NE(colorFilter, nullptr);
    KawaseParameter param(srcRect, dstRect, 1, colorFilter, 0.f);
    kawaseBlurFilter->CheckInputImage(canvas, image, param, checkedImage);
    EXPECT_EQ(param.src.GetBottom(), 0);
}

/**
 * @tc.name: CheckInputImageTestLUMA
 * @tc.desc: Verify function CheckInputImage
 * @tc.type:FUNC
 * @tc.require: issueI9TOXM
 */
HWTEST_F(KawaseBlurFilterTest, CheckInputImageTestLUMA, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas(1, 1);
    auto image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    auto checkedImage = std::make_shared<Drawing::Image>();
    ASSERT_NE(checkedImage, nullptr);
    Drawing::Rect srcRect;
    Drawing::Rect dstRect;
    auto colorFilter = std::make_shared<Drawing::ColorFilter>(Drawing::ColorFilter::FilterType::LUMA);
    ASSERT_NE(colorFilter, nullptr);
    KawaseParameter param(srcRect, dstRect, 1, colorFilter, 0.f);
    kawaseBlurFilter->CheckInputImage(canvas, image, param, checkedImage);
    EXPECT_EQ(param.src.GetBottom(), 0);
}

/**
 * @tc.name: CheckInputImageTestOVER_DRAW
 * @tc.desc: Verify function CheckInputImage
 * @tc.type:FUNC
 * @tc.require: issueI9TOXM
 */
HWTEST_F(KawaseBlurFilterTest, CheckInputImageTestOVER_DRAW, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas(1, 1);
    auto image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    auto checkedImage = std::make_shared<Drawing::Image>();
    ASSERT_NE(checkedImage, nullptr);
    Drawing::Rect srcRect;
    Drawing::Rect dstRect;
    auto colorFilter = std::make_shared<Drawing::ColorFilter>(Drawing::ColorFilter::FilterType::OVER_DRAW);
    ASSERT_NE(colorFilter, nullptr);
    KawaseParameter param(srcRect, dstRect, 1, colorFilter, 0.f);
    kawaseBlurFilter->CheckInputImage(canvas, image, param, checkedImage);
    EXPECT_EQ(param.src.GetBottom(), 0);
}

/**
 * @tc.name: ExecutePingPongBlurTest
 * @tc.desc: Verify function ExecutePingPongBlur
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(KawaseBlurFilterTest, ExecutePingPongBlurTest, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas;
    auto input = std::make_shared<Drawing::Image>();
    ASSERT_NE(input, nullptr);
    Drawing::Rect srcRect;
    Drawing::Rect dstRect;
    auto colorFilter = std::make_shared<Drawing::ColorFilter>();
    ASSERT_NE(colorFilter, nullptr);
    KawaseParameter inParam(srcRect, dstRect, 1, colorFilter, 0.f);
    KawaseBlurFilter::BlurParams blur;
    kawaseBlurFilter->ExecutePingPongBlur(canvas, input, inParam, blur);
    kawaseBlurFilter->SetupSimpleFilter();
    kawaseBlurFilter->ExecutePingPongBlur(canvas, input, inParam, blur);
    EXPECT_EQ(kawaseBlurFilter->blurEffectAF_, nullptr);
}

/**
 * @tc.name: ExecutePingPongBlurTestNO_TYPE
 * @tc.desc: Verify function ExecutePingPongBlur
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(KawaseBlurFilterTest, ExecutePingPongBlurTestNO_TYPE, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas;
    auto input = std::make_shared<Drawing::Image>();
    ASSERT_NE(input, nullptr);
    Drawing::Rect srcRect;
    Drawing::Rect dstRect;
    auto colorFilter = std::make_shared<Drawing::ColorFilter>(Drawing::ColorFilter::FilterType::NO_TYPE);
    ASSERT_NE(colorFilter, nullptr);
    KawaseParameter inParam(srcRect, dstRect, 1, colorFilter, 0.f);
    KawaseBlurFilter::BlurParams blur;
    kawaseBlurFilter->ExecutePingPongBlur(canvas, input, inParam, blur);
    kawaseBlurFilter->SetupSimpleFilter();
    kawaseBlurFilter->ExecutePingPongBlur(canvas, input, inParam, blur);
    EXPECT_EQ(kawaseBlurFilter->blurEffectAF_, nullptr);
}

/**
 * @tc.name: ExecutePingPongBlurTestBLEND_MODE
 * @tc.desc: Verify function ExecutePingPongBlur
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(KawaseBlurFilterTest, ExecutePingPongBlurTestBLEND_MODE, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas;
    auto input = std::make_shared<Drawing::Image>();
    ASSERT_NE(input, nullptr);
    Drawing::Rect srcRect;
    Drawing::Rect dstRect;
    auto colorFilter = std::make_shared<Drawing::ColorFilter>(Drawing::ColorFilter::FilterType::BLEND_MODE);
    ASSERT_NE(colorFilter, nullptr);
    KawaseParameter inParam(srcRect, dstRect, 1, colorFilter, 0.f);
    KawaseBlurFilter::BlurParams blur;
    kawaseBlurFilter->ExecutePingPongBlur(canvas, input, inParam, blur);
    kawaseBlurFilter->SetupSimpleFilter();
    kawaseBlurFilter->ExecutePingPongBlur(canvas, input, inParam, blur);
    EXPECT_EQ(kawaseBlurFilter->blurEffectAF_, nullptr);
}

/**
 * @tc.name: ExecutePingPongBlurTestMATRIX
 * @tc.desc: Verify function ExecutePingPongBlur
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(KawaseBlurFilterTest, ExecutePingPongBlurTestMATRIX, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas;
    auto input = std::make_shared<Drawing::Image>();
    ASSERT_NE(input, nullptr);
    Drawing::Rect srcRect;
    Drawing::Rect dstRect;
    auto colorFilter = std::make_shared<Drawing::ColorFilter>(Drawing::ColorFilter::FilterType::MATRIX);
    ASSERT_NE(colorFilter, nullptr);
    KawaseParameter inParam(srcRect, dstRect, 1, colorFilter, 0.f);
    KawaseBlurFilter::BlurParams blur;
    kawaseBlurFilter->ExecutePingPongBlur(canvas, input, inParam, blur);
    kawaseBlurFilter->SetupSimpleFilter();
    kawaseBlurFilter->ExecutePingPongBlur(canvas, input, inParam, blur);
    EXPECT_EQ(kawaseBlurFilter->blurEffectAF_, nullptr);
}

/**
 * @tc.name: ExecutePingPongBlurTestLINEAR_TO_SRGB_GAMMA
 * @tc.desc: Verify function ExecutePingPongBlur
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(KawaseBlurFilterTest, ExecutePingPongBlurTestLINEAR_TO_SRGB_GAMMA, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas;
    auto input = std::make_shared<Drawing::Image>();
    ASSERT_NE(input, nullptr);
    Drawing::Rect srcRect;
    Drawing::Rect dstRect;
    auto colorFilter = std::make_shared<Drawing::ColorFilter>(Drawing::ColorFilter::FilterType::LINEAR_TO_SRGB_GAMMA);
    ASSERT_NE(colorFilter, nullptr);
    KawaseParameter inParam(srcRect, dstRect, 1, colorFilter, 0.f);
    KawaseBlurFilter::BlurParams blur;
    kawaseBlurFilter->ExecutePingPongBlur(canvas, input, inParam, blur);
    kawaseBlurFilter->SetupSimpleFilter();
    kawaseBlurFilter->ExecutePingPongBlur(canvas, input, inParam, blur);
    EXPECT_EQ(kawaseBlurFilter->blurEffectAF_, nullptr);
}

/**
 * @tc.name: ExecutePingPongBlurTestSRGB_GAMMA_TO_LINEAR
 * @tc.desc: Verify function ExecutePingPongBlur
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(KawaseBlurFilterTest, ExecutePingPongBlurTestSRGB_GAMMA_TO_LINEAR, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas;
    auto input = std::make_shared<Drawing::Image>();
    ASSERT_NE(input, nullptr);
    Drawing::Rect srcRect;
    Drawing::Rect dstRect;
    auto colorFilter = std::make_shared<Drawing::ColorFilter>(Drawing::ColorFilter::FilterType::SRGB_GAMMA_TO_LINEAR);
    ASSERT_NE(colorFilter, nullptr);
    KawaseParameter inParam(srcRect, dstRect, 1, colorFilter, 0.f);
    KawaseBlurFilter::BlurParams blur;
    kawaseBlurFilter->ExecutePingPongBlur(canvas, input, inParam, blur);
    kawaseBlurFilter->SetupSimpleFilter();
    kawaseBlurFilter->ExecutePingPongBlur(canvas, input, inParam, blur);
    EXPECT_EQ(kawaseBlurFilter->blurEffectAF_, nullptr);
}

/**
 * @tc.name: ExecutePingPongBlurTestCOMPOSE
 * @tc.desc: Verify function ExecutePingPongBlur
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(KawaseBlurFilterTest, ExecutePingPongBlurTestCOMPOSE, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas;
    auto input = std::make_shared<Drawing::Image>();
    ASSERT_NE(input, nullptr);
    Drawing::Rect srcRect;
    Drawing::Rect dstRect;
    auto colorFilter = std::make_shared<Drawing::ColorFilter>(Drawing::ColorFilter::FilterType::COMPOSE);
    ASSERT_NE(colorFilter, nullptr);
    KawaseParameter inParam(srcRect, dstRect, 1, colorFilter, 0.f);
    KawaseBlurFilter::BlurParams blur;
    kawaseBlurFilter->ExecutePingPongBlur(canvas, input, inParam, blur);
    kawaseBlurFilter->SetupSimpleFilter();
    kawaseBlurFilter->ExecutePingPongBlur(canvas, input, inParam, blur);
    EXPECT_EQ(kawaseBlurFilter->blurEffectAF_, nullptr);
}

/**
 * @tc.name: ExecutePingPongBlurTestLUMA
 * @tc.desc: Verify function ExecutePingPongBlur
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(KawaseBlurFilterTest, ExecutePingPongBlurTestLUMA, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas;
    auto input = std::make_shared<Drawing::Image>();
    ASSERT_NE(input, nullptr);
    Drawing::Rect srcRect;
    Drawing::Rect dstRect;
    auto colorFilter = std::make_shared<Drawing::ColorFilter>(Drawing::ColorFilter::FilterType::LUMA);
    ASSERT_NE(colorFilter, nullptr);
    KawaseParameter inParam(srcRect, dstRect, 1, colorFilter, 0.f);
    KawaseBlurFilter::BlurParams blur;
    kawaseBlurFilter->ExecutePingPongBlur(canvas, input, inParam, blur);
    kawaseBlurFilter->SetupSimpleFilter();
    kawaseBlurFilter->ExecutePingPongBlur(canvas, input, inParam, blur);
    EXPECT_EQ(kawaseBlurFilter->blurEffectAF_, nullptr);
}

/**
 * @tc.name: ExecutePingPongBlurTestOVER_DRAW
 * @tc.desc: Verify function ExecutePingPongBlur
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(KawaseBlurFilterTest, ExecutePingPongBlurTestOVER_DRAW, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    ASSERT_NE(kawaseBlurFilter, nullptr);
    Drawing::Canvas canvas;
    auto input = std::make_shared<Drawing::Image>();
    ASSERT_NE(input, nullptr);
    Drawing::Rect srcRect;
    Drawing::Rect dstRect;
    auto colorFilter = std::make_shared<Drawing::ColorFilter>(Drawing::ColorFilter::FilterType::OVER_DRAW);
    ASSERT_NE(colorFilter, nullptr);
    KawaseParameter inParam(srcRect, dstRect, 1, colorFilter, 0.f);
    KawaseBlurFilter::BlurParams blur;
    kawaseBlurFilter->ExecutePingPongBlur(canvas, input, inParam, blur);
    kawaseBlurFilter->SetupSimpleFilter();
    kawaseBlurFilter->ExecutePingPongBlur(canvas, input, inParam, blur);
    EXPECT_EQ(kawaseBlurFilter->blurEffectAF_, nullptr);
}
/**
 * @tc.name: ApplySimpleFilterTest
 * @tc.desc: Verify function ApplySimpleFilter
 * @tc.type: FUNC
 * @tc.require: #ICEF7K
 */
HWTEST_F(KawaseBlurFilterTest, ApplySimpleFilterTest, TestSize.Level1)
{
    auto kawaseBlurFilter = std::make_shared<KawaseBlurFilter>();
    Drawing::Canvas canvas;
    auto input = std::make_shared<Drawing::Image>();
    Drawing::Matrix blurMatrix;
    blurMatrix.Translate(-1, -1);
    blurMatrix.PostScale(1, 1);

    auto scaledInfo = Drawing::ImageInfo(100, 200, Drawing::COLORTYPE_ALPHA_8, Drawing::ALPHATYPE_OPAQUE, nullptr);
    Drawing::SamplingOptions linear(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    auto shaderEffect = kawaseBlurFilter->ApplySimpleFilter(canvas, input, blurMatrix, scaledInfo, linear);
    EXPECT_EQ(shaderEffect, nullptr);
}
} // namespace Rosen
} // namespace OHOS
