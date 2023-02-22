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
    void onDrawImageRect(const SkImage* image, const SkRect* src, const SkRect& dst,
            const SkPaint* paint, SrcRectConstraint constraint) override;

private:
    std::shared_ptr<RSCanvasListener> listener_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_OVERDRAW_RS_LISTENED_CANVAS_H
