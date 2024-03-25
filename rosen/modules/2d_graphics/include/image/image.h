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

#ifndef IMAGE_H
#define IMAGE_H

#include "drawing/engine_adapter/impl_interface/image_impl.h"
#include "image/gpu_context.h"
#include "include/core/SkImage.h"
#include "utils/drawing_macros.h"
#ifdef RS_ENABLE_VK
#include "vulkan/vulkan.h"
#endif

namespace OHOS {
namespace Rosen {
namespace Drawing {
enum class BitDepth {
    KU8,
    KF16,
};

enum class CompressedType {
    NoneType,
    ETC2_RGB8_UNORM, // the same ad ETC1
    BC1_RGB8_UNORM,
    BC1_RGBA8_UNORM,
    ASTC_RGBA8_4x4,
    ASTC_RGBA8_6x6,
    ASTC_RGBA8_8x8,
};

enum class TextureOrigin {
    TOP_LEFT,
    BOTTOM_LEFT,
};

class Surface;

#ifdef RS_ENABLE_VK
struct VKAlloc {
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkDeviceSize offset = 0;
    VkDeviceSize size = 0;
    uint32_t flags = 0;
    std::string statName = "";
};

struct VKYcbcrConversionInfo {
    VkFormat format = VK_FORMAT_UNDEFINED;
    uint64_t externalFormat = 0;
    VkSamplerYcbcrModelConversion ycbcrModel = VK_SAMPLER_YCBCR_MODEL_CONVERSION_RGB_IDENTITY;
    VkSamplerYcbcrRange ycbcrRange = VK_SAMPLER_YCBCR_RANGE_ITU_FULL;
    VkChromaLocation xChromaOffset = VK_CHROMA_LOCATION_COSITED_EVEN;
    VkChromaLocation yChromaOffset = VK_CHROMA_LOCATION_COSITED_EVEN;
    VkFilter chromaFilter = VK_FILTER_NEAREST;
    VkBool32 forceExplicitReconstruction = false;
    VkFormatFeatureFlags formatFeatures = 0;
};

struct VKTextureInfo {
    VkImage vkImage = VK_NULL_HANDLE;
    VKAlloc vkAlloc;
    VkImageTiling imageTiling = VK_IMAGE_TILING_OPTIMAL;
    VkImageLayout imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    VkFormat format = VK_FORMAT_UNDEFINED;
    VkImageUsageFlags imageUsageFlags = 0;
    uint32_t sampleCount = 1;
    uint32_t levelCount = 0;
    uint32_t currentQueueFamily = VK_QUEUE_FAMILY_IGNORED;
    bool vkProtected = false;
    VKYcbcrConversionInfo ycbcrConversionInfo;
    VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE;
};
#endif

class DRAWING_API TextureInfo {
public:
    /*
     * @brief  Sets the width value of Texture.
     */
    void SetWidth(int width)
    {
        width_ = width;
    }

    /*
     * @brief  Sets the height value of Texture.
     */
    void SetHeight(int height)
    {
        height_ = height;
    }

    /*
     * @brief  Used to say whether a texture has mip levels allocated or not.
     */
    void SetIsMipMapped(bool isMipMapped)
    {
        isMipMapped_ = isMipMapped;
    }

    /*
     * @brief         Sets the target type of texture to render.
     * @param target  The target type of texture.
     */
    void SetTarget(unsigned int target)
    {
        target_ = target;
    }

    /*
     * @brief     Sets the Id of texture to render.
     * @param id  Texture Id value.
     */
    void SetID(unsigned int id)
    {
        id_ = id;
    }

    /*
     * @brief         Set the format of texture.
     * @param format  The format of texture.
     */
    void SetFormat(unsigned int format)
    {
        format_ = format;
    }

    /*
     * @brief  Gets the width of TextureInfo.
     */
    int GetWidth() const
    {
        return width_;
    }

    /*
     * @brief  Gets the height of TextureInfo.
     */
    int GetHeight() const
    {
        return height_;
    }

    /*
     * @brief  Gets whether the texture has mip levels allocated.
     */
    bool GetIsMipMapped() const
    {
        return isMipMapped_;
    }

    /*
     * @brief   Gets the target type of TextureInfo.
     * @return  The target type of TextureInfo.
     */
    unsigned int GetTarget() const
    {
        return target_;
    }

    /*
     * @brief   Gets the Id of TextureInfo.
     * @return  The Id of TextureInfo.
     */
    unsigned int GetID() const
    {
        return id_;
    }

    /*
     * @brief   Gets the format of TextureInfo.
     * @return  The target format of TextureInfo.
     */
    unsigned int GetFormat() const
    {
        return format_;
    }

#ifdef RS_ENABLE_VK
    std::shared_ptr<VKTextureInfo> GetVKTextureInfo() const
    {
        return vkTextureInfo_;
    }
    void SetVKTextureInfo(std::shared_ptr<VKTextureInfo> vkTextureInfo)
    {
        vkTextureInfo_ = vkTextureInfo;
    }
#endif
private:
    int width_ = 0;
    int height_ = 0;
    bool isMipMapped_ = false;
    unsigned int target_ = 0;
    unsigned int id_ = 0;
    unsigned int format_ = 0;
#ifdef RS_ENABLE_VK
    std::shared_ptr<VKTextureInfo> vkTextureInfo_ = nullptr;
#endif
};

class DRAWING_API BackendTexture {
public:
    BackendTexture() noexcept;
    BackendTexture(bool isValid) noexcept;
    virtual ~BackendTexture() {};

    bool IsValid() const;
    void SetTextureInfo(const TextureInfo& textureInfo);
    const TextureInfo& GetTextureInfo() const;

private:
    bool isValid_;
    TextureInfo textureInfo_;
};

class DRAWING_API Image {
public:
    Image() noexcept;
    // constructor adopt a raw image ptr, using for ArkUI, should remove after enable multi-media image decode.
    explicit Image(void* rawImg) noexcept;
    explicit Image(std::shared_ptr<ImageImpl> imageImpl);
    virtual ~Image() {};
    bool BuildFromBitmap(const Bitmap& bitmap);

    /**
     * @brief                        Create Image from Pixmap.
     * @param  pixmap                pixmap.
     * @param  rasterReleaseProc     function called when pixels can be released; or nullptr.
     * @param  releaseContext        state passed to rasterReleaseProc; or nullptr.
     * @return                       Image sharing pixmap.
     */
    static std::shared_ptr<Image> MakeFromRaster(const Pixmap& pixmap,
        RasterReleaseProc rasterReleaseProc, ReleaseContext releaseContext);

    /**
     * @brief  Create Image from ImageInfo, sharing pixels.
     * @return A shared pointer to Image
     */
    static std::shared_ptr<Image> MakeRasterData(const ImageInfo& info, std::shared_ptr<Data> pixels,
                                                 size_t rowBytes);
#ifdef ACE_ENABLE_GPU
    /**
     * @brief             Create Image from Bitmap. Image is uploaded to GPU back-end using context.
     * @param gpuContext  GPU context.
     * @param bitmap      BitmapInfo, pixel address and row bytes.
     * @return            True if Image is created successed.
     */
    bool BuildFromBitmap(GPUContext& gpuContext, const Bitmap& bitmap);

    bool MakeFromEncoded(const std::shared_ptr<Data>& data);

    /**
     * @brief             Create a GPU-backed Image from compressed data.
     * @param gpuContext  GPU context.
     * @param data        Compressed data to store in Image.
     * @param width       Width of full Image.
     * @param height      Height of full Image.
     * @param type        Type of compression used.
     * @return            True if Image is created successed.
     */
    bool BuildFromCompressed(GPUContext& gpuContext, const std::shared_ptr<Data>& data, int width, int height,
        CompressedType type);

    /**
     * @brief               Create Image from GPU texture associated with context.
     * @param gpuContext    GPU context.
     * @param info          Texture info.
     * @param origin        The origin of the texture space corresponds to the context pixel.
                            One of TextureOrigin::Top_Left, TextureOrigion::Bottom_Left.
     * @param bitmapFormat  It contains ColorType and AlphaType.
     * @param colorSpace    Range of colors, may be nullptr.
     * @return              True if Image is created successed.
     */
    bool BuildFromTexture(GPUContext& gpuContext, const TextureInfo& info, TextureOrigin origin,
        BitmapFormat bitmapFormat, const std::shared_ptr<ColorSpace>& colorSpace,
        void (*deleteFunc)(void*) = nullptr, void* cleanupHelper = nullptr);

    /**
     * @brief GetBackendTexture from surface, then create Image from GPU texture associated with context.
     * @param gpuContext    GPU context
     * @param surface       surface
     * @param origin        The origin of the texture space corresponds to the context pixel.
                            One of TextureOrigin::Top_Left, TextureOrigion::Bottom_Left.
     * @param bitmapFormat  It contains ColorType and AlphaType.
     * @param colorSpace    Range of colors, may be nullptr.
     * @return              True if Image is created successed.
     */
    bool BuildFromSurface(GPUContext& gpuContext, Surface& surface, TextureOrigin origin,
        BitmapFormat bitmapFormat, const std::shared_ptr<ColorSpace>& colorSpace);

    /**
     * @brief Returns subset of this image.
     * image nullptr if any of the following are true:
     * - Subset is empty
     * - Subset is not contained inside the image's bounds
     * - Pixels in the image could not be read or copied
     * If this image is texture-backed, the context parameter is required and must match the
     * context of the source image. If the context parameter is provided, and the image is
     * raster-backed, the subset will be converted to texture-backed.
     * @param image       subset of this image
     * @param rect        bounds of subset
     * @param gpuContext  the GPUContext in play
     * @return            true if build success
     */
    bool BuildSubset(const std::shared_ptr<Image>& image, const RectI& rect, GPUContext& gpuContext);

    BackendTexture GetBackendTexture(bool flushPendingGrContextIO, TextureOrigin* origin) const;

    bool IsValid(GPUContext* context) const;

    bool pinAsTexture(GPUContext& context);
#endif

    /**
     * @brief  Creates raster Bitmap with same pixels as Image.
     */
    bool AsLegacyBitmap(Bitmap& bitmap) const;

    /**
     * @brief  Gets the width of Image.
     * @return width of Image
     */
    int GetWidth() const;

    /**
     * @brief  Gets the height of Image.
     * @return height of image
     */
    int GetHeight() const;

    /**
     * @brief  Gets the color type of Image.
     * @return color Type of Image
     */
    ColorType GetColorType() const;

    /**
     * @brief  Gets the alpha type of Image.
     * @return alpha type of Image
     */
    AlphaType GetAlphaType() const;

    /**
     * @brief  Gets the color space of Image.
     * @return a shared pointer to ColorSpace
     */
    std::shared_ptr<ColorSpace> GetColorSpace() const;

    /**
     * @brief  Gets the unique Id of Image.
     * @return the unique Id of Image
     */
    uint32_t GetUniqueID() const;

    /**
     * @brief  Gets the ImageInfo of Image.
     * @return the ImageInfo of Image
     */
    ImageInfo GetImageInfo();

    /**
     * @brief         Copies a Rect of pixels from Image to Bitmap.
     * @param bitmap  Destination Bitmap.
     * @param x       Column index whose absolute value is less than Image width.
     * @param y       Row index whose absolute value is less than Image height.
     * @return        True of pixels are copied to Bitmap.
     */
    bool ReadPixels(Bitmap& bitmap, int x, int y);

    /**
     * @brief         Copies a Rect of pixels from Image to Pixmap.
     * @param pixmap  Destination Pixmap.
     * @param x       Column index whose absolute value is less than Image width.
     * @param y       Row index whose absolute value is less than Image height.
     * @return        True of pixels are copied to Pixmap.
     */
    bool ReadPixels(Pixmap& pixmap, int x, int y);

    bool ReadPixels(const ImageInfo& dstInfo, void* dstPixels, size_t dstRowBytes,
                    int32_t srcX, int32_t srcY) const;

    bool ScalePixels(const Bitmap& bitmap, const SamplingOptions& sampling,
        bool allowCachingHint = true) const;

    std::shared_ptr<Data> EncodeToData(EncodedImageFormat encodedImageFormat, int quality) const;

    bool IsLazyGenerated() const;

    /**
     * @brief  Get Bitmap by image's directContext, can call it if IsLazyGenerated return false.
     */
    bool GetROPixels(Bitmap& bitmap) const;

    std::shared_ptr<Image> MakeRasterImage() const;

    bool CanPeekPixels() const;

    /**
     * @brief   Returns true the contents of Image was created on or uploaded to GPU memory,
                and is available as a GPU texture.
     * @return  True if Image is a GPU texture.
     */
    bool IsTextureBacked() const;

    bool IsOpaque() const;

    /*
     * @brief Tell engine try to cache gpu resource when texture resource create.
     */
    void HintCacheGpuResource() const;

    template<typename T>
    T* GetImpl() const
    {
        return imageImplPtr->DowncastingTo<T>();
    }

    // using for recording, should to remove after using shared memory
    std::shared_ptr<Data> Serialize() const;
    bool Deserialize(std::shared_ptr<Data> data);

    const sk_sp<SkImage> ExportSkImage();

private:
    std::shared_ptr<ImageImpl> imageImplPtr;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
