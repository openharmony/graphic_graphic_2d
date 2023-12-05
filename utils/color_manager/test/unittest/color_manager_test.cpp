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
constexpr uint32_t MATRIX_SIZE = 3;

static void CheckColorSpaceEqual(const ColorSpace& cs0, const ColorSpace& cs1)
{
    ASSERT_TRUE(FloatEqual(cs0.transferFunc.g, cs1.transferFunc.g));
    ASSERT_TRUE(FloatEqual(cs0.transferFunc.a, cs1.transferFunc.a));
    ASSERT_TRUE(FloatEqual(cs0.transferFunc.b, cs1.transferFunc.b));
    ASSERT_TRUE(FloatEqual(cs0.transferFunc.c, cs1.transferFunc.c));
    ASSERT_TRUE(FloatEqual(cs0.transferFunc.d, cs1.transferFunc.d));
    ASSERT_TRUE(FloatEqual(cs0.transferFunc.e, cs1.transferFunc.e));
    ASSERT_TRUE(FloatEqual(cs0.transferFunc.f, cs1.transferFunc.f));

    for (uint32_t i = 0; i < MATRIX_SIZE; ++i) {
        for (uint32_t j = 0; j < MATRIX_SIZE; ++j) {
            ASSERT_TRUE(FloatEqual(cs0.toXYZ[i][j], cs1.toXYZ[i][j]));
        }
    }
}

static void CheckSkColorSpaceEqual(sk_sp<SkColorSpace> cs0, sk_sp<SkColorSpace> cs1)
{
    skcms_TransferFunction f0;
    cs0->transferFn(&f0);
    skcms_TransferFunction f1;
    cs1->transferFn(&f1);
    ASSERT_TRUE(FloatEqual(f0.g, f1.g));
    ASSERT_TRUE(FloatEqual(f0.a, f1.a));
    ASSERT_TRUE(FloatEqual(f0.b, f1.b));
    ASSERT_TRUE(FloatEqual(f0.c, f1.c));
    ASSERT_TRUE(FloatEqual(f0.d, f1.d));
    ASSERT_TRUE(FloatEqual(f0.e, f1.e));
    ASSERT_TRUE(FloatEqual(f0.f, f1.f));

    skcms_Matrix3x3 m0;
    cs0->toXYZD50(&m0);
    skcms_Matrix3x3 m1;
    cs1->toXYZD50(&m1);
    for (uint32_t i = 0; i < MATRIX_SIZE; ++i) {
        for (uint32_t j = 0; j < MATRIX_SIZE; ++j) {
            ASSERT_TRUE(FloatEqual(m0.vals[i][j], m1.vals[i][j]));
        }
    }
}

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
* CaseDescription: sRGB convert to BT709
*/
HWTEST_F(ColorManagerTest, sRGBToBT709, Function | SmallTest | Level2)
{
    Color color = Color(0.2, 0.4, 0.6, 1.0, SRGB);
    Color result = color.Convert(BT709);
    ASSERT_EQ(FloatEqual(result.r, 0.13811f), true);
    ASSERT_EQ(FloatEqual(result.g, 0.34414f), true);
    ASSERT_EQ(FloatEqual(result.b, 0.55779f), true);
}

/*
* Function: ColorManagerTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: BT709 convert to sRGB
*/
HWTEST_F(ColorManagerTest, BT709TosRGB, Function | SmallTest | Level2)
{
    Color color = Color(0.2, 0.4, 0.6, 1.0, BT709);
    Color result = color.Convert(SRGB);
    ASSERT_EQ(FloatEqual(result.r, 0.26108f), true);
    ASSERT_EQ(FloatEqual(result.g, 0.45287f), true);
    ASSERT_EQ(FloatEqual(result.b, 0.63889f), true);
}

