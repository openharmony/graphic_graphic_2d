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

struct FrostedGlassFloatCounts {
    // Vector2f(2) + Vector2f(2) + Vector2f(2) + Vector3f(3)
    static constexpr int blurRefractFloats = 9;
    // Vector2f(2) + Vector3f(3) + Vector3f(3) + Vector3f(3) + float(1)
    static constexpr int bgParamsFloats = 12;
    // Vector3f(3) + Vector2f(2) + Vector3f(3) + Vector3f(3) + Vector3f(3)
    static constexpr int sdParamsFloats = 14;
    // Vector3f(3) + Vector2f(2) + Vector3f(3) + Vector3f(3) + Vector3f(3)
    static constexpr int envLightFloats = 14;
    // Vector2f*4(8) + Vector3f*3(9)
    static constexpr int edLightFloats = 17;
    static constexpr int totalParamsFloats =
        blurRefractFloats + bgParamsFloats + sdParamsFloats + envLightFloats + edLightFloats;
    // float(1) + float(1) + float(1) + bool(1)
    static constexpr int blurParaFloats = 3;
};

void PrepareFrostedGlassParcel(Parcel& parcel, int floatCount)
{
    parcel.FlushBuffer();
    parcel.WriteUint16(static_cast<uint16_t>(FilterPara::ParaType::FROSTED_GLASS));
    parcel.WriteUint16(static_cast<uint16_t>(FilterPara::ParaType::FROSTED_GLASS));
    for (int i = 0; i < floatCount; ++i) {
        parcel.WriteFloat(1.0f);
    }
}

void PrepareFrostedGlassBlurParcel(Parcel& parcel, int floatCount)
{
    parcel.FlushBuffer();
    parcel.WriteUint16(static_cast<uint16_t>(FilterPara::ParaType::FROSTED_GLASS_BLUR));
    parcel.WriteUint16(static_cast<uint16_t>(FilterPara::ParaType::FROSTED_GLASS_BLUR));
    for (int i = 0; i < floatCount; ++i) {
        parcel.WriteFloat(1.0f);
    }
}

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
    Filter::RegisterUnmarshallingCallback();
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
 * @tc.name: RSUIEffectFilterRegisterUnmarshallingCallbackTest
 * @tc.desc: Verify the Filter::RegisterUnmarshallingCallback static func
 * @tc.type: FUNC
 */
HWTEST_F(RSUIEffectFilterTest, RSUIEffectFilterRegisterUnmarshallingCallbackTest, TestSize.Level1)
{
    auto& singleton = FilterUnmarshallingSingleton::GetInstance();
    EXPECT_NE(nullptr, singleton.GetCallback(static_cast<uint16_t>(FilterPara::ParaType::CONTENT_LIGHT)));
    EXPECT_NE(nullptr, singleton.GetCallback(static_cast<uint16_t>(FilterPara::ParaType::DISPERSION)));
    EXPECT_NE(nullptr, singleton.GetCallback(static_cast<uint16_t>(FilterPara::ParaType::DISPLACEMENT_DISTORT)));
    EXPECT_NE(nullptr, singleton.GetCallback(static_cast<uint16_t>(FilterPara::ParaType::MASK_TRANSITION)));
    EXPECT_NE(nullptr, singleton.GetCallback(static_cast<uint16_t>(FilterPara::ParaType::WATER_RIPPLE)));
    EXPECT_NE(nullptr, singleton.GetCallback(static_cast<uint16_t>(FilterPara::ParaType::FROSTED_GLASS)));
    EXPECT_NE(nullptr, singleton.GetCallback(static_cast<uint16_t>(FilterPara::ParaType::FROSTED_GLASS_BLUR)));
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

namespace {
constexpr float FROSTED_BLUR_PARAMS_X = 48.0f;
constexpr float FROSTED_BLUR_PARAMS_Y = 4.0f;
constexpr float FROSTED_WEIGHTS_EMBOSS_X = 1.0f;
constexpr float FROSTED_WEIGHTS_EMBOSS_Y = 2.0f;
constexpr float FROSTED_WEIGHTS_EDL_X = 0.5f;
constexpr float FROSTED_WEIGHTS_EDL_Y = 0.6f;
constexpr float FROSTED_BG_RATES_X = -0.00003f;
constexpr float FROSTED_BG_RATES_Y = 1.2f;
constexpr float FROSTED_BG_KBS_X = 0.010834f;
constexpr float FROSTED_BG_KBS_Y = 0.007349f;
constexpr float FROSTED_BG_KBS_Z = 1.2f;
constexpr float FROSTED_BG_POS_X = 0.3f;
constexpr float FROSTED_BG_POS_Y = 0.5f;
constexpr float FROSTED_BG_POS_Z = 1.0f;
constexpr float FROSTED_BG_NEG_X = 0.5f;
constexpr float FROSTED_BG_NEG_Y = 0.5f;
constexpr float FROSTED_BG_NEG_Z = 1.0f;
constexpr float FROSTED_BG_ALPHA = 0.8f;
constexpr float FROSTED_REFRACT_X = 1.0f;
constexpr float FROSTED_REFRACT_Y = 2.0f;
constexpr float FROSTED_REFRACT_Z = 3.0f;
constexpr float FROSTED_SD_PARAMS_X = 0.1f;
constexpr float FROSTED_SD_PARAMS_Y = 0.2f;
constexpr float FROSTED_SD_PARAMS_Z = 0.3f;
constexpr float FROSTED_SD_RATES_X = 0.4f;
constexpr float FROSTED_SD_RATES_Y = 0.5f;
constexpr float FROSTED_SD_KBS_X = 0.6f;
constexpr float FROSTED_SD_KBS_Y = 0.7f;
constexpr float FROSTED_SD_KBS_Z = 0.8f;
constexpr float FROSTED_SD_POS_X = 0.9f;
constexpr float FROSTED_SD_POS_Y = 1.0f;
constexpr float FROSTED_SD_POS_Z = 1.1f;
constexpr float FROSTED_SD_NEG_X = 1.2f;
constexpr float FROSTED_SD_NEG_Y = 1.3f;
constexpr float FROSTED_SD_NEG_Z = 1.4f;
constexpr float FROSTED_ENV_LIGHT_PARAMS_X = 2.0f;
constexpr float FROSTED_ENV_LIGHT_PARAMS_Y = 3.0f;
constexpr float FROSTED_ENV_LIGHT_PARAMS_Z = 4.0f;
constexpr float FROSTED_ENV_LIGHT_RATES_X = 5.0f;
constexpr float FROSTED_ENV_LIGHT_RATES_Y = 6.0f;
constexpr float FROSTED_ENV_LIGHT_KBS_X = 7.0f;
constexpr float FROSTED_ENV_LIGHT_KBS_Y = 8.0f;
constexpr float FROSTED_ENV_LIGHT_KBS_Z = 9.0f;
constexpr float FROSTED_ENV_LIGHT_POS_X = 10.0f;
constexpr float FROSTED_ENV_LIGHT_POS_Y = 11.0f;
constexpr float FROSTED_ENV_LIGHT_POS_Z = 12.0f;
constexpr float FROSTED_ENV_LIGHT_NEG_X = 13.0f;
constexpr float FROSTED_ENV_LIGHT_NEG_Y = 14.0f;
constexpr float FROSTED_ENV_LIGHT_NEG_Z = 15.0f;
constexpr float FROSTED_ED_LIGHT_PARAMS_X = 16.0f;
constexpr float FROSTED_ED_LIGHT_PARAMS_Y = 17.0f;
constexpr float FROSTED_ED_LIGHT_ANGLES_X = 18.0f;
constexpr float FROSTED_ED_LIGHT_ANGLES_Y = 19.0f;
constexpr float FROSTED_ED_LIGHT_DIR_X = 20.0f;
constexpr float FROSTED_ED_LIGHT_DIR_Y = 21.0f;
constexpr float FROSTED_ED_LIGHT_RATES_X = 22.0f;
constexpr float FROSTED_ED_LIGHT_RATES_Y = 23.0f;
constexpr float FROSTED_ED_LIGHT_KBS_X = 24.0f;
constexpr float FROSTED_ED_LIGHT_KBS_Y = 25.0f;
constexpr float FROSTED_ED_LIGHT_KBS_Z = 26.0f;
constexpr float FROSTED_ED_LIGHT_POS_X = 27.0f;
constexpr float FROSTED_ED_LIGHT_POS_Y = 28.0f;
constexpr float FROSTED_ED_LIGHT_POS_Z = 29.0f;
constexpr float FROSTED_ED_LIGHT_NEG_X = 30.0f;
constexpr float FROSTED_ED_LIGHT_NEG_Y = 31.0f;
constexpr float FROSTED_ED_LIGHT_NEG_Z = 32.0f;
constexpr bool FROSTED_BASE_VIBRANCY_ENABLED = false;
constexpr float FROSTED_BASE_MATERIAL_TYPE = 1.5f;
constexpr float FROSTED_MATERIAL_COLOR_X = 0.1f;
constexpr float FROSTED_MATERIAL_COLOR_Y = 0.2f;
constexpr float FROSTED_MATERIAL_COLOR_Z = 0.3f;
constexpr float FROSTED_MATERIAL_COLOR_W = 0.4f;
constexpr float FROSTED_DARK_SCALE = 0.5f;
constexpr float FROSTED_SAMPLING_SCALE = 0.75f;
constexpr bool FROSTED_SKIP_FRAME_ENABLE = true;
} // namespace

static void SetupBlurWeightsBgParams(FrostedGlassPara& para)
{
    Vector2f blurParams(FROSTED_BLUR_PARAMS_X, FROSTED_BLUR_PARAMS_Y);
    para.SetBlurParams(blurParams);
    Vector2f weightsEmboss(FROSTED_WEIGHTS_EMBOSS_X, FROSTED_WEIGHTS_EMBOSS_Y);
    para.SetWeightsEmboss(weightsEmboss);
    Vector2f weightsEdl(FROSTED_WEIGHTS_EDL_X, FROSTED_WEIGHTS_EDL_Y);
    para.SetWeightsEdl(weightsEdl);
    Vector2f bgRates(FROSTED_BG_RATES_X, FROSTED_BG_RATES_Y);
    para.SetBgRates(bgRates);
    Vector3f bgKBS(FROSTED_BG_KBS_X, FROSTED_BG_KBS_Y, FROSTED_BG_KBS_Z);
    para.SetBgKBS(bgKBS);
    Vector3f bgPos(FROSTED_BG_POS_X, FROSTED_BG_POS_Y, FROSTED_BG_POS_Z);
    para.SetBgPos(bgPos);
    Vector3f bgNeg(FROSTED_BG_NEG_X, FROSTED_BG_NEG_Y, FROSTED_BG_NEG_Z);
    para.SetBgNeg(bgNeg);
    para.SetBgAlpha(FROSTED_BG_ALPHA);
    Vector3f refractParams(FROSTED_REFRACT_X, FROSTED_REFRACT_Y, FROSTED_REFRACT_Z);
    para.SetRefractParams(refractParams);
}

static void SetupSdEnvLightParams(FrostedGlassPara& para)
{
    Vector3f sdParams(FROSTED_SD_PARAMS_X, FROSTED_SD_PARAMS_Y, FROSTED_SD_PARAMS_Z);
    para.SetSdParams(sdParams);
    Vector2f sdRates(FROSTED_SD_RATES_X, FROSTED_SD_RATES_Y);
    para.SetSdRates(sdRates);
    Vector3f sdKBS(FROSTED_SD_KBS_X, FROSTED_SD_KBS_Y, FROSTED_SD_KBS_Z);
    para.SetSdKBS(sdKBS);
    Vector3f sdPos(FROSTED_SD_POS_X, FROSTED_SD_POS_Y, FROSTED_SD_POS_Z);
    para.SetSdPos(sdPos);
    Vector3f sdNeg(FROSTED_SD_NEG_X, FROSTED_SD_NEG_Y, FROSTED_SD_NEG_Z);
    para.SetSdNeg(sdNeg);
    Vector3f envLightParams(FROSTED_ENV_LIGHT_PARAMS_X, FROSTED_ENV_LIGHT_PARAMS_Y, FROSTED_ENV_LIGHT_PARAMS_Z);
    para.SetEnvLightParams(envLightParams);
    Vector2f envLightRates(FROSTED_ENV_LIGHT_RATES_X, FROSTED_ENV_LIGHT_RATES_Y);
    para.SetEnvLightRates(envLightRates);
    Vector3f envLightKBS(FROSTED_ENV_LIGHT_KBS_X, FROSTED_ENV_LIGHT_KBS_Y, FROSTED_ENV_LIGHT_KBS_Z);
    para.SetEnvLightKBS(envLightKBS);
    Vector3f envLightPos(FROSTED_ENV_LIGHT_POS_X, FROSTED_ENV_LIGHT_POS_Y, FROSTED_ENV_LIGHT_POS_Z);
    para.SetEnvLightPos(envLightPos);
    Vector3f envLightNeg(FROSTED_ENV_LIGHT_NEG_X, FROSTED_ENV_LIGHT_NEG_Y, FROSTED_ENV_LIGHT_NEG_Z);
    para.SetEnvLightNeg(envLightNeg);
}

static void SetupEdLightFinalParams(FrostedGlassPara& para)
{
    Vector2f edLightParams(FROSTED_ED_LIGHT_PARAMS_X, FROSTED_ED_LIGHT_PARAMS_Y);
    para.SetEdLightParams(edLightParams);
    Vector2f edLightAngles(FROSTED_ED_LIGHT_ANGLES_X, FROSTED_ED_LIGHT_ANGLES_Y);
    para.SetEdLightAngles(edLightAngles);
    Vector2f edLightDir(FROSTED_ED_LIGHT_DIR_X, FROSTED_ED_LIGHT_DIR_Y);
    para.SetEdLightDir(edLightDir);
    Vector2f edLightRates(FROSTED_ED_LIGHT_RATES_X, FROSTED_ED_LIGHT_RATES_Y);
    para.SetEdLightRates(edLightRates);
    Vector3f edLightKBS(FROSTED_ED_LIGHT_KBS_X, FROSTED_ED_LIGHT_KBS_Y, FROSTED_ED_LIGHT_KBS_Z);
    para.SetEdLightKBS(edLightKBS);
    Vector3f edLightPos(FROSTED_ED_LIGHT_POS_X, FROSTED_ED_LIGHT_POS_Y, FROSTED_ED_LIGHT_POS_Z);
    para.SetEdLightPos(edLightPos);
    Vector3f edLightNeg(FROSTED_ED_LIGHT_NEG_X, FROSTED_ED_LIGHT_NEG_Y, FROSTED_ED_LIGHT_NEG_Z);
    para.SetEdLightNeg(edLightNeg);
    para.SetBaseVibrancyEnabled(FROSTED_BASE_VIBRANCY_ENABLED);
    para.SetBaseMaterialType(FROSTED_BASE_MATERIAL_TYPE);
    Vector4f materialColor(
        FROSTED_MATERIAL_COLOR_X, FROSTED_MATERIAL_COLOR_Y, FROSTED_MATERIAL_COLOR_Z, FROSTED_MATERIAL_COLOR_W);
    para.SetMaterialColor(materialColor);
    para.SetDarkScale(FROSTED_DARK_SCALE);
    para.SetSamplingScale(FROSTED_SAMPLING_SCALE);
    para.SetSkipFrameEnable(FROSTED_SKIP_FRAME_ENABLE);
}

static void SetupFrostedGlassParaParams(FrostedGlassPara& para)
{
    SetupBlurWeightsBgParams(para);
    SetupSdEnvLightParams(para);
    SetupEdLightFinalParams(para);
}

static void VerifyFrostedGlassParaProperties(const FrostedGlassPara& para)
{
    EXPECT_EQ(Vector2f(FROSTED_BLUR_PARAMS_X, FROSTED_BLUR_PARAMS_Y), para.GetBlurParams());
    EXPECT_EQ(Vector2f(FROSTED_WEIGHTS_EMBOSS_X, FROSTED_WEIGHTS_EMBOSS_Y), para.GetWeightsEmboss());
    EXPECT_EQ(Vector2f(FROSTED_WEIGHTS_EDL_X, FROSTED_WEIGHTS_EDL_Y), para.GetWeightsEdl());
    EXPECT_EQ(Vector2f(FROSTED_BG_RATES_X, FROSTED_BG_RATES_Y), para.GetBgRates());
    EXPECT_EQ(Vector3f(FROSTED_BG_KBS_X, FROSTED_BG_KBS_Y, FROSTED_BG_KBS_Z), para.GetBgKBS());
    EXPECT_EQ(Vector3f(FROSTED_BG_POS_X, FROSTED_BG_POS_Y, FROSTED_BG_POS_Z), para.GetBgPos());
    EXPECT_EQ(Vector3f(FROSTED_BG_NEG_X, FROSTED_BG_NEG_Y, FROSTED_BG_NEG_Z), para.GetBgNeg());
    EXPECT_EQ(FROSTED_BG_ALPHA, para.GetBgAlpha());
    EXPECT_EQ(Vector3f(FROSTED_REFRACT_X, FROSTED_REFRACT_Y, FROSTED_REFRACT_Z), para.GetRefractParams());
    EXPECT_EQ(Vector3f(FROSTED_SD_PARAMS_X, FROSTED_SD_PARAMS_Y, FROSTED_SD_PARAMS_Z), para.GetSdParams());
    EXPECT_EQ(Vector2f(FROSTED_SD_RATES_X, FROSTED_SD_RATES_Y), para.GetSdRates());
    EXPECT_EQ(Vector3f(FROSTED_SD_KBS_X, FROSTED_SD_KBS_Y, FROSTED_SD_KBS_Z), para.GetSdKBS());
    EXPECT_EQ(Vector3f(FROSTED_SD_POS_X, FROSTED_SD_POS_Y, FROSTED_SD_POS_Z), para.GetSdPos());
    EXPECT_EQ(Vector3f(FROSTED_SD_NEG_X, FROSTED_SD_NEG_Y, FROSTED_SD_NEG_Z), para.GetSdNeg());
    EXPECT_EQ(Vector3f(FROSTED_ENV_LIGHT_PARAMS_X, FROSTED_ENV_LIGHT_PARAMS_Y, FROSTED_ENV_LIGHT_PARAMS_Z),
        para.GetEnvLightParams());
    EXPECT_EQ(Vector2f(FROSTED_ENV_LIGHT_RATES_X, FROSTED_ENV_LIGHT_RATES_Y), para.GetEnvLightRates());
    EXPECT_EQ(
        Vector3f(FROSTED_ENV_LIGHT_KBS_X, FROSTED_ENV_LIGHT_KBS_Y, FROSTED_ENV_LIGHT_KBS_Z), para.GetEnvLightKBS());
    EXPECT_EQ(
        Vector3f(FROSTED_ENV_LIGHT_POS_X, FROSTED_ENV_LIGHT_POS_Y, FROSTED_ENV_LIGHT_POS_Z), para.GetEnvLightPos());
    EXPECT_EQ(
        Vector3f(FROSTED_ENV_LIGHT_NEG_X, FROSTED_ENV_LIGHT_NEG_Y, FROSTED_ENV_LIGHT_NEG_Z), para.GetEnvLightNeg());
    EXPECT_EQ(Vector2f(FROSTED_ED_LIGHT_PARAMS_X, FROSTED_ED_LIGHT_PARAMS_Y), para.GetEdLightParams());
    EXPECT_EQ(Vector2f(FROSTED_ED_LIGHT_ANGLES_X, FROSTED_ED_LIGHT_ANGLES_Y), para.GetEdLightAngles());
    EXPECT_EQ(Vector2f(FROSTED_ED_LIGHT_DIR_X, FROSTED_ED_LIGHT_DIR_Y), para.GetEdLightDir());
    EXPECT_EQ(Vector2f(FROSTED_ED_LIGHT_RATES_X, FROSTED_ED_LIGHT_RATES_Y), para.GetEdLightRates());
    EXPECT_EQ(Vector3f(FROSTED_ED_LIGHT_KBS_X, FROSTED_ED_LIGHT_KBS_Y, FROSTED_ED_LIGHT_KBS_Z), para.GetEdLightKBS());
    EXPECT_EQ(Vector3f(FROSTED_ED_LIGHT_POS_X, FROSTED_ED_LIGHT_POS_Y, FROSTED_ED_LIGHT_POS_Z), para.GetEdLightPos());
    EXPECT_EQ(Vector3f(FROSTED_ED_LIGHT_NEG_X, FROSTED_ED_LIGHT_NEG_Y, FROSTED_ED_LIGHT_NEG_Z), para.GetEdLightNeg());
    EXPECT_EQ(FROSTED_BASE_VIBRANCY_ENABLED, para.GetBaseVibrancyEnabled());
    EXPECT_EQ(FROSTED_BASE_MATERIAL_TYPE, para.GetBaseMaterialType());
    EXPECT_EQ(Vector4f(FROSTED_MATERIAL_COLOR_X, FROSTED_MATERIAL_COLOR_Y, FROSTED_MATERIAL_COLOR_Z,
        FROSTED_MATERIAL_COLOR_W), para.GetMaterialColor());
    EXPECT_EQ(FROSTED_DARK_SCALE, para.GetDarkScale());
    EXPECT_EQ(FROSTED_SAMPLING_SCALE, para.GetSamplingScale());
    EXPECT_EQ(nullptr, para.GetMask());
    EXPECT_EQ(nullptr, para.GetDarkAdaptiveParams());
    EXPECT_EQ(FROSTED_SKIP_FRAME_ENABLE, para.GetSkipFrameEnable());
}

/**
 * @tc.name: RSUIEffectFrostedGlassParaTest
 * @tc.desc: Verify the FrostedGlassPara Marshalling and Unmarshalling without optional params
 * @tc.type: FUNC
 */
HWTEST_F(RSUIEffectFilterTest, RSUIEffectFrostedGlassParaTest, TestSize.Level1)
{
    auto para = std::make_shared<FrostedGlassPara>();
    SetupFrostedGlassParaParams(*para);

    Parcel parcel;
    EXPECT_TRUE(para->Marshalling(parcel));

    std::shared_ptr<FilterPara> val = nullptr;
    EXPECT_TRUE(FilterPara::Unmarshalling(parcel, val));
    EXPECT_NE(nullptr, val);
    auto frostedGlassPara = std::static_pointer_cast<FrostedGlassPara>(val);
    VerifyFrostedGlassParaProperties(*frostedGlassPara);
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

    // ReadBgParams fail: write blur+refract params (9 floats) + partial bg params (4 floats)
    constexpr int floatsForBgFail = FrostedGlassFloatCounts::blurRefractFloats + 4;

    PrepareFrostedGlassParcel(parcelTest, floatsForBgFail);
    parcelTest.WriteBool(true);
    EXPECT_FALSE(FrostedGlassPara::OnUnmarshalling(parcelTest, valTest));

    // ReadAdaptiveParams fail after mask=false
    PrepareFrostedGlassParcel(parcelTest, floatsForBgFail);
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

    PrepareFrostedGlassBlurParcel(parcelTest, FrostedGlassFloatCounts::blurParaFloats - 1);
    EXPECT_FALSE(FrostedGlassBlurPara::OnUnmarshalling(parcelTest, valTest));
}

/**
 * @tc.name: RSUIEffectFrostedGlassParaMarshallingWithInvalidMaskTest
 * @tc.desc: Verify the FrostedGlassPara Marshalling fails when maskPara_->Marshalling fails
 * @tc.type: FUNC
 */
HWTEST_F(RSUIEffectFilterTest, RSUIEffectFrostedGlassParaMarshallingWithInvalidMaskTest, TestSize.Level1)
{
    auto para = std::make_shared<FrostedGlassPara>();
    para->SetMask(std::make_shared<MaskPara>());

    Parcel parcel;
    EXPECT_FALSE(para->Marshalling(parcel));
}

/**
 * @tc.name: RSUIEffectFrostedGlassParaUnmarshallingMaskFailTest
 * @tc.desc: Verify the FrostedGlassPara OnUnmarshalling fails when MaskPara::Unmarshalling fails
 * @tc.type: FUNC
 */
HWTEST_F(RSUIEffectFilterTest, RSUIEffectFrostedGlassParaUnmarshallingMaskFailTest, TestSize.Level1)
{
    std::shared_ptr<FilterPara> valTest = nullptr;
    Parcel parcelTest;

    parcelTest.WriteUint16(static_cast<uint16_t>(FilterPara::ParaType::FROSTED_GLASS));
    parcelTest.WriteUint16(static_cast<uint16_t>(FilterPara::ParaType::FROSTED_GLASS));
    for (int i = 0; i < FrostedGlassFloatCounts::totalParamsFloats; ++i) {
        parcelTest.WriteFloat(1.0f);
    }
    parcelTest.WriteBool(true);
    parcelTest.WriteUint16(666);
    EXPECT_FALSE(FrostedGlassPara::OnUnmarshalling(parcelTest, valTest));
}

/**
 * @tc.name: RSUIEffectFrostedGlassParaReadParamsBranchTest
 * @tc.desc: Verify the FrostedGlassPara Read functions branch coverage at different truncate positions
 * @tc.type: FUNC
 */
HWTEST_F(RSUIEffectFilterTest, RSUIEffectFrostedGlassParaReadParamsBranchTest, TestSize.Level1)
{
    std::shared_ptr<FilterPara> valTest = nullptr;
    Parcel parcelTest;

    constexpr int afterBgParams = FrostedGlassFloatCounts::blurRefractFloats + FrostedGlassFloatCounts::bgParamsFloats;
    constexpr int afterSdParams = afterBgParams + FrostedGlassFloatCounts::sdParamsFloats;
    constexpr int afterEnvLight = afterSdParams + FrostedGlassFloatCounts::envLightFloats;
    constexpr int afterEdLight = afterEnvLight + FrostedGlassFloatCounts::edLightFloats;

    PrepareFrostedGlassParcel(parcelTest, afterBgParams + 1);
    EXPECT_FALSE(FrostedGlassPara::OnUnmarshalling(parcelTest, valTest));

    PrepareFrostedGlassParcel(parcelTest, afterSdParams + 1);
    EXPECT_FALSE(FrostedGlassPara::OnUnmarshalling(parcelTest, valTest));

    PrepareFrostedGlassParcel(parcelTest, afterEnvLight + 1);
    EXPECT_FALSE(FrostedGlassPara::OnUnmarshalling(parcelTest, valTest));

    PrepareFrostedGlassParcel(parcelTest, afterEdLight + 1);
    EXPECT_FALSE(FrostedGlassPara::OnUnmarshalling(parcelTest, valTest));
}

/**
 * @tc.name: RSUIEffectFrostedGlassParaReadBlurRefractBranchTest
 * @tc.desc: Cover missing branches in ReadBlurAndRefractParams (+ + + - + -)
 *           Branch 4: blurParams success, weightsEmboss success, weightsEdl fail
 *           Branch 6: blurParams success, weightsEmboss success, weightsEdl success
 * @tc.type: FUNC
 */
HWTEST_F(RSUIEffectFilterTest, RSUIEffectFrostedGlassParaReadBlurRefractBranchTest, TestSize.Level1)
{
    std::shared_ptr<FilterPara> valTest = nullptr;
    Parcel parcelTest;
    constexpr int blurParamsFloats = 2;    // Vector2f
    constexpr int weightsEmbossFloats = 2; // Vector2f
    constexpr int weightsEdlFloats = 2;    // Vector2f

    // Branch 4: blurParams + weightsEmboss success, weightsEdl fail
    PrepareFrostedGlassParcel(parcelTest, blurParamsFloats + weightsEmbossFloats);
    EXPECT_FALSE(FrostedGlassPara::OnUnmarshalling(parcelTest, valTest));

    // Branch 6: blurParams + weightsEmboss + weightsEdl success, refractParams fail
    PrepareFrostedGlassParcel(parcelTest, blurParamsFloats + weightsEmbossFloats + weightsEdlFloats);
    EXPECT_FALSE(FrostedGlassPara::OnUnmarshalling(parcelTest, valTest));
}

/**
 * @tc.name: RSUIEffectFrostedGlassParaReadBgParamsBranchTest
 * @tc.desc: Cover missing branches in ReadBgParams (+ - + - + + + -)
 *           Branch 2: bgRates success
 *           Branch 4: bgRates + bgKBS success
 *           Branch 8: bgRates + bgKBS + bgPos + bgNeg success, bgAlpha fail
 * @tc.type: FUNC
 */
HWTEST_F(RSUIEffectFilterTest, RSUIEffectFrostedGlassParaReadBgParamsBranchTest, TestSize.Level1)
{
    std::shared_ptr<FilterPara> valTest = nullptr;
    Parcel parcelTest;
    constexpr int bgRatesFloats = 2; // Vector2f
    constexpr int bgKBSFloats = 3;   // Vector3f
    constexpr int bgPosFloats = 3;   // Vector3f
    constexpr int bgNegFloats = 3;   // Vector3f

    // Branch 2: bgRates success, bgKBS fail (after blurRefract success)
    PrepareFrostedGlassParcel(parcelTest, FrostedGlassFloatCounts::blurRefractFloats + bgRatesFloats);
    EXPECT_FALSE(FrostedGlassPara::OnUnmarshalling(parcelTest, valTest));

    // Branch 4: bgRates + bgKBS success, bgPos fail
    PrepareFrostedGlassParcel(parcelTest, FrostedGlassFloatCounts::blurRefractFloats + bgRatesFloats + bgKBSFloats);
    EXPECT_FALSE(FrostedGlassPara::OnUnmarshalling(parcelTest, valTest));

    // Branch 8: bgRates + bgKBS + bgPos + bgNeg success, bgAlpha fail
    PrepareFrostedGlassParcel(parcelTest,
        FrostedGlassFloatCounts::blurRefractFloats + bgRatesFloats + bgKBSFloats + bgPosFloats + bgNegFloats);
    EXPECT_FALSE(FrostedGlassPara::OnUnmarshalling(parcelTest, valTest));
}

/**
 * @tc.name: RSUIEffectFrostedGlassParaReadSdParamsBranchTest
 * @tc.desc: Cover missing branches in ReadSdParams (+ + + - + - + -)
 *           Branch 4: sdParams success, sdRates fail
 *           Branch 6: sdParams + sdRates success, sdKBS fail
 *           Branch 8: sdParams + sdRates + sdKBS + sdPos success, sdNeg fail
 * @tc.type: FUNC
 */
HWTEST_F(RSUIEffectFilterTest, RSUIEffectFrostedGlassParaReadSdParamsBranchTest, TestSize.Level1)
{
    std::shared_ptr<FilterPara> valTest = nullptr;
    Parcel parcelTest;
    constexpr int sdParamsFloats = 3; // Vector3f
    constexpr int sdRatesFloats = 2;  // Vector2f
    constexpr int sdKBSFloats = 3;    // Vector3f
    constexpr int sdPosFloats = 3;    // Vector3f
    constexpr int prefixFloats = FrostedGlassFloatCounts::blurRefractFloats + FrostedGlassFloatCounts::bgParamsFloats;

    // Branch 4: sdParams success, sdRates fail (after blurRefract + bgParams success)
    PrepareFrostedGlassParcel(parcelTest, prefixFloats + sdParamsFloats);
    EXPECT_FALSE(FrostedGlassPara::OnUnmarshalling(parcelTest, valTest));

    // Branch 6: sdParams + sdRates success, sdKBS fail
    PrepareFrostedGlassParcel(parcelTest, prefixFloats + sdParamsFloats + sdRatesFloats);
    EXPECT_FALSE(FrostedGlassPara::OnUnmarshalling(parcelTest, valTest));

    // Branch 8: sdParams + sdRates + sdKBS + sdPos success, sdNeg fail
    PrepareFrostedGlassParcel(parcelTest, prefixFloats + sdParamsFloats + sdRatesFloats + sdKBSFloats + sdPosFloats);
    EXPECT_FALSE(FrostedGlassPara::OnUnmarshalling(parcelTest, valTest));
}

/**
 * @tc.name: RSUIEffectFrostedGlassParaReadEnvLightParamsBranchTest
 * @tc.desc: Cover missing branches in ReadEnvLightParams (+ + + - + -)
 *           Branch 4: envLightParams success, envLightRates fail
 *           Branch 6: envLightParams + envLightRates success, envLightKBS fail
 * @tc.type: FUNC
 */
HWTEST_F(RSUIEffectFilterTest, RSUIEffectFrostedGlassParaReadEnvLightParamsBranchTest, TestSize.Level1)
{
    std::shared_ptr<FilterPara> valTest = nullptr;
    Parcel parcelTest;
    constexpr int envLightParamsFloats = 3; // Vector3f
    constexpr int envLightRatesFloats = 2;  // Vector2f
    constexpr int prefixFloats = FrostedGlassFloatCounts::blurRefractFloats + FrostedGlassFloatCounts::bgParamsFloats +
                                 FrostedGlassFloatCounts::sdParamsFloats;

    // Branch 4: envLightParams success, envLightRates fail
    PrepareFrostedGlassParcel(parcelTest, prefixFloats + envLightParamsFloats);
    EXPECT_FALSE(FrostedGlassPara::OnUnmarshalling(parcelTest, valTest));

    // Branch 6: envLightParams + envLightRates success, envLightKBS fail
    PrepareFrostedGlassParcel(parcelTest, prefixFloats + envLightParamsFloats + envLightRatesFloats);
    EXPECT_FALSE(FrostedGlassPara::OnUnmarshalling(parcelTest, valTest));
}

/**
 * @tc.name: RSUIEffectFrostedGlassParaReadEdLightParamsBranchTest
 * @tc.desc: Cover missing branches in ReadEdLightParams (+ - + + + -)
 *           Branch 2: edLightParams success, edLightAngles fail
 *           Branch 6: edLightParams + edLightAngles + edLightDir success, edLightRates fail
 * @tc.type: FUNC
 */
HWTEST_F(RSUIEffectFilterTest, RSUIEffectFrostedGlassParaReadEdLightParamsBranchTest, TestSize.Level1)
{
    std::shared_ptr<FilterPara> valTest = nullptr;
    Parcel parcelTest;
    constexpr int edLightParamsFloats = 2; // Vector2f
    constexpr int edLightAnglesFloats = 2; // Vector2f
    constexpr int edLightDirFloats = 2;    // Vector2f
    constexpr int edLightRatesFloats = 2;  // Vector2f
    constexpr int edLightKBSFloats = 3;    // Vector3f
    constexpr int prefixFloats = FrostedGlassFloatCounts::blurRefractFloats + FrostedGlassFloatCounts::bgParamsFloats +
                                 FrostedGlassFloatCounts::sdParamsFloats + FrostedGlassFloatCounts::envLightFloats;

    // Branch 2: edLightParams success, edLightAngles fail
    PrepareFrostedGlassParcel(parcelTest, prefixFloats + edLightParamsFloats);
    EXPECT_FALSE(FrostedGlassPara::OnUnmarshalling(parcelTest, valTest));

    // Branch 6: edLightParams + edLightAngles + edLightDir + edLightRates + edLightKBS success, edLightPos fail
    PrepareFrostedGlassParcel(parcelTest, prefixFloats + edLightParamsFloats + edLightAnglesFloats + edLightDirFloats +
                                              edLightRatesFloats + edLightKBSFloats);
    EXPECT_FALSE(FrostedGlassPara::OnUnmarshalling(parcelTest, valTest));
}

/**
 * @tc.name: RSUIEffectFrostedGlassParaWriteBaseParamsBranchTest
 * @tc.desc: Cover missing branches in WriteBaseParams (+ - + - + -)
 *           Branch 2: WriteBool success, WriteFloat fail
 *           Branch 4: WriteBool + WriteFloat success, WriteVector4f fail
 *           Branch 6: WriteBool + WriteFloat + WriteVector4f success, WriteFloat(darkScale) fail
 * @tc.type: FUNC
 */
HWTEST_F(RSUIEffectFilterTest, RSUIEffectFrostedGlassParaWriteBaseParamsBranchTest, TestSize.Level1)
{
    std::shared_ptr<FilterPara> valTest = nullptr;
    Parcel parcelTest;
    constexpr int materialColorFloats = 4; // Vector4f

    // Branch 2: WriteBool success, WriteFloat(baseMaterialType) fail
    PrepareFrostedGlassParcel(parcelTest, FrostedGlassFloatCounts::totalParamsFloats);
    parcelTest.WriteBool(true);
    EXPECT_FALSE(FrostedGlassPara::OnUnmarshalling(parcelTest, valTest));

    // Branch 4: WriteBool + WriteFloat(baseMaterialType) success, WriteVector4f fail
    PrepareFrostedGlassParcel(parcelTest, FrostedGlassFloatCounts::totalParamsFloats);
    parcelTest.WriteBool(true);
    parcelTest.WriteFloat(1.0f);
    EXPECT_FALSE(FrostedGlassPara::OnUnmarshalling(parcelTest, valTest));

    // Branch 6: WriteBool + WriteFloat + WriteVector4f success, WriteFloat(darkScale) fail
    PrepareFrostedGlassParcel(parcelTest, FrostedGlassFloatCounts::totalParamsFloats);
    parcelTest.WriteBool(true);
    parcelTest.WriteFloat(1.0f);
    for (int i = 0; i < materialColorFloats; ++i) {
        parcelTest.WriteFloat(1.0f);
    }
    EXPECT_FALSE(FrostedGlassPara::OnUnmarshalling(parcelTest, valTest));
}
} // namespace Rosen
} // namespace OHOS