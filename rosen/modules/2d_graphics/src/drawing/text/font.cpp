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

Font::Font(std::shared_ptr<Typeface> typeface, scalar size, scalar scaleX, scalar skewX)
    : fontImpl_(ImplFactory::CreateFontImpl(typeface, size, scaleX, skewX)) {}

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

void Font::SetScaleX(scalar scaleX)
{
    fontImpl_->SetScaleX(scaleX);
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

scalar Font::GetSize() const
{
    return fontImpl_->GetSize();
}

std::shared_ptr<Typeface> Font::GetTypeface()
{
    return fontImpl_->GetTypeface();
}

scalar Font::MeasureText(const void* text, size_t byteLength, TextEncoding encoding)
{
    return fontImpl_->MeasureText(text, byteLength, encoding);
}

int Font::CountText(const void* text, size_t byteLength, TextEncoding encoding) const
{
    return fontImpl_->CountText(text, byteLength, encoding);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