/*
* Function: ColorManagerTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: sRGB convert to BT601_EBU
*/
HWTEST_F(ColorManagerTest, sRGBToBT601_EBU, Function | SmallTest | Level2)
{
    Color color = Color(0.2, 0.4, 0.6, 1.0, SRGB);
    Color result = color.Convert(BT601_EBU);
    ASSERT_EQ(FloatEqual(result.r, 0.15122f), true);
    ASSERT_EQ(FloatEqual(result.g, 0.34414f), true);
    ASSERT_EQ(FloatEqual(result.b, 0.55984f), true);
}

/*
* Function: ColorManagerTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: BT601_EBU convert to sRGB
*/
HWTEST_F(ColorManagerTest, BT601_EBUTosRGB, Function | SmallTest | Level2)
{
    Color color = Color(0.2, 0.4, 0.6, 1.0, BT601_EBU);
    Color result = color.Convert(SRGB);
    ASSERT_EQ(FloatEqual(result.r, 0.24843f), true);
    ASSERT_EQ(FloatEqual(result.g, 0.45287f), true);
    ASSERT_EQ(FloatEqual(result.b, 0.63709f), true);
}

/*
* Function: ColorManagerTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: sRGB convert to BT601_SMPTE_C
*/
HWTEST_F(ColorManagerTest, sRGBToBT601_SMPTE_C, Function | SmallTest | Level2)
{
    Color color = Color(0.2, 0.4, 0.6, 1.0, SRGB);
    Color result = color.Convert(BT601_SMPTE_C);
    ASSERT_EQ(FloatEqual(result.r, 0.10894f), true);
    ASSERT_EQ(FloatEqual(result.g, 0.34241f), true);
    ASSERT_EQ(FloatEqual(result.b, 0.55660f), true);
}

/*
* Function: ColorManagerTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: BT601_SMPTE_C convert to sRGB
*/
HWTEST_F(ColorManagerTest, BT601_SMPTE_CTosRGB, Function | SmallTest | Level2)
{
    Color color = Color(0.2, 0.4, 0.6, 1.0, BT601_SMPTE_C);
    Color result = color.Convert(SRGB);
    ASSERT_EQ(FloatEqual(result.r, 0.28173f), true);
    ASSERT_EQ(FloatEqual(result.g, 0.45419f), true);
    ASSERT_EQ(FloatEqual(result.b, 0.63995f), true);
}

/*
* Function: ColorManagerTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: sRGB convert to Linear_P3
*/
HWTEST_F(ColorManagerTest, sRGBToLinear_P3, Function | SmallTest | Level2)
{
    Color color = Color(0.2, 0.4, 0.6, 1.0, SRGB);
    Color result = color.Convert(LINEAR_P3);
    ASSERT_EQ(FloatEqual(result.r, 0.05081606f), true);
    ASSERT_EQ(FloatEqual(result.g, 0.1295567f), true);
    ASSERT_EQ(FloatEqual(result.b, 0.30022378f), true);
}

/*
* Function: ColorManagerTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Linear_P3 convert to sRGB
*/
HWTEST_F(ColorManagerTest, Linear_P3TosRGB, Function | SmallTest | Level2)
{
    Color color = Color(0.2, 0.4, 0.6, 1.0, LINEAR_P3);
    Color result = color.Convert(SRGB);
    ASSERT_EQ(FloatEqual(result.r, 0.43018f), true);
    ASSERT_EQ(FloatEqual(result.g, 0.67146f), true);
    ASSERT_EQ(FloatEqual(result.b, 0.81155f), true);
}

/*
* Function: ColorManagerTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: sRGB convert to Linear_sRGB
*/
HWTEST_F(ColorManagerTest, sRGBTosLinear_sRGB, Function | SmallTest | Level2)
{
    Color color = Color(0.2, 0.4, 0.6, 1.0, SRGB);
    Color result = color.Convert(LINEAR_SRGB);
    ASSERT_EQ(FloatEqual(result.r, 0.0331f), true);
    ASSERT_EQ(FloatEqual(result.g, 0.13287f), true);
    ASSERT_EQ(FloatEqual(result.b, 0.31855f), true);
}

/*
* Function: ColorManagerTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: sRGB_Linear convert to sRGB
*/
HWTEST_F(ColorManagerTest, Linear_sRGBTosRGB, Function | SmallTest | Level2)
{
    Color color = Color(0.2, 0.4, 0.6, 1.0, LINEAR_SRGB);
    Color result = color.Convert(SRGB);
    ASSERT_EQ(FloatEqual(result.r, 0.48453f), true);
    ASSERT_EQ(FloatEqual(result.g, 0.66519f), true);
    ASSERT_EQ(FloatEqual(result.b, 0.79774f), true);
}

/*
* Function: ColorManagerTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: sRGB convert to Linear_BT2020
*/
HWTEST_F(ColorManagerTest, sRGBToLinear_BT2020, Function | SmallTest | Level2)
{
    Color color = Color(0.2, 0.4, 0.6, 1.0, SRGB);
    Color result = color.Convert(LINEAR_BT2020);
    ASSERT_EQ(FloatEqual(result.r, 0.07831869f), true);
    ASSERT_EQ(FloatEqual(result.g, 0.12808435f), true);
    ASSERT_EQ(FloatEqual(result.b, 0.29752541f), true);
}

/*
* Function: ColorManagerTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Linear_BT2020 convert to sRGB
*/
HWTEST_F(ColorManagerTest, Linear_BT2020TosRGB, Function | SmallTest | Level2)
{
    Color color = Color(0.2, 0.4, 0.6, 1.0, LINEAR_BT2020);
    Color result = color.Convert(SRGB);
    ASSERT_EQ(FloatEqual(result.r, 0.25638f), true);
    ASSERT_EQ(FloatEqual(result.g, 0.68242f), true);
    ASSERT_EQ(FloatEqual(result.b, 0.81379f), true);
}

/*
* Function: ColorManagerTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: sRGB convert to BT2020_HLG
*/
HWTEST_F(ColorManagerTest, sRGBToBT2020_HLG, Function | SmallTest | Level2)
{
    Color color = Color(0.2, 0.4, 0.6, 1.0, SRGB);
    Color result = color.Convert(BT2020_HLG);
    ASSERT_EQ(FloatEqual(result.r, 0.48441279f), true);
    ASSERT_EQ(FloatEqual(result.g, 0.59994509f), true);
    ASSERT_EQ(FloatEqual(result.b, 0.77255629f), true);
}

/*
* Function: ColorManagerTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: BT2020_HLG convert to sRGB
*/
HWTEST_F(ColorManagerTest, BT2020_HLGTosRGB, Function | SmallTest | Level2)
{
    Color color = Color(0.2, 0.4, 0.6, 1.0, BT2020_HLG);
    Color result = color.Convert(SRGB);
    ASSERT_EQ(FloatEqual(result.r, 0.0f), true);
    ASSERT_EQ(FloatEqual(result.g, 0.26632f), true);
    ASSERT_EQ(FloatEqual(result.b, 0.40685f), true);
}

/*
* Function: ColorManagerTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: sRGB convert to BT2020_PQ
*/
HWTEST_F(ColorManagerTest, sRGBToBT2020_PQ, Function | SmallTest | Level2)
{
    Color color = Color(0.2, 0.4, 0.6, 1.0, SRGB);
    Color result = color.Convert(BT2020_PQ);
    ASSERT_EQ(FloatEqual(result.r, 0.72498339f), true);
    ASSERT_EQ(FloatEqual(result.g, 0.77864546f), true);
    ASSERT_EQ(FloatEqual(result.b, 0.87049267f), true);
}

/*
* Function: ColorManagerTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: BT2020_PQ convert to sRGB
*/
HWTEST_F(ColorManagerTest, BT2020_PQTosRGB, Function | SmallTest | Level2)
{
    Color color = Color(0.2, 0.4, 0.6, 1.0, BT2020_PQ);
    Color result = color.Convert(SRGB);
    ASSERT_EQ(FloatEqual(result.r, 0.0f), true);
    ASSERT_EQ(FloatEqual(result.g, 0.0443f), true);
    ASSERT_EQ(FloatEqual(result.b, 0.17906f), true);
}

/*
* Function: ColorManagerTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: sRGB convert to P3_HLG
*/
HWTEST_F(ColorManagerTest, sRGBToP3_HLG, Function | SmallTest | Level2)
{
    Color color = Color(0.2, 0.4, 0.6, 1.0, SRGB);
    Color result = color.Convert(P3_HLG);
    ASSERT_EQ(FloatEqual(result.r, 0.39044612f), true);
    ASSERT_EQ(FloatEqual(result.g, 0.6026564f), true);
    ASSERT_EQ(FloatEqual(result.b, 0.77439688f), true);
}

/*
* Function: ColorManagerTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: P3_HLG convert to sRGB
*/
HWTEST_F(ColorManagerTest, P3_HLGTosRGB, Function | SmallTest | Level2)
{
    Color color = Color(0.2, 0.4, 0.6, 1.0, P3_HLG);
    Color result = color.Convert(SRGB);
    ASSERT_EQ(FloatEqual(result.r, 0.05432f), true);
    ASSERT_EQ(FloatEqual(result.g, 0.2601f), true);
    ASSERT_EQ(FloatEqual(result.b, 0.40461f), true);
}

/*
* Function: ColorManagerTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: sRGB convert to P3_PQ
*/
HWTEST_F(ColorManagerTest, sRGBToP3_PQ, Function | SmallTest | Level2)
{
    Color color = Color(0.2, 0.4, 0.6, 1.0, SRGB);
    Color result = color.Convert(P3_PQ);
    ASSERT_EQ(FloatEqual(result.r, 0.67822741f), true);
    ASSERT_EQ(FloatEqual(result.g, 0.77999537f), true);
    ASSERT_EQ(FloatEqual(result.b, 0.87151975f), true);
}

/*
* Function: ColorManagerTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: P3_PQ convert to sRGB
*/
HWTEST_F(ColorManagerTest, P3_PQTosRGB, Function | SmallTest | Level2)
{
    Color color = Color(0.2, 0.4, 0.6, 1.0, P3_PQ);
    Color result = color.Convert(SRGB);
    ASSERT_EQ(FloatEqual(result.r, 0.0f), true);
    ASSERT_EQ(FloatEqual(result.g, 0.04344f), true);
    ASSERT_EQ(FloatEqual(result.b, 0.17756f), true);
}

