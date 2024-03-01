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

#ifndef ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_FONT_H
#define ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_FONT_H

#include <memory>

#include "drawing.h"

#include "texgine_font_metrics.h"
#include "texgine_typeface.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class TexgineFont {
public:
    enum FontEdging : uint8_t {
        ALIAS,
        ANTIALIAS,
        SUBPIXEL_ANTIALIAS,
    };

    enum TexgineFontHinting : uint8_t {
        NONE,
        SLIGHT,
        NORMAL,
        FULL,
    };

    std::shared_ptr<RSFont> GetFont() const;

    /*
     * @brief Set typeface to SkFont
     */
    void SetTypeface(const std::shared_ptr<TexgineTypeface> tf);

    /*
     * @brief Set font size
     */
    void SetSize(float textSize);

    /*
     * @brief Get metrics of the font
     */
    float GetMetrics(std::shared_ptr<TexgineFontMetrics> metrics) const;

    /*
     * @brief Get metrics of the font
     */
    void SetSubpixel(const bool isSubpixel);

    /*
     * @brief Set subpixel positioning
     */
    void SetEdging(const FontEdging edging);

    /*
     * @brief Set font optimization mode
     */
    void SetHinting(const TexgineFontHinting hinting);

    /*
     * @brief Set font default non-bold italics mode
     */
    void SetSkewX();

    void SetBold();
private:
    std::shared_ptr<RSFont> font_ = std::make_shared<RSFont>();
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_FONT_H
