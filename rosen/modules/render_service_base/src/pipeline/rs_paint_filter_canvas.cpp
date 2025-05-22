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

#include "platform/common/rs_log.h"
#include "utils/graphic_coretrace.h"

namespace OHOS {
namespace Rosen {

using namespace Drawing;

RSPaintFilterCanvasBase::RSPaintFilterCanvasBase(Drawing::Canvas* canvas)
    : Canvas(canvas ? canvas->GetWidth() : 0, canvas ? canvas->GetHeight() : 0), canvas_(canvas)
{
#ifdef SKP_RECORDING_ENABLED
    this->AddCanvas(canvas);
#endif
}

void RSPaintFilterCanvasBase::SetParallelRender(bool parallelEnable)
{
    canvas_->SetParallelRender(parallelEnable);
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

Drawing::RectI RSPaintFilterCanvasBase::GetRoundInDeviceClipBounds() const
{
    return canvas_->GetRoundInDeviceClipBounds();
}

CM_INLINE uint32_t RSPaintFilterCanvasBase::GetSaveCount() const
{
    return canvas_->GetSaveCount();
}

#ifdef RS_ENABLE_GPU
std::shared_ptr<Drawing::GPUContext> RSPaintFilterCanvasBase::GetGPUContext()
{
    return canvas_ != nullptr ? canvas_->GetGPUContext() : nullptr;
}
#endif

void RSPaintFilterCanvasBase::DrawPoint(const Point& point)
{
#ifdef SKP_RECORDING_ENABLED
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
#ifdef SKP_RECORDING_ENABLED
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
#ifdef SKP_RECORDING_ENABLED
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
#ifdef SKP_RECORDING_ENABLED
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
#ifdef SKP_RECORDING_ENABLED
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
#ifdef SKP_RECORDING_ENABLED
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
#ifdef SKP_RECORDING_ENABLED
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
#ifdef SKP_RECORDING_ENABLED
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
#ifdef SKP_RECORDING_ENABLED
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
#ifdef SKP_RECORDING_ENABLED
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
#ifdef SKP_RECORDING_ENABLED
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
    RECORD_GPURESOURCE_CORETRACE_CALLER(Drawing::CoreFunction::
        RS_RSPAINTFILTERCANVASBASE_DRAWPATH);
#ifdef SKP_RECORDING_ENABLED
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

void RSPaintFilterCanvasBase::DrawPathWithStencil(const Drawing::Path& path, uint32_t stencilVal)
{
#ifdef SKP_RECORDING_ENABLED
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr && OnFilter()) {
            (*iter)->DrawPathWithStencil(path, stencilVal);
        }
    }
#else
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawPathWithStencil(path, stencilVal);
    }
#endif
}

void RSPaintFilterCanvasBase::DrawBackground(const Brush& brush)
{
    Brush b(brush);
#ifdef SKP_RECORDING_ENABLED
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
#ifdef SKP_RECORDING_ENABLED
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

void RSPaintFilterCanvasBase::DrawShadowStyle(const Path& path, const Point3& planeParams, const Point3& devLightPos,
    scalar lightRadius, Color ambientColor, Color spotColor, ShadowFlags flag, bool isLimitElevation)
{
    RECORD_GPURESOURCE_CORETRACE_CALLER(Drawing::CoreFunction::
        RS_RSPAINTFILTERCANVASBASE_DRAWSHADOWSTYLE);
#ifdef SKP_RECORDING_ENABLED
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr && OnFilter()) {
            (*iter)->DrawShadowStyle(
                path, planeParams, devLightPos, lightRadius, ambientColor, spotColor, flag, isLimitElevation);
        }
    }
#else
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawShadowStyle(
            path, planeParams, devLightPos, lightRadius, ambientColor, spotColor, flag, isLimitElevation);
    }
#endif
}

