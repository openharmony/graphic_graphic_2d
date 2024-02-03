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

#ifndef SKIAIMAGE_H
#define SKIAIMAGE_H

#include "include/core/SkBitmap.h"
#include "include/core/SkImage.h"
#include "include/core/SkPaint.h"
#include "include/core/SkPicture.h"
#ifdef ACE_ENABLE_GPU
#ifdef NEW_SKIA
#include "include/gpu/GrDirectContext.h"
#else
#include "include/gpu/GrContext.h"
#endif
#endif
#include "skia_bitmap.h"
#include "skia_color_space.h"
#include "skia_matrix.h"
#include "skia_paint.h"
#include "skia_picture.h"
#include "include/gpu/GrBackendSurface.h"

#include "impl_interface/image_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class DRAWING_API SkiaImage : public ImageImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;

    SkiaImage() noexcept;
    explicit SkiaImage(sk_sp<SkImage> skImg) noexcept;
    ~SkiaImage() override {}

    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }

    static std::shared_ptr<Image> MakeFromRaster(const Pixmap& pixmap,
        RasterReleaseProc rasterReleaseProc, ReleaseContext releaseContext);
    static std::shared_ptr<Image> MakeRasterData(const ImageInfo& info, std::shared_ptr<Data> pixels,
        size_t rowBytes);
    bool BuildFromBitmap(const Bitmap& bitmap) override;
#ifdef ACE_ENABLE_GPU
    bool BuildFromSurface(GPUContext& gpuContext, Surface& surface, TextureOrigin origin,
        BitmapFormat bitmapFormat, const std::shared_ptr<ColorSpace>& colorSpace) override;
    bool BuildFromBitmap(GPUContext& gpuContext, const Bitmap& bitmap) override;
    bool MakeFromEncoded(const std::shared_ptr<Data>& data) override;
    bool BuildSubset(const std::shared_ptr<Image> image, const RectI& rect, GPUContext& gpuContext) override;
    bool BuildFromCompressed(GPUContext& gpuContext, const std::shared_ptr<Data>& data, int width, int height,
        CompressedType type) override;
    bool BuildFromTexture(GPUContext& gpuContext, const TextureInfo& info, TextureOrigin origin,
        BitmapFormat bitmapFormat, const std::shared_ptr<ColorSpace>& colorSpace,
        void (*deleteFunc)(void*) = nullptr, void* cleanupHelper = nullptr) override;
    BackendTexture GetBackendTexture(bool flushPendingGrContextIO, TextureOrigin* origin) override;
    void SetGrBackendTexture(const GrBackendTexture& grBackendTexture);
    bool IsValid(GPUContext* context) const override;
#endif
    bool AsLegacyBitmap(Bitmap& bitmap) const override;
    int GetWidth() const override;
    int GetHeight() const override;
    ColorType GetColorType() const override;
    AlphaType GetAlphaType() const override;
    std::shared_ptr<ColorSpace> GetColorSpace() const override;
    uint32_t GetUniqueID() const override;
    ImageInfo GetImageInfo() override;
    bool ReadPixels(Bitmap& bitmap, int x, int y) override;
    bool ReadPixels(Pixmap& pixmap, int x, int y) override;
    bool ReadPixels(const ImageInfo& dstInfo, void* dstPixels, size_t dstRowBytes,
                    int32_t srcX, int32_t srcY) const override;
    bool IsTextureBacked() const override;

    bool ScalePixels(const Bitmap& bitmap, const SamplingOptions& sampling,
        bool allowCachingHint = true) const override;
    std::shared_ptr<Data> EncodeToData(EncodedImageFormat encodedImageFormat, int quality) const override;
    bool IsLazyGenerated() const override;
    bool GetROPixels(Bitmap& bitmap) const override;
    std::shared_ptr<Image> MakeRasterImage() const override;
    bool CanPeekPixels() const override;

    bool IsOpaque() const override;
    void HintCacheGpuResource() const override;

    const sk_sp<SkImage> GetImage() const;

    /*
     * @brief  Update the member variable to skImage, adaptation layer calls.
     */
    void SetSkImage(const sk_sp<SkImage>& skImage);
#ifdef ACE_ENABLE_GPU
    /*
     * @brief  Export Skia member variables for use by the adaptation layer.
     */
#ifdef NEW_SKIA
    sk_sp<GrDirectContext> GetGrContext() const;
#else
    sk_sp<GrContext> GetGrContext() const;
#endif
#endif

    std::shared_ptr<Data> Serialize() const override;
    bool Deserialize(std::shared_ptr<Data> data) override;

private:
#ifdef ACE_ENABLE_GPU
#ifdef NEW_SKIA
    sk_sp<GrDirectContext> grContext_ = nullptr;
#else
    sk_sp<GrContext> grContext_ = nullptr;
#endif
#endif
    sk_sp<SkImage> skiaImage_;
    GrBackendTexture grBackendTexture_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
