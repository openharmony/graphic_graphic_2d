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

#include "platform/ohos/overdraw/rs_listened_canvas.h"

#include "platform/common/rs_log.h"
#include "platform/ohos/overdraw/rs_canvas_listener.h"

namespace OHOS {
namespace Rosen {
RSListenedCanvas::RSListenedCanvas(Drawing::Canvas& canvas) : RSPaintFilterCanvas(&canvas) {}

RSListenedCanvas::RSListenedCanvas(Drawing::Surface& surface) : RSPaintFilterCanvas(&surface) {}

void RSListenedCanvas::SetListener(const std::shared_ptr<RSCanvasListener> &listener)
{
    listener_ = listener;
}

using namespace Drawing;

void RSListenedCanvas::DrawPoint(const Point& point)
{
    RSPaintFilterCanvas::DrawPoint(point);
    if (listener_ != nullptr) {
        listener_->DrawPoint(point);
    }
}

void RSListenedCanvas::DrawLine(const Point& startPt, const Point& endPt)
{
    RSPaintFilterCanvas::DrawLine(startPt, endPt);
    if (listener_ != nullptr) {
        listener_->DrawLine(startPt, endPt);
    }
}

void RSListenedCanvas::DrawRect(const Rect& rect)
{
    RSPaintFilterCanvas::DrawRect(rect);
    if (listener_ != nullptr) {
        listener_->DrawRect(rect);
    }
}

void RSListenedCanvas::DrawRoundRect(const RoundRect& roundRect)
{
    RSPaintFilterCanvas::DrawRoundRect(roundRect);
    if (listener_ != nullptr) {
        listener_->DrawRoundRect(roundRect);
    }
}

void RSListenedCanvas::DrawNestedRoundRect(const RoundRect& outer, const RoundRect& inner)
{
    RSPaintFilterCanvas::DrawNestedRoundRect(outer, inner);
    if (listener_ != nullptr) {
        listener_->DrawNestedRoundRect(outer, inner);
    }
}

void RSListenedCanvas::DrawArc(const Rect& oval, scalar startAngle, scalar sweepAngle)
{
    RSPaintFilterCanvas::DrawArc(oval, startAngle, sweepAngle);
    if (listener_ != nullptr) {
        listener_->DrawArc(oval, startAngle, sweepAngle);
    }
}

void RSListenedCanvas::DrawPie(const Rect& oval, scalar startAngle, scalar sweepAngle)
{
    RSPaintFilterCanvas::DrawPie(oval, startAngle, sweepAngle);
    if (listener_ != nullptr) {
        listener_->DrawPie(oval, startAngle, sweepAngle);
    }
}

void RSListenedCanvas::DrawOval(const Rect& oval)
{
    RSPaintFilterCanvas::DrawOval(oval);
    if (listener_ != nullptr) {
        listener_->DrawOval(oval);
    }
}

void RSListenedCanvas::DrawCircle(const Point& centerPt, scalar radius)
{
    RSPaintFilterCanvas::DrawCircle(centerPt, radius);
    if (listener_ != nullptr) {
        listener_->DrawCircle(centerPt, radius);
    }
}

void RSListenedCanvas::DrawPath(const Path& path)
{
    RSPaintFilterCanvas::DrawPath(path);
    if (listener_ != nullptr) {
        listener_->DrawPath(path);
    }
}

void RSListenedCanvas::DrawBackground(const Brush& brush)
{
    RSPaintFilterCanvas::DrawBackground(brush);
    if (listener_ != nullptr) {
        listener_->DrawBackground(brush);
    }
}

void RSListenedCanvas::DrawShadow(const Path& path, const Point3& planeParams, const Point3& devLightPos,
    scalar lightRadius, Color ambientColor, Color spotColor, ShadowFlags flag)
{
    RSPaintFilterCanvas::DrawShadow(path, planeParams, devLightPos, lightRadius, ambientColor, spotColor, flag);
    if (listener_ != nullptr) {
        listener_->DrawShadow(path, planeParams, devLightPos, lightRadius, ambientColor, spotColor, flag);
    }
}

void RSListenedCanvas::DrawShadowStyle(const Path& path, const Point3& planeParams, const Point3& devLightPos,
    scalar lightRadius, Color ambientColor, Color spotColor, ShadowFlags flag, bool isShadowStyle)
{
    RSPaintFilterCanvas::DrawShadowStyle(
        path, planeParams, devLightPos, lightRadius, ambientColor, spotColor, flag, isShadowStyle);
    if (listener_ != nullptr) {
        listener_->DrawShadowStyle(
            path, planeParams, devLightPos, lightRadius, ambientColor, spotColor, flag, isShadowStyle);
    }
}

void RSListenedCanvas::DrawRegion(const Drawing::Region& region)
{
    RSPaintFilterCanvas::DrawRegion(region);
    if (listener_ != nullptr) {
        listener_->DrawRegion(region);
    }
}

void RSListenedCanvas::DrawTextBlob(const Drawing::TextBlob* blob, const Drawing::scalar x, const Drawing::scalar y)
{
    RSPaintFilterCanvas::DrawTextBlob(blob, x, y);
    if (listener_ != nullptr) {
        listener_->DrawTextBlob(blob, x, y);
    }
}

void RSListenedCanvas::DrawBitmap(const Bitmap& bitmap, const scalar px, const scalar py)
{
    RSPaintFilterCanvas::DrawBitmap(bitmap, px, py);
    if (listener_ != nullptr) {
        listener_->DrawBitmap(bitmap, px, py);
    }
}

void RSListenedCanvas::DrawImage(const Image& image, const scalar px, const scalar py, const SamplingOptions& sampling)
{
    RSPaintFilterCanvas::DrawImage(image, px, py, sampling);
    if (listener_ != nullptr) {
        listener_->DrawImage(image, px, py, sampling);
    }
}

void RSListenedCanvas::DrawImageRect(const Image& image, const Rect& src, const Rect& dst,
    const SamplingOptions& sampling, SrcRectConstraint constraint)
{
    RSPaintFilterCanvas::DrawImageRect(image, src, dst, sampling, constraint);
    if (listener_ != nullptr) {
        listener_->DrawImageRect(image, src, dst, sampling, constraint);
    }
}

void RSListenedCanvas::DrawImageRect(const Image& image, const Rect& dst, const SamplingOptions& sampling)
{
    RSPaintFilterCanvas::DrawImageRect(image, dst, sampling);
    if (listener_ != nullptr) {
        listener_->DrawImageRect(image, dst, sampling);
    }
}

void RSListenedCanvas::DrawPicture(const Picture& picture)
{
    RSPaintFilterCanvas::DrawPicture(picture);
    if (listener_ != nullptr) {
        listener_->DrawPicture(picture);
    }
}

void RSListenedCanvas::Clear(Drawing::ColorQuad color)
{
    RSPaintFilterCanvas::Clear(color);
    if (listener_ != nullptr) {
        listener_->Clear(color);
    }
}

CoreCanvas& RSListenedCanvas::AttachPen(const Drawing::Pen& pen)
{
    RSPaintFilterCanvas::AttachPen(pen);
    if (listener_ != nullptr) {
        listener_->AttachPen(pen);
    }

    return *this;
}

CoreCanvas& RSListenedCanvas::AttachBrush(const Drawing::Brush& brush)
{
    RSPaintFilterCanvas::AttachBrush(brush);
    if (listener_ != nullptr) {
        listener_->AttachBrush(brush);
    }

    return *this;
}

CoreCanvas& RSListenedCanvas::DetachPen()
{
    RSPaintFilterCanvas::DetachPen();
    if (listener_ != nullptr) {
        listener_->DetachPen();
    }

    return *this;
}

CoreCanvas& RSListenedCanvas::DetachBrush()
{
    RSPaintFilterCanvas::DetachBrush();
    if (listener_ != nullptr) {
        listener_->DetachBrush();
    }

    return *this;
}
} // namespace Rosen
} // namespace OHOS
