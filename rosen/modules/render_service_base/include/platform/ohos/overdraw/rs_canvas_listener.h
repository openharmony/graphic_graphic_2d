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

#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_OVERDRAW_RS_CANVAS_LISTENER_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_OVERDRAW_RS_CANVAS_LISTENER_H

#ifndef USE_ROSEN_DRAWING
#include <include/core/SkCanvas.h>
#else
#include "draw/canvas.h"
#endif

namespace OHOS {
namespace Rosen {
class RSCanvasListener {
public:
#ifndef USE_ROSEN_DRAWING
    explicit RSCanvasListener(SkCanvas &canvas) : canvas_(canvas) {}
#else
    explicit RSCanvasListener(Drawing::Canvas &canvas) : canvas_(canvas) {}
#endif
    virtual ~RSCanvasListener() = default;
    virtual void Draw() {}
    virtual bool IsValid() const
    {
        return false;
    }

    virtual const char *Name() const
    {
        return "RSCanvasListener";
    }

#ifndef USE_ROSEN_DRAWING
    virtual void onDrawPaint(const SkPaint& paint) {}
    virtual void onDrawBehind(const SkPaint&) {}
    virtual void onDrawRect(const SkRect& rect, const SkPaint& paint) {}
    virtual void onDrawRRect(const SkRRect& rrect, const SkPaint& paint) {}
    virtual void onDrawDRRect(const SkRRect& outer, const SkRRect& inner,
                              const SkPaint& paint) {}
    virtual void onDrawOval(const SkRect& rect, const SkPaint& paint) {}
    virtual void onDrawArc(const SkRect& rect, SkScalar startAngle, SkScalar sweepAngle, bool useCenter,
                           const SkPaint& paint) {}
    virtual void onDrawPath(const SkPath& path, const SkPaint& paint) {}
    virtual void onDrawRegion(const SkRegion& region, const SkPaint& paint) {}
    virtual void onDrawTextBlob(const SkTextBlob* blob, SkScalar x, SkScalar y,
                                const SkPaint& paint) {}
    virtual void onDrawPatch(const SkPoint cubics[12], const SkColor colors[4],
                             const SkPoint texCoords[4], SkBlendMode mode,
                             const SkPaint& paint) {}
    virtual void onDrawPoints(SkCanvas::PointMode mode, size_t count, const SkPoint pts[],
                              const SkPaint& paint) {}
    virtual void onDrawEdgeAAQuad(const SkRect& rect, const SkPoint clip[4],
                    SkCanvas::QuadAAFlags aaFlags, const SkColor4f& color, SkBlendMode mode) {}
    virtual void onDrawAnnotation(const SkRect& rect, const char key[], SkData* value) {}
    virtual void onDrawShadowRec(const SkPath&, const SkDrawShadowRec&) {}
    virtual void onDrawDrawable(SkDrawable* drawable, const SkMatrix* matrix) {}
    virtual void onDrawPicture(const SkPicture* picture, const SkMatrix* matrix,
                               const SkPaint* paint) {}

protected:
    SkCanvas &canvas_;
#else
    // shapes
    virtual void DrawPoint(const Drawing::Point& point) {};
    virtual void DrawLine(const Drawing::Point& startPt, const Drawing::Point& endPt) {};
    virtual void DrawRect(const Drawing::Rect& rect) {};
    virtual void DrawRoundRect(const Drawing::RoundRect& roundRect) {};
    virtual void DrawNestedRoundRect(const Drawing::RoundRect& outer, const Drawing::RoundRect& inner) {};
    virtual void DrawArc(const Drawing::Rect& oval, Drawing::scalar startAngle, Drawing::scalar sweepAngle) {};
    virtual void DrawPie(const Drawing::Rect& oval, Drawing::scalar startAngle, Drawing::scalar sweepAngle) {};
    virtual void DrawOval(const Drawing::Rect& oval) {};
    virtual void DrawCircle(const Drawing::Point& centerPt, Drawing::scalar radius) {};
    virtual void DrawPath(const Drawing::Path& path) {};
    virtual void DrawBackground(const Drawing::Brush& brush) {};
    virtual void DrawShadow(const Drawing::Path& path, const Drawing::Point3& planeParams,
        const Drawing::Point3& devLightPos, Drawing::scalar lightRadius,
        Drawing::Color ambientColor, Drawing::Color spotColor, Drawing::ShadowFlags flag) {};
    virtual void DrawRegion(const Drawing::Region& region) {};
    virtual void DrawTextBlob(const Drawing::TextBlob* blob, const Drawing::scalar x, const Drawing::scalar y) {};

    // image
    virtual void DrawBitmap(const Drawing::Bitmap& bitmap, const Drawing::scalar px, const Drawing::scalar py) {};
    virtual void DrawBitmap(OHOS::Media::PixelMap& pixelMap, const Drawing::scalar px, const Drawing::scalar py) {};
    virtual void DrawImage(const Drawing::Image& image,
        const Drawing::scalar px, const Drawing::scalar py, const Drawing::SamplingOptions& sampling) {};
    virtual void DrawImageRect(const Drawing::Image& image,
        const Drawing::Rect& src, const Drawing::Rect& dst, const Drawing::SamplingOptions& sampling,
        Drawing::SrcRectConstraint constraint = Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT) {};
    virtual void DrawImageRect(const Drawing::Image& image,
        const Drawing::Rect& dst, const Drawing::SamplingOptions& sampling) {};
    virtual void DrawPicture(const Drawing::Picture& picture) {};

    virtual void Clear(Drawing::ColorQuad color) {};

    // paint
    virtual void AttachPen(const Drawing::Pen& pen) {};
    virtual void AttachBrush(const Drawing::Brush& brush) {};
    virtual void DetachPen() {};
    virtual void DetachBrush() {};

protected:
    Drawing::Canvas &canvas_;
#endif // USE_ROSEN_DRAWING
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_OVERDRAW_RS_CANVAS_LISTENER_H
