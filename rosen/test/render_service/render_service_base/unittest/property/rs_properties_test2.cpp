/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>

#include "pipeline/rs_context.h"
#include "pipeline/rs_display_render_node.h"
#include "property/rs_properties.h"
#include "common/rs_obj_abs_geometry.h"
#include "property/rs_point_light_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class PropertiesTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void PropertiesTest::SetUpTestCase() {}
void PropertiesTest::TearDownTestCase() {}
void PropertiesTest::SetUp() {}
void PropertiesTest::TearDown() {}

/**
 * @tc.name: SetBgImageInnerRectTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9W24N
 */
HWTEST_F(PropertiesTest, SetBgImageInnerRectTest, TestSize.Level1)
{
    Vector4f rect{1.f, 1.f, 1.f, 1.f};
    RSProperties properties;
    properties.SetBgImageInnerRect(rect);
    EXPECT_EQ(properties.GetBgImageInnerRect(), rect);

    properties.SetBgImageInnerRect(rect);
    EXPECT_EQ(properties.contentDirty_, true);
}

/**
 * @tc.name: SetShadowRadiusTest
 * @tc.desc: test results of SetShadowRadius
 * @tc.type: FUNC
 * @tc.require: issueI9W24N
 */
HWTEST_F(PropertiesTest, SetShadowRadiusTest, TestSize.Level1)
{
    RSProperties properties;
    float radius = 0.1f;
    properties.SetShadowRadius(radius);
    radius = 1.0f;
    properties.SetShadowRadius(radius);

    properties.shadow_->elevation_ = -1.f;
    radius = -1.0f;
    properties.SetShadowRadius(radius);
    EXPECT_FALSE(properties.shadow_->IsValid());
}

/**
 * @tc.name: SetClipBoundsTest
 * @tc.desc: test results of SetClipBounds
 * @tc.type: FUNC
 * @tc.require: issueI9W24N
 */
HWTEST_F(PropertiesTest, SetClipBoundsTest, TestSize.Level1)
{
    RSProperties properties;
    std::shared_ptr<RSPath> path = std::make_shared<RSPath>();
    properties.SetClipBounds(path);
    ASSERT_TRUE(properties.GetClipBounds() == path);
    properties.SetClipBounds(path);
    ASSERT_TRUE(properties.GetClipBounds() == path);

    path = nullptr;
    properties.SetClipBounds(path);
    EXPECT_TRUE(path == nullptr);
}

/**
 * @tc.name: SetClipToBoundsTest
 * @tc.desc: test results of SetClipToBounds
 * @tc.type: FUNC
 * @tc.require: issueI9W24N
 */
HWTEST_F(PropertiesTest, SetClipToBoundsTest, TestSize.Level1)
{
    RSProperties properties;
    properties.SetClipToBounds(true);
    ASSERT_TRUE(properties.GetClipToBounds() == true);
    properties.SetClipToBounds(true);
    ASSERT_TRUE(properties.GetClipToBounds() == true);

    properties.SetClipToBounds(false);
    EXPECT_TRUE(properties.geoDirty_);
}

#if defined(NEW_SKIA) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
/**
 * @tc.name: CreateFilterCacheManagerIfNeedTest
 * @tc.desc: test results of CreateFilterCacheManagerIfNeed
 * @tc.type: FUNC
 * @tc.require: issueI9W24N
 */
HWTEST_F(PropertiesTest, CreateFilterCacheManagerIfNeedTest, TestSize.Level1)
{
    RSProperties properties;
    properties.CreateFilterCacheManagerIfNeed();

    properties.backgroundFilter_ = std::make_shared<RSFilter>();
    properties.CreateFilterCacheManagerIfNeed();

    properties.filter_ = std::make_shared<RSFilter>();
    properties.CreateFilterCacheManagerIfNeed();
    EXPECT_TRUE(properties.filter_ != nullptr);

    properties.CreateFilterCacheManagerIfNeed();
    EXPECT_TRUE(properties.filter_ != nullptr);
}
#endif

/**
 * @tc.name: OnApplyModifiersTest
 * @tc.desc: test results of OnApplyModifiers
 * @tc.type: FUNC
 * @tc.require: issueI9W24N
 */
HWTEST_F(PropertiesTest, OnApplyModifiersTest, TestSize.Level1)
{
    RSProperties properties;
    properties.OnApplyModifiers();
    EXPECT_EQ(properties.GetPixelStretch(), std::nullopt);

    properties.geoDirty_ = true;
    properties.OnApplyModifiers();
    EXPECT_TRUE(properties.geoDirty_);

    properties.hasBounds_ = true;
    properties.OnApplyModifiers();
    EXPECT_TRUE(properties.hasBounds_);

    properties.clipToFrame_ = true;
    properties.OnApplyModifiers();
    EXPECT_TRUE(properties.clipToFrame_);

    properties.clipToBounds_ = true;
    properties.OnApplyModifiers();
    EXPECT_TRUE(properties.clipToBounds_);

    properties.colorFilterNeedUpdate_ = true;
    properties.OnApplyModifiers();
    EXPECT_TRUE(!properties.colorFilterNeedUpdate_);

    properties.pixelStretchNeedUpdate_ = true;
    properties.OnApplyModifiers();
    EXPECT_TRUE(!properties.pixelStretchNeedUpdate_);

    properties.geoDirty_ = false;
    properties.OnApplyModifiers();
    EXPECT_TRUE(!properties.geoDirty_);

    properties.greyCoefNeedUpdate_ = true;
    properties.OnApplyModifiers();
    EXPECT_TRUE(!properties.greyCoefNeedUpdate_);

    properties.shadow_ = std::make_optional<RSShadow>();
    properties.shadow_->colorStrategy_ = SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_MAIN;
    properties.backgroundFilter_ = std::make_shared<RSFilter>();
    properties.filter_ = std::make_shared<RSFilter>();
    properties.foregroundEffectRadius_ = 1.f;
    Vector2f scaleAnchor = Vector2f(0.f, 0.f);
    properties.motionBlurPara_ = std::make_shared<MotionBlurParam>(1.f, scaleAnchor);
    properties.OnApplyModifiers();
    EXPECT_TRUE(properties.filter_ != nullptr);

    properties.geoDirty_ = true;
    properties.frameGeo_.SetX(INFINITY);
    properties.boundsGeo_->SetX(-INFINITY);
    properties.OnApplyModifiers();
    EXPECT_TRUE(properties.clipToFrame_);

    properties.frameGeo_.SetY(INFINITY);
    properties.boundsGeo_->SetY(-INFINITY);
    properties.OnApplyModifiers();
    EXPECT_TRUE(!properties.clipToFrame_);
}

