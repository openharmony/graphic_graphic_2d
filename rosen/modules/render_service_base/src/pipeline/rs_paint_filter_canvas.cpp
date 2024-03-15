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

#include "pipeline/rs_paint_filter_canvas.h"

#include <algorithm>

#include "draw/canvas.h"

namespace OHOS {
namespace Rosen {

using namespace Drawing;

RSPaintFilterCanvasBase::RSPaintFilterCanvasBase(Drawing::Canvas* canvas)
    : Canvas(canvas->GetWidth(), canvas->GetHeight()), canvas_(canvas)
{
#ifdef ENABLE_RECORDING_DCL
    this->AddCanvas(canvas);
#endif
}

Drawing::Matrix RSPaintFilterCanvasBase::GetTotalMatrix() const
{
    return canvas_->GetTotalMatrix();
}

Drawing::Rect RSPaintFilterCanvasBase::GetLocalClipBounds() const
{
    return canvas_->GetLocalClipBounds();
}

Drawing::RectI RSPaintFilterCanvasBase::GetDeviceClipBounds() const
{
    return canvas_->GetDeviceClipBounds();
}

uint32_t RSPaintFilterCanvasBase::GetSaveCount() const
{
    return canvas_->GetSaveCount();
}

#ifdef ACE_ENABLE_GPU
std::shared_ptr<Drawing::GPUContext> RSPaintFilterCanvasBase::GetGPUContext()
{
    return canvas_ != nullptr ? canvas_->GetGPUContext() : nullptr;
}
#endif

void RSPaintFilterCanvasBase::DrawPoint(const Point& point)
{
#ifdef ENABLE_RECORDING_DCL
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr && OnFilter()) {
            (*iter)->DrawPoint(point);
        }
    }
#else
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawPoint(point);
    }
#endif
}

void RSPaintFilterCanvasBase::DrawSdf(const SDFShapeBase& shape)
{
#ifdef ENABLE_RECORDING_DCL
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr && OnFilter()) {
            (*iter)->DrawSdf(shape);
        }
    }
#else
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawSdf(shape);
    }
#endif
}

void RSPaintFilterCanvasBase::DrawPoints(PointMode mode, size_t count, const Point pts[])
{
#ifdef ENABLE_RECORDING_DCL
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr && OnFilter()) {
            (*iter)->DrawPoints(mode, count, pts);
        }
    }
#else
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawPoints(mode, count, pts);
    }
#endif
}

void RSPaintFilterCanvasBase::DrawLine(const Point& startPt, const Point& endPt)
{
#ifdef ENABLE_RECORDING_DCL
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr && OnFilter()) {
            (*iter)->DrawLine(startPt, endPt);
        }
    }
#else
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawLine(startPt, endPt);
    }
#endif
}

void RSPaintFilterCanvasBase::DrawRect(const Rect& rect)
{
#ifdef ENABLE_RECORDING_DCL
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr && OnFilter()) {
            (*iter)->DrawRect(rect);
        }
    }
#else
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawRect(rect);
    }
#endif
}

void RSPaintFilterCanvasBase::DrawRoundRect(const RoundRect& roundRect)
{
#ifdef ENABLE_RECORDING_DCL
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr && OnFilter()) {
            (*iter)->DrawRoundRect(roundRect);
        }
    }
#else
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawRoundRect(roundRect);
    }
#endif
}

void RSPaintFilterCanvasBase::DrawNestedRoundRect(const RoundRect& outer, const RoundRect& inner)
{
#ifdef ENABLE_RECORDING_DCL
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr && OnFilter()) {
            (*iter)->DrawNestedRoundRect(outer, inner);
        }
    }
#else
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawNestedRoundRect(outer, inner);
    }
#endif
}

void RSPaintFilterCanvasBase::DrawArc(const Rect& oval, scalar startAngle, scalar sweepAngle)
{
#ifdef ENABLE_RECORDING_DCL
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr && OnFilter()) {
            (*iter)->DrawArc(oval, startAngle, sweepAngle);
        }
    }
#else
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawArc(oval, startAngle, sweepAngle);
    }
#endif
}

void RSPaintFilterCanvasBase::DrawPie(const Rect& oval, scalar startAngle, scalar sweepAngle)
{
#ifdef ENABLE_RECORDING_DCL
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr && OnFilter()) {
            (*iter)->DrawPie(oval, startAngle, sweepAngle);
        }
    }
#else
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawPie(oval, startAngle, sweepAngle);
    }
#endif
}

void RSPaintFilterCanvasBase::DrawOval(const Rect& oval)
{
#ifdef ENABLE_RECORDING_DCL
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr && OnFilter()) {
            (*iter)->DrawOval(oval);
        }
    }
#else
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawOval(oval);
    }
#endif
}

void RSPaintFilterCanvasBase::DrawCircle(const Point& centerPt, scalar radius)
{
#ifdef ENABLE_RECORDING_DCL
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr && OnFilter()) {
            (*iter)->DrawCircle(centerPt, radius);
        }
    }
#else
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawCircle(centerPt, radius);
    }
#endif
}

void RSPaintFilterCanvasBase::DrawPath(const Path& path)
{
#ifdef ENABLE_RECORDING_DCL
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr && OnFilter()) {
            (*iter)->DrawPath(path);
        }
    }
#else
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawPath(path);
    }
#endif
}

void RSPaintFilterCanvasBase::DrawBackground(const Brush& brush)
{
    Brush b(brush);
#ifdef ENABLE_RECORDING_DCL
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr && OnFilterWithBrush(b)) {
            (*iter)->DrawBackground(b);
        }
    }
#else
    if (canvas_ != nullptr && OnFilterWithBrush(b)) {
        canvas_->DrawBackground(b);
    }
#endif
}

void RSPaintFilterCanvasBase::DrawShadow(const Path& path, const Point3& planeParams, const Point3& devLightPos,
    scalar lightRadius, Color ambientColor, Color spotColor, ShadowFlags flag)
{
#ifdef ENABLE_RECORDING_DCL
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr && OnFilter()) {
            (*iter)->DrawShadow(path, planeParams, devLightPos, lightRadius, ambientColor, spotColor, flag);
        }
    }
#else
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawShadow(path, planeParams, devLightPos, lightRadius, ambientColor, spotColor, flag);
    }
#endif
}

void RSPaintFilterCanvasBase::DrawColor(Drawing::ColorQuad color, Drawing::BlendMode mode)
{
#ifdef ENABLE_RECORDING_DCL
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr && OnFilter()) {
            (*iter)->DrawColor(color, mode);
        }
    }
#else
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawColor(color, mode);
    }
#endif
}

void RSPaintFilterCanvasBase::DrawRegion(const Drawing::Region& region)
{
#ifdef ENABLE_RECORDING_DCL
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr && OnFilter()) {
            (*iter)->DrawRegion(region);
        }
    }
#else
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawRegion(region);
    }
#endif
}

void RSPaintFilterCanvasBase::DrawPatch(const Drawing::Point cubics[12], const Drawing::ColorQuad colors[4],
    const Drawing::Point texCoords[4], Drawing::BlendMode mode)
{
#ifdef ENABLE_RECORDING_DCL
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr && OnFilter()) {
            (*iter)->DrawPatch(cubics, colors, texCoords, mode);
        }
    }
#else
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawPatch(cubics, colors, texCoords, mode);
    }
#endif
}

void RSPaintFilterCanvasBase::DrawVertices(const Drawing::Vertices& vertices, Drawing::BlendMode mode)
{
#ifdef ENABLE_RECORDING_DCL
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr && OnFilter()) {
            (*iter)->DrawVertices(vertices, mode);
        }
    }
#else
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawVertices(vertices, mode);
    }
#endif
}

// opinc_begin
bool RSPaintFilterCanvasBase::BeginOpRecording(const Drawing::Rect* bound, bool isDynamic)
{
    if (canvas_ != nullptr && OnFilter()) {
        return canvas_->BeginOpRecording(bound, isDynamic);
    }
    return false;
}

Drawing::OpListHandle RSPaintFilterCanvasBase::EndOpRecording()
{
    if (canvas_ != nullptr && OnFilter()) {
        return canvas_->EndOpRecording();
    }
    return {};
}

void RSPaintFilterCanvasBase::DrawOpList(Drawing::OpListHandle handle)
{
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawOpList(handle);
    }
}

int RSPaintFilterCanvasBase::CanDrawOpList(Drawing::OpListHandle handle)
{
    if (canvas_ != nullptr && OnFilter()) {
        return canvas_->CanDrawOpList(handle);
    }
    return -1;
}

void RSPaintFilterCanvasBase::PreOpListDrawArea(const Matrix& matrix)
{
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->PreOpListDrawArea(matrix);
    }
}

