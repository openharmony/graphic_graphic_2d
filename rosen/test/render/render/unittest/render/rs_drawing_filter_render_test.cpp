/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
using namespace testing::text;

namespace OHOS::Rosen {

class RSDrawingFilterRenderTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSDrawingFilterRenderTest::SetUpTestCase() {}
void RSDrawingFilterRenderTest::TearDownTestCase() {}
void RSDrawingFilterRenderTest::SetUp() {}
void RSDrawingFilterRenderTest::TearDown() {}

/**
 * @tc.name: TestRSDrawingFilter01
 * @tc.desc: Verify function RSDrawingFilter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterRenderTest, TestRSDrawingFilter01, TestSize.Level1)
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
 * @tc.name: TestRSDrawingFilter02
 * @tc.desc: shaderFilters is empty
 * @tc.type: FUNC
 */
HWTEST_F(RSDrawingFilterRenderTest, TestRSDrawingFilter02, TestSize.Level1)
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
 * @tc.name: TestSetImageFilter01
 * @tc.desc: Verify function SetImageFilter
 * @tc.type: FUNC
 */
HWTEST_F(RSDrawingFilterRenderTest, TestSetImageFilter01, TestSize.Level1)
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
 * @tc.name: TestSetImageFilter02
 * @tc.desc: Verify function SetImageFilter. shaderFilters is empty
 * @tc.type: FUNC
 */
HWTEST_F(RSDrawingFilterRenderTest, TestSetImageFilter02, TestSize.Level1)
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
 * @tc.name: TestProcessImageFilter01
 * @tc.desc: Verify function ProcessImageFilter
 * @tc.type: FUNC
 */
HWTEST_F(RSDrawingFilterRenderTest, TestProcessImageFilter01, TestSize.Level1)
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
 * @tc.name: TestGetDescription01
 * @tc.desc: Verify function GetDescription
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterRenderTest, TestGetDescription01, TestSize.Level1)
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
 * @tc.name: TestGetDescription02
 * @tc.desc: Verify function GetDescription,shaderFilters is empty.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterRenderTest, TestGetDescription02, TestSize.Level1)
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
 * @tc.name: TestGetDetailedDescription01
 * @tc.desc: Verify function GetDetailedDescription
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterRenderTest, TestGetDetailedDescription01, TestSize.Level1)
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
 * @tc.name: TestGetDetailedDescription02
 * @tc.desc: Verify function GetDetailedDescription, shaderFilters is empty
 * @tc.type: FUNC
 */
HWTEST_F(RSDrawingFilterRenderTest, TestGetDetailedDescription02, TestSize.Level1)
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
 * @tc.name: TestCompose01
 * @tc.desc: Verify function Compose
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterRenderTest, TestCompose01, TestSize.Level1)
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
 * @tc.name: TestCompose02
 * @tc.desc: Verify function Compose
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterRenderTest, TestCompose02, TestSize.Level1)
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
 * @tc.name: TestCompose03
 * @tc.desc: Verify function Compose
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterRenderTest, TestCompose03, TestSize.Level1)
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
 * @tc.name: TestApplyColorFilter01
 * @tc.desc: Verify function ApplyColorFilter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterRenderTest, TestApplyColorFilter01, TestSize.Level1)
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
 * @tc.name: TestDrawImageRect01
 * @tc.desc: Verify function DrawImageRect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterRenderTest, TestDrawImageRect01, TestSize.Level1)
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

    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    paintFilterCanvas.SetAlpha(0.5);
    drawingFilter.DrawImageRect(paintFilterCanvas, image, src, dst, { false, false });
    EXPECT_TRUE(image != nullptr);
}

/**
 * @tc.name: TestPrepareAlphaForOnScreenDraw01
 * @tc.desc: Verify function PrepareAlphaForOnScreenDraw
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterRenderTest, TestPrepareAlphaForOnScreenDraw01, TestSize.Level1)
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
 * @tc.name: TestPrepareAlphaForOnScreenDraw02
 * @tc.desc: Verify function PrepareAlphaForOnScreenDraw, shaderFilters is empty
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawingFilterRenderTest, TestPrepareAlphaForOnScreenDraw02, TestSize.Level1)
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
 * @tc.name: TestPreProcess01
 * @tc.desc: Verify function PreProcess
 * @tc.type: FUNC
 */
HWTEST_F(RSDrawingFilterRenderTest, TestPreProcess01, TestSize.Level1)
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
 * @tc.name: TestPreProcess02
 * @tc.desc: Verify function PreProcess,, shaderFilters is empty
 * @tc.type: FUNC
 */
HWTEST_F(RSDrawingFilterRenderTest, TestPreProcess02, TestSize.Level1)
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
 * @tc.name: TestGetFilterTypeString02
 * @tc.desc: Verify function GetFilterTypeString, shaderFilters is empty
 * @tc.type: FUNC
 */
HWTEST_F(RSDrawingFilterRenderTest, TestGetFilterTypeString02, TestSize.Level1)
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
 * @tc.name: TestGetFilterTypeString
 * @tc.desc: Verify function GetFilterTypeString
 * @tc.type: FUNC
 */
HWTEST_F(RSDrawingFilterRenderTest, TestGetFilterTypeString, TestSize.Level1)
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
