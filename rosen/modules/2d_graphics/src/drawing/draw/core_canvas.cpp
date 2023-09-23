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
std::shared_ptr<GPUContext> CoreCanvas::GetGPUContext() const
{
    return impl_->GetGPUContext();
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

void CoreCanvas::DrawPoint(const Point& point)
{
    impl_->DrawPoint(point);
}

void CoreCanvas::DrawPoints(PointMode mode, size_t count, const Point pts[])
{
    impl_->DrawPoints(mode, count, pts);
}

void CoreCanvas::DrawLine(const Point& startPt, const Point& endPt)
{
    impl_->DrawLine(startPt, endPt);
}

void CoreCanvas::DrawRect(const Rect& rect)
{
    impl_->DrawRect(rect);
}

void CoreCanvas::DrawRoundRect(const RoundRect& roundRect)
{
    impl_->DrawRoundRect(roundRect);
}

void CoreCanvas::DrawNestedRoundRect(const RoundRect& outer, const RoundRect& inner)
{
    impl_->DrawNestedRoundRect(outer, inner);
}

void CoreCanvas::DrawArc(const Rect& oval, scalar startAngle, scalar sweepAngle)
{
    impl_->DrawArc(oval, startAngle, sweepAngle);
}

void CoreCanvas::DrawPie(const Rect& oval, scalar startAngle, scalar sweepAngle)
{
    impl_->DrawPie(oval, startAngle, sweepAngle);
}

void CoreCanvas::DrawOval(const Rect& oval)
{
    impl_->DrawOval(oval);
}

void CoreCanvas::DrawCircle(const Point& centerPt, scalar radius)
{
    impl_->DrawCircle(centerPt, radius);
}

void CoreCanvas::DrawPath(const Path& path)
{
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
    impl_->DrawRegion(region);
}

void CoreCanvas::DrawPatch(const Point cubics[12], const ColorQuad colors[4], const Point texCoords[4], BlendMode mode)
{
    impl_->DrawPatch(cubics, colors, texCoords, mode);
}

void CoreCanvas::DrawEdgeAAQuad(const Rect& rect, const Point clip[4],
    QuadAAFlags aaFlags, ColorQuad color, BlendMode mode)
{
    impl_->DrawEdgeAAQuad(rect, clip, aaFlags, color, mode);
}

void CoreCanvas::DrawVertices(const Vertices& vertices, BlendMode mode)
{
    impl_->DrawVertices(vertices, mode);
}

void CoreCanvas::DrawBitmap(const Bitmap& bitmap, const scalar px, const scalar py)
{
    impl_->DrawBitmap(bitmap, px, py);
}

void CoreCanvas::DrawImageNine(const Image* image, const RectI& center, const Rect& dst,
    FilterMode filter, const Brush* brush)
{
    impl_->DrawImageNine(image, center, dst, filter, brush);
}

void CoreCanvas::DrawAnnotation(const Rect& rect, const char* key, const Data& data)
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
    impl_->DrawBitmap(pixelMap, px, py);
}

void CoreCanvas::DrawImage(const Image& image, const scalar px, const scalar py, const SamplingOptions& sampling)
{
    impl_->DrawImage(image, px, py, sampling);
}

void CoreCanvas::DrawImageRect(
    const Image& image, const Rect& src, const Rect& dst, const SamplingOptions& sampling, SrcRectConstraint constraint)
{
    impl_->DrawImageRect(image, src, dst, sampling, constraint);
}

void CoreCanvas::DrawImageRect(const Image& image, const Rect& dst, const SamplingOptions& sampling)
{
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

void CoreCanvas::Save()
{
    impl_->Save();
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
    impl_->AttachPen(pen);
    return *this;
}

CoreCanvas& CoreCanvas::AttachBrush(const Brush& brush)
{
    impl_->AttachBrush(brush);
    return *this;
}

CoreCanvas& CoreCanvas::DetachPen()
{
    impl_->DetachPen();
    return *this;
}

CoreCanvas& CoreCanvas::DetachBrush()
{
    impl_->DetachBrush();
    return *this;
}

std::shared_ptr<CoreCanvasImpl> CoreCanvas::GetCanvasData() const
{
    return impl_;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
