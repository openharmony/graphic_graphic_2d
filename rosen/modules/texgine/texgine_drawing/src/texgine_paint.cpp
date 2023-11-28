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
#ifndef USE_ROSEN_DRAWING
TexginePaint::TexginePaint(): paint_(std::make_shared<SkPaint>()) {}
#else
TexginePaint::TexginePaint()
    : style_(Style::FILL), brush_(std::make_shared<RSBrush>()), pen_(std::make_shared<RSPen>()) {}
#endif

#ifndef USE_ROSEN_DRAWING
SkPaint TexginePaint::GetPaint() const
{
    return *paint_.get();
}
#else
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
#endif

#ifndef USE_ROSEN_DRAWING
void TexginePaint::SetPaint(const SkPaint &paint)
{
    *paint_ = paint;
}
#else
void TexginePaint::SetBrush(const RSBrush &brush)
{
    *brush_ = brush;
}

void TexginePaint::SetPen(const RSPen &pen)
{
    *pen_ = pen;
}
#endif

void TexginePaint::SetColor(const uint32_t color)
{
#ifndef USE_ROSEN_DRAWING
    paint_->setColor(color);
#else
    brush_->SetColor(color);
    pen_->SetColor(color);
#endif
}

void TexginePaint::SetAlphaf(const float alpha)
{
#ifndef USE_ROSEN_DRAWING
    paint_->setAlphaf(alpha);
#else
    brush_->SetAlphaF(alpha);
    pen_->SetAlphaF(alpha);
#endif
}

void TexginePaint::SetStrokeWidth(const double width)
{
#ifndef USE_ROSEN_DRAWING
    paint_->setStrokeWidth(width);
#else
    pen_->SetWidth(width);
#endif
}

void TexginePaint::SetAntiAlias(const bool aa)
{
#ifndef USE_ROSEN_DRAWING
    paint_->setAntiAlias(aa);
#else
    brush_->SetAntiAlias(aa);
    pen_->SetAntiAlias(aa);
#endif
}

void TexginePaint::SetARGB(const unsigned int a, const unsigned int r,
    const unsigned int g, const unsigned int b)
{
#ifndef USE_ROSEN_DRAWING
    paint_->setARGB(a, r, g, b);
#else
    brush_->SetARGB(a, r, g, b);
    pen_->SetARGB(a, r, g, b);
#endif
}

void TexginePaint::SetStyle(Style style)
{
#ifndef USE_ROSEN_DRAWING
    paint_->setStyle(static_cast<SkPaint::Style>(style));
#else
    style_ = style;
#endif
}

void TexginePaint::SetPathEffect(const std::shared_ptr<TexginePathEffect> pathEffect)
{
    if (pathEffect == nullptr) {
        return;
    }
#ifndef USE_ROSEN_DRAWING
    paint_->setPathEffect(pathEffect->GetPathEffect());
#else
    pen_->SetPathEffect(pathEffect->GetPathEffect());
#endif
}

void TexginePaint::SetMaskFilter(const std::shared_ptr<TexgineMaskFilter> maskFilter)
{
    if (maskFilter == nullptr) {
        return;
    }
#ifndef USE_ROSEN_DRAWING
    paint_->setMaskFilter(maskFilter->GetMaskFilter());
#else
    RSFilter filter;
    filter.SetMaskFilter(maskFilter->GetMaskFilter());
    brush_->SetFilter(filter);
    pen_->SetFilter(filter);
#endif
}

void TexginePaint::SetAlpha(const unsigned int alpha)
{
#ifndef USE_ROSEN_DRAWING
    paint_->setAlpha(alpha);
#else
    brush_->SetAlpha(alpha);
    pen_->SetAlpha(alpha);
#endif
}

void TexginePaint::SetBlendMode(TexgineBlendMode mode)
{
#ifndef USE_ROSEN_DRAWING
    paint_->setBlendMode(static_cast<SkBlendMode>(mode));
#else
    brush_->SetBlendMode(static_cast<Drawing::BlendMode>(mode));
    pen_->SetBlendMode(static_cast<Drawing::BlendMode>(mode));
#endif
}

bool TexginePaint::operator ==(const TexginePaint &rhs) const
{
#ifndef USE_ROSEN_DRAWING
    return *paint_ == rhs.GetPaint();
#else
    return style_ == rhs.GetStyle() && *brush_ == rhs.GetBrush() && *pen_ == rhs.GetPen();
#endif
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
