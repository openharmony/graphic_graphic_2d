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
#include "drawable/rs_property_drawable_foreground.h"
#include "drawable/rs_render_node_drawable_adapter.h"
#include "effect/rs_render_filter_base.h"
#include "effect/rs_render_mask_base.h"
#include "effect/rs_render_shader_base.h"
#include "effect/rs_render_shape_base.h"
#include "effect/rs_render_shader_base.h"
#include "ge_visual_effect_container.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_depth_render_node.h"
#include "pipeline/rs_render_node.h"
#include "property/rs_spatial_effect_def.h"
#include "property/rs_spatial_effect_manager.h"
#include "recording/recording_canvas.h"
#include "render/rs_drawing_filter.h"
#include "render/rs_foreground_effect_filter.h"
#include "common/rs_obj_abs_geometry.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSPropertyDrawableForegroundTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSPropertyDrawableForegroundTest::SetUpTestCase() {}
void RSPropertyDrawableForegroundTest::TearDownTestCase() {}
void RSPropertyDrawableForegroundTest::SetUp() {}
void RSPropertyDrawableForegroundTest::TearDown() {}

/**
 * @tc.name: OnGenerateAndOnUpdateTest001
 * @tc.desc: OnGenerate and OnUpdate test
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawableForegroundTest, OnGenerateAndOnUpdateTest001, TestSize.Level1)
{
    RSRenderNode renderNodeTest1(0);
    Vector4f aiInvertTest(0.0f, 0.0f, 0.0f, 1.0f);
    std::shared_ptr<DrawableV2::RSBinarizationDrawable> binarizationDrawable =
        std::make_shared<DrawableV2::RSBinarizationDrawable>();
    EXPECT_NE(binarizationDrawable, nullptr);
    renderNodeTest1.renderProperties_.GetEffect().aiInvert_ = std::nullopt;
    EXPECT_EQ(binarizationDrawable->OnGenerate(renderNodeTest1), nullptr);
    renderNodeTest1.renderProperties_.GetEffect().aiInvert_ = aiInvertTest;
    EXPECT_NE(binarizationDrawable->OnGenerate(renderNodeTest1), nullptr);

    RSRenderNode renderNodeTest2(0);
    std::shared_ptr<DrawableV2::RSColorFilterDrawable> colorFilterDrawable =
        std::make_shared<DrawableV2::RSColorFilterDrawable>();
    EXPECT_NE(colorFilterDrawable, nullptr);
    renderNodeTest2.renderProperties_.GetEffect().colorFilter_ = nullptr;
    EXPECT_EQ(colorFilterDrawable->OnGenerate(renderNodeTest2), nullptr);
    std::shared_ptr<Drawing::ColorFilter> colorFilter = std::make_shared<Drawing::ColorFilter>();
    EXPECT_NE(colorFilter, nullptr);
    renderNodeTest2.renderProperties_.GetEffect().colorFilter_ = colorFilter;
    EXPECT_NE(colorFilterDrawable->OnGenerate(renderNodeTest2), nullptr);

    RSRenderNode renderNodeTest3(0);
    std::shared_ptr<DrawableV2::RSLightUpEffectDrawable> lightUpEffectDrawable =
        std::make_shared<DrawableV2::RSLightUpEffectDrawable>();
    EXPECT_NE(lightUpEffectDrawable, nullptr);
    renderNodeTest3.renderProperties_.GetEffect().lightUpEffectDegree_ = -1.0f;
    EXPECT_EQ(lightUpEffectDrawable->OnGenerate(renderNodeTest3), nullptr);
    renderNodeTest3.renderProperties_.GetEffect().lightUpEffectDegree_ = 0.1f;
    EXPECT_NE(lightUpEffectDrawable->OnGenerate(renderNodeTest3), nullptr);
}

/**
 * @tc.name: OnGenerateAndOnUpdateTest002
 * @tc.desc: OnGenerate and OnUpdate test
 * @tc.type: FUNC
 * @tc.require:issueIA5Y41
 */
HWTEST_F(RSPropertyDrawableForegroundTest, OnGenerateAndOnUpdateTest002, TestSize.Level1)
{
    RSRenderNode renderNodeTest4(0);
    std::shared_ptr<DrawableV2::RSDynamicDimDrawable> dynamicDimDrawable =
        std::make_shared<DrawableV2::RSDynamicDimDrawable>();
    EXPECT_NE(dynamicDimDrawable, nullptr);
    float dynamicDimDegreeTest1 = -1.0f;
    float dynamicDimDegreeTest2 = 0.1f;
    renderNodeTest4.renderProperties_.GetEffect().dynamicDimDegree_ = dynamicDimDegreeTest1;
    EXPECT_EQ(dynamicDimDrawable->OnGenerate(renderNodeTest4), nullptr);
    renderNodeTest4.renderProperties_.GetEffect().dynamicDimDegree_ = dynamicDimDegreeTest2;
    EXPECT_NE(dynamicDimDrawable->OnGenerate(renderNodeTest4), nullptr);

    RSRenderNode renderNodeTest5(0);
    std::shared_ptr<DrawableV2::RSForegroundColorDrawable> foregroundColorDrawable =
        std::make_shared<DrawableV2::RSForegroundColorDrawable>();
    EXPECT_NE(foregroundColorDrawable, nullptr);
    renderNodeTest5.renderProperties_.decoration_ = nullptr;
    EXPECT_EQ(foregroundColorDrawable->OnGenerate(renderNodeTest5), nullptr);
    auto decorationTest = std::make_unique<Decoration>();
    RSColor val(0.0f, 0.0f, 1.0f, 1.0f);
    decorationTest->foregroundColor_ = val;
    renderNodeTest5.renderProperties_.decoration_ = std::move(decorationTest);
    EXPECT_NE(foregroundColorDrawable->OnGenerate(renderNodeTest5), nullptr);

    RSRenderNode renderNodeTest6(0);
    std::shared_ptr<DrawableV2::RSCompositingFilterDrawable> compositingFilterDrawable =
        std::make_shared<DrawableV2::RSCompositingFilterDrawable>();
    EXPECT_NE(compositingFilterDrawable, nullptr);
    renderNodeTest6.renderProperties_.filter_ = nullptr;
    EXPECT_EQ(compositingFilterDrawable->OnGenerate(renderNodeTest6), nullptr);

    RSRenderNode renderNodeTest7(0);
    std::shared_ptr<DrawableV2::RSForegroundFilterDrawable> foregroundFilterDrawable =
        std::make_shared<DrawableV2::RSForegroundFilterDrawable>();
    EXPECT_NE(foregroundFilterDrawable, nullptr);
    renderNodeTest7.renderProperties_.foregroundFilter_ = nullptr;
    EXPECT_EQ(foregroundFilterDrawable->OnGenerate(renderNodeTest7), nullptr);
    EXPECT_FALSE(foregroundFilterDrawable->OnUpdate(renderNodeTest7));
    std::shared_ptr<RSFilter> foregroundFilterTest1 = std::make_shared<RSFilter>();
    EXPECT_NE(foregroundFilterTest1, nullptr);
    renderNodeTest7.renderProperties_.foregroundFilter_ = foregroundFilterTest1;
    EXPECT_NE(foregroundFilterDrawable->OnGenerate(renderNodeTest7), nullptr);
    EXPECT_TRUE(foregroundFilterDrawable->OnUpdate(renderNodeTest7));
}

/**
 * @tc.name: OnGenerateAndOnUpdateTest003
 * @tc.desc: OnGenerate and OnUpdate test
 * @tc.type: FUNC
 * @tc.require:issueIA5Y41
 */
HWTEST_F(RSPropertyDrawableForegroundTest, OnGenerateAndOnUpdateTest003, TestSize.Level1)
{
    RSRenderNode renderNodeTest8(1);
    std::shared_ptr<DrawableV2::RSForegroundFilterRestoreDrawable> foregroundFilterRestoreDrawable =
        std::make_shared<DrawableV2::RSForegroundFilterRestoreDrawable>();
    EXPECT_NE(foregroundFilterRestoreDrawable, nullptr);
    renderNodeTest8.renderProperties_.foregroundFilter_ = nullptr;
    EXPECT_FALSE(foregroundFilterRestoreDrawable->OnUpdate(renderNodeTest8));
    EXPECT_EQ(foregroundFilterRestoreDrawable->stagingNodeId_, renderNodeTest8.GetId());
    EXPECT_EQ(foregroundFilterRestoreDrawable->OnGenerate(renderNodeTest8), nullptr);
    std::shared_ptr<RSFilter> foregroundFilterTest2 = std::make_shared<RSFilter>();
    EXPECT_NE(foregroundFilterTest2, nullptr);
    renderNodeTest8.renderProperties_.foregroundFilter_ = foregroundFilterTest2;
    EXPECT_NE(foregroundFilterRestoreDrawable->OnGenerate(renderNodeTest8), nullptr);
    EXPECT_TRUE(foregroundFilterRestoreDrawable->OnUpdate(renderNodeTest8));

    RSRenderNode renderNodeTest9(0);
    std::shared_ptr<DrawableV2::RSPixelStretchDrawable> pixelStretchDrawable =
        std::make_shared<DrawableV2::RSPixelStretchDrawable>();
    EXPECT_NE(pixelStretchDrawable, nullptr);
    renderNodeTest9.renderProperties_.SetPixelStretch(std::nullopt);
    EXPECT_EQ(pixelStretchDrawable->OnGenerate(renderNodeTest9), nullptr);
    Vector4f pixelStretchTest(0.0f, 0.0f, 0.0f, 1.0f);
    renderNodeTest9.renderProperties_.SetPixelStretch(pixelStretchTest);
    EXPECT_NE(pixelStretchDrawable->OnGenerate(renderNodeTest9), nullptr);
}

/**
 * @tc.name: OnGenerateAndOnUpdateTest004
 * @tc.desc: OnGenerate and OnUpdate test
 * @tc.type: FUNC
 * @tc.require:issueIA5Y41
 */
HWTEST_F(RSPropertyDrawableForegroundTest, OnGenerateAndOnUpdateTest004, TestSize.Level1)
{
    RSRenderNode renderNodeTest10(0);
    std::shared_ptr<DrawableV2::RSBorderDrawable> borderDrawable = std::make_shared<DrawableV2::RSBorderDrawable>();
    EXPECT_NE(borderDrawable, nullptr);
    renderNodeTest10.renderProperties_.border_ = nullptr;
    EXPECT_EQ(borderDrawable->OnGenerate(renderNodeTest10), nullptr);
    std::shared_ptr<RSBorder> border = std::make_shared<RSBorder>();
    EXPECT_NE(border, nullptr);
    renderNodeTest10.renderProperties_.border_ = border;
    EXPECT_EQ(borderDrawable->OnGenerate(renderNodeTest10), nullptr);
    border->colors_.emplace_back(RSColor(1.0f, 1.0f, 1.0f, 1.0f));
    border->widths_.emplace_back(1.0f);
    border->styles_.emplace_back(BorderStyle::SOLID);
    EXPECT_NE(borderDrawable->OnGenerate(renderNodeTest10), nullptr);

    RSRenderNode renderNodeTest11(0);
    std::shared_ptr<DrawableV2::RSOutlineDrawable> outlineDrawable = std::make_shared<DrawableV2::RSOutlineDrawable>();
    EXPECT_NE(outlineDrawable, nullptr);
    renderNodeTest11.renderProperties_.outline_ = nullptr;
    EXPECT_EQ(outlineDrawable->OnGenerate(renderNodeTest11), nullptr);
    std::shared_ptr<RSBorder> outline = std::make_shared<RSBorder>();
    EXPECT_NE(outline, nullptr);
    outline->colors_.emplace_back(RSColor(1.0f, 1.0f, 1.0f, 1.0f));
    outline->widths_.emplace_back(1.0f);
    outline->styles_.emplace_back(BorderStyle::SOLID);
    renderNodeTest11.renderProperties_.outline_ = outline;
    EXPECT_NE(outlineDrawable->OnGenerate(renderNodeTest11), nullptr);
}

/**
 * @tc.name: OnGenerateAndOnUpdateTest005
 * @tc.desc: OnGenerate and OnUpdate test
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawableForegroundTest, OnGenerateAndOnUpdateTest005, TestSize.Level1)
{
    // Only wrote OnUpdate function with a return value of false
    RSRenderNode renderNodeTest13(0);
    std::shared_ptr<DrawableV2::RSParticleDrawable> particleDrawable =
        std::make_shared<DrawableV2::RSParticleDrawable>();
    EXPECT_NE(particleDrawable, nullptr);
    renderNodeTest13.renderProperties_.GetEffect().particles_.renderParticleVector_.clear();
    EXPECT_EQ(particleDrawable->OnGenerate(renderNodeTest13), nullptr);

    std::shared_ptr<ParticleRenderParams> particleParams = std::make_shared<ParticleRenderParams>();
    EXPECT_NE(particleParams, nullptr);
    std::vector<std::shared_ptr<RSRenderParticle>> renderParticleVector { std::make_shared<RSRenderParticle>(
        particleParams) };
    renderParticleVector.at(0)->lifeTime_ = 1;
    RSRenderParticleVector particles;
    particles.renderParticleVector_ = renderParticleVector;
    renderNodeTest13.GetMutableRenderProperties().SetParticles(particles);
    ASSERT_TRUE(renderNodeTest13.GetRenderProperties().GetParticles().GetParticleSize());
    std::shared_ptr<RectF> rect = std::make_shared<RectF>(1, 1, 1, 1);
    EXPECT_NE(rect, nullptr);
    renderNodeTest13.GetMutableRenderProperties().SetDrawRegion(rect);
    EXPECT_NE(particleDrawable->OnGenerate(renderNodeTest13), nullptr);
}

/**
 * @tc.name: OnSyncTest001
 * @tc.desc: OnSync test
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawableForegroundTest, OnSyncTest001, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSBinarizationDrawable> binarizationDrawable =
        std::make_shared<DrawableV2::RSBinarizationDrawable>();
    EXPECT_NE(binarizationDrawable, nullptr);
    binarizationDrawable->needSync_ = true;
    std::optional<Vector4f> stagingAiInvert = 1.0f;
    binarizationDrawable->OnSync();
    EXPECT_FALSE(binarizationDrawable->needSync_);
    binarizationDrawable->OnSync();

    std::shared_ptr<DrawableV2::RSColorFilterDrawable> colorFilterDrawable =
        std::make_shared<DrawableV2::RSColorFilterDrawable>();
    EXPECT_NE(colorFilterDrawable, nullptr);
    colorFilterDrawable->needSync_ = true;
    std::shared_ptr<Drawing::ColorFilter> stagingFilter = std::make_shared<Drawing::ColorFilter>();
    EXPECT_NE(stagingFilter, nullptr);
    colorFilterDrawable->stagingFilter_ = stagingFilter;
    colorFilterDrawable->OnSync();
    EXPECT_FALSE(colorFilterDrawable->needSync_);
    colorFilterDrawable->OnSync();

    std::shared_ptr<DrawableV2::RSLightUpEffectDrawable> lightUpEffectDrawable =
        std::make_shared<DrawableV2::RSLightUpEffectDrawable>();
    EXPECT_NE(lightUpEffectDrawable, nullptr);
    lightUpEffectDrawable->needSync_ = true;
    lightUpEffectDrawable->OnSync();
    EXPECT_FALSE(lightUpEffectDrawable->needSync_);
    lightUpEffectDrawable->OnSync();
}

/**
 * @tc.name: OnSyncTest002
 * @tc.desc: OnSync test
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawableForegroundTest, OnSyncTest002, TestSize.Level1)
{
    const NodeId nodeID = 10;

    std::shared_ptr<DrawableV2::RSDynamicDimDrawable> dynamicDimDrawable =
        std::make_shared<DrawableV2::RSDynamicDimDrawable>();
    EXPECT_NE(dynamicDimDrawable, nullptr);
    dynamicDimDrawable->needSync_ = true;
    dynamicDimDrawable->OnSync();
    EXPECT_FALSE(dynamicDimDrawable->needSync_);
    dynamicDimDrawable->OnSync();
    std::shared_ptr<DrawableV2::RSForegroundFilterDrawable> foregroundFilterDrawable =
        std::make_shared<DrawableV2::RSForegroundFilterDrawable>();
    EXPECT_NE(foregroundFilterDrawable, nullptr);
    RectF stagingBoundsRect = RectF(0.0f, 0.0f, 1.0f, 1.0f);
    foregroundFilterDrawable->stagingBoundsRect_ = stagingBoundsRect;
    foregroundFilterDrawable->needSync_ = true;
    foregroundFilterDrawable->OnSync();
    EXPECT_FALSE(foregroundFilterDrawable->needSync_);
    foregroundFilterDrawable->OnSync();

    std::shared_ptr<DrawableV2::RSForegroundFilterRestoreDrawable> foregroundFilterRestoreDrawable =
        std::make_shared<DrawableV2::RSForegroundFilterRestoreDrawable>();
    EXPECT_NE(foregroundFilterRestoreDrawable, nullptr);
    foregroundFilterRestoreDrawable->needSync_ = true;
    foregroundFilterRestoreDrawable->OnSync();
    std::shared_ptr<RSFilter> stagingForegroundFilter = std::make_shared<RSFilter>();
    EXPECT_NE(stagingForegroundFilter, nullptr);
    foregroundFilterRestoreDrawable->stagingNodeId_ = nodeID;
    foregroundFilterRestoreDrawable->stagingForegroundFilter_ = stagingForegroundFilter;
    foregroundFilterRestoreDrawable->needSync_ = true;
    foregroundFilterRestoreDrawable->OnSync();
    EXPECT_FALSE(foregroundFilterRestoreDrawable->needSync_);
    EXPECT_EQ(foregroundFilterRestoreDrawable->renderNodeId_, nodeID);
    foregroundFilterRestoreDrawable->OnSync();

    std::shared_ptr<DrawableV2::RSPixelStretchDrawable> pixelStretchDrawable =
        std::make_shared<DrawableV2::RSPixelStretchDrawable>();
    EXPECT_NE(pixelStretchDrawable, nullptr);
    pixelStretchDrawable->stagingPixelStretch_ = 1.0f;
    pixelStretchDrawable->needSync_ = true;
    pixelStretchDrawable->OnSync();
    EXPECT_FALSE(pixelStretchDrawable->needSync_);
    pixelStretchDrawable->OnSync();
}
/**
 * @tc.name: OnDrawAndRunTest001
 * @tc.desc: OnDrawAndRun test
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawableForegroundTest, OnDrawAndRunTest001, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSForegroundFilterRestoreDrawable> foregroundFilterRestoreDrawable =
        std::make_shared<DrawableV2::RSForegroundFilterRestoreDrawable>();
    EXPECT_NE(foregroundFilterRestoreDrawable, nullptr);

    // initialize drawing filter
    auto imageFilter = std::make_shared<Drawing::ImageFilter>();
    auto filterPtr = std::make_shared<RSRenderFilterParaBase>();
    std::vector<std::shared_ptr<RSRenderFilterParaBase>> shaderFilters;
    shaderFilters.push_back(filterPtr);
    uint32_t hash = 1;
    std::shared_ptr<RSDrawingFilter> drawingFilter =
        std::make_shared<RSDrawingFilter>(imageFilter, shaderFilters, hash);
    drawingFilter->SetImageFilter(imageFilter);

    // create and sync a foreground filter
    std::shared_ptr<RSFilter> stagingForegroundFilter = drawingFilter;
    EXPECT_NE(stagingForegroundFilter, nullptr);
    foregroundFilterRestoreDrawable->stagingForegroundFilter_ = stagingForegroundFilter;
    foregroundFilterRestoreDrawable->needSync_ = true;
    foregroundFilterRestoreDrawable->OnSync();

    // initial state
    for (auto filter : drawingFilter->visualEffectContainer_->GetFilters()) {
        EXPECT_EQ(filter->GetCanvasInfo().geoWidth, 0.0f);
        EXPECT_EQ(filter->GetCanvasInfo().geoHeight, 0.0f);
    }

    Drawing::Canvas canvasTest;
    RSPaintFilterCanvas paintFilterCanvas(&canvasTest);
    foregroundFilterRestoreDrawable->OnDraw(&paintFilterCanvas, nullptr);
    // rect == nullptr, still initial width/height
    for (auto filter : drawingFilter->visualEffectContainer_->GetFilters()) {
        EXPECT_EQ(filter->GetCanvasInfo().geoWidth, 0.0f);
        EXPECT_EQ(filter->GetCanvasInfo().geoHeight, 0.0f);
    }

    const auto width = 100.0f;
    const auto height = 100.0f;
    Drawing::Rect rect(0.0f, 0.0f, width, height);
    foregroundFilterRestoreDrawable->OnDraw(&paintFilterCanvas, &rect);
    // properly initialized
    for (auto filter : drawingFilter->visualEffectContainer_->GetFilters()) {
        EXPECT_EQ(filter->GetCanvasInfo().geoWidth, width);
        EXPECT_EQ(filter->GetCanvasInfo().geoHeight, height);
    }

    // in case foregroundFilter_ happens to be nullptr
    foregroundFilterRestoreDrawable->foregroundFilter_ = nullptr;
    foregroundFilterRestoreDrawable->OnDraw(&paintFilterCanvas, &rect); // should not crash or anything

    // RSFilter is used instead of RSDrawingFilter
    stagingForegroundFilter = std::make_shared<RSFilter>();
    foregroundFilterRestoreDrawable->stagingForegroundFilter_ = stagingForegroundFilter;
    foregroundFilterRestoreDrawable->needSync_ = true;
    foregroundFilterRestoreDrawable->OnSync();
    foregroundFilterRestoreDrawable->OnDraw(&paintFilterCanvas, &rect);
}

/**
 * @tc.name: DrawBorderTest001
 * @tc.desc: DrawBorder test
 * @tc.type: FUNC
 * @tc.require:issueIA5Y41
 */