bool RSPaintFilterCanvasBase::CanUseOpListDrawArea(Drawing::OpListHandle handle, const Rect* bound)
{
    if (canvas_ != nullptr && OnFilter()) {
        return canvas_->CanUseOpListDrawArea(handle, bound);
    }
    return false;
}

Drawing::OpListHandle RSPaintFilterCanvasBase::GetOpListDrawArea()
{
    if (canvas_ != nullptr && OnFilter()) {
        return canvas_->GetOpListDrawArea();
    }
    return {};
}

void RSPaintFilterCanvasBase::OpincDrawImageRect(const Image& image, Drawing::OpListHandle drawAreas,
    const SamplingOptions& sampling, SrcRectConstraint constraint)
{
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->OpincDrawImageRect(image, drawAreas, sampling, constraint);
    }
}
// opinc_end

void RSPaintFilterCanvasBase::DrawBitmap(const Bitmap& bitmap, const scalar px, const scalar py)
{
#ifdef ENABLE_RECORDING_DCL
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr && OnFilter()) {
            (*iter)->DrawBitmap(bitmap, px, py);
        }
    }
#else
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawBitmap(bitmap, px, py);
    }
#endif
}

void RSPaintFilterCanvasBase::DrawImageNine(const Drawing::Image* image, const Drawing::RectI& center,
    const Drawing::Rect& dst, Drawing::FilterMode filter, const Drawing::Brush* brush)
{
#ifdef ENABLE_RECORDING_DCL
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr && OnFilter()) {
            (*iter)->DrawImageNine(image, center, dst, filter, brush);
        }
    }
#else
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawImageNine(image, center, dst, filter, brush);
    }
#endif
}

void RSPaintFilterCanvasBase::DrawImageLattice(const Drawing::Image* image, const Drawing::Lattice& lattice,
    const Drawing::Rect& dst, Drawing::FilterMode filter, const Drawing::Brush* brush)
{
#ifdef ENABLE_RECORDING_DCL
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr && OnFilter()) {
            (*iter)->DrawImageLattice(image, lattice, dst, filter, brush);
        }
    }
#else
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawImageLattice(image, lattice, dst, filter, brush);
    }
#endif
}

void RSPaintFilterCanvasBase::DrawImage(
    const Image& image, const scalar px, const scalar py, const SamplingOptions& sampling)
{
#ifdef ENABLE_RECORDING_DCL
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr && OnFilter()) {
            (*iter)->DrawImage(image, px, py, sampling);
        }
    }
#else
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawImage(image, px, py, sampling);
    }
#endif
}

void RSPaintFilterCanvasBase::DrawImageRect(const Image& image, const Rect& src, const Rect& dst,
    const SamplingOptions& sampling, SrcRectConstraint constraint)
{
#ifdef ENABLE_RECORDING_DCL
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr && OnFilter()) {
            (*iter)->DrawImageRect(image, src, dst, sampling, constraint);
        }
    }
#else
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawImageRect(image, src, dst, sampling, constraint);
    }
#endif
}

void RSPaintFilterCanvasBase::DrawImageRect(const Image& image, const Rect& dst, const SamplingOptions& sampling)
{
#ifdef ENABLE_RECORDING_DCL
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr && OnFilter()) {
            (*iter)->DrawImageRect(image, dst, sampling);
        }
    }
#else
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawImageRect(image, dst, sampling);
    }
#endif
}

void RSPaintFilterCanvasBase::DrawPicture(const Picture& picture)
{
#ifdef ENABLE_RECORDING_DCL
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr && OnFilter()) {
            (*iter)->DrawPicture(picture);
        }
    }
#else
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawPicture(picture);
    }
#endif
}

void RSPaintFilterCanvasBase::DrawTextBlob(
    const Drawing::TextBlob* blob, const Drawing::scalar x, const Drawing::scalar y)
{
#ifdef ENABLE_RECORDING_DCL
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr && OnFilter()) {
            (*iter)->DrawTextBlob(blob, x, y);
        }
    }
#else
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawTextBlob(blob, x, y);
    }
#endif
}

void RSPaintFilterCanvasBase::ClipRect(const Drawing::Rect& rect, Drawing::ClipOp op, bool doAntiAlias)
{
#ifdef ENABLE_RECORDING_DCL
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr) {
            (*iter)->ClipRect(rect, op, doAntiAlias);
        }
    }
#else
    if (canvas_ != nullptr) {
        canvas_->ClipRect(rect, op, doAntiAlias);
    }
#endif
}

