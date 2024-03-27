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

#include "image_filter_fuzzer.h"
#include <cstddef>
#include <cstdint>
#include "get_object.h"
#include "effect/image_filter.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

bool ImageFilterFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<ColorFilter> colorFilter = ColorFilter::CreateLinearToSrgbGamma();
    scalar sigmaX = GetObject<scalar>();
    scalar sigmaY = GetObject<scalar>();
    std::shared_ptr<ImageFilter> imageFilter = ImageFilter::CreateColorBlurImageFilter(
        *colorFilter, sigmaX, sigmaY);
    scalar sigmaX2 = GetObject<scalar>();
    scalar sigmaY2 = GetObject<scalar>();
    imageFilter->InitWithColorBlur(*colorFilter, sigmaX2, sigmaY2, ImageBlurType::GAUSS);
    imageFilter->Serialize();
    imageFilter->Deserialize(nullptr);
    imageFilter->GetType();
    imageFilter->GetDrawingType();

    return true;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::ImageFilterFuzzTest(data, size);
    return 0;
}
