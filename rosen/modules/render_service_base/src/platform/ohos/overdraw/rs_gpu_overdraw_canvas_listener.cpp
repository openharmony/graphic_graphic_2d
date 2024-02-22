/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "platform/ohos/overdraw/rs_gpu_overdraw_canvas_listener.h"

#ifndef USE_ROSEN_DRAWING
#include <include/core/SkDrawable.h>
#include <include/core/SkOverdrawCanvas.h>
#include <include/core/SkPath.h>
#include <include/core/SkPicture.h>
#include <include/core/SkRegion.h>
#include <include/core/SkTextBlob.h>
#include <include/effects/SkOverdrawColorFilter.h>
#else
#include "draw/blend_mode.h"
#include "draw/color.h"
#include "effect/color_matrix.h"
#include "image/bitmap.h"
#endif

#include "platform/ohos/overdraw/rs_overdraw_controller.h"

namespace OHOS {
namespace Rosen {
#ifndef USE_ROSEN_DRAWING
RSGPUOverdrawCanvasListener::RSGPUOverdrawCanvasListener(SkCanvas &canvas)
    : RSCanvasListener(canvas)
{
    listenedSurface_ = canvas.makeSurface(canvas.imageInfo());
    if (listenedSurface_ != nullptr) {
        overdrawCanvas_ = new SkOverdrawCanvas(listenedSurface_->getCanvas());
    }
}
#else
RSGPUOverdrawCanvasListener::RSGPUOverdrawCanvasListener(Drawing::Canvas& canvas)
    : RSCanvasListener(canvas)
{
    auto gpuContext = canvas.GetGPUContext();
    if (gpuContext == nullptr) {
        ROSEN_LOGE("RSGPUOverdrawCanvasListener, construction failed: need gpu canvas");
        return;
    }

    Drawing::BitmapFormat format = { Drawing::ColorType::COLORTYPE_BGRA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL };
    auto width = canvas.GetWidth();
    auto height = canvas.GetHeight();
    Drawing::Bitmap bitmap;
    bitmap.Build(width, height, format);
    Drawing::Image image;
    image.BuildFromBitmap(*gpuContext, bitmap);

    auto surface = std::make_shared<Drawing::Surface>();
    surface->Bind(image);
    listenedSurface_ = surface;
    overdrawCanvas_ = std::make_shared<Drawing::OverDrawCanvas>(surface->GetCanvas());
}
#endif

RSGPUOverdrawCanvasListener::~RSGPUOverdrawCanvasListener()
{
#ifndef USE_ROSEN_DRAWING
    if (overdrawCanvas_ != nullptr) {
        delete overdrawCanvas_;
    }
#endif
}

void RSGPUOverdrawCanvasListener::Draw()
{
#ifndef USE_ROSEN_DRAWING
    auto image = listenedSurface_->makeImageSnapshot();
    SkPaint paint;
    auto overdrawColors = RSOverdrawController::GetInstance().GetColorArray();
#ifdef NEW_SKIA
    paint.setColorFilter(SkOverdrawColorFilter::MakeWithSkColors(overdrawColors.data()));
    canvas_.drawImage(image, 0, 0, SkSamplingOptions(), &paint);
#else
    paint.setColorFilter(SkOverdrawColorFilter::Make(overdrawColors.data()));
    canvas_.drawImage(image, 0, 0, &paint);
#endif
#else
    auto image = listenedSurface_->GetImageSnapshot();
    if (image == nullptr) {
        ROSEN_LOGE("image is nullptr");
        return;
    }
    Drawing::Brush brush;
    auto overdrawColors = RSOverdrawController::GetInstance().GetColorArray();
    auto colorFilter = Drawing::ColorFilter::CreateOverDrawColorFilter(overdrawColors.data());
    Drawing::Filter filter;
    filter.SetColorFilter(colorFilter);
    brush.SetFilter(filter);
    canvas_.AttachBrush(brush);
    canvas_.DrawImage(*image, 0, 0, Drawing::SamplingOptions());
    canvas_.DetachBrush();
#endif
}

bool RSGPUOverdrawCanvasListener::IsValid() const
{
    return listenedSurface_ != nullptr;
}

#ifndef USE_ROSEN_DRAWING
void RSGPUOverdrawCanvasListener::onDrawRect(const SkRect& rect, const SkPaint& paint)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }
    overdrawCanvas_->drawRect(rect, paint);
}

void RSGPUOverdrawCanvasListener::onDrawRRect(const SkRRect& rect, const SkPaint& paint)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }
    overdrawCanvas_->drawRRect(rect, paint);
}

void RSGPUOverdrawCanvasListener::onDrawDRRect(const SkRRect& outer, const SkRRect& inner,
                                               const SkPaint& paint)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }
    overdrawCanvas_->drawDRRect(outer, inner, paint);
}

void RSGPUOverdrawCanvasListener::onDrawOval(const SkRect& rect, const SkPaint& paint)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }
    overdrawCanvas_->drawOval(rect, paint);
}

void RSGPUOverdrawCanvasListener::onDrawArc(const SkRect& rect, SkScalar startAngle,
                                            SkScalar sweepAngle, bool useCenter,
                                            const SkPaint& paint)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }
    overdrawCanvas_->drawArc(rect, startAngle, sweepAngle, useCenter, paint);
}

void RSGPUOverdrawCanvasListener::onDrawPath(const SkPath& path, const SkPaint& paint)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }
    overdrawCanvas_->drawPath(path, paint);
}

void RSGPUOverdrawCanvasListener::onDrawRegion(const SkRegion& region, const SkPaint& paint)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }
    overdrawCanvas_->drawRegion(region, paint);
}

void RSGPUOverdrawCanvasListener::onDrawTextBlob(const SkTextBlob* blob, SkScalar x, SkScalar y,
                                                 const SkPaint& paint)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }
    overdrawCanvas_->drawTextBlob(blob, x, y, paint);
}

void RSGPUOverdrawCanvasListener::onDrawPatch(const SkPoint cubics[12], const SkColor colors[4],
                                              const SkPoint texCoords[4], SkBlendMode mode,
                                              const SkPaint& paint)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }
    overdrawCanvas_->drawPatch(cubics, colors, texCoords, mode, paint);
}

void RSGPUOverdrawCanvasListener::onDrawPoints(SkCanvas::PointMode mode, size_t count, const SkPoint pts[],
                                               const SkPaint& paint)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }
    overdrawCanvas_->drawPoints(mode, count, pts, paint);
}

void RSGPUOverdrawCanvasListener::onDrawEdgeAAQuad(const SkRect& rect, const SkPoint clip[4],
                                                   SkCanvas::QuadAAFlags aaFlags,
                                                   const SkColor4f& color, SkBlendMode mode)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }
    overdrawCanvas_->experimental_DrawEdgeAAQuad(rect, clip, aaFlags, color.toSkColor(), mode);
}

void RSGPUOverdrawCanvasListener::onDrawAnnotation(const SkRect& rect, const char key[], SkData* value)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }
    overdrawCanvas_->drawAnnotation(rect, key, value);
}

void RSGPUOverdrawCanvasListener::onDrawShadowRec(const SkPath& path, const SkDrawShadowRec& rect)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }
    overdrawCanvas_->private_draw_shadow_rec(path, rect);
}

void RSGPUOverdrawCanvasListener::onDrawDrawable(SkDrawable* drawable, const SkMatrix* matrix)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }
    overdrawCanvas_->drawDrawable(drawable, matrix);
}

void RSGPUOverdrawCanvasListener::onDrawPicture(const SkPicture* picture, const SkMatrix* matrix,
                                                const SkPaint* paint)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }
    overdrawCanvas_->drawPicture(picture, matrix, paint);
}

#else
void RSGPUOverdrawCanvasListener::DrawPoint(const Drawing::Point& point)
{
    DrawRect(Drawing::Rect(point.GetX(), point.GetY(), 1 + point.GetX(), 1 + point.GetY()));
}

void RSGPUOverdrawCanvasListener::DrawLine(const Drawing::Point& startPt, const Drawing::Point& endPt)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }

    Drawing::Path path;
    path.MoveTo(startPt.GetX(), startPt.GetY());
    path.LineTo(endPt.GetX(), endPt.GetY());
    overdrawCanvas_->DrawPath(path);
}

void RSGPUOverdrawCanvasListener::DrawRect(const Drawing::Rect& rect)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }
    overdrawCanvas_->DrawRect(rect);
}

void RSGPUOverdrawCanvasListener::DrawRoundRect(const Drawing::RoundRect& roundRect)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }
    overdrawCanvas_->DrawRoundRect(roundRect);
}

void RSGPUOverdrawCanvasListener::DrawNestedRoundRect(const Drawing::RoundRect& outer, const Drawing::RoundRect& inner)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }
    overdrawCanvas_->DrawNestedRoundRect(outer, inner);
}

void RSGPUOverdrawCanvasListener::DrawArc(
    const Drawing::Rect& oval, Drawing::scalar startAngle, Drawing::scalar sweepAngle)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }
    Drawing::Path path;
    path.AddArc(oval, startAngle, sweepAngle);
    overdrawCanvas_->DrawPath(path);
}

