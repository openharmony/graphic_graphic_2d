/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "drawable/rs_effect_drawable_utils.h"
#include "drawable/rs_property_drawable_utils.h"
#include "draw/surface.h"
#include "effect/rs_render_filter_base.h"
#include "effect/rs_render_shader_base.h"
#include "effect/rs_render_shape_base.h"
#include "feature/filter_cache_manager/rs_filter_cache_manager.h"
#include "feature/filter_cache_manager/rs_filter_cache_memory_controller.h"
#include "ge_visual_effect_container.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "property/rs_properties.h"
#include "property/rs_spatial_effect_manager.h"
#include "platform/common/rs_system_properties.h"
#include "render/rs_fly_out_shader_filter.h"
#include "render/rs_render_linear_gradient_blur_filter.h"
#include "render/rs_render_mesa_blur_filter.h"
#include "hpae_base/rs_hpae_filter_cache_manager.h"
#include "render/rs_blur_filter.h"
#include "render/rs_foreground_effect_filter.h"
#include "render/rs_material_filter.h"
#include "render/rs_motion_blur_filter.h"
#include "common/rs_obj_abs_geometry.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSEffectDrawableUtilsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSEffectDrawableUtilsTest::SetUpTestCase() {}
void RSEffectDrawableUtilsTest::TearDownTestCase() {}
void RSEffectDrawableUtilsTest::SetUp() {}
void RSEffectDrawableUtilsTest::TearDown() {}

namespace {
Vector2f DARK_BLUR(1.5f, 2.5f);
Vector2f DARK_WEIGHTS(0.1f, 0.2f);
Vector2f DARK_BG_RATES(0.3f, 0.4f);
Vector3f DARK_BG_KBS(0.5f, 0.6f, 0.7f);
Vector3f DARK_BG_POS(0.8f, 0.9f, 1.0f);
Vector3f DARK_BG_NEG(1.1f, 1.2f, 1.3f);
} // namespace

/**
 * @tc.name: CreateDefaultRRectShapeTest
 * @tc.desc: Test Func CreateDefaultRRectShape
 * @tc.type: FUNC
 */
HWTEST_F(RSEffectDrawableUtilsTest, CreateDefaultRRectShapeTest, testing::ext::TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSEffectDrawableUtils> drawableUtils =
        std::make_shared<DrawableV2::RSEffectDrawableUtils>();
    ASSERT_NE(drawableUtils, nullptr);

    RRect rrect(RectT<float>(10.0f, 10.0f, 10.0f, 10.0f), 5.0f, 5.0f);
    auto renderShape = drawableUtils->CreateDefaultRRectShape(rrect, 1);
    EXPECT_NE(renderShape, nullptr);
}

/**
 * @tc.name: ApplySDFShapeToEffectTest
 * @tc.desc: Test Func ApplySDFShapeToEffect
 * @tc.type: FUNC
 */
HWTEST_F(RSEffectDrawableUtilsTest, ApplySDFShapeToEffectTest, testing::ext::TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSEffectDrawableUtils> drawableUtils =
        std::make_shared<DrawableV2::RSEffectDrawableUtils>();
    ASSERT_NE(drawableUtils, nullptr);

    RSProperties properties;
    drawableUtils->ApplySDFShapeToFilter(properties, nullptr, 1);
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::AURORA_NOISE);
    drawableUtils->ApplySDFShapeToEffect(properties, shader, 1);
    properties.SetSDFShape(RSNGRenderShapeBase::Create(RSNGEffectType::SDF_RRECT_SHAPE));
    drawableUtils->ApplySDFShapeToEffect(properties, shader, 1);
}

/**
 * @tc.name: DrawFilterWithDRMTest
 * @tc.desc: Test Func DrawFilterWithDRM
 * @tc.type: FUNC
 */
HWTEST_F(RSEffectDrawableUtilsTest, DrawFilterWithDRMTest, testing::ext::TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSEffectDrawableUtils> drawableUtils =
        std::make_shared<DrawableV2::RSEffectDrawableUtils>();
    ASSERT_NE(drawableUtils, nullptr);

    drawableUtils->DrawFilterWithDRM(nullptr, true, nullptr, nullptr);
    Drawing::Canvas canvas;
    drawableUtils->DrawFilterWithDRM(&canvas, true, nullptr, nullptr);
    Drawing::Rect rect;
    drawableUtils->DrawFilterWithDRM(&canvas, true, &rect, nullptr);

    std::shared_ptr<RSFilter> rsFilter = std::make_shared<RSDrawingFilter>();
    ASSERT_NE(rsFilter, nullptr);
    drawableUtils->DrawFilterWithDRM(&canvas, true, &rect, rsFilter);

    auto drawingFilter = std::make_shared<RSDrawingFilter>();
    ASSERT_NE(drawingFilter, nullptr);
    drawingFilter->SetNGRenderFilter(RSNGRenderFilterBase::Create(RSNGEffectType::BLUR));
    drawingFilter->GenerateAndUpdateGEVisualEffect();
    drawableUtils->DrawFilterWithDRM(&canvas, true, &rect, drawingFilter);

    drawingFilter->SetNGRenderFilter(RSNGRenderFilterBase::Create(RSNGEffectType::FROSTED_GLASS));
    drawingFilter->GenerateAndUpdateGEVisualEffect();
    drawableUtils->DrawFilterWithDRM(&canvas, true, &rect, drawingFilter);
}

/**
 * @tc.name: DrawColorUsingSDFWithDRMTest
 * @tc.desc: Test func DrawColorUsingSDFWithDRM
 * @tc.type: FUNC
 */
HWTEST_F(RSEffectDrawableUtilsTest, DrawColorUsingSDFWithDRMTest, testing::ext::TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSEffectDrawableUtils> drawableUtils =
        std::make_shared<DrawableV2::RSEffectDrawableUtils>();
    ASSERT_NE(drawableUtils, nullptr);

    const std::string effectTag = "FrostedGlass";
    const std::string shapeTag = "FrostedGlass_Shape";
    drawableUtils->DrawColorUsingSDFWithDRM(nullptr, nullptr, true, nullptr, effectTag, shapeTag);
    Drawing::Canvas canvas;
    drawableUtils->DrawColorUsingSDFWithDRM(&canvas, nullptr, true, nullptr, effectTag, shapeTag);
    Drawing::Rect rect;
    drawableUtils->DrawColorUsingSDFWithDRM(&canvas, &rect, true, nullptr, effectTag, shapeTag);
    auto filterGEContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    drawableUtils->DrawColorUsingSDFWithDRM(&canvas, &rect, true, filterGEContainer, effectTag, shapeTag);
    auto geVisualEffect = std::make_shared<Drawing::GEVisualEffect>(effectTag);
    filterGEContainer->AddToChainedFilter(geVisualEffect);
    drawableUtils->DrawColorUsingSDFWithDRM(&canvas, &rect, true, filterGEContainer, effectTag, shapeTag);
}

/**
 * @tc.name: CalcShaderDrawRectTest
 * @tc.desc: Test func CalcShaderDrawRect
 * @tc.type: FUNC
 */
HWTEST_F(RSEffectDrawableUtilsTest, CalcShaderDrawRectTest, testing::ext::TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSEffectDrawableUtils> drawableUtils =
        std::make_shared<DrawableV2::RSEffectDrawableUtils>();
    ASSERT_NE(drawableUtils, nullptr);

    std::weak_ptr<RSContext> context = {};
    // bounds rect equals CalcBoundingBox: boundsGeo_ required by CalcBoundingBox
    auto node = std::make_shared<RSRenderNode>(1, context);
    node->GetMutableRenderProperties().boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    auto& properties = node->GetMutableRenderProperties();
    properties.SetBounds(Vector4f(0.f, 0.f, 100.f, 100.f));
    // shader nullptr -> CalcRect returns empty, fallback to CalcBoundingBox
    auto drawRect = drawableUtils->CalcShaderDrawRect(*node, properties, nullptr);
    EXPECT_EQ(drawRect, node->CalcBoundingBox());
    // real shader: drawRect equals bounds (no expansion rule for AURORA_NOSIE), whole node fits in bounds
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::AURORA_NOISE);
    drawRect = drawableUtils->CalcShaderDrawRect(*node, properties, shader);
    EXPECT_EQ(drawRect, node->CalcBoundingBox());
}

/**
 * @tc.name: DrawBackgroundEffectTest
 * @tc.desc: Test func DrawBackgroundEffect
 * @tc.type: FUNC
 */
HWTEST_F(RSEffectDrawableUtilsTest, DrawBackgroundEffectTest, testing::ext::TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSEffectDrawableUtils> drawableUtils =
        std::make_shared<DrawableV2::RSEffectDrawableUtils>();
    ASSERT_NE(drawableUtils, nullptr);
    Drawing::RectI bounds(0, 0, 10, 10);
    std::unique_ptr<RSFilterCacheManager> cacheManager = std::make_unique<RSFilterCacheManager>();
    auto rsFilter = std::make_shared<RSDrawingFilter>();
    ASSERT_NE(rsFilter, nullptr);
    rsFilter->SetNGRenderFilter(RSNGRenderFilterBase::Create(RSNGEffectType::BLUR));

    // null filter / null canvas
    drawableUtils->DrawBackgroundEffect(nullptr, nullptr, 0, cacheManager, bounds, false);
    drawableUtils->DrawBackgroundEffect(nullptr, rsFilter, 0, cacheManager, bounds, false);
    // canvas without surface
    Drawing::Canvas rawCanvas;
    RSPaintFilterCanvas canvasNoSurface(&rawCanvas);
    canvasNoSurface.surface_ = nullptr;
    drawableUtils->DrawBackgroundEffect(&canvasNoSurface, rsFilter, 0, cacheManager, bounds, false);
    // surface nullptr via GetSurface
    drawableUtils->DrawBackgroundEffect(&canvasNoSurface, rsFilter, 0, cacheManager, bounds, false);
    // frosted glass blur info with raster surface
    auto surfacePtr = Drawing::Surface::MakeRasterN32Premul(20, 20); // surface larger than bounds
    ASSERT_NE(surfacePtr, nullptr);
    RSPaintFilterCanvas rasterCanvas(surfacePtr.get());
    drawableUtils->DrawBackgroundEffect(&rasterCanvas, rsFilter, 0, cacheManager, bounds, false);
    // normal path (not frosted glass)
    drawableUtils->DrawBackgroundEffect(&rasterCanvas, rsFilter, 0, cacheManager, bounds, false);
    // behindWindow
    drawableUtils->DrawBackgroundEffect(&rasterCanvas, rsFilter, 0, cacheManager, bounds, true);
    // no cacheManager
    drawableUtils->DrawBackgroundEffect(&rasterCanvas, rsFilter, 0, nullptr, bounds, false);
}

/**
 * @tc.name: DrawBackgroundEffect_WithCacheManager
 * @tc.desc: Test func DrawBackgroundEffect use cacheManager to draw
 * @tc.type: FUNC
 */
HWTEST_F(RSEffectDrawableUtilsTest, DrawBackgroundEffect_WithCacheManager, testing::ext::TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSEffectDrawableUtils> drawableUtils =
        std::make_shared<DrawableV2::RSEffectDrawableUtils>();
    ASSERT_NE(drawableUtils, nullptr);
    RSFilterCacheManager::SetFilterCacheEnabledByCCM(true);
    RSFilterCacheMemoryController::Instance().SetScreenRectInfo(Drawing::RectI(0, 0, 1000, 1000));

    auto rsFilter = std::make_shared<RSDrawingFilter>();
    ASSERT_NE(rsFilter, nullptr);
    rsFilter->SetNGRenderFilter(RSNGRenderFilterBase::Create(RSNGEffectType::BLUR));
    rsFilter->GenerateAndUpdateGEVisualEffect();

    auto surfacePtr = Drawing::Surface::MakeRasterN32Premul(20, 20);
    ASSERT_NE(surfacePtr, nullptr);
    RSPaintFilterCanvas rasterCanvas(surfacePtr.get());

    std::unique_ptr<RSFilterCacheManager> cacheManager = std::make_unique<RSFilterCacheManager>();
    ASSERT_NE(cacheManager, nullptr);
    Drawing::RectI bounds(0, 0, 10, 10);

    EXPECT_TRUE(RSFilterCacheManager::IsFilterCacheEnabled());
    EXPECT_FALSE(rasterCanvas.GetDisableFilterCache());
    EXPECT_FALSE(cacheManager->IsFilterCacheMemExceedThreshold());

    drawableUtils->DrawBackgroundEffect(&rasterCanvas, rsFilter, 0, cacheManager, bounds, false);
    EXPECT_NE(rasterCanvas.GetEffectData(), nullptr);
    EXPECT_NE(cacheManager->GetCachedFilteredSnapshot(), nullptr);
    EXPECT_EQ(cacheManager->GetCachedType(), FilterCacheType::FILTERED_SNAPSHOT);
    drawableUtils->DrawBackgroundEffect(&rasterCanvas, rsFilter, 0, cacheManager, bounds, true);
    EXPECT_NE(rasterCanvas.GetBehindWindowData(), nullptr);

    // disable filter cache
    RSFilterCacheManager::SetFilterCacheEnabledByCCM(false);
    drawableUtils->DrawBackgroundEffect(&rasterCanvas, rsFilter, 0, cacheManager, bounds, true);
    // re-enable
    RSFilterCacheManager::SetFilterCacheEnabledByCCM(true);
}

/**
 * @tc.name: ApplyAdaptiveFrostedGlassParamsTest
 * @tc.desc: Test func ApplyAdaptiveFrostedGlassParams
 * @tc.type: FUNC
 */
