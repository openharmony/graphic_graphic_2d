/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "rs_overdraw_canvas_listener.h"

#include <include/core/SkDrawable.h>
#include <include/core/SkPath.h>
#include <include/core/SkPicture.h>
#include <include/core/SkRegion.h>
#include <include/core/SkTextBlob.h>

namespace OHOS {
namespace Rosen {
RSOverdrawCanvasListener::RSOverdrawCanvasListener(SkCanvas &canvas)
    : RSCanvasListener(canvas)
{
}

void RSOverdrawCanvasListener::Draw()
{
    static const std::map<int, SkColor> overdrawColorMap = {
        {1, 0x00000000},
        {2, 0x220000ff},
        {3, 0x2200ff00},
        {4, 0x22ff0000},
    };

    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setStyle(paint.kFill_Style);

    SkRegion drawed;
    for (int32_t i = regions.size(); i > 0; i--) {
        if (overdrawColorMap.find(i) != overdrawColorMap.end()) {
            paint.setColor(overdrawColorMap.at(i));
        } else {
            paint.setColor(0x44ff0000);
        }

        auto todraw = regions[i];
        todraw.op(drawed, SkRegion::kDifference_Op);
        canvas_.drawRegion(todraw, paint);
        drawed.op(todraw, SkRegion::kUnion_Op);
    }
}

void RSOverdrawCanvasListener::onDrawRect(const SkRect& rect, const SkPaint& paint)
{
    SkPath path;
    path.addRect(rect);
    AppendRegion(path);
}

void RSOverdrawCanvasListener::onDrawRRect(const SkRRect& rect, const SkPaint& paint)
{
    SkPath path;
    path.addRRect(rect);
    AppendRegion(path);
}

void RSOverdrawCanvasListener::onDrawDRRect(const SkRRect& outer, const SkRRect& inner,
                  const SkPaint& paint)
{
    SkPath path;
    path.addRRect(outer);
    path.addRRect(inner);
    AppendRegion(path);
}

void RSOverdrawCanvasListener::onDrawOval(const SkRect& rect, const SkPaint& paint)
{
    SkPath path;
    path.addOval(rect);
    AppendRegion(path);
}

void RSOverdrawCanvasListener::onDrawArc(const SkRect& rect, SkScalar startAngle, SkScalar sweepAngle, bool useCenter,
               const SkPaint& paint)
{
    SkPath path;
    path.addArc(rect, startAngle, sweepAngle);
    AppendRegion(path);
}

void RSOverdrawCanvasListener::onDrawPath(const SkPath& path, const SkPaint& paint)
{
    SkPath tpath = path;
    AppendRegion(tpath);
}

void RSOverdrawCanvasListener::onDrawRegion(const SkRegion& region, const SkPaint& paint)
{
    SkPath path;
    region.getBoundaryPath(&path);
    AppendRegion(path);
}

void RSOverdrawCanvasListener::onDrawTextBlob(const SkTextBlob* blob, SkScalar x, SkScalar y,
                    const SkPaint& paint)
{
    if (blob == nullptr) {
        return;
    }

    auto rect = blob->bounds();
    rect.offset(x, y);
    onDrawRect(rect, paint);
}

void RSOverdrawCanvasListener::onDrawPatch(const SkPoint cubics[12], const SkColor colors[4],
                 const SkPoint texCoords[4], SkBlendMode mode,
                 const SkPaint& paint)
{
    // TODO
}

void RSOverdrawCanvasListener::onDrawPoints(SkCanvas::PointMode mode, size_t count, const SkPoint pts[],
                  const SkPaint& paint)
{
    // TODO
}

void RSOverdrawCanvasListener::onDrawEdgeAAQuad(const SkRect& rect, const SkPoint clip[4],
        SkCanvas::QuadAAFlags aaFlags, const SkColor4f& color, SkBlendMode mode)
{
    SkPaint paint{color};
    paint.setBlendMode(mode);
    if (clip) {
        SkPath clipPath;
        clipPath.addPoly(clip, 4, true);
        onDrawPath(clipPath, paint);
    } else {
        onDrawRect(rect, paint);
    }
}

void RSOverdrawCanvasListener::onDrawAnnotation(const SkRect& rect, const char key[], SkData* value)
{
    // TODO
}

void RSOverdrawCanvasListener::onDrawShadowRec(const SkPath& path, const SkDrawShadowRec& rect)
{
    // TODO
}

void RSOverdrawCanvasListener::onDrawDrawable(SkDrawable* drawable, const SkMatrix* matrix)
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

void RSOverdrawCanvasListener::onDrawPicture(const SkPicture* picture, const SkMatrix* matrix,
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

void RSOverdrawCanvasListener::AppendRegion(SkPath &path)
{
    SkRegion target;
    path.transform(canvas_.getTotalMatrix());
    target.setPath(path, SkRegion(SkIRect::MakeLTRB(-1e9, -1e9, 1e9, 1e9)));

    for (int32_t i = regions.size(); i > 0; i--) {
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
