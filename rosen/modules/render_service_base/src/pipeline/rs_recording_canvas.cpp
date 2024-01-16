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

#ifndef USE_ROSEN_DRAWING
#include "pipeline/rs_recording_canvas.h"

#include "rs_trace.h"

#include "pipeline/rs_draw_cmd.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"

#define RS_DRAWOP_TRACE_FUNC() \
    if (RSSystemProperties::GetDrawOpTraceEnabled()) { \
        RS_TRACE_FUNC(); \
    } do {} while (0)

namespace OHOS {
namespace Rosen {
RSRecordingCanvas::RSRecordingCanvas(int width, int height) : SkCanvasVirtualEnforcer<SkNoDrawCanvas>(width, height)
{
    drawCmdList_ = std::make_shared<DrawCmdList>(width, height);
}

RSRecordingCanvas::~RSRecordingCanvas() = default;

std::shared_ptr<DrawCmdList> RSRecordingCanvas::GetDrawCmdList() const
{
    return drawCmdList_;
}

GrRecordingContext* RSRecordingCanvas::recordingContext()
{
    return grContext_;
}

void RSRecordingCanvas::SetGrRecordingContext(GrRecordingContext* context)
{
    grContext_ = context;
}

void RSRecordingCanvas::didConcat44(const SkM44& mat)
{
    RS_DRAWOP_TRACE_FUNC();
    std::unique_ptr<OpItem> op = std::make_unique<ConcatOpItem>(mat);
    AddOp(std::move(op));
}

void RSRecordingCanvas::didSetM44(const SkM44& mat)
{
    RS_DRAWOP_TRACE_FUNC();
    std::unique_ptr<OpItem> op = std::make_unique<MatrixOpItem>(mat);
    AddOp(std::move(op));
}

void RSRecordingCanvas::didScale(SkScalar dx, SkScalar dy)
{
    RS_DRAWOP_TRACE_FUNC();
    std::unique_ptr<OpItem> op = std::make_unique<ScaleOpItem>(dx, dy);
    AddOp(std::move(op));
}

void RSRecordingCanvas::onDrawGlyphRunList(const SkGlyphRunList& glyphRunList, const SkPaint& paint)
{
    ROSEN_LOGE("RSRecordingCanvas::onDrawGlyphRunList not support yet");
}
void RSRecordingCanvas::onDrawImage2(const SkImage* img, SkScalar dx, SkScalar dy,
    const SkSamplingOptions& samplingOptions, const SkPaint* paint)
{
    RS_DRAWOP_TRACE_FUNC();
    std::unique_ptr<OpItem> op = std::make_unique<BitmapOpItem>(sk_ref_sp(img), dx, dy, samplingOptions, paint);
    AddOp(std::move(op));
}
void RSRecordingCanvas::onDrawImageRect2(const SkImage* img, const SkRect& src, const SkRect& dst,
    const SkSamplingOptions& samplingOptions, const SkPaint* paint, SrcRectConstraint constraint)
{
    RS_DRAWOP_TRACE_FUNC();
    std::unique_ptr<OpItem> op =
        std::make_unique<BitmapRectOpItem>(sk_ref_sp(img), &src, dst, samplingOptions, paint, constraint);
    AddOp(std::move(op));
}
void RSRecordingCanvas::onDrawImageLattice2(const SkImage* img, const Lattice& lattice, const SkRect& dst,
    SkFilterMode mode, const SkPaint* paint)
{
    DrawImageLatticeAsBitmap(img, lattice, dst, paint);
}

void RSRecordingCanvas::onDrawAtlas2(const SkImage*, const SkRSXform[], const SkRect src[],
    const SkColor[], int count, SkBlendMode mode, const SkSamplingOptions& samplingOptions,
    const SkRect* cull, const SkPaint* paint)
{
    ROSEN_LOGE("RSRecordingCanvas::onDrawAtlas2 not support yet");
}
void RSRecordingCanvas::onDrawEdgeAAImageSet2(const ImageSetEntry imageSet[], int count,
    const SkPoint dstClips[], const SkMatrix preViewMatrices[],
    const SkSamplingOptions& SamplingOptions, const SkPaint* paint,
    SrcRectConstraint constraint)
{
    ROSEN_LOGE("RSRecordingCanvas::onDrawEdgeAAImageSet2 not support yet");
}
void RSRecordingCanvas::onDrawVerticesObject(const SkVertices* vertices, SkBlendMode mode, const SkPaint& paint)
{
    RS_DRAWOP_TRACE_FUNC();
    std::unique_ptr<OpItem> op = std::make_unique<VerticesOpItem>(vertices, mode, paint);
    AddOp(std::move(op));
}
void RSRecordingCanvas::onDrawEdgeAAQuad(const SkRect& rect, const SkPoint clip[4], QuadAAFlags aaFlags,
    const SkColor4f& color, SkBlendMode mode)
{
    ROSEN_LOGE("RSRecordingCanvas::onDrawEdgeAAQuad not support yet");
}

void RSRecordingCanvas::DrawPixelMapRect(
    const std::shared_ptr<Media::PixelMap>& pixelmap, const SkRect& src, const SkRect& dst,
    const SkSamplingOptions& samplingOptions, const SkPaint* paint, SrcRectConstraint constraint)
{
    RS_DRAWOP_TRACE_FUNC();
    std::unique_ptr<OpItem> op =
        std::make_unique<PixelMapRectOpItem>(pixelmap, src, dst, samplingOptions, paint, constraint);
    AddOp(std::move(op));
}

void RSRecordingCanvas::DrawPixelMapRect(const std::shared_ptr<Media::PixelMap>& pixelmap, const SkRect& dst,
    const SkSamplingOptions& samplingOptions, const SkPaint* paint)
{
    DrawPixelMapRect(pixelmap, SkRect::MakeIWH(pixelmap->GetWidth(), pixelmap->GetHeight()),
        dst, samplingOptions, paint);
}

void RSRecordingCanvas::DrawRsImage(const std::shared_ptr<RSImageBase>& rsImage,
    const SkSamplingOptions& samplingOptions, const SkPaint& paint, SrcRectConstraint constraint)
{
    RS_DRAWOP_TRACE_FUNC();
    std::unique_ptr<OpItem> op = std::make_unique<PixelMapRectOpItem>(rsImage, samplingOptions, paint, constraint);
    AddOp(std::move(op));
}

void RSRecordingCanvas::DrawPixelMap(const std::shared_ptr<Media::PixelMap>& pixelmap, SkScalar x, SkScalar y,
    const SkSamplingOptions& samplingOptions, const SkPaint* paint)
{
    RS_DRAWOP_TRACE_FUNC();
    std::unique_ptr<OpItem> op = std::make_unique<PixelMapOpItem>(pixelmap, x, y, samplingOptions, paint);
    AddOp(std::move(op));
}

void RSRecordingCanvas::DrawImageWithParm(const sk_sp<SkImage>img, const sk_sp<SkData> data,
    const Rosen::RsImageInfo& rsimageInfo, const SkSamplingOptions& samplingOptions, const SkPaint& paint)
{
    RS_DRAWOP_TRACE_FUNC();
    std::unique_ptr<OpItem> op = std::make_unique<ImageWithParmOpItem>(
        img, data, rsimageInfo, samplingOptions, paint);
    AddOp(std::move(op));
}

void RSRecordingCanvas::DrawPixelMapWithParm(const std::shared_ptr<Media::PixelMap>& pixelmap,
    const Rosen::RsImageInfo& rsImageInfo, const SkSamplingOptions& samplingOptions, const SkPaint& paint)
{
    RS_DRAWOP_TRACE_FUNC();
    std::unique_ptr<OpItem> op = std::make_unique<ImageWithParmOpItem>(pixelmap, rsImageInfo, samplingOptions, paint);
    AddOp(std::move(op));
}

void RSRecordingCanvas::DrawImageNine(
    const std::shared_ptr<Media::PixelMap>& pixelmap, const SkIRect& center, const SkRect& dst,
    SkFilterMode filter, const SkPaint* paint)
{
    RS_DRAWOP_TRACE_FUNC();
    std::unique_ptr<OpItem> op = std::make_unique<PixelmapNineOpItem>(pixelmap, center, dst, filter, paint);
    AddOp(std::move(op));
}

void RSRecordingCanvas::DrawImageLatticeAsBitmap(
    const SkImage* image, const SkCanvas::Lattice& lattice, const SkRect& dst, const SkPaint* paint)
{
    SkBitmap bitmap;
    auto imageInfo = SkImageInfo::Make(dst.width(), dst.height(), kRGBA_8888_SkColorType, kPremul_SkAlphaType);
    bitmap.allocPixels(imageInfo);
    SkCanvas tempCanvas(bitmap);
    // align to [0, 0]
    tempCanvas.translate(-dst.left(), -dst.top());
    tempCanvas.drawImageLattice(image, lattice, dst, SkFilterMode::kNearest, paint);
    tempCanvas.flush();
    // draw on canvas with correct offset
    drawImage(bitmap.asImage(), dst.left(), dst.top());
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
    RS_DRAWOP_TRACE_FUNC();
    std::unique_ptr<OpItem> op = std::make_unique<FlushOpItem>();
    AddOp(std::move(op));
}

void RSRecordingCanvas::willSave()
{
    RS_DRAWOP_TRACE_FUNC();
    std::unique_ptr<OpItem> op = std::make_unique<SaveOpItem>();
    AddOp(std::move(op));
    saveCount_++;
}

SkCanvas::SaveLayerStrategy RSRecordingCanvas::getSaveLayerStrategy(const SaveLayerRec& rec)
{
    RS_DRAWOP_TRACE_FUNC();
    std::unique_ptr<OpItem> op = std::make_unique<SaveLayerOpItem>(rec);
    AddOp(std::move(op));
    saveCount_++;
    return SkCanvas::kNoLayer_SaveLayerStrategy;
}

void RSRecordingCanvas::willRestore()
{
    if (saveCount_ > 0) {
        RS_DRAWOP_TRACE_FUNC();
        std::unique_ptr<OpItem> op = std::make_unique<RestoreOpItem>();
        AddOp(std::move(op));
        --saveCount_;
    }
}

void RSRecordingCanvas::didTranslate(SkScalar dx, SkScalar dy)
{
    RS_DRAWOP_TRACE_FUNC();
    std::unique_ptr<OpItem> op = std::make_unique<TranslateOpItem>(dx, dy);
    AddOp(std::move(op));
}

void RSRecordingCanvas::onClipRect(const SkRect& rect, SkClipOp clipOp, ClipEdgeStyle style)
{
    RS_DRAWOP_TRACE_FUNC();
    std::unique_ptr<OpItem> op = std::make_unique<ClipRectOpItem>(rect, clipOp, style == kSoft_ClipEdgeStyle);
    AddOp(std::move(op));
}

void RSRecordingCanvas::onClipRRect(const SkRRect& rrect, SkClipOp clipOp, ClipEdgeStyle style)
{
    RS_DRAWOP_TRACE_FUNC();
    std::unique_ptr<OpItem> op = std::make_unique<ClipRRectOpItem>(rrect, clipOp, style == kSoft_ClipEdgeStyle);
    AddOp(std::move(op));
}

void RSRecordingCanvas::onClipPath(const SkPath& path, SkClipOp clipOp, ClipEdgeStyle style)
{
    RS_DRAWOP_TRACE_FUNC();
    std::unique_ptr<OpItem> op = std::make_unique<ClipPathOpItem>(path, clipOp, style == kSoft_ClipEdgeStyle);
    AddOp(std::move(op));
}

void RSRecordingCanvas::onClipRegion(const SkRegion& region, SkClipOp clipop)
{
    RS_DRAWOP_TRACE_FUNC();
    std::unique_ptr<OpItem> op = std::make_unique<ClipRegionOpItem>(region, clipop);
    AddOp(std::move(op));
}

void RSRecordingCanvas::onDrawPaint(const SkPaint& paint)
{
    RS_DRAWOP_TRACE_FUNC();
    std::unique_ptr<OpItem> op = std::make_unique<PaintOpItem>(paint);
    AddOp(std::move(op));
}

#ifdef ROSEN_OHOS
void RSRecordingCanvas::DrawSurfaceBuffer(const RSSurfaceBufferInfo& surfaceBufferInfo)
{
    RS_DRAWOP_TRACE_FUNC();
    std::unique_ptr<OpItem> op = std::make_unique<SurfaceBufferOpItem>(surfaceBufferInfo);
    AddOp(std::move(op));
}
#endif

void RSRecordingCanvas::onDrawBehind(const SkPaint& paint)
{
    // [PLANNING]: To be implemented
    ROSEN_LOGE("RSRecordingCanvas::onDrawBehind not support yet");
}

void RSRecordingCanvas::onDrawPath(const SkPath& path, const SkPaint& paint)
{
    RS_DRAWOP_TRACE_FUNC();
    std::unique_ptr<OpItem> op = std::make_unique<PathOpItem>(path, paint);
    AddOp(std::move(op));
}

void RSRecordingCanvas::onDrawRect(const SkRect& rect, const SkPaint& paint)
{
    RS_DRAWOP_TRACE_FUNC();
    std::unique_ptr<OpItem> op = std::make_unique<RectOpItem>(rect, paint);
    AddOp(std::move(op));
}

void RSRecordingCanvas::onDrawRegion(const SkRegion& region, const SkPaint& paint)
{
    RS_DRAWOP_TRACE_FUNC();
    std::unique_ptr<OpItem> op = std::make_unique<RegionOpItem>(region, paint);
    AddOp(std::move(op));
}

void RSRecordingCanvas::onDrawOval(const SkRect& oval, const SkPaint& paint)
{
    RS_DRAWOP_TRACE_FUNC();
    std::unique_ptr<OpItem> op = std::make_unique<OvalOpItem>(oval, paint);
    AddOp(std::move(op));
}

void RSRecordingCanvas::onDrawArc(
    const SkRect& oval, SkScalar startAngle, SkScalar sweepAngle, bool useCenter, const SkPaint& paint)
{
    RS_DRAWOP_TRACE_FUNC();
    std::unique_ptr<OpItem> op = std::make_unique<ArcOpItem>(oval, startAngle, sweepAngle, useCenter, paint);
    AddOp(std::move(op));
}

void RSRecordingCanvas::onDrawRRect(const SkRRect& rrect, const SkPaint& paint)
{
    RS_DRAWOP_TRACE_FUNC();
    std::unique_ptr<OpItem> op = std::make_unique<RoundRectOpItem>(rrect, paint);
    AddOp(std::move(op));
}

void RSRecordingCanvas::onDrawDRRect(const SkRRect& out, const SkRRect& in, const SkPaint& paint)
{
    RS_DRAWOP_TRACE_FUNC();
    std::unique_ptr<OpItem> op = std::make_unique<DRRectOpItem>(out, in, paint);
    AddOp(std::move(op));
}

void RSRecordingCanvas::onDrawDrawable(SkDrawable* drawable, const SkMatrix* matrix)
{
    RS_DRAWOP_TRACE_FUNC();
    std::unique_ptr<OpItem> op = std::make_unique<DrawableOpItem>(drawable, matrix);
    AddOp(std::move(op));
}

void RSRecordingCanvas::onDrawPicture(const SkPicture* picture, const SkMatrix* matrix, const SkPaint* paint)
{
    RS_DRAWOP_TRACE_FUNC();
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
    RS_DRAWOP_TRACE_FUNC();
    std::unique_ptr<OpItem> op = std::make_unique<TextBlobOpItem>(sk_ref_sp(blob), x, y, paint);
    if (IsCustomTextType()) {
        // replace drawOpItem with cached one (generated by CPU)
        ROSEN_LOGD("RSRecordingCanvas::onDrawTextBlob replace drawOpItem with cached one");
        op = op->GenerateCachedOpItem();
    }
    AddOp(std::move(op));
}

void RSRecordingCanvas::onDrawSymbol(const HMSymbolData& symbol, SkPoint locate, const SkPaint& paint)
{
    RS_DRAWOP_TRACE_FUNC();
    std::unique_ptr<OpItem> op = std::make_unique<SymbolOpItem>(symbol, locate, paint);
    AddOp(std::move(op));
}

void RSRecordingCanvas::DrawAdaptiveRRect(float radius, const SkPaint& paint)
{
    RS_DRAWOP_TRACE_FUNC();
    std::unique_ptr<OpItem> op = std::make_unique<AdaptiveRRectOpItem>(radius, paint);
    AddOp(std::move(op));
}

void RSRecordingCanvas::DrawAdaptiveRRectScale(float radiusRatio, const SkPaint& paint)
{
    RS_DRAWOP_TRACE_FUNC();
    std::unique_ptr<OpItem> op = std::make_unique<AdaptiveRRectScaleOpItem>(radiusRatio, paint);
    AddOp(std::move(op));
}

void RSRecordingCanvas::ClipAdaptiveRRect(const SkVector radius[])
{
    RS_DRAWOP_TRACE_FUNC();
    std::unique_ptr<OpItem> op = std::make_unique<ClipAdaptiveRRectOpItem>(radius);
    AddOp(std::move(op));
}

void RSRecordingCanvas::ClipOutsetRect(float dx, float dy)
{
    RS_DRAWOP_TRACE_FUNC();
    std::unique_ptr<OpItem> op = std::make_unique<ClipOutsetRectOpItem>(dx, dy);
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
    RS_DRAWOP_TRACE_FUNC();
    std::unique_ptr<OpItem> op = std::make_unique<PointsOpItem>(mode, count, pts, paint);
    AddOp(std::move(op));
}

void RSRecordingCanvas::onDrawShadowRec(const SkPath& path, const SkDrawShadowRec& rec)
{
    RS_DRAWOP_TRACE_FUNC();
    std::unique_ptr<OpItem> op = std::make_unique<ShadowRecOpItem>(path, rec);
    AddOp(std::move(op));
}

void RSRecordingCanvas::MultiplyAlpha(float alpha)
{
    RS_DRAWOP_TRACE_FUNC();
    std::unique_ptr<OpItem> op = std::make_unique<MultiplyAlphaOpItem>(alpha);
    AddOp(std::move(op));
}

void RSRecordingCanvas::SaveAlpha()
{
    RS_DRAWOP_TRACE_FUNC();
    std::unique_ptr<OpItem> op = std::make_unique<SaveAlphaOpItem>();
    AddOp(std::move(op));
}

void RSRecordingCanvas::RestoreAlpha()
{
    RS_DRAWOP_TRACE_FUNC();
    std::unique_ptr<OpItem> op = std::make_unique<RestoreAlphaOpItem>();
    AddOp(std::move(op));
}

void RSRecordingCanvas::SetIsCustomTextType(bool isCustomTextType)
{
    isCustomTextType_ = isCustomTextType;
}

bool RSRecordingCanvas::IsCustomTextType() const
{
    return isCustomTextType_;
}
void RSRecordingCanvas::DrawDrawFunc(DrawFunc && drawFunc)
{
    RS_DRAWOP_TRACE_FUNC();
    std::unique_ptr<OpItem> op = std::make_unique<DrawFuncOpItem>(std::move(drawFunc));
    AddOp(std::move(op));
}
} // namespace Rosen
} // namespace OHOS

