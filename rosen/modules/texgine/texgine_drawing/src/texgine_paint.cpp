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

#include "texgine_paint.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
TexginePaint::TexginePaint()
    : style_(Style::FILL), brush_(std::make_shared<RSBrush>()), pen_(std::make_shared<RSPen>()) {}

RSBrush TexginePaint::GetBrush() const
{
    return *brush_.get();
}

RSPen TexginePaint::GetPen() const
{
    return *pen_.get();
}

TexginePaint::Style TexginePaint::GetStyle() const
{
    return style_;
}

void TexginePaint::SetBrush(const RSBrush &brush)
{
    *brush_ = brush;
}

void TexginePaint::SetPen(const RSPen &pen)
{
    *pen_ = pen;
}

void TexginePaint::SetColor(const uint32_t color)
{
    brush_->SetColor(color);
    pen_->SetColor(color);
}

void TexginePaint::SetAlphaf(const float alpha)
{
    brush_->SetAlphaF(alpha);
    pen_->SetAlphaF(alpha);
}

void TexginePaint::SetStrokeWidth(const double width)
{
    pen_->SetWidth(width);
}

void TexginePaint::SetAntiAlias(const bool aa)
{
    brush_->SetAntiAlias(aa);
    pen_->SetAntiAlias(aa);
}

void TexginePaint::SetARGB(const unsigned int a, const unsigned int r,
    const unsigned int g, const unsigned int b)
{
    brush_->SetARGB(a, r, g, b);
    pen_->SetARGB(a, r, g, b);
}

void TexginePaint::SetStyle(Style style)
{
    style_ = style;
}

void TexginePaint::SetPathEffect(const std::shared_ptr<TexginePathEffect> pathEffect)
{
    if (pathEffect == nullptr) {
        return;
    }
    pen_->SetPathEffect(pathEffect->GetPathEffect());
}

void TexginePaint::SetMaskFilter(const std::shared_ptr<TexgineMaskFilter> maskFilter)
{
    if (maskFilter == nullptr) {
        return;
    }
    RSFilter filter;
    filter.SetMaskFilter(maskFilter->GetMaskFilter());
    brush_->SetFilter(filter);
    pen_->SetFilter(filter);
}

void TexginePaint::SetAlpha(const unsigned int alpha)
{
    brush_->SetAlpha(alpha);
    pen_->SetAlpha(alpha);
}

void TexginePaint::SetBlendMode(TexgineBlendMode mode)
{
    brush_->SetBlendMode(static_cast<Drawing::BlendMode>(mode));
    pen_->SetBlendMode(static_cast<Drawing::BlendMode>(mode));
}

bool TexginePaint::operator ==(const TexginePaint &rhs) const
{
    return style_ == rhs.GetStyle() && *brush_ == rhs.GetBrush() && *pen_ == rhs.GetPen();
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
