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
#include "color_space_convertor.h"
#include <algorithm>

namespace OHOS {
namespace ColorManager {
static const std::array<float, DIMES_2> ILLUMINANT_D50_XY = {0.34567f, 0.35850f};
static const Vector3 ILLUMINANT_D50_XYZ = {0.964212f, 1.0f, 0.825188f};
static const Matrix3x3 BRADFORD = {{
    {0.8951f, -0.7502f, 0.0389f},
    {0.2664f, 1.7135f, -0.0685f},
    {-0.1614f, 0.0367f, 1.0296f}}};

static bool Equal(std::array<float, DIMES_2> vecA, std::array<float, DIMES_2> vecB)
{
    for (unsigned i = 0; i < vecA.size(); i++) {
        if (std::abs(vecA[i] - vecB[i]) > 1e-3f) {
            return false;
        }
    }
    return true;
}

static Matrix3x3 Adaptation(const Matrix3x3& matrix,
                            const Vector3& srcWhitePoint,
                            const Vector3& dstWhitePoint)
{
    Vector3 srcLMS = matrix * srcWhitePoint;
    Vector3 dstLMS = matrix * dstWhitePoint;
    return Invert(matrix) * (dstLMS / srcLMS) * matrix;
}

static Vector3 XYZ(const Vector3& xyY)
{
    return Vector3 {(xyY[0] * xyY[2]) / xyY[1], xyY[2],
        ((1 - xyY[0] - xyY[1]) * xyY[2]) / xyY[1]};
}

ColorSpaceConvertor::ColorSpaceConvertor(const ColorSpace &src,
    const ColorSpace &dst, GamutMappingMode mappingMode)
    : srcColorSpace(src), dstColorSpace(dst), mappingMode(mappingMode)
{
    if (Equal(srcColorSpace.GetWhitePoint(), dstColorSpace.GetWhitePoint())) {
        transferMatrix = srcColorSpace.GetRGBToXYZ() * dstColorSpace.GetXYZToRGB();
    } else {
        Matrix3x3 rgbToXYZ(srcColorSpace.GetRGBToXYZ());
        Matrix3x3 xyzToRGB(dstColorSpace.GetXYZToRGB());

        Vector3 srcXYZ = XYZ(Vector3 {srcColorSpace.GetWhitePoint()[0],
            srcColorSpace.GetWhitePoint()[1], 1});
        Vector3 dstXYZ = XYZ(Vector3 {dstColorSpace.GetWhitePoint()[0],
            dstColorSpace.GetWhitePoint()[1], 1});

        if (!Equal(srcColorSpace.GetWhitePoint(), ILLUMINANT_D50_XY)) {
            rgbToXYZ = srcColorSpace.GetRGBToXYZ() * Adaptation(BRADFORD, srcXYZ, ILLUMINANT_D50_XYZ);
        }

        if (!Equal(dstColorSpace.GetWhitePoint(), ILLUMINANT_D50_XY)) {
            xyzToRGB = dstColorSpace.GetRGBToXYZ() * Invert(Adaptation(BRADFORD, dstXYZ, ILLUMINANT_D50_XYZ));
        }

        transferMatrix = rgbToXYZ * xyzToRGB;
    }
}

Vector3 ColorSpaceConvertor::Convert(const Vector3& v) const
{
    Vector3 srcLinear = v;
    for (auto& n : srcLinear) {
        n = std::clamp(n, srcColorSpace.clampMin, srcColorSpace.clampMax);
    }
    srcLinear = srcColorSpace.ToLinear(srcLinear);

    Vector3 dstNonLinear =
        dstColorSpace.ToNonLinear(srcLinear * transferMatrix);
    for (auto& n : dstNonLinear) {
        n = std::clamp(n, dstColorSpace.clampMin, dstColorSpace.clampMax);
    }
    return dstNonLinear;
}

Vector3 ColorSpaceConvertor::ConvertLinear(const Vector3& v) const
{
    Vector3 srcLinear = v;
    for (auto& n : srcLinear) {
        n = std::clamp(n, srcColorSpace.clampMin, srcColorSpace.clampMax);
    }

    Vector3 dstLinear = srcLinear * transferMatrix;
    for (auto& n : dstLinear) {
        n = std::clamp(n, dstColorSpace.clampMin, dstColorSpace.clampMax);
    }
    return dstLinear;
}
}  // namespace ColorManager
}  // namespace OHOS