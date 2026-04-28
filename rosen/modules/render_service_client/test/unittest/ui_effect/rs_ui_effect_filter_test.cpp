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

#include "filter/include/filter.h"
#include "filter/include/filter_blur_bubbles_rise_para.h"
#include "filter/include/filter_content_light_para.h"
#include "filter/include/filter_dispersion_para.h"
#include "filter/include/filter_displacement_distort_para.h"
#include "filter/include/filter_frosted_glass_blur_para.h"
#include "filter/include/filter_frosted_glass_para.h"
#include "filter/include/filter_heat_distortion_para.h"
#include "filter/include/filter_mask_transition_para.h"
#include "filter/include/filter_para.h"
#include "filter/include/filter_unmarshalling_singleton.h"
#include "filter/include/filter_water_ripple_para.h"
#include "mask/include/radial_gradient_mask_para.h"

namespace OHOS {
namespace Rosen {

using namespace testing;
using namespace testing::ext;

class RSUIEffectFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUIEffectFilterTest::SetUpTestCase() {}
void RSUIEffectFilterTest::TearDownTestCase() {}

void RSUIEffectFilterTest::SetUp()
{
    ContentLightPara::RegisterUnmarshallingCallback();
    DispersionPara::RegisterUnmarshallingCallback();
    DisplacementDistortPara::RegisterUnmarshallingCallback();
    MaskTransitionPara::RegisterUnmarshallingCallback();
    WaterRipplePara::RegisterUnmarshallingCallback();
    FrostedGlassPara::RegisterUnmarshallingCallback();
    FrostedGlassBlurPara::RegisterUnmarshallingCallback();
}

void RSUIEffectFilterTest::TearDown() {}

/**
 * @tc.name: RSUIEffectContentLightParaTest
 * @tc.desc: Verify the ContentLightPara func
 * @tc.type: FUNC
 */
HWTEST_F(RSUIEffectFilterTest, RSUIEffectContentLightParaTest, TestSize.Level1)
{
    auto para = std::make_shared<ContentLightPara>();
    Vector3f lightPosition = Vector3f(1.0f, 0.5f, 0.6f); // 1.0f, 0.5f, 0.6f is random value
    Vector4f lightColor = Vector4f(1.0f, 0.5f, 0.6f, 0.7f); // 1.0f, 0.5f, 0.6f, 0.7f is random value
    float lightIntensity = 0.5f; // 0.5f is random value
    para->SetLightPosition(lightPosition);
    para->SetLightColor(lightColor);
    para->SetLightIntensity(lightIntensity);

    Parcel parcel;
    EXPECT_TRUE(para->Marshalling(parcel));

    std::shared_ptr<FilterPara> val = nullptr;
    EXPECT_TRUE(FilterPara::Unmarshalling(parcel, val));
    EXPECT_NE(nullptr, val);
    auto contentLightPara = std::static_pointer_cast<ContentLightPara>(val);
    EXPECT_EQ(lightPosition, contentLightPara->GetLightPosition());
    EXPECT_EQ(lightColor, contentLightPara->GetLightColor());
    EXPECT_EQ(lightIntensity, contentLightPara->GetLightIntensity());
    EXPECT_NE(nullptr, para->Clone());

    Parcel parcel1;
    FilterPara::ParaType type = FilterPara::ParaType::CONTENT_LIGHT;
    parcel1.WriteUint16(static_cast<uint16_t>(type));
    EXPECT_FALSE(FilterPara::Unmarshalling(parcel, val));

    parcel1.WriteUint16(static_cast<uint16_t>(type));
    parcel1.WriteUint16(static_cast<uint16_t>(type));
    EXPECT_FALSE(FilterPara::Unmarshalling(parcel1, val));

    parcel1.WriteUint16(static_cast<uint16_t>(type));
    parcel1.WriteUint16(static_cast<uint16_t>(type));
    parcel1.WriteFloat(lightPosition.x_);
    parcel1.WriteFloat(lightPosition.y_);
    parcel1.WriteFloat(lightPosition.z_);
    parcel1.WriteFloat(lightColor.x_);
    parcel1.WriteFloat(lightColor.y_);
    parcel1.WriteFloat(lightColor.z_);
    parcel1.WriteFloat(lightColor.w_);
    parcel1.WriteFloat(lightIntensity);
    EXPECT_TRUE(FilterPara::Unmarshalling(parcel1, val));

    std::shared_ptr<FilterPara> valTest = nullptr;
    Parcel parcelTest;
    EXPECT_EQ(false, ContentLightPara::OnUnmarshalling(parcelTest, valTest));
    parcelTest.FlushBuffer();
    parcelTest.WriteUint16(666);
    EXPECT_EQ(false, ContentLightPara::OnUnmarshalling(parcelTest, valTest));
}

/**
 * @tc.name: RSUIEffectHeatDistortionParaTest
 * @tc.desc: Verify the HeatDistortionPara func
 * @tc.type: FUNC
 */
HWTEST_F(RSUIEffectFilterTest, RSUIEffectHeatDistortionParaTest, TestSize.Level1)
{
    auto para = std::make_shared<HeatDistortionPara>();
    constexpr float intensity = 1.0f;
    constexpr float noiseScale = 2.0f;
    constexpr float riseWeight = 0.4f;
    constexpr float progress = 0.25f;
    para->SetIntensity(intensity);
    para->SetNoiseScale(noiseScale);
    para->SetRiseWeight(riseWeight);
    para->SetProgress(progress);

    EXPECT_EQ(intensity, para->GetIntensity());
    EXPECT_EQ(noiseScale, para->GetNoiseScale());
    EXPECT_EQ(riseWeight, para->GetRiseWeight());
    EXPECT_EQ(progress, para->GetProgress());

    Parcel parcel;
    EXPECT_FALSE(para->Marshalling(parcel));
    EXPECT_EQ(nullptr, para->Clone());
}

/**
 * @tc.name: RSUIEffectBlurBubblesRiseParaTest
 * @tc.desc: Verify the BlurBubblesRisePara func
 * @tc.type: FUNC
 */
HWTEST_F(RSUIEffectFilterTest, RSUIEffectBlurBubblesRiseParaTest, TestSize.Level1)
{
    auto para = std::make_shared<BlurBubblesRisePara>();
    constexpr float blurIntensity = 0.8f;
    constexpr float mixStrength = 0.7f;
    constexpr float progress = 0.35f;
    para->SetBlurIntensity(blurIntensity);
    para->SetMixStrength(mixStrength);
    para->SetProgress(progress);

    EXPECT_EQ(blurIntensity, para->GetBlurIntensity());
    EXPECT_EQ(mixStrength, para->GetMixStrength());
    EXPECT_EQ(progress, para->GetProgress());

    Parcel parcel;
    EXPECT_FALSE(para->Marshalling(parcel));
    EXPECT_EQ(nullptr, para->Clone());
}

/**
 * @tc.name: RSUIEffectDispersionParaTest001
 * @tc.desc: Verify the DispersionPara func by Marshalling
 * @tc.type: FUNC
 */
HWTEST_F(RSUIEffectFilterTest, RSUIEffectRDispersionParaTest001, TestSize.Level1)
{
    auto para = std::make_shared<DispersionPara>();
    float opacity = 0.5f; // 0.5f is random value
    Vector2f redOffset = Vector2f(0.5f, 0.6f); // 0.5f 0.6f is random value
    Vector2f greenOffset = Vector2f(0.5f, 0.8f); // 0.5f 0.8f is random value
    Vector2f blueOffset = Vector2f(0.3f, 0.7f); // 0.3f 0.7f is random value
    para->SetOpacity(opacity);
    para->SetRedOffset(redOffset);
    para->SetGreenOffset(greenOffset);
    para->SetBlueOffset(blueOffset);

    Parcel parcel;
    EXPECT_TRUE(para->Marshalling(parcel));

    std::shared_ptr<FilterPara> val = nullptr;
    EXPECT_TRUE(FilterPara::Unmarshalling(parcel, val));
    EXPECT_NE(nullptr, val);
    auto dispersionPara = std::static_pointer_cast<DispersionPara>(val);
    EXPECT_EQ(opacity, dispersionPara->GetOpacity());
    EXPECT_EQ(redOffset, dispersionPara->GetRedOffset());
    EXPECT_EQ(greenOffset, dispersionPara->GetGreenOffset());
    EXPECT_EQ(blueOffset, dispersionPara->GetBlueOffset());
    EXPECT_EQ(nullptr, dispersionPara->GetMask());
    EXPECT_NE(nullptr, para->Clone());

    Parcel parcel1;
    FilterPara::ParaType type = FilterPara::ParaType::DISPERSION;
    parcel1.WriteUint16(static_cast<uint16_t>(type));
    EXPECT_FALSE(FilterPara::Unmarshalling(parcel, val));

    parcel1.WriteUint16(static_cast<uint16_t>(type));
    parcel1.WriteUint16(static_cast<uint16_t>(type));
    EXPECT_FALSE(FilterPara::Unmarshalling(parcel1, val));

    parcel1.WriteUint16(static_cast<uint16_t>(type));
    parcel1.WriteUint16(static_cast<uint16_t>(type));
    parcel1.WriteBool(true);
    EXPECT_FALSE(FilterPara::Unmarshalling(parcel1, val));

    parcel1.WriteUint16(static_cast<uint16_t>(type));
    parcel1.WriteUint16(static_cast<uint16_t>(type));
    parcel1.WriteBool(false);
    EXPECT_FALSE(FilterPara::Unmarshalling(parcel1, val));

    std::shared_ptr<FilterPara> valTest = nullptr;
    Parcel parcelTest;
    EXPECT_EQ(false, DispersionPara::OnUnmarshalling(parcelTest, valTest));
    parcelTest.FlushBuffer();
    parcelTest.WriteUint16(666);
    EXPECT_EQ(false, DispersionPara::OnUnmarshalling(parcelTest, valTest));

    para = std::make_shared<DispersionPara>();
    auto maskPara = std::make_shared<MaskPara>();
    para->SetMask(maskPara);
    EXPECT_FALSE(para->Marshalling(parcel));
}

/**
 * @tc.name: RSUIEffectDisplacementDistortParaTest001
 * @tc.desc: Verify the DisplacementDistortPara func by Marshalling
 * @tc.type: FUNC
 */
HWTEST_F(RSUIEffectFilterTest, RSUIEffectRDisplacementDistortParaTest001, TestSize.Level1)
{
    auto para = std::make_shared<DisplacementDistortPara>();
    Vector2f factor = { 1.5f, 1.5f }; // 1.5f, 1.5f is random value
    para->SetFactor(factor);

    Parcel parcel;
    EXPECT_TRUE(para->Marshalling(parcel));

    std::shared_ptr<FilterPara> val = nullptr;
    EXPECT_TRUE(FilterPara::Unmarshalling(parcel, val));
    EXPECT_NE(nullptr, val);
    auto displacementDistortPara = std::static_pointer_cast<DisplacementDistortPara>(val);
    EXPECT_EQ(factor, displacementDistortPara->GetFactor());
    EXPECT_EQ(nullptr, displacementDistortPara->GetMask());
    EXPECT_NE(nullptr, para->Clone());

    Parcel parcel1;
    FilterPara::ParaType type = FilterPara::ParaType::DISPLACEMENT_DISTORT;
    parcel1.WriteUint16(static_cast<uint16_t>(type));
    EXPECT_FALSE(FilterPara::Unmarshalling(parcel, val));

    parcel1.WriteUint16(static_cast<uint16_t>(type));
    parcel1.WriteUint16(static_cast<uint16_t>(type));
    EXPECT_FALSE(FilterPara::Unmarshalling(parcel1, val));

    parcel1.WriteUint16(static_cast<uint16_t>(type));
    parcel1.WriteUint16(static_cast<uint16_t>(type));
    parcel1.WriteBool(true);
    EXPECT_FALSE(FilterPara::Unmarshalling(parcel1, val));

    parcel1.WriteUint16(static_cast<uint16_t>(type));
    parcel1.WriteUint16(static_cast<uint16_t>(type));
    parcel1.WriteBool(false);
    EXPECT_FALSE(FilterPara::Unmarshalling(parcel1, val));

    std::shared_ptr<FilterPara> valTest = nullptr;
    Parcel parcelTest;
    EXPECT_EQ(false, DisplacementDistortPara::OnUnmarshalling(parcelTest, valTest));
    parcelTest.FlushBuffer();
    parcelTest.WriteUint16(666);
    EXPECT_EQ(false, DisplacementDistortPara::OnUnmarshalling(parcelTest, valTest));

    para = std::make_shared<DisplacementDistortPara>();
    auto maskPara = std::make_shared<MaskPara>();
    para->SetMask(maskPara);
    EXPECT_FALSE(para->Marshalling(parcel));
}

/**
 * @tc.name: RSUIEffectWaterRippleParaTest
 * @tc.desc: Verify the WaterRipplePara func
 * @tc.type: FUNC
 */
HWTEST_F(RSUIEffectFilterTest, RSUIEffectWaterRippleParaTest, TestSize.Level1)
{
    auto waterRipplePara = std::make_shared<WaterRipplePara>();
    float rippleCenterX = 0.1f;
    waterRipplePara->SetRippleCenterX(rippleCenterX);
    float rippleCenterY = 0.5f;
    waterRipplePara->SetRippleCenterY(rippleCenterY);
    float progress = 0.8f;
    waterRipplePara->SetProgress(progress);
    uint32_t waveCount = 1;
    waterRipplePara->SetWaveCount(waveCount);
    uint32_t rippleMode = 0;
    waterRipplePara->SetRippleMode(rippleMode);

    Parcel parcel;
    EXPECT_EQ(true, waterRipplePara->Marshalling(parcel));
    std::shared_ptr<FilterPara> filterPara = nullptr;
    EXPECT_EQ(true, FilterPara::Unmarshalling(parcel, filterPara));
    EXPECT_NE(nullptr, filterPara);
    EXPECT_NE(nullptr, waterRipplePara->Clone());

    auto filter = std::make_shared<Filter>();
    filter->AddPara(waterRipplePara);
    Parcel parcel2;
    EXPECT_EQ(true, filter->Marshalling(parcel2));
    std::shared_ptr<Filter> val = nullptr;
    EXPECT_EQ(true, Filter::Unmarshalling(parcel2, val));
    EXPECT_NE(nullptr, val);
    auto filterTemp = std::make_shared<Filter>(*filter.get());
    EXPECT_NE(nullptr, filterTemp);

    std::shared_ptr<FilterPara> valTest = nullptr;
    Parcel parcelTest;
    EXPECT_EQ(false, WaterRipplePara::OnUnmarshalling(parcelTest, valTest));

    parcelTest.FlushBuffer();
    parcelTest.WriteUint16(666);
    EXPECT_EQ(false, WaterRipplePara::OnUnmarshalling(parcelTest, valTest));

    parcelTest.FlushBuffer();
    parcelTest.WriteUint16(static_cast<uint16_t>(FilterPara::ParaType::WATER_RIPPLE));
    EXPECT_EQ(false, WaterRipplePara::OnUnmarshalling(parcelTest, valTest));
}

/**
 * @tc.name: RSUIEffectMaskTransitionParaTest
 * @tc.desc: Verify the MaskTransitionPara func
 * @tc.type: FUNC
 */
HWTEST_F(RSUIEffectFilterTest, RSUIEffectMaskTransitionParaTest, TestSize.Level1)
{
    auto maskTransitionPara = std::make_shared<MaskTransitionPara>();
    auto radialGradientMaskPara = std::make_shared<RadialGradientMaskPara>();
    Vector2f center = {0.5f, 0.5f};
    radialGradientMaskPara->SetCenter(center);
    Vector2f radius = {0.5f, 0.5f};
    radialGradientMaskPara->SetRadiusX(radius.x_);
    radialGradientMaskPara->SetRadiusX(radius.y_);
    std::vector<float> colors = { 0.0f, 0.5f, 1.0f };
    std::vector<float> positions = { 0.0f, 0.5f, 1.0f };
    radialGradientMaskPara->SetColors(colors);
    radialGradientMaskPara->SetPositions(positions);
    maskTransitionPara->SetMask(radialGradientMaskPara);
    float factor = 0.5f;
    maskTransitionPara->SetFactor(factor);
    bool inverse = false;
    maskTransitionPara->SetInverse(inverse);

    Parcel parcel;
    EXPECT_EQ(true, maskTransitionPara->Marshalling(parcel));
    std::shared_ptr<FilterPara> val = nullptr;
    EXPECT_EQ(true, FilterPara::Unmarshalling(parcel, val));
    EXPECT_NE(nullptr, val);
    EXPECT_NE(nullptr, maskTransitionPara->Clone());

    auto filter = std::make_shared<Filter>();
    filter->AddPara(maskTransitionPara);
    Parcel parcel2;
    EXPECT_EQ(true, filter->Marshalling(parcel2));
    std::shared_ptr<Filter> valFilter = nullptr;
    EXPECT_EQ(true, Filter::Unmarshalling(parcel2, valFilter));
    EXPECT_NE(nullptr, valFilter);
    auto filterTemp = std::make_shared<Filter>(*filter.get());
    EXPECT_NE(nullptr, filterTemp);

    std::shared_ptr<FilterPara> valTest = nullptr;
    Parcel parcelTest;
    EXPECT_EQ(false, MaskTransitionPara::OnUnmarshalling(parcelTest, valTest));

    parcelTest.FlushBuffer();
    parcelTest.WriteUint16(666);
    EXPECT_EQ(false, MaskTransitionPara::OnUnmarshalling(parcelTest, valTest));
    parcelTest.FlushBuffer();
    parcelTest.WriteUint16(static_cast<uint16_t>(FilterPara::ParaType::MASK_TRANSITION));
    EXPECT_EQ(false, MaskTransitionPara::OnUnmarshalling(parcelTest, valTest));
}

/**
 * @tc.name: RSUIEffectFilterUnmarshallingSingletonTest
 * @tc.desc: Verify the FilterUnmarshallingSingleton func
 * @tc.type: FUNC
 */
HWTEST_F(RSUIEffectFilterTest, RSUIEffectFilterUnmarshallingSingletonTest, TestSize.Level1)
{
    uint16_t type = 666;
    auto unmarshallingFunc = [](Parcel& parcel, std::shared_ptr<FilterPara>& val) -> bool { return true; };
    FilterUnmarshallingSingleton::GetInstance().RegisterCallback(type, unmarshallingFunc);
    auto retFunc = FilterUnmarshallingSingleton::GetInstance().GetCallback(type);
    EXPECT_NE(nullptr, retFunc);
    FilterUnmarshallingSingleton::GetInstance().UnregisterCallback(type);
    retFunc = FilterUnmarshallingSingleton::GetInstance().GetCallback(type);
    EXPECT_EQ(nullptr, retFunc);
}

/**
 * @tc.name: RSUIEffectFilterParamTest
 * @tc.desc: Verify the FilterPara func
 * @tc.type: FUNC
 */
HWTEST_F(RSUIEffectFilterTest, RSUIEffectFilterParamTest, TestSize.Level1)
{
    EXPECT_EQ(false, FilterPara::RegisterUnmarshallingCallback(FilterPara::ParaType::NONE, nullptr));

    EXPECT_EQ(false, FilterPara::RegisterUnmarshallingCallback(FilterPara::ParaType::MASK_TRANSITION, nullptr));

    auto filterParam = std::make_shared<FilterPara>();
    EXPECT_NE(filterParam, nullptr);
    Parcel parcel;
    EXPECT_EQ(false, filterParam->Marshalling(parcel));

    std::shared_ptr<FilterPara> val = nullptr;
    EXPECT_EQ(false, filterParam->Unmarshalling(parcel, val));

    parcel.WriteUint16(static_cast<uint16_t>(FilterPara::ParaType::NONE));
    EXPECT_EQ(false, filterParam->Unmarshalling(parcel, val));

    Parcel parcel1;
    parcel1.WriteUint16(666);
    EXPECT_EQ(false, filterParam->Unmarshalling(parcel1, val));

    EXPECT_EQ(filterParam->Clone(), nullptr);
}

/**
 * @tc.name: RSUIEffectFilterTest001
 * @tc.desc: Verify the FilterPara func
 * @tc.type: FUNC
 */
HWTEST_F(RSUIEffectFilterTest, RSUIEffectFilterTest001, TestSize.Level1)
{
    auto filter = std::make_shared<Filter>();
    EXPECT_NE(filter, nullptr);
    filter->AddPara(nullptr);
    auto filter1 = std::make_shared<Filter>(*filter);

    auto filterPara = std::make_shared<FilterPara>();
    filter->AddPara(filterPara);
    Parcel parcel;
    EXPECT_EQ(true, filter->Marshalling(parcel));
    filter1 = std::make_shared<Filter>(*filter);

    for (int i = 0; i < (FilterPara::UNMARSHALLING_MAX_VECTOR_SIZE + 1); ++i) {
        auto filterPara = std::make_shared<FilterPara>();
        filter->AddPara(filterPara);
    }
    EXPECT_EQ(true, filter->Marshalling(parcel));

    std::shared_ptr<Filter> valTest = nullptr;
    parcel.FlushBuffer();
    parcel.WriteUint32(FilterPara::UNMARSHALLING_MAX_VECTOR_SIZE + 1);
    EXPECT_EQ(false, Filter::Unmarshalling(parcel, valTest));
}

/**
 * @tc.name: RSUIEffectFrostedGlassBlurParaTest
 * @tc.desc: Verify the FrostedGlassBlurPara Marshalling and Unmarshalling
 * @tc.type: FUNC
 */
HWTEST_F(RSUIEffectFilterTest, RSUIEffectFrostedGlassBlurParaTest, TestSize.Level1)
{
    auto para = std::make_shared<FrostedGlassBlurPara>();
    constexpr float radius = 10.0f;
    constexpr float radiusScaleK = 0.5f;
    constexpr float refractOutPx = 2.0f;
    constexpr bool isSkipFrameEnable = true;
    para->SetBlurRadius(radius);
    para->SetBlurRadiusScaleK(radiusScaleK);
    para->SetRefractOutPx(refractOutPx);
    para->SetSkipFrameEnable(isSkipFrameEnable);

    Parcel parcel;
    EXPECT_TRUE(para->Marshalling(parcel));

    std::shared_ptr<FilterPara> val = nullptr;
    EXPECT_TRUE(FilterPara::Unmarshalling(parcel, val));
    EXPECT_NE(nullptr, val);
    auto frostedGlassBlurPara = std::static_pointer_cast<FrostedGlassBlurPara>(val);
    EXPECT_EQ(radius, frostedGlassBlurPara->GetBlurRadius());
    EXPECT_EQ(radiusScaleK, frostedGlassBlurPara->GetBlurRadiusScaleK());
    EXPECT_EQ(refractOutPx, frostedGlassBlurPara->GetRefractOutPx());
    EXPECT_EQ(isSkipFrameEnable, frostedGlassBlurPara->GetSkipFrameEnable());
    EXPECT_NE(nullptr, para->Clone());

    std::shared_ptr<FilterPara> valTest = nullptr;
    Parcel parcelTest;
    EXPECT_FALSE(FrostedGlassBlurPara::OnUnmarshalling(parcelTest, valTest));
    parcelTest.FlushBuffer();
    parcelTest.WriteUint16(666);
    EXPECT_FALSE(FrostedGlassBlurPara::OnUnmarshalling(parcelTest, valTest));

    auto filter = std::make_shared<Filter>();
    filter->AddPara(para);
    Parcel parcel2;
    EXPECT_TRUE(filter->Marshalling(parcel2));
    std::shared_ptr<Filter> valFilter = nullptr;
    EXPECT_TRUE(Filter::Unmarshalling(parcel2, valFilter));
    EXPECT_NE(nullptr, valFilter);
}

/**
 * @tc.name: RSUIEffectFrostedGlassParaTest
 * @tc.desc: Verify the FrostedGlassPara Marshalling and Unmarshalling without optional params
 * @tc.type: FUNC
 */
HWTEST_F(RSUIEffectFilterTest, RSUIEffectFrostedGlassParaTest, TestSize.Level1)
{
    auto para = std::make_shared<FrostedGlassPara>();
    Vector2f blurParams = Vector2f(48.0f, 4.0f);
    Vector2f weightsEmboss = Vector2f(1.0f, 2.0f);
    Vector2f weightsEdl = Vector2f(0.5f, 0.6f);
    Vector2f bgRates = Vector2f(-0.00003f, 1.2f);
    Vector3f bgKBS = Vector3f(0.010834f, 0.007349f, 1.2f);
    Vector3f bgPos = Vector3f(0.3f, 0.5f, 1.0f);
    Vector3f bgNeg = Vector3f(0.5f, 0.5f, 1.0f);
    constexpr float bgAlpha = 0.8f;
    Vector3f refractParams = Vector3f(1.0f, 2.0f, 3.0f);
    Vector3f sdParams = Vector3f(0.1f, 0.2f, 0.3f);
    Vector2f sdRates = Vector2f(0.4f, 0.5f);
    Vector3f sdKBS = Vector3f(0.6f, 0.7f, 0.8f);
    Vector3f sdPos = Vector3f(0.9f, 1.0f, 1.1f);
    Vector3f sdNeg = Vector3f(1.2f, 1.3f, 1.4f);
    Vector3f envLightParams = Vector3f(2.0f, 3.0f, 4.0f);
    Vector2f envLightRates = Vector2f(5.0f, 6.0f);
    Vector3f envLightKBS = Vector3f(7.0f, 8.0f, 9.0f);
    Vector3f envLightPos = Vector3f(10.0f, 11.0f, 12.0f);
    Vector3f envLightNeg = Vector3f(13.0f, 14.0f, 15.0f);
    Vector2f edLightParams = Vector2f(16.0f, 17.0f);
    Vector2f edLightAngles = Vector2f(18.0f, 19.0f);
    Vector2f edLightDir = Vector2f(20.0f, 21.0f);
    Vector2f edLightRates = Vector2f(22.0f, 23.0f);
    Vector3f edLightKBS = Vector3f(24.0f, 25.0f, 26.0f);
    Vector3f edLightPos = Vector3f(27.0f, 28.0f, 29.0f);
    Vector3f edLightNeg = Vector3f(30.0f, 31.0f, 32.0f);
    constexpr bool baseVibrancyEnabled = false;
    constexpr float baseMaterialType = 1.5f;
    Vector4f materialColor = Vector4f(0.1f, 0.2f, 0.3f, 0.4f);
    constexpr float darkScale = 0.5f;
    constexpr float samplingScale = 0.75f;
    constexpr bool isSkipFrameEnable = true;

    para->SetBlurParams(blurParams);
    para->SetWeightsEmboss(weightsEmboss);
    para->SetWeightsEdl(weightsEdl);
    para->SetBgRates(bgRates);
    para->SetBgKBS(bgKBS);
    para->SetBgPos(bgPos);
    para->SetBgNeg(bgNeg);
    para->SetBgAlpha(bgAlpha);
    para->SetRefractParams(refractParams);
    para->SetSdParams(sdParams);
    para->SetSdRates(sdRates);
    para->SetSdKBS(sdKBS);
    para->SetSdPos(sdPos);
    para->SetSdNeg(sdNeg);
    para->SetEnvLightParams(envLightParams);
    para->SetEnvLightRates(envLightRates);
    para->SetEnvLightKBS(envLightKBS);
    para->SetEnvLightPos(envLightPos);
    para->SetEnvLightNeg(envLightNeg);
    para->SetEdLightParams(edLightParams);
    para->SetEdLightAngles(edLightAngles);
    para->SetEdLightDir(edLightDir);
    para->SetEdLightRates(edLightRates);
    para->SetEdLightKBS(edLightKBS);
    para->SetEdLightPos(edLightPos);
    para->SetEdLightNeg(edLightNeg);
    para->SetBaseVibrancyEnabled(baseVibrancyEnabled);
    para->SetBaseMaterialType(baseMaterialType);
    para->SetMaterialColor(materialColor);
    para->SetDarkScale(darkScale);
    para->SetSamplingScale(samplingScale);
    para->SetSkipFrameEnable(isSkipFrameEnable);

    Parcel parcel;
    EXPECT_TRUE(para->Marshalling(parcel));

    std::shared_ptr<FilterPara> val = nullptr;
    EXPECT_TRUE(FilterPara::Unmarshalling(parcel, val));
    EXPECT_NE(nullptr, val);
    auto frostedGlassPara = std::static_pointer_cast<FrostedGlassPara>(val);
    EXPECT_EQ(blurParams, frostedGlassPara->GetBlurParams());
    EXPECT_EQ(weightsEmboss, frostedGlassPara->GetWeightsEmboss());
    EXPECT_EQ(weightsEdl, frostedGlassPara->GetWeightsEdl());
    EXPECT_EQ(bgRates, frostedGlassPara->GetBgRates());
    EXPECT_EQ(bgKBS, frostedGlassPara->GetBgKBS());
    EXPECT_EQ(bgPos, frostedGlassPara->GetBgPos());
    EXPECT_EQ(bgNeg, frostedGlassPara->GetBgNeg());
    EXPECT_EQ(bgAlpha, frostedGlassPara->GetBgAlpha());
    EXPECT_EQ(refractParams, frostedGlassPara->GetRefractParams());
    EXPECT_EQ(sdParams, frostedGlassPara->GetSdParams());
    EXPECT_EQ(sdRates, frostedGlassPara->GetSdRates());
    EXPECT_EQ(sdKBS, frostedGlassPara->GetSdKBS());
    EXPECT_EQ(sdPos, frostedGlassPara->GetSdPos());
    EXPECT_EQ(sdNeg, frostedGlassPara->GetSdNeg());
    EXPECT_EQ(envLightParams, frostedGlassPara->GetEnvLightParams());
    EXPECT_EQ(envLightRates, frostedGlassPara->GetEnvLightRates());
    EXPECT_EQ(envLightKBS, frostedGlassPara->GetEnvLightKBS());
    EXPECT_EQ(envLightPos, frostedGlassPara->GetEnvLightPos());
    EXPECT_EQ(envLightNeg, frostedGlassPara->GetEnvLightNeg());
    EXPECT_EQ(edLightParams, frostedGlassPara->GetEdLightParams());
    EXPECT_EQ(edLightAngles, frostedGlassPara->GetEdLightAngles());
    EXPECT_EQ(edLightDir, frostedGlassPara->GetEdLightDir());
    EXPECT_EQ(edLightRates, frostedGlassPara->GetEdLightRates());
    EXPECT_EQ(edLightKBS, frostedGlassPara->GetEdLightKBS());
    EXPECT_EQ(edLightPos, frostedGlassPara->GetEdLightPos());
    EXPECT_EQ(edLightNeg, frostedGlassPara->GetEdLightNeg());
    EXPECT_EQ(baseVibrancyEnabled, frostedGlassPara->GetBaseVibrancyEnabled());
    EXPECT_EQ(baseMaterialType, frostedGlassPara->GetBaseMaterialType());
    EXPECT_EQ(materialColor, frostedGlassPara->GetMaterialColor());
    EXPECT_EQ(darkScale, frostedGlassPara->GetDarkScale());
    EXPECT_EQ(samplingScale, frostedGlassPara->GetSamplingScale());
    EXPECT_EQ(nullptr, frostedGlassPara->GetMask());
    EXPECT_EQ(nullptr, frostedGlassPara->GetDarkAdaptiveParams());
    EXPECT_EQ(isSkipFrameEnable, frostedGlassPara->GetSkipFrameEnable());
    EXPECT_NE(nullptr, para->Clone());

    std::shared_ptr<FilterPara> valTest = nullptr;
    Parcel parcelTest;
    EXPECT_FALSE(FrostedGlassPara::OnUnmarshalling(parcelTest, valTest));
    parcelTest.FlushBuffer();
    parcelTest.WriteUint16(666);
    EXPECT_FALSE(FrostedGlassPara::OnUnmarshalling(parcelTest, valTest));
}

/**
 * @tc.name: RSUIEffectFrostedGlassParaWithMaskAndDarkTest
 * @tc.desc: Verify the FrostedGlassPara Marshalling with Mask and DarkAdaptiveParams
 * @tc.type: FUNC
 */
HWTEST_F(RSUIEffectFilterTest, RSUIEffectFrostedGlassParaWithMaskAndDarkTest, TestSize.Level1)
{
    auto para = std::make_shared<FrostedGlassPara>();
    auto radialGradientMaskPara = std::make_shared<RadialGradientMaskPara>();
    Vector2f center = Vector2f(0.5f, 0.5f);
    radialGradientMaskPara->SetCenter(center);
    constexpr float radiusX = 0.3f;
    constexpr float radiusY = 0.4f;
    radialGradientMaskPara->SetRadiusX(radiusX);
    radialGradientMaskPara->SetRadiusY(radiusY);
    std::vector<float> colors = { 0.0f, 0.5f, 1.0f };
    std::vector<float> positions = { 0.0f, 0.5f, 1.0f };
    radialGradientMaskPara->SetColors(colors);
    radialGradientMaskPara->SetPositions(positions);
    para->SetMask(radialGradientMaskPara);

    AdaptiveFrostedGlassParams darkParams;
    darkParams.blurParams = Vector2f(50.0f, 5.0f);
    darkParams.weightsEmboss = Vector2f(2.0f, 3.0f);
    darkParams.bgRates = Vector2f(-0.00004f, 1.3f);
    darkParams.bgKBS = Vector3f(0.02f, 0.01f, 1.3f);
    darkParams.bgPos = Vector3f(0.4f, 0.6f, 1.1f);
    darkParams.bgNeg = Vector3f(0.6f, 0.6f, 1.1f);
    para->SetDarkAdaptiveParams(darkParams);

    Parcel parcel;
    EXPECT_TRUE(para->Marshalling(parcel));

    std::shared_ptr<FilterPara> val = nullptr;
    EXPECT_TRUE(FilterPara::Unmarshalling(parcel, val));
    EXPECT_NE(nullptr, val);
    auto frostedGlassPara = std::static_pointer_cast<FrostedGlassPara>(val);
    EXPECT_NE(nullptr, frostedGlassPara->GetMask());
    EXPECT_NE(nullptr, frostedGlassPara->GetDarkAdaptiveParams());
    EXPECT_EQ(darkParams.blurParams, frostedGlassPara->GetDarkAdaptiveParams()->blurParams);
    EXPECT_EQ(darkParams.weightsEmboss, frostedGlassPara->GetDarkAdaptiveParams()->weightsEmboss);
    EXPECT_EQ(darkParams.bgRates, frostedGlassPara->GetDarkAdaptiveParams()->bgRates);
    EXPECT_EQ(darkParams.bgKBS, frostedGlassPara->GetDarkAdaptiveParams()->bgKBS);
    EXPECT_EQ(darkParams.bgPos, frostedGlassPara->GetDarkAdaptiveParams()->bgPos);
    EXPECT_EQ(darkParams.bgNeg, frostedGlassPara->GetDarkAdaptiveParams()->bgNeg);
    EXPECT_NE(nullptr, para->Clone());
}

/**
 * @tc.name: RSUIEffectFrostedGlassParaUnmarshallingFailTest
 * @tc.desc: Verify the FrostedGlassPara OnUnmarshalling failure branches
 * @tc.type: FUNC
 */
HWTEST_F(RSUIEffectFilterTest, RSUIEffectFrostedGlassParaUnmarshallingFailTest, TestSize.Level1)
{
    std::shared_ptr<FilterPara> valTest = nullptr;
    Parcel parcelTest;

    EXPECT_FALSE(FrostedGlassPara::OnUnmarshalling(parcelTest, valTest));

    parcelTest.FlushBuffer();
    parcelTest.WriteUint16(666);
    EXPECT_FALSE(FrostedGlassPara::OnUnmarshalling(parcelTest, valTest));

    parcelTest.FlushBuffer();
    parcelTest.WriteUint16(static_cast<uint16_t>(FilterPara::ParaType::FROSTED_GLASS));
    EXPECT_FALSE(FrostedGlassPara::OnUnmarshalling(parcelTest, valTest));

    constexpr int BLUR_REFRACT_FLOATS = 9;
    constexpr int BG_EXTRA_FLOATS = 4;
    constexpr int FLOATS_FOR_BG_FAIL = BLUR_REFRACT_FLOATS + BG_EXTRA_FLOATS;

    parcelTest.FlushBuffer();
    parcelTest.WriteUint16(static_cast<uint16_t>(FilterPara::ParaType::FROSTED_GLASS));
    parcelTest.WriteUint16(static_cast<uint16_t>(FilterPara::ParaType::FROSTED_GLASS));
    for (int i = 0; i < FLOATS_FOR_BG_FAIL; ++i) {
        parcelTest.WriteFloat(1.0f);
    }
    parcelTest.WriteBool(true);
    EXPECT_FALSE(FrostedGlassPara::OnUnmarshalling(parcelTest, valTest));

    parcelTest.FlushBuffer();
    parcelTest.WriteUint16(static_cast<uint16_t>(FilterPara::ParaType::FROSTED_GLASS));
    parcelTest.WriteUint16(static_cast<uint16_t>(FilterPara::ParaType::FROSTED_GLASS));
    for (int i = 0; i < FLOATS_FOR_BG_FAIL; ++i) {
        parcelTest.WriteFloat(1.0f);
    }
    parcelTest.WriteBool(false);
    parcelTest.WriteBool(true);
    EXPECT_FALSE(FrostedGlassPara::OnUnmarshalling(parcelTest, valTest));
}

/**
 * @tc.name: RSUIEffectFrostedGlassBlurParaUnmarshallingFailTest
 * @tc.desc: Verify the FrostedGlassBlurPara OnUnmarshalling failure branches
 * @tc.type: FUNC
 */
HWTEST_F(RSUIEffectFilterTest, RSUIEffectFrostedGlassBlurParaUnmarshallingFailTest, TestSize.Level1)
{
    std::shared_ptr<FilterPara> valTest = nullptr;
    Parcel parcelTest;

    EXPECT_FALSE(FrostedGlassBlurPara::OnUnmarshalling(parcelTest, valTest));

    parcelTest.FlushBuffer();
    parcelTest.WriteUint16(666);
    EXPECT_FALSE(FrostedGlassBlurPara::OnUnmarshalling(parcelTest, valTest));

    parcelTest.FlushBuffer();
    parcelTest.WriteUint16(static_cast<uint16_t>(FilterPara::ParaType::FROSTED_GLASS_BLUR));
    EXPECT_FALSE(FrostedGlassBlurPara::OnUnmarshalling(parcelTest, valTest));

    constexpr int BLUR_PARA_TOTAL_FLOATS = 3;
    constexpr int PARTIAL_FLOATS = BLUR_PARA_TOTAL_FLOATS - 1;

    parcelTest.FlushBuffer();
    parcelTest.WriteUint16(static_cast<uint16_t>(FilterPara::ParaType::FROSTED_GLASS_BLUR));
    parcelTest.WriteUint16(static_cast<uint16_t>(FilterPara::ParaType::FROSTED_GLASS_BLUR));
    for (int i = 0; i < PARTIAL_FLOATS; ++i) {
        parcelTest.WriteFloat(1.0f);
    }
    EXPECT_FALSE(FrostedGlassBlurPara::OnUnmarshalling(parcelTest, valTest));
}
} // namespace Rosen
} // namespace OHOS