void RSPaintFilterCanvasBase::DrawColor(Drawing::ColorQuad color, Drawing::BlendMode mode)
{
#ifdef SKP_RECORDING_ENABLED
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
#ifdef SKP_RECORDING_ENABLED
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
#ifdef SKP_RECORDING_ENABLED
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
#ifdef SKP_RECORDING_ENABLED
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

bool RSPaintFilterCanvasBase::OpCalculateBefore(const Matrix& matrix)
{
    if (canvas_ != nullptr && OnFilter()) {
        return canvas_->OpCalculateBefore(matrix);
    }
    return false;
}

std::shared_ptr<Drawing::OpListHandle> RSPaintFilterCanvasBase::OpCalculateAfter(const Drawing::Rect& bound)
{
    if (canvas_ != nullptr && OnFilter()) {
        return canvas_->OpCalculateAfter(bound);
    }
    return nullptr;
}

void RSPaintFilterCanvasBase::DrawAtlas(const Drawing::Image* atlas, const Drawing::RSXform xform[],
    const Drawing::Rect tex[], const Drawing::ColorQuad colors[], int count, Drawing::BlendMode mode,
    const Drawing::SamplingOptions& sampling, const Drawing::Rect* cullRect)
{
#ifdef SKP_RECORDING_ENABLED
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr && OnFilter()) {
            (*iter)->DrawAtlas(atlas, xform, tex, colors, count, mode, sampling, cullRect);
        }
    }
#else
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawAtlas(atlas, xform, tex, colors, count, mode, sampling, cullRect);
    }
#endif
}

void RSPaintFilterCanvasBase::DrawBitmap(const Bitmap& bitmap, const scalar px, const scalar py)
{
#ifdef SKP_RECORDING_ENABLED
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
    RECORD_GPURESOURCE_CORETRACE_CALLER(Drawing::CoreFunction::
        RS_RSPAINTFILTERCANVASBASE_DRAWIMAGENINE);
#ifdef SKP_RECORDING_ENABLED
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr && OnFilter()) {
            if (brush) {
                Drawing::Brush b(*brush);
                OnFilterWithBrush(b);
                (*iter)->DrawImageNine(image, center, dst, filter, &b);
            } else {
                (*iter)->DrawImageNine(image, center, dst, filter, GetFilteredBrush());
            }
        }
    }
#else
    if (canvas_ != nullptr && OnFilter()) {
        if (brush) {
            Drawing::Brush b(*brush);
            OnFilterWithBrush(b);
            canvas_->DrawImageNine(image, center, dst, filter, &b);
        } else {
            canvas_->DrawImageNine(image, center, dst, filter, GetFilteredBrush());
        }
    }
#endif
}

void RSPaintFilterCanvasBase::DrawImageLattice(const Drawing::Image* image, const Drawing::Lattice& lattice,
    const Drawing::Rect& dst, Drawing::FilterMode filter)
{
#ifdef SKP_RECORDING_ENABLED
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr && OnFilter()) {
            (*iter)->DrawImageLattice(image, lattice, dst, filter);
        }
    }
#else
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawImageLattice(image, lattice, dst, filter);
    }
#endif
}

void RSPaintFilterCanvasBase::DrawImage(
    const Image& image, const scalar px, const scalar py, const SamplingOptions& sampling)
{
#ifdef SKP_RECORDING_ENABLED
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

void RSPaintFilterCanvasBase::DrawImageWithStencil(const Drawing::Image& image, const Drawing::scalar px,
    const Drawing::scalar py, const Drawing::SamplingOptions& sampling, uint32_t stencilVal)
{
#ifdef SKP_RECORDING_ENABLED
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr && OnFilter()) {
            (*iter)->DrawImageWithStencil(image, px, py, sampling, stencilVal);
        }
    }
#else
    if (canvas_ != nullptr && OnFilter()) {
        canvas_->DrawImageWithStencil(image, px, py, sampling, stencilVal);
    }
#endif
}

void RSPaintFilterCanvasBase::DrawImageRect(const Image& image, const Rect& src, const Rect& dst,
    const SamplingOptions& sampling, SrcRectConstraint constraint)
{
    RECORD_GPURESOURCE_CORETRACE_CALLER(Drawing::CoreFunction::
        RS_RSPAINTFILTERCANVASBASE_DRAWIMAGERECT);
#ifdef SKP_RECORDING_ENABLED
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
#ifdef SKP_RECORDING_ENABLED
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
#ifdef SKP_RECORDING_ENABLED
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
#ifdef SKP_RECORDING_ENABLED
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

void RSPaintFilterCanvasBase::ClearStencil(const Drawing::RectI& rect, uint32_t stencilVal)
{
#ifdef SKP_RECORDING_ENABLED
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr) {
            (*iter)->ClearStencil(rect, stencilVal);
        }
    }
#else
    if (canvas_ != nullptr) {
        canvas_->ClearStencil(rect, stencilVal);
    }
#endif
}

void RSPaintFilterCanvasBase::ClipRect(const Drawing::Rect& rect, Drawing::ClipOp op, bool doAntiAlias)
{
#ifdef SKP_RECORDING_ENABLED
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
#ifdef SKP_RECORDING_ENABLED
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
#ifdef SKP_RECORDING_ENABLED
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
#ifdef SKP_RECORDING_ENABLED
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
#ifdef SKP_RECORDING_ENABLED
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
#ifdef SKP_RECORDING_ENABLED
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

void RSPaintFilterCanvasBase::ResetClip()
{
#ifdef SKP_RECORDING_ENABLED
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr) {
            (*iter)->ResetClip();
        }
    }
#else
    if (canvas_ != nullptr) {
        canvas_->ResetClip();
    }
#endif
}

void RSPaintFilterCanvasBase::SetMatrix(const Matrix& matrix)
{
#ifdef SKP_RECORDING_ENABLED
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
#ifdef SKP_RECORDING_ENABLED
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
#ifdef SKP_RECORDING_ENABLED
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
#ifdef SKP_RECORDING_ENABLED
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
#ifdef SKP_RECORDING_ENABLED
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
#ifdef SKP_RECORDING_ENABLED
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
#ifdef SKP_RECORDING_ENABLED
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
    RECORD_GPURESOURCE_CORETRACE_CALLER(Drawing::CoreFunction::
        RS_RSPAINTFILTERCANVASBASE_FLUSH);
#ifdef SKP_RECORDING_ENABLED
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
#ifdef SKP_RECORDING_ENABLED
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
#ifdef SKP_RECORDING_ENABLED
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
#ifdef SKP_RECORDING_ENABLED
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
#ifdef SKP_RECORDING_ENABLED
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
#ifdef SKP_RECORDING_ENABLED
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
#ifdef SKP_RECORDING_ENABLED
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
#ifdef SKP_RECORDING_ENABLED
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
#ifdef SKP_RECORDING_ENABLED
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
#ifdef SKP_RECORDING_ENABLED
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
#ifdef SKP_RECORDING_ENABLED
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
#ifdef SKP_RECORDING_ENABLED
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

bool RSPaintFilterCanvasBase::DrawBlurImage(const Drawing::Image& image, const Drawing::HpsBlurParameter& blurParams)
{
    bool result = false;
#ifdef SKP_RECORDING_ENABLED
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr) {
            result = ((*iter)->DrawBlurImage(image, blurParams) || result);
        }
    }
#else
    if (canvas_ != nullptr) {
        result = canvas_->DrawBlurImage(image, blurParams);
    }
#endif
    return result;
}

std::array<int, 2> RSPaintFilterCanvasBase::CalcHpsBluredImageDimension(const Drawing::HpsBlurParameter& blurParams)
{
    std::array<int, 2> result = {0, 0}; // There are 2 variables
#ifdef SKP_RECORDING_ENABLED
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr) {
            result = (*iter)->CalcHpsBluredImageDimension(blurParams);
        }
    }
#else
    if (canvas_ != nullptr) {
        result = canvas_->CalcHpsBluredImageDimension(blurParams);
    }
