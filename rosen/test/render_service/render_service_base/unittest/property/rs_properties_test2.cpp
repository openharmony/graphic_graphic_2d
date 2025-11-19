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

#include "effect/rs_render_filter_base.h"
#include "effect/rs_render_shape_base.h"
#include "params/rs_render_params.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_screen_render_node.h"
#include "property/rs_properties.h"
#include "common/rs_obj_abs_geometry.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "property/rs_point_light_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class PropertiesTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    static bool IsForegroundFilter(RSProperties& properties);
    void SetUp() override;
    void TearDown() override;
};

void PropertiesTest::SetUpTestCase() {}
void PropertiesTest::TearDownTestCase() {}
void PropertiesTest::SetUp() {}
void PropertiesTest::TearDown() {}
bool PropertiesTest::IsForegroundFilter(RSProperties& properties)
{
    bool isUniRender = RSProperties::IS_UNI_RENDER;
    if (isUniRender) {
        return properties.GetEffect().foregroundFilterCache_ != nullptr;
    } else {
        return properties.GetEffect().foregroundFilter_ != nullptr;
    }
}

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

    properties.GetEffect().shadow_->elevation_ = -1.f;
    radius = -1.0f;
    properties.SetShadowRadius(radius);
    EXPECT_FALSE(properties.GetEffect().shadow_->IsValid());
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

#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
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

    properties.GetEffect().backgroundFilter_ = std::make_shared<RSFilter>();
    properties.CreateFilterCacheManagerIfNeed();

    properties.GetEffect().filter_ = std::make_shared<RSFilter>();
    properties.CreateFilterCacheManagerIfNeed();
    EXPECT_TRUE(properties.GetEffect().filter_ != nullptr);

    properties.CreateFilterCacheManagerIfNeed();
    EXPECT_TRUE(properties.GetEffect().filter_ != nullptr);
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

    properties.GetEffect().greyCoefNeedUpdate_ = true;
    properties.OnApplyModifiers();
    EXPECT_TRUE(!properties.GetEffect().greyCoefNeedUpdate_);

    properties.GetEffect().shadow_ = std::make_optional<RSShadow>();
    properties.GetEffect().shadow_->colorStrategy_ = SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_MAIN;
    properties.GetEffect().backgroundFilter_ = std::make_shared<RSFilter>();
    properties.GetEffect().filter_ = std::make_shared<RSFilter>();
    properties.GetEffect().foregroundEffectRadius_ = 1.f;
    Vector2f scaleAnchor = Vector2f(0.f, 0.f);
    properties.GetEffect().motionBlurPara_ = std::make_shared<MotionBlurParam>(1.f, scaleAnchor);
    properties.OnApplyModifiers();
    EXPECT_TRUE(properties.GetEffect().filter_ != nullptr);

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
    properties.GetEffect().shadow_ = std::make_optional<RSShadow>();
    properties.GetEffect().shadow_->colorStrategy_ = SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_AVERAGE;
    properties.UpdateFilter();
    EXPECT_TRUE(properties.filterNeedUpdate_);

    properties.GetEffect().foregroundEffectRadius_ = 0.1f;
    properties.UpdateFilter();
    EXPECT_FALSE(properties.GetEffect().foregroundFilter_);

    properties.GetEffect().foregroundEffectRadius_ = -0.1f;
    properties.GetEffect().isSpherizeValid_ = true;
    properties.UpdateFilter();
    EXPECT_TRUE(properties.GetEffect().foregroundFilter_);

    properties.GetEffect().isSpherizeValid_ = false;
    properties.GetEffect().shadow_->imageMask_ = true;
    properties.UpdateFilter();
    EXPECT_TRUE(properties.GetEffect().foregroundFilter_);

    properties.GetEffect().foregroundEffectRadius_ = -0.1f;
    properties.GetEffect().isAttractionValid_ = true;
    properties.UpdateFilter();
    EXPECT_TRUE(properties.GetEffect().foregroundFilter_);

    properties.GetEffect().isAttractionValid_ = false;
    properties.GetEffect().shadow_->imageMask_ = true;
    properties.UpdateFilter();
    EXPECT_TRUE(properties.GetEffect().foregroundFilter_);

    properties.GetEffect().shadow_->imageMask_ = false;
    properties.UpdateFilter();
    EXPECT_TRUE(!properties.GetEffect().foregroundFilter_);

    Vector2f scaleAnchor = Vector2f(0.f, 0.f);
    properties.GetEffect().motionBlurPara_ = std::make_shared<MotionBlurParam>(1.f, scaleAnchor);
    properties.UpdateFilter();
    EXPECT_TRUE(properties.GetEffect().foregroundFilter_);

    uint32_t flyMode = 0;
    RSFlyOutPara rs_fly_out_param = {
        flyMode
    };
    properties.GetEffect().flyOutParams_ = std::optional<RSFlyOutPara>(rs_fly_out_param);
    properties.GetEffect().foregroundEffectRadius_ = -0.1f;
    properties.GetEffect().flyOutDegree_ = 0.5;
    properties.UpdateFilter();
    EXPECT_TRUE(properties.GetEffect().foregroundFilter_);

    properties.GetEffect().flyOutDegree_ = 0.5;
    properties.GetEffect().shadow_->imageMask_ = true;
    properties.UpdateFilter();
    EXPECT_TRUE(properties.GetEffect().foregroundFilter_);

    properties.GetEffect().distortionK_ = 0.7;
    properties.GetEffect().shadow_->imageMask_ = true;
    properties.UpdateFilter();
    EXPECT_TRUE(properties.GetEffect().foregroundFilter_);
}

