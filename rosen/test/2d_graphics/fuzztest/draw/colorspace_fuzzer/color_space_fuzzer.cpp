/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "color_space_fuzzer.h"
#include <cstddef>
#include <cstdint>
#include "get_object.h"
#include "draw/color.h"
#include "effect/color_space.h"
#include "image/bitmap.h"
#include "image/image.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t CMSMATRIX_SIZE = 3;
constexpr size_t FUNCTYPE_SIZE = 4;
constexpr size_t MATRIXTYPE_SIZE = 5;
constexpr size_t MAX_SIZE = 5000;
}
namespace Drawing {
/*
 * 测试以下 ColorSpace 接口：
 * 1. CreateSRGB()
 * 2. Deserialize(...)
 * 3. Serialize()
 * 4. GetSkColorSpace()
 * 5. GetType()
 * 6. GetDrawingType()
 */
bool ColorSpaceFuzzTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<ColorSpace> colorSpace = ColorSpace::CreateSRGB();
    auto dataVal = std::make_shared<Data>();
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    dataVal->BuildWithoutCopy(dataText, length);
    colorSpace->Deserialize(dataVal);
    colorSpace->Serialize();
    colorSpace->GetSkColorSpace();
    colorSpace->GetType();
    colorSpace->GetDrawingType();
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }

    return true;
}

/*
 * 测试以下 ColorSpace 接口：
 * 1. CreateSRGBLinear()
 * 2. CreateRGB(...)
 * 3. CreateCustomRGB(...)
 */
bool ColorSpaceFuzzTest002(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<ColorSpace> colorSpaceOne = ColorSpace::CreateSRGBLinear();
    uint32_t funcType = GetObject<uint32_t>();
    uint32_t matrixType = GetObject<uint32_t>();
    std::shared_ptr<ColorSpace> colorSpaceTwo = ColorSpace::CreateRGB(
        static_cast<CMSTransferFuncType>(funcType % FUNCTYPE_SIZE),
        static_cast<CMSMatrixType>(matrixType % MATRIXTYPE_SIZE));
    float g = GetObject<float>();
    float a = GetObject<float>();
    float b = GetObject<float>();
    float c = GetObject<float>();
    float d = GetObject<float>();
    float e = GetObject<float>();
    float f = GetObject<float>();
    CMSTransferFunction func { g, a, b, c, d, e, f };
    CMSMatrix3x3 matrix;
    for (uint32_t i = 0; i < CMSMATRIX_SIZE; i++) {
        for (uint32_t j = 0; j < CMSMATRIX_SIZE; j++) {
            matrix.vals[i][j] = GetObject<float>();
        }
    }
    std::shared_ptr<ColorSpace> colorSpaceThree = ColorSpace::CreateCustomRGB(func, matrix);

    return true;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::ColorSpaceFuzzTest001(data, size);
    OHOS::Rosen::Drawing::ColorSpaceFuzzTest002(data, size);
    return 0;
}