void RSPaintFilterCanvasBase::ClipIRect(const Drawing::RectI& rect, Drawing::ClipOp op)
{
#ifdef ENABLE_RECORDING_DCL
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr) {
            (*iter)->ClipIRect(rect, op);
        }
    }
#else
    if (canvas_ != nullptr) {
        canvas_->ClipIRect(rect, op);
    }
#endif
}

void RSPaintFilterCanvasBase::ClipRoundRect(const RoundRect& roundRect, ClipOp op, bool doAntiAlias)
{
#ifdef ENABLE_RECORDING_DCL
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr) {
            (*iter)->ClipRoundRect(roundRect, op, doAntiAlias);
        }
    }
#else
    if (canvas_ != nullptr) {
        canvas_->ClipRoundRect(roundRect, op, doAntiAlias);
    }
#endif
}

void RSPaintFilterCanvasBase::ClipRoundRect(const Drawing::Rect& rect,
    std::vector<Drawing::Point>& pts, bool doAntiAlias)
{
#ifdef ENABLE_RECORDING_DCL
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr) {
            (*iter)->ClipRoundRect(rect, pts, doAntiAlias);
        }
    }
#else
    if (canvas_ != nullptr) {
        canvas_->ClipRoundRect(rect, pts, doAntiAlias);
    }
#endif
}

void RSPaintFilterCanvasBase::ClipPath(const Path& path, ClipOp op, bool doAntiAlias)
{
#ifdef ENABLE_RECORDING_DCL
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr) {
            (*iter)->ClipPath(path, op, doAntiAlias);
        }
    }
#else
    if (canvas_ != nullptr) {
        canvas_->ClipPath(path, op, doAntiAlias);
    }
#endif
}

void RSPaintFilterCanvasBase::ClipRegion(const Region& region, ClipOp op)
{
#ifdef ENABLE_RECORDING_DCL
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr) {
            (*iter)->ClipRegion(region, op);
        }
    }
#else
    if (canvas_ != nullptr) {
        canvas_->ClipRegion(region, op);
    }
#endif
}

void RSPaintFilterCanvasBase::SetMatrix(const Matrix& matrix)
{
#ifdef ENABLE_RECORDING_DCL
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr) {
            (*iter)->SetMatrix(matrix);
        }
    }
#else
    if (canvas_ != nullptr) {
        canvas_->SetMatrix(matrix);
    }
#endif
}

void RSPaintFilterCanvasBase::ResetMatrix()
{
#ifdef ENABLE_RECORDING_DCL
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr) {
            (*iter)->ResetMatrix();
        }
    }
#else
    if (canvas_ != nullptr) {
        canvas_->ResetMatrix();
    }
#endif
}

void RSPaintFilterCanvasBase::ConcatMatrix(const Matrix& matrix)
{
#ifdef ENABLE_RECORDING_DCL
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr) {
            (*iter)->ConcatMatrix(matrix);
        }
    }
#else
    if (canvas_ != nullptr) {
        canvas_->ConcatMatrix(matrix);
    }
#endif
}

void RSPaintFilterCanvasBase::Translate(scalar dx, scalar dy)
{
#ifdef ENABLE_RECORDING_DCL
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr) {
            (*iter)->Translate(dx, dy);
        }
    }
#else
    if (canvas_ != nullptr) {
        canvas_->Translate(dx, dy);
    }
#endif
}

void RSPaintFilterCanvasBase::Scale(scalar sx, scalar sy)
{
#ifdef ENABLE_RECORDING_DCL
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr) {
            (*iter)->Scale(sx, sy);
        }
    }
#else
    if (canvas_ != nullptr) {
        canvas_->Scale(sx, sy);
    }
#endif
}

void RSPaintFilterCanvasBase::Rotate(scalar deg, scalar sx, scalar sy)
{
#ifdef ENABLE_RECORDING_DCL
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr) {
            (*iter)->Rotate(deg, sx, sy);
        }
    }
#else
    if (canvas_ != nullptr) {
        canvas_->Rotate(deg, sx, sy);
    }
#endif
}

void RSPaintFilterCanvasBase::Shear(scalar sx, scalar sy)
{
#ifdef ENABLE_RECORDING_DCL
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr) {
            (*iter)->Shear(sx, sy);
        }
    }
#else
    if (canvas_ != nullptr) {
        canvas_->Shear(sx, sy);
    }
#endif
}

void RSPaintFilterCanvasBase::Flush()
{
#ifdef ENABLE_RECORDING_DCL
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr) {
            (*iter)->Flush();
        }
    }
#else
    if (canvas_ != nullptr) {
        canvas_->Flush();
    }
