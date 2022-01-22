/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "engine_adapter/impl_interface/image_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
enum class BitDepth {
    KU8,
    KF16,
};

class Image {
public:
    Image() noexcept;
    virtual ~Image() {};
    Image* BuildFromBitmap(const Bitmap& bitmap);
    Image* BuildFromPicture(const Picture& picture, const SizeI& dimensions, const Matrix& matrix, const Brush& brush,
        BitDepth bitDepth, std::shared_ptr<ColorSpace> colorSpace);
    int GetWidth();
    int GetHeight();
    template<typename T>
    const std::shared_ptr<T> GetImpl() const
    {
        return imageImplPtr->DowncastingTo<T>();
    }

private:
    std::shared_ptr<ImageImpl> imageImplPtr;
    int width_;
    int height_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
