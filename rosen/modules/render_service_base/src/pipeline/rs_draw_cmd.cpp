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

#include "pipeline/rs_draw_cmd.h"

#include "platform/common/rs_log.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_root_render_node.h"
#include "securec.h"
namespace OHOS {
namespace Rosen {
RectOpItem::RectOpItem(SkRect rect, const SkPaint& paint) : OpItemWithPaint(sizeof(RectOpItem)), rect_(rect)
{
    paint_ = paint;
}

void RectOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.drawRect(rect_, paint_);
}

RoundRectOpItem::RoundRectOpItem(const SkRRect& rrect, const SkPaint& paint)
    : OpItemWithPaint(sizeof(RoundRectOpItem)), rrect_(rrect)
{
    paint_ = paint;
}

void RoundRectOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.drawRRect(rrect_, paint_);
}

DRRectOpItem::DRRectOpItem(const SkRRect& outer, const SkRRect& inner, const SkPaint& paint)
    : OpItemWithPaint(sizeof(DRRectOpItem))
{
    outer_ = outer;
    inner_ = inner;
    paint_ = paint;
}

void DRRectOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.drawDRRect(outer_, inner_, paint_);
}

OvalOpItem::OvalOpItem(SkRect rect, const SkPaint& paint) : OpItemWithPaint(sizeof(OvalOpItem)), rect_(rect)
{
    paint_ = paint;
}

void OvalOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.drawOval(rect_, paint_);
}

RegionOpItem::RegionOpItem(SkRegion region, const SkPaint& paint) : OpItemWithPaint(sizeof(RegionOpItem))
{
    region_ = region;
    paint_ = paint;
}

void RegionOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.drawRegion(region_, paint_);
}

ArcOpItem::ArcOpItem(const SkRect& rect, float startAngle, float sweepAngle, bool useCenter, const SkPaint& paint)
    : OpItemWithPaint(sizeof(ArcOpItem)), rect_(rect), startAngle_(startAngle), sweepAngle_(sweepAngle),
      useCenter_(useCenter)
{
    paint_ = paint;
}

void ArcOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.drawArc(rect_, startAngle_, sweepAngle_, useCenter_, paint_);
}

SaveOpItem::SaveOpItem() : OpItem(sizeof(SaveOpItem)) {}

void SaveOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.save();
}

RestoreOpItem::RestoreOpItem() : OpItem(sizeof(RestoreOpItem)) {}

void RestoreOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.restore();
}

FlushOpItem::FlushOpItem() : OpItem(sizeof(FlushOpItem)) {}

void FlushOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.flush();
}

MatrixOpItem::MatrixOpItem(const SkMatrix& matrix) : OpItem(sizeof(MatrixOpItem)), matrix_(matrix) {}

void MatrixOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.setMatrix(matrix_);
}

ClipRectOpItem::ClipRectOpItem(const SkRect& rect, SkClipOp op, bool doAA)
    : OpItem(sizeof(ClipRectOpItem)), rect_(rect), clipOp_(op), doAA_(doAA)
{}

void ClipRectOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.clipRect(rect_, clipOp_, doAA_);
}

ClipRRectOpItem::ClipRRectOpItem(const SkRRect& rrect, SkClipOp op, bool doAA)
    : OpItem(sizeof(ClipRRectOpItem)), rrect_(rrect), clipOp_(op), doAA_(doAA)
{}

void ClipRRectOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.clipRRect(rrect_, clipOp_, doAA_);
}

ClipRegionOpItem::ClipRegionOpItem(const SkRegion& region, SkClipOp op)
    : OpItem(sizeof(ClipRegionOpItem)), region_(region), clipOp_(op)
{}

void ClipRegionOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.clipRegion(region_, clipOp_);
}

TranslateOpItem::TranslateOpItem(float distanceX, float distanceY)
    : OpItem(sizeof(TranslateOpItem)), distanceX_(distanceX), distanceY_(distanceY)
{}

void TranslateOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.translate(distanceX_, distanceY_);
}

TextBlobOpItem::TextBlobOpItem(const sk_sp<SkTextBlob> textBlob, float x, float y, const SkPaint& paint)
    : OpItemWithPaint(sizeof(TextBlobOpItem)), textBlob_(textBlob), x_(x), y_(y)
{
    paint_ = paint;
}

void TextBlobOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.drawTextBlob(textBlob_, x_, y_, paint_);
}

BitmapOpItem::BitmapOpItem(const sk_sp<SkImage> bitmapInfo, float left, float top, const SkPaint* paint)
    : OpItemWithPaint(sizeof(BitmapOpItem)), left_(left), top_(top)
{
    if (bitmapInfo != nullptr) {
        bitmapInfo_ = bitmapInfo;
    }
    if (paint) {
        paint_ = *paint;
    }
}

void BitmapOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.drawImage(bitmapInfo_, left_, top_, &paint_);
}

BitmapRectOpItem::BitmapRectOpItem(
    const sk_sp<SkImage> bitmapInfo, const SkRect* rectSrc, const SkRect& rectDst, const SkPaint* paint)
    : OpItemWithPaint(sizeof(BitmapRectOpItem)), rectDst_(rectDst)
{
    if (bitmapInfo != nullptr) {
        rectSrc_ = (rectSrc == nullptr) ? SkRect::MakeWH(bitmapInfo->width(), bitmapInfo->height()) : *rectSrc;
        bitmapInfo_ = bitmapInfo;
    } else {
        if (rectSrc != nullptr) {
            rectSrc_ = *rectSrc;
        }
    }
    if (paint) {
        paint_ = *paint;
    }
}

void BitmapRectOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.drawImageRect(bitmapInfo_, rectSrc_, rectDst_, &paint_);
}

BitmapLatticeOpItem::BitmapLatticeOpItem(
    const sk_sp<SkImage> bitmapInfo, const SkCanvas::Lattice& lattice, const SkRect& rect, const SkPaint* paint)
    : OpItemWithPaint(sizeof(BitmapLatticeOpItem))
{
    rect_ = rect;
    lattice_ = lattice;
    if (bitmapInfo != nullptr) {
        bitmapInfo_ = bitmapInfo;
    }
    if (paint) {
        paint_ = *paint;
    }
}

void BitmapLatticeOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.drawImageLattice(bitmapInfo_.get(), lattice_, rect_, &paint_);
}

BitmapNineOpItem::BitmapNineOpItem(
    const sk_sp<SkImage> bitmapInfo, const SkIRect& center, const SkRect& rectDst, const SkPaint* paint)
    : OpItemWithPaint(sizeof(BitmapNineOpItem)), center_(center), rectDst_(rectDst)
{
    if (bitmapInfo != nullptr) {
        bitmapInfo_ = bitmapInfo;
    }
    if (paint) {
        paint_ = *paint;
    }
}

void BitmapNineOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.drawImageNine(bitmapInfo_, center_, rectDst_, &paint_);
}

AdaptiveRRectOpItem::AdaptiveRRectOpItem(float radius, const SkPaint& paint)
    : OpItemWithPaint(sizeof(AdaptiveRRectOpItem)), radius_(radius), paint_(paint)
{}

void AdaptiveRRectOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect* rect) const
{
    if (!rect) {
        ROSEN_LOGE("AdaptiveRRectOpItem::Draw, skrect is null");
        return;
    }
    SkRRect rrect = SkRRect::MakeRectXY(*rect, radius_, radius_);
    canvas.drawRRect(rrect, paint_);
}

ClipAdaptiveRRectOpItem::ClipAdaptiveRRectOpItem(float radius)
    : OpItemWithPaint(sizeof(ClipAdaptiveRRectOpItem)), radius_(radius)
{}

void ClipAdaptiveRRectOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect* rect) const
{
    if (!rect) {
        ROSEN_LOGE("ClipAdaptiveRRectOpItem::Draw skrect is null");
        return;
    }
    SkRRect rrect = SkRRect::MakeRectXY(*rect, radius_, radius_);
    canvas.clipRRect(rrect, true);
}

PathOpItem::PathOpItem(const SkPath& path, const SkPaint& paint) : OpItemWithPaint(sizeof(PathOpItem))
{
    path_ = path;
    paint_ = paint;
}

void PathOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.drawPath(path_, paint_);
}

ClipPathOpItem::ClipPathOpItem(const SkPath& path, SkClipOp clipOp, bool doAA)
    : OpItem(sizeof(ClipPathOpItem)), path_(path), clipOp_(clipOp), doAA_(doAA)
{}

void ClipPathOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.clipPath(path_, clipOp_, doAA_);
}

PaintOpItem::PaintOpItem(const SkPaint& paint) : OpItemWithPaint(sizeof(PaintOpItem))
{
    paint_ = paint;
}

void PaintOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.drawPaint(paint_);
}

ImageWithParmOpItem::ImageWithParmOpItem(
    const sk_sp<SkImage> img, int fitNum, int repeatNum, float radius, const SkPaint& paint)
    : OpItemWithPaint(sizeof(ImageWithParmOpItem))
{
    rsImage_ = std::make_shared<RSImage>();
    rsImage_->SetImage(img);
    rsImage_->SetImageFit(fitNum);
    rsImage_->SetImageRepeat(repeatNum);
    rsImage_->SetRadius(radius);
    paint_ = paint;
}

void ImageWithParmOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect* rect) const
{
    if (!rect) {
        ROSEN_LOGE("agp_ace: no rect");
        return;
    }
    rsImage_->CanvasDrawImage(canvas, *rect, paint_);
}

ConcatOpItem::ConcatOpItem(const SkMatrix& matrix) : OpItem(sizeof(ConcatOpItem)), matrix_(matrix) {}

void ConcatOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.concat(matrix_);
}

SaveLayerOpItem::SaveLayerOpItem(const SkCanvas::SaveLayerRec& rec) : OpItemWithPaint(sizeof(SaveLayerOpItem))
{
    if (rec.fBounds) {
        rect_ = *rec.fBounds;
        rectPtr_ = &rect_;
    }
    if (rec.fPaint) {
        paint_ = *rec.fPaint;
    }
    backdrop_ = sk_ref_sp(rec.fBackdrop);
    mask_ = sk_ref_sp(rec.fClipMask);
    matrix_ = rec.fClipMatrix ? *(rec.fClipMatrix) : SkMatrix::I();
    flags_ = rec.fSaveLayerFlags;
}

void SaveLayerOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.saveLayer(
        { rectPtr_, &paint_, backdrop_.get(), mask_.get(), matrix_.isIdentity() ? nullptr : &matrix_, flags_ });
    if (paint_.getImageFilter() || paint_.getColorFilter()) {
        RSRootRenderNode::MarkForceRaster();
    }
}

DrawableOpItem::DrawableOpItem(SkDrawable* drawable, const SkMatrix* matrix) : OpItem(sizeof(DrawableOpItem))
{
    drawable_ = sk_ref_sp(drawable);
    if (matrix) {
        matrix_ = *matrix;
    }
}

void DrawableOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.drawDrawable(drawable_.get(), &matrix_);
}

PictureOpItem::PictureOpItem(const sk_sp<SkPicture> picture, const SkMatrix* matrix, const SkPaint* paint)
    : OpItemWithPaint(sizeof(PictureOpItem)), picture_(picture)
{
    if (matrix) {
        matrix_ = *matrix;
    }
    if (paint) {
        paint_ = *paint;
    }
}

void PictureOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.drawPicture(picture_, &matrix_, &paint_);
}

PointsOpItem::PointsOpItem(SkCanvas::PointMode mode, int count, const SkPoint processedPoints[], const SkPaint& paint)
    : OpItemWithPaint(sizeof(PointsOpItem)), mode_(mode), count_(count), processedPoints_(new SkPoint[count])
{
    errno_t ret = memcpy_s(processedPoints_, count * sizeof(SkPoint), processedPoints, count * sizeof(SkPoint));
    if (ret != EOK) {
        ROSEN_LOGE("PointsOpItem: memcpy failed!");
    }
    paint_ = paint;
}

void PointsOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.drawPoints(mode_, count_, processedPoints_, paint_);
}

VerticesOpItem::VerticesOpItem(const SkVertices* vertices, const SkVertices::Bone bones[],
    int boneCount, SkBlendMode mode, const SkPaint& paint)
    : OpItemWithPaint(sizeof(VerticesOpItem)), vertices_(sk_ref_sp(const_cast<SkVertices*>(vertices))),
      bones_(new SkVertices::Bone[boneCount]), boneCount_(boneCount), mode_(mode)
{
    errno_t ret = memcpy_s(bones_, boneCount * sizeof(SkVertices::Bone), bones, boneCount * sizeof(SkVertices::Bone));
    if (ret != EOK) {
        ROSEN_LOGE("VerticesOpItem: memcpy failed!");
    }
    paint_ = paint;
}

VerticesOpItem::~VerticesOpItem()
{
    delete[] bones_;
}

void VerticesOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.drawVertices(vertices_, bones_, boneCount_, mode_, paint_);
}

MultiplyAlphaOpItem::MultiplyAlphaOpItem(float alpha) : OpItem(sizeof(MultiplyAlphaOpItem)), alpha_(alpha) {}

void MultiplyAlphaOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.MultiplyAlpha(alpha_);
}

SaveAlphaOpItem::SaveAlphaOpItem() : OpItem(sizeof(SaveAlphaOpItem)) {}

void SaveAlphaOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.SaveAlpha();
}

RestoreAlphaOpItem::RestoreAlphaOpItem() : OpItem(sizeof(RestoreAlphaOpItem)) {}

void RestoreAlphaOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.RestoreAlpha();
}
} // namespace Rosen
} // namespace OHOS
