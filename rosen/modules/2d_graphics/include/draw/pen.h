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

#ifndef PEN_H
#define PEN_H

#include "draw/brush.h"
#include "draw/color.h"
#include "effect/filter.h"
#include "effect/path_effect.h"
#include "utils/rect.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class DRAWING_API Pen {
public:
    enum class JoinStyle {
        MITER_JOIN,
        ROUND_JOIN,
        BEVEL_JOIN,
        DEFAULT_JOIN = MITER_JOIN
    };

    enum class CapStyle {
        FLAT_CAP,
        SQUARE_CAP,
        ROUND_CAP,
        DEFAULT_CAP = FLAT_CAP
    };

    Pen() noexcept;
    Pen(const Pen& p) noexcept = default;
    Pen(const Color& c) noexcept;
    Pen(int rgba) noexcept;
    ~Pen() {};

    /**
     * @brief Get the Color of pen
     * 
     * @return Color of pen
     */
    Color GetColor() const;

    /**
     * @brief Set the Color of pen
     * 
     * @param c color object to set
     */
    void SetColor(const Color& c);
    
    /**
     * @brief Set the Color object
     * 
     * @param c color int value to set
     */
    void SetColor(int c);
    
    /**
     * @brief set ARGB of pen
     * 
     * @param a alpha
     * @param r red
     * @param g green
     * @param b blue
     */
    void SetARGB(int a, int r, int g, int b);

    /**
     * @brief Get the Color 4f of pen
     * 
     * @return Color4f 
     */
    Color4f GetColor4f();

    /**
     * @brief Get the Color Space object
     * 
     * @return a shared pointer ColorSpace
     */
    std::shared_ptr<ColorSpace> GetColorSpace() const;

    /**
     * @brief Set the Color and ColorSpace of pen
     * 
     * @param cf color4f object
     * @param s  a shared pointer to colorspace
     */
    void SetColor(const Color4f& cf, std::shared_ptr<ColorSpace> s);

    /**
     * @brief Get the Alpha value of pen
     * 
     * @return uint32_t of alpha
     */
    uint32_t GetAlpha() const;

    /**
     * @brief Get the Alpha F object of pen
     * 
     * @return value of alpha f
     */
    scalar GetAlphaF() const;
    
    /**
     * @brief Set the Alpha value of pen
     * 
     * @param a the alpha value to set
     */
    void SetAlpha(uint32_t a);
    
    /**
     * @brief Set the Alpha F value of pen
     * 
     * @param a the alpha f value
     */
    void SetAlphaF(scalar a);

    /**
     * @brief Get the Width value of pen
     * 
     * @return width value of pen
     */
    scalar GetWidth() const;

    /**
     * @brief Set the Width value of pen
     * 
     * @param width the value of width
     */
    void SetWidth(scalar width);

    /**
     * @brief Get the Miter Limit of pen
     * 
     * @return the value of miter limit
     */
    scalar GetMiterLimit() const;
    
    /**
     * @brief Set the Miter Limit of pen
     * 
     * @param limit the value of miter limit to set
     */
    void SetMiterLimit(scalar limit);

    /**
     * @brief Get the Cap Style of pen
     * 
     * @return CapStyle of pen
     */
    CapStyle GetCapStyle() const;
    
    /**
     * @brief Set the Cap Style of pen
     * 
     * @param cs the cap style object
     */
    void SetCapStyle(CapStyle cs);

    /**
     * @brief Get the Join Style of pen
     * 
     * @return JoinStyle of pen
     */
    JoinStyle GetJoinStyle() const;

    /**
     * @brief Sets the geometry drawn at the corners of strokes.
     * 
     * @param js join style object
     */
    void SetJoinStyle(JoinStyle js);

    /**
     * @brief Get the Blend Mode of pen
     * 
     * @return the value of BlendMode 
     */
    BlendMode GetBlendMode() const;
    
    /**
     * @brief Set the Blend Mode of pen
     * 
     * @param mode the value of pen's blend mode
     */
    void SetBlendMode(BlendMode mode);

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
    std::shared_ptr<Blender> GetBlender() const;

    /**
     * @brief Returns true if pixels on the active edges of Path may be drawn with partial transparency.
     * @return antialiasing state
     */
    bool IsAntiAlias() const;

    /** 
     * @brief Requests, but does not require, that edge pixels draw opaque or with partial transparency.
     * @param aa  setting for antialiasing
     */
    void SetAntiAlias(bool aa);

    /**
     * @brief Sets PathEffect to pen. Pass nullptr to leave the path geometry unaltered.
     * @param e replace Path with a modification when drawn
     */
    void SetPathEffect(std::shared_ptr<PathEffect> e);

    /**
     * @brief Returns PathEffect if set, or nullptr.
     * @return A shared pointer to PathEffect if previously set, nullptr otherwise
     */
    std::shared_ptr<PathEffect> GetPathEffect() const;

    /**
     * @brief Set the Filter object with three property.
     * ColorFilter, MaskFilter, and ImageFilter to apply to subsequent draw
     * @param filter the Filter object to set
     */
    void SetFilter(const Filter& filter);
    Filter GetFilter() const;
    bool HasFilter() const;

    /** 
     * @brief Sets optional colors used when filling a path, such as a gradient. Sets SkShader to shader
     * @param shader how geometry is filled with color; if nullptr, color is used instead
     */
    void SetShaderEffect(std::shared_ptr<ShaderEffect> e);

    /**
     * @brief Returns optional colors used when filling a path, such as a gradient.
     * @return A shared pointer to ShaderEffect
     */
    std::shared_ptr<ShaderEffect> GetShaderEffect() const;

    /**
     * @brief Sets all contents to their initial values. This is equivalent to replacing
     * Pen with the result of Pen().
     */
    void Reset();

    friend DRAWING_API bool operator==(const Pen& p1, const Pen& p2);
    friend DRAWING_API bool operator!=(const Pen& p1, const Pen& p2);

private:
    scalar width_;
    scalar miterLimit_;
    JoinStyle join_;
    CapStyle cap_;
    std::shared_ptr<PathEffect> pathEffect_;

    Brush brush_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif