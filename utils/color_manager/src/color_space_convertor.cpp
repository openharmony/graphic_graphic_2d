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

namespace OHOS {
namespace ColorManager {
ColorSpaceConvertor::ColorSpaceConvertor(const ColorSpace &src,
    const ColorSpace &dst, GamutMappingMode mappingMode)
    : srcColorSpace(src), dstColorSpace(dst), mappingMode(mappingMode)
{
    transferMatrix = dstColorSpace.GetXYZToRGB() * srcColorSpace.GetRGBToXYZ();
}

Vector3 ColorSpaceConvertor::Convert(const Vector3& v) const
{
    Vector3 srcLinear = v;
    for (auto& n : srcLinear) {
        n = std::clamp(n, srcColorSpace.clampMin, srcColorSpace.clampMax);
    }
    srcLinear = srcColorSpace.ToLinear(srcLinear);

    Vector3 dstNonLinear =
        dstColorSpace.ToNonLinear(transferMatrix * srcLinear);
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

    Vector3 dstLinear = transferMatrix * srcLinear;
    for (auto& n : dstLinear) {
        n = std::clamp(n, dstColorSpace.clampMin, dstColorSpace.clampMax);
    }
    return dstLinear;
}

ColorSpaceConvertor ColorSpaceConvertor::sRGBtoP3ColorSpaceConvertor(ColorSpace(ColorSpaceName::SRGB),
    ColorSpace(ColorSpaceName::DISPLAY_P3), GamutMappingMode::GAMUT_MAP_CONSTANT);
ColorSpaceConvertor ColorSpaceConvertor::P3toSRGBColorSpaceConvertor(ColorSpace(ColorSpaceName::DISPLAY_P3),
    ColorSpace(ColorSpaceName::SRGB), GamutMappingMode::GAMUT_MAP_CONSTANT);
}  // namespace ColorManager
}  // namespace OHOS