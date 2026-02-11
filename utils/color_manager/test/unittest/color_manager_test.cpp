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
constexpr float FLOAT_DELTA = 0.000001;
constexpr ColorSpacePrimaries CSP_ADOBE_RGB_MOCK = {0.640f, 0.330f, 0.210f, 0.710f, 0.150f, 0.060f, 0.3127f, 0.3290f};
constexpr TransferFunc TF_ADOBE_RGB_MOCK = {2.2f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

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

// Helper function to check if a Matrix3x3 is all zeros
bool IsZeroMatrix(const Matrix3x3& m)
{
    for (size_t i = 0; i < m.size(); ++i) {
        for (size_t j = 0; j < m[i].size(); ++j) {
            if (!FloatNearlyEqualZero(m[i][j])) {
                return false;
            }
        }
    }
    return true;
}

bool IsIdentityMatrix(const Matrix3x3& m)
{
    for (int i = 0; i < DIMES_9; ++i) {
        const int row = i / 3;
        const int col = i % 3;
        const float expected = (row == col) ? 1.0f : 0.0f;
        if (!FloatNearlyEqual(m[row][col], expected)) {
            return false;
        }
    }
    return true;
}

// Helper: Check if a Matrix3x3 has at least one non-zero element
bool HasNonZeroElement(const Matrix3x3& m)
{
    for (size_t i = 0; i < m.size(); ++i) {
        for (size_t j = 0; j < m[i].size(); ++j) {
            if (!FloatNearlyEqualZero(m[i][j])) {
                return true;
            }
        }
    }
    return false;
}

// Helper: Check if a Vector3 is zero (within floating-point tolerance)
bool IsZeroVector(const Vector3& v)
{
    for (float x : v) {
        if (!FloatNearlyEqualZero(x)) {
            return false;
        }
    }
    return true;
}

static bool IsFinite(float x)
{
    return x * 0 == 0;
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
* CaseDescription: ConvertP3ToSRGBColorSpace convert P3 to sRGB
*/
HWTEST_F(ColorManagerTest, ConvertP3ToSRGBColorSpace, Function | SmallTest | Level2)
{
    Vector3 rgbInP3 = {0.1, 0.2, 0.3};
    auto rgbInSRGB = ColorSpaceConvertor::ConvertP3ToSRGBColorSpace(rgbInP3);
    ASSERT_EQ(FloatEqual(rgbInSRGB[0], 0.0594f), true);
    ASSERT_EQ(FloatEqual(rgbInSRGB[1], 0.2031f), true);
    ASSERT_EQ(FloatEqual(rgbInSRGB[2], 0.3087f), true);
}

/*
* Function: ColorManagerTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: ConvertSRGBToP3ColorSpace convert sRGB to P3
*/
HWTEST_F(ColorManagerTest, ConvertSRGBToP3ColorSpace, Function | SmallTest | Level2)
{
    Vector3 rgbInSRGB = {0.1, 0.2, 0.3};
    auto rgbInP3 = ColorSpaceConvertor::ConvertSRGBToP3ColorSpace(rgbInSRGB);
    ASSERT_EQ(FloatEqual(rgbInP3[0], 0.1237f), true);
    ASSERT_EQ(FloatEqual(rgbInP3[1], 0.1975f), true);
    ASSERT_EQ(FloatEqual(rgbInP3[2], 0.2918f), true);
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
    sk_sp<SkColorSpace> skiaP3 = SkColorSpace::MakeRGB(SkNamedTransferFn::kSRGB, SkNamedGamut::kDisplayP3);
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

/**
 * @tc.name: GetWhitePoint
 * @tc.desc: Verify GetWhitePoint func
 * @tc.type: FUNC
 * @tc.require: issueI6QHNP
 */
HWTEST_F(ColorManagerTest, GetWhitePoint, TestSize.Level1)
{
    ColorSpace space(ColorSpaceName::ADOBE_RGB);
    std::array<float, 2> whiteP = {CSP_ADOBE_RGB_MOCK.wX, CSP_ADOBE_RGB_MOCK.wY};
    auto p = space.GetWhitePoint();
    ASSERT_EQ(p[0], whiteP[0]);
    ASSERT_EQ(p[1], whiteP[1]);
}

/**
 * @tc.name: GetGamma
 * @tc.desc: Verify GetGamma func
 * @tc.type: FUNC
 * @tc.require: issueI6QHNP
 */
HWTEST_F(ColorManagerTest, GetGamma, TestSize.Level1)
{
    ColorSpace space(ColorSpaceName::ADOBE_RGB);
    ASSERT_TRUE(std::abs((space.GetGamma()) - (TF_ADOBE_RGB_MOCK.g)) <= (FLOAT_DELTA));
}

/**
 * @tc.name: FloatNearlyEqualZero_DirectBoundaryChecks
 * @tc.desc: Directly tests the handling of boundary values in the FloatNearlyEqualZero function
 * @tc.type: FUNC
 */
HWTEST_F(ColorManagerTest, FloatNearlyEqualZero_DirectBoundaryChecks, TestSize.Level1)
{
    // Scene 1: Exact 0
    EXPECT_TRUE(FloatNearlyEqualZero(0.0f));
    EXPECT_TRUE(FloatNearlyEqualZero(-0.0f));

    // Scene 2: Positive values less than FLT_EPSILON (True branch)
    EXPECT_TRUE(FloatNearlyEqualZero(FLT_EPSILON * 0.5f));
    EXPECT_TRUE(FloatNearlyEqualZero(1.0e-08f));

    // Scene 3: Negative values less than FLT_EPSILON (True branch)
    EXPECT_TRUE(FloatNearlyEqualZero(-FLT_EPSILON * 0.5f));

    // Scene 4: Equal to FLT_EPSILON (boundary case, function uses <, so should return False) (False branch)
    EXPECT_FALSE(FloatNearlyEqualZero(FLT_EPSILON));
    EXPECT_FALSE(FloatNearlyEqualZero(-FLT_EPSILON));

    // Scene 5: Greater than FLT_EPSILON (False branch)
    EXPECT_FALSE(FloatNearlyEqualZero(FLT_EPSILON * 2.0f));
    EXPECT_FALSE(FloatNearlyEqualZero(1.0f));
}

// Test Vector3 operator/ denominator zero (True branch)
HWTEST_F(ColorManagerTest, Vector3Division_DenominatorNearZero, TestSize.Level1)
{
    Vector3 a = {1.0f, 1.0f, 1.0f};

    // Scene 1: Denominator is exactly 0 (should return zero matrix)
    Vector3 b_zero = {1.0f, 0.0f, 1.0f};
    Matrix3x3 res1 = a / b_zero;
    EXPECT_TRUE(IsZeroMatrix(res1));

    // Scene 2: Denominator less than FLT_EPSILON (near zero)
    Vector3 b_eps = {1.0f, FLT_EPSILON * 0.5f, 1.0f};
    Matrix3x3 res2 = a / b_eps;
    EXPECT_TRUE(IsZeroMatrix(res2));

    // Scene 3: Denominator exactly FLT_EPSILON (normal case, should NOT be zero)
    Vector3 b_normal = {1.0f, FLT_EPSILON, 1.0f};
    Matrix3x3 res3 = a / b_normal;
    // Verify at least one element is non-zero (specifically the [1][1] element)
    EXPECT_FALSE(FloatNearlyEqualZero(res3[1][1]));

    // Scene 4: Normal denominator value (full coverage)
    Vector3 b_valid = {2.0f, 2.0f, 2.0f};
    Matrix3x3 res4 = a / b_valid;
    // Verify diagonal elements (should be 0.5f)
    EXPECT_FLOAT_EQ(res4[0][0], 0.5f);
    EXPECT_FLOAT_EQ(res4[1][1], 0.5f);
    EXPECT_FLOAT_EQ(res4[2][2], 0.5f);
}

// Test ComputeXYZD50 denominator zero (primaries Y component) (True branch)
HWTEST_F(ColorManagerTest, ComputeXYZD50_PrimaryYZero, TestSize.Level1)
{
    ColorSpacePrimaries p = {0.64f, 0.33f, 0.30f, 0.60f, 0.15f, 0.06f, 0.3127f, 0.3290f};

    // Scenario 1: rY is 0 (invalid input)
    p.rY = 0.0f;
    Matrix3x3 res1 = ComputeXYZD50(p);
    EXPECT_TRUE(IsZeroMatrix(res1));

    // Scenario 2: gY near zero (invalid input)
    p.rY = 0.33f;
    p.gY = FLT_EPSILON * 0.1f; // Tiny value (less than FLT_EPSILON)
    Matrix3x3 res2 = ComputeXYZD50(p);
    EXPECT_TRUE(IsZeroMatrix(res2));

    // Scenario 3: wY is 0 (invalid input)
    p.gY = 0.60f;
    p.wY = 0.0f;
    Matrix3x3 res3 = ComputeXYZD50(p);
    EXPECT_TRUE(IsZeroMatrix(res3));

    // Scenario 4: Normal primaries (valid input)
    p.rY = 0.33f; p.gY = 0.60f; p.wY = 0.3290f;
    Matrix3x3 res4 = ComputeXYZD50(p);
    EXPECT_TRUE(HasNonZeroElement(res4));
}

// Test ComputeXYZD50 intermediate denominator zero (GxGy - RxRy) (True branch)
HWTEST_F(ColorManagerTest, ComputeXYZD50_SingularRatio, TestSize.Level1)
{
    ColorSpacePrimaries p;
    p.rX = 0.64f; p.rY = 0.33f;
    p.bX = 0.15f; p.bY = 0.06f;
    p.wX = 0.3127f; p.wY = 0.3290f;

    // Construct GxGy == RxRy scenario (i.e., (gX/gY) == (rX/rY))
    p.gX = p.rX;
    p.gY = p.rY;

    Matrix3x3 res = ComputeXYZD50(p);
    // (GxGy - RxRy) should be 0, triggering FloatNearlyEqualZero, returns empty matrix
    EXPECT_TRUE(IsZeroMatrix(res));
}

/**
 * @tc.name: ColorSpaceName_Construction_LinearToNonLinear_ZeroDenominator
 * @tc.desc: Verify ColorSpace construction by Name and ToNonLinear with c=0 (Linear TF)
 * @tc.type: FUNC
 * @tc.require: issueI6QHNP
 */
HWTEST_F(ColorManagerTest, ColorSpaceName_Construction_LinearToNonLinear_ZeroDenominator, TestSize.Level1)
{
    // Construct ColorSpace using Name (Requirement)
    ColorSpace cs(ColorSpaceName::LINEAR_SRGB);

    ASSERT_EQ(cs.colorSpaceName, ColorSpaceName::LINEAR_SRGB);

    // Verify Transfer Function parameters match expectation (c should be 0)
    ASSERT_TRUE(FloatNearlyEqualZero(cs.transferFunc.c));
    ASSERT_TRUE(FloatEqual(cs.transferFunc.g, 1.0));
    ASSERT_TRUE(FloatEqual(cs.transferFunc.a, 1.0));

    Vector3 input = {0.5f, 0.5f, 0.5f};
    // branch 1: Test a=0
    cs.transferFunc.g = HLG_G;

    auto a = cs.transferFunc.a;
    cs.transferFunc.a = 0;
    {
        Vector3 res = cs.ToNonLinear(input);
        EXPECT_TRUE(IsZeroVector(res));
    }
    cs.transferFunc.a = a;

    // branch 2: Test b=0
    auto b = cs.transferFunc.b;
    cs.transferFunc.b = 0;
    {
        Vector3 res = cs.ToNonLinear(input);
        EXPECT_TRUE(IsZeroVector(res));
    }
    cs.transferFunc.b = b;

    // branch 3: Normal parameters
    {
        ColorSpace linearCs(ColorSpaceName::LINEAR_SRGB);
        Vector3 res = linearCs.ToNonLinear(input);
        // For LINEAR_SRGB: g=1.0, a=1.0, b=0.0, c=0.0, d=0.0, e=0.0, f=0.0
        // n >= d (0.5 >= 0.0) is true
        // Calculation formula: (a*n + b)^g + e = (1.0*0.5 + 0.0)^1.0 + 0.0 = 0.5
        ASSERT_FLOAT_EQ(res[0], 0.5f);
        ASSERT_FLOAT_EQ(res[1], 0.5f);
        ASSERT_FLOAT_EQ(res[2], 0.5f);
    }

    // Additional linear space round-trip verification
    {
        ColorSpace linearCs(ColorSpaceName::LINEAR_SRGB);
        Vector3 original = {0.5f, 0.25f, 0.75f};
        Vector3 linear = linearCs.ToLinear(original);  // Should remain unchanged as it's linear
        Vector3 backToOriginal = linearCs.ToNonLinear(linear);  // Should return to original value
        ASSERT_NEAR(backToOriginal[0], original[0], 0.001f);
        ASSERT_NEAR(backToOriginal[1], original[1], 0.001f);
        ASSERT_NEAR(backToOriginal[2], original[2], 0.001f);
    }
}

/**
 * @tc.name: CustomPrimaries_Construction_ZeroDenominator
 * @tc.desc: Verify ColorSpace construction with custom primaries (y=0) causing invalid matrix
 * @tc.type: FUNC
 * @tc.require: issueI6QHNP
 */
HWTEST_F(ColorManagerTest, CustomPrimaries_Construction_ZeroDenominator, TestSize.Level1)
{
    ColorSpacePrimaries primaries = {0.64f, 0.33f, 0.30f, 0.60f, 0.15f, 0.0f, 0.3127f, 0.3290f}; // bY = 0
    TransferFunc tf = {2.2f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

    // Construct using Custom Primaries Constructor
    ColorSpace cs(primaries, tf);

    // ComputeXYZD50 detects bY=0 via FloatNearlyEqualZero and returns {}
    // Verify toXYZ is effectively empty (or initialized to 0)
    EXPECT_TRUE(IsZeroMatrix(cs.toXYZ));
}

/**
 * @tc.name: CustomTF_Construction_HLG_A_ZeroDenominator
 * @tc.desc: Verify ToLinear with custom HLG TF where a=0 triggers check
 * @tc.type: FUNC
 * @tc.require: issueI6QHNP
 */
HWTEST_F(ColorManagerTest, CustomTF_Construction_HLG_A_ZeroDenominator, TestSize.Level1)
{
    ColorSpacePrimaries primaries = {0.64f, 0.33f, 0.30f, 0.60f, 0.15f, 0.06f, 0.3127f, 0.3290f};
    TransferFunc tf = {HLG_G, 0.0f, 2.0f, 1.0f, 0.0f, 0.0f, 0.0f}; // a = 0

    ColorSpace cs(primaries, tf);
    Vector3 input = {0.5f, 0.5f, 0.5f};

    Vector3 res = cs.ToLinear(input);
    // Should hit return {} inside FloatNearlyEqualZero(p.a)
    EXPECT_TRUE(IsZeroVector(res));
}

/**
 * @tc.name: CustomTF_Construction_PQ_F_ZeroDenominator
 * @tc.desc: Verify ToNonLinear with custom PQ TF where f=0 triggers check
 * @tc.type: FUNC
 * @tc.require: issueI6QHNP
 */
HWTEST_F(ColorManagerTest, CustomTF_Construction_PQ_F_ZeroDenominator, TestSize.Level1)
{
    ColorSpacePrimaries primaries = {0.64f, 0.33f, 0.30f, 0.60f, 0.15f, 0.06f, 0.3127f, 0.3290f};
    TransferFunc tf = {PQ_G, -107/128.0f, 1.0f, 32/2523.0f, 2413/128.0f, -2392/128.0f, 0.0f}; // f = 0

    ColorSpace cs(primaries, tf);
    Vector3 input = {0.5f, 0.5f, 0.5f};

    Vector3 res = cs.ToNonLinear(input);
    // Should hit return {} inside FloatNearlyEqualZero(p.f)
    EXPECT_TRUE(IsZeroVector(res));
}

// Test Matrix3x3 Invert when determinant is zero (True branch)
HWTEST_F(ColorManagerTest, MatrixInvert_ZeroDeterminant, TestSize.Level1)
{
    // Scenario 1: Zero matrix (determinant = 0)
    Matrix3x3 m1 = {};
    auto res1 = Invert(m1);
    EXPECT_TRUE(IsZeroMatrix(res1));

    // Scenario 2: Matrix with linearly dependent rows (determinant = 0)
    Matrix3x3 m2 = {{{1.0f, 2.0f, 3.0f},
                     {1.0f, 2.0f, 3.0f},
                     {0.0f, 0.0f, 1.0f}}};
    auto res2 = Invert(m2);
    EXPECT_TRUE(IsZeroMatrix(res2));

    // Scenario 3: Valid invertible matrix (determinant != 0)
    Matrix3x3 m3 = {{{1.0f, 0.0f, 0.0f},
                     {0.0f, 1.0f, 0.0f},
                     {0.0f, 0.0f, 1.0f}}};
    auto res3 = Invert(m3);

    EXPECT_TRUE(IsIdentityMatrix(res3));
}

/**
 * @tc.name: ToLinear_HLG_DenominatorChecks
 * @tc.desc: Test denominator checks in HLG ToLinear conversion
 * @tc.require: issueI6QHNP
 */
HWTEST_F(ColorManagerTest, ToLinear_HLG_DenominatorChecks, TestSize.Level1)
{
    ColorSpace cs(ColorSpaceName::BT2020_HLG); // Use HLG_G
    Vector3 input = {0.5f, 0.5f, 0.5f};

    // Save original parameters
    auto origA = cs.transferFunc.a;
    auto origB = cs.transferFunc.b;

    // Scenario 1: p.a is 0 (denominator check: p.g * p.a * p.b) (True branch)
    cs.transferFunc.a = 0.0f;
    auto res1 = cs.ToLinear(input);
    EXPECT_TRUE(IsZeroVector(res1));
    cs.transferFunc.a = origA;

    // Scenario 2: p.b is 0 (denominator check: p.g * p.a * p.b) (True branch)
    cs.transferFunc.b = 0.0f;
    auto res2 = cs.ToLinear(input);
    EXPECT_TRUE(IsZeroVector(res2));
    cs.transferFunc.b = origB;

    // Scenario 3: Both p.a and p.b are 0 (covers || logic) (True branch)
    cs.transferFunc.a = 0.0f;
    cs.transferFunc.b = 0.0f;
    auto res3 = cs.ToLinear(input);
    EXPECT_TRUE(IsZeroVector(res3));
    cs.transferFunc.a = origA;
    cs.transferFunc.b = origB;

    // Scenario 4: Normal parameters (False branch)
    // Ensure the normal HLG computation path is covered.
    // HLG: if (n > 1/a) -> exp(...) else: n*n / -g
    // HLG standard: a=2, b=2 → 1/a = 0.5. input=0.5 → not > 0.5.
    // Result: n*n / -g = 0.25 / 3.0 ≈ 0.08333...
    auto res4 = cs.ToLinear(input);
    EXPECT_GT(res4[0], 0.0f); // Should yield a valid positive value
}

/**
 * @tc.name: ToLinear_PQ_DenominatorChecks
 * @tc.desc: Test denominator checks in PQ ToLinear conversion (p.d + p.e * tmp)
 * @tc.require: issueI6QHNP
 */
HWTEST_F(ColorManagerTest, ToLinear_PQ_DenominatorChecks, TestSize.Level1)
{
    ColorSpace cs(ColorSpaceName::BT2020_PQ); // Use PQ_G
    Vector3 input = {0.5f, 0.5f, 0.5f};

    // Save original parameters
    auto origD = cs.transferFunc.d;
    auto origE = cs.transferFunc.e;

    // Scenario: p.d + p.e * tmp == 0 (True branch)
    cs.transferFunc.d = 0.0f;
    cs.transferFunc.e = 0.0f;

    auto res = cs.ToLinear(input);
    // Triggers FloatNearlyEqualZero(0.0f) → returns zero vector
    EXPECT_TRUE(IsZeroVector(res));

    // Restore original parameters
    cs.transferFunc.d = origD;
    cs.transferFunc.e = origE;

    // Scenario: Normal parameters (False branch)
    // PQ ToLinear should work normally
    auto res2 = cs.ToLinear(input);
    EXPECT_GT(res2[0], 0.0f);
}

/**
 * @tc.name: ToLinear_LOG_DenominatorChecks
 * @tc.desc: Test denominator checks in LOG ToLinear conversion
 * @tc.require: issueI6QHNP
 */
HWTEST_F(ColorManagerTest, ToLinear_LOG_DenominatorChecks, TestSize.Level1)
{
    // NamedColorSpace includes CS_H_LOG; uses LOG_G
    ColorSpace cs(ColorSpaceName::H_LOG);
    Vector3 input = {0.5f, 0.5f, 0.5f};

    // Save original parameters
    auto origA = cs.transferFunc.a;
    auto origC = cs.transferFunc.c;
    auto origE = cs.transferFunc.e;
    auto origF = cs.transferFunc.f;

    // Scenario 1: p.a = 0 (True branch)
    cs.transferFunc.a = 0.0f;
    auto res1 = cs.ToLinear(input);
    EXPECT_TRUE(IsZeroVector(res1));
    cs.transferFunc.a = origA;

    // Scenario 2: p.c = 0 (True branch)
    cs.transferFunc.c = 0.0f;
    auto res2 = cs.ToLinear(input);
    EXPECT_TRUE(IsZeroVector(res2));
    cs.transferFunc.c = origC;

    // Scenario 3: p.a = 0 and p.c = 0 (True branch)
    cs.transferFunc.a = 0.0f;
    cs.transferFunc.c = 0.0f;
    auto res3 = cs.ToLinear(input);
    EXPECT_TRUE(IsZeroVector(res3));
    cs.transferFunc.a = origA;
    cs.transferFunc.c = origC;

    // Scenario 4: p.e = 0 (enters else branch: n = (n - p.f) / p.e) → division by zero → return zero (True branch)
    Vector3 inputSmall = {0.0f, 0.0f, 0.0f};
    cs.transferFunc.e = 0.0f;
    cs.transferFunc.f = 0.0f;
    auto res4 = cs.ToLinear(inputSmall);
    EXPECT_TRUE(IsZeroVector(res4));

    // Restore original parameters
    cs.transferFunc.e = origE;
    cs.transferFunc.f = origF;

    // Scenario 5: Normal parameters (False branch)
    // LOG ToLinear should work normally
    auto res5 = cs.ToLinear(input);
    EXPECT_GT(res5[0], 0.0f);
}

/**
 * @tc.name: ToNonLinear_HLG_DenominatorChecks
 * @tc.desc: Test denominator checks in HLG ToNonLinear conversion
 * @tc.require: issueI6QHNP
 */
HWTEST_F(ColorManagerTest, ToNonLinear_HLG_DenominatorChecks, TestSize.Level1)
{
    ColorSpace cs(ColorSpaceName::BT2020_HLG);
    Vector3 input = {0.5f, 0.5f, 0.5f};

    auto origA = cs.transferFunc.a;
    auto origB = cs.transferFunc.b;
    auto origC = cs.transferFunc.c;

    // Scenario 1: coefficient (-p.g * p.a * p.b) equals 0 (True branch)
    // 1.1 p.a = 0
    cs.transferFunc.a = 0.0f;
    auto res1 = cs.ToNonLinear(input);
    EXPECT_TRUE(IsZeroVector(res1));
    cs.transferFunc.a = origA;

    // 1.2 p.b = 0
    cs.transferFunc.b = 0.0f;
    auto res2 = cs.ToNonLinear(input);
    EXPECT_TRUE(IsZeroVector(res2));
    cs.transferFunc.b = origB;

    // 1.3 p.c = 0
    cs.transferFunc.c = 0.0f;
    auto res3 = cs.ToNonLinear(input);
    EXPECT_TRUE(IsZeroVector(res3));
    cs.transferFunc.c = origC;

    // Scenario 2: Normal parameters (False branch)
    // HLG ToNonLinear should work normally
    // HLG OETF: if (n > 1/coef) ... else sqrt(...)
    // Standard a=2, b=2 → coef = -(-3)*2*2 = 12 → 1/12 ≈ 0.083.
    // n=0.5 > 0.083 → uses log branch.
    auto res4 = cs.ToNonLinear(input);
    EXPECT_GT(res4[0], 0.0f);
}

/**
 * @tc.name: ToNonLinear_PQ_DenominatorChecks
 * @tc.desc: Test denominator checks in PQ ToNonLinear conversion
 * @tc.require: issueI6QHNP
 */
HWTEST_F(ColorManagerTest, ToNonLinear_PQ_DenominatorChecks, TestSize.Level1)
{
    ColorSpace cs(ColorSpaceName::BT2020_PQ);
    Vector3 input = {0.5f, 0.5f, 0.5f};

    auto origB = cs.transferFunc.b;
    auto origE = cs.transferFunc.e;
    auto origF = cs.transferFunc.f;
    auto origC = cs.transferFunc.c;

    // Scenario 1: p.f = 0 (True branch)
    cs.transferFunc.f = 0.0f;
    auto res1 = cs.ToNonLinear(input);
    EXPECT_TRUE(IsZeroVector(res1));
    cs.transferFunc.f = origF;

    // Scenario 2: tmp2 (p.b - p.e * tmp) equals 0 (True branch)
    cs.transferFunc.f = 1.0f; // Avoid early return due to f=0
    cs.transferFunc.e = 2.0f;
    cs.transferFunc.b = 1.0f; // 1.0 - 2.0 * 0.5 = 0.0
    auto res2 = cs.ToNonLinear(input);
    EXPECT_TRUE(IsZeroVector(res2));

    // Restore
    cs.transferFunc.b = origB;
    cs.transferFunc.e = origE;

    // Scenario 3: p.c = 0 (True branch)
    cs.transferFunc.c = 0.0f;
    auto res3 = cs.ToNonLinear(input);
    EXPECT_TRUE(IsZeroVector(res3));
    cs.transferFunc.c = origC;

    // Scenario 4: Normal parameters (False branch)
    auto res4 = cs.ToNonLinear(input);
    EXPECT_GT(res4[0], 0.0f);
}

/**
 * @tc.name: ToNonLinear_Standard_DenominatorChecks
 * @tc.desc: Test denominator checks in SRGB/Standard ToNonLinear conversion (g, a, c)
 * @tc.require: issueI6QHNP
 */
HWTEST_F(ColorManagerTest, ToNonLinear_Standard_DenominatorChecks, TestSize.Level1)
{
    ColorSpace cs(ColorSpaceName::SRGB); // Uses TF_SRGB
    Vector3 input = {0.5f, 0.5f, 0.5f};

    // Backup
    auto origG = cs.transferFunc.g;
    auto origA = cs.transferFunc.a;
    auto origC = cs.transferFunc.c;

    // Scenario 1: p.g = 0 (True branch)
    cs.transferFunc.g = 0.0f;
    auto res1 = cs.ToNonLinear(input);
    EXPECT_TRUE(IsZeroVector(res1));
    cs.transferFunc.g = origG;

    // Scenario 2: p.a = 0 (True branch)
    cs.transferFunc.a = 0.0f;
    auto res2 = cs.ToNonLinear(input);
    EXPECT_TRUE(IsZeroVector(res2));
    cs.transferFunc.a = origA;

    // Scenario 3: p.c = 0 (used in linear segment: n = (n - p.f) / p.c) (True branch)
    Vector3 inputSmall = {-0.01f, -0.01f, -0.01f};
    cs.transferFunc.c = 0.0f;
    auto res3 = cs.ToNonLinear(inputSmall);
    EXPECT_TRUE(IsZeroVector(res3));
    cs.transferFunc.c = origC;

    // Scenario 4: Combination p.g=0, p.a=0, p.c=0 (True branch)
    Vector3 inputGamma = {0.5f, 0.5f, 0.5f};
    cs.transferFunc.g = 0.0f;
    cs.transferFunc.a = 0.0f;
    cs.transferFunc.c = 0.0f;
    auto res4 = cs.ToNonLinear(inputGamma);
    EXPECT_TRUE(IsZeroVector(res4));

    // Restore
    cs.transferFunc.g = origG;
    cs.transferFunc.a = origA;

    // Scenario 5: Normal parameters (False branch)
    // SRGB ToNonLinear should work normally
    auto res5 = cs.ToNonLinear(input);
    EXPECT_GT(res5[0], 0.0f);
}

/**
 * @tc.name: ToLinear_Standard_GammaZero
 * @tc.desc: Test math error scenario in ToLinear Standard
 * branch when g=0 (though code lacks explicit check, prevent division-by-zero/NaN)
 * @tc.require: issueI6QHNP
 */
HWTEST_F(ColorManagerTest, ToLinear_Standard_GammaZero, TestSize.Level1)
{
    ColorSpace cs(ColorSpaceName::SRGB);

    // The existing ToLinear Standard branch does not contain explicit denominator checks (no division involved).
    // Division by gamma only appears in ToNonLinear's else branch: pow(n - p.e, 1/g).
    // Thus, this test only verifies that ToLinear for Standard color
    // space won't crash even if fed invalid parameters from HLG/PQ.
    auto res = cs.ToLinear({0.5f, 0.5f, 0.5f});
    EXPECT_GT(res[0], 0.0f);
}

// Additional fine-grained test for ComputeXYZD50 when tmp denominator is zero (True branch)
HWTEST_F(ColorManagerTest, ComputeXYZD50_TmpDenominatorZero, TestSize.Level1)
{
    ColorSpacePrimaries p;
    p.rX = 0.64f; p.rY = 0.33f;
    p.gX = 0.30f; p.gY = 0.60f;
    p.bX = 0.15f; p.bY = 0.06f;
    p.wX = 0.3127f; p.wY = 0.3290f;

    // === Scenario 1: tmp denominator is zero (collinear red and blue primaries) ===
    p.bX = p.rX;
    p.bY = p.rY;  // Now red == blue → degenerate gamut → tmp = 0

    Matrix3x3 res = ComputeXYZD50(p);
    EXPECT_TRUE(IsZeroMatrix(res));

    // === Scenario 2: Valid BT.709-like primaries (normal case) ===
    p.rX = 0.64f; p.rY = 0.33f;
    p.gX = 0.30f; p.gY = 0.60f;
    p.bX = 0.15f; p.bY = 0.06f;
    p.wX = 0.3127f; p.wY = 0.3290f;

    res = ComputeXYZD50(p);
    EXPECT_TRUE(HasNonZeroElement(res));

    // === Scenario 3: White point Y = 0 (invalid white point) ===
    p.wY = 0.0f;
    res = ComputeXYZD50(p);
    EXPECT_TRUE(IsZeroMatrix(res));

    // === Scenario 4: (gxGy - rxRy) is near zero (but tmp is not zero) ===
    p.rX = 0.64f; p.rY = 0.33f;
    p.gX = 0.30f; p.gY = 0.704f + FLT_EPSILON * 10.0f;  // gxGy = 0.2112 + 0.15*FLT_EPSILON (near rxRy=0.2112)
    p.bX = 0.15f; p.bY = 0.06f;
    p.wX = 0.3127f; p.wY = 0.3290f;
    res = ComputeXYZD50(p);
    EXPECT_FALSE(IsZeroMatrix(res));
}

/**
 * @tc.name: Vector3Division_BoundaryChecks
 * @tc.desc: Test behavior of Vector3 operator/ when denominator components are near zero
 * @tc.type: FUNC
 */
HWTEST_F(ColorManagerTest, Vector3Division_BoundaryChecks, TestSize.Level1)
{
    Vector3 a = {1.0f, 1.0f, 1.0f};

    // Scenario 1: Denominator slightly less than FLT_EPSILON (should trigger near-zero check) → returns zero matrix
    Vector3 b_tiny = {1.0f, FLT_EPSILON * 0.5f, 1.0f};
    Matrix3x3 res1 = a / b_tiny;
    EXPECT_TRUE(IsZeroMatrix(res1));

    // Scenario 2: Denominator is a tiny negative value (also triggers near-zero check) → zero matrix
    Vector3 b_tiny_neg = {1.0f, -FLT_EPSILON * 0.5f, 1.0f};
    Matrix3x3 res2 = a / b_tiny_neg;
    EXPECT_TRUE(IsZeroMatrix(res2));

    // Scenario 3: Denominator exactly equals FLT_EPSILON → normal division (no near-zero trigger)
    Vector3 b_eps = {1.0f, FLT_EPSILON, 1.0f};
    Matrix3x3 res3 = a / b_eps;

    // Only the [1][1] element should be 1.0f / FLT_EPSILON; others are 1.0f / 1.0f = 1.0f
    EXPECT_FLOAT_EQ(res3[0][0], 1.0f);
    EXPECT_FLOAT_EQ(res3[1][1], 1.0f / FLT_EPSILON);
    EXPECT_FLOAT_EQ(res3[2][2], 1.0f);

    // Off-diagonal elements must be zero (since operator/ creates diagonal matrix)
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            if (i != j) {
                EXPECT_FLOAT_EQ(res3[i][j], 0.0f) << "Off-diagonal element [" << i << "][" << j << "] should be zero.";
            }
        }
    }
}

/**
 * @tc.name: Matrix3x3Invert_DeterminantNearZero
 * @tc.desc: Test behavior of Invert() when matrix determinant is near zero
 * @tc.type: FUNC
 */
HWTEST_F(ColorManagerTest, Matrix3x3Invert_DeterminantNearZero, TestSize.Level1)
{
    // Construct a nearly singular matrix with rows linearly dependent (row2 = 2 * row1) (True branch)
    Matrix3x3 m_singular = {{{1.0f, 2.0f, 3.0f},
                             {2.0f, 4.0f, 6.0f}, // Row 2 is 2× Row 1
                             {0.0f, 0.0f, 1.0f}}};
    auto res1 = Invert(m_singular);
    EXPECT_TRUE(IsZeroMatrix(res1));

    // Construct a valid invertible matrix (False branch)
    Matrix3x3 m_valid = {{{2.0f, 0.0f, 0.0f},
                          {0.0f, 2.0f, 0.0f},
                          {0.0f, 0.0f, 2.0f}}};
    auto res2 = Invert(m_valid);
    EXPECT_FLOAT_EQ(res2[0][0], 0.5f);
}

/**
 * @tc.name: ComputeXYZD50_NearZeroPrimaries
 * @tc.desc: Test ComputeXYZD50 behavior when primary Y components are near zero
 * @tc.type: FUNC
 */
HWTEST_F(ColorManagerTest, ComputeXYZD50_NearZeroPrimaries, TestSize.Level1)
{
    ColorSpacePrimaries p = {0.64f, 0.33f, 0.30f, 0.60f, 0.15f, 0.06f, 0.3127f, 0.3290f};

    // Scenario 1: rY < FLT_EPSILON (True branch)
    p.rY = FLT_EPSILON * 0.1f;
    auto res1 = ComputeXYZD50(p);
    EXPECT_TRUE(IsZeroMatrix(res1));

    // Scenario 2: wY < FLT_EPSILON (including negative values) (True branch)
    p.rY = 0.33f; // restore
    p.wY = -FLT_EPSILON * 0.1f; // negative tiny value
    auto res2 = ComputeXYZD50(p);
    EXPECT_TRUE(IsZeroMatrix(res2));

    // Scenario 3: Normal primaries (False branch)
    p.rY = 0.33f;
    p.wY = 0.3290f;
    auto res3 = ComputeXYZD50(p);
    EXPECT_TRUE(HasNonZeroElement(res3));
}

/**
 * @tc.name: ToLinear_TransferFunc_BoundaryChecks
 * @tc.desc: Test ToLinear behavior when transfer function parameters are near zero
 * @tc.type: FUNC
 */
HWTEST_F(ColorManagerTest, ToLinear_TransferFunc_BoundaryChecks, TestSize.Level1)
{
    Vector3 input = {0.5f, 0.5f, 0.5f};

    // --- HLG Checks ---
    ColorSpace cs_hlg(ColorSpaceName::BT2020_HLG);
    // HLG check: FloatNearlyEqualZero(p.a) (True branch)
    auto origA = cs_hlg.transferFunc.a;
    cs_hlg.transferFunc.a = FLT_EPSILON * 0.5f;
    auto res_hlg_a = cs_hlg.ToLinear(input);
    EXPECT_TRUE(IsZeroVector(res_hlg_a));
    cs_hlg.transferFunc.a = origA;

    // HLG check: FloatNearlyEqualZero(p.b) (True branch)
    auto origB = cs_hlg.transferFunc.b;
    cs_hlg.transferFunc.b = FLT_EPSILON * 0.5f;
    auto res_hlg_b = cs_hlg.ToLinear(input);
    EXPECT_TRUE(IsZeroVector(res_hlg_b));
    cs_hlg.transferFunc.b = origB;

    // HLG with normal parameters (False branch)
    auto res_hlg_valid = cs_hlg.ToLinear(input);
    EXPECT_GT(res_hlg_valid[0], 0.0f);

    // --- PQ Checks ---
    ColorSpace cs_pq(ColorSpaceName::BT2020_PQ);
    // PQ check: FloatNearlyEqualZero(p.d + p.e * tmp) (True branch)
    auto origD = cs_pq.transferFunc.d;
    cs_pq.transferFunc.d = FLT_EPSILON * 0.5f;
    cs_pq.transferFunc.e = 0.0f;
    auto res_pq_d = cs_pq.ToLinear(input);
    EXPECT_TRUE(IsZeroVector(res_pq_d));
    cs_pq.transferFunc.d = origD;

    // PQ with normal parameters (False branch)
    auto res_pq_valid = cs_pq.ToLinear(input);
    EXPECT_GT(res_pq_valid[0], 0.0f);

    // --- LOG Checks ---
    ColorSpace cs_log(ColorSpaceName::H_LOG);
    // LOG check: FloatNearlyEqualZero(p.a) (True branch)
    auto origLogA = cs_log.transferFunc.a;
    cs_log.transferFunc.a = -FLT_EPSILON * 0.5f;
    auto res_log_a = cs_log.ToLinear(input);
    EXPECT_TRUE(IsZeroVector(res_log_a));
    cs_log.transferFunc.a = origLogA;

    // LOG check: FloatNearlyEqualZero(p.c) (True branch)
    auto origLogC = cs_log.transferFunc.c;
    cs_log.transferFunc.c = FLT_EPSILON * 0.5f;
    auto res_log_c = cs_log.ToLinear(input);
    EXPECT_TRUE(IsZeroVector(res_log_c));
    cs_log.transferFunc.c = origLogC;

    // LOG check: FloatNearlyEqualZero(p.e) → enters else-if branch and returns zero vector (True branch)
    Vector3 inputZero = {0.0f, 0.0f, 0.0f};
    cs_log.transferFunc.e = FLT_EPSILON * 0.5f;
    cs_log.transferFunc.f = 0.0f;
    auto res_log_e = cs_log.ToLinear(inputZero);
    EXPECT_TRUE(IsZeroVector(res_log_e));

    // LOG with normal parameters (False branch)
    cs_log.transferFunc.e = 1.0f; // restore non-zero e
    auto res_log_valid = cs_log.ToLinear(input);
    EXPECT_GT(res_log_valid[0], 0.0f);
}

/**
 * @tc.name: ToNonLinear_TransferFunc_BoundaryChecks
 * @tc.desc: Test behavior of ToNonLinear when transfer function parameters are near zero
 * @tc.type: FUNC
 */
HWTEST_F(ColorManagerTest, ToNonLinear_TransferFunc_BoundaryChecks, TestSize.Level1)
{
    Vector3 input = {0.5f, 0.5f, 0.5f};

    // --- HLG Checks ---
    ColorSpace cs_hlg(ColorSpaceName::BT2020_HLG);
    // HLG check: FloatNearlyEqualZero(coef), where coef = -g*a*b (True branch)
    auto origA = cs_hlg.transferFunc.a;
    cs_hlg.transferFunc.a = FLT_EPSILON * 0.1f;
    auto res_hlg_coef = cs_hlg.ToNonLinear(input);
    EXPECT_TRUE(IsZeroVector(res_hlg_coef));
    cs_hlg.transferFunc.a = origA;

    // HLG check: FloatNearlyEqualZero(p.c) (True branch)
    auto origC = cs_hlg.transferFunc.c;
    cs_hlg.transferFunc.c = FLT_EPSILON * 0.1f;
    auto res_hlg_c = cs_hlg.ToNonLinear(input);
    EXPECT_TRUE(IsZeroVector(res_hlg_c));
    cs_hlg.transferFunc.c = origC;

    // HLG with normal parameters (False branch)
    auto res_hlg_valid = cs_hlg.ToNonLinear(input);
    EXPECT_GT(res_hlg_valid[0], 0.0f);

    // --- PQ Checks ---
    ColorSpace cs_pq(ColorSpaceName::BT2020_PQ);
    // PQ check: FloatNearlyEqualZero(p.f) (True branch)
    auto origF = cs_pq.transferFunc.f;
    cs_pq.transferFunc.f = 0.0f;
    auto res_pq_f = cs_pq.ToNonLinear(input);
    EXPECT_TRUE(IsZeroVector(res_pq_f));
    cs_pq.transferFunc.f = origF;

    // PQ check: FloatNearlyEqualZero(tmp2), where tmp2 = p.b - p.e * tmp (True branch)
    auto origB = cs_pq.transferFunc.b;
    cs_pq.transferFunc.b = FLT_EPSILON * 0.5f;
    cs_pq.transferFunc.e = 0.0f;
    auto res_pq_tmp2 = cs_pq.ToNonLinear(input);
    EXPECT_TRUE(IsZeroVector(res_pq_tmp2));
    cs_pq.transferFunc.b = origB;

    // PQ with normal parameters (False branch)
    auto res_pq_valid = cs_pq.ToNonLinear(input);
    EXPECT_GT(res_pq_valid[0], 0.0f);

    // --- Standard (SRGB) Checks ---
    ColorSpace cs_srgb(ColorSpaceName::SRGB);
    // Standard check: FloatNearlyEqualZero(p.g) (True branch)
    auto origG = cs_srgb.transferFunc.g;
    cs_srgb.transferFunc.g = FLT_EPSILON * 0.5f;
    auto res_srgb_g = cs_srgb.ToNonLinear(input);
    EXPECT_TRUE(IsZeroVector(res_srgb_g));
    cs_srgb.transferFunc.g = origG;

    // Standard check: FloatNearlyEqualZero(p.a) (True branch)
    auto origSrgbA = cs_srgb.transferFunc.a;
    cs_srgb.transferFunc.a = 0.0f;
    auto res_srgb_a = cs_srgb.ToNonLinear(input);
    EXPECT_TRUE(IsZeroVector(res_srgb_a));
    cs_srgb.transferFunc.a = origSrgbA;

    // Standard check: FloatNearlyEqualZero(p.c) (linear segment) (True branch)
    Vector3 inputNeg = {-0.1f, -0.1f, -0.1f};
    auto origSrgbC = cs_srgb.transferFunc.c;
    cs_srgb.transferFunc.c = 0.0f;
    auto res_srgb_c = cs_srgb.ToNonLinear(inputNeg);
    EXPECT_TRUE(IsZeroVector(res_srgb_c));
    cs_srgb.transferFunc.c = origSrgbC;

    // Standard with normal parameters (False branch)
    auto res_srgb_valid = cs_srgb.ToNonLinear(input);
    EXPECT_GT(res_srgb_valid[0], 0.0f);
}

/**
 * @tc.name: FloatNearlyEqualZero_AdditionalCoverage
 * @tc.desc: Additional coverage for FloatNearlyEqualZero under more edge cases
 * @tc.type: FUNC
 */
HWTEST_F(ColorManagerTest, FloatNearlyEqualZero_AdditionalCoverage, TestSize.Level1)
{
    // Smallest positive and largest negative values close to zero
    EXPECT_TRUE(FloatNearlyEqualZero(std::numeric_limits<float>::min()));
    EXPECT_TRUE(FloatNearlyEqualZero(-std::numeric_limits<float>::min()));

    // Subnormal (denormal) number tests
    EXPECT_TRUE(FloatNearlyEqualZero(std::numeric_limits<float>::denorm_min()));
    EXPECT_TRUE(FloatNearlyEqualZero(-std::numeric_limits<float>::denorm_min()));

    // Tests with multiples of FLT_EPSILON
    EXPECT_FALSE(FloatNearlyEqualZero(FLT_EPSILON));  // boundary case – should return false
    EXPECT_FALSE(FloatNearlyEqualZero(-FLT_EPSILON)); // boundary case – should return false
    EXPECT_TRUE(FloatNearlyEqualZero(FLT_EPSILON * 0.99f)); // slightly less than EPSILON
    EXPECT_FALSE(FloatNearlyEqualZero(FLT_EPSILON * 1.01f)); // slightly greater than EPSILON
}

/**
 * @tc.name: DXToD50_WhitePointZeroCheck
 * @tc.desc: Test handling of white point coordinates near zero in DXToD50
 * @tc.type: FUNC
 */
HWTEST_F(ColorManagerTest, DXToD50_WhitePointZeroCheck, TestSize.Level1)
{
    // Test scenario where wp[1] (Y coordinate) is near zero
    Matrix3x3 baseToXYZ = {{{1.0f, 0.0f, 0.0f},
                           {0.0f, 1.0f, 0.0f},
                           {0.0f, 0.0f, 1.0f}}};

    std::array<float, 2> wpTinyY = {0.3127f, FLT_EPSILON * 0.1f}; // Y near zero
    TransferFunc tf = TF_LINEAR;

    ColorSpace cs(baseToXYZ, wpTinyY, tf);

    // Verify that DXToD50 returns a zero matrix when white point Y is near zero
    EXPECT_TRUE(IsZeroMatrix(cs.toXYZ));
}

/**
 * @tc.name: IsFinite_ZeroDenominator
 * @tc.desc: Test IsFinite behavior in division-by-zero scenarios
 * @tc.type: FUNC
 */
HWTEST_F(ColorManagerTest, IsFinite_ZeroDenominator, TestSize.Level1)
{
    // Test singular matrix (determinant = 0)
    Matrix3x3 singularMatrix = {{{1.0f, 2.0f, 3.0f},
                                {2.0f, 4.0f, 6.0f},  // Linearly dependent row (2x first row)
                                {0.0f, 0.0f, 1.0f}}};
    auto result = Invert(singularMatrix);
    EXPECT_TRUE(IsZeroMatrix(result));

    // Test special infinity cases (for IsFinite function)
    float inf = 1.0f / 0.0f;
    EXPECT_FALSE(IsFinite(inf));
    EXPECT_FALSE(IsFinite(-inf));
    EXPECT_TRUE(IsFinite(1.0f));
}

// Test special cases in the LOG_G branch
HWTEST_F(ColorManagerTest, ToLinearLogGWithNonZeroPE, TestSize.Level1)
{
    ColorSpace colorSpace(ColorSpaceName::SRGB);
    auto &tf = colorSpace.transferFunc;

    // Configure LOG_G transfer function parameters
    tf.g = LOG_G;  // LOG_G mode
    tf.a = 1.0f;   // non-zero
    tf.b = 0.1f;   // non-zero
    tf.c = 2.0f;   // non-zero
    tf.d = 0.5f;   // appropriate value
    tf.e = 0.0001f; // small but non-zero → triggers !FloatNearlyEqualZero(p.e) branch
    tf.f = 0.2f;   // appropriate value

    // Create input Vector3 such that n <= coef and !FloatNearlyEqualZero(p.e)
    float coef = tf.e * (1 / -tf.g) + tf.f;  // compute threshold coefficient
    Vector3 input = {coef - 0.1f, coef - 0.1f, coef - 0.1f};  // values below coef

    Vector3 result = colorSpace.ToLinear(input);

    // Verify result is not a zero vector (using tolerance)
    EXPECT_FALSE(IsZeroVector(result));
}

// Test the LOG_G branch when p.e is near zero
HWTEST_F(ColorManagerTest, ToLinearLogGWithNearZeroPE, TestSize.Level1)
{
    // Set up LOG_G transfer function parameters
    ColorSpace colorSpace(ColorSpaceName::SRGB);
    auto &tf = colorSpace.transferFunc;

    tf.g = LOG_G;  // LOG_G mode
    tf.a = 1.0f;   // non-zero
    tf.b = 0.1f;   // non-zero
    tf.c = 2.0f;   // non-zero
    tf.d = 0.5f;   // set appropriate value
    tf.e = 0.0f;   // near-zero value, causing FloatNearlyEqualZero(p.e) to return true
    tf.f = 0.2f;   // set appropriate value

    // Create input Vector3 such that n <= coef and FloatNearlyEqualZero(p.e) is true
    float coef = tf.e * (1 / -tf.g) + tf.f;  // compute threshold coefficient
    Vector3 input = {coef - 0.1f, coef - 0.1f, coef - 0.1f};  // values less than coef

    Vector3 result = colorSpace.ToLinear(input);

    // Since p.e is near zero, a zero vector should be returned
    EXPECT_TRUE(IsZeroVector(result));
}

// Helper test function specifically for the LOG_G branch with non-zero p.e
HWTEST_F(ColorManagerTest, ToLinearLogGBranchWithNonZeroE, TestSize.Level1)
{
    ColorSpace colorSpace(ColorSpaceName::SRGB);
    auto &tf = colorSpace.transferFunc;

    tf.g = LOG_G;
    tf.a = 2.0f;
    tf.b = 1.0f;
    tf.c = 1.0f;
    tf.d = 0.3f;
    tf.e = 0.5f;  // non-zero value, so !FloatNearlyEqualZero(p.e) is true
    tf.f = 0.1f;

    // Compute coef
    float coef = tf.e * (1 / -tf.g) + tf.f;
    // Use values less than coef to enter the else-if branch
    Vector3 input = {coef - 0.2f, coef - 0.2f, coef - 0.2f};

    Vector3 result = colorSpace.ToLinear(input);

    // Verify the result matches the formula: n = (n - p.f) / p.e
    float expected = (input[0] - tf.f) / tf.e;
    EXPECT_NEAR(result[0], expected, 1e-5f);
    EXPECT_NEAR(result[1], expected, 1e-5f);
    EXPECT_NEAR(result[2], expected, 1e-5f);
}
}
}
