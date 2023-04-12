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
{
    paint_ = std::make_shared<SkPaint>();
}

SkPaint TexginePaint::GetPaint() const
{
    return *paint_.get();
}

void TexginePaint::SetPaint(SkPaint paint)
{
    *paint_ = paint;
}

void TexginePaint::SetColor(uint32_t color)
{
    paint_->setColor(color);
}

void TexginePaint::SetAlphaf(float alpha)
{
    paint_->setAlphaf(alpha);
}

void TexginePaint::SetStrokeWidth(double width)
{
    paint_->setStrokeWidth(width);
}

void TexginePaint::SetAntiAlias(bool aa)
{
    paint_->setAntiAlias(aa);
}

void TexginePaint::SetARGB(unsigned int a, unsigned int r, unsigned int g, unsigned int b)
{
    paint_->setARGB(a, r, g, b);
}

void TexginePaint::SetStyle(Style style)
{
    paint_->setStyle(static_cast<SkPaint::Style>(style));
}

void TexginePaint::SetPathEffect(std::shared_ptr<TexginePathEffect> pathEffect)
{
    paint_->setPathEffect(pathEffect->GetPathEffect());
}

void TexginePaint::SetMaskFilter(std::shared_ptr<TexgineMaskFilter> maskFilter)
{
    paint_->setMaskFilter(maskFilter->GetMaskFilter());
}

void TexginePaint::SetAlpha(unsigned int alpha)
{
    paint_->setAlpha(alpha);
}

void TexginePaint::SetBlendMode(TexgineBlendMode mode)
{
    paint_->setBlendMode(static_cast<SkBlendMode>(mode));
}

bool TexginePaint::operator ==(const TexginePaint &rhs) const
{
    return *paint_ == rhs.GetPaint();
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
