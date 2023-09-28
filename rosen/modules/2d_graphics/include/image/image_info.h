/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef IMAGE_INFO_H
#define IMAGE_INFO_H
#include "draw/color.h"
#include "effect/color_space.h"
namespace OHOS {
namespace Rosen {
namespace Drawing {

enum class EncodedImageFormat {
    JPEG,
    PNG,
    WEBP,
    UNKNOWN,
};

class ImageInfo {
public:
    ImageInfo() = default;
    ImageInfo(int width, int height, ColorType colorType, AlphaType alphaType, std::shared_ptr<ColorSpace> colorSpace = nullptr)
        : width_(width), height_(height), colorType_(colorType), alphaType_(alphaType), colorSpace_(colorSpace) {}
    ~ImageInfo() = default;

    /*
     * @brief  Gets the width value of ImageInfo.
     */
    int GetWidth() const
    {
        return width_;
    }

    /*
     * @brief  Gets the height value of ImageInfo.
     */
    int GetHeight() const
    {
        return height_;
    }

    /*
     * @brief  Gets the color type value of ImageInfo.
     */
    ColorType GetColorType() const
    {
        return colorType_;
    }

    /*
     * @brief  Gets the alpha type value of ImageInfo.
     */
    AlphaType GetAlphaType() const
    {
        return alphaType_;
    }

    /*
     * @brief  Gets the color space value of ImageInfo.
     */
    std::shared_ptr<ColorSpace> GetColorSpace() const
    {
        return colorSpace_;
    }

    /*
     * @brief  Sets the width value of ImageInfo.
     */
    void SetWidth(int width)
    {
        width_ = width;
    }

    /*
     * @brief  Sets the height value of ImageInfo.
     */
    void SetHeight(int height)
    {
        height_ = height;
    }

    /*
     * @brief  Sets the color type value of ImageInfo.
     */
    void SetColorType(ColorType colorType)
    {
        colorType_ = colorType;
    }

    /*
     * @brief  Sets the alpha type value of ImageInfo.
     */
    void SetAlphaType(AlphaType alphaType)
    {
        alphaType_ = alphaType;
    }

    /*
     * @brief  Sets the color space value of ImageInfo.
     */
    void SetColorSpace(std::shared_ptr<ColorSpace> colorSpace)
    {
        colorSpace_ = colorSpace;
    }

private:
    int width_ = 0;
    int height_ =0;
    ColorType colorType_ = COLORTYPE_UNKNOWN;
    AlphaType alphaType_ = ALPHATYPE_UNKNOWN;
    std::shared_ptr<ColorSpace> colorSpace_ = nullptr;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif // IMAGE_INFO_H
