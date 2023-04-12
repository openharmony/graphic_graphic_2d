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

#include <include/core/SkFontStyle.h>

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class TexgineFontStyle {
public:
    enum class Slant {
        kUpright_Slant,
        kItalic_Slant,
        kOblique_Slant,
    };

    TexgineFontStyle();
    TexgineFontStyle(int weight, int width, Slant slant);
    TexgineFontStyle(std::shared_ptr<SkFontStyle> style);
    std::shared_ptr<SkFontStyle> GetFontStyle();
    void SetFontStyle(std::shared_ptr<SkFontStyle> fontStyle);
    void SetStyle(SkFontStyle style);

private:
    std::shared_ptr<SkFontStyle> fontStyle_ = nullptr;
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_FONT_STYLE_H
