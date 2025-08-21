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

#include "image_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "image/bitmap.h"
#include "image/image.h"
#include "utils/rect.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t DATA_MIN_SIZE = 2;
} // namespace

namespace Drawing {
/*
 *  测试使用 Bitmap 构建 Image 的功能，包括:
 *  1. Image::BuildFromBitmap()
 */
bool BuildImageFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    Bitmap bitmap;
    int width = static_cast<int>(data[0]);
    int height = static_cast<int>(data[1]);
    BitmapFormat bitmapFormat = { COLORTYPE_ARGB_4444, ALPHATYPE_OPAQUE };
    bitmap.Build(width, height, bitmapFormat);
    if (bitmap.GetWidth() != width || bitmap.GetHeight() != height) {
        return false;
    }
    Image image;
    image.BuildFromBitmap(bitmap);
    return true;
}

/*
 *  测试 Image 的基本功能，包括:
 *  1. GetHeadroom()
 *  2. SetHeadroom()
 */
bool ImageFuzzTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    Image image;
    Bitmap bitmap;
    int width = static_cast<int>(data[0]);
    int height = static_cast<int>(data[1]);
    float headroom = static_cast<int>(data[0]) / 2.0f;
    BitmapFormat bitmapFormat = { COLORTYPE_ARGB_4444, ALPHATYPE_OPAQUE };
    bool buildBitmap = bitmap.Build(width, height, bitmapFormat);
    if (!buildBitmap) {
        return false;
    }
    image.AsLegacyBitmap(bitmap);
    image.SetHeadroom(headroom);
    image.GetHeadroom();
    return true;
}

/*
 *  测试从 Image 缩放的功能:
 *  1. Image::BuildFromBitmap()
 *  2. Image::ScaleImage()
 */
bool ImageFuzzTest002(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    BitmapFormat bitmapFormat = { COLORTYPE_ARGB_4444, ALPHATYPE_OPAQUE };
    Bitmap srcBitmap;
    int srcWidth = static_cast<int>(data[0]);
    int srcHeight = static_cast<int>(data[1]);
    srcBitmap.Build(srcWidth, srcHeight, bitmapFormat);
    if (srcBitmap.GetWidth() != srcWidth || srcBitmap.GetHeight() != srcHeight) {
        return false;
    }
    std::shared_ptr<Image> srcImage = std::make_shared<Image>();
    srcImage->BuildFromBitmap(srcBitmap);
    Bitmap dstBitmap;
    int dstWidth = static_cast<int>(data[0]);
    int dstHeight = static_cast<int>(data[1]);
    dstBitmap.Build(dstWidth, dstHeight, bitmapFormat);
    if (dstBitmap.GetWidth() != dstWidth || dstBitmap.GetHeight() != dstHeight) {
        return false;
    }
    std::shared_ptr<Image> dstImage = std::make_shared<Image>();
    dstImage->BuildFromBitmap(dstBitmap);

    ScalingType type = static_cast<ScalingType>(static_cast<uint32_t>(data[0]) %
        static_cast<uint32_t>(ScalingType::OPTION_INVALID));
    ScalingOption option = {RectI(0, 0, srcWidth, srcHeight), RectI(0, 0, dstWidth, dstHeight), type};
    Image::ScaleImage(srcImage, dstImage, option);
    return true;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::BuildImageFuzzTest(data, size);
    OHOS::Rosen::Drawing::ImageFuzzTest001(data, size);
    OHOS::Rosen::Drawing::ImageFuzzTest002(data, size);
    return 0;
}