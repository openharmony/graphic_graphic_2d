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

private:
    std::shared_ptr<ImageImpl> imageImplPtr;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
