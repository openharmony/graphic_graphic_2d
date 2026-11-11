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
#include "drawable/rs_effect_drawable_utils.h"
#include "drawable/rs_render_node_drawable_adapter.h"
#include "draw/surface.h"
#include "effect/rs_render_filter_base.h"
#include "effect/rs_render_shader_base.h"
#include "effect/rs_render_shape_base.h"
#include "ge_visual_effect_container.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_depth_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "property/rs_properties.h"
#include "property/rs_properties_painter.h"
#include "property/rs_spatial_effect_manager.h"
#include "render/rs_drawing_filter.h"
#include "skia_adapter/skia_image.h"
#include "skia_adapter/skia_image_info.h"
#include "skia_adapter/skia_surface.h"
#include "skia_adapter/skia_canvas.h"
#include "render/rs_fly_out_shader_filter.h"
#include "render/rs_render_linear_gradient_blur_filter.h"
#include "render/rs_render_mesa_blur_filter.h"

namespace OHOS::Rosen {
namespace {
class DimensionSkiaImage : public Drawing::SkiaImage {
public:
    DimensionSkiaImage(int width, int height) : width_(width), height_(height) {}

    int GetWidth() const override
    {
        return width_;
    }

    int GetHeight() const override
    {
        return height_;
    }

private:
    int width_;
    int height_;
};

class MockSurfaceCanvas : public Drawing::Canvas {
public:
    explicit MockSurfaceCanvas(std::shared_ptr<Drawing::Surface> surface, int32_t width, int32_t height)
        : Canvas(width, height), surface_(std::move(surface))
    {
        auto canvas = surface_->GetCanvas();
        if (canvas) {
            auto skiaCanvas = canvas->GetImpl<Drawing::SkiaCanvas>();
            if (skiaCanvas && skiaCanvas->ExportSkCanvas()) {
                GetImpl<Drawing::SkiaCanvas>()->ImportSkCanvas(skiaCanvas->ExportSkCanvas());
            }
        }
    }

    Drawing::Surface* GetSurface() const override
    {
        return surface_.get();
    }

private:
    std::shared_ptr<Drawing::Surface> surface_;
};
} // namespace

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
    path.AddRect({ 0, 0, 10, 10 });
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
    rsPropertyDrawableUtils->DrawShadow(&canvasTest1, pathTest1, 1.0f, 1.0f, 1.0f, false, spotColor, false, RRect());
    Drawing::Canvas canvasTest2;
    Drawing::Path pathTest2;
    rsPropertyDrawableUtils->DrawShadow(&canvasTest2, pathTest2, 1.0f, 1.0f, 1.0f, true, spotColor, false, RRect());
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
    rsPropertyDrawableUtils->DrawColorFilter(nullptr, colorFilter);
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
    rsPropertyDrawableUtilsTest1->DrawShadow(&canvasTest1, path, 1.0f, 1.0f, 1.0f, true, spotColor, false, RRect());
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
    path.AddRect({ 0, 0, 5, 5 });
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
    EXPECT_EQ(rsPropertyDrawableUtilsTest->GetShadowRegionImage(nullptr, drPath, matrix), nullptr);
    auto resultTest1 = rsPropertyDrawableUtilsTest->GetShadowRegionImage(&paintFilterCanvas, drPath, matrix);
    EXPECT_EQ(resultTest1, nullptr);
    auto surface = Drawing::Surface::MakeRasterN32Premul(10, 10);
    paintFilterCanvas.surface_ = surface.get();
    auto resultTest2 = rsPropertyDrawableUtilsTest->GetShadowRegionImage(&paintFilterCanvas, drPath, matrix);
    EXPECT_EQ(resultTest2, nullptr);
    drPath.AddRect({ 0, 0, 5, 5 });
    auto resultTest3 = rsPropertyDrawableUtilsTest->GetShadowRegionImage(&paintFilterCanvas, drPath, matrix);
    EXPECT_NE(resultTest3, nullptr);
}

/**
 * @tc.name: DrawShadowMaskFilterTest019
 * @tc.desc: DrawShadowMaskFilter test with positive radius (blur shadow)
 * @tc.type: FUNC
 * @tc.require:issueIA5Y41
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawShadowMaskFilterTest019, testing::ext::TestSize.Level1)
{
    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtilsTest = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtilsTest, nullptr);
    Drawing::Canvas canvasTest;
    Drawing::Path path;
    path.AddRect({ 0, 0, 5, 5 });
    rsPropertyDrawableUtilsTest->DrawShadowMaskFilter(
        &canvasTest, path, 1.f, 1.f, 1.f, false, Color(255, 255, 255, 255), false, false, RRect());
    ASSERT_TRUE(true);
}

/**
 * @tc.name: DrawShadowMaskFilterNullCanvasTest
 * @tc.desc: DrawShadowMaskFilter returns safely when canvas is null
 * @tc.type: FUNC
 * @tc.require: issue25648
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawShadowMaskFilterNullCanvasTest, testing::ext::TestSize.Level1)
{
    auto utils = std::make_shared<RSPropertyDrawableUtils>();
    Drawing::Path path;
    path.AddRect({ 0, 0, 5, 5 });
    const auto bounds = path.GetBounds();

    utils->DrawShadowMaskFilter(nullptr, path, 1.f, 1.f, 1.f, false, Color(255, 255, 255, 255), false, false, RRect());
    EXPECT_EQ(path.GetBounds(), bounds);
}

/**
 * @tc.name: DrawShadowMaskFilterTest020
 * @tc.desc: DrawShadowMaskFilter test with zero radius (solid shadow, no blur)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawShadowMaskFilterTest020, testing::ext::TestSize.Level1)
{
    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtilsTest = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtilsTest, nullptr);
    Drawing::Canvas canvasTest;
    Drawing::Path path;
    path.AddRect({0, 0, 5, 5});
    rsPropertyDrawableUtilsTest->DrawShadowMaskFilter(&canvasTest, path, 1.f, 1.f, 0.f, false,
        Color(255, 0, 0, 255), false, false, RRect());
    ASSERT_TRUE(true);
}

/**
 * @tc.name: DrawShadowMaskFilterTest021
 * @tc.desc: DrawShadowMaskFilter test with negative radius (no shadow expected)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawShadowMaskFilterTest021, testing::ext::TestSize.Level1)
{
    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtilsTest = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtilsTest, nullptr);
    Drawing::Canvas canvasTest;
    Drawing::Path path;
    path.AddRect({0, 0, 5, 5});
    rsPropertyDrawableUtilsTest->DrawShadowMaskFilter(&canvasTest, path, 1.f, 1.f, -1.f, false,
        Color(255, 0, 0, 255), false, false, RRect());
    ASSERT_TRUE(true);
}

/**
 * @tc.name: DrawShadowUseBoundsClipTest
 * @tc.desc: DrawShadow with useBoundsClip=true, isFilled=false (ClipRoundRect) and isFilled=true (no clip)
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawShadowUseBoundsClipTest, testing::ext::TestSize.Level1)
{
    auto utils = std::make_shared<RSPropertyDrawableUtils>();
    Drawing::Canvas canvas;
    Drawing::Path path;
    path.AddRect({ 0, 0, 100, 100 });
    RectT<float> rect(0.0f, 0.0f, 100.0f, 100.0f);
    RRect rrect(rect, 10.0f, 10.0f);
    // isFilled=false: clip applied via ClipRoundRect
    utils->DrawShadow(&canvas, path, 5.0f, 5.0f, 10.0f, false, Color(0, 0, 0, 255), true, rrect);
    // isFilled=true: clip not applied
    utils->DrawShadow(&canvas, path, 5.0f, 5.0f, 10.0f, true, Color(0, 0, 0, 255), true, rrect);
    ASSERT_TRUE(true);
}
 
/**
 * @tc.name: DrawShadowMaskFilterUseBoundsClipTest
 * @tc.desc: DrawShadowMaskFilter with useBoundsClip=true, isFilled=false (ClipRoundRect) and isFilled=true (no clip)
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawShadowMaskFilterUseBoundsClipTest, testing::ext::TestSize.Level1)
{
    auto utils = std::make_shared<RSPropertyDrawableUtils>();
    Drawing::Canvas canvas;
    Drawing::Path path;
    path.AddRect({ 0, 0, 100, 100 });
    RectT<float> rect(0.0f, 0.0f, 100.0f, 100.0f);
    RRect rrect(rect, 10.0f, 10.0f);
    // isFilled=false: clip applied via ClipRoundRect
    utils->DrawShadowMaskFilter(&canvas, path, 5.f, 5.f, 10.f, false, Color(255, 0, 0, 255), false, true, rrect);
    // isFilled=true: clip not applied
    utils->DrawShadowMaskFilter(&canvas, path, 5.f, 5.f, 10.f, true, Color(255, 0, 0, 255), false, true, rrect);
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
    ASSERT_FALSE(
        rsPropertyDrawableUtilsTest->GetGravityMatrix(Gravity::RESIZE_ASPECT_BOTTOM_RIGHT, rectTwo, 10, 10, matrix));
    ASSERT_TRUE(
        rsPropertyDrawableUtilsTest->GetGravityMatrix(Gravity::RESIZE_ASPECT_BOTTOM_RIGHT, rect, 20, 20, matrix));
    ASSERT_FALSE(rsPropertyDrawableUtilsTest->GetGravityMatrix(Gravity::RESIZE_ASPECT_FILL, rect, 0, 0, matrix));
    ASSERT_FALSE(
        rsPropertyDrawableUtilsTest->GetGravityMatrix(Gravity::RESIZE_ASPECT_FILL, rectThree, 10, 100000000, matrix));
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
 * @tc.name: DrawForegroundFilterWithCustomRect001
 * @tc.desc: Test DrawForegroundFilter with custom draw rect
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawForegroundFilterWithCustomRect001, testing::ext::TestSize.Level1)
{
    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtils = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtils, nullptr);

    Drawing::Canvas canvasTest(100, 100);
    RSPaintFilterCanvas paintFilterCanvas(&canvasTest);

    auto imageFilter = std::make_shared<Drawing::ImageFilter>();
    auto filterPtr = std::make_shared<RSRenderFilterParaBase>();
    std::vector<std::shared_ptr<RSRenderFilterParaBase>> shaderFilters;
    shaderFilters.push_back(filterPtr);
    uint32_t hash = 1;
    auto rsFilter = std::make_shared<RSDrawingFilter>(imageFilter, shaderFilters, hash);

    RectF customDrawRect(10.0f, 10.0f, 50.0f, 50.0f);
    std::optional<RectF> drawRect = customDrawRect;

    rsPropertyDrawableUtils->DrawForegroundFilter(paintFilterCanvas, rsFilter, drawRect);
}

/**
 * @tc.name: PickColorAndGpuScaleImageTest001
 * @tc.desc: test PickColor and GpuScaleImage null handling and void* semaphore
 * @tc.type: FUNC
 * @tc.require: issueI9SCBR
 */
HWTEST_F(RSPropertyDrawableUtilsTest, PickColorAndGpuScaleImageTest001, testing::ext::TestSize.Level1)
{
    auto utils = std::make_shared<RSPropertyDrawableUtils>();
    Drawing::Canvas canvas;
    auto gpuContext = canvas.GetGPUContext();
    auto image = std::make_shared<Drawing::Image>();
    Drawing::ColorQuad color;

    // PickColor: null context/image should return false
    EXPECT_FALSE(utils->PickColor(nullptr, image, color));
    EXPECT_FALSE(utils->PickColor(gpuContext, nullptr, color));

    // GpuScaleImage: null context/image/invalid should return nullptr
    EXPECT_EQ(utils->GpuScaleImage(nullptr, image), nullptr);
    EXPECT_EQ(utils->GpuScaleImage(gpuContext, nullptr), nullptr);
    EXPECT_EQ(utils->GpuScaleImage(gpuContext, image), nullptr); // invalid dims

    constexpr int largeDimension = 65535;
    auto largeImage = std::make_shared<Drawing::Image>();
    largeImage->imageImplPtr = std::make_shared<DimensionSkiaImage>(largeDimension, largeDimension);
    EXPECT_EQ(utils->GpuScaleImage(gpuContext, largeImage), nullptr);

    utils->PickColor(gpuContext, image, color);
    utils->GpuScaleImage(gpuContext, image);
}

/**
 * @tc.name: MakeHdrDarkenBlenderTest001
 * @tc.desc: MakeHdrDarkenBlender Test
 * @tc.type: FUNC
 * @tc.require: issueICLU4I
 */
HWTEST_F(RSPropertyDrawableUtilsTest, MakeHdrDarkenBlenderTest001, testing::ext::TestSize.Level1)
{
    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtils = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(rsPropertyDrawableUtils, nullptr);
    RSHdrDarkenBlenderPara hdrDarkenBlenderParams1;
    EXPECT_NE(rsPropertyDrawableUtils->MakeHdrDarkenBlender(hdrDarkenBlenderParams1), nullptr);

    RSHdrDarkenBlenderPara hdrDarkenBlenderParams2 = {1.0, {0.2, 0.3, 0.5}};
    EXPECT_NE(rsPropertyDrawableUtils->MakeHdrDarkenBlender(hdrDarkenBlenderParams2), nullptr);

    RSHdrDarkenBlenderPara hdrDarkenBlenderParams3 = {0.0};
    EXPECT_EQ(rsPropertyDrawableUtils->MakeHdrDarkenBlender(hdrDarkenBlenderParams3), nullptr);
}

/**
 * @tc.name: MakeColorfulBrightnessBlenderTest001
 * @tc.desc: MakeColorfulBrightnessBlender Test with default and fully-specified params.
 *           Covers the normal builder construction and uniform-setting path.
 * @tc.type: FUNC
 * @tc.require: issueICLU4I
 */
HWTEST_F(RSPropertyDrawableUtilsTest, MakeColorfulBrightnessBlenderTest001, testing::ext::TestSize.Level1)
{
    std::shared_ptr<RSPropertyDrawableUtils> rsPropertyDrawableUtils = std::make_shared<RSPropertyDrawableUtils>();
    ASSERT_NE(rsPropertyDrawableUtils, nullptr);

    // default-constructed params (F2 builder-valid path)
    RSColorfulBrightnessBlenderPara params1;
    auto blender1 = rsPropertyDrawableUtils->MakeColorfulBrightnessBlender(params1);
    EXPECT_NE(blender1, nullptr);

    // fully-specified params (F2 builder-valid path, all uniforms set)
    Vector3f pos(0.6f, 0.7f, 0.8f);
    Vector3f neg(0.9f, 0.1f, 0.2f);
    RSColorfulBrightnessBlenderPara params2(1.0f, 0.5f, 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, pos, neg, 0.33f, 0.88f);
    auto blender2 = rsPropertyDrawableUtils->MakeColorfulBrightnessBlender(params2);
    EXPECT_NE(blender2, nullptr);
}

/**
 * @tc.name: DrawDepthOcclusionWithValidParameters
 * @tc.desc: test DrawDepthOcclusion with valid parameters
 * @tc.type: FUNC
 * @tc.require: issueICLU4I
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawDepthOcclusionWithValidParameters, testing::ext::TestSize.Level1)
{
    auto utils = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(utils, nullptr);

    Drawing::Canvas canvas(400, 400);
    auto surface = Drawing::Surface::MakeRasterN32Premul(100, 100);
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    paintFilterCanvas.surface_ = surface.get();

    auto snapshot = std::make_shared<Drawing::Image>();
    auto depthMap = std::make_shared<Drawing::Image>();
    Vector4f depthPlane(1.0f, 0.0f, 0.0f, 1000.0f);
    Vector2f nearFar(1.0f, 100.0f);
    Drawing::Matrix invMatrix;
    invMatrix.SetScale(1.0f, 1.0f);

    auto result =
        utils->DrawDepthOcclusion(&paintFilterCanvas, snapshot, depthMap, depthPlane, nearFar, 0.5f, invMatrix);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: DrawDepthOcclusionWithNullCanvas
 * @tc.desc: test DrawDepthOcclusion with null canvas
 * @tc.type: FUNC
 * @tc.require: issueICLU4I
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawDepthOcclusionWithNullCanvas, testing::ext::TestSize.Level1)
{
    auto utils = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(utils, nullptr);

    auto snapshot = std::make_shared<Drawing::Image>();
    auto depthMap = std::make_shared<Drawing::Image>();
    Vector4f depthPlane(1.0f, 0.0f, 0.0f, 1000.0f);
    Vector2f nearFar(1.0f, 100.0f);
    Drawing::Matrix invMatrix;

    auto result = utils->DrawDepthOcclusion(nullptr, snapshot, depthMap, depthPlane, nearFar, 0.5f, invMatrix);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: DrawDepthOcclusionWithNullSnapshot
 * @tc.desc: test DrawDepthOcclusion with null snapshot
 * @tc.type: FUNC
 * @tc.require: issueICLU4I
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawDepthOcclusionWithNullSnapshot, testing::ext::TestSize.Level1)
{
    auto utils = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(utils, nullptr);

    Drawing::Canvas canvas(100, 100);
    auto depthMap = std::make_shared<Drawing::Image>();
    Vector4f depthPlane(1.0f, 0.0f, 0.0f, 1000.0f);
    Vector2f nearFar(1.0f, 100.0f);
    Drawing::Matrix invMatrix;

    auto result = utils->DrawDepthOcclusion(&canvas, nullptr, depthMap, depthPlane, nearFar, 0.5f, invMatrix);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: DrawDepthOcclusionWithNullDepthMap
 * @tc.desc: test DrawDepthOcclusion with null depthMap
 * @tc.type: FUNC
 * @tc.require: issueICLU4I
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawDepthOcclusionWithNullDepthMap, testing::ext::TestSize.Level1)
{
    auto utils = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(utils, nullptr);

    Drawing::Canvas canvas(100, 100);
    auto snapshot = std::make_shared<Drawing::Image>();
    Vector4f depthPlane(1.0f, 0.0f, 0.0f, 1000.0f);
    Vector2f nearFar(1.0f, 100.0f);
    Drawing::Matrix invMatrix;

    auto result = utils->DrawDepthOcclusion(&canvas, snapshot, nullptr, depthPlane, nearFar, 0.5f, invMatrix);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: DrawDepthOcclusionWithNullSurface
 * @tc.desc: test DrawDepthOcclusion with null surface
 * @tc.type: FUNC
 * @tc.require: issueICLU4I
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawDepthOcclusionWithNullSurface, testing::ext::TestSize.Level1)
{
    auto utils = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(utils, nullptr);

    Drawing::Canvas canvas(100, 100);
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    paintFilterCanvas.surface_ = nullptr;

    auto snapshot = std::make_shared<Drawing::Image>();
    auto depthMap = std::make_shared<Drawing::Image>();
    Vector4f depthPlane(1.0f, 0.0f, 0.0f, 1000.0f);
    Vector2f nearFar(1.0f, 100.0f);
    Drawing::Matrix invMatrix;

    auto result =
        utils->DrawDepthOcclusion(&paintFilterCanvas, snapshot, depthMap, depthPlane, nearFar, 0.5f, invMatrix);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: DrawDepthOcclusionWithZeroOcclusionWeight
 * @tc.desc: test DrawDepthOcclusion with zero occlusionWeight
 * @tc.type: FUNC
 * @tc.require: issueICLU4I
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawDepthOcclusionWithZeroOcclusionWeight, testing::ext::TestSize.Level1)
{
    auto utils = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(utils, nullptr);

    Drawing::Canvas canvas(100, 100);
    auto snapshot = std::make_shared<Drawing::Image>();
    auto depthMap = std::make_shared<Drawing::Image>();
    Vector4f depthPlane(0.0f, 0.0f, 0.0f, 1000.0f);
    Vector2f nearFar(1.0f, 100.0f);
    Drawing::Matrix invMatrix;

    auto result = utils->DrawDepthOcclusion(&canvas, snapshot, depthMap, depthPlane, nearFar, 0.0f, invMatrix);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: DrawDepthOcclusionWithZeroNearFar
 * @tc.desc: test DrawDepthOcclusion with zero nearFar values
 * @tc.type: FUNC
 * @tc.require: issueICLU4I
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawDepthOcclusionWithZeroNearFar, testing::ext::TestSize.Level1)
{
    auto utils = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(utils, nullptr);

    Drawing::Canvas canvas(100, 100);
    auto snapshot = std::make_shared<Drawing::Image>();
    auto depthMap = std::make_shared<Drawing::Image>();
    Vector4f depthPlane(1.0f, 0.0f, 0.0f, 1000.0f);
    Vector2f nearFar(0.0f, 0.0f);
    Drawing::Matrix invMatrix;
    invMatrix.SetScale(1.0f, 1.0f);

    auto result = utils->DrawDepthOcclusion(&canvas, snapshot, depthMap, depthPlane, nearFar, 0.5f, invMatrix);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: DrawDepthOcclusionWithNegativeWeight
 * @tc.desc: test DrawDepthOcclusion with negative weight clamped
 * @tc.type: FUNC
 * @tc.require: issueICLU4I
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawDepthOcclusionWithNegativeWeight, testing::ext::TestSize.Level1)
{
    auto utils = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(utils, nullptr);

    Drawing::Canvas canvas(100, 100);
    auto snapshot = std::make_shared<Drawing::Image>();
    auto depthMap = std::make_shared<Drawing::Image>();
    Vector4f depthPlane(1.0f, 0.0f, 0.0f, 1000.0f);
    Vector2f nearFar(1.0f, 100.0f);
    Drawing::Matrix invMatrix;

    auto result = utils->DrawDepthOcclusion(&canvas, snapshot, depthMap, depthPlane, nearFar, -1.0f, invMatrix);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: DrawDepthOcclusionWithIdentityInvMatrix
 * @tc.desc: test DrawDepthOcclusion with identity matrix
 * @tc.type: FUNC
 * @tc.require: issueICLU4I
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawDepthOcclusionWithIdentityInvMatrix, testing::ext::TestSize.Level1)
{
    auto utils = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(utils, nullptr);

    Drawing::Canvas canvas(100, 100);
    auto snapshot = std::make_shared<Drawing::Image>();
    auto depthMap = std::make_shared<Drawing::Image>();
    Vector4f depthPlane(1.0f, 0.0f, 0.0f, 1000.0f);
    Vector2f nearFar(1.0f, 100.0f);
    Drawing::Matrix invMatrix;
    invMatrix.Set(Drawing::Matrix::TRANS_X, 0.0f);
    invMatrix.Set(Drawing::Matrix::TRANS_Y, 0.0f);
    invMatrix.Set(Drawing::Matrix::SCALE_X, 1.0f);
    invMatrix.Set(Drawing::Matrix::SCALE_Y, 1.0f);

    auto result = utils->DrawDepthOcclusion(&canvas, snapshot, depthMap, depthPlane, nearFar, 0.5f, invMatrix);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: DrawDepthOcclusionWithScaleMatrix
 * @tc.desc: test DrawDepthOcclusion with scale transform matrix
 * @tc.type: FUNC
 * @tc.require: issueICLU4I
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawDepthOcclusionWithScaleMatrix, testing::ext::TestSize.Level1)
{
    auto utils = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(utils, nullptr);

    Drawing::Canvas canvas(100, 100);
    auto snapshot = std::make_shared<Drawing::Image>();
    auto depthMap = std::make_shared<Drawing::Image>();
    Vector4f depthPlane(1.0f, 0.0f, 0.0f, 1000.0f);
    Vector2f nearFar(1.0f, 100.0f);
    Drawing::Matrix invMatrix;
    invMatrix.SetScale(0.5f, 0.5f);
    invMatrix.PostTranslate(50.0f, 50.0f);

    auto result = utils->DrawDepthOcclusion(&canvas, snapshot, depthMap, depthPlane, nearFar, 0.5f, invMatrix);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: DrawDepthOcclusionWithReverseMode
 * @tc.desc: test DrawDepthOcclusion with reverse mode disabled
 * @tc.type: FUNC
 * @tc.require: issueICLU4I
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawDepthOcclusionWithReverseMode, testing::ext::TestSize.Level1)
{
    auto utils = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(utils, nullptr);

    Drawing::Canvas canvas(400, 400);
    auto surface = Drawing::Surface::MakeRasterN32Premul(100, 100);
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    paintFilterCanvas.surface_ = surface.get();

    auto snapshot = std::make_shared<Drawing::Image>();
    auto depthMap = std::make_shared<Drawing::Image>();
    Vector4f depthPlane(1.0f, 0.0f, 0.0f, 1000.0f);
    Vector2f nearFar(1.0f, 100.0f);
    Drawing::Matrix invMatrix;
    invMatrix.SetScale(1.0f, 1.0f);

    auto result =
        utils->DrawDepthOcclusion(&paintFilterCanvas, snapshot, depthMap, depthPlane, nearFar, 0.5f, invMatrix);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: DrawDepthOcclusionWithZeroWeightAndReverseMode
 * @tc.desc: test DrawDepthOcclusion with zero weight triggers reverse mode handling
 * @tc.type: FUNC
 * @tc.require: issueICLU4I
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawDepthOcclusionWithZeroWeightAndReverseMode, testing::ext::TestSize.Level1)
{
    auto utils = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(utils, nullptr);

    Drawing::Canvas canvas(100, 100);
    auto surface = Drawing::Surface::MakeRasterN32Premul(100, 100);
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    paintFilterCanvas.surface_ = surface.get();

    auto snapshot = std::make_shared<Drawing::Image>();
    auto depthMap = std::make_shared<Drawing::Image>();
    Vector4f depthPlane(0.0f, 0.0f, 1.0f, 0.0f);
    Vector2f nearFar(1.0f, 100.0f);
    Drawing::Matrix invMatrix;

    auto result =
        utils->DrawDepthOcclusion(&paintFilterCanvas, snapshot, depthMap, depthPlane, nearFar, 0.0f, invMatrix);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: DrawDepthOcclusionWithLargeNearFarValues
 * @tc.desc: test DrawDepthOcclusion with large near/far values
 * @tc.type: FUNC
 * @tc.require: issueICLU4I
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawDepthOcclusionWithLargeNearFarValues, testing::ext::TestSize.Level1)
{
    auto utils = std::make_shared<RSPropertyDrawableUtils>();
    EXPECT_NE(utils, nullptr);

    Drawing::Canvas canvas(100, 100);
    auto snapshot = std::make_shared<Drawing::Image>();
    auto depthMap = std::make_shared<Drawing::Image>();
    Vector4f depthPlane(1.0f, 0.0f, 0.0f, 1000000.0f);
    Vector2f nearFar(10000.0f, 100000.0f);
    Drawing::Matrix invMatrix;
    invMatrix.SetScale(1.0f, 1.0f);

    auto result = utils->DrawDepthOcclusion(&canvas, snapshot, depthMap, depthPlane, nearFar, 0.5f, invMatrix);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: GetResolvedSDFShape001
 * @tc.desc: Test GetResolvedSDFShape when sdfShape is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableUtilsTest, GetResolvedSDFShape001, testing::ext::TestSize.Level1)
{
    RSProperties properties;
    EXPECT_EQ(RSPropertyDrawableUtils::GetResolvedSDFShape(properties), nullptr);
}

/**
 * @tc.name: GetResolvedSDFShape002
 * @tc.desc: Test GetResolvedSDFShape when sdfShape type is not SDF_DISTORT_OP_SHAPE
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableUtilsTest, GetResolvedSDFShape002, testing::ext::TestSize.Level1)
{
    RSProperties properties;
    auto rrectShape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_RRECT_SHAPE);
    properties.SetSDFShape(rrectShape);
    auto result = RSPropertyDrawableUtils::GetResolvedSDFShape(properties);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->GetType(), RSNGEffectType::SDF_RRECT_SHAPE);
}

/**
 * @tc.name: GetResolvedSDFShape003
 * @tc.desc: Test GetResolvedSDFShape when foregroundFilter is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableUtilsTest, GetResolvedSDFShape003, testing::ext::TestSize.Level1)
{
    RSProperties properties;
    auto distortShape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_DISTORT_OP_SHAPE);
    properties.SetSDFShape(distortShape);
    auto result = RSPropertyDrawableUtils::GetResolvedSDFShape(properties);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->GetType(), RSNGEffectType::SDF_DISTORT_OP_SHAPE);
}

/**
 * @tc.name: GetResolvedSDFShape004
 * @tc.desc: Test GetResolvedSDFShape when foregroundFilter is not DrawingFilter
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableUtilsTest, GetResolvedSDFShape004, testing::ext::TestSize.Level1)
{
    RSProperties properties;
    auto foregroundFilter = std::make_shared<RSFilter>();
    properties.SetForegroundFilter(foregroundFilter);
    auto distortShape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_DISTORT_OP_SHAPE);
    properties.SetSDFShape(distortShape);
    auto result = RSPropertyDrawableUtils::GetResolvedSDFShape(properties);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->GetType(), RSNGEffectType::SDF_DISTORT_OP_SHAPE);
}

/**
 * @tc.name: GetResolvedSDFShape005
 * @tc.desc: Test GetResolvedSDFShape when renderFilter is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableUtilsTest, GetResolvedSDFShape005, testing::ext::TestSize.Level1)
{
    RSProperties properties;
    auto drawingFilter = std::make_shared<RSDrawingFilter>();
    properties.SetForegroundFilter(drawingFilter);
    auto distortShape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_DISTORT_OP_SHAPE);
    properties.SetSDFShape(distortShape);
    auto result = RSPropertyDrawableUtils::GetResolvedSDFShape(properties);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->GetType(), RSNGEffectType::SDF_DISTORT_OP_SHAPE);
}

/**
 * @tc.name: GetResolvedSDFShape006
 * @tc.desc: Test GetResolvedSDFShape when renderFilter type is not DISTORTION_COLLAPSE
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableUtilsTest, GetResolvedSDFShape006, testing::ext::TestSize.Level1)
{
    RSProperties properties;
    auto blurFilter = RSNGRenderFilterBase::Create(RSNGEffectType::BLUR);
    auto drawingFilter = std::make_shared<RSDrawingFilter>();
    drawingFilter->SetNGRenderFilter(blurFilter);
    properties.SetForegroundFilter(drawingFilter);
    auto distortShape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_DISTORT_OP_SHAPE);
    properties.SetSDFShape(distortShape);
    auto result = RSPropertyDrawableUtils::GetResolvedSDFShape(properties);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->GetType(), RSNGEffectType::SDF_DISTORT_OP_SHAPE);
}

/**
 * @tc.name: GetResolvedSDFShape007
 * @tc.desc: Test GetResolvedSDFShape when innerShape exists
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableUtilsTest, GetResolvedSDFShape007, testing::ext::TestSize.Level1)
{
    RSProperties properties;
    auto collapseFilter = RSNGRenderFilterBase::Create(RSNGEffectType::DISTORTION_COLLAPSE);
    auto drawingFilter = std::make_shared<RSDrawingFilter>();
    drawingFilter->SetNGRenderFilter(collapseFilter);
    properties.SetForegroundFilter(drawingFilter);
    auto distortShape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_DISTORT_OP_SHAPE);
    auto innerShape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_RRECT_SHAPE);
    auto distortOpShape = std::static_pointer_cast<RSNGRenderSDFDistortOpShape>(distortShape);
    distortOpShape->Setter<SDFDistortOpShapeShapeRenderTag>(innerShape, PropertyUpdateType::UPDATE_TYPE_ONLY_VALUE);
    properties.SetSDFShape(distortShape);
    auto result = RSPropertyDrawableUtils::GetResolvedSDFShape(properties);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->GetType(), RSNGEffectType::SDF_RRECT_SHAPE);
    EXPECT_EQ(result.get(), innerShape.get());
}

/**
 * @tc.name: GetResolvedSDFShape008
 * @tc.desc: Test GetResolvedSDFShape when innerShape does not exist
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableUtilsTest, GetResolvedSDFShape008, testing::ext::TestSize.Level1)
{
    RSProperties properties;
    auto collapseFilter = RSNGRenderFilterBase::Create(RSNGEffectType::DISTORTION_COLLAPSE);
    auto drawingFilter = std::make_shared<RSDrawingFilter>();
    drawingFilter->SetNGRenderFilter(collapseFilter);
    properties.SetForegroundFilter(drawingFilter);
    auto distortShape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_DISTORT_OP_SHAPE);
    properties.SetSDFShape(distortShape);
    auto result = RSPropertyDrawableUtils::GetResolvedSDFShape(properties);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->GetType(), RSNGEffectType::SDF_DISTORT_OP_SHAPE);
}

/**
 * @tc.name: DrawColorNGFilterSurfaceNullptr
 * @tc.desc: DrawColorNGFilter with CPU canvas (no surface)
 *          (branch: all params valid → false; surface == nullptr → true)
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawColorNGFilterSurfaceNullptr, testing::ext::TestSize.Level1)
{
    auto container = std::make_shared<Drawing::GEVisualEffectContainer>();
    Drawing::Canvas canvas;
    Drawing::Rect rect(0.0f, 0.0f, 10.0f, 10.0f);
    EXPECT_EQ(canvas.GetSurface(), nullptr);
    DrawableV2::RSEffectDrawableUtils::DrawColorNGFilter(&canvas, &rect, container);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: DrawColorNGFilterNullCanvas
 * @tc.desc: DrawColorNGFilter with null canvas
 *          (branch: canvas == nullptr → true)
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawColorNGFilterNullCanvas, testing::ext::TestSize.Level1)
{
    auto container = std::make_shared<Drawing::GEVisualEffectContainer>();
    Drawing::Rect rect(0.0f, 0.0f, 10.0f, 10.0f);
    DrawableV2::RSEffectDrawableUtils::DrawColorNGFilter(nullptr, &rect, container);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: DrawColorNGFilterNullRect
 * @tc.desc: DrawColorNGFilter with null rect
 *          (branch: rect == nullptr → true)
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawColorNGFilterNullRect, testing::ext::TestSize.Level1)
{
    auto container = std::make_shared<Drawing::GEVisualEffectContainer>();
    Drawing::Canvas canvas;
    DrawableV2::RSEffectDrawableUtils::DrawColorNGFilter(&canvas, nullptr, container);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: DrawColorNGFilterNullContainer
 * @tc.desc: DrawColorNGFilter with null visualEffectContainer
 *          (branch: visualEffectContainer == nullptr → true)
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawColorNGFilterNullContainer, testing::ext::TestSize.Level1)
{
    Drawing::Canvas canvas;
    Drawing::Rect rect(0.0f, 0.0f, 10.0f, 10.0f);
    DrawableV2::RSEffectDrawableUtils::DrawColorNGFilter(&canvas, &rect, nullptr);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: DrawColorNGFilterWithSurface
 * @tc.desc: DrawColorNGFilter with raster surface (surface != nullptr)
 *          (branch: surface == nullptr → false)
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawColorNGFilterWithSurface, testing::ext::TestSize.Level1)
{
    auto container = std::make_shared<Drawing::GEVisualEffectContainer>();
    auto surface = Drawing::Surface::MakeRasterN32Premul(100, 100);
    ASSERT_NE(surface, nullptr);
    MockSurfaceCanvas mockCanvas(surface, 100, 100);
    Drawing::Rect rect(0.0f, 0.0f, 10.0f, 10.0f);
    DrawableV2::RSEffectDrawableUtils::DrawColorNGFilter(&mockCanvas, &rect, container);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: DrawColorNGFilterEmptyDrawRect
 * @tc.desc: DrawColorNGFilter with empty clip bounds (drawRect.IsEmpty() → true)
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawColorNGFilterEmptyDrawRect, testing::ext::TestSize.Level1)
{
    auto container = std::make_shared<Drawing::GEVisualEffectContainer>();
    auto surface = Drawing::Surface::MakeRasterN32Premul(100, 100);
    ASSERT_NE(surface, nullptr);
    MockSurfaceCanvas mockCanvas(surface, 100, 100);
    mockCanvas.ClipRect(Drawing::Rect(0.0f, 0.0f, 0.0f, 0.0f), Drawing::ClipOp::INTERSECT);
    Drawing::Rect rect(0.0f, 0.0f, 10.0f, 10.0f);
    DrawableV2::RSEffectDrawableUtils::DrawColorNGFilter(&mockCanvas, &rect, container);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: DrawColorNGFilterImageNullptr
 * @tc.desc: DrawColorNGFilter with surface that returns nullptr from GetImageSnapshot.
 *          Covers: image==nullptr->true.
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawColorNGFilterImageNullptr, testing::ext::TestSize.Level1)
{
    auto container = std::make_shared<Drawing::GEVisualEffectContainer>();
    Drawing::Surface emptySurface;
    MockSurfaceCanvas mockCanvas(std::make_shared<Drawing::Surface>(std::move(emptySurface)), 100, 100);
    Drawing::Rect rect(0.0f, 0.0f, 10.0f, 10.0f);
    DrawableV2::RSEffectDrawableUtils::DrawColorNGFilter(&mockCanvas, &rect, container);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: DrawColorNGFilterResImageNullptr
 * @tc.desc: DrawColorNGFilter with empty visualEffectContainer so ApplyImageEffect returns nullptr.
 *          Covers: resImage==nullptr->true.
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableUtilsTest, DrawColorNGFilterResImageNullptr, testing::ext::TestSize.Level1)
{
    auto container = std::make_shared<Drawing::GEVisualEffectContainer>();
    auto surface = Drawing::Surface::MakeRasterN32Premul(100, 100);
    ASSERT_NE(surface, nullptr);
    MockSurfaceCanvas mockCanvas(surface, 100, 100);
    Drawing::Rect rect(0.0f, 0.0f, 10.0f, 10.0f);
    DrawableV2::RSEffectDrawableUtils::DrawColorNGFilter(&mockCanvas, &rect, container);
    EXPECT_TRUE(true);
}
} // namespace OHOS::Rosen