/**
 * @tc.name: UpdateForegroundFilterTest
 * @tc.desc: test UpdateForegroundFilter
 * @tc.type: FUNC
 */
HWTEST_F(PropertiesTest, UpdateForegroundFilterTest, TestSize.Level1)
{
    RSProperties properties;
    properties.SetHDRUIBrightness(2.0f); // hdr brightness
    properties.UpdateForegroundFilter();
    bool isUniRender = RSProperties::IS_UNI_RENDER;
    if (isUniRender) {
        EXPECT_FALSE(properties.GetEffect().foregroundFilterCache_ == nullptr);
    } else {
        EXPECT_FALSE(properties.GetEffect().foregroundFilter_ == nullptr);
    }
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
    EXPECT_EQ(properties.GetEffect().particles_.renderParticleVector_.size(), 0);

    auto particleParams = std::make_shared<ParticleRenderParams>();
    std::shared_ptr<RSRenderParticle> newv = std::make_shared<RSRenderParticle>(particleParams);
    particles.renderParticleVector_.push_back(newv);
    properties.SetParticles(particles);
    EXPECT_EQ(properties.GetEffect().particles_.GetParticleSize(), 1);
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
    properties.GetEffect().fgBrightnessParams_ = std::nullopt;
    properties.GetFgBrightnessDescription();

    RSDynamicBrightnessPara value;
    properties.GetEffect().fgBrightnessParams_ = value;
    properties.GetFgBrightnessDescription();
    EXPECT_TRUE(properties.GetEffect().fgBrightnessParams_ != std::nullopt);
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
    properties.GetEffect().bgBrightnessParams_ = std::nullopt;
    properties.GetBgBrightnessDescription();

    RSDynamicBrightnessPara value;
    properties.GetEffect().bgBrightnessParams_ = value;
    properties.GetBgBrightnessDescription();
    EXPECT_TRUE(properties.GetEffect().bgBrightnessParams_ != std::nullopt);
}

/**
 * @tc.name: SetHDRUIBrightnessTest
 * @tc.desc: test SetHDRUIBrightness
 * @tc.type: FUNC
 */
HWTEST_F(PropertiesTest, SetHDRUIBrightnessTest, TestSize.Level1)
{
    RSProperties properties;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(1);
    NodeId surfaceNodeId = 2; // surface node id
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceNodeId);
    properties.backref_ = canvasNode;
    auto stagingRenderParams = std::make_unique<RSRenderParams>(0);
    ASSERT_NE(stagingRenderParams, nullptr);
    canvasNode->stagingRenderParams_ = std::move(stagingRenderParams);
    canvasNode->instanceRootNodeId_ = surfaceNodeId;

    canvasNode->isOnTheTree_ = false;
    properties.SetHDRUIBrightness(1.0f);
    EXPECT_EQ(properties.GetHDRUIBrightness(), 1.0f);
    float hdrBrightness = 2.0f; // hdr brightness
    properties.SetHDRUIBrightness(hdrBrightness);
    EXPECT_EQ(properties.GetHDRUIBrightness(), hdrBrightness);

    canvasNode->isOnTheTree_ = true;
    properties.SetHDRUIBrightness(1.0f);
    EXPECT_EQ(properties.GetHDRUIBrightness(), 1.0f);
}

