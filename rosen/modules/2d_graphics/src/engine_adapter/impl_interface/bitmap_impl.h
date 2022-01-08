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

#ifndef BITMAPIMPL_H
#define BITMAPIMPL_H

#include "base_impl.h"
#include "draw/color.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class Bitmap;
struct BitmapFormat;
class BitmapImpl : public BaseImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::BASE_INTERFACE;
    BitmapImpl() {}
    virtual ~BitmapImpl() {}
    AdapterType GetType() const override { return AdapterType::BASE_INTERFACE; }
    virtual void Build(const int width, const int height, const BitmapFormat& format) = 0;
    virtual int GetWidth() = 0;
    virtual int GetHeight() = 0;
    virtual void* GetPixels() = 0;
    virtual void SetPixels(void* pixel) = 0;
    virtual void CopyPixels(Bitmap& dst, int srcLeft, int srcTop, int width, int height) = 0;
    virtual void ClearWithColor(const ColorQuad& color) = 0;
    virtual ColorQuad GetColor(int x, int y) = 0;
    virtual void Free() = 0;
    virtual bool IsValid() = 0;
};
}
}
}
#endif