HWTEST_F(RSEffectDrawableUtilsTest, ApplyAdaptiveFrostedGlassParamsTest, testing::ext::TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSEffectDrawableUtils> drawableUtils =
        std::make_shared<DrawableV2::RSEffectDrawableUtils>();
    ASSERT_NE(drawableUtils, nullptr);
    drawableUtils->ApplyAdaptiveFrostedGlassParams(nullptr, nullptr);
    auto surfacePtr = Drawing::Surface::MakeRasterN32Premul(20, 20);
    ASSERT_NE(surfacePtr, nullptr);
    RSPaintFilterCanvas paintFilterCanvas(surfacePtr.get());
    drawableUtils->ApplyAdaptiveFrostedGlassParams(&paintFilterCanvas, nullptr);

    auto drawingFilter = std::make_shared<RSDrawingFilter>();
    ASSERT_NE(drawingFilter, nullptr);
    drawableUtils->ApplyAdaptiveFrostedGlassParams(&paintFilterCanvas, drawingFilter);
    EXPECT_EQ(drawingFilter->GetGEContainer(), nullptr);
    drawingFilter->SetNGRenderFilter(RSNGRenderFilterBase::Create(RSNGEffectType::BLUR));
    drawingFilter->GenerateAndUpdateGEVisualEffect();
    EXPECT_NE(drawingFilter->GetGEContainer(), nullptr);
    EXPECT_FALSE(drawingFilter->GetGEContainer()->GetGEVisualEffect(Drawing::GE_FILTER_FROSTED_GLASS));
    drawableUtils->ApplyAdaptiveFrostedGlassParams(&paintFilterCanvas, drawingFilter);

    drawingFilter->SetNGRenderFilter(RSNGRenderFilterBase::Create(RSNGEffectType::FROSTED_GLASS));
    drawingFilter->GenerateAndUpdateGEVisualEffect();
    EXPECT_TRUE(drawingFilter->GetGEContainer()->GetGEVisualEffect(Drawing::GE_FILTER_FROSTED_GLASS));
    drawableUtils->ApplyAdaptiveFrostedGlassParams(&paintFilterCanvas, drawingFilter);
}

/**
 * @tc.name: ClipVisibleRectTest
 * @tc.desc: Test func ClipVisibleRect
 * @tc.type: FUNC
 */
HWTEST_F(RSEffectDrawableUtilsTest, ClipVisibleRectTest, testing::ext::TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSEffectDrawableUtils> drawableUtils =
        std::make_shared<DrawableV2::RSEffectDrawableUtils>();
    ASSERT_NE(drawableUtils, nullptr);
    Drawing::Canvas canvas(400, 400);
    canvas.ClipRect(Drawing::Rect(0, 0, 100, 100));
    RSPaintFilterCanvas pCanvas(&canvas);
    EXPECT_EQ(Drawing::RectI(0, 0, 100, 100), pCanvas.GetDeviceClipBounds());

    Drawing::Rect emptyVisibleRect  = Drawing::Rect(0, 0, 0, 0);
    pCanvas.SetVisibleRect(emptyVisibleRect);
    EXPECT_EQ(emptyVisibleRect, pCanvas.GetVisibleRect());
    EXPECT_TRUE(pCanvas.GetVisibleRect().IsEmpty());
    drawableUtils->ClipVisibleRect(&pCanvas);

    Drawing::Rect visibleRect = Drawing::Rect(0, 0, 50, 50);
    pCanvas.SetVisibleRect(visibleRect);
    EXPECT_EQ(visibleRect, pCanvas.GetVisibleRect());
    EXPECT_FALSE(pCanvas.GetVisibleRect().IsEmpty());
    drawableUtils->ClipVisibleRect(&pCanvas);

    Drawing::Rect visibleNoIntersectRect = Drawing::Rect(150, 150, 200, 200);
    pCanvas.SetVisibleRect(visibleNoIntersectRect);
    EXPECT_EQ(visibleNoIntersectRect, pCanvas.GetVisibleRect());
    EXPECT_FALSE(pCanvas.GetVisibleRect().IsEmpty());
    drawableUtils->ClipVisibleRect(&pCanvas);
}

/**
 * @tc.name: DrawFilterTest
 * @tc.desc: Test func DrawFilter
 * @tc.type: FUNC
 */
HWTEST_F(RSEffectDrawableUtilsTest, DrawFilterTest, testing::ext::TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSEffectDrawableUtils> drawableUtils =
        std::make_shared<DrawableV2::RSEffectDrawableUtils>();
    ASSERT_NE(drawableUtils, nullptr);
    auto drawingFilter = std::make_shared<RSDrawingFilter>();
    ASSERT_NE(drawingFilter, nullptr);
    drawingFilter->SetNGRenderFilter(RSNGRenderFilterBase::Create(RSNGEffectType::BLUR));
    std::unique_ptr<RSFilterCacheManager> cacheManager = std::make_unique<RSFilterCacheManager>();

    Drawing::Canvas rawCanvas;
    RSPaintFilterCanvas pCanvas(&rawCanvas);
    pCanvas.SetVisibleRect(Drawing::Rect(0.0f, 0.0f, 10.0f, 10.0f));

    drawableUtils->DrawFilter(nullptr, nullptr, cacheManager, 1, false, std::nullopt, std::nullopt);
    drawableUtils->DrawFilter(nullptr, drawingFilter, cacheManager, 1, false, std::nullopt, std::nullopt);
    drawableUtils->DrawFilter(&pCanvas, drawingFilter, cacheManager, 1, false, std::nullopt, std::nullopt);

    auto surfacePtr = Drawing::Surface::MakeRasterN32Premul(20, 20);
    ASSERT_NE(surfacePtr, nullptr);
    RSPaintFilterCanvas rasterCanvas(surfacePtr.get());
    rasterCanvas.SetVisibleRect(Drawing::Rect(0.0f, 0.0f, 10.0f, 10.0f));
    drawableUtils->DrawFilter(&rasterCanvas, drawingFilter, cacheManager, 1, true, std::nullopt, std::nullopt);
    drawableUtils->DrawFilter(&rasterCanvas, drawingFilter, cacheManager, 1, false, std::nullopt, std::nullopt);
    drawableUtils->DrawFilter(&rasterCanvas, drawingFilter, cacheManager, 1, false,
        Drawing::RectI(0, 0, 10, 10), Drawing::RectI(0, 0, 10, 10));

    auto para = std::make_shared<RSLinearGradientBlurPara>(
        10.f, std::vector<std::pair<float, float>> {{0.f, 0.f}, {1.f, 1.f}}, GradientDirection::BOTTOM);
    auto lgbFilter = std::make_shared<RSLinearGradientBlurShaderFilter>(para, 100.f, 100.f);
    drawingFilter->SetFilterType(RSFilter::LINEAR_GRADIENT_BLUR);
    drawingFilter->InsertShaderFilter(lgbFilter);
    EXPECT_NE(drawingFilter->GetShaderFilterWithType(RSUIFilterType::LINEAR_GRADIENT_BLUR), nullptr);
    drawableUtils->DrawFilter(&rasterCanvas, drawingFilter, cacheManager, 1, false, std::nullopt, std::nullopt);
    drawableUtils->DrawFilter(&rasterCanvas, drawingFilter, nullptr, 1, false, Drawing::RectI(), Drawing::RectI());
}

