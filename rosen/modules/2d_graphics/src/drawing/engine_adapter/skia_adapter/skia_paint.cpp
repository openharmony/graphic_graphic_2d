/*
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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

#include "effect/image_filter_lazy.h"
#include "effect/shader_effect_lazy.h"
#include "skia_blender.h"
#include "skia_color_filter.h"
#include "skia_color_space.h"
#include "skia_convert_utils.h"
#include "skia_image_filter.h"
#include "skia_mask_filter.h"
#include "skia_matrix.h"
#include "skia_path.h"
#include "skia_path_effect.h"
#include "skia_shader_effect.h"
#ifdef USE_M133_SKIA
#include "include/core/SkPathUtils.h"
#endif

namespace OHOS {
namespace Rosen {
namespace Drawing {

// Helper function to apply shader effect on SkPaint with lazy handling
static void ApplyShaderEffect(SkPaint& skPaint, const ShaderEffect* shaderEffect)
{
    if (!shaderEffect) {
        return;
    }

    const ShaderEffect* actualShader = shaderEffect;
    // Handle Lazy ShaderEffect: materialize to get actual implementation
    if (shaderEffect->IsLazy()) {
        auto lazyShader = const_cast<ShaderEffectLazy*>(
            static_cast<const ShaderEffectLazy*>(shaderEffect));
        actualShader = lazyShader->Materialize().get();
    }

    if (actualShader) {
        auto skShaderImpl = actualShader->GetImpl<SkiaShaderEffect>();
        if (skShaderImpl) {
            skPaint.setShader(skShaderImpl->GetShader());
        }
    }
}

// Helper function to apply image filter on SkPaint with lazy handling
static void ApplyImageFilter(SkPaint& skPaint, const ImageFilter* imageFilter)
{
    if (!imageFilter) {
        return;
    }

    const ImageFilter* actualImageFilter = imageFilter;
    // Handle Lazy ImageFilter: materialize to get actual implementation
    if (imageFilter->IsLazy()) {
        auto lazyImageFilter = const_cast<ImageFilterLazy*>(
            static_cast<const ImageFilterLazy*>(imageFilter));
        actualImageFilter = lazyImageFilter->Materialize().get();
    }

    if (actualImageFilter) {
        auto skImageFilterImpl = actualImageFilter->GetImpl<SkiaImageFilter>();
        if (skImageFilterImpl) {
            skPaint.setImageFilter(skImageFilterImpl->GetImageFilter());
        }
    }
}

void SkiaPaint::BrushToSkPaint(const Brush& brush, SkPaint& paint)
{
    if (const ColorSpace* cs = brush.GetColorSpacePtr()) {
        auto skColorSpaceImpl = cs->GetImpl<SkiaColorSpace>();
        sk_sp<SkColorSpace> colorSpace = (skColorSpaceImpl != nullptr) ? skColorSpaceImpl->GetColorSpace() : nullptr;
        paint.setColor(SkColor4f::FromColor(brush.GetColor().CastToColorQuad()), colorSpace.get());
    } else {
        paint.setColor(brush.GetColor().CastToColorQuad());
    }

    if (brush.GetAlpha() != Color::RGB_MAX) {
        paint.setAlpha(brush.GetAlpha());
    }
    paint.setAntiAlias(brush.IsAntiAlias());

    ApplyShaderEffect(paint, brush.GetShaderEffectPtr());

    if (const Blender* b = brush.GetBlenderPtr()) {
        if (SkiaBlender* skBlenderImpl = b->GetImpl<SkiaBlender>()) {
            paint.setBlender(skBlenderImpl->GetBlender());
        }
    } else if (brush.GetBlendMode() != BlendMode::SRC_OVER) {
        paint.setBlendMode(static_cast<SkBlendMode>(brush.GetBlendMode()));
    }

    if (brush.HasFilter()) {
        ApplyFilter(paint, brush.GetFilter());
    }
    paint.setStyle(SkPaint::kFill_Style);
}

void SkiaPaint::PenToSkPaint(const Pen& pen, SkPaint& paint)
{
    if (const ColorSpace* cs = pen.GetColorSpacePtr()) {
        auto skColorSpaceImpl = cs->GetImpl<SkiaColorSpace>();
        sk_sp<SkColorSpace> colorSpace = (skColorSpaceImpl != nullptr) ? skColorSpaceImpl->GetColorSpace() : nullptr;
        paint.setColor(SkColor4f::FromColor(pen.GetColor().CastToColorQuad()), colorSpace.get());
    } else {
        paint.setColor(pen.GetColor().CastToColorQuad());
    }

    paint.setStrokeMiter(pen.GetMiterLimit());
    paint.setStrokeWidth(pen.GetWidth());
    paint.setAntiAlias(pen.IsAntiAlias());
    paint.setAlpha(pen.GetAlpha());
    paint.setStrokeCap(static_cast<SkPaint::Cap>(pen.GetCapStyle()));
    paint.setStrokeJoin(static_cast<SkPaint::Join>(pen.GetJoinStyle()));

    if (const PathEffect* pe = pen.GetPathEffectPtr()) {
        if (SkiaPathEffect* skPathEffectImpl = pe->GetImpl<SkiaPathEffect>()) {
            paint.setPathEffect(skPathEffectImpl->GetPathEffect());
        }
    }

    ApplyShaderEffect(paint, pen.GetShaderEffectPtr());

    if (const Blender* blender = pen.GetBlenderPtr()) {
        if (SkiaBlender* skBlenderImpl = blender->GetImpl<SkiaBlender>()) {
            paint.setBlender(skBlenderImpl->GetBlender());
        }
    } else if (pen.GetBlendMode() != BlendMode::SRC_OVER) {
        paint.setBlendMode(static_cast<SkBlendMode>(pen.GetBlendMode()));
    }

    if (pen.HasFilter()) {
        ApplyFilter(paint, pen.GetFilter());
    }
    paint.setStyle(SkPaint::kStroke_Style);
}

void SkiaPaint::PaintToSkPaint(const Paint& paint, SkPaint& skPaint)
{
    skPaint.setStyle(static_cast<SkPaint::Style>(paint.GetStyle() - Paint::PaintStyle::PAINT_FILL));

    if (const ColorSpace* cs = paint.GetColorSpacePtr()) {
        auto skColorSpaceImpl = cs->GetImpl<SkiaColorSpace>();
        sk_sp<SkColorSpace> colorSpace = (skColorSpaceImpl != nullptr) ? skColorSpaceImpl->GetColorSpace() : nullptr;
        skPaint.setColor(SkColor4f::FromColor(paint.GetColor().CastToColorQuad()), colorSpace.get());
    } else {
        skPaint.setColor(paint.GetColor().CastToColorQuad());
    }

    skPaint.setAntiAlias(paint.IsAntiAlias());

    ApplyShaderEffect(skPaint, paint.GetShaderEffectPtr());

    if (paint.HasFilter()) {
        ApplyFilter(skPaint, paint.GetFilter());
    }

    if (paint.HasStrokeStyle()) {
        ApplyStrokeParam(paint, skPaint);
    }

    if (const Blender* blender = paint.GetBlenderPtr()) {
        if (SkiaBlender* skBlenderImpl = blender->GetImpl<SkiaBlender>()) {
            skPaint.setBlender(skBlenderImpl->GetBlender());
        }
    } else if (paint.GetBlendMode() != BlendMode::SRC_OVER) {
        skPaint.setBlendMode(static_cast<SkBlendMode>(paint.GetBlendMode()));
    }
}

void SkiaPaint::ApplyStrokeParam(const Paint& paint, SkPaint& skPaint)
{
    if (!IsScalarAlmostEqual(paint.GetMiterLimit(), Paint::DEFAULT_MITER_VAL)) {
        skPaint.setStrokeMiter(paint.GetMiterLimit());
    }
    skPaint.setStrokeWidth(paint.GetWidth());
    if (paint.GetCapStyle() != Pen::CapStyle::DEFAULT_CAP) {
        skPaint.setStrokeCap(static_cast<SkPaint::Cap>(paint.GetCapStyle()));
    }
    if (paint.GetJoinStyle() != Pen::JoinStyle::DEFAULT_JOIN) {
        skPaint.setStrokeJoin(static_cast<SkPaint::Join>(paint.GetJoinStyle()));
    }
    if (const PathEffect* pe = paint.GetPathEffectPtr()) {
        if (SkiaPathEffect* skPathEffectImpl = pe->GetImpl<SkiaPathEffect>()) {
            skPaint.setPathEffect(skPathEffectImpl->GetPathEffect());
        }
    }
}

SkiaPaint::SkiaPaint() noexcept {}

SkiaPaint::~SkiaPaint() {}

void SkiaPaint::ApplyFilter(SkPaint& paint, const Filter& filter)
{
    if (const ColorFilter* cs = filter.GetColorFilterPtr()) {
        if (SkiaColorFilter* skColorFilterImpl = cs->GetImpl<SkiaColorFilter>()) {
            paint.setColorFilter(skColorFilterImpl->GetColorFilter());
        }
    }

    ApplyImageFilter(paint, filter.GetImageFilterPtr());

    if (const MaskFilter* mf = filter.GetMaskFilterPtr()) {
        if (SkiaMaskFilter* skMaskFilterImpl = mf->GetImpl<SkiaMaskFilter>()) {
            paint.setMaskFilter(skMaskFilterImpl->GetMaskFilter());
        }
    }
}

bool SkiaPaint::GetFillPath(const Pen& pen, const Path& src, Path& dst, const Rect* rect, const Matrix& matrix)
{
    SkPaint skPaint;
    PenToSkPaint(pen, skPaint);
    auto srcPathImpl = src.GetImpl<SkiaPath>();
    auto dstPathImpl = dst.GetImpl<SkiaPath>();
    auto matrixImpl = matrix.GetImpl<SkiaMatrix>();
    if (!srcPathImpl || !dstPathImpl || !matrixImpl) {
        return false;
    }
#ifdef USE_M133_SKIA
    if (!rect) {
        return skpathutils::FillPathWithPaint(srcPathImpl->GetMutablePath(), skPaint, &dstPathImpl->GetMutablePath(),
                                              nullptr, matrixImpl->ExportMatrix());
    }
    SkRect skRect = SkRect::MakeLTRB(rect->GetLeft(), rect->GetTop(), rect->GetRight(), rect->GetBottom());
    return skpathutils::FillPathWithPaint(srcPathImpl->GetMutablePath(), skPaint, &dstPathImpl->GetMutablePath(),
                                          &skRect, matrixImpl->ExportMatrix());
#else
    if (!rect) {
        return skPaint.getFillPath(srcPathImpl->GetMutablePath(), &dstPathImpl->GetMutablePath(),
                                   nullptr, matrixImpl->ExportMatrix());
    }
    SkRect skRect = SkRect::MakeLTRB(rect->GetLeft(), rect->GetTop(), rect->GetRight(), rect->GetBottom());
    return skPaint.getFillPath(srcPathImpl->GetMutablePath(), &dstPathImpl->GetMutablePath(),
                               &skRect, matrixImpl->ExportMatrix());
#endif
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
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS