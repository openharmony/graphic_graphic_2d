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

#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_OVERDRAW_RS_LISTENED_CANVAS_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_OVERDRAW_RS_LISTENED_CANVAS_H

#include "pipeline/rs_paint_filter_canvas.h"

namespace OHOS {
namespace Rosen {
class RSCanvasListener;

#ifndef USE_ROSEN_DRAWING
class RSB_EXPORT RSListenedCanvas : public RSPaintFilterCanvas {
public:
    RSListenedCanvas(SkCanvas* canvas, float alpha = 1.0f);
    RSListenedCanvas(SkSurface* skSurface, float alpha = 1.0f);

    void SetListener(const std::shared_ptr<RSCanvasListener> &listener);

    void onDrawPaint(const SkPaint& paint) override;
    void onDrawRect(const SkRect& rect, const SkPaint& paint) override;
    void onDrawRRect(const SkRRect& rrect, const SkPaint& paint) override;
    void onDrawDRRect(const SkRRect& outer, const SkRRect& inner,
                      const SkPaint& paint) override;
    void onDrawOval(const SkRect& rect, const SkPaint& paint) override;
    void onDrawArc(const SkRect& rect, SkScalar startAngle, SkScalar sweepAngle, bool useCenter,
                   const SkPaint& paint) override;
    void onDrawPath(const SkPath& path, const SkPaint& paint) override;
    void onDrawRegion(const SkRegion& region, const SkPaint& paint) override;
    void onDrawTextBlob(const SkTextBlob* blob, SkScalar x, SkScalar y,
                        const SkPaint& paint) override;
    void onDrawPatch(const SkPoint cubics[12], const SkColor colors[4],
                     const SkPoint texCoords[4], SkBlendMode mode,
                     const SkPaint& paint) override;
    void onDrawPoints(SkCanvas::PointMode mode, size_t count, const SkPoint pts[],
                      const SkPaint& paint) override;
    void onDrawAnnotation(const SkRect& rect, const char key[], SkData* value) override;
    void onDrawShadowRec(const SkPath& path, const SkDrawShadowRec& rect) override;
    void onDrawDrawable(SkDrawable* drawable, const SkMatrix* matrix) override;
    void onDrawPicture(const SkPicture* picture, const SkMatrix* matrix,
                       const SkPaint* paint) override;
#ifdef NEW_SKIA
    void onDrawImageRect2(const SkImage* image, const SkRect& src, const SkRect& dst,
            const SkSamplingOptions& samplingOptions, const SkPaint* paint, SrcRectConstraint constraint) override;
#else
    void onDrawImageRect(const SkImage* image, const SkRect* src, const SkRect& dst,
            const SkPaint* paint, SrcRectConstraint constraint) override;
#endif

private:
    std::shared_ptr<RSCanvasListener> listener_ = nullptr;
};

#else
class RSB_EXPORT RSListenedCanvas : public RSPaintFilterCanvas {
public:
    RSListenedCanvas(Drawing::Canvas& canvas, float alpha = 1.0f);
    RSListenedCanvas(Drawing::Surface& surface, float alpha = 1.0f);

    void SetListener(const std::shared_ptr<RSCanvasListener>& listener);
    // shapes
    void DrawPoint(const Drawing::Point& point) override;
    void DrawLine(const Drawing::Point& startPt, const Drawing::Point& endPt) override;
    void DrawRect(const Drawing::Rect& rect) override;
    void DrawRoundRect(const Drawing::RoundRect& roundRect) override;
    void DrawNestedRoundRect(const Drawing::RoundRect& outer, const Drawing::RoundRect& inner) override;
    void DrawArc(const Drawing::Rect& oval, Drawing::scalar startAngle, Drawing::scalar sweepAngle) override;
    void DrawPie(const Drawing::Rect& oval, Drawing::scalar startAngle, Drawing::scalar sweepAngle) override;
    void DrawOval(const Drawing::Rect& oval) override;
    void DrawCircle(const Drawing::Point& centerPt, Drawing::scalar radius) override;
    void DrawPath(const Drawing::Path& path) override;
    void DrawBackground(const Drawing::Brush& brush) override;
    void DrawShadow(const Drawing::Path& path, const Drawing::Point3& planeParams,
        const Drawing::Point3& devLightPos, Drawing::scalar lightRadius,
        Drawing::Color ambientColor, Drawing::Color spotColor, Drawing::ShadowFlags flag) override;
    void DrawRegion(const Drawing::Region& region) override;
    void DrawTextBlob(const Drawing::TextBlob* blob, const Drawing::scalar x, const Drawing::scalar y) override;

    // image
    void DrawBitmap(const Drawing::Bitmap& bitmap, const Drawing::scalar px, const Drawing::scalar py) override;
    void DrawBitmap(OHOS::Media::PixelMap& pixelMap, const Drawing::scalar px, const Drawing::scalar py) override;
    void DrawImage(const Drawing::Image& image,
        const Drawing::scalar px, const Drawing::scalar py, const Drawing::SamplingOptions& sampling) override;
    void DrawImageRect(const Drawing::Image& image,
        const Drawing::Rect& src, const Drawing::Rect& dst, const Drawing::SamplingOptions& sampling,
        Drawing::SrcRectConstraint constraint = Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT) override;
    void DrawImageRect(const Drawing::Image& image,
        const Drawing::Rect& dst, const Drawing::SamplingOptions& sampling) override;
    void DrawPicture(const Drawing::Picture& picture) override;

    void Clear(Drawing::ColorQuad color) override;
    // paint
    CoreCanvas& AttachPen(const Drawing::Pen& pen) override;
    CoreCanvas& AttachBrush(const Drawing::Brush& brush) override;
    CoreCanvas& DetachPen() override;
    CoreCanvas& DetachBrush() override;

private:
    std::shared_ptr<RSCanvasListener> listener_ = nullptr;
};
#endif // USE_ROSEN_DRAWING
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_OVERDRAW_RS_LISTENED_CANVAS_H
