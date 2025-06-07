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

#include <gtest/gtest.h>
#include "drawable/rs_property_drawable_foreground.h"
#include "pipeline/rs_render_node.h"
#include "render/rs_drawing_filter.h"
#include "render/rs_foreground_effect_filter.h"
#include "common/rs_obj_abs_geometry.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSProfilerDrawableForegroundTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSProfilerDrawableForegroundTest::SetUpTestCase() {}
void RSProfilerDrawableForegroundTest::TearDownTestCase() {}
void RSProfilerDrawableForegroundTest::SetUp() {}
void RSProfilerDrawableForegroundTest::TearDown() {}

/**
 * @tc.name: OnGenerateAndOnUpdateTest001
 * @tc.desc: OnGenerate and OnUpdate test
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSProfilerDrawableForegroundTest, OnGenerateAndOnUpdateTest001, TestSize.Level1)
{
    RSRenderNode renderNodeTest1(0);
    Vector4f aiInvertTest(0.0f, 0.0f, 0.0f, 1.0f);
    std::shared_ptr<DrawableV2::RSBinarizationDrawable> binarizationDrawable =
        std::make_shared<DrawableV2::RSBinarizationDrawable>();
    EXPECT_NE(binarizationDrawable, nullptr);
    renderNodeTest1.renderContent_->renderProperties_.aiInvert_ = std::nullopt;
    EXPECT_EQ(binarizationDrawable->OnGenerate(renderNodeTest1), nullptr);
    renderNodeTest1.renderContent_->renderProperties_.aiInvert_ = aiInvertTest;
    EXPECT_NE(binarizationDrawable->OnGenerate(renderNodeTest1), nullptr);

    RSRenderNode renderNodeTest2(0);
    std::shared_ptr<DrawableV2::RSColorFilterDrawable> colorFilterDrawable =
        std::make_shared<DrawableV2::RSColorFilterDrawable>();
    EXPECT_NE(colorFilterDrawable, nullptr);
    renderNodeTest2.renderContent_->renderProperties_.colorFilter_ = nullptr;
    EXPECT_EQ(colorFilterDrawable->OnGenerate(renderNodeTest2), nullptr);
    std::shared_ptr<Drawing::ColorFilter> colorFilter = std::make_shared<Drawing::ColorFilter>();
    EXPECT_NE(colorFilter, nullptr);
    renderNodeTest2.renderContent_->renderProperties_.colorFilter_ = colorFilter;
    EXPECT_NE(colorFilterDrawable->OnGenerate(renderNodeTest2), nullptr);

    RSRenderNode renderNodeTest3(0);
    std::shared_ptr<DrawableV2::RSLightUpEffectDrawable> lightUpEffectDrawable =
        std::make_shared<DrawableV2::RSLightUpEffectDrawable>();
    EXPECT_NE(lightUpEffectDrawable, nullptr);
    renderNodeTest3.renderContent_->renderProperties_.lightUpEffectDegree_ = -1.0f;
    EXPECT_EQ(lightUpEffectDrawable->OnGenerate(renderNodeTest3), nullptr);
    renderNodeTest3.renderContent_->renderProperties_.lightUpEffectDegree_ = 0.1f;
    EXPECT_NE(lightUpEffectDrawable->OnGenerate(renderNodeTest3), nullptr);
}

/**
 * @tc.name: OnGenerateAndOnUpdateTest002
 * @tc.desc: OnGenerate and OnUpdate test
 * @tc.type: FUNC
 * @tc.require:issueIA5Y41
 */
HWTEST_F(RSProfilerDrawableForegroundTest, OnGenerateAndOnUpdateTest002, TestSize.Level1)
{
    RSRenderNode renderNodeTest4(0);
    std::shared_ptr<DrawableV2::RSDynamicDimDrawable> dynamicDimDrawable =
        std::make_shared<DrawableV2::RSDynamicDimDrawable>();
    EXPECT_NE(dynamicDimDrawable, nullptr);
    float dynamicDimDegreeTest1 = -1.0f;
    float dynamicDimDegreeTest2 = 0.1f;
    renderNodeTest4.renderContent_->renderProperties_.dynamicDimDegree_ = dynamicDimDegreeTest1;
    EXPECT_EQ(dynamicDimDrawable->OnGenerate(renderNodeTest4), nullptr);
    renderNodeTest4.renderContent_->renderProperties_.dynamicDimDegree_ = dynamicDimDegreeTest2;
    EXPECT_NE(dynamicDimDrawable->OnGenerate(renderNodeTest4), nullptr);

    RSRenderNode renderNodeTest5(0);
    std::shared_ptr<DrawableV2::RSForegroundColorDrawable> foregroundColorDrawable =
        std::make_shared<DrawableV2::RSForegroundColorDrawable>();
    EXPECT_NE(foregroundColorDrawable, nullptr);
    renderNodeTest5.renderContent_->renderProperties_.decoration_ = std::nullopt;
    EXPECT_EQ(foregroundColorDrawable->OnGenerate(renderNodeTest5), nullptr);
    Decoration decorationTest;
    RSColor val(0.0f, 0.0f, 1.0f, 1.0f);
    decorationTest.foregroundColor_ = val;
    renderNodeTest5.renderContent_->renderProperties_.decoration_ = decorationTest;
    EXPECT_NE(foregroundColorDrawable->OnGenerate(renderNodeTest5), nullptr);

    RSRenderNode renderNodeTest6(0);
    std::shared_ptr<DrawableV2::RSCompositingFilterDrawable> compositingFilterDrawable =
        std::make_shared<DrawableV2::RSCompositingFilterDrawable>();
    EXPECT_NE(compositingFilterDrawable, nullptr);
    renderNodeTest6.renderContent_->renderProperties_.filter_ = nullptr;
    EXPECT_EQ(compositingFilterDrawable->OnGenerate(renderNodeTest6), nullptr);

    RSRenderNode renderNodeTest7(0);
    std::shared_ptr<DrawableV2::RSForegroundFilterDrawable> foregroundFilterDrawable =
        std::make_shared<DrawableV2::RSForegroundFilterDrawable>();
    EXPECT_NE(foregroundFilterDrawable, nullptr);
    renderNodeTest7.renderContent_->renderProperties_.foregroundFilter_ = nullptr;
    EXPECT_EQ(foregroundFilterDrawable->OnGenerate(renderNodeTest7), nullptr);
    EXPECT_FALSE(foregroundFilterDrawable->OnUpdate(renderNodeTest7));
    std::shared_ptr<RSFilter> foregroundFilterTest1 = std::make_shared<RSFilter>();
    EXPECT_NE(foregroundFilterTest1, nullptr);
    renderNodeTest7.renderContent_->renderProperties_.foregroundFilter_ = foregroundFilterTest1;
    EXPECT_NE(foregroundFilterDrawable->OnGenerate(renderNodeTest7), nullptr);
    EXPECT_TRUE(foregroundFilterDrawable->OnUpdate(renderNodeTest7));
}

/**
 * @tc.name: OnGenerateAndOnUpdateTest003
 * @tc.desc: OnGenerate and OnUpdate test
 * @tc.type: FUNC
 * @tc.require:issueIA5Y41
 */
HWTEST_F(RSProfilerDrawableForegroundTest, OnGenerateAndOnUpdateTest003, TestSize.Level1)
{
    RSRenderNode renderNodeTest8(0);
    std::shared_ptr<DrawableV2::RSForegroundFilterRestoreDrawable> foregroundFilterRestoreDrawable =
        std::make_shared<DrawableV2::RSForegroundFilterRestoreDrawable>();
    EXPECT_NE(foregroundFilterRestoreDrawable, nullptr);
    renderNodeTest8.renderContent_->renderProperties_.foregroundFilter_ = nullptr;
    EXPECT_FALSE(foregroundFilterRestoreDrawable->OnUpdate(renderNodeTest8));
    EXPECT_EQ(foregroundFilterRestoreDrawable->OnGenerate(renderNodeTest8), nullptr);
    std::shared_ptr<RSFilter> foregroundFilterTest2 = std::make_shared<RSFilter>();
    EXPECT_NE(foregroundFilterTest2, nullptr);
    renderNodeTest8.renderContent_->renderProperties_.foregroundFilter_ = foregroundFilterTest2;
    EXPECT_NE(foregroundFilterRestoreDrawable->OnGenerate(renderNodeTest8), nullptr);
    EXPECT_TRUE(foregroundFilterRestoreDrawable->OnUpdate(renderNodeTest8));

    RSRenderNode renderNodeTest9(0);
    std::shared_ptr<DrawableV2::RSPixelStretchDrawable> pixelStretchDrawable =
        std::make_shared<DrawableV2::RSPixelStretchDrawable>();
    EXPECT_NE(pixelStretchDrawable, nullptr);
    renderNodeTest9.renderContent_->renderProperties_.pixelStretch_ = std::nullopt;
    EXPECT_EQ(pixelStretchDrawable->OnGenerate(renderNodeTest9), nullptr);
    Vector4f pixelStretchTest(0.0f, 0.0f, 0.0f, 1.0f);
    renderNodeTest9.renderContent_->renderProperties_.pixelStretch_ = pixelStretchTest;
    EXPECT_NE(pixelStretchDrawable->OnGenerate(renderNodeTest9), nullptr);
}

/**
 * @tc.name: OnGenerateAndOnUpdateTest004
 * @tc.desc: OnGenerate and OnUpdate test
 * @tc.type: FUNC
 * @tc.require:issueIA5Y41
 */
HWTEST_F(RSProfilerDrawableForegroundTest, OnGenerateAndOnUpdateTest004, TestSize.Level1)
{
    RSRenderNode renderNodeTest10(0);
    std::shared_ptr<DrawableV2::RSBorderDrawable> borderDrawable = std::make_shared<DrawableV2::RSBorderDrawable>();
    EXPECT_NE(borderDrawable, nullptr);
    renderNodeTest10.renderContent_->renderProperties_.border_ = nullptr;
    EXPECT_EQ(borderDrawable->OnGenerate(renderNodeTest10), nullptr);
    std::shared_ptr<RSBorder> border = std::make_shared<RSBorder>();
    EXPECT_NE(border, nullptr);
    renderNodeTest10.renderContent_->renderProperties_.border_ = border;
    EXPECT_EQ(borderDrawable->OnGenerate(renderNodeTest10), nullptr);
    border->colors_.emplace_back(RSColor(1.0f, 1.0f, 1.0f, 1.0f));
    border->widths_.emplace_back(1.0f);
    border->styles_.emplace_back(BorderStyle::SOLID);
    EXPECT_NE(borderDrawable->OnGenerate(renderNodeTest10), nullptr);

    RSRenderNode renderNodeTest11(0);
    std::shared_ptr<DrawableV2::RSOutlineDrawable> outlineDrawable = std::make_shared<DrawableV2::RSOutlineDrawable>();
    EXPECT_NE(outlineDrawable, nullptr);
    renderNodeTest11.renderContent_->renderProperties_.outline_ = nullptr;
    EXPECT_EQ(outlineDrawable->OnGenerate(renderNodeTest11), nullptr);
    std::shared_ptr<RSBorder> outline = std::make_shared<RSBorder>();
    EXPECT_NE(outline, nullptr);
    outline->colors_.emplace_back(RSColor(1.0f, 1.0f, 1.0f, 1.0f));
    outline->widths_.emplace_back(1.0f);
    outline->styles_.emplace_back(BorderStyle::SOLID);
    renderNodeTest11.renderContent_->renderProperties_.outline_ = outline;
    EXPECT_NE(outlineDrawable->OnGenerate(renderNodeTest11), nullptr);
}

/**
 * @tc.name: OnGenerateAndOnUpdateTest005
 * @tc.desc: OnGenerate and OnUpdate test
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSProfilerDrawableForegroundTest, OnGenerateAndOnUpdateTest005, TestSize.Level1)
{
    RSRenderNode renderNodeTest12(0);
    RSProperties propertiesTest1;
    propertiesTest1.illuminatedPtr_ = nullptr;
    std::shared_ptr<DrawableV2::RSPointLightDrawable> pointLightDrawableTest =
        std::make_shared<DrawableV2::RSPointLightDrawable>(propertiesTest1);
    EXPECT_NE(pointLightDrawableTest, nullptr);
    EXPECT_EQ(pointLightDrawableTest->OnGenerate(renderNodeTest12), nullptr);
    std::shared_ptr<RSIlluminated> illuminated = std::make_shared<RSIlluminated>();
    EXPECT_NE(illuminated, nullptr);
    renderNodeTest12.renderContent_->renderProperties_.illuminatedPtr_ = illuminated;
    renderNodeTest12.renderContent_->renderProperties_.illuminatedPtr_->illuminatedType_ = IlluminatedType::BORDER;
    EXPECT_NE(pointLightDrawableTest->OnGenerate(renderNodeTest12), nullptr);
    // Only wrote OnUpdate function with a return value of false
    RSRenderNode renderNodeTest13(0);
    std::shared_ptr<DrawableV2::RSParticleDrawable> particleDrawable =
        std::make_shared<DrawableV2::RSParticleDrawable>();
    EXPECT_NE(particleDrawable, nullptr);
    renderNodeTest13.renderContent_->renderProperties_.particles_.renderParticleVector_.clear();
    EXPECT_EQ(particleDrawable->OnGenerate(renderNodeTest13), nullptr);

    std::shared_ptr<ParticleRenderParams> particleParams = std::make_shared<ParticleRenderParams>();
    EXPECT_NE(particleParams, nullptr);
    std::vector<std::shared_ptr<RSRenderParticle>> renderParticleVector { std::make_shared<RSRenderParticle>(
        particleParams) };
    renderParticleVector.at(0)->lifeTime_ = 1;
    RSRenderParticleVector particles;
    particles.renderParticleVector_ = renderParticleVector;
    renderNodeTest13.renderContent_->GetMutableRenderProperties().SetParticles(particles);
    ASSERT_TRUE(renderNodeTest13.GetRenderProperties().GetParticles().GetParticleSize());
    std::shared_ptr<RectF> rect = std::make_shared<RectF>(1, 1, 1, 1);
    EXPECT_NE(rect, nullptr);
    renderNodeTest13.renderContent_->GetMutableRenderProperties().SetDrawRegion(rect);
    EXPECT_NE(particleDrawable->OnGenerate(renderNodeTest13), nullptr);
}

/**
 * @tc.name: OnSyncTest001
 * @tc.desc: OnSync test
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSProfilerDrawableForegroundTest, OnSyncTest001, TestSize.Level1)
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
HWTEST_F(RSProfilerDrawableForegroundTest, OnSyncTest002, TestSize.Level1)
{
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
    std::shared_ptr<RSFilter> stagingForegroundFilter = std::make_shared<RSFilter>();
    EXPECT_NE(stagingForegroundFilter, nullptr);
    foregroundFilterRestoreDrawable->stagingForegroundFilter_ = stagingForegroundFilter;
    foregroundFilterRestoreDrawable->needSync_ = true;
    foregroundFilterRestoreDrawable->OnSync();
    EXPECT_FALSE(foregroundFilterRestoreDrawable->needSync_);
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
 * @tc.name: OnSyncTest003
 * @tc.desc: OnSync test
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSProfilerDrawableForegroundTest, OnSyncTest003, TestSize.Level1)
{
    RSProperties propertiesTest1;
    std::shared_ptr<RSIlluminated> illuminatedPtrTest1 = std::make_shared<RSIlluminated>();
    EXPECT_NE(illuminatedPtrTest1, nullptr);
    propertiesTest1.illuminatedPtr_ = illuminatedPtrTest1;
    propertiesTest1.illuminatedPtr_->lightSourcesAndPosMap_.clear();

    std::shared_ptr<DrawableV2::RSPointLightDrawable> pointLightDrawableTest1 =
        std::make_shared<DrawableV2::RSPointLightDrawable>(propertiesTest1);
    EXPECT_NE(pointLightDrawableTest1, nullptr);
    pointLightDrawableTest1->OnSync();
    RSProperties propertiesTest2;
    std::shared_ptr<RSIlluminated> illuminatedPtrTest2 = std::make_shared<RSIlluminated>();
    EXPECT_NE(illuminatedPtrTest2, nullptr);
    illuminatedPtrTest2->lightSourcesAndPosMap_.emplace(
        std::make_shared<RSLightSource>(), Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    illuminatedPtrTest2->illuminatedType_ = IlluminatedType::BORDER_CONTENT;
    propertiesTest2.illuminatedPtr_ = illuminatedPtrTest2;

    propertiesTest2.boundsGeo_->absRect_ = RectI(0, 1, 2, 3);

    std::shared_ptr<DrawableV2::RSPointLightDrawable> pointLightDrawableTest2 =
        std::make_shared<DrawableV2::RSPointLightDrawable>(propertiesTest2);
    EXPECT_NE(pointLightDrawableTest2, nullptr);
    pointLightDrawableTest2->OnSync();
    EXPECT_EQ(pointLightDrawableTest2->rect_, RectI(0, 1, 2, 3));
}

/**
 * @tc.name: CreateDrawFuncTest001
 * @tc.desc: CreateDrawFunc test
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSProfilerDrawableForegroundTest, CreateDrawFuncTest001, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSBinarizationDrawable> binarizationDrawable =
        std::make_shared<DrawableV2::RSBinarizationDrawable>();
    EXPECT_NE(binarizationDrawable, nullptr);
    EXPECT_NE(binarizationDrawable->CreateDrawFunc(), nullptr);

    std::shared_ptr<DrawableV2::RSColorFilterDrawable> colorFilterDrawable =
        std::make_shared<DrawableV2::RSColorFilterDrawable>();
    EXPECT_NE(colorFilterDrawable, nullptr);
    EXPECT_NE(colorFilterDrawable->CreateDrawFunc(), nullptr);

    std::shared_ptr<DrawableV2::RSLightUpEffectDrawable> lightUpEffectDrawable =
        std::make_shared<DrawableV2::RSLightUpEffectDrawable>();
    EXPECT_NE(lightUpEffectDrawable, nullptr);
    EXPECT_NE(lightUpEffectDrawable->CreateDrawFunc(), nullptr);

    std::shared_ptr<DrawableV2::RSDynamicDimDrawable> dynamicDimDrawable =
        std::make_shared<DrawableV2::RSDynamicDimDrawable>();
    EXPECT_NE(dynamicDimDrawable, nullptr);
    EXPECT_NE(dynamicDimDrawable->CreateDrawFunc(), nullptr);

    std::shared_ptr<DrawableV2::RSForegroundFilterDrawable> foregroundFilterDrawable =
        std::make_shared<DrawableV2::RSForegroundFilterDrawable>();
    EXPECT_NE(foregroundFilterDrawable, nullptr);
    EXPECT_NE(foregroundFilterDrawable->CreateDrawFunc(), nullptr);

    std::shared_ptr<DrawableV2::RSForegroundFilterRestoreDrawable> foregroundFilterRestoreDrawable =
        std::make_shared<DrawableV2::RSForegroundFilterRestoreDrawable>();
    EXPECT_NE(foregroundFilterRestoreDrawable, nullptr);
    EXPECT_NE(foregroundFilterRestoreDrawable->CreateDrawFunc(), nullptr);

    std::shared_ptr<DrawableV2::RSPixelStretchDrawable> pixelStretchDrawable =
        std::make_shared<DrawableV2::RSPixelStretchDrawable>();
    EXPECT_NE(pixelStretchDrawable, nullptr);
    EXPECT_NE(pixelStretchDrawable->CreateDrawFunc(), nullptr);

    RSProperties propertiesTest;
    std::shared_ptr<DrawableV2::RSPointLightDrawable> pointLightDrawableTest =
        std::make_shared<DrawableV2::RSPointLightDrawable>(propertiesTest);
    EXPECT_NE(pointLightDrawableTest, nullptr);
    EXPECT_NE(pointLightDrawableTest->CreateDrawFunc(), nullptr);
}

/**
 * @tc.name: DrawLightTest001
 * @tc.desc: DrawLight test
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSProfilerDrawableForegroundTest, DrawLightTest001, TestSize.Level1)
{
    RSProperties propertiesTest;
    std::shared_ptr<DrawableV2::RSPointLightDrawable> pointLightDrawableTest =
        std::make_shared<DrawableV2::RSPointLightDrawable>(propertiesTest);
    EXPECT_NE(pointLightDrawableTest, nullptr);
    Drawing::Canvas canvasTest1;
    pointLightDrawableTest->DrawLight(&canvasTest1);
    Drawing::Canvas canvasTest2;
    pointLightDrawableTest->illuminatedType_ = IlluminatedType::BORDER_CONTENT;
    pointLightDrawableTest->lightSourcesAndPosVec_.emplace_back(
        std::make_shared<RSLightSource>(), Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    pointLightDrawableTest->DrawLight(&canvasTest2);
    pointLightDrawableTest->lightSourcesAndPosVec_.clear();

    Drawing::Canvas canvasTest3;
    pointLightDrawableTest->illuminatedType_ = IlluminatedType::CONTENT;
    pointLightDrawableTest->lightSourcesAndPosVec_.emplace_back(
        std::make_shared<RSLightSource>(), Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    pointLightDrawableTest->DrawLight(&canvasTest3);
    pointLightDrawableTest->lightSourcesAndPosVec_.clear();

    Drawing::Canvas canvasTest4;
    pointLightDrawableTest->illuminatedType_ = IlluminatedType::BORDER;
    pointLightDrawableTest->lightSourcesAndPosVec_.emplace_back(
        std::make_shared<RSLightSource>(), Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    pointLightDrawableTest->DrawLight(&canvasTest4);
}

/**
 * @tc.name: DrawBorderTest001
 * @tc.desc: DrawBorder test
 * @tc.type: FUNC
 * @tc.require:issueIA5Y41
 */
HWTEST_F(RSProfilerDrawableForegroundTest, DrawBorderTest001, TestSize.Level1)
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
} // namespace OHOS::Rosen