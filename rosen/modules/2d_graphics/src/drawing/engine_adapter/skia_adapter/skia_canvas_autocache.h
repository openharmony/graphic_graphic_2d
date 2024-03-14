/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef SKIACANVAS_AUTOCACHE_H
#define SKIACANVAS_AUTOCACHE_H

#include <vector>
#include <map>
#include <string>
#include "skia_canvas_op.h"
#include "include/utils/SkNWayCanvas.h"
#include "include/core/SkCanvasVirtualEnforcer.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaCanvasAutoCache : public SkiaCanvasOp {
public:
    explicit SkiaCanvasAutoCache(SkCanvas* canvas);

    // drawing func
    bool OpCanCache(const SkRect& bound) override;
    std::vector<SkRect>& GetOpListDrawArea() override;
    SkRect& GetOpUnionRect() override;

    int GetOpsNum() override
    {
        return totalOpNums_;
    }

    int GetOpsPercent() override
    {
        return percent_;
    }

    void Init(const SkMatrix& m);

    // dfx
    void ShowDrawResult(const SkRect &bound);

    SkISize getBaseLayerSize() const override
    {
        return proxy()->getBaseLayerSize();
    }

    GrRecordingContext* recordingContext() override
    {
        return proxy()->recordingContext();
    }

protected:
    SaveLayerStrategy getSaveLayerStrategy(const SaveLayerRec&) override;
    bool onDoSaveBehind(const SkRect* rect) override;
    void onDrawPaint(const SkPaint&) override;
    void onDrawBehind(const SkPaint&) override;
    void onDrawPoints(PointMode, size_t count, const SkPoint pts[], const SkPaint&) override;
    void onDrawRect(const SkRect&, const SkPaint&) override;
    void onDrawRRect(const SkRRect&, const SkPaint&) override;
    void onDrawDRRect(const SkRRect&, const SkRRect&, const SkPaint&) override;
    void onDrawRegion(const SkRegion&, const SkPaint&) override;
    void onDrawOval(const SkRect&, const SkPaint&) override;
    void onDrawArc(const SkRect&, SkScalar, SkScalar, bool, const SkPaint&) override;
    void onDrawPath(const SkPath&, const SkPaint&) override;

    void onDrawImage2(const SkImage*, SkScalar, SkScalar, const SkSamplingOptions&,
        const SkPaint*) override;
    void onDrawImageRect2(const SkImage*, const SkRect&, const SkRect&, const SkSamplingOptions&,
        const SkPaint*, SrcRectConstraint) override;
    void onDrawImageLattice2(const SkImage*, const Lattice&, const SkRect&, SkFilterMode,
        const SkPaint*) override;
    void onDrawAtlas2(const SkImage*, const SkRSXform[], const SkRect[], const SkColor[], int,
        SkBlendMode, const SkSamplingOptions&, const SkRect*, const SkPaint*) override;

    void onDrawVerticesObject(const SkVertices*, SkBlendMode, const SkPaint&) override;
    void onDrawPatch(const SkPoint cubics[12], const SkColor colors[4],
        const SkPoint texCoords[4], SkBlendMode, const SkPaint& paint) override;
    void onDrawPicture(const SkPicture*, const SkMatrix*, const SkPaint*) override;
    void onDrawDrawable(SkDrawable*, const SkMatrix*) override;

    void onDrawGlyphRunList(const SkGlyphRunList&, const SkPaint&) override;
    void onDrawTextBlob(const SkTextBlob* blob, SkScalar x, SkScalar y,
        const SkPaint& paint) override;
    void onDrawAnnotation(const SkRect& rect, const char key[], SkData* value) override;
    void onDrawShadowRec(const SkPath& path, const SkDrawShadowRec& rec) override;

    void onDrawEdgeAAQuad(const SkRect&, const SkPoint[4], QuadAAFlags, const SkColor4f&,
        SkBlendMode) override;
    void onDrawEdgeAAImageSet2(const ImageSetEntry[], int count, const SkPoint[], const SkMatrix[],
        const SkSamplingOptions&, const SkPaint*, SrcRectConstraint) override;

    // draw on actual canvas
    sk_sp<SkSurface> onNewSurface(const SkImageInfo&, const SkSurfaceProps&) override;
    bool onPeekPixels(SkPixmap* pixmap) override;
    bool onAccessTopLayerPixels(SkPixmap* pixmap) override;
    SkImageInfo onImageInfo() const override;
    bool onGetProps(SkSurfaceProps* props) const override;

private:
    bool OpShouldRecord() const;
    void RecordUnsupportOp(const char* name);
    void RecordUnsupportOp(const char* name, const SkPaint& paint);
    bool RecordDrawArea(const SkRect& bounds, const SkPaint& paint, const SkMatrix* matrix = nullptr);
    void MergeDrawAreaRects();
    SkCanvas* proxy() const { return fList[0]; }

    std::vector<SkRect> drawAreaRects_;
    SkRect unionDrawArea_ = SkRect::MakeEmpty();
    SkRect rejectBounds_ = SkRect::MakeEmpty();
    SkMatrix originMatrixInvert_;
    SkMatrix nodeMatrix_;
    int totalOpNums_ = 0;
    int64_t totalDrawAreas_ = 0;
    int percent_ = 0;
    bool opCanCache_ = true;
    std::map<std::string, int> debugNotSupportOps_;
    int calNotSupport_ = 0;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
