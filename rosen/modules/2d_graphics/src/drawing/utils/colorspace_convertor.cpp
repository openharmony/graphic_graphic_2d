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

#include "utils/colorspace_convertor.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
std::shared_ptr<OHOS::Rosen::Drawing::ColorSpace> ColorSpaceConvertor::ColorSpaceConvertToDrawingColorSpace(
    std::shared_ptr<OHOS::ColorManager::ColorSpace> colorspace)
{
    if (colorspace == nullptr) {
        return nullptr;
    }
    OHOS::ColorManager::Matrix3x3 toXYZ = colorspace->GetRGBToXYZ();
    OHOS::ColorManager::TransferFunc transferFunc = colorspace->GetTransferFunction();
    OHOS::Rosen::Drawing::CMSTransferFunction drawingTransferFunc = {
        transferFunc.g,
        transferFunc.a,
        transferFunc.b,
        transferFunc.c,
        transferFunc.d,
        transferFunc.e,
        transferFunc.f
    };
    OHOS::Rosen::Drawing::CMSMatrix3x3 drawingMatrix;

    for (int i = 0; i < DIMES_3; i++) {
        for (int j = 0; j < DIMES_3; j++) {
            drawingMatrix.vals[i][j] = toXYZ[i][j];
        }
    }
    return OHOS::Rosen::Drawing::ColorSpace::CreateCustomRGB(drawingTransferFunc, drawingMatrix);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS