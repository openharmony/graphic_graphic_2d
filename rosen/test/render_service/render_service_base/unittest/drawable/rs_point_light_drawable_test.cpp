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

#include "common/rs_obj_abs_geometry.h"
#include "ge_visual_effect_container.h"
#include "drawable/rs_point_light_drawable.h"
#include "effect/rs_render_shape_base.h"
#include "pipeline/rs_render_node.h"
#include "property/rs_properties_def.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSPropertyDrawablePointLightTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSPropertyDrawablePointLightTest::SetUpTestCase() {}
void RSPropertyDrawablePointLightTest::TearDownTestCase() {}
void RSPropertyDrawablePointLightTest::SetUp() {}
void RSPropertyDrawablePointLightTest::TearDown() {}

/**
 * @tc.name: OnGenerateAndOnUpdateTest001
 * @tc.desc: OnGenerate and OnUpdate test
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawablePointLightTest, OnGenerateAndOnUpdateTest001, TestSize.Level1)
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
}

/**
 * @tc.name: OnSyncTest001
 * @tc.desc: OnSync test
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawablePointLightTest, OnSyncTest001, TestSize.Level1)
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
        std::make_pair(*lightSourcePtr, Vector4f(0.0f, 0.0f, 1.0f, 1.0f)));
    pointLightDrawableTest2->stagingIlluminatedType_ = IlluminatedType::BORDER_CONTENT;
    pointLightDrawableTest2->needSync_ = true;
    pointLightDrawableTest2->OnSync();
    EXPECT_FALSE(pointLightDrawableTest2->enableEDREffect_);

    lightSourcePtr->SetLightIntensity(2.0f);
    pointLightDrawableTest2->stagingLightSourcesAndPosVec_.emplace_back(
        std::make_pair(*lightSourcePtr, Vector4f(0.0f, 0.0f, 1.0f, 1.0f)));
    pointLightDrawableTest2->stagingIlluminatedType_ = IlluminatedType::NORMAL_BORDER_CONTENT;
    pointLightDrawableTest2->needSync_ = true;
    pointLightDrawableTest2->OnSync();
    EXPECT_FALSE(pointLightDrawableTest2->enableEDREffect_);
}

/**
 * @tc.name: OnSyncTest002
 * @tc.desc: OnSync test
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawablePointLightTest, OnSyncTest002, TestSize.Level1)
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
 * @tc.name: OnSyncTest003
 * @tc.desc: OnSync test
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawablePointLightTest, OnSyncTest003, TestSize.Level1)
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
    RSLightSource lightSource;
    pointLightDrawableTest1->lightSourcesAndPosVec_.emplace_back(lightSource, Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    pointLightDrawableTest1->OnSync();
    EXPECT_TRUE(pointLightDrawableTest1->lightSourcesAndPosVec_.empty());
    constexpr int size = DrawableV2::MAX_LIGHT_SOURCES + 1;
    for (int i = 0; i < size; i++) {
        pointLightDrawableTest1->stagingLightSourcesAndPosVec_.emplace_back(
            lightSource, Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    }
    pointLightDrawableTest1->needSync_ = true;
    pointLightDrawableTest1->stagingEnableEDREffect_ = true;
    pointLightDrawableTest1->OnSync();
    EXPECT_FALSE(pointLightDrawableTest1->needSync_);
}

/**
 * @tc.name: DrawLightTest001
 * @tc.desc: DrawLight test
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawablePointLightTest, DrawLightTest001, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSPointLightDrawable> pointLightDrawableTest =
        std::make_shared<DrawableV2::RSPointLightDrawable>();
    EXPECT_NE(pointLightDrawableTest, nullptr);
    Drawing::Canvas canvasTest1;
    pointLightDrawableTest->OnDraw(&canvasTest1, nullptr);
    pointLightDrawableTest->DrawLight(&canvasTest1);
    Drawing::Canvas canvasTest2;
    RSLightSource lightSource;
    pointLightDrawableTest->illuminatedType_ = IlluminatedType::BORDER_CONTENT;
    pointLightDrawableTest->lightSourcesAndPosVec_.emplace_back(lightSource, Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    pointLightDrawableTest->OnDraw(&canvasTest2, nullptr);
    pointLightDrawableTest->DrawLight(&canvasTest2);
    pointLightDrawableTest->lightSourcesAndPosVec_.clear();

    Drawing::Canvas canvasTest3;
    pointLightDrawableTest->illuminatedType_ = IlluminatedType::CONTENT;
    pointLightDrawableTest->lightSourcesAndPosVec_.emplace_back(lightSource, Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    pointLightDrawableTest->OnDraw(&canvasTest3, nullptr);
    pointLightDrawableTest->DrawLight(&canvasTest3);
    pointLightDrawableTest->lightSourcesAndPosVec_.clear();

    Drawing::Canvas canvasTest4;
    pointLightDrawableTest->illuminatedType_ = IlluminatedType::BORDER;
    pointLightDrawableTest->lightSourcesAndPosVec_.emplace_back(lightSource, Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    pointLightDrawableTest->OnDraw(&canvasTest4, nullptr);
    pointLightDrawableTest->DrawLight(&canvasTest4);
    pointLightDrawableTest->lightSourcesAndPosVec_.clear();

    Drawing::Canvas canvasTest5;
    pointLightDrawableTest->illuminatedType_ = IlluminatedType::FEATHERING_BORDER;
    pointLightDrawableTest->lightSourcesAndPosVec_.emplace_back(lightSource, Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    pointLightDrawableTest->OnDraw(&canvasTest5, nullptr);
    pointLightDrawableTest->DrawLight(&canvasTest5);

    Drawing::Canvas canvasTest6;
    pointLightDrawableTest->illuminatedType_ = IlluminatedType::NORMAL_BORDER_CONTENT;
    pointLightDrawableTest->enableEDREffect_ = true;
    pointLightDrawableTest->displayHeadroom_ = 1.0f;
    pointLightDrawableTest->lightSourcesAndPosVec_.emplace_back(lightSource, Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    pointLightDrawableTest->OnDraw(&canvasTest6, nullptr);
    pointLightDrawableTest->DrawLight(&canvasTest6);
}

/**
 * @tc.name: DrawLightTest002
 * @tc.desc: DrawLight test
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawablePointLightTest, DrawLightTest002, TestSize.Level1)
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
    RSLightSource lightSource;
    pointLightDrawableTest->illuminatedType_ = IlluminatedType::NORMAL_BORDER_CONTENT;
    pointLightDrawableTest->lightSourcesAndPosVec_.emplace_back(lightSource, Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    pointLightDrawableTest->DrawLight(&canvasTest2);

    Drawing::Canvas canvasTest3;
    pointLightDrawableTest->illuminatedType_ = IlluminatedType::NORMAL_BORDER_CONTENT;
    pointLightDrawableTest->lightSourcesAndPosVec_.emplace_back(lightSource, Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    pointLightDrawableTest->contentRRect_.SetRect(Drawing::RectF(0.f, 0.f, 200.f, 200.f));
    pointLightDrawableTest->DrawLight(&canvasTest3);

    Drawing::Canvas canvasTest4;
    pointLightDrawableTest->illuminatedType_ = IlluminatedType::BLEND_BORDER;
    pointLightDrawableTest->lightSourcesAndPosVec_.emplace_back(lightSource, Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    pointLightDrawableTest->contentRRect_.SetRect(Drawing::RectF(0.f, 0.f, 200.f, 200.f));
    pointLightDrawableTest->displayHeadroom_ = 1.0f;
    pointLightDrawableTest->DrawLight(&canvasTest4);

    Drawing::Canvas canvasTest5;
    pointLightDrawableTest->illuminatedType_ = IlluminatedType::BLEND_CONTENT;
    pointLightDrawableTest->lightSourcesAndPosVec_.emplace_back(lightSource, Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    pointLightDrawableTest->contentRRect_.SetRect(Drawing::RectF(0.f, 0.f, 200.f, 200.f));
    pointLightDrawableTest->displayHeadroom_ = 1.0f;
    pointLightDrawableTest->DrawLight(&canvasTest5);

    Drawing::Canvas canvasTest6;
    pointLightDrawableTest->illuminatedType_ = IlluminatedType::BLEND_BORDER_CONTENT;
    pointLightDrawableTest->lightSourcesAndPosVec_.emplace_back(lightSource, Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    pointLightDrawableTest->contentRRect_.SetRect(Drawing::RectF(0.f, 0.f, 200.f, 200.f));
    pointLightDrawableTest->displayHeadroom_ = 1.0f;
    pointLightDrawableTest->DrawLight(&canvasTest6);
}
/**
 * @tc.name: GetShaderTest001
 * @tc.desc: GetPhongShaderBuilder GetFeatheringBoardLightShaderBuilder test
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawablePointLightTest, GetShaderTest001, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSPointLightDrawable> pointLightDrawableTest =
        std::make_shared<DrawableV2::RSPointLightDrawable>();
    EXPECT_NE(pointLightDrawableTest, nullptr);
    EXPECT_NE(pointLightDrawableTest->GetPhongShaderBuilder(), nullptr);
    EXPECT_NE(pointLightDrawableTest->GetFeatheringBorderLightShaderBuilder(), nullptr);
}

/**
 * @tc.name: GetShaderTest002
 * @tc.desc: GetPhongShaderBuilder GetFeatheringBoardLightShaderBuilder GetNormalLightShaderBuilder test
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawablePointLightTest, GetShaderTest002, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSPointLightDrawable> pointLightDrawableTest =
        std::make_shared<DrawableV2::RSPointLightDrawable>();
    EXPECT_NE(pointLightDrawableTest, nullptr);
    EXPECT_NE(pointLightDrawableTest->GetPhongShaderBuilder(), nullptr);
    EXPECT_NE(pointLightDrawableTest->GetFeatheringBorderLightShaderBuilder(), nullptr);
    EXPECT_NE(pointLightDrawableTest->GetNormalLightShaderBuilder(), nullptr);
}

/**
 * @tc.name: MakeShaderTest001
 * @tc.desc: MakeFeatheringBoardLightShaderBuilder MakeNormalLightShaderBuilder test
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawablePointLightTest, MakeShaderTest001, TestSize.Level1)
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
HWTEST_F(RSPropertyDrawablePointLightTest, GetBrightnessMappingTest001, TestSize.Level1)
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
HWTEST_F(RSPropertyDrawablePointLightTest, CalcBezierResultYTest001, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSPointLightDrawable> pointLightDrawableTest =
        std::make_shared<DrawableV2::RSPointLightDrawable>();
    auto resultYOptional = pointLightDrawableTest->CalcBezierResultY({0.0f, 0.0f}, {1.0f, 1.0f}, {0.5f, 0.5f}, 0.5f);
    EXPECT_EQ(0.5f, resultYOptional.value_or(0.5f));
    resultYOptional = pointLightDrawableTest->CalcBezierResultY({0.0f, 0.0f}, {1.0f, 1.0f}, {0.5f, 0.5f}, 0.5f);
    EXPECT_EQ(0.5f, resultYOptional.value_or(0.5f));
    resultYOptional = pointLightDrawableTest->CalcBezierResultY({0.0f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f}, 0.5f);
    EXPECT_EQ(0.5f, resultYOptional.value_or(0.5f));
    resultYOptional = pointLightDrawableTest->CalcBezierResultY({0.0f, 0.0f}, {1.0f, 1.0f}, {2.0f, 2.0f}, 2.0f);
    EXPECT_EQ(0.0f, resultYOptional.value_or(0.0f));
    resultYOptional = pointLightDrawableTest->CalcBezierResultY({0.0f, 0.0f}, {1.0f, 1.0f}, {2.0f, 2.0f}, 1.0f);
    EXPECT_EQ(1.0f, resultYOptional.value_or(1.0f));
    resultYOptional = pointLightDrawableTest->CalcBezierResultY({4.0f, 0.0f}, {-1.0f, 1.0f}, {2.0f, 2.0f}, 2.0f);
    EXPECT_EQ(1.191836f, resultYOptional.value_or(1.191836f));
}
/**
 * @tc.name: DrawBorderLightTest001
 * @tc.desc: DrawBorderLight test with sdf shader effect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyDrawablePointLightTest, DrawBorderLightTest001, TestSize.Level1) {
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
HWTEST_F(RSPropertyDrawablePointLightTest, DrawContentLightTest001, TestSize.Level1) {
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
HWTEST_F(RSPropertyDrawablePointLightTest, DrawSDFBorderLightTest001, TestSize.Level1) {
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
HWTEST_F(RSPropertyDrawablePointLightTest, DrawSDFBorderLightTest002, TestSize.Level1) {
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
HWTEST_F(RSPropertyDrawablePointLightTest, DrawSDFBorderLightTest003, TestSize.Level1) {
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
HWTEST_F(RSPropertyDrawablePointLightTest, DrawSDFBorderLightTest004, TestSize.Level1) {
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
HWTEST_F(RSPropertyDrawablePointLightTest, DrawSDFContentLightTest001, TestSize.Level1) {
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
    EXPECT_EQ(brush1.GetBlendMode(), Drawing::BlendMode::SRC_OVER);
    EXPECT_EQ(nullptr, brush1.GetShaderEffect());
}

/**
 * @tc.name: DrawSDFContentLightTest002
 * @tc.desc: DrawSDFContentLightTest with different BLEND illuminatedType
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyDrawablePointLightTest, DrawSDFContentLightTest002, TestSize.Level1) {
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
    EXPECT_EQ(brush11.GetBlendMode(), Drawing::BlendMode::SRC_OVER);
    EXPECT_NE(nullptr, brush11.GetShaderEffect());
}

/**
 * @tc.name: DrawSDFContentLightTest003
 * @tc.desc: DrawSDFContentLightTest with INVALID or NONE illuminatedType
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyDrawablePointLightTest, DrawSDFContentLightTest003, TestSize.Level1) {
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
    EXPECT_EQ(brush4.GetBlendMode(), Drawing::BlendMode::SRC_OVER);
    EXPECT_NE(nullptr, brush4.GetShaderEffect());

    Drawing::Canvas canvas5;
    Drawing::Brush brush5;
    brush5.SetAntiAlias(false);
    std::shared_ptr<Drawing::ShaderEffect> lightShaderEffect5 = std::make_shared<Drawing::ShaderEffect>();
    pointLightDrawableTest->illuminatedType_ = IlluminatedType::NONE;
    EXPECT_TRUE(pointLightDrawableTest->DrawSDFContentLight(canvas5, lightShaderEffect5, brush5));
    EXPECT_FALSE(brush5.IsAntiAlias());
    EXPECT_EQ(brush5.GetBlendMode(), Drawing::BlendMode::SRC_OVER);
    EXPECT_NE(nullptr, brush5.GetShaderEffect());
}

/**
 * @tc.name: DrawSDFContentLightTest004
 * @tc.desc: DrawSDFContentLightTest with different BLOOM illuminatedType
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyDrawablePointLightTest, DrawSDFContentLightTest004, TestSize.Level1) {
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
    EXPECT_EQ(brush9.GetBlendMode(), Drawing::BlendMode::SRC_OVER);
    EXPECT_NE(nullptr, brush9.GetShaderEffect());

    Drawing::Canvas canvas10;
    Drawing::Brush brush10;
    brush10.SetAntiAlias(false);
    std::shared_ptr<Drawing::ShaderEffect> lightShaderEffect10 = std::make_shared<Drawing::ShaderEffect>();
    pointLightDrawableTest->illuminatedType_ = IlluminatedType::BLOOM_BORDER_CONTENT;
    EXPECT_TRUE(pointLightDrawableTest->DrawSDFContentLight(canvas10, lightShaderEffect10, brush10));
    EXPECT_FALSE(brush10.IsAntiAlias());
    EXPECT_EQ(brush10.GetBlendMode(), Drawing::BlendMode::SRC_OVER);
    EXPECT_NE(nullptr, brush10.GetShaderEffect());
}

/**
 * @tc.name: DrawSDFContentLightTest005
 * @tc.desc: DrawSDFContentLightTest with base illuminatedType
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyDrawablePointLightTest, DrawSDFContentLightTest005, TestSize.Level1) {
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
    EXPECT_EQ(brush6.GetBlendMode(), Drawing::BlendMode::SRC_OVER);
    EXPECT_NE(nullptr, brush6.GetShaderEffect());

    Drawing::Canvas canvas7;
    Drawing::Brush brush7;
    brush7.SetAntiAlias(false);
    std::shared_ptr<Drawing::ShaderEffect> lightShaderEffect7 = std::make_shared<Drawing::ShaderEffect>();
    pointLightDrawableTest->illuminatedType_ = IlluminatedType::CONTENT;
    EXPECT_TRUE(pointLightDrawableTest->DrawSDFContentLight(canvas7, lightShaderEffect7, brush7));
    EXPECT_FALSE(brush7.IsAntiAlias());
    EXPECT_EQ(brush7.GetBlendMode(), Drawing::BlendMode::SRC_OVER);
    EXPECT_NE(nullptr, brush7.GetShaderEffect());

    Drawing::Canvas canvas8;
    Drawing::Brush brush8;
    brush8.SetAntiAlias(false);
    std::shared_ptr<Drawing::ShaderEffect> lightShaderEffect8 = std::make_shared<Drawing::ShaderEffect>();
    pointLightDrawableTest->illuminatedType_ = IlluminatedType::BORDER_CONTENT;
    EXPECT_TRUE(pointLightDrawableTest->DrawSDFContentLight(canvas8, lightShaderEffect8, brush8));
    EXPECT_FALSE(brush8.IsAntiAlias());
    EXPECT_EQ(brush8.GetBlendMode(), Drawing::BlendMode::SRC_OVER);
    EXPECT_NE(nullptr, brush8.GetShaderEffect());
}

/**
 * @tc.name: DrawSDFContentLightTest006
 * @tc.desc: DrawSDFContentLightTest with FEATHERING_BORDER or NORMAL_BORDER_CONTENT illuminatedType
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyDrawablePointLightTest, DrawSDFContentLightTest006, TestSize.Level1) {
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
    EXPECT_EQ(brush12.GetBlendMode(), Drawing::BlendMode::SRC_OVER);
    EXPECT_NE(nullptr, brush12.GetShaderEffect());

    Drawing::Canvas canvas13;
    Drawing::Brush brush13;
    brush13.SetAntiAlias(false);
    std::shared_ptr<Drawing::ShaderEffect> lightShaderEffect13 = std::make_shared<Drawing::ShaderEffect>();
    pointLightDrawableTest->illuminatedType_ = IlluminatedType::NORMAL_BORDER_CONTENT;
    EXPECT_TRUE(pointLightDrawableTest->DrawSDFContentLight(canvas13, lightShaderEffect13, brush13));
    EXPECT_FALSE(brush13.IsAntiAlias());
    EXPECT_EQ(brush13.GetBlendMode(), Drawing::BlendMode::SRC_OVER);
    EXPECT_NE(nullptr, brush13.GetShaderEffect());
}

/**
 * @tc.name: RSPointLightDrawableOnUpdateTest001
 * @tc.desc: OnGenerate and OnUpdate test
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawablePointLightTest, RSPointLightDrawableOnUpdateTest001, TestSize.Level1)
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
HWTEST_F(RSPropertyDrawablePointLightTest, DrawLightTest003, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSPointLightDrawable> pointLightDrawableTest =
        std::make_shared<DrawableV2::RSPointLightDrawable>();
    EXPECT_NE(pointLightDrawableTest, nullptr);

    Drawing::Canvas canvasTest;
    pointLightDrawableTest->illuminatedType_ = IlluminatedType::FEATHERING_BORDER;
    pointLightDrawableTest->sdfShaderEffect_ = std::make_shared<Drawing::ShaderEffect>();
    EXPECT_NE(pointLightDrawableTest->sdfShaderEffect_, nullptr);
    RSLightSource lightSource;
    pointLightDrawableTest->lightSourcesAndPosVec_.emplace_back(lightSource, Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    pointLightDrawableTest->DrawLight(&canvasTest);
}
}