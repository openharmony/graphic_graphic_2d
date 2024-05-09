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
    opCanCache_ = false;
}

/* The intersecting regins are merged into one rect. The disjoint regions are not merged. */
void SkiaCanvasAutoCache::MergeDrawAreaRects()
{
}

void SkiaCanvasAutoCache::ShowDrawResult(const SkRect& bound)
{
}

bool SkiaCanvasAutoCache::RecordDrawArea(const SkRect& bounds, const SkPaint& paint, const SkMatrix* matrix)
{
    return false;
}

SkCanvas::SaveLayerStrategy SkiaCanvasAutoCache::getSaveLayerStrategy(const SaveLayerRec& rec)
{
    return this->SkNWayCanvas::getSaveLayerStrategy(rec);
}

bool SkiaCanvasAutoCache::onDoSaveBehind(const SkRect* rect)
{
    RecordUnsupportOp(__func__);
    return this->SkNWayCanvas::onDoSaveBehind(rect);
}

void SkiaCanvasAutoCache::onDrawPaint(const SkPaint& paint)
{
}

void SkiaCanvasAutoCache::onDrawBehind(const SkPaint& paint)
{
}

void SkiaCanvasAutoCache::onDrawPoints(PointMode mode, size_t count, const SkPoint pts[],
                                       const SkPaint& paint)
{
}

void SkiaCanvasAutoCache::onDrawRect(const SkRect& rect, const SkPaint& paint)
{
}

void SkiaCanvasAutoCache::onDrawRRect(const SkRRect& rrect, const SkPaint& paint)
{
}

void SkiaCanvasAutoCache::onDrawDRRect(const SkRRect& outer, const SkRRect& inner,
                                       const SkPaint& paint)
{
}

void SkiaCanvasAutoCache::onDrawRegion(const SkRegion& region, const SkPaint& paint)
{
}

void SkiaCanvasAutoCache::onDrawOval(const SkRect& rect, const SkPaint& paint)
{
}

void SkiaCanvasAutoCache::onDrawArc(const SkRect& rect, SkScalar startAngle, SkScalar sweepAngle,
                                    bool useCenter, const SkPaint& paint)
{
}

void SkiaCanvasAutoCache::onDrawPath(const SkPath& path, const SkPaint& paint)
{
}

/* image not null, SkCanvas::drawImage will RETURN_ON_NULL(image) */
void SkiaCanvasAutoCache::onDrawImage2(const SkImage* image, SkScalar left, SkScalar top,
                                       const SkSamplingOptions& sampling, const SkPaint* paint)
{
}

void SkiaCanvasAutoCache::onDrawImageRect2(const SkImage* image, const SkRect& src,
                                           const SkRect& dst, const SkSamplingOptions& sampling,
                                           const SkPaint* paint, SrcRectConstraint constraint)
{
}

void SkiaCanvasAutoCache::onDrawImageLattice2(const SkImage* image, const Lattice& lattice,
                                              const SkRect& dst, SkFilterMode filter,
                                              const SkPaint* paint)
{
}

/* vertices not null, SkCanvas::drawVertices() will RETURN_ON_NULL(vertices) */
void SkiaCanvasAutoCache::onDrawVerticesObject(const SkVertices* vertices,
                                               SkBlendMode bmode, const SkPaint& paint)
{
}

void SkiaCanvasAutoCache::onDrawPatch(const SkPoint cubics[12], const SkColor colors[4],
                                      const SkPoint texCoords[4], SkBlendMode bmode,
                                      const SkPaint& paint)
{
}

/* picture not null, SkCanvas::drawPicture() will RETURN_ON_NULL(picture) */
void SkiaCanvasAutoCache::onDrawPicture(const SkPicture* picture, const SkMatrix* m,
                                        const SkPaint* originalPaint)
{
}

void SkiaCanvasAutoCache::onDrawDrawable(SkDrawable* drawable, const SkMatrix* matrix)
{
}

void SkiaCanvasAutoCache::onDrawGlyphRunList(const SkGlyphRunList& list, const SkPaint& paint)
{
}

/* blob not null, SkCanvas::drawTextBlob() will RETURN_ON_NULL(blob) */
void SkiaCanvasAutoCache::onDrawTextBlob(const SkTextBlob* blob, SkScalar x, SkScalar y,
                                         const SkPaint& paint)
{
}

void SkiaCanvasAutoCache::onDrawAtlas2(const SkImage* image, const SkRSXform xform[],
                                       const SkRect tex[], const SkColor colors[], int count,
                                       SkBlendMode bmode, const SkSamplingOptions& sampling,
                                       const SkRect* cull, const SkPaint* paint)
{
}

void SkiaCanvasAutoCache::onDrawAnnotation(const SkRect& rect, const char key[], SkData* value)
{
}

void SkiaCanvasAutoCache::onDrawShadowRec(const SkPath& path, const SkDrawShadowRec& rec)
{
}

void SkiaCanvasAutoCache::onDrawEdgeAAQuad(const SkRect& rect, const SkPoint clip[4],
                                           QuadAAFlags aa, const SkColor4f& color, SkBlendMode mode)
{
}

void SkiaCanvasAutoCache::onDrawEdgeAAImageSet2(const ImageSetEntry set[], int count,
                                                const SkPoint dstClips[],
                                                const SkMatrix preViewMatrices[],
                                                const SkSamplingOptions& sampling,
                                                const SkPaint* paint,
                                                SrcRectConstraint constraint)
{
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
