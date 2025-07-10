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
#include "render/rs_render_kawase_blur_filter.h"
#include "render/rs_render_aibar_filter.h"
#include "render/rs_render_bezier_warp_filter.h"
#include "render/rs_render_linear_gradient_blur_filter.h"
#include "render/rs_render_maskcolor_filter.h"
#include "render/rs_render_mesa_blur_filter.h"
#include "render/rs_render_light_blur_filter.h"
#include "ge_visual_effect_container.h"
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
    auto filterPtr = std::make_shared<RSRenderFilterParaBase>();
    std::vector<std::shared_ptr<RSRenderFilterParaBase>> shaderFilters;
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
    auto filterPtr = std::make_shared<RSRenderFilterParaBase>();
    std::vector<std::shared_ptr<RSRenderFilterParaBase>> shaderFilters;
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
    auto filterPtr = std::make_shared<RSRenderFilterParaBase>();
    std::vector<std::shared_ptr<RSRenderFilterParaBase>> shaderFilters;
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
    auto filterPtr = std::make_shared<RSRenderFilterParaBase>();
    std::vector<std::shared_ptr<RSRenderFilterParaBase>> shaderFilters;

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
    auto filterPtr = std::make_shared<RSRenderFilterParaBase>();
    std::vector<std::shared_ptr<RSRenderFilterParaBase>> shaderFilters;
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
    filterPtr->type_ = RSUIFilterType::KAWASE;
    std::vector<std::shared_ptr<RSRenderFilterParaBase>> shaderFilters;
    shaderFilters.push_back(filterPtr);
    uint32_t hash = 1;
    RSDrawingFilter drawingFilter(imageFilter, shaderFilters, hash);
    drawingFilter.SetFilterType(RSFilter::BLUR);
    EXPECT_TRUE(!drawingFilter.GetDescription().empty());
    filterPtr->type_ = RSUIFilterType::LINEAR_GRADIENT_BLUR;
    EXPECT_TRUE(!drawingFilter.GetDescription().empty());
    filterPtr->type_ = RSUIFilterType::MESA;
    EXPECT_TRUE(!drawingFilter.GetDescription().empty());
    filterPtr->type_ = RSUIFilterType::NONE;
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
    filterPtr->type_ = RSUIFilterType::KAWASE;
    std::vector<std::shared_ptr<RSRenderFilterParaBase>> shaderFilters;

    uint32_t hash = 1;
    RSDrawingFilter drawingFilter(imageFilter, shaderFilters, hash);
    drawingFilter.SetFilterType(RSFilter::BLUR);
    EXPECT_TRUE(!drawingFilter.GetDescription().empty());
    filterPtr->type_ = RSUIFilterType::LINEAR_GRADIENT_BLUR;
    EXPECT_TRUE(!drawingFilter.GetDescription().empty());
    filterPtr->type_ = RSUIFilterType::MESA;
    EXPECT_TRUE(!drawingFilter.GetDescription().empty());
    filterPtr->type_ = RSUIFilterType::NONE;
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
    filterPtr->type_ = RSUIFilterType::KAWASE;
    std::vector<std::shared_ptr<RSRenderFilterParaBase>> shaderFilters;
    shaderFilters.push_back(filterPtr);
    uint32_t hash = 1;
    RSDrawingFilter drawingFilter(imageFilter, shaderFilters, hash);
    drawingFilter.SetFilterType(RSFilter::BLUR);
    EXPECT_TRUE(!drawingFilter.GetDetailedDescription().empty());

    filterPtr->type_ = RSUIFilterType::GREY;
    EXPECT_TRUE(!drawingFilter.GetDetailedDescription().empty());

    filterPtr->type_ = RSUIFilterType::MASK_COLOR;
    EXPECT_TRUE(!drawingFilter.GetDetailedDescription().empty());

    filterPtr->type_ = RSUIFilterType::LINEAR_GRADIENT_BLUR;
    EXPECT_TRUE(!drawingFilter.GetDetailedDescription().empty());

    filterPtr->type_ = RSUIFilterType::NONE;
    EXPECT_TRUE(!drawingFilter.GetDetailedDescription().empty());

    int radius = 1;
    auto filterPtr2 = std::make_shared<RSMESABlurShaderFilter>(radius);
    std::vector<std::shared_ptr<RSRenderFilterParaBase>> shaderFilters2;
    shaderFilters2.push_back(filterPtr2);
    RSDrawingFilter drawingFilter2(imageFilter, shaderFilters2, hash);
    drawingFilter2.SetFilterType(RSFilter::BLUR);
    filterPtr2->type_ = RSUIFilterType::MESA;
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
    filterPtr->type_ = RSUIFilterType::KAWASE;
    std::vector<std::shared_ptr<RSRenderFilterParaBase>> shaderFilters;

    uint32_t hash = 1;
    RSDrawingFilter drawingFilter(imageFilter, shaderFilters, hash);
    drawingFilter.SetFilterType(RSFilter::BLUR);
    EXPECT_TRUE(!drawingFilter.GetDetailedDescription().empty());

    filterPtr->type_ = RSUIFilterType::GREY;
    EXPECT_TRUE(!drawingFilter.GetDetailedDescription().empty());

    filterPtr->type_ = RSUIFilterType::MASK_COLOR;
    EXPECT_TRUE(!drawingFilter.GetDetailedDescription().empty());

    filterPtr->type_ = RSUIFilterType::LINEAR_GRADIENT_BLUR;
    EXPECT_TRUE(!drawingFilter.GetDetailedDescription().empty());

    filterPtr->type_ = RSUIFilterType::NONE;
    EXPECT_TRUE(!drawingFilter.GetDetailedDescription().empty());

    int radius = 1;
    auto filterPtr2 = std::make_shared<RSMESABlurShaderFilter>(radius);
    std::vector<std::shared_ptr<RSRenderFilterParaBase>> shaderFilters2;
    shaderFilters2.push_back(filterPtr2);
    RSDrawingFilter drawingFilter2(imageFilter, shaderFilters2, hash);
    drawingFilter2.SetFilterType(RSFilter::BLUR);
    filterPtr2->type_ = RSUIFilterType::MESA;
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
    auto filterPtr = std::make_shared<RSRenderFilterParaBase>();
    std::vector<std::shared_ptr<RSRenderFilterParaBase>> shaderFilters;
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
    auto filterPtr = std::make_shared<RSRenderFilterParaBase>();
    std::vector<std::shared_ptr<RSRenderFilterParaBase>> shaderFilters;
    shaderFilters.push_back(filterPtr);
    uint32_t hash = 1;
    RSDrawingFilter drawingFilter(imageFilter, shaderFilters, hash);
    std::shared_ptr<RSRenderFilterParaBase> other = nullptr;
    drawingFilter.Compose(other);
    EXPECT_TRUE(imageFilter != nullptr);

    other = std::make_shared<RSRenderFilterParaBase>();
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
    auto filterPtr = std::make_shared<RSRenderFilterParaBase>();
    std::vector<std::shared_ptr<RSRenderFilterParaBase>> shaderFilters;
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
    auto filterPtr = std::make_shared<RSRenderFilterParaBase>();
    std::vector<std::shared_ptr<RSRenderFilterParaBase>> shaderFilters;
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
    auto filterPtr = std::make_shared<RSRenderFilterParaBase>();
    std::vector<std::shared_ptr<RSRenderFilterParaBase>> shaderFilters;
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

    auto rsShaderFilter = std::make_shared<RSRenderFilterParaBase>();
    rsShaderFilter->type_ = RSUIFilterType::MASK_COLOR;
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
    auto filterPtr = std::make_shared<RSRenderFilterParaBase>();
    std::vector<std::shared_ptr<RSRenderFilterParaBase>> shaderFilters;
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

    auto rsShaderFilter = std::make_shared<RSRenderFilterParaBase>();
    rsShaderFilter->type_ = RSUIFilterType::MASK_COLOR;
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
    auto filterPtr = std::make_shared<RSRenderFilterParaBase>();
    filterPtr->type_ = RSUIFilterType::KAWASE;
    std::vector<std::shared_ptr<RSRenderFilterParaBase>> shaderFilters;
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

    filterPtr->type_ = RSUIFilterType::NONE;
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
    auto filterPtr = std::make_shared<RSRenderFilterParaBase>();
    filterPtr->type_ = RSUIFilterType::KAWASE;
    std::vector<std::shared_ptr<RSRenderFilterParaBase>> shaderFilters;
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
    auto filterPtr = std::make_shared<RSRenderFilterParaBase>();
    filterPtr->type_ = RSUIFilterType::KAWASE;
    std::vector<std::shared_ptr<RSRenderFilterParaBase>> shaderFilters;

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
    auto filterPtr = std::make_shared<RSRenderFilterParaBase>();
    std::vector<std::shared_ptr<RSRenderFilterParaBase>> shaderFilters;
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
    auto filterPtr = std::make_shared<RSRenderFilterParaBase>();
    std::vector<std::shared_ptr<RSRenderFilterParaBase>> shaderFilters;

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
    auto filterPtr = std::make_shared<RSRenderFilterParaBase>();
    filterPtr->type_ = RSUIFilterType::KAWASE;
    std::vector<std::shared_ptr<RSRenderFilterParaBase>> shaderFilters;

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
    auto filterPtr = std::make_shared<RSRenderFilterParaBase>();
    filterPtr->type_ = RSUIFilterType::KAWASE;
    std::vector<std::shared_ptr<RSRenderFilterParaBase>> shaderFilters;
    shaderFilters.push_back(filterPtr);
    uint32_t hash = 1;
    RSDrawingFilter drawingFilter(imageFilter, shaderFilters, hash);
    drawingFilter.type_ = RSFilter::BLUR;
    EXPECT_TRUE(drawingFilter.GetFilterTypeString() == "RSBlurFilterBlur");
}

