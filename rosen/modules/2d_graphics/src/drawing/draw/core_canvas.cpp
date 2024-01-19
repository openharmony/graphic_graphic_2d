/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.. All rights reserved.
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

#include "core_canvas.h"

#include "impl_factory.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
CoreCanvas::CoreCanvas() : impl_(ImplFactory::CreateCoreCanvasImpl()) {}

CoreCanvas::CoreCanvas(void* rawCanvas) : impl_(ImplFactory::CreateCoreCanvasImpl(rawCanvas)) {}

CoreCanvas::CoreCanvas(int32_t width, int32_t height) : impl_(ImplFactory::CreateCoreCanvasImpl(width, height)) {}

void CoreCanvas::Bind(const Bitmap& bitmap)
{
    impl_->Bind(bitmap);
}

Matrix CoreCanvas::GetTotalMatrix() const
{
    return impl_->GetTotalMatrix();
}

Rect CoreCanvas::GetLocalClipBounds() const
{
    return impl_->GetLocalClipBounds();
}

RectI CoreCanvas::GetDeviceClipBounds() const
{
    return impl_->GetDeviceClipBounds();
}

#ifdef ACE_ENABLE_GPU
std::shared_ptr<GPUContext> CoreCanvas::GetGPUContext()
{
    if (!gpuContext_) {
        gpuContext_ = impl_->GetGPUContext();
    }
    return gpuContext_;
}
#endif

int32_t CoreCanvas::GetWidth() const
{
    return impl_->GetWidth();
}

int32_t CoreCanvas::GetHeight() const
{
    return impl_->GetHeight();
}

ImageInfo CoreCanvas::GetImageInfo()
{
    return impl_->GetImageInfo();
}

bool CoreCanvas::ReadPixels(const ImageInfo& dstInfo, void* dstPixels, size_t dstRowBytes,
    int srcX, int srcY)
{
    return impl_->ReadPixels(dstInfo, dstPixels, dstRowBytes, srcX, srcY);
}

bool CoreCanvas::ReadPixels(const Bitmap& dstBitmap, int srcX, int srcY)
{
    return impl_->ReadPixels(dstBitmap, srcX, srcY);
}

void CoreCanvas::DrawPoint(const Point& point)
{
    AttachPaint();
    impl_->DrawPoint(point);
}

void CoreCanvas::DrawPoints(PointMode mode, size_t count, const Point pts[])
{
    AttachPaint();
    impl_->DrawPoints(mode, count, pts);
}

void CoreCanvas::DrawLine(const Point& startPt, const Point& endPt)
{
    AttachPaint();
    impl_->DrawLine(startPt, endPt);
}

void CoreCanvas::DrawRect(const Rect& rect)
{
    AttachPaint();
    impl_->DrawRect(rect);
}

void CoreCanvas::DrawRoundRect(const RoundRect& roundRect)
{
    AttachPaint();
    impl_->DrawRoundRect(roundRect);
}

void CoreCanvas::DrawNestedRoundRect(const RoundRect& outer, const RoundRect& inner)
{
    AttachPaint();
    impl_->DrawNestedRoundRect(outer, inner);
}

void CoreCanvas::DrawArc(const Rect& oval, scalar startAngle, scalar sweepAngle)
{
    AttachPaint();
    impl_->DrawArc(oval, startAngle, sweepAngle);
}

void CoreCanvas::DrawPie(const Rect& oval, scalar startAngle, scalar sweepAngle)
{
    AttachPaint();
    impl_->DrawPie(oval, startAngle, sweepAngle);
}

void CoreCanvas::DrawOval(const Rect& oval)
{
    AttachPaint();
    impl_->DrawOval(oval);
}

void CoreCanvas::DrawCircle(const Point& centerPt, scalar radius)
{
    AttachPaint();
    impl_->DrawCircle(centerPt, radius);
}

void CoreCanvas::DrawPath(const Path& path)
{
    AttachPaint();
    impl_->DrawPath(path);
}

void CoreCanvas::DrawBackground(const Brush& brush)
{
    impl_->DrawBackground(brush);
}

void CoreCanvas::DrawShadow(const Path& path, const Point3& planeParams, const Point3& devLightPos, scalar lightRadius,
    Color ambientColor, Color spotColor, ShadowFlags flag)
{
    impl_->DrawShadow(path, planeParams, devLightPos, lightRadius, ambientColor, spotColor, flag);
}

void CoreCanvas::DrawColor(ColorQuad color, BlendMode mode)
{
    impl_->DrawColor(color, mode);
}

void CoreCanvas::DrawRegion(const Region& region)
{
    AttachPaint();
    impl_->DrawRegion(region);
}

void CoreCanvas::DrawPatch(const Point cubics[12], const ColorQuad colors[4], const Point texCoords[4], BlendMode mode)
{
    AttachPaint();
    impl_->DrawPatch(cubics, colors, texCoords, mode);
}

void CoreCanvas::DrawEdgeAAQuad(const Rect& rect, const Point clip[4],
    QuadAAFlags aaFlags, ColorQuad color, BlendMode mode)
{
    impl_->DrawEdgeAAQuad(rect, clip, aaFlags, color, mode);
}

void CoreCanvas::DrawVertices(const Vertices& vertices, BlendMode mode)
{
    AttachPaint();
    impl_->DrawVertices(vertices, mode);
}

void CoreCanvas::DrawBitmap(const Bitmap& bitmap, const scalar px, const scalar py)
{
    AttachPaint();
    impl_->DrawBitmap(bitmap, px, py);
}

void CoreCanvas::DrawImageNine(const Image* image, const RectI& center, const Rect& dst,
    FilterMode filter, const Brush* brush)
{
    impl_->DrawImageNine(image, center, dst, filter, brush);
}

void CoreCanvas::DrawAnnotation(const Rect& rect, const char* key, const Data* data)
{
    impl_->DrawAnnotation(rect, key, data);
}

void CoreCanvas::DrawImageLattice(const Image* image, const Lattice& lattice, const Rect& dst,
    FilterMode filter, const Brush* brush)
{
    impl_->DrawImageLattice(image, lattice, dst, filter, brush);
}

void CoreCanvas::DrawBitmap(Media::PixelMap& pixelMap, const scalar px, const scalar py)
{
    AttachPaint();
    impl_->DrawBitmap(pixelMap, px, py);
}

void CoreCanvas::DrawImage(const Image& image, const scalar px, const scalar py, const SamplingOptions& sampling)
{
    AttachPaint();
    impl_->DrawImage(image, px, py, sampling);
}

void CoreCanvas::DrawImageRect(
    const Image& image, const Rect& src, const Rect& dst, const SamplingOptions& sampling, SrcRectConstraint constraint)
{
    AttachPaint();
    impl_->DrawImageRect(image, src, dst, sampling, constraint);
}

void CoreCanvas::DrawImageRect(const Image& image, const Rect& dst, const SamplingOptions& sampling)
{
    AttachPaint();
    impl_->DrawImageRect(image, dst, sampling);
}

void CoreCanvas::DrawPicture(const Picture& picture)
{
    impl_->DrawPicture(picture);
}

void CoreCanvas::DrawSVGDOM(const sk_sp<SkSVGDOM>& svgDom)
{
    impl_->DrawSVGDOM(svgDom);
}

void CoreCanvas::DrawTextBlob(const TextBlob* blob, const scalar x, const scalar y)
{
    AttachPaint();
    impl_->DrawTextBlob(blob, x, y);
}

void CoreCanvas::DrawSymbol(const DrawingHMSymbolData& symbol, Point locate)
{
    AttachPaint();
    impl_->DrawSymbol(symbol, locate);
}

void CoreCanvas::ClipRect(const Rect& rect, ClipOp op, bool doAntiAlias)
{
    impl_->ClipRect(rect, op, doAntiAlias);
}

void CoreCanvas::ClipIRect(const RectI& rect, ClipOp op)
{
    impl_->ClipIRect(rect, op);
}

void CoreCanvas::ClipRoundRect(const RoundRect& roundRect, ClipOp op, bool doAntiAlias)
{
    impl_->ClipRoundRect(roundRect, op, doAntiAlias);
}

void CoreCanvas::ClipRoundRect(const Rect& rect, std::vector<Point>& pts, bool doAntiAlias)
{
    impl_->ClipRoundRect(rect, pts, doAntiAlias);
}

void CoreCanvas::ClipPath(const Path& path, ClipOp op, bool doAntiAlias)
{
    impl_->ClipPath(path, op, doAntiAlias);
}

void CoreCanvas::ClipRegion(const Region& region, ClipOp op)
{
    impl_->ClipRegion(region, op);
}

bool CoreCanvas::IsClipEmpty()
{
    return impl_->IsClipEmpty();
}

bool CoreCanvas::IsClipRect()
{
    return impl_->IsClipRect();
}

bool CoreCanvas::QuickReject(const Rect& rect)
{
    return impl_->QuickReject(rect);
}

void CoreCanvas::SetMatrix(const Matrix& matrix)
{
    impl_->SetMatrix(matrix);
}

void CoreCanvas::ResetMatrix()
{
    impl_->ResetMatrix();
}

void CoreCanvas::ConcatMatrix(const Matrix& matrix)
{
    impl_->ConcatMatrix(matrix);
}

void CoreCanvas::Translate(scalar dx, scalar dy)
{
    impl_->Translate(dx, dy);
}

void CoreCanvas::Scale(scalar sx, scalar sy)
{
    impl_->Scale(sx, sy);
}

void CoreCanvas::Rotate(scalar deg, scalar sx, scalar sy)
{
    impl_->Rotate(deg, sx, sy);
}

void CoreCanvas::Shear(scalar sx, scalar sy)
{
    impl_->Shear(sx, sy);
}

void CoreCanvas::Flush()
{
    impl_->Flush();
}

void CoreCanvas::Clear(ColorQuad color)
{
    impl_->Clear(color);
}

uint32_t CoreCanvas::Save()
{
    return impl_->Save();
}

void CoreCanvas::SaveLayer(const SaveLayerOps& saveLayerOps)
{
    impl_->SaveLayer(saveLayerOps);
}

void CoreCanvas::Restore()
{
    impl_->Restore();
}

uint32_t CoreCanvas::GetSaveCount() const
{
    return impl_->GetSaveCount();
}

void CoreCanvas::Discard()
{
    return impl_->Discard();
}

CoreCanvas& CoreCanvas::AttachPen(const Pen& pen)
{
    paintPen_.AttachPen(pen);
    return *this;
}

CoreCanvas& CoreCanvas::AttachBrush(const Brush& brush)
{
    paintBrush_.AttachBrush(brush);
    return *this;
}

CoreCanvas& CoreCanvas::AttachPaint(const Paint& paint)
{
    paintBrush_.Disable();
    paintPen_ = paint;
    return *this;
}

CoreCanvas& CoreCanvas::DetachPen()
{
    paintPen_.Disable();
    return *this;
}

CoreCanvas& CoreCanvas::DetachBrush()
{
    paintBrush_.Disable();
    return *this;
}

CoreCanvas& CoreCanvas::DetachPaint()
{
    paintPen_.Disable();
    return *this;
}

std::shared_ptr<CoreCanvasImpl> CoreCanvas::GetCanvasData() const
{
    return impl_;
}

ColorQuad CoreCanvas::GetEnvForegroundColor() const
{
    return Color::COLOR_BLACK;
}

bool CoreCanvas::isHighContrastEnabled() const
{
    return false;
}

Drawing::CacheType CoreCanvas::GetCacheType() const
{
    return Drawing::CacheType::UNDEFINED;
}

Drawing::Surface* CoreCanvas::GetSurface() const
{
    return nullptr;
}

float CoreCanvas::GetAlpha() const
{
    return 1.0f;
}

int CoreCanvas::GetAlphaSaveCount() const
{
    return 0;
}

void CoreCanvas::AttachPaint()
{
    bool brushValid = paintBrush_.IsValid();
    bool penValid = paintPen_.IsValid();
    if (!brushValid && !penValid) {
        LOGE("Drawing CoreCanvas AttachPaint with Invalid Paint");
        return;
    }

    if (brushValid && penValid && Paint::CanCombinePaint(paintBrush_, paintPen_)) {
        paintPen_.SetStyle(Paint::PaintStyle::PAINT_FILL_STROKE);
        impl_->AttachPaint(paintPen_);
        paintPen_.SetStyle(Paint::PaintStyle::PAINT_STROKE);
        return;
    }

    if (brushValid) {
        impl_->AttachPaint(paintBrush_);
    }

    if (penValid) {
        impl_->AttachPaint(paintPen_);
    }
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
