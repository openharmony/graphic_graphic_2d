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
    RSRenderNode renderNodeTest12(0);
    renderNodeTest12.renderProperties_.GetEffect().illuminatedPtr_ = nullptr;
    std::shared_ptr<DrawableV2::RSPointLightDrawable> pointLightDrawableTest =
        std::make_shared<DrawableV2::RSPointLightDrawable>();
    EXPECT_NE(pointLightDrawableTest, nullptr);
    EXPECT_EQ(pointLightDrawableTest->OnGenerate(renderNodeTest12), nullptr);
    std::shared_ptr<RSIlluminated> illuminated = std::make_shared<RSIlluminated>();
    EXPECT_NE(illuminated, nullptr);
    renderNodeTest12.renderProperties_.GetEffect().illuminatedPtr_ = illuminated;
    renderNodeTest12.renderProperties_.GetEffect().illuminatedPtr_->illuminatedType_ =
        IlluminatedType::NORMAL_BORDER_CONTENT;
    EXPECT_NE(pointLightDrawableTest->OnGenerate(renderNodeTest12), nullptr);
    renderNodeTest12.renderProperties_.GetEffect().illuminatedPtr_->lightSourcesAndPosMap_.emplace(
        std::make_shared<RSLightSource>(), Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    EXPECT_NE(pointLightDrawableTest->OnGenerate(renderNodeTest12), nullptr);
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
 * @tc.name: OnSyncTest003
 * @tc.desc: OnSync test
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawableForegroundTest, OnSyncTest003, TestSize.Level1)
{
    RSProperties propertiesTest1;
    std::shared_ptr<RSIlluminated> illuminatedPtrTest1 = std::make_shared<RSIlluminated>();
    EXPECT_NE(illuminatedPtrTest1, nullptr);
    propertiesTest1.GetEffect().illuminatedPtr_ = illuminatedPtrTest1;
    propertiesTest1.GetEffect().illuminatedPtr_->lightSourcesAndPosMap_.clear();

    std::shared_ptr<DrawableV2::RSPointLightDrawable> pointLightDrawableTest1 =
        std::make_shared<DrawableV2::RSPointLightDrawable>();
    EXPECT_NE(pointLightDrawableTest1, nullptr);
    pointLightDrawableTest1->OnSync();
    RSProperties propertiesTest2;
    std::shared_ptr<RSIlluminated> illuminatedPtrTest2 = std::make_shared<RSIlluminated>();
    EXPECT_NE(illuminatedPtrTest2, nullptr);
    illuminatedPtrTest2->lightSourcesAndPosMap_.emplace(
        std::make_shared<RSLightSource>(), Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    illuminatedPtrTest2->illuminatedType_ = IlluminatedType::BORDER_CONTENT;
    propertiesTest2.GetEffect().illuminatedPtr_ = illuminatedPtrTest2;

    propertiesTest2.boundsGeo_->absRect_ = RectI(0, 1, 2, 3);

    std::shared_ptr<DrawableV2::RSPointLightDrawable> pointLightDrawableTest2 =
        std::make_shared<DrawableV2::RSPointLightDrawable>();
    EXPECT_NE(pointLightDrawableTest2, nullptr);
    pointLightDrawableTest2->needSync_ = true;
    pointLightDrawableTest2->OnSync();
    EXPECT_FALSE(pointLightDrawableTest2->enableEDREffect_);

    auto lightSourcePtr = std::make_shared<RSLightSource>();
    pointLightDrawableTest2->stagingLightSourcesAndPosVec_.emplace_back(
        std::make_pair(lightSourcePtr, Vector4f(0.0f, 0.0f, 1.0f, 1.0f)));
    pointLightDrawableTest2->stagingIlluminatedType_ = IlluminatedType::BORDER_CONTENT;
    pointLightDrawableTest2->needSync_ = true;
    pointLightDrawableTest2->OnSync();
    EXPECT_FALSE(pointLightDrawableTest2->enableEDREffect_);

    lightSourcePtr->SetLightIntensity(2.0f);
    pointLightDrawableTest2->stagingLightSourcesAndPosVec_.emplace_back(
        std::make_pair(lightSourcePtr, Vector4f(0.0f, 0.0f, 1.0f, 1.0f)));
    pointLightDrawableTest2->stagingIlluminatedType_ = IlluminatedType::NORMAL_BORDER_CONTENT;
    pointLightDrawableTest2->needSync_ = true;
    pointLightDrawableTest2->OnSync();
    EXPECT_FALSE(pointLightDrawableTest2->enableEDREffect_);
}

/**
 * @tc.name: OnSyncTest004
 * @tc.desc: OnSync test
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawableForegroundTest, OnSyncTest004, TestSize.Level1)
{
    RSProperties propertiesTest1;
    std::shared_ptr<RSIlluminated> illuminatedPtrTest1 = std::make_shared<RSIlluminated>();
    EXPECT_NE(illuminatedPtrTest1, nullptr);
    illuminatedPtrTest1->lightSourcesAndPosMap_.emplace(
        std::make_shared<RSLightSource>(), Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    illuminatedPtrTest1->illuminatedType_ = IlluminatedType::BLEND_BORDER_CONTENT;
    propertiesTest1.GetEffect().illuminatedPtr_ = illuminatedPtrTest1;
    std::shared_ptr<DrawableV2::RSPointLightDrawable> pointLightDrawableTest1 =
        std::make_shared<DrawableV2::RSPointLightDrawable>();
    EXPECT_NE(pointLightDrawableTest1, nullptr);
    pointLightDrawableTest1->OnSync();
    RSProperties propertiesTest2;
    std::shared_ptr<RSIlluminated> illuminatedPtrTest2 = std::make_shared<RSIlluminated>();
    EXPECT_NE(illuminatedPtrTest2, nullptr);
    illuminatedPtrTest2->lightSourcesAndPosMap_.emplace(
        std::make_shared<RSLightSource>(), Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    illuminatedPtrTest2->illuminatedType_ = IlluminatedType::FEATHERING_BORDER;
    propertiesTest2.GetEffect().illuminatedPtr_ = illuminatedPtrTest2;
    std::shared_ptr<RSObjAbsGeometry> boundsGeo = std::make_shared<RSObjAbsGeometry>();
    EXPECT_NE(boundsGeo, nullptr);
    boundsGeo->absRect_ = RectI(0, 1, 2, 3);
    propertiesTest2.boundsGeo_ = boundsGeo;
    propertiesTest2.SetIlluminatedBorderWidth(1.0f);

    std::shared_ptr<DrawableV2::RSPointLightDrawable> pointLightDrawableTest2 =
        std::make_shared<DrawableV2::RSPointLightDrawable>();
    EXPECT_NE(pointLightDrawableTest2, nullptr);
    pointLightDrawableTest2->stagingBorderWidth_ = 1.0f;
    pointLightDrawableTest2->stagingRRect_ = {{0, 1, 2, 3}, 2, 2};
    pointLightDrawableTest2->needSync_ = true;
    pointLightDrawableTest2->OnSync();
    EXPECT_FLOAT_EQ(pointLightDrawableTest2->contentRRect_.GetRect().GetWidth(),
        pointLightDrawableTest2->borderRRect_.GetRect().GetWidth() + 1.0f);
}


/**
 * @tc.name: CreateDrawFuncTest001
 * @tc.desc: CreateDrawFunc test
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawableForegroundTest, CreateDrawFuncTest001, TestSize.Level1)
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

    std::shared_ptr<DrawableV2::RSPointLightDrawable> pointLightDrawableTest =
        std::make_shared<DrawableV2::RSPointLightDrawable>();
    EXPECT_NE(pointLightDrawableTest, nullptr);
    EXPECT_NE(pointLightDrawableTest->CreateDrawFunc(), nullptr);
}

/**
 * @tc.name: DrawLightTest001
 * @tc.desc: DrawLight test
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawableForegroundTest, DrawLightTest001, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSPointLightDrawable> pointLightDrawableTest =
        std::make_shared<DrawableV2::RSPointLightDrawable>();
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
    pointLightDrawableTest->lightSourcesAndPosVec_.clear();

    Drawing::Canvas canvasTest5;
    pointLightDrawableTest->illuminatedType_ = IlluminatedType::FEATHERING_BORDER;
    pointLightDrawableTest->lightSourcesAndPosVec_.emplace_back(
        std::make_shared<RSLightSource>(), Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    pointLightDrawableTest->DrawLight(&canvasTest5);

    Drawing::Canvas canvasTest6;
    pointLightDrawableTest->illuminatedType_ = IlluminatedType::NORMAL_BORDER_CONTENT;
    pointLightDrawableTest->enableEDREffect_ = true;
    pointLightDrawableTest->lightSourcesAndPosVec_.emplace_back(
        std::make_shared<RSLightSource>(), Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    pointLightDrawableTest->DrawLight(&canvasTest6);
}

/**
 * @tc.name: DrawLightTest002
 * @tc.desc: DrawLight test
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawableForegroundTest, DrawLightTest002, TestSize.Level1)
{
    RSProperties propertiesTest;
    std::shared_ptr<DrawableV2::RSPointLightDrawable> pointLightDrawableTest =
        std::make_shared<DrawableV2::RSPointLightDrawable>();
    EXPECT_NE(pointLightDrawableTest, nullptr);
    propertiesTest.SetBoundsWidth(200);
    propertiesTest.SetBoundsHeight(200);
    Drawing::Canvas canvasTest1;
    pointLightDrawableTest->DrawLight(&canvasTest1);

    Drawing::Canvas canvasTest2;
    pointLightDrawableTest->illuminatedType_ = IlluminatedType::NORMAL_BORDER_CONTENT;
    pointLightDrawableTest->lightSourcesAndPosVec_.emplace_back(
        std::make_shared<RSLightSource>(), Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    pointLightDrawableTest->DrawLight(&canvasTest2);

    Drawing::Canvas canvasTest3;
    pointLightDrawableTest->illuminatedType_ = IlluminatedType::NORMAL_BORDER_CONTENT;
    pointLightDrawableTest->lightSourcesAndPosVec_.emplace_back(
        std::make_shared<RSLightSource>(), Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    pointLightDrawableTest->contentRRect_.SetRect(Drawing::RectF(0.f, 0.f, 200.f, 200.f));
    pointLightDrawableTest->DrawLight(&canvasTest3);
}

/**
 * @tc.name: GetShaderTest001
 * @tc.desc: GetPhongShaderBuilder GetFeatheringBoardLightShaderBuilder test
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawableForegroundTest, GetShaderTest001, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSPointLightDrawable> pointLightDrawableTest =
        std::make_shared<DrawableV2::RSPointLightDrawable>();
    EXPECT_NE(pointLightDrawableTest, nullptr);
    EXPECT_NE(pointLightDrawableTest->GetPhongShaderBuilder(), nullptr);
    EXPECT_NE(pointLightDrawableTest->GetFeatheringBoardLightShaderBuilder(), nullptr);
}

/**
 * @tc.name: GetShaderTest002
 * @tc.desc: GetPhongShaderBuilder GetFeatheringBoardLightShaderBuilder GetNormalLightShaderBuilder test
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawableForegroundTest, GetShaderTest002, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSPointLightDrawable> pointLightDrawableTest =
        std::make_shared<DrawableV2::RSPointLightDrawable>();
    EXPECT_NE(pointLightDrawableTest, nullptr);
    EXPECT_NE(pointLightDrawableTest->GetPhongShaderBuilder(), nullptr);
    EXPECT_NE(pointLightDrawableTest->GetFeatheringBoardLightShaderBuilder(), nullptr);
    EXPECT_NE(pointLightDrawableTest->GetNormalLightShaderBuilder(), nullptr);
    static constexpr char shaderString[] = "";
    EXPECT_NE(pointLightDrawableTest->GetLightShaderBuilder<shaderString>(), nullptr);
}

/**
 * @tc.name: MakeShaderTest001
 * @tc.desc: MakeFeatheringBoardLightShaderBuilder MakeNormalLightShaderBuilder test
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawableForegroundTest, MakeShaderTest001, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSPointLightDrawable> pointLightDrawableTest =
        std::make_shared<DrawableV2::RSPointLightDrawable>();
    EXPECT_NE(pointLightDrawableTest, nullptr);
    EXPECT_NE(pointLightDrawableTest->MakeFeatheringBoardLightShaderBuilder(), nullptr);
    EXPECT_NE(pointLightDrawableTest->MakeNormalLightShaderBuilder(), nullptr);
}

/**
 * @tc.name: GetBrightnessMappingTest001
 * @tc.desc: test results of GetBrightnessMapping
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
*/
HWTEST_F(RSPropertyDrawableForegroundTest, GetBrightnessMappingTest001, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSPointLightDrawable> pointLightDrawableTest =
        std::make_shared<DrawableV2::RSPointLightDrawable>();
    EXPECT_NE(pointLightDrawableTest, nullptr);
    EXPECT_EQ(pointLightDrawableTest->GetBrightnessMapping(10.0f, 4.0f), 4.0f);
    EXPECT_EQ(pointLightDrawableTest->GetBrightnessMapping(1.5f, -1.0f), 0.0f);
    EXPECT_NE(pointLightDrawableTest->GetBrightnessMapping(1.5f, 1.25f), 1.25f);
    EXPECT_NE(pointLightDrawableTest->GetBrightnessMapping(1.5f, 1.1f), 1.1f);
}

/**
 * @tc.name: CalcBezierResultYTest001
 * @tc.desc: test results of CalcBezierResultY
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
*/
HWTEST_F(RSPropertyDrawableForegroundTest, CalcBezierResultYTest001, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSPointLightDrawable> pointLightDrawableTest =
        std::make_shared<DrawableV2::RSPointLightDrawable>();
    auto resultYOptional = pointLightDrawableTest->CalcBezierResultY({0.0f, 0.0f}, {1.0f, 1.0f}, {0.5f, 0.5f}, 0.5f);
    EXPECT_EQ(0.5f, resultYOptional.value_or(0.5f));
    resultYOptional = pointLightDrawableTest->CalcBezierResultY({0.0f, 0.0f}, {1.0f, 1.0f}, {2.0f, 2.0f}, 2.0f);
    EXPECT_EQ(0.0f, resultYOptional.value_or(0.0f));
    resultYOptional = pointLightDrawableTest->CalcBezierResultY({0.0f, 0.0f}, {1.0f, 1.0f}, {2.0f, 2.0f}, 1.0f);
    EXPECT_EQ(1.0f, resultYOptional.value_or(1.0f));
    resultYOptional = pointLightDrawableTest->CalcBezierResultY({4.0f, 0.0f}, {-1.0f, 1.0f}, {2.0f, 2.0f}, 2.0f);
    EXPECT_EQ(1.191836f, resultYOptional.value_or(1.191836f));
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

    auto foregroundFilter = std::make_shared<RSSDFEffectFilter>(sdfShape);
    node.GetMutableRenderProperties().SetForegroundFilterCache(foregroundFilter);
    node.GetMutableRenderProperties().SetForegroundFilter(foregroundFilter);

    std::shared_ptr<RSBorder> border = std::make_shared<RSBorder>();
    Color borderColor = Color();
    Drawing::Color drawingColor(
        borderColor.GetRed(), borderColor.GetGreen(), borderColor.GetBlue(), borderColor.GetAlpha());
    float borderWidth = 2.f;
    border->colors_.emplace_back(borderColor);
    border->widths_.emplace_back(borderWidth);

    Drawing::Canvas canvas;
    borderDrawable->DrawBorder(node.GetRenderProperties(), canvas, border, false);
    EXPECT_FALSE(foregroundFilter->HasBorder());

    border->styles_.emplace_back(BorderStyle::SOLID);
    borderDrawable->DrawBorder(node.GetRenderProperties(), canvas, border, false);
    border->SetRadiusFour({0.f, 0.f, 0.f, 0.f});
    borderDrawable->DrawBorder(node.GetRenderProperties(), canvas, border, true);
    border->SetRadiusFour({1.f, 1.f, 1.f, 1.f});
    borderDrawable->DrawBorder(node.GetRenderProperties(), canvas, border, true);
    EXPECT_FALSE(foregroundFilter->HasBorder());
    EXPECT_EQ(border->widths_.size(), 1);
}

/**
 * @tc.name: DrawBorderLightTest001
 * @tc.desc: DrawBorderLight test with sdf shader effect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyDrawableForegroundTest, DrawBorderLightTest001, TestSize.Level1) {
    std::shared_ptr<DrawableV2::RSPointLightDrawable> pointLightDrawableTest =
        std::make_shared<DrawableV2::RSPointLightDrawable>();
    EXPECT_NE(pointLightDrawableTest, nullptr);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::RuntimeShaderBuilder> lightBuilder = pointLightDrawableTest->GetPhongShaderBuilder();
    EXPECT_NE(lightBuilder, nullptr);
    Drawing::Pen pen;
    std::array<float, DrawableV2::MAX_LIGHT_SOURCES> lightIntensityArray = {0.5f, 0.6f, 0.7f};

    pointLightDrawableTest->sdfShaderEffect_ = std::make_shared<Drawing::ShaderEffect>();
    pointLightDrawableTest->DrawBorderLight(canvas, lightBuilder, pen, lightIntensityArray);
}

/**
 * @tc.name: DrawContentLightTest001
 * @tc.desc: DrawContentLight test with sdf shader effect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyDrawableForegroundTest, DrawContentLightTest001, TestSize.Level1) {
    std::shared_ptr<DrawableV2::RSPointLightDrawable> pointLightDrawableTest =
        std::make_shared<DrawableV2::RSPointLightDrawable>();
    EXPECT_NE(pointLightDrawableTest, nullptr);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::RuntimeShaderBuilder> lightBuilder = pointLightDrawableTest->GetPhongShaderBuilder();
    EXPECT_NE(lightBuilder, nullptr);
    Drawing::Brush brush;
    std::array<float, DrawableV2::MAX_LIGHT_SOURCES> lightIntensityArray = {0.5f, 0.65f, 0.7f};

    pointLightDrawableTest->sdfShaderEffect_ = std::make_shared<Drawing::ShaderEffect>();
    pointLightDrawableTest->DrawContentLight(canvas, lightBuilder, brush, lightIntensityArray);
}

/**
 * @tc.name: DrawSDFBorderLightTest001
 * @tc.desc: DrawSDFBorderLight with null lightShaderEffect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyDrawableForegroundTest, DrawSDFBorderLightTest001, TestSize.Level1) {
    std::shared_ptr<DrawableV2::RSPointLightDrawable> pointLightDrawableTest =
        std::make_shared<DrawableV2::RSPointLightDrawable>();
    EXPECT_NE(pointLightDrawableTest, nullptr);

    Drawing::Canvas canvas1;
    std::shared_ptr<Drawing::ShaderEffect> lightShaderEffect1 = nullptr;
    pointLightDrawableTest->illuminatedType_ = IlluminatedType::BLEND_CONTENT;
    EXPECT_FALSE(pointLightDrawableTest->DrawSDFBorderLight(canvas1, lightShaderEffect1));
}

/**
 * @tc.name: DrawSDFBorderLightTest002
 * @tc.desc: DrawSDFBorderLight with different BLEND illuminatedType
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyDrawableForegroundTest, DrawSDFBorderLightTest002, TestSize.Level1) {
    std::shared_ptr<DrawableV2::RSPointLightDrawable> pointLightDrawableTest =
        std::make_shared<DrawableV2::RSPointLightDrawable>();
    EXPECT_NE(pointLightDrawableTest, nullptr);

    Drawing::Canvas canvas2;
    std::shared_ptr<Drawing::ShaderEffect> lightShaderEffect2 = std::make_shared<Drawing::ShaderEffect>();
    pointLightDrawableTest->illuminatedType_ = IlluminatedType::BLEND_CONTENT;
    EXPECT_TRUE(pointLightDrawableTest->DrawSDFBorderLight(canvas2, lightShaderEffect2));

    Drawing::Canvas canvas3;
    std::shared_ptr<Drawing::ShaderEffect> lightShaderEffect3 = std::make_shared<Drawing::ShaderEffect>();
    pointLightDrawableTest->illuminatedType_ = IlluminatedType::BLEND_BORDER_CONTENT;
    EXPECT_TRUE(pointLightDrawableTest->DrawSDFBorderLight(canvas3, lightShaderEffect3));

    Drawing::Canvas canvas11;
    std::shared_ptr<Drawing::ShaderEffect> lightShaderEffect11 = std::make_shared<Drawing::ShaderEffect>();
    pointLightDrawableTest->illuminatedType_ = IlluminatedType::BLEND_BORDER;
    EXPECT_TRUE(pointLightDrawableTest->DrawSDFBorderLight(canvas11, lightShaderEffect11));
}

/**
 * @tc.name: DrawSDFBorderLightTest003
 * @tc.desc: DrawSDFBorderLight with INVALID or NONE illuminatedType
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyDrawableForegroundTest, DrawSDFBorderLightTest003, TestSize.Level1) {
    std::shared_ptr<DrawableV2::RSPointLightDrawable> pointLightDrawableTest =
        std::make_shared<DrawableV2::RSPointLightDrawable>();
    EXPECT_NE(pointLightDrawableTest, nullptr);

    Drawing::Canvas canvas4;
    std::shared_ptr<Drawing::ShaderEffect> lightShaderEffect4 = std::make_shared<Drawing::ShaderEffect>();
    pointLightDrawableTest->illuminatedType_ = IlluminatedType::INVALID;
    EXPECT_TRUE(pointLightDrawableTest->DrawSDFBorderLight(canvas4, lightShaderEffect4));

    Drawing::Canvas canvas5;
    std::shared_ptr<Drawing::ShaderEffect> lightShaderEffect5 = std::make_shared<Drawing::ShaderEffect>();
    pointLightDrawableTest->illuminatedType_ = IlluminatedType::NONE;
    EXPECT_TRUE(pointLightDrawableTest->DrawSDFBorderLight(canvas5, lightShaderEffect5));
}

/**
 * @tc.name: DrawSDFBorderLightTest004
 * @tc.desc: DrawSDFBorderLight with base illuminatedType
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyDrawableForegroundTest, DrawSDFBorderLightTest004, TestSize.Level1) {
    std::shared_ptr<DrawableV2::RSPointLightDrawable> pointLightDrawableTest =
        std::make_shared<DrawableV2::RSPointLightDrawable>();
    EXPECT_NE(pointLightDrawableTest, nullptr);

    Drawing::Canvas canvas6;
    std::shared_ptr<Drawing::ShaderEffect> lightShaderEffect6 = std::make_shared<Drawing::ShaderEffect>();
    pointLightDrawableTest->illuminatedType_ = IlluminatedType::BORDER;
    EXPECT_TRUE(pointLightDrawableTest->DrawSDFBorderLight(canvas6, lightShaderEffect6));

    Drawing::Canvas canvas7;
    std::shared_ptr<Drawing::ShaderEffect> lightShaderEffect7 = std::make_shared<Drawing::ShaderEffect>();
    pointLightDrawableTest->illuminatedType_ = IlluminatedType::CONTENT;
    EXPECT_TRUE(pointLightDrawableTest->DrawSDFBorderLight(canvas7, lightShaderEffect7));

    Drawing::Canvas canvas8;
    std::shared_ptr<Drawing::ShaderEffect> lightShaderEffect8 = std::make_shared<Drawing::ShaderEffect>();
    pointLightDrawableTest->illuminatedType_ = IlluminatedType::BORDER_CONTENT;
    EXPECT_TRUE(pointLightDrawableTest->DrawSDFBorderLight(canvas8, lightShaderEffect8));

    Drawing::Canvas canvas9;
    std::shared_ptr<Drawing::ShaderEffect> lightShaderEffect9 = std::make_shared<Drawing::ShaderEffect>();
    pointLightDrawableTest->illuminatedType_ = IlluminatedType::BLOOM_BORDER;
    EXPECT_TRUE(pointLightDrawableTest->DrawSDFBorderLight(canvas9, lightShaderEffect9));

    Drawing::Canvas canvas10;
    std::shared_ptr<Drawing::ShaderEffect> lightShaderEffect10 = std::make_shared<Drawing::ShaderEffect>();
    pointLightDrawableTest->illuminatedType_ = IlluminatedType::BLOOM_BORDER_CONTENT;
    EXPECT_TRUE(pointLightDrawableTest->DrawSDFBorderLight(canvas10, lightShaderEffect10));

    Drawing::Canvas canvas12;
    std::shared_ptr<Drawing::ShaderEffect> lightShaderEffect12 = std::make_shared<Drawing::ShaderEffect>();
    pointLightDrawableTest->illuminatedType_ = IlluminatedType::FEATHERING_BORDER;
    EXPECT_TRUE(pointLightDrawableTest->DrawSDFBorderLight(canvas12, lightShaderEffect12));

    Drawing::Canvas canvas13;
    std::shared_ptr<Drawing::ShaderEffect> lightShaderEffect13 = std::make_shared<Drawing::ShaderEffect>();
    pointLightDrawableTest->illuminatedType_ = IlluminatedType::NORMAL_BORDER_CONTENT;
    EXPECT_TRUE(pointLightDrawableTest->DrawSDFBorderLight(canvas13, lightShaderEffect13));
}

/**
 * @tc.name: DrawSDFContentLightTest001
 * @tc.desc: DrawSDFContentLightTest with null lightShaderEffect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyDrawableForegroundTest, DrawSDFContentLightTest001, TestSize.Level1) {
    std::shared_ptr<DrawableV2::RSPointLightDrawable> pointLightDrawableTest =
        std::make_shared<DrawableV2::RSPointLightDrawable>();
    EXPECT_NE(pointLightDrawableTest, nullptr);

    Drawing::Canvas canvas1;
    Drawing::Brush brush1;
    brush1.SetAntiAlias(false);
    std::shared_ptr<Drawing::ShaderEffect> lightShaderEffect1 = nullptr;
    pointLightDrawableTest->illuminatedType_ = IlluminatedType::BLEND_CONTENT;
    EXPECT_FALSE(pointLightDrawableTest->DrawSDFContentLight(canvas1, lightShaderEffect1, brush1));
    EXPECT_FALSE(brush1.IsAntiAlias());
    EXPECT_EQ(brush1.GetBlendMode(), Drawing::BlendMode::CLEAR);
    EXPECT_EQ(nullptr, brush1.GetShaderEffect());
}

/**
 * @tc.name: DrawSDFContentLightTest002
 * @tc.desc: DrawSDFContentLightTest with different BLEND illuminatedType
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyDrawableForegroundTest, DrawSDFContentLightTest002, TestSize.Level1) {
    std::shared_ptr<DrawableV2::RSPointLightDrawable> pointLightDrawableTest =
        std::make_shared<DrawableV2::RSPointLightDrawable>();
    EXPECT_NE(pointLightDrawableTest, nullptr);

    Drawing::Canvas canvas2;
    Drawing::Brush brush2;
    brush2.SetAntiAlias(false);
    std::shared_ptr<Drawing::ShaderEffect> lightShaderEffect2 = std::make_shared<Drawing::ShaderEffect>();
    pointLightDrawableTest->illuminatedType_ = IlluminatedType::BLEND_CONTENT;
    EXPECT_TRUE(pointLightDrawableTest->DrawSDFContentLight(canvas2, lightShaderEffect2, brush2));
    EXPECT_TRUE(brush2.IsAntiAlias());
    EXPECT_EQ(brush2.GetBlendMode(), Drawing::BlendMode::OVERLAY);
    EXPECT_NE(nullptr, brush2.GetShaderEffect());

    Drawing::Canvas canvas3;
    Drawing::Brush brush3;
    brush3.SetAntiAlias(false);
    std::shared_ptr<Drawing::ShaderEffect> lightShaderEffect3 = std::make_shared<Drawing::ShaderEffect>();
    pointLightDrawableTest->illuminatedType_ = IlluminatedType::BLEND_BORDER_CONTENT;
    EXPECT_TRUE(pointLightDrawableTest->DrawSDFContentLight(canvas3, lightShaderEffect3, brush3));
    EXPECT_TRUE(brush3.IsAntiAlias());
    EXPECT_EQ(brush3.GetBlendMode(), Drawing::BlendMode::OVERLAY);
    EXPECT_NE(nullptr, brush3.GetShaderEffect());

    Drawing::Canvas canvas11;
    Drawing::Brush brush11;
    brush11.SetAntiAlias(false);
    std::shared_ptr<Drawing::ShaderEffect> lightShaderEffect11 = std::make_shared<Drawing::ShaderEffect>();
    pointLightDrawableTest->illuminatedType_ = IlluminatedType::BLEND_BORDER;
    EXPECT_TRUE(pointLightDrawableTest->DrawSDFContentLight(canvas11, lightShaderEffect11, brush11));
    EXPECT_FALSE(brush11.IsAntiAlias());
    EXPECT_EQ(brush11.GetBlendMode(), Drawing::BlendMode::CLEAR);
    EXPECT_NE(nullptr, brush11.GetShaderEffect());
}

/**
 * @tc.name: DrawSDFContentLightTest003
 * @tc.desc: DrawSDFContentLightTest with INVALID or NONE illuminatedType
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyDrawableForegroundTest, DrawSDFContentLightTest003, TestSize.Level1) {
    std::shared_ptr<DrawableV2::RSPointLightDrawable> pointLightDrawableTest =
        std::make_shared<DrawableV2::RSPointLightDrawable>();
    EXPECT_NE(pointLightDrawableTest, nullptr);

    Drawing::Canvas canvas4;
    Drawing::Brush brush4;
    brush4.SetAntiAlias(false);
    std::shared_ptr<Drawing::ShaderEffect> lightShaderEffect4 = std::make_shared<Drawing::ShaderEffect>();
    pointLightDrawableTest->illuminatedType_ = IlluminatedType::INVALID;
    EXPECT_TRUE(pointLightDrawableTest->DrawSDFContentLight(canvas4, lightShaderEffect4, brush4));
    EXPECT_FALSE(brush4.IsAntiAlias());
    EXPECT_EQ(brush4.GetBlendMode(), Drawing::BlendMode::CLEAR);
    EXPECT_NE(nullptr, brush4.GetShaderEffect());

    Drawing::Canvas canvas5;
    Drawing::Brush brush5;
    brush5.SetAntiAlias(false);
    std::shared_ptr<Drawing::ShaderEffect> lightShaderEffect5 = std::make_shared<Drawing::ShaderEffect>();
    pointLightDrawableTest->illuminatedType_ = IlluminatedType::NONE;
    EXPECT_TRUE(pointLightDrawableTest->DrawSDFContentLight(canvas5, lightShaderEffect5, brush5));
    EXPECT_FALSE(brush5.IsAntiAlias());
    EXPECT_EQ(brush5.GetBlendMode(), Drawing::BlendMode::CLEAR);
    EXPECT_NE(nullptr, brush5.GetShaderEffect());
}

/**
 * @tc.name: DrawSDFContentLightTest004
 * @tc.desc: DrawSDFContentLightTest with different BLOOM illuminatedType
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyDrawableForegroundTest, DrawSDFContentLightTest004, TestSize.Level1) {
    std::shared_ptr<DrawableV2::RSPointLightDrawable> pointLightDrawableTest =
        std::make_shared<DrawableV2::RSPointLightDrawable>();
    EXPECT_NE(pointLightDrawableTest, nullptr);

    Drawing::Canvas canvas9;
    Drawing::Brush brush9;
    brush9.SetAntiAlias(false);
    std::shared_ptr<Drawing::ShaderEffect> lightShaderEffect9 = std::make_shared<Drawing::ShaderEffect>();
    pointLightDrawableTest->illuminatedType_ = IlluminatedType::BLOOM_BORDER;
    EXPECT_TRUE(pointLightDrawableTest->DrawSDFContentLight(canvas9, lightShaderEffect9, brush9));
    EXPECT_FALSE(brush9.IsAntiAlias());
    EXPECT_EQ(brush9.GetBlendMode(), Drawing::BlendMode::CLEAR);
    EXPECT_NE(nullptr, brush9.GetShaderEffect());

    Drawing::Canvas canvas10;
    Drawing::Brush brush10;
    brush10.SetAntiAlias(false);
    std::shared_ptr<Drawing::ShaderEffect> lightShaderEffect10 = std::make_shared<Drawing::ShaderEffect>();
    pointLightDrawableTest->illuminatedType_ = IlluminatedType::BLOOM_BORDER_CONTENT;
    EXPECT_TRUE(pointLightDrawableTest->DrawSDFContentLight(canvas10, lightShaderEffect10, brush10));
    EXPECT_FALSE(brush10.IsAntiAlias());
    EXPECT_EQ(brush10.GetBlendMode(), Drawing::BlendMode::CLEAR);
    EXPECT_NE(nullptr, brush10.GetShaderEffect());
}

/**
 * @tc.name: DrawSDFContentLightTest005
 * @tc.desc: DrawSDFContentLightTest with base illuminatedType
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyDrawableForegroundTest, DrawSDFContentLightTest005, TestSize.Level1) {
    std::shared_ptr<DrawableV2::RSPointLightDrawable> pointLightDrawableTest =
        std::make_shared<DrawableV2::RSPointLightDrawable>();
    EXPECT_NE(pointLightDrawableTest, nullptr);

    Drawing::Canvas canvas6;
    Drawing::Brush brush6;
    brush6.SetAntiAlias(false);
    std::shared_ptr<Drawing::ShaderEffect> lightShaderEffect6 = std::make_shared<Drawing::ShaderEffect>();
    pointLightDrawableTest->illuminatedType_ = IlluminatedType::BORDER;
    EXPECT_TRUE(pointLightDrawableTest->DrawSDFContentLight(canvas6, lightShaderEffect6, brush6));
    EXPECT_FALSE(brush6.IsAntiAlias());
    EXPECT_EQ(brush6.GetBlendMode(), Drawing::BlendMode::CLEAR);
    EXPECT_NE(nullptr, brush6.GetShaderEffect());

    Drawing::Canvas canvas7;
    Drawing::Brush brush7;
    brush7.SetAntiAlias(false);
    std::shared_ptr<Drawing::ShaderEffect> lightShaderEffect7 = std::make_shared<Drawing::ShaderEffect>();
    pointLightDrawableTest->illuminatedType_ = IlluminatedType::CONTENT;
    EXPECT_TRUE(pointLightDrawableTest->DrawSDFContentLight(canvas7, lightShaderEffect7, brush7));
    EXPECT_FALSE(brush7.IsAntiAlias());
    EXPECT_EQ(brush7.GetBlendMode(), Drawing::BlendMode::CLEAR);
    EXPECT_NE(nullptr, brush7.GetShaderEffect());

    Drawing::Canvas canvas8;
    Drawing::Brush brush8;
    brush8.SetAntiAlias(false);
    std::shared_ptr<Drawing::ShaderEffect> lightShaderEffect8 = std::make_shared<Drawing::ShaderEffect>();
    pointLightDrawableTest->illuminatedType_ = IlluminatedType::BORDER_CONTENT;
    EXPECT_TRUE(pointLightDrawableTest->DrawSDFContentLight(canvas8, lightShaderEffect8, brush8));
    EXPECT_FALSE(brush8.IsAntiAlias());
    EXPECT_EQ(brush8.GetBlendMode(), Drawing::BlendMode::CLEAR);
    EXPECT_NE(nullptr, brush8.GetShaderEffect());
}

/**
 * @tc.name: DrawSDFContentLightTest006
 * @tc.desc: DrawSDFContentLightTest with FEATHERING_BORDER or NORMAL_BORDER_CONTENT illuminatedType
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyDrawableForegroundTest, DrawSDFContentLightTest006, TestSize.Level1) {
    std::shared_ptr<DrawableV2::RSPointLightDrawable> pointLightDrawableTest =
        std::make_shared<DrawableV2::RSPointLightDrawable>();
    EXPECT_NE(pointLightDrawableTest, nullptr);

    Drawing::Canvas canvas12;
    Drawing::Brush brush12;
    brush12.SetAntiAlias(false);
    std::shared_ptr<Drawing::ShaderEffect> lightShaderEffect12 = std::make_shared<Drawing::ShaderEffect>();
    pointLightDrawableTest->illuminatedType_ = IlluminatedType::FEATHERING_BORDER;
    EXPECT_TRUE(pointLightDrawableTest->DrawSDFContentLight(canvas12, lightShaderEffect12, brush12));
    EXPECT_FALSE(brush12.IsAntiAlias());
    EXPECT_EQ(brush12.GetBlendMode(), Drawing::BlendMode::CLEAR);
    EXPECT_NE(nullptr, brush12.GetShaderEffect());

    Drawing::Canvas canvas13;
    Drawing::Brush brush13;
    brush13.SetAntiAlias(false);
    std::shared_ptr<Drawing::ShaderEffect> lightShaderEffect13 = std::make_shared<Drawing::ShaderEffect>();
    pointLightDrawableTest->illuminatedType_ = IlluminatedType::NORMAL_BORDER_CONTENT;
    EXPECT_TRUE(pointLightDrawableTest->DrawSDFContentLight(canvas13, lightShaderEffect13, brush13));
    EXPECT_FALSE(brush13.IsAntiAlias());
    EXPECT_EQ(brush13.GetBlendMode(), Drawing::BlendMode::CLEAR);
    EXPECT_NE(nullptr, brush13.GetShaderEffect());
}

/**
 * @tc.name: RSPointLightDrawableOnUpdateTest001
 * @tc.desc: OnGenerate and OnUpdate test
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawableForegroundTest, RSPointLightDrawableOnUpdateTest001, TestSize.Level1)
{
    RSRenderNode renderNodeTest(0);
    std::shared_ptr<DrawableV2::RSPointLightDrawable> pointLightDrawableTest =
        std::make_shared<DrawableV2::RSPointLightDrawable>();
    EXPECT_NE(pointLightDrawableTest, nullptr);
    std::shared_ptr<RSIlluminated> illuminated = std::make_shared<RSIlluminated>();
    renderNodeTest.renderProperties_.GetEffect().illuminatedPtr_ = illuminated;
    EXPECT_NE(renderNodeTest.renderProperties_.GetEffect().illuminatedPtr_, nullptr);
    renderNodeTest.renderProperties_.GetEffect().illuminatedPtr_->illuminatedType_ =
        IlluminatedType::NORMAL_BORDER_CONTENT;
    EXPECT_NE(pointLightDrawableTest->OnGenerate(renderNodeTest), nullptr);

    renderNodeTest.renderProperties_.GetEffect().illuminatedPtr_->lightSourcesAndPosMap_.emplace(
        std::make_shared<RSLightSource>(), Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    EXPECT_NE(pointLightDrawableTest->OnGenerate(renderNodeTest), nullptr);

    auto sdfShape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_UNION_OP_SHAPE);
    EXPECT_NE(sdfShape, nullptr);
    renderNodeTest.GetMutableRenderProperties().SetSDFShape(sdfShape);
    EXPECT_NE(pointLightDrawableTest->OnGenerate(renderNodeTest), nullptr);
}

/**
 * @tc.name: DrawLightTest003
 * @tc.desc: DrawLight test with sdf and IlluminatedType::FEATHERING_BORDER
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableForegroundTest, DrawLightTest003, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSPointLightDrawable> pointLightDrawableTest =
        std::make_shared<DrawableV2::RSPointLightDrawable>();
    EXPECT_NE(pointLightDrawableTest, nullptr);

    Drawing::Canvas canvasTest;
    pointLightDrawableTest->illuminatedType_ = IlluminatedType::FEATHERING_BORDER;
    pointLightDrawableTest->sdfShaderEffect_ = std::make_shared<Drawing::ShaderEffect>();
    EXPECT_NE(pointLightDrawableTest->sdfShaderEffect_, nullptr);
    pointLightDrawableTest->lightSourcesAndPosVec_.emplace_back(
        std::make_shared<RSLightSource>(), Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    pointLightDrawableTest->DrawLight(&canvasTest);
}
} // namespace OHOS::Rosen