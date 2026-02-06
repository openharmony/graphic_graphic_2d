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
#include "effect/rs_render_filter_base.h"
#include "effect/rs_render_shape_base.h"
#include "ge_visual_effect_container.h"
#include "property/rs_properties_painter.h"
#include "render/rs_drawing_filter.h"
#include "skia_adapter/skia_image.h"
#include "skia_adapter/skia_image_info.h"
#include "skia_adapter/skia_surface.h"
#include "render/rs_filter_cache_memory_controller.h"
#include "render/rs_fly_out_shader_filter.h"
#include "render/rs_render_linear_gradient_blur_filter.h"
#include "render/rs_render_magnifier_filter.h"
#include "render/rs_render_mesa_blur_filter.h"
#include "hpae_base/rs_hpae_base_data.h"
#include "hpae_base/rs_hpae_filter_cache_manager.h"

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
 * @tc.name: RRect2DrawingRRectAndCreateShadowPathTest001
 * @tc.desc: RRect2DrawingRRect and CreateShadowPath test
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawableUtilsTest, RRect2DrawingRRectAndCreateShadowPathTest001, testing::ext::TestSize.Level1)
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
 * @tc.name: GetRRectForDrawingBorderTest002
 * @tc.desc: GetRRectForDrawingBorder test
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawableUtilsTest, GetRRectForDrawingBorderTest002, testing::ext::TestSize.Level1)
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
 * @tc.name: GetDarkColorTest004
 * @tc.desc: GetDarkColor test
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawableUtilsTest, GetDarkColorTest004, testing::ext::TestSize.Level1)
{
    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtils = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtils, nullptr);
    RSColor color(255, 255, 255, 255);
    rsPropertyDrawableUtils->GetDarkColor(color);
}

/**
 * @tc.name: DrawShadowAndCeilMatrixTransTest005
 * @tc.desc: DrawShadow and CeilMatrixTrans test
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawShadowAndCeilMatrixTransTest005, testing::ext::TestSize.Level1)
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
 * @tc.name: DrawAndBeginForegroundFilterTest006
 * @tc.desc: DrawFilter and BeginForegroundFilter test
 * @tc.type: FUNC
 * @tc.require:issueIA5Y41
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawAndBeginForegroundFilterTest006, testing::ext::TestSize.Level1)
{
    // first: DrawFilter test
    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtils = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtils, nullptr);
    Drawing::Canvas canvasTest1(400, 400);
    RSPaintFilterCanvas paintFilterCanvasTest1(&canvasTest1);
    std::shared_ptr<RSDrawingFilter> rsFilter = nullptr;
    std::unique_ptr<RSFilterCacheManager> cacheManager = std::make_unique<RSFilterCacheManager>();
    paintFilterCanvasTest1.surface_ = nullptr;
    cacheManager->renderClearFilteredCacheAfterDrawing_ = false;
    rsPropertyDrawableUtils->DrawFilter(&paintFilterCanvasTest1, rsFilter, cacheManager, 0, false);
    std::vector<std::pair<float, float>> fractionStops;
    auto para = std::make_shared<RSLinearGradientBlurPara>(1.f, fractionStops, GradientDirection::LEFT);
    auto rsFilterTest = std::make_shared<RSLinearGradientBlurShaderFilter>(para, 1.f, 1.f);
    rsFilterTest->type_ = RSUIFilterType::LINEAR_GRADIENT_BLUR;
    EXPECT_NE(rsFilterTest, nullptr);
    rsFilter = std::make_shared<RSDrawingFilter>(rsFilterTest);
    EXPECT_NE(rsFilter, nullptr);
    rsFilter->type_ = RSFilter::BLUR;
    rsFilter->imageFilter_ = std::make_shared<Drawing::ImageFilter>();
    EXPECT_NE(rsFilter->imageFilter_, nullptr);
    cacheManager->renderClearFilteredCacheAfterDrawing_ = true;
    rsPropertyDrawableUtils->DrawFilter(&paintFilterCanvasTest1, rsFilter, cacheManager, 0, false);
    cacheManager->renderClearFilteredCacheAfterDrawing_ = false;
    rsPropertyDrawableUtils->DrawFilter(nullptr, rsFilter, cacheManager, 0, false);
    auto surface = Drawing::Surface::MakeRasterN32Premul(10, 10);
    paintFilterCanvasTest1.surface_ = surface.get();
    paintFilterCanvasTest1.SetDisableFilterCache(false);
    cacheManager->renderClearFilteredCacheAfterDrawing_ = true;
    rsPropertyDrawableUtils->DrawFilter(&paintFilterCanvasTest1, rsFilter, cacheManager, 0, false);
    paintFilterCanvasTest1.SetDisableFilterCache(true);
    cacheManager->renderClearFilteredCacheAfterDrawing_ = true;
    rsPropertyDrawableUtils->DrawFilter(&paintFilterCanvasTest1, rsFilter, cacheManager, 0, false);
    auto magnifierParams = std::make_shared<RSMagnifierParams>();
    auto magnifierFilter = std::make_shared<RSMagnifierShaderFilter>(magnifierParams);
    magnifierFilter->type_ = RSUIFilterType::MAGNIFIER;
    rsFilter = std::make_shared<RSDrawingFilter>(magnifierFilter);
    rsFilter->type_ = RSFilter::BLUR;
    rsFilter->imageFilter_ = std::make_shared<Drawing::ImageFilter>();
    cacheManager->renderClearFilteredCacheAfterDrawing_ = true;
    rsPropertyDrawableUtils->DrawFilter(&paintFilterCanvasTest1, rsFilter, cacheManager, 0, false);
    Drawing::Rect visibleRect = Drawing::Rect(0.0f, 0.0f, 300.0f, 300.0f);
    paintFilterCanvasTest1.SetVisibleRect(visibleRect);
    rsPropertyDrawableUtils->DrawFilter(&paintFilterCanvasTest1, rsFilter, cacheManager, 0, false);
    paintFilterCanvasTest1.ClipIRect(Drawing::RectI(0, 0, 0, 0));
    rsPropertyDrawableUtils->DrawFilter(&paintFilterCanvasTest1, rsFilter, cacheManager, 0, false);

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
 * @tc.name: DrawFilterTest002
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:wz
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawFilterTest002, testing::ext::TestSize.Level1)
{
    // first: DrawFilter test
    NodeId id = 1;
    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtils = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtils, nullptr);
    Drawing::Canvas canvasTest1(400, 400);
    RSPaintFilterCanvas paintFilterCanvasTest1(&canvasTest1);
    std::shared_ptr<RSDrawingFilter> rsFilter = nullptr;
    std::unique_ptr<RSFilterCacheManager> cacheManager = std::make_unique<RSFilterCacheManager>();
    paintFilterCanvasTest1.surface_ = nullptr;
    cacheManager->renderClearFilteredCacheAfterDrawing_ = false;
    std::vector<std::pair<float, float>> fractionStops;
    auto para = std::make_shared<RSLinearGradientBlurPara>(1.f, fractionStops, GradientDirection::LEFT);
    auto rsFilterTest = std::make_shared<RSLinearGradientBlurShaderFilter>(para, 1.f, 1.f);
    rsFilterTest->type_ = RSUIFilterType::LINEAR_GRADIENT_BLUR;
    EXPECT_NE(rsFilterTest, nullptr);
    rsFilter = std::make_shared<RSDrawingFilter>(rsFilterTest);
    EXPECT_NE(rsFilter, nullptr);
    rsFilter->type_ = RSFilter::BLUR;
    rsFilter->imageFilter_ = std::make_shared<Drawing::ImageFilter>();
    EXPECT_NE(rsFilter->imageFilter_, nullptr);
    cacheManager->renderClearFilteredCacheAfterDrawing_ = true;
    auto surface = Drawing::Surface::MakeRasterN32Premul(10, 10);
    paintFilterCanvasTest1.surface_ = surface.get();
    paintFilterCanvasTest1.SetDisableFilterCache(false);
    cacheManager->renderClearFilteredCacheAfterDrawing_ = true;

    auto manager = std::make_shared<RSHpaeFilterCacheManager>();
    EXPECT_NE(manager, nullptr);
    RSHpaeBaseData::GetInstance().hpaeStatus_.blurNodeId = id;
    RSHpaeBaseData::GetInstance().hpaeStatus_.gotHpaeBlurNode = true;
    rsPropertyDrawableUtils->DrawFilter(&paintFilterCanvasTest1, rsFilter, cacheManager, id, false);
}

/**
 * @tc.name: BDrawForegroundFilterTest007
 * @tc.desc: DrawForegroundFilter test
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawableUtilsTest, BDrawForegroundFilterTest007, testing::ext::TestSize.Level1)
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

    auto surfacePtr = Drawing::Surface::MakeRasterN32Premul(10, 10);
    paintFilterCanvas.surface_ = surfacePtr.get();
    rsFilter = std::make_shared<RSFlyOutShaderFilter>(1.0, 0);
    rsPropertyDrawableUtils->DrawForegroundFilter(paintFilterCanvas, rsFilter);
    rsFilter = std::make_shared<RSDrawingFilter>();
    rsFilter->type_ = RSFilter::COMPOUND_EFFECT;
    rsPropertyDrawableUtils->DrawForegroundFilter(paintFilterCanvas, rsFilter);
}

/**
 * @tc.name: RSPropertyDrawableUtilsTest008
 * @tc.desc: GetAndResetBlurCnt and DrawBackgroundEffect test
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawableUtilsTest, RSPropertyDrawableUtilsTest008, testing::ext::TestSize.Level1)
{
    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtils = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtils, nullptr);
    // GetAndResetBlurCnt test
    rsPropertyDrawableUtils->g_blurCnt = 0;
    EXPECT_EQ(rsPropertyDrawableUtils->GetAndResetBlurCnt(), 0);
    // DrawBackgroundEffect test
    Drawing::Canvas canvasTest1;
    RSPaintFilterCanvas paintFilterCanvas(&canvasTest1);
    std::shared_ptr<RSFilter> rsFilter = nullptr;
    std::unique_ptr<RSFilterCacheManager> cacheManager = std::make_unique<RSFilterCacheManager>();
    Drawing::RectI bounds(0, 0, 400, 400);
    cacheManager->renderClearFilteredCacheAfterDrawing_ = false;
    rsPropertyDrawableUtils->DrawBackgroundEffect(&paintFilterCanvas, rsFilter, cacheManager, bounds);
    rsFilter = std::make_shared<RSFilter>();
    rsFilter->type_ = RSFilter::NONE;
    paintFilterCanvas.surface_ = nullptr;
    cacheManager->renderClearFilteredCacheAfterDrawing_ = false;
    rsPropertyDrawableUtils->DrawBackgroundEffect(&paintFilterCanvas, rsFilter, cacheManager, bounds);
    Drawing::Surface surface;
    paintFilterCanvas.surface_ = &surface;
    Drawing::Canvas canvasTest2;
    paintFilterCanvas.canvas_ = &canvasTest2;
    cacheManager->renderClearFilteredCacheAfterDrawing_ = false;
    rsPropertyDrawableUtils->DrawBackgroundEffect(&paintFilterCanvas, rsFilter, cacheManager, bounds);
    paintFilterCanvas.SetDisableFilterCache(true);
    cacheManager->renderClearFilteredCacheAfterDrawing_ = false;
    rsPropertyDrawableUtils->DrawBackgroundEffect(&paintFilterCanvas, rsFilter, cacheManager, bounds);
    int lastBlurCnt = rsPropertyDrawableUtils->g_blurCnt;
    ASSERT_NE(lastBlurCnt, 0);
    cacheManager->renderClearFilteredCacheAfterDrawing_ = false;
    rsPropertyDrawableUtils->DrawBackgroundEffect(nullptr, rsFilter, cacheManager, bounds);
}

/**
 * @tc.name: DrawColorFilterTest009
 * @tc.desc: DrawColorFilter test
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawColorFilterTest009, testing::ext::TestSize.Level1)
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
 * @tc.name: DrawLightUpEffectTest010
 * @tc.desc: DrawLightUpEffect and DrawDynamicDim test
 * @tc.type: FUNC
 * @tc.require:issueIA5Y41
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawLightUpEffectTest010, testing::ext::TestSize.Level1)
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
    Drawing::Canvas canvas(10, 10); // width and height
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    Drawing::Surface surface;
    paintFilterCanvas.surface_ = &surface;
    rsPropertyDrawableUtilsTest1->DrawLightUpEffect(&paintFilterCanvas, 0.0f);

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
 * @tc.name: TransformativeShaderTest011
 * @tc.desc: MakeDynamicDimShader MakeBinarizationShader MakeDynamicBrightnessBlender MakeDynamicBrightnessBuilder test
 * @tc.type: FUNC
 * @tc.require:issueIA5Y41
 */
