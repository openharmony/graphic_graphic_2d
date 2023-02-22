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

#include <include/core/SkCanvas.h>

namespace OHOS {
namespace Rosen {
class RSCanvasListener {
public:
    explicit RSCanvasListener(SkCanvas &canvas) : canvas_(canvas) {}
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
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_OVERDRAW_RS_CANVAS_LISTENER_H
