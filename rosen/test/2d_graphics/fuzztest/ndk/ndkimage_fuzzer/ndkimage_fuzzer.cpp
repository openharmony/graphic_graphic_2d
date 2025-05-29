/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#include "ndkimage_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "get_object.h"

#include "drawing_bitmap.h"
#include "drawing_image.h"


namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t DATA_MIN_SIZE = 2;
constexpr size_t COLORFORMAT_SIZE = 6;
constexpr size_t ALPHAFORMAT_SIZE = 4;
} // namespace

namespace Drawing {

void NativeDrawingImageTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    OH_Drawing_Image* image = OH_Drawing_ImageCreate();
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreate();

    OH_Drawing_ImageBuildFromBitmap(nullptr, bitmap);
    OH_Drawing_ImageBuildFromBitmap(image, bitmap);
    OH_Drawing_ImageGetWidth(nullptr);
    OH_Drawing_ImageGetWidth(image);
    OH_Drawing_ImageGetHeight(nullptr);
    OH_Drawing_ImageGetHeight(image);

    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    uint32_t colorType = GetObject<uint32_t>();
    uint32_t alphaType = GetObject<uint32_t>();
    OH_Drawing_Image_Info imageInfo = { width, height,
        static_cast<OH_Drawing_ColorFormat>(colorType % COLORFORMAT_SIZE),
        static_cast<OH_Drawing_AlphaFormat>(alphaType % ALPHAFORMAT_SIZE) };
    OH_Drawing_ImageGetImageInfo(nullptr, &imageInfo);
    OH_Drawing_ImageGetImageInfo(image, &imageInfo);

    OH_Drawing_BitmapDestroy(bitmap);
    OH_Drawing_ImageDestroy(image);
}


} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::NativeDrawingImageTest(data, size);
    return 0;
}