#endif
}

void RSPaintFilterCanvasBase::Clear(ColorQuad color)
{
#ifdef ENABLE_RECORDING_DCL
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr) {
            (*iter)->Clear(color);
        }
    }
#else
    if (canvas_ != nullptr) {
        canvas_->Clear(color);
    }
#endif
}

uint32_t RSPaintFilterCanvasBase::Save()
{
#ifdef ENABLE_RECORDING_DCL
    uint32_t count = 0U;
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr) {
            auto c = (*iter)->Save();
            if ((*iter) == canvas_) {
                count = c;
            }
        }
    }
    return count;
#else
    if (canvas_ != nullptr) {
        return canvas_->Save();
    }
    return 0;
#endif
}

void RSPaintFilterCanvasBase::SaveLayer(const SaveLayerOps& saveLayerRec)
{
    if (canvas_ == nullptr) {
        return;
    }
    Brush brush;
    if (saveLayerRec.GetBrush() != nullptr) {
        brush = *saveLayerRec.GetBrush();
        OnFilterWithBrush(brush);
    }
    SaveLayerOps slo(saveLayerRec.GetBounds(), &brush, saveLayerRec.GetSaveLayerFlags());
#ifdef ENABLE_RECORDING_DCL
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr) {
            (*iter)->SaveLayer(slo);
        }
    }
#else
    canvas_->SaveLayer(slo);
#endif
}

void RSPaintFilterCanvasBase::Restore()
{
#ifdef ENABLE_RECORDING_DCL
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr) {
            (*iter)->Restore();
        }
    }
#else
    if (canvas_ != nullptr) {
        canvas_->Restore();
    }
#endif
}

void RSPaintFilterCanvasBase::Discard()
{
#ifdef ENABLE_RECORDING_DCL
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr) {
            (*iter)->Discard();
        }
    }
#else
    if (canvas_ != nullptr) {
        canvas_->Discard();
    }
#endif
}

CoreCanvas& RSPaintFilterCanvasBase::AttachPen(const Pen& pen)
{
#ifdef ENABLE_RECORDING_DCL
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr) {
            (*iter)->AttachPen(pen);
        }
    }
#else
    if (canvas_ != nullptr) {
        canvas_->AttachPen(pen);
    }
#endif
    return *this;
}

CoreCanvas& RSPaintFilterCanvasBase::AttachBrush(const Brush& brush)
{
#ifdef ENABLE_RECORDING_DCL
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr) {
            (*iter)->AttachBrush(brush);
        }
    }
#else
    if (canvas_ != nullptr) {
        canvas_->AttachBrush(brush);
    }
#endif
    return *this;
}

CoreCanvas& RSPaintFilterCanvasBase::AttachPaint(const Drawing::Paint& paint)
{
#ifdef ENABLE_RECORDING_DCL
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr) {
            (*iter)->AttachPaint(paint);
        }
    }
#else
    if (canvas_ != nullptr) {
        canvas_->AttachPaint(paint);
    }
#endif
    return *this;
}

CoreCanvas& RSPaintFilterCanvasBase::DetachPen()
{
#ifdef ENABLE_RECORDING_DCL
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr) {
            (*iter)->DetachPen();
        }
    }
#else
    if (canvas_ != nullptr) {
        canvas_->DetachPen();
    }
#endif
    return *this;
}

CoreCanvas& RSPaintFilterCanvasBase::DetachBrush()
{
#ifdef ENABLE_RECORDING_DCL
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr) {
            (*iter)->DetachBrush();
        }
    }
#else
    if (canvas_ != nullptr) {
        canvas_->DetachBrush();
    }
#endif
    return *this;
}

CoreCanvas& RSPaintFilterCanvasBase::DetachPaint()
{
#ifdef ENABLE_RECORDING_DCL
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr) {
            (*iter)->DetachPaint();
        }
    }
#else
    if (canvas_ != nullptr) {
        canvas_->DetachPaint();
    }
#endif
    return *this;
}

RSPaintFilterCanvas::RSPaintFilterCanvas(Drawing::Canvas* canvas, float alpha)
    : RSPaintFilterCanvasBase(canvas), alphaStack_({ std::clamp(alpha, 0.f, 1.f) }), // construct stack with given alpha
      // Temporary fix, this default color should be 0x000000FF, fix this after foreground color refactor
      envStack_({ Env({ RSColor(0xFF000000) }) }), // construct stack with default foreground color
      blendModeStack_({std::nullopt})
{}

RSPaintFilterCanvas::RSPaintFilterCanvas(Drawing::Surface* surface, float alpha)
    : RSPaintFilterCanvasBase(surface ? surface->GetCanvas().get() : nullptr), surface_(surface),
      alphaStack_({ std::clamp(alpha, 0.f, 1.f) }), // construct stack with given alpha
      // Temporary fix, this default color should be 0x000000FF, fix this after foreground color refactor
      envStack_({ Env({ RSColor(0xFF000000) }) }), // construct stack with default foreground color
      blendModeStack_({std::nullopt})
{}

Drawing::Surface* RSPaintFilterCanvas::GetSurface() const
{
    return surface_;
}

CoreCanvas& RSPaintFilterCanvas::AttachPen(const Pen& pen)
{
    if (canvas_ == nullptr) {
        return *this;
    }

    Pen p(pen);
    if (p.GetColor() == 0x00000001) { // foreground color and foreground color strategy identification
        p.SetColor(envStack_.top().envForegroundColor_.AsArgbInt());
    }

    // use alphaStack_.top() to multiply alpha
    if (alphaStack_.top() < 1 && alphaStack_.top() > 0) {
        p.SetAlpha(p.GetAlpha() * alphaStack_.top());
    }

    // use blendModeStack_.top() to set blend mode
    if (auto& blendMode = blendModeStack_.top()) {
        p.SetBlendMode(static_cast<Drawing::BlendMode>(*blendMode));
    }

#ifdef ENABLE_RECORDING_DCL
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr) {
            (*iter)->AttachPen(p);
        }
    }
#else
    canvas_->AttachPen(p);
#endif
    return *this;
}

CoreCanvas& RSPaintFilterCanvas::AttachBrush(const Brush& brush)
{
    if (canvas_ == nullptr) {
        return *this;
    }

    Brush b(brush);
    if (b.GetColor() == 0x00000001) { // foreground color and foreground color strategy identification
        b.SetColor(envStack_.top().envForegroundColor_.AsArgbInt());
    }

    // use alphaStack_.top() to multiply alpha
    if (alphaStack_.top() < 1 && alphaStack_.top() > 0) {
        b.SetAlpha(b.GetAlpha() * alphaStack_.top());
    }

    // use blendModeStack_.top() to set blend mode
    if (auto& blendMode = blendModeStack_.top()) {
        b.SetBlendMode(static_cast<Drawing::BlendMode>(*blendMode));
    }
#ifdef ENABLE_RECORDING_DCL
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr) {
            (*iter)->AttachBrush(b);
        }
    }
#else
    canvas_->AttachBrush(b);
#endif
    return *this;
}

CoreCanvas& RSPaintFilterCanvas::AttachPaint(const Drawing::Paint& paint)
{
    if (canvas_ == nullptr) {
        return *this;
    }

    Paint p(paint);
    if (p.GetColor() == 0x00000001) { // foreground color and foreground color strategy identification
        p.SetColor(envStack_.top().envForegroundColor_.AsArgbInt());
    }

    // use alphaStack_.top() to multiply alpha
    if (alphaStack_.top() < 1 && alphaStack_.top() > 0) {
        p.SetAlpha(p.GetAlpha() * alphaStack_.top());
    }

    // use blendModeStack_.top() to set blend mode
    if (auto& blendMode = blendModeStack_.top()) {
        p.SetBlendMode(static_cast<Drawing::BlendMode>(*blendMode));
    }

#ifdef ENABLE_RECORDING_DCL
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr) {
            (*iter)->AttachPaint(p);
        }
    }
#else
    canvas_->AttachPaint(p);
#endif
    return *this;
}

bool RSPaintFilterCanvas::OnFilter() const
{
    return alphaStack_.top() > 0.f;
}

Drawing::Canvas* RSPaintFilterCanvas::GetRecordingCanvas() const
{
    return recordingState_ ? canvas_ : nullptr;
}


bool RSPaintFilterCanvas::GetRecordingState() const
{
    return recordingState_;
}

void RSPaintFilterCanvas::SetRecordingState(bool flag)
{
    recordingState_ = flag;
}

void RSPaintFilterCanvas::MultiplyAlpha(float alpha)
{
    // multiply alpha to top of stack
    alphaStack_.top() *= std::clamp(alpha, 0.f, 1.f);
}

void RSPaintFilterCanvas::SetAlpha(float alpha)
{
    alphaStack_.top() = std::clamp(alpha, 0.f, 1.f);
}

