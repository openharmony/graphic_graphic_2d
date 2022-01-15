/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include <iostream>
#include <string>

#include "include/core/SkBitmap.h"
#if defined(USE_CANVASKIT0310_SKIA)
#include "include/core/SkSamplingOptions.h"
#endif

#include "image/bitmap.h"
#include "image/image.h"

#include "skia_bitmap.h"
#include "skia_image.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
SkiaImage::SkiaImage() noexcept : skiaImage_(nullptr) {}

void* SkiaImage::BuildFromBitmap(const Bitmap& bitmap)
{
    auto skBitmapImpl = bitmap.GetImpl<SkiaBitmap>();
    if (skBitmapImpl != nullptr) {
        const SkBitmap skBitmap = skBitmapImpl->ExportSkiaBitmap();
        skiaImage_ = SkImage::MakeFromBitmap(skBitmap);
    }
    return nullptr;
}

int SkiaImage::GetWidth()
{
    return skiaImage_->width();
}

int SkiaImage::GetHeight()
{
    return skiaImage_->height();
}

const sk_sp<SkImage> SkiaImage::GetImage() const
{
    return skiaImage_;
}
}
}
}
