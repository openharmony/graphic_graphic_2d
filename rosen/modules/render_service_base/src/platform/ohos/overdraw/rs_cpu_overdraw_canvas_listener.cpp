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

#include "platform/ohos/overdraw/rs_cpu_overdraw_canvas_listener.h"

#include <include/core/SkDrawable.h>
#include <include/core/SkPath.h>
#include <include/core/SkPicture.h>
#include <include/core/SkRegion.h>
#include <include/core/SkTextBlob.h>

#include "platform/ohos/overdraw/rs_overdraw_controller.h"

namespace OHOS {
namespace Rosen {
RSCPUOverdrawCanvasListener::RSCPUOverdrawCanvasListener(SkCanvas &canvas)
    : RSCanvasListener(canvas)
{
}

void RSCPUOverdrawCanvasListener::Draw()
{
    auto overdrawColorMap = RSOverdrawController::GetInstance().GetColorMap();
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setStyle(paint.kFill_Style);

    SkRegion drawed;
    for (size_t i = regions.size(); i > 0; i--) {
        if (overdrawColorMap.find(i) != overdrawColorMap.end()) {
            paint.setColor(overdrawColorMap.at(i));
        } else {
            paint.setColor(overdrawColorMap.at(0));
        }

        auto todraw = regions[i];
        todraw.op(drawed, SkRegion::kDifference_Op);
        canvas_.drawRegion(todraw, paint);
        drawed.op(todraw, SkRegion::kUnion_Op);
    }
}

void RSCPUOverdrawCanvasListener::onDrawRect(const SkRect& rect, const SkPaint& paint)
{
    SkPath path;
    path.addRect(rect);
    AppendRegion(path);
}

void RSCPUOverdrawCanvasListener::onDrawRRect(const SkRRect& rect, const SkPaint& paint)
{
    SkPath path;
    path.addRRect(rect);
    AppendRegion(path);
}

void RSCPUOverdrawCanvasListener::onDrawDRRect(const SkRRect& outer, const SkRRect& inner,
                                               const SkPaint& paint)
{
    SkPath path;
    path.addRRect(outer);
    path.addRRect(inner);
    AppendRegion(path);
}

void RSCPUOverdrawCanvasListener::onDrawOval(const SkRect& rect, const SkPaint& paint)
{
    SkPath path;
    path.addOval(rect);
    AppendRegion(path);
}

void RSCPUOverdrawCanvasListener::onDrawArc(const SkRect& rect, SkScalar startAngle,
                                            SkScalar sweepAngle, bool useCenter,
                                            const SkPaint& paint)
{
    SkPath path;
    path.addArc(rect, startAngle, sweepAngle);
    AppendRegion(path);
}

void RSCPUOverdrawCanvasListener::onDrawPath(const SkPath& path, const SkPaint& paint)
{
    SkPath tpath = path;
    AppendRegion(tpath);
}

void RSCPUOverdrawCanvasListener::onDrawRegion(const SkRegion& region, const SkPaint& paint)
{
    SkPath path;
    region.getBoundaryPath(&path);
    AppendRegion(path);
}

void RSCPUOverdrawCanvasListener::onDrawTextBlob(const SkTextBlob* blob, SkScalar x, SkScalar y,
                                                 const SkPaint& paint)
{
    if (blob == nullptr) {
        return;
    }

    auto rect = blob->bounds();
    rect.offset(x, y);
    onDrawRect(rect, paint);
}

void RSCPUOverdrawCanvasListener::onDrawPatch(const SkPoint cubics[12], const SkColor colors[4],
                                              const SkPoint texCoords[4], SkBlendMode mode,
                                              const SkPaint& paint)
{
    // need know patch region
}

void RSCPUOverdrawCanvasListener::onDrawPoints(SkCanvas::PointMode mode, size_t count, const SkPoint pts[],
                                               const SkPaint& paint)
{
    for (size_t i = 0; i < count; i++) {
        onDrawRect(SkRect::MakeXYWH(pts[i].x(), pts[i].y(), 1, 1), paint);
    }
}

void RSCPUOverdrawCanvasListener::onDrawEdgeAAQuad(const SkRect& rect, const SkPoint clip[4],
                                                   SkCanvas::QuadAAFlags aaFlags,
                                                   const SkColor4f& color, SkBlendMode mode)
{
    SkPaint paint(color);
    paint.setBlendMode(mode);
    if (clip) {
        SkPath clipPath;
        clipPath.addPoly(clip, 0x4, true); // 4 from clip[4]
        onDrawPath(clipPath, paint);
    } else {
        onDrawRect(rect, paint);
    }
}

void RSCPUOverdrawCanvasListener::onDrawAnnotation(const SkRect& rect, const char key[], SkData* value)
{
    // need know annotation region
}

void RSCPUOverdrawCanvasListener::onDrawShadowRec(const SkPath& path, const SkDrawShadowRec& rect)
{
    // need know shadow rect region
}

void RSCPUOverdrawCanvasListener::onDrawDrawable(SkDrawable* drawable, const SkMatrix* matrix)
{
    if (drawable == nullptr) {
        return;
    }

    canvas_.save();
    if (matrix) {
        auto nowMatrix = canvas_.getTotalMatrix();
        nowMatrix.postConcat(*matrix);
        canvas_.setMatrix(nowMatrix);
    }

    onDrawRect(drawable->getBounds(), {});
    canvas_.restore();
}

void RSCPUOverdrawCanvasListener::onDrawPicture(const SkPicture* picture, const SkMatrix* matrix,
                                                const SkPaint* paint)
{
    if (picture == nullptr) {
        return;
    }

    canvas_.save();
    if (matrix) {
        auto nowMatrix = canvas_.getTotalMatrix();
        nowMatrix.postConcat(*matrix);
        canvas_.setMatrix(nowMatrix);
    }

    SkPaint p;
    if (paint) {
        p = *paint;
    }

    onDrawRect(picture->cullRect(), p);
    canvas_.restore();
}

void RSCPUOverdrawCanvasListener::AppendRegion(SkPath &path)
{
    SkRegion target;
    path.transform(canvas_.getTotalMatrix());
    target.setPath(path, SkRegion(SkIRect::MakeLTRB(-1e9, -1e9, 1e9, 1e9)));

    for (size_t i = regions.size(); i > 0; i--) {
        if (regions[i].intersects(target)) {
            auto regi = regions[i];
            regi.op(target, SkRegion::kIntersect_Op);
            regions[i + 1].op(regi, SkRegion::kUnion_Op);
        }
    }
    regions[1].op(target, SkRegion::kUnion_Op);
}
} // namespace Rosen
} // namespace OHOS
