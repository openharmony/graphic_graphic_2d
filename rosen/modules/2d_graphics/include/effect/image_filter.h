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

#ifndef IMAGE_FILTER_H
#define IMAGE_FILTER_H

#include "effect/color_filter.h"
#include "drawing/engine_adapter/impl_interface/image_filter_impl.h"
#include "utils/drawing_macros.h"
#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class ImageFilterImpl;
class DRAWING_API ImageFilter {
public:
    enum class FilterType {
        NO_TYPE,
        BLUR,
        COLOR_FILTER,
        OFFSET,
        ARITHMETIC,
        COMPOSE,
        GRADIENT_BLUR,
    };
    /**
     * @brief Create a filter that blurs its input by the separate X and Y sinma value.
     * @param sigmaX  The Gaussian sigma value for blurring along the X axis.
     * @param sigmaY  The Gaussian sigma value for blurring along the Y axis.
     * @param mode    The tile mode applied at edges.
     * @param input   The input filter that is blurred, uses source bitmap if this is null.
     * @return        A shared pointer to ImageFilter that its type is blur.
     */
    static std::shared_ptr<ImageFilter> CreateBlurImageFilter(scalar sigmaX, scalar sigmaY, TileMode mode,
        std::shared_ptr<ImageFilter> input, ImageBlurType blurType = ImageBlurType::GAUSS);
    /**
     * @brief Create a filter that applies the color filter to the input filter results.
     * @param cf     The color filter that transforms the input image.
     * @param input  The input filter, or uses the source bitmap if this is null.
     * @return       A shared pointer to ImageFilter that its type is color.
     */
    static std::shared_ptr<ImageFilter> CreateColorFilterImageFilter(const ColorFilter& cf,
        std::shared_ptr<ImageFilter> input);

    static std::shared_ptr<ImageFilter> CreateColorBlurImageFilter(const ColorFilter& cf, scalar sigmaX, scalar sigmaY,
        ImageBlurType blurType = ImageBlurType::GAUSS);
    /*
     * @brief        Create a filter that offsets the input filter by the given vector.
     * @param dx     The x offset in local space that the image is shifted.
     * @param dy     The y offset in local space that the image is shifted.
     * @param input  The input that will be moved, if null the source bitmap is used instead.
     * @return       A shared pointer to ImageFilter that its type is offset.
     */
    static std::shared_ptr<ImageFilter> CreateOffsetImageFilter(scalar dx, scalar dy,
        std::shared_ptr<ImageFilter> input);
    /**
     * @brief Create a filter that implements a custom blend mode.
     * @param coefficients    Get the four coefficients used to combine the foreground and background in the vector.
                              And The vector size must be four, otherwise the call fails.
     * @param enforcePMColor  If true, the RGB channels will be clamped to the Calculated alpha.
     * @param background      The Background content, using the source bitmap when this is null.
     * @param foreground      The foreground content, using the source bitmap when this is null.
     * @return                A shared point to ImageFilter that its type is arithmetic.
     */
    static std::shared_ptr<ImageFilter> CreateArithmeticImageFilter(const std::vector<scalar>& coefficients,
        bool enforcePMColor, std::shared_ptr<ImageFilter> background, std::shared_ptr<ImageFilter> foreground);
    /**
     * @brief Create a filter that composes f1 with f2.
     * @param f1  The outer filter that evaluates the results of inner.
     * @param f2  The inner filter that produces the input to outer.
     * @return    A shared pointer to ImageFilter that its type is compose.
     */
    static std::shared_ptr<ImageFilter> CreateComposeImageFilter(std::shared_ptr<ImageFilter> f1,
        std::shared_ptr<ImageFilter> f2);

    static std::shared_ptr<ImageFilter> CreateGradientBlurImageFilter(float radius,
        const std::vector<std::pair<float, float>>& fractionStops, GradientDir direction,
        GradientBlurType blurType, std::shared_ptr<ImageFilter> input);

    virtual ~ImageFilter() = default;
    FilterType GetType() const;
    virtual DrawingType GetDrawingType() const
    {
        return DrawingType::COMMON;
    }
    std::shared_ptr<Data> Serialize() const;
    bool Deserialize(std::shared_ptr<Data> data);
    template<typename T>
    T* GetImpl() const
    {
        return impl_->DowncastingTo<T>();
    }

    ImageFilter(FilterType t, scalar x, scalar y, std::shared_ptr<ImageFilter> input) noexcept;
    ImageFilter(FilterType t, scalar x, scalar y, TileMode mode, std::shared_ptr<ImageFilter> input,
        ImageBlurType blurType) noexcept;
    ImageFilter(FilterType t, const ColorFilter& cf, std::shared_ptr<ImageFilter> input) noexcept;
    ImageFilter(FilterType t, const ColorFilter& cf, scalar x, scalar y, ImageBlurType blurType) noexcept;
    ImageFilter(FilterType t, const std::vector<scalar>& coefficients, bool enforcePMColor,
        std::shared_ptr<ImageFilter> background, std::shared_ptr<ImageFilter> foreground) noexcept;
    ImageFilter(FilterType t, std::shared_ptr<ImageFilter> f1, std::shared_ptr<ImageFilter> f2) noexcept;
    ImageFilter(FilterType t, float radius, const std::vector<std::pair<float, float>>& fractionStops,
        GradientDir direction, GradientBlurType blurType, std::shared_ptr<ImageFilter> input) noexcept;
    ImageFilter(FilterType t) noexcept;
    void InitWithColorBlur(const ColorFilter& cf, scalar x, scalar y, ImageBlurType blurType);
protected:
    ImageFilter() noexcept;

private:
    FilterType type_;
    std::shared_ptr<ImageFilterImpl> impl_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif