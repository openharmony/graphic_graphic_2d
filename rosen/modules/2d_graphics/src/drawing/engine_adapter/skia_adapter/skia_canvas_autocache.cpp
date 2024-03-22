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

#include "skia_canvas_autocache.h"

#include "include/core/SkPaint.h"
#include "include/core/SkPixmap.h"
#include "include/core/SkSurface.h"
#include "include/core/SkRRect.h"
#include "include/core/SkRegion.h"
#include "include/core/SkPath.h"
#include "include/core/SkVertices.h"
#include "include/core/SkPathEffect.h"
#include "include/core/SkImageFilter.h"
#include "include/core/SkColorFilter.h"
#include "include/core/SkImage.h"
#include "include/core/SkMaskFilter.h"
#include "include/core/SkPicture.h"
#include "include/core/SkTextBlob.h"
#include "include/core/SkDrawable.h"
#include "src/core/SkGlyphRun.h"
#include "src/utils/SkPatchUtils.h"
#include "src/core/SkCanvasPriv.h"
#include "src/core/SkTLazy.h"
#include "src/core/SkMatrixPriv.h"

#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

static constexpr int32_t MAX_PERCENTAGE = 8;
static constexpr int32_t MIN_OPS_NUM = 3;
static constexpr int32_t MAX_OPS_NUM = 8;
static constexpr int32_t PERCENT = 100;

SkiaCanvasAutoCache::SkiaCanvasAutoCache(SkCanvas* canvas)
    : SkiaCanvasOp(canvas)
{
    SkMatrix originMatrix = canvas->getTotalMatrix();
    if (!originMatrix.invert(&originMatrixInvert_)) {
        LOGE("opinc originMatrix cannot invert");
        opCanCache_ = false;
    }
    rejectBounds_ = canvas->getTotalMatrix().mapRect(canvas->getLocalClipBounds());
    this->clipRect(SkRect::Make(canvas->getDeviceClipBounds()));
    this->setMatrix(originMatrix);
    this->addCanvas(canvas);
}

void SkiaCanvasAutoCache::Init(const SkMatrix& m)
{
    if (!m.invert(&nodeMatrix_)) {
        LOGE("opinc originMatrix cannot invert");
        opCanCache_ = false;
    }
    nodeMatrix_.preConcat(originMatrixInvert_);
}

bool SkiaCanvasAutoCache::OpCanCache(const SkRect& bound)
{
#ifdef DDGR_ENABLE_FEATURE_OP_INC_DFX
    MergeDrawAreaRects();
#endif
    do {
        if (!opCanCache_ || totalOpNums_ == 0 || drawAreaRects_.size() == 0) {
            opCanCache_ = false;
            calNotSupport_ = __LINE__;
            break;
        }
        MergeDrawAreaRects(); // get unionDrawArea_
        if (((int)(unionDrawArea_.width()) > (int)(bound.width())) ||
            ((int)(unionDrawArea_.height()) > (int)(bound.height()))) {
            opCanCache_ = false;
            calNotSupport_ = __LINE__;
            break;
        }

        percent_ = (totalDrawAreas_ * PERCENT) / ((int)(unionDrawArea_.width()) * (int)(unionDrawArea_.height()));
        if (totalOpNums_ >= MAX_OPS_NUM ||
            (percent_ > MAX_PERCENTAGE && totalOpNums_ > MIN_OPS_NUM)) {
            opCanCache_ = true;
            break;
        }
        opCanCache_ = false;
        calNotSupport_ = __LINE__;
    } while (false);
#ifdef DDGR_ENABLE_FEATURE_OP_INC_DFX
    ShowDrawResult(bound);
#endif
    return opCanCache_;
}

std::vector<SkRect>& SkiaCanvasAutoCache::GetOpListDrawArea()
{
    return drawAreaRects_;
}

SkRect& SkiaCanvasAutoCache::GetOpUnionRect()
{
    return unionDrawArea_;
}

bool SkiaCanvasAutoCache::OpShouldRecord() const
{
    return opCanCache_;
}

void SkiaCanvasAutoCache::RecordUnsupportOp(const char* name)
{
    SkPaint paint;
    RecordUnsupportOp(name, paint);
}

void SkiaCanvasAutoCache::RecordUnsupportOp(const char* name, const SkPaint& paint)
{
#ifdef DDGR_ENABLE_FEATURE_OP_INC_DFX
    if (name != nullptr) {
        std::string ret(name);
        ret.append("_");
        if (paint.asBlendMode().has_value()) {
            ret.append(std::string(SkBlendMode_Name(paint.asBlendMode().value())));
        } else {
            ret.append("noblend");
        }
        ret.append("_");
        ret.append(std::to_string(paint.getAlpha()));
        debugNotSupportOps_[ret.c_str()]++;
    }
#endif
    opCanCache_ = false;
}

static bool CmpSkRectLTRB(const SkRect& a, const SkRect& b)
{
    if (a.left() < b.left()) {
        return true;
    }
    if (a.left() > b.left()) {
        return false;
    }
    if (a.top() < b.top()) {
        return true;
    }
    if (a.top() > b.top()) {
        return false;
    }
    if (a.right() < b.right()) {
        return true;
    }
    if (a.right() > b.right()) {
        return false;
    }
    if (a.bottom() < b.bottom()) {
        return true;
    }
    if (a.bottom() > b.bottom()) {
        return false;
    }
    return false;
}

/* The intersecting regins are merged into one rect. The disjoint regions are not merged. */
void SkiaCanvasAutoCache::MergeDrawAreaRects()
{
    std::vector<SkRect>& drawAreaRects = drawAreaRects_;
    for (uint32_t i = 0; i < drawAreaRects.size(); i++) {
        for (uint32_t j = 0; j < drawAreaRects.size(); j++) {
            if (i != j && drawAreaRects[i].intersects(drawAreaRects[j])) {
                drawAreaRects[i].join(drawAreaRects[j]);
                drawAreaRects[j] = drawAreaRects[i];
            }
        }
    }
    std::sort(drawAreaRects.begin(), drawAreaRects.end(), CmpSkRectLTRB);
    drawAreaRects.erase(std::unique(drawAreaRects.begin(), drawAreaRects.end()), drawAreaRects.end());

    for (uint32_t i = 0; i < drawAreaRects.size(); i++) {
        for (uint32_t j = 0; j < drawAreaRects.size(); j++) {
            if (drawAreaRects[i].intersects(drawAreaRects[j])) {
                opCanCache_ = false;
                return;
            }
        }
    }

    SkRect unionDrawAreaTemp = SkRect::MakeEmpty();
    for (uint32_t i = 0; i < drawAreaRects.size(); i++) {
        drawAreaRects[i] = nodeMatrix_.mapRect(drawAreaRects[i]);
        unionDrawAreaTemp.join(drawAreaRects[i]);
    }
    unionDrawAreaTemp.roundOut(&unionDrawArea_);
}

void SkiaCanvasAutoCache::ShowDrawResult(const SkRect& bound)
{
#ifdef DDGR_ENABLE_FEATURE_OP_INC_DFX
    std::vector<SkRect>& drawAreaRects = drawAreaRects_;
    LOGI("DDGR draw result %d, canvas w%d h%d, opNum%d, percent%d, cal%d, "
        "node[%.2f %.2f %.2f %.2f] bound[%.2f %.2f %.2f %.2f] reject[%.2f %.2f %.2f %.2f]"
        "rect num %d not support %d",
        opCanCache_, proxy()->imageInfo().width(), proxy()->imageInfo().height(),
        totalOpNums_, percent_, calNotSupport_,
        bound.x(), bound.y(), bound.width(), bound().height(),
        unionDrawArea_.x(), unionDrawArea_.y(), unionDrawArea_.width(), unionDrawArea_.height(),
        (int)drawAreaRects.size(), (int)debugNotSupportOps_.size());
    for (uint32_t i = 0; i < drawAreaRects.size(); i++) {
        SkRect &rect = drawAreaRects[i];
        LOGI("DDGR rect[%u], [%.2f %.2f %.2f %.2f]", i, rect.x(), rect.y(), rect.width(), rect.height());
    }

    int j = 0;
    for (auto& iter : debugNotSupportOps_) {
        LOGI("DDGR ops[%d] [%s %d]", j, iter.first.c_str(), iter.second);
        j++;
    }
#endif
}

static SkPaint ProcessPaintForImage(const SkPaint* paint)
{
    SkPaint ret;
    if (paint) {
        ret = *paint;
        ret.setStyle(SkPaint::kFill_Style);
        ret.setPathEffect(nullptr);
    }
    return ret;
}

static SkPaint ProcessPaintForVertices(SkPaint paint)
{
    paint.setStyle(SkPaint::kFill_Style);
    paint.setMaskFilter(nullptr);
    paint.setPathEffect(nullptr);
    return paint;
}

static inline bool BlendModeCanCache(SkBlendMode mode)
{
    return (mode == SkBlendMode::kSrcOver) || (mode == SkBlendMode::kSrc);
}

static inline bool PaintCanCache(const SkPaint& paint)
{
    const auto bm = paint.asBlendMode();
    if (bm && !BlendModeCanCache(bm.value())) {
        return false;
    }
    return true;
}

bool SkiaCanvasAutoCache::RecordDrawArea(const SkRect& bounds, const SkPaint& paint, const SkMatrix* matrix)
{
    if (!bounds.isFinite() || paint.nothingToDraw()) {
        return true;
    }

    if (PaintCanCache(paint) && paint.canComputeFastBounds()) {
        SkRect tmp = matrix ? matrix->mapRect(bounds) : bounds;
        SkRect devRect = getTotalMatrix().mapRect(paint.computeFastBounds(tmp, &tmp));
        if (!devRect.isEmpty()) {
            drawAreaRects_.push_back(devRect);
            totalOpNums_++;
            totalDrawAreas_ += (int)(devRect.width()) * (int)(devRect.height());
        }
        return true;
    }
    return false;
}

SkCanvas::SaveLayerStrategy SkiaCanvasAutoCache::getSaveLayerStrategy(const SaveLayerRec& rec)
{
    if (OpShouldRecord()) {
        if (rec.fPaint && !PaintCanCache(*rec.fPaint)) {
            RecordUnsupportOp(__func__, *rec.fPaint);
        }
    }
    return this->SkNWayCanvas::getSaveLayerStrategy(rec);
}

bool SkiaCanvasAutoCache::onDoSaveBehind(const SkRect* rect)
{
    RecordUnsupportOp(__func__);
    return this->SkNWayCanvas::onDoSaveBehind(rect);
}

void SkiaCanvasAutoCache::onDrawPaint(const SkPaint& paint)
{
    if ((!paint.nothingToDraw()) && !this->isClipEmpty()) {
        RecordUnsupportOp(__func__, paint);
    }
    this->SkNWayCanvas::onDrawPaint(paint);
}

void SkiaCanvasAutoCache::onDrawBehind(const SkPaint& paint)
{
    RecordUnsupportOp(__func__, paint);
    this->SkNWayCanvas::onDrawBehind(paint);
}

void SkiaCanvasAutoCache::onDrawPoints(PointMode mode, size_t count, const SkPoint pts[],
                                       const SkPaint& paint)
{
    if (count <= 0 || paint.nothingToDraw()) {
        return;
    }

    if (OpShouldRecord()) {
        SkRect bounds;
        if (count == 2) { // 2: two points
            bounds.set(pts[0], pts[1]);
        } else {
            bounds.setBounds(pts, SkToInt(count));
        }
        SkPaint strokePaint = paint;
        strokePaint.setStyle(SkPaint::kStroke_Style);
        if (!RecordDrawArea(bounds, strokePaint)) {
            RecordUnsupportOp(__func__, strokePaint);
        }
    }
    this->SkNWayCanvas::onDrawPoints(mode, count, pts, paint);
}

void SkiaCanvasAutoCache::onDrawRect(const SkRect& rect, const SkPaint& paint)
{
    if (OpShouldRecord()) {
        if (!RecordDrawArea(rect, paint)) {
            RecordUnsupportOp(__func__, paint);
        }
    }
    this->SkNWayCanvas::onDrawRect(rect, paint);
}

void SkiaCanvasAutoCache::onDrawRRect(const SkRRect& rrect, const SkPaint& paint)
{
    if (OpShouldRecord()) {
        const SkRect& bounds = rrect.getBounds();
        if (!RecordDrawArea(bounds, paint)) {
            RecordUnsupportOp(__func__, paint);
        }
    }
    this->SkNWayCanvas::onDrawRRect(rrect, paint);
}

void SkiaCanvasAutoCache::onDrawDRRect(const SkRRect& outer, const SkRRect& inner,
                                       const SkPaint& paint)
{
    if (OpShouldRecord()) {
        if (!RecordDrawArea(outer.getBounds(), paint)) {
            RecordUnsupportOp(__func__, paint);
        }
    }
    this->SkNWayCanvas::onDrawDRRect(outer, inner, paint);
}

void SkiaCanvasAutoCache::onDrawRegion(const SkRegion& region, const SkPaint& paint)
{
    if (OpShouldRecord()) {
        const SkRect bounds = SkRect::Make(region.getBounds());
        if (!RecordDrawArea(bounds, paint)) {
            RecordUnsupportOp(__func__, paint);
        }
    }
    this->SkNWayCanvas::onDrawRegion(region, paint);
}

void SkiaCanvasAutoCache::onDrawOval(const SkRect& rect, const SkPaint& paint)
{
    if (OpShouldRecord()) {
        if (!RecordDrawArea(rect, paint)) {
            RecordUnsupportOp(__func__, paint);
        }
    }
    this->SkNWayCanvas::onDrawOval(rect, paint);
}

void SkiaCanvasAutoCache::onDrawArc(const SkRect& rect, SkScalar startAngle, SkScalar sweepAngle,
                                    bool useCenter, const SkPaint& paint)
{
    if (OpShouldRecord()) {
        if (!RecordDrawArea(rect, paint)) {
            RecordUnsupportOp(__func__, paint);
        }
    }
    this->SkNWayCanvas::onDrawArc(rect, startAngle, sweepAngle, useCenter, paint);
}

void SkiaCanvasAutoCache::onDrawPath(const SkPath& path, const SkPaint& paint)
{
    if (OpShouldRecord()) {
        if (!path.isInverseFillType()) {
            const SkRect& pathBounds = path.getBounds();
            if (!RecordDrawArea(pathBounds, paint)) {
                RecordUnsupportOp(__func__, paint);
            }
        } else {
            RecordUnsupportOp(__func__, paint);
        }
    }
    this->SkNWayCanvas::onDrawPath(path, paint);
}

/* image not null, SkCanvas::drawImage will RETURN_ON_NULL(image) */
void SkiaCanvasAutoCache::onDrawImage2(const SkImage* image, SkScalar left, SkScalar top,
                                       const SkSamplingOptions& sampling, const SkPaint* paint)
{
    SkPaint realPaint = paint ? *paint : SkPaint();
    if (OpShouldRecord()) {
        SkRect bounds = SkRect::MakeXYWH(left, top, image->width(), image->height());
        if (!RecordDrawArea(bounds, realPaint)) {
            RecordUnsupportOp(__func__, realPaint);
        }
    }
    this->SkNWayCanvas::onDrawImage2(image, left, top, sampling, paint);
}

void SkiaCanvasAutoCache::onDrawImageRect2(const SkImage* image, const SkRect& src,
                                           const SkRect& dst, const SkSamplingOptions& sampling,
                                           const SkPaint* paint, SrcRectConstraint constraint)
{
    if (OpShouldRecord()) {
        SkPaint realPaint = ProcessPaintForImage(paint);
        if (!RecordDrawArea(dst, realPaint)) {
            RecordUnsupportOp(__func__, realPaint);
        }
    }
    this->SkNWayCanvas::onDrawImageRect2(image, src, dst, sampling, paint, constraint);
}

void SkiaCanvasAutoCache::onDrawImageLattice2(const SkImage* image, const Lattice& lattice,
                                              const SkRect& dst, SkFilterMode filter,
                                              const SkPaint* paint)
{
    if (OpShouldRecord()) {
        SkPaint realPaint = ProcessPaintForImage(paint);
        if (!RecordDrawArea(dst, realPaint)) {
            RecordUnsupportOp(__func__, realPaint);
        }
    }
    this->SkNWayCanvas::onDrawImageLattice2(image, lattice, dst, filter, paint);
}

/* vertices not null, SkCanvas::drawVertices() will RETURN_ON_NULL(vertices) */
void SkiaCanvasAutoCache::onDrawVerticesObject(const SkVertices* vertices,
                                               SkBlendMode bmode, const SkPaint& paint)
{
    if (OpShouldRecord()) {
        SkPaint realPaint = ProcessPaintForVertices(paint);
        if (!BlendModeCanCache(bmode) || !RecordDrawArea(vertices->bounds(), realPaint)) {
            RecordUnsupportOp(__func__, realPaint);
        }
    }
    this->SkNWayCanvas::onDrawVerticesObject(vertices, bmode, paint);
}

void SkiaCanvasAutoCache::onDrawPatch(const SkPoint cubics[12], const SkColor colors[4],
                                      const SkPoint texCoords[4], SkBlendMode bmode,
                                      const SkPaint& paint)
{
    if (OpShouldRecord()) {
        SkPaint realPaint = ProcessPaintForVertices(paint);
        SkRect bounds;
        bounds.setBounds(cubics, SkPatchUtils::kNumCtrlPts);
        if (!BlendModeCanCache(bmode) || !RecordDrawArea(bounds, realPaint)) {
            RecordUnsupportOp(__func__, realPaint);
        }
    }
    this->SkNWayCanvas::onDrawPatch(cubics, colors, texCoords, bmode, paint);
}

/* picture not null, SkCanvas::drawPicture() will RETURN_ON_NULL(picture) */
void SkiaCanvasAutoCache::onDrawPicture(const SkPicture* picture, const SkMatrix* m,
                                        const SkPaint* originalPaint)
{
    SkPaint tmpPaint = originalPaint ? *originalPaint : SkPaint();
    const SkPaint* newPaint = &tmpPaint;
    if ((picture->approximateOpCount() <= kMaxPictureOpsToUnrollInsteadOfRef) ||
        !RecordDrawArea(picture->cullRect(), tmpPaint, m)) {
        RecordUnsupportOp(__func__, tmpPaint);
    }

    if (!originalPaint) {
        if ((std::fabs(newPaint->getAlphaf() - 1.0f) <= std::numeric_limits<float>::epsilon()) &&
            newPaint->getColorFilter() == nullptr &&
            newPaint->getImageFilter() == nullptr && newPaint->asBlendMode() == SkBlendMode::kSrcOver) {
            newPaint = nullptr;
        }
    }
    this->SkNWayCanvas::onDrawPicture(picture, m, newPaint);
}

void SkiaCanvasAutoCache::onDrawDrawable(SkDrawable* drawable, const SkMatrix* matrix)
{
    this->SkNWayCanvas::onDrawDrawable(drawable, matrix);
}

void SkiaCanvasAutoCache::onDrawGlyphRunList(const SkGlyphRunList& list, const SkPaint& paint)
{
    if (OpShouldRecord()) {
        SkRect bounds = list.sourceBounds();
        if (!RecordDrawArea(bounds, paint)) {
            RecordUnsupportOp(__func__, paint);
        }
    }
    this->SkNWayCanvas::onDrawGlyphRunList(list, paint);
}

/* blob not null, SkCanvas::drawTextBlob() will RETURN_ON_NULL(blob) */
void SkiaCanvasAutoCache::onDrawTextBlob(const SkTextBlob* blob, SkScalar x, SkScalar y,
                                         const SkPaint& paint)
{
    if (OpShouldRecord()) {
        if (!RecordDrawArea(blob->bounds().makeOffset({x, y}), paint)) {
            RecordUnsupportOp(__func__, paint);
        }
    }
    this->SkNWayCanvas::onDrawTextBlob(blob, x, y, paint);
}

void SkiaCanvasAutoCache::onDrawAtlas2(const SkImage* image, const SkRSXform xform[],
                                       const SkRect tex[], const SkColor colors[], int count,
                                       SkBlendMode bmode, const SkSamplingOptions& sampling,
                                       const SkRect* cull, const SkPaint* paint)
{
    if (OpShouldRecord()) {
        SkPaint realPaint = ProcessPaintForVertices(ProcessPaintForImage(paint));
        if (!cull || !BlendModeCanCache(bmode) || !RecordDrawArea(*cull, realPaint)) {
            RecordUnsupportOp(__func__, realPaint);
        }
    }
    this->SkNWayCanvas::onDrawAtlas2(image, xform, tex, colors, count, bmode, sampling, cull, paint);
}

void SkiaCanvasAutoCache::onDrawAnnotation(const SkRect& rect, const char key[], SkData* value)
{
    // enable onDrawAnnotation for skp when debug
    RecordUnsupportOp(__func__);
    this->SkNWayCanvas::onDrawAnnotation(rect, key, value);
}

void SkiaCanvasAutoCache::onDrawShadowRec(const SkPath& path, const SkDrawShadowRec& rec)
{
    RecordUnsupportOp(__func__);
    this->SkNWayCanvas::onDrawShadowRec(path, rec);
}

void SkiaCanvasAutoCache::onDrawEdgeAAQuad(const SkRect& rect, const SkPoint clip[4],
                                           QuadAAFlags aa, const SkColor4f& color, SkBlendMode mode)
{
    if (OpShouldRecord()) {
        SkPaint paint;
        paint.setColor(color);
        paint.setBlendMode(mode);
        if (!BlendModeCanCache(mode) || !RecordDrawArea(rect, paint)) {
            RecordUnsupportOp(__func__, paint);
        }
    }
    this->SkNWayCanvas::onDrawEdgeAAQuad(rect, clip, aa, color, mode);
}

void SkiaCanvasAutoCache::onDrawEdgeAAImageSet2(const ImageSetEntry set[], int count,
                                                const SkPoint dstClips[],
                                                const SkMatrix preViewMatrices[],
                                                const SkSamplingOptions& sampling,
                                                const SkPaint* paint,
                                                SrcRectConstraint constraint)
{
    RecordUnsupportOp(__func__);
    this->SkNWayCanvas::onDrawEdgeAAImageSet2(
        set, count, dstClips, preViewMatrices, sampling, paint, constraint);
}

sk_sp<SkSurface> SkiaCanvasAutoCache::onNewSurface(const SkImageInfo& info,
                                                   const SkSurfaceProps& props)
{
    return this->proxy()->makeSurface(info, &props);
}

bool SkiaCanvasAutoCache::onPeekPixels(SkPixmap* pixmap)
{
    return this->proxy()->peekPixels(pixmap);
}

bool SkiaCanvasAutoCache::onAccessTopLayerPixels(SkPixmap* pixmap)
{
    SkImageInfo info;
    size_t rowByteSize;

    void* addr = this->proxy()->accessTopLayerPixels(&info, &rowByteSize);
    if (!addr) {
        return false;
    }

    if (pixmap) {
        pixmap->reset(info, addr, rowByteSize);
    }
    return true;
}

SkImageInfo SkiaCanvasAutoCache::onImageInfo() const
{
    return this->proxy()->imageInfo();
}

bool SkiaCanvasAutoCache::onGetProps(SkSurfaceProps* props) const
{
    return this->proxy()->getProps(props);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