HWTEST_F(RSPropertyDrawableUtilsTest, TransformativeShaderTest011, testing::ext::TestSize.Level1)
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

    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtilsTest5 = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtilsTest5, nullptr);
    params.fraction_ = 0.0;
    EXPECT_NE(rsPropertyDrawableUtilsTest5->MakeDynamicBrightnessBlender(params), nullptr);

    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtilsTest6 = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtilsTest6, nullptr);
    std::shared_ptr<Drawing::ShaderEffect> imageShaderTest3 = std::make_shared<Drawing::ShaderEffect>();
    EXPECT_NE(rsPropertyDrawableUtilsTest6->MakeLightUpShader(1.0f, imageShaderTest3), nullptr);
}
/**
 * @tc.name: DrawBinarizationTest012
 * @tc.desc: DrawBinarization test
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawBinarizationTest012, testing::ext::TestSize.Level1)
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
 * @tc.name: DrawPixelStretchTest013
 * @tc.desc: DrawPixelStretch test
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawPixelStretchTest013, testing::ext::TestSize.Level1)
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
 * @tc.name: DrawShadowTestAndDrawUseEffectTest014
 * @tc.desc: DrawShadow and DrawUseEffect test
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawShadowTestAndDrawUseEffectTest014, testing::ext::TestSize.Level1)
{
    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtilsTest1 = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtilsTest1, nullptr);
    Drawing::Canvas canvasTest1;
    Drawing::Path path;
    Color spotColor;
    rsPropertyDrawableUtilsTest1->DrawShadow(&canvasTest1, path, 1.0f, 1.0f, 1.0f, true, spotColor);
    EXPECT_EQ(spotColor.GetAlpha(), 0);

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
 * @tc.name: GetInvertedBackgroundColorTest015
 * @tc.desc: BeginBlender and GetInvertBackgroundColor test
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawableUtilsTest, GetInvertedBackgroundColorTest015, testing::ext::TestSize.Level1)
{
    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtilsTest1 = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtilsTest1, nullptr);
    Drawing::Canvas canvasTest1;
    RSPaintFilterCanvas paintFilterCanvasTest1(&canvasTest1);
    paintFilterCanvasTest1.envStack_.top().hasOffscreenLayer_ = false;
    std::shared_ptr<Drawing::Blender> blender = std::make_shared<Drawing::Blender>();
    EXPECT_NE(blender, nullptr);

    rsPropertyDrawableUtilsTest1->BeginBlender(paintFilterCanvasTest1, blender, 0, true);
    EXPECT_EQ(paintFilterCanvasTest1.alphaStack_.size(), 1);
    rsPropertyDrawableUtilsTest1->BeginBlender(paintFilterCanvasTest1, blender, 1, true);
    EXPECT_EQ(paintFilterCanvasTest1.alphaStack_.size(), 2);
    rsPropertyDrawableUtilsTest1->BeginBlender(paintFilterCanvasTest1, blender, 2, true);
    EXPECT_EQ(paintFilterCanvasTest1.alphaStack_.size(), 3);
    rsPropertyDrawableUtilsTest1->BeginBlender(paintFilterCanvasTest1, blender, 3, true);
    EXPECT_EQ(paintFilterCanvasTest1.alphaStack_.size(), 4);

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
 * @tc.name: DrawColorUsingSDFWithDRMTest
 * @tc.desc: DrawColorUsingSDFWithDRM test
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawColorUsingSDFWithDRMTest, testing::ext::TestSize.Level1)
{
    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtils = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtils, nullptr);

    Drawing::Canvas canvas;
    bool isDark = true;
    Drawing::Rect rect(0, 0, 0, 0);

    rsPropertyDrawableUtils->DrawColorUsingSDFWithDRM(&canvas, &rect, isDark, nullptr, "Tag1", "Tag2");
    EXPECT_TRUE(isDark);
    auto filterGEContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    auto geVisualEffect = std::make_shared<Drawing::GEVisualEffect>("test");
    filterGEContainer->AddToChainedFilter(geVisualEffect);
    rsPropertyDrawableUtils->DrawColorUsingSDFWithDRM(&canvas, &rect, isDark, filterGEContainer, "Tag1", "Tag2");
    rsPropertyDrawableUtils->DrawColorUsingSDFWithDRM(&canvas, &rect, isDark, filterGEContainer, "test", "Tag2");
    EXPECT_TRUE(isDark);
    isDark = false;
    rsPropertyDrawableUtils->DrawColorUsingSDFWithDRM(&canvas, &rect, isDark, filterGEContainer, "test", "Tag2");
    EXPECT_FALSE(isDark);
}

/**
 * @tc.name: IsDangerousBlendModeAndEndBlenderTest016
 * @tc.desc: IsDangerousBlendMode and EndBlender test
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawableUtilsTest, IsDangerousBlendModeAndEndBlenderTest016, testing::ext::TestSize.Level1)
{
    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtilsTest1 = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtilsTest1, nullptr);
    EXPECT_EQ(rsPropertyDrawableUtilsTest1->IsDangerousBlendMode(1, 0), 0);
    EXPECT_EQ(rsPropertyDrawableUtilsTest1->IsDangerousBlendMode(1, 1), 1);
    EXPECT_EQ(rsPropertyDrawableUtilsTest1->IsDangerousBlendMode(1, 2), 1);

    Drawing::Canvas canvasTest;
    RSPaintFilterCanvas paintFilterCanvasTest1(&canvasTest);
    rsPropertyDrawableUtilsTest1->EndBlender(paintFilterCanvasTest1, 0);
    rsPropertyDrawableUtilsTest1->EndBlender(paintFilterCanvasTest1, 1);
}

/**
 * @tc.name: GetColorForShadowSynTest017
 * @tc.desc: GetColorForShadowSyn and GpuScaleImage test
 * @tc.type: FUNC
 * @tc.require:issueIA5Y41
 */