#else
#include "pipeline/rs_recording_canvas.h"
#include "pipeline/rs_draw_cmd.h"
#include "recording/cmd_list_helper.h"
#include "render/rs_pixel_map_util.h"

namespace OHOS {
namespace Rosen {

ExtendRecordingCanvas::ExtendRecordingCanvas(int width, int height, bool addDrawOpImmediate)
    : Drawing::RecordingCanvas(width, height, addDrawOpImmediate) {}

void ExtendRecordingCanvas::DrawImageWithParm(
    const std::shared_ptr<Drawing::Image>& image, const std::shared_ptr<Drawing::Data>& data,
    const Drawing::AdaptiveImageInfo& rsImageInfo, const Drawing::SamplingOptions& sampling)
{
    if (!addDrawOpImmediate_) {
        AddDrawOpDeferred<Drawing::DrawImageWithParmOpItem>(image, data, rsImageInfo, sampling);
        return;
    }
    auto object = std::make_shared<RSExtendImageObject>(image, data, rsImageInfo);
    auto drawCallList = Drawing::RecordingCanvas::GetDrawCmdList();
    auto objectHandle =
        Drawing::CmdListHelper::AddImageObjectToCmdList(*drawCallList, object);
    AddDrawOpImmediate<Drawing::DrawImageWithParmOpItem::ConstructorHandle>(objectHandle, sampling);
}

void ExtendRecordingCanvas::DrawPixelMapWithParm(const std::shared_ptr<Media::PixelMap>& pixelMap,
    const Drawing::AdaptiveImageInfo& rsImageInfo, const Drawing::SamplingOptions& sampling)
{
    if (!addDrawOpImmediate_) {
        AddDrawOpDeferred<Drawing::DrawPixelMapWithParmOpItem>(pixelMap, rsImageInfo, sampling);
        return;
    }
    auto object = std::make_shared<RSExtendImageObject>(pixelMap, rsImageInfo);
    auto drawCallList = Drawing::RecordingCanvas::GetDrawCmdList();
    auto objectHandle =
        Drawing::CmdListHelper::AddImageObjectToCmdList(*drawCallList, object);
    AddDrawOpImmediate<Drawing::DrawPixelMapWithParmOpItem::ConstructorHandle>(objectHandle, sampling);
}

void ExtendRecordingCanvas::DrawPixelMapRect(const std::shared_ptr<Media::PixelMap>& pixelMap, const Drawing::Rect& src,
    const Drawing::Rect& dst, const Drawing::SamplingOptions& sampling,
    Drawing::SrcRectConstraint constraint)
{
    if (!addDrawOpImmediate_) {
        AddDrawOpDeferred<Drawing::DrawPixelMapRectOpItem>(pixelMap, src, dst, sampling);
        return;
    }
    auto object = std::make_shared<RSExtendImageBaseObj>(pixelMap, src, dst);
    auto drawCallList = Drawing::RecordingCanvas::GetDrawCmdList();
    auto objectHandle =
        Drawing::CmdListHelper::AddImageBaseObjToCmdList(*drawCallList, object);
    AddDrawOpImmediate<Drawing::DrawPixelMapRectOpItem::ConstructorHandle>(objectHandle, sampling);
}

void ExtendRecordingCanvas::DrawDrawFunc(Drawing::RecordingCanvas::DrawFunc&& drawFunc)
{
    if (!addDrawOpImmediate_) {
        cmdList_->AddDrawOp(std::make_shared<Drawing::DrawFuncOpItem>(std::move(drawFunc)));
        return;
    }
    auto object = std::make_shared<RSExtendDrawFuncObj>(std::move(drawFunc));
    auto drawCallList = Drawing::RecordingCanvas::GetDrawCmdList();
    auto objectHandle =
        Drawing::CmdListHelper::AddDrawFuncObjToCmdList(*drawCallList, object);
    cmdList_->AddOp<Drawing::DrawFuncOpItem::ConstructorHandle>(objectHandle);
}

#ifdef ROSEN_OHOS
void ExtendRecordingCanvas::DrawSurfaceBuffer(const DrawingSurfaceBufferInfo& surfaceBufferInfo)
{
    if (!addDrawOpImmediate_) {
        AddDrawOpDeferred<Drawing::DrawSurfaceBufferOpItem>(surfaceBufferInfo);
        return;
    }
    AddDrawOpImmediate<Drawing::DrawSurfaceBufferOpItem::ConstructorHandle>(
        Drawing::CmdListHelper::AddSurfaceBufferToCmdList(*cmdList_, surfaceBufferInfo.surfaceBuffer_),
        surfaceBufferInfo.offSetX_, surfaceBufferInfo.offSetY_,
        surfaceBufferInfo.width_, surfaceBufferInfo.height_);
}
#endif

template<typename T, typename... Args>
void ExtendRecordingCanvas::AddDrawOpImmediate(Args&&... args)
{
    bool brushValid = paintBrush_.IsValid();
    bool penValid = paintPen_.IsValid();
    if (!brushValid && !penValid) {
        Drawing::PaintHandle paintHandle;
        paintHandle.isAntiAlias = true;
        paintHandle.style = Drawing::Paint::PaintStyle::PAINT_FILL;
        cmdList_->AddDrawOp<T>(std::forward<Args>(args)..., paintHandle);
        return;
    }
    if (brushValid && penValid && Drawing::Paint::CanCombinePaint(paintBrush_, paintPen_)) {
        Drawing::PaintHandle paintHandle;
        paintPen_.SetStyle(Drawing::Paint::PaintStyle::PAINT_FILL_STROKE);
        Drawing::DrawOpItem::GenerateHandleFromPaint(*cmdList_, paintPen_, paintHandle);
        cmdList_->AddDrawOp<T>(std::forward<Args>(args)..., paintHandle);
        paintPen_.SetStyle(Drawing::Paint::PaintStyle::PAINT_STROKE);
        return;
    }
    if (brushValid) {
        Drawing::PaintHandle paintHandle;
        Drawing::DrawOpItem::GenerateHandleFromPaint(*cmdList_, paintBrush_, paintHandle);
        cmdList_->AddDrawOp<T>(std::forward<Args>(args)..., paintHandle);
    }
    if (penValid) {
        Drawing::PaintHandle paintHandle;
        Drawing::DrawOpItem::GenerateHandleFromPaint(*cmdList_, paintPen_, paintHandle);
        cmdList_->AddDrawOp<T>(std::forward<Args>(args)..., paintHandle);
    }
}

template<typename T, typename... Args>
void ExtendRecordingCanvas::AddDrawOpDeferred(Args&&... args)
{
    bool brushValid = paintBrush_.IsValid();
    bool penValid = paintPen_.IsValid();
    if (!brushValid && !penValid) {
        Drawing::Paint paint;
        paint.SetAntiAlias(true);
        paint.SetStyle(Drawing::Paint::PaintStyle::PAINT_FILL);
        cmdList_->AddDrawOp(std::make_shared<T>(std::forward<Args>(args)..., paint));
        return;
    }
    if (brushValid && penValid && Drawing::Paint::CanCombinePaint(paintBrush_, paintPen_)) {
        paintPen_.SetStyle(Drawing::Paint::PaintStyle::PAINT_FILL_STROKE);
        cmdList_->AddDrawOp(std::make_shared<T>(std::forward<Args>(args)..., paintPen_));
        paintPen_.SetStyle(Drawing::Paint::PaintStyle::PAINT_STROKE);
        return;
    }
    if (brushValid) {
        cmdList_->AddDrawOp(std::make_shared<T>(std::forward<Args>(args)..., paintBrush_));
    }
    if (penValid) {
        cmdList_->AddDrawOp(std::make_shared<T>(std::forward<Args>(args)..., paintPen_));
    }
}

void ExtendRecordingCanvas::DrawImageNineWithPixelMap(const std::shared_ptr<Media::PixelMap>& pixelmap,
    const Drawing::RectI& center, const Drawing::Rect& dst, Drawing::FilterMode filter, const Drawing::Brush* brush)
{
    auto image = RSPixelMapUtil::ExtractDrawingImage(pixelmap);
    Drawing::RecordingCanvas::DrawImageNine(image.get(), center, dst, filter, brush);
}
} // namespace Rosen
} // namespace OHOS
#endif // USE_ROSEN_DRAWING