/**
 * @tc.name: UpdateFilterTest
 * @tc.desc: test results of UpdateFilter
 * @tc.type: FUNC
 * @tc.require: issueI9W24N
 */
HWTEST_F(PropertiesTest, UpdateFilterTest, TestSize.Level1)
{
    RSProperties properties;
    properties.shadow_ = std::make_optional<RSShadow>();
    properties.shadow_->colorStrategy_ = SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_AVERAGE;
    properties.UpdateFilter();
    EXPECT_TRUE(properties.filterNeedUpdate_);

    properties.foregroundEffectRadius_ = 0.1f;
    properties.UpdateFilter();
    EXPECT_FALSE(properties.foregroundFilter_);

    properties.foregroundEffectRadius_ = -0.1f;
    properties.isSpherizeValid_ = true;
    properties.UpdateFilter();
    EXPECT_TRUE(properties.foregroundFilter_);

    properties.isSpherizeValid_ = false;
    properties.shadow_->imageMask_ = true;
    properties.UpdateFilter();
    EXPECT_TRUE(properties.foregroundFilter_);

    properties.foregroundEffectRadius_ = -0.1f;
    properties.isAttractionValid_ = true;
    properties.UpdateFilter();
    EXPECT_TRUE(properties.foregroundFilter_);

    properties.isAttractionValid_ = false;
    properties.shadow_->imageMask_ = true;
    properties.UpdateFilter();
    EXPECT_TRUE(properties.foregroundFilter_);

    properties.shadow_->imageMask_ = false;
    properties.UpdateFilter();
    EXPECT_TRUE(!properties.foregroundFilter_);

    Vector2f scaleAnchor = Vector2f(0.f, 0.f);
    properties.motionBlurPara_ = std::make_shared<MotionBlurParam>(1.f, scaleAnchor);
    properties.UpdateFilter();
    EXPECT_TRUE(properties.foregroundFilter_);

    uint32_t flyMode = 0;
    RSFlyOutPara rs_fly_out_param = {
        flyMode
    };
    properties.flyOutParams_ = std::optional<RSFlyOutPara>(rs_fly_out_param);
    properties.foregroundEffectRadius_ = -0.1f;
    properties.flyOutDegree_ = 0.5;
    properties.UpdateFilter();
    EXPECT_TRUE(properties.foregroundFilter_);

    properties.flyOutDegree_ = 0.5;
    properties.shadow_->imageMask_ = true;
    properties.UpdateFilter();
    EXPECT_TRUE(properties.foregroundFilter_);

    properties.distortionK_ = 0.7;
    properties.shadow_->imageMask_ = true;
    properties.UpdateFilter();
    EXPECT_TRUE(properties.foregroundFilter_);
}

/**
 * @tc.name: SetParticlesTest
 * @tc.desc: test results of SetParticles
 * @tc.type: FUNC
 * @tc.require: issueI9W24N
 */
HWTEST_F(PropertiesTest, SetParticlesTest, TestSize.Level1)
{
    RSProperties properties;
    RSRenderParticleVector particles;
    properties.SetParticles(particles);
    EXPECT_EQ(properties.particles_.renderParticleVector_.size(), 0);

    auto particleParams = std::make_shared<ParticleRenderParams>();
    std::shared_ptr<RSRenderParticle> newv = std::make_shared<RSRenderParticle>(particleParams);
    particles.renderParticleVector_.push_back(newv);
    properties.SetParticles(particles);
    EXPECT_EQ(properties.particles_.GetParticleSize(), 1);
}

/**
 * @tc.name: SetForegroundColorTest
 * @tc.desc: test results of SetForegroundColor
 * @tc.type: FUNC
 * @tc.require: issueI9W24N
 */
HWTEST_F(PropertiesTest, SetForegroundColorTest, TestSize.Level1)
{
    RSProperties properties;
    Color color;
    properties.SetForegroundColor(color);
    EXPECT_EQ(properties.contentDirty_, true);

    properties.SetForegroundColor(color);
    EXPECT_EQ(properties.contentDirty_, true);
}

/**
 * @tc.name: SetBorderColorTest
 * @tc.desc: test results of SetBorderColor
 * @tc.type: FUNC
 * @tc.require: issueI9W24N
 */
HWTEST_F(PropertiesTest, SetBorderColorTest, TestSize.Level1)
{
    RSProperties properties;
    Color color(0, 0, 0, 0);
    Vector4<Color> borderColor = {color, color, color, color};
    properties.SetBorderColor(borderColor);
    EXPECT_TRUE(properties.contentDirty_);
}

/**
 * @tc.name: SetForegroundFilterCacheTest
 * @tc.desc: test results of SetForegroundFilterCache
 * @tc.type: FUNC
 * @tc.require: issueI9W24N
 */
HWTEST_F(PropertiesTest, SetForegroundFilterCacheTest, TestSize.Level1)
{
    RSProperties properties;
    std::shared_ptr<RSFilter> foregroundFilterCache;
    properties.SetForegroundFilterCache(foregroundFilterCache);
    EXPECT_TRUE(foregroundFilterCache == nullptr);

    foregroundFilterCache = std::make_shared<RSFilter>();
    properties.SetForegroundFilterCache(foregroundFilterCache);
    EXPECT_TRUE(properties.isDrawn_);
}

/**
 * @tc.name: GetFgBrightnessDescriptionTest
 * @tc.desc: test results of GetFgBrightnessDescription
 * @tc.type: FUNC
 * @tc.require: issueI9W24N
 */
HWTEST_F(PropertiesTest, GetFgBrightnessDescriptionTest, TestSize.Level1)
{
    RSProperties properties;
    properties.fgBrightnessParams_ = std::nullopt;
    properties.GetFgBrightnessDescription();

    RSDynamicBrightnessPara value;
    properties.fgBrightnessParams_ = value;
    properties.GetFgBrightnessDescription();
    EXPECT_TRUE(properties.fgBrightnessParams_ != std::nullopt);
}

/**
 * @tc.name: GetBgBrightnessDescriptionTest
 * @tc.desc: test results of GetBgBrightnessDescription
 * @tc.type: FUNC
 * @tc.require: issueI9W24N
 */
HWTEST_F(PropertiesTest, GetBgBrightnessDescriptionTest, TestSize.Level1)
{
    RSProperties properties;
    properties.bgBrightnessParams_ = std::nullopt;
    properties.GetBgBrightnessDescription();

    RSDynamicBrightnessPara value;
    properties.bgBrightnessParams_ = value;
    properties.GetBgBrightnessDescription();
    EXPECT_TRUE(properties.bgBrightnessParams_ != std::nullopt);
}

/**
 * @tc.name: SetGetTest
 * @tc.desc: test results of SetGet
 * @tc.type: FUNC
 * @tc.require: issueI9W24N
 */
HWTEST_F(PropertiesTest, SetGetTest, TestSize.Level1)
{
    RSProperties properties;
    Color color(1, 1, 1);
    Vector4<Color> outLineColor = { color, color, color, color };
    properties.SetOutlineColor(outLineColor);
    EXPECT_EQ(properties.GetOutlineColor(), outLineColor);

    Color colorNew(0, 0, 0);
    Vector4<Color> outLineColorNew = { colorNew, colorNew, colorNew, colorNew };
    properties.SetOutlineColor(outLineColorNew);
    EXPECT_EQ(properties.GetOutlineColor(), outLineColorNew);

    Vector4f zeroWidth = { 0, 0, 0, 0 };
    Vector4f width = { 1.0, 1.0, 1.0, 1.0 };
    properties.SetOutlineWidth(zeroWidth);
    properties.SetOutlineWidth(width);

    Vector4<uint32_t> style = { 1, 1, 1, 1 };
    properties.SetOutlineStyle(style);
    properties.GetOutlineStyle();

    Vector4f radius = { 1.0, 1.0, 1.0, 1.0 };
    properties.SetOutlineRadius(radius);
    properties.GetOutlineRadius();

    EXPECT_NE(nullptr, properties.GetOutline());
}

/**
 * @tc.name: SetHDRBrightnessFactor001
 * @tc.desc: test results of SetHDRBrightnessFactor
 * @tc.type: FUNC
 * @tc.require: issueI9W24N
 */
HWTEST_F(PropertiesTest, SetHDRBrightnessFactor001, TestSize.Level1)
{
    RSProperties properties;
    float initialFactor = 1.0f;
    properties.SetHDRBrightnessFactor(initialFactor);
    properties.SetHDRBrightnessFactor(initialFactor);
    EXPECT_EQ(properties.GetHDRBrightnessFactor(), initialFactor);
}

/**
 * @tc.name: SetHDRBrightnessFactor002
 * @tc.desc: test results of SetHDRBrightnessFactor
 * @tc.type: FUNC
 * @tc.require: issueI9W24N
 */
HWTEST_F(PropertiesTest, SetHDRBrightnessFactor002, TestSize.Level1)
{
    RSProperties properties;
    float initialFactor = 1.0f;
    properties.SetHDRBrightnessFactor(initialFactor);
    float newFactor = 0.5f;
    properties.SetHDRBrightnessFactor(newFactor);
    EXPECT_EQ(properties.GetHDRBrightnessFactor(), newFactor);
}

/**
 * @tc.name: SetHDRBrightnessFactor003
 * @tc.desc: test results of SetHDRBrightnessFactor
 * @tc.type: FUNC
 * @tc.require: issueI9W24N
 */
