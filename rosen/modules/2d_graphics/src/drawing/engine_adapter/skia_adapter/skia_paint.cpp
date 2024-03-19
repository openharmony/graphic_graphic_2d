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

#include "skia_blender.h"
#include "skia_color_filter.h"
#include "skia_color_space.h"
#include "skia_convert_utils.h"
#include "skia_image_filter.h"
#include "skia_mask_filter.h"
#include "skia_path.h"
#include "skia_path_effect.h"
#include "skia_shader_effect.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
void SkiaPaint::BrushToSkPaint(const Brush& brush, SkPaint& paint)
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

    if (brush.GetBlender() != nullptr) {
        auto skBlenderImpl = brush.GetBlender()->GetImpl<SkiaBlender>();
        sk_sp<SkBlender> skBlender = (skBlenderImpl != nullptr) ? skBlenderImpl->GetBlender() : nullptr;
        paint.setBlender(skBlender);
    }

    auto filter = brush.GetFilter();
    ApplyFilter(paint, filter);
    paint.setStyle(SkPaint::kFill_Style);
}

void SkiaPaint::PenToSkPaint(const Pen& pen, SkPaint& paint)
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

void SkiaPaint::PaintToSkPaint(const Paint& paint, SkPaint& skPaint)
{
    switch (paint.GetStyle()) {
        case Paint::PaintStyle::PAINT_FILL:
            skPaint.setStyle(SkPaint::kFill_Style);
            break;
        case Paint::PaintStyle::PAINT_STROKE:
            skPaint.setStyle(SkPaint::kStroke_Style);
            break;
        case Paint::PaintStyle::PAINT_FILL_STROKE:
            skPaint.setStyle(SkPaint::kStrokeAndFill_Style);
            break;
        default:
            break;
    }

    auto cs = paint.GetColorSpace();
    if (cs != nullptr) {
        auto skColorSpaceImpl = cs->GetImpl<SkiaColorSpace>();
        sk_sp<SkColorSpace> colorSpace = (skColorSpaceImpl != nullptr) ? skColorSpaceImpl->GetColorSpace() : nullptr;
        skPaint.setColor(SkColor4f::FromColor(paint.GetColor().CastToColorQuad()), colorSpace.get());
    } else {
        skPaint.setColor(paint.GetColor().CastToColorQuad());
    }

    skPaint.setAntiAlias(paint.IsAntiAlias());
    if (paint.GetBlendMode() != BlendMode::SRC_OVER) {
        skPaint.setBlendMode(static_cast<SkBlendMode>(paint.GetBlendMode()));
    }

    auto s = paint.GetShaderEffect();
    if (s != nullptr) {
        auto skShaderImpl = s->GetImpl<SkiaShaderEffect>();
        sk_sp<SkShader> skShader = (skShaderImpl != nullptr) ? skShaderImpl->GetShader() : nullptr;
        skPaint.setShader(skShader);
    }

    if (paint.HasFilter()) {
        auto filter = paint.GetFilter();
        ApplyFilter(skPaint, filter);
    }

    if (paint.HasStrokeStyle()) {
        ApplyStrokeParam(paint, skPaint);
    }
}

void SkiaPaint::ApplyStrokeParam(const Paint& paint, SkPaint& skPaint)
{
    skPaint.setStrokeMiter(paint.GetMiterLimit());
    skPaint.setStrokeWidth(paint.GetWidth());

    switch (paint.GetCapStyle()) {
        case Pen::CapStyle::FLAT_CAP:
            skPaint.setStrokeCap(SkPaint::kButt_Cap);
            break;
        case Pen::CapStyle::SQUARE_CAP:
            skPaint.setStrokeCap(SkPaint::kSquare_Cap);
            break;
        case Pen::CapStyle::ROUND_CAP:
            skPaint.setStrokeCap(SkPaint::kRound_Cap);
            break;
        default:
            break;
    }

    switch (paint.GetJoinStyle()) {
        case Pen::JoinStyle::MITER_JOIN:
            skPaint.setStrokeJoin(SkPaint::kMiter_Join);
            break;
        case Pen::JoinStyle::ROUND_JOIN:
            skPaint.setStrokeJoin(SkPaint::kRound_Join);
            break;
        case Pen::JoinStyle::BEVEL_JOIN:
            skPaint.setStrokeJoin(SkPaint::kBevel_Join);
            break;
        default:
            break;
    }

    auto p = paint.GetPathEffect();
    if (p != nullptr) {
        auto skPathEffectImpl = p->GetImpl<SkiaPathEffect>();
        sk_sp<SkPathEffect> skPathEffect = (skPathEffectImpl != nullptr) ? skPathEffectImpl->GetPathEffect() : nullptr;
        skPaint.setPathEffect(skPathEffect);
    }
}

SkiaPaint::SkiaPaint() noexcept {}

SkiaPaint::~SkiaPaint() {}

void SkiaPaint::ApplyPaint(const Paint& paint)
{
    if (paintInUse_ >= MAX_PAINTS_NUMBER || !paint.IsValid()) {
        return;
    }
    SkPaint& skPaint = paints_[paintInUse_];
    skPaint = defaultPaint_;
    PaintToSkPaint(paint, skPaint);
    paintInUse_++;
}

SortedPaints& SkiaPaint::GetSortedPaints()
{
    sortedPaints_.count_ = paintInUse_;
    for (int i = 0; i < paintInUse_; i++) {
        sortedPaints_.paints_[i] = &paints_[i];
    }
    paintInUse_ = 0;
    return sortedPaints_;
}

void SkiaPaint::ApplyFilter(SkPaint& paint, const Filter& filter)
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

bool SkiaPaint::CanComputeFastBounds(const Brush& brush)
{
    SkPaint skPaint;
    BrushToSkPaint(brush, skPaint);
    return skPaint.canComputeFastBounds();
}

const Rect& SkiaPaint::ComputeFastBounds(const Brush& brush, const Rect& orig, Rect* storage)
{
    if (storage == nullptr) {
        return orig;
    }
    SkPaint skPaint;
    BrushToSkPaint(brush, skPaint);
    SkRect skOrig, skStorage;
    SkiaConvertUtils::DrawingRectCastToSkRect(orig, skOrig);
    SkiaConvertUtils::DrawingRectCastToSkRect(*storage, skStorage);
    const SkRect& skRect = skPaint.computeFastBounds(skOrig, &skStorage);
    SkiaConvertUtils::SkRectCastToDrawingRect(skStorage, *storage);
    if (&skRect == &skOrig) {
        return orig;
    }
    return *storage;
}

bool SkiaPaint::AsBlendMode(const Brush& brush)
{
    SkPaint skPaint;
    BrushToSkPaint(brush, skPaint);
    return skPaint.asBlendMode().has_value();
}

void SkiaPaint::Reset()
{
    paintInUse_ = 0;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS