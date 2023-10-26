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

#include "skia_paint.h"

#include "skia_color_filter.h"
#include "skia_color_space.h"
#include "skia_image_filter.h"
#include "skia_mask_filter.h"
#include "skia_path.h"
#include "skia_path_effect.h"
#include "skia_shader_effect.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
SkiaPaint::SkiaPaint() noexcept
    : isStrokeFirst_(false)
{
    fill_.paintData_.fillCore_ = new SkStyleFillCore();
    stroke_.paintData_.strokeCore_ = new SkStyleStrokeCore();
}

SkiaPaint::~SkiaPaint()
{
    delete fill_.paintData_.fillCore_;
    fill_.paintData_.fillCore_ = nullptr;
    delete stroke_.paintData_.strokeCore_;
    stroke_.paintData_.strokeCore_ = nullptr;
}

void SkiaPaint::ApplyBrushToFill(const Brush& brush)
{
    fill_.isEnabled_ = true;
    SkStyleFillCore* fillData = fill_.paintData_.fillCore_;
    fillData->Reset();
    ApplyBrushOrPenToFillCore(brush, *fillData);
}

void SkiaPaint::ApplyPenToStroke(const Pen& pen)
{
    stroke_.isEnabled_ = true;
    SkStyleStrokeCore* strokeData = stroke_.paintData_.strokeCore_;
    strokeData->Reset();
    ApplyPenToStrokeCore(pen, *strokeData);
}

void SkiaPaint::BrushToSkPaint(const Brush& brush, SkPaint& paint) const
{
    auto cs = brush.GetColorSpace();
    if (cs != nullptr) {
        auto skColorSpaceImpl = cs->GetImpl<SkiaColorSpace>();
        sk_sp<SkColorSpace> colorSpace = (skColorSpaceImpl != nullptr) ? skColorSpaceImpl->GetColorSpace() : nullptr;
        paint.setColor(SkColor4f::FromColor(brush.GetColor().CastToColorQuad()), colorSpace.get());
    } else {
        paint.setColor(brush.GetColor().CastToColorQuad());
    }

    if (brush.GetBlendMode() != BlendMode::SRC_OVER) {
        paint.setBlendMode(static_cast<SkBlendMode>(brush.GetBlendMode()));
    }

    paint.setAlpha(brush.GetAlpha());
    paint.setAntiAlias(brush.IsAntiAlias());

    auto s = brush.GetShaderEffect();
    if (s != nullptr) {
        auto skShaderImpl = s->GetImpl<SkiaShaderEffect>();
        sk_sp<SkShader> skShader = (skShaderImpl != nullptr) ? skShaderImpl->GetShader() : nullptr;
        paint.setShader(skShader);
    }

    auto filter = brush.GetFilter();
    ApplyFilter(paint, filter);
    paint.setStyle(SkPaint::kFill_Style);
}

void SkiaPaint::PenToSkPaint(const Pen& pen, SkPaint& paint) const
{
    auto cs = pen.GetColorSpace();
    if (cs != nullptr) {
        auto skColorSpaceImpl = cs->GetImpl<SkiaColorSpace>();
        sk_sp<SkColorSpace> colorSpace = (skColorSpaceImpl != nullptr) ? skColorSpaceImpl->GetColorSpace() : nullptr;
        paint.setColor(SkColor4f::FromColor(pen.GetColor().CastToColorQuad()), colorSpace.get());
    } else {
        paint.setColor(pen.GetColor().CastToColorQuad());
    }

    if (pen.GetBlendMode() != BlendMode::SRC_OVER) {
        paint.setBlendMode(static_cast<SkBlendMode>(pen.GetBlendMode()));
    }

    paint.setStrokeMiter(pen.GetMiterLimit());
    paint.setStrokeWidth(pen.GetWidth());
    paint.setAntiAlias(pen.IsAntiAlias());
    paint.setAlpha(pen.GetAlpha());

    switch (pen.GetCapStyle()) {
        case Pen::CapStyle::FLAT_CAP:
            paint.setStrokeCap(SkPaint::kButt_Cap);
            break;
        case Pen::CapStyle::SQUARE_CAP:
            paint.setStrokeCap(SkPaint::kSquare_Cap);
            break;
        case Pen::CapStyle::ROUND_CAP:
            paint.setStrokeCap(SkPaint::kRound_Cap);
            break;
        default:
            break;
    }

    switch (pen.GetJoinStyle()) {
        case Pen::JoinStyle::MITER_JOIN:
            paint.setStrokeJoin(SkPaint::kMiter_Join);
            break;
        case Pen::JoinStyle::ROUND_JOIN:
            paint.setStrokeJoin(SkPaint::kRound_Join);
            break;
        case Pen::JoinStyle::BEVEL_JOIN:
            paint.setStrokeJoin(SkPaint::kBevel_Join);
            break;
        default:
            break;
    }

    auto p = pen.GetPathEffect();
    if (p != nullptr) {
        auto skPathEffectImpl = p->GetImpl<SkiaPathEffect>();
        sk_sp<SkPathEffect> skPathEffect = (skPathEffectImpl != nullptr) ? skPathEffectImpl->GetPathEffect() : nullptr;
        paint.setPathEffect(skPathEffect);
    }

    auto s = pen.GetShaderEffect();
    if (s != nullptr) {
        auto skShaderImpl = s->GetImpl<SkiaShaderEffect>();
        sk_sp<SkShader> skShader = (skShaderImpl != nullptr) ? skShaderImpl->GetShader() : nullptr;
        paint.setShader(skShader);
    }

    auto filter = pen.GetFilter();
    ApplyFilter(paint, filter);
    paint.setStyle(SkPaint::kStroke_Style);
}

void SkiaPaint::DisableStroke()
{
    stroke_.isEnabled_ = false;
}

void SkiaPaint::DisableFill()
{
    fill_.isEnabled_ = false;
}

SortedPaints& SkiaPaint::GetSortedPaints()
{
    sortedPaints_.count_ = 0;
    if (!fill_.isEnabled_ && !stroke_.isEnabled_) {
        return sortedPaints_;
    }

    if (fill_.isEnabled_ && !stroke_.isEnabled_) {
        GenerateFillPaint();
        sortedPaints_.count_ = 1;
        sortedPaints_.paints_[0] = &fillPaint_;
        return sortedPaints_;
    }

    if (stroke_.isEnabled_ && !fill_.isEnabled_) {
        GenerateStrokePaint();
        sortedPaints_.count_ = 1;
        sortedPaints_.paints_[0] = &strokePaint_;
        return sortedPaints_;
    }

    if (stroke_.paintData_.strokeCore_->extend_ == *fill_.paintData_.fillCore_) {
        GenerateFillAndStrokePaint();
        sortedPaints_.count_ = 1;
        sortedPaints_.paints_[0] = &fillAndStrokePaint_;
    } else if (IsStrokeFirst()) {
        GenerateFillPaint();
        GenerateStrokePaint();
        sortedPaints_.count_ = MAX_PAINTS_NUMBER;
        sortedPaints_.paints_[0] = &strokePaint_;
        sortedPaints_.paints_[1] = &fillPaint_;
    } else {
        GenerateFillPaint();
        GenerateStrokePaint();
        sortedPaints_.count_ = MAX_PAINTS_NUMBER;
        sortedPaints_.paints_[0] = &fillPaint_;
        sortedPaints_.paints_[1] = &strokePaint_;
    }
    return sortedPaints_;
}

void SkiaPaint::SetStrokeFirst(bool isStrokeFirst)
{
    isStrokeFirst_ = isStrokeFirst;
}

bool SkiaPaint::IsStrokeFirst() const
{
    return isStrokeFirst_;
}

