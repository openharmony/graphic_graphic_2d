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

#ifndef COLORSPACE
#define COLORSPACE

#include <array>
#include <cfloat>
#include <cmath>
#include <map>
#include <utility>

#include "include/core/SkColorSpace.h"
#ifdef USE_M133_SKIA
#include "modules/skcms/skcms.h"
#else
#include "include/third_party/skcms/skcms.h"
#endif

namespace OHOS {
namespace ColorManager {
#define DIMES_3 3
#define DIMES_2 2

using Vector3 = std::array<float, DIMES_3>;
using Matrix3x3 = std::array<Vector3, DIMES_3>;
static constexpr float COLOR_EPSILON = 0.0018f; // 1/255/2 = 0.00196
static const std::array<float, DIMES_2> ILLUMINANT_D50_XY = {0.34567f, 0.35850f};
static const Vector3 ILLUMINANT_D50_XYZ = {0.964212f, 1.0f, 0.825188f};
static const Matrix3x3 BRADFORD = {{
    {0.8951f, 0.2664f, -0.1614f},
    {-0.7502f, 1.7135f, 0.0367f},
    {0.0389f, -0.0685f, 1.0296f}}};
static const Matrix3x3 BRADFORD_INV = {{
    {0.9869929f, -0.1470543f, 0.1599627f},
    {0.4323053f,  0.5183603f, 0.0492912f},
    {-0.0085287f, 0.0400428f, 0.9684867f},
}};

enum ColorSpaceName : uint32_t {
    NONE = 0,
    ADOBE_RGB = 1,
    DCI_P3 = 2,
    DISPLAY_P3 = 3,
    SRGB = 4,
    BT709 = 6,
    BT601_EBU = 7,
    BT601_SMPTE_C = 8,
    BT2020_HLG = 9,
    BT2020_PQ = 10,
    P3_HLG = 11,
    P3_PQ = 12,
    ADOBE_RGB_LIMIT = 13,
    DISPLAY_P3_LIMIT = 14,
    SRGB_LIMIT = 15,
    BT709_LIMIT = 16,
    BT601_EBU_LIMIT = 17,
    BT601_SMPTE_C_LIMIT = 18,
    BT2020_HLG_LIMIT = 19,
    BT2020_PQ_LIMIT = 20,
    P3_HLG_LIMIT = 21,
    P3_PQ_LIMIT = 22,
    LINEAR_P3 = 23,
    LINEAR_SRGB = 24,
    LINEAR_BT709 = LINEAR_SRGB,
    LINEAR_BT2020 = 25,
    DISPLAY_SRGB = SRGB,
    DISPLAY_P3_SRGB = DISPLAY_P3,
    DISPLAY_P3_HLG = P3_HLG,
    DISPLAY_P3_PQ = P3_PQ,
    DISPLAY_BT2020_SRGB = 26,
    DISPLAY_BT2020_HLG = BT2020_HLG,
    DISPLAY_BT2020_PQ = BT2020_PQ,
    BT2020 = 27,
    NTSC_1953 = 28,
    PRO_PHOTO_RGB = 29,
    ACES = 30,
    ACESCG = 31,
    CIE_LAB = 32,
    CIE_XYZ = 33,
    EXTENDED_SRGB = 34,
    LINEAR_EXTENDED_SRGB = 35,
    SMPTE_C = 36,
    CUSTOM = 5,
    H_LOG = 37,
};

enum GamutMappingMode : uint32_t {
    GAMUT_MAP_CONSTANT = 0,
    GAMUT_MAP_EXPENSION,
    GAMUT_MAP_HDR_CONSTANT,
    GAMUT_MAP_HDR_EXPENSION,
};

struct ColorSpacePrimaries {
    float rX;
    float rY;
    float gX;
    float gY;
    float bX;
    float bY;
    float wX;
    float wY;
};

struct TransferFunc {
    float g;
    float a;
    float b;
    float c;
    float d;
    float e;
    float f;
};

Matrix3x3 operator*(const Matrix3x3& a, const Matrix3x3& b);

Vector3 operator*(const Vector3& x, const Matrix3x3& a);

Vector3 operator*(const Matrix3x3& a, const Vector3& x);

Matrix3x3 operator/(const Vector3& a, const Vector3& b);

Matrix3x3 Invert(const Matrix3x3& src);

inline Vector3 XYZ(const Vector3& xyY)
{
    return Vector3 {(xyY[0] * xyY[2]) / xyY[1], xyY[2],
        ((1 - xyY[0] - xyY[1]) * xyY[2]) / xyY[1]};
}

inline bool FloatEqual(const float src, const float dst)
{
    return fabs(src - dst) < COLOR_EPSILON;
}

inline bool FloatNearlyEqualZero(const float num)
{
    return fabs(num) < FLT_EPSILON;
}

inline std::array<float, DIMES_2> ComputeWhitePoint(Matrix3x3 &toXYZ)
{
    Vector3 w = toXYZ * Vector3 {1.0f};
    float sumW = w[0] + w[1] + w[2];
    return {{w[0] / sumW, w[1] / sumW}};
}

inline Matrix3x3 SkToXYZToMatrix3(const skcms_Matrix3x3 &skToXYZ)
{
    return {{{skToXYZ.vals[0][0], skToXYZ.vals[0][1], skToXYZ.vals[0][2]},
        {skToXYZ.vals[1][0], skToXYZ.vals[1][1], skToXYZ.vals[1][2]},
        {skToXYZ.vals[2][0], skToXYZ.vals[2][1], skToXYZ.vals[2][2]}}};
}

// Compute a toXYZD50 matrix from a given rgb and white point
Matrix3x3 ComputeXYZD50(const ColorSpacePrimaries& primaries);

// Convert toXYZ to toXYZD50 matrix with its white point
Matrix3x3 DXToD50(const Matrix3x3 &toXYZ, const std::array<float, DIMES_2> &wp);

class ColorSpace {
public:
    ColorSpace(ColorSpaceName name);