HWTEST_F(PropertiesTest, SetHDRBrightnessFactor003, TestSize.Level1)
{
    RSProperties properties;
    float initialFactor = 1.0f;

    std::shared_ptr<RSRenderNode> node = std::make_shared<RSRenderNode>(1);
    properties.backref_ = node;
    properties.SetHDRBrightnessFactor(initialFactor);

    NodeId displayRenderNodeId = 2;
    struct RSDisplayNodeConfig config;
    auto context = std::make_shared<RSContext>();
    auto displayRenderNode = std::make_shared<RSDisplayRenderNode>(displayRenderNodeId, config, context);

    properties.backref_ = displayRenderNode;
    displayRenderNode->InsertHDRNode(displayRenderNodeId);
    EXPECT_NE(displayRenderNode->hdrNodeList_.find(displayRenderNodeId), displayRenderNode->hdrNodeList_.end());
    properties.SetHDRBrightnessFactor(0.5f);

    NodeId nodeId1 = 0;
    auto node1 = std::make_shared<RSRenderNode>(nodeId1);
    pid_t pid1 = ExtractPid(nodeId1);
    context->GetMutableNodeMap().renderNodeMap_[pid1][nodeId1] = node1;
    displayRenderNode->InsertHDRNode(nodeId1);
    properties.SetHDRBrightnessFactor(0.6f);

    pid_t pid = ExtractPid(displayRenderNodeId);
    context->GetMutableNodeMap().renderNodeMap_[pid][displayRenderNodeId] = displayRenderNode;
    properties.SetHDRBrightnessFactor(0.8f);

    struct RSDisplayNodeConfig config2;
    std::shared_ptr<RSContext> context2;
    auto displayNode2 = std::make_shared<RSDisplayRenderNode>(3, config2, context2);
    properties.backref_ = displayNode2;
    displayRenderNode->InsertHDRNode(3);
    EXPECT_NE(displayRenderNode->hdrNodeList_.find(3), displayRenderNode->hdrNodeList_.end());
    properties.SetHDRBrightnessFactor(0.9f);
}

/**
 * @tc.name: SetCanvasNodeHDRBrightnessFactor001
 * @tc.desc: test results of SetCanvasNodeHDRBrightnessFactor
 * @tc.type: FUNC
 * @tc.require: issueI9W24N
 */
HWTEST_F(PropertiesTest, SetCanvasNodeHDRBrightnessFactor001, TestSize.Level1)
{
    RSProperties properties;
    float initialFactor = 1.0f;
    properties.SetCanvasNodeHDRBrightnessFactor(initialFactor);
    properties.SetCanvasNodeHDRBrightnessFactor(initialFactor);
    EXPECT_EQ(properties.GetCanvasNodeHDRBrightnessFactor(), initialFactor);
}

/**
 * @tc.name: SetCanvasNodeHDRBrightnessFactor002
 * @tc.desc: test results of SetCanvasNodeHDRBrightnessFactor
 * @tc.type: FUNC
 * @tc.require: issueI9W24N
 */
HWTEST_F(PropertiesTest, SetCanvasNodeHDRBrightnessFactor002, TestSize.Level1)
{
    RSProperties properties;
    float initialFactor = 1.0f;
    float newFactor = 0.5f;
    properties.SetCanvasNodeHDRBrightnessFactor(initialFactor);
    properties.SetCanvasNodeHDRBrightnessFactor(newFactor);
    EXPECT_EQ(properties.GetCanvasNodeHDRBrightnessFactor(), newFactor);
}

/**
 * @tc.name: SetEmitterUpdaterTest
 * @tc.desc: test results of SetEmitterUpdater
 * @tc.type: FUNC
 * @tc.require: issueI9W24N
 */
HWTEST_F(PropertiesTest, SetEmitterUpdaterTest, TestSize.Level1)
{
    RSProperties properties;
    std::vector<std::shared_ptr<EmitterUpdater>> para;
    properties.SetEmitterUpdater(para);
    EXPECT_EQ(properties.emitterUpdater_.empty(), true);

    auto emitter = std::make_shared<EmitterUpdater>(0);
    para.push_back(emitter);
    properties.SetEmitterUpdater(para);
    EXPECT_EQ(properties.emitterUpdater_.empty(), false);

    std::shared_ptr<RSRenderNode> node = std::make_shared<RSRenderNode>(1);
    properties.backref_ = node;
    properties.SetEmitterUpdater(para);
    EXPECT_EQ(properties.emitterUpdater_.empty(), false);

    auto renderNode = properties.backref_.lock();
    PropertyId propertyId = 0;
    AnimationId animationId = 0;
    renderNode->animationManager_.particleAnimations_.insert({ propertyId, animationId });
    auto renderAnimation = std::make_shared<RSRenderAnimation>();
    renderNode->animationManager_.animations_.insert({ animationId, renderAnimation });
    properties.SetEmitterUpdater(para);
    EXPECT_EQ(renderNode->animationManager_.animations_.empty(), false);
}

/**
 * @tc.name: SetLinearGradientBlurParaTest
 * @tc.desc: test results of SetLinearGradientBlurPara
 * @tc.type: FUNC
 * @tc.require: issueI9W24N
 */
HWTEST_F(PropertiesTest, SetLinearGradientBlurParaTest, TestSize.Level1)
{
    RSProperties properties;
    float blurRadius = 1.f;
    std::vector<std::pair<float, float>>fractionStops;
    GradientDirection direction = GradientDirection::LEFT;
    auto para = std::make_shared<RSLinearGradientBlurPara>(blurRadius, fractionStops, direction);
    properties.SetLinearGradientBlurPara(para);
    EXPECT_NE(properties.linearGradientBlurPara_, nullptr);
    EXPECT_EQ(properties.GetLinearGradientBlurPara(), para);
    properties.IfLinearGradientBlurInvalid();

    para->blurRadius_ = 0.f;
    properties.SetLinearGradientBlurPara(para);
    EXPECT_NE(properties.linearGradientBlurPara_, nullptr);

    para = nullptr;
    properties.SetLinearGradientBlurPara(para);
    EXPECT_EQ(properties.linearGradientBlurPara_, nullptr);
}

