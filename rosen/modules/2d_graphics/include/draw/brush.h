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

#ifndef BRUSH_H
#define BRUSH_H

#include "draw/color.h"
#include "effect/color_space.h"
#include "effect/blender.h"
#include "effect/blur_draw_looper.h"
#include "effect/filter.h"
#include "effect/shader_effect.h"
#include "utils/drawing_macros.h"
#include "utils/rect.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class DRAWING_API Brush {
public:
    Brush() noexcept;
    Brush(const Brush& b) noexcept = default;
    Brush(const Color& c) noexcept;
    Brush(const std::shared_ptr<ShaderEffect> e) noexcept;
    Brush(int rgba) noexcept;

    ~Brush() {}

    /**
     * @brief Retrieves alpha and RGB, unpremultiplied, packed into 32 bits.
     * @return unpremultiplied ARGB.
     */
    const Color& GetColor() const;

    /**
     * @brief Sets alpha and RGB used when stroking and filling. The color is a 32-bit value,
     *        unpremultiplied, packing 8-bit components for alpha, red, blue, and green.
     * @param color    unpremultiplied ARGB
     */
    void SetColor(const Color& c);

    /**
     * @brief Sets alpha and RGB used when stroking and filling. The color is a 32-bit value,
     *        unpremultiplied, packing 8-bit components for alpha, red, blue, and green.
     * @param c    unsigned 32-bit value as ARGB
     */
    void SetColor(uint32_t c);

    /**
     * @brief Sets color used when drawing solid fills. The color components range from 0 to 255.
     *        The color is unpremultiplied; alpha sets the transparency independent of RGB.
     * @param a    amount of alpha, from fully transparent (0) to fully opaque (255)
     * @param r    amount of red, from no red (0) to full red (255)
     * @param g    amount of green, from no green (0) to full green (255)
     * @param b    amount of blue, from no blue (0) to full blue (255)
     */
    void SetARGB(int a, int r, int g, int b);

    /**
     * @brief Retrieves alpha and RGB, unpremultiplied, as four floating point values. RGB are
     *        extended sRGB values (sRGB gamut, and encoded with the sRGB transfer function).
     * @return unpremultiplied RGBA
     */
    Color4f GetColor4f();

    /**
     * @brief Retrieves a shared pointer to color space of current Brush.
     * @return a shared pointer to color space of current Brush
     */
    std::shared_ptr<ColorSpace> GetColorSpace() const;

    /**
     * @brief Sets alpha and RGB used when stroking and filling. The color is four floating
     *        point values, unpremultiplied. The color values are interpreted as being in
     *        the s. If s is nullptr, then color is assumed to be in the sRGB color space.
     * @param color    unpremultiplied ARGB
     * @param s        ColorSpace describing the encoding of color
     */
    void SetColor(const Color4f& cf, std::shared_ptr<ColorSpace> s);

    /**
     * @brief Helper that scales the alpha by 255.
     * @return alpha scaled 255
     */
    inline uint32_t GetAlpha() const
    {
        return color_.GetAlpha();
    }

    /**
     * @brief Retrieves alpha from the color used when stroking and filling.
     * @return alpha ranging from zero, fully transparent, to 255, fully opaque
     */
    inline scalar GetAlphaF() const
    {
        return color_.GetAlphaF();
    }

    /**
     * @brief Helper that accepts an int between 0 and 255, and divides it by 255.0.
     * @param a    An int between 0 and 255
     */
    void SetAlpha(uint32_t a);

    /**
     * @brief Replaces alpha, leaving RGB unchanged.
     *        An out of range value triggers an assert in the debug build.
     *        a is a value from 0.0 to 1.0. a set to zero makes color fully transparent; a set to
     *        1.0 makes color fully opaque.
     * @param a    alpha component of color
     */
    void SetAlphaF(scalar a);

    /**
     * @brief Queries the blender.
     * @return the blender represented as a BlendMode
     */
    const BlendMode& GetBlendMode() const { return blendMode_; }

    /**
     * @brief Sets a blender that implements the specified blendmode enum.
     * @param mode  the BlendMode used to set the blender
     */
    void SetBlendMode(const BlendMode& mode);

    /**
     * @brief Sets Filter to filter.
     * @param filter  Filter to apply to subsequent draw
     */
    void SetFilter(const Filter& filter);

    /**
     * @brief Returns Filter if set, ot nullptr.
     * @return Filter if previously set, nullptr otherwise
     */
    const Filter& GetFilter() const;

    /**
     * @brief Queries the existance of filter.
     * @return true if the Brush has a filter, otherwise false
     */
    bool HasFilter() const { return hasFilter_; }

    /**
     * @brief Sets optional colors used when filling a path, such as a gradient.
     * @param e  how geometry is filled with color
     */
    void SetShaderEffect(std::shared_ptr<ShaderEffect> e);

    /**
     * @brief Returns optional colors used when filling a path, such as a gradient.
     * @return ShaderEffect if previously set, nullptr otherwise
     */
    std::shared_ptr<ShaderEffect> GetShaderEffect() const;

    /**
     * @brief Sets the current blender, increasing its refcnt, and if a blender is already
     *        present, decreasing that object's refcnt.
     * @param blender  Blender used to set
     */
    void SetBlender(std::shared_ptr<Blender> blender);

    /**
     * @brief Returns the user-supplied blend function, if one has been set.
     * @return the Blender assigned to this Brush, otherwise nullptr
     */
    std::shared_ptr<Blender> GetBlender() const { return blender_; }

    /**
     * @brief Returns true if pixels on the active edges of Path may be drawn with partial transparency.
     * @return antialiasing state
     */
    bool IsAntiAlias() const;

    /**
     * @brief Requests, but does not require, that edge pixels draw opaque or with
     *        partial transparency.
     * @param aa setting for antialiasing
     */
    void SetAntiAlias(bool aa);

    /**
     * @brief Returns true if Brush does not include elements requiring extensive computation
     *        to compute bounds of drawn geometry. For instance, Brush with PathEffect
     *        always returns false.
     * @return true if Brush allows for fast computation of bounds
     */
    bool CanComputeFastBounds();

    /**
     * @brief Only call this if CanComputeFastBounds() returned true. This takes a
     *        raw rectangle (the raw bounds of a shape), and adjusts it for stylistic
     *        effects in the Brush (e.g. stroking). If needed, it uses the storage
     *        parameter. It returns the adjusted bounds that can then be used
     *        for CoreCanvas::QuickReject tests.
     *
     *        The returned Rect will either be orig or storage, thus the caller
     *        should not rely on storage being set to the result, but should always
     *        use the returned value. It is legal for orig and storage to be the same Rect.
     * @param orig     geometry modified by Brush when drawn
     * @param storage  computed bounds of geometry; may not be nullptr
     * @return         fast computed bounds
     */
    const Rect& ComputeFastBounds(const Rect& orig, Rect* storage);

    /**
     * @brief Queries Whether the current blender can be represented as a BlendMode enum or not.
     * @return true if can be represented, otherwise false
     */
    bool AsBlendMode();

    /**
     * @brief Sets all Brush contents to their initial values. This is equivalent to replacing
     *        Brush with the result of Brush().
     */
    void Reset();

    /**
     * @brief Sets BlurDrawLooper, it will generate two draw operations, which may affect performance.
     */
    void SetLooper(std::shared_ptr<BlurDrawLooper> blurDrawLooper);

    /**
     * @brief Gets BlurDrawLooper.
     */
    std::shared_ptr<BlurDrawLooper> GetLooper() const;

    friend DRAWING_API bool operator==(const Brush& b1, const Brush& b2);
    friend DRAWING_API bool operator!=(const Brush& b1, const Brush& b2);

private:
    Color color_;
    BlendMode blendMode_;
    Filter filter_;
    std::shared_ptr<ColorSpace> colorSpace_;
    std::shared_ptr<ShaderEffect> shaderEffect_;
    std::shared_ptr<Blender> blender_;
    std::shared_ptr<BlurDrawLooper> blurDrawLooper_;

    bool antiAlias_;
    bool hasFilter_ = false;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif