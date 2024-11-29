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

#include "drawable/rs_property_drawable_utils.h"
#include "draw/surface.h"
#include "property/rs_properties_painter.h"
#include "render/rs_drawing_filter.h"
#include "skia_adapter/skia_image.h"
#include "skia_adapter/skia_image_info.h"
#include "skia_adapter/skia_surface.h"
#include "render/rs_linear_gradient_blur_shader_filter.h"
#include "render/rs_magnifier_shader_filter.h"
#include "render/rs_mesa_blur_shader_filter.h"

namespace OHOS::Rosen {
class RSPropertyDrawableUtilsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSPropertyDrawableUtilsTest::SetUpTestCase() {}
void RSPropertyDrawableUtilsTest::TearDownTestCase() {}
void RSPropertyDrawableUtilsTest::SetUp() {}
void RSPropertyDrawableUtilsTest::TearDown() {}

/**
 * @tc.name: GetRRectForDrawingBorderTest
 * @tc.desc: GetRRectForDrawingBorder test
 * @tc.type: FUNC
 * @tc.require:issueIB7R0A
 */
HWTEST_F(RSPropertyDrawableUtilsTest, GetRRectForDrawingBorderTest, testing::ext::TestSize.Level1)
{
    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtils = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtils, nullptr);
    RSProperties properties;
    properties.rrect_ = RRect(RectT<float>(0.0f, 0.0f, 1.0f, 1.0f), 0.0f, 1.0f);
    std::shared_ptr<RSBorder> border = std::make_shared<RSBorder>();
    EXPECT_NE(border, nullptr);
    EXPECT_EQ(rsPropertyDrawableUtils->GetRRectForDrawingBorder(properties, border, false),
        RRect(RectT<float>(0.0f, 0.0f, 1.0f, 1.0f), 0.0f, 1.0f));
    EXPECT_EQ(rsPropertyDrawableUtils->GetInnerRRectForDrawingBorder(properties, border, true),
        RRect(RectT<float>(0.0f, 0.0f, 1.0f, 1.0f), 0.0f, 1.0f));
    border = nullptr;
    rsPropertyDrawableUtils->GetRRectForDrawingBorder(properties, border, false);
    rsPropertyDrawableUtils->GetInnerRRectForDrawingBorder(properties, border, true);
}

/**
 * @tc.name: GetDarkColorTest
 * @tc.desc: GetDarkColor test
 * @tc.type: FUNC
 * @tc.require:issueIB7R0A
 */
HWTEST_F(RSPropertyDrawableUtilsTest, GetDarkColorTest, testing::ext::TestSize.Level1)
{
    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtils = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtils, nullptr);
    RSColor color(255, 255, 255, 255);
    rsPropertyDrawableUtils->GetDarkColor(color);
}

/**
 * @tc.name: RRect2DrawingRRectAndCreateShadowPathTest
 * @tc.desc: RRect2DrawingRRect and CreateShadowPath test
 * @tc.type: FUNC
 * @tc.require:issueIB7R0A
 */
HWTEST_F(RSPropertyDrawableUtilsTest, RRect2DrawingRRectAndCreateShadowPathTest, testing::ext::TestSize.Level1)
{
    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtils = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtils, nullptr);
    Drawing::Path path;
    path.AddRect({0, 0, 10, 10});
    std::shared_ptr<RSPath> rsPath = RSPath::CreateRSPath(path);
    EXPECT_NE(rsPath, nullptr);
    std::shared_ptr<RSPath> clipBounds = std::make_shared<RSPath>();
    RRect rrect(RectT<float>(0.0f, 0.0f, 1.0f, 1.0f), 0.0f, 1.0f);
    rsPropertyDrawableUtils->CreateShadowPath(rsPath, clipBounds, rrect);
    rsPath = nullptr;
    rsPropertyDrawableUtils->CreateShadowPath(rsPath, clipBounds, rrect);
    clipBounds = nullptr;
    rsPropertyDrawableUtils->CreateShadowPath(rsPath, clipBounds, rrect);
}

/**
 * @tc.name: DrawColorFilterTest
 * @tc.desc: DrawColorFilter test
 * @tc.type: FUNC
 * @tc.require:issueIB7R0A
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawColorFilterTest, testing::ext::TestSize.Level1)
{
    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtils = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtils, nullptr);

    Drawing::Canvas canvasTest1;
    std::shared_ptr<Drawing::ColorFilter> colorFilter = nullptr;
    rsPropertyDrawableUtils->DrawColorFilter(&canvasTest1, colorFilter);

    colorFilter = std::make_shared<Drawing::ColorFilter>();
    Drawing::Canvas canvasTest2;
    RSPaintFilterCanvas paintFilterCanvas(&canvasTest2);
    paintFilterCanvas.surface_ = nullptr;
    rsPropertyDrawableUtils->DrawColorFilter(&paintFilterCanvas, colorFilter);

    Drawing::Surface surface;
    paintFilterCanvas.surface_ = &surface;
    rsPropertyDrawableUtils->DrawColorFilter(&paintFilterCanvas, colorFilter);
}

/**
 * @tc.name: DrawLightUpEffectTest
 * @tc.desc: DrawLightUpEffect and DrawDynamicDim test
 * @tc.type: FUNC
 * @tc.require:issueIA5Y41
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawLightUpEffectTest, testing::ext::TestSize.Level1)
{
    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtilsTest1 = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtilsTest1, nullptr);
    Drawing::Canvas canvasTest1;
    RSPaintFilterCanvas paintFilterCanvasTest1(&canvasTest1);
    paintFilterCanvasTest1.surface_ = nullptr;
    rsPropertyDrawableUtilsTest1->DrawLightUpEffect(&paintFilterCanvasTest1, -1.0f);
    rsPropertyDrawableUtilsTest1->DrawLightUpEffect(&paintFilterCanvasTest1, 1.0f);
    rsPropertyDrawableUtilsTest1->DrawLightUpEffect(&paintFilterCanvasTest1, 0.0f);
    Drawing::Surface surfaceTest1;
    paintFilterCanvasTest1.surface_ = &surfaceTest1;
    rsPropertyDrawableUtilsTest1->DrawLightUpEffect(&paintFilterCanvasTest1, 0.0f);

    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtilsTest2 = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtilsTest2, nullptr);
    Drawing::Canvas canvasTest2;
    RSPaintFilterCanvas paintFilterCanvasTest2(&canvasTest2);
    paintFilterCanvasTest2.surface_ = nullptr;
    rsPropertyDrawableUtilsTest2->DrawDynamicDim(&paintFilterCanvasTest2, -1.0f);
    rsPropertyDrawableUtilsTest1->DrawDynamicDim(&paintFilterCanvasTest2, 1.0f);
    rsPropertyDrawableUtilsTest2->DrawDynamicDim(&paintFilterCanvasTest2, 0.0f);
    Drawing::Surface surfaceTest2;
    paintFilterCanvasTest2.surface_ = &surfaceTest2;
    std::shared_ptr<Drawing::SkiaSurface> implTest2 = std::make_shared<Drawing::SkiaSurface>();
    EXPECT_NE(implTest2, nullptr);
    implTest2->skSurface_ = nullptr;
    paintFilterCanvasTest2.surface_->impl_ = implTest2;
    rsPropertyDrawableUtilsTest2->DrawDynamicDim(&paintFilterCanvasTest2, 0.0f);
}

/**
 * @tc.name: DrawShadowAndCeilMatrixTransTest
 * @tc.desc: DrawShadow and CeilMatrixTrans test
 * @tc.type: FUNC
 * @tc.require:issueIB7R0A
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawShadowAndCeilMatrixTransTest, testing::ext::TestSize.Level1)
{
    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtils = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtils, nullptr);
    Drawing::Canvas canvasTest1;
    Drawing::Path pathTest1;
    Color spotColor;
    rsPropertyDrawableUtils->DrawShadow(&canvasTest1, pathTest1, 1.0f, 1.0f, 1.0f, false, spotColor);
    Drawing::Canvas canvasTest2;
    Drawing::Path pathTest2;
    rsPropertyDrawableUtils->DrawShadow(&canvasTest2, pathTest2, 1.0f, 1.0f, 1.0f, true, spotColor);
}

/**
 * @tc.name: DrawAndBeginForegroundFilterTest
 * @tc.desc: DrawFilter and BeginForegroundFilter test
 * @tc.type: FUNC
 * @tc.require:issueIA5Y41
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawAndBeginForegroundFilterTest, testing::ext::TestSize.Level1)
{
    // first: DrawFilter test
    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtils = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtils, nullptr);
    Drawing::Canvas canvasTest1;
    RSPaintFilterCanvas paintFilterCanvasTest1(&canvasTest1);
    std::shared_ptr<RSDrawingFilter> rsFilter = nullptr;
    std::unique_ptr<RSFilterCacheManager> cacheManager = std::make_unique<RSFilterCacheManager>();
    paintFilterCanvasTest1.surface_ = nullptr;
    rsPropertyDrawableUtils->DrawFilter(&paintFilterCanvasTest1, rsFilter, cacheManager, false, false);
    std::vector<std::pair<float, float>> fractionStops;
    auto para = std::make_shared<RSLinearGradientBlurPara>(1.f, fractionStops, GradientDirection::LEFT);
    auto rsFilterTest = std::make_shared<RSLinearGradientBlurShaderFilter>(para, 1.f, 1.f);
    rsFilterTest->type_ = RSShaderFilter::LINEAR_GRADIENT_BLUR;
    rsFilter = std::make_shared<RSDrawingFilter>(rsFilterTest);
    rsFilter->type_ = RSFilter::BLUR;
    rsFilter->imageFilter_ = std::make_shared<Drawing::ImageFilter>();
    EXPECT_NE(rsFilter->imageFilter_, nullptr);
    rsPropertyDrawableUtils->DrawFilter(&paintFilterCanvasTest1, rsFilter, cacheManager, true, false);
    rsPropertyDrawableUtils->DrawFilter(nullptr, rsFilter, cacheManager, false, false);
    auto surface = Drawing::Surface::MakeRasterN32Premul(10, 10);
    paintFilterCanvasTest1.surface_ = surface.get();
    paintFilterCanvasTest1.SetDisableFilterCache(false);
    rsPropertyDrawableUtils->DrawFilter(&paintFilterCanvasTest1, rsFilter, cacheManager, true, false);
    paintFilterCanvasTest1.SetDisableFilterCache(true);
    rsPropertyDrawableUtils->DrawFilter(&paintFilterCanvasTest1, rsFilter, cacheManager, true, false);
    auto magnifierParams = std::make_shared<RSMagnifierParams>();
    auto magnifierFilter = std::make_shared<RSMagnifierShaderFilter>(magnifierParams);
    magnifierFilter->type_ = RSShaderFilter::MAGNIFIER;
    rsFilter = std::make_shared<RSDrawingFilter>(magnifierFilter);
    rsFilter->type_ = RSFilter::BLUR;
    rsFilter->imageFilter_ = std::make_shared<Drawing::ImageFilter>();
    rsPropertyDrawableUtils->DrawFilter(&paintFilterCanvasTest1, rsFilter, cacheManager, true, false);

    // second: BeginForegroundFilter test
    Drawing::Canvas canvasTest2;
    RSPaintFilterCanvas paintFilterCanvasTest2(&canvasTest2);
    paintFilterCanvasTest2.surface_ = nullptr;
    RectF bounds = RectF(1.0f, 1.0f, 1.0f, 1.0f);
    rsPropertyDrawableUtils->BeginForegroundFilter(paintFilterCanvasTest2, bounds);
    Drawing::Surface surfaceTest2;
    paintFilterCanvasTest2.surface_ = &surfaceTest2;
    rsPropertyDrawableUtils->BeginForegroundFilter(paintFilterCanvasTest2, bounds);
    auto surfaceTest3 = Drawing::Surface::MakeRasterN32Premul(10, 10);
    paintFilterCanvasTest2.surface_ = surfaceTest3.get();
    rsPropertyDrawableUtils->BeginForegroundFilter(paintFilterCanvasTest2, bounds);
    EXPECT_NE(paintFilterCanvasTest2.surface_->Width(), 0);
    EXPECT_NE(paintFilterCanvasTest2.surface_->Height(), 0);

    // third: DrawFilterWithDRM test
    Drawing::Canvas canvasTest3;
    RSPaintFilterCanvas paintFilterCanvasTest3(&canvasTest3);
    rsPropertyDrawableUtils->DrawFilterWithDRM(&paintFilterCanvasTest3, true);
    rsPropertyDrawableUtils->DrawFilterWithDRM(&paintFilterCanvasTest3, false);
}

/**
 * @tc.name: BDrawForegroundFilterTest
 * @tc.desc: DrawForegroundFilter test
 * @tc.type: FUNC
 * @tc.require:issueIB7R0A
 */