#endif
    return result;
}

bool RSPaintFilterCanvasBase::IsClipRect()
{
    bool result = false;
#ifdef SKP_RECORDING_ENABLED
    for (auto iter = pCanvasList_.begin(); iter != pCanvasList_.end(); ++iter) {
        if ((*iter) != nullptr) {
            result = result || (*iter)->IsClipRect();
        }
    }
#else
    if (canvas_ != nullptr) {
        result = canvas_->IsClipRect();
    }
#endif
    return result;
}

RSPaintFilterCanvas::RSPaintFilterCanvas(Drawing::Canvas* canvas, float alpha)
    : RSPaintFilterCanvasBase(canvas), alphaStack_({ 1.0f }),
      envStack_({ Env { .envForegroundColor_ = RSColor(0xFF000000), .hasOffscreenLayer_ = false } })
{
    (void)alpha; // alpha is no longer used, but we keep it for backward compatibility
}

RSPaintFilterCanvas::RSPaintFilterCanvas(Drawing::Surface* surface, float alpha)
    : RSPaintFilterCanvasBase(surface ? surface->GetCanvas().get() : nullptr), surface_(surface), alphaStack_({ 1.0f }),
      envStack_({ Env { .envForegroundColor_ = RSColor(0xFF000000), .hasOffscreenLayer_ = false } })
{
    (void)alpha; // alpha is no longer used, but we keep it for backward compatibility
}

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

    // use envStack_.top().blender_ to set blender
    if (auto& blender = envStack_.top().blender_) {
        if (p.GetBlenderEnabled()) {
            p.SetBlender(blender);
        }
    }

#ifdef SKP_RECORDING_ENABLED
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

    // use envStack_.top().blender_ to set blender
    if (auto& blender = envStack_.top().blender_) {
        if (b.GetBlenderEnabled()) {
            b.SetBlender(blender);
        }
    }

#ifdef SKP_RECORDING_ENABLED
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

    // use envStack_.top().blender_ to set blender
    if (auto& blender = envStack_.top().blender_) {
        if (p.GetBlenderEnabled()) {
            p.SetBlender(blender);
        }
    }

#ifdef SKP_RECORDING_ENABLED
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
    std::shared_ptr<Drawing::Blender> blender = nullptr;
    if (blendMode) {
        // map blendMode to Drawing::BlendMode and convert to Blender
        blender = Drawing::Blender::CreateWithBlendMode(static_cast<Drawing::BlendMode>(*blendMode));
    }
    RSPaintFilterCanvas::SetBlender(blender);
}

void RSPaintFilterCanvas::SetBlender(std::shared_ptr<Drawing::Blender> blender)
{
    envStack_.top().blender_ = blender;
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

void RSPaintFilterCanvas::PushDirtyRegion(Drawing::Region& resultRegion)
{
    dirtyRegionStack_.push(std::move(resultRegion));
}

void RSPaintFilterCanvas::PopDirtyRegion()
{
    if (dirtyRegionStack_.empty()) {
        RS_LOGW("PopDirtyRegion dirtyRegionStack_ is empty");
        return;
    }
    dirtyRegionStack_.pop();
}

Drawing::Region& RSPaintFilterCanvas::GetCurDirtyRegion()
{
    return dirtyRegionStack_.top();
}

bool RSPaintFilterCanvas::IsDirtyRegionStackEmpty()
{
    return dirtyRegionStack_.empty();
}

void RSPaintFilterCanvas::CopyHDRConfiguration(const RSPaintFilterCanvas& other)
{
    brightnessRatio_ = other.brightnessRatio_;
    screenId_ = other.screenId_;
    targetColorGamut_ = other.targetColorGamut_;
    isHdrOn_ = other.isHdrOn_;
    hdrBrightness_ = other.hdrBrightness_;
}

void RSPaintFilterCanvas::CopyConfigurationToOffscreenCanvas(const RSPaintFilterCanvas& other)
{
    // Note:
    // 1. we don't need to copy alpha status, alpha will be applied when drawing cache.
    // 2. This function should only be called when creating offscreen canvas.
    // copy high contrast flag
    isHighContrastEnabled_.store(other.isHighContrastEnabled_.load());
    // copy env
    envStack_.top() = other.envStack_.top();
    // update effect matrix
    auto effectData = other.envStack_.top().effectData_;
    if (effectData != nullptr) {
        // make a deep copy of effect data, and calculate the mapping matrix from
        // local coordinate system to global coordinate system.
        auto copiedEffectData = std::make_shared<CachedEffectData>(*effectData);
        if (copiedEffectData == nullptr) {
            ROSEN_LOGE("RSPaintFilterCanvas::CopyConfigurationToOffscreenCanvas fail to create effectData");
            return;
        }
        Drawing::Matrix inverse;
        if (other.GetTotalMatrix().Invert(inverse)) {
            copiedEffectData->cachedMatrix_.PostConcat(inverse);
        } else {
            ROSEN_LOGE("RSPaintFilterCanvas::CopyConfigurationToOffscreenCanvas get invert matrix failed!");
        }
        envStack_.top().effectData_ = copiedEffectData;
    }
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
    threadIndex_ = other.threadIndex_;
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

void RSPaintFilterCanvas::SetBehindWindowData(
    const std::shared_ptr<RSPaintFilterCanvas::CachedEffectData>& behindWindowData)
{
    envStack_.top().behindWindowData_ = behindWindowData;
}

const std::shared_ptr<RSPaintFilterCanvas::CachedEffectData>& RSPaintFilterCanvas::GetBehindWindowData() const
{
    return envStack_.top().behindWindowData_;
}

void RSPaintFilterCanvas::ReplaceMainScreenData(std::shared_ptr<Drawing::Surface>& offscreenSurface,
    std::shared_ptr<RSPaintFilterCanvas>& offscreenCanvas)
{
    if (offscreenSurface != nullptr && offscreenCanvas != nullptr) {
        storeMainScreenSurface_.push(surface_);
        storeMainScreenCanvas_.push(canvas_);
        surface_ = offscreenSurface.get();
        canvas_ = offscreenCanvas.get();
        OffscreenData offscreenData = {offscreenSurface, offscreenCanvas};
        offscreenDataList_.push(offscreenData);
    }
}

void RSPaintFilterCanvas::SwapBackMainScreenData()
{
    if (!storeMainScreenSurface_.empty() && !storeMainScreenCanvas_.empty() && !offscreenDataList_.empty()) {
        surface_ = storeMainScreenSurface_.top();
        canvas_ = storeMainScreenCanvas_.top();
        storeMainScreenSurface_.pop();
        storeMainScreenCanvas_.pop();
        offscreenDataList_.pop();
    }
}

void RSPaintFilterCanvas::SavePCanvasList()
{
    storedPCanvasList_.push_back(pCanvasList_);
}

void RSPaintFilterCanvas::RestorePCanvasList()
{
    if (!storedPCanvasList_.empty()) {
        auto item = storedPCanvasList_.back();
        pCanvasList_.swap(item);
        storedPCanvasList_.pop_back();
    }
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
    : cachedImage_(image), cachedRect_(rect), cachedMatrix_(Drawing::Matrix())
{}

void RSPaintFilterCanvas::SetIsParallelCanvas(bool isParallel)
{
    isParallelCanvas_ = isParallel;
}

bool RSPaintFilterCanvas::GetIsParallelCanvas() const
{
    return isParallelCanvas_;
}

// UNI_MAIN_THREAD_INDEX, UNI_RENDER_THREAD_INDEX, subthread 0 1 2.
void RSPaintFilterCanvas::SetParallelThreadIdx(uint32_t idx)
{
    threadIndex_ = idx;
}

uint32_t RSPaintFilterCanvas::GetParallelThreadIdx() const
{
    return threadIndex_;
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
bool RSPaintFilterCanvas::HasOffscreenLayer() const
{
    return envStack_.top().hasOffscreenLayer_;
}
void RSPaintFilterCanvas::SaveLayer(const Drawing::SaveLayerOps& saveLayerOps)
{
    envStack_.top().hasOffscreenLayer_ = true;
    RSPaintFilterCanvasBase::SaveLayer(saveLayerOps);
}

bool RSPaintFilterCanvas::IsOnMultipleScreen() const
{
    return multipleScreen_;
}
void RSPaintFilterCanvas::SetOnMultipleScreen(bool multipleScreen)
{
    multipleScreen_ = multipleScreen;
}

ScreenId RSPaintFilterCanvas::GetScreenId() const
{
    return screenId_;
}

void RSPaintFilterCanvas::SetScreenId(ScreenId screenId)
{
    screenId_ = screenId;
}

bool RSPaintFilterCanvas::GetHdrOn() const
{
    return isHdrOn_;
}

void RSPaintFilterCanvas::SetHdrOn(bool isHdrOn)
{
    isHdrOn_ = isHdrOn;
}

float RSPaintFilterCanvas::GetHDRBrightness() const
{
    return hdrBrightness_;
}

void RSPaintFilterCanvas::SetHDRBrightness(float hdrBrightness)
{
    hdrBrightness_ = hdrBrightness;
}

GraphicColorGamut RSPaintFilterCanvas::GetTargetColorGamut() const
{
    return targetColorGamut_;
}

void RSPaintFilterCanvas::SetTargetColorGamut(GraphicColorGamut colorGamut)
{
    targetColorGamut_ = colorGamut;
}

float RSPaintFilterCanvas::GetBrightnessRatio() const
{
    return brightnessRatio_;
}

void RSPaintFilterCanvas::SetBrightnessRatio(float brightnessRatio)
{
    brightnessRatio_ = brightnessRatio;
}

bool RSPaintFilterCanvas::GetIsWindowFreezeCapture() const
{
    return isWindowFreezeCapture_;
}

void RSPaintFilterCanvas::SetIsWindowFreezeCapture(bool isWindowFreezeCapture)
{
    isWindowFreezeCapture_ = isWindowFreezeCapture;
}

#ifdef RS_ENABLE_VK
CoreCanvas& RSHybridRenderPaintFilterCanvas::AttachPaint(const Drawing::Paint& paint)
{
    if (paint.GetColor() == Color::COLOR_FOREGROUND) {
        SetRenderWithForegroundColor(true);
    }
    return RSPaintFilterCanvas::AttachPaint(paint);
}
#endif
bool RSPaintFilterCanvas::GetIsDrawingCache() const
{
    return isDrawingCache_;
}

void RSPaintFilterCanvas::SetIsDrawingCache(bool isDrawingCache)
{
    isDrawingCache_ = isDrawingCache;
}

RSPaintFilterCanvas::CacheBehindWindowData::CacheBehindWindowData(
    std::shared_ptr<RSFilter> filter, const Drawing::Rect rect)
    : filter_(filter), rect_(rect)
{}

void RSPaintFilterCanvas::SetCacheBehindWindowData(const std::shared_ptr<CacheBehindWindowData>& data)
{
    cacheBehindWindowData_ = data;
}

const std::shared_ptr<RSPaintFilterCanvas::CacheBehindWindowData>& RSPaintFilterCanvas::GetCacheBehindWindowData() const
{
    return cacheBehindWindowData_;
}

void RSPaintFilterCanvas::SetEffectIntersectWithDRM(bool intersect)
{
    isIntersectWithDRM_ = intersect;
}

bool RSPaintFilterCanvas::GetEffectIntersectWithDRM() const
{
    return isIntersectWithDRM_;
}

void RSPaintFilterCanvas::SetDarkColorMode(bool isDark)
{
    isDarkColorMode_ = isDark;
}

bool RSPaintFilterCanvas::GetDarkColorMode() const
{
    return isDarkColorMode_;
}
} // namespace Rosen
} // namespace OHOS