/**
 * @tc.name: SetDynamicLightUpRateTest
 * @tc.desc: test results of SetDynamicLightUpRate
 * @tc.type: FUNC
 * @tc.require: issueI9W24N
 */
HWTEST_F(PropertiesTest, SetDynamicLightUpRateTest, TestSize.Level1)
{
    RSProperties properties;
    std::optional<float> rate;
    properties.SetDynamicLightUpRate(rate);
    EXPECT_TRUE(!rate.has_value());

    rate = std::optional<float>(1.f);
    properties.SetDynamicLightUpRate(rate);
    EXPECT_EQ(properties.filterNeedUpdate_, true);
    ASSERT_TRUE(properties.GetDynamicLightUpRate().has_value());
    EXPECT_EQ(properties.GetDynamicLightUpRate().value(), rate.value());
    EXPECT_FALSE(properties.IsDynamicLightUpValid());
}

/**
 * @tc.name: SetDynamicLightUpDegreeTest
 * @tc.desc: test results of SetDynamicLightUpDegree
 * @tc.type: FUNC
 * @tc.require: issueI9W24N
 */
HWTEST_F(PropertiesTest, SetDynamicLightUpDegreeTest, TestSize.Level1)
{
    RSProperties properties;
    std::optional<float> lightUpDegree;
    properties.SetDynamicLightUpRate(lightUpDegree);
    EXPECT_TRUE(!lightUpDegree.has_value());

    lightUpDegree = std::optional<float>(1.f);
    properties.SetDynamicLightUpDegree(lightUpDegree);
    EXPECT_EQ(properties.filterNeedUpdate_, true);
    ASSERT_TRUE(properties.GetDynamicLightUpDegree().has_value());
    EXPECT_EQ(properties.GetDynamicLightUpDegree().value(), lightUpDegree.value());
}

/**
 * @tc.name: SetNGetDynamicDimDegreeTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9W24N
 */
HWTEST_F(PropertiesTest, SetNGetDynamicDimDegreeTest, TestSize.Level1)
{
    RSProperties properties;
    std::optional<float> dimDegree(0.99f);
    properties.SetDynamicDimDegree(dimDegree);
    ASSERT_TRUE(properties.GetDynamicDimDegree().has_value());
    EXPECT_EQ(properties.GetDynamicDimDegree().value(), dimDegree.value());
    EXPECT_TRUE(properties.IsDynamicDimValid());
    dimDegree = 1.f;
    properties.SetDynamicDimDegree(dimDegree);
    ASSERT_TRUE(properties.GetDynamicDimDegree().has_value());
    EXPECT_EQ(properties.GetDynamicDimDegree().value(), dimDegree.value());
    EXPECT_FALSE(properties.IsDynamicDimValid());

    std::optional<float> degree;
    properties.SetDynamicDimDegree(degree);
    EXPECT_TRUE(!degree.has_value());
}

/**
 * @tc.name: SetFilterTest
 * @tc.desc: test results of SetFilter
 * @tc.type: FUNC
 * @tc.require: issueI9W24N
 */
HWTEST_F(PropertiesTest, SetFilterTest, TestSize.Level1)
{
    RSProperties properties;
    std::shared_ptr<RSFilter> filter = std::make_shared<RSFilter>();
    properties.SetFilter(filter);
    EXPECT_NE(properties.filter_, nullptr);
    EXPECT_EQ(properties.GetFilter(), filter);

    filter = nullptr;
    properties.SetFilter(filter);
    EXPECT_EQ(properties.filter_, nullptr);
}

/**
 * @tc.name: SetShadowIsFilledTest
 * @tc.desc: test results of SetShadowIsFilled
 * @tc.type: FUNC
 * @tc.require: issueI9W24N
 */
HWTEST_F(PropertiesTest, SetShadowIsFilledTest, TestSize.Level1)
{
    RSProperties properties;
    properties.SetShadowIsFilled(true);
    EXPECT_EQ(properties.contentDirty_, true);

    properties.SetShadowIsFilled(true);
    EXPECT_EQ(properties.contentDirty_, true);
    EXPECT_EQ(properties.filterNeedUpdate_, false);
    RSShadow shadow;
    shadow.SetMask(true);
    properties.shadow_ = shadow;
    properties.SetShadowIsFilled(true);
    EXPECT_EQ(properties.filterNeedUpdate_, true);
}

/**
 * @tc.name: SetClipRRectTest
 * @tc.desc: test results of SetClipRRect
 * @tc.type: FUNC
 * @tc.require: issueI9W24N
 */
HWTEST_F(PropertiesTest, SetClipRRectTest, TestSize.Level1)
{
    RSProperties properties;
    RectT<float> rect(1.f, 1.f, 1.f, 1.f);
    RRect clipRRect(rect, 1.f, 1.f);
    properties.SetClipRRect(clipRRect);
    EXPECT_EQ(properties.geoDirty_, true);
    EXPECT_EQ(properties.GetClipRRect(), clipRRect);
    EXPECT_TRUE(properties.GetClipToRRect());

    RRect clipRRectNew;
    properties.SetClipRRect(clipRRectNew);
    EXPECT_TRUE(properties.clipRRect_.has_value());
}

/**
 * @tc.name: SetClipToFrameTest
 * @tc.desc: test results of SetClipToFrame
 * @tc.type: FUNC
 * @tc.require: issueI9W24N
 */
HWTEST_F(PropertiesTest, SetClipToFrameTest, TestSize.Level1)
{
    RSProperties properties;
    properties.SetClipToFrame(true);
    EXPECT_EQ(properties.isDrawn_, true);
    EXPECT_TRUE(properties.GetClipToFrame() == true);
    properties.SetClipToFrame(true);
    EXPECT_TRUE(properties.GetClipToFrame() == true);

    properties.SetClipToFrame(false);
    EXPECT_TRUE(!properties.clipToFrame_);
}

/**
 * @tc.name: GenerateBackgroundMaterialBlurFilterTest
 * @tc.desc: test results of GenerateBackgroundMaterialBlurFilter
 * @tc.type: FUNC
 * @tc.require: issueI9W24N
 */
HWTEST_F(PropertiesTest, GenerateBackgroundMaterialBlurFilterTest, TestSize.Level1)
{
    RSProperties properties;
    Vector2f vectorValue = { 1.f, 1.f };
    properties.GenerateBackgroundMaterialBlurFilter();
    EXPECT_EQ(vectorValue.x_, 1.f);

    properties.greyCoef_ = vectorValue;
    properties.GenerateBackgroundMaterialBlurFilter();
    EXPECT_EQ(vectorValue.x_, 1.f);

    properties.backgroundColorMode_ = BLUR_COLOR_MODE::AVERAGE;
    properties.GenerateBackgroundMaterialBlurFilter();
    EXPECT_EQ(vectorValue.x_, 1.f);

    properties.backgroundColorMode_ = BLUR_COLOR_MODE::FASTAVERAGE;
    properties.GenerateBackgroundMaterialBlurFilter();
    EXPECT_EQ(vectorValue.x_, 1.f);
}

/**
 * @tc.name: GenerateForegroundFilterTest
 * @tc.desc: test results of GenerateForegroundFilter
 * @tc.type: FUNC
 * @tc.require: issueI9W24N
 */
HWTEST_F(PropertiesTest, GenerateForegroundFilterTest, TestSize.Level1)
{
    RSProperties properties;
    properties.GenerateForegroundFilter();
    EXPECT_EQ(properties.backgroundBlurSaturation_, 1.f);

    properties.foregroundBlurRadiusX_ = 2.f;
    properties.GenerateForegroundFilter();
    EXPECT_TRUE(properties.IsForegroundBlurRadiusXValid());

    properties.foregroundBlurRadiusY_ = 2.f;
    properties.GenerateForegroundFilter();
    EXPECT_TRUE(properties.IsForegroundBlurRadiusYValid());

    properties.foregroundBlurRadius_ = 2.f;
    properties.GenerateForegroundFilter();
    EXPECT_TRUE(properties.IsForegroundMaterialFilterVaild());

    std::vector<std::pair<float, float>> fractionStops;
    GradientDirection direction;
    properties.linearGradientBlurPara_ = std::make_shared<RSLinearGradientBlurPara>(-1.f, fractionStops, direction);
    properties.linearGradientBlurPara_->blurRadius_ = 1.f;
    properties.GenerateForegroundFilter();
    EXPECT_TRUE(properties.linearGradientBlurPara_);
}

/**
 * @tc.name: CheckEmptyBoundsTest
 * @tc.desc: test results of CheckEmptyBounds
 * @tc.type: FUNC
 * @tc.require: issueI9W24N
 */
HWTEST_F(PropertiesTest, CheckEmptyBoundsTest, TestSize.Level1)
{
    RSProperties properties;
    properties.CheckEmptyBounds();
    EXPECT_EQ(properties.hasBounds_, false);

    properties.hasBounds_ = true;
    properties.CheckEmptyBounds();
}

/**
 * @tc.name: SetMaskTest
 * @tc.desc: test results of SetMask
 * @tc.type: FUNC
 * @tc.require: issueI9W24N
 */
HWTEST_F(PropertiesTest, SetMaskTest, TestSize.Level1)
{
    RSProperties properties;
    std::shared_ptr<RSMask> mask = std::make_shared<RSMask>();
    properties.SetMask(mask);
    EXPECT_EQ(properties.GetMask(), mask);
    EXPECT_EQ(properties.contentDirty_, true);

    mask = nullptr;
    properties.SetMask(mask);
    EXPECT_EQ(properties.GetMask(), nullptr);
}

/**
 * @tc.name: SetNGetSpherizeTest
 * @tc.desc: test results of SetSpherize
 * @tc.type: FUNC
 * @tc.require: issueI9W24N
 */
HWTEST_F(PropertiesTest, SetNGetSpherizeTest, TestSize.Level1)
{
    RSProperties properties;
    float spherizeDegree{1.f};
    properties.SetSpherize(spherizeDegree);
    EXPECT_EQ(properties.GetSpherize(), spherizeDegree);

    spherizeDegree = 0.001f;
    properties.SetSpherize(spherizeDegree);
    EXPECT_EQ(properties.GetSpherize(), spherizeDegree);
}

/**
 * @tc.name: CalculateFrameOffsetTest
 * @tc.desc: test results of CalculateFrameOffset
 * @tc.type: FUNC
 * @tc.require: issueI9W24N
 */
HWTEST_F(PropertiesTest, CalculateFrameOffsetTest, TestSize.Level1)
{
    RSProperties properties;
    properties.frameGeo_.SetX(1.f);
    properties.frameGeo_.SetY(1.f);
    properties.boundsGeo_->SetX(1.f);
    properties.boundsGeo_->SetY(1.f);
    properties.CalculateFrameOffset();
    EXPECT_NE(properties.isDrawn_, true);

    properties.frameGeo_.SetX(-INFINITY);
    properties.boundsGeo_->SetX(-INFINITY);
    properties.CalculateFrameOffset();
    EXPECT_EQ(properties.isDrawn_, true);

    properties.frameGeo_.SetY(-INFINITY);
    properties.boundsGeo_->SetY(-INFINITY);
    properties.CalculateFrameOffset();
    EXPECT_EQ(properties.isDrawn_, true);

    properties.frameGeo_.SetX(0);
    properties.frameGeo_.SetY(0);
    properties.CalculateFrameOffset();
    EXPECT_EQ(properties.isDrawn_, true);
}

/**
 * @tc.name: SetHaveEffectRegionTest
 * @tc.desc: test results of SetHaveEffectRegion
 * @tc.type: FUNC
 * @tc.require: issueI9W24N
 */
HWTEST_F(PropertiesTest, SetHaveEffectRegionTest, TestSize.Level1)
{
    RSProperties properties;
    properties.SetHaveEffectRegion(false);
    EXPECT_NE(properties.isDrawn_, true);

    properties.backgroundFilterCacheManager_ = std::make_unique<RSFilterCacheManager>();
    properties.backgroundFilterCacheManager_->cachedSnapshot_ =
        std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    properties.backgroundFilterCacheManager_->cachedFilteredSnapshot_ =
        std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    properties.SetHaveEffectRegion(false);
    EXPECT_NE(properties.isDrawn_, true);
}

/**
 * @tc.name: CheckGreyCoefTest
 * @tc.desc: test results of CheckGreyCoef
 * @tc.type: FUNC
 * @tc.require: issueI9W24N
 */
HWTEST_F(PropertiesTest, CheckGreyCoefTest, TestSize.Level1)
{
    RSProperties properties;
    properties.greyCoef_ = Vector2f(1.0f, 2.0f);
    properties.CheckGreyCoef();
    EXPECT_NE(properties.greyCoef_, std::nullopt);

    properties.greyCoef_->y_ = 128.f;
    properties.CheckGreyCoef();
    EXPECT_EQ(properties.greyCoef_, std::nullopt);

    properties.greyCoef_->y_ = -0.1f;
    properties.CheckGreyCoef();
    EXPECT_EQ(properties.greyCoef_, std::nullopt);

    properties.greyCoef_->x_ = 128.f;
    properties.CheckGreyCoef();
    EXPECT_EQ(properties.greyCoef_, std::nullopt);

    properties.greyCoef_->x_ = -0.1f;
    properties.CheckGreyCoef();
    EXPECT_EQ(properties.greyCoef_, std::nullopt);
}

/**
 * @tc.name: SetFlyOutDegreeTest
 * @tc.desc: test results of SetFlyOutDegree
 * @tc.type: FUNC
 * @tc.require: issueIAH2TY
 */
HWTEST_F(PropertiesTest, SetFlyOutDegreeTest, TestSize.Level1)
{
    RSProperties properties;
    properties.SetFlyOutDegree(0.7f);
    float degree = properties.GetFlyOutDegree();
    EXPECT_FLOAT_EQ(degree, 0.7f);
}

/**
 * @tc.name: SetFlyOutParamsTest
 * @tc.desc: test results of SetFlyOutParams
 * @tc.type: FUNC
 * @tc.require: issueIAH2TY
 */
HWTEST_F(PropertiesTest, SetFlyOutParamsTest, TestSize.Level1)
{
    RSProperties properties;
    std::optional<RSFlyOutPara> paramsNull = std::nullopt;
    properties.SetFlyOutParams(paramsNull);
    ASSERT_TRUE(properties.isDrawn_ == false);

    uint32_t flyMode = 0;
    RSFlyOutPara rs_fly_out_param = {
        flyMode
    };
    auto params = std::optional<RSFlyOutPara>(rs_fly_out_param);
    properties.SetFlyOutParams(params);
    ASSERT_TRUE(properties.isDrawn_ == true);
    ASSERT_TRUE(properties.GetFlyOutParams().has_value());
}

/**
 * @tc.name: IsFlyOutValidTest
 * @tc.desc: test results of IsFlyOutValid
 * @tc.type: FUNC
 * @tc.require: issueIAH2TY
 */
HWTEST_F(PropertiesTest, IsFlyOutValidTest, TestSize.Level1)
{
    RSProperties properties;
    // if degree > 1
    properties.SetFlyOutDegree(2.0f);
    ASSERT_FALSE(properties.IsFlyOutValid());
    // if degree < 0
    properties.SetFlyOutDegree(-5.0f);
    ASSERT_FALSE(properties.IsFlyOutValid());
    // if no flyOutParams
    properties.SetFlyOutDegree(1.0f);
    ASSERT_FALSE(properties.IsFlyOutValid());
    uint32_t flyMode = 0;
    RSFlyOutPara rs_fly_out_param = {
        flyMode
    };
    auto params = std::optional<RSFlyOutPara>(rs_fly_out_param);
    properties.SetFlyOutParams(params);
    ASSERT_TRUE(properties.IsFlyOutValid());
}

/**
 * @tc.name: CreateFlyOutShaderFilterTest
 * @tc.desc: test results of CreateFlyOutShaderFilter
 * @tc.type: FUNC
 * @tc.require: issueIAH2TY
 */
HWTEST_F(PropertiesTest, CreateFlyOutShaderFilterTest, TestSize.Level1)
{
    RSProperties properties;
    properties.SetFlyOutDegree(0.4f);
    uint32_t flyMode = 1;
    RSFlyOutPara rs_fly_out_param = {
        flyMode
    };
    auto params = std::optional<RSFlyOutPara>(rs_fly_out_param);
    properties.SetFlyOutParams(params);
    if (properties.IsFlyOutValid()) {
        properties.CreateFlyOutShaderFilter();
    }
    EXPECT_FALSE(properties.GetForegroundFilter() == nullptr);
}
/**
 * @tc.name: SetDistortionKTest
 * @tc.desc: test results of SetDistortionK
 * @tc.type: FUNC
 * @tc.require: issueIAS8IM
 */