HWTEST_F(RSPropertyDrawableUtilsTest, BDrawForegroundFilterTest, testing::ext::TestSize.Level1)
{
    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtils = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtils, nullptr);
    Drawing::Canvas canvasTest;
    RSPaintFilterCanvas paintFilterCanvas(&canvasTest);
    std::shared_ptr<RSFilter> rsFilter = nullptr;
    rsPropertyDrawableUtils->DrawForegroundFilter(paintFilterCanvas, rsFilter);
    paintFilterCanvas.surface_ = nullptr;
    rsFilter = std::make_shared<RSFilter>();
    rsPropertyDrawableUtils->DrawForegroundFilter(paintFilterCanvas, rsFilter);
    Drawing::Surface surface;
    paintFilterCanvas.surface_ = &surface;
    rsPropertyDrawableUtils->DrawForegroundFilter(paintFilterCanvas, rsFilter);
}

/**
 * @tc.name: RSPropertyDrawableUtilsTest
 * @tc.desc: GetAndResetBlurCnt and DrawBackgroundEffect test
 * @tc.type: FUNC
 * @tc.require:issueIB7R0A
 */
HWTEST_F(RSPropertyDrawableUtilsTest, RSPropertyDrawableUtilsTest, testing::ext::TestSize.Level1)
{
    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtils = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtils, nullptr);
    // GetAndResetBlurCnt test
    rsPropertyDrawableUtils->g_blurCnt = 0;
    // DrawBackgroundEffect test
    Drawing::Canvas canvasTest1;
    RSPaintFilterCanvas paintFilterCanvas(&canvasTest1);
    std::shared_ptr<RSFilter> rsFilter = nullptr;
    std::unique_ptr<RSFilterCacheManager> cacheManager = std::make_unique<RSFilterCacheManager>();
    Drawing::RectI bounds(0, 0, 400, 400);
    rsPropertyDrawableUtils->DrawBackgroundEffect(&paintFilterCanvas, rsFilter, cacheManager, false, bounds);
    rsFilter = std::make_shared<RSFilter>();
    rsFilter->type_ = RSFilter::NONE;
    paintFilterCanvas.surface_ = nullptr;
    rsPropertyDrawableUtils->DrawBackgroundEffect(&paintFilterCanvas, rsFilter, cacheManager, false, bounds);
    Drawing::Surface surface;
    paintFilterCanvas.surface_ = &surface;
    Drawing::Canvas canvasTest2;
    paintFilterCanvas.canvas_ = &canvasTest2;
    rsPropertyDrawableUtils->DrawBackgroundEffect(&paintFilterCanvas, rsFilter, cacheManager, false, bounds);
    paintFilterCanvas.SetDisableFilterCache(true);
    rsPropertyDrawableUtils->DrawBackgroundEffect(&paintFilterCanvas, rsFilter, cacheManager, false, bounds);
    int lastBlurCnt = rsPropertyDrawableUtils->g_blurCnt;
    ASSERT_NE(lastBlurCnt, 0);
    rsPropertyDrawableUtils->DrawBackgroundEffect(nullptr, rsFilter, cacheManager, false, bounds);
    ASSERT_EQ(lastBlurCnt, lastBlurCnt);
}

/**
 * @tc.name: MakeLightUpEffectBlender
 * @tc.desc: test results of MakeLightUpEffectBlender
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyDrawableUtilsTest, MakeLightUpEffectBlender, testing::ext::TestSize.Level1)
{
    auto blender = RSPropertyDrawableUtils::MakeLightUpEffectBlender(1.0f);
    EXPECT_NE(blender, nullptr);
}

/**
 * @tc.name: DrawBinarizationTest
 * @tc.desc: DrawBinarization test
 * @tc.type: FUNC
 * @tc.require:issueIB7R0A
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawBinarizationTest, testing::ext::TestSize.Level1)
{
    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtilsTest = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtilsTest, nullptr);

    Drawing::Canvas canvasTest;
    RSPaintFilterCanvas paintFilterCanvasTest(&canvasTest);

    std::optional<Vector4f> aiInvert = std::nullopt;
    rsPropertyDrawableUtilsTest->DrawBinarization(&paintFilterCanvasTest, aiInvert);

    aiInvert = 1.0f;
    paintFilterCanvasTest.surface_ = nullptr;
    rsPropertyDrawableUtilsTest->DrawBinarization(&paintFilterCanvasTest, aiInvert);

    Drawing::Surface surface;
    paintFilterCanvasTest.surface_ = &surface;
    rsPropertyDrawableUtilsTest->DrawBinarization(&paintFilterCanvasTest, aiInvert);
}

/**
 * @tc.name: DrawPixelStretchTest
 * @tc.desc: DrawPixelStretch test
 * @tc.type: FUNC
 * @tc.require:issueIB7R0A
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawPixelStretchTest, testing::ext::TestSize.Level1)
{
    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtilsTest = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtilsTest, nullptr);
    Drawing::Canvas canvasTest;
    RSPaintFilterCanvas paintFilterCanvasTest(&canvasTest);
    RectF boundsRect = RectF(0.0f, 0.0f, 1.0f, 1.0f);
    std::optional<Vector4f> pixelStretch = std::nullopt;
    rsPropertyDrawableUtilsTest->DrawPixelStretch(
        &paintFilterCanvasTest, pixelStretch, boundsRect, true, Drawing::TileMode::CLAMP);
    pixelStretch = 1.0f;
    paintFilterCanvasTest.surface_ = nullptr;
    rsPropertyDrawableUtilsTest->DrawPixelStretch(
        &paintFilterCanvasTest, pixelStretch, boundsRect, true, Drawing::TileMode::CLAMP);
    Drawing::Surface surface;
    paintFilterCanvasTest.surface_ = &surface;
    rsPropertyDrawableUtilsTest->DrawPixelStretch(
        &paintFilterCanvasTest, pixelStretch, boundsRect, true, Drawing::TileMode::CLAMP);
}

/**
 * @tc.name: TransformativeShaderTest
 * @tc.desc: MakeDynamicDimShader MakeBinarizationShader MakeDynamicBrightnessBlender MakeDynamicBrightnessBuilder test
 * @tc.type: FUNC
 * @tc.require:issueIA5Y41
 */
HWTEST_F(RSPropertyDrawableUtilsTest, TransformativeShaderTest, testing::ext::TestSize.Level1)
{
    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtilsTest1 = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtilsTest1, nullptr);
    std::shared_ptr<Drawing::ShaderEffect> imageShaderTest1 = std::make_shared<Drawing::ShaderEffect>();
    EXPECT_NE(rsPropertyDrawableUtilsTest1->MakeDynamicDimShader(1.0f, imageShaderTest1), nullptr);

    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtilsTest2 = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtilsTest2, nullptr);
    std::shared_ptr<Drawing::ShaderEffect> imageShaderTest2 = std::make_shared<Drawing::ShaderEffect>();
    EXPECT_NE(rsPropertyDrawableUtilsTest2->MakeBinarizationShader(1.0f, 1.0f, 1.0f, 1.0f, imageShaderTest2), nullptr);

    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtilsTest3 = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtilsTest3, nullptr);
    EXPECT_NE(rsPropertyDrawableUtilsTest3->MakeDynamicBrightnessBuilder(), nullptr);

    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtilsTest4 = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtilsTest4, nullptr);
    RSDynamicBrightnessPara params;
    EXPECT_NE(rsPropertyDrawableUtilsTest4->MakeDynamicBrightnessBlender(params), nullptr);
}

/**
 * @tc.name: DrawShadowTestAndDrawUseEffectTest
 * @tc.desc: DrawShadow and DrawUseEffect test
 * @tc.type: FUNC
 * @tc.require:issueIB7R0A
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawShadowTestAndDrawUseEffectTest, testing::ext::TestSize.Level1)
{
    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtilsTest1 = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtilsTest1, nullptr);
    Drawing::Canvas canvasTest1;
    Drawing::Path path;
    Color spotColor;
    rsPropertyDrawableUtilsTest1->DrawShadow(&canvasTest1, path, 1.0f, 1.0f, 1.0f, true, spotColor);

    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtilsTest2 = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtilsTest2, nullptr);
    Drawing::Canvas canvasTest2;
    RSPaintFilterCanvas paintFilterCanvasTest(&canvasTest2);
    std::shared_ptr<RSPaintFilterCanvas::CachedEffectData> effectData = nullptr;
    paintFilterCanvasTest.SetEffectData(effectData);
    rsPropertyDrawableUtilsTest2->DrawUseEffect(&paintFilterCanvasTest, UseEffectType::DEFAULT);
    effectData = std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    EXPECT_NE(effectData, nullptr);
    effectData->cachedImage_ = nullptr;
    paintFilterCanvasTest.SetEffectData(effectData);
    rsPropertyDrawableUtilsTest2->DrawUseEffect(&paintFilterCanvasTest, UseEffectType::DEFAULT);

    std::shared_ptr<Drawing::Image> cachedImage = std::make_shared<Drawing::Image>();
    EXPECT_NE(cachedImage, nullptr);
    effectData->cachedImage_ = cachedImage;
    paintFilterCanvasTest.SetEffectData(effectData);
    paintFilterCanvasTest.SetVisibleRect(Drawing::Rect(0.0f, 0.0f, 1.0f, 1.0f));
    rsPropertyDrawableUtilsTest2->DrawUseEffect(&paintFilterCanvasTest, UseEffectType::DEFAULT);
}

/**
 * @tc.name: IsDangerousBlendModeAndEndBlenderTest
 * @tc.desc: IsDangerousBlendMode and EndBlender test
 * @tc.type: FUNC
 * @tc.require:issueIB7R0A
 */
