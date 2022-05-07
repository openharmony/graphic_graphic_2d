/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include <map>
#include <utility>
#include <cmath>
#include <cfloat>

namespace OHOS {
namespace ColorManager {
#define DIMES_3 3
#define DIMES_2 2

using Vector3 = std::array<float, DIMES_3>;
using Matrix3x3 = std::array<Vector3, DIMES_3>;

enum ColorSpaceName : uint32_t {
    NONE = 0,
    ACES,
    ACESCG,
    ADOBE_RGB,
    BT2020,
    BT709,
    CIE_LAB,
    CIE_XYZ,
    DCI_P3,
    DISPLAY_P3,
    EXTENDED_SRGB,
    LINEAR_EXTENDED_SRGB,
    LINEAR_SRGB,
    NTSC_1953,
    PRO_PHOTO_RGB,
    SMPTE_C,
    SRGB,
    CUSTOM,
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

// Compute a toXYZD50 matrix from a given rgb and white point
Matrix3x3 ComputeXYZ(const ColorSpacePrimaries& primaries);

class ColorSpace {
public:
    ColorSpace(ColorSpaceName name);

    ColorSpace(const ColorSpacePrimaries &primaries, const TransferFunc &transferFunc);

    ColorSpace(const ColorSpacePrimaries &primaries, float gamma);

    ColorSpace(const Matrix3x3& toXYZ, const std::array<float, 2> &whitePoint, const TransferFunc& transferFunc);

    ColorSpace(const Matrix3x3& toXYZ, const std::array<float, 2>& whitePoint, float gamma);

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

    Vector3 ToLinear(Vector3 color) const;
    Vector3 ToNonLinear(Vector3 color) const;

    float clampMin = 0.0f;
    float clampMax = 1.0f;

private:
    ColorSpaceName colorSpaceName = ColorSpaceName::SRGB;
    Matrix3x3 toXYZ;
    std::array<float, DIMES_2> whitePoint;
    TransferFunc transferFunc = {};
};
} // namespace ColorSpace
} // namespace OHOS
#endif  // COLORSPACE
