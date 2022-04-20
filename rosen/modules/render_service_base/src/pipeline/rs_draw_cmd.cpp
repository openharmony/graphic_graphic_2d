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

ImageWithParmOpItem::ImageWithParmOpItem(const sk_sp<SkImage> img,
    const RsImageInfo& rsimageInfo, const SkPaint& paint)
    : OpItemWithPaint(sizeof(ImageWithParmOpItem))
{
    rsImage_ = std::make_shared<RSImage>();
    rsImage_->SetImage(img);
    rsImage_->SetImageFit(rsimageInfo.fitNum_);
    rsImage_->SetImageRepeat(rsimageInfo.repeatNum_);
    rsImage_->SetRadius(rsimageInfo.radius_);
    rsImage_->SetScale(rsimageInfo.scale_);
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

#ifdef ROSEN_OHOS

// RectOpItem
bool RectOpItem::Marshalling(Parcel& parcel) const
{
    bool success = true;
    success &= RSMarshallingHelper::Marshalling(parcel, rect_);
    success &= RSMarshallingHelper::Marshalling(parcel, paint_);
    return success;
}

OpItem* RectOpItem::Unmarshalling(Parcel& parcel)
{
    SkRect rect;
    SkPaint paint;
    if (!RSMarshallingHelper::Unmarshalling(parcel, rect)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, paint)) {
        return nullptr;
    }

    return new RectOpItem(rect, paint);
}

// RoundRectOpItem
bool RoundRectOpItem::Marshalling(Parcel& parcel) const
{
    bool success = true;
    success &= RSMarshallingHelper::Marshalling(parcel, rrect_);
    success &= RSMarshallingHelper::Marshalling(parcel, paint_);
    return success;
}

OpItem* RoundRectOpItem::Unmarshalling(Parcel& parcel)
{
    SkRRect rrect;
    SkPaint paint;
    if (!RSMarshallingHelper::Unmarshalling(parcel, rrect)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, paint)) {
        return nullptr;
    }

    return new RoundRectOpItem(rrect, paint);
}

// ImageWithParmOpItem
bool ImageWithParmOpItem::Marshalling(Parcel& parcel) const
{
    bool success = true;
    success &= rsImage_->Marshalling(parcel);
    success &= RSMarshallingHelper::Marshalling(parcel, paint_);
    return success;
}

OpItem* ImageWithParmOpItem::Unmarshalling(Parcel& parcel)
{
    sk_sp<SkImage> img;
    int fitNum;
    int repeatNum;
    float radius;
    SkPaint paint;
    if (!RSMarshallingHelper::Unmarshalling(parcel, img)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, fitNum)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, repeatNum)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, radius)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, paint)) {
        return nullptr;
    }

    return new ImageWithParmOpItem(img, fitNum, repeatNum, radius, paint);
}

// DRRectOpItem
bool DRRectOpItem::Marshalling(Parcel& parcel) const
{
    bool success = true;
    success &= RSMarshallingHelper::Marshalling(parcel, outer_);
    success &= RSMarshallingHelper::Marshalling(parcel, inner_);
    success &= RSMarshallingHelper::Marshalling(parcel, paint_);
    return success;
}

OpItem* DRRectOpItem::Unmarshalling(Parcel& parcel)
{
    SkRRect outer;
    SkRRect inner;
    SkPaint paint;
    if (!RSMarshallingHelper::Unmarshalling(parcel, outer)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, inner)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, paint)) {
        return nullptr;
    }

    return new DRRectOpItem(outer, inner, paint);
}

// OvalOpItem
bool OvalOpItem::Marshalling(Parcel& parcel) const
{
    bool success = true;
    success &= RSMarshallingHelper::Marshalling(parcel, rect_);
    success &= RSMarshallingHelper::Marshalling(parcel, paint_);
    return success;
}

OpItem* OvalOpItem::Unmarshalling(Parcel& parcel)
{
    SkRect rect;
    SkPaint paint;
    if (!RSMarshallingHelper::Unmarshalling(parcel, rect)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, paint)) {
        return nullptr;
    }

    return new OvalOpItem(rect, paint);
}

// RegionOpItem
bool RegionOpItem::Marshalling(Parcel& parcel) const
{
    bool success = true;
    success &= RSMarshallingHelper::Marshalling(parcel, region_);
    success &= RSMarshallingHelper::Marshalling(parcel, paint_);
    return success;
}

