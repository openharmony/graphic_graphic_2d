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

#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_OVERDRAW_RS_CANVAS_LISTENER_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_OVERDRAW_RS_CANVAS_LISTENER_H

#include "draw/canvas.h"

namespace OHOS {
namespace Rosen {
class RSCanvasListener {
public:
    explicit RSCanvasListener(Drawing::Canvas &canvas) : canvas_(canvas) {}
    virtual ~RSCanvasListener() = default;
    virtual void Draw() {}
    virtual bool IsValid() const
    {
        return false;
    }

    virtual const char *Name() const
    {
        return "RSCanvasListener";
    }

    // shapes
    virtual void DrawPoint(const Drawing::Point& point) {};
    virtual void DrawLine(const Drawing::Point& startPt, const Drawing::Point& endPt) {};
    virtual void DrawRect(const Drawing::Rect& rect) {};
    virtual void DrawRoundRect(const Drawing::RoundRect& roundRect) {};
    virtual void DrawNestedRoundRect(const Drawing::RoundRect& outer, const Drawing::RoundRect& inner) {};
    virtual void DrawArc(const Drawing::Rect& oval, Drawing::scalar startAngle, Drawing::scalar sweepAngle) {};
    virtual void DrawPie(const Drawing::Rect& oval, Drawing::scalar startAngle, Drawing::scalar sweepAngle) {};
    virtual void DrawOval(const Drawing::Rect& oval) {};
    virtual void DrawCircle(const Drawing::Point& centerPt, Drawing::scalar radius) {};
    virtual void DrawPath(const Drawing::Path& path) {};
    virtual void DrawBackground(const Drawing::Brush& brush) {};
    virtual void DrawShadow(const Drawing::Path& path, const Drawing::Point3& planeParams,
        const Drawing::Point3& devLightPos, Drawing::scalar lightRadius,
        Drawing::Color ambientColor, Drawing::Color spotColor, Drawing::ShadowFlags flag) {};
    virtual void DrawShadowStyle(const Drawing::Path& path, const Drawing::Point3& planeParams,
        const Drawing::Point3& devLightPos, Drawing::scalar lightRadius,
        Drawing::Color ambientColor, Drawing::Color spotColor, Drawing::ShadowFlags flag, bool isShadowStyle) {};
    virtual void DrawRegion(const Drawing::Region& region) {};
    virtual void DrawTextBlob(const Drawing::TextBlob* blob, const Drawing::scalar x, const Drawing::scalar y) {};

    // image
    virtual void DrawBitmap(const Drawing::Bitmap& bitmap, const Drawing::scalar px, const Drawing::scalar py) {};
    virtual void DrawImage(const Drawing::Image& image,
        const Drawing::scalar px, const Drawing::scalar py, const Drawing::SamplingOptions& sampling) {};
    virtual void DrawImageRect(const Drawing::Image& image,
        const Drawing::Rect& src, const Drawing::Rect& dst, const Drawing::SamplingOptions& sampling,
        Drawing::SrcRectConstraint constraint = Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT) {};
    virtual void DrawImageRect(const Drawing::Image& image,
        const Drawing::Rect& dst, const Drawing::SamplingOptions& sampling) {};
    virtual void DrawPicture(const Drawing::Picture& picture) {};

    virtual void Clear(Drawing::ColorQuad color) {};

    // paint
    virtual void AttachPen(const Drawing::Pen& pen) {};
    virtual void AttachBrush(const Drawing::Brush& brush) {};
    virtual void DetachPen() {};
    virtual void DetachBrush() {};

protected:
    Drawing::Canvas &canvas_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_OVERDRAW_RS_CANVAS_LISTENER_H
