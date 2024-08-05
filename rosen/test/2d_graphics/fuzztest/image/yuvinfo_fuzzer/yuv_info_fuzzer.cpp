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

#include "yuv_info_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "get_object.h"
#include "image/yuv_info.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t MATH_THREE = 3;
constexpr size_t MATH_TWO = 2;
} // namespace

namespace Drawing {
void YuvInfoFuzzTest000(const uint8_t* data, size_t size)
{
    g_data = data;
    g_size = size;
    g_pos = 0;

    int width = GetObject<int>();
    int height = GetObject<int>();
    uint32_t config = GetObject<uint32_t>();
    uint32_t sampling = GetObject<uint32_t>();
    uint32_t colorSpace = GetObject<uint32_t>();
    uint32_t dataType = GetObject<uint32_t>();

    YUVInfo yuv = YUVInfo(width, height, static_cast<YUVInfo::PlaneConfig>(config % MATH_THREE),
        static_cast<YUVInfo::SubSampling>(sampling % MATH_TWO),
        static_cast<YUVInfo::YUVColorSpace>(colorSpace % MATH_THREE),
        static_cast<YUVInfo::YUVDataType>(dataType % MATH_TWO));
    yuv.GetWidth();
    yuv.GetHeight();
    yuv.GetConfig();
    yuv.GetSampling();
    yuv.GetColorSpace();
    yuv.GetDataType();
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::YuvInfoFuzzTest000(data, size);
    return 0;
}