OpItem* RegionOpItem::Unmarshalling(Parcel& parcel)
{
    SkRegion region;
    SkPaint paint;
    if (!RSMarshallingHelper::Unmarshalling(parcel, region)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, paint)) {
        return nullptr;
    }

    return new RegionOpItem(region, paint);
}

// ArcOpItem
bool ArcOpItem::Marshalling(Parcel& parcel) const
{
    bool success = true;
    success &= RSMarshallingHelper::Marshalling(parcel, rect_);
    success &= RSMarshallingHelper::Marshalling(parcel, startAngle_);
    success &= RSMarshallingHelper::Marshalling(parcel, sweepAngle_);
    success &= RSMarshallingHelper::Marshalling(parcel, useCenter_);
    success &= RSMarshallingHelper::Marshalling(parcel, paint_);
    return success;
}

OpItem* ArcOpItem::Unmarshalling(Parcel& parcel)
{
    SkRect rect;
    float startAngle;
    float sweepAngle;
    bool useCenter;
    SkPaint paint;
    if (!RSMarshallingHelper::Unmarshalling(parcel, rect)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, startAngle)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, sweepAngle)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, useCenter)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, paint)) {
        return nullptr;
    }
    return new ArcOpItem(rect, startAngle, sweepAngle, useCenter, paint);
}

// SaveOpItem
OpItem* SaveOpItem::Unmarshalling(Parcel& parcel)
{
    return new SaveOpItem();
}

// RestoreOpItem
OpItem* RestoreOpItem::Unmarshalling(Parcel& parcel)
{
    return new RestoreOpItem();
}

// FlushOpItem
OpItem* FlushOpItem::Unmarshalling(Parcel& parcel)
{
    return new FlushOpItem();
}

// MatrixOpItem
bool MatrixOpItem::Marshalling(Parcel& parcel) const
{
    bool success = true;
    success &= RSMarshallingHelper::Marshalling(parcel, matrix_);
    return success;
}

OpItem* MatrixOpItem::Unmarshalling(Parcel& parcel)
{
    SkMatrix matrix;
    if (!RSMarshallingHelper::Unmarshalling(parcel, matrix)) {
        return nullptr;
    }
    return new MatrixOpItem(matrix);
}

// ClipRectOpItem
bool ClipRectOpItem::Marshalling(Parcel& parcel) const
{
    bool success = true;
    success &= RSMarshallingHelper::Marshalling(parcel, rect_);
    success &= RSMarshallingHelper::Marshalling(parcel, clipOp_);
    success &= RSMarshallingHelper::Marshalling(parcel, doAA_);
    return success;
}

OpItem* ClipRectOpItem::Unmarshalling(Parcel& parcel)
{
    SkRect rect;
    SkClipOp clipOp;
    bool doAA;
    if (!RSMarshallingHelper::Unmarshalling(parcel, rect)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, clipOp)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, doAA)) {
        return nullptr;
    }
    return new ClipRectOpItem(rect, clipOp, doAA);
}

// ClipRRectOpItem
bool ClipRRectOpItem::Marshalling(Parcel& parcel) const
{
    bool success = true;
    success &= RSMarshallingHelper::Marshalling(parcel, rrect_);
    success &= RSMarshallingHelper::Marshalling(parcel, clipOp_);
    success &= RSMarshallingHelper::Marshalling(parcel, doAA_);
    return success;
}

OpItem* ClipRRectOpItem::Unmarshalling(Parcel& parcel)
{
    SkRRect rrect;
    SkClipOp clipOp;
    bool doAA;
    if (!RSMarshallingHelper::Unmarshalling(parcel, rrect)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, clipOp)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, doAA)) {
        return nullptr;
    }
    return new ClipRRectOpItem(rrect, clipOp, doAA);
}

// ClipRegionOpItem
bool ClipRegionOpItem::Marshalling(Parcel& parcel) const
{
    bool success = true;
    success &= RSMarshallingHelper::Marshalling(parcel, region_);
    success &= RSMarshallingHelper::Marshalling(parcel, clipOp_);
    return success;
}

OpItem* ClipRegionOpItem::Unmarshalling(Parcel& parcel)
{
    SkRegion region;
    SkClipOp clipOp;
    if (!RSMarshallingHelper::Unmarshalling(parcel, region)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, clipOp)) {
        return nullptr;
    }
    return new ClipRegionOpItem(region, clipOp);
}

