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

#ifndef IMAGEIMPL_H
#define IMAGEIMPL_H

#include "base_impl.h"

#include "draw/brush.h"
#include "effect/color_space.h"
#include "image/bitmap.h"
#include "image/pixmap.h"
#include "image/picture.h"
#include "image/image_info.h"
#include "utils/matrix.h"
#include "utils/size.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class Data;
#ifdef ACE_ENABLE_GPU
class GPUContext;
enum class CompressedType;
class BackendTexture;
class TextureInfo;
enum class TextureOrigin;
#ifdef RS_ENABLE_VK
struct VKTextureInfo;
#endif
#endif
enum class BitDepth;
class Surface;

/** Caller data passed to RasterReleaseProc; may be nullptr.
*/
typedef void* ReleaseContext;

/** Function called when SkImage no longer shares pixels. ReleaseContext is
    provided by caller when SkImage is created, and may be nullptr.
*/
typedef void (*RasterReleaseProc)(const void* pixels, ReleaseContext);

class ImageImpl : public BaseImpl {
public:
    ImageImpl() noexcept {}
    ~ImageImpl() override {}

    virtual bool BuildFromBitmap(const Bitmap& bitmap) = 0;
#ifdef ACE_ENABLE_GPU
    virtual bool BuildFromBitmap(GPUContext& gpuContext, const Bitmap& bitmap) = 0;
    virtual bool MakeFromEncoded(const std::shared_ptr<Data>& data) = 0;
    virtual bool BuildSubset(const std::shared_ptr<Image> image, const RectI& rect, GPUContext& gpuContext);
    virtual bool BuildFromCompressed(GPUContext& gpuContext, const std::shared_ptr<Data>& data, int width, int height,
        CompressedType type) = 0;
    virtual bool BuildFromTexture(GPUContext& gpuContext, const TextureInfo& info, TextureOrigin origin,
        BitmapFormat bitmapFormat, const std::shared_ptr<ColorSpace>& colorSpace,
        void (*deleteFunc)(void*) = nullptr, void* cleanupHelper = nullptr) = 0;
    virtual bool BuildFromSurface(GPUContext& gpuContext, Surface& surface, TextureOrigin origin,
        BitmapFormat bitmapFormat, const std::shared_ptr<ColorSpace>& colorSpace) = 0;
    virtual BackendTexture GetBackendTexture(bool flushPendingGrContextIO, TextureOrigin* origin) = 0;
    virtual bool IsValid(GPUContext* context) const = 0;
#endif
    virtual bool AsLegacyBitmap(Bitmap& bitmap) const = 0;
    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;
    virtual ColorType GetColorType() const = 0;
    virtual AlphaType GetAlphaType() const = 0;
    virtual std::shared_ptr<ColorSpace> GetColorSpace() const = 0;
    virtual uint32_t GetUniqueID() const = 0;
    virtual ImageInfo GetImageInfo() = 0;
    virtual bool ReadPixels(Bitmap& bitmap, int x, int y) = 0;
    virtual bool ReadPixels(Pixmap& pixmap, int x, int y) = 0;
    virtual bool ReadPixels(const ImageInfo& dstInfo, void* dstPixels, size_t dstRowBytes,
                            int32_t srcX, int32_t srcY) const = 0;
    virtual bool IsTextureBacked() const = 0;
    virtual bool ScalePixels(const Bitmap& bitmap, const SamplingOptions& sampling,
        bool allowCachingHint = true) const = 0;
    virtual std::shared_ptr<Data> EncodeToData(EncodedImageFormat encodedImageFormat, int quality) const = 0;
    virtual bool IsLazyGenerated() const = 0;
    virtual bool GetROPixels(Bitmap& bitmap) const = 0;
    virtual std::shared_ptr<Image> MakeRasterImage() const = 0;
    virtual bool CanPeekPixels() const = 0;
    virtual bool IsOpaque() const = 0;
    virtual void HintCacheGpuResource() const = 0;

    // using for recording, should to remove after using shared memory
    virtual std::shared_ptr<Data> Serialize() const = 0;
    virtual bool Deserialize(std::shared_ptr<Data> data) = 0;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