HWTEST_F(RSPropertyDrawableForegroundTest, DrawBorderTest001, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSBorderDrawable> borderDrawable = std::make_shared<DrawableV2::RSBorderDrawable>();
    EXPECT_NE(borderDrawable, nullptr);
    RSProperties properties;
    Drawing::Canvas canvas;
    std::shared_ptr<RSBorder> border = std::make_shared<RSBorder>();
    border->colors_.emplace_back(Color());
    border->styles_.emplace_back(BorderStyle::SOLID);
    EXPECT_EQ(border->colors_.size(), 1);
    EXPECT_EQ(border->styles_.size(), 1);
    borderDrawable->DrawBorder(properties, canvas, border, false);

    border->styles_.at(0) = BorderStyle::DASHED;
    Vector4<float> radiusTest1 = { 0.0f, 0.0f, 0.0f, 0.0f };
    border->radius_ = radiusTest1;
    borderDrawable->DrawBorder(properties, canvas, border, true);

    Vector4<float> radiusTest2 = { 10.0f, 0.0f, 0.0f, 0.0f };
    border->radius_ = radiusTest2;
    border->widths_.emplace_back(1.0f);
    border->dashWidth_.emplace_back(1.0f);
    border->dashGap_.emplace_back(1.0f);
    borderDrawable->DrawBorder(properties, canvas, border, true);

    border->dashGap_.clear();
    borderDrawable->DrawBorder(properties, canvas, border, true);
}

/**
 * @tc.name: DrawBorderTest002
 * @tc.desc: DrawBorder test with sdf effect filter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyDrawableForegroundTest, DrawBorderTest002, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);

    std::shared_ptr<DrawableV2::RSBorderDrawable> borderDrawable = std::make_shared<DrawableV2::RSBorderDrawable>();

    auto sdfShape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_UNION_OP_SHAPE);
    EXPECT_NE(sdfShape, nullptr);
    node.GetMutableRenderProperties().SetSDFShape(sdfShape);

    std::shared_ptr<RSBorder> border = std::make_shared<RSBorder>();
    Color borderColor = Color();
    Drawing::Color drawingColor(
        borderColor.GetRed(), borderColor.GetGreen(), borderColor.GetBlue(), borderColor.GetAlpha());
    float borderWidth = 2.f;
    border->colors_.emplace_back(borderColor);
    border->widths_.emplace_back(borderWidth);

    Drawing::Canvas canvas;
    borderDrawable->DrawBorder(node.GetRenderProperties(), canvas, border, false);

    border->styles_.emplace_back(BorderStyle::SOLID);
    borderDrawable->DrawBorder(node.GetRenderProperties(), canvas, border, false);
    border->SetRadiusFour({ 0.f, 0.f, 0.f, 0.f });
    borderDrawable->DrawBorder(node.GetRenderProperties(), canvas, border, true);
    border->SetRadiusFour({ 1.f, 1.f, 1.f, 1.f });
    borderDrawable->DrawBorder(node.GetRenderProperties(), canvas, border, true);
    EXPECT_EQ(border->widths_.size(), 1);
}

/**
 * @tc.name: RSParticleDrawableOnUpdateCacheTest001
 * @tc.desc: Test RSParticleDrawable::OnUpdate caching - first call creates cachedDrawable_
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableForegroundTest, RSParticleDrawableOnUpdateCacheTest001, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSParticleDrawable> particleDrawable =
        std::make_shared<DrawableV2::RSParticleDrawable>();
    EXPECT_NE(particleDrawable, nullptr);
    EXPECT_EQ(particleDrawable->cachedDrawable_, nullptr);

    RSRenderNode renderNode(0);
    std::shared_ptr<ParticleRenderParams> particleParams = std::make_shared<ParticleRenderParams>();
    std::vector<std::shared_ptr<RSRenderParticle>> renderParticleVector;
    renderParticleVector.push_back(std::make_shared<RSRenderParticle>(particleParams));
    renderParticleVector.at(0)->lifeTime_ = 1;

    RSRenderParticleVector particles;
    particles.renderParticleVector_ = renderParticleVector;
    renderNode.GetMutableRenderProperties().SetParticles(particles);

    std::shared_ptr<RectF> rect = std::make_shared<RectF>(0, 0, 100, 100);
    renderNode.GetMutableRenderProperties().SetDrawRegion(rect);

    bool result = particleDrawable->OnUpdate(renderNode);
    EXPECT_TRUE(result);
    EXPECT_NE(particleDrawable->cachedDrawable_, nullptr);
}

/**
 * @tc.name: RSParticleDrawableOnUpdateCacheTest002
 * @tc.desc: Test RSParticleDrawable::OnUpdate caching - second call reuses cachedDrawable_
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableForegroundTest, RSParticleDrawableOnUpdateCacheTest002, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSParticleDrawable> particleDrawable =
        std::make_shared<DrawableV2::RSParticleDrawable>();
    EXPECT_NE(particleDrawable, nullptr);

    RSRenderNode renderNode(0);
    std::shared_ptr<ParticleRenderParams> particleParams = std::make_shared<ParticleRenderParams>();
    std::vector<std::shared_ptr<RSRenderParticle>> renderParticleVector;
    renderParticleVector.push_back(std::make_shared<RSRenderParticle>(particleParams));
    renderParticleVector.at(0)->lifeTime_ = 1;

    RSRenderParticleVector particles;
    particles.renderParticleVector_ = renderParticleVector;
    renderNode.GetMutableRenderProperties().SetParticles(particles);

    std::shared_ptr<RectF> rect = std::make_shared<RectF>(0, 0, 100, 100);
    renderNode.GetMutableRenderProperties().SetDrawRegion(rect);

    // First call
    particleDrawable->OnUpdate(renderNode);
    auto firstCachedDrawable = particleDrawable->cachedDrawable_;
    EXPECT_NE(firstCachedDrawable, nullptr);

    // Second call - should reuse cached drawable (UpdateData branch)
    renderParticleVector.push_back(std::make_shared<RSRenderParticle>(particleParams));
    renderParticleVector.at(1)->lifeTime_ = 1;
    particles.renderParticleVector_ = renderParticleVector;
    renderNode.GetMutableRenderProperties().SetParticles(particles);

    bool result = particleDrawable->OnUpdate(renderNode);
    EXPECT_TRUE(result);
    EXPECT_EQ(particleDrawable->cachedDrawable_, firstCachedDrawable);
}

/**
 * @tc.name: RSParticleDrawableOnUpdateCacheTest003
 * @tc.desc: Test RSParticleDrawable::OnUpdate returns false when particle size is 0
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableForegroundTest, RSParticleDrawableOnUpdateCacheTest003, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSParticleDrawable> particleDrawable =
        std::make_shared<DrawableV2::RSParticleDrawable>();
    EXPECT_NE(particleDrawable, nullptr);

    RSRenderNode renderNode(0);
    // Empty particles
    RSRenderParticleVector particles;
    renderNode.GetMutableRenderProperties().SetParticles(particles);

    bool result = particleDrawable->OnUpdate(renderNode);
    EXPECT_FALSE(result);
}

class RSTestRenderNodeDrawableAdapter : public DrawableV2::RSRenderNodeDrawableAdapter {
public:
    explicit RSTestRenderNodeDrawableAdapter(std::shared_ptr<const RSRenderNode> node)
        : RSRenderNodeDrawableAdapter(std::move(node)), nodeId_(GetId())
    {
        renderParams_ = std::make_unique<RSDepthRenderParams>(nodeId_);
    }
    ~RSTestRenderNodeDrawableAdapter() override = default;

    void Draw(Drawing::Canvas& canvas) override
    {
        printf("Draw:GetRecordingState: %d \n", canvas.GetRecordingState());
    }

private:
    NodeId nodeId_;
};

/**
 * @tc.name: RSSpatialEffectDrawableOnGenerate
 * @tc.desc: Test RSSpatialEffectDrawable OnGenerate at fail and succeesful case
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableForegroundTest, RSSpatialEffectDrawableOnGenerate, TestSize.Level1)
{
    // fail case
    RSSpatialEffectManager::Instance()->spatialEffectDepthNodeMap_.clear();
    auto renderNodePtr = std::make_shared<RSRenderNode>(0);
    RSRenderNode& renderNode = *renderNodePtr;
    EXPECT_EQ(DrawableV2::RSSpatialEffectDrawable::OnGenerate(renderNode), nullptr);

    // succeesful case
    auto depthNode = std::make_shared<RSDepthRenderNode>(1);
    auto depthNodeDrawable = std::make_shared<RSTestRenderNodeDrawableAdapter>(depthNode);
    depthNode->renderDrawable_ = depthNodeDrawable;
    RSSpatialEffectManager::Instance()->RegisterDepthNodeAndSpatialEffect(depthNode, renderNodePtr);

    auto& props = renderNode.GetMutableRenderProperties();
    SpatialEffectPara spatialEffectPara;
    spatialEffectPara.leftTop = Vector3f(0.0f, 0.0f, 1.0f);
    spatialEffectPara.rightTop = Vector3f(1.0f, 0.0f, 1.0f);
    spatialEffectPara.leftBottom = Vector3f(0.0f, 1.0f, 1.0f);
    spatialEffectPara.rightBottom = Vector3f(1.0f, 1.0f, 1.0f);
    props.SetSpatialEffectPara(spatialEffectPara);
    EXPECT_NE(DrawableV2::RSSpatialEffectDrawable::OnGenerate(renderNode), nullptr);
}

/**
 * @tc.name: RSSpatialEffectDrawableOnUpdate
 * @tc.desc: Test RSSpatialEffectDrawable OnUpdate on different cases
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableForegroundTest, RSSpatialEffectDrawableOnUpdate, TestSize.Level1)
{
    // fail case with no depth node
    RSSpatialEffectManager::Instance()->spatialEffectDepthNodeMap_.clear();
    auto renderNodePtr = std::make_shared<RSRenderNode>(0);
    RSRenderNode& renderNode = *renderNodePtr;
    std::shared_ptr<DrawableV2::RSSpatialEffectDrawable> spatialEffectDrawable =
        std::make_shared<DrawableV2::RSSpatialEffectDrawable>();
    EXPECT_NE(spatialEffectDrawable, nullptr);
    EXPECT_FALSE(spatialEffectDrawable->OnUpdate(renderNode));

    // fail case with node not depth node
    auto fakeNode = std::make_shared<RSRenderNode>(1);
    RSSpatialEffectManager::Instance()->RegisterDepthNodeAndSpatialEffect(fakeNode, renderNodePtr);
    EXPECT_FALSE(spatialEffectDrawable->OnUpdate(renderNode));

    // fail case with depth node has no drawable
    auto depthNode = std::make_shared<RSDepthRenderNode>(2);
    RSSpatialEffectManager::Instance()->spatialEffectDepthNodeMap_.clear();
    RSSpatialEffectManager::Instance()->RegisterDepthNodeAndSpatialEffect(depthNode, renderNodePtr);
    EXPECT_FALSE(spatialEffectDrawable->OnUpdate(renderNode));

    // fail case with no spatial effect para
    auto depthNodeDrawable = std::make_shared<RSTestRenderNodeDrawableAdapter>(depthNode);
    depthNode->renderDrawable_ = depthNodeDrawable;
    EXPECT_TRUE(spatialEffectDrawable->OnUpdate(renderNode));

    // succeesful case
    auto& props = renderNode.GetMutableRenderProperties();
    SpatialEffectPara spatialEffectPara;
    spatialEffectPara.leftTop = Vector3f(0.0f, 0.0f, 1.0f);
    spatialEffectPara.rightTop = Vector3f(1.0f, 0.0f, 1.0f);
    spatialEffectPara.leftBottom = Vector3f(0.0f, 1.0f, 1.0f);
    spatialEffectPara.rightBottom = Vector3f(1.0f, 1.0f, 1.0f);
    props.SetSpatialEffectPara(spatialEffectPara);
    EXPECT_TRUE(spatialEffectDrawable->OnUpdate(renderNode));
    EXPECT_TRUE(spatialEffectDrawable->needSync_);
    RSSpatialEffectManager::Instance()->spatialEffectDepthNodeMap_.clear();
}

/**
 * @tc.name: RSSpatialEffectDrawableOnUpdate002
 * @tc.desc: Test GLOBAL depthNode but no master global depthNode
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableForegroundTest, RSSpatialEffectDrawableOnUpdate002, TestSize.Level1)
{
    RSSpatialEffectManager::Instance()->spatialEffectDepthNodeMap_.clear();
    auto depthNode = std::make_shared<RSDepthRenderNode>(102);
    depthNode->SetDepthSpaceType(DepthSpaceType::GLOBAL);

    auto renderNode = std::make_shared<RSRenderNode>(3);
    renderNode->SetParent(depthNode);

    std::shared_ptr<DrawableV2::RSSpatialEffectDrawable> spatialEffectDrawable =
        std::make_shared<DrawableV2::RSSpatialEffectDrawable>();
    ASSERT_NE(spatialEffectDrawable, nullptr);

    RSSpatialEffectManager::Instance()->RegisterDepthNodeAndSpatialEffect(depthNode, renderNode);
    EXPECT_FALSE(spatialEffectDrawable->OnUpdate(*renderNode));
    RSSpatialEffectManager::Instance()->spatialEffectDepthNodeMap_.clear();
}

/**
 * @tc.name: RSSpatialEffectDrawableOnUpdate003
 * @tc.desc: Test GLOBAL depthNode and master global depthNode exists
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableForegroundTest, RSSpatialEffectDrawableOnUpdate003, TestSize.Level1)
{
    RSSpatialEffectManager::Instance()->spatialEffectDepthNodeMap_.clear();

    auto globalDepthNode = std::make_shared<RSDepthRenderNode>(200);
    globalDepthNode->SetDepthSpaceType(DepthSpaceType::GLOBAL);

    auto depthNode = std::make_shared<RSDepthRenderNode>(101);
    depthNode->SetDepthSpaceType(DepthSpaceType::GLOBAL);
    RSSpatialEffectManager::Instance()->masterGlobalDepthNodeMap_.emplace(depthNode->GetLogicalDisplayNodeId(),
        globalDepthNode->weak_from_this());

    auto renderNode = std::make_shared<RSRenderNode>(2);
    renderNode->SetParent(depthNode);

    std::shared_ptr<DrawableV2::RSSpatialEffectDrawable> spatialEffectDrawable =
        std::make_shared<DrawableV2::RSSpatialEffectDrawable>();
    ASSERT_NE(spatialEffectDrawable, nullptr);

    RSSpatialEffectManager::Instance()->RegisterDepthSpace(depthNode);
    RSSpatialEffectManager::Instance()->RegisterDepthNodeAndSpatialEffect(globalDepthNode, renderNode);
    auto drawable = std::make_shared<RSTestRenderNodeDrawableAdapter>(globalDepthNode);
    globalDepthNode->renderDrawable_ = drawable;
    EXPECT_TRUE(spatialEffectDrawable->OnUpdate(*renderNode));
    RSSpatialEffectManager::Instance()->spatialEffectDepthNodeMap_.clear();
}

/**
 * @tc.name: RSSpatialEffectDrawableOnSync
 * @tc.desc: Test RSSpatialEffectDrawable OnSync method
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableForegroundTest, RSSpatialEffectDrawableOnSync, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSSpatialEffectDrawable> spatialEffectDrawable =
        std::make_shared<DrawableV2::RSSpatialEffectDrawable>();
    EXPECT_NE(spatialEffectDrawable, nullptr);

    spatialEffectDrawable->stagingSpatialEffectPara_ = SpatialEffectPara();
    spatialEffectDrawable->OnSync();
    EXPECT_FALSE(spatialEffectDrawable->spatialEffectPara_.has_value());

    spatialEffectDrawable->needSync_ = true;
    spatialEffectDrawable->OnSync();
    EXPECT_TRUE(spatialEffectDrawable->spatialEffectPara_.has_value());
}

/**
 * @tc.name: RSSpatialEffectDrawableDrawSucc
 * @tc.desc: Test RSSpatialEffectDrawable OnDraw in normal case
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableForegroundTest, RSSpatialEffectDrawableDrawSucc, TestSize.Level1)
{
    // init surface and canvas
    auto surface = Drawing::Surface::MakeRasterN32Premul(10, 10);
    EXPECT_TRUE(surface != nullptr);
    RSPaintFilterCanvas pfCanvas(surface.get());

    // init depth image
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat bitmapFormat { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_OPAQUE };
    bitmap.Build(10, 10, bitmapFormat);
    auto depthImage = std::make_shared<Drawing::Image>();
    depthImage->BuildFromBitmap(bitmap);

    // init node and drawable
    auto renderNodePtr = std::make_shared<RSRenderNode>(0);
    RSRenderNode& renderNode = *renderNodePtr;
    auto depthNode = std::make_shared<RSDepthRenderNode>(1);
    auto depthNodeDrawable = std::make_shared<RSTestRenderNodeDrawableAdapter>(depthNode);
    const auto& depthParams = static_cast<RSDepthRenderParams*>(depthNodeDrawable->GetRenderParams().get());
    depthParams->SetDepthImage(depthImage);
    depthNode->renderDrawable_ = depthNodeDrawable;
    RSSpatialEffectManager::Instance()->RegisterDepthNodeAndSpatialEffect(depthNode, renderNodePtr);

    // init properties
    auto& props = renderNode.GetMutableRenderProperties();
    SpatialEffectPara spatialEffectPara;
    spatialEffectPara.leftTop = Vector3f(0.0f, 0.0f, 1.0f);
    spatialEffectPara.rightTop = Vector3f(1.0f, 0.0f, 1.0f);
    spatialEffectPara.leftBottom = Vector3f(0.0f, 1.0f, 1.0f);
    spatialEffectPara.rightBottom = Vector3f(1.0f, 1.0f, 1.0f);
    spatialEffectPara.occlusionWeight = 1.0;
    props.SetSpatialEffectPara(spatialEffectPara);
    std::vector<Drawing::Point> dstPoints = {
        Drawing::Point(0.f, 0.f),
        Drawing::Point(100.f, 0.f),
        Drawing::Point(100.f, 100.f),
        Drawing::Point(0.f, 100.f)
    };
    props.SetSpatialEffectDstPoints(dstPoints);

    // generate effect drawable and sync
    auto effectDrawable = DrawableV2::RSSpatialEffectDrawable::OnGenerate(renderNode);
    effectDrawable->OnSync();

    Drawing::Rect rect(0.0f, 0.0f, 100, 100);
    EXPECT_EQ(pfCanvas.customStack_.size(), 0);
    effectDrawable->OnDraw(&pfCanvas, &rect);
    // no gpu context, RSPropertyDrawableUtils::DrawDepthOcclusion still return nullptr
    EXPECT_EQ(pfCanvas.customStack_.size(), 0);
}

/**
 * @tc.name: RSSpatialEffectDrawableDrawWithDepth
 * @tc.desc: Test RSSpatialEffectDrawable OnDraw in normal case with depth
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableForegroundTest, RSSpatialEffectDrawableDrawWithDepth, TestSize.Level1)
{
    RSSpatialEffectManager::Instance()->depthSpatialEffectNodeMap_.clear();
    RSSpatialEffectManager::Instance()->spatialEffectDepthNodeMap_.clear();
    // init surface and canvas
    auto surface = Drawing::Surface::MakeRasterN32Premul(10, 10);
    EXPECT_TRUE(surface != nullptr);
    RSPaintFilterCanvas pfCanvas(surface.get());

    // init depth image
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat bitmapFormat { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_OPAQUE };
    bitmap.Build(10, 10, bitmapFormat);
    auto depthImage = std::make_shared<Drawing::Image>();
    depthImage->BuildFromBitmap(bitmap);

    // init node and drawable
    auto renderNodePtr = std::make_shared<RSRenderNode>(0);
    RSRenderNode& renderNode = *renderNodePtr;
    auto depthNode = std::make_shared<RSDepthRenderNode>(1);
    auto depthNodeDrawable = std::make_shared<RSTestRenderNodeDrawableAdapter>(depthNode);
    const auto& depthParams = static_cast<RSDepthRenderParams*>(depthNodeDrawable->GetRenderParams().get());
    depthParams->SetDepthImage(depthImage);
    depthNode->renderDrawable_ = depthNodeDrawable;
    RSSpatialEffectManager::Instance()->RegisterDepthNodeAndSpatialEffect(depthNode, renderNodePtr);

    // init properties
    auto& props = renderNode.GetMutableRenderProperties();
    DepthEffectPara depthEffectPara;
    depthEffectPara.occlusionWeight = 1.0;
    props.SetDepthEffectPara(depthEffectPara);
    std::vector<Drawing::Point> dstPoints = {
        Drawing::Point(0.f, 0.f),
        Drawing::Point(100.f, 0.f),
        Drawing::Point(100.f, 100.f),
        Drawing::Point(0.f, 100.f)
    };
    props.SetSpatialEffectDstPoints(dstPoints);

    // generate effect drawable and sync
    auto effectDrawable = DrawableV2::RSSpatialEffectDrawable::OnGenerate(renderNode);
    ASSERT_NE(effectDrawable, nullptr);
    effectDrawable->OnSync();

    Drawing::Rect rect(0.0f, 0.0f, 100, 100);
    EXPECT_EQ(pfCanvas.customStack_.size(), 0);
    effectDrawable->OnDraw(&pfCanvas, &rect);
    // no gpu context, RSPropertyDrawableUtils::DrawDepthOcclusion still return nullptr
    EXPECT_EQ(pfCanvas.customStack_.size(), 0);
}

/**
 * @tc.name: RSSpatialEffectDrawableDrawNoEffectPara
 * @tc.desc: Test RSSpatialEffectDrawable OnDraw early exit with no effect para
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableForegroundTest, RSSpatialEffectDrawableDrawNoEffectPara, TestSize.Level1)
{
    RSSpatialEffectManager::Instance()->spatialEffectDepthNodeMap_.clear();
    auto canvas = std::make_shared<Drawing::RecordingCanvas>(100, 100);
    EXPECT_NE(canvas, nullptr);
    EXPECT_EQ(canvas->GetSurface(), nullptr);

    std::shared_ptr<DrawableV2::RSSpatialEffectDrawable> spatialEffectDrawable =
        std::make_shared<DrawableV2::RSSpatialEffectDrawable>();
    EXPECT_NE(spatialEffectDrawable, nullptr);
    spatialEffectDrawable->OnDraw(canvas.get(), nullptr);
    EXPECT_EQ(canvas->GetDrawCmdList()->IsEmpty(), true);
}

/**
 * @tc.name: RSSpatialEffectDrawableDrawNoOcclusionWeight
 * @tc.desc: Test RSSpatialEffectDrawable OnDraw early exit with occlusionWeight = 0
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableForegroundTest, RSSpatialEffectDrawableDrawNoOcclusionWeight, TestSize.Level1)
{
    RSSpatialEffectManager::Instance()->spatialEffectDepthNodeMap_.clear();
    auto canvas = std::make_shared<Drawing::RecordingCanvas>(100, 100);
    EXPECT_NE(canvas, nullptr);
    EXPECT_EQ(canvas->GetSurface(), nullptr);

    std::shared_ptr<DrawableV2::RSSpatialEffectDrawable> spatialEffectDrawable =
        std::make_shared<DrawableV2::RSSpatialEffectDrawable>();
    EXPECT_NE(spatialEffectDrawable, nullptr);
    SpatialEffectPara spatialEffectPara;
    spatialEffectPara.occlusionWeight = 0.0;
    spatialEffectDrawable->spatialEffectPara_ = spatialEffectPara;
    spatialEffectDrawable->OnDraw(canvas.get(), nullptr);
    EXPECT_EQ(canvas->GetDrawCmdList()->IsEmpty(), true);
}

/**
 * @tc.name: RSSpatialEffectDrawableDrawNoDstPts
 * @tc.desc: Test RSSpatialEffectDrawable OnDraw early exit with no dstPoints
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableForegroundTest, RSSpatialEffectDrawableDrawNoDstPts, TestSize.Level1)
{
    RSSpatialEffectManager::Instance()->spatialEffectDepthNodeMap_.clear();
    auto canvas = std::make_shared<Drawing::RecordingCanvas>(100, 100);
    EXPECT_NE(canvas, nullptr);
    EXPECT_EQ(canvas->GetSurface(), nullptr);

    std::shared_ptr<DrawableV2::RSSpatialEffectDrawable> spatialEffectDrawable =
        std::make_shared<DrawableV2::RSSpatialEffectDrawable>();
    EXPECT_NE(spatialEffectDrawable, nullptr);
    SpatialEffectPara spatialEffectPara;
    spatialEffectPara.occlusionWeight = 1.0;
    spatialEffectDrawable->spatialEffectPara_ = spatialEffectPara;
    spatialEffectDrawable->OnDraw(canvas.get(), nullptr);
    EXPECT_EQ(canvas->GetDrawCmdList()->IsEmpty(), true);
}

/**
 * @tc.name: RSSpatialEffectDrawableDrawNoDrawable
 * @tc.desc: Test RSSpatialEffectDrawable OnDraw early exit when depthNodeDrawable_ is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableForegroundTest, RSSpatialEffectDrawableDrawNoDrawable, TestSize.Level1)
{
    RSSpatialEffectManager::Instance()->spatialEffectDepthNodeMap_.clear();
    auto canvas = std::make_shared<Drawing::RecordingCanvas>(100, 100);
    EXPECT_NE(canvas, nullptr);
    EXPECT_EQ(canvas->GetSurface(), nullptr);

    std::shared_ptr<DrawableV2::RSSpatialEffectDrawable> spatialEffectDrawable =
        std::make_shared<DrawableV2::RSSpatialEffectDrawable>();
    EXPECT_NE(spatialEffectDrawable, nullptr);
    SpatialEffectPara spatialEffectPara;
    spatialEffectPara.occlusionWeight = 1.0;
    std::vector<Drawing::Point> dstPoints = {};
    spatialEffectDrawable->spatialEffectPara_ = spatialEffectPara;
    spatialEffectDrawable->spatialEffectDstPoints_ = dstPoints;
    spatialEffectDrawable->depthNodeDrawable_.reset();
    spatialEffectDrawable->OnDraw(canvas.get(), nullptr);
    EXPECT_EQ(canvas->GetDrawCmdList()->IsEmpty(), true);
}

/**
 * @tc.name: RSSpatialEffectDrawableDrawNoParams
 * @tc.desc: Test RSSpatialEffectDrawable OnDraw early exit when renderParams is null
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableForegroundTest, RSSpatialEffectDrawableDrawNoParams, TestSize.Level1)
{
    RSSpatialEffectManager::Instance()->spatialEffectDepthNodeMap_.clear();
    auto canvas = std::make_shared<Drawing::RecordingCanvas>(100, 100);
    EXPECT_NE(canvas, nullptr);

    auto depthNode = std::make_shared<RSDepthRenderNode>(1);
    auto depthNodeDrawable = std::make_shared<RSTestRenderNodeDrawableAdapter>(depthNode);
    depthNodeDrawable->renderParams_ = nullptr;
    depthNode->renderDrawable_ = depthNodeDrawable;

    auto renderNodePtr = std::make_shared<RSRenderNode>(0);
    RSSpatialEffectManager::Instance()->RegisterDepthNodeAndSpatialEffect(depthNode, renderNodePtr);

    auto& props = renderNodePtr->GetMutableRenderProperties();
    SpatialEffectPara spatialEffectPara;
    spatialEffectPara.leftTop = Vector3f(0.0f, 0.0f, 1.0f);
    spatialEffectPara.rightTop = Vector3f(1.0f, 0.0f, 1.0f);
    spatialEffectPara.leftBottom = Vector3f(0.0f, 1.0f, 1.0f);
    spatialEffectPara.rightBottom = Vector3f(1.0f, 1.0f, 1.0f);
    spatialEffectPara.occlusionWeight = 1.0;
    props.SetSpatialEffectPara(spatialEffectPara);
    std::vector<Drawing::Point> dstPoints = {
        Drawing::Point(0.f, 0.f),
        Drawing::Point(100.f, 0.f),
        Drawing::Point(100.f, 100.f),
        Drawing::Point(0.f, 100.f)
    };
    props.SetSpatialEffectDstPoints(dstPoints);
    auto effectDrawable = DrawableV2::RSSpatialEffectDrawable::OnGenerate(*renderNodePtr);
    effectDrawable->OnSync();
    effectDrawable->OnDraw(canvas.get(), nullptr);
    EXPECT_EQ(canvas->GetDrawCmdList()->IsEmpty(), true);
    RSSpatialEffectManager::Instance()->spatialEffectDepthNodeMap_.clear();
}

/**
 * @tc.name: RSSpatialEffectDrawableDrawNoDepthImage
 * @tc.desc: Test RSSpatialEffectDrawable OnDraw early exit when depth image is null
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableForegroundTest, RSSpatialEffectDrawableDrawNoDepthImage, TestSize.Level1)
{
    RSSpatialEffectManager::Instance()->spatialEffectDepthNodeMap_.clear();
    auto canvas = std::make_shared<Drawing::RecordingCanvas>(100, 100);
    EXPECT_NE(canvas, nullptr);

    auto depthNode = std::make_shared<RSDepthRenderNode>(1);
    auto depthNodeDrawable = std::make_shared<RSTestRenderNodeDrawableAdapter>(depthNode);
    depthNode->renderDrawable_ = depthNodeDrawable;

    auto renderNodePtr = std::make_shared<RSRenderNode>(0);
    RSSpatialEffectManager::Instance()->RegisterDepthNodeAndSpatialEffect(depthNode, renderNodePtr);

    auto& props = renderNodePtr->GetMutableRenderProperties();
    SpatialEffectPara spatialEffectPara;
    spatialEffectPara.leftTop = Vector3f(0.0f, 0.0f, 1.0f);
    spatialEffectPara.rightTop = Vector3f(1.0f, 0.0f, 1.0f);
    spatialEffectPara.leftBottom = Vector3f(0.0f, 1.0f, 1.0f);
    spatialEffectPara.rightBottom = Vector3f(1.0f, 1.0f, 1.0f);
    spatialEffectPara.occlusionWeight = 1.0;
    props.SetSpatialEffectPara(spatialEffectPara);
    std::vector<Drawing::Point> dstPoints = {
        Drawing::Point(0.f, 0.f),
        Drawing::Point(100.f, 0.f),
        Drawing::Point(100.f, 100.f),
        Drawing::Point(0.f, 100.f)
    };
    props.SetSpatialEffectDstPoints(dstPoints);
    auto effectDrawable = DrawableV2::RSSpatialEffectDrawable::OnGenerate(*renderNodePtr);
    effectDrawable->OnSync();
    effectDrawable->OnDraw(canvas.get(), nullptr);
    EXPECT_EQ(canvas->GetDrawCmdList()->IsEmpty(), true);
    RSSpatialEffectManager::Instance()->spatialEffectDepthNodeMap_.clear();
}

/**
 * @tc.name: RSSpatialEffectDrawableIsNeedSkipOcclusion001
 * @tc.desc: Test OnDraw - occlusionWeight > 0, IsNeedSkipOcclusion returns false, continues to draw
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableForegroundTest, RSSpatialEffectDrawableIsNeedSkipOcclusion001, TestSize.Level1)
{
    RSSpatialEffectManager::Instance()->spatialEffectDepthNodeMap_.clear();
    auto canvas = std::make_shared<Drawing::RecordingCanvas>(100, 100);
    ASSERT_NE(canvas, nullptr);

    auto depthNode = std::make_shared<RSDepthRenderNode>(1);
    auto depthNodeDrawable = std::make_shared<RSTestRenderNodeDrawableAdapter>(depthNode);
    depthNode->renderDrawable_ = depthNodeDrawable;

    auto renderNodePtr = std::make_shared<RSRenderNode>(0);
    RSSpatialEffectManager::Instance()->RegisterDepthNodeAndSpatialEffect(depthNode, renderNodePtr);

    auto& props = renderNodePtr->GetMutableRenderProperties();
    SpatialEffectPara spatialEffectPara;
    spatialEffectPara.leftTop = Vector3f(0.0f, 0.0f, 1.0f);
    spatialEffectPara.rightTop = Vector3f(1.0f, 0.0f, 1.0f);
    spatialEffectPara.leftBottom = Vector3f(0.0f, 1.0f, 1.0f);
    spatialEffectPara.rightBottom = Vector3f(1.0f, 1.0f, 1.0f);
    spatialEffectPara.occlusionWeight = 1.0;
    props.SetSpatialEffectPara(spatialEffectPara);
    std::vector<Drawing::Point> dstPoints = {
        Drawing::Point(0.f, 0.f),
        Drawing::Point(100.f, 0.f),
        Drawing::Point(100.f, 100.f),
        Drawing::Point(0.f, 100.f)
    };
    props.SetSpatialEffectDstPoints(dstPoints);
    auto effectDrawable = DrawableV2::RSSpatialEffectDrawable::OnGenerate(*renderNodePtr);
    ASSERT_NE(effectDrawable, nullptr);
    effectDrawable->OnSync();
    effectDrawable->OnDraw(canvas.get(), nullptr);
    RSSpatialEffectManager::Instance()->spatialEffectDepthNodeMap_.clear();
}

/**
 * @tc.name: RSSpatialEffectDrawableIsNeedSkipOcclusion002
 * @tc.desc: Test OnDraw - occlusionWeight <= 0, all corners within NearFar, IsNeedSkipOcclusion returns true
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableForegroundTest, RSSpatialEffectDrawableIsNeedSkipOcclusion002, TestSize.Level1)
{
    RSSpatialEffectManager::Instance()->spatialEffectDepthNodeMap_.clear();
    auto canvas = std::make_shared<Drawing::RecordingCanvas>(100, 100);
    ASSERT_NE(canvas, nullptr);

    auto depthNode = std::make_shared<RSDepthRenderNode>(1);
    auto depthNodeDrawable = std::make_shared<RSTestRenderNodeDrawableAdapter>(depthNode);
    depthNode->renderDrawable_ = depthNodeDrawable;

    auto renderNodePtr = std::make_shared<RSRenderNode>(0);
    RSSpatialEffectManager::Instance()->RegisterDepthNodeAndSpatialEffect(depthNode, renderNodePtr);

    auto& props = renderNodePtr->GetMutableRenderProperties();
    SpatialEffectPara spatialEffectPara;
    constexpr float DEPTH_WITHIN_RANGE = -5.0f;
    spatialEffectPara.leftTop = Vector3f(0.0f, 0.0f, DEPTH_WITHIN_RANGE);
    spatialEffectPara.rightTop = Vector3f(1.0f, 0.0f, DEPTH_WITHIN_RANGE);
    spatialEffectPara.leftBottom = Vector3f(0.0f, 1.0f, DEPTH_WITHIN_RANGE);
    spatialEffectPara.rightBottom = Vector3f(1.0f, 1.0f, DEPTH_WITHIN_RANGE);
    spatialEffectPara.occlusionWeight = 0.0;
    props.SetSpatialEffectPara(spatialEffectPara);
    std::vector<Drawing::Point> dstPoints = {
        Drawing::Point(0.f, 0.f),
        Drawing::Point(100.f, 0.f),
        Drawing::Point(100.f, 100.f),
        Drawing::Point(0.f, 100.f)
    };
    props.SetSpatialEffectDstPoints(dstPoints);
    auto effectDrawable = DrawableV2::RSSpatialEffectDrawable::OnGenerate(*renderNodePtr);
    ASSERT_NE(effectDrawable, nullptr);
    effectDrawable->OnSync();
    effectDrawable->OnDraw(canvas.get(), nullptr);
    EXPECT_EQ(canvas->GetDrawCmdList()->IsEmpty(), true);
    RSSpatialEffectManager::Instance()->spatialEffectDepthNodeMap_.clear();
}

/**
 * @tc.name: RSSpatialEffectDrawableIsNeedSkipOcclusion003
 * @tc.desc: Test OnDraw - occlusionWeight <= 0, corner beyond NearFar near, IsNeedSkipOcclusion returns false
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableForegroundTest, RSSpatialEffectDrawableIsNeedSkipOcclusion003, TestSize.Level1)
{
    RSSpatialEffectManager::Instance()->spatialEffectDepthNodeMap_.clear();
    auto canvas = std::make_shared<Drawing::RecordingCanvas>(100, 100);
    ASSERT_NE(canvas, nullptr);

    auto depthNode = std::make_shared<RSDepthRenderNode>(1);
    auto depthNodeDrawable = std::make_shared<RSTestRenderNodeDrawableAdapter>(depthNode);
    depthNode->renderDrawable_ = depthNodeDrawable;

    auto renderNodePtr = std::make_shared<RSRenderNode>(0);
    RSSpatialEffectManager::Instance()->RegisterDepthNodeAndSpatialEffect(depthNode, renderNodePtr);

    auto& props = renderNodePtr->GetMutableRenderProperties();
    SpatialEffectPara spatialEffectPara;
    constexpr float DEPTH_BEYOND_NEAR = -0.01f;
    spatialEffectPara.leftTop = Vector3f(0.0f, 0.0f, DEPTH_BEYOND_NEAR);
    spatialEffectPara.rightTop = Vector3f(1.0f, 0.0f, DEPTH_BEYOND_NEAR);
    spatialEffectPara.leftBottom = Vector3f(0.0f, 1.0f, DEPTH_BEYOND_NEAR);
    spatialEffectPara.rightBottom = Vector3f(1.0f, 1.0f, DEPTH_BEYOND_NEAR);
    spatialEffectPara.occlusionWeight = 0.0;
    props.SetSpatialEffectPara(spatialEffectPara);
    std::vector<Drawing::Point> dstPoints = {
        Drawing::Point(0.f, 0.f),
        Drawing::Point(100.f, 0.f),
        Drawing::Point(100.f, 100.f),
        Drawing::Point(0.f, 100.f)
    };
    props.SetSpatialEffectDstPoints(dstPoints);
    auto effectDrawable = DrawableV2::RSSpatialEffectDrawable::OnGenerate(*renderNodePtr);
    ASSERT_NE(effectDrawable, nullptr);
    effectDrawable->OnSync();
    effectDrawable->OnDraw(canvas.get(), nullptr);
    RSSpatialEffectManager::Instance()->spatialEffectDepthNodeMap_.clear();
}

/**
 * @tc.name: RSSpatialEffectDrawableIsNeedSkipOcclusion004
 * @tc.desc: Test OnDraw - occlusionWeight <= 0, corner beyond NearFar far, IsNeedSkipOcclusion returns false
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableForegroundTest, RSSpatialEffectDrawableIsNeedSkipOcclusion004, TestSize.Level1)
{
    RSSpatialEffectManager::Instance()->spatialEffectDepthNodeMap_.clear();
    auto canvas = std::make_shared<Drawing::RecordingCanvas>(100, 100);
    ASSERT_NE(canvas, nullptr);

    auto depthNode = std::make_shared<RSDepthRenderNode>(1);
    auto depthNodeDrawable = std::make_shared<RSTestRenderNodeDrawableAdapter>(depthNode);
    depthNode->renderDrawable_ = depthNodeDrawable;

    auto renderNodePtr = std::make_shared<RSRenderNode>(0);
    RSSpatialEffectManager::Instance()->RegisterDepthNodeAndSpatialEffect(depthNode, renderNodePtr);

    auto& props = renderNodePtr->GetMutableRenderProperties();
    SpatialEffectPara spatialEffectPara;
    constexpr float DEPTH_BEYOND_FAR = -200.0f;
    spatialEffectPara.leftTop = Vector3f(0.0f, 0.0f, DEPTH_BEYOND_FAR);
    spatialEffectPara.rightTop = Vector3f(1.0f, 0.0f, DEPTH_BEYOND_FAR);
    spatialEffectPara.leftBottom = Vector3f(0.0f, 1.0f, DEPTH_BEYOND_FAR);
    spatialEffectPara.rightBottom = Vector3f(1.0f, 1.0f, DEPTH_BEYOND_FAR);
    spatialEffectPara.occlusionWeight = 0.0;
    props.SetSpatialEffectPara(spatialEffectPara);
    std::vector<Drawing::Point> dstPoints = {
        Drawing::Point(0.f, 0.f),
        Drawing::Point(100.f, 0.f),
        Drawing::Point(100.f, 100.f),
        Drawing::Point(0.f, 100.f)
    };
    props.SetSpatialEffectDstPoints(dstPoints);
    auto effectDrawable = DrawableV2::RSSpatialEffectDrawable::OnGenerate(*renderNodePtr);
    ASSERT_NE(effectDrawable, nullptr);
    effectDrawable->OnSync();
    effectDrawable->OnDraw(canvas.get(), nullptr);
    RSSpatialEffectManager::Instance()->spatialEffectDepthNodeMap_.clear();
}

/**
 * @tc.name: RSSpatialEffectDrawableIsNeedSkipOcclusion005
 * @tc.desc: Test OnDraw - occlusionWeight <= 0, corners within NearFar with cameraPara, skip occlusion
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableForegroundTest, RSSpatialEffectDrawableIsNeedSkipOcclusion005, TestSize.Level1)
{
    RSSpatialEffectManager::Instance()->spatialEffectDepthNodeMap_.clear();
    auto canvas = std::make_shared<Drawing::RecordingCanvas>(100, 100);
    ASSERT_NE(canvas, nullptr);

    auto depthNode = std::make_shared<RSDepthRenderNode>(1);
    auto depthNodeDrawable = std::make_shared<RSTestRenderNodeDrawableAdapter>(depthNode);
    const auto& depthParams = static_cast<RSDepthRenderParams*>(depthNodeDrawable->GetRenderParams().get());
    DepthCameraPara cameraPara;
    cameraPara.position = Vector3f(0.0f, 0.0f, 0.0f);
    cameraPara.zNear = 0.1f;
    cameraPara.zFar = 100.0f;
    depthParams->SetDepthCameraPara(cameraPara);
    depthNode->renderDrawable_ = depthNodeDrawable;

    auto renderNodePtr = std::make_shared<RSRenderNode>(0);
    RSSpatialEffectManager::Instance()->RegisterDepthNodeAndSpatialEffect(depthNode, renderNodePtr);

    auto& props = renderNodePtr->GetMutableRenderProperties();
    SpatialEffectPara spatialEffectPara;
    constexpr float DEPTH_WITHIN_CAMERA_RANGE = -5.0f;
    spatialEffectPara.leftTop = Vector3f(0.0f, 0.0f, DEPTH_WITHIN_CAMERA_RANGE);
    spatialEffectPara.rightTop = Vector3f(1.0f, 0.0f, DEPTH_WITHIN_CAMERA_RANGE);
    spatialEffectPara.leftBottom = Vector3f(0.0f, 1.0f, DEPTH_WITHIN_CAMERA_RANGE);
    spatialEffectPara.rightBottom = Vector3f(1.0f, 1.0f, DEPTH_WITHIN_CAMERA_RANGE);
    spatialEffectPara.occlusionWeight = 0.0;
    props.SetSpatialEffectPara(spatialEffectPara);
    std::vector<Drawing::Point> dstPoints = {
        Drawing::Point(0.f, 0.f),
        Drawing::Point(100.f, 0.f),
        Drawing::Point(100.f, 100.f),
        Drawing::Point(0.f, 100.f)
    };
    props.SetSpatialEffectDstPoints(dstPoints);
    auto effectDrawable = DrawableV2::RSSpatialEffectDrawable::OnGenerate(*renderNodePtr);
    ASSERT_NE(effectDrawable, nullptr);
    effectDrawable->OnSync();
    effectDrawable->OnDraw(canvas.get(), nullptr);
    EXPECT_EQ(canvas->GetDrawCmdList()->IsEmpty(), true);
    RSSpatialEffectManager::Instance()->spatialEffectDepthNodeMap_.clear();
}

/**
 * @tc.name: RSSpatialEffectDrawableIsNeedSkipOcclusion006
 * @tc.desc: Test OnDraw - occlusionWeight <= 0, corner beyond camera NearFar, IsNeedSkipOcclusion returns false
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableForegroundTest, RSSpatialEffectDrawableIsNeedSkipOcclusion006, TestSize.Level1)
{
    RSSpatialEffectManager::Instance()->spatialEffectDepthNodeMap_.clear();
    auto canvas = std::make_shared<Drawing::RecordingCanvas>(100, 100);
    ASSERT_NE(canvas, nullptr);

    auto depthNode = std::make_shared<RSDepthRenderNode>(1);
    auto depthNodeDrawable = std::make_shared<RSTestRenderNodeDrawableAdapter>(depthNode);
    const auto& depthParams = static_cast<RSDepthRenderParams*>(depthNodeDrawable->GetRenderParams().get());
    DepthCameraPara cameraPara;
    cameraPara.position = Vector3f(0.0f, 0.0f, 0.0f);
    cameraPara.zNear = 0.1f;
    cameraPara.zFar = 100.0f;
    depthParams->SetDepthCameraPara(cameraPara);
    depthNode->renderDrawable_ = depthNodeDrawable;

    auto renderNodePtr = std::make_shared<RSRenderNode>(0);
    RSSpatialEffectManager::Instance()->RegisterDepthNodeAndSpatialEffect(depthNode, renderNodePtr);

    auto& props = renderNodePtr->GetMutableRenderProperties();
    SpatialEffectPara spatialEffectPara;
    constexpr float DEPTH_BEYOND_CAMERA_FAR = -200.0f;
    spatialEffectPara.leftTop = Vector3f(0.0f, 0.0f, DEPTH_BEYOND_CAMERA_FAR);
    spatialEffectPara.rightTop = Vector3f(1.0f, 0.0f, DEPTH_BEYOND_CAMERA_FAR);
    spatialEffectPara.leftBottom = Vector3f(0.0f, 1.0f, DEPTH_BEYOND_CAMERA_FAR);
    spatialEffectPara.rightBottom = Vector3f(1.0f, 1.0f, DEPTH_BEYOND_CAMERA_FAR);
    spatialEffectPara.occlusionWeight = 0.0;
    props.SetSpatialEffectPara(spatialEffectPara);
    std::vector<Drawing::Point> dstPoints = {
        Drawing::Point(0.f, 0.f),
        Drawing::Point(100.f, 0.f),
        Drawing::Point(100.f, 100.f),
        Drawing::Point(0.f, 100.f)
    };
    props.SetSpatialEffectDstPoints(dstPoints);
    auto effectDrawable = DrawableV2::RSSpatialEffectDrawable::OnGenerate(*renderNodePtr);
    ASSERT_NE(effectDrawable, nullptr);
    effectDrawable->OnSync();
    effectDrawable->OnDraw(canvas.get(), nullptr);
    RSSpatialEffectManager::Instance()->spatialEffectDepthNodeMap_.clear();
}

/**
 * @tc.name: RSSpatialEffectDrawableIsNeedSkipOcclusion007
 * @tc.desc: Test OnDraw - not PerspectiveEnabled
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableForegroundTest, RSSpatialEffectDrawableIsNeedSkipOcclusion007, TestSize.Level1)
{
    RSSpatialEffectManager::Instance()->spatialEffectDepthNodeMap_.clear();
    auto canvas = std::make_shared<Drawing::RecordingCanvas>(100, 100);
    ASSERT_NE(canvas, nullptr);

    auto depthNode = std::make_shared<RSDepthRenderNode>(1);
    auto depthNodeDrawable = std::make_shared<RSTestRenderNodeDrawableAdapter>(depthNode);
    depthNode->renderDrawable_ = depthNodeDrawable;

    auto renderNodePtr = std::make_shared<RSRenderNode>(0);
    RSSpatialEffectManager::Instance()->RegisterDepthNodeAndSpatialEffect(depthNode, renderNodePtr);

    auto& props = renderNodePtr->GetMutableRenderProperties();
    SpatialEffectVariantPara para;
    para.position = -5.0f; // -5.0 : in NearFar range
    para.occlusionWeight = 0.0f;
    props.SetSpatialEffectVariantPara(para);
    auto effectDrawable = DrawableV2::RSSpatialEffectDrawable::OnGenerate(*renderNodePtr);
    ASSERT_NE(effectDrawable, nullptr);
    effectDrawable->OnSync();
    effectDrawable->OnDraw(canvas.get(), nullptr);

    para.position = -500.0f; // out of Far
    props.SetSpatialEffectVariantPara(para);
    effectDrawable = DrawableV2::RSSpatialEffectDrawable::OnGenerate(*renderNodePtr);
    ASSERT_NE(effectDrawable, nullptr);
    effectDrawable->OnSync();
    effectDrawable->OnDraw(canvas.get(), nullptr);

    para.position = 1.0f; // out of Near
    props.SetSpatialEffectVariantPara(para);
    effectDrawable = DrawableV2::RSSpatialEffectDrawable::OnGenerate(*renderNodePtr);
    ASSERT_NE(effectDrawable, nullptr);
    effectDrawable->OnSync();
    effectDrawable->OnDraw(canvas.get(), nullptr);
    RSSpatialEffectManager::Instance()->spatialEffectDepthNodeMap_.clear();
}

/**
 * @tc.name: RSSpatialEffectDrawableCalcDepthPlaneInsufficientPts
 * @tc.desc: Test CalcDepthPlane with insufficient dstPoints (less than 3)
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableForegroundTest, RSSpatialEffectDrawableCalcDepthPlaneInsufficientPts, TestSize.Level1)
{
    SpatialEffectPara para;
    para.leftTop = Vector3f(0.0f, 0.0f, -1.0f);
    para.rightTop = Vector3f(1.0f, 0.0f, -1.0f);
    para.rightBottom = Vector3f(1.0f, 1.0f, -1.0f);

    constexpr int RECT_LEFT = 0;
    constexpr int RECT_TOP = 0;
    Drawing::RectI drawRect(RECT_LEFT, RECT_TOP, 100, 100);

    std::vector<Drawing::Point> dstPoints = { Drawing::Point(0.f, 0.f), Drawing::Point(100.f, 0.f) };

    auto drawable = std::make_shared<DrawableV2::RSSpatialEffectDrawable>();
    auto depthNode = std::make_shared<RSDepthRenderNode>(1);
    auto depthNodeDrawable = std::make_shared<RSTestRenderNodeDrawableAdapter>(depthNode);
    const auto& depthParams = static_cast<RSDepthRenderParams*>(depthNodeDrawable->GetRenderParams().get());
    Vector4f result = drawable->CalcDepthPlane(*depthParams, para, dstPoints, drawRect);

    Vector4f expect(0.f, 0.f, 1.f, -1.f);
    EXPECT_FLOAT_EQ(result.x_, expect.x_);
    EXPECT_FLOAT_EQ(result.y_, expect.y_);
    EXPECT_FLOAT_EQ(result.z_, expect.z_);
    EXPECT_FLOAT_EQ(result.w_, expect.w_);
}

/**
 * @tc.name: RSSpatialEffectDrawableCalcDepthPlaneCollinearPts
 * @tc.desc: Test CalcDepthPlane with collinear points
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableForegroundTest, RSSpatialEffectDrawableCalcDepthPlaneCollinearPts, TestSize.Level1)
{
    SpatialEffectPara para;
    para.leftTop = Vector3f(0.0f, 0.0f, -1.0f);
    para.rightTop = Vector3f(1.0f, 0.0f, -1.0f);
    para.rightBottom = Vector3f(2.0f, 0.0f, -1.0f);

    Drawing::RectI drawRect(0, 0, 100, 100);

    std::vector<Drawing::Point> dstPoints = { Drawing::Point(0.f, 0.f), Drawing::Point(50.f, 0.f),
        Drawing::Point(100.f, 0.f) };

    auto drawable = std::make_shared<DrawableV2::RSSpatialEffectDrawable>();
    auto depthNode = std::make_shared<RSDepthRenderNode>(1);
    auto depthNodeDrawable = std::make_shared<RSTestRenderNodeDrawableAdapter>(depthNode);
    const auto& depthParams = static_cast<RSDepthRenderParams*>(depthNodeDrawable->GetRenderParams().get());
    Vector4f result = drawable->CalcDepthPlane(*depthParams, para, dstPoints, drawRect);

    Vector4f expect(0.f, 0.f, 1.f, -1.f);
    EXPECT_FLOAT_EQ(result.x_, expect.x_);
    EXPECT_FLOAT_EQ(result.y_, expect.y_);
    EXPECT_FLOAT_EQ(result.z_, expect.z_);
    EXPECT_FLOAT_EQ(result.w_, expect.w_);
}

/**
 * @tc.name: RSSpatialEffectDrawableCalcDepthPlaneNormal
 * @tc.desc: Test CalcDepthPlane with valid non-collinear points
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableForegroundTest, RSSpatialEffectDrawableCalcDepthPlaneNormal, TestSize.Level1)
{
    SpatialEffectPara para;
    para.leftTop = Vector3f(0.0f, 0.0f, 2.0f);
    para.rightTop = Vector3f(1.0f, 0.0f, 3.0f);
    para.rightBottom = Vector3f(1.0f, 1.0f, 4.0f);

    Drawing::RectI drawRect(10, 20, 100, 100);

    std::vector<Drawing::Point> dstPoints = { Drawing::Point(10.f, 20.f), Drawing::Point(60.f, 20.f),
        Drawing::Point(60.f, 60.f) };

    auto drawable = std::make_shared<DrawableV2::RSSpatialEffectDrawable>();
    auto depthNode = std::make_shared<RSDepthRenderNode>(1);
    auto depthNodeDrawable = std::make_shared<RSTestRenderNodeDrawableAdapter>(depthNode);
    const auto& depthParams = static_cast<RSDepthRenderParams*>(depthNodeDrawable->GetRenderParams().get());
    Vector4f result = drawable->CalcDepthPlane(*depthParams, para, dstPoints, drawRect);

    EXPECT_GT(result.GetSqrLength(), 0.f);
}

/**
 * @tc.name: DrawBorderWithSDFShader001
 * @tc.desc: Test DrawBorder with SDFShape and SDFShader set (SDF shader path)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyDrawableForegroundTest, DrawBorderWithSDFShader001, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);

    auto sdfShape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_RRECT_SHAPE);
    ASSERT_NE(sdfShape, nullptr);
    node.GetMutableRenderProperties().SetSDFShape(sdfShape);

    auto sdfShader = RSNGRenderShaderBase::Create(RSNGEffectType::BORDER_SDF_SHADER);
    ASSERT_NE(sdfShader, nullptr);

    std::shared_ptr<RSBorder> border = std::make_shared<RSBorder>();
    border->colors_.emplace_back(Color(0xFF, 0xFF, 0xFF, 0xFF));
    border->widths_.emplace_back(2.0f);
    border->styles_.emplace_back(BorderStyle::SOLID);
    border->SetSDFShader(sdfShader);

    std::shared_ptr<DrawableV2::RSBorderDrawable> borderDrawable =
        std::make_shared<DrawableV2::RSBorderDrawable>();
    Drawing::Canvas canvas;
    borderDrawable->DrawBorder(node.GetRenderProperties(), canvas, border, false);
}

/**
 * @tc.name: DrawBorderWithSDFShaderAndOutline001
 * @tc.desc: Test DrawBorder SDF shader path with isOutline=true and zero radius
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyDrawableForegroundTest, DrawBorderWithSDFShaderAndOutline001, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);

    auto sdfShape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_RRECT_SHAPE);
    ASSERT_NE(sdfShape, nullptr);
    node.GetMutableRenderProperties().SetSDFShape(sdfShape);

    auto sdfShader = RSNGRenderShaderBase::Create(RSNGEffectType::BORDER_SDF_SHADER);
    ASSERT_NE(sdfShader, nullptr);

    std::shared_ptr<RSBorder> border = std::make_shared<RSBorder>();
    border->colors_.emplace_back(Color(0xFF, 0xFF, 0xFF, 0xFF));
    border->widths_.emplace_back(2.0f);
    border->styles_.emplace_back(BorderStyle::SOLID);
    border->SetSDFShader(sdfShader);
    border->SetRadiusFour({0.f, 0.f, 0.f, 0.f});

    std::shared_ptr<DrawableV2::RSBorderDrawable> borderDrawable =
        std::make_shared<DrawableV2::RSBorderDrawable>();
    Drawing::Canvas canvas;
    borderDrawable->DrawBorder(node.GetRenderProperties(), canvas, border, true);
}

/**
 * @tc.name: DrawBorderWithSDFShaderNonZeroRadius001
 * @tc.desc: Test DrawBorder SDF shader path with isOutline=true and non-zero radius
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyDrawableForegroundTest, DrawBorderWithSDFShaderNonZeroRadius001, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);

    auto sdfShape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_RRECT_SHAPE);
    ASSERT_NE(sdfShape, nullptr);
    node.GetMutableRenderProperties().SetSDFShape(sdfShape);

    auto sdfShader = RSNGRenderShaderBase::Create(RSNGEffectType::BORDER_SDF_SHADER);
    ASSERT_NE(sdfShader, nullptr);

    std::shared_ptr<RSBorder> border = std::make_shared<RSBorder>();
    border->colors_.emplace_back(Color(0xFF, 0xFF, 0xFF, 0xFF));
    border->widths_.emplace_back(2.0f);
    border->styles_.emplace_back(BorderStyle::SOLID);
    border->SetSDFShader(sdfShader);
    border->SetRadiusFour({10.f, 10.f, 10.f, 10.f});

    std::shared_ptr<DrawableV2::RSBorderDrawable> borderDrawable =
        std::make_shared<DrawableV2::RSBorderDrawable>();
    Drawing::Canvas canvas;
    borderDrawable->DrawBorder(node.GetRenderProperties(), canvas, border, true);
}

/**
 * @tc.name: DrawBorderNoSDFShape001
 * @tc.desc: Test DrawBorder without SDFShape does not enter SDF shader path
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyDrawableForegroundTest, DrawBorderNoSDFShape001, TestSize.Level1)
{
    RSProperties properties;
    std::shared_ptr<RSBorder> border = std::make_shared<RSBorder>();
    border->colors_.emplace_back(Color(0xFF, 0xFF, 0xFF, 0xFF));
    border->widths_.emplace_back(2.0f);
    border->styles_.emplace_back(BorderStyle::SOLID);

    auto sdfShader = RSNGRenderShaderBase::Create(RSNGEffectType::BORDER_SDF_SHADER);
    ASSERT_NE(sdfShader, nullptr);
    border->SetSDFShader(sdfShader);

    std::shared_ptr<DrawableV2::RSBorderDrawable> borderDrawable =
        std::make_shared<DrawableV2::RSBorderDrawable>();
    Drawing::Canvas canvas;
    borderDrawable->DrawBorder(properties, canvas, border, false);
}

/**
 * @tc.name: DrawBorderNoSDFShader001
 * @tc.desc: Test DrawBorder without SDFShader does not enter SDF shader path
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyDrawableForegroundTest, DrawBorderNoSDFShader001, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);

    auto sdfShape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_RRECT_SHAPE);
    ASSERT_NE(sdfShape, nullptr);
    node.GetMutableRenderProperties().SetSDFShape(sdfShape);

    std::shared_ptr<RSBorder> border = std::make_shared<RSBorder>();
    border->colors_.emplace_back(Color(0xFF, 0xFF, 0xFF, 0xFF));
    border->widths_.emplace_back(2.0f);
    border->styles_.emplace_back(BorderStyle::SOLID);

    auto sdfShader = RSNGRenderShaderBase::Create(RSNGEffectType::BORDER_SDF_SHADER);
    ASSERT_NE(sdfShader, nullptr);
    border->SetSDFShader(sdfShader);

    std::shared_ptr<DrawableV2::RSBorderDrawable> borderDrawable =
        std::make_shared<DrawableV2::RSBorderDrawable>();
    Drawing::Canvas canvas;
    borderDrawable->DrawBorder(node.GetRenderProperties(), canvas, border, false);
}

/**
 * @tc.name: RSForegroundFilterRestoreDrawableDrawRectTest001
 * @tc.desc: Test RSForegroundFilterRestoreDrawable::OnUpdate with custom draw rect
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableForegroundTest, RSForegroundFilterRestoreDrawableDrawRectTest001, TestSize.Level1)
{
    RSRenderNode renderNode(1);
    auto drawable = std::make_shared<DrawableV2::RSForegroundFilterRestoreDrawable>();
    EXPECT_NE(drawable, nullptr);

    auto imageFilter = std::make_shared<Drawing::ImageFilter>();
    auto filterPtr = std::make_shared<RSRenderFilterParaBase>();
    std::vector<std::shared_ptr<RSRenderFilterParaBase>> shaderFilters;
    shaderFilters.push_back(filterPtr);
    uint32_t hash = 1;
    auto drawingFilter = std::make_shared<RSDrawingFilter>(imageFilter, shaderFilters, hash);

    renderNode.GetMutableRenderProperties().SetForegroundFilter(drawingFilter);
    renderNode.GetMutableRenderProperties().GetBoundsRect() = RectF(0.0f, 0.0f, 100.0f, 100.0f);

    EXPECT_TRUE(drawable->OnUpdate(renderNode));
    EXPECT_EQ(drawable->stagingDrawRect_, nullptr);
}

/**
 * @tc.name: RSForegroundFilterRestoreDrawableDrawRectTest002
 * @tc.desc: Test RSForegroundFilterRestoreDrawable::OnSync with custom draw rect
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableForegroundTest, RSForegroundFilterRestoreDrawableDrawRectTest002, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSForegroundFilterRestoreDrawable>();
    EXPECT_NE(drawable, nullptr);

    auto stagingDrawRect = std::make_unique<RectF>(10.0f, 10.0f, 50.0f, 50.0f);
    drawable->stagingDrawRect_ = std::make_unique<RectF>(*stagingDrawRect);
    drawable->needSync_ = true;

    drawable->OnSync();

    EXPECT_FALSE(drawable->needSync_);
    EXPECT_NE(drawable->drawRect_, nullptr);
    EXPECT_EQ(drawable->drawRect_->left_, stagingDrawRect->left_);
    EXPECT_EQ(drawable->drawRect_->top_, stagingDrawRect->top_);
    EXPECT_EQ(drawable->drawRect_->width_, stagingDrawRect->width_);
    EXPECT_EQ(drawable->drawRect_->height_, stagingDrawRect->height_);
}

/**
 * @tc.name: RSForegroundFilterRestoreDrawableDrawRectTest003
 * @tc.desc: Test RSForegroundFilterRestoreDrawable::OnSync with null staging draw rect
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableForegroundTest, RSForegroundFilterRestoreDrawableDrawRectTest003, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSForegroundFilterRestoreDrawable>();
    EXPECT_NE(drawable, nullptr);

    drawable->stagingDrawRect_ = nullptr;
    
    drawable->drawRect_ = std::make_unique<RectF>(10.0f, 10.0f, 50.0f, 50.0f);
    drawable->needSync_ = true;

    drawable->OnSync();

    EXPECT_FALSE(drawable->needSync_);
    EXPECT_EQ(drawable->drawRect_, nullptr);
}

/**
 * @tc.name: RSForegroundFilterRestoreDrawableHasCustomRegion001
 * @tc.desc: Test RSForegroundFilterRestoreDrawable::OnUpdate with HasCustomRegion true (line 383 coverage)
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableForegroundTest, RSForegroundFilterRestoreDrawableHasCustomRegion001, TestSize.Level1)
{
    RSRenderNode renderNode(1);
    auto drawable = std::make_shared<DrawableV2::RSForegroundFilterRestoreDrawable>();
    EXPECT_NE(drawable, nullptr);

    auto imageFilter = std::make_shared<Drawing::ImageFilter>();
    auto filterPtr = std::make_shared<RSRenderFilterParaBase>();
    std::vector<std::shared_ptr<RSRenderFilterParaBase>> shaderFilters;
    shaderFilters.push_back(filterPtr);
    uint32_t hash = 1;
    auto drawingFilter = std::make_shared<RSDrawingFilter>(imageFilter, shaderFilters, hash);

    auto renderFilter = RSNGRenderFilterBase::Create(RSNGEffectType::FROSTED_GLASS);
    drawingFilter->SetNGRenderFilter(renderFilter);
    drawingFilter->SetHasCustomRegion(true);

    renderNode.GetMutableRenderProperties().SetForegroundFilter(drawingFilter);
    renderNode.GetMutableRenderProperties().GetBoundsRect() = RectF(0.0f, 0.0f, 100.0f, 100.0f);

    EXPECT_TRUE(drawable->OnUpdate(renderNode));
    EXPECT_NE(drawable->stagingDrawRect_, nullptr);
}

/**
 * @tc.name: RSForegroundFilterRestoreDrawableIsDrawingFilter002
 * @tc.desc: Test OnUpdate with IsDrawingFilter true but HasCustomRegion false
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableForegroundTest, RSForegroundFilterRestoreDrawableIsDrawingFilter002, TestSize.Level1)
{
    RSRenderNode renderNode(1);
    auto drawable = std::make_shared<DrawableV2::RSForegroundFilterRestoreDrawable>();
    EXPECT_NE(drawable, nullptr);

    auto imageFilter = std::make_shared<Drawing::ImageFilter>();
    auto filterPtr = std::make_shared<RSRenderFilterParaBase>();
    std::vector<std::shared_ptr<RSRenderFilterParaBase>> shaderFilters;
    shaderFilters.push_back(filterPtr);
    uint32_t hash = 1;
    auto drawingFilter = std::make_shared<RSDrawingFilter>(imageFilter, shaderFilters, hash);

    drawingFilter->SetHasCustomRegion(false);

    renderNode.GetMutableRenderProperties().SetForegroundFilter(drawingFilter);
    renderNode.GetMutableRenderProperties().GetBoundsRect() = RectF(0.0f, 0.0f, 100.0f, 100.0f);

    EXPECT_TRUE(drawable->OnUpdate(renderNode));
    EXPECT_EQ(drawable->stagingDrawRect_, nullptr);
}

/**
 * @tc.name: RSForegroundFilterRestoreDrawableIsDrawingFilter003
 * @tc.desc: Test OnUpdate with IsDrawingFilter false (non-drawing filter)
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableForegroundTest, RSForegroundFilterRestoreDrawableIsDrawingFilter003, TestSize.Level1)
{
    RSRenderNode renderNode(1);
    auto drawable = std::make_shared<DrawableV2::RSForegroundFilterRestoreDrawable>();
    EXPECT_NE(drawable, nullptr);

    auto rsFilter = std::make_shared<RSFilter>();
    EXPECT_FALSE(rsFilter->IsDrawingFilter());

    renderNode.GetMutableRenderProperties().SetForegroundFilter(rsFilter);
    renderNode.GetMutableRenderProperties().GetBoundsRect() = RectF(0.0f, 0.0f, 100.0f, 100.0f);

    EXPECT_TRUE(drawable->OnUpdate(renderNode));
    EXPECT_EQ(drawable->stagingDrawRect_, nullptr);
}
} // namespace OHOS::Rosen