HWTEST_F(RSPropertyDrawableUtilsTest, IsDangerousBlendModeAndEndBlenderTest, testing::ext::TestSize.Level1)
{
    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtilsTest1 = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtilsTest1, nullptr);
    EXPECT_EQ(rsPropertyDrawableUtilsTest1->IsDangerousBlendMode(1, 0), 0);
    EXPECT_EQ(rsPropertyDrawableUtilsTest1->IsDangerousBlendMode(1, 1), 1);

    Drawing::Canvas canvasTest;
    RSPaintFilterCanvas paintFilterCanvasTest1(&canvasTest);
    rsPropertyDrawableUtilsTest1->EndBlender(paintFilterCanvasTest1, 0);
    rsPropertyDrawableUtilsTest1->EndBlender(paintFilterCanvasTest1, 1);
}

/**
 * @tc.name: GetColorForShadowSynTest
 * @tc.desc: GetColorForShadowSyn and GpuScaleImage test
 * @tc.type: FUNC
 * @tc.require:issueIA5Y41
 */
HWTEST_F(RSPropertyDrawableUtilsTest, GetColorForShadowSynTest, testing::ext::TestSize.Level1)
{
    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtilsTest = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtilsTest, nullptr);

    Drawing::Canvas canvasTest1;
    Drawing::Path path;
    Color color;
    RSPaintFilterCanvas paintFilterCanvas(&canvasTest1);
    paintFilterCanvas.surface_ = nullptr;
    rsPropertyDrawableUtilsTest->GetColorForShadowSyn(&paintFilterCanvas, path, color, false);
    path.AddRect({0, 0, 5, 5});
    auto surface = Drawing::Surface::MakeRasterN32Premul(10, 10);
    paintFilterCanvas.surface_ = surface.get();
    rsPropertyDrawableUtilsTest->GetColorForShadowSyn(&paintFilterCanvas, path, color, false);

    Drawing::Canvas canvasTest2;
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    EXPECT_NE(image, nullptr);
    std::shared_ptr<Drawing::SkiaImage> imageImpl = std::make_shared<Drawing::SkiaImage>();
    image->imageImplPtr = imageImpl;
    rsPropertyDrawableUtilsTest->GpuScaleImage(&canvasTest2, image);
}

/**
 * @tc.name: GetInvertedBackgroundColorTest
 * @tc.desc: BeginBlender and GetInvertBackgroundColor test
 * @tc.type: FUNC
 * @tc.require:issueIB7R0A
 */
HWTEST_F(RSPropertyDrawableUtilsTest, GetInvertedBackgroundColorTest, testing::ext::TestSize.Level1)
{
    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtilsTest1 = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtilsTest1, nullptr);
    Drawing::Canvas canvasTest1;
    RSPaintFilterCanvas paintFilterCanvasTest1(&canvasTest1);
    paintFilterCanvasTest1.envStack_.top().hasOffscreenLayer_ = false;
    std::shared_ptr<Drawing::Blender> blender = std::make_shared<Drawing::Blender>();
    EXPECT_NE(blender, nullptr);

    rsPropertyDrawableUtilsTest1->BeginBlender(paintFilterCanvasTest1, blender, 0, true);
    rsPropertyDrawableUtilsTest1->BeginBlender(paintFilterCanvasTest1, blender, 1, true);

    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtilsTest2 = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtilsTest2, nullptr);
    Drawing::Canvas canvasTest2;
    RSPaintFilterCanvas paintFilterCanvasTest2(&canvasTest2);
    Color backgroundColor;
    backgroundColor.alpha_ = 0xff;
    EXPECT_EQ(rsPropertyDrawableUtilsTest2->GetInvertBackgroundColor(
                  paintFilterCanvasTest2, false, Vector4f(0.0f, 0.0f, 0.0f, 1.0f), backgroundColor),
        RSPropertyDrawableUtils::CalculateInvertColor(backgroundColor));

    backgroundColor.alpha_ = 0;
    Drawing::Surface surfaceTest2;
    paintFilterCanvasTest2.surface_ = &surfaceTest2;
    rsPropertyDrawableUtilsTest2->GetInvertBackgroundColor(
        paintFilterCanvasTest2, false, Vector4f(0.0f, 0.0f, 0.0f, 1.0f), backgroundColor);
}

/**
 * @tc.name: GetShadowRegionImageTest
 * @tc.desc: GetShadowRegionImage test
 * @tc.type: FUNC
 * @tc.require:issueIA5Y41
 */
