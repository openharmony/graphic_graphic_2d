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

#ifndef ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_PAINT_H
#define ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_PAINT_H

#include <memory>

#include "drawing.h"

#include "texgine_mask_filter.h"
#include "texgine_path_effect.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class TexginePaint {
public:
    enum Style : uint8_t {
        FILL,
        STROKE,
        STROKEANDFILL,
    };

    enum TexgineBlendMode : uint8_t {
        CLEAR,
        SRC,
        DST,
        SRC_OVER,
        DST_OVER,
        SRC_IN,
        DST_IN,
        SRC_OUT,
        DST_OUT,
        SRC_ATOP,
        DST_ATOP,
        XOR,
        PLUS,
        MODULATE,
        SCREEN,
        OVERLAY,
        DARKEN,
        LIGHTEN,
        COLOR_DODGE,
        COLOR_BURN,
        HARD_LIGHT,
        SOFT_LIGHT,
        DIFFERENCE,
        EXCLUSION,
        MULTIPLY,
        HUE,
        STATURATION,
        COLOR,
        LUMINOSITY,
    };

    TexginePaint();

    /*
     * @brief Gets Brush from TexginePaint
     */
    RSBrush GetBrush() const;

    /*
     * @brief Gets Pen from TexginePaint
     */
    RSPen GetPen() const;

    /*
     * @brief Gets Style from TexginePaint
     */
    Style GetStyle() const;

    /*
     * @brief Sets Brush to TexginePaint
     */
    void SetBrush(const RSBrush &brush);

    /*
     * @brief Sets Pen to TexginePaint
     */
    void SetPen(const RSPen &pen);

    /*
     * @brief Sets alpha and RGB used when stroking and filling
     */
    void SetColor(const uint32_t color);

    /*
     * @brief Replace alpha while keeping RGB unchanged.
     */
    void SetAlphaf(const float alpha);

    /*
     * @brief Sets the thickness of the pen used by the paint to outline the shape
     */
    void SetStrokeWidth(const double width);

    /*
     * @brief Sets edge pixels are drawn as opaque or partially transparent.
     */
    void SetAntiAlias(const bool aa);

    /*
     * @brief Sets color used when drawing solid fills
     * @param a The quantity, from completely transparent (0) to completely opaque (255)
     * @param r Red, from no red (0) to full red (255)
     * @param g Green, from no green (0) to full green (255)
     * @param b blue, from no blue (0) to full blue (255)
     */
    void SetARGB(const unsigned int a, const unsigned int r,
        const unsigned int g, const unsigned int b);

    /*
     * @brief Sets whether the geometry is filled, stroked or filled and stroked
     */
    void SetStyle(Style style);

    /*
     * @brief Sets TexginePathEffect to the paint
     * @param pathEffect The path that replace SkPath when drawn
     */
    void SetPathEffect(const std::shared_ptr<TexginePathEffect> pathEffect);

    /*
     * @brief Sets TexgineMaskFilter to the paint
     * @param maskFilter Modifies clipping mask generated from drawn geometry
     */
    void SetMaskFilter(const std::shared_ptr<TexgineMaskFilter> maskFilter);

    /*
     * @brief Replaces alpha, leaving RGB
     * @param alpha Is from 0.0 to 1.0
     *              0.0 makes color fully transparent
     *              1.0 makes color fully opaque
     */
    void SetAlpha(const unsigned int alpha);

    /*
     * @brief Sets blend mode to paint
     */
    void SetBlendMode(TexgineBlendMode mode);
    bool operator==(const TexginePaint &rhs) const;

private:
    Style style_;
    std::shared_ptr<RSBrush> brush_ = nullptr;
    std::shared_ptr<RSPen> pen_ = nullptr;
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_PAINT_H