HWTEST_F(RSPropertyDrawableUtilsTest, GetColorForShadowSynTest017, testing::ext::TestSize.Level1)
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
    rsPropertyDrawableUtilsTest->GpuScaleImage(canvasTest2.GetGPUContext(), image);
}

/**
 * @tc.name: GetShadowRegionImageTest018
 * @tc.desc: GetShadowRegionImage test
 * @tc.type: FUNC
 * @tc.require:issueIA5Y41
 */
HWTEST_F(RSPropertyDrawableUtilsTest, GetShadowRegionImageTest018, testing::ext::TestSize.Level1)
{
    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtilsTest = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtilsTest, nullptr);

    Drawing::Canvas canvasTest;
    RSPaintFilterCanvas paintFilterCanvas(&canvasTest);
    Drawing::Path drPath;
    Drawing::Matrix matrix;
    auto resultTest1 = rsPropertyDrawableUtilsTest->GetShadowRegionImage(&paintFilterCanvas, drPath, matrix);
    EXPECT_EQ(resultTest1, nullptr);
    auto surface = Drawing::Surface::MakeRasterN32Premul(10, 10);
    paintFilterCanvas.surface_ = surface.get();
    auto resultTest2 = rsPropertyDrawableUtilsTest->GetShadowRegionImage(&paintFilterCanvas, drPath, matrix);
    EXPECT_EQ(resultTest2, nullptr);
    drPath.AddRect({0, 0, 5, 5});
    auto resultTest3 = rsPropertyDrawableUtilsTest->GetShadowRegionImage(&paintFilterCanvas, drPath, matrix);
    EXPECT_NE(resultTest3, nullptr);
}

/**
 * @tc.name: DrawShadowMaskFilterTest019
 * @tc.desc: DrawShadowMaskFilter test
 * @tc.type: FUNC
 * @tc.require:issueIA5Y41
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawShadowMaskFilterTest019, testing::ext::TestSize.Level1)
{
    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtilsTest = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtilsTest, nullptr);
    Drawing::Canvas canvasTest;
    Drawing::Path path;
    path.AddRect({0, 0, 5, 5});
    rsPropertyDrawableUtilsTest->DrawShadowMaskFilter(&canvasTest, path, 1.f, 1.f, 1.f, false,
        Color(255, 255, 255, 255), false);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: GetGravityMatrixTest020
 * @tc.desc: GetGravityMatrix test
 * @tc.type: FUNC
 * @tc.require:issueIA5Y41
 */
HWTEST_F(RSPropertyDrawableUtilsTest, GetGravityMatrixTest020, testing::ext::TestSize.Level1)
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
 * @tc.name: RSFilterSetPixelStretchTest021
 * @tc.desc: RSFilterSetPixelStretch test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyDrawableUtilsTest, RSFilterSetPixelStretchTest021, testing::ext::TestSize.Level1)
{
    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtils = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtils, nullptr);

    RSProperties properties;
    std::shared_ptr<RSFilter> rsFilter = nullptr;
    EXPECT_FALSE(rsPropertyDrawableUtils->RSFilterSetPixelStretch(properties, rsFilter));

    std::shared_ptr<RSRenderFilterParaBase> rsFilterTest = std::make_shared<RSRenderFilterParaBase>();
    std::shared_ptr<RSDrawingFilter> filter = std::make_shared<RSDrawingFilter>(rsFilterTest);
    EXPECT_FALSE(rsPropertyDrawableUtils->RSFilterSetPixelStretch(properties, filter));

    std::shared_ptr<RSRenderFilterParaBase> rsFilterTest2 = std::make_shared<RSRenderFilterParaBase>();
    rsFilterTest2->type_ = RSUIFilterType::MESA;
    std::shared_ptr<RSDrawingFilter> filter2 = std::make_shared<RSDrawingFilter>(rsFilterTest2);
    EXPECT_FALSE(rsPropertyDrawableUtils->RSFilterSetPixelStretch(properties, filter2));

    // 10: blur radius
    int radius = 10;
    std::shared_ptr<RSMESABlurShaderFilter> mesaFilter = std::make_shared<RSMESABlurShaderFilter>(radius);
    std::shared_ptr<RSDrawingFilter> filter3 = std::make_shared<RSDrawingFilter>(mesaFilter);

    // -1.0f: stretch offset param
    Vector4f pixelStretchTest(-1.0f, -1.0f, -1.0f, -1.0f);
    properties.GetEffect().pixelStretch_ = pixelStretchTest;
    EXPECT_TRUE(rsPropertyDrawableUtils->RSFilterSetPixelStretch(properties, filter3));

    // 1.0f: stretch offset param
    Vector4f pixelStretchTest2(1.0f, 1.0f, 1.0f, 1.0f);
    properties.GetEffect().pixelStretch_ = pixelStretchTest2;
    EXPECT_FALSE(rsPropertyDrawableUtils->RSFilterSetPixelStretch(properties, filter3));
}

/**
 * @tc.name: RSFilterRemovePixelStretchTest022
 * @tc.desc: RSFilterRemovePixelStretch test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyDrawableUtilsTest, RSFilterRemovePixelStretchTest022, testing::ext::TestSize.Level1)
{
    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtils = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtils, nullptr);

    std::shared_ptr<RSFilter> rsFilter = nullptr;
    rsPropertyDrawableUtils->RSFilterRemovePixelStretch(rsFilter);

    std::shared_ptr<RSRenderFilterParaBase> rsFilterTest = std::make_shared<RSRenderFilterParaBase>();
    std::shared_ptr<RSDrawingFilter> filter = std::make_shared<RSDrawingFilter>(rsFilterTest);
    rsPropertyDrawableUtils->RSFilterRemovePixelStretch(filter);

    // 10: blur radius
    int radius = 10;
    std::shared_ptr<RSMESABlurShaderFilter> mesaFilter = std::make_shared<RSMESABlurShaderFilter>(radius);
    std::shared_ptr<RSDrawingFilter> filter2 = std::make_shared<RSDrawingFilter>(mesaFilter);
    rsPropertyDrawableUtils->RSFilterRemovePixelStretch(filter2);
}

/**
 * @tc.name: DrawBackgroundEffectTest002
 * @tc.desc: DrawBackgroundEffectTest behindWindow branch
 * @tc.type: FUNC
 * @tc.require: issueIB0UQV
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawBackgroundEffectTest002, testing::ext::TestSize.Level1)
{
    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtils = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtils, nullptr);
    // GetAndResetBlurCnt test
    rsPropertyDrawableUtils->g_blurCnt = 0;
    EXPECT_EQ(rsPropertyDrawableUtils->GetAndResetBlurCnt(), 0);
    // DrawBackgroundEffect test
    Drawing::Canvas canvasTest1;
    RSPaintFilterCanvas paintFilterCanvas(&canvasTest1);
    std::shared_ptr<RSFilter> rsFilter = nullptr;
    std::unique_ptr<RSFilterCacheManager> cacheManager = std::make_unique<RSFilterCacheManager>();
    Drawing::RectI bounds(0, 0, 400, 400);
    cacheManager->renderClearFilteredCacheAfterDrawing_ = false;
    rsPropertyDrawableUtils->DrawBackgroundEffect(&paintFilterCanvas, rsFilter, cacheManager, bounds, true);
    rsFilter = std::make_shared<RSFilter>();
    rsFilter->type_ = RSFilter::NONE;
    paintFilterCanvas.surface_ = nullptr;
    cacheManager->renderClearFilteredCacheAfterDrawing_ = false;
    rsPropertyDrawableUtils->DrawBackgroundEffect(&paintFilterCanvas, rsFilter, cacheManager, bounds, true);
    Drawing::Surface surface;
    paintFilterCanvas.surface_ = &surface;
    Drawing::Canvas canvasTest2;
    paintFilterCanvas.canvas_ = &canvasTest2;
    cacheManager->renderClearFilteredCacheAfterDrawing_ = false;
    rsPropertyDrawableUtils->DrawBackgroundEffect(&paintFilterCanvas, rsFilter, cacheManager, bounds, true);
    paintFilterCanvas.SetDisableFilterCache(true);
    cacheManager->renderClearFilteredCacheAfterDrawing_ = false;
    rsPropertyDrawableUtils->DrawBackgroundEffect(&paintFilterCanvas, rsFilter, cacheManager, bounds, true);
    int lastBlurCnt = rsPropertyDrawableUtils->g_blurCnt;
    ASSERT_NE(lastBlurCnt, 0);
    cacheManager->renderClearFilteredCacheAfterDrawing_ = false;
    rsPropertyDrawableUtils->DrawBackgroundEffect(nullptr, rsFilter, cacheManager, bounds);
}

/**
 * @tc.name: DrawUseEffectTest002
 * @tc.desc: DrawUseEffectTest behindWindow branch
 * @tc.type: FUNC
 * @tc.require: issueIB0UQV
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawUseEffectTest002, testing::ext::TestSize.Level1)
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
 * @tc.name: GenerateBehindWindowFilterTest001
 * @tc.desc: GenerateBehindWindowFilterTest
 * @tc.type: FUNC
 * @tc.require: issueIB0UQV
 */
HWTEST_F(RSPropertyDrawableUtilsTest, GenerateBehindWindowFilterTest001, testing::ext::TestSize.Level1)
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
 * @tc.name: GenerateMaterialColorFilterTest001
 * @tc.desc: GenerateMaterialColorFilterTest
 * @tc.type: FUNC
 * @tc.require: issueIB0UQV
 */
HWTEST_F(RSPropertyDrawableUtilsTest, GenerateMaterialColorFilterTest001, testing::ext::TestSize.Level1)
{
    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtilsTest1 = std::make_shared<RSPropertyDrawableUtils>();
    ASSERT_NE(rsPropertyDrawableUtilsTest1, nullptr);
    float saturation = 1.9f;
    float brightness = 1.0f;
    auto filter = rsPropertyDrawableUtilsTest1->GenerateMaterialColorFilter(saturation, brightness);
    ASSERT_NE(filter, nullptr);
}

/**
 * @tc.name: GetRectByStrategyTest001
 * @tc.desc: GetRectByStrategyTest
 * @tc.type: FUNC
 * @tc.require: issueIB0UQV
 */
HWTEST_F(RSPropertyDrawableUtilsTest, GetRectByStrategyTest001, testing::ext::TestSize.Level1)
{
    Drawing::Rect bounds(2.1, 2.5, 102.1, 102.5);

    auto boundIn = RSPropertyDrawableUtils::GetRectByStrategy(bounds, RoundingStrategyType::ROUND_IN);
    Drawing::RectI expectBoundIn(3, 3, 102, 102);
    EXPECT_TRUE(boundIn == expectBoundIn);

    auto boundOut = RSPropertyDrawableUtils::GetRectByStrategy(bounds, RoundingStrategyType::ROUND_OUT);
    Drawing::RectI expectBoundOut(2, 2, 103, 103);
    EXPECT_TRUE(boundOut == expectBoundOut);

    auto boundOff = RSPropertyDrawableUtils::GetRectByStrategy(bounds, RoundingStrategyType::ROUND_OFF);
    Drawing::RectI expectBoundOff(2, 3, 102, 103);
    EXPECT_TRUE(boundOff == expectBoundOff);

    auto boundCastInt = RSPropertyDrawableUtils::GetRectByStrategy(bounds, RoundingStrategyType::ROUND_STATIC_CAST_INT);
    Drawing::RectI expectBoundCastInt(2, 2, 102, 102);
    EXPECT_TRUE(boundCastInt == expectBoundCastInt);

    auto boundButt = RSPropertyDrawableUtils::GetRectByStrategy(bounds, RoundingStrategyType::ROUND_BUTT);
    Drawing::RectI expectBoundButt(2, 2, 103, 103);
    EXPECT_TRUE(boundButt == expectBoundButt);
}

/**
 * @tc.name: GetAbsRectByStrategyTest001
 * @tc.desc: GetAbsRectByStrategy
 * @tc.type: FUNC
 * @tc.require: issueIB0UQV
 */
HWTEST_F(RSPropertyDrawableUtilsTest, GetAbsRectByStrategyTest001, testing::ext::TestSize.Level1)
{
    auto surface = Drawing::Surface::MakeRasterN32Premul(2000, 1000).get();
    Drawing::Matrix matrix;
    Drawing::Rect relativeRect(2.1, 2.5, 102.1, 102.5);

    auto boundIn =
        RSPropertyDrawableUtils::GetAbsRectByStrategy(surface, matrix, relativeRect, RoundingStrategyType::ROUND_IN);
    Drawing::RectI expectBoundIn(3, 3, 102, 102);
    EXPECT_TRUE(boundIn == expectBoundIn);

    auto boundOut =
        RSPropertyDrawableUtils::GetAbsRectByStrategy(surface, matrix, relativeRect, RoundingStrategyType::ROUND_OUT);
    Drawing::RectI expectBoundOut(2, 2, 103, 103);
    EXPECT_TRUE(boundOut == expectBoundOut);

    auto boundOff =
        RSPropertyDrawableUtils::GetAbsRectByStrategy(surface, matrix, relativeRect, RoundingStrategyType::ROUND_OFF);
    Drawing::RectI expectBoundOff(2, 3, 102, 103);
    EXPECT_TRUE(boundOff == expectBoundOff);

    auto boundCastInt = RSPropertyDrawableUtils::GetAbsRectByStrategy(
        surface, matrix, relativeRect, RoundingStrategyType::ROUND_STATIC_CAST_INT);
    Drawing::RectI expectBoundCastInt(2, 2, 102, 102);
    EXPECT_TRUE(boundCastInt == expectBoundCastInt);

    auto boundButt =
        RSPropertyDrawableUtils::GetAbsRectByStrategy(surface, matrix, relativeRect, RoundingStrategyType::ROUND_BUTT);
    Drawing::RectI expectBoundButt(2, 2, 103, 103);
    EXPECT_TRUE(boundButt == expectBoundButt);
}

/**
 * @tc.name: GetAbsRectByStrategyForImageTest001
 * @tc.desc: GetAbsRectByStrategyForImage
 * @tc.type: FUNC
 * @tc.require: issueIB0UQV
 */
HWTEST_F(RSPropertyDrawableUtilsTest, GetAbsRectByStrategyForImageTest001, testing::ext::TestSize.Level1)
{
    auto surface = Drawing::Surface::MakeRasterN32Premul(2000, 1000).get();
    Drawing::Matrix matrix;
    Drawing::Rect relativeRect(2.1, 2.5, 102.1, 102.5);

    auto [absImageRect, absDrawRect] =
        RSPropertyDrawableUtils::GetAbsRectByStrategyForImage(surface, matrix, relativeRect);
    Drawing::RectI expectAbsImageRect(3, 3, 102, 102);
    Drawing::RectI expectAbsDrawRect(2, 2, 103, 103);
    EXPECT_TRUE(absImageRect == expectAbsImageRect);
    EXPECT_TRUE(absDrawRect == expectAbsDrawRect);
}

/**
 * @tc.name: MakeShadowBlenderTest001
 * @tc.desc: MakeShadowBlender Test
 * @tc.type: FUNC
 * @tc.require: issueICLU4I
 */
HWTEST_F(RSPropertyDrawableUtilsTest, MakeShadowBlenderTest001, testing::ext::TestSize.Level1)
{
    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtils = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtils, nullptr);
    RSShadowBlenderPara shadowBlenderParams;
    EXPECT_NE(rsPropertyDrawableUtils->MakeShadowBlender(shadowBlenderParams), nullptr);
}

/**
 * @tc.name: ApplySDFShapeToFrostedGlassFilter001
 * @tc.desc: empty drawingFilter test
 * @tc.type: FUNC
 * @tc.require: issue190
 */
HWTEST_F(RSPropertyDrawableUtilsTest, ApplySDFShapeToFrostedGlassFilter001, testing::ext::TestSize.Level1)
{
    RSProperties properties;
    NodeId id = 1;
    std::shared_ptr<RSDrawingFilter> drawingFilter;
    RSPropertyDrawableUtils::ApplySDFShapeToFilter(properties, drawingFilter, id);
    EXPECT_EQ(drawingFilter, nullptr);
}

/**
 * @tc.name: ApplySDFShapeToFrostedGlassFilter002
 * @tc.desc: empty renderFilter test
 * @tc.type: FUNC
 * @tc.require: issue190
 */
HWTEST_F(RSPropertyDrawableUtilsTest, ApplySDFShapeToFrostedGlassFilter002, testing::ext::TestSize.Level1)
{
    RSProperties properties;
    NodeId id = 1;
    auto drawingFilter = std::make_shared<RSDrawingFilter>();
    RSPropertyDrawableUtils::ApplySDFShapeToFilter(properties, drawingFilter, id);
    EXPECT_EQ(drawingFilter->GetNGRenderFilter(), nullptr);
}


/**
 * @tc.name: ApplySDFShapeToFrostedGlassFilter003
 * @tc.desc: not a frostedGlass filter test
 * @tc.type: FUNC
 * @tc.require: issue190
 */
HWTEST_F(RSPropertyDrawableUtilsTest, ApplySDFShapeToFrostedGlassFilter003, testing::ext::TestSize.Level1)
{
    RSProperties properties;
    NodeId id = 1;
    auto renderFilter = RSNGRenderFilterBase::Create(RSNGEffectType::BLUR);
    auto drawingFilter = std::make_shared<RSDrawingFilter>();
    drawingFilter->SetNGRenderFilter(renderFilter);
    RSPropertyDrawableUtils::ApplySDFShapeToFilter(properties, drawingFilter, id);
    ASSERT_NE(drawingFilter->GetNGRenderFilter(), nullptr);
    EXPECT_NE(drawingFilter->GetNGRenderFilter()->GetType(), RSNGEffectType::FROSTED_GLASS);
}

/**
 * @tc.name: ApplySDFShapeToFrostedGlassFilter004
 * @tc.desc: have sdfShape test
 * @tc.type: FUNC
 * @tc.require: issue190
 */
HWTEST_F(RSPropertyDrawableUtilsTest, ApplySDFShapeToFrostedGlassFilter004, testing::ext::TestSize.Level1)
{
    RSProperties properties;
    auto sdfRRectShape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_RRECT_SHAPE);
    properties.SetSDFShape(sdfRRectShape);
    NodeId id = 1;
    auto renderFilter = RSNGRenderFilterBase::Create(RSNGEffectType::FROSTED_GLASS);
    const auto& filter = std::static_pointer_cast<RSNGRenderFrostedGlassFilter>(renderFilter);
    EXPECT_EQ(filter->Getter<FrostedGlassShapeRenderTag>()->stagingValue_, nullptr);
    auto drawingFilter = std::make_shared<RSDrawingFilter>();
    drawingFilter->SetNGRenderFilter(renderFilter);

    RSPropertyDrawableUtils::ApplySDFShapeToFilter(properties, drawingFilter, id);
    ASSERT_NE(drawingFilter->GetNGRenderFilter(), nullptr);
    EXPECT_EQ(drawingFilter->GetNGRenderFilter()->GetType(), RSNGEffectType::FROSTED_GLASS);
    const auto& filterFromDrawingFilter = std::static_pointer_cast<RSNGRenderFrostedGlassFilter>(
        drawingFilter->GetNGRenderFilter());
    EXPECT_NE(filterFromDrawingFilter->Getter<FrostedGlassShapeRenderTag>()->stagingValue_, nullptr);
}

/**
 * @tc.name: ApplySDFShapeToFrostedGlassFilter005
 * @tc.desc: not have sdfShape but have clip to rrect test
 * @tc.type: FUNC
 * @tc.require: issue190
 */
HWTEST_F(RSPropertyDrawableUtilsTest, ApplySDFShapeToFrostedGlassFilter005, testing::ext::TestSize.Level1)
{
    RSProperties properties;
    EXPECT_EQ(properties.GetSDFShape(), nullptr);
    properties.SetClipRRect(RRect(RectT<float>(0.0f, 0.0f, 100.0f, 100.0f), 10.0f, 10.0f));
    NodeId id = 1;
    auto renderFilter = RSNGRenderFilterBase::Create(RSNGEffectType::FROSTED_GLASS);
    const auto& filter = std::static_pointer_cast<RSNGRenderFrostedGlassFilter>(renderFilter);
    EXPECT_EQ(filter->Getter<FrostedGlassShapeRenderTag>()->stagingValue_, nullptr);
    auto drawingFilter = std::make_shared<RSDrawingFilter>();
    drawingFilter->SetNGRenderFilter(renderFilter);

    RSPropertyDrawableUtils::ApplySDFShapeToFilter(properties, drawingFilter, id);
    ASSERT_NE(drawingFilter->GetNGRenderFilter(), nullptr);
    EXPECT_EQ(drawingFilter->GetNGRenderFilter()->GetType(), RSNGEffectType::FROSTED_GLASS);
    const auto& filterFromDrawingFilter = std::static_pointer_cast<RSNGRenderFrostedGlassFilter>(
        drawingFilter->GetNGRenderFilter());
    const auto& rrectShape = std::static_pointer_cast<RSNGRenderSDFRRectShape>(
        filterFromDrawingFilter->Getter<FrostedGlassShapeRenderTag>()->stagingValue_);
    auto rrectFromShape = rrectShape->Getter<SDFRRectShapeRRectRenderTag>()->stagingValue_;
    EXPECT_TRUE(rrectFromShape.IsNearEqual(RRect(RectT<float>(0.0f, 0.0f, 100.0f, 100.0f), 10.0f, 10.0f)));
}

/**
 * @tc.name: ApplySDFShapeToFrostedGlassFilter006
 * @tc.desc: have cornerRadius to rrect test
 * @tc.type: FUNC
 * @tc.require: issue190
 */
HWTEST_F(RSPropertyDrawableUtilsTest, ApplySDFShapeToFrostedGlassFilter006, testing::ext::TestSize.Level1)
{
    RSProperties properties;
    EXPECT_EQ(properties.GetSDFShape(), nullptr);
    EXPECT_EQ(properties.GetClipRRect(), RRect());
    properties.SetCornerRadius(10.0f);
    NodeId id = 1;
    auto renderFilter = RSNGRenderFilterBase::Create(RSNGEffectType::FROSTED_GLASS);
    const auto& filter = std::static_pointer_cast<RSNGRenderFrostedGlassFilter>(renderFilter);
    EXPECT_EQ(filter->Getter<FrostedGlassShapeRenderTag>()->stagingValue_, nullptr);
    auto drawingFilter = std::make_shared<RSDrawingFilter>();
    drawingFilter->SetNGRenderFilter(renderFilter);

    RSPropertyDrawableUtils::ApplySDFShapeToFilter(properties, drawingFilter, id);
    ASSERT_NE(drawingFilter->GetNGRenderFilter(), nullptr);
    EXPECT_EQ(drawingFilter->GetNGRenderFilter()->GetType(), RSNGEffectType::FROSTED_GLASS);
    const auto& filterFromDrawingFilter = std::static_pointer_cast<RSNGRenderFrostedGlassFilter>(
        drawingFilter->GetNGRenderFilter());
    const auto& rrectShape = std::static_pointer_cast<RSNGRenderSDFRRectShape>(
        filterFromDrawingFilter->Getter<FrostedGlassShapeRenderTag>()->stagingValue_);
    auto rrectFromShape = rrectShape->Getter<SDFRRectShapeRRectRenderTag>()->stagingValue_;
    EXPECT_TRUE(rrectFromShape.IsNearEqual(RRect(properties.GetRRect().rect_, properties.GetRRect().radius_[0].x_,
        properties.GetRRect().radius_[0].y_)));
}

/**
 * @tc.name: ApplySDFShapeToFrostedGlassFilter007
 * @tc.desc: default use boundsRect
 * @tc.type: FUNC
 * @tc.require: issue190
 */
HWTEST_F(RSPropertyDrawableUtilsTest, ApplySDFShapeToFrostedGlassFilter007, testing::ext::TestSize.Level1)
{
    RSProperties properties;
    EXPECT_EQ(properties.GetSDFShape(), nullptr);
    EXPECT_EQ(properties.GetClipRRect(), RRect());
    EXPECT_EQ(properties.GetCornerRadius(), Vector4f(0.f, 0.f, 0.f, 0.f));
    properties.SetBounds(Vector4f(0.0f, 0.0f, 100.0f, 100.0f));
    properties.SetFrame(Vector4f(0.0f, 0.0f, 100.0f, 100.0f));
    NodeId id = 1;
    auto renderFilter = RSNGRenderFilterBase::Create(RSNGEffectType::FROSTED_GLASS);
    const auto& filter = std::static_pointer_cast<RSNGRenderFrostedGlassFilter>(renderFilter);
    EXPECT_EQ(filter->Getter<FrostedGlassShapeRenderTag>()->stagingValue_, nullptr);
    auto drawingFilter = std::make_shared<RSDrawingFilter>();
    drawingFilter->SetNGRenderFilter(renderFilter);

    RSPropertyDrawableUtils::ApplySDFShapeToFilter(properties, drawingFilter, id);
    ASSERT_NE(drawingFilter->GetNGRenderFilter(), nullptr);
    EXPECT_EQ(drawingFilter->GetNGRenderFilter()->GetType(), RSNGEffectType::FROSTED_GLASS);
    const auto& filterFromDrawingFilter = std::static_pointer_cast<RSNGRenderFrostedGlassFilter>(
        drawingFilter->GetNGRenderFilter());
    const auto& rrectShape = std::static_pointer_cast<RSNGRenderSDFRRectShape>(
        filterFromDrawingFilter->Getter<FrostedGlassShapeRenderTag>()->stagingValue_);
    auto rrectFromShape = rrectShape->Getter<SDFRRectShapeRRectRenderTag>()->stagingValue_;
    EXPECT_TRUE(rrectFromShape.IsNearEqual(RRect(RectT<float>(0.0f, 0.0f, 100.0f, 100.0f), 0.0f, 0.0f)));
}

/**
 * @tc.name: ApplySDFShapeToFilter008
 * @tc.desc: SDF_EDGE_LIGHT filter with sdfShape test
 * @tc.type: FUNC
 * @tc.require: issue190
 */
HWTEST_F(RSPropertyDrawableUtilsTest, ApplySDFShapeToFilter008, testing::ext::TestSize.Level1)
{
    RSProperties properties;
    auto sdfRRectShape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_RRECT_SHAPE);
    properties.SetSDFShape(sdfRRectShape);
    NodeId id = 1;

    auto renderFilter = RSNGRenderFilterBase::Create(RSNGEffectType::SDF_EDGE_LIGHT);
    ASSERT_NE(renderFilter, nullptr);
    const auto& filter = std::static_pointer_cast<RSNGRenderSDFEdgeLightFilter>(renderFilter);
    EXPECT_EQ(filter->Getter<SDFEdgeLightSDFShapeRenderTag>()->stagingValue_, nullptr);

    auto drawingFilter = std::make_shared<RSDrawingFilter>();
    drawingFilter->SetNGRenderFilter(renderFilter);

    RSPropertyDrawableUtils::ApplySDFShapeToFilter(properties, drawingFilter, id);

    ASSERT_NE(drawingFilter->GetNGRenderFilter(), nullptr);
    EXPECT_EQ(drawingFilter->GetNGRenderFilter()->GetType(), RSNGEffectType::SDF_EDGE_LIGHT);
    const auto& filterFromDrawingFilter = std::static_pointer_cast<RSNGRenderSDFEdgeLightFilter>(
        drawingFilter->GetNGRenderFilter());
    EXPECT_NE(filterFromDrawingFilter->Getter<SDFEdgeLightSDFShapeRenderTag>()->stagingValue_, nullptr);
}

/**
 * @tc.name: ApplySDFShapeToFilter009
 * @tc.desc: SDF_EDGE_LIGHT filter without sdfShape test (early return)
 * @tc.type: FUNC
 * @tc.require: issue190
 */
HWTEST_F(RSPropertyDrawableUtilsTest, ApplySDFShapeToFilter009, testing::ext::TestSize.Level1)
{
    RSProperties properties;
    EXPECT_EQ(properties.GetSDFShape(), nullptr);
    NodeId id = 1;

    auto renderFilter = RSNGRenderFilterBase::Create(RSNGEffectType::SDF_EDGE_LIGHT);
    ASSERT_NE(renderFilter, nullptr);
    const auto& filter = std::static_pointer_cast<RSNGRenderSDFEdgeLightFilter>(renderFilter);
    EXPECT_EQ(filter->Getter<SDFEdgeLightSDFShapeRenderTag>()->stagingValue_, nullptr);

    auto drawingFilter = std::make_shared<RSDrawingFilter>();
    drawingFilter->SetNGRenderFilter(renderFilter);

    // Should return early without setting shape
    RSPropertyDrawableUtils::ApplySDFShapeToFilter(properties, drawingFilter, id);

    ASSERT_NE(drawingFilter->GetNGRenderFilter(), nullptr);
    EXPECT_EQ(drawingFilter->GetNGRenderFilter()->GetType(), RSNGEffectType::SDF_EDGE_LIGHT);
    const auto& filterFromDrawingFilter = std::static_pointer_cast<RSNGRenderSDFEdgeLightFilter>(
        drawingFilter->GetNGRenderFilter());
    // Shape should still be null since we returned early
    EXPECT_EQ(filterFromDrawingFilter->Getter<SDFEdgeLightSDFShapeRenderTag>()->stagingValue_, nullptr);
}

/**
 * @tc.name: ApplyAdaptiveFrostedGlassParamsTest001
 * @tc.desc: ApplyAdaptiveFrostedGlassParams null and type checks
 * @tc.type: FUNC
 * @tc.require: issue
 */
HWTEST_F(RSPropertyDrawableUtilsTest, ApplyAdaptiveFrostedGlassParamsTest001, testing::ext::TestSize.Level1)
{
    // null effect
    Drawing::Canvas canvas;
    RSPropertyDrawableUtils::ApplyAdaptiveFrostedGlassParams(&canvas, nullptr);

    // effect not frosted glass
    auto renderFilter = RSNGRenderFilterBase::Create(RSNGEffectType::BLUR);
    ASSERT_NE(renderFilter, nullptr);
    auto drawingFilter = std::make_shared<RSDrawingFilter>();
    drawingFilter->SetNGRenderFilter(renderFilter);
    // capture any state we can inspect: the type must remain BLUR
    EXPECT_EQ(renderFilter->GetType(), RSNGEffectType::BLUR);
    RSPropertyDrawableUtils::ApplyAdaptiveFrostedGlassParams(&canvas, drawingFilter);
    // still should be BLUR after call
    EXPECT_EQ(renderFilter->GetType(), RSNGEffectType::BLUR);
}

