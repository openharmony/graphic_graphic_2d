/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.. All rights reserved.
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

#include "recording/recording_canvas.h"

#include "recording/cmd_list_helper.h"
#include "recording/draw_cmd.h"
#include "recording/draw_cmd_list.h"
#include "recording/recording_path.h"
#include "recording/recording_color_filter.h"
#include "recording/recording_color_space.h"
#include "recording/recording_image_filter.h"
#include "recording/recording_mask_filter.h"
#include "recording/recording_path_effect.h"
#include "recording/recording_shader_effect.h"
#include "recording/recording_region.h"
#include "draw/color.h"
#include "effect/color_filter.h"
#include "effect/color_space.h"
#include "effect/filter.h"
#include "effect/image_filter.h"
#include "effect/mask_filter.h"
#include "effect/path_effect.h"
#include "effect/shader_effect.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
void RecordingCanvas::GenerateHandleFromPaint(CmdList& cmdList, const Paint& paint, PaintHandle& paintHandle)
{
    paintHandle.isAntiAlias = paint.IsAntiAlias();
    paintHandle.style = paint.GetStyle();
    paintHandle.color = paint.GetColor();
    paintHandle.mode = paint.GetBlendMode();

    if (paint.HasFilter()) {
        const Filter& filter = paint.GetFilter();
        paintHandle.filterQuality = filter.GetFilterQuality();
        paintHandle.colorFilterHandle = CmdListHelper::AddColorFilterToCmdList(cmdList, filter.GetColorFilter());
        paintHandle.imageFilterHandle = CmdListHelper::AddImageFilterToCmdList(cmdList, filter.GetImageFilter());
        paintHandle.maskFilterHandle = CmdListHelper::AddMaskFilterToCmdList(cmdList, filter.GetMaskFilter());
    }

    if (paint.GetColorSpace()) {
        paintHandle.colorSpaceHandle = CmdListHelper::AddColorSpaceToCmdList(cmdList, paint.GetColorSpace());
    }

    if (paint.GetShaderEffect()) {
        paintHandle.shaderEffectHandle = CmdListHelper::AddShaderEffectToCmdList(cmdList, paint.GetShaderEffect());
    }

    if (!paint.HasStrokeStyle()) {
        return;
    }

    paintHandle.width = paint.GetWidth();
    paintHandle.miterLimit = paint.GetMiterLimit();
    paintHandle.capStyle = paint.GetCapStyle();
    paintHandle.joinStyle = paint.GetJoinStyle();
    if (paint.GetPathEffect()) {
        paintHandle.pathEffectHandle = CmdListHelper::AddPathEffectToCmdList(cmdList, paint.GetPathEffect());
    }
}

RecordingCanvas::RecordingCanvas(int width, int height) : Canvas(width, height)
{
    cmdList_ = std::make_shared<DrawCmdList>(width, height);
}

std::shared_ptr<DrawCmdList> RecordingCanvas::GetDrawCmdList() const
{
    return cmdList_;
}

void RecordingCanvas::Clear() const
{
    if (cmdList_ == nullptr) {
        return;
    }
    cmdList_->ClearOp();
}

void RecordingCanvas::DrawPoint(const Point& point)
{
    AddOp<DrawPointOpItem::ConstructorHandle>(point);
}

void RecordingCanvas::DrawPoints(PointMode mode, size_t count, const Point pts[])
{
    std::vector<Point> points(pts, pts + count);
    auto pointsData = CmdListHelper::AddVectorToCmdList<Point>(*cmdList_, points);
    AddOp<DrawPointsOpItem::ConstructorHandle>(mode, pointsData);
}

void RecordingCanvas::DrawLine(const Point& startPt, const Point& endPt)
{
    AddOp<DrawLineOpItem::ConstructorHandle>(startPt, endPt);
}

void RecordingCanvas::DrawRect(const Rect& rect)
{
    AddOp<DrawRectOpItem::ConstructorHandle>(rect);
}

void RecordingCanvas::DrawRoundRect(const RoundRect& roundRect)
{
    AddOp<DrawRoundRectOpItem::ConstructorHandle>(roundRect);
}

void RecordingCanvas::DrawNestedRoundRect(const RoundRect& outer, const RoundRect& inner)
{
    AddOp<DrawNestedRoundRectOpItem::ConstructorHandle>(outer, inner);
}

void RecordingCanvas::DrawArc(const Rect& oval, scalar startAngle, scalar sweepAngle)
{
    AddOp<DrawArcOpItem::ConstructorHandle>(oval, startAngle, sweepAngle);
}

void RecordingCanvas::DrawPie(const Rect& oval, scalar startAngle, scalar sweepAngle)
{
    AddOp<DrawPieOpItem::ConstructorHandle>(oval, startAngle, sweepAngle);
}

void RecordingCanvas::DrawOval(const Rect& oval)
{
    AddOp<DrawOvalOpItem::ConstructorHandle>(oval);
}

void RecordingCanvas::DrawCircle(const Point& centerPt, scalar radius)
{
    AddOp<DrawCircleOpItem::ConstructorHandle>(centerPt, radius);
}

void RecordingCanvas::DrawPath(const Path& path)
{
    auto pathHandle = CmdListHelper::AddPathToCmdList(*cmdList_, path);
    AddOp<DrawPathOpItem::ConstructorHandle>(pathHandle);
}

void RecordingCanvas::DrawBackground(const Brush& brush)
{
    Filter filter = brush.GetFilter();
    BrushHandle brushHandle = {
        brush.GetColor(),
        brush.GetBlendMode(),
        brush.IsAntiAlias(),
        filter.GetFilterQuality(),
        CmdListHelper::AddColorSpaceToCmdList(*cmdList_, brush.GetColorSpace()),
        CmdListHelper::AddShaderEffectToCmdList(*cmdList_, brush.GetShaderEffect()),
        CmdListHelper::AddColorFilterToCmdList(*cmdList_, filter.GetColorFilter()),
        CmdListHelper::AddImageFilterToCmdList(*cmdList_, filter.GetImageFilter()),
        CmdListHelper::AddMaskFilterToCmdList(*cmdList_, filter.GetMaskFilter()),
    };
    cmdList_->AddOp<DrawBackgroundOpItem::ConstructorHandle>(brushHandle);
}

void RecordingCanvas::DrawShadow(const Path& path, const Point3& planeParams, const Point3& devLightPos,
    scalar lightRadius, Color ambientColor, Color spotColor, ShadowFlags flag)
{
    auto pathHandle = CmdListHelper::AddPathToCmdList(*cmdList_, path);
    cmdList_->AddOp<DrawShadowOpItem::ConstructorHandle>(
        pathHandle, planeParams, devLightPos, lightRadius, ambientColor, spotColor, flag);
}

void RecordingCanvas::DrawRegion(const Region& region)
{
    auto regionHandle = CmdListHelper::AddRecordedToCmdList<RecordingRegion>(*cmdList_, region);
    AddOp<DrawRegionOpItem::ConstructorHandle>(regionHandle);
}

void RecordingCanvas::DrawPatch(const Point cubics[12], const ColorQuad colors[4],
    const Point texCoords[4], BlendMode mode)
{
    const size_t cubicsCount = 12;
    std::vector<Point> skiaCubics = {};
    if (cubics != nullptr) {
        skiaCubics = std::vector<Point>(cubics, cubics + cubicsCount);
    }

    const size_t colorsCount = 4;
    std::vector<ColorQuad> skiaColors = {};
    if (colors != nullptr) {
        skiaColors = std::vector<ColorQuad>(colors, colors + colorsCount);
    }

    const size_t texCoordsCount = 4;
    std::vector<Point> skiaTexCoords = {};
    if (texCoords != nullptr) {
        skiaTexCoords = std::vector<Point>(texCoords, texCoords + texCoordsCount);
    }

    auto cubicsData = CmdListHelper::AddVectorToCmdList<Point>(*cmdList_, skiaCubics);
    auto colorsData = CmdListHelper::AddVectorToCmdList<ColorQuad>(*cmdList_, skiaColors);
    auto texCoordsData = CmdListHelper::AddVectorToCmdList<Point>(*cmdList_, skiaTexCoords);
    AddOp<DrawPatchOpItem::ConstructorHandle>(cubicsData, colorsData, texCoordsData, mode);
}

void RecordingCanvas::DrawEdgeAAQuad(const Rect& rect, const Point clip[4],
    QuadAAFlags aaFlags, ColorQuad color, BlendMode mode)
{
    const size_t clipCount = 4;
    std::vector<Point> clipData = {};
    if (clip != nullptr) {
        clipData = std::vector<Point>(clip, clip + clipCount);
    }

    auto clipDataPtr = CmdListHelper::AddVectorToCmdList<Point>(*cmdList_, clipData);
    cmdList_->AddOp<DrawEdgeAAQuadOpItem::ConstructorHandle>(rect, clipDataPtr, aaFlags, color, mode);
}

void RecordingCanvas::DrawVertices(const Vertices& vertices, BlendMode mode)
{
    auto opDataHandle = CmdListHelper::AddVerticesToCmdList(*cmdList_, vertices);
    AddOp<DrawVerticesOpItem::ConstructorHandle>(opDataHandle, mode);
}

void RecordingCanvas::DrawImageNine(const Image* image, const RectI& center, const Rect& dst,
    FilterMode filterMode, const Brush* brush)
{
    auto imageHandle = CmdListHelper::AddImageToCmdList(*cmdList_, *image);
    BrushHandle brushHandle;
    bool hasBrush = false;
    if (brush != nullptr) {
        hasBrush = true;
        Filter filter = brush->GetFilter();
        brushHandle = {
            brush->GetColor(),
            brush->GetBlendMode(),
            brush->IsAntiAlias(),
            filter.GetFilterQuality(),
            CmdListHelper::AddColorSpaceToCmdList(*cmdList_, brush->GetColorSpace()),
            CmdListHelper::AddShaderEffectToCmdList(*cmdList_, brush->GetShaderEffect()),
            CmdListHelper::AddColorFilterToCmdList(*cmdList_, filter.GetColorFilter()),
            CmdListHelper::AddImageFilterToCmdList(*cmdList_, filter.GetImageFilter()),
            CmdListHelper::AddMaskFilterToCmdList(*cmdList_, filter.GetMaskFilter()),
        };
    }

    cmdList_->AddOp<DrawImageNineOpItem::ConstructorHandle>(
        imageHandle, center, dst, filterMode, brushHandle, hasBrush);
}

void RecordingCanvas::DrawAnnotation(const Rect& rect, const char* key, const Data* data)
{
    auto dataHandle = CmdListHelper::AddDataToCmdList(*cmdList_, data);
    cmdList_->AddOp<DrawAnnotationOpItem::ConstructorHandle>(rect, key, dataHandle);
}

void RecordingCanvas::DrawImageLattice(const Image* image, const Lattice& lattice, const Rect& dst,
    FilterMode filterMode, const Brush* brush)
{
    auto imageHandle = CmdListHelper::AddImageToCmdList(*cmdList_, *image);
    BrushHandle brushHandle;
    bool hasBrush = false;
    if (brush != nullptr) {
        hasBrush = true;
        Filter filter = brush->GetFilter();
        brushHandle = {
            brush->GetColor(),
            brush->GetBlendMode(),
            brush->IsAntiAlias(),
            filter.GetFilterQuality(),
            CmdListHelper::AddColorSpaceToCmdList(*cmdList_, brush->GetColorSpace()),
            CmdListHelper::AddShaderEffectToCmdList(*cmdList_, brush->GetShaderEffect()),
            CmdListHelper::AddColorFilterToCmdList(*cmdList_, filter.GetColorFilter()),
            CmdListHelper::AddImageFilterToCmdList(*cmdList_, filter.GetImageFilter()),
            CmdListHelper::AddMaskFilterToCmdList(*cmdList_, filter.GetMaskFilter()),
        };
    }

    cmdList_->AddOp<DrawImageLatticeOpItem::ConstructorHandle>(
        imageHandle, lattice, dst, filterMode, brushHandle, hasBrush);
}

void RecordingCanvas::DrawColor(ColorQuad color, BlendMode mode)
{
    cmdList_->AddOp<DrawColorOpItem::ConstructorHandle>(color, mode);
}

void RecordingCanvas::DrawBitmap(const Bitmap& bitmap, const scalar px, const scalar py)
{
    auto bitmapHandle = CmdListHelper::AddBitmapToCmdList(*cmdList_, bitmap);
    AddOp<DrawBitmapOpItem::ConstructorHandle>(bitmapHandle, px, py);
}

void RecordingCanvas::DrawImage(const Image& image, const scalar px, const scalar py, const SamplingOptions& sampling)
{
    auto imageHandle = CmdListHelper::AddImageToCmdList(*cmdList_, image);
    AddOp<DrawImageOpItem::ConstructorHandle>(imageHandle, px, py, sampling);
}

void RecordingCanvas::DrawImageRect(
    const Image& image, const Rect& src, const Rect& dst, const SamplingOptions& sampling, SrcRectConstraint constraint)
{
    auto imageHandle = CmdListHelper::AddImageToCmdList(*cmdList_, image);
    AddOp<DrawImageRectOpItem::ConstructorHandle>(imageHandle, src, dst, sampling, constraint);
}

void RecordingCanvas::DrawImageRect(const Image& image, const Rect& dst, const SamplingOptions& sampling)
{
    auto imageHandle = CmdListHelper::AddImageToCmdList(*cmdList_, image);
    Rect src(0, 0, image.GetWidth(), image.GetHeight());
    AddOp<DrawImageRectOpItem::ConstructorHandle>(
        imageHandle, src, dst, sampling, SrcRectConstraint::FAST_SRC_RECT_CONSTRAINT);
}

void RecordingCanvas::DrawPicture(const Picture& picture)
{
    auto pictureHandle = CmdListHelper::AddPictureToCmdList(*cmdList_, picture);
    cmdList_->AddOp<DrawPictureOpItem::ConstructorHandle>(pictureHandle);
}

void RecordingCanvas::DrawTextBlob(const TextBlob* blob, const scalar x, const scalar y)
{
    if (!blob) {
        LOGE("blob nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return;
    }

#ifdef ROSEN_OHOS
    if (IsCustomTextType()) {
        LOGD("RecordingCanvas::DrawTextBlob replace drawOpItem with cached one");
        GenerateCachedOpForTextblob(blob, x, y);
        return;
    }
#endif
    auto textBlobHandle = CmdListHelper::AddTextBlobToCmdList(*cmdList_, blob);
    AddOp<DrawTextBlobOpItem::ConstructorHandle>(textBlobHandle, x, y);
}

void RecordingCanvas::DrawSymbol(const DrawingHMSymbolData& symbol, Point locate)
{
    auto symbolHandle = CmdListHelper::AddSymbolToCmdList(*cmdList_, symbol);
    AddOp<DrawSymbolOpItem::ConstructorHandle>(symbolHandle, locate);
}

#ifdef ROSEN_OHOS
void RecordingCanvas::DrawSurfaceBuffer(const DrawingSurfaceBufferInfo& surfaceBufferInfo)
{
    AddOp<DrawSurfaceBufferOpItem::ConstructorHandle>(
        CmdListHelper::AddSurfaceBufferToCmdList(*cmdList_, surfaceBufferInfo.surfaceBuffer_),
        surfaceBufferInfo.offSetX_, surfaceBufferInfo.offSetY_,
        surfaceBufferInfo.width_, surfaceBufferInfo.height_);
}
#endif

void RecordingCanvas::ClipRect(const Rect& rect, ClipOp op, bool doAntiAlias)
{
    CheckForLazySave();
    cmdList_->AddOp<ClipRectOpItem::ConstructorHandle>(rect, op, doAntiAlias);
    Canvas::ClipRect(rect, op, doAntiAlias);
}

void RecordingCanvas::ClipIRect(const RectI& rect, ClipOp op)
{
    CheckForLazySave();
    cmdList_->AddOp<ClipIRectOpItem::ConstructorHandle>(rect, op);
    Canvas::ClipIRect(rect, op);
}

void RecordingCanvas::ClipRoundRect(const RoundRect& roundRect, ClipOp op, bool doAntiAlias)
{
    CheckForLazySave();
    cmdList_->AddOp<ClipRoundRectOpItem::ConstructorHandle>(roundRect, op, doAntiAlias);
    Canvas::ClipRoundRect(roundRect, op, doAntiAlias);
}

void RecordingCanvas::ClipRoundRect(const Rect& rect, std::vector<Point>& pts, bool doAntiAlias)
{
    CheckForLazySave();
    RoundRect roundRect = RoundRect(rect, pts);
    cmdList_->AddOp<ClipRoundRectOpItem::ConstructorHandle>(roundRect, ClipOp::INTERSECT, doAntiAlias);
    Canvas::ClipRoundRect(roundRect, ClipOp::INTERSECT, doAntiAlias);
}

void RecordingCanvas::ClipPath(const Path& path, ClipOp op, bool doAntiAlias)
{
    CheckForLazySave();
    auto pathHandle = CmdListHelper::AddPathToCmdList(*cmdList_, path);
    cmdList_->AddOp<ClipPathOpItem::ConstructorHandle>(pathHandle, op, doAntiAlias);
    Canvas::ClipPath(path, op, doAntiAlias);
}

void RecordingCanvas::ClipRegion(const Region& region, ClipOp op)
{
    CheckForLazySave();
    auto regionHandle = CmdListHelper::AddRecordedToCmdList<RecordingRegion>(*cmdList_, region);
    cmdList_->AddOp<ClipRegionOpItem::ConstructorHandle>(regionHandle, op);
    Canvas::ClipRegion(region, op);
}

void RecordingCanvas::SetMatrix(const Matrix& matrix)
{
    CheckForLazySave();
    Matrix::Buffer matrixBuffer;
    matrix.GetAll(matrixBuffer);
    cmdList_->AddOp<SetMatrixOpItem::ConstructorHandle>(matrixBuffer);
    Canvas::SetMatrix(matrix);
}

void RecordingCanvas::ResetMatrix()
{
    CheckForLazySave();
    cmdList_->AddOp<ResetMatrixOpItem::ConstructorHandle>();
    Canvas::ResetMatrix();
}

void RecordingCanvas::ConcatMatrix(const Matrix& matrix)
{
    if (!matrix.IsIdentity()) {
        CheckForLazySave();
        Matrix::Buffer matrixBuffer;
        matrix.GetAll(matrixBuffer);
        cmdList_->AddOp<ConcatMatrixOpItem::ConstructorHandle>(matrixBuffer);
        Canvas::ConcatMatrix(matrix);
    }
}

void RecordingCanvas::Translate(scalar dx, scalar dy)
{
    if (dx || dy) {
        CheckForLazySave();
        cmdList_->AddOp<TranslateOpItem::ConstructorHandle>(dx, dy);
        Canvas::Translate(dx, dy);
    }
}

void RecordingCanvas::Scale(scalar sx, scalar sy)
{
    if (sx != 1 || sy != 1) {
        CheckForLazySave();
        cmdList_->AddOp<ScaleOpItem::ConstructorHandle>(sx, sy);
        Canvas::Scale(sx, sy);
    }
}

void RecordingCanvas::Rotate(scalar deg, scalar sx, scalar sy)
{
    if (deg) {
        CheckForLazySave();
        cmdList_->AddOp<RotateOpItem::ConstructorHandle>(deg, sx, sy);
        Canvas::Rotate(deg, sx, sy);
    }
}

void RecordingCanvas::Shear(scalar sx, scalar sy)
{
    if (sx || sy) {
        CheckForLazySave();
        cmdList_->AddOp<ShearOpItem::ConstructorHandle>(sx, sy);
        Canvas::Shear(sx, sy);
    }
}

void RecordingCanvas::Flush()
{
    cmdList_->AddOp<FlushOpItem::ConstructorHandle>();
}

void RecordingCanvas::Clear(ColorQuad color)
{
    cmdList_->AddOp<ClearOpItem::ConstructorHandle>(color);
}

uint32_t RecordingCanvas::Save()
{
    uint32_t ret = static_cast<uint32_t>(saveOpStateStack_.size());
    saveOpStateStack_.push(LazySaveOp);
    return ret;
}

void RecordingCanvas::SaveLayer(const SaveLayerOps& saveLayerOps)
{
    Rect rect;
    bool hasBrush = false;
    BrushHandle brushHandle;

    if (saveLayerOps.GetBounds() != nullptr) {
        rect = *saveLayerOps.GetBounds();
    }
    const Brush* brush = saveLayerOps.GetBrush();
    if (brush != nullptr) {
        hasBrush = true;
        Filter filter = brush->GetFilter();
        brushHandle = {
            brush->GetColor(),
            brush->GetBlendMode(),
            brush->IsAntiAlias(),
            filter.GetFilterQuality(),
            CmdListHelper::AddColorSpaceToCmdList(*cmdList_, brush->GetColorSpace()),
            CmdListHelper::AddShaderEffectToCmdList(*cmdList_, brush->GetShaderEffect()),
            CmdListHelper::AddColorFilterToCmdList(*cmdList_, filter.GetColorFilter()),
            CmdListHelper::AddImageFilterToCmdList(*cmdList_, filter.GetImageFilter()),
            CmdListHelper::AddMaskFilterToCmdList(*cmdList_, filter.GetMaskFilter()),
        };
    }
    CmdListHandle imageFilterHandle = CmdListHelper::AddRecordedToCmdList<RecordingImageFilter>(
        *cmdList_, saveLayerOps.GetImageFilter());

    cmdList_->AddOp<SaveLayerOpItem::ConstructorHandle>(rect, hasBrush, brushHandle,
        imageFilterHandle, saveLayerOps.GetSaveLayerFlags());
    saveOpStateStack_.push(RealSaveOp);
    Canvas::SaveLayer(saveLayerOps);
}

void RecordingCanvas::Restore()
{
    if (saveOpStateStack_.empty()) {
        return;
    }

    SaveOpState state = saveOpStateStack_.top();
    saveOpStateStack_.pop();
    if (state == RealSaveOp) {
        cmdList_->AddOp<RestoreOpItem::ConstructorHandle>();
        Canvas::Restore();
    }
}

uint32_t RecordingCanvas::GetSaveCount() const
{
    return static_cast<uint32_t>(saveOpStateStack_.size());
}

void RecordingCanvas::Discard()
{
    cmdList_->AddOp<DiscardOpItem::ConstructorHandle>();
}

void RecordingCanvas::ClipAdaptiveRoundRect(const std::vector<Point>& radius)
{
    auto radiusData = CmdListHelper::AddVectorToCmdList<Point>(*cmdList_, radius);
    cmdList_->AddOp<ClipAdaptiveRoundRectOpItem::ConstructorHandle>(radiusData);
}

void RecordingCanvas::DrawImage(const std::shared_ptr<Image>& image, const std::shared_ptr<Data>& data,
    const AdaptiveImageInfo& rsImageInfo, const SamplingOptions& smapling)
{
    OpDataHandle imageHandle;
    if (data != nullptr) {
        imageHandle = CmdListHelper::AddCompressDataToCmdList(*cmdList_, data);
        AddOp<DrawAdaptiveImageOpItem::ConstructorHandle>(imageHandle, rsImageInfo, smapling, false);
        return;
    }
    if (image != nullptr) {
        imageHandle = CmdListHelper::AddImageToCmdList(*cmdList_, image);
        AddOp<DrawAdaptiveImageOpItem::ConstructorHandle>(imageHandle, rsImageInfo, smapling, true);
    }
}

void RecordingCanvas::DrawPixelMap(const std::shared_ptr<Media::PixelMap>& pixelMap,
    const AdaptiveImageInfo& rsImageInfo, const SamplingOptions& smapling)
{
    auto pixelmapHandle = CmdListHelper::AddPixelMapToCmdList(*cmdList_, pixelMap);
    AddOp<DrawAdaptivePixelMapOpItem::ConstructorHandle>(pixelmapHandle, rsImageInfo, smapling);
}

void RecordingCanvas::SetIsCustomTextType(bool isCustomTextType)
{
    isCustomTextType_ = isCustomTextType;
}

bool RecordingCanvas::IsCustomTextType() const
{
    return isCustomTextType_;
}

void RecordingCanvas::CheckForLazySave()
{
    if (!saveOpStateStack_.empty() && saveOpStateStack_.top() == LazySaveOp) {
        cmdList_->AddOp<SaveOpItem::ConstructorHandle>();
        saveOpStateStack_.top() = RealSaveOp;
        Canvas::Save();
    }
}

template<typename T, typename... Args>
void RecordingCanvas::AddOp(Args&&... args)
{
    PaintHandle paintHandle;
    bool brushValid = paintBrush_.IsValid();
    bool penValid = paintPen_.IsValid();
    if (!brushValid && !penValid) {
        paintHandle.isAntiAlias = true;
        paintHandle.style = Paint::PaintStyle::PAINT_FILL;
        cmdList_->AddOp<T>(std::forward<Args>(args)..., paintHandle);
        return;
    }
    if (brushValid && penValid && Paint::CanCombinePaint(paintBrush_, paintPen_)) {
        paintPen_.SetStyle(Paint::PaintStyle::PAINT_FILL_STROKE);
        GenerateHandleFromPaint(*cmdList_, paintPen_, paintHandle);
        cmdList_->AddOp<T>(std::forward<Args>(args)..., paintHandle);
        paintPen_.SetStyle(Paint::PaintStyle::PAINT_STROKE);
        return;
    }
    if (brushValid) {
        GenerateHandleFromPaint(*cmdList_, paintBrush_, paintHandle);
        cmdList_->AddOp<T>(std::forward<Args>(args)..., paintHandle);
    }
    if (penValid) {
        GenerateHandleFromPaint(*cmdList_, paintPen_, paintHandle);
        cmdList_->AddOp<T>(std::forward<Args>(args)..., paintHandle);
    }
}

void RecordingCanvas::GenerateCachedOpForTextblob(const TextBlob* blob, const scalar x, const scalar y)
{
    bool brushValid = paintBrush_.IsValid();
    bool penValid = paintPen_.IsValid();
    if (!brushValid && !penValid) {
        Paint paint;
        paint.SetAntiAlias(true);
        paint.SetStyle(Paint::PaintStyle::PAINT_FILL);
        DrawTextBlobOpItem::ConstructorHandle::GenerateCachedOpItem(*cmdList_, blob, x, y, paint);
        return;
    }
    if (brushValid && penValid && Paint::CanCombinePaint(paintBrush_, paintPen_)) {
        paintPen_.SetStyle(Paint::PaintStyle::PAINT_FILL_STROKE);
        DrawTextBlobOpItem::ConstructorHandle::GenerateCachedOpItem(*cmdList_, blob, x, y, paintPen_);
        paintPen_.SetStyle(Paint::PaintStyle::PAINT_STROKE);
        return;
    }
    if (brushValid) {
        DrawTextBlobOpItem::ConstructorHandle::GenerateCachedOpItem(*cmdList_, blob, x, y, paintBrush_);
    }
    if (penValid) {
        DrawTextBlobOpItem::ConstructorHandle::GenerateCachedOpItem(*cmdList_, blob, x, y, paintPen_);
    }
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
