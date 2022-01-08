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
    : stroke_(std::make_shared<PaintData>()), fill_(std::make_shared<PaintData>()), isStrokeFirst_(false) {}

void SkiaPaint::ApplyBrushToFill(const Brush& brush)
{
    fill_->paint.reset();
    fill_->isEnabled = true;
    BrushToSkPaint(brush, fill_->paint);
}

void SkiaPaint::ApplyPenToStroke(const Pen& pen)
{
    stroke_->paint.reset();
    stroke_->isEnabled = true;
    PenToSkPaint(pen, stroke_->paint);
}

void SkiaPaint::BrushToSkPaint(const Brush& brush, SkPaint &paint)
{
    paint.setColor(brush.GetColor().CastToColorQuad());

    auto cs = brush.GetColorSpace();
    if (cs != nullptr) {
        auto skColorSpaceImpl = cs->GetImpl<SkiaColorSpace>();
        sk_sp<SkColorSpace> colorSpace = (skColorSpaceImpl != nullptr) ? skColorSpaceImpl->GetColorSpace() : nullptr;
        paint.setColor(paint.getColor4f(), colorSpace.get());
    }

    paint.setAlpha(brush.GetAlpha());
    paint.setBlendMode(static_cast<SkBlendMode>(brush.GetBlendMode()));

    auto s = brush.GetShaderEffect();
    if (s != nullptr) {
        auto skShaderImpl = s->GetImpl<SkiaShaderEffect>();
        sk_sp<SkShader> skShader = (skShaderImpl != nullptr) ? skShaderImpl->GetShader() : nullptr;
        paint.setShader(skShader);
    } else {
        paint.setShader(nullptr);
    }

    auto filter = brush.GetFilter();
    ApplyFilter(paint, filter);
    paint.setStyle(SkPaint::kFill_Style);
}

void SkiaPaint::PenToSkPaint(const Pen& pen, SkPaint &paint)
{
    paint.setColor(pen.GetColor().CastToColorQuad());

    auto cs = pen.GetColorSpace();
    if (cs != nullptr) {
        auto skColorSpaceImpl = cs->GetImpl<SkiaColorSpace>();
        sk_sp<SkColorSpace> colorSpace = (skColorSpaceImpl != nullptr) ? skColorSpaceImpl->GetColorSpace() : nullptr;
        paint.setColor(paint.getColor4f(), colorSpace.get());
    }

    paint.setStrokeWidth(pen.GetWidth());
    paint.setAntiAlias(pen.IsAntiAlias());
    paint.setAlpha(pen.GetAlpha());
    paint.setBlendMode(static_cast<SkBlendMode>(pen.GetBlendMode()));

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
    } else {
        paint.setPathEffect(nullptr);
    }

    auto s = pen.GetShaderEffect();
    if (s != nullptr) {
        auto skShaderImpl = s->GetImpl<SkiaShaderEffect>();
        sk_sp<SkShader> skShader = (skShaderImpl != nullptr) ? skShaderImpl->GetShader() : nullptr;
        paint.setShader(skShader);
    } else {
        paint.setShader(nullptr);
    }

    auto filter = pen.GetFilter();
    ApplyFilter(paint, filter);
    paint.setStyle(SkPaint::kStroke_Style);
}

void SkiaPaint::DisableStroke()
{
    stroke_->isEnabled = false;
}

void SkiaPaint::DisableFill()
{
    fill_->isEnabled = false;
}

std::vector<std::shared_ptr<PaintData>> SkiaPaint::GetSortedPaints()
{
    std::vector<std::shared_ptr<PaintData>> paints;
    if (IsStrokeFirst() && stroke_->isEnabled && fill_->isEnabled)
    {
        paints.push_back(stroke_);
        paints.push_back(fill_);
        return paints;
    }

    if (stroke_->isEnabled) {
        paints.push_back(stroke_);
    }
    if (fill_->isEnabled) {
        paints.push_back(fill_);
    }
    return paints;
}

void SkiaPaint::SetStrokeFirst(bool isStrokeFirst)
{
    isStrokeFirst_ = isStrokeFirst;
}

bool SkiaPaint::IsStrokeFirst()
{
    return isStrokeFirst_;
}

void SkiaPaint::ApplyFilter(SkPaint &paint, const Filter& filter)
{
#if !defined(USE_CANVASKIT0310_SKIA)
    switch (filter.GetFilterQuality()) {
        case Filter::FilterQuality::LOW:
            paint.setFilterQuality(SkFilterQuality::kLow_SkFilterQuality);
            break;
        case Filter::FilterQuality::MEDIUM:
            paint.setFilterQuality(SkFilterQuality::kMedium_SkFilterQuality);
            break;
        case Filter::FilterQuality::HIGH:
            paint.setFilterQuality(SkFilterQuality::kHigh_SkFilterQuality);
            break;
        case Filter::FilterQuality::NONE:
            paint.setFilterQuality(SkFilterQuality::kNone_SkFilterQuality);
            break;
        default:
            break;
    }
#endif

    auto c = filter.GetColorFilter();
    if (c != nullptr) {
        auto skColorFilterImpl = c->GetImpl<SkiaColorFilter>();
        sk_sp<SkColorFilter> colorFilter =
            (skColorFilterImpl != nullptr) ? skColorFilterImpl->GetColorFilter() : nullptr;
        paint.setColorFilter(colorFilter);
    } else {
        paint.setColorFilter(nullptr);
    }

    auto i = filter.GetImageFilter();
    if (i != nullptr) {
        auto skImageFilterImpl = i->GetImpl<SkiaImageFilter>();
        sk_sp<SkImageFilter> imageFilter =
            (skImageFilterImpl != nullptr) ? skImageFilterImpl->GetImageFilter() : nullptr;
        paint.setImageFilter(imageFilter);
    } else {
        paint.setImageFilter(nullptr);
    }

    auto m = filter.GetMaskFilter();
    if (m != nullptr) {
        auto skMaskFilterImpl = m->GetImpl<SkiaMaskFilter>();
        sk_sp<SkMaskFilter> maskFilter = (skMaskFilterImpl != nullptr) ? skMaskFilterImpl->GetMaskFilter() : nullptr;
        paint.setMaskFilter(maskFilter);
    } else {
        paint.setMaskFilter(nullptr);
    }
}
}
}
}