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

#include "texgine_font_style.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
#ifndef USE_ROSEN_DRAWING
TexgineFontStyle::TexgineFontStyle(): fontStyle_(std::make_shared<SkFontStyle>())
#else
TexgineFontStyle::TexgineFontStyle(): fontStyle_(std::make_shared<RSFontStyle>())
#endif
{
}

#ifndef USE_ROSEN_DRAWING
TexgineFontStyle::TexgineFontStyle(
    int weight, int width, Slant slant): fontStyle_(
        std::make_shared<SkFontStyle>(weight, width, static_cast<SkFontStyle::Slant>(slant)))
#else
TexgineFontStyle::TexgineFontStyle(
    int weight, int width, Slant slant): fontStyle_(
        std::make_shared<RSFontStyle>(weight, width, static_cast<RSFontStyle::Slant>(slant)))
#endif
{
}

#ifndef USE_ROSEN_DRAWING
TexgineFontStyle::TexgineFontStyle(std::shared_ptr<SkFontStyle> style)
#else
TexgineFontStyle::TexgineFontStyle(std::shared_ptr<RSFontStyle> style)
#endif
{
    fontStyle_ = style;
}

#ifndef USE_ROSEN_DRAWING
std::shared_ptr<SkFontStyle> TexgineFontStyle::GetFontStyle() const
#else
std::shared_ptr<RSFontStyle> TexgineFontStyle::GetFontStyle() const
#endif
{
    return fontStyle_;
}

#ifndef USE_ROSEN_DRAWING
void TexgineFontStyle::SetFontStyle(const std::shared_ptr<SkFontStyle> fontStyle)
#else
void TexgineFontStyle::SetFontStyle(const std::shared_ptr<RSFontStyle> fontStyle)
#endif
{
    fontStyle_ = fontStyle;
}

#ifndef USE_ROSEN_DRAWING
void TexgineFontStyle::SetStyle(const SkFontStyle &style)
#else
void TexgineFontStyle::SetStyle(const RSFontStyle &style)
#endif
{
    if (fontStyle_ == nullptr) {
        return;
    }
    *fontStyle_ = style;
}

int TexgineFontStyle::GetWeight() const
{
    if (fontStyle_ == nullptr) {
        return 0;
    }
#ifndef USE_ROSEN_DRAWING
    return fontStyle_->weight();
#else
    return fontStyle_->GetWeight();
#endif
}

int TexgineFontStyle::GetWidth() const
{
    if (fontStyle_ == nullptr) {
        return 0;
    }
#ifndef USE_ROSEN_DRAWING
    return fontStyle_->width();
#else
    return fontStyle_->GetWidth();
#endif
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
