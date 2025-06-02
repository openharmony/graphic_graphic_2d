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

#include "pipeline/rs_paint_filter_canvas.h"
#include "render/rs_drawing_filter.h"
#include "render/rs_kawase_blur_shader_filter.h"
#include "render/rs_linear_gradient_blur_shader_filter.h"
#include "render/rs_maskcolor_shader_filter.h"
#include "render/rs_mesa_blur_shader_filter.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class RSDrawingFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSDrawingFilterTest::SetUpTestCase() {}
void RSDrawingFilterTest::TearDownTestCase() {}
void RSDrawingFilterTest::SetUp() {}
void RSDrawingFilterTest::TearDown() {}

/**
 * @tc.name: RSDrawingFilter001
 * @tc.desc: test results of RSDrawingFilter
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(RSDrawingFilterTest, RSDrawingFilter001, TestSize.Level1)
{
    auto imageFilter = std::make_shared<Drawing::ImageFilter>();
    auto filterPtr = std::make_shared<RSShaderFilter>();
    std::vector<std::shared_ptr<RSShaderFilter>> shaderFilters;
    shaderFilters.push_back(filterPtr);
    uint32_t hash = 1;
    RSDrawingFilter drawingFilter(imageFilter, shaderFilters, hash);
    EXPECT_TRUE(imageFilter != nullptr);

    RSDrawingFilter drawingFilter1(imageFilter, filterPtr, hash);
    EXPECT_TRUE(imageFilter != nullptr);
}

/**
 * @tc.name: RSDrawingFilter002
 * @tc.desc: shaderFilters is empty
 * @tc.type: FUNC
 */
HWTEST_F(RSDrawingFilterTest, RSDrawingFilter002, TestSize.Level1)
{
    auto imageFilter = std::make_shared<Drawing::ImageFilter>();
    auto filterPtr = std::make_shared<RSShaderFilter>();
    std::vector<std::shared_ptr<RSShaderFilter>> shaderFilters;
    uint32_t hash = 1;
    RSDrawingFilter drawingFilter(imageFilter, shaderFilters, hash);
    EXPECT_TRUE(imageFilter != nullptr);

    RSDrawingFilter drawingFilter1(imageFilter, filterPtr, hash);
    EXPECT_TRUE(imageFilter != nullptr);
}

/**
 * @tc.name: SetImageFilter001
 * @tc.desc: test results of SetImageFilter
 * @tc.type: FUNC
 */
HWTEST_F(RSDrawingFilterTest, SetImageFilter001, TestSize.Level1)
{
    auto imageFilter = std::make_shared<Drawing::ImageFilter>();
    auto filterPtr = std::make_shared<RSShaderFilter>();
    std::vector<std::shared_ptr<RSShaderFilter>> shaderFilters;
    shaderFilters.push_back(filterPtr);
    uint32_t hash = 1;
    RSDrawingFilter drawingFilter(imageFilter, shaderFilters, hash);
    drawingFilter.SetImageFilter(imageFilter);
    EXPECT_TRUE(imageFilter != nullptr);
}
 
/**
 * @tc.name: SetImageFilter002
 * @tc.desc: test results of SetImageFilter. shaderFilters is empty
 * @tc.type: FUNC
 */
HWTEST_F(RSDrawingFilterTest, SetImageFilter002, TestSize.Level1)
{
    auto imageFilter = std::make_shared<Drawing::ImageFilter>();
    auto filterPtr = std::make_shared<RSShaderFilter>();
    std::vector<std::shared_ptr<RSShaderFilter>> shaderFilters;

    uint32_t hash = 1;
    RSDrawingFilter drawingFilter(imageFilter, shaderFilters, hash);
    drawingFilter.SetImageFilter(imageFilter);
    EXPECT_TRUE(imageFilter != nullptr);
}


/**
 * @tc.name: ProcessImageFilter001
 * @tc.desc: test results of ProcessImageFilter
 * @tc.type: FUNC
 */
HWTEST_F(RSDrawingFilterTest, ProcessImageFilter001, TestSize.Level1)
{
    auto imageFilter = std::make_shared<Drawing::ImageFilter>();
    auto filterPtr = std::make_shared<RSShaderFilter>();
    std::vector<std::shared_ptr<RSShaderFilter>> shaderFilters;
    shaderFilters.push_back(filterPtr);
    uint32_t hash = 1;
    RSDrawingFilter drawingFilter(imageFilter, shaderFilters, hash);
    EXPECT_TRUE(drawingFilter.ProcessImageFilter(0.5) != nullptr);
}

/**
 * @tc.name: GetDescription001
 * @tc.desc: test results of GetDescription
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(RSDrawingFilterTest, GetDescription001, TestSize.Level1)
{
    auto imageFilter = std::make_shared<Drawing::ImageFilter>();
    std::vector<std::pair<float, float>> fractionStops;
    auto para = std::make_shared<RSLinearGradientBlurPara>(1.f, fractionStops, GradientDirection::LEFT);
    auto filterPtr = std::make_shared<RSLinearGradientBlurShaderFilter>(para, 1.f, 1.f);
    filterPtr->type_ = RSShaderFilter::KAWASE;
    std::vector<std::shared_ptr<RSShaderFilter>> shaderFilters;
    shaderFilters.push_back(filterPtr);
    uint32_t hash = 1;
    RSDrawingFilter drawingFilter(imageFilter, shaderFilters, hash);
    drawingFilter.SetFilterType(RSFilter::BLUR);
    EXPECT_TRUE(!drawingFilter.GetDescription().empty());
    filterPtr->type_ = RSShaderFilter::LINEAR_GRADIENT_BLUR;
    EXPECT_TRUE(!drawingFilter.GetDescription().empty());
    filterPtr->type_ = RSShaderFilter::MESA;
    EXPECT_TRUE(!drawingFilter.GetDescription().empty());
    filterPtr->type_ = RSShaderFilter::NONE;
    EXPECT_TRUE(!drawingFilter.GetDescription().empty());
}
 
/**
 * @tc.name: GetDescription002
 * @tc.desc: test results of GetDescription,shaderFilters is empty.
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(RSDrawingFilterTest, GetDescription002, TestSize.Level1)
{
    auto imageFilter = std::make_shared<Drawing::ImageFilter>();
    std::vector<std::pair<float, float>> fractionStops;
    auto para = std::make_shared<RSLinearGradientBlurPara>(1.f, fractionStops, GradientDirection::LEFT);
    auto filterPtr = std::make_shared<RSLinearGradientBlurShaderFilter>(para, 1.f, 1.f);
    filterPtr->type_ = RSShaderFilter::KAWASE;
    std::vector<std::shared_ptr<RSShaderFilter>> shaderFilters;

    uint32_t hash = 1;
    RSDrawingFilter drawingFilter(imageFilter, shaderFilters, hash);
    drawingFilter.SetFilterType(RSFilter::BLUR);
    EXPECT_TRUE(!drawingFilter.GetDescription().empty());
    filterPtr->type_ = RSShaderFilter::LINEAR_GRADIENT_BLUR;
    EXPECT_TRUE(!drawingFilter.GetDescription().empty());
    filterPtr->type_ = RSShaderFilter::MESA;
    EXPECT_TRUE(!drawingFilter.GetDescription().empty());
    filterPtr->type_ = RSShaderFilter::NONE;
    EXPECT_TRUE(!drawingFilter.GetDescription().empty());
}

/**
 * @tc.name: GetDetailedDescription001
 * @tc.desc: test results of GetDetailedDescription
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(RSDrawingFilterTest, GetDetailedDescription001, TestSize.Level1)
{
    auto imageFilter = std::make_shared<Drawing::ImageFilter>();
    std::vector<std::pair<float, float>> fractionStops;
    auto para = std::make_shared<RSLinearGradientBlurPara>(1.f, fractionStops, GradientDirection::LEFT);
    auto filterPtr = std::make_shared<RSLinearGradientBlurShaderFilter>(para, 1.f, 1.f);
    filterPtr->type_ = RSShaderFilter::KAWASE;
    std::vector<std::shared_ptr<RSShaderFilter>> shaderFilters;
    shaderFilters.push_back(filterPtr);
    uint32_t hash = 1;
    RSDrawingFilter drawingFilter(imageFilter, shaderFilters, hash);
    drawingFilter.SetFilterType(RSFilter::BLUR);
    EXPECT_TRUE(!drawingFilter.GetDetailedDescription().empty());

    filterPtr->type_ = RSShaderFilter::GREY;
    EXPECT_TRUE(!drawingFilter.GetDetailedDescription().empty());

    filterPtr->type_ = RSShaderFilter::MASK_COLOR;
    EXPECT_TRUE(!drawingFilter.GetDetailedDescription().empty());

    filterPtr->type_ = RSShaderFilter::LINEAR_GRADIENT_BLUR;
    EXPECT_TRUE(!drawingFilter.GetDetailedDescription().empty());

    filterPtr->type_ = RSShaderFilter::NONE;
    EXPECT_TRUE(!drawingFilter.GetDetailedDescription().empty());

    int radius = 1;
    auto filterPtr2 = std::make_shared<RSMESABlurShaderFilter>(radius);
    std::vector<std::shared_ptr<RSShaderFilter>> shaderFilters2;
    shaderFilters2.push_back(filterPtr2);
    RSDrawingFilter drawingFilter2(imageFilter, shaderFilters2, hash);
    drawingFilter2.SetFilterType(RSFilter::BLUR);
    filterPtr2->type_ = RSShaderFilter::MESA;
    EXPECT_TRUE(!drawingFilter2.GetDetailedDescription().empty());
}

/**
 * @tc.name: GetDetailedDescription002
 * @tc.desc: test results of GetDetailedDescription, shaderFilters is empty
 * @tc.type: FUNC
 */
HWTEST_F(RSDrawingFilterTest, GetDetailedDescription002, TestSize.Level1)
{
    auto imageFilter = std::make_shared<Drawing::ImageFilter>();
    std::vector<std::pair<float, float>> fractionStops;
    auto para = std::make_shared<RSLinearGradientBlurPara>(1.f, fractionStops, GradientDirection::LEFT);
    auto filterPtr = std::make_shared<RSLinearGradientBlurShaderFilter>(para, 1.f, 1.f);
    filterPtr->type_ = RSShaderFilter::KAWASE;
    std::vector<std::shared_ptr<RSShaderFilter>> shaderFilters;

    uint32_t hash = 1;
    RSDrawingFilter drawingFilter(imageFilter, shaderFilters, hash);
    drawingFilter.SetFilterType(RSFilter::BLUR);
    EXPECT_TRUE(!drawingFilter.GetDetailedDescription().empty());

    filterPtr->type_ = RSShaderFilter::GREY;
    EXPECT_TRUE(!drawingFilter.GetDetailedDescription().empty());

    filterPtr->type_ = RSShaderFilter::MASK_COLOR;
    EXPECT_TRUE(!drawingFilter.GetDetailedDescription().empty());

    filterPtr->type_ = RSShaderFilter::LINEAR_GRADIENT_BLUR;
    EXPECT_TRUE(!drawingFilter.GetDetailedDescription().empty());

    filterPtr->type_ = RSShaderFilter::NONE;
    EXPECT_TRUE(!drawingFilter.GetDetailedDescription().empty());

    int radius = 1;
    auto filterPtr2 = std::make_shared<RSMESABlurShaderFilter>(radius);
    std::vector<std::shared_ptr<RSShaderFilter>> shaderFilters2;
    shaderFilters2.push_back(filterPtr2);
    RSDrawingFilter drawingFilter2(imageFilter, shaderFilters2, hash);
    drawingFilter2.SetFilterType(RSFilter::BLUR);
    filterPtr2->type_ = RSShaderFilter::MESA;
    EXPECT_TRUE(!drawingFilter2.GetDetailedDescription().empty());
}

/**
 * @tc.name: Compose001
 * @tc.desc: test results of Compose
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(RSDrawingFilterTest, Compose001, TestSize.Level1)
{
    auto imageFilter = std::make_shared<Drawing::ImageFilter>();
    auto filterPtr = std::make_shared<RSShaderFilter>();
    std::vector<std::shared_ptr<RSShaderFilter>> shaderFilters;
    shaderFilters.push_back(filterPtr);
    uint32_t hash = 1;
    RSDrawingFilter drawingFilter(imageFilter, shaderFilters, hash);
    std::shared_ptr<RSDrawingFilter> other = nullptr;
    drawingFilter.Compose(other);
    EXPECT_TRUE(imageFilter != nullptr);

    other = std::make_shared<RSDrawingFilter>(imageFilter, shaderFilters, hash);
    drawingFilter.Compose(other);
    EXPECT_TRUE(other != nullptr);
}

/**
 * @tc.name: Compose002
 * @tc.desc: test results of Compose
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(RSDrawingFilterTest, Compose002, TestSize.Level1)
{
    auto imageFilter = std::make_shared<Drawing::ImageFilter>();
    auto filterPtr = std::make_shared<RSShaderFilter>();
    std::vector<std::shared_ptr<RSShaderFilter>> shaderFilters;
    shaderFilters.push_back(filterPtr);
    uint32_t hash = 1;
    RSDrawingFilter drawingFilter(imageFilter, shaderFilters, hash);
    std::shared_ptr<RSShaderFilter> other = nullptr;
    drawingFilter.Compose(other);
    EXPECT_TRUE(imageFilter != nullptr);

    other = std::make_shared<RSShaderFilter>();
    drawingFilter.Compose(other);
    EXPECT_TRUE(other != nullptr);
}

/**
 * @tc.name: Compose003
 * @tc.desc: test results of Compose
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(RSDrawingFilterTest, Compose003, TestSize.Level1)
{
    auto imageFilter = std::make_shared<Drawing::ImageFilter>();
    auto filterPtr = std::make_shared<RSShaderFilter>();
    std::vector<std::shared_ptr<RSShaderFilter>> shaderFilters;
    shaderFilters.push_back(filterPtr);
    uint32_t hash = 1;
    RSDrawingFilter drawingFilter(imageFilter, shaderFilters, hash);
    std::shared_ptr<Drawing::ImageFilter> other = nullptr;
    drawingFilter.Compose(other, hash);
    EXPECT_TRUE(imageFilter != nullptr);

    other = std::make_shared<Drawing::ImageFilter>();
    drawingFilter.Compose(other, hash);
    EXPECT_TRUE(other != nullptr);
}

/**
 * @tc.name: ApplyColorFilter001
 * @tc.desc: test results of ApplyColorFilter
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(RSDrawingFilterTest, ApplyColorFilter001, TestSize.Level1)
{
    auto imageFilter = std::make_shared<Drawing::ImageFilter>();
    auto filterPtr = std::make_shared<RSShaderFilter>();
    std::vector<std::shared_ptr<RSShaderFilter>> shaderFilters;
    shaderFilters.push_back(filterPtr);
    uint32_t hash = 1;
    RSDrawingFilter drawingFilter(imageFilter, shaderFilters, hash);
    Drawing::Canvas canvas;
    auto image = std::make_shared<Drawing::Image>();
    Drawing::Rect src;
    Drawing::Rect dst;
    drawingFilter.ApplyColorFilter(canvas, image, src, dst, 1.0f);
    drawingFilter.ApplyColorFilter(canvas, nullptr, src, dst, 1.0f);
    EXPECT_TRUE(image != nullptr);
}

/**
 * @tc.name: IsHpsBlurApplied001
 * @tc.desc: test results of IsHpsBlurApplied if system disable HPS
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterTest, IsHpsBlurApplied001, TestSize.Level1)
{
    auto imageFilter = std::make_shared<Drawing::ImageFilter>();
    auto filterPtr = std::make_shared<RSShaderFilter>();
    std::vector<std::shared_ptr<RSShaderFilter>> shaderFilters;
    shaderFilters.push_back(filterPtr);
    uint32_t hash = 1;
    RSDrawingFilter drawingFilter(imageFilter, shaderFilters, hash);
    Drawing::Canvas canvas;
    auto image = std::make_shared<Drawing::Image>();
    Drawing::Brush brush;
    RSDrawingFilter::DrawImageRectAttributes attr;

    RSSystemProperties::SetForceHpsBlurDisabled(true);
    bool ret1 = drawingFilter.IsHpsBlurApplied(canvas, image, attr, brush, 0.0f);
    EXPECT_FALSE(ret1);

    attr.brushAlpha = 0.5f;
    bool ret2 = drawingFilter.IsHpsBlurApplied(canvas, image, attr, brush, 0.5f);
    EXPECT_FALSE(ret2);

    attr.brushAlpha = 1.0f;
    bool ret3 = drawingFilter.IsHpsBlurApplied(canvas, image, attr, brush, 0.5f);
    EXPECT_FALSE(ret3);

    drawingFilter.type_ = RSFilter::MATERIAL;
    bool ret4 = drawingFilter.IsHpsBlurApplied(canvas, image, attr, brush, 1.0f);
    EXPECT_FALSE(ret4);

    attr.brushAlpha = 0.5f;
    bool ret5 = drawingFilter.IsHpsBlurApplied(canvas, image, attr, brush, 1.0f);
    EXPECT_FALSE(ret5);

    auto rsShaderFilter = std::make_shared<RSShaderFilter>();
    rsShaderFilter->type_ = RSShaderFilter::ShaderFilterType::MASK_COLOR;
    drawingFilter.shaderFilters_.push_back(rsShaderFilter);
    bool ret6 = drawingFilter.IsHpsBlurApplied(canvas, image, attr, brush, 1.0f);
    EXPECT_FALSE(ret6);
}

/**
 * @tc.name: IsHpsBlurApplied002
 * @tc.desc: test results of IsHpsBlurApplied, if system enable HPS
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterTest, IsHpsBlurApplied002, TestSize.Level1)
{
    auto imageFilter = std::make_shared<Drawing::ImageFilter>();
    auto filterPtr = std::make_shared<RSShaderFilter>();
    std::vector<std::shared_ptr<RSShaderFilter>> shaderFilters;
    shaderFilters.push_back(filterPtr);
    uint32_t hash = 1;
    RSDrawingFilter drawingFilter(imageFilter, shaderFilters, hash);
    Drawing::Canvas canvas;
    auto image = std::make_shared<Drawing::Image>();
    Drawing::Brush brush;
    RSDrawingFilter::DrawImageRectAttributes attr;

    RSSystemProperties::SetForceHpsBlurDisabled(false);
    bool ret1 = drawingFilter.IsHpsBlurApplied(canvas, image, attr, brush, 0.0f);
    EXPECT_FALSE(ret1);

    attr.brushAlpha = 0.5f;
    bool ret2 = drawingFilter.IsHpsBlurApplied(canvas, image, attr, brush, 0.5f);
    EXPECT_FALSE(ret2);

    attr.brushAlpha = 1.0f;
    bool ret3 = drawingFilter.IsHpsBlurApplied(canvas, image, attr, brush, 0.5f);
    EXPECT_FALSE(ret3);

    drawingFilter.type_ = RSFilter::MATERIAL;
    bool ret4 = drawingFilter.IsHpsBlurApplied(canvas, image, attr, brush, 1.0f);
    EXPECT_FALSE(ret4);

    attr.brushAlpha = 0.5f;
    bool ret5 = drawingFilter.IsHpsBlurApplied(canvas, image, attr, brush, 1.0f);
    EXPECT_FALSE(ret5);

    auto rsShaderFilter = std::make_shared<RSShaderFilter>();
    rsShaderFilter->type_ = RSShaderFilter::ShaderFilterType::MASK_COLOR;
    drawingFilter.shaderFilters_.push_back(rsShaderFilter);
    bool ret6 = drawingFilter.IsHpsBlurApplied(canvas, image, attr, brush, 1.0f);
    EXPECT_FALSE(ret6);
}

/**
 * @tc.name: DrawImageRect001
 * @tc.desc: test results of DrawImageRect
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(RSDrawingFilterTest, DrawImageRect001, TestSize.Level1)
{
    auto imageFilter = std::make_shared<Drawing::ImageFilter>();
    auto filterPtr = std::make_shared<RSShaderFilter>();
    filterPtr->type_ = RSShaderFilter::KAWASE;
    std::vector<std::shared_ptr<RSShaderFilter>> shaderFilters;
    shaderFilters.push_back(filterPtr);
    uint32_t hash = 1;
    RSDrawingFilter drawingFilter(imageFilter, shaderFilters, hash);
    Drawing::Canvas canvas;
    auto image = std::make_shared<Drawing::Image>();
    Drawing::Rect src;
    Drawing::Rect dst;
    drawingFilter.DrawImageRect(canvas, image, src, dst, { false, true });
    EXPECT_TRUE(image != nullptr);

    drawingFilter.DrawImageRect(canvas, image, src, dst, { false, false });
    EXPECT_TRUE(image != nullptr);

    filterPtr->type_ = RSShaderFilter::NONE;
    drawingFilter.DrawImageRect(canvas, image, src, dst, { false, true });
    EXPECT_TRUE(image != nullptr);

    drawingFilter.DrawImageRect(canvas, image, src, dst, { false, false });
    EXPECT_TRUE(image != nullptr);

    auto colorShaderFilter = std::make_shared<RSMaskColorShaderFilter>(0, RSColor());
    colorShaderFilter->maskColor_.SetAlpha(102);
    drawingFilter.InsertShaderFilter(colorShaderFilter);
    drawingFilter.DrawImageRect(canvas, image, src, dst, { false, false });
    EXPECT_TRUE(image != nullptr);

    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    paintFilterCanvas.SetAlpha(0.5);
    drawingFilter.DrawImageRect(paintFilterCanvas, image, src, dst, { false, false });
    EXPECT_TRUE(image != nullptr);
}

/**
 * @tc.name: PrepareAlphaForOnScreenDraw001
 * @tc.desc: test results of PrepareAlphaForOnScreenDraw
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterTest, PrepareAlphaForOnScreenDraw001, TestSize.Level1)
{
    auto imageFilter = std::make_shared<Drawing::ImageFilter>();
    auto filterPtr = std::make_shared<RSShaderFilter>();
    filterPtr->type_ = RSShaderFilter::KAWASE;
    std::vector<std::shared_ptr<RSShaderFilter>> shaderFilters;
    shaderFilters.push_back(filterPtr);
    uint32_t hash = 1;
    RSDrawingFilter drawingFilter(imageFilter, shaderFilters, hash);
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    float canvasAlpha = 0.5f;
    float defaultAlpha = 1.0f;
    float brushAlpha = defaultAlpha;

    paintFilterCanvas.SetAlpha(canvasAlpha);
    brushAlpha = drawingFilter.PrepareAlphaForOnScreenDraw(paintFilterCanvas);
    // no RSMaskColorShaderFilter, result is canvasAlpha
    EXPECT_TRUE(ROSEN_EQ(brushAlpha, canvasAlpha));

    auto colorShaderFilter = std::make_shared<RSMaskColorShaderFilter>(0, RSColor());
    colorShaderFilter->maskColor_.SetAlpha(102);
    drawingFilter.InsertShaderFilter(colorShaderFilter);

    canvasAlpha = 1.0f;
    paintFilterCanvas.SetAlpha(canvasAlpha);
    brushAlpha = drawingFilter.PrepareAlphaForOnScreenDraw(paintFilterCanvas);
    // brush alpha is 1.0 - result is default
    EXPECT_TRUE(ROSEN_EQ(brushAlpha, defaultAlpha));

    canvasAlpha = 0.0f;
    paintFilterCanvas.SetAlpha(canvasAlpha);
    brushAlpha = drawingFilter.PrepareAlphaForOnScreenDraw(paintFilterCanvas);
    // brush alpha is 0.0 - result is default
    EXPECT_TRUE(ROSEN_EQ(brushAlpha, defaultAlpha));

    canvasAlpha = 0.5f;
    paintFilterCanvas.SetAlpha(canvasAlpha);
    brushAlpha = drawingFilter.PrepareAlphaForOnScreenDraw(paintFilterCanvas);
    // brush alpha is canvasAlpha * (coeff from RSMaskColorShaderFilter)
    EXPECT_TRUE(ROSEN_EQ(brushAlpha, 0.375f));
}

/**
 * @tc.name: PrepareAlphaForOnScreenDraw002
 * @tc.desc: test results of PrepareAlphaForOnScreenDraw, shaderFilters is empty
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterTest, PrepareAlphaForOnScreenDraw002, TestSize.Level1)
{
    auto imageFilter = std::make_shared<Drawing::ImageFilter>();
    auto filterPtr = std::make_shared<RSShaderFilter>();
    filterPtr->type_ = RSShaderFilter::KAWASE;
    std::vector<std::shared_ptr<RSShaderFilter>> shaderFilters;

    uint32_t hash = 1;
    RSDrawingFilter drawingFilter(imageFilter, shaderFilters, hash);
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    float canvasAlpha = 0.5f;
    float defaultAlpha = 1.0f;
    float brushAlpha = defaultAlpha;

    paintFilterCanvas.SetAlpha(canvasAlpha);
    brushAlpha = drawingFilter.PrepareAlphaForOnScreenDraw(paintFilterCanvas);
    // no RSMaskColorShaderFilter, result is canvasAlpha
    EXPECT_TRUE(ROSEN_EQ(brushAlpha, canvasAlpha));

    auto colorShaderFilter = std::make_shared<RSMaskColorShaderFilter>(0, RSColor());
    colorShaderFilter->maskColor_.SetAlpha(102);
    drawingFilter.InsertShaderFilter(colorShaderFilter);

    canvasAlpha = 1.0f;
    paintFilterCanvas.SetAlpha(canvasAlpha);
    brushAlpha = drawingFilter.PrepareAlphaForOnScreenDraw(paintFilterCanvas);
    // brush alpha is 1.0 - result is default
    EXPECT_TRUE(ROSEN_EQ(brushAlpha, defaultAlpha));

    canvasAlpha = 0.0f;
    paintFilterCanvas.SetAlpha(canvasAlpha);
    brushAlpha = drawingFilter.PrepareAlphaForOnScreenDraw(paintFilterCanvas);
    // brush alpha is 0.0 - result is default
    EXPECT_TRUE(ROSEN_EQ(brushAlpha, defaultAlpha));

    canvasAlpha = 0.5f;
    paintFilterCanvas.SetAlpha(canvasAlpha);
    brushAlpha = drawingFilter.PrepareAlphaForOnScreenDraw(paintFilterCanvas);
    // brush alpha is canvasAlpha * (coeff from RSMaskColorShaderFilter)
    EXPECT_TRUE(ROSEN_EQ(brushAlpha, 0.375f));
}

/**
 * @tc.name: PreProcess001
 * @tc.desc: test results of PreProcess
 * @tc.type: FUNC
 */
HWTEST_F(RSDrawingFilterTest, PreProcess001, TestSize.Level1)
{
    auto imageFilter = std::make_shared<Drawing::ImageFilter>();
    auto filterPtr = std::make_shared<RSShaderFilter>();
    std::vector<std::shared_ptr<RSShaderFilter>> shaderFilters;
    shaderFilters.push_back(filterPtr);
    uint32_t hash = 1;
    RSDrawingFilter drawingFilter(imageFilter, shaderFilters, hash);
    auto image = std::make_shared<Drawing::Image>();
    drawingFilter.PreProcess(image);
    EXPECT_TRUE(image != nullptr);
}
 
/**
 * @tc.name: PreProcess002
 * @tc.desc: test results of PreProcess,, shaderFilters is empty
 * @tc.type: FUNC
 */
HWTEST_F(RSDrawingFilterTest, PreProcess002, TestSize.Level1)
{
    auto imageFilter = std::make_shared<Drawing::ImageFilter>();
    auto filterPtr = std::make_shared<RSShaderFilter>();
    std::vector<std::shared_ptr<RSShaderFilter>> shaderFilters;

    uint32_t hash = 1;
    RSDrawingFilter drawingFilter(imageFilter, shaderFilters, hash);
    auto image = std::make_shared<Drawing::Image>();
    drawingFilter.PreProcess(image);
    EXPECT_TRUE(image != nullptr);
}