/**
 * @tc.name: ApplyImageEffect001
 * @tc.desc: test ApplyImageEffect
 * @tc.type: FUNC
 */
HWTEST_F(RSDrawingFilterTest, ApplyImageEffect001, TestSize.Level1)
{
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> nullImage;
    auto image = std::make_shared<Drawing::Image>();
    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    RSDrawingFilter::DrawImageRectAttributes attr;

    auto shaderFilter = std::make_shared<RSRenderFilterParaBase>();
    RSDrawingFilter drawingFilter(shaderFilter);

    /* image is null */
    nullImage = nullptr;
    drawingFilter.ApplyImageEffect(canvas, nullImage, visualEffectContainer, attr);

    /* go to hps branch */
    int radius = 4;
    auto kawaseShaderFilter = std::make_shared<RSKawaseBlurShaderFilter>(radius);
    drawingFilter.InsertShaderFilter(kawaseShaderFilter);
    drawingFilter.ApplyImageEffect(canvas, image, visualEffectContainer, attr);

    /* go to lightblur branch */
    int radius0 = 0;
    auto lightBlurShaderFilter = std::make_shared<RSLightBlurShaderFilter>(radius0);
    RSDrawingFilter drawingFilter1(lightBlurShaderFilter);
    lightBlurShaderFilter->GenerateGEVisualEffect(visualEffectContainer);
    drawingFilter1.ApplyImageEffect(canvas, image, visualEffectContainer, attr);

    EXPECT_FALSE(shaderFilter == nullptr);
}

