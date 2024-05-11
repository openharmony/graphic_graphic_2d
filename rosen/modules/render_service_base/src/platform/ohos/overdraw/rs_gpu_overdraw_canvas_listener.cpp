/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "platform/ohos/overdraw/rs_gpu_overdraw_canvas_listener.h"

#include "draw/blend_mode.h"
#include "draw/color.h"
#include "effect/color_matrix.h"
#include "image/bitmap.h"

#include "platform/ohos/overdraw/rs_overdraw_controller.h"

namespace OHOS {
namespace Rosen {
RSGPUOverdrawCanvasListener::RSGPUOverdrawCanvasListener(Drawing::Canvas& canvas)
    : RSCanvasListener(canvas)
{
    auto gpuContext = canvas.GetGPUContext();
    if (gpuContext == nullptr) {
        ROSEN_LOGE("RSGPUOverdrawCanvasListener, construction failed: need gpu canvas");
        return;
    }

    Drawing::BitmapFormat format = { Drawing::ColorType::COLORTYPE_BGRA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL };
    auto width = canvas.GetWidth();
    auto height = canvas.GetHeight();
    Drawing::Bitmap bitmap;
    bitmap.Build(width, height, format);
    Drawing::Image image;
    image.BuildFromBitmap(*gpuContext, bitmap);

    auto surface = std::make_shared<Drawing::Surface>();
    surface->Bind(image);
    listenedSurface_ = surface;
    overdrawCanvas_ = std::make_shared<Drawing::OverDrawCanvas>(surface->GetCanvas());
}

RSGPUOverdrawCanvasListener::~RSGPUOverdrawCanvasListener()
{
}

void RSGPUOverdrawCanvasListener::Draw()
{
    auto image = listenedSurface_->GetImageSnapshot();
    if (image == nullptr) {
        ROSEN_LOGE("image is nullptr");
        return;
    }
    Drawing::Brush brush;
    auto overdrawColors = RSOverdrawController::GetInstance().GetColorArray();
    auto colorFilter = Drawing::ColorFilter::CreateOverDrawColorFilter(overdrawColors.data());
    Drawing::Filter filter;
    filter.SetColorFilter(colorFilter);
    brush.SetFilter(filter);
    canvas_.AttachBrush(brush);
    canvas_.DrawImage(*image, 0, 0, Drawing::SamplingOptions());
    canvas_.DetachBrush();
}

bool RSGPUOverdrawCanvasListener::IsValid() const
{
    return listenedSurface_ != nullptr;
}

void RSGPUOverdrawCanvasListener::DrawPoint(const Drawing::Point& point)
{
    DrawRect(Drawing::Rect(point.GetX(), point.GetY(), 1 + point.GetX(), 1 + point.GetY()));
}

void RSGPUOverdrawCanvasListener::DrawLine(const Drawing::Point& startPt, const Drawing::Point& endPt)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }

    Drawing::Path path;
    path.MoveTo(startPt.GetX(), startPt.GetY());
    path.LineTo(endPt.GetX(), endPt.GetY());
    overdrawCanvas_->DrawPath(path);
}

void RSGPUOverdrawCanvasListener::DrawRect(const Drawing::Rect& rect)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }
    overdrawCanvas_->DrawRect(rect);
}

void RSGPUOverdrawCanvasListener::DrawRoundRect(const Drawing::RoundRect& roundRect)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }
    overdrawCanvas_->DrawRoundRect(roundRect);
}

void RSGPUOverdrawCanvasListener::DrawNestedRoundRect(const Drawing::RoundRect& outer, const Drawing::RoundRect& inner)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }
    overdrawCanvas_->DrawNestedRoundRect(outer, inner);
}

void RSGPUOverdrawCanvasListener::DrawArc(
    const Drawing::Rect& oval, Drawing::scalar startAngle, Drawing::scalar sweepAngle)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }
    Drawing::Path path;
    path.AddArc(oval, startAngle, sweepAngle);
    overdrawCanvas_->DrawPath(path);
}

void RSGPUOverdrawCanvasListener::DrawPie(
    const Drawing::Rect& oval, Drawing::scalar startAngle, Drawing::scalar sweepAngle)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }
    Drawing::Path path;
    path.AddArc(oval, startAngle, sweepAngle);
    overdrawCanvas_->DrawPath(path);
}

void RSGPUOverdrawCanvasListener::DrawOval(const Drawing::Rect& oval)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }
    Drawing::Path path;
    path.AddOval(oval);
    overdrawCanvas_->DrawPath(path);
}

void RSGPUOverdrawCanvasListener::DrawCircle(const Drawing::Point& centerPt, Drawing::scalar radius)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }
    Drawing::Path path;
    path.AddCircle(centerPt.GetX(), centerPt.GetY(), radius);
    overdrawCanvas_->DrawPath(path);
}

void RSGPUOverdrawCanvasListener::DrawPath(const Drawing::Path& path)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }
    overdrawCanvas_->DrawPath(path);
}

void RSGPUOverdrawCanvasListener::DrawBackground(const Drawing::Brush& brush)
{
    // need know canvas rect region
}

void RSGPUOverdrawCanvasListener::DrawShadow(const Drawing::Path& path, const Drawing::Point3& planeParams,
    const Drawing::Point3& devLightPos, Drawing::scalar lightRadius, Drawing::Color ambientColor,
    Drawing::Color spotColor, Drawing::ShadowFlags flag)
{
    // need know shadow rect region
}

void RSGPUOverdrawCanvasListener::DrawShadowStyle(const Drawing::Path& path, const Drawing::Point3& planeParams,
    const Drawing::Point3& devLightPos, Drawing::scalar lightRadius, Drawing::Color ambientColor,
    Drawing::Color spotColor, Drawing::ShadowFlags flag, bool isLimitElevation)
{
    // need know shadow rect region
}

void RSGPUOverdrawCanvasListener::DrawRegion(const Drawing::Region& region)
{
    // need know region path region
}

void RSGPUOverdrawCanvasListener::DrawBitmap(
    const Drawing::Bitmap& bitmap, const Drawing::scalar px, const Drawing::scalar py)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }
    overdrawCanvas_->DrawRect(Drawing::Rect(px, py, bitmap.GetWidth() + px, bitmap.GetHeight() + py));
}

void RSGPUOverdrawCanvasListener::DrawImage(const Drawing::Image& image, const Drawing::scalar px,
    const Drawing::scalar py, const Drawing::SamplingOptions& sampling)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }
    overdrawCanvas_->DrawRect(Drawing::Rect(px, py, image.GetWidth() + px, image.GetHeight() + py));
}

void RSGPUOverdrawCanvasListener::DrawImageRect(const Drawing::Image& image, const Drawing::Rect& src,
    const Drawing::Rect& dst, const Drawing::SamplingOptions& sampling, Drawing::SrcRectConstraint constraint)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }
    overdrawCanvas_->DrawRect(dst);
}

void RSGPUOverdrawCanvasListener::DrawImageRect(
    const Drawing::Image& image, const Drawing::Rect& dst, const Drawing::SamplingOptions& sampling)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }
    overdrawCanvas_->DrawRect(dst);
}

void RSGPUOverdrawCanvasListener::DrawPicture(const Drawing::Picture& picture)
{
    // need know picture rect region
}

void RSGPUOverdrawCanvasListener::Clear(Drawing::ColorQuad color)
{
    // need know canvas rect region
}

void RSGPUOverdrawCanvasListener::AttachPen(const Drawing::Pen& pen)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("RSGPUOverdrawCanvasListener::AttachPen overdrawCanvas_ is nullptr");
        return;
    }
    overdrawCanvas_->AttachPen(pen);
}

void RSGPUOverdrawCanvasListener::AttachBrush(const Drawing::Brush& brush)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("RSGPUOverdrawCanvasListener::AttachBrush overdrawCanvas_ is nullptr");
        return;
    }
    overdrawCanvas_->AttachBrush(brush);
}

void RSGPUOverdrawCanvasListener::DetachPen()
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("RSGPUOverdrawCanvasListener::DetachPen overdrawCanvas_ is nullptr");
        return;
    }
    overdrawCanvas_->DetachPen();
}

void RSGPUOverdrawCanvasListener::DetachBrush()
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("RSGPUOverdrawCanvasListener::DetachBrush overdrawCanvas_ is nullptr");
        return;
    }
    overdrawCanvas_->DetachBrush();
}

} // namespace Rosen
} // namespace OHOS
