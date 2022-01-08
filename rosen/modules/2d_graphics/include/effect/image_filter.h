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

#ifndef IMAGE_FILTER_H
#define IMAGE_FILTER_H

#include "effect/color_filter.h"
#include "engine_adapter/impl_interface/image_filter_impl.h"
#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class ImageFilterImpl;
struct ImageFilterData;
class ImageFilter {
public:
    enum class FilterType {
        NO_TYPE,
        BLUR,
        COLOR,
        OFFSET,
        ARITHMETIC,
        COMPOSE,
    };
    static std::shared_ptr<ImageFilter> CreateBlurImageFilter(scalar sigmaX, scalar sigmaY, ImageFilter& input);
    static std::shared_ptr<ImageFilter> CreateColorFilterImageFilter(ColorFilter& cf, ImageFilter& input);
    static std::shared_ptr<ImageFilter> CreateOffsetImageFilter(scalar dx, scalar dy, ImageFilter& input);
    static std::shared_ptr<ImageFilter> CreateArithmeticImageFilter(scalar k1, scalar k2, scalar k3, scalar k4, bool enforcePMColor,
                                                   ImageFilter& background, ImageFilter& foreground);
    static std::shared_ptr<ImageFilter> CreateComposeImageFilter(ImageFilter& f1, ImageFilter& f2);

    ~ImageFilter() {}
    FilterType GetType() const;
    template<typename T> const std::shared_ptr<T> GetImpl() const { return impl_->DowncastingTo<T>(); }

    ImageFilter(FilterType t, scalar x, scalar y, ImageFilter& input) noexcept;
    ImageFilter(FilterType t, ColorFilter& cf, ImageFilter& input) noexcept;
    ImageFilter(FilterType t, scalar k1, scalar k2, scalar k3, scalar k4, bool enforcePMColor,
                ImageFilter& background, ImageFilter& foreground) noexcept;
    ImageFilter(FilterType t, ImageFilter& f1, ImageFilter& f2) noexcept;
protected:
    ImageFilter() noexcept;
private:
    FilterType type_;
    std::shared_ptr<ImageFilterImpl> impl_;
};
}
}
}
#endif