HWTEST_F(RSPropertyDrawableUtilsTest, GetShadowRegionImageTest, testing::ext::TestSize.Level1)
{
    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtilsTest = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtilsTest, nullptr);

    Drawing::Canvas canvasTest;
    RSPaintFilterCanvas paintFilterCanvas(&canvasTest);
    Drawing::Path drPath;
    Drawing::Matrix matrix;
    auto resultTest1 = rsPropertyDrawableUtilsTest->GetShadowRegionImage(&paintFilterCanvas, drPath, matrix);
    auto surface = Drawing::Surface::MakeRasterN32Premul(10, 10);
    paintFilterCanvas.surface_ = surface.get();
    auto resultTest2 = rsPropertyDrawableUtilsTest->GetShadowRegionImage(&paintFilterCanvas, drPath, matrix);
    drPath.AddRect({0, 0, 5, 5});
    auto resultTest3 = rsPropertyDrawableUtilsTest->GetShadowRegionImage(&paintFilterCanvas, drPath, matrix);
    EXPECT_NE(resultTest3, nullptr);
}

/**
 * @tc.name: DrawShadowMaskFilterTest
 * @tc.desc: DrawShadowMaskFilter test
 * @tc.type: FUNC
 * @tc.require:issueIA5Y41
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawShadowMaskFilterTest, testing::ext::TestSize.Level1)
{
    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtilsTest = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtilsTest, nullptr);
    Drawing::Canvas canvasTest;
    Drawing::Path path;
    path.AddRect({0, 0, 5, 5});
    rsPropertyDrawableUtilsTest->DrawShadowMaskFilter(&canvasTest, path, 1.f, 1.f, 1.f, false,
        Color(255, 255, 255, 255));
    ASSERT_TRUE(true);
}

/**
 * @tc.name: RSFilterSetPixelStretchTest
 * @tc.desc: RSFilterSetPixelStretch test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyDrawableUtilsTest, RSFilterSetPixelStretchTest, testing::ext::TestSize.Level1)
{
    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtils = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtils, nullptr);

    RSProperties properties;
    std::shared_ptr<RSFilter> rsFilter = nullptr;
    EXPECT_FALSE(rsPropertyDrawableUtils->RSFilterSetPixelStretch(properties, rsFilter));

    std::shared_ptr<RSShaderFilter> rsFilterTest = std::make_shared<RSShaderFilter>();
    std::shared_ptr<RSDrawingFilter> filter = std::make_shared<RSDrawingFilter>(rsFilterTest);
    EXPECT_FALSE(rsPropertyDrawableUtils->RSFilterSetPixelStretch(properties, filter));

    std::shared_ptr<RSShaderFilter> rsFilterTest2 = std::make_shared<RSShaderFilter>();
    rsFilterTest2->type_ = RSShaderFilter::MESA;
    std::shared_ptr<RSDrawingFilter> filter2 = std::make_shared<RSDrawingFilter>(rsFilterTest2);
    EXPECT_FALSE(rsPropertyDrawableUtils->RSFilterSetPixelStretch(properties, filter2));

    // 10: blur radius
    int radius = 10;
    std::shared_ptr<RSMESABlurShaderFilter> mesaFilter = std::make_shared<RSMESABlurShaderFilter>(radius);
    std::shared_ptr<RSDrawingFilter> filter3 = std::make_shared<RSDrawingFilter>(mesaFilter);

    // -1.0f: stretch offset param
    Vector4f pixelStretchTest(-1.0f, -1.0f, -1.0f, -1.0f);
    properties.pixelStretch_ = pixelStretchTest;
    EXPECT_TRUE(rsPropertyDrawableUtils->RSFilterSetPixelStretch(properties, filter3));

    // 1.0f: stretch offset param
    Vector4f pixelStretchTest2(1.0f, 1.0f, 1.0f, 1.0f);
    properties.pixelStretch_ = pixelStretchTest2;
    EXPECT_FALSE(rsPropertyDrawableUtils->RSFilterSetPixelStretch(properties, filter3));
}

/**
 * @tc.name: RSFilterRemovePixelStretchTest
 * @tc.desc: RSFilterRemovePixelStretch test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyDrawableUtilsTest, RSFilterRemovePixelStretchTest, testing::ext::TestSize.Level1)
{
    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtils = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtils, nullptr);

    std::shared_ptr<RSFilter> rsFilter = nullptr;
    rsPropertyDrawableUtils->RSFilterRemovePixelStretch(rsFilter);

    std::shared_ptr<RSShaderFilter> rsFilterTest = std::make_shared<RSShaderFilter>();
    std::shared_ptr<RSDrawingFilter> filter = std::make_shared<RSDrawingFilter>(rsFilterTest);
    rsPropertyDrawableUtils->RSFilterRemovePixelStretch(filter);

    // 10: blur radius
    int radius = 10;
    std::shared_ptr<RSMESABlurShaderFilter> mesaFilter = std::make_shared<RSMESABlurShaderFilter>(radius);
    std::shared_ptr<RSDrawingFilter> filter2 = std::make_shared<RSDrawingFilter>(mesaFilter);
    rsPropertyDrawableUtils->RSFilterRemovePixelStretch(filter2);
}

/**
 * @tc.name: GetGravityMatrixTest
 * @tc.desc: GetGravityMatrix test
 * @tc.type: FUNC
 * @tc.require:issueIA5Y41
 */
HWTEST_F(RSPropertyDrawableUtilsTest, GetGravityMatrixTest, testing::ext::TestSize.Level1)
{
    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtilsTest = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtilsTest, nullptr);
    Drawing::Rect rect(0, 0, 10, 10);
    Drawing::Rect rectTwo(0, 0, 0, 10);
    Drawing::Rect rectThree(0, 0, 0, 0.0000001);
    Drawing::Matrix matrix;
    ASSERT_FALSE(rsPropertyDrawableUtilsTest->GetGravityMatrix(Gravity::CENTER, rect, 10, 10, matrix));
    ASSERT_TRUE(rsPropertyDrawableUtilsTest->GetGravityMatrix(Gravity::CENTER, rect, 20, 20, matrix));
    ASSERT_TRUE(rsPropertyDrawableUtilsTest->GetGravityMatrix(Gravity::TOP, rect, 20, 20, matrix));
    ASSERT_TRUE(rsPropertyDrawableUtilsTest->GetGravityMatrix(Gravity::BOTTOM, rect, 20, 20, matrix));
    ASSERT_TRUE(rsPropertyDrawableUtilsTest->GetGravityMatrix(Gravity::LEFT, rect, 20, 20, matrix));
    ASSERT_TRUE(rsPropertyDrawableUtilsTest->GetGravityMatrix(Gravity::RIGHT, rect, 20, 20, matrix));
    ASSERT_FALSE(rsPropertyDrawableUtilsTest->GetGravityMatrix(Gravity::TOP_LEFT, rect, 20, 20, matrix));
    ASSERT_TRUE(rsPropertyDrawableUtilsTest->GetGravityMatrix(Gravity::TOP_RIGHT, rect, 20, 20, matrix));
    ASSERT_TRUE(rsPropertyDrawableUtilsTest->GetGravityMatrix(Gravity::BOTTOM_LEFT, rect, 20, 20, matrix));
    ASSERT_TRUE(rsPropertyDrawableUtilsTest->GetGravityMatrix(Gravity::BOTTOM_RIGHT, rect, 20, 20, matrix));
    ASSERT_FALSE(rsPropertyDrawableUtilsTest->GetGravityMatrix(Gravity::RESIZE, rect, 0, 0, matrix));
    ASSERT_TRUE(rsPropertyDrawableUtilsTest->GetGravityMatrix(Gravity::RESIZE, rect, 20, 20, matrix));
    ASSERT_FALSE(rsPropertyDrawableUtilsTest->GetGravityMatrix(Gravity::RESIZE_ASPECT, rect, 0, 0, matrix));
    ASSERT_FALSE(rsPropertyDrawableUtilsTest->GetGravityMatrix(Gravity::RESIZE_ASPECT, rectTwo, 10, 10, matrix));
    ASSERT_TRUE(rsPropertyDrawableUtilsTest->GetGravityMatrix(Gravity::RESIZE_ASPECT, rect, 20, 20, matrix));
    ASSERT_FALSE(rsPropertyDrawableUtilsTest->GetGravityMatrix(Gravity::RESIZE_ASPECT_TOP_LEFT, rect, 0, 0, matrix));
    ASSERT_TRUE(rsPropertyDrawableUtilsTest->GetGravityMatrix(Gravity::RESIZE_ASPECT_TOP_LEFT, rect, 20, 20, matrix));
    ASSERT_FALSE(
        rsPropertyDrawableUtilsTest->GetGravityMatrix(Gravity::RESIZE_ASPECT_BOTTOM_RIGHT, rect, 0, 0, matrix));
    ASSERT_FALSE(rsPropertyDrawableUtilsTest->GetGravityMatrix(
        Gravity::RESIZE_ASPECT_BOTTOM_RIGHT, rectTwo, 10, 10, matrix));
    ASSERT_TRUE(
        rsPropertyDrawableUtilsTest->GetGravityMatrix(Gravity::RESIZE_ASPECT_BOTTOM_RIGHT, rect, 20, 20, matrix));
    ASSERT_FALSE(rsPropertyDrawableUtilsTest->GetGravityMatrix(Gravity::RESIZE_ASPECT_FILL, rect, 0, 0, matrix));
    ASSERT_FALSE(rsPropertyDrawableUtilsTest->GetGravityMatrix(
        Gravity::RESIZE_ASPECT_FILL, rectThree, 10, 100000000, matrix));
    ASSERT_TRUE(rsPropertyDrawableUtilsTest->GetGravityMatrix(Gravity::RESIZE_ASPECT_FILL, rect, 20, 20, matrix));
    ASSERT_FALSE(
        rsPropertyDrawableUtilsTest->GetGravityMatrix(Gravity::RESIZE_ASPECT_FILL_TOP_LEFT, rect, 0, 0, matrix));
    ASSERT_TRUE(
        rsPropertyDrawableUtilsTest->GetGravityMatrix(Gravity::RESIZE_ASPECT_FILL_TOP_LEFT, rect, 20, 20, matrix));
    ASSERT_FALSE(
        rsPropertyDrawableUtilsTest->GetGravityMatrix(Gravity::RESIZE_ASPECT_FILL_BOTTOM_RIGHT, rect, 0, 0, matrix));
    ASSERT_FALSE(rsPropertyDrawableUtilsTest->GetGravityMatrix(
        Gravity::RESIZE_ASPECT_FILL_BOTTOM_RIGHT, rectThree, 10, 100000000, matrix));
    ASSERT_TRUE(
        rsPropertyDrawableUtilsTest->GetGravityMatrix(Gravity::RESIZE_ASPECT_FILL_BOTTOM_RIGHT, rect, 20, 20, matrix));
    ASSERT_FALSE(rsPropertyDrawableUtilsTest->GetGravityMatrix(static_cast<Gravity>(22), rect, 20, 20, matrix));
}

/**
 * @tc.name: DrawBackgroundEffectTest
 * @tc.desc: DrawBackgroundEffectTest behindWindow branch
 * @tc.type: FUNC
 * @tc.require: issueIB0UQV
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawBackgroundEffectTest, testing::ext::TestSize.Level1)
{
    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtils = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtils, nullptr);
    // GetAndResetBlurCnt test
    rsPropertyDrawableUtils->g_blurCnt = 0;
    // DrawBackgroundEffect test
    Drawing::Canvas canvasTest1;
    RSPaintFilterCanvas paintFilterCanvas(&canvasTest1);
    std::shared_ptr<RSFilter> rsFilter = nullptr;
    std::unique_ptr<RSFilterCacheManager> cacheManager = std::make_unique<RSFilterCacheManager>();
    Drawing::RectI bounds(0, 0, 400, 400);
    rsPropertyDrawableUtils->DrawBackgroundEffect(&paintFilterCanvas, rsFilter, cacheManager, false, bounds, true);
    rsFilter = std::make_shared<RSFilter>();
    rsFilter->type_ = RSFilter::NONE;
    paintFilterCanvas.surface_ = nullptr;
    rsPropertyDrawableUtils->DrawBackgroundEffect(&paintFilterCanvas, rsFilter, cacheManager, false, bounds, true);
    Drawing::Surface surface;
    paintFilterCanvas.surface_ = &surface;
    Drawing::Canvas canvasTest2;
    paintFilterCanvas.canvas_ = &canvasTest2;
    rsPropertyDrawableUtils->DrawBackgroundEffect(&paintFilterCanvas, rsFilter, cacheManager, false, bounds, true);
    paintFilterCanvas.SetDisableFilterCache(true);
    rsPropertyDrawableUtils->DrawBackgroundEffect(&paintFilterCanvas, rsFilter, cacheManager, false, bounds, true);
    int lastBlurCnt = rsPropertyDrawableUtils->g_blurCnt;
    ASSERT_NE(lastBlurCnt, 0);
    rsPropertyDrawableUtils->DrawBackgroundEffect(nullptr, rsFilter, cacheManager, false, bounds);
    ASSERT_EQ(lastBlurCnt, lastBlurCnt);
}

/**
 * @tc.name: GenerateBehindWindowFilterTest
 * @tc.desc: GenerateBehindWindowFilterTest
 * @tc.type: FUNC
 * @tc.require: issueIB0UQV
 */
HWTEST_F(RSPropertyDrawableUtilsTest, GenerateBehindWindowFilterTest, testing::ext::TestSize.Level1)
{
    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtilsTest1 = std::make_shared<RSPropertyDrawableUtils>();
    ASSERT_NE(rsPropertyDrawableUtilsTest1, nullptr);
    float radius = 80.0f;
    float saturation = 1.9f;
    float brightness = 1.0f;
    RSColor maskColor(0xFFFFFFE5);
    auto filter = rsPropertyDrawableUtilsTest1->GenerateBehindWindowFilter(radius, saturation, brightness, maskColor);
    ASSERT_NE(filter, nullptr);
}


/**
 * @tc.name: DrawUseEffectTest
 * @tc.desc: DrawUseEffectTest behindWindow branch
 * @tc.type: FUNC
 * @tc.require: issueIB0UQV
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawUseEffectTest, testing::ext::TestSize.Level1)
{
    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtilsTest = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtilsTest, nullptr);
    Drawing::Canvas canvasTest;
    RSPaintFilterCanvas paintFilterCanvasTest(&canvasTest);
    std::shared_ptr<RSPaintFilterCanvas::CachedEffectData> effectData = nullptr;
    paintFilterCanvasTest.SetBehindWindowData(effectData);
    rsPropertyDrawableUtilsTest->DrawUseEffect(&paintFilterCanvasTest, UseEffectType::BEHIND_WINDOW);
    effectData = std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    EXPECT_NE(effectData, nullptr);
    effectData->cachedImage_ = nullptr;
    paintFilterCanvasTest.SetBehindWindowData(effectData);
    rsPropertyDrawableUtilsTest->DrawUseEffect(&paintFilterCanvasTest, UseEffectType::BEHIND_WINDOW);

    std::shared_ptr<Drawing::Image> cachedImage = std::make_shared<Drawing::Image>();
    EXPECT_NE(cachedImage, nullptr);
    effectData->cachedImage_ = cachedImage;
    paintFilterCanvasTest.SetBehindWindowData(effectData);
    paintFilterCanvasTest.SetVisibleRect(Drawing::Rect(0.0f, 0.0f, 1.0f, 1.0f));
    rsPropertyDrawableUtilsTest->DrawUseEffect(&paintFilterCanvasTest, UseEffectType::BEHIND_WINDOW);
}

/**
 * @tc.name: GenerateMaterialColorFilterTest
 * @tc.desc: GenerateMaterialColorFilterTest
 * @tc.type: FUNC
 * @tc.require: issueIB0UQV
 */
HWTEST_F(RSPropertyDrawableUtilsTest, GenerateMaterialColorFilterTest, testing::ext::TestSize.Level1)
{
    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtilsTest1 = std::make_shared<RSPropertyDrawableUtils>();
    ASSERT_NE(rsPropertyDrawableUtilsTest1, nullptr);
    float saturation = 1.9f;
    float brightness = 1.0f;
    auto filter = rsPropertyDrawableUtilsTest1->GenerateMaterialColorFilter(saturation, brightness);
    ASSERT_NE(filter, nullptr);
}
} // namespace OHOS::Rosen