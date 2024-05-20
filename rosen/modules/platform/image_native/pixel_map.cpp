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

#include "pixel_map.h"

namespace OHOS {
namespace Media {

std::unique_ptr<PixelMap> PixelMap::Create(const InitializationOptions &opts)
{
    return nullptr;
}

int32_t PixelMap::GetRowBytes()
{
    return 0;
}

int32_t PixelMap::GetWidth()
{
    return 0;
}

int32_t PixelMap::GetHeight()
{
    return 0;
}

int32_t PixelMap::GetByteCount()
{
    return 0;
}

const uint32_t *PixelMap::GetPixel32(int32_t x, int32_t y)
{
    return nullptr;
}

void PixelMap::GetImageInfo(ImageInfo &imageInfo)
{
}

const uint8_t *PixelMap::GetPixels()
{
    return nullptr;
}

AllocatorType PixelMap::GetAllocatorType()
{
    return allocatorType_;
}

PixelMap *PixelMap::Unmarshalling(Parcel &data)
{
    return nullptr;
}

#ifdef IMAGE_COLORSPACE_FLAG
void PixelMap::InnerSetColorSpace(const OHOS::ColorManager::ColorSpace &grColorSpace)
{
    return;
}

OHOS::ColorManager::ColorSpace PixelMap::InnerGetGrColorSpace()
{
    return OHOS::ColorManager::ColorSpace(OHOS::ColorManager::ColorSpaceName::SRGB);
}
#endif
} // namespace Media
} // namespace OHOS
