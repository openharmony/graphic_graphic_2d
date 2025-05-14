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

#include "blurdrawlooper_fuzzer.h"
#include <cstddef>
#include <cstdint>
#include "get_object.h"
#include "effect/blur_draw_looper.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

bool BlurDrawLooperFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    float blurRadius = GetObject<float>();
    float dx = GetObject<float>();
    float dy = GetObject<float>();
    Color color = Color(GetObject<uint32_t>(), GetObject<uint32_t>(), GetObject<uint32_t>(), GetObject<uint32_t>());
    std::shared_ptr<BlurDrawLooper> blurDrawLooper = BlurDrawLooper::CreateBlurDrawLooper(blurRadius, dx, dy, color);
    BlurDrawLooper blurDrawLooperTwo = BlurDrawLooper(GetObject<float>(), GetObject<float>(), GetObject<float>(),
        color);
    BlurDrawLooper blurDrawLooperThree = BlurDrawLooper(GetObject<float>(), GetObject<float>(), GetObject<float>(),
        color);
    blurDrawLooper->GetRadius();
    blurDrawLooper->GetXOffset();
    blurDrawLooper->GetYOffset();
    blurDrawLooper->GetColor();
    blurDrawLooper->GetMaskFilter();
    if (blurDrawLooperTwo == blurDrawLooperThree) {}
    if (blurDrawLooperTwo != blurDrawLooperThree) {}
    return true;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::BlurDrawLooperFuzzTest(data, size);
    return 0;
}
