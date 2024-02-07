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

#ifndef SKIABITMAP_H
#define SKIABITMAP_H

#include "include/core/SkBitmap.h"

#include "impl_interface/bitmap_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class DRAWING_API SkiaBitmap : public BitmapImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;

    SkiaBitmap();
    ~SkiaBitmap() override {}

    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }

    void Build(int32_t width, int32_t height, const BitmapFormat& format, int32_t stride) override;
    void Build(const ImageInfo& imageInfo, int32_t stride) override;
    int GetWidth() const override;
    int GetHeight() const override;
    int GetRowBytes() const override;
    ColorType GetColorType() const override;
    AlphaType GetAlphaType() const override;
    bool ExtractSubset(Bitmap& dst, const Rect& subset) const override;
    bool ReadPixels(const ImageInfo& dstInfo, void* dstPixels, size_t dstRowBytes,
                    int32_t srcX, int32_t srcY) const override;
    void* GetPixels() const override;
    void SetPixels(void* pixels) override;
    bool InstallPixels(const ImageInfo& info, void* pixels, size_t rowBytes,
                       ReleaseProc releaseProc, void* context) override;
    bool PeekPixels(Pixmap& pixmap) const override;
    size_t ComputeByteSize() const override;
    const SkBitmap& ExportSkiaBitmap() const;
    void CopyPixels(Bitmap& dst, int srcLeft, int srcTop) const override;
    bool IsImmutable() override;
    void SetImmutable() override;
    void ClearWithColor(const ColorQuad& color) const override;
    ColorQuad GetColor(int x, int y) const override;
    void Free() override;
    bool IsValid() const override;
    bool IsEmpty() const override;
    Pixmap GetPixmap() const override;
    void SetInfo(const ImageInfo& info) override;
    bool TryAllocPixels(const ImageInfo& info) override;
    void SetSkBitmap(const SkBitmap& skBitmap);

    std::shared_ptr<Data> Serialize() const override;
    bool Deserialize(std::shared_ptr<Data> data) override;

private:
    SkBitmap skiaBitmap_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