/**
 * @tc.name: DrawFilter_WithCacheManager
 * @tc.desc: Test func DrawFilter use cacheManager to draw
 * @tc.type: FUNC
 */
HWTEST_F(RSEffectDrawableUtilsTest, DrawFilter_WithCacheManager, testing::ext::TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSEffectDrawableUtils> drawableUtils =
        std::make_shared<DrawableV2::RSEffectDrawableUtils>();
    ASSERT_NE(drawableUtils, nullptr);
    RSFilterCacheManager::SetFilterCacheEnabledByCCM(true);
    RSFilterCacheMemoryController::Instance().SetScreenRectInfo(Drawing::RectI(0, 0, 1000, 1000));

    auto drawingFilter = std::make_shared<RSDrawingFilter>();
    ASSERT_NE(drawingFilter, nullptr);
    drawingFilter->SetNGRenderFilter(RSNGRenderFilterBase::Create(RSNGEffectType::BLUR));

    auto surfacePtr = Drawing::Surface::MakeRasterN32Premul(20, 20);
    ASSERT_NE(surfacePtr, nullptr);
    RSPaintFilterCanvas rasterCanvas(surfacePtr.get());

    std::unique_ptr<RSFilterCacheManager> cacheManager = std::make_unique<RSFilterCacheManager>();
    ASSERT_NE(cacheManager, nullptr);

    EXPECT_TRUE(RSFilterCacheManager::IsFilterCacheEnabled());
    EXPECT_FALSE(rasterCanvas.GetDisableFilterCache());
    EXPECT_FALSE(cacheManager->IsFilterCacheMemExceedThreshold());

    drawableUtils->DrawFilter(&rasterCanvas, drawingFilter, cacheManager, 0, false,
        Drawing::RectI(0, 0, 10, 10), Drawing::RectI(0, 0, 10, 10));
    EXPECT_NE(cacheManager->GetCachedFilteredSnapshot(), nullptr);

    EXPECT_EQ(cacheManager->GetCachedType(), FilterCacheType::FILTERED_SNAPSHOT);
    drawableUtils->DrawFilter(&rasterCanvas, drawingFilter, cacheManager, 0, true, std::nullopt, std::nullopt);

    auto para = std::make_shared<RSLinearGradientBlurPara>(
        10.f, std::vector<std::pair<float, float>> {{0.f, 0.f}, {1.f, 1.f}}, GradientDirection::BOTTOM);
    auto lgbFilter = std::make_shared<RSLinearGradientBlurShaderFilter>(para, 100.f, 100.f);
    drawingFilter->SetFilterType(RSFilter::LINEAR_GRADIENT_BLUR);
    drawingFilter->InsertShaderFilter(lgbFilter);
    EXPECT_NE(drawingFilter->GetShaderFilterWithType(RSUIFilterType::LINEAR_GRADIENT_BLUR), nullptr);
    EXPECT_EQ(cacheManager->GetCachedType(), FilterCacheType::FILTERED_SNAPSHOT);
    drawableUtils->DrawFilter(&rasterCanvas, drawingFilter, cacheManager, 0, true, std::nullopt, std::nullopt);

    // disable filter cache
    RSFilterCacheManager::SetFilterCacheEnabledByCCM(false);
    drawableUtils->DrawFilter(&rasterCanvas, drawingFilter, cacheManager, 0, true, std::nullopt, std::nullopt);
    // re-enable
    RSFilterCacheManager::SetFilterCacheEnabledByCCM(false);
}

/**
 * @tc.name: RSFilterSetPixelStretchTest
 * @tc.desc: Test func RSFilterSetPixelStretch
 * @tc.type: FUNC
 */
HWTEST_F(RSEffectDrawableUtilsTest, RSFilterSetPixelStretchTest, testing::ext::TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSEffectDrawableUtils> drawableUtils =
        std::make_shared<DrawableV2::RSEffectDrawableUtils>();
    ASSERT_NE(drawableUtils, nullptr);
    RSProperties properties;
    properties.SetBounds(Vector4f(0.f, 0.f, 100.f, 100.f));
    // null filter
    EXPECT_FALSE(drawableUtils->RSFilterSetPixelStretch(properties, nullptr));

    // build MESA drawing filter
    auto rsFilter = std::make_shared<RSDrawingFilter>();
    ASSERT_NE(rsFilter, nullptr);
    auto mesaBlurFilter = std::make_shared<RSMESABlurShaderFilter>(1);
    ASSERT_NE(mesaBlurFilter, nullptr);
    rsFilter->InsertShaderFilter(mesaBlurFilter);

    // pixel stretch all zero (default) -> not fuzed regardless of the fuzed switch
    EXPECT_FALSE(drawableUtils->RSFilterSetPixelStretch(properties, rsFilter));
    // negative stretch factors -> fuzed when the MESA fuzed switch is on (platform dependent)
    bool fuzedEnabled = RSSystemProperties::GetMESABlurFuzedEnabled();
    properties.SetPixelStretch(Vector4f(-5.f, -5.f, -5.f, -5.f));
    EXPECT_EQ(drawableUtils->RSFilterSetPixelStretch(properties, rsFilter), fuzedEnabled);
    // positive stretch factor -> not fuzed
    properties.SetPixelStretch(Vector4f(5.f, -5.f, -5.f, -5.f));
    EXPECT_FALSE(drawableUtils->RSFilterSetPixelStretch(properties, rsFilter));
    properties.SetPixelStretch(Vector4f(-5.f, 5.f, -5.f, -5.f));
    EXPECT_FALSE(drawableUtils->RSFilterSetPixelStretch(properties, rsFilter));
    properties.SetPixelStretch(Vector4f(-5.f, -5.f, 5.f, -5.f));
    EXPECT_FALSE(drawableUtils->RSFilterSetPixelStretch(properties, rsFilter));
    properties.SetPixelStretch(Vector4f(-5.f, -5.f, -5.f, 5.f));
    EXPECT_FALSE(drawableUtils->RSFilterSetPixelStretch(properties, rsFilter));
    // filter without MESA shader filter
    auto blurOnlyFilter = std::make_shared<RSDrawingFilter>();
    blurOnlyFilter->SetNGRenderFilter(RSNGRenderFilterBase::Create(RSNGEffectType::BLUR));
    EXPECT_FALSE(drawableUtils->RSFilterSetPixelStretch(properties, blurOnlyFilter));
}

/**
 * @tc.name: RSFilterRemovePixelStretchTest
 * @tc.desc: Test func RSFilterRemovePixelStretch
 * @tc.type: FUNC
 */
HWTEST_F(RSEffectDrawableUtilsTest, RSFilterRemovePixelStretchTest, testing::ext::TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSEffectDrawableUtils> drawableUtils =
        std::make_shared<DrawableV2::RSEffectDrawableUtils>();
    ASSERT_NE(drawableUtils, nullptr);
    // null filter
    drawableUtils->RSFilterRemovePixelStretch(nullptr);

    // filter without MESA shader filter
    auto blurOnlyFilter = std::make_shared<RSDrawingFilter>();
    blurOnlyFilter->SetNGRenderFilter(RSNGRenderFilterBase::Create(RSNGEffectType::BLUR));
    drawableUtils->RSFilterRemovePixelStretch(blurOnlyFilter);

    // filter with MESA shader filter -> params reset to nullptr
    auto rsFilter = std::make_shared<RSDrawingFilter>();
    ASSERT_NE(rsFilter, nullptr);
    auto mesaBlurFilter = std::make_shared<RSMESABlurShaderFilter>(1);
    ASSERT_NE(mesaBlurFilter, nullptr);
    rsFilter->InsertShaderFilter(mesaBlurFilter);
    RSProperties properties;
    properties.SetBounds(Vector4f(0.f, 0.f, 100.f, 100.f));
    properties.SetPixelStretch(Vector4f(-5.f, -5.f, -5.f, -5.f));
    EXPECT_EQ(drawableUtils->RSFilterSetPixelStretch(properties, rsFilter),
        RSSystemProperties::GetMESABlurFuzedEnabled());
    drawableUtils->RSFilterRemovePixelStretch(rsFilter);
    SUCCEED();
}

/**
 * @tc.name: GenerateMaterialColorFilterTest
 * @tc.desc: Test func GenerateMaterialColorFilter
 * @tc.type: FUNC
 */
HWTEST_F(RSEffectDrawableUtilsTest, GenerateMaterialColorFilterTest, testing::ext::TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSEffectDrawableUtils> drawableUtils =
        std::make_shared<DrawableV2::RSEffectDrawableUtils>();
    ASSERT_NE(drawableUtils, nullptr);
    float saturation = 1.9f;
    float brightness = 1.0f;
    auto filter = drawableUtils->GenerateMaterialColorFilter(saturation, brightness);
    ASSERT_NE(filter, nullptr);
}

/**
 * @tc.name: GenerateBehindWindowFilterTest
 * @tc.desc: Test func GenerateBehindWindowFilter
 * @tc.type: FUNC
 */
HWTEST_F(RSEffectDrawableUtilsTest, GenerateBehindWindowFilterTest, testing::ext::TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSEffectDrawableUtils> drawableUtils =
        std::make_shared<DrawableV2::RSEffectDrawableUtils>();
    ASSERT_NE(drawableUtils, nullptr);

    float radius = 5.0f;
    float saturation = 1.5f;
    float brightness = 0.8f;
    RSColor maskColor;
    auto filter = drawableUtils->GenerateBehindWindowFilter(radius, saturation, brightness, maskColor);
    ASSERT_NE(filter, nullptr);
    EXPECT_EQ(filter->GetFilterType(), RSFilter::MATERIAL);
}

/**
 * @tc.name: IsBlurFilterTypeTest
 * @tc.desc: Test func IsBlurFilterType
 * @tc.type: FUNC
 */
HWTEST_F(RSEffectDrawableUtilsTest, IsBlurFilterTypeTest, testing::ext::TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSEffectDrawableUtils> drawableUtils =
        std::make_shared<DrawableV2::RSEffectDrawableUtils>();
    ASSERT_NE(drawableUtils, nullptr);
    EXPECT_TRUE(drawableUtils->IsBlurFilterType(RSFilter::BLUR));
    EXPECT_TRUE(drawableUtils->IsBlurFilterType(RSFilter::MATERIAL));
    EXPECT_TRUE(drawableUtils->IsBlurFilterType(RSFilter::LINEAR_GRADIENT_BLUR));
    EXPECT_TRUE(drawableUtils->IsBlurFilterType(RSFilter::FOREGROUND_EFFECT));
    EXPECT_TRUE(drawableUtils->IsBlurFilterType(RSFilter::MOTION_BLUR));
    EXPECT_FALSE(drawableUtils->IsBlurFilterType(RSFilter::NONE));
}

/**
 * @tc.name: GetBlurFilterRadiusTest
 * @tc.desc: Test func GetBlurFilterRadius
 * @tc.type: FUNC
 */
HWTEST_F(RSEffectDrawableUtilsTest, GetBlurFilterRadiusTest, testing::ext::TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSEffectDrawableUtils> drawableUtils =
        std::make_shared<DrawableV2::RSEffectDrawableUtils>();
    ASSERT_NE(drawableUtils, nullptr);
    // null filter
    EXPECT_EQ(drawableUtils->GetBlurFilterRadius(nullptr), 0.f);

    // BLUR
    auto blurFilter = std::make_shared<RSBlurFilter>(10.f, 10.f);
    ASSERT_NE(blurFilter, nullptr);
    EXPECT_EQ(drawableUtils->GetBlurFilterRadius(blurFilter), 10.f);

    // MATERIAL
    auto materialFilter = std::make_shared<RSMaterialFilter>(1, 1.f, BLUR_COLOR_MODE::DEFAULT, 1.f);
    ASSERT_NE(materialFilter, nullptr);
    EXPECT_GE(drawableUtils->GetBlurFilterRadius(materialFilter), 0.f);

    // FOREGROUND_EFFECT
    auto foregroundEffectFilter = std::make_shared<RSForegroundEffectFilter>(10.f);
    ASSERT_NE(foregroundEffectFilter, nullptr);
    EXPECT_EQ(drawableUtils->GetBlurFilterRadius(foregroundEffectFilter), 39.f);

    // MOTION_BLUR
    Vector2f anchor(0.f, 0.f);
    auto motionBlurParam = std::make_shared<MotionBlurParam>(10.f, anchor);
    auto motionBlurFilter = std::make_shared<RSMotionBlurFilter>(motionBlurParam);
    ASSERT_NE(motionBlurFilter, nullptr);
    EXPECT_EQ(drawableUtils->GetBlurFilterRadius(motionBlurFilter), 10.f);

    // LINEAR_GRADIENT_BLUR (drawing filter, shader filter exists / not exists)
    auto linearGradientBlurPara = std::make_shared<RSLinearGradientBlurPara>(10.f,
        std::vector<std::pair<float, float>> { { 0.f, 0.f }, { 1.f, 1.f } },
        GradientDirection::BOTTOM);
    ASSERT_NE(linearGradientBlurPara, nullptr);
    auto linearGradientBlurFilter = std::make_shared<RSLinearGradientBlurShaderFilter>(
        linearGradientBlurPara, 100.f, 100.f);
    ASSERT_NE(linearGradientBlurFilter, nullptr);
    auto drawingFilter = std::make_shared<RSDrawingFilter>();
    ASSERT_NE(drawingFilter, nullptr);
    drawingFilter->SetFilterType(RSFilter::LINEAR_GRADIENT_BLUR);
    drawingFilter->InsertShaderFilter(linearGradientBlurFilter);
    EXPECT_GT(drawableUtils->GetBlurFilterRadius(drawingFilter), 0.f);
    auto drawingFilterNoShader = std::make_shared<RSDrawingFilter>();
    drawingFilterNoShader->SetFilterType(RSFilter::LINEAR_GRADIENT_BLUR);
    EXPECT_EQ(drawableUtils->GetBlurFilterRadius(drawingFilterNoShader), 0.f);

    // default
    auto noneFilter = std::make_shared<RSFilter>();
    ASSERT_NE(noneFilter, nullptr);
    EXPECT_EQ(drawableUtils->GetBlurFilterRadius(noneFilter), 0.f);
}

/**
 * @tc.name: ApplySDFShapeToFilterTest
 * @tc.desc: Test func ApplySDFShapeToFilter 
 * @tc.type: FUNC
 */
HWTEST_F(RSEffectDrawableUtilsTest, ApplySDFShapeToFilterTest, testing::ext::TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSEffectDrawableUtils> drawableUtils =
        std::make_shared<DrawableV2::RSEffectDrawableUtils>();
    ASSERT_NE(drawableUtils, nullptr);
    RSProperties properties;
    properties.SetBounds(Vector4f(0.f, 0.f, 100.f, 100.f));
    NodeId nodeId = 1;

    // null drawing filter
    drawableUtils->ApplySDFShapeToFilter(properties, nullptr, nodeId);

    auto drawingFilter = std::make_shared<RSDrawingFilter>();
    ASSERT_NE(drawingFilter, nullptr);
    // drawing filter without NG render filter
    drawableUtils->ApplySDFShapeToFilter(properties, drawingFilter, nodeId);

    // SDF_EDGE_LIGHT filter with explicit sdf shape
    drawingFilter->SetNGRenderFilter(RSNGRenderFilterBase::Create(RSNGEffectType::SDF_EDGE_LIGHT));
    properties.SetSDFShape(RSNGRenderShapeBase::Create(RSNGEffectType::SDF_RRECT_SHAPE));
    drawableUtils->ApplySDFShapeToFilter(properties, drawingFilter, nodeId);
    // SDF_EDGE_LIGHT filter without explicit sdf shape (early return inside if)
    properties.SetSDFShape(nullptr);
    drawableUtils->ApplySDFShapeToFilter(properties, drawingFilter, nodeId);

    // MAGNIFIER filter (ApplySDFShapeToMagnifier, both explicit/implicit shape paths)
    drawableUtils->ApplySDFShapeToMagnifier(properties, nullptr, nodeId);
    drawingFilter->SetNGRenderFilter(RSNGRenderFilterBase::Create(RSNGEffectType::MAGNIFIER));
    properties.SetSDFShape(RSNGRenderShapeBase::Create(RSNGEffectType::SDF_RRECT_SHAPE));
    drawableUtils->ApplySDFShapeToFilter(properties, drawingFilter, nodeId);
    properties.SetSDFShape(nullptr);
    drawableUtils->ApplySDFShapeToFilter(properties, drawingFilter, nodeId);

    // FROSTED_GLASS filter with explicit sdf shape / with default rrect fallback
    drawingFilter->SetNGRenderFilter(RSNGRenderFilterBase::Create(RSNGEffectType::FROSTED_GLASS));
    properties.SetSDFShape(RSNGRenderShapeBase::Create(RSNGEffectType::SDF_RRECT_SHAPE));
    drawableUtils->ApplySDFShapeToFilter(properties, drawingFilter, nodeId);
    properties.SetSDFShape(nullptr);
    drawableUtils->ApplySDFShapeToFilter(properties, drawingFilter, nodeId);

    // other filter type (neither SDF_EDGE_LIGHT / MAGNIFIER / FROSTED_GLASS)
    drawingFilter->SetNGRenderFilter(RSNGRenderFilterBase::Create(RSNGEffectType::BLUR));
    drawableUtils->ApplySDFShapeToFilter(properties, drawingFilter, nodeId);
}

/**
 * @tc.name: GetAndResetBlurCntTest
 * @tc.desc: Test func GetAndResetBlurCnt
 * @tc.type: FUNC
 */
HWTEST_F(RSEffectDrawableUtilsTest, GetAndResetBlurCntTest, testing::ext::TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSEffectDrawableUtils> drawableUtils =
        std::make_shared<DrawableV2::RSEffectDrawableUtils>();
    ASSERT_NE(drawableUtils, nullptr);
    drawableUtils->GetAndResetBlurCnt();
    drawableUtils->IncBlurCnt();
    drawableUtils->IncBlurCnt();
    drawableUtils->IncBlurCnt();
    drawableUtils->IncBlurCnt();
    drawableUtils->DecBlurCnt();
    EXPECT_EQ(drawableUtils->GetAndResetBlurCnt(), 3);
    EXPECT_EQ(drawableUtils->GetAndResetBlurCnt(), 0);
}

/**
 * @tc.name: SetPointLightData001
 * @tc.desc: Test SetPointLightData with null shader, FindShader returns null and returns early
 * @tc.type: FUNC
 */
HWTEST_F(RSEffectDrawableUtilsTest, SetPointLightData001, testing::ext::TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    node.GetMutableRenderProperties().SetBounds(Vector4f(0.0f, 0.0f, 100.0f, 100.0f));
    std::shared_ptr<RSNGRenderShaderBase> shader = nullptr;
    DrawableV2::RSEffectDrawableUtils::SetPointLightData(shader, node);
    EXPECT_EQ(shader, nullptr);
}

/**
 * @tc.name: SetPointLightData002
 * @tc.desc: Test SetPointLightData with a shader chain that contains no POINT_LIGHT shader
 * @tc.type: FUNC
 */
HWTEST_F(RSEffectDrawableUtilsTest, SetPointLightData002, testing::ext::TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    node.GetMutableRenderProperties().SetBounds(Vector4f(0.0f, 0.0f, 100.0f, 100.0f));
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::AURORA_NOISE);
    ASSERT_NE(shader, nullptr);
    DrawableV2::RSEffectDrawableUtils::SetPointLightData(shader, node);
    EXPECT_NE(shader->GetType(), RSNGEffectType::POINT_LIGHT);
}

/**
 * @tc.name: SetPointLightData003
 * @tc.desc: Test SetPointLightData with POINT_LIGHT shader but illuminated is null
 *           (SetPointLightPropToShader early-return on !illuminated branch)
 * @tc.type: FUNC
 */
HWTEST_F(RSEffectDrawableUtilsTest, SetPointLightData003, testing::ext::TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    node.GetMutableRenderProperties().SetBounds(Vector4f(0.0f, 0.0f, 100.0f, 100.0f));
    EXPECT_EQ(node.GetRenderProperties().GetIlluminated(), nullptr);
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::POINT_LIGHT);
    ASSERT_NE(shader, nullptr);
    DrawableV2::RSEffectDrawableUtils::SetPointLightData(shader, node);
    auto pls = std::static_pointer_cast<RSNGRenderPointLight>(shader);
    EXPECT_TRUE(pls->Getter<PointLightLightPositionsRenderTag>()->Get().empty());
    EXPECT_TRUE(pls->Getter<PointLightLightColorsRenderTag>()->Get().empty());
    EXPECT_TRUE(pls->Getter<PointLightLightIntensitiesRenderTag>()->Get().empty());
}

