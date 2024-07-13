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

#include "maskfilter_fuzzer.h"
#include <cstddef>
#include <cstdint>
#include "get_object.h"
#include "effect/mask_filter.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
constexpr size_t BLURTYPE_SIZE = 4;
constexpr size_t FILTERTYPE_SIZE = 2;

bool MaskFilterFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t blurType = GetObject<uint32_t>();
    float sigma = GetObject<float>();
    bool respectCTM = GetObject<bool>();
    std::shared_ptr<MaskFilter> maskFilter = MaskFilter::CreateBlurMaskFilter(
        static_cast<BlurType>(blurType % BLURTYPE_SIZE), sigma, respectCTM);
    maskFilter->GetType();
    maskFilter->GetDrawingType();
    uint32_t t = GetObject<uint32_t>();
    MaskFilter(static_cast<MaskFilter::FilterType>(t % FILTERTYPE_SIZE),
        static_cast<BlurType>(blurType % BLURTYPE_SIZE), sigma, respectCTM);
    MaskFilter(static_cast<MaskFilter::FilterType>(t % FILTERTYPE_SIZE));
    maskFilter->Serialize();
    maskFilter->Deserialize(nullptr);
    return true;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::MaskFilterFuzzTest(data, size);
    return 0;
}
