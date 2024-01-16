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
    : pixmapImplPtr_(ImplFactory::CreatePixmapImpl())
{}

Pixmap::Pixmap(const ImageInfo& imageInfo, const void* addr, size_t rowBytes)
    : pixmapImplPtr_(ImplFactory::CreatePixmapImpl(imageInfo, addr, rowBytes))
{}

std::shared_ptr<ColorSpace> Pixmap::GetColorSpace() const
{
    return pixmapImplPtr_->GetColorSpace();
}

ColorType Pixmap::GetColorType() const
{
    return pixmapImplPtr_->GetColorType();
}

AlphaType Pixmap::GetAlphaType() const
{
    return pixmapImplPtr_->GetAlphaType();
}

ColorQuad Pixmap::GetColor(int x, int y) const
{
    return pixmapImplPtr_->GetColor(x, y);
}

size_t Pixmap::GetRowBytes() const
{
    return pixmapImplPtr_->GetRowBytes();
}

const void* Pixmap::GetAddr() const
{
    return pixmapImplPtr_->GetAddr();
}

int32_t Pixmap::GetWidth() const
{
    return pixmapImplPtr_->GetWidth();
}

int32_t Pixmap::GetHeight() const
{
    return pixmapImplPtr_->GetHeight();
}

bool Pixmap::ScalePixels(const Pixmap& dst, const SamplingOptions& options) const
{
    return pixmapImplPtr_->ScalePixels(dst, options);
}

Pixmap::~Pixmap() {}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