/**
 * @tc.name: SetPointLightData004
 * @tc.desc: Test SetPointLightData with POINT_LIGHT shader and illuminated not valid (NONE type)
 *           (SetPointLightPropToShader !IsIlluminatedValid branch)
 * @tc.type: FUNC
 */
HWTEST_F(RSEffectDrawableUtilsTest, SetPointLightData004, testing::ext::TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    node.GetMutableRenderProperties().SetBounds(Vector4f(0.0f, 0.0f, 100.0f, 100.0f));
    node.GetMutableRenderProperties().SetIlluminatedBorderWidth(2.0f);
    ASSERT_NE(node.GetRenderProperties().GetIlluminated(), nullptr);
    EXPECT_FALSE(node.GetRenderProperties().GetIlluminated()->IsIlluminatedValid());
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::POINT_LIGHT);
    ASSERT_NE(shader, nullptr);
    DrawableV2::RSEffectDrawableUtils::SetPointLightData(shader, node);
    auto pls = std::static_pointer_cast<RSNGRenderPointLight>(shader);
    EXPECT_TRUE(pls->Getter<PointLightLightPositionsRenderTag>()->Get().empty());
}

/**
 * @tc.name: SetPointLightData005
 * @tc.desc: Test SetPointLightData with valid illuminated but empty lightMap
 *           (SetPointLightPropToShader ClearPointLight branch)
 * @tc.type: FUNC
 */
HWTEST_F(RSEffectDrawableUtilsTest, SetPointLightData005, testing::ext::TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    node.GetMutableRenderProperties().SetBounds(Vector4f(0.0f, 0.0f, 100.0f, 100.0f));
    auto& props = node.GetMutableRenderProperties();
    props.SetIlluminatedBorderWidth(1.0f);
    IlluminatedType type = IlluminatedType::BORDER;
    props.GetEffectProperties().GetShaderEffect().illuminatedPtr_->SetIlluminatedType(type);
    ASSERT_NE(node.GetRenderProperties().GetIlluminated(), nullptr);
    EXPECT_TRUE(node.GetRenderProperties().GetIlluminated()->IsIlluminatedValid());
    EXPECT_TRUE(node.GetRenderProperties().GetIlluminated()->GetLightSourcesAndPosMap().empty());
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::POINT_LIGHT);
    ASSERT_NE(shader, nullptr);
    DrawableV2::RSEffectDrawableUtils::SetPointLightData(shader, node);
    auto pls = std::static_pointer_cast<RSNGRenderPointLight>(shader);
    EXPECT_TRUE(pls->Getter<PointLightLightPositionsRenderTag>()->Get().empty());
    EXPECT_TRUE(pls->Getter<PointLightLightColorsRenderTag>()->Get().empty());
    EXPECT_TRUE(pls->Getter<PointLightLightIntensitiesRenderTag>()->Get().empty());
}

/**
 * @tc.name: SetPointLightData006
 * @tc.desc: Test SetPointLightData with one valid light source, type BORDER (not NORMAL_BORDER_CONTENT)
 *           so enableEDREffect is false
 * @tc.type: FUNC
 */
HWTEST_F(RSEffectDrawableUtilsTest, SetPointLightData006, testing::ext::TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    node.GetMutableRenderProperties().SetBounds(Vector4f(0.0f, 0.0f, 100.0f, 100.0f));
    auto& props = node.GetMutableRenderProperties();
    props.SetIlluminatedBorderWidth(1.0f);
    IlluminatedType type = IlluminatedType::BORDER;
    props.GetEffectProperties().GetShaderEffect().illuminatedPtr_->SetIlluminatedType(type);
    RSLightSource lightSource;
    lightSource.SetLightIntensity(2.0f);
    lightSource.SetLightColor(Color(255, 128, 64, 255));
    Vector4f pos(10.0f, 20.0f, 0.0f, 0.0f);
    props.GetIlluminated()->AddLightSourcesAndPos(1001, lightSource, pos);
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::POINT_LIGHT);
    ASSERT_NE(shader, nullptr);
    DrawableV2::RSEffectDrawableUtils::SetPointLightData(shader, node);
    auto pls = std::static_pointer_cast<RSNGRenderPointLight>(shader);
    EXPECT_EQ(pls->Getter<PointLightLightPositionsRenderTag>()->Get().size(), 1u);
    EXPECT_EQ(pls->Getter<PointLightLightColorsRenderTag>()->Get().size(), 1u);
    EXPECT_EQ(pls->Getter<PointLightLightIntensitiesRenderTag>()->Get().size(), 1u);
    EXPECT_FALSE(pls->Getter<PointLightEnableEDREffectRenderTag>()->Get());
}

/**
 * @tc.name: SetPointLightData007
 * @tc.desc: Test SetPointLightData with NORMAL_BORDER_CONTENT type and light intensity > 1.0
 *           so enableEDREffect is true
 * @tc.type: FUNC
 */
HWTEST_F(RSEffectDrawableUtilsTest, SetPointLightData007, testing::ext::TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    node.GetMutableRenderProperties().SetBounds(Vector4f(0.0f, 0.0f, 100.0f, 100.0f));
    auto& props = node.GetMutableRenderProperties();
    props.SetIlluminatedBorderWidth(1.0f);
    IlluminatedType type = IlluminatedType::NORMAL_BORDER_CONTENT;
    props.GetEffectProperties().GetShaderEffect().illuminatedPtr_->SetIlluminatedType(type);
    RSLightSource lightSource;
    lightSource.SetLightIntensity(2.0f);
    lightSource.SetLightColor(Color(255, 0, 0, 255));
    Vector4f pos(5.0f, 5.0f, 0.0f, 0.0f);
    props.GetIlluminated()->AddLightSourcesAndPos(1002, lightSource, pos);
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::POINT_LIGHT);
    ASSERT_NE(shader, nullptr);
    DrawableV2::RSEffectDrawableUtils::SetPointLightData(shader, node);
    auto pls = std::static_pointer_cast<RSNGRenderPointLight>(shader);
    EXPECT_EQ(pls->Getter<PointLightLightPositionsRenderTag>()->Get().size(), 1u);
    EXPECT_TRUE(pls->Getter<PointLightEnableEDREffectRenderTag>()->Get());
}

/**
 * @tc.name: SetPointLightData008
 * @tc.desc: Test SetPointLightData with NORMAL_BORDER_CONTENT type but light intensity <= 1.0
 *           so enableEDREffect is false
 * @tc.type: FUNC
 */
