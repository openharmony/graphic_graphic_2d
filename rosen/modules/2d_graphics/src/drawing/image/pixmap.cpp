/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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

#include "image/bitmap.h"

#include "impl_factory.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
Pixmap::Pixmap()
    : pixmapImplPtr(ImplFactory::CreatePixmapImpl())
{}

Pixmap::Pixmap(const ImageInfo& imageInfo, const void* addr, size_t rowBytes)
    : pixmapImplPtr(ImplFactory::CreatePixmapImpl(imageInfo, addr, rowBytes))
{}

std::shared_ptr<ColorSpace> Pixmap::GetColorSpace()
{
    return pixmapImplPtr->GetColorSpace();
}

ColorType Pixmap::GetColorType()
{
    return pixmapImplPtr->GetColorType();
}

AlphaType Pixmap::GetAlphaType()
{
    return pixmapImplPtr->GetAlphaType();
}

size_t Pixmap::GetRowBytes()
{
    return pixmapImplPtr->GetRowBytes();
}

const void* Pixmap::GetAddr()
{
    return pixmapImplPtr->GetAddr();
}

int Pixmap::GetWidth()
{
    return pixmapImplPtr->GetWidth();
}

int Pixmap::GetHeight()
{
    return pixmapImplPtr->GetHeight();
}

Pixmap::~Pixmap() {}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
