/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include <array>
#include <gtest/gtest.h>
#include <hilog/log.h>
#include <cmath>

#include "color.h"
#include "color_space.h"
#include "color_space_convertor.h"

using namespace testing::ext;

namespace OHOS {
namespace ColorManager {
class ColorManagerTest : public testing::Test {
public:
    static constexpr HiviewDFX::HiLogLabel LOG_LABEL = {LOG_CORE, 0, "ColorManagerTest"};

    static void SetUpTestCase()
    {}

    static void TearDownTestCase()
    {}
};

/*
* Function: ColorManagerTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: sRGB convert to sRGB
*/
HWTEST_F(ColorManagerTest, sRGBTosRGB, Function | SmallTest | Level2)
{
    Color color = Color(0.1, 0.2, 0.3, 0.4);
    Color result = color.Convert(SRGB);
    ASSERT_EQ(FloatEqual(result.r, 0.1f), true);
    ASSERT_EQ(FloatEqual(result.g, 0.2f), true);
    ASSERT_EQ(FloatEqual(result.b, 0.3f), true);
}

/*
* Function: ColorManagerTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: sRGB convert to Display_P3
*/
HWTEST_F(ColorManagerTest, sRGBToDisplay_P3, Function | SmallTest | Level2)
{
    Color color = Color(0.1, 0.2, 0.3, 0.4);
    Color result = color.Convert(ColorSpace(DISPLAY_P3));
    ASSERT_EQ(FloatEqual(result.r, 0.1237f), true);
    ASSERT_EQ(FloatEqual(result.g, 0.1975f), true);
    ASSERT_EQ(FloatEqual(result.b, 0.2918f), true);
}

/*
* Function: ColorManagerTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Display_P3 convert to sRGB
*/
HWTEST_F(ColorManagerTest, Display_P3TosRGB, Function | SmallTest | Level2)
{
    Color color = Color(0.1, 0.2, 0.3, 0.4, DISPLAY_P3);
    Color result = color.Convert(ColorSpace(SRGB));
    ASSERT_EQ(FloatEqual(result.r, 0.0594f), true);
    ASSERT_EQ(FloatEqual(result.g, 0.2031f), true);
    ASSERT_EQ(FloatEqual(result.b, 0.3087f), true);
}

/*
* Function: ColorManagerTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: sRGB convert to Adobe
*/
HWTEST_F(ColorManagerTest, sRGBToAdobe, Function | SmallTest | Level2)
{
    Color color = Color(0.1, 0.2, 0.3, 0.4);
    Color result = color.Convert(ColorSpace(ADOBE_RGB));
    ASSERT_EQ(FloatEqual(result.r, 0.155216f), true);
    ASSERT_EQ(FloatEqual(result.g, 0.212433f), true);
    ASSERT_EQ(FloatEqual(result.b, 0.301627f), true);
}

/*
* Function: ColorManagerTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Adobe convert to sRGB
*/
HWTEST_F(ColorManagerTest, AdobeTosRGB, Function | SmallTest | Level2)
{
    Color color = Color(0.1, 0.2, 0.3, 0.4, ADOBE_RGB);
    Color result = color.Convert(ColorSpace(SRGB));
    ASSERT_EQ(FloatEqual(result.r, 0.0f), true);
    ASSERT_EQ(FloatEqual(result.g, 0.186285f), true);
    ASSERT_EQ(FloatEqual(result.b, 0.298569f), true);
}

/*
* Function: ColorManagerTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: sRGB convert to DCI_P3
*/
HWTEST_F(ColorManagerTest, sRGBToDCI_P3, Function | SmallTest | Level2)
{
    Color color = Color(0.1, 0.2, 0.3, 0.4, SRGB);
    Color result = color.Convert(DCI_P3);
    ASSERT_EQ(FloatEqual(result.r, 0.189058f), true);
    ASSERT_EQ(FloatEqual(result.g, 0.267553f), true);
    ASSERT_EQ(FloatEqual(result.b, 0.358257f), true);
}

/*
* Function: ColorManagerTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: DCI_P3 convert to sRGB
*/
HWTEST_F(ColorManagerTest, DCI_P3TosRGB, Function | SmallTest | Level2)
{
    Color color = Color(0.1, 0.2, 0.3, 0.4, DCI_P3);
    Color result = color.Convert(SRGB);
    ASSERT_EQ(FloatEqual(result.r, 0.005633f), true);
    ASSERT_EQ(FloatEqual(result.g, 0.131560f), true);
    ASSERT_EQ(FloatEqual(result.b, 0.239266f), true);
}

/*
* Function: ColorManagerTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Adobe convert to Display_P3
*/
HWTEST_F(ColorManagerTest, AdobeToDisplay_P3, Function | SmallTest | Level2)
{
    auto convertor = ColorSpaceConvertor(ColorSpace(ADOBE_RGB), ColorSpace(DISPLAY_P3), GAMUT_MAP_CONSTANT);
    std::array<float, 3> rgb = {0.1, 0.2, 0.3};
    auto result = convertor.Convert(rgb);
    ASSERT_EQ(FloatEqual(result[0], 0.0375351f), true);
    ASSERT_EQ(FloatEqual(result[1], 0.182594f), true);
    ASSERT_EQ(FloatEqual(result[2], 0.289389f), true);
}

/*
* Function: ColorManagerTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Display_P3 convert to Adobe
*/
HWTEST_F(ColorManagerTest, Display_P3ToAdobe, Function | SmallTest | Level2)
{
    auto convertor = ColorSpaceConvertor(ColorSpace(DISPLAY_P3), ColorSpace(ADOBE_RGB), GAMUT_MAP_CONSTANT);
    std::array<float, 3> rgb = {0.1, 0.2, 0.3};
    auto result = convertor.Convert(rgb);
    ASSERT_EQ(FloatEqual(result[0], 0.139683f), true);
    ASSERT_EQ(FloatEqual(result[1], 0.215243f), true);
    ASSERT_EQ(FloatEqual(result[2], 0.30965f), true);
}

/*
* Function: ColorManagerTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: DCI_P3 convert to Display_P3
*/
HWTEST_F(ColorManagerTest, DCI_P3ToDisplay_P3, Function | SmallTest | Level2)
{
    Color color = Color(0.1, 0.2, 0.3, 0.4, DCI_P3);
    Color result = color.Convert(DISPLAY_P3);
    ASSERT_EQ(FloatEqual(result.r, 0.04051f), true);
    ASSERT_EQ(FloatEqual(result.g, 0.12905f), true);
    ASSERT_EQ(FloatEqual(result.b, 0.23113f), true);
}

/*
* Function: ColorManagerTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: DISPLAY_P3 convert to DCI_P3
*/
HWTEST_F(ColorManagerTest, Display_P3ToDCI_P3, Function | SmallTest | Level2)
{
    Color color = Color(0.1, 0.2, 0.3, 0.4, DISPLAY_P3);
    Color result = color.Convert(DCI_P3);
    ASSERT_EQ(FloatEqual(result.r, 0.161843f), true);
    ASSERT_EQ(FloatEqual(result.g, 0.269979f), true);
    ASSERT_EQ(FloatEqual(result.b, 0.366057f), true);
}

/*
* Function: ColorManagerTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Adobe convert to DCI_P3
*/
HWTEST_F(ColorManagerTest, AdobeToDCI_P3, Function | SmallTest | Level2)
{
    Color color = Color(0.1, 0.2, 0.3, 0.4, ADOBE_RGB);
    Color result = color.Convert(DCI_P3);
    ASSERT_EQ(FloatEqual(result.r, 0.083082f), true);
    ASSERT_EQ(FloatEqual(result.g, 0.252975f), true);
    ASSERT_EQ(FloatEqual(result.b, 0.355959f), true);
}

/*
* Function: ColorManagerTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: DCI_P3 convert to Adobe
*/
HWTEST_F(ColorManagerTest, DCI_P3ToAdobe, Function | SmallTest | Level2)
{
    Color color = Color(0.1, 0.2, 0.3, 0.4, DCI_P3);
    Color result = color.Convert(ADOBE_RGB);
    ASSERT_EQ(FloatEqual(result.r, 0.0880399f), true);
    ASSERT_EQ(FloatEqual(result.g, 0.151082f), true);
    ASSERT_EQ(FloatEqual(result.b, 0.245245f), true);
}

/*
* Function: ColorManagerTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: sRGB convert to DCI_P3
*/
HWTEST_F(ColorManagerTest, skiaToColorSpace, Function | SmallTest | Level2)
{
    sk_sp<SkColorSpace> skiaSrgb = SkColorSpace::MakeSRGB();
    ASSERT_NE(nullptr, skiaSrgb);
    Color color = Color(0.1, 0.2, 0.3, 0.4);
    ColorSpace srgb = ColorSpace(skiaSrgb);
    Color result = color.Convert(srgb);
    ASSERT_EQ(color.ColorEqual(result), true);

    sk_sp<SkColorSpace> skiaP3 = SkColorSpace::MakeRGB(SkNamedTransferFn::kSRGB, SkNamedGamut::kDCIP3);
    ASSERT_NE(nullptr, skiaP3);
    ColorSpace displayP3 = ColorSpace(skiaP3);
    result = color.Convert(displayP3);
    Color p3Color = Color(0.1238f, 0.19752f, 0.29182f, 0.4, ColorSpaceName::DISPLAY_P3);
    ASSERT_EQ(p3Color.ColorEqual(result), true);
}

/**
 * @tc.name: CreateColorSpace001
 * @tc.desc: Create colorspace with customized parameters
 * @tc.type: FUNC
 * @tc.require: issueI6PXHH
 */
HWTEST_F(ColorManagerTest, CreateColorSpace001, TestSize.Level1)
{
    // customized parameters
    ColorSpacePrimaries primaries{0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f};
    TransferFunc transFunc{0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f};
    Matrix3x3 toXYZ = {{
        {0.1f, 0.2f, 0.3f},
        {0.1f, 0.2f, 0.3f},
        {0.1f, 0.2f, 0.3f}}};
    std::array<float, DIMES_2> whitePoint{0.1f, 0.2f};
    float gamma = 0.1;
    
    std::shared_ptr<ColorSpace> cs = std::make_shared<ColorSpace>(primaries, transFunc);
    ASSERT_NE(cs, nullptr);
    cs = std::make_shared<ColorSpace>(primaries, gamma);
    ASSERT_NE(cs, nullptr);
    cs = std::make_shared<ColorSpace>(toXYZ, whitePoint, transFunc);
    ASSERT_NE(cs, nullptr);
    cs = std::make_shared<ColorSpace>(toXYZ, whitePoint, gamma);
    ASSERT_NE(cs, nullptr);
}

/**
 * @tc.name: CreateColorSpace002
 * @tc.desc: Create colorspace with skcms_ICCProfile
 * @tc.type: FUNC
 * @tc.require: issueI6PXHH
 */
HWTEST_F(ColorManagerTest, CreateColorSpace002, TestSize.Level1)
{
    std::shared_ptr<skcms_ICCProfile> profile = std::make_shared<skcms_ICCProfile>();
    sk_sp<SkColorSpace> skiaSrgb = SkColorSpace::MakeSRGB();
    skiaSrgb->toProfile(profile.get());
    
    std::shared_ptr<ColorSpace> cs = std::make_shared<ColorSpace>(*profile);
    ASSERT_NE(cs, nullptr);
}

/**
 * @tc.name: CreateColorSpace003
 * @tc.desc: Create colorspace with invalid params
 * @tc.type: FUNC
 * @tc.require: issueI6QHNP
 */
HWTEST_F(ColorManagerTest, CreateColorSpace003, TestSize.Level1)
{
    // cannot create customized colorspace with name
    std::shared_ptr<ColorSpace> cs = std::make_shared<ColorSpace>(ColorSpaceName::CUSTOM);
    ASSERT_EQ(cs->GetColorSpaceName(), ColorSpaceName::NONE);

    cs = std::make_shared<ColorSpace>(nullptr, ColorSpaceName::NONE);
    ASSERT_EQ(cs->GetColorSpaceName(), ColorSpaceName::NONE);

    std::shared_ptr<skcms_ICCProfile> profile = std::make_shared<skcms_ICCProfile>();
    profile->has_toXYZD50 = false;
    cs = std::make_shared<ColorSpace>(*profile, ColorSpaceName::NONE);
    ASSERT_EQ(cs->GetColorSpaceName(), ColorSpaceName::NONE);
}

/**
 * @tc.name: VectorOperator
 * @tc.desc: Verify vector calculation
 * @tc.type: FUNC
 * @tc.require: issueI6QHNP
 */
HWTEST_F(ColorManagerTest, VectorOperator, TestSize.Level1)
{
    const Vector3 v1 = {2.0f, 1.0f, 0.1f};
    const Matrix3x3 m1 = {{
        {1.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 1.0f}}};
    
    const Vector3 v2 = v1 * m1;
    ASSERT_EQ(v2, v1);
}
}
}