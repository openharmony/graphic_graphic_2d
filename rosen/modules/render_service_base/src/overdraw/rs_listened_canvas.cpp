/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "overdraw/rs_listened_canvas.h"
#include "overdraw/rs_canvas_listener.h"

namespace OHOS {
namespace Rosen {
RSListenedCanvas::RSListenedCanvas(RSPaintFilterCanvas *canvas)
    : RSPaintFilterCanvas(canvas)
{
    canvas_ = canvas;
}

RSListenedCanvas::~RSListenedCanvas()
{
    if (canvas_ != nullptr) {
        delete canvas_;
    }
}

void RSListenedCanvas::SetListener(const std::shared_ptr<RSCanvasListener> &listener)
{
    listener_ = listener;
}

void RSListenedCanvas::onDrawPaint(const SkPaint& paint)
{
    if (listener_ != nullptr) {
        listener_->onDrawPaint(paint);
    }

    if (canvas_ != nullptr) {
        canvas_->drawPaint(paint);
    }
}

void RSListenedCanvas::onDrawRect(const SkRect& rect, const SkPaint& paint)
{
    if (listener_ != nullptr) {
        listener_->onDrawRect(rect, paint);
    }

    if (canvas_ != nullptr) {
        canvas_->drawRect(rect, paint);
    }
}

void RSListenedCanvas::onDrawRRect(const SkRRect& rrect, const SkPaint& paint)
{
    if (listener_ != nullptr) {
        listener_->onDrawRRect(rrect, paint);
    }

    if (canvas_ != nullptr) {
        canvas_->drawRRect(rrect, paint);
    }
}

void RSListenedCanvas::onDrawDRRect(const SkRRect& outer, const SkRRect& inner,
                                    const SkPaint& paint)
{
    if (listener_ != nullptr) {
        listener_->onDrawDRRect(outer, inner, paint);
    }

    if (canvas_ != nullptr) {
        canvas_->drawDRRect(outer, inner, paint);
    }
}

void RSListenedCanvas::onDrawOval(const SkRect& rect, const SkPaint& paint)
{
    if (listener_ != nullptr) {
        listener_->onDrawOval(rect, paint);
    }

    if (canvas_ != nullptr) {
        canvas_->drawOval(rect, paint);
    }
}

void RSListenedCanvas::onDrawArc(const SkRect& rect, SkScalar startAngle, SkScalar sweepAngle, bool useCenter,
                                 const SkPaint& paint)
{
    if (listener_ != nullptr) {
        listener_->onDrawArc(rect, startAngle, sweepAngle, useCenter, paint);
    }

    if (canvas_ != nullptr) {
        canvas_->drawArc(rect, startAngle, sweepAngle, useCenter, paint);
    }
}

void RSListenedCanvas::onDrawPath(const SkPath& path, const SkPaint& paint)
{
    if (listener_ != nullptr) {
        listener_->onDrawPath(path, paint);
    }

    if (canvas_ != nullptr) {
        canvas_->drawPath(path, paint);
    }
}

void RSListenedCanvas::onDrawRegion(const SkRegion& region, const SkPaint& paint)
{
    if (listener_ != nullptr) {
        listener_->onDrawRegion(region, paint);
    }

    if (canvas_ != nullptr) {
        canvas_->drawRegion(region, paint);
    }
}

void RSListenedCanvas::onDrawTextBlob(const SkTextBlob* blob, SkScalar x, SkScalar y,
                                      const SkPaint& paint)
{
    if (listener_ != nullptr) {
        listener_->onDrawTextBlob(blob, x, y, paint);
    }

    if (canvas_ != nullptr) {
        canvas_->drawTextBlob(blob, x, y, paint);
    }
}

void RSListenedCanvas::onDrawPatch(const SkPoint cubics[12], const SkColor colors[4],
                                   const SkPoint texCoords[4], SkBlendMode mode,
                                   const SkPaint& paint)
{
    if (listener_ != nullptr) {
        listener_->onDrawPatch(cubics, colors, texCoords, mode, paint);
    }

    if (canvas_ != nullptr) {
        canvas_->drawPatch(cubics, colors, texCoords, mode, paint);
    }
}

void RSListenedCanvas::onDrawPoints(SkCanvas::PointMode mode, size_t count, const SkPoint pts[],
                                    const SkPaint& paint)
{
    if (listener_ != nullptr) {
        listener_->onDrawPoints(mode, count, pts, paint);
    }

    if (canvas_ != nullptr) {
        canvas_->drawPoints(mode, count, pts, paint);
    }
}

void RSListenedCanvas::onDrawAnnotation(const SkRect& rect, const char key[], SkData* value)
{
    if (listener_ != nullptr) {
        listener_->onDrawAnnotation(rect, key, value);
    }

    if (canvas_ != nullptr) {
        canvas_->drawAnnotation(rect, key, value);
    }
}

void RSListenedCanvas::onDrawShadowRec(const SkPath& path, const SkDrawShadowRec& rect)
{
    if (listener_ != nullptr) {
        listener_->onDrawShadowRec(path, rect);
    }

    if (canvas_ != nullptr) {
        canvas_->private_draw_shadow_rec(path, rect);
    }
}

void RSListenedCanvas::onDrawDrawable(SkDrawable* drawable, const SkMatrix* matrix)
{
    if (listener_ != nullptr) {
        listener_->onDrawDrawable(drawable, matrix);
    }

    if (canvas_ != nullptr) {
        canvas_->drawDrawable(drawable, matrix);
    }
}

void RSListenedCanvas::onDrawPicture(const SkPicture* picture, const SkMatrix* matrix,
                                     const SkPaint* paint)
{
    if (listener_ != nullptr) {
        listener_->onDrawPicture(picture, matrix, paint);
    }

    if (canvas_ != nullptr) {
        canvas_->drawPicture(picture, matrix, paint);
    }
}

void RSListenedCanvas::willSave()
{
    if (canvas_ != nullptr) {
        canvas_->save();
    }
}

void RSListenedCanvas::willRestore()
{
    if (canvas_ != nullptr) {
        canvas_->restore();
    }
}

void RSListenedCanvas::onFlush()
{
    if (listener_ != nullptr) {
        listener_->onFlush();
    }

    if (canvas_ != nullptr) {
        canvas_->flush();
    }
}

void RSListenedCanvas::didTranslate(SkScalar dx, SkScalar dy)
{
    if (canvas_ != nullptr) {
        canvas_->translate(dx, dy);
    }
}

void RSListenedCanvas::onClipRect(const SkRect& rect, SkClipOp clipOp, ClipEdgeStyle style)
{
    if (canvas_ != nullptr) {
        canvas_->clipRect(rect, clipOp, style);
    }
}

void RSListenedCanvas::onClipRRect(const SkRRect& rect, SkClipOp clipOp, ClipEdgeStyle style)
{
    if (canvas_ != nullptr) {
        canvas_->clipRRect(rect, clipOp, style);
    }
}

void RSListenedCanvas::onClipPath(const SkPath& path, SkClipOp clipOp, ClipEdgeStyle style)
{
    if (canvas_ != nullptr) {
        canvas_->clipPath(path, clipOp, style);
    }
}

void RSListenedCanvas::onClipRegion(const SkRegion& region, SkClipOp clipop)
{
    if (canvas_ != nullptr) {
        canvas_->clipRegion(region, clipop);
    }
}
} // namespace Rosen
} // namespace OHOS
