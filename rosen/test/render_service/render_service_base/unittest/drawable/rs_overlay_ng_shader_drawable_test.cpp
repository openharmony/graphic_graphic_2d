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
#include "effect/rs_render_shader_base.h"
#include "ge_visual_effect_container.h"
#include "drawable/rs_overlay_ng_shader_drawable.h"
#include "effect/rs_render_shape_base.h"
#include "pipeline/rs_render_node.h"
#include "property/rs_properties_def.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSPropertyDrawableOverlayNGShaderTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSPropertyDrawableOverlayNGShaderTest::SetUpTestCase() {}
void RSPropertyDrawableOverlayNGShaderTest::TearDownTestCase() {}
void RSPropertyDrawableOverlayNGShaderTest::SetUp() {}
void RSPropertyDrawableOverlayNGShaderTest::TearDown() {}

/**
 * @tc.name: OnGenerateAndOnUpdateTest001
 * @tc.desc: OnGenerate and OnUpdate test
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, OnGenerateAndOnUpdateTest001, TestSize.Level1)
{
    RSRenderNode renderNodeTest12(0);
    renderNodeTest12.renderProperties_.GetEffect().illuminatedPtr_ = nullptr;
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> pointLightDrawableTest =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
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
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, OnSyncTest001, TestSize.Level1)
{
    RSProperties propertiesTest1;
    std::shared_ptr<RSIlluminated> illuminatedPtrTest1 = std::make_shared<RSIlluminated>();
    EXPECT_NE(illuminatedPtrTest1, nullptr);
    propertiesTest1.GetEffect().illuminatedPtr_ = illuminatedPtrTest1;
    propertiesTest1.GetEffect().illuminatedPtr_->lightSourcesAndPosMap_.clear();

    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> pointLightDrawableTest1 =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
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

    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> pointLightDrawableTest2 =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
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
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, OnSyncTest002, TestSize.Level1)
{
    RSProperties propertiesTest1;
    std::shared_ptr<RSIlluminated> illuminatedPtrTest1 = std::make_shared<RSIlluminated>();
    EXPECT_NE(illuminatedPtrTest1, nullptr);
    illuminatedPtrTest1->lightSourcesAndPosMap_.emplace(
        std::make_shared<RSLightSource>(), Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    illuminatedPtrTest1->illuminatedType_ = IlluminatedType::BLEND_BORDER_CONTENT;
    propertiesTest1.GetEffect().illuminatedPtr_ = illuminatedPtrTest1;
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> pointLightDrawableTest1 =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
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

    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> pointLightDrawableTest2 =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
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
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, OnSyncTest003, TestSize.Level1)
{
    RSProperties propertiesTest1;
    std::shared_ptr<RSIlluminated> illuminatedPtrTest1 = std::make_shared<RSIlluminated>();
    EXPECT_NE(illuminatedPtrTest1, nullptr);
    illuminatedPtrTest1->lightSourcesAndPosMap_.emplace(
        std::make_shared<RSLightSource>(), Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    illuminatedPtrTest1->illuminatedType_ = IlluminatedType::BLEND_BORDER_CONTENT;
    propertiesTest1.GetEffect().illuminatedPtr_ = illuminatedPtrTest1;
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> pointLightDrawableTest1 =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
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
 * @tc.name: OnSyncTest004
 * @tc.desc: OnSync test with enableEDREffect
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, OnSyncTest004, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> pointLightDrawableTest =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(pointLightDrawableTest, nullptr);
    
    RSLightSource lightSource;
    lightSource.SetLightIntensity(5.0f);
    pointLightDrawableTest->stagingLightSourcesAndPosVec_.emplace_back(
        lightSource, Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    pointLightDrawableTest->stagingIlluminatedType_ = IlluminatedType::NORMAL_BORDER_CONTENT;
    pointLightDrawableTest->stagingEnableEDREffect_ = true;
    pointLightDrawableTest->needSync_ = true;
    pointLightDrawableTest->OnSync();
    EXPECT_TRUE(pointLightDrawableTest->enableEDREffect_);
    EXPECT_FALSE(pointLightDrawableTest->needSync_);
}

/**
 * @tc.name: OnSyncTest005
 * @tc.desc: OnSync test with borderRRect and contentRRect
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, OnSyncTest005, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> pointLightDrawableTest =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(pointLightDrawableTest, nullptr);
    
    RSLightSource lightSource;
    pointLightDrawableTest->stagingLightSourcesAndPosVec_.emplace_back(
        lightSource, Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    pointLightDrawableTest->stagingIlluminatedType_ = IlluminatedType::BORDER_CONTENT;
    pointLightDrawableTest->stagingRRect_ = {{10, 20, 100, 200}, 5, 5};
    pointLightDrawableTest->stagingBorderWidth_ = 3.0f;
    pointLightDrawableTest->needSync_ = true;
    pointLightDrawableTest->OnSync();
    EXPECT_FLOAT_EQ(pointLightDrawableTest->borderWidth_, 3.0f);
}

/**
 * @tc.name: OnSyncTest006
 * @tc.desc: OnSync test to validate illuminatedType_
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, OnSyncTest006, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(drawable, nullptr);
    
    RSLightSource lightSource;
    drawable->stagingLightSourcesAndPosVec_.emplace_back(lightSource, Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    drawable->stagingIlluminatedType_ = IlluminatedType::BORDER_CONTENT;
    drawable->needSync_ = true;
    
    drawable->OnSync();
    
    EXPECT_EQ(drawable->illuminatedType_, IlluminatedType::BORDER_CONTENT);
    EXPECT_FALSE(drawable->needSync_);
}

/**
 * @tc.name: OnSyncTest007
 * @tc.desc: OnSync test to validate nodeId_ and screenNodeId_
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, OnSyncTest007, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(drawable, nullptr);
    
    RSLightSource lightSource;
    drawable->stagingLightSourcesAndPosVec_.emplace_back(lightSource, Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    drawable->stagingNodeId_ = 12345;
    drawable->stagingScreenNodeId_ = 67890;
    drawable->needSync_ = true;
    
    drawable->OnSync();
    
    EXPECT_EQ(drawable->nodeId_, 12345);
    EXPECT_EQ(drawable->screenNodeId_, 67890);
    EXPECT_FALSE(drawable->needSync_);
}

/**
 * @tc.name: OnSyncTest008
 * @tc.desc: OnSync test to validate sdfShaderEffect_
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, OnSyncTest008, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(drawable, nullptr);
    
    RSLightSource lightSource;
    drawable->stagingLightSourcesAndPosVec_.emplace_back(lightSource, Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    auto sdfShader = std::make_shared<Drawing::ShaderEffect>();
    drawable->stagingSDFShaderEffect_ = sdfShader;
    drawable->needSync_ = true;
    
    drawable->OnSync();
    
    EXPECT_NE(drawable->sdfShaderEffect_, nullptr);
    EXPECT_FALSE(drawable->needSync_);
}

/**
 * @tc.name: OnSyncTest009
 * @tc.desc: OnSync test to validate borderRRect_ and contentRRect_ calculation
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, OnSyncTest009, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(drawable, nullptr);
    
    RSLightSource lightSource;
    drawable->stagingLightSourcesAndPosVec_.emplace_back(lightSource, Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    drawable->stagingRRect_ = {{50, 60, 300, 400}, 15, 15};
    drawable->stagingBorderWidth_ = 20.0f;
    drawable->needSync_ = true;
    
    drawable->OnSync();
    
    EXPECT_EQ(drawable->contentRRect_.GetRect().GetLeft(), 50);
    EXPECT_EQ(drawable->contentRRect_.GetRect().GetTop(), 60);
    EXPECT_EQ(drawable->contentRRect_.GetRect().GetWidth(), 300);
    EXPECT_EQ(drawable->contentRRect_.GetRect().GetHeight(), 400);
    EXPECT_FLOAT_EQ(drawable->borderWidth_, 20.0f);
    EXPECT_FALSE(drawable->needSync_);
}

/**
 * @tc.name: OnSyncTest010
 * @tc.desc: OnSync test to validate displayHeadroom_ with EDR enabled
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, OnSyncTest010, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(drawable, nullptr);
    
    RSLightSource lightSource;
    lightSource.SetLightIntensity(5.0f);
    drawable->stagingLightSourcesAndPosVec_.emplace_back(lightSource, Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    drawable->stagingIlluminatedType_ = IlluminatedType::NORMAL_BORDER_CONTENT;
    drawable->stagingEnableEDREffect_ = true;
    drawable->stagingScreenNodeId_ = 1;
    drawable->needSync_ = true;
    
    drawable->OnSync();
    
    EXPECT_TRUE(drawable->enableEDREffect_);
    EXPECT_FALSE(drawable->needSync_);
}

/**
 * @tc.name: OnSyncTest011
 * @tc.desc: OnSync test to validate visualEffectContainer_ with overlay shader
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, OnSyncTest011, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(drawable, nullptr);
    
    RSLightSource lightSource;
    drawable->stagingLightSourcesAndPosVec_.emplace_back(lightSource, Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    auto overlayShader = std::make_shared<RSNGRenderAIBarRectHalo>();
    drawable->stagingOverlayShader_ = overlayShader;
    drawable->needSync_ = true;
    
    drawable->OnSync();
    
    EXPECT_NE(drawable->visualEffectContainer_, nullptr);
    EXPECT_FALSE(drawable->needSync_);
}

/**
 * @tc.name: OnSyncTest012
 * @tc.desc: OnSync test with all properties
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, OnSyncTest012, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(drawable, nullptr);
    
    RSLightSource lightSource;
    lightSource.SetLightIntensity(5.0f);
    drawable->stagingLightSourcesAndPosVec_.emplace_back(lightSource, Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    drawable->stagingIlluminatedType_ = IlluminatedType::NORMAL_BORDER_CONTENT;
    drawable->stagingEnableEDREffect_ = true;
    drawable->stagingBorderWidth_ = 10.0f;
    drawable->stagingRRect_ = {{50, 60, 300, 400}, 15, 15};
    drawable->stagingNodeId_ = 111;
    drawable->stagingScreenNodeId_ = 222;
    auto sdfShader = std::make_shared<Drawing::ShaderEffect>();
    drawable->stagingSDFShaderEffect_ = sdfShader;
    drawable->needSync_ = true;
    
    drawable->OnSync();
    
    EXPECT_EQ(drawable->illuminatedType_, IlluminatedType::NORMAL_BORDER_CONTENT);
    EXPECT_TRUE(drawable->enableEDREffect_);
    EXPECT_FLOAT_EQ(drawable->borderWidth_, 10.0f);
    EXPECT_EQ(drawable->nodeId_, 111);
    EXPECT_EQ(drawable->screenNodeId_, 222);
    EXPECT_NE(drawable->sdfShaderEffect_, nullptr);
    EXPECT_EQ(drawable->contentRRect_.GetRect().GetLeft(), 50);
    EXPECT_EQ(drawable->contentRRect_.GetRect().GetTop(), 60);
    EXPECT_FALSE(drawable->needSync_);
}

/**
 * @tc.name: OnSyncTest013
 * @tc.desc: OnSync test with empty light sources
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, OnSyncTest013, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(drawable, nullptr);
    
    drawable->stagingLightSourcesAndPosVec_.clear();
    drawable->stagingIlluminatedType_ = IlluminatedType::BORDER_CONTENT;
    drawable->needSync_ = true;
    
    drawable->OnSync();
    
    EXPECT_TRUE(drawable->lightSourcesAndPosVec_.empty());
    EXPECT_EQ(drawable->illuminatedType_, IlluminatedType::BORDER_CONTENT);
    EXPECT_FALSE(drawable->needSync_);
}

/**
 * @tc.name: OnSyncTest014
 * @tc.desc: OnSync test with more than MAX_LIGHT_SOURCES
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, OnSyncTest014, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(drawable, nullptr);
    
    RSLightSource lightSource;
    constexpr int size = DrawableV2::MAX_LIGHT_SOURCES + 5;
    for (int i = 0; i < size; i++) {
        drawable->stagingLightSourcesAndPosVec_.emplace_back(
            lightSource, Vector4f(static_cast<float>(i), static_cast<float>(i), 1.0f, 1.0f));
    }
    drawable->needSync_ = true;
    
    drawable->OnSync();
    
    EXPECT_GT(drawable->lightSourcesAndPosVec_.size(), DrawableV2::MAX_LIGHT_SOURCES);
    EXPECT_FALSE(drawable->needSync_);
}

/**
 * @tc.name: OnSyncTest015
 * @tc.desc: OnSync test with null SDF shader
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, OnSyncTest015, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(drawable, nullptr);
    
    RSLightSource lightSource;
    drawable->stagingLightSourcesAndPosVec_.emplace_back(lightSource, Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    drawable->stagingSDFShaderEffect_ = nullptr;
    drawable->needSync_ = true;
    
    drawable->OnSync();
    
    EXPECT_EQ(drawable->sdfShaderEffect_, nullptr);
    EXPECT_FALSE(drawable->needSync_);
}

/**
 * @tc.name: OnSyncTest016
 * @tc.desc: OnSync test with zero border width
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, OnSyncTest016, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(drawable, nullptr);
    
    RSLightSource lightSource;
    drawable->stagingLightSourcesAndPosVec_.emplace_back(lightSource, Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    drawable->stagingRRect_ = {{10, 20, 100, 200}, 5, 5};
    drawable->stagingBorderWidth_ = 0.0f;
    drawable->needSync_ = true;
    
    drawable->OnSync();
    
    EXPECT_FLOAT_EQ(drawable->borderWidth_, 0.0f);
    EXPECT_EQ(drawable->contentRRect_.GetRect().GetLeft(), 10);
    EXPECT_EQ(drawable->borderRRect_.GetRect().GetLeft(), 10);
    EXPECT_FALSE(drawable->needSync_);
}

/**
 * @tc.name: OnSyncTest017
 * @tc.desc: OnSync test with EDR disabled
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, OnSyncTest017, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(drawable, nullptr);
    
    RSLightSource lightSource;
    drawable->stagingLightSourcesAndPosVec_.emplace_back(lightSource, Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    drawable->stagingEnableEDREffect_ = false;
    drawable->needSync_ = true;
    
    drawable->OnSync();
    
    EXPECT_FALSE(drawable->enableEDREffect_);
    EXPECT_FALSE(drawable->needSync_);
}

/**
 * @tc.name: OnSyncTest018
 * @tc.desc: OnSync test to validate light source sorting
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, OnSyncTest018, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(drawable, nullptr);
    
    RSLightSource lightSource;
    drawable->stagingLightSourcesAndPosVec_.emplace_back(lightSource, Vector4f(10.0f, 10.0f, 1.0f, 1.0f));
    drawable->stagingLightSourcesAndPosVec_.emplace_back(lightSource, Vector4f(5.0f, 5.0f, 1.0f, 1.0f));
    drawable->stagingLightSourcesAndPosVec_.emplace_back(lightSource, Vector4f(15.0f, 15.0f, 1.0f, 1.0f));
    drawable->needSync_ = true;
    
    drawable->OnSync();
    
    EXPECT_EQ(drawable->lightSourcesAndPosVec_.size(), 3u);
    EXPECT_FALSE(drawable->needSync_);
}

/**
 * @tc.name: OnSyncTest019
 * @tc.desc: OnSync test when needSync_ is false
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, OnSyncTest019, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(drawable, nullptr);
    
    RSLightSource lightSource;
    drawable->lightSourcesAndPosVec_.emplace_back(lightSource, Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    drawable->needSync_ = false;
    
    drawable->OnSync();
    
    EXPECT_TRUE(drawable->lightSourcesAndPosVec_.empty());
    EXPECT_FALSE(drawable->needSync_);
}

/**
 * @tc.name: DrawLightTest001
 * @tc.desc: DrawLight test
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, DrawLightTest001, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> pointLightDrawableTest =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
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
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, DrawLightTest002, TestSize.Level1)
{
    RSProperties propertiesTest;
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> pointLightDrawableTest =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
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
 * @tc.name: DrawLightTest003
 * @tc.desc: DrawLight test with sdf and IlluminatedType::FEATHERING_BORDER
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, DrawLightTest003, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> pointLightDrawableTest =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(pointLightDrawableTest, nullptr);

    Drawing::Canvas canvasTest;
    pointLightDrawableTest->illuminatedType_ = IlluminatedType::FEATHERING_BORDER;
    pointLightDrawableTest->sdfShaderEffect_ = std::make_shared<Drawing::ShaderEffect>();
    EXPECT_NE(pointLightDrawableTest->sdfShaderEffect_, nullptr);
    RSLightSource lightSource;
    pointLightDrawableTest->lightSourcesAndPosVec_.emplace_back(lightSource, Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    pointLightDrawableTest->DrawLight(&canvasTest);
}

/**
 * @tc.name: DrawLightTest004
 * @tc.desc: DrawLight test with BLEND_BORDER and BLEND_CONTENT
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, DrawLightTest004, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> pointLightDrawableTest =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(pointLightDrawableTest, nullptr);

    RSLightSource lightSource;
    Drawing::Canvas canvasTest1;
    pointLightDrawableTest->illuminatedType_ = IlluminatedType::BLEND_BORDER;
    pointLightDrawableTest->lightSourcesAndPosVec_.emplace_back(lightSource, Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    pointLightDrawableTest->contentRRect_.SetRect(Drawing::RectF(0.f, 0.f, 300.f, 300.f));
    pointLightDrawableTest->displayHeadroom_ = 2.0f;
    pointLightDrawableTest->DrawLight(&canvasTest1);

    Drawing::Canvas canvasTest2;
    pointLightDrawableTest->illuminatedType_ = IlluminatedType::BLEND_CONTENT;
    pointLightDrawableTest->lightSourcesAndPosVec_.clear();
    pointLightDrawableTest->lightSourcesAndPosVec_.emplace_back(lightSource, Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    pointLightDrawableTest->DrawLight(&canvasTest2);
}


/**
 * @tc.name: GetShaderTest001
 * @tc.desc: GetPhongShaderBuilder GetFeatheringBoardLightShaderBuilder test
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, GetShaderTest001, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> pointLightDrawableTest =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
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
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, GetShaderTest002, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> pointLightDrawableTest =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
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
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, MakeShaderTest001, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> pointLightDrawableTest =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
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
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, GetBrightnessMappingTest001, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> pointLightDrawableTest =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(pointLightDrawableTest, nullptr);
    EXPECT_EQ(pointLightDrawableTest->GetBrightnessMapping(10.0f, 4.0f), 4.0f);
    EXPECT_EQ(pointLightDrawableTest->GetBrightnessMapping(1.5f, -1.0f), 0.0f);
    EXPECT_NE(pointLightDrawableTest->GetBrightnessMapping(1.5f, 1.25f), 1.25f);
    EXPECT_NE(pointLightDrawableTest->GetBrightnessMapping(1.5f, 1.1f), 1.1f);
}

/**
 * @tc.name: RSOverlayNGShaderDrawableOnUpdateTest001
 * @tc.desc: OnGenerate and OnUpdate test
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, RSOverlayNGShaderDrawableOnUpdateTest001, TestSize.Level1)
{
    RSRenderNode renderNodeTest(0);
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> pointLightDrawableTest =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
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
 * @tc.name: OnUpdateTest001
 * @tc.desc: OnUpdate test with different illuminatedType
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, OnUpdateTest001, TestSize.Level1)
{
    RSRenderNode renderNodeTest(0);
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> pointLightDrawableTest =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(pointLightDrawableTest, nullptr);
    
    std::shared_ptr<RSIlluminated> illuminated = std::make_shared<RSIlluminated>();
    renderNodeTest.renderProperties_.GetEffect().illuminatedPtr_ = illuminated;
    illuminated->illuminatedType_ = IlluminatedType::BORDER;
    illuminated->lightSourcesAndPosMap_.emplace(
        std::make_shared<RSLightSource>(), Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    
    EXPECT_TRUE(pointLightDrawableTest->OnUpdate(renderNodeTest));
}

/**
 * @tc.name: OnUpdateTest002
 * @tc.desc: OnUpdate test with CONTENT illuminatedType
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, OnUpdateTest002, TestSize.Level1)
{
    RSRenderNode renderNodeTest(0);
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> pointLightDrawableTest =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(pointLightDrawableTest, nullptr);
    
    std::shared_ptr<RSIlluminated> illuminated = std::make_shared<RSIlluminated>();
    renderNodeTest.renderProperties_.GetEffect().illuminatedPtr_ = illuminated;
    illuminated->illuminatedType_ = IlluminatedType::CONTENT;
    auto lightSource = std::make_shared<RSLightSource>();
    lightSource->SetLightIntensity(2.0f);
    illuminated->lightSourcesAndPosMap_.emplace(lightSource, Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
    
    EXPECT_TRUE(pointLightDrawableTest->OnUpdate(renderNodeTest));
}


/**
 * @tc.name: OnGenerateWithOverlayShaderTest001
 * @tc.desc: OnGenerate test with overlay shader
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, OnGenerateWithOverlayShaderTest001, TestSize.Level1)
{
    RSRenderNode renderNodeTest(0);
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> pointLightDrawableTest =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(pointLightDrawableTest, nullptr);
    
    auto overlayShader = std::make_shared<RSNGRenderAIBarRectHalo>();
    EXPECT_NE(overlayShader, nullptr);
    renderNodeTest.GetMutableRenderProperties().SetOverlayNGShader(overlayShader);
    
    auto result = pointLightDrawableTest->OnGenerate(renderNodeTest);
    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: OnUpdateWithOverlayShaderTest001
 * @tc.desc: OnUpdate test with overlay shader
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, OnUpdateWithOverlayShaderTest001, TestSize.Level1)
{
    RSRenderNode renderNodeTest(0);
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> pointLightDrawableTest =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(pointLightDrawableTest, nullptr);
    
    auto overlayShader = std::make_shared<RSNGRenderAIBarRectHalo>();
    EXPECT_NE(overlayShader, nullptr);
    renderNodeTest.GetMutableRenderProperties().SetOverlayNGShader(overlayShader);
    
    EXPECT_TRUE(pointLightDrawableTest->OnUpdate(renderNodeTest));
}

/**
 * @tc.name: OnSyncWithOverlayShaderTest001
 * @tc.desc: OnSync test with overlay shader
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, OnSyncWithOverlayShaderTest001, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> pointLightDrawableTest =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(pointLightDrawableTest, nullptr);
    
    auto overlayShader = std::make_shared<RSNGRenderAIBarRectHalo>();
    EXPECT_NE(overlayShader, nullptr);
    pointLightDrawableTest->stagingOverlayShader_ = overlayShader;
    pointLightDrawableTest->needSync_ = true;
    
    pointLightDrawableTest->OnSync();
    EXPECT_EQ(pointLightDrawableTest->stagingOverlayShader_, overlayShader);
}

/**
 * @tc.name: OnDrawWithOverlayShaderTest001
 * @tc.desc: OnDraw test with overlay shader
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, OnDrawWithOverlayShaderTest001, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> pointLightDrawableTest =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(pointLightDrawableTest, nullptr);
    
    auto overlayShader = std::make_shared<RSNGRenderAIBarRectHalo>();
    EXPECT_NE(overlayShader, nullptr);
    pointLightDrawableTest->stagingOverlayShader_ = overlayShader;
    
    Drawing::Canvas canvasTest;
    Drawing::Rect rect(0, 0, 100, 100);
    pointLightDrawableTest->OnDraw(&canvasTest, &rect);
}

/**
 * @tc.name: OnUpdateWithOverlayShaderAndIlluminatedTest001
 * @tc.desc: OnUpdate test with both overlay shader and illuminated
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, OnUpdateWithOverlayShaderAndIlluminatedTest001, TestSize.Level1)
{
    RSRenderNode renderNodeTest(0);
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> pointLightDrawableTest =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(pointLightDrawableTest, nullptr);
    
    auto overlayShader = std::make_shared<RSNGRenderAIBarRectHalo>();
    EXPECT_NE(overlayShader, nullptr);
    renderNodeTest.GetMutableRenderProperties().SetOverlayNGShader(overlayShader);
    
    std::shared_ptr<RSIlluminated> illuminated = std::make_shared<RSIlluminated>();
    illuminated->illuminatedType_ = IlluminatedType::BORDER_CONTENT;
    illuminated->lightSourcesAndPosMap_.emplace(
        std::make_shared<RSLightSource>(), Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    renderNodeTest.renderProperties_.GetEffect().illuminatedPtr_ = illuminated;
    
    EXPECT_TRUE(pointLightDrawableTest->OnUpdate(renderNodeTest));
}

/**
 * @tc.name: OnGenerateTest001
 * @tc.desc: OnGenerate returns nullptr when illuminatedPtr exists but IsIlluminatedValid is false
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, OnGenerateTest001, TestSize.Level1)
{
    RSRenderNode renderNodeTest(0);
    std::shared_ptr<RSIlluminated> illuminated = std::make_shared<RSIlluminated>();
    illuminated->illuminatedType_ = IlluminatedType::NONE;
    renderNodeTest.renderProperties_.GetEffect().illuminatedPtr_ = illuminated;

    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> drawable =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_EQ(drawable->OnGenerate(renderNodeTest), nullptr);
}

/**
 * @tc.name: OnGenerateTest002
 * @tc.desc: OnGenerate returns nullptr when illuminatedPtr exists but illuminatedType is INVALID
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, OnGenerateTest002, TestSize.Level1)
{
    RSRenderNode renderNodeTest(0);
    std::shared_ptr<RSIlluminated> illuminated = std::make_shared<RSIlluminated>();
    illuminated->illuminatedType_ = IlluminatedType::INVALID;
    renderNodeTest.renderProperties_.GetEffect().illuminatedPtr_ = illuminated;

    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> drawable =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_EQ(drawable->OnGenerate(renderNodeTest), nullptr);
}

/**
 * @tc.name: OnDrawTest001
 * @tc.desc: OnDraw enters the branch where visualEffectContainer_ != nullptr && rect != nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, OnDrawTest001, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> drawable =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(drawable, nullptr);

    auto overlayShader = std::make_shared<RSNGRenderAIBarRectHalo>();
    drawable->stagingOverlayShader_ = overlayShader;
    drawable->needSync_ = true;
    drawable->OnSync();
    EXPECT_NE(drawable->visualEffectContainer_, nullptr);

    Drawing::Canvas canvasTest;
    Drawing::Rect rect(0, 0, 100, 100);
    drawable->OnDraw(&canvasTest, &rect);
}

/**
 * @tc.name: OnUpdateTest003
 * @tc.desc: OnUpdate returns false when illuminatedPtr is nullptr and no overlayShader
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, OnUpdateTest003, TestSize.Level1)
{
    RSRenderNode renderNodeTest(0);
    renderNodeTest.renderProperties_.GetEffect().illuminatedPtr_ = nullptr;

    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> drawable =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(drawable, nullptr);
    EXPECT_FALSE(drawable->OnUpdate(renderNodeTest));
}

/**
 * @tc.name: OnUpdateTest004
 * @tc.desc: OnUpdate returns false when illuminatedPtr exists but IsIlluminatedValid returns false (NONE)
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, OnUpdateTest004, TestSize.Level1)
{
    RSRenderNode renderNodeTest(0);
    std::shared_ptr<RSIlluminated> illuminated = std::make_shared<RSIlluminated>();
    illuminated->illuminatedType_ = IlluminatedType::NONE;
    renderNodeTest.renderProperties_.GetEffect().illuminatedPtr_ = illuminated;

    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> drawable =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(drawable, nullptr);
    EXPECT_FALSE(drawable->OnUpdate(renderNodeTest));
}

/**
 * @tc.name: OnUpdateTest005
 * @tc.desc: OnUpdate returns false when illuminatedPtr exists but IsIlluminatedValid returns false (INVALID)
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, OnUpdateTest005, TestSize.Level1)
{
    RSRenderNode renderNodeTest(0);
    std::shared_ptr<RSIlluminated> illuminated = std::make_shared<RSIlluminated>();
    illuminated->illuminatedType_ = IlluminatedType::INVALID;
    renderNodeTest.renderProperties_.GetEffect().illuminatedPtr_ = illuminated;

    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> drawable =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(drawable, nullptr);
    EXPECT_FALSE(drawable->OnUpdate(renderNodeTest));
}

/**
 * @tc.name: GetSDFContentLightShaderBuilderTest001
 * @tc.desc: GetSDFContentLightShaderBuilder test
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, GetSDFContentLightShaderBuilderTest001, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> drawable =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(drawable, nullptr);
    auto builder = drawable->GetSDFContentLightShaderBuilder();
    EXPECT_NE(builder, nullptr);
    auto cachedBuilder = drawable->GetSDFContentLightShaderBuilder();
    EXPECT_EQ(builder, cachedBuilder);
}

/**
 * @tc.name: GetSDFBorderLightShaderBuilderTest001
 * @tc.desc: GetSDFBorderLightShaderBuilder test
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, GetSDFBorderLightShaderBuilderTest001, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> drawable =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(drawable, nullptr);
    auto builder = drawable->GetSDFBorderLightShaderBuilder();
    EXPECT_NE(builder, nullptr);
    auto cachedBuilder = drawable->GetSDFBorderLightShaderBuilder();
    EXPECT_EQ(builder, cachedBuilder);
}

/**
 * @tc.name: DrawSDFContentLightTest001
 * @tc.desc: DrawSDFContentLight via CONTENT type with sdfShaderEffect_ (else branch)
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, DrawSDFContentLightTest001, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> drawable =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(drawable, nullptr);

    RSLightSource lightSource;
    drawable->illuminatedType_ = IlluminatedType::CONTENT;
    drawable->sdfShaderEffect_ = std::make_shared<Drawing::ShaderEffect>();
    drawable->lightSourcesAndPosVec_.emplace_back(lightSource, Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    drawable->contentRRect_.SetRect(Drawing::RectF(0.f, 0.f, 200.f, 200.f));

    Drawing::Canvas canvasTest;
    drawable->DrawLight(&canvasTest);
}

/**
 * @tc.name: DrawSDFContentLightTest002
 * @tc.desc: DrawSDFContentLight via BLEND_CONTENT type with sdfShaderEffect_ (SaveLayer branch)
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, DrawSDFContentLightTest002, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> drawable =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(drawable, nullptr);

    RSLightSource lightSource;
    drawable->illuminatedType_ = IlluminatedType::BLEND_CONTENT;
    drawable->sdfShaderEffect_ = std::make_shared<Drawing::ShaderEffect>();
    drawable->lightSourcesAndPosVec_.emplace_back(lightSource, Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    drawable->contentRRect_.SetRect(Drawing::RectF(0.f, 0.f, 200.f, 200.f));

    Drawing::Canvas canvasTest;
    drawable->DrawLight(&canvasTest);
}

/**
 * @tc.name: DrawSDFContentLightTest003
 * @tc.desc: DrawSDFContentLight via BLEND_BORDER_CONTENT type with sdfShaderEffect_ (SaveLayer branch)
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, DrawSDFContentLightTest003, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> drawable =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(drawable, nullptr);

    RSLightSource lightSource;
    drawable->illuminatedType_ = IlluminatedType::BLEND_BORDER_CONTENT;
    drawable->sdfShaderEffect_ = std::make_shared<Drawing::ShaderEffect>();
    drawable->lightSourcesAndPosVec_.emplace_back(lightSource, Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    drawable->contentRRect_.SetRect(Drawing::RectF(0.f, 0.f, 200.f, 200.f));

    Drawing::Canvas canvasTest;
    drawable->DrawLight(&canvasTest);
}

/**
 * @tc.name: DrawSDFContentLightTest004
 * @tc.desc: DrawSDFContentLight via BORDER_CONTENT type with sdfShaderEffect_ (else branch)
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, DrawSDFContentLightTest004, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> drawable =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(drawable, nullptr);

    RSLightSource lightSource;
    drawable->illuminatedType_ = IlluminatedType::BORDER_CONTENT;
    drawable->sdfShaderEffect_ = std::make_shared<Drawing::ShaderEffect>();
    drawable->lightSourcesAndPosVec_.emplace_back(lightSource, Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    drawable->contentRRect_.SetRect(Drawing::RectF(0.f, 0.f, 200.f, 200.f));
    drawable->borderRRect_.SetRect(Drawing::RectF(0.f, 0.f, 200.f, 200.f));

    Drawing::Canvas canvasTest;
    drawable->DrawLight(&canvasTest);
}

/**
 * @tc.name: DrawSDFContentLightTest005
 * @tc.desc: DrawSDFContentLight via NORMAL_BORDER_CONTENT type with sdfShaderEffect_
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, DrawSDFContentLightTest005, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> drawable =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(drawable, nullptr);

    RSLightSource lightSource;
    lightSource.SetLightIntensity(2.0f);
    drawable->illuminatedType_ = IlluminatedType::NORMAL_BORDER_CONTENT;
    drawable->sdfShaderEffect_ = std::make_shared<Drawing::ShaderEffect>();
    drawable->lightSourcesAndPosVec_.emplace_back(lightSource, Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    drawable->contentRRect_.SetRect(Drawing::RectF(0.f, 0.f, 200.f, 200.f));
    drawable->borderRRect_.SetRect(Drawing::RectF(0.f, 0.f, 200.f, 200.f));

    Drawing::Canvas canvasTest;
    drawable->DrawLight(&canvasTest);
}

/**
 * @tc.name: DrawSDFBorderLightTest001
 * @tc.desc: DrawSDFBorderLight via BORDER type with sdfShaderEffect_ (else branch)
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, DrawSDFBorderLightTest001, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> drawable =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(drawable, nullptr);

    RSLightSource lightSource;
    drawable->illuminatedType_ = IlluminatedType::BORDER;
    drawable->sdfShaderEffect_ = std::make_shared<Drawing::ShaderEffect>();
    drawable->borderWidth_ = 5.0f;
    drawable->lightSourcesAndPosVec_.emplace_back(lightSource, Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    drawable->contentRRect_.SetRect(Drawing::RectF(0.f, 0.f, 200.f, 200.f));
    drawable->borderRRect_.SetRect(Drawing::RectF(0.f, 0.f, 200.f, 200.f));

    Drawing::Canvas canvasTest;
    drawable->DrawLight(&canvasTest);
}

/**
 * @tc.name: DrawSDFBorderLightTest002
 * @tc.desc: DrawSDFBorderLight via BLEND_BORDER type with sdfShaderEffect_ (SaveLayer branch)
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, DrawSDFBorderLightTest002, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> drawable =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(drawable, nullptr);

    RSLightSource lightSource;
    drawable->illuminatedType_ = IlluminatedType::BLEND_BORDER;
    drawable->sdfShaderEffect_ = std::make_shared<Drawing::ShaderEffect>();
    drawable->borderWidth_ = 5.0f;
    drawable->lightSourcesAndPosVec_.emplace_back(lightSource, Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    drawable->contentRRect_.SetRect(Drawing::RectF(0.f, 0.f, 200.f, 200.f));
    drawable->borderRRect_.SetRect(Drawing::RectF(0.f, 0.f, 200.f, 200.f));

    Drawing::Canvas canvasTest;
    drawable->DrawLight(&canvasTest);
}

/**
 * @tc.name: DrawSDFBorderLightTest003
 * @tc.desc: DrawSDFBorderLight via BLEND_BORDER_CONTENT type with sdfShaderEffect_ (SaveLayer branch)
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, DrawSDFBorderLightTest003, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> drawable =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(drawable, nullptr);

    RSLightSource lightSource;
    drawable->illuminatedType_ = IlluminatedType::BLEND_BORDER_CONTENT;
    drawable->sdfShaderEffect_ = std::make_shared<Drawing::ShaderEffect>();
    drawable->borderWidth_ = 5.0f;
    drawable->lightSourcesAndPosVec_.emplace_back(lightSource, Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    drawable->contentRRect_.SetRect(Drawing::RectF(0.f, 0.f, 200.f, 200.f));
    drawable->borderRRect_.SetRect(Drawing::RectF(0.f, 0.f, 200.f, 200.f));

    Drawing::Canvas canvasTest;
    drawable->DrawLight(&canvasTest);
}

/**
 * @tc.name: DrawSDFBorderLightTest004
 * @tc.desc: DrawSDFBorderLight via NORMAL_BORDER_CONTENT type with sdfShaderEffect_ (else branch)
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, DrawSDFBorderLightTest004, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> drawable =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(drawable, nullptr);

    RSLightSource lightSource;
    lightSource.SetLightIntensity(2.0f);
    drawable->illuminatedType_ = IlluminatedType::NORMAL_BORDER_CONTENT;
    drawable->sdfShaderEffect_ = std::make_shared<Drawing::ShaderEffect>();
    drawable->borderWidth_ = 5.0f;
    drawable->lightSourcesAndPosVec_.emplace_back(lightSource, Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    drawable->contentRRect_.SetRect(Drawing::RectF(0.f, 0.f, 200.f, 200.f));
    drawable->borderRRect_.SetRect(Drawing::RectF(0.f, 0.f, 200.f, 200.f));

    Drawing::Canvas canvasTest;
    drawable->DrawLight(&canvasTest);
}

/**
 * @tc.name: CalcBezierResultYTest001
 * @tc.desc: CalcBezierResultY with discriminant < 0 (ROSEN_LNE branch)
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, CalcBezierResultYTest001, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> drawable =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(drawable, nullptr);

    Vector2f start = {0.0f, 0.0f};
    Vector2f end = {0.0f, 1.0f};
    Vector2f control = {0.5f, 0.5f};
    float input = 0.5f;
    auto result = drawable->CalcBezierResultY(start, end, control, input);
    EXPECT_FALSE(result.has_value());
}

/**
 * @tc.name: CalcBezierResultYTest002
 * @tc.desc: CalcBezierResultY with a==0 and b==0 (ROSEN_EQ(b,0.f) branch)
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, CalcBezierResultYTest002, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> drawable =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(drawable, nullptr);

    Vector2f start = {0.5f, 0.0f};
    Vector2f end = {0.5f, 1.0f};
    Vector2f control = {0.5f, 0.5f};
    float input = 0.5f;
    auto result = drawable->CalcBezierResultY(start, end, control, input);
    EXPECT_FALSE(result.has_value());
}

/**
 * @tc.name: CalcBezierResultYTest003
 * @tc.desc: CalcBezierResultY with a!=0 (else branch, quadratic, t1 in range)
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, CalcBezierResultYTest003, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> drawable =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(drawable, nullptr);

    Vector2f start = {0.0f, 0.0f};
    Vector2f end = {1.0f, 1.0f};
    Vector2f control = {0.3f, 0.5f};
    float input = 0.5f;
    auto result = drawable->CalcBezierResultY(start, end, control, input);
    EXPECT_TRUE(result.has_value());
}

/**
 * @tc.name: CalcBezierResultYTest004
 * @tc.desc: CalcBezierResultY with a!=0, t2 in range
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, CalcBezierResultYTest004, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> drawable =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(drawable, nullptr);

    Vector2f start = { 2.0f, 2.0f };
    Vector2f end = { 0.0f, 0.0f };
    Vector2f control = { 1.5f, 1.0f };
    float input = 1.5f;
    auto result = drawable->CalcBezierResultY(start, end, control, input);
    EXPECT_TRUE(result.has_value());
}

/**
 * @tc.name: DrawSDFContentLightTest006
 * @tc.desc: DrawSDFContentLight with null lightShaderEffect (short-circuit: !sdfLightBuilder false, !lightShaderEffect
 * true)
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, DrawSDFContentLightTest006, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> drawable =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(drawable, nullptr);

    Drawing::Canvas canvasTest;
    Drawing::Brush brush;
    std::shared_ptr<Drawing::ShaderEffect> nullShader = nullptr;
    EXPECT_FALSE(drawable->DrawSDFContentLight(canvasTest, nullShader, brush));
}

/**
 * @tc.name: DrawSDFBorderLightTest005
 * @tc.desc: DrawSDFBorderLight with null lightShaderEffect (short-circuit: !sdfLightBuilder false, !lightShaderEffect
 * true)
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, DrawSDFBorderLightTest005, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> drawable =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(drawable, nullptr);

    Drawing::Canvas canvasTest;
    std::shared_ptr<Drawing::ShaderEffect> nullShader = nullptr;
    EXPECT_FALSE(drawable->DrawSDFBorderLight(canvasTest, nullShader));
}

/**
 * @tc.name: DrawSDFContentLightTest007
 * @tc.desc: DrawSDFContentLight direct call with non-null lightShaderEffect (!sdfLightBuilder false, !lightShaderEffect
 * false)
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, DrawSDFContentLightTest007, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> drawable =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(drawable, nullptr);

    auto validShader = std::make_shared<Drawing::ShaderEffect>();
    drawable->sdfShaderEffect_ = validShader;
    drawable->contentRRect_.SetRect(Drawing::RectF(0.f, 0.f, 200.f, 200.f));
    drawable->illuminatedType_ = IlluminatedType::CONTENT;

    Drawing::Canvas canvasTest;
    Drawing::Brush brush;
    drawable->DrawSDFContentLight(canvasTest, validShader, brush);
}

/**
 * @tc.name: DrawSDFContentLightTest008
 * @tc.desc: DrawSDFContentLight direct call with non-null lightShaderEffect and BLEND_CONTENT type
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, DrawSDFContentLightTest008, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> drawable =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(drawable, nullptr);

    auto validShader = std::make_shared<Drawing::ShaderEffect>();
    drawable->sdfShaderEffect_ = validShader;
    drawable->contentRRect_.SetRect(Drawing::RectF(0.f, 0.f, 200.f, 200.f));
    drawable->illuminatedType_ = IlluminatedType::BLEND_CONTENT;

    Drawing::Canvas canvasTest;
    Drawing::Brush brush;
    drawable->DrawSDFContentLight(canvasTest, validShader, brush);
}

/**
 * @tc.name: DrawSDFBorderLightTest006
 * @tc.desc: DrawSDFBorderLight direct call with non-null lightShaderEffect (!sdfLightBuilder false, !lightShaderEffect
 * false)
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, DrawSDFBorderLightTest006, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> drawable =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(drawable, nullptr);

    auto validShader = std::make_shared<Drawing::ShaderEffect>();
    drawable->sdfShaderEffect_ = validShader;
    drawable->contentRRect_.SetRect(Drawing::RectF(0.f, 0.f, 200.f, 200.f));
    drawable->borderWidth_ = 5.0f;
    drawable->illuminatedType_ = IlluminatedType::BORDER;

    Drawing::Canvas canvasTest;
    drawable->DrawSDFBorderLight(canvasTest, validShader);
}

/**
 * @tc.name: DrawSDFBorderLightTest007
 * @tc.desc: DrawSDFBorderLight direct call with non-null lightShaderEffect and BLEND_BORDER type
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, DrawSDFBorderLightTest007, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> drawable =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(drawable, nullptr);

    auto validShader = std::make_shared<Drawing::ShaderEffect>();
    drawable->sdfShaderEffect_ = validShader;
    drawable->contentRRect_.SetRect(Drawing::RectF(0.f, 0.f, 200.f, 200.f));
    drawable->borderWidth_ = 5.0f;
    drawable->illuminatedType_ = IlluminatedType::BLEND_BORDER;

    Drawing::Canvas canvasTest;
    drawable->DrawSDFBorderLight(canvasTest, validShader);
}
} // namespace OHOS::Rosen