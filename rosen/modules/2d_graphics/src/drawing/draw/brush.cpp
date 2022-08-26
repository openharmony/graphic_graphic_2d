/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "draw/brush.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
Brush::Brush() noexcept
    : color_(),
      blendMode_(BlendMode::SRC_OVER),
      filter_(),
      colorSpace_(nullptr),
      shaderEffect_(nullptr),
      antiAlias_(false)
{}

Brush::Brush(const Color& c) noexcept
    : color_(c),
      blendMode_(BlendMode::SRC_OVER),
      filter_(),
      colorSpace_(nullptr),
      shaderEffect_(nullptr),
      antiAlias_(false)
{}

Brush::Brush(int rgba) noexcept
    : color_(rgba),
      blendMode_(BlendMode::SRC_OVER),
      filter_(),
      colorSpace_(nullptr),
      shaderEffect_(nullptr),
      antiAlias_(false)
{}

Brush::Brush(std::shared_ptr<ShaderEffect> e) noexcept
    : color_(), blendMode_(BlendMode::SRC_OVER), filter_(), colorSpace_(nullptr), shaderEffect_(e), antiAlias_(false)
{}

Color Brush::GetColor() const
{
    return color_;
}

void Brush::SetColor(const Color& c)
{
    color_ = c;
}

void Brush::SetColor(int c)
{
    color_.SetColorQuad(c);
}

void Brush::SetARGB(int r, int g, int b, int a)
{
    color_.SetRgb(r, g, b, a);
}

Color4f Brush::GetColor4f()
{
    return color_.GetColor4f();
}

std::shared_ptr<ColorSpace> Brush::GetColorSpace() const
{
    return colorSpace_;
}

void Brush::SetColor(const Color4f& cf, std::shared_ptr<ColorSpace> s)
{
    color_.SetRgbF(cf.redF_, cf.greenF_, cf.blueF_, cf.alphaF_);
    colorSpace_ = s;
}

uint32_t Brush::GetAlpha() const
{
    return color_.GetAlpha();
}

void Brush::SetAlpha(uint32_t a)
{
    color_.SetAlpha(a);
}

void Brush::SetAlphaF(scalar a)
{
    color_.SetAlphaF(a);
}

BlendMode Brush::GetBlendMode() const
{
    return blendMode_;
}

void Brush::SetBlendMode(BlendMode mode)
{
    blendMode_ = mode;
}

void Brush::SetFilter(const Filter& filter)
{
    filter_ = filter;
}

Filter Brush::GetFilter() const
{
    return filter_;
}

void Brush::SetShaderEffect(std::shared_ptr<ShaderEffect> e)
{
    shaderEffect_ = e;
}

std::shared_ptr<ShaderEffect> Brush::GetShaderEffect() const
{
    return shaderEffect_;
}

bool Brush::IsAntiAlias() const
{
    return antiAlias_;
}

void Brush::SetAntiAlias(bool aa)
{
    antiAlias_ = aa;
}

void Brush::Reset()
{
    *this = Brush();
}

bool operator==(const Brush& b1, const Brush& b2)
{
    return b1.color_ == b2.color_ && b1.blendMode_ == b2.blendMode_ && b1.shaderEffect_ == b2.shaderEffect_ &&
        b1.colorSpace_ == b2.colorSpace_ && b1.filter_ == b2.filter_ && b1.antiAlias_ == b2.antiAlias_;
}

bool operator!=(const Brush& b1, const Brush& b2)
{
    return b1.color_ != b2.color_ || b1.blendMode_ == b2.blendMode_ || b1.shaderEffect_ != b2.shaderEffect_ ||
        b1.colorSpace_ == b2.colorSpace_ || b1.filter_ != b2.filter_ || b1.antiAlias_ == b2.antiAlias_;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