void RSGPUOverdrawCanvasListener::DrawPie(
    const Drawing::Rect& oval, Drawing::scalar startAngle, Drawing::scalar sweepAngle)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }
    Drawing::Path path;
    path.AddArc(oval, startAngle, sweepAngle);
    overdrawCanvas_->DrawPath(path);
}

void RSGPUOverdrawCanvasListener::DrawOval(const Drawing::Rect& oval)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }
    Drawing::Path path;
    path.AddOval(oval);
    overdrawCanvas_->DrawPath(path);
}

void RSGPUOverdrawCanvasListener::DrawCircle(const Drawing::Point& centerPt, Drawing::scalar radius)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }
    Drawing::Path path;
    path.AddCircle(centerPt.GetX(), centerPt.GetY(), radius);
    overdrawCanvas_->DrawPath(path);
}

void RSGPUOverdrawCanvasListener::DrawPath(const Drawing::Path& path)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }
    overdrawCanvas_->DrawPath(path);
}

void RSGPUOverdrawCanvasListener::DrawBackground(const Drawing::Brush& brush)
{
    // need know canvas rect region
}

void RSGPUOverdrawCanvasListener::DrawShadow(const Drawing::Path& path, const Drawing::Point3& planeParams,
    const Drawing::Point3& devLightPos, Drawing::scalar lightRadius, Drawing::Color ambientColor,
    Drawing::Color spotColor, Drawing::ShadowFlags flag)
{
    // need know shadow rect region
}

void RSGPUOverdrawCanvasListener::DrawRegion(const Drawing::Region& region)
{
    // need know region path region
}

void RSGPUOverdrawCanvasListener::DrawBitmap(
    const Drawing::Bitmap& bitmap, const Drawing::scalar px, const Drawing::scalar py)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }
    overdrawCanvas_->DrawRect(Drawing::Rect(px, py, bitmap.GetWidth() + px, bitmap.GetHeight() + py));
}

void RSGPUOverdrawCanvasListener::DrawBitmap(
    OHOS::Media::PixelMap& pixelMap, const Drawing::scalar px, const Drawing::scalar py)
{
    // need know pixelMap region
}

void RSGPUOverdrawCanvasListener::DrawImage(const Drawing::Image& image, const Drawing::scalar px,
    const Drawing::scalar py, const Drawing::SamplingOptions& sampling)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }
    overdrawCanvas_->DrawRect(Drawing::Rect(px, py, image.GetWidth() + px, image.GetHeight() + py));
}

void RSGPUOverdrawCanvasListener::DrawImageRect(const Drawing::Image& image, const Drawing::Rect& src,
    const Drawing::Rect& dst, const Drawing::SamplingOptions& sampling, Drawing::SrcRectConstraint constraint)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }
    overdrawCanvas_->DrawRect(dst);
}

void RSGPUOverdrawCanvasListener::DrawImageRect(
    const Drawing::Image& image, const Drawing::Rect& dst, const Drawing::SamplingOptions& sampling)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }
    overdrawCanvas_->DrawRect(dst);
}

void RSGPUOverdrawCanvasListener::DrawPicture(const Drawing::Picture& picture)
{
    // need know picture rect region
}

void RSGPUOverdrawCanvasListener::Clear(Drawing::ColorQuad color)
{
    // need know canvas rect region
}

void RSGPUOverdrawCanvasListener::AttachPen(const Drawing::Pen& pen)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("RSGPUOverdrawCanvasListener::AttachPen overdrawCanvas_ is nullptr");
        return;
    }
    overdrawCanvas_->AttachPen(pen);
}

void RSGPUOverdrawCanvasListener::AttachBrush(const Drawing::Brush& brush)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("RSGPUOverdrawCanvasListener::AttachBrush overdrawCanvas_ is nullptr");
        return;
    }
    overdrawCanvas_->AttachBrush(brush);
}

void RSGPUOverdrawCanvasListener::DetachPen()
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("RSGPUOverdrawCanvasListener::DetachPen overdrawCanvas_ is nullptr");
        return;
    }
    overdrawCanvas_->DetachPen();
}

void RSGPUOverdrawCanvasListener::DetachBrush()
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("RSGPUOverdrawCanvasListener::DetachBrush overdrawCanvas_ is nullptr");
        return;
    }
    overdrawCanvas_->DetachBrush();
}

#endif // USE_ROSEN_DRAWING
} // namespace Rosen
} // namespace OHOS
