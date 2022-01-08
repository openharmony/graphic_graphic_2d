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

#ifndef BITMAP_H
#define BITMAP_H

#include "engine_adapter/impl_interface/bitmap_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
struct BitmapFormat {
    ColorType colorType;
    AlphaType alphaType;
};

class Bitmap {
public:
    Bitmap();
    virtual ~Bitmap();
    void Build(const int width, const int height, const BitmapFormat& format);
    int GetWidth();
    int GetHeight();
    void* GetPixels();
    void SetPixels(void* pixel);
    void CopyPixels(Bitmap& dst, int srcLeft, int srcTop, int width, int height);
    void ClearWithColor(const ColorQuad& color);
    bool IsValid();
    ColorQuad GetColor(int x, int y);
    void Free();
    BitmapFormat GetFormat();
    template<typename T> const std::shared_ptr<T> GetImpl() const { return bmpImplPtr->DowncastingTo<T>(); }
private:
    std::shared_ptr<BitmapImpl> bmpImplPtr;
    void* pixels_;
    int width_;
    int height_;
    BitmapFormat format_;
};
}
}
}
#endif
