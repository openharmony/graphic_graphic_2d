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

#ifndef BITMAP_H
#define BITMAP_H

#include "drawing/engine_adapter/impl_interface/bitmap_impl.h"
#include "utils/drawing_macros.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
struct BitmapFormat {
    ColorType colorType;
    AlphaType alphaType;
};

class DRAWING_API Bitmap {
public:
    Bitmap();
    virtual ~Bitmap();
    void Build(int32_t width, int32_t height, const BitmapFormat& format, int32_t stride = 0);
    void Build(const ImageInfo& imageInfo, int32_t stride = 0);

    /*
     * @brief  Gets the width of Bitmap.
     */
    int GetWidth() const;

    /*
     * @brief  Gets the height of Bitmap.
     */
    int GetHeight() const;

    int GetRowBytes() const;
    ColorType GetColorType() const;
    AlphaType GetAlphaType() const;
    bool ExtractSubset(Bitmap& dst, const Rect& subset) const;
    bool ReadPixels(const ImageInfo& dstInfo, void* dstPixels, size_t dstRowBytes,
                    int32_t srcX, int32_t srcY) const;
    bool PeekPixels(Pixmap& pixmap) const;

    /*
     * @brief  Gets the pointer to Bitmap buffer.
     */
    void* GetPixels() const;
    void SetPixels(void* pixel);
    void CopyPixels(Bitmap& dst, int srcLeft, int srcTop) const;
    bool InstallPixels(const ImageInfo& info, void* pixels, size_t rowBytes,
                       ReleaseProc releaseProc = nullptr, void* context = nullptr);
    bool IsImmutable();
    void SetImmutable();
    void ClearWithColor(const ColorQuad& color) const;
    bool IsValid() const;
    bool IsEmpty() const;
    ColorQuad GetColor(int x, int y) const;
    void Free();
    BitmapFormat GetFormat() const;
    void SetFormat(const BitmapFormat& format);
    void SetInfo(const ImageInfo& info);
    ImageInfo GetImageInfo() const;
    Pixmap GetPixmap() const;
    bool TryAllocPixels(const ImageInfo& info);
    template<typename T>
    T* GetImpl() const
    {
        return bmpImplPtr->DowncastingTo<T>();
    }

    std::shared_ptr<Data> Serialize() const;
    bool Deserialize(std::shared_ptr<Data> data);

private:
    std::shared_ptr<BitmapImpl> bmpImplPtr;
    BitmapFormat format_;
    ImageInfo imageInfo_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
