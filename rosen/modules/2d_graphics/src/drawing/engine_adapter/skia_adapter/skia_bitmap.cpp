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

#include "skia_bitmap.h"
#include "skia_pixmap.h"

#include "include/core/SkImageInfo.h"

#include "skia_image_info.h"

#include "image/bitmap.h"

#include "src/core/SkAutoMalloc.h"
#include "src/core/SkReadBuffer.h"
#include "src/core/SkWriteBuffer.h"
#include "utils/data.h"
namespace OHOS {
namespace Rosen {
namespace Drawing {
SkiaBitmap::SkiaBitmap() : skiaBitmap_() {}

static inline SkImageInfo MakeSkImageInfo(const int width, const int height, const BitmapFormat& format)
{
    auto imageInfo = SkImageInfo::Make(width, height,
                                       SkiaImageInfo::ConvertToSkColorType(format.colorType),
                                       SkiaImageInfo::ConvertToSkAlphaType(format.alphaType));
    return imageInfo;
}

void SkiaBitmap::Build(int32_t width, int32_t height, const BitmapFormat& format, int32_t stride)
{
    auto imageInfo = MakeSkImageInfo(width, height, format);
    skiaBitmap_.setInfo(imageInfo, stride);
    skiaBitmap_.allocPixels();
}

void SkiaBitmap::Build(const ImageInfo& imageInfo, int32_t stride)
{
    auto skImageInfo = SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    skiaBitmap_.setInfo(skImageInfo, stride);
    skiaBitmap_.allocPixels();
}

int SkiaBitmap::GetWidth() const
{
    return skiaBitmap_.width();
}

int SkiaBitmap::GetHeight() const
{
    return skiaBitmap_.height();
}

int SkiaBitmap::GetRowBytes() const
{
    return skiaBitmap_.rowBytes();
}

ColorType SkiaBitmap::GetColorType() const
{
    return SkiaImageInfo::ConvertToColorType(skiaBitmap_.colorType());
}

AlphaType SkiaBitmap::GetAlphaType() const
{
    return SkiaImageInfo::ConvertToAlphaType(skiaBitmap_.alphaType());
}

bool SkiaBitmap::ExtractSubset(Bitmap& dst, const Rect& subset) const
{
    const SkBitmap& subBitmap = dst.GetImpl<SkiaBitmap>()->ExportSkiaBitmap();
    SkIRect subRect = SkIRect::MakeLTRB(subset.GetLeft(), subset.GetTop(), subset.GetRight(), subset.GetBottom());
    return skiaBitmap_.extractSubset(const_cast<SkBitmap*>(&subBitmap), subRect);
}

bool SkiaBitmap::ReadPixels(const ImageInfo& dstInfo, void* dstPixels, size_t dstRowBytes,
                            int32_t srcX, int32_t srcY) const
{
    SkImageInfo skImageInfo = SkiaImageInfo::ConvertToSkImageInfo(dstInfo);
    return skiaBitmap_.readPixels(skImageInfo, dstPixels, dstRowBytes, srcX, srcY);
}

void* SkiaBitmap::GetPixels() const
{
    return skiaBitmap_.getPixels();
}

void SkiaBitmap::SetPixels(void* pixels)
{
    skiaBitmap_.setPixels(pixels);
}

bool SkiaBitmap::InstallPixels(const ImageInfo& info, void* pixels, size_t rowBytes,
                               ReleaseProc releaseProc, void* context)
{
    SkImageInfo skImageInfo = SkiaImageInfo::ConvertToSkImageInfo(info);
    return skiaBitmap_.installPixels(skImageInfo, pixels, rowBytes, releaseProc, context);
}

bool SkiaBitmap::PeekPixels(Pixmap& pixmap) const
{
    const SkPixmap& skiaPixmap = pixmap.GetImpl<SkiaPixmap>()->ExportSkiaPixmap();
    return skiaBitmap_.peekPixels(const_cast<SkPixmap*>(&skiaPixmap));
}

size_t SkiaBitmap::ComputeByteSize() const
{
    return skiaBitmap_.computeByteSize();
}

const SkBitmap& SkiaBitmap::ExportSkiaBitmap() const
{
    return skiaBitmap_;
}

void SkiaBitmap::CopyPixels(Bitmap& dst, int srcLeft, int srcTop) const
{
    ImageInfo imageInfo = dst.GetImageInfo();
    void* dstPixels = dst.GetPixels();

    SkImageInfo skImageInfo = SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int srcX = srcLeft;
    int srcY = srcTop;

    skiaBitmap_.readPixels(skImageInfo, dstPixels, dst.GetRowBytes(), srcX, srcY);
}

bool SkiaBitmap::IsImmutable()
{
    return skiaBitmap_.isImmutable();
}

void SkiaBitmap::SetImmutable()
{
    skiaBitmap_.setImmutable();
}

void SkiaBitmap::ClearWithColor(const ColorQuad& color) const
{
    SkColor skColor = static_cast<SkColor>(color);
    skiaBitmap_.eraseColor(skColor);
}

ColorQuad SkiaBitmap::GetColor(int x, int y) const
{
    SkColor color;
    color = skiaBitmap_.getColor(x, y);
    return static_cast<ColorQuad>(color);
}

void SkiaBitmap::Free()
{
    skiaBitmap_.reset();
}

bool SkiaBitmap::IsValid() const
{
    return skiaBitmap_.drawsNothing();
}

bool SkiaBitmap::IsEmpty() const
{
    return skiaBitmap_.empty();
}

Pixmap SkiaBitmap::GetPixmap() const
{
    SkPixmap skPixmap = skiaBitmap_.pixmap();
    Pixmap pixmap;
    pixmap.GetImpl<SkiaPixmap>()->ImportSkiaPixmap(skPixmap);
    return pixmap;
}

void SkiaBitmap::SetInfo(const ImageInfo& info)
{
    SkImageInfo skImageInfo = SkiaImageInfo::ConvertToSkImageInfo(info);
    skiaBitmap_.setInfo(skImageInfo);
}

bool SkiaBitmap::TryAllocPixels(const ImageInfo& info)
{
    SkImageInfo skImageInfo = SkiaImageInfo::ConvertToSkImageInfo(info);
    return skiaBitmap_.tryAllocPixels(skImageInfo);
}

void SkiaBitmap::SetSkBitmap(const SkBitmap& skBitmap)
{
    skiaBitmap_ = skBitmap;
}

std::shared_ptr<Data> SkiaBitmap::Serialize() const
{
#ifdef ROSEN_OHOS
    SkBinaryWriteBuffer writer;
    size_t rb = skiaBitmap_.rowBytes();
    int width = skiaBitmap_.width();
    int height = skiaBitmap_.height();
    const void *addr = skiaBitmap_.pixmap().addr();
    size_t pixmapSize = rb * static_cast<size_t>(height);

    writer.writeUInt(pixmapSize);
    if (addr == nullptr) {
        return nullptr;
    }
    writer.writeByteArray(addr, pixmapSize);

    writer.writeUInt(rb);
    writer.writeInt(width);
    writer.writeInt(height);

    writer.writeUInt(skiaBitmap_.colorType());
    writer.writeUInt(skiaBitmap_.alphaType());

    if (skiaBitmap_.colorSpace() == nullptr) {
        writer.writeUInt(0);
    } else {
        auto data = skiaBitmap_.colorSpace()->serialize();
        writer.writeUInt(data->size());
        writer.writeByteArray(data->data(), data->size());
    }
    size_t length = writer.bytesWritten();
    std::shared_ptr<Data> data = std::make_shared<Data>();
    data->BuildUninitialized(length);
    writer.writeToMemory(data->WritableData());
    return data;
#else
    return nullptr;
#endif
}

bool SkiaBitmap::Deserialize(std::shared_ptr<Data> data)
{
    if (data == nullptr) {
        return false;
    }
    SkReadBuffer reader(data->GetData(), data->GetSize());

    size_t pixmapSize = reader.readUInt();
    if (pixmapSize == 0) {
        return false;
    }
    SkAutoMalloc pixBuffer(pixmapSize);
    if (!reader.readByteArray(pixBuffer.get(), pixmapSize)) {
        return false;
    }

    size_t rb = reader.readUInt();
    int width = reader.readInt();
    int height = reader.readInt();

    SkColorType colorType = static_cast<SkColorType>(reader.readUInt());
    SkAlphaType alphaType = static_cast<SkAlphaType>(reader.readUInt());
    sk_sp<SkColorSpace> colorSpace;

    size_t size = reader.readUInt();
    if (size == 0) {
        colorSpace = nullptr;
    } else {
        SkAutoMalloc colorBuffer(size);
        if (!reader.readByteArray(colorBuffer.get(), size)) {
            return false;
        }
        colorSpace = SkColorSpace::Deserialize(colorBuffer.get(), size);
    }

    SkImageInfo imageInfo = SkImageInfo::Make(width, height, colorType, alphaType, colorSpace);
    skiaBitmap_.setInfo(imageInfo, rb);
    skiaBitmap_.allocPixels();
    skiaBitmap_.setPixels(const_cast<void*>(pixBuffer.get()));
    return true;
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