/*
* Function: ColorManagerTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: SkColorSpace to ColorSpace
*/
HWTEST_F(ColorManagerTest, SkColorSpaceToColorSpace, Function | SmallTest | Level2)
{
    sk_sp<SkColorSpace> skiaSrgb = SkColorSpace::MakeSRGB();
    ASSERT_NE(nullptr, skiaSrgb);
    Color color = Color(0.1, 0.2, 0.3, 0.4);
    ColorSpace srgb = ColorSpace(skiaSrgb);
    Color result = color.Convert(srgb);
    ASSERT_EQ(color.ColorEqual(result), true);
#if defined(NEW_SKIA)
    sk_sp<SkColorSpace> skiaP3 = SkColorSpace::MakeRGB(SkNamedTransferFn::kSRGB, SkNamedGamut::kDisplayP3);
#else
    sk_sp<SkColorSpace> skiaP3 = SkColorSpace::MakeRGB(SkNamedTransferFn::kSRGB, SkNamedGamut::kDCIP3);
#endif
    ASSERT_NE(nullptr, skiaP3);
    ColorSpace displayP3 = ColorSpace(skiaP3);
    result = color.Convert(displayP3);
    Color p3Color = Color(0.1238f, 0.19752f, 0.29182f, 0.4, ColorSpaceName::DISPLAY_P3);
    ASSERT_EQ(p3Color.ColorEqual(result), true);

    ColorSpace csSrgb(SkColorSpace::MakeRGB(SkNamedTransferFn::kSRGB, SkNamedGamut::kSRGB));
    CheckColorSpaceEqual(csSrgb, ColorSpace(SRGB));

    ColorSpace csAdobeRgb(SkColorSpace::MakeRGB(SkNamedTransferFn::k2Dot2, SkNamedGamut::kAdobeRGB));
    CheckColorSpaceEqual(csAdobeRgb, ColorSpace(ADOBE_RGB));

    ColorSpace csDisplayP3(SkColorSpace::MakeRGB(SkNamedTransferFn::kSRGB, SkNamedGamut::kDisplayP3));
    CheckColorSpaceEqual(csDisplayP3, ColorSpace(DISPLAY_P3));

    ColorSpace csBt2020(SkColorSpace::MakeRGB(SkNamedTransferFn::kRec2020, SkNamedGamut::kRec2020));
    CheckColorSpaceEqual(csBt2020, ColorSpace(BT2020));

    ColorSpace csBt2020Hlg(SkColorSpace::MakeRGB(SkNamedTransferFn::kHLG, SkNamedGamut::kRec2020));
    CheckColorSpaceEqual(csBt2020Hlg, ColorSpace(BT2020_HLG));

    ColorSpace csBt2020Pq(SkColorSpace::MakeRGB(SkNamedTransferFn::kPQ, SkNamedGamut::kRec2020));
    CheckColorSpaceEqual(csBt2020Pq, ColorSpace(BT2020_PQ));

    ColorSpace csLinearBt2020(SkColorSpace::MakeRGB(SkNamedTransferFn::kLinear, SkNamedGamut::kRec2020));
    CheckColorSpaceEqual(csLinearBt2020, ColorSpace(LINEAR_BT2020));
}

/*
* Function: ColorManagerTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: ColorSpace to SkColorSpace
*/
HWTEST_F(ColorManagerTest, ColorSpaceToSkColorSpace, Function | SmallTest | Level2)
{
    auto skSrgb = ColorSpace(SRGB).ToSkColorSpace();
    CheckSkColorSpaceEqual(skSrgb, SkColorSpace::MakeRGB(SkNamedTransferFn::kSRGB, SkNamedGamut::kSRGB));

    auto skAdobeRgb = ColorSpace(ADOBE_RGB).ToSkColorSpace();
    CheckSkColorSpaceEqual(skAdobeRgb, SkColorSpace::MakeRGB(SkNamedTransferFn::k2Dot2, SkNamedGamut::kAdobeRGB));

    auto skDisplayP3 = ColorSpace(DISPLAY_P3).ToSkColorSpace();
    CheckSkColorSpaceEqual(skDisplayP3, SkColorSpace::MakeRGB(SkNamedTransferFn::kSRGB, SkNamedGamut::kDisplayP3));

    auto skBt2020 = ColorSpace(BT2020).ToSkColorSpace();
    CheckSkColorSpaceEqual(skBt2020, SkColorSpace::MakeRGB(SkNamedTransferFn::kRec2020, SkNamedGamut::kRec2020));

    auto skBt2020Hlg = ColorSpace(BT2020_HLG).ToSkColorSpace();
    CheckSkColorSpaceEqual(skBt2020Hlg, SkColorSpace::MakeRGB(SkNamedTransferFn::kHLG, SkNamedGamut::kRec2020));

    auto skBt2020Pq = ColorSpace(BT2020_PQ).ToSkColorSpace();
    CheckSkColorSpaceEqual(skBt2020Pq, SkColorSpace::MakeRGB(SkNamedTransferFn::kPQ, SkNamedGamut::kRec2020));

    auto skLinearBt2020 = ColorSpace(LINEAR_BT2020).ToSkColorSpace();
    CheckSkColorSpaceEqual(skLinearBt2020, SkColorSpace::MakeRGB(SkNamedTransferFn::kLinear, SkNamedGamut::kRec2020));
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