int RSPaintFilterCanvas::SaveAlpha()
{
    // make a copy of top of stack
    alphaStack_.push(alphaStack_.top());
    // return prev stack height
    return alphaStack_.size() - 1;
}

float RSPaintFilterCanvas::GetAlpha() const
{
    // return top of stack
    return alphaStack_.top();
}

void RSPaintFilterCanvas::RestoreAlpha()
{
    // sanity check, stack should not be empty
    if (alphaStack_.size() <= 1u) {
        return;
    }
    alphaStack_.pop();
}

int RSPaintFilterCanvas::GetAlphaSaveCount() const
{
    return alphaStack_.size();
}

void RSPaintFilterCanvas::RestoreAlphaToCount(int count)
{
    // sanity check, stack should not be empty
    if (count < 1) {
        count = 1;
    }
    // poo stack until stack height equals count
    int n = static_cast<int>(alphaStack_.size()) - count;
    for (int i = 0; i < n; ++i) {
        alphaStack_.pop();
    }
}

void RSPaintFilterCanvas::SetBlendMode(std::optional<int> blendMode)
{
    blendModeStack_.top() = blendMode;
}

int RSPaintFilterCanvas::SaveBlendMode()
{
    // make a copy of top of stack
    blendModeStack_.push(blendModeStack_.top());
    // return prev stack height
    return blendModeStack_.size() - 1;
}

void RSPaintFilterCanvas::RestoreBlendMode()
{
    if (blendModeStack_.empty()) {
        return;
    }
    blendModeStack_.pop();
}

std::optional<int> RSPaintFilterCanvas::GetBlendMode()
{
    return blendModeStack_.empty() ? std::nullopt : blendModeStack_.top();
}

void RSPaintFilterCanvas::AddBlendOffscreenLayer(bool isExtra)
{
    blendOffscreenStack_.push(isExtra);
}

void RSPaintFilterCanvas::MinusBlendOffscreenLayer()
{
    if (blendOffscreenStack_.empty()) {
        return;
    }
    blendOffscreenStack_.pop();
}

bool RSPaintFilterCanvas::IsBlendOffscreenExtraLayer() const
{
    return blendOffscreenStack_.empty() ? false : blendOffscreenStack_.top();
}

int RSPaintFilterCanvas::GetBlendOffscreenLayerCnt() const
{
    return blendOffscreenStack_.size();
}

int RSPaintFilterCanvas::SaveEnv()
{
    // make a copy of top of stack
    envStack_.push(envStack_.top());
    // return prev stack height
    return envStack_.size() - 1;
}

void RSPaintFilterCanvas::RestoreEnv()
{
    // sanity check, stack should not be empty
    if (envStack_.size() <= 1) {
        return;
    }
    envStack_.pop();
}

void RSPaintFilterCanvas::RestoreEnvToCount(int count)
{
    // sanity check, stack should not be empty
    if (count < 1) {
        count = 1;
    }
    // poo stack until stack height equals count
    int n = static_cast<int>(envStack_.size()) - count;
    for (int i = 0; i < n; ++i) {
        envStack_.pop();
    }
}

int RSPaintFilterCanvas::GetEnvSaveCount() const
{
    return envStack_.size();
}

void RSPaintFilterCanvas::SetEnvForegroundColor(Rosen::RSColor color)
{
    // sanity check, stack should not be empty
    if (envStack_.empty()) {
        return;
    }
    envStack_.top().envForegroundColor_ = color;
}

Drawing::ColorQuad RSPaintFilterCanvas::GetEnvForegroundColor() const
{
    // sanity check, stack should not be empty
    if (envStack_.empty()) {
        return Drawing::Color::COLOR_BLACK; // 0xFF000000 is default value -- black
    }
    return envStack_.top().envForegroundColor_.AsArgbInt();
}

RSPaintFilterCanvas::SaveStatus RSPaintFilterCanvas::SaveAllStatus(SaveType type)
{
    // save and return status on demand
    return { (RSPaintFilterCanvas::kCanvas & type) ? Save() : GetSaveCount(),
        (RSPaintFilterCanvas::kAlpha & type) ? SaveAlpha() : GetAlphaSaveCount(),
        (RSPaintFilterCanvas::kEnv & type) ? SaveEnv() : GetEnvSaveCount() };
}

RSPaintFilterCanvas::SaveStatus RSPaintFilterCanvas::GetSaveStatus() const
{
    return { GetSaveCount(), GetAlphaSaveCount(), GetEnvSaveCount() };
}

void RSPaintFilterCanvas::RestoreStatus(const SaveStatus& status)
{
    // simultaneously restore canvas and alpha
    RestoreToCount(status.canvasSaveCount);
    RestoreAlphaToCount(status.alphaSaveCount);
    RestoreEnvToCount(status.envSaveCount);
}

void RSPaintFilterCanvas::CopyConfiguration(const RSPaintFilterCanvas& other)
{
    // Note:
    // 1. we don't need to copy alpha status, alpha will be applied when drawing cache.
    // copy high contrast flag
    isHighContrastEnabled_.store(other.isHighContrastEnabled_.load());
    // copy env
    envStack_.top() = other.envStack_.top();
    // cache related
    if (other.isHighContrastEnabled()) {
        // explicit disable cache for high contrast mode
        SetCacheType(RSPaintFilterCanvas::CacheType::DISABLED);
    } else {
        // planning: maybe we should copy source cache status
        SetCacheType(other.GetCacheType());
    }
    isParallelCanvas_ = other.isParallelCanvas_;
    disableFilterCache_ = other.disableFilterCache_;
}

void RSPaintFilterCanvas::SetHighContrast(bool enabled)
{
    isHighContrastEnabled_ = enabled;
}
bool RSPaintFilterCanvas::isHighContrastEnabled() const
{
    return isHighContrastEnabled_;
}

void RSPaintFilterCanvas::SetCacheType(CacheType type)
{
    cacheType_ = type;
}
Drawing::CacheType RSPaintFilterCanvas::GetCacheType() const
{
    return cacheType_;
}

void RSPaintFilterCanvas::SetVisibleRect(Drawing::Rect visibleRect)
{
    visibleRect_ = visibleRect;
}

Drawing::Rect RSPaintFilterCanvas::GetVisibleRect() const
{
    return visibleRect_;
}

std::optional<Drawing::Rect> RSPaintFilterCanvas::GetLocalClipBounds(const Drawing::Canvas& canvas,
    const Drawing::RectI* clipRect)
{
    // if clipRect is explicitly specified, use it as the device clip bounds
    Drawing::Rect bounds = Rect((clipRect != nullptr) ? *clipRect : canvas.GetDeviceClipBounds());

    if (!bounds.IsValid()) {
        return std::nullopt;
    }

    Drawing::Matrix inverse;
    // if we can't invert the CTM, we can't return local clip bounds
    if (!(canvas.GetTotalMatrix().Invert(inverse))) {
        return std::nullopt;
    }
    // return the inverse of the CTM applied to the device clip bounds as local clip bounds
    Drawing::Rect dst;
    inverse.MapRect(dst, bounds);
    return dst;
}


void RSPaintFilterCanvas::SetEffectData(const std::shared_ptr<RSPaintFilterCanvas::CachedEffectData>& effectData)
{
    envStack_.top().effectData_ = effectData;
}

const std::shared_ptr<RSPaintFilterCanvas::CachedEffectData>& RSPaintFilterCanvas::GetEffectData() const
{
    return envStack_.top().effectData_;
}

void RSPaintFilterCanvas::SetCanvasStatus(const CanvasStatus& status)
{
    SetAlpha(status.alpha_);
    SetMatrix(status.matrix_);
    SetEffectData(status.effectData_);
}

RSPaintFilterCanvas::CanvasStatus RSPaintFilterCanvas::GetCanvasStatus() const
{
    return { GetAlpha(), GetTotalMatrix(), GetEffectData() };
}

RSPaintFilterCanvas::CachedEffectData::CachedEffectData(std::shared_ptr<Drawing::Image>&& image,
    const Drawing::RectI& rect)
    : cachedImage_(image), cachedRect_(rect)
{}

void RSPaintFilterCanvas::SetIsParallelCanvas(bool isParallel)
{
    isParallelCanvas_ = isParallel;
}

bool RSPaintFilterCanvas::GetIsParallelCanvas() const
{
    return isParallelCanvas_;
}

void RSPaintFilterCanvas::SetDisableFilterCache(bool disable)
{
    disableFilterCache_ = disable;
}

bool RSPaintFilterCanvas::GetDisableFilterCache() const
{
    return disableFilterCache_;
}

void RSPaintFilterCanvas::SetRecordDrawable(bool enable)
{
    recordDrawable_ = enable;
}

bool RSPaintFilterCanvas::GetRecordDrawable() const
{
    return recordDrawable_;
}
} // namespace Rosen
} // namespace OHOS
