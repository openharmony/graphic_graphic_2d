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