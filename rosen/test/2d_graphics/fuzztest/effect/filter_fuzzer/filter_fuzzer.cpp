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

#include "filter_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "get_object.h"
#include "effect/filter.h"
#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
constexpr size_t BLURTYPE_SIZE = 4;
constexpr size_t FQ_SIZE = 4;
namespace Drawing {
bool FilterFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Filter filter;
    std::shared_ptr<ColorFilter> colorFilter = ColorFilter::CreateLinearToSrgbGamma();
    filter.SetColorFilter(colorFilter);
    filter.GetColorFilter();
    filter.GetColorFilterPtr();
    scalar sigmaX = GetObject<scalar>();
    scalar sigmaY = GetObject<scalar>();
    std::shared_ptr<ImageFilter> imageFilter = ImageFilter::CreateColorBlurImageFilter(*colorFilter, sigmaX, sigmaY);
    filter.SetImageFilter(imageFilter);
    filter.GetImageFilter();
    filter.GetImageFilterPtr();
    uint32_t blurType = GetObject<uint32_t>();
    scalar sigma = GetObject<scalar>();
    bool respectCTM = GetObject<bool>();
    std::shared_ptr<MaskFilter> maskFilter = MaskFilter::CreateBlurMaskFilter(
        static_cast<BlurType>(blurType % BLURTYPE_SIZE), sigma, respectCTM);
    filter.SetMaskFilter(maskFilter);
    filter.GetMaskFilter();
    filter.GetMaskFilterPtr();
    filter.GetFilterQuality();
    uint32_t fq = GetObject<uint32_t>();
    filter.SetFilterQuality(static_cast<Filter::FilterQuality>(fq % FQ_SIZE));
    filter.Reset();
    Filter filterTwo;
    if (filter == filterTwo) {}
    if (filter != filterTwo) {}
    return true;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::FilterFuzzTest(data, size);
    return 0;
}
