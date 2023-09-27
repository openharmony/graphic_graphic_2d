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
#include "utils/getter_setter.h"
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

    GETTER(GetWidth, int, width_)
    GETTER(GetHeight, int, height_)
    GETTER(GetColorType, ColorType, colorType_)
    GETTER(GetAlphaType, AlphaType, alphaType_)
    GETTER(GetColorSpace, std::shared_ptr<ColorSpace>, colorSpace_)

    SETTER(SetWidth, int, width_)
    SETTER(SetHeight, int, height_)
    SETTER(SetColorType, ColorType, colorType_)
    SETTER(SetAlphaType, AlphaType, alphaType_)
    SETTER(SetColorSpace, std::shared_ptr<ColorSpace>, colorSpace_)

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