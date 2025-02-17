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

#include "property/rs_properties.h"
#include "common/rs_obj_abs_geometry.h"
#include "property/rs_point_light_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class PropertiesOneTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void PropertiesOneTest::SetUpTestCase() {}
void PropertiesOneTest::TearDownTestCase() {}
void PropertiesOneTest::SetUp() {}
void PropertiesOneTest::TearDown() {}

/**
 * @tc.name: SetShadowRadius
 * @tc.desc: test results of SetShadowRadius
 * @tc.type: FUNC
 * @tc.require: issuesIB7RKW
 */
HWTEST_F(PropertiesOneTest, SetShadowRadius, TestSize.Level1)
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
 * @tc.name: SetClipToBounds
 * @tc.desc: test results of SetClipToBounds
 * @tc.type: FUNC
 * @tc.require: issuesIB7RKW
 */
HWTEST_F(PropertiesOneTest, SetClipToBounds, TestSize.Level1)
{
    RSProperties properties;
    properties.SetClipToBounds(true);
    ASSERT_TRUE(properties.GetClipToBounds() == true);
    properties.SetClipToBounds(true);
    ASSERT_TRUE(properties.GetClipToBounds() == true);

    properties.SetClipToBounds(false);
    EXPECT_TRUE(properties.geoDirty_);
}

/**
 * @tc.name: SetClipBounds
 * @tc.desc: test results of SetClipBounds
 * @tc.type: FUNC
 * @tc.require: issuesIB7RKW
 */
HWTEST_F(PropertiesOneTest, SetClipBounds, TestSize.Level1)
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

#if defined(NEW_SKIA) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
/**
 * @tc.name: CreateFilterCacheManagerIfNeed
 * @tc.desc: test results of CreateFilterCacheManagerIfNeed
 * @tc.type: FUNC
 * @tc.require: issuesIB7RKW
 */
HWTEST_F(PropertiesOneTest, CreateFilterCacheManagerIfNeed, TestSize.Level1)
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
 * @tc.name: UpdateFilter
 * @tc.desc: test results of UpdateFilter
 * @tc.type: FUNC
 * @tc.require: issuesIB7RKW
 */
HWTEST_F(PropertiesOneTest, UpdateFilter, TestSize.Level1)
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
    EXPECT_FALSE(properties.foregroundFilter_);

    properties.isSpherizeValid_ = false;
    properties.shadow_->imageMask_ = true;
    properties.UpdateFilter();
    EXPECT_FALSE(properties.foregroundFilter_);

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
 * @tc.name: OnApplyModifiers
 * @tc.desc: test results of OnApplyModifiers
 * @tc.type: FUNC
 * @tc.require: issuesIB7RKW
 */
HWTEST_F(PropertiesOneTest, OnApplyModifiers, TestSize.Level1)
{
    RSProperties properties;
    properties.OnApplyModifiers();

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
 * @tc.name: SetForegroundFilterCache
 * @tc.desc: test results of SetForegroundFilterCache
 * @tc.type: FUNC
 * @tc.require: issuesIB7RKW
 */
HWTEST_F(PropertiesOneTest, SetForegroundFilterCache, TestSize.Level1)
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
 * @tc.name: SetBorderColor
 * @tc.desc: test results of SetBorderColor
 * @tc.type: FUNC
 * @tc.require: issuesIB7RKW
 */
HWTEST_F(PropertiesOneTest, SetBorderColor, TestSize.Level1)
{
    RSProperties properties;
    Color color(0, 0, 0, 0);
    Vector4<Color> borderColor = {color, color, color, color};
    properties.SetBorderColor(borderColor);
    EXPECT_TRUE(properties.contentDirty_);
}

/**
 * @tc.name: GetBgBrightnessDescription
 * @tc.desc: test results of GetBgBrightnessDescription
 * @tc.type: FUNC
 * @tc.require: issuesIB7RKW
 */
HWTEST_F(PropertiesOneTest, GetBgBrightnessDescription, TestSize.Level1)
{
    RSProperties properties;
    properties.bgBrightnessParams_ = std::nullopt;
    properties.GetBgBrightnessDescription();
    EXPECT_TRUE(properties.bgBrightnessParams_ == std::nullopt);

    RSDynamicBrightnessPara value;
    properties.bgBrightnessParams_ = value;
    properties.GetBgBrightnessDescription();
    EXPECT_TRUE(properties.bgBrightnessParams_ != std::nullopt);
}

/**
 * @tc.name: GetFgBrightnessDescription
 * @tc.desc: test results of GetFgBrightnessDescription
 * @tc.type: FUNC
 * @tc.require: issuesIB7RKW
 */
HWTEST_F(PropertiesOneTest, GetFgBrightnessDescription, TestSize.Level1)
{
    RSProperties properties;
    properties.fgBrightnessParams_ = std::nullopt;
    properties.GetFgBrightnessDescription();
    EXPECT_TRUE(properties.fgBrightnessParams_ == std::nullopt);

    RSDynamicBrightnessPara value;
    properties.fgBrightnessParams_ = value;
    properties.GetFgBrightnessDescription();
    EXPECT_TRUE(properties.fgBrightnessParams_ != std::nullopt);
}

/**
 * @tc.name: SetDynamicLightUpRate
 * @tc.desc: test results of SetDynamicLightUpRate
 * @tc.type: FUNC
 * @tc.require: issuesIB7RKW
 */
HWTEST_F(PropertiesOneTest, SetDynamicLightUpRate, TestSize.Level1)
{
    RSProperties properties;
    std::optional<float> rate;
    properties.SetDynamicLightUpRate(rate);
    EXPECT_TRUE(!rate.has_value());

    rate = std::optional<float>(1.f);
    properties.SetDynamicLightUpRate(rate);
    ASSERT_TRUE(properties.GetDynamicLightUpRate().has_value());
    EXPECT_FALSE(properties.IsDynamicLightUpValid());
}

/**
 * @tc.name: SetNGetDynamicDimDegree
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issuesIB7RKW
 */
HWTEST_F(PropertiesOneTest, SetNGetDynamicDimDegree, TestSize.Level1)
{
    RSProperties properties;
    std::optional<float> dimDegree(0.99f);
    properties.SetDynamicDimDegree(dimDegree);
    ASSERT_TRUE(properties.GetDynamicDimDegree().has_value());
    EXPECT_TRUE(properties.IsDynamicDimValid());
    dimDegree = 1.f;
    properties.SetDynamicDimDegree(dimDegree);
    ASSERT_TRUE(properties.GetDynamicDimDegree().has_value());
    EXPECT_FALSE(properties.IsDynamicDimValid());

    std::optional<float> degree;
    properties.SetDynamicDimDegree(degree);
    EXPECT_TRUE(!degree.has_value());
}

/**
 * @tc.name: SetDynamicLightUpDegree
 * @tc.desc: test results of SetDynamicLightUpDegree
 * @tc.type: FUNC
 * @tc.require: issuesIB7RKW
 */
HWTEST_F(PropertiesOneTest, SetDynamicLightUpDegree, TestSize.Level1)
{
    RSProperties properties;
    std::optional<float> lightUpDegree;
    properties.SetDynamicLightUpRate(lightUpDegree);
    EXPECT_TRUE(!lightUpDegree.has_value());

    lightUpDegree = std::optional<float>(1.f);
    properties.SetDynamicLightUpDegree(lightUpDegree);
    ASSERT_TRUE(properties.GetDynamicLightUpDegree().has_value());
}

/**
 * @tc.name: SetClipRRect
 * @tc.desc: test results of SetClipRRect
 * @tc.type: FUNC
 * @tc.require: issuesIB7RKW
 */
HWTEST_F(PropertiesOneTest, SetClipRRect, TestSize.Level1)
{
    RSProperties properties;
    RectT<float> rect(1.f, 1.f, 1.f, 1.f);
    RRect clipRRect(rect, 1.f, 1.f);
    properties.SetClipRRect(clipRRect);
    EXPECT_TRUE(properties.GetClipToRRect());

    RRect clipRRectNew;
    properties.SetClipRRect(clipRRectNew);
    EXPECT_TRUE(properties.clipRRect_.has_value());
}

/**
 * @tc.name: SetClipToFrame
 * @tc.desc: test results of SetClipToFrame
 * @tc.type: FUNC
 * @tc.require: issuesIB7RKW
 */
HWTEST_F(PropertiesOneTest, SetClipToFrame, TestSize.Level1)
{
    RSProperties properties;
    properties.SetClipToFrame(true);
    EXPECT_TRUE(properties.GetClipToFrame() == true);
    properties.SetClipToFrame(true);
    EXPECT_TRUE(properties.GetClipToFrame() == true);

    properties.SetClipToFrame(false);
    EXPECT_TRUE(!properties.clipToFrame_);
}

/**
 * @tc.name: GenerateForegroundFilter
 * @tc.desc: test results of GenerateForegroundFilter
 * @tc.type: FUNC
 * @tc.require: issuesIB7RKW
 */
HWTEST_F(PropertiesOneTest, GenerateForegroundFilter, TestSize.Level1)
{
    RSProperties properties;
    properties.GenerateForegroundFilter();

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
 * @tc.name: SetHaveEffectRegion
 * @tc.desc: test results of SetHaveEffectRegion
 * @tc.type: FUNC
 * @tc.require: issuesIB7RKW
 */
HWTEST_F(PropertiesOneTest, SetHaveEffectRegion, TestSize.Level1)
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
 * @tc.name: CalculateFrameOffset
 * @tc.desc: test results of CalculateFrameOffset
 * @tc.type: FUNC
 * @tc.require: issuesIB7RKW
 */
HWTEST_F(PropertiesOneTest, CalculateFrameOffset, TestSize.Level1)
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
}


/**
 * @tc.name: SetFlyOutParams
 * @tc.desc: test results of SetFlyOutParams
 * @tc.type: FUNC
 * @tc.require: issuesIB7RKW
 */
HWTEST_F(PropertiesOneTest, SetFlyOutParams, TestSize.Level1)
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
 * @tc.name: SetFlyOutDegree
 * @tc.desc: test results of SetFlyOutDegree
 * @tc.type: FUNC
 * @tc.require: issuesIB7RKW
 */
HWTEST_F(PropertiesOneTest, SetFlyOutDegree, TestSize.Level1)
{
    RSProperties properties;
    properties.SetFlyOutDegree(0.7f);
    float degree = properties.GetFlyOutDegree();
    EXPECT_FLOAT_EQ(degree, 0.7f);
}

/**
 * @tc.name: CreateFlyOutShaderFilter
 * @tc.desc: test results of CreateFlyOutShaderFilter
 * @tc.type: FUNC
 * @tc.require: issuesIB7RKW
 */
HWTEST_F(PropertiesOneTest, CreateFlyOutShaderFilter, TestSize.Level1)
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
 * @tc.name: IsFlyOutValid
 * @tc.desc: test results of IsFlyOutValid
 * @tc.type: FUNC
 * @tc.require: issuesIB7RKW
 */
HWTEST_F(PropertiesOneTest, IsFlyOutValid, TestSize.Level1)
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
 * @tc.name: SetDistortionK
 * @tc.desc: test results of SetDistortionK
 * @tc.type: FUNC
 * @tc.require: issuesIB7RKW
 */
HWTEST_F(PropertiesOneTest, SetDistortionK, TestSize.Level1)
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
 * @tc.name: GetDistortionDirty
 * @tc.desc: test results of GetDistortionDirty
 * @tc.type: FUNC
 * @tc.require: issuesIB7RKW
 */
HWTEST_F(PropertiesOneTest, GetDistortionDirty, TestSize.Level1)
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
 * @tc.name: IsDistortionKValid
 * @tc.desc: test results of IsDistortionKValid
 * @tc.type: FUNC
 * @tc.require: issuesIB7RKW
 */
HWTEST_F(PropertiesOneTest, IsDistortionKValid, TestSize.Level1)
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
} // namespace Rosen
} // namespace OHOS