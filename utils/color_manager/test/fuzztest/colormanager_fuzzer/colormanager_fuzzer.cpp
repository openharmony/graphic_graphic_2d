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

#include "colormanager_fuzzer.h"

#include <securec.h>

#include "color.h"
#include "color_space.h"
#include "color_space_convertor.h"

namespace OHOS {
namespace ColorManager {
namespace {
    const uint8_t* g_data = nullptr;
    size_t g_size = 0;
    size_t g_pos;
} // namespace

/*
* describe: get data from outside untrusted data(g_data) which size is according to sizeof(T)
* tips: only support basic type
*/
template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (g_data == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, g_data + g_pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_pos += objectSize;
    return object;
}

bool ColorFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Color randomColor = Color(GetData<float>(), GetData<float>(), GetData<float>(), GetData<float>());
    ColorSpace csObject = ColorSpace(GetData<ColorSpaceName>());
    ColorSpaceConvertor convertor = ColorSpaceConvertor(GetData<ColorSpaceName>(), GetData<ColorSpaceName>(),
                                        GetData<GamutMappingMode>());
    // color test
    Color convertColor = Color();
    (void)randomColor.Convert(GetData<ColorSpaceName>());
    (void)randomColor.Convert(csObject);
    convertColor = randomColor.Convert(convertor);
    (void)randomColor.ColorEqual(convertColor);

    return true;
}

bool ColorSpaceFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    ColorSpace csObject = ColorSpace(GetData<ColorSpaceName>());
    // colorspace test
    ColorSpacePrimaries primaries = GetData<ColorSpacePrimaries>();
    TransferFunc func = GetData<TransferFunc>();
    float gamma = GetData<float>();
    Matrix3x3 toXYZ = GetData<Matrix3x3>();
    std::array<float, DIMES_2> whitePoint = GetData<std::array<float, DIMES_2>>();

    (void)ColorSpace(primaries, func);
    (void)ColorSpace(primaries, gamma);
    (void)ColorSpace(toXYZ, whitePoint, func);
    (void)ColorSpace(toXYZ, whitePoint, gamma);
    csObject.GetColorSpaceName();
    csObject.GetGamma();
    csObject.GetRGBToXYZ();
    csObject.GetWhitePoint();
    csObject.GetXYZToRGB();

    Vector3 color = { GetData<float>(), GetData<float>(), GetData<float>() };
    (void)csObject.ToLinear(color);
    (void)csObject.ToNonLinear(color);

    return true;
}

bool ColorSpaceConvertorFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    ColorSpaceConvertor convertor = ColorSpaceConvertor(GetData<ColorSpaceName>(), GetData<ColorSpaceName>(),
                                        GetData<GamutMappingMode>());
    // convertor test
    Vector3 val = { GetData<float>(), GetData<float>(), GetData<float>() };
    (void)convertor.GetSrcColorSpace();
    (void)convertor.GetDstColorSpace();
    (void)convertor.GetTransform();
    (void)convertor.Convert(val);
    (void)convertor.ConvertLinear(val);

    return true;
}
} // namespace ColorManager
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::ColorManager::ColorFuzzTest(data, size);
    OHOS::ColorManager::ColorSpaceFuzzTest(data, size);
    OHOS::ColorManager::ColorSpaceConvertorFuzzTest(data, size);
    return 0;
}

