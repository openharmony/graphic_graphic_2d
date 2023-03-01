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

#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_OVERDRAW_RS_CPU_OVERDRAW_CANVAS_LISTENER_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_OVERDRAW_RS_CPU_OVERDRAW_CANVAS_LISTENER_H

#include "rs_canvas_listener.h"

#include <map>

#include <include/core/SkRegion.h>

#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSCPUOverdrawCanvasListener : public RSCanvasListener {
public:
    explicit RSCPUOverdrawCanvasListener(SkCanvas &canvas);

    void Draw() override;
    bool IsValid() const override { return true; }
    const char *Name() const override { return "RSCPUOverdrawCanvasListener"; }

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
    void AppendRegion(SkPath &path);

    std::map<int, SkRegion> regions;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_OVERDRAW_RS_CPU_OVERDRAW_CANVAS_LISTENER_H