// TranslateOpItem
bool TranslateOpItem::Marshalling(Parcel& parcel) const
{
    bool success = true;
    success &= RSMarshallingHelper::Marshalling(parcel, distanceX_);
    success &= RSMarshallingHelper::Marshalling(parcel, distanceY_);
    return success;
}

OpItem* TranslateOpItem::Unmarshalling(Parcel& parcel)
{
    float distanceX;
    float distanceY;
    if (!RSMarshallingHelper::Unmarshalling(parcel, distanceX)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, distanceY)) {
        return nullptr;
    }
    return new TranslateOpItem(distanceX, distanceY);
}

// TextBlobOpItem
bool TextBlobOpItem::Marshalling(Parcel& parcel) const
{
    bool success = true;
    success &= RSMarshallingHelper::Marshalling(parcel, textBlob_);
    success &= RSMarshallingHelper::Marshalling(parcel, x_);
    success &= RSMarshallingHelper::Marshalling(parcel, y_);
    success &= RSMarshallingHelper::Marshalling(parcel, paint_);
    return success;
}

OpItem* TextBlobOpItem::Unmarshalling(Parcel& parcel)
{
    sk_sp<SkTextBlob> textBlob;
    float x;
    float y;
    SkPaint paint;
    if (!RSMarshallingHelper::Unmarshalling(parcel, textBlob)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, x)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, y)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, paint)) {
        return nullptr;
    }

    return new TextBlobOpItem(textBlob, x, y, paint);
}

// BitmapOpItem
bool BitmapOpItem::Marshalling(Parcel& parcel) const
{
    bool success = true;
    success &= RSMarshallingHelper::Marshalling(parcel, bitmapInfo_);
    success &= RSMarshallingHelper::Marshalling(parcel, left_);
    success &= RSMarshallingHelper::Marshalling(parcel, top_);
    success &= RSMarshallingHelper::Marshalling(parcel, paint_);
    return success;
}

OpItem* BitmapOpItem::Unmarshalling(Parcel& parcel)
{
    sk_sp<SkImage> bitmapInfo;
    float left;
    float top;
    SkPaint paint;
    if (!RSMarshallingHelper::Unmarshalling(parcel, bitmapInfo)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, left)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, top)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, paint)) {
        return nullptr;
    }

    return new BitmapOpItem(bitmapInfo, left, top, &paint);
}

// BitmapRectOpItem
bool BitmapRectOpItem::Marshalling(Parcel& parcel) const
{
    bool success = true;
    success &= RSMarshallingHelper::Marshalling(parcel, bitmapInfo_);
    success &= RSMarshallingHelper::Marshalling(parcel, rectSrc_);
    success &= RSMarshallingHelper::Marshalling(parcel, rectDst_);
    success &= RSMarshallingHelper::Marshalling(parcel, paint_);
    return success;
}

OpItem* BitmapRectOpItem::Unmarshalling(Parcel& parcel)
{
    sk_sp<SkImage> bitmapInfo;
    SkRect rectSrc;
    SkRect rectDst;
    SkPaint paint;
    if (!RSMarshallingHelper::Unmarshalling(parcel, bitmapInfo)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, rectSrc)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, rectDst)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, paint)) {
        return nullptr;
    }

    return new BitmapRectOpItem(bitmapInfo, &rectSrc, rectDst, &paint);
}

// BitmapNineOpItem
bool BitmapNineOpItem::Marshalling(Parcel& parcel) const
{
    bool success = true;
    success &= RSMarshallingHelper::Marshalling(parcel, bitmapInfo_);
    success &= RSMarshallingHelper::Marshalling(parcel, center_);
    success &= RSMarshallingHelper::Marshalling(parcel, rectDst_);
    success &= RSMarshallingHelper::Marshalling(parcel, paint_);
    return success;
}

OpItem* BitmapNineOpItem::Unmarshalling(Parcel& parcel)
{
    sk_sp<SkImage> bitmapInfo;
    SkIRect center;
    SkRect rectDst;
    SkPaint paint;
    if (!RSMarshallingHelper::Unmarshalling(parcel, bitmapInfo)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, center)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, rectDst)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, paint)) {
        return nullptr;
    }

    return new BitmapNineOpItem(bitmapInfo, center, rectDst, &paint);
}

// AdaptiveRRectOpItem
bool AdaptiveRRectOpItem::Marshalling(Parcel& parcel) const
{
    bool success = true;
    success &= RSMarshallingHelper::Marshalling(parcel, radius_);
    success &= RSMarshallingHelper::Marshalling(parcel, paint_);
    return success;
}

OpItem* AdaptiveRRectOpItem::Unmarshalling(Parcel& parcel)
{
    float radius;
    SkPaint paint;
    if (!RSMarshallingHelper::Unmarshalling(parcel, radius)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, paint)) {
        return nullptr;
    }

    return new AdaptiveRRectOpItem(radius, paint);
}

// ClipAdaptiveRRectOpItem
bool ClipAdaptiveRRectOpItem::Marshalling(Parcel& parcel) const
{
    bool success = true;
    success &= RSMarshallingHelper::Marshalling(parcel, radius_);
    return success;
}

OpItem* ClipAdaptiveRRectOpItem::Unmarshalling(Parcel& parcel)
{
    float radius;
    if (!RSMarshallingHelper::Unmarshalling(parcel, radius)) {
        return nullptr;
    }
    return new ClipAdaptiveRRectOpItem(radius);
}

// PathOpItem
bool PathOpItem::Marshalling(Parcel& parcel) const
{
    bool success = true;
    success &= RSMarshallingHelper::Marshalling(parcel, path_);
    success &= RSMarshallingHelper::Marshalling(parcel, paint_);
    return success;
}

OpItem* PathOpItem::Unmarshalling(Parcel& parcel)
{
    SkPath path;
    SkPaint paint;
    if (!RSMarshallingHelper::Unmarshalling(parcel, path)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, paint)) {
        return nullptr;
    }

    return new PathOpItem(path, paint);
}

// ClipPathOpItem
bool ClipPathOpItem::Marshalling(Parcel& parcel) const
{
    bool success = true;
    success &= RSMarshallingHelper::Marshalling(parcel, path_);
    success &= RSMarshallingHelper::Marshalling(parcel, clipOp_);
    success &= RSMarshallingHelper::Marshalling(parcel, doAA_);
    return success;
}

OpItem* ClipPathOpItem::Unmarshalling(Parcel& parcel)
{
    SkPath path;
    SkClipOp clipOp;
    bool doAA;
    if (!RSMarshallingHelper::Unmarshalling(parcel, path)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, clipOp)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, doAA)) {
        return nullptr;
    }

    return new ClipPathOpItem(path, clipOp, doAA);
}

// PaintOpItem
bool PaintOpItem::Marshalling(Parcel& parcel) const
{
    bool success = true;
    success &= RSMarshallingHelper::Marshalling(parcel, paint_);
    return success;
}

OpItem* PaintOpItem::Unmarshalling(Parcel& parcel)
{
    SkPaint paint;
    if (!RSMarshallingHelper::Unmarshalling(parcel, paint)) {
        return nullptr;
    }

    return new PaintOpItem(paint);
}

// ConcatOpItem
bool ConcatOpItem::Marshalling(Parcel& parcel) const
{
    bool success = true;
    success &= RSMarshallingHelper::Marshalling(parcel, matrix_);
    return success;
}

OpItem* ConcatOpItem::Unmarshalling(Parcel& parcel)
{
    SkMatrix matrix;
    if (!RSMarshallingHelper::Unmarshalling(parcel, matrix)) {
        return nullptr;
    }

    return new ConcatOpItem(matrix);
}

// SaveLayerOpItem
bool SaveLayerOpItem::Marshalling(Parcel& parcel) const
{
    bool success = true;
    success &= RSMarshallingHelper::Marshalling(parcel, rect_);
    success &= RSMarshallingHelper::Marshalling(parcel, backdrop_);
    success &= RSMarshallingHelper::Marshalling(parcel, mask_);
    success &= RSMarshallingHelper::Marshalling(parcel, matrix_);
    success &= RSMarshallingHelper::Marshalling(parcel, flags_);
    success &= RSMarshallingHelper::Marshalling(parcel, paint_);
    return success;
}

OpItem* SaveLayerOpItem::Unmarshalling(Parcel& parcel)
{
    SkRect rect;
    sk_sp<SkImageFilter> backdrop;
    sk_sp<SkImage> mask;
    SkMatrix matrix;
    SkCanvas::SaveLayerFlags flags;
    SkPaint paint;
    if (!RSMarshallingHelper::Unmarshalling(parcel, rect)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, backdrop)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, mask)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, matrix)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, flags)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, paint)) {
        return nullptr;
    }
    SkCanvas::SaveLayerRec rec = { &rect, &paint, backdrop.get(), mask.get(), &matrix, flags };

    return new SaveLayerOpItem(rec);
}

// DrawableOpItem
bool DrawableOpItem::Marshalling(Parcel& parcel) const
{
    bool success = true;
    success &= RSMarshallingHelper::Marshalling(parcel, drawable_);
    success &= RSMarshallingHelper::Marshalling(parcel, matrix_);
    return success;
}

OpItem* DrawableOpItem::Unmarshalling(Parcel& parcel)
{
    sk_sp<SkDrawable> drawable;
    SkMatrix matrix;
    if (!RSMarshallingHelper::Unmarshalling(parcel, drawable)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, matrix)) {
        return nullptr;
    }

    return new DrawableOpItem(drawable.release(), &matrix);
}

// PictureOpItem
bool PictureOpItem::Marshalling(Parcel& parcel) const
{
    bool success = true;
    success &= RSMarshallingHelper::Marshalling(parcel, picture_);
    success &= RSMarshallingHelper::Marshalling(parcel, matrix_);
    success &= RSMarshallingHelper::Marshalling(parcel, paint_);
    return success;
}

OpItem* PictureOpItem::Unmarshalling(Parcel& parcel)
{
    sk_sp<SkPicture> picture;
    SkMatrix matrix;
    SkPaint paint;
    if (!RSMarshallingHelper::Unmarshalling(parcel, picture)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, matrix)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, paint)) {
        return nullptr;
    }

    return new PictureOpItem(picture, &matrix, &paint);
}

// PointsOpItem
bool PointsOpItem::Marshalling(Parcel& parcel) const
{
    bool success = true;
    success &= RSMarshallingHelper::Marshalling(parcel, mode_);
    success &= RSMarshallingHelper::Marshalling(parcel, count_);
    success &= RSMarshallingHelper::Marshalling(parcel, processedPoints_, count_);
    success &= RSMarshallingHelper::Marshalling(parcel, paint_);
    return success;
}

OpItem* PointsOpItem::Unmarshalling(Parcel& parcel)
{
    SkCanvas::PointMode mode;
    int count;
    const SkPoint* processedPoints = nullptr;
    SkPaint paint;
    if (!RSMarshallingHelper::Unmarshalling(parcel, mode)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, count)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, processedPoints, count)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, paint)) {
        return nullptr;
    }

    return new PointsOpItem(mode, count, processedPoints, paint);
}

// VerticesOpItem
bool VerticesOpItem::Marshalling(Parcel& parcel) const
{
    bool success = true;
    success &= RSMarshallingHelper::Marshalling(parcel, vertices_);
    success &= RSMarshallingHelper::Marshalling(parcel, boneCount_);
    success &= RSMarshallingHelper::Marshalling(parcel, bones_, boneCount_);
    success &= RSMarshallingHelper::Marshalling(parcel, mode_);
    success &= RSMarshallingHelper::Marshalling(parcel, paint_);
    return success;
}

OpItem* VerticesOpItem::Unmarshalling(Parcel& parcel)
{
    sk_sp<SkVertices> vertices;
    const SkVertices::Bone* bones = nullptr;
    int boneCount;
    SkBlendMode mode;
    SkPaint paint;
    if (!RSMarshallingHelper::Unmarshalling(parcel, vertices)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, boneCount)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, bones, boneCount)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, mode)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, paint)) {
        return nullptr;
    }

    return new VerticesOpItem(vertices.get(), bones, boneCount, mode, paint);
}

// MultiplyAlphaOpItem
bool MultiplyAlphaOpItem::Marshalling(Parcel& parcel) const
{
    bool success = true;
    success &= RSMarshallingHelper::Marshalling(parcel, alpha_);
    return success;
}

OpItem* MultiplyAlphaOpItem::Unmarshalling(Parcel& parcel)
{
    float alpha;
    if (!RSMarshallingHelper::Unmarshalling(parcel, alpha)) {
        return nullptr;
    }
    return new MultiplyAlphaOpItem(alpha);
}

// SaveAlphaOpItem
OpItem* SaveAlphaOpItem::Unmarshalling(Parcel& parcel)
{
    return new SaveAlphaOpItem();
}

// RestoreAlphaOpItem
OpItem* RestoreAlphaOpItem::Unmarshalling(Parcel& parcel)
{
    return new RestoreAlphaOpItem();
}

#endif
} // namespace Rosen
} // namespace OHOS