/**
 * @tc.name: GetFilterTypeString002
 * @tc.desc: test GetFilterTypeString, shaderFilters is empty
 * @tc.type: FUNC
 */
HWTEST_F(RSDrawingFilterTest, GetFilterTypeString002, TestSize.Level1)
{
    auto imageFilter = std::make_shared<Drawing::ImageFilter>();
    auto filterPtr = std::make_shared<RSShaderFilter>();
    filterPtr->type_ = RSShaderFilter::KAWASE;
    std::vector<std::shared_ptr<RSShaderFilter>> shaderFilters;

    uint32_t hash = 1;
    RSDrawingFilter drawingFilter(imageFilter, shaderFilters, hash);
    drawingFilter.type_ = RSFilter::BLUR;
    EXPECT_TRUE(drawingFilter.GetFilterTypeString() == "RSBlurFilterBlur");
}

/**
 * @tc.name: GetFilterTypeString
 * @tc.desc: test GetFilterTypeString
 * @tc.type: FUNC
 */
HWTEST_F(RSDrawingFilterTest, GetFilterTypeString, TestSize.Level1)
{
    auto imageFilter = std::make_shared<Drawing::ImageFilter>();
    auto filterPtr = std::make_shared<RSShaderFilter>();
    filterPtr->type_ = RSShaderFilter::KAWASE;
    std::vector<std::shared_ptr<RSShaderFilter>> shaderFilters;
    shaderFilters.push_back(filterPtr);
    uint32_t hash = 1;
    RSDrawingFilter drawingFilter(imageFilter, shaderFilters, hash);
    drawingFilter.type_ = RSFilter::BLUR;
    EXPECT_TRUE(drawingFilter.GetFilterTypeString() == "RSBlurFilterBlur");
}

} // namespace OHOS::Rosen