HWTEST_F(RSEffectDrawableUtilsTest, SetPointLightData008, testing::ext::TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    node.GetMutableRenderProperties().SetBounds(Vector4f(0.0f, 0.0f, 100.0f, 100.0f));
    auto& props = node.GetMutableRenderProperties();
    props.SetIlluminatedBorderWidth(1.0f);
    IlluminatedType type = IlluminatedType::NORMAL_BORDER_CONTENT;
    props.GetEffectProperties().GetShaderEffect().illuminatedPtr_->SetIlluminatedType(type);
    RSLightSource lightSource;
    lightSource.SetLightIntensity(1.0f);
    lightSource.SetLightColor(Color(255, 0, 0, 255));
    Vector4f pos(5.0f, 5.0f, 0.0f, 0.0f);
    props.GetIlluminated()->AddLightSourcesAndPos(1003, lightSource, pos);
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::POINT_LIGHT);
    ASSERT_NE(shader, nullptr);
    DrawableV2::RSEffectDrawableUtils::SetPointLightData(shader, node);
    auto pls = std::static_pointer_cast<RSNGRenderPointLight>(shader);
    EXPECT_EQ(pls->Getter<PointLightLightIntensitiesRenderTag>()->Get().size(), 1u);
    EXPECT_FALSE(pls->Getter<PointLightEnableEDREffectRenderTag>()->Get());
}

/**
 * @tc.name: SetPointLightData009
 * @tc.desc: Test SetPointLightData with more than MAX_LIGHTS(12) valid light sources
 *           (SetPointLightPropToShader sort-and-truncate branch)
 * @tc.type: FUNC
 */
HWTEST_F(RSEffectDrawableUtilsTest, SetPointLightData009, testing::ext::TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    node.GetMutableRenderProperties().SetBounds(Vector4f(0.0f, 0.0f, 100.0f, 100.0f));
    auto& props = node.GetMutableRenderProperties();
    props.SetIlluminatedBorderWidth(1.0f);
    IlluminatedType type = IlluminatedType::BORDER;
    props.GetEffectProperties().GetShaderEffect().illuminatedPtr_->SetIlluminatedType(type);
    constexpr int lightCount = 13; // MAX_LIGHTS is 12, add one extra to trigger truncation
    for (int i = 0; i < lightCount; ++i) {
        RSLightSource lightSource;
        lightSource.SetLightIntensity(1.0f);
        Vector4f pos(static_cast<float>(i), static_cast<float>(i), 0.0f, 0.0f);
        props.GetIlluminated()->AddLightSourcesAndPos(static_cast<NodeId>(2000 + i), lightSource, pos);
    }
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::POINT_LIGHT);
    ASSERT_NE(shader, nullptr);
    DrawableV2::RSEffectDrawableUtils::SetPointLightData(shader, node);
    auto pls = std::static_pointer_cast<RSNGRenderPointLight>(shader);
    EXPECT_EQ(pls->Getter<PointLightLightPositionsRenderTag>()->Get().size(), 12u);
    EXPECT_EQ(pls->Getter<PointLightLightColorsRenderTag>()->Get().size(), 12u);
    EXPECT_EQ(pls->Getter<PointLightLightIntensitiesRenderTag>()->Get().size(), 12u);
}

/**
 * @tc.name: SetPointLightData010
 * @tc.desc: Test SetPointLightData with invalid light sources (intensity 0) in the lightMap
 *           (CollectLightSource skips invalid sources)
 * @tc.type: FUNC
 */
HWTEST_F(RSEffectDrawableUtilsTest, SetPointLightData010, testing::ext::TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    node.GetMutableRenderProperties().SetBounds(Vector4f(0.0f, 0.0f, 100.0f, 100.0f));
    auto& props = node.GetMutableRenderProperties();
    props.SetIlluminatedBorderWidth(1.0f);
    IlluminatedType type = IlluminatedType::BORDER;
    props.GetEffectProperties().GetShaderEffect().illuminatedPtr_->SetIlluminatedType(type);
    RSLightSource invalidSource;
    invalidSource.SetLightIntensity(0.0f);
    RSLightSource validSource;
    validSource.SetLightIntensity(3.0f);
    validSource.SetLightColor(Color(10, 20, 30, 255));
    Vector4f pos1(1.0f, 1.0f, 0.0f, 0.0f);
    Vector4f pos2(2.0f, 2.0f, 0.0f, 0.0f);
    props.GetIlluminated()->AddLightSourcesAndPos(3001, invalidSource, pos1);
    props.GetIlluminated()->AddLightSourcesAndPos(3002, validSource, pos2);
    EXPECT_EQ(node.GetRenderProperties().GetIlluminated()->GetLightSourcesAndPosMap().size(), 2u);
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::POINT_LIGHT);
    ASSERT_NE(shader, nullptr);
    DrawableV2::RSEffectDrawableUtils::SetPointLightData(shader, node);
    auto pls = std::static_pointer_cast<RSNGRenderPointLight>(shader);
    EXPECT_EQ(pls->Getter<PointLightLightPositionsRenderTag>()->Get().size(), 1u);
    EXPECT_EQ(pls->Getter<PointLightLightIntensitiesRenderTag>()->Get().size(), 1u);
    EXPECT_FLOAT_EQ(pls->Getter<PointLightLightIntensitiesRenderTag>()->Get()[0], 3.0f);
}

/**
 * @tc.name: SetPointLightData011
 * @tc.desc: Test SetPointLightData with a shader chain where POINT_LIGHT is not the head,
 *           FindShader walks the chain to find it
 * @tc.type: FUNC
 */
HWTEST_F(RSEffectDrawableUtilsTest, SetPointLightData011, testing::ext::TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    node.GetMutableRenderProperties().SetBounds(Vector4f(0.0f, 0.0f, 100.0f, 100.0f));
    auto& props = node.GetMutableRenderProperties();
    props.SetIlluminatedBorderWidth(1.0f);
    IlluminatedType type = IlluminatedType::BORDER;
    props.GetEffectProperties().GetShaderEffect().illuminatedPtr_->SetIlluminatedType(type);
    RSLightSource lightSource;
    lightSource.SetLightIntensity(2.0f);
    Vector4f pos(8.0f, 8.0f, 0.0f, 0.0f);
    props.GetIlluminated()->AddLightSourcesAndPos(4001, lightSource, pos);
    auto head = RSNGRenderShaderBase::Create(RSNGEffectType::AURORA_NOISE);
    auto pointLight = RSNGRenderShaderBase::Create(RSNGEffectType::POINT_LIGHT);
    ASSERT_NE(head, nullptr);
    ASSERT_NE(pointLight, nullptr);
    head->nextEffect_ = pointLight;
    DrawableV2::RSEffectDrawableUtils::SetPointLightData(head, node);
    auto pls = std::static_pointer_cast<RSNGRenderPointLight>(pointLight);
    EXPECT_EQ(pls->Getter<PointLightLightPositionsRenderTag>()->Get().size(), 1u);
    EXPECT_EQ(pls->Getter<PointLightLightIntensitiesRenderTag>()->Get().size(), 1u);
}

} // namespace OHOS::Rosen

