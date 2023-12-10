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

#ifndef ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_FONT_STYLE_H
#define ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_FONT_STYLE_H

#include <memory>

#ifndef USE_ROSEN_DRAWING
#include <include/core/SkFontStyle.h>
#else
#include "drawing.h"
#endif

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class TexgineFontStyle {
public:
    enum class Slant {
        K_UPRIGHT_SLANT,
        K_ITALIC_SLANT,
        K_OBLIQUE_SLANT,
    };

    TexgineFontStyle();

    TexgineFontStyle(int weight, int width, Slant slant);

#ifndef USE_ROSEN_DRAWING
    explicit TexgineFontStyle(const std::shared_ptr<SkFontStyle> style);
#else
    explicit TexgineFontStyle(const std::shared_ptr<RSFontStyle> style);
#endif

    /*
     * @brief Sets SkFontStyle to TexgineFontStyle
     */
#ifndef USE_ROSEN_DRAWING
    void SetStyle(const SkFontStyle &style);
#else
    void SetStyle(const RSFontStyle &style);
#endif

    /*
     * @brief Sets SkFontStyle to TexgineFontStyle
     */
#ifndef USE_ROSEN_DRAWING
    void SetFontStyle(const std::shared_ptr<SkFontStyle> fontStyle);
#else
    void SetFontStyle(const std::shared_ptr<RSFontStyle> fontStyle);
#endif

    /*
     * @brief Returns SkFontStyle
     */
#ifndef USE_ROSEN_DRAWING
    std::shared_ptr<SkFontStyle> GetFontStyle() const;
#else
    std::shared_ptr<RSFontStyle> GetFontStyle() const;
#endif

    /*
     * @brief Returns weight
     */
    int GetWeight() const;

    /*
     * @brief Returns width
     */
    int GetWidth() const;

private:
#ifndef USE_ROSEN_DRAWING
    std::shared_ptr<SkFontStyle> fontStyle_ = nullptr;
#else
    std::shared_ptr<RSFontStyle> fontStyle_ = nullptr;
#endif
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_FONT_STYLE_H
