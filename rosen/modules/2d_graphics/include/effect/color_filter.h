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

#ifndef COLOR_FILTER_H
#define COLOR_FILTER_H

#include "color_matrix.h"

#include "draw/blend_mode.h"
#include "draw/color.h"
#include "drawing/engine_adapter/impl_interface/color_filter_impl.h"
#include "utils/drawing_macros.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class ColorFilterImpl;
struct ColorFilterData;
class DRAWING_API ColorFilter {
public:
    enum class FilterType {
        NO_TYPE,
        BLEND_MODE,
        MATRIX,
        LINEAR_TO_SRGB_GAMMA,
        SRGB_GAMMA_TO_LINEAR,
        COMPOSE,
        LUMA,
        OVER_DRAW,
    };

    // Color matrix is a 4x5 float type matrix.
    constexpr static int MATRIX_SIZE = 20;
    constexpr static int OVER_DRAW_COLOR_NUM = 6;

    static std::shared_ptr<ColorFilter> CreateBlendModeColorFilter(ColorQuad c, BlendMode mode);
    static std::shared_ptr<ColorFilter> CreateComposeColorFilter(ColorFilter& f1, ColorFilter& f2);
    static std::shared_ptr<ColorFilter> CreateComposeColorFilter(
        const float (&f1)[MATRIX_SIZE], const float (&f2)[MATRIX_SIZE]);
    static std::shared_ptr<ColorFilter> CreateMatrixColorFilter(const ColorMatrix& m);
    static std::shared_ptr<ColorFilter> CreateFloatColorFilter(const float (&f)[MATRIX_SIZE]);
    static std::shared_ptr<ColorFilter> CreateLinearToSrgbGamma();
    static std::shared_ptr<ColorFilter> CreateSrgbGammaToLinear();
    static std::shared_ptr<ColorFilter> CreateOverDrawColorFilter(const ColorQuad colors[OVER_DRAW_COLOR_NUM]);
    /*
     * @brief   Create a ColorFilter that its type is luma.
     * @return  A shared pointer to ColorFilter that its type is luma.
     */
    static std::shared_ptr<ColorFilter> CreateLumaColorFilter();

    virtual ~ColorFilter() = default;
    FilterType GetType() const;

    virtual DrawingType GetDrawingType() const
    {
        return DrawingType::COMMON;
    }

    /*
     * @brief         Combine with another ColorFilter, and set it to itself.
     * @param filter  Another ColorFilter to Combine.
     */
    virtual void Compose(const ColorFilter& filter);
    template<typename T>
    T* GetImpl() const
    {
        return impl_->DowncastingTo<T>();
    }

    ColorFilter(FilterType t, ColorQuad c, BlendMode mode) noexcept;
    ColorFilter(FilterType t, const ColorMatrix& m) noexcept;
    ColorFilter(FilterType t, const float f[MATRIX_SIZE]) noexcept;
    ColorFilter(FilterType t, ColorFilter& f1, ColorFilter& f2) noexcept;
    ColorFilter(FilterType t, const float f1[MATRIX_SIZE], const float f2[MATRIX_SIZE]) noexcept;
    ColorFilter(FilterType t) noexcept;
    ColorFilter(FilterType t, const ColorQuad colors[OVER_DRAW_COLOR_NUM]) noexcept;
    std::shared_ptr<Data> Serialize() const;
    bool Deserialize(std::shared_ptr<Data> data);
    bool AsAColorMatrix(scalar matrix[MATRIX_SIZE]) const;

    void InitWithCompose(const float f1[MATRIX_SIZE], const float f2[MATRIX_SIZE]);

protected:
    ColorFilter() noexcept;

private:
    FilterType type_;
    std::shared_ptr<ColorFilterImpl> impl_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