    ColorSpace(const ColorSpacePrimaries &primaries, const TransferFunc &transferFunc);

    ColorSpace(const ColorSpacePrimaries &primaries, float gamma);

    ColorSpace(const Matrix3x3& toXYZ, const std::array<float, 2> &whitePoint, const TransferFunc& transferFunc);

    ColorSpace(const Matrix3x3& toXYZ, const std::array<float, 2>& whitePoint, float gamma);

    // convert SKColorSpace to OHOS ColorSpce
    ColorSpace(const sk_sp<SkColorSpace> src, ColorSpaceName name = ColorSpaceName::CUSTOM);

    ColorSpace(const skcms_ICCProfile& srcIcc, ColorSpaceName name = ColorSpaceName::CUSTOM);

    ColorSpaceName GetColorSpaceName() const
    {
        return colorSpaceName;
    }

    Matrix3x3 GetRGBToXYZ() const
    {
        return toXYZ;
    }

    Matrix3x3 GetXYZToRGB() const
    {
        auto toRGB = Invert(toXYZ);
        return toRGB;
    }

    std::array<float, DIMES_2> GetWhitePoint() const
    {
        return whitePoint;
    }

    float GetGamma() const
    {
        return transferFunc.g;
    }

    TransferFunc GetTransferFunction() const
    {
        return transferFunc;
    }

    Vector3 ToLinear(Vector3 color) const;
    Vector3 ToNonLinear(Vector3 color) const;

    // convert OHOS ColorSpce to SKColorSpace
    sk_sp<SkColorSpace> ToSkColorSpace() const;

    float clampMin = 0.0f;
    float clampMax = 1.0f;

private:
    skcms_Matrix3x3 ToSkiaXYZ() const;

    ColorSpaceName colorSpaceName = ColorSpaceName::SRGB;
    Matrix3x3 toXYZ;
    std::array<float, DIMES_2> whitePoint;
    TransferFunc transferFunc = {};
};

const ColorSpacePrimaries CSP_ADOBE_RGB = {0.640f, 0.330f, 0.210f, 0.710f, 0.150f, 0.060f, 0.3127f, 0.3290f};
const ColorSpacePrimaries CSP_P3_DCI = {0.680f, 0.320f, 0.265f, 0.690f, 0.150f, 0.060f, 0.314f, 0.351f};
const ColorSpacePrimaries CSP_P3_D65 = {0.680f, 0.320f, 0.265f, 0.690f, 0.150f, 0.060f, 0.3127f, 0.3290f};
const ColorSpacePrimaries CSP_BT709 = {0.640f, 0.330f, 0.300f, 0.600f, 0.150f, 0.060f, 0.3127f, 0.3290f};
const ColorSpacePrimaries CSP_BT601_P = {0.640f, 0.330f, 0.290f, 0.600f, 0.150f, 0.060f, 0.3127f, 0.3290f};
const ColorSpacePrimaries CSP_BT601_N = {0.630f, 0.340f, 0.310f, 0.595f, 0.155f, 0.070f, 0.3127f, 0.3290f};
const ColorSpacePrimaries CSP_BT2020 = {0.708f, 0.292f, 0.170f, 0.797f, 0.131f, 0.046f, 0.3127f, 0.3290f};
const ColorSpacePrimaries CSP_NTSC_1953 = {0.670f, 0.330f, 0.210f, 0.710f, 0.140f, 0.080f, 0.310f, 0.316f};
const ColorSpacePrimaries CSP_PRO_PHOTO_RGB = {0.7347f, 0.2653f, 0.1596f, 0.8404f, 0.0366f, 0.0001f, 0.34567f,
    0.35850f};

// use unique g value to represent HLG and PG transfer function
constexpr float HLG_G = -3.0f;
constexpr float PQ_G = -2.0f;
constexpr float LOG_G = -100.0f;

const TransferFunc TF_ADOBE_RGB = {2.2f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
const TransferFunc TF_GAMMA_2_6 = {2.6f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
const TransferFunc TF_SRGB = {2.4f, 1 / 1.055f, 0.055f / 1.055f, 1 / 12.92f, 0.04045f, 0.0f, 0.0f};
const TransferFunc TF_BT709 = {1 / 0.45f, 1 / 1.099f, 0.099f / 1.099f, 1 / 4.5f, 0.081f, 0.0f, 0.0f};
const TransferFunc TF_HLG = {HLG_G, 2.0f, 2.0f, 1 / 0.17883277f, 0.28466892f, 0.55991073f, 0.0f};
const TransferFunc TF_PQ = {PQ_G, -107 / 128.0f, 1.0f, 32 / 2523.0f, 2413 / 128.0f, -2392 / 128.0f, 8192 / 1305.0f};
const TransferFunc TF_LINEAR = {1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
const TransferFunc TF_PRO_PHOTO_RGB = {1.8f, 1.0f, 0.0f, 1 / 16.0f, 0.031248f, 0.0f, 0.0f};
const TransferFunc TF_LOG = {LOG_G, 5.555556f, 0.050943f, 0.130233f, 0.452962f, 6.842619f, 0.092864f};

} // namespace ColorSpace
} // namespace OHOS
#endif  // COLORSPACE
