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
RecordingCanvas::RecordingCanvas(int32_t width, int32_t height, bool addDrawOpImmediate)
    : NoDrawCanvas(width, height), addDrawOpImmediate_(addDrawOpImmediate)
{
    DrawCmdList::UnmarshalMode mode =
        addDrawOpImmediate ? DrawCmdList::UnmarshalMode::IMMEDIATE : DrawCmdList::UnmarshalMode::DEFERRED;
    cmdList_ = std::make_shared<DrawCmdList>(width, height, mode);
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

void RecordingCanvas::Reset(int32_t width, int32_t height, bool addDrawOpImmediate)
{
    DrawCmdList::UnmarshalMode mode =
        addDrawOpImmediate ? DrawCmdList::UnmarshalMode::IMMEDIATE : DrawCmdList::UnmarshalMode::DEFERRED;
    cmdList_ = std::make_shared<DrawCmdList>(width, height, mode);
    addDrawOpImmediate_ = addDrawOpImmediate;
    isCustomTextType_ = false;
    customTextBrush_ = std::nullopt;
    customTextPen_ = std::nullopt;
    saveOpStateStack_ = std::stack<SaveOpState>();
    gpuContext_ = nullptr;
    RemoveAll();
    DetachBrush();
    DetachPen();
    NoDrawCanvas::Reset(width, height);
}

void RecordingCanvas::DrawPoint(const Point& point)
{
    if (!addDrawOpImmediate_) {
        AddDrawOpDeferred<DrawPointOpItem>(point);
        return;
    }
    AddDrawOpImmediate<DrawPointOpItem::ConstructorHandle>(point);
}

void RecordingCanvas::DrawPoints(PointMode mode, size_t count, const Point pts[])
{
    std::vector<Point> points(pts, pts + count);
    if (!addDrawOpImmediate_) {
        AddDrawOpDeferred<DrawPointsOpItem>(mode, points);
        return;
    }
    auto pointsData = CmdListHelper::AddVectorToCmdList<Point>(*cmdList_, points);
    AddDrawOpImmediate<DrawPointsOpItem::ConstructorHandle>(mode, pointsData);
}

void RecordingCanvas::DrawLine(const Point& startPt, const Point& endPt)
{
    if (!addDrawOpImmediate_) {
        AddDrawOpDeferred<DrawLineOpItem>(startPt, endPt);
        return;
    }
    AddDrawOpImmediate<DrawLineOpItem::ConstructorHandle>(startPt, endPt);
}

void RecordingCanvas::DrawRect(const Rect& rect)
{
    if (!addDrawOpImmediate_) {
        AddDrawOpDeferred<DrawRectOpItem>(rect);
        return;
    }
    AddDrawOpImmediate<DrawRectOpItem::ConstructorHandle>(rect);
}

void RecordingCanvas::DrawRoundRect(const RoundRect& roundRect)
{
    if (!addDrawOpImmediate_) {
        AddDrawOpDeferred<DrawRoundRectOpItem>(roundRect);
        return;
    }
    AddDrawOpImmediate<DrawRoundRectOpItem::ConstructorHandle>(roundRect);
}

void RecordingCanvas::DrawNestedRoundRect(const RoundRect& outer, const RoundRect& inner)
{
    if (!addDrawOpImmediate_) {
        AddDrawOpDeferred<DrawNestedRoundRectOpItem>(outer, inner);
        return;
    }
    AddDrawOpImmediate<DrawNestedRoundRectOpItem::ConstructorHandle>(outer, inner);
}

void RecordingCanvas::DrawArc(const Rect& oval, scalar startAngle, scalar sweepAngle)
{
    if (!addDrawOpImmediate_) {
        AddDrawOpDeferred<DrawArcOpItem>(oval, startAngle, sweepAngle);
        return;
    }
    AddDrawOpImmediate<DrawArcOpItem::ConstructorHandle>(oval, startAngle, sweepAngle);
}

void RecordingCanvas::DrawPie(const Rect& oval, scalar startAngle, scalar sweepAngle)
{
    if (!addDrawOpImmediate_) {
        AddDrawOpDeferred<DrawPieOpItem>(oval, startAngle, sweepAngle);
        return;
    }
    AddDrawOpImmediate<DrawPieOpItem::ConstructorHandle>(oval, startAngle, sweepAngle);
}

void RecordingCanvas::DrawOval(const Rect& oval)
{
    if (!addDrawOpImmediate_) {
        AddDrawOpDeferred<DrawOvalOpItem>(oval);
        return;
    }
    AddDrawOpImmediate<DrawOvalOpItem::ConstructorHandle>(oval);
}

void RecordingCanvas::DrawCircle(const Point& centerPt, scalar radius)
{
    if (!addDrawOpImmediate_) {
        AddDrawOpDeferred<DrawCircleOpItem>(centerPt, radius);
        return;
    }
    AddDrawOpImmediate<DrawCircleOpItem::ConstructorHandle>(centerPt, radius);
}

void RecordingCanvas::DrawPath(const Path& path)
{
    if (!addDrawOpImmediate_) {
        AddDrawOpDeferred<DrawPathOpItem>(path);
        return;
    }
    auto pathHandle = CmdListHelper::AddPathToCmdList(*cmdList_, path);
    AddDrawOpImmediate<DrawPathOpItem::ConstructorHandle>(pathHandle);
}

void RecordingCanvas::DrawBackground(const Brush& brush)
{
    if (!addDrawOpImmediate_) {
        cmdList_->AddDrawOp(std::make_shared<DrawBackgroundOpItem>(brush));
        return;
    }
    BrushHandle brushHandle;
    DrawOpItem::BrushToBrushHandle(brush, *cmdList_, brushHandle);
    cmdList_->AddDrawOp<DrawBackgroundOpItem::ConstructorHandle>(brushHandle);
}

void RecordingCanvas::DrawShadow(const Path& path, const Point3& planeParams, const Point3& devLightPos,
    scalar lightRadius, Color ambientColor, Color spotColor, ShadowFlags flag)
{
    if (!addDrawOpImmediate_) {
        cmdList_->AddDrawOp(std::make_shared<DrawShadowOpItem>(
            path, planeParams, devLightPos, lightRadius, ambientColor, spotColor, flag));
        return;
    }
    auto pathHandle = CmdListHelper::AddPathToCmdList(*cmdList_, path);
    cmdList_->AddDrawOp<DrawShadowOpItem::ConstructorHandle>(
        pathHandle, planeParams, devLightPos, lightRadius, ambientColor, spotColor, flag);
}

void RecordingCanvas::DrawRegion(const Region& region)
{
    if (!addDrawOpImmediate_) {
        AddDrawOpDeferred<DrawRegionOpItem>(region);
        return;
    }
    auto regionHandle = CmdListHelper::AddRegionToCmdList(*cmdList_, region);
    AddDrawOpImmediate<DrawRegionOpItem::ConstructorHandle>(regionHandle);
}

void RecordingCanvas::DrawPatch(const Point cubics[12], const ColorQuad colors[4],
    const Point texCoords[4], BlendMode mode)
{
    LOGD("RecordingCanvas::DrawPatch not support yet");
}

void RecordingCanvas::DrawVertices(const Vertices& vertices, BlendMode mode)
{
    if (!addDrawOpImmediate_) {
        AddDrawOpDeferred<DrawVerticesOpItem>(vertices, mode);
        return;
    }
    auto opDataHandle = CmdListHelper::AddVerticesToCmdList(*cmdList_, vertices);
    AddDrawOpImmediate<DrawVerticesOpItem::ConstructorHandle>(opDataHandle, mode);
}

void RecordingCanvas::DrawImageNine(const Image* image, const RectI& center, const Rect& dst,
    FilterMode filterMode, const Brush* brush)
{
    if (!addDrawOpImmediate_) {
        cmdList_->AddDrawOp(std::make_shared<DrawImageNineOpItem>(image, center, dst, filterMode, brush));
        return;
    }

    auto imageHandle = CmdListHelper::AddImageToCmdList(*cmdList_, *image);
    BrushHandle brushHandle;
    bool hasBrush = false;
    if (brush != nullptr) {
        hasBrush = true;
        DrawOpItem::BrushToBrushHandle(*brush, *cmdList_, brushHandle);
    }

    cmdList_->AddDrawOp<DrawImageNineOpItem::ConstructorHandle>(
        imageHandle, center, dst, filterMode, brushHandle, hasBrush);
}

void RecordingCanvas::DrawImageLattice(const Image* image, const Lattice& lattice, const Rect& dst,
    FilterMode filterMode, const Brush* brush)
{
    if (!addDrawOpImmediate_) {
        cmdList_->AddDrawOp(std::make_shared<DrawImageLatticeOpItem>(image, lattice, dst, filterMode, brush));
        return;
    }

    auto imageHandle = CmdListHelper::AddImageToCmdList(*cmdList_, *image);
    BrushHandle brushHandle;
    bool hasBrush = false;
    if (brush != nullptr) {
        hasBrush = true;
        DrawOpItem::BrushToBrushHandle(*brush, *cmdList_, brushHandle);
    }

    cmdList_->AddDrawOp<DrawImageLatticeOpItem::ConstructorHandle>(
        imageHandle, lattice, dst, filterMode, brushHandle, hasBrush);
}

void RecordingCanvas::DrawColor(ColorQuad color, BlendMode mode)
{
    if (!addDrawOpImmediate_) {
        cmdList_->AddDrawOp(std::make_shared<DrawColorOpItem>(color, mode));
        return;
    }
    cmdList_->AddDrawOp<DrawColorOpItem::ConstructorHandle>(color, mode);
}

void RecordingCanvas::DrawBitmap(const Bitmap& bitmap, const scalar px, const scalar py)
{
    auto image = bitmap.MakeImage();
    if (image) {
        DrawImage(*image, px, py, SamplingOptions());
    }
}

void RecordingCanvas::DrawImage(const Image& image, const scalar px, const scalar py, const SamplingOptions& sampling)
{
    if (!addDrawOpImmediate_) {
        AddDrawOpDeferred<DrawImageOpItem>(image, px, py, sampling);
        return;
    }
    auto imageHandle = CmdListHelper::AddImageToCmdList(*cmdList_, image);
    AddDrawOpImmediate<DrawImageOpItem::ConstructorHandle>(imageHandle, px, py, sampling);
}

void RecordingCanvas::DrawImageRect(
    const Image& image, const Rect& src, const Rect& dst, const SamplingOptions& sampling, SrcRectConstraint constraint)
{
    if (!addDrawOpImmediate_) {
        AddDrawOpDeferred<DrawImageRectOpItem>(image, src, dst, sampling, constraint);
        return;
    }
    auto imageHandle = CmdListHelper::AddImageToCmdList(*cmdList_, image);
    AddDrawOpImmediate<DrawImageRectOpItem::ConstructorHandle>(imageHandle, src, dst, sampling, constraint);
}

void RecordingCanvas::DrawImageRect(const Image& image, const Rect& dst, const SamplingOptions& sampling)
{
    if (!addDrawOpImmediate_) {
        Rect src(0, 0, image.GetWidth(), image.GetHeight());
        AddDrawOpDeferred<DrawImageRectOpItem>(image, src, dst, sampling, SrcRectConstraint::FAST_SRC_RECT_CONSTRAINT);
        return;
    }
    auto imageHandle = CmdListHelper::AddImageToCmdList(*cmdList_, image);
    Rect src(0, 0, image.GetWidth(), image.GetHeight());
    AddDrawOpImmediate<DrawImageRectOpItem::ConstructorHandle>(
        imageHandle, src, dst, sampling, SrcRectConstraint::FAST_SRC_RECT_CONSTRAINT);
}

void RecordingCanvas::DrawPicture(const Picture& picture)
{
    if (!addDrawOpImmediate_) {
        cmdList_->AddDrawOp(std::make_shared<DrawPictureOpItem>(picture));
        return;
    }
    auto pictureHandle = CmdListHelper::AddPictureToCmdList(*cmdList_, picture);
    cmdList_->AddDrawOp<DrawPictureOpItem::ConstructorHandle>(pictureHandle);
}

void RecordingCanvas::DrawTextBlob(const TextBlob* blob, const scalar x, const scalar y)
{
    if (!blob) {
        return;
    }
#ifdef ROSEN_OHOS
    if (IsCustomTextType()) {
        LOGD("RecordingCanvas::DrawTextBlob replace drawOpItem with cached one");
        GenerateCachedOpForTextblob(blob, x, y);
        return;
    }
#endif
    if (!addDrawOpImmediate_) {
        AddDrawOpDeferred<DrawTextBlobOpItem>(blob, x, y);
        return;
    }
    auto textBlobHandle = CmdListHelper::AddTextBlobToCmdList(*cmdList_, blob);
    AddDrawOpImmediate<DrawTextBlobOpItem::ConstructorHandle>(textBlobHandle, x, y);
}

void RecordingCanvas::DrawSymbol(const DrawingHMSymbolData& symbol, Point locate)
{
    if (!addDrawOpImmediate_) {
        AddDrawOpDeferred<DrawSymbolOpItem>(symbol, locate);
        return;
    }
    auto symbolHandle = CmdListHelper::AddSymbolToCmdList(*cmdList_, symbol);
    AddDrawOpImmediate<DrawSymbolOpItem::ConstructorHandle>(symbolHandle, locate);
}

void RecordingCanvas::ClipRect(const Rect& rect, ClipOp op, bool doAntiAlias)
{
    CheckForLazySave();
    Canvas::ClipRect(rect, op, doAntiAlias);
    if (!addDrawOpImmediate_) {
        cmdList_->AddDrawOp(std::make_shared<ClipRectOpItem>(rect, op, doAntiAlias));
        return;
    }
    cmdList_->AddDrawOp<ClipRectOpItem::ConstructorHandle>(rect, op, doAntiAlias);
}

void RecordingCanvas::ClipIRect(const RectI& rect, ClipOp op)
{
    CheckForLazySave();
    Canvas::ClipIRect(rect, op);
    if (!addDrawOpImmediate_) {
        cmdList_->AddDrawOp(std::make_shared<ClipIRectOpItem>(rect, op));
        return;
    }
    cmdList_->AddDrawOp<ClipIRectOpItem::ConstructorHandle>(rect, op);
}

void RecordingCanvas::ClipRoundRect(const RoundRect& roundRect, ClipOp op, bool doAntiAlias)
{
    CheckForLazySave();
    Canvas::ClipRoundRect(roundRect, op, doAntiAlias);
    if (!addDrawOpImmediate_) {
        cmdList_->AddDrawOp(std::make_shared<ClipRoundRectOpItem>(roundRect, op, doAntiAlias));
        return;
    }
    cmdList_->AddDrawOp<ClipRoundRectOpItem::ConstructorHandle>(roundRect, op, doAntiAlias);
}

void RecordingCanvas::ClipRoundRect(const Rect& rect, std::vector<Point>& pts, bool doAntiAlias)
{
    CheckForLazySave();
    RoundRect roundRect = RoundRect(rect, pts);
    Canvas::ClipRoundRect(roundRect, ClipOp::INTERSECT, doAntiAlias);
    if (!addDrawOpImmediate_) {
        cmdList_->AddDrawOp(std::make_shared<ClipRoundRectOpItem>(roundRect, ClipOp::INTERSECT, doAntiAlias));
        return;
    }
    cmdList_->AddDrawOp<ClipRoundRectOpItem::ConstructorHandle>(roundRect, ClipOp::INTERSECT, doAntiAlias);
}

void RecordingCanvas::ClipPath(const Path& path, ClipOp op, bool doAntiAlias)
{
    CheckForLazySave();
    Canvas::ClipPath(path, op, doAntiAlias);
    if (!addDrawOpImmediate_) {
        cmdList_->AddDrawOp(std::make_shared<ClipPathOpItem>(path, op, doAntiAlias));
        return;
    }
    auto pathHandle = CmdListHelper::AddPathToCmdList(*cmdList_, path);
    cmdList_->AddDrawOp<ClipPathOpItem::ConstructorHandle>(pathHandle, op, doAntiAlias);
}

void RecordingCanvas::ClipRegion(const Region& region, ClipOp op)
{
    CheckForLazySave();
    Canvas::ClipRegion(region, op);
    if (!addDrawOpImmediate_) {
        cmdList_->AddDrawOp(std::make_shared<ClipRegionOpItem>(region, op));
        return;
    }
    auto regionHandle = CmdListHelper::AddRegionToCmdList(*cmdList_, region);
    cmdList_->AddDrawOp<ClipRegionOpItem::ConstructorHandle>(regionHandle, op);
}

void RecordingCanvas::SetMatrix(const Matrix& matrix)
{
    CheckForLazySave();
    Canvas::SetMatrix(matrix);
    if (!addDrawOpImmediate_) {
        cmdList_->AddDrawOp(std::make_shared<SetMatrixOpItem>(matrix));
        return;
    }
    Matrix::Buffer matrixBuffer;
    matrix.GetAll(matrixBuffer);
    cmdList_->AddDrawOp<SetMatrixOpItem::ConstructorHandle>(matrixBuffer);
}

void RecordingCanvas::ResetMatrix()
{
    CheckForLazySave();
    Canvas::ResetMatrix();
    if (!addDrawOpImmediate_) {
        cmdList_->AddDrawOp(std::make_shared<ResetMatrixOpItem>());
        return;
    }
    cmdList_->AddDrawOp<ResetMatrixOpItem::ConstructorHandle>();
}

void RecordingCanvas::ConcatMatrix(const Matrix& matrix)
{
    if (!matrix.IsIdentity()) {
        CheckForLazySave();
        Canvas::ConcatMatrix(matrix);
        if (!addDrawOpImmediate_) {
            cmdList_->AddDrawOp(std::make_shared<ConcatMatrixOpItem>(matrix));
            return;
        }
        Matrix::Buffer matrixBuffer;
        matrix.GetAll(matrixBuffer);
        cmdList_->AddDrawOp<ConcatMatrixOpItem::ConstructorHandle>(matrixBuffer);
    }
}

void RecordingCanvas::Translate(scalar dx, scalar dy)
{
    if (dx || dy) {
        CheckForLazySave();
        Canvas::Translate(dx, dy);
        if (!addDrawOpImmediate_) {
            cmdList_->AddDrawOp(std::make_shared<TranslateOpItem>(dx, dy));
            return;
        }
        cmdList_->AddDrawOp<TranslateOpItem::ConstructorHandle>(dx, dy);
    }
}

void RecordingCanvas::Scale(scalar sx, scalar sy)
{
    if (sx != 1 || sy != 1) {
        CheckForLazySave();
        Canvas::Scale(sx, sy);
        if (!addDrawOpImmediate_) {
            cmdList_->AddDrawOp(std::make_shared<ScaleOpItem>(sx, sy));
            return;
        }
        cmdList_->AddDrawOp<ScaleOpItem::ConstructorHandle>(sx, sy);
    }
}

void RecordingCanvas::Rotate(scalar deg, scalar sx, scalar sy)
{
    if (deg) {
        CheckForLazySave();
        Canvas::Rotate(deg, sx, sy);
        if (!addDrawOpImmediate_) {
            cmdList_->AddDrawOp(std::make_shared<RotateOpItem>(deg, sx, sy));
            return;
        }
        cmdList_->AddDrawOp<RotateOpItem::ConstructorHandle>(deg, sx, sy);
    }
}

void RecordingCanvas::Shear(scalar sx, scalar sy)
{
    if (sx || sy) {
        CheckForLazySave();
        Canvas::Shear(sx, sy);
        if (!addDrawOpImmediate_) {
            cmdList_->AddDrawOp(std::make_shared<ShearOpItem>(sx, sy));
            return;
        }
        cmdList_->AddDrawOp<ShearOpItem::ConstructorHandle>(sx, sy);
    }
}

void RecordingCanvas::Flush()
{
    if (!addDrawOpImmediate_) {
        cmdList_->AddDrawOp(std::make_shared<FlushOpItem>());
        return;
    }
    cmdList_->AddDrawOp<FlushOpItem::ConstructorHandle>();
}

void RecordingCanvas::Clear(ColorQuad color)
{
    if (!addDrawOpImmediate_) {
        cmdList_->AddDrawOp(std::make_shared<ClearOpItem>(color));
        return;
    }
    cmdList_->AddDrawOp<ClearOpItem::ConstructorHandle>(color);
}

uint32_t RecordingCanvas::Save()
{
    uint32_t ret = static_cast<uint32_t>(saveOpStateStack_.size());
    saveOpStateStack_.push(LazySaveOp);
    return ret;
}

void RecordingCanvas::SaveLayer(const SaveLayerOps& saveLayerOps)
{
    Canvas::SaveLayer(saveLayerOps);
    saveOpStateStack_.push(RealSaveOp);
    if (!addDrawOpImmediate_) {
        cmdList_->AddDrawOp(std::make_shared<SaveLayerOpItem>(saveLayerOps));
        return;
    }

    Rect rect;
    bool hasBrush = false;
    BrushHandle brushHandle;

    if (saveLayerOps.GetBounds() != nullptr) {
        rect = *saveLayerOps.GetBounds();
    }
    const Brush* brush = saveLayerOps.GetBrush();
    if (brush != nullptr) {
        hasBrush = true;
        DrawOpItem::BrushToBrushHandle(*brush, *cmdList_, brushHandle);
    }

    cmdList_->AddDrawOp<SaveLayerOpItem::ConstructorHandle>(rect, hasBrush, brushHandle,
        saveLayerOps.GetSaveLayerFlags());
}

void RecordingCanvas::Restore()
{
    if (saveOpStateStack_.empty()) {
        return;
    }

    SaveOpState state = saveOpStateStack_.top();
    saveOpStateStack_.pop();
    if (state == RealSaveOp) {
        Canvas::Restore();
        if (!addDrawOpImmediate_) {
            cmdList_->AddDrawOp(std::make_shared<RestoreOpItem>());
            return;
        }
        cmdList_->AddDrawOp<RestoreOpItem::ConstructorHandle>();
    }
}

uint32_t RecordingCanvas::GetSaveCount() const
{
    return static_cast<uint32_t>(saveOpStateStack_.size());
}

void RecordingCanvas::Discard()
{
    if (!addDrawOpImmediate_) {
        cmdList_->AddDrawOp(std::make_shared<DiscardOpItem>());
        return;
    }
    cmdList_->AddDrawOp<DiscardOpItem::ConstructorHandle>();
}

void RecordingCanvas::ClipAdaptiveRoundRect(const std::vector<Point>& radius)
{
    if (!addDrawOpImmediate_) {
        cmdList_->AddDrawOp(std::make_shared<ClipAdaptiveRoundRectOpItem>(radius));
        return;
    }
    auto radiusData = CmdListHelper::AddVectorToCmdList<Point>(*cmdList_, radius);
    cmdList_->AddDrawOp<ClipAdaptiveRoundRectOpItem::ConstructorHandle>(radiusData);
}

void RecordingCanvas::DrawImage(const std::shared_ptr<Image>& image, const std::shared_ptr<Data>& data,
    const AdaptiveImageInfo& rsImageInfo, const SamplingOptions& sampling)
{
    if (!addDrawOpImmediate_) {
        AddDrawOpDeferred<DrawAdaptiveImageOpItem>(image, data, rsImageInfo, sampling);
        return;
    }
    OpDataHandle imageHandle;
    if (data != nullptr) {
        imageHandle = CmdListHelper::AddCompressDataToCmdList(*cmdList_, data);
        AddDrawOpImmediate<DrawAdaptiveImageOpItem::ConstructorHandle>(imageHandle, rsImageInfo, sampling, false);
        return;
    }
    if (image != nullptr) {
        imageHandle = CmdListHelper::AddImageToCmdList(*cmdList_, image);
        AddDrawOpImmediate<DrawAdaptiveImageOpItem::ConstructorHandle>(imageHandle, rsImageInfo, sampling, true);
    }
}

void RecordingCanvas::DrawPixelMap(const std::shared_ptr<Media::PixelMap>& pixelMap,
    const AdaptiveImageInfo& rsImageInfo, const SamplingOptions& sampling)
{
    if (!addDrawOpImmediate_) {
        AddDrawOpDeferred<DrawAdaptivePixelMapOpItem>(pixelMap, rsImageInfo, sampling);
        return;
    }
    auto pixelmapHandle = CmdListHelper::AddPixelMapToCmdList(*cmdList_, pixelMap);
    AddDrawOpImmediate<DrawAdaptivePixelMapOpItem::ConstructorHandle>(pixelmapHandle, rsImageInfo, sampling);
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
        Canvas::Save();
        if (!addDrawOpImmediate_) {
            cmdList_->AddDrawOp(std::make_shared<SaveOpItem>());
        } else {
            cmdList_->AddDrawOp<SaveOpItem::ConstructorHandle>();
        }
        saveOpStateStack_.top() = RealSaveOp;
    }
}

template<typename T, typename... Args>
void RecordingCanvas::AddDrawOpImmediate(Args&&... args)
{
    bool brushValid = paintBrush_.IsValid();
    bool penValid = paintPen_.IsValid();
    if (!brushValid && !penValid) {
        PaintHandle paintHandle;
        paintHandle.isAntiAlias = true;
        paintHandle.style = Paint::PaintStyle::PAINT_FILL;
        cmdList_->AddDrawOp<T>(std::forward<Args>(args)..., paintHandle);
        return;
    }
    if (brushValid && penValid && Paint::CanCombinePaint(paintBrush_, paintPen_)) {
        PaintHandle paintHandle;
        paintPen_.SetStyle(Paint::PaintStyle::PAINT_FILL_STROKE);
        DrawOpItem::GenerateHandleFromPaint(*cmdList_, paintPen_, paintHandle);
        cmdList_->AddDrawOp<T>(std::forward<Args>(args)..., paintHandle);
        paintPen_.SetStyle(Paint::PaintStyle::PAINT_STROKE);
        return;
    }
    if (brushValid) {
        PaintHandle paintHandle;
        DrawOpItem::GenerateHandleFromPaint(*cmdList_, paintBrush_, paintHandle);
        cmdList_->AddDrawOp<T>(std::forward<Args>(args)..., paintHandle);
    }
    if (penValid) {
        PaintHandle paintHandle;
        DrawOpItem::GenerateHandleFromPaint(*cmdList_, paintPen_, paintHandle);
        cmdList_->AddDrawOp<T>(std::forward<Args>(args)..., paintHandle);
    }
}

template<typename T, typename... Args>
void RecordingCanvas::AddDrawOpDeferred(Args&&... args)
{
    bool brushValid = paintBrush_.IsValid();
    bool penValid = paintPen_.IsValid();
    if (!brushValid && !penValid) {
        Paint paint;
        paint.SetAntiAlias(true);
        paint.SetStyle(Paint::PaintStyle::PAINT_FILL);
        cmdList_->AddDrawOp(std::make_shared<T>(std::forward<Args>(args)..., paint));
        return;
    }
    if (brushValid && penValid && Paint::CanCombinePaint(paintBrush_, paintPen_)) {
        paintPen_.SetStyle(Paint::PaintStyle::PAINT_FILL_STROKE);
        cmdList_->AddDrawOp(std::make_shared<T>(std::forward<Args>(args)..., paintPen_));
        paintPen_.SetStyle(Paint::PaintStyle::PAINT_STROKE);
        return;
    }
    if (brushValid) {
        cmdList_->AddDrawOp(std::make_shared<T>(std::forward<Args>(args)..., paintBrush_));
    }
    if (penValid) {
        cmdList_->AddDrawOp(std::make_shared<T>(std::forward<Args>(args)..., paintPen_));
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
        GenerateCachedOpForTextblob(blob, x, y, paint);
        return;
    }
    if (brushValid && penValid && Paint::CanCombinePaint(paintBrush_, paintPen_)) {
        paintPen_.SetStyle(Paint::PaintStyle::PAINT_FILL_STROKE);
        GenerateCachedOpForTextblob(blob, x, y, paintPen_);
        paintPen_.SetStyle(Paint::PaintStyle::PAINT_STROKE);
        return;
    }
    if (brushValid) {
        GenerateCachedOpForTextblob(blob, x, y, paintBrush_);
    }
    if (penValid) {
        GenerateCachedOpForTextblob(blob, x, y, paintPen_);
    }
}

void RecordingCanvas::GenerateCachedOpForTextblob(const TextBlob* blob, const scalar x, const scalar y, Paint& paint)
{
    if (!addDrawOpImmediate_) {
        std::shared_ptr<DrawTextBlobOpItem> op = std::make_shared<DrawTextBlobOpItem>(blob, x, y, paint);
        cmdList_->AddDrawOp(op->GenerateCachedOpItem(nullptr));
    } else {
        DrawTextBlobOpItem::ConstructorHandle::GenerateCachedOpItem(*cmdList_, blob, x, y, paint);
    }
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
