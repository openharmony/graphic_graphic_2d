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

#include "platform/ohos/overdraw/rs_listened_canvas.h"

#include "platform/common/rs_log.h"
#include "platform/ohos/overdraw/rs_canvas_listener.h"

namespace OHOS {
namespace Rosen {
RSListenedCanvas::RSListenedCanvas(SkCanvas* canvas, float alpha)
    : RSPaintFilterCanvas(canvas, alpha)
{
}

RSListenedCanvas::RSListenedCanvas(SkSurface* skSurface, float alpha)
    : RSPaintFilterCanvas(skSurface, alpha)
{
}

void RSListenedCanvas::SetListener(const std::shared_ptr<RSCanvasListener> &listener)
{
    listener_ = listener;
}

void RSListenedCanvas::onDrawPaint(const SkPaint& paint)
{
    RSPaintFilterCanvas::onDrawPaint(paint);
    if (listener_ != nullptr) {
        listener_->onDrawPaint(paint);
    }
}

void RSListenedCanvas::onDrawRect(const SkRect& rect, const SkPaint& paint)
{
    RSPaintFilterCanvas::onDrawRect(rect, paint);
    if (listener_ != nullptr) {
        listener_->onDrawRect(rect, paint);
    }
}

void RSListenedCanvas::onDrawRRect(const SkRRect& rrect, const SkPaint& paint)
{
    RSPaintFilterCanvas::onDrawRRect(rrect, paint);
    if (listener_ != nullptr) {
        listener_->onDrawRRect(rrect, paint);
    }
}

void RSListenedCanvas::onDrawDRRect(const SkRRect& outer, const SkRRect& inner,
                                    const SkPaint& paint)
{
    RSPaintFilterCanvas::onDrawDRRect(outer, inner, paint);
    if (listener_ != nullptr) {
        listener_->onDrawDRRect(outer, inner, paint);
    }
}

void RSListenedCanvas::onDrawOval(const SkRect& rect, const SkPaint& paint)
{
    RSPaintFilterCanvas::onDrawOval(rect, paint);
    if (listener_ != nullptr) {
        listener_->onDrawOval(rect, paint);
    }
}

void RSListenedCanvas::onDrawArc(const SkRect& rect, SkScalar startAngle, SkScalar sweepAngle, bool useCenter,
                                 const SkPaint& paint)
{
    RSPaintFilterCanvas::onDrawArc(rect, startAngle, sweepAngle, useCenter, paint);
    if (listener_ != nullptr) {
        listener_->onDrawArc(rect, startAngle, sweepAngle, useCenter, paint);
    }
}

void RSListenedCanvas::onDrawPath(const SkPath& path, const SkPaint& paint)
{
    RSPaintFilterCanvas::onDrawPath(path, paint);
    if (listener_ != nullptr) {
        listener_->onDrawPath(path, paint);
    }
}

void RSListenedCanvas::onDrawRegion(const SkRegion& region, const SkPaint& paint)
{
    RSPaintFilterCanvas::onDrawRegion(region, paint);
    if (listener_ != nullptr) {
        listener_->onDrawRegion(region, paint);
    }
}

void RSListenedCanvas::onDrawTextBlob(const SkTextBlob* blob, SkScalar x, SkScalar y,
                                      const SkPaint& paint)
{
    RSPaintFilterCanvas::onDrawTextBlob(blob, x, y, paint);
    if (listener_ != nullptr) {
        listener_->onDrawTextBlob(blob, x, y, paint);
    }
}

void RSListenedCanvas::onDrawPatch(const SkPoint cubics[12], const SkColor colors[4],
                                   const SkPoint texCoords[4], SkBlendMode mode,
                                   const SkPaint& paint)
{
    RSPaintFilterCanvas::onDrawPatch(cubics, colors, texCoords, mode, paint);
    if (listener_ != nullptr) {
        listener_->onDrawPatch(cubics, colors, texCoords, mode, paint);
    }
}

void RSListenedCanvas::onDrawPoints(SkCanvas::PointMode mode, size_t count, const SkPoint pts[],
                                    const SkPaint& paint)
{
    RSPaintFilterCanvas::onDrawPoints(mode, count, pts, paint);
    if (listener_ != nullptr) {
        listener_->onDrawPoints(mode, count, pts, paint);
    }
}

void RSListenedCanvas::onDrawAnnotation(const SkRect& rect, const char key[], SkData* value)
{
    RSPaintFilterCanvas::onDrawAnnotation(rect, key, value);
    if (listener_ != nullptr) {
        listener_->onDrawAnnotation(rect, key, value);
    }
}

void RSListenedCanvas::onDrawShadowRec(const SkPath& path, const SkDrawShadowRec& rect)
{
    RSPaintFilterCanvas::onDrawShadowRec(path, rect);
    if (listener_ != nullptr) {
        listener_->onDrawShadowRec(path, rect);
    }
}

void RSListenedCanvas::onDrawDrawable(SkDrawable* drawable, const SkMatrix* matrix)
{
    RSPaintFilterCanvas::onDrawDrawable(drawable, matrix);
    if (listener_ != nullptr) {
        listener_->onDrawDrawable(drawable, matrix);
    }
}

void RSListenedCanvas::onDrawPicture(const SkPicture* picture, const SkMatrix* matrix,
                                     const SkPaint* paint)
{
    RSPaintFilterCanvas::onDrawPicture(picture, matrix, paint);
    if (listener_ != nullptr) {
        listener_->onDrawPicture(picture, matrix, paint);
    }
}

void RSListenedCanvas::onDrawImageRect(const SkImage* image, const SkRect* src, const SkRect& dst,
                                       const SkPaint* paint, SrcRectConstraint constraint)
{
    RSPaintFilterCanvas::onDrawImageRect(image, src, dst, paint, constraint);
    if (listener_ != nullptr) {
        listener_->onDrawRect(dst, {});
    }
}
} // namespace Rosen
} // namespace OHOS
