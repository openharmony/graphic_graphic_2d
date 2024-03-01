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
std::shared_ptr<RSFont> TexgineFont::GetFont() const
{
    return font_;
}

void TexgineFont::SetTypeface(const std::shared_ptr<TexgineTypeface> tf)
{
    if (tf) {
        font_->SetTypeface(tf->GetTypeface());
    } else {
        font_->SetTypeface(nullptr);
    }
}

void TexgineFont::SetSize(float textSize)
{
    font_->SetSize(textSize);
}

float TexgineFont::GetMetrics(std::shared_ptr<TexgineFontMetrics> metrics) const
{
    if (metrics == nullptr) {
        return 0;
    }

    return font_->GetMetrics(metrics->GetFontMetrics().get());
}
void TexgineFont::SetSubpixel(const bool isSubpixel)
{
    font_->SetSubpixel(isSubpixel);
}

void TexgineFont::SetEdging(const FontEdging edging)
{
    font_->SetEdging(static_cast<Drawing::FontEdging>(edging));
}

void TexgineFont::SetHinting(const TexgineFontHinting hinting)
{
    font_->SetHinting(static_cast<Drawing::FontHinting>(hinting));
}

void TexgineFont::SetSkewX()
{
    font_->SetSkewX(-DEFAULT_ITALIC / 4); // standard italic offset is 1/4
}

void TexgineFont::SetBold()
{
    font_->SetEmbolden(true);
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
