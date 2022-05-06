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

#include "bitmap_build_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "image/bitmap.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
    bool BitmapBuildFuzzTest(const uint8_t* data, size_t size)
    {
        bool result = false;
        Bitmap bitmap;
        BitmapFormat bitmapFormat = {COLORTYPE_ARGB_4444, ALPHATYPE_OPAQUYE};
        bitmap.Build(reinterpret_cast<const uint32_t>(data), size, bitmapFormat);
        if (!bitmap.GetWidth()) {
            result = true;
        }
        return result;
    }
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::BitmapBuildFuzzTest(data, size);
    return 0;
}