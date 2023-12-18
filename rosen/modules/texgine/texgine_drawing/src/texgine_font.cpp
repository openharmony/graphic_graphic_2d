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

#include "texgine_font.h"

#include <include/core/SkFontTypes.h>

namespace OHOS {
namespace Rosen {
namespace TextEngine {
#define DEFAULT_ITALIC 1.0f
#ifndef USE_ROSEN_DRAWING
std::shared_ptr<SkFont> TexgineFont::GetFont() const
#else
std::shared_ptr<RSFont> TexgineFont::GetFont() const
#endif
{
    return font_;
}

void TexgineFont::SetTypeface(const std::shared_ptr<TexgineTypeface> tf)
{
    if (tf) {
#ifndef USE_ROSEN_DRAWING
        font_->setTypeface(tf->GetTypeface());
#else
        font_->SetTypeface(tf->GetTypeface());
#endif
    } else {
#ifndef USE_ROSEN_DRAWING
        font_->setTypeface(nullptr);
#else
        font_->SetTypeface(nullptr);
#endif
    }
}

void TexgineFont::SetSize(float textSize)
{
#ifndef USE_ROSEN_DRAWING
    font_->setSize(textSize);
#else
    font_->SetSize(textSize);
#endif
}

float TexgineFont::GetMetrics(std::shared_ptr<TexgineFontMetrics> metrics) const
{
    if (metrics == nullptr) {
        return 0;
    }

#ifndef USE_ROSEN_DRAWING
    return font_->getMetrics(metrics->GetFontMetrics().get());
#else
    return font_->GetMetrics(metrics->GetFontMetrics().get());
#endif
}
void TexgineFont::SetSubpixel(const bool isSubpixel)
{
#ifndef USE_ROSEN_DRAWING
    font_->setSubpixel(isSubpixel);
#else
    font_->SetSubpixel(isSubpixel);
#endif
}

void TexgineFont::SetEdging(const FontEdging edging)
{
#ifndef USE_ROSEN_DRAWING
    font_->setEdging(static_cast<SkFont::Edging>(edging));
#else
    font_->SetEdging(static_cast<Drawing::FontEdging>(edging));
#endif
}

void TexgineFont::SetHinting(const TexgineFontHinting hinting)
{
#ifndef USE_ROSEN_DRAWING
    font_->setHinting(static_cast<SkFontHinting>(hinting));
#else
    font_->SetHinting(static_cast<Drawing::FontHinting>(hinting));
#endif
}

void TexgineFont::SetSkewX()
{
#ifndef USE_ROSEN_DRAWING
    font_->setSkewX(-DEFAULT_ITALIC / 4); // standard italic offset is 1/4
#else
    font_->SetSkewX(-DEFAULT_ITALIC / 4); // standard italic offset is 1/4
#endif
}

void TexgineFont::SetBold()
{
#ifndef USE_ROSEN_DRAWING
    font_->setEmbolden(true); // false means bold is turned off by default
#else
    font_->SetEmbolden(true);
#endif
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
