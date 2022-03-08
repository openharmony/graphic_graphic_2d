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

#include "pipeline/rs_recording_canvas.h"

#include "platform/common/rs_log.h"
#include "pipeline/rs_draw_cmd.h"

namespace OHOS {
namespace Rosen {
RSRecordingCanvas::RSRecordingCanvas(int width, int height) : SkCanvasVirtualEnforcer<SkNoDrawCanvas>(width, height)
{
    drawCmdList_ = std::make_shared<DrawCmdList>(width, height);
}

RSRecordingCanvas::~RSRecordingCanvas() {}

std::shared_ptr<DrawCmdList> RSRecordingCanvas::GetDrawCmdList() const
{
    return drawCmdList_;
}

void RSRecordingCanvas::Clear() const
{
    if (drawCmdList_ == nullptr) {
        return;
    }
    drawCmdList_->ClearOp();
}

void RSRecordingCanvas::AddOp(std::unique_ptr<OpItem>&& opItem)
{
    if (drawCmdList_ == nullptr || opItem == nullptr) {
        ROSEN_LOGE("RSRecordingCanvas:AddOp, drawCmdList_ or opItem is nullptr");
        return;
    }
    drawCmdList_->AddOp(std::move(opItem));
}

sk_sp<SkSurface> RSRecordingCanvas::onNewSurface(const SkImageInfo& info, const SkSurfaceProps& props)
{
    return nullptr;
}

void RSRecordingCanvas::onFlush()
{
    std::unique_ptr<OpItem> op = std::make_unique<FlushOpItem>();
    AddOp(std::move(op));
}

void RSRecordingCanvas::willSave()
{
    std::unique_ptr<OpItem> op = std::make_unique<SaveOpItem>();
    AddOp(std::move(op));
    saveCount_++;
}

SkCanvas::SaveLayerStrategy RSRecordingCanvas::getSaveLayerStrategy(const SaveLayerRec& rec)
{
    std::unique_ptr<OpItem> op = std::make_unique<SaveLayerOpItem>(rec);
    AddOp(std::move(op));
    saveCount_++;
    return SkCanvas::kNoLayer_SaveLayerStrategy;
}

void RSRecordingCanvas::willRestore()
{
    if (saveCount_ > 0) {
        std::unique_ptr<OpItem> op = std::make_unique<RestoreOpItem>();
        AddOp(std::move(op));
        --saveCount_;
    }
}

void RSRecordingCanvas::didConcat(const SkMatrix& matrix)
{
    std::unique_ptr<OpItem> op = std::make_unique<ConcatOpItem>(matrix);
    AddOp(std::move(op));
}

void RSRecordingCanvas::didSetMatrix(const SkMatrix& matrix)
{
    std::unique_ptr<OpItem> op = std::make_unique<MatrixOpItem>(matrix);
    AddOp(std::move(op));
}

void RSRecordingCanvas::didTranslate(SkScalar dx, SkScalar dy)
{
    std::unique_ptr<OpItem> op = std::make_unique<TranslateOpItem>(dx, dy);
    AddOp(std::move(op));
}

void RSRecordingCanvas::onClipRect(const SkRect& rect, SkClipOp clipOp, ClipEdgeStyle style)
{
    std::unique_ptr<OpItem> op = std::make_unique<ClipRectOpItem>(rect, clipOp, style == kSoft_ClipEdgeStyle);
    AddOp(std::move(op));
}

void RSRecordingCanvas::onClipRRect(const SkRRect& rrect, SkClipOp clipOp, ClipEdgeStyle style)
{
    std::unique_ptr<OpItem> op = std::make_unique<ClipRRectOpItem>(rrect, clipOp, style == kSoft_ClipEdgeStyle);
    AddOp(std::move(op));
}

void RSRecordingCanvas::onClipPath(const SkPath& path, SkClipOp clipOp, ClipEdgeStyle style)
{
    std::unique_ptr<OpItem> op = std::make_unique<ClipPathOpItem>(path, clipOp, style == kSoft_ClipEdgeStyle);
    AddOp(std::move(op));
}

void RSRecordingCanvas::onClipRegion(const SkRegion& region, SkClipOp clipop)
{
    std::unique_ptr<OpItem> op = std::make_unique<ClipRegionOpItem>(region, clipop);
    AddOp(std::move(op));
}

void RSRecordingCanvas::onDrawPaint(const SkPaint& paint)
{
    std::unique_ptr<OpItem> op = std::make_unique<PaintOpItem>(paint);
    AddOp(std::move(op));
}

void RSRecordingCanvas::DrawImageWithParm(const sk_sp<SkImage>img, int fitNum, int repeatNum, float radius,
    const SkPaint& paint)
{
    std::unique_ptr<OpItem> op = std::make_unique<ImageWithParmOpItem>(img, fitNum, repeatNum, radius, paint);
    AddOp(std::move(op));
}

void RSRecordingCanvas::onDrawBehind(const SkPaint& paint)
{
    // [PLANNING]: To be implemented
    ROSEN_LOGE("RSRecordingCanvas::onDrawBehind not support yet");
}

void RSRecordingCanvas::onDrawPath(const SkPath& path, const SkPaint& paint)
{
    std::unique_ptr<OpItem> op = std::make_unique<PathOpItem>(path, paint);
    AddOp(std::move(op));
}

void RSRecordingCanvas::onDrawRect(const SkRect& rect, const SkPaint& paint)
{
    std::unique_ptr<OpItem> op = std::make_unique<RectOpItem>(rect, paint);
    AddOp(std::move(op));
}

void RSRecordingCanvas::onDrawRegion(const SkRegion& region, const SkPaint& paint)
{
    std::unique_ptr<OpItem> op = std::make_unique<RegionOpItem>(region, paint);
    AddOp(std::move(op));
}

void RSRecordingCanvas::onDrawOval(const SkRect& oval, const SkPaint& paint)
{
    std::unique_ptr<OpItem> op = std::make_unique<OvalOpItem>(oval, paint);
    AddOp(std::move(op));
}

void RSRecordingCanvas::onDrawArc(
    const SkRect& oval, SkScalar startAngle, SkScalar sweepAngle, bool useCenter, const SkPaint& paint)
{
    std::unique_ptr<OpItem> op = std::make_unique<ArcOpItem>(oval, startAngle, sweepAngle, useCenter, paint);
    AddOp(std::move(op));
}

void RSRecordingCanvas::onDrawRRect(const SkRRect& rrect, const SkPaint& paint)
{
    std::unique_ptr<OpItem> op = std::make_unique<RoundRectOpItem>(rrect, paint);
    AddOp(std::move(op));
}

void RSRecordingCanvas::onDrawDRRect(const SkRRect& out, const SkRRect& in, const SkPaint& paint)
{
    std::unique_ptr<OpItem> op = std::make_unique<DRRectOpItem>(out, in, paint);
    AddOp(std::move(op));
}

void RSRecordingCanvas::onDrawDrawable(SkDrawable* drawable, const SkMatrix* matrix)
{
    std::unique_ptr<OpItem> op = std::make_unique<DrawableOpItem>(drawable, matrix);
    AddOp(std::move(op));
}

void RSRecordingCanvas::onDrawPicture(const SkPicture* picture, const SkMatrix* matrix, const SkPaint* paint)
{
    std::unique_ptr<OpItem> op = std::make_unique<PictureOpItem>(sk_ref_sp(picture), matrix, paint);
    AddOp(std::move(op));
}

void RSRecordingCanvas::onDrawAnnotation(const SkRect& rect, const char key[], SkData* val)
{
    // [PLANNING]: To be implemented
    ROSEN_LOGE("RSRecordingCanvas::onDrawAnnotation not support yet");
}

void RSRecordingCanvas::onDrawTextBlob(const SkTextBlob* blob, SkScalar x, SkScalar y, const SkPaint& paint)
{
    std::unique_ptr<OpItem> op = std::make_unique<TextBlobOpItem>(sk_ref_sp(blob), x, y, paint);
    AddOp(std::move(op));
}

void RSRecordingCanvas::onDrawBitmap(const SkBitmap& bm, SkScalar x, SkScalar y, const SkPaint* paint)
{
    std::unique_ptr<OpItem> op = std::make_unique<BitmapOpItem>(SkImage::MakeFromBitmap(bm), x, y, paint);
    AddOp(std::move(op));
}

void RSRecordingCanvas::onDrawBitmapNine(
    const SkBitmap& bm, const SkIRect& center, const SkRect& dst, const SkPaint* paint)
{
    std::unique_ptr<OpItem> op = std::make_unique<BitmapNineOpItem>(SkImage::MakeFromBitmap(bm), center, dst, paint);
    AddOp(std::move(op));
}

void RSRecordingCanvas::onDrawBitmapRect(
    const SkBitmap& bm, const SkRect* src, const SkRect& dst, const SkPaint* paint, SrcRectConstraint constraint)
{
    std::unique_ptr<OpItem> op = std::make_unique<BitmapRectOpItem>(SkImage::MakeFromBitmap(bm), src, dst, paint);
    AddOp(std::move(op));
}

void RSRecordingCanvas::onDrawBitmapLattice(
    const SkBitmap& bm, const SkCanvas::Lattice& lattice, const SkRect& dst, const SkPaint* paint)
{
    std::unique_ptr<OpItem> op =
        std::make_unique<BitmapLatticeOpItem>(SkImage::MakeFromBitmap(bm), lattice, dst, paint);
    AddOp(std::move(op));
}

void RSRecordingCanvas::onDrawImage(const SkImage* img, SkScalar x, SkScalar y, const SkPaint* paint)
{
    std::unique_ptr<OpItem> op = std::make_unique<BitmapOpItem>(sk_ref_sp(img), x, y, paint);
    AddOp(std::move(op));
}

void RSRecordingCanvas::onDrawImageNine(
    const SkImage* img, const SkIRect& center, const SkRect& dst, const SkPaint* paint)
{
    std::unique_ptr<OpItem> op = std::make_unique<BitmapNineOpItem>(sk_ref_sp(img), center, dst, paint);
    AddOp(std::move(op));
}

void RSRecordingCanvas::onDrawImageRect(
    const SkImage* img, const SkRect* src, const SkRect& dst, const SkPaint* paint, SrcRectConstraint constraint)
{
    std::unique_ptr<OpItem> op = std::make_unique<BitmapRectOpItem>(sk_ref_sp(img), src, dst, paint);
    AddOp(std::move(op));
}

void RSRecordingCanvas::onDrawImageLattice(
    const SkImage* img, const SkCanvas::Lattice& lattice, const SkRect& dst, const SkPaint* paint)
{
    std::unique_ptr<OpItem> op = std::make_unique<BitmapLatticeOpItem>(sk_ref_sp(img), lattice, dst, paint);
    AddOp(std::move(op));
}

void RSRecordingCanvas::DrawAdaptiveRRect(float radius, const SkPaint& paint)
{
    std::unique_ptr<OpItem> op = std::make_unique<AdaptiveRRectOpItem>(radius, paint);
    AddOp(std::move(op));
}

void RSRecordingCanvas::ClipAdaptiveRRect(float radius)
{
    std::unique_ptr<OpItem> op = std::make_unique<ClipAdaptiveRRectOpItem>(radius);
    AddOp(std::move(op));
}

void RSRecordingCanvas::onDrawPatch(const SkPoint cubics[12], const SkColor colors[4], const SkPoint texCoords[4],
    SkBlendMode bmode, const SkPaint& paint)
{
    // [PLANNING]: To be implemented
    ROSEN_LOGE("RSRecordingCanvas::onDrawPatch not support yet");
}

void RSRecordingCanvas::onDrawPoints(SkCanvas::PointMode mode, size_t count, const SkPoint pts[], const SkPaint& paint)
{
    std::unique_ptr<OpItem> op = std::make_unique<PointsOpItem>(mode, count, pts, paint);
    AddOp(std::move(op));
}

void RSRecordingCanvas::onDrawVerticesObject(
    const SkVertices* vertices, const SkVertices::Bone bones[], int boneCount, SkBlendMode mode, const SkPaint& paint)
{
    std::unique_ptr<OpItem> op = std::make_unique<VerticesOpItem>(vertices, bones, boneCount, mode, paint);
    AddOp(std::move(op));
}

void RSRecordingCanvas::onDrawAtlas(const SkImage* atlas, const SkRSXform xforms[], const SkRect texs[],
    const SkColor colors[], int count, SkBlendMode bmode, const SkRect* cull, const SkPaint* paint)
{
    // [PLANNING]: To be implemented
    ROSEN_LOGE("RSRecordingCanvas::onDrawAtlas not support yet");
}

void RSRecordingCanvas::onDrawShadowRec(const SkPath& path, const SkDrawShadowRec& rec)
{
    // [PLANNING]: To be implemented
    ROSEN_LOGE("RSRecordingCanvas::onDrawShadowRec not support yet");
}

void RSRecordingCanvas::MultiplyAlpha(float alpha)
{
    std::unique_ptr<OpItem> op = std::make_unique<MultiplyAlphaOpItem>(alpha);
    AddOp(std::move(op));
}

void RSRecordingCanvas::SaveAlpha()
{
    std::unique_ptr<OpItem> op = std::make_unique<SaveAlphaOpItem>();
    AddOp(std::move(op));
}

void RSRecordingCanvas::RestoreAlpha()
{
    std::unique_ptr<OpItem> op = std::make_unique<RestoreAlphaOpItem>();
    AddOp(std::move(op));
}
} // namespace Rosen
} // namespace OHOS
