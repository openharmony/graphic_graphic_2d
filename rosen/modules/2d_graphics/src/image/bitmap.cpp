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

#include "image/bitmap.h"

#include "impl_factory.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
Bitmap::Bitmap()
    : bmpImplPtr(ImplFactory::CreateBitmapImpl()), pixels_(nullptr),
      width_(0), height_(0), format_({COLORTYPE_UNKNOWN, ALPHATYPE_UNKNOWN})
{
}

Bitmap::~Bitmap() {}

void Bitmap::Build(const int width, const int height, const BitmapFormat& format)
{
    bmpImplPtr->Build(width, height, format);
}

int Bitmap::GetWidth()
{
    width_ = bmpImplPtr->GetWidth();
    return width_;
}

int Bitmap::GetHeight()
{
    height_ = bmpImplPtr->GetHeight();
    return height_;
}

void Bitmap::SetPixels(void* pixel)
{
    pixels_ = pixel;
    bmpImplPtr->SetPixels(pixel);
}

void* Bitmap::GetPixels()
{
    pixels_ = bmpImplPtr->GetPixels();
    return pixels_;
}

void Bitmap::CopyPixels(Bitmap& dst, int srcLeft, int srcTop, int width, int height)
{
    bmpImplPtr->CopyPixels(dst, srcLeft, srcTop, width, height);
}

void Bitmap::ClearWithColor(const ColorQuad& color)
{
    bmpImplPtr->ClearWithColor(color);
}

bool Bitmap::IsValid()
{
    return bmpImplPtr->IsValid();
}

ColorQuad Bitmap::GetColor(int x, int y)
{
    return bmpImplPtr->GetColor(x, y);
}

void Bitmap::Free()
{
    bmpImplPtr->Free();
}

BitmapFormat Bitmap::GetFormat()
{
    return format_;
}
}
}
}
