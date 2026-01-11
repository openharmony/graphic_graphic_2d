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
#include "effect/rs_render_shape_base.h"
#include "ge_visual_effect_container.h"
#include "pipeline/rs_render_node.h"
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
    renderNodeTest5.renderProperties_.decoration_ = std::nullopt;
    EXPECT_EQ(foregroundColorDrawable->OnGenerate(renderNodeTest5), nullptr);
    Decoration decorationTest;
    RSColor val(0.0f, 0.0f, 1.0f, 1.0f);
    decorationTest.foregroundColor_ = val;
    renderNodeTest5.renderProperties_.decoration_ = decorationTest;
    EXPECT_NE(foregroundColorDrawable->OnGenerate(renderNodeTest5), nullptr);

    RSRenderNode renderNodeTest6(0);
    std::shared_ptr<DrawableV2::RSCompositingFilterDrawable> compositingFilterDrawable =
        std::make_shared<DrawableV2::RSCompositingFilterDrawable>();
    EXPECT_NE(compositingFilterDrawable, nullptr);
    renderNodeTest6.renderProperties_.GetEffect().filter_ = nullptr;
    EXPECT_EQ(compositingFilterDrawable->OnGenerate(renderNodeTest6), nullptr);

    RSRenderNode renderNodeTest7(0);
    std::shared_ptr<DrawableV2::RSForegroundFilterDrawable> foregroundFilterDrawable =
        std::make_shared<DrawableV2::RSForegroundFilterDrawable>();
    EXPECT_NE(foregroundFilterDrawable, nullptr);
    renderNodeTest7.renderProperties_.GetEffect().foregroundFilter_ = nullptr;
    EXPECT_EQ(foregroundFilterDrawable->OnGenerate(renderNodeTest7), nullptr);
    EXPECT_FALSE(foregroundFilterDrawable->OnUpdate(renderNodeTest7));
    std::shared_ptr<RSFilter> foregroundFilterTest1 = std::make_shared<RSFilter>();
    EXPECT_NE(foregroundFilterTest1, nullptr);
    renderNodeTest7.renderProperties_.GetEffect().foregroundFilter_ = foregroundFilterTest1;
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
    renderNodeTest8.renderProperties_.GetEffect().foregroundFilter_ = nullptr;
    EXPECT_FALSE(foregroundFilterRestoreDrawable->OnUpdate(renderNodeTest8));
    EXPECT_EQ(foregroundFilterRestoreDrawable->stagingNodeId_, renderNodeTest8.GetId());
    EXPECT_EQ(foregroundFilterRestoreDrawable->OnGenerate(renderNodeTest8), nullptr);
    std::shared_ptr<RSFilter> foregroundFilterTest2 = std::make_shared<RSFilter>();
    EXPECT_NE(foregroundFilterTest2, nullptr);
    renderNodeTest8.renderProperties_.GetEffect().foregroundFilter_ = foregroundFilterTest2;
    EXPECT_NE(foregroundFilterRestoreDrawable->OnGenerate(renderNodeTest8), nullptr);
    EXPECT_TRUE(foregroundFilterRestoreDrawable->OnUpdate(renderNodeTest8));

    RSRenderNode renderNodeTest9(0);
    std::shared_ptr<DrawableV2::RSPixelStretchDrawable> pixelStretchDrawable =
        std::make_shared<DrawableV2::RSPixelStretchDrawable>();
    EXPECT_NE(pixelStretchDrawable, nullptr);
    renderNodeTest9.renderProperties_.GetEffect().pixelStretch_ = std::nullopt;
    EXPECT_EQ(pixelStretchDrawable->OnGenerate(renderNodeTest9), nullptr);
    Vector4f pixelStretchTest(0.0f, 0.0f, 0.0f, 1.0f);
    renderNodeTest9.renderProperties_.GetEffect().pixelStretch_ = pixelStretchTest;
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
    border->SetRadiusFour({0.f, 0.f, 0.f, 0.f});
    borderDrawable->DrawBorder(node.GetRenderProperties(), canvas, border, true);
    border->SetRadiusFour({1.f, 1.f, 1.f, 1.f});
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
} // namespace OHOS::Rosen