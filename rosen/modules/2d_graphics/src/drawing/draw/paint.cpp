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

#include "draw/paint.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
Paint::Paint() noexcept
    : filter_() {}

Paint::Paint(const Paint& other) noexcept
{
    antiAlias_ = other.antiAlias_;
    color_ = other.color_;
    blendMode_ = other.blendMode_;
    style_ = other.style_;
    width_ = other.width_;
    miterLimit_ = other.miterLimit_;
    cap_ = other.cap_;
    join_ = other.join_;
    if (other.hasFilter_) {
        filter_ = other.filter_;
        hasFilter_ = true;
    } else {
        filter_.Reset();
        hasFilter_ = false;
    }
    colorSpace_ = other.colorSpace_;
    shaderEffect_ = other.shaderEffect_;
    pathEffect_ = other.pathEffect_;
    blender_ = other.blender_;
}

Paint::Paint(const Color& c, std::shared_ptr<ColorSpace> colorSpace) noexcept
    : color_(c), filter_(), colorSpace_(colorSpace) {}

Paint& Paint::operator=(const Paint& other)
{
    antiAlias_ = other.antiAlias_;
    color_ = other.color_;
    blendMode_ = other.blendMode_;
    style_ = other.style_;
    width_ = other.width_;
    miterLimit_ = other.miterLimit_;
    cap_ = other.cap_;
    join_ = other.join_;
    if (other.hasFilter_) {
        filter_ = other.filter_;
        hasFilter_ = true;
    } else {
        filter_.Reset();
        hasFilter_ = false;
    }
    colorSpace_ = other.colorSpace_;
    shaderEffect_ = other.shaderEffect_;
    pathEffect_ = other.pathEffect_;
    blender_ = other.blender_;
    return *this;
}

bool Paint::CanCombinePaint(const Paint& pen, const Paint& brush)
{
    return pen.antiAlias_ == brush.antiAlias_ &&
        pen.color_ == brush.color_ &&
        pen.blendMode_ == brush.blendMode_ &&
        pen.hasFilter_ == brush.hasFilter_ &&
        pen.filter_ == brush.filter_ &&
        pen.colorSpace_ == brush.colorSpace_ &&
        pen.shaderEffect_ == brush.shaderEffect_ &&
        pen.blender_ == brush.blender_;
}

void Paint::AttachBrush(const Brush& brush)
{
    antiAlias_ = brush.IsAntiAlias();
    color_ = brush.GetColor();
    blendMode_ = brush.GetBlendMode();
    style_ = PaintStyle::PAINT_FILL;
    if (brush.HasFilter()) {
        filter_ = brush.GetFilter();
        hasFilter_ = true;
    } else {
        filter_.Reset();
        hasFilter_ = false;
    }
    colorSpace_ = brush.GetColorSpace();
    shaderEffect_ = brush.GetShaderEffect();
    blender_ = brush.GetBlender();
}

void Paint::AttachPen(const Pen& pen)
{
    antiAlias_ = pen.IsAntiAlias();
    color_ = pen.GetColor();
    blendMode_ = pen.GetBlendMode();
    style_ = PaintStyle::PAINT_STROKE;
    width_ = pen.GetWidth();
    miterLimit_ = pen.GetMiterLimit();
    cap_ = pen.GetCapStyle();
    join_ = pen.GetJoinStyle();
    if (pen.HasFilter()) {
        filter_ = pen.GetFilter();
        hasFilter_ = true;
    } else {
        filter_.Reset();
        hasFilter_ = false;
    }
    colorSpace_ = pen.GetColorSpace();
    shaderEffect_ = pen.GetShaderEffect();
    pathEffect_ = pen.GetPathEffect();
    blender_ = pen.GetBlender();
}

void Paint::SetStyle(const PaintStyle& style)
{
    style_ = style;
}

bool Paint::HasStrokeStyle() const
{
    return style_ == PaintStyle::PAINT_FILL_STROKE || style_ == PaintStyle::PAINT_STROKE;
}

void Paint::SetColor(const Color& c)
{
    color_ = c;
}

void Paint::SetARGB(int a, int r, int g, int b)
{
    color_.SetRgb(r, g, b, a);
}

void Paint::SetColor(const Color4f& cf, std::shared_ptr<ColorSpace> s)
{
    color_.SetRgbF(cf.redF_, cf.greenF_, cf.blueF_, cf.alphaF_);
    colorSpace_ = s;
}

void Paint::SetAlpha(uint32_t a)
{
    color_.SetAlpha(a);
}

void Paint::SetAlphaF(scalar a)
{
    color_.SetAlphaF(a);
}

void Paint::SetWidth(scalar width)
{
    width_ = width;
}

void Paint::SetMiterLimit(scalar limit)
{
    miterLimit_ = limit;
}

void Paint::SetCapStyle(Pen::CapStyle cs)
{
    cap_ = cs;
}

void Paint::SetJoinStyle(Pen::JoinStyle js)
{
    join_ = js;
}

void Paint::SetBlendMode(BlendMode mode)
{
    blendMode_ = mode;
}

void Paint::SetFilter(const Filter& filter)
{
    filter_ = filter;
    hasFilter_ = true;
}

void Paint::SetShaderEffect(std::shared_ptr<ShaderEffect> e)
{
    shaderEffect_ = e;
}

void Paint::SetPathEffect(std::shared_ptr<PathEffect> e)
{
    pathEffect_ = e;
}

void Paint::SetBlender(std::shared_ptr<Blender> blender)
{
    blender_ = blender;
}

void Paint::SetAntiAlias(bool aa)
{
    antiAlias_ = aa;
}

void Paint::Reset()
{
    antiAlias_ = false;
    color_ = Color::COLOR_BLACK;
    blendMode_ = BlendMode::SRC_OVER;
    style_ = PaintStyle::PAINT_NONE;
    width_ = 0;
    miterLimit_ = DEFAULT_MITER_VAL;
    join_ = Pen::JoinStyle::DEFAULT_JOIN;
    cap_ = Pen::CapStyle::DEFAULT_CAP;

    hasFilter_ = false;
    filter_.Reset();

    colorSpace_ = nullptr;
    shaderEffect_ = nullptr;
    pathEffect_ = nullptr;
}

void Paint::Disable()
{
    style_ = PaintStyle::PAINT_NONE;
    hasFilter_ = false;
}

bool operator==(const Paint& p1, const Paint& p2)
{
    return p1.antiAlias_ == p2.antiAlias_ &&
        p1.color_ == p2.color_ &&
        p1.blendMode_ == p2.blendMode_ &&
        p1.style_ == p2.style_ &&
        IsScalarAlmostEqual(p1.width_, p2.width_) &&
        IsScalarAlmostEqual(p1.miterLimit_, p2.miterLimit_) &&
        p1.join_ == p2.join_ &&
        p1.cap_ == p2.cap_ &&
        p1.filter_ == p2.filter_ &&
        p1.colorSpace_ == p2.colorSpace_ &&
        p1.shaderEffect_ == p2.shaderEffect_ &&
        p1.pathEffect_ == p2.pathEffect_ &&
        p1.blender_ == p2.blender_;
}

bool operator!=(const Paint& p1, const Paint& p2)
{
    return p1.antiAlias_ != p2.antiAlias_ ||
        p1.color_ != p2.color_ ||
        p1.blendMode_ != p2.blendMode_ ||
        p1.style_ != p2.style_ ||
        !IsScalarAlmostEqual(p1.width_, p2.width_) ||
        !IsScalarAlmostEqual(p1.miterLimit_, p2.miterLimit_) ||
        p1.join_ != p2.join_ ||
        p1.cap_ != p2.cap_ ||
        p1.filter_ != p2.filter_ ||
        p1.colorSpace_ != p2.colorSpace_ ||
        p1.shaderEffect_ != p2.shaderEffect_ ||
        p1.pathEffect_ != p2.pathEffect_ ||
        p1.blender_ != p2.blender_;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS