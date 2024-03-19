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

#include "platform/ohos/overdraw/rs_cpu_overdraw_canvas_listener.h"

#include "utils/region.h"

#include "platform/ohos/overdraw/rs_overdraw_controller.h"

namespace OHOS {
namespace Rosen {
RSCPUOverdrawCanvasListener::RSCPUOverdrawCanvasListener(Drawing::Canvas &canvas)
    : RSCanvasListener(canvas)
{
}

void RSCPUOverdrawCanvasListener::Draw()
{
    auto overdrawColorMap = RSOverdrawController::GetInstance().GetColorMap();

    Drawing::Brush brush;
    brush.SetAntiAlias(true);

    Drawing::Region drawed;
    for (size_t i = regions_.size(); i > 0; i--) {
        if (overdrawColorMap.find(i) != overdrawColorMap.end()) {
            brush.SetColor(overdrawColorMap.at(i));
        } else {
            brush.SetColor(overdrawColorMap.at(0));
        }

        auto toDraw = regions_[i];
        toDraw.Op(drawed, Drawing::RegionOp::DIFFERENCE);
        canvas_.AttachBrush(brush);
        canvas_.DrawRegion(toDraw);
        canvas_.DetachBrush();
        drawed.Op(toDraw, Drawing::RegionOp::UNION);
    }
}

void RSCPUOverdrawCanvasListener::DrawPoint(const Drawing::Point& point)
{
    DrawRect(Drawing::Rect(point.GetX(), point.GetY(), 1 + point.GetX(), 1 + point.GetY()));
}

void RSCPUOverdrawCanvasListener::DrawLine(const Drawing::Point& startPt, const Drawing::Point& endPt)
{
    Drawing::Path path;
    path.MoveTo(startPt.GetX(), startPt.GetY());
    path.LineTo(endPt.GetX(), endPt.GetY());
    AppendRegion(path);
}

void RSCPUOverdrawCanvasListener::DrawRect(const Drawing::Rect& rect)
{
    Drawing::Path path;
    path.AddRect(rect);
    AppendRegion(path);
}

void RSCPUOverdrawCanvasListener::DrawRoundRect(const Drawing::RoundRect& roundRect)
{
    Drawing::Path path;
    path.AddRoundRect(roundRect);
    AppendRegion(path);
}

void RSCPUOverdrawCanvasListener::DrawNestedRoundRect(const Drawing::RoundRect& outer, const Drawing::RoundRect& inner)
{
    Drawing::Path path;
    path.AddRoundRect(outer);
    path.AddRoundRect(inner);
    AppendRegion(path);
}

void RSCPUOverdrawCanvasListener::DrawArc(
    const Drawing::Rect& oval, Drawing::scalar startAngle, Drawing::scalar sweepAngle)
{
    Drawing::Path path;
    path.AddArc(oval, startAngle, sweepAngle);
    AppendRegion(path);
}

void RSCPUOverdrawCanvasListener::DrawPie(
    const Drawing::Rect& oval, Drawing::scalar startAngle, Drawing::scalar sweepAngle)
{
    Drawing::Path path;
    path.AddArc(oval, startAngle, sweepAngle);
    AppendRegion(path);
}

void RSCPUOverdrawCanvasListener::DrawOval(const Drawing::Rect& oval)
{
    Drawing::Path path;
    path.AddOval(oval);
    AppendRegion(path);
}

void RSCPUOverdrawCanvasListener::DrawCircle(const Drawing::Point& centerPt, Drawing::scalar radius)
{
    Drawing::Path path;
    path.AddCircle(centerPt.GetX(), centerPt.GetY(), radius);
    AppendRegion(path);
}

void RSCPUOverdrawCanvasListener::DrawPath(const Drawing::Path& path)
{
    Drawing::Path tPath = path;
    AppendRegion(tPath);
}

void RSCPUOverdrawCanvasListener::DrawBackground(const Drawing::Brush& brush)
{
    // need know canvas rect region
}

void RSCPUOverdrawCanvasListener::DrawShadow(const Drawing::Path& path, const Drawing::Point3& planeParams,
    const Drawing::Point3& devLightPos, Drawing::scalar lightRadius, Drawing::Color ambientColor,
    Drawing::Color spotColor, Drawing::ShadowFlags flag)
{
    // need know shadow rect region
}

void RSCPUOverdrawCanvasListener::DrawRegion(const Drawing::Region& region)
{
    // need know region path region
}

void RSCPUOverdrawCanvasListener::DrawTextBlob(
    const Drawing::TextBlob* blob, const Drawing::scalar x, const Drawing::scalar y)
{
    if (blob == nullptr) {
        return;
    }

    auto rect = blob->Bounds();
    rect->Offset(x, y);
    DrawRect(*rect);
}

void RSCPUOverdrawCanvasListener::DrawBitmap(
    const Drawing::Bitmap& bitmap, const Drawing::scalar px, const Drawing::scalar py)
{
    DrawRect(Drawing::Rect(px, py, bitmap.GetWidth() + px, bitmap.GetHeight() + py));
}

void RSCPUOverdrawCanvasListener::DrawImage(const Drawing::Image& image, const Drawing::scalar px,
    const Drawing::scalar py, const Drawing::SamplingOptions& sampling)
{
    DrawRect(Drawing::Rect(px, py, image.GetWidth() + px, image.GetHeight() + py));
}

void RSCPUOverdrawCanvasListener::DrawImageRect(const Drawing::Image& image, const Drawing::Rect& src,
    const Drawing::Rect& dst, const Drawing::SamplingOptions& sampling, Drawing::SrcRectConstraint constraint)
{
    DrawRect(dst);
}

void RSCPUOverdrawCanvasListener::DrawImageRect(
    const Drawing::Image& image, const Drawing::Rect& dst, const Drawing::SamplingOptions& sampling)
{
    DrawRect(dst);
}

void RSCPUOverdrawCanvasListener::DrawPicture(const Drawing::Picture& picture)
{
    // need know picture rect region
}

void RSCPUOverdrawCanvasListener::Clear(Drawing::ColorQuad color)
{
    // need know canvas rect region
}

void RSCPUOverdrawCanvasListener::AppendRegion(Drawing::Path& path)
{
    Drawing::Region target;
    Drawing::Region drRegion;
    path.Transform(canvas_.GetTotalMatrix());
    drRegion.SetRect(Drawing::RectI(-1e9, -1e9, 1e9, 1e9));
    target.SetPath(path, drRegion);

    for (size_t i = regions_.size(); i > 0; i--) {
        if (regions_[i].IsIntersects(target)) {
            auto regi = regions_[i];
            regi.Op(target, Drawing::RegionOp::INTERSECT);
            regions_[i + 1].Op(regi, Drawing::RegionOp::UNION);
        }
    }
    regions_[1].Op(target, Drawing::RegionOp::UNION);
}
} // namespace Rosen
} // namespace OHOS