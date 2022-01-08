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

#include "skia_canvas.h"

#include "image/bitmap.h"
#include "skia_path.h"

#include "include/core/SkImage.h"

#include "pixel_map.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
SkiaCanvas::SkiaCanvas() : skiaCanvas_(std::make_shared<SkCanvas>()), skiaPaint_() {}

SkiaCanvas::~SkiaCanvas() {}

const std::shared_ptr<SkCanvas> SkiaCanvas::ExportSkiaCanvas() const
{
    return skiaCanvas_;
}

void SkiaCanvas::Bind(const Bitmap& bitmap)
{
    auto skBitmapImpl = bitmap.GetImpl<SkiaBitmap>();
    if (skBitmapImpl != nullptr) {
        skiaCanvas_ = std::make_shared<SkCanvas>(skBitmapImpl->ExportSkiaBitmap());
    }
}

void SkiaCanvas::DrawPoint(const Point& point)
{
    for (auto d : skiaPaint_.GetSortedPaints()) {
        if (d != nullptr) {
            skiaCanvas_->drawPoint(SkPoint::Make(point.GetX(), point.GetY()), d->paint);
        }
    }
}

void SkiaCanvas::DrawLine(const Point& startPt, const Point& endPt)
{
    for (auto d : skiaPaint_.GetSortedPaints()) {
        if (d != nullptr) {
            skiaCanvas_->drawLine(SkPoint::Make(startPt.GetX(), startPt.GetY()),
                SkPoint::Make(endPt.GetX(), endPt.GetY()), d->paint);
        }
    }
}

void SkiaCanvas::DrawRect(const Rect& rect)
{
    SkRect r = SkRect::MakeLTRB(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
    for (auto d : skiaPaint_.GetSortedPaints()) {
        if (d != nullptr) {
            skiaCanvas_->drawRect(r, d->paint);
        }
    }
}

void SkiaCanvas::DrawRoundRect(const RoundRect& roundRect)
{
    SkRRect rRect;
    RoundRectCastToSkRRect(roundRect, rRect);
    for (auto d : skiaPaint_.GetSortedPaints()) {
        if (d != nullptr) {
            skiaCanvas_->drawRRect(rRect, d->paint);
        }
    }
}

void SkiaCanvas::DrawNestedRoundRect(const RoundRect& outer, const RoundRect& inner)
{
    SkRRect outerRRect;
    RoundRectCastToSkRRect(outer, outerRRect);

    SkRRect innerRRect;
    RoundRectCastToSkRRect(inner, innerRRect);

    for (auto d : skiaPaint_.GetSortedPaints()) {
        if (d != nullptr) {
            skiaCanvas_->drawDRRect(outerRRect, innerRRect, d->paint);
        }
    }
}

void SkiaCanvas::DrawArc(const Rect& oval, scalar startAngle, scalar sweepAngle)
{
    SkRect arcRect = SkRect::MakeLTRB(oval.GetLeft(), oval.GetTop(), oval.GetRight(), oval.GetBottom());
    for (auto d : skiaPaint_.GetSortedPaints()) {
        if (d != nullptr) {
            skiaCanvas_->drawArc(arcRect, startAngle, sweepAngle, false, d->paint);
        }
    }
}

void SkiaCanvas::DrawPie(const Rect& oval, scalar startAngle, scalar sweepAngle)
{
    SkRect pieRect = SkRect::MakeLTRB(oval.GetLeft(), oval.GetTop(), oval.GetRight(), oval.GetBottom());
    for (auto d : skiaPaint_.GetSortedPaints()) {
        if (d != nullptr) {
            skiaCanvas_->drawArc(pieRect, startAngle, sweepAngle, true, d->paint);
        }
    }
}

void SkiaCanvas::DrawOval(const Rect& oval)
{
    SkRect ovalRect = SkRect::MakeLTRB(oval.GetLeft(), oval.GetTop(), oval.GetRight(), oval.GetBottom());
    for (auto d : skiaPaint_.GetSortedPaints()) {
        if (d != nullptr) {
            skiaCanvas_->drawOval(ovalRect, d->paint);
        }
    }
}

void SkiaCanvas::DrawCircle(const Point& centerPt, scalar radius)
{
    for (auto d : skiaPaint_.GetSortedPaints()) {
        if (d != nullptr) {
            skiaCanvas_->drawCircle(centerPt.GetX(), centerPt.GetY(), radius, d->paint);
        }
    }
}

void SkiaCanvas::DrawPath(const Path& path)
{
    auto skPathImpl = path.GetImpl<SkiaPath>();
    for (auto d : skiaPaint_.GetSortedPaints()) {
        if (skPathImpl != nullptr && d != nullptr) {
            skiaCanvas_->drawPath(skPathImpl->GetPath(), d->paint);
        }
    }
}

void SkiaCanvas::DrawBackground(const Brush& brush)
{
    SkPaint paint;
    skiaPaint_.BrushToSkPaint(brush, paint);
    skiaCanvas_->drawPaint(paint);
}

void SkiaCanvas::DrawShadow(const Path& path, const Point3& planeParams, const Point3& devLightPos,
                            scalar lightRadius, Color ambientColor, Color spotColor, ShadowFlags flag)
{
    auto skPathImpl = path.GetImpl<SkiaPath>();
    SkPoint3 point1 =  SkPoint3::Make(planeParams.GetX(), planeParams.GetY(), planeParams.GetZ());
    SkPoint3 point2 =  SkPoint3::Make(devLightPos.GetX(), devLightPos.GetY(), devLightPos.GetZ());
    SkColor color1 = ambientColor.CastToColorQuad();
    SkColor color2 = spotColor.CastToColorQuad();
    SkShadowFlags flags = static_cast<SkShadowFlags>(flag);
    if (skPathImpl != nullptr) {
        SkShadowUtils::DrawShadow(skiaCanvas_.get(), skPathImpl->GetPath(), point1, point2, color1, color2, flags);
    }
}

void SkiaCanvas::DrawBitmap(const Bitmap& bitmap, const scalar px, const scalar py)
{
    SkBitmap bmp;

    auto skBitmapImpl = bitmap.GetImpl<SkiaBitmap>();
    if (skBitmapImpl != nullptr) {
        bmp = skBitmapImpl->ExportSkiaBitmap();
    }

    auto paints = skiaPaint_.GetSortedPaints();
    if (paints.empty()) {
#if defined(USE_CANVASKIT0310_SKIA)
        skiaCanvas_->drawImage(bmp.asImage(), px, py);
#else
        skiaCanvas_->drawBitmap(bmp, px, py);
#endif
         return;
    }

    for (auto d : skiaPaint_.GetSortedPaints()) {
        if (d != nullptr) {
#if defined(USE_CANVASKIT0310_SKIA)
            skiaCanvas_->drawImage(bmp.asImage(), px, py, SkSamplingOptions(), &d->paint);
#else
            skiaCanvas_->drawBitmap(bmp, px, py, &d->paint);
#endif
        }
    }
}

static sk_sp<SkColorSpace> ColorSpaceToSkColorSpace(Media::PixelMap& pixmap)
{
    return SkColorSpace::MakeSRGB();
}

static SkAlphaType AlphaTypeToSkAlphaType(Media::AlphaType alphaType)
{
    switch (alphaType) {
        case Media::AlphaType::IMAGE_ALPHA_TYPE_UNKNOWN:
            return SkAlphaType::kUnknown_SkAlphaType;
        case Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE:
            return SkAlphaType::kOpaque_SkAlphaType;
        case Media::AlphaType::IMAGE_ALPHA_TYPE_PREMUL:
            return SkAlphaType::kPremul_SkAlphaType;
        case Media::AlphaType::IMAGE_ALPHA_TYPE_UNPREMUL:
            return SkAlphaType::kUnpremul_SkAlphaType;
        default:
            return SkAlphaType::kUnknown_SkAlphaType;
    }
}

static SkColorType PixelFormatToSkColorType(Media::PixelFormat format)
{
    switch (format) {
        case Media::PixelFormat::RGB_565:
            return SkColorType::kRGB_565_SkColorType;
        case Media::PixelFormat::RGBA_8888:
            return SkColorType::kRGBA_8888_SkColorType;
        case Media::PixelFormat::BGRA_8888:
            return SkColorType::kBGRA_8888_SkColorType;
        case Media::PixelFormat::ALPHA_8:
            return SkColorType::kAlpha_8_SkColorType;
        case Media::PixelFormat::RGBA_F16:
            return SkColorType::kRGBA_F16_SkColorType;
        case Media::PixelFormat::UNKNOWN:
        case Media::PixelFormat::ARGB_8888:
        case Media::PixelFormat::RGB_888:
        case Media::PixelFormat::NV21:
        case Media::PixelFormat::NV12:
        case Media::PixelFormat::CMYK:
        default:
            return SkColorType::kUnknown_SkColorType;
    }
}

static SkImageInfo MakeSkImageInfoFromPixelMap(Media::PixelMap& pixelMap)
{
    SkColorType ct = PixelFormatToSkColorType(pixelMap.GetPixelFormat());
    SkAlphaType at = AlphaTypeToSkAlphaType(pixelMap.GetAlphaType());
    sk_sp<SkColorSpace> cs = ColorSpaceToSkColorSpace(pixelMap);
    LOGI("SkColorType %{pubilic}d, SkAlphaType %{public}d", ct, at);
    return SkImageInfo::Make(pixelMap.GetWidth(), pixelMap.GetHeight(), ct, at, cs);
}

void SkiaCanvas::DrawBitmap(Media::PixelMap& pixelMap, const scalar px, const scalar py)
{
    if (pixelMap.GetPixels() == nullptr) {
        LOGE("PutPixelMap failed, pixelMap data invalid");
        return;
    }
    SkBitmap bitmap;
    auto imageInfo = MakeSkImageInfoFromPixelMap(pixelMap);
    bitmap.installPixels(imageInfo, (void*) pixelMap.GetPixels(), pixelMap.GetRowBytes());

    auto paints = skiaPaint_.GetSortedPaints();
    if (paints.empty()) {
#if defined(USE_CANVASKIT0310_SKIA)
        skiaCanvas_->drawImage(bitmap.asImage(), px, py);
#else
        skiaCanvas_->drawBitmap(bitmap, px, py);
#endif
         return;
    }

    for (auto d : paints) {
        if (d != nullptr) {
#if defined(USE_CANVASKIT0310_SKIA)
            skiaCanvas_->drawImage(bitmap.asImage(), px, py, SkSamplingOptions(), &d->paint);
#else
            skiaCanvas_->drawBitmap(bitmap, px, py, &d->paint);
#endif
        }
    }
}

void SkiaCanvas::DrawImage() {} // TODO...

void SkiaCanvas::DrawText(const Text& text) {} // TODO...

void SkiaCanvas::ClipRect(const Rect& rect, ClipOp op)
{
    SkRect clipRect = SkRect::MakeLTRB(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
    SkClipOp clipOp = static_cast<SkClipOp>(op);
    skiaCanvas_->clipRect(clipRect, clipOp);
}

void SkiaCanvas::ClipRoundRect(const RoundRect& roundRect, ClipOp op)
{
    SkRRect rRect;
    RoundRectCastToSkRRect(roundRect, rRect);
    SkClipOp clipOp = static_cast<SkClipOp>(op);
    skiaCanvas_->clipRRect(rRect, clipOp);
}

void SkiaCanvas::ClipPath(const Path& path, ClipOp op)
{
    auto skPathImpl = path.GetImpl<SkiaPath>();
    if (skPathImpl != nullptr) {
        SkClipOp clipOp = static_cast<SkClipOp>(op);
        skiaCanvas_->clipPath(skPathImpl->GetPath(), clipOp);
    }
}

void SkiaCanvas::SetMatrix(const Matrix& matrix)
{
    auto m = matrix.GetImpl<SkiaMatrix>();
    if (m != nullptr) {
        skiaCanvas_->setMatrix(m->ExportSkiaMatrix());
    }
}

void SkiaCanvas::ResetMatrix()
{
    skiaCanvas_->resetMatrix();
}

void SkiaCanvas::ConcatMatrix(const Matrix& matrix)
{
    auto m = matrix.GetImpl<SkiaMatrix>();
    if (m != nullptr) {
        skiaCanvas_->concat(m->ExportSkiaMatrix());
    }
}

void SkiaCanvas::Translate(scalar dx, scalar dy)
{
    skiaCanvas_->translate(dx, dy);
}

void SkiaCanvas::Scale(scalar sx, scalar sy)
{
    skiaCanvas_->scale(sx, sy);
}

void SkiaCanvas::Rotate(scalar deg)
{
    skiaCanvas_->rotate(deg);
}

void SkiaCanvas::Shear(scalar sx, scalar sy)
{
    skiaCanvas_->skew(sx, sy);
}

void SkiaCanvas::Flush()
{
    skiaCanvas_->flush();
}

void SkiaCanvas::Clear(ColorQuad color)
{
    skiaCanvas_->clear(color);
}

void SkiaCanvas::Save()
{
    skiaCanvas_->save();
}

void SkiaCanvas::SaveLayer(const Rect& rect, const Brush& brush)
{
    SkRect bounds = SkRect::MakeLTRB(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
    SkPaint paint;
    skiaPaint_.BrushToSkPaint(brush, paint);
    skiaCanvas_->saveLayer(bounds, &paint);
}

void SkiaCanvas::Restore()
{
    skiaCanvas_->restore();
}

void SkiaCanvas::AttachPen(const Pen& pen)
{
    skiaPaint_.ApplyPenToStroke(pen);
}

void SkiaCanvas::AttachBrush(const Brush& brush)
{
    skiaPaint_.ApplyBrushToFill(brush);
}

void SkiaCanvas::DetachPen()
{
    skiaPaint_.DisableStroke();
}

void SkiaCanvas::DetachBrush()
{
    skiaPaint_.DisableFill();
}

void SkiaCanvas::RoundRectCastToSkRRect(const RoundRect& roundRect, SkRRect& skRRect)
{
    Rect rect = roundRect.GetRect();
    SkRect outer = SkRect::MakeLTRB(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());

    SkVector radii[4];
    Point p;

    p = roundRect.GetCornerRadius(RoundRect::TOP_LEFT_POS);
    radii[SkRRect::kUpperLeft_Corner] = { p.GetX(), p.GetY() };
    p = roundRect.GetCornerRadius(RoundRect::TOP_RIGHT_POS);
    radii[SkRRect::kUpperRight_Corner] = { p.GetX(), p.GetY() };
    p = roundRect.GetCornerRadius(RoundRect::BOTTOM_RIGHT_POS);
    radii[SkRRect::kLowerRight_Corner] = { p.GetX(), p.GetY() };
    p = roundRect.GetCornerRadius(RoundRect::BOTTOM_LEFT_POS);
    radii[SkRRect::kLowerLeft_Corner] = { p.GetX(), p.GetY() };

    skRRect.setRectRadii(outer, radii);
}
}
}
}
