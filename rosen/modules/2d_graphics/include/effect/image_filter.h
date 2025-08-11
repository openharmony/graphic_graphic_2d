/*
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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
#include "utils/sampling_options.h"
#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class ImageFilterImpl;

static const Rect noCropRect = {
    -std::numeric_limits<scalar>::infinity(), -std::numeric_limits<scalar>::infinity(),
    std::numeric_limits<scalar>::infinity(), std::numeric_limits<scalar>::infinity()
};

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
        BLEND,
        SHADER,
        IMAGE,
        HD_SAMPLE, // high definition sample
        LAZY_IMAGE_FILTER
    };
    /**
     * @brief Create a filter that blurs its input by the separate X and Y sinma value.
     * @param sigmaX     The Gaussian sigma value for blurring along the X axis.
     * @param sigmaY     The Gaussian sigma value for blurring along the Y axis.
     * @param mode       The tile mode applied at edges.
     * @param input      The input filter that is blurred, uses source bitmap if this is null.
     * @param blurType   The BlurType of Image, default as GAUSS.
     * @param cropRect   Optional rectangle that crops the input and output
     * @return           A shared pointer to ImageFilter that its type is blur.
     */
    static std::shared_ptr<ImageFilter> CreateBlurImageFilter(scalar sigmaX, scalar sigmaY, TileMode mode,
        std::shared_ptr<ImageFilter> input, ImageBlurType blurType = ImageBlurType::GAUSS,
        const Rect& cropRect = noCropRect);
    /**
     * @brief Create a filter that applies the color filter to the input filter results.
     * @param cf     The color filter that transforms the input image.
     * @param input  The input filter, or uses the source bitmap if this is null.
     * @param cropRect   Optional rectangle that crops the input and output
     * @return       A shared pointer to ImageFilter that its type is color.
     */
    static std::shared_ptr<ImageFilter> CreateColorFilterImageFilter(const ColorFilter& cf,
        std::shared_ptr<ImageFilter> input, const Rect& cropRect = noCropRect);

    static std::shared_ptr<ImageFilter> CreateColorBlurImageFilter(const ColorFilter& cf, scalar sigmaX, scalar sigmaY,
        ImageBlurType blurType = ImageBlurType::GAUSS, const Rect& cropRect = noCropRect);
    /*
     * @brief        Create a filter that offsets the input filter by the given vector.
     * @param dx     The x offset in local space that the image is shifted.
     * @param dy     The y offset in local space that the image is shifted.
     * @param input  The input that will be moved, if null the source bitmap is used instead.
     * @param cropRect   Optional rectangle that crops the input and output
     * @return       A shared pointer to ImageFilter that its type is offset.
     */
    static std::shared_ptr<ImageFilter> CreateOffsetImageFilter(scalar dx, scalar dy,
        std::shared_ptr<ImageFilter> input, const Rect& cropRect = noCropRect);
    /**
     * @brief Create a filter that implements a custom blend mode.
     * @param coefficients    Get the four coefficients used to combine the foreground and background in the vector.
                              And The vector size must be four, otherwise the call fails.
     * @param enforcePMColor  If true, the RGB channels will be clamped to the Calculated alpha.
     * @param background      The Background content, using the source bitmap when this is null.
     * @param foreground      The foreground content, using the source bitmap when this is null.
     * @param cropRect   Optional rectangle that crops the input and output
     * @return                A shared point to ImageFilter that its type is arithmetic.
     */
    static std::shared_ptr<ImageFilter> CreateArithmeticImageFilter(const std::vector<scalar>& coefficients,
        bool enforcePMColor, std::shared_ptr<ImageFilter> background, std::shared_ptr<ImageFilter> foreground,
        const Rect& cropRect = noCropRect);
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
    
    /**
     * @brief This filter takes an BlendMode and uses it to composite the two filters together
     * @param mode  The blend mode that defines the compositing operation
     * @param background  The Dst pixels used in blending, if null the source bitmap is used.
     * @param foreground  The Src pixels used in blending, if null the source bitmap is used.
     * @param cropRect   Optional rectangle that crops the input and output
     * @return    A shared pointer to ImageFilter that its type is blend.
     */
    static std::shared_ptr<ImageFilter> CreateBlendImageFilter(BlendMode mode,
        std::shared_ptr<ImageFilter> background, std::shared_ptr<ImageFilter> foreground = nullptr,
        const Rect& cropRect = noCropRect);

    /**
     * @brief Create a filter that fills the output with the per-pixel evaluation of the ShaderEffect. The
     *        shader is defined in the image filter's local coordinate system, so will automatically
     *        be affected by Canvas's transform.
     *
     *        Like Image() and Picture(), this is a leaf filter that can be used to introduce inputs to
     *        a complex filter graph, but should generally be combined with a filter that as at least
     *        one null input to use the implicit source image.
     * @param shader  The shader that fills the result image
     * @param cropRect   Optional rectangle that crops the input and output
     * @return    A shared pointer to ImageFilter that its type is shader.
     */
    static std::shared_ptr<ImageFilter> CreateShaderImageFilter(std::shared_ptr<ShaderEffect> shader,
        const Rect& cropRect = noCropRect);

    /**
     * @brief Create a filter to render the contents of the input Image with rect.
     *
     * @param srcRect Indicates the pointer to a src rect object.
     * @param dstRect Indicates the pointer to a dst rect object.
     * @return    A shared pointer to ImageFilter that its type is bitmap.
     */
    static std::shared_ptr<ImageFilter> CreateImageImageFilter(
        const std::shared_ptr<Image>& image, const Rect& srcRect, const Rect& dstRect,
        const SamplingOptions& options = SamplingOptions());

     /**
     * @brief Create a filter that draws the 'srcRect' portion of image into 'dstRect' using HD Sampling.
     *
     * @param image The image that the filter will process.
     * @param src   The source pixels sampled from the image, subset of image rect.
     * @param dst   The local rectangle to draw the image into.
     * @return      A shared pointer to ImageFilter that its type is hd sample.
     */
    static std::shared_ptr<ImageFilter> CreateHDSampleImageFilter(
        const std::shared_ptr<Image>& image, const Rect& src, const Rect& dst, const HDSampleInfo& info);

    virtual ~ImageFilter() = default;
    FilterType GetType() const;
    virtual bool IsLazy() const { return false; }
    virtual DrawingType GetDrawingType() const
    {
        return DrawingType::COMMON;
    }
    virtual std::shared_ptr<Data> Serialize() const;
    virtual bool Deserialize(std::shared_ptr<Data> data);
