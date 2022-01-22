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

#include "image/image.h"

#include "impl_factory.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
Image::Image() noexcept : imageImplPtr(ImplFactory::CreateImageImpl()), width_(0), height_(0) {}

Image* Image::BuildFromBitmap(const Bitmap& bitmap)
{
    return static_cast<Image*>(imageImplPtr->BuildFromBitmap(bitmap));
}

Image* Image::BuildFromPicture(const Picture& picture, const SizeI& dimensions, const Matrix& matrix,
    const Brush& brush, BitDepth bitDepth, std::shared_ptr<ColorSpace> colorSpace)
{
    return static_cast<Image*>(
        imageImplPtr->BuildFromPicture(picture, dimensions, matrix, brush, bitDepth, colorSpace));
}

int Image::GetWidth()
{
    width_ = imageImplPtr->GetWidth();
    return width_;
}

int Image::GetHeight()
{
    height_ = imageImplPtr->GetHeight();
    return height_;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
