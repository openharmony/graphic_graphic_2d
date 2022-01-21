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

#include "core_canvas.h"

#include <iostream>
#include <string>

#include "impl_factory.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
CoreCanvas::CoreCanvas() : impl_(ImplFactory::CreateCoreCanvasImpl()) {}

void CoreCanvas::Bind(const Bitmap& bitmap)
{
    impl_->Bind(bitmap);
}

void CoreCanvas::DrawPoint(const Point& point)
{
    impl_->DrawPoint(point);
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

void CoreCanvas::DrawBitmap(const Bitmap& bitmap, const scalar px, const scalar py)
{
    impl_->DrawBitmap(bitmap, px, py);
}

void CoreCanvas::DrawBitmap(Media::PixelMap& pixelMap, const scalar px, const scalar py)
{
    impl_->DrawBitmap(pixelMap, px, py);
}

void CoreCanvas::DrawImage(const Image& image, const scalar px, const scalar py, const SamplingOptions& sampling)
{
    impl_->DrawImage(image, px, py, sampling);
}

void CoreCanvas::DrawText(const Text& text)
{
    impl_->DrawText(text);
}

void CoreCanvas::ClipRect(const Rect& rect, ClipOp op)
{
    impl_->ClipRect(rect, op);
}

void CoreCanvas::ClipRoundRect(const RoundRect& roundRect, ClipOp op)
{
    impl_->ClipRoundRect(roundRect, op);
}

void CoreCanvas::ClipPath(const Path& path, ClipOp op)
{
    impl_->ClipPath(path, op);
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

void CoreCanvas::Rotate(scalar deg)
{
    impl_->Rotate(deg);
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

void CoreCanvas::SaveLayer(const Rect& rect, const Brush& brush)
{
    impl_->SaveLayer(rect, brush);
}

void CoreCanvas::Restore()
{
    impl_->Restore();
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
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
