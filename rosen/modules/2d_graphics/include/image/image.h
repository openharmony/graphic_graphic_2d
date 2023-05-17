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

namespace OHOS {
namespace Rosen {
namespace Drawing {
enum class BitDepth {
    KU8,
    KF16,
};

enum class CompressedType {
    ETC1,
    ASTC,
};

enum class TextureOrigin {
    TOP_LEFT,
    BOTTOM_LEFT,
};

class TextureInfo {
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

private:
    int width_ = 0;
    int height_ = 0;
    bool isMipMapped_ = false;
    unsigned int target_ = 0;
    unsigned int id_ = 0;
    unsigned int format_ = 0;
};

class Image {
public:
    Image() noexcept;
    // constructor adopt a raw image ptr, using for ArkUI, should remove after enable multi-media image decode.
    explicit Image(void* rawImg) noexcept;
    virtual ~Image() {};
    Image* BuildFromBitmap(const Bitmap& bitmap);
    Image* BuildFromPicture(const Picture& picture, const SizeI& dimensions, const Matrix& matrix, const Brush& brush,
        BitDepth bitDepth, std::shared_ptr<ColorSpace> colorSpace);

#ifdef ACE_ENABLE_GPU
    /*
     * @brief             Create Image from Bitmap. Image is uploaded to GPU back-end using context.
     * @param gpuContext  GPU context.
     * @param bitmap      BitmapInfo, pixel address and row bytes.
     * @return            True if Image is created successed.
     */
    bool BuildFromBitmap(GPUContext& gpuContext, const Bitmap& bitmap);

    /*
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

    /*
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
        BitmapFormat bitmapFormat, const std::shared_ptr<ColorSpace>& colorSpace);
#endif

    /*
     * @brief  Gets the width of Image.
     */
    int GetWidth() const;

    /*
     * @brief  Gets the height of Image.
     */
    int GetHeight() const;

    /*
     * @brief  Gets the unique Id of Image.
     */
    uint32_t GetUniqueID() const;

    /*
     * @brief         Copies a Rect of pixels from Image to Bitmap.
     * @param bitmap  Destination Bitmap.
     * @param x       Column index whose absolute value is less than Image width.
     * @param y       Row index whose absolute value is less than Image height.
     * @return        True of pixels are copied to Bitmap.
     */
    bool ReadPixels(Bitmap& bitmap, int x, int y);

    /*
     * @brief   Returns true the contents of Image was created on or uploaded to GPU memory,
                and is available as a GPU texture.
     * @return  True if Image is a GPU texture.
     */
    bool IsTextureBacked() const;

    template<typename T>
    const std::shared_ptr<T> GetImpl() const
    {
        return imageImplPtr->DowncastingTo<T>();
    }

    // using for recording, should to remove after using shared memory
    std::shared_ptr<Data> Serialize() const;
    bool Deserialize(std::shared_ptr<Data> data);

private:
    std::shared_ptr<ImageImpl> imageImplPtr;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
