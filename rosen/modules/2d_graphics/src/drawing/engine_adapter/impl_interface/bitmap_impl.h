/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef BITMAPIMPL_H
#define BITMAPIMPL_H

#include "base_impl.h"
#include "utils/data.h"
#include "draw/color.h"
#include "image/image_info.h"
#include "utils/rect.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class Bitmap;
struct BitmapFormat;
class BitmapImpl : public BaseImpl {
public:
    BitmapImpl() {}
    ~BitmapImpl() override {}

    virtual void Build(int32_t width, int32_t height, const BitmapFormat& format, int32_t stride = 0) = 0;
    virtual void Build(const ImageInfo& imageInfo, int32_t stride = 0) = 0;
    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;
    virtual int GetRowBytes() const = 0;
    virtual ColorType GetColorType() const = 0;
    virtual AlphaType GetAlphaType() const = 0;
    virtual void* GetPixels() const = 0;
    virtual void SetPixels(void* pixel) = 0;
    virtual void CopyPixels(Bitmap& dst, int srcLeft, int srcTop) const = 0;
    virtual bool IsImmutable() = 0;
    virtual void SetImmutable() = 0;
    virtual void ClearWithColor(const ColorQuad& color) const = 0;
    virtual ColorQuad GetColor(int x, int y) const = 0;
    virtual void Free() = 0;
    virtual bool IsValid() const = 0;
    virtual std::shared_ptr<Data> Serialize() const = 0;
    virtual bool Deserialize(std::shared_ptr<Data> data) = 0;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