/**
 * @tc.name: IsHDRUIBrightnessValidTest
 * @tc.desc: test IsHDRUIBrightnessValid
 * @tc.type: FUNC
 */
HWTEST_F(PropertiesTest, IsHDRUIBrightnessValidTest, TestSize.Level1)
{
    RSProperties properties;
    float hdrBrightness = 2.0f; // hdr brightness
    properties.SetHDRUIBrightness(hdrBrightness);
    EXPECT_TRUE(properties.IsHDRUIBrightnessValid());
    properties.SetHDRUIBrightness(1.0f);
    EXPECT_FALSE(properties.IsHDRUIBrightnessValid());
}

/**
 * @tc.name: CreateHDRUIBrightnessFilterTest
 * @tc.desc: test CreateHDRUIBrightnessFilter
 * @tc.type: FUNC
 */
HWTEST_F(PropertiesTest, CreateHDRUIBrightnessFilterTest, TestSize.Level1)
{
    RSProperties properties;
    float hdrBrightness = 2.0f; // hdr brightness
    properties.SetHDRUIBrightness(hdrBrightness);
    properties.CreateHDRUIBrightnessFilter();
    bool isUniRender = RSProperties::IS_UNI_RENDER;
    if (isUniRender) {
        EXPECT_FALSE(properties.GetEffect().foregroundFilterCache_ == nullptr);
    } else {
        EXPECT_FALSE(properties.GetEffect().foregroundFilter_ == nullptr);
    }
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

    NodeId screenRenderNodeId = 2;
    ScreenId screenId = 0;
    auto context = std::make_shared<RSContext>();
    auto screenRenderNode = std::make_shared<RSScreenRenderNode>(screenRenderNodeId, screenId, context);

    properties.backref_ = screenRenderNode;
    screenRenderNode->InsertHDRNode(screenRenderNodeId);
    EXPECT_NE(screenRenderNode->hdrNodeList_.find(screenRenderNodeId), screenRenderNode->hdrNodeList_.end());
    properties.SetHDRBrightnessFactor(0.5f);

    NodeId nodeId1 = 0;
    auto node1 = std::make_shared<RSRenderNode>(nodeId1);
    pid_t pid1 = ExtractPid(nodeId1);
    context->GetMutableNodeMap().renderNodeMap_[pid1][nodeId1] = node1;
    screenRenderNode->InsertHDRNode(nodeId1);
    properties.SetHDRBrightnessFactor(0.6f);

    pid_t pid = ExtractPid(screenRenderNodeId);
    context->GetMutableNodeMap().renderNodeMap_[pid][screenRenderNodeId] = screenRenderNode;
    properties.SetHDRBrightnessFactor(0.8f);

    ScreenId screenId2 = 1;
    std::shared_ptr<RSContext> context2;
    auto screenNode2 = std::make_shared<RSScreenRenderNode>(3, screenId2, context2);
    properties.backref_ = screenNode2;
    screenRenderNode->InsertHDRNode(3);
    EXPECT_NE(screenRenderNode->hdrNodeList_.find(3), screenRenderNode->hdrNodeList_.end());
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
    EXPECT_EQ(properties.GetEffect().emitterUpdater_.empty(), true);

    auto emitter = std::make_shared<EmitterUpdater>(0);
    para.push_back(emitter);
    properties.SetEmitterUpdater(para);
    EXPECT_EQ(properties.GetEffect().emitterUpdater_.empty(), false);

    std::shared_ptr<RSRenderNode> node = std::make_shared<RSRenderNode>(1);
    properties.backref_ = node;
    properties.SetEmitterUpdater(para);
    EXPECT_EQ(properties.GetEffect().emitterUpdater_.empty(), false);

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
    EXPECT_NE(properties.GetEffect().linearGradientBlurPara_, nullptr);
    EXPECT_EQ(properties.GetLinearGradientBlurPara(), para);
    properties.IfLinearGradientBlurInvalid();

    para->blurRadius_ = 0.f;
    properties.SetLinearGradientBlurPara(para);
    EXPECT_NE(properties.GetEffect().linearGradientBlurPara_, nullptr);

    para = nullptr;
    properties.SetLinearGradientBlurPara(para);
    EXPECT_EQ(properties.GetEffect().linearGradientBlurPara_, nullptr);
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
    properties.GetEffect().shadow_ = shadow;
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

    properties.GetEffect().greyCoef_ = vectorValue;
    properties.GenerateBackgroundMaterialBlurFilter();
    EXPECT_EQ(vectorValue.x_, 1.f);

    properties.GetEffect().backgroundColorMode_ = BLUR_COLOR_MODE::AVERAGE;
    properties.GenerateBackgroundMaterialBlurFilter();
    EXPECT_EQ(vectorValue.x_, 1.f);

    properties.GetEffect().backgroundColorMode_ = BLUR_COLOR_MODE::FASTAVERAGE;
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
    EXPECT_EQ(properties.GetEffect().backgroundBlurSaturation_, 1.f);

    properties.GetEffect().foregroundBlurRadiusX_ = 2.f;
    properties.GenerateForegroundFilter();
    EXPECT_TRUE(properties.IsForegroundBlurRadiusXValid());

    properties.GetEffect().foregroundBlurRadiusY_ = 2.f;
    properties.GenerateForegroundFilter();
    EXPECT_TRUE(properties.IsForegroundBlurRadiusYValid());

    properties.GetEffect().foregroundBlurRadius_ = 2.f;
    properties.GenerateForegroundFilter();
    EXPECT_TRUE(properties.IsForegroundMaterialFilterVaild());

    std::vector<std::pair<float, float>> fractionStops;
    GradientDirection direction;
    properties.GetEffect().linearGradientBlurPara_ =
        std::make_shared<RSLinearGradientBlurPara>(-1.f, fractionStops, direction);
    properties.GetEffect().linearGradientBlurPara_->blurRadius_ = 1.f;
    properties.GenerateForegroundFilter();
    EXPECT_TRUE(properties.GetEffect().linearGradientBlurPara_);
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
    properties.GetEffect().greyCoef_ = Vector2f(1.0f, 2.0f);
    properties.CheckGreyCoef();
    EXPECT_NE(properties.GetEffect().greyCoef_, std::nullopt);

    properties.GetEffect().greyCoef_->y_ = 128.f;
    properties.CheckGreyCoef();
    EXPECT_EQ(properties.GetEffect().greyCoef_, std::nullopt);

    properties.GetEffect().greyCoef_->y_ = -0.1f;
    properties.CheckGreyCoef();
    EXPECT_EQ(properties.GetEffect().greyCoef_, std::nullopt);

    properties.GetEffect().greyCoef_->x_ = 128.f;
    properties.CheckGreyCoef();
    EXPECT_EQ(properties.GetEffect().greyCoef_, std::nullopt);

    properties.GetEffect().greyCoef_->x_ = -0.1f;
    properties.CheckGreyCoef();
    EXPECT_EQ(properties.GetEffect().greyCoef_, std::nullopt);
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
 * @tc.name: SetShadowColorTest
 * @tc.desc: test SetShadowColor with shadow mask
 * @tc.type: FUNC
 */
HWTEST_F(PropertiesTest, SetShadowColorTest, TestSize.Level1)
{
    RSProperties properties;
    Color color(0XFF0000FF); // 0XFF0000FF is RBGA
    properties.SetShadowColor(color);
    EXPECT_FALSE(properties.filterNeedUpdate_);

    properties.SetShadowMask(2); // mask color blur
    properties.filterNeedUpdate_ = false;
    properties.SetShadowColor(color);
    EXPECT_TRUE(properties.filterNeedUpdate_);
}

/**
 * @tc.name: UpdateForegroundFilterTest001
 * @tc.desc: test UpdateForegroundFilter with shadow mask
 * @tc.type: FUNC
 */
HWTEST_F(PropertiesTest, UpdateForegroundFilterTest001, TestSize.Level1)
{
    RSProperties properties;
    properties.SetShadowMask(0); // mask none
    properties.UpdateForegroundFilter();
    EXPECT_FALSE(IsForegroundFilter(properties));

    properties.SetShadowMask(10); // 10 is invalid
    properties.UpdateForegroundFilter();
    EXPECT_FALSE(IsForegroundFilter(properties));

    properties.SetShadowMask(2); // mask color blur
    properties.UpdateForegroundFilter();
    EXPECT_TRUE(IsForegroundFilter(properties));
}

/**
 * @tc.name: SetAlwaysSnapshot
 * @tc.desc: SetAlwaysSnapshot
 * @tc.type: FUNC
 */
HWTEST_F(PropertiesTest, SetAlwaysSnapshotTest, TestSize.Level1)
{
    RSProperties properties;
    properties.GenerateBackgroundFilter();
    EXPECT_EQ(properties.GetEffect().backgroundFilter_, nullptr);

    properties.SetAlwaysSnapshot(true);
    EXPECT_EQ(properties.GetAlwaysSnapshot(), true);
    properties.GenerateBackgroundFilter();
    ASSERT_NE(properties.GetEffect().backgroundFilter_, nullptr);
    EXPECT_EQ(properties.GetEffect().backgroundFilter_->GetFilterType(), RSFilter::ALWAYS_SNAPSHOT);
}

/**
 * @tc.name: GenerateAlwaysSnapshotFilterTest
 * @tc.desc: test GenerateAlwaysSnapshotFilter
 * @tc.type: FUNC
 */
HWTEST_F(PropertiesTest,  GenerateAlwaysSnapshotFilterTest, TestSize.Level1)
{
    RSProperties properties;
    properties.GenerateAlwaysSnapshotFilter();
    ASSERT_NE(properties.GetEffect().backgroundFilter_, nullptr);
    EXPECT_EQ(properties.GetEffect().backgroundFilter_->GetFilterType(), RSFilter::ALWAYS_SNAPSHOT);
}

/**
 * @tc.name: ShadowBlenderTest
 * @tc.desc: test ShadowBlender SetParams, GetParams, Invalid and Description.
 * @tc.type: FUNC
 */
HWTEST_F(PropertiesTest, ShadowBlenderTest, TestSize.Level1)
{
    RSProperties properties;
    std::optional<RSShadowBlenderPara> paramsNull = std::nullopt;
    properties.SetShadowBlenderParams(paramsNull);
    EXPECT_FALSE(properties.isDrawn_);
    std::string description = "shadowBlenderParams_ is nullopt";
    EXPECT_EQ(description, properties.GetShadowBlenderDescription());

    float cubic = 0;
    float quadratic = 0;
    float linear = 0;
    float constant = 0;
    auto params = std::optional<RSShadowBlenderPara>({ cubic, quadratic, linear, constant });
    properties.SetShadowBlenderParams(params);
    EXPECT_TRUE(properties.isDrawn_);
    EXPECT_TRUE(properties.GetShadowBlenderParams().has_value());
    EXPECT_TRUE(properties.IsShadowBlenderValid());
    description = "ShadowBlender, cubic: " + std::to_string(cubic) +
        ", quadratic: " + std::to_string(quadratic) +
        ", linear: " + std::to_string(linear) +
        ", constant: " + std::to_string(constant);
    EXPECT_EQ(description, properties.GetShadowBlenderDescription());
}

/**
 * @tc.name: UpdateForegroundFilterTest_RenderFilter001
 * @tc.desc: test UpdateForegroundFilter with render filter only
 * @tc.type: FUNC
 */
HWTEST_F(PropertiesTest,  UpdateForegroundFilterTest_RenderFilter001, TestSize.Level1)
{
    RSProperties properties;
    properties.SetForegroundNGFilter(RSNGRenderFilterBase::Create(RSNGEffectType::BLUR));
    properties.UpdateForegroundFilter();
    EXPECT_FALSE(properties.GetEffect().foregroundFilter_ == nullptr);
}

/**
 * @tc.name: PixelStretchUpdateFilterFlag001
 * @tc.desc: test update filter flag with pixel stretch enable
 * @tc.type: FUNC
 */
HWTEST_F(PropertiesTest, PixelStretchUpdateFilterFlag001, TestSize.Level1)
{
    RSProperties properties;
    properties.SetPixelStretch(Vector4f(10.0, 10.0, 10.0, 10.0));
    properties.UpdateFilter();
    EXPECT_TRUE(properties.NeedFilter());
    EXPECT_TRUE(properties.NeedHwcFilter());
    EXPECT_TRUE(properties.DisableHWCForFilter());
}

/**
 * @tc.name: SetMaterialFilter001
 * @tc.desc: test SetMaterialFilter
 * @tc.type: FUNC
 */
HWTEST_F(PropertiesTest, SetMaterialFilter001, TestSize.Level1)
{
    RSProperties properties;
    std::shared_ptr<RSNGRenderFilterBase> filter = RSNGRenderFilterBase::Create(RSNGEffectType::BLUR);
    properties.SetMaterialNGFilter(filter);
    ASSERT_NE(properties.effect_->mtNGRenderFilter_, nullptr);
    ASSERT_TRUE(properties.isDrawn_);
    ASSERT_TRUE(properties.filterNeedUpdate_);
    ASSERT_TRUE(properties.contentDirty_);
}

/**
 * @tc.name: GetMaterialFilter001
 * @tc.desc: test GetMaterialFilter
 * @tc.type: FUNC
 */
HWTEST_F(PropertiesTest, GetMaterialFilter001, TestSize.Level1)
{
    RSProperties properties;
    std::shared_ptr<RSNGRenderFilterBase> filter = RSNGRenderFilterBase::Create(RSNGEffectType::BLUR);
    ASSERT_NE(filter, nullptr);
    properties.GetEffect().mtNGRenderFilter_ = filter;
    ASSERT_NE(properties.GetMaterialNGFilter(), nullptr);
}

/**
 * @tc.name: GenerateMaterialFilter001
 * @tc.desc: test GenerateMaterialFilter
 * @tc.type: FUNC
 */
HWTEST_F(PropertiesTest, GenerateMaterialFilter001, TestSize.Level1)
{
    RSProperties properties;
    ASSERT_EQ(properties.GetMaterialNGFilter(), nullptr);
    properties.GenerateMaterialFilter();
    ASSERT_EQ(properties.GetEffect().materialFilter_, nullptr);
}

/**
 * @tc.name: GenerateMaterialFilter002
 * @tc.desc: test GenerateMaterialFilter
 * @tc.type: FUNC
 */
HWTEST_F(PropertiesTest, GenerateMaterialFilter002, TestSize.Level1)
{
    RSProperties properties;
    std::shared_ptr<RSNGRenderFilterBase> filter = RSNGRenderFilterBase::Create(RSNGEffectType::BLUR);
    properties.GetEffect().mtNGRenderFilter_ = filter;
    ASSERT_NE(properties.GetMaterialNGFilter(), nullptr);
    properties.GenerateMaterialFilter();
    ASSERT_NE(properties.GetEffect().materialFilter_, nullptr);
}

/**
 * @tc.name: OnSDFShapeChangeTest001
 * @tc.desc: test when NeedGenerateForegroundFilterCache returns true
 * @tc.type: FUNC
 */
HWTEST_F(PropertiesTest, OnSDFShapeChangeTest001, TestSize.Level1)
{
    RSProperties properties;

    auto sdfShape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_UNION_OP_SHAPE);
    EXPECT_NE(sdfShape, nullptr);
    properties.renderSDFShape_ = sdfShape;

    properties.OnSDFShapeChange();
    EXPECT_NE(RSProperties::IS_UNI_RENDER ?
        properties.GetEffect().foregroundFilterCache_ : properties.GetEffect().foregroundFilter_, nullptr);
}

/**
 * @tc.name: OnSDFShapeChangeTest002
 * @tc.desc: test when NeedGenerateForegroundFilterCache returns false
 * @tc.type: FUNC
 */
HWTEST_F(PropertiesTest, OnSDFShapeChangeTest002, TestSize.Level1)
{
    RSProperties properties;

    auto sdfShape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_UNION_OP_SHAPE);
    EXPECT_NE(sdfShape, nullptr);
    properties.renderSDFShape_ = sdfShape;
    properties.sdfFilter_ = std::make_shared<RSSDFEffectFilter>(sdfShape);

    properties.OnSDFShapeChange();
    EXPECT_NE(RSProperties::IS_UNI_RENDER ?
        properties.GetEffect().foregroundFilterCache_ : properties.GetEffect().foregroundFilter_, nullptr);
}

/**
 * @tc.name: OnSDFShapeChangeTest003
 * @tc.desc: test when renderSDFShape_ is false
 * @tc.type: FUNC
 */
HWTEST_F(PropertiesTest, OnSDFShapeChangeTest003, TestSize.Level1)
{
    RSProperties properties;
    properties.SetSDFShape(nullptr);

    properties.OnSDFShapeChange();

    EXPECT_EQ(properties.GetEffect().foregroundFilter_, nullptr);
    EXPECT_EQ(properties.GetEffect().foregroundFilterCache_, nullptr);
}
} // namespace Rosen
} // namespace OHOS