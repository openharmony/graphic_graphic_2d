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

#include "text/font.h"

#include "impl_factory.h"
#include "impl_interface/font_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
Font::Font() : fontImpl_(ImplFactory::CreateFontImpl()) {}

Font::Font(const Font& other) noexcept
{
    fontImpl_.reset(other.fontImpl_->Clone());
}

Font& Font::operator=(const Font& other) noexcept
{
    fontImpl_.reset(other.fontImpl_->Clone());
    return *this;
}

void Font::SetEdging(FontEdging edging)
{
    fontImpl_->SetEdging(edging);
}

void Font::SetSubpixel(bool isSubpixel)
{
    fontImpl_->SetSubpixel(isSubpixel);
}

void Font::SetHinting(FontHinting hintingLevel)
{
    fontImpl_->SetHinting(hintingLevel);
}

void Font::SetTypeface(std::shared_ptr<Typeface> typeface)
{
    fontImpl_->SetTypeface(typeface);
}

void Font::SetSize(scalar textSize)
{
    fontImpl_->SetSize(textSize);
}

void Font::SetEmbolden(bool isEmbolden)
{
    fontImpl_->SetEmbolden(isEmbolden);
}

void Font::SetSkewX(scalar skewX)
{
    fontImpl_->SetSkewX(skewX);
}

void Font::SetLinearMetrics(bool isLinearMetrics)
{
    fontImpl_->SetLinearMetrics(isLinearMetrics);
}

scalar Font::GetMetrics(FontMetrics* metrics) const
{
    return fontImpl_->GetMetrics(metrics);
}

void Font::GetWidths(const uint16_t glyphs[], int count, scalar widths[]) const
{
    fontImpl_->GetWidths(glyphs, count, widths);
}

void Font::GetWidths(const uint16_t glyphs[], int count, scalar widths[], Rect bounds[]) const
{
    fontImpl_->GetWidths(glyphs, count, widths, bounds);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
