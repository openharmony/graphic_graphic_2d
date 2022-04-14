/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_RECORDING_CANVAS_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_RECORDING_CANVAS_H

#include "common/rs_macros.h"
#include "include/core/SkBitmap.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkCanvasVirtualEnforcer.h"
#include "include/core/SkDrawable.h"
#include "include/core/SkPaint.h"
#include "include/core/SkPath.h"
#include "include/core/SkRect.h"
#include "include/utils/SkNoDrawCanvas.h"
#include "pipeline/rs_draw_cmd_list.h"
#include "property/rs_properties_def.h"


namespace OHOS {
namespace Rosen {
class DrawCmdList;
class OpItem;
class RSRecordingCanvas : public SkCanvasVirtualEnforcer<SkNoDrawCanvas> {
public:
    RSRecordingCanvas(int width, int height);
    virtual ~RSRecordingCanvas();
    std::shared_ptr<DrawCmdList> GetDrawCmdList() const;
    void Clear() const;
    void AddOp(std::unique_ptr<OpItem>&& opItem);

    sk_sp<SkSurface> onNewSurface(const SkImageInfo& info, const SkSurfaceProps& props) override;

    void willSave() override;
    SaveLayerStrategy getSaveLayerStrategy(const SaveLayerRec& rec) override;
    void willRestore() override;

    void onFlush() override;

    void didConcat(const SkMatrix& matrix) override;
    void didSetMatrix(const SkMatrix& matrix) override;
    void didTranslate(SkScalar dx, SkScalar dy) override;

    void onClipRect(const SkRect& rect, SkClipOp clipOp, ClipEdgeStyle style) override;
    void onClipRRect(const SkRRect& rect, SkClipOp clipOp, ClipEdgeStyle style) override;
    void onClipPath(const SkPath& path, SkClipOp clipOp, ClipEdgeStyle style) override;
    void onClipRegion(const SkRegion& region, SkClipOp clipop) override;

    void onDrawPaint(const SkPaint& paint) override;
    void onDrawBehind(const SkPaint&) override;
    void onDrawPath(const SkPath& path, const SkPaint& paint) override;
    void onDrawRect(const SkRect& rect, const SkPaint& paint) override;
    void onDrawRegion(const SkRegion& region, const SkPaint& paint) override;
    void onDrawOval(const SkRect& oval, const SkPaint& paint) override;
    void onDrawArc(const SkRect& oval, SkScalar startAngle, SkScalar sweepAngle,
        bool useCenter, const SkPaint& paint) override;
    void onDrawRRect(const SkRRect& rrect, const SkPaint& paint) override;
    void onDrawDRRect(const SkRRect& out, const SkRRect& in, const SkPaint& paint) override;

    void onDrawDrawable(SkDrawable* drawable, const SkMatrix* matrix) override;
    void onDrawPicture(const SkPicture* picture, const SkMatrix* matrix, const SkPaint* paint) override;
    void onDrawAnnotation(const SkRect&, const char[], SkData*) override;

    void onDrawTextBlob(const SkTextBlob* blob, SkScalar x, SkScalar y, const SkPaint& paint) override;

    void onDrawBitmap(const SkBitmap& bm, SkScalar x, SkScalar y, const SkPaint* paint) override;
    void onDrawBitmapLattice(const SkBitmap& bm, const SkCanvas::Lattice& lattice, const SkRect& dst,
        const SkPaint* paint) override;
    void onDrawBitmapNine(const SkBitmap& bm, const SkIRect& center, const SkRect& dst, const SkPaint* paint) override;
    void onDrawBitmapRect(const SkBitmap& bm, const SkRect* src, const SkRect& dst,
        const SkPaint* paint, SrcRectConstraint constraint) override;

    void onDrawImage(const SkImage* img, SkScalar x, SkScalar y, const SkPaint* paint) override;
    void onDrawImageLattice(const SkImage* img, const SkCanvas::Lattice& lattice,
        const SkRect& dst, const SkPaint* paint) override;
    void onDrawImageNine(const SkImage* img, const SkIRect& center, const SkRect& dst, const SkPaint* paint) override;
    void onDrawImageRect(const SkImage* img, const SkRect* src, const SkRect& dst, const SkPaint* paint,
        SrcRectConstraint constraint) override;

    void onDrawPatch(const SkPoint[12], const SkColor[4], const SkPoint[4], SkBlendMode, const SkPaint&) override;
    void onDrawPoints(SkCanvas::PointMode mode, size_t count, const SkPoint pts[], const SkPaint& paint) override;
    void onDrawVerticesObject(
        const SkVertices*, const SkVertices::Bone bones[], int boneCount, SkBlendMode, const SkPaint&) override;
    void onDrawAtlas(const SkImage*, const SkRSXform[], const SkRect[], const SkColor[], int, SkBlendMode,
        const SkRect*, const SkPaint*) override;
    void onDrawShadowRec(const SkPath&, const SkDrawShadowRec&) override;

    void DrawAdaptiveRRect(float radius, const SkPaint& paint);
    void ClipAdaptiveRRect(float radius);
    void DrawImageWithParm(const sk_sp<SkImage>, int, int, float, const SkPaint& paint);
    void DrawImageWithParm(const sk_sp<SkImage>, const Rosen::RsImageInfo& rsImageInfo, const SkPaint& paint);

    void MultiplyAlpha(float alpha);
    void SaveAlpha();
    void RestoreAlpha();

    void onDrawEdgeAAQuad(const SkRect&, const SkPoint[4], QuadAAFlags, SkColor,
                          SkBlendMode) override {}
    void onDrawEdgeAAImageSet(const ImageSetEntry[], int, const SkPoint[],
                              const SkMatrix[], const SkPaint*, SrcRectConstraint) override {}

private:
    std::shared_ptr<DrawCmdList> drawCmdList_ { nullptr };
    int saveCount_ = 0;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_RECORDING_CANVAS_H