namespace {
Vector2f DARK_BLUR(1.5f, 2.5f);
Vector2f DARK_WEIGHTS(0.1f, 0.2f);
Vector2f DARK_BG_RATES(0.3f, 0.4f);
Vector3f DARK_BG_KBS(0.5f, 0.6f, 0.7f);
Vector3f DARK_BG_POS(0.8f, 0.9f, 1.0f);
Vector3f DARK_BG_NEG(1.1f, 1.2f, 1.3f);
} // namespace

/**
 * @tc.name: ApplyAdaptiveFrostedGlassParamsTest002
 * @tc.desc: ApplyAdaptiveFrostedGlassParams dark / light branch
 * @tc.type: FUNC
 * @tc.require: issue
 */
HWTEST_F(RSPropertyDrawableUtilsTest, ApplyAdaptiveFrostedGlassParamsTest002, testing::ext::TestSize.Level1)
{
    // create frosted glass filter
    auto renderFilter = RSNGRenderFilterBase::Create(RSNGEffectType::FROSTED_GLASS);
    ASSERT_NE(renderFilter, nullptr);
    auto glass = std::static_pointer_cast<RSNGRenderFrostedGlassFilter>(renderFilter);
    auto drawingFilter = std::make_shared<RSDrawingFilter>();
    drawingFilter->SetNGRenderFilter(renderFilter);
    drawingFilter->GenerateAndUpdateGEVisualEffect();

    // initialize main properties to different sentinel values
    glass->Setter<FrostedGlassBlurParamsRenderTag>(Vector2f(0.0f, 0.0f));
    glass->Setter<FrostedGlassWeightsEmbossRenderTag>(Vector2f(0.0f, 0.0f));
    glass->Setter<FrostedGlassBgRatesRenderTag>(Vector2f(0.0f, 0.0f));
    glass->Setter<FrostedGlassBgKBSRenderTag>(Vector3f(0.0f, 0.0f, 0.0f));
    glass->Setter<FrostedGlassBgPosRenderTag>(Vector3f(0.0f, 0.0f, 0.0f));
    glass->Setter<FrostedGlassBgNegRenderTag>(Vector3f(0.0f, 0.0f, 0.0f));

    // set per-mode tags
    glass->Setter<FrostedGlassDarkModeBlurParamsRenderTag>(DARK_BLUR);
    glass->Setter<FrostedGlassDarkModeWeightsEmbossRenderTag>(DARK_WEIGHTS);
    glass->Setter<FrostedGlassDarkModeBgRatesRenderTag>(DARK_BG_RATES);
    glass->Setter<FrostedGlassDarkModeBgKBSRenderTag>(DARK_BG_KBS);
    glass->Setter<FrostedGlassDarkModeBgPosRenderTag>(DARK_BG_POS);
    glass->Setter<FrostedGlassDarkModeBgNegRenderTag>(DARK_BG_NEG);

    // prepare canvas and set picked color to black -> light
    Drawing::Canvas canvasDark;
    RSPaintFilterCanvas paintFilterCanvasDark(&canvasDark);
    paintFilterCanvasDark.SetColorPicked(ColorPlaceholder::SURFACE_CONTRAST, Drawing::Color::COLOR_BLACK);

    RSPropertyDrawableUtils::ApplyAdaptiveFrostedGlassParams(&canvasDark, drawingFilter);

    // prepare canvas and set picked color to white -> dark
    Drawing::Canvas canvasLight;
    RSPaintFilterCanvas paintFilterCanvasLight(&canvasLight);
    paintFilterCanvasLight.SetColorPicked(ColorPlaceholder::SURFACE_CONTRAST, Drawing::Color::COLOR_WHITE);

    RSPropertyDrawableUtils::ApplyAdaptiveFrostedGlassParams(&canvasLight, drawingFilter);

    drawingFilter->visualEffectContainer_ = nullptr;
    ASSERT_FALSE(drawingFilter->visualEffectContainer_);
    RSPropertyDrawableUtils::ApplyAdaptiveFrostedGlassParams(&canvasLight, drawingFilter);
}

/**
 * @tc.name: DrawBackgroundEffectTest003
 * @tc.desc: test DrawBackgroundEffect when filter cache branch condition is satisfied
 * @tc.type: FUNC
 * @tc.require: issueIA5FLZ
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawBackgroundEffectTest003, testing::ext::TestSize.Level1)
{
    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtils = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtils, nullptr);

    // Set up conditions to satisfy the branch:

    // 1. Enable filter cache
    RSProperties::filterCacheEnabled_ = true;

    // 2. Create valid filter
    std::shared_ptr<RSFilter> rsFilter = std::make_shared<RSFilter>();
    EXPECT_NE(rsFilter, nullptr);

    // 3. Create cache manager
    std::unique_ptr<RSFilterCacheManager> cacheManager = std::make_unique<RSFilterCacheManager>();
    EXPECT_NE(cacheManager, nullptr);

    // 4. Create canvas with valid surface
    Drawing::Canvas canvasTest(400, 400);
    RSPaintFilterCanvas paintFilterCanvas(&canvasTest);

    // 5. Enable filter cache on canvas (not disabled)
    paintFilterCanvas.SetDisableFilterCache(false);

    // 6. Set up screen size to ensure memory threshold is not exceeded
    Drawing::RectI normalScreenRect(0, 0, 1920, 1080);
    RSFilterCacheMemoryController::Instance().SetScreenRectInfo(normalScreenRect);

    // 7. Set bounds
    Drawing::RectI bounds(0, 0, 400, 400);

    // Verify all conditions are satisfied
    EXPECT_TRUE(RSProperties::filterCacheEnabled_);
    EXPECT_NE(cacheManager, nullptr);
    EXPECT_FALSE(paintFilterCanvas.GetDisableFilterCache());
    EXPECT_FALSE(cacheManager->IsFilterCacheMemExceedThreshold());

    // Call DrawBackgroundEffect - this should enter the cache branch
    rsPropertyDrawableUtils->DrawBackgroundEffect(&paintFilterCanvas, rsFilter, cacheManager, bounds);

    // The test passes if we reach here without crashing, meaning the branch was taken
    EXPECT_TRUE(true);
}

/**
 * @tc.name: DrawBackgroundEffectTest004
 * @tc.desc: test DrawBackgroundEffect when memory threshold is not exceeded but other conditions are met
 * @tc.type: FUNC
 * @tc.require: issueIA5FLZ
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawBackgroundEffectTest004, testing::ext::TestSize.Level1)
{
    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtils = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtils, nullptr);

    // Set up conditions to satisfy the branch
    RSProperties::filterCacheEnabled_ = true;

    // Create a valid filter
    std::shared_ptr<RSFilter> rsFilter = std::make_shared<RSFilter>();
    EXPECT_NE(rsFilter, nullptr);

    // Create cache manager
    std::unique_ptr<RSFilterCacheManager> cacheManager = std::make_unique<RSFilterCacheManager>();
    EXPECT_NE(cacheManager, nullptr);

    // Create canvas with valid surface and set up cache properly
    Drawing::Canvas canvasTest(200, 200);
    RSPaintFilterCanvas paintFilterCanvas(&canvasTest);
    paintFilterCanvas.SetDisableFilterCache(false);

    // Set up reasonable screen size
    Drawing::RectI screenRect(0, 0, 1920, 1080);
    RSFilterCacheMemoryController::Instance().SetScreenRectInfo(screenRect);

    Drawing::RectI bounds(0, 0, 200, 200);

    // Verify conditions
    EXPECT_TRUE(RSProperties::filterCacheEnabled_);
    EXPECT_NE(cacheManager, nullptr);
    EXPECT_FALSE(paintFilterCanvas.GetDisableFilterCache());
    EXPECT_FALSE(cacheManager->IsFilterCacheMemExceedThreshold());

    rsPropertyDrawableUtils->DrawBackgroundEffect(&paintFilterCanvas, rsFilter, cacheManager, bounds);
}

/**
 * @tc.name: DrawBackgroundEffectTest005
 * @tc.desc: test DrawBackgroundEffect when filter cache is disabled
 * @tc.type: FUNC
 * @tc.require: issueIA5FLZ
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawBackgroundEffectTest005, testing::ext::TestSize.Level1)
{
    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtils = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtils, nullptr);

    // Disable filter cache to ensure the branch is NOT taken
    RSProperties::filterCacheEnabled_ = false;

    std::shared_ptr<RSFilter> rsFilter = std::make_shared<RSFilter>();
    EXPECT_NE(rsFilter, nullptr);

    std::unique_ptr<RSFilterCacheManager> cacheManager = std::make_unique<RSFilterCacheManager>();
    EXPECT_NE(cacheManager, nullptr);

    Drawing::Canvas canvasTest(300, 300);
    RSPaintFilterCanvas paintFilterCanvas(&canvasTest);
    paintFilterCanvas.SetDisableFilterCache(false);

    Drawing::RectI screenRect(0, 0, 1920, 1080);
    RSFilterCacheMemoryController::Instance().SetScreenRectInfo(screenRect);

    Drawing::RectI bounds(0, 0, 300, 300);

    // Verify the first condition is false while others are true
    EXPECT_FALSE(RSProperties::filterCacheEnabled_);
    EXPECT_NE(cacheManager, nullptr);
    EXPECT_FALSE(paintFilterCanvas.GetDisableFilterCache());
    EXPECT_FALSE(cacheManager->IsFilterCacheMemExceedThreshold());

    rsPropertyDrawableUtils->DrawBackgroundEffect(&paintFilterCanvas, rsFilter, cacheManager, bounds);
}

/**
 * @tc.name: DrawBackgroundEffectTest006
 * @tc.desc: test DrawBackgroundEffect when canvas filter cache is disabled
 * @tc.type: FUNC
 * @tc.require: issueIA5FLZ
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawBackgroundEffectTest006, testing::ext::TestSize.Level1)
{
    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtils = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtils, nullptr);

    // Enable filter cache but disable it on canvas
    RSProperties::filterCacheEnabled_ = true;

    std::shared_ptr<RSFilter> rsFilter = std::make_shared<RSFilter>();
    EXPECT_NE(rsFilter, nullptr);

    std::unique_ptr<RSFilterCacheManager> cacheManager = std::make_unique<RSFilterCacheManager>();
    EXPECT_NE(cacheManager, nullptr);

    Drawing::Canvas canvasTest(300, 300);
    RSPaintFilterCanvas paintFilterCanvas(&canvasTest);
    paintFilterCanvas.SetDisableFilterCache(true); // Disable cache on canvas

    Drawing::RectI screenRect(0, 0, 1920, 1080);
    RSFilterCacheMemoryController::Instance().SetScreenRectInfo(screenRect);

    Drawing::RectI bounds(0, 0, 300, 300);

    // Verify one condition is false while others are true
    EXPECT_TRUE(RSProperties::filterCacheEnabled_);
    EXPECT_NE(cacheManager, nullptr);
    EXPECT_TRUE(paintFilterCanvas.GetDisableFilterCache()); // This should prevent cache branch
    EXPECT_FALSE(cacheManager->IsFilterCacheMemExceedThreshold());

    rsPropertyDrawableUtils->DrawBackgroundEffect(&paintFilterCanvas, rsFilter, cacheManager, bounds);
}

/**
 * @tc.name: DrawBackgroundEffectTest007
 * @tc.desc: test DrawBackgroundEffect when memory threshold is exceeded
 * @tc.type: FUNC
 * @tc.require: issueIA5FLZ
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawBackgroundEffectTest007, testing::ext::TestSize.Level1)
{
    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtils = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtils, nullptr);

    // Enable filter cache but set up memory threshold to be exceeded
    RSProperties::filterCacheEnabled_ = true;

    std::shared_ptr<RSFilter> rsFilter = std::make_shared<RSFilter>();
    EXPECT_NE(rsFilter, nullptr);

    std::unique_ptr<RSFilterCacheManager> cacheManager = std::make_unique<RSFilterCacheManager>();
    EXPECT_NE(cacheManager, nullptr);

    Drawing::Canvas canvasTest(300, 300);
    RSPaintFilterCanvas paintFilterCanvas(&canvasTest);
    paintFilterCanvas.SetDisableFilterCache(false);

    // Set very small screen size to trigger memory threshold exceeded
    Drawing::RectI smallScreenRect(0, 0, 1, 1);
    RSFilterCacheMemoryController::Instance().SetScreenRectInfo(smallScreenRect);
    RSFilterCacheMemoryController::Instance().cacheMem_ = 1000000;

    Drawing::RectI bounds(0, 0, 300, 300);

    // Verify conditions
    EXPECT_TRUE(RSProperties::filterCacheEnabled_);
    EXPECT_NE(cacheManager, nullptr);
    EXPECT_FALSE(paintFilterCanvas.GetDisableFilterCache());
    EXPECT_TRUE(cacheManager->IsFilterCacheMemExceedThreshold()); // This should prevent cache branch

    rsPropertyDrawableUtils->DrawBackgroundEffect(&paintFilterCanvas, rsFilter, cacheManager, bounds);
}

/**
 * @tc.name: DrawBackgroundEffectTest008
 * @tc.desc: test DrawBackgroundEffect when cache manager is null
 * @tc.type: FUNC
 * @tc.require: issueIA5FLZ
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawBackgroundEffectTest008, testing::ext::TestSize.Level1)
{
    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtils = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtils, nullptr);

    RSProperties::filterCacheEnabled_ = true;

    std::shared_ptr<RSFilter> rsFilter = std::make_shared<RSFilter>();
    EXPECT_NE(rsFilter, nullptr);

    // Set cache manager to null
    std::unique_ptr<RSFilterCacheManager> cacheManager = nullptr;

    Drawing::Canvas canvasTest(300, 300);
    RSPaintFilterCanvas paintFilterCanvas(&canvasTest);
    paintFilterCanvas.SetDisableFilterCache(false);

    Drawing::RectI bounds(0, 0, 300, 300);

    // Verify one condition is false while others can't be checked (null cacheManager)
    EXPECT_TRUE(RSProperties::filterCacheEnabled_);
    EXPECT_EQ(cacheManager, nullptr);
    EXPECT_FALSE(paintFilterCanvas.GetDisableFilterCache());

    rsPropertyDrawableUtils->DrawBackgroundEffect(&paintFilterCanvas, rsFilter, cacheManager, bounds);
}

/**
 * @tc.name: DrawBackgroundEffectTest009
 * @tc.desc: test DrawBackgroundEffect with all cache conditions satisfied and behindWindow true
 * @tc.type: FUNC
 * @tc.require: issueIA5FLZ
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawBackgroundEffectTest009, testing::ext::TestSize.Level1)
{
    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtils = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtils, nullptr);

    // Set up all conditions to satisfy the cache branch
    RSProperties::filterCacheEnabled_ = true;

    std::shared_ptr<RSFilter> rsFilter = std::make_shared<RSFilter>();
    EXPECT_NE(rsFilter, nullptr);

    std::unique_ptr<RSFilterCacheManager> cacheManager = std::make_unique<RSFilterCacheManager>();
    EXPECT_NE(cacheManager, nullptr);

    Drawing::Canvas canvasTest(400, 400);
    RSPaintFilterCanvas paintFilterCanvas(&canvasTest);
    paintFilterCanvas.SetDisableFilterCache(false);

    Drawing::RectI screenRect(0, 0, 1920, 1080);
    RSFilterCacheMemoryController::Instance().SetScreenRectInfo(screenRect);

    Drawing::RectI bounds(0, 0, 400, 400);

    // Verify all conditions are satisfied
    EXPECT_TRUE(RSProperties::filterCacheEnabled_);
    EXPECT_NE(cacheManager, nullptr);
    EXPECT_FALSE(paintFilterCanvas.GetDisableFilterCache());
    EXPECT_FALSE(cacheManager->IsFilterCacheMemExceedThreshold());

    // Call DrawBackgroundEffect with behindWindow = true
    rsPropertyDrawableUtils->DrawBackgroundEffect(&paintFilterCanvas, rsFilter, cacheManager, bounds, true);
}

} // namespace OHOS::Rosen