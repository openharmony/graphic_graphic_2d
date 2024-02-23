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

#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_OVERDRAW_RS_GPU_OVERDRAW_CANVAS_LISTENER_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_OVERDRAW_RS_GPU_OVERDRAW_CANVAS_LISTENER_H

#include "rs_canvas_listener.h"

#ifndef USE_ROSEN_DRAWING
#include <include/core/SkSurface.h>
#else
#include "draw/surface.h"
#endif

#include "common/rs_macros.h"

#ifndef USE_ROSEN_DRAWING
class SkCanvas;
#endif
namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSGPUOverdrawCanvasListener : public RSCanvasListener {
public:
#ifndef USE_ROSEN_DRAWING
    explicit RSGPUOverdrawCanvasListener(SkCanvas &canvas);
#else
    explicit RSGPUOverdrawCanvasListener(Drawing::Canvas& canvas);
#endif
    ~RSGPUOverdrawCanvasListener() override;

    void Draw() override;
    bool IsValid() const override;
    const char *Name() const override { return "RSGPUOverdrawCanvasListener"; }

#ifndef USE_ROSEN_DRAWING
    void onDrawRect(const SkRect& rect, const SkPaint& paint) override;
    void onDrawRRect(const SkRRect& rect, const SkPaint& paint) override;
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
    void onDrawEdgeAAQuad(const SkRect& rect, const SkPoint clip[4],
            SkCanvas::QuadAAFlags aaFlags, const SkColor4f& color, SkBlendMode mode) override;
    void onDrawAnnotation(const SkRect& rect, const char key[], SkData* value) override;
    void onDrawShadowRec(const SkPath& path, const SkDrawShadowRec& rect) override;
    void onDrawDrawable(SkDrawable* drawable, const SkMatrix* matrix) override;
    void onDrawPicture(const SkPicture* picture, const SkMatrix* matrix,
                       const SkPaint* paint) override;

private:
    sk_sp<SkSurface> listenedSurface_ = nullptr;
    SkCanvas *overdrawCanvas_ = nullptr;
#else
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
    void AttachPen(const Drawing::Pen& pen) override;
    void AttachBrush(const Drawing::Brush& brush) override;
    void DetachPen() override;
    void DetachBrush() override;

private:
    std::shared_ptr<Drawing::Surface> listenedSurface_ = nullptr;
    std::shared_ptr<Drawing::OverDrawCanvas> overdrawCanvas_ = nullptr;
#endif // USE_ROSEN_DRAWING
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_OVERDRAW_RS_GPU_OVERDRAW_CANVAS_LISTENER_H