#ifdef ROSEN_OHOS
    virtual bool Marshalling(Parcel& parcel);
    static std::shared_ptr<ImageFilter> Unmarshalling(Parcel& parcel, bool& isValid);
#endif
    template<typename T>
    T* GetImpl() const
    {
        return impl_->DowncastingTo<T>();
    }

    ImageFilter(FilterType t, scalar x, scalar y, std::shared_ptr<ImageFilter> input,
        const Rect& cropRect = noCropRect) noexcept;
    ImageFilter(FilterType t, scalar x, scalar y, TileMode mode, std::shared_ptr<ImageFilter> input,
        ImageBlurType blurType, const Rect& cropRect = noCropRect) noexcept;
    ImageFilter(FilterType t, const ColorFilter& cf,
        std::shared_ptr<ImageFilter> input, const Rect& cropRect = noCropRect) noexcept;
    ImageFilter(FilterType t, const ColorFilter& cf, scalar x, scalar y,
        ImageBlurType blurType, const Rect& cropRect = noCropRect) noexcept;
    ImageFilter(FilterType t, const std::vector<scalar>& coefficients, bool enforcePMColor,
        std::shared_ptr<ImageFilter> background, std::shared_ptr<ImageFilter> foreground,
        const Rect& cropRect = noCropRect) noexcept;
    ImageFilter(FilterType t, std::shared_ptr<ImageFilter> f1, std::shared_ptr<ImageFilter> f2) noexcept;
    ImageFilter(FilterType t, float radius, const std::vector<std::pair<float, float>>& fractionStops,
        GradientDir direction, GradientBlurType blurType, std::shared_ptr<ImageFilter> input) noexcept;
    ImageFilter(FilterType t) noexcept;
    void InitWithColorBlur(const ColorFilter& cf, scalar x, scalar y, ImageBlurType blurType,
        const Rect& cropRect = noCropRect);
    ImageFilter(FilterType t, BlendMode mode, std::shared_ptr<ImageFilter> background,
        std::shared_ptr<ImageFilter> foreground = nullptr,
        const Rect& cropRect = noCropRect) noexcept;
    ImageFilter(FilterType t, std::shared_ptr<ShaderEffect> shader, const Rect& cropRect = noCropRect) noexcept;
    ImageFilter(FilterType t, const std::shared_ptr<Image>& image, const RectF& srcRect,
        const RectF& dstRect, const SamplingOptions& options = SamplingOptions()) noexcept;
    ImageFilter(FilterType t, const std::shared_ptr<Image>& image,
        const Rect& src, const Rect& dst, const HDSampleInfo& info) noexcept;

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