/**
 * @tc.name: ApplyImageEffect002
 * @tc.desc: test ApplyImageEffect
 * @tc.type: FUNC
 */
HWTEST_F(RSDrawingFilterTest, ApplyImageEffect002, TestSize.Level1)
{
    Drawing::Canvas canvas;
    auto image = std::make_shared<Drawing::Image>();
    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    RSDrawingFilter::DrawImageRectAttributes attr;

    int radius = 4;
    int radius0 = 0;
    auto kawaseShaderFilter = std::make_shared<RSKawaseBlurShaderFilter>(radius);

    /* go to hps 1.0 kawase ApplyColorFilter branch */
    auto visualEffectContainer1 = std::make_shared<Drawing::GEVisualEffectContainer>();
    auto kawaseShaderFilter2 = std::make_shared<RSKawaseBlurShaderFilter>(radius0);
    RSDrawingFilter drawingFilter2(kawaseShaderFilter2);
    kawaseShaderFilter2->GenerateGEVisualEffect(visualEffectContainer1);
    auto rsRenderBezierWarpFilterPara = std::make_shared<RSRenderBezierWarpFilterPara>(0);
    drawingFilter2.InsertShaderFilter(rsRenderBezierWarpFilterPara);
    rsRenderBezierWarpFilterPara->GenerateGEVisualEffect(visualEffectContainer1);
    drawingFilter2.ApplyImageEffect(canvas, image, visualEffectContainer1, attr);

    /* go to hps 1.0 kawase branch */
    auto visualEffectContainer3 = std::make_shared<Drawing::GEVisualEffectContainer>();
    RSDrawingFilter drawingFilter3(kawaseShaderFilter);
    drawingFilter3.InsertShaderFilter(rsRenderBezierWarpFilterPara);
    rsRenderBezierWarpFilterPara->GenerateGEVisualEffect(visualEffectContainer3);
    drawingFilter3.ApplyImageEffect(canvas, image, visualEffectContainer3, attr);
    /* go to ge kawase branch */
    drawingFilter3.SetFilterType(RSFilter::WATER_RIPPLE);
    drawingFilter3.ApplyImageEffect(canvas, image, visualEffectContainer3, attr);

    /* go to no blur branch */
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    auto visualEffectContainer4 = std::make_shared<Drawing::GEVisualEffectContainer>();
    rsAIBarShaderFilter->GenerateGEVisualEffect(visualEffectContainer4);
    RSDrawingFilter drawingFilter4(rsAIBarShaderFilter);
    drawingFilter4.ApplyImageEffect(canvas, image, visualEffectContainer4, attr);

    EXPECT_FALSE(kawaseShaderFilter == nullptr);
}

/**
 * @tc.name: ApplyHpsImageEffect001
 * @tc.desc: test ApplyHpsImageEffect
 * @tc.type: FUNC
 */
HWTEST_F(RSDrawingFilterTest, ApplyHpsImageEffect001, TestSize.Level1)
{
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> nullImage;
    auto image = std::make_shared<Drawing::Image>();
    auto outImage = std::make_shared<Drawing::Image>();
    RSDrawingFilter::DrawImageRectAttributes attr;
    Drawing::Brush brush;

    auto shaderFilter = std::make_shared<RSRenderFilterParaBase>();
    RSDrawingFilter drawingFilter(shaderFilter);

    /* normal case */
    drawingFilter.ApplyHpsImageEffect(canvas, image, outImage, attr, brush);

    /* make outImage nullptr */
    nullImage = nullptr;
    drawingFilter.ApplyHpsImageEffect(canvas, nullImage, outImage, attr, brush);
    EXPECT_FALSE(shaderFilter == nullptr);
}

/**
 * @tc.name: DrawKawaseEffect
 * @tc.desc: test DrawKawaseEffect
 * @tc.type: FUNC
 */
HWTEST_F(RSDrawingFilterTest, DrawKawaseEffect, TestSize.Level1)
{
    Drawing::Canvas canvas;
    auto outImage = std::make_shared<Drawing::Image>();
    RSDrawingFilter::DrawImageRectAttributes attr;
    Drawing::Brush brush;
    int radius = 1;
    auto kawaseShaderFilter = std::static_pointer_cast<RSRenderFilterParaBase>(
        std::make_shared<RSKawaseBlurShaderFilter>(radius));
    auto shaderFilter = std::make_shared<RSRenderFilterParaBase>();
    RSDrawingFilter drawingFilter(shaderFilter);

    /* normal case */
    drawingFilter.DrawKawaseEffect(canvas, outImage, attr, brush, kawaseShaderFilter);

    /* make blurImage nullptr */
    outImage = nullptr;
    drawingFilter.DrawKawaseEffect(canvas, outImage, attr, brush, kawaseShaderFilter);

    EXPECT_FALSE(kawaseShaderFilter == nullptr);
}
} // namespace OHOS::Rosen
