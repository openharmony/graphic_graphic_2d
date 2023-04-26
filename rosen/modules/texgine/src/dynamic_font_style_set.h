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

#ifndef ROSEN_MODULES_TEXGINE_SRC_DYNAMIC_FONT_STYLE_SET_H
#define ROSEN_MODULES_TEXGINE_SRC_DYNAMIC_FONT_STYLE_SET_H

#include "texgine_font_style.h"
#include "texgine_font_style_set.h"
#include "typeface.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class DynamicFontStyleSet {
public:
    explicit DynamicFontStyleSet(std::unique_ptr<Typeface> typeface);

    /*
     * @brief Return the count of typeface
     */
    int Count() const;

    /*
     * @brief Get the font style of typeface
     * @param index The index of the typeface in font config
     * @param style The out param of typeface style
     * @param name The name of typeface
     */
    void GetStyle(int index, std::shared_ptr<TexgineFontStyle> style, std::shared_ptr<TexgineString> name) const;

    /*
     * @brief create typeface by index
     */
    std::shared_ptr<TexgineTypeface> CreateTypeface(int index);

    /*
     * @brief  Matching typeface by font style
     * @param  pattern The font style of typeface
     * @Return The typeface that font style is pattern
     */
    std::shared_ptr<TexgineTypeface> MatchStyle(std::shared_ptr<TexgineFontStyle> pattern);

private:
    std::unique_ptr<Typeface> typeface_ = nullptr;
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_DYNAMIC_FONT_STYLE_SET_H