HWTEST_F(PropertiesTest, SetDistortionKTest, TestSize.Level1)
{
    RSProperties properties;
    // if distortionK_ has no value
    properties.SetDistortionK(std::nullopt);
    auto distortionK = properties.GetDistortionK();
    EXPECT_TRUE(distortionK == std::nullopt);

    // if distortionK_ has value
    properties.SetDistortionK(0.7f);
    distortionK = properties.GetDistortionK();
    EXPECT_FLOAT_EQ(*distortionK, 0.7f);
}

/**
 * @tc.name: IsDistortionKValidTest
 * @tc.desc: test results of IsDistortionKValid
 * @tc.type: FUNC
 * @tc.require: issueIAS8IM
 */
HWTEST_F(PropertiesTest, IsDistortionKValidTest, TestSize.Level1)
{
    RSProperties properties;
    // if distortionK_ has no value
    ASSERT_FALSE(properties.IsDistortionKValid());

    // if distortionK_ > 1
    properties.SetDistortionK(1.7f);
    ASSERT_FALSE(properties.IsDistortionKValid());

    // if distortionK_ < -1
    properties.SetDistortionK(-1.7f);
    ASSERT_FALSE(properties.IsDistortionKValid());

    properties.SetDistortionK(0.7f);
    ASSERT_TRUE(properties.IsDistortionKValid());
}

/**
 * @tc.name: GetDistortionDirtyTest
 * @tc.desc: test results of GetDistortionDirty
 * @tc.type: FUNC
 * @tc.require: issueIAS8IM
 */
HWTEST_F(PropertiesTest, GetDistortionDirtyTest, TestSize.Level1)
{
    RSProperties properties;
    // if distortionK_ has no value
    ASSERT_FALSE(properties.GetDistortionDirty());

    // if distortionK_ > 1
    properties.SetDistortionK(1.7f);
    ASSERT_FALSE(properties.GetDistortionDirty());

    // if distortionK_ < 0
    properties.SetDistortionK(-0.1f);
    ASSERT_FALSE(properties.GetDistortionDirty());

    // if distortionK_ > 0 and < 1
    properties.SetDistortionK(0.7f);
    ASSERT_TRUE(properties.GetDistortionDirty());
}

/**
 * @tc.name: TransformFactor
 * @tc.desc: test results of transform data: skew, perspect, scale, translate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PropertiesTest, TransformFactor, TestSize.Level1)
{
    RSProperties properties;
    properties.SetScaleZ(1.0f);
    EXPECT_EQ(properties.GetScaleZ(), 1.0f);

    properties.SetPersp(Vector4f(1.0, 1.0, 1.0, 1.0));
    EXPECT_EQ(properties.GetPersp(), Vector4f(1.0, 1.0, 1.0, 1.0));

    properties.SetPerspZ(2.0);
    EXPECT_EQ(properties.GetPerspZ(), 2.0);

    properties.SetPerspW(3.0);
    EXPECT_EQ(properties.GetPerspW(), 3.0);
}

/**
 * @tc.name: GenerateRenderFilter_001
 * @tc.desc: test GenerateRenderFilter
 * @tc.type: FUNC
 */
HWTEST_F(PropertiesTest, GenerateRenderFilter_001, TestSize.Level1)
{
    RSProperties properties;
    auto renderFilter = std::make_shared<RSRenderFilter>();
    auto renderFilterBase = RSRenderFilter::CreateRenderFilterPara(RSUIFilterType::BLUR);
    renderFilter->Insert(RSUIFilterType::BLUR, renderFilterBase);
    properties.backgroundRenderFilter_ = renderFilter;
    properties.GenerateRenderFilter();
    EXPECT_EQ(properties.backgroundFilter_, nullptr);

    auto renderFilter1 = std::make_shared<RSRenderFilter>();
    auto renderFilterBase1 = RSRenderFilter::CreateRenderFilterPara(RSUIFilterType::COLOR_GRADIENT);
    renderFilter1->Insert(RSUIFilterType::COLOR_GRADIENT, renderFilterBase1);
    properties.backgroundRenderFilter_ = renderFilter1;
    properties.GenerateRenderFilter();
    EXPECT_EQ(properties.backgroundFilter_, nullptr);
}

/**
 * @tc.name: GenerateRenderFilterColorGradient_001
 * @tc.desc: test GenerateRenderFilterColorGradient
 * @tc.type: FUNC
 */
HWTEST_F(PropertiesTest, GenerateRenderFilterColorGradient_001, TestSize.Level1)
{
    RSProperties properties;
    properties.GenerateRenderFilterColorGradient();
    EXPECT_EQ(properties.backgroundFilter_, nullptr);

    auto renderFilter1 = std::make_shared<RSRenderFilter>();
    auto renderFilterBase1 = RSRenderFilter::CreateRenderFilterPara(RSUIFilterType::COLOR_GRADIENT);
    renderFilter1->Insert(RSUIFilterType::COLOR_GRADIENT, renderFilterBase1);
    properties.backgroundRenderFilter_ = renderFilter1;
    properties.GenerateRenderFilterColorGradient();
    EXPECT_EQ(properties.backgroundFilter_, nullptr);
}
} // namespace Rosen
} // namespace OHOS