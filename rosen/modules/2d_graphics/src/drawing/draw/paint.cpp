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

#include <string>
#include "draw/paint.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
Paint::Paint() noexcept
    : filter_() {}

Paint::Paint(const Paint& other) noexcept
{
    // Tell the compiler there is no alias and to select wider load/store instructions.
    bool antiAlias = other.antiAlias_;
    bool blenderEnabled = other.blenderEnabled_;
    bool hasFilter = other.hasFilter_;
    PaintStyle style = other.style_;
    BlendMode blendMode = other.blendMode_;
    scalar width = other.width_;
    scalar miterLimit = other.miterLimit_;
    Pen::JoinStyle join = other.join_;
    Pen::CapStyle cap = other.cap_;
    antiAlias_ = antiAlias;
    blenderEnabled_ = blenderEnabled;
    hasFilter_ = hasFilter;
    style_ = style;
    blendMode_ = blendMode;
    width_ = width;
    miterLimit_ = miterLimit;
    join_ = join;
    cap_ = cap;

    colorSpace_ = other.colorSpace_;
    shaderEffect_ = other.shaderEffect_;
    pathEffect_ = other.pathEffect_;
    blender_ = other.blender_;
    blurDrawLooper_ = other.blurDrawLooper_;
    color_ = other.color_;
    if (other.hasFilter_) {
        filter_ = other.filter_;
    } else {
        filter_.Reset();
    }
}

Paint::Paint(const Color& c, std::shared_ptr<ColorSpace> colorSpace) noexcept
    : colorSpace_(colorSpace), color_(c), filter_() {}

Paint& Paint::operator=(const Paint& other)
{
    // Tell the compiler there is no alias and to select wider load/store instructions.
    bool antiAlias = other.antiAlias_;
    bool blenderEnabled = other.blenderEnabled_;
    bool hasFilter = other.hasFilter_;
    PaintStyle style = other.style_;
    BlendMode blendMode = other.blendMode_;
    scalar width = other.width_;
    scalar miterLimit = other.miterLimit_;
    Pen::JoinStyle join = other.join_;
    Pen::CapStyle cap = other.cap_;
    antiAlias_ = antiAlias;
    blenderEnabled_ = blenderEnabled;
    hasFilter_ = hasFilter;
    style_ = style;
    blendMode_ = blendMode;
    width_ = width;
    miterLimit_ = miterLimit;
    join_ = join;
    cap_ = cap;

    colorSpace_ = other.colorSpace_;
    shaderEffect_ = other.shaderEffect_;
    pathEffect_ = other.pathEffect_;
    blender_ = other.blender_;
    blurDrawLooper_ = other.blurDrawLooper_;
    color_ = other.color_;
    if (other.hasFilter_) {
        filter_ = other.filter_;
    } else {
        filter_.Reset();
    }
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
        pen.blender_ == brush.blender_ &&
        pen.blenderEnabled_ == brush.blenderEnabled_ &&
        pen.blurDrawLooper_ == brush.blurDrawLooper_;
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
    blenderEnabled_ = brush.GetBlenderEnabled();
    blurDrawLooper_ = brush.GetLooper();
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
    blenderEnabled_ = pen.GetBlenderEnabled();
    blurDrawLooper_ = pen.GetLooper();
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
    blender_ = nullptr;
    blendMode_ = mode;
}

void Paint::SetFilter(const Filter& filter)
{
    filter_ = filter;
    hasFilter_ = true;
}

void Paint::SetShaderEffect(std::shared_ptr<ShaderEffect> e)
{
#ifdef DRAWING_DISABLE_API
    if (DrawingConfig::IsDisabled(DrawingConfig::DrawingDisableFlag::DISABLE_SHADER)) {
        shaderEffect_ = nullptr;
        return;
    }
#endif
    shaderEffect_ = e;
}

void Paint::SetPathEffect(std::shared_ptr<PathEffect> e)
{
#ifdef DRAWING_DISABLE_API
    if (DrawingConfig::IsDisabled(DrawingConfig::DrawingDisableFlag::DISABLE_PATH_EFFECT)) {
        pathEffect_ = nullptr;
        return;
    }
#endif
    pathEffect_ = e;
}

void Paint::SetBlender(std::shared_ptr<Blender> blender)
{
#ifdef DRAWING_DISABLE_API
    if (DrawingConfig::IsDisabled(DrawingConfig::DrawingDisableFlag::DISABLE_BLENDER)) {
        blender_ = nullptr;
        return;
    }
#endif
    blender_ = blender;
}

void Paint::SetBlenderEnabled(bool blenderEnabled)
{
    blenderEnabled_ = blenderEnabled;
}

void Paint::SetLooper(std::shared_ptr<BlurDrawLooper> blurDrawLooper)
{
    blurDrawLooper_ = blurDrawLooper;
}

std::shared_ptr<BlurDrawLooper> Paint::GetLooper() const
{
    return blurDrawLooper_;
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
    blurDrawLooper_ = nullptr;
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
        p1.blender_ == p2.blender_ &&
        p1.blenderEnabled_ == p2.blenderEnabled_ &&
        p1.blurDrawLooper_ == p2.blurDrawLooper_;
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
        p1.blender_ != p2.blender_ ||
        p1.blenderEnabled_ != p2.blenderEnabled_ ||
        p1.blurDrawLooper_ != p2.blurDrawLooper_;
}

void Paint::Dump(std::string& out) const
{
    out += "[antiAlias:" + std::string(antiAlias_ ? "true" : "false");
    out += " color";
    color_.Dump(out);
    out += " blendMode:" + std::to_string(static_cast<int>(blendMode_));
    out += " style:" + std::to_string(static_cast<uint8_t>(style_));
    out += " width:" + std::to_string(width_);
    out += " miterLimit:" + std::to_string(miterLimit_);
    out += " join:" + std::to_string(static_cast<int>(join_));
    out += " cap:" + std::to_string(static_cast<int>(cap_));
    out += " blenderEnabled:" + std::string(blenderEnabled_ ? "true" : "false");
    out += " hasFilter:" + std::string(hasFilter_ ? "true" : "false");
    out += ']';
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS