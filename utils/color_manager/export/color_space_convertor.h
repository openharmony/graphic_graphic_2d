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

#ifndef COLORSPACECONVERTOR
#define COLORSPACECONVERTOR

#include "color_space.h"

namespace OHOS {
namespace ColorManager {
class ColorSpaceConvertor {
public:
    ColorSpaceConvertor(const ColorSpace &src, const ColorSpace &dst, GamutMappingMode mappingMode);

    ColorSpace GetSrcColorSpace() const
    {
        return srcColorSpace;
    }

    ColorSpace GetDstColorSpace() const
    {
        return dstColorSpace;
    }

    Matrix3x3 GetTransform() const
    {
        return transferMatrix;
    }
    
    Vector3 Convert(const Vector3& v) const;
    Vector3 ConvertLinear(const Vector3& v) const;

    static Vector3 ConvertSRGBToP3ColorSpace(const Vector3& sRGBColorValue)
    {
        return ColorSpaceConvertor::sRGBtoP3ColorSpaceConvertor.Convert(sRGBColorValue);
    }

    static Vector3 ConvertP3ToSRGBColorSpace(const Vector3& p3ColorValue)
    {
        return ColorSpaceConvertor::P3toSRGBColorSpaceConvertor.Convert(p3ColorValue);
    }

    static ColorSpaceConvertor sRGBtoP3ColorSpaceConvertor;
    static ColorSpaceConvertor P3toSRGBColorSpaceConvertor;

private:
    ColorSpace srcColorSpace;
    ColorSpace dstColorSpace;
    [[maybe_unused]]GamutMappingMode mappingMode;
    Matrix3x3 transferMatrix;
};
}  // namespace ColorManager
}  // namespace OHOS
#endif