void SkiaPaint::GenerateFillPaint()
{
    fillPaint_ = defaultPaint_;
    SkStyleFillCore* fillData = fill_.paintData_.fillCore_;
    fillPaint_.setStyle(SkPaint::kFill_Style);
    ApplyFillCoreToSkPaint(*fillData, fillPaint_);
}

void SkiaPaint::GenerateStrokePaint()
{
    strokePaint_ = defaultPaint_;
    SkStyleStrokeCore* strokeData = stroke_.paintData_.strokeCore_;
    strokePaint_.setStyle(SkPaint::kStroke_Style);
    ApplyStrokeCoreToSkPaint(*strokeData, strokePaint_);
}

void SkiaPaint::GenerateFillAndStrokePaint()
{
    fillAndStrokePaint_ = defaultPaint_;
    SkStyleStrokeCore* strokeData = stroke_.paintData_.strokeCore_;
    fillAndStrokePaint_.setStyle(SkPaint::kStrokeAndFill_Style);
    ApplyStrokeCoreToSkPaint(*strokeData, fillAndStrokePaint_);
}

template <class T>
void SkiaPaint::ApplyBrushOrPenToFillCore(const T& brushOrPen, SkStyleFillCore& fillData)
{
    fillData.antiAlias_ = brushOrPen.IsAntiAlias();
    fillData.alpha_ = brushOrPen.GetAlpha();
    fillData.color_ = brushOrPen.GetColor().CastToColorQuad();
    fillData.blendMode_ = static_cast<SkBlendMode>(brushOrPen.GetBlendMode());

    auto cs = brushOrPen.GetColorSpace();
    if (cs != nullptr) {
        auto skColorSpaceImpl = cs->template GetImpl<SkiaColorSpace>();
        fillData.colorSpace_ = (skColorSpaceImpl != nullptr) ? skColorSpaceImpl->GetColorSpace() : nullptr;
    }

    auto s = brushOrPen.GetShaderEffect();
    if (s != nullptr) {
        auto skShaderImpl = s->template GetImpl<SkiaShaderEffect>();
        fillData.shader_ = (skShaderImpl != nullptr) ? skShaderImpl->GetShader() : nullptr;
    }

    auto filter = brushOrPen.GetFilter();
    auto c = filter.GetColorFilter();
    if (c != nullptr) {
        auto skColorFilterImpl = c->template GetImpl<SkiaColorFilter>();
        fillData.colorFilter_ = (skColorFilterImpl != nullptr) ? skColorFilterImpl->GetColorFilter() : nullptr;
    }

    auto i = filter.GetImageFilter();
    if (i != nullptr) {
        auto skImageFilterImpl = i->template GetImpl<SkiaImageFilter>();
        fillData.imageFilter_ = (skImageFilterImpl != nullptr) ? skImageFilterImpl->GetImageFilter() : nullptr;
    }

    auto m = filter.GetMaskFilter();
    if (m != nullptr) {
        auto skMaskFilterImpl = m->template GetImpl<SkiaMaskFilter>();
        fillData.maskFilter_ = (skMaskFilterImpl != nullptr) ? skMaskFilterImpl->GetMaskFilter() : nullptr;
    }
}

void SkiaPaint::ApplyPenToStrokeCore(const Pen& pen, SkStyleStrokeCore& strokeData)
{
    strokeData.miter_ = pen.GetMiterLimit();
    strokeData.width_ = pen.GetWidth();

    switch (pen.GetCapStyle()) {
        case Pen::CapStyle::FLAT_CAP:
            strokeData.cap_ = SkPaint::kButt_Cap;
            break;
        case Pen::CapStyle::SQUARE_CAP:
            strokeData.cap_ = SkPaint::kSquare_Cap;
            break;
        case Pen::CapStyle::ROUND_CAP:
            strokeData.cap_ = SkPaint::kRound_Cap;
            break;
        default:
            break;
    }

    switch (pen.GetJoinStyle()) {
        case Pen::JoinStyle::MITER_JOIN:
            strokeData.join_ = SkPaint::kMiter_Join;
            break;
        case Pen::JoinStyle::ROUND_JOIN:
            strokeData.join_ = SkPaint::kRound_Join;
            break;
        case Pen::JoinStyle::BEVEL_JOIN:
            strokeData.join_ = SkPaint::kBevel_Join;
            break;
        default:
            break;
    }

    auto p = pen.GetPathEffect();
    if (p != nullptr) {
        auto skPathEffectImpl = p->GetImpl<SkiaPathEffect>();
        strokeData.pathEffect_ = (skPathEffectImpl != nullptr) ? skPathEffectImpl->GetPathEffect() : nullptr;
    }

    ApplyBrushOrPenToFillCore(pen, strokeData.extend_);
}

void SkiaPaint::ApplyFillCoreToSkPaint(const SkStyleFillCore& fillCore, SkPaint& paint)
{
    paint.setAntiAlias(fillCore.antiAlias_);
    paint.setAlpha(fillCore.alpha_);

    if (fillCore.blendMode_ != SkBlendMode::kSrcOver) {
        paint.setBlendMode(fillCore.blendMode_);
    }

    if (fillCore.colorSpace_) {
        paint.setColor(SkColor4f::FromColor(fillCore.color_), fillCore.colorSpace_.get());
    } else {
        paint.setColor(fillCore.color_);
    }

    if (fillCore.shader_) {
        paint.setShader(fillCore.shader_);
    }

    if (fillCore.colorFilter_) {
        paint.setColorFilter(fillCore.colorFilter_);
    }

    if (fillCore.imageFilter_) {
        paint.setImageFilter(fillCore.imageFilter_);
    }

    if (fillCore.maskFilter_) {
        paint.setMaskFilter(fillCore.maskFilter_);
    }
}

void SkiaPaint::ApplyStrokeCoreToSkPaint(const SkStyleStrokeCore& strokeCore, SkPaint& paint)
{
    ApplyFillCoreToSkPaint(strokeCore.extend_, paint);

    if (strokeCore.miter_ != SkPaintDefaults_MiterLimit) {
        paint.setStrokeMiter(strokeCore.miter_);
    }

    paint.setStrokeWidth(strokeCore.width_);

    if (strokeCore.cap_ != SkPaint::kDefault_Cap) {
        paint.setStrokeCap(strokeCore.cap_);
    }

    if (strokeCore.join_ != SkPaint::kDefault_Join) {
        paint.setStrokeJoin(strokeCore.join_);
    }

    if (strokeCore.pathEffect_) {
        paint.setPathEffect(strokeCore.pathEffect_);
    }
}

void SkiaPaint::ApplyFilter(SkPaint& paint, const Filter& filter) const
{
    auto c = filter.GetColorFilter();
    if (c != nullptr) {
        auto skColorFilterImpl = c->GetImpl<SkiaColorFilter>();
        sk_sp<SkColorFilter> colorFilter =
            (skColorFilterImpl != nullptr) ? skColorFilterImpl->GetColorFilter() : nullptr;
        paint.setColorFilter(colorFilter);
    }

    auto i = filter.GetImageFilter();
    if (i != nullptr) {
        auto skImageFilterImpl = i->GetImpl<SkiaImageFilter>();
        sk_sp<SkImageFilter> imageFilter =
            (skImageFilterImpl != nullptr) ? skImageFilterImpl->GetImageFilter() : nullptr;
        paint.setImageFilter(imageFilter);
    }

    auto m = filter.GetMaskFilter();
    if (m != nullptr) {
        auto skMaskFilterImpl = m->GetImpl<SkiaMaskFilter>();
        sk_sp<SkMaskFilter> maskFilter = (skMaskFilterImpl != nullptr) ? skMaskFilterImpl->GetMaskFilter() : nullptr;
        paint.setMaskFilter(maskFilter);
    }
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS