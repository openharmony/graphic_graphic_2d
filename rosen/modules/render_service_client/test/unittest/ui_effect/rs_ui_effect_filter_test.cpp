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
#include "filter/include/filter_para.h"
#include "filter/include/filter_content_light_para.h"
#include "filter/include/filter_dispersion_para.h"
#include "filter/include/filter_displacement_distort_para.h"
#include "filter/include/filter_water_ripple_para.h"
#include "filter/include/filter_mask_transition_para.h"
#include "filter/include/filter_unmarshalling_singleton.h"
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
} // namespace Rosen
} // namespace OHOS