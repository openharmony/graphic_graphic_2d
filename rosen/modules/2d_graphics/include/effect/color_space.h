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

#ifndef COLOR_SPACE_H
#define COLOR_SPACE_H

#include <string>

#include "engine_adapter/impl_interface/color_space_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class Image;
class ColorSpace {
public:
    enum class ColorSpaceType {
        NO_TYPE,
        SRGB,
        SRGB_LINEAR,
        REF_IMAGE,
    };

    static std::shared_ptr<ColorSpace> CreateSRGB();
    static std::shared_ptr<ColorSpace> CreateSRGBLinear();
    static std::shared_ptr<ColorSpace> CreateRefImage(const Image& image);

    ~ColorSpace() {}
    ColorSpaceType GetType() const;
    template<typename T>
    const std::shared_ptr<T> GetImpl() const
    {
        return impl_->DowncastingTo<T>();
    }

    ColorSpace(ColorSpaceType t) noexcept;
    ColorSpace(ColorSpaceType t, const Image& image) noexcept;

protected:
    ColorSpace() noexcept;

private:
    ColorSpaceType type_;
    std::shared_ptr<ColorSpaceImpl> impl_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif