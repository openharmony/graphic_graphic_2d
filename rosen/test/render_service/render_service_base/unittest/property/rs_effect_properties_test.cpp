/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "property/rs_effect_properties.h"

#include "effect/rs_render_filter_base.h"
#include "effect/rs_render_shader_base.h"
#include "render/rs_drawing_filter.h"
#include "render/rs_path.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSEffectPropertiesTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSEffectPropertiesTest::SetUpTestCase() {}
void RSEffectPropertiesTest::TearDownTestCase() {}
void RSEffectPropertiesTest::SetUp() {}
void RSEffectPropertiesTest::TearDown() {}

/**
 * @tc.name: EffectParamsLazyCreate001
 * @tc.desc: test HasXXXEffect and const/non-const GetXXXEffect lazy creation for all five effect params
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectPropertiesTest, EffectParamsLazyCreate001, TestSize.Level1)
{
    RSEffectProperties properties;
    const RSEffectProperties& constProperties = properties;
    EXPECT_EQ(constProperties.GetFilterEffect(), nullptr);
    EXPECT_EQ(constProperties.GetShaderEffect(), nullptr);
    EXPECT_EQ(constProperties.GetBlenderEffect(), nullptr);
    EXPECT_EQ(constProperties.GetNodesEffect(), nullptr);
    EXPECT_EQ(constProperties.GetDepthEffect(), nullptr);

    auto& filter = properties.GetFilterEffect();
    EXPECT_NE(&filter, nullptr);
    EXPECT_NE(constProperties.GetFilterEffect(), nullptr);

    auto& shader = properties.GetShaderEffect();
    EXPECT_NE(&shader, nullptr);
    EXPECT_NE(constProperties.GetShaderEffect(), nullptr);

    auto& blender = properties.GetBlenderEffect();
    EXPECT_NE(&blender, nullptr);
    EXPECT_NE(constProperties.GetBlenderEffect(), nullptr);

    auto& nodes = properties.GetNodesEffect();
    EXPECT_NE(&nodes, nullptr);
    EXPECT_NE(constProperties.GetNodesEffect(), nullptr);

    auto& depth = properties.GetDepthEffect();
    EXPECT_NE(&depth, nullptr);
    EXPECT_NE(constProperties.GetDepthEffect(), nullptr);
}

/**
 * @tc.name: SetNGetMotionBlurPara001
 * @tc.desc: test SetMotionBlurPara/GetMotionBlurPara with and without filter effect params
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectPropertiesTest, SetNGetMotionBlurPara001, TestSize.Level1)
{
    RSEffectProperties properties;
    EXPECT_EQ(properties.GetMotionBlurPara(), nullptr);
    Vector2f scaleAnchor = Vector2f(0.f, 0.f);
    auto para = std::make_shared<MotionBlurParam>(1.f, scaleAnchor);
    properties.SetMotionBlurPara(para);
    EXPECT_EQ(properties.GetMotionBlurPara(), para);
    properties.SetMotionBlurPara(nullptr);
    EXPECT_EQ(properties.GetMotionBlurPara(), nullptr);
}

/**
 * @tc.name: WaterRipple001
 * @tc.desc: test SetWaterRippleProgress/GetWaterRippleProgress/SetWaterRippleParams/GetWaterRippleParams and
 *           IsWaterRippleValid branches
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectPropertiesTest, WaterRipple001, TestSize.Level1)
{
    RSEffectProperties properties;
    EXPECT_EQ(properties.GetWaterRippleProgress(), 0.0f);
    EXPECT_FALSE(properties.GetWaterRippleParams().has_value());
    EXPECT_FALSE(properties.IsWaterRippleValid());

    properties.SetWaterRippleProgress(0.5f);
    EXPECT_EQ(properties.GetWaterRippleProgress(), 0.5f);

    RSWaterRipplePara params;
    params.waveCount = 2;
    properties.SetWaterRippleParams(params);
    EXPECT_EQ(properties.GetWaterRippleParams(), params);
    EXPECT_TRUE(properties.IsWaterRippleValid());

    // waveCount out of range branches
    params.waveCount = 0;
    properties.SetWaterRippleParams(params);
    EXPECT_FALSE(properties.IsWaterRippleValid());
    params.waveCount = 4;
    properties.SetWaterRippleParams(params);
    EXPECT_FALSE(properties.IsWaterRippleValid());
    // waveCount boundary values
    params.waveCount = 1;
    properties.SetWaterRippleParams(params);
    EXPECT_TRUE(properties.IsWaterRippleValid());
    params.waveCount = 3;
    properties.SetWaterRippleParams(params);
    EXPECT_TRUE(properties.IsWaterRippleValid());
    // progress out of range branches
    params.waveCount = 2;
    properties.SetWaterRippleParams(params);
    properties.SetWaterRippleProgress(-0.1f);
    EXPECT_FALSE(properties.IsWaterRippleValid());
    properties.SetWaterRippleProgress(1.1f);
    EXPECT_FALSE(properties.IsWaterRippleValid());
    // params nullopt branch
    properties.SetWaterRippleProgress(0.5f);
    properties.SetWaterRippleParams(std::nullopt);
    EXPECT_FALSE(properties.IsWaterRippleValid());
}

/**
 * @tc.name: FlyOut001
 * @tc.desc: test SetFlyOutDegree/GetFlyOutDegree/SetFlyOutParams/GetFlyOutParams and IsFlyOutValid branches
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectPropertiesTest, FlyOut001, TestSize.Level1)
{
    RSEffectProperties properties;
    EXPECT_EQ(properties.GetFlyOutDegree(), 0.0f);
    EXPECT_FALSE(properties.GetFlyOutParams().has_value());
    EXPECT_FALSE(properties.IsFlyOutValid());

    properties.SetFlyOutDegree(0.5f);
    EXPECT_EQ(properties.GetFlyOutDegree(), 0.5f);
    RSFlyOutPara params;
    properties.SetFlyOutParams(params);
    EXPECT_EQ(properties.GetFlyOutParams(), params);
    EXPECT_TRUE(properties.IsFlyOutValid());

    // degree out of range branches
    properties.SetFlyOutDegree(-0.5f);
    EXPECT_FALSE(properties.IsFlyOutValid());
    properties.SetFlyOutDegree(1.5f);
    EXPECT_FALSE(properties.IsFlyOutValid());
    // params nullopt branch
    properties.SetFlyOutDegree(0.5f);
    properties.SetFlyOutParams(std::nullopt);
    EXPECT_FALSE(properties.IsFlyOutValid());
}

/**
 * @tc.name: Spherize001
 * @tc.desc: test SetSpherize/GetSpherize and IsSpherizeValid branches
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectPropertiesTest, Spherize001, TestSize.Level1)
{
    RSEffectProperties properties;
    EXPECT_EQ(properties.GetSpherize(), 0.f);
    EXPECT_FALSE(properties.IsSpherizeValid());

    properties.SetSpherize(0.5f);
    EXPECT_EQ(properties.GetSpherize(), 0.5f);
    EXPECT_TRUE(properties.IsSpherizeValid());

    properties.SetSpherize(0.0005f);
    EXPECT_FALSE(properties.IsSpherizeValid());
    properties.SetSpherize(-1.f);
    EXPECT_FALSE(properties.IsSpherizeValid());
}

/**
 * @tc.name: LightUpEffect001
 * @tc.desc: test SetLightUpEffect/GetLightUpEffect and IsLightUpEffectValid branches
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectPropertiesTest, LightUpEffect001, TestSize.Level1)
{
    RSEffectProperties properties;
    EXPECT_EQ(properties.GetLightUpEffect(), 1.0f);
    EXPECT_FALSE(properties.IsLightUpEffectValid());

    properties.SetLightUpEffect(0.5f);
    EXPECT_EQ(properties.GetLightUpEffect(), 0.5f);
    EXPECT_TRUE(properties.IsLightUpEffectValid());

    properties.SetLightUpEffect(-0.5f);
    EXPECT_FALSE(properties.IsLightUpEffectValid());
    properties.SetLightUpEffect(1.0f);
    EXPECT_FALSE(properties.IsLightUpEffectValid());
}

/**
 * @tc.name: ForegroundEffectRadius001
 * @tc.desc: test SetForegroundEffectRadius/GetForegroundEffectRadius and IsForegroundEffectRadiusValid branches
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectPropertiesTest, ForegroundEffectRadius001, TestSize.Level1)
{
    RSEffectProperties properties;
    EXPECT_EQ(properties.GetForegroundEffectRadius(), 0.f);
    EXPECT_FALSE(properties.IsForegroundEffectRadiusValid());

    // radius must be greater than 1.001 to be valid
    properties.SetForegroundEffectRadius(2.f);
    EXPECT_EQ(properties.GetForegroundEffectRadius(), 2.f);
    EXPECT_TRUE(properties.IsForegroundEffectRadiusValid());

    properties.SetForegroundEffectRadius(0.5f);
    EXPECT_FALSE(properties.IsForegroundEffectRadiusValid());
}

/**
 * @tc.name: LinearGradientBlurPara001
 * @tc.desc: test SetLinearGradientBlurPara/GetLinearGradientBlurPara and IfLinearGradientBlurInvalid branches
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectPropertiesTest, LinearGradientBlurPara001, TestSize.Level1)
{
    RSEffectProperties properties;
    EXPECT_EQ(properties.GetLinearGradientBlurPara(), nullptr);
    // no para, invalid check should do nothing
    properties.IfLinearGradientBlurInvalid();
    EXPECT_EQ(properties.GetLinearGradientBlurPara(), nullptr);

    auto para = std::make_shared<RSLinearGradientBlurPara>(1.f,
        std::vector<std::pair<float, float>> { { 0.f, 1.f } }, GradientDirection::LEFT);
    properties.SetLinearGradientBlurPara(para);
    EXPECT_EQ(properties.GetLinearGradientBlurPara(), para);
    // valid radius branch, para is kept
    properties.IfLinearGradientBlurInvalid();
    EXPECT_EQ(properties.GetLinearGradientBlurPara(), para);

    auto invalidPara = std::make_shared<RSLinearGradientBlurPara>(-1.f,
        std::vector<std::pair<float, float>> { { 0.f, 1.f } }, GradientDirection::LEFT);
    properties.SetLinearGradientBlurPara(invalidPara);
    properties.IfLinearGradientBlurInvalid();
    EXPECT_EQ(properties.GetLinearGradientBlurPara(), nullptr);
}

/**
 * @tc.name: GreyCoef001
 * @tc.desc: test SetGreyCoef/GetGreyCoef/IsGreyCoefNeedUpdate/SetGreyCoefNeedUpdate with and without filter effect
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectPropertiesTest, GreyCoef001, TestSize.Level1)
{
    RSEffectProperties properties;
    EXPECT_FALSE(properties.GetGreyCoef().has_value());
    EXPECT_FALSE(properties.IsGreyCoefNeedUpdate());
    // no filter effect params, update flag setter does nothing
    properties.SetGreyCoefNeedUpdate(true);
    EXPECT_FALSE(properties.IsGreyCoefNeedUpdate());

    Vector2f coef { 50.f, 50.f };
    properties.SetGreyCoef(coef);
    EXPECT_EQ(properties.GetGreyCoef(), coef);
    EXPECT_TRUE(properties.IsGreyCoefNeedUpdate());
    properties.SetGreyCoefNeedUpdate(false);
    EXPECT_FALSE(properties.IsGreyCoefNeedUpdate());
    properties.SetGreyCoef(std::nullopt);
    EXPECT_FALSE(properties.GetGreyCoef().has_value());
    EXPECT_TRUE(properties.IsGreyCoefNeedUpdate());
}

/**
 * @tc.name: CheckGreyCoef001
 * @tc.desc: test CheckGreyCoef branches: no value, valid value, invalid x, invalid y and both zero
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectPropertiesTest, CheckGreyCoef001, TestSize.Level1)
{
    RSEffectProperties properties;
    // no value branch
    properties.CheckGreyCoef();
    EXPECT_FALSE(properties.GetGreyCoef().has_value());

    // valid value branch
    properties.SetGreyCoef(Vector2f { 50.f, 50.f });
    properties.CheckGreyCoef();
    EXPECT_TRUE(properties.GetGreyCoef().has_value());

    // invalid x branch
    properties.SetGreyCoef(Vector2f { -1.f, 50.f });
    properties.CheckGreyCoef();
    EXPECT_FALSE(properties.GetGreyCoef().has_value());

    // invalid y branch
    properties.SetGreyCoef(Vector2f { 50.f, 128.f });
    properties.CheckGreyCoef();
    EXPECT_FALSE(properties.GetGreyCoef().has_value());

    // both zero branch
    properties.SetGreyCoef(Vector2f { 0.f, 0.f });
    properties.CheckGreyCoef();
    EXPECT_FALSE(properties.GetGreyCoef().has_value());
}

/**
 * @tc.name: DynamicDimDegree001
 * @tc.desc: test SetDynamicDimDegree/GetDynamicDimDegree and IsDynamicDimValid branches
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectPropertiesTest, DynamicDimDegree001, TestSize.Level1)
{
    RSEffectProperties properties;
    EXPECT_FALSE(properties.GetDynamicDimDegree().has_value());
    EXPECT_FALSE(properties.IsDynamicDimValid());

    properties.SetDynamicDimDegree(0.5f);
    EXPECT_EQ(properties.GetDynamicDimDegree(), 0.5f);
    EXPECT_TRUE(properties.IsDynamicDimValid());

    properties.SetDynamicDimDegree(1.5f);
    EXPECT_FALSE(properties.IsDynamicDimValid());
    properties.SetDynamicDimDegree(-0.5f);
    EXPECT_FALSE(properties.IsDynamicDimValid());
    properties.SetDynamicDimDegree(std::nullopt);
    EXPECT_FALSE(properties.IsDynamicDimValid());
}

/**
 * @tc.name: DynamicLightUp001
 * @tc.desc: test SetDynamicLightUpRate/SetDynamicLightUpDegree/Get/IsDynamicLightUpValid branches
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectPropertiesTest, DynamicLightUp001, TestSize.Level1)
{
    RSEffectProperties properties;
    EXPECT_EQ(properties.GetDynamicLightUpRate(), 0.f);
    EXPECT_EQ(properties.GetDynamicLightUpDegree(), 0.f);
    EXPECT_FALSE(properties.IsDynamicLightUpValid());
    // nullopt rate without para, should not create para
    properties.SetDynamicLightUpRate(std::nullopt);
    EXPECT_EQ(properties.GetDynamicLightUpRate(), 0.f);
    properties.SetDynamicLightUpDegree(std::nullopt);
    EXPECT_EQ(properties.GetDynamicLightUpDegree(), 0.f);

    properties.SetDynamicLightUpRate(0.5f);
    properties.SetDynamicLightUpDegree(0.5f);
    EXPECT_EQ(properties.GetDynamicLightUpRate(), 0.5f);
    EXPECT_EQ(properties.GetDynamicLightUpDegree(), 0.5f);
    EXPECT_TRUE(properties.IsDynamicLightUpValid());

    // rate reset branch, para exists
    properties.SetDynamicLightUpRate(std::nullopt);
    EXPECT_EQ(properties.GetDynamicLightUpRate(), 0.f);
    EXPECT_FALSE(properties.IsDynamicLightUpValid());

    // degree out of range branches
    properties.SetDynamicLightUpRate(0.5f);
    properties.SetDynamicLightUpDegree(2.0f);
    EXPECT_FALSE(properties.IsDynamicLightUpValid());
    properties.SetDynamicLightUpDegree(-1.5f);
    EXPECT_FALSE(properties.IsDynamicLightUpValid());
    // degree reset branch
    properties.SetDynamicLightUpDegree(std::nullopt);
    EXPECT_EQ(properties.GetDynamicLightUpDegree(), 0.f);
    EXPECT_TRUE(properties.IsDynamicLightUpValid());
}

/**
 * @tc.name: ColorParamsSetNGet001
 * @tc.desc: test Set/Get of GrayScale, Brightness, Contrast, Saturate, Sepia, Invert, HueRotate, AiInvert,
 *           SystemBarEffect, ColorAdaptive and GetColorFilter with and without filter effect params
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectPropertiesTest, ColorParamsSetNGet001, TestSize.Level1)
{
    RSEffectProperties properties;
    EXPECT_FALSE(properties.GetGrayScale().has_value());
    EXPECT_FALSE(properties.GetBrightness().has_value());
    EXPECT_FALSE(properties.GetContrast().has_value());
    EXPECT_FALSE(properties.GetSaturate().has_value());
    EXPECT_FALSE(properties.GetSepia().has_value());
    EXPECT_FALSE(properties.GetInvert().has_value());
    EXPECT_FALSE(properties.GetHueRotate().has_value());
    EXPECT_FALSE(properties.GetAiInvert().has_value());
    EXPECT_FALSE(properties.GetSystemBarEffect());
    EXPECT_FALSE(properties.GetColorAdaptive());
    EXPECT_EQ(properties.GetColorFilter(), nullptr);

    std::optional<float> value = 0.5f;
    properties.SetGrayScale(value);
    properties.SetBrightness(value);
    properties.SetContrast(value);
    properties.SetSaturate(value);
    properties.SetSepia(value);
    properties.SetInvert(value);
    properties.SetHueRotate(value);
    properties.SetAiInvert(Vector4f { 1.f, 1.f, 1.f, 1.f });
    properties.SetSystemBarEffect(true);
    properties.SetColorAdaptive(true);
    EXPECT_EQ(properties.GetGrayScale(), value);
    EXPECT_EQ(properties.GetBrightness(), value);
    EXPECT_EQ(properties.GetContrast(), value);
    EXPECT_EQ(properties.GetSaturate(), value);
    EXPECT_EQ(properties.GetSepia(), value);
    EXPECT_EQ(properties.GetInvert(), value);
    EXPECT_EQ(properties.GetHueRotate(), value);
    EXPECT_TRUE(properties.GetAiInvert().has_value());
    EXPECT_TRUE(properties.GetSystemBarEffect());
    EXPECT_TRUE(properties.GetColorAdaptive());
}

/**
 * @tc.name: ColorParamsSetNGet002
 * @tc.desc: test Set/Get of GrayScale, Brightness, Contrast, Saturate, Sepia, Invert, HueRotate, AiInvert,
 *           SystemBarEffect, ColorAdaptive and GetColorFilter with and without filter effect params
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectPropertiesTest, ColorParamsSetNGet002, TestSize.Level1)
{
    RSEffectProperties properties;
    properties.SetGrayScale(std::nullopt);
    properties.SetBrightness(std::nullopt);
    properties.SetContrast(std::nullopt);
    properties.SetSaturate(std::nullopt);
    properties.SetSepia(std::nullopt);
    properties.SetInvert(std::nullopt);
    properties.SetHueRotate(std::nullopt);
    properties.SetAiInvert(std::nullopt);
    properties.SetSystemBarEffect(false);
    properties.SetColorAdaptive(false);
    EXPECT_FALSE(properties.GetGrayScale().has_value());
    EXPECT_FALSE(properties.GetBrightness().has_value());
    EXPECT_FALSE(properties.GetContrast().has_value());
    EXPECT_FALSE(properties.GetSaturate().has_value());
    EXPECT_FALSE(properties.GetSepia().has_value());
    EXPECT_FALSE(properties.GetInvert().has_value());
    EXPECT_FALSE(properties.GetHueRotate().has_value());
    EXPECT_FALSE(properties.GetAiInvert().has_value());
    EXPECT_FALSE(properties.GetSystemBarEffect());
    EXPECT_FALSE(properties.GetColorAdaptive());
}

/**
 * @tc.name: GenerateColorFilter001
 * @tc.desc: test GenerateColorFilter with no color params
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectPropertiesTest, GenerateColorFilter001, TestSize.Level1)
{
    RSEffectProperties properties;
    EXPECT_FALSE(properties.GenerateColorFilter());
}

/**
 * @tc.name: GenerateColorFilter002
 * @tc.desc: test GenerateColorFilter with each single color param, including skip branches
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectPropertiesTest, GenerateColorFilter002, TestSize.Level1)
{
    // grayScale branch
    {
        RSEffectProperties properties;
        properties.SetGrayScale(0.5f);
        EXPECT_TRUE(properties.GenerateColorFilter());
        EXPECT_NE(properties.GetColorFilter(), nullptr);
    }
    // brightness skip branch (NearEqual 1.0) and generate branch
    {
        RSEffectProperties properties;
        properties.SetBrightness(1.0f);
        EXPECT_TRUE(properties.GenerateColorFilter());
        EXPECT_EQ(properties.GetColorFilter(), nullptr);
        properties.SetBrightness(1.5f);
        EXPECT_TRUE(properties.GenerateColorFilter());
        EXPECT_NE(properties.GetColorFilter(), nullptr);
    }
    // contrast skip branch and generate branch
    {
        RSEffectProperties properties;
        properties.SetContrast(1.0f);
        EXPECT_TRUE(properties.GenerateColorFilter());
        EXPECT_EQ(properties.GetColorFilter(), nullptr);
        properties.SetContrast(0.5f);
        EXPECT_TRUE(properties.GenerateColorFilter());
        EXPECT_NE(properties.GetColorFilter(), nullptr);
    }
    // saturate skip branches (NearEqual 1.0 and negative) and generate branch
    {
        RSEffectProperties properties;
        properties.SetSaturate(1.0f);
        EXPECT_TRUE(properties.GenerateColorFilter());
        EXPECT_EQ(properties.GetColorFilter(), nullptr);
        properties.SetSaturate(-1.0f);
        EXPECT_TRUE(properties.GenerateColorFilter());
        EXPECT_EQ(properties.GetColorFilter(), nullptr);
        properties.SetSaturate(0.5f);
        EXPECT_TRUE(properties.GenerateColorFilter());
        EXPECT_NE(properties.GetColorFilter(), nullptr);
    }
}

/**
 * @tc.name: GenerateColorFilter003
 * @tc.desc: test GenerateColorFilter with each single color param, including skip branches
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectPropertiesTest, GenerateColorFilter003, TestSize.Level1)
{
    // sepia skip branch and generate branch
    {
        RSEffectProperties properties;
        properties.SetSepia(0.0f);
        EXPECT_TRUE(properties.GenerateColorFilter());
        EXPECT_EQ(properties.GetColorFilter(), nullptr);
        properties.SetSepia(0.5f);
        EXPECT_TRUE(properties.GenerateColorFilter());
        EXPECT_NE(properties.GetColorFilter(), nullptr);
    }
    // invert skip branch, generate branch and clamp branch
    {
        RSEffectProperties properties;
        properties.SetInvert(0.0f);
        EXPECT_TRUE(properties.GenerateColorFilter());
        EXPECT_EQ(properties.GetColorFilter(), nullptr);
        properties.SetInvert(0.5f);
        EXPECT_TRUE(properties.GenerateColorFilter());
        EXPECT_NE(properties.GetColorFilter(), nullptr);
        properties.SetInvert(2.0f);
        EXPECT_TRUE(properties.GenerateColorFilter());
        EXPECT_NE(properties.GetColorFilter(), nullptr);
    }
    // hueRotate skip branch and generate branch
    {
        RSEffectProperties properties;
        properties.SetHueRotate(0.0f);
        EXPECT_TRUE(properties.GenerateColorFilter());
        EXPECT_EQ(properties.GetColorFilter(), nullptr);
        properties.SetHueRotate(100.0f);
        EXPECT_TRUE(properties.GenerateColorFilter());
        EXPECT_NE(properties.GetColorFilter(), nullptr);
    }
    // colorBlend skip branch (transparent) and generate branch
    {
        RSEffectProperties properties;
        properties.SetColorBlend(RgbPalette::Transparent());
        EXPECT_TRUE(properties.GenerateColorFilter());
        EXPECT_EQ(properties.GetColorFilter(), nullptr);
        properties.SetColorBlend(RgbPalette::Red());
        EXPECT_TRUE(properties.GenerateColorFilter());
        EXPECT_NE(properties.GetColorFilter(), nullptr);
    }
}

/**
 * @tc.name: GenerateColorFilter004
 * @tc.desc: test GenerateColorFilter with all color params set, covering compose with existing filter
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectPropertiesTest, GenerateColorFilter004, TestSize.Level1)
{
    RSEffectProperties properties;
    properties.SetGrayScale(0.5f);
    properties.SetBrightness(1.5f);
    properties.SetContrast(0.5f);
    properties.SetSaturate(0.5f);
    properties.SetSepia(0.5f);
    properties.SetInvert(0.5f);
    properties.SetHueRotate(100.0f);
    properties.SetColorBlend(RgbPalette::Red());
    EXPECT_TRUE(properties.GenerateColorFilter());
    EXPECT_NE(properties.GetColorFilter(), nullptr);

    // second generation resets filter first, grayScale branch composes with previous filter
    EXPECT_TRUE(properties.GenerateColorFilter());
    EXPECT_NE(properties.GetColorFilter(), nullptr);
}

/**
 * @tc.name: GenerateColorFilter005
 * @tc.desc: test GenerateColorFilter hueRotate types 0/1/2 and degree wrap-around loop
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectPropertiesTest, GenerateColorFilter005, TestSize.Level1)
{
    RSEffectProperties properties;
    // type 0
    properties.SetHueRotate(100.0f);
    EXPECT_TRUE(properties.GenerateColorFilter());
    EXPECT_NE(properties.GetColorFilter(), nullptr);
    // type 1
    properties.SetHueRotate(200.0f);
    EXPECT_TRUE(properties.GenerateColorFilter());
    EXPECT_NE(properties.GetColorFilter(), nullptr);
    // type 2
    properties.SetHueRotate(300.0f);
    EXPECT_TRUE(properties.GenerateColorFilter());
    EXPECT_NE(properties.GetColorFilter(), nullptr);
    // while loop subtracts 360
    properties.SetHueRotate(720.0f);
    EXPECT_TRUE(properties.GenerateColorFilter());
    EXPECT_NE(properties.GetColorFilter(), nullptr);
}

/**
 * @tc.name: BackgroundBlur001
 * @tc.desc: test background blur radius/saturation/brightness/maskColor/colorMode/radiusX/radiusY set/get and
 *           validity branches
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectPropertiesTest, BackgroundBlur001, TestSize.Level1)
{
    RSEffectProperties properties;
    EXPECT_EQ(properties.GetBackgroundBlurRadius(), 0.f);
    EXPECT_FALSE(properties.IsBackgroundBlurRadiusValid());
    EXPECT_EQ(properties.GetBackgroundBlurSaturation(), 1.f);
    EXPECT_FALSE(properties.IsBackgroundBlurSaturationValid());
    EXPECT_EQ(properties.GetBackgroundBlurBrightness(), 1.f);
    EXPECT_FALSE(properties.IsBackgroundBlurBrightnessValid());
    EXPECT_EQ(properties.GetBackgroundBlurMaskColor(), RSColor());
    EXPECT_FALSE(properties.IsBackgroundBlurMaskColorValid());
    EXPECT_EQ(properties.GetBackgroundBlurColorMode(), BLUR_COLOR_MODE::DEFAULT);
    EXPECT_EQ(properties.GetBackgroundBlurRadiusX(), 0.f);
    EXPECT_FALSE(properties.IsBackgroundBlurRadiusXValid());
    EXPECT_EQ(properties.GetBackgroundBlurRadiusY(), 0.f);
    EXPECT_FALSE(properties.IsBackgroundBlurRadiusYValid());

    // first setter call creates para, second call reuses it
    properties.SetBackgroundBlurRadius(1.f);
    properties.SetBackgroundBlurRadius(2.f);
    EXPECT_EQ(properties.GetBackgroundBlurRadius(), 2.f);
    EXPECT_TRUE(properties.IsBackgroundBlurRadiusValid());

    properties.SetBackgroundBlurSaturation(0.5f);
    properties.SetBackgroundBlurSaturation(0.8f);
    EXPECT_EQ(properties.GetBackgroundBlurSaturation(), 0.8f);
    EXPECT_TRUE(properties.IsBackgroundBlurSaturationValid());
    properties.SetBackgroundBlurSaturation(-0.5f);
    EXPECT_FALSE(properties.IsBackgroundBlurSaturationValid());

    properties.SetBackgroundBlurBrightness(0.5f);
    properties.SetBackgroundBlurBrightness(0.8f);
    EXPECT_EQ(properties.GetBackgroundBlurBrightness(), 0.8f);
    EXPECT_TRUE(properties.IsBackgroundBlurBrightnessValid());
    properties.SetBackgroundBlurBrightness(-0.5f);
    EXPECT_FALSE(properties.IsBackgroundBlurBrightnessValid());

    Color maskColor = Color::FromArgbInt(0xFF0000FF);
    properties.SetBackgroundBlurMaskColor(maskColor);
    properties.SetBackgroundBlurMaskColor(maskColor);
    EXPECT_EQ(properties.GetBackgroundBlurMaskColor(), maskColor);
    EXPECT_TRUE(properties.IsBackgroundBlurMaskColorValid());

    properties.SetBackgroundBlurColorMode(BLUR_COLOR_MODE::AVERAGE);
    properties.SetBackgroundBlurColorMode(BLUR_COLOR_MODE::AVERAGE);
    EXPECT_EQ(properties.GetBackgroundBlurColorMode(), BLUR_COLOR_MODE::AVERAGE);

    properties.SetBackgroundBlurRadiusX(1.f);
    properties.SetBackgroundBlurRadiusX(2.f);
    EXPECT_EQ(properties.GetBackgroundBlurRadiusX(), 2.f);
    EXPECT_TRUE(properties.IsBackgroundBlurRadiusXValid());

    properties.SetBackgroundBlurRadiusY(1.f);
    properties.SetBackgroundBlurRadiusY(2.f);
    EXPECT_EQ(properties.GetBackgroundBlurRadiusY(), 2.f);
    EXPECT_TRUE(properties.IsBackgroundBlurRadiusYValid());
}

/**
 * @tc.name: BgBlurDisableSystemAdaptation001
 * @tc.desc: test SetBgBlurDisableSystemAdaptation/GetBgBlurDisableSystemAdaptation branches
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectPropertiesTest, BgBlurDisableSystemAdaptation001, TestSize.Level1)
{
    RSEffectProperties properties;
    EXPECT_TRUE(properties.GetBgBlurDisableSystemAdaptation());
    properties.SetBgBlurDisableSystemAdaptation(false);
    EXPECT_FALSE(properties.GetBgBlurDisableSystemAdaptation());
    properties.SetBgBlurDisableSystemAdaptation(true);
    EXPECT_TRUE(properties.GetBgBlurDisableSystemAdaptation());
}

/**
 * @tc.name: ForegroundBlur001
 * @tc.desc: test foreground blur radius/saturation/brightness/maskColor/colorMode/radiusX/radiusY set/get and
 *           validity branches
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectPropertiesTest, ForegroundBlur001, TestSize.Level1)
{
    RSEffectProperties properties;
    EXPECT_EQ(properties.GetForegroundBlurRadius(), 0.f);
    EXPECT_FALSE(properties.IsForegroundBlurRadiusValid());
    EXPECT_EQ(properties.GetForegroundBlurSaturation(), 1.f);
    EXPECT_TRUE(properties.IsForegroundBlurSaturationValid());
    EXPECT_EQ(properties.GetForegroundBlurBrightness(), 1.f);
    EXPECT_TRUE(properties.IsForegroundBlurBrightnessValid());
    EXPECT_EQ(properties.GetForegroundBlurMaskColor(), RSColor());
    EXPECT_FALSE(properties.IsForegroundBlurMaskColorValid());
    EXPECT_EQ(properties.GetForegroundBlurColorMode(), BLUR_COLOR_MODE::DEFAULT);
    EXPECT_EQ(properties.GetForegroundBlurRadiusX(), 0.f);
    EXPECT_FALSE(properties.IsForegroundBlurRadiusXValid());
    EXPECT_EQ(properties.GetForegroundBlurRadiusY(), 0.f);
    EXPECT_FALSE(properties.IsForegroundBlurRadiusYValid());
    EXPECT_TRUE(properties.GetFgBlurDisableSystemAdaptation());

    properties.SetForegroundBlurRadius(1.f);
    properties.SetForegroundBlurRadius(2.f);
    EXPECT_EQ(properties.GetForegroundBlurRadius(), 2.f);
    EXPECT_TRUE(properties.IsForegroundBlurRadiusValid());

    properties.SetForegroundBlurSaturation(1.5f);
    properties.SetForegroundBlurSaturation(1.8f);
    EXPECT_EQ(properties.GetForegroundBlurSaturation(), 1.8f);
    EXPECT_TRUE(properties.IsForegroundBlurSaturationValid());
    properties.SetForegroundBlurSaturation(0.8f);
    EXPECT_FALSE(properties.IsForegroundBlurSaturationValid());

    properties.SetForegroundBlurBrightness(1.5f);
    properties.SetForegroundBlurBrightness(1.8f);
    EXPECT_EQ(properties.GetForegroundBlurBrightness(), 1.8f);
    EXPECT_TRUE(properties.IsForegroundBlurBrightnessValid());
    properties.SetForegroundBlurBrightness(0.8f);
    EXPECT_FALSE(properties.IsForegroundBlurBrightnessValid());
}

/**
 * @tc.name: ForegroundBlur002
 * @tc.desc: test foreground blur radius/saturation/brightness/maskColor/colorMode/radiusX/radiusY set/get and
 *           validity branches
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectPropertiesTest, ForegroundBlur002, TestSize.Level1)
{
    RSEffectProperties properties;
    Color maskColor = Color::FromArgbInt(0xFF00FF00);
    properties.SetForegroundBlurMaskColor(maskColor);
    properties.SetForegroundBlurMaskColor(maskColor);
    EXPECT_EQ(properties.GetForegroundBlurMaskColor(), maskColor);
    EXPECT_TRUE(properties.IsForegroundBlurMaskColorValid());

    properties.SetForegroundBlurColorMode(BLUR_COLOR_MODE::AVERAGE);
    properties.SetForegroundBlurColorMode(BLUR_COLOR_MODE::AVERAGE);
    EXPECT_EQ(properties.GetForegroundBlurColorMode(), BLUR_COLOR_MODE::AVERAGE);

    properties.SetForegroundBlurRadiusX(1.f);
    properties.SetForegroundBlurRadiusX(2.f);
    EXPECT_EQ(properties.GetForegroundBlurRadiusX(), 2.f);
    EXPECT_TRUE(properties.IsForegroundBlurRadiusXValid());

    properties.SetForegroundBlurRadiusY(1.f);
    properties.SetForegroundBlurRadiusY(2.f);
    EXPECT_EQ(properties.GetForegroundBlurRadiusY(), 2.f);
    EXPECT_TRUE(properties.IsForegroundBlurRadiusYValid());

    properties.SetFgBlurDisableSystemAdaptation(false);
    EXPECT_FALSE(properties.GetFgBlurDisableSystemAdaptation());
    properties.SetFgBlurDisableSystemAdaptation(true);
    EXPECT_TRUE(properties.GetFgBlurDisableSystemAdaptation());
}

/**
 * @tc.name: MaterialFilterValid001
 * @tc.desc: test IsBackgroundMaterialFilterValid and IsForegroundMaterialFilterValid branches
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectPropertiesTest, MaterialFilterValid001, TestSize.Level1)
{
    RSEffectProperties properties;
    EXPECT_FALSE(properties.IsBackgroundMaterialFilterValid());
    EXPECT_FALSE(properties.IsForegroundMaterialFilterValid());

    properties.SetBackgroundBlurRadius(1.f);
    EXPECT_TRUE(properties.IsBackgroundMaterialFilterValid());

    RSEffectProperties brightnessProperties;
    brightnessProperties.SetBackgroundBlurBrightness(0.f);
    EXPECT_TRUE(brightnessProperties.IsBackgroundMaterialFilterValid());

    RSEffectProperties saturationProperties;
    saturationProperties.SetBackgroundBlurSaturation(0.5f);
    EXPECT_TRUE(saturationProperties.IsBackgroundMaterialFilterValid());

    RSEffectProperties fgProperties;
    fgProperties.SetForegroundBlurRadius(1.f);
    EXPECT_TRUE(fgProperties.IsForegroundMaterialFilterValid());
}

/**
 * @tc.name: AlwaysSnapshot001
 * @tc.desc: test SetAlwaysSnapshot/GetAlwaysSnapshot with and without filter effect params
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectPropertiesTest, AlwaysSnapshot001, TestSize.Level1)
{
    RSEffectProperties properties;
    EXPECT_FALSE(properties.GetAlwaysSnapshot());
    properties.SetAlwaysSnapshot(true);
    EXPECT_TRUE(properties.GetAlwaysSnapshot());
    properties.SetAlwaysSnapshot(false);
    EXPECT_FALSE(properties.GetAlwaysSnapshot());
}

/**
 * @tc.name: NGRenderFilter001
 * @tc.desc: test Set/Get of background/foreground/material/compositing NG filters
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectPropertiesTest, NGRenderFilter001, TestSize.Level1)
{
    RSEffectProperties properties;
    EXPECT_EQ(properties.GetBackgroundNGFilter(), nullptr);
    EXPECT_EQ(properties.GetForegroundNGFilter(), nullptr);
    EXPECT_EQ(properties.GetMaterialNGFilter(), nullptr);
    EXPECT_EQ(properties.GetCompositingNGFilter(), nullptr);

    auto bgFilter = RSNGRenderFilterBase::Create(RSNGEffectType::BLUR);
    properties.SetBackgroundNGFilter(bgFilter);
    EXPECT_EQ(properties.GetBackgroundNGFilter(), bgFilter);
    auto fgFilter = RSNGRenderFilterBase::Create(RSNGEffectType::BLUR);
    properties.SetForegroundNGFilter(fgFilter);
    EXPECT_EQ(properties.GetForegroundNGFilter(), fgFilter);
    auto mtFilter = RSNGRenderFilterBase::Create(RSNGEffectType::BLUR);
    properties.SetMaterialNGFilter(mtFilter);
    EXPECT_EQ(properties.GetMaterialNGFilter(), mtFilter);
    auto cgFilter = RSNGRenderFilterBase::Create(RSNGEffectType::BLUR);
    properties.SetCompositingNGFilter(cgFilter);
    EXPECT_EQ(properties.GetCompositingNGFilter(), cgFilter);

    properties.SetBackgroundNGFilter(nullptr);
    properties.SetForegroundNGFilter(nullptr);
    properties.SetMaterialNGFilter(nullptr);
    properties.SetCompositingNGFilter(nullptr);
    EXPECT_EQ(properties.GetBackgroundNGFilter(), nullptr);
    EXPECT_EQ(properties.GetForegroundNGFilter(), nullptr);
    EXPECT_EQ(properties.GetMaterialNGFilter(), nullptr);
    EXPECT_EQ(properties.GetCompositingNGFilter(), nullptr);
}

/**
 * @tc.name: MaterialFilter001
 * @tc.desc: test GetMaterialFilter/SetMaterialFilter and GenerateMaterialFilter branches
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectPropertiesTest, MaterialFilter001, TestSize.Level1)
{
    RSEffectProperties properties;
    EXPECT_EQ(properties.GetMaterialFilter(), nullptr);

    // no NG filter branch
    properties.GenerateMaterialFilter();
    EXPECT_EQ(properties.GetMaterialFilter(), nullptr);

    // non frosted glass NG filter branch
    properties.SetMaterialNGFilter(RSNGRenderFilterBase::Create(RSNGEffectType::BLUR));
    properties.GenerateMaterialFilter();
    ASSERT_NE(properties.GetMaterialFilter(), nullptr);
    EXPECT_EQ(properties.GetMaterialFilter()->GetFilterType(), RSFilter::COMPOUND_EFFECT);

    // frosted glass NG filter branch
    properties.SetMaterialNGFilter(RSNGRenderFilterBase::Create(RSNGEffectType::FROSTED_GLASS));
    properties.GenerateMaterialFilter();
    ASSERT_NE(properties.GetMaterialFilter(), nullptr);
    EXPECT_EQ(properties.GetMaterialFilter()->GetFilterType(), RSFilter::COMPOUND_EFFECT);

    // SetMaterialFilter overrides generated filter
    auto filter = std::make_shared<RSDrawingFilter>();
    properties.SetMaterialFilter(filter);
    EXPECT_EQ(properties.GetMaterialFilter(), filter);
}

/**
 * @tc.name: PixelStretch001
 * @tc.desc: test SetPixelStretch/GetPixelStretch with value, nullopt and without para branches
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectPropertiesTest, PixelStretch001, TestSize.Level1)
{
    RSEffectProperties properties;
    EXPECT_EQ(properties.GetPixelStretch(), Vector4f());

    // nullopt without para
    properties.SetPixelStretch(std::nullopt);
    EXPECT_EQ(properties.GetPixelStretch(), Vector4f());

    Vector4f stretch { 1.f, 2.f, 3.f, 4.f };
    properties.SetPixelStretch(stretch);
    EXPECT_EQ(properties.GetPixelStretch(), stretch);
    // second set reuses para
    properties.SetPixelStretch(stretch);
    EXPECT_EQ(properties.GetPixelStretch(), stretch);

    // nullopt with para resets size
    properties.SetPixelStretch(std::nullopt);
    EXPECT_EQ(properties.GetPixelStretch(), Vector4f());
}

/**
 * @tc.name: PixelStretchPercent001
 * @tc.desc: test SetPixelStretchPercent/GetPixelStretchPercent/ConvertPixelStretchPercentToSize branches
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectPropertiesTest, PixelStretchPercent001, TestSize.Level1)
{
    RSEffectProperties properties;
    EXPECT_EQ(properties.GetPixelStretchPercent(), Vector4f());

    // nullopt without para
    properties.SetPixelStretchPercent(std::nullopt);
    EXPECT_EQ(properties.GetPixelStretchPercent(), Vector4f());

    // convert without para creates para
    properties.ConvertPixelStretchPercentToSize(100.f, 200.f);
    EXPECT_EQ(properties.GetPixelStretch(), Vector4f());

    Vector4f percent { 0.5f, 0.5f, 0.5f, 0.5f };
    properties.SetPixelStretchPercent(percent);
    EXPECT_EQ(properties.GetPixelStretchPercent(), percent);
    properties.SetPixelStretchPercent(percent);
    EXPECT_EQ(properties.GetPixelStretchPercent(), percent);

    properties.ConvertPixelStretchPercentToSize(100.f, 200.f);
    EXPECT_EQ(properties.GetPixelStretch(), Vector4f(50.f, 100.f, 50.f, 100.f));

    // nullopt with para resets percent
    properties.SetPixelStretchPercent(std::nullopt);
    EXPECT_EQ(properties.GetPixelStretchPercent(), Vector4f());
}

/**
 * @tc.name: PixelStretchTileMode001
 * @tc.desc: test SetPixelStretchTileMode clamp branches and GetPixelStretchTileMode
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectPropertiesTest, PixelStretchTileMode001, TestSize.Level1)
{
    RSEffectProperties properties;
    EXPECT_EQ(properties.GetPixelStretchTileMode(), 0);

    properties.SetPixelStretchTileMode(100);
    EXPECT_EQ(properties.GetPixelStretchTileMode(), static_cast<int>(Drawing::TileMode::DECAL));
    properties.SetPixelStretchTileMode(-1);
    EXPECT_EQ(properties.GetPixelStretchTileMode(), static_cast<int>(Drawing::TileMode::CLAMP));
    properties.SetPixelStretchTileMode(static_cast<int>(Drawing::TileMode::MIRROR));
    EXPECT_EQ(properties.GetPixelStretchTileMode(), static_cast<int>(Drawing::TileMode::MIRROR));
}

/**
 * @tc.name: ResetPixelStretchPara001
 * @tc.desc: test ResetPixelStretchPara with and without filter effect params
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectPropertiesTest, ResetPixelStretchPara001, TestSize.Level1)
{
    RSEffectProperties properties;
    // without filter effect params
    properties.ResetPixelStretchPara();
    EXPECT_EQ(properties.GetPixelStretch(), Vector4f());

    properties.SetPixelStretch(Vector4f { 1.f, 1.f, 1.f, 1.f });
    properties.ResetPixelStretchPara();
    EXPECT_EQ(properties.GetPixelStretch(), Vector4f());
}

/**
 * @tc.name: Distortion001
 * @tc.desc: test SetDistortionK/GetDistortionK/SetDistortionDirty/GetDistortionDirty/IsDistortionKValid branches
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectPropertiesTest, Distortion001, TestSize.Level1)
{
    RSEffectProperties properties;
    EXPECT_FALSE(properties.GetDistortionK().has_value());
    EXPECT_FALSE(properties.IsDistortionKValid());
    EXPECT_FALSE(properties.GetDistortionDirty());

    // nullopt without para
    properties.SetDistortionK(std::nullopt);
    EXPECT_FALSE(properties.GetDistortionK().has_value());

    // valid range, dirty is true
    properties.SetDistortionK(0.5f);
    EXPECT_EQ(properties.GetDistortionK(), 0.5f);
    EXPECT_TRUE(properties.GetDistortionDirty());
    EXPECT_TRUE(properties.IsDistortionKValid());

    // out of valid range, dirty is false
    properties.SetDistortionK(2.0f);
    EXPECT_FALSE(properties.GetDistortionDirty());
    EXPECT_FALSE(properties.IsDistortionKValid());
    properties.SetDistortionK(-2.0f);
    EXPECT_FALSE(properties.IsDistortionKValid());

    // nullopt with para resets
    properties.SetDistortionK(std::nullopt);
    EXPECT_EQ(properties.GetDistortionK(), 0.f);
    EXPECT_FALSE(properties.GetDistortionDirty());
    EXPECT_TRUE(properties.IsDistortionKValid());

    // SetDistortionDirty with existing para
    properties.SetDistortionK(0.5f);
    properties.SetDistortionDirty(false);
    EXPECT_FALSE(properties.GetDistortionDirty());
    properties.SetDistortionDirty(true);
    EXPECT_TRUE(properties.GetDistortionDirty());
}

/**
 * @tc.name: Attraction001
 * @tc.desc: test SetAttractionFraction/GetAttractionFraction/SetAttractionDstPoint/GetAttractionDstPoint/
 *           IsAttractionValid/Get/SetAttractionEffectCurrentDirtyRegion branches
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectPropertiesTest, Attraction001, TestSize.Level1)
{
    RSEffectProperties properties;
    EXPECT_EQ(properties.GetAttractionFraction(), 0.f);
    EXPECT_EQ(properties.GetAttractionDstPoint(), Vector2f(0.f, 0.f));
    EXPECT_EQ(properties.GetAttractionEffectCurrentDirtyRegion(), RectI(0, 0, 0, 0));
    EXPECT_FALSE(properties.IsAttractionValid());

    // valid fraction
    EXPECT_TRUE(properties.SetAttractionFraction(0.5f));
    EXPECT_EQ(properties.GetAttractionFraction(), 0.5f);
    EXPECT_TRUE(properties.IsAttractionValid());

    // invalid fraction
    EXPECT_FALSE(properties.SetAttractionFraction(0.0005f));
    EXPECT_EQ(properties.GetAttractionFraction(), 0.0005f);
    EXPECT_FALSE(properties.IsAttractionValid());

    Vector2f dstPoint { 1.f, 2.f };
    properties.SetAttractionDstPoint(dstPoint);
    EXPECT_EQ(properties.GetAttractionDstPoint(), dstPoint);

    RectI dirtyRegion { 1, 2, 3, 4 };
    properties.SetAttractionEffectCurrentDirtyRegion(dirtyRegion);
    EXPECT_EQ(properties.GetAttractionEffectCurrentDirtyRegion(), dirtyRegion);
}

/**
 * @tc.name: ShadowParams001
 * @tc.desc: test all shadow setters creating shadow lazily and reusing it, plus GetShadow
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectPropertiesTest, ShadowParams001, TestSize.Level1)
{
    RSEffectProperties properties;
    EXPECT_FALSE(properties.GetShadow().has_value());

    Color color = Color::FromArgbInt(0xFF000000);
    properties.SetShadowColor(color);
    properties.SetShadowColor(color);
    EXPECT_EQ(properties.GetShadow()->GetColor(), color);

    properties.SetShadowOffsetX(1.f);
    properties.SetShadowOffsetX(2.f);
    EXPECT_EQ(properties.GetShadow()->GetOffsetX(), 2.f);

    properties.SetShadowOffsetY(1.f);
    properties.SetShadowOffsetY(2.f);
    EXPECT_EQ(properties.GetShadow()->GetOffsetY(), 2.f);

    properties.SetShadowElevation(1.f);
    properties.SetShadowElevation(2.f);
    EXPECT_EQ(properties.GetShadow()->GetElevation(), 2.f);

    properties.SetShadowRadius(1.f);
    properties.SetShadowRadius(2.f);
    EXPECT_EQ(properties.GetShadow()->GetRadius(), 2.f);

    auto path = std::make_shared<RSPath>();
    properties.SetShadowPath(path);
    properties.SetShadowPath(path);
    EXPECT_EQ(properties.GetShadow()->GetPath(), path);

    properties.SetShadowMask(1);
    properties.SetShadowMask(2);
    EXPECT_EQ(properties.GetShadow()->GetMask(), 2);

    properties.SetShadowIsFilled(true);
    properties.SetShadowIsFilled(false);
    EXPECT_EQ(properties.GetShadow()->GetIsFilled(), false);

    properties.SetShadowColorStrategy(SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_MAIN);
    properties.SetShadowColorStrategy(SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_AVERAGE);
    EXPECT_EQ(properties.GetShadow()->GetColorStrategy(), SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_AVERAGE);

    properties.SetShadowDisableSDFBlur(true);
    properties.SetShadowDisableSDFBlur(false);
    EXPECT_EQ(properties.GetShadow()->GetDisableSDFBlur(), false);
}

/**
 * @tc.name: ShadowValid001
 * @tc.desc: test IsShadowMaskValid and IsShadowValid branches
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectPropertiesTest, ShadowValid001, TestSize.Level1)
{
    RSEffectProperties properties;
    EXPECT_FALSE(properties.IsShadowMaskValid());
    EXPECT_FALSE(properties.IsShadowValid());

    // mask in valid range
    properties.SetShadowMask(SHADOW_MASK_STRATEGY::MASK_BLUR);
    EXPECT_TRUE(properties.IsShadowMaskValid());
    // mask out of valid range
    properties.SetShadowMask(SHADOW_MASK_STRATEGY::MASK_COLOR_BLUR + 1);
    EXPECT_FALSE(properties.IsShadowMaskValid());
    // mask none
    properties.SetShadowMask(SHADOW_MASK_STRATEGY::MASK_NONE);
    EXPECT_FALSE(properties.IsShadowMaskValid());

    // shadow invalid: radius -1 and elevation 0
    properties.SetShadowRadius(-1.f);
    EXPECT_FALSE(properties.IsShadowValid());
    // valid by radius
    properties.SetShadowRadius(1.f);
    EXPECT_TRUE(properties.IsShadowValid());
    // valid by elevation and non-zero alpha
    properties.SetShadowRadius(-1.f);
    properties.SetShadowElevation(5.f);
    properties.SetShadowColor(Color::FromArgbInt(0xFF000000));
    EXPECT_TRUE(properties.IsShadowValid());
}

/**
 * @tc.name: UseEffect001
 * @tc.desc: test SetUseEffect/GetUseEffect/SetUseEffectType/GetUseEffectType with clamp branches
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectPropertiesTest, UseEffect001, TestSize.Level1)
{
    RSEffectProperties properties;
    EXPECT_FALSE(properties.GetUseEffect());
    EXPECT_EQ(properties.GetUseEffectType(), 0);

    properties.SetUseEffect(true);
    EXPECT_TRUE(properties.GetUseEffect());

    properties.SetUseEffectType(1);
    EXPECT_EQ(properties.GetUseEffectType(), 1);
    // clamp upper branch
    properties.SetUseEffectType(100);
    EXPECT_EQ(properties.GetUseEffectType(), static_cast<int>(UseEffectType::MAX));
    // clamp lower branch
    properties.SetUseEffectType(-1);
    EXPECT_EQ(properties.GetUseEffectType(), 0);
}

/**
 * @tc.name: NodesEffect001
 * @tc.desc: test Set/Get of needDrawBehindWindow, haveEffectRegion, useShadowBatching, useUnion, unionSpacing
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectPropertiesTest, NodesEffect001, TestSize.Level1)
{
    RSEffectProperties properties;
    EXPECT_FALSE(properties.GetNeedDrawBehindWindow());
    EXPECT_FALSE(properties.GetHaveEffectRegion());
    EXPECT_FALSE(properties.GetUseShadowBatching());
    EXPECT_FALSE(properties.GetUseUnion());
    EXPECT_EQ(properties.GetUnionSpacing(), 0.f);

    properties.SetNeedDrawBehindWindow(true);
    properties.SetNeedDrawBehindWindow(false);
    EXPECT_FALSE(properties.GetNeedDrawBehindWindow());
    properties.SetNeedDrawBehindWindow(true);
    EXPECT_TRUE(properties.GetNeedDrawBehindWindow());

    properties.SetHaveEffectRegion(true);
    properties.SetHaveEffectRegion(false);
    EXPECT_FALSE(properties.GetHaveEffectRegion());
    properties.SetHaveEffectRegion(true);
    EXPECT_TRUE(properties.GetHaveEffectRegion());

    properties.SetUseShadowBatching(true);
    properties.SetUseShadowBatching(false);
    EXPECT_FALSE(properties.GetUseShadowBatching());
    properties.SetUseShadowBatching(true);
    EXPECT_TRUE(properties.GetUseShadowBatching());

    properties.SetUseUnion(true);
    properties.SetUseUnion(false);
    EXPECT_FALSE(properties.GetUseUnion());
    properties.SetUseUnion(true);
    EXPECT_TRUE(properties.GetUseUnion());

    properties.SetUnionSpacing(1.5f);
    EXPECT_EQ(properties.GetUnionSpacing(), 1.5f);
}

/**
 * @tc.name: FgBrightnessParams001
 * @tc.desc: test SetFgBrightnessParams/GetFgBrightnessParams/IsFgBrightnessValid branches
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectPropertiesTest, FgBrightnessParams001, TestSize.Level1)
{
    RSEffectProperties properties;
    EXPECT_FALSE(properties.GetFgBrightnessParams().has_value());
    EXPECT_FALSE(properties.IsFgBrightnessValid());

    properties.SetFgBrightnessParams(std::nullopt);
    EXPECT_FALSE(properties.GetFgBrightnessParams().has_value());

    RSDynamicBrightnessPara params;
    properties.SetFgBrightnessParams(params);
    EXPECT_EQ(properties.GetFgBrightnessParams(), params);
    // default fraction 1.0 is invalid
    EXPECT_FALSE(properties.IsFgBrightnessValid());

    params.fraction_ = 0.5f;
    properties.SetFgBrightnessParams(params);
    EXPECT_EQ(properties.GetFgBrightnessParams(), params);
    EXPECT_TRUE(properties.IsFgBrightnessValid());
}

/**
 * @tc.name: FgBrightnessMembers001
 * @tc.desc: test all FgBrightness member setters with lazy para creation and reuse
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectPropertiesTest, FgBrightnessMembers001, TestSize.Level1)
{
    RSEffectProperties properties;
    Vector4f rates { 1.f, 2.f, 3.f, 4.f };
    properties.SetFgBrightnessRates(rates);
    properties.SetFgBrightnessRates(rates);
    properties.SetFgBrightnessSaturation(0.5f);
    properties.SetFgBrightnessSaturation(0.6f);
    properties.SetFgBrightnessPosCoeff(rates);
    properties.SetFgBrightnessPosCoeff(rates);
    properties.SetFgBrightnessNegCoeff(rates);
    properties.SetFgBrightnessNegCoeff(rates);
    properties.SetFgBrightnessFract(0.5f);
    properties.SetFgBrightnessFract(0.6f);
    properties.SetFgBrightnessHdr(true);
    properties.SetFgBrightnessHdr(false);

    auto params = properties.GetFgBrightnessParams();
    ASSERT_TRUE(params.has_value());
    EXPECT_EQ(params->rates_, rates);
    EXPECT_EQ(params->saturation_, 0.6f);
    EXPECT_EQ(params->posCoeff_, rates);
    EXPECT_EQ(params->negCoeff_, rates);
    EXPECT_EQ(params->fraction_, 0.6f);
    EXPECT_EQ(params->enableHdr_, false);
}

/**
 * @tc.name: FgBrightnessEnableEDR001
 * @tc.desc: test GetFgBrightnessEnableEDR branches
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectPropertiesTest, FgBrightnessEnableEDR001, TestSize.Level1)
{
    RSEffectProperties properties;
    EXPECT_FALSE(properties.GetFgBrightnessEnableEDR());

    RSDynamicBrightnessPara params;
    params.enableHdr_ = false;
    params.fraction_ = 0.5f;
    properties.SetFgBrightnessParams(params);
    EXPECT_FALSE(properties.GetFgBrightnessEnableEDR());

    params.enableHdr_ = true;
    params.fraction_ = 1.0f;
    properties.SetFgBrightnessParams(params);
    EXPECT_FALSE(properties.GetFgBrightnessEnableEDR());

    params.enableHdr_ = true;
    params.fraction_ = 0.5f;
    properties.SetFgBrightnessParams(params);
    EXPECT_TRUE(properties.GetFgBrightnessEnableEDR());
}

/**
 * @tc.name: FgBrightnessDescription001
 * @tc.desc: test GetFgBrightnessDescription with and without params
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectPropertiesTest, FgBrightnessDescription001, TestSize.Level1)
{
    RSEffectProperties properties;
    EXPECT_EQ(properties.GetFgBrightnessDescription(), "fgBrightnessParams_ is nullopt");

    RSDynamicBrightnessPara params;
    params.fraction_ = 0.5f;
    properties.SetFgBrightnessParams(params);
    std::string description = properties.GetFgBrightnessDescription();
    EXPECT_NE(description.find("ForegroundBrightness"), std::string::npos);
    EXPECT_NE(description.find("fgBrightnessHdr: 0"), std::string::npos);
}

/**
 * @tc.name: ShadowBlender001
 * @tc.desc: test SetShadowBlenderParams/GetShadowBlenderParams/IsShadowBlenderValid/GetShadowBlenderDescription
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectPropertiesTest, ShadowBlender001, TestSize.Level1)
{
    RSEffectProperties properties;
    EXPECT_FALSE(properties.GetShadowBlenderParams().has_value());
    EXPECT_FALSE(properties.IsShadowBlenderValid());
    EXPECT_EQ(properties.GetShadowBlenderDescription(), "shadowBlenderParams_ is nullopt");

    properties.SetShadowBlenderParams(std::nullopt);
    EXPECT_FALSE(properties.GetShadowBlenderParams().has_value());

    RSShadowBlenderPara params { 1.f, 1.f, 1.f, 1.f };
    properties.SetShadowBlenderParams(params);
    EXPECT_EQ(properties.GetShadowBlenderParams(), params);
    EXPECT_TRUE(properties.IsShadowBlenderValid());
    std::string description = properties.GetShadowBlenderDescription();
    EXPECT_NE(description.find("ShadowBlender"), std::string::npos);

    // out of range values are invalid
    RSShadowBlenderPara invalidParams { -1000.f, 0.f, 0.f, 0.f };
    properties.SetShadowBlenderParams(invalidParams);
    EXPECT_FALSE(properties.IsShadowBlenderValid());
}

/**
 * @tc.name: HdrDarkenBlender001
 * @tc.desc: test SetHdrDarkenBlenderParams/GetHdrDarkenBlenderParams/IsHdrDarkenBlenderValid/
 *           GetHdrDarkenBlenderDescription
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectPropertiesTest, HdrDarkenBlender001, TestSize.Level1)
{
    RSEffectProperties properties;
    EXPECT_FALSE(properties.GetHdrDarkenBlenderParams().has_value());
    EXPECT_FALSE(properties.IsHdrDarkenBlenderValid());
    EXPECT_EQ(properties.GetHdrDarkenBlenderDescription(), "hdrDarkenBlenderParams is nullopt");

    RSHdrDarkenBlenderPara params { 1.5f, Vector3f { 0.3f, 0.6f, 0.1f } };
    properties.SetHdrDarkenBlenderParams(params);
    EXPECT_EQ(properties.GetHdrDarkenBlenderParams(), params);
    EXPECT_TRUE(properties.IsHdrDarkenBlenderValid());
    std::string description = properties.GetHdrDarkenBlenderDescription();
    EXPECT_NE(description.find("HdrDarkenBlender"), std::string::npos);
}

/**
 * @tc.name: ColorfulBrightnessBlender001
 * @tc.desc: test SetColorfulBrightnessBlenderParams/GetColorfulBrightnessBlenderParams/
 *           IsColorfulBrightnessBlenderValid/GetColorfulBrightnessBlenderDescription
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectPropertiesTest, ColorfulBrightnessBlender001, TestSize.Level1)
{
    RSEffectProperties properties;
    EXPECT_EQ(properties.GetColorfulBrightnessBlenderParams(), nullptr);
    EXPECT_FALSE(properties.IsColorfulBrightnessBlenderValid());
    EXPECT_EQ(properties.GetColorfulBrightnessBlenderDescription(), "colorfulBrightnessBlenderParams is nullopt");

    properties.SetColorfulBrightnessBlenderParams(nullptr);
    EXPECT_EQ(properties.GetColorfulBrightnessBlenderParams(), nullptr);
    EXPECT_FALSE(properties.IsColorfulBrightnessBlenderValid());

    auto params = std::make_unique<RSColorfulBrightnessBlenderPara>(1.f, 0.5f, 0.1f, 0.2f, 0.3f, 0.4f, 0.5f,
        Vector3f { 1.f, 1.f, 1.f }, Vector3f { 1.f, 1.f, 1.f }, 0.33f, 0.88f, true, 0.66f);
    const RSColorfulBrightnessBlenderPara* paramsPtr = params.get();
    properties.SetColorfulBrightnessBlenderParams(std::move(params));
    EXPECT_EQ(properties.GetColorfulBrightnessBlenderParams(), paramsPtr);
    EXPECT_TRUE(properties.IsColorfulBrightnessBlenderValid());
    std::string description = properties.GetColorfulBrightnessBlenderDescription();
    EXPECT_NE(description.find("ColorfulBrightnessBlender"), std::string::npos);
    EXPECT_NE(description.find("hdrEnabled: 1"), std::string::npos);
}

/**
 * @tc.name: ColorfulBrightnessBlenderMember001
 * @tc.desc: test SetColorfulBrightnessBlenderMember template lazy creation and reuse
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectPropertiesTest, ColorfulBrightnessBlenderMember001, TestSize.Level1)
{
    RSEffectProperties properties;
    EXPECT_EQ(properties.GetColorfulBrightnessBlenderParams(), nullptr);

    properties.SetColorfulBrightnessBlenderMember(&RSColorfulBrightnessBlenderPara::darkenWeight_, 1.0f);
    const auto* params = properties.GetColorfulBrightnessBlenderParams();
    ASSERT_NE(params, nullptr);
    EXPECT_EQ(params->darkenWeight_, 1.0f);

    properties.SetColorfulBrightnessBlenderMember(&RSColorfulBrightnessBlenderPara::saturation_, 0.5f);
    EXPECT_EQ(properties.GetColorfulBrightnessBlenderParams()->saturation_, 0.5f);
    EXPECT_EQ(properties.GetColorfulBrightnessBlenderParams()->darkenWeight_, 1.0f);
}

/**
 * @tc.name: ColorBlend001
 * @tc.desc: test SetColorBlendMode/GetColorBlendMode/SetColorBlendApplyType/GetColorBlendApplyType/
 *           SetColorBlend/GetColorBlend with clamp branches
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectPropertiesTest, ColorBlend001, TestSize.Level1)
{
    RSEffectProperties properties;
    EXPECT_EQ(properties.GetColorBlendMode(), 0);
    EXPECT_EQ(properties.GetColorBlendApplyType(), 0);
    EXPECT_FALSE(properties.GetColorBlend().has_value());

    properties.SetColorBlendMode(5);
    EXPECT_EQ(properties.GetColorBlendMode(), 5);
    // clamp upper branch
    properties.SetColorBlendMode(1000);
    EXPECT_EQ(properties.GetColorBlendMode(), static_cast<int>(RSColorBlendMode::MAX));
    // clamp lower branch
    properties.SetColorBlendMode(-1);
    EXPECT_EQ(properties.GetColorBlendMode(), 0);

    properties.SetColorBlendApplyType(1);
    EXPECT_EQ(properties.GetColorBlendApplyType(), 1);
    // clamp upper branch
    properties.SetColorBlendApplyType(1000);
    EXPECT_EQ(properties.GetColorBlendApplyType(), static_cast<int>(RSColorBlendApplyType::MAX));
    // clamp lower branch
    properties.SetColorBlendApplyType(-1);
    EXPECT_EQ(properties.GetColorBlendApplyType(), 0);

    Color colorBlend = Color::FromArgbInt(0xFF0000FF);
    properties.SetColorBlend(colorBlend);
    EXPECT_EQ(properties.GetColorBlend(), colorBlend);
    properties.SetColorBlend(std::nullopt);
    EXPECT_TRUE(properties.GetColorBlend().has_value());
}
} // namespace Rosen
} // namespace OHOS
