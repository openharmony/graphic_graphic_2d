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

#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_OVERDRAW_RS_CPU_OVERDRAW_CANVAS_LISTENER_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_OVERDRAW_RS_CPU_OVERDRAW_CANVAS_LISTENER_H

#include "rs_canvas_listener.h"

#include <map>

#include "utils/region.h"

#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSCPUOverdrawCanvasListener : public RSCanvasListener {
public:
    explicit RSCPUOverdrawCanvasListener(Drawing::Canvas &canvas);

    void Draw() override;
    bool IsValid() const override { return true; }
    const char *Name() const override { return "RSCPUOverdrawCanvasListener"; }

    // shapes
    void DrawPoint(const Drawing::Point& point) override;
    void DrawLine(const Drawing::Point& startPt, const Drawing::Point& endPt) override;
    void DrawRect(const Drawing::Rect& rect) override;
    void DrawRoundRect(const Drawing::RoundRect& roundRect) override;
    void DrawNestedRoundRect(const Drawing::RoundRect& outer, const Drawing::RoundRect& inner) override;
    void DrawArc(const Drawing::Rect& oval, Drawing::scalar startAngle, Drawing::scalar sweepAngle) override;
    void DrawPie(const Drawing::Rect& oval, Drawing::scalar startAngle, Drawing::scalar sweepAngle) override;
    void DrawOval(const Drawing::Rect& oval) override;
    void DrawCircle(const Drawing::Point& centerPt, Drawing::scalar radius) override;
    void DrawPath(const Drawing::Path& path) override;
    void DrawBackground(const Drawing::Brush& brush) override;
    void DrawShadow(const Drawing::Path& path, const Drawing::Point3& planeParams,
        const Drawing::Point3& devLightPos, Drawing::scalar lightRadius,
        Drawing::Color ambientColor, Drawing::Color spotColor, Drawing::ShadowFlags flag) override;
    void DrawRegion(const Drawing::Region& region) override;
    void DrawTextBlob(const Drawing::TextBlob* blob, const Drawing::scalar x, const Drawing::scalar y) override;

    // image
    void DrawBitmap(const Drawing::Bitmap& bitmap, const Drawing::scalar px, const Drawing::scalar py) override;
    void DrawImage(const Drawing::Image& image,
        const Drawing::scalar px, const Drawing::scalar py, const Drawing::SamplingOptions& sampling) override;
    void DrawImageRect(const Drawing::Image& image,
        const Drawing::Rect& src, const Drawing::Rect& dst, const Drawing::SamplingOptions& sampling,
        Drawing::SrcRectConstraint constraint = Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT) override;
    void DrawImageRect(const Drawing::Image& image,
        const Drawing::Rect& dst, const Drawing::SamplingOptions& sampling) override;
    void DrawPicture(const Drawing::Picture& picture) override;

    void Clear(Drawing::ColorQuad color) override;

    // paint
    void AttachPen(const Drawing::Pen& pen) override {};
    void AttachBrush(const Drawing::Brush& brush) override {};
    void DetachPen() override {};
    void DetachBrush() override {};
private:
    void AppendRegion(Drawing::Path& path);
    std::map<int, Drawing::Region> regions_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_OVERDRAW_RS_CPU_OVERDRAW_CANVAS_LISTENER_H
