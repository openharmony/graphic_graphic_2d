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

#ifndef COLOR_FILTER_H
#define COLOR_FILTER_H

#include "color_matrix.h"
#include "draw/blend_mode.h"
#include "draw/color.h"
#include "engine_adapter/impl_interface/color_filter_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class ColorFilterImpl;
struct ColorFilterData;
class ColorFilter {
public:
    enum class FilterType {
        NO_TYPE,
        BLEND_MODE,
        MATRIX,
        LINEAR_TO_SRGB_GAMMA,
        SRGB_GAMMA_TO_LINEAR,
        COMPOSE,
    };

    static std::shared_ptr<ColorFilter> CreateBlendModeColorFilter(ColorQuad c, BlendMode mode);
    static std::shared_ptr<ColorFilter> CreateComposeColorFilter(ColorFilter& f1, ColorFilter& f2);
    static std::shared_ptr<ColorFilter> CreateMatrixColorFilter(ColorMatrix& m);
    static std::shared_ptr<ColorFilter> CreateLinearToSrgbGamma();
    static std::shared_ptr<ColorFilter> CreateSrgbGammaToLinear();

    ~ColorFilter() {}
    FilterType GetType() const;
    void Compose(ColorFilter& filter);
    template<typename T> const std::shared_ptr<T> GetImpl() const { return impl_->DowncastingTo<T>(); }

    ColorFilter(FilterType t, ColorQuad c, BlendMode mode) noexcept;
    ColorFilter(FilterType t, ColorMatrix& m) noexcept;
    ColorFilter(FilterType t, ColorFilter& f1, ColorFilter& f2) noexcept;
    ColorFilter(FilterType t) noexcept;
protected:
    ColorFilter() noexcept;
private:
    FilterType type_;
    std::shared_ptr<ColorFilterImpl> impl_;
};
}
}
}
#endif