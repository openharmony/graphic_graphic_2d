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
#include "gtest/gtest.h"

#include "ge_visual_effect.h"
#include "ge_visual_effect_container.h"
#include "render/rs_render_content_light_filter.h"
#include "render/rs_shader_mask.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSRenderContentLightFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderContentLightFilterTest::SetUpTestCase() {}
void RSRenderContentLightFilterTest::TearDownTestCase() {}
void RSRenderContentLightFilterTest::SetUp() {}
void RSRenderContentLightFilterTest::TearDown() {}

/**
 * @tc.name: GetDescription001
 * @tc.desc: Verify function GetDescription
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderContentLightFilterTest, GetDescription001, TestSize.Level1)
{
    auto rsRenderFilterPara = std::make_shared<RSRenderContentLightFilterPara>(0);
    std::string temp = "RSRenderContentLightFilterPara";
    std::string out;
    rsRenderFilterPara->GetDescription(out);
    EXPECT_EQ(out, temp);
}

/**
 * @tc.name: WriteToParcel001
 * @tc.desc: test WriteToParcel
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderContentLightFilterTest, WriteToParcel001, TestSize.Level1)
{
    auto filterPara = std::make_shared<RSRenderContentLightFilterPara>(0);
    Parcel parcel1;
    auto ret = filterPara->WriteToParcel(parcel1);
    EXPECT_TRUE(ret);

    for (int i = 0; i < 65540; i++) {
        filterPara->properties_[RSUIFilterType(i)] = std::make_shared<RSRenderContentLightFilterPara>(i);
    }
    Parcel parcel2;
    ret = filterPara->WriteToParcel(parcel2);
    filterPara->properties_.clear();

    Vector3f lightPosition = Vector3f(0.0f, 0.0f, 0.0f);
    auto lightPositionProperty = std::make_shared<RSRenderAnimatableProperty<Vector3f>>(lightPosition);
    filterPara->Setter(RSUIFilterType::LIGHT_POSITION, lightPositionProperty);
    Parcel parcel3;
    ret = filterPara->WriteToParcel(parcel3);
    EXPECT_FALSE(ret);

    Vector4f lightColor = Vector4f(0.1f, 0.2f, 0.3f, 0.0f);
    auto lightColorProperty = std::make_shared<RSRenderAnimatableProperty<Vector4f>>(lightColor);
    filterPara->Setter(RSUIFilterType::LIGHT_COLOR, lightColorProperty);
    Parcel parcel4;
    ret = filterPara->WriteToParcel(parcel4);
    EXPECT_FALSE(ret);

    auto lightIntensityProperty = std::make_shared<RSRenderAnimatableProperty<float>>(0.5f);
    filterPara->Setter(RSUIFilterType::LIGHT_INTENSITY, lightIntensityProperty);
    Parcel parcel5;
    ret = filterPara->WriteToParcel(parcel5);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: ReadFromParcel001
 * @tc.desc: test ReadFromParcel
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderContentLightFilterTest, ReadFromParcel001, TestSize.Level1)
{
    auto filterPara = std::make_shared<RSRenderContentLightFilterPara>(0);
    Parcel parcel;

    Vector3f lightPosition = Vector3f(0.0f, 0.0f, 0.0f);
    auto lightPositionProperty = std::make_shared<RSRenderAnimatableProperty<Vector3f>>(lightPosition);
    filterPara->Setter(RSUIFilterType::LIGHT_POSITION, lightPositionProperty);

    Vector4f lightColor = Vector4f(0.1f, 0.2f, 0.3f, 0.0f);
    auto lightColorProperty = std::make_shared<RSRenderAnimatableProperty<Vector4f>>(lightColor);
    filterPara->Setter(RSUIFilterType::LIGHT_COLOR, lightColorProperty);

    auto lightIntensityProperty = std::make_shared<RSRenderAnimatableProperty<float>>(0.5f);
    filterPara->Setter(RSUIFilterType::LIGHT_INTENSITY, lightIntensityProperty);

    auto ret = filterPara->WriteToParcel(parcel);
    EXPECT_TRUE(ret);

    ret = filterPara->ReadFromParcel(parcel);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: CreateRenderProperty001
 * @tc.desc: test CreateRenderProperty
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderContentLightFilterTest, CreateRenderProperty001, TestSize.Level1)
{
    auto filterPara = std::make_shared<RSRenderContentLightFilterPara>(0);

    auto lightPositionProperty = filterPara->CreateRenderProperty(RSUIFilterType::LIGHT_POSITION);
    EXPECT_NE(lightPositionProperty, nullptr);

    auto lightColorProperty = filterPara->CreateRenderProperty(RSUIFilterType::LIGHT_COLOR);
    EXPECT_NE(lightColorProperty, nullptr);

    auto lightIntensityProperty = filterPara->CreateRenderProperty(RSUIFilterType::LIGHT_INTENSITY);
    EXPECT_NE(lightIntensityProperty, nullptr);

    auto blurProperty = filterPara->CreateRenderProperty(RSUIFilterType::BLUR);
    EXPECT_EQ(blurProperty, nullptr);
}

/**
 * @tc.name: GetLeafRenderProperties001
 * @tc.desc: test GetLeafRenderProperties
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderContentLightFilterTest, GetLeafRenderProperties001, TestSize.Level1)
{
    auto filterPara = std::make_shared<RSRenderContentLightFilterPara>(0);
    auto lightIntensityProperty = std::make_shared<RSRenderAnimatableProperty<float>>(0.5f);
    filterPara->Setter(RSUIFilterType::LIGHT_INTENSITY, lightIntensityProperty);
    auto rsRenderPropertyBaseVec = filterPara->GetLeafRenderProperties();
    EXPECT_FALSE(rsRenderPropertyBaseVec.empty());
}

/**
 * @tc.name: ParseFilterValuesTest001
 * @tc.desc: Verify function ParseFilterValues
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderContentLightFilterTest, ParseFilterValuesTest001, TestSize.Level1)
{
    auto filterPara = std::make_shared<RSRenderContentLightFilterPara>(0);
    bool ret = filterPara->ParseFilterValues();
    EXPECT_FALSE(ret);

    Vector3f lightPosition = Vector3f(0.0f, 0.0f, 0.0f);
    auto lightPositionProperty = std::make_shared<RSRenderAnimatableProperty<Vector3f>>(lightPosition);
    filterPara->Setter(RSUIFilterType::LIGHT_POSITION, lightPositionProperty);
    ret = filterPara->ParseFilterValues();
    EXPECT_FALSE(ret);

    Vector4f lightColor = Vector4f(0.1f, 0.2f, 0.3f, 0.0f);
    auto lightColorProperty = std::make_shared<RSRenderAnimatableProperty<Vector4f>>(lightColor);
    filterPara->Setter(RSUIFilterType::LIGHT_COLOR, lightColorProperty);
    ret = filterPara->ParseFilterValues();
    EXPECT_TRUE(ret);

    auto lightIntensityProperty = std::make_shared<RSRenderAnimatableProperty<float>>(0.5f);
    filterPara->Setter(RSUIFilterType::LIGHT_INTENSITY, lightIntensityProperty);

    ret = filterPara->ParseFilterValues();
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: GetLightPositionTest001
 * @tc.desc: Verify function GetLightPosition
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderContentLightFilterTest, GetLightPositionTest001, TestSize.Level1)
{
    auto filterPara = std::make_shared<RSRenderContentLightFilterPara>(0);
    auto lightPosition = filterPara->GetLightPosition();
    EXPECT_EQ(lightPosition, Vector3f(0.0f, 0.0f, 0.0f));
}

/**
 * @tc.name: GetLightColorTest001
 * @tc.desc: Verify function GetLightColor
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderContentLightFilterTest, GetLightColorTest001, TestSize.Level1)
{
    auto filterPara = std::make_shared<RSRenderContentLightFilterPara>(0);
    auto lightColor = filterPara->GetLightColor();
    EXPECT_EQ(lightColor, Vector4f(0.0f, 0.0f, 0.0f, 0.0f));
}

/**
 * @tc.name: GetLightIntensityTest001
 * @tc.desc: Verify function GetLightIntensity
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderContentLightFilterTest, GetLightIntensityTest001, TestSize.Level1)
{
    auto filterPara = std::make_shared<RSRenderContentLightFilterPara>(0);
    auto lightIntensity = filterPara->GetLightIntensity();
    EXPECT_NE(lightIntensity, 0.0f);
}

/**
 * @tc.name: GetRotationAngleTest001
 * @tc.desc: Verify function GetRotationAngleTest
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderContentLightFilterTest, GetRotationAngleTest001, TestSize.Level1)
{
    auto filterPara = std::make_shared<RSRenderContentLightFilterPara>(0);
    auto rotationAngle = filterPara->GetRotationAngle();
    EXPECT_EQ(rotationAngle, Vector3f(0.0f, 0.0f, 0.0f));
}

/**
 * @tc.name: GenerateGEVisualEffectTest001
 * @tc.desc: Verify function GenerateGEVisualEffect
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderContentLightFilterTest, GenerateGEVisualEffectTest001, TestSize.Level1)
{
    auto filterPara = std::make_shared<RSRenderContentLightFilterPara>(0);
    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    filterPara->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());
}
} // namespace Rosen
} // namespace OHOS